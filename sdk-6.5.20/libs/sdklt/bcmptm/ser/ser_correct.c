/*! \file ser_correct.c
 *
 * Interface functions for SER correction
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_port_intr.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <sal/sal_types.h>
#include <shr/shr_thread.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>

#include "ser_correct.h"
#include "ser_intr_dispatch.h"
#include "ser_tcam.h"
#include "ser_tcam_scan.h"
#include "ser_sram_scan.h"
#include "ser_bd.h"
#include "ser_config.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER
#define ACC_FOR_SER TRUE

/*! Recent 32 time reported SID */
#define  SERC_CACHE_MAX_NUM                 32

/*! Create SID of buffer or bus */
#define BB_MAGIC_SID_CREATE(_membase, _stage_id) \
            ((1 << 31) | (_membase & 0xFF) | ((_stage_id & 0xFF) << 8))
/*! GET membase from SID of buffer or bus */
#define BB_MAGIC_SID_MEMBASE_GET(_sid_magic) \
            ((_sid_magic & 0xFF))
/*! GET ID from SID of buffer or bus */
#define BB_MAGIC_SID_STAGE_ID_GET(_sid_magic) \
            ((_sid_magic & 0xFF00) >> 8)

/*!
* Function used to get SER information from SER FIFOs
*/
typedef int(* bcmptm_ser_fifo_pop_f)(int unit,
                                       bcmptm_pre_serc_info_t *serc_info,
                                       bcmptm_ser_raw_info_t *crt_info);
/*!
* Function used to parse SER information from SER FIFOs
*/
typedef int(* bcmptm_ser_fifo_parse_f)(int unit,
                                 bcmptm_ser_raw_info_t *crt_info,
                                 bcmptm_ser_correct_info_t *spci);
/*!
* This struct used to map ser_info_type with Popping and parsing functions
*/
typedef struct bcmptm_ser_pop_parse_f_map_s {
    /* SER blk type */
    bcmptm_ser_info_type_t   ser_info_type;
    /* Popping function */
    bcmptm_ser_fifo_pop_f   ser_fifo_pop;
    /* Parsing function */
    bcmptm_ser_fifo_parse_f  ser_fifo_parse;
} bcmptm_ser_pop_parse_f_map_t;

/*!
* This struct used to save field value of NG_SER_FIFOm or EGR_SER_FIFOm
* or L2_MGMT_SER_FIFOm
*/
typedef struct bcmptm_ser_ing_egr_fifo_fld_val_s {
    uint32_t valid;      /* VALIDf */
    uint32_t ecc_parity; /* ECC_PARITYf */
    uint32_t mem_type;   /* MEM_TYPEf */
    uint32_t address;    /* ADDRESSf */
    uint32_t pipe_stage; /* PIPE_STAGEf */
    uint32_t intruction; /* INSTRUCTION_TYPEf */
    uint32_t drop;       /* DROPf */
    uint32_t multiple;   /* MULTIPLEf */
    uint32_t non_sbus;   /* NON_SBUSf */
    uint32_t membase;    /* MEMBASEf */
    uint32_t memindex;   /* MEMINDEXf */
    uint32_t regbase;    /* REGBASEf */
    uint32_t regindex;   /* REGINDEXf */
    uint32_t hw_correction; /* HW_CORRECTIONf */
} bcmptm_ser_ing_egr_fifo_fld_val_t;

typedef struct bcmptm_serc_info_s {
    /*! Device number */
    int unit;
    /*! SERC thread control information */
    shr_thread_t serc_thread_ctrl;
} bcmptm_serc_info_t;

/*! Used to save thread ID of SERC */
static bcmptm_serc_info_t serc_info[BCMDRD_CONFIG_MAX_UNITS];

/*! Record latest SERC_CACHE_MAX_NUM SIDs reported by H/W */
static bcmdrd_sid_t ser_corrected_sids[BCMDRD_CONFIG_MAX_UNITS][SERC_CACHE_MAX_NUM];
/*! New SID owning SER will save to index of ring buffer ser_corrected_sids[] */
static int ser_corrected_index[BCMDRD_CONFIG_MAX_UNITS];
/*! Number of SID in buffer ser_corrected_sids[] */
static int ser_corrected_count[BCMDRD_CONFIG_MAX_UNITS];
/*! The last time to do severity fault checking */
static sal_usecs_t ser_sf_checking_latest_time[BCMDRD_CONFIG_MAX_UNITS];
/*! Handler to map SID reported by H/W to SID used to correct SER error. */
static bcmptm_ser_alpm_info_get_f ser_alpm_info_get_f[BCMDRD_CONFIG_MAX_UNITS];
/*! Handler to map SID reported by H/W to SID used to correct SER error in mtop . */
static bcmptm_ser_alpm_info_get_f ser_alpm_mtop_info_get_f[BCMDRD_CONFIG_MAX_UNITS];

/*! Clear value of field which can be updated by H/W */
#define BCMPTM_ENTRY_PARITY_CLEAR(unit, ecc_field_name)  \
    do { \
        symbols = bcmdrd_dev_symbols_get(unit, 0); \
        symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid, ecc_field_name, &finfo); \
        if (symbol) { \
            (void)bcmdrd_pt_field_set(unit, sid, entry_data, finfo.fid, &fval); \
        } \
    } while(0);

#define INVALID_MEM_ADDR    0xFFFFFFFF

#define ALPM_REMAP_COUNT    2

/*! assign SID to ring cache */
#define ASSIGN_NEW_SID_TO_RING_CACHE(_unit, _sid) \
    do { \
        ser_corrected_sids[_unit][ser_corrected_index[_unit]] = _sid; \
        if (ser_corrected_index[_unit] < (SERC_CACHE_MAX_NUM - 1)) { \
            ser_corrected_index[_unit]++; \
        } else { \
            ser_corrected_index[_unit] = 0; \
        } \
        if (ser_corrected_count[_unit] < SERC_CACHE_MAX_NUM) { \
            ser_corrected_count[_unit]++; \
        } \
    } while(0)

/******************************************************************************
 * Private Functions
 */
/*
 * SER check type of a PT is either Parity check or ECC check. If one PT has Parity check, this
 * routine will return 0, otherwise, return 1.
 */
static bool
bcmptm_ser_mem_ecc_protection(int unit, bcmdrd_sid_t sid)
{
    int ecc_checking = 0;

    (void)bcmptm_ser_mem_ecc_info_get(unit, sid, 0, NULL, NULL, &ecc_checking);
    if (ecc_checking == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/*
 * Pop SER correction data from MMU FIFOs,
 * such as: MMU_GCFG_MEM_FAIL_ADDR_64m etc.
 */
static int
bcmptm_ser_mmu_cfg_fail_mem_pop(int unit, bcmdrd_sid_t fifo_mem, int inst,
                                bcmdrd_fid_t multi_bits_fid,
                                bcmdrd_fid_t err_type_fid,
                                bcmdrd_fid_t eaddr_fid,
                                int blk_type,
                                bcmptm_ser_mmu_crt_info_t *ser_mmu_info)
{
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
    bcmbd_pt_dyn_info_t dyn_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry_data, 0, BCMDRD_MAX_PT_WSIZE * sizeof(uint32_t));
    dyn_info.index = 0;
    dyn_info.tbl_inst = inst;
    /* get block instance */
    ser_mmu_info->blk_inst = inst;
    /* get block type */
    ser_mmu_info->blk_type = blk_type;

    /* pop fifo mem */
    rv = bcmbd_pt_pop(unit, fifo_mem, &dyn_info, NULL, entry_data, BCMDRD_MAX_PT_WSIZE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* read data from ERR_MULTf */
    rv = bcmdrd_pt_field_get(unit, fifo_mem, entry_data,
                             multi_bits_fid, &(ser_mmu_info->multi_bits));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* read data from ERR_TYPEf */
    rv = bcmdrd_pt_field_get(unit, fifo_mem, entry_data,
                             err_type_fid, &(ser_mmu_info->err_type));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* read data from EADDRf */
    rv = bcmdrd_pt_field_get(unit, fifo_mem, entry_data,
                             eaddr_fid, &(ser_mmu_info->eaddr));
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * The first phase of SER correction of MMU PTs.
 * According to SER interrupt number, SER logic can know which sub-block has SER error,
 * and then pop SER correction data from the sub-block.
 */
static int
bcmptm_ser_mmu_fifo_pop(int unit,
                        bcmptm_pre_serc_info_t *pre_serc_info,
                        bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE;
    uint32_t fifo_stat_val[1] = {0}, fld_val = 0;
    bcmptm_serc_fifo_t *intr_map = (bcmptm_serc_fifo_t *)pre_serc_info->stat_info;
    bcmptm_ser_mmu_crt_info_t *ser_mmu_info = NULL;
    uint32_t intr_cnt[1] = {0};
    int blk_inst = pre_serc_info->blk_inst;

    SHR_FUNC_ENTER(unit);

    ser_mmu_info = (bcmptm_ser_mmu_crt_info_t *)(&crt_info->serc_info);
    crt_info->valid = 0;

    rv = bcmptm_ser_pt_read(unit, intr_map->stat_reg, 0,
                            blk_inst, -1, fifo_stat_val, 1,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, intr_map->stat_reg, fifo_stat_val,
                             intr_map->intr_stat_fld, &fld_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* FIFO memories are empty */
    if (fld_val == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Value of EMPTYf of fifo status register[%s] is TRUE\n"),
                     bcmdrd_pt_sid_to_name(unit, intr_map->stat_reg)));
        /* exit */
        SHR_ERR_EXIT(rv);
    }
    ser_mmu_info->blk_inst = blk_inst;
    rv = bcmptm_ser_mmu_cfg_fail_mem_pop(unit, intr_map->fifo_mem, blk_inst,
                                         intr_map->fifo_mem_fids[0],
                                         intr_map->fifo_mem_fids[1],
                                         intr_map->fifo_mem_fids[2],
                                         intr_map->blk_type,
                                         ser_mmu_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    crt_info->valid = 1;

    rv = bcmptm_ser_pt_read(unit, intr_map->ser_fail_ctr_reg, 0,
                            blk_inst, -1, intr_cnt, 1,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Found MMU interrupt from [%s%d], count [%d]\n"),
              intr_map->blk_name, blk_inst, intr_cnt[0]));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "%s %s error at address 0x%08x\n"),
               ser_mmu_info->multi_bits ? "multiple" : "",
               (ser_mmu_info->err_type == 1) ? "1bit": "parity/2bit",
               ser_mmu_info->eaddr));
exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to pop information from MMU FIFO memory\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * The second phase of SER correction of MMU PTs.
 * Parse data popped from FIFOs, in order to get SID, index,
 * instance, error type, etc.
 */
