#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
/*add for config define for linux dvb *.h*/

#include <linux/dvb/dmx.h>

#include "noovo_dmx.h"

NV_ERROR_t dmx_open(int dev_no,DVBDmx_t *dmx)
{
	int i;
	snprintf(dmx->dev_name, sizeof(dmx->dev_name), "/dev/dvb/adapter0/demux%d", dev_no);
	nv_dmx_debug("%s open : dmx->dev_name\n",dmx->dev_name);
	for(i=0; i<DMX_FILTER_COUNT; i++)
		dmx->fd[i] = -1;
	
	return NV_DMX_OK;
}


NV_ERROR_t dmx_alloc_filter(DVBDmx_t *dmx, int filter)
{
	int fd=0;
	fd = open(dmx->dev_name, O_RDWR);
    nv_dmx_debug("%s dmx ->name ",dmx->dev_name);
	if(fd==-1)
	{
		nv_dmx_debug( "cannot open %s", dmx->dev_name);
		return NV_DMX_ERR_CANNOT_OPEN_DEV;
	}
	dmx->fd[filter] = fd;
	return NV_DMX_OK;
}

NV_ERROR_t  dmx_free_filter(DVBDmx_t *dmx, int filter)
{	
	int fd = dmx->fd[filter];
	close(fd);
	dmx->fd[filter] = -1;	
	return NV_DMX_OK;
}

NV_ERROR_t dvb_set_sec_filter(DVBDmx_t *dmx, int filter,const int pid ,const int table_id)
{
	struct dmx_sct_filter_params p;
	int fd = dmx->fd[filter];
	int ret;
	memset(&p,0,sizeof(p));
	p.pid = pid;
	p.filter.filter[0] = table_id;
	p.filter.mask[0]   = 0xff;
	p.flags |= DMX_CHECK_CRC;
	ret = ioctl(fd, DMX_SET_FILTER, &p);
	if(ret < 0)
	{
		nv_dmx_debug("set section filter failed ");
		return NV_DMX_ERR_SYS;
	}
	return NV_DMX_OK;
}

NV_ERROR_t  dmx_set_pes_filter(DVBDmx_t *dmx,int filter,const int pid)
{
	int fd = dmx->fd[filter];
	int ret;
 	struct dmx_pes_filter_params pparam;
	memset(&pparam,0,sizeof(pparam));
	pparam.pid = pid;
	pparam.input = DMX_IN_FRONTEND;
	pparam.output = DMX_OUT_TS_TAP;
	pparam.pes_type = DMX_PES_OTHER;
	fcntl(fd,F_SETFL,O_NONBLOCK);
	ret = ioctl(fd, DMX_SET_PES_FILTER, &pparam);
	if(ret==-1)
	{
		printf("set section filter failed ");
		return NV_DMX_ERR_SYS;
	}	
	return NV_DMX_OK;
}

NV_ERROR_t dmx_enable_filter(DVBDmx_t *dmx, int filter, NV_BOOL_t enable)
{
    int fd = dmx->fd[filter];
	int ret;
	if(enable)
		ret = ioctl(fd, DMX_START, 0);
	else
		ret = ioctl(fd, DMX_STOP, 0);
	if(ret==-1)
	{
		nv_dmx_debug("start filter failed ");
		return NV_DMX_ERR_SYS;
	}
	return NV_DMX_OK;
}

NV_ERROR_t dmx_set_buf_size(DVBDmx_t *dmx, int filter, int size)
{
	int fd = dmx->fd[filter];
	int ret;
	ret = ioctl(fd, DMX_SET_BUFFER_SIZE, size);
	if(ret < 0)
	{
		nv_dmx_debug("set buffer size failed ");
		return NV_DMX_ERR_SYS;
	}	
	return NV_DMX_OK;
}

NV_ERROR_t  dmx_poll(DVBDmx_t *dmx, NV_DMX_FilterMask_t *mask, int timeout)
{
	struct pollfd fds[DMX_FILTER_COUNT];
	int fids[DMX_FILTER_COUNT];
	int i, cnt = 0, ret;	
	for(i=0; i<DMX_FILTER_COUNT; i++)
	{
		if(dmx->fd[i]!=-1)
		{
			fds[cnt].events = POLLIN|POLLERR;
			fds[cnt].fd     = dmx->fd[i];
			fids[cnt] = i;
			cnt++;
		}
	}
	if(!cnt)
		return NV_DMX_ERR_NOT_ALLOCATED;	
	ret = poll(fds, cnt, timeout);
	if(ret<=0)
	{
		return NV_DMX_ERR_TIMEOUT;
	}	
	for(i=0; i<cnt; i++)
	{
		if(fds[i].revents&(POLLIN|POLLERR))
		{
			NV_DMX_FILTER_MASK_SET(mask, fids[i]);
		}
	}
	return NV_DMX_OK;
}


NV_ERROR_t dmx_read(DVBDmx_t *dmx, int filter, uint8_t *buf, int *size)
{
	int fd =  dmx->fd[filter];
	int len = *size;
	int ret;
	struct pollfd pfd;
	if(fd < 0)
		return NV_DMX_ERR_NOT_ALLOCATED;
	
	pfd.events = POLLIN|POLLERR;
	pfd.fd     = fd;
	
	ret = poll(&pfd, 1, 0);
	if(ret <=0)
		return NV_DMX_ERR_NO_DATA;
	ret = read(fd, buf, len);
	if(ret<=0)
	{
		if(errno==ETIMEDOUT)
			return NV_DMX_ERR_TIMEOUT;
		return NV_DMX_ERR_SYS;
	}
	*size = ret;
	return NV_DMX_OK;
}


NV_ERROR_t dmx_set_source(int dev_no,NV_DMX_Source_t src)
{
	char buf[32];
	char *cmd;
	int fd =0,len=0,ret=0;
	snprintf(buf, sizeof(buf), "/sys/class/stb/demux%d_source", dev_no);
	
	switch(src)
	{
		case NV_DMX_SRC_TS0:
			cmd = "ts0";
		break;
		case NV_DMX_SRC_TS1:
			cmd = "ts1";
		break;

		case NV_DMX_SRC_HIU:
			cmd = "hiu";
		break;
		default:
			nv_dmx_debug("do not support demux source %d", src);
		    return NV_DMX_ERR_INVALID_ARG;
	}
	
	fd = open(buf, O_WRONLY);
	if(fd==-1)
	{
		nv_dmx_debug("cannot open file ");
		return NV_DMX_ERR_INVALID_ARG;
	}
	len = strlen(cmd);
	ret = write(fd, cmd, len);
	if(ret!=len)
	{
		nv_dmx_debug("write failed file");
		close(fd);
		return NV_DMX_ERR_SRC_ERROR;
	}
	close(fd);
	return NV_DMX_OK;
}

