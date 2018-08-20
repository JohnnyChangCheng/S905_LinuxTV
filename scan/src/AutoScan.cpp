#include "AutoScan.h"

#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>

extern "C"{
	#include "noovo_dmx.h"
	#include "noovo_dvr.h"
	#include "memory.h"
	#include "unistd.h"
}
#include "Demuxer.h"
#include "ScanLog.h"

#define PAT 0
#define SDT 0x11
#define PMT 1

#define NV_NULL_Error -6
#define NV_OK 0
#define NV_DB_Error -3
#define NV_FR_Error -5

#define DVBC  0,
#define DVBSX  1
#define DVBTX  2
#define ISDBT  3
#define DTMB  4
#define ISDBS  5
#define ABSS  6
#define ATSC  7
#define DVBC2  8

using namespace Noovo;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
     debug_msg("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   debug_msg("\n");
   return 0;
}

AutoScan::AutoScan(){
	
}
AutoScan::~AutoScan(){
}
NV_AS_Error_t Noovo::AutoScan::FrequencyScanOnly(unsigned int start,unsigned int end,unsigned int bandwidth){
	int ret = 0;
	_bandwidth = bandwidth;
	try{
		if(_demod == nullptr){
			ret = NV_NULL_Error;
			throw std::runtime_error("Failed to initialize the demod ptr!");
		}
		for(unsigned int i = start;i <= end;i+=(bandwidth*1000)){
			ret = _demod ->Lockchannel(i,bandwidth*1000);
			if(ret == 0){
				_frequencylist.push_back(i);
			}
		}
	}catch (std::runtime_error e){
        std::cout << "Runtime error: " << e.what()<<std::endl;
		return NV_NULL_Error;
    }
	return NV_OK;
}
NV_AS_Error_t Noovo::AutoScan::ScanSI(){
	int ret = 0;
	try{
		if(_frequencylist.empty()){
			ret = NV_FR_Error;
			throw std::runtime_error("Please Frequency all Scan first");
		}
		for(auto it =_frequencylist.begin();it!=_frequencylist.end();it++){
			if(_demod ->Lockchannel((*it),_bandwidth*1000)!=0)
				continue;
			_sitablesolution(0,(*it));
		}
	}catch (std::runtime_error e){
        std::cout << "Runtime error: " << e.what()<<std::endl;
		return ret;
    }
	return NV_OK;
}
NV_AS_Error_t Noovo::AutoScan::ScanSI(unsigned int fre,unsigned int band){
	int ret = 0;
	try{
		if(_demod ->Lockchannel(fre,band*1000)!=0)
				throw std::runtime_error("This channel cannot lock");
		ret = _sitablesolution(0,fre);
	}catch (std::runtime_error e){
        std::cout << "Runtime error: " << e.what()<<std::endl;
		return NV_FR_Error;
    }
	return ret;
}
NV_AS_Error_t Noovo::AutoScan::_sitablesolution(int dmx_no,unsigned int freq){
	DVBDmx_t *dmx = new DVBDmx_t();
	Demuxer* demux = new Demuxer();	
	int ret = 0;
	if(!dmx)
	{
		debug_msg("not enough memory\n");
		return NV_DMX_ERR_SYS;
	}
	ret |= dmx_open(dmx_no,dmx);
	ret |= dmx_set_source(dmx_no,NV_DMX_SRC_TS0);	
	ret |= _tableprocess((void*)dmx,(void*)demux,0,0);	//PAT filter
	ret |= _tableprocess((void*)dmx,(void*)demux,0x11,0x42); //SDT Filter
	for(auto it =demux->ReturnSITable().begin();it != demux -> ReturnSITable().end();it++){
		int pmt_ret = _tableprocess((void*)dmx,(void*)demux,(*it)->ReturnPMT(),0x02);
		
			_insertdatabase((void*)*it);
	}
	demux->CoutSITable();
	dmx_free_filter(dmx,0);//這邊都只用一個filter
	delete dmx;
	delete demux;
	return ret;
}
NV_AS_Error_t Noovo::AutoScan::_tableprocess(void* dmx_void,void* demux_void,const int pid,const int table_id){
	try{
		uint8_t *data_buffer = new uint8_t[1024];
		int size = 1024;
		Noovo::Demuxer *demux = (Noovo::Demuxer*)demux_void;
		DVBDmx_t *dmx=(DVBDmx_t*)dmx_void;	
		if(_demuxset((void*)dmx,pid,table_id) != 0 )
			throw std::runtime_error("Set sec error");
		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();//設定計時器 
		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		while(std::chrono::duration_cast<std::chrono::duration<int>>(t2-t1).count()<20){ //設定20秒嘗試否則放棄
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			t2 = std::chrono::steady_clock::now();
			memset(data_buffer,0,1024);
			if(_dataprocess((void*)dmx,&size,data_buffer)!=0)
				continue;
			if(demux->DoTask(data_buffer,size)!=0)
				continue;
			break;
		}
	}catch (std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
	}
	return 0;
}
NV_AS_Error_t Noovo::AutoScan::_demuxset(void* dmx_void,const int pid,const int table_id){
	int ret = 0;
	DVBDmx_t *dmx=(DVBDmx_t*)dmx_void;
	if(dmx->fd[0]==-1)
		ret |=  dmx_alloc_filter(dmx,0);
	ret |=  dmx_enable_filter(dmx,0,NV_FALSE);
	ret |=  dvb_set_sec_filter(dmx,0,pid,table_id);
	ret |=  dmx_enable_filter(dmx,0,NV_TRUE);
	return ret;
}
NV_AS_Error_t Noovo::AutoScan::_dataprocess(void* dmx_void,int* size,uint8_t* buf){
	int ret = 0;
	try{
		DVBDmx_t *dmx=(DVBDmx_t*)dmx_void;
		NV_DMX_FilterMask_t mask;
		NV_DMX_FILTER_MASK_CLEAR(&mask);
		ret = dmx_poll(dmx, &mask, DMX_POLL_TIMEOUT);
		if(ret < 0){
				throw std::runtime_error("Poll runtime no data ");
		}
		ret = dmx_read(dmx, 0,buf, size);
		if(ret < 0){
				throw std::runtime_error("Read failed!");
		}
	}catch (std::runtime_error e){
         std::cout << "Runtime error: ret "<< ret  << e.what()<<std::endl;
	}
	return ret;
}
NV_AS_Error_t Noovo::AutoScan::InitDvbt(AVLDemod* demod){
	try{
		if(demod == nullptr)
			throw std::runtime_error("Failed to initialize the demod ptr!");
		_demod = demod;
		if(_demod->Init(_chipid)!=0)
			throw std::runtime_error("Init failed!");
		if(_demod ->SetDvbsys(DVBTX)!=0)
			throw std::runtime_error("Set Dvbt failed!");
		
	}catch (std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
		 return NV_NULL_Error;
    }
	return NV_OK;	
}
NV_AS_Error_t Noovo::AutoScan::InitDatabase(std::string db_path){
	char *zErrMsg = nullptr;
	try{
		int ret = 0;
   		ret = sqlite3_open(db_path.c_str(), &_db);		
		if( ret ){
			throw std::runtime_error("Failed to initialize the database ! ");
		}
		const char *sql = "CREATE TABLE IF NOT EXISTS Channel" \
					"( Program INT PRIMARY KEY NOT NULL," \
					"PMT INT NOT NULL," \
					"Server CHAR(20)," \
					"ServerProvider CHAR(20)," \
					"VIDEO INT NOT NULL," \
					"AUDIO INT NOT NULL);";
		ret = sqlite3_exec(_db, sql, callback, 0, &zErrMsg);
		if( ret != SQLITE_OK ){
			throw std::runtime_error(zErrMsg);
		}
		if(zErrMsg){
			sqlite3_free(zErrMsg);
		}
		return NV_OK;
	}catch (std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
		 if(zErrMsg){
			 sqlite3_free(zErrMsg);
		 }
		 return NV_DB_Error;
    }	

}
NV_AS_Error_t Noovo::AutoScan::CloseDatabase(){
	if(_db){
		int ret =sqlite3_close(_db);
		return ret;
	}else
		return -1;
}