static int
bcmptm_ser_mmu_fifo_entry_parse(int unit,
                                bcmptm_ser_raw_info_t *crt_info,
                                bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_mmu_crt_info_t *mmu_crt_info = NULL;
    int rv = SHR_E_NONE;
    uint32_t adext = 0;
    bcmptm_cmic_adext_info_t adext_info;
    bcmdrd_sid_t sid = INVALIDm;
    bcmbd_pt_dyn_info_t dyn_info;
    int blkinst = 0;
    int blk_type = -1;
    int blk_type_overlay[BCMPTM_SER_OVERLAY_BLK_TYPE_NUM] = { -1, -1 };
    int i = 0, inst_num = 0, j = 0, copy_num = 0;

    SHR_FUNC_ENTER(unit);

    mmu_crt_info = (bcmptm_ser_mmu_crt_info_t *)(&crt_info->serc_info);
    blkinst = mmu_crt_info->blk_inst;
    blk_type = mmu_crt_info->blk_type;

    rv = bcmptm_ser_blk_type_map(unit, blk_type, blk_type_overlay, NULL);
    SHR_IF_ERR_EXIT(rv);
    for (j = 0; ; j++) {
        /*
         * Access type of mmu memory may be duplicate, unique or single.
         * Try to use unique acc_type to find SID,
         * if not found, then try to use default acc_type to find SID.
         * If still not found, then change block type from GLB to EB or ITM.
         */
        adext_info.acc_type = blkinst;
        for (i = 0; i < 2; i++) {
            rv = bcmptm_ser_blknum_get(unit, INVALIDm, 0, blk_type,
                                       &(adext_info.blk_num));
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            rv = bcmptm_ser_cmic_adext_get(unit, adext_info, &adext);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            /* SER checking only for MMU memories */
            rv = bcmbd_pt_addr_decode(unit, adext, mmu_crt_info->eaddr,
                                      BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
            if(SHR_SUCCESS(rv)) {
                break;
            }
            if (i == 0) {
                adext_info.acc_type = -1;
            }
        }
        if (SHR_SUCCESS(rv) || j == BCMPTM_SER_OVERLAY_BLK_TYPE_NUM) {
            break;
        }
        blk_type = blk_type_overlay[j];
        /* H/W reports error block type for PT MMU_QSCH_L0_CREDIT_MEMm etc. */
        /* H/W reports error block type for PT MMU_RL_FBANK10m etc. */
        if (blk_type == -1) {
            break;
        }
    }
    if (SHR_SUCCESS(rv)) {
        mmu_crt_info->blk_type = blk_type;
        spci->addr[0] = mmu_crt_info->eaddr;
        spci->sid[0] = sid;
        spci->index[0] = dyn_info.index;

        rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &inst_num, &copy_num);
        SHR_IF_ERR_EXIT(rv);
        if (copy_num != 0) {
            spci->tbl_copy = blkinst;
            spci->tbl_inst[0] = dyn_info.tbl_inst;
        } else if (dyn_info.tbl_inst <= 0 && blkinst > 0 && blkinst < inst_num) {
            spci->tbl_inst[0] = blkinst;
        } else {
            spci->tbl_inst[0] = dyn_info.tbl_inst;
        }
    } else {
        spci->sid[0] = INVALIDm;
        spci->addr[0] = mmu_crt_info->eaddr;
        adext_info.acc_type = blkinst;
        rv = bcmptm_ser_cmic_adext_get(unit, adext_info, &adext);
        SHR_IF_ERR_EXIT(rv);
    }
    spci->sid[1] = INVALIDm;
    spci->addr[1] = INVALID_MEM_ADDR;
    /* Information in spci will be modified in this routine */
    rv = bcmptm_ser_mmu_mem_remap(unit, spci, adext, blkinst);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
    } else if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to decode index and sid of MMU PT again.\n")));
        SHR_IF_ERR_EXIT(rv);
    }
    if (mmu_crt_info->err_type > 1) {
        spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
    }
    if (mmu_crt_info->multi_bits) {
        spci->flags |= BCMPTM_SER_FLAG_MULTIBIT;
    }
    if (TRUE == bcmptm_ser_mem_ecc_protection(unit, sid)) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
    } else {
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_MMU;
    spci->blk_type = mmu_crt_info->blk_type;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to parse information from MMU FIFO memory\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * Get SER information from status registers
 * which belong to BCMPTM_SER_INFO_TYPE_MAC.
 * After that, the status registers are cleaned up.
 */
static int
bcmptm_ser_mac_status_reg_clear(int unit,
                                bcmdrd_sid_t clr_reg, bcmdrd_fid_t clr_fid,
                                int clr_method,
                                int reg_index, int tbl_inst, int err_type,
                                bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem)
{
    int rv = SHR_E_NONE;
    uint32_t rval[BCMDRD_MAX_PT_WSIZE], fval = 0;

    SHR_FUNC_ENTER(unit);

    /* ECC SER type checking */
    if (err_type) {
        /* 1bit ECC error */
        ser_misc_mem->err_type = 0;
        if (err_type == 2) {
            /* Multi error */
            ser_misc_mem->multi_bits = 1;
        }
    } else {
        /* 2bit ECC error */
        ser_misc_mem->err_type = 1;
    }
    if (clr_reg == INVALIDr) {
        /* For example: CDMAC */
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "The interrupt clear register does not exist\n")));
        rv = SHR_E_NONE;
        /* exit */
        SHR_ERR_EXIT(rv);
    }
    if (clr_fid == INVALIDf) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fields number of clear register should"
                              " be equal to the number of status register\n")));
        rv = SHR_E_CONFIG;
        SHR_ERR_EXIT(rv);
    }
    /* clear interrupt status register */
    if (clr_method == BCMPTM_SER_CLR_INTR_WRITE_ZERO ||
        clr_method == BCMPTM_SER_CLR_INTR_RAISE_EDGE) {
        rv = bcmptm_ser_pt_read(unit, clr_reg, reg_index, tbl_inst, -1, rval,
                                BCMDRD_MAX_PT_WSIZE,
                                BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        fval = 0;
        rv = bcmdrd_pt_field_set(unit, clr_reg, rval, clr_fid, &fval);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_write(unit, clr_reg, reg_index, tbl_inst,
                                 -1, rval, 0);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if (clr_method == BCMPTM_SER_CLR_INTR_RAISE_EDGE ||
        clr_method == BCMPTM_SER_CLR_INTR_WRITE_ONE) {
        fval = 1;
        rv = bcmdrd_pt_field_set(unit, clr_reg, rval, clr_fid, &fval);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_write(unit, clr_reg, reg_index, tbl_inst,
                                 -1, rval, 0);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get SER information from ECC status registers
 * which belong to BCMPTM_SER_INFO_TYPE_ECC.
 */
static int
bcmptm_ser_ecc_parity_status_reg_get(int unit,
                                     bcmdrd_sid_t reg,
                                     uint32_t *status_entry_data,
                                     bcmdrd_fid_t *fld_list,
                                     int *fval_multiple_err,
                                     int *fval_entry_idx,
                                     int *fval_double_bit_err)
{
    int rv = SHR_E_NONE, field_idx = 0;
    int fval[3] = {-1, -1, -1};
    uint32_t unsigned_fval = 0;

    SHR_FUNC_ENTER(unit);

    for (field_idx = 0; fld_list[field_idx] != INVALIDf; field_idx++) {
        rv = bcmdrd_pt_field_get(unit, reg, status_entry_data,
                                 fld_list[field_idx], &unsigned_fval);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ECC or parity status register[%s] may not have field[%d]\n"),
                       bcmdrd_pt_sid_to_name(unit, reg), fld_list[field_idx]));
            SHR_ERR_EXIT(rv);
        }
        fval[field_idx] = (int)unsigned_fval;
    }
    /* -1 means that the fields are not used */
    *fval_multiple_err = fval[0];
    *fval_entry_idx = fval[1];
    *fval_double_bit_err = fval[2]; /* parity status registers do not have this field */

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get SER information from ECC status registers
 * which belong to SER type SEC.
 */
static int
bcmptm_ser_sec_parity_status_reg_get(int unit,
                                     bcmdrd_sid_t reg,
                                     uint32_t *status_entry_data,
                                     bcmdrd_fid_t *fld_list,
                                     int *fval_multiple_err,
                                     int *fval_entry_idx,
                                     int *fval_double_bit_err)
{
    int rv = SHR_E_NONE, field_idx = 0;
    int fval[4] = {-1, -1, -1, -1};
    uint32_t unsigned_fval[2] = {0};

    SHR_FUNC_ENTER(unit);

    for (field_idx = 0; fld_list[field_idx] != INVALIDf; field_idx++) {
        rv = bcmdrd_pt_field_get(unit, reg, status_entry_data,
                                 fld_list[field_idx], unsigned_fval);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ECC or parity status register[%s] may not have field[%d]\n"),
                       bcmdrd_pt_sid_to_name(unit, reg), fld_list[field_idx]));
            SHR_ERR_EXIT(rv);
        }
        fval[field_idx] = (int)unsigned_fval[0];
    }
    rv = bcmptm_ser_status_field_parse(unit, reg, 4, fval);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    /* -1 means that the fields are not used */
    *fval_double_bit_err = fval[1];
    *fval_multiple_err = 0;
    *fval_entry_idx = fval[3];

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get SER information from ECC status registers
 * which belong to BCMPTM_SER_INFO_TYPE_ECC.
 * After that, clear the status registers.
 */
static int
bcmptm_ser_ecc_status_reg_clear(int unit,
                                bcmdrd_sid_t status_reg,
                                int index, int tbl_inst, uint32_t *entry_data,
                                bcmptm_serc_non_fifo_t *serc_non_fifo,
                                bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem)
{
    int rv = SHR_E_NONE;
    uint32_t rval = 0;
    bcmptm_ser_ecc_status_fld_t *ecc_status_fld_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecc_status_fld_info = serc_non_fifo->ecc_stat_fld_info;

    if (ecc_status_fld_info->ecc_err_field == INVALIDf ||
        ecc_status_fld_info->intr_status_field_list == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[intr_status_field] should be assigned by ECC_ERRf/PAR_ERRf\n"
                              " [intr_status_field_list] should be assigned by other FIDs"
                              " of status register[%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, status_reg)));
        rv = SHR_E_CONFIG;
        SHR_ERR_EXIT(rv);
    }
    rv = bcmptm_ser_ecc_parity_status_reg_get(unit, status_reg, entry_data,
                                              ecc_status_fld_info->intr_status_field_list,
                                              &ser_misc_mem->multi_bits,
                                              &ser_misc_mem->entry_idx,
                                              &ser_misc_mem->err_type);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_misc_mem->status_reg = status_reg;
    rval = 0;
    /* clear interrupt status register */
    rv = bcmptm_ser_pt_write(unit, status_reg, index, tbl_inst,
                             -1, &rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Clear any register used to corrupt data.
 */
static int
bcmptm_ser_sec_corrupt_clear(int unit, int tbl_inst,
                             bcmdrd_sid_t reg,
                             bcmdrd_fid_t fid,
                             uint32_t val)
{
    int rv = SHR_E_NONE;
    uint32_t rval[2] = {0};

    SHR_FUNC_ENTER(unit);
    rv = bcmptm_ser_pt_read(unit, reg, 0, tbl_inst,
                            -1, rval, 2, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Unable to read register %s\n"),
                        bcmdrd_pt_sid_to_name(unit, reg)));
        SHR_ERR_EXIT(rv);
    }
    rv = bcmdrd_pt_field_set(unit, reg, rval,
                             fid, &val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmptm_ser_pt_write(unit, reg, 0, tbl_inst,
                             -1, rval, 0);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Unable to write register %s\n"),
                        bcmdrd_pt_sid_to_name(unit, reg)));
        SHR_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Get SER information from SEC status registers
 * which belong to BCMPTM_SER_INFO_TYPE_SEC.
 * After that, clear the status registers.
 */
