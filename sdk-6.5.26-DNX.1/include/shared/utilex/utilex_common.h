/** \file utilex_common.h
 *  
 * 
 * All utilities common to all the various utilex services. 
 *  
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_COMMON_H_INCLUDED
/** { */
#define UTILEX_COMMON_H_INCLUDED

/*************
* INCLUDES  *
*************/
/** { */

#include <soc/sand/shrextend/shrextend_debug.h>
/** } */
/*************
* DEFINES   *
*************/
/** { */
/**
 * \brief
 * Verify specified unit has a legal value. If not, software goes to \n
 * exit with error code.
 * 
 * Notes: \n
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_VERIFY_UNIT_IS_LEGAL(_unit) \
    if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
    { \
        /* \
         * If this is an illegal unit identifier, quit \
         * with error. \
         */ \
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL) ; \
        SHR_EXIT() ; \
    }
/*
 * Define the various 'packed' attributes.
 * {
 */
#ifdef _MSC_VER
/* { */
/*
 * On MS-Windows platform this attribute is not defined.
 */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__
/* } */
#endif

#pragma warning(disable  : 4103)
#pragma warning(disable  : 4127)
#pragma pack(push)
#pragma pack(1)
/* } */
#elif defined(__GNUC__)
/* { */
/*
 * GNUC packing attribute.
 */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__ __attribute__ ((packed))
/* } */
#endif
/* } */
#elif defined(GHS)
/* { */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__ __attribute__ ((__packed__))
/* } */
#endif
/* } */
#else
/* { */
/*
 * Here add any other definition (custom)
 */
#define __ATTRIBUTE_PACKED__
/* } */
#endif

/*
 * }
 */

/** } */
/*************
* TYPEDEFS   *
*************/
/** { */
/** } */

/*************
* PROTOTYPES *
*************/
/** { */
/** } */

/** } */
/** UTILEX_COMMON_H_INCLUDED  */
#endif
