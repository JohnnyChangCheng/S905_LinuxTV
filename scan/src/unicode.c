
#include <assert.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined(_WIN32)
#  include <io.h>
#endif
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <math.h>
#include <wctype.h>
#include <string.h>

#include "noovo_iconv.h"

#define likely(expr) (expr)
#define unlikely(expr) (expr)



size_t towc (const char *str, uint32_t *restrict pwc)
{
    uint8_t *ptr = (uint8_t *)str, c;
    uint32_t cp;

    assert (str != NULL);

    c = *ptr;
    if (unlikely(c > 0xF4))
        return -1;

    int charlen = clz8 (c ^ 0xFF);
    switch (charlen)
    {
        case 0: // 7-bit ASCII character -> short cut
            *pwc = c;
            return c != '\0';

        case 1: // continuation byte -> error
            return -1;

        case 2:
            if (unlikely(c < 0xC2)) // ASCII overlong
                return -1;
            cp = (c & 0x1F) << 6;
            break;

        case 3:
            cp = (c & 0x0F) << 12;
            break;

        case 4:
            cp = (c & 0x07) << 18;
            break;

        //default:
            //vlc_assert_unreachable ();
    }

    /* Unrolled continuation bytes decoding */
    switch (charlen)
    {
        case 4:
            c = *++ptr;
            if (unlikely((c & 0xC0) != 0x80)) // not a continuation byte
                return -1;
            cp |= (c & 0x3F) << 12;

            if (unlikely(cp >= 0x110000)) // beyond Unicode range
                return -1;
            /* fall through */
        case 3:
            c = *++ptr;
            if (unlikely((c & 0xC0) != 0x80)) // not a continuation byte
                return -1;
            cp |= (c & 0x3F) << 6;

            if (unlikely(cp >= 0xD800 && cp < 0xE000)) // UTF-16 surrogate
                return -1;
            if (unlikely(cp < (1u << (5 * charlen - 4)))) // non-ASCII overlong
                return -1;
            /* fall through */
        case 2:
            c = *++ptr;
            if (unlikely((c & 0xC0) != 0x80)) // not a continuation byte
                return -1;
            cp |= (c & 0x3F);
            break;
    }

    *pwc = cp;
    return charlen;
}


/**
 * Checks UTF-8 validity.
 *
 * Checks whether a null-terminated string is a valid UTF-8 bytes sequence.
 *
 * \param str string to check
 *
 * \retval str the string is a valid null-terminated UTF-8 sequence
 * \retval NULL the string is not an UTF-8 sequence
 */
static inline const char *IsUTF8(const char *str)
{
    size_t n;
    uint32_t cp;

    while ((n = towc(str, &cp)) != 0)
        if (likely(n != (size_t)-1))
            str += n;
        else
            return NULL;
    return str;
}

/**
 * Removes non-UTF-8 sequences.
 *
 * Replaces invalid or <i>over-long</i> UTF-8 bytes sequences within a
 * null-terminated string with question marks. This is so that the string can
 * be printed at least partially.
 *
 * \warning Do not use this were correctness is critical. use IsUTF8() and
 * handle the error case instead. This function is mainly for display or debug.
 *
 * \note Converting from Latin-1 to UTF-8 in place is not possible (the string
 * size would be increased). So it is not attempted even if it would otherwise
 * be less disruptive.
 *
 * \retval str the string is a valid null-terminated UTF-8 sequence
 *             (i.e. no changes were made)
 * \retval NULL the string is not an UTF-8 sequence
 */
static inline char *EnsureUTF8(char *str)
{
    char *ret = str;
    size_t n;
    uint32_t cp;

    while ((n = towc(str, &cp)) != 0)
        if (likely(n != (size_t)-1))
            str += n;
        else
        {
            *str++ = '?';
            ret = NULL;
        }
    return ret;
}

