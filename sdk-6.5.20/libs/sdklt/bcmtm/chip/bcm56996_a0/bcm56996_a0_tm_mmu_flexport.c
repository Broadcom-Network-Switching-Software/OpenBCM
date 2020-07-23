/*! \file bcm56996_a0_tm_mmu_flexport.c
 *
 * File containing flexport sequence for bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56996_a0_tm_mmu_flexport.h"
#include "bcm56996_a0_tm_mmu_tdm.h"

#define BSL_LOG_MODULE  BSL_LS_BCMTM_FLEXPORT
#ifndef BSL_LS_SOC_COMMON
#define BSL_LS_SOC_COMMON BSL_LOG_MODULE
#endif


/*! @fn int th4g_mmu_get_thdi_bst_mode(int unit, int *thdi_bst_mode)
 *  @param unit Device number
 *  @param *thdi_bst_mode Return value thdi bst mode
 *  @brief API to return the THDI bst mode programmed
 */
static int
th4g_mmu_get_thdi_bst_mode(int unit, uint32_t *thdi_bst_mode)
{
    int inst = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval;

    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = MMU_THDI_BSTCONFIGr;
    bcmdrd_fid_t fid = TRACKING_MODEf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    *thdi_bst_mode = fval;
exit:
    SHR_FUNC_EXIT();
}

/*! @fn int th4g_mmu_get_thdo_bst_mode(int unit, int *thdo_bst_mode)
 *  @param unit Device number
 *  @param *thdo_bst_mode Return value thdo bst mode
 *  @brief API to return the thdo bst mode programmed
 */
static int
th4g_mmu_get_thdo_bst_mode(int unit, uint32_t *thdo_bst_mode)
{
    int inst = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval;

    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = MMU_THDO_BST_CONFIGr;
    bcmdrd_fid_t fid = TRACKING_MODEf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    *thdo_bst_mode = fval;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * @fn int th4g_mmu_qsch_port_flush(int unit,
 *      int lport, uint32_t set_val)
 *  @param unit Device number
 *  @param lport Port number
 *  @param set_val Value to be set to the QSCH port flush sidisters
 *  @brief API to Set the QSCH Port Flush registers
 */
static int
th4g_mmu_qsch_port_flush(int unit, int lport, uint32_t set_val)
{
    uint32_t temp;
    uint32_t ltmbuf;
    uint32_t new_val;
    uint32_t enable_val;
    int inst;
    int lcl_mmu_port;

    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    sid = MMU_QSCH_PORT_FLUSHr;

    inst = port_map->lport_map[lport].pipe;

    /* Read-modify-write. */

    ltmbuf = 0;
    enable_val = 0;

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "before MMU_QSCH_PORT_FLUSHr \n")));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, ENABLEf, &ltmbuf, &enable_val));


    new_val = 1;
    lcl_mmu_port = port_map->lport_map[lport].mlocal_port;
    temp = TH4G_MMU_FLUSH_ON;
    new_val <<= lcl_mmu_port;

    if (set_val == temp) {
        enable_val |= new_val;
    } else {
        new_val = ~new_val;
        enable_val &= new_val;
    }

    ltmbuf = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, ENABLEf, &ltmbuf, &enable_val));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "after MMU_QSCH_PORT_FLUSHr \n")));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * @fn int th4g_mmu_rqe_port_flush(int unit, int itm)
 *  @param unit Device number
 *  @param itm ITM number to flush
 *  @brief API to Set the RQE Snapshot sidister to Flush out packets in the
 *         RQE Replication FIFO
 */
