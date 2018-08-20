#include "Dvrlock.h"

#include <stdexcept>
#include <iostream>

#include "AVLDemod.h"



#define DMX_NO 0
#define DVR_NO 0
#define DVBC  0,
#define DVBSX  1
#define DVBTX  2
#define ISDBT  3
#define DTMB  4
#define ISDBS  5
#define ABSS  6
#define ATSC  7
#define DVBC2  8

#define NV_DL_NO_ALLOC -3
#define NV_DL_OK 0
#define NV_DL_INIT_Error -4
#define POLL_TIME 1000
#define DVR_BUF_SIZE 188*1024

using namespace Noovo;

Dvrlock::Dvrlock(){

}
Dvrlock::~Dvrlock(){
    
}
DV_Error_t Dvrlock::InitDvr(unsigned int fre ,unsigned int bandwidth){
    try{
        AVLDemod *_demod=AVLDemod::GetInstance();
        if(_demod->ReturnWhetherInit() == false){
            if(_demod->Init(_chipid)!=0)
                throw std::runtime_error("Failed to Init Demod"+std::to_string(__LINE__));
        }
        if(_demod->SetDvbsys(DVBTX)!=0){
            throw std::runtime_error("Failed to Set Dvb System"+std::to_string(__LINE__));
        }
        if(_demod->Lockchannel(fre,bandwidth)!=0){
            throw std::runtime_error("Failed to Lock Channel"+std::to_string(__LINE__));
        }
        if(_dmxdvrprocess()!=0){
            throw std::runtime_error("Failed to Init Dmx"+std::to_string(__LINE__));
        }
        return NV_DL_OK;
    }catch(std::runtime_error e){
        std::cout << "Runtime error: " << e.what()<<std::endl;
        return NV_DL_INIT_Error;
    }
}
//這邊都先用0做dev_num for demo
DV_Error_t Dvrlock::_dmxdvrprocess(){
    int ret = 0;
    _dmx = new DVBDmx_t();
    if(!_dmx)
        return NV_DL_NO_ALLOC;
    ret |= dmx_open(DMX_NO,_dmx);
	ret |= dvr_open(DVR_NO,&_dvr_fb);
    ret |= dmx_set_source(DMX_NO,NV_DMX_SRC_TS0);
    ret |= dvr_set_source(DMX_NO,NV_DVR_SRC_ASYNC_FIFO0);	
  //  ret |= dvr_set_buf_size(_dvr_fb,DVR_BUF_SIZE);//不知道為什麼設定這個會出問題
    return ret;    
}
DV_Error_t Dvrlock::Filterpids(std::vector<std::pair<int,int>> & pids){
    int ret = 0;
    if(!_dmx)
        return NV_DL_NO_ALLOC;
    for(auto it = pids.begin();it != pids.end();it++){
        if(it->first >0){
            ret |= dmx_alloc_filter(_dmx, _filter);
            ret |= dmx_set_pes_filter(_dmx,_filter,it->first);
            ret |= dmx_enable_filter(_dmx,_filter,NV_TRUE);
            _filter++;
        }
        if(it->second > 0){
            ret |= dmx_alloc_filter(_dmx, _filter);
            ret |= dmx_set_pes_filter(_dmx, _filter, it->second);
            ret |= dmx_enable_filter(_dmx, _filter, NV_TRUE);
            _filter++;
        }
    }
    return ret;
}
int Dvrlock::ReadData(Datastock<uint8_t>*& datastock,int require){
    int ret = 0;
    int size = require;
    datastock->Allocate(size);
    while(1){    
        ret=dvr_poll(_dvr_fb,POLL_TIME);
        if(ret != NV_DVR_OK){      
            continue;
        }  
        ret =dvr_read(_dvr_fb, datastock->Get(), &size);
        if(ret == NV_DVR_OK){ 
            datastock->Set(size);  
            return size;
        }     
    }
}