#include <stdint.h>
#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define nv_dmx_debug(_fmt...) \
	do{\
		fprintf(stderr, "NV_DMX_DEBUG:");\
		fprintf(stderr, _fmt);\
		fprintf(stderr, "LINE:%d FUNC:%s\n",__LINE__,__func__);\
	}while(0)
#else
#define nv_dmx_debug(_fmt...)
#endif

#define DMX_FILTER_COUNT      (32)
typedef int NV_ERROR_t;
#define AM_DVR_MAX_PID_COUNT  (32)
#define NV_TRUE (1)
#define DMX_POLL_TIMEOUT   (200)
#define NV_FALSE (0)

#define NV_DMX_FILTER_MASK_ISEMPTY(m)    (!(*(m)))
#define NV_DMX_FILTER_MASK_CLEAR(m)      (*(m)=0)
#define NV_DMX_FILTER_MASK_ISSET(m,i)    (*(m)&(1<<(i)))
#define NV_DMX_FILTER_MASK_SET(m,i)      (*(m)|=(1<<(i)))
typedef int NV_BOOL_t ;
typedef uint32_t NV_DMX_FilterMask_t;

enum NV_DMX_ErrorCode
{   
    NV_DMX_OK = 0,
	NV_DMX_ERR_INVALID_ARG = -1,			/**< Invalid argument*/
	NV_DMX_ERR_INVALID_DEV_NO =-2,		/**< Invalid decide number*/
	NV_DMX_ERR_BUSY=-3,                        /**< The device has already been openned*/
	NV_DMX_ERR_NOT_ALLOCATED=-4,           /**< The device has not been allocated*/
	NV_DMX_ERR_CANNOT_CREATE_THREAD=-5,    /**< Cannot create a new thread*/
	NV_DMX_ERR_CANNOT_OPEN_DEV=-6,         /**< Cannot open the device*/
	NV_DMX_ERR_NOT_SUPPORTED=-7,           /**< Not supported*/
	NV_DMX_ERR_NO_MEM=-8,                  /**< Not enough memory*/
	NV_DMX_ERR_TIMEOUT=-9,                 /**< Timeout*/
	NV_DMX_ERR_SYS=-10,                     /**< System error*/
	NV_DMX_ERR_NO_DATA=-11,                 /**< No data received*/
	NV_DMX_ERR_CANNOT_OPEN_OUTFILE=-12,		/**< Cannot open the output file*/
	NV_DMX_ERR_TOO_MANY_STRENVS=-13,		/**< PID number is too big*/
	NV_DMX_ERR_STRENV_ALREADY_ADD=-14,		/**< The elementary streNV has already been added*/
	NV_DMX_ERR_END=-15,
	NV_DMX_ERR_SRC_ERROR=-16
};

typedef struct
{
	char   dev_name[32];
	int    fd[DMX_FILTER_COUNT];
} DVBDmx_t;

typedef struct
{
	char   dev_name[32];
	int    fd[DMX_FILTER_COUNT];
} Table_t;

typedef enum
{
	NV_DMX_SRC_TS0,                    /**< TS input port 0*/
	NV_DMX_SRC_TS1,                    /**< TS input port 1*/
	NV_DMX_SRC_TS2,                    /**< TS input port 2*/
	NV_DMX_SRC_HIU                     /**< HIU input (memory)*/
} NV_DMX_Source_t;

NV_ERROR_t dmx_open(int dev_no,DVBDmx_t *dmx);
NV_ERROR_t dmx_close(DVBDmx_t *dmx);
NV_ERROR_t dmx_alloc_filter(DVBDmx_t *dmx, int filter);
NV_ERROR_t dmx_free_filter(DVBDmx_t *dmx, int filter);
NV_ERROR_t dmx_set_pes_filter(DVBDmx_t *dmx,int filter,const int pid);
NV_ERROR_t dmx_enable_filter(DVBDmx_t *dmx, int filter, NV_BOOL_t enable);
NV_ERROR_t dmx_set_buf_size(DVBDmx_t *dmx, int filter, int size);
NV_ERROR_t dmx_set_source(int dev_no,NV_DMX_Source_t src);
NV_ERROR_t dvb_set_sec_filter(DVBDmx_t *dmx, int filter,const int pid,const int table_id);
NV_ERROR_t dmx_read(DVBDmx_t *dmx, int filter, uint8_t *buf, int *size);
NV_ERROR_t dmx_poll(DVBDmx_t *dmx, NV_DMX_FilterMask_t *mask, int timeout);