static int
th4g_mmu_rqe_port_flush(int unit, int itm)
{
    int inst;
    int count = 0;
    uint32_t lbuf;
    uint32_t wr_val;
    uint32_t rd_val;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid[2] = {ITMCFG0_INT_STATf, ITMCFG1_INT_STATf};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);

    inst = itm;

    /* Set MMU_RQE_QUEUE_FLUSH_EN.VALUE to 1. */

    lbuf = 0;
    wr_val = 1;
    sid = MMU_RQE_QUEUE_SNAPSHOT_ENr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, VALUEf, &lbuf, &wr_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &lbuf));

    /* Wait for MMU_RQE_INT_STAT.FLUSH_DONE interrupt to be set. */

    sid = MMU_RQE_INT_STATr;
    while (1) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info,&lbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, FLUSH_DONEf, &lbuf, &rd_val));

        if (rd_val == 1) {
            break;
        }

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        count++;
        if (count > 30000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "RQE Flush: FLUSH_DONE isn't reset even after"
                                  " 30000 iterations\n")));
            return SHR_E_TIMEOUT;
        }
    }

    /* Reset MMU_RQE_QUEUE_FLUSH_EN.VALUE to 0. */

    lbuf = 0;
    wr_val = 0;
    sid = MMU_RQE_QUEUE_SNAPSHOT_ENr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, VALUEf, &lbuf, &wr_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &lbuf));

    /* Clear MMU_RQE_INT_STAT.FLUSH_DONE interrupt. */

    wr_val = 1; /* w1tc */
    lbuf = 0;
    sid = MMU_RQE_INT_STATr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, FLUSH_DONEf, &lbuf, &wr_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &lbuf));

    /* Clear ITM and GLB propagated interrupts. */
    rd_val = 0;
    lbuf = 0;
    sid = MMU_ITMCFG_CPU_INT_STATr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info,&lbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, RQE_INT_STATf, &lbuf, &rd_val));
    if (rd_val == 1) {
        wr_val = 1; /* w1tc */
        lbuf = 0;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, RQE_INT_STATf, &lbuf, &wr_val));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &lbuf));

        rd_val = 0;
        lbuf = 0;
        sid = MMU_GLBCFG_CPU_INT_STATr;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info,&lbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid[inst], &lbuf, &rd_val));

        if (rd_val == 1) {
            wr_val = 1; /* w1tc */
            lbuf = 0;
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid[inst], &lbuf, &wr_val));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &lbuf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * @fn int th4g_mmu_wait_ebctm_empty(int unit,
 *                                   int lport)
 *  @param unit Device number
 *  @param lport Port number
 *  @brief API to poll MMU EBCTM Port Empty register
 */
static int
th4g_mmu_wait_ebctm_empty(int unit, int lport)
{
    uint32_t ltmbuf;
    uint32_t empty_val;
    int inst, count = 0;
    int lcl_mmu_port;
    int port_empty;
    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    sid = MMU_EBCTM_PORT_EMPTY_STSr;
    inst = port_map->lport_map[lport].pipe;
    lcl_mmu_port = port_map->lport_map[lport].mlocal_port;
    while (1) {
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                              sid,
                                              ltid,
                                              &pt_dyn_info,
                                              &ltmbuf));
        SHR_IF_ERR_EXIT(bcmtm_field_get(unit,
                                        sid,
                                        DST_PORT_EMPTYf,
                                        &ltmbuf,
                                        &empty_val));
        port_empty = ((empty_val & (1 << lcl_mmu_port)) != 0);
        if (port_empty == 1) {
            break;
        }
        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 30000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "WAIT_EBCTM_EMPTY: Ports not empty in 30000 "
                                  "iterations\n")));
            return SHR_E_TIMEOUT;
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * @fn int th4g_mmu_mtro_port_metering_config(int unit,
 *                                           int lport,
 *                                           uint32_t set_val_mtro)
 *  @param unit Device number
 *  @param lport Port number
 *  @brief API to enable/disable MTRO refresh for port during port up/down
 *  operations.
 */
static int
th4g_mmu_mtro_port_metering_config(int unit,
                                  int lport,
                                  uint32_t set_val_mtro)
{

    int inst;
    int lcl_mmu_port;
    uint32_t ltmbuf;
    uint32_t bitmap_val;
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    sid = MMU_MTRO_PORT_ENTITY_DISABLEr;
    inst = port_map->lport_map[lport].pipe; /* Pipe number*/

    /* Read-modify-write.*/

    ltmbuf = 0;

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, METERING_DISABLEf, &ltmbuf, &bitmap_val));

    lcl_mmu_port = port_map->lport_map[lport].mlocal_port;

    /* Now, Set or Clear disable bit depending on function input. */
    if (set_val_mtro == 1) { /* Disable refresh-on all UP/DOWN Ports. */
        /* Set the 'lcl_mmu_port' bit. */
        uint32_t temp;
        temp = 1;
        temp <<= lcl_mmu_port;
        bitmap_val |= temp;
    }
    else { /* Enable refresh */
        /* Clear the 'lcl_mmu_port' bit. */
        uint32_t temp;
        temp = 1;
        temp <<= lcl_mmu_port;
        temp = ~temp;
        bitmap_val &= temp;
    }

    ltmbuf = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, METERING_DISABLEf, &ltmbuf, &bitmap_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * @fn int th4g_mmu_clear_mtro_bucket_mems(int unit,
 *                                        int lport)
 *  @param unit Device number
 *  @param lport Port number
 *  @brief API to clear MTRO bucket count memories to ensure that a port
 *         doesn't start out of profile in the new configuration.
 */
