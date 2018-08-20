#include "PATTable.h"

#include <stdexcept> 

#include "ScanLog.h"
using namespace Noovo;
#define NV_PAT_DECODE_ERROR -1
#define NV_PAT_OK 0
#define NV_PAT_NULL -2
#define PAT_START_BYTE 8

PATTable::PATTable(std::vector<SITable*>* table_list): _sitable_list(table_list){
	
}
PATTable::~PATTable()
{
}
NV_PAT_Error_t Noovo::PATTable::Parse(uint8_t *metadata,int length)
{
	if(metadata == nullptr)
		return NV_PAT_NULL;
	int ret = _decodepat(metadata,length);
	return ret;
}
bool Noovo::PATTable::IsReadable()
{
	return _readable;
}
NV_PAT_Error_t Noovo::PATTable::_decodepat(uint8_t *metadata,int length)
{
	try{
		int sec_len =((metadata[1]&0x0f)<<8)+metadata[2];//Check PAT section length
		debug_msg("Section length  %d in %s\n",sec_len,__FILE__);
		if(sec_len +3 != length) //3 btyes : table id and section length
			throw std::runtime_error("Section length error");		
		for(int i = 8; i< (length-4);i+=4){
			int program_id = (metadata[i]<<8) +metadata[i+1];
			if(program_id==0)
				continue;
			int pmt = ((metadata[i+2]&0x1f)<<8) + metadata[i+3];
			debug_msg("Program id : %d PMT pid : %d  in %s\n",program_id,pmt,__FILE__);
			_sitableconstruct(program_id,pmt);
		}
		_readable = true;
		return NV_PAT_OK;
	}catch (std::runtime_error e) {	
		std::cout << "Runtime error: " << e.what()<<std::endl;
		_readable =false;
		return NV_PAT_DECODE_ERROR;
	}
}
void Noovo::PATTable::_sitableconstruct(int program_id,int pmt){
	SITable *sitable = new SITable(program_id,pmt);
	if(sitable == nullptr)
		throw std::runtime_error("Malloc sitable error");
	(*_sitable_list).push_back(sitable);
	return;
}	