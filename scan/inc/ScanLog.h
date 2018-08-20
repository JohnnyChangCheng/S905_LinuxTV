#include <string>

#define Debug

#define info_msg(args...)\
    do{\
        if(1)\
            printf(args);\
    }while(0)
#ifdef Debug  
#define debug_msg(args...)\
    do{\
        if(1)\
            fprintf(stderr,args);\
    }while(0)
#else 
#define debug_msg(args...)
#endif
