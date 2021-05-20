// Decode.cpp: implementation of the CDecode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Decode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FFLITE_LOGI(...) //AfxMessageBox(__VA_ARGS__)
#define FFLITE_LOGE(...) AfxMessageBox(__VA_ARGS__)

#define BUFFER_READ_SIZE 4096
#define BUFFER_MAX_SIZE 4096*8

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDecode::CDecode()
{
	parser=NULL;	
	m_pCodecCtx=NULL;
	m_pCodec=NULL;
	m_pFrameRGB=NULL;
	m_pFrame=NULL;
	m_buffer=NULL;

}

CDecode::~CDecode()
{
}

bool CDecode::GetNextFrame()
{
	static int frame_index = 0;
	AVPacket  packet;

	if (0 == frame_index)
	{
		packet.data = NULL; 
	}

	while ((NULL != m_file) || (m_raw_cur != m_raw_ptr)) {
		uint8_t* data = NULL;
  		int size = 0;
		int bytes_used;
		int bytes_read;
		int got_frame;
		int result;

		if ((NULL != m_file) && (m_raw_cur + BUFFER_READ_SIZE <= m_raw_end)) {
			bytes_read = fread(m_raw_cur, 1, BUFFER_READ_SIZE, m_file);
			if (bytes_read <= 0) {
				// EOF or error
				fclose(m_file);
				m_file = NULL;
			} else {
				m_raw_cur += bytes_read;
			}
		}

		bytes_used = av_parser_parse2(parser, m_pCodecCtx, &data, &size, m_raw_ptr, m_raw_cur - m_raw_ptr, 0, 0, AV_NOPTS_VALUE);
		if ((size == 0) && (NULL == m_file)) {
			/* Drop broken data */
			m_raw_cur = m_raw_ptr;
			break;
		} else if (size == 0) {
			continue;
		}
		if (bytes_used > 0) {
			// Move unused data in buffer to front, if any
			if (m_raw_cur - m_raw_ptr > bytes_used) {
				m_raw_cur -= bytes_used;
				memcpy(m_raw_ptr, m_raw_ptr + bytes_used, m_raw_cur - m_raw_ptr);
			} else {
				m_raw_cur = m_raw_ptr;
			}

			// We have data of one packet, decode it; or decode whatever when ending
			av_init_packet(&packet);
			packet.data = data;
			packet.size = size;
			got_frame = 0;

			result = avcodec_decode_video2(m_pCodecCtx, m_pFrame, &got_frame, &packet);
			if (got_frame) {
				FFLITE_LOGI("Got frame %d\n", frame_index);
				frame_index++;
				return true;
			} else {
				FFLITE_LOGI("Error while decoding frame %d\n", frame_index);
				continue;
			}
		}
	}

	// Flush the decoder
	return false;
}

//////////////////////////////////////////////////////////////////////////
// show Image

bool CDecode::InitDecode(char* infile)
{
	// Register all formats and codecs
	avcodec_register(&ff_h264_decoder);
	av_register_codec_parser(&ff_h264_parser);

	// Open video file
	m_file = fopen(infile, "rb");
	if (!m_file) {
		FFLITE_LOGE("Could not open infile\n");
		return false;
	}

	m_pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!m_pCodec) {
		FFLITE_LOGE("Codec not found\n");
		return false;
	}

	m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
	if (!m_pCodecCtx) {
		FFLITE_LOGE("Could not allocate video codec context\n");
		return false;
	}

	// Inform the codec that we can handle truncated bitstreams -- i.e.,
	// bitstreams where frame boundaries can fall in the middle of packets
	if(m_pCodec->capabilities & CODEC_CAP_TRUNCATED)
		m_pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;

	// Open codec
	if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0) {
		FFLITE_LOGE("Could not open codec\n");
		return false;
	}

	parser = av_parser_init(AV_CODEC_ID_H264);
	if(!parser) {
		FFLITE_LOGE("Could not create H264 parser\n");
		return false;
	}

	// Allocate video frame
	m_pFrame = av_frame_alloc();
	if (!m_pFrame) {
		FFLITE_LOGE("Could not allocate video frame\n");
		return false;
	}
	// Allocate an AVFrame structure
	m_pFrameRGB = av_frame_alloc();
	if (!m_pFrameRGB) {
		FFLITE_LOGE("Could not allocate RGB frame\n");
		return false;
	}

	// Find the first video stream
	m_videoStream = -1;

	m_raw_ptr = new uint8_t[BUFFER_MAX_SIZE];
	m_raw_cur = m_raw_ptr;
	m_raw_end = m_raw_ptr + BUFFER_MAX_SIZE;
	
	return true;
}

//封装ffmpeg中的ima_convert()函数
void CDecode::ImgConvert() 
{
	if (NULL == m_buffer) {
		int numBytes;

		// Determine required buffer size and allocate buffer
		numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height, 1);
		m_buffer = new uint8_t[numBytes];
		// Assign appropriate parts of buffer to image planes in pFrameRGB
		av_image_fill_arrays(m_pFrameRGB->data, m_pFrameRGB->linesize, m_buffer, AV_PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height, 1);
	}
	//av_img_convert((AVPicture *)m_pFrameRGB, AV_PIX_FMT_BGR24,(AVPicture*)m_pFrame,m_pCodecCtx->pix_fmt, m_pCodecCtx->width,m_pCodecCtx->height);
}

unsigned char* CDecode::GetBmpData()   //取得图像数据
{
	return m_pFrameRGB->data[0];
}

int CDecode::GetFrameWidth()   //取得图像的实际宽度
{
	return m_pCodecCtx->width;
}

int CDecode::GetFrameHeight()           //取得图像的实际高度
{
	return m_pCodecCtx->height;
}

//释放对象关闭资源
void CDecode::ReleseObj()
{
	if(m_buffer!=NULL) {
		free(m_buffer);
		m_buffer = NULL;
	}
	if(m_pFrameRGB!=NULL) {
		av_frame_free(&m_pFrameRGB);
	}
	// Free the YUV frame
	if(m_pFrame!=NULL) {
		av_frame_free(&m_pFrame);
	}
	// Close the codec
	if (m_pCodecCtx != NULL) {
		avcodec_close(m_pCodecCtx);
		av_free(m_pCodecCtx);
		m_pCodecCtx = NULL;
	}

	// Close the video file
	if (parser!=NULL) {
		av_parser_close(parser);
		parser = NULL;
	}
}
