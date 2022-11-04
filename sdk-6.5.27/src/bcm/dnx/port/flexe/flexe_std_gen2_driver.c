/** \file flexe_std_gen2_driver.c
 *
 * Functions for FlexE gen2 adapter layer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_FLEXE

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/thread.h>
#include <sal/core/boot.h>
#include <soc/error.h>

#include <shared/util.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_flexe_core_access.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/port/flexe/flexe_std_gen2.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/port/port_framer_reg_access.h>
#include <shared/pbmp.h>

#include "flexe_common.h"
/*
 * Framer
 */
#ifndef NO_FRAMER_LIB_BUILD
#include <chips.h>
#include <framer_sal_j2x_flexe_mux1_core.h>
#include <framer_sal_j2x_subsys_map_otn_adapter.h>
#include <framer_sal_j2x_subsys_bp_adapter.h>
#include <framer_sal_j2x_subsys_flexe_adapter.h>
#include <framer_sal_j2x_flexe_oam.h>
#include <framer_sal_j2x_ieee1588.h>
#include <framer_sal_j2x_env_mac2.h>
#include <framer_sal_j2x_ohif_flexe.h>
#endif
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define FLEXE_STD_GEN2_IF_ERR_EXIT(expr)                   \
    do                                                     \
    {                                                      \
        int rv = expr;                                     \
        int err_translate[] = {_SHR_E_NONE,                \
                               _SHR_E_FAIL,                \
                               _SHR_E_PARAM,               \
                               _SHR_E_MEMORY,              \
                               _SHR_E_INIT,                \
                               _SHR_E_MEMORY,              \
                               _SHR_E_INIT,                \
                               _SHR_E_UNAVAIL,             \
                               _SHR_E_UNAVAIL,             \
                               _SHR_E_PARAM};              \
        SHR_IF_ERR_EXIT(err_translate[rv]);                \
    }                                                      \
    while(0)
#define FLEXE_STD_GEN2_BLOCK_IS_BUSB(unit, block_id)        \
    ((block_id == 9) || (block_id == 26))

#define FLEXE_STD_GEN2_MAX_NOF_SLOTS_PER_INSTANCE                    (100)
#define FLEXE_STD_GEN2_FLEXE_PHYMAP_OCTETS                           (32)
#define FLEXE_STD_GEN2_VIRTUAL_FLEXE_MODULE_ID                       (1)

#define FLEXE_STD_GEN2_INIT_DEVICE_SLOT_MODE_SHIFT                   (3)
#define FLEXE_STD_GEN2_INIT_TYPE_SHIFT                               (28)

#define FLEXE_STD_GEN2_OAM_TYPE_SHIFT                                (8)
#define FLEXE_STD_GEN2_OAM_DEF_PERIOD                                (3)

#define FLEXE_STD_GEN2_BUSA_PTP_PORT                                 (336)
#define FLEXE_STD_GEN2_BUSA_OH_OAM_PORT                              (337)

#define FLEXE_STD_GEN2_CCU_BASE_VLAN                                 (11)
#define FLEXE_STD_GEN2_MAX_BLOCK_NUM                                 (0x7FFFFFF)

#define MACRX_1588_VLAN_FILTER_DISABLE                               (7)
#define MACRX_1588_LAYER_FILTER_DISABLE                              (7)
#define MACRX_SSM_VLAN_FILTER_DISABLE                                (7)
#define MACRX_PARSE_UNICAST_DA_ENABLE                                (7)

#define NOF_CLIENT_ID_RAMS_FOR_1G_SLOT                               (5)
#define NOF_CLIENT_ID_RAMS_FOR_1P25G_SLOT                            (4)
#define NOF_CLIENT_ID_RAMS_FOR_5G_SLOT                               (1)

#define MAC_ENV_SRC_CLK_FROM_FLEXE1                                  (1)

#define FLEXE_OAM_REQ_CALENDAR_GRANULARITY                           (250)
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct flexe_std_gen2_alarm_map_s
{
    int field_id;
    uint32 flags;
} flexe_std_gen2_alarm_map_t;

typedef enum flexe_std_gen2_j2x_init_mode_s
{
    J2X_INIT_COMPONENT = 0,
    J2X_INIT_ALL,
    J2X_INIT_WITHOUT_RAM
} flexe_std_gen2_j2x_init_mode_t;

typedef enum flexe_std_gen2_sch_cal_control_type_e
{
    CAL_CTRL_BY_C_BIT = 0,
    CAL_CTRL_BY_REGISTER
} flexe_std_gen2_sch_cal_control_type_t;

/* } */

#ifndef NO_FRAMER_LIB_BUILD

/*************
 * GLOBALS   *
 *************/
/* { */
const flexe_std_gen2_alarm_map_t oh_alarm_field_to_flags_map_table[] = {
    {flexe1_rx_oh_lc_GIDM_ALM, FLEXE_CORE_OH_ALARM_GID_MISMATCH},
    {flexe1_rx_oh_lc_PIDM_ALM, FLEXE_CORE_OH_ALARM_PHY_NUM_MISMATCH},
    {flexe1_rx_oh_lc_LOF_ALM, FLEXE_CORE_OH_ALARM_LOF},
    {flexe1_rx_oh_lc_LOM_ALM, FLEXE_CORE_OH_ALARM_LOM},
    {flexe1_rx_oh_lc_RPF_ALM, FLEXE_CORE_OH_ALARM_RPF},
    {flexe1_rx_oh_lc_OH1_BLOCK_ALM, FLEXE_CORE_OH_ALARM_OH1_ALARM},
    {flexe1_rx_oh_lc_C_BIT_ALM, FLEXE_CORE_OH_ALARM_C_BIT_ALARM},
    {flexe1_rx_oh_lc_PMM_ALM, FLEXE_CORE_OH_ALARM_PHY_MAP_MISMATCH},
    {flexe1_rx_oh_lc_CRC_ALM, FLEXE_CORE_OH_ALARM_CRC},
    {flexe1_rx_oh_lc_SCM_ALM, FLEXE_CORE_OH_ALARM_SC_MISMATCH},
    {flexe1_rx_oh_lc_GID_UNEQ_ALM, FLEXE_CORE_OH_ALARM_GID_UNEQUIPPED}
};
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*initialize FlexE framer access register.*/
static shr_error_e
flexe_std_gen2_access_init(
    int unit)
{
    FRAMER_UAS_DRV_T *framer_uas_drv = NULL;
    FRAMER_RET rt;

    SHR_FUNC_INIT_VARS(unit);
    framer_uas_drv = sal_alloc(sizeof(FRAMER_UAS_DRV_T), "FRAMER_UAS_DRV_T");
    if (framer_uas_drv == NULL)
    {
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

    framer_uas_drv->read = dnx_framer_reg_read;
    framer_uas_drv->write = dnx_framer_reg_write;
    framer_uas_drv->ram_read = dnx_framer_ram_read;
    framer_uas_drv->ram_write = dnx_framer_ram_write;
    rt = framer_uas_drv_register(unit, framer_uas_drv);
    if (rt != 0)
    {
        sal_free(framer_uas_drv);
        framer_uas_drv = NULL;
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    dnx_drv_soc_info_framer_access_drv_set(unit, framer_uas_drv);
    SHR_FUNC_EXIT;
}

/*De-initialize FlexE framer access register.*/
static shr_error_e
flexe_std_gen2_access_deinit(
    int unit)
{
    FRAMER_UAS_DRV_T *framer_uas_drv = NULL;
    FRAMER_RET rt;

    SHR_FUNC_INIT_VARS(unit);
    rt = framer_uas_drv_unregister(unit);
    if (rt != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    framer_uas_drv = dnx_drv_soc_info_framer_access_drv_get(unit);
    if (framer_uas_drv != NULL)
    {
        sal_free(framer_uas_drv);
        framer_uas_drv = NULL;
        dnx_drv_soc_info_framer_access_drv_set(unit, framer_uas_drv);
    }

exit:
    SHR_FUNC_EXIT;
}

static uint32
flexe_std_gen2_init_flags_get(
    int unit)
{
    uint32 flags = 0;

    /*
     * Prepare initializaiton flags
     */
    if (soc_sand_is_emulation_system(unit))
    {
        if (dnx_data_device.emulation.framer_init_mode_get(unit) == J2X_INIT_COMPONENT)
        {
            flags |= (FRAMER_INIT_COMPONENT << FLEXE_STD_GEN2_INIT_TYPE_SHIFT);
        }
        else if (dnx_data_device.emulation.framer_init_mode_get(unit) == J2X_INIT_WITHOUT_RAM)
        {
            flags |= (FRAMER_RAM_INIT_OFF_REG_INIT_ON << FLEXE_STD_GEN2_INIT_TYPE_SHIFT);
        }
    }
    if (dnx_data_nif.flexe.device_slot_mode_get(unit) == DNX_FLEXE_SLOT_MODE_1G_AND_5G)
    {
        flags |= (0x1 << FLEXE_STD_GEN2_INIT_DEVICE_SLOT_MODE_SHIFT);
    }

    return flags;
}
/**
 * \brief - FlexE modules init.
 *
 * \param [in] unit - chip unit id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_init(
    int unit)
{
    RST_TYPE init_type;
    uint32 flags = 0, default_val = 0x1ff;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    if (sw_state_is_warm_boot(unit))
    {
        init_type = FRAMER_RST_RESTORE;
    }
    else
    {
        init_type = FRAMER_RST_COLD;
    }

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_chip_create(unit, 0));

    SHR_IF_ERR_EXIT(flexe_std_gen2_access_init(unit));

    flags = flexe_std_gen2_init_flags_get(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_chip_init(unit, init_type, flags));
    /*
     * Configure FlexE deskew. Remove this when Framer driver is updated
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_multi_flexe_deskew_reg_field_set
                               (unit, 0, FLEXE_GRP_DESKEW_CTRL, multi_flexe_deskew_lc_grp_ssf_en, 0));
    for (index = 0; index < 1600; ++index)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_demux_ram_set
                                   (unit, 0, DCCM_CALENDARA_1, index, (FRAMER_U32 *) & default_val));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_demux_ram_set
                                   (unit, 0, DCCM_CALENDARB_1, index, (FRAMER_U32 *) & default_val));
    }
    for (index = 0; index < 16; ++index)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_rx_mode_cfg_set(unit, index, flexe1_rx_oh_lc_omf_chk_ctrl, 1));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_rx_mode_cfg_set(unit, index, flexe1_rx_oh_lc_omf_crc_ctrl, 1));
    }
    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Configure demux to RR mode.
         * Will remove and use WRR.
         */
        regw(unit, 0x20000, 0x403, 1);
        /*
         * Mark FlexE core is active
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_set(unit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - FlexE modules deinit.
 *
 * \param [in] unit - chip unit id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable SChan access to deinit framer in deinit process.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_chip_uninit(unit));

    SHR_IF_ERR_EXIT(flexe_std_gen2_access_deinit(unit));

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_chip_destory(unit));

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - FlexE modules sync for WB.
 *
 * \param [in] unit - chip unit id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_sync(
    int unit)
{
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    flags = flexe_std_gen2_init_flags_get(unit);
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_chip_init(unit, FRAMER_RST_STORE, flags));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure phymap
 *
 */
static shr_error_e
flexe_std_gen2_oh_phymap_set(
    int unit,
    int group_index,
    uint32 *instance_bmp,
    uint8 *instance_num_array,
    int enable)
{
    bcm_pbmp_t flexe_core_ports;
    int instance_id = 0, nof_ports;
    int phy_map_offset, clear_phymap, flexe_core_port_tmp;
    uint32 phy_map[32], group_instances[1] = { 0 }, instances_tmp;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get all the flexe core port in the group
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports));
    /*
     * Check if it is the only FlexE core port in the group
     */
    _SHR_PBMP_COUNT(flexe_core_ports, nof_ports);
    if (nof_ports > 1)
    {
        /*
         * If there is more than 1 FlexE PHYs in the group,
         * need to get the existing FlexE core port. We need to update
         * the phymap based on the exsiting phymap
         */
        _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port_tmp)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port_tmp, &instances_tmp));
            group_instances[0] |= instances_tmp;
        }
        if (!enable ||
            SHR_BITEQ_RANGE(group_instances, instance_bmp, 0, dnx_data_nif.flexe.nof_flexe_instances_get(unit)))
        {
            /*
             * If disable the phymap or there is no existing PHY in the group,
             * get the instance id from the instance_bmp
             */
            instance_id =
                shr_bitop_first_bit_get((uint32 *) instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit));
        }
        else
        {
            /*
             * Get the instance id from the existing instance bitmap
             */
            SHR_BIT_ITER(group_instances, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
            {
                if (!SHR_BITGET(instance_bmp, instance_id))
                {
                    break;
                }
            }
        }
    }
    else
    {
        instance_id =
            shr_bitop_first_bit_get((uint32 *) instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit));
        group_instances[0] |= *instance_bmp;
    }
    /*
     * Get original PHY MAP
     */
    for (phy_map_offset = 0; phy_map_offset < 32; ++phy_map_offset)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_phymap_get
                                   (unit, instance_id, phy_map_offset, (FRAMER_U32 *) & phy_map[phy_map_offset]));
    }
    /*
     * Enable or disable phy map
     */
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        phy_map_offset = instance_num_array[instance_id] / 8;
        SHR_BITWRITE(&phy_map[phy_map_offset], (instance_num_array[instance_id] % 8), enable);
    }
    /*
     * Update HW
     */
    for (phy_map_offset = 0; phy_map_offset < FLEXE_STD_GEN2_FLEXE_PHYMAP_OCTETS; ++phy_map_offset)
    {
        SHR_BIT_ITER(group_instances, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            clear_phymap = (((enable == 0) && SHR_BITGET(instance_bmp, instance_id)) ? 1 : 0);
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_phymap_set
                                       (unit, instance_id, phy_map_offset, clear_phymap ? 0 : phy_map[phy_map_offset]));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_phymap_rx_exp_set
                                       (unit, instance_id, phy_map_offset, clear_phymap ? 0 : phy_map[phy_map_offset]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set FlexE group ID for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] group_id - group ID in overhead
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_group_id_set(
    int unit,
    int flexe_core_port,
    int group_id)
{
    int instance_id;
    uint32 instance_bmp[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get instance bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_set
                                   (unit, instance_id, flexe1_tx_oh_lc_gid_ins, group_id));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_exgid_oh_cfg_set
                                   (unit, instance_id, flexe1_rx_oh_lc_exgid, group_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE group ID for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] flags - flags
 * \param [out] group_id - group ID in overhead
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_group_id_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *group_id)
{
    int instance_id;
    uint32 instance_bmp[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get instance bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        if (flags & BCM_PORT_FLEXE_TX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_get
                                       (unit, instance_id, flexe1_tx_oh_lc_gid_ins, (FRAMER_U32 *) group_id));
        }
        else if (flags & BCM_PORT_FLEXE_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_id_rx_value_get
                                       (unit, instance_id, flexe1_rx_oh_lc_acgid, (FRAMER_U32 *) group_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
        }
        break;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE phy mode
 *
 */
static shr_error_e
flexe_std_gen2_instance_mode_get(
    int unit,
    int speed,
    bcm_port_flexe_phy_slot_mode_t slot_mode,
    flexe_phy_mode * instance_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (speed == 50000)
    {
        if (slot_mode == bcmPortFlexePhySlot1G)
        {
            *instance_mode = FLEXE_PHY_50G_1GSLOT;
        }
        else if (slot_mode == bcmPortFlexePhySlot1P25G)
        {
            *instance_mode = FLEXE_PHY_50G_1dot25GSLOT;
        }
        else
        {
            *instance_mode = FLEXE_PHY_50G_5GSLOT;
        }
    }
    else
    {
        if (slot_mode == bcmPortFlexePhySlot1G)
        {
            *instance_mode = FLEXE_PHY_100G_1GSLOT;
        }
        else if (slot_mode == bcmPortFlexePhySlot1P25G)
        {
            *instance_mode = FLEXE_PHY_100G_1dot25GSLOT;
        }
        else
        {
            *instance_mode = FLEXE_PHY_100G_5GSLOT;
        }
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE slot base
 *
 */
static shr_error_e
flexe_std_gen2_slot_index_to_slot_base_get(
    int unit,
    flexe_phy_mode phy_mode,
    int slot_index,
    int *slot_base)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the slot base accroding the the Framer driver requirement
     *
     */
    if ((phy_mode == FLEXE_PHY_100G_1GSLOT) && (slot_index >= 50))
    {
        *slot_base = (FLEXE_STD_GEN2_MAX_NOF_SLOTS_PER_INSTANCE / 2) - 50;
    }
    else if ((phy_mode == FLEXE_PHY_100G_1dot25GSLOT) && (slot_index >= 40))
    {
        *slot_base = (FLEXE_STD_GEN2_MAX_NOF_SLOTS_PER_INSTANCE / 2) - 40;
    }
    else if ((phy_mode == FLEXE_PHY_100G_5GSLOT) && (slot_index >= 10))
    {
        *slot_base = (FLEXE_STD_GEN2_MAX_NOF_SLOTS_PER_INSTANCE / 2) - 10;
    }
    else
    {
        *slot_base = 0;
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE core ports to FlexE group
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] group_index - FlexE group index
 * \param [in] flexe_core_port_bmp - flexe core port bitmap
 * \param [in] logical_phy_id - FlexE logical PHY id
 * \param [in] port_info - FlexE core port info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_group_phy_add(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp,
    const int *logical_phy_id,
    const flexe_core_port_info_t * port_info)
{
    int instance_id, flexe_core_port, instance_index;
    uint32 instance_bmp_tmp[1] = { 0 };
    uint32 instance_bmp[1] = { 0 };
    int existed_group_id;
    uint8 instance_num_array_tmp[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES] = { 0 };
    uint8 instance_num_array[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES] = { 0 };
    flexe_phy_mode instance_mode;
    flexe_core_port_speed_mode_t speed_mode;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        instance_index = 0;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp_tmp));
        /*
         * Update DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.set(unit, flexe_core_port, group_index));
        /*
         * Get flexe core_port to instance id mapping
         */
        SHR_BIT_ITER(instance_bmp_tmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            SHR_BITSET(instance_bmp, instance_id);
            /*
             * Update instance id array
             */
            SHR_IF_ERR_EXIT(flexe_common_logical_phy_id_to_instance_num_get
                            (unit, port_info->speed, logical_phy_id[flexe_core_port], instance_index,
                             &instance_num_array_tmp[instance_id]));
            ++instance_index;
        }
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    SHR_IF_ERR_EXIT(flexe_std_gen2_instance_mode_get(unit, port_info->speed, slot_mode, &instance_mode));
    SHR_IF_ERR_EXIT(flexe_common_speed_mode_get(unit, port_info->speed, &speed_mode));
    /*
     * Set instance num array
     */
    instance_index = 0;
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        instance_num_array[instance_index] = instance_num_array_tmp[instance_id];
        ++instance_index;
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Configure Demux
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode,
                                    speed_mode, 0, 1));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Get existed group ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_index_to_id_get(unit, group_index, &existed_group_id));
        /*
         * Configure MUX
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode,
                                    speed_mode, 1, 1));
        /*
         * Configure overhead
         */
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_set
                                       (unit, instance_id, flexe1_tx_oh_lc_pid_ins,
                                        instance_num_array_tmp[instance_id]));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_exgid_oh_cfg_set
                                       (unit, instance_id, flexe1_rx_oh_lc_expid, instance_num_array_tmp[instance_id]));
        }
        SHR_IF_ERR_EXIT(flexe_std_gen2_oh_phymap_set(unit, group_index, instance_bmp, instance_num_array_tmp, 1));
        /*
         * Update group ID.
         * 1. If existed group id is -1, meaning the group id is not configured,
         *    so setting it to "1" - default value.
         * 2. If existed group id is not -1, meaning the group id has been configured,
         *    so setting it to the existed value for all the instances
         */
        _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
        {
            if (existed_group_id == -1)
            {
                SHR_IF_ERR_EXIT(flexe_std_gen2_oh_group_id_set(unit, flexe_core_port, 1));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_std_gen2_oh_group_id_set(unit, flexe_core_port, existed_group_id));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Remove FlexE core ports to FlexE group
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] group_index - FlexE group index
 * \param [in] flexe_core_port_bmp - flexe core port bitmap
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_group_phy_remove(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp)
{
    int instance_id, flexe_core_port;
    int instance_index, speed, logical_phy_id;
    uint32 instance_bmp[1] = { 0 }, instance_bmp_tmp[1] =
    {
    0};
    uint8 instance_num_array_tmp[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES] = { 0 };
    uint8 instance_num_array[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES] = { 0 };
    flexe_phy_mode instance_mode = FLEXE_PHY_100G_5GSLOT;
    bcm_port_flexe_phy_slot_mode_t slot_mode;
    flexe_core_port_speed_mode_t speed_mode;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        instance_index = 0;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, flexe_core_port, &speed));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.logical_phy_id.get(unit, flexe_core_port, &logical_phy_id));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp_tmp));
        /*
         * Get flexe core_port to instance id mapping
         */
        SHR_BIT_ITER(instance_bmp_tmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            SHR_BITSET(instance_bmp, instance_id);
            /*
             * Update instance id array
             */
            SHR_IF_ERR_EXIT(flexe_common_logical_phy_id_to_instance_num_get
                            (unit, speed, logical_phy_id, instance_index, &instance_num_array_tmp[instance_id]));
            ++instance_index;
        }
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    SHR_IF_ERR_EXIT(flexe_std_gen2_instance_mode_get(unit, speed, slot_mode, &instance_mode));
    SHR_IF_ERR_EXIT(flexe_common_speed_mode_get(unit, speed, &speed_mode));
    /*
     * Set instance num array
     */
    instance_index = 0;
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        instance_num_array[instance_index] = instance_num_array_tmp[instance_id];
        ++instance_index;
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Configure Demux
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode, speed_mode,
                                    0, 0));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Configure MUX
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode, speed_mode,
                                    1, 0));
        /*
         * Configure overhead
         */
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_set
                                       (unit, instance_id, flexe1_tx_oh_lc_pid_ins, 1));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_exgid_oh_cfg_set
                                       (unit, instance_id, flexe1_rx_oh_lc_expid, 1));
        }
        SHR_IF_ERR_EXIT(flexe_std_gen2_oh_phymap_set(unit, group_index, instance_bmp, instance_num_array_tmp, 0));

        _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_oh_group_id_set(unit, flexe_core_port, 1));
        }
    }
    /*
     * Update DB
     */
    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.set(unit, flexe_core_port, -1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get number of slots per instance
 *
 */
static shr_error_e
flexe_std_gen2_nof_slots_per_instance_get(
    int unit,
    int flexe_core_port,
    int *nof_slots)
{
    int group_index, instance_speed, speed, nof_instances;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get group index
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group_index));
    /*
     * Get slot mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    /*
     * Get instance speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, flexe_core_port, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_nof_instances_get(unit, flexe_core_port, &nof_instances));
    instance_speed = speed / nof_instances;

    *nof_slots = instance_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get number of slots per Calendar overhead RAM
 *
 */
