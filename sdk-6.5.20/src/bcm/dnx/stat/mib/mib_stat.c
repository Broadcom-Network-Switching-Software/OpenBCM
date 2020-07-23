/** \file mib_stat.c
 * 
 *
 * MIB stat procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <sal/core/boot.h>

#include <shared/shrextend/shrextend_error.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/common/rx.h>
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

/**
 * \brief - padding size definition for snmpIfInOctets feature.
 */
#define DNX_MIB_PAD_SIZE_MIN    (64)
#define DNX_MIB_PAD_SIZE_MAX    (96)
#define DNX_MIB_PAD_SIZE_IS_IN_RANGE(pad_size) \
    (pad_size >= DNX_MIB_PAD_SIZE_MIN && pad_size <= DNX_MIB_PAD_SIZE_MAX)

/** see .h file */
shr_error_e
dnx_mib_init(
    int unit)
{
    const dnx_data_mib_general_stat_pbmp_t *stat_pbmp;
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;
    bcm_pbmp_t fabric_ports_bitmap;
    bcm_pbmp_t nif_eth_ports_bitmap;
    bcm_pbmp_t nif_ilkn_ports_bitmap;
    soc_controlled_counter_t **controlled_counters_p_p;
    soc_controlled_counter_t *dnx_controlled_counters_array = NULL;
    int counter_id;
    int allocated_array_need_free = FALSE;
    int allocated_db_need_free = FALSE;

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
     * During Warm boot, add nif ports to counter bitmap here.
     * Because dnx_counter_port_add() was not called in adding ports stage
     */
    if (sw_state_is_warm_boot(unit))
    {
        VOL pbmp_t *counter_pbmp_p;
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                         &nif_eth_ports_bitmap));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &nif_ilkn_ports_bitmap));

        BCM_PBMP_AND(nif_eth_ports_bitmap, stat_pbmp->pbmp);
        BCM_PBMP_AND(nif_ilkn_ports_bitmap, stat_pbmp->pbmp);
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_PBMP, (void **) (&counter_pbmp_p)));
        BCM_PBMP_OR(*counter_pbmp_p, nif_eth_ports_bitmap);
        BCM_PBMP_OR(*counter_pbmp_p, nif_ilkn_ports_bitmap);
    }
    {
        /*
         * update counters bitmap in case counter thread is not start
         */
        VOL pbmp_t *counter_pbmp_p;

        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_PBMP, (void **) (&counter_pbmp_p)));
        BCM_PBMP_OR(*counter_pbmp_p, fabric_ports_bitmap);
    }

#ifdef INCLUDE_XFLOW_MACSEC
    if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_macsec_supported))
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
        SHR_IF_ERR_EXIT(dnx_fabric_if_fmac_block_enable_get(unit, fmac_idx, &enable));
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
 * \brief - Deinit the BCM statistics module
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
dnx_stat_stop(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    rc = soc_counter_stop(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_clear() API
 */
static shr_error_e
dnx_stat_clear_verify(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_clear_verify(unit, port));
    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &cmic_bitmap_p));
    if (BCM_PBMP_MEMBER(*cmic_bitmap_p, port))
    {
        /*
         * Rudimentary CPU statistics -- needs soc_reg_twork
         */
        uint32 *dma_pkt_p;

        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_RBYT, (void **) &dma_pkt_p));
        *dma_pkt_p = 0;
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_RPKT, (void **) &dma_pkt_p));
        *dma_pkt_p = 0;
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_TBYT, (void **) &dma_pkt_p));
        *dma_pkt_p = 0;
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, DMA_TPKT, (void **) &dma_pkt_p));
        *dma_pkt_p = 0;

        SHR_EXIT();
    }

    if (dnx_drv_soc_controlled_counter_use(unit, port))
    {
        SHR_IF_ERR_EXIT(soc_controlled_counter_clear(unit, port));
    }

    if (!dnx_drv_soc_controlled_counter_use(unit, port) && dnx_drv_soc_feature(unit, soc_feature_generic_counters))
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
    SHR_IF_ERR_EXIT(rc);