static int
bcmptm_ser_sec_status_reg_clear(int unit,
                                bcmdrd_sid_t status_reg,
                                int index, int tbl_inst, uint32_t *entry_data,
                                bcmptm_serc_non_fifo_t *serc_non_fifo,
                                bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_ecc_status_fld_t *ecc_status_fld_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecc_status_fld_info = serc_non_fifo->ecc_stat_fld_info;

    if (ecc_status_fld_info->intr_status_field_list == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[intr_status_field] should be assigned by ECC_ERRf/PAR_ERRf\n"
                              " [intr_status_field_list] should be assigned by other FIDs"
                              " of status register[%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, status_reg)));
        rv = SHR_E_CONFIG;
        SHR_ERR_EXIT(rv);
    }
    if (serc_non_fifo->intr_clr_reg != INVALIDr) {
        rv = bcmptm_ser_sec_corrupt_clear(unit, tbl_inst, serc_non_fifo->intr_clr_reg,
                                    serc_non_fifo->sec_stat_fld_info->intr_status_field,
                                    serc_non_fifo->sec_stat_fld_info->wr_val);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmptm_ser_sec_parity_status_reg_get(unit, status_reg, entry_data,
                                              ecc_status_fld_info->intr_status_field_list,
                                              &ser_misc_mem->multi_bits,
                                              &ser_misc_mem->entry_idx,
                                              &ser_misc_mem->err_type);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_misc_mem->status_reg = status_reg;
    /* Read on clear interrupt status register */
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/* Clear top level status registers. */
static int
bcmptm_ser_group_intr_status_reg_clear(int unit,
                                       bcmdrd_sid_t  group_status_reg,
                                       bcmdrd_fid_t  group_status_fid,
                                       int tbl_inst)
{
    uint32_t rval[1] = {0}, fval = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* read status register */
    rv = bcmptm_ser_pt_read(unit, group_status_reg, 0, tbl_inst,
                            -1, rval, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    fval = 0;
    rv = bcmdrd_pt_field_get(unit, group_status_reg, rval,
                             group_status_fid, &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (fval == 0) {
        /* exit */
        SHR_ERR_EXIT(rv);
    }
    rval[0] = 0;
    /* Write 1 to clear status register like CLPORT_INTR_STATUSr.MIB_TX_MEM_ERRf */
    rv = bcmdrd_pt_field_set(unit, group_status_reg, rval,
                             group_status_fid, &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, group_status_reg, 0, tbl_inst,
                             -1, rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to clear interrupt status register[%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, group_status_reg)));
    }
    SHR_FUNC_EXIT();
}

/*
 * To get SER error type, according to
 * value of top level interrupt status register.
 */
static int
bcmptm_ser_group_intr_status_reg_check(int unit,
                                       bcmptm_serc_non_fifo_t *serc_info_list,
                                       int info_depth, int tbl_inst,
                                       bcmptm_serc_non_fifo_t **pre_serc_info)
{
    uint32_t rval[BCMDRD_MAX_PT_WSIZE] = {0}, fval =0;
    int rv = SHR_E_NONE, i;
    uint32_t rval_low[BCMDRD_MAX_PT_WSIZE] = {0};
    bcmdrd_sid_t  status_reg_low = INVALIDr;
    const size_t wsize = BCMDRD_MAX_PT_WSIZE;
    bcmdrd_sid_t  group_status_reg = serc_info_list[0].stat_reg;
    bcmdrd_fid_t  status_fid = serc_info_list[0].intr_stat_fld;

    SHR_FUNC_ENTER(unit);

    *pre_serc_info = NULL;
    /* read status register */
    rv = bcmptm_ser_pt_read(unit, group_status_reg, 0, tbl_inst,
                            -1, rval, wsize, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < info_depth; i++) {
        status_fid = serc_info_list[i].intr_stat_fld;

        rv = bcmdrd_pt_field_get(unit, group_status_reg, rval,
                                 status_fid, &fval);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to check group status register[%s]"
                                  " instance[%d] value [0x%08x]\n"),
                       bcmdrd_pt_sid_to_name(unit, group_status_reg),
                       tbl_inst, rval[0]));
            break;
        }
        if (fval) {
            if ((serc_info_list[i].ecc_mac_mode == 2) &&
                (serc_info_list[i].sec_stat_fld_info != NULL) &&
                (serc_info_list[i].sec_stat_fld_info->rd_skip == 0)) {
                /* Read additional status layer. */
                /* Clear on read for SEC status registers. Read only once. */
                if (status_reg_low != serc_info_list[i].sec_stat_fld_info->intr_status_reg) {
                    status_reg_low = serc_info_list[i].sec_stat_fld_info->intr_status_reg;
                    rv = bcmptm_ser_pt_read(unit, status_reg_low, 0, tbl_inst,
                                            -1, rval_low, wsize,
                                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
                    if (SHR_FAILURE(rv)) {
                        continue;
                    }
                }
                status_fid = serc_info_list[i].sec_stat_fld_info->intr_status_field;
                rv = bcmdrd_pt_field_get(unit, status_reg_low, rval_low,
                                        status_fid, &fval);
                if (!fval || SHR_FAILURE(rv)) {
                    continue;
                }
            }
            *pre_serc_info = &serc_info_list[i];
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get SID of interrupt status register of low level.
 * Also get FIDs of the status register.
 */
static int
bcmptm_ser_low_level_intr_status_regs_get(int unit,
                                          bcmptm_serc_non_fifo_t *pre_serc_info,
                                          bcmptm_ser_buf_bus_ctrl_reg_t *status_reg_array,
                                          bcmptm_ser_buf_bus_ctrl_reg_t **status_reg_ptr,
                                          bcmdrd_fid_t *status_fid_ptr,
                                          bcmptm_ser_mac_status_fld_t **mac_status_fld_info)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_ecc_status_fld_t *ecc_status_fld_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* get SIDs */
    if (pre_serc_info->intr_stat_reg != INVALIDr) {
        status_reg_array[0].reg = pre_serc_info->intr_stat_reg;
        status_reg_array[0].mem_str = NULL;
        status_reg_array[0].membase = -1;
        status_reg_array[0].stage_id = -1;
        status_reg_array[1].reg = INVALIDr;
        *status_reg_ptr = status_reg_array;
    } else if (pre_serc_info->intr_stat_reg_list != NULL) {
        *status_reg_ptr = pre_serc_info->intr_stat_reg_list;
    } else {
        rv = SHR_E_CONFIG;
        SHR_ERR_EXIT(rv);
    }
    /* get FIDs, the registers have the same FIDs*/
    if (pre_serc_info->ecc_mac_mode == 1) {
        ecc_status_fld_info = pre_serc_info->ecc_stat_fld_info;
        *status_fid_ptr = ecc_status_fld_info->ecc_err_field;
    } else if (pre_serc_info->ecc_mac_mode == 0) {
        *mac_status_fld_info = pre_serc_info->mac_stat_fld_info;
    } else if (pre_serc_info->ecc_mac_mode == 2) {
        /* SEC status registers are clear on read. */
        *status_fid_ptr = INVALIDf;
    } else {
        rv = SHR_E_CONFIG;
        SHR_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get intr status register and its field\n")));
    }
    SHR_FUNC_EXIT();
}

/* Get SID of interrupt clear register of low level. */
static int
bcmptm_ser_low_level_intr_clr_regs_get(int unit,
                                       bcmptm_serc_non_fifo_t *pre_serc_info,
                                       bcmdrd_sid_t *clr_reg)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    /* get SIDs */
    if (pre_serc_info->intr_clr_reg != INVALIDr) {
        *clr_reg = pre_serc_info->intr_clr_reg;
    } else {
        /* can be INVALIDr */
        *clr_reg = INVALIDr;
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get intr clr register and its field\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * The first phase of SER process of Port/IDB/POST blocks.
 * Read raw data from status registers, and clear status registers.
 */
static int
bcmptm_ser_non_mmu_process(int unit,
                           bcmptm_pre_serc_info_t *pre_serc_info,
                           bcmptm_ser_raw_info_t *crt_info)
{
    bcmptm_ser_buf_bus_ctrl_reg_t status_reg_array[2], *status_reg_ptr = NULL;
    bcmdrd_sid_t clr_reg;
    int rv = SHR_E_NONE;
    bcmdrd_fid_t ecc_status_fid = INVALIDf, intr_status_fid = INVALIDf;
    bcmdrd_fid_t intr_clr_fid = INVALIDf;
    bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem = NULL;
    int reg_idx = 0, field_idx = 0, err_type = 0;
    bcmdrd_sid_t status_reg, group_status_reg;
    bcmbd_pt_dyn_info_t dyn_info = {0, -1}, group_dyn_info = {0, -1};
    uint32_t rval[2] = {0}, fval =0;
    bcmptm_serc_non_fifo_t *serc_info_list = NULL;
    bcmptm_serc_non_fifo_t *serc_info_ptr = NULL;
    const char *mem_desc = NULL;
    bcmptm_ser_mac_status_fld_t *mac_status_fld_info = NULL;
    int blk_inst, max_index, port;
    bcmptm_ser_intr_clr_type_t clr_method = BCMPTM_SER_CLR_INTR_NO_OPER;

    SHR_FUNC_ENTER(unit);

    serc_info_list = (bcmptm_serc_non_fifo_t *)pre_serc_info->stat_info;
    blk_inst = pre_serc_info->blk_inst;
    port = pre_serc_info->port;

    ser_misc_mem = (bcmptm_ser_misc_blk_crt_info_t *)(&crt_info->serc_info);
    crt_info->valid = 1;

    group_status_reg = serc_info_list[0].stat_reg;
    group_dyn_info.index = 0;
    group_dyn_info.tbl_inst = blk_inst;

    if (group_status_reg != INVALIDr) {
        rv = bcmptm_ser_group_intr_status_reg_check(unit,
                                                    serc_info_list,
                                                    pre_serc_info->stat_depth,
                                                    group_dyn_info.tbl_inst,
                                                    &serc_info_ptr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        serc_info_ptr = &serc_info_list[0];
    }

    if (serc_info_ptr == NULL) {
        /* This condition is satisfied when the interrupt can't be handled by SER */
        /* Or all events had been processed. */
        crt_info->valid = 0;
        /* exit */
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    rv = bcmptm_ser_low_level_intr_status_regs_get(unit, serc_info_ptr,
                                                   status_reg_array,
                                                   &status_reg_ptr,
                                                   &ecc_status_fid,
                                                   &mac_status_fld_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_low_level_intr_clr_regs_get(unit, serc_info_ptr, &clr_reg);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (reg_idx = 0; status_reg_ptr[reg_idx].reg != INVALIDr; reg_idx++) {
        status_reg = status_reg_ptr[reg_idx].reg;
        dyn_info.tbl_inst = blk_inst;
        if (port >= 0 && bcmdrd_pt_is_port_reg(unit, status_reg)) {
            dyn_info.tbl_inst = port;
        }
        mem_desc = status_reg_ptr[reg_idx].mem_str;
        if (status_reg_ptr[reg_idx].membase >= 0 &&
            status_reg_ptr[reg_idx].stage_id >= 0) {
            ser_misc_mem->magic_sid =
                BB_MAGIC_SID_CREATE(status_reg_ptr[reg_idx].membase,
                                    status_reg_ptr[reg_idx].stage_id);
        } else {
            ser_misc_mem->magic_sid = INVALIDr;
        }

        dyn_info.index = 0;
        max_index = bcmdrd_pt_index_max(unit, status_reg);
        for ( ; dyn_info.index <= max_index; dyn_info.index++) {
            if (!bcmdrd_pt_index_valid(unit, status_reg, dyn_info.tbl_inst, dyn_info.index)) {
                continue;
            }
            /* read status register */
            rval[0] = 0;
            rv = bcmptm_ser_pt_read(unit, status_reg, dyn_info.index, dyn_info.tbl_inst,
                                    -1, rval, 2, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            if (rval[0] == 0) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Value of instance [%d] of status register[%s] is zero\n"),
                           dyn_info.tbl_inst, bcmdrd_pt_sid_to_name(unit, status_reg)));
                continue;
            }
            fval = 0;
            for (field_idx = 0; ; field_idx++) {
                if ((serc_info_ptr->ecc_mac_mode == 1) ||
                    (serc_info_ptr->ecc_mac_mode == 2)) {
                    /* only one field */
                    if (field_idx == 1) {
                        intr_status_fid = INVALIDf;
                    } else {
                        intr_status_fid = ecc_status_fid;
                    }
                } else {
                    intr_clr_fid = mac_status_fld_info[field_idx].intr_clr_field;
                    err_type = mac_status_fld_info[field_idx].err_type;
                    intr_status_fid = mac_status_fld_info[field_idx].intr_status_field;
                    clr_method = mac_status_fld_info[field_idx].clr_method;
                }
                if (intr_status_fid == INVALIDf) {
                    break;
                }
                rv = bcmdrd_pt_field_get(unit, status_reg, rval,
                                         intr_status_fid, &fval);
                if (SHR_FAILURE(rv)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          " Status register[%s] may not have field[%d]\n"),
                               bcmdrd_pt_sid_to_name(unit, status_reg), intr_status_fid));
                    SHR_ERR_EXIT(rv);
                }
                if (fval) {
                    if (mem_desc != NULL) {
                        LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "SER event may be reported by [%s]\n"),
                                  mem_desc));
                    }
                    break;
                }
            }
            /* All status fields needed to check are zero, go to check the next register */
            /* Value of register can be non-zero, but value of all status fields can be zero */
            if ((intr_status_fid == INVALIDf) &&
                (serc_info_ptr->ecc_mac_mode != 2)) {
                continue;
            }
            ser_misc_mem->blk_type = serc_info_ptr->blk_type;
            ser_misc_mem->tbl_inst = dyn_info.tbl_inst;
            /* when value is -1, it means the members are not used */
            ser_misc_mem->entry_idx = -1;
            ser_misc_mem->err_type = -1;
            ser_misc_mem->multi_bits = -1;
            ser_misc_mem->status_reg = INVALIDr;
            if (serc_info_ptr->ecc_mac_mode == 0) {
                /* get information from low level status register, and clear low level status register */
                rv = bcmptm_ser_mac_status_reg_clear(unit, clr_reg, intr_clr_fid,
                                                     clr_method, dyn_info.index,
                                                     dyn_info.tbl_inst,
                                                     err_type, ser_misc_mem);
            } else if (serc_info_ptr->ecc_mac_mode == 2) {
                /* SEC ECC SER error */
                crt_info->non_fifo_status_reg = 1;
                rv = bcmptm_ser_sec_status_reg_clear(unit, status_reg,
                                                     dyn_info.index,
                                                     dyn_info.tbl_inst, rval,
                                                     serc_info_ptr, ser_misc_mem);
            } else {
                /* ECC SER error */
                rv = bcmptm_ser_ecc_status_reg_clear(unit, status_reg,
                                                     dyn_info.index,
                                                     dyn_info.tbl_inst, rval,
                                                     serc_info_ptr, ser_misc_mem);
            }
            /* handle only one error at a time */
            SHR_ERR_EXIT(rv);
        }
    }
    crt_info->valid = 0;

    /* low level status register had been cleared, so clear top level interrupt status register */
    if ((group_status_reg != INVALIDr) &&
        (serc_info_ptr->ecc_mac_mode != 2)) {
        rv = bcmptm_ser_group_intr_status_reg_clear(unit, group_status_reg,
                                                    serc_info_ptr->intr_stat_fld,
                                                    group_dyn_info.tbl_inst);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "All SER events on %s instance[%d] have been corrected\n"),
               serc_info_ptr->mem_str, group_dyn_info.tbl_inst));
exit:
    if (SHR_FAILURE(rv)) {
        crt_info->valid = 0;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to process SER event for non mmu memory\n")));
    } else if (crt_info->valid == 1) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " %s instance[%d] has %s %s error at index %d\n"),
                  serc_info_ptr->mem_str,
                  dyn_info.tbl_inst,
                  (ser_misc_mem->multi_bits > 0) ? "multiple" : "single",
                  (ser_misc_mem->err_type > 0) ? "2bit": "1bit",
                  ser_misc_mem->entry_idx));
    }
    SHR_FUNC_EXIT();
}

/*
 * The first phase of SER process of IDB block.
 * Read raw data from status registers, and clear status registers.
 */
static int
bcmptm_ser_ipipe_idb_process(int unit,
                             bcmptm_pre_serc_info_t *pre_serc_info,
                             bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, pre_serc_info, crt_info);
}

/*
 * The first phase of SER process of IPIPE POST block.
 * Read raw data from status registers, and clear status registers.
 */
static int
bcmptm_ser_ipipe_post_process(int unit,
                              bcmptm_pre_serc_info_t *pre_serc_info,
                              bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, pre_serc_info, crt_info);
}

/*
 * The first phase of SER process of Port block.
 * Read raw data from status registers, and clear status registers.
 */
