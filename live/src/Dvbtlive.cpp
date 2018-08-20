#include "Dvbtlive.h"

#include <chrono>
#include <fstream>
/************************
這個ASYNC同步機制非常爛 要修改
一定要修改!!!!!!
不能用thread sleep來稿 哈哈
*************************/
#define PACKET_FFMEPG_SIZE 1024*10
#define QUEUE_SIZE 100
#define MIN_QUEUE 10
#define DV_INIT_ERROR -6
using namespace Noovo;
Queue<Datastock<uint8_t>*>* _queue =new Queue<Datastock<uint8_t>*>(100,10);
Datastock<uint8_t>* buff=new Datastock<uint8_t>();
static int ReadData(void* opaque , uint8_t* data,int size){
	int ret=0;
	if (buff->Returnsize()<= 0 ) {
		delete buff;
		buff = _queue->front_then_pop();
		ret = buff->Copy(data, size);
		return ret;
	}else {
		ret = buff->Copy(data, size);
		return ret;
	}
}
Dvbtlive::Dvbtlive() {
	_dvr = new Dvrlock();
}
Dvbtlive::~Dvbtlive() {
	delete _dvr;
}
Dt_Error_t Dvbtlive::Init(unsigned int fre, unsigned int bandwidth, std::vector<std::pair<int, int>>& pids,SYS_t model) {
	try {
		if(	_dvr->InitDvr(fre, bandwidth,model) != 0)
			throw std::runtime_error("Failed to Init Demod" + std::to_string(__LINE__)+std::string(__FILE__));
		if( _dvr->Filterpids(pids)!=0)
			throw std::runtime_error("Failed to set pid filter" + std::to_string(__LINE__) + std::string(__FILE__));
	
		return 0;
	}catch(std::exception const& e){
        std::cout << "Exception: " << e.what() << "\n";
		return DV_INIT_ERROR;
    } 
}
void Dvbtlive::_dvrthread() {
	while (1){
		Datastock<uint8_t>* buff = new Datastock<uint8_t>();
		if (_dvr->ReadData(buff) > 0)
			_queue->push_check_size(buff);
		else	
			delete buff;
	}
}
void Dvbtlive::MainThread(std::vector<std::pair<int, int>> pids) {
	std::thread dvr_thread(&Dvbtlive::_dvrthread,this);
	std::thread live_thread(&Dvbtlive::_livethread,this,pids);
	live_thread.join();

}
void Dvbtlive::_livethread(std::vector<std::pair<int, int>> pids){
	 Transcoder &_trans = Transcoder::Instance();
	_trans.InitalAvio(PACKET_FFMEPG_SIZE,nullptr,&ReadData,"live.m3u8",pids);
 	_trans.Process();
}
void Dvbtlive::DVRThread() {
	std::thread dvr_thread(&Dvbtlive::_dvrthread, this);
	std::thread live_thread(&Dvbtlive::_testthread, this, &ReadData);
	live_thread.join();
}
void Dvbtlive::_testthread(int(*read)(void *a, uint8_t *b, int c)) {
	std::fstream file;
	file.open("test2.ts", std::ios::out);
	while (1){
		uint8_t* buffer = new uint8_t[1024 * 10];
		int len =read(nullptr, buffer, 1024 * 10);
		if(len <= 0)
			std::cout << "\033[1;31m Failed here !! \033[0m\n";
		else{
			std::cout << "Get Data" << len <<std::endl;
			file.write((char*)buffer,len);
		}
		delete[] buffer;
	}
}