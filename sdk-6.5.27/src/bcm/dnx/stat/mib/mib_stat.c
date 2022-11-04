/** \file mib_stat.c
 * 
 *
 * MIB stat procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/boot.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/mib/algo_mib.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_signal_quality.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/auto_generated/dnx_stat_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/epfc/epfc.h>
#include <bcm_int/common/rx.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/pkt_rx.h>
#endif
#include <soc/counter.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_rx_access.h>
#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#endif

#include <sal/types.h>
#include "mib_stat_dbal.h"
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnxc/dnxc_ha.h>
#include <phymod/phymod.h>

/**
 * \brief - padding size definition for snmpIfInOctets feature.
 */
#define DNX_MIB_PAD_SIZE_MIN    (64)
#define DNX_MIB_PAD_SIZE_MAX    (96)
#define DNX_MIB_PAD_SIZE_IS_IN_RANGE(pad_size) \
    (pad_size >= DNX_MIB_PAD_SIZE_MIN && pad_size <= DNX_MIB_PAD_SIZE_MAX)

#define PM_MAX_LANES          8

/** see .h file */
shr_error_e
dnx_mib_init(
    int unit)
{
    const dnx_data_mib_general_stat_pbmp_t *stat_pbmp;
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;
    bcm_pbmp_t fabric_ports_bitmap;
    soc_controlled_counter_t **controlled_counters_p_p;
    soc_controlled_counter_t *dnx_controlled_counters_array = NULL;
    int counter_id;
    int allocated_array_need_free = FALSE;
    int allocated_db_need_free = FALSE;
    VOL pbmp_t *counter_pbmp_p;

    SHR_FUNC_INIT_VARS(unit);

    /** allocate the database for storing counter values per row */
    dnx_mib_counter_values_database_init(unit);
    allocated_db_need_free = TRUE;

    /** allocate controlled counters array */
    SHR_ALLOC_SET_ZERO(dnx_controlled_counters_array, (sizeof(soc_controlled_counter_t) * (dnx_mib_counter_nof + 1)),
                       "DNX controlled counters array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    /** construct controlled counters array */
    for (counter_id = 0; counter_id < dnx_mib_counter_nof; counter_id++)
    {
        /** get counter data */
        counter_data = dnx_data_mib.controlled_counters.counter_data_get(unit, counter_id);

        dnx_controlled_counters_array[counter_id].flags = 0;
        dnx_controlled_counters_array[counter_id].short_cname = counter_data->doc;
        dnx_controlled_counters_array[counter_id].cname = counter_data->doc;
        /*
         * Currently it's assumed that array index is the same as counter_id.
         * If this will change - then different function will require a change as well, such as the usage of
         * dnx_stat_get_by_counter_idx by the bcm API.
         */
        dnx_controlled_counters_array[counter_id].counter_id = counter_id;
        dnx_controlled_counters_array[counter_id].counter_idx = COUNTER_IDX_NOT_INITIALIZED;
        dnx_controlled_counters_array[counter_id].controlled_counter_f = dnx_mib_counter_get;
        dnx_controlled_counters_array[counter_id].controlled_counter_clear_f = dnx_mib_counter_clear;
        dnx_controlled_counters_array[counter_id].controlled_counter_length_f = dnx_mib_counter_length_get;
    }

    /** mark last entry as invalid */
    dnx_controlled_counters_array[counter_id].controlled_counter_f = NULL;

    /** attach the controlled counters */
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, CONTROLLED_COUNTERS, (void **) (&controlled_counters_p_p)));
    *controlled_counters_p_p = dnx_controlled_counters_array;
    allocated_array_need_free = TRUE;
    SHR_IF_ERR_EXIT(soc_counter_attach(unit));

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(mib_stat_init(unit));
    }
    /*
     * During Warm boot, add nif ports to counter bitmap here.
     * Because dnx_counter_port_add() was not called in adding ports stage
     */
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_PBMP, (void **) (&counter_pbmp_p)));
    if (sw_state_is_warm_boot(unit))
    {
        bcm_pbmp_t counter_pbmp;
        /*
         * Get counter pbmp from SW DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_mib_counter_pbmp_get(unit, &counter_pbmp));
        BCM_PBMP_ASSIGN(*counter_pbmp_p, counter_pbmp);
    }
    else
    {
        stat_pbmp = dnx_data_mib.general.stat_pbmp_get(unit);
        /*
         * This stage is after fabric init. Here, only fabric ports are
         * added to counter bitmap. For nif ports, they are added in
         * dnx_counter_port_add() during adding ports stage.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
        BCM_PBMP_AND(fabric_ports_bitmap, stat_pbmp->pbmp);
        /*
         * update counters bitmap in case counter thread is not start
         */
        BCM_PBMP_ASSIGN(*counter_pbmp_p, fabric_ports_bitmap);
        /*
         * Update counter pbmp SW DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_mib_counter_pbmp_set(unit, *counter_pbmp_p));
    }

#ifdef INCLUDE_XFLOW_MACSEC
    if (dnx_data_macsec.general.is_macsec_enabled_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_stat_init(unit));
    }
#endif
    allocated_db_need_free = FALSE;
    allocated_array_need_free = FALSE;

exit:
    if (allocated_db_need_free)
    {
        dnx_mib_counter_values_database_deinit(unit);
    }
    if (allocated_array_need_free)
    {
        SHR_FREE(dnx_controlled_counters_array);
        *controlled_counters_p_p = NULL;
    }

    /**
     * There is no leak, the allocated mib counters array - "dnx_controlled_counters_array" is pointed by
     *  controlled_counters_p_p and used by counter thread.
     */
     /* coverity[resource_leak:FALSE]  */
    SHR_FUNC_EXIT;
}

/**
* \brief -  Initialize mib stat
*
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
mib_stat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(mib_stat_fabric_init(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   initialize mib stat fabric
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
mib_stat_fabric_init(
    int unit)
{
    uint32 fmac_idx;
    uint32 nof_instances_fmac = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_fmac = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);

    for (fmac_idx = 0; fmac_idx < nof_instances_fmac; fmac_idx++)
    {
        int enable;
        SHR_IF_ERR_EXIT(soc_dnxc_fabric_fmac_block_valid_get(unit, fmac_idx, &enable));
        if (enable)
        {
            SHR_IF_ERR_EXIT(mib_stat_fabric_fmac_hw_init(unit, fmac_idx));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See h. for reference
 */
