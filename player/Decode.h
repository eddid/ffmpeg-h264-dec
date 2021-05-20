// Decode.h: interface for the CDecode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECODE_H__736C105F_1871_4D98_B1D5_30399923F64A__INCLUDED_)
#define AFX_DECODE_H__736C105F_1871_4D98_B1D5_30399923F64A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
extern AVCodec ff_h264_decoder;
extern AVCodecParser ff_h264_parser;
};

class CDecode  
{
private:
	AVCodecParserContext* parser;

	AVCodecContext  *m_pCodecCtx;
	AVCodec         *m_pCodec;
	AVFrame         *m_pFrameRGB;
	AVFrame         *m_pFrame;
	uint8_t         *m_buffer;
	int             m_videoStream;

    /* input members */
	FILE            *m_file;
	uint8_t         *m_raw_ptr;
	uint8_t         *m_raw_cur;
	uint8_t         *m_raw_end;

public:
	CDecode();
	virtual ~CDecode();

	bool InitDecode(char* infile);
	bool GetNextFrame();
	void ImgConvert();   //封装ffmpeg中的ima_convert(....)函数
	void ReleseObj();   //释放对象关闭资源

	unsigned char* GetBmpData();
	int   GetFrameWidth();
	int   GetFrameHeight();
};

#endif // !defined(AFX_DECODE_H__736C105F_1871_4D98_B1D5_30399923F64A__INCLUDED_)
