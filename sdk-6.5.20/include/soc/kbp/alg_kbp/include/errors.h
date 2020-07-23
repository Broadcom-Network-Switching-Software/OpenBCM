/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __ERRORS_H
#define __ERRORS_H


#ifdef __cplusplus
extern "C" {
#endif







typedef enum kbp_status { 
#define KBP_INC_SEL(name,string) name,
#include "error_tbl.def"
#undef KBP_INC_SEL
    KBP_STATUS_LAST_UNUSED
} kbp_status;



const char *kbp_get_status_string(kbp_status status);



#define KBP_TRY(A)                                                      \
    do                                                                  \
    {                                                                   \
        kbp_status __tmp_status = A;                                    \
        if (__tmp_status != KBP_OK)                                     \
        {                                                               \
            kbp_printf(#A" failed: %s\n", kbp_get_status_string(__tmp_status)); \
            return __tmp_status;                                        \
        }                                                               \
    }                                                                   \
    while(0)



#define KBP_STRY(A)                                                     \
    do                                                                  \
    {                                                                   \
        kbp_status __tmp_status = A;                                    \
        if (__tmp_status != KBP_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)



#define KBP_ETRY(A,E)                                                   \
    do                                                                  \
    {                                                                   \
        kbp_status __tmp_status = A;                                    \
        kbp_status __exp_status = E;                                    \
        if (__tmp_status != KBP_OK && __tmp_status != __exp_status)     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)




#ifdef __cplusplus
}
#endif
#endif                          
