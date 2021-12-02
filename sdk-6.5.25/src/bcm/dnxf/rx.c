/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * DNXF RX
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX

#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf/auto_generated/dnxf_rx_dispatch.h>
#include <bcm/debug.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/property.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dnxf/rx.h>

#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <soc/dnxc/dnxc_captured_buffer.h>

/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>

#define BCM_DNXF_RX_MAX_PAYLOAD_SIZE_IN_BYTES (256)

/*
 * Following two values is not strict. Can be increased if there is a need to
 * because it only depends on available SW resources
 */
#define DNXF_FABRIC_SR_CELLS_MAX_CELLS_IN_BUFFER        (30)
#define DNXF_FABRIC_CAPTURED_CELLS_MAX_CELLS_IN_BUFFER  (50)

typedef struct _bcm_dnxf_rx_callback_info_s
{
    bcm_rx_cb_f cb_f;
    void *cookie;
    int is_valid;
} _bcm_dnxf_rx_callback_info_t;

_bcm_dnxf_rx_callback_info_t *bcm_dnxf_rx_callback_info_array[BCM_UNITS_MAX];

/*
 * Function:
 *      _bcm_dnxf_rx_captured_cell_get
 * Purpose:
 *      Used by the RX thread to return the captured cell if such.
 *      If no cell return _SHR_E_EMPTY
 * Parameters:
 *      unit                 - (IN) Unit number
 *      cell                 - (OUT) NOT USED
 *
 * Returns:
 *      _SHR_E_xxx
 */
STATIC int
_bcm_dnxf_rx_captured_cell_get(
    int unit,
    dnxc_captured_cell_t * cell)
{
    int rv;
    int is_empty;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Check if there is something inside the SW buffer
     */
    SHR_IF_ERR_EXIT(dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty));

    /**
     * If buffer is not empty start emptying it
     */
    while (is_empty)
    {
        rv = soc_dnxf_fabric_cell_load(unit);
        if (rv == _SHR_E_EMPTY)
        {
            break;
        }
        SHR_IF_ERR_EXIT(rv);

        SHR_IF_ERR_EXIT(dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty));
    }

    if (is_empty)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

    /*
     * Pull cell from data base
     */
    SHR_IF_ERR_EXIT(dnxc_captured_buffer_get(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, cell));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnxf_rx_pkt_thread
 * Purpose:
 *      The callback constantly called from the RX thread.
 *      It checks if there is a cell captured and if so parses it
 *      and calls the function registered by the user by providing to it the cell
 * Parameters:
 *      unit                      - (IN) Unit number
 *      user_data                 - (IN) NOT USED
 *
 * Returns:
 *      _SHR_E_xxx
 */