static int
th4g_mmu_clear_mtro_bucket_mems(int unit, int lport)
{
    int cos;
    int lcl_port;
    int pipe_number;
    int pport;
    uint32_t mem_fld;
    uint32_t mem_indx;
    uint32_t entry_mtro_prt_bkt[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t entry_mtro_l0_bkt[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmdrd_sid_t mem_mtro_prt_bkt, mem_mtro_l0_bkt;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmtm_port_map_info_t *port_map;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    pipe_number = port_map->lport_map[lport].pipe;

    lcl_port = port_map->pport_map[pport].mlocal_port;

    mem_mtro_prt_bkt = MMU_MTRO_EGRMETERINGBUCKETm;
    mem_mtro_l0_bkt = MMU_MTRO_BUCKET_L0m;

    sal_memset(entry_mtro_prt_bkt, 0, sizeof(entry_mtro_prt_bkt));
    sal_memset(entry_mtro_l0_bkt, 0, sizeof(entry_mtro_l0_bkt));

    /* PORT BUCKET Memory */
    mem_indx = lcl_port;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mem_indx, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                          mem_mtro_prt_bkt,
                                          ltid,
                                          &pt_dyn_info,
                                          entry_mtro_prt_bkt));
    mem_fld = 0;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                    mem_mtro_prt_bkt,
                                    BUCKETf,
                                    entry_mtro_prt_bkt,
                                    &mem_fld));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                           mem_mtro_prt_bkt,
                                           ltid,
                                           &pt_dyn_info,
                                           entry_mtro_prt_bkt));

    /* L0 Bucket Memory */
    for (cos = 0; cos < TH4G_MMU_NUM_COS; cos++) {

        mem_indx = (lcl_port * TH4G_MMU_NUM_COS) + cos;
        BCMTM_PT_DYN_INFO(pt_dyn_info, mem_indx, pipe_number);

        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                              mem_mtro_l0_bkt,
                                              ltid,
                                              &pt_dyn_info,
                                              entry_mtro_l0_bkt));
        mem_fld = 0;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                        mem_mtro_l0_bkt,
                                        MIN_BUCKETf,
                                        entry_mtro_l0_bkt,
                                        &mem_fld));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                        mem_mtro_l0_bkt,
                                        MAX_BUCKETf,
                                        entry_mtro_l0_bkt,
                                        &mem_fld));

        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               mem_mtro_l0_bkt,
                                               ltid,
                                               &pt_dyn_info,
                                               entry_mtro_l0_bkt));
    }

exit:
    SHR_FUNC_EXIT();

}


/*!
 * @fn int th4g_mmu_thdi_bst_clr(int unit,
 *                               int lport)
 *  @param unit Device number
 *  @param lport Port number
 *  @brief API to clear port/PG hdrm and shared BST counters for a given MMU
 *         source port
 */
static int
th4g_mmu_thdi_bst_clr(int unit, int lport)
{
    int pipe_number;
    int local_mmu_port;
    int pport, mmu_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_sid_t mmu_thdi_port_pg_bst;
    bcmdrd_sid_t mmu_thdi_port_sp_bst;
    bcmdrd_sid_t mmu_thdi_port_pg_hdrm_bst;

    /*Set up pipe & port information*/

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->pport_map[pport].mport;
    local_mmu_port = mmu_port % TH4G_MMU_PORT_PIPE_OFFSET;
    pipe_number = port_map->lport_map[lport].pipe;

    sal_memset(data, 0, sizeof(data));

    mmu_thdi_port_pg_bst = MMU_THDI_PORT_PG_SHARED_BSTm;
    mmu_thdi_port_sp_bst = MMU_THDI_PORTSP_BSTm;
    mmu_thdi_port_pg_hdrm_bst = MMU_THDI_PORT_PG_HDRM_BSTm;

    /* Writing 0 to all fields. */
    BCMTM_PT_DYN_INFO(pt_dyn_info, local_mmu_port, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                           mmu_thdi_port_pg_bst,
                                           ltid,
                                           &pt_dyn_info,
                                           data));
    BCMTM_PT_DYN_INFO(pt_dyn_info, local_mmu_port, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                           mmu_thdi_port_pg_hdrm_bst,
                                           ltid,
                                           &pt_dyn_info,
                                           data));
    BCMTM_PT_DYN_INFO(pt_dyn_info, local_mmu_port, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                           mmu_thdi_port_sp_bst,
                                           ltid,
                                           &pt_dyn_info,
                                           data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *  @fn int th4g_mmu_thdo_bst_clr_queue(int unit, int lport)
 *  @param int unit Chip unit number
 *  @param int lport Port number
 *  @brief Clears thdo bst total queue counters of the port
 */
static int
th4g_mmu_thdo_bst_clr_queue(int unit, int lport)
{
    int itm;
    int curr_idx;
    int num_queues;
    int mem_idx;
    int chip_q_num_base_for_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_sid_t thdo_total_queue_bst_mem;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));
    bcmtm_port_map_get(unit, &port_map);

    thdo_total_queue_bst_mem = MMU_THDO_BST_TOTAL_QUEUEm;

    num_queues = port_map->lport_map[lport].num_mcq + port_map->lport_map[lport].num_ucq;
    chip_q_num_base_for_port = port_map->lport_map[lport].base_ucq;
    mem_idx = chip_q_num_base_for_port;

    for (curr_idx = 0; curr_idx < num_queues; curr_idx++) {
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "curr_idx = %0d, mem_idx = %0d\n"),
                   curr_idx, mem_idx));

        for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, mem_idx, itm);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit,
                                           thdo_total_queue_bst_mem,
                                           ltid,
                                           &pt_dyn_info,
                                           data));
            }
        }
        mem_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int th4g_mmu_thdo_bst_clr_port(int unit,
 *                                   int pipe, int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_device_port global device port
 *  @brief Clears thdo bst port counters of the port
 */
static int
th4g_mmu_thdo_bst_clr_port(int unit, int global_device_port)
{
    int itm;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));
    mem = MMU_THDO_BST_SHARED_PORTm;

    for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
        if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, global_device_port, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_dyn_info, data));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*! @fn int th4g_mmu_thdo_bst_clr_portsp(int unit,
 *                                   int pipe, int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_device_port global device port
 *  @brief Clears thdo bst port/sp counters of the port
 */
static int
th4g_mmu_thdo_bst_clr_portsp(int unit, int global_device_port)
{
    int itm;
    int sp;
    int mem_idx;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];

    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));
    mem = MMU_THDO_BST_SHARED_PORTSP_MCm;

    for (sp = 0; sp < TH4G_MMU_NUM_SPS; sp++) {
        mem_idx = (global_device_port * 4) + sp;
        for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, mem_idx, itm);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_dyn_info, data));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}




