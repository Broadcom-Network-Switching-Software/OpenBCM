

#ifndef __KAPS_JR1_ERRORS_H
#define __KAPS_JR1_ERRORS_H

#include "kaps_jr1_portable.h"

#ifdef __cplusplus
extern "C" {
#endif







typedef enum kaps_jr1_status { 
#define KAPS_JR1_INC_SEL(name,string) name,
#include "kaps_jr1_error_tbl.def"
#undef KAPS_JR1_INC_SEL
    KAPS_JR1_STATUS_LAST_UNUSED
} kaps_jr1_status;



const char *kaps_jr1_get_status_string(kaps_jr1_status status);



#define KAPS_JR1_TRY(A)                                                      \
    do                                                                  \
    {                                                                   \
        kaps_jr1_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_JR1_OK)                                     \
        {                                                               \
            kaps_jr1_printf(#A" failed: %s\n", kaps_jr1_get_status_string(__tmp_status)); \
            return __tmp_status;                                        \
        }                                                               \
    }                                                                   \
    while(0)



#define KAPS_JR1_STRY(A)                                                     \
    do                                                                  \
    {                                                                   \
        kaps_jr1_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_JR1_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)



#define KAPS_JR1_ETRY(A,E)                                                   \
    do                                                                  \
    {                                                                   \
        kaps_jr1_status __tmp_status = A;                                    \
        kaps_jr1_status __exp_status = E;                                    \
        if (__tmp_status != KAPS_JR1_OK && __tmp_status != __exp_status)     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)




#ifdef __cplusplus
}
#endif
#endif                          
