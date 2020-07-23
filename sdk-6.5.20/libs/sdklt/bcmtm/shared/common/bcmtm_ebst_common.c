/*! \file bcmtm_ebst_common.c
 *
 * TM EBST common functional things.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>

/******************************************************************************
 * Local definitions
 */
static bcmtm_ebst_dev_info_t *bcmtm_ebst_dev_info[BCMDRD_CONFIG_MAX_UNITS];

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM
#define EBST_MSGQ_SIZE          10
#define EBST_MSGQ_TIME_OUT      10000000
#define EBST_THRD_TIME_OUT      10000000
#define FIFO_POP_CMD_MSG        0x2a

typedef struct mem_info_s {
    uint32_t acctype;
    uint32_t offset;
    int index;
    int blknum;
    int lane;
} mem_info_t;

#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)

/******************************************************************************
 * Private functions
 */
/*!
 * \brief free memory for device specific TM EBST driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static void
bcmtm_ebst_dev_info_free(int unit)
{
    if (bcmtm_ebst_dev_info[unit]){
        sal_free(bcmtm_ebst_dev_info[unit]);
        bcmtm_ebst_dev_info[unit] = NULL;
    }
}

/*!
 * \brief EBST schan cmic related settings.
 *
 * \param [in] unit Logical unit number.
 * \param [in] opcode Operational code.
 * \param [in] dst_blk Destination block.
 * \param [in] acc_type Access type.
 * \param [in] data_len Data length
 * \param [in] dma Dma settings.
 * \param [in] ignore_mask Mask to ignore.
 */
static uint32_t
cmicx_schan_opcode_set(int unit, int op_code, int dst_blk, int acc_type,
                       int data_len, int dma, uint32_t ignore_mask)
{
    uint32_t oc = 0;

    SCMH_OPCODE_SET(oc, op_code);
    SCMH_DSTBLK_SET(oc, dst_blk);
    SCMH_ACCTYPE_SET(oc, acc_type);
    SCMH_DATALEN_SET(oc, data_len);
    SCMH_DMA_SET(oc, dma);
    SCMH_BANK_SET(oc, ignore_mask);

    return oc;
}


/*!
 * \brief TM_EBST_DATA LT imm updates.
 * The entries from EBST FIFO is read by EBST thread and based on the
 * outputs it populates the data into the setion allocated for the
 * corresponding scan mode and entities.
 *
 * \param [in] unit Logical unit number.
 * \param [in] buffer_pool Buffer Pool.
 * \param [in] map idx Mapping index.
 * \param [in] data EBST data to be updated.
 *
 * \retval SHR_E_NONE No error. Returns corresponding error code in case
 * of errors.
 */