/*! @fn int th4g_mmu_thdo_bst_clr(int unit,
 *              port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param port_resource_t *port_resource_t Port resource struct containing
           epipe and port info
 *  @brief Clears thdo queue/port bst counters of an egress port
 */
static int
th4g_mmu_thdo_bst_clr(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    /* Clear per-queue memories. */
    SHR_IF_ERR_EXIT
        (th4g_mmu_thdo_bst_clr_queue(unit, lport));
    /* Clear per-port memories. */
    SHR_IF_ERR_EXIT
        (th4g_mmu_thdo_bst_clr_port(unit, lport));
    /* Clear per-port-per-spid memories. */
    SHR_IF_ERR_EXIT
        (th4g_mmu_thdo_bst_clr_portsp(unit, lport));

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int th4g_mmu_thdo_pktstat_clr_queue(int unit, int pipe,
 *              int global_mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int global_mmu_port Global mmu port
 *  @brief Internal function. Clears thdo stats counters of all queues
 *         in the egress port
 */
static int
th4g_mmu_thdo_pktstat_clr_queue(int unit, int lport)
{
    int itm;
    int curr_idx;
    int num_queues;
    int mem_idx;
    int chip_q_num_base_for_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    sal_memset(data, 0, sizeof(data));
    mem = MMU_THDO_QUEUE_DROP_COUNTm;

    num_queues = port_map->lport_map[lport].num_ucq + port_map->lport_map[lport].num_mcq;

    chip_q_num_base_for_port = port_map->lport_map[lport].base_ucq;
    mem_idx = chip_q_num_base_for_port;

    for (curr_idx = 0; curr_idx < num_queues; curr_idx++) {
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "curr_idx = %0d, mem_idx = %0d\n"),
                   curr_idx, mem_idx));
        for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, mem_idx, itm);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_dyn_info, data));
            }
        }
        mem_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int th4g_mmu_thdo_pktstat_clr_port(int unit, int pipe,
 *              int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int global_device_port Global device port
 *  @brief Internal function. Clears thdo stats counters of
 *         the egress ports
 */
static int
th4g_mmu_thdo_pktstat_clr_port(int unit, int global_device_port)
{
    int itm;
    int local_device_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmdrd_sid_t mem;
    bcmdrd_sid_t mc_mem;
    bcmdrd_sid_t uc_mem;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));

    mem = MMU_THDO_SRC_PORT_DROP_COUNTm;
    mc_mem = MMU_THDO_PORT_DROP_COUNT_MCm;
    uc_mem = MMU_THDO_PORT_DROP_COUNT_UCm;

    local_device_port = global_device_port % TH4G_MMU_PORTS_PER_PIPE;
    for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
        if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, local_device_port, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_dyn_info, data));

            BCMTM_PT_DYN_INFO(pt_dyn_info, global_device_port, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, mc_mem, ltid, &pt_dyn_info, data));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, uc_mem, ltid, &pt_dyn_info, data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int th4g_mmu_thdo_pktstat_clr(int unit,
 *              port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param port_resource_t *port_resource_t Port resource struct containing
 *         epipe and port info
 *  @brief Clears thdo stats counters of all queues/ports in the egress port
 */
static int
th4g_mmu_thdo_pktstat_clr(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    /* Clear per-queue memories. */
    SHR_IF_ERR_EXIT
        (th4g_mmu_thdo_pktstat_clr_queue(unit, lport));
    /* Clear per-port memories. */
    SHR_IF_ERR_EXIT
        (th4g_mmu_thdo_pktstat_clr_port(unit, lport));
exit:
    SHR_FUNC_EXIT();
}

/*! @fn int th4g_mmu_clear_qsch_credit_memories(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear QSCH credit memories and accum memories for the
 *         ports that are flexing during flexport.
 */
static int
th4g_mmu_clear_qsch_credit_memories(int unit, int lport)
{
    int itm;
    int lcl_port;
    int pport;
    int pipe_number;
    uint32_t entry[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_qsch_cr_l0[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_qsch_cr_l1[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_qsch_cr_l2[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_qsch_acc_l0[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_qsch_acc_l1[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_qsch_acc_l2[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t mem_indx;
    int cos, schq;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t qsch_l0_credit_mem;
    bcmdrd_sid_t qsch_l1_credit_mem;
    bcmdrd_sid_t qsch_l2_credit_mem;
    bcmdrd_sid_t qsch_l0_accum_comp_mem;
    bcmdrd_sid_t qsch_l1_accum_comp_mem;
    bcmdrd_sid_t qsch_l2_accum_comp_mem;
    bcmtm_port_map_info_t *port_map;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    lcl_port = port_map->pport_map[pport].mlocal_port;

    pipe_number = port_map->lport_map[lport].pipe;

    qsch_l0_credit_mem = MMU_QSCH_L0_CREDIT_MEMm;
    qsch_l1_credit_mem = MMU_QSCH_L1_CREDIT_MEMm;
    qsch_l2_credit_mem = MMU_QSCH_L2_CREDIT_MEMm;

    qsch_l0_accum_comp_mem = MMU_QSCH_L0_ACCUM_COMP_MEMm;
    qsch_l1_accum_comp_mem = MMU_QSCH_L1_ACCUM_COMP_MEMm;
    qsch_l2_accum_comp_mem = MMU_QSCH_L2_ACCUM_COMP_MEMm;

    sal_memset(entry_qsch_cr_l0, 0, sizeof(entry_qsch_cr_l0));
    sal_memset(entry_qsch_cr_l1, 0, sizeof(entry_qsch_cr_l1));
    sal_memset(entry_qsch_cr_l2, 0, sizeof(entry_qsch_cr_l2));
    sal_memset(entry_qsch_acc_l0, 0, sizeof(entry_qsch_acc_l0));
    sal_memset(entry_qsch_acc_l1, 0, sizeof(entry_qsch_acc_l1));
    sal_memset(entry_qsch_acc_l2, 0, sizeof(entry_qsch_acc_l2));

    sal_memset(entry, 0, sizeof(entry));

    sal_memset(data, 0, sizeof(data));


    /* Writing 0 to all fields. */

    for (cos = 0; cos < TH4G_MMU_NUM_COS; cos++) {

        mem_indx = (lcl_port * TH4G_MMU_NUM_COS) + cos;

        BCMTM_PT_DYN_INFO(pt_dyn_info, mem_indx, pipe_number);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               qsch_l0_credit_mem,
                                               ltid,
                                               &pt_dyn_info,
                                               data));
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               qsch_l0_accum_comp_mem,
                                               ltid,
                                               &pt_dyn_info,
                                               data));
    }

    for (schq = 0; schq < TH4G_MMU_NUM_L0_NODES_PER_HSP_PORT; schq++) {

        mem_indx = (lcl_port * TH4G_MMU_NUM_L0_NODES_PER_HSP_PORT) + schq;

        BCMTM_PT_DYN_INFO(pt_dyn_info, mem_indx, pipe_number);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               qsch_l1_credit_mem,
                                               ltid,
                                               &pt_dyn_info,
                                               data));
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               qsch_l1_accum_comp_mem,
                                               ltid,
                                               &pt_dyn_info,
                                               data));
    }

    for (schq = 0; schq < TH4G_MMU_NUM_L0_NODES_PER_HSP_PORT; schq++) {
        for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
                mem_indx =
                          (((lcl_port * TH4G_MMU_NUM_L0_NODES_PER_HSP_PORT) +
                            schq) * 2) + itm;

                BCMTM_PT_DYN_INFO(pt_dyn_info, mem_indx, pipe_number);
                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                                       qsch_l2_credit_mem,
                                                       ltid,
                                                       &pt_dyn_info,
                                                       data));
                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                                       qsch_l2_accum_comp_mem,
                                                       ltid,
                                                       &pt_dyn_info,
                                                       data));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int th4g_mmu_wred_clr(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear the WRED memories to clear stale counts for new ports

 */
static int
th4g_mmu_wred_clr(int unit, int lport)
{
    int q, sp;
    int itm;
    int cosq_mem_index, portsp_mem_index;

    uint32_t mem_data;
    uint32_t wred_q_data[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t wred_portsp_data[BCMTM_MAX_ENTRY_WSIZE];

    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_sid_t mmu_wred_avg_qsize, mmu_wred_avg_portsp_size;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    /*
     * Calculate base indexes for the set of UC queues and port SPs for the
     * given port.
     */

    sal_memset(wred_q_data, 0, sizeof(wred_q_data));
    sal_memset(wred_portsp_data, 0, sizeof(wred_portsp_data));

    mmu_wred_avg_portsp_size = MMU_WRED_AVG_PORTSP_SIZEm;
    mmu_wred_avg_qsize = MMU_WRED_AVG_QSIZEm;
    mem_data = 0;

    /* Per-UCQ loop */
    for (q = 0; q < TH4G_MMU_NUM_Q_PER_HSP_PORT; q++) {

        cosq_mem_index = (lport * TH4G_MMU_NUM_Q_PER_HSP_PORT) + q;

        for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, cosq_mem_index, itm);
                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                                      mmu_wred_avg_qsize,
                                                      ltid,
                                                      &pt_dyn_info,
                                                      wred_q_data));

                SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                                mmu_wred_avg_qsize,
                                                AVG_QSIZE_FRACTIONf,
                                                wred_q_data,
                                                &mem_data));
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                                mmu_wred_avg_qsize,
                                                AVG_QSIZEf,
                                                wred_q_data,
                                                &mem_data));

                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                                       mmu_wred_avg_qsize,
                                                       ltid,
                                                       &pt_dyn_info,
                                                       wred_q_data));
            }
        }
    }

    /* Per-SP loop */
    for (sp = 0; sp < TH4G_MMU_NUM_SPS; sp++) {

        portsp_mem_index = (lport * TH4G_MMU_NUM_SPS) + sp;

        for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, portsp_mem_index, itm);
                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                                      mmu_wred_avg_portsp_size,
                                                      ltid,
                                                      &pt_dyn_info,
                                                      wred_portsp_data));

                SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                                mmu_wred_avg_portsp_size,
                                                AVG_QSIZE_FRACTIONf,
                                                wred_portsp_data,
                                                &mem_data));
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                                mmu_wred_avg_portsp_size,
                                                AVG_QSIZEf,
                                                wred_portsp_data,
                                                &mem_data));

                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                                       mmu_wred_avg_portsp_size,
                                                       ltid,
                                                       &pt_dyn_info,
                                                       wred_portsp_data));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
