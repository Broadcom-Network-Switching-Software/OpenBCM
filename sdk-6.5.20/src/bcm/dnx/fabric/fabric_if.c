/**
 * \file fabric_if.c $Id$ Fabric Ports Interfaces procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/common/link.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/counter.h>
#include <shared/pbmp.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <src/bcm/dnx/port/imb/imb_utils.h>
#include <soc/portmod/portmod.h>
#include <phymod/phymod.h>
/* } */

/*
 * Leaky Bucket configurations
 */
#define DNX_FABRIC_IF_LINK_FAP_BKT_FILL_RATE  (6)
#define DNX_FABRIC_IF_LINK_FAP_BKT_UP_LINK_TH (32)
#define DNX_FABRIC_IF_LINK_FAP_BKT_DN_LINK_TH (16)

/*
 * Comma Burst configurations
 */
#define DNX_FABRIC_IF_COMMA_BURST_SIZE         (1)
#define DNX_FABRIC_IF_COMMA_BURST_PERIOD       (11)
#define DNX_FABRIC_IF_COMMA_BURST_LLFC_ENABLER (0x7)
#define DNX_FABRIC_IF_COMMA_BURST_BYTE_MODE    (0x1)

/*
 * Journal entry for counter thread
 */
typedef struct dnx_fabric_port_er_counter_journal_s
{
    /*
     * Indicate if it is for probe API
     */
    int is_add;
    /*
     * If the action is start the thread
     */
    int is_start_thread;
} dnx_fabric_port_er_counter_journal_t;

/*
 * See .h file.
 */
shr_error_e
dnx_fabric_if_link_force_signal_set(
    int unit,
    int link,
    int force_signal)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_FORCE_SIGNAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, force_signal);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize FMAC configurations for a fabric port
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *   Logical port of the fabric link.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_if_port_mac_init(
    int unit,
    bcm_port_t logical_port)
{
    uint32 entry_handle_id;
    int control_cells_enable = 0;
    uint32 nof_pipes = 0;
    int link = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * FMAC Does not work in emulation
     */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link));

        /*
         * Configure nof pipes
         */
        nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_PIPES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_PIPE_ID, INST_SINGLE, nof_pipes - 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Enable PCP
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_IF_PCP_ENABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Configure Leaky Bucket
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_IF_LEAKY_BUCKET, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_FILL_RATE, INST_SINGLE,
                                     DNX_FABRIC_IF_LINK_FAP_BKT_FILL_RATE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_LINK_UP_TH, INST_SINGLE,
                                     DNX_FABRIC_IF_LINK_FAP_BKT_UP_LINK_TH);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_LINK_DOWN_TH, INST_SINGLE,
                                     DNX_FABRIC_IF_LINK_FAP_BKT_DN_LINK_TH);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_PHY_RESET_ENABLE, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_MAC_RESET_ENABLE, INST_SINGLE, 1);
        /** Turn on link pump when leaky bucket is down for better link utilization */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LINK_PUMP, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Configure Comma Burst
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_IF_COMMA_BURST, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMA_BURST_SIZE, INST_SINGLE,
                                     DNX_FABRIC_IF_COMMA_BURST_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMA_PERIOD, INST_SINGLE,
                                     DNX_FABRIC_IF_COMMA_BURST_PERIOD);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMA_BURST_SIZE_LLFC, INST_SINGLE,
                                     DNX_FABRIC_IF_COMMA_BURST_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMA_PERIOD_LLFC, INST_SINGLE,
                                     DNX_FABRIC_IF_COMMA_BURST_PERIOD);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMA_BURST_LLFC_ENABLE, INST_SINGLE,
                                     DNX_FABRIC_IF_COMMA_BURST_LLFC_ENABLER);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_COMMA_PERIOD_BYTES, INST_SINGLE,
                                     DNX_FABRIC_IF_COMMA_BURST_BYTE_MODE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Set link topology mode
         */
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &control_cells_enable));

        if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_0, INST_SINGLE, control_cells_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_1, INST_SINGLE,
                                         (control_cells_enable == FALSE));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_2, INST_SINGLE, control_cells_enable);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Enable RX_LOS_SYNC interrupt
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_IF_RX_SYNC_CONFIG, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FORCE_SYNC, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_if_mac_init(
    int unit,
    int fmac_index)
{
    uint32 entry_handle_id;
    int control_cells_enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * FMAC Does not work in emulation
     */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &control_cells_enable));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_MAC_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmac_index);

        /*
         * Set FMAC to work on FAP mode
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FAP, INST_SINGLE, 1);

        /*
         * Set LLFC control cells
         */

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_LLFC_DISABLE, INST_SINGLE,
                                     (control_cells_enable == TRUE) ? 0 : 1);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_fabric_if_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_if_port_enable_set_phase1(unit, logical_port, enable));
    sal_usleep(1000);
    SHR_IF_ERR_EXIT(dnx_fabric_if_port_enable_set_phase2(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_fabric_if_port_enable_set_phase1(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    uint32 entry_handle_id;
    int link_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Convert port to fabric link
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));

    /*
     * Set link force signal
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_link_force_signal_set(unit, link_id, (enable) ? 0 : 1));

    /*
     * Note that the order of the steps for disable is reversed to the
     * order of the steps for enable.
     */
    if (enable)
    {
        /*
         * Enable link in FDTL
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_ENABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRE_ENABLE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Enable port
         */
        SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, logical_port, 0, enable));
    }
    else
    {
        /*
         * Disable link in FDT
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_ENABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_ENABLE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Disable link in RTP regardless of sw_state
         */
        SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_id, 1));

        /*
         * Stop transmitting reachability cells on the link regardless of sw_state
         */
        SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_set(unit, link_id, 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_fabric_if_port_enable_set_phase2(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    uint32 entry_handle_id;
    int link_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Convert port to fabric link
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));

    /*
     * Note that the order of the steps for disable is reversed to the
     * order of the steps for enable.
     */
    if (enable)
    {
        int is_link_allowed = 0;
        int is_link_isolated = 0;

        /*
         * Enable transmitting reachability cells on the link
         * according to sw_state
         */
        SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_isolated.get(unit, link_id, &is_link_isolated));
        if (is_link_isolated == 0)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_set(unit, link_id, 0));
        }
        /*
         * Enable link in RTP according to sw_state
         */
        SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.get(unit, link_id, &is_link_allowed));
        if (is_link_allowed)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_id, 0));
        }

        /*
         * Enable link in FDT
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_ENABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_ENABLE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Disable port
         */
        SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, logical_port, 0, enable));

        /*
         * Disable link in FDTL
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_ENABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRE_ENABLE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for imb_port_add and imb_port_remove
 */
