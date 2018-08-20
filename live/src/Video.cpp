#include "Video.h"


#include <fstream>
#include <iostream>
#include <chrono>
extern "C" {
	#include <sched.h>
	#include <unistd.h>
};

#define SCALE_THRESHOLD 900
#define MAX_QUEUE 20
#define RESTART_SIZE 1
#define RESIZE_WIDTH 720		
#define RESIZE_HEIGH 480
using namespace Noovo;
int64_t Video::_count_frames = 0;
static std::vector<AVPacket*> temt;
Video::Video() {


}
Video::~Video() {

}
void Video::SetTool(AVStream* input_stream, int outputindex, AVFormatContext* ofmt_ctx,std::shared_ptr<Packet_wrapper> packet_wrapper) {
	_input_stream = input_stream;
	_video_index = outputindex;
	_ofmt_ctx = ofmt_ctx;
	_width = input_stream->codec->width;
	_height = input_stream->codec->height;
}
void Video::InitalTool() {
	if(_height > SCALE_THRESHOLD){
		_handle_enc = vl_video_encoder_init(CODEC_ID_H264, RESIZE_WIDTH, RESIZE_HEIGH, 30, 30*RESIZE_HEIGH*RESIZE_WIDTH*8*3/2, 1);
		_sws_ctx = sws_getContext(_width, _height, AV_PIX_FMT_YUV420P,
                             RESIZE_WIDTH, RESIZE_HEIGH, AV_PIX_FMT_YUV420P,
                             SWS_FAST_BILINEAR, NULL, NULL, NULL);
	}
	else{
		_handle_enc = vl_video_encoder_init(CODEC_ID_H264, _width, _height, 30, 30*_width*_height*8*3/2, 1);
		_sws_ctx = nullptr;
	}
}
void Video::SetTime(int64_t start_pts, int64_t mux_pts) {
	_start_time = start_pts;
	_mux_pts = mux_pts;
	_caluculate_duration();
}
void Video::_caluculate_duration() {
	_duration = _ofmt_ctx->streams[_video_index]->time_base.den/av_q2d(_input_stream->codec->framerate);
}
int Video::_videodecoder(AVPacket* packet,AVFrame* decode_frame) {
	try{	
		
		av_packet_rescale_ts(packet,_input_stream->time_base,_ofmt_ctx->streams[_video_index]->codec->time_base);
		int got_frame;
		if (!decode_frame) {
			throw std::runtime_error("Decoding frame initalize error");
		}
		int ret = avcodec_decode_video2(_input_stream->codec, decode_frame, &got_frame, packet);
		if (ret < 0) {
			throw std::runtime_error("Decoding failed");
		}
		av_free_packet(packet);
		av_freep(packet);
		if (got_frame) {
			return 0;
		}
		else {
			av_frame_free(&decode_frame);
			_got_frame = 0;
			throw std::runtime_error("Decoding failed");
		}
		
	}catch (std::exception const& e)	{
		std::cout << "Exception: " << e.what() << "\n";
		av_frame_free(&decode_frame);
		return -1;
	}	
}
void Video::Codecname() {
	std::cout << "Input Stream Codec " << _input_stream->codec->codec->name << std::endl;
	std::cout << "Output Stream Codec " << _ofmt_ctx->streams[_video_index]->codec->codec->name << std::endl;
	std::cout << std::boolalpha << _interlaced << std::endl;
}
static int frame_count = 0;

int Video::_encode_frame(AVFrame* raw_frame) {
	AVPacket *enc_pkt=(AVPacket*)av_malloc(sizeof(AVPacket));
	try{
		av_new_packet(enc_pkt,512*1024);
		int got_frame = 0;
		raw_frame->pict_type =  AV_PICTURE_TYPE_NONE;
		if(avcodec_encode_video2(_ofmt_ctx->streams[_video_index]->codec,enc_pkt,raw_frame,&got_frame) < 0){
				throw std::runtime_error("Decoding failed");	
		}
		enc_pkt->pts = raw_frame->best_effort_timestamp*_duration;
		enc_pkt->dts = raw_frame->best_effort_timestamp*_duration;
		enc_pkt->stream_index = _video_index;
		if(got_frame)
			av_interleaved_write_frame(_ofmt_ctx, enc_pkt);
		av_frame_free(&raw_frame);
		av_free_packet(enc_pkt);
		av_freep(enc_pkt);
		return 0;
	}catch (std::exception const& e)	{
		std::cout << "Exception: " << e.what() << "\n";
		if(raw_frame)
			av_frame_free(&raw_frame);;
		if(enc_pkt){
			av_free_packet(enc_pkt);
			av_freep(enc_pkt);
		}
	}
	return 0;
}
void Video::FlushEncoder() {
	AVPacket enc_pkt;
	while (1) {
		av_init_packet(&enc_pkt);
		enc_pkt.data = nullptr;
		enc_pkt.size = 0;
		int ret = avcodec_encode_video2(_ofmt_ctx->streams[_video_index]->codec, &enc_pkt, NULL, &_got_frame);
		if (ret < 0)
			break;
		if (!_got_frame)
			break;
		av_packet_rescale_ts(&enc_pkt, _ofmt_ctx->streams[_video_index]->codec->time_base, _ofmt_ctx->streams[_video_index]->time_base);
		enc_pkt.stream_index = 0;
		ret = av_interleaved_write_frame(_ofmt_ctx, &enc_pkt);
		av_free_packet(&enc_pkt);
	}
}
void Video::_resetpts(AVPacket* packet) {
	packet->pts = packet->pts - _start_time;
	packet->dts = packet->dts - _start_time;
}

