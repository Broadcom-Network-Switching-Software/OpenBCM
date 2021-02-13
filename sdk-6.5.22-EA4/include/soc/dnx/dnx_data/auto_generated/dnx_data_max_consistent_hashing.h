/** \file dnx_data_max_consistent_hashing.h
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
#ifndef _DNX_DATA_MAX_CONSISTENT_HASHING_H_
/*{*/
#define _DNX_DATA_MAX_CONSISTENT_HASHING_H_
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
 * the maximal number of entries in the consistent hashing calendar
 */
#define DNX_DATA_MAX_CONSISTENT_HASHING_CALENDAR_MAX_NOF_ENTRIES_IN_CALENDAR (512)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_CONSISTENT_HASHING_H_*/
/* *INDENT-ON* */
