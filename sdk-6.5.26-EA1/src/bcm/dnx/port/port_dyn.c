/**
 * \file port_dyn.c
 *
 *  Dynamicaly adding and removing ports
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <sal/core/boot.h>

#include <shared/bslenum.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_stk_sys_access.h>
#include <soc/counter.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_cosq_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_common_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/port/port_ingress.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/egress/egq_ps_db.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/link/link.h>
#include <bcm_int/dnx/oam/oam_init.h>
#include <bcm_int/dnx/cosq/egress/egr_tm_dispatch.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/sand/sand_aux_access.h>
#include "port_utils.h"
#include "src/bcm/dnx/algo/port/algo_port_internal.h"
#include "src/bcm/dnx/trunk/trunk_utils.h"
#include "src/bcm/dnx/trunk/trunk_sw_db.h"
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnxc/dnxc_data_property.h>
#include <bcm_int/dnx/port/port_sit.h>

/*
 * }
 */
/*
 * Defines.
 * {
 */
/*
 * }
 */
/*
 * Macros.
 * {
 */
/**
 * \brief - supported flags for API bcm_dnx_port_add()
 */
#define DNX_PORT_ADD_SUPPORTED_FLAGS (BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID | BCM_PORT_ADD_CGM_AUTO_ADJUST_DISABLE | \
                                      BCM_PORT_ADD_CONFIG_CHANNELIZED | BCM_PORT_ADD_STIF_PORT | \
                                      BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT | \
                                      BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE | \
                                      BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE | \
                                      BCM_PORT_ADD_TDM_PORT | BCM_PORT_ADD_IF_TDM_ONLY | BCM_PORT_ADD_IF_TDM_HYBRID | \
                                      BCM_PORT_ADD_FLEXE_MAC | BCM_PORT_ADD_FLEXE_PHY | BCM_PORT_ADD_FLEXE_SAR | \
                                      BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN | BCM_PORT_ADD_STIF_AND_DATA_PORT | \
                                      BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT | BCM_PORT_ADD_STAT_ONLY | BCM_PORT_ADD_IF_FRAMER_MGMT | \
                                      BCM_PORT_ADD_OTN_SAR | BCM_PORT_ADD_TXPI_MODE_EXT | BCM_PORT_ADD_MC_PORT)

/**
 * Brief - max size of string phys bitmap
 */
#define DNX_PORT_PHYS_STR_LENGTH 256

#define DNX_ALGO_PORT_LEGACY_INTERFACES_MAX_NOF_LANES  (4)
/*
 * }
 */

/*
 * Typedefs.
 * {
 */
/**
 * \brief - relevant information required for each step in port add / remove sequence
 */
typedef struct dnx_port_add_remove_step_s dnx_port_add_remove_step_t;
struct dnx_port_add_remove_step_s
{
    /**
     * Step name
     */
    char *name;
    /**
     * Callback to add function
     */
    utilex_seq_cb_f add_func;
    /**
     * Callback to remove function
     */
    utilex_seq_cb_f remove_func;
    /**
     * List of sub-steps (optional - might be null if not required)
     */
    const dnx_port_add_remove_step_t *sub_list;

    /**
     * Flag function CB that will run prior to the init function of
     * this step to determine which flags are needed to the step
     * according to the CB logic - could be looking for certain SOC
     * properties for example.
     */
    dnx_step_flag_cb flag_function;
};

/*
 * }
 */

/*
 * Local Functions
 * {
 */
static int dnx_port_init_ilkn_lane_order_set(
    int unit);

/**
 * \brief - device specific callback function
 * 
 */
static shr_error_e
device_specific_skip(
    int unit,
    int *dynamic_flags)
{
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

/**
 * \brief - Perform port add verifications unique to ILKN ports
 */
static shr_error_e
dnx_port_add_ilkn_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    dnx_algo_port_info_s * port_info,
    bcm_port_mapping_info_t * mapping_info,
    dnx_algo_port_info_indicator_t * indicator)
{
    int nof_lanes;
    int ilkn_id;
    int ilkn_core;
    int add_to_ilkn_core;
    int nof_active_ilkn_ids;
    bcm_pbmp_t phy_pbmp;
    bcm_info_t info;
    uint32 ilkn_ids[1];

    const bcm_pbmp_t *supported_phys;
    const int l1_only_mode = dnx_data_nif.global.l1_only_mode_get(
    unit);
    const int ilkn_unit_if_nof = dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
    const int is_elk = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, (*port_info));
    const int nof_ilkn_ids = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * ilkn_unit_if_nof;
    const int supported_device_core =
        dnx_data_nif.ilkn.supported_device_core_get(unit, interface_info->interface_id)->core;
    const dnx_data_nif_ilkn_ilkn_cores_t *ilkn_core_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_info_get(unit, &info));

    if (!(dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_supported)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "ILKN is not supported for device 0x%x. Please reconfigure port %d properties \n",
                     info.device, port);
    }

    if (indicator->is_elk && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_elk_supported))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "ELK is not supported for device 0x%x. Please remove KBP suffix from port %d configuration\n",
                     info.device, port);
    }

    /*
     * Checks for L1 only Mode 
     */
    if (l1_only_mode == DNX_PORT_NIF_L1_ONLY_MODE_WITHOUT_ILKN)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Port %d: ILKN is not supported when l1_only_mode is set to L1_ONLY_WITHOUT_ILKN.\n", port);
    }

    /** verify  interface ID for ILKN */
    if (interface_info->interface_id >= nof_ilkn_ids)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN port (%d) mapped to unsupported ILKN ID (%d)\n", port,
                     interface_info->interface_id);
    }

    /** verify core */
    if (supported_device_core != mapping_info->core)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN port (%d) mapped to unsupported core (%d)\n", port, mapping_info->core);
    }

    /** pervent adding any ilkn DATA port on ilkn core that is already used for ELK and vice versa */
    add_to_ilkn_core = interface_info->interface_id / ilkn_unit_if_nof;

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_active_ids_get(unit,
                                                      (is_elk ?
                                                       DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY :
                                                       DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY), ilkn_ids));

    SHR_BIT_ITER(ilkn_ids, nof_ilkn_ids, ilkn_id)
    {
        ilkn_core = ilkn_id / ilkn_unit_if_nof;
        if (ilkn_core == add_to_ilkn_core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Core (%d) already in use by %s. Adding ILKN port (%d) on the same "
                         "core is not allowed\n", ilkn_core, (is_elk ? "DATA" : "ELK"), port);
        }
    }
    ilkn_core_info = dnx_data_nif.ilkn.ilkn_cores_get(unit, add_to_ilkn_core);
    if ((is_elk && !ilkn_core_info->is_elk_supported) || (!is_elk && !ilkn_core_info->is_data_supported))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core (%d) doesn't support %s. But adding ILKN port (%d) requires this type\n ",
                     add_to_ilkn_core, (is_elk ? "ELK" : "DATA"), port);
    }
    /** verify that phys supported by ILKN-ID */
    if (flags & BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT)
    {
        supported_phys = &dnx_data_nif.ilkn.supported_phys_get(unit, interface_info->interface_id)->fabric_phys;
    }
    else
    {
        supported_phys = &dnx_data_nif.ilkn.supported_phys_get(unit, interface_info->interface_id)->nif_phys;
    }
    BCM_PBMP_ASSIGN(phy_pbmp, interface_info->phy_pbmp);
    BCM_PBMP_REMOVE(phy_pbmp, *supported_phys);
    if (!BCM_PBMP_IS_NULL(phy_pbmp))
    {
        char phys_buf[DNX_PORT_PHYS_STR_LENGTH];
        shr_pbmp_range_format(phy_pbmp, phys_buf, DNX_PORT_PHYS_STR_LENGTH);
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d, assigned to ILKN ID %d, does not support phys %s \n", port,
                     interface_info->interface_id, phys_buf);
    }
    BCM_PBMP_COUNT(interface_info->phy_pbmp, nof_lanes);
    if (nof_lanes > dnx_data_nif.ilkn.supported_phys_get(unit, interface_info->interface_id)->max_phys)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d, assigned to ILKN ID %d, the MAX nof phys should be %d \n",
                     port, interface_info->interface_id,
                     dnx_data_nif.ilkn.supported_phys_get(unit, interface_info->interface_id)->max_phys);
    }

    /** verify that we didn't cross the max number of supported ILKN DATA interfaces */
    if (dnx_data_nif.ilkn.max_nof_ifs_get(unit) != -1
        /** elk: 0, L1: 1 */
        && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, (*port_info), DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_active_ids_get(unit, DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY, ilkn_ids));
        /** add the new ilkn id */
        SHR_BITSET(ilkn_ids, interface_info->interface_id);
        SHR_BITCOUNT_RANGE(ilkn_ids, nof_active_ilkn_ids, 0, nof_ilkn_ids);
        if (nof_active_ilkn_ids > dnx_data_nif.ilkn.max_nof_ifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Max number of active ILKN DATA interfaces is limited to %d \n",
                         dnx_data_nif.ilkn.max_nof_ifs_get(unit));
        }
    }

    /** verify that we didn't cross the max number of supported ILKN ELK interfaces */
    if (dnx_data_nif.ilkn.max_nof_elk_ifs_get(unit) != -1 && is_elk)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_active_ids_get(unit, DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY, ilkn_ids));
        /** add the new ilkn id */
        SHR_BITSET(ilkn_ids, interface_info->interface_id);
        SHR_BITCOUNT_RANGE(ilkn_ids, nof_active_ilkn_ids, 0, nof_ilkn_ids);
        if (nof_active_ilkn_ids > dnx_data_nif.ilkn.max_nof_elk_ifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Max number of active ILKN ELK interfaces is limited to %d \n",
                         dnx_data_nif.ilkn.max_nof_elk_ifs_get(unit));
        }
    }

    /*
     * ILKN phys logical verifier is only relevant for ilkn port over NIF
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT))
    {
        /** verify that For ILKN cores which are connected to Flacon, the upper 8 lanes of each ILKN port (lanes 16-23) can be taken either for the upper or for the below ilkn core but not both*/
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_phys_verify
                        (unit, mapping_info->core, port, interface_info->interface_id, interface_info->phy_pbmp));
    }

    /*
     * Check assigned phys are legal for connectivity mode
     */
    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_12lanes_mode))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_phys_12lane_mode_verify
                        (unit, mapping_info->core, port, interface_info->interface_id, interface_info->phy_pbmp));
    }

    /*
     * ILKN over Fabric ports cannot be added dynamically
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT) && dnxc_init_is_init_done_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN over fabric ports cannot be added dynamically! \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get indications from flags and info
 */
