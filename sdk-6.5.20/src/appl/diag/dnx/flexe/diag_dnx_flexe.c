/**
 * \file diag_dnx_flexe.c Contains all of the flexe diag commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 */
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <shared/bsl.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
/** allow soc/mcm/memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
#include <soc/drv.h> /** needed for access */
#include <soc/mcm/memregs.h> /** needed for SOC_REG_NAME */
#include <bcm/l3.h>
#include <appl/diag/diag.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <shared/utilex/utilex_bitstream.h>

#include "diag_dnx_flexe.h"
#include "diag_flexe_adapt.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port_flexe.h>

/*************
 * DEFINES   *
 */

#define DNX_DIAG_FLEXE_CORE_NOF_PORTS        8
#define DNX_DIAG_FLEXE_CORE_NOF_INSTANCES    8
#define DNX_DIAG_FLEXE_CORE_NOF_LPHYS        8

#define DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS    80
#define DNX_DIAG_FLEXE_CORE_NOF_SUBCALS      10
#define DNX_DIAG_FLEXE_SAR_NOF_TIMESLOTS     80

#define DNX_DIAG_FLEXE_INVALID_PORT_ID       0xF
#define DNX_DIAG_FLEXE_CORE_NOF_CHANNELS     80
#define DNX_DIAG_FLEXE_CORE_MAX_NOF_CHANNELS (DNX_DIAG_FLEXE_CORE_NOF_CHANNELS+2)

#define DNX_DIAG_FLEXE_66BSWITCH_NOF_SUBSYS  3

#ifdef BE_HOST
/* incompatible_cast coverity check should be disabled for this MACRO */
#define DNX_DIAG_SHR_BITCOPY_RANGE_U64_TO_U32(_dest, _dest_offset,_src, _src_offset, _num_bits) \
        dnx_diag_bitop_range_copy_u64_to_u32_big_endian(_dest, _dest_offset,_src, _src_offset, _num_bits)
#else
/* incompatible_cast coverity check should be disabled for this MACRO */
#define DNX_DIAG_SHR_BITCOPY_RANGE_U64_TO_U32(_dest, _dest_offset,_src, _src_offset, _num_bits) \
        SHR_BITCOPY_RANGE(_dest, _dest_offset,(SHR_BITDCL *)_src, _src_offset, _num_bits)
#endif

#define DNX_DIAG_FLEXE_DEMUX_GROUP_INFO_GET(_val, _phy, _min_phy) \
    SHR_BITCOPY_RANGE(_min_phy, 0, _val, _phy*4, 4)

#define DNX_DIAG_FLEXE_RX_LF_SEL_GET(_lf_sel, _flexe_port, _bcm_port) \
    SHR_BITCOPY_RANGE(_bcm_port, 0, _lf_sel, ((_flexe_port)*3), 3)

#define DNX_DIAG_FLEXE_MULTI_DESKEW_GRP_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, ((_idx%4)*8), 8)

#define DNX_DIAG_FLEXE_DEMUX_REORDER_INFO_GET(_val, _lphy, _dest) \
    DNX_DIAG_SHR_BITCOPY_RANGE_U64_TO_U32(_dest, 0, _val, ((_lphy)*8), 8)

#define DNX_DIAG_FLEXE_DEMUX_GROUP_CFG_GET(_val, _lphy, _min_lphy) \
    DNX_DIAG_SHR_BITCOPY_RANGE_U64_TO_U32(_min_lphy, 0, _val, ((_lphy)*4), 4)

#define DNX_DIAG_FLEXE_MUX_MODE_GET(_val, _idx, _mode) \
    SHR_BITCOPY_RANGE(_mode, 0, _val, ((_idx)*2), 2)

#define DNX_DIAG_FLEXE_MUX_CPHY2_CFG_GET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_lphy, 0, _val, ((_idx)*4), 3)

#define DNX_DIAG_FLEXE_MUX_EN_SEL_CFG(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _sel, 0, 3)

#define DNX_DIAG_FLEXE_MUX_GROUP_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, ((_idx)*4), 4)

#define DNX_DIAG_FLEXE_MUX_INST_SEL_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, (_idx*4), 3)

#define DNX_DIAG_FLEXE_PHY_SEL_CFG_GET(_cfg, _idx, _val) \
    SHR_BITCOPY_RANGE(_val, 0, _cfg, ((_idx)*4), 4)

#define DNX_DIAG_FLEXE_RX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port) \
    SHR_BITCOPY_RANGE(_flexe_port, 0, _map, ((_bcm_port)*4), 4)

#define DNX_DIAG_FLEXE_TX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port) \
    DNX_DIAG_FLEXE_RX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port)

#define DNX_DIAG_FLEXE_MUX_MSI_CFG_GET(_val, _idx, _channel) \
    SHR_BITCOPY_RANGE(_channel, 0, _val, (((_idx)/20)*8), 7)

#define DNX_DIAG_FLEXE_MUX_TSNUM_CFG_GET(_val, _idx, _tsnum) \
    SHR_BITCOPY_RANGE(_tsnum, 0, _val, (((_idx)/20)*8), 7)

#define DNX_DIAG_FLEXE_DEMUX_CALENDAR_CFG_GET(_val, _timeslot, _chanel) \
    SHR_BITCOPY_RANGE(_chanel, 0, _val, ((_timeslot)*8), 8)

#define DNX_DIAG_FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_GET(_val, _chanel, _lphymap) \
    SHR_BITCOPY_RANGE(_lphymap, 0, _val, ((_chanel)*8), 8)

#define DNX_DIAG_FLEXE_DEMUX_RDRULE_GET(_val, _subcal, _timeslot) \
    SHR_BITCOPY_RANGE(_timeslot, 0, _val, ((_subcal)*8), 8)

#define DNX_DIAG_FLEXE_DEMUX_TS_TO_WR_TBL_ID(_timeslot) \
    (((_timeslot)/40)*5 + (_timeslot) % 5)

#define DNX_DIAG_FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(_timeslot) \
    ((((_timeslot) % 40)/5)*10)

#define DNX_DIAG_FLEXE_MUX_CYCLE_CFG_GET(_val, _idx, _cycle) \
    SHR_BITCOPY_RANGE(_cycle, 0, _val, (((_idx)/20)*8), 7)

#define DNX_DIAG_FLEXE_MUX_BLK_CFG_GET(_val, _idx, _blk) \
    SHR_BITCOPY_RANGE(_blk, 0, _val, (_idx*4), 4)

#define DNX_DIAG_FLEXE_MUX_CHANNEL_BELONG_FLEXE_GET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_lphy, 0, _val, (_idx*4), 4)

#define DNX_DIAG_FLEXE_OP_A_OR_B(_sel, _a, _b, ...) \
    if (_sel == bcmPortFlexeGroupCalA) \
    { \
        SHR_IF_ERR_EXIT(_a(__VA_ARGS__)); \
    } \
    else \
    { \
        SHR_IF_ERR_EXIT(_b(__VA_ARGS__)); \
    } \

/**
 * \brief
 * required info per interface type in order parse ucode_port SoC property
 */
typedef struct
{
    char *interface_type_name;
    int nof_lanes;
} diag_flexe_phy_info_t;

typedef enum
{
    diag_flexe_group_rx,
    diag_flexe_group_tx,
    diag_flexe_group_all
} diag_flexe_direction_e;

typedef enum
{
    diag_flexe_calendar_a,
    diag_flexe_calendar_b,
} diag_flexe_calendar_e;

typedef enum
{
    diag_virtual_client_none,
    diag_virtual_client_mac,
    diag_virtual_client_sar
} diag_virtual_client_type_e;

typedef enum
{
    diag_flexe_oh_group_id,
    diag_flexe_oh_phy_id,
    diag_flexe_oh_c,
    diag_flexe_oh_cr,
    diag_flexe_oh_ca
} diag_flexe_oh_type_e;

typedef struct
{
    char *row_desc;
    uint32 reg;
    uint32 field;
    char *row_comments;
} diag_flexe_dump_reg_row_t;

typedef struct
{
    char *row_desc;
    soc_mem_t mem;
    uint32 base;
} diag_flexe_dump_mem_row_t;

/**
 * \brief enum that represents FlexE MUX mode\n 
 */
typedef enum
{
    DNX_DIAG_FLEXE_MUX_MODE_50G = 0,
    DNX_DIAG_FLEXE_MUX_MODE_100G = 1,
    DNX_DIAG_FLEXE_MUX_MODE_100G_2ND_PHY = 2,
} dnx_diag_flexe_mux_mode_e;

/* *INDENT-OFF* */
sh_sand_enum_t flexe_group_id_table[] = {
    {
         .string = "max",
         .desc = "Number of FlexE group supported",
         .value = 0,
         .plugin_str = "DNX_DATA.nif.flexe.nof_groups-1"
     },
    {NULL}
};

static sh_sand_enum_t dnx_flexe_core_module_enum_table[] = {
    {"mcmaxrx",         diag_flexe_core_module_mcmaxrx,         "subsys_mac.mac.mcmacrx"},
    {"mcmaxtx",         diag_flexe_core_module_mcmaxtx,         "subsys_mac.mac.mcmactx"},
    {"b66sar_rx",       diag_flexe_core_module_b66sar_rx,       "subsys_mac.mac.b66sar_rx"},
    {"b66sar_tx",       diag_flexe_core_module_b66sar_tx,       "subsys_mac.mac.b66sar_tx"},
    {"cpb_egress_mac",  diag_flexe_core_module_cpb_egress_mac,  "subsys_mac.cpb_egress_mac.cpb_egress"},
    {"flexe_macrx",     diag_flexe_core_module_flexe_macrx,     "subsys_flexe.flexe_mac.flexe_macrx"},

    {"sar",             diag_flexe_core_module_sar,             "sar related counter"},
    {"all",             diag_flexe_core_module_all,             "all flexe core module"},
    {"none",            diag_flexe_core_module_none,            "no flexe core module is chosed"},
    {NULL}
};

static sh_sand_enum_t dnx_flexe_core_conf_enum_table[] = {
    {"b66switch",    diag_flexe_core_conf_b66switch,    "b66switch"},
    {"b66sar",       diag_flexe_core_conf_b66sar,       "b66sar"},
    {"oam_alarm",    diag_flexe_core_conf_oam_alarm,    "oam_alarm"},
    {"oh_alarm",     diag_flexe_core_conf_oh_alarm,     "oh_alarm"},
    {"oam_counter",  diag_flexe_core_conf_oam_counter,  "oam_counter"},
    {NULL}
};


static sh_sand_enum_t dnx_flexe_print_mode_enum_table[] = {
    {"default", diag_flexe_counter_print_default,   "print counter using default mode"},
    {"nozero",  diag_flexe_counter_print_nozero,    "Only print no zero counter"},
    {"all",     diag_flexe_counter_print_all,       "print all counter"},
    {NULL}
};


static sh_sand_enum_t dnx_flexe_direction_enum_table[] = {
    {"rx",  diag_flexe_group_rx,   "BCM_FLEXE_PORT_GROUP_RX"},
    {"tx",  diag_flexe_group_tx,   "BCM_FLEXE_PORT_GROUP_TX"},
    {"all", diag_flexe_group_all,  "BCM_FLEXE_PORT_GROUP_ALL"},
    {NULL}
};

static sh_sand_enum_t dnx_flexe_calendar_enum_table[] = {
    {"CalA",  diag_flexe_calendar_a,    "calendar A"},
    {"CalB",  diag_flexe_calendar_b,    "calendar B"},
    {NULL}
};


/* *INDENT-OFF* */
static sh_sand_enum_t dnx_virtual_client_enum_table[] = {
    {"none", diag_virtual_client_none,  "Not Virtual clinent"},
    {"mac",  diag_virtual_client_mac,   "FlexE Virtual clinent of MAC"},
    {"sar",  diag_virtual_client_sar,   "FlexE Virtual clinent of SAR"},
    {NULL}
};

static sh_sand_enum_t dnx_flexe_oh_type_enum_table[] = {
    {"group_id",  diag_flexe_oh_group_id,   "Overhead group number"},
    {"phy_id",    diag_flexe_oh_phy_id,     "Overhead logical phy id"},
    {"C",         diag_flexe_oh_c,          "Overhead Cal In Use bit"},
    {"CR",        diag_flexe_oh_cr,         "Overhead Cal Request bit"},
    {"CA",        diag_flexe_oh_ca,         "Overhead Cal Acknowledge bit"},
    {NULL}
};

/*************
* FUNCTIONS *
 */

/** see header file */
shr_error_e
sh_cmd_is_flexe_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        /**
         * if init is not done, return that flexe is available.
         * this is done to prevent access to DNX-Data before it is init
         */
        if (!dnx_init_is_init_done_get(unit))
        {
            SHR_EXIT();
        }

        /** Not supported for devices without flexe support */
        if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
sh_cmd_is_flexe_core_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Can't run flexe core on simulated system(pcid, cmodel etc) */
    if (SAL_BOOT_PLISIM)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        SHR_EXIT();
    }

    if (SOC_IS_DNX(unit))
    {
        /**
         * if init is not done, return that flexe is available.
         * this is done to prevent access to DNX-Data before it is init
         */
        if (!dnx_init_is_init_done_get(unit))
        {
            SHR_EXIT();
        }

        /** Not supported for devices without flexe support */
        if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }

        if (!dnx_diag_is_flexe_core_enabled(unit))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }

        /** using standard flexe driver instead of flexe core */
        if (dnx_data_dev_init.general.flexe_core_drv_select_get(unit))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
exit:
    SHR_FUNC_EXIT;
}


/** see header file */
shr_error_e
sh_cmd_is_flexe_enable(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Can't run flexe on simulated system(pcid, cmodel etc) */
    if (SAL_BOOT_PLISIM)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        SHR_EXIT();
    }

    if (SOC_IS_DNX(unit))
    {
        /**
         * if init is not done, return that flexe is available.
         * this is done to prevent access to DNX-Data before it is init
         */
        if (!dnx_init_is_init_done_get(unit))
        {
            SHR_EXIT();
        }

        /** Not supported for devices without flexe support */
        if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }

        if (!dnx_diag_is_flexe_core_enabled(unit))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
exit:
    SHR_FUNC_EXIT;
}


/*
----------------------------------------------------------------------------------------
------------------------------flexe group command---------------------------------------
    flexe group create gid=x nof-pcs=xx phy0=y0:z0 phy1=y1:z
    flexe group add-phy gid=x phy-interface=y:z
    flexe group remove-phy gid=x phy-interface= y:z
    flexe group destroy gid=x
    flexe group calendar direction=RX/TX calendar=CalA/CalB
    flexe group dump
----------------------------------------------------------------------------------------
*/

/**
 * \brief
 *   Parse phy_info, The input format is <port:logical_phy_id>
 * \param [in] unit - The unit number.
 * \param [in] str_phy_info - The input string include flexe phy info.
 * \param [out] phy_info - flexe phy info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_diag_parse_flexe_phy_info(
    int unit,
    char *str_phy_info,
    bcm_port_flexe_group_phy_info_t *phy_info)
{
    shr_error_e rc = _SHR_E_PARAM;
    char* p_phy_port;
    char* p_logical_phy_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(str_phy_info,_SHR_E_PARAM, "str_phy_info");
    SHR_NULL_CHECK(phy_info,_SHR_E_PARAM, "phy_info");

    p_phy_port = str_phy_info;
    p_logical_phy_id = sal_strchr(str_phy_info, ':');
    if (p_logical_phy_id == NULL)
    {
        p_logical_phy_id = sal_strchr(str_phy_info, '-');
    }
    if (p_logical_phy_id != NULL)
    {
        *p_logical_phy_id = 0;
        p_logical_phy_id++;

        phy_info->port = parse_integer(p_phy_port);
        phy_info->logical_phy_id = parse_integer(p_logical_phy_id);
        rc = _SHR_E_NONE;
    }

    cli_out("dnx_diag_parse_flexe_phy_info: str_phy_info=%s, phy_info=%d:%d\n",
             str_phy_info, phy_info->port, phy_info->logical_phy_id);

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Parse multi phy_info, The input format is <port:logical_phy_id,...>
 * \param [in] unit - The unit number.
 * \param [in] str_phys_info - The input string include flexe phy info.
 * \param [out] phy_num  - number of flexe phy.
 * \param [out] phy_info - flexe phy info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

static shr_error_e
dnx_diag_parse_flexe_phys_info(
    int unit,
    char *str_phys_info,
    uint32 *phy_num,
    bcm_port_flexe_group_phy_info_t *phy_info)
{
    uint32 num;
    char *str_phy_1, *str_phy_2;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(phy_num,_SHR_E_PARAM, "phy_num");
    SHR_NULL_CHECK(phy_info,_SHR_E_PARAM, "phy_info");

    num = 0;
    str_phy_1 = str_phys_info;
    while (str_phy_1 != NULL)
    {
        while (*str_phy_1 == ' ')
        {
            str_phy_1++;
            if (*str_phy_1 == 0)
            {
                break;
            }
        }
        str_phy_2 = sal_strchr(str_phy_1, ',');
        if (str_phy_2 != NULL)
        {
            *str_phy_2 = 0;
            str_phy_2++;
        }

        SHR_IF_ERR_EXIT(dnx_diag_parse_flexe_phy_info(unit, str_phy_1, phy_info+num));
        num++;
        if (num >= DIAG_FLEXE_MAX_PHY_OF_GROUP)
        {
            break;
        }

        str_phy_1 = str_phy_2;
    }
    *phy_num = num;
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_diag_flexe_group_create_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 group_index, nof_phy, ii;
    char*   str_phy_info;

    bcm_gport_t gport;
    bcm_port_flexe_group_phy_info_t phy_info[DIAG_FLEXE_MAX_GROUP_INDEX];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("GRouP", group_index);
    SH_SAND_GET_STR("phy", str_phy_info);

    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);
    SHR_IF_ERR_EXIT(dnx_diag_parse_flexe_phys_info(unit, str_phy_info, &nof_phy, phy_info));

    cli_out("group_index=%d, nof_phy=%d\n", group_index, nof_phy);
    for (ii=0; ii<nof_phy; ii++)
    {
        cli_out("\t%-4d%-8d%-8d\n", ii, phy_info[ii].port, phy_info[ii].logical_phy_id);
    }

    BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_create(unit, gport, 0, nof_phy, phy_info));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_group_destroy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 group_index;
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("GRouP", group_index);
    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);

    BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_destroy(unit, gport));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

/*
    bcm_port_flexe_group_cal_active_set
    bcm_port_flexe_group_cal_active_get
*/
static shr_error_e
dnx_diag_flexe_group_calendar_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 group_index, flexe_calendar;
    bcm_gport_t gport;
    uint32 flags;
    bcm_port_flexe_group_cal_t active_cal;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("GRouP", group_index);
    SH_SAND_GET_ENUM("CaLenDAR", flexe_calendar);

    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);
    BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
    /* only tx is valid for calendar active set */
    flags = BCM_PORT_FLEXE_TX;
    flags = BCM_PORT_FLEXE_TX;

    active_cal = bcmPortFlexeGroupCalA;
    if (flexe_calendar == diag_flexe_calendar_a)
    {
        active_cal = bcmPortFlexeGroupCalA;
    }
    else
    {
        active_cal = bcmPortFlexeGroupCalB;
    }
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_active_set(unit, gport, flags, active_cal));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_format_time_slots_string(
    int unit,
    int row,
    int nof_slot,
    int *time_slot,
    char* time_slot_string,
    int string_size,
    int is_oh)
{
    int ii, disp_num, disp_start;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(time_slot_string, _SHR_E_PARAM, "time_slot_string");
    if ((row * DIAG_DISP_SLOT_NUM_IN_ROW) >= nof_slot)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    disp_start = row * DIAG_DISP_SLOT_NUM_IN_ROW;
    disp_num = nof_slot - disp_start;
    if (disp_num > DIAG_DISP_SLOT_NUM_IN_ROW)
    {
        disp_num = DIAG_DISP_SLOT_NUM_IN_ROW;
    }

    sal_memset(time_slot_string, 0, string_size);
    sal_snprintf(time_slot_string, string_size, "    ");
    for (ii=0; ii<disp_num; ii++)
    {
        if (is_oh)
        {
            sal_snprintf(time_slot_string + sal_strlen(time_slot_string),
                        string_size - sal_strlen(time_slot_string), "%-4x ",
                        time_slot[disp_start+ii]);
        }
        else
        {
            sal_snprintf(time_slot_string + sal_strlen(time_slot_string),
                        string_size - sal_strlen(time_slot_string), "%-4d ",
                        time_slot[disp_start+ii]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_dump_group_info_time_slot(
    int unit,
    int row_i,
    int nof_rx_slots,
    int nof_tx_slots,
    int *rx_slots,
    int *tx_slots,
    int *oh_rx_slots,
    int *oh_tx_slots,
    prt_control_t *prt_ctr)
{
    char time_slot_string[DIAG_DISP_SLOT_TIME_STRING_LEN];
    int  rows_rx_slot, rows_tx_slot;

    SHR_FUNC_INIT_VARS(unit);

    rows_rx_slot = (nof_rx_slots + DIAG_DISP_SLOT_NUM_IN_ROW - 1) / DIAG_DISP_SLOT_NUM_IN_ROW + 1;
    rows_tx_slot = (nof_tx_slots + DIAG_DISP_SLOT_NUM_IN_ROW - 1) / DIAG_DISP_SLOT_NUM_IN_ROW + 1;
    {
    if (row_i == 0)
    {
        PRT_CELL_SET("RX(nof_slots=%d)", nof_rx_slots);
        PRT_CELL_SET("RX-OH(nof_slots=%d)", nof_rx_slots);
    }
    else if (row_i < rows_rx_slot)
    {
        SHR_IF_ERR_EXIT(dnx_diag_format_time_slots_string(unit, row_i-1, nof_rx_slots, rx_slots,
                                                          time_slot_string, sizeof(time_slot_string), 0));
        PRT_CELL_SET("%s", time_slot_string);

        SHR_IF_ERR_EXIT(dnx_diag_format_time_slots_string(unit, row_i-1, nof_rx_slots, oh_rx_slots,
                                                          time_slot_string, sizeof(time_slot_string), 1));
        PRT_CELL_SET("%s", time_slot_string);
    }
    else if (row_i == rows_rx_slot)
    {
        PRT_CELL_SET("TX(nof_slots=%d)", nof_tx_slots);
        PRT_CELL_SET("TX-OH(nof_slots=%d)", nof_tx_slots);
    }
    else if (row_i < (rows_rx_slot + rows_tx_slot))
    {
        SHR_IF_ERR_EXIT(dnx_diag_format_time_slots_string(unit, row_i-rows_rx_slot-1, nof_tx_slots, tx_slots,
                                                          time_slot_string, sizeof(time_slot_string), 0));
        PRT_CELL_SET("%s", time_slot_string);

        SHR_IF_ERR_EXIT(dnx_diag_format_time_slots_string(unit, row_i-rows_rx_slot-1, nof_tx_slots, oh_tx_slots,
                                                          time_slot_string, sizeof(time_slot_string), 1));
        PRT_CELL_SET("%s", time_slot_string);
    }
    }
exit:
    SHR_FUNC_EXIT;
}


/* ref sh_dnx_diag_counter_graphical_print
    -> sh_dnx_counter_graphic_section_print*/
static shr_error_e
dnx_diag_flexe_dump_group_info(
    int unit,
    uint32 group_index,
    prt_control_t * prt_ctr)
{
    bcm_gport_t gport;
    uint32 flags = 0;
    shr_error_e rc;

    bcm_port_flexe_group_phy_info_t *phy_info = NULL;
    int *rx_slots=NULL, *tx_slots=NULL;
    int *oh_rx_slots=NULL, *oh_tx_slots=NULL;
    int actual_nof_pcs, actual_nof_rx_slots, actual_nof_tx_slots;
    bcm_port_flexe_group_cal_t active_cal_rx, active_cal_tx, cal;
    int rows_phy, rows_timeslot, rows_rx_slot, rows_tx_slot;
    int row, rows_max, row_i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);

    SHR_ALLOC(phy_info, sizeof(bcm_port_flexe_group_phy_info_t) * DIAG_FLEXE_MAX_PHY_OF_GROUP,
              "SHR_ALLOC", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(rx_slots, sizeof(int) * DIAG_FLEXE_MAX_SLOT_OF_GROUP,
              "rx_slots", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(tx_slots, sizeof(int) * DIAG_FLEXE_MAX_SLOT_OF_GROUP,
              "tx_slots", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(oh_rx_slots, sizeof(int) * DIAG_FLEXE_MAX_SLOT_OF_GROUP,
              "oh_rx_slots", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(oh_tx_slots, sizeof(int) * DIAG_FLEXE_MAX_SLOT_OF_GROUP,
              "oh_tx_slots", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    /* if group exist */
    BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
    rc = bcm_port_flexe_group_get(unit, gport, flags, DIAG_FLEXE_MAX_PHY_OF_GROUP, phy_info, &actual_nof_pcs);
    if (rc != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    /* get active calender of RX&TX */
    flags = BCM_PORT_FLEXE_RX;
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_active_get(unit, gport, flags, &active_cal_rx));
    flags = BCM_PORT_FLEXE_TX;
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_active_get(unit, gport, flags, &active_cal_tx));

    /* get time slot configure of Cal A */
    cal = bcmPortFlexeGroupCalA;
    flags = BCM_PORT_FLEXE_RX;
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                       rx_slots, &actual_nof_rx_slots));
    flags = BCM_PORT_FLEXE_TX;
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                       tx_slots, &actual_nof_tx_slots));
    /* OH time slot of Cal A */
    flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_OVERHEAD;
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                       oh_rx_slots, &actual_nof_rx_slots));
    flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_OVERHEAD;
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                       oh_tx_slots, &actual_nof_tx_slots));

    rows_phy = actual_nof_pcs;
    rows_rx_slot = (actual_nof_rx_slots + DIAG_DISP_SLOT_NUM_IN_ROW - 1) / DIAG_DISP_SLOT_NUM_IN_ROW + 1;
    rows_tx_slot = (actual_nof_tx_slots + DIAG_DISP_SLOT_NUM_IN_ROW - 1) / DIAG_DISP_SLOT_NUM_IN_ROW + 1;
    /* list 2 Cal info */
    rows_timeslot = (rows_rx_slot + rows_tx_slot) * 2 + 1;
    rows_max = (rows_timeslot > rows_phy) ? rows_timeslot : rows_phy;
    for (row=0; row<rows_max; row++)
    {
        if (row == rows_max - 1)
        {
            /*
             * the last row drawed with underscore
             */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }
        else
        {
            /*
             * the other rows drawed with non_line
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }

        /* Group index */
        if (row == 0)
        {
            PRT_CELL_SET("%d", group_index);
        }
        else
        {
            PRT_CELL_SET("");
        }

        /* phy info */
        if (row < rows_phy)
        {
            PRT_CELL_SET("%d: %d/%d", row, phy_info[row].port, phy_info[row].logical_phy_id)
        }
        else
        {
            PRT_CELL_SET("");
        }

        /* Calendar */
        if (row == 0)
        {
            PRT_CELL_SET("CalA");
        }
        else if ((row == 1) || (row == (rows_rx_slot + rows_tx_slot) + 2))
        {
            if ((cal == active_cal_rx) && (cal == active_cal_tx))
            {
                PRT_CELL_SET("Active: RX/TX");
            }
            else if (cal == active_cal_rx)
            {
                PRT_CELL_SET("Active: Rx");
            }
            else if (cal == active_cal_tx)
            {
                PRT_CELL_SET("Active Tx");
            }
            else
            {
                PRT_CELL_SET("InActive");
            }
        }
        else if (row == (rows_rx_slot + rows_tx_slot))
        {
            PRT_CELL_SET("----------------");
        }
        else if (row == (rows_rx_slot + rows_tx_slot) + 1)
        {
            PRT_CELL_SET("CalB");
        }
        else
        {
            PRT_CELL_SET("");
        }

        /* timeslots & OH Client Cal*/
        if (row < (rows_rx_slot + rows_tx_slot))
        {
            row_i = row;
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_group_info_time_slot(unit, row_i, actual_nof_rx_slots, actual_nof_tx_slots,
                                                                     rx_slots, tx_slots, oh_rx_slots, oh_tx_slots, prt_ctr));
        }
        else if (row == (rows_rx_slot + rows_tx_slot))
        {
            PRT_CELL_SET("----------------------------------------------------");
            PRT_CELL_SET("----------------------------------------------------");

            /* at this time, all Cal A was displayed, get Cal B configure */
            /* get time slot configure of Cal A */
            cal = bcmPortFlexeGroupCalB;
            flags = BCM_PORT_FLEXE_RX;
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                               rx_slots, &actual_nof_rx_slots));
            flags = BCM_PORT_FLEXE_TX;
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                               tx_slots, &actual_nof_tx_slots));
            /* OH time slot of Cal A */
            flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_OVERHEAD;
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                               oh_rx_slots, &actual_nof_rx_slots));
            flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_OVERHEAD;
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, gport, flags, cal, DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                               oh_tx_slots, &actual_nof_tx_slots));
        }
        else if (row < rows_timeslot)
        {
            row_i = row - (rows_rx_slot + rows_tx_slot) - 1;
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_group_info_time_slot(unit, row_i, actual_nof_rx_slots, actual_nof_tx_slots,
                                                                     rx_slots, tx_slots, oh_rx_slots, oh_tx_slots, prt_ctr));
        }
        else
        {
            PRT_CELL_SET("");
            PRT_CELL_SET("");
        }
    }

