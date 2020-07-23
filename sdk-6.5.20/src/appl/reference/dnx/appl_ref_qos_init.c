/** \file appl_ref_l2_init.c
 * 
 *
 * L@ application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/sand/sand_aux_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define TC_MASK  0x7
#define TC_SHIFT 0x2
#define ECN_MASK 0x3
#define EXP_MASK 0x7
#define PRI_MASK 0x7
#define PRI_SHIFT 0x1
#define CFI_MASK 0x1

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/**
 * \brief
 *     This function initializes l2 tagged PHB mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_l2_tag_phb_init(
    int unit)
{
    int phb_map_id;
    int map_opcode;
    uint32 flags;
    int pcp, cfi;
    bcm_qos_map_t ingress_phb_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &phb_map_id));

    /** allocate opcode  */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** set opcode map*/
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, phb_map_id));
    /** init tc/dp map, 1:1 maping  */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_PHB;
    for (pcp = 0; pcp <= dnx_data_qos.qos.packet_max_priority_get(unit); pcp++)
    {
        for (cfi = 0; cfi <= dnx_data_qos.qos.packet_max_cfi_get(unit); cfi++)
        {
            bcm_qos_map_t_init(&ingress_phb_map);
            ingress_phb_map.pkt_pri = pcp;
            ingress_phb_map.pkt_cfi = cfi;
            /** tc equal pcp, dp = cfi */
            ingress_phb_map.int_pri = pcp;
            ingress_phb_map.color = cfi;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, map_opcode));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes l2 untagged PHB mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_l2_untag_phb_init(
    int unit)
{
    int phb_map_id;
    uint32 flags;
    int tc = 0, dp = 0;
    bcm_qos_map_t ingress_phb_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &phb_map_id));

    /** init  tc=0 dp=0 */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_PHB;
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.int_pri = tc;
    ingress_phb_map.color = dp;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, phb_map_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes IPV4 PHB mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_ipv4_phb_init(
    int unit)
{
    int phb_map_id;
    int map_opcode;
    uint32 flags;
    int tos;
    bcm_qos_map_t ingress_phb_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &phb_map_id));

    /** allocate opcode */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map */
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, phb_map_id));

    /** init  tc/dp map, 1:1 maping  */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
    for (tos = 0; tos <= dnx_data_qos.qos.packet_max_dscp_get(unit); tos++)
    {
        bcm_qos_map_t_init(&ingress_phb_map);
        ingress_phb_map.dscp = tos;

        ingress_phb_map.int_pri = tos & TC_MASK;
        ingress_phb_map.color = 0;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes recycle PHB mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_rch_phb_init(
    int unit)
{
    int phb_map_id;
    int map_opcode;
    uint32 flags;
    int tc, dp;
    bcm_qos_map_t ingress_phb_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &phb_map_id));

    /** allocate opcode */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map */
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_RCH | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, phb_map_id));

    /** init  tc/dp map, 1:1 maping  */
    flags = BCM_QOS_MAP_RCH | BCM_QOS_MAP_PHB;
    for (tc = 0; tc <= dnx_data_qos.qos.packet_max_priority_get(unit); tc++)
    {
        for (dp = 0; dp <= dnx_data_qos.qos.packet_max_dp_get(unit); dp++)
        {
            bcm_qos_map_t_init(&ingress_phb_map);
            ingress_phb_map.remark_int_pri = tc;
            ingress_phb_map.remark_color = dp;
            ingress_phb_map.int_pri = tc;
            ingress_phb_map.color = dp;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, map_opcode));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes IPV6 PHB mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_ipv6_phb_init(
    int unit)
{
    int phb_map_id;
    int map_opcode;
    uint32 flags;
    int tos;
    bcm_qos_map_t ingress_phb_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &phb_map_id));

    /** allocate opcode*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map */
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, phb_map_id));

    /** init tc/dp map, 1:1 maping */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_PHB;
    for (tos = 0; tos <= dnx_data_qos.qos.packet_max_dscp_get(unit); tos++)
    {
        bcm_qos_map_t_init(&ingress_phb_map);
        ingress_phb_map.dscp = tos;

        ingress_phb_map.int_pri = tos & TC_MASK;
        ingress_phb_map.color = 0;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes MPLS PHB mapping,
 *     default initialization profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_mpls_phb_init(
    int unit)
{
    int phb_map_id;
    int map_opcode;
    uint32 flags;
    int exp;
    bcm_qos_map_t ingress_phb_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &phb_map_id));

    /** allocate opcode  */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map  */
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, phb_map_id));

    /** init tc/dp map, 1:1 maping */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB;
    for (exp = 0; exp <= dnx_data_qos.qos.packet_max_exp_get(unit); exp++)
    {
        bcm_qos_map_t_init(&ingress_phb_map);
        ingress_phb_map.exp = exp;

        ingress_phb_map.int_pri = exp;
        ingress_phb_map.color = 0;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_phb_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress l2 tagged qos remark mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_l2_tag_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int pcp, cfi;
    bcm_qos_map_t ingress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** allocate opcode */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map*/
    bcm_qos_map_t_init(&ingress_remark_map);
    ingress_remark_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, remark_map_id));

    /** init remark mapping  1:1 */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_REMARK;
    for (pcp = 0; pcp <= dnx_data_qos.qos.packet_max_priority_get(unit); pcp++)
    {
        for (cfi = 0; cfi <= dnx_data_qos.qos.packet_max_cfi_get(unit); cfi++)
        {
            bcm_qos_map_t_init(&ingress_remark_map);
            ingress_remark_map.pkt_pri = pcp;
            ingress_remark_map.pkt_cfi = cfi;

            ingress_remark_map.remark_int_pri = (pcp << PRI_SHIFT) + cfi;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, map_opcode));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress l2 untag qos remark mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_l2_untag_remark_init(
    int unit)
{
    int remark_map_id;
    uint32 flags;
    bcm_qos_map_t ingress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_REMARK;
    bcm_qos_map_t_init(&ingress_remark_map);
    /** init qos var to 0 for untagged packet*/
    ingress_remark_map.remark_int_pri = 0;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, remark_map_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress ipv4 qos remark mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_ipv4_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int tos;
    bcm_qos_map_t ingress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** allocate opcode */
    map_opcode = 0;
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map*/
    bcm_qos_map_t_init(&ingress_remark_map);
    ingress_remark_map.opcode = map_opcode;

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, remark_map_id));
    /** init remark mapping  1:1 */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
    for (tos = 0; tos <= dnx_data_qos.qos.packet_max_dscp_get(unit); tos++)
    {
        bcm_qos_map_t_init(&ingress_remark_map);
        ingress_remark_map.dscp = tos;

        ingress_remark_map.remark_int_pri = tos;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress recycle qos remark mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_rch_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int nwk_qos;
    bcm_qos_map_t ingress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** allocate opcode */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map*/
    bcm_qos_map_t_init(&ingress_remark_map);
    ingress_remark_map.opcode = map_opcode;

    flags = BCM_QOS_MAP_RCH | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, remark_map_id));
    /** init remark mapping  1:1 */
    flags = BCM_QOS_MAP_RCH | BCM_QOS_MAP_REMARK;
    for (nwk_qos = 0; nwk_qos <= dnx_data_qos.qos.packet_max_dscp_get(unit); nwk_qos++)
    {
        bcm_qos_map_t_init(&ingress_remark_map);
        ingress_remark_map.int_pri = nwk_qos;

        ingress_remark_map.remark_int_pri = nwk_qos;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress ipv6 qos remark mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_ipv6_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int tos;
    bcm_qos_map_t ingress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** allocate opcode*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map*/
    bcm_qos_map_t_init(&ingress_remark_map);
    ingress_remark_map.opcode = map_opcode;

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, remark_map_id));

    /** init remark mapping  1:1  */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
    for (tos = 0; tos <= dnx_data_qos.qos.packet_max_dscp_get(unit); tos++)
    {
        bcm_qos_map_t_init(&ingress_remark_map);
        ingress_remark_map.dscp = tos;

        ingress_remark_map.remark_int_pri = tos;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress mpls qos remark mapping,
 *     default initialization setting profile 0 to
 *     perform 1:1 mapping
 */
static shr_error_e
appl_dnx_qos_ingress_mpls_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int exp;
    bcm_qos_map_t ingress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** allocate opcode */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map*/
    bcm_qos_map_t_init(&ingress_remark_map);
    ingress_remark_map.opcode = map_opcode;

    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, remark_map_id));

    /*
     * init remark mapping  1:1
     */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
    for (exp = 0; exp <= dnx_data_qos.qos.packet_max_exp_get(unit); exp++)
    {
        bcm_qos_map_t_init(&ingress_remark_map);
        ingress_remark_map.exp = exp;

        ingress_remark_map.remark_int_pri = exp;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_remark_map, map_opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes egress remark L2 mapping.
 *     default initialization for profile 0
 *     perform 1:1 mapping for pcp-dei
 */
static shr_error_e
appl_dnx_qos_egress_l2_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    uint32 color;
    int var;
    bcm_qos_map_t egress_remark_map;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** opcode for pcp remark */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map  */
    bcm_qos_map_t_init(&egress_remark_map);
    egress_remark_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, remark_map_id));

    /** init outer pcp/dei map  */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_TWO_TAGS;
    for (color = 0; color <= dnx_data_qos.qos.packet_max_dp_get(unit); color++)
    {
        for (var = 0; var <= dnx_data_qos.qos.max_egress_var_get(unit); var++)
        {
            bcm_qos_map_t_init(&egress_remark_map);
            egress_remark_map.int_pri = var;
            egress_remark_map.color = color;
            system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
            /*
             *Jericho mode */
            if (system_headers_mode == 0)
            {
                /** qos 1:1 mapping */
                egress_remark_map.pkt_pri = var & PRI_MASK;
                egress_remark_map.pkt_cfi = color & CFI_MASK;
                egress_remark_map.inner_pkt_pri = var & PRI_MASK;;
                egress_remark_map.inner_pkt_cfi = color & CFI_MASK;
            }
            else
            {
                /** qos 1:1 mapping */
                egress_remark_map.pkt_pri = (var >> PRI_SHIFT) & PRI_MASK;
                egress_remark_map.pkt_cfi = var & CFI_MASK;
                egress_remark_map.inner_pkt_pri = (var >> PRI_SHIFT) & PRI_MASK;
                egress_remark_map.inner_pkt_cfi = var & CFI_MASK;
            }
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, map_opcode));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes egress remark IPv4 mapping.
 *     default initialization for profile 0
 *     perform 1:1 mapping for tos
 */
