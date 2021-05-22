#include "h264_decoder.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "yuv2rgb.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

extern AVCodec ff_h264_decoder;
extern AVCodecParser ff_h264_parser;

#define BUFFER_READ_SIZE 4096
#define BUFFER_MAX_SIZE 4096*8

#define FFLITE_LOGI(...) //AfxMessageBox(__VA_ARGS__)
#define FFLITE_LOGE(...) printf(__VA_ARGS__)

typedef struct {
    AVCodecParserContext* parser;

    AVCodecContext  *codecCtx;
    AVCodec         *codec;
    AVFrame         *frame;
    uint8_t         *buf_rgb;

    /* input members */
    FILE            *infile;
    uint8_t         *raw_ptr;
    uint8_t         *raw_cur;
    uint8_t         *raw_end;
} h264_decoder;

void *h264_decoder_create() {
    h264_decoder *decoder = (h264_decoder *)malloc(sizeof(*decoder));

    if (NULL != decoder) {
        memset((void *)decoder, 0x00, sizeof(*decoder));
    }

    // Register all formats and codecs
    avcodec_register(&ff_h264_decoder);
    av_register_codec_parser(&ff_h264_parser);

    return (void *)decoder;
}

int h264_decoder_init(void *context, char* infile) {
    h264_decoder *decoder = (h264_decoder *)context;
    int result;

    if ((NULL == decoder) || (NULL == infile)) {
        return AVERROR(EINVAL);
    }

    // Open video file
    if (decoder->infile) {
        fclose(decoder->infile);
    }
    decoder->infile = fopen(infile, "rb");
    if (!decoder->infile) {
        FFLITE_LOGE("Could not open infile\n");
        return AVERROR(ENOENT);
    }

    if (!decoder->codec) {
        decoder->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (!decoder->codec) {
            FFLITE_LOGE("Codec not found\n");
            return AVERROR(ENOENT);
        }
    }

    if (!decoder->codecCtx) {
        decoder->codecCtx = avcodec_alloc_context3(decoder->codec);
        if (!decoder->codecCtx) {
            FFLITE_LOGE("Could not allocate video codec context\n");
            return AVERROR(ENOMEM);
        }

        // Inform the codec that we can handle truncated bitstreams -- i.e.,
        // bitstreams where frame boundaries can fall in the middle of packets
        if(decoder->codec->capabilities & CODEC_CAP_TRUNCATED)
            decoder->codecCtx->flags|=CODEC_FLAG_TRUNCATED;

        // Open codec
        result = avcodec_open2(decoder->codecCtx, decoder->codec, NULL);
        if (result < 0) {
            FFLITE_LOGE("Could not open codec\n");
            return AVERROR(ENOMEM);
        }
    }

    if(!decoder->parser) {
        decoder->parser = av_parser_init(AV_CODEC_ID_H264);
        if(!decoder->parser) {
            FFLITE_LOGE("Could not create H264 parser\n");
            return AVERROR(ENOMEM);
        }
    }

    // Allocate video frame
    if (!decoder->frame) {
        decoder->frame = av_frame_alloc();
        if (!decoder->frame) {
            FFLITE_LOGE("Could not allocate video frame\n");
            return AVERROR(ENOMEM);
        }
    }

    if (decoder->buf_rgb != NULL) {
        free(decoder->buf_rgb);
        decoder->buf_rgb = NULL;
    }

    if (!decoder->raw_ptr) {
        decoder->raw_ptr = malloc(BUFFER_MAX_SIZE);
    }
    decoder->raw_cur = decoder->raw_ptr;
    decoder->raw_end = decoder->raw_ptr + BUFFER_MAX_SIZE;

    return 0;
}

