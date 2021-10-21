/** \file diag_dnx_lag.c
 * 
 * diagnostics for lag
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm/trunk.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
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

sh_sand_option_t sh_dnx_flow_agg_arguments[] = {
    /*name           type                  desc            default      ext                                 valid range */
    {"ID",  SAL_FIELD_TYPE_INT32, "flow aggregate ID",     "0",         (void*)sh_enum_table_flow_agg_id,   "0-max"},
    {NULL}
};

sh_sand_man_t sh_dnx_flow_agg_man = {
    .brief = "Print Flow aggregate info",
    .examples = "ID=0",
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

/** see header file / man section */
shr_error_e
sh_dnx_flow_agg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flow_agg_id;
    int base_voq;
    int pool;
    int group;
    int trunk_group_is_used;
    bcm_flow_agg_info_t flow_agg_info;
    bcm_trunk_info_t trunk_info;
    int member_count;
    bcm_trunk_member_t *member_array = NULL;
    int max_nof_members_in_group;
    int flow_aggs_in_bundle = dnx_data_trunk.flow_agg.flow_aggs_in_bundle_get(unit);
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", flow_agg_id);

    /** set table headers */
    PRT_TITLE_SET("Flow Aggregate");
    PRT_INFO_ADD("For more info on trunk use relevant diag (tm lag) with pool and group");
    PRT_COLUMN_ADD("Base VOQ");
    PRT_COLUMN_ADD("Trunk Pool");
    PRT_COLUMN_ADD("Trunk Group");
    PRT_COLUMN_ADD("TC size");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

    /** get data */
    SHR_CLI_EXIT_IF_ERR(bcm_trunk_flow_agg_get(unit, flow_agg_id, 0, &flow_agg_info), "");

    /** print data */
    base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(flow_agg_info.base_voq);
    PRT_CELL_SET("%d", base_voq);
    BCM_TRUNK_ID_POOL_GET(pool, flow_agg_info.trunk_id);
    PRT_CELL_SET("%d", pool);
    BCM_TRUNK_ID_GROUP_GET(group, flow_agg_info.trunk_id);
    PRT_CELL_SET("%d", group);
    PRT_CELL_SET("%d", flow_agg_info.nof_cosq_levels);
    PRT_COMMITX;

    /** get trunk size */
    SHR_CLI_EXIT_IF_ERR(dnx_trunk_is_used_get(unit, pool, group, &trunk_group_is_used), "");
    if (!trunk_group_is_used)
    {
        SHR_EXIT();
    }
    max_nof_members_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    member_array = sal_alloc(sizeof(bcm_trunk_member_t) * max_nof_members_in_group, "member_array");
    SHR_CLI_EXIT_IF_ERR(bcm_trunk_get
                        (unit, flow_agg_info.trunk_id, &trunk_info, max_nof_members_in_group, member_array,
                         &member_count), "");

    if (member_count == 0)
    {
        LOG_CLI((BSL_META("The Trunk that is associated with this Flow aggregate is currently empty\n"
                          "At this point no further mapping can be presented\n"
                          "Add members to the trunk and try again this diag\n")));
        SHR_EXIT();
    }

    /** set table headers */
    PRT_TITLE_SET("Flow Aggregate bundle full mapping");
    PRT_COLUMN_ADD("Flow Agg ID");
    PRT_COLUMN_ADD("Member Index");
    PRT_COLUMN_ADD("TC");
    PRT_COLUMN_ADD("VOQ");
    PRT_COLUMN_ADD("System Port ID");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    for (int flow_agg_index = 0; flow_agg_index < flow_aggs_in_bundle; ++flow_agg_index)
    {
        int current_flow_agg;
        /** print flow agg ID, find bundle from provided input and parse the whole bundle */
        current_flow_agg = flow_agg_id / flow_aggs_in_bundle * flow_aggs_in_bundle + flow_agg_index;
        PRT_CELL_SET("%d", current_flow_agg);

        for (int member_index = 0; member_index < member_count; ++member_index)
        {
            if (member_index != 0)
            {
                PRT_CELL_SKIP(1);
            }
            /** print member index */
            PRT_CELL_SET("%d", member_index);

            for (int tc = 0; tc < flow_agg_info.nof_cosq_levels; ++tc)
            {
                int voq;
                uint32 base_queue_quartet = 0;
                uint32 master_sysport = 0;
                if (tc != 0)
                {
                    PRT_CELL_SKIP(2);
                }
                /** print tc */
                PRT_CELL_SET("%d", tc);
                /** print VOQ */
                voq =
                    base_voq + member_index * flow_aggs_in_bundle * flow_agg_info.nof_cosq_levels +
                    flow_agg_index * flow_agg_info.nof_cosq_levels + tc;
                PRT_CELL_SET("%d", voq);

                /** print mapped port?? check with Leon what is the available mapping in this mode (Flow based forwarding) (is VOQ -> system port mapping available) */
                /** TBD */
                base_queue_quartet = voq / min_bundle_size;
                /** assuming voqs are symetric! */
                SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get
                                (unit, 0, base_queue_quartet, &master_sysport));
                PRT_CELL_SET("%d", master_sysport);

                /** new line */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