static shr_error_e
appl_dnx_qos_egress_ipv4_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int var;
    uint32 color;
    bcm_qos_map_t egress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init  */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** opcode IPV4 TOS remark  */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map  */
    bcm_qos_map_t_init(&egress_remark_map);
    egress_remark_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, remark_map_id));

    /** init  IPV4 tos mapping   */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
    for (color = 0; color <= dnx_data_qos.qos.packet_max_dp_get(unit); color++)
    {
        for (var = 0; var <= dnx_data_qos.qos.max_egress_var_get(unit); var++)
        {
            bcm_qos_map_t_init(&egress_remark_map);
            egress_remark_map.int_pri = var;
            egress_remark_map.color = color;
            /** qos 1:1 mapping  */
            egress_remark_map.dscp = var;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, map_opcode));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes egress remark IPv6 mapping.
 *     default initialization for profile 0
 *     perform 1:1 mapping for IPv6 TC
 */
static shr_error_e
appl_dnx_qos_egress_ipv6_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int var;
    uint32 color;
    bcm_qos_map_t egress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** opcode for IPV6 TC remark   */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map */
    bcm_qos_map_t_init(&egress_remark_map);
    egress_remark_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, remark_map_id));

    /** init IPV6 map */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
    for (color = 0; color <= dnx_data_qos.qos.packet_max_dp_get(unit); color++)
    {
        for (var = 0; var <= dnx_data_qos.qos.max_egress_var_get(unit); var++)
        {
            bcm_qos_map_t_init(&egress_remark_map);
            egress_remark_map.int_pri = var;
            egress_remark_map.color = color;
            /** qos 1:1 mapping  */
            egress_remark_map.dscp = var;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, map_opcode));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function initializes egress remark mpls exp mapping.
 *     default initialization for profile 0 to
 *     perform 1:1 mapping for mpls exp
 */
