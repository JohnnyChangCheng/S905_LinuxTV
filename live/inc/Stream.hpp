#ifndef __STREAM_H__
#define __STREAM_H__
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>
#include <iostream>
#include <queue>
#include <chrono>
#include <condition_variable>
#include <unordered_map>
extern "C"{
    #include <stdio.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include "libavformat/avio.h"
    #include <libavutil/file.h>
    #include <libavfilter/buffersink.h>
    #include "libavutil/audio_fifo.h"
    #include "libavutil/avassert.h"
    #include <libavfilter/buffersrc.h>
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include "libavutil/timestamp.h"
    #include "libswresample/swresample.h"
    #include "libavutil/avstring.h"
    #include "libavutil/avutil.h"
    #include "libavutil/frame.h"
    #include "libavutil/opt.h"
    #include <libavutil/imgutils.h>
    #include <libavutil/parseutils.h>
    #include <libswscale/swscale.h>

}

namespace Noovo{
     struct DtsLess{
        bool operator()( AVPacket* const& lhs, AVPacket*const& rhs) const {
                        return lhs->dts > rhs->dts;
                    }
    };
    class Packet_wrapper{ //實做在Packet.cpp
        public:
            Packet_wrapper(AVFormatContext* of):_ofmt(of){}
            ~Packet_wrapper(){}
            void Push_packet(AVPacket* packet){
                _mutex.lock();
                _packet_queue.push(packet);
                _mutex.unlock();
            }
           void Write_frame(){
                 _thread = std::thread(&Packet_wrapper::_write_packet_thread,this);
            }
        private:
            std::priority_queue<AVPacket*,std::vector<AVPacket*>,DtsLess> _packet_queue;
            std::mutex _mutex;
			std::thread _thread;
            bool _bool_cntl = true;
            AVFormatContext *_ofmt;
            int _min_size = 20;
            void _write_packet_thread(){  
                while(_bool_cntl){
                    _mutex.lock();
                    if(_packet_queue.size() > _min_size){
                        AVPacket *write_packet = _packet_queue.top();
                        _packet_queue.pop();
                        _mutex.unlock();
                        av_write_frame(_ofmt,write_packet);
                        av_free_packet(write_packet);
                        av_freep(write_packet);
                    }else{
                        _mutex.unlock();
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));//encoder more slowly so here i sleep	
                    }
               }
            }
    };

    class Stream{
    public:
        virtual ~Stream(){};
        virtual void SetTool(AVStream*,int,AVFormatContext*,std::shared_ptr<Packet_wrapper>)=0;
        virtual void FlushEncoder()=0;
        virtual void SetTime(int64_t,int64_t)=0;
        virtual int Flow(AVPacket*)=0;
        virtual void Codecname()=0;
        virtual void InitalTool()=0;
        virtual void CleanUp()=0;
        virtual int64_t ReturnEndPoint()=0;
    };
    class Ofmt_list{
    public:
        Ofmt_list():_ofmt_ctx(nullptr),_video_index(-1),_audio_index(-1){};
        ~Ofmt_list(){};
        inline void SetAvformat(AVFormatContext* ctx){
            _ofmt_ctx=ctx;
        }
        inline void SetVideo(int index){
            _video_index=index;
        }
        inline void SetAudio(int index){
            _audio_index=index;
        }
        inline void SetConcat(int64_t concate){
            _concatenate=concate;
        }
        inline AVFormatContext* ReturnAvformat(){
            return _ofmt_ctx;
        }
        inline int Returnvideo(){
            return _video_index;
        }
        inline int Returnaudio(){
            return _audio_index;
        }
        inline int64_t Returnconcate(){
            return _concatenate;
        }
        inline void SetStream(std::shared_ptr<Stream> audio,std::shared_ptr<Stream> video){
            _audio = audio;
            _video = video;    
        }
        inline void GetConcatpoint(){
            _concatenate =_video->ReturnEndPoint();
        }
        inline void GetConcatpoint(int64_t con){
            _concatenate =con;
        }
    private:
        AVFormatContext* _ofmt_ctx;
        int _video_index;
        int _audio_index;
        int64_t _concatenate;
        std::shared_ptr<Stream> _audio,_video;
    };
   
}

#endif