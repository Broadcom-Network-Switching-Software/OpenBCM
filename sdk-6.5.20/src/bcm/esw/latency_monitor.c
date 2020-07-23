/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/mem.h>
#include <soc/register.h>
#include <soc/debug.h>
#include <soc/dma.h>
#include <soc/schanmsg_internal.h>
#include <soc/sbusdma_internal.h>

#include <bcm/error.h>
#include <bcm_int/esw/latency_monitor.h>
#include <soc/drv.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/common/debug.h>
#include <include/soc/tomahawk3.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int inside_bcm_shutdown[BCM_MAX_NUM_UNITS];

static latency_monitor_driver_data_t latency_monitor_driver_data;
static latency_monitor_mem_t latency_monitor_mem[BCM_MAX_NUM_UNITS][LATENCY_MONITOR_MAX_COUNT];
int bcm_tomahawk3_latency_monitor_enable(int unit, uint8 monitor_id, uint8 enable);
int bcm_tomahawk3_latency_monitor_destroy(int, uint8 monitor_id);

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
STATIC int _bcm_th3_latency_monitor_scache_alloc(int unit, int create);
STATIC int _bcm_th3_latency_monitor_wb_recover(int unit);
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

uint8   wb_post_init;

/**
 * Initialize the linked list (fifo) to empty value.
 * @param desc_handle_list
 */
void
_sbusdma_desc_handle_list_init(_sbusdma_desc_handle_list_t * desc_handle_list)
{
   desc_handle_list->head = desc_handle_list->tail = NULL;
}

/**
 * Push an entry to the end of the linked list
 *
 * @param desc_handle_list -linked list on which entry is added
 * @param handle entry - to get pushed.
 *
 * @return int BCM_E_MEMORY if memory allocation fails,
 *         BCM_E_NONE otherwise.
 */
int
_sbusdma_desc_handle_list_push(_sbusdma_desc_handle_list_t *desc_handle_list, const _sbusdma_desc_handle_data_t *data)
{
    _sbusdma_desc_handle_node_t *new_node = sal_alloc(sizeof(_sbusdma_desc_handle_node_t), "sbus dma fifo");
    if (new_node == NULL) {
        return  BCM_E_MEMORY;
    }
    new_node->next = NULL;
    new_node->data = (_sbusdma_desc_handle_data_t *)data;
    if (desc_handle_list->head != NULL) {
       desc_handle_list->tail->next = new_node;
       desc_handle_list->tail = new_node;
    } else {
       desc_handle_list->head = desc_handle_list->tail = new_node;
    }
    return BCM_E_NONE;
}


/**
 * Pop an event from the front of the linked list (fifo)
 *
 * @param desc_handle_list - linked list where event is pushed.
 * @param handle - event is set here.
 *
 * @return int BCM_E_EMPTY if linked list is empty, BCM_E_NONE
 *         otherwise.
 */
int
_sbusdma_desc_handle_list_pop(_sbusdma_desc_handle_list_t *desc_handle_list, _sbusdma_desc_handle_data_t **data)
{
   if (desc_handle_list->head) {
      _sbusdma_desc_handle_node_t *temp_head = desc_handle_list->head;
      *data = desc_handle_list->head->data;
      if (desc_handle_list->head == desc_handle_list->tail) {
          desc_handle_list->head = desc_handle_list->tail = NULL;
      } else {
          desc_handle_list->head = desc_handle_list->head->next;
      }
      sal_free(temp_head);
      return BCM_E_NONE;
   } else {
      return BCM_E_EMPTY;
   }
}


int 
_sbusdma_desc_handle_list_is_empty(_sbusdma_desc_handle_list_t *desc_handle_list)
{
    if (!desc_handle_list->head) {
        return TRUE;
    }

    return FALSE;
}


#ifdef BCM_TOMAHAWK3_SUPPORT
static int
latency_monitor_driver_data_init(int unit)
{
    int i;

    if (!latency_monitor_driver_data.init_done) {
        latency_monitor_driver_data.desc_mode     = 1;
        latency_monitor_driver_data.dma_wr_en     = TRUE;
        latency_monitor_driver_data.dma_rd_en     = TRUE;
        latency_monitor_driver_data.num_mem       = 1;
        latency_monitor_driver_data.init_done     = TRUE;

        for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
            SOC_LATENCY_MONITOR_INFO(unit, i).time_step = -1;
            SOC_LATENCY_MONITOR_INFO(unit, i).flex_pool_number = -1;
            BCM_PBMP_CLEAR(SOC_LATENCY_MONITOR_INFO(unit, i).src_pbmp);
            BCM_PBMP_CLEAR(SOC_LATENCY_MONITOR_INFO(unit, i).dest_pbmp);
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

static void
latency_monitor_sbusdma_cb(int unit, int status, sbusdma_desc_handle_t handle,
                   void *data)
{
    int rv;
    int monitor_id;
    cb_data_t   *p_cb_data;
    uint32      *entbuf;
    uint8        frag_count;
    _sbusdma_desc_handle_data_t     *list_data;

    if (status != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_LATENCY_MONITOR,
                    (BSL_META_U(unit, "SBUS DMA failed: type %d\n"),
                   PTR_TO_INT(data)));
        if (status == BCM_E_TIMEOUT)
            (void)soc_sbusdma_desc_delete(unit, handle);
    }

    if (data == NULL) {
        return;
    }

    p_cb_data = (cb_data_t *)data;
    monitor_id = p_cb_data->monitor_id;

    LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                (BSL_META_U(unit, "latency_monitor_sbusdma_cb:: monitor_id:%d \n"),
                             monitor_id));

    if (p_cb_data->write_op) {
        BCM_EXIT;
    }

    frag_count = latency_monitor_mem[unit][monitor_id].frag_count;
    entbuf = sal_alloc(WORDS2BYTES(latency_monitor_mem[unit][monitor_id].bufwords/frag_count), "CB dma fifo");
    if (entbuf) {
        /* Since the DMA able memory is scarce, copy and free it at the earliest possible. */
        sal_memcpy(entbuf, p_cb_data->data, WORDS2BYTES((latency_monitor_mem[unit][monitor_id].bufwords)/frag_count));
        if (p_cb_data->data) {
            soc_cm_sfree(unit, p_cb_data->data);
        }
    } else {
        latency_monitor_driver_data.run[monitor_id] = FALSE;
        BCM_EXIT;
    }

    list_data = sal_alloc(sizeof(_sbusdma_desc_handle_data_t), "DMA handler list data");
    if (list_data) {
        list_data->dbuf = entbuf;
        list_data->pipe = p_cb_data->pipe_num;
    }

    if (latency_monitor_driver_data.run[monitor_id] == TRUE) {
        sal_sem_take(latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock, sal_sem_FOREVER);
        _sbusdma_desc_handle_list_push(&latency_monitor_mem[unit][monitor_id].handle_list, list_data);
        sal_sem_give(latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem);
        sal_sem_give(latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock);
    } else {
        sal_free(entbuf);
        sal_free(list_data);
    }

    LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                (BSL_META_U(unit,"Call back completed for handle:%d Monitor:%d data:%p\n"),
                             handle, monitor_id, p_cb_data->data));
exit:
    rv = soc_sbusdma_desc_delete(unit, handle);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_LATENCY_MONITOR,
                    (BSL_META_U(unit,
                                "Failed to delete sbusdma descriptor Monitor:%d\n"),
                                    monitor_id));
    } else {
        LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit,
                                "Deleting handle %d Monitor:%d\n"),
                                 handle, monitor_id));
    }
    
    if (latency_monitor_driver_data.sbusdma_run_lock) {
        sal_sem_give(latency_monitor_driver_data.sbusdma_run_lock);
    }
    sal_free(p_cb_data);
    /*    coverity[leaked_storage : FALSE]    */
}


static int
latency_monitor_sbusdma_setup(int unit, int monitor_id, int pipe, uint32 *entbuf, int index_min,
                      int index_max, int frag, int write, sbusdma_desc_handle_t *handle_to_run)
{
    /* for soc_sbusdma_desc_create and soc_sbusdma_desc_run */
    soc_sbusdma_desc_ctrl_t ctrl;
    soc_sbusdma_desc_cfg_t cfg;
    soc_sbusdma_desc_cfg_t *cfg_array;
    int rv = BCM_E_NONE;
    int         i;
    uint8       at;
    cb_data_t   *p_cb_data;

    if (latency_monitor_driver_data.desc_mode > 0) {
        /* descriptor based sbus dma mode through sdk api */
        sal_memset(&ctrl, 0, sizeof(soc_sbusdma_desc_ctrl_t));
        sal_memset(&cfg, 0, sizeof(soc_sbusdma_desc_cfg_t));
        cfg_array = soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_cfg_t) *
                                  latency_monitor_driver_data.desc_mode, "sbusdma_desc_cfg");
        ctrl.cfg_count = latency_monitor_driver_data.desc_mode;
        ctrl.cb = latency_monitor_sbusdma_cb;
        p_cb_data = sal_alloc(sizeof(cb_data_t), "CB Data");
        if (p_cb_data == NULL) {
            return  BCM_E_MEMORY;
        }
        p_cb_data->monitor_id = monitor_id;
        p_cb_data->data = entbuf;
        if (write == 0) {
            ctrl.flags = SOC_SBUSDMA_MEMORY_CMD_MSG;
        } else {
            ctrl.flags = SOC_SBUSDMA_WRITE_CMD_MSG | SOC_SBUSDMA_MEMORY_CMD_MSG;
        }
        p_cb_data->write_op = write;
        p_cb_data->pipe_num = pipe;
        ctrl.data = p_cb_data;

        cfg.acc_type = SOC_MEM_ACC_TYPE(unit, latency_monitor_mem[unit][monitor_id].mem+pipe);
        cfg.blk = SOC_BLOCK2SCH(unit, latency_monitor_mem[unit][monitor_id].copyno);
        cfg.addr = soc_mem_addr_get(unit, latency_monitor_mem[unit][monitor_id].mem+pipe, 0,
                                    latency_monitor_mem[unit][monitor_id].copyno, index_min, &at);
        cfg.width = latency_monitor_mem[unit][monitor_id].entwords;
        cfg.count = index_max - index_min + 1;
        cfg.buff = entbuf;
        cfg.addr_shift = 0;
        for (i = 0; i < latency_monitor_driver_data.desc_mode; i++) {
            sal_memcpy(cfg_array + i, &cfg, sizeof(soc_sbusdma_desc_cfg_t));
        }

        if (SOC_FAILURE((rv = (soc_sbusdma_desc_create(unit, &ctrl, cfg_array,
                                                       TRUE, handle_to_run))))) {
            _SHR_ERROR_TRACE(rv);
        }

        LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit,"Create handle %d Monitor:%d\n"),
                                *handle_to_run, monitor_id));

        soc_cm_sfree(unit, cfg_array);
    } 

    return rv;
}

