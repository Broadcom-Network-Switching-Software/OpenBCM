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
#include <j2x_sal_framer_flexe_mux1_core.h>
#include <j2x_sal_framer_subsys_map_otn_adapter.h>
#include <j2x_sal_framer_subsys_bp_adapter.h>
#include <j2x_sal_framer_subsys_flexe_adapter.h>
#include <j2x_sal_framer_flexe_oam.h>
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

#define FLEXE_STD_GEN2_NOF_CLIENT_ID_RAMS_PER_INSTANCE               (5)
#define FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM                   (20)
#define FLEXE_STD_GEN2_MAX_NOF_SLOTS_PER_INSTANCE                    (100)
#define FLEXE_STD_GEN2_FLEXE_PHYMAP_OCTETS                           (32)
#define FLEXE_STD_GEN2_VIRTUAL_FLEXE_MODULE_ID                       (1)

#define FLEXE_STD_GEN2_INIT_DEVICE_SLOT_MODE_SHIFT                   (0)
#define FLEXE_STD_GEN2_INIT_TYPE_SHIFT                               (28)

#define FLEXE_STD_GEN2_OAM_TYPE_SHIFT                                (8)
#define FLEXE_STD_GEN2_OAM_DEF_PERIOD                                (3)

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

typedef enum flexe_std_gen2_framer_init_mode_s
{
    FRAMER_INIT_COMPONENT = 0,
    FRAMER_INIT_ALL,
    FRAMER_INIT_WITHOUT_RAM
} flexe_std_gen2_framer_init_mode_t;
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

