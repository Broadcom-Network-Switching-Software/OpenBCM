
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef SHREXTEND_DEBUG_H_INCLUDED

#define SHREXTEND_DEBUG_H_INCLUDED

#if defined(__STDC_VERSION__)

#if (__STDC_VERSION__ >= 199901L)

#define C99_IS_ACTIVE 1

#else

#define C99_IS_ACTIVE 0

#endif

#else

#define C99_IS_ACTIVE 1

#endif

#include <shared/bsl.h>
#include <shared/bsltypes.h>
#include <soc/sand/shrextend/shrextend_error.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>
#include <sal/core/time.h>

#ifdef BCM_DNXC_SUPPORT
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/dnxc_verify.h>
#endif

#define NO_UNIT              BSL_UNIT_UNKNOWN

#define EMPTY                ""

#define NO_LOG_INFO_AND_DOWN 0

#define LOG_FATAL_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)    \
  if (bsl_fast_check(_log_module|BSL_FATAL)) \
  {                                          \
    sal_printf("\r\n") ;                     \
  }                                          \
  LOG_FATAL(_log_module,                                             \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )
#define LOG_ERROR_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)    \
  if (bsl_fast_check(_log_module|BSL_ERROR)) \
  { \
    sal_printf("\r\n") ; \
  } \
  LOG_ERROR(_log_module,                                             \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )
#define LOG_WARN_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)     \
  if (bsl_fast_check(_log_module|BSL_WARN)) \
  {                                         \
    sal_printf("\r\n") ;                    \
  }                                         \
  LOG_WARN(_log_module,                                              \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )

#define LOG_CLI_EX(_format_string, _p1,_p2,_p3,_p4)     \
  LOG_CLI((BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )

#if !NO_LOG_INFO_AND_DOWN

#define LOG_INFO_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)     \
  LOG_INFO(_log_module,                                              \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )
#define LOG_VERBOSE_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)  \
  LOG_VERBOSE(_log_module,                                           \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )
#define LOG_DEBUG_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)  \
  LOG_DEBUG(_log_module,                                               \
    (BSL_META_U(_func_unit,":%d,%s(),"),__LINE__,__func__)) ;      \
  LOG_DEBUG(_log_module,                                               \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4))

#define LOG_DEBUG_CONT_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)  \
  LOG_DEBUG(_log_module,                                                \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4))

#define LOG_DEBUG_PREF(_log_module,_format_string, _p1,_p2,_p3,_p4)  \
{                                                                  \
  bsl_severity_t sev_min, sev_max ;                                \
  BSLDNX_GET_PREFIX_RANGE_MIN_MAX get_proc ;                       \
  BSLDNX_SET_PREFIX_RANGE_MIN_MAX set_proc ;                       \
  get_proc_get_prefix_range_min_max(&get_proc) ;                   \
  get_proc_set_prefix_range_min_max(&set_proc) ;                   \
  if (bsl_fast_check(_log_module|BSL_DEBUG))                       \
  {                                                                \
    if (get_proc && set_proc)                                      \
    {                                                              \
      get_proc(&sev_min,&sev_max) ;                                \
      set_proc(bslSeverityOff + 1,bslSeverityDebug) ;              \
    }                                                              \
  }                                                                \
  LOG_DEBUG(_log_module,                                           \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  ) ;   \
  if (bsl_fast_check(_log_module|BSL_DEBUG))                       \
  {                                                                \
    if (get_proc && set_proc)                                      \
    {                                                              \
      set_proc(sev_min,sev_max) ;                                  \
    }                                                              \
  }                                                                \
}

#else

#define LOG_INFO_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)
#define LOG_VERBOSE_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)
#define LOG_DEBUG_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)
#define LOG_DEBUG_CONT_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)
#define LOG_DEBUG_PREF(_log_module,_format_string, _p1,_p2,_p3,_p4)

#endif