void
counter_accrue_thread(void* up)
{
    int             unit = PTR_TO_INT(up) & 0xff;
    int             monitor_id;
    uint32          packet_count=0;
    uint64          byte_count;
    soc_memacc_t    memacc_pkt_x;
    soc_memacc_t    memacc_byte_x;
    egr_flex_ctr_counter_table_0_entry_t   *data_ptr;
    uint64          (*arr_pkt_cnt)[LATENCY_MONITOR_BUCKET_COUNT];
    uint64          (*arr_byte_cnt)[LATENCY_MONITOR_BUCKET_COUNT];
    int             l, j, k;
    int             data_buffer_idx = 0;
    uint32          *dbuf = NULL;
    int             frag_count;
    uint64          *sw_buff = NULL;
    uint32          sw_buf_idx = 0;
    _sbusdma_desc_handle_data_t *list_data;
    int             pipe = 0;
    int             pipe_lower = -1;
    uint64          byte_count_tmp;
    uint32          packet_count_tmp;
    uint8           pkt_ctr_f_width;
    uint64          tmp64;
    uint64          mask64;
    bcm_latency_monitor_cb_info_t cb_info;

    monitor_id = PTR_TO_INT(up) >> 8;
    if (((soc_memacc_init(unit, latency_monitor_mem[unit][monitor_id].mem, PACKET_COUNTERf, &memacc_pkt_x))) ||
        ((soc_memacc_init(unit, latency_monitor_mem[unit][monitor_id].mem, BYTE_COUNTERf, &memacc_byte_x)))) {
                  return;
    }

    frag_count = latency_monitor_mem[unit][monitor_id].frag_count;
    pkt_ctr_f_width = soc_mem_field_length(unit,latency_monitor_mem[unit][monitor_id].mem, PACKET_COUNTERf);

    while (TRUE) {
        if (sal_sem_take(latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem, sal_sem_FOREVER) == 0) {
            if ((latency_monitor_driver_data.run[monitor_id] == FALSE) &&
                (_sbusdma_desc_handle_list_is_empty(&latency_monitor_mem[unit][monitor_id].handle_list))) {
                LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit, "Exiting the counter_accrue_thread monitor_id:%d \n"), monitor_id));
                BCM_EXIT;
            }
            sal_sem_take(latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock, sal_sem_FOREVER);
            _sbusdma_desc_handle_list_pop(&latency_monitor_mem[unit][monitor_id].handle_list, &list_data);
            sal_sem_give(latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock);
 
            if (list_data) {
                dbuf = list_data->dbuf;
                pipe = list_data->pipe;
                sal_free(list_data);
            } else {
                BCM_EXIT;
            }

            if (latency_monitor_mem[unit][monitor_id].pipe_map_temp == 0) {
                latency_monitor_mem[unit][monitor_id].pipe_map_temp = latency_monitor_mem[unit][monitor_id].dest_pbmp_pipe_map;
                pipe_lower = pipe;
                /* Acquire lock for the first pipe instance of pbmp. */
                sal_sem_take(latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem, sal_sem_FOREVER);
            } else {
                pipe_lower = -1;
            }

            latency_monitor_mem[unit][monitor_id].pipe_map_temp &= ~(1 << pipe);

            if (dbuf) {
                arr_pkt_cnt = (uint64 (*)[LATENCY_MONITOR_BUCKET_COUNT])latency_monitor_mem[unit][monitor_id].histogram_packet64_counter;
                arr_byte_cnt = (uint64 (*)[LATENCY_MONITOR_BUCKET_COUNT])latency_monitor_mem[unit][monitor_id].histogram_byte64_counter;
                data_ptr = (egr_flex_ctr_counter_table_0_entry_t *)dbuf;
                data_buffer_idx = 0;
                for (l=0; l<(TIME_STEP_SET_COUNT/frag_count); l++) {
                    if (latency_monitor_mem[unit][monitor_id].monitor_mode == bcmLatencyMonitorTimeSeriesMode) {
                        sw_buff = (uint64 *)latency_monitor_mem[unit][monitor_id].sw_buf_addr;
                    }
                    for (j=0; j<LATENCY_MONITOR_QUEUE_COUNT; j++) {
                        for (k=0; k<LATENCY_MONITOR_BUCKET_COUNT; k++) {
                            soc_memacc_field64_get(&memacc_byte_x,
                                    (uint64 *)&data_ptr[data_buffer_idx], &byte_count);
                            packet_count = soc_memacc_field32_get(&memacc_pkt_x,
                                    (uint32 *)&data_ptr[data_buffer_idx]);
                            COMPILER_64_ADD_32(arr_pkt_cnt[j][k], packet_count);
                            COMPILER_64_ADD_64(arr_byte_cnt[j][k], byte_count);
                            data_buffer_idx++;
                            if (latency_monitor_mem[unit][monitor_id].monitor_mode == bcmLatencyMonitorTimeSeriesMode) {
                                sw_buf_idx = latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx;

                                packet_count_tmp = 0;
                                COMPILER_64_ZERO(byte_count_tmp);
                                if (pipe_lower != -1) {
                                    /* Overwrite counter for the the pipe corresponding to first port in dbmp */
                                    COMPILER_64_COPY(sw_buff[sw_buf_idx], byte_count);
                                } else {
                                    COMPILER_64_COPY(tmp64, sw_buff[sw_buf_idx]);
                                    COMPILER_64_SHR(tmp64, pkt_ctr_f_width);
                                    COMPILER_64_COPY(byte_count_tmp, tmp64);
                                    COMPILER_64_COPY(tmp64, sw_buff[sw_buf_idx]);
                                    COMPILER_64_SET(mask64, 0, (uint32)((1 << pkt_ctr_f_width) - 1));
                                    COMPILER_64_AND(tmp64, mask64);
                                    packet_count_tmp =  COMPILER_64_LO(tmp64);
                                    /* Accumulate counter for the remaining pipe corresponding to port in dbmp */
                                    COMPILER_64_ADD_64(byte_count_tmp, byte_count);
                                    COMPILER_64_COPY(sw_buff[sw_buf_idx], byte_count_tmp);
                                }
                                packet_count_tmp += packet_count;
                                COMPILER_64_SHL(sw_buff[sw_buf_idx], pkt_ctr_f_width);
                                COMPILER_64_ADD_32(sw_buff[sw_buf_idx], packet_count_tmp);
                            
                                latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx++;
                                latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx %= latency_monitor_mem[unit][monitor_id].sw_buf_size;
                            }
                        }
                    }
                }

                LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit, "Freed buffer for dbuf:%p  Monitor:%d\n"), dbuf, monitor_id));
                sal_free(dbuf);

                /* Call the callback handler after accumulation for all the pipes */
                if ((latency_monitor_mem[unit][monitor_id].monitor_mode == bcmLatencyMonitorTimeSeriesMode) &&
                    (latency_monitor_mem[unit][monitor_id].pipe_map_temp == 0) &&
                    (latency_monitor_mem[unit][monitor_id].cb_fn)) {
                    cb_info.monitor_id = monitor_id;
                    cb_info.monitor_status = latency_monitor_mem[unit][monitor_id].status;
                    cb_info.sw_buf_addr = sw_buff;
                    cb_info.sw_buf_size = latency_monitor_mem[unit][monitor_id].sw_buf_size;
                    cb_info.sw_buf_wr_idx = sw_buf_idx;

                    latency_monitor_mem[unit][monitor_id].cb_fn(unit, &cb_info, latency_monitor_mem[unit][monitor_id].user_data);
                }

                /* Counter accumulation pending for other pipes. Back track the sw_buf_wr_idx index in S/W Ring buffer */
                if ((latency_monitor_mem[unit][monitor_id].monitor_mode == bcmLatencyMonitorTimeSeriesMode) &&
                    (latency_monitor_mem[unit][monitor_id].pipe_map_temp != 0)) {
                    if (latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx == 0) {
                        latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx = latency_monitor_mem[unit][monitor_id].sw_buf_size;
                    } 

                    if (latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx) {
                        latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx -=
                                 (LATENCY_MONITOR_QUEUE_COUNT * LATENCY_MONITOR_BUCKET_COUNT)*(TIME_STEP_SET_COUNT/frag_count);
                    }
                }
            }

            /* Release the lock after accumulation for all the pipe instances */
            if (latency_monitor_mem[unit][monitor_id].pipe_map_temp == 0) {
                sal_sem_give(latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem);
            }
        }
    }

exit:
    if (latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem) {
        sal_sem_destroy(latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem);
        latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem = NULL;
    }

    if (latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock) {
        sal_sem_destroy(latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock);
        latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock = NULL;
    }

    latency_monitor_mem[unit][monitor_id].init_done = FALSE;

    if (latency_monitor_mem[unit][monitor_id].status == LATENCY_MONITOR_STATE_RUNNING) {
        latency_monitor_mem[unit][monitor_id].status = LATENCY_MONITOR_STATE_IDLE;
    }

    sal_thread_exit(0);

    return;
}

static int
_get_curr_histogram_set(int unit, int monitor_id)
{
    soc_reg_t       histogram_set_reg;
    uint32          reg_value;
    int             rv = SOC_E_NONE;
    int             histogram_set;

    switch (monitor_id) {
    case MONITOR_ID_0:
        histogram_set_reg       = EGR_HISTO_MON_0_HISTOGRAM_SETr;
        break;
    case MONITOR_ID_1:
        histogram_set_reg       = EGR_HISTO_MON_1_HISTOGRAM_SETr;
        break;
    case MONITOR_ID_2:
        histogram_set_reg       = EGR_HISTO_MON_2_HISTOGRAM_SETr;
        break;
    case MONITOR_ID_3:
        histogram_set_reg       = EGR_HISTO_MON_3_HISTOGRAM_SETr;
        break;
    default:
        return SOC_E_PARAM;
    }

    rv = soc_reg32_get(unit, histogram_set_reg, REG_PORT_ANY, 0, &reg_value);
    SOC_IF_ERROR_RETURN(rv);
    histogram_set = soc_reg_field_get(unit,histogram_set_reg,reg_value, HISTOGRAM_SETf);

    return histogram_set;
}

