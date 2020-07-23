/**
 * \file algo_stat_pp.h Internal DNX Managment APIs 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_STAT_PP_H_INCLUDED
/*
 * { 
 */
#define ALGO_STAT_PP_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

 /**
 * Resource name defines for algo stat_pp 
 * \see
 * dnx_stat_pp_init_templates 
 * {
 */
#define DNX_ALGO_STAT_PP_IRPP_PROFILE_ID     "STAT_PP_IRPP_PROFILE_ID"
#define DNX_ALGO_STAT_PP_ETPP_PROFILE_ID     "STAT_PP_ETPP_PROFILE_ID"
#define DNX_ALGO_STAT_PP_ETPP_COUNTING_PROFILE_ID     "STAT_PP_ETPP_COUNTING_PROFILE_ID"
#define DNX_ALGO_STAT_PP_ETPP_METERING_PROFILE_ID     "STAT_PP_ETPP_METERING_PROFILE_ID"
#define DNX_ALGO_STAT_PP_ERPP_PROFILE_ID     "STAT_PP_ERPP_PROFILE_ID"

/**
 * \brief - Init function, init all resources
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_stat_pp_init(
    int unit);

/*
 * } 
 */
#endif /* ALGO_STAT_PP_H_INCLUDED */
