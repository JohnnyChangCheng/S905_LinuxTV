#ifndef __AVLDEMOD_H__
#define __AVLDEMOD_H__

#include <memory>
#include <mutex>

typedef int NV_AVL_Error_t;
namespace Noovo {
	class AVLDemod{
	public:
		static AVLDemod* GetInstance();
		NV_AVL_Error_t Init(int chip);
		NV_AVL_Error_t SetDvbsys(unsigned int dvbsystem);
		unsigned int GetDvbSystem();
		NV_AVL_Error_t Lockchannel(unsigned int frequency,unsigned int bandwidth);
		NV_AVL_Error_t GetSQI(int &sqi);
		NV_AVL_Error_t GetSNR(float &snr);
		NV_AVL_Error_t GetSSI(int &ssi);
		bool ReturnWhetherInit();
	    ~AVLDemod();
	private:
		AVLDemod();
		unsigned int _dvbsystem = 0,_frequency;
		bool _init=false;
		static std::unique_ptr<AVLDemod> _instance;
		static std::once_flag _once_flag;
	};
};
#endif