static int
_check_for_wraparound_condition(int unit, int monitor_id, int frag)
{
    soc_reg_t       histogram_set_reg;
    soc_reg_t       flex_ctr_wrap_ctr_reg;
    int             histogram_set;
    uint32          flex_ctr_wrap_ctr_old;
    uint32          flex_ctr_wrap_ctr_new;
    uint32          reg_value;
    int             rv = SOC_E_NONE;
    int             frag_count;
    uint8           frag_set_idx;

    switch (monitor_id) {
    case MONITOR_ID_0:
        histogram_set_reg       = EGR_HISTO_MON_0_HISTOGRAM_SETr;
        flex_ctr_wrap_ctr_reg   = EGR_HISTO_MON_0_FLEX_COUNTER_WRAP_COUNTERr;
        break;
    case MONITOR_ID_1:
        histogram_set_reg       = EGR_HISTO_MON_1_HISTOGRAM_SETr;
        flex_ctr_wrap_ctr_reg   = EGR_HISTO_MON_1_FLEX_COUNTER_WRAP_COUNTERr;
        break;
    case MONITOR_ID_2:
        histogram_set_reg       = EGR_HISTO_MON_2_HISTOGRAM_SETr;
        flex_ctr_wrap_ctr_reg   = EGR_HISTO_MON_2_FLEX_COUNTER_WRAP_COUNTERr;
        break;
    case MONITOR_ID_3:
        histogram_set_reg       = EGR_HISTO_MON_3_HISTOGRAM_SETr;
        flex_ctr_wrap_ctr_reg   = EGR_HISTO_MON_3_FLEX_COUNTER_WRAP_COUNTERr;
        break;
    default:
        return BCM_E_PARAM;
    }

    frag_count = latency_monitor_mem[unit][monitor_id].frag_count;

    while (latency_monitor_driver_data.run[monitor_id]) {
        rv = soc_reg32_get(unit, histogram_set_reg, REG_PORT_ANY, 0, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        histogram_set = soc_reg_field_get(unit,histogram_set_reg,reg_value, HISTOGRAM_SETf);

        rv = soc_reg32_get(unit, flex_ctr_wrap_ctr_reg, REG_PORT_ANY, 0, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        flex_ctr_wrap_ctr_new = soc_reg_field_get(unit,flex_ctr_wrap_ctr_reg,reg_value, FLEX_COUNTER_WRAP_COUNTf);

        flex_ctr_wrap_ctr_old = latency_monitor_mem[unit][monitor_id].flex_ctr_wrap_ctr;
        /* On The H/W reg field FLEX_COUNTER_WRAP_COUNT wrap, reset the counters */ 
        if ((flex_ctr_wrap_ctr_old == 0xFFFFFFFF) && (flex_ctr_wrap_ctr_new == 0)) {
            flex_ctr_wrap_ctr_old = 0;
        }
        latency_monitor_mem[unit][monitor_id].flex_ctr_wrap_ctr = flex_ctr_wrap_ctr_new;

        if ((flex_ctr_wrap_ctr_new - flex_ctr_wrap_ctr_old) > 1 ) {
            /* Error: Data Overwrite Occurred */
            return BCM_E_FAIL;
        }
        if (frag == 0) {    /* Check for 1st Fragment */
            /* Checking to schedule DMA for 1st Half of the flex counter block */
            if (histogram_set >= (TIME_STEP_SET_COUNT/frag_count)) {
                /*i.e the hardware is upgrading the 2nd half of the allocated flex pool */
                if ((flex_ctr_wrap_ctr_new - flex_ctr_wrap_ctr_old) > 0) {
                    /* Error: Data Overwrite Occurred */
                    return BCM_E_FAIL;
                } else {
                    frag_set_idx = histogram_set - (TIME_STEP_SET_COUNT/frag_count);
                    latency_monitor_mem[unit][monitor_id].dma_schedule_period = 
                        ((TIME_STEP_SET_COUNT/frag_count) - frag_set_idx) * latency_monitor_mem[unit][monitor_id].time_step;
                    /* We are good */
                    return BCM_E_NONE;
                }
            } else {
                if (latency_monitor_driver_data.run[monitor_id] == FALSE) {
                    return BCM_E_NONE;
                }
            }
        } else if (frag == 1) {   /* Check for 2nd Fragment */ 
            /* Checking to schedule DMA for 2nd Half of the flex counter block */
            if (histogram_set < (TIME_STEP_SET_COUNT/frag_count)) {
                /*i.e the hardware is upgrading the 1st half of the allocated flex pool */
                if ((flex_ctr_wrap_ctr_new - flex_ctr_wrap_ctr_old) > 1) {
                    /* Error: Data Overwrite Occurred */
                    return BCM_E_FAIL;
                } else {
                    latency_monitor_mem[unit][monitor_id].dma_schedule_period = 
                        ((TIME_STEP_SET_COUNT/frag_count) - histogram_set) * latency_monitor_mem[unit][monitor_id].time_step;
                    /* We are good */
                    return BCM_E_NONE;
                }
            } else {
                if (latency_monitor_driver_data.run[monitor_id] == FALSE) {
                    return BCM_E_NONE;
                }
            }
        } else {
            /* We never enter here. Error condition. */
            return BCM_E_FAIL;
        }
    };
    
    return BCM_E_NONE;
}

static int
sbusdma_handler_run (int unit, sbusdma_desc_handle_t handle)
{
    soc_timeout_t   to;
    sal_usecs_t     dma_timeout = 1000000;
    int             ret;

    soc_timeout_init(&to, 2 * dma_timeout, 0);
    do {
        ret = soc_sbusdma_desc_run(unit, handle);
        if ((ret == BCM_E_BUSY) || (ret == BCM_E_INIT)) {
            if (ret == BCM_E_INIT) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_BCM_LATENCY_MONITOR,
                         (BSL_META_U(unit,
                                     "sbusdma desc run operation timeout\n")));
                return BCM_E_TIMEOUT;
            }
            sal_usleep(10);
        }
    } while ((ret == BCM_E_BUSY) || (ret == BCM_E_INIT));

    return BCM_E_NONE;
}

static void
sbusdma_read(void* up)
{
    int         unit = PTR_TO_INT(up) & 0xff;
    uint32      *entbuf;
    uint32      *cleanup_buf;
    int         frag;
    int         index_min, index_max;
    char        *access_type;
    int         cnt;
    int         monitor_id;
    sal_usecs_t dma_timeout = 1000000;
    sbusdma_desc_handle_t handle_to_run = 0;
    uint64      rval, val;
    int         frag_count;
    uint8       frag_set_idx;
    uint8       histogram_set;
    uint8       pipe;
    int         rv = BCM_E_NONE;
    bcm_latency_monitor_cb_info_t cb_info;

    if (latency_monitor_driver_data.dma_rd_en) {
        access_type = sal_strdup("SBUS DMA");
    } else {
        access_type = sal_strdup("SBUS    ");
    }

    monitor_id = PTR_TO_INT(up) >> 8;
    frag_count = latency_monitor_mem[unit][monitor_id].frag_count;

    latency_monitor_mem[unit][monitor_id].sbusdma_read_thread_pid = sal_thread_id_get();

    while (latency_monitor_driver_data.run[monitor_id] == FALSE) {
        sal_usleep(10);
    }

    cnt = WORDS2BYTES((latency_monitor_mem[unit][monitor_id].bufwords)/frag_count);
    cleanup_buf = soc_cm_salloc(unit, cnt, "latency monitor cleanup buffer");
    if (!cleanup_buf) {
        BCM_EXIT;
    }
    sal_memset(cleanup_buf, 0x0, cnt);

    histogram_set = _get_curr_histogram_set(unit, monitor_id);
    if (histogram_set >= (TIME_STEP_SET_COUNT/frag_count)) {
        frag = 1;       /* Start from second Half of the flex counter pool block */
        frag_set_idx = histogram_set - (TIME_STEP_SET_COUNT/frag_count);
        latency_monitor_mem[unit][monitor_id].dma_schedule_period = ((TIME_STEP_SET_COUNT/frag_count) - frag_set_idx) *
                                                    latency_monitor_mem[unit][monitor_id].time_step;
    } else {
        frag = 0;       /* Start from first Half of the flex counter pool block */
        latency_monitor_mem[unit][monitor_id].dma_schedule_period = ((TIME_STEP_SET_COUNT/frag_count) - histogram_set) *
                                                    latency_monitor_mem[unit][monitor_id].time_step;
    }

    while (latency_monitor_driver_data.run[monitor_id]) {
        frag %= latency_monitor_mem[unit][monitor_id].frag_count;
        for (; frag < latency_monitor_mem[unit][monitor_id].frag_count; frag++) {
            index_min = latency_monitor_mem[unit][monitor_id].frag_index_min[frag];
            index_max = latency_monitor_mem[unit][monitor_id].frag_index_max[frag];

            /* 
             * The Binary Semaphore time out here is used for periodic scheduling of SBUS DMA.
             * The semaphore is also used to signal that the monitor is disabled and quick termination of the thread.
             */
            sal_sem_take(latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger, 
                            latency_monitor_mem[unit][monitor_id].dma_schedule_period);
            if (latency_monitor_driver_data.run[monitor_id] == FALSE) {
                BCM_EXIT;
            }
            if (_check_for_wraparound_condition(unit, monitor_id, frag) != SOC_E_NONE) {
                /* Disable Monitor */
                bcm_tomahawk3_latency_monitor_enable(unit, monitor_id, 0);
                latency_monitor_mem[unit][monitor_id].status = LATENCY_MONITOR_STATE_ERROR;
                if (latency_monitor_mem[unit][monitor_id].cb_fn) {
                    cb_info.monitor_id = monitor_id;
                    cb_info.monitor_status = LATENCY_MONITOR_STATE_ERROR;
                    cb_info.sw_buf_addr = NULL;
                    cb_info.sw_buf_size = 0 ;
                    cb_info.sw_buf_wr_idx = 0;

                    latency_monitor_mem[unit][monitor_id].cb_fn(unit, &cb_info, latency_monitor_mem[unit][monitor_id].user_data);
                }
                BCM_EXIT;
            }

            entbuf = soc_cm_salloc(unit, cnt,
                            "latency_monitor_driver_data source");
            if (!entbuf) {
                latency_monitor_driver_data.run[monitor_id] = FALSE;
                BCM_EXIT;
            }
            sal_memset(entbuf, 0x0, cnt);

            LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit, "Allocated %d bytes addr:%p Monitor:%d\n"),
                                cnt, entbuf, monitor_id));
            LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit, "New DMA Satrted index_min:%d index_max;%d Monitor:%d\n"),
                                 index_min, index_max, monitor_id));

            LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                      (BSL_META_U(unit, "%s reading from %s[%0d..%0d] to address 0x%08x\n"),
                      access_type, SOC_MEM_NAME(unit, latency_monitor_mem[unit][monitor_id].mem),
                      index_max,index_min, PTR_TO_INT(entbuf)));

            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                if (!(latency_monitor_mem[unit][monitor_id].dest_pbmp_pipe_map & (1 << pipe))) {
                    continue;
                }
                sal_sem_take(latency_monitor_driver_data.sbusdma_run_lock, sal_sem_FOREVER);

                if (latency_monitor_driver_data.dma_rd_en) {
                    /* Setup DMA handler for read operation */
                    latency_monitor_sbusdma_setup(unit, monitor_id, pipe, entbuf, index_min,
                                          index_max, frag, 0, &handle_to_run);
                }

                LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                         (BSL_META_U(unit, "handle:%d Monitor:%d \n"), handle_to_run, monitor_id));

                if (latency_monitor_driver_data.dma_rd_en && latency_monitor_driver_data.desc_mode > 0) {
                    soc_timeout_t to;
                    soc_timeout_init(&to, 2 * dma_timeout, 0);
                    /* Execute SBUS DMA command to read the flex pool block */
                    sbusdma_handler_run(unit, handle_to_run);
                    LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                                (BSL_META_U(unit, "DMA RUN completed Monitor:%d\n"), monitor_id));
                }

            }
        }
    }