static shr_error_e
dnx_port_add_indicator_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info,
    dnx_algo_port_info_indicator_t * indicator)
{
    int ethu_id;
    int first_phy;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * if ELK port, update flag
     */
    sal_memset(indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    if (flags & BCM_PORT_ADD_KBP_PORT)
    {
        indicator->is_elk = 1;
    }
    /*
     * add indication for STIF port
     */
    if (flags & (BCM_PORT_ADD_STIF_PORT | BCM_PORT_ADD_STIF_AND_DATA_PORT))
    {
        _SHR_PBMP_FIRST(interface_info->phy_pbmp, first_phy);
        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

        if (dnx_data_stif.port.ethu_id_stif_valid_get(unit, ethu_id)->valid == 1)
        {
            indicator->is_stif = 1;

            /** indicate that this is also an ingress tm port (data) */
            if (flags & BCM_PORT_ADD_STIF_AND_DATA_PORT)
            {
                indicator->is_stif_data = 1;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot add given port as STIF. port=%d\n", port);
        }
    }
    /*
     * add indication for cross connect port
     */
    if (flags & BCM_PORT_ADD_CROSS_CONNECT)
    {
        indicator->is_cross_connect = 1;
    }

    if (flags & BCM_PORT_ADD_FLEXE_PHY)
    {
        indicator->is_flexe_phy = 1;
    }
    /*
     * add indication for cross connect port
     */
    if (flags & BCM_PORT_ADD_FLEXE_MAC)
    {
        indicator->is_flexe_mac = 1;
    }

    if (flags & BCM_PORT_ADD_FLEXE_SAR)
    {
        indicator->is_flexe_sar = 1;
    }

    if ((interface_info->interface == BCM_PORT_IF_NIF_ETH)
        && dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_mgmt_port_supported))
    {
        _SHR_PBMP_FIRST(interface_info->phy_pbmp, first_phy);
        if (first_phy >= dnx_data_nif.phys.first_mgmt_phy_get(unit))
        {
            indicator->is_mgmt = 1;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifying 'bcm_dnx_port_add()'
 *          For additional info goto 'bcm_dnx_port_add()'  definition.
 */
static shr_error_e
dnx_port_add_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    int is_init_done;
    const dnx_data_nif_phys_core_phys_map_t *nif_core_phys_map;
    bcm_pbmp_t phy_pbmp, phy_pbmp_temp;
    dnx_algo_port_info_indicator_t indicator;
    dnx_algo_port_info_s port_info;
    int nof_fap_ids, first_phy_port, nof_lanes;
    int first_phy, lane;
    int is_channelized;
    int out_tm_port;
    bcm_port_t out_tm_master_port;
    int master_base_q_pair, master_base_hr;
    int out_tm_valid;
    int pm_index, phy_map_granularity, is_phy_active;
    int existed_flr_fw_status, port_is_flr_support = 0;
    int is_ext_txpi_support = 0;
    bcm_pbmp_t pm_phys;
    bcm_port_t logical_port_tmp;

    SHR_FUNC_INIT_VARS(unit);

    /** null checks */
    SHR_NULL_CHECK(interface_info, _SHR_E_PARAM, "inteface_info");
    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    /** flags */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_USE_PHY_PBMP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_USE_PHY_PBMP flag is not supported. phy_pbmp is used only for BCM_PORT_IF_ILKN and BCM_PORT_IF_NIF_ETH interface types.\n");
    }

    SHR_MASK_VERIFY(flags, DNX_PORT_ADD_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    /** make sure that TDM port added to a system that support TDM ports */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_TDM_PORT) && (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_NONE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "According to device type or soc properties, TDM is not supported, TDM port %d cannot be added.\n",
                     port);
    }

    if ((interface_info->interface != BCM_PORT_IF_ILKN) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT flag is supported only for BCM_PORT_IF_ILKN interface type.\n");
    }

    /** make sure that TDM port added to TDM interface that support TDM ports */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_TDM_PORT)
        && !_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_IF_TDM_ONLY | BCM_PORT_ADD_IF_TDM_HYBRID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "TDM port %d can only added to TDM only interface (BCM_PORT_ADD_IF_TDM_ONLY flag) or to TDM hybrid interface (BCM_PORT_ADD_IF_TDM_HYBRID flag).\n",
                     port);
    }

    /** Throw an error if NON-TDM ports added to TDM only interface */
    if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_TDM_PORT) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_IF_TDM_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "NON-TDM port %d can not be added to TDM only interface (BCM_PORT_ADD_IF_TDM_ONLY flag).\n", port);
    }

    /** make sure FlexE related ports are added when FlexE mode is enabled and flexe feature is enabled */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_PHY) || _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_CROSS_CONNECT)
        || _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_SAR) || _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_IF_FRAMER_MGMT)
        || (interface_info->interface == BCM_PORT_IF_FLEXE_CLIENT)
        || (interface_info->interface == BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT))
    {
        if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "FlexE related ports can be added only on the device supported FlexE feature.\n");
        }
        else
        {
            if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_device_mode_support))
            {
                if (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISABLED)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "FlexE related ports can be added only when flexe mode is enabled by SOC property flexe_device_mode.\n");
                }
                else if ((dnx_data_nif.flexe.flexe_mode_get(unit) != DNX_FLEXE_MODE_DISTRIBUTED)
                         && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_SAR))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "SAR is only valid in distributed mode.\n");
                }
                if ((_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_CROSS_CONNECT)
                     || _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_SAR))
                    && dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_disable_l1))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "L1 feature is not supported on this device.\n");
                }
            }
            /** L1 MAC client is supported */
            if (!dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_l1_support) &&
                _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_MAC) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_CROSS_CONNECT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_PORT_ADD_CROSS_CONNECT flag is not supported along with BCM_PORT_ADD_FLEXE_MAC.\n");
            }
            /** make sure that FlexE phy flag is set only the interface type is ethernet port */
            if ((interface_info->interface != BCM_PORT_IF_NIF_ETH) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_PHY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_PORT_ADD_FLEXE_PHY flag is supported only for BCM_PORT_IF_NIF_ETH interface type.\n");
            }

            /** make sure that FlexE MAC/SAR flag is set only the interface type is virtual client port */
            if ((interface_info->interface != BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT)
                && (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_MAC) || _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_SAR)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_PORT_ADD_FLEXE_MAC/SAR flag is supported only for BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT interface type.\n");
            }
            /** make sure that BCM_PORT_ADD_IF_FRAMER_MGMT flag is set correctly */
            if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_IF_FRAMER_MGMT))
            {
                if (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The Framer mgmt interface is handled by MAC client channel 80 and 81, BCM_PORT_ADD_IF_FRAMER_MGMT is not required\n");
                }
                if ((interface_info->interface != BCM_PORT_IF_NIF_ETH) &&
                    !(interface_info->interface == BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT
                      && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_MAC)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The Framer management interface is ether ETH or Virtual MAC client.\n");
                }
            }
        }
    }

    /** make sure that FlexE phy flag is set only the interface type is ethernet port */
    if ((interface_info->interface != BCM_PORT_IF_NIF_ETH) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_PHY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_FLEXE_PHY flag is supported only for BCM_PORT_IF_NIF_ETH interface type.\n");
    }

    /** make sure that FlexE MAC/SAR flag is set only the interface type is virtual client port */
    if ((interface_info->interface != BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT)
        && (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_MAC) || _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_SAR)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_FLEXE_MAC/SAR flag is supported only for BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT interface type.\n");
    }

    /** make sure that FlexE MAC flag is set only the interface type is virtual client port */
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent) &&
        _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLEXE_MAC) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_CROSS_CONNECT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_CROSS_CONNECT flag is not supported along with BCM_PORT_ADD_FLEXE_MAC.\n");
    }
    /** valid and free logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, port));

    /*
     * Get indications from flags 
     */
    sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    SHR_IF_ERR_EXIT(dnx_port_add_indicator_get(unit, port, flags, interface_info, mapping_info, &indicator));

    /*
     * Convert interface type to port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, interface_info->interface, &indicator, &port_info));

    /*
     * Perform ILKN related verifications
     */
    if (interface_info->interface == BCM_PORT_IF_ILKN)
    {
        SHR_IF_ERR_EXIT(dnx_port_add_ilkn_verify
                        (unit, port, flags, interface_info, &port_info, mapping_info, &indicator));
    }
    /*
     * Perform OT related verifications
     */
    if (interface_info->interface == BCM_PORT_IF_OTN)
    {
        if (!dnx_data_nif.global.otn_supported_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "OTN is unavailable! Please add SOC property 'otn_supported=1' to config.bcm file\n");
        }
        if (!dnx_data_nif.otn.feature_get(unit, dnx_data_nif_otn_is_supported))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "OTN related ports can be added only on the device supported OTN feature.\n");
        }
    }
    /*
     * Verify L1 only mode for ETH and FlexE
     */
    if (dnx_data_nif.global.l1_only_mode_get(unit) != DNX_PORT_NIF_L1_ONLY_MODE_DISABLED)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0)
            || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "ETH and Framer MAC L2/L3 port are not allowed in L1 only mode.\n");
        }
    }
    /*
     * Only the following ports are supporting dynamic port add:
     * 1. NIF ports (excluding ELK, STIF and MGMT)
     * 2. Mirror-recycle ports
     * 3. Recycle ports
     * 4. FlexE client
     * 5. SCH only (LAG scheduler)
     */

    if ((!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, FLEXE, OTN_PHY))) &&
        !DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info) && !DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info)
        && !DNX_ALGO_PORT_TYPE_IS_FRAMER(unit, port_info) && !DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info))
    {
        is_init_done = dnxc_init_is_init_done_get(unit);
        if (is_init_done)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Only NIF port (excluding ELK / STIF), RCY, mirror-recycle, FlexE client and SCH only can be dynamically added. port=%d\n",
                         port);
        }
    }
    /** setting pp_port is not supported - make sure that pp port wasn't set explicitly*/
    if (mapping_info->pp_port != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Setting PP port is not supported (only getting). pp_port must be set to -1\n");
    }

    /** validate core/phys mapping (if any) */
    DNXCMN_CORE_VALIDATE(unit, mapping_info->core, FALSE);

    BCM_PBMP_CLEAR(phy_pbmp);

    {
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            nif_core_phys_map = dnx_data_nif.phys.core_phys_map_get(unit, mapping_info->core);

            if (BCM_PBMP_IS_NULL(interface_info->phy_pbmp))
            {
                if (interface_info->interface == BCM_PORT_IF_ILKN || interface_info->interface == BCM_PORT_IF_NIF_ETH
                    || interface_info->interface == BCM_PORT_IF_OTN)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Port (%d) cannot be assigned with empty phy bitmap."
                                 "Ports with interface type of BCM_PORT_IF_ILKN or BCM_PORT_IF_NIF_ETH or BCM_PORT_IF_OTN must have phy_pbmp parameter not empty. \n",
                                 port);
                }

                if ((interface_info->phy_port < 1) || (interface_info->phy_port > dnx_data_nif.phys.nof_phys_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Either phy_pbmp or phy_port should be set correctly for port %d.\n",
                                 port);
                }

                if (interface_info->num_lanes == 0
                    || interface_info->num_lanes > DNX_ALGO_PORT_LEGACY_INTERFACES_MAX_NOF_LANES)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "When providing port phys with phy_port parameter (not with phy_pbmp parameter) num_lanes parameter should hold a valid value in range 0 - %d. Value %d is invalid.\n",
                                 DNX_ALGO_PORT_LEGACY_INTERFACES_MAX_NOF_LANES, interface_info->num_lanes);
                }

                /*
                 * legacy type, when using type that is NOT BCM_PORT_IF_NIF_ETH
                 * phy_port is 1-based in legacy calls, thus we need to do minus one
                 */
                first_phy_port = interface_info->phy_port - 1;

                /** get number of lanes according to legacy type */
                SHR_IF_ERR_EXIT(dnx_algo_port_legacy_interface_to_nof_lanes_get
                                (unit, interface_info->interface, &nof_lanes));
                for (lane = 0; lane < nof_lanes; ++lane)
                {
                    BCM_PBMP_PORT_ADD(phy_pbmp, first_phy_port + lane);
                }
            }
            else
            {
                BCM_PBMP_ASSIGN(phy_pbmp, interface_info->phy_pbmp);
            }
            /*
             * Save phy bitmap for future use
             */
            BCM_PBMP_ASSIGN(phy_pbmp_temp, phy_pbmp);
            /*
             * If not all links provided are part of the supported bitmap return error
             */
            BCM_PBMP_AND(phy_pbmp_temp, dnx_data_nif.phys.general_get(unit)->supported_phys);
            if (BCM_PBMP_NEQ(phy_pbmp_temp, phy_pbmp))
            {
                /*
                 * Find first PHY that is not supported.
                 */
                BCM_PBMP_ITER(phy_pbmp, first_phy)
                {
                    if (!BCM_PBMP_MEMBER(dnx_data_nif.phys.general_get(unit)->supported_phys, first_phy))
                    {
                        break;
                    }
                }

                SHR_ERR_EXIT(_SHR_E_PARAM, "SerDes %d, provided for port %d is not valid for this device \n", first_phy,
                             port);
            }

            BCM_PBMP_ASSIGN(phy_pbmp_temp, phy_pbmp);
            if ((interface_info->interface != BCM_PORT_IF_ILKN) && BCM_PBMP_NOT_NULL(phy_pbmp_temp) &&
                !DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
            {
                BCM_PBMP_AND(phy_pbmp_temp, nif_core_phys_map->phys);
                if (BCM_PBMP_IS_NULL(phy_pbmp_temp))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Should not assign port %d to core %d\n", port, mapping_info->core);
                }
            }
        }

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            phy_map_granularity = dnx_data_nif.eth.phy_map_granularity_get(unit);
            _SHR_PBMP_FIRST(interface_info->phy_pbmp, first_phy);
            pm_index = dnx_data_nif.eth.phy_map_get(unit, first_phy / phy_map_granularity)->pm_index;

            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT)))
            {
                SHR_IF_ERR_EXIT(dnx_port_pre_add_validate(unit, port, phy_pbmp));
                is_ext_txpi_support = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->ext_txpi_support;
            }
            /*
             * Verify the FLR configuration
             */
            if (dnx_data_nif.eth.pm_properties_get(unit, pm_index)->flr_support)
            {
                /*
                 * Check if all the FLR configurations for the ports in the PM
                 * are consistent.
                 */
                port_is_flr_support = 1;
                pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
                _SHR_PBMP_REMOVE(pm_phys, interface_info->phy_pbmp);
                _SHR_PBMP_ITER(pm_phys, lane)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, lane, &is_phy_active));
                    if (is_phy_active)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, lane, 0, 0, &logical_port_tmp));
                        /*
                         * Get existed FLR configuration
                         */
                        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get
                                        (unit, logical_port_tmp, &existed_flr_fw_status));
                        /*
                         * Return error if there is inconstent
                         */
                        if (existed_flr_fw_status != _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT))
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG,
                                         "Port %d - All the ports in the same PM should have the same FLR firmware enable status.\n",
                                         port);
                        }
                        break;
                    }
                }
            }
        }
        if (!port_is_flr_support && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d - The port doesn't support Fast Link Recovery.\n", port);
        }
        /*
         * Verify Ext TXPI mode
         */
        if (!is_ext_txpi_support && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_TXPI_MODE_EXT))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d - The ext TXPI mode is not configurable on this port.\n", port);
        }
    }

    /** Adding ERP port dynamically is not supported */
    is_init_done = dnxc_init_is_init_done_get(unit);
    if (is_init_done)
    {
        if (interface_info->interface == BCM_PORT_IF_ERP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - adding ERP port dynamically is not supported, use soc property ucode_port instead.\n",
                         port);
        }
    }

    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        /*
         * if tm_port is contradicting in_tm_port/out_tm_port/pp_dsp return an error
         */
        if (mapping_info->tm_port != -1 &&
            ((mapping_info->in_tm_port != -1 && mapping_info->tm_port != mapping_info->in_tm_port) ||
             (mapping_info->out_tm_port != -1 && mapping_info->tm_port != mapping_info->out_tm_port) ||
             (mapping_info->pp_dsp != -1 && mapping_info->tm_port != mapping_info->pp_dsp)))

        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - cannot specify in_tm_port/pp_dsp/out_tm_port when tm_port is specified \n", port);

        }
        if (is_init_done)
        {
            /** tm port allowed verify */
            SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.get(unit, &nof_fap_ids));
            /** after init need to make sure pp dsp < 256 * nof_fap_ids */
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_allowed_verify(unit, mapping_info->core,
                                                                (mapping_info->tm_port == -1 ? mapping_info->pp_dsp :
                                                                 mapping_info->tm_port), nof_fap_ids));
        }
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE))
    {
        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - flag BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE is not allowed for this port type \n",
                         port);
        }
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE))
    {
        if ((!DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info)) && (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - flag BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE is not allowed for this port type \n",
                         port);
        }
    }
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
    {
        if (mapping_info->in_channel != -1 || mapping_info->out_channel != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - in/out_channel cannot be specified for this port type\n", port);

        }
        SHR_IF_ERR_EXIT(dnx_algo_flexe_channel_verify(unit, port_info, mapping_info->channel));
    }

    if (DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID flag cannot be specified for SCH_ONLY port\n",
                         port);
        }
        if (DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info) && mapping_info->base_hr == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - base_hr must be specified for SCH_ONLY port\n", port);
        }
    }
    else
    {
        if (mapping_info->base_hr != -1 && !_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - base_hr can be specified only with BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID flag\n", port);
        }
        if (mapping_info->num_sch_priorities != 0 && !_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - num_sch_priorities can be specified only with BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID flag\n",
                         port);
        }
        if (mapping_info->num_sch_priorities > mapping_info->num_priorities)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - num_sch_priorities (%d) cannot be bigger than num_priorities (%d)\n",
                         port, mapping_info->num_sch_priorities, mapping_info->num_priorities);
        }
    }
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID))
    {
        out_tm_port = mapping_info->tm_port == -1 ? mapping_info->out_tm_port : mapping_info->tm_port;
        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get(unit, mapping_info->core, out_tm_port, &out_tm_valid));
        if (out_tm_valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get(unit, mapping_info->core, out_tm_port,
                                                                &out_tm_master_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, out_tm_master_port, &master_base_q_pair));
            if (mapping_info->base_q_pair != master_base_q_pair)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Port %d - OUT TM port (%d, %d) already defined with base q pair %d. Requested base q pair %d\n",
                             port, mapping_info->core, out_tm_port, master_base_q_pair, mapping_info->base_q_pair);
            }
            if (mapping_info->base_hr != -1)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, out_tm_master_port, &master_base_hr));
                if (mapping_info->base_hr != master_base_hr)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Port %d - OUT TM port (%d, %d) already defined with base HR %d. Requested base HR %d\n",
                                 port, mapping_info->core, out_tm_port, master_base_hr, mapping_info->base_hr);
                }

            }
        }
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN) &&
        (mapping_info->num_sch_priorities == 1 ||
         (mapping_info->num_sch_priorities == 0 && mapping_info->num_priorities == 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - cannot apply priority propagation on 1-priority port\n", port);
    }

    is_channelized = _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_CONFIG_CHANNELIZED) ||
        DNX_ALGO_PORT_TYPE_IS_ILKN_L1(unit, port_info);
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN) && !is_channelized)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - cannot apply priority propagation on non channelized port\n", port);
    }

    if (is_channelized && DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - management port cannot be channelized \n", port);

    }
    if (is_channelized)
    {
        /*
         * if channel is contradicting in_channel/out_channel return an error
         */
        if (mapping_info->channel != -1 &&
            ((mapping_info->in_channel != -1 && mapping_info->channel != mapping_info->in_channel) ||
             (mapping_info->out_channel != -1 && mapping_info->channel != mapping_info->out_channel)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - cannot specify in_channel/out_channel when channel is specified \n", port);
        }
        if (mapping_info->channel == -1 && mapping_info->in_channel != mapping_info->out_channel &&
            !DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - in_channel and out_channel must be equal for this port type \n", port);
        }
    }

    if ((interface_info->interface == BCM_PORT_IF_CRPS) &&
        !dnx_data_crps.eviction.supported_eviction_destination_get(unit, bcmStatEvictionDestinationNetwork)->valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - cannot add CRPS port if narrow counter eviction is not supported\n",
                     port);
    }

    /** additional verification will be done by port mgmt when adding the new port to database*/

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Called by dnx_port_remove_verify() to ensure
 *          no time slot occupied by the client to be removed
 */