#define SHR_TIME_MEASURE_INIT(_unit)    \
  { \
    unsigned int measure_elapsed_time ; \
    sal_usecs_t elapsed_time[10] ;      \
    sal_usecs_t start_time ;            \
    unsigned int ii, num_measured ;     \
    num_measured = 0 ;                  \
    start_time = 0 ; \
    if (dnx_drv_soc_property_suffix_num_get(_unit, -1, spn_CUSTOM_FEATURE, "time_log_en", 0)) \
    {                                   \
        measure_elapsed_time = TRUE ;   \
    }                                   \
    else                                \
    {                                   \
        measure_elapsed_time = FALSE ;  \
    }                                   \
    if (measure_elapsed_time)           \
    {                                   \
        start_time = sal_time_usecs() ; \
    }

#define SHR_TIME_MEASURE_CLOSE(_unit)        \
    for (ii = 0 ; ii < num_measured ; ii++)  \
    {                                        \
        LOG_CLI_EX("elapsed_time[%d] = %d microsec %s%s\r\n", ii,elapsed_time[ii], EMPTY, EMPTY);  \
    }                                        \
  }

#define SHR_TIME_MEASURE()        \
    if (measure_elapsed_time)  \
    {                          \
        if (num_measured < (sizeof(elapsed_time) / sizeof(elapsed_time[0])))  \
        {                                \
            elapsed_time[num_measured++] = sal_time_usecs() - start_time ;  \
        }                                \
        start_time = sal_time_usecs() ;  \
    }

#define SHR_SET_SEVERITY_FOR_MODULE(_bsl_severity) \
{                                                \
  int layer, source ;                            \
  BSLENABLE_SET set_proc ;                       \
  get_proc_bslenable_set(&set_proc) ;            \
  layer = BSL_LAYER_GET(BSL_LOG_MODULE) ;        \
  source = BSL_SOURCE_GET(BSL_LOG_MODULE) ;      \
  if (set_proc)                                  \
  {                                              \
    set_proc((bsl_layer_t)layer, (bsl_source_t)source, (bsl_severity_t)_bsl_severity) ; \
  }                                              \
}

#define SHR_GET_SEVERITY_FOR_MODULE(_bsl_severity)       \
{                                                        \
  BSLENABLE_GET local_get_proc ;                         \
  get_proc_bslenable_get(&local_get_proc) ;              \
  if (local_get_proc)                                    \
  {                                                      \
    _bsl_severity =                                      \
      local_get_proc((bsl_layer_t)BSL_LAYER_GET(BSL_LOG_MODULE), \
        (bsl_source_t)BSL_SOURCE_GET(BSL_LOG_MODULE)) ;  \
  }                                                      \
  else                                                   \
  {                                                      \
    _bsl_severity = bslSeverityOff ;                     \
  }                                                      \
}

#define SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(_module,_bsl_severity) \
{                                                \
  int layer, source ;                            \
  BSLENABLE_SET set_proc ;                       \
  get_proc_bslenable_set(&set_proc) ;            \
  layer = BSL_LAYER_GET(_module) ;        \
  source = BSL_SOURCE_GET(_module) ;      \
  if (set_proc)                                  \
  {                                              \
    set_proc((bsl_layer_t)layer, (bsl_source_t)source, (bsl_severity_t)_bsl_severity) ; \
  }                                              \
}

#define SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(_module,_bsl_severity)       \
{                                                        \
  BSLENABLE_GET local_get_proc ;                         \
  get_proc_bslenable_get(&local_get_proc) ;              \
  if (local_get_proc)                                    \
  {                                                      \
    _bsl_severity =                                      \
      local_get_proc((bsl_layer_t)BSL_LAYER_GET(_module), \
        (bsl_source_t)BSL_SOURCE_GET(_module)) ;  \
  }                                                      \
  else                                                   \
  {                                                      \
    _bsl_severity = bslSeverityOff ;                     \
  }                                                      \
}

#ifdef BCM_DNXC_SUPPORT

#define SHR_FUNC_INIT_VARS(_unit)    \
  int _func_unit = _unit ;           \
  int _func_rv = _SHR_E_NONE ;       \
  COMPILER_REFERENCE(_func_unit);    \
  DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(_unit, __func__, MTA_FLAG_SHR_INIT_FUNC, TRUE));

#else

#define SHR_FUNC_INIT_VARS(_unit)    \
  int _func_unit = _unit ;           \
  int _func_rv = _SHR_E_NONE ;       \
  COMPILER_REFERENCE(_func_unit);

#endif

#ifdef BCM_DNXC_SUPPORT