static shr_error_e
dnx_fabric_port_error_recovery_probe_detach_rollback(
    int unit,
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
        SHR_IF_ERR_EXIT(imb_port_remove(unit, *logical_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_port_add(unit, *logical_port, 0));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for counter thread
 */
static shr_error_e
dnx_fabric_port_er_counter_rollback(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    uint32 counter_flags;
    int counter_interval;
    bcm_pbmp_t counter_pbmp, *pbmp = NULL;
    dnx_fabric_port_er_counter_journal_t *journal_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    pbmp = (bcm_pbmp_t *) payload;
    journal_entry = (dnx_fabric_port_er_counter_journal_t *) metadata;

    if (journal_entry->is_start_thread)
    {
        /*
         * Pause counter thread
         */
        SHR_IF_ERR_EXIT(soc_counter_stop(unit));
    }
    else
    {
        /*
         * Get counter status
         */
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

        if (counter_interval > 0)
        {
            if (journal_entry->is_add)
            {
                BCM_PBMP_REMOVE(counter_pbmp, *pbmp);
            }
            else
            {
                BCM_PBMP_OR(counter_pbmp, *pbmp);
            }
            /*
             * Continue counter thread
             */
            SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
        }
        else
        {
            bcm_pbmp_t *counter_pbmp_p;
            SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_PBMP, (void **) &counter_pbmp_p));
            if (journal_entry->is_add)
            {
                BCM_PBMP_REMOVE(*counter_pbmp_p, *pbmp);
            }
            else
            {
                BCM_PBMP_OR(*counter_pbmp_p, *pbmp);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for linkscan pause and continue
 */
static shr_error_e
dnx_fabric_port_er_linkscan_pause_continue_rollback(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *is_paused = NULL;

    SHR_FUNC_INIT_VARS(unit);

    is_paused = (int *) metadata;

    if (*is_paused)
    {
        SHR_IF_ERR_EXIT(_bcm_linkscan_continue(unit));
    }
    else
    {
        SHR_IF_ERR_EXIT(_bcm_linkscan_pause(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Insert imb related info to generic ER database
 */
static shr_error_e
dnx_fabric_port_er_imb_entry_insert(
    int unit,
    int is_add,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new
                    (unit, (uint8 *) &is_add, sizeof(int),
                     (uint8 *) &logical_port, sizeof(bcm_port_t),
                     &dnx_fabric_port_error_recovery_probe_detach_rollback));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Insert counter thread related info to generic ER database
 */
static shr_error_e
dnx_fabric_port_er_counter_thread_entry_insert(
    int unit,
    dnx_fabric_port_er_counter_journal_t * journal_entry,
    bcm_pbmp_t * pbmp)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new
                    (unit, (uint8 *) journal_entry, sizeof(dnx_fabric_port_er_counter_journal_t),
                     (uint8 *) pbmp, sizeof(bcm_pbmp_t), &dnx_fabric_port_er_counter_rollback));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Insert linkscan thread related info to generic ER database
 */
static shr_error_e
dnx_fabric_port_er_linkscan_thread_entry_insert(
    int unit,
    int linkscan_paused)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit, (uint8 *) &linkscan_paused, sizeof(int),
                                                        (uint8 *) &linkscan_paused, sizeof(int),
                                                        &dnx_fabric_port_er_linkscan_pause_continue_rollback));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Attach the fabric port in IMB
 */
static shr_error_e
dnx_fabric_if_port_imb_add(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int need_err_recovery)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(imb_port_add(unit, logical_port, flags));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    if (need_err_recovery)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_port_er_imb_entry_insert(unit, 1, logical_port));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Remove the fabric port in IMB
 */
static shr_error_e
dnx_fabric_if_port_imb_remove(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(imb_port_remove(unit, logical_port));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_IF_ERR_EXIT(dnx_fabric_port_er_imb_entry_insert(unit, 0, logical_port));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   1. Stop the counter thread
 *   2. Add the pbmp to the counter bitmap
 */
static shr_error_e
dnx_fabric_if_port_counter_bitmap_add(
    int unit,
    bcm_pbmp_t * pbmp)
{
    int counter_interval = 0;
    uint32 counter_flags = 0;
    bcm_pbmp_t counter_pbmp;
    dnx_fabric_port_er_counter_journal_t counter_journal_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    /*
     * Stop counter thread and update PBMP
     */
    if (counter_interval > 0)
    {
        soc_counter_stop(unit);
        BCM_PBMP_OR(counter_pbmp, *pbmp);
    }
    else
    {
        bcm_pbmp_t *counter_pbmp_p;
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_PBMP, (void **) &counter_pbmp_p));
        /** update counter bitmap directly */
        BCM_PBMP_OR(*counter_pbmp_p, *pbmp);
    }
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    counter_journal_entry.is_add = 1;
    counter_journal_entry.is_start_thread = 0;
    SHR_IF_ERR_EXIT(dnx_fabric_port_er_counter_thread_entry_insert(unit, &counter_journal_entry, pbmp));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   1. Stop the counter thread
 *   2. Remove the pbmp to the counter bitmap
 */
static shr_error_e
dnx_fabric_if_port_counter_bitmap_remove(
    int unit,
    bcm_pbmp_t * pbmp)
{
    int counter_interval = 0;
    uint32 counter_flags = 0;
    bcm_pbmp_t counter_pbmp;
    dnx_fabric_port_er_counter_journal_t counter_journal_entry;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get counter status
     */
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    /*
     * Pause counter thread
     */
    SHR_IF_ERR_EXIT(soc_counter_stop(unit));
    /*
     * Remove the desired pbmp from counter_pbmp, so they won't exist in the
     * bitmap when we later call soc_counter_start().
     */
    BCM_PBMP_REMOVE(counter_pbmp, *pbmp);
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    counter_journal_entry.is_add = 0;
    counter_journal_entry.is_start_thread = 0;
    SHR_IF_ERR_EXIT(dnx_fabric_port_er_counter_thread_entry_insert(unit, &counter_journal_entry, pbmp));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Start the counter thread
 */
static shr_error_e
dnx_fabric_if_port_counter_thread_start(
    int unit)
{
    int counter_interval = 0;
    uint32 counter_flags = 0;
    bcm_pbmp_t counter_pbmp;
    dnx_fabric_port_er_counter_journal_t counter_journal_entry;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get counter status
     */
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
    if (counter_interval > 0)
    {
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        counter_journal_entry.is_start_thread = 1;
        SHR_IF_ERR_EXIT(dnx_fabric_port_er_counter_thread_entry_insert(unit, &counter_journal_entry, &counter_pbmp));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Start the linkscan thread
 */
static shr_error_e
dnx_fabric_if_port_linkscan_continue(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_bcm_linkscan_continue(unit));
    SHR_IF_ERR_EXIT(dnx_fabric_port_er_linkscan_thread_entry_insert(unit, 0));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Pause the linkscan thread
 */
static shr_error_e
dnx_fabric_if_port_linkscan_pause(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_bcm_linkscan_pause(unit));
    SHR_IF_ERR_EXIT(dnx_fabric_port_er_linkscan_thread_entry_insert(unit, 1));
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_port_probe(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp)
{
    bcm_port_t logical_port = 0;
    int broadcast_load_fabric = 0;
    phymod_firmware_load_method_t fw_load_method_fabric = phymodFirmwareLoadMethodNone;
    int is_init_sequence = 0;
    uint32 flags = 0;
    dnx_algo_port_fabric_add_t fabric_add_info;
    pbmp_t sfi_pbmp, *pbmp_p;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*okay_pbmp);

    is_init_sequence = !(dnx_init_is_init_done_get(unit));

    /*
     * Loading firmware can take relatively long time.
     * On init sequence the user probably want to load firmware for all
     * the FSRDs, which can take very long time.
     * If External Method is used, we can load firmware by broadcast, which
     * means that the firmware will be loaded to all FSRDs simultaneously.
     * In that case, we init the port the following way:
     * 1. Init the port until firmware load.
     * 2. Load the firmware to all FSRDs using broadcast.
     * 3. Resume init the port after firmware load.
     */
    if (is_init_sequence)
    {
        fw_load_method_fabric = dnx_data_port.static_add.fabric_fw_load_method_get(unit);
        if (fw_load_method_fabric == phymodFirmwareLoadMethodExternal)
        {
            broadcast_load_fabric = 1;
        }
    }

    if (broadcast_load_fabric)
    {
        /** Set flag to load port until firmware load */
        IMB_PORT_ADD_F_INIT_PASS1_SET(flags);
    }

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_clock_power_down) &&
        dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        /** in single fap, take fabric out of power down when adding first link */
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_SFI_ALL_BITMAP, &pbmp_p));
        sfi_pbmp = *pbmp_p;
        if (_SHR_PBMP_IS_NULL(sfi_pbmp))
        {
            /** Power down fabric blocks */
            SHR_IF_ERR_EXIT(dnx_fabric_blocks_power_down_set(unit, FALSE));
        }
    }

    BCM_PBMP_ITER(pbmp, logical_port)
    {
        /*
         * Add Fabric port to port mgmt database.
         */
        sal_memset(&fabric_add_info, 0, sizeof(dnx_algo_port_fabric_add_t));
        fabric_add_info.link_id = logical_port - dnx_data_port.general.fabric_port_base_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_add(unit, logical_port, &fabric_add_info));

        /*
         * Init the fabric port.
         */
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_imb_add(unit, logical_port, flags, 1));
    }

    /*
     * Update FSRD broadcast info.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_brdc_fsrd_blk_id_set(unit));

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    if (broadcast_load_fabric && !BCM_PBMP_IS_NULL(pbmp))
    {
        /*
         * Load fabric firmware
         */
        SHR_IF_ERR_EXIT(soc_dnxc_fabric_broadcast_firmware_loader(unit, &pbmp));
    }
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    BCM_PBMP_ITER(pbmp, logical_port)
    {
        if (broadcast_load_fabric)
        {
            /*
             * Resume init the fabric port after firmware load.
             */
            IMB_PORT_ADD_F_INIT_PASS1_CLR(flags);
            IMB_PORT_ADD_F_INIT_PASS2_SET(flags);
            /*
             * Since it's the 2nd time we call imb_port_add for the port,
             * we want to skip the imb internal configurations.
             */
            IMB_PORT_ADD_F_SKIP_SETTINGS_SET(flags);

            SHR_IF_ERR_EXIT(dnx_fabric_if_port_imb_add(unit, logical_port, flags, 0));
        }

        /*
         * Mark added fabric link as valid
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_add_process_done(unit, logical_port));

        BCM_PBMP_PORT_ADD(*okay_pbmp, logical_port);
    }

    /*
     * Init FMAC for the fabric link
     */
    BCM_PBMP_ITER(*okay_pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_mac_init(unit, logical_port));
    }

    if (!is_init_sequence)
    {
        /*
         * Update Counter thread
         */
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_counter_bitmap_add(unit, okay_pbmp));
        /*
         * Start counter thread
         */
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_counter_thread_start(unit));
        /*
         * Update linkscan thread for fabric link
         */
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        BCM_PBMP_ITER(*okay_pbmp, logical_port)
        {
            SHR_IF_ERR_EXIT(_bcm_linkscan_port_init(unit, logical_port));
        }
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        /*
         * On init, each fabric link is configured to have llfc enabled.
         * So all the ports that are now probed should have the configurations
         * they would have get on init.
         */
        BCM_PBMP_ITER(*okay_pbmp, logical_port)
        {
            bcm_dnx_fabric_link_control_set(unit, logical_port, bcmFabricLLFControlSource, 0);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_port_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*detached);
    /*
     * Mark as removed port
     */
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_remove_process_start(unit, logical_port));
    }
    /*
     * Update counter status
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_port_counter_bitmap_remove(unit, &pbmp));
    /*
     * Pause linkscan thread
     */
    PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_linkscan_set(unit, logical_port, BCM_LINKSCAN_MODE_NONE));
    }

    SHR_IF_ERR_EXIT(dnx_fabric_if_port_linkscan_pause(unit));
    /*
     * Remove port
     */
    PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_imb_remove(unit, logical_port));
        /** Remove from port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_remove(unit, logical_port));

        BCM_PBMP_PORT_ADD(*detached, logical_port);
    }
    SHR_IF_ERR_EXIT(dnx_fabric_if_brdc_fsrd_blk_id_set(unit));
    /*
     * Continue linkscan thread
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_port_linkscan_continue(unit));
    /*
     * Continue counter thread
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_port_counter_thread_start(unit));
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_link_rate_egress_pps_set(
    int unit,
    bcm_port_t port,
    uint32 pps,
    uint32 burst)
{
    uint32 fmac_core_clock_speed = 0;
    uint32 nof_tiks = 0;
    uint64 nof_tiks64;
    int link = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_verify(unit, port, DNX_ALGO_PORT_TYPE_FABRIC));

    if ((pps == 0 && burst != 0) || (burst == 0 && pps != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Both pps and burst must equal to 0 in order to disable the shaper");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link));

    /*
     * Calculate packets per tiks:
     * nof tiks = burst * core_clock / pps
     */
    /** KHz to Hz */
    fmac_core_clock_speed = dnx_data_fabric.general.fmac_clock_khz_get(unit) * 1000;
    COMPILER_64_SET(nof_tiks64, 0, burst);
    /** packets per tiks to packets per time */
    COMPILER_64_UMUL_32(nof_tiks64, fmac_core_clock_speed);

    nof_tiks = 0;
    /** pps == 0 --> disables rate limiting */
    if (pps != 0)
    {
        uint64 pps64;

        COMPILER_64_SET(pps64, 0, pps);
        COMPILER_64_UDIV_64(nof_tiks64, pps64);
        nof_tiks = COMPILER_64_LO(nof_tiks64);
    }

    /*
     * Config shaper
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_rate_egress_ppt_set(unit, link, burst, nof_tiks));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_link_rate_egress_pps_get(
    int unit,
    bcm_port_t port,
    uint32 *pps,
    uint32 *burst)
{
    uint64 pps64;
    uint64 nof_tiks64;
    uint32 nof_tiks = 0;
    uint32 fmac_core_clock_speed = 0;
    int link = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(pps, _SHR_E_PARAM, "pps");
    SHR_NULL_CHECK(burst, _SHR_E_PARAM, "burst");

    SHR_IF_ERR_EXIT(dnx_algo_port_type_verify(unit, port, DNX_ALGO_PORT_TYPE_FABRIC));

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link));

    /*
     * Get shaper info
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_rate_egress_ppt_get(unit, link, burst, &nof_tiks));

    if (nof_tiks == 0)
    {
        *pps = 0;
        SHR_EXIT();
    }

    /*
     * Calculate packets per sec
     * pps = burst * core_clock / nof_tiks
     */
    fmac_core_clock_speed = dnx_data_fabric.general.fmac_clock_khz_get(unit);
    COMPILER_64_SET(pps64, 0, *burst);
    /** packets per tiks to packets per time */
    COMPILER_64_UMUL_32(pps64, fmac_core_clock_speed);
    /** KHz to Hz */
    COMPILER_64_UMUL_32(pps64, 1000);

    COMPILER_64_SET(nof_tiks64, 0, nof_tiks);
    COMPILER_64_UDIV_64(pps64, nof_tiks64);

    *pps = COMPILER_64_LO(pps64);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_fsrd_block_enable_get(
    int unit,
    int fsrd_block,
    int *enable)
{
    int blk;
    int *fsrd_block_p;
    uint8 *block_valid_p;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FSRD_BLOCK_INT, &fsrd_block_p));
    fsrd_block_p = &(fsrd_block_p[fsrd_block]);
    blk = *fsrd_block_p;
    SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
    block_valid_p = &(block_valid_p[blk]);
    *enable = (*block_valid_p == 1) ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_fmac_block_enable_get(
    int unit,
    int fmac_block,
    int *enable)
{
    int blk;
    int *fmac_block_p;
    uint8 *block_valid_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FMAC_BLOCK_INT, &fmac_block_p));
    fmac_block_p = &(fmac_block_p[fmac_block]);
    blk = *fmac_block_p;
    SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
    block_valid_p = &(block_valid_p[blk]);
    *enable = (*block_valid_p == 1) ? 1 : 0;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set clock power of master FSRD.
 *   In some devices, the reference clock of a FSRD is coming from
 *   another FSRD (which we call 'master FSRD').
 */
