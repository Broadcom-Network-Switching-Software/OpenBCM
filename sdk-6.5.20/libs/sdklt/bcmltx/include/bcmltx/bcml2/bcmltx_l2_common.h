/*! \file bcmltx_l2_common.h
 *
 * BCMLTX L2 transform public common Header file.
 * This file contains public common field transform information for BCMLTX L2.
 * L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID Transform Handler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_COMMON_H
#define BCMLTX_L2_COMMON_H

/*! Multicast MAC bit position. */
#define MC_MAC_BIT_POS        40

/*! L2 MAC type. */
typedef struct bcmltx_l2_mac_type_s {
    /*! Indicates multicast type MAC. */
    bool        is_mc;
} bcmltx_l2_mac_type_t;

/*! L2 MAC type version 1. */
extern bcmltx_l2_mac_type_t bcmltx_l2_mac_type;
/*! L2 MAC type version 2. */
extern bcmltx_l2_mac_type_t bcmltx_l2_mac_type2;

#endif /* BCMLTX_L2_COMMON_H */