th4g_mmu_stat_clear(int unit, int lport) {
    uint32_t thdi_bst_mode = 0;
    uint32_t thdo_bst_mode = 0;

    SHR_FUNC_ENTER(unit);
    th4g_mmu_get_thdi_bst_mode(unit, &thdi_bst_mode);
    th4g_mmu_get_thdo_bst_mode(unit, &thdo_bst_mode);
    /* Clear MTRO bucket memories. */
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "before th4g_mmu_clear_mtro_bucket_mems\n")));
    SHR_IF_ERR_EXIT
        (th4g_mmu_clear_mtro_bucket_mems(unit, lport));

    /* Clear QSCH credit memories. */
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "th4g_mmu_clear_qsch_credit_memories\n")));
    SHR_IF_ERR_EXIT
        (th4g_mmu_clear_qsch_credit_memories(unit, lport));

    /* Clear WRED Avg_Qsize instead of waiting for background process. */
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before th4g_mmu_wred_clr\n")));
    SHR_IF_ERR_EXIT
        (th4g_mmu_wred_clr(unit, lport));

    if (thdi_bst_mode == TH4G_MMU_BST_WMARK_MODE) {
        /* Clear THDI BST in watermark mode. */
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "before th4g_mmu_thdi_bst_clr\n")));
        SHR_IF_ERR_EXIT
            (th4g_mmu_thdi_bst_clr(unit, lport));
    }
    if (thdo_bst_mode == TH4G_MMU_BST_WMARK_MODE) {
        /* Clear THDO BST counters in watermark mode. */
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "before th4g_mmu_thdo_bst_clr\n")));
        SHR_IF_ERR_EXIT
            (th4g_mmu_thdo_bst_clr(unit, lport));

        /* Clear PktStat counters in THDO for Queues. */
        SHR_IF_ERR_EXIT
            (th4g_mmu_thdo_pktstat_clr(unit, lport));
    }
