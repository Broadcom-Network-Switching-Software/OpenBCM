/*! \file ser_tcam.c
 *
 * Functions for CMIC SER engine.
 *
 * Include CMIC SER engine init configuration etc.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_time.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmcfg/bcmcfg_lt.h>

#include "ser_tcam.h"
#include "ser_intr_dispatch.h"
#include "ser_bd.h"
#include "ser_config.h"


/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/******************************************************************************
 * Private Functions
 */
/*
 * TCAM mode including unique mode and wide mode
 * can be configured during initialization.
 * This routine is used to get and save the mode of the TCAMs.
 */
static bool
bcmptm_ser_tcam_mode_check(int unit, bcmptm_ser_tcam_generic_info_t *tcam_info)
{
    uint64_t fld_val;
    int rv = SHR_E_NONE;

    if (tcam_info->cfg_lt_sid == 0xffffffff ||
        tcam_info->cfg_lt_fid == 0xffffffff) {
        return FALSE;
    }
    rv = bcmcfg_field_get(unit, tcam_info->cfg_lt_sid,
                          tcam_info->cfg_lt_fid, &fld_val);
    if (SHR_FAILURE(rv) || (uint32_t)fld_val != 1) {
        return FALSE;
    }
    return TRUE;
}

/* Get SER check granularity of CMIC SER engine. */
static int
bcmptm_ser_granularity_get(bcmptm_ser_protection_type_t prot_type,
                           bcmptm_ser_protection_mode_t prot_mode,
                           int *ser_mem_gran)
{
    if (prot_type == BCMPTM_SER_TYPE_PARITY) {
        switch (prot_mode) {
            case BCMPTM_SER_PARITY_2BITS:
                *ser_mem_gran = 2;
                break;
            case BCMPTM_SER_PARITY_4BITS:
                *ser_mem_gran = 4;
                break;
            case BCMPTM_SER_PARITY_8BITS:
                *ser_mem_gran = 8;
                break;
            case BCMPTM_SER_PARITY_1BIT:
            default:
                *ser_mem_gran = 1;
                break;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_UNAVAIL;
    }
}

/* Configure SER_ACC_TYPE_MAPm. */
static int
bcmptm_ser_at_map_init(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *mem_info = NULL;
    int  reg_num = 0;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_acc_type_map_info_get(unit, &mem_info, &reg_num);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, mem_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/* Write registers to configure CMIC SER engine for all TCAMs. */
static int
bcmptm_ser_cmic_engine_config(int unit, int hw_ser_ix,
                              bcmptm_ser_tcam_generic_info_t *cur_ser_info,
                              uint32_t mem_start_addr, uint32_t mem_end_addr)
{
    int rv = SHR_E_NONE;
    int i = 0, group_num, blknum, ser_mem_index_num;
    bcmdrd_sid_t range_cfg = INVALIDr, range_start = INVALIDr;
    bcmdrd_sid_t range_end = INVALIDr, range_result = INVALIDr;
    bcmdrd_sid_t *prot_word = NULL;
    bcmdrd_sid_t range_addr_bits = INVALIDr, range_disable = INVALIDr;
    bcmdrd_sid_t ser_memory_sid, sid;
    uint32_t     field_val_list[5];
    bcmdrd_fid_t *range_cfg_flds = NULL, *range_prot_flds = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_cmic_ser_engine_regs_get(unit, &range_cfg, &range_start,
                                             &range_end, &range_result, &prot_word,
                                             &range_addr_bits, &range_disable,
                                             &group_num, &range_cfg_flds,
                                             &range_prot_flds, &ser_memory_sid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_mem_index_num = bcmptm_scor_pt_index_count(unit, ser_memory_sid);
    if (cur_ser_info->ser_section_end >= (uint32_t)ser_mem_index_num) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ser_section_end [%d] is larger than depth [%d]"
                                " of memory [%s]\n"),
                     cur_ser_info->ser_section_end,
                     bcmptm_scor_pt_index_count(unit, ser_memory_sid),
                     bcmdrd_pt_sid_to_name(unit, ser_memory_sid)));
        rv = SHR_E_RESOURCE;
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    /* Clear SER_MEMORYt */
    if (hw_ser_ix == 0) {
        rv = bcmptm_ser_mem_dma_clear(unit, ser_memory_sid, -1, ser_mem_index_num);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if (group_num < hw_ser_ix || hw_ser_ix < 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "hw_ser_ix[%d] is not within"
                                "the range of [0] to [%d]\n"),
                     hw_ser_ix, group_num));
    }
    rv = bcmptm_ser_pt_write(unit, range_start,
                             hw_ser_ix, -1, -1, &mem_start_addr,
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, range_end,
                             hw_ser_ix, -1, -1, &mem_end_addr,
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_RANGE_DISABLE) {
        rv = bcmptm_ser_pt_write(unit, range_addr_bits, hw_ser_ix,
                                 -1, -1, &cur_ser_info->addr_start_bit,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_write(unit, range_disable, hw_ser_ix,
                                 -1, -1, &cur_ser_info->addr_mask,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmptm_ser_pt_write(unit, range_result, hw_ser_ix,
                             -1, -1, &(cur_ser_info->ser_section_start),
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < BCMPTM_NUM_PROT_SECTIONS; i++) {
        if (cur_ser_info->start_end_bits[i].start_bit != -1) {
            sid = prot_word[hw_ser_ix];
            field_val_list[0] = cur_ser_info->start_end_bits[i].start_bit;
            field_val_list[1] = cur_ser_info->start_end_bits[i].end_bit;
            rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, i, range_prot_flds,
                                                    0, field_val_list, NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    sid = range_cfg;
    field_val_list[0] = cur_ser_info->intrlv_mode;
    field_val_list[1] = cur_ser_info->prot_mode;
    field_val_list[2] = cur_ser_info->prot_type;
    field_val_list[3] = (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK)? 1 : 0;

    rv = bcmptm_ser_blknum_get(unit, cur_ser_info->mem, 0, -1, &blknum);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    field_val_list[4] = (uint32_t)blknum;
    rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, hw_ser_ix, range_cfg_flds,
                                            0, field_val_list, NULL);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Range[%2dth](0x%04x, 0x%04x): %s "
                            "Sbus range (0x%08x-0x%08x)\n"),
                 hw_ser_ix, cur_ser_info->ser_section_start,
                 cur_ser_info->ser_section_end,
                 bcmdrd_pt_sid_to_name(unit, cur_ser_info->mem),
                 mem_start_addr, mem_end_addr));

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to configure range[%2dth](0x%04x, 0x%04x) for"
                             " %s owning Sbus range (0x%08x-0x%08x)\n"),
                  hw_ser_ix, cur_ser_info->ser_section_start,
                  cur_ser_info->ser_section_end,
                  bcmdrd_pt_sid_to_name(unit, cur_ser_info->mem),
                  mem_start_addr, mem_end_addr));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_tcam_cmic_scan_init(int unit, bool warm)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL, *cur_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE;
    int info_ix;
    int index_min, index_max, ser_mem_count, ser_mem_gran;
    uint32_t ser_mem_addr = 0, ser_mem_total = 0;
    uint32_t start_addr, end_addr;
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num = 0, intr_num = 0;
    bcmptm_ser_sinfo_t *ser_ha_info = bcmptm_ser_ha_info_get(unit);
    uint32_t ser_range_enable = ser_ha_info->ser_range_enable;
    bool unique_mode;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_tcam_info_get(unit, &tcam_ser_info, &tcam_info_num);
    /* Some chips have no CMIC SER scan engine */
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (warm) {
        rv = bcmptm_ser_cmic_ser_engine_enable(unit, ser_range_enable,
                                               &regs_ctrl_info, &reg_num,
                                               &intr_num);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        bcmptm_ser_intr_enable(unit, intr_num, 1);

        rv = bcmptm_ser_lp_intr_func_set(unit, intr_num, bcmptm_ser_notify, intr_num);
        /* exit */
        SHR_ERR_EXIT(rv);
    }
    /* cold boot */
    rv = bcmptm_ser_at_map_init(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    info_ix = 0;
    while (info_ix < tcam_info_num) {
        cur_ser_info = &(tcam_ser_info[info_ix]);
        if (!bcmdrd_pt_attr_is_cam(unit, cur_ser_info->mem)) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        index_min = bcmptm_scor_pt_index_min(unit, cur_ser_info->mem);
        index_max = bcmptm_scor_pt_index_max(unit, cur_ser_info->mem);
        ser_mem_count = index_max - index_min + 1;
        if (ser_mem_count <= 0) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        rv = bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, -1,
                                    index_min, &start_addr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, -1,
                                    index_max, &end_addr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_granularity_get(cur_ser_info->prot_type,
                                        cur_ser_info->prot_mode,
                                        &ser_mem_gran);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        ser_mem_count *= ser_mem_gran;
        /* Width of each entry of SER_MEMORYm is 32 */
        ser_mem_count = ((ser_mem_count + 31) / 32);

        if (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK) {
            ser_mem_count *= cur_ser_info->num_instances;
        }
        ser_mem_addr = ser_mem_total;
        ser_mem_total += ser_mem_count;

        /* Record section for mem_clear use */
        cur_ser_info->ser_section_start = ser_mem_addr;
        cur_ser_info->ser_section_end = ser_mem_total - 1;
        rv = bcmptm_ser_cmic_engine_config(unit, info_ix, cur_ser_info,
                                           start_addr, end_addr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        ser_range_enable |= 1 << info_ix;
        if (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK) {
            if ((info_ix == 0)) {
                rv = SHR_E_CONFIG;
                SHR_IF_ERR_EXIT(rv);
            }
            unique_mode = bcmptm_ser_tcam_mode_check(unit, cur_ser_info);
            if (unique_mode) {
                /* Disable global protection */
                ser_range_enable &= ~(1 << (info_ix - 1));
            } else {
                /* Disable unique protection */
                ser_range_enable &= ~(1 << info_ix);
            }
        }
        rv = cth_drv->pt_ctrl_imm_update(unit, cur_ser_info->mem,
                                         SER_PT_CONTROLt_ECC_PARITY_CHECKf, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        info_ix++;
    }
    rv =  bcmptm_ser_cmic_ser_engine_enable(unit, ser_range_enable,
                                           &regs_ctrl_info, &reg_num, &intr_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_ha_info->ser_range_enable = ser_range_enable;

    bcmptm_ser_intr_enable(unit, intr_num, 1);

    rv = bcmptm_ser_lp_intr_func_set(unit, intr_num, bcmptm_ser_notify, intr_num);
    SHR_IF_ERR_EXIT(rv);

    ser_ha_info->ser_range_in_usage = ser_range_enable;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to configure CMIC SER engine\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_tcam_fifo_pop(int unit,
                         bcmptm_pre_serc_info_t *serc_info,
                         bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE, err_num = 0;
    bcmptm_ser_ctrl_reg_info_t *ser_error_list;
    bcmptm_ser_ctrl_reg_info_t *ser_result_0, *ser_result_1;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], count = 0;
    bcmptm_ser_tcam_crt_info_t *tcam_crt_info = NULL;

    SHR_FUNC_ENTER(unit);

    tcam_crt_info = (bcmptm_ser_tcam_crt_info_t *)(&crt_info->serc_info);

    rv = bcmptm_ser_cmic_ser_result_get(unit, &ser_error_list, &ser_result_0,
                                        &ser_result_1, &err_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, ser_error_list, err_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(entry_data, 0, sizeof(entry_data));
    crt_info->valid = 1;

    if (ser_error_list[0].ctrl_flds_val[0].rd_val[unit] != 0) {
        rv = bcmptm_ser_ctrl_reg_operate(unit, ser_result_0, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        tcam_crt_info->hw_idx = ser_result_0->ctrl_flds_val[0].rd_val[unit];
        tcam_crt_info->sbus_addr = ser_result_0->ctrl_flds_val[1].rd_val[unit];
        tcam_crt_info->acc_type = ser_result_0->ctrl_flds_val[2].rd_val[unit];
        /* clear ser_result_0m, also clear interrupt */
        rv = bcmptm_ser_pt_write(unit, ser_error_list[0].ctrl_reg,
                                 0, -1, -1, entry_data, 0);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else if (ser_error_list[1].ctrl_flds_val[0].rd_val[unit] != 0) {
        rv = bcmptm_ser_ctrl_reg_operate(unit, ser_result_1, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        tcam_crt_info->hw_idx = ser_result_1->ctrl_flds_val[0].rd_val[unit];
        tcam_crt_info->sbus_addr = ser_result_1->ctrl_flds_val[1].rd_val[unit];
        tcam_crt_info->acc_type = ser_result_1->ctrl_flds_val[2].rd_val[unit];
        /* clear ser_result_1m, also clear interrupt */
        rv = bcmptm_ser_pt_write(unit, ser_error_list[1].ctrl_reg,
                                 0, -1, -1, entry_data, 0);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /* All SER information have read */
        crt_info->valid = 0;
        SHR_EXIT();
    }
    /* Data come from register SER_MISSED_EVENTr */
    count = ser_error_list[2].ctrl_flds_val[0].rd_val[unit];
    if (count != 0) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Overflow events: %d.\n"), count));
    }
exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to pop data from SER_RESULT_0/1m !\n")));
        crt_info->valid = 0;
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_tcam_fifo_entry_parse(int unit,
                                 bcmptm_ser_raw_info_t *crt_info,
                                 bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL, *cur_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE;
    int phys_index_min, phys_index_max;
    int ser_mem_count = 0, pipe_num = -1;
    uint32_t start_addr, end_addr;
    bcmptm_ser_tcam_crt_info_t *tcam_crt_info = NULL;
    int info_idx, acc_type;
    uint32_t sbus_addr;

    SHR_FUNC_ENTER(unit);

    tcam_crt_info = (bcmptm_ser_tcam_crt_info_t *)(&crt_info->serc_info);
    info_idx = (int)tcam_crt_info->hw_idx;
    acc_type = (int)tcam_crt_info->acc_type;
    sbus_addr = tcam_crt_info->sbus_addr;

    rv = bcmptm_ser_tcam_info_get(unit, &tcam_ser_info, &tcam_info_num);
    /* some chips have no CMIC SER scan engine */
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (info_idx >= tcam_info_num) {
        rv = SHR_E_INTERNAL;
        SHR_ERR_EXIT(rv);
    }
    cur_ser_info = &(tcam_ser_info[info_idx]);

    if (!(cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK)) {
        pipe_num = -1;
    } else {
        pipe_num = acc_type;
    }

    phys_index_min = bcmptm_scor_pt_index_min(unit, cur_ser_info->mem);

    phys_index_max = bcmptm_scor_pt_index_max(unit, cur_ser_info->mem);

    ser_mem_count = phys_index_max - phys_index_min + 1;
    if (!ser_mem_count) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER parity failure with unavailable mem range !!\n")));
        rv = SHR_E_FAIL;
        SHR_ERR_EXIT(rv);
    }
    rv = bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, pipe_num,
                               phys_index_min, &start_addr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, pipe_num,
                                phys_index_max, &end_addr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if ((sbus_addr < start_addr) || (sbus_addr > end_addr)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER parity failure with invalid mem SBUS addr[0x%x]"
                             " start addr[0x%x] end addr[0x%x]!!\n"),
                  sbus_addr, start_addr, end_addr));
        rv = SHR_E_FAIL;
        SHR_ERR_EXIT(rv);
    }
    spci->addr[0] = sbus_addr;
    spci->tbl_inst[0] = pipe_num;
    spci->sid[0] = cur_ser_info->mem;
    spci->index[0] = (sbus_addr - start_addr) + phys_index_min;
    spci->flags |= BCMPTM_SER_FLAG_PARITY;
    spci->flags |= BCMPTM_SER_FLAG_TCAM;
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_SBUS;
    spci->blk_type = cur_ser_info->blk_type;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Table[%s] instance[%d] index[%d] error at address 0x%08x\n"),
               bcmdrd_pt_sid_to_name(unit, spci->sid[0]), spci->tbl_inst[0],
               spci->index[0], spci->addr[0]));
exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to parse SER information for TCAM!\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_tcam_pt_ser_enable(int unit, bcmdrd_sid_t sid, int enable)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL, *cur_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE;
    int info_ix = 0;
    uint32_t in_usage = 0;
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num = 0;
    int intr_num = 0;
    bcmptm_ser_sinfo_t *ser_ha_info = bcmptm_ser_ha_info_get(unit);
    uint32_t ser_range_enable = ser_ha_info->ser_range_enable;
    const uint32_t used_ser_range = ser_ha_info->ser_range_in_usage;

    SHR_FUNC_ENTER(unit);

    if (sid == INVALIDm) {
        rv = SHR_E_PARAM;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmptm_ser_tcam_info_get(unit, &tcam_ser_info, &tcam_info_num);
    /* some chips have no CMIC SER scan engine */
    if (rv == SHR_E_UNAVAIL) {
        rv = bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_EC, enable);
        /* exit */
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    while (info_ix < tcam_info_num) {
        cur_ser_info = &(tcam_ser_info[info_ix]);
        in_usage = (used_ser_range & (1 << info_ix)) ? 1 : 0;
        if ((cur_ser_info->mem == sid) && in_usage) {
            break;
        }
        info_ix++;
    }
    if (info_ix == tcam_info_num) {
        rv = SHR_E_UNAVAIL;
        SHR_ERR_EXIT(rv);
    }
    /* Update cache of field SER_RANGE_ENABLEf of reg SER_RANGE_ENABLEr */
    if (enable) {
        ser_range_enable |= 1 << info_ix;
    } else {
        ser_range_enable &= ~(1 << info_ix);
    }
    rv = bcmptm_ser_cmic_ser_engine_enable(unit, ser_range_enable,
                                           &regs_ctrl_info, &reg_num, &intr_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_ha_info->ser_range_enable = ser_range_enable;
exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to enable or disable TCAM SER checking of [%s]!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