void Noovo::AutoScan::CheckFreqLiest(){
	for(auto it =_frequencylist.begin();it!=_frequencylist.end();it++){
		std::cout << *it <<std::endl;
	}
}
NV_AS_Error_t Noovo::AutoScan::_insertdatabase(void* table_void){
	char *zErrMsg = nullptr;
	try{
	SITable *table = (SITable*)table_void;
	
	char sql[256];
	memset(sql,0,256);
	int ret =snprintf(sql,256, "INSERT OR REPLACE INTO Channel" \
					"(Program,PMT,Server,ServerProvider,VIDEO,AUDIO)" \
					"VALUES ( %d , %d , \'%s\' , \' %s \' ,%d,%d);" \
					,table->ReturnProgram(),table->ReturnPMT(),table->ReturnServer().c_str(),table->ReturnServerProvider().c_str(), \
					table->RetrunVideoPid(),table->RetrunAudioPid());
	if( ret < 0 ){
			throw std::runtime_error("The sql command error");
	}
	debug_msg("%s\n",sql);
	ret = sqlite3_exec(_db, sql, callback, 0, &zErrMsg);		
	if( ret != SQLITE_OK ){
			throw std::runtime_error(zErrMsg);
	}
	if(zErrMsg){
		sqlite3_free(zErrMsg);
	}
	return 0;
	}catch (std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
		 if(zErrMsg){
			 sqlite3_free(zErrMsg);
		 }
		 return NV_DB_Error;
	}	
}