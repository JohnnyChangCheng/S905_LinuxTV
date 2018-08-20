#ifndef __AUTOSCAN_H__
#define __AUTOSCAN_H__

#include <vector>
#include <string>

extern "C"{
    #include <sqlite3.h>
}
#include "AVLDemod.h"

typedef int NV_AS_Error_t ;
namespace Noovo {
	class AutoScan {
        public:
            AutoScan();
            ~AutoScan();
            NV_AS_Error_t FrequencyScanOnly(unsigned int start,unsigned int end,unsigned int bandwidth);
            NV_AS_Error_t InitDvbt(AVLDemod* demod);
            NV_AS_Error_t InitDatabase(std::string db_path);
            NV_AS_Error_t CloseDatabase();
            NV_AS_Error_t ScanSI();
            NV_AS_Error_t ScanSI(unsigned int fre,unsigned int band);
            void CheckFreqLiest();
        private:
            NV_AS_Error_t _tableprocess(void* dmx_void,void* demux_void,const int pid,const int table_id);
            NV_AS_Error_t _demuxset(void* dmx_void,const int pid,const int table_id);
            NV_AS_Error_t _sitablesolution(int dmx_no,unsigned int freq);
            NV_AS_Error_t _dataprocess(void* dmx_void,int* size,uint8_t* buf);
            NV_AS_Error_t _insertdatabase(void* table_void);
            AVLDemod* _demod=nullptr;
            std::vector<unsigned int> _frequencylist;
            unsigned int _bandwidth =0;      
            sqlite3 *_db=nullptr;   
            int _chipid=0;
	};
};

#endif
