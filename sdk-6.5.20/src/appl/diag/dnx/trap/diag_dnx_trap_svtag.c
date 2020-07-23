/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * diag_dnx_trap_svtag.c
 *
 *  Created on: APr 11, 2018
 *      Author: sm888907
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_trap.h"

/**
* \brief
*    Init callback function for "trap svtag" diag command
*    1) Create an ingress user defined trap
*    2) Set svtag trap
* \param [in] unit - unit ID
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
diag_dnx_trap_svtag_init_cb(
    int unit)
{
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t action_gport;
    int sci = 0xA, sci_profile = 1;
    int err_code = bcmSwitchSvtagErrCodeSpTcamMiss, err_code_profile = 3;
    int trap_id;
    bcm_rx_trap_svtag_key_t svtag_trap_key;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    int svtag_trap_en;

    SHR_FUNC_INIT_VARS(unit);

    svtag_trap_en = dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_svtag_trap_en);
    if (svtag_trap_en == 0)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COLOR;
    trap_config.color = 2;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    BCM_GPORT_TRAP_SET(action_gport, trap_id, 15, 0);

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);

    key.type = bcmSwitchSvtagSciProfile;
    key.index = sci;

    info.value = sci_profile;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = err_code;

    info.value = err_code_profile;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    svtag_trap_key.sci_profile = sci_profile;
    svtag_trap_key.error_code_profile = err_code_profile;
    svtag_trap_key.pkt_type = bcmSvtagPktTypeNonKayMgmt;
    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_trap_svtag_cmd_man = {
    .brief = "Show svtag traps information\n",
    .full = "Lists of all svtag traps configured \n",
    .init_cb = diag_dnx_trap_svtag_init_cb
};

/**
 * See diag_dnx_trap.h
 */
shr_error_e
sh_dnx_trap_svtag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    bcm_gport_t action_gport;
    bcm_rx_trap_svtag_key_t svtag_trap_key;
    int pkt_type_idx, sci_idx, err_code_idx, flag_idx;
    int svtag_trap_en;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    svtag_trap_en = dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_svtag_trap_en);
    if (svtag_trap_en == 0)
    {
        SHR_EXIT();
    }

    PRT_TITLE_SET("List of SVTAG Traps configured");
    PRT_COLUMN_ADD("SEC Type");
    PRT_COLUMN_ADD("Packet Type ");
    PRT_COLUMN_ADD("SCI Profile");
    PRT_COLUMN_ADD("Error Code Profile");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Fwd Strength");
    PRT_COLUMN_ADD("Snp Strength");

    for (flag_idx = 0; flag_idx < 2; flag_idx++)
    {
        bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
        svtag_trap_key.flags = flag_idx;

        for (pkt_type_idx = 0; pkt_type_idx <= bcmSvtagPktTypeKayMgmt; pkt_type_idx++)
        {
            svtag_trap_key.pkt_type = pkt_type_idx;

            for (err_code_idx = 0; err_code_idx < dnx_data_trap.ingress.nof_error_code_profiles_get(unit);
                 err_code_idx++)
            {
                svtag_trap_key.error_code_profile = err_code_idx;

                for (sci_idx = 0; sci_idx < dnx_data_trap.ingress.nof_sci_profiles_get(unit); sci_idx++)
                {
                    svtag_trap_key.sci_profile = sci_idx;
                    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_get(unit, &svtag_trap_key, &action_gport));

                    if (BCM_GPORT_TRAP_GET_ID(action_gport) != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        if (svtag_trap_key.flags == 0)
                        {
                            PRT_CELL_SET("MACSEC");
                        }
                        else
                        {
                            PRT_CELL_SET("IPSEC");
                        }

                        PRT_CELL_SET("%d", svtag_trap_key.pkt_type);
                        PRT_CELL_SET("%d", svtag_trap_key.sci_profile);
                        PRT_CELL_SET("%d", svtag_trap_key.error_code_profile);
                        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_ID(action_gport));
                        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(action_gport));
                        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport));
                    }
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
