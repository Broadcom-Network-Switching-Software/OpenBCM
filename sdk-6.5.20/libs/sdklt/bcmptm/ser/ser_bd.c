/*! \file ser_bd.c
 *
 * APIs of SER sub-component can write or read H/W by BD routine.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_ser_internal.h>

#include "ser_bd.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/******************************************************************************
 * Private Functions
 */
/*
 * Update multiple fields of one register.
 */
static int
bcmptm_ser_reg_multi_fields_update(int unit, bcmdrd_sid_t sid,
                                   int inst, int index, bcmdrd_fid_t *field_list,
                                   bool en, uint32_t *new_fld_val_list,
                                   uint32_t *old_fld_val_list)
{
    uint32_t fval, fld_bit_num, fld_new_val;
    int rv = SHR_E_NONE, f;
    bcmdrd_sym_field_info_t finfo;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE] = {0};

    SHR_FUNC_ENTER(unit);

    sal_memset(entry_data, 0, sizeof(entry_data));
    rv = bcmptm_ser_pt_read(unit, sid, index, inst, -1, entry_data,
                            BCMDRD_MAX_PT_WSIZE, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (f = 0; ;f++) {
        if (field_list[f] == INVALIDf) {
            break;
        }
        rv = bcmdrd_pt_field_info_get(unit, sid, field_list[f], &finfo);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Can't find field [%d] of reg/mem [%s].\n"),
                      field_list[f],
                      bcmdrd_pt_sid_to_name(unit, sid)));
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        /* Save old field value. */
        if (old_fld_val_list != NULL) {
            rv = bcmdrd_pt_field_get(unit, sid, entry_data, field_list[f], &fval);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            old_fld_val_list[f] = fval;
        }
        /* Write new field value. */
        if (new_fld_val_list == NULL) {
            if (en) {
                fld_bit_num = finfo.maxbit - finfo.minbit + 1;
                fld_new_val = (1 << fld_bit_num) - 1;
            } else {
                fld_new_val = 0;
            }
            fval = fld_new_val;
        } else {
            fval = new_fld_val_list[f];
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Try to modify field [%s] of reg/mem [%s] as [0x%x].\n"),
                     finfo.name,
                     bcmdrd_pt_sid_to_name(unit, sid), fval));
        rv = bcmdrd_pt_field_set(unit, sid, entry_data, field_list[f], &fval);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if (f > 0) {
        rv = bcmptm_ser_pt_write(unit, sid, index, inst, -1, entry_data,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_pt_write(int unit, bcmdrd_sid_t sid,
                    int index, int tbl_inst, int tbl_copy,
                    uint32_t *data, int flags)
{
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_info_t  dyn_info;
    int ovrr_info_inst = -1;
    void *ovrr_info_ptr = NULL;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;
    bool is_port_base_reg = 0, invalid_inst, cache_en;
    int num_inst = 0, copy_num = 0;

    SHR_FUNC_ENTER(unit);

    invalid_inst = bcmptm_ser_pt_inst_remap(unit, sid, &tbl_inst, &tbl_copy);
    if (!invalid_inst) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Table instance [%d] or copy [%d] of [%s] is invalid\n"),
                  tbl_inst, tbl_copy, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    dyn_info.index = index;
    dyn_info.tbl_inst = tbl_inst;

    rv = bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to get access type of PT(%s)\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(rv);
    }
    if (PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
        rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &num_inst, &copy_num);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        if (tbl_copy >= 0) {
            ovrr_info_inst = tbl_copy;
            ovrr_info_ptr = (void *)(&ovrr_info_inst);
        } else if (tbl_inst >= 0 && 0 == copy_num) {
            is_port_base_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, sid);
            if (!is_port_base_reg) {
                ovrr_info_inst = tbl_inst;
                ovrr_info_ptr = (void *)(&ovrr_info_inst);
                dyn_info.tbl_inst = -1;
            }
        }
        if (flags & BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE) {
            ovrr_info_ptr = NULL;
        }
    }
    if ((flags & BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE)) {
        rv = bcmptm_pt_attr_is_cacheable(unit, sid, &cache_en);
        SHR_IF_ERR_EXIT(rv);
        if (cache_en == 0) {
            flags &= ~BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE;
        }
    }
    rv = bcmptm_scor_write(unit, flags, sid, &dyn_info, ovrr_info_ptr, data);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to write memory (%s) instance [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), tbl_inst));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_pt_read(int unit, bcmdrd_sid_t sid,
                   int index, int tbl_inst, int tbl_copy,
                   uint32_t *data, size_t wsize, int flags)
{
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_info_t  dyn_info;
    int ovrr_info_inst = -1;
    void *ovrr_info_ptr = NULL;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;
    bool is_port_base_reg = 0, invalid_inst;
    int num_inst = 0, copy_num = 0;

    SHR_FUNC_ENTER(unit);

    invalid_inst = bcmptm_ser_pt_inst_remap(unit, sid, &tbl_inst, &tbl_copy);
    if (!invalid_inst) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Table instance [%d] or copy [%d] of [%s] is invalid\n"),
                  tbl_inst, tbl_copy, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    dyn_info.index = index;
    dyn_info.tbl_inst = tbl_inst;

    rv = bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to get access type of PT(%s)\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(rv);
    }
    if (PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
        if (!(flags & BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE)) {
            dyn_info.tbl_inst = 0;
        } else {
            rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &num_inst, &copy_num);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            if (tbl_copy >= 0) {
                ovrr_info_inst = tbl_copy;
                ovrr_info_ptr = (void *)(&ovrr_info_inst);
            } else if (tbl_inst >= 0 && 0 == copy_num) {
                is_port_base_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, sid);
                if (!is_port_base_reg) {
                    ovrr_info_inst = tbl_inst;
                    ovrr_info_ptr = (void *)(&ovrr_info_inst);
                    dyn_info.tbl_inst = -1;
                }
            }
        }
    }
    rv = bcmptm_scor_read(unit, flags, sid, &dyn_info, ovrr_info_ptr, wsize, data);
    /* PTcache is not used for SID */
    if ((flags & BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE) == 0 &&
        rv == SHR_E_UNAVAIL) {
        sal_memset(data, 0, 4 * wsize);
        rv = SHR_E_NONE;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "There is no PTcache for PT(%s)\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to read memory(%s), instance [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), tbl_inst));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_tbl_inst_num_get(int unit, bcmdrd_sid_t sid,
                            int *inst_num, int *copy_num)
{
    int rv = SHR_E_NONE;
    const char *acctype_str;
    uint32_t acc_type = 0;
    int is_port_reg = 0;

    SHR_FUNC_ENTER(unit);

    *copy_num = 0;
    rv = bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
    SHR_IF_ERR_EXIT(rv);

    is_port_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, sid);
    if (is_port_reg) {
        *inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
        if (PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
            /* Such as: EGR_1588_LINK_DELAY_64r */
            *copy_num = bcmdrd_pt_num_pipe_inst(unit, sid);
        }
    } else if (PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
        *inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
        if (*inst_num == 1) {
            if (PT_IS_DUPLICATE_2_ACC_TYPE(acctype_str)) {
              *inst_num = bcmdrd_pt_num_pipe_inst(unit, sid)/2;
            } else if (PT_IS_DUPLICATE_4_ACC_TYPE(acctype_str)) {
              *inst_num = bcmdrd_pt_num_pipe_inst(unit, sid)/4;
            } else  {
              *inst_num = bcmdrd_pt_num_pipe_inst(unit, sid);
            }
         } else {
            /* Only for MMU PTs such as MMU_THDM_DB_PORTSP_CONFIGm */
            *copy_num = bcmdrd_pt_num_pipe_inst(unit, sid);
        }

    } else {
        *inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to get instance number of PT [%s]!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_sbusdma_work_exec(int unit, bcmbd_sbusdma_work_t *work,
                             sbusdma_cb_f cb)
{
    int rv = SHR_E_NONE;

    if (work == NULL) {
        return SHR_E_INTERNAL;
    }
    work->state = SBUSDMA_WORK_NULL;
    /* Create the work */
    if (work->items == 1) {
        rv = bcmbd_sbusdma_light_work_init(unit, work);
    } else {
        rv = bcmbd_sbusdma_batch_work_create(unit, work);
    }
    if(SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to init SBUS DMA work.\n")));
        return rv;
    }
    /* Execute work by sync or async mode */
    if (cb == NULL) {
        rv = bcmbd_sbusdma_work_execute(unit, work);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to execute SBUS DMA work.\n")));
            return rv;
        }
        if (work->items > 1) {
            rv = bcmbd_sbusdma_batch_work_delete(unit, work);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to add delete batch work.\n")));
            }
        }
    } else {
        work->cb = cb;
        work->cb_data = work;
        rv = bcmbd_sbusdma_work_add(unit, work);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to add SBUS DMA work to queue.\n")));
        }
    }
    return rv;
}

