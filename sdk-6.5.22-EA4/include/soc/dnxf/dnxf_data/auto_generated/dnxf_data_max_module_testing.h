/** \file dnxf_data_max_module_testing.h
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
#ifndef _DNXF_DATA_MAX_MODULE_TESTING_H_
/*{*/
#define _DNXF_DATA_MAX_MODULE_TESTING_H_
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
#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MAX DEFINES:
 * {
 */
/**
 * \brief
 * supported define expected to return value
 */
#define DNXF_DATA_MAX_MODULE_TESTING_UNSUPPORTED_SUPPORTED_DEF (305419896)

/**
 * \brief
 * unsupported feature expected to return NULL
 */
#define DNXF_DATA_MAX_MODULE_TESTING_UNSUPPORTED_UNSUPPORTED_DEF (0)

/*
 * }
 */

/*}*/
#endif /*_DNXF_DATA_MAX_MODULE_TESTING_H_*/
/* *INDENT-ON* */