static shr_error_e
flexe_std_gen2_client_overhead_ram_info_get(
    int unit,
    int flexe_core_port,
    int *nof_slots_per_ram,
    int *nof_rams)
{
    int group_index, instance_speed, speed, nof_instances;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get group index
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group_index));
    /*
     * Get slot mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    switch (slot_mode)
    {
        case bcmPortFlexePhySlot1G:
            *nof_rams = NOF_CLIENT_ID_RAMS_FOR_1G_SLOT;
            break;
        case bcmPortFlexePhySlot1P25G:
            *nof_rams = NOF_CLIENT_ID_RAMS_FOR_1P25G_SLOT;
            break;
        default:
            *nof_rams = NOF_CLIENT_ID_RAMS_FOR_5G_SLOT;
    }
    /*
     * Get instance speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, flexe_core_port, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_nof_instances_get(unit, flexe_core_port, &nof_instances));
    instance_speed = speed / nof_instances;
    /*
     * Get nof slots per RAM
     */
    *nof_slots_per_ram =
        instance_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val / *nof_rams;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create time slots for each FlexE PHY instance
 *
 */
static shr_error_e
flexe_std_gen2_ts_array_get(
    int unit,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports,
    int *ts_array,
    int *nof_slots)
{
    int i, max_slots_per_instance, nof_instances, base_instance, group_index;
    int inner_instance_index, global_instance_index, inner_slot_index, speed;
    int slot_base;
    bcm_port_flexe_phy_slot_mode_t slot_mode;
    bcm_port_flexe_time_slot_t tmp_ts_mask;
    flexe_phy_mode instance_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));
    /*
     * Get slot mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port_array[0], &group_index));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    /*
     * Get speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, flexe_core_port_array[0], &speed));
    /*
     * Get instance mode
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_instance_mode_get(unit, speed, slot_mode, &instance_mode));
    /*
     * Get nof instances per flexe core port
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_nof_instances_get(unit, flexe_core_port_array[0], &nof_instances));

    *nof_slots = 0;
    global_instance_index = 0;
    for (i = 0; i < nof_flexe_core_ports; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port_array[i], &base_instance));
        for (inner_instance_index = 0; inner_instance_index < nof_instances; ++inner_instance_index)
        {
            /*
             * Get the time slots for the instance
             */
            SHR_BITCOPY_RANGE(tmp_ts_mask, 0, ts_mask, global_instance_index * max_slots_per_instance,
                              max_slots_per_instance);
            SHR_BIT_ITER(tmp_ts_mask, max_slots_per_instance, inner_slot_index)
            {
                SHR_IF_ERR_EXIT(flexe_std_gen2_slot_index_to_slot_base_get
                                (unit, instance_mode, inner_slot_index, &slot_base));
                ts_array[*nof_slots] =
                    FLEXE_STD_GEN2_MAX_NOF_SLOTS_PER_INSTANCE * (base_instance + inner_instance_index) +
                    inner_slot_index + slot_base;
                (*nof_slots)++;
            }
            global_instance_index++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - clear the client id in overhead
 *
 */
static shr_error_e
flexe_std_gen2_oh_client_id_clear(
    int unit,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int max_slots_per_instance, i, inner_ts_index, instance_index;
    int nof_rams, nof_slots_per_ram;
    int instance_array[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES] =
        { 0 }, nof_instances, base_instance, total_nof_instances;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));
    SHR_IF_ERR_EXIT(flexe_std_gen2_client_overhead_ram_info_get
                    (unit, flexe_core_port_array[0], &nof_slots_per_ram, &nof_rams));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_nof_instances_get(unit, flexe_core_port_array[0], &nof_instances));
    /*
     * Get instance array
     */
    total_nof_instances = 0;
    for (i = 0; i < nof_flexe_core_ports; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port_array[i], &base_instance));
        for (instance_index = 0; instance_index < nof_instances; ++instance_index)
        {
            instance_array[total_nof_instances] = base_instance + instance_index;
            total_nof_instances++;
        }
    }
    /*
     * Update HW
     */
    for (i = 0; i < total_nof_instances; ++i)
    {
        for (inner_ts_index = 0; inner_ts_index < max_slots_per_instance; ++inner_ts_index)
        {
            if (SHR_BITGET(ts_mask, (inner_ts_index + i * max_slots_per_instance)))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ccab_set
                                           (unit, cal_id, inner_ts_index / nof_slots_per_ram, instance_array[i],
                                            inner_ts_index % nof_slots_per_ram, 0));
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_ccab_rx_exp_set
                                           (unit, cal_id, inner_ts_index / nof_slots_per_ram, instance_array[i],
                                            inner_ts_index % nof_slots_per_ram, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add flexe clients to FlexE group
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - FlexE client channel
 * \param [in] flags - flags
 * \param [in] cal_id - FlexE calendar A or B
 * \param [in] flexe_core_port_array - FlexE core port array
 * \param [in] ts_mask - timeslots bitmap
 * \param [in] nof_flexe_core_ports - nof flexe core ports the timeslots occupied
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int nof_slots, *ts_array = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(ts_array, dnx_data_nif.flexe.max_nof_slots_get(unit) * sizeof(int), "ts_array", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(flexe_std_gen2_ts_array_get(unit, flexe_core_port_array, ts_mask,
                                                nof_flexe_core_ports, ts_array, &nof_slots));
    if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (FRAMER_U32 *) ts_array, nof_slots, 0, cal_id, 1));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_mode_cfg
                                   (unit, LC_FLEXE1_OAM_MODULE, client_channel, OAM_MODE_FLEXE_BAS32, FRAMER_DIR_IN));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (FRAMER_U32 *) ts_array, nof_slots, 1, cal_id, 1));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_mode_cfg
                                   (unit, LC_FLEXE1_OAM_MODULE, client_channel, OAM_MODE_FLEXE_BAS32, FRAMER_DIR_OUT));
    }
exit:
    SHR_FREE(ts_array);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete flexe clients from FlexE group
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - FlexE client channel
 * \param [in] flags - flags
 * \param [in] cal_id - FlexE calendar A or B
 * \param [in] flexe_core_port_array - FlexE core port array
 * \param [in] ts_mask - timeslots bitmap
 * \param [in] nof_flexe_core_ports - nof flexe core ports the timeslots occupied
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int nof_slots, *ts_array = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(ts_array, dnx_data_nif.flexe.max_nof_slots_get(unit) * sizeof(int), "ts_array", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(flexe_std_gen2_ts_array_get(unit, flexe_core_port_array, ts_mask,
                                                nof_flexe_core_ports, ts_array, &nof_slots));
    if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (FRAMER_U32 *) ts_array, nof_slots, 0, cal_id, 0));

    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (FRAMER_U32 *) ts_array, nof_slots, 1, cal_id, 0));
        SHR_IF_ERR_EXIT(flexe_std_gen2_oh_client_id_clear
                        (unit, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));
    }
exit:
    SHR_FREE(ts_array);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Select the MGMT path, busa or busb
 *
 */