#define SHR_FUNC_EXIT                                                 \
  DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(_func_unit, __func__, MTA_FLAG_SHR_INIT_FUNC, FALSE)); \
  return _func_rv

#else

#define SHR_FUNC_EXIT                                                 \
  return _func_rv

#endif

#define SHR_EXIT() goto exit

#if (C99_IS_ACTIVE)

#define _SHR_ERR_EXIT(_expr,_formatting_string,...) \
{ \
  int _rv = _expr;    \
  if (bsl_fast_check(BSL_LOG_MODULE|BSL_WARN)) \
  { \
    sal_printf("\r\n") ; \
  } \
  LOG_ERROR(BSL_LOG_MODULE,          \
    (BSL_META_U(_func_unit," Error '%s' indicated, " _formatting_string "%s"), shrextend_errmsg_get(_rv), ##__VA_ARGS__) ) ; \
  _func_rv = _rv ;    \
  SHR_EXIT() ;        \
}

#define SHR_ERR_EXIT(...) _SHR_ERR_EXIT(__VA_ARGS__, "")

#else

#define SHR_ERR_EXIT()

#endif

#define SHR_IF_ERR_EXIT(_expr)      \
do                      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
    _func_rv = _rv ;  \
    SHR_EXIT() ;        \
  }                     \
} while (0)

#define SHR_ERR_HANDLE_AFTER_EXIT(_expr)      \
do                      \
{                       \
  int curr_rv = _expr;      \
  if (SHR_FAILURE(curr_rv)) \
  {                     \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(curr_rv) ,EMPTY,EMPTY,EMPTY) ;  \
    if(!SHR_FAILURE(_func_rv)) \
    {                  \
        _func_rv = curr_rv ;  \
    }                       \
  }                     \
} while (0)

#define SHR_IF_ERR_EXIT_NO_MSG(_expr)      \
do                      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    _func_rv = _rv ;  \
    SHR_EXIT() ;        \
  }                     \
} while (0)

#define SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(_expr, _rv_except)      \
do                                                                 \
{                                                                  \
  int _rv = _expr;                                                 \
  if (SHR_FAILURE(_rv) && (_rv != _rv_except))                     \
  {                                                                \
    _func_rv = _rv ;                                               \
    SHR_IF_ERR_EXIT(_func_rv) ;                                    \
  }                                                                \
  else if (SHR_FAILURE(_rv) && (_rv == _rv_except))                \
  {                                                                \
    _func_rv = _rv ;                                               \
    SHR_IF_ERR_EXIT_NO_MSG(_func_rv) ;                             \
  }                                                                \
} while (0)

#define SHR_IF_ERR_RETURN(_expr)      \
do                      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
    return _rv ;  \
  }                     \
} while (0)

#define SHR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4) \
do                      \
{                       \
    _func_rv = _expr ;    \
    LOG_ERROR_EX(BSL_LOG_MODULE,                   \
      " Error: %s ; " _formatting_string,          \
      shrextend_errmsg_get(_func_rv) ,_param2,_param3,_param4) ; \
    SHR_EXIT() ;        \
   \
   \
} while (0)

#define SHR_LOG_AND_EXIT(_expr,_stuff) \
do                      \
{                       \
    _func_rv = _expr ;    \
    LOG_ERROR(BSL_LOG_MODULE, _stuff);  \
    SHR_EXIT() ;        \
   \
   \
} while (0)

#define SHR_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4) \
do                      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    SHR_EXIT_WITH_LOG(_rv,_formatting_string,_param2,_param3,_param4); \
  }                     \
   \
   \
} while (0)

#if defined(BCM_DNXC_SUPPORT)

