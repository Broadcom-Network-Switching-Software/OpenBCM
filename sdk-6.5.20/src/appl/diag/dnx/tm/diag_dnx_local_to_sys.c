/** \file diag_dnx_local_to_sys.c
 * 
 * Diagnostics for system ports.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_PORT

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
/** allow drv.h include excplictly for system*/
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h> /** FORMAT_PBMP_MAX */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
/** sal */
#include <sal/appl/sal.h>
/*
 * }
 */

sh_sand_man_t sh_dnx_ingress_local_to_sys_man = {
    .brief = "Print mapping between local port and system port",
    .full = NULL,
    .synopsis = NULL,
    .examples = NULL
};

/**
 * \brief - Print mapping between local port and system port.
 */
shr_error_e
sh_dnx_ingress_local_to_sys_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t logical_ports;
    int local_port_id = -1;
    bcm_gport_t gport;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&gport_info, 0, sizeof(gport_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING, 0, &logical_ports));

    /*
     * Print table header
     */
    PRT_TITLE_SET("Local Port to System Port Mapping");
    PRT_COLUMN_ADD("Local Port");
    PRT_COLUMN_ADD("System Port");
    PRT_COLUMN_ADD("Port Name");

    BCM_PBMP_ITER(logical_ports, local_port_id)
    {
        /** Convert local_port_id to gport */
        BCM_GPORT_LOCAL_SET(gport, local_port_id);
        /** Get gport info (including system port) */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", local_port_id);
        PRT_CELL_SET("%d", gport_info.sys_port);
        PRT_CELL_SET("%s", BCM_PORT_NAME(unit, local_port_id));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