static int
bcmptm_ser_port_process(int unit,
                        bcmptm_pre_serc_info_t *pre_serc_info,
                        bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, pre_serc_info, crt_info);
}

/*
 * The first phase of SER process of ip/ep DII block.
 * Read raw data from status registers, and clear status registers.
 */
static int
bcmptm_ser_ipep_dii_process(int unit,
                              bcmptm_pre_serc_info_t *pre_serc_info,
                              bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, pre_serc_info, crt_info);
}

/*
 * The first phase of SER process of Macsec block.
 * Read raw data from status registers, and clear status registers.
 */
static int
bcmptm_ser_sec_process(int unit,
                              bcmptm_pre_serc_info_t *pre_serc_info,
                              bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, pre_serc_info, crt_info);
}

/*
 * The second phase of SER process of Port/IDB/POST blocks.
 * Parse raw data from status registers.
 */
static int
bcmptm_ser_non_mmu_info_parse(int unit,
                              bcmptm_ser_raw_info_t *crt_info,
                              bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem = NULL;

    SHR_FUNC_ENTER(unit);

    ser_misc_mem = (bcmptm_ser_misc_blk_crt_info_t *)(&crt_info->serc_info);

    spci->tbl_inst[0] = ser_misc_mem->tbl_inst;
    spci->blk_type = ser_misc_mem->blk_type;
    if (ser_misc_mem->entry_idx >= 0) {
        spci->index[0] = ser_misc_mem->entry_idx;
    }
    if (ser_misc_mem->multi_bits > 0) {
        spci->flags |= BCMPTM_SER_FLAG_MULTIBIT;
    }
    if (ser_misc_mem->err_type > 0) {
        spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
    }
    if (ser_misc_mem->err_type >= 0) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
    } else {
        /* should not be parity error */
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    spci->sid[0] = INVALIDm;
    spci->reported_sid = INVALIDm;
    spci->sid[1] = INVALIDm;
    spci->status_reg_sid = ser_misc_mem->status_reg;

    if (ser_misc_mem->magic_sid != INVALIDr) {
        spci->sid[0] = ser_misc_mem->magic_sid;
        spci->flags |= BCMPTM_SER_FLAG_NON_SBUS;
    }

    SHR_IF_ERR_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*
 * The second phase of SER process of IDB blocks.
 * Parse raw data from status registers.
 */
static int
bcmptm_ser_ipipe_idb_parse(int unit,
                           bcmptm_ser_raw_info_t *crt_info,
                           bcmptm_ser_correct_info_t *spci)
{
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    return bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
}

/*
 * The second phase of SER process of IPIPE POST blocks.
 * Parse raw data from status registers.
 */
static int
bcmptm_ser_ipipe_post_parse(int unit,
                            bcmptm_ser_raw_info_t *crt_info,
                            bcmptm_ser_correct_info_t *spci)
{
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    return bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
}

/*
 * The second phase of SER process of Port blocks.
 * Parse raw data from status registers.
 */
static int
bcmptm_ser_port_info_parse(int unit,
                           bcmptm_ser_raw_info_t *crt_info,
                           bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE;

    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_PORT;
    rv = bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
    return rv;
}

/*
 * The second phase of SER process of ip/ep DII blocks.
 * Parse raw data from status registers.
 */
static int
bcmptm_ser_ipep_dii_info_parse(int unit,
                               bcmptm_ser_raw_info_t *crt_info,
                               bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE;

    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    rv = bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
    return rv;
}

/*
 * The second phase of SER process of Macsec blocks.
 * Parse raw data from status registers.
 */
static int
bcmptm_ser_sec_info_parse(int unit,
                               bcmptm_ser_raw_info_t *crt_info,
                               bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE;

    rv = bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
    return rv;
}

/*
* For some PTs reported by H/W, there is no PTcache.
* So SER logic will map the reported view of the PTs to the view which has PTcache.
*/
static int
bcmptm_ser_correct_info_remap(int unit, bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE, out_count = 0, i;
    bcmbd_pt_dyn_info_t hw_dyn_info, out_dyn_info[ALPM_REMAP_COUNT];
    bcmdrd_sid_t sid[ALPM_REMAP_COUNT];

    SHR_FUNC_ENTER(unit);


    if (ser_alpm_info_get_f[unit] != NULL || ser_alpm_mtop_info_get_f[unit] != NULL) {
        hw_dyn_info.index = spci->index[0];
        hw_dyn_info.tbl_inst = spci->tbl_inst[0];
        if ( ser_alpm_info_get_f[unit] != NULL ) {
             rv = ser_alpm_info_get_f[unit](unit, spci->sid[0], hw_dyn_info,
                                            ALPM_REMAP_COUNT,
                                            &sid[0], &out_dyn_info[0],
                                            &out_count);
        }
        if (out_count == 0 && ser_alpm_mtop_info_get_f[unit] != NULL) {
             rv = ser_alpm_mtop_info_get_f[unit](unit, spci->sid[0], hw_dyn_info,
                                                ALPM_REMAP_COUNT,
                                                &sid[0], &out_dyn_info[0],
                                                &out_count);

        }

        if (SHR_SUCCESS(rv)) {
            for (i = 0; i < out_count; i++) {
                spci->sid[i] = sid[i];
                spci->index[i] = out_dyn_info[i].index;
                spci->tbl_inst[i] = out_dyn_info[i].tbl_inst;
            }
        } else {
            rv = SHR_E_NONE;
        }
    }
    if (out_count == 0) {
        rv = bcmptm_ser_ipep_mem_remap(unit, spci->sid[0], &spci->sid[0]);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to remap SER info of [%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, spci->sid[0])));
    }
    SHR_FUNC_EXIT();
}

/*
 * The second phase of SER process of IP/EP.
 * Parse raw data popped from FIFOs of IP/EP.
 */
static int
bcmptm_ser_ing_egr_fifo_parse(int unit,
                              bcmptm_ser_raw_info_t *crt_info,
                              bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_ing_egr_crt_info_t *ing_egr_info = (bcmptm_ser_ing_egr_crt_info_t *)(&crt_info->serc_info);
    uint32_t adext = 0, type;
    bcmptm_cmic_adext_info_t adext_info;
    bcmbd_pt_dyn_info_t dyn_info = {-1, 0};
    int rv = SHR_E_NONE;
    bcmdrd_sid_t sid;
    bcmptm_ser_log_flag_t err_type[] = {
        BCMPTM_SER_FLAG_ERR_IN_SOP,
        BCMPTM_SER_FLAG_ERR_IN_MOP,
        BCMPTM_SER_FLAG_ERR_IN_EOP,
        BCMPTM_SER_FLAG_ERR_IN_SBUS,
        BCMPTM_SER_FLAG_ERR_IN_OTHER
    };
    int blk_type = -1;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0, addr_flag;

    SHR_FUNC_ENTER(unit);

    if (ing_egr_info->multi) {
        spci->flags |= BCMPTM_SER_FLAG_MULTIBIT;
    }
    if (ing_egr_info->drop) {
        spci->flags |= BCMPTM_SER_FLAG_DROP;
    }
    if (ing_egr_info->hw_correction) {
        spci->flags |= BCMPTM_SER_FLAG_HW_CORRECTED;
    }
    if (ing_egr_info->ecc_parity > 0) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
        if (ing_egr_info->ecc_parity > 1) {
            spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
        }
    } else {
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    type = ing_egr_info->instruction_type;
    if (type < COUNTOF(err_type)) {
        spci->flags |= err_type[type];
    } else {
        spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    }

    spci->addr[0] = ing_egr_info->address;
    spci->blk_type = ing_egr_info->blk_type;
    blk_type = spci->blk_type;

    if (ing_egr_info->non_sbus) {
        spci->flags |= BCMPTM_SER_FLAG_NON_SBUS;
        spci->sid[0] = BB_MAGIC_SID_CREATE(ing_egr_info->reg_mem_base,
                                           ing_egr_info->pipe_stage);
    } else {
        adext_info.acc_type = -1;
        rv = bcmptm_ser_blknum_get(unit, INVALIDm, 0,
                                   blk_type, &(adext_info.blk_num));
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_cmic_adext_get(unit, adext_info, &adext);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        addr_flag = (ing_egr_info->reg) ?
            BCMDRD_SYMBOL_FLAG_REGISTER : BCMDRD_SYMBOL_FLAG_MEMORY;

        rv = bcmbd_pt_addr_decode(unit, adext, ing_egr_info->address,
                                  addr_flag, &sid, &dyn_info);
        SHR_IF_ERR_EXIT(rv);

        spci->index[0] = dyn_info.index;
        /* for port registers, table instance is equal to port number */
        spci->tbl_inst[0] = dyn_info.tbl_inst;
        if (!PT_IS_PORT_OR_SW_PORT_REG(unit, sid)) {
            rv = bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
            SHR_IF_ERR_EXIT(rv);
            /*
             * For PT which access type is unique or duplicate,
             * dyn_info.tbl_inst is -1 from bcmbd_pt_addr_decode.
             */
            if (PT_IS_UNIQUE_ACC_TYPE(acctype_str) ||
                PT_IS_DUPLICATE_ACC_TYPE(acctype_str)) {
                spci->tbl_inst[0] = ing_egr_info->blk_inst;
            }
        }
        spci->sid[0] = sid;
        rv = bcmptm_ser_correct_info_remap(unit, spci);
        SHR_IF_ERR_EXIT(rv);

        if (bcmdrd_pt_attr_is_cam(unit, sid) &&
            (ing_egr_info->ecc_parity == 0)) {
            spci->flags |= BCMPTM_SER_FLAG_TCAM;
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to parse information from ING or EGR FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * Get value of every field from entry data of SER FIFOm.
 * such as: ING_SER_FIFOm/EGR_SER_FIFOm.
 */
static int
bcmptm_ser_ing_egr_fifo_val_set(int unit, bcmdrd_sid_t sid, uint32_t *entry_data,
                                bcmdrd_fid_t *fid_list,
                                bcmptm_ser_ing_egr_fifo_fld_val_t *flds_val)
{
    int rv = SHR_E_NONE;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[0], &(flds_val->valid));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[1], &(flds_val->ecc_parity));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[2], &(flds_val->mem_type));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[3], &(flds_val->address));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[4], &(flds_val->pipe_stage));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[5], &(flds_val->intruction));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[6], &(flds_val->drop));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[7], &(flds_val->multiple));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[8], &(flds_val->non_sbus));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[9], &(flds_val->membase));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[10], &(flds_val->memindex));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[11], &(flds_val->regbase));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[12], &(flds_val->regindex));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    if (SHR_E_NONE == bcmdrd_pt_field_info_get(unit, sid, fid_list[13], &finfo)) {
        rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[13], &(flds_val->hw_correction));
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get field value from ING or EGR FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * The first phase of SER process of IP/EP.
 * Get raw data from FIFOs of IP/EP.
 */
