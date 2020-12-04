/*! \file ser_correct.h
 *
 * Interface functions for SER correction
 *
 * This file contains implementation of SER correction
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_CORRECT_H
#define SER_CORRECT_H

#include <bcmdrd/bcmdrd_types.h>

#include <bcmptm/bcmptm_ser_chip_internal.h>

/*!
 * \brief SER correction information read from SER_RESULT_0m or SER_RESULT_1m.
 */
typedef struct bcmptm_ser_tcam_correct_info_s {
    /*! value of field RANGEf */
    uint32_t hw_idx;
    /*! value of field SBUS_ADDRf */
    uint32_t sbus_addr;
    /*! value of field ACC_TYPEf */
    uint32_t acc_type;
} bcmptm_ser_tcam_crt_info_t;

/*!
 * \brief SER correction information read from SER FIFOm of MMU.
 */
typedef struct bcmptm_ser_mmu_correct_info_s {
    /*! value of field RANGEf, multiple bits have error */
    uint32_t multi_bits;
    /*! value of field SBUS_ADDRf,  "1bit" or "parity/2bit" */
    uint32_t err_type;
    /*! value of field ACC_TYPEf */
    uint32_t eaddr;
    /*! block type of mem reporting SER error */
    int      blk_type;
    /*! instance of block XPE, SC etc. */
    int      blk_inst;
} bcmptm_ser_mmu_crt_info_t;

/*!
 * \brief SER correction information of buffer or bus.
 */
typedef struct bcmptm_ser_misc_blk_correct_info_s {
    /*! Multiple bits have error */
    int             multi_bits;
    /*! entry index*/
    int             entry_idx;
    /*! 2bit ECC error */
    int             err_type;
    /*! block type of mem reporting SER error */
    int             blk_type;
    /*! instance of memory table */
    int             tbl_inst;
    /*!
     * SID of status register for buffer or bus, such as: IDB_IS_TDM_CAL_ECC_STATUSr.
     * According to status register, SERC can know which memory needs to be corrected or cleared.
     */
    bcmdrd_sid_t    status_reg;
    /*! Software ID of buffer or bus, created by "BCMPTM_SER_BB_SID_MAGIC_CREATE" */
    bcmdrd_sid_t    magic_sid;
} bcmptm_ser_misc_blk_crt_info_t;

/*!
 * \brief SER correction information read from SER FIFOs of IP/EP.
 */
typedef struct bcmptm_ser_ing_egr_correct_info_s {
    /*! value of field NON_SBUSf, 0: sbus accessabe, 1: non-sbus accessable*/
    uint32_t non_sbus;
    /*! value of field DROPf, 1: SER caused packet drop */
    uint32_t drop;
    /*! value of field ECC_PARITYf, ecc or parity SER error */
    uint32_t ecc_parity;
    /*! value of field MEM_TYPEf,  1: reg, 0: mem */
    uint32_t reg;
    /*! value of field ADDRESSf */
    uint32_t address;
    /*! value of field INSTRUCTION_TYPEf */
    uint32_t instruction_type;
    /*! value of field MULTIPLEf */
    uint32_t multi;
    /*! value of field MEMINDEXf or REGINDEXf */
    uint32_t index;
    /*! block type of mem reporting SER error */
    int      blk_type;
    /*! ING EGR pipe number */
    int      blk_inst;
    /*! stage of pipe */
    int      pipe_stage;
    /*! memory base, used to find buffer or bus */
    int      reg_mem_base;
    /*! indicate error is recovered by HW */
    int      hw_correction;
} bcmptm_ser_ing_egr_crt_info_t;

/*!
 * \brief SER correction information read from SER FIFOs.
 */
typedef union bcmptm_ser_crt_info_s {
    bcmptm_ser_tcam_crt_info_t     ser_engine_tcam;
    bcmptm_ser_mmu_crt_info_t      ser_mmu_mem;
    bcmptm_ser_misc_blk_crt_info_t ser_misc_mem;
    bcmptm_ser_ing_egr_crt_info_t  ser_ing_egr_mem;
} bcmptm_ser_raw_crt_info_t;


typedef struct bcmptm_ser_raw_info_s {
    /* valid : 0, information in "serc_info" is invalid */
    uint32_t                   valid;
    /*
     * If there is no FIFO status register for IPIPE/EPIPE,
     * pop only one entry from FIFOm for every SER message.
     * If there are still other SER events in FIFOm, the
     * SER message will be reported from H/W again.
     */
    bool                       non_fifo_status_reg;
    /* Raw SER correction information */
    bcmptm_ser_raw_crt_info_t  serc_info;
} bcmptm_ser_raw_info_t;

/*!
 * \brief Clear fields which value can be updated by H/W.
 *
 * \param [in] unit Unit number.
 * \param [in] sid SID.
 * \param [in/out] entry_data Entry data.
 *
 * \retval NONE
 */
extern void
bcmptm_ser_parity_field_clear(int unit, bcmdrd_sid_t sid, uint32_t *entry_data);

#endif /* SER_CORRECT_H */
