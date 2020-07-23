/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE RX
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_RX

#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm/debug.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/property.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dfe/rx.h>
#include  <soc/dcmn/dcmn_captured_buffer.h>

#define BCM_DFE_RX_MAX_PAYLOAD_SIZE_IN_BYTES (256)

typedef struct _bcm_dfe_rx_callback_info_s{
    bcm_rx_cb_f cb_f;
    void *cookie;
    int is_valid;
} _bcm_dfe_rx_callback_info_t;

_bcm_dfe_rx_callback_info_t* bcm_dfe_rx_callback_info_array[BCM_UNITS_MAX];

int
_bcm_dfe_rx_captured_cell_get(int unit, dcmn_captured_cell_t* cell)
{
    int rv;
    int is_empty;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dcmn_captured_buffer_is_empty(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, &is_empty);
    BCMDNX_IF_ERR_EXIT(rv);

    while (is_empty)
    {
        rv = soc_dfe_fabric_cell_load(unit);
        if (rv == BCM_E_EMPTY)
        {
            break;
        }
        BCMDNX_IF_ERR_EXIT(rv);

        rv = dcmn_captured_buffer_is_empty(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, &is_empty);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (is_empty)
    {                         
        BCM_ERR_EXIT_NO_MSG(BCM_E_EMPTY);
    }

    /*Pull cell from data base*/
    rv = dcmn_captured_buffer_get(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, cell);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC void
_bcm_dfe_rx_pkt_thread(int unit)
{
    int _rv;
    dcmn_captured_cell_t cell;
    bcm_pkt_t packet;
    bcm_pkt_blk_t packet_data;
    int interval;
    int cell_received;

    SOC_DFE_CONTROL(unit)->rx_reception_thread_running = 1;
    SOC_DFE_CONTROL(unit)->rx_reception_thread_closed = 0;

    
    if (!SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable)
    {
        interval = 1000000;
    } 
    else 
    {
        interval = sal_sem_FOREVER;
    } 
 
    while ((SOC_DFE_CONTROL(unit))->rx_reception_thread_running == 1)
    {

        /* Get a cpu cell */
        _rv = _bcm_dfe_rx_captured_cell_get(unit, &cell);
        if ((_rv != BCM_E_NONE) && (_rv != BCM_E_EMPTY))
        {
            LOG_ERROR(BSL_LS_BCM_RX, (BSL_META_U(unit, "bcm_dfe_rx_captured_cell_get: failed \n")));
            SOC_DFE_CONTROL(unit)->rx_reception_thread_running = 0;
            SOC_DFE_CONTROL(unit)->rx_reception_thread_closed = 1;
            BCMDNX_FUNC_RETURN_VOID;
        }

        cell_received = (_rv == BCM_E_EMPTY) ? 0:1;
        /* build "packet" struct from "captured_cell" struct */

        if (cell_received)
        {
            uint32 byte_data;
            uint32 offset_byte, bytes_in_first_chunk, bytes_in_second_chunk;
            int i;
            int j=0;
            uint8 pkt_data[BCM_DFE_RX_MAX_PAYLOAD_SIZE_IN_BYTES];
			
            packet_data.len = cell.cell_size + 1;
            bytes_in_first_chunk = packet_data.len <= 128 ? packet_data.len:128;
            bytes_in_second_chunk = packet_data.len <= 128 ? 0:packet_data.len - 128;

            offset_byte = 127; 

            for (i=0; i < bytes_in_first_chunk ; i++)
            {
                SHR_BITCOPY_RANGE(&byte_data, 0 , cell.data, offset_byte*8 , 8);
                pkt_data[j] = byte_data & 0xFF;
                j++;
                offset_byte --;
            }

            offset_byte = 255;

            for (i=0; i < bytes_in_second_chunk; i++)
            {
                SHR_BITCOPY_RANGE(&byte_data, 0, cell.data, offset_byte*8 , 8);
                pkt_data[j] = byte_data & 0xFF;
                j++;
                offset_byte --;
            }


            packet_data.data = pkt_data;

            
            packet.pkt_data = &packet_data;
            packet.unit = unit;
            packet.cos = cell.pipe_index;
            packet.src_mod = cell.source_device;

            /* Initiate the callback function */
            if (bcm_dfe_rx_callback_info_array[unit]->cb_f != NULL)
            {
                bcm_dfe_rx_callback_info_array[unit]->cb_f(unit, &packet, bcm_dfe_rx_callback_info_array[unit]->cookie);
            }
            else
            {
                LOG_ERROR(BSL_LS_BCM_RX, (BSL_META_U(unit, "rx_pkt_thread failed: callback function is NULL \n")));
                SOC_DFE_CONTROL(unit)->rx_reception_thread_running = 0;
                SOC_DFE_CONTROL(unit)->rx_reception_thread_closed = 1;
                BCMDNX_FUNC_RETURN_VOID;
            }
            
        }
        else /* No cell received */
        {
            /* Waiting on the semphore */
            sal_sem_take(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore, interval);
        }
    }

    SOC_DFE_CONTROL(unit)->rx_reception_thread_running = 0;
    SOC_DFE_CONTROL(unit)->rx_reception_thread_closed = 1;

    sal_thread_exit(0);

}

int
bcm_dfe_rx_register(int unit, const char *name, bcm_rx_cb_f cb_f, uint8 pri, 
                      void *cookie, uint32 flags)
{
    BCMDNX_INIT_FUNC_DEFS;
    DFE_UNIT_LOCK_TAKE(unit);

    bcm_dfe_rx_callback_info_array[unit]->is_valid = 1;
    bcm_dfe_rx_callback_info_array[unit]->cb_f = cb_f;
    bcm_dfe_rx_callback_info_array[unit]->cookie = cookie;

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN; 
}

int
bcm_dfe_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
    sal_thread_t rx_thread_id;
    int priority;
    BCMDNX_INIT_FUNC_DEFS;
    DFE_UNIT_LOCK_TAKE(unit);

    if (bcm_dfe_rx_callback_info_array[unit]->is_valid == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_rx_start error - must register a callback function first")));
    }

    /* Verify that the thread is not running already */
    if ((SOC_DFE_CONTROL(unit))->rx_reception_thread_running == 1)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_rx_start error - thread already running")));
    }

    if (cfg->num_of_cpu_addresses != 0)
    {
        BCMDNX_NULL_CHECK(cfg->cpu_address);
    }
    /* assign the buffers with the according mod ids */

    BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_rx_cpu_address_modid_set, (unit, cfg->num_of_cpu_addresses, cfg->cpu_address)));

    /* get thread prio */

    priority =  soc_dfe_property_get(unit, spn_BCM_RX_THREAD_PRI, 1, RX_THREAD_PRI_DFLT);
                                                          
    /* create thread */
    rx_thread_id = sal_thread_create("bcmRX", SAL_THREAD_STKSZ, priority, (void (*)(void*)) _bcm_dfe_rx_pkt_thread, INT_TO_PTR(unit)); 

    if (rx_thread_id == NULL) /* thread creation failed */
    {
        return BCM_E_MEMORY;
    }

    sal_sem_give(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore);

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;

}