static shr_error_e
dnx_port_flexe_client_remove_verify(
    int unit,
    bcm_port_t client)
{
    uint16 logical_port_demux, logical_port_mux;
    int group, calendar_id, group_speed, nof_slots, slot_index, valid;
    bcm_port_flexe_phy_slot_mode_t slot_mode;
    SHR_FUNC_INIT_VARS(unit);

    for (group = 0; group < dnx_data_nif.flexe.nof_groups_get(unit); ++group)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, group, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, group, &group_speed));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group, &slot_mode));
            nof_slots = group_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
            for (calendar_id = bcmPortFlexeGroupCalA; calendar_id < bcmPortFlexeGroupCalCount; ++calendar_id)
            {
                for (slot_index = 0; slot_index < nof_slots; ++slot_index)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                                    cal_slots.get(unit, group, calendar_id, slot_index, &logical_port_demux));
                    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                                    cal_slots.get(unit, group, calendar_id, slot_index, &logical_port_mux));
                    if ((logical_port_demux == client) || logical_port_mux == client)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "Please call bcm_port_flexe_group_cal_slots_set() to remove the time slot occupied by the client(%d) first!\n",
                                     client);
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Called by dnx_port_remove_verify() to ensure
 *          no flexe group is using the phy port to be removed
 */
static shr_error_e
dnx_port_flexe_phy_remove_verify(
    int unit,
    bcm_port_t flexe_phy)
{
    bcm_pbmp_t phy_ports;
    int group, valid;
    SHR_FUNC_INIT_VARS(unit);

    for (group = 0; group < dnx_data_nif.flexe.nof_groups_get(unit); ++group)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, group, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group, &phy_ports));
            if (_SHR_PBMP_MEMBER(phy_ports, flexe_phy))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "The port(%d) is occupied by the flexe group(%d), cannot remove it!\n", flexe_phy, group);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifying 'bcm_dnx_port_remove()'
 *          For additional info goto 'bcm_dnx_port_remove()'  definition.
 */
static shr_error_e
dnx_port_remove_verify(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int is_ilkn_over_fabric = 0;
    uint32 supported_flags;
    dnx_algo_port_info_s port_info;
    uint32 port_speed, min_speed, dummy_flags = 0;
    bcm_gport_t gport;
    bcm_core_t core;
    uint32 out_tm_port;
    int out_tm_ref_count = 0;
    int port_in_lag = 0;
    uint32 app_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** flags verify */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "flags are not supported 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /** valid port verify */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

   /** 
     * Check if the port is part of a LAG.
     * Ports that are part of a LAG must be removed from the LAG first
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));
    if (port_in_lag != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Port %d is a member of a LAG and can not be removed. Need to first remove the port from the LAG.\n",
                     port);
    }

    /*
     * Only the following ports are supporting port removal:
     * 1. NIF ports (excluding ELK and STIF)
     * 2. RCY ports
     * 3. Mirror-recycle ports
     * 4. FlexE PHY and MAC
     * 5. SCH only (LAG scheduler)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /**
     * Check if port is ILKNoFabric
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_ilkn_over_fabric));
    }
    if (((!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, FLEXE, MGMT, OTN_PHY))) &&
         (!DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info)) && (!DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
         && (!DNX_ALGO_PORT_TYPE_IS_FRAMER(unit, port_info))
         && !DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info)) || is_ilkn_over_fabric)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Only NIF port (excluding ELK / STIF/ ILKNoFabric), RCY, mirror-recycle, FlexE client and SCH only can be removed. port=%d\n",
                     port);
    }

    /** Check if there's time slot allocation for the flexe client to be removed */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_client_remove_verify(unit, port));
    }

    /** Check if there's flexe group occupying the phy port to be removed */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_phy_remove_verify(unit, port));
    }

    /*
     * Port can be removed only if its egr and sch shapers were set to zero
     */
    /*
     * Checking speed of egr port shaper
     */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, port, &core, &out_tm_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get(unit, core, out_tm_port, &out_tm_ref_count));

        if (out_tm_ref_count <= 1)
        {
            BCM_GPORT_LOCAL_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &dummy_flags));
            if (port_speed > 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Port %d - port can't be removed if egr shapers were not set to 0. Current egr port shaper speed is %d!\n",
                             port, port_speed);
            }
        }
    }

    /*
     * Checking speed of sch port shaper
     */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        if (out_tm_ref_count <= 1)
        {
            BCM_COSQ_GPORT_E2E_PORT_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &dummy_flags));
            if (port_speed > 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Port %d - port can't be removed if sch shapers were not set to 0. Current sch port shaper speed is %d!\n",
                             port, port_speed);
            }
        }
    }

    /*
     * If this is COE port on PP layer, should disable first
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_app_flags_get(unit, port, &app_flags));
    if (DNX_PORT_IS_COE_PORT(app_flags))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d - COE attribute on PP layer is not disabled!\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Adding new logical port info to port mgmt db (algo_port_*)
 *          For additional info about params goto 'bcm_dnx_port_add()'  definition.
 */
