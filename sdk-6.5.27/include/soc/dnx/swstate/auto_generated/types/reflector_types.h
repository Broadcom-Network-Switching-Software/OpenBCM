
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __REFLECTOR_TYPES_H__
#define __REFLECTOR_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>


typedef struct {
    int encap_id;
    int is_allocated;
} reflector_uc_sw_db_info_t;

typedef struct {
    reflector_uc_sw_db_info_t uc;
    reflector_uc_sw_db_info_t uc_l2_int;
    reflector_uc_sw_db_info_t uc_l2_int_onepass;
    uint32 sbfd_reflector_cnt;
    uint32 sbfd_reflector_discriminator;
} reflector_sw_db_info_t;


#endif 
