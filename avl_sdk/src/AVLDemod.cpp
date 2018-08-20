#include "AVLDemod.h"

#include <iostream>

#include "AVL68XX_Porting.h"
#include "ScanLog.h"
using namespace Noovo;
std::unique_ptr<AVLDemod> AVLDemod::_instance;
std::once_flag AVLDemod::_once_flag;

#define AVL_NONE_LOCK -3

AVLDemod* AVLDemod::GetInstance()
{
    std::call_once(_once_flag, [] {
        AVLDemod::_instance.reset(new AVLDemod); 
    });
    return (_instance.get());
}
AVLDemod::AVLDemod():_dvbsystem(AVL_DVBTX),_init(false)
{	                  
}
AVLDemod::~AVLDemod()
{
}
NV_AVL_Error_t AVLDemod::Init(int chip){
	int return_code = 0;
	try{
		return_code = AVL_IBSP_Initialize();//(I2C_SERVER_IP,I2C_SERVER_PORT,nullptr,0);
		if(return_code != AVL_EC_OK){
			throw std::runtime_error("Failed to initialize the BSP!"+std::to_string(__LINE__));
		}
		return_code = AVL_IBSP_Reset();
		if(return_code != AVL_EC_OK){
			throw std::runtime_error("Failed to Resed demod via BSP!"+std::to_string(__LINE__));
		}
		SetChipID(chip);
		return_code = AVL_Init();
		if(return_code != AVL_EC_OK){
			throw std::runtime_error("Failed to Init demod!"+std::to_string(__LINE__));
		}
		_init=true;
	}catch (std::runtime_error e) {
         std::cout << "Runtime error: " << e.what()<<std::endl;
    }
	return 	return_code;
}

NV_AVL_Error_t AVLDemod::SetDvbsys(unsigned int dvbsystem){
	int ret=0;
	_dvbsystem = dvbsystem;
	try{
		ret = AVL_SetWorkMode((AVL_DemodMode)dvbsystem);
		if(ret != 0){
			throw std::runtime_error(std::string("Error in Set Dvbsys:")+std::to_string(ret));	
		}
	}catch (std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
    }
	return ret;
}
unsigned int AVLDemod::GetDvbSystem(){
	return _dvbsystem;
}
NV_AVL_Error_t AVLDemod::Lockchannel(unsigned int frequency,unsigned int bandwidth){ 
    int ret=0;
	AVL_uchar nLockFlag =0;
	try{
		ret=AVL_ScanChannel_Tx(frequency, bandwidth); 
		if(ret != 0){
			throw std::runtime_error(std::string("Error in channel lock:")+std::to_string(ret));
		}
		DVB_Tx_locksignal_example(frequency,bandwidth);
		AVL_Check_LockStatus(&nLockFlag) ;
		if(nLockFlag!=1){
			ret = AVL_NONE_LOCK;
			throw std::runtime_error(std::string("Error in channel lock:")+std::to_string(__LINE__));
		}
		_frequency=frequency;
	}catch(std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
    }
	return ret;
}
NV_AVL_Error_t AVLDemod::Lockchannel_S(unsigned int frequency,unsigned int bandwidth){ 
    int ret=0;
	AVL_uchar nLockFlag =0;
	try{
		ret=AVL_LockChannel_DVBSx(frequency, bandwidth); 
		AVL_Check_LockStatus(&nLockFlag) ;
		if(nLockFlag!=1){
			ret = AVL_NONE_LOCK;
			throw std::runtime_error(std::string("Error in channel lock:")+std::to_string(__LINE__));
		}
		_frequency=frequency;
	}catch(std::runtime_error e){
         std::cout << "Runtime error: " << e.what()<<std::endl;
    }
	return ret;
}
NV_AVL_Error_t AVLDemod::GetSQI(int &sqi){
	int ret = 0;
	ret=get_SQI_info(sqi);
	return ret;
}
NV_AVL_Error_t AVLDemod::GetSNR(float &snr){
	int ret = 0;
	ret=get_SNR_info(snr);
	return ret;
}
NV_AVL_Error_t AVLDemod::GetSSI(int &ssi){
	int ret = 0;
	ret=get_SSI_info(ssi);
	return ret;
}
bool AVLDemod::ReturnWhetherInit(){
	return _init;
}