exit:
    if (latency_monitor_driver_data.monitors_in_use_count) {
        latency_monitor_driver_data.monitors_in_use_count--;
        if (latency_monitor_driver_data.monitors_in_use_count == 0) {
            if (latency_monitor_driver_data.sbusdma_run_lock) {
                /* Wait for other thread to release the semaphore before deleting it */
                sal_sem_take(latency_monitor_driver_data.sbusdma_run_lock, sal_sem_FOREVER);
                sal_sem_destroy(latency_monitor_driver_data.sbusdma_run_lock);
                latency_monitor_driver_data.sbusdma_run_lock = NULL;
            }
            latency_monitor_driver_data.init_done = FALSE;
            if (cleanup_buf) {
                soc_cm_sfree(unit, cleanup_buf);
            }
            /* Since none of the monitors are in use, clear the hist_1us_enable bit */
            rv = soc_reg_get(unit, AUX_ARB_CONTROLr, REG_PORT_ANY, 0, &rval);
            if (rv == BCM_E_NONE) {
                COMPILER_64_SET(val, 0, 0);
                soc_reg64_field_set(unit, AUX_ARB_CONTROLr, &rval, HIST_1US_ENABLEf, val);
                /* coverity[check_return : FALSE] */
                soc_reg_set(unit, AUX_ARB_CONTROLr, REG_PORT_ANY, 0, rval);
            }
        }
    }

    latency_monitor_driver_data.run[monitor_id] = FALSE;
    sal_free(access_type);

    latency_monitor_mem[unit][monitor_id].sbusdma_read_thread_pid = 0;
    sal_thread_exit(0);
}

int
latency_monitor_read(int unit, int monitor_id)
{
    sal_thread_t pid;

    /* sbus dma read */
    latency_monitor_driver_data.mem_done[monitor_id] = FALSE;

    if (latency_monitor_mem[unit][monitor_id].sbusdma_read_thread_pid) {
        return BCM_E_NONE;
    }

    pid = sal_thread_create("bcm_latency_monitor_sbusdma_read",
                            SAL_THREAD_STKSZ,
                            200,
                            sbusdma_read,
                            INT_TO_PTR(monitor_id << 8 | unit));


    LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                 (BSL_META_U(unit, "\npid_sbusdma[%0d] = %p"), monitor_id, pid));

    if (pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_BCM_LATENCY_MONITOR,
                        (BSL_META_U(unit, "Failed to create sbusdma_read thread for monitor ID:%d \n"),
                                     monitor_id));
        return BCM_E_FAIL;
    }

    pid = sal_thread_create("bcm_latency_monitor_counter_accrue",
                            SAL_THREAD_STKSZ,
                            200,
                            counter_accrue_thread,
                            INT_TO_PTR(monitor_id << 8 | unit));

    if (pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_BCM_LATENCY_MONITOR,
                        (BSL_META_U(unit, "Failed to create counter_accrue_thread, for monitor ID:%d \n"),
                                     monitor_id));
        return BCM_E_FAIL;
    } else {
        LOG_DEBUG(BSL_LS_BCM_LATENCY_MONITOR,
                     (BSL_META_U(unit, "created counter_accrue_thread for monitor%d  \n"), monitor_id));
    }

    return BCM_E_NONE;
}

int
latency_monitor_init(int unit, int monitor_id)
{
    int index_min, index_max;
    int frag, delta;

    if (!latency_monitor_mem[unit][monitor_id].init_done) {
        latency_monitor_driver_data.monitors_in_use_count++;
        latency_monitor_mem[unit][monitor_id].frag_count  = LATENCY_MONITOR_DEFAULT_FRAG_COUNT;

        if (latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem == NULL) {
            latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem =
                    sal_sem_create("DMA Handler Count Semaphore", sal_sem_COUNTING, 0);
        }

        if (latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock == NULL) {
            latency_monitor_mem[unit][monitor_id].dma_handle_queue_lock =
                    sal_sem_create("DMA Handler Queue Lock", sal_sem_BINARY, 1);
        }

        if (latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem == NULL) {
            latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem =
                    sal_sem_create("Latency Monitor Counter Semaphore", sal_sem_BINARY, 1);
        }

        _sbusdma_desc_handle_list_init(&latency_monitor_mem[unit][monitor_id].handle_list);

        if (latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger == NULL) {
            latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger =
                    sal_sem_create("Latency Monitor Thread Sempahore", sal_sem_BINARY, 0);
        }

        latency_monitor_mem[unit][monitor_id].init_done = TRUE;
        latency_monitor_driver_data.run[monitor_id] = FALSE;
        latency_monitor_driver_data.mem_done[monitor_id] = TRUE;
    }

    if (latency_monitor_driver_data.sbusdma_run_lock == NULL) {
        latency_monitor_driver_data.sbusdma_run_lock =
                                    sal_sem_create("SBUS DMA RUN Lock", sal_sem_BINARY, 1);
    }

    latency_monitor_mem[unit][monitor_id].copyno = SOC_MEM_BLOCK_ANY(unit, latency_monitor_mem[unit][monitor_id].mem);
    if (!SOC_MEM_BLOCK_VALID(unit, latency_monitor_mem[unit][monitor_id].mem, latency_monitor_mem[unit][monitor_id].copyno)) {
        LOG_ERROR(BSL_LS_BCM_LATENCY_MONITOR,
                  (BSL_META_U(unit,
                  "Invalid copyno %d specified in %s\n"),
                  latency_monitor_mem[unit][monitor_id].copyno,
                  SOC_MEM_NAME(unit, latency_monitor_mem[unit][monitor_id].mem)));
        return BCM_E_FAIL;
    }

    index_min = soc_mem_index_min(unit, latency_monitor_mem[unit][monitor_id].mem);
    index_max = soc_mem_index_max(unit, latency_monitor_mem[unit][monitor_id].mem);

    if (latency_monitor_mem[unit][monitor_id].index_min <= index_min) {
        latency_monitor_mem[unit][monitor_id].index_min = index_min;
    }
    if (latency_monitor_mem[unit][monitor_id].index_min > index_max) {
        latency_monitor_mem[unit][monitor_id].index_min = index_min;
    }

    if (latency_monitor_mem[unit][monitor_id].count <= 0) {
        /*
         * do not use soc_mem_index_count, index_max may have been,
         * index_max may have been modified above
         */
       latency_monitor_mem[unit][monitor_id].count = index_max - index_min + 1;
    }

    if (latency_monitor_mem[unit][monitor_id].index_min + latency_monitor_mem[unit][monitor_id].count - 1 > index_max) {
        latency_monitor_mem[unit][monitor_id].count = index_max - latency_monitor_mem[unit][monitor_id].index_min + 1;
    }

    latency_monitor_mem[unit][monitor_id].index_max = latency_monitor_mem[unit][monitor_id].index_min +
                             latency_monitor_mem[unit][monitor_id].count - 1;

    if (latency_monitor_mem[unit][monitor_id].frag_count <= latency_monitor_driver_data.num_mem) {
        latency_monitor_mem[unit][monitor_id].frag_count = latency_monitor_driver_data.num_mem;
        latency_monitor_mem[unit][monitor_id].frag_index_min[0] = latency_monitor_mem[unit][monitor_id].index_min;
        latency_monitor_mem[unit][monitor_id].frag_index_max[0] = latency_monitor_mem[unit][monitor_id].index_max;
    } else {
        if (latency_monitor_mem[unit][monitor_id].frag_count > latency_monitor_mem[unit][monitor_id].count) {
            latency_monitor_mem[unit][monitor_id].frag_count = latency_monitor_mem[unit][monitor_id].count;
        }

        delta = (latency_monitor_mem[unit][monitor_id].index_max - latency_monitor_mem[unit][monitor_id].index_min + 1) /
                latency_monitor_mem[unit][monitor_id].frag_count;
        for (frag = 0; frag < latency_monitor_mem[unit][monitor_id].frag_count; frag++) {
            latency_monitor_mem[unit][monitor_id].frag_index_min[frag] = latency_monitor_mem[unit][monitor_id].index_min +
                                                frag * delta;
            latency_monitor_mem[unit][monitor_id].frag_index_max[frag] = latency_monitor_mem[unit][monitor_id].index_min +
                                                (frag + 1) * delta - 1;
        }
        latency_monitor_mem[unit][monitor_id].frag_index_max[latency_monitor_mem[unit][monitor_id].frag_count - 1] =
            latency_monitor_mem[unit][monitor_id].index_max;
    }

    return BCM_E_NONE;
}