STATIC int
_bcm_dnxf_rx_pkt_thread(
    int unit,
    void *user_data)
{
    int rv;
    int is_quit_signaled = 0;
    dnxc_captured_cell_t cell;
    bcm_pkt_t packet;
    bcm_pkt_blk_t packet_data;
    int cell_received;

    SHR_FUNC_INIT_VARS(unit);

    while (1)
    {
        SHR_IF_ERR_EXIT(shr_thread_manager_is_quit_signaled_get
                        (SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr, &is_quit_signaled));
        if (is_quit_signaled)
        {
            SHR_EXIT();
        }

        /*
         * Get a cpu cell
         */
        rv = _bcm_dnxf_rx_captured_cell_get(unit, &cell);
        if ((rv != _SHR_E_NONE) && (rv != _SHR_E_EMPTY))
        {
            LOG_ERROR(BSL_LS_BCM_RX, (BSL_META_U(unit, "bcm_dnxf_rx_captured_cell_get: failed \n")));
            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }

        cell_received = (rv == _SHR_E_EMPTY) ? 0 : 1;
        /*
         * build "packet" struct from "captured_cell" struct
         */

        if (cell_received)
        {
            uint32 byte_data;
            uint32 offset_byte;
            int i;
            uint8 pkt_data[BCM_DNXF_RX_MAX_PAYLOAD_SIZE_IN_BYTES] = { 0 };

            packet_data.len = cell.cell_size;

            offset_byte = 255;

            for (i = 0; i < packet_data.len; i++)
            {
                SHR_BITCOPY_RANGE(&byte_data, 0, cell.data, offset_byte * 8, 8);
                pkt_data[i] = byte_data & 0xFF;
                offset_byte--;
            }

            packet_data.data = pkt_data;

            packet.pkt_data = &packet_data;
            packet.unit = unit;
            packet.cos = cell.pipe_index;
            packet.src_mod = cell.source_device;

            /*
             * Initiate the callback function
             */
            if (bcm_dnxf_rx_callback_info_array[unit]->cb_f != NULL)
            {
                bcm_dnxf_rx_callback_info_array[unit]->cb_f(unit, &packet,
                                                            bcm_dnxf_rx_callback_info_array[unit]->cookie);
            }
            else
            {
                LOG_ERROR(BSL_LS_BCM_RX, (BSL_META_U(unit, "rx_pkt_thread failed: callback function is NULL \n")));
                SHR_SET_CURRENT_ERR(_SHR_E_NO_HANDLER);
                SHR_EXIT();
            }

        }
        else    /* No cell received */
        {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_rx_register(
    int unit,
    const char *name,
    bcm_rx_cb_f cb_f,
    uint8 pri,
    void *cookie,
    uint32 flags)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    bcm_dnxf_rx_callback_info_array[unit]->is_valid = 1;
    bcm_dnxf_rx_callback_info_array[unit]->cb_f = cb_f;
    bcm_dnxf_rx_callback_info_array[unit]->cookie = cookie;

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    int is_active = 0;
    int interval;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_NULL_CHECK(cfg, _SHR_E_PARAM, "cfg");

    if (cfg->num_of_cpu_addresses != 0)
    {
        SHR_NULL_CHECK(cfg->cpu_address, _SHR_E_PARAM, "cfg->cpu_address");
    }

    if (bcm_dnxf_rx_callback_info_array[unit]->is_valid == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bcm_rx_start error - must register a callback function first");
    }

    /*
     * Verify that the thread is not running already
     */
    SHR_IF_ERR_EXIT(shr_thread_manager_is_active_get(SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr, &is_active));
    if (is_active)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bcm_rx_start error - thread already running");
    }

    /*
     * assign the buffers with the according mod ids
     */
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                    (unit, mbcm_dnxf_rx_cpu_address_modid_set, (unit, cfg->num_of_cpu_addresses, cfg->cpu_address)));

    interval = dnxf_data_fabric.cell.fifo_dma_enable_get(unit) ? sal_sem_FOREVER : 1000000;
    SHR_IF_ERR_EXIT(shr_thread_manager_interval_set(SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr, interval));
    SHR_IF_ERR_EXIT(shr_thread_manager_start(SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr));
    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int
bcm_dnxf_rx_stop(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * Stop the thread
     */
    SHR_IF_ERR_EXIT(shr_thread_manager_stop(SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr, 0));

    /*
     * In case this function is being called without cfg, we still want to
     * stop the thread.
     */
    if (cfg != NULL)
    {
        if (cfg->num_of_cpu_addresses != 0)
        {
            SHR_NULL_CHECK(cfg->cpu_address, _SHR_E_PARAM, "cfg->cpu_address");
        }

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                        (unit, mbcm_dnxf_rx_cpu_address_modid_set,
                         (unit, cfg->num_of_cpu_addresses, cfg->cpu_address)));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int
bcm_dnxf_rx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Calling APIs per init step is not allowed
     * If needed search for advanced init feature
     */
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

STATIC int
dnxf_rx_init_verify(
    int unit)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit))
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_fabric_cell_validate, (unit));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
dnxf_rx_dma_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create buffer for saved cells
     */
    SHR_IF_ERR_EXIT(dnxc_cells_buffer_create(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer,
                                             DNXF_FABRIC_SR_CELLS_MAX_CELLS_IN_BUFFER));

    SHR_IF_ERR_EXIT(dnxc_captured_buffer_create(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer,
                                                DNXF_FABRIC_CAPTURED_CELLS_MAX_CELLS_IN_BUFFER));

    /*
     * Initialize FIFO RX DMA
     * Move all to new step called DMA or to the RX itself
     */
    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit) && !(SAL_BOOT_PLISIM))
    {
        SHR_IF_ERR_EXIT(soc_dnxf_fifo_dma_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnxf_rx_init(
    int unit)
{
    int priority, interval;
    shr_thread_manager_config_t tmngr_cfg;
    _bcm_dnxf_rx_callback_info_t *rx_callback_struct_ptr;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_INVOKE_VERIFY_DNXC(dnxf_rx_init_verify(unit));

    SHR_IF_ERR_EXIT(dnxf_rx_dma_init(unit));

    /*
     * init callback info
     */
    rx_callback_struct_ptr =
        (_bcm_dnxf_rx_callback_info_t *) sal_alloc(sizeof(_bcm_dnxf_rx_callback_info_t), "RX callback struct");
    if (rx_callback_struct_ptr == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "sal_alloc failed");
    }
    rx_callback_struct_ptr->is_valid = 0;
    bcm_dnxf_rx_callback_info_array[unit] = rx_callback_struct_ptr;

    /*
     * set thread flag to false
     */

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * initalize cpu addresses registers
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_rx_cpu_address_modid_init, (unit)));
    }

    /*
     * get thread prio
     */
    priority = dnxf_data_fabric.cell.rx_thread_pri_get(unit);
    interval = dnxf_data_fabric.cell.fifo_dma_enable_get(unit) ? sal_sem_FOREVER : 1000000;

    /*
     * create thread manager
     */
    shr_thread_manager_config_t_init(&tmngr_cfg);
    tmngr_cfg.name = "bcmRX";
    tmngr_cfg.thread_priority = priority;
    tmngr_cfg.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    tmngr_cfg.bsl_module = BSL_LOG_MODULE;
    tmngr_cfg.interval = interval;
    tmngr_cfg.callback = &_bcm_dnxf_rx_pkt_thread;
    tmngr_cfg.start_operation = 0;
    tmngr_cfg.stop_timeout_usec = 3 * SECOND_USEC;
    SHR_IF_ERR_EXIT(shr_thread_manager_create(unit, &tmngr_cfg, &SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
dnxf_rx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (bcm_dnxf_rx_callback_info_array[unit] != NULL)
    {
        sal_free(bcm_dnxf_rx_callback_info_array[unit]);
        bcm_dnxf_rx_callback_info_array[unit] = NULL;
    }

    if (SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr != NULL)
    {
        SHR_IF_ERR_EXIT(shr_thread_manager_destroy(&SOC_DNXF_CONTROL(unit)->rx_reception_tmngr_hndlr));
    }

    SHR_IF_ERR_CONT(dnxc_cells_buffer_destroy(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer));

    SHR_IF_ERR_CONT(dnxc_captured_buffer_destroy(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer));

    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit) && !(SAL_BOOT_PLISIM))
    {
        SHR_IF_ERR_CONT(soc_dnxf_fifo_dma_deinit(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_rx_unregister(
    int unit,
    bcm_rx_cb_f callback,
    uint8 priority)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    bcm_dnxf_rx_callback_info_array[unit]->is_valid = 0;
    bcm_dnxf_rx_callback_info_array[unit]->cb_f = NULL;
    bcm_dnxf_rx_callback_info_array[unit]->cookie = NULL;

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
