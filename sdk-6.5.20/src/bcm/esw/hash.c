/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Load balance hash flex bin selection APIs.
 *   This module provides add/get/traverse/delete APIs for the flex hash bin
 * selection operation. The add API creates a hardware entry which consists of
 * key part and policy part. The key part is to specify rules to match
 * specific packets. The policy part selects matched packet's fields and switch
 * objects for hash bins. User can specify the entry ID with option 
 * BCM_HASH_FLEX_BIN_OPTION_WITH_ID. Higher entry_id has high priority during
 * hardware lookup in case of multiple entries are matched. If entry_id is not
 * specified, then a lowest available entry_id is automatically returned.
 * The hash bin is 16 bit wide. With option BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT,
 * user can populate the lower half and upper half of the bin independently.
 * In this case, the bin ID BCM_HASH_BIN_A_0 indicates lower half of the
 * BIN_A_0 while BCM_HASH_BIN_A_0_UPPER_HALF indicates the upper half. Otherwise
 * BCM_HASH_BIN_A_0 indicates the entire 16-bit BIN_A_0.
 */

#include <soc/drv.h>
#include <soc/scache.h>

#include <shared/bsl.h>
#include <assert.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <soc/esw/cancun.h>
#include <bcm_int/esw/switch.h>
#include <bcm/hash.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/hash.h>

bcmi_hash_flex_bin_info_t bcmi_hash_flex_bin_info[BCM_MAX_NUM_UNITS] = { {0} };


/*
 * Function:
 *   bcm_esw_hash_init 
 * Purpose:
 *   Initialize the hash flex bin module.
 *
 * Parameters:
 *   unit    - (IN)    Unit ID.
 */

int bcm_esw_hash_init (int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        soc_mem_lock(unit, FLEX_RTAG7_HASH_TCAMm);
        rv = bcmi_td3_hash_flex_bin_init(unit);
        soc_mem_unlock(unit, FLEX_RTAG7_HASH_TCAMm);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}
/*
 * Function:
 *   bcm_esw_hash_flex_bin_add
 * Purpose:
 *   Specify the flex bin selection rule and write to the TCAM entry. Higher 
 * entry_id has high priority during TCAM lookup in case of multiple rules
 * are matched. If entry_id is not specified, i.e. option 
 * BCM_HASH_FLEX_BIN_OPTION_WITH_ID is not given, then a lowest available
 * entry_id is automatically assigned.
 *
 * Parameters:
 *   unit     - (IN)    Unit ID.
 *   cfg      - (INOUT) Key part of selection rule and other parameters 
 *   num_bins - (IN)   Number of bin cmds. 
 *   bin_cmd  - (IN) Array of bin commands. Policy data part of rule.
 *   num_fields - (IN) Number of flex fields.
 *   flex_field - (IN) Array of flex fields.
 */

int bcm_esw_hash_flex_bin_add (int unit,
    bcm_hash_flex_bin_config_t *cfg,
    int num_bins,
    bcm_hash_flex_bin_cmd_t *bin_cmd,
    int num_fields,
    bcm_hash_flex_field_t *flex_field)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        soc_mem_lock(unit, FLEX_RTAG7_HASH_TCAMm);
        rv = bcmi_td3_hash_flex_bin_add (unit, cfg, num_bins, bin_cmd,
                 num_fields, flex_field);
        soc_mem_unlock(unit, FLEX_RTAG7_HASH_TCAMm);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *   bcm_esw_hash_flex_bin_delete
 * Purpose:
 *   Delete the given entry 
 * Parameters:
 *   unit        - (IN)  Unit ID.
 *   entry_id    - (IN)  The entry ID
 */

