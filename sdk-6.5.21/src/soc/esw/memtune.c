/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        memtune.c
 * Purpose:     External memory interface tuning routines.
 * Notes:       Framework for memory tuning sequences.
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/mem.h>
#include <soc/memtune.h>
#include <soc/er_tcam.h>

#include <shared/alloc.h>

#if defined(BCM_TRIUMPH_SUPPORT)

#ifdef BCM_TRIUMPH_SUPPORT
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

#ifndef NO_MEMTUNE
STATIC int
_mem_config_set(char *name, char *value)
{
    if (soc_mem_config_set) {
        return soc_mem_config_set(name, value);
    }
    return SOC_E_UNAVAIL;
}
#endif

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_tr_ddr_phase_sel_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr;

    reg = tr_mt_data->config_reg3;
    if (mt_ctrl->phase_sel_cur != -1) {
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, PHASE_SELf,
                          mt_ctrl->phase_sel_cur);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, OVRD_SM_ENf, 1);
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_ddr_latency_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr, sel_early2, sel_early1;

    /* EM latency setting */
    if (mt_ctrl->em_latency_cur != -1) {
        reg = tr_mt_data->tmode0;
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, EM_LATENCYf,
                          mt_ctrl->em_latency_cur & 0x7);
        if (soc_reg_field_valid(mt_ctrl->unit, reg, EM_LATENCY8f)) {
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, EM_LATENCY8f,
                              mt_ctrl->em_latency_cur >> 3);
        }
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    /* DDR latency setting */
    if (mt_ctrl->ddr_latency_cur != -1) {
        /*
         * DDR latency settings
         * if latency == 0, select result from 2 steps earlier
         * if latency == 1, select result from 1 step earlier
         * if latency == 2, select regular result
         */
        sel_early1 = 0;
        sel_early2 = 0;
        if (mt_ctrl->ddr_latency_cur == 0) {
            sel_early2 = 1;
        } else if (mt_ctrl->ddr_latency_cur == 1) {
            sel_early1 = 1;
        }

        reg = tr_mt_data->config_reg2;
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        if (mt_data->slice_mask & 0x1) {
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, SEL_EARLY2_0f,
                              sel_early2);
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, SEL_EARLY1_0f,
                              sel_early1);
        }
        if (mt_data->slice_mask & 0x2) {
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, SEL_EARLY2_1f,
                              sel_early2);
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, SEL_EARLY1_1f,
                              sel_early1);
        }
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_ddr_txrx_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr;

    reg = tr_mt_data->config_reg1;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;

    /* TX phase offset settings */
    if (mt_ctrl->tx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DLL90_OFFSET_TXf,
                          mt_ctrl->tx_offset_cur & 0x0f);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DLL90_OFFSET_TX4f,
                          mt_ctrl->tx_offset_cur >> 4);
    }

    /* RX phase offset settings */
    if (mt_ctrl->rx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DLL90_OFFSET_QKf,
                          mt_ctrl->rx_offset_cur & 0xf);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DLL90_OFFSET_QKBf,
                          mt_ctrl->rx_offset_cur & 0xf);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DLL90_OFFSET_QK4f,
                          mt_ctrl->rx_offset_cur >> 4);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DLL90_OFFSET_QKB4f,
                          mt_ctrl->rx_offset_cur >> 4);
    }

    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_ddr_nops_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr;

    reg = tr_mt_data->sram_ctl;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->r2w_nops_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, NUM_R2W_NOPSf,
                          mt_ctrl->r2w_nops_cur);
    }
    if (mt_ctrl->w2r_nops_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, NUM_W2R_NOPSf,
                          mt_ctrl->w2r_nops_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_ddr_per_rx_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 rval, addr;

    reg = tr_mt_data->config_reg1;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, reg, &rval, EN_RDFIFOf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    soc_reg_field_set(mt_ctrl->unit, reg, &rval, EN_RDFIFOf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_invert_clk_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr;

    reg = tr_mt_data->config_reg3;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->invert_txclk_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, INVERT_TXCLKf,
                          mt_ctrl->invert_txclk_cur);
    }
    if (mt_ctrl->invert_rxclk_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, INVERT_RXCLKf,
                          mt_ctrl->invert_rxclk_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_txrx_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr;

    reg = tr_mt_data->config_reg1;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->tx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, VCDL_TX_OFFSETf,
                          mt_ctrl->tx_offset_cur & 0x1f);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, MSBMIDL_OFFSET_TXf,
                          mt_ctrl->tx_offset_cur >> 5);
    }
    if (mt_ctrl->rx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, VCDL_RX_OFFSETf,
                          mt_ctrl->rx_offset_cur & 0x1f);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, MSBMIDL_OFFSET_RXf,
                          mt_ctrl->rx_offset_cur >> 5);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_dpeo_rbus_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    soc_reg_t reg;
    uint32 oval, rval, addr;

    reg = tr_mt_data->config_reg3;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->fcd_dpeo_cur != -1) {
        /*
         * DPEO_0 is connected to DPEO pin of first TCAM
         * DPEO_1 is connected to DPEO pin of optional second TCAM
         */
        if (!mt_ctrl->dpeo_sel_cur) {
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, FCD_DPEO_0f,
                              mt_ctrl->fcd_dpeo_cur);
        } else {
            soc_reg_field_set(mt_ctrl->unit, reg, &rval, FCD_DPEO_1f,
                              mt_ctrl->fcd_dpeo_cur);
        }
    }
    if (mt_ctrl->fcd_rbus_cur != -1) {
        /* use fcd_rbus + 1 for FCD_SMFL_0 and FCD_SMFL_1 */
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, FCD_SMFL_0f,
                          mt_ctrl->fcd_rbus_cur + 1);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, FCD_SMFL_1f,
                          mt_ctrl->fcd_rbus_cur + 1);
        /* use same value for FCD_RBUS and FCD_RV */
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, FCD_RBUSf,
                          mt_ctrl->fcd_rbus_cur);
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, FCD_RVf,
                          mt_ctrl->fcd_rbus_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    reg = tr_mt_data->etc_ctl;
    addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->rbus_sync_dly_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, RBUS_SYNC_DLYf,
                          mt_ctrl->rbus_sync_dly_cur);
    }
    if (mt_ctrl->dpeo_sync_dly_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, reg, &rval, DPEO_SYNC_DLYf,
                          mt_ctrl->dpeo_sync_dly_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_per_tx_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    uint32 rval;

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    /*
     * Following is the notes from DE for TCAM reset:
     * Without this fix
     * - we see flaky extt result for 1 board which has
     * 36Mb__MCM_ext_tcam Eye will be completely 0 and then switch to normal
     * (even for same rc)
     * Why does it work?
     * - My guess is that changing of tx, disturbs the clock going to ext_tcam
     * and can throw PLL in ext_tcam off, and more so for dual-die ext_tcam
     */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, &rval));
    soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                      EXT_TCAM_RSTf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
    sal_usleep(1000);
    soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                      EXT_TCAM_RSTf, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
    sal_usleep(1000);

    ((soc_tcam_info_t *)SOC_CONTROL(mt_ctrl->unit)->tcam_info)->num_tcams = 0;

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_per_rx_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    uint32 rval;

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    /*
     * Notes from DE for TCAM_PHY reset:
     * Without this fix
     * - we will see intermittent hard-failures in middle of tcam eye
     *   which leads to change in Tcam area when extt is run severa times
     * Why does it work?
     * - My guess is that changing of rx, changes the rx_clock in
     *   tcam_phy and disturbs the FIFO synchronizer logic
     */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, &rval));
    soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_TCAM_RST_Lf, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
    sal_usleep(10);
    soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                      CMIC_TCAM_RST_Lf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
    sal_usleep(10);

    return SOC_E_NONE;
}