static shr_error_e
flexe_std_gen2_mgmt_path_sel_set(
    int unit,
    int is_ptp,
    int is_busb)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_ptp)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_field_set
                                   (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_EN, ccu_rx_cfg_lc_ccu_rx_path_sel, is_busb));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_field_set
                                   (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_EN, ccu_tx_cfg_lc_ccu_tx_path_sel, is_busb));
    }
    else
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ohif_flexe_upi_3th_ab_en_set
                                   (unit, TUNNEL_J2X_OHIF_FLEXE, ohif_flexe_lc_a_b_en, is_busb));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Map Framer channel to mgmt channel
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - Framer MAC channel
 * \param [in] mgmt_channel - MGMT channel
 * \param [in] is_rx - is Rx or Tx
 * \param [in] enable - enable or disbale mapping
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_mac_mgmt_channel_mapping_set(
    int unit,
    int client_channel,
    int mgmt_channel,
    int is_rx,
    int enable)
{
    int is_ptp;
    flexe_core_66bswitch_channel_info_t busb_info, mgmt_info;

    SHR_FUNC_INIT_VARS(unit);

    flexe_core_66bswitch_channel_info_t_init(unit, &busb_info);
    flexe_core_66bswitch_channel_info_t_init(unit, &mgmt_info);
    busb_info.channel = client_channel;
    /*
     * Get related block IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_framer_66bsw_block_id_get(unit, DNX_ALGO_PORT_TYPE_FRAMER_MAC, 0, &busb_info.block_id));
    mgmt_info.block_id = dnx_data_nif.framer.mgmt_channel_info_get(unit, mgmt_channel)->block_id;
    /*
     * Config mapping between client channel and mgmt channel
     */
    if (is_rx)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_66b_switch_set(unit, &mgmt_info, &busb_info, 0, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_66b_switch_set(unit, &busb_info, &mgmt_info, 0, enable));
    }
    /*
     * Configure mgmt channel to BusB side for both Rx and Tx
     */
    if (enable)
    {
        is_ptp = dnx_data_nif.framer.mgmt_channel_info_get(unit, mgmt_channel)->is_ptp;
        SHR_IF_ERR_EXIT(flexe_std_gen2_mgmt_path_sel_set(unit, is_ptp, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add a MAC client to bus B
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] mgmt_channel - mgmt channel
 * \param [in] flags - Rx or Tx
 * \param [in] speed - port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_mac_client_add(
    int unit,
    int client_channel,
    int mgmt_channel,
    uint32 flags,
    int speed)
{
    uint32 orig_speed;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        if (mgmt_channel != -1)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_mac_mgmt_channel_mapping_set(unit, client_channel, mgmt_channel, 1, 1));
        }
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Get original speed
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_env_mac1_busb_rato_query
                                   (unit, 1, client_channel, (FRAMER_U32 *) & orig_speed));
        /*
         * Clear orignal speed if it has been configured
         */
        if (orig_speed != UTILEX_U32_MAX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_busb_cfg
                                       (unit, client_channel, orig_speed, 0));
        }
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_busb_cfg(unit, client_channel, speed, 1));

        if (mgmt_channel != -1)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_mac_mgmt_channel_mapping_set(unit, client_channel, mgmt_channel, 0, 1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete a MAC client from bus B
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] mgmt_channel - mgmt channel
 * \param [in] flags - Rx or Tx
 * \param [in] speed - port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_mac_client_delete(
    int unit,
    int client_channel,
    int mgmt_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        if (mgmt_channel != -1)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_mac_mgmt_channel_mapping_set(unit, client_channel, mgmt_channel, 1, 0));
        }
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_busb_cfg(unit, client_channel, speed, 0));
        if (mgmt_channel != -1)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_mac_mgmt_channel_mapping_set(unit, client_channel, mgmt_channel, 0, 0));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - G.hao configuration for MAC client
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] speed - port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_mac_client_ghao_config_set(
    int unit,
    int client_channel,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_env_mac1_busb_ghao_resize(unit, LC_ENV_BUSB, client_channel, speed));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Convert BCM SAR cell mode to Framer SAR cell mode
 */
