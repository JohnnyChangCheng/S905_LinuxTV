#ifndef __DVBTLIVE_H__
#define __DVBTLIVE_H__

#include <vector>
#include <queue>
#include <thread>

#include "Transcoder.h"
#include "Dvrlock.h"
#include "Datastock.hpp"

typedef int Dt_Error_t;

namespace Noovo {
	class Dvbtlive {
	public:
		Dvbtlive();
		~Dvbtlive();
		Dt_Error_t Init(unsigned int fre, unsigned int band,std::vector<std::pair<int, int>>& pids );
		void MainThread(std::vector<std::pair<int, int>> pids);
	private:
		void _livethread(std::vector<std::pair<int, int>> pids);
		void  _dvrthread();
		Dvrlock* _dvr = nullptr;
	};

}
#endif