STATIC int
_soc_tr_sram_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    tr_ext_sram_bist_t *sram_bist = mt_ctrl->sram_bist;
    int loop_mode, i;

    sram_bist->w2r_nops = mt_ctrl->w2r_nops_cur;
    sram_bist->r2w_nops = mt_ctrl->r2w_nops_cur;
    sram_bist->bg_tcam_loop_count = 1;

    for (i = 0; i < mt_data->test_count; i++) {
        for (loop_mode = 1; loop_mode < 4; loop_mode++) {
            if (loop_mode == 1) { /* RR */
                sram_bist->loop_mode = 0; /* WW */
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                                     mt_data->sub_interface,
                                                     TRUE, TRUE));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit,
                                                     mt_data->sub_interface,
                                                     sram_bist));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_op(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             sram_bist, NULL));
            }
            sram_bist->loop_mode = loop_mode;
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                                 mt_data->sub_interface,
                                                 TRUE, TRUE));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit,
                                                 mt_data->sub_interface,
                                                 sram_bist));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_op(mt_ctrl->unit, mt_data->sub_interface,
                                         sram_bist, NULL));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                                 mt_data->sub_interface,
                                                 FALSE, FALSE));
            if (sram_bist->err_cnt) {
                mt_ctrl->cur_fail_count++;
                if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                    return SOC_E_NONE;
                }
                continue;
            }
        }
    }

    if (mt_ctrl->cur_fail_count == 0) {
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             TRUE, TRUE));
        sram_bist->loop_mode = 3; /* WW_RR */
        sram_bist->bg_tcam_loop_count =
            mt_data->bg_tcam_bist ? mt_data->bg_tcam_loop_count : 1;
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             sram_bist));
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_op(mt_ctrl->unit, mt_data->sub_interface,
                                     sram_bist, NULL));
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             FALSE, FALSE));
        if (sram_bist->err_cnt) {
            mt_ctrl->cur_fail_count++;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 rval;
    int rv, addr, i;
    uint32 exp_dpeo_rise, exp_dpeo_fall;
    uint32 pattern[4];
    uint32 data[8], data_miss[8];
    int index, exp_result0[2], exp_result1[2], result0, result1, valid;
    uint32 mask[4];

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;
    partitions = tcam_info->partitions;

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->inst_status, REG_PORT_ANY,
                        0);

    switch (mt_data->sub_interface) {
    case 0: /* DBUS */
        pattern[0] = 0xffffffff;
        pattern[1] = 0x00000000;
        pattern[2] = 0x55555555;
        pattern[3] = 0xaaaaaaaa;
        mask[0] = mask[1] = mask[2] = mask[3] = 0;

        /* Use the last few entries in the firts block of the first device */
        index = 16380;
        for (i = 0; i < 4; i ++) {
            data[0] = 0;
            data[1] = pattern[i] & 0xff;
            data[2] = data[3] = pattern[i];
            SOC_IF_ERROR_RETURN
                (soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_DEV0_TBL72,
                                      index + i, mask, data));
        }
        for (i = 0; i < 4; i ++) {
            rv = soc_tcam_read_entry(mt_ctrl->unit, TCAM_PARTITION_DEV0_TBL72,
                                     index + i, mask, data, &valid);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            if (rv < 0 || (rval & tr_mt_data->dbus_fail_mask)) {
                mt_ctrl->cur_fail_count = mt_data->test_count;
                mt_ctrl->cur_fail_reason = 0x10000;
                mt_ctrl->cur_fail_detail0 = rval;
                break;
            }
            if (data[1] != (pattern[i] & 0xff) || data[2] != pattern[i] ||
                data[3] != pattern[i]) {
                mt_ctrl->cur_fail_count = mt_data->test_count;
                mt_ctrl->cur_fail_reason = 0x20000;
                mt_ctrl->cur_fail_detail0 = i;
                break;
            }
        }
        if (mt_ctrl->cur_fail_count) {
            break;
        }

        if (tcam_info->num_tcams == 1 || tcam_info->monolith_dev) {
            /* not MCM device */
            break;
        }

        /* Use the last few entries in the first block of the second device */
        index = 16380;
        for (i = 0; i < 4; i ++) {
            data[0] = 0;
            data[1] = pattern[i] & 0xff;
            data[2] = data[3] = pattern[i];
            SOC_IF_ERROR_RETURN
                (soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_DEV1_TBL72,
                                      index + i, mask, data));
        }
        for (i = 0; i < 4; i ++) {
            rv = soc_tcam_read_entry(mt_ctrl->unit, TCAM_PARTITION_DEV1_TBL72,
                                     index + i, mask, data, &valid);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            if (rv < 0 || (rval & tr_mt_data->dbus_fail_mask)) {
                mt_ctrl->cur_fail_count = mt_data->test_count;
                mt_ctrl->cur_fail_reason = 0x10001;
                mt_ctrl->cur_fail_detail0 = rval;
                break;
            }
            if (data[1] != (pattern[i] & 0xff) || data[2] != pattern[i] ||
                data[3] != pattern[i]) {
                mt_ctrl->cur_fail_count = mt_data->test_count;
                mt_ctrl->cur_fail_reason = 0x20001;
                mt_ctrl->cur_fail_detail0 = i;
                break;
            }
        }
        break;

    case 1: /* DBUS_out (DPEO) */
        sal_memset(data, 0, sizeof(data));
        data[0] = data[4] = 0xabcd;
        SOC_IF_ERROR_RETURN
            (soc_tr_tcam_type1_memtest_dpeo(mt_ctrl->unit, 2, 0x3, data));
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        /*
         * DPEO_0 is connected to DPEO pin of first TCAM
         * DPEO_1 is connected to DPEO pin of optional second TCAM
         */
        if (!mt_ctrl->dpeo_sel_cur) { /* DPEO_0 */
            exp_dpeo_rise = 1;
            exp_dpeo_fall = 1;
        } else { /* DPEO_1 */
            exp_dpeo_rise = 2;
            exp_dpeo_fall = 2;
        }
        if (!soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                               SEQ_DPEO_ERRf) ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPEO_RISEf) != exp_dpeo_rise ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPEO_FALLf) != exp_dpeo_fall) {
            mt_ctrl->cur_fail_count = mt_data->test_count;
            mt_ctrl->cur_fail_detail0 = rval;
        }
        break;

    case 2: /* RBUS */
        if (mt_data->mask[0] == 0xffffffff) {
            sal_memset(mask, 0, sizeof(mask));
            sal_memset(data, 0, sizeof(data));
            /* all '1's for lower 72-bit, all '0's for upper 72-bit */
            data[5] = 0xff;
            data[6] = data[7] = 0xffffffff;
        } else {
            sal_memcpy(mask, mt_data->mask, sizeof(mask));
            sal_memcpy(data, mt_data->data, sizeof(data));
        }
        sal_memcpy(data_miss, data, sizeof(data_miss));
        data_miss[7] ^= 1; /* use a bogus key to cause a miss */

        /* first do a search miss, ignore result index */
        rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_DEV0_TBL72,
                                   TCAM_PARTITION_DEV0_TBL144, data_miss,
                                   &result0, &result1);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        /* then do a search hit, ignore result index, only check for status */
        rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_DEV0_TBL72,
                                   TCAM_PARTITION_DEV0_TBL144, data,
                                   &result0, &result1);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        if (rval & tr_mt_data->rbus_fail_mask) {
            mt_ctrl->cur_fail_count = mt_data->test_count;
            mt_ctrl->cur_fail_reason = 0x10000;
            mt_ctrl->cur_fail_detail0 = rval;
            break;
        }
        if ((rval & tr_mt_data->rbus_valid_mask) !=
            tr_mt_data->rbus_valid_mask) {
            mt_ctrl->cur_fail_count = mt_data->test_count;
            mt_ctrl->cur_fail_reason = 0x20000;
            mt_ctrl->cur_fail_detail0 = rval;
            break;
        }

        if (!partitions[TCAM_PARTITION_DEV1_TBL72].num_entries ||
            !partitions[TCAM_PARTITION_DEV1_TBL144].num_entries) {
            /* not MCM device */
            break;
        }

        /* first do a search miss, ignore result index */
        rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_DEV1_TBL72,
                                   TCAM_PARTITION_DEV1_TBL144, data_miss,
                                   &result0, &result1);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        /* then do a search hit, ignore result index, only check for status */
        rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_DEV1_TBL72,
                                   TCAM_PARTITION_DEV1_TBL144, data,
                                   &result0, &result1);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        if (rval & tr_mt_data->rbus_fail_mask) {
            mt_ctrl->cur_fail_count = mt_data->test_count;
            mt_ctrl->cur_fail_reason = 0x10001;
            mt_ctrl->cur_fail_detail0 = rval;
            break;
        }
        if ((rval & tr_mt_data->rbus_valid_mask) !=
            tr_mt_data->rbus_valid_mask) {
            mt_ctrl->cur_fail_count = mt_data->test_count;
            mt_ctrl->cur_fail_reason = 0x20001;
            mt_ctrl->cur_fail_detail0 = rval;
            break;
        }
        break;

    case 3: /* search test */
        /* No reset on simulation, put test entry at index 0 to avoid hitting
         * leftover entries */
        index = SAL_BOOT_SIMULATION ? 0 : 0x1555;
        if (mt_data->mask[0] == 0xffffffff) {
            sal_memset(mask, 0, sizeof(mask));
            sal_memset(data, 0, sizeof(data));
            /* all '1's for lower 72-bit, all '0's for upper 72-bit */
            data[5] = 0xff;
            data[6] = data[7] = 0xffffffff;
        } else {
            sal_memcpy(mask, mt_data->mask, sizeof(mask));
            sal_memcpy(data, mt_data->data, sizeof(data));
        }

        exp_result0[0] = partitions[TCAM_PARTITION_DEV0_TBL72].tcam_base +
            (index  << partitions[TCAM_PARTITION_DEV0_TBL72].tcam_width_shift);
        exp_result1[0] = partitions[TCAM_PARTITION_DEV0_TBL144].tcam_base +
            (index  <<
             partitions[TCAM_PARTITION_DEV0_TBL144].tcam_width_shift);
        exp_result0[1] = partitions[TCAM_PARTITION_DEV1_TBL72].tcam_base +
            (index  << partitions[TCAM_PARTITION_DEV1_TBL72].tcam_width_shift);
        exp_result1[1] = partitions[TCAM_PARTITION_DEV1_TBL144].tcam_base +
            (index  <<
             partitions[TCAM_PARTITION_DEV1_TBL144].tcam_width_shift);

        for (i = 0; i < mt_data->test_count; i++) {
            rv = soc_tcam_search_entry(mt_ctrl->unit,
                                       TCAM_PARTITION_DEV0_TBL72,
                                       TCAM_PARTITION_DEV0_TBL144,
                                       data, &result0, &result1);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            if (rv < 0 || (rval & tr_mt_data->rbus_fail_mask)) {
                mt_ctrl->cur_fail_count++;
                mt_ctrl->cur_fail_reason = 0x10000;
                mt_ctrl->cur_fail_detail0 = rval;
                continue;
            }

            if (result0 != exp_result0[0] || result1 != exp_result1[0]) {
                mt_ctrl->cur_fail_count++;
                mt_ctrl->cur_fail_reason = 0x20000;
                mt_ctrl->cur_fail_detail0 = result0;
                mt_ctrl->cur_fail_detail1 = result1;
                continue;
            }

            rv = soc_triumph_tcam_search_bist(mt_ctrl->unit,
                                              TCAM_PARTITION_DEV0_TBL72,
                                              TCAM_PARTITION_DEV0_TBL144,
                                              data,
                                              exp_result0[0], exp_result1[0],
                                              mt_data->tcam_loop_count);
            if (rv < 0) {
                mt_ctrl->cur_fail_count++;
                mt_ctrl->cur_fail_reason = 0x30000;
                continue;
            }

            if (!partitions[TCAM_PARTITION_DEV1_TBL72].num_entries ||
                !partitions[TCAM_PARTITION_DEV1_TBL144].num_entries) {
                /* not MCM device */
                continue;
            }

            rv = soc_tcam_search_entry(mt_ctrl->unit,
                                       TCAM_PARTITION_DEV1_TBL72,
                                       TCAM_PARTITION_DEV1_TBL144,
                                       data, &result0, &result1);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            if (rv < 0 || (rval & tr_mt_data->rbus_fail_mask)) {
                mt_ctrl->cur_fail_count++;
                mt_ctrl->cur_fail_reason = 0x10001;
                continue;
            }

            if (result0 != exp_result0[1] || result1 != exp_result1[1]) {
                mt_ctrl->cur_fail_count++;
                mt_ctrl->cur_fail_reason = 0x20001;
                mt_ctrl->cur_fail_detail0 = result0;
                mt_ctrl->cur_fail_detail1 = result1;
                continue;
            }

            rv = soc_triumph_tcam_search_bist(mt_ctrl->unit,
                                              TCAM_PARTITION_DEV1_TBL72,
                                              TCAM_PARTITION_DEV1_TBL144,
                                              data,
                                              exp_result0[1], exp_result1[1],
                                              mt_data->tcam_loop_count);
            if (rv < 0) {
                mt_ctrl->cur_fail_count++;
                mt_ctrl->cur_fail_reason = 0x30001;
                continue;
            }
        }

        break;

    default:
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tune_generate_config(soc_memtune_data_t *mt_data)
{
#ifndef NO_MEMTUNE
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    uint32 rval, val, pvt_val;
    char name_str[23], val_str[11];
    int use_midl;

    pvt_val = 0;
    if (mt_ctrl->odtres_val != -1) {
        pvt_val |= SOC_TR_MEMTUNE_OVRD_ODTRES_MASK <<
            SOC_TR_MEMTUNE_OVRD_ODTRES_SHIFT;
        pvt_val |= (mt_ctrl->odtres_val & SOC_TR_MEMTUNE_ODTRES_MASK) <<
            SOC_TR_MEMTUNE_ODTRES_SHIFT;
    }
    if (mt_ctrl->ndrive_val != -1 && mt_ctrl->pdrive_val != -1) {
        pvt_val |= SOC_TR_MEMTUNE_OVRD_DRIVER_MASK <<
            SOC_TR_MEMTUNE_OVRD_DRIVER_SHIFT;
        pvt_val |= (mt_ctrl->pdrive_val & SOC_TR_MEMTUNE_PDRIVE_MASK) <<
            SOC_TR_MEMTUNE_PDRIVE_SHIFT;
        pvt_val |= (mt_ctrl->ndrive_val & SOC_TR_MEMTUNE_NDRIVE_MASK) <<
            SOC_TR_MEMTUNE_NDRIVE_SHIFT;
    }
    if (mt_ctrl->slew_val != -1) {
        pvt_val |= SOC_TR_MEMTUNE_OVRD_SLEW_MASK <<
            SOC_TR_MEMTUNE_OVRD_SLEW_SHIFT;
        pvt_val |= (mt_ctrl->slew_val & SOC_TR_MEMTUNE_SLEW_MASK) <<
            SOC_TR_MEMTUNE_SLEW_SHIFT;
    }

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_SRAM:
        val = 0;
        if (mt_ctrl->phase_sel_cur != -1) {
            val |= SOC_TR_MEMTUNE_OVRD_SM_MASK <<
                SOC_TR_MEMTUNE_OVRD_SM_SHIFT;
            val |= (mt_ctrl->phase_sel_cur & SOC_TR_MEMTUNE_PHASE_SEL_MASK) <<
                SOC_TR_MEMTUNE_PHASE_SEL_SHIFT;
        }
        if (mt_ctrl->ddr_latency_cur != -1) {
            val |=
                (mt_ctrl->ddr_latency_cur & SOC_TR_MEMTUNE_DDR_LATENCY_MASK) <<
                SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT;
        }

        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    val |
                    /* use 0 in the 3-bit field to represent EM_LATENCY is 8 */
                    ((mt_ctrl->em_latency_cur &
                      SOC_TR_MEMTUNE_EM_LATENCY_MASK) <<
                     SOC_TR_MEMTUNE_EM_LATENCY_SHIFT) |
                    ((mt_ctrl->tx_offset_cur &
                      SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT) |
                    ((mt_ctrl->rx_offset_cur &
                      SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT) |
                    ((mt_ctrl->w2r_nops_cur & SOC_TR_MEMTUNE_W2R_NOPS_MASK) <<
                     SOC_TR_MEMTUNE_W2R_NOPS_SHIFT) |
                    ((mt_ctrl->r2w_nops_cur & SOC_TR_MEMTUNE_R2W_NOPS_MASK) <<
                     SOC_TR_MEMTUNE_R2W_NOPS_SHIFT) |
                    ((mt_data->freq & SOC_TR_MEMTUNE_DDR_FREQ_MASK) <<
                     SOC_TR_MEMTUNE_DDR_FREQ_SHIFT));
        sal_sprintf(name_str, "%s%d.%d", spn_EXT_SRAM_TUNING,
                    mt_data->sub_interface, mt_ctrl->unit);
        if (_mem_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        if (pvt_val) {
            sal_sprintf(val_str, "0x%08x",
                        SOC_TR_MEMTUNE_CONFIG_VALID_MASK | pvt_val);
            sal_sprintf(name_str, "%s%d.%d", spn_EXT_SRAM_PVT,
                        mt_data->sub_interface, mt_ctrl->unit);
            if (_mem_config_set(name_str, val_str) < 0) {
                return SOC_E_MEMORY;
            }
        }
        if (mt_ctrl->tx_offset_min == mt_ctrl->tx_offset_max ||
            mt_ctrl->rx_offset_min == mt_ctrl->rx_offset_max) {
            break;
        }
        if (mt_ctrl->phase_sel_cur != -1) {
            result = mt_ctrl->ps_data[mt_ctrl->phase_sel_cur];
        } else {
            result = mt_ctrl->ps_data[0];
        }
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((result->width & SOC_TR_MEMTUNE_STATS_WIDTH_MASK) <<
                     SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) |
                    ((result->height & SOC_TR_MEMTUNE_STATS_HEIGHT_MASK) <<
                     SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) |
                    ((result->fail_count & SOC_TR_MEMTUNE_STATS_FAIL_MASK) <<
                     SOC_TR_MEMTUNE_STATS_FAIL_SHIFT));
        sal_sprintf(name_str, "%s%d.%d", spn_EXT_SRAM_TUNING_STATS,
                    mt_data->sub_interface, mt_ctrl->unit);
        if (_mem_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        break;
    case SOC_MEM_INTERFACE_TCAM:
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(mt_ctrl->unit,
                                                           &rval));
        use_midl =
            soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              MIDL_TX_ENf) &&
            soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              SEL_TX_CLKDLY_BLKf) ? 1 : 0;
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((mt_ctrl->fcd_dpeo_cur & SOC_TR_MEMTUNE_FCD_DPEO_MASK) <<
                     SOC_TR_MEMTUNE_FCD_DPEO_SHIFT) |
                    ((mt_ctrl->dpeo_sync_dly_cur &
                      SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK) <<
                     SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT) |
                    ((mt_ctrl->fcd_rbus_cur & SOC_TR_MEMTUNE_FCD_RBUS_MASK) <<
                     SOC_TR_MEMTUNE_FCD_RBUS_SHIFT) |
                    ((mt_ctrl->rbus_sync_dly_cur &
                      SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK) <<
                     SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT));
        sal_sprintf(name_str, "%s0.%d", spn_EXT_TCAM_TUNING, mt_ctrl->unit);
        if (_mem_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        val = 0;
        if (mt_ctrl->invert_txclk_cur != -1) {
            val |= (mt_ctrl->invert_txclk_cur &
                    SOC_TR_MEMTUNE_INVERT_TXCLK_MASK) <<
                SOC_TR_MEMTUNE_INVERT_TXCLK_SHIFT;
        }
        if (mt_ctrl->invert_rxclk_cur != -1) {
            val |= (mt_ctrl->invert_rxclk_cur &
                    SOC_TR_MEMTUNE_INVERT_RXCLK_MASK) <<
                SOC_TR_MEMTUNE_INVERT_RXCLK_SHIFT;
        }
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    val |
                    ((mt_ctrl->tx_offset_cur &
                      SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT) |
                    ((mt_ctrl->rx_offset_cur &
                      SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT) |
                    ((mt_data->freq &
                      SOC_TR_MEMTUNE_TCAM_FREQ_MASK) <<
                     SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT) |
                    ((use_midl & SOC_TR_MEMTUNE_USE_MIDL_MASK) <<
                     SOC_TR_MEMTUNE_USE_MIDL_SHIFT));
        sal_sprintf(name_str, "%s1.%d", spn_EXT_TCAM_TUNING, mt_ctrl->unit);
        if (_mem_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        if (pvt_val) {
            sal_sprintf(val_str, "0x%08x",
                        SOC_TR_MEMTUNE_CONFIG_VALID_MASK | pvt_val);
            sal_sprintf(name_str, "%s.%d", spn_EXT_TCAM_PVT, mt_ctrl->unit);
            if (_mem_config_set(name_str, val_str) < 0) {
                return SOC_E_MEMORY;
            }
        }
        if (mt_ctrl->tx_offset_min == mt_ctrl->tx_offset_max ||
            mt_ctrl->rx_offset_min == mt_ctrl->rx_offset_max) {
            break;
        }
        if (mt_ctrl->invert_txclk_cur != -1 &&
            mt_ctrl->invert_rxclk_cur != -1) {
            result = &mt_ctrl->result[mt_ctrl->invert_txclk_cur * 2 +
                                      mt_ctrl->invert_rxclk_cur];
        } else {
            result = &mt_ctrl->result[0];
        }
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((result->width & SOC_TR_MEMTUNE_STATS_WIDTH_MASK) <<
                     SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) |
                    ((result->height & SOC_TR_MEMTUNE_STATS_HEIGHT_MASK) <<
                     SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) |
                    ((result->fail_count & SOC_TR_MEMTUNE_STATS_FAIL_MASK) <<
                     SOC_TR_MEMTUNE_STATS_FAIL_SHIFT));
        sal_sprintf(name_str, "%s.%d", spn_EXT_TCAM_TUNING_STATS,
                    mt_ctrl->unit);
        if (_mem_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        break;
    default:
        break;
    }
#endif /* NO_MEMTUNE */

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#define ABS(n) (((n) < 0) ? -(n) : (n))

STATIC void
_soc_memtune_get_txrx_area(soc_memtune_data_t *mt_data,
                           int tx_offset, int rx_offset,
                           int *width, int *height)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int best_area, best_shape, area, shape;
    int dt, dr, max_dt, max_dr, index, num_rx;

    num_rx = mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1;
    index = (tx_offset - mt_ctrl->tx_offset_min) * num_rx +
            rx_offset - mt_ctrl->rx_offset_min;
    if (mt_ctrl->fail_array[index] > 0) {
        *width = *height = 0;
        return;
    }

    max_dt = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min) / 2 + 1;
    for (dt = 1; dt < max_dt; dt++) {
        if (tx_offset - dt < mt_ctrl->tx_offset_min ||
            tx_offset + dt > mt_ctrl->tx_offset_max ||
            mt_ctrl->fail_array[index - dt * num_rx] > 0 ||
            mt_ctrl->fail_array[index + dt * num_rx] > 0) {
            max_dt = dt;
            break;
        }
    }

    max_dr = (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min) / 2 + 1;
    for (dr = 1; dr < max_dr; dr++) {
        if (rx_offset - dr < mt_ctrl->rx_offset_min ||
            rx_offset + dr > mt_ctrl->rx_offset_max ||
            mt_ctrl->fail_array[index - dr] > 0 ||
            mt_ctrl->fail_array[index + dr] > 0) {
            max_dr = dr;
            break;
        }
    }

    area = max_dr * 2 - 1;
    shape = area - 1;
    best_area = area;
    best_shape = shape;
    *width = area;
    *height = 1;

    for (dt = 1; dt < max_dt; dt++) {
        for (dr = 1; dr < max_dr; dr++) {
            if (mt_ctrl->fail_array[index - dt * num_rx - dr] > 0 ||
                mt_ctrl->fail_array[index - dt * num_rx + dr] > 0 ||
                mt_ctrl->fail_array[index + dt * num_rx - dr] > 0 ||
                mt_ctrl->fail_array[index + dt * num_rx + dr] > 0) {
                max_dr = dr;
                break;
            }
        }

        dr--;
        area = (dt * 2 + 1) * (dr * 2 + 1);
        shape = ABS(dt - dr) * 2;
        if (area > best_area ||
            (area == best_area && shape < best_shape)) {
            best_area = area;
            best_shape = shape;
            *width = dr * 2 + 1;
            *height = dt * 2 + 1;
        }
    }
}

STATIC int
_soc_memtune_txrx_analyze(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    int offset_count, fail_count;
    int index, tx_offset, rx_offset;
    int area, shape, width, height;
    int best_area, best_shape, best_width, best_height;
    char buf[128];
    int buf_index;

    offset_count = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min + 1) *
                   (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1);
    fail_count = 0;
    index = 0;
    for (tx_offset = mt_ctrl->tx_offset_min;
         tx_offset <= mt_ctrl->tx_offset_max; tx_offset++) {
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            if (mt_ctrl->fail_array[index] > 0) {
                fail_count++;
            }
            index++;
        }
    }

    if (mt_ctrl->flags & (SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
                          SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS)) {
        buf_index = sal_sprintf(buf, "unit %d %s interface",
                                mt_ctrl->unit, mt_ctrl->intf_name);
        switch (mt_data->interface) {
        case SOC_MEM_INTERFACE_SRAM:
            if (mt_ctrl->em_latency_cur != -1) {
                buf_index += sal_sprintf(&buf[buf_index], " em_latency=%d",
                                         mt_ctrl->em_latency_cur);
            }
            if (mt_ctrl->ddr_latency_cur != -1) {
                buf_index += sal_sprintf(&buf[buf_index], " ddr_latency=%d",
                                         mt_ctrl->ddr_latency_cur);
            }
            break;
        case SOC_MEM_INTERFACE_TCAM:
            if (mt_ctrl->invert_txclk_cur != -1) {
                buf_index += sal_sprintf(&buf[buf_index], " invert_txclk=%d",
                                         mt_ctrl->invert_txclk_cur);
            }
            if (mt_ctrl->invert_rxclk_cur != -1) {
                buf_index += sal_sprintf(&buf[buf_index], " invert_rxclk=%d",
                                         mt_ctrl->invert_rxclk_cur);
            }
            break;
        default:
            break;
        }
    }

    if (fail_count == offset_count) {
        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "Memory tuning %s failure count: all fail\n"), buf));
        }
        result = &mt_ctrl->result[mt_ctrl->result_count];
        result->fail_count = fail_count;
        mt_ctrl->result_count++;
        return SOC_E_NONE;
    }

    /* Print failure matrix */
    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Memory tuning %s failure count:\n"), buf));
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "TX (vertical axis) range: %d - %d, "
                            "RX (horizontal axis) range: %d - %d\n"),
                 mt_ctrl->tx_offset_min, mt_ctrl->tx_offset_max,
                 mt_ctrl->rx_offset_min, mt_ctrl->rx_offset_max));
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "        |")));
        for (index = mt_ctrl->rx_offset_min; index <= mt_ctrl->rx_offset_max;
             index++) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "%4d\t"), index));
        }
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "\n--------+")));
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "--------")));
        }
        index = 0;
        for (tx_offset = mt_ctrl->tx_offset_min;
             tx_offset <= mt_ctrl->tx_offset_max; tx_offset++) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "\n     %2d |"), tx_offset));
            for (rx_offset = mt_ctrl->rx_offset_min;
                 rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
                if (mt_ctrl->fail_array[index] >= 0) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "%4d\t"), mt_ctrl->fail_array[index]));
                } else {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        " (%d)\t"), -mt_ctrl->fail_array[index]));
                }
                index++;
            }
        }
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "\n")));
    }

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Memory tuning %s analysis "
                            "[box area, abs(width - height)]:\n"), buf));
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "        |")));
        for (index = mt_ctrl->rx_offset_min; index <= mt_ctrl->rx_offset_max;
             index++) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "%4d\t"), index));
        }
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "\n--------+")));
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "--------")));
        }
    }

    best_area = 0;
    best_shape = mt_ctrl->tx_offset_max + mt_ctrl->rx_offset_max;
    best_width = 0;
    best_height = 0;

    for (tx_offset = mt_ctrl->tx_offset_min;
         tx_offset <= mt_ctrl->tx_offset_max; tx_offset++) {
        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "\n     %2d |"), tx_offset));
        }
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            _soc_memtune_get_txrx_area(mt_data, tx_offset, rx_offset,
                                       &width, &height);
            area = width * height;
            shape = ABS(width - height);
            if (area != 0) {
                if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "%3d,%1d\t"), area, shape));
                }
                if (area > best_area ||
                    (area == best_area && shape < best_shape)) {
                    best_area = area;
                    best_shape = shape;
                    best_width = width;
                    best_height = height;
                    mt_ctrl->tx_offset_cur = tx_offset;
                    mt_ctrl->rx_offset_cur = rx_offset;
                }
            } else {
                if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "  *,*\t")));
                }
            }
        }
    }
    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "\n")));
    }

    result = &mt_ctrl->result[mt_ctrl->result_count];

    result->fail_count = fail_count;
    if (best_area != 0) {
        /* save the stats and TX/RX settings for this result */
        result->width = best_width;
        result->height = best_height;
        result->tx_offset = mt_ctrl->tx_offset_cur;
        result->rx_offset = mt_ctrl->rx_offset_cur;
    }

    mt_ctrl->result_count++;

    return SOC_E_NONE;
}

