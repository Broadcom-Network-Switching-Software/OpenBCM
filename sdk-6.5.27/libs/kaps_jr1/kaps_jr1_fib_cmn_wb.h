

#ifndef __KAPS_JR1_FIB_CMN_WB_H
#define __KAPS_JR1_FIB_CMN_WB_H

#include "kaps_jr1_errors.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define KAPS_JR1_WB_IS_EQUAL(arg1, arg2, arg3)           \
    do                                              \
    {                                               \
        if (arg1 != arg2)                           \
        {                                           \
            kaps_jr1_printf(#arg3);                      \
            kaps_jr1_sassert(0);                              \
            return -1;                        \
        }                                           \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif                          
