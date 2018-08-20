/*
 *           Copyright 2007-2014 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */


//using namespace System;
#include "AVL_Tuner.h"
#include "MxL608.h"
#include "ExtAV2011.h"
#include "AVL_Demod.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>

AVL_ErrorCode A8293_Control( AVL_uint32 LNB_LEVEL);

void SetChipID(int chip);

void AVL_Check_LockStatus(AVL_uchar *pLockFlag);

AVL_ErrorCode DVB_Sx_tuner_Lock(AVL_uint32 Freq_Khz,AVL_uint32 Symbol_Khz);

AVL_ErrorCode DVB_C_tuner_Lock(AVL_uint32 Freq_Khz,AVL_uint32 BandWidth_Khz);

AVL_ErrorCode DVB_Tx_tuner_Lock(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz);

AVL_ErrorCode ISDBT_tuner_Lock(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz);

AVL_ErrorCode AVL_Init(void);

AVL_ErrorCode AVL_LockChannel_DVBSx(AVL_uint32 Freq_Khz,AVL_uint32 Symbol_Khz) ;

AVL_ErrorCode AVL_LockChannel_ISDBT(AVL_uint32 Freq_Khz, AVL_uint16 BandWidth_Khz);

AVL_DVBTxBandWidth Convert2DemodBand(AVL_uint16 BandWidth_Khz);

AVL_ErrorCode AVL_LockChannel_T(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz, AVL_int32 DVBT_layer_info);

AVL_ErrorCode AVL_LockChannel_T2(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz, AVL_uchar T2_Profile, AVL_int32 PLP_ID);

AVL_ErrorCode AVL_LockChannel_DVBC(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz);

void Channels_Filter_and_Adjust(struct AVL_ChannelInfo *Ch_list_valid, AVL_uchar *TP_No_valid, struct AVL_ChannelInfo *Ch_list_Temp, AVL_uchar TP_No_Temp);

void AVL_Blindscan_init(void);

void AVL_BlindScanProcess(AVL_uint16 centerFreq_Mhz,AVL_uint16 *pnextCenterFreq_Mhz,AVL_ChannelInfo *pChannelList,AVL_uchar *Find_TP_num, AVL_int32 uiChipNo);

void BlindScanExamples();

void  ISDBT_EWBSCheck_Example(void);

AVL_ErrorCode AVL_SX_22K_Control(AVL_uchar OnOff);

AVL_ErrorCode AVL_SX_SetToneOut(AVL_uchar ucTone);

void AVL_SX_DiseqcSendCmd(AVL_puchar pCmd, AVL_uchar CmdSize);

void DiseqcExamples(void);

AVL_ErrorCode AVL_SetWorkMode(AVL_DemodMode eDemodWorkMode);

AVL_ErrorCode AVL_ScanChannel_Tx(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz);

AVL_ErrorCode get_SSI_info(int &ssi);

AVL_ErrorCode get_SQI_info(int &sqi);

AVL_ErrorCode get_SNR_info(float &snr);

AVL_ErrorCode get_PER_info(float &per);

void DVB_Tx_locksignal_example(AVL_uint32 Freq_Khz,AVL_uint16 BandWidth_Khz);

void AVL_PrintVersion(void);

int AVL_DVBTxChannelScan_example(int start_khz,int end_khz,int bandwidth_khz);


/*
    ||=====                                                    ====            ====
    ||       \\                                     ||         ||\\            //||
    ||        ||                                    ||         || \\          // ||
    ||       //                                     ||         ||  \\        //  ||
    ||=====         //===\\     //===\\       //====||         ||   \\      //   ||   //===\\ 
    ||       \\    ||=====//   ||     ||     ||     ||         ||    \\    //    ||  ||=====//
    ||        \\   ||          ||     ||     ||     ||         ||     \\  //     ||  ||
    ||         \\   \\====//    \\===//\\     \\===//\\        ||      \\//      ||   \\====//
*/
//****************************************************************************//
//****************************************************************************//
//       User can ignore the detailed implementation in the above functions.  //
//       Just need to invoke above functions as main() function does.         //
//****************************************************************************//
//****************************************************************************//