static int
bcmtm_ebst_data_lt_update(int unit,
                          int buffer_pool,
                          uint32_t map_idx,
                          bcmtm_ebst_data_t *data)
{
    bcmtm_ebst_entity_map_t *map = NULL;
    int *write_ptr, rv = SHR_E_NONE;
    bcmltd_fields_t in;
    bcmltd_fid_t fid = 0;
    bcmltd_sid_t ltid = TM_EBST_DATAt;
    const bcmlrd_field_data_t *field;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_map_info_t *map_info;
    uint64_t fval = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    map_info = &(ebst_info->map_info);

    sal_mutex_take(map_info->lock, SAL_MUTEX_FOREVER);

    switch (data->type) {
        case EBST_SCAN_MODE_QUEUE:
            map = &(map_info->q_map[map_idx]);
            break;
        case EBST_SCAN_MODE_PORTSP_UC:
            map = &(map_info->portsp_map[map_idx].uc_sp);
            break;
        case EBST_SCAN_MODE_PORTSP_MC:
            map = &(map_info->portsp_map[map_idx].mc_sp);
            break;
        case EBST_SCAN_MODE_SP:
            map = &(map_info->sp_map[map_idx]);
            break;
        default:
            break;
    }

    /* No mapping found. */
    SHR_NULL_CHECK(map, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_EBST_DATAt_FIELD_COUNT, &in));
    in.count = 0;
    /* Get the last updated entry. */
    write_ptr = &(map->write_ptr[buffer_pool]);

    /* incrementing the entry to be updated. */
    if ((*write_ptr == -1) || (++(*write_ptr) > map->max_id)) {
        *write_ptr = map->base_id;
    }

    while ( fid < TM_EBST_DATAt_FIELD_COUNT) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, ltid, fid, &field)) {
            fid++;
            continue;
        }
        switch (fid) {
            case TM_EBST_DATAt_BUFFER_POOLf:
                fval = buffer_pool;
                break;
            case TM_EBST_DATAt_TM_EBST_DATA_IDf:
                fval = *write_ptr;
                break;
            case TM_EBST_DATAt_TIMESTAMPf:
                fval = data->timestamp;
                break;
            case TM_EBST_DATAt_CELLSf:
                fval = data->count;
                break;
            case TM_EBST_DATAt_GREEN_DROPf:
                fval = data->green_drop_state;
                break;
            case TM_EBST_DATAt_YELLOW_DROPf:
                fval = data->yellow_drop_state;
                break;
            case TM_EBST_DATAt_RED_DROPf:
                fval = data->red_drop_state;
                break;
            default:
                break;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0, fval));
        fid++;
    }

    rv = bcmimm_entry_update(unit, 0, ltid, &in);
    if (rv == SHR_E_NOT_FOUND || rv == SHR_E_EMPTY) {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    sal_mutex_give(map_info->lock);
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function determines the write pointers to be updated
 * based on the scan mode.
 *
 * \param [in] unit Logical unit number.
 * \param [in] buffer_pool Buffer pool.
 * \param [in] entry EBST Entry.
 *
 * \retval SHR_E_NONE No error. Returns error code in case of errors.
 */
static int
bcmtm_ebst_data_update(int unit, int buffer_pool, uint32_t *entry)
{

    bcmtm_ebst_data_t data;
    uint32_t entity;
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;
    bcmtm_ebst_scan_mode_t scan_mode;
    bcmtm_ebst_dev_info_t *ebst_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INTERNAL);
    SHR_IF_ERR_EXIT(drv->ebst_drv_get(unit, &ebst_drv));

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    scan_mode = ebst_info->scan_mode;
    if (ebst_drv.ebst_data_parse) {
        SHR_IF_ERR_EXIT
            (ebst_drv.ebst_data_parse(unit, entry, scan_mode, &entity, &data));
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_data_lt_update(unit, buffer_pool, entity, &data));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief BCMTM EBST buffer process.
 *
 * \param [in] unit Logical unit number.
 * \param [in] buffer_pool Buffer pool.
 * \param [in] ptr Start pointer for fifo dma.
 * \param [in] num Number of entries.
 */
static void
bcmtm_ebst_process_buffer(int unit,
                          int buffer_pool,
                          uint32_t ptr,
                          uint32_t num)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    uint32_t *ebst_fifodma_entry, *buf;
    bcmbd_fifodma_data_t *data;
    uint32_t i, ptr_end;

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    if (ptr >= ebst_info->num_entries) {
        return;
    }

    if ((ptr + num) < ebst_info->num_entries) {
        ptr_end = num;
    } else {
        ptr_end = ebst_info->num_entries - ptr;
    }

    data = &ebst_info->fifodata[buffer_pool].ebst_data;
    buf = ebst_info->fifodata[buffer_pool].buffer;

    for (i = 0; i < ptr_end; i++) {
        ebst_fifodma_entry = buf + (ptr + i) * data->data_width;
        bcmtm_ebst_data_update(unit, buffer_pool, ebst_fifodma_entry);
    }

    /* FIFO wrap around */
    if (i == ebst_info->num_entries) {
        for (i = 0; i < (num - (ptr_end - ptr)); i++) {
            ebst_fifodma_entry = buf + (i) * data->data_width;
            bcmtm_ebst_data_update(unit, buffer_pool, ebst_fifodma_entry);
        }
    }
}

/*!
 * \brief EBST FIFODMA handler.
 *
 * This function receives information from message queue and takes corresponding
 * action. It populates data logical table for EBST_MSG_DMA, stops the thread
 * for EBST_MSG_EXIT_THREAD and update control status table when EBST_FIFO_FULL
 * is received.
 *
 * \param [in] tc Thread control.
 * \param [in] vp Argument input for the function.
 */
