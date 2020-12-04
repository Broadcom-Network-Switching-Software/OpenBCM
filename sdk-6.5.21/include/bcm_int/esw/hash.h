/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Hash module definitions internal to the BCM library.
 */


#ifndef _BCM_INT_HASH_H
#define _BCM_INT_HASH_H

#include <bcm/types.h>
#include <soc/drv.h>
#include <bcm/hash.h>

/* Hash flex bin operation bookkeeping structure */
typedef struct {
    int initialized;
    SHR_BITDCL *entry_bmp;   /* TCAM entry usage bit map */
    uint16 *bin_obj_arr;      /* software bin object id for each bin */
} bcmi_hash_flex_bin_info_t;

extern bcmi_hash_flex_bin_info_t bcmi_hash_flex_bin_info[BCM_MAX_NUM_UNITS];

extern int _bcm_esw_hash_sync(int unit);
extern int bcmi_td3_hash_flex_bin_sync(int unit);
extern int bcmi_td3_hash_flex_field_name_get (int unit, uint32 field_id,
           bcm_hash_flex_object_t *object, int *size, char *field_name);
extern int bcmi_td3_hash_flex_field_id_get (int unit,
           bcm_hash_flex_object_t object, const char *field_name,
           uint32 *field_id);
extern int bcmi_td3_hash_flex_bin_traverse(int unit, uint32 option,
           bcm_hash_flex_bin_traverse_cb cb, void *user_data);
extern int bcmi_td3_hash_flex_bin_get (int unit,
           bcm_hash_flex_bin_config_t *cfg, int num_bins,
           bcm_hash_flex_bin_cmd_t *bin_cmd, int num_fields,
           bcm_hash_flex_field_t *flex_field);
extern int bcmi_td3_hash_flex_bin_delete_all( int unit);
extern int bcmi_td3_hash_flex_bin_delete( int unit, int entry_id);
extern int bcmi_td3_hash_flex_bin_add (int unit,
           bcm_hash_flex_bin_config_t *cfg, int num_bins,
           bcm_hash_flex_bin_cmd_t *bin_cmd, int num_fields,
           bcm_hash_flex_field_t *flex_field);
extern int bcmi_td3_hash_flex_bin_init (int unit);

#endif  /* !_BCM_INT_HASH_H */
