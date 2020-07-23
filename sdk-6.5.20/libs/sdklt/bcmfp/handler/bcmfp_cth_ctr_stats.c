/*! \file bcmfp_cth_ctr_stats.c
 *
 * APIs FP counter LTs and flex counters parameters.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate counter config provided
 * using counter template LTs.
 *
 * This file also contains functions to process
 * flex counter parameters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmfp/bcmfp_ctr_egr_stat.h>
#include <bcmptm/bcmptm.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_stat_lookup(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  uint8_t pipe,
                  bcmfp_stage_id_t stage_id,
                  uint32_t hw_index,
                  uint64_t *byte_count,
                  uint64_t *packet_count)
{
    uint32_t rsp_flags;
    uint32_t rsp_entry_wsize;
    uint32_t entry_sz;
    uint32_t *pt_entry_buff = NULL;
    uint32_t pipe_map;
    uint64_t pkt_val;
    uint64_t byte_val;
    uint64_t byte_counter = 0;
    uint64_t packet_counter = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid;
    bcmltd_sid_t ctr_stat_ltid;
    bcmdrd_sid_t drd_sid;
    bcmfp_stage_oper_mode_t oper_mode;
    bcmfp_stage_t *stage = NULL;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(byte_count, SHR_E_PARAM);
    SHR_NULL_CHECK(packet_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    
    entry_sz = 5;
    entry_sz *= 4;
    SHR_ALLOC(pt_entry_buff, entry_sz , "bcmfpCtrEgrFpStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, entry_sz);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dev_valid_pipes_get(unit,
                                   stage->stage_id,
                                   &pipe_map));

    drd_sid = stage->tbls.ctr_stat_tbl->counter_sid;
    ctr_stat_ltid = stage->tbls.ctr_stat_tbl->sid;
    
    rsp_entry_wsize = 5;

    /* Initialize physical table index */
    pt_dyn_info.index = hw_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
    /* read specified pipe only */
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
       pipe_map &= 1 << pipe;
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    pipe = 0;
    while (pipe_map) {
        if (pipe_map & 1) {
            pt_dyn_info.tbl_inst = pipe;
            /* Read counter table. */
            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_read(unit,
                                      req_flags,
                                      drd_sid,
                                      &pt_dyn_info,
                                      NULL,
                                      rsp_entry_wsize,
                                      ctr_stat_ltid,
                                      pt_entry_buff,
                                      &rsp_ltid,
                                      &rsp_flags));
            pkt_val = (uint64_t)*pt_entry_buff |
                      (uint64_t)*(pt_entry_buff + 1) << 32;
            byte_val = (uint64_t)*(pt_entry_buff+2) |
                       (uint64_t)*(pt_entry_buff + 3) << 32;

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "index %d  pipe %d "
                        "pkt_val = %lu byte_val = %lu\n"),
                        hw_index, pipe,
                        (unsigned long int)pkt_val,
                        (unsigned long int)byte_val));

            packet_counter += pkt_val;
            byte_counter += byte_val;
            /* Reset the counter value in hardware to 0 */
            sal_memset(pt_entry_buff, 0, entry_sz);
            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_write(unit,
                                       req_flags,
                                       drd_sid,
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       ctr_stat_ltid,
                                       &rsp_ltid,
                                       &rsp_flags));
        }
        pipe++;
        pipe_map >>= 1;
    }
    *packet_count = packet_counter;
    *byte_count = byte_counter;

