#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "Stream.hpp"

#include <thread>

#include "vpcodec_1_0.h"
#include "Datastock.hpp"


namespace Noovo{

    class Video : public Stream
    {
    public:
        Video();
        virtual ~Video();      
        virtual void SetTool(AVStream* ,int, AVFormatContext*,std::shared_ptr<Packet_wrapper> );
        virtual void FlushEncoder();
        virtual int Flow(AVPacket*);
        virtual void Codecname();
        virtual void SetTime(int64_t,int64_t);
        virtual void InitalTool();
        virtual void CleanUp(){	
            delete _frame_queue;
	        delete _packet_queue;};
        virtual int64_t ReturnEndPoint();
        static int64_t ReturnAmount();
        static void ResetAmount();   
        void MuxFlow(AVPacket*);        
    private:
        AVStream* _input_stream,*_output_stream;
        AVFormatContext* _ofmt_ctx;     
        AVFrame* _decode_frame;
		struct SwsContext* _sws_ctx;
        vl_codec_handle_t _handle_enc;
        int _duration =0,_video_index=0,_width = 0 ,_height = 0,_got_frame = 0;
        int64_t _start_time,_endpoint,_mux_pts,last_correct_pts = 0,last_correct_dts=0;
        bool _interlaced = true,_encode_bool=true,_decode_bool=true;
        static int64_t _count_frames;//for Timer use
        uint8_t* _resized_buff=nullptr;
        std::thread _encode_thread ,_decode_thread;
        Queue<AVFrame*> *_frame_queue = nullptr;
        Queue<AVPacket*> *_packet_queue = nullptr;
        std::shared_ptr<Packet_wrapper> _packet_wrapper =nullptr;
        int  _videodecoder();
        int  _encode_frame();
        void _resetpts(AVPacket*);
        void _caluculate_duration();
        void _yuvtonv12(AVFrame *frame,uint8_t*);
        void _scaleframe(AVFrame *src_frame,AVFrame *dst_frame);
        void _hardwareencode(AVFrame* frame,AVPacket* enc_pkt);
    };
}
#endif