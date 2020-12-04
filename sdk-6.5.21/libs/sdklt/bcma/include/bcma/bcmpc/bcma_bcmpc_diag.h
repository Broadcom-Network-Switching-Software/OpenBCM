/*! \file bcma_bcmpc_diag.h
 *
 * BCMPC diagnostics functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPC_DIAG_H
#define BCMA_BCMPC_DIAG_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Initialize prbs_stat_cb_t structure.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_init(int unit);

/*!
 * \brief Clean up prbs_stat_cb_t structure.
 *
 * If \c unit is negative, all resources of devices will be cleaned up.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_cleanup(int unit);

/*!
 * \brief Display current prbs stat configuration.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_status(int unit);

/*!
 * \brief Start prbs stat collecting.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 * \param [in] time Interval time to collect prbs stat.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_start(int unit, bcmdrd_pbmp_t *pbmp, uint32_t time);

/*!
 * \brief Stop prbs stat collecting.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_stop(int unit);

/*!
 * \brief Display prbs stat error counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_counter(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Display prbs stat ber.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_ber(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Clean up prbs stat counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_prbsstat_clear(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Initialize fec_stat_cb_t structure.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_init(int unit);

/*!
 * \brief Clean up fec_stat_cb_t structure.
 *
 * If \c unit is negative, all resources of devices will be cleaned up.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_cleanup(int unit);

/*!
 * \brief Display current fec stat configuration.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_status(int unit);

/*!
 * \brief Start fec stat collecting.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 * \param [in] time Interval time to collect prbs stat.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_start(int unit, bcmdrd_pbmp_t *pbmp, uint32_t time);

/*!
 * \brief Stop fec stat collecting.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_stop(int unit);

/*!
 * \brief Display fec stat error counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_counter(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Display fec stat ber.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_ber(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Clean up FEC stat counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port map.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_diag_fecstat_clear(int unit, bcmdrd_pbmp_t *pbmp);

#endif /* BCMA_BCMPC_DIAG_H */