static shr_error_e
dnx_port_mgmt_db_add(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_add_t if_info;
    dnx_algo_port_tm_add_t *tm_info;
    dnx_algo_port_common_add_t *common_info;
    dnx_algo_port_special_interface_add_t special_info;
    int lane, nof_lanes, first_phy_port;
    bcm_port_t master_port;
    int interface_id_h0, interface_id_h1;
    dnx_algo_port_info_indicator_t indicator;
    int out_tm_valid = FALSE;
    int framer_channel = -1;
    bcm_port_t out_tm_master_port;

    SHR_FUNC_INIT_VARS(unit);

    /** init IF info to 0 */
    sal_memset(&if_info, 0, sizeof(dnx_algo_port_if_add_t));

    /** init indicator info to 0 */
    sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));

    /** if ELK port, update flag */
    if (flags & BCM_PORT_ADD_KBP_PORT)
    {
        indicator.is_elk = 1;
    }

    /** add indication for STIF port */
    if (flags & BCM_PORT_ADD_STIF_PORT)
    {
        indicator.is_stif = 1;
    }

    /** add indication for STIF and DATA port */
    if (flags & BCM_PORT_ADD_STIF_AND_DATA_PORT)
    {
        indicator.is_stif_data = 1;
    }

    if ((interface_info->interface == BCM_PORT_IF_ILKN) && (flags & BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT))
    {
        indicator.is_ilkn_over_fabric = 1;
    }

    /** add indication for cross connect port */
    if (flags & BCM_PORT_ADD_CROSS_CONNECT)
    {
        indicator.is_cross_connect = 1;
    }

    /** add indication for flexe client */
    if (flags & BCM_PORT_ADD_FLEXE_MAC)
    {
        indicator.is_flexe_mac = 1;
    }
    if (flags & BCM_PORT_ADD_FLEXE_PHY)
    {
        indicator.is_flexe_phy = 1;
    }
    if (flags & BCM_PORT_ADD_FLEXE_SAR)
    {
        indicator.is_flexe_sar = 1;
    }

    if ((interface_info->interface == BCM_PORT_IF_NIF_ETH)
        && dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_mgmt_port_supported))
    {
        _SHR_PBMP_FIRST(interface_info->phy_pbmp, first_phy_port);
        if (first_phy_port >= dnx_data_nif.phys.first_mgmt_phy_get(unit))
        {
            indicator.is_mgmt = 1;
        }
    }
    if (flags & BCM_PORT_ADD_MC_PORT)
    {
        indicator.is_mc = 1;
    }
    if (flags & BCM_PORT_ADD_OTN_SAR)
    {
        indicator.is_otn_sar = 1;
    }
    if (flags & BCM_PORT_ADD_STAT_ONLY)
    {
        indicator.is_stat_only = 1;
    }
    if (flags & BCM_PORT_ADD_IF_FRAMER_MGMT)
    {
        indicator.is_framer_mgmt = 1;
    }
    /** Convert interface type to port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, interface_info->interface, &indicator, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER(unit, port_info))
    {
        if ((DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info)
             || DNX_ALGO_PORT_USE_FLEXE_CLIENT_CHANNEL(unit, port_info))
            && !DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, mapping_info->channel))
        {
            /*
             * Allocate channel for the following framer ports:
             *   1. FlexE client
             *   2. Framer MAC client
             *   3. Framer SAR client
             *   4. Framer L1 ETH port
             *   5. FlexE unaware interface
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_client_channel_alloc(unit, port_info, &framer_channel));
        }
        else
        {
            framer_channel = mapping_info->channel;
        }
    }
    /** Add to port per port type */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, OTN_PHY)))
    {
        if_info.interface = interface_info->interface;
        if_info.core = mapping_info->core;
        if_info.ilkn_info.is_elk_if = indicator.is_elk;
        if_info.eth_info.is_stif = indicator.is_stif;
        if_info.eth_info.is_stif_data = indicator.is_stif_data;
        if_info.eth_info.is_flexe_phy = indicator.is_flexe_phy;
        if_info.eth_info.is_mgmt = indicator.is_mgmt;
        if_info.is_cross_connect = indicator.is_cross_connect;
        if_info.ilkn_info.is_ilkn_over_fabric = indicator.is_ilkn_over_fabric;
        if_info.eth_info.is_flr_fw_support = _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT);
        if_info.eth_info.ext_txpi_enable = _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_TXPI_MODE_EXT);
        if_info.eth_info.framer_channel = framer_channel;
        /** Set relevant parameters to nif port. */
        /** Interface parameters */
        if_info.interface_offset = interface_info->interface_id;
        if (interface_info->interface == BCM_PORT_IF_ILKN)
        {
            /** Set NIF phy bitmap (0-95) out of phy lanes bitmap */
            BCM_PBMP_ASSIGN(if_info.phys, interface_info->phy_pbmp);

            /** Set ILKN bitmap (0-23) out of the phy lanes bitmap */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_to_ilkn_lanes_get
                            (unit, interface_info->interface_id, &interface_info->phy_pbmp,
                             &if_info.ilkn_info.ilkn_lanes));

        }
        else if (interface_info->interface == BCM_PORT_IF_NIF_ETH)
        {
            BCM_PBMP_ASSIGN(if_info.phys, interface_info->phy_pbmp);
        }
        else if (interface_info->interface == BCM_PORT_IF_OTN)
        {
            BCM_PBMP_ASSIGN(if_info.phys, interface_info->phy_pbmp);
        }
        else
        {
            /*
             * legacy type, when using type that is NOT BCM_PORT_IF_NIF_ETH
             * phy_port is 1-based in legacy calls, thus we need to do minus one
             */
            first_phy_port = interface_info->phy_port - 1;

                /** get number of lanes according to legacy type */
            SHR_IF_ERR_EXIT(dnx_algo_port_legacy_interface_to_nof_lanes_get
                            (unit, interface_info->interface, &nof_lanes));
            for (lane = 0; lane < nof_lanes; ++lane)
            {
                BCM_PBMP_PORT_ADD(if_info.phys, first_phy_port + lane);
            }
        }

        tm_info = &if_info.tm_info;
        common_info = &if_info.common_info;

        /** get DB interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                        (unit, port_info, mapping_info->core, interface_info->interface_id, &if_info.phys, -1,
                         &interface_id_h0, &interface_id_h1));

    }
    /** TM Non-NIF ports OR FlexE clients */
    else if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info) ||
             DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        /** set special tm interface params */
        sal_memset(&special_info, 0, sizeof(dnx_algo_port_special_interface_add_t));

        special_info.framer_info.client_channel = framer_channel;
        special_info.interface = interface_info->interface;
        special_info.core = mapping_info->core;

        /** Set interface offset (used by recycle ports and expected to be -1 for every other port) */
        special_info.interface_offset = interface_info->interface_id;
        tm_info = &special_info.tm_info;
        common_info = &special_info.common_info;

        /** get DB interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                        (unit, port_info, mapping_info->core, interface_info->interface_id, NULL,
                         special_info.framer_info.client_channel, &interface_id_h0, &interface_id_h1));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d is not supported within this function.\n", port);
    }

    common_info->is_channelized = (((flags & BCM_PORT_ADD_CONFIG_CHANNELIZED) ||
                                    DNX_ALGO_PORT_TYPE_IS_ILKN_L1(unit, port_info)) ? 1 : 0);
    if (common_info->is_channelized)
    {
        if (mapping_info->channel != -1)
        {
            common_info->in_channel = mapping_info->channel;
            common_info->out_channel = mapping_info->channel;
        }
        else
        {
            common_info->in_channel = mapping_info->in_channel;
            common_info->out_channel = mapping_info->out_channel;
        }
    }
    else
    {
        common_info->in_channel = 0;
        common_info->out_channel = 0;
    }

    common_info->is_ingress_interleave = _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE) ? 1 : 0;

    /** TDM Interface mode */
    common_info->if_tdm_mode = DNX_ALGO_PORT_IF_NO_TDM;
    if ((flags & BCM_PORT_ADD_IF_TDM_ONLY) ||
        (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE)
         && dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port)))
    {
        common_info->if_tdm_mode = DNX_ALGO_PORT_IF_TDM_ONLY;
    }
    if (flags & BCM_PORT_ADD_IF_TDM_HYBRID)
    {
        common_info->if_tdm_mode = DNX_ALGO_PORT_IF_TDM_HYBRID;
    }

    /** TDM port mode */
    common_info->tdm_mode = DNX_ALGO_PORT_TDM_MODE_NONE;
    if (flags & BCM_PORT_ADD_TDM_PORT)
    {
        if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET)
        {
            common_info->tdm_mode = DNX_ALGO_PORT_TDM_MODE_PACKET;
        }
        else if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_STANDARD
                 || dnx_data_tdm.params.mode_get(unit) == TDM_MODE_OPTIMIZED)
        {
            common_info->tdm_mode = DNX_ALGO_PORT_TDM_MODE_BYPASS;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "TDM port %d created over over a system with no TDM\n", port);
        }
    }
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE)
        && dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port))
    {
        if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_STANDARD
            || dnx_data_tdm.params.mode_get(unit) == TDM_MODE_OPTIMIZED)
        {
            common_info->tdm_mode = DNX_ALGO_PORT_TDM_MODE_BYPASS;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "SAR port can only use TDM ingress bypass mode.\n");
        }
    }

    /** For all TM ports */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        /** Set Switch port params */
        if (mapping_info->tm_port != -1)
        {
            tm_info->out_tm_port = mapping_info->tm_port;
            tm_info->in_tm_port = mapping_info->tm_port;
            tm_info->pp_dsp = mapping_info->tm_port;
        }
        else
        {
            tm_info->out_tm_port = mapping_info->out_tm_port;
            tm_info->in_tm_port = mapping_info->in_tm_port;
            tm_info->pp_dsp = mapping_info->pp_dsp;

        }
        tm_info->in_tm_port_nof = 1;
        tm_info->num_priorities = mapping_info->num_priorities;

        /**
         * Allocate Egress interface and qpairs
         * Egress queuing ports only
         */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info)
            && (!dnx_data_dev_init.general.access_only_get(unit) && !dnx_data_dev_init.general.heat_up_get(unit)))
        {
            SHR_INVOKE_VERIFY_DNXC(dnx_algo_port_out_tm_add_verify(unit, port_info,
                                                                   mapping_info->core, tm_info->out_tm_port,
                                                                   tm_info, interface_id_h0, interface_id_h1));

            /** allocate egress interface*/
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_inteface_to_master_get
                            (unit, interface_id_h0, interface_id_h1, &master_port));
            {
                SHR_IF_ERR_EXIT(dnx_egr_tm_interface_allocate
                                (unit, port, master_port, mapping_info->core, interface_info,
                                 common_info->is_channelized, special_info.framer_info.client_channel,
                                 &(tm_info->egress_interface), &(tm_info->channelized_egress_interface)));
                SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get
                                (unit, mapping_info->core, tm_info->out_tm_port, &out_tm_valid));

                if (!out_tm_valid)
                {
                    int is_rcy = DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info);
                    /** allocate queue or qpair */
                    tm_info->base_q_pair = mapping_info->base_q_pair;
                    SHR_IF_ERR_EXIT(dnx_egr_tm_base_queue_allocate
                                    (unit, flags, port, mapping_info->core, tm_info->egress_interface,
                                     tm_info->num_priorities, common_info->is_channelized, is_rcy,
                                     &tm_info->base_q_pair));
                }
                tm_info->is_egress_interleave =
                    _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE) ? 1 : 0;
                tm_info->is_mc = DNX_ALGO_PORT_TYPE_IS_MC(unit, port_info);
            }
        }
    }

    /** Allocate HRs - E2E SCH ports only */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info)
        && (!dnx_data_dev_init.general.access_only_get(unit) && !dnx_data_dev_init.general.heat_up_get(unit)))
    {
        int with_id = 0;
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /** set base HR - to be allocated later on */
            tm_info->base_hr = (mapping_info->base_hr == -1 ? tm_info->base_q_pair : mapping_info->base_hr);
            with_id = 1;
        }
        else
        {
            if (mapping_info->base_hr != -1)
            {
                tm_info->base_hr = mapping_info->base_hr;
                with_id = 1;
            }
        }

        tm_info->sch_priorities =
            (mapping_info->num_sch_priorities ? mapping_info->num_sch_priorities : tm_info->num_priorities);

        if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info) && tm_info->sch_priorities == 0)
        {
            /** to be backward compatible, use mapping_info->num_priorities for ERP port */
            tm_info->sch_priorities = mapping_info->num_priorities;
        }

        tm_info->sch_priority_propagation_enable =
            _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN) ? 1 : 0;

        if (!out_tm_valid)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_allocate(unit, mapping_info->core, port_info,
                                                           tm_info->sch_priorities,
                                                           tm_info->sch_priority_propagation_enable,
                                                           with_id, &tm_info->base_hr));
        }
        else
        {
            /** out TM valid - take base HR from the port with this out TM port */
            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get(unit, mapping_info->core, tm_info->out_tm_port,
                                                                &out_tm_master_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, out_tm_master_port, &tm_info->base_hr));
        }
    }
    /*
     * Update Framer mgmt DB
     */
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_framer_mgmt_channel_add(unit, framer_channel, port_info));
    }
    /** Add to port per port type */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, OTN_PHY)))
    {
        /** add nif port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_add(unit, port, &if_info));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info) ||
             DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        /** add special tm port */
        special_info.framer_info.is_flexe_mac = indicator.is_flexe_mac;
        special_info.framer_info.is_flexe_sar = indicator.is_flexe_sar;
        special_info.framer_info.is_otn_sar = indicator.is_otn_sar;
        special_info.framer_info.is_cross_connet = indicator.is_cross_connect;
        special_info.framer_info.is_stat_only = indicator.is_stat_only;
        SHR_IF_ERR_EXIT(dnx_algo_port_special_interface_add(unit, port, &special_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d is not supported within this function. \n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifying 'bcm_dnx_port_get()'
 *          For additional info goto 'bcm_dnx_port_get()'  definition.
 */
static shr_error_e
dnx_port_get_verify(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "NULL flags parameter");
    SHR_NULL_CHECK(interface_info, _SHR_E_PARAM, "NULL interface_info parameter");
    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "NULL mapping_info parameter");

    /** valid port verify */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get logical port info from port mgmt db (algo_port_*)
 *          For additional info about params goto 'bcm_dnx_port_get()'  definition.
 */
static shr_error_e
dnx_port_mgmt_db_get(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    dnx_algo_port_info_s port_info;
    int is_channelized;
    int is_egress_interleave;
    int is_ingress_interleave;
    dnx_algo_port_tdm_mode_e tdm_mode;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    dnx_algo_port_info_indicator_t indicator;
    int port_in_lag;
    int is_over_fabric;
    int sch_priority_propagation_enable;
    int is_flr_fw_support;
    int ext_txpi_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** init vars */
    sal_memset(interface_info, 0, sizeof(bcm_port_interface_info_t));
    bcm_port_mapping_info_t_init(mapping_info);
    *flags = 0;

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /** get interface type */
    SHR_IF_ERR_EXIT(dnx_algo_port_to_interface_type_get(unit, port_info, &indicator, &(interface_info->interface)));

    /** Get channel_id */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_channel_get(unit, port, (int *) &(mapping_info->in_channel)));
    SHR_IF_ERR_EXIT(dnx_algo_port_out_channel_get(unit, port, (int *) &(mapping_info->out_channel)));
    if (mapping_info->in_channel == mapping_info->out_channel)
    {
        mapping_info->channel = mapping_info->in_channel;
    }
    /** get flags */
    {
        /** is channelized flag */
        SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
        if (is_channelized)
        {
            *flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
        }
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
        {
            /** is ingress interleave flag */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_ingress_interleave_get(unit, port, &is_ingress_interleave));
            if (is_ingress_interleave)
            {
                *flags |= BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE;
            }
        }
    }

    /** get TM info */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        uint32 tm_port = -1;
        if (DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info))
        {
            /** Get tm port and core */
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, port, &(mapping_info->core), &(mapping_info->pp_dsp)));
            tm_port = mapping_info->pp_dsp;
        }
        if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
        {
            /** Get tm port and core */
            SHR_IF_ERR_EXIT(dnx_algo_port_in_tm_port_get(unit, port,
                                                         &(mapping_info->core), &(mapping_info->in_tm_port)));
            tm_port = mapping_info->in_tm_port;
        }
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /** Get tm port and core */
            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, port,
                                                          &(mapping_info->core), &(mapping_info->out_tm_port)));
            tm_port = mapping_info->out_tm_port;
        }
        if ((tm_port == mapping_info->pp_dsp || !DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info)) &&
            (tm_port == mapping_info->in_tm_port || !DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info) ||
             dnx_data_ingr_reassembly.context.feature_get(unit, dnx_data_ingr_reassembly_context_pp_tm_context)) &&
            (tm_port == mapping_info->out_tm_port || !DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info)))
        {
            mapping_info->tm_port = tm_port;
        }

        /** get TM flags */
        {
            /** is egress interleave flag */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_egress_interleave_get(unit, port, &is_egress_interleave));
            if (is_egress_interleave)
            {
                *flags |= BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE;
            }
            /** For SAR, it is native TDM port, so the TDM flag is irrelevant */
            if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
            {
                /** is tdm flag */
                SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));
                if ((tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS) || (tdm_mode == DNX_ALGO_PORT_TDM_MODE_PACKET))
                {
                    *flags |= BCM_PORT_ADD_TDM_PORT;
                }
                /** interface tdm mode flag */
                SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
                if (if_tdm_mode == DNX_ALGO_PORT_IF_TDM_ONLY)
                {
                    *flags |= BCM_PORT_ADD_IF_TDM_ONLY;
                }
                if (if_tdm_mode == DNX_ALGO_PORT_IF_TDM_HYBRID)
                {
                    *flags |= BCM_PORT_ADD_IF_TDM_HYBRID;
                }
            }
        }

        /** Get EGR Queuing related attributes */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /** Get base_q_pair */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, (int *) &(mapping_info->base_q_pair)));
            /** Get number of port priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, (int *) &(mapping_info->num_priorities)));

        }
    }
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        /** Get core in case the port is not TM port */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &(mapping_info->core)));

        /** Get base_hr */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, (int *) &(mapping_info->base_hr)));

        /** Get number of port sch priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, (int *) &(mapping_info->num_sch_priorities)));

        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port,
                                                                          &sch_priority_propagation_enable));
        if (sch_priority_propagation_enable)
        {
            *flags |= BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN;
        }
    }

    /** Get PP related attributes */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
    {
        /** Get pp port and core */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, port, &(mapping_info->core), &(mapping_info->pp_port)));
    }

    /** Get NIF related attributes */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, OTN_PHY)))
    {
        /** Get number of lanes */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, (int *) &(interface_info->num_lanes)));

        /** Get phys bitmap */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
            if (is_over_fabric)
            {
                *flags |= BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT;
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &(mapping_info->core)));
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &(interface_info->phy_pbmp)));

        /** Get first phy port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, (int *) &(interface_info->phy_port)));
        /** Get interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, (int *) &(interface_info->interface_id)));
        /** Get FLR firmware status */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
        if (is_flr_fw_support)
        {
            *flags |= BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT;
        }
        /** Get TXPI enable status */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ext_txpi_enable_get(unit, port, &ext_txpi_enable));
        if (ext_txpi_enable)
        {
            *flags |= BCM_PORT_ADD_TXPI_MODE_EXT;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
    {
        /** Get interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, (int *) &(interface_info->interface_id)));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
    {
        /** Get forward_port ID */
        if (dnx_data_ingr_reassembly.context.feature_get(unit, dnx_data_ingr_reassembly_context_pp_tm_context))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_forward_port_get(unit, port, &(mapping_info->forward_port)));
        }
    }
    /** Get Framer channel for Framer client */
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info) || DNX_ALGO_PORT_USE_FLEXE_CLIENT_CHANNEL(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, (int *) &(mapping_info->channel)));
    }

    /** is ELK port flag */
    if (indicator.is_elk)
    {
        *flags |= BCM_PORT_ADD_KBP_PORT;
    }

    /** is STIF port */
    if (indicator.is_stif)
    {
        if (indicator.is_stif_data)
        {
            *flags |= BCM_PORT_ADD_STIF_AND_DATA_PORT;
        }
        else
        {
            *flags |= BCM_PORT_ADD_STIF_PORT;
        }
    }

    /** is FlexE PHY port */
    if (indicator.is_flexe_phy)
    {
        *flags |= BCM_PORT_ADD_FLEXE_PHY;
    }

    /** is cross-connect port */
    if (indicator.is_cross_connect)
    {
        *flags |= BCM_PORT_ADD_CROSS_CONNECT;
    }
    /** is FlexE SAR */
    if (indicator.is_flexe_sar)
    {
        *flags |= BCM_PORT_ADD_FLEXE_SAR;
    }
    /** is OTN SAR */
    if (indicator.is_otn_sar)
    {
        *flags |= BCM_PORT_ADD_OTN_SAR;
    }
    /** is FlexE MAC */
    if (indicator.is_flexe_mac)
    {
        *flags |= BCM_PORT_ADD_FLEXE_MAC;
    }
    if (indicator.is_stat_only)
    {
        *flags |= BCM_PORT_ADD_STAT_ONLY;
    }
    if (indicator.is_framer_mgmt)
    {
        *flags |= BCM_PORT_ADD_IF_FRAMER_MGMT;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - counts NOF members in step list
 *
 * \param [in] unit - unit #
 * \param [in] step_list - pointer to step list
 * \param [out] nof_steps - returned result
 * \return
 *   See shr_error_e
 * \remark
 *   list MUST contain at least one member (last member) with
 *   name  == NULL.
 * \see
 *   * None
 */
static shr_error_e
dnx_port_add_remove_step_list_count_steps(
    int unit,
    const dnx_port_add_remove_step_t * step_list,
    int *nof_steps)
{
    int step_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * all lists must have at least one member - the last one - with name = NULL
     */
    for (step_index = 0; step_list[step_index].name != NULL; ++step_index)
    {
        /*
         * Do nothing
         */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Deep free of utilex_seq_step_t
 *          Should be called after 'dnx_port_add_remove_step_list_convert()'
 */
static shr_error_e
dnx_port_add_remove_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    /** recursive destroy */
    for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
    {
        if (step_list[step_index].step_sub_array != NULL)
        {
            SHR_IF_ERR_CONT(dnx_port_add_remove_step_list_destory(unit, step_list[step_index].step_sub_array));
        }
    }

    /** destroy current */
    SHR_FREE(step_list);

    SHR_FUNC_EXIT;
}
/**
 * \brief - Deep conversion of dnx_port_add_remove_step_t to utilex_seq_step_t
 *          The conversion allocate memory that should be freed using 'dnx_step_list_destroy()'
 */
static shr_error_e
dnx_port_add_remove_step_list_convert(
    int unit,
    const dnx_port_add_remove_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list)
{
    int list_size;
    int step_index;
    utilex_seq_step_t *step;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get list size
     */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_step_list_count_steps(unit, dnx_step_list, &list_size));
    list_size++; /** Count the last step too */

    /*
     * Allocate memory for list
     */
    *step_list = NULL;
    SHR_ALLOC_ERR_EXIT((*step_list), sizeof(utilex_seq_step_t) * list_size, "dnx port add remove sequence list",
                       "%s%s%s", "*step_list", EMPTY, EMPTY);
    /*
     * Convert each step
     */
    for (step_index = 0; step_index < list_size; step_index++)
    {
        step = &((*step_list)[step_index]);
        SHR_IF_ERR_EXIT(utilex_seq_step_t_init(unit, step));

        step->step_id = dnx_step_list[step_index].name != NULL ? 1 /** won't be used */ : UTILEX_SEQ_STEP_LAST;
        step->step_name = dnx_step_list[step_index].name;
        step->forward = dnx_step_list[step_index].add_func;
        step->backward = dnx_step_list[step_index].remove_func;
        step->dyn_flags = dnx_step_list[step_index].flag_function;
        if (dnx_step_list[step_index].sub_list != NULL)
        {
            SHR_IF_ERR_EXIT(dnx_port_add_remove_step_list_convert
                            (unit, dnx_step_list[step_index].sub_list, &(step->step_sub_array)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Converting port add \ remove sequnce to utilex_seq
 */
static shr_error_e
dnx_port_add_remove_seq_convert(
    int unit,
    const dnx_port_add_remove_step_t * step_list,
    utilex_seq_t * seq)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init seq structure
     */
    SHR_IF_ERR_EXIT(utilex_seq_t_init(unit, seq));

    /*
     * Configure sequence
     */
    /** Logging */
    seq->bsl_flags = BSL_LOG_MODULE;
    sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "PORT:");
    seq->log_severity = bslSeverityVerbose;

    /** sequence */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_step_list_convert(unit, step_list, &seq->step_list));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Running port add remove sequence forward or backward
 *          The function returns the last step id passed with no errors - to be used by error recovery
 *          And allows to start from a specific step (also to be used by error recovery
 */
static shr_error_e
dnx_port_add_remove_run(
    int unit,
    const dnx_port_add_remove_step_t * step_list,
    int forward,
    int first_step_id,
    int *last_step_id_passed)
{
    utilex_seq_t seq;
    SHR_FUNC_INIT_VARS(unit);

    /** Convert step list to general utilex_seq list */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_seq_convert(unit, step_list, &seq));
    /** set first step id */
    if (first_step_id != -1)
    {
        seq.first_step = first_step_id;
    }
    /** Run list forward, Check error just after utilex seq list destroy */
    SHR_IF_ERR_EXIT(utilex_seq_run(unit, &seq, forward));

exit:
    /** returns the last step id passed with no errors - to be used by error recovery */
    *last_step_id_passed = seq.last_passed_step;
        /** Destroy step list (allocated in 'dnx_port_add_remove_seq_convert()') */
    dnx_port_add_remove_step_list_destory(unit, seq.step_list);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief port add / remove sequence
 *        * Each step in port add / remove step should be added here
 *        * Both add and remove callbacks gets just unit as parameter
 *          The information about the added port can be extracted from port mgmt,
 *          Using function: 'dnx_algo_port_added_port_get()' or 'dnx_algo_port_removed_port_get()'
 *          In addition each step should check if the port is indeed relevant:
 *          For example: NIF step should filter CPU ports
 *        * The last step must be all NULLs
 */
static const dnx_port_add_remove_step_t port_add_remove_sequence[] = {
    /*NAME                ADD_FUNC                                  REMOVE_FUNC                                  SUB_LIST FLAG_CB  */
    {"PortIngress",       dnx_port_ingress_port_add_handle,         dnx_port_ingress_port_remove_handle,         NULL,    NULL},
    {"PortPP",            dnx_port_add_pp_init,                     dnx_port_remove_pp_init,                     NULL,    NULL},
    /** IngressCongestion must come after PortIngress */
    {"IngressCongestion", dnx_cosq_ingress_port_add_handle,         dnx_cosq_ingress_port_remove_handle,         NULL,    NULL},
    /** MirrorRcy must come after IngressCongestion */
    {"MirrorRcy",         dnx_mirror_rcy_port_add_handle,           dnx_mirror_rcy_port_remove_handle,           NULL,    NULL},
    {"EgqQueuing",        dnx_egr_tm_queuing_port_add_handle,       dnx_egr_tm_queuing_port_remove_handle,       NULL,    NULL},
    {"PortEcgm",          dnx_ecgm_port_add,                        dnx_ecgm_port_remove,                        NULL,    device_specific_skip},
    {"ImbPath",           dnx_port_imb_path_port_add_handle,        dnx_port_imb_path_port_remove_handle,        NULL,    NULL},
    /** SchPort must come after EgqQueuing */
    {"SchPort",           dnx_sch_port_add_handle,                  dnx_sch_port_remove_handle,                  NULL,    NULL},
    {"PortTune",          dnx_tune_port_add,                        dnx_tune_port_remove,                        NULL,    device_specific_skip},
    {"Egress Visibility", dnx_visibility_port_egress_enable,        dnx_visibility_port_egress_disable,          NULL,    device_specific_skip},
    {"Disable&Flash",     NULL,                                     dnx_egr_tm_queuing_port_disable_handle,      NULL,    NULL},
    {"Linkscan",          dnx_linkscan_port_init,                   dnx_linkscan_port_remove,                    NULL,    device_specific_skip},
    /** CounterUpdate must be the last step */
    {"CounterUpdate",     dnx_counter_port_add,                     dnx_counter_port_remove,                     NULL,    device_specific_skip},
    {NULL,                NULL,                                     NULL,                                        NULL} /* Last step must be all NULLs*/
};
/* *INDENT-ON* */

static int
dnx_port_add_sch_only_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    dnx_algo_port_info_indicator_t indicator;
    dnx_algo_port_info_s port_info, original_port_info;
    int is_channelized;
    uint32 sch_only_supported_flags;
    bcm_port_if_t interface_type;
    int num_egq_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** null checks */
    SHR_NULL_CHECK(interface_info, _SHR_E_PARAM, "inteface_info");
    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    /** flags */
    sch_only_supported_flags =
        (BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID | BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN | BCM_PORT_ADD_SCH_ONLY);

    SHR_MASK_VERIFY(flags, sch_only_supported_flags, _SHR_E_PARAM, "unexpected flags.\n");

    /** valid logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /*
     * Get port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port=%d already has scheduler\n", port);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_to_interface_type_get(unit, port_info, &indicator, &interface_type));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, interface_type, &indicator, &original_port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, original_port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port=%d cannot have scheduler\n", port);
    }

    if (mapping_info->base_hr == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - base_hr must be specified with BCM_PORT_ADD_SCH_ONLY flag\n", port);
    }
    if (mapping_info->num_sch_priorities == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Port %d - num_sch_priorities must be specified with BCM_PORT_ADD_SCH_ONLY flag\n", port);
    }

    /*
     * Get number of priorities for current port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_egq_priorities));

    if (mapping_info->num_sch_priorities > num_egq_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Port %d - num_sch_priorities (%d) cannot be bigger than num_priorities (%d)\n",
                     port, mapping_info->num_sch_priorities, num_egq_priorities);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN) && (mapping_info->num_sch_priorities == 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - cannot apply priority propagation on 1-priority port\n", port);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN) && !is_channelized)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - cannot apply priority propagation on non channelized port\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_add_sch_only(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    dnx_algo_port_tm_add_t tm_info;
    bcm_core_t core;
    dnx_algo_port_info_s port_info;
    int with_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_add_sch_only_verify(unit, port, flags, interface_info, mapping_info));

    sal_memset(&tm_info, 0, sizeof(dnx_algo_port_tm_add_t));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /**
     * Allocate HRs
     * E2E SCH ports only
     */
    /** when adding scheduler to existing port, HR must be specified by user */
    tm_info.base_hr = mapping_info->base_hr;
    with_id = 1;

    tm_info.sch_priorities = mapping_info->num_sch_priorities;
    tm_info.sch_priority_propagation_enable = _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN) ? 1 : 0;

    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_allocate(unit, core, port_info, tm_info.sch_priorities,
                                                   tm_info.sch_priority_propagation_enable, with_id, &tm_info.base_hr));

    /** add update port DB with the new scheduler */
    dnx_algo_port_sch_only_add(unit, port, &tm_info);

    /*
     * Run scheduler step of port add
     */
    SHR_IF_ERR_EXIT(dnx_sch_port_add_sch(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new port
 *
 * Port might be one of the following:
 * * NIF (Network Interface Port)
 * * KBP
 * * CPU, CPU for ARM
 * * RCY, RCY for mirror
 * * OAMP
 * * OLP
 * * ERP
 *
 * The fuction will create:
 * * logical port instance.
 * * the required physical port (if not already exist)
 * * the required switch port
 * * Map logical port to both physical and switch port
 *
 * Adding a new port won't affect any of the active ports in the system
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Free logical port #
 * \param [in] flags - see BCM_PORT_ADD_F_* \n
 *             Supported flags:
 *                - BCM_PORT_ADD_CONFIG_CHANNELIZED  -  Should be set for any channelized port
 *                                                    (together with the channel id)
 *                - BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID  -   If set the base qpair should set explicitly.
 *                                                      Otherwise, base qpair will be assigned automatically.
 *                - BCM_PORT_ADD_TDM_PORT  -  Set for TDM port.
 *                - BCM_PORT_ADD_IF_TDM_ONLY  -  Set for TDM port which added to interface with TDM ports only.
 *                  BCM_PORT_ADD_IF_TDM_HYBRID  -  Set for TDM port which added to interface with both TDM and NON-TDM ports.
 *                - BCM_PORT_ADD_DONT_PROBE  -   If set, the physical port won't be initialized.
 *                - BCM_PORT_ADD_STIF_PORT  -   If set, the port will be assigned to statistic interface.
 *                - BCM_PORT_ADD_KBP_PORT  -  If set, the port will be assigned to kbp.
 *
 * \param [in] interface_info - Physical port properties:
 *                Supported members:
 *                - interface_type:
 *                    -- BCM_PORT_IF_NIF
 *                    -- BCM_PORT_IF_CPU
 *                    -- BCM_PORT_IF_RCY
 *                    -- BCM_PORT_IF_OLP
 *                    -- BCM_PORT_IF_OAMP
 *                    -- BCM_PORT_IF_ERP
 *                    -- BCM_PORT_IF_SAT
 *                - interface_id  -  interface offset, must be set for any NIF interface.
 *                - phy_pbmp  -  must be set when add Interlaken interface
 * \param [in] mapping_info - Switch port properties
 *                Supported members:
 *                - core - core id of the relevant port
 *                - tm_port - tm port number
 *                - channel - channel id or -1 for non-channelized port.
 *                - num_priorities - number of port priorities.
 *                - base_q_pair - base queue pair id (if not set the API will allocate free base q-pair)
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_add(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    int last_step_passed = -1;
    int first_phy = -1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_port_add_sch_only(unit, port, flags, interface_info, mapping_info));
    }
    else
    {
        /*
         * Verify
         */
        SHR_INVOKE_VERIFY_DNXC(dnx_port_add_verify(unit, port, flags, interface_info, mapping_info));

        /*
         * Add to port mgmt db
         */
        if (interface_info->interface == BCM_PORT_IF_NIF_ETH)
        {
            BCM_PBMP_ITER(interface_info->phy_pbmp, first_phy)
            {
                break;
            }
            BCM_PBMP_COUNT(interface_info->phy_pbmp, interface_info->num_lanes);
            interface_info->interface_id = first_phy / interface_info->num_lanes;
        }

        SHR_IF_ERR_EXIT(dnx_port_mgmt_db_add(unit, port, flags, interface_info, mapping_info));
        /*
         * Run 'port_add_remove_sequence' defined above
         * Skip in access only init,
         * (in order to allow access per port - just need to add the port to DB)
         */
        if (!dnx_data_dev_init.general.access_only_get(unit) && !dnx_data_dev_init.general.heat_up_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_port_add_remove_run(unit, port_add_remove_sequence, 1, -1, &last_step_passed));
        }

        /*
         * Port add done
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_add_process_done(unit, port));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifying 'bcm_dnx_port_remove()' when remove is for scheduler only
 */
static shr_error_e
dnx_port_remove_sch_only_verify(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 supported_flags;
    dnx_algo_port_info_s port_info;
    uint32 port_speed, min_speed, dummy_flags = 0;
    bcm_gport_t gport;
    bcm_port_t sch_port;

    SHR_FUNC_INIT_VARS(unit);

    /** flags verify */
    supported_flags = BCM_PORT_REMOVE_SCH_ONLY;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "flags are not supported 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /** valid port verify */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /*
     * Make sure the port is E2E scheduler
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can not remove scheduler - port does not have scheduler. port=%d\n", port);
    }
    /*
     * Make sure the port is Egress TM
     */
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Scheduler only remove is suppored for egress TM ports only. port=%d\n", port);
    }

    /*
     * Port can be removed only if its egr and sch shapers were set to zero
     */
    /*
     * Checking speed of sch port shaper
     */
    BCM_COSQ_GPORT_E2E_PORT_SET(gport, port);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &dummy_flags));
    if (port_speed > 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Port %d - port can't be removed if sch shapers were not set to 0. Current sch port shaper speed is %d!\n",
                     port, port_speed);
    }

    /** make sure the port is connected to a different scheduler */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_scheduler_get(unit, port, &sch_port));
    if (port == sch_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot remove scheduler of port %d - scheduler is used by the port\n", port);

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Remove any kind of logical port.
 *  The function will:
 *  * Free the logical port
 *  * If the logical port mapped to physical port, and it is only one - will remove the physical port instance.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Valid logical port #
 * \param [in] flags - see BCM_PORT_ADD_F_* (currently there are no supported flags)
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_port_remove_sch_only(
    int unit,
    bcm_port_t port,
    uint32 flags)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Port remove verify
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_remove_sch_only_verify(unit, port, flags));

    /*
     * Run scheduler step of port remove
     */
    SHR_IF_ERR_EXIT(dnx_sch_port_remove_sch(unit, port));
    /*
     * Update DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_remove_sch_only(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Remove any kind of logical port.
 *  The function will:
 *  * Free the logical port
 *  * If the logical port mapped to physical port, and it is only one - will remove the physical port instance.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Valid logical port #
 * \param [in] flags - see BCM_PORT_ADD_F_* (currently there are no supported flags)
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_remove(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int last_step_passed = -1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_REMOVE_SCH_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_port_remove_sch_only(unit, port, flags));
    }
    else
    {

        /*
         * Port remove verify
         */
        SHR_INVOKE_VERIFY_DNXC(dnx_port_remove_verify(unit, port, flags));

        /*
         * Mark as removed port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_remove_process_start(unit, port));

        /*
         * Run 'port_add_remove_sequence' defined above
         */
        SHR_IF_ERR_EXIT(dnx_port_add_remove_run(unit, port_add_remove_sequence, 0, -1, &last_step_passed));
        /*
         * Remove from port mgmt
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_remove(unit, port));
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ports attributes
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Logical port
 * \param [in] flags - Port add flags (BCM_PORT_ADD_F_...)
 * \param [in] interface_info - Interface related Information (for more info \see bcm_dnx_port_add)
 * \param [in] mapping_info - Logical related information (for more info \see bcm_dnx_port_add)
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_port_add API
 */
