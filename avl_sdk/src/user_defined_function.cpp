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

// https://www.acmesystems.it/user_i2c

#include "user_defined_function.h"
#include <stdio.h>
#include <stdlib.h>
 #include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>  /* Semaphore */
#include <memory.h>
#include <pthread.h>
#include <chrono>
#include <thread>
//AA_GPIO_BITS
#define AA_GPIO_SCL         0x01        ///< Pin 1
#define  AA_GPIO_SDA        0x02        ///< Pin 3
#define AA_GPIO_MISO        0x04        ///< Pin 5
#define AA_GPIO_SCK         0x08        ///< Pin 7
#define AA_GPIO_MOSI        0x10        ///< Pin 8
#define  AA_GPIO_SS         0x20        ///< Pin 9
#define I2C_RETRIES 0x0701

#define I2C_TIMEOUT 0x0702


sem_t mutex;


const static AVL_uchar ucGPIODirMask = (AA_GPIO_MISO | AA_GPIO_MOSI | AA_GPIO_SCK) & (~AA_GPIO_SS);  //Pin9 is set up to input
const static AVL_uchar ucGPIOValueMask = AA_GPIO_MISO;  //only the reset pin output high, all others output low.
const static AVL_uchar ucGPIOResetPin = AA_GPIO_MISO;

int fd;

int __i2c_msg_write(AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  pusSize){
	int value,r;

	struct i2c_rdwr_ioctl_data data = {0};
	struct i2c_msg msg;			/*construct 2 msgs, 1 for reg addr, 1 for reg value, send together*/
	//printf(" usSlaveAddr = %x  pusSize = %d \n", (usSlaveAddr), *pusSize);

	memset(&msg, 0, sizeof(msg));
	/*write value*/
	msg.addr = usSlaveAddr;
	msg.flags = 0;  //I2C_M_NOSTART;	/*i2c_transfer will emit a stop flag, so we should send 2 msg together,
					// * and the second msg's flag=I2C_M_NOSTART, to get the right timing*/
	msg.buf = pucBuff;
	msg.len = *pusSize;

	data.msgs = &msg;
	data.nmsgs = 1;
    if(ioctl(fd, I2C_RDWR, &data) < 0) {
		printf("ioctl error: %d\n", (errno));
		close(fd);
		return 1;
	}
	
	return 0;
}


int __i2c_msg_read(AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  pusSize){

	int value,r;

	struct i2c_rdwr_ioctl_data data = {0};
	struct i2c_msg msg[1];			/*construct 2 msgs, 1 for reg addr, 1 for reg value, send together*/
	//printf(" usSlaveAddr = %x  pusSize = %d \n", (usSlaveAddr), *pusSize);

 	memset(msg, 0, sizeof(msg));
	
	msg[0].addr = usSlaveAddr;
	msg[0].flags |=  I2C_M_RD;  //write  I2C_M_RD=0x01
	msg[0].len = *pusSize;
	msg[0].buf = pucBuff;

	data.msgs = msg;
	data.nmsgs = 1;
    if(ioctl(fd, I2C_RDWR, &data) < 0) {
		printf("ioctl error: %d\n", (errno));
		close(fd);
		return 1;
	}
	return 0;
}


int __write(AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  pusSize){

	int value,i;
	
	if (ioctl(fd, I2C_SLAVE, usSlaveAddr) < 0) {
		printf("ioctl error: %d\n", (errno));
		close(fd);
		return 1;
	}

	if (write(fd, pucBuff, (size_t)*pusSize) < 0){
		printf("write error\n");
	}
	
	return 0;
}


int __read(AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  pusSize){

	int value,i;
	
	if (read(fd, pucBuff, (size_t)*pusSize) < 0){
		printf("\n read error\n");
	}
	
	return 0;
}

AVL_uint32 AVL_IBSP_Reset(void)
{
    return(0);
}

AVL_uint32 AVL_IBSP_Delay(AVL_uint32 uiDelay_ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(uiDelay_ms));
    return (0);
}

AVL_uint32 AVL_IBSP_I2C_Read(AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff, AVL_puint16 pusSize)
{
	int ret,i ;

    ret = __i2c_msg_read(usSlaveAddr, pucBuff, pusSize);

    return ret;
}

AVL_uint32 AVL_IBSP_I2C_Write(AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  pusSize)
{
	int ret,i ;

    ret = __i2c_msg_write(usSlaveAddr, pucBuff, pusSize);

    return ret;
}

AVL_uint32 AVL_IBSP_Initialize(void)
{
	fd = open("/dev/i2c-0", O_RDWR);
   // ioctl(fd, I2C_TIMEOUT, 1);/*set timeout value*/

   // ioctl(fd, I2C_RETRIES, 2);/*set retry times*/
	if (fd < 0) {
		printf("Error opening file: %d\n", (errno));
		close(fd);
		return 1;
	}

    return(0);
}

AVL_uint32 AVL_IBSP_InitSemaphore(AVL_psemaphore pSemaphore)
{
	sem_init(pSemaphore, 0, 1);
    return(0);
}

AVL_uint32 AVL_IBSP_ReleaseSemaphore(AVL_psemaphore pSemaphore)
{
	int ret = sem_post(pSemaphore);
	if(ret!=0){
		printf("\n sem_post :0x%x\n", (unsigned int *)pSemaphore);
		printf("\n sem_post :%d\n", *(unsigned int *)pSemaphore);		
		printf("\n sem_post state :%d\n", ret);		
	}
    return ret;
}

AVL_uint32 AVL_IBSP_WaitSemaphore(AVL_psemaphore pSemaphore)
{
	int ret = sem_wait(pSemaphore);
	if(ret!=0){
		printf("\n sem_wait :0x%x\n", (unsigned int *)pSemaphore);
		printf("\n sem_wait :%d\n", *(unsigned int *)pSemaphore);		
		printf("\n sem_wait state :%d\n", ret);		
	}
    return ret;
}

AVL_uint32 AVL_IBSP_Dispose(void)
{
	close(fd);
    return(0);
}