void *h264_decoder_getframe(void *context) {
    h264_decoder *decoder = (h264_decoder *)context;
    static int frame_index = 0;
    AVPacket  packet;

    if (NULL == decoder) {
        return NULL;
    }

    while ((NULL != decoder->infile) || (decoder->raw_cur != decoder->raw_ptr)) {
        uint8_t* data = NULL;
          int size = 0;
        int bytes_used;
        int bytes_read;
        int got_frame;
        int result;

        if ((NULL != decoder->infile) && (decoder->raw_cur + BUFFER_READ_SIZE <= decoder->raw_end)) {
            bytes_read = fread(decoder->raw_cur, 1, BUFFER_READ_SIZE, decoder->infile);
            if (bytes_read <= 0) {
                // EOF or error
                fclose(decoder->infile);
                decoder->infile = NULL;
            } else {
                decoder->raw_cur += bytes_read;
            }
        }

        bytes_used = av_parser_parse2(decoder->parser, decoder->codecCtx, &data, &size, decoder->raw_ptr, decoder->raw_cur - decoder->raw_ptr, 0, 0, AV_NOPTS_VALUE);
        if (bytes_used > 0) {
            // Move unused data in buffer to front, if any
            if (decoder->raw_cur - decoder->raw_ptr > bytes_used) {
                decoder->raw_cur -= bytes_used;
                memcpy(decoder->raw_ptr, decoder->raw_ptr + bytes_used, decoder->raw_cur - decoder->raw_ptr);
            } else {
                decoder->raw_cur = decoder->raw_ptr;
            }
        }

        if (size == 0) {
            continue;
        }

        // We have data of one packet, decode it; or decode whatever when ending
        av_init_packet(&packet);
        packet.data = data;
        packet.size = size;
        got_frame = 0;

        result = avcodec_decode_video2(decoder->codecCtx, decoder->frame, &got_frame, &packet);
        if (got_frame) {
            FFLITE_LOGI("Got frame %d\n", frame_index);
            frame_index++;
            return decoder->frame->data[0];
        } else {
            FFLITE_LOGI("Error while decoding frame %d\n", frame_index);
            continue;
        }
    }

    // Flush the decoder
    return NULL;
}

void *h264_decoder_getbmp(void *context) {
    h264_decoder *decoder = (h264_decoder *)context;
    void *frame = h264_decoder_getframe(context);

    if ((NULL == decoder) || (NULL == frame)) {
        return NULL;
    }
    if (NULL == decoder->buf_rgb) {
        int numBytes;

        // Determine required buffer size and allocate buffer
        numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, decoder->codecCtx->width, decoder->codecCtx->height, 1);
        decoder->buf_rgb = malloc(numBytes);
    }
    switch (decoder->frame->format) {
        case AV_PIX_FMT_YUV420P:
            yuv420p_to_rgb24_1(decoder->frame->data[0], decoder->frame->data[1], decoder->frame->data[2], decoder->buf_rgb, decoder->codecCtx->width, decoder->codecCtx->height);
            break;
        case AV_PIX_FMT_YUV422P:
            yuv422p_to_rgb24(frame, decoder->buf_rgb, decoder->codecCtx->width, decoder->codecCtx->height);
            break;
        default:
            break;
    }
    return decoder->buf_rgb;
}

int h264_decoder_getheight(void *context) {
    h264_decoder *decoder = (h264_decoder *)context;
    if ((NULL == decoder) || (NULL == decoder->codecCtx)) {
        return 0;
    }
    return decoder->codecCtx->height;
}

int h264_decoder_getwidth(void *context) {
    h264_decoder *decoder = (h264_decoder *)context;
    if ((NULL == decoder) || (NULL == decoder->codecCtx)) {
        return 0;
    }
    return decoder->codecCtx->width;
}

int h264_decoder_destroy(void *context) {
    h264_decoder *decoder = (h264_decoder *)context;
    if (NULL == decoder) {
        return AVERROR(EINVAL);
    }

    if (decoder->raw_ptr != NULL) {
        free(decoder->raw_ptr);
        decoder->raw_ptr = NULL;
    }
    if (decoder->buf_rgb != NULL) {
        free(decoder->buf_rgb);
        decoder->buf_rgb = NULL;
    }
    // Free the YUV frame
    if (decoder->frame != NULL) {
        av_frame_free(&decoder->frame);
    }
    // Close the codec
    if (decoder->codecCtx != NULL) {
        avcodec_free_context(&decoder->codecCtx);
    }

    // Close the video file
    if (decoder->parser != NULL) {
        av_parser_close(decoder->parser);
        decoder->parser = NULL;
    }
    free(context);
    return 0;
}