exit:
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

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /*
     * Check valid type
     */
    if (type < 0 || type >= snmpValCount)
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

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_get_verify(unit, port, type, value));

    sal_memset(add_cnt_type, 0xffffffff, sizeof(add_cnt_type));
    sal_memset(sub_cnt_type, 0xffffffff, sizeof(sub_cnt_type));

    add_num_cntrs = sub_num_cntrs = DNX_MIB_MAX_COUNTER_BUNDLE;
    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &pbmp_p));

    if (BCM_PBMP_MEMBER(*pbmp_p, port))
    {
        /*
         * Rudimentary CPU statistics -- needs work
         */
        switch (type)
        {
                uint32 *dma_pkt_p;
            case snmpIfInOctets:
            {
                SHR_IF_ERR_EXIT(dnx_rx_parser_info.cpu_port_to_dma_channel.get(unit, port, &chan));
                COMPILER_64_SET(*value, 0, RX_CHAN_CTL(unit, chan).rbyte);
                break;
            }
            case snmpIfInUcastPkts:
            {
                SHR_IF_ERR_EXIT(dnx_rx_parser_info.cpu_port_to_dma_channel.get(unit, port, &chan));
                COMPILER_64_SET(*value, 0, RX_CHAN_CTL(unit, chan).rpkt);
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

    if ((dnx_drv_soc_controlled_counter_use(unit, port)))
    {
        SHR_IF_ERR_EXIT(dnx_mib_mapping_stat_get(unit, port, type, DNX_MIB_MAX_COUNTER_BUNDLE, add_cnt_type,
                                                 &add_num_cntrs, sub_cnt_type, &sub_num_cntrs));
        COMPILER_64_ZERO(val);

        for (cntr_index = 0; cntr_index < add_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, type, add_cnt_type[cntr_index], value));
            COMPILER_64_ADD_64(val, *value);    /* val += (*value) ; */
        }

        for (cntr_index = 0; cntr_index < sub_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, type, sub_cnt_type[cntr_index], value));
            COMPILER_64_SUB_64(val, *value);    /* val -= (*value) ; */
        }

        *value = val;
    }
    else
    {
        return _SHR_E_UNAVAIL;
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

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

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
    int i;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_multi_get_verify(unit, port, nstat, stat_arr, value_arr));
    for (i = 0; i < nstat; i++)
    {
        rc = bcm_dnx_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rc);
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
    if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_macsec_supported))
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

    SHR_FUNC_INIT_VARS(unit);

    is_init_sequence = !(dnx_init_is_init_done_get(unit));

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE) && is_master_port)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE, TRUE)
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1))
    {
        if (is_init_sequence)
        {
            stat_pbmp = dnx_data_mib.general.stat_pbmp_get(unit);
            if (BCM_PBMP_MEMBER(stat_pbmp->pbmp, port))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 1));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 1));
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

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE) && is_master_port)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE, TRUE)
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1))
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE) && is_master_port && nof_channels > 1)
        {
            /** get next master */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
        }
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

        SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 0));
        if (next_master_port != DNX_ALGO_PORT_INVALID)
        {
            if (BCM_PBMP_MEMBER(counter_pbmp, port))
            {
                SHR_IF_ERR_EXIT(soc_controlled_counter_update_by_port(unit, next_master_port, port));
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, next_master_port, 1));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_controlled_counter_clear(unit, port));
        }
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_init_mib_thread_start(
    int unit)
{
    uint32 interval;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * start counter
     */
    interval = dnx_data_mib.general.stat_interval_get(unit);

    if (interval > 0 && interval < ((SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8) && (SAL_BOOT_PLISIM))
    {
        interval = (SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8;
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
        SHR_IF_ERR_EXIT(soc_counter_start(unit, 0, interval, *counter_pbmp_p));
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
            int pad_size = 0;
            if (!SAL_BOOT_PLISIM)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, port, bcmPortControlPadToSize, &pad_size));
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
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE /** ELK*/ , FALSE /** L1 */ ))
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

    SHR_IF_ERR_EXIT(dnx_mib_counter_length_get(unit, counter_id, &length));
    COMPILER_64_MASK_CREATE(mask, length, 0);
    COMPILER_64_AND((*val), mask);

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
    int phys_returned;
    uint32 val32 = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

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
            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerLow, val));
            }
            break;
        case dnx_mib_counter_nif_rx_stats_drop_events_sch_high:
            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerHigh, val));
            }
            break;
        case dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm:
            if (((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info) == FALSE) &&
                 (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info) == FALSE)) ||
                (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
            {
                if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
                {
                    SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerTDM, val));
                }
            }
            break;
        case dnx_mib_counter_nif_rx_fec_correctable_errors:
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE /** ELK*/ , TRUE /** L1 */ ))
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
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE /** ELK*/ , TRUE /** L1 */ ))
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
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE /** ELK*/ , FALSE /** L1 */ ))
            {
                COMPILER_64_SET(*val, 0, 0);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d", counter_id, port);
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
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
    /*
     * counter thread is on, we should requested counter per row 
     * that means reading 8 counter per one HW access.
     */
    if (ethu_info.imb_type == imbDispatchTypeImb_cdu && *counter_interval_p != 0)
    {
        SHR_IF_ERR_EXIT(dnx_mib_nif_cdu_optimized_stat_dbal_get(unit, port, counter_id, val));
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
         */
        if (ethu_info.imb_type == imbDispatchTypeImb_cdu && *counter_interval_p != 0)
        {
            SHR_IF_ERR_EXIT(dnx_mib_nif_cdu_optimized_stat_dbal_clear(unit, port, counter_id));
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
    int *length)
{
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;

    SHR_FUNC_INIT_VARS(unit);

    /** get counter info */
    counter_data = dnx_data_mib.controlled_counters.counter_data_get(unit, counter_id);
    if (counter_data->length < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d has invalid length", counter_id);
    }

    *length = counter_data->length;

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
            while (map_data->counters_to_add_array[ii] != -1)
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
            while (map_data->counters_to_subtract_array[ii] != -1)
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
