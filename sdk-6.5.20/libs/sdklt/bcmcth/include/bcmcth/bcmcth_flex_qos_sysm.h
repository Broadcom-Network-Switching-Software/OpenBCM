/*! \file bcmcth_flex_qos_sysm.h
 *
 * Header file of BCMCTH Flex QoS interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_FLEX_QOS_SYSM_H
#define BCMCTH_FLEX_QOS_SYSM_H

/*!
 * \brief Start Flex QoS component.
 *
 * Register Flex QoS component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmcth_flex_qos_start(void);

#endif /* BCMCTH_FLEX_QOS_SYSM_H */