int
bcm_dnx_port_get(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    bcm_port_t local_port;
    int phy_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port = gport_info.local_port;

    /** verify */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_get_verify(unit, local_port, flags, interface_info, mapping_info));

    /** retrieve information from port db */
    SHR_IF_ERR_EXIT(dnx_port_mgmt_db_get(unit, local_port, flags, interface_info, mapping_info));

    /** legacy compatibility - need to convert phy to be 1 base*/
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, local_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT))
        && interface_info->interface != BCM_PORT_IF_NIF_ETH
        && interface_info->interface != BCM_PORT_IF_ILKN && interface_info->interface != BCM_PORT_IF_OTN)
    {
        interface_info->phy_port += 1;
        BCM_PBMP_CLEAR(interface_info->phy_pbmp);
        for (phy_id = interface_info->phy_port; phy_id < interface_info->phy_port + interface_info->num_lanes; phy_id++)
        {
            BCM_PBMP_PORT_ADD(interface_info->phy_pbmp, phy_id);
        }
    }

exit:
    /** Warmboot skipped, because of a usage between pkt_send and pkt_verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
static shr_error_e
dnx_port_init_port_internal_ucode_add(
    int unit,
    bcm_port_t logical_port,
    int init_db_only,
    const dnx_data_port_static_add_ucode_port_t * ucode_port)
{
    bcm_port_interface_info_t if_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int lane;
    const dnx_data_nif_ilkn_phys_t *ilkn_phys;
    bcm_pbmp_t otn_lanes;
    int is_over_fabric = 0;
    int is_flr_fw_support = 0;
    int is_ext_txpi_mode = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Call 'bcm_port_add' for each port defined by soc property 'ucode_port'
     */
    if (ucode_port->interface != BCM_PORT_IF_NULL)
    {
        /** init vars */
        sal_memset(&if_info, 0, sizeof(bcm_port_interface_info_t));
        bcm_port_mapping_info_t_init(&mapping_info);
        flags = 0;

        /** Set relevant input vars for 'bcm_port_add'*/
        /** interface */
        if_info.interface = ucode_port->interface;
        if_info.interface_id = ucode_port->interface_offset;

        /** interlaken phy bitmap, set use_pbmp_flag */
        if (ucode_port->interface == BCM_PORT_IF_ILKN)
        {
            ilkn_phys = dnx_data_nif.ilkn.phys_get(unit, ucode_port->interface_offset);
            SHR_NULL_CHECK(ilkn_phys, _SHR_E_CONFIG, "interface offset is out of bound");
            BCM_PBMP_ASSIGN(if_info.phy_pbmp, ilkn_phys->bitmap);
        }
        else if (ucode_port->interface == BCM_PORT_IF_NIF_ETH)
        {
            BCM_PBMP_CLEAR(if_info.phy_pbmp);
            for (lane = 0; lane < ucode_port->nof_lanes; ++lane)
            {
                /*
                 * phy is zero based, calculate each phy and add to PBMP
                 */
                BCM_PBMP_PORT_ADD(if_info.phy_pbmp, (ucode_port->interface_offset * ucode_port->nof_lanes) + lane);
            }
        }
        else if (ucode_port->interface == BCM_PORT_IF_OTN)
        {
            if (dnx_data_nif.global.otn_supported_get(unit)
                && dnx_data_nif.otn.feature_get(unit, dnx_data_nif_otn_is_supported))
            {
                BCM_PBMP_CLEAR(otn_lanes);
                BCM_PBMP_ASSIGN(otn_lanes, dnx_data_lane_map.otn.logical_lanes_get(unit, logical_port)->val);
                if (BCM_PBMP_IS_NULL(otn_lanes))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "otn_lanes_%d=bitmap is not specified in Soc Properties.\n",
                                 logical_port);
                }
                BCM_PBMP_ASSIGN(if_info.phy_pbmp, otn_lanes);
            }
        }
        else
        {
            /** first PHY one based legacy support */
            if_info.phy_port = ucode_port->interface_offset * ucode_port->nof_lanes + 1;
        }

        if (ucode_port->interface == BCM_PORT_IF_ILKN)
        {
            is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, ucode_port->interface_offset)->is_over_fabric;
            if (is_over_fabric)
            {
                flags |= BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT;
            }
        }
        is_flr_fw_support = dnx_data_port.static_add.flr_config_get(unit, logical_port)->flr_fw_support;
        if (is_flr_fw_support)
        {
            flags |= BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT;
        }
        is_ext_txpi_mode = dnx_data_port.static_add.txpi_mode_get(unit, logical_port)->val;
        if (is_ext_txpi_mode)
        {
            flags |= BCM_PORT_ADD_TXPI_MODE_EXT;
        }
        /** switch port */
        mapping_info.core = ucode_port->core;
        if (ucode_port->out_tm_port != -1)
        {
            if (!dnx_data_ingr_reassembly.context.feature_get(unit, dnx_data_ingr_reassembly_context_pp_tm_context))
            {
                mapping_info.in_tm_port = ucode_port->tm_port;
            }
            mapping_info.pp_dsp = ucode_port->tm_port;
            mapping_info.out_tm_port = ucode_port->out_tm_port;
        }
        else
        {
            mapping_info.tm_port = ucode_port->tm_port;
        }
        mapping_info.channel = ucode_port->channel;
        mapping_info.num_priorities = ucode_port->num_priorities;
        mapping_info.num_sch_priorities = ucode_port->num_sch_priorities;
        /** base queue pair with id flag */
        if (ucode_port->base_q_pair != -1)
        {
            mapping_info.base_q_pair = ucode_port->base_q_pair;
            flags |= BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID;
        }
        /** base HR */
        if (ucode_port->base_hr != -1)
        {
            mapping_info.base_hr = ucode_port->base_hr;
        }

        /** forward_port */
        /** attributes: kbp, stif, tdm */
        if (ucode_port->is_stif)
        {
            flags |= BCM_PORT_ADD_STIF_PORT;
        }
        if (ucode_port->is_stif_data)
        {
            flags |= BCM_PORT_ADD_STIF_AND_DATA_PORT;
        }
        if (ucode_port->is_kbp)
        {
            flags |= BCM_PORT_ADD_KBP_PORT;
        }
        if (ucode_port->is_flexe_phy)
        {
            flags |= BCM_PORT_ADD_FLEXE_PHY;
        }
        if (ucode_port->is_cross_connect)
        {
            flags |= BCM_PORT_ADD_CROSS_CONNECT;
        }
        if (ucode_port->is_ingress_interleave)
        {
            flags |= BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE;
        }
        if (ucode_port->is_egress_interleave)
        {
            flags |= BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE;
        }
        if (ucode_port->is_tdm)
        {
            flags |= BCM_PORT_ADD_TDM_PORT;
        }
        if (ucode_port->is_if_tdm_only)
        {
            flags |= BCM_PORT_ADD_IF_TDM_ONLY;
        }
        if (ucode_port->sch_priority_propagation_en)
        {
            flags |= BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN;
        }
        if (ucode_port->is_if_tdm_hybrid)
        {
            flags |= BCM_PORT_ADD_IF_TDM_HYBRID;
        }
        /** If ILKN or channelized set flag is_channelized */
        if (ucode_port->channel != -1)
        {
            flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
        }
        if (ucode_port->is_mc)
        {
            flags |= BCM_PORT_ADD_MC_PORT;
        }

        /** add the port */
        if (init_db_only)
        {
            SHR_IF_ERR_EXIT(dnx_port_mgmt_db_add(unit, logical_port, flags, &if_info, &mapping_info));
        }
        else
        {
            if (!sw_state_is_warm_boot(unit))
            {
                SHR_IF_ERR_EXIT_WITH_LOG(bcm_dnx_port_add(unit, logical_port, flags, &if_info, &mapping_info),
                                         "Adding port %d failed.\n%s%s", logical_port, EMPTY, EMPTY);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_internal_add(
    int unit,
    int init_db_only)
{
    const dnx_data_port_static_add_ucode_port_t *ucode_port;
    bcm_port_t logical_port;
    bcm_pbmp_t rcy_mirror_pbmp;
    int max_num_ports;

    SHR_FUNC_INIT_VARS(unit);

    max_num_ports = dnx_data_port.general.fabric_port_base_get(unit);
    BCM_PBMP_CLEAR(rcy_mirror_pbmp);
    for (logical_port = 0; logical_port < max_num_ports; logical_port++)
    {
        ucode_port = dnx_data_port.static_add.ucode_port_get(unit, logical_port);

        /** delay rcy_mirror_ports to be last since their creation sequence depends on other ports (forward ports )*/
        {
            SHR_IF_ERR_EXIT(dnx_port_init_port_internal_ucode_add(unit, logical_port, init_db_only, ucode_port));
        }
    }

    /** add delayed ports */
    BCM_PBMP_ITER(rcy_mirror_pbmp, logical_port)
    {
        ucode_port = dnx_data_port.static_add.ucode_port_get(unit, logical_port);
        SHR_IF_ERR_EXIT(dnx_port_init_port_internal_ucode_add(unit, logical_port, init_db_only, ucode_port));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_init_port_add(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_init_port_internal_add(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the padding size for every new
 *        port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_init_pad_size_config(
    int unit,
    bcm_port_t port)
{
    int padding_size;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, MGMT)))
    {
        /** check if there is a new value from soc property */
        padding_size = dnx_data_port.static_add.eth_padding_get(unit, port)->pad_size;

        /** use API to set new value */
        if (padding_size != 0)
        {
            bcm_dnx_port_control_set(unit, port, bcmPortControlPadToSize, padding_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the stat oversize for every new
 *        port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_init_stat_oversize_config(
    int unit,
    bcm_port_t port)
{
    int stat_jumbo;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)))
    {
        /** check if there is a new value from soc property */
        stat_jumbo = dnx_data_mib.general.stat_jumbo_get(unit);
        SHR_IF_ERR_EXIT(bcm_dnx_port_control_set(unit, port, bcmPortControlStatOversize, stat_jumbo));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert per port properties to BCM API calls (excluding NIF modules)
 */
static shr_error_e
dnx_port_init_special_ifs_properties_set(
    int unit)
{
    bcm_pbmp_t nif_ports_bitmap;
    bcm_pbmp_t tm_master_ports_bitmap;
    bcm_port_t logical_port;
    int port_speed;
    bcm_port_resource_t *resource = NULL;
    int port_index = 0;
    int nof_ports;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get special interfaces ports bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &nif_ports_bitmap));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &tm_master_ports_bitmap));
    BCM_PBMP_REMOVE(tm_master_ports_bitmap, nif_ports_bitmap);

    BCM_PBMP_COUNT(tm_master_ports_bitmap, nof_ports);
    SHR_ALLOC(resource, nof_ports * sizeof(bcm_port_resource_t), "special ifs resource", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Speed
     */
    BCM_PBMP_ITER(tm_master_ports_bitmap, logical_port)
    {

        port_speed = dnx_data_port.static_add.speed_get(unit, logical_port)->val;
        if (port_speed <= 0)
        {
            port_speed = dnx_data_port.static_add.default_speed_for_special_if_get(unit);
        }
        bcm_port_resource_t_init(&resource[port_index]);

        resource[port_index].port = logical_port;
        resource[port_index].speed = port_speed;
        /** No need to tune ECGM in bcm_port_resource_multi_set API, it is done in separate tuning function */
        resource[port_index].flags = BCM_PORT_RESOURCE_ECGM_AUTO_ADJUST_DISABLE;
        port_index++;
    }

    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, nof_ports, resource));

exit:
    SHR_FREE(resource);
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_port_init_serdes_tx_taps_config(
    int unit,
    bcm_port_t port)
{
    bcm_port_phy_tx_t tx;
    int lane_rate = 0;
    bcm_pbmp_t phys;
    dnx_algo_port_info_s port_info;
    int num_lane, speed, port_has_speed;
    SHR_FUNC_INIT_VARS(unit);

    bcm_port_phy_tx_t_init(&tx);

    /*
     * Get SoC property TX FIR configuration
     */
    tx.main = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->main;
    tx.pre = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->pre;
    tx.post = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->post;
    tx.pre2 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->pre2;
    tx.pre3 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->pre3;
    tx.post2 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->post2;
    tx.post3 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->post3;
    tx.tx_tap_mode = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->tx_tap_mode;
    tx.signalling_mode = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->signalling_mode;

    /*
     * Get port lane rate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &port_has_speed));
    if (port_has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
    }
    else
    {
        speed = 0;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, OTN_PHY)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
        BCM_PBMP_COUNT(phys, num_lane);
        lane_rate = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1))
            || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) ? speed : (speed / num_lane);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        lane_rate = speed;
    }

    /*
     * If main is INVALID, this means SoC property was not configured.
     * For PAM4 rate - get the default values from phymod.
     * For NRZ rates - do nothing. The default values are configured in portmod attach.
     */
    if ((tx.main == DNXC_PORT_TX_FIR_INVALID_MAIN_TAP) && (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate)))
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_taps_default_get(unit, port, 1, &tx));
    }

    /*
     * If main is valid, call TX taps set API.
     */
    if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_phy_tx_set(unit, port, &tx));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the FLR timeout value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_init_flr_config(
    int unit,
    bcm_port_t port)
{
    int is_flr_fw_support, flr_enable;
    dnx_algo_port_info_s port_info;
    bcm_port_flr_timeout_t timeout;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
        if (is_flr_fw_support)
        {
            /*
             * Get timeout value from SOC
             */
            timeout = dnx_data_port.static_add.flr_config_get(unit, port)->timeout;
            flr_enable = (timeout == bcmPortFlrTimeoutInvaild) ? 0 : 1;
            SHR_IF_ERR_EXIT(bcm_dnx_port_fast_link_recovery_enable_set(unit, port, flr_enable, timeout));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the Serdes timestamp
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_init_phy_timestamp(
    int unit,
    bcm_port_t port)
{
    int timesync_enable, one_step_enable;
    dnx_algo_port_info_s port_info;
    bcm_port_phy_timesync_config_t timesync_phy_config;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        timesync_enable = dnx_data_port.static_add.timestamp_phy_init_get(unit, port)->timestamp_enable;
        one_step_enable = dnx_data_port.static_add.timestamp_phy_init_get(unit, port)->one_step_enable;

        if (timesync_enable)
        {
            bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
            timesync_phy_config.flags |= BCM_PORT_PHY_TIMESYNC_ENABLE;
            if (one_step_enable)
            {
                timesync_phy_config.flags |= BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE;
            }

            SHR_IF_ERR_EXIT(bcm_dnx_port_phy_timesync_config_set(unit, port, &timesync_phy_config));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert per port properties to BCM API calls (Only NIF module)
 */
static shr_error_e
dnx_port_init_nif_properties_set(
    int unit)
{
    bcm_pbmp_t nif_master_ports_bitmap;
    bcm_port_t nif_port;
    int nof_ports;
    int port_index = 0;
    int tx_pam4_precoder;
    int lp_tx_precoder;
    int precoder_support;
    int link_training;
    int rx_speed = 0;
    const dnx_data_port_static_add_speed_t *port_speed;
    bcm_port_resource_t *resource = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Speed
     */

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &nif_master_ports_bitmap));

    BCM_PBMP_COUNT(nif_master_ports_bitmap, nof_ports);
    SHR_ALLOC(resource, nof_ports * sizeof(bcm_port_resource_t), "NIF resource", "%s%s%s", EMPTY, EMPTY, EMPTY);

    BCM_PBMP_ITER(nif_master_ports_bitmap, nif_port)
    {
        port_speed = dnx_data_port.static_add.speed_get(unit, nif_port);
        if (port_speed->val == DNXC_PORT_INVALID_SPEED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF port MUST have speed");
        }
        /*
         * According to DS, 20G is not supported"
         */
        if (port_speed->val == 20000)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF port CAN'T support 20000 speed");
        }
        /*
         * padding size config
         */
        SHR_IF_ERR_EXIT(dnx_port_init_pad_size_config(unit, nif_port));
        /*
         * statistics oversize config
         */

        if (!(dnx_data_nif.portmod.feature_get(unit, dnx_data_nif_portmod_not_implemented)))
        {
            SHR_IF_ERR_EXIT(dnx_port_init_stat_oversize_config(unit, nif_port));
        }
        /*
         * Skip setting port properties in case of speed==-1
         */
        if (port_speed->val == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            nof_ports--;
            continue;
        }
        /*
         * Link training cannot co-exist with FLR firmware
         */
        if ((dnx_data_port.static_add.link_training_get(unit, nif_port)->val == 1) &&
            dnx_data_port.static_add.flr_config_get(unit, nif_port)->flr_fw_support)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: if FLR firmware is used, enabling link training is not allowed.", nif_port);
        }
        bcm_port_resource_t_init(&resource[port_index]);
        resource[port_index].port = nif_port;
        resource[port_index].speed = port_speed->val;
        resource[port_index].fec_type = dnx_data_port.static_add.fec_type_get(unit, nif_port)->val;
        resource[port_index].link_training = dnx_data_port.static_add.link_training_get(unit, nif_port)->val;
        resource[port_index].phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

        if (dnx_data_port.static_add.feature_get(unit, dnx_data_port_static_add_base_flexe_instance_exists))
        {
            resource[port_index].base_flexe_instance =
                dnx_data_port.static_add.base_flexe_instance_get(unit, nif_port)->val;
        }

        if (dnx_data_nif.global.otn_supported_get(unit)
            && dnx_data_nif.otn.feature_get(unit, dnx_data_nif_otn_is_supported))
        {
            resource[port_index].logical_otn_lane = dnx_data_lane_map.otn.logical_otn_lane_get(unit, nif_port)->val;
        }
        if ((dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_not_implemented)))
        {
            /** set Tx speed in mgmt db */
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_set
                            (unit, resource[port_index].port, DNX_ALGO_PORT_SPEED_F_TX, resource[port_index].speed));
            /** set Rx speed in mgmt db */
            rx_speed =
                (resource[port_index].flags & BCM_PORT_RESOURCE_ASYMMETRICAL) ?
                resource[port_index].rx_speed : resource[port_index].speed;
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_set
                            (unit, resource[port_index].port, DNX_ALGO_PORT_SPEED_F_RX, rx_speed));
        }
        else
        {

            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_default_get(unit, nif_port, 0, &resource[port_index]));

            /** No need to tune ECGM in bcm_port_resource_multi_set API, it is done in separate tuning function
              * The flag is added after bcm_port_resource_default_get API, because flags must be 0 for it */
            resource[port_index].flags |= BCM_PORT_RESOURCE_ECGM_AUTO_ADJUST_DISABLE;

            /** override phy lane config bits with values from SoC property */
            SHR_IF_ERR_EXIT(dnx_port_resource_lane_config_soc_properties_get(unit, nif_port, &resource[port_index]));
        }
        port_index++;
    }
    if (!(dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_not_implemented)))
    {
        /*
         * ILKN lane order remap
         */
        SHR_IF_ERR_EXIT(dnx_port_init_ilkn_lane_order_set(unit));

        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, nof_ports, resource));

        for (port_index = 0; port_index < nof_ports; ++port_index)
        {
            link_training = resource[port_index].link_training;
            nif_port = resource[port_index].port;

            /** set precoder according to SoC property */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_precoder_supported_get(unit, nif_port, &precoder_support));
            tx_pam4_precoder = dnx_data_port.static_add.tx_pam4_precoder_get(unit, nif_port)->val;
            if ((precoder_support == 0) && (tx_pam4_precoder == 1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: port does not support tx_pam4_precoder.", nif_port);
            }

            if ((link_training == 0) && (precoder_support == 1))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                                (unit, nif_port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, tx_pam4_precoder));
            }
            else if ((link_training == 1) && (tx_pam4_precoder == 1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "port %d: if link training is enabled, enabling tx_pam4_precoder is not allowed.",
                             nif_port);
            }

            /** set lp precoder according to SoC property */
            lp_tx_precoder = dnx_data_port.static_add.lp_tx_precoder_get(unit, nif_port)->val;
            if ((precoder_support == 0) && (lp_tx_precoder == 1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: port does not support lp_tx_precoder.", nif_port);
            }

            if ((link_training == 0) && (precoder_support == 1))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                                (unit, nif_port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, lp_tx_precoder));
            }
            else if ((link_training == 1) && (lp_tx_precoder == 1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "port %d: if link training is enabled, enabling lp_tx_precoder is not allowed.", nif_port);
            }

            /*
             * config TX FIR TAPs
             */
            if (link_training == 0)
            {
                SHR_IF_ERR_EXIT(dnx_port_init_serdes_tx_taps_config(unit, nif_port));
            }
            SHR_IF_ERR_EXIT(dnx_port_init_flr_config(unit, nif_port));
            SHR_IF_ERR_EXIT(dnx_port_init_phy_timestamp(unit, nif_port));
        }

        /*
         * Enable NIF ports
         */
        BCM_PBMP_ITER(nif_master_ports_bitmap, nif_port)
        {
            port_speed = dnx_data_port.static_add.speed_get(unit, nif_port);
            if (port_speed->val > 0)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, nif_port, 1));
            }
        }
    }

