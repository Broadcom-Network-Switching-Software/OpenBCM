/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/udf.h>

#if defined (BCM_FLOWTRACKER_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_udf.h>
#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_common.h>

bcmi_ft_udf_tracking_param_info_db_t
        *bcmi_ft_udf_param_info_db[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_init
 * Purpose:
 *   Initialize udf tracking param info.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_info_init(int unit)
{
    bcmi_ft_udf_tracking_param_info_db_t *udf_param_info_db = NULL;

    if (bcmi_ft_udf_param_info_db[unit] != NULL) {
        bcmi_ft_udf_tracking_param_info_cleanup(unit);
    }

    BCMI_FT_ALLOC(udf_param_info_db,
            sizeof (bcmi_ft_udf_tracking_param_info_db_t),
            "UDF tracking param info db");
    if (udf_param_info_db == NULL) {
        return BCM_E_MEMORY;
    }

    bcmi_ft_udf_param_info_db[unit] = udf_param_info_db;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_cleanup
 * Purpose:
 *   Cleanup udf tracking param info.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_info_cleanup(int unit)
{
    bcmi_ft_udf_tracking_param_info_t *prev, *temp;

    if (bcmi_ft_udf_param_info_db[unit] != NULL) {

        if (bcmi_ft_udf_param_info_db[unit]->head) {
            prev = temp = bcmi_ft_udf_param_info_db[unit]->head;

            while(temp) {
                prev = temp;
                temp = temp->next;
                sal_free(prev);
                prev = NULL;
            }
            bcmi_ft_udf_param_info_db[unit]->head = NULL;
        }
        BCMI_FT_FREE(bcmi_ft_udf_param_info_db[unit]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_delete
 * Purpose:
 *   Delete udf tracking param info node.
 * Parameters:
 *   unit      - (IN) BCM device id
 *   param     - (IN) Flowtracker Tracking Parameter
 *   custom_id - (IN) Custom identifier for tracking param type custom
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_udf_tracking_param_info_delete(
    int unit,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id)
{
    bcmi_ft_udf_tracking_param_info_t *temp = NULL;
    bcmi_ft_udf_tracking_param_info_t *prev = NULL;
    bcmi_ft_udf_tracking_param_info_db_t *info_db = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    info_db = bcmi_ft_udf_param_info_db[unit];

    prev = NULL;
    temp = info_db->head;
    while (temp) {
        if ((temp->param == param) &&
            (temp->custom_id == custom_id)) {
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Requested param<%s,%d> not configured\n"),
                    plist[param], custom_id));
        return BCM_E_NOT_FOUND;
    }

    if (prev == NULL) {
        info_db->head = temp->next;
        BCMI_FT_FREE(temp);
    } else {
        prev->next = temp->next;
        BCMI_FT_FREE(temp);
    }
    info_db->count--;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_add
 * Purpose:
 *   Add udf tracking param info to database
 * Parameters:
 *   unit      - (IN) BCM device id
 *   flags     - (IN) Flags
 *   param     - (IN) Flowtracker Tracking Parameter
 *   custom_id - (IN) Custom identifier for tracking param type custom
 *   udf_info  - (IN) Flowtracker Tracking param udf info
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_udf_tracking_param_info_add(
    int unit,
    uint32 flags,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id,
    bcm_flowtracker_udf_info_t *udf_info)
{
    bcmi_ft_udf_tracking_param_info_t *prev = NULL;
    bcmi_ft_udf_tracking_param_info_t *temp = NULL;
    bcmi_ft_udf_tracking_param_info_t *node = NULL;
    bcmi_ft_udf_tracking_param_info_db_t *info_db = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    info_db = bcmi_ft_udf_param_info_db[unit];

    prev = NULL;
    temp = info_db->head;
    while (temp) {
        if ((temp->param == param) &&
            (temp->custom_id == custom_id)) {
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    if (temp != NULL) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Requested param<%s,%d> exists\n"),
                    plist[param], custom_id));
        return BCM_E_EXISTS;
    }

    /* Allocate */
    BCMI_FT_ALLOC(node, sizeof(bcmi_ft_udf_tracking_param_info_t),
        "Udf tracking param info");
    if (node == NULL) {
        return BCM_E_MEMORY;
    }

    node->next = NULL;
    node->param = param;
    node->udf_id = udf_info->udf_id;
    node->custom_id = custom_id;
    node->width  = BITS2BYTES(udf_info->width);
    node->offset = BITS2BYTES(udf_info->offset);

    if (prev == NULL) {
        info_db->head = node;
    } else {
        prev->next = node;
    }

    info_db->count++;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_set
 * Purpose:
 *   Main function to set tracking param udf information
 * Parameters:
 *   unit      - (IN) BCM device id
 *   flags     - (IN) Flags
 *   param     - (IN) Flowtracker Tracking Parameter
 *   udf_info  - (IN) Flowtracker Tracking param udf info
 *   custom_id - (IN/OUT) Custom identifier for tracking param type custom
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_set(
    int unit,
    uint32 flags,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id,
    bcm_flowtracker_udf_info_t *udf_info)
{
    int rv = BCM_E_NONE;
    uint16 param_width = 0;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    if (bcmi_ft_udf_param_info_db[unit] == NULL) {
        return BCM_E_INIT;
    }

    if ((param < 0) || (param >= bcmFlowtrackerTrackingParamTypeCount)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Invalid Tracking Param\n")));
        return BCM_E_PARAM;
    }

    if (param != bcmFlowtrackerTrackingParamTypeCustom) {
        custom_id = 0;
    }

    /* Reset udf tracking param info */
    if (udf_info == NULL) {
        rv = bcmi_ft_udf_tracking_param_info_delete(unit,
                param, custom_id);
        return rv;
    }

    /* Validate tracking param */
    if (_bcm_field_ft_udf_tracking_param_supported(unit, param, &param_width)
            != TRUE) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "param do not support UDF configuration.\n")));
        return BCM_E_PARAM;
    }

    /* Validate udf info */
    if (((udf_info->offset % 8) != 0) ||
            (udf_info->width == 0) ||
            ((udf_info->width % 8) != 0)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Invalid UDF info.\n")));
        return BCM_E_PARAM;
    }

    /* Tracking Param width do not match (non-custom type) */
    if ((param_width) && (udf_info->width != param_width)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "invalid param <%s> width. requested=%d required=%d\n"),
                    plist[param], udf_info->width, param_width));
        return BCM_E_PARAM;
    }

    /* Get udf object info */
    rv = _bcm_udf_td3_object_info_get(unit, udf_info->udf_id, &obj_info);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "UDF object (%d) is not configured\n"),
                    udf_info->udf_id));
        return rv;
    }

    /* Validate udf object */
    if ((udf_info->offset + udf_info->width) > (obj_info->width * 8)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Requested width exceed UDF width\n")));
        return BCM_E_PARAM;
    }

    /* Add to the info_db */
    rv = bcmi_ft_udf_tracking_param_info_add(unit,
            flags, param, custom_id, udf_info);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_get
 * Purpose:
 *   Main function to get tracking param udf information
 * Parameters:
 *   unit      - (IN) BCM device id
 *   flags     - (IN) Flags
 *   param     - (IN) Flowtracker Tracking Parameter
 *   custom_id - (IN) Custom identifier for tracking param type custom
 *   udf_info  - (OUT) Flowtracker Tracking param udf info
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_get(
    int unit,
    uint32 flags,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id,
    bcm_flowtracker_udf_info_t *udf_info)
{
    bcmi_ft_udf_tracking_param_info_t *node = NULL;
    bcmi_ft_udf_tracking_param_info_db_t *info_db = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    if (udf_info == NULL) {
        return BCM_E_PARAM;
    }

    if ((param < 0) || (param >= bcmFlowtrackerTrackingParamTypeCount)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Invalid Tracking Param\n")));
        return BCM_E_PARAM;
    }

    if (param != bcmFlowtrackerTrackingParamTypeCustom) {
        custom_id = 0;
    }

    info_db = bcmi_ft_udf_param_info_db[unit];

    node = info_db->head;
    while(node) {
        if ((node->param == param) &&
                (node->custom_id == custom_id)) {
            break;
        }
        node = node->next;
    }

    if (node == NULL) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Requested tracking param<%s,%d> not configured\n"),
                    plist[param], custom_id));
        return BCM_E_NOT_FOUND;
    }

    udf_info->udf_id = node->udf_id;
    udf_info->width  = BYTES2BITS(node->width);
    udf_info->offset = BYTES2BITS(node->offset);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_alloc_size
 * Purpose:
 *   Calculate size required for warmboot sync of udf tracking param info
 * Parameters:
 *   unit      - (IN) BCM device id
 *   size      - (OUT) calculated size
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_info_alloc_size(
    int unit,
    int *size)
{
    int struct_size = 0;

    if (bcmi_ft_udf_param_info_db[unit] == NULL) {
        return BCM_E_NONE;
    }

    *size += sizeof(uint16); /* Count */

    struct_size += sizeof(bcm_flowtracker_tracking_param_type_t); /* Param */
    struct_size += sizeof(uint16);          /* Custom Id */
    struct_size += sizeof(bcm_udf_id_t);    /* Udf Id */
    struct_size += sizeof(uint16);          /* Offset */
    struct_size += sizeof(uint16);          /* Width */

    *size += struct_size * bcmi_ft_udf_param_info_db[unit]->count;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_sync
 * Purpose:
 *   Sync udf tracking param info database
 * Parameters:
 *   unit      - (IN) BCM device id
 *   p_ft_scache_ptr - (IN/OUT) Pointer to scache pointer
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_info_sync(
    int unit,
    uint8 **p_ft_scache_ptr)
{
    uint16 count = 0;
    uint8 *ft_scache_ptr = NULL;
    bcmi_ft_udf_tracking_param_info_t *node = NULL;
    bcmi_ft_udf_tracking_param_info_db_t *info_db = NULL;

    ft_scache_ptr = *p_ft_scache_ptr;

    info_db = bcmi_ft_udf_param_info_db[unit];
    if (info_db == NULL) {
        count = 0;
        sal_memcpy(ft_scache_ptr, &count, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);
        *p_ft_scache_ptr = ft_scache_ptr;
        return BCM_E_NONE;
    }

    sal_memcpy(ft_scache_ptr, &info_db->count, sizeof(uint16));
    ft_scache_ptr += sizeof(uint16);

    node = info_db->head;
    while(node) {
        sal_memcpy(ft_scache_ptr, &node->param,
                    sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(ft_scache_ptr, &node->custom_id, sizeof(uint8));
        ft_scache_ptr += sizeof(uint16);

        sal_memcpy(ft_scache_ptr, &node->udf_id,
                    sizeof(bcm_udf_id_t));
        ft_scache_ptr += sizeof(bcm_udf_id_t);

        sal_memcpy(ft_scache_ptr, &node->offset, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);

        sal_memcpy(ft_scache_ptr, &node->width, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);

        node = node->next;
    }

    *p_ft_scache_ptr = ft_scache_ptr;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_recover
 * Purpose:
 *   Recover udf tracking param info database
 * Parameters:
 *   unit      - (IN) BCM device id
 *   p_ft_scache_ptr - (IN/OUT) Pointer to scache pointer
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_info_recover(
    int unit,
    uint8 **p_ft_scache_ptr)
{
    int idx = 0;
    uint16 count = 0;
    uint8 *ft_scache_ptr = NULL;
    bcmi_ft_udf_tracking_param_info_t *node = NULL;
    bcmi_ft_udf_tracking_param_info_t *temp = NULL;
    bcmi_ft_udf_tracking_param_info_db_t *info_db = NULL;

    ft_scache_ptr = *p_ft_scache_ptr;

    sal_memcpy(&count, ft_scache_ptr,sizeof(uint16));
    ft_scache_ptr += sizeof(uint16);

    info_db = bcmi_ft_udf_param_info_db[unit];
    if (info_db == NULL) {
        *p_ft_scache_ptr = ft_scache_ptr;
        return BCM_E_NONE;
    }

    for (idx = 0; idx < count; idx++ ) {

        node = NULL;
        BCMI_FT_ALLOC(node, sizeof(bcmi_ft_udf_tracking_param_info_t),
        "Udf tracking param info");

        sal_memcpy(&node->param, ft_scache_ptr,
                    sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(&node->custom_id, ft_scache_ptr, sizeof(uint8));
        ft_scache_ptr += sizeof(uint16);

        sal_memcpy(&node->udf_id, ft_scache_ptr,
                    sizeof(bcm_udf_id_t));
        ft_scache_ptr += sizeof(bcm_udf_id_t);

        sal_memcpy(&node->offset, ft_scache_ptr, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);

        sal_memcpy(&node->width, ft_scache_ptr, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);

        if (temp != NULL) {
            temp->next = node;
            temp = node;
        } else if (info_db->head == NULL) {
            info_db->head = node;
            temp = node;
        }
    }
    info_db->count = count;

    *p_ft_scache_ptr = ft_scache_ptr;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_udf_tracking_param_info_show
 * Purpose:
 *   Diag function to show udf tracking param info database
 * Parameters:
 *   unit      - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_udf_tracking_param_info_show(int unit)
{
    bcmi_ft_udf_tracking_param_info_t *node = NULL;
    bcmi_ft_udf_tracking_param_info_db_t *info_db = NULL;
    char *param_str[] = BCM_FT_TRACKING_PARAM_STRINGS;

    info_db = bcmi_ft_udf_param_info_db[unit];
    if (info_db == NULL) {
        return BCM_E_NONE;
    }

    LOG_CLI((BSL_META_U(unit, "\n\rFlowtracker UDF Param Mappings: Total=%d"),
                info_db->count));
    node = info_db->head;
    while (node) {
        LOG_CLI((BSL_META_U(unit, "\n\rParam: %s"),
                    param_str[node->param]));
        LOG_CLI((BSL_META_U(unit, "(%d) Udf_Id=%d Offset=%d Width=%d"),
                    node->custom_id, node->udf_id,
                    node->offset * 8, node->width * 8));
        node = node->next;
    }
    LOG_CLI((BSL_META_U(unit, "\n\r")));

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_udf_not_empty;
#endif /* BCM_FLOWTRACKER_SUPPORT */
