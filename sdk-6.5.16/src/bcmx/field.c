 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <sal/core/alloc.h>

#include <bcmx/field.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_FIELD_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_FIELD_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_FIELD_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_FIELD_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_FIELD_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)

static uint32    _bcmx_group_id;
static uint32    _bcmx_entry_id;
static uint32    _bcmx_range_id;
static uint32    _bcmx_stat_id;

/* 
 * BCMX uses a range of numbers starting at a high value for,
 *   Group Id, Entry Id, Range Id
 */
#define GROUP_ID_ALLOC    (('X'<<24) + (++_bcmx_group_id))
#define ENTRY_ID_ALLOC    (('Y'<<24) + (++_bcmx_entry_id))
#define RANGE_ID_ALLOC    (('Z'<<24) + (++_bcmx_range_id))
#define STAT_ID_ALLOC     (( 1 <<16) + (++_bcmx_stat_id))


/*
 * Function:
 *     bcmx_field_init
 * Purpose:
 *     Initialize field software subsystem
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_EXISTS    Field Processor previously initialized
 *     BCM_E_MEMORY    Allocation failure
 *     BCM_E_XXX       Error code
 *     BCM_E_NONE
 */

int
bcmx_field_init(void)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    _bcmx_group_id = 0;
    _bcmx_entry_id = 0;
    _bcmx_range_id = 0;
    _bcmx_stat_id  = 0;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_field_control_set
 * Purpose:
 *     Set control status.
 * Parameters:
 *     control  - Control element to set
 *     status   - Status for field module
 * Returns:
 *     BCM_E_INIT    - BCM unit not initialized
 *     BCM_E_NONE    - Success
 */

int
bcmx_field_control_set(bcm_field_control_t control,
                       uint32 state)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_control_set(bcm_unit, control, state);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_control_get
 * Purpose:
 *     Get control status info.
 * Parameters:
 *     control  - Control element to get
 *     status   - (OUT) Status of field element
 * Returns:
 *     BCM_E_INIT    - BCM unit not initialized
 *     BCM_E_PARAM   - *state pointing to NULL
 *     BCM_E_NONE    - Success
 * Notes:
 *     Get value from first successful call
 */

