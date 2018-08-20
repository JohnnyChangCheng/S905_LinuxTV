#include "vpcodec_1_0.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
#include <chrono>
int main(int argc, const char *argv[]){
    int width, height, gop, framerate, bitrate, num, in_size = 0;
    int outfd = -1;
    FILE *fp = NULL;
    int datalen = 0;
    vl_codec_handle_t handle_enc;
    if (argc < 8)
    {
        printf("Amlogic AVC Encode API \n");
        printf(" usage: output [srcfile][outfile][width][height][gop][framerate][bitrate][num]\n");
        printf("  options  :\n");
        printf("  srcfile  : yuv data url in your root fs\n");
        printf("  outfile  : stream url in your root fs\n");
        printf("  width    : width\n");
        printf("  height   : height\n");
        printf("  gop      : I frame refresh interval\n");
        printf("  framerate: framerate \n ");
        printf("  bitrate  : bit rate \n ");
        printf("  num      : encode frame count \n ");
        return -1;
    }
    else
    {
        printf("%s\n", argv[1]);
        printf("%s\n", argv[2]);
    }
    width =  atoi(argv[3]);
    if ((width < 1) || (width > 1920))
    {
        printf("invalid width \n");
        return -1;
    }
    height = atoi(argv[4]);
    if ((height < 1) || (height > 1080))
    {
        printf("invalid height \n");
        return -1;
    }
    gop = atoi(argv[5]);
    framerate = atoi(argv[6]);
    bitrate = atoi(argv[7]);
    num = atoi(argv[8]);
    if ((framerate < 0) || (framerate > 30))
    {
        printf("invalid framerate \n");
        return -1;
    }
    if (bitrate <= 0)
    {
        printf("invalid bitrate \n");
        return -1;
    }
    if (num < 0)
    {
        printf("invalid num \n");
        return -1;
    }
    printf("src_url is: %s ;\n", argv[1]);
    printf("out_url is: %s ;\n", argv[2]);
    printf("width   is: %d ;\n", width);
    printf("height  is: %d ;\n", height);
    printf("gop     is: %d ;\n", gop);
    printf("frmrate is: %d ;\n", framerate);
    printf("bitrate is: %d ;\n", bitrate);
    printf("frm_num is: %d ;\n", num);

    unsigned framesize  = width * height * 3 / 2;
    unsigned char **input= (unsigned char **)malloc(sizeof(char*)*3);
    unsigned char *buffer = (unsigned char *)malloc(512 * 1024 * sizeof(char));
    input[0] = (unsigned char *)malloc(width * height );
    input[1] = (unsigned char *)malloc(width * height/4 );
    input[2] = (unsigned char *)malloc(width * height/4 );
    fp = fopen((char *)argv[1], "rb");
    if (fp == NULL)
    {
        printf("open src file error!\n");
        goto exit;
    }
    outfd = open((char *)argv[2], O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (outfd < 0)
    {
        printf("open dist file error!\n");
        goto exit;
    }
	handle_enc = vl_video_encoder_init(CODEC_ID_H264, width, height, framerate, 1920*1080*8*3/2*30, gop);
    while (num > 0) {
        if (fread(input[0], 1, width * height, fp) != width * height) {
            printf("read input file error!\n");
            goto exit;
        }
          if (fread(input[1], 1, width * height/4, fp) != width * height/4) {
            printf("read input file error!\n");
            goto exit;
        }
          if (fread(input[2], 1, width * height/4, fp) != width * height/4) {
            printf("read input file error!\n");
            goto exit;
        }
        memset(buffer, 0, 512 * 1024 * sizeof(char));
      
        datalen = vl_video_encoder_encode(handle_enc, FRAME_TYPE_IDR, input, 512*1024, buffer);
      
        if (datalen >= 0)
            write(outfd, (unsigned char *)buffer, datalen);
        num--;
	}
    vl_video_encoder_destory(handle_enc);
    close(outfd);
    fclose(fp);
    free(buffer);
    free(input);
    return 0;
exit:
    if (input)
        free(input);
    if (buffer)
        free(buffer);
    if (outfd >= 0)
        close(outfd);
    if (fp)
        fclose(fp);
    return -1;
}