static void
bcmtm_ebst_fifodma_handler(shr_thread_t tc, void *vp)
{
    int unit = (int)(unsigned long)vp;
    bcmtm_ebst_dev_info_t *ebst_info;
    int rv, buffer_pool;
    uint32_t ptr, num = 0;
    bcmtm_ebst_msgq_notif_t notif;
    int ebst_buffer_full[MAX_BUFFER_POOL] = {0};

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    while (true) {
        rv = sal_msgq_recv(ebst_info->msgq_intr, &notif, SAL_MSGQ_FOREVER);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        buffer_pool = notif.buffer_pool;
        switch (notif.msg) {
            case EBST_MSG_FIFODMA:
                rv = bcmbd_fifodma_pop(unit, ebst_info->chan[buffer_pool],
                        0, &ptr, &num);
                if (SHR_FAILURE(rv) && (rv != SHR_E_EMPTY)) {
                    break;
                }
                bcmtm_ebst_process_buffer(unit, buffer_pool, ptr, num);
                break;

            case EBST_MSG_EXIT_THREAD:
                return;
            case EBST_FIFO_FULL:
                ebst_buffer_full[buffer_pool] = 1;
                bcmtm_ebst_status_update(unit, EBST_ENABLE_MODE_FULL,
                        ebst_buffer_full);
                break;

            default:
                break;
        }
    }
}

/*!
 * \brief Free the data mapping resources.
 *
 * \param [in] unit  Logical unit number.
 */
static void
bcmtm_ebst_data_map_free(int unit)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_map_info_t *map_info;

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    map_info = &(ebst_info->map_info);
    sal_mutex_take(map_info->lock, SAL_MUTEX_FOREVER);

    if (map_info->q_map) {
        sal_free(map_info->q_map);
        map_info->q_map = NULL;
    }
    if (map_info->portsp_map) {
        sal_free(map_info->portsp_map);
        map_info->portsp_map = NULL;
    }
    if (map_info->sp_map) {
        sal_free(map_info->sp_map);
        map_info->sp_map = NULL;
    }
    sal_mutex_give(map_info->lock);
}


/*!
 * \brief FIFODMA interrupt callback.
 * This function is called when EBST threshold is reached. The interrupt
 * received is posted in message queue which is processed by EBST thread.
 *
 * \param [in] unit Logical unit number.
 * \param [in] cb_data Callback data.
 */
static void
bcmtm_ebst_cmicx_fifodma_intr_cb(int unit, void *cb_data)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_msgq_notif_t notif;
    int buffer_pool;

    buffer_pool = (int)(unsigned long)cb_data;

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    if (ebst_info->start[buffer_pool]) {
        notif.msg = EBST_MSG_FIFODMA;
        notif.buffer_pool = buffer_pool;
        if (sal_msgq_post(ebst_info->msgq_intr, &notif, 0, EBST_MSGQ_TIME_OUT) ==
                    SAL_MSGQ_E_TIMEOUT) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META( "EBST_MSG_FIFODMA Timeout for Buffer pool: %d\n"),
                                buffer_pool));
        }
    }
}

/*!
 * \brief FIFO DMA work buffer create.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 */
static int
bcmtm_ebst_cmicx_fifodma_work_create(int unit)
{
    const bcmdrd_chip_info_t *cinfo;
    int idx;
    bcmdrd_sym_info_t sinfo;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmbd_fifodma_data_t *data;
    bcmbd_fifodma_work_t *work;
    int blktype;
    mem_info_t mi;
    uint32_t pipe_info, sect_size;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }
    SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, ebst_info->fifo_sid, &sinfo));

    /* Initialize fifodma work. */
    blktype = bcmdrd_pt_blktype_get(unit, sinfo.sid, 0);
    mi.acctype = BCMBD_CMICX_BLKACC2ACCTYPE(sinfo.blktypes);
    mi.index = sinfo.index_min;
    mi.blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);
    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        mi.lane = -1;
    }
    pipe_info = bcmbd_cmicx_pipe_info(unit, sinfo.offset,
                                      mi.acctype, blktype, -1);

    sect_size = BCMBD_CMICX_PIPE_SECT_SIZE(pipe_info);
    for (idx = 0; idx < ebst_info->max_buffer_pool; idx++) {
        mi.offset = sinfo.offset + (idx * sect_size);
        data = &ebst_info->fifodata[idx].ebst_data;
        data->start_addr =
            bcmbd_block_port_addr(unit, mi.blknum, mi.lane, mi.offset, mi.index);
        data->data_width = sinfo.entry_wsize;

        data->op_code =
            cmicx_schan_opcode_set(unit, FIFO_POP_CMD_MSG,
                    mi.blknum, mi.acctype,
                    sinfo.entry_wsize * sizeof(uint32_t), 0, 0);

        data->num_entries = ebst_info->num_entries;
        data->thresh_entries = ebst_info->thd_entries;
        data->cmd_spacing = bcmbd_mor_clks_read_get(unit, sinfo.sid);
        work = &ebst_info->fifodata[idx].ebst_work;
        work->data = data;
        work->cb = bcmtm_ebst_cmicx_fifodma_intr_cb;
        work->cb_data = (void*)(unsigned long)idx;
        work->flags = FIFODMA_WF_INT_MODE | FIFODMA_WF_IGN_EARLYACK;
        SHR_IF_ERR_EXIT
            (bcmbd_fifodma_start(unit, ebst_info->chan[idx], work));
        ebst_info->fifodata[idx].buffer = work->buf_cache;
        ebst_info->start[idx] = TRUE;
    }