#define BACKSPACE               "\010"
STATIC int
_soc_memtune_phase_sel_compare(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *ps_data;
    int area[4], adj_area[4], shape, side;
    int best_area, best_shape, best_side, best_adj_area;
    int i, index;

    for (i = mt_ctrl->phase_sel_min; i <= mt_ctrl->phase_sel_max; i++) {
        area[i] = mt_ctrl->ps_data[i]->width * mt_ctrl->ps_data[i]->height;
    }
    for (i = mt_ctrl->phase_sel_min; i <= mt_ctrl->phase_sel_max; i++) {
        /* For each of the phase_sel values, get a sum of the
         * areas of the 2 neighboring phase_sel values. Later,
         * if 2 phase_sel values are found to have the same area/shape,
         * preference is given to the one with better neighbors.
         */
        adj_area[i] = area[i];
        index = i == mt_ctrl->phase_sel_min ? mt_ctrl->phase_sel_max : i - 1;
        if (mt_ctrl->ps_data[index]->ddr_latency ==
            mt_ctrl->ps_data[i]->ddr_latency &&
            mt_ctrl->ps_data[index]->em_latency ==
            mt_ctrl->ps_data[i]->em_latency) {
            adj_area[i] += area[index];
        }
        index = i == mt_ctrl->phase_sel_max ? mt_ctrl->phase_sel_min : i + 1;
        if (mt_ctrl->ps_data[index]->ddr_latency ==
            mt_ctrl->ps_data[i]->ddr_latency &&
            mt_ctrl->ps_data[index]->em_latency ==
            mt_ctrl->ps_data[i]->em_latency) {
            adj_area[i] += area[index];
        }
    }

    best_side = 0;
    best_adj_area = 0;
    best_area = 0;
    best_shape = 0;
    mt_ctrl->phase_sel_cur = 0;
    for (i = mt_ctrl->phase_sel_min; i <= mt_ctrl->phase_sel_max; i++) {
        ps_data = mt_ctrl->ps_data[i];
        if (ps_data->width > ps_data->height) {
            side = ps_data->height;
            shape = ps_data->width - ps_data->height;
        } else {
            side = ps_data->width;
            shape = ps_data->height - ps_data->width;
        }
        if (!area[i]) {
            continue;
        }
        if (side < best_side) {
            continue;
        } else if (side == best_side) {
            if (adj_area[i] < best_adj_area) {
                continue;
            } else if (adj_area[i] == best_adj_area) {
                if (area[i] < best_area) {
                    continue;
                } else if (area[i] == best_area) {
                    if (shape >= best_shape) {
                        continue;
                    }
                }
            }
        }

        best_side = side;
    /*    coverity[uninit_use : FALSE]    */
        best_adj_area = adj_area[i];
        best_area = area[i];
        best_shape = shape;
        mt_ctrl->phase_sel_cur = i;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_memtune_ddr_single_test(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int rv;

    mt_ctrl->cur_fail_count = 0;

    rv = (*mt_ctrl->test_fn)(mt_data);

    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                  (BSL_META_U(mt_ctrl->unit,
                              "unit %d %s test failure: %s\n"),
                   mt_ctrl->unit, mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }

    if ((!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS) &&
         mt_ctrl->cur_fail_count != mt_data->test_count) ||
        (!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL) &&
         mt_ctrl->cur_fail_count == mt_data->test_count)) {
        return SOC_E_NONE;
    }

    if (mt_ctrl->phase_sel_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Phase select=%d "), mt_ctrl->phase_sel_cur));
    }
    if (mt_ctrl->em_latency_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "EM lat=%d "), mt_ctrl->em_latency_cur));
    }
    if (mt_ctrl->ddr_latency_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "DDR lat=%d "), mt_ctrl->ddr_latency_cur));
    }
    if (mt_ctrl->tx_offset_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "TX offset=%d "), mt_ctrl->tx_offset_cur));
    }
    if (mt_ctrl->rx_offset_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "RX offset=%d "), mt_ctrl->rx_offset_cur));
    }
    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS) {
        if (mt_ctrl->r2w_nops_cur != -1) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "R2W nops=%d "), mt_ctrl->r2w_nops_cur));
        }
        if (mt_ctrl->w2r_nops_cur != -1) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "W2R nops=%d "), mt_ctrl->w2r_nops_cur));
        }
    }
    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                        ": Fail count=%d\n"), mt_ctrl->cur_fail_count));

    return SOC_E_NONE;
}

STATIC int
_soc_memtune_ddr_main(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    int rv, success, offset_count, old_result_count, index, i;
    char progress_symbol;

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Memory tuning test: %s interface:\n"),
                 mt_ctrl->intf_name));
    }

    if (mt_data->manual_settings) {
        mt_ctrl->phase_sel_cur = mt_data->man_phase_sel;
        mt_ctrl->em_latency_cur = mt_data->man_em_latency;
        mt_ctrl->ddr_latency_cur = mt_data->man_ddr_latency;
        mt_ctrl->tx_offset_cur = mt_data->man_tx_offset;
        mt_ctrl->rx_offset_cur = mt_data->man_rx_offset;
        mt_ctrl->w2r_nops_cur = mt_data->man_w2r_nops;
        mt_ctrl->r2w_nops_cur = mt_data->man_r2w_nops;
        mt_ctrl->cur_fail_count = 0;

        (*mt_ctrl->prog_hw3_fn)(mt_data); /* phase */
        (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */
        (*mt_ctrl->prog_hw1_fn)(mt_data); /* TX/RX */
        if (*mt_ctrl->per_tx_fn != NULL) {
            (*mt_ctrl->per_tx_fn)(mt_data);
        }
        if (*mt_ctrl->per_rx_fn != NULL) {
            (*mt_ctrl->per_rx_fn)(mt_data);
        }

        SOC_IF_ERROR_RETURN(_soc_memtune_ddr_single_test(mt_data));
        if (mt_ctrl->cur_fail_count == 0 && mt_data->config) {
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                SOC_IF_ERROR_RETURN(_soc_tr_tune_generate_config(mt_data));
            }
#endif /* BCM_TRIUMPH_SUPPORT */
        }
        return SOC_E_NONE;
    }

    offset_count = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min + 1) *
                   (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1);
    mt_ctrl->fail_array = sal_alloc(offset_count * sizeof(int),
                                    "memtune working data");
    if (mt_ctrl->fail_array == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl->fail_array, 0, offset_count * sizeof(int));

    mt_ctrl->w2r_nops_cur = mt_ctrl->w2r_nops_max;
    mt_ctrl->r2w_nops_cur = mt_ctrl->r2w_nops_max;
    mt_ctrl->result_count = 0;

    for (mt_ctrl->phase_sel_cur = mt_ctrl->phase_sel_min;
         mt_ctrl->phase_sel_cur <= mt_ctrl->phase_sel_max;
         mt_ctrl->phase_sel_cur++) {
        if (mt_ctrl->phase_sel_cur != -1) {
            (*mt_ctrl->prog_hw3_fn)(mt_data); /* phase */
        }

        old_result_count = mt_ctrl->result_count;
        progress_symbol = '-';
        if (mt_data->do_txrx_first) {
            for (mt_ctrl->tx_offset_cur = mt_ctrl->tx_offset_min;
                 mt_ctrl->tx_offset_cur <= mt_ctrl->tx_offset_max;
                 mt_ctrl->tx_offset_cur++) {
                for (mt_ctrl->rx_offset_cur = mt_ctrl->rx_offset_min;
                     mt_ctrl->rx_offset_cur <= mt_ctrl->rx_offset_max;
                     mt_ctrl->rx_offset_cur++) {
                    (*mt_ctrl->prog_hw1_fn)(mt_data); /* TX/RX */
                    if (mt_ctrl->rx_offset_cur == mt_ctrl->rx_offset_min) {
                        if (*mt_ctrl->per_tx_fn != NULL) {
                            (*mt_ctrl->per_tx_fn)(mt_data);
                        }
                    }
                    if (*mt_ctrl->per_rx_fn != NULL) {
                        (*mt_ctrl->per_rx_fn)(mt_data);
                    }
                    for (mt_ctrl->em_latency_cur = mt_ctrl->em_latency_min;
                         mt_ctrl->em_latency_cur <= mt_ctrl->em_latency_max;
                         mt_ctrl->em_latency_cur++) {
                        for (mt_ctrl->ddr_latency_cur =
                                 mt_ctrl->ddr_latency_min;
                             mt_ctrl->ddr_latency_cur <=
                                 mt_ctrl->ddr_latency_max;
                             mt_ctrl->ddr_latency_cur++) {
                            if (mt_ctrl->flags &
                                SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS) {
                                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                                    "%c" BACKSPACE), progress_symbol));
                                progress_symbol ^= '-' ^ '|';
                            }
                            (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */
                            rv = _soc_memtune_ddr_single_test(mt_data);
                            if (SOC_FAILURE(rv)) { /* fail unexpected */
                                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                                    " " BACKSPACE)));
                                sal_free(mt_ctrl->fail_array);
                                return rv;
                            }
                            if (mt_ctrl->cur_fail_count == 0) {
                                break;
                            }
                        } /* mt_ctrl->ddr_latency_cur loop */
                        if (mt_ctrl->cur_fail_count == 0) {
                            break;
                        }
                    } /* mt_ctrl->em_latency_cur loop */
                    index = (mt_ctrl->tx_offset_cur - mt_ctrl->tx_offset_min) *
                        (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1) +
                        mt_ctrl->rx_offset_cur - mt_ctrl->rx_offset_min;
                    if (mt_ctrl->cur_fail_count > 0) {
                        mt_ctrl->fail_array[index] = mt_ctrl->cur_fail_count;
                    } else {
                        mt_ctrl->fail_array[index] = -mt_ctrl->em_latency_cur;
                    }
                } /* mt_ctrl->rx_offset_cur loop */
            } /* mt_ctrl->tx_offset_cur loop */
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " " BACKSPACE)));
            }

            /* Analyze and print result matrix */
            result = &mt_ctrl->result[mt_ctrl->result_count];
            mt_ctrl->em_latency_cur = -1;
            mt_ctrl->ddr_latency_cur = -1;
            _soc_memtune_txrx_analyze(mt_data);
            result->phase_sel = mt_ctrl->phase_sel_cur;
            result->em_latency = -1;
            result->ddr_latency = -1;
        } else {
            for (mt_ctrl->em_latency_cur = mt_ctrl->em_latency_min;
                 mt_ctrl->em_latency_cur <= mt_ctrl->em_latency_max;
                 mt_ctrl->em_latency_cur++) {
                for (mt_ctrl->ddr_latency_cur = mt_ctrl->ddr_latency_min;
                     mt_ctrl->ddr_latency_cur <= mt_ctrl->ddr_latency_max;
                     mt_ctrl->ddr_latency_cur++) {
                    success = FALSE;
                    for (mt_ctrl->tx_offset_cur = mt_ctrl->tx_offset_min;
                         mt_ctrl->tx_offset_cur <= mt_ctrl->tx_offset_max;
                         mt_ctrl->tx_offset_cur++) {
                        for (mt_ctrl->rx_offset_cur = mt_ctrl->rx_offset_min;
                             mt_ctrl->rx_offset_cur <= mt_ctrl->rx_offset_max;
                             mt_ctrl->rx_offset_cur++) {
                            if (mt_ctrl->flags &
                                SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS) {
                                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                                    "%c" BACKSPACE), progress_symbol));
                                progress_symbol ^= '-' ^ '|';
                            }
                            (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */
                            (*mt_ctrl->prog_hw1_fn)(mt_data); /* TX/RX */
                            if (mt_ctrl->rx_offset_cur ==
                                mt_ctrl->rx_offset_min) {
                                if (*mt_ctrl->per_tx_fn != NULL) {
                                    (*mt_ctrl->per_tx_fn)(mt_data);
                                }
                            }
                            if (*mt_ctrl->per_rx_fn != NULL) {
                                (*mt_ctrl->per_rx_fn)(mt_data);
                            }
                            rv = _soc_memtune_ddr_single_test(mt_data);
                            if (SOC_FAILURE(rv)) { /* fail unexpected */
                                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                                    " " BACKSPACE)));
                                sal_free(mt_ctrl->fail_array);
                                return rv;
                            }
                            index = (mt_ctrl->tx_offset_cur -
                                     mt_ctrl->tx_offset_min) *
                                (mt_ctrl->rx_offset_max -
                                 mt_ctrl->rx_offset_min + 1) +
                                mt_ctrl->rx_offset_cur -
                                mt_ctrl->rx_offset_min;
                            mt_ctrl->fail_array[index] =
                                mt_ctrl->cur_fail_count;
                            if (mt_ctrl->cur_fail_count == 0) {
                                success = TRUE;
                            }
                        } /* mt_ctrl->rx_offset_cur loop */
                    } /* mt_ctrl->tx_offset_cur loop */
                    if (success) {
                        if (mt_ctrl->flags &
                            SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS) {
                            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                                " " BACKSPACE)));
                        }

                        /* Analyze and print result matrix */
                        result = &mt_ctrl->result[mt_ctrl->result_count];
                        _soc_memtune_txrx_analyze(mt_data);
                        result->phase_sel = mt_ctrl->phase_sel_cur;
                        result->em_latency = mt_ctrl->em_latency_cur;
                        result->ddr_latency = mt_ctrl->ddr_latency_cur;
                        if (!mt_data->test_all_latency) {
                            goto loop_done;
                        }
                    }
                } /* mt_ctrl->ddr_latency_cur loop */
            } /* mt_ctrl->em_latency_cur loop */

            /* If nothing is passed for any EM_LATENCY, DDR_LATENCY setting */
            if (mt_ctrl->result_count == old_result_count) {
                result = &mt_ctrl->result[mt_ctrl->result_count];
                _soc_memtune_txrx_analyze(mt_data);
                result->phase_sel = mt_ctrl->phase_sel_cur;
            }
        }

    loop_done:
        index = mt_ctrl->phase_sel_cur == -1 ? 0 : mt_ctrl->phase_sel_cur;
        mt_ctrl->ps_data[index] = &mt_ctrl->result[old_result_count];
        for (i = old_result_count + 1; i < mt_ctrl->result_count; i++) {
            if (mt_ctrl->ps_data[index]->width *
                mt_ctrl->ps_data[index]->height <
                mt_ctrl->result[i].width * mt_ctrl->result[i].height) {
                mt_ctrl->ps_data[index] = &mt_ctrl->result[i];
            }
        }
    }

    if (mt_ctrl->phase_sel_min != -1) {
        /* Select the best result from 4 different phase */
        _soc_memtune_phase_sel_compare(mt_data);
        result = mt_ctrl->ps_data[mt_ctrl->phase_sel_cur];

        /* Program phase setting from the best result */
        (*mt_ctrl->prog_hw3_fn)(mt_data); /* phase */
    } else {
        mt_ctrl->phase_sel_cur = mt_ctrl->phase_sel_min;
        result = mt_ctrl->ps_data[0];
    }

    if (result->fail_count != offset_count) {
        /* Program latency setting if do_txrx_first is FALSE */
        if (result->em_latency != -1 || result->ddr_latency != -1) {
            mt_ctrl->em_latency_cur = result->em_latency;
            mt_ctrl->ddr_latency_cur = result->ddr_latency;
            (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */
        }
        /* Program TX/RX setting from the best result */
        mt_ctrl->tx_offset_cur = result->tx_offset;
        mt_ctrl->rx_offset_cur = result->rx_offset;
        (*mt_ctrl->prog_hw1_fn)(mt_data); /* TX/RX */
        if (*mt_ctrl->per_tx_fn != NULL) {
            (*mt_ctrl->per_tx_fn)(mt_data);
        }
        if (*mt_ctrl->per_rx_fn != NULL) {
            (*mt_ctrl->per_rx_fn)(mt_data);
        }
        /* Find latency setting if do_txrx_first is TRUE */
        if (result->em_latency == -1 && result->ddr_latency == -1) {
            for (mt_ctrl->em_latency_cur = mt_ctrl->em_latency_min;
                 mt_ctrl->em_latency_cur <= mt_ctrl->em_latency_max;
                 mt_ctrl->em_latency_cur++) {
                for (mt_ctrl->ddr_latency_cur = mt_ctrl->ddr_latency_min;
                     mt_ctrl->ddr_latency_cur <= mt_ctrl->ddr_latency_max;
                     mt_ctrl->ddr_latency_cur++) {
                    (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */
                    rv = _soc_memtune_ddr_single_test(mt_data);
                    if (SOC_FAILURE(rv)) { /* fail unexpected */
                        sal_free(mt_ctrl->fail_array);
                        return rv;
                    }
                    if (mt_ctrl->cur_fail_count == 0) {
                        result->ddr_latency = mt_ctrl->ddr_latency_cur;
                        break;
                    }
                } /* mt_ctrl->ddr_latency_cur loop */
                if (mt_ctrl->cur_fail_count == 0) {
                    result->em_latency = mt_ctrl->em_latency_cur;
                    break;
                }
            } /* mt_ctrl->em_latency_cur loop */
        }

        /* Tune number of nop between write-to-read and read-to-write */
        if (mt_ctrl->w2r_nops_min != mt_ctrl->w2r_nops_max &&
            mt_ctrl->r2w_nops_min != mt_ctrl->r2w_nops_max) {
            mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS;
            success = FALSE;
            for (mt_ctrl->w2r_nops_cur = mt_ctrl->w2r_nops_min;
                 mt_ctrl->w2r_nops_cur <= mt_ctrl->w2r_nops_max;
                 mt_ctrl->w2r_nops_cur++) {
                for (mt_ctrl->r2w_nops_cur = mt_ctrl->r2w_nops_max;
                     mt_ctrl->r2w_nops_cur >= mt_ctrl->r2w_nops_min;
                     mt_ctrl->r2w_nops_cur--) {
                    if (*mt_ctrl->prog_hw4_fn != NULL) {
                        (*mt_ctrl->prog_hw4_fn)(mt_data);  /* nops */
                    }
                    rv = _soc_memtune_ddr_single_test(mt_data);
                    if (SOC_FAILURE(rv)) { /* fail unexpected */
                        sal_free(mt_ctrl->fail_array);
                        return rv;
                    }
                    if (mt_ctrl->cur_fail_count == 0) {
                        success = TRUE;
                        continue;
                    }
                    if (mt_ctrl->r2w_nops_cur < mt_ctrl->r2w_nops_max) {
                        mt_ctrl->r2w_nops_cur++;
                    }
                    break;
                }
                if (mt_ctrl->r2w_nops_cur < mt_ctrl->r2w_nops_min) {
                    mt_ctrl->r2w_nops_cur++;
                }
                if (success) {
                    break;
                }
            }
            if (success) {
                if (mt_ctrl->r2w_nops_cur < 1) {
                    mt_ctrl->r2w_nops_cur = 1; /* hardware interprets 0 as 4 */
                }
                if (*mt_ctrl->prog_hw4_fn != NULL) {
                    (*mt_ctrl->prog_hw4_fn)(mt_data);  /* nops */
                }
            } else { /* should not be here */
                mt_ctrl->r2w_nops_cur = mt_ctrl->r2w_nops_max;
                mt_ctrl->w2r_nops_cur = mt_ctrl->w2r_nops_max;
            }
            mt_ctrl->flags &= ~SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS;
        }

        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "Memory tuning selection: ")));
            if (mt_ctrl->phase_sel_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "Phase sel=%d "), mt_ctrl->phase_sel_cur));
            }
            if (mt_ctrl->em_latency_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "EM lat=%d "), mt_ctrl->em_latency_cur));
            }
            if (mt_ctrl->ddr_latency_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "DDR lat=%d "), mt_ctrl->ddr_latency_cur));
            }
            if (mt_ctrl->tx_offset_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "TX offset=%d "), mt_ctrl->tx_offset_cur));
            }
            if (mt_ctrl->rx_offset_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "RX offset=%d "), mt_ctrl->rx_offset_cur));
            }
            if (mt_ctrl->w2r_nops_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "W2R nops=%d "), mt_ctrl->w2r_nops_cur));
            }
            if (mt_ctrl->r2w_nops_cur != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "R2W nops=%d "), mt_ctrl->r2w_nops_cur));
            }
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "\n")));
        }
    }

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "\n Interface  , Area , Width , Height , PS ,"
                                "  TX ,  RX , Lat , Em , Pass , Fail")));
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " , Freq , Tb , Sb , Dac")));
            }
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "\n")));
        }

        for (i = 0; i < mt_ctrl->result_count; i++) {
            result = &mt_ctrl->result[i];

            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                " %10s , %4d ,  %3d  ,  %3d   ,"),
                     mt_ctrl->intf_name, result->width * result->height,
                     result->width, result->height));
            if (result->phase_sel != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  %1d ,"), result->phase_sel));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " ** ,")));
            }
            if (result->fail_count != offset_count) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  %2d ,  %2d ,"),
                         result->tx_offset, result->rx_offset));
                if (result->ddr_latency != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        " %2d  ,"), result->ddr_latency));
                } else {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        " **  ,")));
                }
                if (result->em_latency != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        " %2d ,"),
                             result->em_latency));
                } else {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        " ** ,")));
                }
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  ** ,  ** , **  , ** ,")));
            }
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                " %4d , %4d"),
                     offset_count - result->fail_count,
                     result->fail_count));
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " ,  %3d ,  %c ,  %c ,"),
                         mt_data->freq,
                         mt_data->bg_tcam_bist ? 'Y' : 'N',
                         mt_data->bg_sram_bist ? 'Y' : 'N'));
                if (mt_ctrl->dac_value != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "  %2d"), mt_ctrl->dac_value));
                } else {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "  **")));
                }
            }
            index = result->phase_sel == -1 ? 0 : result->phase_sel;
            if (result != mt_ctrl->ps_data[index]) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " (ignored)\n")));
            } else if (result->fail_count != offset_count &&
                       mt_ctrl->phase_sel_cur != -1 &&
                       result == mt_ctrl->ps_data[mt_ctrl->phase_sel_cur]) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " <\n")));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "\n")));
            }
        }
    }

    if ((mt_ctrl->phase_sel_min != -1) &&
        (mt_ctrl->phase_sel_cur != -1)) {
        result = mt_ctrl->ps_data[mt_ctrl->phase_sel_cur];
    } else {
        result = mt_ctrl->ps_data[0];
    }
    if (result->fail_count == offset_count) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "ERROR - no passing \"eye\" was found, \n"
                            "please contact your Field Applications Engineer or "
                            "Sales Manager\n"
                            "for additional assistance.\n")));
        rv = SOC_E_FAIL;
    } else {
        if (result->height < mt_ctrl->tx_pass_threshold ||
            result->width < mt_ctrl->rx_pass_threshold) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "!!! SRAM tuning result does not pass qualified "
                                "threshold !!!\n")));
        }
        rv = SOC_E_NONE;
        if (mt_data->config) {
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                rv = _soc_tr_tune_generate_config(mt_data);
            }
