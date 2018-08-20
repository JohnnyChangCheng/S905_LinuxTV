# S905-LinuxTV-Interface  
LinuxTV Interface   <br />
- Use Linux TV interface dvb/adapter0/dmx0 dvr0 to record TS File.  <br />
   Check `live/src/Dvrlock.cpp`  <br />
- Sqlite database record TS table such as : PAT PMT SDT  <br />
   Check scan file and `scan/src/autoscan.cpp`.   <br />
- This program is built on Linux TV Interface so check your linux whether `/dev/dvb` exist   <br />
- ToolChain set is in `CMakeLists.txt`   <br />