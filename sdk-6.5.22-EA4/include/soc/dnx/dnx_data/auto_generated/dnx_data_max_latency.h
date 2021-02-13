/** \file dnx_data_max_latency.h
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
#ifndef _DNX_DATA_MAX_LATENCY_H_
/*{*/
#define _DNX_DATA_MAX_LATENCY_H_
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
 * Number of egress (end-to-end) latency profiles
 */
#define DNX_DATA_MAX_LATENCY_PROFILE_EGRESS_NOF (16)

/**
 * \brief
 * Number of ingress latency profiles
 */
#define DNX_DATA_MAX_LATENCY_PROFILE_INGRESS_NOF (32)

/**
 * \brief
 * Number of AQM profiles
 */
#define DNX_DATA_MAX_LATENCY_AQM_PROFILES_NOF (16)

/**
 * \brief
 * Number AQM flow IDs
 */
#define DNX_DATA_MAX_LATENCY_AQM_FLOWS_NOF (256)

/**
 * \brief
 * The time value taken from the OAM TS header is shifted left by this amount of bits
 */
#define DNX_DATA_MAX_LATENCY_INGRESS_JR_MODE_LATENCY_TIMESTAMP_LEFT_SHIFT (5)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_LATENCY_H_*/
/* *INDENT-ON* */