exit:
    SHR_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcmfp_stat_update(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmfp_stage_t *stage,
                  const bcmltd_fields_t *in,
                  const bcmltd_generic_arg_t *arg)
{
    uint32_t i = 0;
    uint32_t ctr_stat_id = 0;
    bcmltd_field_t *fld;
    bool update_pkt_cnt = false;
    bool update_byte_cnt = false;
    uint32_t hw_index = 0;
    uint32_t rsp_flags;
    uint32_t entry_sz;
    uint32_t rsp_entry_wsize;
    uint32_t *pt_entry_buff = NULL;
    uint32_t pipe_map;
    uint8_t pipe = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmfp_stage_oper_mode_t oper_mode;
    bcmltd_sid_t rsp_ltid;
    bcmdrd_sid_t drd_sid = 0;
    bcmltd_sid_t ctr_stat_sid = 0;
    bcmltd_fid_t stat_id_fid = 0;
    bcmltd_fid_t byte_count_fid = 0;
    bcmltd_fid_t pkt_count_fid = 0;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    pkt_count_fid = stage->tbls.ctr_stat_tbl->pkt_count_fid;
    byte_count_fid = stage->tbls.ctr_stat_tbl->byte_count_fid;
    drd_sid = stage->tbls.ctr_stat_tbl->counter_sid;
    ctr_stat_sid = stage->tbls.ctr_stat_tbl->sid;
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;
    stage_id = stage->stage_id;

    /*
     * Set PACKET_COUNT and BYTE_COUNT values in the PT
     * entry buffer.
     */
    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
            ctr_stat_id = fld->data;
        } else if (fld->id == pkt_count_fid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ctr_stat_pkt_count_set(unit,
                                              stage_id,
                                              ctr_stat_id,
                                              fld->data));
            /* Reset byte counter value to 0 in hardware */
            update_pkt_cnt = true;
        } else if (fld->id == byte_count_fid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ctr_stat_byte_count_set(unit,
                                               stage_id,
                                               ctr_stat_id,
                                               fld->data));
            /* Reset pkt counter value to 0 in hardware */
            update_byte_cnt = true;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));

    SHR_IF_ERR_VERBOSE_EXIT
          (bcmfp_ctr_entry_remote_config_get(unit,
                                             op_arg,
                                             stage_id,
                                             ctr_stat_id,
                                             &ctr_entry_config));
    if (ctr_entry_config == NULL) {
        /*
         * Return without updating the HW as the CTR_EGR_FP_ENTRY
         * is not inserted
         */
        SHR_EXIT();
    }
    pipe = ctr_entry_config->pipe_id;

    
    entry_sz = 5;
    entry_sz *= 4;

    SHR_ALLOC(pt_entry_buff, entry_sz, "bcmfpCtrEgrFpStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dev_valid_pipes_get(unit,
                                   stage->stage_id,
                                   &pipe_map));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_hw_index_get(unit, stage_id, ctr_stat_id, &hw_index));
    /* Initialize physical table index */
    pt_dyn_info.index = hw_index;

    /* read specified pipe only */
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
       pipe_map &= (1 << pipe);
    }
    pipe = 0;
    rsp_entry_wsize = 5;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    while (pipe_map) {
        if (pipe_map & 1) {
            pt_dyn_info.tbl_inst = pipe;

            /* Get the current BYTE/PKT count */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_ireq_read(unit,
                                      req_flags,
                                      drd_sid,
                                      &pt_dyn_info,
                                      NULL,
                                      rsp_entry_wsize,
                                      ctr_stat_sid,
                                      pt_entry_buff,
                                      &rsp_ltid,
                                      &rsp_flags));
            /*
             * Reset the HW value pf pkt/byte count
             * to 0 on updating them.
             */
            if (update_pkt_cnt)  {
                pt_entry_buff[0] = 0;
                pt_entry_buff[1] = 0;
            }
            if (update_byte_cnt) {
                pt_entry_buff[2] = 0;
                pt_entry_buff[3] = 0;
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "Reset HW value of ctr_stat_id %d,"
                        "  pipe %d \n"), ctr_stat_id, pipe));
            /* Write back counter table. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_ireq_write(unit,
                                       req_flags,
                                       drd_sid,
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       ctr_stat_sid,
                                       &rsp_ltid,
                                       &rsp_flags));
        }
        pipe++;
        pipe_map >>= 1;
    }

    SHR_EXIT();

exit:
    SHR_FREE(pt_entry_buff);
    bcmfp_ctr_entry_config_free(unit, ctr_entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_id_set(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint32_t ctr_stat_id,
                      bool set)
{
    uint32_t offset = 0;
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint32_t *ctr_egr_fp_stat_id_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_egr_stats_id_bmp_seg;
    ctr_egr_fp_stat_id_bmp =  (uint32_t *)((uint8_t *)ha_mem + offset);
    if (set == TRUE)  {
        SHR_BITSET(ctr_egr_fp_stat_id_bmp, ctr_stat_id - 1);
    } else  {
        SHR_BITCLR(ctr_egr_fp_stat_id_bmp, ctr_stat_id - 1);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_id_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint32_t ctr_stat_id)
{
    uint32_t offset = 0;
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint32_t *ctr_egr_fp_stat_id_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_egr_stats_id_bmp_seg;
    ctr_egr_fp_stat_id_bmp =  (uint32_t *)((uint8_t *)ha_mem + offset);
    if (!(SHR_BITGET(ctr_egr_fp_stat_id_bmp, ctr_stat_id - 1))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_byte_count_get(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t ctr_id,
                              uint64_t *byte_count)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    void *ha_mem = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint64_t *ctr_egr_stats_byte_count = NULL;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
        ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_egr_stat_byte_count_map_seg;
    ctr_egr_stats_byte_count =  (uint64_t *)((uint8_t *)ha_mem + offset);
    *byte_count = ctr_egr_stats_byte_count[ctr_id - 1];

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_byte_count_set(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t ctr_id,
                              uint64_t byte_count)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    void *ha_mem = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint64_t *ctr_egr_stats_byte_count = NULL;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
        ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_egr_stat_byte_count_map_seg;
    ctr_egr_stats_byte_count =  (uint64_t *)((uint8_t *)ha_mem + offset);
    ctr_egr_stats_byte_count[ctr_id - 1] = byte_count;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_ctr_stat_pkt_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t ctr_id,
                             uint64_t *pkt_count)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    void *ha_mem = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint64_t *ctr_egr_stats_pkt_count = NULL;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
        ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_egr_stat_pkt_count_map_seg;
    ctr_egr_stats_pkt_count =  (uint64_t *)((uint8_t *)ha_mem + offset);
    *pkt_count = ctr_egr_stats_pkt_count[ctr_id - 1];

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_pkt_count_set(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t ctr_id,
                             uint64_t pkt_count)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    void *ha_mem = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint64_t *ctr_egr_stats_pkt_count = NULL;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
        ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_egr_stat_pkt_count_map_seg;
    ctr_egr_stats_pkt_count =  (uint64_t *)((uint8_t *)ha_mem + offset);
    ctr_egr_stats_pkt_count[ctr_id - 1] = pkt_count;

exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_ctr_stat_hw_get(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      uint32_t hw_index,
                      uint8_t pipe,
                      uint64_t *pkt_count,
                      uint64_t *byte_count)
{
    uint32_t rsp_flags;
    uint32_t rsp_entry_wsize;
    uint32_t entry_sz;
    uint32_t *pt_entry_buff = NULL;
    uint32_t pipe_map;
    uint64_t pkt_val;
    uint64_t byte_val;
    uint64_t byte_counter = 0;
    uint64_t packet_counter = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid;
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_oper_mode_t oper_mode;
    bcmltd_sid_t ctr_stat_tbl_id = 0;
    bcmdrd_sid_t ctr_hw_sid;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pkt_count, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(byte_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    *pkt_count = 0;
    *byte_count = 0;

    
    entry_sz = 5;
    entry_sz *= 4;
    SHR_ALLOC(pt_entry_buff, entry_sz , "bcmfpCtrStatsHw");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, entry_sz);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dev_valid_pipes_get(unit,
                                   stage->stage_id,
                                   &pipe_map));

    
    rsp_entry_wsize = 5;

    /* Initialize physical table index */
    pt_dyn_info.index = hw_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));

    /* read specified pipe only */
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
       pipe_map &= (1 << pipe);
    }
    pipe = 0;
    ctr_stat_tbl_id = stage->tbls.ctr_stat_tbl->sid;
    ctr_hw_sid = stage->tbls.ctr_stat_tbl->counter_sid;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    while (pipe_map) {
        if (pipe_map & 1) {
            pt_dyn_info.tbl_inst = pipe;
            /* Read counter table. */
            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_read(unit,
                                      req_flags,
                                      ctr_hw_sid,
                                      &pt_dyn_info,
                                      NULL,
                                      rsp_entry_wsize,
                                      ctr_stat_tbl_id,
                                      pt_entry_buff,
                                      &rsp_ltid,
                                      &rsp_flags));
            pkt_val = (uint64_t)*pt_entry_buff |
                       (uint64_t)*(pt_entry_buff + 1) << 32;
            byte_val = (uint64_t)*(pt_entry_buff+2) |
                      (uint64_t)*(pt_entry_buff + 3) << 32;

            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                        "Index %d " "Pipe %d "
                        "Packets = %lu " "Bytes = %lu\n"),
                        hw_index, pipe,
                        (unsigned long int)pkt_val,
                        (unsigned long int)byte_val));

            packet_counter += pkt_val;
            byte_counter += byte_val;

            /* Reset the counter value in hardware to 0 */
            sal_memset(pt_entry_buff, 0, entry_sz);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_ireq_write(unit,
                                       req_flags,
                                       ctr_hw_sid,
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       ctr_stat_tbl_id,
                                       &rsp_ltid,
                                       &rsp_flags));
        }
        pipe++;
        pipe_map >>= 1;
    }

    *pkt_count = packet_counter;
    *byte_count = byte_counter;