shr_error_e
mib_stat_fabric_fsrd_init(
    int unit,
    int fsrd_idx)
{
    uint32 fmac_block;
    uint32 fmac_idx;
    SHR_FUNC_INIT_VARS(unit);
    fmac_block = fsrd_idx * dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit);
    for (fmac_idx = fmac_block; fmac_idx < fmac_block + dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit); fmac_idx++)
    {
        SHR_IF_ERR_EXIT(mib_stat_fabric_fmac_hw_init(unit, fmac_idx));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear the port-based statistics for the indicated device port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_clear(
    int unit,
    bcm_port_t port)
{
    pbmp_t *cmic_bitmap_p;
    int logical_port;
    int chan;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /**
     * Make sure that fabric gport is supported
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &cmic_bitmap_p));
    if (BCM_PBMP_MEMBER(*cmic_bitmap_p, logical_port))
    {
        uint32 *dma_pkt_p;
        SHR_IF_ERR_EXIT(dnx_rx_parser_info.cpu_port_to_dma_channel.get(unit, port, &chan));
#ifdef BCM_ACCESS_SUPPORT
        if (ACCESS_IS_INITIALIZED(unit))
        {
            /** Clear bytes received by CPU port */
            ACCESS_RX_CHAN_CTL(unit, chan).rbyte = 0;
            /** Clear packets received by CPU port */
            ACCESS_RX_CHAN_CTL(unit, chan).rpkt = 0;
        }
        else
#endif
        {
            /** Clear bytes received by CPU port */
            RX_CHAN_CTL(unit, chan).rbyte = 0;
            /** Clear packets received by CPU port */
            RX_CHAN_CTL(unit, chan).rpkt = 0;
        }
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_TBYT, (void **) &dma_pkt_p));
        *dma_pkt_p = 0;
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_TPKT, (void **) &dma_pkt_p));
        *dma_pkt_p = 0;

        SHR_EXIT();
    }

    if (dnx_drv_soc_controlled_counter_use(unit, logical_port))
    {
        SHR_IF_ERR_EXIT(soc_controlled_counter_clear(unit, logical_port));
    }

    if (!dnx_drv_soc_controlled_counter_use(unit, logical_port)
        && dnx_drv_soc_feature(unit, soc_feature_generic_counters))
    {
        bcm_port_t port_base;
        int phy_port;
        pbmp_t pbm;
        int *port_l2p_mapping_p;
        int *port_p2l_mapping_p;

        /*
         * get port base for channelized case
         */
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &port_l2p_mapping_p));
        port_l2p_mapping_p = &(port_l2p_mapping_p[port]);
        phy_port = *port_l2p_mapping_p;
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &port_p2l_mapping_p));
        port_p2l_mapping_p = &(port_p2l_mapping_p[phy_port]);
        port_base = *port_p2l_mapping_p;

        BCM_PBMP_CLEAR(pbm);
        BCM_PBMP_PORT_ADD(pbm, port_base);
        SHR_IF_ERR_EXIT(soc_counter_set32_by_port(unit, pbm, 0));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Synchronize software counters with hardware
 *
 * \param [in] unit - chip unit id.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_sync(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    rc = soc_counter_sync(unit);
    SHR_SET_CURRENT_ERR(rc);
    SHR_EXIT();

exit:
    /** Warmboot skipped, to synce HW value to SW DB.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_get() API
 */
static shr_error_e
dnx_stat_get_verify(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    int set_id, nof_sets;
    const dnx_data_mib_controlled_counters_map_data_t *map_data;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /*
     * Check valid type
     */
    if ((int) type < 0 || type >= snmpValCount)
    {
        return _SHR_E_PARAM;
    }

    /** check type is supported by the port */
    SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
    for (set_id = 0; set_id < nof_sets; set_id++)
    {
        map_data = dnx_data_mib.controlled_counters.map_data_get(unit, type, set_array[set_id]);

        if ((map_data->counters_to_add_array[0] != -1) || (map_data->counters_to_get_cb != NULL))
        {
            break;
        }
    }
    if ((set_id == nof_sets) && (nof_sets > 0))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "stat type %d is not supported for port %d", type, port);
    }
    else if (nof_sets == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port %d is not supported for statistics", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_stat_get_by_counter_idx(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int counter_idx,
    uint64 *value)
{
    int supported = 0, printable;
    int rv;
    soc_controlled_counter_t **controlled_counters_p_p;
    VOL int *counter_interval_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mib_counter_enable_get(unit, port, counter_idx, &supported, &printable));
    if (!supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Statistic type %d (counter index %d) is not supported for port %d\n",
                     type, counter_idx, port);
    }
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, CONTROLLED_COUNTERS, (void **) (&controlled_counters_p_p)));
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));

    if (COUNTER_IS_COLLECTED((*controlled_counters_p_p)[counter_idx]) && *counter_interval_p != 0)
    {
        /*
         * counter is collected by counter thread
         */
        rv = soc_counter_get(unit, port, counter_idx, 0, value);
    }
    else
    {
        /*
         * counter isn't collected by counter thread
         */
        rv = (*controlled_counters_p_p)[counter_idx].controlled_counter_f(unit,
                                                                          (*controlled_counters_p_p)
                                                                          [counter_idx].counter_id, port, value, NULL);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_get() API
 */
static shr_error_e
dnx_stat_rate_get_verify(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *rate)
{
    VOL int *counter_interval_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stat_get_verify(unit, port, type, rate));

    if (!dnx_drv_soc_controlled_counter_use(unit, port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "given port:%d is not using controlled counter", port);
    }

    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));
    if (*counter_interval_p == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "rate API is nut supported when counter thread is disabled\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in]  type  - SNMP statistics type defined in bcm_stat_val_t.
 * \param [out] value - collected 64-bit statistics values.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    int rv = _SHR_E_NONE;
    uint32 add_cnt_type[DNX_MIB_MAX_COUNTER_BUNDLE], sub_cnt_type[DNX_MIB_MAX_COUNTER_BUNDLE];
    int add_num_cntrs, sub_num_cntrs;
    int cntr_index;
    uint64 val;
    pbmp_t *pbmp_p;
    int chan;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Get local port from port.
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_INVOKE_VERIFY_DNXC(dnx_stat_get_verify(unit, logical_port, type, value));

    sal_memset(add_cnt_type, 0xffffffff, sizeof(add_cnt_type));
    sal_memset(sub_cnt_type, 0xffffffff, sizeof(sub_cnt_type));

    add_num_cntrs = sub_num_cntrs = DNX_MIB_MAX_COUNTER_BUNDLE;
    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &pbmp_p));

    if (BCM_PBMP_MEMBER(*pbmp_p, logical_port))
    {
        switch (type)
        {
                uint32 *dma_pkt_p;
            case snmpIfInOctets:
            {
                SHR_IF_ERR_EXIT(dnx_rx_parser_info.cpu_port_to_dma_channel.get(unit, logical_port, &chan));
#ifdef BCM_ACCESS_SUPPORT
                if (ACCESS_IS_INITIALIZED(unit))
                {
                    COMPILER_64_SET(*value, 0, ACCESS_RX_CHAN_CTL(unit, chan).rbyte);
                }
                else
#endif
                {
                    COMPILER_64_SET(*value, 0, RX_CHAN_CTL(unit, chan).rbyte);
                }
                break;
            }
            case snmpIfInUcastPkts:
            {
                SHR_IF_ERR_EXIT(dnx_rx_parser_info.cpu_port_to_dma_channel.get(unit, logical_port, &chan));
#ifdef BCM_ACCESS_SUPPORT
                if (ACCESS_IS_INITIALIZED(unit))
                {
                    COMPILER_64_SET(*value, 0, ACCESS_RX_CHAN_CTL(unit, chan).rpkt);
                }
                else
#endif
                {
                    COMPILER_64_SET(*value, 0, RX_CHAN_CTL(unit, chan).rpkt);
                }
                break;
            }
            case snmpIfOutOctets:
            {
                SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_TBYT, (void **) &dma_pkt_p));
                COMPILER_64_SET(*value, 0, *dma_pkt_p);
                break;
            }
            case snmpIfOutUcastPkts:
            {
                SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_TPKT, (void **) &dma_pkt_p));
                COMPILER_64_SET(*value, 0, *dma_pkt_p);
                break;
            }
            default:
            {
                COMPILER_64_ZERO(*value);
                break;
            }
        }
        SHR_EXIT();
    }

    if (type == snmpBcmSignalQualityLastErrorRate)
    {
        uint32_t error_rate;

        SHR_IF_ERR_EXIT(dnx_port_signal_quality_last_er_error_rate_get(unit, logical_port, &error_rate));

        *value = error_rate;

        SHR_EXIT();
    }

    if ((dnx_drv_soc_controlled_counter_use(unit, logical_port)))
    {
        SHR_IF_ERR_EXIT(dnx_mib_mapping_stat_get(unit, logical_port, type, DNX_MIB_MAX_COUNTER_BUNDLE, add_cnt_type,
                                                 &add_num_cntrs, sub_cnt_type, &sub_num_cntrs));
        COMPILER_64_ZERO(val);

        for (cntr_index = 0; cntr_index < add_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, logical_port, type, add_cnt_type[cntr_index], value));
            COMPILER_64_ADD_64(val, *value);    /* val += (*value) ; */
        }

        for (cntr_index = 0; cntr_index < sub_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, logical_port, type, sub_cnt_type[cntr_index], value));
            COMPILER_64_SUB_64(val, *value);    /* val -= (*value) ; */
        }

        *value = val;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "given port:%d is not using controlled counter", port);
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    /** Warmboot skipped, because of a usage during packet verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_stat_rate_get_by_counter_idx(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int counter_idx,
    uint64 *rate)
{
    int supported = 0, printable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mib_counter_enable_get(unit, port, counter_idx, &supported, &printable));
    if (!supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Statistic type %d (counter index %d) is not supported for port %d\n",
                     type, counter_idx, port);
    }

    /*
     * counter is collected by counter thread
     */
    SHR_IF_ERR_EXIT(soc_counter_get_rate(unit, port, counter_idx, 0, rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the specified statistics rate from the device for
 *        the port. Rate is normalized to a 1 second period.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in]  type  - SNMP statistics type defined in bcm_stat_val_t.
 * \param [out] rate - collected 64-bit statistics rate. Rate is normalized to a 1 second period.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_rate_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *rate)
{
    uint32 add_cnt_type[DNX_MIB_MAX_COUNTER_BUNDLE], sub_cnt_type[DNX_MIB_MAX_COUNTER_BUNDLE];
    int add_num_cntrs, sub_num_cntrs;
    int cntr_index;
    uint64 val;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Get local port from port.
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_INVOKE_VERIFY_DNXC(dnx_stat_rate_get_verify(unit, logical_port, type, rate));

    sal_memset(add_cnt_type, 0xffffffff, sizeof(add_cnt_type));
    sal_memset(sub_cnt_type, 0xffffffff, sizeof(sub_cnt_type));

    add_num_cntrs = sub_num_cntrs = DNX_MIB_MAX_COUNTER_BUNDLE;

    SHR_IF_ERR_EXIT(dnx_mib_mapping_stat_get(unit, logical_port, type, DNX_MIB_MAX_COUNTER_BUNDLE, add_cnt_type,
                                             &add_num_cntrs, sub_cnt_type, &sub_num_cntrs));

    COMPILER_64_ZERO(val);

    for (cntr_index = 0; cntr_index < add_num_cntrs; cntr_index++)
    {
        SHR_IF_ERR_EXIT(dnx_stat_rate_get_by_counter_idx(unit, logical_port, type, add_cnt_type[cntr_index], rate));
        COMPILER_64_ADD_64(val, *rate); /* val += (*rate) ; */
    }

    for (cntr_index = 0; cntr_index < sub_num_cntrs; cntr_index++)
    {
        SHR_IF_ERR_EXIT(dnx_stat_rate_get_by_counter_idx(unit, logical_port, type, sub_cnt_type[cntr_index], rate));
        COMPILER_64_SUB_64(val, *rate); /* val -= (*rate) ; */
    }

    *rate = val;

exit:
    /** Warmboot skipped, because of a usage during packet verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_get() API
 */
static shr_error_e
dnx_stat_clear_single_verify(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type)
{
    int set_id, nof_sets;
    const dnx_data_mib_controlled_counters_map_data_t *map_data;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /*
     * Check valid type
     */
    if ((int) type < 0 || type >= snmpValCount)
    {
        return _SHR_E_PARAM;
    }

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * Exclude ILKN and CPU port
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)) ||
        DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The API is not supported for ILKN/CPU port");
    }

    /** check type is supported by the port */
    SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
    for (set_id = 0; set_id < nof_sets; set_id++)
    {
        map_data = dnx_data_mib.controlled_counters.map_data_get(unit, type, set_array[set_id]);

        if ((map_data->counters_to_add_array[0] != -1) || (map_data->counters_to_get_cb != NULL))
        {
            break;
        }
    }
    if ((set_id == nof_sets) && (nof_sets > 0))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "stat type %d is not supported for port %d", type, port);
    }
    else if (nof_sets == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port %d is not supported for statistics", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear the specified statistics from the device for
 *        the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in]  type  - SNMP statistics type defined in bcm_stat_val_t.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_clear_single(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type)
{
    int rv = _SHR_E_NONE;
    uint32 add_cnt_type[DNX_MIB_MAX_COUNTER_BUNDLE], sub_cnt_type[DNX_MIB_MAX_COUNTER_BUNDLE];
    int add_num_cntrs, sub_num_cntrs;
    int cntr_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_stat_clear_single_verify(unit, port, type));

    if ((dnx_drv_soc_controlled_counter_use(unit, port)))
    {
        SHR_IF_ERR_EXIT(dnx_mib_mapping_stat_get(unit, port, type, DNX_MIB_MAX_COUNTER_BUNDLE, add_cnt_type,
                                                 &add_num_cntrs, sub_cnt_type, &sub_num_cntrs));

        for (cntr_index = 0; cntr_index < add_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(soc_controlled_counter_clear_by_counter_idx(unit, port, add_cnt_type[cntr_index]));
        }

        for (cntr_index = 0; cntr_index < sub_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(soc_controlled_counter_clear_by_counter_idx(unit, port, sub_cnt_type[cntr_index]));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "given port:%d is not using controlled counter", port);
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    /** Warmboot skipped, because of a usage during packet verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_multi_get() API
 */
static shr_error_e
dnx_stat_multi_get_verify(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (nstat <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nstat");
    }
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in]  unit  - chip unit id.
 * \param [in]  port  - logical port.
 * \param [in]  nstat - number of elements in stat array.
 * \param [in]  stat_arr  - array of SNMP statistics types defined in bcm_stat_val_t.
 * \param [out] value_arr - collected 64-bit statistics values.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_multi_get(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *value_arr)
{
    int rc = _SHR_E_NONE;
    int i, logical_port;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /**
     * Make sure that fabric gport is supported
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_INVOKE_VERIFY_DNXC(dnx_stat_multi_get_verify(unit, logical_port, nstat, stat_arr, value_arr));
    for (i = 0; i < nstat; i++)
    {
        rc = bcm_dnx_stat_get(unit, logical_port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Get the specified rate statistics from the device for
 *        the port. Rate is normalized to a 1 second period.
 *
 * \param [in]  unit  - chip unit id.
 * \param [in]  port  - logical port.
 * \param [in]  nstat - number of elements in stat array.
 * \param [in]  stat_arr  - array of SNMP statistics types defined in bcm_stat_val_t.
 * \param [out] rate_arr - collected 64-bit statistics rates. Rate is normalized to a 1 second period.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_rate_multi_get(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *rate_arr)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_stat_multi_get_verify(unit, port, nstat, stat_arr, rate_arr));

    for (i = 0; i < nstat; i++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_stat_rate_get(unit, port, stat_arr[i], &(rate_arr[i])));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_mib_deinit(
    int unit)
{
    soc_controlled_counter_t **controlled_counters_p_p;
    SHR_FUNC_INIT_VARS(unit);

#ifdef INCLUDE_XFLOW_MACSEC
    if (dnx_data_macsec.general.is_macsec_enabled_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_stat_deinit(unit));
    }
#endif
    /*
     * counters detach
     */
    SHR_IF_ERR_EXIT(soc_counter_detach(unit));

    /** free cotrolled counters array */
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, CONTROLLED_COUNTERS, (void **) (&controlled_counters_p_p)));
    SHR_FREE(*controlled_counters_p_p);
    /** free the database for storing counter values per row */
    dnx_mib_counter_values_database_deinit(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for counter_port_add and counter_port_remove
 */
static shr_error_e
dnx_counter_port_er_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *is_add = NULL, *logical_port = NULL;

    SHR_FUNC_INIT_VARS(unit);

    is_add = (int *) metadata;
    logical_port = (int *) payload;

    if (*is_add)
    {
        SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, *logical_port, 0));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, *logical_port, 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for counter_port_remove to update the stats
 */