exit:
    SHR_FUNC_EXIT();
}

/*** END SDK API COMMON CODE ***/

/*!
 *  @fn int th4g_flex_mmu_port_flush(int unit,
 *                                  int lport)
 *  @param unit Device number
 *  @param lport Port number
 *  @brief API to flush MMU during port down operation.
 */
static int
th4g_flex_mmu_port_flush(int unit, int lport)
{
    int itm;

    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "setting FLUSH_ON\n")));

    SHR_IF_ERR_EXIT
        (th4g_mmu_qsch_port_flush(unit, lport, TH4G_MMU_FLUSH_ON));

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before mtro_port_metering_config\n")));
    SHR_IF_ERR_EXIT
        (th4g_mmu_mtro_port_metering_config(unit, lport, TH4G_MMU_FLUSH_ON));

    /* Delay of 500ns required before RQE flush */
    sal_usleep(1);

    /* Per-ITM configuration */
    for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
        if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "before mmu_rqe_port_flush\n")));
            SHR_IF_ERR_EXIT
                (th4g_mmu_rqe_port_flush(unit, itm));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "after FLUSH_ON set\n")));


    /* Wait for EBCTM empty status */
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before mmu_wait_ebctm_empty\n")));
    SHR_IF_ERR_EXIT
        (th4g_mmu_wait_ebctm_empty(unit, lport));


    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "setting FLUSH_OFF\n")));

    SHR_IF_ERR_EXIT
        (th4g_mmu_mtro_port_metering_config(unit, lport, TH4G_MMU_FLUSH_OFF));

    SHR_IF_ERR_EXIT
        (th4g_mmu_qsch_port_flush(unit, lport, TH4G_MMU_FLUSH_OFF));

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "after FLUSH_OFF set\n")));
exit:
    SHR_FUNC_EXIT();

}
static int
th4g_mmu_port_map(int unit, int pport, int port_add)
{
    bcmtm_port_map_info_t *port_map;
    int mmu_port, lport, mmu_chip_port;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid ;
    bcmdrd_sid_t sid_list[6] = {MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                        MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr,
                        MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm,
                        MMU_THDO_DEVICE_PORT_MAPm,
                        MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                        MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr};
    uint32_t local_phys_port;
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    int index, sid_ind, inst;
    uint32_t fval ;
    uint32_t ltmbuf = 0;


    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    /* Valid physical port */
    lport = port_map->pport_map[pport].lport;
    mmu_port = port_map->pport_map[pport].mport;
    mmu_chip_port = port_map->lport_map[lport].mchip_port;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "mmu_portmapping: lport=%3d\n"), lport));

    SHR_IF_ERR_EXIT(th4g_tdm_global_to_local_phy_num(pport, &local_phys_port));
    /*
     * MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPING - Global dev to Global mmu
     * MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPING - Global MMU to global dev
     * MMU_RQE_PORT_TO_DEVICE_PORT_MAPPING-memory, global dev to global mmu
     * MMU_THDO_DEVICE_PORT_MAP -memory, chip port num to dev port
     * MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPING - Global mmu to local phy
     * port
     * MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPING - Global mmu to global phy
     * port
     * MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPING - Global mmu to local phy
     * port
 */

    for (sid_ind = 0; sid_ind < COUNTOF(sid_list); sid_ind++) {
        sid = sid_list[sid_ind];
        ltmbuf = 0;
        switch (sid) {
            case MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr:
                fid = MMU_PORTf;
                inst = lport;
                index = 0;
                fval = port_add ? mmu_port : TH4G_INVALID_MMU_PORT;
                break;
            case MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr:
                fid = DEVICE_PORTf;
                inst = mmu_port;
                index = 0;
                fval = port_add ? lport : TH4G_INVALID_DEV_PORT;
                break;
            case MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm:
                fid = MMU_PORTf;
                index = lport;
                inst = 0;
                fval = port_add ? mmu_port : TH4G_INVALID_MMU_PORT;
                break;
            case MMU_THDO_DEVICE_PORT_MAPm:
                fid = DEVICE_PORTf;
                index = mmu_chip_port;
                inst = 0;
                fval = port_add ? lport : TH4G_INVALID_DEV_PORT;
                break;
            case MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr:
                fid = PHY_PORT_NUMf;
                inst = mmu_port;
                index = 0;
                fval = port_add ? local_phys_port : TH4G_INVALID_LOCAL_PHYS_PORT;
                break;
            case MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr:
                fid = PHY_PORT_NUMf;
                inst = mmu_port;
                index = 0;
                fval = port_add ? local_phys_port : TH4G_INVALID_LOCAL_PHYS_PORT;
                break;
           default:
                fid = -1;
                index = -1;
                inst = -1;
                fval = -1;
                break;
        }

        BCMTM_PT_DYN_INFO(pt_dyn_info, index, inst);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid,
                                  (void*)&pt_dyn_info, &ltmbuf));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,
                                   (void*)&pt_dyn_info, &ltmbuf));
    }

    if (bcmtm_lport_is_fp(unit, lport)) {
        /* Applicable only for Front Panel Ports */
        sid = MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr;
        fid = PHY_PORT_NUMf;
        inst = mmu_port;
        fval = port_add ? pport : TH4G_INVALID_PHYS_PORT;
        ltmbuf = 0;

        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                                             (void*)&pt_dyn_info, &ltmbuf));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,
                                   (void*)&pt_dyn_info, &ltmbuf));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
