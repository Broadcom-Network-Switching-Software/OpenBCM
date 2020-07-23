/*! \file dev_misc.h
 *
 * Device misc configuation header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_DEV_MISC_H
#define BCMI_LTSW_DEV_MISC_H

/*!
 * \brief Clean up the device misc configurations.
 *
 * This function is used to clean up the devcie misc configuations.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_dev_misc_cleanup(int unit);


/*!
 * \brief Initialize the device misc behaviors.
 *
 * \param [in]    unit     Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_dev_misc_init(int unit);

#endif /* BCMI_LTSW_DEV_MISC_H */
