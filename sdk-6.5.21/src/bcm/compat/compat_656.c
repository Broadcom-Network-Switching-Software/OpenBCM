/*
 * $Id: compat_656.c,v 1.1 2016/10/27
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.6 routines
 */

#ifdef	BCM_RPC_SUPPORT

#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/compat/compat_656.h>

/*
 * Function:
 *      _bcm_compat656in_field_qset_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from older versions to
 *      SDK 6.5.6+
 * Parameters:
 *      from        - (IN) The old version of the datatype
 *      to          - (OUT) The SDK 6.5.6+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat656in_field_qset_t(
    bcm_compat656_field_qset_t *from,
    bcm_field_qset_t *to)
{
    uint32  field_qualify_cnt_size =
        (BCM_COMPAT656_FIELD_QUALIFY_CNT < bcmFieldQualifyCount) ?
            BCM_COMPAT656_FIELD_QUALIFY_CNT :  bcmFieldQualifyCount;
    uint32  field_user_num_udfs_size =
       (BCM_COMPAT656_FIELD_USER_NUM_UDFS < BCM_FIELD_USER_NUM_UDFS) ?
        BCM_COMPAT656_FIELD_USER_NUM_UDFS : BCM_FIELD_USER_NUM_UDFS;

    SHR_BITCOPY_RANGE(to->w, 0, from->w, 0, field_qualify_cnt_size);
    SHR_BITCOPY_RANGE(to->w, bcmFieldQualifyCount,
                      from->w, BCM_COMPAT656_FIELD_QUALIFY_CNT,
                      field_user_num_udfs_size);
    SHR_BITCOPY_RANGE(to->udf_map, 0,
                      from->udf_map, 0,
                      field_user_num_udfs_size);
}

/*
 * Function:
 *      _bcm_compat656out_field_qset_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from SDK 6.5.6+ to
 *      older versions
 * Parameters:
 *      from        - (IN) The SDK 6.5.6+ version of the datatype
 *      to          - (OUT) The old version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat656out_field_qset_t(
    bcm_field_qset_t *from,
    bcm_compat656_field_qset_t *to)
{
    uint32  field_qualify_cnt_size =
        (BCM_COMPAT656_FIELD_QUALIFY_CNT < bcmFieldQualifyCount) ?
            BCM_COMPAT656_FIELD_QUALIFY_CNT :  bcmFieldQualifyCount;
    uint32  field_user_num_udfs_size =
       (BCM_COMPAT656_FIELD_USER_NUM_UDFS < BCM_FIELD_USER_NUM_UDFS) ?
        BCM_COMPAT656_FIELD_USER_NUM_UDFS : BCM_FIELD_USER_NUM_UDFS;

    SHR_BITCOPY_RANGE(to->w, 0, from->w, 0, field_qualify_cnt_size);
    SHR_BITCOPY_RANGE(to->w, BCM_COMPAT656_FIELD_QUALIFY_CNT,
                      from->w, bcmFieldQualifyCount,
                      field_user_num_udfs_size);
    SHR_BITCOPY_RANGE(to->udf_map, 0,
                      from->udf_map, 0,
                      field_user_num_udfs_size);
}

/*
 * Function: bcm_compat656_field_group_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_create.
 *      Used to create a FP group using an older version of the datatype.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     qset - (IN) Old field qualifier set
 *     pri  - (IN) Priority within allowable range,
 *                 or BCM_FIELD_GROUP_PRIO_ANY to automatically assign
 *                 priority; each priority value may be used only once
 *     group - (OUT) New field group ID
 *
 * Returns:
 *     BCM_E_INIT     - BCM unit not initialized
 *     BCM_E_PARAM    - pri out of range (0-15 for FB & ER) or group == NULL
 *     BCM_E_RESOURCE - no select codes will satisfy qualifier set
 *     BCM_E_MEMORY   - allocation failure
 *     BCM_E_NONE     - Success
 */