static shr_error_e
dnx_fabric_if_master_fsrd_clock_power_set(
    int unit,
    int fsrd_block,
    int enable)
{
    uint32 master_fsrd_block = dnx_data_fabric.blocks.fsrd_ref_clk_master_get(unit, fsrd_block)->master_fsrd_index;
    int is_master_fsrd_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable FSRD clock power if not already enabled
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_get(unit, master_fsrd_block, &is_master_fsrd_enabled));
    if (is_master_fsrd_enabled == 0)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_clock_power_set(unit, master_fsrd_block, enable));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_fsrd_clock_power_get(
    int unit,
    int fsrd_id,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 is_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_FSRD_CLOCKS_POWER_DOWN, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FSRD, fsrd_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &is_enable);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = is_enable ? 0 : 1;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_fsrd_clock_power_set(
    int unit,
    int fsrd_id,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_FSRD_CLOCKS_POWER_DOWN, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FSRD, fsrd_id);

    /*
     * request for value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 0 : 1);

    /*
     * commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_fsrd_block_enable_set(
    int unit,
    int fsrd_block,
    int enable)
{
    uint32 entry_handle_id;
    int fmac_block = 0;
    int blk = 0;
    int fmac_index = 0;

    int *fsrd_block_p;
    int *fmac_block_p;
    uint8 *block_valid_p;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (enable)
    {
        /** Enable master FSRD clock during power up sequence, if necessary */
        SHR_IF_ERR_EXIT(dnx_fabric_if_master_fsrd_clock_power_set(unit, fsrd_block, 1));
    }

    /** Power up/down FSRD block */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_BLOCKS_POWER_DOWN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FSRD, fsrd_block);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Mark FSRD block as valid/non-valid */
    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FSRD_BLOCK_INT, &fsrd_block_p));
    fsrd_block_p = &(fsrd_block_p[fsrd_block]);
    blk = *fsrd_block_p;
    SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
    block_valid_p = &(block_valid_p[blk]);
    *block_valid_p = enable ? 1 : 0;

    /** Mark FAMC blocks as valid/non-valid */
    fmac_block = fsrd_block * dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit);
    for (fmac_index = fmac_block; fmac_index < fmac_block + dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit);
         fmac_index++)
    {
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FMAC_BLOCK_INT, &fmac_block_p));
        fmac_block_p = &(fmac_block_p[fmac_index]);
        blk = *fmac_block_p;
        SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
        block_valid_p = &(block_valid_p[blk]);
        *block_valid_p = enable ? 1 : 0;

        if (*block_valid_p)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_if_mac_init(unit, fmac_index));
        }
    }

    if (!enable)
    {
        /** Power down master FSRD clock, during power down sequence, if necessary */
        SHR_IF_ERR_EXIT(dnx_fabric_if_master_fsrd_clock_power_set(unit, fsrd_block, 0));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_port_fsrd_block_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    int link;
    int fsrd_block;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link));
    fsrd_block = link / dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_set(unit, fsrd_block, enable));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_fsrd_blocks_power_init(
    int unit)
{
    int fsrd_block = 0;
    bcm_pbmp_t supported_fsrds;
    int nof_fsrd = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get supported FSRDs
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_supported_fsrd_get(unit, &supported_fsrds));

    /*
     * Power all FSRD blocks according to their supported bitmap
     */
    for (fsrd_block = 0; fsrd_block < nof_fsrd; ++fsrd_block)
    {
        /** Disable FSRD to align sw state with hw state */
        SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_set(unit, fsrd_block, 0));

        if (_SHR_PBMP_MEMBER(supported_fsrds, fsrd_block))
        {
            /** Set FSRD state according to supported FSRD bitmap (supported_links) */
            SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_set(unit, fsrd_block, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_supported_fsrd_get(
    int unit,
    bcm_pbmp_t * supported_fsrds)
{

    int pm_id = 0;
    int link_id = 0;
    int nof_links_in_pm = 0;
    int nof_fabric_pms = 0;
    int i_link = 0;
    bcm_pbmp_t supported_links;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*supported_fsrds);

    /** Init local Fabric ports */
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_links_in_pm = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);

    /** get a bitmap of all supported links */
    supported_links = dnx_data_fabric.links.general_get(unit)->supported_links;

    /** Create fabric ports bitmap */
    for (pm_id = 0; pm_id < nof_fabric_pms; ++pm_id)
    {
        for (i_link = 0; i_link < nof_links_in_pm; ++i_link)
        {
            /** calculate link-id */
            link_id = pm_id * nof_links_in_pm + i_link;

            /** add corresponding FSRD to bitmap */
            if (_SHR_PBMP_MEMBER(supported_links, link_id))
            {
                BCM_PBMP_PORT_ADD(*supported_fsrds, pm_id);
            }
        }
    }

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_brdc_fsrd_blk_id_set(
    int unit)
{
    uint32 entry_handle_id;
    int blk = -1;
    int chain = -1;
    int index_in_chain = -1;
    int fsrd = 0;
    int fmac = 0;
    int fsrd_last_in_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_SBUS_CHAINS];
    int fmac_last_in_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_SBUS_CHAINS];
    int fsrd_sbus_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_SBUS_CHAINS][DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FSRD];
    int fmac_sbus_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_SBUS_CHAINS][DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC];
    int enable = 0;
    uint32 nof_fsrds = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    uint32 nof_fmacs = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    uint32 nof_sbus_chains = dnx_data_fabric.blocks.nof_sbus_chains_get(unit);
    uint32 fsrds_per_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_SBUS_CHAINS] = { 0 };
    uint32 fmacs_per_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_SBUS_CHAINS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(fsrd_last_in_chain, -1, sizeof(fsrd_last_in_chain));
    sal_memset(fmac_last_in_chain, -1, sizeof(fmac_last_in_chain));
    sal_memset(fsrd_sbus_chain, -1, sizeof(fsrd_sbus_chain));
    sal_memset(fmac_sbus_chain, -1, sizeof(fmac_sbus_chain));

    /*
     * Mark the last FSRD in the SBUS chain.
     * Note that the order of the FSRDs in the SBUS chain is not necessarily
     * by their numerical order, so we put all active fsrds in fsrd_sbus_chain
     * according to their index in the chain, and then we search for the last
     * fsrd in the chain that is active, meaning we start the search from the
     * end of fsrd_sbus_chain array.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_FSRD_SBUS_LAST_IN_CHAIN, &entry_handle_id));
    for (fsrd = 0; fsrd < nof_fsrds; fsrd++)
    {
        chain = dnx_data_fabric.blocks.fsrd_sbus_chain_get(unit, fsrd)->chain;
        index_in_chain = dnx_data_fabric.blocks.fsrd_sbus_chain_get(unit, fsrd)->index_in_chain;

        SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_get(unit, fsrd, &enable));
        if (enable)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FSRD, fsrd);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAST_IN_CHAIN, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            fsrd_sbus_chain[chain][index_in_chain] = fsrd;
        }
        ++fsrds_per_chain[chain];
    }

    for (chain = 0; chain < nof_sbus_chains; ++chain)
    {
        for (blk = fsrds_per_chain[chain] - 1; blk >= 0; --blk)
        {
            if (fsrd_sbus_chain[chain][blk] != -1)
            {
                fsrd_last_in_chain[chain] = fsrd_sbus_chain[chain][blk];
                break;
            }
        }
    }

    for (chain = 0; chain < nof_sbus_chains; ++chain)
    {
        if (fsrd_last_in_chain[chain] != -1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FSRD, fsrd_last_in_chain[chain]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAST_IN_CHAIN, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /*
     * Mark the last FMAC in the SBUS chain.
     * Note that the order of the FMACs in the SBUS chain is not necessarily
     * by their numerical order, so we put all active fmacs in fmac_sbus_chain
     * according to their index in the chain, and then we search for the last
     * fmac in the chain that is active, meaning we start the search from the
     * end of fmac_sbus_chain array.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_IF_FMAC_SBUS_LAST_IN_CHAIN, entry_handle_id));
    for (fmac = 0; fmac < nof_fmacs; fmac++)
    {
        chain = dnx_data_fabric.blocks.fmac_sbus_chain_get(unit, fmac)->chain;
        index_in_chain = dnx_data_fabric.blocks.fmac_sbus_chain_get(unit, fmac)->index_in_chain;

        SHR_IF_ERR_EXIT(dnx_fabric_if_fmac_block_enable_get(unit, fmac, &enable));
        if (enable)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmac);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAST_IN_CHAIN, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            fmac_sbus_chain[chain][index_in_chain] = fmac;
        }
        ++fmacs_per_chain[chain];
    }

    for (chain = 0; chain < nof_sbus_chains; ++chain)
    {
        for (blk = fmacs_per_chain[chain] - 1; blk >= 0; --blk)
        {
            if (fmac_sbus_chain[chain][blk] != -1)
            {
                fmac_last_in_chain[chain] = fmac_sbus_chain[chain][blk];
                break;
            }
        }
    }

    for (chain = 0; chain < nof_sbus_chains; ++chain)
    {
        if (fmac_last_in_chain[chain] != -1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmac_last_in_chain[chain]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAST_IN_CHAIN, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Force traffic on clos mode
 */