static shr_error_e
flexe_std_gen2_bcm_to_framer_sar_cell_mode_get(
    int unit,
    int bcm_cell_mode,
    int *sar_cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_cell_mode)
    {
        case bcmPortSarCell15x66bAnd13x66b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_YS28_S2;
            break;
        }
        case bcmPortSarCell15x66bAnd14x66b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_YS29_S2;
            break;
        }
        case bcmPortSarCell28x66b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_LC28_NON_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell29x66b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_LC29_NON_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell30x66bAnd28x66b:
        {
            *sar_cell_mode = BMOD_512_FLEXE_LC58_NON_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell30x66bAnd29x66b:
        {
            *sar_cell_mode = BMOD_512_FLEXE_LC59_NON_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell30x66bAnd30x66b:
        {
            *sar_cell_mode = BMOD_512_FLEXE_LC60_NON_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell28x65b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_LC28_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell29x65b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_LC29_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell30x65b:
        {
            *sar_cell_mode = BMOD_256_FLEXE_LC30_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell31x65bAnd29x65b:
        {
            *sar_cell_mode = BMOD_512_FLEXE_LC60_COMPRESS_S2;
            break;
        }
        case bcmPortSarCell31x65bAnd30x65b:
        {
            *sar_cell_mode = BMOD_512_FLEXE_LC61_COMPRESS_S2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported cell mode.\n");
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get SAR Encap value
 */
static shr_error_e
flexe_std_gen2_sar_encap_get(
    int unit,
    int client_channel,
    int speed,
    int is_rx,
    uint32 *value)
{
    int ts_num, bcm_cell_mode, framer_cell_mode;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the SAR cell mode and calendar slot mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_cell_mode_get(unit, client_channel, is_rx, &bcm_cell_mode));
    SHR_IF_ERR_EXIT(flexe_std_gen2_bcm_to_framer_sar_cell_mode_get(unit, bcm_cell_mode, &framer_cell_mode));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_cal_granularity_calculate(unit, speed, &slot_mode));
    /*
     * Encap the BP ID
     */
    ts_num = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;

    if (slot_mode == bcmPortFlexePhySlot5G)
    {
        *value = LC_ID_BUSINESS_OFFSET(framer_cell_mode, FRAMER_ID_FLEXE_CLIENT_5G(ts_num));
    }
    else if (slot_mode == bcmPortFlexePhySlot1G)
    {
        *value = LC_ID_BUSINESS_OFFSET(framer_cell_mode, FRAMER_ID_FLEXE_CLIENT_1G(ts_num));
    }
    else
    {
        *value = LC_ID_BUSINESS_OFFSET(framer_cell_mode, FRAMER_ID_FLEXE_CLIENT_1G25(ts_num));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get SAR configuration from Framer DB
 *
 */
static int
flexe_std_gen2_sar_config_get(
    int unit,
    int client_channel,
    int is_rx,
    int *is_configured,
    int *speed)
{
    SAR_TX_CALENDAR_T *tx_cal = NULL;
    SAR_RX_CALENDAR_T *rx_cal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(tx_cal, sizeof(SAR_TX_CALENDAR_T), "tx_cal", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(rx_cal, sizeof(SAR_RX_CALENDAR_T), "rx_cal", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (is_rx)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_sar_tx_query_api(unit, tx_cal));
        *is_configured = (tx_cal->ch_use_flag[client_channel] == SAR_TX_CH_USED) ? 1 : 0;
        *speed = (tx_cal->ch_ts[client_channel]) * dnx_data_nif.sar_client.cal_granularity_get(unit);
    }
    else
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_sar_rx_query_api(unit, rx_cal));
        *is_configured = (rx_cal->ch_use_flag[client_channel] == LC_CH_USED) ? 1 : 0;
        *speed = (rx_cal->ch_ts[client_channel]) * dnx_data_nif.sar_client.cal_granularity_get(unit);
    }
exit:
    SHR_FREE(tx_cal);
    SHR_FREE(rx_cal);
    SHR_FUNC_EXIT;
}

/**
 * \brief - internal Function to add SAR client
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] flags - flags
 * \param [in] speed - port speed
 * \param [in] peer_channel - peer channel the SAR connects to
 * \param [in] enable - Enable or disable the SAR channel
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_sar_client_internal_config(
    int unit,
    int client_channel,
    uint32 flags,
    int speed,
    int peer_channel,
    int enable)
{
    bcm_port_t sar_logical_port;
    uint32 bp_id;
    int is_configured, is_eth, orig_speed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                    (unit, DNX_ALGO_PORT_TYPE_FRAMER_SAR, client_channel, &sar_logical_port));
    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Check if SAR channel is configured, remove it before configuring new values
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_config_get(unit, client_channel, 1, &is_configured, &orig_speed));
        if (is_configured)
        {
            /*
             * Get SAR BP ID
             */
            SHR_IF_ERR_EXIT(flexe_std_gen2_sar_encap_get(unit, client_channel, orig_speed, 1, &bp_id));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_sar_flexe_client_cfg
                                       (unit, LC_BP_TRAFFIC_DEL, client_channel, peer_channel, bp_id, client_channel, 0,
                                        BP_SARTX_TO_BCM));
        }
        if (enable)
        {
            /*
             * Get SAR BP ID
             */
            SHR_IF_ERR_EXIT(flexe_std_gen2_sar_encap_get(unit, client_channel, speed, 1, &bp_id));
            /*
             * Configure global SAR property
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_sar_is_eth_get(unit, sar_logical_port, 1, &is_eth));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_sar_variable_cfg
                                       (unit, BP_SAR_CLIENT_TYPE, client_channel, is_eth));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_sar_flexe_client_cfg
                                       (unit, LC_BP_TRAFFIC_ADD, client_channel, peer_channel, bp_id, client_channel, 0,
                                        BP_SARTX_TO_BCM));
        }
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Check if SAR channel is configured, remove it before configuring new values
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_config_get(unit, client_channel, 0, &is_configured, &orig_speed));
        if (is_configured)
        {
            /*
             * Get SAR BP ID
             */
            SHR_IF_ERR_EXIT(flexe_std_gen2_sar_encap_get(unit, client_channel, orig_speed, 0, &bp_id));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_sar_flexe_client_cfg
                                       (unit, LC_BP_TRAFFIC_DEL, client_channel, peer_channel, bp_id, client_channel, 0,
                                        BP_BCM_TO_SARRX));
        }
        if (enable)
        {
            /*
             * Get SAR BP ID
             */
            SHR_IF_ERR_EXIT(flexe_std_gen2_sar_encap_get(unit, client_channel, speed, 0, &bp_id));
            /*
             * Configure global SAR property
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_sar_is_eth_get(unit, sar_logical_port, 0, &is_eth));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_sar_variable_cfg
                                       (unit, BP_SAR_CLIENT_TYPE, client_channel, is_eth));
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_sar_flexe_client_cfg
                                       (unit, LC_BP_TRAFFIC_ADD, client_channel, peer_channel, bp_id, client_channel, 0,
                                        BP_BCM_TO_SARRX));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - internal Function to configure SAR channel map
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] flags - flags
 * \param [in] peer_channel - peer channel the SAR connects to
 * \param [in] enable - Enable or disable the SAR channel
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_sar_channel_map_internal_config(
    int unit,
    int client_channel,
    uint32 flags,
    int peer_channel,
    int enable)
{
    flexe_core_rate_adpt_info_t orig_config;
    int speed;
    bcm_port_t sar_logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                    (unit, DNX_ALGO_PORT_TYPE_FRAMER_SAR, client_channel, &sar_logical_port));
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        /*
         * Always clear configurations
         */
        orig_config.module_id = FLEXE_STD_GEN2_VIRTUAL_FLEXE_MODULE_ID;
        SHR_IF_ERR_EXIT(flexe_std_gen2_rateadpt_get(unit, client_channel, &orig_config));

        if (orig_config.speed != LC_RATEADP_RATE_DEFAULT_VALUE)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_map_otn_sar_client_cfg
                                       (unit, 0, peer_channel, client_channel, speed, 0));
        }
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_map_otn_sar_client_cfg
                                       (unit, enable, peer_channel, client_channel, speed, 0));
        }
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
        /*
         * Always clear configurations
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_map_otn_sar_client_cfg
                                   (unit, 0, peer_channel, client_channel, speed, 1));
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_map_otn_sar_client_cfg
                                       (unit, enable, peer_channel, client_channel, speed, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add SAR client to bus C
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] flags - flags
 * \param [in] speed - port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_sar_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config(unit, client_channel, flags, speed, client_channel, 1));
    /*
     * Connect SAR channel and FlexE2 channel
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_sar_channel_map_internal_config(unit, client_channel, flags, client_channel, 1));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete SAR client from bus C
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] flags - flags
 * \param [in] speed - port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_sar_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config(unit, client_channel, flags, speed, LC_BP_INVLAND_CH, 0));
    /*
     * Disconnect SAR channel and FlexE2 channel
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_sar_channel_map_internal_config(unit, client_channel, flags, client_channel, 0));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the rate adapter configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [out] rate_adpt_info - rate adapt config info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_rateadpt_get(
    int unit,
    int client_channel,
    flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    uint8 bus_type;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get speed
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_rate_get
                               (unit, rate_adpt_info->module_id, client_channel,
                                (FRAMER_U32 *) & rate_adpt_info->speed));
    /*
     * Get bus type
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_bus_type_get
                               (unit, rate_adpt_info->module_id, client_channel, (FRAMER_U8 *) & bus_type));
    rate_adpt_info->bus_type = bus_type;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure rate adpt for flexe1 and flexe2
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] rate_adpt_info - rate adapt config info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_rateadpt_add(
    int unit,
    int client_channel,
    const flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    flexe_core_rate_adpt_info_t orig_config;
    LC_RATEADP_CFG_INFO_T cfg_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cfg_info, 0, sizeof(cfg_info));
    /*
     * Check if rate adapter is configured
     */
    orig_config.module_id = 0;
    SHR_IF_ERR_EXIT(flexe_std_gen2_rateadpt_get(unit, client_channel, &orig_config));

    if ((orig_config.speed == rate_adpt_info->speed) && (orig_config.bus_type == rate_adpt_info->bus_type))
    {
        /*
         * Nothing is changed.
         */
        SHR_EXIT();
    }

    if (orig_config.speed != LC_RATEADP_RATE_DEFAULT_VALUE)
    {
        cfg_info.bus_type = orig_config.bus_type;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_cfg
                                   (unit, FRAMER_TRAFFIC_REMOVE, rate_adpt_info->module_id, client_channel, &cfg_info));
    }
    /*
     * Configure new parameters
     */
    cfg_info.ch_rate = rate_adpt_info->speed;
    cfg_info.bus_type = rate_adpt_info->bus_type;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_cfg
                               (unit, FRAMER_TRAFFIC_ADD, rate_adpt_info->module_id, client_channel, &cfg_info));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete time slots in rate adapter
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] rate_adpt_info - rate adapt config info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_rateadpt_delete(
    int unit,
    int client_channel,
    const flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    LC_RATEADP_CFG_INFO_T cfg_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cfg_info, 0, sizeof(cfg_info));
    cfg_info.ch_rate = rate_adpt_info->speed;
    cfg_info.bus_type = rate_adpt_info->bus_type;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_cfg
                               (unit, FRAMER_TRAFFIC_REMOVE, rate_adpt_info->module_id, client_channel, &cfg_info));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Map SAR channel to peer channel
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] map_info - SAR client channel map info
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_sar_channel_map_set(
    int unit,
    uint32 flags,
    const flexe_core_sar_channel_map_info_t * map_info,
    int enable)
{
    int speed;
    bcm_port_t sar_logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Update SAR channel configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                    (unit, DNX_ALGO_PORT_TYPE_FRAMER_SAR, map_info->sar_channel, &sar_logical_port));
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config
                        (unit, map_info->sar_channel, BCM_PORT_FLEXE_RX, speed, map_info->peer_channel, 1));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config
                        (unit, map_info->sar_channel, BCM_PORT_FLEXE_TX, speed, map_info->peer_channel, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add a cross-connect in 66b switch
 *
 * \param [in] unit - chip unit id
 * \param [in] src_info - src channel info
 * \param [in] dest_info - dest channel info
 * \param [in] index - 0: main path, 1: backup path
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_66b_switch_set(
    int unit,
    const flexe_core_66bswitch_channel_info_t * src_info,
    const flexe_core_66bswitch_channel_info_t * dest_info,
    int index,
    int enable)
{
    int logical_port, speed;

    SHR_FUNC_INIT_VARS(unit);

    if (FLEXE_STD_GEN2_BLOCK_IS_BUSB(unit, dest_info->block_id))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                        (unit, DNX_ALGO_PORT_TYPE_FRAMER_MAC, dest_info->channel, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_cross_set_wrr(unit, src_info->block_id, src_info->channel,
                                                                    dest_info->block_id, dest_info->channel, speed));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_cross_clear_wrr(unit, src_info->block_id, src_info->channel,
                                                                      dest_info->block_id, dest_info->channel, speed));
        }
    }
    else
    {
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_cross_set(unit, src_info->block_id, src_info->channel,
                                                                dest_info->block_id, dest_info->channel));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_cross_clear(unit, src_info->block_id, src_info->channel,
                                                                  dest_info->block_id, dest_info->channel));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get 66b switching info
 *
 * \param [in] unit - chip unit id
 * \param [in] src_info - src channel info
 * \param [out] dest_info - dest channel info
 * \param [out] nof_dest - the number of dest info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_66b_switch_get(
    int unit,
    const flexe_core_66bswitch_channel_info_t * src_info,
    flexe_core_66bswitch_channel_info_t * dest_info,
    int *nof_dest)
{
    int i;
    int dst_block[FRAMER_MAX_SUPPORT_MUL];
    int dst_channel[FRAMER_MAX_SUPPORT_MUL];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init channel id
     */
    for (i = 0; i < FRAMER_MAX_SUPPORT_MUL; ++i)
    {
        dst_channel[i] = CROSS_DEFAULT_VALUE;
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_cross_query
                               (unit, src_info->block_id, src_info->channel, (FRAMER_U32 *) dst_block,
                                (FRAMER_U32 *) dst_channel));

    *nof_dest = 0;
    for (i = 0; i < FRAMER_MAX_SUPPORT_MUL; ++i)
    {
        if (dst_channel[i] != CROSS_DEFAULT_VALUE)
        {
            dest_info[*nof_dest].block_id = dst_block[i];
            dest_info[*nof_dest].channel = dst_channel[i];
            (*nof_dest)++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get 66b switching info from dest info
 *
 * \param [in] unit - chip unit id
 * \param [in] dest_info - dest channel info
 * \param [out] src_info - src channel info
 * \param [out] nof_src - the number of dest info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_66b_switch_reverse_get(
    int unit,
    const flexe_core_66bswitch_channel_info_t * dest_info,
    flexe_core_66bswitch_channel_info_t * src_info,
    int *nof_src)
{
    int i;
    int src_block[FRAMER_MAX_SUPPORT_MUL];
    int src_channel[FRAMER_MAX_SUPPORT_MUL];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init channel id
     */
    for (i = 0; i < FRAMER_MAX_SUPPORT_MUL; ++i)
    {
        src_channel[i] = CROSS_DEFAULT_VALUE;
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_cross_inverse_query
                               (unit, dest_info->block_id, dest_info->channel, (FRAMER_U32 *) src_block,
                                (FRAMER_U32 *) src_channel));

    *nof_src = 0;
    for (i = 0; i < FRAMER_MAX_SUPPORT_MUL; ++i)
    {
        if (src_channel[i] != CROSS_DEFAULT_VALUE)
        {
            src_info[*nof_src].block_id = src_block[i];
            src_info[*nof_src].channel = src_channel[i];
            (*nof_src)++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FlexE logical PHY ID for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] speed - port speed
 * \param [in] logical_phy_id - logical phy id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_logical_phy_id_set(
    int unit,
    int flexe_core_port,
    int speed,
    int logical_phy_id)
{
    uint32 instance_bmp[1];
    uint8 ori_instance_num_array[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES];
    uint8 instance_num_array[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES];
    int index;
    int instance_id, group_index;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get instances bitmap
     */
    index = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        SHR_IF_ERR_EXIT(flexe_common_logical_phy_id_to_instance_num_get
                        (unit, speed, logical_phy_id, index, &instance_num_array[index]));
        /*
         * Get original instance num
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_get
                                   (unit, instance_id, flexe1_tx_oh_lc_pid_ins,
                                    (FRAMER_U32 *) & ori_instance_num_array[index]));
        /*
         * Update new instance num
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_set
                                   (unit, instance_id, flexe1_tx_oh_lc_pid_ins, instance_num_array[index]));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_exgid_oh_cfg_set
                                   (unit, instance_id, flexe1_rx_oh_lc_expid, instance_num_array[index]));
        index++;
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group_index));
    /*
     * Skip phymap configure if group index is -1
     */
    if (group_index != -1)
    {
        /*
         * Clear the original PHY ID in the phymap
         */
        if (ori_instance_num_array[0] != 0)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_oh_phymap_set(unit, group_index, instance_bmp, ori_instance_num_array, 0));
        }
        /*
         * Update the new PHY ID in the phymap
         */
        if (instance_num_array[0] != 0)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_oh_phymap_set(unit, group_index, instance_bmp, instance_num_array, 1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE logical PHY ID for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] speed - port speed
 * \param [in] flags - flags
 * \param [out] logical_phy_id - logical phy id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_logical_phy_id_get(
    int unit,
    int flexe_core_port,
    int speed,
    uint32 flags,
    int *logical_phy_id)
{
    int base_instance;
    uint32 instance_num;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port, &base_instance));
    /*
     * Get original instance num
     */
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_get
                                   (unit, base_instance, flexe1_tx_oh_lc_pid_ins, (FRAMER_U32 *) & instance_num));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_id_rx_value_get
                                   (unit, base_instance, flexe1_rx_oh_lc_acpid, (FRAMER_U32 *) & instance_num));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
    }
    /*
     * Convert the instance number to logical phy ID
     */
    SHR_IF_ERR_EXIT(flexe_common_instance_num_to_logical_phy_id_get(unit, speed, instance_num, logical_phy_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set FlexE client IDs for FlexE OH calendar
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_id - FlexE calendar ID
 * \param [in] nof_ports - nof flexe core ports
 * \param [in] flexe_core_port_array - FlexE core port array
 * \param [in] nof_slots - nof slots
 * \param [in] calendar_slots - client ID array
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_client_id_set(
    int unit,
    int cal_id,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    const int *calendar_slots)
{
    int max_slots_per_instance, i, inner_ts_index, nof_slots_per_ram;
    int instance_id, instance_index, client_id, nof_rams;
    uint32 instance_bmp[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));
    SHR_IF_ERR_EXIT(flexe_std_gen2_client_overhead_ram_info_get
                    (unit, flexe_core_port_array[0], &nof_slots_per_ram, &nof_rams));

    instance_index = 0;
    for (i = 0; i < nof_ports; ++i)
    {
        /*
         * Get instance bitmap for flexe core port
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port_array[i], instance_bmp));
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            for (inner_ts_index = 0; inner_ts_index < max_slots_per_instance; ++inner_ts_index)
            {
                client_id = calendar_slots[inner_ts_index + instance_index * max_slots_per_instance];
                if (client_id != 0)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ccab_set
                                               (unit, cal_id, inner_ts_index / nof_slots_per_ram, instance_id,
                                                inner_ts_index % nof_slots_per_ram, client_id));
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_ccab_rx_exp_set
                                               (unit, cal_id, inner_ts_index / nof_slots_per_ram, instance_id,
                                                inner_ts_index % nof_slots_per_ram, client_id));
                }
            }
            instance_index++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE client IDs for FlexE OH calendar
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_id - FlexE calendar ID
 * \param [in] flags - flags
 * \param [in] nof_ports - nof flexe core ports
 * \param [in] flexe_core_port_array - FlexE core port array
 * \param [in] nof_slots - nof slots
 * \param [out] calendar_slots - client ID array
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_client_id_get(
    int unit,
    int cal_id,
    uint32 flags,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    int *calendar_slots)
{
    int max_slots_per_instance, i, inner_ts_index, nof_slots_per_ram;
    int instance_id, instance_index, ts_index, nof_rams;
    uint32 instance_bmp[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));
    SHR_IF_ERR_EXIT(flexe_std_gen2_client_overhead_ram_info_get
                    (unit, flexe_core_port_array[0], &nof_slots_per_ram, &nof_rams));

    instance_index = 0;
    for (i = 0; i < nof_ports; ++i)
    {
        /*
         * Get instance bitmap for flexe core port
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port_array[i], instance_bmp));
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            for (inner_ts_index = 0; inner_ts_index < max_slots_per_instance; ++inner_ts_index)
            {
                ts_index = inner_ts_index + instance_index * max_slots_per_instance;
                if (flags & BCM_PORT_FLEXE_TX)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ccab_get
                                               (unit, cal_id,
                                                inner_ts_index / nof_slots_per_ram,
                                                instance_id,
                                                inner_ts_index % nof_slots_per_ram,
                                                (FRAMER_U32 *) & calendar_slots[ts_index]));
                }
                else if (flags & BCM_PORT_FLEXE_RX)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_ccab_rx_get
                                               (unit, cal_id,
                                                inner_ts_index / nof_slots_per_ram,
                                                instance_id,
                                                inner_ts_index % nof_slots_per_ram,
                                                (FRAMER_U32 *) & calendar_slots[ts_index]));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
                }
            }
            instance_index++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure CA, CR, C bit in overhead
 *
 */
static int
flexe_std_gen2_oh_ca_cr_c_bit_set(
    int unit,
    int flexe_core_port,
    int field_id,
    int cal_id)
{
    int instance_id;
    uint32 instance_bmp[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get instance bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_set(unit, instance_id, field_id, cal_id));
        /*
         * Set expect CCC bit
         */
        if (field_id == flexe1_tx_oh_lc_ccc_ins)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_excc_oh_cfg_set
                                       (unit, instance_id, flexe1_rx_oh_lc_exccc, cal_id));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get CA, CR, C bit in overhead
 *
 */
static int
flexe_std_gen2_oh_ca_cr_c_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int field_id,
    int *cal_id)
{
    int instance_id;
    uint32 instance_bmp[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get instance bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        if (flags & BCM_PORT_FLEXE_TX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_ins_cfg0_get
                                       (unit, instance_id, field_id, (FRAMER_U32 *) cal_id));
        }
        else if (flags & BCM_PORT_FLEXE_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_cc_rx_value_get
                                       (unit, instance_id, field_id, (FRAMER_U32 *) cal_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
        }
        break;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set CR bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] cal_id - FlexE calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_cr_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_set(unit, flexe_core_port, flexe1_tx_oh_lc_cr_ins, cal_id));
exit:

    SHR_FUNC_EXIT;
}
/**
 * \brief - Get CR bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] flags - flags
 * \param [out] cal_id - FlexE calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_cr_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int field_id;

    SHR_FUNC_INIT_VARS(unit);

    field_id = (flags & BCM_PORT_FLEXE_TX) ? flexe1_tx_oh_lc_cr_ins : flexe1_rx_oh_lc_accr;
    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_get(unit, flexe_core_port, flags, field_id, cal_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set CA bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] cal_id - FlexE calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_ca_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_set(unit, flexe_core_port, flexe1_tx_oh_lc_ca_ins, cal_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get CA bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] flags - flags
 * \param [out] cal_id - FlexE calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_ca_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int field_id;

    SHR_FUNC_INIT_VARS(unit);

    field_id = (flags & BCM_PORT_FLEXE_TX) ? flexe1_tx_oh_lc_ca_ins : flexe1_rx_oh_lc_acca;
    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_get(unit, flexe_core_port, flags, field_id, cal_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set C bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] cal_id - FlexE calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_c_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_set(unit, flexe_core_port, flexe1_tx_oh_lc_ccc_ins, cal_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get C bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] flags - flags
 * \param [out] cal_id - FlexE calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_c_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int field_id;

    SHR_FUNC_INIT_VARS(unit);

    field_id = (flags & BCM_PORT_FLEXE_TX) ? flexe1_tx_oh_lc_ccc_ins : flexe1_rx_oh_lc_acccc;
    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_get(unit, flexe_core_port, flags, field_id, cal_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set SC bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - flexe core port
 * \param [in] sync_config - synchronization configuration in overhead
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_sc_bit_set(
    int unit,
    int flexe_core_port,
    int sync_config)
{
    int instance_id;
    uint32 instance_bmp;

    SHR_FUNC_INIT_VARS(unit);

    if ((sync_config != 0) && (sync_config != 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported synchronization configuration.\r\n");
    }
    /*
     * Get instance bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, &instance_bmp));
    SHR_BIT_ITER(&instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_reserved_set
                                   (unit, instance_id, flexe1_tx_oh_lc_sc_ins, sync_config));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_excc_oh_cfg_set
                                   (unit, instance_id, flexe1_rx_oh_lc_exsc, sync_config));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get SC bit for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - FlexE core port
 * \param [in] flags - Rx or Tx
 * \param [in] sync_config - SC bit
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_sc_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *sync_config)
{
    int instance_id;
    uint32 instance_bmp;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get instance bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, &instance_bmp));
    SHR_BIT_ITER(&instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        if (flags == BCM_PORT_FLEXE_TX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_tx_reserved_get
                                       (unit, instance_id, flexe1_tx_oh_lc_sc_ins, (FRAMER_U32 *) sync_config));
        }
        else if (flags == BCM_PORT_FLEXE_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_cc_rx_value_get
                                       (unit, instance_id, flexe1_rx_oh_lc_acsc, (FRAMER_U32 *) sync_config));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set active calendar for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - FlexE core port
 * \param [in] cal_id - Calendar A or B
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_active_calendar_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int direction, group_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group_index));
    direction = (cal_id == 0) ? 1 : 0;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_mux1_core_c_cfg(unit, group_index, direction));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get active calendar for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - FlexE core port
 * \param [in] flags - flags
 * \param [out] cal_id - Calendar A or B
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_active_calendar_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int base_instance, logical_phy;
    uint32 reg_val[1];
    j2x_flexe_mux1_core_info_t **flexe_mux_chip_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == BCM_PORT_FLEXE_TX)
    {
        SHR_ALLOC_SET_ZERO(flexe_mux_chip_info, sizeof(j2x_flexe_mux1_core_info_t *), "flexe_mux_chip_info",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_mux1_core_algorithm_chip_info_get(unit, flexe_mux_chip_info));
        /*
         * Get instance ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port, &base_instance));
        /*
         * Get logical_phy
         */
        logical_phy = (*flexe_mux_chip_info)->phy[base_instance].phy_comb_index[0];
        /*
         * Access HW
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_mux_reg_get(unit, TUNNEL_J2X_FLEXE_MUX_M0, C_CFG, reg_val));
        /*
         * Get calendar ID
         */
        *cal_id = SHR_BITGET(reg_val, logical_phy) ? 1 : 0;
    }
    else if (flags == BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_oh_ca_cr_c_bit_get(unit, flexe_core_port, flags, flexe1_rx_oh_lc_acccc, cal_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }
exit:
    SHR_FREE(flexe_mux_chip_info);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the slots of FlexE calendar
 *    into schedule calendar and switch the schedule
 *    calendar
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - group index
 * \param [in] flags - flags, rx or tx
 * \param [in] flexe_cal - the FlexE calendar ID, A or B
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_schedule_calendar_switch(
    int unit,
    int group_index,
    uint32 flags,
    int flexe_cal)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_mux1_core_schedule_config(unit, group_index, 0, flexe_cal, 1));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_mux_reg_set(unit, TUNNEL_J2X_FLEXE_MUX_M0, CFG_PLS, 1));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_demux1_core_schedule_cfg(unit, group_index, 0, flexe_cal, 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OH alarm status for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - FlexE core port
 * \param [out] alarm_status - alarm status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_alarm_status_get(
    int unit,
    int flexe_core_port,
    uint16 *alarm_status)
{
    int instance_id, alarm, group_index;
    int max_slots_per_instance, i, client_id_ram_index;
    int nof_slots_per_ram, nof_rams;
    bcm_port_flexe_group_cal_t curr_cal;
    uint32 instance_bmp[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get group index
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group_index));

    if (group_index != -1)
    {
        /*
         * Get MAX number of slots per instance, the group slot mode is specified when creating group.
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port, &max_slots_per_instance));
        SHR_IF_ERR_EXIT(flexe_std_gen2_client_overhead_ram_info_get
                        (unit, flexe_core_port, &nof_slots_per_ram, &nof_rams));
    }
    /*
     * Get instance bitmap
     */
    *alarm_status = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        for (i = 0; i < COUNTOF(oh_alarm_field_to_flags_map_table); ++i)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oh_rx_alm_get
                                       (unit, instance_id, oh_alarm_field_to_flags_map_table[i].field_id,
                                        (FRAMER_U32 *) & alarm));
            *alarm_status |= (alarm ? oh_alarm_field_to_flags_map_table[i].flags : 0);
        }
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_cc_rx_value_get
                                   (unit, instance_id, flexe1_rx_oh_lc_acccc, (FRAMER_U32 *) & curr_cal));
        /*
         * Client mismatch alarm is only relevant for FlexE group
         */
        if (group_index != -1)
        {
            for (client_id_ram_index = 0; client_id_ram_index < nof_rams; ++client_id_ram_index)
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_ccm_rx_alm_get
                                           (unit, instance_id, client_id_ram_index, flexe1_rx_oh_lc_ccm0,
                                            (FRAMER_U32 *) & alarm));
                *alarm_status |=
                    (alarm
                     ? ((curr_cal ==
                         bcmPortFlexeGroupCalA) ? FLEXE_CORE_OH_ALARM_CAL_A_MISMATCH :
                        FLEXE_CORE_OH_ALARM_CAL_B_MISMATCH) : 0);
            }
        }
    }
    /*
     * Need to add group deskew alarm
     */
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get calendar OH mismatch timeslots for FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - FlexE core port
 * \param [out] time_slots - mismatched timeslots bitmap
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_oh_cal_mismatch_ts_get(
    int unit,
    int flexe_core_port,
    bcm_port_flexe_time_slot_t * time_slots)
{
    int max_slots_per_instance, index, nof_slots_per_ram;
    int instance_id, client_id_ram_index, nof_rams;
    uint32 instance_bmp[1], mismatched_slots[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port, &max_slots_per_instance));
    SHR_IF_ERR_EXIT(flexe_std_gen2_client_overhead_ram_info_get(unit, flexe_core_port, &nof_slots_per_ram, &nof_rams));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));

    SHR_BITCLR_RANGE(*time_slots, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);
    index = 0;
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        for (client_id_ram_index = 0; client_id_ram_index < nof_rams; ++client_id_ram_index)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_ccm_rx_alm_get
                                       (unit, instance_id, client_id_ram_index, flexe1_rx_oh_lc_ccm0,
                                        mismatched_slots));
            SHR_BITCOPY_RANGE(*time_slots, index * nof_slots_per_ram, mismatched_slots, 0, nof_slots_per_ram);
            index++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure default FlexE phy settings
 *
 * \param [in] unit - chip unit id
 * \param [in] flexe_core_port - FlexE core port
 * \param [in] local_port - FlexE local port
 * \param [in] speed - port speed
 * \param [in] enable - enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_phy_default_config_set(
    int unit,
    int flexe_core_port,
    int local_port,
    int speed,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Convert bcm port type to Framer port type
 *
 */
static shr_error_e
flexe_std_gen2_busa_port_info_get(
    int unit,
    const flexe_core_busa_channel_map_info_t * channel_map_info,
    int *busa_port_type,
    int *busa_port)
{
    int speed;

    SHR_FUNC_INIT_VARS(unit);

    *busa_port = channel_map_info->local_port;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, channel_map_info->flexe_core_port, &speed));
    switch (speed)
    {
        case 10000:
        {
            if (channel_map_info->is_mgmt_intf)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_PTP_10G;
                *busa_port = FLEXE_STD_GEN2_BUSA_PTP_PORT;
            }
            else
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_10G;
            }
            break;
        }
        case 25000:
        {
            if (channel_map_info->is_mgmt_intf)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_OHIF_25G;
                *busa_port = FLEXE_STD_GEN2_BUSA_OH_OAM_PORT + channel_map_info->local_port;
            }
            else if (channel_map_info->is_transparent && channel_map_info->is_fec)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_25G;
            }
            else
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_25G_NO_CWM;
            }
            break;
        }
        case 50000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_FLEXE_50G;
            }
            else if (channel_map_info->is_transparent)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_50G;
            }
            else
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_50G_NO_AM;
            }
            break;
        }
        case 100000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_FLEXE_100G;
            }
            else if (channel_map_info->is_transparent)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_100G;
            }
            else
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_100G_NO_AM;
            }
            break;
        }
        case 200000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_FLEXE_200G;
            }
            else if (channel_map_info->is_transparent)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_200G;
            }
            else
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_200G_NO_AM;
            }
            break;
        }
        case 400000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_FLEXE_400G;
            }
            else if (channel_map_info->is_transparent)
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_400G;
            }
            else
            {
                *busa_port_type = FRAMER_MAC_ENV_PORT_ETH_400G_NO_AM;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure CCU 1588 configurations,
 *    including filters and port mapping
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] ccu_traffic_type - FlexE or ETH 1588 traffic
 * \param [in] busa_port - FlexE instance id or ETH first PHY, 
 * \param [in] ccu_id - ccu port id, equal to the first PHY
 * \param [in] enable - enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_ccu_1588_config_set(
    int unit,
    uint32 flags,
    int ccu_traffic_type,
    int busa_port,
    int ccu_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure port mapping and vlan ID
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_ieee1588_ccu_list_cfg
                               (unit, ccu_traffic_type, busa_port, ccu_id,
                                FLEXE_STD_GEN2_CCU_BASE_VLAN + ccu_id, ccu_id, enable));
    /*
     * Configure unicast EN for L2/IPv4/IPv6
     */
    if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_ram_field_set
                                   (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_CFG_LIST, ccu_id, ccu_tx_cfg_lc_l2_uni_en,
                                    (FRAMER_U32 *) & enable));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_ram_field_set
                                   (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_CFG_LIST, ccu_id,
                                    ccu_tx_cfg_lc_l3_ipv4_uni_en, (FRAMER_U32 *) & enable));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_ram_field_set
                                   (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_CFG_LIST, ccu_id,
                                    ccu_tx_cfg_lc_l3_ipv6_uni_en, (FRAMER_U32 *) & enable));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_ram_field_set
                                   (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_CFG_LIST, ccu_id, ccu_rx_cfg_lc_l2_uni_en,
                                    (FRAMER_U32 *) & enable));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_ram_field_set
                                   (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_CFG_LIST, ccu_id,
                                    ccu_rx_cfg_lc_l3_ipv4_uni_en, (FRAMER_U32 *) & enable));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_ram_field_set
                                   (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_CFG_LIST, ccu_id,
                                    ccu_rx_cfg_lc_l3_ipv6_uni_en, (FRAMER_U32 *) & enable));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set the BusA adapt channel map in FlexE Core
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] channel_map_info - channel map info
 * \param [in] enable - enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_busa_channel_map_set(
    int unit,
    uint32 flags,
    const flexe_core_busa_channel_map_info_t * channel_map_info,
    int enable)
{
    int port_type = 0, busa_port, ccu_busa_port;
    int ccu_traffic_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_busa_port_info_get(unit, channel_map_info, &port_type, &busa_port));

    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Speed is irrelevant
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_busa66_cfg
                                   (unit, channel_map_info->flexe_core_port, port_type, busa_port, 0,
                                    FLEXE_MUX, J2X_DIRECTION_TX, enable));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Speed and dst_type are irrelevant
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_subsys_flexe_adapter_busa66_cfg
                                   (unit, channel_map_info->flexe_core_port, port_type, busa_port, 0,
                                    0, J2X_DIRECTION_RX, enable));
    }
    /*
     * Configure PTP/OHIF direction for BusA side
     */
    if ((port_type == FRAMER_MAC_ENV_PORT_PTP_10G) || (port_type == FRAMER_MAC_ENV_PORT_OHIF_25G))
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_mgmt_path_sel_set(unit, (port_type == FRAMER_MAC_ENV_PORT_PTP_10G), 0));
    }
    /*
     * Configure CCU port
     */
    if (channel_map_info->is_eth || channel_map_info->is_flexe_phy)
    {
        ccu_traffic_type = channel_map_info->is_eth ? J2X_IEEE1588_TRAFFIC_ETH : J2X_IEEE1588_TRAFFIC_FLEXE;
        ccu_busa_port = channel_map_info->is_eth ? channel_map_info->flexe_core_port : channel_map_info->local_port;
        SHR_IF_ERR_EXIT(flexe_std_gen2_ccu_1588_config_set
                        (unit, flags, ccu_traffic_type, ccu_busa_port, channel_map_info->flexe_core_port, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/*Get OAM alarm status for FlexE core port*/
shr_error_e
flexe_std_gen2_oam_alarm_status_get(
    int unit,
    int client_channel,
    uint16 *alarms_status)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_CH_ALM_GET(0), val));

    *alarms_status = 0;
    if (val[2] & SAL_BIT(0))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_CSF_LPI;
    }
    if (val[2] & SAL_BIT(1))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_CS_LF;
    }
    if (val[2] & SAL_BIT(2))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_CS_RF;
    }
    if (val[2] & SAL_BIT(3))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_BASE_OAM_LOS;
    }
    if (val[2] & SAL_BIT(4))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_BASE_CRC;
    }
    if (val[2] & SAL_BIT(5))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_BASE_RDI;
    }
    if (val[2] & SAL_BIT(6))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_BASE_PERIOD_MISMATCH;
    }
    if (val[2] & SAL_BIT(7))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_SDBIP;
    }
    if (val[2] & SAL_BIT(8))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_SDREI;
    }
    if (val[2] & SAL_BIT(9))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_SFBIP;
    }
    if (val[2] & SAL_BIT(10))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_SFREI;
    }
    if (val[2] & SAL_BIT(12))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_LF;
    }
    if (val[2] & SAL_BIT(13))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_RF;
    }
