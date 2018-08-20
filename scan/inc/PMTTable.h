#ifndef __PMTTABLE_H__
#define __PMTTABLE_H__
#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include "DvbtTable.h"
typedef int NV_PMT_Error_t;
namespace Noovo {
	class PMTTable : public  DvbtTable
	{
		public:
			PMTTable(std::vector<SITable*>* table_list);
			virtual ~PMTTable();
			virtual bool IsReadable();
			virtual int Parse( uint8_t *metadata,int length);
		private:
			NV_PMT_Error_t _decodepmt( uint8_t *metadata,int length);
			std::vector<SITable*>* _sitable_list = nullptr;
			NV_PMT_Error_t _findSITable(int program_id,SITable** table);
	};
};
#endif
