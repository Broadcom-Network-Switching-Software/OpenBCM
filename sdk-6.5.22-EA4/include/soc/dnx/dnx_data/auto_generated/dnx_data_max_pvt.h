/** \file dnx_data_max_pvt.h
 * 
 * MODULE DATA MAX - 
 * Maintain max value (of all devices) per define
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MAX_PVT_H_
/*{*/
#define _DNX_DATA_MAX_PVT_H_
/*
 * INCLUDE FILES:
 * {
 */
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MAX DEFINES:
 * {
 */
/**
 * \brief
 * Number of device PVT monitors
 */
#define DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS (9)

/**
 * \brief
 * PVT base. Used for correct calculation of the monitor readings
 */
#define DNX_DATA_MAX_PVT_GENERAL_PVT_BASE (43410000)

/**
 * \brief
 * PVT factor Used for correct calculation of the monitor readings
 */
#define DNX_DATA_MAX_PVT_GENERAL_PVT_FACTOR (53504)

/**
 * \brief
 * Number of bits in the field PVT_MON_CONTROL_SEL
 */
#define DNX_DATA_MAX_PVT_GENERAL_PVT_MON_CONTROL_SEL_NOF_BITS (5)

/**
 * \brief
 * Number of bits in the field THERMAL_DATA
 */
#define DNX_DATA_MAX_PVT_GENERAL_THERMAL_DATA_NOF_BITS (11)

/**
 * \brief
 * select voltage value
 */
#define DNX_DATA_MAX_PVT_GENERAL_VOL_SEL (10)

/**
 * \brief
 * voltage calculation base value
 */
#define DNX_DATA_MAX_PVT_GENERAL_VOL_FACTOR (1020)

/**
 * \brief
 * voltage calculation multiple value
 */
#define DNX_DATA_MAX_PVT_GENERAL_VOL_MULTIPLE (5)

/**
 * \brief
 * voltage calculation divisor
 */
#define DNX_DATA_MAX_PVT_GENERAL_VOL_DIVISOR (4096)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_PVT_H_*/
/* *INDENT-ON* */