/* Set monitor with a given configuration. */
int
bcm_tomahawk3_latency_monitor_config_set(
    int unit,
    uint8 monitor_id,
    bcm_latency_monitor_config_t *config)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    pbmp_t                  src_pbm;
    pbmp_t                  dest_pbm;
    soc_field_t             field_ids[5];
    uint32                  field_values[5];
    int                     port_idx;
    bcm_stat_object_t       stat_obj;
    uint32                  stat_counter_id = 0;
    uint32                  num_entries;
    bcm_stat_group_mode_t   group;
    bcm_stat_object_t       object;
    uint32                  mode;
    uint32                  pool_number;
    uint32                  base_idx;
    soc_reg_t               flex_bank_sel_reg;
    soc_field_t             pbmp_field;
    uint32                  sw_buf_size;
    uint16                  time_step_incr;
    uint32                  time_step_set;
    soc_info_t              *si;
    int                     rv = BCM_E_NONE;
    uint8                   count_mode;


    if (monitor_id > MONITOR_ID_3) {
        return BCM_E_PARAM;
    }

    if (!config) {
        return BCM_E_PARAM;
    }

    if ((!wb_post_init) &&
        (latency_monitor_mem[unit][monitor_id].status == LATENCY_MONITOR_STATE_RUNNING)) {
        return BCM_E_BUSY;
    }

    if ((config->mode != bcmLatencyMonitorAccrueMode) &&
        (config->mode != bcmLatencyMonitorTimeSeriesMode)) {
        return BCM_E_PARAM;
    }

    if ((config->count_mode < bcmLatencyMonitorCutThrough) ||
        (config->count_mode > bcmLatencyMonitorCountAll)) {
        return _SHR_E_PARAM;
    }

    if ((latency_monitor_mem[unit][monitor_id].status != LATENCY_MONITOR_STATE_CLEANUP) &&
        (config->time_step < LATENCY_MONITOR_MIN_TIME_STEP)) {
        return BCM_E_PARAM;
    }

    latency_monitor_mem[unit][monitor_id].monitor_mode = config->mode;
    latency_monitor_mem[unit][monitor_id].sw_buf_size = 0;
    if (config->mode == bcmLatencyMonitorTimeSeriesMode) {
        if (config->histogram_set_count < TIME_STEP_SET_COUNT) {
            return BCM_E_PARAM;
        }

        if (latency_monitor_mem[unit][monitor_id].sw_buf_addr) {
            sal_free(latency_monitor_mem[unit][monitor_id].sw_buf_addr);
        }
 
        sw_buf_size = (config->histogram_set_count) * COUNTERS_PER_STEP_SET * LATENCY_MONITOR_SW_BUF_WIDTH;
        latency_monitor_mem[unit][monitor_id].sw_buf_addr =
                                    sal_alloc(sw_buf_size, "sw ring buffer");
        if (latency_monitor_mem[unit][monitor_id].sw_buf_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Save the sw ring buffer size in terms of 64 bit word count */
        latency_monitor_mem[unit][monitor_id].sw_buf_size = config->histogram_set_count * COUNTERS_PER_STEP_SET;
    }

    if ((latency_monitor_mem[unit][monitor_id].status != LATENCY_MONITOR_STATE_CLEANUP) &&
        (!latency_monitor_driver_data.init_done)) {
        latency_monitor_driver_data_init(unit);
    }

    SOC_LATENCY_MONITOR_INFO(unit, monitor_id).time_step = config->time_step;
    SOC_LATENCY_MONITOR_INFO(unit, monitor_id).flex_pool_number = -1;
    BCM_PBMP_ASSIGN(SOC_LATENCY_MONITOR_INFO(unit, monitor_id).src_pbmp, config->src_pbmp);
    BCM_PBMP_ASSIGN(SOC_LATENCY_MONITOR_INFO(unit, monitor_id).dest_pbmp, config->dest_pbmp);

    /* Convert from zero indexed enum value to one indexed value. Register field value '0'  is reserved in HW */
    count_mode = config->count_mode + 1;

    switch (monitor_id) {
    case MONITOR_ID_0:
        field_ids[0] = TIME_SERIES_STEPf;
        field_values[0] = config->time_step;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_0_FLEX_BANK_SEL_0r, REG_PORT_ANY, 1,
                                     field_ids, field_values));

        field_ids[0] = MON_CUT_THROUGHf;
        field_values[0] = count_mode;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_0_CONFIGr, REG_PORT_ANY, 1,
                                     field_ids, field_values));
        pbmp_field = MONITOR_0f;
        stat_obj = bcmIntStatObjectEgrLatencyMonitor0;
        flex_bank_sel_reg = EGR_HISTO_MON_0_FLEX_BANK_SEL_1r;
        break;
    case MONITOR_ID_1:
        field_ids[0] = TIME_SERIES_STEPf;
        field_values[0] = config->time_step;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_1_FLEX_BANK_SEL_0r, REG_PORT_ANY, 1,
                                     field_ids, field_values));

        field_ids[0] = MON_CUT_THROUGHf;
        field_values[0] = count_mode;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_1_CONFIGr, REG_PORT_ANY, 1,
                                     field_ids, field_values));
        pbmp_field = MONITOR_1f;
        stat_obj = bcmIntStatObjectEgrLatencyMonitor1;
        flex_bank_sel_reg = EGR_HISTO_MON_1_FLEX_BANK_SEL_1r;
        break;
    case MONITOR_ID_2:
        field_ids[0] = TIME_SERIES_STEPf;
        field_values[0] = config->time_step;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_2_FLEX_BANK_SEL_0r, REG_PORT_ANY, 1,
                                     field_ids, field_values));

        field_ids[0] = MON_CUT_THROUGHf;
        field_values[0] = count_mode;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_2_CONFIGr, REG_PORT_ANY, 1,
                                     field_ids, field_values));
        pbmp_field = MONITOR_2f;
        stat_obj = bcmIntStatObjectEgrLatencyMonitor2;
        flex_bank_sel_reg = EGR_HISTO_MON_2_FLEX_BANK_SEL_1r;
        break;
    case MONITOR_ID_3:
        field_ids[0] = TIME_SERIES_STEPf;
        field_values[0] = config->time_step;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_3_FLEX_BANK_SEL_0r, REG_PORT_ANY, 1,
                                     field_ids, field_values));

        field_ids[0] = MON_CUT_THROUGHf;
        field_values[0] = count_mode;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EGR_HISTO_MON_3_CONFIGr, REG_PORT_ANY, 1,
                                     field_ids, field_values));
        pbmp_field = MONITOR_3f;
        stat_obj = bcmIntStatObjectEgrLatencyMonitor3;
        flex_bank_sel_reg = EGR_HISTO_MON_3_FLEX_BANK_SEL_1r;
        break;
    default:
        return BCM_E_PARAM;
    }

    if (latency_monitor_mem[unit][monitor_id].status != LATENCY_MONITOR_STATE_CLEANUP) {
        if (latency_monitor_mem[unit][monitor_id].stat_counter_id != 0) {
            rv = _bcm_esw_stat_group_destroy(unit,
                        latency_monitor_mem[unit][monitor_id].stat_counter_id);

            /* Return error if the stat group exists and on failure to delete it */
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        rv = _bcm_esw_stat_group_create(unit, stat_obj,
                                        bcmIntStatGroupModeLatencyMonitor,
                                        &stat_counter_id, &num_entries);

        if (rv != BCM_E_NONE) {
            bcm_tomahawk3_latency_monitor_destroy(unit, monitor_id);
            return rv;
        }

        _bcm_esw_stat_get_counter_id_info(unit, stat_counter_id, &group, &object,
                                           &mode, &pool_number, &base_idx);

        time_step_incr = TIME_STEP_COUNTER_INCR;
        time_step_set = (TIME_STEP_SET_COUNT - 1);
    } else {
        latency_monitor_mem[unit][monitor_id].sw_buf_size = 0;
        pool_number = 0;
        base_idx = 0;
        time_step_incr = 0;
        time_step_set = 0;
    }

    latency_monitor_mem[unit][monitor_id].time_step = config->time_step;
    latency_monitor_mem[unit][monitor_id].wrap_around_time =
                                        config->time_step * TIME_STEP_SET_COUNT;

    src_pbm = PBMP_PORT_ALL(unit);
    dest_pbm = PBMP_PORT_ALL(unit);
    if (latency_monitor_mem[unit][monitor_id].status != LATENCY_MONITOR_STATE_CLEANUP) {
        field_values[0] = 1;
        BCM_PBMP_AND(src_pbm, config->src_pbmp);
        BCM_PBMP_AND(dest_pbm, config->dest_pbmp);
    } else {
        field_values[0] = 0;
    }

    field_ids[0] = pbmp_field;
    BCM_PBMP_ITER(src_pbm, port_idx) {
        BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, EGR_HISTO_SRC_PORT_MAPr, port_idx, 1,
                                 field_ids, field_values));
    }

    latency_monitor_mem[unit][monitor_id].dest_pbmp_pipe_map = 0;
    si = &SOC_INFO(unit);
    field_ids[0] = pbmp_field;
    BCM_PBMP_ITER(dest_pbm, port_idx) {
        latency_monitor_mem[unit][monitor_id].dest_pbmp_pipe_map |= 1 << si->port_pipe[port_idx];
        BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, EGR_HISTO_DST_PORT_MAPr, port_idx, 1,
                                 field_ids, field_values));
    }


    /* If the base index is zero, skip the first two reserved entries and select the next power of 2 entry */
    if (base_idx == 0)
        latency_monitor_mem[unit][monitor_id].index_min = base_idx + 4;
    else
        latency_monitor_mem[unit][monitor_id].index_min = base_idx;

    field_ids[0] = FLEX_CTR_POOL_NUMBERf;
    field_values[0] = pool_number;
    field_ids[1] = HISTOGRAM_OFFSETf;
    field_values[1] = latency_monitor_mem[unit][monitor_id].index_min;
    field_ids[2] = TIME_STEP_COUNTER_INCRf;
    field_values[2] = time_step_incr;
    field_ids[3] = FLEX_CTR_OFFSET_MODEf;
    field_values[3] = 0;
    field_ids[4] = MAX_TIME_STEP_SETSf;
    field_values[4] = time_step_set;

    BCM_IF_ERROR_RETURN
    (soc_reg_fields32_modify(unit, flex_bank_sel_reg, REG_PORT_ANY, 5,
                             field_ids, field_values));

    latency_monitor_mem[unit][monitor_id].stat_counter_id = stat_counter_id;
    latency_monitor_mem[unit][monitor_id].status = LATENCY_MONITOR_STATE_IDLE;

    latency_monitor_mem[unit][monitor_id].count = COUNTERS_PER_STEP_SET * TIME_STEP_SET_COUNT;
    latency_monitor_mem[unit][monitor_id].index_max = latency_monitor_mem[unit][monitor_id].index_min + latency_monitor_mem[unit][monitor_id].count;
    switch (pool_number) {
    case 0:
        latency_monitor_mem[unit][monitor_id].mem = EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE0m;
        break;
    case 1:
        latency_monitor_mem[unit][monitor_id].mem = EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE0m;
        break;
    case 2:
        latency_monitor_mem[unit][monitor_id].mem = EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE0m;
        break;
    case 3:
        latency_monitor_mem[unit][monitor_id].mem = EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE0m;
        break;
    default:
        return BCM_E_BADID;
    }

    latency_monitor_mem[unit][monitor_id].entwords = soc_mem_entry_words(unit, latency_monitor_mem[unit][monitor_id].mem);
    latency_monitor_mem[unit][monitor_id].bufwords = latency_monitor_mem[unit][monitor_id].entwords * latency_monitor_mem[unit][monitor_id].count;

#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

