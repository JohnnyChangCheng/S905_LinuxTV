#include "Dvbtlive.h"

#include <chrono>
#include <fstream>

#define PACKET_FFMEPG_SIZE 10*1024
#define QUEUE_SIZE 100
#define MIN_QUEUE 10
#define DV_INIT_ERROR -6
using namespace Noovo;
Queue<Datastock<uint8_t>*> *_queue = nullptr ;
static int ReadData(void* opaque , uint8_t* data,int size){
	int ret = 0;
	try{
		
			Datastock<uint8_t>*  buff = _queue->front_then_pop();
			if(buff){
				ret = buff->Copy(data, size);
				delete buff;
			}else
				ret =0;
			
			return ret;
		
	}catch(...){
		std::cout << "\033[1;31m Error at FFMPEG READ DATA\033[0m\n";
		return 0;
	}
}
Dvbtlive::Dvbtlive() {
	_dvr = new Dvrlock();
}
Dvbtlive::~Dvbtlive() {
	delete _dvr;
}
Dt_Error_t Dvbtlive::Init(unsigned int fre, unsigned int bandwidth, std::vector<std::pair<int, int>>& pids) {
	try {
		if(	_dvr->InitDvr(fre, bandwidth) != 0)
			throw std::runtime_error("Failed to Init Demod" + std::to_string(__LINE__)+std::string(__FILE__));
		if( _dvr->Filterpids(pids)!=0)
			throw std::runtime_error("Failed to set pid filter" + std::to_string(__LINE__) + std::string(__FILE__));
		_queue = new Queue<Datastock<uint8_t>*>(100,10);
		return 0;
	}catch(std::exception const& e){
        std::cout << "Exception: " << e.what() << "\n";
		return DV_INIT_ERROR;
    } 
}
void Dvbtlive::_dvrthread() {
	while (1){
		
		Datastock<uint8_t>* buff = new Datastock<uint8_t>();
		if(_dvr->ReadData(buff,PACKET_FFMEPG_SIZE) > 0) 
				_queue->push_check_size(buff);
		else	
			delete buff;
	}
}
void Dvbtlive::MainThread(std::vector<std::pair<int, int>> pids) {
	std::thread dvr_thread(&Dvbtlive::_dvrthread,this);
	std::thread live_thread(&Dvbtlive::_livethread,this,pids);
	dvr_thread.join();
	live_thread.join();
}
void Dvbtlive::_livethread(std::vector<std::pair<int, int>> pids){
	 Transcoder &_trans = Transcoder::Instance();
	_trans.InitalAvio(PACKET_FFMEPG_SIZE,nullptr,&ReadData,"live.m3u8",pids);
 	_trans.Process();
}