static int
bcmptm_ser_ing_egr_fifo_pop(int unit,
                            bcmptm_pre_serc_info_t *pre_serc_info,
                            bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_ing_egr_fifo_fld_val_t fifo_fld_val;
    int is_reg, non_sbus = 0;
    uint32_t entry_data[2], rval[1] = {0}, fld_val = 0;
    bcmbd_pt_dyn_info_t dyn_info;
    bcmptm_ser_ing_egr_crt_info_t *ing_egr_info = NULL;
    bcmptm_serc_fifo_t *fifo_info = (bcmptm_serc_fifo_t *)pre_serc_info->stat_info;
    int blk_inst = pre_serc_info->blk_inst;

    SHR_FUNC_ENTER(unit);

    ing_egr_info = (bcmptm_ser_ing_egr_crt_info_t *)(&crt_info->serc_info);

    dyn_info.index = 0;
    dyn_info.tbl_inst = blk_inst;
    crt_info->valid = 0;

    if (fifo_info->stat_reg == INVALIDr) {
        crt_info->non_fifo_status_reg = 1;
        rv = bcmbd_pt_pop(unit, fifo_info->fifo_mem, &dyn_info, NULL, entry_data, 2);
        /* IPIPE FIFO is empty */
        if (SHR_FAILURE(rv)) {
            rv = SHR_E_NONE;
            SHR_ERR_EXIT(rv);
        }
    } else {
        rv = bcmptm_ser_pt_read(unit, fifo_info->stat_reg,
                                0, blk_inst, -1, rval, 1,
                                BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmdrd_pt_field_get(unit, fifo_info->stat_reg, rval,
                                 fifo_info->intr_stat_fld, &fld_val);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* EPIPE FIFO is empty */
        if (fld_val == 0) {
            rv = SHR_E_NONE;
            SHR_ERR_EXIT(rv);
        }
        rv = bcmbd_pt_pop(unit, fifo_info->fifo_mem, &dyn_info, NULL, entry_data, 2);
        SHR_IF_ERR_EXIT(rv);
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Found SER error from block [%s].\n"),
              fifo_info->blk_name));

    sal_memset(&fifo_fld_val, 0, sizeof(fifo_fld_val));
    rv = bcmptm_ser_ing_egr_fifo_val_set(unit, fifo_info->fifo_mem, entry_data,
                                         fifo_info->fifo_mem_fids, &fifo_fld_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    is_reg = fifo_fld_val.mem_type;
    non_sbus = fifo_fld_val.non_sbus;

    if (!is_reg) {
        ing_egr_info->index = fifo_fld_val.memindex;
        ing_egr_info->reg = 0;
        ing_egr_info->reg_mem_base = fifo_fld_val.membase;
        ing_egr_info->non_sbus = non_sbus;
    } else {
        ing_egr_info->index = fifo_fld_val.regindex;
        ing_egr_info->reg = 1;
        ing_egr_info->reg_mem_base = fifo_fld_val.regbase;
        ing_egr_info->non_sbus = non_sbus;
    }
    ing_egr_info->instruction_type = fifo_fld_val.intruction;
    ing_egr_info->address = fifo_fld_val.address;
    ing_egr_info->blk_inst = blk_inst;
    ing_egr_info->blk_type = fifo_info->blk_type;
    ing_egr_info->drop = fifo_fld_val.drop;
    ing_egr_info->ecc_parity = fifo_fld_val.ecc_parity;
    ing_egr_info->multi = fifo_fld_val.multiple;
    ing_egr_info->pipe_stage = fifo_fld_val.pipe_stage;
    ing_egr_info->hw_correction = fifo_fld_val.hw_correction;

    /* non_sbus may be changed to 1 from 0 */
    rv = bcmptm_ser_ipep_hw_info_remap(unit, ing_egr_info->address,
                                       ing_egr_info->blk_type,
                                       ing_egr_info->reg_mem_base,
                                       ing_egr_info->instruction_type,
                                       &ing_egr_info->address, &non_sbus);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (!is_reg && ing_egr_info->non_sbus != (uint32_t)non_sbus) {
        ing_egr_info->non_sbus = non_sbus;
    }
    crt_info->valid = 1;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to pop information"
                              " from ING or EGR FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/* Conduct the two SER correction phases of all blocks. */
static int
bcmptm_ser_fifo_pop_and_parse(int unit,
                              bcmptm_ser_info_type_t ser_info_type,
                              bcmptm_pre_serc_info_t *pre_serc_info,
                              bcmptm_ser_correct_info_t *spci,
                              bool *only_one)
{
    bcmptm_ser_raw_info_t crt_info;
    int rv = SHR_E_NONE;
    bcmptm_ser_pop_parse_f_map_t pop_parse_mapping[] = {
        { BCMPTM_SER_INFO_TYPE_SER_ENGINE, bcmptm_ser_tcam_fifo_pop, bcmptm_ser_tcam_fifo_entry_parse },
        { BCMPTM_SER_INFO_TYPE_MMU, bcmptm_ser_mmu_fifo_pop, bcmptm_ser_mmu_fifo_entry_parse },
        { BCMPTM_SER_INFO_TYPE_EPIPE, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_IPIPE, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_CLPORT, bcmptm_ser_port_process, bcmptm_ser_port_info_parse },
        { BCMPTM_SER_INFO_TYPE_XLPORT, bcmptm_ser_port_process, bcmptm_ser_port_info_parse },
        { BCMPTM_SER_INFO_TYPE_IPIPE_IDB, bcmptm_ser_ipipe_idb_process, bcmptm_ser_ipipe_idb_parse },
        { BCMPTM_SER_INFO_TYPE_L2_MGMT, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_EPIPE_2, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_IPIPE_DLB_ECMP, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_EPIPE_EDB, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_IPIPE_POST, bcmptm_ser_ipipe_post_process, bcmptm_ser_ipipe_post_parse },
        { BCMPTM_SER_INFO_TYPE_CDPORT, bcmptm_ser_port_process, bcmptm_ser_port_info_parse },
        { BCMPTM_SER_INFO_TYPE_PORT_IF_IDB, bcmptm_ser_ipipe_idb_process, bcmptm_ser_ipipe_idb_parse },
        { BCMPTM_SER_INFO_TYPE_PORT_IF_EDB, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse },
        { BCMPTM_SER_INFO_TYPE_IPIPE_DII, bcmptm_ser_ipep_dii_process, bcmptm_ser_ipep_dii_info_parse },
        { BCMPTM_SER_INFO_TYPE_EPIPE_DII, bcmptm_ser_ipep_dii_process, bcmptm_ser_ipep_dii_info_parse },
        { BCMPTM_SER_INFO_TYPE_SEC, bcmptm_ser_sec_process, bcmptm_ser_sec_info_parse },
    };
    int i = 0, num_mapping = COUNTOF(pop_parse_mapping);

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_mapping; i++) {
        if (pop_parse_mapping[i].ser_info_type != ser_info_type) {
            continue;
        }
        spci->detect_time = sal_time_usecs();
        if (pop_parse_mapping[i].ser_fifo_pop == NULL) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        crt_info.non_fifo_status_reg = 0;
        rv = pop_parse_mapping[i].ser_fifo_pop(unit, pre_serc_info, &crt_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* FIFO is empty, and interrupt status registers are cleared */
        if (rv == SHR_E_NONE && crt_info.valid == 0) {
            SHR_ERR_EXIT(SHR_E_EMPTY);
        }
        *only_one = crt_info.non_fifo_status_reg;
        if (pop_parse_mapping[i].ser_fifo_parse == NULL) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        rv = pop_parse_mapping[i].ser_fifo_parse(unit, &crt_info, spci);
        /* we reserve SHR_E_EMPTY to tell caller that the SER interrupt status is cleared */
        if (rv == SHR_E_EMPTY) {
            rv = SHR_E_FAIL;
        }
        /* exit */
        SHR_ERR_EXIT(rv);
    }
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "SER has on handler for SER info type[%d]\n"),
               ser_info_type));
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    if (rv != SHR_E_EMPTY && SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error in SER info type[%d]\n"),
                   ser_info_type));
    }
    SHR_FUNC_EXIT();
}

/* Print SER log. */
static void
bcmptm_ser_log_print(int unit, bcmdrd_sid_t pt_id, uint32_t index,
                     uint32_t inst, uint32_t address, uint32_t ser_type,
                     uint32_t ser_recovery, uint32_t ser_instruction,
                     uint32_t detect_utime, uint32_t correct_utime,
                     uint32_t correct, uint32_t drop)
{
    char *err_instruct_string[] = {
            "Error in: SOP cell EP/IP.",
            "Error in: MOP cell EP/IP.",
            "Error in: EOP cell EP/IP.",
            "Error in: SBUS transaction. EP/IP",
            "Error in: transaction - refresh, aging etc EP/IP.",
            "Error in: MMU.",
            "Error in: PORT.",
            "Error in: SEC."
        };
    char *err_type_string[] = {
            "ser_err_parity",
            "ser_err_ecc_1bit",
            "ser_err_ecc_2bit"
        };
    char *err_resp_string[] = {
            "unknown",
            "ser_recovery_cache_restore",
            "ser_recovery_entry_clear",
            "ser_recovery_no_operation"
        };
    uint32_t instruct_num = COUNTOF(err_instruct_string);
    uint32_t err_type_num = COUNTOF(err_type_string);
    uint32_t err_resp_num = COUNTOF(err_resp_string);

    if (bcmdrd_pt_is_valid(unit, pt_id)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "%s: index[%d] instance[%d] "
                             "address[0x%08x] has SER error\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id),
                  index, inst, address));
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Buffer or bus index[%d] "
                             "address[0x%08x] has SER error\n"),
                  index, address));
    }
    if (drop) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER caused packet drop.\n")));
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "SER type [%s], SER recovery type[%s], %s\n"),
             (ser_type >= err_type_num) ? "unknown" : err_type_string[ser_type],
             (ser_recovery >= err_resp_num) ? "unknown" : err_resp_string[ser_recovery],
             (ser_instruction >= instruct_num) ? "unknown" : err_instruct_string[ser_instruction]));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "The SER is detected at time(0x%08x) %s corrected at time(0x%08x)."
                         " System takes (0x%08x)us to process it\n"),
              detect_utime, correct ? "and is": "but is not",
              correct_utime, (correct_utime - detect_utime)));
}

/* Update SER log to LT SER_LOG, and then print SER log. */
static void
bcmptm_ser_log(int unit, bcmptm_ser_correct_info_t *spci, bool corrected)
{
    bcmdrd_sid_t pt_id = spci->sid[0];
    uint32_t address = spci->addr[0], index = spci->index[0];
    uint32_t inst = (spci->tbl_inst[0] < 0)? 0 : spci->tbl_inst[0];
    uint32_t ser_type = 0, ser_instruction = 0;
    uint32_t instruction_map[][2] = {
            { BCMPTM_SER_FLAG_ERR_IN_SOP, SER_INSTRUCTION_SOP },
            { BCMPTM_SER_FLAG_ERR_IN_MOP, SER_INSTRUCTION_MOP },
            { BCMPTM_SER_FLAG_ERR_IN_EOP, SER_INSTRUCTION_EOP },
            { BCMPTM_SER_FLAG_ERR_IN_SBUS, SER_INSTRUCTION_SBUS },
            { BCMPTM_SER_FLAG_ERR_IN_OTHER, SER_INSTRUCTION_OTHER },
            { BCMPTM_SER_FLAG_ERR_IN_MMU, SER_INSTRUCTION_MMU },
            { BCMPTM_SER_FLAG_ERR_IN_PORT, SER_INSTRUCTION_PORT },
            { BCMPTM_SER_FLAG_ERR_IN_SEC, SER_INSTRUCTION_SEC },
        };
    int map_num = 0, i = 0, rv = SHR_E_NONE;
    sal_usecs_t detect_utime = 0, correct_utime = 0;
    uint32_t drop = (spci->flags & BCMPTM_SER_FLAG_DROP) ? 1 : 0;
    bcmptm_ser_log_flds_t *ser_log = NULL;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    correct_utime = sal_time_usecs();
    detect_utime = spci->detect_time;

    if (spci->flags & BCMPTM_SER_FLAG_PARITY) {
        ser_type = SER_ERR_PARITY;
    } else {
        if (spci->flags & BCMPTM_SER_FLAG_DOUBLEBIT) {
            ser_type = SER_ERR_ECC_2BIT;
        } else {
            ser_type = SER_ERR_ECC_1BIT;
        }
    }
    ser_instruction = SER_INSTRUCTION_OTHER;
    map_num = COUNTOF(instruction_map);
    for (i = 0; i < map_num; i++) {
        if (spci->flags & instruction_map[i][0]) {
            ser_instruction = instruction_map[i][1];
            break;
        }
    }
    if ((SER_RECOVERY_NO_OPERATION == spci->resp_type) &&
        !(spci->flags & BCMPTM_SER_FLAG_HW_CORRECTED)) {
        corrected = 0;
    }
    ser_log = cth_drv->ser_log_cache_get(unit);
    ser_log->timestamp = correct_utime;
    ser_log->ser_recovery_type = spci->resp_type;
    if (spci->flags & BCMPTM_SER_FLAG_ERR_IN_SHADOW) {
        pt_id = spci->reported_sid;
    }
    ser_log->pt_id = pt_id;
    ser_log->pt_index = index;
    ser_log->pt_instance = inst;
    ser_log->ser_err_type = ser_type;
    ser_log->ser_instruction_type = ser_instruction;
    ser_log->ser_err_corrected = corrected;
    ser_log->blk_type = spci->blk_type;
    if (spci->flags & BCMPTM_SER_FLAG_HF) {
        ser_log->hw_fault = 1;
    } else {
        ser_log->hw_fault = 0;
    }
    if (spci->flags & BCMPTM_SER_FLAG_HSF) {
        ser_log->high_severity_err = 1;
    } else {
        ser_log->high_severity_err = 0;
    }
    /* write to IMM LT */
    rv = cth_drv->ser_log_imm_update(unit);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to update cache data to IMM SER_LOGt\n")));
    }
    bcmptm_ser_log_print(unit, pt_id, spci->index[0],
                         inst, address, ser_type,
                         spci->resp_type, ser_instruction,
                         (uint32_t)detect_utime, (uint32_t)correct_utime,
                         corrected, drop);
}

/* Update SER statistics to LT SER_STATS. */
static void
bcmptm_ser_statistics(int unit, bcmptm_ser_correct_info_t *spci)
{
    uint32_t flags = spci->flags;
    bcmdrd_sid_t pt_id = spci->sid[0];
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    /* write to IMM LT */
    cth_drv->ser_stats_update(unit, pt_id, flags,
                              spci->blk_type, spci->resp_type);
}

