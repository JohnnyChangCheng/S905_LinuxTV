#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define nv_dvr_debug(_fmt...) \
	do{\
		fprintf(stderr, "NV_DVR_DEBUG:");\
		fprintf(stderr, _fmt);\
		fprintf(stderr, "LINE:%d FUNC:%s\n",__LINE__,__func__);\
	}while(0)
#else
#define nv_dvr_debug(_fmt...)
#endif

typedef int NV_ERROR_t;
enum NV_DVR_ErrorCode
{   
    NV_DVR_OK = 0,
	NV_DVR_ERR_INVALID_ARG = -1,			/**< Invalid argument*/
	NV_DVR_ERR_INVALID_DEV_NO =-2,		/**< Invalid decide number*/
	NV_DVR_ERR_BUSY =-3,                        /**< The device has already been openned*/
	NV_DVR_ERR_NOT_ALLOCATED=-4,           /**< The device has not been allocated*/
	NV_DVR_ERR_CANNOT_CREATE_THREAD=-5,    /**< Cannot create a new thread*/
	NV_DVR_ERR_CANNOT_OPEN_DEV=-6,         /**< Cannot open the device*/
	NV_DVR_ERR_NOT_SUPPORTED=-7,           /**< Not supported*/
	NV_DVR_ERR_NO_MEM=-8,                  /**< Not enough memory*/
	NV_DVR_ERR_TIMEOUT=-9,                 /**< Timeout*/
	NV_DVR_ERR_SYS=-10,                     /**< System error*/
	NV_DVR_ERR_NO_DATA=-11,                 /**< No data received*/
	NV_DVR_ERR_CANNOT_OPEN_OUTFILE=-12,		/**< Cannot open the output file*/
	NV_DVR_ERR_TOO_MANY_STRENVS=-13,		/**< PID number is too big*/
	NV_DVR_ERR_STRENV_ALREADY_ADD=-14,		/**< The elementary streNV has already been added*/
	NV_DVR_SET_SRC_ERROR=-15,
	NV_DVR_ERR_END=-16
};
typedef enum
{
	NV_DVR_SRC_ASYNC_FIFO0, /**< asyncfifo 0*/
	NV_DVR_SRC_ASYNC_FIFO1  /**< asyncfifo 1*/
}NV_DVR_Source_t;

NV_ERROR_t dvr_open(int dev_no,int *file);
NV_ERROR_t dvr_set_buf_size(int file, int size);
NV_ERROR_t dvr_poll(int file, int timeout);
NV_ERROR_t dvr_read(int file, uint8_t *buf, int *size);
NV_ERROR_t dvr_set_source(int dev_no,NV_DVR_Source_t src);
NV_ERROR_t dvr_close(int fd);