/* Get configuration for a given monitor. */
int
bcm_tomahawk3_latency_monitor_config_get(
    int unit,
    uint8 monitor_id,
    bcm_latency_monitor_config_t *config)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint32          reg_value;
    int             rv, port_idx;
    soc_field_t     pbmp_field;
    soc_reg_t       mon_count_mode_reg;

    if (config == NULL)
        return BCM_E_PARAM;

    switch (monitor_id) {
    case MONITOR_ID_0:
        rv = soc_reg32_get(unit, EGR_HISTO_MON_0_FLEX_BANK_SEL_0r, REG_PORT_ANY,0,
                            &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        config->time_step = soc_reg_field_get(unit,EGR_HISTO_MON_0_FLEX_BANK_SEL_0r,reg_value,
                               TIME_SERIES_STEPf);
        pbmp_field = MONITOR_0f;
        mon_count_mode_reg = EGR_HISTO_MON_0_CONFIGr;
        break;
    case MONITOR_ID_1:
        rv = soc_reg32_get(unit, EGR_HISTO_MON_1_FLEX_BANK_SEL_0r, REG_PORT_ANY,0,
                            &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        config->time_step = soc_reg_field_get(unit,EGR_HISTO_MON_1_FLEX_BANK_SEL_0r,reg_value,
                               TIME_SERIES_STEPf);
        pbmp_field = MONITOR_1f;
        mon_count_mode_reg = EGR_HISTO_MON_1_CONFIGr;
        break;
    case MONITOR_ID_2:
        rv = soc_reg32_get(unit, EGR_HISTO_MON_2_FLEX_BANK_SEL_0r, REG_PORT_ANY,0,
                            &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        config->time_step = soc_reg_field_get(unit,EGR_HISTO_MON_2_FLEX_BANK_SEL_0r,reg_value,
                               TIME_SERIES_STEPf);
        pbmp_field = MONITOR_2f;
        mon_count_mode_reg = EGR_HISTO_MON_2_CONFIGr;
        break;
    case MONITOR_ID_3:
        rv = soc_reg32_get(unit, EGR_HISTO_MON_3_FLEX_BANK_SEL_0r, REG_PORT_ANY,0,
                            &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        config->time_step = soc_reg_field_get(unit,EGR_HISTO_MON_3_FLEX_BANK_SEL_0r,reg_value,
                               TIME_SERIES_STEPf);
        pbmp_field = MONITOR_3f;
        mon_count_mode_reg = EGR_HISTO_MON_3_CONFIGr;
        break;
    default:
        return BCM_E_PARAM;
    }

    config->mode = latency_monitor_mem[unit][monitor_id].monitor_mode;
    if (config->mode == bcmLatencyMonitorTimeSeriesMode) {
        config->histogram_set_count = (latency_monitor_mem[unit][monitor_id].sw_buf_size)/COUNTERS_PER_STEP_SET;
    }

    rv = soc_reg32_get(unit, mon_count_mode_reg, REG_PORT_ANY, 0, &reg_value);
    SOC_IF_ERROR_RETURN(rv);
    config->count_mode = soc_reg_field_get(unit, EGR_HISTO_MON_0_CONFIGr, reg_value,
                           MON_CUT_THROUGHf);

    /* Convert from '1' indexed register field value to '0' indexed enum type */
    config->count_mode -= 1;

    BCM_PBMP_CLEAR(config->src_pbmp);
    BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port_idx) {
        rv = soc_reg32_get(unit, EGR_HISTO_SRC_PORT_MAPr, port_idx, 0,
                                         &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        if (soc_reg_field_get(unit,EGR_HISTO_SRC_PORT_MAPr,reg_value,
                           pbmp_field))
            BCM_PBMP_PORT_ADD(config->src_pbmp, port_idx);
    }

    BCM_PBMP_CLEAR(config->dest_pbmp);
    BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port_idx) {
        rv = soc_reg32_get(unit, EGR_HISTO_DST_PORT_MAPr, port_idx, 0,
                                         &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        if (soc_reg_field_get(unit,EGR_HISTO_DST_PORT_MAPr,reg_value,
                           pbmp_field))
            BCM_PBMP_PORT_ADD(config->dest_pbmp, port_idx);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Configures the LATENCY_LIMIT device table, which has 32 entries max
 * and each entry is specified in terms of 64 nSec quanta.
 */
int
bcm_tomahawk3_latency_monitor_latency_limit_table_set(
    int unit,
    uint8 table_size,
    uint32 *latency_limit_table)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    int i;
    uint32  value;

    if (table_size > LATENCY_LIMIT_TABLE_SIZE)
        return BCM_E_PARAM;

    if (latency_limit_table == NULL)
        return BCM_E_PARAM;

    for (i=0; i<table_size; i++) {
        if (latency_limit_table[i] < LATENCY_LIMIT_QUANTA)
            value = 1;                  /* Min is 64 nS */
        else
            value = latency_limit_table[i]/LATENCY_LIMIT_QUANTA;

        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, EGR_HISTO_LATENCY_LIMITm, i, LATENCY_LIMITf,
                                    value));
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Output is the table size and configured values of LATENCY_LIMIT device
 * table.
 */

int
bcm_tomahawk3_latency_monitor_latency_limit_table_get(
    int unit,
    uint8 table_size,
    uint32 *latency_limit_table)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    int i, rv;
    uint32 value;

    if (table_size > LATENCY_LIMIT_TABLE_SIZE)
        table_size = LATENCY_LIMIT_TABLE_SIZE;

    if (latency_limit_table == NULL)
        return BCM_E_PARAM;

    for (i=0; i<table_size; i++) {
        rv = soc_mem_read(unit, EGR_HISTO_LATENCY_LIMITm, MEM_BLOCK_ANY,
                             i, &value);
        if (SOC_FAILURE(rv)) {
            return (rv);
        }

        latency_limit_table[i] = soc_mem_field32_get(unit,
                                                    EGR_HISTO_LATENCY_LIMITm,
                                                    &value, LATENCY_LIMITf);
        latency_limit_table[i] *= LATENCY_LIMIT_QUANTA;
    }
#endif
    return BCM_E_NONE;
}


/*
 * Configure the bucket size and upper latency limit of each bucket, for a
 * given cos queue and monitor id.
 */
int
bcm_tomahawk3_latency_monitor_cosq_config_set(
    int unit,
    uint8 monitor_id,
    uint8 cosq,
    bcm_latency_monitor_cosq_info_t cosq_info)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    int         i, j, index;
    soc_mem_t   mem;
    uint8   cosq_first;
    uint8   cosq_last;

    if ((cosq >= LATENCY_MONITOR_QUEUE_COUNT) && ((int8)cosq != -1)) 
        return BCM_E_PARAM;

    if (cosq_info.bucket_count > LATENCY_HISTOGRAM_BUCKET_COUNT)
        return BCM_E_PARAM;

    switch (monitor_id) {
    case MONITOR_ID_0:
        mem = EGR_HISTO_MON_0_Q_LATENCY_LIMIT_SELm;
        break;
    case MONITOR_ID_1:
        mem = EGR_HISTO_MON_1_Q_LATENCY_LIMIT_SELm;
        break;
    case MONITOR_ID_2:
        mem = EGR_HISTO_MON_2_Q_LATENCY_LIMIT_SELm;
        break;
    case MONITOR_ID_3:
        mem = EGR_HISTO_MON_3_Q_LATENCY_LIMIT_SELm;
        break;
    default:
        return BCM_E_PARAM;
    }

    cosq_first = cosq;
    cosq_last  = cosq;
    if ((int8)cosq == -1) {
        cosq_first = 0;
        cosq_last = (LATENCY_MONITOR_QUEUE_COUNT - 1);
    }

    for (i=cosq_first; i<=cosq_last; i++) {
        latency_monitor_mem[unit][monitor_id].cosq_info[i].bucket_count = cosq_info.bucket_count;
        for (j=0; j<(cosq_info.bucket_count - 1); j++) {
            if (cosq_info.latency_index_array[j] >= LATENCY_LIMIT_TABLE_SIZE)
                return BCM_E_PARAM;

            index = (i * (LATENCY_HISTOGRAM_BUCKET_COUNT - 1)) + j;
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, mem, index, Q_LATENCY_LIMIT_SELf,
                                        cosq_info.latency_index_array[j]));

        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Output is the bucket size and upper latency limit of each bucket for a
 * given cos queue and monitor.
 */
int
bcm_tomahawk3_latency_monitor_cosq_config_get(
    int unit,
    uint8 monitor_id,
    uint8 cosq,
    bcm_latency_monitor_cosq_info_t *cosq_info)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    int         i, index, rv;
    uint32      value;
    soc_mem_t   mem;

    if (cosq_info == NULL)
        return BCM_E_PARAM;

    if (cosq >= LATENCY_MONITOR_QUEUE_COUNT)
        return BCM_E_PARAM;

    switch (monitor_id) {
    case MONITOR_ID_0:
        mem = EGR_HISTO_MON_0_Q_LATENCY_LIMIT_SELm;
        break;
    case MONITOR_ID_1:
        mem = EGR_HISTO_MON_1_Q_LATENCY_LIMIT_SELm;
        break;
    case MONITOR_ID_2:
        mem = EGR_HISTO_MON_2_Q_LATENCY_LIMIT_SELm;
        break;
    case MONITOR_ID_3:
        mem = EGR_HISTO_MON_3_Q_LATENCY_LIMIT_SELm;
        break;
    default:
        return BCM_E_PARAM;
    }

    cosq_info->bucket_count = latency_monitor_mem[unit][monitor_id].cosq_info[cosq].bucket_count;

    for (i=0; i<(cosq_info->bucket_count - 1); i++) {
        index = (cosq * (LATENCY_HISTOGRAM_BUCKET_COUNT - 1)) + i;

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &value);
        if (SOC_FAILURE(rv)) {
            return (rv);
        }

        cosq_info->latency_index_array[i] = soc_mem_field32_get(unit,
                                                    mem, &value, Q_LATENCY_LIMIT_SELf);
    }

#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

#ifdef BCM_TOMAHAWK3_SUPPORT
static int
_clear_debug_registers(int unit, uint8 monitor_id)
{
    if (SOC_IS_TOMAHAWK3(unit)) {
        switch (monitor_id) {
        case MONITOR_ID_0:
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_0_LAST_COUNTER_INDEXr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_0_HISTOGRAM_SETr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_0_FLEX_COUNTER_WRAP_COUNTERr, REG_PORT_ANY, 0, 0));
            break;
        case MONITOR_ID_1:
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_1_LAST_COUNTER_INDEXr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_1_HISTOGRAM_SETr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_1_FLEX_COUNTER_WRAP_COUNTERr, REG_PORT_ANY, 0, 0));
            break;
        case MONITOR_ID_2:
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_2_LAST_COUNTER_INDEXr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_2_HISTOGRAM_SETr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_2_FLEX_COUNTER_WRAP_COUNTERr, REG_PORT_ANY, 0, 0));
            break;
        case MONITOR_ID_3:
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_3_LAST_COUNTER_INDEXr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_3_HISTOGRAM_SETr, REG_PORT_ANY, 0, 0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EGR_HISTO_MON_3_FLEX_COUNTER_WRAP_COUNTERr, REG_PORT_ANY, 0, 0));
            break;
        default:
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}
#endif

