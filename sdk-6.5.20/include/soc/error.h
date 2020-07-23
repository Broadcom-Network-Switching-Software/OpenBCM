/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_ERROR_H
#define _SOC_ERROR_H

#include <shared/error.h>
#include <soc/debug.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
#include <soc/dcmn/dcmn_gen_err.h>
#endif

/*
 * Defines:	SOC_E_XXX
 * Purpose:	SOC API error codes
 * Notes:
 *      An error code may be converted to a string by passing
 *      the code to soc_errmsg().
 */

typedef enum {
    SOC_E_NONE                  = _SHR_E_NONE,
    SOC_E_INTERNAL              = _SHR_E_INTERNAL,
    SOC_E_MEMORY                = _SHR_E_MEMORY,
    SOC_E_UNIT                  = _SHR_E_UNIT,
    SOC_E_PARAM                 = _SHR_E_PARAM,
    SOC_E_EMPTY                 = _SHR_E_EMPTY,
    SOC_E_FULL                  = _SHR_E_FULL,
    SOC_E_NOT_FOUND             = _SHR_E_NOT_FOUND,
    SOC_E_EXISTS                = _SHR_E_EXISTS,
    SOC_E_TIMEOUT               = _SHR_E_TIMEOUT,
    SOC_E_BUSY                  = _SHR_E_BUSY,
    SOC_E_FAIL                  = _SHR_E_FAIL,
    SOC_E_DISABLED              = _SHR_E_DISABLED,
    SOC_E_BADID                 = _SHR_E_BADID,
    SOC_E_RESOURCE              = _SHR_E_RESOURCE,
    SOC_E_CONFIG                = _SHR_E_CONFIG,
    SOC_E_UNAVAIL               = _SHR_E_UNAVAIL,
    SOC_E_INIT                  = _SHR_E_INIT,
    SOC_E_PORT                  = _SHR_E_PORT,
    SOC_E_IO                    = _SHR_E_IO,
    SOC_E_ACCESS                = _SHR_E_ACCESS,
    SOC_E_NO_HANDLER            = _SHR_E_NO_HANDLER,
    SOC_E_PARTIAL               = _SHR_E_PARTIAL,
    SOC_E_LIMIT                 = _SHR_E_LIMIT
} soc_error_t;

#define SOC_SUCCESS(rv)         _SHR_E_SUCCESS(rv)
#define SOC_FAILURE(rv)         _SHR_E_FAILURE(rv)
#define SOC_IF_ERROR_RETURN(op) _SHR_E_IF_ERROR_RETURN(op)
#define soc_errmsg(rv)          _SHR_ERRMSG(rv)
#define SOC_IF_ERROR_CLEAN_RETURN(op,exop) _SHR_E_IF_ERROR_CLEAN_RETURN(op,exop)

typedef enum {
    SOC_SWITCH_EVENT_IO_ERROR       = _SHR_SWITCH_EVENT_IO_ERROR,
    SOC_SWITCH_EVENT_PARITY_ERROR   = _SHR_SWITCH_EVENT_PARITY_ERROR,
    SOC_SWITCH_EVENT_THREAD_ERROR   = _SHR_SWITCH_EVENT_THREAD_ERROR,
    SOC_SWITCH_EVENT_ACCESS_ERROR   = _SHR_SWITCH_EVENT_ACCESS_ERROR,
    SOC_SWITCH_EVENT_ASSERT_ERROR   = _SHR_SWITCH_EVENT_ASSERT_ERROR,
    SOC_SWITCH_EVENT_MODID_CHANGE   = _SHR_SWITCH_EVENT_MODID_CHANGE,
    SOC_SWITCH_EVENT_DOS_ATTACK     = _SHR_SWITCH_EVENT_DOS_ATTACK,
    SOC_SWITCH_EVENT_STABLE_FULL    = _SHR_SWITCH_EVENT_STABLE_FULL,
    SOC_SWITCH_EVENT_STABLE_ERROR   = _SHR_SWITCH_EVENT_STABLE_ERROR,
    SOC_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN   = _SHR_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN,
    SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE     = _SHR_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
    SOC_SWITCH_EVENT_TUNE_ERROR     = _SHR_SWITCH_EVENT_TUNE_ERROR,
    SOC_SWITCH_EVENT_DEVICE_INTERRUPT        = _SHR_SWITCH_EVENT_DEVICE_INTERRUPT,
    SOC_SWITCH_EVENT_ALARM          = _SHR_SWITCH_EVENT_ALARM, 
    SOC_SWITCH_EVENT_MMU_BST_TRIGGER = _SHR_SWITCH_EVENT_MMU_BST_TRIGGER,
    SOC_SWITCH_EVENT_EPON_ALARM     = _SHR_SWITCH_EVENT_EPON_ALARM,
    SOC_SWITCH_EVENT_RUNT_DETECT    = _SHR_SWITCH_EVENT_RUNT_DETECT,
    SOC_SWITCH_EVENT_AUTONEG_SPEED_ERROR = _SHR_SWITCH_EVENT_AUTONEG_SPEED_ERROR,
    SOC_SWITCH_EVENT_COUNT          = _SHR_SWITCH_EVENT_COUNT        /* last, as always */
} soc_switch_event_t;

