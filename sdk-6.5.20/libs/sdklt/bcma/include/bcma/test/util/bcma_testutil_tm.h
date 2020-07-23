/*! \file bcma_testutil_tm.h
 *
 * TM test utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_TM_H
#define BCMA_TESTUTIL_TM_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Check health info object.
 *
 * Structure containing information to be checked in check health util.
 * Contains sid, fid and expected value.
 */
typedef struct check_health_info_s
{
    /*! Symbol ID */
    bcmdrd_sid_t sid;
    /*! Field ID */
    bcmdrd_fid_t fid;
    /*! Expected Value */
    uint32_t exp_value[2];
} check_health_info_t;

/*!
 * \brief Compare PT field value against expected value with mask.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 * \param [in] index Index of sid.
 * \param [in] inst Instance of sid.
 * \param [in] wsize Word size.
 * \param [in] *exp_value Expected value data words array.
 *
 * \retval 0 if expected value matches actual, Fail count if unmatched.
 */
extern int
bcma_testustil_tm_comp_pt_field_value(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t
        fid, int index, int inst, int wsize, uint32_t *exp_value);

/*!
 * \brief Clear stats of all Port/Queue based TM drop counters.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 *
 * \retval SHR_E_NONE No error.
 */

extern int
bcma_testutil_tm_stat_clear(int unit, int port);

/*!
 * \brief Clear stats of Global drop counters.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_global_drop_stat_clear(int unit);

/*!
 * \brief Retrieve OBM packet drop stats for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] value Packet drop count.
 *
 * \retval SHR_E_NONE No error.
 */

extern int
bcma_testutil_tm_obm_pkt_drop_get(int unit, int port, uint64_t *value);

/*!
 * \brief Retrieve MMU ING packet drop stats for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] value Packet drop count.
 *
 * \retval SHR_E_NONE No error.
 */

extern int
bcma_testutil_tm_ing_pkt_drop_get(int unit, int port, uint64_t *value);

/*!
 * \brief Retrieve MMU UC queue drop stats for a port, queue.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] queue Queue Id.
 * \param [out] value Packet drop count.
 *
 * \retval SHR_E_NONE No error.
 */

extern int
bcma_testutil_tm_egr_uc_q_pkt_drop_get(int unit, int port, int queue,
    uint64_t *value);
/*!
 * \brief Retrieve MMU MC queue drop stats for a port, queue.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] queue Queue Id.
 * \param [out] value Packet drop count.
 *
 * \retval SHR_E_NONE No error.
 */

extern int
bcma_testutil_tm_egr_mc_q_pkt_drop_get(int unit, int port, int queue,
    uint64_t *value);

/*!
 * \brief Retrieve MMU total queue packet drop stats for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] value Packet drop count.
 *
 * \retval SHR_E_NONE No error.
 */

extern int
bcma_testutil_tm_total_queue_pkt_drop_get(int unit, int port, uint64_t *value);

/*!
 * \brief Retrieve MMU total queue packet drop stats for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Test port bitmap.
 * \param [in] length Packet size in bytes.
 * \param [out] cnt Number of packets to be injected.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_num_inject_pkst_line_rate_get(int unit, bcmdrd_pbmp_t pbmp,
    int length, uint32_t *cnt);

/*!
 * \brief Check for global drop packet counts in MMU.
 *
 * \param [in] unit Unit number.
 * \param [out] test_result Test result status returned.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_global_drop_count_check(int unit, bool *test_result);

/*!
 * \brief Clear stats for TM Global drop counter LTs.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_global_drop_stat_clear(int unit);

/*!
 * \brief Apply port metering for a percentage of port speed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] meter_perc Percentage of port rate for metering.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_port_meter(int unit, int port, int meter_perc);

/*!
 * \brief Cleanup port metering on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_port_meter_cleanup(int unit, int port);

/*!
 * \brief Setup cos map strength profile.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int(*bcma_testutil_drv_cos_map_strength_profile_set_f)(int unit);

/*!
 * \brief Check if MMu is in lossless mode.
 *
 * \param [in] unit Unit number.
 * \param [out] lossless_mode If MMU lossless mode is true.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_tm_mmu_thd_mode_is_lossless(int unit, bool *lossless_mode);

/*!
 * \brief Check the credit loops between Port->EP and EP->MMU.
 *
 * \param [in] unit Unit number.
 * \param [out] err_count Number of errors.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmtm_util_check_port_ep_mmu_credit_loop(int unit, int *err_count);
#endif /* BCMA_TESTUTIL_TM_H */