/* Enable or Disable a given monitor */
int
bcm_tomahawk3_latency_monitor_enable(
    int unit,
    uint8 monitor_id,
    uint8 enable)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_field_t         field_ids[1];
    uint32              field_values[1];
    soc_reg_t           mon_config_reg;
    int                 rv = BCM_E_NONE;
    uint32              hist_1us_enable;
    uint32              rval;

    if (monitor_id > MONITOR_ID_3) {
        return BCM_E_PARAM;
    }

    if (!wb_post_init) {
        /* If monitor is in cleanup/error state, return failure. */
        if (latency_monitor_mem[unit][monitor_id].status == LATENCY_MONITOR_STATE_CLEANUP) {
            return BCM_E_FAIL;
        }

        /* If monitor is already enabled and running. */
        if ((enable) &&(latency_monitor_mem[unit][monitor_id].status == LATENCY_MONITOR_STATE_RUNNING)) {
            return BCM_E_NONE;
        }

        /* If monitor is already disabled and idle. */
        if ((!enable) && (latency_monitor_mem[unit][monitor_id].status == LATENCY_MONITOR_STATE_IDLE)) {
            return BCM_E_NONE;
        }
    }

    if (enable)
        latency_monitor_init(unit, monitor_id);

    switch (monitor_id) {
    case MONITOR_ID_0:
        mon_config_reg = EGR_HISTO_MON_0_CONFIGr;
        break;
    case MONITOR_ID_1:
        mon_config_reg = EGR_HISTO_MON_1_CONFIGr;
        break;
    case MONITOR_ID_2:
        mon_config_reg = EGR_HISTO_MON_2_CONFIGr;
        break;
    case MONITOR_ID_3:
        mon_config_reg = EGR_HISTO_MON_3_CONFIGr;
        break;
    default:
        return BCM_E_PARAM;
    }

    field_ids[0] = MONITOR_ENf;
    field_values[0] = enable;
    BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, mon_config_reg, REG_PORT_ANY, 1,
                                 field_ids, field_values));

    if (enable) {
        sal_memset(latency_monitor_mem[unit][monitor_id].histogram_packet64_counter, 0,
                    sizeof(latency_monitor_mem[unit][monitor_id].histogram_packet64_counter));
        sal_memset(latency_monitor_mem[unit][monitor_id].histogram_byte64_counter, 0,
                    sizeof(latency_monitor_mem[unit][monitor_id].histogram_byte64_counter));

        if (latency_monitor_mem[unit][monitor_id].monitor_mode == bcmLatencyMonitorTimeSeriesMode) {
            if (latency_monitor_mem[unit][monitor_id].sw_buf_addr) {
                sal_memset(latency_monitor_mem[unit][monitor_id].sw_buf_addr, 0,
                        latency_monitor_mem[unit][monitor_id].sw_buf_size);
            }
            latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx = 0;
        }

        /* Clear below debug registers for the monitor */
        SOC_IF_ERROR_RETURN(_clear_debug_registers(unit, monitor_id));

        latency_monitor_mem[unit][monitor_id].flex_ctr_wrap_ctr = 0;

        latency_monitor_mem[unit][monitor_id].status = LATENCY_MONITOR_STATE_RUNNING;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, AUX_ARB_CONTROLr, REG_PORT_ANY, 0, &rval));
        hist_1us_enable = soc_reg_field_get(unit, AUX_ARB_CONTROLr, rval, HIST_1US_ENABLEf);
        if (!hist_1us_enable) {
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, HIST_1US_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, AUX_ARB_CONTROLr, REG_PORT_ANY, 0, rval));
        }

        rv = latency_monitor_read(unit, monitor_id);
        if (rv == BCM_E_NONE)
            latency_monitor_driver_data.run[monitor_id] = TRUE;
        else
            return rv;
    } else {
        latency_monitor_mem[unit][monitor_id].pipe_map_temp = 0;
        latency_monitor_driver_data.run[monitor_id] = FALSE;
        /* Signal the sbusdma_read thread for termination */
        sal_sem_give(latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger);
        /* Signal the counter_accrue_thread for termination */
        sal_sem_give(latency_monitor_mem[unit][monitor_id].dma_handle_counter_sem);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Returns the accrued histogram counter values for a given monitor ID
 * and cosq (Valid in Accrue mode only)
 */
int
bcm_tomahawk3_latency_monitor_histogram_get(
    int unit,
    uint8 monitor_id,
    uint8 cosq,
    uint8 byte_counter_flag,
    bcm_latency_monitor_histogram_t *histogram)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint64      (*arr_pkt_cnt)[LATENCY_MONITOR_BUCKET_COUNT];
    uint64      (*arr_byte_cnt)[LATENCY_MONITOR_BUCKET_COUNT];

    if (histogram == NULL)
        return BCM_E_PARAM;

    if (monitor_id >= LATENCY_MONITOR_MAX_COUNT)
        return BCM_E_PARAM;

    if (latency_monitor_mem[unit][monitor_id].monitor_mode != bcmLatencyMonitorAccrueMode)
        return BCM_E_UNAVAIL;

    if (cosq >= LATENCY_MONITOR_QUEUE_COUNT)
        return BCM_E_PARAM;

    if (histogram->histogram_size > LATENCY_MONITOR_BUCKET_COUNT)
        histogram->histogram_size = LATENCY_MONITOR_BUCKET_COUNT;

    arr_pkt_cnt = (uint64 (*)[LATENCY_MONITOR_BUCKET_COUNT])latency_monitor_mem[unit][monitor_id].histogram_packet64_counter;
    arr_byte_cnt = (uint64 (*)[LATENCY_MONITOR_BUCKET_COUNT])latency_monitor_mem[unit][monitor_id].histogram_byte64_counter;

    sal_sem_take(latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem, sal_sem_FOREVER);
    if (byte_counter_flag)
        sal_memcpy(histogram->histogram, arr_byte_cnt[cosq], sizeof(uint64)*histogram->histogram_size);
    else
        sal_memcpy(histogram->histogram, arr_pkt_cnt[cosq], sizeof(uint64)*histogram->histogram_size);
    sal_sem_give(latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem);
#endif

    return BCM_E_NONE;
}

int
bcm_tomahawk3_latency_monitor_info_get(
    int unit,
    uint8 monitor_id,
    bcm_latency_monitor_info_t *monitor_info)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (monitor_info == NULL)
        return BCM_E_PARAM;

    if (monitor_id >= LATENCY_MONITOR_MAX_COUNT)
        return BCM_E_PARAM;

    switch (latency_monitor_mem[unit][monitor_id].status) {
    case LATENCY_MONITOR_STATE_RUNNING:
        monitor_info->monitor_status = bcmLatencyMonitorStatusRunning;
        break;
    case LATENCY_MONITOR_STATE_IDLE:
        monitor_info->monitor_status = bcmLatencyMonitorStatusIdle;
        break;
    case LATENCY_MONITOR_STATE_ERROR:
        monitor_info->monitor_status = bcmLatencyMonitorStatusError;
        break;
    default:
        monitor_info->monitor_status = bcmLatencyMonitorStatusUninit;
        break;
    }

    monitor_info->sw_buf_addr = latency_monitor_mem[unit][monitor_id].sw_buf_addr;
    monitor_info->sw_buf_size = latency_monitor_mem[unit][monitor_id].sw_buf_size;
    monitor_info->sw_buf_wr_idx = (latency_monitor_mem[unit][monitor_id].sw_buf_wr_idx - 1); 
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

/* Release all the resource allocated for a given monitor. */
int
bcm_tomahawk3_latency_monitor_destroy(
    int unit,
    uint8 monitor_id)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    bcm_latency_monitor_config_t        monitor_cfg;
    bcm_latency_monitor_cosq_info_t     cosq_cfg;

    if (monitor_id >= LATENCY_MONITOR_MAX_COUNT)
        return BCM_E_PARAM;

    if (latency_monitor_mem[unit][monitor_id].status == LATENCY_MONITOR_STATE_RUNNING)
        return BCM_E_BUSY;

    _bcm_esw_stat_group_destroy(unit, latency_monitor_mem[unit][monitor_id].stat_counter_id);
    latency_monitor_mem[unit][monitor_id].stat_counter_id = 0;

    latency_monitor_mem[unit][monitor_id].status = LATENCY_MONITOR_STATE_CLEANUP;
    bcm_latency_monitor_config_t_init(&monitor_cfg);
    SOC_PBMP_ASSIGN(monitor_cfg.src_pbmp, SOC_LATENCY_MONITOR_INFO(unit, monitor_id).src_pbmp);
    SOC_PBMP_ASSIGN(monitor_cfg.dest_pbmp, SOC_LATENCY_MONITOR_INFO(unit, monitor_id).dest_pbmp);
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_latency_monitor_config_set(unit, monitor_id, &monitor_cfg));
    bcm_latency_monitor_cosq_info_t_init(&cosq_cfg);
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_latency_monitor_cosq_config_set(unit, monitor_id, -1, cosq_cfg));

    latency_monitor_mem[unit][monitor_id].status = 0;
    latency_monitor_mem[unit][monitor_id].init_done = FALSE;

    SOC_LATENCY_MONITOR_INFO(unit, monitor_id).time_step = -1;
    SOC_LATENCY_MONITOR_INFO(unit, monitor_id).flex_pool_number = -1;
    BCM_PBMP_CLEAR(SOC_LATENCY_MONITOR_INFO(unit, monitor_id).src_pbmp);
    BCM_PBMP_CLEAR(SOC_LATENCY_MONITOR_INFO(unit, monitor_id).dest_pbmp);

    if (latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger) {
        sal_sem_destroy(latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger);
        latency_monitor_mem[unit][monitor_id].latency_monitor_thread_trigger = NULL;
    }

    if (latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem) {
        sal_sem_destroy(latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem);
        latency_monitor_mem[unit][monitor_id].latency_monitor_counter_sem = NULL;
    }

    sal_memset(latency_monitor_mem[unit][monitor_id].histogram_packet64_counter, 0,
                sizeof(latency_monitor_mem[unit][monitor_id].histogram_packet64_counter));
    sal_memset(latency_monitor_mem[unit][monitor_id].histogram_byte64_counter, 0,
                sizeof(latency_monitor_mem[unit][monitor_id].histogram_byte64_counter));

    if (latency_monitor_mem[unit][monitor_id].monitor_mode == bcmLatencyMonitorTimeSeriesMode) {
        if (latency_monitor_mem[unit][monitor_id].sw_buf_addr) {
            sal_free(latency_monitor_mem[unit][monitor_id].sw_buf_addr);
            latency_monitor_mem[unit][monitor_id].sw_buf_addr = NULL;
        }
    }

