# 指令编译器和选项
ARM =1
ifeq ($(ARM), 1)
CC=/home/johnny/toolchain/gcc/linux-x86/aarch64/gcc-linaro-aarch64-linux-gnu-4.9/bin/aarch64-linux-gnu-g++
endif

ifeq ($(ARM), 1)
CFLAGS+=-DARM
endif
CFLAGS+= -g -Wall -std=c++11
# 目标文件
TARGET = ../libEnc.a
LDFLAGS += -lm -lrt

INC = -I./include -I./enc/gx_enc_fast -I.

testApi:libvpcodec.o AML_HWEncoder.o enc_api.o gxvenclib_fast.o rate_control_gx_fast.o parser.o test.o
	$(CC) -o $@ $(CFLAGS) $(INC)  $(LDFLAGS) $^

test.o: test.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@
libvpcodec.o: libvpcodec.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

AML_HWEncoder.o: AML_HWEncoder.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

enc_api.o: enc_api.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

gxvenclib_fast.o: ./enc/gx_enc_fast/gxvenclib_fast.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

rate_control_gx_fast.o: ./enc/gx_enc_fast/rate_control_gx_fast.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

parser.o: ./enc/gx_enc_fast/parser.cpp
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)