/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <bcm_int/dnxf_dispatch.h>
#include <bcm/debug.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/property.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dnxf/rx.h>
#include  <soc/dnxc/dnxc_captured_buffer.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>

#define BCM_DNXF_RX_MAX_PAYLOAD_SIZE_IN_BYTES (256)

typedef struct _bcm_dnxf_rx_callback_info_s{
    bcm_rx_cb_f cb_f;
    void *cookie;
    int is_valid;
} _bcm_dnxf_rx_callback_info_t;

_bcm_dnxf_rx_callback_info_t* bcm_dnxf_rx_callback_info_array[BCM_UNITS_MAX];

int
_bcm_dnxf_rx_captured_cell_get(int unit, dnxc_captured_cell_t* cell)
{
    int rv;
    int is_empty;
    SHR_FUNC_INIT_VARS(unit);

    rv = dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty);
    SHR_IF_ERR_EXIT(rv);

    while (is_empty)
    {
        rv = soc_dnxf_fabric_cell_load(unit);
        if (rv == _SHR_E_EMPTY)
        {
            break;
        }
        SHR_IF_ERR_EXIT(rv);

        rv = dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty);
        SHR_IF_ERR_EXIT(rv);
    }

    if (is_empty)
    {                         
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

    /*Pull cell from data base*/
    rv = dnxc_captured_buffer_get(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, cell);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}



STATIC void
_bcm_dnxf_rx_pkt_thread(int unit)
{
    int _func_rv;
    int _func_unit = unit;
    dnxc_captured_cell_t cell;
    bcm_pkt_t packet;
    bcm_pkt_blk_t packet_data;
    int interval;
    int cell_received;

    SOC_DNXF_CONTROL(unit)->rx_reception_thread_running = 1;
    SOC_DNXF_CONTROL(unit)->rx_reception_thread_closed = 0;

    
    if (!(dnxf_data_fabric.cell.fifo_dma_enable_get(unit)))
    {
        interval = 1000000;
    } 
    else 
    {
        interval = sal_sem_FOREVER;
    } 
 
    while ((SOC_DNXF_CONTROL(unit))->rx_reception_thread_running == 1)
    {

        /* Get a cpu cell */
        _func_rv = _bcm_dnxf_rx_captured_cell_get(unit, &cell);
        if ((_func_rv != _SHR_E_NONE) && (_func_rv != _SHR_E_EMPTY))
        {
            LOG_ERROR(BSL_LS_BCM_RX, (BSL_META_U(unit, "bcm_dnxf_rx_captured_cell_get: failed \n")));
            SOC_DNXF_CONTROL(unit)->rx_reception_thread_running = 0;
            SOC_DNXF_CONTROL(unit)->rx_reception_thread_closed = 1;
            SHR_VOID_FUNC_EXIT;
        }

        cell_received = (_func_rv == _SHR_E_EMPTY) ? 0:1;
        /* build "packet" struct from "captured_cell" struct */

        if (cell_received)
        {
            uint32 byte_data;
            uint32 offset_byte;
            int i;
            uint8 pkt_data[BCM_DNXF_RX_MAX_PAYLOAD_SIZE_IN_BYTES] = {0};

            packet_data.len = cell.cell_size;

            offset_byte = 255;

            for (i=0; i < packet_data.len ; i++)
            {
                SHR_BITCOPY_RANGE(&byte_data, 0 , cell.data, offset_byte*8 , 8);
                pkt_data[i] = byte_data & 0xFF;
                offset_byte --;
            }

            packet_data.data = pkt_data;

            packet.pkt_data = &packet_data;
            packet.unit = unit;
            packet.cos = cell.pipe_index;
            packet.src_mod = cell.source_device;

            /* Initiate the callback function */
            if (bcm_dnxf_rx_callback_info_array[unit]->cb_f != NULL)
            {
                bcm_dnxf_rx_callback_info_array[unit]->cb_f(unit, &packet, bcm_dnxf_rx_callback_info_array[unit]->cookie);
            }
            else
            {
                LOG_ERROR(BSL_LS_BCM_RX, (BSL_META_U(unit, "rx_pkt_thread failed: callback function is NULL \n")));
                SOC_DNXF_CONTROL(unit)->rx_reception_thread_running = 0;
                SOC_DNXF_CONTROL(unit)->rx_reception_thread_closed = 1;
                SHR_VOID_FUNC_EXIT;
            }
            
        }
        else /* No cell received */
        {
            /* Waiting on the semphore */
            sal_sem_take(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore, interval);
        }
    }

    SOC_DNXF_CONTROL(unit)->rx_reception_thread_running = 0;
    SOC_DNXF_CONTROL(unit)->rx_reception_thread_closed = 1;

    sal_thread_exit(0);

}