exit:
    SHR_FREE(resource);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Take PP port soc-properties and call the right BCM PP port APIs accordingly.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_init_packet_processing_properties_set(
    int unit)
{
    bcm_port_t port;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    int header_type_in;
    int header_type_out;
    bcm_pbmp_t logical_ports;
    dnx_algo_port_info_s port_info;
    int is_lag;
    int first_header_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, port)
    {
        if (dnx_data_port.static_add.header_type_get(unit, port)->header_type_in == BCM_SWITCH_PORT_HEADER_TYPE_NONE)
        {
            header_type_in = dnx_data_port.static_add.header_type_get(unit, port)->header_type;
        }
        else
        {
            header_type_in = dnx_data_port.static_add.header_type_get(unit, port)->header_type_in;
        }

        key.type = bcmSwitchPortHeaderType;
        key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
        value.value = header_type_in;

        /** Set the incoming header type */
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, port, key, value));
        if ((header_type_in == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
            || (header_type_in == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
            || (header_type_in == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
            || (header_type_in == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP))
        {
            first_header_offset = dnx_data_port.general.first_header_size_get(unit, port)->first_header_size;
            if (first_header_offset != 0)
            {
                /** Set the size of the first header to skip */
                SHR_IF_ERR_EXIT(bcm_dnx_port_control_set
                                (unit, port, bcmPortControlFirstHeaderSize, first_header_offset));
            }
        }

        if (dnx_data_port.static_add.header_type_get(unit, port)->header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_NONE)
        {
            header_type_out = dnx_data_port.static_add.header_type_get(unit, port)->header_type;
        }
        else
        {
            header_type_out = dnx_data_port.static_add.header_type_get(unit, port)->header_type_out;
        }
        if (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2 is not supported on egress!");
        }

        key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT;
        value.value = header_type_out;

        /** Get the port`s type */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &is_lag));

        if (DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_info, is_lag))
        {
            /** Set the outgoing header type */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, port, key, value));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Handle removing of ingress packet processing properties from a certain ingress pp port.
 */