static shr_error_e
dnx_counter_port_update_er_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *next_master_port = NULL, *logical_port = NULL;

    SHR_FUNC_INIT_VARS(unit);

    next_master_port = (int *) metadata;
    logical_port = (int *) payload;

    SHR_IF_ERR_EXIT(soc_controlled_counter_update_by_port(unit, *logical_port, *next_master_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_counter_port_add(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    int is_master_port;
    int is_init_sequence = 0;
    const dnx_data_mib_general_stat_pbmp_t *stat_pbmp;
    int is_add = 1;

    SHR_FUNC_INIT_VARS(unit);

    is_init_sequence = !(dnxc_init_is_init_done_get(unit));

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, 0, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT, FRAMER_MGMT))
         && is_master_port)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1))
        || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, FALSE)
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE))
    {
        if (is_init_sequence)
        {
            stat_pbmp = dnx_data_mib.general.stat_pbmp_get(unit);
            if (BCM_PBMP_MEMBER(stat_pbmp->pbmp, port))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 1));
                SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                                    (uint8 *) &is_add,
                                                                    sizeof(int),
                                                                    (uint8 *) &port,
                                                                    sizeof(bcm_port_t),
                                                                    &dnx_counter_port_er_rollback, FALSE));
                SHR_IF_ERR_EXIT(dnx_algo_mib_counter_pbmp_port_enable(unit, port, 1));
            }
        }
        else
        {
            /*
             * sbusdma desc alloc
             */
            if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_counters_statdma_support))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_sbusdma_desc_alloc(unit, port));
            }
            SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 1));
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                                (uint8 *) &is_add,
                                                                sizeof(int),
                                                                (uint8 *) &port,
                                                                sizeof(bcm_port_t),
                                                                &dnx_counter_port_er_rollback, FALSE));
            SHR_IF_ERR_EXIT(dnx_algo_mib_counter_pbmp_port_enable(unit, port, 1));

        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_counter_port_remove(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    int is_master_port;
    int nof_channels = 0;
    bcm_port_t next_master_port = DNX_ALGO_PORT_INVALID;
    int counter_interval;
    uint32 counter_flags;
    bcm_pbmp_t counter_pbmp;
    int is_add = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, 0, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT, FRAMER_MGMT))
         && is_master_port)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1))
        || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, FALSE)
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE))
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, MGMT)) &&
            is_master_port && nof_channels > 1)
        {
            /** get next master */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
        }
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

        SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 0));
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                            (uint8 *) &is_add,
                                                            sizeof(int),
                                                            (uint8 *) &port,
                                                            sizeof(bcm_port_t), &dnx_counter_port_er_rollback, FALSE));
        SHR_IF_ERR_EXIT(dnx_algo_mib_counter_pbmp_port_enable(unit, port, 0));
        if (next_master_port != DNX_ALGO_PORT_INVALID)
        {
            if (BCM_PBMP_MEMBER(counter_pbmp, port))
            {
                SHR_IF_ERR_EXIT(soc_controlled_counter_update_by_port(unit, next_master_port, port));
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, next_master_port, 1));
                SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                                    (uint8 *) &next_master_port,
                                                                    sizeof(int),
                                                                    (uint8 *) &port,
                                                                    sizeof(bcm_port_t),
                                                                    &dnx_counter_port_update_er_rollback, FALSE));
                SHR_IF_ERR_EXIT(dnx_algo_mib_counter_pbmp_port_enable(unit, next_master_port, 1));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_controlled_counter_clear(unit, port));
            /*
             * sbusdma desc free
             */
            if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_counters_statdma_support))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_sbusdma_desc_free(unit, port));
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_init_mib_thread_start(
    int unit)
{
    uint32 counter_flags = 0;
    int interval;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * start counter
     */
    interval = dnx_data_mib.general.stat_interval_get(unit);
    if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_counters_statdma_support))
    {
        counter_flags |= SOC_COUNTER_F_DMA;
    }

    if (interval > 0 && interval < ((SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8) && (SAL_BOOT_PLISIM))
    {
        interval = (SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8;
    }
    /*
     * Update interval from or to SW DB
     */
    if (sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_mib_interval_get(unit, &interval));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_mib_interval_set(unit, interval));
    }

    if (sw_state_is_warm_boot(unit))
    {
        /** Allow writing to registers for the counter start time */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_DBAL));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    }
    {
        VOL pbmp_t *counter_pbmp_p;

        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_PBMP, (void **) (&counter_pbmp_p)));
        SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, interval, *counter_pbmp_p));
    }
    if (sw_state_is_warm_boot(unit))
    {
        /** Return to warmboot normal mode */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_DBAL));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_init_mib_thread_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_counter_stop(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_mib_utils_pad_skip_get(
    int unit,
    bcm_port_t port,
    int *skip_pad)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * Eth L1, and BusB MAC clients, supports or not according to
     * the features.
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE))
    {
        *skip_pad = dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported) ? 1 : 0;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, FALSE))
    {
        *skip_pad = dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_tx_pading_supported) ? 0 : 1;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
    {
        *skip_pad = 1;
    }
    else
    {
        *skip_pad = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_counters_map_get_cb(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_nof_counters_to_add,
    int max_nof_counters_subtract,
    int *nof_counters_to_add,
    dnx_mib_counter_t * counters_to_add,
    int *nof_counters_to_subtract,
    dnx_mib_counter_t * counters_to_subtract)
{
    SHR_FUNC_INIT_VARS(unit);

    *nof_counters_to_add = 0;
    *nof_counters_to_subtract = 0;

    switch (type)
    {
        case snmpIfOutErrors:
        {
            int skip_pad = 0;
            int pad_size = 0;
            if (!SAL_BOOT_PLISIM)
            {
                SHR_IF_ERR_EXIT(dnx_mib_utils_pad_skip_get(unit, port, &skip_pad));
                if (!skip_pad)
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, port, bcmPortControlPadToSize, &pad_size));
                }
            }

            if (DNX_MIB_PAD_SIZE_IS_IN_RANGE(pad_size))
            {
                if (max_nof_counters_to_add < 2)
                {
                    SHR_ERR_EXIT(BCM_E_UNAVAIL, "counters_to_add array size (%d) is too small.\n",
                                 max_nof_counters_to_add);
                }

                *nof_counters_to_add = 2;
                counters_to_add[0] = dnx_mib_counter_nif_eth_tfcs;
                counters_to_add[1] = dnx_mib_counter_nif_eth_tjbr;
            }
            else
            {
                if (max_nof_counters_to_add < 3)
                {
                    SHR_ERR_EXIT(BCM_E_UNAVAIL, "counters_to_add array size (%d) is too small.\n",
                                 max_nof_counters_to_add);
                }
                *nof_counters_to_add = 3;
                counters_to_add[0] = dnx_mib_counter_nif_eth_trpkt;
                counters_to_add[1] = dnx_mib_counter_nif_eth_tfcs;
                counters_to_add[2] = dnx_mib_counter_nif_eth_tjbr;
            }
            break;
        }
        case snmpBcmIfOutPreemptErrors:
        {
            int skip_pad = 0;
            int pad_size = 0;
            if (!SAL_BOOT_PLISIM)
            {
                SHR_IF_ERR_EXIT(dnx_mib_utils_pad_skip_get(unit, port, &skip_pad));
                if (!skip_pad)
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, port, bcmPortControlPadToSize, &pad_size));
                }
            }

            if (DNX_MIB_PAD_SIZE_IS_IN_RANGE(pad_size))
            {
                if (max_nof_counters_to_add < 2)
                {
                    SHR_ERR_EXIT(BCM_E_UNAVAIL, "counters_to_add array size (%d) is too small.\n",
                                 max_nof_counters_to_add);
                }

                *nof_counters_to_add = 2;
                counters_to_add[0] = dnx_mib_preempt_counter_nif_eth_tfcs;
                counters_to_add[1] = dnx_mib_preempt_counter_nif_eth_tjbr;
            }
            else
            {
                if (max_nof_counters_to_add < 3)
                {
                    SHR_ERR_EXIT(BCM_E_UNAVAIL, "counters_to_add array size (%d) is too small.\n",
                                 max_nof_counters_to_add);
                }
                *nof_counters_to_add = 3;
                counters_to_add[0] = dnx_mib_preempt_counter_nif_eth_trpkt;
                counters_to_add[1] = dnx_mib_preempt_counter_nif_eth_tfcs;
                counters_to_add[2] = dnx_mib_preempt_counter_nif_eth_tjbr;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "The type %d has no callback to obtain counters.\n", type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_counters_map_get_dummy_cb(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_nof_counters_to_add,
    int max_nof_counters_subtract,
    int *nof_counters_to_add,
    dnx_mib_counter_t * counters_to_add,
    int *nof_counters_to_subtract,
    dnx_mib_counter_t * counters_to_subtract)
{
    SHR_FUNC_INIT_VARS(unit);

    *nof_counters_to_add = 0;
    *nof_counters_to_subtract = 0;

    sal_memset(counters_to_add, 0, max_nof_counters_to_add * sizeof(int));
    sal_memset(counters_to_subtract, 0, max_nof_counters_subtract * sizeof(int));

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_fmac_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    int fmac_idx, lane_idx, link;
    uint64 mask;
    int length;
    int nof_links_in_mac, nof_sets, set_id;
    int hw_counter_id = -1;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];
    const dnx_data_mib_controlled_counters_set_data_t *set_data;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_mac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    if (clear_on_read)
    {
        *clear_on_read = TRUE;
    }
    /** get internal hw counter id */
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
        for (set_id = 0; set_id < nof_sets; set_id++)
        {
            /** get set_data */
            set_data = dnx_data_mib.controlled_counters.set_data_get(unit, counter_id, set_array[set_id]);
            if (set_data->get_cb != NULL)
            {
                hw_counter_id = set_data->hw_counter_id;
                break;
            }
        }
    }

    /** get port type - can be either fabric or ilkn over fabric */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK)))
    {
        int phy_port;

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &phy_port));
        link = phy_port;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Function not supported for port %d", port);
    }

    /** hw access - get counter value */
    fmac_idx = link / nof_links_in_mac;
    lane_idx = link % nof_links_in_mac;
    SHR_IF_ERR_EXIT(dnx_mib_fmac_stat_dbal_get(unit, fmac_idx, lane_idx, hw_counter_id, val));

    SHR_IF_ERR_EXIT(dnx_mib_counter_length_get(unit, counter_id, port, &length));
    if (length == 0)
    {
        COMPILER_64_ZERO(*val);
    }
    else if (length < 64)
    {
        COMPILER_64_MASK_CREATE(mask, length, 0);
        COMPILER_64_AND((*val), mask);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_fmac_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 data;
    SHR_FUNC_INIT_VARS(unit);

    /** FMAC counter is clear on read */
    SHR_IF_ERR_EXIT(dnx_mib_fmac_counter_get(unit, counter_id, port, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_nif_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    dnx_algo_port_info_s port_info;
    bcm_port_prio_config_t priority_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy;
    phymod_phy_stat_t stat;
    int phys_returned;
    uint32 val32 = 0;
    static uint64 ber_cnt = 0, bip_error_cnt = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    sal_memset(&priority_config, 0, sizeof(priority_config));

    if (clear_on_read)
    {
        *clear_on_read = TRUE;
    }

    COMPILER_64_ZERO(*val);

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    switch (counter_id)
    {
        case dnx_mib_counter_nif_rx_stats_drop_events_sch_low:

            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
            {
                /** no PRD for MGMT port */
                break;
            }

            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerLow, val));
            }
            break;
        case dnx_mib_counter_nif_rx_stats_drop_events_sch_high:

            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
            {
                /** no PRD for MGMT port */
                break;
            }

            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerHigh, val));
            }
            break;
        case dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm:

            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
            {
                /** no PRD for MGMT port */
                break;
            }

            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
                {
                    SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerTDM, val));
                }
            }
            break;
        case dnx_mib_counter_nif_rx_stats_drop_events_sch_data:
            /*
             * This counter will only be used for ILKN to read the data HRF.
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
            {
                /** no PRD for MGMT port */
                break;
            }

            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerLow, val));
            }
            break;
        case dnx_mib_counter_nif_rx_fec_correctable_errors:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy, &phys_returned, NULL));
                SHR_IF_ERR_EXIT(phymod_phy_fec_cl91_correctable_counter_get(&phy, &val32));
                COMPILER_64_SET(*val, 0, val32);
            }
            else
            {
                SHR_IF_ERR_EXIT(imb_port_mib_counter_get
                                (unit, port, dnx_mib_counter_nif_rx_fec_correctable_errors, val));
            }
            break;
        case dnx_mib_counter_nif_rx_fec_uncorrectable_errors:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy, &phys_returned, NULL));
                SHR_IF_ERR_EXIT(phymod_phy_fec_cl91_uncorrectable_counter_get(&phy, &val32));
                COMPILER_64_SET(*val, 0, val32);
            }
            else
            {
                SHR_IF_ERR_EXIT(imb_port_mib_counter_get
                                (unit, port, dnx_mib_counter_nif_rx_fec_uncorrectable_errors, val));
            }
            break;
        case dnx_mib_counter_nif_rx_bip_errors:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                uint32 i, nof_lane_in_pm = 0;
                dnx_algo_port_ethu_access_info_t ethu_info;
                SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
                nof_lane_in_pm = dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->nof_lanes_in_pm;

                SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy, &phys_returned, NULL));
                SHR_IF_ERR_EXIT(phymod_phy_stat_get(&phy, &stat));

                COMPILER_64_ZERO(ber_cnt);
                COMPILER_64_ZERO(bip_error_cnt);
                for (i = 0; i < nof_lane_in_pm; i++)
                {
                    if (phy.access.lane_mask & (1U << i))
                    {
                        COMPILER_64_ADD_32(ber_cnt, stat.pcs_ber_count[i]);
                        COMPILER_64_ADD_32(bip_error_cnt, stat.pcs_bip_err_count[i]);
                    }
                }
                COMPILER_64_ADD_64(*val, ber_cnt);
                COMPILER_64_ADD_64(*val, bip_error_cnt);
            }
            break;
        case dnx_mib_counter_nif_rx_ber_only:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                COMPILER_64_ADD_64(*val, ber_cnt);
            }
            break;
        case dnx_mib_counter_nif_rx_bip_errors_only:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                COMPILER_64_ADD_64(*val, bip_error_cnt);
            }
            break;
        case dnx_mib_counter_rx_rs_fec_symbol_error_rate:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                portmod_speed_config_t port_speed_config;
                uint32 i, error_count[PM_MAX_LANES];
                soc_port_phy_rsfec_symb_errcnt_t symbol_error_count;
                int has_tx_speed;

                SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
                SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
                if (has_tx_speed)
                {
                    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));
                }
                if (port_speed_config.fec > PORTMOD_PORT_PHY_FEC_NONE)
                {
                    symbol_error_count.max_count = PM_MAX_LANES;
                    symbol_error_count.symbol_errcnt = error_count;
                    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                                    (unit, port, &params, 1, &phy, &phys_returned, NULL));
                    SHR_IF_ERR_EXIT(phymod_phy_rsfec_symbol_error_counter_get
                                    (&phy, symbol_error_count.max_count, &symbol_error_count.actual_count,
                                     symbol_error_count.symbol_errcnt));
                    for (i = 0; i < symbol_error_count.actual_count; i++)
                    {
                        COMPILER_64_ADD_32(*val, error_count[i]);
                    }
                }
            }
            break;
        case dnx_mib_counter_rx_rs_fec_bit_error:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                portmod_speed_config_t port_speed_config;
                int has_tx_speed;

                SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
                SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
                if (has_tx_speed)
                {
                    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));
                }
                if (port_speed_config.fec > PORTMOD_PORT_PHY_FEC_NONE)
                {
                    uint32 value = 0;

                    {
                        dnx_algo_port_ethu_access_info_t ethu_info;

                        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

                        SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                                        (unit, port, &params, 1, &phy, &phys_returned, NULL));
                        if (ethu_info.imb_type != imbDispatchTypeImb_clu)
                        {
                            SHR_IF_ERR_EXIT(phymod_phy_fec_error_bits_counter_get(&phy, &value));
                        }
                    }

                    *val = value;
                }
            }
            break;
        case dnx_mib_counter_nif_rx_stats_discards:
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE_L1) ||
                DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0))
            {
                SHR_IF_ERR_EXIT(imb_port_mib_counter_get(unit, port, dnx_mib_counter_nif_rx_stats_discards, val));
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d", counter_id, port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_nif_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 data;
    SHR_FUNC_INIT_VARS(unit);

    /** assume clear on read */
    SHR_IF_ERR_EXIT(dnx_mib_nif_counter_get(unit, counter_id, port, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_ilkn_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }
    COMPILER_64_ZERO(*val);
    SHR_IF_ERR_EXIT(imb_port_mib_counter_get(unit, port, counter_id, val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear ilkn port counters
 */
static int
dnx_mib_ilkn_counter_clear_by_port(
    int unit,
    int port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_port_mib_counter_clear(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_ilkn_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** ignore counter_id */
    SHR_IF_ERR_EXIT(dnx_mib_ilkn_counter_clear_by_port(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_nif_eth_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    dbal_fields_e field_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    VOL int *counter_interval_p;
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }

    COMPILER_64_ZERO(*val);

    /** check for PM type*/
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));

    if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_counters_statdma_support)
        && *counter_interval_p != 0)
    {
        SHR_IF_ERR_EXIT(dnx_mib_nif_eth_statdma_counter_get(unit, port, counter_id, val));
    }
    /*
     * counter thread is on, we should requested counter per row 
     * that means reading 8 counter per one HW access.
     */
    else if (dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->mib_type
             == DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CDMAC && *counter_interval_p != 0)
    {
        SHR_IF_ERR_EXIT(dnx_mib_nif_cdmac_optimized_stat_dbal_get(unit, port, counter_id, val));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, clear_on_read));
        SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_get(unit, port, field_id, val));
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
int
dnx_mib_nif_eth_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 val;
    dbal_fields_e field_id;
    uint32 clear_on_read;
    dnx_algo_port_ethu_access_info_t ethu_info;
    VOL int *counter_interval_p;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(val);
    /** map counter id to dbal field */
    SHR_IF_ERR_EXIT(dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, &clear_on_read));

    if (clear_on_read)
    {
        SHR_IF_ERR_EXIT(dnx_mib_nif_eth_counter_get(unit, counter_id, port, &val, NULL));
    }
    else
    {
        /** check for PM type*/
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));
        /*
         * counter thread is on, we should set counter per row
         * if support stat dma,clear counter one by one
         */
        if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_counters_statdma_support)
            && *counter_interval_p != 0)
        {
            /*
             * Clear DMA buff
             */
            SHR_IF_ERR_EXIT(dnx_mib_nif_eth_statdma_counter_clear(unit, port, counter_id));
            /*
             * Clear HW
             */
            SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_set(unit, port, field_id, val));
        }
        else if (dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->mib_type ==
                 DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CDMAC && *counter_interval_p != 0)
        {
            SHR_IF_ERR_EXIT(dnx_mib_nif_cdmac_optimized_stat_dbal_clear(unit, port, counter_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_set(unit, port, field_id, val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_nif_eth_preempt_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    dbal_fields_e field_id;
    VOL int *counter_interval_p;
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }

    COMPILER_64_ZERO(*val);

    if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_preempt_counters_support))
    {
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));
        if (*counter_interval_p != 0)
        {
            SHR_IF_ERR_EXIT(dnx_mib_nif_cdmac_optimized_stat_dbal_get(unit, port, counter_id, val));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, clear_on_read));
            SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_get(unit, port, field_id, val));
        }

    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
