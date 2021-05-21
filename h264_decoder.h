#ifndef H264_DECODER_H
#define H264_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

void *h264_decoder_create();
int h264_decoder_init(void *context, char* infile);
void *h264_decoder_getframe(void *context);
void *h264_decoder_getbmp(void *context);
int h264_decoder_getheight(void *context);
int h264_decoder_getwidth(void *context);
int h264_decoder_destroy(void *context);

#ifdef __cplusplus
}
#endif

#endif
