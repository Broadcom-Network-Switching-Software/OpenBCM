/*! \file bcma_testutil_ser.h
 *
 * SER utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_SER_H
#define BCMA_TESTUTIL_SER_H

#include <shr/shr_types.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_arg.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief No PT name is inputted when user runs TR144.
 */
#define PT_NAME_NONE          "PT_NAME_NONE"

/*!
 * \brief Parameters for SER test.
 */
typedef struct bcma_testutil_ser_param_s {
    /*! Memory or Register name. */
    const char *pt_id_name;

    /*! Instance. */
    int inst;

    /*! If 'inst' is used as depth of PT(EGR_1588_LINK_DELAY_64r), adopt 'copy' as instance. */
    int copy;

    /*! Single, Spread. */
    char *index_type;

    /*! Index for 'Single index_type'. */
    int index;

    /*! Inject SER error only or inject and validate. */
    int inject_only;

    /*! Inject error bit number(1bit or 2bit). */
    int err_bit_num;

    /*! Inject SER error to XOR bank. */
    int xor_bank;

    /*! Time to wait. */
    int wait_time;

    /*! Test status of control registers. */
    int test_ctrl_reg;

    /*! Test SER function from this PT. */
    bcmdrd_sid_t ssid;

    /*! Test PT number. */
    int test_pt_num;

    /*! Test only one instance for a PT. */
    int only_one_inst;

    /*! Test all PTs even which recovery type is none. */
    int all_resp_type;

    /*! SER info such as inst, index etc. is appointed randomly. */
    int rand_gen_info;

    /*! Test 1bit error for ECC protecting PTs. */
    int single_bit_test;

    /*!
     * Write data to H/W and PTcache firstly, after SER injection test,
     * need to check whether data recovered by SERC is right or not.
     */
    int check_entry_data;
} bcma_testutil_ser_param_t;

/*!
 * \brief Parse inputting parameters for SER test case.
 *
 * \param [in] unit           Logical device id
 * \param [in] cli             CLI object.
 * \param [in] a               Argument list.
 * \param [out] ser_param Ser parameters.
 * \param [in] show          Print CLI parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_param_parse(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                              bcma_testutil_ser_param_t *ser_param, bool show);
/*!
 * \brief Inject SER error to a PT, after that validate whether SER error is corrected or not.
 *
 * \param [in] unit           Logical device id
 * \param [in] ser_param  SER parameters.
 * \param [out] num_entry Number of tested entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_inject_validate(int unit, bcma_testutil_ser_param_t *ser_param, int *num_entry);

/*!
 * \brief Test SER function of all PTs.
 *
 * \param [in] unit           Logical device id
 * \param [in] ser_param  Ser parameters.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_all_pts_test(int unit, bcma_testutil_ser_param_t *ser_param);

/*!
 * \brief Enable/disable SER memory scan threads.
 *
 * \param [in] unit           Logical device id
 * \param [in] enable       1: start memory scan thread.
 * \param [in] resume       1: use default parameters to configure scan thread.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_mem_scan_enable(int unit, int enable, int resume);

/*!
 * \brief Provide instructions of SER test cases.
 *
 * \param [in] unit           Logical device id
 *
 * \return None.
 */
extern void
bcma_testutil_ser_print_help(int unit);

/*!
 * \brief To validate whether SER parity control registers are enabled successfully.
 *
 * \param [in] unit           Logical device id
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_parity_ctrl_check(int unit);

/*!
 * \brief To Disable or revert counter polling.
 *
 * \param [in] unit           Logical device id
 * \param [in] disable       1: disable counter polling, 0: revert counter polling
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_counter_poll_config(int unit, bool disable);

/*!
 * \brief To check whether SER feature is enabled or not.
 *
 * \param [in] unit           Logical device id
 *
 * \return TRUE or FALSE.
 */
extern bool
bcma_testutil_ser_enabled(int unit);

/*!
 * \brief Get SER information through LT SER_PT_STATUS.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_name PT name.
 * \param [in] single_bit 1: Get recovery type of single bit error.
 * \param [out] index_num Depth of PT.
 * \param [out] inst_num Instance number of PT.
 * \param [out] copy_num Copy number of PT.
 * \param [out] check_type SER check type of PT.
 * \param [out] recovery_type SER recovery type of PT.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_pt_status_get(int unit, const char *pt_name,
                                bool single_bit, int *index_num,
                                int *inst_num, int *copy_num, int *check_type,
                                int *recovery_type);

/*!
 * \brief To enable or disable high severity fault mechanism.
 *
 * \param [in] unit Logical device id
 * \param [in] enable 1: enable, 0: disable.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ctrl_hsf_machine_enable(int unit, bool enable);

/*!
 * \brief Specify the time.
 *
 * Within the time, the same SER events will be squashed.
 *
 * \param [in] unit Logical device id
 * \param [in] time Time(ms).
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_ser_ctrl_squash_config(int unit, uint64_t time);

#endif /* BCMA_TESTUTIL_SER_H */