exit:
    SHR_FUNC_EXIT;
}

/*Enable/disable OAM alarm for FlexE client*/
shr_error_e
flexe_std_gen2_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/*Set BAS OAM period for FlexE client*/
shr_error_e
flexe_std_gen2_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period)
{
    FRAMER_U32 val[3];
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = base_period;
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_BAS_PERIOD_SET(0), val));
        /*
         * Update DB, only relevant for Tx
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                        (unit, DNX_ALGO_PORT_TYPE_FLEXE_CLIENT, client_channel, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_bas_period_set(unit, logical_port, base_period));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BAS_PERIOD_SET(0), val));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period)
{
    FRAMER_U32 val[3];
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * For Tx, get it from DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                        (unit, DNX_ALGO_PORT_TYPE_FLEXE_CLIENT, client_channel, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_bas_period_get(unit, logical_port, base_period));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
        val[1] = client_channel;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BAS_PERIOD_GET(0), val));
        *base_period = val[2];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*Enable/disable BAS insertion for FlexE client*/
shr_error_e
flexe_std_gen2_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    FRAMER_U32 val[5];
    uint32 orig_period;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get original period
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_oam_base_period_get(unit, client_channel, BCM_PORT_FLEXE_TX, &orig_period));
    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    val[3] = orig_period;
    val[4] = FRAMER_DIR_OUT;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_BAS_EN_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_BAS_EN_GET(0), val));
    *enable = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Enable/disable OAM bypass for FlexE client*/
shr_error_e
flexe_std_gen2_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    FRAMER_U32 val[3];
    uint32 orig_period;

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_BYPASS_SET(0), val));
        /*
         * The bypass will clear the orignal BAS period in HW,
         * so need to recover the orginal period when bypass is disabled
         */
        if (!enable)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_oam_base_period_get(unit, client_channel, BCM_PORT_FLEXE_TX, &orig_period));
            SHR_IF_ERR_EXIT(flexe_std_gen2_oam_base_period_set(unit, client_channel, BCM_PORT_FLEXE_TX, orig_period));
        }
        /*
         * If Tx OAM is bypassed, need to disable the LF/RF replacement.
         * If Tx OAM is not bypassed, need to replace the LF/RF with IDLE
         */
        val[0] = !enable;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_oam_tx_mtn_ram_field_set
                                   (unit, 0, LF_RF_LPI_EN, client_channel, flexe_oam_tx_mtn_lc_lfrf_rpl_en, val));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BYPASS_SET(0), val));
        /*
         * If Rx OAM is bypassed, don't need to recover the LF/RF according to the alarm in BAS OAM
         * If Rx OAM is not bypassed, need to recover the LF/RF according to the alarm in BAS OAM
         */
        val[0] = !enable;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_oam_rx_ram_field_set
                                   (unit, 0, RX_IDLE_RPL_EN, client_channel, flexe_oam_rx_lc_rx_idle_rpl_en, val));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;

    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_BYPASS_GET(0), val));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BYPASS_GET(0), val));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
    }
    *enable = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Enable/disable OAM local fault insertion for FlexE client*/
shr_error_e
flexe_std_gen2_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_INSERT_LF_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_INSERT_LF_GET(0), val));
    *enable = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Enable/disable OAM remote fault insertion for FlexE client*/
shr_error_e
flexe_std_gen2_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_INSERT_RF_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_INSERT_RF_GET(0), val));
    *enable = val[2];
exit:
    SHR_FUNC_EXIT;
}

/* Enable/disable OAM rdi insertion for FlexE client */
shr_error_e
flexe_std_gen2_oam_rdi_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_INSERT_RDI_SET(0), val));

exit:
    SHR_FUNC_EXIT;
}

/* Get OAM rdi insertion status for FlexE client */
shr_error_e
flexe_std_gen2_oam_rdi_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_TX_INSERT_RDI_GET(0), val));
    *enable = val[2];

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flexe_std_gen2_block_num_verify(
    int unit,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    if (block_num > FLEXE_STD_GEN2_MAX_BLOCK_NUM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Block num need be less or equal than %d\r\n", FLEXE_STD_GEN2_MAX_BLOCK_NUM);
    }

exit:
    SHR_FUNC_EXIT;
}

