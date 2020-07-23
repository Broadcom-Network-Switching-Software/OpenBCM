/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOCDNX_ERROR_H
#define _SOCDNX_ERROR_H

#include <soc/dcmn/dcmn_gen_err.h>
#include <shared/error.h>
#include <shared/bsl.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/drv.h>
#include <soc/dpp/error.h>
#include <soc/dpp/debug.h> 
#include <soc/dpp/SAND/Management/sand_general_macros.h> 
#endif 

#if  defined(BROADCOM_DEBUG)


    #define _SOCDNX_MSG(string) string "\n"
    #define _SOCDNX_MSG_NO_UNIT(string) string "\n"

    #define _BSL_SOCDNX_MSG(string) _ERR_MSG_MODULE_NAME, unit, string "\n"
    #define _BSL_SOCDNX_MSG_NO_UNIT(string) _ERR_MSG_MODULE_NAME, BSL_UNIT_UNKNOWN, string "\n"

#else 
    #define _SOCDNX_MSG(string) string
    #define _SOCDNX_MSG_W_UNIT(string) string
    #define _SOCDNX_MSG_NO_UNIT(string) string

    #define _BSL_SOCDNX_MSG(string) _ERR_MSG_MODULE_NAME, unit, string
    #define _BSL_SOCDNX_MSG_NO_UNIT(string) _ERR_MSG_MODULE_NAME, unit, string
    
#endif 
    #define _BSL_SOCDNX_MSG_STR(string) _ERR_MSG_MODULE_NAME, unit, string

#define SOCDNX_IF_ERR_RETURN(_rv) \
  do { \
  int __rv__ = _rv; \
  GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __rv__); \
  if (__rv__ != SOC_E_NONE) { \
  _bsl_error(_BSL_SOCDNX_MSG("%s"),soc_errmsg(__rv__)); \
  return __rv__; \
  } \
  } while (0)

#define _BSL_SOCDNX_VVERBOSE_MSG(string) _ERR_MSG_MODULE_NAME, unit, "%s[%d]%s unit: %d  " string "\n", __FILE__, __LINE__, FUNCTION_NAME(), unit


#define SOCDNX_INIT_FUNC_DEFS \
    int _rv = SOC_E_NONE, _lock_taken = 0; \
    (void)_lock_taken;  \
    _bsl_vverbose(_BSL_SOCDNX_VVERBOSE_MSG("enter")); 

#define SOCDNX_FUNC_ERROR \
    SOC_FAILURE(_rv)

#define SOCDNX_FUNC_RETURN \
    _bsl_vverbose(_BSL_SOCDNX_VVERBOSE_MSG("exit")); \
    return _rv;

#define SOCDNX_FUNC_RETURN_VOID \
    _bsl_vverbose(_BSL_SOCDNX_VVERBOSE_MSG("exit"));\
    COMPILER_REFERENCE(_rv);

#define SOCDNX_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            _bsl_warn(_BSL_SOCDNX_MSG("%s"),soc_errmsg(__err__rc)); \
            _rv = __err__rc; \
        } \
      } while(0)

#define SOCDNX_SAND_IF_ERR_CONT(_sand_ret) \
    do { \
        int __err__rc;\
        int error_code = soc_sand_get_error_code_from_error_word(_sand_ret);\
        __err__rc = (error_code == SOC_SAND_OK) ? SOC_E_NONE : SOC_E_FAIL;\
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            _bsl_warn(_BSL_SOCDNX_MSG("%s"),soc_errmsg(__err__rc)); \
            _rv = __err__rc; \
        } \
      } while(0)

#define SOCDNX_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            _bsl_error(_BSL_SOCDNX_MSG("%s"),soc_errmsg(__err__rc)); \
            _rv = __err__rc; \
            SOC_EXIT; \
        } \
      } while(0)


#define SOCDNX_IF_ERR_REPORT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            _bsl_error(_BSL_SOCDNX_MSG("%s"),soc_errmsg(__err__rc)); \
            if (_rv !=  SOC_E_NONE) _rv = __err__rc; \
        } \
      } while(0)

#define SOCDNX_IF_ERR_EXIT_NO_UNIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != SOC_E_NONE) { \
            _bsl_error(_BSL_SOCDNX_MSG_NO_UNIT("%s"),soc_errmsg(__err__rc)); \
            _rv = __err__rc; \
            SOC_EXIT; \
        } \
      } while(0)


