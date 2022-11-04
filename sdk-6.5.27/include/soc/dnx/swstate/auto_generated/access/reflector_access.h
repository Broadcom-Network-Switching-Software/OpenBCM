
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __REFLECTOR_ACCESS_H__
#define __REFLECTOR_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/reflector_types.h>


typedef int (*reflector_sw_db_info_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*reflector_sw_db_info_init_cb)(
    int unit);

typedef int (*reflector_sw_db_info_uc_encap_id_set_cb)(
    int unit, int encap_id);

typedef int (*reflector_sw_db_info_uc_encap_id_get_cb)(
    int unit, int *encap_id);

typedef int (*reflector_sw_db_info_uc_is_allocated_set_cb)(
    int unit, int is_allocated);

typedef int (*reflector_sw_db_info_uc_is_allocated_get_cb)(
    int unit, int *is_allocated);

typedef int (*reflector_sw_db_info_uc_l2_int_encap_id_set_cb)(
    int unit, int encap_id);

typedef int (*reflector_sw_db_info_uc_l2_int_encap_id_get_cb)(
    int unit, int *encap_id);

typedef int (*reflector_sw_db_info_uc_l2_int_is_allocated_set_cb)(
    int unit, int is_allocated);

typedef int (*reflector_sw_db_info_uc_l2_int_is_allocated_get_cb)(
    int unit, int *is_allocated);

typedef int (*reflector_sw_db_info_uc_l2_int_onepass_encap_id_set_cb)(
    int unit, int encap_id);

typedef int (*reflector_sw_db_info_uc_l2_int_onepass_encap_id_get_cb)(
    int unit, int *encap_id);

typedef int (*reflector_sw_db_info_uc_l2_int_onepass_is_allocated_set_cb)(
    int unit, int is_allocated);

typedef int (*reflector_sw_db_info_uc_l2_int_onepass_is_allocated_get_cb)(
    int unit, int *is_allocated);

typedef int (*reflector_sw_db_info_sbfd_reflector_cnt_set_cb)(
    int unit, uint32 sbfd_reflector_cnt);

typedef int (*reflector_sw_db_info_sbfd_reflector_cnt_get_cb)(
    int unit, uint32 *sbfd_reflector_cnt);

typedef int (*reflector_sw_db_info_sbfd_reflector_discriminator_set_cb)(
    int unit, uint32 sbfd_reflector_discriminator);

typedef int (*reflector_sw_db_info_sbfd_reflector_discriminator_get_cb)(
    int unit, uint32 *sbfd_reflector_discriminator);



typedef struct {
    reflector_sw_db_info_uc_encap_id_set_cb set;
    reflector_sw_db_info_uc_encap_id_get_cb get;
} reflector_sw_db_info_uc_encap_id_cbs;

typedef struct {
    reflector_sw_db_info_uc_is_allocated_set_cb set;
    reflector_sw_db_info_uc_is_allocated_get_cb get;
} reflector_sw_db_info_uc_is_allocated_cbs;

typedef struct {
    reflector_sw_db_info_uc_encap_id_cbs encap_id;
    reflector_sw_db_info_uc_is_allocated_cbs is_allocated;
} reflector_sw_db_info_uc_cbs;

typedef struct {
    reflector_sw_db_info_uc_l2_int_encap_id_set_cb set;
    reflector_sw_db_info_uc_l2_int_encap_id_get_cb get;
} reflector_sw_db_info_uc_l2_int_encap_id_cbs;

typedef struct {
    reflector_sw_db_info_uc_l2_int_is_allocated_set_cb set;
    reflector_sw_db_info_uc_l2_int_is_allocated_get_cb get;
} reflector_sw_db_info_uc_l2_int_is_allocated_cbs;

typedef struct {
    reflector_sw_db_info_uc_l2_int_encap_id_cbs encap_id;
    reflector_sw_db_info_uc_l2_int_is_allocated_cbs is_allocated;
} reflector_sw_db_info_uc_l2_int_cbs;

typedef struct {
    reflector_sw_db_info_uc_l2_int_onepass_encap_id_set_cb set;
    reflector_sw_db_info_uc_l2_int_onepass_encap_id_get_cb get;
} reflector_sw_db_info_uc_l2_int_onepass_encap_id_cbs;

typedef struct {
    reflector_sw_db_info_uc_l2_int_onepass_is_allocated_set_cb set;
    reflector_sw_db_info_uc_l2_int_onepass_is_allocated_get_cb get;
} reflector_sw_db_info_uc_l2_int_onepass_is_allocated_cbs;

typedef struct {
    reflector_sw_db_info_uc_l2_int_onepass_encap_id_cbs encap_id;
    reflector_sw_db_info_uc_l2_int_onepass_is_allocated_cbs is_allocated;
} reflector_sw_db_info_uc_l2_int_onepass_cbs;

typedef struct {
    reflector_sw_db_info_sbfd_reflector_cnt_set_cb set;
    reflector_sw_db_info_sbfd_reflector_cnt_get_cb get;
} reflector_sw_db_info_sbfd_reflector_cnt_cbs;

typedef struct {
    reflector_sw_db_info_sbfd_reflector_discriminator_set_cb set;
    reflector_sw_db_info_sbfd_reflector_discriminator_get_cb get;
} reflector_sw_db_info_sbfd_reflector_discriminator_cbs;

typedef struct {
    reflector_sw_db_info_is_init_cb is_init;
    reflector_sw_db_info_init_cb init;
    reflector_sw_db_info_uc_cbs uc;
    reflector_sw_db_info_uc_l2_int_cbs uc_l2_int;
    reflector_sw_db_info_uc_l2_int_onepass_cbs uc_l2_int_onepass;
    reflector_sw_db_info_sbfd_reflector_cnt_cbs sbfd_reflector_cnt;
    reflector_sw_db_info_sbfd_reflector_discriminator_cbs sbfd_reflector_discriminator;
} reflector_sw_db_info_cbs;





extern reflector_sw_db_info_cbs reflector_sw_db_info;

#endif 
