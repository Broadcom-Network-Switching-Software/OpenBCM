

#ifndef __KAPS_ERRORS_H
#define __KAPS_ERRORS_H

#include "kaps_portable.h"

#ifdef __cplusplus
extern "C" {
#endif







typedef enum kaps_status { 
#define KAPS_INC_SEL(name,string) name,
#include "kaps_error_tbl.def"
#undef KAPS_INC_SEL
    KAPS_STATUS_LAST_UNUSED
} kaps_status;



const char *kaps_get_status_string(kaps_status status);



#define KAPS_TRY(A)                                                      \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            kaps_printf(#A" failed: %s\n", kaps_get_status_string(__tmp_status)); \
            return __tmp_status;                                        \
        }                                                               \
    }                                                                   \
    while(0)



#define KAPS_STRY(A)                                                     \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)



#define KAPS_ETRY(A,E)                                                   \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        kaps_status __exp_status = E;                                    \
        if (__tmp_status != KAPS_OK && __tmp_status != __exp_status)     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)




#ifdef __cplusplus
}
#endif
#endif                          