#define SOCDNX_SAND_IF_ERR_RETURN(_sand_ret) \
      do { \
      soc_error_t _rv = handle_sand_result(_sand_ret); \
      GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, _rv); \
      if (_rv != SOC_E_NONE) { \
      _bsl_error(_BSL_SOCDNX_MSG("%s"),soc_errmsg(_rv)); \
      return _rv; \
      } \
      } while (0)

#define SOCDNX_SAND_IF_ERR_EXIT(_sand_ret) \
    do { \
        int __err__rc = handle_sand_result(_sand_ret); \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            _bsl_error(_BSL_SOCDNX_MSG("%s"),soc_errmsg(_rv)); \
            _rv = __err__rc; \
            SOC_EXIT; \
        } \
      } while(0)

#define SOCDNX_SAND_IF_ERR_EXIT_MSG(_sand_ret, stuff) \
    do { \
        int __err__rc = handle_sand_result(_sand_ret); \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_SOC, __err__rc); \
        if(__err__rc != SOC_E_NONE) { \
            SOCDNX_EXIT_WITH_ERR(__err__rc, stuff); \
        } \
      } while(0)


#define SOCDNX_EXIT_WITH_ERR(_rc, stuff) \
      do { \
            _bsl_error stuff ; \
            _rv = _rc; \
            SOC_EXIT; \
      } while(0)

#define SOCDNX_EXIT_WITH_ERR_NO_MSG(_rc) \
      do { \
            _rv = _rc; \
            SOC_EXIT; \
      } while(0)

#define SOCDNX_IF_ERR_EXIT_MSG(_rc, stuff)  \
      do { \
      int __err__rc = _rc; \
      if(__err__rc != SOC_E_NONE) { \
          _bsl_error stuff ; \
          _rv = __err__rc; \
          SOC_EXIT; \
      } \
          \
           \
} while (0); 

#define SOCDNX_NULL_CHECK(arg) \
    do {   \
        if ((arg) == NULL) \
        { SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null parameter" ))); } \
    } while (0)

#define SOCDNX_FREE(arg)   \
  do {   \
      if(arg != NULL) \
      {sal_free(arg);} \
  } while (0)

#define SOCDNX_ALLOC(var, type, count,str)                                     \
    do {                                                                       \
      if(var != NULL)                                                       \
      {                                                                     \
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Trying to allocate to a non null ptr is forbidden"))); \
      }                                                                     \
      var = (type*)sal_alloc((count) * sizeof(type),str);                  \
      if (var == NULL)                                                      \
      {                                                                     \
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Allocating failed")));  \
      }                                                                     \
      sal_memset(                                                 \
          var,                                                          \
          0x0,                                                          \
          (size_t) ((count) * sizeof(type))                                        \
        );                                                    \
    } while(0)

#ifdef BROADCOM_DEBUG
#define SOCDNX_VERIFY(exp) if (!(exp)) { SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("VERIFY FAILED (%s:%d): %s\n"), __FILE__, __LINE__, #exp)); }
#else
#define SOCDNX_VERIFY(exp) ((void)0)
#endif

#ifdef DCMN_RUNTIME_DEBUG
typedef enum {
    dcmn_runtime_debug_state_loading = 0,
    dcmn_runtime_debug_state_initializing = 1,
    dcmn_runtime_debug_state_running = 2
} dcmn_runtime_debug_state_t;
typedef struct {
    sal_time_t start_time;
    sal_time_t last_time;
    dcmn_runtime_debug_state_t run_stage;
} dcmn_runtime_debug_t;
extern dcmn_runtime_debug_t dcmn_runtime_debug_per_device[SOC_MAX_NUM_DEVICES];

void dcmn_runtime_debug_update_print_time(int unit, const char *string_to_print);

#define DCMN_RUNTIME_DEBUG_PRINT(unit) dcmn_runtime_debug_update_print_time(unit, "")
#define DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, location) dcmn_runtime_debug_update_print_time(unit, (location))
#else 
#define DCMN_RUNTIME_DEBUG_PRINT(unit)
#define DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, location)
#endif 


#ifdef BCM_PCID_LITE
#define SOCDNX_PCID_LITE_SKIP(unit)                         \
    SOC_EXIT
#else
#define SOCDNX_PCID_LITE_SKIP(unit)                         
#endif

#endif  
