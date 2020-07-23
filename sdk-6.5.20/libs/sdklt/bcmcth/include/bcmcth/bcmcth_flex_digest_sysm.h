/*! \file bcmcth_flex_digest_sysm.h
 *
 * Header file of BCMCTH Flex Digest interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_FLEX_DIGEST_SYSM_H
#define BCMCTH_FLEX_DIGEST_SYSM_H

/*!
 * \brief Start Flex Digest component.
 *
 * Register Flex Digest component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmcth_flex_digest_start(void);

#endif /* BCMCTH_FLEX_DIGEST_SYSM_H */
