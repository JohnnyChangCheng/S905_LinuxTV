#ifndef __DEMUXER_H__
#define __DEMUXER_H__
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include "PATTable.h"
#include "SDTTable.h"
#include "PMTTable.h"

typedef enum
{
	NV_PAT_SECT = 0,                   
	NV_PMT_SECT = 0X02,                    
	NV_SDT_SECT = 0X42,                   
} NV_SECT_t;

typedef int NV_DM_Error_t ;
namespace Noovo {
	class Demuxer {
	public:		
		Demuxer();
		~Demuxer();
		NV_DM_Error_t DoTask(uint8_t* metadata,int length);
		void  CoutSITable();
		std::vector<SITable*>& ReturnSITable();
		bool Patsdtreadable();
	private:
		std::vector<SITable*> _sitable_list;
		bool _patready=false,_sdtready=false,_pmtready=false;
		std::shared_ptr<Noovo::PATTable> _pat=nullptr;
		std::shared_ptr<Noovo::SDTTable> _sdt=nullptr;
		std::shared_ptr<Noovo::PMTTable> _pmt=nullptr;
		NV_DM_Error_t _patInit(uint8_t *metadata,int length);
		NV_DM_Error_t _pmtInit(uint8_t *metadata,int length);
		NV_DM_Error_t _sdtInit(uint8_t *metadata,int length);
		NV_DM_Error_t _handlePacket(uint8_t *metadata,int length);
	};
}
#endif
