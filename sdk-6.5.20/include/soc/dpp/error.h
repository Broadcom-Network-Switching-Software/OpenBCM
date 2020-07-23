/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __DPP_ERROR_H__
#define __DPP_ERROR_H__

#include <soc/error.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>


int
translate_sand_success_failure(const SOC_SAND_SUCCESS_FAILURE result);


int
handle_sand_result(uint32 soc_sand_result);

#define SOC_SAND_IF_ERR_RETURN(_sand_ret) \
    do { \
        soc_error_t _rv = handle_sand_result(_sand_ret); \
        if (_rv != SOC_E_NONE) { \
            return _rv; \
        } \
    } while (0)

#define SOC_SAND_IF_ERR_EXIT(_sand_ret) \
    do { \
        soc_error_t _rv = handle_sand_result(_sand_ret); \
        if (_rv != SOC_E_NONE) { \
            goto exit; \
        } \
    } while (0)


#define SOC_SAND_IF_FAIL_RETURN(_sand_success) \
    do { \
        _rv = translate_sand_success_failure(_sand_success); \
        if (_rv != SOC_E_NONE) { \
            return _rv; \
        } \
    } while (0)


#define SOC_SAND_IF_FAIL_EXIT(_sand_success) \
    do { \
        _rv = translate_sand_success_failure(_sand_success); \
        if (_rv != SOC_E_NONE) { \
            goto exit; \
        } \
    } while (0)


#define SOC_SAND_IF_ERR_OR_FAIL_RETURN(_sand_ret, _sand_success) \
    do { \
        SOC_SAND_IF_ERR_RETURN(_sand_ret); \
        SOC_SAND_IF_FAIL_RETURN(_sand_success); \
    } while (0)

#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)
        
#define SOC_SAND_IF_ERROR_RETURN(_sand_ret)                                                \
    do {                                                                               \
        soc_error_t _rv = handle_sand_result(_sand_ret);                               \
        if (_rv != SOC_E_NONE) {                                                       \
	        _bsl_warn(_BSL_SOCDNX_MSG("%d"),_rv);                                       \
            return _rv;                                                                \
        }                                                                              \
    } while (0)

#endif 
