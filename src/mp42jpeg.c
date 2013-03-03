#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/audioconvert.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
}

#define MAX_PATH 256

int GetVidDuration( char cPath[], char cFLVFileName[])
{
	int nDuration = 0;
	int nStart = 0;
	char cFileName[MAX_PATH];
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	av_register_all();

	sprintf_s( cFileName, MAX_PATH, "%s%s", cPath, cFLVFileName);
	if(avformat_open_input(&pFormatCtx, cFileName, NULL, NULL)==0)
	{
		if(av_find_stream_info(pFormatCtx)>=0)
			nDuration = (int)(pFormatCtx->duration - pFormatCtx->start_time) / AV_TIME_BASE;

		av_close_input_file(pFormatCtx);
	}
	return nDuration;
}

bool PullFrame( char cInPath[], char cFileName[], char cImgOutPath[], char cImgOutName[], int nTimeStamp )
{
	bool bRet = false;
	int videoStream   = -1;
	AVCodec *pMJPEGCodec=NULL;
	AVFrame *pFrame=NULL; 
	AVFrame *pFrameRGB=NULL;
	AVPacket packet;
	int frameFinished=0;

	char cFileLocale[MAX_PATH];
	sprintf_s( cFileLocale, MAX_PATH, "%s%s", cInPath, cFileName);

	avformat_network_init();
	av_register_all();      

	AVFormatContext *pFormatCtx=avformat_alloc_context();
	if(avformat_open_input(&pFormatCtx, cFileLocale, NULL, NULL) == 0 )   
	{
		if(av_find_stream_info(pFormatCtx) >= 0 )   
		{
			for(int i=0; i<(int)pFormatCtx->nb_streams; i++)   
			{
				if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)   
				{
					videoStream=i;
					break;
				}
			}
			if(videoStream >= 0 )  
			{
				AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
				AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
				if(pCodec != NULL)
				{
					if( avcodec_open(pCodecCtx, pCodec) >= 0 )
					{
						av_init_packet(&packet);
						pFrame=avcodec_alloc_frame();
						pFrameRGB=avcodec_alloc_frame();
						if(pFrameRGB != NULL)
						{   
							int numBytes=0;
							int nFramesSaved = 0;

							numBytes=avpicture_get_size(PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height);
							uint8_t *buffer=new uint8_t[numBytes];

							avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height);
							nTimeStamp = nTimeStamp * AV_TIME_BASE;

							int nRet = av_seek_frame(pFormatCtx, -1, nTimeStamp + pFormatCtx->start_time, AVSEEK_FLAG_ANY );
							if( nRet < 0 )
								nRet = av_seek_frame(pFormatCtx, -1, nTimeStamp + pFormatCtx->start_time, AVSEEK_FLAG_BACKWARD );
							if( nRet >= 0 )
							{
								if(av_read_frame(pFormatCtx, &packet)>=0 )
								{
									if(packet.stream_index==videoStream)
									{
										avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);   
										if( frameFinished )   
										{
											//DO SOMETHING with the frame.
										}
									}
									av_free_packet(&packet);   
								}
								delete [] buffer;
							}
							else 
								bRet = false;
						}
						else
							bRet = false;

						av_free(pFrameRGB);
						av_free(pFrame);
						avcodec_close(pCodecCtx);
					}
					else
						bRet = false;
				}
				else
					bRet = false;
			}
			else
				bRet = false;
		}
		else
			bRet = false;
		av_close_input_file(pFormatCtx);
	}
	return bRet;
}

void main()
{
	char cFilePath[MAX_PATH];
	char cFileName[MAX_PATH];
	strcpy_s( cFileName, MAX_PATH, "testfile.mp4" );
	strcpy_s( cFilePath, MAX_PATH, "D:\\Testing\\ffmpegTest\\PullImage\\");

	GetVidDuration(cFilePath, cFileName);
	PullFrame(cFilePath, cFileName, cFilePath, "imageout.jpg", 10 ); 
}
