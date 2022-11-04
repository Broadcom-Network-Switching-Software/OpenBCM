
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SRV6_ACCESS_H__
#define __SRV6_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/srv6_types.h>


typedef int (*srv6_modes_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*srv6_modes_init_cb)(
    int unit);

typedef int (*srv6_modes_egress_is_psp_set_cb)(
    int unit, uint8 egress_is_psp);

typedef int (*srv6_modes_egress_is_psp_get_cb)(
    int unit, uint8 *egress_is_psp);

typedef int (*srv6_modes_encap_is_reduced_set_cb)(
    int unit, uint8 encap_is_reduced);

typedef int (*srv6_modes_encap_is_reduced_get_cb)(
    int unit, uint8 *encap_is_reduced);

typedef int (*srv6_modes_gsid_prefix_is_64b_set_cb)(
    int unit, uint8 gsid_prefix_is_64b);

typedef int (*srv6_modes_gsid_prefix_is_64b_get_cb)(
    int unit, uint8 *gsid_prefix_is_64b);

typedef int (*srv6_modes_usid_prefix_is_48b_set_cb)(
    int unit, uint8 usid_prefix_is_48b);

typedef int (*srv6_modes_usid_prefix_is_48b_get_cb)(
    int unit, uint8 *usid_prefix_is_48b);

typedef int (*srv6_modes_lpm_destination_for_usp_trap_set_cb)(
    int unit, int lpm_destination_for_usp_trap);

typedef int (*srv6_modes_lpm_destination_for_usp_trap_get_cb)(
    int unit, int *lpm_destination_for_usp_trap);



typedef struct {
    srv6_modes_egress_is_psp_set_cb set;
    srv6_modes_egress_is_psp_get_cb get;
} srv6_modes_egress_is_psp_cbs;

typedef struct {
    srv6_modes_encap_is_reduced_set_cb set;
    srv6_modes_encap_is_reduced_get_cb get;
} srv6_modes_encap_is_reduced_cbs;

typedef struct {
    srv6_modes_gsid_prefix_is_64b_set_cb set;
    srv6_modes_gsid_prefix_is_64b_get_cb get;
} srv6_modes_gsid_prefix_is_64b_cbs;

typedef struct {
    srv6_modes_usid_prefix_is_48b_set_cb set;
    srv6_modes_usid_prefix_is_48b_get_cb get;
} srv6_modes_usid_prefix_is_48b_cbs;

typedef struct {
    srv6_modes_lpm_destination_for_usp_trap_set_cb set;
    srv6_modes_lpm_destination_for_usp_trap_get_cb get;
} srv6_modes_lpm_destination_for_usp_trap_cbs;

typedef struct {
    srv6_modes_is_init_cb is_init;
    srv6_modes_init_cb init;
    srv6_modes_egress_is_psp_cbs egress_is_psp;
    srv6_modes_encap_is_reduced_cbs encap_is_reduced;
    srv6_modes_gsid_prefix_is_64b_cbs gsid_prefix_is_64b;
    srv6_modes_usid_prefix_is_48b_cbs usid_prefix_is_48b;
    srv6_modes_lpm_destination_for_usp_trap_cbs lpm_destination_for_usp_trap;
} srv6_modes_cbs;





extern srv6_modes_cbs srv6_modes;

#endif 
