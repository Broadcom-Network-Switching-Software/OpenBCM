/** \file diag_dnx_lag.c
 * 
 * diagnostics for lag
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

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm/trunk.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
/** sal */
#include <sal/appl/sal.h>

#include "diag_dnx_lag.h"

shr_error_e
sh_dnx_lag_init_cb(
    int unit)
{
    bcm_gport_t trunk_gport_1;
    bcm_gport_t trunk_gport_2;
    int ports_1[2] = { 13, 14 };
    int ports_2[2] = { 15, 16 };

    int pool_index = 0;
    int group_index_1 = 21;
    int group_index_2 = 22;
    int trunk_id_1;
    int trunk_id_2;
    int nof_members = 2;
    int member;
    bcm_trunk_member_t members_1[2];
    bcm_trunk_member_t members_2[2];
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_trunk_info_t trunk_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_trunk_info_t_init(&trunk_info);

    BCM_TRUNK_ID_SET(trunk_id_1, pool_index, group_index_1);
    BCM_TRUNK_ID_SET(trunk_id_2, pool_index, group_index_2);

    SHR_IF_ERR_EXIT(bcm_trunk_create(unit, 0, &trunk_id_1));
    SHR_IF_ERR_EXIT(bcm_trunk_create(unit, 0, &trunk_id_2));

    BCM_GPORT_TRUNK_SET(trunk_gport_1, trunk_id_1);
    BCM_GPORT_TRUNK_SET(trunk_gport_2, trunk_id_2);

    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, trunk_gport_1, trunk_gport_2, 1));

    /** Set trunk header type - Set trunk header types for both directions to BCM_SWITCH_PORT_HEADER_TYPE_ETH */
    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_set(unit, trunk_gport_1, key, value));
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_set(unit, trunk_gport_2, key, value));

    /** Create members array */
    for (member = 0; member < nof_members; ++member)
    {
        bcm_gport_t system_port_gport;

        bcm_trunk_member_t_init(&members_1[member]);
        /** convert system port to system port gport and set it */
        BCM_GPORT_SYSTEM_PORT_ID_SET(system_port_gport, ports_1[member]);
        members_1[member].gport = system_port_gport;

        /** init member */
        bcm_trunk_member_t_init(&members_2[member]);
        /** convert system port to system port gport and set it */
        BCM_GPORT_SYSTEM_PORT_ID_SET(system_port_gport, ports_2[member]);
        members_2[member].gport = system_port_gport;
    }

    /** Configure trunk info */
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;

    /** Set trunk with members */
    SHR_IF_ERR_EXIT(bcm_trunk_set(unit, trunk_id_1, &trunk_info, nof_members, members_1));
    SHR_IF_ERR_EXIT(bcm_trunk_set(unit, trunk_id_2, &trunk_info, nof_members, members_2));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_lag_arguments[] = {
    /*name    type                  desc                 default ext                         valid range */
    {"pool",  SAL_FIELD_TYPE_INT32, "lag pool",          "-1",   (void*)sh_enum_table_lag_pool,  "0-max"},
    {"group", SAL_FIELD_TYPE_INT32, "lag group in pool", "-1",   NULL},
    {NULL}
};

sh_sand_man_t sh_dnx_lag_man = {
    .brief = "Print LAG pools/groups info",
    .examples = "pool=0 group=21\n" "pool=0\n",
    .init_cb = sh_dnx_lag_init_cb,
};
/* *INDENT-ON* */

/** see header file / man section */
shr_error_e
sh_dnx_lag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 in_pool, in_group;
    bcm_trunk_t trunk_id;
    bcm_trunk_info_t trunk_info;
    int member_count;
    bcm_trunk_member_t *member_array = NULL;
    int pool;
    int group;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("pool", in_pool);
    SH_SAND_GET_INT32("group", in_group);

    /** Print lag info table */
    PRT_TITLE_SET("Lag info");
    PRT_COLUMN_ADD("Pool");
    PRT_COLUMN_ADD("Max Groups");
    PRT_COLUMN_ADD("Max Members");
    PRT_COLUMN_ADD("Group");
    PRT_COLUMN_ADD("PSC");
    PRT_COLUMN_ADD("System port");
    PRT_COLUMN_ADD("Is Disabled");

    for (pool = 0; pool < dnx_data_trunk.parameters.nof_pools_get(unit); ++pool)
    {
        int max_nof_groups_in_pool = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_groups_in_pool;
        int max_nof_members_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
        int first_group_in_pool = TRUE;
        if ((in_pool != -1) && (in_pool != pool))
            continue;

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) * max_nof_members_in_group, "member_array");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%d", pool);
        PRT_CELL_SET("%d", max_nof_groups_in_pool);
        PRT_CELL_SET("%d", max_nof_members_in_group);

        for (group = 0; group < max_nof_groups_in_pool; ++group)
        {
            int current_member;
            int trunk_group_is_used;
                /** Get info */
            SHR_IF_ERR_EXIT(dnx_trunk_is_used_get(unit, pool, group, &trunk_group_is_used));
            if (!trunk_group_is_used)
                continue;

            if ((in_group != -1) && (in_group != group))
                continue;

            /** get trunk info */
            BCM_TRUNK_ID_SET(trunk_id, pool, group);
            SHR_CLI_EXIT_IF_ERR(bcm_trunk_get(unit, trunk_id, &trunk_info, max_nof_members_in_group, member_array,
                                              &member_count), "");
            if (first_group_in_pool == TRUE)
            {
                first_group_in_pool = FALSE;
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(3);
            }
            PRT_CELL_SET("%d", group);
            if (trunk_info.psc == BCM_TRUNK_PSC_C_LAG)
            {
                PRT_CELL_SET("BCM_TRUNK_PSC_C_LAG");
            }
            else if (trunk_info.psc == BCM_TRUNK_PSC_SMOOTH_DIVISION)
            {
                PRT_CELL_SET("BCM_TRUNK_PSC_SMOOTH_DIVISION");
            }
            else if (trunk_info.psc == BCM_TRUNK_PSC_PORTFLOW)
            {
                PRT_CELL_SET("BCM_TRUNK_PSC_PORTFLOW");
            }
            else
            {
                PRT_CELL_SET("%d", trunk_info.psc);
            }

            for (current_member = 0; current_member < member_count; ++current_member)
            {
                if (current_member != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(5);
                }
                            /** Print Lag members */
                PRT_CELL_SET("0x%08x", member_array[current_member].gport);
                PRT_CELL_SET("%s",
                             sh_sand_bool_str((member_array[current_member].flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)));
            }
        }
        sal_free(member_array);
        member_array = NULL;
    }
    PRT_COMMITX;

exit:
    if (member_array)
    {
        sal_free(member_array);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}