static shr_error_e
dnx_port_packet_processing_properties_remove_internal_ingress(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id)
{
    uint32 entry_handle_id;
    uint32 invalid_system_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);

    SHR_IF_ERR_EXIT(dnx_port_untagged_vlan_set_internal(unit, pp_port, core_id, 0));
    /** Disable learning*/
    SHR_IF_ERR_EXIT(dnx_port_pp_learn_set(unit, core_id, pp_port, BCM_PORT_LEARN_FWD));

    /*
     * Configure the INGRESS_LLR_CONTEXT_PROPERTIES table:
     * Disable LLVP and Initial VID per port.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_LLR_CONTEXT_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_VID_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_CS_PORT_CS_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_RAW_PORT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SEL_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_PTC_PROFILE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FILTER_SA_EQ_DA_ENABLE, INST_SINGLE, 0);
    /*
     * Disable acceptable frame type filter
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure the INGRESS_PP_PORT table: disable learn-lif.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_PP_PORT, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIF_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT,
                                 INST_SINGLE, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_A1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SAME_INTERFACE_FILTER_PROFILE,
                                 INST_SINGLE, DBAL_ENUM_FVAL_IN_PORT_SAME_INTERFACE_FILTER_PROFILE_DISABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_IF, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE,
                                 invalid_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Handle removing of egress packet processing properties from a certain egress pp port
 * See dnx_port_pp_egress_set and dnx_port_pp_egress_hw_set.
 * The HW clear is done at dnx_port_pp_egress_unset
 */