static shr_error_e
dnx_fabric_force_clos_set(
    int unit,
    dnx_fabric_force_e force)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_force_e dbal_fabric_force;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (force == DNX_FABRIC_FORCE_FABRIC)
    {
        dbal_fabric_force = DBAL_ENUM_FVAL_FABRIC_FORCE_FABRIC;
    }
    else if (force == DNX_FABRIC_FORCE_LOCAL0)
    {
        dbal_fabric_force = DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_0;
    }
    else if (force == DNX_FABRIC_FORCE_LOCAL1
             && dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        dbal_fabric_force = DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_1;
    }
    else if (force == DNX_FABRIC_FORCE_RESTORE)
    {
        dbal_fabric_force = DBAL_ENUM_FVAL_FABRIC_FORCE_DISABLE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Force option %d is not supported", force);
    }

    /*
     * Force traffic
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_FORCE, &entry_handle_id));

    /** Set Force Fabric for data and control cells */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_FORCE, INST_SINGLE, dbal_fabric_force);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Force tdm traffic
 */
static shr_error_e
dnx_fabric_force_tdm_set(
    int unit,
    dnx_fabric_force_e force)
{
    uint32 entry_handle_id;
    int is_force_fabric;
    int is_force_local;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (force == DNX_FABRIC_FORCE_FABRIC)
    {
        /** Force Fabric for data and control cells */
        is_force_fabric = TRUE;

        /** Disable Force Local */
        is_force_local = FALSE;
    }
    else if (force == DNX_FABRIC_FORCE_LOCAL0)
    {
        /** Disable Force Fabric */
        is_force_fabric = FALSE;

        /** Force traffic to local core */
        is_force_local = TRUE;
    }
    else if (force == DNX_FABRIC_FORCE_RESTORE)
    {
        if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
        {
            /** Force Fabric for data and control cells */
            is_force_fabric = TRUE;

            /** Disable Force Local */
            is_force_local = FALSE;
        }
        else
        {
            /** Disable Force Fabric */
            is_force_fabric = FALSE;

            /** Disable Force Local */
            is_force_local = FALSE;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Force option %d is not supported", force);
    }

    /*
     * Force traffic
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_FORCE_TDM, &entry_handle_id));

    /** Set Force Fabric for data and control cells */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_FABRIC, INST_SINGLE, is_force_fabric);

    /** Set Force Local */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_LOCAL, INST_SINGLE, is_force_local);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Force traffic on single fap mode
 */
