/** \file src/bcm/dnx/cosq/ingress/rate_class_common.h
 * 
 * 
 * Common functionality for VOQ and VSQ rate classes
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _RATE_CLASS_COMMON_H_INCLUDED_
#define _RATE_CLASS_COMMON_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include "ingress_congestion_dbal.h"

/**
 * \brief -
 *     Sets WRED parameters per rate-class and drop precedence,
 *     including wred-enable.
 */
int dnx_rate_class_common_wred_set(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_info_t * wred);

/**
 * \brief -
 * get WRED parameters and translate them to user units
 */
int dnx_rate_class_common_wred_get(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_info_t * info);

/** 
 * \brief -  Set WRED average configuration
 */
int dnx_rate_class_common_wred_avrg_set(
    int unit,
    dbal_tables_e wred_table_id,
    dbal_tables_e avrg_table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    uint32 gain);

/** 
 * \brief -  get WRED average configuration
 */
int dnx_rate_class_common_wred_avrg_get(
    int unit,
    dbal_tables_e wred_table_id,
    dbal_tables_e avrg_table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    uint32 *gain);

#endif
