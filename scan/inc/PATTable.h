#ifndef __PATTABLE_H__
#define __PATTABLE_H__
#include "DvbtTable.h"

#include <string>
#include <vector>
#include <iostream>

typedef int NV_PAT_Error_t;

namespace Noovo {
	class PATTable : public DvbtTable
	{
	public:
		PATTable(std::vector<SITable*>*);
		virtual ~PATTable();
		virtual bool IsReadable();
		virtual int Parse( uint8_t *metadata,int length);
	private:
		NV_PAT_Error_t _decodepat(uint8_t *metadata,int length);
		void _sitableconstruct(int program_id,int pmt);
		bool _readable=false;
		std::vector<SITable*>* _sitable_list = nullptr;
	};
};
#endif