int bcm_esw_hash_flex_bin_delete( int unit, int entry_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_hash_flex_bin)) {
        soc_mem_lock(unit, FLEX_RTAG7_HASH_TCAMm);
        rv = bcmi_td3_hash_flex_bin_delete(unit,entry_id);
        soc_mem_unlock(unit, FLEX_RTAG7_HASH_TCAMm);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *   bcm_esw_hash_flex_bin_delete_all
 * Purpose:
 *   Delete all entries 
 * Parameters:
 *   unit - (IN)  Unit ID.
 */

int bcm_esw_hash_flex_bin_delete_all( int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_hash_flex_bin)) {
        soc_mem_lock(unit, FLEX_RTAG7_HASH_TCAMm);
        rv = bcmi_td3_hash_flex_bin_delete_all(unit);
        soc_mem_unlock(unit, FLEX_RTAG7_HASH_TCAMm);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *   bcm_esw_hash_flex_bin_get
 * Purpose:
 *   Get the entry configuration parameters for the given entry_id. 
 * Parameters:
 *   unit     - (IN)    Unit ID.
 *   cfg      - (INOUT) Key part of selection rule and other parameters 
 *   num_bins - (IN)   Number of bin cmds. 
 *   bin_cmd  - (IN) Array of bin commands. Policy part of selection rule.
 *   num_fields - (IN) Number of flex fields.
 *   flex_field - (IN) Array of flex fields.
 */

int bcm_esw_hash_flex_bin_get(
    int unit,
    bcm_hash_flex_bin_config_t *cfg,
    int num_bins,
    bcm_hash_flex_bin_cmd_t *bin_cmd,
    int num_fields,
    bcm_hash_flex_field_t *flex_field)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        rv = bcmi_td3_hash_flex_bin_get(unit, cfg, num_bins, bin_cmd,
                 num_fields, flex_field);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *      bcm_esw_hash_flex_bin_traverse
 * Purpose:
 *      Traverse all added entries and invoke the callback routine on
 * each entry.
 * 
 * Parameters:
 *      unit      - (IN)  Unit ID.
 *      option    - (IN)  bin_cmd for 8bit data or 16bit. Default 16bit. 
 *                        BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT: 8bit
 *      cb        - (IN)  callback function
 *      user_data - (IN)  User context data
 */

int bcm_esw_hash_flex_bin_traverse(
    int unit,
    uint32 option,
    bcm_hash_flex_bin_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        rv = bcmi_td3_hash_flex_bin_traverse(unit, option, cb, user_data);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *     bcm_esw_hash_flex_field_id_get
 * Description:
 *     Get the field_id from the flex object's field name. In Trident3,
 * the flex objects are defined in the ceh.yml file. Each object defines a
 * list of flex fields. For example:
 *   Flex object:  Z1_MATCH_ID
 *   Field:        ETAG
 * The ceh.yml file is packaged and loaded in the runtime.
 *
 * API call:
 *   uint32 field_id;   
 *   bcm_hash_flex_field_id_get(unit, BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE1,
 *                                     "ETAG",
 *                                     &field_id);
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    object         (IN) Flex object id 
 *    field_name     (IN) Field name 
 *    field_id       (OUT) field ID 
 * 
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *    This API is used for bcm_hash_flex_bin_add API's flex field.
 */

int bcm_esw_hash_flex_field_id_get(
    int unit,
    bcm_hash_flex_object_t object,
    const char *field_name,
    uint32 *field_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        rv = bcmi_td3_hash_flex_field_id_get(unit, object, field_name,
                 field_id);
    } 
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *     bcm_esw_hash_flex_field_name_get
 * Description:
 *     Get the flex object ID and field name from the field_id. In Trident3,
 * the flex objects are defined in the ceh.yml file. Each object defines a
 * list of flex fields. For example:
 *   Flex object:  Z1_MATCH_ID
 *   Field:        ETAG
 * The ceh.yml file is packaged and loaded in the runtime.
 *
 * API call:
 *   bcm_hash_flex_object_t object;
 *   char field_name[30];
 *   bcm_hash_flex_field_name_get(unit, field_id, &object, 30, field_name);
 *
 * Parameters:
 *    unit           (IN) Unit number
 *    field_id       (IN) flex field ID
 *    object         (OUT) Flex object id 
 *    size           (IN) size of the field name buffer 
 *    field_name     (OUT) Field name buffer
 * 
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *    This API is used for bcm_hash_flex_bin_get API's flex field.
 */

int bcm_esw_hash_flex_field_name_get (
    int unit,
    uint32 field_id,
    bcm_hash_flex_object_t *object,
    int size,
    char *field_name)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        int local_size;

        local_size = size;
        rv = bcmi_td3_hash_flex_field_name_get(unit,field_id, object,
                 &local_size, field_name);
    } 
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int _bcm_esw_hash_sync(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_hash_flex_bin)) {
        rv = bcmi_td3_hash_flex_bin_sync(unit);
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