#endif
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                          (BSL_META_U(mt_ctrl->unit,
                                      "unit %d %s fail to generate config: %s\n"),
                           mt_ctrl->unit, mt_ctrl->intf_name,
                           soc_errmsg(rv)));
            }
        }
    }

    sal_free(mt_ctrl->fail_array);
    return rv;
}

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_memtune_tcam_single_test(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    char *fail_name, fail_reason[80];
    int rv, index;
    uint32 addr, rval, mask[8], data[8];
    int do_init;

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;
    partitions = tcam_info->partitions;
    do_init = tcam_info->num_tcams <= 0 ? TRUE : FALSE;

    mt_ctrl->dpeo_sync_dly_cur = -1;
    mt_ctrl->fcd_dpeo_cur = -1;
    mt_ctrl->rbus_sync_dly_cur = -1;
    mt_ctrl->fcd_rbus_cur = -1;
    fail_name = NULL;
    fail_reason[0] = '\0';

    /* DBUS */
    mt_data->sub_interface = 0;
    rv = (*mt_ctrl->prog_hw1_fn)(mt_data); /* dpeo/rbus */
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                  (BSL_META_U(mt_ctrl->unit,
                              "%s prog hardware failure: %s\n"),
                              mt_ctrl->intf_name,
                   soc_errmsg(rv)));
        return rv;
    }

    if (do_init) {
        for (index = 0; index < TCAM_PARTITION_COUNT; index++) {
            tcam_info->partitions[index].num_entries = 0;
        }
        partitions[TCAM_PARTITION_DEV0_TBL72].num_entries = 16384;
        partitions[TCAM_PARTITION_DEV0_TBL144].num_entries = 8192;
        partitions[TCAM_PARTITION_DEV1_TBL72].num_entries = 16384;
        partitions[TCAM_PARTITION_DEV1_TBL144].num_entries = 8192;
        soc_tcam_init(mt_ctrl->unit);
        mt_ctrl->do_setup_for_rbus_test = TRUE;
    }

    mt_ctrl->cur_fail_count = 0;
    rv = (*mt_ctrl->test_fn)(mt_data);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                  (BSL_META_U(mt_ctrl->unit,
                              "%s dbus test failure: %s\n"),
                              mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }
    if (mt_ctrl->cur_fail_count) {
        fail_name = "DBUS";
        switch (mt_ctrl->cur_fail_reason & 0xffff0000) {
        case 0x10000:
            sal_sprintf(fail_reason, "bank %d error bit assert (0x%08x)",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_detail0);
            break;
        case 0x20000:
            sal_sprintf(fail_reason, "bank %d entry %d mismatch",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_detail0);
            break;
        default:
            return SOC_E_INTERNAL;
        }
        goto done;
    }

    /* DBUS_out (DPEO) */
    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_FALLf, 3);
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_RISEf, 3);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));

    mt_data->sub_interface = 1;
    for (mt_ctrl->dpeo_sync_dly_cur = mt_ctrl->dpeo_sync_dly_min;
         mt_ctrl->dpeo_sync_dly_cur <= mt_ctrl->dpeo_sync_dly_max;
         mt_ctrl->dpeo_sync_dly_cur++) {
        for (mt_ctrl->fcd_dpeo_cur = mt_ctrl->fcd_dpeo_min;
             mt_ctrl->fcd_dpeo_cur <= mt_ctrl->fcd_dpeo_max;
             mt_ctrl->fcd_dpeo_cur++) {
            rv = (*mt_ctrl->prog_hw1_fn)(mt_data);  /* dpeo/rbus */
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                          (BSL_META_U(mt_ctrl->unit,
                                      "%s prog hardware failure: "
                                      "%s\n"),
                           mt_ctrl->intf_name,
                           soc_errmsg(rv)));
                return rv;
            }
            mt_ctrl->cur_fail_count = 0;
            rv = (*mt_ctrl->test_fn)(mt_data);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                          (BSL_META_U(mt_ctrl->unit,
                                      "%s dpeo test failure: %s\n"),
                                      mt_ctrl->intf_name,
                           soc_errmsg(rv)));
                return rv;
            }
            if (mt_ctrl->cur_fail_count == 0) {
                break;
            }
        } /* mt_ctrl->fcd_dpeo_cur loop */
        if (mt_ctrl->cur_fail_count == 0) {
            break;
        }
    } /* mt_ctrl->dpeo_sync_dly_cur loop */
    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_FALLf, 0);
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_RISEf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    if (mt_ctrl->cur_fail_count) {
        fail_name = "DPEO";
        sal_sprintf(fail_reason, "error bit assert (0x%08x)",
                    mt_ctrl->cur_fail_detail0);
        goto done;
    }

    /* setup for RBUS test */
    if (mt_ctrl->do_setup_for_rbus_test) {
        if (!do_init) {
            soc_tcam_init(mt_ctrl->unit);
        }
        mt_ctrl->do_setup_for_rbus_test = FALSE;
        sal_memset(mask, 0, sizeof(mask));
        sal_memset(data, 0, sizeof(data));

        /* No reset on simulation, put test entry at index 0 to avoid hitting
         * leftover entries */
        index = SAL_BOOT_SIMULATION ? 0 : 0x1555;
        if (mt_data->mask[0] == 0xffffffff) {
            /* all '1's for lower 72-bit, all '0's for upper 72-bit */
            data[5] = 0xff;
            data[6] = data[7] = 0xffffffff;
        } else {
            sal_memcpy(mask, mt_data->mask, sizeof(mask));
            sal_memcpy(data, mt_data->data, sizeof(data));
        }
        soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_DEV0_TBL72, index,
                             &mask[4], &data[4]);
        soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_DEV0_TBL144, index,
                             mask, data);
        if (partitions[TCAM_PARTITION_DEV1_TBL72].num_entries) {
            soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_DEV1_TBL72,
                                 index, &mask[4], &data[4]);
        }
        if (partitions[TCAM_PARTITION_DEV1_TBL144].num_entries) {
            soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_DEV1_TBL144,
                                 index, mask, data);
        }
    }

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval, IN_SMFL00f,
                      0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));

    mt_data->sub_interface = 2;
    for (mt_ctrl->rbus_sync_dly_cur = mt_ctrl->rbus_sync_dly_min;
         mt_ctrl->rbus_sync_dly_cur <= mt_ctrl->rbus_sync_dly_max;
         mt_ctrl->rbus_sync_dly_cur++) {
        for (mt_ctrl->fcd_rbus_cur = mt_ctrl->fcd_rbus_min;
             mt_ctrl->fcd_rbus_cur <= mt_ctrl->fcd_rbus_max;
             mt_ctrl->fcd_rbus_cur++) {
            rv = (*mt_ctrl->prog_hw1_fn)(mt_data);  /* dpeo/rbus */
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                          (BSL_META_U(mt_ctrl->unit,
                                      "%s prog hardware failure: "
                                      "%s\n"),
                           mt_ctrl->intf_name,
                           soc_errmsg(rv)));
                return rv;
            }
            mt_ctrl->cur_fail_count = 0;
            rv = (*mt_ctrl->test_fn)(mt_data);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                          (BSL_META_U(mt_ctrl->unit,
                                      "%s rbus test failure: %s\n"),
                                      mt_ctrl->intf_name,
                           soc_errmsg(rv)));
                return rv;
            }
            if (mt_ctrl->cur_fail_count == 0) {
                break;
            }
        }
        if (mt_ctrl->cur_fail_count == 0) {
            break;
        }
    }
    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval, IN_SMFL00f,
                      1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    if (mt_ctrl->cur_fail_count) {
        fail_name = "RBUS";
        switch (mt_ctrl->cur_fail_reason & 0xffff0000) {
        case 0x10000:
            sal_sprintf(fail_reason, "bank %d error bit assert (0x%08x)",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_detail0);
            break;
        case 0x20000:
            sal_sprintf(fail_reason, "bank %d valid bit not assert (0x%08x)",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_detail0);
            break;
        default:
            return SOC_E_INTERNAL;
        }
        goto done;
    }

    /* search test */
    mt_data->sub_interface = 3;
    mt_ctrl->cur_fail_count = 0;
    rv = (*mt_ctrl->test_fn)(mt_data);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                  (BSL_META_U(mt_ctrl->unit,
                              "%s search test failure: %s\n"),
                              mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }

    if (mt_ctrl->cur_fail_count) {
        fail_name = "search";
        switch (mt_ctrl->cur_fail_reason & 0xffff0000) {
        case 0x10000:
            sal_sprintf(fail_reason, "bank %d status=0x%08x, fail count=%d",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_detail0,
                        mt_ctrl->cur_fail_count);
            break;
        case 0x20000:
            sal_sprintf(fail_reason, "bank %d index mismatch (0x%x/0x%x), "
                        "fail count=%d",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_detail0,
                        mt_ctrl->cur_fail_detail1,
                        mt_ctrl->cur_fail_count);
            break;
        case 0x30000:
            sal_sprintf(fail_reason, "bank %d BIST search fail, fail count=%d",
                        mt_ctrl->cur_fail_reason & 0xf,
                        mt_ctrl->cur_fail_count);
            break;
        default:
            return SOC_E_INTERNAL;
        }
        goto done;
    }

 done:
    if ((!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS) &&
         mt_ctrl->cur_fail_count < mt_data->test_count) ||
        (!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL) &&
         mt_ctrl->cur_fail_count >= mt_data->test_count)) {
        return SOC_E_NONE;
    }

    if (mt_ctrl->tx_offset_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "TX offset=%d "), mt_ctrl->tx_offset_cur));
    }
    if (mt_ctrl->rx_offset_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "RX offset=%d "), mt_ctrl->rx_offset_cur));
    }
    if (mt_ctrl->dpeo_sync_dly_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "DPEO_SYNC_DLY=%d "), mt_ctrl->dpeo_sync_dly_cur));
    }
    if (mt_ctrl->fcd_dpeo_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "FCD_DPEO=%d "), mt_ctrl->fcd_dpeo_cur));
    }
    if (mt_ctrl->rbus_sync_dly_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "RBUS_SYNC_DLY=%d "), mt_ctrl->rbus_sync_dly_cur));
    }
    if (mt_ctrl->fcd_rbus_cur != -1) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "FCD_RBUS=%d "), mt_ctrl->fcd_rbus_cur));
    }
    if (fail_name) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            ": %s test FAIL %s\n"), fail_name, fail_reason));
    } else {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            ": Fail count=%d\n"), mt_ctrl->cur_fail_count));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_memtune_tcam_main(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    int rv, offset_count, index, i;

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "TCAM tuning test:%s interface:\n"),
                 mt_ctrl->intf_name));
    }

    /* No TCAM reset on simulation, only need to re-init TCAM to setup
     * partitions used by test */
    if (SAL_BOOT_SIMULATION) {
        tcam_info->num_tcams = -1;
    }

    offset_count = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min + 1) *
                   (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1);
    mt_ctrl->fail_array = sal_alloc(offset_count * sizeof(int),
                                    "memtune working data");
    if (mt_ctrl->fail_array == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl->fail_array, 0, offset_count * sizeof(int));

    mt_ctrl->do_setup_for_rbus_test = FALSE;
    mt_ctrl->result_count = 0;

    for (mt_ctrl->invert_txclk_cur = mt_ctrl->invert_txclk_min;
         mt_ctrl->invert_txclk_cur <= mt_ctrl->invert_txclk_max;
         mt_ctrl->invert_txclk_cur++) {
        for (mt_ctrl->invert_rxclk_cur = mt_ctrl->invert_rxclk_min;
             mt_ctrl->invert_rxclk_cur <= mt_ctrl->invert_rxclk_max;
             mt_ctrl->invert_rxclk_cur++) {
            if (mt_ctrl->invert_txclk_cur != -1 &&
                mt_ctrl->invert_rxclk_cur != -1) {
                (*mt_ctrl->prog_hw3_fn)(mt_data); /* clock invert */
            }
            for (mt_ctrl->tx_offset_cur = mt_ctrl->tx_offset_min;
                 mt_ctrl->tx_offset_cur <= mt_ctrl->tx_offset_max;
                 mt_ctrl->tx_offset_cur++) {
                for (mt_ctrl->rx_offset_cur = mt_ctrl->rx_offset_min;
                     mt_ctrl->rx_offset_cur <= mt_ctrl->rx_offset_max;
                     mt_ctrl->rx_offset_cur++) {
                    (*mt_ctrl->prog_hw2_fn)(mt_data); /* TX/RX */
                    if (mt_ctrl->rx_offset_cur == mt_ctrl->rx_offset_min) {
                        if (*mt_ctrl->per_tx_fn != NULL) {
                            (*mt_ctrl->per_tx_fn)(mt_data);
                        }
                    }
                    if (*mt_ctrl->per_rx_fn != NULL) {
                        (*mt_ctrl->per_rx_fn)(mt_data);
                    }
                    rv = _soc_memtune_tcam_single_test(mt_data);
                    if (SOC_FAILURE(rv)) { /* fail unexpected */
                        sal_free(mt_ctrl->fail_array);
                        return rv;
                    }
                    index = (mt_ctrl->tx_offset_cur - mt_ctrl->tx_offset_min) *
                        (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1) +
                        mt_ctrl->rx_offset_cur - mt_ctrl->rx_offset_min;
                    mt_ctrl->fail_array[index] = mt_ctrl->cur_fail_count;
                } /* mt_ctrl->rx_offset_cur loop */
            } /* mt_ctrl->tx_offset_cur loop */
            result = &mt_ctrl->result[mt_ctrl->result_count];
            result->invert_txclk = mt_ctrl->invert_txclk_cur;
            result->invert_rxclk = mt_ctrl->invert_rxclk_cur;
            /* Analyze and print result matrix */
            _soc_memtune_txrx_analyze(mt_data);
            result->dpeo_sync_dly = mt_ctrl->dpeo_sync_dly_cur;
            result->fcd_dpeo = mt_ctrl->fcd_dpeo_cur;
            result->rbus_sync_dly = mt_ctrl->rbus_sync_dly_cur;
            result->fcd_rbus = mt_ctrl->fcd_rbus_cur;
        } /* mt_ctrl->invert_rxclk_cur loop */
    } /* mt_ctrl->invert_txclk_cur loop */

    if (mt_ctrl->invert_txclk_min != -1 && mt_ctrl->invert_rxclk_min != -1) {
        result = &mt_ctrl->result[0];
        for (i = 1; i < 4; i++) {
            if (result->width * result->height <
                mt_ctrl->result[i].width * mt_ctrl->result[i].height) {
                result = &mt_ctrl->result[i];
            }
        }
        mt_ctrl->invert_txclk_cur = result->invert_txclk;
        mt_ctrl->invert_rxclk_cur = result->invert_rxclk;

        /* Program clock invert setting from the best result */
        (*mt_ctrl->prog_hw3_fn)(mt_data);  /* clock invert */
    } else {
        mt_ctrl->invert_txclk_cur = mt_ctrl->invert_txclk_min;
        mt_ctrl->invert_rxclk_cur = mt_ctrl->invert_rxclk_min;
        result = &mt_ctrl->result[0];
    }

    if (result->fail_count != offset_count) {
        /* Program TX/RX setting from the best result */
        mt_ctrl->tx_offset_cur = result->tx_offset;
        mt_ctrl->rx_offset_cur = result->rx_offset;
        (*mt_ctrl->prog_hw2_fn)(mt_data);  /* TX/RX */
        if (*mt_ctrl->per_tx_fn != NULL) {
            (*mt_ctrl->per_tx_fn)(mt_data);
        }
        if (*mt_ctrl->per_rx_fn != NULL) {
            (*mt_ctrl->per_rx_fn)(mt_data);
        }
        rv = _soc_memtune_tcam_single_test(mt_data);
        if (SOC_FAILURE(rv) || mt_ctrl->cur_fail_count) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "!!! TCAM tuning FAIL --- unstable result !!!\n")));
            result->fail_count = offset_count;
        } else {
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "TCAM tuning selection: ")));
                if (mt_ctrl->invert_txclk_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "invert_txclk=%d "),
                             mt_ctrl->invert_txclk_cur));
                }
                if (mt_ctrl->invert_rxclk_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "invert_rxclk=%d "),
                             mt_ctrl->invert_rxclk_cur));
                }
                if (mt_ctrl->tx_offset_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "TX offset=%d "), mt_ctrl->tx_offset_cur));
                }
                if (mt_ctrl->rx_offset_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "RX offset=%d "), mt_ctrl->rx_offset_cur));
                }
                if (mt_ctrl->dpeo_sync_dly_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "dpeo_sync_dly=%d "),
                             mt_ctrl->dpeo_sync_dly_cur));
                }
                if (mt_ctrl->fcd_dpeo_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "fcd_dpeo=%d "), mt_ctrl->fcd_dpeo_cur));
                }
                if (mt_ctrl->rbus_sync_dly_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "rbus_sync_dly=%d "),
                             mt_ctrl->rbus_sync_dly_cur));
                }
                if (mt_ctrl->fcd_rbus_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "fcd_rbus=%d "), mt_ctrl->fcd_rbus_cur));
                }
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "\n")));
            }
        }
    }

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "\n Interface  ,  Area , Width , Height ,"
                                " Tc , Rc ,  TX ,  RX , Pass , Fail ,"
                                " Freq , Sb , Dac , Dist\n")));
        }
        for (i = 0; i < mt_ctrl->result_count; i++) {
            result = &mt_ctrl->result[i];

            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                " %10s , %4d  ,  %3d  ,  %3d   ,"),
                     mt_ctrl->intf_name, result->width * result->height,
                     result->width, result->height));
            if (result->invert_txclk != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " %c  ,"), result->invert_txclk ? 'I' : ' '));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " ** ,")));
            }
            if (result->invert_rxclk != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " %c  ,"), result->invert_rxclk ? 'I' : ' '));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " ** ,")));
            }
            if (result->fail_count != offset_count) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  %2d ,  %2d ,"),
                         result->tx_offset, result->rx_offset));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  ** ,  ** ,")));
            }
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                " %4d , %4d ,  %3d ,  %c ,"),
                     offset_count - result->fail_count, result->fail_count,
                     mt_data->freq,
                     mt_data->bg_sram_bist ? 'Y' : 'N'));
            if (mt_ctrl->dac_value != -1) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  %2d ,"), mt_ctrl->dac_value));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  ** ,")));
            }
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "  %2d"), mt_ctrl->ptr_dist));
            if (result->fail_count != offset_count &&
                mt_ctrl->invert_txclk_cur != -1 &&
                result->invert_txclk == mt_ctrl->invert_txclk_cur &&
                mt_ctrl->invert_rxclk_cur != -1 &&
                result->invert_rxclk == mt_ctrl->invert_rxclk_cur) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " <\n")));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "\n")));
            }
        }
    }

    if (result->fail_count == offset_count) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "ERROR - no passing \"eye\" was found, \n"
                            "please contact your Field Applications Engineer or "
                            "Sales Manager\n"
                            "for additional assistance.\n")));
        rv = SOC_E_FAIL;
    } else {
        if (result->height < mt_ctrl->tx_pass_threshold ||
            result->width < mt_ctrl->rx_pass_threshold) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "!!! TCAM tuning result does not pass qualified "
                                "threshold !!!\n")));
        }
        rv = SOC_E_NONE;
        if (mt_data->config) {
            rv = _soc_tr_tune_generate_config(mt_data);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_MEMTUNE,
                          (BSL_META_U(mt_ctrl->unit,
                                      "%s fail to generate config: %s\n"),
                           mt_ctrl->intf_name,
                           soc_errmsg(rv)));
            }
        }
    }

    sal_free(mt_ctrl->fail_array);
    return rv;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_tr_mem_interface_tune_setup(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t  *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 rval;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(mt_ctrl->unit, &dev_id, &rev_id);

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_SRAM:
        switch (mt_data->sub_interface) {
        case 0:
            mt_ctrl->intf_name = "SRAM0";
            tr_mt_data->config_reg1 = ES0_DDR36_CONFIG_REG1_ISr;
            tr_mt_data->config_reg2 = ES0_DDR36_CONFIG_REG2_ISr;
            tr_mt_data->config_reg3 = ES0_DDR36_CONFIG_REG3_ISr;
            tr_mt_data->status_reg2 = ES0_DDR36_STATUS_REG2_ISr;
            tr_mt_data->mode = ES0_DTU_MODEr;
            tr_mt_data->tmode0 = ES0_DTU_LTE_TMODE0r;
            tr_mt_data->tmode0_other_sram = ES1_DTU_LTE_TMODE0r;
            tr_mt_data->sram_ctl = ES0_SRAM_CTLr;
            break;
        case 1:
            mt_ctrl->intf_name = "SRAM1";
            tr_mt_data->config_reg1 = ES1_DDR36_CONFIG_REG1_ISr;
            tr_mt_data->config_reg2 = ES1_DDR36_CONFIG_REG2_ISr;
            tr_mt_data->config_reg3 = ES1_DDR36_CONFIG_REG3_ISr;
            tr_mt_data->status_reg2 = ES1_DDR36_STATUS_REG2_ISr;
            tr_mt_data->mode = ES1_DTU_MODEr;
            tr_mt_data->tmode0 = ES1_DTU_LTE_TMODE0r;
            tr_mt_data->tmode0_other_sram = ES0_DTU_LTE_TMODE0r;
            tr_mt_data->sram_ctl = ES1_SRAM_CTLr;
            break;
        default:
            return SOC_E_PARAM;
        }

        if ((SOC_IS_TRIUMPH(mt_ctrl->unit) && rev_id >= BCM56624_B2_REV_ID) ||
            SOC_IS_TRIUMPH2(mt_ctrl->unit)) {
            mt_ctrl->tx_pass_threshold = 9;
            mt_ctrl->rx_pass_threshold = 11;
        }

        if (soc_feature(mt_ctrl->unit, soc_feature_esm_rxfifo_resync)) {
            mt_data->phase_sel_ovrd = FALSE;
        }

        mt_ctrl->prog_hw1_fn = _soc_tr_ddr_txrx_prog_hw_cb;
        mt_ctrl->prog_hw2_fn = _soc_tr_ddr_latency_prog_hw_cb;
        mt_ctrl->prog_hw3_fn = _soc_tr_ddr_phase_sel_prog_hw_cb;
        mt_ctrl->prog_hw4_fn = _soc_tr_ddr_nops_prog_hw_cb;
        if (soc_feature(mt_ctrl->unit, soc_feature_esm_rxfifo_resync)) {
            mt_ctrl->per_rx_fn = _soc_tr_ddr_per_rx_cb;
        }
        mt_ctrl->test_fn = _soc_tr_sram_tune_test_cb;

        mt_ctrl->phase_sel_min =
            mt_data->phase_sel_ovrd ? SOC_TR_PHASE_SEL_MIN : -1;
        mt_ctrl->phase_sel_max =
            mt_data->phase_sel_ovrd ? SOC_TR_PHASE_SEL_MAX : -1;
        mt_ctrl->em_latency_min = SOC_TR_EM_LATENCY_MIN;
        if (soc_reg_field_valid(mt_ctrl->unit, tr_mt_data->tmode0,
                                EM_LATENCY8f)) {
            mt_ctrl->em_latency_max = SOC_TR_EM_LATENCY_MAX;;
        } else {
            mt_ctrl->em_latency_max = 7;
        }
        if (mt_data->man_em_latency != -1) {
            mt_ctrl->em_latency_min = mt_data->man_em_latency;
            mt_ctrl->em_latency_max = mt_data->man_em_latency;
        }
        if (soc_reg_field_valid(mt_ctrl->unit, tr_mt_data->config_reg2,
                                SEL_EARLY1_0f)) {
            mt_ctrl->ddr_latency_min = SOC_TR_DDR_LATENCY_MIN;
            mt_ctrl->ddr_latency_max = SOC_TR_DDR_LATENCY_MAX;
            if (mt_data->man_ddr_latency != -1) {
                mt_ctrl->ddr_latency_min = mt_data->man_ddr_latency;
                mt_ctrl->ddr_latency_max = mt_data->man_ddr_latency;
            }
        } else {
            mt_ctrl->ddr_latency_min = -1;
            mt_ctrl->ddr_latency_max = -1;
            mt_data->man_ddr_latency = -1;
        }
        mt_ctrl->tx_offset_min = SOC_TR_SRAM_TX_OFFSET_MIN;
        mt_ctrl->tx_offset_max = SOC_TR_SRAM_TX_OFFSET_MAX;
        if (mt_data->man_tx_offset != -1) {
            mt_ctrl->tx_offset_min = mt_data->man_tx_offset;
            mt_ctrl->tx_offset_max = mt_data->man_tx_offset;
        }
        mt_ctrl->rx_offset_min = SOC_TR_SRAM_RX_OFFSET_MIN;
        mt_ctrl->rx_offset_max = SOC_TR_SRAM_RX_OFFSET_MAX;
        if (mt_data->man_rx_offset != -1) {
            mt_ctrl->rx_offset_min = mt_data->man_rx_offset;
            mt_ctrl->rx_offset_max = mt_data->man_rx_offset;
        }
        mt_ctrl->w2r_nops_min = SOC_TR_SRAM_W2R_NOPS_MIN;
        mt_ctrl->w2r_nops_max = SOC_TR_SRAM_W2R_NOPS_MAX;
        mt_ctrl->r2w_nops_min = SOC_TR_SRAM_R2W_NOPS_MIN;
        mt_ctrl->r2w_nops_max = SOC_TR_SRAM_R2W_NOPS_MAX;
        break;

    case SOC_MEM_INTERFACE_TCAM:
        mt_ctrl->intf_name = "TCAM";
        tr_mt_data->config_reg1 = ETU_DDR72_CONFIG_REG1_ISr;
        tr_mt_data->config_reg3 = ETU_DDR72_CONFIG_REG3_ISr;
        tr_mt_data->etc_ctl = ETC_CTLr;
        tr_mt_data->inst_status = ETU_ET_INST_STATUSr;
        tr_mt_data->dbus_fail_mask = 0;
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->dbus_fail_mask, SEQ_DPEO_ERRf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->dbus_fail_mask, DPEO_RISEf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->dbus_fail_mask, DPEO_FALLf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->dbus_fail_mask, DPRERR0f, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->dbus_fail_mask, DPRERR1f, 1);
        tr_mt_data->rbus_fail_mask = 0;
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_fail_mask, SEQ_DPEO_ERRf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_fail_mask, RDACKf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_fail_mask, DPEO_RISEf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_fail_mask, DPEO_FALLf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_fail_mask, DPRERR0f, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_fail_mask, DPRERR1f, 1);
        tr_mt_data->rbus_valid_mask = 0;
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_valid_mask, RBUS0_VALIDf, 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->inst_status,
                          &tr_mt_data->rbus_valid_mask, RBUS1_VALIDf, 1);

        if ((SOC_IS_TRIUMPH(mt_ctrl->unit) && rev_id >= BCM56624_B2_REV_ID) ||
            SOC_IS_TRIUMPH2(mt_ctrl->unit)) {
            mt_ctrl->tx_pass_threshold = 11;
            mt_ctrl->rx_pass_threshold = 11;
        }

        mt_ctrl->prog_hw1_fn = _soc_tr_tcam_dpeo_rbus_prog_hw_cb;
        mt_ctrl->prog_hw2_fn = _soc_tr_tcam_txrx_prog_hw_cb;
        mt_ctrl->prog_hw3_fn = _soc_tr_tcam_invert_clk_prog_hw_cb;
        mt_ctrl->per_tx_fn = _soc_tr_tcam_per_tx_cb;
        mt_ctrl->per_rx_fn = _soc_tr_tcam_per_rx_cb;
        mt_ctrl->test_fn = _soc_tr_tcam_tune_test_cb;

        SOC_IF_ERROR_RETURN
            (READ_ETU_DDR72_CONFIG_REG3_ISr(mt_ctrl->unit, &rval));
        if (soc_reg_field_valid(mt_ctrl->unit, ETU_DDR72_CONFIG_REG3_ISr,
                                INVERT_TXCLKf)) {
            if (mt_data->phase_sel_ovrd) {
                mt_ctrl->invert_txclk_min = SOC_TR_TCAM_INVERT_TXCLK_MIN;
                mt_ctrl->invert_txclk_max = SOC_TR_TCAM_INVERT_TXCLK_MAX;
            } else {
                mt_ctrl->invert_txclk_min =
                    mt_ctrl->invert_txclk_max =
                    soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG3_ISr,
                                      rval, INVERT_TXCLKf);
            }
        } else {
            mt_ctrl->invert_txclk_min = -1;
            mt_ctrl->invert_txclk_max = -1;
        }
        if (soc_reg_field_valid(mt_ctrl->unit, ETU_DDR72_CONFIG_REG3_ISr,
                                INVERT_RXCLKf)) {
            if (mt_data->phase_sel_ovrd) {
                mt_ctrl->invert_rxclk_min = SOC_TR_TCAM_INVERT_RXCLK_MIN;
                mt_ctrl->invert_rxclk_max = SOC_TR_TCAM_INVERT_RXCLK_MAX;
            } else {
                mt_ctrl->invert_rxclk_min =
                    mt_ctrl->invert_rxclk_max =
                    soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG3_ISr,
                                      rval, INVERT_RXCLKf);
            }
        } else {
            mt_ctrl->invert_rxclk_min = -1;
            mt_ctrl->invert_rxclk_max = -1;
        }
        SOC_IF_ERROR_RETURN
            (READ_ETU_DDR72_CONFIG_REG1_ISr(mt_ctrl->unit, &rval));
        mt_ctrl->tx_offset_min = SOC_TR_TCAM_TX_OFFSET_MIN;
        if (soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr,
                              rval, MIDL_TX_ENf) &&
            soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr,
                              rval, SEL_TX_CLKDLY_BLKf)) {
            mt_ctrl->tx_offset_max = SOC_TR_TCAM_MIDL_TX_OFFSET_MAX;
        } else {
            mt_ctrl->tx_offset_max = SOC_TR_TCAM_VCDL_TX_OFFSET_MAX;
        }
        if (mt_data->man_tx_offset != -1) {
            mt_ctrl->tx_offset_min = mt_data->man_tx_offset;
            mt_ctrl->tx_offset_max = mt_data->man_tx_offset;
        }
        mt_ctrl->rx_offset_min = SOC_TR_TCAM_RX_OFFSET_MIN;
        if (soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr,
                              rval, MIDL_RX_ENf) &&
            soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr,
                              rval, SEL_RX_CLKDLY_BLKf)) {
            mt_ctrl->rx_offset_max = SOC_TR_TCAM_MIDL_RX_OFFSET_MAX;
        } else {
            mt_ctrl->rx_offset_max = SOC_TR_TCAM_VCDL_RX_OFFSET_MAX;
        }
        if (mt_data->man_rx_offset != -1) {
            mt_ctrl->rx_offset_min = mt_data->man_rx_offset;
            mt_ctrl->rx_offset_max = mt_data->man_rx_offset;
        }

        mt_ctrl->dpeo_sel_cur = 0; /* do DPEO_0 only */
        mt_ctrl->dpeo_sync_dly_min = SOC_TR_DPEO_SYNC_DLY_MIN;
        mt_ctrl->dpeo_sync_dly_max = SOC_TR_DPEO_SYNC_DLY_MAX;
        mt_ctrl->fcd_dpeo_min = SOC_TR_FCD_DPEO_MIN;
        mt_ctrl->fcd_dpeo_max = SOC_TR_FCD_DPEO_MAX;

        mt_ctrl->rbus_sync_dly_min = SOC_TR_RBUS_SYNC_DLY_MIN;
        mt_ctrl->rbus_sync_dly_max = SOC_TR_RBUS_SYNC_DLY_MAX;
        mt_ctrl->fcd_rbus_min = SOC_TR_FCD_RBUS_MIN;
        mt_ctrl->fcd_rbus_max = SOC_TR_FCD_RBUS_MAX;
        break;

    default:
        return SOC_E_PARAM;
    }

    if (mt_ctrl->tx_pass_threshold >
        mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min + 1) {
        mt_ctrl->tx_pass_threshold = 0;
    }
    if (mt_ctrl->rx_pass_threshold >
        mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1) {
        mt_ctrl->rx_pass_threshold = 0;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_mem_interface_tune(int unit, soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data;
    tr_ext_sram_bist_t *sram_bist, bg_sram_bist;
    soc_reg_t reg;
    uint32 addr, rval, sub_interface_mask;
    int rv, i;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    tr_mt_data = sal_alloc(sizeof(soc_tr_memtune_data_t),
                           "memtune working data");
    if (!tr_mt_data) {
        return SOC_E_MEMORY;
    }
    sal_memset(tr_mt_data, 0, sizeof(soc_tr_memtune_data_t));
    mt_ctrl->data = tr_mt_data;

    sram_bist = sal_alloc(sizeof(tr_ext_sram_bist_t), "memtune working data");
    if (!sram_bist) {
        return SOC_E_MEMORY;
    }
    sal_memset(sram_bist, 0, sizeof(tr_ext_sram_bist_t));
    mt_ctrl->sram_bist = sram_bist;

    if (mt_data->bg_sram_bist) {
        sal_memset(&bg_sram_bist, 0, sizeof(bg_sram_bist));
        if (mt_data->bg_d0r_0 == 0xffffffff) {
            bg_sram_bist.d0r_0 = bg_sram_bist.d0r_1 = 0x3ffff;
            bg_sram_bist.d0f_0 = bg_sram_bist.d0f_1 = 0;
            bg_sram_bist.d1r_0 = bg_sram_bist.d1r_1 = 0x3ffff;
            bg_sram_bist.d1f_0 = bg_sram_bist.d1f_1 = 0;
        } else {
            bg_sram_bist.d0r_0 = mt_data->bg_d0r_0 & 0x3ffff;
            bg_sram_bist.d0r_1 = mt_data->bg_d0r_1 & 0x3ffff;
            bg_sram_bist.d0f_0 = mt_data->bg_d0f_0 & 0x3ffff;
            bg_sram_bist.d0f_1 = mt_data->bg_d0f_1 & 0x3ffff;
            bg_sram_bist.d1r_0 = mt_data->bg_d1r_0 & 0x3ffff;
            bg_sram_bist.d1r_1 = mt_data->bg_d1r_1 & 0x3ffff;
            bg_sram_bist.d1f_0 = mt_data->bg_d1f_0 & 0x3ffff;
            bg_sram_bist.d1f_1 = mt_data->bg_d1f_1 & 0x3ffff;
        }
        if (mt_data->bg_adr0 == -1) {
            bg_sram_bist.adr0 = 0x0;   /* starting address */
        } else {
            bg_sram_bist.adr0 = mt_data->bg_adr0 & 0x3ffffe;
        }
        if (mt_data->bg_adr1 == -1 || mt_data->bg_adr_mode == 3) {
            bg_sram_bist.adr1 = 0x10;  /* end address */
        } else {
            bg_sram_bist.adr1 = mt_data->bg_adr1 & 0x3ffffe;
        }
        if (mt_data->bg_loop_mode == -1) {
            bg_sram_bist.loop_mode = 3; /* WW_RR */
        } else {
            bg_sram_bist.loop_mode = mt_data->bg_loop_mode & 3;
        }
        if (mt_data->bg_adr_mode == -1) {
            bg_sram_bist.adr_mode = 2; /* from ADR0 to ADR1 w/ step of 2 */
        } else {
            bg_sram_bist.adr_mode = mt_data->bg_adr_mode & 3;
        }
        bg_sram_bist.em_latency = -1;
        bg_sram_bist.w2r_nops = -1;
        bg_sram_bist.r2w_nops = -1;
    }

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_SRAM:
        if (mt_data->sub_interface > 1) {
            return SOC_E_PARAM;
        } else if (mt_data->sub_interface < 0) {
            sub_interface_mask = 0x3;
        } else {
            sub_interface_mask = 1 << mt_data->sub_interface;
        }

        /* (Optionally) program frequency */
        if (mt_data->freq != -1) {
            (void)soc_triumph_esm_init_set_sram_freq(unit, mt_data->freq);
        } else {
            mt_data->freq = tcam_info->sram_freq;
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
                LOG_CLI((BSL_META_U(unit,
                                    "Using SRAM frequency %d\n"),
                         tcam_info->sram_freq));
            }
        }

        rv = SOC_E_NONE;
        for (i = 0; i < 2; i++) {
            if (!(sub_interface_mask & (1 << i))) {
                continue;
            }
            mt_data->sub_interface = i;
            SOC_IF_ERROR_RETURN
                (_soc_tr_mem_interface_tune_setup(mt_data));

            /* Setup sram_bist control structure
             *          --   ---------   ---------   ---------   ---------   --
             *            \ /         \ /         \ /         \ /         \ /
             * DQ[35:18]   X   D0R_1   X   D0F_1  X    D1R_1   X   D1F_1   X
             *            / \         / \         / \         / \         / \
             *          --   ---------   ---------   ---------   ---------   --
             *
             *          --   ---------   ---------   ---------   ---------   --
             *            \ /         \ /         \ /         \ /         \ /
             * DQ[17:0]    X   D0R_0   X   D0F_0   X   D1R_0   X   D1F_0   X
             *            / \         / \         / \         / \         / \
             *          --   ---------   ---------   ---------   ---------   --
             */
            if (mt_data->d0r_0 == 0xffffffff) {
                sram_bist->d1r_0 = sram_bist->d0r_0 = 0xffffffff & 0x3ffff;
                sram_bist->d1r_1 = sram_bist->d0r_1 = 0x55555555 & 0x3ffff;
                sram_bist->d1f_0 = sram_bist->d0f_0 = 0x00000000 & 0x3ffff;
                sram_bist->d1f_1 = sram_bist->d0f_1 = 0xaaaaaaaa & 0x3ffff;
            } else {
                sram_bist->d0r_0 = mt_data->d0r_0 & 0x3ffff;
                sram_bist->d0r_1 = mt_data->d0r_1 & 0x3ffff;
                sram_bist->d0f_0 = mt_data->d0f_0 & 0x3ffff;
                sram_bist->d0f_1 = mt_data->d0f_1 & 0x3ffff;
                sram_bist->d1r_0 = mt_data->d1r_0 & 0x3ffff;
                sram_bist->d1r_1 = mt_data->d1r_1 & 0x3ffff;
                sram_bist->d1f_0 = mt_data->d1f_0 & 0x3ffff;
                sram_bist->d1f_1 = mt_data->d1f_1 & 0x3ffff;
            }
            if (mt_data->adr0 == -1) {
                sram_bist->adr0 = 0x0;   /* starting address */
            } else {
                sram_bist->adr0 = mt_data->adr0 & 0x3ffffe;
            }
            if (mt_data->adr1 == -1 || mt_data->adr_mode == 3) {
                sram_bist->adr1 = 0x10;  /* end address */
            } else {
                sram_bist->adr1 = mt_data->adr1 & 0x3ffffe;
            }
            if (mt_data->adr_mode == -1) {
                sram_bist->adr_mode = 2; /* from ADR0 to ADR1 w/ step of 2 */
            } else {
                sram_bist->adr_mode = mt_data->adr_mode & 3;
            }
            sram_bist->wdoebr = -1;
            sram_bist->em_latency = -1;
            sram_bist->w2r_nops = -1;
            sram_bist->r2w_nops = -1;
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_bist_setup(unit, mt_data->sub_interface,
                                                 sram_bist));

            /*
             * Following sram bist registers has been set properly,
             * don't touch them during test to save time:
             * D0R_0, D0R_1, D0F_0, D0F_1, D1R_0, D1R_1, D1F_0, D1F_1,
             * ADR0, ADR1, TMODE1
             */
            sram_bist->d0r_0 = 0xffffffff;
            sram_bist->d1r_0 = 0xffffffff;
            sram_bist->adr0 = -1;
            sram_bist->adr1 = -1;
            sram_bist->wdoebr = -1;

            /* Get BIASGEN DAC_VALUE */
            reg = tr_mt_data->config_reg3;
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
            mt_ctrl->dac_value = -1;
            if (soc_reg_field_get(unit, reg, rval, BIASGEN_DAC_ENf)) {
                mt_ctrl->dac_value = soc_reg_field_get(unit, reg, rval,
                                                       BIASGEN_DAC_CTRLf);
            }

            /* Optionally do PVT compensation */
            mt_ctrl->odtres_val = -1;
            mt_ctrl->pdrive_val = -1;
            mt_ctrl->ndrive_val = -1;
            mt_ctrl->slew_val = -1;
            if (mt_data->do_pvt_comp) {
                reg = tr_mt_data->config_reg2;
                SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, i));
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
                if (soc_reg_field_get(unit, reg, rval, OVRD_EN_ODTRES_PVTf)) {
                    mt_ctrl->odtres_val = soc_reg_field_get(unit, reg, rval,
                                                            PVT_ODTRES_VALf);
                }
                if (soc_reg_field_get(unit, reg, rval, OVRD_EN_DRIVER_PVTf)) {
                    mt_ctrl->pdrive_val = soc_reg_field_get(unit, reg, rval,
                                                            PVT_PDRIVE_VALf);
                    mt_ctrl->ndrive_val = soc_reg_field_get(unit, reg, rval,
                                                            PVT_NDRIVE_VALf);
                }
                if (soc_reg_field_get(unit, reg, rval, OVRD_EN_SLEW_PVTf)) {
                    mt_ctrl->slew_val = soc_reg_field_get(unit, reg, rval,
                                                          PVT_SLEW_VALf);
                }
            }

            /* Optionally setup background TCAM BIST */
            if (mt_data->bg_tcam_bist) {
                LOG_CLI((BSL_META_U(unit,
                                    "Do TCAM BIST loop count %d\n"),
                         mt_data->bg_tcam_loop_count));
                SOC_IF_ERROR_RETURN
                    (soc_tr_tcam_type1_memtest_dpeo(unit, 8,
                                                    mt_data->bg_tcam_oemap,
                                                    mt_data->bg_tcam_data));
                rval = 0;
                soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval,
                                  RD_EN_BIST_RSLTSf, 1);
                soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                                  mt_data->bg_tcam_loop_count);
                SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(unit, rval));
            }

            /* Optionally do background BIST on the other SRAM interface */
            if (mt_data->bg_sram_bist) {
                LOG_CLI((BSL_META_U(unit,
                                    "Do SRAM BIST on SRAM%d with loop mode %d\n"),
                         mt_data->sub_interface ^ 1,
                         bg_sram_bist.loop_mode));
                soc_triumph_ext_sram_enable_set(unit,
                                                mt_data->sub_interface ^ 1,
                                                TRUE, TRUE);
                soc_triumph_ext_sram_bist_setup(unit,
                                                mt_data->sub_interface ^ 1,
                                                &bg_sram_bist);

                reg = tr_mt_data->tmode0_other_sram;
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                rv = soc_reg32_read(unit, addr, &rval);
                if (SOC_SUCCESS(rv)) {
                    soc_reg_field_set(unit, reg, &rval, START_LAB_TESTf, 1);
                    soc_reg_field_set(unit, reg, &rval, FOREVERf, 1);
                    (void)soc_reg32_write(unit, addr, rval);
                }
            }

            rv = _soc_memtune_ddr_main(mt_data);

            if (mt_data->bg_sram_bist) {
                soc_reg_field_set(unit, reg, &rval, FOREVERf, 0);
                (void)soc_reg32_write(unit, addr, rval);
                soc_triumph_ext_sram_enable_set(unit,
                                                mt_data->sub_interface ^ 1,
                                                FALSE, TRUE);
            }

            if (mt_data->bg_tcam_bist) {
                rval = 0;
                soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                                  1);
                SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(unit, rval));
            }

            if (SOC_FAILURE(rv)) {
                break;
            }
        }
        break;

    case SOC_MEM_INTERFACE_TCAM:
        if (mt_data->sub_interface > 3) {
            return SOC_E_PARAM;
        }
        mt_data->sub_interface = 0;
        SOC_IF_ERROR_RETURN(_soc_tr_mem_interface_tune_setup(mt_data));

        if (mt_data->freq != -1) {
            (void)soc_triumph_esm_init_set_tcam_freq(mt_ctrl->unit,
                                                     mt_data->freq);
        } else {
            mt_data->freq = tcam_info->tcam_freq;
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
                LOG_CLI((BSL_META_U(unit,
                                    "Using TCAM frequency %d\n"),
                         tcam_info->tcam_freq));
            }
        }

        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
        mt_ctrl->dac_value = -1;
        if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                              BIASGEN_DAC_ENf)) {
            mt_ctrl->dac_value =
                soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  BIASGEN_DAC_CTRLf);
        }

        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
        mt_ctrl->ptr_dist =
            soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              SEL_WRFIFO_PTR_CLKf);

        mt_ctrl->odtres_val = -1;
        mt_ctrl->pdrive_val = -1;
        mt_ctrl->ndrive_val = -1;
        mt_ctrl->slew_val = -1;
        if (mt_data->do_pvt_comp) {
            SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 2));
            SOC_IF_ERROR_RETURN
                (READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
            if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  OVRD_EN_ODTRES_PVTf)) {
                mt_ctrl->odtres_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_ODTRES_VALf);
            }
            if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  OVRD_EN_DRIVER_PVTf)) {
                mt_ctrl->pdrive_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_PDRIVE_VALf);
                mt_ctrl->ndrive_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_NDRIVE_VALf);
            }
            if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  OVRD_EN_SLEW_PVTf)) {
                mt_ctrl->slew_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_SLEW_VALf);
            }
        }

        if (mt_data->bg_sram_bist) {
            LOG_CLI((BSL_META_U(unit,
                                "Do SRAM BIST on both SRAM with loop mode %d\n"),
                     bg_sram_bist.loop_mode));
            soc_triumph_ext_sram_enable_set(unit, 0, TRUE, TRUE);
            soc_triumph_ext_sram_bist_setup(unit, 0, &bg_sram_bist);
            SOC_IF_ERROR_RETURN(READ_ES0_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES0_DTU_LTE_TMODE0r, &rval,
                              START_LAB_TESTf, 1);
            soc_reg_field_set(unit, ES0_DTU_LTE_TMODE0r, &rval, FOREVERf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES0_DTU_LTE_TMODE0r(unit, rval));

            soc_triumph_ext_sram_enable_set(unit, 1, TRUE, TRUE);
            soc_triumph_ext_sram_bist_setup(unit, 1, &bg_sram_bist);
            SOC_IF_ERROR_RETURN(READ_ES1_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES1_DTU_LTE_TMODE0r, &rval,
                              START_LAB_TESTf, 1);
            soc_reg_field_set(unit, ES1_DTU_LTE_TMODE0r, &rval, FOREVERf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES1_DTU_LTE_TMODE0r(unit, rval));
        }

        rv = _soc_memtune_tcam_main(mt_data);

        if (mt_data->bg_sram_bist) {
            SOC_IF_ERROR_RETURN(READ_ES0_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES0_DTU_LTE_TMODE0r, &rval, FOREVERf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ES0_DTU_LTE_TMODE0r(unit, rval));
            soc_triumph_ext_sram_enable_set(unit, 0, FALSE, TRUE);

            SOC_IF_ERROR_RETURN(READ_ES1_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES1_DTU_LTE_TMODE0r, &rval, FOREVERf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ES1_DTU_LTE_TMODE0r(unit, rval));
            soc_triumph_ext_sram_enable_set(unit, 1, FALSE, TRUE);
        }

        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return rv;
}
#endif /* BCM_TRIUMPH_SUPPORT */

