#include "push.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/codec.h"
#include "libavcodec/avcodec.h"
#include "libavutil/pixdesc.h"
#include "libavutil/hwcontext.h"
#include "libavutil/pixfmt.h"
#include "libavutil/opt.h"
#include "libavutil/log.h"
#include "libavutil/avassert.h"
#include "libavutil/imgutils.h"
#include <stdbool.h>

int sum(int a,int b){
    return (a+b);
}
//char* inf() {
//    return av_version_info();
//}

/*void setAvLog() {
	av_log_set_level(AV_LOG_INFO);
	av_log_set_callback(logCallback);
	av_log(NULL, AV_LOG_ERROR, "Invalid prediction method.\n");
}*/

int push() {

    AVFormatContext* ic = NULL;
    const char* in_path = "C:\\Users\\casair\\Downloads\\ke.mp4"; //输入文件
    const char* out_path = "C:\\Users\\casair\\Desktop\\rjm-out.yuv"; //输出文件
    int ret;

    ret = avformat_open_input(&ic, in_path, NULL, NULL);
    if (ret < 0) {
        return ret;
    }
    //av_dump_format(ic, 0, inputFile, 0); //打印媒体信息

    ret = avformat_find_stream_info(ic, NULL); //查询流信息
    if (ret < 0) {
        avformat_free_context(ic);
        return ret;
    }

    const AVCodec* codec;
    ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0); //获取视频流索引
    if (ret < 0) {
        return ret;
    }
    int videoStreamIndex = ret;
    AVStream* videoStream = ic->streams[ret];

    //const AVCodec* codec = avcodec_find_decoder(videoStream->codecpar->codec_id); //获取解码器

    AVCodecContext* avctx = avcodec_alloc_context3(codec);//分配AVCodecContext空间
    ret = avcodec_parameters_to_context(avctx, videoStream->codecpar); //拷贝解码器
    if (ret < 0) {
        return ret;
    }

    ret = avcodec_open2(avctx, codec, NULL);
    if (ret < 0) {
        return ret;
    }

    AVPacket* pkt = av_packet_alloc(); //分配包
    if (!pkt) {
        return 1;
    }
    AVFrame* frame = av_frame_alloc(); //分配帧
    if (!frame) {
        return 1;
    }

    FILE* pFile;
    fopen_s(&pFile, out_path, "wb+");

    int frame_count = 0;
    //int frame_count_max = 100;
    int frame_count_max = INT_MAX;
    while (true && frame_count < frame_count_max) {
        ret = av_read_frame(ic, pkt);
        if (ret < 0) {
            break;
        }

        if (pkt->stream_index != videoStreamIndex) {
            continue;
        }

        if (avcodec_send_packet(avctx, pkt) != 0) { //将读取的包发送到解码器上下文，进行解码
            break;
        }

        while (avcodec_receive_frame(avctx, frame) == 0) { //不断循环，取出avcodec_send_packet解码后的帧数据
            frame_count++;

            int size = av_image_get_buffer_size(frame->format, frame->width, frame->height, 1);
            uint8_t* buffer = (uint8_t*)av_malloc(size); //申请一块内存
            av_image_copy_to_buffer(buffer, size,
            	(const uint8_t* const*)frame->data,
            	(const int*)frame->linesize, frame->format,
            	frame->width, frame->height, 1); //将数据拷贝到buffer中

            fwrite(buffer, 1, size, pFile);//将buffer数据写入文件中去

            av_freep(&buffer);//清除buffer
        }
    }

    if (pFile)
        fclose(pFile);
    av_packet_free(&pkt);
    avcodec_close(avctx);
    avcodec_free_context(&avctx);
    av_frame_free(&frame);
    avformat_close_input(&ic);
    return 0;
}
