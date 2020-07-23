/*! \file issumgmt.h
 *
 * ISSU management definition for LTSW.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_LTSW_ISSUMGMT_H
#define DIAG_LTSW_ISSUMGMT_H

/*!
 * \brief Configure the start SDK version for ISSU upgrade.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
issumgmt_start_version_set(const char *start_ver);

/*!
 * \brief Configure the current SDK version for ISSU upgrade.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
issumgmt_current_version_set(const char *current_ver);

/*!
 * \brief Start the ISSU upgrade process.
 *
 * This management function is used to notify the SDK and SDKLT that
 * the ISSU process is starting now.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
issumgmt_upgrade_start(void);

/*!
 * \brief The ISSU upgrade process done.
 *
 * This management function is used to notify the SDK and SDKLT that
 * the ISSU process is done.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
issumgmt_upgrade_done(void);

#endif /* DIAG_LTSW_ISSUMGMT_H */