/*initialize FlexE framer j2x access register.*/
static shr_error_e
flexe_std_gen2_access_init(
    int unit)
{
    J2X_UAS_DRV_T *j2x_uas_drv = NULL;
    J2X_RET rt;

    SHR_FUNC_INIT_VARS(unit);
    j2x_uas_drv = sal_alloc(sizeof(J2X_UAS_DRV_T), "J2X_UAS_DRV_T");
    if (j2x_uas_drv == NULL)
    {
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

    j2x_uas_drv->read = dnx_framer_reg_read;
    j2x_uas_drv->write = dnx_framer_reg_write;
    j2x_uas_drv->ram_read = dnx_framer_ram_read;
    j2x_uas_drv->ram_write = dnx_framer_ram_write;
    rt = j2x_uas_drv_register(unit, j2x_uas_drv);
    if (rt != 0)
    {
        sal_free(j2x_uas_drv);
        j2x_uas_drv = NULL;
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    dnx_drv_soc_info_framer_access_drv_set(unit, j2x_uas_drv);
    SHR_FUNC_EXIT;
}

/*De-initialize FlexE framer j2x access register.*/
static shr_error_e
flexe_std_gen2_access_deinit(
    int unit)
{
    J2X_UAS_DRV_T *j2x_uas_drv = NULL;
    J2X_RET rt;

    SHR_FUNC_INIT_VARS(unit);
    rt = j2x_uas_drv_unregister(unit);
    if (rt != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    j2x_uas_drv = dnx_drv_soc_info_framer_access_drv_get(unit);
    if (j2x_uas_drv != NULL)
    {
        sal_free(j2x_uas_drv);
        j2x_uas_drv = NULL;
        dnx_drv_soc_info_framer_access_drv_set(unit, j2x_uas_drv);
    }

exit:
    SHR_FUNC_EXIT;
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
    uint32 flags = 0;
    RST_TYPE init_type;

    SHR_FUNC_INIT_VARS(unit);

    if (sw_state_is_warm_boot(unit))
    {
    }
    init_type = J2X_RST_COLD;
    /*
     * temporarily enable SChan access. Will remove it after updating deinit and WB codes
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_chip_create(unit, 0));

    SHR_IF_ERR_EXIT(flexe_std_gen2_access_init(unit));
    /*
     * Prepare initializaiton flags
     */
    if (soc_sand_is_emulation_system(unit))
    {
        if (dnx_data_device.emulation.framer_init_mode_get(unit) == FRAMER_INIT_COMPONENT)
        {
            flags |= (J2X_INIT_COMPONENT << FLEXE_STD_GEN2_INIT_TYPE_SHIFT);
        }
        else if (dnx_data_device.emulation.framer_init_mode_get(unit) == FRAMER_INIT_WITHOUT_RAM)
        {
            flags |= (J2X_RAM_INIT_OFF_REG_INIT_ON << FLEXE_STD_GEN2_INIT_TYPE_SHIFT);
        }
    }
    if (dnx_data_nif.flexe.device_slot_mode_get(unit) == DNX_FLEXE_SLOT_MODE_1G_AND_5G)
    {
        flags |= (0x1 << FLEXE_STD_GEN2_INIT_DEVICE_SLOT_MODE_SHIFT);
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_chip_init(unit, init_type, flags));
    /*
     * Configure demux to RR mode.
     * Will remove and use WRR.
     */
    regw(unit, 0x20000, 0x403, 1);

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));
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
     * temporarily enable SChan access. Will remove it after updating deinit and WB codes
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_chip_uninit(unit));

    SHR_IF_ERR_EXIT(flexe_std_gen2_access_deinit(unit));

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_chip_destory(unit));

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
    SHR_FUNC_INIT_VARS(unit);
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_phymap_get
                                   (unit, instance_id, phy_map_offset, (J2X_U32 *) & phy_map[phy_map_offset]));
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_phymap_set
                                       (unit, instance_id, phy_map_offset, clear_phymap ? 0 : phy_map[phy_map_offset]));
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_phymap_rx_exp_set
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_set
                                   (unit, instance_id, flexe1_tx_oh_lc_gid_ins, group_id));
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_exgid_oh_cfg_set
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_get
                                       (unit, instance_id, flexe1_tx_oh_lc_gid_ins, (J2X_U32 *) group_id));
        }
        else if (flags & BCM_PORT_FLEXE_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_id_rx_value_get
                                       (unit, instance_id, flexe1_rx_oh_lc_acgid, (J2X_U32 *) group_id));
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
    uint32 instance_bmp_tmp[1] = { 0 }, instance_bmp[1] =
    {
    0};
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode,
                                    speed_mode, 0, 1));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Configure MUX
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode,
                                    speed_mode, 1, 1));
        /*
         * Configure overhead
         */
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_set
                                       (unit, instance_id, flexe1_tx_oh_lc_pid_ins,
                                        instance_num_array_tmp[instance_id]));
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_exgid_oh_cfg_set
                                       (unit, instance_id, flexe1_rx_oh_lc_expid, instance_num_array_tmp[instance_id]));
        }
        SHR_IF_ERR_EXIT(flexe_std_gen2_oh_phymap_set(unit, group_index, instance_bmp, instance_num_array_tmp, 1));
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
    uint32 instance_bmp[1] = { 0 };
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
        /*
         * Get flexe core_port to instance id mapping
         */
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode, speed_mode,
                                    0, 0));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Configure MUX
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_group_cfg
                                   (unit, group_index, instance_bmp[0], instance_num_array, instance_mode, speed_mode,
                                    1, 0));
        /*
         * Configure overhead
         */
        SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_set
                                       (unit, instance_id, flexe1_tx_oh_lc_pid_ins, 1));
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_exgid_oh_cfg_set
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
    int instance_array[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES] =
        { 0 }, nof_instances, base_instance, total_nof_instances;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));
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
                FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ccab_set
                                           (unit, cal_id, inner_ts_index / 20, instance_array[i], inner_ts_index % 20,
                                            0));
                FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_ccab_rx_exp_set
                                           (unit, cal_id, inner_ts_index / 20, instance_array[i], inner_ts_index % 20,
                                            0));
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (J2X_U32 *) ts_array, nof_slots, 0, cal_id, 1));

    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (J2X_U32 *) ts_array, nof_slots, 1, cal_id, 1));
    }
    /** FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_mode_cfg
                                   (unit, LC_FLEXE1_OAM_MODULE, client_channel, OAM_MODE_SPN_BAS32)); */
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (J2X_U32 *) ts_array, nof_slots, 0, cal_id, 0));

    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_flexe_client
                                   (unit, client_channel, (J2X_U32 *) ts_array, nof_slots, 1, cal_id, 0));
        SHR_IF_ERR_EXIT(flexe_std_gen2_oh_client_id_clear
                        (unit, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));
    }
