# S905-LinuxTV-Interface  <br />
LinuxTV Interface  <br />
- Use Linux TV interface dvb/adapter0/dmx0 dvr0 to record TS File. <br />
   Check `live/src/Dvbtlive.cpp` <br />
- Sqlite database record TS table such as : PAT PMT SDT <br />
   Check scan file and `scan/src/autoscan.cpp`. <br />
- Put libavcodec.a ... (ffmpeg libraries) and sqlite(libraries) <br />
- Use scan to get channel database and use dvbtlive.cpp to live it. <br />
- This program is follow this flow (MXL608->AVLDemod->HLS prtoco) <br />