exit:
    if (SHR_FUNC_ERR()) {
        for (idx = 0; idx < ebst_info->max_buffer_pool; idx++) {
            bcmbd_fifodma_stop(unit, ebst_info->chan[idx]);
            ebst_info->start[idx] = 0;
        }
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_ebst_start(int unit)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_drv_t *drv;
    bcmtm_ebst_drv_t ebst_drv;
    bool psim, asim;

    SHR_FUNC_ENTER(unit);

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    if (psim || asim) {
        SHR_EXIT();
    }
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (drv->ebst_drv_get(unit, &ebst_drv));

    SHR_IF_ERR_EXIT(bcmtm_ebst_cmicx_fifodma_work_create(unit));
    ebst_info->msgq_intr =
        sal_msgq_create(sizeof(bcmtm_ebst_msgq_notif_t),
                        EBST_MSGQ_SIZE, "bcmtmEbstMsgQ");
    SHR_NULL_CHECK(ebst_info->msgq_intr, SHR_E_MEMORY);

    ebst_info->pid = shr_thread_start("bcmtmEbstThread",
                                       SAL_THREAD_PRIO_DEFAULT,
                                       bcmtm_ebst_fifodma_handler,
                                       (void *)(unsigned long)(unit));
    if (ebst_info->pid == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Allocate the data map for EBST. */
    if (ebst_drv.ebst_map_alloc) {
        SHR_IF_ERR_EXIT
            (ebst_drv.ebst_map_alloc(unit));
    }
    /* Populate the data to the data map. */
    SHR_IF_ERR_EXIT
        (bcmtm_ebst_q_map_populate(unit, TM_EBST_UC_Qt));
    SHR_IF_ERR_EXIT
        (bcmtm_ebst_q_map_populate(unit, TM_EBST_MC_Qt));
    SHR_IF_ERR_EXIT
        (bcmtm_ebst_portsp_map_populate(unit, TM_EBST_PORT_SERVICE_POOLt));
exit:
    if (SHR_FUNC_ERR()) {
        bcmtm_ebst_fifodma_stop(unit, true);
    }
    SHR_FUNC_EXIT();
}

void
bcmtm_ebst_fifodma_stop(int unit, bool resource_free)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    bool psim, asim;
    int idx;

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    if (psim || asim) {
        return;
    }

    /* Get the number of buffer pool. */
    for (idx = 0; idx < ebst_info->max_buffer_pool; idx++) {
        if (ebst_info->start[idx]) {
            ebst_info->start[idx] = 0;
            bcmbd_fifodma_stop(unit, ebst_info->chan[idx]);
        }
    }

    if (ebst_info->pid) {
        bcmtm_ebst_msgq_notif_t notif;
        notif.msg = EBST_MSG_EXIT_THREAD;
        if (sal_msgq_post(ebst_info->msgq_intr, &notif, 0,
                    EBST_MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "EBST message queue post timeout.\n")));
        }

        if (SHR_FAILURE(shr_thread_stop(ebst_info->pid, 500000))) {
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,
                            "EBST thread exit error. \n")));
        }
        ebst_info->pid = NULL;
    }
    /*
     * Free up the thread by sending message to the thread to shut down the
     * thread. Then free the ebst data map.
     */
    if (resource_free) {
        bcmtm_ebst_data_map_free(unit);
    }
    if (ebst_info->msgq_intr) {
        sal_msgq_destroy(ebst_info->msgq_intr);
        ebst_info->msgq_intr = NULL;
    }
}

