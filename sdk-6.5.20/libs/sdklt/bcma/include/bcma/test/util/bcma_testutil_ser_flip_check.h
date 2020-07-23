/*! \file bcma_testutil_ser_flip_check.h
 *
 * SER utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_SER_FLIP_CHECK_H
#define BCMA_TESTUTIL_SER_FLIP_CHECK_H

#include <shr/shr_types.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_arg.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Parameters for SER in house test.
 */
typedef struct bcma_testutil_ser_ih_param_s {
    /*! Test data pattern */
    int    ser_test_pattern;

    /*! Read or write mode */
    int    ser_rd_wr_mode;

    /*! Mute debug message */
    int    ser_quiet_mode;

    /*! Number of written memory */
    uint32_t num_mem_written;

    /*! Number of read memory */
    uint32_t num_mem_read;

    /*! Number of tested bit */
    uint32_t num_bit_tested;

    /*! Number of flip bit */
    uint32_t num_bit_flip;

    /*! Number of check fail */
    uint32_t num_mem_check_fail;

    /*! Number of read fail */
    uint32_t num_mem_read_fail;

    /*! Mask array of all memories */
    uint32_t **mask_array;
} bcma_testutil_ser_ih_param_t;

/*!
 * \brief Parse inputting parameters for SER in house test case.
 *
 * \param [in] unit           Logical device id.
 * \param [in] cli             CLI object.
 * \param [in] a               Argument list.
 * \param [out] ser_param SER test case parameters.
 * \param [in] show          Print CLI parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ih_param_parse(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                                 bcma_testutil_ser_ih_param_t *ser_param, bool show);

/*!
 * \brief Provide instructions of SER in house test case.
 *
 * \param [in] unit           Logical device id.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern void
bcma_testutil_ser_ih_print_help(int unit);

/*!
 * \brief Init parameters for SER in house test case.
 *
 * \param [in] unit                Logical device id.
 * \param [out] ser_ih_param SER test case parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ih_init(int unit, bcma_testutil_ser_ih_param_t *ser_ih_param);

/*!
 * \brief Cleanup memory space allocated during SER in house test case.
 *
 * \param [in] unit               Logical device id.
 * \param [in] ser_ih_param  SER test case parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ih_cleanup(int unit, bcma_testutil_ser_ih_param_t *ser_ih_param);

/*!
 * \brief Write all PTs with parameters configured by user.
 *
 * \param [in] unit        Logical device id.
 * \param [in] ser_ih_p  SER test case parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ih_write(int unit, bcma_testutil_ser_ih_param_t *ser_ih_p);

/*!
 * \brief Read and check all PTs with parameters configured by user.
 *
 * \param [in] unit        Logical device id.
 * \param [in] ser_ih_p  SER test case parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ih_read_check(int unit, bcma_testutil_ser_ih_param_t *ser_ih_p);

/*!
 * \brief Get entry data.
 *
 * All bits should be 1 except H/W refreshed fields like 'ECC/Parity/Hit' fields.
 *
 * \param [in] unit Logical device id.
 * \param [in] sid  SER test case parameters.
 * \param [out] entry_data Entry data.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ih_mem_datamask_get(int unit, bcmdrd_sid_t sid,
                                      uint32_t *entry_data);

#endif /* BCMA_TESTUTIL_SER_FLIP_CHECK_H */