th4g_mmu_port_speed_encode(int unit, int pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    int mmu_port;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid ;
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    struct {
        bcmdrd_sid_t ref_reg;
        bcmdrd_fid_t encode_field;
    } rf_list[5] = {
        {MMU_CRB_SRC_PORT_CFGr, PORT_SPEEDf},
        {MMU_EBCTM_EPORT_CT_CFGr, DST_PORT_SPEEDf},
        {MMU_EBPCC_EPORT_CFGr, DST_PORT_SPEEDf},
        {MMU_EBQS_PORT_CFGr, PORT_SPEEDf},
        {MMU_EBPTS_EBSHP_PORT_CFGr, PORT_SPEEDf}
    };
    int sid_idx;
    uint32_t fval;
    uint32_t ltmbuf = 0;
    int speed;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    mmu_port = port_map->pport_map[pport].mport;
    speed = port_map->lport_map[lport].cur_speed;
    LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "mmu_port_speed_encode: lport=%3d\n"), lport));


    /* Retrieve encoding value to be used based on port speed */
    SHR_IF_ERR_EXIT
        (th4g_mmu_get_speed_decode(unit, speed, &fval));

    for (sid_idx = 0; sid_idx < COUNTOF(rf_list); sid_idx++) {
        sid = rf_list[sid_idx].ref_reg;
        fid = rf_list[sid_idx].encode_field;
        ltmbuf = 0;
        if (((!bcmtm_lport_is_fp(unit, lport))) &&
            ((sid == MMU_EBCTM_EPORT_CT_CFGr) ||
             (sid == MMU_EBPTS_EBSHP_PORT_CFGr))) {
            /* Register invalid for CPU,LB and MGMT ports */
            continue;
        }

        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                                             (void*)&pt_dyn_info, &ltmbuf));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,
                                   (void*)&pt_dyn_info, &ltmbuf));

    }