static shr_error_e
appl_dnx_qos_egress_mpls_remark_init(
    int unit)
{
    int remark_map_id;
    int map_opcode;
    uint32 flags;
    int var;
    uint32 color;
    bcm_qos_map_t egress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** opcode for MPLS exp remark */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_create(unit, flags, &map_opcode));

    /** init opcode map   */
    bcm_qos_map_t_init(&egress_remark_map);
    egress_remark_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, remark_map_id));

    /** init exp map */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
    for (color = 0; color <= dnx_data_qos.qos.packet_max_dp_get(unit); color++)
    {
        for (var = 0; var <= dnx_data_qos.qos.max_egress_var_get(unit); var++)
        {
            bcm_qos_map_t_init(&egress_remark_map);
            egress_remark_map.int_pri = var;
            egress_remark_map.color = color;
            /** qos 1:1 mapping  */
            egress_remark_map.exp = var & EXP_MASK;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, map_opcode));
        }
    }
    /** init second exp map */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_MPLS_SECOND | BCM_QOS_MAP_REMARK;
    for (color = 0; color <= dnx_data_qos.qos.packet_max_dp_get(unit); color++)
    {
        for (var = 0; var <= dnx_data_qos.qos.max_egress_var_get(unit); var++)
        {
            bcm_qos_map_t_init(&egress_remark_map);
            egress_remark_map.int_pri = var;
            egress_remark_map.color = color;
            /** qos 1:1 mapping  */
            egress_remark_map.exp = var & EXP_MASK;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, map_opcode));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function initializes egress next layer remark mapping.
 *     default initialization for profile 0 to
 *     initialize next layer is same current layer.
 */
