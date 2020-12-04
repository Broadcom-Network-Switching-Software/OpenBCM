/*
 * appl_ref_mtu_compressed_layer_type.h
 *
 *  Created on: Aug 6, 2018
 *      Author: dp889757
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/diag.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <shared/utilex/utilex_seq.h>

/**
 * \brief - This function initializes compressed layer type values:
 * bcmFieldLayerTypeEth  - 1
 * bcmFieldLayerTypeArp  - 1
 * bcmFieldLayerTypeIp4  - 2
 * bcmFieldLayerTypeIp6  - 2
 * bcmFieldLayerTypeMpls - 2
 *
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_mtu_compressed_layer_type_init(
    int unit);