static shr_error_e
dnx_fabric_force_single_fap_set(
    int unit,
    dnx_fabric_force_e force)
{
    dbal_enum_value_field_system_mesh_context_e dqcq_context;
    uint32 max_nof_fap_ids_per_core = dnx_data_device.general.max_nof_fap_ids_per_core_get(unit);
    uint32 *modid_array = NULL;
    int modid_count = 0;
    int i_modid = 0;
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(modid_array, max_nof_fap_ids_per_core, "modid_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        if (force == DNX_FABRIC_FORCE_FABRIC)
        {
            dqcq_context = DBAL_ENUM_FVAL_SYSTEM_MESH_CONTEXT_DEST_0;
        }
        else if (force == DNX_FABRIC_FORCE_LOCAL0)
        {
            dqcq_context = DBAL_ENUM_FVAL_SYSTEM_MESH_CONTEXT_LOCAL_0;
        }
        else if (force == DNX_FABRIC_FORCE_LOCAL1)
        {
            dqcq_context = DBAL_ENUM_FVAL_SYSTEM_MESH_CONTEXT_LOCAL_1;
        }
        else if (force == DNX_FABRIC_FORCE_RESTORE)
        {
            if (core == 0)
            {
                dqcq_context = DBAL_ENUM_FVAL_SYSTEM_MESH_CONTEXT_LOCAL_0;
            }
            else
            {
                dqcq_context = DBAL_ENUM_FVAL_SYSTEM_MESH_CONTEXT_LOCAL_1;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Force option %d is not supported", force);
        }

        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get_all(unit, core, modid_array, &modid_count));

        for (i_modid = 0; i_modid < modid_count; i_modid++)
        {
            SHR_IF_ERR_EXIT(dnx_stk_sys_mesh_dqcq_map_set(unit, modid_array[i_modid], dqcq_context));
        }
    }

exit:
    SHR_FREE(modid_array);
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_force_set(
    int unit,
    dnx_fabric_force_e force)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Force traffic
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_force_clos_set(unit, force));
    }
    else if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_force_single_fap_set(unit, force));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "fabric force is not supported for Mesh mode");
    }

    /*
     * Force TDM traffic
     */
    if (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_force_tdm_set(unit, force));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_force_tdm_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set 'fabric force tdm' to default
     */
    if (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_force_tdm_set(unit, DNX_FABRIC_FORCE_RESTORE));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_if_link_status_verify(
    int unit,
    bcm_port_t link_id,
    uint32 *link_status,
    uint32 *errored_token_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_verify(unit, link_id, DNX_ALGO_PORT_TYPE_FABRIC));

    SHR_NULL_CHECK(link_status, _SHR_E_PARAM, "link_status");
    SHR_NULL_CHECK(errored_token_count, _SHR_E_PARAM, "errored_token_count");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Clear fabric link status interrupts in HW.
 */