int
soc_mem_interface_tune(int unit, soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl;
    int rv = SOC_E_NONE;

    mt_ctrl = sal_alloc(sizeof(soc_memtune_ctrl_t), "memtune working data");
    if (!mt_ctrl) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl, 0, sizeof(soc_memtune_ctrl_t));
    mt_ctrl->unit = unit;
    mt_data->mt_ctrl = mt_ctrl;

    if (mt_data->verbose) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING;
        if (!mt_data->suppress_fail) {
            mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL;
        }
    }
    if (mt_data->summary) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY;
    }
    if (mt_data->summary_header) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER;
    }
    if (mt_data->show_matrix) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION;
    }
    if (mt_data->show_progress) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        rv = _soc_tr_mem_interface_tune(unit, mt_data);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (mt_ctrl->data) {
        sal_free(mt_ctrl->data);
    }
    if (mt_ctrl->sram_bist) {
        sal_free(mt_ctrl->sram_bist);
    }
    sal_free(mt_ctrl);

    return rv;
}

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_tr_memtune_lvl2_tcam_bist_setup(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    uint32 oe_map, data[32], rval;
    int i;

    sal_memset(data, 0, sizeof(data));

#define SETUP_72BIT_DATA(name,index,d0_msb,d1,d2_lsb) \
    name[index * 4] = d2_lsb; \
    name[index * 4 + 1] = d1; \
    name[index * 4 + 2] = d0_msb;
    switch (mt_data->tcam_data_choice) {
    case 0:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 1, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x22, 0x22222222, 0x22222222);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 5, 0x44, 0x44444444, 0x44444444);
        SETUP_72BIT_DATA(data, 6, 0x01, 0x23456789, 0x01234567);
        SETUP_72BIT_DATA(data, 7, 0x76, 0x54321098, 0x76543210);
        break;
    case 1:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0xff, 0xffffffff, 0xffffffff);
        break;
    case 2:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 1, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 2, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 3, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 4, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 5, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 6, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 7, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        break;
    case 3:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0x00, 0x00000000, 0x00000000);
        break;
    case 4:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 1, 0x55, 0x55555555, 0x55555555);
        SETUP_72BIT_DATA(data, 2, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 3, 0x55, 0x55555555, 0x55555555);
        SETUP_72BIT_DATA(data, 4, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 5, 0x55, 0x55555555, 0x55555555);
        SETUP_72BIT_DATA(data, 6, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 7, 0x55, 0x55555555, 0x55555555);
        break;
    case 5:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 1, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 2, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 3, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 5, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 6, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 7, 0xff, 0xffffffff, 0xffffffff);
        break;
    case 6:
        oe_map = 0x55;
        SETUP_72BIT_DATA(data, 0, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 2, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 6, 0xff, 0xffffffff, 0xffffffff);
        break;
    case 7:
        oe_map = 0x55;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        break;
    case 8:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 1, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 3, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 5, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 7, 0x00, 0x00000000, 0x00000000);
        break;
    case 81:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0x00, 0x00000000, 0x00000000);
        break;
    case 200:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        break;
    case 2000:
        oe_map = 0;
        break;
    default:
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Unknown TcamDataChoice %d\n"), mt_data->tcam_data_choice));
        return SOC_E_PARAM;
    }