char * Descriptor(const void *buf, size_t length)
{
    if (unlikely(length == 0))
        return NULL;

    char encbuf[12];
    const char *encoding = encbuf;

    const char *in = buf;
    size_t offset = 1;
    unsigned char c = *in;

    if (c >= 0x20)
    {
        offset = 0;
        encoding = "ISO_6937";
    }
    else if ((1 << c) & 0x0EFE) /* 1-7, 9-11 -> ISO 8859-(c+4) */
    {
        snprintf (encbuf, sizeof (encbuf), "ISO_8859-%u", 4u + c);
    }
    else switch (c)
    {
        case 0x10: /* two more bytes */
            offset = 3;
            if (length < 3 || in[1] != 0x00)
                return NULL;

            c = in[2];
            if ((1 << c) & 0xEFFE) /* 1-11, 13-15 -> ISO 8859-(c) */
               snprintf (encbuf, sizeof (encbuf), "ISO_8859-%hhu", c);
           else
               return NULL;
           break;
        case 0x11: /* the BMP */
        case 0x14: /* Big5 subset of the BMP */
            encoding = "UCS-2BE";
            break;
        case 0x12:
            /* DVB has no clue about Korean. KS X 1001 (a.k.a. KS C 5601) is a
             * character set, not a character encoding... So we assume EUC-KR.
             * It is an encoding of KS X 1001. In practice, I guess nobody uses
             * this in any real DVB system. */
            encoding = "EUC-KR";
            break;
        case 0x13: /* GB-2312-1980 */
            encoding = "GB2312";
            break;
        case 0x15:
            encoding = "UTF-8";
            break;
#if 0
        case 0x1F: /* operator-specific(?) */
            offset = 2;
#endif
        default:
            return NULL;
    }

    in += offset;
    length -= offset;

    char *out = FromCharset (encoding, in, length);
    if (out == NULL)
    {   /* Fallback... */
        out = strndup (in, length);
        if (unlikely(out == NULL))
            return NULL;
        EnsureUTF8 (out);
    }

    length = strlen(out);
    /* Convert control codes */
    char *p;
    for (p = strchr (out, '\xC2'); p; p = strchr (p + 1, '\xC2'))
    {
        /* We have valid UTF-8, to 0xC2 is followed by a continuation byte. */
        /* 0x80-0x85,0x88-0x89 are reserved.
         * 0x86-0x87 are identical to Unicode and Latin-1.
         * 0x8A is CR/LF.
         * 0x8B-0x9F are unspecified. */
        if (p[1] == '\x8A')
            memcpy (p, "\r\n", 2);

        /* Strip character emphasis */
        if (p[1] == '\x86' || p[1] == '\x87') {
            const size_t n = p - out;
            memmove (p, p+2, length - n);
            length -= 2;
            out[length] = '\0';
            if (length == n)
                break;
        }
    }

    /* Private use area */
    
    for (p = strchr (out, '\xEE'); p; p = strchr (p + 1, '\xEE'))
    {
        /* Within UTF-8, 0xEE is followed by a two continuation bytes. */
        if (p[1] != '\x82')
            continue;
        if (p[2] == '\x8A')
            memcpy (p, "\r\r\n", 3); /* we need three bytes, so to CRs ;) */

        /* Strip character emphasis */
        if (p[2] == '\x86' || p[2] == '\x87') {
            const size_t n = p - out;
            memmove (p, p+3, length - n);
            length -= 3;
            out[length] = '\0';
            if (length == n)
                break;
        }
    }

    return out;
}

/**
 * Converts a string from the given character encoding to utf-8.
 *
 * @return a nul-terminated utf-8 string, or null in case of error.
 * The result must be freed using free().
 */
char *FromCharset(const char *charset, const void *data, size_t data_size)
{
    noovo_iconv_t handle = noovo_iconv_open ("UTF-8", charset);
    if (handle == (noovo_iconv_t)(-1))
        return NULL;

    char *out = NULL;
    for(unsigned mul = 4; mul < 8; mul++ )
    {
        size_t in_size = data_size;
        const char *in = data;
        size_t out_max = mul * data_size;
        char *tmp = out = malloc (1 + out_max);
        if (!out)
            break;

        if (noovo_iconv (handle, &in, &in_size, &tmp, &out_max) != (size_t)(-1)) {
            *tmp = '\0';
            break;
        }
        free(out);
        out = NULL;

        if (errno != E2BIG)
            break;
    }
    noovo_iconv_close(handle);
    return out;
}