static shr_error_e
dnx_fabric_if_link_status_clear(
    int unit,
    int link_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_ERROR, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIZE_ERROR, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MISALIGN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CODE_GROUP_ERROR, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get fabric link status
 * \param [in] unit -
 *   The unit number.
 * \param [in] link_id -
 *   Fabric link index.
 * \param [out] link_status -
 *   Link status indications.
 *   Indications can be 'OR' of the following flags:
 *     BCM_FABRIC_LINK_STATUS_CRC_ERROR
 *     BCM_FABRIC_LINK_STATUS_SIZE_ERROR
 *     BCM_FABRIC_LINK_STATUS_MISALIGN
 *     BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR
 *     BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP
 *     BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS
 * \param [out] errored_token_count -
 *   Number of errored tokens.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Note that the internal fucntion dnx_fabric_if_link_status_get
 *     uses link id, and the api function uses port id.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_if_link_status_get(
    int unit,
    int link_id,
    uint32 *link_status,
    uint32 *errored_token_count)
{
    uint32 entry_handle_id;
    uint32 crc_error = 0;
    uint32 size_error = 0;
    uint32 misalign = 0;
    uint32 code_group_error = 0;
    uint32 sig_acc = 0;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *link_status = 0;

    /*
     * Get link status
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_IF_LINK_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_ERRORED_TOKENS, INST_SINGLE, errored_token_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRC_ERROR, INST_SINGLE, &crc_error);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SIZE_ERROR, INST_SINGLE, &size_error);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MISALIGN, INST_SINGLE, &misalign);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CODE_GROUP_ERROR, INST_SINGLE, &code_group_error);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (crc_error)
    {
        *link_status |= BCM_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    if (size_error)
    {
        *link_status |= BCM_FABRIC_LINK_STATUS_SIZE_ERROR;
    }
    if (misalign)
    {
        *link_status |= BCM_FABRIC_LINK_STATUS_MISALIGN;
    }
    if (code_group_error)
    {
        *link_status |= BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, link_id, &logical_port));
    SHR_IF_ERR_EXIT(soc_dnxc_port_rx_locked_get(unit, logical_port, &sig_acc));
    if (!sig_acc)
    {
        *link_status |= BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP;
    }

    if (*errored_token_count < 63)
    {
        *link_status |= BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS;
    }

    /*
     * Clear sticky indication
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_link_status_clear(unit, link_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get fabric link status
 * \param [in] unit -
 *   The unit number.
 * \param [in] link_id -
 *   Fabric logical port.
 * \param [out] link_status -
 *   Link status indications.
 *   Indications can be 'OR' of the following flags:
 *     BCM_FABRIC_LINK_STATUS_CRC_ERROR
 *     BCM_FABRIC_LINK_STATUS_SIZE_ERROR
 *     BCM_FABRIC_LINK_STATUS_MISALIGN
 *     BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR
 *     BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP
 *     BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS
 * \param [out] errored_token_count -
 *   Number of errored tokens.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Note that the internal fucntion dnx_fabric_if_link_status_get
 *     uses link id, and the api function uses port id.
 * \see
 *   * None.
 */
int
bcm_dnx_fabric_link_status_get(
    int unit,
    bcm_port_t link_id,
    uint32 *link_status,
    uint32 *errored_token_count)
{
    int link_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_if_link_status_verify(unit, link_id, link_status, errored_token_count));

    /*
     * Convert fabric port to fabric link index.
     * The name 'link_id' in the API is confusing - it's actually
     * means 'fabric logical_port'.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, link_id, &link_idx));

    SHR_IF_ERR_EXIT(dnx_fabric_if_link_status_get(unit, link_idx, link_status, errored_token_count));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
