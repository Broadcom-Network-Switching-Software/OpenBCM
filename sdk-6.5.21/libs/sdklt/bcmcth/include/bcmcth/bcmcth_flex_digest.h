/*! \file bcmcth_flex_digest.h
 *
 * BCMCTH Flex Digest top-level APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_FLEX_DIGEST_H
#define BCMCTH_FLEX_DIGEST_H

/*!
 * \brief Attach Flex Digest device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_flex_digest_attach(int unit, bool warm);

/*!
 * \brief Clean up Flex Digest device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_flex_digest_detach(int unit);

#endif /* BCMCTH_FLEX_DIGEST_H */