/*Configure SD alarm block number for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_block_num_verify(unit, block_num));

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BIP8_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BIP8_BLOCK_NUM_GET(0), val));
    *block_num = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SD BIP8 error set threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BIP8_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BIP8_SET_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SD BIP8 error clear threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BIP8_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BIP8_CLR_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SF bip8 alarm block number for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_block_num_verify(unit, block_num));

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BIP8_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BIP8_BLOCK_NUM_GET(0), val));
    *block_num = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SF BIP8 error set threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BIP8_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BIP8_SET_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SF BIP8 error clear threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BIP8_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BIP8_CLR_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SD BEI alarm block number for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_block_num_verify(unit, block_num));

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BEI_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BEI_BLOCK_NUM_GET(0), val));
    *block_num = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SD BEI error set threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BEI_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BEI_SET_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SD BEI error clear threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BEI_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SD_BEI_CLR_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SF BEI alarm block number for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_block_num_verify(unit, block_num));

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BEI_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BEI_BLOCK_NUM_GET(0), val));
    *block_num = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SF BEI error set threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BEI_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BEI_SET_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Configure SF BEI error clear threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BEI_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    FRAMER_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd
                               (unit, FRAMER_CMD_FLEXE_OAM_RX_SF_BEI_CLR_THRESHOLD_GET(0), val));
    *threshold = val[2];
exit:
    SHR_FUNC_EXIT;
}

/*Get OAM BIP8 counter for FlexE client*/
shr_error_e
flexe_std_gen2_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    FRAMER_U32 val_tmp[3];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BIP8_CNT_GET(0), val_tmp));
    COMPILER_64_SET(*val, 0, val_tmp[2]);
exit:
    SHR_FUNC_EXIT;
}

/*Get OAM BEI counter for FlexE client*/
shr_error_e
flexe_std_gen2_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    FRAMER_U32 val_tmp[3];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BEI_CNT_GET(0), val_tmp));
    COMPILER_64_SET(*val, 0, val_tmp[2]);
exit:
    SHR_FUNC_EXIT;
}

/*Get OAM packet counter for FlexE client*/
shr_error_e
flexe_std_gen2_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    FRAMER_U32 val_tmp[4];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_CNT_GET(0), val_tmp));
    COMPILER_64_SET(*val, val_tmp[3], val_tmp[2]);
exit:
    SHR_FUNC_EXIT;
}

/*Get OAM BAS packet counter for FlexE client*/
shr_error_e
flexe_std_gen2_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    FRAMER_U32 val_tmp[4];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_J2X_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_cmd(unit, FRAMER_CMD_FLEXE_OAM_RX_BAS_CNT_GET(0), val_tmp));
    COMPILER_64_SET(*val, val_tmp[3], val_tmp[2]);
exit:
    SHR_FUNC_EXIT;
}

/* Enable bas bip recalculation */
shr_error_e
flexe_std_gen2_oam_bas_bip_recal_enable_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_oam_rx_reg_field_set
                               (unit, TUNNEL_J2X_FLEXE_OAM_RX_M0, OAM_RX_CONFIG, flexe_oam_rx_lc_bip_recal_en, enable));

exit:
    SHR_FUNC_EXIT;
}

/* Get bas bip recalculation */
shr_error_e
flexe_std_gen2_oam_bas_bip_recal_enable_get(
    int unit,
    int *enable)
{
    FRAMER_U32 val_get;

    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_oam_rx_reg_field_get
                               (unit, TUNNEL_J2X_FLEXE_OAM_RX_M0, OAM_RX_CONFIG, flexe_oam_rx_lc_bip_recal_en,
                                &val_get));
    *enable = (int) val_get;

exit:
    SHR_FUNC_EXIT;
}

/* Configure if Bas OAM period mismatch trigger Los ALM */
shr_error_e
flexe_std_gen2_oam_bas_period_mismatch_trigger_los_alarm_set(
    int unit,
    int val)
{
    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_oam_rx_reg_field_set
                               (unit, TUNNEL_J2X_FLEXE_OAM_RX_M0, OAM_RX_CONFIG,
                                flexe_oam_rx_lc_oam_rx_bas_los_sel, val));

exit:
    SHR_FUNC_EXIT;
}

/* Get if Bas OAM period mismatch trigger Los ALM */
shr_error_e
flexe_std_gen2_oam_bas_period_mismatch_trigger_los_alarm_get(
    int unit,
    int *val)
{
    FRAMER_U32 val_get;

    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_oam_rx_reg_field_get
                               (unit, TUNNEL_J2X_FLEXE_OAM_RX_M0, OAM_RX_CONFIG,
                                flexe_oam_rx_lc_oam_rx_bas_los_sel, &val_get));
    *val = (int) val_get;

exit:
    SHR_FUNC_EXIT;
}

/* Enable or disable to trigger local fault by deskew alarm */
shr_error_e
flexe_std_gen2_deskew_alarm_trigger_lf_set(
    int unit,
    int group_index,
    int val)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * tunnel_id is always 0, alarm type is 2 for local fault
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_deskew_alm_en_set
                               (unit, 0, (FRAMER_U8) group_index, 2, (FRAMER_U32) val));

exit:
    SHR_FUNC_EXIT;
}

/* Get status which trigger local fault by deskew alarm */
shr_error_e
flexe_std_gen2_deskew_alarm_trigger_lf_get(
    int unit,
    int group_index,
    int *val)
{
    FRAMER_U32 val_get;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * tunnel_id is always 0, alarm type is 2 for local fault
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_deskew_alm_en_get(unit, 0, (FRAMER_U8) group_index, 2, &val_get));

    *val = (int) val_get;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SAR cell mode per channel in Framer core
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - SAR client channel
 * \param [in] is_rx - rx or tx
 * \param [in] cell_mode - SAR cell mode
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_sar_cell_mode_per_channel_set(
    int unit,
    int client_channel,
    int is_rx,
    int cell_mode)
{
    int peer_channel, speed;
    uint32 flags, speed_flags;
    bcm_port_t sar_logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                    (unit, DNX_ALGO_PORT_TYPE_FRAMER_SAR, client_channel, &sar_logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_sar_virtual_flexe_channel_get(unit, sar_logical_port, &peer_channel));

    flags = is_rx ? BCM_PORT_FLEXE_RX : BCM_PORT_FLEXE_TX;
    speed_flags = is_rx ? DNX_ALGO_PORT_SPEED_F_RX : DNX_ALGO_PORT_SPEED_F_TX;
    peer_channel = (peer_channel == -1) ? LC_BP_INVLAND_CH : peer_channel;
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, speed_flags, &speed));
    SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config(unit, client_channel, flags, speed, peer_channel, 1));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Enable/disable SAR client
 */
shr_error_e
flexe_std_gen2_sar_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags == BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_sar_tx_channel_cfg_set
                                   (unit, SAR_TX_EN, TUNNEL_J2X_SAR_TX, client_channel, enable));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Only Rx flag .\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get SAR client enable/disable status
 */
shr_error_e
flexe_std_gen2_sar_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    int *enable)
{
    FRAMER_U32 enable_tmp;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_sar_tx_channel_cfg_get
                                   (unit, SAR_TX_EN, TUNNEL_J2X_SAR_TX, client_channel, &enable_tmp));
        *enable = enable_tmp;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Only Rx flag .\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure FLR rate generator divider
 */
static shr_error_e
flexe_std_gen2_flr_traffic_type_get(
    int unit,
    int speed,
    bcm_port_phy_fec_t fec_type,
    int *traffic_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (speed)
    {
        case 10000:
        {
            *traffic_type = PHYMUX_10GE;
            break;
        }
        default:
        {
            if ((fec_type == bcmPortPhyFecRs544) || (fec_type == bcmPortPhyFecRs272) ||
                (fec_type == bcmPortPhyFecRs544_2xN) || (fec_type == bcmPortPhyFecRs272_2xN))
            {
                *traffic_type = PHYMUX_25GE_50GE_100GE_200GE_400GE_KP4;
            }
            else
            {
                *traffic_type = PHYMUX_25GE_50GE_100GE_200GE_400GE;
            }
            break;
        }
    }
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure FLR rate generator divider
 */
shr_error_e
flexe_std_gen2_flr_rate_generator_divider_set(
    int unit,
    bcm_port_t logical_port)
{
    int speed, traffic_type;
    int nof_lanes, first_logical_lane, lane_index;
    bcm_port_phy_fec_t fec_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_fec_type_get(unit, logical_port, &fec_type));

    SHR_IF_ERR_EXIT(flexe_std_gen2_flr_traffic_type_get(unit, speed, fec_type, &traffic_type));
    /*
     * Get FLR logical lane
     */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_get(unit, logical_port, &nof_lanes, &first_logical_lane));
    for (lane_index = 0; lane_index < nof_lanes; ++lane_index)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_lane_freq_cfg
                                   (unit, 1, lane_index + first_logical_lane, traffic_type, FREQ_DEFAULT_PERIOD,
                                    FREQ_DEFAULT_PPM));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure FLR logical lane to serdes physical lane mapping
 */
shr_error_e
flexe_std_gen2_flr_logical_to_serdes_lane_set(
    int unit,
    bcm_port_t logical_port)
{
#ifdef INCLUDE_FLEXE_OTN
    int speed, count = 0;
    int nof_otn_logical_lanes, nof_phy_lanes;
    int is_pam4, i;
    uint32 flr_logical_lane;
    int otn_logical_lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES];
    int phy_lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES];
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef INCLUDE_FLEXE_OTN
    /*
     * Get OTN physical lanes
     */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_lanes_get
                    (unit, logical_port, DNX_DATA_MAX_NIF_OTN_NOF_LANES, phy_lanes, &nof_phy_lanes));
    /*
     * Get FLR logical lanes
     */
    SHR_IF_ERR_EXIT(imb_port_lane_map_get
                    (unit, logical_port, 0, DNX_DATA_MAX_NIF_OTN_NOF_LANES, otn_logical_lanes, &nof_otn_logical_lanes));

    is_pam4 = (nof_otn_logical_lanes == nof_phy_lanes) ? 0 : 1;
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
    /*
     * Configure lane mapping
     */
    for (i = 0; i < nof_phy_lanes; ++i)
    {
        flr_logical_lane = is_pam4 ? otn_logical_lanes[2 * count] : otn_logical_lanes[count];
        SHR_IF_ERR_EXIT(framer_uas_j2x_lane_otn_fiber_los_ram_set
                        (unit, 0, CFG_LOGIC_LOS_MUX, phy_lanes[i], (FRAMER_U32 *) & flr_logical_lane));
        SHR_IF_ERR_EXIT(framer_uas_j2x_lane_otn_fiber_los_ram_set
                        (unit, 0, CFG_FREQ_ALM_MUX, phy_lanes[i], (FRAMER_U32 *) & flr_logical_lane));
        count++;
    }
exit:
#endif
    SHR_FUNC_EXIT;
}
/*
 * \brief - G.hao configuration in Framer SAR
 */
shr_error_e
flexe_std_gen2_sar_client_ghao_config_set(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    uint32 bp_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_encap_get(unit, client_channel, speed, 1, &bp_id));
        /*
         * Start G.hao in rate adapter
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_ghao_resize_new1
                                   (unit, LC_RATEADP_MODULE_FLEXE2, client_channel, speed));
        /*
         * Adjust LCR
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_sar_tx_traffic_ghao_lcr_cfg(unit, client_channel));
        /*
         * Adjust BWR
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_sar_tx_traffic_ghao_bwr_cfg(unit, client_channel, bp_id));
        /*
         * Complete G.hao in rate adapter
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_ghao_resize_new2
                                   (unit, LC_RATEADP_MODULE_FLEXE2, client_channel, speed));
        /*
         * Update SW
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_ghao_resize_new3
                                   (unit, LC_RATEADP_MODULE_FLEXE2, client_channel, speed));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_encap_get(unit, client_channel, speed, 0, &bp_id));
        /*
         * Adjust LCR
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_sar_rx_traffic_ghao_lcr_cfg(unit, client_channel, bp_id));
        /*
         * Adjust BWR
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_sar_rx_traffic_ghao_bwr_cfg(unit, client_channel, bp_id));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the demux schedule active calendar
 *    control, from C bit or register
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - group index
 * \param [in] type - control type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_demux_sch_calendar_control_set(
    int unit,
    int group_index,
    flexe_std_gen2_sch_cal_control_type_t type)
{
    int index, nof_logical_phys;
    uint32 ab_enable[1] = { 0 };
    uint32 logical_phys[1] = { 0 };
    flexe_demux1_core_info_t **demux_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(demux_info, sizeof(flexe_demux1_core_info_t *), "demux_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_demux1_core_algorithm_chip_info_get(unit, demux_info));
    /*
     * Get all the logical PHYs
     */
    nof_logical_phys = dnx_data_nif.flexe.nof_flexe_lphys_get(unit);
    for (index = 0; index < nof_logical_phys; index++)
    {
        if (group_index == (*demux_info)->des_phy[index].group_id)
        {
            SHR_BITSET(logical_phys, index);
        }
    }
    /*
     * Get original value
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_demux_reg_get
                               (unit, TUNNEL_J2X_FLEXE_DEMUX_M0, AB_ENABLE_1, (FRAMER_U32 *) ab_enable));
    if (type == CAL_CTRL_BY_C_BIT)
    {
        SHR_BITOR_RANGE(ab_enable, logical_phys, 0, nof_logical_phys, ab_enable);
    }
    else
    {
        SHR_BITREMOVE_RANGE(ab_enable, logical_phys, 0, nof_logical_phys, ab_enable);
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_flexe_demux_reg_set
                               (unit, TUNNEL_J2X_FLEXE_DEMUX_M0, AB_ENABLE_1, ab_enable[0]));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Switch the FlexE calendar from A to B
 *    or B-> A
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - group index
 * \param [in] cal_id - target calendar ID
 * \param [in] client_channel - the client channel with speed change
 * \param [in] flags - flags, rx or tx
 * \param [in] speed - new client speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_flexe_client_ghao_cal_switch(
    int unit,
    int group_index,
    int cal_id,
    int client_channel,
    uint32 flags,
    int speed)
{
    int direction, working_cal_id;
    flexe_demux1_core_info_t **demux_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_ALLOC_SET_ZERO(demux_info, sizeof(flexe_demux1_core_info_t *), "demux_info",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_demux1_core_algorithm_chip_info_get(unit, demux_info));
        /*
         * Make sure flexe and schedule calendar work at the same calendar ID
         */
        working_cal_id = (cal_id == 0) ? 1 : 0;
        if (working_cal_id != (*demux_info)->group_info[group_index].schedule_ab)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_demux1_core_schedule_cfg
                                       (unit, group_index, 0, working_cal_id, 1));
        }
        /*
         * Use C bit to control the calendar
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_demux_sch_calendar_control_set(unit, group_index, CAL_CTRL_BY_C_BIT));
        /*
         * Update the standby schedule calendar according to the new calendar ID
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_demux1_core_schedule_cfg(unit, group_index, 0, cal_id, 1));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Update schedule calendar
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_mux1_core_schedule_config(unit, group_index, 0, cal_id, 1));
        if (!SAL_BOOT_PLISIM)
        {
            /*
             * Update rate in rate adpater and Trigger G.hao in rate adapter
             */
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_ghao_resize_new1
                                       (unit, LC_RATEADP_MODULE_FLEXE1, client_channel, speed));
        }
        /*
         * Trigger Cal switching in MUX
         */
        direction = (cal_id == 0) ? 1 : 0;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_mux1_core_c_cfg(unit, group_index, direction));
    }