exit:
    SHR_FREE(phy_info);
    SHR_FREE(rx_slots);
    SHR_FREE(tx_slots);
    SHR_FREE(oh_rx_slots);
    SHR_FREE(oh_tx_slots);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_group_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 group;
    bcm_pbmp_t group_ports;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE group info");
    PRT_COLUMN_ADD("INDex");
    PRT_COLUMN_ADD("PHY");
    PRT_COLUMN_ADD("CALendar");
    PRT_COLUMN_ADD("TimeSlot");
    PRT_COLUMN_ADD("OH Group Slot");

    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_GROUP, &group_ports));
    BCM_PBMP_ITER(group_ports, group)
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_group_info(unit, group, prt_ctr));
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
    bcm_port_flexe_group_phy_add
*/
static shr_error_e
dnx_diag_flexe_group_add_phy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 group_index;
    char*  str_phy_info;
    bcm_gport_t gport;
    bcm_port_flexe_group_phy_info_t phy_info;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("GRouP", group_index);
    SH_SAND_GET_STR("phy", str_phy_info);

    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);

    sal_memset(&phy_info, 0, sizeof(phy_info));
    SHR_IF_ERR_EXIT(dnx_diag_parse_flexe_phy_info(unit, str_phy_info, &phy_info));

    BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_phy_add(unit, gport, 0, &phy_info));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

/*
    bcm_port_flexe_group_phy_remove
*/
static shr_error_e
dnx_diag_flexe_group_remove_phy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 group_index;
    char*  str_phy_info;
    bcm_gport_t gport;
    bcm_port_flexe_group_phy_info_t phy_info;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("GRouP", group_index);
    SH_SAND_GET_STR("phy", str_phy_info);

    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);

    sal_memset(&phy_info, 0, sizeof(phy_info));
    SHR_IF_ERR_EXIT(dnx_diag_parse_flexe_phy_info(unit, str_phy_info, &phy_info));

    BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
    SHR_IF_ERR_EXIT(bcm_port_flexe_group_phy_remove(unit, gport, 0, &phy_info));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}


static sh_sand_option_t dnx_diag_flexe_group_create_options[] = {
    {"GRouP",   SAL_FIELD_TYPE_UINT32, "FlexE group INDex(0-7)",  NULL, NULL, "0-7"},
    {"phy",     SAL_FIELD_TYPE_STR,    "FlexE group phy infos",   ""},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_group_destroy_options[] = {
    {"GRouP",   SAL_FIELD_TYPE_UINT32, "FlexE group INDex",       NULL, NULL, "0-7"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_group_add_phy_options[] = {
    {"GRouP",   SAL_FIELD_TYPE_UINT32, "FlexE group INDex",                  NULL, NULL, "0-7"},
    {"phy",     SAL_FIELD_TYPE_STR,    "FlexE group phy infos(port:phyID)",  ""},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_group_remove_phy_options[] = {
    {"GRouP",   SAL_FIELD_TYPE_UINT32, "FlexE group INDex",                  NULL, NULL, "0-7"},
    {"phy",     SAL_FIELD_TYPE_STR,    "FlexE group phy infos(port:phyID)",  ""},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_group_calendar_options[] = {
    {"GRouP",    SAL_FIELD_TYPE_UINT32, "FlexE group INDex",                 NULL, NULL, "0-7"},
    {"CaLenDAR", SAL_FIELD_TYPE_ENUM,   "FlexE group calendar(CalA/CalB)",   "CalA", (void*)dnx_flexe_calendar_enum_table},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_group_dump_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_group_create_man = {
    .brief = "Create FlexE group command\n",
    .examples = "group=1 phy=20:5\n"
                "group=2 phy=21:10"
};

static sh_sand_man_t dnx_diag_flexe_group_destroy_man = {
    .brief = "Destroy FlexE group command\n",
    .examples = "group=2"
};

static sh_sand_man_t dnx_diag_flexe_group_add_phy_man = {
    .brief = "Add phy interface to FlexE group command\n",
    .examples = "group=2 phy=22:11"
};

static sh_sand_man_t dnx_diag_flexe_group_remove_phy_man = {
    .brief = "Remove phy interface from FlexE group command\n",
    .examples = "group=2 phy=21:11"
};

static sh_sand_man_t dnx_diag_flexe_group_calendar_man = {
    .brief = "Set FlexE group TX active calendar command\n",
    .examples = "group=2 calendar=CalA"
};

static sh_sand_man_t dnx_diag_flexe_group_dump_man = {
    .brief = "Dump FlexE group command\n",
    .examples = ""
};


sh_sand_cmd_t dnx_diag_flexe_group_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"CReate",    dnx_diag_flexe_group_create_cmd,      NULL, dnx_diag_flexe_group_create_options,     &dnx_diag_flexe_group_create_man,     NULL, NULL, SH_CMD_NOT_RELEASE},
    {"Add",       dnx_diag_flexe_group_add_phy_cmd,     NULL, dnx_diag_flexe_group_add_phy_options,    &dnx_diag_flexe_group_add_phy_man,    NULL, NULL, SH_CMD_NOT_RELEASE},
    {"ReMoVe",    dnx_diag_flexe_group_remove_phy_cmd,  NULL, dnx_diag_flexe_group_remove_phy_options, &dnx_diag_flexe_group_remove_phy_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {"CaLenDAR",  dnx_diag_flexe_group_calendar_cmd,    NULL, dnx_diag_flexe_group_calendar_options,   &dnx_diag_flexe_group_calendar_man,   NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DuMP",      dnx_diag_flexe_group_dump_cmd,        NULL, dnx_diag_flexe_group_dump_options,       &dnx_diag_flexe_group_dump_man,       NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DeSTRoY",   dnx_diag_flexe_group_destroy_cmd,     NULL, dnx_diag_flexe_group_destroy_options,    &dnx_diag_flexe_group_destroy_man,    NULL, NULL, SH_CMD_NOT_RELEASE},
    {NULL}
};

static sh_sand_man_t  dnx_diag_flexe_group_man = {
    .brief = "FlexE group commands\n",
};

/*
----------------------------------------------------------------------------------------
------------------------------flexe client command--------------------------------------
    flexe client create port=x virtual=mac/sar CRoss_CoNnect
    flexe client delete port=x
    flexe client time-slot port=x slot="xxxxxxx"
    flexe client dump
----------------------------------------------------------------------------------------
*/
static shr_error_e
dnx_diag_flexe_client_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port;
    uint32 flags;
    diag_virtual_client_type_e client_type;
    int is_cross_connect;
    bcm_port_interface_info_t if_info;
    bcm_port_mapping_info_t mapping_info;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("Port", port);
    SH_SAND_GET_ENUM("VIRTual", client_type);
    SH_SAND_GET_BOOL("CRoss_CoNnect", is_cross_connect);

    SHR_RANGE_VERIFY(port, 0, SOC_MAX_NUM_PORTS, _SHR_E_PARAM, "Invalid input of Port\n");

    /** init vars */
    sal_memset(&if_info, 0, sizeof(bcm_port_interface_info_t));
    bcm_port_mapping_info_t_init(&mapping_info);

    if (client_type == diag_virtual_client_mac)
    {
        flags = BCM_PORT_ADD_FLEXE_MAC;
        if (is_cross_connect)
        {
            flags |= BCM_PORT_ADD_CROSS_CONNECT;
        }

        if_info.interface = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        if_info.interface_id = -1;
        mapping_info.tm_port = port;
        if (!is_cross_connect)
        {
            mapping_info.num_priorities = 2;
            mapping_info.base_q_pair = -1;
            mapping_info.num_sch_priorities = 0;
        }
    }
    else if (client_type == diag_virtual_client_sar)
    {
        flags = BCM_PORT_ADD_FLEXE_SAR;
        if (is_cross_connect)
        {
            flags |= BCM_PORT_ADD_CROSS_CONNECT;
        }
        if_info.interface = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
    }
    else
    {
        flags = 0;
        if_info.interface = BCM_PORT_IF_FLEXE_CLIENT;
    }

    SHR_IF_ERR_EXIT(bcm_port_add(unit, port, flags, &if_info, &mapping_info));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_diag_flexe_client_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("Port", port);
    SHR_RANGE_VERIFY(port, 0, SOC_MAX_NUM_PORTS, _SHR_E_PARAM, "Invalid input of Port\n");

    SHR_IF_ERR_EXIT(bcm_port_remove(unit, port, 0));
exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}


/*
int bcm_port_flexe_group_cal_slots_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots);

int bcm_port_flexe_group_cal_slots_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int max_nof_slots,
    int *calendar_slots,
    int *actual_nof_slots);

*/


static shr_error_e
dnx_diag_flexe_client_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port, slot_present;
    uint32 oh_client_id;
    bcm_gport_t grp_gport;
    uint32 group_index;
    diag_flexe_direction_e flexe_direction;
    diag_flexe_calendar_e flexe_calendar;
    uint32 *array_slot;
    uint32 flags = 0;
    bcm_port_flexe_group_cal_t cal;
    int *time_slots=NULL, nof_slots, ii;
    int *oh_slots = NULL;
    dnx_algo_port_info_s port_info;
    bcm_port_resource_t resource, resource_get;
    /* bcm_port_prio_config_t priority_config; */
    int speed, alloc_slot_num;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("SLOT", slot_present);
    SH_SAND_GET_INT32("Port", port);
    SH_SAND_GET_UINT32("GRouP", group_index);
    SH_SAND_GET_ENUM("DIRection", flexe_direction);
    SH_SAND_GET_ENUM("CaLenDAR", flexe_calendar);
    SH_SAND_GET_ARRAY32("SLOT", array_slot);
    SH_SAND_GET_UINT32("OverHead", oh_client_id);
    SH_SAND_GET_INT32("SPeeD", speed);

    SHR_ALLOC(time_slots, sizeof(int) * DIAG_FLEXE_MAX_SLOT_OF_GROUP,
              "time_slots", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(oh_slots, sizeof(int) * DIAG_FLEXE_MAX_SLOT_OF_GROUP,
              "time_slots", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_RANGE_VERIFY(port, 0, SOC_MAX_NUM_PORTS, _SHR_E_PARAM, "Invalid input of Port\n");
    SHR_FLEXE_GROUP_RANGE_VERIFY(group_index, "group_index %d is invalid.", group_index);

    /*
        need check if port is valid client.
        For time_slot & OverHead configure,  only bus A is meaningful
        For Speed configure, only bus B is meaningful
    */
    SHR_IF_ERR_EXIT(diag_flexe_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1))
    {
        if (speed <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid input SPeeD parameter!!\n");
        }

        /* get the speed first */
        sal_memset(&resource_get, 0, sizeof(resource_get));
        resource_get.port = port;
        resource_get.flags = 0;
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource_get));
        if ((resource_get.flags & BCM_PORT_RESOURCE_ASYMMETRICAL) == 0)
        {
            resource_get.rx_speed = resource_get.speed;
        }

        /* set the speed */
        sal_memset(&resource, 0, sizeof(resource));
        resource.port = port;
        if (flexe_direction == diag_flexe_group_rx)
        {
            resource.flags = BCM_PORT_RESOURCE_ASYMMETRICAL;
            resource.rx_speed = speed;
            resource.speed = resource_get.speed;
        }
        else if (flexe_direction == diag_flexe_group_tx)
        {
            resource.flags = BCM_PORT_RESOURCE_ASYMMETRICAL;
            resource.speed = speed;
            resource.rx_speed = resource_get.rx_speed;
        }
        else
        {
            resource.speed = speed;
        }
        SHR_IF_ERR_EXIT(bcm_port_resource_set(unit, port, &resource));

        SHR_EXIT();

        /* priority configure was already set while client create. */
        /*
        sal_memset(&priority_config, 0, sizeof(priority_config));
        priority_config.nof_priority_groups = 1;
        if (is_tdm)
        {
            priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
        }
        else
        {
            priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
            priority_config.priority_groups[0].source_priority  = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1;
            priority_config.priority_groups[0].source_priority |= BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
        }
        priority_config.priority_groups[1].num_of_entries = speed / 5000;
        SHR_IF_ERR_EXIT(bcm_port_priority_config_set(unit, port, &priority_config));
        */
    }
    else if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid input client port!!\n");
    }

    /* time slot configure & speed for client bus A */

    /* Set speed of client bus A */
    if (speed > 0)
    {
        /* get the speed first */
        sal_memset(&resource_get, 0, sizeof(resource_get));
        resource_get.port = port;
        resource_get.flags = 0;
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource_get));
        if ((resource_get.flags & BCM_PORT_RESOURCE_ASYMMETRICAL) == 0)
        {
            resource_get.rx_speed = resource_get.speed;
        }

        /* set the speed */
        sal_memset(&resource, 0, sizeof(resource));
        resource.port = port;
        if (flexe_direction == diag_flexe_group_rx)
        {
            resource.flags = BCM_PORT_RESOURCE_ASYMMETRICAL;
            resource.rx_speed = speed;
            resource.speed = resource_get.speed;
        }
        else if (flexe_direction == diag_flexe_group_tx)
        {
            resource.flags = BCM_PORT_RESOURCE_ASYMMETRICAL;
            resource.speed = speed;
            resource.rx_speed = resource_get.rx_speed;
        }
        else
        {
            resource.speed = speed;
        }
        SHR_IF_ERR_EXIT(bcm_port_resource_set(unit, port, &resource));
    }

    BCM_FLEXE_GROUP_GPORT_SET(grp_gport, group_index);
    if (flexe_calendar == diag_flexe_calendar_a)
    {
        cal = bcmPortFlexeGroupCalA;
    }
    else
    {
        cal = bcmPortFlexeGroupCalB;
    }
    /* rx time slot configure */
    if ((flexe_direction == diag_flexe_group_rx) || (flexe_direction == diag_flexe_group_all))
    {
        flags = BCM_PORT_FLEXE_RX;

        if (slot_present)
        {
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, grp_gport, flags, cal,
                                                               DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                               time_slots, &nof_slots));
            alloc_slot_num = 0;
            for (ii=0; ii<nof_slots; ii++)
            {
                if (utilex_bitstream_test_bit(array_slot, ii))
                {
                    time_slots[ii] = port;
                    alloc_slot_num++;
                }
                else
                {
                    if (time_slots[ii] == port)
                    {
                        time_slots[ii] = 0;
                    }
                }
            }

            cli_out("Set group %x calendar RX slots, flags=%d, cal=%d, nof_slots=%d, alloc_slot_num=%d",
                        grp_gport, grp_gport, cal, nof_slots, alloc_slot_num);
            for (ii=0; ii<nof_slots; ii++)
            {
                if ((ii % DIAG_DISP_SLOT_NUM_IN_ROW) == 0)
                {
                    cli_out("\n    ");
                }
                cli_out("%-6d", time_slots[ii]);
            }
            cli_out("\n");
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_set(unit, grp_gport, flags, cal,
                                                               nof_slots, time_slots));
        }
    }

    /* tx time slot configure */
    if ((flexe_direction == diag_flexe_group_tx) || (flexe_direction == diag_flexe_group_all))
    {
        flags = BCM_PORT_FLEXE_TX;
        if (slot_present)
        {
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, grp_gport, flags, cal,
                                                           DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                           time_slots, &nof_slots));
            alloc_slot_num = 0;
            for (ii=0; ii<nof_slots; ii++)
            {
                if (utilex_bitstream_test_bit(array_slot, ii))
                {
                    time_slots[ii] = port;
                    alloc_slot_num++;
                }
                else
                {
                    if (time_slots[ii] == port)
                    {
                        time_slots[ii] = 0;
                    }
                }
            }

            cli_out("Set group %x calendar RX slots, flags=%d, cal=%d, nof_slots=%d, alloc_slot_num=%d",
                    grp_gport, grp_gport, cal, nof_slots, alloc_slot_num);
            for (ii=0; ii<nof_slots; ii++)
            {
                if ((ii % DIAG_DISP_SLOT_NUM_IN_ROW) == 0)
                {
                    cli_out("\n    ");
                }
                cli_out("%-6d", time_slots[ii]);
            }
            cli_out("\n");
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_set(unit, grp_gport, flags, cal,
                                                               nof_slots, time_slots));
        }

        if (oh_client_id != 0)
        {
            flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_OVERHEAD;
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_get(unit, grp_gport, flags, cal,
                                                               DIAG_FLEXE_MAX_SLOT_OF_GROUP,
                                                               oh_slots, &nof_slots));

            for (ii=0; ii<nof_slots; ii++)
            {
                if (time_slots[ii] == port)
                {
                    oh_slots[ii] = oh_client_id;
                }
            }
            SHR_IF_ERR_EXIT(bcm_port_flexe_group_cal_slots_set(unit, grp_gport, flags, cal,
                                                               nof_slots, oh_slots));
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FREE(time_slots);
    SHR_FREE(oh_slots);
    SHR_FUNC_EXIT;
}