#define SHR_INVOKE_VERIFY_DNXC(_expr)                        \
do                                                          \
{                                                           \
  if (DNXC_VERIFY_ALLOWED_GET(_func_unit))                  \
  {                                                         \
      int _rv = _expr ;                                     \
      if (SHR_FAILURE(_rv))                                 \
      {                                                     \
        LOG_ERROR_EX(BSL_LOG_MODULE,                        \
          " Error indicated (%s) on VERIFY ; %s%s%s\r\n" ,  \
          shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;   \
        _func_rv = _rv ;                                    \
        SHR_EXIT() ;                                        \
      }                                                     \
  }                                                         \
} while (0)

#endif

#define SHR_NULL_CHECK(_ptr, _rv, _str) \
{ \
  if ((_ptr) == NULL) \
  {                   \
    if (SHR_FAILURE(_rv)) \
    {                     \
      LOG_ERROR_EX(BSL_LOG_MODULE,                       \
        " Error: %s, because %s is NULL %s%s\r\n",       \
        shrextend_errmsg_get(_rv), _str, EMPTY, EMPTY) ; \
    }                     \
    _func_rv = _rv ;      \
    SHR_EXIT() ;          \
  }                       \
   \
   \
}

#define SHR_VAL_VERIFY(_a, _b, _rv, _str) \
  if ((_a) != (_b))     \
  {                     \
    SHR_ERR_EXIT(_rv, _str); \
  }

#if (C99_IS_ACTIVE)

#define SHR_ASSERT_EQ(_expected, _expr) \
    do \
    { \
        int _res = (_expr); \
        int _exp = (_expected); \
        if (_res != _exp) \
        { \
            SHR_ERR_EXIT(_SHR_E_FAIL, \
                         "Assertion error: " #_expr " = %d (0x%08x). Expected: %d (0x%08x).\n", \
                         _res, _res, _exp, _exp); \
        }\
    } \
    while(0)

#else

#define SHR_ASSERT_EQ(_expected, _expr)

#endif

#define SHR_MASK_VERIFY(_val, _mask, _rv, _str)         \
{                                                       \
         uint32 unsupported_bits = ((_val) & (~_mask)); \
         if (unsupported_bits != 0)                     \
         {                                              \
             SHR_ERR_EXIT(_rv, _str);                   \
         }                                              \
}

#define SHR_BOOL_VERIFY(_val, _rv, _str)                               \
     if ((_val != TRUE) && (_val != FALSE))                            \
     {                                                                 \
         SHR_ERR_EXIT(_rv, "%s value:%d is not boolean", _str, _val);  \
     }                                                                 \


#define SHR_NOF_SET_BITS_IN_RANGE_VERIFY(_val, _mask,  _min, _max, _rv, _str) \
do                                                            \
{                                                             \
    int nof_set_bits;                                         \
    uint32 relevant_bits = ((_val) & (_mask));                \
    SHR_BITCOUNT_RANGE(&relevant_bits, nof_set_bits, 0, 32);  \
    if (nof_set_bits < (_min) || nof_set_bits > (_max))       \
         {                                                    \
             SHR_ERR_EXIT(_rv, _str);                         \
         }                                                    \
}                                                             \
while (0)

#if (C99_IS_ACTIVE)

#define SHR_RANGE_VERIFY(_val, _min, _max, _rv, ...)   \
{                                                       \
         if ((_val) < (_min) || (_val) > (_max))        \
         {                                              \
             SHR_ERR_EXIT(_rv, __VA_ARGS__);                   \
         }                                              \
}

#define SHR_MAX_VERIFY(_val, _max, _rv, ...)   \
{                                                       \
         if ((_val) > (_max))                           \
         {                                              \
             SHR_ERR_EXIT(_rv, __VA_ARGS__);                   \
         }                                              \
}

#else

#define SHR_RANGE_VERIFY()
#define SHR_MAX_VERIFY()

#endif

#define SHR_IF_NOT_NULL_FILL(_ptr, _value) \
do                    \
{                     \
  if ((_ptr) != NULL) \
  {                   \
    *(_ptr) = (_value);    \
  }                        \
} while (0)

#define SHR_FUNC_ERR()    SHR_FAILURE(_func_rv)

#define SHR_IF_ERR_CONT(_expr)      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    LOG_WARN_EX(BSL_LOG_MODULE,                       \
      " Warning '%s' indicated ; %s%s%s\r\n" ,             \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
    _func_rv = _rv ;  \
  }                     \
}

#define SHR_IF_ERR_SET(_expr)      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
     _func_rv = _rv ;  \
  }                     \
}