#undef SETUP_72BIT_DATA

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "TCAM setting:\n")));
        for (i = 0; i < 8; i++) {
            if (oe_map & (1 << i)) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  K%d: %02x-%08x-%08x OE: ENABLE\n"),
                         i, data[i * 4 + 2], data[i * 4 + 1], data[i * 4]));
            } else {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "  K%d: zz-zzzzzzzz-zzzzzzzz OE: DISABLE\n"), i));
            }
        }
    }
    SOC_IF_ERROR_RETURN
        (soc_tr_tcam_type1_memtest_dpeo(mt_ctrl->unit, 8, oe_map, data));

    rval = 0;
    soc_reg_field_set(mt_ctrl->unit, ETU_LTE_BIST_CTLr, &rval,
                      RD_EN_BIST_RSLTSf, 1);
    soc_reg_field_set(mt_ctrl->unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                      mt_data->tcam_loop_count);
    SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(mt_ctrl->unit, rval));

    return SOC_E_NONE;
}

STATIC int
_soc_tr_memtune_lvl2_sram_bist_setup(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    tr_ext_sram_bist_t sram_bist;
    soc_reg_t reg;
    int addr;
    uint32 rval;

    sal_memset(&sram_bist, 0, sizeof(sram_bist));

#define SPLIT_36BIT_DATA(name,d0_msb,d1_lsb) \
    name## _0 = (d1_lsb) & 0x3ffff; \
    name## _1 = ((d0_msb & 0xf) << 14) | ((d1_lsb & 0xfffc0000) >> 18);
    switch (mt_data->sram_data_choice) {
    case -1:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x1, 0x23456789);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xa, 0xbcdef012);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x3, 0x456789ab);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xc, 0xdef01234);
        break;
    case 0:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x5, 0x55555555);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xa, 0xaaaaaaaa);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x5, 0x55555555);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xa, 0xaaaaaaaa);
        break;
    case 1:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 2:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 3:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xf, 0xffffffff);
        break;
    case 4:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 5:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xf, 0xffffffff);
        break;
    case 6:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xfffc1020);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xfffc1020);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 7:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xf, 0xffffffff);
        break;
    case 8:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    default:
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Unknown SramDataChoice %d\n"), mt_data->sram_data_choice));
        return SOC_E_PARAM;
    }
