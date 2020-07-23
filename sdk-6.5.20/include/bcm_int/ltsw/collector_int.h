/*! \file collector.h
 *
 * Collector header file.
 * This file contains the management for Collector.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMINT_LTSW_COLLECTOR_INT_H
#define BCMINT_LTSW_COLLECTOR_INT_H

#include <shr/shr_bitop.h>
#include <bcm/collector.h>
#include <bcm_int/ltsw/collector.h>
/******************************************************************************
 * Defines
 */


/*! Maximum ID of export profile logical table.*/
#define _EXPORT_PROFILE_ID_MAX 32

/*! Total number of IPV4 and IPV6 collector IDs.*/
#define _COLLECTOR_ID_MAX 64

/*! Maximum ID of collector ID per transport type.*/
#define _COLLECTOR_ID_MAX_PER_TYPE 32

/******************************************************************************
 * LTSW field structures
 */

/*
 * Typedef:
 *   bcmint_export_profile_id_bmp_s
 * Purpose:
 *   Entry bit map to track allocation of export profile ids.
 */
typedef struct bcmint_export_profile_info_s{
    SHR_BITDCL *export_profile_id_bmp;
    int *ref_count;
} bcmint_export_profile_info_t;


/*
 * Typedef:
 *  bcmint_collector_id_info_s
 *  Purpose:
 *  Book keeping information to track allocation of collector ids.
 */
typedef struct bcmint_collector_id_info_s{
    /*! Mapping of Internal ID : Transport type.*/
    SHR_BITDCL *ipv4_id_bitmap;
    SHR_BITDCL *ipv6_id_bitmap;
    SHR_BITDCL *externel_id_bitmap;
    /*! Two dimensional array mapping [ID][TYPE] */
    int *id_to_type_map;
    int *ref_count;
    bcm_ltsw_collector_export_app_t *user;
}bcmint_collector_id_info_t;

/* Collector Info Structure */
typedef struct bcmint_collector_bookkeeping_info_s {

    /*! Bitmap of allocated export profile id. */
    bcmint_export_profile_info_t export_profile_info;

    /*! Bitmap of allocated collector id and their type. */
    bcmint_collector_id_info_t collector_info;

}bcmint_collector_bookkeeping_info_t;

#endif /* BCMINT_LTSW_COLLECTOR_INT_H */