exit:
    SHR_FREE(ts_array);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add a MAC client to bus B
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
flexe_std_gen2_mac_client_add(
    int unit,
    int client_channel,
    int speed)
{
    uint32 orig_speed;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get original speed
     */
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_env_mac1_busb_rato_query
                               (unit, 1, client_channel, (J2X_U32 *) & orig_speed));
    /*
     * Clear orignal speed if it has been configured
     */
    if (orig_speed != UTILEX_U32_MAX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_busb_cfg(unit, client_channel, orig_speed, 0));
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_busb_cfg(unit, client_channel, speed, 1));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete a MAC client from bus B
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
flexe_std_gen2_mac_client_delete(
    int unit,
    int client_channel,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_busb_cfg(unit, client_channel, speed, 0));
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
        *value = LC_ID_BUSINESS_OFFSET(framer_cell_mode, J2X_ID_FLEXE_CLIENT_5G(ts_num));
    }
    else if (slot_mode == bcmPortFlexePhySlot1G)
    {
        *value = LC_ID_BUSINESS_OFFSET(framer_cell_mode, J2X_ID_FLEXE_CLIENT_1G(ts_num));
    }
    else
    {
        *value = LC_ID_BUSINESS_OFFSET(framer_cell_mode, J2X_ID_FLEXE_CLIENT_1G25(ts_num));
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_sar_tx_query_api(unit, tx_cal));
        *is_configured = (tx_cal->ch_use_flag[client_channel] == SAR_TX_CH_USED) ? 1 : 0;
        *speed = (tx_cal->ch_ts[client_channel]) * dnx_data_nif.sar_client.cal_granularity_get(unit);
    }
    else
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_sar_rx_query_api(unit, rx_cal));
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

    SHR_IF_ERR_EXIT(dnx_algo_port_sar_channel_to_logical_get(unit, client_channel, &sar_logical_port));
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_sar_flexe_client_cfg
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_sar_variable_cfg
                                       (unit, BP_SAR_CLIENT_TYPE, client_channel, is_eth));
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_sar_flexe_client_cfg
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_sar_flexe_client_cfg
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_sar_variable_cfg
                                       (unit, BP_SAR_CLIENT_TYPE, client_channel, is_eth));
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_sar_flexe_client_cfg
                                       (unit, LC_BP_TRAFFIC_ADD, client_channel, peer_channel, bp_id, client_channel, 0,
                                        BP_BCM_TO_SARRX));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Check if rate adapter is configured
 *
 */
