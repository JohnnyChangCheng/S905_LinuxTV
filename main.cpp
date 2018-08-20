
#include "Dvrlock.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include "memory.h"
#include "Dvbtlive.h"
#include "Transcoder.h"

using namespace Noovo;
using namespace std;

int main(void)
{
    Dvbtlive *_dvb=new Dvbtlive();
    std::vector<std::pair<int,int>> Pid = {{3041,-1}};
    _dvb->Init(557000 ,6000,Pid,DVBTX);
    _dvb->DVRThread();

}
