#ifndef __DVRLOCK_H__
#define __DVRLOCK_H__

#include <vector>

extern "C"{
    #include "noovo_dmx.h"
    #include "noovo_dvr.h"
};
#include "Datastock.hpp"
#include "Transcoder.h"
typedef const enum SYS{ DVBSX =1,DVBTX=2} SYS_t;
typedef int DV_Error_t ;
namespace Noovo {
	class Dvrlock {
	public:		
		Dvrlock();
		~Dvrlock();
        DV_Error_t InitDvr(unsigned int fre ,unsigned int bandwidth,SYS_t model);
        DV_Error_t Filterpids(std::vector<std::pair<int, int>> &pids);
        DV_Error_t ReadData( Datastock<uint8_t>*& datastock );		
	private:
        DV_Error_t _dmxdvrprocess();
        DVBDmx_t* _dmx=nullptr;
        Transcoder *_trans = nullptr;
        int _dvr_fb =-1,_filter =0,_chipid=0;
	};
}
#endif