static shr_error_e
dnx_port_packet_processing_properties_remove_internal_egress(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/** See .h */
shr_error_e
dnx_port_packet_processing_properties_remove_internal(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id,
    int is_ingress,
    int is_egress)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_packet_processing_properties_remove_internal_ingress(unit, pp_port, core_id));
    }

    if (is_egress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_packet_processing_properties_remove_internal_egress(unit, pp_port, core_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See h. for reference
 */
shr_error_e
dnx_port_packet_processing_properties_remove(
    int unit,
    bcm_port_t port,
    int is_ingress,
    int is_egress)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_packet_processing_properties_remove_internal
                        (unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                         gport_info.internal_port_pp_info.core_id[pp_port_index], is_ingress, is_egress));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remap the ILKN logical lane order.
 *    Lane order remapping feature is only relevant for ILKN
 *    and ELK ports.
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
static int
dnx_port_init_ilkn_lane_order_set(
    int unit)
{
    int ilkn_id, ilkn_lane;
    int phy_iter;
    int ilkn_lane_id[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = { 0 };
    int count = 0, i;
    bcm_pbmp_t nif_phys;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    const dnx_data_nif_ilkn_phys_t *ilkn_phys;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &logical_ports));

    BCM_PBMP_ITER(logical_ports, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        /*
         * This feature is only relevant for ILKN and ELK ports
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
        {
            count = 0;
            sal_memset(ilkn_lane_id, 0, DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF * sizeof(int));

            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
            ilkn_phys = dnx_data_nif.ilkn.phys_get(unit, ilkn_id);

            /*
             * Here we need to check weather the ilkn lanes are presented in bitmap format, if no: the order is set
             * from ilkn_lanes soc property if yes: Get the ILKN lane re-mapping configuration from ilkn_lanes_map Soc
             * property
             */
            if (ilkn_phys->array_order_valid)
            {
                BCM_PBMP_COUNT(ilkn_phys->bitmap, count);
                for (i = 0; i < count; i++)
                {
                    ilkn_lane_id[i] = ilkn_phys->lanes_array[i];
                }
            }
            else
            {
                /*
                 * If legacy mode is not supported for device return error 
                 */
                if (!(dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_legacy_lane_format)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "HEX ILKN lanes presentation is not supported in device.\n");
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));
                count = 0;
                BCM_PBMP_ITER(nif_phys, phy_iter)
                {
                    /*
                     * Get the ILKN lane re-mapping configuration from ilkn_lanes_map Soc property
                     */
                    ilkn_lane = dnx_data_lane_map.ilkn.remapping_get(unit, phy_iter, ilkn_id)->ilkn_lane_id;
                    ilkn_lane_id[ilkn_lane] = phy_iter;
                    count++;
                }
            }

            SHR_IF_ERR_EXIT(bcm_dnx_port_ilkn_lane_map_set(unit, 0, port, count, ilkn_lane_id));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_properties_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * NIF
     */
    SHR_IF_ERR_EXIT(dnx_port_init_nif_properties_set(unit));

    
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_application_v1))
    {

        /*
         * Special interfaces (TM ports excluding NIF)
         */
        SHR_IF_ERR_EXIT(dnx_port_init_special_ifs_properties_set(unit));
    }

    /*
     * PP
     */
    SHR_IF_ERR_EXIT(dnx_port_init_packet_processing_properties_set(unit));

    /*
     * Set per port oam profile, this should be set after header type is set
     */
    SHR_IF_ERR_EXIT(dnx_oam_port_profile_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_lane_map_set(
    int unit)
{
    int flags = 0, lane_id;
    int map_size = dnx_data_nif.phys.nof_phys_get(unit);
    bcm_port_lane_to_serdes_map_t lane2serdes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set lane mapping for NIF phys
     */
    /** Get lane mapping info from dnx-data */
    for (lane_id = 0; lane_id < map_size; lane_id++)
    {
        lane2serdes[lane_id].serdes_rx_id = dnx_data_lane_map.nif.mapping_get(unit, lane_id)->serdes_rx_id;
        lane2serdes[lane_id].serdes_tx_id = dnx_data_lane_map.nif.mapping_get(unit, lane_id)->serdes_tx_id;
    }
    /** Store lane mapping info to swstate */
    SHR_IF_ERR_EXIT(bcm_dnx_port_lane_to_serdes_map_set(unit, flags, map_size, lane2serdes));

exit:
    SHR_FUNC_EXIT;

}