/* Record SER error entry data to SER_LOG. */
static int
bcmptm_ser_error_entry_record(int unit, bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE, rv_en = SHR_E_EMPTY;
    uint32_t error_entry[BCMDRD_MAX_PT_WSIZE], error_entry_logging = 0;
    bcmptm_ser_log_flds_t *ser_log = NULL;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    uint32_t ser_en = 0;
    bool cache_en = 0;

    SHR_FUNC_ENTER(unit);

    error_entry_logging = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_ERRONEOUS_ENTRIES_LOGGINGf);
    if (error_entry_logging == 0 || bcmdrd_pt_is_valid(unit, spci->sid[0]) == 0) {
        return rv;
    }
    /*
     * If one PT is not cacheable, it may be updated by H/W, so we can't disable SER checking.
     * Furthermore, some PTs like MMU_MB_PAYLOAD_* can't be read by SBUS.
     */
    (void)bcmptm_pt_attr_is_cacheable(unit, spci->sid[0], &cache_en);
    if (cache_en == 0 && spci->resp_type == SER_RECOVERY_NO_OPERATION) {
        return rv;
    }
    /* disable PT SER checking */
    rv_en = bcmptm_ser_pt_ser_enable(unit, spci->sid[0], BCMDRD_SER_EN_TYPE_EC, 0);
    rv = rv_en;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(error_entry, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
    /* read from H/W */
    rv = bcmptm_ser_pt_read(unit, spci->sid[0], spci->index[0],
                            spci->tbl_inst[0], -1,
                            error_entry, BCMDRD_MAX_PT_WSIZE,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_log = cth_drv->ser_log_cache_get(unit);
    sal_memcpy(ser_log->err_entry_content, error_entry, BCMDRD_MAX_PT_WSIZE);

exit:
    ser_en = cth_drv->pt_ctrl_fld_val_get
        (unit, spci->sid[0], SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    /* re-enable parity or ecc checking */
    if (rv_en == SHR_E_NONE && ser_en) {
        (void)bcmptm_ser_pt_ser_enable(unit, spci->sid[0], BCMDRD_SER_EN_TYPE_EC, 1);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to record error entry of [%s] to cache!\n"),
                   bcmdrd_pt_sid_to_name(unit, spci->sid[0])));
    }
    SHR_FUNC_EXIT();
}

/*
 * If a PT has hard fault or high severity error, SER logic will report that to applications.
 */
static void
bcmptm_ser_hf_severity_record(int unit, bcmdrd_sid_t pt_id, uint32_t flags)
{
    uint32_t hf = 1, severity = 1;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    /* notify users there is HW fault */
    if (BCMPTM_SER_FLAG_HF & flags) {
        (void)cth_drv->ser_hw_fault_notify(unit, pt_id, hf, 0);
    } else if (BCMPTM_SER_FLAG_HSF & flags) {
        (void)cth_drv->ser_hw_fault_notify(unit, pt_id, 0, severity);
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "There is no HF or HSF for [%s]!!\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
        return;
    }
}

/* Check whether SID0 and SID0 share the same physical table. */
static int
bcmptm_ser_same_pt_check(int unit, bcmdrd_sid_t sid0, bcmdrd_sid_t sid1)
{
    /* static */
    static bcmdrd_pt_ser_en_ctrl_t en_ctrl_0, en_ctrl_1;
    static bcmdrd_sid_t sid0_last = INVALIDm, sid1_last = INVALIDm;
    int rv = SHR_E_NONE;

    if (sid0_last != sid0) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid0, &en_ctrl_0);
        if (SHR_FAILURE(rv)) {
            return FALSE;
        }
        sid0_last = sid0;
    }
    if (sid1_last != sid1) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid1, &en_ctrl_1);
        if (SHR_FAILURE(rv)) {
            return FALSE;
        }
        sid1_last = sid1;
    }
    if (en_ctrl_0.sid == en_ctrl_1.sid &&
        en_ctrl_0.fid == en_ctrl_1.fid &&
        en_ctrl_0.fmask == en_ctrl_1.fmask) {
        return TRUE;
    }
    return FALSE;
}

/* Prepare for SER high severity fault check. */
static void
bcmptm_ser_severity_fault_check_init(int unit, bcmptm_ser_cth_drv_t *cth_drv)
{
    sal_usecs_t past_time = ser_sf_checking_latest_time[unit];
    sal_usecs_t latest_time = sal_time_usecs();
    sal_usecs_t distance = 0, interval = 0;

    if (past_time < latest_time) {
        distance = latest_time - past_time;
    } else {
        distance = (sal_usecs_t)(-1) - past_time + latest_time;
    }
    interval = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_HIGH_SEVERITY_ERR_INTERVALf);
    interval *= 1000000;
    if (distance <= interval && past_time) {
        return;
    }
    /* If latest_time is zero, assign it as 1, initialization value of ser_sf_checking_latest_time is 0. */
    latest_time = latest_time ? latest_time : 1;

    ser_sf_checking_latest_time[unit] = latest_time;

    ser_corrected_index[unit] = 0;
    ser_corrected_count[unit] = 0;
}

/*
 * Conduct SER high severity fault check for all PTs,
 * internal buffers and buses.
 */
static int
bcmptm_ser_severity_fault_check(int unit, bcmptm_ser_correct_info_t *spci)
{
    int k = 0;
    uint32_t time = 0;
    bcmdrd_sid_t sid_cached, sid;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    uint32_t hsf_limit = 0, hsf_en = 0;
    bool is_pt_id = false;

    sid = spci->sid[0];
    is_pt_id = bcmdrd_pt_is_valid(unit, sid);
    if (is_pt_id) {
        hsf_en = cth_drv->pt_ctrl_fld_val_get
            (unit, sid, SER_PT_CONTROLt_HIGH_SEVERITY_ERR_SUPPRESSIONf);
    } else {
        /* Internal buffer or bus */
        hsf_en = cth_drv->ctrl_fld_val_get
            (unit, SER_CONTROLt_HIGH_SEVERITY_ERR_SUPPRESSIONf);
    }
    if (hsf_en == 0) {
        return FALSE;
    }
    hsf_limit = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_HIGH_SEVERITY_ERR_THRESHOLDf);
    if (SERC_CACHE_MAX_NUM <= hsf_limit) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Severity error threshold should be less than [%d]!!\n"),
                   SERC_CACHE_MAX_NUM));
        return FALSE;
    }
    bcmptm_ser_severity_fault_check_init(unit, cth_drv);

    /* must not use bcmdrd_pt_is_valid here*/
    if (sid == INVALIDm) {
        return FALSE;
    }
    ASSIGN_NEW_SID_TO_RING_CACHE(unit, sid);

    for (k = 0; k < ser_corrected_count[unit]; k++) {
        sid_cached = ser_corrected_sids[unit][k];
        if (sid == sid_cached) {
            time++;
        } else if (bcmptm_ser_same_pt_check(unit, sid, sid_cached)) {
            time++;
        }
    }
    if (time < hsf_limit) {
        return FALSE;
    }
    spci->flags |= BCMPTM_SER_FLAG_HSF;
    /* High severity SER error of buffer or bus or PTs will be reported again and again */
    if (is_pt_id) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Severity fault detected at: [%s][%d][%d]!!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid),
                  spci->tbl_inst[0], spci->index[0]));
        bcmptm_ser_hf_severity_record(unit, sid, spci->flags);
    }
    return TRUE;
}

/* Conduct SER hard fault check for all PTs. */
static int
bcmptm_ser_hard_fault_check(int unit, bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE, hf = FALSE, i, k, entry_size, sw_cmp_err = FALSE;
    uint32_t hw_entry[BCMDRD_MAX_PT_WSIZE], sw_entry[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_sid_t sid;
    int index, tbl_inst, cached_resp = 0;
    const size_t wsize = BCMDRD_MAX_PT_WSIZE;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    /* Don't need to do HW checking */
    if (cth_drv->ctrl_fld_val_get(unit, SER_CONTROLt_HW_FAULTf) == 0) {
        return FALSE;
    }
    /* can't do hard fault checking for this recovery type */
    if (spci->resp_type == SER_RECOVERY_NO_OPERATION) {
        return FALSE;
    }
    cached_resp = (spci->resp_type != SER_RECOVERY_ENTRY_CLEAR) ? 1 : 0;
    for (i = 0; i < 2; i++) {
        hf = FALSE;
        sid = spci->sid[i];
        index = spci->index[i];
        tbl_inst = spci->tbl_inst[i];
        if (bcmdrd_pt_is_valid(unit, sid) == FALSE) {
            break;
        }
        sal_memset(hw_entry, 0, sizeof(hw_entry));
        /* Read data from H/W */
        rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, -1,
                                hw_entry, wsize, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        if (SHR_FAILURE(rv)) {
            /* Obvious hard fault */
            hf = TRUE;
        } else if (cached_resp) {
            sw_cmp_err = FALSE;
            sal_memset(sw_entry, 0, sizeof(sw_entry));
            pt_dyn_info.index = spci->index[i];
            pt_dyn_info.tbl_inst = spci->tbl_inst[i];
            /* Read data from S/W */
            rv = bcmptm_wal_serc_data_find(unit, 0, sid, &pt_dyn_info,
                                           wsize, -1,
                                           BCMPTM_TCAM_SLICE_INVALID, sw_entry);
            if (SHR_FAILURE(rv)) {
                /* Read data from PTcache */
                rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, -1,
                                        sw_entry, wsize, 0);
                if (SHR_FAILURE(rv)) {
                    continue;
                }
            }
            bcmptm_ser_parity_field_clear(unit, sid, hw_entry);
            bcmptm_ser_parity_field_clear(unit, sid, sw_entry);
            if (sal_memcmp(sw_entry, hw_entry, sizeof(hw_entry)) != 0) {
                hf = TRUE;
                sw_cmp_err = TRUE;
            }
        }
        if (hf) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Hard fault detected (read) at: %s instance[%d] index[%d]!!\n"),
                       bcmdrd_pt_sid_to_name(unit, sid), tbl_inst, index));
            entry_size = bcmdrd_pt_entry_wsize(unit, sid);
            for (k = 0; k < entry_size; k++) {
                if (sw_cmp_err) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "SW data[%d]: 0x%08"PRIx32","
                                          " HW data[%d]: 0x%08"PRIx32"\n"),
                               k, sw_entry[k], k, hw_entry[k]));
                } else {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "HW data[%d]: 0x%08"PRIx32"\n"),
                               k, hw_entry[k]));
                }
            }
            spci->flags |= BCMPTM_SER_FLAG_HF;
            bcmptm_ser_hf_severity_record(unit, sid, spci->flags);
        }
    }
    return (spci->flags & BCMPTM_SER_FLAG_HF) ? 1 : 0;
}

/* Correct SER error for all PTs. */
static int
bcmptm_ser_correct(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   int tbl_copy, int ser_resp)
{
    int rv = SHR_E_NONE;
    int index, tbl_inst;
    uint32_t correct_data[BCMDRD_MAX_PT_WSIZE];
    size_t wsize = BCMDRD_MAX_PT_WSIZE;

    SHR_FUNC_ENTER(unit);

    index = pt_dyn_info->index;
    tbl_inst = pt_dyn_info->tbl_inst;

    if (ser_resp == SER_RECOVERY_NO_OPERATION) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    } else if (ser_resp == SER_RECOVERY_ENTRY_CLEAR) {
        sal_memset(correct_data, 0, sizeof(correct_data));
    } else {
        sal_memset(correct_data, 0, sizeof(correct_data));
        rv = bcmptm_wal_serc_data_find(unit, 0, sid, pt_dyn_info,
                                       wsize, 0, BCMPTM_TCAM_SLICE_INVALID,
                                       correct_data);
        if (SHR_FAILURE(rv)) {
            /* read from PTcache */
            rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, -1,
                                    correct_data, wsize, 0);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    rv = bcmptm_ser_pt_write(unit, sid, index, tbl_inst, -1, correct_data, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error of %s index [%d] instance [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), index, tbl_inst));
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Correct SER error of %s index [%d] instance [%d] successfully\n"),
                 bcmdrd_pt_sid_to_name(unit, sid), index, tbl_inst));
    }
    SHR_FUNC_EXIT();
}