void Video::MuxFlow(AVPacket* pkt) {
	try {
		AVStream *in_stream = _input_stream;
		AVStream *out_stream = _ofmt_ctx->streams[_video_index];
		if (pkt->dts == AV_NOPTS_VALUE) {
			last_correct_dts += pkt->duration;
			pkt->dts = last_correct_dts;
		}
		else {
			last_correct_dts = pkt->dts;
		}
		if (pkt->pts == AV_NOPTS_VALUE) {
			last_correct_pts += pkt->duration;
			pkt->pts = last_correct_pts;
		}
		else {
			last_correct_pts = pkt->pts;
		}

		pkt->pts = av_rescale_q_rnd(pkt->pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF);
		pkt->dts = av_rescale_q_rnd(pkt->dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF);
		pkt->duration = av_rescale_q(pkt->duration, in_stream->time_base, out_stream->time_base);
		pkt->pos = -1;
		pkt->stream_index = _video_index;
		int ret = av_interleaved_write_frame(_ofmt_ctx, pkt);
		if (ret < 0) {
			std::cout << "dts : " << pkt->dts << std::endl;
			throw std::runtime_error("Error muxing packet");
		}
	}
	catch (std::exception const& e) {
		std::cout << "Exception: " << e.what() << "\n";
	}
}
static int timer = 0;
int Video::Flow(AVPacket* packet) {
	try {
		AVFrame *frame = av_frame_alloc();
		if(_videodecoder(packet,frame) < 0)
			throw std::runtime_error("Error decoding packet");
		_encode_frame(frame);
	}
	catch (std::exception const& e) {

		std::cout << "Exception: " << e.what() << "\n";
		return -1;
	}

	return 0;
}

int64_t Video::ReturnAmount() {
	return _count_frames;
}
int64_t Video::ReturnEndPoint() {
	return _endpoint;
}
void Video::ResetAmount() {
	_count_frames = 0;
}
void Video::_hardwareencode(AVFrame* frame,AVPacket* enc_pkt) {
	int datalen = vl_video_encoder_encode(_handle_enc, FRAME_TYPE_IDR, frame->data, 512*1024,enc_pkt->data);
	enc_pkt->size = datalen;
	enc_pkt->flags = AV_PKT_FLAG_KEY;
	enc_pkt->duration = _duration;
}
void Video::_yuvtonv12(AVFrame *frame,uint8_t* nv12) {	
	int size = _width * _height;
	std::thread yuv_y([&]{
		memcpy(nv12, frame->data[0], size);
		});
	
	std::thread yuv_uv([&](int frame_size){
		int u =frame_size/4;
		uint8_t *src = nv12+frame_size;
		while(u--){
			*src++ =*frame->data[1]++;		
			*src++ =*frame->data[2]++;
		}
	},size);
	
	yuv_y.join();
	yuv_uv.join();
}
void Video::_scaleframe(AVFrame *src_frame,AVFrame *dst_frame) {	
	memset(_resized_buff,0,RESIZE_HEIGH*RESIZE_WIDTH*3/2);
	avpicture_fill((AVPicture *)dst_frame, (uint8_t*)_resized_buff, AV_PIX_FMT_YUV420P, RESIZE_WIDTH, RESIZE_HEIGH);	
	sws_scale(_sws_ctx, (const uint8_t * const*)src_frame->data,
				src_frame->linesize, 0, _height, dst_frame->data, dst_frame->linesize);
}