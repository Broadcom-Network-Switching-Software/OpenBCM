

#ifndef __KAPS_JR1_INIT_H
#define __KAPS_JR1_INIT_H



#include <stdint.h>
#include <stdio.h>

#include "kaps_jr1_errors.h"

#ifdef __cplusplus
extern "C" {
#endif





enum kaps_jr1_device_type {
    KAPS_JR1_DEVICE_UNKNOWN,       
    KAPS_JR1_DEVICE = 4,      
    KAPS_JR1_DEVICE_INVALID        
};


enum kaps_jr1_device_pair_type {
    KAPS_JR1_DEVICE_PAIR_TYPE_A,       
    KAPS_JR1_DEVICE_PAIR_TYPE_MAX        
};




enum kaps_jr1_device_flags {
    KAPS_JR1_DEVICE_DEFAULT = 0,           
    KAPS_JR1_DEVICE_SMT = 1,               
    KAPS_JR1_DEVICE_DUAL_PORT = 2,         
    KAPS_JR1_DEVICE_NBO = 4,               
    KAPS_JR1_DEVICE_SUPPRESS_INDEX = 8,    
    KAPS_JR1_DEVICE_ISSU = 16,             
    KAPS_JR1_DEVICE_CASCADE = 32,          
    KAPS_JR1_DEVICE_SKIP_INIT = 64,        
    KAPS_JR1_DEVICE_SUPPRESS_PARITY = 128, 
    KAPS_JR1_DEVICE_QUAD_PORT = 256,       
    KAPS_JR1_DEVICE_HST_DIG_LPBK = 512,    
    KAPS_JR1_DEVICE_THREAD_SAFETY = 1024   
};







struct kaps_jr1_device_config {

    

    uint32_t num_devices;


};
















#ifdef __cplusplus
}
#endif
#endif 