/*int bcm_port_get(
    int unit,
    bcm_port_t port,
    uint32* flags,
    bcm_port_interface_info_t *interface_info,
    bcm_port_mapping_info_t *mapping_info);*/

static shr_error_e
dnx_diag_flexe_client_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port, enable, status;
    uint32 flags, fault_flags;
    bcm_pbmp_t client_ports;
    bcm_port_interface_info_t if_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t resource;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    flags = DIAG_FLEXE_PORT_TYPE_CLIENT | DIAG_FLEXE_PORT_TYPE_CLIENT_MAC | DIAG_FLEXE_PORT_TYPE_CLIENT_SAR;
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, flags, &client_ports));

    PRT_TITLE_SET("FlexE CLIENT info");
    PRT_COLUMN_ADD("Client");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Channel");
    PRT_COLUMN_ADD("Speed");
    PRT_COLUMN_ADD("Enable");
    PRT_COLUMN_ADD("Link");
    /* PRT_COLUMN_ADD("time_slot"); */      /* only normal client is valid */

    BCM_PBMP_ITER(client_ports, port)
    {
        /** init vars */
        sal_memset(&if_info, 0, sizeof(bcm_port_interface_info_t));
        bcm_port_mapping_info_t_init(&mapping_info);

        SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &if_info, &mapping_info));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /* client port */
        PRT_CELL_SET("%d", port);

        /* type: normal/sar/mac/mac-CrossConnect */
        if (if_info.interface == BCM_PORT_IF_FLEXE_CLIENT)
        {
            PRT_CELL_SET("Normal");
        }
        else if (if_info.interface == BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT)
        {
            if (flags & BCM_PORT_ADD_FLEXE_SAR)
            {
                PRT_CELL_SET("SAR");
            }
            else if (flags & BCM_PORT_ADD_FLEXE_MAC)
            {
                /*
                 * get FlexE RMC info
                 */
                dnx_algo_port_rmc_info_t rmc;
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, 0, &rmc));
                if (flags & BCM_PORT_ADD_CROSS_CONNECT)
                {
                    PRT_CELL_SET("MAC-CrossConnect(rmc_id=%d)", rmc.rmc_id);
                }
                else
                {
                    PRT_CELL_SET("MAC(rmc_id=%d)", rmc.rmc_id);
                }
            }
            else
            {
                PRT_CELL_SET("ERRor_1");
            }
        }
        else
        {
            PRT_CELL_SET("ERRor_2");
        }

        /* channel */
        PRT_CELL_SET("%d", mapping_info.channel);

        sal_memset(&resource, 0, sizeof(resource));
        resource.port = port;
        resource.flags = 0;
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource));
        if (resource.flags & BCM_PORT_RESOURCE_ASYMMETRICAL)
        {
            PRT_CELL_SET("ASYMMETRICAL: RX=%d, TX=%d", resource.rx_speed, resource.speed);
        }
        else
        {
            PRT_CELL_SET("Speed=%d", resource.speed);
        }

        /* timeslot(include RX/TX at 2 callendar) */
        SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, port, &enable));
        PRT_CELL_SET("%d", enable);

        if ((if_info.interface == BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT) &&
            (flags & BCM_PORT_ADD_FLEXE_MAC))
        {
            /* busB client, link_status & LF & RF get */
            SHR_IF_ERR_EXIT(bcm_port_link_status_get(unit, port, &status));
            SHR_IF_ERR_EXIT(bcm_port_fault_get(unit, port, &fault_flags));

            PRT_CELL_SET("Link=%d, fault=%x", status, fault_flags);
        }
        else
        {
            PRT_CELL_SET("");
        }
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}


static sh_sand_option_t dnx_diag_flexe_client_add_options[] = {
    {"Port",            SAL_FIELD_TYPE_INT32,   "Logical port number for client",   "-1"},
    {"VIRTual",         SAL_FIELD_TYPE_ENUM,    "Virtual client type(MAC/SAR)",     "none",  (void*)dnx_virtual_client_enum_table},
    {"CRoss_CoNnect",   SAL_FIELD_TYPE_BOOL,    "If the virtual client is corss connect", "No"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_client_delete_options[] = {
    {"Port",            SAL_FIELD_TYPE_INT32, "Logical port number for client",     "-1"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_client_timeslot_options[] = {
    {"Port",      SAL_FIELD_TYPE_INT32,   "Logical port number for client",    "-1"},
    {"DIRection", SAL_FIELD_TYPE_ENUM,    "FlexE client direction(RX/TX/ALL)", "ALL",  (void*)dnx_flexe_direction_enum_table},
    {"CaLenDAR",  SAL_FIELD_TYPE_ENUM,    "FlexE group calendar(CalA/CalB)",   "CalA", (void*)dnx_flexe_calendar_enum_table},
    {"GRouP",     SAL_FIELD_TYPE_UINT32,  "FlexE group INDex(0-7)",            "0",    NULL, "0-7"},
    {"SLOT",      SAL_FIELD_TYPE_ARRAY32, "slot bitmap for this client",       "0"},
    {"OverHead",  SAL_FIELD_TYPE_UINT32,  "FlexE client OH ClientID",          "0"},
    {"SPeeD",     SAL_FIELD_TYPE_INT32,   "Speed of virtual FlexE client",      "-1"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_client_dump_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_client_add_man = {
    .brief = "Add FlexE client command\n",
    .examples = "port=150\n" "port=151 virtual=mac\n" "port=152 virtual=mac CRoss_CoNnect=yes\n"
                "port=250"
};

static sh_sand_man_t dnx_diag_flexe_client_delete_man = {
    .brief = "Delete FlexE client command\n",
    .examples = "port=250"
};

static sh_sand_man_t dnx_diag_flexe_client_timeslot_man = {
    .brief = "Set FlexE client time slot command\n",
    .examples = "port=150 GRouP=1 DIRection=rx CaLenDAR=CalA slot=0x3C\n"
                "port=150 GRouP=1 DIRection=tx slot=0x2D OverHead=9\n"
                "port=151 SPeeD=5000\n" "port=152 SPeeD=10000"

};

static sh_sand_man_t dnx_diag_flexe_client_dump_man = {
    .brief = "Dump FlexE client info command\n",
    .examples = ""
};



sh_sand_cmd_t dnx_diag_flexe_client_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"Add",    dnx_diag_flexe_client_add_cmd,    NULL, dnx_diag_flexe_client_add_options,      &dnx_diag_flexe_client_add_man,       NULL, NULL, SH_CMD_NOT_RELEASE},
    {"Set",    dnx_diag_flexe_client_set_cmd,    NULL, dnx_diag_flexe_client_timeslot_options, &dnx_diag_flexe_client_timeslot_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DuMP",   dnx_diag_flexe_client_dump_cmd,   NULL, dnx_diag_flexe_client_dump_options,     &dnx_diag_flexe_client_dump_man,      NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DeLeTe", dnx_diag_flexe_client_delete_cmd, NULL, dnx_diag_flexe_client_delete_options,   &dnx_diag_flexe_client_delete_man,    NULL, NULL, SH_CMD_NOT_RELEASE},
    {NULL}
};

static sh_sand_man_t  dnx_diag_flexe_client_man = {
    .brief = "FlexE client commands \n",
};

/*
----------------------------------------------------------------------------------------
------------------------------flexe flow command--------------------------------------
    flexe flow set src_port=xx dst_port=yy
    flexe flow clear src_port=xx dst_port=yy
    flexe flow dump

int bcm_port_flexe_flow_set(
    int unit,
    uint32 flags,
    bcm_port_t src_port,
    bcm_port_t dest_port);

int bcm_port_flexe_flow_clear(
    int unit,
    bcm_port_t src_port,
    bcm_port_t dest_port);

    Note: When src_port or dest_port is NIF ETH, NIF ILKN or Virtual Client,
    it should be created with BCM_PORT_ADD_CROSS_CONNECT ????
----------------------------------------------------------------------------------------
*/
/*
    int bcm_port_flexe_flow_set(
        int unit,
        uint32 flags,
        bcm_port_t src_port,
        bcm_port_t dest_port);

    int bcm_port_flexe_flow_clear(
        int unit,
        bcm_port_t src_port,
        bcm_port_t dest_port);

    flow set:  dnx_port_flexe_flow_set
        1. client_2_client:  configure b66 switch:
            dnx_port_flexe_flow_bus2bus_set
                -> b66switch_cfg
        2. mac_2_eth:
            dnx_port_flexe_flow_tmac2eth_set
                DBAL: DBAL_TABLE_NIF_FEU_L1_DEST_CTRL(DBAL_FIELD_TARGET_TMC)
        3. eth_2_mac:
            dnx_port_flexe_flow_eth2tmac_set
                DBAL: DBAL_TABLE_NIF_FEU_L1_SRC_CTRL(DBAL_FIELD_SOURCE_CLIENT)
            FEU_RX_RMC_L_1_CFG_2
            FEU_RX_RMC_L_1_CFG_3
            FEU_TX_TMC_L_1_CFG
        4. SAR_2_ILKN
            dnx_port_flexe_flow_sar2ilkn_set
                -> sar_tx_chan_map_cfg
        5. ILKN_2_SAR
            dnx_port_flexe_flow_ilkn2sar_set
                -> sar_rx_chan_map_cfg
        6. MAC_2_ILKN
            dnx_port_flexe_flow_tmac2ilkn_set
                DBAL_TABLE_NIF_FEU_RX_RMC_CTRL

*/

static shr_error_e
dnx_diag_flexe_flow_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t src_port, dst_port;
    int channel;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("SouRCe", src_port);
    SH_SAND_GET_INT32("DeSTination", dst_port);
    SH_SAND_GET_INT32("CHaNnel", channel);

    SHR_IF_ERR_EXIT(bcm_port_flexe_flow_set(unit, 0, src_port, channel, dst_port));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_flow_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t src_port, dst_port;
    int channel;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("SouRCe", src_port);
    SH_SAND_GET_INT32("DeSTination", dst_port);
    SH_SAND_GET_INT32("CHaNnel", channel);

    SHR_IF_ERR_EXIT(bcm_port_flexe_flow_clear(unit, src_port, channel, dst_port));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
flexe_port_name(
    int unit,
    bcm_port_t port,
    char* port_name)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    if (DIAG_FLEXE_GPORT_IS_FLEXE_CHANNEL(port))
    {
        sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "channel(%d)", port&0xFF);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_flexe_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "client(%d)", port);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1))
    {
        sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "t_mac(%d)", port);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "sar(%d)", port);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "flexe_phy(%d)", port);
    }
    else
    {
        sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "%s(%d)", BCM_PORT_NAME(unit, port), port);
        /* sal_snprintf(port_name, DIAG_FLEXE_PORT_NAME_LEN, "%s(%d)", "XXX", port); */
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_diag_flexe_dump_flow_client(
    int unit,
    uint32 src_port,
    uint32 dst_num,
    bcm_port_t *dst_port,
    uint32* flow_num,
    prt_control_t *prt_ctr)
{
    uint32 ii;
    dnx_algo_port_info_s dst_port_info;
    char port_name1[DIAG_FLEXE_PORT_NAME_LEN];
    char port_name2[DIAG_FLEXE_PORT_NAME_LEN];
    char port_name3[DIAG_FLEXE_PORT_NAME_LEN];
    bcm_port_t dst_eth_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_port_name(unit, src_port, port_name1));
    for (ii=0; ii<dst_num; ii++)
    {
        if (ii == 0)
        {
            if (*flow_num == 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }
            PRT_CELL_SET("%s ->", port_name1);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
        }

        SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_port[ii], port_name2));
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dst_port[ii], &dst_port_info));
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, dst_port_info))
        {
            /* DNX_ALGO_PORT_FLEXE_FLOW_CLIENT2CLENT */
            PRT_CELL_SET("%-20s", port_name2);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_virtual_client(unit, dst_port[ii], &dst_eth_port));
            if (dst_eth_port != SOC_PORT_INVALID)
            {
                SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_eth_port, port_name3));
                PRT_CELL_SET("%-20s -> %s", port_name2, port_name3);
            }
            else
            {
                PRT_CELL_SET("%s(*)", port_name2);
            }
        }
    }
    *flow_num = *flow_num + 1;

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_diag_flexe_dump_flow_eth_ilkn(
    int unit,
    uint32 src_port,
    bcm_port_t dst_port,
    bcm_pbmp_t *exclude_vClient_2_eth,
    uint32* flow_num,
    prt_control_t *prt_ctr)
{
    uint32 ii;
    dnx_algo_port_info_s dst_port_info;
    char port_name[DIAG_FLEXE_PORT_NAME_LEN];
    char port_name_1[DIAG_FLEXE_PORT_NAME_LEN];
    uint32  dst_num;
    bcm_port_t  dst_client[DIAG_FLEXE_FLOW_DST_PORT_MAX];
    bcm_port_t dst_eth_port;
    char str_dst_row[DIAG_DISP_ROW_MAX_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_port_name(unit, src_port, port_name));
    if (*flow_num == 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    }
    else
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    }
    PRT_CELL_SET("%s ->", port_name);

    SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_port, port_name_1));
    SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_client(unit, dst_port, &dst_num, dst_client));
    if (dst_num == 0)
    {
        /* flow from eth was break at virtual client */
        PRT_CELL_SET("%s(*)", port_name_1);
    }
    else
    {
        for (ii=0; ii<dst_num; ii++)
        {
            BCM_PBMP_PORT_ADD(*exclude_vClient_2_eth, dst_client[ii]);
            sal_memset(str_dst_row, 0, sizeof(str_dst_row));
            if (ii == 0)
            {
                sal_snprintf(str_dst_row, sizeof(str_dst_row), "%-20s", port_name_1);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("");
                sal_snprintf(str_dst_row, sizeof(str_dst_row), "%-20s", "");
            }

            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dst_client[ii], &dst_port_info));
            if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, dst_port_info))
            {
                /* flow from eth was dest to normal client */
                SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_client[ii], port_name));
                sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                     sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %-20s", port_name);
            }
            else
            {
                /* flow from eth was loopback at virtual client */
                SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_virtual_client(unit, dst_client[ii], &dst_eth_port));
                if (dst_eth_port != SOC_PORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_client[ii], port_name));
                    sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                                 sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %-20s", port_name);
                    SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_eth_port, port_name));
                    sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                                sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %-20s", port_name);
                }
                else
                {
                    SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_client[ii], port_name));
                    sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                                 sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %s(*)", port_name);
                }
            }

            PRT_CELL_SET("%s", str_dst_row);
        }
    }
    *flow_num = *flow_num + 1;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_diag_flexe_dump_flow_v_client_ing(
    int unit,
    bcm_port_t v_client,
    uint32 *flow_num,
    prt_control_t *prt_ctr)
{
    bcm_port_t dst_eth_port;
    char port_name1[DIAG_FLEXE_PORT_NAME_LEN];
    char port_name2[DIAG_FLEXE_PORT_NAME_LEN];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_virtual_client(unit, v_client, &dst_eth_port));
    if (dst_eth_port != SOC_PORT_INVALID)
    {
        if (*flow_num == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("");
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
        }

        SHR_IF_ERR_EXIT(flexe_port_name(unit, v_client, port_name1));
        SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_eth_port, port_name2));
        PRT_CELL_SET("%-20s -> %s", port_name1, port_name2);

        *flow_num = *flow_num + 1;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_dump_flow_v_client_egr(
    int unit,
    bcm_port_t v_client,
    uint32 *flow_num,
    prt_control_t *prt_ctr)
{
    uint32 ii;
    dnx_algo_port_info_s dst_port_info;
    char port_name[DIAG_FLEXE_PORT_NAME_LEN];
    char port_name_1[DIAG_FLEXE_PORT_NAME_LEN];

    uint32  dst_num;
    bcm_port_t  dst_client[DIAG_FLEXE_FLOW_DST_PORT_MAX];
    char str_dst_row[DIAG_DISP_ROW_MAX_LEN];
    bcm_port_t dst_eth_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_client(unit, v_client, &dst_num, dst_client));
    if (dst_num == 0)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(flexe_port_name(unit, v_client, port_name_1));
    for (ii=0; ii<dst_num; ii++)
    {
        sal_memset(str_dst_row, 0, sizeof(str_dst_row));
        if (ii == 0)
        {
            if (*flow_num == 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("");
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("");
            }
            sal_snprintf(str_dst_row, sizeof(str_dst_row), "%-20s", port_name_1);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            sal_snprintf(str_dst_row, sizeof(str_dst_row), "%-20s", "");
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dst_client[ii], &dst_port_info));
        if (dst_port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_CLIENT)
        {
            /* flow from eth was dest to normal client */
            SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_client[ii], port_name));
            sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                 sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %s", port_name);
        }
        else
        {
            /* flow from eth was loopback at virtual client */
            SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_virtual_client(unit, dst_client[ii], &dst_eth_port));
            if (dst_eth_port != SOC_PORT_INVALID)
            {
                SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_client[ii], port_name));
                sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                             sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %-20s", port_name);

                SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_eth_port, port_name));
                sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                            sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %-20s", port_name);
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_port_name(unit, dst_client[ii], port_name));
                sal_snprintf(str_dst_row + sal_strlen(str_dst_row),
                             sizeof(str_dst_row) - sal_strlen(str_dst_row), " -> %s(*)", port_name);
            }
        }

        PRT_CELL_SET("%s", str_dst_row);
    }
    *flow_num = *flow_num + 1;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_flow_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t client_ports, v_client_ports;
    bcm_pbmp_t eth_ports, ilkn_ports;
    bcm_pbmp_t exclude_vClient_2_eth, exclude_vClient_2_client;
    bcm_port_t port, dst_port[DIAG_FLEXE_FLOW_DST_PORT_MAX];
    uint32     dst_num, ii, flow_num;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE flow configure");
    PRT_COLUMN_ADD("SrcPort");
    PRT_COLUMN_ADD("DstPort");

    BCM_PBMP_CLEAR(exclude_vClient_2_eth);
    BCM_PBMP_CLEAR(exclude_vClient_2_client);
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_CLIENT, &client_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_CLIENT_VIRTUAL, &v_client_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_ETH, &eth_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_ILKN, &ilkn_ports));

    /* flow from flexe client(bus A) */
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("Bus A");
    PRT_CELL_SET("DstPort(Bus A/B/C [->ETH/ILKN port])");
    flow_num = 0;
    BCM_PBMP_ITER(client_ports, port)
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_client(unit, port, &dst_num, dst_port));
        if (dst_num > 0)
        {
            for (ii=0; ii<dst_num; ii++)
            {
                BCM_PBMP_PORT_ADD(exclude_vClient_2_eth, dst_port[ii]);
            }
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_flow_client(unit, port, dst_num, dst_port, &flow_num, prt_ctr));
        }
    }

    /*
     * Flow from ETH port.
     */
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("");
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("ETH");
    PRT_CELL_SET("DstPort(Bus B/C->BusA  or  Bus B/C->Bus B/C->Eth/ILKN)");
    flow_num = 0;
    BCM_PBMP_ITER(eth_ports, port)
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_eth(unit, port, dst_port));
        if (dst_port[0] != SOC_PORT_INVALID)
        {
            BCM_PBMP_PORT_ADD(exclude_vClient_2_client, dst_port[0]);
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_flow_eth_ilkn(unit, port, dst_port[0],
                                                              &exclude_vClient_2_eth,
                                                              &flow_num, prt_ctr));
        }
    }

    /*
     * Flow from ILKN port.
     */
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("");
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("InterLaken");
    PRT_CELL_SET("DstPort(Bus B/C->BusA  or  Bus B/C->Bus B/C->Eth/ILKN)");
    flow_num = 0;
    BCM_PBMP_ITER(ilkn_ports, port)
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_ilkn(unit, port, dst_port));
        if (dst_port[0] != SOC_PORT_INVALID)
        {
            BCM_PBMP_PORT_ADD(exclude_vClient_2_client, dst_port[0]);
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_flow_eth_ilkn(unit, port, dst_port[0],
                                                              &exclude_vClient_2_eth,
                                                              &flow_num, prt_ctr));
        }
    }

    /* Incomplete flow dump */
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("");
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("Incomplete flow");
    PRT_CELL_SET("All start from Bus B/C");
    /* Incomplete flow dump */
    flow_num = 0;
    BCM_PBMP_ITER(v_client_ports, port)
    {
        if (!BCM_PBMP_MEMBER(exclude_vClient_2_eth, port))
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_flow_v_client_ing(unit, port, &flow_num, prt_ctr));
        }
    }
    flow_num = 0;
    BCM_PBMP_ITER(v_client_ports, port)
    {
        if (!BCM_PBMP_MEMBER(exclude_vClient_2_client, port))
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_flow_v_client_egr(unit, port, &flow_num, prt_ctr));
        }
    }
    PRT_COMMITX;
    cli_out("\n\n");

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_flow_add_options[] = {
    {"SouRCe",      SAL_FIELD_TYPE_INT32, "logical port number of source client",       NULL},
    {"DeSTination", SAL_FIELD_TYPE_INT32, "logical port number of destination client",  NULL},
    {"CHaNnel",     SAL_FIELD_TYPE_INT32, "Channel index of flexE flow",                "0",    NULL, "0-2"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_flow_clear_options[] = {
    {"SouRCe",      SAL_FIELD_TYPE_INT32, "logical port number of source client",       NULL},
    {"DeSTination", SAL_FIELD_TYPE_INT32, "logical port number of destination client",  NULL},
    {"CHaNnel",     SAL_FIELD_TYPE_INT32, "Channel index of flexE flow",                "0",    NULL, "0-2"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_flow_dump_options[] = {
    {NULL}
};




static sh_sand_man_t dnx_diag_flexe_flow_add_man = {
    .brief = "Add FlexE cross connect command\n",
    .examples = "SouRCe=100 DeSTination=101\n" "SouRCe=100 DeSTination=101 CHaNnel=1"
};

static sh_sand_man_t dnx_diag_flexe_flow_clear_man = {
    .brief = "Delete FlexE cross connect command\n",
    .examples = "SouRCe=100 DeSTination=101\n" "SouRCe=100 DeSTination=101 CHaNnel=1"
};

static sh_sand_man_t dnx_diag_flexe_flow_dump_man = {
    .brief = "Dump FlexE cross connect command\n",
    .examples = ""
};

sh_sand_cmd_t dnx_diag_flexe_flow_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"Add",   dnx_diag_flexe_flow_add_cmd,    NULL, dnx_diag_flexe_flow_add_options,   &dnx_diag_flexe_flow_add_man,   NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DuMP",  dnx_diag_flexe_flow_dump_cmd,   NULL, dnx_diag_flexe_flow_dump_options,  &dnx_diag_flexe_flow_dump_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"CLear", dnx_diag_flexe_flow_clear_cmd,  NULL, dnx_diag_flexe_flow_clear_options, &dnx_diag_flexe_flow_clear_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {NULL}
};

static sh_sand_man_t  dnx_diag_flexe_flow_man = {
    .brief = "FlexE flow commands \n",
};

/*
----------------------------------------------------------------------------------------
----------------------------flexe overhead command--------------------------------------
----------------------------------------------------------------------------------------
    flexe overhead set type=gid/phyid/CalInUse/CalReq/CalAck port=xxx RX/TX val=x
    flexe overhead dump type=gid/phyid/CalInUse/CalReq/CalAck port=xxx

int bcm_port_flexe_oh_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_type_t type,
    int val);

int bcm_port_flexe_oh_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_type_t type,
    int *val);

typedef enum
{
    bcmPortFlexeOhGroupID = 0;
    bcmPortFlexeOhLogicalPhyID;
    bcmPortFlexeOhCalInUse;
    bcmPortFlexeOhCalRequest;
    bcmPortFlexeOhCalAck;
} bcm_port_flexe_oh_type_t;
*/
static shr_error_e
dnx_diag_flexe_overhead_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_flexe_oh_type_t type;
    int     flexe_type, oh_port;
    uint32  value;
    bcm_gport_t gport;
    uint32  flags;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("TYpe", flexe_type);
    SH_SAND_GET_INT32("Port", oh_port);
    SH_SAND_GET_UINT32("VALue", value);

    /*if (flexe_direction == diag_flexe_group_rx)
    {
        flags = BCM_PORT_FLEXE_RX;
    }
    else if (flexe_direction == diag_flexe_group_tx)
    {
        flags = BCM_PORT_FLEXE_TX;
    }
    else
    {
        flags = 0;
    }*/
    /* only tx was valid for oh set */
    flags = BCM_PORT_FLEXE_TX;

    switch (flexe_type)
    {
        case diag_flexe_oh_group_id:
        {
            type = bcmPortFlexeOhGroupID;
            BCM_FLEXE_GROUP_GPORT_SET(gport, oh_port);
            SHR_IF_ERR_EXIT(bcm_port_flexe_oh_set(unit, gport, flags, type, value));
            break;
        }
        case diag_flexe_oh_phy_id:
        {
            type = bcmPortFlexeOhLogicalPhyID;
            gport = oh_port;
            SHR_IF_ERR_EXIT(bcm_port_flexe_oh_set(unit, gport, flags, type, value));
            break;
        }
        case diag_flexe_oh_c:
        {
            type = bcmPortFlexeOhCalInUse;
            BCM_FLEXE_GROUP_GPORT_SET(gport, oh_port);
            SHR_IF_ERR_EXIT(bcm_port_flexe_oh_set(unit, gport, flags, type, value));
            break;
        }
        case diag_flexe_oh_cr:
        {
            type = bcmPortFlexeOhCalRequest;
            BCM_FLEXE_GROUP_GPORT_SET(gport, oh_port);
            SHR_IF_ERR_EXIT(bcm_port_flexe_oh_set(unit, gport, flags, type, value));
            break;
        }
        case diag_flexe_oh_ca:
        {
            type = bcmPortFlexeOhCalAck;
            BCM_FLEXE_GROUP_GPORT_SET(gport, oh_port);
            SHR_IF_ERR_EXIT(bcm_port_flexe_oh_set(unit, gport, flags, type, value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid input type\n");
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_dump_group_oh(
    int unit,
    prt_control_t *prt_ctr)
{
    int group_id, CalInUse, CalRequest, CalAck;
    bcm_gport_t gport;
    uint32 index, flags;
    bcm_port_flexe_group_phy_info_t *phy_info = NULL;
    int rc, actual_nof_pcs;
    bcm_port_flexe_oh_type_t type;
    bcm_pbmp_t group_ports;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(phy_info, sizeof(bcm_port_flexe_group_phy_info_t) * DIAG_FLEXE_MAX_PHY_OF_GROUP,
              "SHR_ALLOC", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_GROUP, &group_ports));
    BCM_PBMP_ITER(group_ports, index)
    {
        flags = 0;
        BCM_FLEXE_GROUP_GPORT_SET(gport, index);
        rc = bcm_port_flexe_group_get(unit, gport, flags, DIAG_FLEXE_MAX_PHY_OF_GROUP, phy_info, &actual_nof_pcs);
        if (rc != _SHR_E_NONE)
        {
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Group-%d", index);

        flags = BCM_PORT_FLEXE_TX;
        type = bcmPortFlexeOhGroupID;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &group_id));
        type = bcmPortFlexeOhCalInUse;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &CalInUse));
        type = bcmPortFlexeOhCalRequest;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &CalRequest));
        type = bcmPortFlexeOhCalAck;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &CalAck));
        PRT_CELL_SET("TX:  GroupID=0x%05x, CalInUse=%d, CalRequest=%d, CalAck=%d",
                      group_id, CalInUse, CalRequest, CalAck);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("");

        flags = BCM_PORT_FLEXE_RX;
        type = bcmPortFlexeOhGroupID;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &group_id));
        type = bcmPortFlexeOhCalInUse;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &CalInUse));
        type = bcmPortFlexeOhCalRequest;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &CalRequest));
        type = bcmPortFlexeOhCalAck;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, gport, flags, type, &CalAck));
        PRT_CELL_SET("RX:  GroupID=0x%05x, CalInUse=%d, CalRequest=%d, CalAck=%d",
                      group_id, CalInUse, CalRequest, CalAck);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_dump_phy_oh(
    int unit,
    prt_control_t *prt_ctr)
{
    bcm_pbmp_t flexe_phys;
    int port, tx_phyID, rx_phyID;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_PHY, &flexe_phys));
    BCM_PBMP_ITER(flexe_phys, port)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("FlexEPhy-%d", port);

        flags = BCM_PORT_FLEXE_TX;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, port, flags, bcmPortFlexeOhLogicalPhyID, &tx_phyID));
        flags = BCM_PORT_FLEXE_RX;
        SHR_IF_ERR_EXIT(bcm_port_flexe_oh_get(unit, port, flags, bcmPortFlexeOhLogicalPhyID, &rx_phyID));
        PRT_CELL_SET("phyID(tx/rx)=%d/%d", tx_phyID, rx_phyID);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_overhead_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE OverHead Info");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Info");

    /* dump group overhead info */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_group_oh(unit, prt_ctr));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("");
    PRT_CELL_SET("");

    /* dump Phy overhead info */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_dump_phy_oh(unit, prt_ctr));

    PRT_COMMITX;
exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}


static sh_sand_option_t dnx_diag_flexe_overhead_set_options[] = {

    {"TYpe",      SAL_FIELD_TYPE_ENUM,   "FlexE overhead type",       NULL,   (void*)dnx_flexe_oh_type_enum_table},
    {"Port",      SAL_FIELD_TYPE_INT32,  "FlexE Index(base on TYpe)", NULL},
    {"VALue",     SAL_FIELD_TYPE_UINT32, "FlexE OH set value",        "0"},

    /*
    {"DIRection", SAL_FIELD_TYPE_ENUM,   "FlexE client direction(RX/TX/ALL)", "tx",  (void*)dnx_flexe_direction_enum_table},
    {"CaLenDAR",  SAL_FIELD_TYPE_ENUM,   "FlexE group calendar(CalA/CalB)",   "CalA", (void*)dnx_flexe_calendar_enum_table},
    */
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_overhead_dump_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_overhead_set_man = {
    .brief = "Set FlexE Over head command\n",
    .examples = "type=C port=100 VALue=0"
};


static sh_sand_man_t dnx_diag_flexe_overhead_dump_man = {
    .brief = "Dump FlexE Over head command\n",
    .examples = ""
};


sh_sand_cmd_t dnx_diag_flexe_overhead_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"Set",  dnx_diag_flexe_overhead_set_cmd,  NULL, dnx_diag_flexe_overhead_set_options,  &dnx_diag_flexe_overhead_set_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DuMP", dnx_diag_flexe_overhead_dump_cmd, NULL, dnx_diag_flexe_overhead_dump_options, &dnx_diag_flexe_overhead_dump_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {NULL}
};

static sh_sand_man_t  dnx_diag_flexe_overhead_man = {
    .brief = "FlexE overhead commands \n",
};


#if 1
/*
----------------------------------------------------------------------------------------
-------------------------------FlexE core command---------------------------------------
----------------------------------------------------------------------------------------
*/
static shr_error_e
dnx_diag_flexe_core_debug_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 debug_idx;
    uint32 debug_value[10];
    char   *string;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("INDex", debug_idx);
    SH_SAND_GET_UINT32("VALue_0", debug_value[0]);
    SH_SAND_GET_UINT32("VALue_1", debug_value[1]);
    SH_SAND_GET_STR("PARAMeter", string);

    SHR_IF_ERR_EXIT(dnx_diag_flexe_core_run_debug_function(unit, debug_idx, debug_value, string, sand_control));
exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

void
diag_flexe_print_mem_entry(
    int unit,
    uint32 *entry_data,
    uint32 num_bytes)
{
    char tmp[64];

    _shr_format_long_integer(tmp, entry_data, num_bytes);
    cli_out("%s\n", tmp);

    return;
}


static shr_error_e
dnx_diag_flexe_core_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int reg, mem, ii, id_present;
    uint32 id, min_id, max_id;
    uint32 min_entry, max_entry, array_idx;
    int address, counter;
    int mem_index, mem_Num_BIts, width_8;
    int max_reg_in_line = 8;
    uint32 value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    SH_SAND_IS_PRESENT("ID", id_present);
    SH_SAND_GET_UINT32_RANGE("ID", min_id, max_id);

    if (reg == mem)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Specify reg or mem to be read.\n");
    }

    if (reg == TRUE)
    {
        if (id_present)
        {
            for (id=min_id; id<=max_id; id++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_reg_id_dump(unit, id, sand_control));
            }
        }
        else
        {
            SH_SAND_GET_INT32("ADdRess", address);
            SH_SAND_GET_INT32("count", counter);

            for (ii=0; ii<counter; ii++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, address+ii, value));
                if ((ii % max_reg_in_line) == 0)
                {
                    cli_out("\n%06X: ", address+ii);
                }
                cli_out(" %08X", value[0]);
            }
            cli_out("\n");
        }
    }
    else if (mem == TRUE)
    {
        if (id_present)
        {
            SH_SAND_GET_UINT32("ARray", array_idx);
            SH_SAND_GET_UINT32_RANGE("ENTry", min_entry, max_entry);
            for (id=min_id; id<=max_id; id++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_ram_id_dump(unit, id, array_idx, min_entry, max_entry, sand_control));
            }
        }
        else
        {
            SH_SAND_GET_INT32("ADdRess", address);
            SH_SAND_GET_INT32("count", counter);
            SH_SAND_GET_INT32("index", mem_index);
            SH_SAND_GET_INT32("Num_BIts", mem_Num_BIts);
            for (ii=0; ii<counter; ii++)
            {
                width_8 = BITS2BYTES(mem_Num_BIts);
                SHR_IF_ERR_EXIT(diag_flexe_mem_read(unit, address, mem_index+ii, mem_Num_BIts, value));

                cli_out("%-6d:  ", mem_index+ii);
                diag_flexe_print_mem_entry(unit, value, width_8);
            }
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_core_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int reg, mem, ii, id_present;
    uint32 id, min_id, max_id;
    uint32 min_entry, max_entry, array_idx;
    int address, counter;
    int mem_index, mem_Num_BIts;
    uint32* value_uint32;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    SH_SAND_GET_ARRAY32("VALue", value_uint32);
    SH_SAND_IS_PRESENT("ID", id_present);
    SH_SAND_GET_UINT32_RANGE("ID", min_id, max_id);

    if (reg == mem)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Specify reg or mem to be write.\n");
    }

    if (reg == TRUE)
    {
        if (id_present)
        {
            cli_out("not support now!!");
            SHR_EXIT();
        }
        else
        {
            SH_SAND_GET_INT32("ADdRess", address);
            SH_SAND_GET_INT32("count", counter);

            for (ii=0; ii<counter; ii++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_reg_write(unit, address+ii, value_uint32[0]));
            }
        }
    }
    else if (mem == TRUE)
    {
        if (id_present)
        {
            SH_SAND_GET_UINT32("ARray", array_idx);
            SH_SAND_GET_UINT32_RANGE("ENTry", min_entry, max_entry);
            for (id=min_id; id<=max_id; id++)
            {
                for (ii=min_entry; ii<=max_entry; ii++)
                {
                    SHR_IF_ERR_EXIT(diag_flexe_test_ram_id_write(unit, id, array_idx, ii, value_uint32));
                }
            }
        }
        else
        {
            SH_SAND_GET_INT32("ADdRess", address);
            SH_SAND_GET_INT32("count", counter);
            SH_SAND_GET_INT32("index", mem_index);
            SH_SAND_GET_INT32("Num_BIts", mem_Num_BIts);
            for (ii=0; ii<counter; ii++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_mem_write(unit, address, mem_index+ii, mem_Num_BIts, value_uint32));
            }
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_core_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int parameter;
    diag_flexe_core_conf_t conf_type;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("type", conf_type);
    SH_SAND_GET_INT32("PARAMeter", parameter);

    switch (conf_type)
    {
        case diag_flexe_core_conf_b66switch:
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_dump_b66switch(unit, sand_control));
            break;
        }
        case diag_flexe_core_conf_b66sar:
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_dump_b66sar(unit, sand_control));
            break;
        }
        case diag_flexe_core_conf_oam_alarm:
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_dump_oam_alarm(unit, sand_control));
            break;
        }
        case diag_flexe_core_conf_oh_alarm:
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_dump_oh_alarm(unit, sand_control));
            break;
        }
        case diag_flexe_core_conf_oam_counter:
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_dump_oam_cnt(unit, parameter, sand_control));
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_core_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int reg, mem;
    int name_present, id_present;
    uint32 min_id, max_id;
    char *list_name = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    SH_SAND_IS_PRESENT("ID", id_present);

    if (reg == mem)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "reg or mem should be specified.\n");
    }

    if (id_present)
    {
        SH_SAND_GET_UINT32_RANGE("ID", min_id, max_id);
        if (reg)
        {
            SHR_IF_ERR_EXIT(diag_flexe_reg_ram_id_list(unit, 0, min_id, max_id, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(diag_flexe_reg_ram_id_list(unit, 1, min_id, max_id, sand_control));
        }
    }
    else
    {
        list_name = NULL;
        SH_SAND_IS_PRESENT("name", name_present);
        if (name_present)
        {
            SH_SAND_GET_STR("name", list_name);
        }
        if (reg)
        {
            SHR_IF_ERR_EXIT(diag_flexe_reg_ram_list(unit, 0, list_name, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(diag_flexe_reg_ram_list(unit, 1, list_name, sand_control));
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_diag_flexe_core_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;
    int reg, mem, changed;
    uint32 array_idx=0;
    uint32 id, min_id, max_id;
    uint32 min_entry=0, max_entry=1;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("type", test_type);
    SH_SAND_GET_UINT32_RANGE("ID", min_id, max_id);
    SH_SAND_GET_UINT32_RANGE("ENTry", min_entry, max_entry);
    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    SH_SAND_GET_BOOL("CHanGed", changed);
    SH_SAND_GET_UINT32("ARray", array_idx);
    SH_SAND_GET_UINT32("FLaG", flags);

    if (test_type)
    {
        if (sal_strcasecmp(test_type, "dump") == 0)
        {
            if (reg == mem)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "reg or mem should be specified.\n");
            }
            if (changed)
            {
                if (reg)
                {
                    SHR_IF_ERR_EXIT(diag_flexe_test_reg_default(unit, min_id, max_id, flags, sand_control));
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_flexe_test_ram_default(unit, min_id, max_id, flags, sand_control));
                }
            }
            else
            {
                for (id=min_id; id<=max_id; id++)
                {
                    if (reg)
                    {
                        SHR_IF_ERR_EXIT(diag_flexe_reg_id_dump(unit, id, sand_control));
                    }
                    else if (mem)
                    {
                        SHR_IF_ERR_EXIT(diag_flexe_ram_id_dump(unit, id, array_idx, min_entry, max_entry, sand_control));
                    }
                }
            }
        }
        else if (sal_strcasecmp(test_type, "default") == 0)
        {
            if (reg == mem)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "reg or mem should be specified.\n");
            }
            if (reg)
            {
                SHR_IF_ERR_EXIT(diag_flexe_test_reg_default_set(unit, min_id, max_id));
            }
            else if (mem)
            {
                SHR_IF_ERR_EXIT(diag_flexe_test_ram_default_set(unit, min_id, max_id));
            }
        }
        else if (sal_strcasecmp(test_type, "reset") == 0)
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_reset(unit));
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}


static sh_sand_enum_t sand_diag_flexe_core_enum[] = {
    {"max", SAL_UINT32_MAX, "Max value for FlexE core reg/ram ID no", NULL},
    {NULL}
};