int
bcm_compat656_field_group_create(int unit,
                                bcm_compat656_field_qset_t qset,
                                int pri,
                                bcm_field_group_t *group)
{
    int rv;
    bcm_field_qset_t *p_new_qset;

    /* Create the qualifier set from heap to avoid the stack to bloat */
    p_new_qset =
        (bcm_field_qset_t *) sal_alloc(sizeof(bcm_field_qset_t), "NewQset");
    if (NULL == p_new_qset) {
        return BCM_E_MEMORY;
    }

    /* Initialize the new qualifier set */
    BCM_FIELD_QSET_INIT(*p_new_qset);

    /* Transform the qulaifier set from the old format to new one */
    _bcm_compat656in_field_qset_t(&qset, p_new_qset);

    /* Call the BCM API with new qset format */
    rv = bcm_field_group_create(unit, *p_new_qset, pri, group);

    /* Deallocate the q set memory */
    sal_free(p_new_qset);

    return rv;
}

/*
 * Function: bcm_compat656_field_group_create_id
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_create_id.
 *      Used to create a FP group using an older version of the datatype.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     qset - (IN) Old field qualifier set
 *     pri  - (IN) Priority within allowable range,
 *                 or BCM_FIELD_GROUP_PRIO_ANY to automatically assign
 *                 priority; each priority value may be used only once
 *     group - (IN) Requested new field group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - No unused group/slices left
 *     BCM_E_PARAM     - priority out of range (0-15 for FB & ER)
 *     BCM_E_EXISTS    - group with that id already exists on this unit.
 *     BCM_E_MEMORY    - Memory allocation failure
 *     BCM_E_NONE      - Success
 */
int
bcm_compat656_field_group_create_id(int unit,
                                    bcm_compat656_field_qset_t qset,
                                    int pri,
                                    bcm_field_group_t group)
{
    int rv;
    bcm_field_qset_t *p_new_qset;

    /* Create the qualifier set from heap to avoid the huge stack variable */
    p_new_qset =
        (bcm_field_qset_t *) sal_alloc(sizeof(bcm_field_qset_t), "NewQset");
    if (NULL == p_new_qset) {
        return BCM_E_MEMORY;
    }

    /* Initialize the new qualifier set */
    BCM_FIELD_QSET_INIT(*p_new_qset);

    /* Transform the qulaifier set from the old format to new one */
    _bcm_compat656in_field_qset_t(&qset, p_new_qset);

    /* Call the BCM API with new qset format */
    rv = bcm_field_group_create_id(unit, *p_new_qset, pri, group);

    /* Deallocate the q set memory */
    sal_free(p_new_qset);

    return (rv);
}

/*
 * Function: bcm_compat656_field_qset_data_qualifier_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_qset_data_qualifier_add
 *      Used to add field data qualifier to group qset.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     qset - (IN/OUT) Old group qualifier set
 *     qualifier_id  - (IN) Data qualifier id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat656_field_qset_data_qualifier_add(int unit,
                                            bcm_compat656_field_qset_t *qset,
                                            int qual_id)
{
    int rv;
    bcm_field_qset_t *p_new_qset=NULL;

    /* Only tranform qualifier set if given a valid qset */
    if (qset) {

        /* Create the qualifier set from heap to avoid the stack to bloat */
        p_new_qset =
            (bcm_field_qset_t *)sal_alloc(sizeof(bcm_field_qset_t), "NewQset");
        if (NULL == p_new_qset) {
            return BCM_E_MEMORY;
        }

        /* Initialize the new qualifier set */
        BCM_FIELD_QSET_INIT(*p_new_qset);

        /* Transform the qualifier set from the old format to new one */
        _bcm_compat656in_field_qset_t(qset, p_new_qset);
    }

    /* Call the BCM API with new qset format */
    rv = bcm_field_qset_data_qualifier_add(unit, p_new_qset, qual_id);

    if (qset) {
        /* Copy back the qset from new to old format */
        if (BCM_SUCCESS(rv)) {
            _bcm_compat656out_field_qset_t(p_new_qset, qset);
        }

        /* Deallocate the q set memory */
        sal_free(p_new_qset);
    }

    return (rv);
}

#endif  /* BCM_RPC_SUPPORT */
