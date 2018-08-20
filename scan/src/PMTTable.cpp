#include "PMTTable.h"

#include <stdexcept> 

#include "ScanLog.h"

using namespace Noovo;

#define PMT_START_BYTE 12
#define NV_PMT_NULL -2
#define NV_PMT_DECODE_ERROR -1
#define NV_PMT_DECODE_SUCCESS 0
#define NO_PROGRAM -3
#define NV_PMT_PROGRAM_FIND 5
static std::string GetTypeName(uint8_t type)
{
  switch (type)
    {
    case 0x00:
      return "Reserved";
    case 0x01:
      return "ISO/IEC 11172 Video";
    case 0x02:
      return "ISO/IEC 13818-2 Video";
    case 0x03:
      return "ISO/IEC 11172 Audio";
    case 0x04:
      return "ISO/IEC 13818-3 Audio";
    case 0x05:
      return "ISO/IEC 13818-1 Private Section";
    case 0x06:
      return "ISO/IEC 13818-1 Private PES data packets";
    case 0x07:
      return "ISO/IEC 13522 MHEG";
    case 0x08:
      return "ISO/IEC 13818-1 Annex A DSM CC";
    case 0x09:
      return "H222.1";
    case 0x0A:
      return "ISO/IEC 13818-6 type A";
    case 0x0B:
      return "ISO/IEC 13818-6 type B";
    case 0x0C:
      return "ISO/IEC 13818-6 type C";
    case 0x0D:
      return "ISO/IEC 13818-6 type D";
    case 0x0E:
      return "ISO/IEC 13818-1 auxillary";
    case 0x11:
      return "MP3";
      //return "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1";
    case 0x1b:
      return "H.264";
      //return "AVC video stream as defined in ITU-T Rec. H.264 | ISO/IEC 14496-10 Video (h.264)";
    default:
      if (type < 0x80)
	return "ISO/IEC 13818-1 reserved";
      else
    return "User Private";
    }
}

PMTTable::PMTTable(std::vector<SITable*>* table_list): _sitable_list(table_list){

}
PMTTable::~PMTTable(){
}
int Noovo::PMTTable::Parse(uint8_t *metadata,int length)
{        
    if(metadata == nullptr)
        return NV_PMT_NULL;
    int ret = _decodepmt(metadata,length);
        return ret;
    
}
NV_PMT_Error_t Noovo::PMTTable::_decodepmt(uint8_t *metadata,int length)
{  
    try{
        int video(0),audio(0);
        SITable* table=nullptr;
        int sec_len =((metadata[1]&0x0f)<<8)+metadata[2];//Check PAT section format
        if(sec_len +3 != length) //3 btyes : table id and section length
            throw std::runtime_error("Section length error");
        int program_id = ((metadata[3])<<8)+metadata[4];
        debug_msg("Section length  %d Program %d in %s\n",sec_len,program_id,__FILE__);
        if( _findSITable(program_id,&table)<0)
            throw std::runtime_error("No this program id");
        for(int i = 12;i <length-4;i+=5){
            int stream_type = metadata[i];
            int pid =  ((metadata[i+1]&0x1f)<<8)+metadata[i+2];
            debug_msg("PID  %d Stream Type %d in %s\n",pid,stream_type,__FILE__);
            if((stream_type == 0x01 ||stream_type == 0x02 )||stream_type == 0x1b ){
                video =   pid;  
            }else if((stream_type == 0x03 ||stream_type == 0x04 )||stream_type == 0x11){
                audio = pid;    
            }
        }
        table->SetStream(video,audio); 
        return NV_PMT_DECODE_SUCCESS;
    }catch (std::runtime_error e) {	
        std::cout << "Runtime error: " << e.what()<<std::endl;
        return NV_PMT_DECODE_ERROR;
	  } 
}
bool Noovo::PMTTable::IsReadable()
{   
    for(auto it = (*_sitable_list).begin();it!= (*_sitable_list).end();it++){
        if(!(*it)->RetrunPMTDone()){ 
            return false;
        }
    }
    return true;
}
NV_PMT_Error_t Noovo::PMTTable::_findSITable(int program_id,SITable **table){ 
    for(auto it = (*_sitable_list).begin();it!= (*_sitable_list).end();it++){
        if((*it)->ReturnProgram()==program_id){
          (*table) =(*it);
          return NV_PMT_PROGRAM_FIND;
        }
    }
    return NO_PROGRAM;
}