int
bcmtm_ebst_status_update(int unit,
                         bcmtm_ebst_enable_mode_t mode,
                         int *ebst_full_state)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid = TM_EBST_STATUSt;
    size_t num_fid;
    int buffer_pool, idx;
    int entry_exists = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    /* BUFFER_POOL is considered. */
    fid = TM_EBST_STATUSt_FIFO_FULLf;
    buffer_pool = bcmlrd_field_idx_count_get(unit, ltid, fid);
    num_fid = TM_EBST_STATUSt_FIELD_COUNT + buffer_pool - 1;
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fid, &in));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &in))) {
        entry_exists = 1;
    }

    in.count = 0;
    /* TM_EBST_STATUSt_EBSTf */
    fid = TM_EBST_STATUSt_EBSTf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, mode));

    /* TM_EBST_STATUSt_FIFO_FULLf */
    if (ebst_full_state) {
        fid = TM_EBST_STATUSt_FIFO_FULLf;
        for (idx = 0; idx < buffer_pool; idx++) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, idx, ebst_full_state[idx]));
        }
    }

    if (entry_exists) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, 0, ltid, &in));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

int
bcmtm_ebst_cleanup(int unit)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    int idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    if (ebst_info == NULL) {
        SHR_EXIT();
    }

    for (idx = 0; idx < ebst_info->max_buffer_pool; idx++) {
        if (ebst_info->start[idx]) {
            bcmtm_ebst_fifodma_stop(unit, TRUE);
            break;
        }
    }
    if (ebst_info->map_info.lock != NULL) {
        sal_mutex_destroy(ebst_info->map_info.lock);
    }
    bcmtm_ebst_dev_info_free(unit);
exit:
    SHR_FUNC_EXIT();
}

void
bcmtm_ebst_intr_notify(int unit, uint32_t intr_param)
{
    uint32_t intr_inst, intr_num;
    bcmtm_ebst_msgq_notif_t notif;
    int rv;
    bcmtm_ebst_dev_info_t *ebst_info;

    intr_num = MMU_INTR_NUM_GET(intr_param);
    intr_inst = MMU_INTR_INST_GET(intr_param);
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    (void) bcmbd_mmu_intr_disable(unit, intr_num);
    notif.msg = EBST_FIFO_FULL;
    notif.buffer_pool = intr_inst;

    rv = sal_msgq_post(ebst_info->msgq_intr, &notif, 0, EBST_MSGQ_TIME_OUT);
    if (rv == SAL_MSGQ_E_TIMEOUT) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "[EBST_MSG_FIFO_FULL] message timeout")));
    }
}

int
bcmtm_ebst_init(int unit, bool warm)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_enable_mode_t enable_mode;
    bcmtm_ebst_control_cfg_t ebst_cfg;
    int ebst_buffer_full[MAX_BUFFER_POOL] = {0};

    SHR_FUNC_ENTER(unit);

    enable_mode = EBST_ENABLE_MODE_OFF;
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    /* setting thread id to invalid. */
    ebst_info->pid = NULL;

    if (ebst_info->map_info.lock == NULL) {
        ebst_info->map_info.lock = sal_mutex_create("bcmtmEbstDataLock");
        SHR_NULL_CHECK(ebst_info->map_info.lock, SHR_E_MEMORY);
    }
    if (warm) {
        sal_memset(&ebst_cfg, 0, sizeof(bcmtm_ebst_control_cfg_t));
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_control_imm_lkup(unit, &ebst_cfg));
        if (ebst_cfg.enable_mode != EBST_ENABLE_MODE_OFF) {
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_start(unit));
            ebst_info->enable_mode = ebst_cfg.enable_mode;
            ebst_info->scan_mode = ebst_cfg.scan_mode;
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_status_update(unit, enable_mode, ebst_buffer_full));
    }
exit:
    if (SHR_FUNC_ERR()) {
        (void)(bcmtm_ebst_cleanup(unit));
    }
    SHR_FUNC_EXIT();
}

void
bcmtm_ebst_dev_info_get(int unit, bcmtm_ebst_dev_info_t **ebst_dev_info)
{
    *ebst_dev_info = bcmtm_ebst_dev_info[unit];
}

int
bcmtm_ebst_dev_info_alloc(int unit)
{
    bcmtm_ebst_dev_info_t *ebst_dev_info = NULL;
    uint32_t size = sizeof(bcmtm_ebst_dev_info_t);

    SHR_FUNC_ENTER(unit);

    ebst_dev_info = sal_alloc(size, "bcmtmEbstDevInfo");
    SHR_NULL_CHECK(ebst_dev_info, SHR_E_MEMORY);

    sal_memset(ebst_dev_info, 0, sizeof(bcmtm_ebst_dev_info_t));
    bcmtm_ebst_dev_info[unit] = ebst_dev_info;
exit:
    SHR_FUNC_EXIT();
}