static sh_sand_option_t dnx_diag_flexe_core_debug_options[] = {
    {"INDex",       SAL_FIELD_TYPE_UINT32, "FH debug function id(0 show help info)",   "0"},

    {"PARAMeter",   SAL_FIELD_TYPE_STR,      "FlexE debug function param of string",    ""},
    {"VALue_0",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 0",      "0"},
    {"VALue_1",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 1",      "0"},
    /*{"VALue_2",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 2",      "0"},
    {"VALue_3",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 3",      "0"},
    {"VALue_4",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 4",      "0"},
    {"VALue_5",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 5",      "0"},
    {"VALue_6",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 6",      "0"},
    {"VALue_7",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 7",      "0"},
    {"VALue_8",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 8",      "0"},
    {"VALue_9",     SAL_FIELD_TYPE_UINT32,   "FlexE debug function param value 9",      "0"},*/
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_core_read_options[] = {
    {"register", SAL_FIELD_TYPE_BOOL,   "Flag used to specify that we will read register", "No"},
    {"memory",   SAL_FIELD_TYPE_BOOL,   "Flag used to specify that we will read memory",   "No"},
    {"ADdRess",  SAL_FIELD_TYPE_INT32,  "Register address",                         "0"},
    {"index",    SAL_FIELD_TYPE_INT32,  "Memory entry Index",                       "-1"},
    {"count",    SAL_FIELD_TYPE_INT32,  "number of registers or memory entries",    "1"},
    {"Num_BIts", SAL_FIELD_TYPE_INT32,  "number of bits at memory entry",           "-1"},

    {"ID",       SAL_FIELD_TYPE_UINT32, "The range of reg/ram ID no", "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {"ENTry",    SAL_FIELD_TYPE_UINT32, "The range of memory entry",  "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {"ARray",    SAL_FIELD_TYPE_UINT32, "The memory arrary index",    "0"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_core_write_options[] = {
    {"register", SAL_FIELD_TYPE_BOOL,   "Flag used to specify that we will set a value to a register", "No"},
    {"memory",   SAL_FIELD_TYPE_BOOL,   "Flag used to specify that we will set a value to a memory",   "No"},
    {"ADdRess",  SAL_FIELD_TYPE_INT32,  "Register address",                         "0"},
    {"index",    SAL_FIELD_TYPE_INT32,  "Memory entry Index",                       "-1"},
    {"count",    SAL_FIELD_TYPE_INT32,  "number of registers or memory entries",    "1"},
    {"Num_BIts", SAL_FIELD_TYPE_INT32,  "number of bits at memory entry",           "0"},
    {"VALue",    SAL_FIELD_TYPE_ARRAY32, "New value of the mem/reg",                NULL},

    {"ID",       SAL_FIELD_TYPE_UINT32, "The range of reg/ram ID no", "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {"ENTry",    SAL_FIELD_TYPE_UINT32, "The range of memory entry",  "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {"ARray",    SAL_FIELD_TYPE_UINT32, "The memory arrary index",    "0"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_core_dump_options[] = {
    {"type",        SAL_FIELD_TYPE_ENUM,  "FlexE core configure type",  NULL, (void*)dnx_flexe_core_conf_enum_table, NULL, SH_SAND_ARG_FREE},
    {"PARAMeter",   SAL_FIELD_TYPE_INT32, "FlexE core configure param", "0"},
    {NULL}
};



static sh_sand_option_t dnx_diag_flexe_core_list_options[] = {
    {"register", SAL_FIELD_TYPE_BOOL,   "Flag used to specify that if list register", "No"},
    {"memory",   SAL_FIELD_TYPE_BOOL,   "Flag used to specify that if list memory",   "No"},
    {"name",     SAL_FIELD_TYPE_STR,    "name of flexe core reg/ram to list",         "", NULL, NULL, SH_SAND_ARG_FREE},
    {"ID",       SAL_FIELD_TYPE_UINT32, "The range of reg/ram ID no", "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_diag_flexe_core_test_options[] = {
    {"type",     SAL_FIELD_TYPE_STR,    "FlexE reg/memory test type", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"register", SAL_FIELD_TYPE_BOOL,   "Flag used to specify that if test register", "No"},
    {"memory",   SAL_FIELD_TYPE_BOOL,   "Flag used to specify that if test memory",   "No"},
    {"CHanGed",  SAL_FIELD_TYPE_BOOL,   "Flag used to specify that if test memory",   "Yes"},
    {"ID",       SAL_FIELD_TYPE_UINT32, "The range of reg/ram ID no", "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {"ENTry",    SAL_FIELD_TYPE_UINT32, "The range of memory entry",  "0", (void*) sand_diag_flexe_core_enum, "0-max"},
    {"ARray",    SAL_FIELD_TYPE_UINT32, "The memory arrary index",    "0"},
    {"FLaG",    SAL_FIELD_TYPE_UINT32,  "Test flag",    "0"},

    {NULL}
};



static sh_sand_man_t  dnx_diag_flexe_core_debug_man = {
    .brief = "FlexE core debug commands \n",
    .examples = "INDex=0 VALue_0=1 VALue_1=2\n" "INDex=7 PARAMeter=\"aaa\""
};

static sh_sand_man_t dnx_diag_flexe_core_read_man = {
    .brief = "Read FlexE core register/memory command\n",
    .examples = "register address=0x0 count=1\n" "memory address=0xa200 index=3 count=8 Num_BIts=24\n"
                "register ID=0x0\n" "memory ID=35 ARray=0 ENTry=1-3"
};

static sh_sand_man_t dnx_diag_flexe_core_write_man = {
    .brief = "Write FlexE core register/memory command\n",
    .examples = "register address=0x0 count=1 VALue=0x1\n"
                "memory address=0xa200 index=1 count=8 Num_BIts=24 VALue=0xff1234\n"
                "memory ID=35 ARray=0 ENTry=1-3 VALue=0xFFFF"
};

static sh_sand_man_t dnx_diag_flexe_core_dump_man = {
    .brief = "Dump FlexE core configure command\n",
    .full = "Include the configure of b66switch, b66sar, oam_alarm for flexe&sar, oh_alarm etc.\n"
            "For FlexE & SAR OAM's alarm, bits definition is:\n"
            "    Bit 0: RX_LPI;    Bit 1: RX_LF,   Bit 2: RX_RF,   Bit 3: BASE_OAM_LOS\n"
            "    Bit 4: RX_SDBIP;  Bit 5: RX_CRC,  Bit 6: RX_RDI,  Bit 7: RX_PERIOD_MISMATCH\n"
            "For OH's alarm, bits definition is:\n"
            "    Bit 0:  GIDM_ALM;  Bit 1:  phy_num mismatch,  Bit 2:  LOF,        Bit 3:  LOM\n"
            "    Bit 4:  RPF;       Bit 5:  OH1_BLOCK_ALM,     Bit 6:  C_BIT_ALM,  Bit 7:  PMM\n"
            "    Bit 8:  CCAM;      Bit 9:  CCBM,              Bit 10: FlexE_LOGA",
    .examples = "b66switch\n" "oam_alarm\n" "oam_counter PARAMeter=3"
};

static sh_sand_man_t dnx_diag_flexe_core_list_man = {
    .brief = "List FlexE core register/memory infomation\n",
    .examples = "register\n" "memory SWRAM\n"  "register id=1-3\n" "memory id=20-25"
};

static sh_sand_man_t dnx_diag_flexe_core_test_man = {
    .brief = "List FlexE core register/memory infomation\n",
    .examples = "register CHanGed dump id=2\n" "register default id=0-max\n"  "register dump id=0-max flag=1\n"
                "memory dump id=0-max array=0 entry=0 flag=1\n" "memory default id=0-max"
};


sh_sand_cmd_t dnx_diag_flexe_core_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"DeBuG",  dnx_diag_flexe_core_debug_cmd,  NULL, dnx_diag_flexe_core_debug_options, &dnx_diag_flexe_core_debug_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {"read",   dnx_diag_flexe_core_read_cmd,   NULL, dnx_diag_flexe_core_read_options,  &dnx_diag_flexe_core_read_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"write",  dnx_diag_flexe_core_write_cmd,  NULL, dnx_diag_flexe_core_write_options, &dnx_diag_flexe_core_write_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {"Dump",   dnx_diag_flexe_core_dump_cmd,   NULL, dnx_diag_flexe_core_dump_options,  &dnx_diag_flexe_core_dump_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"LIST",   dnx_diag_flexe_core_list_cmd,   NULL, dnx_diag_flexe_core_list_options,  &dnx_diag_flexe_core_list_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"TeST",   dnx_diag_flexe_core_test_cmd,   NULL, dnx_diag_flexe_core_test_options,  &dnx_diag_flexe_core_test_man,  NULL, NULL, SH_CMD_NOT_RELEASE},



    /*{"Dump",   NULL,  dnx_diag_flexe_core_dump_cmds, NULL, &dnx_diag_flexe_core_dump_man},*/

    {NULL}
};

static sh_sand_man_t  dnx_diag_flexe_core_man = {
    .brief = "FlexE core commands \n",
};
#endif

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
int
dnx_diag_is_flexe_core_enabled(
    int unit)
{
    int rc;
    shr_error_e rt;
    bcm_pbmp_t phy_ports;

    rc = 0;
    BCM_PBMP_CLEAR(phy_ports);
    rt = dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_PHY, &phy_ports);
    if (rt == _SHR_E_NONE)
    {
        if (_SHR_PBMP_NOT_NULL(phy_ports))
        {
            rc = 1;
        }
    }

    return rc;
}

shr_error_e
dnx_diag_flexe_info_pbmp_print(
    int unit,
    char *str_port_type,
    bcm_pbmp_t *pbmp,
    prt_control_t * prt_ctr)
{
    bcm_port_t port;
    char str_port_list[DIAG_DISP_ROW_MAX_LEN];
    int port_num, print_first;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(str_port_type,_SHR_E_PARAM,"str_port_type");
    SHR_NULL_CHECK(pbmp,_SHR_E_PARAM,"pbmp");

    if (BCM_PBMP_IS_NULL(*pbmp))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", str_port_type);
        PRT_CELL_SET("%s", "null");
        SHR_EXIT();
    }

    port_num = 0;
    print_first = 1;
    BCM_PBMP_ITER(*pbmp, port)
    {
        if (port_num == 0)
        {
            sal_memset(str_port_list, 0, sizeof(str_port_list));
        }

        sal_snprintf(str_port_list + sal_strlen(str_port_list),
                     sizeof(str_port_list) - sal_strlen(str_port_list), "%-4d", port);
        port_num++;

        if (port_num >= DIAG_DISP_PORT_NUM_IN_ROW)
        {
            port_num = 0;
            if (print_first)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("%s", str_port_type);
                PRT_CELL_SET("%s", str_port_list);
                print_first = 0;
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("");
                PRT_CELL_SET("%s", str_port_list);
            }
        }
    }
    if (port_num != 0)
    {
        if (print_first)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%s", str_port_type);
            PRT_CELL_SET("%s", str_port_list);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            PRT_CELL_SET("%s", str_port_list);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* global info of flexe, such as Centralized/Distributed mode */
static shr_error_e
dnx_diag_flexe_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flexe_mode = 0, flexe_driver;
    char* str_flexe_mode[] = {"DISABLED", "CENTRALIZED", "DISTRIBUTED"};
    bcm_pbmp_t phy_ports, group_ports, client_ports, client_macs, client_sars, eth_ports, ilkn_ports;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    flexe_mode = dnx_data_nif.flexe.flexe_mode_get(unit);
    flexe_driver = dnx_data_dev_init.general.flexe_core_drv_select_get(unit);
    PRT_TITLE_SET("FlexE mode=%s, driver: %s", str_flexe_mode[flexe_mode], flexe_driver? "STD" : "DBG");

    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_PHY, &phy_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_GROUP, &group_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_CLIENT, &client_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_CLIENT_MAC, &client_macs));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_CLIENT_SAR, &client_sars));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_ETH, &eth_ports));
    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_ILKN, &ilkn_ports));
    PRT_COLUMN_ADD("Port type");
    PRT_COLUMN_ADD("Ports");

    /* ETH L1 port */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "NIF ETH L1 ports", &eth_ports, prt_ctr));

    /* ILKN port */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "ILKN ports", &ilkn_ports, prt_ctr));

    /* FlexE Phy port */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "FlexE PHY", &phy_ports, prt_ctr));

    /* FlexE group */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "FlexE group", &group_ports, prt_ctr));

    /* Client */
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "FlexeE Client(A)", &client_ports, prt_ctr));
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "FlexeE Client(B)", &client_macs, prt_ctr));
    SHR_IF_ERR_EXIT(dnx_diag_flexe_info_pbmp_print(unit, "FlexeE Client(C)", &client_sars, prt_ctr));

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_info_options[] = {
    /*{"type", SAL_FIELD_TYPE_STR, "Configure mdb ser info option<table or map>", "", NULL, NULL, SH_SAND_ARG_FREE},*/
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_info_man = {
    .brief = "flexe info commands \n",
    .full = "flexe info\n",
    .synopsis = "",
    .examples = "" ""
};

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
sh_flexe_counter_register_t flexe_counter_flexewp_0[] =
{
    {
        FLEXEWP_FLEXEWP_NB_FLEXE_COUNTERSr,
        SOC_BLK_FLEXEWP,
        {NB_TX_CREDIT_COUNTER_FLEXE_PORT_Nf, NB_RX_DATA_IN_FLEXE_PORT_Nf, NB_TX_DATA_OUT_FLEXE_PORT_Nf, INVALIDf},
        0
    },
    {
        FLEXEWP_FLEXEWP_SB_FLEXE_IN_COUNTERSr,
        SOC_BLK_FLEXEWP,
        {SB_RX_DATA_IN_COUNTER_FLEXE_CLIENT_Nf, SB_RX_DATA_OUT_COUNTER_FLEXE_CLIENT_Nf, INVALIDf, INVALIDf},
        0
    }
};


sh_flexe_counter_register_t flexe_counter_flexewp_1[] =
{
    {
        FLEXEWP_NB_MSBUS_0_PORT_COUNTERr,
        SOC_BLK_FLEXEWP,
        {NB_TX_CREDIT_COUNTER_MSBUS_0_PORT_Nf, NB_TX_AM_CREDIT_COUNTER_MSBUS_0_PORT_Nf, NB_RX_DATA_IN_COUNTER_MSBUS_0_PORT_Nf, INVALIDf},
        0
    },
    {
        FLEXEWP_NB_MSBUS_1_PORT_COUNTERr,
        SOC_BLK_FLEXEWP,
        {NB_TX_CREDIT_COUNTER_MSBUS_1_PORT_Nf, NB_TX_AM_CREDIT_COUNTER_MSBUS_1_PORT_Nf, NB_RX_DATA_IN_COUNTER_MSBUS_1_PORT_Nf, INVALIDf},
        0
    },

    {
        FLEXEWP_FLEXEWP_SB_TX_FLEXE_OUT_COUNTERSr,
        SOC_BLK_FLEXEWP,
        {SB_TX_CREDIT_FROM_FLEXE_COUNTER_CLIENT_Nf, SB_TX_DATA_TO_FLEXE_CLIENT_Nf, INVALIDf, INVALIDf},
        0
    },
    {
        FLEXEWP_FLEXEWP_SB_TX_TMAC_COUNTERSr,
        SOC_BLK_FLEXEWP,
        {SB_TX_CREDIT_TO_TMAC_COUNTER_CLIENT_Nf, SB_TX_DATA_FROM_TMAC_CLIENT_Nf, INVALIDf, INVALIDf},
        0
    }
};


sh_flexe_counter_register_t flexe_counter_feu_0[] =
{
    {
        FEU_RX_TOTAL_BYTE_COUNTER_1r,
        SOC_BLK_FEU,
        {RX_TOTAL_BYTE_COUNTER_TO_IREf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_BYTE_COUNTER_2r,
        SOC_BLK_FEU,
        {RX_TOTAL_BYTE_COUNTER_TO_ESBf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_BYTE_COUNTER_3r,
        SOC_BLK_FEU,
        {RX_TOTAL_BYTE_COUNTER_TO_ILU_FROM_RXBUFf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_BYTE_COUNTER_FROM_PMr,
        SOC_BLK_FEU,
        {RX_TOTAL_BYTE_COUNTER_FROM_PMf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_BYTE_WITH_ERR_COUNTERr,
        SOC_BLK_FEU,
        {RX_TOTAL_BYTE_WITH_ERR_COUNTERf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_COUNTER_1r,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_COUNTER_TO_IREf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_COUNTER_2r,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_COUNTER_TO_ESBf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_COUNTER_3r,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_COUNTER_TO_ILU_FROM_RXBUFf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_COUNTER_4r,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_COUNTER_TO_ILU_FROM_SARf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_COUNTER_FROM_PMr,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_COUNTER_FROM_PMf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_COUNTER_FROM_PM_WITH_ERRr,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_COUNTER_FROM_PM_WITH_ERRf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_RX_TOTAL_PKT_WITH_ERR_COUNTERr,
        SOC_BLK_FEU,
        {RX_TOTAL_PKT_WITH_ERR_COUNTERf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
};

sh_flexe_counter_register_t flexe_counter_feu_1[] =
{
    {
        FEU_TX_TOTAL_BYET_SENT_TO_PM_1r,
        SOC_BLK_FEU,
        {TX_TOTAL_BYTE_SENT_TO_TMACf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_BYTE_COUNTER_1r,
        SOC_BLK_FEU,
        {TX_TOTAL_BYTE_COUNTER_FROM_ESBf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_BYTE_COUNTER_2r,
        SOC_BLK_FEU,
        {TX_TOTAL_BYTE_COUNTER_FROM_IRE_TO_TXBUFf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_BYTE_COUNTER_3r,
        SOC_BLK_FEU,
        {TX_TOTAL_BYTE_COUNTER_FROM_IREf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_ERR_ON_SOP_PKT_COUNTER_1r,
        SOC_BLK_FEU,
        {TX_TOTAL_ERR_ON_SOP_PKT_COUNTER_FROM_ESBf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_ERR_ON_SOP_PKT_COUNTER_2r,
        SOC_BLK_FEU,
        {TX_TOTAL_ERR_ON_SOP_PKT_COUNTER_FROM_IREf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_COUNTER_1r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_COUNTER_FROM_ESBf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_COUNTER_2r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_COUNTER_FROM_IRE_TO_TXBUFf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_COUNTER_3r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_COUNTER_FROM_IRE_TO_SARf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_SENT_TO_PM_1r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_SENT_TO_TMACf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_SENT_TO_PM_2r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_SENT_TO_TMAC_WITH_ERRORf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_WITH_ERR_COUNTER_1r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_WITH_ERR_COUNTER_FROM_ESBf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        FEU_TX_TOTAL_PKT_WITH_ERR_COUNTER_2r,
        SOC_BLK_FEU,
        {TX_TOTAL_PKT_WITH_ERR_COUNTER_FROM_IREf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
};

sh_flexe_counter_register_t flexe_counter_esb_0[] =
{
    {
        ESB_ESB_Q_CNTr,
        SOC_BLK_ESB,
        {ESB_Q_CNTf, INVALIDf, INVALIDf, INVALIDf},
        0
    },
    {
        ESB_FLEXE_INCOMING_COUNTERSr,
        SOC_BLK_FLEXEWP,
        {FLEXE_INCOMING_BYTES_CNTRf, FLEXE_INCOMING_ERRS_CNTRf, FLEXE_INCOMING_PKTS_CNTRf, FLEXE_INCOMING_SEGMENTS_CNTRf},
        0
    }
};

sh_flexe_counter_register_t flexe_counter_esb_1[] =
{
    {
        ESB_FLEXE_OUTGOING_COUNTERSr,
        SOC_BLK_ESB,
        {FLEXE_OUTGOING_BYTES_CNTRf, FLEXE_OUTGOING_PKTS_CNTRf, INVALIDf, INVALIDf},
        0
    }
};

sh_flexe_counter_register_t flexe_counter_nmg[] =
{
    {
        NMG_FEU_SHAPER_MASKS_SCH_CNTr,
        SOC_BLK_NMG,
        {FEU_SHAPER_HALTS_SCH_CNTf, FEU_SHAPER_MASKS_FEU_SCH_CNTf, INVALIDf, INVALIDf},
        0
    },
    {
        NMG_NMG_TO_FEU_DEBUG_COUNTERSr,
        SOC_BLK_NMG,
        {NMG_TO_FEU_BYTE_COUNTERf, NMG_TO_FEU_PKT_COUNTERf, INVALIDf, INVALIDf},
        0
    }
};

sh_flexe_block_counter_t flexe_block_counters_rx_client[] =
    {
        {"FLEXEWP", flexe_counter_flexewp_0, COUNTOF(flexe_counter_flexewp_0)},
        {"FEU", flexe_counter_feu_0, COUNTOF(flexe_counter_feu_0)},
        {"ESB", flexe_counter_esb_0, COUNTOF(flexe_counter_esb_0)},
    };

sh_flexe_block_counter_t flexe_block_counters_rx_nif[] =
    {
        {"NMG", flexe_counter_nmg, COUNTOF(flexe_counter_nmg)},
        {"ESB", flexe_counter_esb_1, COUNTOF(flexe_counter_esb_1)},
        {"FEU", flexe_counter_feu_1, COUNTOF(flexe_counter_feu_1)},
        {"FLEXEWP", flexe_counter_flexewp_1, COUNTOF(flexe_counter_flexewp_1)},
    };


/*
Block     register    field/value
XXX       COUNTER1     FFF:  111
          COUNTER2     DDD1: 222
                       DDD2: 333
                       DDD3: 444
          COUNTER3     XXX[64]
                           [0]:  aaa
                           [1]:  bbb
*/
static shr_error_e
sh_dnx_flexe_single_counter_info_print(
    int unit,
    int in_hex,
    char *block_name,
    sh_flexe_counter_register_t *counter_info,
    uint32 counter_num,
    prt_control_t * prt_ctr)
{
    uint32 ii, jj, kk;
    uint32 arrry_num, start_idx, index;
    soc_reg_t reg;
    soc_field_t field;

    int block_set, reg_set, row_valid;
    soc_reg_above_64_val_t reg_value;
    soc_reg_above_64_val_t fld_value;
    int field_len;
    char value_str[PRT_COLUMN_WIDTH_BIG];

    SHR_FUNC_INIT_VARS(unit);
    block_set = 0;
    for (ii=0; ii<counter_num; ii++)
    {
        reg_set = 0;

        reg = counter_info[ii].reg;
        if (SOC_REG_IS_ARRAY(unit, reg))
        {
            arrry_num = SOC_REG_NUMELS(unit, reg);
            start_idx = SOC_REG_INFO(unit, reg).first_array_index;
        }
        else
        {
            arrry_num = 1;
            start_idx = 0;
        }
        for (jj=0; jj<arrry_num; jj++)
        {
            index = start_idx + jj;
            SOC_REG_ABOVE_64_CLEAR(reg_value);
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, 0, index, reg_value));

            for (kk=0; kk<FLEXE_COUNTER_REG_NOF_FIELD; kk++)
            {
                field = counter_info[ii].cnt_field[kk];
                if (field == INVALIDf)
                {
                    continue;
                }
                SOC_REG_ABOVE_64_CLEAR(fld_value);

                field_len = soc_reg_field_length(unit, reg, field);
                soc_reg_above_64_field_get(unit, reg, reg_value, field, fld_value);

                row_valid = 1;
                if ((arrry_num > 1) && SOC_REG_ABOVE_64_IS_ZERO(fld_value))
                {
                    /* For array counter, only print no zero entry */
                    row_valid = 0;
                }

                if (row_valid)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /* block name */
                    if (block_set == 0)
                    {
                        PRT_CELL_SET("%s", block_name);
                        block_set = 1;
                    }
                    else
                    {
                        PRT_CELL_SET("");
                    }

                    /* reg name */
                    if (reg_set == 0)
                    {
                        if (arrry_num > 1)
                        {
                            PRT_CELL_SET("%s[%d]", SOC_REG_NAME(unit, reg), arrry_num);
                        }
                        else
                        {
                            PRT_CELL_SET("%s", SOC_REG_NAME(unit, reg));
                        }
                        reg_set = 1;
                    }
                    else
                    {
                        PRT_CELL_SET("");
                    }

                    /* Field name */
                    if (arrry_num > 1)
                    {
                        PRT_CELL_SET("%s[%d]", SOC_FIELD_NAME(unit, field), index);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", SOC_FIELD_NAME(unit, field));
                    }

                    /* Field Value */
                    format_value_to_str(fld_value, field_len, in_hex, value_str);
                    PRT_CELL_SET("%s", value_str);
                }
            }
        }


        if (reg_set == 0)
        {
            /* In case of all entry was zero for reg array, Just print a row for this counter */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            /* block name */
            if (block_set == 0)
            {
                PRT_CELL_SET("%s", block_name);
                block_set = 1;
            }
            else
            {
                PRT_CELL_SET("");
            }

            if (arrry_num > 1)
            {
                PRT_CELL_SET("%s[%d]", SOC_REG_NAME(unit, reg), arrry_num);
            }
            else
            {
                PRT_CELL_SET("%s", SOC_REG_NAME(unit, reg));
            }

            PRT_CELL_SET("");
            PRT_CELL_SET("All zero");
        }
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("");
    PRT_CELL_SET("");
    PRT_CELL_SET("");
    PRT_CELL_SET("");

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_flexe_block_counter_print(
    int unit,
    int in_hex,
    sh_flexe_block_counter_t *block_info,
    uint32 block_num,
    prt_control_t * prt_ctr)
{
    uint32 ii;
    SHR_FUNC_INIT_VARS(unit);

    for (ii=0; ii<block_num; ii++)
    {
        SHR_IF_ERR_EXIT(sh_dnx_flexe_single_counter_info_print(unit, in_hex, block_info[ii].block_name,
                                                               block_info[ii].counter_regs,
                                                               block_info[ii].counter_num, prt_ctr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the flexe counters in virable format specified by option
 * \param [in] unit - Unit #
 * \param [in] in_hex - if print in hex mode
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_diag_flexe_feu_counter_print(
    int unit,
    int in_hex,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE counter");

    PRT_COLUMN_ADD("BLOCK");
    PRT_COLUMN_ADD("CounterReg");
    PRT_COLUMN_ADD("Fields");
    PRT_COLUMN_ADD("Values");

    SHR_IF_ERR_EXIT(sh_dnx_flexe_block_counter_print(unit, in_hex, flexe_block_counters_rx_client,
                                                     COUNTOF(flexe_block_counters_rx_client), prt_ctr));

    SHR_IF_ERR_EXIT(sh_dnx_flexe_block_counter_print(unit, in_hex, flexe_block_counters_rx_nif,
                                                     COUNTOF(flexe_block_counters_rx_nif), prt_ctr));

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the flexe counters in virable format specified by option
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_diag_flexe_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int in_hex, clear_flag;
    diag_flexe_core_module_t core_module;
    diag_flexe_counter_print_opt_e print_mode;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("MODULE", core_module);
    SH_SAND_GET_ENUM("MoDe", print_mode);
    SH_SAND_GET_BOOL("hex", in_hex);
    SH_SAND_GET_BOOL("CLear", clear_flag);

    if (clear_flag)
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_core_counter_clear(unit));
    }
    else
    {
        if (core_module != diag_flexe_core_module_none)
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_counter_print(unit, in_hex, print_mode, core_module, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_feu_counter_print(unit, in_hex, sand_control));
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}


static sh_sand_option_t dnx_diag_flexe_counter_options[] = {
    {"MoDe",    SAL_FIELD_TYPE_ENUM, "FlexE ounter print mode",        "default", (void*)dnx_flexe_print_mode_enum_table},
    {"MODULE",  SAL_FIELD_TYPE_ENUM, "FlexE core module",              "none",  (void*)dnx_flexe_core_module_enum_table},
    {"hex",     SAL_FIELD_TYPE_BOOL, "display counter in hex format",  "false", NULL},
    {"CLear",   SAL_FIELD_TYPE_BOOL, "If clear Flexe core counter",    "false", NULL},
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_counter_man = {
    .brief = "flexe counter commands \n",
    .full = "flexe counter\n",
    .synopsis = "",
    .examples = "Mode=all hex\n" "MODULE=sar\n" "CLear"
};

#ifdef BE_HOST
static void
dnx_diag_bitop_range_copy_u64_to_u32_big_endian(
    uint32 *dest,
    int dest_offset,
    uint64 *u64_src,
    int src_offset,
    int num_bits)
{
    uint32 buf[2];

    buf[0] = COMPILER_64_LO(*u64_src);
    buf[1] = COMPILER_64_HI(*u64_src);

    SHR_BITCOPY_RANGE(dest, dest_offset, buf, src_offset, num_bits);
}
#endif

static shr_error_e
dnx_diag_flexe_dump_group_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    soc_reg_t grp_cfg_regs[] = { FSCL_FLEXE_INSTANCE_GRP_CFG_0r, FSCL_FLEXE_INSTANCE_GRP_CFG_1r };
    uint32 grp_instances = 0;
    uint32 grp_active_bmp = 0;
    uint32 flexegroup_info = 0;
    uint32 mode_info = 0;
    uint32 lf_sel = 0;
    uint32 min_instance=0;
    uint32 grp_cfg[2] = {0};
    uint32 instance;
    uint32 reg_val = 0;
    uint32 grp_mode = 0;
    uint32 deskew = 0;
    uint32 val = 0;
    char cell[64];
    char buf[8];
    char *alarm_desc[][2] =
        {
            {"E", "FIFO empty"},
            {"F", "FIFO full"},
            {"O", "Over margin"},
            {"A", "Alm"}
        };
    char *row_desc[] = {
            "Instances",
            "Instances mode",
            "LF select",
            "Deskew instance"
        };
    char *row_comments[] = {
            "base instance = NB port ID",
            "1: 100G; 0: 50G",
            "LF selects from NB port",
            "equals to the minimum instance in the group"
        };
    int ii, group;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE group info");
    PRT_COLUMN_ADD("Group");

    /*
    * Find active groups
    */
    for (group=0; group<DNX_DIAG_FLEXE_CORE_NOF_PORTS; ++group)
    {
        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, &grp_cfg[group/4]));
        /* coverity[ptr_arith:FALSE] */
        DNX_DIAG_FLEXE_MULTI_DESKEW_GRP_CFG_GET(&grp_cfg[group/4], group, &grp_instances);

        if (grp_instances != 0)
        {
            /* coverity[ptr_arith:FALSE] */
            SHR_BITSET(&grp_active_bmp, group);
        }
    }

    /*
    * Add a column for each of the groups
    */

    /* coverity[ptr_arith:FALSE] */
    SHR_BIT_ITER(&grp_active_bmp, DNX_DIAG_FLEXE_CORE_NOF_PORTS, group)
    {
        PRT_COLUMN_ADD("%d", group);
    }

    PRT_COLUMN_ADD("comments");

    SHR_IF_ERR_EXIT(READ_FSCL_MODE_INFOr(unit, &mode_info));
    SHR_IF_ERR_EXIT(READ_FSCL_LF_SELr(unit, &lf_sel));
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXEGROUP_INFOr(unit, &flexegroup_info));

    for (ii=0; ii<COUNTOF(row_desc); ++ii)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", row_desc[ii]);

        /* coverity[ptr_arith:FALSE] */
        SHR_BIT_ITER(&grp_active_bmp, DNX_DIAG_FLEXE_CORE_NOF_PORTS, group)
        {
            cell[0] = '\0';
            buf[0] = '\0';

            /* coverity[ptr_arith:FALSE] */
            DNX_DIAG_FLEXE_MULTI_DESKEW_GRP_CFG_GET(&grp_cfg[group/4], group, &grp_instances);

            if (grp_instances != 0)
            {
                for(instance=0; instance<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++instance)
                {
                    /* coverity[ptr_arith:FALSE] */
                    if (SHR_BITGET(&grp_instances, instance))
                    {
                        switch (ii)
                        {
                        case 0:
                            /*
                            * Dump instances belong to each group
                            */
                            sal_snprintf(buf, sizeof(buf), (instance==0)?"%d":"|%d", instance);
                            break;
                        case 1:
                            /*
                            * Print LF select for instances in the group
                            */
                            /* coverity[ptr_arith:FALSE] */
                            sal_snprintf(buf, sizeof(buf), (instance==0)?"%d":"|%d", SHR_BITGET(&mode_info, instance)?1:0);
                            break;
                        case 2:
                            /*
                            * Print LF select for instances in the group
                            */
                            /* coverity[ptr_arith:FALSE] */
                            DNX_DIAG_FLEXE_RX_LF_SEL_GET(&lf_sel, instance, &val);
                            sal_snprintf(buf, sizeof(buf), (instance==0)?"%d":"|%d", val);
                            break;
                        case 3:
                            /*
                            * Print deskew instance for instances in the group
                            */
                            /* coverity[ptr_arith:FALSE] */
                            DNX_DIAG_FLEXE_DEMUX_GROUP_INFO_GET(&flexegroup_info, instance, &min_instance);
                            sal_snprintf(buf, sizeof(buf), (instance==0)?"%d":"|%d", min_instance);
                            break;
                        default:
                            break;
                        }

                        sal_strncat(cell, buf, 2);
                    }
                    else
                    {
                        sal_strncat(cell, (instance==0)?"-":"|-", 2);
                    }
                }

                PRT_CELL_SET("%s", cell);
            }
        }

        PRT_CELL_SET("%s", row_comments[ii]);
    }

    /*
    * Dump instance mode for each group (100G/50G)
    */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Group mode");

    /* coverity[ptr_arith:FALSE] */
    SHR_BIT_ITER(&grp_active_bmp, DNX_DIAG_FLEXE_CORE_NOF_PORTS, group)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GRP_MODEr(unit, &reg_val));
        grp_mode = soc_reg_field_get(unit, FSCL_FLEXE_GRP_MODEr, reg_val, GRP_MODEf);

        /* coverity[ptr_arith:FALSE] */
        if (SHR_BITGET(&grp_mode, group))
        {
            PRT_CELL_SET("100G");
        }
        else
        {
            PRT_CELL_SET("50G");
        }
    }

    PRT_CELL_SET("");

    /*
    * Dump group deskew value
    */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Deskew value");

    /* coverity[ptr_arith:FALSE] */
    SHR_BIT_ITER(&grp_active_bmp, DNX_DIAG_FLEXE_CORE_NOF_PORTS, group)
    {
        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, FSCL_FLEXE_DSW_MEASURE_0r + group/2, REG_PORT_ANY, 0, &reg_val));

        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE(&deskew, 0, &reg_val, 12*(group % 2), 12);
        PRT_CELL_SET("0x%X", deskew);
    }

    PRT_CELL_SET("");

    /*
    * Dump group alarms
    */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Deskew alarms");

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_DSW_ALMr(unit, &reg_val));

    /* coverity[ptr_arith:FALSE] */
    SHR_BIT_ITER(&grp_active_bmp, DNX_DIAG_FLEXE_CORE_NOF_PORTS, group)
    {
        cell[0] = '\0';
        buf[0] = '\0';

        for (ii=0; ii<4; ++ii)
        {
            /* coverity[ptr_arith:FALSE] */
            if (SHR_BITGET(&reg_val, ii*8+group))
            {
                sal_snprintf(buf, sizeof(buf), (ii==0)?"%s":"|%s", alarm_desc[ii][0]);
                sal_strncat(cell, buf, (ii==0)?1:2);
            }
        }

        /*
        * Print group alarms
        */
        PRT_CELL_SET("%s", cell);
    }

    /*
    * Print 4 rows of alarm description
    */
    for (ii=0; ii<4; ++ii)
    {
        if (ii==0)
        {
            PRT_CELL_SET("%s: %s", alarm_desc[ii][0], alarm_desc[ii][1]);
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("");

        /* coverity[ptr_arith:FALSE] */
        SHR_BIT_ITER(&grp_active_bmp, DNX_DIAG_FLEXE_CORE_NOF_PORTS, group)
        {
            PRT_CELL_SET("");
        }

        PRT_CELL_SET("%s: %s", alarm_desc[ii][0], alarm_desc[ii][1]);
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_group_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_group_man = {
    .brief = "dump flexe groups \n",
    .full = "flexe dump group\n",
    .synopsis = "",
    .examples = "" ""
};

static diag_flexe_dump_reg_row_t flexe_demux_lphy_alarms[] =
    {
        {
            "FIFO full alm",
            FSCL_FLEXEDEMUX_FIFO_FULLr,
            FLEXEDEMUX_FIFO_FULLf,
            "",
        },
        {
            "FIFO empty alm",
            FSCL_FLEXEDEMUX_FIFO_EMPTYr,
            FLEXEDEMUX_FIFO_EMPTYf,
            "",
        },
        {
            "HEAD check alm",
            FSCL_HEAD_CHECK_PHY_ALMr,
            HEAD_CHECK_PHY_ALMf,
            "",
        },
        {
            "FP check alm",
            FSCL_FP_CHECK_ALMr,
            FP_CHECK_ALMf,
            "Per instance",
        },
    };

static shr_error_e
dnx_diag_flexe_dump_demux_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint64 reorder_info;
    uint64 group_cfg;
    uint32 min_lphy = 0;
    uint32 reorder_sel = 0;
    uint32 reg_val = 0;
    uint32 val;
    int row, lphy;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE demux info");
    PRT_COLUMN_ADD("Lphys");

    for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
    {
        PRT_COLUMN_ADD(" %d ", lphy);
    }

    PRT_COLUMN_ADD("comments");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Src LPHY");

    COMPILER_64_ZERO(reorder_info);
    SHR_IF_ERR_EXIT(READ_FSCL_REORDERr(unit, &reorder_info));

    COMPILER_64_ZERO(group_cfg);
    SHR_IF_ERR_EXIT(READ_FSCL_GROUP_CFGr(unit, &group_cfg));

    for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
    {
        /* coverity[incompatible_cast:FALSE] */
        DNX_DIAG_FLEXE_DEMUX_REORDER_INFO_GET(&reorder_info, lphy, &reorder_sel);

        if (reorder_sel < DNX_DIAG_FLEXE_CORE_NOF_INSTANCES)
        {
            PRT_CELL_SET("%d.0", reorder_sel);
        }
        else if (reorder_sel < (DNX_DIAG_FLEXE_CORE_NOF_INSTANCES << 1))
        {
            PRT_CELL_SET("%d.1", reorder_sel-DNX_DIAG_FLEXE_CORE_NOF_INSTANCES);
        }
        else
        {
            PRT_CELL_SET("");
        }
    }

    PRT_CELL_SET("x.0: 1st LPHY of a instance; x.1: 2nd LPHY of a 100G instance");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Deskew LPHY");

    for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
    {
        /* coverity[incompatible_cast:FALSE] */
        DNX_DIAG_FLEXE_DEMUX_GROUP_CFG_GET(&group_cfg, lphy, &min_lphy);

        if (min_lphy < DNX_DIAG_FLEXE_CORE_NOF_LPHYS)
        {
            PRT_CELL_SET("%d", min_lphy);
        }
        else
        {
            PRT_CELL_SET("");
        }
    }

    PRT_CELL_SET("equals to the minimum LPHY in the group");

    for (row=0; row<COUNTOF(flexe_demux_lphy_alarms); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_demux_lphy_alarms[row].row_desc);

        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, flexe_demux_lphy_alarms[row].reg, REG_PORT_ANY, 0, &reg_val));
        val = soc_reg_field_get(unit, flexe_demux_lphy_alarms[row].reg, reg_val, flexe_demux_lphy_alarms[row].field);

        for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
        {
            /* coverity[ptr_arith:FALSE] */
            if (SHR_BITGET(&val, lphy))
            {
                PRT_CELL_SET("1");
            }
            else
            {
                PRT_CELL_SET("");
            }
        }

        PRT_CELL_SET("%s", flexe_demux_lphy_alarms[row].row_comments);
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_demux_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_demux_man = {
    .brief = "dump flexe demux information \n",
    .full = "flexe dump demux\n",
    .synopsis = "",
    .examples = "" ""
};

static diag_flexe_dump_reg_row_t flexe_mux_lphy_alarms[] =
    {
        {
            "FIFO full",
            FSCL_SYS_SIGNAL_FIFO_ALMr,
            SYS_SGL_FIFO_FULLf,
            "",
        },
        {
            "FIFO empty",
            FSCL_SYS_SIGNAL_FIFO_ALMr,
            SYS_SGL_FIFO_EMPTYf,
            "",
        },
    };

static shr_error_e
dnx_diag_flexe_dump_mux_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 flexe_mode;
    uint32 flexe_enable;
    uint32 cphy2_cfg = 0;
    uint32 en_sel_cfg = 0;
    uint32 inst_sel_cfg = 0;
    uint32 phy_sel_cfg = 0;
    uint32 flexe_group_cfg = 0;
    uint32 reg_val = 0;
    uint32 val = 0;
    int lphy;
    int ii, jj, row;

    char *row_desc[] = {
            "FlexE enable",
            "LPHY mode",
            "CPHY2 cfg",
            "En select",
            "Min lphy",
        };
    char *row_comments[] = {
            "LPHY is enabled for FlexE",
            "0: 50G; 1: 100G; 2: 2nd PHY of a 100G instance",
            "if mode == 2 then the 1st LPHY of a 100G instance",
            "EN source (=NB port ID)",
            "Sorted according to 0/4, 1/5, 2/6, 3/7, 1-based"
        };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE mux LPHYs");
    PRT_COLUMN_ADD("Lphys");

    for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
    {
        PRT_COLUMN_ADD("%d", lphy);
    }

    PRT_COLUMN_ADD("comments");

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_PCS_ENABLEr(unit, &reg_val));
    flexe_enable = soc_reg_field_get(unit, FSCL_FLEXE_PCS_ENABLEr, reg_val, FLEXE_ENABLEf);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MODEr(unit, &reg_val));
    flexe_mode = soc_reg_field_get(unit, FSCL_FLEXE_MODEr, reg_val, FLEXE_MODEf);

    SHR_IF_ERR_EXIT(READ_FSCL_CPHY_2_CFGr(unit, &cphy2_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_EN_SEL_CFGr(unit, &en_sel_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GROUP_CFGr(unit, &flexe_group_cfg));

    for (ii=0; ii<COUNTOF(row_desc); ++ii)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", row_desc[ii]);

        for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
        {
            val = 0;

            switch (ii)
            {
            case 0:
                /* coverity[ptr_arith:FALSE] */
                PRT_CELL_SET(SHR_BITGET(&flexe_enable, lphy)?"1":"0");
                break;
            case 1:
                /* coverity[ptr_arith:FALSE] */
                DNX_DIAG_FLEXE_MUX_MODE_GET(&flexe_mode, lphy, &val);
                PRT_CELL_SET("%d", val);
                break;
            case 2:
                /* coverity[ptr_arith:FALSE] */
                DNX_DIAG_FLEXE_MUX_MODE_GET(&flexe_mode, lphy, &val);

                if (val == DNX_DIAG_FLEXE_MUX_MODE_100G_2ND_PHY)
                {
                    val = 0;

                    for (jj=0; jj<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++jj)
                    {
                        /* coverity[ptr_arith:FALSE] */
                        DNX_DIAG_FLEXE_MUX_CPHY2_CFG_GET(&cphy2_cfg, jj, &val);

                        if (val == lphy)
                        {
                            PRT_CELL_SET("%d", jj);
                            break;
                        }
                    }

                    if (jj >= DNX_DIAG_FLEXE_CORE_NOF_LPHYS)
                    {
                        /*
                        * Failed to find the base LPHY
                        */
                        PRT_CELL_SET("E");
                    }
                }
                else
                {
                    PRT_CELL_SET("");
                }
                break;
            case 3:
                /* coverity[ptr_arith:FALSE] */
                DNX_DIAG_FLEXE_MUX_EN_SEL_CFG(&en_sel_cfg, lphy, &val);
                PRT_CELL_SET("%d", val);
                break;
            case 4:
                /* coverity[ptr_arith:FALSE] */
                DNX_DIAG_FLEXE_MUX_GROUP_CFG_GET(&flexe_group_cfg, lphy, &val);
                PRT_CELL_SET("%d", val);
                break;
            default:
                break;
            }
        }

        PRT_CELL_SET("%s", row_comments[ii]);
    }

    for (row=0; row<COUNTOF(flexe_mux_lphy_alarms); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_mux_lphy_alarms[row].row_desc);

        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, flexe_mux_lphy_alarms[row].reg, REG_PORT_ANY, 0, &reg_val));
        val = soc_reg_field_get(unit, flexe_mux_lphy_alarms[row].reg, reg_val, flexe_mux_lphy_alarms[row].field);

        for (lphy=0; lphy<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++lphy)
        {
            /* coverity[ptr_arith:FALSE] */
            if (SHR_BITGET(&val, lphy))
            {
                PRT_CELL_SET("1");
            }
            else
            {
                PRT_CELL_SET("");
            }
        }

        PRT_CELL_SET("%s", flexe_mux_lphy_alarms[row].row_comments);
    }

    PRT_COMMITX;

    cli_out("\n");

    PRT_TITLE_SET("FlexE mux instances");
    PRT_COLUMN_ADD("Instance");

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
    {
        PRT_COLUMN_ADD("%d", ii);
    }

    PRT_COLUMN_ADD("comments");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Instance sel");

    SHR_IF_ERR_EXIT(READ_FSCL_INST_SEL_CFGr(unit, &inst_sel_cfg));

    val = 0;

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        DNX_DIAG_FLEXE_MUX_INST_SEL_CFG_GET(&inst_sel_cfg, ii, &val);
        PRT_CELL_SET("%d", val);
    }

    PRT_CELL_SET("Selects base LPHY for a instance");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Minimum instance");

    SHR_IF_ERR_EXIT(READ_FSCL_PHY_SEL_CFGr(unit, &phy_sel_cfg));

    val = 0;

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        DNX_DIAG_FLEXE_PHY_SEL_CFG_GET(&phy_sel_cfg, ii, &val);
        PRT_CELL_SET("%d", val);
    }

    PRT_CELL_SET("Minimum instances in the group, 1-based");

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_mux_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_mux_man = {
    .brief = "dump flexe mux information \n",
    .full = "flexe dump mux\n",
    .synopsis = "",
    .examples = "" ""
};

static shr_error_e
dnx_diag_flexe_dump_port_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 reg_val = 0;
    uint32 full_alm = 0;
    uint32 empty_alm = 0;
    uint32 val = 0;
    uint32 is_200g;
    int ii;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE port info");
    PRT_COLUMN_ADD("NB port");

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_PORTS; ++ii)
    {
        PRT_COLUMN_ADD("%d", ii);
    }

    PRT_COLUMN_ADD("comments");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("RX map");

    SHR_IF_ERR_EXIT(READ_FSCL_RX_MAP_TABLEr(unit, &reg_val));

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        DNX_DIAG_FLEXE_RX_MAP_TABLE_GET(&reg_val, ii, (SHR_BITDCL *) & val);

        if (val != DNX_DIAG_FLEXE_INVALID_PORT_ID)
        {
            PRT_CELL_SET("%d", val);
        }
        else
        {
            PRT_CELL_SET("");
        }
    }

    PRT_CELL_SET("Local FlexE RX port");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Is 200G");

    SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
    is_200g = soc_reg_field_get(unit, FSCL_MODE_CFGr, reg_val, IS_200Gf);

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS/2; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        if (SHR_BITGET(&is_200g, ii))
        {
            PRT_CELL_SET("Y");
        }
        else
        {
            PRT_CELL_SET("N");
        }

        PRT_CELL_SET("");
    }

    PRT_CELL_SET("");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Is 400G");

    SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));

    if (soc_reg_field_get(unit, FSCL_MODE_CFGr, reg_val, IS_400Gf))
    {
        PRT_CELL_SET("Y");
    }
    else
    {
        PRT_CELL_SET("N");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("TX map");

    SHR_IF_ERR_EXIT(READ_FSCL_TX_MAP_TABLEr(unit, &reg_val));

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        DNX_DIAG_FLEXE_TX_MAP_TABLE_GET(&reg_val, ii, (SHR_BITDCL *) & val);

        if (val != DNX_DIAG_FLEXE_INVALID_PORT_ID)
        {
            PRT_CELL_SET("%d", val);
        }
        else
        {
            PRT_CELL_SET("");
        }
    }

    PRT_CELL_SET("Local FlexE TX port");

    PRT_COMMITX;

    cli_out("\n");

    PRT_TITLE_SET("Gearbox alarms");
    PRT_COLUMN_ADD("Gearbox");

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        PRT_COLUMN_ADD("%d", ii);
    }

    PRT_COLUMN_ADD("comments");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("RX alarm");

    SHR_IF_ERR_EXIT(READ_FSCL_RX_GEARBOX_ALMr(unit, &reg_val));

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        if (SHR_BITGET(&reg_val, ii) && SHR_BITGET(&reg_val, ii+8))
        {
            PRT_CELL_SET("F|E");
        }
        /* coverity[ptr_arith:FALSE] */
        else if (SHR_BITGET(&reg_val, ii))
        {
            PRT_CELL_SET("F");
        }
        /* coverity[ptr_arith:FALSE] */
        else if (SHR_BITGET(&reg_val, ii+8))
        {
            PRT_CELL_SET("E");
        }
    }

    PRT_CELL_SET("F: FIFO full; E: FIFO empty");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("TX alarm 50G");

    SHR_IF_ERR_EXIT(READ_FSCL_TX_GEARBOX_ALMr(unit, &reg_val));
    full_alm = soc_reg_field_get(unit, FSCL_TX_GEARBOX_ALMr, reg_val, FULL_GB_RAf);
    empty_alm = soc_reg_field_get(unit, FSCL_TX_GEARBOX_ALMr, reg_val, EMPTY_GB_RAf);

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        if (SHR_BITGET(&full_alm, ii) && SHR_BITGET(&empty_alm, ii))
        {
            PRT_CELL_SET("F|E");
        }
        /* coverity[ptr_arith:FALSE] */
        else if (SHR_BITGET(&reg_val, ii))
        {
            PRT_CELL_SET("F");
        }
        /* coverity[ptr_arith:FALSE] */
        else if (SHR_BITGET(&reg_val, ii))
        {
            PRT_CELL_SET("E");
        }
        else
        {
            PRT_CELL_SET("");
        }
    }

    PRT_CELL_SET("F: FIFO full; E: FIFO empty");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("TX alarm 200G");

    full_alm = soc_reg_field_get(unit, FSCL_TX_GEARBOX_ALMr, reg_val, FULL_GB_200Gf);
    empty_alm = soc_reg_field_get(unit, FSCL_TX_GEARBOX_ALMr, reg_val, EMPTY_GB_200Gf);

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS/2; ++ii)
    {
        /* coverity[ptr_arith:FALSE] */
        if (SHR_BITGET(&full_alm, ii) && SHR_BITGET(&empty_alm, ii))
        {
            PRT_CELL_SET("F|E");
        }
        /* coverity[ptr_arith:FALSE] */
        else if (SHR_BITGET(&reg_val, ii))
        {
            PRT_CELL_SET("F");
        }
        /* coverity[ptr_arith:FALSE] */
        else if (SHR_BITGET(&reg_val, ii))
        {
            PRT_CELL_SET("E");
        }
        else
        {
            PRT_CELL_SET("");
        }

        PRT_CELL_SET("");
    }

    PRT_CELL_SET("F: FIFO full; E: FIFO empty");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("TX alarm 400G");

    full_alm = soc_reg_field_get(unit, FSCL_TX_GEARBOX_ALMr, reg_val, FULL_GB_400Gf);
    empty_alm = soc_reg_field_get(unit, FSCL_TX_GEARBOX_ALMr, reg_val, EMPTY_GB_400Gf);

    if (full_alm && empty_alm)
    {
        PRT_CELL_SET("F|E");
    }
    else if (full_alm)
    {
        PRT_CELL_SET("F");
    }
    else if (empty_alm)
    {
        PRT_CELL_SET("E");
    }

    for (ii=1; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        PRT_CELL_SET("");
    }

    PRT_CELL_SET("F: FIFO full; E: FIFO empty");

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_port_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_port_man = {
    .brief = "dump flexe port map information \n",
    .full = "flexe dump port\n",
    .synopsis = "",
    .examples = "" ""
};

