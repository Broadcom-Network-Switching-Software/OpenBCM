

#ifndef __KAPS_INIT_H
#define __KAPS_INIT_H



#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"

#ifdef __cplusplus
extern "C" {
#endif





enum kaps_device_type {
    KAPS_DEVICE_UNKNOWN,       
    KAPS_DEVICE_KAPS,          
    KAPS_DEVICE_INVALID        
};



enum kaps_device_flags {
    KAPS_DEVICE_DEFAULT = 0,           
    KAPS_DEVICE_SMT = 1,               
    KAPS_DEVICE_DUAL_PORT = 2,         
    KAPS_DEVICE_NBO = 4,               
    KAPS_DEVICE_SUPPRESS_INDEX = 8,    
    KAPS_DEVICE_ISSU = 16,             
    KAPS_DEVICE_CASCADE = 32,          
    KAPS_DEVICE_SKIP_INIT = 64,        
    KAPS_DEVICE_SUPPRESS_PARITY = 128, 
    KAPS_DEVICE_QUAD_PORT = 256,       
    KAPS_DEVICE_HST_DIG_LPBK = 512,    
    KAPS_DEVICE_THREAD_SAFETY = 1024   
};







struct kaps_device_config {

    

    uint32_t num_devices;


};
















#ifdef __cplusplus
}
#endif
#endif 