#define SHR_IF_ERR_CONT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4) \
{                                                 \
  shr_error_e _rv = _expr ;                       \
  if (SHR_FAILURE(_rv))                           \
  {                                               \
    LOG_WARN_EX(BSL_LOG_MODULE,                   \
      " Warning: %s ; " _formatting_string,shrextend_errmsg_get(_rv),_param2,_param3,_param4 ) ; \
    _func_rv = _rv ;                            \
  }                                               \
}

#define SHR_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except)   \
{                                                  \
  int _rv = _expr ;                                \
  if (SHR_FAILURE(_rv) && _rv != _rv_except)       \
  {                                                \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
    _func_rv = _rv ;                            \
    SHR_EXIT() ;                                \
  }                                             \
}

#define SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(_expr, _rv_expect)    \
{                                                             \
  int _rv = _expr ;                                           \
  if (SHR_FAILURE(_rv) && _rv != _rv_expect)                  \
  {                                                           \
    LOG_ERROR_EX(BSL_LOG_MODULE,                              \
      " Error '%s' indicated ; %s%s%s\r\n" ,                  \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;         \
    _func_rv = _rv ;                                          \
    SHR_EXIT() ;                                              \
  }                                                           \
  else if (_rv == _rv_expect)                                 \
  {                                                           \
    _func_rv = _rv ;                                          \
  }                                                           \
}

#define SHR_IF_NOT_EXCEPT_ERR_EXIT(_expr, _rv_except)   \
{                                                  \
  int _rv = _expr ;                                \
  if ( _rv != _rv_except)       \
  {                                                \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' was indicated, expected error '%s' ; %s%s\r\n" ,           \
      shrextend_errmsg_get(_rv) ,shrextend_errmsg_get(_rv_except),EMPTY,EMPTY) ;  \
    _func_rv = _rv ;                            \
    SHR_EXIT() ;                                \
  }                                             \
}

#define SHR_IF_ERR_EXIT_EXCEPT_IF_WITH_LOG(_expr, _rv_except,_formatting_string,_param2,_param3,_param4)   \
{                                                  \
  int _rv = _expr ;                                \
  if (SHR_FAILURE(_rv) && _rv != _rv_except)       \
  {                                                \
    LOG_ERROR_EX(BSL_LOG_MODULE,                   \
      " Error: %s ; " _formatting_string,          \
      shrextend_errmsg_get(_rv) ,_param2,_param3,_param4) ; \
    _func_rv = _rv ;                            \
    SHR_EXIT() ;                                \
  }                                             \
}

#define SHR_IF_ERR_REPLACE_AND_EXIT(_expr, _rv_to_replace, _rv_replacement)   \
{                               \
  int __rv = _expr ;             \
  if (__rv == _rv_to_replace)    \
  {                             \
      __rv = _rv_replacement;    \
  }                             \
  SHR_IF_ERR_EXIT(__rv);         \
}

#define SHR_IF_ERR_EXIT_WITH_EXCEPTION(_expr, _rv_to_replace)   \
{                               \
  int __rv = _expr ;             \
  if (__rv == _rv_to_replace)    \
  {                             \
      SHR_EXIT();    \
  }                             \
  SHR_IF_ERR_EXIT(__rv);         \
}

#define SHR_FUNC_VAL_IS(_rv)     (_func_rv == (_rv))

#define SHR_GET_CURRENT_ERR()     _func_rv

#define SHR_SET_CURRENT_ERR(_rv)     _func_rv = _rv

#define SHR_ALLOC(_ptr, _sz, _str, _formatting_string,_param2,_param3,_param4)  \
{                                                \
  SHR_ALLOC2(_ptr, _sz, SAL_ALLOC_F_NONE, _str, _formatting_string,_param2,_param3,_param4) \
}

#define SHR_ALLOC_ERR_EXIT(_ptr, _sz, _str, _formatting_string,_param2,_param3,_param4)  \
{                                                \
  SHR_ALLOC2_ERR_EXIT(_ptr, _sz, SAL_ALLOC_F_NONE, _str, _formatting_string,_param2,_param3,_param4) \
}

#define SHR_ALLOC2(_ptr, _sz, _flags, _str, _formatting_string,_param2,_param3,_param4)  \
{                                                \
  if ((_ptr) == NULL)                            \
  {                                              \
    (_ptr) = sal_alloc2(_sz, _flags, _str) ;              \
    if ((_ptr) == NULL)                          \
    {                                            \
      LOG_ERROR_EX(BSL_LOG_MODULE,               \
        " Failed to allocate: mem id is '%s' ; " _formatting_string, \
        _str ,_param2,_param3,_param4) ;         \
    }                                            \
  }                                              \
  else                                           \
  {                                              \
    LOG_ERROR_EX(BSL_LOG_MODULE,                 \
      " Ptr is NOT NULL at alloc: mem id is '%s' ; " _formatting_string, \
      _str ,_param2,_param3,_param4) ;           \
    SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);        \
    SHR_EXIT() ;                                 \
  }                                              \
}

#define SHR_ALLOC2_ERR_EXIT(_ptr, _sz, _flags, _str, _formatting_string,_param2,_param3,_param4)  \
{                                                \
  if ((_ptr) == NULL)                            \
  {                                              \
    (_ptr) = sal_alloc2(_sz, _flags, _str) ;     \
    if ((_ptr) == NULL)                          \
    {                                            \
      LOG_ERROR_EX(BSL_LOG_MODULE,               \
        " Failed to allocate: mem id is '%s' ; " _formatting_string, \
        _str ,_param2,_param3,_param4) ;         \
      SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);        \
      SHR_EXIT() ;                               \
    }                                            \
  }                                              \
  else                                           \
  {                                              \
    LOG_ERROR_EX(BSL_LOG_MODULE,                 \
      " Ptr is NOT NULL at alloc: mem id is '%s' ; " _formatting_string, \
      _str ,_param2,_param3,_param4) ;           \
    SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);          \
    SHR_EXIT() ;                                 \
  }                                              \
}