static diag_flexe_dump_reg_row_t flexe_oh_rows[] =
    {
        {
            "RX Mode",
            FSCL_FLEXE_RX_MODE_CFGr,
            FLEXE_RX_MODEf,
            "0: 50G; 1: 100G; 2: 200G; 3: 400G",
        },
        {
            "TX Mode",
            FSCL_FLEXE_TX_MODE_CFGr,
            FLEXE_TX_MODEf,
            "0: 50G; 1: 100G; 2: 200G; 3: 400G",
        },
        {
            "RX multi-frame mode",
            FSCL_FLEXE_RX_MODE_CFGr,
            MF_16T_32_RX_CFGf,
            "0: each MF has 16 frames; 1: each MF has 32 frames",
        },
        {
            "TX multi-frame mode",
            FSCL_FLEXE_TX_MODE_CFGr,
            MF_16T_32_TX_CFGf,
            "0: each MF has 16 frames; 1: each MF has 32 frames",
        },
        {
            "Exp CCC",
            FSCL_EXCC_OH_CFGr,
            EXCCCf,
            "Expected calendar configuration in use",
        },
        {
            "CCC inserted",
            FSCL_OH_INS_CFG_0r,
            CCC_INSf,
            "Calendar configuration in use inserted",
        },
        {
            "Actual CCC",
            FSCL_CC_RX_VALUEr,
            ACCCCf,
            "Actual calendar configuration in use received",
        },
        {
            "CR inserted",
            FSCL_OH_INS_CFG_0r,
            CR_INSf,
            "Calendar switch request inserted",
        },
        {
            "Actual CR",
            FSCL_CC_RX_VALUEr,
            ACCRf,
            "Actual calendar switch request received",
        },
        {
            "CA inserted",
            FSCL_OH_INS_CFG_0r,
            CA_INSf,
            "Calendar switch ACK inserted",
        },
        {
            "Actual CA",
            FSCL_CC_RX_VALUEr,
            ACCAf,
            "Actual calendar switch ACK received",
        },
        {
            "Exp SC",
            FSCL_EXCC_OH_CFGr,
            EXSCf,
            "Expected synchronization configuration",
        },
        {
            "Actual SC",
            FSCL_CC_RX_VALUEr,
            ACSCf,
            "Actual synchronization configuration received",
        },
        {
            "Exp GID",
            FSCL_EXGID_OH_CFGr,
            EXGIDf,
            "Expected group ID",
        },
        {
            "GID inserted",
            FSCL_OH_INS_CFG_0r,
            GID_INSf,
            "Group ID inserted",
        },
        {
            "Actual GID",
            FSCL_ID_RX_VALUEr,
            ACGIDf,
            "Actual group ID",
        },
        {
            "Exp PID",
            FSCL_EXGID_OH_CFGr,
            EXPIDf,
            "Expected PHY ID",
        },
        {
            "PID inserted",
            FSCL_OH_INS_CFG_0r,
            PID_INSf,
            "PHY ID inserted",
        },
        {
            "Actual PID",
            FSCL_ID_RX_VALUEr,
            ACPIDf,
            "Actual PHY ID",
        }
    };

