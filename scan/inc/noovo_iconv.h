#ifndef __NICONV__
#define __NICONV__

static inline unsigned (clz)(unsigned x)
{
#ifdef __GNUC__
    return __builtin_clz (x);
#else
    unsigned i = sizeof (x) * 8;

    while (x)
    {
        x >>= 1;
        i--;
    }
    return i;
#endif
}
static inline int64_t GCD ( int64_t a, int64_t b )
{
    while( b )
    {
        int64_t c = a % b;
        a = b;
        b = c;
    }
    return a;
}
#define clz8( x ) (clz(x) - ((sizeof(unsigned) - sizeof (uint8_t)) * 8))
typedef void *noovo_iconv_t;
noovo_iconv_t noovo_iconv_open( const char *tocode, const char *fromcode );
size_t noovo_iconv( noovo_iconv_t cd, const char **inbuf, size_t *inbytesleft,
                  char **outbuf, size_t *outbytesleft );
int noovo_iconv_close( noovo_iconv_t cd );
bool noovo_ureduce( unsigned *pi_dst_nom, unsigned *pi_dst_den,
                        uint64_t i_nom, uint64_t i_den, uint64_t i_max );
char *FromCharset(const char *charset, const void *data, size_t data_size);
char * Descriptor(const void *buf, size_t length);
#endif