static shr_error_e
appl_dnx_qos_egress_next_layer_remark_init(
    int unit)
{
    int remark_map_id;
    uint32 flags;
    int var;
    uint32 color = 0;
    bcm_qos_map_t egress_remark_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &remark_map_id));

    /** inheritance to next layer qos  */
    flags = BCM_QOS_MAP_REMARK;
    for (color = 0; color <= dnx_data_qos.qos.packet_max_dp_get(unit); color++)
    {
        for (var = 0; var <= dnx_data_qos.qos.max_egress_var_get(unit); var++)
        {
            bcm_qos_map_t_init(&egress_remark_map);
            egress_remark_map.int_pri = var;
            egress_remark_map.color = color;
            /** qos 1:1 mapping  */
            egress_remark_map.remark_int_pri = var;
            egress_remark_map.remark_color = color;
            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &egress_remark_map, remark_map_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos ingress vlan edit pcp dei mapping
 * default initialization for profile 0
 * to default 1:1.
 */
static shr_error_e
appl_dnx_qos_ingress_pcp_dei_explicit_map_init(
    int unit)
{
    int pcp_map_id;
    uint32 flags;
    int pcp, cfi;
    bcm_qos_map_t ingress_pcp_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2_VLAN_PCP;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &pcp_map_id));

    /** init pcp mapping  1:1 */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
    for (pcp = 0; pcp <= dnx_data_qos.qos.packet_max_priority_get(unit); pcp++)
    {
        for (cfi = 0; cfi <= dnx_data_qos.qos.packet_max_cfi_get(unit); cfi++)
        {
            bcm_qos_map_t_init(&ingress_pcp_map);
            ingress_pcp_map.int_pri = pcp;
            
            ingress_pcp_map.color = cfi;
            ingress_pcp_map.pkt_pri = pcp;
            ingress_pcp_map.pkt_cfi = cfi;

            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_pcp_map, pcp_map_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos ingress vlan edit pcp dei mapping from tc/dp
 * default initialization for profile 0
 * to default 1:1.
 */
static shr_error_e
appl_dnx_qos_ingress_pcp_dei_tc_dp_map_init(
    int unit)
{
    int pcp_map_id;
    uint32 flags;
    int tc, dp;
    bcm_qos_map_t ingress_pcp_map;

    SHR_FUNC_INIT_VARS(unit);

    /** profile 0 is allocated in qos moudle init   */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2_VLAN_PCP;
    SHR_IF_ERR_EXIT(bcm_qos_map_id_get_by_profile(unit, flags, 0, &pcp_map_id));

    /** init pcp mapping  1:1 */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED;
    for (tc = 0; tc <= dnx_data_qos.qos.packet_max_tc_get(unit); tc++)
    {
        for (dp = 0; dp <= dnx_data_qos.qos.packet_max_dp_get(unit); dp++)
        {
            bcm_qos_map_t_init(&ingress_pcp_map);
            ingress_pcp_map.int_pri = tc;
            
            ingress_pcp_map.color = dp;
            ingress_pcp_map.pkt_pri = tc;
            ingress_pcp_map.pkt_cfi = dp & CFI_MASK;

            SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &ingress_pcp_map, pcp_map_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
appl_dnx_qos_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** init ingress ipv4 phb mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_ipv4_phb_init(unit));

    /** init ingress ipv6 phb mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_ipv6_phb_init(unit));

    /** init ingress mpls phb mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_mpls_phb_init(unit));

    /** init ingress l2 tagged phb mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_l2_tag_phb_init(unit));

    /** init ingress l2 untagged phb mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_l2_untag_phb_init(unit));

    /** init ingress recycle header phb mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_rch_phb_init(unit));

    /** init ingress ipv4 remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_ipv4_remark_init(unit));

    /** init ingress ipv6 remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_ipv6_remark_init(unit));

    /** init ingress mpls remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_mpls_remark_init(unit));

    /** init ingress l2 tag remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_l2_tag_remark_init(unit));

    /** init ingress l2 untag remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_l2_untag_remark_init(unit));

    /** init ingress recycle header remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_rch_remark_init(unit));

    /** ingress ive, pcp dei explicit map */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_pcp_dei_explicit_map_init(unit));

    /** ingress ive, pcp dei tc/dp map */
    SHR_IF_ERR_EXIT(appl_dnx_qos_ingress_pcp_dei_tc_dp_map_init(unit));

    /** init egress ipv4 remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_egress_ipv4_remark_init(unit));

    /** init egress ipv6 remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_egress_ipv6_remark_init(unit));

    /** init egress mpls exp remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_egress_mpls_remark_init(unit));

    /** init egress l2 pcp/dei remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_egress_l2_remark_init(unit));

    /** init egress next layer remark mapping */
    SHR_IF_ERR_EXIT(appl_dnx_qos_egress_next_layer_remark_init(unit));

exit:
    SHR_FUNC_EXIT;
}