static shr_error_e
dnx_diag_flexe_dump_oh_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 reg_val = 0;
    uint32 val = 0;
    int ii, row;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE overhead info");
    PRT_COLUMN_ADD("Instance");

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
    {
        PRT_COLUMN_ADD("%d", ii);
    }

    PRT_COLUMN_ADD("comments");

    for (row=0; row<COUNTOF(flexe_oh_rows); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_oh_rows[row].row_desc);

        for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
        {
            SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, flexe_oh_rows[row].reg, REG_PORT_ANY, ii, &reg_val));
            val = soc_reg_field_get(unit, flexe_oh_rows[row].reg, reg_val, flexe_oh_rows[row].field);

            PRT_CELL_SET("%d", val);
        }

        PRT_CELL_SET("%s", flexe_oh_rows[row].row_comments);
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_oh_info_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_oh_info_man = {
    .brief = "dump flexe overhead information \n",
    .full = "flexe dump oh info\n",
    .synopsis = "",
    .examples = "" ""
};

static diag_flexe_dump_reg_row_t flexe_oh_alarm_rows[] =
    {
        {
            "OOF",
            FSCL_OH_RX_ALMr,
            OOF_ALMf,
            "Out of frame",
        },
        {
            "LOF",
            FSCL_OH_RX_ALMr,
            LOF_ALMf,
            "Loss of frame",
        },
        {
            "OOM",
            FSCL_OH_RX_ALMr,
            OOM_ALMf,
            "Out of multi-frame",
        },
        {
            "LOM",
            FSCL_OH_RX_ALMr,
            LOM_ALMf,
            "Los of multi-frame",
        },
        {
            "CRC-16",
            FSCL_OH_RX_ALMr,
            CRC_ALMf,
            "CRC-16 error",
        },
        {
            "GIDM",
            FSCL_OH_RX_ALMr,
            GIDM_ALMf,
            "Group ID mismatch",
        },
        {
            "PIDM",
            FSCL_OH_RX_ALMr,
            PIDM_ALMf,
            "PHY ID mismatch",
        },
        {
            "PMM",
            FSCL_OH_RX_ALMr,
            PMM_ALMf,
            "PHYMAP mismatch",
        },
        {
            "RPF",
            FSCL_OH_RX_ALMr,
            RPF_ALMf,
            "Remote PHY fault",
        },
        {
            "SCM",
            FSCL_OH_RX_ALMr,
            SCM_ALMf,
            "Synchronization configuration mismatch",
        },
        {
            "OOP",
            FSCL_OH_RX_ALMr,
            OOP_ALMf,
            "Out of PAD",
        },
        {
            "LOP",
            FSCL_OH_RX_ALMr,
            LOP_ALMf,
            "Loss of PAD",
        },
        {
            "OH1 block",
            FSCL_OH_RX_ALMr,
            OH_1_BLOCK_ALMf,
            "OH1 block error (!= {2'b10, 8'b4B})",
        },
        {
            "C_bit",
            FSCL_OH_RX_ALMr,
            C_BIT_ALMf,
            "C-bit overhead error (3 C-bit values are not all equal)",
        },
        {
            "GID unequal",
            FSCL_OH_RX_ALMr,
            GID_UNEQ_ALMf,
            "Group instance unloaded (GID==0)",
        },
        {
            "66B code error",
            FSCL_B_66_CODE_ERRr,
            B_66_CODE_ERRf,
            "",
        },
    };

static shr_error_e
dnx_diag_flexe_dump_oh_alm_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 reg_val = 0;
    uint32 val = 0;
    int ii, row;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE overhead alarms");
    PRT_COLUMN_ADD("Instance");

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
    {
        PRT_COLUMN_ADD("%d", ii);
    }

    PRT_COLUMN_ADD("comments");

    for (row=0; row<COUNTOF(flexe_oh_alarm_rows); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_oh_alarm_rows[row].row_desc);

        for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_INSTANCES; ++ii)
        {
            SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, flexe_oh_alarm_rows[row].reg, REG_PORT_ANY, ii, &reg_val));
            val = soc_reg_field_get(unit, flexe_oh_alarm_rows[row].reg, reg_val, flexe_oh_alarm_rows[row].field);

            PRT_CELL_SET("%d", val);
        }

        PRT_CELL_SET("%s", flexe_oh_alarm_rows[row].row_comments);
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_oh_alm_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_oh_alm_man = {
    .brief = "dump flexe overhead alarms \n",
    .full = "flexe dump oh alm\n",
    .synopsis = "",
    .examples = "" ""
};

static diag_flexe_dump_mem_row_t flexe_phymap_rows[] =
    {
        {
            "PHYMAP received",
            FSCL_PHYMAP_RXm,
        },
    {
        "PHYMAP RX expected",
        FSCL_PHYMAP_RX_EXPm,
    },
    {
        "PHYMAP inserted",
        FSCL_PHYMAP_INS_CFGm,
    },
    };

static shr_error_e
dnx_diag_flexe_dump_oh_phymap_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char value_str[PRT_COLUMN_MAX_NUM];
    int pos;
    uint32 instance = 0;
    uint32 data = 0;
    int ii, index, row, col;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("INSTance", instance);

    PRT_TITLE_SET("FlexE instance %d phymap", instance);
    PRT_COLUMN_ADD("Index");

    for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        PRT_COLUMN_ADD("%d", ii);
    }

    PRT_COLUMN_ADD("Additional (if not empty then it indicates a misconfiguration)");

    for (row=0; row<COUNTOF(flexe_phymap_rows); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_phymap_rows[row].row_desc);

        col = 0;
        pos = 0;
        value_str[0] = '\0';

        for (ii=0; ii<32; ++ii)
        {
            SHR_IF_ERR_EXIT(soc_custom_mem_array_read
                            (unit, flexe_phymap_rows[row].mem, instance, FSCL_BLOCK(unit), ii, &data));

            if (data != 0)
            {
                /* coverity[ptr_arith:FALSE] */
                SHR_BIT_ITER(&data, 8, index)
                {
                    if (col < DNX_DIAG_FLEXE_CORE_NOF_LPHYS)
                    {
                        PRT_CELL_SET("%d", (ii<<3) | index);
                        ++col;
                    }
                    else
                    {
                        pos += sal_snprintf(value_str+pos, PRT_COLUMN_MAX_NUM-pos, "%d ", (ii<<3) | index);
                    }
                }
            }
        }

        PRT_CELL_SET("%s", value_str);
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_oh_phymap_options[] = {
    {"INSTance",   SAL_FIELD_TYPE_UINT32, "FlexE instance ID",       NULL, NULL, "0-7"},
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_oh_phymap_man = {
    .brief = "dump flexe oh phymap information \n",
    .full = "flexe dump oh phymap inst=<0-7>\n",
    .synopsis = "",
    .examples = "instance=0"
};

static sh_sand_man_t dnx_diag_flexe_dump_oh_man = {
    .brief = "Dump FlexE overhead information \n",
    .full = NULL
};

sh_sand_cmd_t dnx_diag_flexe_dump_oh_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"INFo",   dnx_diag_flexe_dump_oh_info_cmd,   NULL, dnx_diag_flexe_dump_oh_info_options,   &dnx_diag_flexe_dump_oh_info_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {"ALarM",  dnx_diag_flexe_dump_oh_alm_cmd,    NULL, dnx_diag_flexe_dump_oh_alm_options,    &dnx_diag_flexe_dump_oh_alm_man,  NULL, NULL, SH_CMD_NOT_RELEASE},
    {"PhyMaP", dnx_diag_flexe_dump_oh_phymap_cmd, NULL, dnx_diag_flexe_dump_oh_phymap_options, &dnx_diag_flexe_dump_oh_phymap_man, NULL, NULL, SH_CMD_NOT_RELEASE},

    {NULL}
};


static diag_flexe_dump_reg_row_t flexe_channel_alarms[] =
    {
        {
            "Decode error",
            FSCL_FLEXE_MACRX_FAULT_ALMr,
            ARM_DECODE_ERRf,
            "",
        },
        {
            "Local fault",
            FSCL_FLEXE_MACRX_FAULT_ALMr,
            ARM_LFf,
            "",
        },
        {
            "Remote fault",
            FSCL_FLEXE_MACRX_FAULT_ALMr,
            ARM_RFf,
            "",
        },
        {
            "EOP abnormal",
            FSCL_FLEXE_MACRX_EOP_ABNOR_ALMr,
            EOP_ABNOR_ALMf,
            "",
        },
    };

static shr_error_e
dnx_diag_flexe_dump_channel_alm_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 channel = 0;
    uint32 reg_val = 0;
    uint32 val;
    int row;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("CHaNnel", channel);

    PRT_TITLE_SET("FlexE channel alarms");
    PRT_COLUMN_ADD("Alarm");
    PRT_COLUMN_ADD("Value");

    for (row=0; row<COUNTOF(flexe_channel_alarms); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_channel_alarms[row].row_desc);

        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, flexe_channel_alarms[row].reg, REG_PORT_ANY, 0, &reg_val));
        val = soc_reg_field_get(unit, flexe_channel_alarms[row].reg, reg_val, flexe_channel_alarms[row].field);

        /* coverity[ptr_arith:FALSE] */
        if (SHR_BITGET(&val, channel))
        {
            PRT_CELL_SET("1");
        }
        else
        {
            PRT_CELL_SET("0");
        }
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_channel_alm_options[] = {
    {"CHaNnel",   SAL_FIELD_TYPE_UINT32, "FlexE channel ID",       NULL, NULL, "0-79"},
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_channel_alm_man = {
    .brief = "dump flexe channel alarms \n",
    .full = "flexe dump chn alm chn=<0-79>\n",
    .synopsis = "",
    .examples = "channel=0"
};

static diag_flexe_dump_mem_row_t flexe_counter_rows[] =
    {
        {
            "Decode err",
            FSCL_FLEXE_MACRX_DECODE_ERR_CODEm,
            0
        },
        {
            "Total bytes",
            FSCL_FLEXE_MACRX_TOTAL_BYTESm,
            0
        },
        {
            "Good bytes",
            FSCL_FLEXE_MACRX_GOOD_BYTESm,
            0
        },
        {
            "Bad bytes",
            FSCL_FLEXE_MACRX_BAD_BYTESm,
            0
        },
        {
            "Fragment bytes",
            FSCL_FLEXE_MACRX_FRAGMENT_BYTESm,
            0
        },
        {
            "Jabber bytes",
            FSCL_FLEXE_MACRX_JABBER_BYTESm,
            0
        },
        {
            "Oversize bytes",
            FSCL_FLEXE_MACRX_OVERSIZE_BYTESm,
            0
        },
        {
            "Unicats bytes",
            FSCL_FLEXE_MACRX_UNICAST_BYTESm,
            0
        },
        {
            "Multicats bytes",
            FSCL_FLEXE_MACRX_MULTICAST_BYTESm,
            0
        },
        {
            "Broadcats bytes",
            FSCL_FLEXE_MACRX_BROADCAST_BYTESm,
            0
        },
        {
            "64B bytes",
            FSCL_FLEXE_MACRX_B_64_BYTESm,
            0
        },
        {
            "65~127 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            0
        },
        {
            "128~255 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            80
        },
        {
            "256~511 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            160
        },
        {
            "512~1023 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            240
        },
        {
            "1024~1518 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            320
        },
        {
            "1519~2047 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            400
        },
        {
            "2048~4095 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            480
        },
        {
            "4096~9216 bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            560
        },
        {
            "9217~mtu bytes",
            FSCL_FLEXE_MACRX_B_65_MPL_BYTESm,
            640
        },
        {
            "Total frames",
            FSCL_FLEXE_MACRX_TOTAL_FRAMEm,
            0
        },
        {
            "Good frames",
            FSCL_FLEXE_MACRX_GOOD_FRAMEm,
            0
        },
        {
            "Bad frames",
            FSCL_FLEXE_MACRX_BAD_FRAMEm,
            0
        },
        {
            "Fragment frames",
            FSCL_FLEXE_MACRX_FRAGMENT_FRAMEm,
            0
        },
        {
            "Jabber frames",
            FSCL_FLEXE_MACRX_JABBER_FRAMEm,
            0
        },
        {
            "Oversize frames",
            FSCL_FLEXE_MACRX_OVERSIZE_FRAMEm,
            0
        },
        {
            "Unicast frames",
            FSCL_FLEXE_MACRX_UNICAST_FRAMEm,
            0
        },
        {
            "Multicast frames",
            FSCL_FLEXE_MACRX_MULTICAST_FRAMEm,
            0
        },
        {
            "Broadcast frames",
            FSCL_FLEXE_MACRX_BROADCAST_FRAMEm,
            0
        },
        {
            "64 bytes frames",
            FSCL_FLEXE_MACRX_B_64_FRAMEm,
            0
        },
        {
            "65~127 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            0
        },
        {
            "128~255 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            80
        },
        {
            "256~511 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            160
        },
        {
            "512~1023 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            240
        },
        {
            "1024~1518 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            320
        },
        {
            "1519~2047 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            400
        },
        {
            "2048~4095 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            480
        },
        {
            "4096~9216 bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            560
        },
        {
            "9217~mtu bytes frames",
            FSCL_FLEXE_MACRX_B_65_MPL_FRAMEm,
            640
        },
    };

static shr_error_e
dnx_diag_flexe_dump_channel_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char value_str[PRT_COLUMN_MAX_NUM];
    uint32 data[2] = { 0, 0 };
    uint32 channel = 0;
    int row;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("CHaNnel", channel);

    PRT_TITLE_SET("FlexE channel statistics");
    PRT_COLUMN_ADD("Counter");
    PRT_COLUMN_ADD("Value");

    for (row=0; row<COUNTOF(flexe_counter_rows); ++row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", flexe_counter_rows[row].row_desc);

        SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, flexe_counter_rows[row].mem, 0, FSCL_BLOCK(unit), channel, &data));
        format_value_to_str(data, 64, 0, value_str);

        PRT_CELL_SET("%s", value_str);
    }

    PRT_COMMITX;

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_channel_counter_options[] = {
    {"CHaNnel",   SAL_FIELD_TYPE_UINT32, "FlexE channel ID",       NULL, NULL, "0-79"},
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_channel_counter_man = {
    .brief = "dump flexe channel counters \n",
    .full = "flexe dump chn cou chn=<0-79>\n",
    .synopsis = "",
    .examples = "channel=0"
};

static shr_error_e
dnx_diag_flexe_dump_channel_demux_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS)];
    soc_mem_t wcfgram_mem, rdrule_mem, chproperty_mem;
    soc_reg_above_64_val_t calendar_cfg;
    soc_reg_above_64_val_t own_tsgroup;
    soc_reg_above_64_val_t rdrule;
    soc_reg_above_64_val_t read_ts;
    uint32 channel_own_tsgroup = 0;
    uint32 chproperty = 0;
    uint32 timeslots[DNX_DIAG_FLEXE_CORE_NOF_SUBCALS] = { 0 };
    int ram_addr[DNX_DIAG_FLEXE_CORE_NOF_SUBCALS];
    uint32 rownum;
    uint32 channel_1_based;
    uint32 channel;
    uint32 timeslot_plus_1;
    uint32 val = 0;
    int blk, cycle, tsnum;
    int blk_cnt, cycle_cnt;
    int ii, col, subcal, timeslot;
    int calendar_sel = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("CHaNnel", channel);

    channel_1_based = channel + 1;

    for (calendar_sel=bcmPortFlexeGroupCalA; calendar_sel<bcmPortFlexeGroupCalCount; ++calendar_sel)
    {
        cli_out("Calendar %d:\n\n", calendar_sel);

        SOC_REG_ABOVE_64_CLEAR(calendar_cfg);
        SOC_REG_ABOVE_64_CLEAR(own_tsgroup);
        SOC_REG_ABOVE_64_CLEAR(read_ts);

        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CALENDAR_CFGr, READ_FSCL_CALENDAR_CFG_Br, unit, calendar_cfg);
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CHANNEL_OWN_TSGROUPr, READ_FSCL_CHANNEL_OWN_TSGROUP_Br, unit, own_tsgroup);
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_READY_TSr, READ_FSCL_READY_TS_Br, unit, read_ts);

        SHR_BITCLR_RANGE(tsmap, 0, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS);

        tsnum = 0;

        for(ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS; ++ii)
        {
            /*
             * Configure calendar, map timeslot to channel
             */
            DNX_DIAG_FLEXE_DEMUX_CALENDAR_CFG_GET(calendar_cfg, ii, &val);

            if (val == channel_1_based)
            {
                SHR_BITSET(tsmap, ii);
                ++tsnum;
            }
        }

        DNX_DIAG_FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_GET(own_tsgroup, channel, &channel_own_tsgroup);

        PRT_TITLE_SET("FlexE channel timeslot information, channel own tsgroup 0x%2X", channel_own_tsgroup);
        PRT_COLUMN_ADD("Index");

        for (ii=0; ii<tsnum; ++ii)
        {
            PRT_COLUMN_ADD("%d", ii);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("Timeslot");

        SHR_BIT_ITER(tsmap, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS, ii)
        {
            PRT_CELL_SET("%d", ii);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("TS ready");

        /*
         * Read timeslot en
         */
        SOC_REG_ABOVE_64_CLEAR(read_ts);
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_READY_TSr, READ_FSCL_READY_TS_Br, unit, read_ts);

        SHR_BIT_ITER(tsmap, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS, ii)
        {
            if (SHR_BITGET(read_ts, ii))
            {
                PRT_CELL_SET("1");
            }
            else
            {
                PRT_CELL_SET("");
            }
        }

        PRT_COMMITX;

        cli_out("\n");

        chproperty_mem = (calendar_sel == bcmPortFlexeGroupCalA) ? FSCL_CHPROPERTYm : FSCL_CHPROPERTY_Bm;

        SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, chproperty_mem, 0, FSCL_BLOCK(unit), channel, &chproperty));

        tsnum = soc_mem_field32_get(unit, FSCL_CHPROPERTYm, &chproperty, TSNUMf);
        blk_cnt = soc_mem_field32_get(unit, FSCL_CHPROPERTYm, &chproperty, BLKf);;
        cycle_cnt = soc_mem_field32_get(unit, FSCL_CHPROPERTYm, &chproperty, CYCLEf);

        if ((tsnum == 0)
            || (tsnum >= DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS)
            || (blk_cnt >= DNX_DIAG_FLEXE_CORE_NOF_SUBCALS)
            || (cycle_cnt >= DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS))
        {
            continue;
        }

        PRT_TITLE_SET("FlexE channel demux information");
        PRT_COLUMN_ADD("Sub calendars");

        for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++ii)
        {
            PRT_COLUMN_ADD("%d", ii);
        }

        PRT_COLUMN_ADD("comments");

        timeslot = 0;
        timeslot_plus_1 = 0;
        subcal = 0;
        wcfgram_mem = (calendar_sel == bcmPortFlexeGroupCalA) ? FSCL_WCFGRAMm : FSCL_WCFGRAM_Bm;
        rdrule_mem = (calendar_sel == bcmPortFlexeGroupCalA) ? FSCL_RDRULEm : FSCL_RDRULE_Bm;

        for (blk = 0; blk <= blk_cnt; ++blk)
        {
            for (cycle = 0; cycle <= cycle_cnt; ++cycle)
            {
                /*
                 * Row 0: timeslots
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                if (cycle == 0)
                {
                    PRT_CELL_SET("blk%d cycle%d", blk, cycle);
                }
                else
                {
                    PRT_CELL_SET("    cycle%d", cycle);
                }

                SOC_REG_ABOVE_64_CLEAR(rdrule);

                SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit,
                                                           rdrule_mem,
                                                           0,
                                                           FSCL_BLOCK(unit),
                                                           channel * DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS + blk * (cycle_cnt+1) + cycle,
                                                           rdrule));

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    rownum = (blk + col) % DNX_DIAG_FLEXE_CORE_NOF_SUBCALS;

                    /* coverity[ptr_arith:FALSE] */
                    DNX_DIAG_FLEXE_DEMUX_RDRULE_GET(rdrule, rownum, &timeslot_plus_1);

                    /*
                    * timeslot in memory is 1-based
                    */
                    timeslots[col] = timeslot_plus_1 - 1;

                    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit,
                                                               wcfgram_mem,
                                                               DNX_DIAG_FLEXE_DEMUX_TS_TO_WR_TBL_ID(timeslots[col]),
                                                               FSCL_BLOCK(unit),
                                                               DNX_DIAG_FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(timeslots[col]) +
                                                               subcal, &rownum));

                    /*
                    * RAM addr in memory is 1-based
                    */
                    ram_addr[col] = rownum - 1;

                    if ((timeslot % tsnum) == 0)
                    {
                        ++subcal;
                    }
                }

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    PRT_CELL_SET("%d", timeslots[col]);
                }

                PRT_CELL_SET("timeslot");

                /*
                 * Row 1: RD rule
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("");

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    rownum = (blk + col) % DNX_DIAG_FLEXE_CORE_NOF_SUBCALS;
                    PRT_CELL_SET("%d", rownum);
                }

                PRT_CELL_SET("Read RAM addr");

                /*
                 * Row 2: WR rule
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("");

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    PRT_CELL_SET("%d", ram_addr[col]);
                }

                PRT_CELL_SET("Write RAM addr");
            }
        }

        PRT_COMMITX;
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_channel_demux_options[] = {
    {"CHaNnel",   SAL_FIELD_TYPE_UINT32, "FlexE channel ID",       NULL, NULL, "0-79"},
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_channel_demux_man = {
    .brief = "dump flexe channel demux information \n",
    .full = "flexe dump chn dmx chn=<0-79>\n",
    .synopsis = "",
    .examples = "channel=0"
};

/**
 * \brief
 *   Converts logical timeslot into hardware timeslot or verse versa
 */