exit:
    SHR_FUNC_EXIT();
}

static int
th4g_mmu_port_rx_enables(int unit, int pport)
{
    bcmtm_port_map_info_t *port_map;
    int mmu_port, lport;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pipe;
    uint32_t ltmbuf = 0;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    mmu_port = port_map->pport_map[pport].mport;
    lport = port_map->pport_map[pport].lport;
    pipe = port_map->lport_map[lport].pipe;
    if (pipe < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sid = MMU_CRB_SRC_PORT_CFGr;
    fid = RX_PORT_ENf;
    fval = 1;
    ltmbuf = 0;

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}



/*******************************************************************************
 * BCMTM PUBLIC FUNCTIONS
 */

int
bcm56996_a0_tm_port_mmu_down(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    bool sim = bcmdrd_feature_is_sim(unit);

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        if (!sim) {
            SHR_IF_ERR_EXIT
                (th4g_flex_mmu_port_flush(unit, lport));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcm56996_a0_tm_port_mmu_delete(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        SHR_IF_ERR_EXIT
            (th4g_mmu_tdm_port_delete(unit, lport));
        /* Disable metering before clearing bucket mems */
        SHR_IF_ERR_EXIT
            (th4g_mmu_mtro_port_metering_config(unit, lport, TH4G_MMU_FLUSH_ON));
        SHR_IF_ERR_EXIT
            (th4g_mmu_stat_clear(unit, lport));
        /* Reenable metering */
        SHR_IF_ERR_EXIT
            (th4g_mmu_mtro_port_metering_config(unit, lport, TH4G_MMU_FLUSH_OFF));
        SHR_IF_ERR_EXIT(th4g_mmu_port_map(unit, pport, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_port_mmu_add (int unit, int pport) {
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        SHR_IF_ERR_EXIT
            (th4g_mmu_tdm_port_add(unit, lport));
        SHR_IF_ERR_EXIT(th4g_mmu_port_map(unit, pport, 1));
        SHR_IF_ERR_EXIT(th4g_mmu_port_rx_enables(unit, pport));
        SHR_IF_ERR_EXIT(th4g_mmu_port_speed_encode(unit, pport));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_rqe_port_flush(int unit)
{
    int itm;

    SHR_FUNC_ENTER(unit);
    for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
        if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
            SHR_IF_ERR_EXIT
                (th4g_mmu_rqe_port_flush(unit, itm));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