#undef SPLIT_36BIT_DATA

    switch (mt_data->alt_adr) {
    case 0: /* ADR0:0 ADR1:78, INC2 */
        sram_bist.adr1 = 78;
        sram_bist.adr_mode = 2;
        break;
    case 1: /* ADR0:0 ADR1:0x3ffffe, ALT_A0A1 */
        sram_bist.adr1 = 0x3ffffe;
        break;
    case 2: /* ADR0:0 ADR1:2, ALT_A0A1 */
        sram_bist.adr1 = 2;
        break;
    case 3: /* ADR0:0x3ffffe ADR1:0, ALT_A0A1 */
        sram_bist.adr0 = 0x3ffffe;
        break;
    default:
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Unknown AltAdr %d\n"), mt_data->alt_adr));
        return SOC_E_PARAM;
    }

    sram_bist.wdoebr = -1;
    sram_bist.em_latency = -1;

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "SRAM setting:\n")));
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "  D0R:%05x-%05x D0F:%05x-%05x D1R:%05x-%05x "
                            "D1F:%05x-%05x\n"),
                 sram_bist.d0r_1, sram_bist.d0r_0,
                 sram_bist.d0f_1, sram_bist.d0f_0,
                 sram_bist.d1r_1, sram_bist.d1r_0,
                 sram_bist.d1f_1, sram_bist.d1f_0));
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "  ADR0:%05x ADR1:%05x ADR_MODE:%d\n"),
                 sram_bist.adr0, sram_bist.adr1, sram_bist.adr_mode));
    }

    if (mt_data->loop_mode[0] != 4) {
        reg = ES0_SRAM_CTLr;
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        sram_bist.w2r_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_W2R_NOPSf);
        sram_bist.r2w_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_R2W_NOPSf);

        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "  (SRAM0) W2R_NOPS:%d R2W_NOPS:%d LOOP_MODE: %d\n"),
                     sram_bist.w2r_nops, sram_bist.r2w_nops,
                     mt_data->loop_mode[0]));
        }
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, 0, &sram_bist));
    }

    if (mt_data->loop_mode[1] != 4) {
        reg = ES1_SRAM_CTLr;
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        sram_bist.w2r_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_W2R_NOPSf);
        sram_bist.r2w_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_R2W_NOPSf);

        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "  (SRAM1) W2R_NOPS:%d R2W_NOPS:%d LOOP_MODE: %d\n"),
                     sram_bist.w2r_nops, sram_bist.r2w_nops,
                     mt_data->loop_mode[1]));
        }
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, 1, &sram_bist));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_memtune_lvl2_test(soc_memtune_data_t *mt_data, int *fail_count)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    tr_ext_sram_bist_t sram_bist[2], *sram_bist_ptr, *opt_sram_bist_ptr;
    int i, count, start, end, done;

    if (mt_data->loop_mode[0] == 4) {
        start = end = 1;
        sram_bist_ptr = &sram_bist[1];
        opt_sram_bist_ptr = NULL;
    } else if (mt_data->loop_mode[1] == 4) {
        start = end = 0;
        sram_bist_ptr = &sram_bist[0];
        opt_sram_bist_ptr = NULL;
    } else {
        start = 0;
        end = 1;
        sram_bist_ptr = &sram_bist[0];
        opt_sram_bist_ptr = &sram_bist[1];
    }

    for (i = start; i <= end; i++) {
        sal_memset(&sram_bist[i], 0, sizeof(tr_ext_sram_bist_t));

        /*
         * Following sram bist registers has been set properly,
         * don't touch them during test to save time:
         * D0R_0, D0R_1, D0F_0, D0F_1, D1R_0, D1R_1, D1F_0, D1F_1,
         * ADR0, ADR1, TMODE1
         */
        sram_bist[i].d0r_0 = 0xffffffff;
        sram_bist[i].d1r_0 = 0xffffffff;
        sram_bist[i].adr0 = -1;
        sram_bist[i].adr1 = -1;
        sram_bist[i].wdoebr = -1;
        sram_bist[i].em_latency = -1;
        sram_bist[i].r2w_nops = -1;
        sram_bist[i].w2r_nops = -1;

        sram_bist[i].bg_tcam_loop_count = mt_data->tcam_loop_count;
    }

    for (count = 0; count < mt_data->test_count; count++) {
        for (i = start; i <= end; i++) {
            if (mt_data->loop_mode[i] == 1) /* RR */ {
                sram_bist[i].loop_mode = 0; /* WW */
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_enable_set(mt_ctrl->unit, i, TRUE,
                                                     TRUE));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, i,
                                                     &sram_bist[i]));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_op(mt_ctrl->unit, i, &sram_bist[i],
                                             NULL));
            }
            sram_bist[i].loop_mode = mt_data->loop_mode[i];
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit, i, TRUE,
                                                 TRUE));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, i,
                                                 &sram_bist[i]));
        }

        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_op(mt_ctrl->unit, start, sram_bist_ptr,
                                     opt_sram_bist_ptr));
        if (mt_data->ok_on_both_pass && start != end) {
            sram_bist[0].err_cnt += sram_bist[1].err_cnt;
            sram_bist[1].err_cnt = sram_bist[0].err_cnt;
        }

        done = FALSE;
        for (i = start; i <= end; i++) {
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit, i, FALSE,
                                                 FALSE));
            if (sram_bist[i].err_cnt) {
                fail_count[i]++;
                if (fail_count[i] >= mt_data->max_fail_count) {
                    done = TRUE;
                }
            }
        }
        if (done) {
            break;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_mem_interface_tune_lvl2(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data;
    soc_memtune_result_t *result;
    int offset_count[2], index[2];
    int tx_offset[2], tx_offset_cur[2], tx_offset_min[2], tx_offset_max[2];
    int rx_offset[2], rx_offset_cur[2], rx_offset_min[2], rx_offset_max[2];
    int em_latency[2], em_latency_cur[2], em_latency_min[2], em_latency_max[2];
    int last_tx_offset[2], last_rx_offset[2];
    int tx_pass_threshold[2], rx_pass_threshold[2];
    int fail_count[2];
    int has_fail[2];
    int delta_max;
    static const struct {
        soc_reg_t tmode0;
        soc_reg_t config_reg1;
    } ddr_reg[2] = {
        {
            ES0_DTU_LTE_TMODE0r,
            ES0_DDR36_CONFIG_REG1_ISr
        },
        {
            ES1_DTU_LTE_TMODE0r,
            ES1_DDR36_CONFIG_REG1_ISr
        },
    };
    int addr, dt, dr, i, start, end;
    uint32 rval;
    int sram_data_choice, alt_adr, loop_mode[2], tcam_loop_count, test_count;

    if (mt_data->loop_mode[0] < 0 || mt_data->loop_mode[0] > 4) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Unknown LoopMode %d\n"), mt_data->loop_mode[0]));
        return SOC_E_PARAM;
    }

    if (mt_data->loop_mode[1] < 0 || mt_data->loop_mode[1] > 4) {
        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                            "Unknown LoopMode %d\n"), mt_data->loop_mode[1]));
        return SOC_E_PARAM;
    }

    start = 0;
    end = 1;
    if (mt_data->loop_mode[0] == 4) {
        if (mt_data->loop_mode[1] == 4) {
            return SOC_E_NONE; /* do nothing */
        } else {
            start++;
        }
    } else if (mt_data->loop_mode[1] == 4) {
        end--;
    }

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    tr_mt_data = sal_alloc(sizeof(soc_tr_memtune_data_t),
                           "memtune working data");
    if (!tr_mt_data) {
        return SOC_E_MEMORY;
    }
    sal_memset(tr_mt_data, 0, sizeof(soc_tr_memtune_data_t));
    mt_ctrl->data = tr_mt_data;

    mt_data->interface = SOC_MEM_INTERFACE_SRAM;
    mt_data->sub_interface = 0; /* any legal value to allow setup to pass */
    mt_data->man_em_latency = -1;
    mt_data->man_ddr_latency = -1;
    mt_data->man_tx_offset = -1;
    mt_data->man_rx_offset = -1;
    _soc_tr_mem_interface_tune_setup(mt_data);
    mt_ctrl->em_latency_cur = -1;
    mt_ctrl->ddr_latency_cur = -1;
    mt_ctrl->phase_sel_cur = -1;

    em_latency_min[0] = em_latency_max[0] = -1;
    em_latency_min[1] = em_latency_max[1] = -1;
    has_fail[0] = has_fail[1] = FALSE;

    for (i = start; i <= end; i++) {
        if (mt_data->tx_offset[i] != -1 || mt_data->rx_offset[i] != -1) {
            if (mt_data->tx_offset[i] < mt_ctrl->tx_offset_min ||
                mt_data->tx_offset[i] > mt_ctrl->tx_offset_max ||
                mt_data->rx_offset[i] < mt_ctrl->rx_offset_min ||
                mt_data->rx_offset[i] > mt_ctrl->rx_offset_max) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "Invalid TX/RX value %d/%d\n"),
                         mt_data->tx_offset[i], mt_data->rx_offset[i]));
                return SOC_E_PARAM;
            }
            tx_offset[i] = mt_data->tx_offset[i];
            rx_offset[i] = mt_data->rx_offset[i];
        } else {
            /* Get current TX/RX offset setting as center of sweep test */
            addr = soc_reg_addr(mt_ctrl->unit, ddr_reg[i].config_reg1,
                                REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            tx_offset[i] =
                soc_reg_field_get(mt_ctrl->unit, ddr_reg[i].config_reg1, rval,
                                  DLL90_OFFSET_TXf) |
                (soc_reg_field_get(mt_ctrl->unit, ddr_reg[i].config_reg1, rval,
                                   DLL90_OFFSET_TX4f) << 4);
            rx_offset[i] =
                soc_reg_field_get(mt_ctrl->unit, ddr_reg[i].config_reg1, rval,
                                  DLL90_OFFSET_QKf) |
                (soc_reg_field_get(mt_ctrl->unit, ddr_reg[i].config_reg1, rval,
                                   DLL90_OFFSET_QK4f) << 4);
        }

        addr = soc_reg_addr(mt_ctrl->unit, ddr_reg[i].tmode0, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        em_latency[i] =
            soc_reg_field_get(mt_ctrl->unit, ddr_reg[i].tmode0, rval,
                              EM_LATENCYf);
        if (soc_reg_field_valid(mt_ctrl->unit, ddr_reg[i].tmode0,
                                EM_LATENCY8f) &&
            soc_reg_field_get(mt_ctrl->unit, ddr_reg[i].tmode0, rval,
                              EM_LATENCY8f)) {
            em_latency[i] |= 8;
        }

        /* Find out the min/max range of the sweeping window, the specified
         * delta may include TX/RX points beyond the physical range supported
         * by hardware */
        tx_offset_min[i] = mt_ctrl->tx_offset_min;
        if (tx_offset[i] - mt_data->delta[i] >= tx_offset_min[i]) {
            tx_offset_min[i] = tx_offset[i] - mt_data->delta[i];
        }
        tx_offset_max[i] = mt_ctrl->tx_offset_max;
        if (tx_offset[i] + mt_data->delta[i] <= tx_offset_max[i]) {
            tx_offset_max[i] = tx_offset[i] + mt_data->delta[i];
        }
        rx_offset_min[i] = mt_ctrl->rx_offset_min;
        if (rx_offset[i] - mt_data->delta[i] >= rx_offset_min[i]) {
            rx_offset_min[i] = rx_offset[i] - mt_data->delta[i];
        }
        rx_offset_max[i] = mt_ctrl->rx_offset_max;
        if (rx_offset[i] + mt_data->delta[i] <= rx_offset_max[i]) {
            rx_offset_max[i] = rx_offset[i] + mt_data->delta[i];
        }
        offset_count[i] = (tx_offset_max[i] - tx_offset_min[i] + 1) *
            (rx_offset_max[i] - rx_offset_min[i] + 1);

        if (mt_data->test_all_latency) {
            em_latency_min[i] = mt_ctrl->em_latency_min;
            em_latency_max[i] = mt_ctrl->em_latency_max;
        }

        /* Set passing threshold if the sweeping window is larger than the
         * size of qualified window */
        tx_pass_threshold[i] = mt_ctrl->tx_pass_threshold;
        if (tx_pass_threshold[i] > tx_offset_max[i] - tx_offset_min[i] + 1) {
            tx_pass_threshold[i] = 0;
        }
        rx_pass_threshold[i] = mt_ctrl->rx_pass_threshold;
        if (rx_pass_threshold[i] > rx_offset_max[i] - rx_offset_min[i] + 1) {
            rx_pass_threshold[i] = 0;
        }
    }

    /* Setup TCAM BIST */
    SOC_IF_ERROR_RETURN(_soc_tr_memtune_lvl2_tcam_bist_setup(mt_data));

    sram_data_choice = mt_data->sram_data_choice;
    alt_adr = mt_data->alt_adr;
    loop_mode[0] = mt_data->loop_mode[0];
    loop_mode[1] = mt_data->loop_mode[1];
    tcam_loop_count = mt_data->tcam_loop_count;
    test_count = mt_data->test_count;

    last_tx_offset[0] = last_tx_offset[1] = -1;
    last_rx_offset[0] = last_rx_offset[1] = -1;
    delta_max = mt_data->delta[0] > mt_data->delta[1] ?
        mt_data->delta[0] : mt_data->delta[1];
    for (dt = -delta_max; dt <= delta_max; dt++) {
        for (dr = -delta_max; dr <= delta_max; dr++) {
            for (i = start; i <= end; i++) {
                /* Find out the actual TX/RX setting for testing, this can be
                 * different from the "intended" sweeping point
                 * (center +/- delta). For example, if the point after moving
                 * delta steps from center is out of supported min/max range,
                 * the actual tx (or rx) setting for testing will be at the
                 * valid window edge of that direction */
                if (tx_offset[i] + dt < tx_offset_min[i]) {
                    tx_offset_cur[i] = tx_offset_min[i];
                } else if (tx_offset[i] + dt > tx_offset_max[i]) {
                    tx_offset_cur[i] = tx_offset_max[i];
                } else {
                    tx_offset_cur[i] = tx_offset[i] + dt;
                }
                if (rx_offset[i] + dr < rx_offset_min[i]) {
                    rx_offset_cur[i] = rx_offset_min[i];
                } else if (rx_offset[i] + dr > rx_offset_max[i]) {
                    rx_offset_cur[i] = rx_offset_max[i];
                } else {
                    rx_offset_cur[i] = rx_offset[i] + dr;
                }
                if (tx_offset[i] + dt != tx_offset_cur[i] ||
                    rx_offset[i] + dr != rx_offset_cur[i]) {
                    /* Do not record result if the "intended" sweeping point
                     * is not the actual TX/RX setting for testing */
                    index[i] = -1;
                } else {
                    index[i] = (tx_offset_cur[i] - tx_offset_min[i]) *
                        (rx_offset_max[i] - rx_offset_min[i] + 1) +
                        rx_offset_cur[i] - rx_offset_min[i];
                    if (mt_data->fail_array[i][index[i]] > 0) {
                        has_fail[i] = TRUE;
                        if (mt_data->fail_array[i][index[i]] >=
                            mt_data->max_fail_count) {
                            /* Do not record result if the currently
                             * accumulated error count has already exceeded
                             * the specified max fail count */
                            index[i] = -1;
                        }
                    }
                }
            }

            if (index[start] == -1 && index[end] == -1) {
                /* If none of the interface needs to record the result
                 * go to next TX/RX */
                continue;
            }

            for (i = start; i <= end; i++) {
                em_latency_cur[i] = -1;
                /* If the TX/RX setting is the same as last test, no need to
                 * reporgram TX/RX in hardware */
                if (tx_offset_cur[i] == last_tx_offset[i] &&
                    rx_offset_cur[i] == last_rx_offset[i]) {
                    continue;
                }
                mt_ctrl->tx_offset_cur = tx_offset_cur[i];
                mt_ctrl->rx_offset_cur = rx_offset_cur[i];
                tr_mt_data->config_reg1 = ddr_reg[i].config_reg1;
                (*mt_ctrl->prog_hw1_fn)(mt_data); /* TX/RX */
                if (tx_offset_cur[i] != last_tx_offset[i]) {
                    if (*mt_ctrl->per_tx_fn != NULL) {
                        (*mt_ctrl->per_tx_fn)(mt_data);
                    }
                }
                if (rx_offset_cur[i] != last_rx_offset[i]) {
                    if (*mt_ctrl->per_rx_fn != NULL) {
                        (*mt_ctrl->per_rx_fn)(mt_data);
                    }
                }
                last_tx_offset[i] = tx_offset_cur[i];
                last_rx_offset[i] = rx_offset_cur[i];

                if (em_latency_min[i] == -1) {
                    continue;
                }

                /* Find EM_LATENCY for the TX/RX setting */
                for (mt_ctrl->em_latency_cur = em_latency_min[i];
                     mt_ctrl->em_latency_cur <= em_latency_max[i];
                     mt_ctrl->em_latency_cur++) {
                    tr_mt_data->tmode0 = ddr_reg[i].tmode0;
                    (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */
                    mt_data->sram_data_choice = -1;
                    mt_data->alt_adr = 2;
                    mt_data->loop_mode[i] = 1;
                    mt_data->loop_mode[i ^ 1] = 4;
                    mt_data->tcam_loop_count = 1;
                    mt_data->test_count = 1;
                    SOC_IF_ERROR_RETURN
                        (_soc_tr_memtune_lvl2_sram_bist_setup(mt_data));

                    fail_count[i] = 0;
                    _soc_tr_memtune_lvl2_test(mt_data, fail_count);

                    if (fail_count[i] == 0) {
                        em_latency_cur[i] = mt_ctrl->em_latency_cur;
                        break;
                    }
                }
            }

            /* Setup user-specified test parameters in hardware */
            mt_data->sram_data_choice = sram_data_choice;
            mt_data->alt_adr = alt_adr;
            mt_data->loop_mode[0] = loop_mode[0];
            mt_data->loop_mode[1] = loop_mode[1];
            mt_data->tcam_loop_count = tcam_loop_count;
            mt_data->test_count = test_count;
            SOC_IF_ERROR_RETURN(_soc_tr_memtune_lvl2_sram_bist_setup(mt_data));

            /* Do user-specified test */
            for (i = start; i <= end; i++) {
                fail_count[i] = 0;
            }
            _soc_tr_memtune_lvl2_test(mt_data, fail_count);

            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL ||
                mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS) {
                for (i = start; i <= end; i++) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "SRAM%d TX=%d RX=%d "), i,
                             tx_offset_cur[i], rx_offset_cur[i]));
                    if (em_latency_cur[i] != -1) {
                        LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                            "EM lat=%d "), em_latency_cur[i]));
                    }
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        ": Fail count=%d "), fail_count[i]));
                }
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "\n")));
            }

            for (i = start; i <= end; i++) {
                if (index[i] == -1) {
                    continue;
                }
                if (mt_data->fail_array[i][index[i]] <= 0) {
                    if (fail_count[i] == 0 && em_latency_cur[i] != -1) {
                        mt_data->fail_array[i][index[i]] = -em_latency_cur[i];
                    } else {
                        mt_data->fail_array[i][index[i]] = fail_count[i];
                    }
                } else {
                    mt_data->fail_array[i][index[i]] += fail_count[i];
                }
                if (fail_count[i] > 0) {
                    has_fail[i] = TRUE;
                    if (mt_data->fail_array[i][index[i]] >
                        mt_data->max_fail_count) {
                        mt_data->fail_array[i][index[i]] =
                            mt_data->max_fail_count;
                    }
                }
            }
        }
    }

    /* Clear TCAM BIST setting */
    rval = 0;
    soc_reg_field_set(mt_ctrl->unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf, 1);
    (void)WRITE_ETU_LTE_BIST_CTLr(mt_ctrl->unit, rval);

    for (i = start; i <= end; i++) {
        /* When sweeping only one SRAM, no analysis on the non-sweeping SRAM */
        if (mt_data->delta[i] == 0 && mt_data->delta[i ^ 1] != 0) {
            continue;
        }
        mt_ctrl->tx_offset_cur = tx_offset[i];
        mt_ctrl->rx_offset_cur = rx_offset[i];
        mt_ctrl->intf_name = i == 0 ? "SRAM0" : "SRAM1";
        mt_ctrl->result_count = 0;
        result = &mt_ctrl->result[mt_ctrl->result_count];
        if (!has_fail[i]) {
            /* All tested TX/RX passes, no need to analyze */
            mt_ctrl->tx_offset_cur = (tx_offset_min[i] + tx_offset_max[i]) / 2;
            mt_ctrl->rx_offset_cur = (rx_offset_min[i] + rx_offset_max[i]) / 2;
            result->fail_count = 0;
            result->width = rx_offset_max[i] - rx_offset_min[i] + 1;
            result->height = tx_offset_max[i] - tx_offset_min[i] + 1;
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "Memory tuning %s interface"),
                         mt_ctrl->intf_name));
                if (mt_ctrl->em_latency_cur != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        " em_latency=%d"), mt_ctrl->em_latency_cur));
                }
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    " failure count: all pass\n")));
            }
        } else {
            /* Analyze and print result matrix */
            mt_ctrl->tx_offset_min = tx_offset_min[i];
            mt_ctrl->tx_offset_max = tx_offset_max[i];
            mt_ctrl->rx_offset_min = rx_offset_min[i];
            mt_ctrl->rx_offset_max = rx_offset_max[i];
            mt_ctrl->fail_array = mt_data->fail_array[i];
            _soc_memtune_txrx_analyze(mt_data);
        }

        if (result->fail_count == offset_count[i] ||
            mt_data->restore_txrx_after_test) {
            /* Test fail, or restore TX/RX flag is set */
            mt_ctrl->tx_offset_cur = tx_offset[i];
            mt_ctrl->rx_offset_cur = rx_offset[i];
        }

        /* Restore TX/RX in hardware */
        tr_mt_data->config_reg1 = ddr_reg[i].config_reg1;
        (*mt_ctrl->prog_hw1_fn)(mt_data); /* TX/RX */
        if (*mt_ctrl->per_tx_fn != NULL) {
            (*mt_ctrl->per_tx_fn)(mt_data);
        }
        if (*mt_ctrl->per_rx_fn != NULL) {
            (*mt_ctrl->per_rx_fn)(mt_data);
        }

        /* Find EM_LATENCY for the TX/RX setting */
        if (em_latency_min[i] != -1) {
            for (em_latency_cur[i] = em_latency_min[i];
                 em_latency_cur[i] <= em_latency_max[i]; em_latency_cur[i]++) {
                mt_ctrl->em_latency_cur = em_latency_cur[i];
                tr_mt_data->tmode0 = ddr_reg[i].tmode0;
                (*mt_ctrl->prog_hw2_fn)(mt_data); /* latency */

                mt_data->sram_data_choice = -1;
                mt_data->alt_adr = 2;
                mt_data->loop_mode[i] = 1;
                mt_data->loop_mode[i ^ 1] = 4;
                mt_data->tcam_loop_count = 1;
                mt_data->test_count = 1;
                SOC_IF_ERROR_RETURN
                    (_soc_tr_memtune_lvl2_sram_bist_setup(mt_data));

                fail_count[i] = 0;
                _soc_tr_memtune_lvl2_test(mt_data, fail_count);
                if (fail_count[i] == 0) {
                    break;
                }
            }
        }

        if (result->fail_count == offset_count[i]) {
            LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                "ERROR - no passing \"eye\" was found, \n"
                                "please contact your Field Applications "
                                "Engineer or Sales Manager\n"
                                "for additional assistance.\n")));
        } else {
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "%s OldTX=%d OldRX=%d "),
                         mt_ctrl->intf_name, tx_offset[i], rx_offset[i]));
                if (em_latency_min[i] != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "OldEmLat=%d "), em_latency[i]));
                }
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "NewTX=%d NewRX=%d "),
                         mt_ctrl->tx_offset_cur, mt_ctrl->rx_offset_cur));
                if (em_latency_min[i] != -1) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "NewEmLat=%d "), mt_ctrl->em_latency_cur));
                }
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "Delta=%d "), mt_data->delta[i]));
                if (result->fail_count == 0) {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "no failure seen in tested points\n")));
                } else {
                    LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                        "Area=%d Width=%d Height=%d\n"),
                             result->width * result->height,
                             result->width, result->height));
                }
            }

            if (result->fail_count > 0 &&
                (result->height < tx_pass_threshold[i] ||
                 result->width < rx_pass_threshold[i])) {
                LOG_CLI((BSL_META_U(mt_ctrl->unit,
                                    "ERROR - \"eye\" of size %d * %d is smaller "
                                    "than minimum size of %d * %d,\n"
                                    "please contact your Field Applications "
                                    "Engineer or Sales Manager\n"
                                    "for additional assistance.\n"),
                         result->height, result->width,
                         mt_ctrl->tx_pass_threshold,
                         mt_ctrl->rx_pass_threshold));
            }