#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

int
bcm_tomahawk3_latency_monitor_detach (int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    int monitor_id;

    if (soc_feature(unit, soc_feature_latency_monitor)) {
        for (monitor_id = 0; monitor_id < LATENCY_MONITOR_MAX_COUNT; monitor_id++) {
            if (latency_monitor_mem[unit][monitor_id].status ==
                        LATENCY_MONITOR_STATE_RUNNING) {
                latency_monitor_driver_data.run[monitor_id] = FALSE;
                bcm_tomahawk3_latency_monitor_enable(unit, monitor_id, 0);
                while (latency_monitor_mem[unit][monitor_id].status ==
                                             LATENCY_MONITOR_STATE_RUNNING) {
                    sal_usleep(5);
                }
            }
            bcm_tomahawk3_latency_monitor_destroy(unit, monitor_id);
        }
 
        latency_monitor_driver_data.init_done = 0;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

int
bcm_tomahawk3_latency_monitor_init (int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    int monitor_id;
    int rv;

    if (soc_feature(unit, soc_feature_latency_monitor)) {
        for (monitor_id = 0; monitor_id < LATENCY_MONITOR_MAX_COUNT; monitor_id++) {
            if (latency_monitor_mem[unit][monitor_id].status ==
                        LATENCY_MONITOR_STATE_RUNNING) {
                latency_monitor_driver_data.run[monitor_id] = FALSE;
                rv = bcm_tomahawk3_latency_monitor_enable(unit, monitor_id, 0);
                BCM_IF_ERROR_RETURN(rv);
            }
            rv = bcm_tomahawk3_latency_monitor_destroy(unit, monitor_id);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (!latency_monitor_driver_data.init_done)
            latency_monitor_driver_data_init(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            rv = _bcm_th3_latency_monitor_wb_recover(unit);
        } else {
            rv = _bcm_th3_latency_monitor_scache_alloc(unit, 1);
        }

        if (rv == BCM_E_NOT_FOUND) {
            /* Continue with initialization if scache not found */
            rv = BCM_E_NONE;
        }

        if (BCM_FAILURE(rv)) {
            return (rv);
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

int bcm_tomahawk3_latency_monitor_register(int unit, int monitor_id,
                                           bcm_latency_monitor_callback_fn cb,
                                           void *user_data)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (monitor_id > MONITOR_ID_3) {
        return BCM_E_PARAM;
    }

    if (!cb) {
        return BCM_E_PARAM;
    }

    if (latency_monitor_mem[unit][monitor_id].cb_fn != NULL) {
        return BCM_E_EXISTS;
    }

    latency_monitor_mem[unit][monitor_id].cb_fn = cb;
    latency_monitor_mem[unit][monitor_id].user_data = user_data;
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}

int bcm_tomahawk3_latency_monitor_unregister(int unit, int monitor_id,
                                             bcm_latency_monitor_callback_fn cb)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (monitor_id > MONITOR_ID_3) {
        return BCM_E_PARAM;
    }

    if (latency_monitor_mem[unit][monitor_id].cb_fn == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (latency_monitor_mem[unit][monitor_id].status ==
            LATENCY_MONITOR_STATE_RUNNING) {
        return BCM_E_FAIL;
    }

    latency_monitor_mem[unit][monitor_id].cb_fn = NULL;
    latency_monitor_mem[unit][monitor_id].user_data = NULL;
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return BCM_E_NONE;
}
 

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT

#define BCM_WB_LATENCY_MONITOR_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_LATENCY_MONITOR_DEFAULT_VERSION  BCM_WB_LATENCY_MONITOR_VERSION_1_0

#define LATENCY_MONITOR_SCACHE_WRITE(_scache, _val, _type)                     \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define LATENCY_MONITOR_SCACHE_READ(_scache, _var, _type)                      \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)

/*
 * Function:
 *     _bcm_th3_latency_monitor_scache_v0_global_size_get
 * Purpose:
 *     Get the size required to sync Latency Monitor global data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th3_latency_monitor_scache_v0_global_size_get(int unit)
{
    uint32 size = 0;

    /* Reserved bits */
    size += sizeof(uint32);

    /* Monitor status */
    size += sizeof(uint8) * LATENCY_MONITOR_MAX_COUNT;

    /*Monitor Mode */
    size += sizeof(uint8) * LATENCY_MONITOR_MAX_COUNT;

    /* SW Ring Buffer size (Timeseries mode only) */
    size += sizeof(uint32) * LATENCY_MONITOR_MAX_COUNT;

    /* Store configured bucket count for each Queue */
    size += sizeof(uint8) * LATENCY_MONITOR_QUEUE_COUNT * LATENCY_MONITOR_MAX_COUNT;

    return size;
}


/*
 * Function:
 *     _bcm_th3_latency_monitor_scache_v0_size_get
 * Purpose:
 *     Get Latency monitor scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_th3_latency_monitor_scache_v0_size_get(int unit)
{
    uint32 size = 0;

    /* Get global data size */
    size += _bcm_th3_latency_monitor_scache_v0_global_size_get(unit);

    return size;
}

int
_bcm_th3_latency_monitor_wb_post_init(int unit)
{
    int i=0;
    bcm_latency_monitor_config_t cfg;
    int monitor_status;
 
    wb_post_init = 1;   
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        if (latency_monitor_mem[unit][i].status) {
            monitor_status = latency_monitor_mem[unit][i].status;
            bcm_tomahawk3_latency_monitor_config_get(unit, i, &cfg);
            bcm_tomahawk3_latency_monitor_config_set(unit, i, &cfg);
            if (monitor_status == LATENCY_MONITOR_STATE_RUNNING) {
                bcm_tomahawk3_latency_monitor_enable(unit, i, 1);
            }
        }
    }
    wb_post_init = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_latency_monitor_scache_v0_global_recover
 * Purpose:
 *     Recover Latency monitor global data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_latency_monitor_scache_v0_global_recover(int unit, uint8 **scache)
{
    int i=0;
    int j=0;

    /* Reserved bits */
    *scache += sizeof(uint32);

    /* Restore Monitor state info */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        LATENCY_MONITOR_SCACHE_READ(*scache, latency_monitor_mem[unit][i].status, uint8);
    }
    
    /* Restore Monitor Mode */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        LATENCY_MONITOR_SCACHE_READ(*scache, latency_monitor_mem[unit][i].monitor_mode, uint8);
    }

    /* Restore Histogram set count in SW Ring buffer used for Timeseries mode */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        LATENCY_MONITOR_SCACHE_READ(*scache, latency_monitor_mem[unit][i].sw_buf_size, uint32);
    }

    /* Restore bucket count for each queues */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        for (j=0; j<LATENCY_MONITOR_QUEUE_COUNT; j++) {
            LATENCY_MONITOR_SCACHE_READ(*scache,
                     latency_monitor_mem[unit][i].cosq_info[j].bucket_count, uint8);
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_th3_latency_monitor_scache_v0_recover
 * Purpose:
 *     Recover Latency Monitor scache v0 data
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_latency_monitor_scache_v0_recover(int unit, uint8 **scache)
{
    /* Recover global data */
    BCM_IF_ERROR_RETURN(_bcm_th3_latency_monitor_scache_v0_global_recover(unit, scache));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_latency_monitor_scache_alloc
 * Purpose:
 *      Latency Monitor WB scache alloc
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_th3_latency_monitor_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_th3_latency_monitor_scache_v0_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_LATENCY_MONITOR, 0);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                                     &scache, BCM_WB_LATENCY_MONITOR_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _BCM_th3_latency_monitor_wb_recover
 * Purpose:
 *      Latency Monitor WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_th3_latency_monitor_wb_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_LATENCY_MONITOR, 0);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_LATENCY_MONITOR_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_th3_latency_monitor_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_th3_latency_monitor_scache_v0_recover(unit, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_th3_latency_monitor_scache_alloc(unit, 0));

    return rv;
}

/*
 * Function:
 *     _bcm_th3_latency_monitor_scache_v0_global_sync
 * Purpose:
 *     Sync Latency Monitor global data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th3_latency_monitor_scache_v0_global_sync(int unit, uint8 **scache)
{
    int i;
    int j;
    uint8 tmp = 0;

    /* Reserve first 32 bits */
    LATENCY_MONITOR_SCACHE_WRITE(*scache, 0, uint32);

    /* Store Monitor state info */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        LATENCY_MONITOR_SCACHE_WRITE(*scache, latency_monitor_mem[unit][i].status, uint8);
    }

    /* Store Monitor Mode */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        if (latency_monitor_mem[unit][i].monitor_mode == bcmLatencyMonitorTimeSeriesMode) {
            tmp = 1;
        } else {
            tmp = 0;
        }
        LATENCY_MONITOR_SCACHE_WRITE(*scache, tmp, uint8);
    }

    /* Store SW ring buffer size (used in Timeseries mode */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        LATENCY_MONITOR_SCACHE_WRITE(*scache, latency_monitor_mem[unit][i].sw_buf_size, uint32);
    }

    /* Store bucket count for each queues */
    for (i=0; i<LATENCY_MONITOR_MAX_COUNT; i++) {
        for (j=0; j<LATENCY_MONITOR_QUEUE_COUNT; j++) {
            LATENCY_MONITOR_SCACHE_WRITE(*scache, 
                     latency_monitor_mem[unit][i].cosq_info[j].bucket_count, uint8);
        }
    }


}

/*
 * Function:
 *     _bcm_th3_latency_monitor_scache_v0_sync
 * Purpose:
 *     Sync Latency Monitor scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_th3_latency_monitor_scache_v0_sync(int unit, uint8 **scache)
{
    /* Sync global data */
    _bcm_th3_latency_monitor_scache_v0_global_sync(unit, scache);
}

/*
 * Function:
 *      _bcm_th3_latency_monitor_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_th3_latency_monitor_sync(int unit)
{
    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    /* Get FT module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_LATENCY_MONITOR, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    /* Sync FT scache v0 */
    _bcm_th3_latency_monitor_scache_v0_sync(unit, &scache);

    return BCM_E_NONE;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