/* Correct SER error for counter PTs. */
static int
bcmptm_ser_cci_ser_req(int unit, bcmptm_ser_correct_info_t *spci,
                       bcmdrd_sid_t *sids_ptr, int sid_num)
{
    int rv = SHR_E_NONE, i = 0, sid_num_tmp = 0;
    bcmptm_cci_ser_req_info_t *ser_req_info_ptr = NULL;
    size_t req_info_size = 0;
    int pt_err = 0, hf = FALSE;

    SHR_FUNC_ENTER(unit);

    if (spci == NULL) {
        rv = SHR_E_PARAM;
        SHR_IF_ERR_EXIT(rv);
    }
    /* SER error is reported by ECC status register */
    pt_err = (sids_ptr != NULL) ? 0 : 1;
    /* One PT reports SER error */
    if (pt_err) {
        if (bcmdrd_pt_is_valid(unit, spci->sid[0])) {
            sid_num_tmp++;
            if (bcmdrd_pt_is_valid(unit, spci->sid[1])) {
                sid_num_tmp++;
            }
        } else {
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_EXIT(rv);
        }
        /*
         * 1/ Return for a PT which recovery type is NONE,
         * 2/ for single bit error of a PT which owns ECC protection.
         */
        if (spci->resp_type == SER_RECOVERY_NO_OPERATION) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    } else {
        if (sid_num == 0) {
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_EXIT(rv);
        }
        sid_num_tmp = sid_num;
    }
    req_info_size = sizeof(bcmptm_cci_ser_req_info_t) * sid_num_tmp;
    ser_req_info_ptr = sal_alloc(req_info_size, "bcmptmSerCciReqInfo");
    rv = SHR_E_MEMORY;
    SHR_NULL_CHECK(ser_req_info_ptr, rv);

    if (pt_err) {
        for (i = 0; i < sid_num_tmp; i++) {
            ser_req_info_ptr[i].sid = spci->sid[i];
            ser_req_info_ptr[i].dyn_info.index = spci->index[i];
            ser_req_info_ptr[i].dyn_info.tbl_inst = spci->tbl_inst[i];
        }
    } else {
        for (i = 0; i < sid_num_tmp; i++) {
            ser_req_info_ptr[i].sid = sids_ptr[i];
            ser_req_info_ptr[i].dyn_info.index = spci->index[0];
            ser_req_info_ptr[i].dyn_info.tbl_inst = spci->tbl_inst[0];
        }
    }
    spci->sid[0] = ser_req_info_ptr[0].sid;
    rv = bcmptm_scor_locks_take(unit, spci->sid[0]);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to take locks\n")));

        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    (void)bcmptm_ser_error_entry_record(unit, spci);

    rv = bcmptm_pt_cci_ser_array_req(unit, ser_req_info_ptr, sid_num_tmp);

    if (pt_err && SHR_SUCCESS(rv)) {
        hf = bcmptm_ser_hard_fault_check(unit, spci);
        if (hf == TRUE) {
            rv = SHR_E_FAIL;
        }
    }
    (void)bcmptm_scor_locks_give(unit, spci->sid[0]);

    if (hf == false) {
        (void)bcmptm_ser_severity_fault_check(unit, spci);
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    if (ser_req_info_ptr != NULL) {
        sal_free(ser_req_info_ptr);
    }
    if (SHR_FAILURE(rv)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Fail to correct CCI SER error\n")));
    }
    SHR_FUNC_EXIT();

}

/* Correct SER error for PTs which are not XOR, LP tables. */
static int
bcmptm_ser_mem_direct_correct(int unit, bcmdrd_sid_t sid,
                              int index, int tbl_inst, int tbl_copy,
                              int ser_resp, uint32_t *flags)
{
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = tbl_inst;

    rv = bcmptm_ser_correct(unit, sid, &pt_dyn_info, tbl_copy, ser_resp);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error in %s\n"),
                   bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Correct SER error for PTs which are LP tables. */
static int
bcmptm_ser_lp_hw_correct(int unit, bcmdrd_sid_t reported_sid,
                         int reported_inst,  int reported_index,
                         bcmptm_ser_sram_info_t *sram_info, uint32_t *flags)
{
    bcmdrd_sid_t pt_view = INVALIDm;
    uint32_t corrected_index = 0;
    uint32_t ram = 0, index_count = 0, i = 0;
    int rv = SHR_E_NONE;
    uint32_t recovery_data[BCMDRD_MAX_PT_WSIZE];
    int corrected_inst = (reported_inst == 0) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    for (ram = 0; ram < sram_info->ram_count; ram++) {
        pt_view = sram_info->pt_view[ram];
        if (pt_view == INVALIDm) {
            break;
        }
        index_count = sram_info->index_count[ram];
        if (index_count == 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Number of index should not be zero\n")));
            break;
        }
        for (i = 0; i < index_count; i++) {
            corrected_index = sram_info->indexes[ram][i];
            sal_memset(recovery_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
            /* read correction data from pipe which has no SER error. */
            rv = bcmptm_ser_pt_read(unit, pt_view, corrected_index,
                                    corrected_inst, -1, recovery_data,
                                    BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_EXIT(rv);
            /* write correction data to pipe which has SER error. */
            rv = bcmptm_ser_pt_write(unit, pt_view, corrected_index, reported_inst,
                                     -1, recovery_data, 0);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error of [%s] by H/W recovery type\n"),
                   bcmdrd_pt_sid_to_name(unit, reported_sid)));
    }
    SHR_FUNC_EXIT();
}

/* Correct SER error for PTs which have XOR banks. */
static int
bcmptm_ser_mem_special_correct(int unit, bcmdrd_sid_t reported_sid,
                               int reported_inst, int reported_index,
                               bcmptm_ser_sram_info_t *sram_info,
                               int ser_resp, uint32_t *flags)
{
    uint32_t ram = 0, index_count = 0, i;
    bcmdrd_sid_t pt_view;
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t fld_old_val[BCMPTM_SER_EXTRA_REG_NUM] = {0 , 0}, fld_old_check;
    uint32_t fld_new_val;
    bcmptm_ser_control_reg_info_t ctrl_reg_info;
    bcmdrd_sid_t ctrl_reg = INVALIDr;
    bcmdrd_fid_t fld_list[BCMPTM_SER_EXTRA_REG_NUM];

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_sram_ctrl_reg_get(unit, reported_sid,
                                      reported_index, &ctrl_reg_info);
    SHR_IF_ERR_EXIT(rv);

    /* Disable parity checking when write HASH_XOR memory PTs */
    ctrl_reg = ctrl_reg_info.wr_check_reg;
    if (ctrl_reg != INVALIDr) {
        fld_list[0] = ctrl_reg_info.wr_check_field;
        fld_list[1] = INVALIDf; /* must */
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, 0, fld_list,
                                                1, NULL, &fld_old_check);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Freeze l2x H/W operations, etc. */
    for (i = 0; i < BCMPTM_SER_EXTRA_REG_NUM; i++) {
        ctrl_reg = ctrl_reg_info.extra_reg[i];
        if (ctrl_reg == INVALIDr) {
            break;
        }
        fld_list[0] = ctrl_reg_info.extra_field[i];
        fld_list[1] = INVALIDf; /* must */
        fld_new_val = ctrl_reg_info.extra_field_val[i];
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, 0, fld_list, 1,
                                                &fld_new_val, &fld_old_val[i]);
        SHR_IF_ERR_EXIT(rv);
    }
    for (ram = 0; ram < sram_info->ram_count; ram++) {
        pt_view = sram_info->pt_view[ram];
        if (pt_view == INVALIDm) {
            break;
        }
        pt_dyn_info.tbl_inst = reported_inst;

        index_count = sram_info->index_count[ram];
        if (index_count == 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Number of index should not be zero\n")));
            break;
        }
        for (i = 0; i < index_count; i++) {
            pt_dyn_info.index = sram_info->indexes[ram][i];
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Correct XOR memory table index [%d] in Ram [%d]\n"),
                       pt_dyn_info.index, ram));
            rv = bcmptm_ser_correct(unit, pt_view, &pt_dyn_info, -1, ser_resp);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to correct SER error in %s\n"),
                           bcmdrd_pt_sid_to_name(unit, pt_view)));
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }
    /* Restore parity checking when write HASH_XOR memory PTs */
    ctrl_reg = ctrl_reg_info.wr_check_reg;
    if (ctrl_reg != INVALIDr) {
        ctrl_reg = ctrl_reg_info.wr_check_reg;
        fld_list[0] = ctrl_reg_info.wr_check_field;
        fld_list[1] = INVALIDf; /* must */
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, 0, fld_list,
                                                fld_old_check ? 1 : 0, NULL, NULL);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Thaw l2x H/W operations, etc. */
    for (i = 0; i < BCMPTM_SER_EXTRA_REG_NUM; i++) {
        ctrl_reg = ctrl_reg_info.extra_reg[i];
        if (ctrl_reg == INVALIDr) {
            break;
        }
        fld_list[0] = ctrl_reg_info.extra_field[i];
        fld_list[1] = INVALIDf; /* must */
        fld_new_val = fld_old_val[i];
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, 0, fld_list,
                                                1, &fld_new_val, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error of [%s] by special type\n"),
                   bcmdrd_pt_sid_to_name(unit, reported_sid)));
    }
    SHR_FUNC_EXIT();
}

/*
 * Handle SER information for internal buffers or buses.
 * Such SER error can't be corrected.
 */
static void
bcmptm_ser_buffer_bus_ser_req(int unit, bcmptm_ser_correct_info_t *spci)
{
    int sf = FALSE, rv = SHR_E_NONE;
    char *base_info = NULL, *bd_bb_name = NULL;
    int  addrbase = -1, stage_id = -1;
    bool mem_flag = 0;

    addrbase = BB_MAGIC_SID_MEMBASE_GET(spci->sid[0]);
    stage_id = BB_MAGIC_SID_STAGE_ID_GET(spci->sid[0]);
    rv = bcmptm_ser_hwmem_base_info_get(unit, addrbase, stage_id,
                                        &bd_bb_name, &base_info, &mem_flag);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER logical does not know Mem hwbase[0x%x]\n"),
                  addrbase));
        return;
    }
    if (mem_flag) {
        spci->flags |= BCMPTM_SER_FLAG_NON_SBUS_MEM;
    } else {
        spci->flags |= BCMPTM_SER_FLAG_NON_SBUS_BUS;
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\"%s\", Mem hwbase[0x%x] stage id[0x%x] has SER error...\n"),
              base_info, addrbase, stage_id));

    sf = bcmptm_ser_severity_fault_check(unit, spci);
    if (sf == FALSE) {
        return;
    }
    if (sal_strcmp(bd_bb_name, "NONE") == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER error is reported from unknown buffer or bus\n")));
        return;
    }
    LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "\"%s\", Mem hwbase[0x%x] has severity SER error, so"
                        " disable its SER checking\n"),
             base_info, addrbase));
}

/* Handle SER information for all PTs. */
static int
bcmptm_ser_non_cci_ser_req(int unit, bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE, hf = FALSE;
    int i;
    bcmptm_ser_sram_info_t *sram_info = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_scor_locks_take(unit, spci->sid[0]);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to take locks\n")));

        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    (void)bcmptm_ser_error_entry_record(unit, spci);

    for (i = 0; i < 2; i++) {
        if (spci->sid[i] == INVALIDm) {
            break;
        }

        if (spci->resp_type == SER_RECOVERY_NO_OPERATION) {
            rv = SHR_E_FAIL;
            break;
        }
        sram_info = sal_alloc(sizeof(bcmptm_ser_sram_info_t), "bcmptmSerXorCorrectInfo");
        rv = bcmptm_ser_xor_lp_mem_info_get(unit, spci->sid[i],
                                            spci->index[i], sram_info);
        if (SHR_SUCCESS(rv)) {
            /* LP tables reports SER error, except FPEM_LPm */
            if (sram_info->hw_recovery) {
                rv = bcmptm_ser_lp_hw_correct(unit, spci->sid[i],
                                              spci->tbl_inst[i],
                                              (int)spci->index[i], sram_info,
                                              &spci->flags);
            } else {
                /* XOR tables reports SER error */
                rv = bcmptm_ser_mem_special_correct(unit, spci->sid[i],
                                                    spci->tbl_inst[i],
                                                    (int)spci->index[i], sram_info,
                                                    spci->resp_type, &spci->flags);
            }
            if (SHR_FAILURE(rv)) {
                break;
            }
        } else if (rv == SHR_E_UNAVAIL) {
            /* non xor or lp reports SER error */
            rv = bcmptm_ser_mem_direct_correct(unit, spci->sid[i], spci->index[i],
                                               spci->tbl_inst[i], spci->tbl_copy,
                                               spci->resp_type, &spci->flags);
            if (SHR_FAILURE(rv)) {
                break;
            }
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to get sram information of HASH_XOR or LP table [%s]\n"),
                       bcmdrd_pt_sid_to_name(unit, spci->sid[i])));
            rv = SHR_E_INTERNAL;
            break;
        }
    }
    /*
     * Hard fault checking should be protected by scor_locks,
     * because the routine may use data from PTcache.
     */
    if (SHR_SUCCESS(rv)) {
        hf = bcmptm_ser_hard_fault_check(unit, spci);
        if (hf == TRUE) {
            rv = SHR_E_FAIL;
        }
    }
    (void)bcmptm_scor_locks_give(unit, spci->sid[0]);
    /*
     * 1/ Some PTs' SER recovery type is BCMDRD_SER_RESP_NONE or
     * there H/W fault in PTs but H/W fault checking is not enabled.
     * 2/ the PTs are read or hit by H/W or S/W continuously.
     * So SER error of the PTs is reported again and agian. SER logic will
     * notify applications.
     */
    if (hf == FALSE) {
        (void)bcmptm_ser_severity_fault_check(unit, spci);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (sram_info != NULL) {
        sal_free(sram_info);
    }
    if (SHR_FAILURE(rv)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Fail to correct non CCI SER error\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * For all TCAMs if DROP_EN=1,
 * then SW has to clear it using STICKY_BIT_CLEAR(0->1, 1->0),
 * else all traffic hitting that TCAM will be dropped.
 */
static int
bcmptm_ser_req_drop_clear(int unit, bcmdrd_sid_t sid)
{
    int rv;
    bcmdrd_sid_t ctrl_sid = INVALIDr;
    bcmdrd_fid_t ctrl_fids[2] = { INVALIDf, INVALIDf };

    rv = bcmptm_ser_tcam_sticky_bit_clr_get(unit, sid, &ctrl_sid, ctrl_fids);

    if (rv == SHR_E_NONE && ctrl_sid != INVALIDr) {
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_sid, 0, ctrl_fids,
                                                1, NULL, NULL);
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_sid, 0, ctrl_fids,
                                                0, NULL, NULL);
    } else if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
    }
    return rv;
}

/*
 * The third phase of SER correction.
 * Conduct SER correction according to SER information
 * parsed by the second phase.
 */