/* Early demand request error event */
#define SOC_SWITCH_EVENT_TUNE_ERROR_QM_BLOCK  1

/* Top level parity error info returned in event callbacks */
typedef enum {
    SOC_SWITCH_EVENT_DATA_ERROR_PARITY = 1,
    SOC_SWITCH_EVENT_DATA_ERROR_ECC,
    SOC_SWITCH_EVENT_DATA_ERROR_UNSPECIFIED,
    SOC_SWITCH_EVENT_DATA_ERROR_FATAL,
    SOC_SWITCH_EVENT_DATA_ERROR_CORRECTED,
    SOC_SWITCH_EVENT_DATA_ERROR_LOG,
    SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE,
    SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
    SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
    SOC_SWITCH_EVENT_DATA_ERROR_ECC_1BIT_CORRECTED, /* Single-bit ECC corrected, no traffic loss */
    SOC_SWITCH_EVENT_DATA_ERROR_ECC_2BIT_CORRECTED, /* Double-bit ECC corrected, potential traffic loss */
    SOC_SWITCH_EVENT_DATA_ERROR_PARITY_CORRECTED, /* Parity error correction */
    SOC_SWITCH_EVENT_DATA_ERROR_CFAP_MEM_FAIL
} soc_switch_data_error_t;

/* Top level switch event alarm info returned in event callbacks */
typedef enum {
    SOC_SWITCH_EVENT_ALARM_HIGH_TEMP = 1,
    SOC_SWITCH_EVENT_ALARM_125C_TEMP = 2,
    SOC_SWITCH_EVENT_ALARM_COUNT = 3
} bcm_switch_event_alarm_t;

#define SOC_SER_ERROR_PIPE_BP              16
#define SOC_SER_ERROR_DATA_ENCODING_BP     24
#define SOC_SER_ERROR_DATA_FIELD_ID_OFFSET 1 /* Field id and index */
#define SOC_SER_ERROR_DATA_REG_ID_OFFSET   2 /* Reg id and index */
#define SOC_SER_ERROR_DATA_ID_OFFSET       3 /* Mem id and index */
#define SOC_SER_ERROR_DATA_BLK_ADDR        4 /* Block, pipe and address */
#define SOC_SER_ERROR_ENTRY_ID_MINFO_OFFSET 5 /* entry id, block,pipe and field info */
#define SOC_SER_ERROR_DATA_FIELD_ID_OFFSET_SET \
        (SOC_SER_ERROR_DATA_FIELD_ID_OFFSET << SOC_SER_ERROR_DATA_ENCODING_BP)
#define SOC_SER_ERROR_DATA_REG_ID_OFFSET_SET \
        (SOC_SER_ERROR_DATA_REG_ID_OFFSET << SOC_SER_ERROR_DATA_ENCODING_BP)
#define SOC_SER_ERROR_DATA_ID_OFFSET_SET \
        (SOC_SER_ERROR_DATA_ID_OFFSET << SOC_SER_ERROR_DATA_ENCODING_BP)
#define SOC_SER_ERROR_DATA_BLK_ADDR_SET \
        (SOC_SER_ERROR_DATA_BLK_ADDR << SOC_SER_ERROR_DATA_ENCODING_BP)
#define SOC_SER_ERROR_ENTRY_ID_MINFO_SET \
        (SOC_SER_ERROR_ENTRY_ID_MINFO_OFFSET << SOC_SER_ERROR_DATA_ENCODING_BP)

#define SOC_ERROR_BLK_BP                   24
#define SOC_ERROR_PIPE_BP                  16
#define SOC_ERROR_FIELD_ENUM_MASK          0xffff
#define SOC_ERROR_MEM_ENUM_MASK            0xffff
#define SOC_ERROR_REG_ENUM_MASK            0xffff

