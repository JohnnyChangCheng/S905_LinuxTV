#ifndef __SDTTable_H__
#define __SDTTable_H__
#include "DvbtTable.h"
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include <vector>

typedef int NV_SDT_Error_t;
namespace Noovo {
	class SDTTable : public DvbtTable
	{
	public: 
		SDTTable(std::vector<SITable*>* _table);
		virtual ~SDTTable();
		virtual bool IsReadable();
		virtual int Parse( uint8_t *metadata,int length);
	private:
		NV_SDT_Error_t _decodesdt(uint8_t *metadata,int length) ;
		NV_SDT_Error_t _process_descriptor(uint8_t* sub_data,int len);
		void _dumpdescriptor(uint8_t* descriptor_data,int length,SITable *table);
		bool _ready=false;
		std::vector<SITable*>* _sitable_list = nullptr;
		NV_SDT_Error_t _findSITable(int program_id,SITable **table);
	};
};
#endif
