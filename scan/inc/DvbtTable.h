#ifndef __DVBTTABLE_H__
#define __DVBTTABLE_H__

#define HAVE_SYS_TYPES_H  //For libdvbpsi table turn up

#include <stdint.h>

#include <string>
namespace Noovo {
    class SITable{
        public:
            SITable(int program,int pmt):_program(program),_pmt(pmt),_video(-1),_audio(-1),_server("NULL"),_serverprovider("NULL")
            {
            }
            void SetStream(unsigned int vid,unsigned int aid){
                _video = vid;
                _audio = aid;
               if(_video && _audio)
                    _done=true;
            }
            void SetSDT(char* server,char* provider){
                _server = std::string(server);
                _serverprovider = std::string(provider);
            }    
            inline int ReturnProgram(){
                return _program;
            }
            inline int ReturnPMT(){
                return _pmt;
            }
            inline bool RetrunPMTDone(){
                return _done;
            }
            inline int RetrunVideoPid(){
                return _video;
            }
            inline int RetrunAudioPid(){
                return _audio;
            }
            inline std::string ReturnServer(){
                return _server;
            }
            inline std::string ReturnServerProvider(){
                return _serverprovider;
            }
        private:
            int _program,_pmt,_video,_audio;
            std::string _server,_serverprovider;
            bool _done = false;
    }  ;
	class DvbtTable {
        public:
            virtual ~DvbtTable() {};
            virtual bool IsReadable() = 0;
            virtual int Parse(uint8_t *metadata,int length) =0;
	};
};
#endif