#ifndef NO_MEMTUNE
            if (mt_data->config) {
                char name_str[24], val_str[11];
                sal_sprintf(name_str, "%s%d.%d", spn_EXT_SRAM_TUNING, i,
                            mt_ctrl->unit);
                rval = soc_property_get(mt_ctrl->unit, name_str, 0);
                /* use 0 in the 3-bit field to represent EM_LATENCY is 8 */
                if (em_latency_min[i] != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_EM_LATENCY_MASK <<
                              SOC_TR_MEMTUNE_EM_LATENCY_SHIFT);
                    rval |= ((mt_ctrl->em_latency_cur &
                              SOC_TR_MEMTUNE_EM_LATENCY_MASK) <<
                             SOC_TR_MEMTUNE_EM_LATENCY_SHIFT);
                }
                rval &= ~(SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK <<
                          SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT);
                rval |= ((mt_ctrl->tx_offset_cur &
                          SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK) <<
                         SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT);
                rval &= ~(SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK <<
                          SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT);
                rval |= ((mt_ctrl->rx_offset_cur &
                          SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK) <<
                         SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT);
                sal_sprintf(val_str, "0x%08x", rval);
                if (_mem_config_set(name_str, val_str) < 0) {
                    return SOC_E_MEMORY;
                }
                sal_sprintf(name_str, "%s%d.%d", spn_EXT_SRAM_TUNING2_STATS, i,
                            mt_ctrl->unit);
                rval = SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((result->width & SOC_TR_MEMTUNE_STATS_WIDTH_MASK) <<
                     SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) |
                    ((result->height & SOC_TR_MEMTUNE_STATS_HEIGHT_MASK) <<
                     SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) |
                    ((result->fail_count &
                      SOC_TR_MEMTUNE_STATS_FAIL_MASK) <<
                     SOC_TR_MEMTUNE_STATS_FAIL_SHIFT);
                sal_sprintf(val_str, "0x%08x", rval);
                if (_mem_config_set(name_str, val_str) < 0) {
                    return SOC_E_MEMORY;
                }
            }
#endif /* NO_MEMTUNE */
        }
    }

    mt_data->sram_data_choice = sram_data_choice;
    mt_data->alt_adr = alt_adr;
    mt_data->loop_mode[0] = loop_mode[0];
    mt_data->loop_mode[1] = loop_mode[1];
    mt_data->tcam_loop_count = tcam_loop_count;
    mt_data->test_count = test_count;

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

int
soc_mem_interface_tune_lvl2(int unit, soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl;
    int rv = SOC_E_NONE;

    mt_ctrl = sal_alloc(sizeof(soc_memtune_ctrl_t), "memtune working data");
    if (!mt_ctrl) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl, 0, sizeof(soc_memtune_ctrl_t));
    mt_ctrl->unit = unit;
    mt_data->mt_ctrl = mt_ctrl;

    if (mt_data->verbose) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING;
    }
    if (mt_data->summary) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY;
    }
    if (mt_data->show_setting) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING;
    }
    if (mt_data->show_matrix) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        rv = _soc_tr_mem_interface_tune_lvl2(mt_data);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (mt_ctrl->data) {
        sal_free(mt_ctrl->data);
    }
    sal_free(mt_ctrl);

    return rv;
}
#endif /* BCM_TRIUMPH_SUPPORT */