void
bcmptm_ser_sbusdma_work_cleanup(int unit, bcmbd_sbusdma_work_t *work)
{
    sal_free(work->data);
    work->data = NULL;
    sal_free(work);
    work = NULL;
}

bcmbd_sbusdma_work_t *
bcmptm_ser_sbusdma_work_init(int unit, bcmdrd_sid_t sid, int tbl_inst,
                             int tbl_copy, int *start_indexes, int *index_ranges,
                             int work_num, uint64_t buf_paddr, uint32_t work_flags)
{
    bcmdrd_sym_info_t sinfo = {0};
    int rv = SHR_E_NONE, i = 0;
    int blknum = 0, data_len = 0, size_work_data = 0, size_work = 0;
    uint32_t opcode = 0;
    bcmbd_sbusdma_data_t *work_data = NULL;
    bcmbd_sbusdma_work_t *work = NULL;
    int cmd = 0;
    bool is_port_reg = FALSE;

    SHR_FUNC_ENTER(unit);

    if ((buf_paddr == 0) && ((SBUSDMA_WF_DUMB_READ & work_flags) == 0)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to allocate tcam scan read buffer!\n")));
        rv = SHR_E_MEMORY;
        SHR_IF_ERR_EXIT(rv);
    }
    is_port_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, sid);

    rv = bcmdrd_pt_info_get(unit, sid, &sinfo);
    SHR_IF_ERR_EXIT(rv);

    if (is_port_reg) {
        rv = bcmptm_ser_blknum_get(unit, sid, start_indexes[0], -1, &blknum);
        SHR_IF_ERR_EXIT(rv);
    } else {
        rv = bcmptm_ser_blknum_get(unit, sid, tbl_inst, -1, &blknum);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Structure the work data */
    size_work_data = sizeof(bcmbd_sbusdma_data_t) * work_num;
    work_data = sal_alloc(size_work_data, "bcmptmSerWorkData");
    if (work_data == NULL) {
        rv = SHR_E_MEMORY;
        SHR_IF_ERR_EXIT(rv);
    }
    sal_memset(work_data, 0, size_work_data);

    size_work = sizeof(bcmbd_sbusdma_work_t);
    work = sal_alloc(size_work, "bcmptmSerWork");
    if (work == NULL) {
        rv = SHR_E_MEMORY;
        SHR_IF_ERR_EXIT(rv);
    }
    sal_memset(work, 0, size_work);
    data_len = sizeof(uint32_t) * sinfo.entry_wsize;

    if (is_port_reg) {
        if (work_flags & SBUSDMA_WF_WRITE_CMD) {
            cmd = BCMPTM_SER_WRITE_REGISTER_CMD_MSG;
        } else {
            cmd = BCMPTM_SER_READ_REGISTER_CMD_MSG;
        }
        rv = bcmptm_ser_cmic_schan_opcode_get(unit, cmd, blknum, sinfo.blktypes,
                                              -1, tbl_copy, data_len,
                                              0, 0, &opcode);
        SHR_IF_ERR_EXIT(rv);
    } else {
        if (work_flags & SBUSDMA_WF_WRITE_CMD) {
            cmd = BCMPTM_SER_WRITE_MEMORY_CMD_MSG;
        } else {
            cmd = BCMPTM_SER_READ_MEMORY_CMD_MSG;
        }
        rv = bcmptm_ser_cmic_schan_opcode_get(unit, cmd, blknum, sinfo.blktypes,
                                              tbl_inst, tbl_copy, data_len,
                                              0, 0, &opcode);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Structure the work data */
    for (i = 0; i < work_num; i++) {
        if (is_port_reg) {
            rv = bcmptm_ser_pt_addr_get(unit, sid, start_indexes[i],
                                        0, &(work_data[i].start_addr));
            SHR_IF_ERR_EXIT(rv);
        } else {
            rv = bcmptm_ser_pt_addr_get(unit, sid, tbl_inst, start_indexes[i],
                                        &(work_data[i].start_addr));
            SHR_IF_ERR_EXIT(rv);
        }
        work_data[i].op_count = index_ranges[i];
        work_data[i].op_code = opcode;
        work_data[i].attrs = SBUSDMA_DA_APND_BUF;
        work_data[i].data_width = sinfo.entry_wsize;
        work_data[i].addr_gap = 0;
    }
    work_data[0].buf_paddr = buf_paddr;

    work->data = work_data;
    work->items = work_num;
    work->flags = work_flags;
    /* work.flags |= SBUSDMA_WF_INT_MODE; */

exit:
    if (!SHR_FUNC_ERR()) {
        return work;
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to create DMA work for PT[%s]!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        if (work_data != NULL) {
            sal_free(work_data);
            work_data = NULL;
        }
        if (work != NULL) {
            sal_free(work);
            work = NULL;
        }
        return NULL;
    }
}

int
bcmptm_ser_ctrl_reg_operate(int unit,
                            bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info,
                            int reg_num)
{
    int  reg_idx = 0, write_time = 0;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], fld_idx = 0, fld_value = 0;
    bcmdrd_fid_t fid = 0;
    int  rv = SHR_E_NONE;
    int  index = 0, inst = 0, inst_num = 1, i = 0;
    bcmdrd_sid_t sid = INVALIDm;
    bcmbd_pt_dyn_info_t dyn_info;

    SHR_FUNC_ENTER(unit);

    if (regs_ctrl_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    for (reg_idx = 0; reg_idx < reg_num; reg_idx++) {
        sid = regs_ctrl_info[reg_idx].ctrl_reg;
        if (sid == INVALIDr || sid == INVALIDm) {
            break;
        }
        inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);

        for (i = 0; i < inst_num; i++) {
            if (inst_num == 1) {
                inst = -1;
            } else {
                inst = i;
                if (!bcmdrd_pt_index_valid(unit, sid, inst, 0)) {
                    continue;
                }
            }
            sal_memset(entry_data, 0, sizeof(entry_data));
            index = regs_ctrl_info[reg_idx].ctrl_reg_index;

            if (regs_ctrl_info[reg_idx].ctrl_flds_val[0].rd <= 1) {
                rv = bcmptm_ser_pt_read(unit, sid, index, inst,
                                        -1, entry_data, BCMDRD_MAX_PT_WSIZE,
                                        BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } else {
                /* clear MMU_EBCFG_MEM_FAIL_ADDR_64m */
                dyn_info.tbl_inst = inst;
                dyn_info.index = index;
                (void)bcmbd_pt_pop(unit, sid, &dyn_info, NULL,
                                   entry_data, BCMDRD_MAX_PT_WSIZE);
                /* continue */
                continue;
            }
            write_time = 0;
            /* write fields */
            for (fld_idx = 0; fld_idx < regs_ctrl_info[reg_idx].num_fld; fld_idx++) {
                if (regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].rd != 0) {
                    continue;
                }
                write_time++;
                fid = regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].ctrl_fld;
                fld_value = regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].wr_val;
                rv = bcmdrd_pt_field_set(unit, sid, entry_data, fid, &fld_value);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (write_time) {
                rv = bcmptm_ser_pt_write(unit, sid, index, inst, -1, entry_data,
                                         BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (regs_ctrl_info[reg_idx].utime > 0) {
                sal_usleep(regs_ctrl_info[reg_idx].utime);
            }
            /* read fields */
            sal_memset(entry_data, 0, sizeof(entry_data));
            rv = bcmptm_ser_pt_read(unit, sid, index, inst,
                                    -1, entry_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            for (fld_idx = 0; fld_idx < regs_ctrl_info[reg_idx].num_fld; fld_idx++) {
                if (regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].rd == 0) {
                    continue;
                }
                fld_value = 0;
                fid = regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].ctrl_fld;
                rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid, &fld_value);
                regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].rd_val[unit] = fld_value;
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to write or read controlling register[%s]\n"),
                             bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_reg_multi_fields_modify(int unit, bcmdrd_sid_t sid,
                                   int index, bcmdrd_fid_t *field_list, bool en,
                                   uint32_t *new_fld_val_list,
                                   uint32_t *old_fld_val_list)
{
    int rv = SHR_E_NONE;
    int inst_num = 0, inst = 0;

    SHR_FUNC_ENTER(unit);

    if (field_list == NULL || sid == INVALIDr) {
        return SHR_E_PARAM;
    }
    inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);

    if (inst_num == 1) {
        rv = bcmptm_ser_reg_multi_fields_update(unit, sid, -1, index, field_list,
                                                en, new_fld_val_list,
                                                old_fld_val_list);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /* for unique, for port */
        for (inst = inst_num - 1; inst >= 0; inst--) {
            if (!bcmdrd_pt_index_valid(unit, sid, inst, index)) {
                continue;
            }
            rv = bcmptm_ser_reg_multi_fields_update(unit, sid, inst, index,
                                                    field_list, en,
                                                    new_fld_val_list,
                                                    old_fld_val_list);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to modify reg/mem [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_blknum_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                      int blk_type, int *blk_num)
{
    int blktype_tmp;
    const bcmdrd_chip_info_t *cinfo = NULL;
    bcmdrd_sym_info_t sinfo;
    int port, inst;
    bcmdrd_pblk_t pblk;
    const bcmdrd_symbols_t *symbols = NULL;
    bcmdrd_pipe_info_t drd_pi, *pi = &drd_pi;
    int rv = SHR_E_NONE;
    uint32_t acctype = 0;
    const char *acctype_str = NULL;
    uint32_t linst = 0, pmask = 0;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }
    sal_memset(&sinfo, 0, sizeof(sinfo));

    if (bcmdrd_pt_is_valid(unit, sid)) {
        blktype_tmp = bcmdrd_pt_blktype_get(unit, sid, 0);

        symbols = bcmdrd_dev_symbols_get(unit, 0);
        if (bcmdrd_sym_info_get(symbols, sid, &sinfo) == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        rv = bcmptm_ser_pt_acctype_get(unit, sid, &acctype, &acctype_str);
        SHR_IF_ERR_EXIT(rv);

        sal_memset(pi, 0, sizeof(*pi));
        pi->offset = sinfo.offset;
        pi->acctype = acctype;
        pi->blktype = blktype_tmp;
        pi->baseidx = -1;
        pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
        linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);

        if (pmask != 0 || linst != 0) {
            /* Use instance 0 as block instance for per-pipe table. */
            inst = 0;
        } else if(PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
            /* for ip/ep/mmu_eb duplicate access type memory tables */
            inst = 0;
        } else {
            inst = tbl_inst;
        }
    } else if (blk_type >= 0) {
        blktype_tmp = blk_type;
        inst = tbl_inst;
    } else {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid blktype [%d] and invalid SID [%d]\n"),
                             blk_type, sid));
    }

    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_PORT) {
        if (!bcmdrd_pt_is_valid(unit, sid)) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Invalid port based SID [%d]\n"),
                                 sid));
        }
        /* Iterate over blocks until matching port is found */
        port = tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype_tmp, &pblk) == 0) {
                break;
            }
            blktype_tmp = bcmdrd_pt_blktype_get(unit, sid, blktype_tmp + 1);
        } while (blktype_tmp >= 0);

        if (blktype_tmp < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "port = %d\n"), port));
        }
        *blk_num = pblk.blknum;
    } else {
        if (inst < 0) {
            inst = 0;
        }
        *blk_num = bcmdrd_chip_block_number(cinfo, blktype_tmp, inst);
        if (*blk_num < 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_pt_addr_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                       int index, uint32_t *addr)
{
    int rv = SHR_E_NONE;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_chip_info_t *cinfo = NULL;
    int blknum = 0, lane = -1, offset = 0;
    int port = 0, blktype = 0, blkinst = 0;
    bcmdrd_pblk_t pblk = {0};
    uint32_t linst = 0, pmask = 0, sec_size = 0, sect_shft = 0;
    uint32_t acctype = 0;
    const char *acctype_str = NULL;
    bcmdrd_pipe_info_t drd_pi, *pi = &drd_pi;
    int index_tmp = 0;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    rv = bcmdrd_pt_info_get(unit, sid, sinfo);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid sid [%d]\n"), sid));
    }
    offset = sinfo->offset;
    index_tmp = index * sinfo->step_size;

    rv = bcmptm_ser_pt_acctype_get(unit, sid, &acctype, &acctype_str);
    SHR_IF_ERR_EXIT(rv);

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = offset;
    pi->acctype = acctype;
    pi->blktype = blktype;
    pi->baseidx = -1;

    pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
    linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);
    sect_shft = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_SECT_SHFT);
    sec_size = (sect_shft == 0) ? 0 : (1 << sect_shft);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "blktype = %d, blkinst = %d\n"),
                                 blktype, tbl_inst));
        }
        blknum = pblk.blknum;
        lane = pblk.lane;
    } else {
        if (pmask != 0 || linst != 0) {
            /* Use instance 0 as block instance for per-pipe table. */
            blkinst = 0;
        } else if(PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
            /* for duplicate access type memory tables */
            blkinst = 0;
        } else {
            blkinst = tbl_inst;
        }
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "blknum = %d, blkinst = %d\n"),
                                 blknum, blkinst));
        }
        /*
         * The SIDs with UNIQUE access type and BASE_TYPE instance > 1
         * should have been separate to per-pipe SIDs.
         */
        if (pmask != 0 && linst > 1) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "pmask = %d, linst = %d\n"),
                                 pmask, linst));
        }
        if (pmask == 0) {
            if (linst != 0) {
                lane = tbl_inst;
            } else {
                lane = 0;
            }
        }
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        if (linst != 0 && sec_size != 0) {
           offset += tbl_inst * sec_size;
        }
        lane = -1;
    }
    *addr = bcmbd_block_port_addr(unit, blknum, lane, offset, index_tmp);
exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_ser_pt_copy_no_valid(int unit, bcmdrd_sid_t sid,
                            int tbl_inst, int copy_no)
{
    uint32_t idx_invalid = 0, basetype_pmask = 0;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    int rv = SHR_E_NONE;
    int inst_num = 0, copy_num = 0;
    int map_inst = tbl_inst, map_copy = copy_no;
    bool inst_valid = 0;

    (void)bcmptm_ser_tbl_inst_num_get(unit, sid, &inst_num, &copy_num);

    rv = bcmdrd_pt_info_get(unit, sid, sinfo);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }
    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = sinfo->offset;
    pi->acctype = copy_no;
    pi->blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    pi->baseidx = tbl_inst;

    basetype_pmask = bcmdrd_dev_pipe_info(unit, pi,
                                          BCMDRD_PIPE_INFO_TYPE_BASETYPE_PMASK);
    if (basetype_pmask == 0) {
        /* For IP/EP etc. their basetype_mask is 0x0. */
        if (tbl_inst >= inst_num || copy_no >= copy_num ||
            (copy_num > 0 && copy_no < 0)) {
            return FALSE;
        }
        /* Only for PT which access type is duplicate_2 or duplicate_4 */
        inst_valid = bcmptm_ser_pt_inst_remap(unit, sid, &map_inst, &map_copy);
        if (!inst_valid) {
            return FALSE;
        }
        return (tbl_inst == map_inst && copy_no == map_copy) ? TRUE : FALSE;
    } else {
        /* For SC/XPE/EB/ITM */
        idx_invalid =
            bcmdrd_dev_pipe_info(unit, pi,
                                 BCMDRD_PIPE_INFO_TYPE_BASEIDX_INVALID);
        return (idx_invalid == 0);
    }
}

