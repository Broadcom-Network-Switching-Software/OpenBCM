/** \file dnx/swstate/auto_generated/types/reflector_types.h
 *
 * sw state types (structs/enums/typedefs)
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __REFLECTOR_TYPES_H__
#define __REFLECTOR_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
/*
 * STRUCTs
 */

/**
 * A struct used to hold UC reflector info
 */
typedef struct {
    /**
     * global encap_id allocated for UC reflector (both L2 and L3)
     */
    int encap_id;
    /**
     * 1 if encap for UC reflector allocated
     */
    int is_allocated;
} reflector_uc_sw_db_info_t;

/**
 * A struct used to hold reflector SW info
 */
typedef struct {
    /**
     * L3 UC Reflector / L2 UC external reflector SW db info
     */
    reflector_uc_sw_db_info_t uc;
    /**
     * L2 UC internal Reflector SW db info
     */
    reflector_uc_sw_db_info_t uc_l2_int;
    /**
     * L2 UC internal One-pass Reflector SW db info
     */
    reflector_uc_sw_db_info_t uc_l2_int_onepass;
    /**
     * nof sbfd reflector
     */
    uint32 sbfd_reflector_cnt;
} reflector_sw_db_info_t;


#endif /* __REFLECTOR_TYPES_H__ */