exit:
    SHR_FREE(demux_info);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Complete the G.hao procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - group index
 * \param [in] client_channel - the client channel with speed change
 * \param [in] flags - flags, rx or tx
 * \param [in] speed - new client speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_flexe_client_ghao_complete(
    int unit,
    int group_index,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Use register to control the calendar
         * The active calendar ID in register has been updated
         * during calendar switching
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_demux_sch_calendar_control_set(unit, group_index, CAL_CTRL_BY_REGISTER));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        if (!SAL_BOOT_PLISIM)
        {
            /*
             * Mark G.hao as Done
             */
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_ghao_resize_new2
                                       (unit, LC_RATEADP_MODULE_FLEXE1, client_channel, speed));
            /*
             * Update SW
             */
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_rateadp_ghao_resize_new3
                                       (unit, LC_RATEADP_MODULE_FLEXE1, client_channel, speed));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Stop the G.hao procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - group index
 * \param [in] cal_id - the target calendar ID
 * \param [in] flags - flags, rx or tx
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_flexe_client_ghao_stop(
    int unit,
    int group_index,
    int cal_id,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Switch back to the orignal calendar
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_demux1_core_schedule_cfg(unit, group_index, 0, cal_id, 1));
        /*
         * Use register to control the calendar
         * The active calendar ID in register has been updated
         * during calendar switching
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_demux_sch_calendar_control_set(unit, group_index, CAL_CTRL_BY_REGISTER));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get all the 1588 MAC filters
 *
 * \param [in] unit - chip unit id
 * \param [in] port_1588 - the 1588 port in framer
 * \param [out] filters - filters array
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_l1_eth_1588_mac_rx_filter_get(
    int unit,
    int port_1588,
    uint16 *filters)
{
    uint32 tmp_filter;
    int count;
    int filter_fields[] = {
        MACRX_SVLAN_FOR_1588_CFG,
        MACRX_CVLAN_FOR_1588_CFG,
        MACRX_ETYPE_FOR_1588_CFG,
        MACRX_ETYPE_IPV4_FOR_1588_CFG,
        MACRX_ETYPE_IPV6_FOR_1588_CFG,
        MACRX_ETYPE_SSM_FOR_1588_CFG,
        MACRX_ETYPE_SUB_TYPE_FOR_1588_CFG,
        MACRX_VLAN_FILTER_OF_PTP_CFG,
        MACRX_LAYER_FLITER_OF_PTP_CFG,
        MACRX_VLAN_FILTER_OF_SSM_CFG,
        MACRX_UNI_DA_PARSE_EN
    };
    SHR_FUNC_INIT_VARS(unit);

    for (count = 0; count < COUNTOF(filter_fields); ++count)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_mac1_1588_by_port_cfg_get
                                   (unit, TUNNEL_J2X_MAC1_RX, port_1588, filter_fields[count],
                                    (FRAMER_U32 *) & tmp_filter));
        filters[count] = tmp_filter;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure MAC1 and MAC2 for L1 ETH 1588
 *
 * \param [in] unit - chip unit id
 * \param [in] eth_channel - the flexe1 channel for L1 ETH
 * \param [in] mac1_channel - The channel for MAC1
 * \param [in] mac2_channel - The channel for MAC2
 * \param [in] speed - port speed
 * \param [in] flags - Rx or Tx flags
 * \param [in] mac1_15888_cfg - 1588 configurations
 * \param [in] enable - enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
flexe_std_gen2_framer_mac1_mac2_cfg(
    int unit,
    int eth_channel,
    int mac1_channel,
    int mac2_channel,
    int speed,
    uint32 flags,
    LC_BP_MAC1_1588_INFO_T * mac1_15888_cfg,
    int enable)
{
    LC_ENV_BUSB_MAC1_CFG_INFO_T env_mac1_cfg_inf;
    LC_ENV_MAC2_CFG_INFO_T env_mac2_cfg_inf;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&env_mac1_cfg_inf, 0, sizeof(LC_ENV_BUSB_MAC1_CFG_INFO_T));
    sal_memset(&env_mac2_cfg_inf, 0, sizeof(LC_ENV_MAC2_CFG_INFO_T));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * mac1 rx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_mac1_rx_traffic_cfg
                                   (unit, enable, mac1_channel, speed, mac1_15888_cfg));
        /*
         * fpb_mac_rx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_fpb_mac_rx_calendar_cfg(unit, enable, mac1_channel, speed));
        /*
         * pkt_cpb_egress
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_pkt_cpb_egress_traffic_cfg(unit, enable, mac1_channel, 2, speed));
        /*
         * mac2 tx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_mac2_tx_traffic_cfg(unit, enable, mac1_channel, mac2_channel, speed));
        /*
         * mac2 env, 0xffff means invalid values
         */
        env_mac2_cfg_inf.ch_rate = speed;
        env_mac2_cfg_inf.ch_mapa = 0xFFFF;
        env_mac2_cfg_inf.ch_mapb = 0xFFFF;
        env_mac2_cfg_inf.reverse_ch_map = 0xFFFF;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_env_mac2_cfg(unit, enable, mac1_channel, &env_mac2_cfg_inf, 1));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Mac1 env, the clock source should be from FlexE1 rate adapter
         * 0xffff means invalid values
         */
        env_mac1_cfg_inf.ch_rate = speed;
        env_mac1_cfg_inf.src_sel = MAC_ENV_SRC_CLK_FROM_FLEXE1;
        env_mac1_cfg_inf.ch_mapa = eth_channel;
        env_mac1_cfg_inf.ch_mapb = 0xFFFF;
        env_mac1_cfg_inf.reverse_ch_map = eth_channel;
        env_mac1_cfg_inf.oduflex_type = 0xFFFF;
        env_mac1_cfg_inf.env_mac_num = 0xFFFF;
        env_mac1_cfg_inf.ch_mapc = 0xFFFF;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_env_mac1_busb_cfg
                                   (unit, enable, 0, mac1_channel, &env_mac1_cfg_inf, 1));
        /*
         * mac1 tx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_mac1_tx_traffic_cfg
                                   (unit, enable, mac1_channel, speed, 1, mac1_15888_cfg->port_1588));
        /*
         * fpb_mac_tx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_fpb_mac_tx_calendar_cfg(unit, enable, mac1_channel, speed));
        /*
         * pkt cpb ingress
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_pkt_cpb_ingress_traffic_cfg(unit, enable, mac1_channel, 1, speed));
        /*
         * mac2 rx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_mac2_rx_traffic_cfg(unit, enable, mac1_channel, mac2_channel, speed));
        /*
         * stat tx
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_stat_tx_monitor_mode_cfg(unit, mac1_channel, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure 1588 for L1 ETH, need to configure path between MAC1 and MAC2 first,
 *     then configure 1588 in MAC1.
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - Rx or Tx
 * \param [in] eth_1588_info - 1588 info
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_l1_eth_1588_enable_set(
    int unit,
    uint32 flags,
    const flexe_core_l1_eth_1588_info_t * eth_1588_info,
    int enable)
{
    LC_BP_MAC1_1588_INFO_T mac1_info;
    flexe_core_66bswitch_channel_info_t eth_info, mac_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mac1_info, 0, sizeof(LC_BP_MAC1_1588_INFO_T));
    /*
     * Get configured filters
     */
    mac1_info.port_1588 = eth_1588_info->ptp_port;
    /*
     * Filters are only relevant for Rx
     */
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_l1_eth_1588_mac_rx_filter_get
                        (unit, eth_1588_info->ptp_port, (uint16 *) mac1_info.value_1588));
        /*
         * Update filters
         */
        mac1_info.value_1588[MACRX_VLAN_FILTER_OF_PTP_CFG] = enable ? MACRX_1588_VLAN_FILTER_DISABLE : 0;
        mac1_info.value_1588[MACRX_LAYER_FLITER_OF_PTP_CFG] = enable ? MACRX_1588_LAYER_FILTER_DISABLE : 0;
        mac1_info.value_1588[MACRX_VLAN_FILTER_OF_SSM_CFG] = enable ? MACRX_SSM_VLAN_FILTER_DISABLE : 0;
        mac1_info.value_1588[MACRX_UNI_DA_PARSE_EN] = enable ? MACRX_PARSE_UNICAST_DA_ENABLE : 0;
    }
    /*
     * Configure path between MAC1 and MAC2
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_framer_mac1_mac2_cfg
                    (unit, eth_1588_info->eth_channel, eth_1588_info->mac1_channel, eth_1588_info->mac2_channel,
                     eth_1588_info->speed, flags, &mac1_info, enable));
    /*
     * Connect L1 ETH and MAC1
     */
    eth_info.channel = eth_1588_info->eth_channel;
    SHR_IF_ERR_EXIT(dnx_algo_framer_66bsw_block_id_get(unit, DNX_ALGO_PORT_TYPE_NIF_ETH, 0, &eth_info.block_id));
    mac_info.channel = eth_1588_info->mac1_channel;
    SHR_IF_ERR_EXIT(dnx_algo_framer_inner_mac_block_id_get(unit, 0, 0, &mac_info.block_id));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_66b_switch_set(unit, &eth_info, &mac_info, 0, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_66b_switch_set(unit, &mac_info, &eth_info, 0, enable));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 packet header
 */
shr_error_e
flexe_std_gen2_1588_pkt_header_set(
    int unit,
    uint32 flags,
    uint32 val)
{
    uint32 new_mac_hi, new_mac_lo, dip;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Config MAC
     */
    if (flags &
        (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO | FLEXE_CORE_1588_SRC_MAC_HI |
         FLEXE_CORE_1588_SRC_MAC_LO | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
    {
        /*
         * Get original MAC
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_DA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_SA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_ETH_UNI_DA_W0,
                                            (FRAMER_U32 *) & new_mac_lo));
            }
        }
        else if (flags & FLEXE_CORE_1588_TX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_DA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_SA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_ETH_UNI_DA_W0,
                                            (FRAMER_U32 *) & new_mac_lo));
            }
        }
        if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI))
        {
            new_mac_hi = (val >> 8) & 0xFFFF;
            new_mac_lo = ((new_mac_lo & 0xFFFFFF) | ((val & 0xFF) << 24));
        }
        else if (flags &
                 (FLEXE_CORE_1588_DEST_MAC_LO | FLEXE_CORE_1588_SRC_MAC_LO | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
        {
            new_mac_lo = (new_mac_lo & 0xFF000000) | val;
        }

        if (flags & FLEXE_CORE_1588_RX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_DEST_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                               (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_DA_W1, new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_DA_W0, new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_SRC_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                               (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_SA_W1, new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_SA_W0, new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                if (flags & FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                               (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_ETH_UNI_DA_W1, new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_ETH_UNI_DA_W0, new_mac_lo));
            }
        }
        else if (flags & FLEXE_CORE_1588_TX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_DEST_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                               (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_DA_W1, new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_DA_W0, new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_SRC_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                               (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_SA_W1, new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_SA_W0, new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                if (flags & FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                               (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_ETH_UNI_DA_W1, new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_ETH_UNI_DA_W0, new_mac_lo));
            }
        }
    }
    else
    {
        /*
         * IP configuration
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                       (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_IPV4_UNI_DA, (FRAMER_U32 *) & dip));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                       (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_IPV4_UNI_DA, (FRAMER_U32 *) & dip));
        }
        if (flags & FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_HI)
        {
            dip &= 0xffff;
            dip |= ((val & 0xffff) << 16);
        }
        else
        {
            dip &= 0xffff0000;
            dip |= (val & 0xffff);
        }
        if (flags & FLEXE_CORE_1588_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_set
                                       (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_IPV4_UNI_DA, dip));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_set
                                       (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_IPV4_UNI_DA, dip));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 packet header
 */
shr_error_e
flexe_std_gen2_1588_pkt_header_get(
    int unit,
    uint32 flags,
    uint32 *val)
{
    uint32 new_mac_hi, new_mac_lo, dip;

    SHR_FUNC_INIT_VARS(unit);

    if (flags &
        (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO | FLEXE_CORE_1588_SRC_MAC_HI |
         FLEXE_CORE_1588_SRC_MAC_LO | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
    {
        if (flags & FLEXE_CORE_1588_RX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_DEST_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                               (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_DA_W1,
                                                (FRAMER_U32 *) & new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_DA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_SRC_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                               (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_SA_W1,
                                                (FRAMER_U32 *) & new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_SA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                if (flags & FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                               (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_ETH_UNI_DA_W1,
                                                (FRAMER_U32 *) & new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_ETH_UNI_DA_W0,
                                            (FRAMER_U32 *) & new_mac_lo));
            }
        }
        else if (flags & FLEXE_CORE_1588_TX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_DEST_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                               (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_DA_W1,
                                                (FRAMER_U32 *) & new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_DA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                if (flags & FLEXE_CORE_1588_SRC_MAC_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                               (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_SA_W1,
                                                (FRAMER_U32 *) & new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_SA_W0, (FRAMER_U32 *) & new_mac_lo));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                if (flags & FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                               (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_ETH_UNI_DA_W1,
                                                (FRAMER_U32 *) & new_mac_hi));
                }
                FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                           (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_ETH_UNI_DA_W0,
                                            (FRAMER_U32 *) & new_mac_lo));
            }
        }
        if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI))
        {
            *val = (new_mac_hi << 8) | ((new_mac_lo >> 24) & 0xFF);
        }
        else
        {
            *val = new_mac_lo & 0xFFFFFF;
        }
    }
    else
    {
        /*
         * Get unicast IP
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_rx_cfg_reg_get
                                       (unit, TUNNEL_J2X_CCU_RX_CFG, CCU_RX_IPV4_UNI_DA, (FRAMER_U32 *) & dip));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ccu_tx_cfg_reg_get
                                       (unit, TUNNEL_J2X_CCU_TX_CFG, CCU_TX_IPV4_UNI_DA, (FRAMER_U32 *) & dip));
        }
        if (flags & FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_HI)
        {
            *val = ((dip >> 16) & 0xffff);
        }
        else
        {
            *val = (dip & 0xffff);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the STAT MAC1 channel for
 *    FlexE client
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - FlexE client channel
 * \param [in] stat_channel - MAC1 STAT channel
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_stat_channel_config_set(
    int unit,
    int client_channel,
    int stat_channel,
    int enable)
{
    flexe_core_66bswitch_channel_info_t src_info, dest_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure stat Rx
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_stat_rx_traffic_cfg(unit, enable, stat_channel, LC_BP_INVLAND_CH, 0));
    /*
     * Connect client channel to stat channel
     */
    src_info.channel = client_channel;
    SHR_IF_ERR_EXIT(dnx_algo_framer_66bsw_block_id_get(unit, DNX_ALGO_PORT_TYPE_FLEXE_CLIENT, 0, &src_info.block_id));
    dest_info.channel = stat_channel;
    SHR_IF_ERR_EXIT(dnx_algo_framer_inner_mac_block_id_get(unit, 0, 1, &dest_info.block_id));
    /*
     * Configure 66b switching
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_66b_switch_set(unit, &src_info, &dest_info, 0, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get MAC1 stat values
 *
 * \param [in] unit - chip unit id
 * \param [in] stat_channel - MAC1 STAT channel
 * \param [out] counter_val - STAT values
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
flexe_std_gen2_stat_mac_counter_get(
    int unit,
    int stat_channel,
    uint64 *counter_val)
{
    int index;
    FRAMER_U32 tmp_val[2];
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get counters from Framer core
     */
    for (index = 0; index < flexe_core_mac_Count; ++index)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_stat_rx_frame_cnt_ram_get(unit, 0, stat_channel, index, tmp_val));
        COMPILER_64_SET(counter_val[index], tmp_val[1], tmp_val[0]);
    }
    /*
     * Clear counters
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_stat_rx_reg_field_set
                               (unit, 0, STATRX_CNT_CLR_CHAN_CFG, stat_rx_lc_statrx_cnt_clr_chan_cfg, stat_channel));
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_stat_rx_reg_set(unit, 0, STATRX_CNT_CLR, 2));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure OH/OAM MACs
 */
static shr_error_e
flexe_std_gen2_oh_oam_mac_config(
    int unit,
    uint32 flags,
    int field_id,
    uint32 val)
{
    uint32 mac[2] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ohif_flexe_upi_da_sa_3th_cfg_get
                               (unit, TUNNEL_J2X_OHIF_FLEXE, field_id, (FRAMER_U32 *) mac));

    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_SRC_MAC_HI))
    {
        mac[1] = (val >> 8) & 0xFFFF;
        mac[0] = ((mac[0] & 0xFFFFFF) | ((val & 0xFF) << 24));
    }
    else
    {
        mac[0] = (mac[0] & 0xFF000000) | (val & 0xFFFFFF);
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ohif_flexe_upi_da_sa_3th_cfg_set
                               (unit, TUNNEL_J2X_OHIF_FLEXE, field_id, (FRAMER_U32 *) mac));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Set FlexE oh/oam packet MAC
 */
shr_error_e
flexe_std_gen2_oh_oam_pkt_mac_set(
    int unit,
    uint32 flags,
    uint32 val)
{
    int field_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure extraction direction. From Framer to external processor
     */
    field_id = (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_DEST_MAC_LO)) ? 0 : 1;
    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_oam_mac_config(unit, flags, field_id, val));
    /*
     * Configure insertion direction. From external processor to Framer.
     */
    field_id = (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_DEST_MAC_LO)) ? 3 : 2;
    SHR_IF_ERR_EXIT(flexe_std_gen2_oh_oam_mac_config(unit, flags, field_id, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE oh/oam packet MAC
 */
shr_error_e
flexe_std_gen2_oh_oam_pkt_mac_get(
    int unit,
    uint32 flags,
    uint32 *val)
{
    int field_id;
    uint32 mac[2];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get original MAC
     */
    field_id = (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_DEST_MAC_LO)) ? 0 : 1;
    FLEXE_STD_GEN2_IF_ERR_EXIT(framer_uas_j2x_ohif_flexe_upi_da_sa_3th_cfg_get
                               (unit, TUNNEL_J2X_OHIF_FLEXE, field_id, (FRAMER_U32 *) mac));
    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_SRC_MAC_HI))
    {
        *val = ((mac[1] << 8) | ((mac[0] >> 24) & 0xFF));
    }
    else
    {
        *val = (mac[0] & 0xFFFFFF);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Delete TS for OAM Rx
 */
shr_error_e
flexe_std_gen2_oam_timeslot_delete(
    int unit,
    int client_channel,
    uint32 flags)
{
    uint32 oam_speed;
    LC_FLEXE_OAM_TS_MODE_TYPE ts_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_rate_query
                                   (unit, 0, client_channel, FRAMER_DIR_IN, (FRAMER_U32 *) & oam_speed));
        if (oam_speed != LC_FLEXE_OAM_RATE_DEFAULT_VALUE)
        {
            ts_mode =
                (dnx_data_nif.flexe.device_slot_mode_get(unit) ==
                 DNX_FLEXE_SLOT_MODE_1G_AND_5G) ? OAM_TS_MODE_1G : OAM_TS_MODE_1G25;
            FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_rx_ts_del(unit, 0, ts_mode, client_channel));
        }
    }
    /*
     * Configure OAM request scheduler
     */
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_ohif_flexe_oam_ch_cfg(unit, OHIF_OAM1, client_channel, 0));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_ohif_flexe_oam_calendar_cfg(unit, OHIF_OAM1));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure TS for OAM Rx
 */
