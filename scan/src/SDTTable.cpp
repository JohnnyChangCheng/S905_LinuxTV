#include "SDTTable.h"
extern "C" {
	#include "noovo_iconv.h"
	#include <memory.h>
	#include <string.h>
};

#include <stdexcept> 

#include "ScanLog.h"

#define NV_SDT_NULL -3
#define NV_SDT_PROGRAM_FIND 3
#define NO_PROGRAM -3
using namespace Noovo;

SDTTable::SDTTable(std::vector<SITable*>* table_list): _sitable_list(table_list)
{
}
SDTTable::~SDTTable()
{

}
NV_SDT_Error_t Noovo::SDTTable::Parse(uint8_t *metadata,int length)
{
	if(metadata == nullptr)
		return NV_SDT_NULL;
	 _decodesdt(metadata,length);
	return 0;
}
bool Noovo::SDTTable::IsReadable()
{
	return (_ready);
}
NV_SDT_Error_t Noovo::SDTTable::_decodesdt(uint8_t *metadata,int length) {

		int sec_len =((metadata[1]&0x0f)<<8)+metadata[2];//Check SDT section length
		if(sec_len +3 != length) //3 btyes : table id and section length
			throw std::runtime_error("Section length error");		
		debug_msg("Section length  %d in %s\n",sec_len,__FILE__);
		_process_descriptor(metadata+11,length-11-4);
}

NV_SDT_Error_t Noovo::SDTTable::_process_descriptor(uint8_t* sub_data,int length)
{
	int i = 0;
	while(i <length){
		SITable *table=nullptr;
		int service_id = (sub_data[i]<<8)+sub_data[i+1];
		if(_findSITable(service_id,&table)<0)
			throw std::runtime_error("Out program id");
		int descriptor_len = ((sub_data[i+3] & 0x1f)<<8 )+sub_data[i+4];
		debug_msg("Service id %d loop length   %d in %s\n",service_id,descriptor_len,__FILE__);
		_dumpdescriptor(sub_data+i+5,descriptor_len,table);
		i+=4+descriptor_len+1;//跳到下一個service id
	}
}
NV_SDT_Error_t Noovo::SDTTable::_findSITable(int program_id,SITable **table){ 
    for(auto it = (*_sitable_list).begin();it!= (*_sitable_list).end();it++){
        if((*it)->ReturnProgram()==program_id){
          (*table) =(*it);
          return NV_SDT_PROGRAM_FIND;
        }
    }
    return NO_PROGRAM;
}

void Noovo::SDTTable::_dumpdescriptor(uint8_t* descriptor_data,int length,SITable *table)
{
	/*
	Table 86: Service descriptor
	Syntax                                       Number of bits                             Identifier
	service_descriptor(){
	descriptor_tag                                       8                                uimsbf
	descriptor_length                                    8                                uimsbf
	service_type                                         8                                uimsbf
	service_provider_name_length                         8                                uimsbf
	for (i=0;i<N;i++){
	char                                                 8                                uimsbf
	}
	service_name_length                                  8                                uimsbf
	for (i=0;i<N;i++){
	char                                             8                                uimsbf
	}
	}
	*/
	//libdvpsi help us filter out the descriptor tag and descriptor length , so our data begin at service type
	unsigned char service_type = descriptor_data[2];
	int service_provider_name_length = descriptor_data[3];
	unsigned char *service_provider_name = new unsigned char[service_provider_name_length];
	
	memset(service_provider_name,0,service_provider_name_length);
	memmove(service_provider_name,(descriptor_data+4),service_provider_name_length);

	int service_begin =4+service_provider_name_length ;
	int service_name_length = descriptor_data[service_begin ];
	char* service_name = new char[service_name_length];
	memset(service_name,0,service_name_length);
	memmove(service_name,(descriptor_data+1+ service_begin),service_name_length);
	char * conv = Descriptor((const void*)service_provider_name, service_provider_name_length);
	char * conv_2 = Descriptor((const void*)service_name, service_name_length);
	table->SetSDT(conv_2,conv);
	delete[] service_provider_name;
	delete[] service_name;
}