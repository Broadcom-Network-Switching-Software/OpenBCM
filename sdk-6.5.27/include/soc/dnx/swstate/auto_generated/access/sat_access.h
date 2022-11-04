
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SAT_ACCESS_H__
#define __SAT_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/sat_types.h>


typedef int (*sat_pkt_header_info_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*sat_pkt_header_info_init_cb)(
    int unit);

typedef int (*sat_pkt_header_info_pkt_header_base_set_cb)(
    int unit, uint32 pkt_header_base_idx_0, uint16 pkt_header_base);

typedef int (*sat_pkt_header_info_pkt_header_base_get_cb)(
    int unit, uint32 pkt_header_base_idx_0, uint16 *pkt_header_base);

typedef int (*sat_pkt_header_info_pkt_header_base_alloc_cb)(
    int unit);

typedef int (*sat_pkt_header_info_pkt_header_base_is_allocated_cb)(
    int unit, uint8 *is_allocated);



typedef struct {
    sat_pkt_header_info_pkt_header_base_set_cb set;
    sat_pkt_header_info_pkt_header_base_get_cb get;
    sat_pkt_header_info_pkt_header_base_alloc_cb alloc;
    sat_pkt_header_info_pkt_header_base_is_allocated_cb is_allocated;
} sat_pkt_header_info_pkt_header_base_cbs;

typedef struct {
    sat_pkt_header_info_is_init_cb is_init;
    sat_pkt_header_info_init_cb init;
    sat_pkt_header_info_pkt_header_base_cbs pkt_header_base;
} sat_pkt_header_info_cbs;





extern sat_pkt_header_info_cbs sat_pkt_header_info;

#endif 