shr_error_e
flexe_std_gen2_oam_timeslot_add(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    int nof_slots;
    LC_FLEXE_OAM_TS_MODE_TYPE ts_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        ts_mode =
            (dnx_data_nif.flexe.device_slot_mode_get(unit) ==
             DNX_FLEXE_SLOT_MODE_1G_AND_5G) ? OAM_TS_MODE_1G : OAM_TS_MODE_1G25;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_flexe_oam_rx_ts_add(unit, 0, ts_mode, client_channel, speed));
    }
    /*
     * Configure OAM request scheduler
     */
    if (flags & BCM_PORT_FLEXE_TX)
    {
        nof_slots = speed / FLEXE_OAM_REQ_CALENDAR_GRANULARITY;
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_ohif_flexe_oam_ch_cfg(unit, OHIF_OAM1, client_channel, nof_slots));
        FLEXE_STD_GEN2_IF_ERR_EXIT(framer_sal_j2x_ohif_flexe_oam_calendar_cfg(unit, OHIF_OAM1));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Common register read function for Framer
 */
shr_error_e
flexe_std_gen2_register_get(
    int unit,
    uint32 module_id,
    uint32 reg_id,
    uint32 field_id,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_field_read(unit, module_id, reg_id, field_id, (FRAMER_U32 *) value));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Common memory read function for Framer
 */
shr_error_e
flexe_std_gen2_memory_get(
    int unit,
    uint32 module_id,
    uint32 reg_id,
    uint32 field_id,
    uint32 index,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_ram_field_read(unit, module_id, reg_id, index, field_id, (FRAMER_U32 *) value));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Perform soft reset in Framer
 */
shr_error_e
flexe_std_gen2_soft_reset(
    int unit,
    int in_reset)
{
    int val;

    SHR_FUNC_INIT_VARS(unit);

    val = in_reset ? 0 : 1;

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE1_OH_TOP, FLEXE1_OH_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE2_OH_TOP, FLEXE2_OH_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE_DEMUX_TOP_M0, FLEXE_DEMUX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE_DEMUX_TOP_M1, FLEXE_DEMUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE_AWARE_RX_TOP, FLEXE_AWARE_RX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE_AWARE_TX_TOP, FLEXE_AWARE_TX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_B66_SW_TOP, B66_SW_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE_MUX_TOP_M0, FLEXE_MUX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXE_MUX_TOP_M1, FLEXE_MUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_RATE_ADP_TOP_M0, FRA_GLOBAL_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_RATE_ADP_TOP_M1, FRA_GLOBAL_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_MAP_TOP, MAP_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_DEMAP_TOP, FDMP_GLOBAL_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OAM_TOP_M0, OAM_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OAM_TOP_M1, OAM_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_SAR_TOP, FSAR_GLOBAL_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_MAC_RX_TOP, MAC_RX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_MAC_TX_TOP, MAC_TX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_GLOBAL_IEEE1588_RX, IEEE1588_RX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_GLOBAL_IEEE1588_TX, IEEE1588_TX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_EGRESS_CPB_TOP, EGRESS_CPB_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_INGRESS_CPB_TOP, INGRESS_CPB_GLB_LOGIC_RST, val));

#ifdef INCLUDE_FLEXE_OTN
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LANE_FEC_RX_TOP_M0, LANE_FEC_RX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LANE_FEC_RX_TOP_M1, LANE_FEC_RX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LANE_FEC_RX_TOP_M2, LANE_FEC_RX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LANE_FEC_RX_TOP_M3, LANE_FEC_RX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LANE_FEC_TX_TOP_M0, LANE_FEC_TX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LANE_FEC_TX_TOP_M1, LANE_FEC_TX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_ODU_SW_TOP, ODU_SW_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_PHYMUX_RX_TOP, PHYMUX_RX_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_PHYMUX_TX_TOP, PHYMUX_TX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_LINE_OTN_TOP, LINE_OTN_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_FLEXO_TOP, FPB_FLEXO_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPUC_DEMUX_TOP, OPUC_DEMUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU4_DEMUXI_TOP, OPU4_DEMUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU2_DEMUX_TOP, OPU2_DEMUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU25_DEMUX_TOP, OPU25_DEMUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPUC_MUX_TOP, OPUC_MUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU4_MUXI_TOP, OPU4_MUXI_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU4_MUXJ_TOP, OPU4_MUXJ_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU2_MUXI_TOP, OPU2_MUXI_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU25_MUX_TOP, OPU25_MUX_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_GFP_DEMAP_TOP, GFP_DEMAP_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_ENCRYPT_LINE_TOP, ENCRYPT_LINE_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_DECRYPT_LINE_TOP, DECRYPT_LINE_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_ENCRYPT_SAR_TOP, ENCYPT_SAR_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_DECRYPT_SAR_TOP, DECRYPT_SAR_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_ODU_OH_TOP_M0, ODU_OH_GLB_LOGIC_RST, val));
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_ODU_OH_TOP_M1, ODU_OH_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU2_MUXJ_TOP, OPU2_MUXJ_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU4_MUXK_TOP, OPU4_MUXK_GLB_LOGIC_RST, val));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_reg_write(unit, MODULE_J2X_OPU4_DEMUXJ_TOP, OPU4_DEMUXJ_GLB_LOGIC_RST, val));
#endif

exit:
    SHR_FUNC_EXIT;
}

#else

shr_error_e
flexe_std_gen2_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_sync(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_group_id_set(
    int unit,
    int flexe_core_port,
    int group_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_group_id_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *group_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_group_phy_add(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp,
    const int *logical_phy_id,
    const flexe_core_port_info_t * port_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_group_phy_remove(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_mac_client_add(
    int unit,
    int client_channel,
    int mgmt_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_mac_client_delete(
    int unit,
    int client_channel,
    int mgmt_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_gen2_sar_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_sar_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_rateadpt_get(
    int unit,
    int client_channel,
    flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_rateadpt_add(
    int unit,
    int client_channel,
    const flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_rateadpt_delete(
    int unit,
    int client_channel,
    const flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_sar_channel_map_set(
    int unit,
    uint32 flags,
    const flexe_core_sar_channel_map_info_t * map_info,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_66b_switch_set(
    int unit,
    const flexe_core_66bswitch_channel_info_t * src_info,
    const flexe_core_66bswitch_channel_info_t * dest_info,
    int index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_66b_switch_get(
    int unit,
    const flexe_core_66bswitch_channel_info_t * src_info,
    flexe_core_66bswitch_channel_info_t * dest_info,
    int *nof_dest)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_logical_phy_id_set(
    int unit,
    int flexe_core_port,
    int speed,
    int logical_phy_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_logical_phy_id_get(
    int unit,
    int flexe_core_port,
    int speed,
    uint32 flags,
    int *logical_phy_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_client_id_set(
    int unit,
    int cal_id,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    const int *calendar_slots)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_client_id_get(
    int unit,
    int cal_id,
    uint32 flags,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    int *calendar_slots)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_cr_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_cr_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_ca_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_ca_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_c_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_c_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_sc_bit_set(
    int unit,
    int flexe_core_port,
    int sync_config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_sc_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *sync_config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_active_calendar_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_active_calendar_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_alarm_status_get(
    int unit,
    int flexe_core_port,
    uint16 *alarm_status)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oh_cal_mismatch_ts_get(
    int unit,
    int flexe_core_port,
    bcm_port_flexe_time_slot_t * time_slots)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_phy_default_config_set(
    int unit,
    int flexe_core_port,
    int local_port,
    int speed,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_busa_channel_map_set(
    int unit,
    uint32 flags,
    const flexe_core_busa_channel_map_info_t * channel_map_info,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_alarm_status_get(
    int unit,
    int flexe_core_port,
    uint16 *alarms_status)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}

/*Enable/disable OAM bypass for FlexE client*/
shr_error_e
flexe_std_gen2_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_rdi_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_rdi_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}

/*Configure SD BIP8 error set threshold for FlexE client*/
shr_error_e
flexe_std_gen2_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_gen2_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bas_bip_recal_enable_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bas_bip_recal_enable_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bas_period_mismatch_trigger_los_alarm_set(
    int unit,
    int val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_bas_period_mismatch_trigger_los_alarm_get(
    int unit,
    int *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_deskew_alarm_trigger_lf_set(
    int unit,
    int group_index,
    int val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_deskew_alarm_trigger_lf_get(
    int unit,
    int group_index,
    int *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_sar_cell_mode_per_channel_set(
    int unit,
    int client_channel,
    int is_ingress,
    int cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Enable/disable SAR client
 */
shr_error_e
flexe_std_gen2_sar_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get SAR client enable/disable status
 */
shr_error_e
flexe_std_gen2_sar_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get 66b switching info from dest info
 */
shr_error_e
flexe_std_gen2_66b_switch_reverse_get(
    int unit,
    const flexe_core_66bswitch_channel_info_t * dest_info,
    flexe_core_66bswitch_channel_info_t * src_info,
    int *nof_src)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure FLR rate generator divider
 */
shr_error_e
flexe_std_gen2_flr_rate_generator_divider_set(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure FLR logical lane to serdes physical lane mapping
 */
shr_error_e
flexe_std_gen2_flr_logical_to_serdes_lane_set(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - G.hao configuration for MAC client
 */
shr_error_e
flexe_std_gen2_mac_client_ghao_config_set(
    int unit,
    int client_channel,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - G.hao configuration in Framer SAR
 */
shr_error_e
flexe_std_gen2_sar_client_ghao_config_set(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Map Framer channel to mgmt channel
 */
shr_error_e
flexe_std_gen2_mac_mgmt_channel_mapping_set(
    int unit,
    int client_channel,
    int mgmt_channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the slots of FlexE calendar
 *    into schedule calendar and switch the schedule
 *    calendar
 */
shr_error_e
flexe_std_gen2_schedule_calendar_switch(
    int unit,
    int group_index,
    uint32 flags,
    int flexe_cal)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Switch the FlexE calendar from A to B
 *    or B-> A
 */
shr_error_e
flexe_std_gen2_flexe_client_ghao_cal_switch(
    int unit,
    int group_index,
    int cal_id,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Complete the G.hao procedure
 */
shr_error_e
flexe_std_gen2_flexe_client_ghao_complete(
    int unit,
    int group_index,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Stop the G.hao procedure
 */
shr_error_e
flexe_std_gen2_flexe_client_ghao_stop(
    int unit,
    int group_index,
    int cal_id,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure 1588 for L1 ETH
 */
shr_error_e
flexe_std_gen2_l1_eth_1588_enable_set(
    int unit,
    uint32 flags,
    const flexe_core_l1_eth_1588_info_t * eth_1588_info,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 packet header
 */
shr_error_e
flexe_std_gen2_1588_pkt_header_set(
    int unit,
    uint32 flags,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 packet header
 */
shr_error_e
flexe_std_gen2_1588_pkt_header_get(
    int unit,
    uint32 flags,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the STAT MAC1 channel for
 *    FlexE client
 */
shr_error_e
flexe_std_gen2_stat_channel_config_set(
    int unit,
    int client_channel,
    int stat_channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get MAC1 stat values
 */
shr_error_e
flexe_std_gen2_stat_mac_counter_get(
    int unit,
    int mac_channel,
    uint64 *counter_val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE oh/oam packet MAC
 */
shr_error_e
flexe_std_gen2_oh_oam_pkt_mac_set(
    int unit,
    uint32 flags,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE oh/oam packet MAC
 */
shr_error_e
flexe_std_gen2_oh_oam_pkt_mac_get(
    int unit,
    uint32 flags,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure TS for OAM Rx
 */
shr_error_e
flexe_std_gen2_oam_timeslot_delete(
    int unit,
    int client_channel,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure TS for OAM Rx
 */
shr_error_e
flexe_std_gen2_oam_timeslot_add(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Common register read function for Framer
 */
shr_error_e
flexe_std_gen2_register_get(
    int unit,
    uint32 module_id,
    uint32 reg_id,
    uint32 field_id,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Common memory read function for Framer
 */
shr_error_e
flexe_std_gen2_memory_get(
    int unit,
    uint32 module_id,
    uint32 reg_id,
    uint32 field_id,
    uint32 index,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Framer Lib is not compiled!.\r\n");
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Perform soft reset in Framer
 */
shr_error_e
flexe_std_gen2_soft_reset(
    int unit,
    int in_reset)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif
/* } */
