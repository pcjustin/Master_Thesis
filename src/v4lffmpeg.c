#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>

int main(int argc, char **argv)
{
    // General
    int bRet, ix;
    // Input
    const char *sFile = "/dev/video0";
    AVFormatContext *pIFormatCtx;
    AVCodecContext *pICodecCtx;
    AVCodec *pICodec;
    AVFrame *pFrame;
    int ixInputStream = -1;
    AVInputFormat *pIFormat;
    AVPacket oPacket;
    int fFrame = 0;
    // Output
    AVCodecContext *pOCodecCtx;
    AVCodec *pOCodec;
    uint8_t *pBuffer;
    int szBuffer;
    int szBufferActual;
    int bImgFormat = PIX_FMT_YUVJ420P;
    int bQuality = 3;
    FILE *fdJPEG;

    // Prepare ffmpeg library
    av_register_all();
    avdevice_register_all();

    // Open video stream
    pIFormat = av_find_input_format("video4linux2");

    bRet = av_open_input_file(&pIFormatCtx, sFile, pIFormat, 0, NULL);

    printf("Abertura retornou %d\n",bRet);

    if (bRet != 0) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    /* Retrieve stream information */
    if (av_find_stream_info(pIFormatCtx) < 0) {
        fprintf(stderr, "No stream info\n");
        return 1;
    }

    /* Find the first video stream */
    ixInputStream = -1;
    for (ix = 0; ix < pIFormatCtx->nb_streams; ix++) {
        if (pIFormatCtx->streams[ix]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            ixInputStream = ix;
            break;
        }
    }
    if (ixInputStream == -1) {
        fprintf(stderr, "No video stream in file\n");
        return 1;
    }

    /* Get a pointer to the codec context for the video stream */
    pICodecCtx = pIFormatCtx->streams[ixInputStream]->codec;

    /* Find the decoder for the video stream */
    pICodec = avcodec_find_decoder(pICodecCtx->codec_id);
    if (!pICodec) {
        fprintf(stderr, "Codec not found\n");
        return 1;
    }

    /* Open input codec */
    if (avcodec_open(pICodecCtx, pICodec) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return 1;
    }

    /* Allocate video frame */
    pFrame = avcodec_alloc_frame();

    /* Determine required buffer size and allocate buffer */
    szBuffer = avpicture_get_size(bImgFormat, pICodecCtx->width, pICodecCtx->height);
    pBuffer = av_mallocz(szBuffer);

    /* Allocate Output Codec */
    pOCodecCtx = avcodec_alloc_context();
    if (!pOCodecCtx) {
        fprintf(stderr, "Could not allocate codec\n");
        return 1;
    }

    /* Initialize picture size and other format parameters */
    pOCodecCtx->bit_rate = pICodecCtx->bit_rate;
    pOCodecCtx->width = pICodecCtx->width;
    pOCodecCtx->height = pICodecCtx->height;
    pOCodecCtx->pix_fmt = bImgFormat;
    pOCodecCtx->codec_id = CODEC_ID_MJPEG;
    pOCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pOCodecCtx->time_base.num = pICodecCtx->time_base.num;
    pOCodecCtx->time_base.den = pICodecCtx->time_base.den;

    /* Allocate codec for JPEG */
    pOCodec = avcodec_find_encoder(pOCodecCtx->codec_id);
    if (!pOCodec) {
        fprintf(stderr, "Codec not found\n");
        return 1;
    }
    if (avcodec_open(pOCodecCtx, pOCodec) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return 1;
    }

    /* Initialize all VBR settings */
    pOCodecCtx->qmin = pOCodecCtx->qmax = bQuality;
    pOCodecCtx->mb_lmin = pOCodecCtx->lmin = pOCodecCtx->qmin * FF_QP2LAMBDA;
    pOCodecCtx->mb_lmax = pOCodecCtx->lmax = pOCodecCtx->qmax * FF_QP2LAMBDA;
    pOCodecCtx->flags |= CODEC_FLAG_QSCALE;
    pOCodecCtx->global_quality = pOCodecCtx->qmin * FF_QP2LAMBDA;

    /* Get 1 frame */
    bRet = av_read_frame(pIFormatCtx, &oPacket);
    if (bRet < 0) {
        fprintf(stderr, "Could not read frame\n");
        return 1;
    }

    if (oPacket.stream_index != ixInputStream) {
        fprintf(stderr, "Packet is not for our stream\n");
        return 1;
    }

    /* Decode video frame */
    avcodec_decode_video2(pICodecCtx, pFrame, &fFrame,&oPacket);

    av_free_packet(&oPacket);
    if (!fFrame) {
        fprintf(stderr, "Error reading frame\n");
        return 1;
    }

    /* Encode the frame as a JPEG using certain quality settings */
    pFrame->pts = 1;
    pFrame->quality = pOCodecCtx->global_quality;
    szBufferActual = avcodec_encode_video(pOCodecCtx, pBuffer, szBuffer, pFrame);

    /* Write JPEG to file */
    fdJPEG = fopen("test.jpg", "wb");
    bRet = fwrite(pBuffer, sizeof(uint8_t), szBufferActual, fdJPEG);
    fclose(fdJPEG);
    if (bRet != szBufferActual) {
        fprintf(stderr, "Error writing jpeg file\n");
        return 1;
    }

    /* Cleanup */
    if (pBuffer) {
        av_freep(&pBuffer);
        pBuffer = NULL;
        szBuffer = 0;
    }

    if (pFrame) {
        av_freep(&pFrame);
        pFrame = NULL;
    }

    if (pICodecCtx) {
        avcodec_close(pICodecCtx);
        pICodecCtx = NULL;
    }

    if (pOCodecCtx) {
        avcodec_close(pOCodecCtx);
        pOCodecCtx = NULL;
    }

    if (pIFormatCtx) {
        av_close_input_file(pIFormatCtx);
        pIFormatCtx = NULL;
    }

    return 0;
}

