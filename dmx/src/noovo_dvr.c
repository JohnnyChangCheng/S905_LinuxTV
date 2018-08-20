#include <limits.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <linux/dvb/dmx.h>

#include "noovo_dvr.h"

/*add for config define for linux dvb *.h*/
NV_ERROR_t dvr_open(int dev_no,int *file){
	char dev_name[32];
	*file = 0;
	snprintf(dev_name, sizeof(dev_name), "/dev/dvb/adapter0/dvr%d",dev_no);
	*file = open(dev_name, O_RDONLY);
	if (*file == -1)
	{
		nv_dvr_debug("cannot open\n");
		return 	NV_DVR_ERR_SYS;
	}
	fcntl(*file, F_SETFL, fcntl(*file, F_GETFL, 0) | O_NONBLOCK, 0);
	return NV_DVR_OK;
}
NV_ERROR_t dvr_close(int fd)
{
	close(fd);
	return NV_DVR_OK;
}
NV_ERROR_t dvr_set_buf_size(int file, int size)
{
	int fd = file;
	int ret;
	ret = ioctl(fd, DMX_SET_BUFFER_SIZE, size);
	if(ret==-1)
	{
		nv_dvr_debug("set buffer size failed\n");
		return NV_DVR_ERR_SYS;
	}
	
	return NV_DVR_OK;
}
NV_ERROR_t dvr_poll(int file, int timeout)
{
	struct pollfd fds;
	int ret = 0;
	memset(&fds,0,sizeof(struct pollfd));
	fds.events = POLLIN|POLLERR;
	fds.fd     =  file;

	ret = poll(&fds, 1, timeout);
	if(ret <= 0)
	{
		nv_dvr_debug("set dvr poll failed\n");
		return NV_DVR_ERR_SYS;
	}
	
	return NV_DVR_OK;
}

NV_ERROR_t dvr_read(int file, uint8_t *buf, int *size)
{
	int fd = file;
	int len = *size;
	int ret;
	if(fd==-1)
		return NV_DVR_ERR_NOT_ALLOCATED;
	
	ret = read(fd, buf, len);
	if(ret<=0)
	{
		if(errno==ETIMEDOUT)
			return NV_DVR_ERR_TIMEOUT;
		nv_dvr_debug( "read dvr failed ");
		return NV_DVR_ERR_SYS;
	}
	*size = ret;
	return NV_DVR_OK;
}

NV_ERROR_t dvr_set_source(int dev_no,NV_DVR_Source_t src)
{
	char buf[64];
	char *cmd;
	int fd, len, ret;
	snprintf(buf, sizeof(buf), "/sys/class/stb/asyncfifo%d_source", src);
	
	switch(dev_no)
	{
		case 0:
			cmd = "dmx0";
		break;
		case 1:
			cmd = "dmx1";
		break;
		default:
			nv_dvr_debug( "do not support demux no");
		return NV_DVR_ERR_SYS;
	}
	fd = open(buf, O_WRONLY);
	if(fd==-1)
	{
		nv_dvr_debug("cannot open file ");
		return NV_DVR_ERR_SYS;
	}
	len = strlen(cmd);
	ret = write(fd, cmd, len);
	if(ret!=len)
	{
		nv_dvr_debug("write failed file");
		close(fd);
		return NV_DVR_SET_SRC_ERROR;
	}
	close(fd);
	return NV_DVR_OK;
}
	