int
bcmx_field_control_get(bcm_field_control_t control,
                       uint32 *state)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(state);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_control_get(bcm_unit, control, state);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_group_traverse
 * Purpose:
 *      Traverse all the fp groups in the system, calling a specified
 *      callback for each one
 * Parameters:
 *      callback - (IN) A pointer to the callback function to call for each fp group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcmx_field_group_traverse(bcm_field_group_traverse_cb callback, void *user_data)
{
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */
    int rv;                    /* Overall operation result.      */

    BCMX_FIELD_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_field_group_traverse (bcm_unit, callback, user_data);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_group_create
 * Purpose:
 *     Create a field group based on the field group selector flags.
 * Parameters:
 *     qset  - Field qualifier set
 *     pri   - Priority within allowable range 
 *             or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *             priority; each priority value may be used only once
 *     group - (OUT) Field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left OR qset doesn't work on
 *                      hardware
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_create(bcm_field_qset_t qset,
                        int pri,
                        bcm_field_group_t *group)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;
    bcm_field_group_t    gid;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(group);

    gid = GROUP_ID_ALLOC;
    *group = gid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_create_id(bcm_unit, qset, pri, gid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_create_id
 * Purpose:
 *     Create a field group based on the field group selector flags
 *     with a requested ID.
 * Parameters:
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    group - Requested field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_create_id(bcm_field_qset_t qset,
                           int pri,
                           bcm_field_group_t group)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_create_id(bcm_unit, qset, pri, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_wlan_create_mode
 * Purpose:
 *     Create a wlan field group based on the field group selector flags
 *     and the given group mode.
 * Parameters:
 *     qset  - Field qualifier set
 *     pri   - Priority within allowable range
 *             or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *             priority; each priority value may be used only once
 *     mode  - Group mode (single, double, triple or Auto-wide)
 *     group - (OUT) Field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left OR qset doesn't work on
 *                      hardware
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_wlan_create_mode(bcm_field_qset_t qset,
                                  int pri,
                                  bcm_field_group_mode_t mode,
                                  bcm_field_group_t *group)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;
    bcm_field_group_t    gid;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(group);

    gid = GROUP_ID_ALLOC;
    *group = gid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_wlan_create_mode_id(bcm_unit, qset, 
                                                     pri, mode, gid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_wlan_create_mode_id
 * Purpose:
 *     Create a wlan field group based on the field group selector flags,
 *     the given group mode, and the requested group ID.
 * Parameters:
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */
int
bcmx_field_group_wlan_create_mode_id(bcm_field_qset_t qset,
                                     int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t group)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_wlan_create_mode_id(bcm_unit, qset, 
                                                     pri, mode, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_create_mode
 * Purpose:
 *     Create a field group based on the field group selector flags
 *     and the given group mode.
 * Parameters:
 *     qset  - Field qualifier set
 *     pri   - Priority within allowable range
 *             or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *             priority; each priority value may be used only once
 *     mode  - Group mode (single, double, triple or Auto-wide)
 *     group - (OUT) Field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left OR qset doesn't work on
 *                      hardware
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_create_mode(bcm_field_qset_t qset,
                             int pri,
                             bcm_field_group_mode_t mode,
                             bcm_field_group_t *group)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;
    bcm_field_group_t    gid;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(group);

    gid = GROUP_ID_ALLOC;
    *group = gid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_create_mode_id(bcm_unit, qset, 
                                                pri, mode, gid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_create_mode_id
 * Purpose:
 *     Create a field group based on the field group selector flags,
 *     the given group mode, and the requested group ID.
 * Parameters:
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */
int
bcmx_field_group_create_mode_id(bcm_field_qset_t qset,
                                int pri,
                                bcm_field_group_mode_t mode,
                                bcm_field_group_t group)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_create_mode_id(bcm_unit, qset, 
                                                pri, mode, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_port_create_mode
 * Purpose:
 *     Create a field group based on the field group selector flags,
 *     given group mode and the given logical port.
 * Parameters:
 *     port  - Logical port number where group is defined
 *     qset  - Field qualifier set
 *     pri   - Priority within allowable range
 *             or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *             priority; each priority value may be used only once
 *     mode  - Group mode (single, double, triple or Auto-wide)
 *     group - (OUT) Field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left OR qset doesn't work on
 *                      hardware
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_port_create_mode(bcmx_lport_t port,
                                  bcm_field_qset_t qset,
                                  int pri,
                                  bcm_field_group_mode_t mode,
                                  bcm_field_group_t *group)
{
    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(group);

    *group = GROUP_ID_ALLOC;

    return bcmx_field_group_port_create_mode_id(port, qset, pri, mode, *group);
}

/*
 * Function:
 *     bcmx_field_group_port_create_mode_id
 * Purpose:
 *     Create a field group based on the field group selector flags,
 *     given group mode, logical port, and requested group ID.
 * Parameters:
 *     port - Logical port number where group is defined
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_port_create_mode_id(bcmx_lport_t port,
                                     bcm_field_qset_t qset,
                                     int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t group)
{
    int               rv = BCM_E_UNAVAIL, tmp_rv;
    int               bcm_unit;
    bcm_port_t        bcm_port;
    bcm_pbmp_t        pbmp;
    int               i, unit;

    BCMX_FIELD_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    /* Create group in unit where given port resides */
    tmp_rv = bcm_field_group_port_create_mode_id(bcm_unit, bcm_port, qset,
                                                 pri, mode, group);
    BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

    /* Create group with null ports on rest of units */
    BCM_PBMP_CLEAR(pbmp);
    BCMX_UNIT_ITER(unit, i) {
        if (unit == bcm_unit) {
            continue;
        }
        tmp_rv = bcm_field_group_ports_create_mode_id(unit, pbmp, qset,
                                                      pri, mode, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_ports_create_mode
 * Purpose:
 *     Create a field group based on the field group selector flags,
 *     given group mode and the given logical port list.
 * Parameters:
 *     lplist - Logical port list for which the group is defined
 *     qset   - Field qualifier set
 *     pri    - Priority within allowable range
 *              or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *              priority; each priority value may be used only once
 *     mode   - Group mode (single, double, triple or Auto-wide)
 *     group  - (OUT) Field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left OR qset doesn't work on
 *                      hardware
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_ports_create_mode(bcmx_lplist_t lplist,
                                   bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_mode_t mode,
                                   bcm_field_group_t *group)
{
    int               rv = BCM_E_UNAVAIL, tmp_rv;
    int               i, bcm_unit;
    bcm_pbmp_t        pbmp;
    bcm_field_group_t gid;

    BCMX_FIELD_INIT_CHECK;

    if (bcmx_lplist_is_null(&lplist)) {
        return BCM_E_PARAM;
    }

    BCMX_PARAM_NULL_CHECK(group);

    gid = GROUP_ID_ALLOC;
    *group = gid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        tmp_rv = bcm_field_group_ports_create_mode_id(bcm_unit, pbmp, qset,
                                                      pri, mode, gid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_ports_create_mode_id
 * Purpose:
 *     Create a field group based on the field group selector flags,
 *     given group mode, logical port list, and requested group ID.
 * Parameters:
 *     lplist - Logical port list for which the group is defined
 *     qset   - Field qualifier set
 *     pri    - Priority within allowable range 
 *              or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *              priority; each priority value may be used only once
 *     mode   - Group mode (single, double, triple or Auto-wide)
 *     group - Requested field group ID
 * Returns:
 *     BCM_E_RESOURCE   No unused group/slices left OR qset doesn't work on
 *                      hardware
 *     BCM_E_PARAM      Priority out of range (0-15 for FB & ER)
 *     BCM_E_MEMORY     Group memory allocation failure
 *     BCM_E_XXX        Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_ports_create_mode_id(bcmx_lplist_t lplist,
                                      bcm_field_qset_t qset,
                                      int pri,
                                      bcm_field_group_mode_t mode,
                                      bcm_field_group_t group)
{
    int               rv = BCM_E_UNAVAIL, tmp_rv;
    int               i, bcm_unit;
    bcm_pbmp_t        pbmp;

    BCMX_FIELD_INIT_CHECK;

    if (bcmx_lplist_is_null(&lplist)) {
        return BCM_E_PARAM;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        tmp_rv = bcm_field_group_ports_create_mode_id(bcm_unit, pbmp, qset,
                                                      pri, mode, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_mode_get
 * Purpose:
 *     Get the mode for the specified field group
 * Parameters:
 *     group - Field group ID
 *     mode  - (OUT) Field group mode
 * Returns:
 *     BCM_E_NOT_FOUND   Field Group ID not found
 *     BCM_E_PARAM       mode is NULL
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call
 */

int 
bcmx_field_group_mode_get(bcm_field_group_t group,
                          bcm_field_group_mode_t *mode)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_group_mode_get(bcm_unit, group, mode);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_group_set
 * Purpose:
 *     Update a previously created field group based on the field
 *     group selector flags
 * Parameters:
 *     group - Field group ID
 *     qset  - Field qualifier set
 * Returns:
 *     BCM_E_NOT_FOUND   Group ID not found
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 * Notes:
 *     If no entry exist that use this group then updates are always
 *     permitted.
 *     If entries exist that use this group then updates are permitted
 *     only if it can be satisfied with the current selection of
 *     (f0, f1, f2, f3) field selector encodings.
 */

int
bcmx_field_group_set(bcm_field_group_t group,
                     bcm_field_qset_t qset)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_set(bcm_unit, group, qset);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_get
 * Purpose:
 *     Get the qualifier set for a previously created field group
 * Parameters:
 *     group - Field group ID
 *     qset  - (OUT) Field qualifier set
 * Returns:
 *     BCM_E_NOT_FOUND   Field Group ID not found
 *     BCM_E_PARAM       qset is NULL
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call
 */

int
bcmx_field_group_get(bcm_field_group_t group,
                     bcm_field_qset_t *qset)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(qset);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_group_get(bcm_unit, group, qset);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_group_destroy
 * Purpose:
 *     Delete a field group
 * Parameters:
 *     group - Field group ID
 * Returns:
 *     BCM_E_FAIL        Entries not destroyed yet 
 *     BCM_E_NONE
 * Notes:
 *      All entries that uses this group should have been destroyed
 *      before calling this routine.
 *      Operation will fail if entries exist that uses this template
 */

int
bcmx_field_group_destroy(bcm_field_group_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_destroy(bcm_unit, group);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_enable_set
 * Purpose:
 *     Enable or disable packet lookup on the given group.
 * Parameters:
 *     group   - Field group ID
 *     enable  - Packet lookup is enable(if !=0) or disable(if == 0) on group
 * Returns:
 *     BCM_E_NOT_FOUND   Field Group ID not found
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_enable_set(bcm_field_group_t group,
                            int enable)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_enable_set(bcm_unit, group, enable);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_enable_get
 * Purpose:
 *     Get the packet lookup enable/disable state on the given group.
 * Parameters:
 *     group   - Field group ID
 *     enable  - (OUT) Packet lookup enable/disable state of group
 * Returns:
 *     BCM_E_NOT_FOUND   Field Group ID not found
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call
 */

int
bcmx_field_group_enable_get(bcm_field_group_t group,
                            int *enable)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_group_enable_get(bcm_unit, group, enable);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_group_install
 * Purpose:
 *     Install all entries of a field group into the hardware tables.
 * Parameters:
 *     group - Field group ID
 * Returns:
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_group_install(bcm_field_group_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_install(bcm_unit, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_remove
 * Purpose:
 *     Remove all entries of a field group from the hardware table.
 * Parameters:
 *     group - Field group ID
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     This does not destroy the group or its entries; it uninstalls them from
 *     the hardware tables.
 *     Destroy a group using bcmx_field_group_destroy()
 */

int
bcmx_field_group_remove(bcm_field_group_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_remove(bcm_unit, group);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_range_create
 * Purpose:
 *     Allocate a port range checker
 * Parameters:
 *     range - (OUT) Range ID
 *     flags - One or more of BCM_FIELD_RANGE_xxx flags
 *     min   - Lower bounds of range to be checked
 *     max   - Upper bounds of range to be checked
 * Returns:
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_range_create(bcm_field_range_t *range,
                        uint32 flags,
                        bcm_l4_port_t min,
                        bcm_l4_port_t max)
{
    int               rv = BCM_E_UNAVAIL, tmp_rv;
    int               i, bcm_unit;
    bcm_field_range_t range_id;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(range);

    range_id = RANGE_ID_ALLOC;
    *range = range_id;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_range_create_id(bcm_unit, range_id,
                                           flags, min, max);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_range_create_id
 * Purpose:
 *     Allocate a port range checker with requested ID.
 * Parameters:
 *     range - Range check ID to use
 *     flags - One or more of BCM_FIELD_RANGE_xxx flags
 *     min   - Lower bounds of range to be checked
 *     max   - Upper bounds of range to be checked
 * Returns:
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_range_create_id(bcm_field_range_t range,
                           uint32 flags,
                           bcm_l4_port_t min,
                           bcm_l4_port_t max)
{
    int               rv = BCM_E_UNAVAIL, tmp_rv;
    int               i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_range_create_id(bcm_unit, range,
                                           flags, min, max);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: bcmx_field_range_group_create
 *    
 * Purpose:
 *     Allocate an interface group range checker and set its parameters.
 *
 * Parameters:
 *     range  - (OUT) Range check ID, will not be zero
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *     group  - L3 interface group number
 *
 * Returns:
 *     BCM_E_XXX       - Error code
 *     BCM_E_NONE      - Success
 */
int
bcmx_field_range_group_create(bcm_field_range_t *range,
                              uint32 flags,
                              bcm_l4_port_t min,
                              bcm_l4_port_t max, 
                              bcm_if_group_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(range);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_range_group_create(bcm_unit, range,
                                              flags, min, max, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: bcmx_field_range_group_create_id
 *    
 * Purpose:
 *     Allocate an interface group range checker with requested ID
 *     and set its parameters.
 *
 * Parameters:
 *     range  - Range check ID to use
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *     group  - L3 interface group number
 *
 * Returns:
 *     BCM_E_XXX       - Error code
 *     BCM_E_NONE      - Success
 */
int
bcmx_field_range_group_create_id(bcm_field_range_t range,
                                 uint32 flags,
                                 bcm_l4_port_t min,
                                 bcm_l4_port_t max, 
                                 bcm_if_group_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_range_group_create_id(bcm_unit, range,
                                                 flags, min, max, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_range_get
 * Purpose:
 *     Get the TCP/UDP port range check setup
 * Parameters:
 *     range - Range ID
 *     flags - (OUT) One or more of BCM_FIELD_RANGE_xxx flags
 *     min   - (OUT) Lower bounds of range to be checked
 *     max   - (OUT) Upper bounds of range to be checked
 * Returns:
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call
 */
int
bcmx_field_range_get(bcm_field_range_t range,
                     uint32 *flags,
                     bcm_l4_port_t *min,
                     bcm_l4_port_t *max)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);
    BCMX_PARAM_NULL_CHECK(min);
    BCMX_PARAM_NULL_CHECK(max);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_range_get(bcm_unit, range, flags, min, max);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_range_destroy
 * Purpose:
 *     Deallocate a range
 * Parameters:
 *     range - Range ID
 * Returns:
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call
 */
int
bcmx_field_range_destroy(bcm_field_range_t range)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_range_destroy(bcm_unit, range);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_create
 * Purpose:
 *     Create a blank entry group based on a group.
 * Parameters:
 *     group - Field group ID
 *     entry - (OUT) New entry ID
 * Returns: 
 *     BCM_E_EXISTS      Entry ID already in use
 *     BCM_E_NOT_FOUND   Group ID not found
 *     BCM_E_MEMORY      allocation failure
 *     BCM_E_NONE
 */

int
bcmx_field_entry_create(bcm_field_group_t group,
                        bcm_field_entry_t *entry)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;
    bcm_field_entry_t    eid;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(entry);

    eid = ENTRY_ID_ALLOC;
    *entry = eid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_create_id(bcm_unit, group, eid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_create_id
 * Purpose:
 *     Create a blank entry group with requested ID based on a group.
 * Parameters:
 *     group - Field group ID
 *     entry - Requested entry ID
 * Returns: 
 *     BCM_E_EXISTS      Entry ID already in use
 *     BCM_E_NOT_FOUND   Group ID not found
 *     BCM_E_MEMORY      allocation failure
 *     BCM_E_NONE
 */

int
bcmx_field_entry_create_id(bcm_field_group_t group,
                           bcm_field_entry_t entry)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_create_id(bcm_unit, group, entry);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_multi_get
 * Purpose:
 *     Gets an array of a group's entry IDs
 * Parameters:
 *     group       - Field group ID.
 *     entry_size  - Maximum number of entries to return.  Set to 0
 *                   to get the number of entries available.
 *     entry_array - (OUT) Pointer to a buffer to fill with the array of
 *                         entry IDs.  Ignored if entry_size is 0.
 *     entry_count - (OUT) Returns the number of entries returned in the
 *                         array, or if entry_size was 0, the number of
 *                         entries available.
 * Returns:
 *     BCM_E_INIT      - Unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found
 *     BCM_E_NONE      - Success
 * Notes:
 *     Assumes homogeneuous systems and all devices in stack
 *     has the same field groups/entries successfully installed.
 */
int
bcmx_field_entry_multi_get(bcm_field_group_t group, 
                           int entry_size,
                           bcm_field_entry_t *entry_array,
                           int *entry_count)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(entry_count);

    /*
     * No need to gather entries from all units.  BCMX assumes
     * that field groups/entries were created with BCMX APIs.
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_entry_multi_get(bcm_unit, group, entry_size,
                                       entry_array, entry_count);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_entry_destroy
 * Purpose:
 *     Destroy an entry
 * Parameters:
 *     entry - Entry ID
 * Returns:
 *     BCM_E_INTERNAL    Null group found
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE      
 */

int
bcmx_field_entry_destroy(bcm_field_entry_t entry)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_destroy(bcm_unit, entry);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_destroy_all
 * Purpose:
 *     Destroy all entries
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_entry_destroy_all(void)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_copy
 * Purpose:
 *     Create a copy of an existing entry
 * Parameters:
 *     src_entry - Entry to copy
 *     dst_entry - (OUT) New entry
 * Returns:
 *     BCM_E_NOT_FOUND   Source entry not found
 *     BCM_E_INTERNAL    Newly created entry not found.
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_entry_copy(bcm_field_entry_t src_entry,
                      bcm_field_entry_t *dst_entry)
{
    int                  rv = BCM_E_UNAVAIL, tmp_rv;
    int                  i, bcm_unit;
    bcm_field_entry_t    eid;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dst_entry);

    eid = ENTRY_ID_ALLOC;
    *dst_entry = eid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_copy_id(bcm_unit, src_entry, eid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_install
 * Purpose:
 *     Install entry into the hardware tables
 * Parameters:
 *     entry - Entry to install
 * Returns:
 *     BCM_E_NOT_FOUND   Entry ID not found
 *     BCM_E_INTERNAL    No group for entry ID
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 * Notes:
 *     Qualifications should be made and actions should be added
 *     prior to installing the entry.
 *     The bcm_field_entry_t object is not consumed or destroyed.
 *     It can be used to remove or update the entry.
 */

int
bcmx_field_entry_install(bcm_field_entry_t entry)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_install(bcm_unit, entry);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_reinstall
 * Purpose:
 *     Re-install entry into the hardware tables
 * Parameters:
 *     entry - Entry to install
 * Returns:
 *     BCM_E_NOT_FOUND   Entry ID not found
 *     BCM_E_INTERNAL    No group for entry ID
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 *
 * Notes:
 *     Reinstallation may only be used to change the actions for
 *     an installed entry without having to remove and re-add the
 *     entry.
 */

int
bcmx_field_entry_reinstall(bcm_field_entry_t entry)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_reinstall(bcm_unit, entry);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_remove
 * Purpose:
 *     Remove entry from the hardware tables
 * Parameters:
 *     entry - Entry to remove
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     This does not destroy the entry; it uninstalls it from
 *     the hardware tables.
 *     Destroy a entry using bcmx_field_entry_destroy()
 */

int
bcmx_field_entry_remove(bcm_field_entry_t entry)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_remove(bcm_unit, entry);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: bcmx_field_entry_prio_get
 *
 * Purpose:
 *     Gets the priority within the group of the entry.
 *
 * Parameters:
 *     entry  - Field entry to operate on
 *     prio   - (OUT) priority of entry
 *
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_PARAM      - prio pointing to NULL
 *     BCM_E_NOT_FOUND  - Entry ID not found
 */

int
bcmx_field_entry_prio_get(bcm_field_entry_t entry,
                          int *prio)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(prio);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_entry_prio_get(bcm_unit, entry, prio);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function: bcmx_field_entry_prio_set
 *
 * Purpose:
 *     Sets the priority for the given entry.
 *
 * Parameters:
 *     entry - Field entry to operate on
 *     prio  - Priority of entry
 *
 * Returns:
 *     BCM_E_NONE       Success
 */

int
bcmx_field_entry_prio_set(bcm_field_entry_t entry,
                          int prio)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_prio_set(bcm_unit, entry, prio);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_entry_policer_attach
 */
int
bcmx_field_entry_policer_attach(bcm_field_entry_t entry_id, int level,
                                bcm_policer_t policer_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_policer_attach(bcm_unit, entry_id,
                                                level, policer_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_policer_detach
 */
int
bcmx_field_entry_policer_detach(bcm_field_entry_t entry_id, int level)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_policer_detach(bcm_unit, entry_id,
                                                level);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_policer_detach_all
 */
int
bcmx_field_entry_policer_detach_all(bcm_field_entry_t entry_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_policer_detach_all(bcm_unit, entry_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_entry_policer_get
 */
int
bcmx_field_entry_policer_get(bcm_field_entry_t entry_id, int level, 
                             bcm_policer_t *policer_id)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(policer_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_entry_policer_get(bcm_unit, entry_id, level,
                                         policer_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_field_resync
 * Purpose:
 *     Invalidate HW FP entries that have not been replayed after warm boot.
 *     Can be used in general to clear entries that are in HW but not in SW.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_INIT      - Unit not initialized
 *     BCM_E_XXX       - Other errors
 *     BCM_E_NONE      - Success
 */

int
bcmx_field_resync(void)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_resync(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_clear
 * Purpose:
 *     Remove all field qualifications from a filter entry
 * Parameters:
 *     entry - Field entry to operate on
 * Returns:
 *     BCM_E_NOT_FOUND   Entry ID not found
 *     BCM_E_XXX         Error code
 *     BCM_E_NONE
 */

int
bcmx_field_qualify_clear(bcm_field_entry_t entry)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_clear(bcm_unit, entry);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InPort
 * Purpose:
 *     Add single logical port qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     port   - Logical port
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_InPort(bcm_field_entry_t entry,
                          bcmx_lport_t port)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit, unit;
    bcm_port_t    data, in_port;
    bcm_port_t    mask;

    BCMX_FIELD_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &unit, &in_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    mask = BCM_FIELD_EXACT_MATCH_MASK;

    /*
     * Set data to zero in devices that do not contain port
     * in order to override default value (which would match all ports)
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (bcm_unit == unit) {
            data = in_port;
        } else {
            data = 0;
        }
        tmp_rv = bcm_field_qualify_InPort(bcm_unit, entry, data, mask);

        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OutPort
 * Purpose:
 *     Add single logical output port qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     port   - Logical port
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_OutPort(bcm_field_entry_t entry,
                           bcmx_lport_t port)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit, unit;
    bcm_port_t    data, out_port;
    bcm_port_t    mask;

    BCMX_FIELD_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &unit, &out_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    mask = BCM_FIELD_EXACT_MATCH_MASK;


    /*
     * Set data to zero in devices that do not contain port
     * in order to override default value (which would match all ports)
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (bcm_unit == unit) {
            data = out_port;
        } else {
            data = 0;
        }
        tmp_rv = bcm_field_qualify_OutPort(bcm_unit, entry, data, mask);

        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InPorts
 * Purpose:
 *     Add the ingress ports qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     lplist - Logical ingress port list
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_InPorts(bcm_field_entry_t entry,
                           bcmx_lplist_t lplist)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_pbmp_t    data;
    bcm_pbmp_t    mask;

    BCMX_FIELD_INIT_CHECK;

    BCM_PBMP_CLEAR(mask);
    BCM_PBMP_NEGATE(mask, mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, data);
        tmp_rv = bcm_field_qualify_InPorts(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OutPorts
 */

int
bcmx_field_qualify_OutPorts(bcm_field_entry_t entry,
                            bcmx_lplist_t lplist)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_pbmp_t    data;
    bcm_pbmp_t    mask;

    BCMX_FIELD_INIT_CHECK;

    BCM_PBMP_CLEAR(mask);
    BCM_PBMP_NEGATE(mask, mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, data);
        tmp_rv = bcm_field_qualify_OutPorts(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Drop
 */

int
bcmx_field_qualify_Drop(bcm_field_entry_t entry,
                        uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Drop(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcPort
 * Purpose:
 *     Add the source logical port qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     port   - Source logical port
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_SrcPort(bcm_field_entry_t entry,
                           bcmx_lport_t port)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_module_t  data_modid, mask_modid;
    bcm_port_t    data_port, mask_port;

    BCMX_FIELD_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port,
                                  &data_modid, &data_port,
                                  BCMX_DEST_CONVERT_DEFAULT));

    mask_modid = mask_port = BCM_FIELD_EXACT_MATCH_MASK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcPort(bcm_unit, entry, 
                                           data_modid, mask_modid,
                                           data_port, mask_port);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcTrunk
 * Purpose:
 *     Add the source trunk group ID qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     tid    - Source trunk group ID
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_SrcTrunk(bcm_field_entry_t entry,
                            bcm_trunk_t tid)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_port_t    data;
    bcm_port_t    mask;

    BCMX_FIELD_INIT_CHECK;

    data = tid;
    mask = BCM_FIELD_EXACT_MATCH_MASK;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcTrunk(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstPort
 * Purpose:
 *     Add the destination logical port qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     port   - Destination logical port
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_DstPort(bcm_field_entry_t entry,
                           bcmx_lport_t port)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_module_t  data_modid, mask_modid;
    bcm_port_t    data_port, mask_port;

    BCMX_FIELD_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port,
                                  &data_modid, &data_port,
                                  BCMX_DEST_CONVERT_DEFAULT));

    mask_modid = mask_port = BCM_FIELD_EXACT_MATCH_MASK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstPort(bcm_unit, entry, 
                                           data_modid, mask_modid,
                                           data_port, mask_port);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstTrunk
 * Purpose:
 *     Add the destination trunk group ID qualification to a filter entry
 * Parameters:
 *     entry  - Field entry to operate on
 *     tid    - Destination trunk group ID
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_INTERNAL   No group exists for entry ID
 *     BCM_E_XXX
 */

int
bcmx_field_qualify_DstTrunk(bcm_field_entry_t entry,
                            bcm_trunk_t tid)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_port_t    data;
    bcm_port_t    mask;

    BCMX_FIELD_INIT_CHECK;

    data = tid;
    mask = BCM_FIELD_EXACT_MATCH_MASK;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstTrunk(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_XXX (class of routines)
 * Purpose:
 *     Add a field qualification to a filter entry.
 * Parameters:
 *     entry - Field entry to operate on
 *     data  - Data to qualify with (type varies)
 *     mask  - Mask to qualify with (type is same as for data)
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found.
 *     BCM_E_INTERNAL   No group exists for entry ID.
 *     BCM_E_XXX
 */

/*
 * Function:
 *     bcmx_field_qualify_L4SrcPort
 */

int
bcmx_field_qualify_L4SrcPort(bcm_field_entry_t entry,
                             bcm_l4_port_t data, bcm_l4_port_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L4SrcPort(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L4DstPort
 */

int
bcmx_field_qualify_L4DstPort(bcm_field_entry_t entry,
                             bcm_l4_port_t data, bcm_l4_port_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L4DstPort(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OuterVlan
 */

int
bcmx_field_qualify_OuterVlan(bcm_field_entry_t entry,
                             bcm_vlan_t data, bcm_vlan_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_OuterVlan(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OuterVlanId
 */

int
bcmx_field_qualify_OuterVlanId(bcm_field_entry_t entry,
                               bcm_vlan_t data, bcm_vlan_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_OuterVlanId(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_ForwardingVlanId
 */

int
bcmx_field_qualify_ForwardingVlanId(bcm_field_entry_t entry,
                               bcm_vlan_t data, bcm_vlan_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_ForwardingVlanId(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OuterVlanPri
 */

int
bcmx_field_qualify_OuterVlanPri(bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_OuterVlanPri(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OuterVlanCfi
 */

int
bcmx_field_qualify_OuterVlanCfi(bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_OuterVlanCfi(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerVlan
 */

int
bcmx_field_qualify_InnerVlan(bcm_field_entry_t entry,
                             bcm_vlan_t data, bcm_vlan_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerVlan(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerVlanId
 */

int
bcmx_field_qualify_InnerVlanId(bcm_field_entry_t entry,
                               bcm_vlan_t data, bcm_vlan_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerVlanId(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerVlanPri
 */

int
bcmx_field_qualify_InnerVlanPri(bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerVlanPri(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerVlanCfi
 */

int
bcmx_field_qualify_InnerVlanCfi(bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerVlanCfi(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_EtherType
 */

int
bcmx_field_qualify_EtherType(bcm_field_entry_t entry,
                             uint16 data, uint16 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_EtherType(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpProtocol
 */

int
bcmx_field_qualify_IpProtocol(bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpProtocol(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpInfo
 */

int
bcmx_field_qualify_IpInfo(bcm_field_entry_t entry,
                          uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpInfo(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_PacketRes
 */

int
bcmx_field_qualify_PacketRes(bcm_field_entry_t entry,
                             uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_PacketRes(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcIp
 */

int
bcmx_field_qualify_SrcIp(bcm_field_entry_t entry,
                         bcm_ip_t data, bcm_ip_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcIp(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstIp
 */

int
bcmx_field_qualify_DstIp(bcm_field_entry_t entry,
                         bcm_ip_t data, bcm_ip_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstIp(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DSCP
 */

int
bcmx_field_qualify_DSCP(bcm_field_entry_t entry,
                        uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DSCP(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Tos
 */

int
bcmx_field_qualify_Tos(bcm_field_entry_t entry,
                       uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Tos(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpFlags
 */

int
bcmx_field_qualify_IpFlags(bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpFlags(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_TcpControl
 */

int
bcmx_field_qualify_TcpControl(bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_TcpControl(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Ttl
 */

int
bcmx_field_qualify_Ttl(bcm_field_entry_t entry,
                       uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Ttl(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_RangeCheck
 */

int
bcmx_field_qualify_RangeCheck(bcm_field_entry_t entry,
                              int range, int result)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_RangeCheck(bcm_unit, entry, range, result);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcIp6
 */

int
bcmx_field_qualify_SrcIp6(bcm_field_entry_t entry,
                          bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcIp6(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstIp6
 */

int
bcmx_field_qualify_DstIp6(bcm_field_entry_t entry,
                          bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstIp6(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcIp6High
 */

int
bcmx_field_qualify_SrcIp6High(bcm_field_entry_t entry,
                              bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcIp6High(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstIp6High
 */

int
bcmx_field_qualify_DstIp6High(bcm_field_entry_t entry,
                              bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstIp6High(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstIp6Low
 */

int
bcmx_field_qualify_DstIp6Low(bcm_field_entry_t entry,
                              bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstIp6Low(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcIp6Low
 */

int
bcmx_field_qualify_SrcIp6Low(bcm_field_entry_t entry,
                              bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcIp6Low(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Ip6NextHeader
 */

int
bcmx_field_qualify_Ip6NextHeader(bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Ip6NextHeader(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Ip6TrafficClass
 */

int
bcmx_field_qualify_Ip6TrafficClass(bcm_field_entry_t entry,
                                   uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Ip6TrafficClass(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Ip6FlowLabel
 */

int
bcmx_field_qualify_Ip6FlowLabel(bcm_field_entry_t entry,
                                uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Ip6FlowLabel(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Ip6HopLimit
 */

int
bcmx_field_qualify_Ip6HopLimit(bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Ip6HopLimit(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcMac
 */

int
bcmx_field_qualify_SrcMac(bcm_field_entry_t entry,
                          bcm_mac_t data, bcm_mac_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcMac(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstMac
 */

int
bcmx_field_qualify_DstMac(bcm_field_entry_t entry,
                          bcm_mac_t data, bcm_mac_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstMac(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DstClassL2
 */
int
bcmx_field_qualify_DstClassL2(bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstClassL2(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_qualify_DstClassL3
 */
int
bcmx_field_qualify_DstClassL3(bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstClassL3(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_qualify_DstClassField
 */
int
bcmx_field_qualify_DstClassField(bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstClassField(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_qualify_SrcClassL2
 */
int
bcmx_field_qualify_SrcClassL2(bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcClassL2(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_qualify_SrcClassL3
 */
int
bcmx_field_qualify_SrcClassL3(bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcClassL3(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_qualify_SrcClassField
 */
int
bcmx_field_qualify_SrcClassField(bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcClassField(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpType
 */

int
bcmx_field_qualify_IpType(bcm_field_entry_t entry,
                          bcm_field_IpType_t type)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpType(bcm_unit, entry, type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerIpType
 */

int
bcmx_field_qualify_InnerIpType(bcm_field_entry_t entry, 
                               bcm_field_IpType_t type)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerIpType(bcm_unit, entry, type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_ForwardingType
 */

int
bcmx_field_qualify_ForwardingType(bcm_field_entry_t entry,
                                  bcm_field_ForwardingType_t type)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_ForwardingType(bcm_unit, entry, type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2Format
 */

int
bcmx_field_qualify_L2Format(bcm_field_entry_t entry,
                            bcm_field_L2Format_t type)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2Format(bcm_unit, entry, type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_VlanFormat
 */

int
bcmx_field_qualify_VlanFormat(bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_VlanFormat(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_TranslatedVlanFormat
 */

int
bcmx_field_qualify_TranslatedVlanFormat(bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_TranslatedVlanFormat(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_MHOpcode
 */

int
bcmx_field_qualify_MHOpcode(bcm_field_entry_t entry,
                            uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_MHOpcode(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *    bcmx_field_qualify_Decap
 */

int 
bcmx_field_qualify_Decap(bcm_field_entry_t entry,
                         bcm_field_decap_t decap)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Decap(bcm_unit, entry, decap);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_HiGig
 */

int
bcmx_field_qualify_HiGig(bcm_field_entry_t entry,
                         uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_HiGig(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_DstHiGig
 * Purpose:
 *      Qualify on packets destined to higig port.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_DstHiGig(bcm_field_entry_t entry,
                             uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstHiGig(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InterfaceClassL2
 */
int
bcmx_field_qualify_InterfaceClassL2(bcm_field_entry_t entry,
                               uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InterfaceClassL2(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InterfaceClassL3
 */
int
bcmx_field_qualify_InterfaceClassL3(bcm_field_entry_t entry,
                               uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InterfaceClassL3(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_qualify_InterfaceClassPort
 */
int
bcmx_field_qualify_InterfaceClassPort(bcm_field_entry_t entry,
                               uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InterfaceClassPort(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpProtocolCommon
 */
int
bcmx_field_qualify_IpProtocolCommon(bcm_field_entry_t entry,
                                    bcm_field_IpProtocolCommon_t protocol)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpProtocolCommon(bcm_unit, entry,
                                                    protocol);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerIpProtocolCommon
 */
int
bcmx_field_qualify_InnerIpProtocolCommon(bcm_field_entry_t entry, 
                                         bcm_field_IpProtocolCommon_t protocol)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerIpProtocolCommon(bcm_unit, entry,
                                                         protocol);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Snap
 */
int
bcmx_field_qualify_Snap(bcm_field_entry_t entry,
                        bcm_field_snap_header_t data, 
                        bcm_field_snap_header_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Snap(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Llc
 */
int
bcmx_field_qualify_Llc(bcm_field_entry_t entry,
                       bcm_field_llc_header_t data, 
                       bcm_field_llc_header_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Llc(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerTpid
 */
int
bcmx_field_qualify_InnerTpid(bcm_field_entry_t entry,
                             uint16 tpid)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerTpid(bcm_unit, entry, tpid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_OuterTpid
 */
int
bcmx_field_qualify_OuterTpid(bcm_field_entry_t entry,
                             uint16 tpid)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_OuterTpid(bcm_unit, entry, tpid);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L3Routable
 */
int
bcmx_field_qualify_L3Routable(bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L3Routable(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpFrag
 */
int bcmx_field_qualify_IpFrag(bcm_field_entry_t entry,
                              bcm_field_IpFrag_t frag_info)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpFrag(bcm_unit, entry, frag_info);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Vrf
 */
int
bcmx_field_qualify_Vrf(bcm_field_entry_t entry,
                       uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Vrf(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L3Ingress
 */
int
bcmx_field_qualify_L3Ingress(bcm_field_entry_t entry,
                       uint32 data, uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L3Ingress(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_Vpn
 */
int
bcmx_field_qualify_Vpn(bcm_field_entry_t entry,
                       bcm_vpn_t data, bcm_vpn_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Vpn(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_ExtensionHeaderType
 */
int
bcmx_field_qualify_ExtensionHeaderType(bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_ExtensionHeaderType(bcm_unit, entry,
                                                       data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_ExtensionHeaderSubCode
 */
int
bcmx_field_qualify_ExtensionHeaderSubCode(bcm_field_entry_t entry, 
                                          uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_ExtensionHeaderSubCode(bcm_unit, entry,
                                                          data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_ExtensionHeader2Type
 */
int
bcmx_field_qualify_ExtensionHeader2Type(bcm_field_entry_t entry,
                                        uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_ExtensionHeader2Type(bcm_unit, entry,
                                                        data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L4Ports
 */
int bcmx_field_qualify_L4Ports(bcm_field_entry_t entry, 
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L4Ports(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_MirrorCopy
 */
int
bcmx_field_qualify_MirrorCopy(bcm_field_entry_t entry, 
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_MirrorCopy(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_TunnelTerminated
 */
int
bcmx_field_qualify_TunnelTerminated(bcm_field_entry_t entry, 
                                    uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_TunnelTerminated(bcm_unit, entry,
                                                    data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_MplsTerminated
 */
int
bcmx_field_qualify_MplsTerminated(bcm_field_entry_t entry, 
                                  uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_MplsTerminated(bcm_unit, entry,
                                                  data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerSrcIp
 */
int
bcmx_field_qualify_InnerSrcIp(bcm_field_entry_t entry,
                              bcm_ip_t data, bcm_ip_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerSrcIp(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerDstIp
 */
int
bcmx_field_qualify_InnerDstIp(bcm_field_entry_t entry,
                              bcm_ip_t data, bcm_ip_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerDstIp(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerSrcIp6
 */
int
bcmx_field_qualify_InnerSrcIp6(bcm_field_entry_t entry,
                               bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerSrcIp6(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerDstIp6
 */
int bcmx_field_qualify_InnerDstIp6(bcm_field_entry_t entry,
                                   bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerDstIp6(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerSrcIp6High
 */
int
bcmx_field_qualify_InnerSrcIp6High(bcm_field_entry_t entry,
                                   bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerSrcIp6High(bcm_unit, entry,
                                                   data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerDstIp6High
 */
int
bcmx_field_qualify_InnerDstIp6High(bcm_field_entry_t entry, 
                                   bcm_ip6_t data, bcm_ip6_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerDstIp6High(bcm_unit, entry,
                                                   data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerTtl
 */
int
bcmx_field_qualify_InnerTtl(bcm_field_entry_t entry,
                            uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerTtl(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerTos
 */
int
bcmx_field_qualify_InnerTos(bcm_field_entry_t entry, 
                             uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerTos(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerDSCP
 */
int
bcmx_field_qualify_InnerDSCP(bcm_field_entry_t entry, 
                             uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerDSCP(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerIpProtocol
 */
int
bcmx_field_qualify_InnerIpProtocol(bcm_field_entry_t entry, 
                                   uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerIpProtocol(bcm_unit, entry,
                                                   data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerIpFrag
 */
int
bcmx_field_qualify_InnerIpFrag(bcm_field_entry_t entry, 
                               bcm_field_IpFrag_t frag_info)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerIpFrag(bcm_unit, entry, frag_info);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_DosAttack
 */
int
bcmx_field_qualify_DosAttack(bcm_field_entry_t entry, 
                             uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DosAttack(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IpmcStarGroupHit
 */
int
bcmx_field_qualify_IpmcStarGroupHit(bcm_field_entry_t entry, 
                                    uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IpmcStarGroupHit(bcm_unit, entry,
                                                    data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_MyStationHit
 */
int
bcmx_field_qualify_MyStationHit(bcm_field_entry_t entry, 
                                uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_MyStationHit(bcm_unit, entry,
                                                data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2PayloadFirstEightBytes
 */
int
bcmx_field_qualify_L2PayloadFirstEightBytes(bcm_field_entry_t entry, 
                                            uint32 data1, uint32 data2, 
                                            uint32 mask1, uint32 mask2)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2PayloadFirstEightBytes(bcm_unit, entry,
                                                            data1, data2,
                                                            mask1, mask2);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L3DestRouteHit
 */
int
bcmx_field_qualify_L3DestRouteHit(bcm_field_entry_t entry, 
                                  uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L3DestRouteHit(bcm_unit, entry,
                                                  data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L3DestHostHit
 */
int
bcmx_field_qualify_L3DestHostHit(bcm_field_entry_t entry, 
                                 uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L3DestHostHit(bcm_unit, entry,
                                                 data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L3SrcHostHit
 */
int
bcmx_field_qualify_L3SrcHostHit(bcm_field_entry_t entry, 
                                uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L3SrcHostHit(bcm_unit, entry,
                                                data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2CacheHit
 */
int
bcmx_field_qualify_L2CacheHit(bcm_field_entry_t entry, 
                              uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2CacheHit(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2StationMove
 */
int
bcmx_field_qualify_L2StationMove(bcm_field_entry_t entry, 
                                 uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2StationMove(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2DestHit
 */
int
bcmx_field_qualify_L2DestHit(bcm_field_entry_t entry, 
                             uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2DestHit(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2SrcStatic
 */
int
bcmx_field_qualify_L2SrcStatic(bcm_field_entry_t entry, 
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2SrcStatic(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_L2SrcHit
 */
int
bcmx_field_qualify_L2SrcHit(bcm_field_entry_t entry, 
                            uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_L2SrcHit(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_IngressStpState
 */
int
bcmx_field_qualify_IngressStpState(bcm_field_entry_t entry, 
                                   uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IngressStpState(bcm_unit, entry,
                                                   data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_ForwardingVlanValid
 */
int
bcmx_field_qualify_ForwardingVlanValid(bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_ForwardingVlanValid(bcm_unit, entry,
                                                       data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_SrcVirtualPortValid
 */
int
bcmx_field_qualify_SrcVirtualPortValid(bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcVirtualPortValid(bcm_unit, entry,
                                                       data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_VlanTranslationHit
 */
int
bcmx_field_qualify_VlanTranslationHit(bcm_field_entry_t entry, 
                                      uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_VlanTranslationHit(bcm_unit, entry,
                                                      data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerIp6FlowLabel
 */
int
bcmx_field_qualify_InnerIp6FlowLabel(bcm_field_entry_t entry, 
                                     uint32 data,  uint32 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerIp6FlowLabel(bcm_unit, entry,
                                                     data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerL4DstPort
 */
int
bcmx_field_qualify_InnerL4DstPort(bcm_field_entry_t entry, 
                                  bcm_l4_port_t data, 
                                  bcm_l4_port_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerL4DstPort(bcm_unit, entry,
                                                  data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qualify_InnerL4SrcPort
 */
int
bcmx_field_qualify_InnerL4SrcPort(bcm_field_entry_t entry, 
                                  bcm_l4_port_t data, 
                                  bcm_l4_port_t mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_InnerL4SrcPort(bcm_unit, entry,
                                                  data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_Loopback
 * Purpose:
 *      Add loopback field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field entry id.
 *      data - (IN) Data to qualify with.
 *      mask - (IN) Mask to qualify with.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Loopback(bcm_field_entry_t entry, 
                            uint8 data, 
                            uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Loopback(bcm_unit, entry,
                                            data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_LoopbackType
 * Purpose:
 *      Add loopback type field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field entry id.
 *      loopback_type - (IN) Loopback type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_LoopbackType(bcm_field_entry_t entry, 
                                bcm_field_LoopbackType_t loopback_type)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_LoopbackType(bcm_unit, entry,
                                            loopback_type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_field_qualify_TunnelType
 * Purpose:
 *      Add tunnel type field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field entry id.
 *      tunnel_type - (IN) Data to qualify with.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TunnelType(bcm_field_entry_t entry, 
                              bcm_field_TunnelType_t tunnel_type)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_TunnelType(bcm_unit, entry,
                                              tunnel_type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_field_qualify_DstL3Egress
 * Purpose:
 *     Add a destination egress object field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      if_id - (IN) Egress object id. 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstL3Egress(bcm_field_entry_t entry, 
                               bcm_if_t if_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstL3Egress(bcm_unit, entry,
                                               if_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}


/*
 * Function:
 *      bcmx_field_qualify_DstMulticastGroup
 * Purpose:
 *     Add a destination multicast group field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      group - (IN) Multicast group id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMulticastGroup(bcm_field_entry_t entry, 
                                     bcm_multicast_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstMulticastGroup(bcm_unit, entry,
                                                     group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcMplsGport
 * Purpose:
 *      Add source mpls port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      mpls_port_id - (IN) Mpls virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcMplsGport(bcm_field_entry_t entry, 
                                bcm_gport_t mpls_port_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcMplsGport(bcm_unit, entry,
                                                mpls_port_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}


/*
 * Function:
 *      bcmx_field_qualify_DstMplsGport
 * Purpose:
 *      Add destination mpls port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      mpls_port_id - (IN)  Mpls virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMplsGport(bcm_field_entry_t entry, 
                                bcm_gport_t mpls_port_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstMplsGport(bcm_unit, entry,
                                                mpls_port_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}


/*
 * Function:
 *      bcmx_field_qualify_SrcMimGport
 * Purpose:
 *      Add source mac in mac port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      mim_port_id - (IN) Mim virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcMimGport(bcm_field_entry_t entry, 
                               bcm_gport_t mim_port_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcMimGport(bcm_unit, entry,
                                               mim_port_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}


/*
 * Function:
 *      bcmx_field_qualify_DstMimGport
 * Purpose:
 *      Add destination mac in mac port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      mim_port_id - (IN) Mim virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMimGport(bcm_field_entry_t entry, 
                               bcm_gport_t mim_port_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstMimGport(bcm_unit, entry,
                                               mim_port_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}



/*
 * Function:
 *      bcmx_field_qualify_SrcWlanGport
 * Purpose:
 *      Add source wlan port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      wlan_port_id - (IN) Wlan virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcWlanGport(bcm_field_entry_t entry, 
                                bcm_gport_t wlan_port_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcWlanGport(bcm_unit, entry,
                                                wlan_port_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}



/*
 * Function:
 *      bcmx_field_qualify_DstWlanGport
 * Purpose:
 *      Add destination wlan port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      wlan_port_id - (IN) Wlan virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstWlanGport(bcm_field_entry_t entry, 
                                bcm_gport_t wlan_port_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstWlanGport(bcm_unit, entry,
                                                wlan_port_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcModPortGport
 * Purpose:
 *      Add source mod/port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      data  - (IN) Wlan virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcModPortGport(bcm_field_entry_t entry, 
                                   bcm_gport_t data)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcModPortGport(bcm_unit, entry,
                                                   data);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcModuleGport
 * Purpose:
 *      Add source mod/port field qualification to a field entry.
 * Parameters:
 *      entry - (IN) Field Entry id.
 *      data  - (IN) Wlan virtual port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcModuleGport(bcm_field_entry_t entry, 
                                   bcm_gport_t data)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_SrcModuleGport(bcm_unit, entry,
                                                   data);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_Color
 * Purpose:
 *      Set match criteria for bcmFieldQualifyColor
 *      qualifier in the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      color - (IN) Qualifier match color.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Color(bcm_field_entry_t entry, uint8 color)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_Color(bcm_unit, entry, color);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_IntPriority
 * Purpose:
 *      Set match criteria for bcmFieldQualifyColor
 *      qualifier in the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IntPriority(bcm_field_entry_t entry, 
                               uint8 data, uint8 mask)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_IntPriority(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_FibreChanOuter
 * Purpose:
 *      Add Fibre Channel outer header type field qualification to
 *      field entry.
 * Parameters:
 *      entry            - (IN) BCM field entry id.
 *      fibre_chan_type  - (IN) Data to qualify with.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_FibreChanOuter(bcm_field_entry_t entry,
                                  bcm_field_FibreChan_t fibre_chan_type)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_FibreChanOuter(bcm_unit, entry,
                                                  fibre_chan_type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_FibreChanInner
 * Purpose:
 *      Add Fibre Channel inner header type field qualification to
 *      field entry.
 * Parameters:
 *      entry            - (IN) BCM field entry id.
 *      fibre_chan_type  - (IN) Data to qualify with.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_FibreChanInner(bcm_field_entry_t entry,
                                  bcm_field_FibreChan_t fibre_chan_type)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_FibreChanInner(bcm_unit, entry,
                                                  fibre_chan_type);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_VnTag
 * Purpose:
 *      Add NIV VN tag field qualification to field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (IN) Qualifier match data.
 *      mask  - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_VnTag(bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_VnTag(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_CnTag
 * Purpose:
 *      Add QCN CN tag field qualification to field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (IN) Qualifier match data.
 *      mask  - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_CnTag(bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_CnTag(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_FabricQueueTag
 * Purpose:
 *      Add Fabric Queue tag field qualification to field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (IN) Qualifier match data.
 *      mask  - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_FabricQueueTag(bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_FabricQueueTag(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_DstMultipath
 * Purpose:
 *      Add a multipath egress object field qualification to a field entry.
 * Parameters:
 *      entry  - (IN) BCM field entry id.
 *      mpintf - (IN) Multipath egress object id. 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_DstMultipath(bcm_field_entry_t entry, bcm_if_t mpintf)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_DstMultipath(bcm_unit, entry, mpintf);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_action_add
 * Purpose:
 *     Add action performed when entry rule is matched for a packet
 * Parameters:
 *     entry  - Entry ID
 *     action - Action to perform (bcmFieldActionXXX)
 *     param0 - Action parameter (use 0 if not required)
 *     param1 - Action parameter (use 0 if not required)
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_MEMORY     Allocation failure
 *     BCM_E_RESOURCE   Counter not previously created for entry
 *     BCM_E_PARAM      param0 not an even index for counter
 *     BCM_E_NONE
 * Notes:
 *     For port related actions, param0 is a BCMX logical port or
 *     other GPORT type.
 */

int
bcmx_field_action_add(bcm_field_entry_t entry,
                      bcm_field_action_t action,
                      uint32 param0,
                      uint32 param1)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    /*
     * For port related actions, param0 can be either:
     *   - A BCMX lport, then translate to modid/port
     *   - Another gport type, such as trunk
     */
    if ((action == bcmFieldActionRedirectPort) ||
        (action == bcmFieldActionRedirect) ||
        (action == bcmFieldActionMirrorIngress) ||
        (action == bcmFieldActionMirrorEgress)) {
        bcm_module_t  modid;
        bcm_port_t    modport;

        /*
         * When param0 is a valid GPORT, but it is NOT of of MODPORT type,
         * the conversion to modid/port will fail.  However, the GPORT
         * value may be a valid data for certain devices (such as ESW)
         * where a trunk (in gport format) is being passed in.
         *
         * NOTE that the BCM API expects the gport data in the
         * place where the 'port' parameter is normally passed in.
         *
         * This is a bit uggly,
         * but we must accomodate different architectures and
         * the partial support of gport formats.
         */
        if (BCM_SUCCESS(_bcmx_dest_to_modid_port((bcmx_lport_t)param0,
                                                 &modid, &modport,
                                                 BCMX_DEST_CONVERT_DEFAULT))) {
            param0 = (uint32)modid;
            param1 = (uint32)modport;
        } else {
            /* Assign gport value to expected 'param1' in BCM */
            param1 = param0;
        }
    }
    
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_action_add(bcm_unit, entry, action, 
                                      param0, param1);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_action_delete
 * Purpose:
 *     Delete an action performed when entry rule is matched for a packet
 * Parameters:
 *     entry  - Entry ID
 *     action - Action to perform (bcmFieldActionXXX)
 *     param0 - Action parameter (use 0 if not required)
 *     param1 - Action parameter (use 0 if not required)
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_MEMORY     Allocation failure
 *     BCM_E_RESOURCE   Counter not previously created for entry
 *     BCM_E_PARAM      param0 not an even index for counter
 *     BCM_E_NONE
 * Notes:
 *     For port related actions, param0 is a BCMX logical port or
 *     other GPORT type.
 */

int
bcmx_field_action_delete(bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 param0,
                         uint32 param1)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    /*
     * For port related actions, param0 can be either:
     *   - A BCMX lport, then translate to modid/port
     *   - Another gport type, such as trunk
     */
    if ((action == bcmFieldActionRedirectPort) ||
        (action == bcmFieldActionRedirect) ||
        (action == bcmFieldActionMirrorIngress) ||
        (action == bcmFieldActionMirrorEgress)) {
        int         modid;
        bcm_port_t  modport;

        /*
         * When param0 is a valid GPORT, but it is NOT of of MODPORT type,
         * the conversion to modid/port will fail.  However, the GPORT
         * value may be a valid data for certain devices (such as ESW)
         * where a trunk (in gport format) is being passed in.
         *
         * NOTE that the BCM API expects the gport data in the
         * place where the 'port' parameter is normally passed in.
         *
         * This is a bit uggly,
         * but we must accomodate different architectures and
         * the partial support of gport formats.
         */
        if (BCM_SUCCESS(_bcmx_dest_to_modid_port((bcmx_lport_t)param0,
                                                 &modid, &modport,
                                                 BCMX_DEST_CONVERT_DEFAULT))) {
            param0 = (uint32)modid;
            param1 = (uint32)modport;
        } else {
            /* Assign gport value to expected argument 'param1' for BCM */
            param1 = param0;
        }
    }
    
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_action_delete(bcm_unit, entry, action, 
                                         param0, param1);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_action_mac_add
 * Purpose:
 *     Add mac related actions to field entry.
 * Parameters:
 *     entry  - Entry ID
 *     action - Action (bcmFieldActionSrcMacNew or 
 *                      bcmFieldActionDstMacNew
 *     mac    - Mac address.
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_MEMORY     Allocation failure
 *     BCM_E_RESOURCE   
 *     BCM_E_PARAM      
 *     BCM_E_NONE
 */
int
bcmx_field_action_mac_add(bcm_field_entry_t entry,
                            bcm_field_action_t action,
                            bcm_mac_t mac)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_action_mac_add(bcm_unit, entry, action, mac);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *     bcmx_field_action_mac_get
 * Purpose:
 *     Get field action SrcMacNew/DstMacNew parameters.
 * Parameters:
 *     entry  - Entry ID
 *     action - Action (bcmFieldActionSrcMacNew or 
 *                      bcmFieldActionDstMacNew
 *     mac    - (OUT) Mac address.
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - incorrect action OR pbmp is NULL
 *     BCM_E_NONE      - Success
 */
int
bcmx_field_action_mac_get(bcm_field_entry_t entry,
                            bcm_field_action_t action,
                            bcm_mac_t *mac)
{
    int           rv;
    int           i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mac);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_action_mac_get(bcm_unit, entry, action, mac);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_action_ports_add
 * Purpose:
 *     Add PBMP related actions, when entry rule is matched for a packet
 * Parameters:
 *     entry  - Entry ID
 *     action - Action to perform (bcmFieldActionRedirectPbmp or 
 *                                 bcmFieldActionEgressMask
 *     lplist - Logical Port list
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_MEMORY     Allocation failure
 *     BCM_E_RESOURCE   
 *     BCM_E_PARAM      
 *     BCM_E_NONE
 * Notes:
 *     This API has limitations when dealing with remote ports
 *     for certain topologies.
 *     FP action is applied to ports from given list that are
 *     local to the devices where packets are ingressing.
 */
int
bcmx_field_action_ports_add(bcm_field_entry_t entry,
                            bcm_field_action_t action,
                            bcmx_lplist_t lplist)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_pbmp_t    pbmp;

    BCMX_FIELD_INIT_CHECK;

    if (bcmx_lplist_is_null(&lplist)) {
        return BCM_E_PARAM;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        tmp_rv = bcm_field_action_ports_add(bcm_unit, entry, action, pbmp);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_action_ports_get
 * Purpose:
 *     Get parameters associated with port list related entry actions
 * Parameters:
 *     entry  - Entry ID
 *     action - Action (bcmFieldActionRedirectPbmp or 
 *                      bcmFieldActionEgressMask
 *     lplist - (OUT) Logical Port list
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - incorrect action OR pbmp is NULL
 *     BCM_E_NONE      - Success
 */
int
bcmx_field_action_ports_get(bcm_field_entry_t entry,
                            bcm_field_action_t action,
                            bcmx_lplist_t *lplist)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_pbmp_t    pbmp;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(lplist);

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCM_PBMP_CLEAR(pbmp);    
        tmp_rv = bcm_field_action_ports_get(bcm_unit, entry, action, &pbmp);

        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbmp);
            } else {
                break;
            }
        }
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_action_get
 * Purpose:
 *     Get parameters associated with an entry action
 * Parameters:
 *     entry  - Entry ID
 *     action - Action to perform (bcmFieldActionXXX)
 *     param0 - (OUT) Action parameter
 *     param1 - (OUT) Action parameter
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID or action not found
 *     BCM_E_PARAM      paramX is NULL
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call.
 *     For port related actions, param0 returns a BCMX logical port.
 */

int
bcmx_field_action_get(bcm_field_entry_t entry,
                      bcm_field_action_t action,
                      uint32 *param0,
                      uint32 *param1)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(param0);
    BCMX_PARAM_NULL_CHECK(param1);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_action_get(bcm_unit, entry, action, 
                                  param0, param1);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            if (BCM_SUCCESS(rv)) {
                /*
                 * For port related actions, param0 returns an lport,
                 * translate modid/port to lport
                 */
                if ((action == bcmFieldActionRedirectPort) ||
                    (action == bcmFieldActionRedirect) ||
                    (action == bcmFieldActionMirrorIngress) ||
                    (action == bcmFieldActionMirrorEgress)) {
                    bcmx_lport_t  port;

                    BCM_IF_ERROR_RETURN
                        (_bcmx_dest_from_modid_port(&port,
                                                    (bcm_module_t)(*param0),
                                                    (bcm_port_t)(*param1),
                                                    BCMX_DEST_CONVERT_DEFAULT));
                    *param0 = (uint32)port;
                    *param1 = 0;
                }
            }
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_action_remove
 * Purpose:
 *     Remove an action performed when entry rule is matched for
 *     a packet
 * Parameters:
 *     entry  - Entry ID
 *     action - Action to remove (bcmFieldActionXXX)
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID or action not found
 *     BCM_E_PARAM      Action out of valid range
 *     BCM_E_NONE
 */

int
bcmx_field_action_remove(bcm_field_entry_t entry,
                         bcm_field_action_t action)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_action_remove(bcm_unit, entry, action);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_action_remove_all
 * Purpose:
 *     Remove all actions from an entry rule
 * Parameters:
 *     entry - Entry ID
 * Returns:
 *     BCM_E_NOT_FOUND  Entry ID not found
 *     BCM_E_NONE
 */

int
bcmx_field_action_remove_all(bcm_field_entry_t entry)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_action_remove_all(bcm_unit, entry);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function: 
 *     bcmx_field_stat_create
 * Description:
 *     Create statistics collection entity.
 * Parameters:
 *     group    - Field group id. 
 *     nstat    - Number of elements in stat array.
 *     stat_arr - Collected statistics descriptors array.
 *     stat_id  - (OUT) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_field_stat_create(bcm_field_group_t group, int nstat,
                       bcm_field_stat_t *stat_arr, int *stat_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    int  id;

    BCMX_FIELD_INIT_CHECK;

    if (nstat != 0) {
        BCMX_PARAM_NULL_CHECK(stat_arr);
    }
    BCMX_PARAM_NULL_CHECK(stat_id);

    id = STAT_ID_ALLOC;
    *stat_id = id;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_create_id(bcm_unit, group,
                                          nstat, stat_arr, id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_create_id
 * Description:
 *     Create statistics collection entity with given id.
 * Parameters:
 *     group    - Field group id. 
 *     nstat    - Number of elements in stat array.
 *     stat_arr - Collected statistics descriptors array.
 *     stat_id  - Statistics entity id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_create_id(bcm_field_group_t group, int nstat,
                          bcm_field_stat_t *stat_arr, int stat_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    if (nstat != 0) {
        BCMX_PARAM_NULL_CHECK(stat_arr);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_create_id(bcm_unit, group,
                                          nstat, stat_arr, stat_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_destroy
 * Description:
 *     Destroy statistics collection entity.
 * Parameters:
 *     stat_id  - Statistics entity id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_destroy(int stat_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_destroy(bcm_unit, stat_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_size
 * Description:
 *     Get number of different statistics associated with statistics
 *     collection entity.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     stat_size - (OUT) Number of collected statistics
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_size(int stat_id, int *stat_size)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_size);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_stat_size(bcm_unit, stat_id, stat_size);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function: 
 *     bcmx_field_stat_config_get
 * Description:
 *     Get enabled statistics for specific collection entity.
 * Parameters:
 *     stat_id  - Statistics entity id.
 *     nstat    - Number of elements in stat array.
 *     stat_arr - (OUT) Collected statistics descriptors array.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_config_get(int stat_id, int nstat, bcm_field_stat_t *stat_arr)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    if (nstat != 0) {
        BCMX_PARAM_NULL_CHECK(stat_arr);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_stat_config_get(bcm_unit, stat_id, nstat, stat_arr);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function: 
 *     bcmx_field_stat_set
 * Description:
 *     Set 64 bit counter value for specific statistic type.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     stat      - Collected statistics descriptor.
 *     value     - Value to set.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_set(int stat_id, bcm_field_stat_t stat, uint64 value)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_set(bcm_unit, stat_id, stat, value);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_set32
 * Description:
 *     Set lower 32 bit counter value for specific statistic type.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     stat      - Collected statistics descriptor.
 *     value     - Value to set.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_set32(int stat_id, bcm_field_stat_t stat, uint32 value)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_set32(bcm_unit, stat_id, stat, value);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_all_set
 * Description:
 *     Set 64 bit counter values for all statistic types.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     value     - Value to set.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_all_set(int stat_id, uint64 value)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_all_set(bcm_unit, stat_id, value);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_all_set32
 * Description:
 *     Set lower 32 bit counter values for all statistic types.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     value     - Value to set.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_all_set32(int stat_id, uint32 value)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_all_set32(bcm_unit, stat_id, value);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_get
 * Description:
 *     Get 64 bit counter value for specific statistic type.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     stat      - Collected statistics descriptor.
 *     value     - (OUT) Collected counters value.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_get(int stat_id, bcm_field_stat_t stat, uint64 *value)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint64  tmp_val;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    COMPILER_64_ZERO(*value);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_get(bcm_unit, stat_id, stat, &tmp_val);

        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                COMPILER_64_ADD_64(*value, tmp_val);
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_get32
 * Description:
 *     Get lower 32 bit counter value for specific statistic type.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     stat      - Collected statistics descriptor.
 *     value     - (OUT) Collected counters value.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_get32(int stat_id, bcm_field_stat_t stat, uint32 *value)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint32  tmp_val;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    *value = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_get32(bcm_unit, stat_id, stat, &tmp_val);

        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                *value += tmp_val;
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_multi_get
 * Description:
 *     Get 64 bit counter values for multiple statistic types.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     nstat     - Number of elements in stat array.
 *     stat_arr  - Collected statistics descriptors array.
 *     value_arr - (OUT) Collected counters values.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_multi_get(int stat_id, int nstat, 
                          bcm_field_stat_t *stat_arr, uint64 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    int     j;
    uint64  *tmp_value_arr;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    tmp_value_arr = sal_alloc(sizeof(uint64) * nstat, "BCMX field stat");
    if (tmp_value_arr == NULL) {
        return BCM_E_MEMORY;
    }

    for (j = 0; j < nstat; j++) {
        COMPILER_64_ZERO(value_arr[j]);
    }

    /* Gather statistic values from all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_multi_get(bcm_unit, stat_id, nstat,
                                          stat_arr, tmp_value_arr);

        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (j = 0; j < nstat; j++) {
                    COMPILER_64_ADD_64(value_arr[j], tmp_value_arr[j]);
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_value_arr);

    return rv;
}

/*
 * Function: 
 *     bcmx_field_stat_multi_get32
 * Description:
 *     Get lower 32 bit counter values for multiple statistic types.
 * Parameters:
 *     stat_id   - Statistics entity id.
 *     nstat     - Number of elements in stat array.
 *     stat_arr  - Collected statistics descriptors array.
 *     value_arr - (OUT) Collected counters values.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_stat_multi_get32(int stat_id, int nstat, 
                            bcm_field_stat_t *stat_arr, uint32 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    int     j;
    uint32  *tmp_value_arr;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    tmp_value_arr = sal_alloc(sizeof(uint32) * nstat, "BCMX field stat");
    if (tmp_value_arr == NULL) {
        return BCM_E_MEMORY;
    }

    for (j = 0; j < nstat; j++) {
        value_arr[j] = 0;
    }

    /* Gather statistic values from all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_stat_multi_get32(bcm_unit, stat_id, nstat,
                                            stat_arr, tmp_value_arr);

        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (j = 0; j < nstat; j++) {
                    value_arr[j] += tmp_value_arr[j];
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_value_arr);

    return rv;
}

/*
 * Function: 
 *     bcmx_field_entry_stat_attach
 * Description:
 *     Attach statistics entity to Field Processor entry.
 * Parameters:
 *     entry     - Field entry id. 
 *     stat_id   - Statistics entity id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_entry_stat_attach(bcm_field_entry_t entry, int stat_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_stat_attach(bcm_unit, entry, stat_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_entry_stat_detach
 * Description:
 *     Detach statistics entity from Field Processor entry.
 * Parameters:
 *     entry     - Field entry id. 
 *     stat_id   - Statistics entity id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_entry_stat_detach(bcm_field_entry_t entry, int stat_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_entry_stat_detach(bcm_unit, entry, stat_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function: 
 *     bcmx_field_entry_stat_get
 * Description:
 *     Get statistics entity attached to Field Processor entry.  
 * Parameters:
 *     entry     - Field entry id. 
 *     stat_id   - (OUT) Statistics entity id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_field_entry_stat_get(bcm_field_entry_t entry, int *stat_id)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_entry_stat_get(bcm_unit, entry, stat_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_field_group_compress
 * Description:
 *     Compresses field group entries.
 * Parameters:
 *     group       - Field group ID
 * Returns:
 *     BCM_E_XXX   - some error
 *     BCM_E_NONE
 */

int
bcmx_field_group_compress(bcm_field_group_t group)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_compress(bcm_unit, group);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_field_group_priority_set
 * Description:
 *     Set the group priority.
 * Parameters:
 *     group       - Group ID
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 */

int
bcmx_field_group_priority_set(bcm_field_group_t group, int priority)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_group_priority_set(bcm_unit, group, priority);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_group_priority_get
 * Description:
 *     Get the group priority.
 * Parameters:
 *     group       - Group ID
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     Get value from first successful call
 */

int
bcmx_field_group_priority_get(bcm_field_group_t group, int *priority)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(priority);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_group_priority_get(bcm_unit, group, priority);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

void bcmx_field_data_qualifier_t_init(bcmx_field_data_qualifier_t *data_qual)
{
    bcm_field_data_qualifier_t_init((bcm_field_data_qualifier_t *)data_qual);
}

void bcmx_field_data_ethertype_t_init(bcmx_field_data_ethertype_t *etype)
{
    bcm_field_data_ethertype_t_init((bcm_field_data_ethertype_t *)etype);
}


void bcmx_field_data_ip_protocol_t_init(bcmx_field_data_ip_protocol_t
                                        *ip_protocol)
{
    bcm_field_data_ip_protocol_t_init
        ((bcm_field_data_ip_protocol_t *)ip_protocol);
}

void bcmx_field_data_packet_format_t_init(bcmx_field_data_packet_format_t
                                          *packet_format)
{
    bcm_field_data_packet_format_t_init
        ((bcm_field_data_packet_format_t *)packet_format);
}

int bcmx_field_data_qualifier_create(bcmx_field_data_qualifier_t
                                     *data_qualifier)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;
    uint32  flags_orig;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data_qualifier);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_create(bcm_unit,data_qualifier);
        if (BCM_FAILURE(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * If qualifier id is not specified, use returned id from
         * first successful 'create' for remaining units.
         */
        if (!(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                data_qualifier->flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    data_qualifier->flags &= ~BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *      bcmx_field_data_qualifier_multi_get
 * Purpose:
 *      Return list of ids of defined data qualifiers.
 * Parameters:
 *      qual_size  - (IN)  Size of given qualifier id array; if 0, indicates
 *                         return count of data qualifiers only.
 *      qual_array - (OUT) Base of array where to store returned data qualifier
 *                         ids.
 *      qual_count - (OUT) Number of qualifier ids stored in above array; if
 *                         qual_size was given as 0, then number of defined
 *                         qualifiers. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_field_data_qualifier_multi_get(int qual_size,
                                    int *qual_array, int *qual_count)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(qual_size, qual_array);
    BCMX_PARAM_NULL_CHECK(qual_count);

    /*
     * No need to gather qualifier ids from all units.  BCMX assumes
     * that field qualifier ids were created with BCMX APIs.
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_data_qualifier_multi_get(bcm_unit, qual_size,
                                                qual_array, qual_count);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_data_qualifier_get
 * Purpose:
 *      Return configuration of given data qualifier.
 * Parameters:
 *      qual_id    - (IN)  Id of data qualifier.
 *      qual       - (OUT) Attributes of given data qualifier.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_field_data_qualifier_get(int qual_id, 
                              bcmx_field_data_qualifier_t *qual)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(qual);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_data_qualifier_get(bcm_unit, qual_id, 
                                          (bcm_field_data_qualifier_t *)qual);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int bcmx_field_data_qualifier_destroy(int qual_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_destroy(bcm_unit, qual_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_destroy_all(void)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_ethertype_add(int qual_id,
                                            bcmx_field_data_ethertype_t *etype)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(etype);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_ethertype_add
            (bcm_unit, qual_id, (bcm_field_data_ethertype_t *)etype);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_ethertype_delete(int qual_id,
                                               bcmx_field_data_ethertype_t
                                               *etype)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(etype);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_ethertype_delete
            (bcm_unit, qual_id, (bcm_field_data_ethertype_t *)etype);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_ip_protocol_add(int qual_id,
                                              bcmx_field_data_ip_protocol_t
                                              *ip_protocol)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ip_protocol);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_ip_protocol_add
            (bcm_unit, qual_id, (bcm_field_data_ip_protocol_t *)ip_protocol);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_ip_protocol_delete(int qual_id,
                                                 bcmx_field_data_ip_protocol_t
                                                 *ip_protocol)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ip_protocol);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_ip_protocol_delete
            (bcm_unit, qual_id, (bcm_field_data_ip_protocol_t *)ip_protocol);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_packet_format_add(int qual_id,
                                                bcmx_field_data_packet_format_t
                                                *packet_format)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(packet_format);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_packet_format_add
            (bcm_unit, qual_id,
             (bcm_field_data_packet_format_t *)packet_format);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_data_qualifier_packet_format_delete(int qual_id,
        bcmx_field_data_packet_format_t *packet_format)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(packet_format);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_data_qualifier_packet_format_delete
            (bcm_unit, qual_id,
             (bcm_field_data_packet_format_t *)packet_format);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_qset_data_qualifier_add(bcm_field_qset_t *qset, int qual_id)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(qset);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qset_data_qualifier_add(bcm_unit, qset, qual_id);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_field_qualify_data(bcm_field_entry_t eid, int qual_id,
                            uint8 *data, uint8 *mask, uint16 length)
{
    int    rv = BCM_E_UNAVAIL, tmp_rv;
    int    i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_data(bcm_unit, eid, qual_id,
                                        data, mask, length);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_field_qset_data_qualifier_get
 */
int 
bcmx_field_qset_data_qualifier_get(bcm_field_qset_t *qset,
                                   int qual_max, int *qual_arr,
                                   int *qual_count)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(qset);
    BCMX_PARAM_NULL_CHECK(qual_count);
    BCMX_PARAM_ARRAY_NULL_CHECK(qual_max, qual_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qset_data_qualifier_get(bcm_unit, *qset,
                                               qual_max, qual_arr,
                                               qual_count);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_field_qualify_LoopbackType_get
 * Purpose:
 *      Get loopback type field qualification from a field entry.
 * Parameters:
 *      entry - (IN) Field entry id.
 *      loopback_type - (OUT) Loopback type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_LoopbackType_get(bcm_field_entry_t entry, 
                                    bcm_field_LoopbackType_t *loopback_type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(loopback_type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_LoopbackType_get(bcm_unit, entry, loopback_type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_TunnelType_get
 * Purpose:
 *      Get tunnel type field qualification from a field entry.
 * Parameters:
 *      entry - (IN) Field entry id.
 *      tunnel_type - (OUT) Tunnel type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TunnelType_get(bcm_field_entry_t entry, 
                                  bcm_field_TunnelType_t *tunnel_type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tunnel_type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_TunnelType_get(bcm_unit, entry, tunnel_type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstL3Egress_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstL3Egress
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      if_id - (OUT) L3 forwarding object.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstL3Egress_get(bcm_field_entry_t entry, 
                                   bcm_if_t *if_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(if_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstL3Egress_get(bcm_unit, entry, if_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstMulticastGroup_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstMulticastGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      group - (OUT) Multicast group id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMulticastGroup_get(bcm_field_entry_t entry, 
                                         bcm_multicast_t *group)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(group);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstMulticastGroup_get(bcm_unit, entry, group);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Color_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyColor
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      color - (OUT) Qualifier match color.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Color_get(bcm_field_entry_t entry, uint8 *color)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(color);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Color_get(bcm_unit, entry, color);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IntPriority_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIntPriority
 *      qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IntPriority_get(bcm_field_entry_t entry, 
                                   uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IntPriority_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcModPortGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcModPortGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Mod/port gport id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcModPortGport_get(bcm_field_entry_t entry, 
                                       bcm_gport_t *data)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcModPortGport_get(bcm_unit, entry, data);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcModuleGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcModuleGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Mod/port gport id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcModuleGport_get(bcm_field_entry_t entry, 
                                       bcm_gport_t *data)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcModuleGport_get(bcm_unit, entry, data);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcMplsGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcMplsGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      mpls_port_id - (OUT) Mpls port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcMplsGport_get(bcm_field_entry_t entry, 
                                    bcm_gport_t *mpls_port_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mpls_port_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcMplsGport_get(bcm_unit, entry, mpls_port_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_field_qualify_DstMplsGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstMplsGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      mpls_port_id - (OUT) Mpls port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMplsGport_get(bcm_field_entry_t entry, 
                                    bcm_gport_t *mpls_port_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mpls_port_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstMplsGport_get(bcm_unit, entry, mpls_port_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcMimGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcMimGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      mim_port_id - (OUT) Mim port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcMimGport_get(bcm_field_entry_t entry, 
                                   bcm_gport_t *mim_port_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mim_port_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcMimGport_get(bcm_unit, entry, mim_port_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstMimGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstMimGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      mim_port_id - (OUT) Mim port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMimGport_get(bcm_field_entry_t entry, 
                                   bcm_gport_t *mim_port_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mim_port_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstMimGport_get(bcm_unit, entry, mim_port_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcWlanGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcWlanGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      wlan_port_id - (OUT) Wlan port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcWlanGport_get(bcm_field_entry_t entry, 
                                    bcm_gport_t *wlan_port_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(wlan_port_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcWlanGport_get(bcm_unit, entry, wlan_port_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstWlanGport_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstWlanGport
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      wlan_port_id - (OUT) Wlan port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstWlanGport_get(bcm_field_entry_t entry, 
                                    bcm_gport_t *wlan_port_id)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(wlan_port_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstWlanGport_get(bcm_unit, entry, wlan_port_id);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Loopback_get
 * Purpose:
 *      Get loopback field qualification from  a field entry.
 * Parameters:
 *      entry - (IN) Field entry id.
 *      data - (OUT) Data to qualify with.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Loopback_get(bcm_field_entry_t entry, 
                                uint8 *data, 
                                uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Loopback_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InPort_get(bcm_field_entry_t entry, 
                              bcm_port_t *data, 
                              bcm_port_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OutPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOutPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OutPort_get(bcm_field_entry_t entry, 
                               bcm_port_t *data, 
                               bcm_port_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OutPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InPorts_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInPorts
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InPorts_get(bcm_field_entry_t entry, 
                               bcm_pbmp_t *data, 
                               bcm_pbmp_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InPorts_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OutPorts_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOutPorts
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OutPorts_get(bcm_field_entry_t entry, 
                                bcm_pbmp_t *data, 
                                bcm_pbmp_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OutPorts_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Drop_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDrop
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Drop_get(bcm_field_entry_t entry, 
                            uint8 *data, 
                            uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Drop_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data_modid - (OUT) Qualifier module id.
 *      mask_modid - (OUT) Qualifier module id mask.
 *      data_port - (OUT) Qualifier port id.
 *      mask_port - (OUT) Qualifier port id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcPort_get(bcm_field_entry_t entry, 
                               bcm_module_t *data_modid, 
                               bcm_module_t *mask_modid, 
                               bcm_port_t *data_port, 
                               bcm_port_t *mask_port)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data_modid);
    BCMX_PARAM_NULL_CHECK(mask_modid);
    BCMX_PARAM_NULL_CHECK(data_port);
    BCMX_PARAM_NULL_CHECK(mask_port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcPort_get(bcm_unit, entry, data_modid, mask_modid, data_port, mask_port);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcTrunk_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcTrunk
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcTrunk_get(bcm_field_entry_t entry, 
                                bcm_trunk_t *data, 
                                bcm_trunk_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcTrunk_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data_modid - (OUT) Qualifier module id.
 *      mask_modid - (OUT) Qualifier module id mask.
 *      data_port - (OUT) Qualifier port id.
 *      mask_port - (OUT) Qualifier port id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstPort_get(bcm_field_entry_t entry, 
                               bcm_module_t *data_modid, 
                               bcm_module_t *mask_modid, 
                               bcm_port_t *data_port, 
                               bcm_port_t *mask_port)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data_modid);
    BCMX_PARAM_NULL_CHECK(mask_modid);
    BCMX_PARAM_NULL_CHECK(data_port);
    BCMX_PARAM_NULL_CHECK(mask_port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstPort_get(bcm_unit, entry, data_modid, mask_modid, data_port, mask_port);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstTrunk_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstTrunk
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstTrunk_get(bcm_field_entry_t entry, 
                                bcm_trunk_t *data, 
                                bcm_trunk_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstTrunk_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerL4SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerL4SrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerL4SrcPort_get(bcm_field_entry_t entry, 
                                      bcm_l4_port_t *data, 
                                      bcm_l4_port_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerL4SrcPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerL4DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerL4DstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerL4DstPort_get(bcm_field_entry_t entry, 
                                      bcm_l4_port_t *data, 
                                      bcm_l4_port_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerL4DstPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L4SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL4SrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L4SrcPort_get(bcm_field_entry_t entry, 
                                 bcm_l4_port_t *data, 
                                 bcm_l4_port_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L4SrcPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L4DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL4DstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L4DstPort_get(bcm_field_entry_t entry, 
                                 bcm_l4_port_t *data, 
                                 bcm_l4_port_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L4DstPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OuterVlan_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOuterVlan
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OuterVlan_get(bcm_field_entry_t entry, 
                                 bcm_vlan_t *data, 
                                 bcm_vlan_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OuterVlan_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OuterVlanId_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOuterVlanId
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OuterVlanId_get(bcm_field_entry_t entry, 
                                   bcm_vlan_t *data, 
                                   bcm_vlan_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OuterVlanId_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_ForwardingVlanId_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyForwardingVlanId
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_ForwardingVlanId_get(bcm_field_entry_t entry, 
                                   bcm_vlan_t *data, 
                                   bcm_vlan_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_ForwardingVlanId_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OuterVlanCfi_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOuterVlanCfi
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OuterVlanCfi_get(bcm_field_entry_t entry, 
                                   uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OuterVlanCfi_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OuterVlanPri_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOuterVlanPri
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OuterVlanPri_get(bcm_field_entry_t entry, 
                                   uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OuterVlanPri_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerVlan_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerVlan
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerVlan_get(bcm_field_entry_t entry, 
                                 bcm_vlan_t *data, 
                                 bcm_vlan_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerVlan_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerVlanId_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerVlanId
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerVlanId_get(bcm_field_entry_t entry, 
                                   bcm_vlan_t *data, 
                                   bcm_vlan_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerVlanId_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerVlanPri_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerVlanPri
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerVlanPri_get(bcm_field_entry_t entry, 
                                   uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerVlanPri_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerVlanCfi_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerVlanCfi
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerVlanCfi_get(bcm_field_entry_t entry, 
                                   uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerVlanCfi_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_EtherType_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyEtherType
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_EtherType_get(bcm_field_entry_t entry, 
                                 uint16 *data, 
                                 uint16 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_EtherType_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IpProtocol_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpProtocol
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpProtocol_get(bcm_field_entry_t entry, 
                                  uint8 *data, 
                                  uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpProtocol_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_field_qualify_IpInfo_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpInfo
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpInfo_get(bcm_field_entry_t entry, 
                              uint32 *data, 
                              uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpInfo_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_PacketRes_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyPacketRes
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_PacketRes_get(bcm_field_entry_t entry, 
                                 uint32 *data, 
                                 uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_PacketRes_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcIp_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcIp
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcIp_get(bcm_field_entry_t entry, 
                             bcm_ip_t *data, 
                             bcm_ip_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcIp_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstIp_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstIp_get(bcm_field_entry_t entry, 
                             bcm_ip_t *data, 
                             bcm_ip_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstIp_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DSCP_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DSCP_get(bcm_field_entry_t entry, 
                            uint8 *data, 
                            uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DSCP_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Tos_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTos
 *      qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_Tos_get(bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Tos_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IpFlags_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpFlags
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpFlags_get(bcm_field_entry_t entry, 
                               uint8 *data, 
                               uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpFlags_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_TcpControl_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTcpControl
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TcpControl_get(bcm_field_entry_t entry, 
                                  uint8 *data, 
                                  uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_TcpControl_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_TcpSequenceZero_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTcpSequenceZero
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier match flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TcpSequenceZero_get(bcm_field_entry_t entry, 
                                       uint32 *flag)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flag);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_TcpSequenceZero_get(bcm_unit, entry, flag);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_TcpHeaderSize_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTcpHeaderSize
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TcpHeaderSize_get(bcm_field_entry_t entry, 
                                     uint8 *data, 
                                     uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_TcpHeaderSize_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Ttl_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTtl
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Ttl_get(bcm_field_entry_t entry, 
                           uint8 *data, 
                           uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Ttl_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_RangeCheck_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyRangeCheck
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      max_count - (IN) Max entries to fill.
 *      range - (OUT) Range checkers array.
 *      invert - (OUT) Range checkers invert array.
 *      count - (OUT) Number of filled range checkers.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_RangeCheck_get(bcm_field_entry_t entry, 
                                  int max_count, 
                                  bcm_field_range_t *range, 
                                  int *invert, 
                                  int *count)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(count);
    BCMX_PARAM_ARRAY_NULL_CHECK(max_count, range);
    BCMX_PARAM_ARRAY_NULL_CHECK(max_count, invert);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_RangeCheck_get(bcm_unit, entry, max_count, range, invert, count);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcIp6_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcIp6_get(bcm_field_entry_t entry, 
                              bcm_ip6_t *data, 
                              bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcIp6_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstIp6_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstIp6_get(bcm_field_entry_t entry, 
                              bcm_ip6_t *data, 
                              bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstIp6_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcIp6High_get(bcm_field_entry_t entry, 
                                  bcm_ip6_t *data, 
                                  bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcIp6High_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcIp6Low_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcIp6Low
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcIp6Low_get(bcm_field_entry_t entry, 
                                  bcm_ip6_t *data, 
                                  bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcIp6Low_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstIp6Low_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstIp6Low
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstIp6Low_get(bcm_field_entry_t entry, 
                                  bcm_ip6_t *data, 
                                  bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstIp6Low_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstIp6High_get(bcm_field_entry_t entry, 
                                  bcm_ip6_t *data, 
                                  bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstIp6High_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Ip6NextHeader_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIp6NextHeader
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Ip6NextHeader_get(bcm_field_entry_t entry, 
                                     uint8 *data, 
                                     uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Ip6NextHeader_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Ip6TrafficClass_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIp6TrafficClass
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Ip6TrafficClass_get(bcm_field_entry_t entry, 
                                       uint8 *data, 
                                       uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Ip6TrafficClass_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerIp6FlowLabel_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerIp6FlowLabel
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerIp6FlowLabel_get(bcm_field_entry_t entry, 
                                         uint32 *data, 
                                         uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerIp6FlowLabel_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Ip6FlowLabel_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIp6FlowLabel
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Ip6FlowLabel_get(bcm_field_entry_t entry, 
                                    uint32 *data, 
                                    uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Ip6FlowLabel_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Ip6HopLimit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIp6HopLimit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Ip6HopLimit_get(bcm_field_entry_t entry, 
                                   uint8 *data, 
                                   uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Ip6HopLimit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcMac_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcMac
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcMac_get(bcm_field_entry_t entry, 
                              bcm_mac_t *data, 
                              bcm_mac_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcMac_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstMac_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstMac
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstMac_get(bcm_field_entry_t entry, 
                              bcm_mac_t *data, 
                              bcm_mac_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstMac_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerIpType_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerIpType
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Inner ip header ip type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerIpType_get(bcm_field_entry_t entry, 
                                   bcm_field_IpType_t *type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerIpType_get(bcm_unit, entry, type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_ForwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_ForwardingType_get(bcm_field_entry_t entry, 
                                      bcm_field_ForwardingType_t *type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_ForwardingType_get(bcm_unit, entry, type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IpType_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpType
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match ip type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpType_get(bcm_field_entry_t entry, 
                              bcm_field_IpType_t *type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpType_get(bcm_unit, entry, type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2Format_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2Format
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match l2 format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L2Format_get(bcm_field_entry_t entry, 
                                bcm_field_L2Format_t *type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2Format_get(bcm_unit, entry, type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_VlanFormat_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyVlanFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_VlanFormat_get(bcm_field_entry_t entry, 
                                  uint8 *data, 
                                  uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_VlanFormat_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_TranslatedVlanFormat_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTranslatedVlanFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TranslatedVlanFormat_get(bcm_field_entry_t entry, 
                                            uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_TranslatedVlanFormat_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_MHOpcode_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyMHOpcode
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_MHOpcode_get(bcm_field_entry_t entry, 
                                uint8 *data, 
                                uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_MHOpcode_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_HiGig_get
 * Purpose:
 *      Qualify on HiGig packets.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_HiGig_get(bcm_field_entry_t entry, 
                             uint8 *data, 
                             uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_HiGig_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstHiGig_get
 * Purpose:
 *      Qualify on packets destined to higig port.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstHiGig_get(bcm_field_entry_t entry, 
                             uint8 *data, 
                             uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstHiGig_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InterfaceClassPort_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInterfaceClassPort
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InterfaceClassPort_get(bcm_field_entry_t entry, 
                                          uint32 *data, 
                                          uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InterfaceClassPort_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InterfaceClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInterfaceClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InterfaceClassL2_get(bcm_field_entry_t entry, 
                                        uint32 *data, 
                                        uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InterfaceClassL2_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InterfaceClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInterfaceClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InterfaceClassL3_get(bcm_field_entry_t entry, 
                                        uint32 *data, 
                                        uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InterfaceClassL3_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcClassL2_get(bcm_field_entry_t entry, 
                                  uint32 *data, 
                                  uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcClassL2_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcClassL3_get(bcm_field_entry_t entry, 
                                  uint32 *data, 
                                  uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcClassL3_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcClassField_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcClassField
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_SrcClassField_get(bcm_field_entry_t entry, 
                                     uint32 *data, 
                                     uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcClassField_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstClassL2_get(bcm_field_entry_t entry, 
                                  uint32 *data, 
                                  uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstClassL2_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstClassL3_get(bcm_field_entry_t entry, 
                                  uint32 *data, 
                                  uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstClassL3_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstClassField_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstClassField
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DstClassField_get(bcm_field_entry_t entry, 
                                     uint32 *data, 
                                     uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstClassField_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IpProtocolCommon_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpProtocolCommon
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      protocol - (OUT) Qualifier protocol encoding.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpProtocolCommon_get(bcm_field_entry_t entry, 
                                        bcm_field_IpProtocolCommon_t *protocol)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(protocol);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpProtocolCommon_get(bcm_unit, entry, protocol);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerIpProtocolCommon_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerIpProtocolCommon
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      protocol - (OUT) Qualifier inner ip protocol encodnig.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerIpProtocolCommon_get(bcm_field_entry_t entry, 
                                             bcm_field_IpProtocolCommon_t *protocol)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(protocol);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerIpProtocolCommon_get(bcm_unit, entry, protocol);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Snap_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySnap
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Snap_get(bcm_field_entry_t entry, 
                            bcm_field_snap_header_t *data, 
                            bcm_field_snap_header_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Snap_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Llc_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyLlc
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Llc_get(bcm_field_entry_t entry, 
                           bcm_field_llc_header_t *data, 
                           bcm_field_llc_header_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Llc_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerTpid_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerTpid
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      tpid - (OUT) Qualifier tpid.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerTpid_get(bcm_field_entry_t entry, 
                                 uint16 *tpid)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tpid);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerTpid_get(bcm_unit, entry, tpid);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_OuterTpid_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyOuterTpid
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      tpid - (OUT) Qualifier tpid.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_OuterTpid_get(bcm_field_entry_t entry, 
                                 uint16 *tpid)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tpid);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_OuterTpid_get(bcm_unit, entry, tpid);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L3Routable_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL3Routable
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L3Routable_get(bcm_field_entry_t entry, 
                                  uint8 *data, 
                                  uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L3Routable_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IpFrag_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpFrag
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      frag_info - (OUT) Qualifier ip framentation encoding.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpFrag_get(bcm_field_entry_t entry, 
                              bcm_field_IpFrag_t *frag_info)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(frag_info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpFrag_get(bcm_unit, entry, frag_info);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_Vrf_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyVrf
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Vrf_get(bcm_field_entry_t entry, 
                           uint32 *data, 
                           uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Vrf_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_field_qualify_L3Ingress_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL3Ingress
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L3Ingress_get(bcm_field_entry_t entry, 
                           uint32 *data, 
                           uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L3Ingress_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_field_qualify_Vpn_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyVpn
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_Vpn_get(bcm_field_entry_t entry, 
                           bcm_vpn_t *data, 
                           bcm_vpn_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_Vpn_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_ExtensionHeaderType_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyExtensionHeaderType
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_ExtensionHeaderType_get(bcm_field_entry_t entry, 
                                           uint8 *data, 
                                           uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_ExtensionHeaderType_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_ExtensionHeaderSubCode_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyExtensionHeaderSubCode
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_ExtensionHeaderSubCode_get(bcm_field_entry_t entry, 
                                              uint8 *data, 
                                              uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_ExtensionHeaderSubCode_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_ExtensionHeader2Type_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyExtensionHeader2Type
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_ExtensionHeader2Type_get(bcm_field_entry_t entry, 
                                            uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_ExtensionHeader2Type_get(bcm_unit, entry,
                                                        data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L4Ports_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL4Ports
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L4Ports_get(bcm_field_entry_t entry, 
                               uint8 *data, 
                               uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L4Ports_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_MirrorCopy_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyMirrorCopy
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_MirrorCopy_get(bcm_field_entry_t entry, 
                                  uint8 *data, 
                                  uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_MirrorCopy_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_TunnelTerminated_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyTunnelTerminated
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TunnelTerminated_get(bcm_field_entry_t entry, 
                                        uint8 *data, 
                                        uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_TunnelTerminated_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_MplsTerminated_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyMplsTerminated
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_MplsTerminated_get(bcm_field_entry_t entry, 
                                      uint8 *data, 
                                      uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_MplsTerminated_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerSrcIp_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerSrcIp
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerSrcIp_get(bcm_field_entry_t entry, 
                                  bcm_ip_t *data, 
                                  bcm_ip_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerSrcIp_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerDstIp_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerDstIp_get(bcm_field_entry_t entry, 
                                  bcm_ip_t *data, 
                                  bcm_ip_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerDstIp_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerSrcIp6_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerSrcIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerSrcIp6_get(bcm_field_entry_t entry, 
                                   bcm_ip6_t *data, 
                                   bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerSrcIp6_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerDstIp6_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerDstIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerDstIp6_get(bcm_field_entry_t entry, 
                                   bcm_ip6_t *data, 
                                   bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerDstIp6_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerSrcIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerSrcIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerSrcIp6High_get(bcm_field_entry_t entry, 
                                       bcm_ip6_t *data, 
                                       bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerSrcIp6High_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerDstIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerDstIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerDstIp6High_get(bcm_field_entry_t entry, 
                                       bcm_ip6_t *data, 
                                       bcm_ip6_t *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerDstIp6High_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerTtl_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerTtl
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerTtl_get(bcm_field_entry_t entry, 
                                uint8 *data, 
                                uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerTtl_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerTos_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerTos_get(bcm_field_entry_t entry, 
                                 uint8 *data, 
                                 uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerTos_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerDSCP_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerDSCP_get(bcm_field_entry_t entry, 
                                 uint8 *data, 
                                 uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerDSCP_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerIpProtocol_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerIpProtocol
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerIpProtocol_get(bcm_field_entry_t entry, 
                                       uint8 *data, 
                                       uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerIpProtocol_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_InnerIpFrag_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyInnerIpFrag
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      frag_info - (OUT) Inner ip header fragmentation info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_InnerIpFrag_get(bcm_field_entry_t entry, 
                                   bcm_field_IpFrag_t *frag_info)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(frag_info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_InnerIpFrag_get(bcm_unit, entry, frag_info);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DosAttack_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDosAttack
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_DosAttack_get(bcm_field_entry_t entry, 
                                 uint8 *data, 
                                 uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DosAttack_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IpmcStarGroupHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIpmcStarGroupHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IpmcStarGroupHit_get(bcm_field_entry_t entry, 
                                        uint8 *data, 
                                        uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IpmcStarGroupHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_MyStationHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyMyStationHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_MyStationHit_get(bcm_field_entry_t entry, 
                                    uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_MyStationHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2PayloadFirstEightBytes_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2PayloadFirstEightBytes
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data1 - (OUT) Qualifier first four bytes of match data.
 *      data2 - (OUT) Qualifier last four bytes of match data.
 *      mask1 - (OUT) Qualifier first four bytes of match mask.
 *      mask2 - (OUT) Qualifier last four bytes of match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_L2PayloadFirstEightBytes_get(bcm_field_entry_t entry, 
                                                uint32 *data1, uint32 *data2, 
                                                uint32 *mask1, uint32 *mask2)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data1);
    BCMX_PARAM_NULL_CHECK(data2);
    BCMX_PARAM_NULL_CHECK(mask1);
    BCMX_PARAM_NULL_CHECK(mask2);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2PayloadFirstEightBytes_get(bcm_unit, entry,
                                                            data1, data2,
                                                            mask1, mask2);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L3DestRouteHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL3DestRouteHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L3DestRouteHit_get(bcm_field_entry_t entry, 
                                      uint8 *data, 
                                      uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L3DestRouteHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L3DestHostHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL3DestHostHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L3DestHostHit_get(bcm_field_entry_t entry, 
                                     uint8 *data, 
                                     uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L3DestHostHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L3SrcHostHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL3SrcHostHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L3SrcHostHit_get(bcm_field_entry_t entry, 
                                    uint8 *data, 
                                    uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L3SrcHostHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2CacheHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2CacheHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L2CacheHit_get(bcm_field_entry_t entry, 
                                  uint8 *data, 
                                  uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2CacheHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2StationMove_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2StationMove
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L2StationMove_get(bcm_field_entry_t entry, 
                                     uint8 *data, 
                                     uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2StationMove_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2DestHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2DestHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L2DestHit_get(bcm_field_entry_t entry, 
                                 uint8 *data, 
                                 uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2DestHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2SrcStatic_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2SrcStatic
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L2SrcStatic_get(bcm_field_entry_t entry, 
                                   uint8 *data, 
                                   uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2SrcStatic_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_L2SrcHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyL2SrcHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_L2SrcHit_get(bcm_field_entry_t entry, 
                                uint8 *data, 
                                uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_L2SrcHit_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IngressStpState_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIngressStpState
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IngressStpState_get(bcm_field_entry_t entry, 
                                       uint8 *data, 
                                       uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IngressStpState_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_ForwardingVlanValid_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyForwardingVlanValid
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_ForwardingVlanValid_get(bcm_field_entry_t entry, 
                                           uint8 *data, 
                                           uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_ForwardingVlanValid_get(bcm_unit, entry,
                                                       data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_SrcVirtualPortValid_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifySrcVirtualPortValid
 *                qualifier from the field entry.
 * Parameters:
 *      entry - (IN)  BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_SrcVirtualPortValid_get(bcm_field_entry_t entry, 
                                           uint8 *data, uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_SrcVirtualPortValid_get(bcm_unit, entry,
                                                       data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_VlanTranslationHit_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyVlanTranslationHit
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_VlanTranslationHit_get(bcm_field_entry_t entry, 
                                          uint8 *data, 
                                          uint8 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_VlanTranslationHit_get(bcm_unit, entry,
                                                      data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_IcmpTypeCode_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyIcmpTypeCode
 *                     qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_IcmpTypeCode_get(bcm_field_entry_t entry, 
                                    uint16 *data, 
                                    uint16 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_IcmpTypeCode_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_FibreChanOuter_get
 * Purpose:
 *      Get Fibre Channel outer header type field qualification from
 *      field entry.
 * Parameters:
 *      entry           - (IN) BCM field entry id.
 *      fibre_chan_type - (OUT) Fibre Channel header type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_FibreChanOuter_get(bcm_field_entry_t entry,
                                      bcm_field_FibreChan_t *fibre_chan_type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(fibre_chan_type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_FibreChanOuter_get(bcm_unit, entry,
                                                  fibre_chan_type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_FibreChanInner_get
 * Purpose:
 *      Get Fibre Channel inner header type field qualification from
 *      field entry.
 * Parameters:
 *      entry           - (IN) BCM field entry id.
 *      fibre_chan_type - (OUT) Fibre Channel header type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_FibreChanInner_get(bcm_field_entry_t entry, 
                                      bcm_field_FibreChan_t *fibre_chan_type)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(fibre_chan_type);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_FibreChanInner_get(bcm_unit, entry,
                                                  fibre_chan_type);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_VnTag_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyVnTag
 *      qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_VnTag_get(bcm_field_entry_t entry,
                             uint32 *data, uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_VnTag_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_CnTag_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyCnTag
 *      qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_CnTag_get(bcm_field_entry_t entry, 
                             uint32 *data, uint32 *mask)
{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_CnTag_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_FabricQueueTag_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyFabricQueueTag
 *      qualifier from the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_FabricQueueTag_get(bcm_field_entry_t entry, 
                                      uint32 *data, uint32 *mask)

{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(data);
    BCMX_PARAM_NULL_CHECK(mask);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_FabricQueueTag_get(bcm_unit, entry, data, mask);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_field_qualify_DstMultipath_get
 * Purpose:
 *      Get match criteria for bcmFieldQualifyDstMultipath
 *      qualifier from the field entry.
 * Parameters:
 *      entry  - (IN) BCM field entry id.
 *      mpintf - (OUT) Multipath egress object id. 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_field_qualify_DstMultipath_get(bcm_field_entry_t entry, bcm_if_t *mpintf)

{
    int rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mpintf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_field_qualify_DstMultipath_get(bcm_unit, entry, mpintf);
        if (BCMX_FIELD_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_field_qualifier_delete
 * Purpose:
 *      Remove match criteria from a field processor entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      qual_id - (IN) BCM field qualifier id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualifier_delete(bcm_field_entry_t entry, 
                            bcm_field_qualify_t qual_id)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualifier_delete(bcm_unit, entry, qual_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FIELD_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_TcpSequenceZero
 * Purpose:
 *      Set match criteria for bcmFieldQualifyTcpSequenceZero
 *                qualifier to the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      flag  - (IN) Flag.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TcpSequenceZero(bcm_field_entry_t entry, 
                                   uint32 flag)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_TcpSequenceZero(bcm_unit, entry, flag);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}

/*
 * Function:
 *      bcmx_field_qualify_TcpHeaderSize
 * Purpose:
 *      Set match criteria for bcmFieldQualifyTcpHeaderSize
 *                qualifier to the field entry.
 * Parameters:
 *      entry - (IN) BCM field entry id.
 *      data  - (IN) Qualifier match data.
 *      mask  - (IN) Qualifier match data mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcmx_field_qualify_TcpHeaderSize(bcm_field_entry_t entry, 
                                 uint8 data, uint8 mask)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_FIELD_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_field_qualify_TcpHeaderSize(bcm_unit, entry, data, mask);
        BCM_IF_ERROR_RETURN(BCMX_FIELD_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    return rv;
}
