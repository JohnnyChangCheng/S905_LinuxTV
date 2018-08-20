#include "Demuxer.h"

#include <string>
extern "C"{
	#include <memory.h>
}

#include "ScanLog.h"

#define NV_DM_DATA_NULL -6
#define NV_DM_FAILED -3
#define NV_DM_OK 0
#define NV_DM_PAT_ALREADY -7

#define GET_PID(buf) ((((buf)[1] & 0x1f) << 8) + (buf)[2])
using namespace Noovo;

Demuxer::Demuxer( ) 
{
}
Demuxer::~Demuxer()
{
	for(auto it = _sitable_list.begin();it!=_sitable_list.end();it++)
		delete (*it);
}

NV_DM_Error_t Noovo::Demuxer::DoTask(uint8_t *metadata,int length)
{	
	
	try {
		if(metadata == nullptr)
			throw std::runtime_error("set the metadata pointer first!");
		if(_handlePacket(metadata,length)!=0)
			throw std::runtime_error("Handle data failed!");
	}catch (std::runtime_error e) {	
		std::cout << "Runtime error: " << e.what()<<std::endl;
		return NV_DM_FAILED;
	}
	return NV_DM_OK;
}

NV_DM_Error_t Noovo::Demuxer::_handlePacket(uint8_t *metadata,int length)
{	
	int ret(0);
	switch(metadata[0]){
		case 0x00:
			debug_msg("PAT Table decode\n");
			ret = _patInit(metadata,length);
		break;
		case 0x02:
			if(!_patready){
				throw std::runtime_error("Construct PAT first");
			}
			debug_msg("PMT Table decode\n");
			ret = _pmtInit(metadata,length);
		break;
		case 0x42:
			if(!_patready){
				throw std::runtime_error("Construct PAT first");
			}
			ret = _sdtInit(metadata,length);

		break;
		default:
			debug_msg("Table id unknown %d\n",metadata[0]);
			throw std::runtime_error("Unknown table id");
	}
	return ret;
}
NV_DM_Error_t Noovo::Demuxer::_patInit(uint8_t *metadata,int length)
{	
	if (_pat == nullptr) {
		_pat = std::make_shared<Noovo::PATTable>(&_sitable_list);
	}
	int ret = _pat->Parse(metadata,length);
	_patready = _pat->IsReadable();
	return ret;
}
NV_DM_Error_t Noovo::Demuxer::_pmtInit(uint8_t *metadata,int length)
{
	if (_pmt == nullptr) {
		_pmt = std::make_shared<Noovo::PMTTable>(&_sitable_list);
	}
	int ret = _pmt->Parse(metadata,length);
	_pmtready= _pmt->IsReadable();
	return ret;
}
NV_DM_Error_t Noovo::Demuxer::_sdtInit(uint8_t *metadata,int length)
{	
	if(_sdt==nullptr) {
		_sdt = std::make_shared<SDTTable>(&_sitable_list);
	}		
	int ret=_sdt->Parse(metadata,length);
	_sdtready = _sdt->IsReadable();
	return ret;
}
std::vector<SITable*>&  Noovo::Demuxer::ReturnSITable(){
	return _sitable_list;
}
void  Noovo::Demuxer::CoutSITable(){
 for(auto it = _sitable_list.begin();it!=_sitable_list.end();it++)
        std::cout <<"Program : "<< (*it)->ReturnProgram()<< "PMT : " \
            <<(*it)->ReturnPMT() << "  Video : "<<(*it)->RetrunVideoPid() \
                <<" Audio : "<< (*it)->RetrunAudioPid() <<" ServerProvider :"<<(*it)->ReturnServerProvider()\
				<<" Server :"<<(*it)->ReturnServer()<<std::endl;
}
bool Noovo::Demuxer::Patsdtreadable(){	
	return (_sdtready && _patready);
}