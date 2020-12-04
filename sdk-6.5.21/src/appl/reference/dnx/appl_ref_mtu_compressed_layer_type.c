/*
 * appl_ref_mtu_compressed_layer_type.c
 *
 *  Created on: Aug 6, 2018
 *      Author: dp889757
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

/** soc */
#include <soc/schanmsg.h>
#include <shared/utilex/utilex_seq.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/diag.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include "appl_ref_mtu_compressed_layer_type.h"
/** bcm */
#include <bcm/field.h>
#include <bcm/switch.h>
#include <bcm/rx.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

shr_error_e
appl_dnx_mtu_compressed_layer_type_init(
    int unit)
{

    bcm_field_layer_type_t l2_layer_types[] = { bcmFieldLayerTypeEth, bcmFieldLayerTypeArp };
    bcm_field_layer_type_t l3_layer_types[] = { bcmFieldLayerTypeIp4, bcmFieldLayerTypeIp6, bcmFieldLayerTypeMpls };
    int layer_type_index;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    int l2_num_els = sizeof(l2_layer_types) / sizeof(bcm_field_layer_type_t);
    int l3_num_els = sizeof(l3_layer_types) / sizeof(bcm_field_layer_type_t);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Creating a compressed forwarding layer type 
     */
    key.type = bcmSwitchLinkLayerMtuFilter;

    for (layer_type_index = 0; layer_type_index < l2_num_els; layer_type_index++)
    {
        key.index = l2_layer_types[layer_type_index];

        info.value = 1;

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    }

    for (layer_type_index = 0; layer_type_index < l3_num_els; layer_type_index++)
    {
        key.index = l3_layer_types[layer_type_index];

        info.value = 2;

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    }

exit:
    SHR_FUNC_EXIT;
}