exit:
    SHR_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_insert(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg)
{
    int rv;
    uint32_t ctr_id = 0;
    uint32_t i = 0;
    bcmltd_field_t *fld = NULL;
    bcmltd_fid_t stat_id_fid;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;

    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
            ctr_id = fld->data;
            break;
        }
    }

    /*
     * Return ENTRY_EXISTS error if the stat ID is
     * already inserted.
     */
    rv = bcmfp_ctr_stat_id_get(unit, stage_id, ctr_id);
    if (SHR_SUCCESS(rv)) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stat_update(unit, op_arg, stage, in, arg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_id_set(unit, stage_id, ctr_id, TRUE));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_update(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg)
{
    uint32_t ctr_id = 0;
    uint32_t i = 0;
    bcmltd_field_t *fld = NULL;
    bcmltd_fid_t stat_id_fid;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;

    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
            ctr_id = fld->data;
            break;
        }
    }

    /* Return error if the stat id is not inserted */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_id_get(unit, stage_id, ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stat_update(unit, op_arg, stage, in, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_delete(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg)
{
    uint32_t ctr_id = 0;
    uint32_t i = 0;
    bcmltd_field_t *fld = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fid_t stat_id_fid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;

    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
            ctr_id = fld->data;
            break;
        }
    }

    /* Return if the stat id is not inserted */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_id_get(unit, stage_id, ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_id_set(unit, stage_id, ctr_id, FALSE));

    /* Reset the byte and packet count */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_byte_count_set(unit, stage_id, ctr_id, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_pkt_count_set(unit, stage_id, ctr_id, 0));

exit:
     SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_lookup(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{
    uint32_t ctr_id = 0;
    uint32_t hw_index = 0;
    uint64_t sw_byte_count = 0;
    uint64_t sw_pkt_count = 0;
    uint64_t hw_byte_count = 0;
    uint64_t hw_pkt_count = 0;
    uint32_t i = 0;
    bcmltd_field_t *fld = NULL;
    bcmltd_fid_t pkts_fid;
    bcmltd_fid_t bytes_fid;
    bcmltd_fid_t stat_id_fid;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;
    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
            ctr_id = fld->data;
            break;
        }
    }

    if (out->count < 2) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "out supplied to lookup is too small"
                 " to hold the desired fields.\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_id_get(unit,
                               stage_id,
                               ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       ctr_id,
                                       &ctr_entry_config));
    if (ctr_entry_config != NULL) {
        uint8_t pipe_id = 0;
        /* Get the HW index of the counter. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_hw_index_get(unit,
                                    stage_id,
                                    ctr_id,
                                    &hw_index));

        /* Get the counter values from the HW. */
        pipe_id = ctr_entry_config->pipe_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stat_lookup(unit,
                               op_arg,
                               pipe_id,
                               stage_id,
                               hw_index,
                               &hw_byte_count,
                               &hw_pkt_count));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_byte_count_get(unit,
                                       stage_id,
                                       ctr_id,
                                       &sw_byte_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_pkt_count_get(unit,
                                      stage_id,
                                      ctr_id,
                                      &sw_pkt_count));

    bytes_fid = stage->tbls.ctr_stat_tbl->byte_count_fid;
    pkts_fid = stage->tbls.ctr_stat_tbl->pkt_count_fid;

    /* Update/Add the byte/pkt count read from the HA memory */
    out->count = 0;
    /* Set PACKET_COUNT logical field value. */
    out->field[out->count]->id = pkts_fid;
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = (sw_pkt_count +
                                    hw_pkt_count);
    sw_pkt_count = out->field[out->count++]->data;

    /* Set BYTE_COUNT logical field value. */
    out->field[out->count]->id = bytes_fid;
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = (sw_byte_count +
                                    hw_byte_count);
    sw_byte_count = out->field[out->count++]->data;

    /*
     * As the HW stats value is cleared, set the
     * HA memory with latest value.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_byte_count_set(unit,
                                       stage_id,
                                       ctr_id,
                                       sw_byte_count));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_pkt_count_set(unit,
                                      stage_id,
                                      ctr_id,
                                      sw_pkt_count));

exit:
    bcmfp_ctr_entry_config_free(unit, ctr_entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_validate(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmlt_opcode_t opcode,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t i = 0;
    bcmltd_field_t *fld = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fid_t stat_id_fid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;
    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
            SHR_EXIT();
        }
    }

    /* Return error if key field is missing */
    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_next(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmfp_stage_id_t stage_id,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out,
                    const bcmltd_generic_arg_t *arg)
{
    int rv;
    uint32_t max_ctr_id = 0;
    uint8_t pipe_id = 0;
    uint32_t ctr_id = 0;
    uint32_t hw_index = 0;
    uint64_t sw_byte_count = 0;
    uint64_t sw_pkt_count = 0;
    uint64_t hw_byte_count = 0;
    uint64_t hw_pkt_count = 0;
    uint32_t i = 0;
    bcmltd_field_t *fld = NULL;
    bcmltd_fid_t pkts_fid;
    bcmltd_fid_t bytes_fid;
    bcmltd_fid_t stat_id_fid;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (out->count < 3) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                  "out supplied to egr traverse next is too"
                  "small to hold the desired fields.\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    out->count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;
    for (i = 0; i < in->count; i++) {
        fld = in->field[i];
        if (fld->id == stat_id_fid) {
           ctr_id = fld->data;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_ctr_id_get(unit, stage_id, &max_ctr_id));
    do {
        ctr_id = ctr_id + 1;
        if (ctr_id > max_ctr_id) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        rv = bcmfp_ctr_stat_id_get(unit, stage_id, ctr_id);
    } while (SHR_FAILURE(rv));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       ctr_id,
                                       &ctr_entry_config));
    if (ctr_entry_config != NULL) {
        /* Get the HW index of the counter. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_hw_index_get(unit,
                                    stage_id,
                                    ctr_id,
                                    &hw_index));

        /* Get the counter values from the HW. */
        pipe_id = ctr_entry_config->pipe_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stat_lookup(unit,
                               op_arg,
                               pipe_id,
                               stage_id,
                               hw_index,
                               &hw_byte_count,
                               &hw_pkt_count));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_byte_count_get(unit,
                                       stage_id,
                                       ctr_id,
                                       &sw_byte_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_pkt_count_get(unit,
                                      stage_id,
                                      ctr_id,
                                      &sw_pkt_count));

    bytes_fid = stage->tbls.ctr_stat_tbl->byte_count_fid;
    pkts_fid = stage->tbls.ctr_stat_tbl->pkt_count_fid;

    out->count = 0;
    /* Set PACKET_COUNT logical field value. */
    out->field[out->count]->id = pkts_fid;
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = (sw_pkt_count +
                                    hw_pkt_count);
    sw_pkt_count = out->field[out->count++]->data;

    /* Set BYTE_COUNT logical field value. */
    out->field[out->count]->id = bytes_fid;
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = (sw_byte_count +
                                    hw_byte_count);
    sw_byte_count = out->field[out->count++]->data;

    /* Update the out field with the couter stat ID. */
    out->field[out->count]->id = stat_id_fid;
    out->field[out->count]->data = ctr_id;
    out->count++;

    /*
     * As the HW stats value is cleared, set the
     * HA memory with latest value.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_byte_count_set(unit,
                                       stage_id,
                                       ctr_id,
                                       sw_byte_count));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_pkt_count_set(unit,
                                      stage_id,
                                      ctr_id,
                                      sw_pkt_count));
exit:
    bcmfp_ctr_entry_config_free(unit, ctr_entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_stat_first(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmfp_stage_id_t stage_id,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    bcmltd_fid_t stat_id_fid;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    stat_id_fid = stage->tbls.ctr_stat_tbl->key_fid;
    /* Set stat ID to 0 */
    out->field[0]->data = 0;
    out->field[0]->id = stat_id_fid;
    out->field[0]->idx = 0;
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_ctr_stat_next(unit,
                              op_arg,
                              stage_id,
                              out,
                              out,
                              arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_validate(int unit,
                        bcmlt_opcode_t opcode,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_validate(unit,
                                 BCMFP_STAGE_ID_EGRESS,
                                 opcode,
                                 in,
                                 arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(out);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_insert(unit,
                               op_arg,
                               BCMFP_STAGE_ID_EGRESS,
                               in,
                               arg));
exit:
     SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(out);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_update(unit,
                               op_arg,
                               BCMFP_STAGE_ID_EGRESS,
                               in,
                               arg));
exit:
     SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(out);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_delete(unit,
                               op_arg,
                               BCMFP_STAGE_ID_EGRESS,
                               in,
                               arg));
exit:
     SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_lookup(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_lookup(unit,
                               op_arg,
                               BCMFP_STAGE_ID_EGRESS,
                               in,
                               out,
                               arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_first(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_first(unit,
                              op_arg,
                              BCMFP_STAGE_ID_EGRESS,
                              out,
                              arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_egr_stat_next(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_stat_next(unit,
                             op_arg,
                             BCMFP_STAGE_ID_EGRESS,
                             in,
                             out,
                             arg));
exit:
    SHR_FUNC_EXIT();
}