int 
bcm_dfe_rx_stop(
    int unit, 
    bcm_rx_cfg_t *cfg)
{
    soc_timeout_t to;

    BCMDNX_INIT_FUNC_DEFS;
    DFE_UNIT_LOCK_TAKE(unit);

    
    if (cfg == NULL)
    {
        BCM_EXIT;

    }

    /* Stop the thread */
    SOC_DFE_CONTROL(unit)->rx_reception_thread_running = 0;
    sal_sem_give(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore);

    soc_timeout_init(&to, 3000000, 100); /* 3 seconds timeout for closing the thread */

    while (SOC_DFE_CONTROL(unit)->rx_reception_thread_closed == 0) /* Thread is still running */
    {
        sal_sem_give(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore);
        if (soc_timeout_check(&to)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_TIMEOUT, (_BSL_BCM_MSG("Rx stop failed: couldn't close rx thread \n")));   
        }
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_rx_cpu_address_modid_set, (unit, cfg->num_of_cpu_addresses, cfg->cpu_address)));

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;

}

int 
bcm_dfe_rx_init(
    int unit)
{
    _bcm_dfe_rx_callback_info_t *rx_callback_struct_ptr;
    
    BCMDNX_INIT_FUNC_DEFS;
    DFE_UNIT_LOCK_TAKE(unit);

    /*init callback info */

    rx_callback_struct_ptr = (_bcm_dfe_rx_callback_info_t*)sal_alloc(sizeof(_bcm_dfe_rx_callback_info_t), "RX callback struct");
    if (rx_callback_struct_ptr == NULL)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("sal_alloc failed")));
    }
    rx_callback_struct_ptr->is_valid = 0;
    bcm_dfe_rx_callback_info_array[unit] = rx_callback_struct_ptr;

    /* set thread flag to false */
    SOC_DFE_CONTROL(unit)->rx_reception_thread_running = 0;
    SOC_DFE_CONTROL(unit)->rx_reception_thread_closed = 1;

    if (!SOC_WARM_BOOT(unit)) {
        /* initalize cpu addresses registers */
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_rx_cpu_address_modid_init, (unit)));
    }

    /* Create a semaphore to be used by the thread */

    SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore = sal_sem_create("rx_pk_thread_wait", sal_sem_BINARY, 0);
    BCMDNX_NULL_CHECK(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore);


exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN; 
}

int 
_bcm_dfe_rx_deinit(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    sal_sem_destroy(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore);
    SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore = NULL;

    sal_free(bcm_dfe_rx_callback_info_array[unit]);

    BCMDNX_FUNC_RETURN;
}

int 
bcm_dfe_rx_unregister(
    int unit, 
    bcm_rx_cb_f callback, 
    uint8 priority)
{
    BCMDNX_INIT_FUNC_DEFS;
    DFE_UNIT_LOCK_TAKE(unit);

    bcm_dfe_rx_callback_info_array[unit]->is_valid = 0;
    bcm_dfe_rx_callback_info_array[unit]->cb_f = NULL;
    bcm_dfe_rx_callback_info_array[unit]->cookie = NULL;

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN; 
}


#undef _ERR_MSG_MODULE_NAME