static int
flexe_std_gen2_rateadpt_configured_get(
    int unit,
    int mode_id,
    int client_channel,
    int *is_configured)
{
    uint32 rate;

    SHR_FUNC_INIT_VARS(unit);

    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_rateadp_rate_get(unit, mode_id, client_channel, (J2X_U32 *) & rate));
    *is_configured = (rate == LC_RATEADP_RATE_DEFAULT_VALUE) ? 0 : 1;
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
    int speed, is_configured;
    bcm_port_t sar_logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sar_channel_to_logical_get(unit, client_channel, &sar_logical_port));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        /*
         * Always clear configurations
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_rateadpt_configured_get
                        (unit, FLEXE_STD_GEN2_VIRTUAL_FLEXE_MODULE_ID, client_channel, &is_configured));
        if (is_configured)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_map_otn_sar_client_cfg
                                       (unit, 0, peer_channel, client_channel, speed, 0));
        }
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_map_otn_sar_client_cfg
                                       (unit, enable, peer_channel, client_channel, speed, 0));
        }
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, sar_logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
        /*
         * Always clear configurations
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_map_otn_sar_client_cfg
                                   (unit, 0, peer_channel, client_channel, speed, 1));
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_map_otn_sar_client_cfg
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
    int peer_channel;
    bcm_port_t sar_logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sar_channel_to_logical_get(unit, client_channel, &sar_logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_sar_virtual_flexe_channel_get(unit, sar_logical_port, &peer_channel));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config
                        (unit, client_channel, BCM_PORT_FLEXE_RX, speed, peer_channel, 1));
        /*
         * Update rate adapter 2
         */
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_channel_map_internal_config
                        (unit, client_channel, BCM_PORT_FLEXE_RX, peer_channel, 1));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_client_internal_config
                        (unit, client_channel, BCM_PORT_FLEXE_TX, speed, peer_channel, 1));
        SHR_IF_ERR_EXIT(flexe_std_gen2_sar_channel_map_internal_config
                        (unit, client_channel, BCM_PORT_FLEXE_TX, peer_channel, 1));
    }
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
    int is_configured;
    LC_RATEADP_CFG_INFO_T cfg_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cfg_info, 0, sizeof(cfg_info));
    /*
     * Check if rate adapter is configured
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_rateadpt_configured_get(unit, 0, client_channel, &is_configured));
    if (is_configured)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_rateadp_cfg
                                   (unit, J2X_TRAFFIC_REMOVE, rate_adpt_info->module_id, client_channel, &cfg_info));
    }
    cfg_info.ch_rate = rate_adpt_info->speed;
    cfg_info.bus_type = rate_adpt_info->bus_type;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_rateadp_cfg
                               (unit, J2X_TRAFFIC_ADD, rate_adpt_info->module_id, client_channel, &cfg_info));

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
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_rateadp_cfg
                               (unit, J2X_TRAFFIC_REMOVE, rate_adpt_info->module_id, client_channel, &cfg_info));

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
    SHR_IF_ERR_EXIT(dnx_algo_port_sar_channel_to_logical_get(unit, map_info->sar_channel, &sar_logical_port));
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
        SHR_IF_ERR_EXIT(dnx_algo_port_framer_mac_channel_to_logical_get(unit, dest_info->channel, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_cross_set_wrr(unit, src_info->block_id, src_info->channel,
                                                                    dest_info->block_id, dest_info->channel, speed));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_cross_clear_wrr(unit, src_info->block_id, src_info->channel,
                                                                      dest_info->block_id, dest_info->channel, speed));
        }
    }
    else
    {
        if (enable)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_cross_set(unit, src_info->block_id, src_info->channel,
                                                                dest_info->block_id, dest_info->channel));
        }
        else
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_cross_clear(unit, src_info->block_id, src_info->channel,
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
    int dst_block[J2X_MAX_SUPPORT_MUL];
    int dst_channel[J2X_MAX_SUPPORT_MUL];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init channel id
     */
    for (i = 0; i < J2X_MAX_SUPPORT_MUL; ++i)
    {
        dst_channel[i] = CROSS_DEFAULT_VALUE;
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_cross_query
                               (unit, src_info->block_id, src_info->channel, (J2X_U32 *) dst_block,
                                (J2X_U32 *) dst_channel));

    *nof_dest = 0;
    for (i = 0; i < J2X_MAX_SUPPORT_MUL; ++i)
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
    int src_block[J2X_MAX_SUPPORT_MUL];
    int src_channel[J2X_MAX_SUPPORT_MUL];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init channel id
     */
    for (i = 0; i < J2X_MAX_SUPPORT_MUL; ++i)
    {
        src_channel[i] = CROSS_DEFAULT_VALUE;
    }
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_cross_inverse_query
                               (unit, dest_info->block_id, dest_info->channel, (J2X_U32 *) src_block,
                                (J2X_U32 *) src_channel));

    *nof_src = 0;
    for (i = 0; i < J2X_MAX_SUPPORT_MUL; ++i)
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_get
                                   (unit, instance_id, flexe1_tx_oh_lc_pid_ins,
                                    (J2X_U32 *) & ori_instance_num_array[index]));
        /*
         * Update new instance num
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_set
                                   (unit, instance_id, flexe1_tx_oh_lc_pid_ins, instance_num_array[index]));
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_exgid_oh_cfg_set
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_get
                                   (unit, base_instance, flexe1_tx_oh_lc_pid_ins, (J2X_U32 *) & instance_num));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_id_rx_value_get
                                   (unit, base_instance, flexe1_rx_oh_lc_acpid, (J2X_U32 *) & instance_num));
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
    int max_slots_per_instance, i, inner_ts_index;
    int instance_id, instance_index, client_id;
    uint32 instance_bmp[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));

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
                    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ccab_set
                                               (unit, cal_id, inner_ts_index / 20, instance_id, inner_ts_index % 20,
                                                client_id));
                    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_ccab_rx_exp_set
                                               (unit, cal_id, inner_ts_index / 20, instance_id, inner_ts_index % 20,
                                                client_id));
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
    int max_slots_per_instance, i, inner_ts_index;
    int instance_id, instance_index, ts_index;
    uint32 instance_bmp[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port_array[0], &max_slots_per_instance));

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
                    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ccab_get
                                               (unit, cal_id,
                                                inner_ts_index / FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM,
                                                instance_id,
                                                inner_ts_index % FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM,
                                                (J2X_U32 *) & calendar_slots[ts_index]));
                }
                else if (flags & BCM_PORT_FLEXE_RX)
                {
                    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_ccab_rx_get
                                               (unit, cal_id,
                                                inner_ts_index / FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM,
                                                instance_id,
                                                inner_ts_index % FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM,
                                                (J2X_U32 *) & calendar_slots[ts_index]));
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_set(unit, instance_id, field_id, cal_id));
        /*
         * Set expect CCC bit
         */
        if (field_id == flexe1_tx_oh_lc_ccc_ins)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_excc_oh_cfg_set
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_ins_cfg0_get
                                       (unit, instance_id, field_id, (J2X_U32 *) cal_id));
        }
        else if (flags & BCM_PORT_FLEXE_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_cc_rx_value_get
                                       (unit, instance_id, field_id, (J2X_U32 *) cal_id));
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_reserved_set
                                   (unit, instance_id, flexe1_tx_oh_lc_sc_ins, sync_config));
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
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_tx_reserved_get
                                       (unit, instance_id, flexe1_tx_oh_lc_sc_ins, (J2X_U32 *) sync_config));
        }
        else if (flags == BCM_PORT_FLEXE_RX)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_cc_rx_value_get
                                       (unit, instance_id, flexe1_rx_oh_lc_acsc, (J2X_U32 *) sync_config));
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
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_mux1_core_c_cfg(unit, group_index, direction));
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
    framer_flexe_mux1_core_info_t **flexe_mux_chip_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == BCM_PORT_FLEXE_TX)
    {
        SHR_ALLOC_SET_ZERO(flexe_mux_chip_info, sizeof(framer_flexe_mux1_core_info_t *), "flexe_mux_chip_info",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_mux1_core_algorithm_chip_info_get(unit, flexe_mux_chip_info));
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_uas_framer_flexe_mux_reg_get(unit, TUNNEL_FRAMER_FLEXE_MUX_M0, C_CFG, reg_val));
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
    int instance_id, alarm;
    int max_slots_per_instance, i, client_id_ram_index;
    bcm_port_flexe_group_cal_t curr_cal;
    uint32 instance_bmp[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port, &max_slots_per_instance));
    /*
     * Get instance bitmap
     */
    *alarm_status = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        for (i = 0; i < COUNTOF(oh_alarm_field_to_flags_map_table); ++i)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oh_rx_alm_get
                                       (unit, instance_id, oh_alarm_field_to_flags_map_table[i].field_id,
                                        (J2X_U32 *) & alarm));
            *alarm_status |= (alarm ? oh_alarm_field_to_flags_map_table[i].flags : 0);
        }
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_cc_rx_value_get
                                   (unit, instance_id, flexe1_rx_oh_lc_acccc, (J2X_U32 *) & curr_cal));
        for (client_id_ram_index = 0;
             client_id_ram_index < (max_slots_per_instance / FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM);
             ++client_id_ram_index)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_ccm_rx_alm_get
                                       (unit, instance_id, client_id_ram_index, flexe1_rx_oh_lc_ccm0,
                                        (J2X_U32 *) & alarm));
            *alarm_status |=
                (alarm
                 ? ((curr_cal ==
                     bcmPortFlexeGroupCalA) ? FLEXE_CORE_OH_ALARM_CAL_A_MISMATCH : FLEXE_CORE_OH_ALARM_CAL_B_MISMATCH) :
                 0);
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
    int max_slots_per_instance, index;
    int instance_id, client_id_ram_index;
    uint32 instance_bmp[1], mismatched_slots[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get MAX number of slots per instance
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_nof_slots_per_instance_get(unit, flexe_core_port, &max_slots_per_instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_instances_get(unit, flexe_core_port, instance_bmp));

    SHR_BITCLR_RANGE(*time_slots, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);
    index = 0;
    SHR_BIT_ITER(instance_bmp, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance_id)
    {
        for (client_id_ram_index = 0;
             client_id_ram_index < (max_slots_per_instance / FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM);
             ++client_id_ram_index)
        {
            FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_ccm_rx_alm_get
                                       (unit, instance_id, client_id_ram_index, flexe1_rx_oh_lc_ccm0,
                                        mismatched_slots));
            SHR_BITCOPY_RANGE(*time_slots, index * FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM, mismatched_slots, 0,
                              FLEXE_STD_GEN2_NOF_SLOTS_PER_CLIENT_ID_RAM);
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
flexe_std_gen2_busa_port_type_get(
    int unit,
    const flexe_core_busa_channel_map_info_t * channel_map_info,
    int *busa_port_type)
{
    int speed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, channel_map_info->flexe_core_port, &speed));
    switch (speed)
    {
        case 10000:
        {
            if (channel_map_info->is_mgmt_intf)
            {
                *busa_port_type = J2X_MAC_ENV_PORT_PTP_10G;
            }
            else
            {
                *busa_port_type = J2X_MAC_ENV_PORT_ETH_10G;
            }
            break;
        }
        case 25000:
        {
            if (channel_map_info->is_mgmt_intf)
            {
                *busa_port_type = J2X_MAC_ENV_PORT_OHIF_25G;
            }
            else
            {
                *busa_port_type = J2X_MAC_ENV_PORT_ETH_25G;
            }
            break;
        }
        case 50000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = J2X_MAC_ENV_PORT_FLEXE_50G;
            }
            else
            {
                *busa_port_type = J2X_MAC_ENV_PORT_ETH_50G;
            }
            break;
        }
        case 100000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = J2X_MAC_ENV_PORT_FLEXE_100G;
            }
            else
            {
                *busa_port_type = J2X_MAC_ENV_PORT_ETH_100G;
            }
            break;
        }
        case 200000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = J2X_MAC_ENV_PORT_FLEXE_200G;
            }
            else
            {
                *busa_port_type = J2X_MAC_ENV_PORT_ETH_200G;
            }
            break;
        }
        case 400000:
        {
            if (channel_map_info->is_flexe_phy)
            {
                *busa_port_type = J2X_MAC_ENV_PORT_FLEXE_400G;
            }
            else
            {
                *busa_port_type = J2X_MAC_ENV_PORT_ETH_400G;
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
    int port_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_gen2_busa_port_type_get(unit, channel_map_info, &port_type));

    if (flags & BCM_PORT_FLEXE_TX)
    {
        /*
         * Speed is irrelevant
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_busa66_cfg
                                   (unit, channel_map_info->flexe_core_port, port_type, channel_map_info->local_port, 0,
                                    FLEXE_MUX, FRAMER_DIRECTION_TX, enable));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Speed and dst_type are irrelevant
         */
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_subsys_flexe_adapter_busa66_cfg
                                   (unit, channel_map_info->flexe_core_port, port_type, channel_map_info->local_port, 0,
                                    0, FRAMER_DIRECTION_RX, enable));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_CH_ALM_GET(0), val));

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
    J2X_U32 val[3];
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = base_period;
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_BAS_PERIOD_SET(0), val));
        /*
         * Update DB, only relevant for Tx
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get
                        (unit, DNX_ALGO_PORT_TYPE_FLEXE_CLIENT, client_channel, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_bas_period_set(unit, logical_port, base_period));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BAS_PERIOD_SET(0), val));
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
    J2X_U32 val[3];
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
        val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
        val[1] = client_channel;
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BAS_PERIOD_GET(0), val));
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
    J2X_U32 val[4];
    uint32 orig_period;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get original period
     */
    SHR_IF_ERR_EXIT(flexe_std_gen2_oam_base_period_get(unit, client_channel, BCM_PORT_FLEXE_TX, &orig_period));
    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    val[3] = orig_period;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_BAS_EN_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_BAS_EN_GET(0), val));
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
    J2X_U32 val[3];
    uint32 orig_period;

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_BYPASS_SET(0), val));
        /*
         * The bypass will clear the orignal BAS period in HW,
         * so need to recover the orginal period
         */
        if (enable)
        {
            SHR_IF_ERR_EXIT(flexe_std_gen2_oam_base_period_get(unit, client_channel, BCM_PORT_FLEXE_TX, &orig_period));
            SHR_IF_ERR_EXIT(flexe_std_gen2_oam_base_period_set(unit, client_channel, BCM_PORT_FLEXE_TX, orig_period));
        }
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BYPASS_SET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;

    if (flags & BCM_PORT_FLEXE_TX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_BYPASS_GET(0), val));
    }
    else if (flags & BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BYPASS_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_INSERT_LF_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_INSERT_LF_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = enable;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_INSERT_RF_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_TX_INSERT_RF_GET(0), val));
    *enable = val[2];
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SD_BIP8_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SD_BIP8_BLOCK_NUM_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BIP8_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BIP8_SET_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BIP8_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BIP8_CLR_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SF_BIP8_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SF_BIP8_BLOCK_NUM_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BIP8_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BIP8_SET_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BIP8_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BIP8_CLR_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SD_BEI_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SD_BEI_BLOCK_NUM_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BEI_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BEI_SET_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BEI_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SD_BEI_CLR_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = block_num;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SF_BEI_BLOCK_NUM_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_SF_BEI_BLOCK_NUM_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BEI_SET_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BEI_SET_THRESHOLD_GET(0), val));
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
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    val[2] = threshold;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BEI_CLR_THRESHOLD_SET(0), val));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
flexe_std_gen2_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    J2X_U32 val[3];

    SHR_FUNC_INIT_VARS(unit);

    val[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd
                               (unit, J2X_CMD_FLEXE_OAM_RX_SF_BEI_CLR_THRESHOLD_GET(0), val));
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
    J2X_U32 val_tmp[3];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BIP8_CNT_GET(0), val_tmp));
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
    J2X_U32 val_tmp[3];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BEI_CNT_GET(0), val_tmp));
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
    J2X_U32 val_tmp[4];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_CNT_GET(0), val_tmp));
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
    J2X_U32 val_tmp[4];

    SHR_FUNC_INIT_VARS(unit);

    val_tmp[0] = TUNNEL_FRAMER_FLEXE_OAM_RX_M0;
    val_tmp[1] = client_channel;
    FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_flexe_oam_cmd(unit, J2X_CMD_FLEXE_OAM_RX_BAS_CNT_GET(0), val_tmp));
    COMPILER_64_SET(*val, val_tmp[3], val_tmp[2]);
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

    SHR_IF_ERR_EXIT(dnx_algo_port_sar_channel_to_logical_get(unit, client_channel, &sar_logical_port));
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_uas_framer_sar_tx_channel_cfg_set
                                   (unit, SAR_TX_EN, TUNNEL_FRAMER_SAR_TX, client_channel, enable));
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
    J2X_U32 enable_tmp;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == BCM_PORT_FLEXE_RX)
    {
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_uas_framer_sar_tx_channel_cfg_get
                                   (unit, SAR_TX_EN, TUNNEL_FRAMER_SAR_TX, client_channel, &enable_tmp));
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
            *traffic_type = LANE_FEC_ETH_10GE;
            break;
        }
        case 25000:
        {
            *traffic_type = LANE_FEC_ETH_25GE;
            break;
        }
        case 50000:
        {
            if (fec_type == bcmPortPhyFecRs544)
            {
                *traffic_type = LANE_FEC_ETH_50GE_KP4;
            }
            else
            {
                *traffic_type = LANE_FEC_ETH_50GE;
            }
            break;
        }
        case 100000:
        {
            if (fec_type == bcmPortPhyFecRs544)
            {
                *traffic_type = LANE_FEC_ETH_100GE_KP4;
            }
            else
            {
                *traffic_type = LANE_FEC_ETH_100GE;
            }
            break;
        }
        case 200000:
        {
            *traffic_type = LANE_FEC_ETH_200GE_KP4;
            break;
        }
        case 400000:
        {
            *traffic_type = LANE_FEC_ETH_400GE_KP4;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed.\r\n");
        }
    }
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
        FLEXE_STD_GEN2_IF_ERR_EXIT(j2x_sal_framer_lane_fec_rx_cfg_local_gen_m_n_set
                                   (unit, traffic_type, lane_index + first_logical_lane));
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
    int actual_size, speed, lane_id, count = 0;
    uint32 flr_logical_lane;
    bcm_pbmp_t phys;
    int lane_order[DNX_DATA_MAX_NIF_OTN_NOF_LANES];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get logical lanes
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phys));
    /*
     * Get FLR logical lanes
     */
    SHR_IF_ERR_EXIT(imb_port_lane_map_get
                    (unit, logical_port, 0, DNX_DATA_MAX_NIF_OTN_NOF_LANES, lane_order, &actual_size));

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));

    _SHR_PBMP_ITER(phys, lane_id)
    {
        flr_logical_lane = DNXC_PORT_PHY_SPEED_IS_PAM4(speed) ? lane_order[2 * count] : lane_order[count];
        SHR_IF_ERR_EXIT(j2x_uas_framer_lane_otn_fiber_los_ram_set
                        (unit, 0, CFG_LOGIC_LOS_MUX, lane_id, (J2X_U32 *) & flr_logical_lane));
        count++;
    }
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
#endif
/* } */
