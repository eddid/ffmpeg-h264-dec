#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#include <time.h>
#include <winsock.h>
#else
#include <sys/time.h>
#endif
#include "h264_decoder.h"

static void yuv_save(unsigned char *buf[], int wrap[], int xsize,int ysize, FILE *f)
{
	int i;
	for (i = 0; i < ysize; i++) {
		fwrite(buf[0] + i * wrap[0], 1, xsize, f);
	}
	for (i = 0; i < ysize / 2; i++) {
		fwrite(buf[1] + i * wrap[1], 1, xsize/2, f);
	}
	for (i = 0; i < ysize / 2; i++) {
		fwrite(buf[2] + i * wrap[2], 1, xsize/2, f);
	}
}

static void h264_video_decode(const char *filename, const char *outfilename)
{
	FILE *outfile;
	void *decoder;
	void *frame;

	int result = 0;
	int frame_index = 0;
	int frame_width = 0;
	int frame_height = 0;
	
	int64_t tv_start, tv_end;
	float time;
	float speed;

	printf("Decode file '%s' to '%s'\n", filename, outfilename);

	decoder = h264_decoder_create();
	if (!decoder) {
		fprintf(stderr, "Could not create decoder\n");
		exit(1);
	}

	result = h264_decoder_init(decoder, filename);
	if (result < 0) {
		fprintf(stderr, "Could not init decoder(%d)\n", result);
		exit(1);
	}
	
	outfile = fopen(outfilename, "wb");
	if (!outfile) {
		fprintf(stderr, "Could not open '%s'\n", outfilename);
		exit(1);
	}

	tv_start = av_gettime();
	while (1) {
	    frame= h264_decoder_getframe(decoder);
	    if (NULL == frame) {
	        break;
	    }
	    frame_width = h264_decoder_getwidth(decoder);
	    frame_height = h264_decoder_getheight(decoder);
		printf("Got frame %d\n", frame_index);
		yuv_save(frame, frame, frame_width, frame_height, outfile);
		frame_index++;
	}

	tv_end = av_gettime();

	fclose(outfile);

	h264_decoder_destroy(decoder);
	printf("Done\n");

	time = (tv_end - tv_start) / 1000000.0;
	speed = (frame_index + 1) / time;
	printf("Decoding time: %.3fs, speed: %.1f FPS\n", time, speed);
}

int main(int argc, char* argv[])
{
	if (argc == 3) {
		h264_video_decode(argv[1], argv[2]);
	} else {
		printf("Usage: %s <input_file> <output_file>\n", argv[0]);
		//h264_video_decode("test/352x288Foreman.264", "test.yuv");
	}
	return 0;		
}