int
bcm_dnxf_rx_register(int unit, const char *name, bcm_rx_cb_f cb_f, uint8 pri, 
                      void *cookie, uint32 flags)
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
bcm_dnxf_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
    sal_thread_t rx_thread_id;
    int priority;
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

    /* Verify that the thread is not running already */
    if ((SOC_DNXF_CONTROL(unit))->rx_reception_thread_running == 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bcm_rx_start error - thread already running");
    }

    /* assign the buffers with the according mod ids */

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_rx_cpu_address_modid_set, (unit, cfg->num_of_cpu_addresses, cfg->cpu_address)));

    /* get thread prio */

    priority =  dnxf_data_fabric.cell.rx_thread_pri_get(unit);

    /* create thread */
    rx_thread_id = sal_thread_create("bcmRX", SAL_THREAD_STKSZ, priority, (void (*)(void*)) _bcm_dnxf_rx_pkt_thread, INT_TO_PTR(unit)); 

    if (rx_thread_id == NULL) /* thread creation failed */
    {
        return _SHR_E_MEMORY;
    }

    sal_sem_give(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit); 
    SHR_FUNC_EXIT;

}

int 
bcm_dnxf_rx_stop(
    int unit, 
    bcm_rx_cfg_t *cfg)
{
    soc_timeout_t to;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    /* Stop the thread */
    SOC_DNXF_CONTROL(unit)->rx_reception_thread_running = 0;
    sal_sem_give(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore);

    soc_timeout_init(&to, 3000000, 100); /* 3 seconds timeout for closing the thread */

    while (SOC_DNXF_CONTROL(unit)->rx_reception_thread_closed == 0) /* Thread is still running */
    {
        sal_sem_give(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore);
        if (soc_timeout_check(&to)) {
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Rx stop failed: couldn't close rx thread \n");   
        }
    }

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

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_rx_cpu_address_modid_set, (unit, cfg->num_of_cpu_addresses, cfg->cpu_address)));
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

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

int
dnxf_rx_init(
    int unit)
{
    _bcm_dnxf_rx_callback_info_t *rx_callback_struct_ptr;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNXF_UNIT_LOCK_TAKE(unit);

    /*init callback info */

    rx_callback_struct_ptr = (_bcm_dnxf_rx_callback_info_t*)sal_alloc(sizeof(_bcm_dnxf_rx_callback_info_t), "RX callback struct");
    if (rx_callback_struct_ptr == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "sal_alloc failed");
    }
    rx_callback_struct_ptr->is_valid = 0;
    bcm_dnxf_rx_callback_info_array[unit] = rx_callback_struct_ptr;

    /* set thread flag to false */
    SOC_DNXF_CONTROL(unit)->rx_reception_thread_running = 0;
    SOC_DNXF_CONTROL(unit)->rx_reception_thread_closed = 1;

    if (!SOC_WARM_BOOT(unit)) {
        /* initalize cpu addresses registers */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_rx_cpu_address_modid_init, (unit)));
    }

    /* Create a semaphore to be used by the thread */

    SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore = sal_sem_create("rx_pk_thread_wait", sal_sem_BINARY, 0);
    SHR_NULL_CHECK(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore, _SHR_E_PARAM, "SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore");


exit:
    DNXF_UNIT_LOCK_RELEASE(unit); 
    SHR_FUNC_EXIT; 
}

int 
dnxf_rx_deinit(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore != NULL) {
        sal_sem_destroy(SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore);
        SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore = NULL;

        sal_free(bcm_dnxf_rx_callback_info_array[unit]);
    }

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