typedef enum {
    SOC_SWITCH_EVENT_THREAD_L2X      = 1,
    SOC_SWITCH_EVENT_THREAD_COUNTER,
    SOC_SWITCH_EVENT_THREAD_MEMSCAN,
    SOC_SWITCH_EVENT_THREAD_L2MOD,
    SOC_SWITCH_EVENT_THREAD_L2MOD_DMA,
    SOC_SWITCH_EVENT_THREAD_IPFIX,
    SOC_SWITCH_EVENT_THREAD_LINKSCAN,
    SOC_SWITCH_EVENT_THREAD_REGEX_REPORT,
    SOC_SWITCH_EVENT_THREAD_HWDOS_MONITOR,
    SOC_SWITCH_EVENT_THREAD_CTR_EVICT_DMA,
    SOC_SWITCH_EVENT_THREAD_SRAMSCAN,
    SOC_SWITCH_EVENT_THREAD_L2X_LEARN
} soc_switch_thread_id_t;


/*Single point of exit*/
#define SOC_EXIT goto exit

/* Must appear at each function right after parameters definition */
#define SOC_INIT_FUNC_DEFS \
    int _rv = SOC_E_NONE, _lock_taken = 0; \
    (void)_lock_taken; \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META("enter\n"))); 

#define SOC_FUNC_ERROR \
    SOC_FAILURE(_rv)

#define SOC_FUNC_RETURN \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META("exit\n"))); \
    return _rv;

#define SOC_FUNC_RETURN_VOID \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META("exit\n")));\
        COMPILER_REFERENCE(_rv);

#if defined(BCM_SAND_SUPPORT) && defined(BCM_GEN_ERR_MECHANISM)
#define _SOC_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META("%s\n"), soc_errmsg(__err__rc))); \
            _rv = __err__rc; \
        } \
      } while(0)

#define _SOC_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META("%s\n"), soc_errmsg(__err__rc))); \
            _rv = __err__rc; \
            SOC_EXIT; \
        } \
      } while(0)
#else
#define _SOC_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != SOC_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META("%s\n"), soc_errmsg(__err__rc))); \
            _rv = __err__rc; \
        } \
      } while(0)

#define _SOC_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != SOC_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META("%s\n"), soc_errmsg(__err__rc))); \
            _rv = __err__rc; \
            SOC_EXIT; \
        } \
      } while(0)
#endif

#define _SOC_EXIT_WITH_ERR(_rc, stuff) \
      do { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, stuff); \
            _rv = _rc; \
            SOC_EXIT; \
      } while(0)
      

#define SOC_NULL_CHECK(arg) \
    do {   \
        if ((arg) == NULL) \
        { _SOC_EXIT_WITH_ERR(SOC_E_PARAM,(BSL_META("null parameter\n"))); } \
    } while (0)

#define SOC_FREE(arg)   \
  do {   \
      if(arg != NULL) \
      {sal_free(arg);} \
  } while (0)



#define SOC_ALLOC(var, type, count,str)                                     \
  do {                                                                       \
    if(var != NULL)                                                       \
    {    \
      _SOC_EXIT_WITH_ERR(SOC_E_PARAM,(BSL_META("Trying to allocate to a non null ptr is forbidden"))); \
    }                                                                     \
    var = (type*)sal_alloc((count) * sizeof(type),str);                  \
    if (var == NULL)                                                      \
    {\
      _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,(BSL_META("Allocating failed")));  \
    }                                                                     \
    sal_memset(                                                 \
        var,                                                          \
        0x0,                                                          \
        (size_t) ((count) * sizeof(type))                                        \
      );                                                    \
  } while(0)

#define SOC_CLEAR(var_ptr, type, count)                                 \
  do {                                                                                 \
    sal_memset(                                                 \
        var_ptr,                                                      \
        0x0,                                                          \
        (size_t) ((count) * sizeof(type))                                  \
      );                                                              \
  } while(0)

#define SOC_COPY(var_dest_ptr, var_src_ptr, type, count)                \
  do {                                                                     \
    sal_memcpy(                                                 \
        var_dest_ptr,                                                 \
        var_src_ptr,                                                  \
        (size_t) ((count) * sizeof(type))                             \
      );                                                             \
  }while(0)

#define SOC_COMP(var_ptr1, var_ptr2, type, count, is_equal_res)         \
  do {         \
    assert(var_ptr1 != NULL && var_ptr2 != NULL);                         \
    is_equal_res = sal_memcmp(          \
            var_ptr1,                                                     \
            var_ptr2,   \
            (size_t) ((count) * sizeof(type)) \
          ) ? FALSE : TRUE;   \
  }while(0)

#define _SOC_IF_ERROR_NOT_UNAVAIL_RETURN(op)  \
      _SHR_E_IF_ERROR_NOT_UNAVAIL_RETURN(op)

#endif  /* !_SOC_ERROR_H */
