

#ifndef __KAPS_ERRORS_H
#define __KAPS_ERRORS_H

#include <shared/shrextend/shrextend_debug.h>

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


#define DNX_KAPS_TRY(A)                                                                                              \
    do                                                                                                              \
    {                                                                                                               \
        kaps_status __tmp_status = A;                                                                                \
        if (__tmp_status != KAPS_OK)                                                                                 \
        {                                                                                                           \
            SHR_IF_ERR_EXIT_WITH_LOG(kaps_dnx_error_translation(__tmp_status),                                       \
                                       "\n"#A" failed: %s%s%s\n",                                                   \
                                       kaps_get_status_string(__tmp_status), EMPTY, EMPTY);                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)


#define DNX_KAPS_TRY_PRINT(A, _format_, ...)                                                                         \
    do                                                                                                              \
    {                                                                                                               \
        kaps_status __tmp_status = A;                                                                                \
        if (__tmp_status != KAPS_OK)                                                                                 \
        {                                                                                                           \
            LOG_CLI((BSL_META(_format_), ##__VA_ARGS__));                                                             \
            SHR_IF_ERR_EXIT_WITH_LOG(kaps_dnx_error_translation(__tmp_status),                                       \
                                       "\n"#A" failed: %s%s%s\n",                                                   \
                                       kaps_get_status_string(__tmp_status), EMPTY, EMPTY);                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

#define DNX_KAPS_IS_ERROR(kaps_status)          (kaps_status != KAPS_OK)



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


shr_error_e kaps_dnx_error_translation(
    kaps_status kaps_err);



#ifdef __cplusplus
}
#endif
#endif                          
