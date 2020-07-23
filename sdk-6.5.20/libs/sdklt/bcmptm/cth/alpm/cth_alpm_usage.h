/*! \file cth_alpm_usage.h
 *
 * CTH_ALPM usage specific info
 *
 * This file contains usage specific info which are internal to CTH_ALPM CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CTH_ALPM_USAGE_H
#define CTH_ALPM_USAGE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>

/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief ALPM STATS KEY types
 */
typedef enum {
    KEY_TYPE_IPV4                   = 0,    /*!< IPv4 */
    KEY_TYPE_IPV4_VRF               = 1,    /*!< IPv4 vrf */
    KEY_TYPE_IPV4_OVERRIDE          = 2,    /*!< IPv4 override */
    KEY_TYPE_IPV6_SINGLE            = 3,    /*!< Ipv6 single wide */
    KEY_TYPE_IPV6_SINGLE_VRF        = 4,    /*!< Ipv6 vrf single wide */
    KEY_TYPE_IPV6_SINGLE_OVERRIDE   = 5,    /*!< Ipv6 override */
    KEY_TYPE_IPV6_DOUBLE            = 6,    /*!< Ipv6 double wide */
    KEY_TYPE_IPV6_DOUBLE_VRF        = 7,    /*!< Ipv6 vrf double wide */
    KEY_TYPE_IPV6_DOUBLE_OVERRIDE   = 8,    /*!< Ipv6 override double wide */
    KEY_TYPE_IPV6_QUAD              = 9,   /*!< Ipv6 quad wide */
    KEY_TYPE_IPV6_QUAD_VRF          = 10,   /*!< Ipv6 vrf quad wide */
    KEY_TYPE_IPV6_QUAD_OVERRIDE     = 11,   /*!< Ipv6 override quad wide */
    KEY_TYPE_COMP_V4                = 12,   /*!< Ipv4 compression */
    KEY_TYPE_COMP_V6                = 13,   /*!< Ipv6 compression */
    KEY_TYPE_L3MC_V4                = 14,   /*!< Ipv4 multicast */
    KEY_TYPE_L3MC_V6                = 15,   /*!< Ipv6 multicast */
} alpm_usage_key_type_t;

/*******************************************************************************
 * Function prototypes
 */
extern int
bcmptm_cth_alpm_usage_pre_insert(int u, int mtop);

extern int
bcmptm_cth_alpm_usage_imm_register(int u, int mtop);

#endif /* CTH_ALPM_USAGE_H */