static int
dnx_diag_flexe_mux_timeslot_convert(
    int timeslot,
    int is_100g)
{
    int timeslot_mod_20 = timeslot % 20;
    int hw_timeslot = timeslot;

    if (is_100g)
    {
        /*
         * for 100G phy, the timeslots are arranged as 0~4/10~14/5~9/15~19
         */
        if ((timeslot_mod_20 >= 5) && timeslot_mod_20 < 10)
        {
            /*
            * 5~9, return ts+5
            */
            hw_timeslot = timeslot + 5;
        }
        else if ((timeslot_mod_20 >= 10) && timeslot_mod_20 < 15)
        {
            /*
            * 10~14, return ts-5
            */
            hw_timeslot = timeslot - 5;
        }
    }

    return hw_timeslot;
}

static shr_error_e
dnx_diag_flexe_dump_channel_mux_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_BITDCL hw_tsmap_msi[_SHR_BITDCLSIZE(DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS)];
    SHR_BITDCL tsmap_msi[_SHR_BITDCLSIZE(DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS)];
    uint32 timeslots[DNX_DIAG_FLEXE_CORE_NOF_SUBCALS];
    int ram_addr[DNX_DIAG_FLEXE_CORE_NOF_SUBCALS];
    soc_reg_above_64_val_t reg_val_above_64;
    soc_reg_above_64_val_t wr_cfg;
    soc_mem_t wrcfg_mem, rdcfg_mem;
    uint32 reg_val = 0;
    uint32 msi_cfg = 0;
    uint32 flexe_mode;
    uint32 mux_mode = 0;
    uint32 lphy = 0;
    uint32 channel;
    uint32 val = 0;
    int tsnum = 0;
    int calendar_sel = 0;
    int blk_cnt = 0, cycle_cnt = 0;
    int blk, cycle, rownum;
    int ii, ts, col, subcal, is_100g = 1;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("CHaNnel", channel);

    for (calendar_sel=bcmPortFlexeGroupCalA; calendar_sel<bcmPortFlexeGroupCalCount; ++calendar_sel)
    {
        cli_out("Calendar %d:\n\n", calendar_sel);

        SHR_BITCLR_RANGE(hw_tsmap_msi, 0, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS);
        SHR_BITCLR_RANGE(tsmap_msi, 0, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS);

        for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS; ++ii)
        {
            /*
             * MSI cfg: the channel that takes current timeslot
             */
            DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_FLEXE_MSI_CFGr, READ_FSCL_FLEXE_MSI_CFG_Br, unit, ii % 20,
                            &msi_cfg);
            DNX_DIAG_FLEXE_MUX_MSI_CFG_GET(&msi_cfg, ii, (SHR_BITDCL *) & val);

            if (val == channel)
            {
                SHR_BITSET(hw_tsmap_msi, ii);
                ++tsnum;
            }
        }

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MODEr(unit, &reg_val));
        flexe_mode = soc_reg_field_get(unit, FSCL_FLEXE_MODEr, reg_val, FLEXE_MODEf);

        SHR_BIT_ITER(hw_tsmap_msi, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS, ts)
        {
            /*
             * Get MUX mode of LPHY (can be any LPHY in the group)
             */
            lphy = ts / 10;
            DNX_DIAG_FLEXE_MUX_MODE_GET(&flexe_mode, lphy, &mux_mode);
            break;
        }

        is_100g = (mux_mode == DNX_DIAG_FLEXE_MUX_MODE_50G) ? 0 : 1;

        SHR_BIT_ITER(hw_tsmap_msi, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS, ii)
        {
            ts = dnx_diag_flexe_mux_timeslot_convert(ii, is_100g);
            SHR_BITSET(tsmap_msi, ts);
        }

        /*
         * Read minimum LPHY of the channel
         */
        SHR_IF_ERR_EXIT(READ_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));
        lphy = 0;
        DNX_DIAG_FLEXE_MUX_CHANNEL_BELONG_FLEXE_GET(reg_val_above_64, channel, &lphy);

        PRT_TITLE_SET("FlexE channel timeslot information, min LPHY = %d", lphy);
        PRT_COLUMN_ADD("Index");

        for (ii=0; ii<tsnum; ++ii)
        {
            PRT_COLUMN_ADD("%d", ii);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("Timeslot");

        SHR_BIT_ITER(tsmap_msi, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS, ts)
        {
            PRT_CELL_SET("%d", ts);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("Enable");

        /*
         * Read timeslot en
         */
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_TS_ENABLEr, READ_FSCL_TS_ENABLE_Br, unit, reg_val_above_64);

        SHR_BIT_ITER(tsmap_msi, DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS, ii)
        {
            ts = dnx_diag_flexe_mux_timeslot_convert(ii, is_100g);

            if (SHR_BITGET(reg_val_above_64, ts))
            {
                PRT_CELL_SET("1");
            }
            else
            {
                PRT_CELL_SET("");
            }
        }

        PRT_COMMITX;

        val = 0;
        /*
         * Read timeslot number
         */
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_TSNUM_CFGr, READ_FSCL_TSNUM_CFG_Br, unit, channel % 20, &reg_val);
        DNX_DIAG_FLEXE_MUX_TSNUM_CFG_GET(&reg_val, channel, (SHR_BITDCL *) & val);

        if (val != tsnum)
        {
            cli_out("Timeslot number mismatch, msi: %d, cfg: %d\n", tsnum, val);
            SHR_EXIT();
        }

        /*
         * Read cycle count
         */
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CYCLE_CFGr, READ_FSCL_CYCLE_CFG_Br, unit, channel % 20, &reg_val);
        DNX_DIAG_FLEXE_MUX_CYCLE_CFG_GET(&reg_val, channel, (SHR_BITDCL *) & cycle_cnt);

        /*
         * Read block count
         */
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        DNX_DIAG_FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_BLOCK_CFGr, READ_FSCL_BLOCK_CFG_Br, unit, reg_val_above_64);
        DNX_DIAG_FLEXE_MUX_BLK_CFG_GET(reg_val_above_64, channel, (SHR_BITDCL *) & blk_cnt);

        if ((tsnum == 0)
            || (tsnum >= DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS)
            || (blk_cnt >= DNX_DIAG_FLEXE_CORE_NOF_SUBCALS)
            || (cycle_cnt >= DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS))
        {
            continue;
        }

        cli_out("\n");

        PRT_TITLE_SET("FlexE channel mux information");
        PRT_COLUMN_ADD("Sub calendars");

        for (ii=0; ii<DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++ii)
        {
            PRT_COLUMN_ADD("%d", ii);
        }

        PRT_COLUMN_ADD("comments");

        rdcfg_mem = (calendar_sel == bcmPortFlexeGroupCalA) ? FSCL_RD_CFGm : FSCL_RD_CFG_Bm;
        wrcfg_mem = (calendar_sel == bcmPortFlexeGroupCalA) ? FSCL_WR_CFGm : FSCL_WR_CFG_Bm;

        subcal = 0;
        ts = 0;

        sal_memset(timeslots, 0, sizeof(timeslots));
        sal_memset(ram_addr, 0, sizeof(ram_addr));

        /*
         * Read WR cfg/RD cfg
         */
        for (blk = 0; blk <= blk_cnt; ++blk)
        {
            for (cycle = 0; cycle <= cycle_cnt; ++cycle)
            {
                /*
                 * Row 0: timeslots
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                if (cycle == 0)
                {
                    PRT_CELL_SET("blk%d cycle%d", blk, cycle);
                }
                else
                {
                    PRT_CELL_SET("    cycle%d", cycle);
                }

                SOC_REG_ABOVE_64_CLEAR(wr_cfg);

                SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit,
                                                           wrcfg_mem,
                                                           0,
                                                           FSCL_BLOCK(unit),
                                                           channel * DNX_DIAG_FLEXE_CORE_NOF_TIMESLOTS + blk * (cycle_cnt + 1) +
                                                           cycle, &wr_cfg));

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    rownum = (blk + col) % DNX_DIAG_FLEXE_CORE_NOF_SUBCALS;

                    DNX_DIAG_FLEXE_DEMUX_RDRULE_GET(wr_cfg, rownum, &timeslots[col]);

                    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit,
                                                               rdcfg_mem,
                                                               DNX_DIAG_FLEXE_DEMUX_TS_TO_WR_TBL_ID(timeslots[col]),
                                                               FSCL_BLOCK(unit),
                                                               DNX_DIAG_FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(timeslots[col]) +
                                                               subcal, &ram_addr[col]));

                    ++ts;

                    if ((ts % tsnum) == 0)
                    {
                        ++subcal;
                    }
                }

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    PRT_CELL_SET("%d", dnx_diag_flexe_mux_timeslot_convert(timeslots[col], is_100g));
                }

                PRT_CELL_SET("timeslot");

                /*
                 * Row 1: RD rule
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("");

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    rownum = (blk + col) % DNX_DIAG_FLEXE_CORE_NOF_SUBCALS;
                    PRT_CELL_SET("%d", rownum);
                }

                PRT_CELL_SET("Read RAM addr");

                /*
                 * Row 2: WR rule
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("");

                for (col = 0; col < DNX_DIAG_FLEXE_CORE_NOF_SUBCALS; ++col)
                {
                    PRT_CELL_SET("%d", ram_addr[col]);
                }

                PRT_CELL_SET("Write RAM addr");
            }
        }

        PRT_COMMITX;
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_channel_mux_options[] = {
    {"CHaNnel",   SAL_FIELD_TYPE_UINT32, "FlexE channel ID",       NULL, NULL, "0-79"},
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_channel_mux_man = {
    .brief = "dump flexe channel mux information \n",
    .full = "flexe dump chn mux chn=<0-79>\n",
    .synopsis = "",
    .examples = "channel=0"
};

static sh_sand_man_t dnx_diag_flexe_dump_channel_man = {
    .brief = "Dump FlexE channel information \n",
    .full = NULL
};

sh_sand_cmd_t dnx_diag_flexe_dump_channel_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"ALarM",    dnx_diag_flexe_dump_channel_alm_cmd,      NULL, dnx_diag_flexe_dump_channel_alm_options,     &dnx_diag_flexe_dump_channel_alm_man,     NULL, NULL, SH_CMD_NOT_RELEASE},
    {"COUnter",  dnx_diag_flexe_dump_channel_counter_cmd,  NULL, dnx_diag_flexe_dump_channel_counter_options, &dnx_diag_flexe_dump_channel_counter_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DeMuX",    dnx_diag_flexe_dump_channel_demux_cmd,    NULL, dnx_diag_flexe_dump_channel_demux_options,   &dnx_diag_flexe_dump_channel_demux_man,   NULL, NULL, SH_CMD_NOT_RELEASE},
    {"MUX",      dnx_diag_flexe_dump_channel_mux_cmd,      NULL, dnx_diag_flexe_dump_channel_mux_options,     &dnx_diag_flexe_dump_channel_mux_man,     NULL, NULL, SH_CMD_NOT_RELEASE},

    {NULL}
};

static shr_error_e
dnx_diag_flexe_dump_sw_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 data = 0;
    uint32 val = 0;
    int ii, jj, table, row;

    char *titles[] =
        {
            "FlexE channel destination",
            "SAR channel destination",
            "TinyMAC channel destination",
        };
    int nof_channels[] = {DNX_DIAG_FLEXE_CORE_NOF_CHANNELS, DNX_DIAG_FLEXE_SAR_NOF_TIMESLOTS, DNX_DIAG_FLEXE_CORE_MAX_NOF_CHANNELS};
    int channel_end[] =
        {
            DNX_DIAG_FLEXE_CORE_NOF_CHANNELS,
            DNX_DIAG_FLEXE_CORE_NOF_CHANNELS+DNX_DIAG_FLEXE_SAR_NOF_TIMESLOTS,
            DNX_DIAG_FLEXE_CORE_NOF_CHANNELS+DNX_DIAG_FLEXE_SAR_NOF_TIMESLOTS+DNX_DIAG_FLEXE_CORE_MAX_NOF_CHANNELS
        };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    for (table=0; table<COUNTOF(titles); ++table)
    {
        PRT_TITLE_SET("%s", titles[table]);
        PRT_COLUMN_ADD("Channel");
        PRT_COLUMN_ADD("FlexE dest");
        PRT_COLUMN_ADD("SAR dest");
        PRT_COLUMN_ADD("TinyMAC dest");

        for (row=0; row<nof_channels[table]; ++row)
        {
            SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, FSCL_SWRAMm, table, FSCL_BLOCK(unit), row, &data));

            if (data != 0xFFFFFF)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("%d", row);

                for (ii=0; ii<DNX_DIAG_FLEXE_66BSWITCH_NOF_SUBSYS; ++ii)
                {
                    for (jj=0; jj<DNX_DIAG_FLEXE_66BSWITCH_NOF_SUBSYS; ++jj)
                    {
                        /* coverity[ptr_arith:FALSE] */
                        SHR_BITCOPY_RANGE(&val, 0, &data, 8 * jj, 8);

                        if (((ii == 0) && (val < channel_end[0]))
                            || ((ii > 0 ) && (val >= channel_end[ii-1]) && (val < channel_end[ii])))
                        {
                            PRT_CELL_SET("%d", (ii>0)? (val - channel_end[ii-1]) : val);
                            break;
                        }
                    }

                    if (jj >= DNX_DIAG_FLEXE_66BSWITCH_NOF_SUBSYS)
                    {
                        PRT_CELL_SET("");
                    }
                }
            }
        }

        PRT_COMMITX;
        cli_out("\n");
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_diag_flexe_dump_sw_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_diag_flexe_dump_sw_man = {
    .brief = "dump flexe switch information \n",
    .full = "flexe dump sw\n",
    .synopsis = "",
    .examples = "" ""
};

sh_sand_cmd_t dnx_diag_flexe_dump_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage
     */
    {"GRouP",    dnx_diag_flexe_dump_group_cmd,      NULL,                              dnx_diag_flexe_dump_group_options,     &dnx_diag_flexe_dump_group_man,   NULL, NULL, SH_CMD_NOT_RELEASE},
    {"DeMuX",    dnx_diag_flexe_dump_demux_cmd,      NULL,                              dnx_diag_flexe_dump_demux_options,     &dnx_diag_flexe_dump_demux_man,   NULL, NULL, SH_CMD_NOT_RELEASE},
    {"MUX",      dnx_diag_flexe_dump_mux_cmd,        NULL,                              dnx_diag_flexe_dump_mux_options,       &dnx_diag_flexe_dump_mux_man,     NULL, NULL, SH_CMD_NOT_RELEASE},
    {"Port",     dnx_diag_flexe_dump_port_cmd,       NULL,                              dnx_diag_flexe_dump_port_options,      &dnx_diag_flexe_dump_port_man,    NULL, NULL, SH_CMD_NOT_RELEASE},
    {"OverHead", NULL,                               dnx_diag_flexe_dump_oh_cmds,       NULL,                                  &dnx_diag_flexe_dump_oh_man,      NULL, NULL, SH_CMD_NOT_RELEASE},
    {"CHaNnel",  NULL,                               dnx_diag_flexe_dump_channel_cmds,  NULL,                                  &dnx_diag_flexe_dump_channel_man, NULL, NULL, SH_CMD_NOT_RELEASE},
    {"SW",       dnx_diag_flexe_dump_sw_cmd,         NULL,                              dnx_diag_flexe_dump_sw_options,        &dnx_diag_flexe_dump_sw_man,      NULL, NULL, SH_CMD_NOT_RELEASE},

    {NULL}
};

static sh_sand_man_t  dnx_diag_flexe_dump_man = {
    .brief = "Dump FlexE driver information \n",
    .full = NULL
};

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
sh_sand_man_t sh_dnx_diag_flexe_man = {
    .brief = "FLEXE commands",
    .full = "The dispatcher for the different FLEXE diagnostics commands\n"
};

/**
 * \brief DNX FlexE diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for flexe diagnostic commands
 */
sh_sand_cmd_t sh_dnx_diag_flexe_cmds[] = {
    /*
     * Name |    Leaf Action | Junction Array | Options for Leaf | Usage | Options call back for Leaf | Invoke | flags
     */
    {"INFo",     dnx_diag_flexe_info_cmd,             NULL,                         dnx_diag_flexe_info_options,    &dnx_diag_flexe_info_man,     NULL, NULL, SH_CMD_NOT_RELEASE},
    {"counter",  dnx_diag_flexe_counter_cmd,          NULL,                         dnx_diag_flexe_counter_options, &dnx_diag_flexe_counter_man,  NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_enable},
    {"GRouP",    NULL,                                dnx_diag_flexe_group_cmds,    NULL,                           &dnx_diag_flexe_group_man,    NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_enable},
    {"CLIENT",   NULL,                                dnx_diag_flexe_client_cmds,   NULL,                           &dnx_diag_flexe_client_man,   NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_enable},
    {"FLOW",     NULL,                                dnx_diag_flexe_flow_cmds,     NULL,                           &dnx_diag_flexe_flow_man,     NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_enable},
    {"OverHead", NULL,                                dnx_diag_flexe_overhead_cmds, NULL,                           &dnx_diag_flexe_overhead_man, NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_enable},
    {"CORE",     NULL,                                dnx_diag_flexe_core_cmds,     NULL,                           &dnx_diag_flexe_core_man,     NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_core_available},
    {"DuMP",     NULL,                                dnx_diag_flexe_dump_cmds,     NULL,                           &dnx_diag_flexe_dump_man,     NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NOT_RELEASE, sh_cmd_is_flexe_enable},

    {NULL}
};