#define SHR_ALLOC_SET_ZERO(_ptr, _sz, _str, _formatting_string,_param2,_param3,_param4)   \
{                                                                           \
  SHR_ALLOC2(_ptr, _sz, SAL_ALLOC_F_ZERO, _str, _formatting_string,_param2,_param3,_param4);   \
}

#define SHR_ALLOC_SET_ZERO_ERR_EXIT(_ptr, _sz, _str, _formatting_string,_param2,_param3,_param4)   \
{                                                                           \
  SHR_ALLOC2_ERR_EXIT(_ptr, _sz, SAL_ALLOC_F_ZERO, _str, _formatting_string,_param2,_param3,_param4);   \
}

#define SHR_ALLOC_IGNORE_COUNTERS(_ptr, _sz, _str, _formatting_string, _param2, _param3, _param4)              \
{                                                                                                              \
  SHR_ALLOC2(_ptr, _sz, SAL_ALLOC_F_IGNORE_COUNTERS, _str, _formatting_string, _param2, _param3, _param4);     \
}

#define SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(_ptr, _sz, _str, _formatting_string, _param2, _param3, _param4)                        \
{                                                                                                                                          \
  SHR_ALLOC2_ERR_EXIT(_ptr, _sz, SAL_ALLOC_F_ZERO | SAL_ALLOC_F_IGNORE_COUNTERS, _str, _formatting_string, _param2, _param3, _param4);     \
}

#define SHR_FREE(_ptr)          \
{                                 \
  if ((_ptr) != NULL)             \
  {                               \
    sal_free((void *)(_ptr)) ;    \
    (_ptr) = NULL ;               \
  }                               \
}

#define SHR_FREE_IGNORE_COUNTERS(_ptr)                         \
{                                                              \
  if ((_ptr) != NULL)                                          \
  {                                                            \
    sal_free2((void *)(_ptr), SAL_ALLOC_F_IGNORE_COUNTERS) ;   \
    (_ptr) = NULL ;                                            \
  }                                                            \
}

#define SHR_VOID_FUNC_EXIT                             \
{                                                      \
  if (SHR_FAILURE(_func_rv))                           \
  {                                                    \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_func_rv) ,EMPTY,EMPTY,EMPTY) ;  \
  }                                                    \
  return;                                              \
}

#define SHR_VOID_NO_RETURN_FUNC_WO_EXIT                \
{                                                      \
  if (SHR_FAILURE(_func_rv))                           \
  {                                                    \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_func_rv) ,EMPTY,EMPTY,EMPTY) ;  \
  }                                                    \
}

#define SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH 1024

#endif
