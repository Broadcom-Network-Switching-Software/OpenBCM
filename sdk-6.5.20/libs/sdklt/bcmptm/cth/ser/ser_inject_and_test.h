/*! \file ser_inject_and_test.h
 *
 * Interface functions for SER inject and test
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_INJECT_AND_TEST_H
#define SER_INJECT_AND_TEST_H


#include <bcmdrd/bcmdrd_types.h>
#include <sal/sal_time.h>

/* Inject SER error to key and mask field of TCAM */
#define BCMPTM_SER_INJECT_ERROR_KEY_MASK  (1 << 0)

/* Inject 2bits SER error memory tables */
#define BCMPTM_SER_INJECT_ERROR_2BIT      (1 << 1)

/* Inject SER error to XOR banks */
#define BCMPTM_SER_INJECT_ERROR_XOR_BANK  (2 << 1)

/*!
 * \brief Inject SER error to a TCAM.
 * TCAMs are protected by CMIC SER engine.
 *
 * \param [in] unit          Unit number
 * \param [in] pt_id_mapped  TCAM table SID, view protected by H/W engine.
 * \param [in] tbl_inst      Table instance
 * \param [in] phy_idx       Physical index of TCAM table
 * \param [in] flags         Flags of TCAM table
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_tcam_ser_inject(int unit, bcmdrd_sid_t pt_id_mapped,
                           int tbl_inst, int phy_idx, uint32_t flags);
/*!
 * \brief Inject SER error to a SRAM.
 *
 *
 * \param [in] unit         Unit number
 * \param [in] sid          SRAM or registers SID
 * \param [in] tbl_inst     Table instance
 * \param [in] tbl_copy     For some duplicate PTs,
 *                          use tbl_inst & tbl_copy to access appointed instance.
 * \param [in] fv_idx       Functioanl index
 * \param [in] flags        Flags of SRAM
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_sram_ser_inject(int unit, bcmdrd_sid_t sid, int tbl_inst,
                           int tbl_copy, int fv_idx, uint32_t flags);

/*!
 * \brief  Inject SER error to an XOR bank of SRAM.
 *
 *
 * \param [in] unit         Unit number
 * \param [in] fv_sid       SRAM SID
 * \param [in] tbl_inst     Table instance
 * \param [in] fv_idx       Functional index
 * \param [in] flags        Flags of SRAM
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_xor_bank_ser_inject(int unit, bcmdrd_sid_t fv_sid,
                               int tbl_inst, int fv_idx, uint32_t flags);

/*!
 * \brief Validate SER error correction functionality.
 *
 * Validate that an SER error can be successfully injected and
 * corrected on a given memory location.
 *
 * \param [in] unit      Unit number.
 * \param [in] sid       Physical table ID.
 * \param [in] tbl_inst  Table instance.
 * \param [in] tbl_copy  Table copy number (base index).
 * \param [in] phy_idx   Physical table index.
 * \param [in] usleep    Microseconds to wait for error correction.
 * \param [in] flag      BCMPTM_SER_INJECT_ERROR_2BIT: validate 2bit SER error
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_err_injection_validate(int unit, bcmdrd_sid_t sid,
                                  int tbl_inst, int tbl_copy, int phy_idx,
                                  sal_time_t usleep, uint32_t flags);

#endif /* SER_INJECT_AND_TEST_H */