int
bcmptm_ser_mem_dma_clear(int unit, bcmdrd_sid_t sid, int inst, int index_num)
{
    int  rv = SHR_E_NONE;
    uint64_t dma_addr;
    void *buf_paddr = NULL;
    size_t max_entry_size = 0;
    uint32_t work_flags = 0, ent_size;
    int start_indexes = 0, index_ranges = index_num;
    bcmbd_sbusdma_work_t *dma_work = NULL;
    const uint32_t *default_entry_data = NULL;

    SHR_FUNC_ENTER(unit);

    max_entry_size = BCMDRD_MAX_PT_WSIZE * sizeof(uint32_t);
    buf_paddr = bcmdrd_hal_dma_alloc(unit, max_entry_size, "bcmptmSerDmaClr", &dma_addr);
    if (NULL == buf_paddr) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to allocate DMA buffer used to clear PTs\n")));
        rv = SHR_E_MEMORY;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    sal_memset(buf_paddr, 0, max_entry_size);

    default_entry_data = bcmdrd_pt_default_value_get(unit, sid);
    ent_size = bcmdrd_pt_entry_size(unit, sid);
    if (default_entry_data != NULL) {
        sal_memcpy(buf_paddr, default_entry_data, ent_size);
    } else {
        sal_memset(buf_paddr, 0, ent_size);
    }
    work_flags = SBUSDMA_WF_WRITE_CMD | SBUSDMA_WF_SGL_DATA;

    dma_work = bcmptm_ser_sbusdma_work_init(unit, sid, inst, -1,
                                            &start_indexes,
                                            &index_ranges, 1,
                                            dma_addr, work_flags);
    if (dma_work == NULL) {
        rv = SHR_E_INIT;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmptm_ser_sbusdma_work_exec(unit, dma_work, NULL);
    bcmptm_ser_sbusdma_work_cleanup(unit, dma_work);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (buf_paddr != NULL) {
        bcmdrd_hal_dma_free(unit, 0, buf_paddr, dma_addr);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_refresh_regs_config(int unit, int disable)
{
    uint32_t info_num = 0, i = 0;
    bcmptm_ser_ctrl_reg_info_t *mem_refresh_info = NULL;
    bcmptm_ser_fld_info_t      *ctrl_flds_info = NULL;
    bcmdrd_sid_t sid = INVALIDm;
    int rv = SHR_E_NONE;
    int j = 0, fid_num = 0;
    uint32_t fld_val[4], *ptr_fld_old_val;
    bcmdrd_fid_t fid_array[5];
    /* disable counter */
    static int disable_cnt[BCMDRD_CONFIG_MAX_UNITS] = {0};
    static uint32_t fld_old_val[BCMDRD_CONFIG_MAX_UNITS][64];

    SHR_FUNC_ENTER(unit);

    if (disable) {
        disable_cnt[unit]++;
        /* Exit: already disable. */
        if (disable_cnt[unit] > 1) {
            SHR_EXIT();
        }
    } else {
        if (disable_cnt[unit] == 1) {
            disable_cnt[unit]--;
        } else if (disable_cnt[unit] > 1) {
            disable_cnt[unit]--;
            /* Disable more than one times. */
            SHR_EXIT();
        } else if (disable_cnt[unit] < 1) {
            /* Already disable. */
            disable_cnt[unit] = 0;
            SHR_EXIT();
        }
    }
    (void)bcmptm_ser_refresh_regs_info_get(unit, &mem_refresh_info, &info_num);
    if (info_num > 16) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }
    for (i = 0; i < info_num; i++) {
        sid = mem_refresh_info[i].ctrl_reg;
        fid_num = mem_refresh_info[i].num_fld;
        ctrl_flds_info = mem_refresh_info[i].ctrl_flds_val;
        if (fid_num > 4) {
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_EXIT(rv);
        }
        ptr_fld_old_val = &(fld_old_val[unit][i * 4]);
        for (j = 0; j < fid_num; j++, ctrl_flds_info++) {
            fid_array[j] = ctrl_flds_info->ctrl_fld;
            if (disable) {
                fld_val[j] = ctrl_flds_info->wr_val;
            } else {
                fld_val[j] = ptr_fld_old_val[j];
            }
        }
        fid_array[j] = INVALIDf;
        rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, 0, fid_array, 0, fld_val,
                                                (disable ? ptr_fld_old_val:NULL));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to write [%s] to disable refresh function\n"),
                   bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}