static bool
bcmptm_ser_req(int unit, bcmptm_ser_correct_info_t *spci)
{
    int rv = SHR_E_NONE;
    bcmptm_ptcache_ctr_info_t ctr_info;
    const bcmdrd_sid_t *sids_ptr = NULL;
    uint32_t sids_num = 0, i = 0, sids_ctr_num = 0;
    uint32_t index = 0;
    bcmdrd_sid_t *sids_ptr_ctr = NULL;
    uint32_t fid_lt = (spci->flags & BCMPTM_SER_FLAG_DOUBLEBIT) ?
        SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_DOUBLE_BITf :
        SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_SINGLE_BITf;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    SHR_FUNC_ENTER(unit);

    if (spci->flags & BCMPTM_SER_FLAG_NON_SBUS) {
        bcmptm_ser_buffer_bus_ser_req(unit, spci);
    }
    /* spci->sid could be BB_MAGIC_SID_CREATE(mem_base, stage) */
    if (bcmdrd_pt_is_valid(unit, spci->sid[0])) {
        if (spci->status_reg_sid != INVALIDr) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "SID of status register should be INVALIDr\n")));
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        spci->resp_type = cth_drv->pt_status_fld_val_get(unit, spci->sid[0], fid_lt);

        /* Check whether SID belongs to counter or not */
        (void)bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, spci->sid[0],
                                          &ctr_info);
        if (ctr_info.pt_is_ctr) {
            rv = bcmptm_ser_cci_ser_req(unit, spci, NULL, 0);
        } else {
            rv = bcmptm_ser_non_cci_ser_req(unit, spci);
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_req_drop_clear(unit, spci->sid[0]);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else if (bcmdrd_pt_is_valid(unit, spci->status_reg_sid)) {
        /* The same index */
        (void)bcmptm_ser_status_reg_map(unit, spci, &sids_ptr,
                                        &index, &sids_num);
        if (sids_num == 0) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        sids_ptr_ctr = sal_alloc(sizeof(bcmdrd_sid_t) * sids_num, "bcmptmSerCntPts");
        SHR_NULL_CHECK(sids_ptr_ctr, SHR_E_MEMORY);

        for (i = 0; i < sids_num; i++) {
            /* Check whether SID belongs to counter or not */
            (void)bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sids_ptr[i],
                                              &ctr_info);
            if (ctr_info.pt_is_ctr) {
                sids_ptr_ctr[sids_ctr_num] = sids_ptr[i];
                sids_ctr_num++;
            } else {
                spci->sid[0] = sids_ptr[i];
                spci->sid[1] = INVALIDm;
                /* index can be remapped */
                spci->index[0] = index;
                spci->resp_type = cth_drv->pt_status_fld_val_get(unit, spci->sid[0], fid_lt);
                /* Does not need to modify spci->tbl_inst[0] */
                rv = bcmptm_ser_non_cci_ser_req(unit, spci);
                SHR_IF_ERR_EXIT(rv);
            }
        }
        if (sids_ctr_num) {
            /* Counter registers should be cleared. */
            spci->resp_type = SER_RECOVERY_ENTRY_CLEAR;
            spci->index[0] = index;
            rv = bcmptm_ser_cci_ser_req(unit, spci, sids_ptr_ctr, sids_ctr_num);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (sids_ptr_ctr != NULL) {
        sal_free(sids_ptr_ctr);
    }
    if (SHR_FUNC_ERR()) {
        return FALSE;
    }
    return TRUE;
}

/* Init data structure of SER correction. */
static void
bcmptm_ser_spci_init(int unit, bcmptm_ser_correct_info_t *spci)
{
    spci->flags = 0;
    spci->reported_sid = INVALIDm;
    spci->sid[1] = INVALIDm;
    spci->sid[0] = INVALIDm;
    spci->tbl_inst[0] = -1;
    spci->tbl_inst[1] = -1;
    spci->blk_type = -1;
    spci->addr[0] = -1;
    spci->addr[1] = -1;
    spci->index[0] = -1;
    spci->index[1] = -1;
    spci->resp_type = SER_RECOVERY_NO_OPERATION;
    spci->status_reg_sid = INVALIDr;
    spci->tbl_copy = -1;
}

/* Main routine of SERC thread. */
static void
bcmptm_serc_process(shr_thread_t tc, void *arg)
{
    int rv = SHR_E_NONE;
    bcmptm_serc_info_t *serc_info_ptr = (bcmptm_serc_info_t *)arg;
    int unit = serc_info_ptr->unit;
    bcmptm_ser_intr_msg_q_info_t ser_msg;
    bcmptm_ser_info_type_t ser_info_type;
    bcmptm_pre_serc_info_t pre_serc_info;
    bcmptm_ser_correct_info_t *spci;
    bool corrected = 1, only_one = 0, is_stopping = 0;

    spci = sal_alloc(sizeof(bcmptm_ser_correct_info_t), "bcmptmSerCorrectData");
    if (spci == NULL) {
        LOG_FATAL(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to allocate memory to"
                              " save ser correction data\n")));
        return;
    }
    while (1) {
        sal_memset(&ser_msg, 0, sizeof(ser_msg));

        if (bcmptm_ser_msg_count(unit) == 0) {
            (void)bcmptm_scor_done(unit);
        }
        rv = bcmptm_ser_msg_recv(unit, &ser_msg, SAL_MSGQ_FOREVER);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        /* Terminative event sent by bcmptm_ser_serc_stop */
        if (ser_msg.ser_intr_num == 0) {
            break;
        }
        rv = bcmptm_ser_intr_dispatch(unit, ser_msg.ser_intr_num,
                                      &ser_info_type, &pre_serc_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        only_one = 0;
        while (1) {
            bcmptm_ser_spci_init(unit, spci);
            rv = bcmptm_ser_fifo_pop_and_parse(unit, ser_info_type,
                                               &pre_serc_info, spci, &only_one);
            /* FIFO is empty */
            if (SHR_FAILURE(rv)) {
                break;
            }
            if (spci->flags & BCMPTM_SER_FLAG_HW_CORRECTED) {
                corrected = TRUE;
            } else {
                corrected = bcmptm_ser_req(unit, spci);
            }

            bcmptm_ser_log(unit, spci, corrected);

            bcmptm_ser_statistics(unit, spci);
            /*
             * For IP, there is no status register for FIFOm,
             * So SERC does not how many valid entries in FIFOm,
             * So only handle the first valid entry. The others will
             * be reported in next round.
             */
            if (only_one) {
                break;
            }
            /* Maybe need to exit */
            is_stopping = shr_thread_stopping(tc);
            if (is_stopping) {
                break;
            }
        }
        bcmptm_ser_intr_enable(unit, ser_msg.ser_intr_num, 1);

        is_stopping = shr_thread_stopping(tc);
        if (is_stopping) {
            break;
        }
    }
    if (spci != NULL) {
        sal_free(spci);
    }
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_serc_start(int unit)
{
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    bool enable = 0;
    int is_stopping = 0, stopped = 1;
    shr_thread_t thread_ctrl = serc_info[unit].serc_thread_ctrl;

    enable = cth_drv->ser_check_enable(unit);
    if (enable == 0) {
        return SHR_E_NONE;
    }
    if (thread_ctrl != NULL) {
        if (shr_thread_done(thread_ctrl)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "SERC thread has exited abnormally.\n")));
            serc_info[unit].serc_thread_ctrl = NULL;
            do {
                is_stopping = shr_thread_stop(thread_ctrl, 1000000);
                if (SHR_FAILURE(is_stopping)) {
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "Fail to cleaup thread ctrl info, [%d]!\n"),
                              is_stopping));
                }
            } while (SHR_FAILURE(is_stopping));
        } else {
            /* Active */
            stopped = 0;
        }
    }
    if (stopped == 1) {
        thread_ctrl = shr_thread_start("bcmptmSerCorr", -1,
                                       bcmptm_serc_process,
                                       (void *)(&serc_info[unit]));
        serc_info[unit].serc_thread_ctrl = thread_ctrl;
        if (thread_ctrl == NULL) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Fail to start SERC thread!\n")));
            return SHR_E_FAIL;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SERC thread is running.\n")));
    }
    /* Enable all SER interrupts */
    (void)bcmptm_ser_all_intr_enable(unit, 1);

    if (bcmptm_ser_tcam_scan_start(unit) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to start TCAM scan thread!\n")));
        /* Do not return ERROR */
    }
    if (bcmptm_ser_sram_scan_start(unit, 0) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to start SRAM scan thread!\n")));
        /* Do not return ERROR */
    }
    return SHR_E_NONE;
}

int
bcmptm_ser_serc_stop(int unit)
{
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    bool enable = 0;
    int is_stopping;
    shr_thread_t serc_thread = serc_info[unit].serc_thread_ctrl;

    enable = cth_drv->ser_check_enable(unit);
    if (enable == 0) {
        return SHR_E_NONE;
    }
    if (bcmptm_ser_tcam_scan_stop(unit) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to stop TCAM scan thread!\n")));
        /* Do not return ERROR */
    }
    if (bcmptm_ser_sram_scan_stop(unit) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to stop SRAM scan thread!\n")));
        /* Do not return ERROR */
    }
    (void)bcmptm_ser_all_intr_enable(unit, 0);

    if (serc_thread == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SERC thread has already been terminated\n")));
        return SHR_E_NONE;
    } else {
        serc_info[unit].serc_thread_ctrl = NULL;
        /*
         * If SER event is received by SERC thread before this terminative event,
         * the SER event will be handled by SERC thread.
         */
        bcmptm_ser_notify(unit, 0);
        /* Notify SERC thread to exit */
        do {
            is_stopping = shr_thread_stop(serc_thread, 1000000);
            if (SHR_FAILURE(is_stopping)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "SERC thread is exiting: %d!\n"),
                          is_stopping));
            }
        } while (SHR_FAILURE(is_stopping));

        return SHR_E_NONE;
    }
}

void
bcmptm_ser_alpm_info_cb_reg(int unit, bool mtop, bcmptm_ser_alpm_info_get_f alpm_info_get)
{
    if (mtop) {
        ser_alpm_mtop_info_get_f[unit] = alpm_info_get;
    } else {
        ser_alpm_info_get_f[unit] = alpm_info_get;
    }
}

int
bcmptm_ser_sid_remap(int unit, bcmdrd_sid_t sid, bcmdrd_sid_t *sid_map, int *index_map)
{
    bcmptm_ser_correct_info_t spci;
    int rv;

    sal_memset(&spci, 0, sizeof(spci));
    spci.sid[0] = sid;
    spci.index[0] = *index_map;

    rv = bcmptm_ser_correct_info_remap(unit, &spci);
    if (SHR_SUCCESS(rv)) {
        *sid_map = spci.sid[0];
        *index_map = spci.index[0];
    } else {
        *sid_map = sid;
    }
    return rv;
}

void
bcmptm_ser_parity_field_clear(int unit, bcmdrd_sid_t sid,
                              uint32_t *entry_data)
{
    const bcmdrd_symbol_t *symbol = NULL;
    bcmdrd_sym_field_info_t finfo;
    uint32_t fval = 0;
    const bcmdrd_symbols_t *symbols = NULL;

    /* Heuristic to clear parity/ecc bits */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_A");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_B");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_LOWER");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_UPPER");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP5");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP6");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP7");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PBM_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EVEN_EGR_VLAN_STG_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P5");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P6");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_P7");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P5");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P6");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_P7");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P5");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P6");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_P7");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_PBM_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_PBM_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_PBM_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_PBM_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_PBM_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_PBM_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_PBM_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_PBM_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_PBM_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_PBM_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_PBM_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_PBM_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_KEY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_MASK");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_MASK_UPR");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_MASK_LWR");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_KEY_UPR");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_KEY_LWR");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_KEY1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_KEY0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_MASK1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "TCAM_PARITY_MASK0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_ECC0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_ECC1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_ECCP0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_ECCP1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_PARITY0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_PARITY1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_TCAM_PARITY_MASK");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_TCAM_PARITY_KEY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "UPR_TCAM_PARITY_MASK");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "UPR_TCAM_PARITY_KEY");

    /* Clear hit bits */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_HIT0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LWR_HIT1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITDA_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITDA_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITDA_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITDA_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITSA_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITSA_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITSA_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITSA_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITDA");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HITSA");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "B0_HIT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "B1_HIT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "L3_HIT_DCM");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "L3_HIT_PM");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "HIT_BITS");
    /* Clear hardware-modified fields in meter tables  */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "BUCKETCOUNT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "REFRESHCOUNT");

    /* Clear parity/ecc bits on svm_meter_table */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_EXCESS");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_EXCESS");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_EXCESS");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_COMMITTED");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_COMMITTED");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_COMMITTED");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_LRN");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_LRN");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_LRN");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECCP_CONFIG");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "PARITY_CONFIG");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "ECC_CONFIG");
    /* Clear hardware-modified fields in svm_meter_table */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "LRN");         /* LAST_REFRESH_NUMBERf */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "COMMITTED");   /* COMMITTED_BUCKETCOUNTf */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, "EXCESS");      /* EXCESS_BUCKETCOUNTf */

}

