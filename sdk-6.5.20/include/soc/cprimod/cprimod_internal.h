
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _CPRIMOD_INTERNAL_H_
#define _CPRIMOD_INTERNAL_H_

#include <shared/bsl.h>
#include <soc/cprimod/cprimod.h>
#include <appl/diag/diag.h>


#define CPRIMOD_DEBUG_ERROR(stuff_) \
    LOG_ERROR(BSL_LS_SOC_PORT,stuff_)

#define CPRIMOD_DEBUG_WARN(stuff_) \
    LOG_WARN(BSL_LS_SOC_PORT, stuff_)

#define CPRIMOD_RETURN_WITH_ERR(A,B) \
    do {   \
        CPRIMOD_DEBUG_ERROR(B);\
        return A; \
    } while (0)


#define CPRIMOD_NULL_CHECK(A) \
    do {   \
        if ((A) == NULL) \
        { CPRIMOD_RETURN_WITH_ERR(SOC_E_PARAM,(BSL_META("NULL Parameter\n"))); } \
    } while (0)


/*!
 * cprimod_port_cpm_type_get
 *
 * @brief Get the pm type of a port.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  pm_type         - dispatch type
 */
int cprimod_port_cpm_type_get(int unit, int port, cprimod_dispatch_type_t* pm_type);

#endif /*_PORTMOD_INTERNAL_H_*/
