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
		Dt_Error_t Init(unsigned int fre, unsigned int band,std::vector<std::pair<int, int>>& pids,SYS_t model );
		void MainThread(std::vector<std::pair<int, int>> pids);
		void DVRThread();
	private:
		void _livethread(std::vector<std::pair<int, int>> pids);
		void _dvrthread();
		void _testthread(int(*read)(void *a, uint8_t *b, int c));
		Dvrlock* _dvr = nullptr;
	};

}
#endif