int
dnx_mib_nif_eth_preempt_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 val;
    dbal_fields_e field_id;
    uint32 clear_on_read;
    VOL int *counter_interval_p;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(val);

    if (dnx_data_mib.general.feature_get(unit, dnx_data_mib_general_mib_preempt_counters_support))
    {
        /** map counter id to dbal field */
        SHR_IF_ERR_EXIT(dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, &clear_on_read));
        if (clear_on_read)
        {
            SHR_IF_ERR_EXIT(dnx_mib_nif_eth_preempt_counter_get(unit, counter_id, port, &val, NULL));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));
            if (*counter_interval_p != 0)
            {
                SHR_IF_ERR_EXIT(dnx_mib_nif_cdmac_optimized_stat_dbal_clear(unit, port, counter_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_set(unit, port, field_id, val));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_flexe_mac_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }

    COMPILER_64_ZERO(*val);

    if (!dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_pfc_pause_supported) &&
        ((counter_id >= dnx_mib_counter_nif_eth_rpfc0 && counter_id <= dnx_mib_counter_nif_eth_rpfcoff7) ||
         (counter_id >= dnx_mib_counter_nif_eth_tpfc0 && counter_id <= dnx_mib_counter_nif_eth_tpfcoff7)))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(imb_port_mib_counter_get(unit, port, counter_id, val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_flexe_mac_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 val;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(val);

    if (!dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_pfc_pause_supported) &&
        ((counter_id >= dnx_mib_counter_nif_eth_rpfc0 && counter_id <= dnx_mib_counter_nif_eth_rpfcoff7) ||
         (counter_id >= dnx_mib_counter_nif_eth_tpfc0 && counter_id <= dnx_mib_counter_nif_eth_tpfcoff7)))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(imb_port_mib_counter_set(unit, port, counter_id, &val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    int nof_sets, set_id;
    const dnx_data_mib_controlled_counters_set_data_t *set_data;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];

    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = TRUE;
    }
    COMPILER_64_ZERO(*val);

    /** try to find a valid match for the port set types and the counter id */
    SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
    for (set_id = 0; set_id < nof_sets; set_id++)
    {
        set_data = dnx_data_mib.controlled_counters.set_data_get(unit, counter_id, set_array[set_id]);
        if (set_data->get_cb != NULL)
        {
            /** invoke counter get function for the specific counter set */
            SHR_IF_ERR_EXIT(set_data->get_cb(unit, counter_id, port, val, clear_on_read));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    int nof_sets, set_id;
    const dnx_data_mib_controlled_counters_set_data_t *set_data;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];

    SHR_FUNC_INIT_VARS(unit);

    /** try to find a valid match for the port set types and the counter id */
    SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
    for (set_id = 0; set_id < nof_sets; set_id++)
    {
        set_data = dnx_data_mib.controlled_counters.set_data_get(unit, counter_id, set_array[set_id]);
        if (set_data->clear_cb != NULL)
        {
            /** invoke counter clear function for the specific counter set */
            SHR_IF_ERR_EXIT(set_data->clear_cb(unit, counter_id, port));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_mib_counter_length_get(
    int unit,
    int counter_id,
    int port,
    int *length)
{
    dnx_algo_port_info_s port_info;
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;
    const dnx_data_mib_controlled_counters_flexmac_counter_data_t *flexmac_counter_data;

    SHR_FUNC_INIT_VARS(unit);

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, FALSE) &&
        counter_id >= dnx_mib_counter_nif_eth_r64 && counter_id <= dnx_mib_counter_nif_eth_tbyt)
    {
        /** get counter info */
        flexmac_counter_data = dnx_data_mib.controlled_counters.flexmac_counter_data_get(unit, counter_id);
        if (flexmac_counter_data->length < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type for flexmac %d has invalid length", counter_id);
        }
        *length = flexmac_counter_data->length;
    }
    else
    {
        /** get counter info */
        counter_data = dnx_data_mib.controlled_counters.counter_data_get(unit, counter_id);
        if (counter_data->length < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d has invalid length", counter_id);
        }
        *length = counter_data->length;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_counter_enable_get(
    int unit,
    bcm_port_t port,
    int index,
    int *enable,
    int *printable)
{

    int nof_sets, set_id, counter_id;
    const dnx_data_mib_controlled_counters_set_data_t *set_data;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];
    soc_controlled_counter_t **controlled_counters_p_p;

    SHR_FUNC_INIT_VARS(unit);

    /** extract controlled counter_id from controlled counters array according to provided array index */
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, CONTROLLED_COUNTERS, (void **) (&controlled_counters_p_p)));
    counter_id = (*controlled_counters_p_p)[index].counter_id;

    *enable = FALSE;
    /** try to find a valid match for the port set types and the counter id */
    SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
    for (set_id = 0; set_id < nof_sets; set_id++)
    {
        set_data = dnx_data_mib.controlled_counters.set_data_get(unit, counter_id, set_array[set_id]);
        if (set_data->get_cb != NULL)
        {
            *enable = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_mapping_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_array_size,
    dnx_mib_counter_t * add_counters,
    int *add_array_size,
    dnx_mib_counter_t * sub_counters,
    int *sub_array_size)
{
    int nof_sets, set_id;
    const dnx_data_mib_controlled_counters_map_data_t *map_data;
    dnx_algo_port_mib_counter_set_type_e set_array[DNX_ALGO_PORT_COUNTER_SETS_MAX];

    SHR_FUNC_INIT_VARS(unit);

    *sub_array_size = 0;
    *add_array_size = 0;

    /**
     * try to find a valid match for the port set types and the bcm stat type.
     * Once match is found - extract list of counters and stop.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_counter_sets_get(unit, port, &nof_sets, set_array));
    for (set_id = 0; set_id < nof_sets; set_id++)
    {
        map_data = dnx_data_mib.controlled_counters.map_data_get(unit, type, set_array[set_id]);

        /** extract counters bundle from provided callback */
        if (map_data->counters_to_get_cb != NULL)
        {
            SHR_IF_ERR_EXIT(map_data->counters_to_get_cb(unit, port, type, max_array_size, max_array_size,
                                                         add_array_size, add_counters, sub_array_size, sub_counters));
            break;

        }
        else if (map_data->counters_to_add_array[0] != -1)
        {
            /** extract counters bundle from dnx data */
            int ii;

            /** construct add list */
            ii = 0;
            while (ii < DNX_MIB_MAX_COUNTER_BUNDLE && map_data->counters_to_add_array[ii] != -1)
            {
                add_counters[ii] = map_data->counters_to_add_array[ii];
                ii++;

                if (ii > max_array_size)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "add counter list is bigger than maximal allowed=%d", max_array_size);
                }
            }
            *add_array_size = ii;

            /** construct sub list */
            ii = 0;
            while (ii < DNX_MIB_MAX_COUNTER_BUNDLE && map_data->counters_to_subtract_array[ii] != -1)
            {
                sub_counters[ii] = map_data->counters_to_subtract_array[ii];
                ii++;

                if (ii > max_array_size)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "add counter list is bigger than maximal allowed=%d", max_array_size);
                }
            }
            *sub_array_size = ii;

            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_mib_nif_eth_epfc_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }

    COMPILER_64_ZERO(*val);

    switch (counter_id)
    {
        case dnx_mib_counter_nif_eth_epfc_tpfc0:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority0, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc1:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority1, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc2:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority2, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc3:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority3, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc4:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority4, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc5:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority5, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc6:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority6, val);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc7:
            dnx_port_encrypted_pfc_tx_stat_get(unit, port, snmpBcmTxEncryptedPFCFramePriority7, val);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d", counter_id, port);
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

int
dnx_mib_nif_eth_epfc_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    switch (counter_id)
    {
        case dnx_mib_counter_nif_eth_epfc_tpfc0:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority0);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc1:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority1);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc2:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority2);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc3:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority3);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc4:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority4);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc5:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority5);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc6:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority6);
            break;
        case dnx_mib_counter_nif_eth_epfc_tpfc7:
            dnx_port_encrypted_pfc_tx_stat_clear(unit, port, snmpBcmTxEncryptedPFCFramePriority7);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d", counter_id, port);
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
