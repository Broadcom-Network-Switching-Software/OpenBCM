/*! \file bcma_testcase_pkt_internal.h
 *
 * Packet I/O test framework functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PKT_INTERNAL_H
#define BCMA_TESTCASE_PKT_INTERNAL_H

#include <sal/sal_types.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/test/util/bcma_testutil_port.h>

/*!
 * \brief Packet I/O test select function.
 *
 * \param [in] unit Unit number.
 *
 * \return true if test case is selected on this device, otherwise false.
 */
extern int
bcma_testcase_pkt_select(int unit);

/*!
 * \brief Cleanup packet I/O device procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pkt_dev_cleanup_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Init packet I/O device procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pkt_dev_init_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Map queues to a RX channel procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pkt_chan_qmap_set_cb(int unit, void *bp, uint32_t option);

#endif /* BCMA_TESTCASE_PKT_INTERNAL_H */
