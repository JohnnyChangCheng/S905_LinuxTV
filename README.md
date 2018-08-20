# S905-LinuxTV-Interface  \
LinuxTV Interface  \
1. Use Linux TV interface dvb/adapter0/dmx0 dvr0 to record TS File. \
   Check live/src/Dvbtlive.cpp \
2. Sqlite database record TS table such as : PAT PMT SDT \
   Check scan file and scan/src/autoscan.cpp. \
3. Put libavcodec.a ... (ffmpeg libraries) and sqlite(libraries) \
4. Use scan to get channel database and use dvbtlive.cpp to live it. \
5. This program is follow this flow (MXL608->AVLDemod->HLS prtoco)