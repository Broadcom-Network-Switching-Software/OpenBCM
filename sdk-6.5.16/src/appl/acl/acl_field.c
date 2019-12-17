/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        acl_field.c
 *
 * Purpose:     
 *     Field Processor specific functionality for ACL
 */


#if defined(INCLUDE_ACL)

#include <shared/bsl.h>

#include <assert.h>

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/appl/io.h>

#include <bcm/error.h>
#include <bcmx/field.h>
#include <bcmx/port.h>

#include <appl/acl/acl.h>

#include "acl_util.h"
#include "acl_field.h"

/* Macros */

#define ACL_PRIO_ENTRY_DEFAULT   1024
#define ACL_PRIO_GROUP_DEFAULT   16

/*
 * Macro:
 *     ACL_FIELD_IS_INIT (internal)
 * Purpose:
 *     Confirm that the ACL field functions are initialized.
 *
 * Parameters:
 *
 * Notes:
 *     Results in return(BCM_E_UNAVAIL), or return(BCM_E_INIT) if fails.
 */
#define ACL_FIELD_IS_INIT() do {                                        \
        if (acl_field_control == NULL) {                                \
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,                       \
                      (BSL_META("ACL Error: ACL Field  not initialized\n"))); \
            return BCM_E_INIT;                                          \
        }                                                               \
    } while (0)

/* Local typedefs */

/*
 * Typedef: _acl_field_range_t
 *
 * Purpose:
 */
typedef struct _acl_field_range_check_s _acl_field_range_check_t;

struct _acl_field_range_check_s {
    bcm_field_range_t         range_id;   /* Range Check ID                */
    int                       references; /* Number of entries using range */
    _acl_field_range_check_t  *next;      /* for linked-lists              */
};

/*
 * Typedef: _acl_field_entry_t
 *
 * Purpose:
 */
typedef struct _acl_field_entry_s {
    bcm_field_entry_t          eid;          /* Entry ID                 */
    bcma_acl_rule_t            *rule;        /* Corresponding rule       */
    int                        prio;         /* entry priority           */
    _acl_field_range_check_t   *range;       /* Range checker, if needed */
    struct _acl_field_entry_s  *next;        /* for linked-lists         */
} _acl_field_entry_t;

/*
 * Typedef: _acl_field_group_t
 *
 * Purpose:
 *     Field specific device-wide parameters for ACL.
 */
typedef struct _acl_field_group_s {
    bcm_field_group_t          gid;          /* Group ID         */
    bcm_field_qset_t           qset;         /* Qualifier Set    */
    int                        prio_least;   /* Lowest priority used. */
    struct _acl_field_entry_s  *entry_list;  /* List of group's entries */
    struct _acl_field_group_s  *next;        /* for linked-lists */
} _acl_field_group_t;

/*
 * Typedef: _acl_field_control_t
 *
 * Purpose:
 *     Field specific device-wide parameters for ACL.
 */
typedef struct _acl_field_control_s {
    int                       prio_prev;/* priority used for most recent group*/
    _acl_field_group_t        *group_list;/* groups                           */
    _acl_field_range_check_t  *ranges; /* Range checkers defined on device    */
} _acl_field_control_t;


/* Local Prototypes */
STATIC int _acl_field_group_create(_acl_field_control_t *control,
                                   bcm_field_qset_t qset,
                                   _acl_field_group_t **ret_group);
STATIC int _acl_field_group_remove_all(void);
STATIC int _acl_field_group_destroy_all(void);
STATIC int _acl_field_group_destroy(_acl_field_control_t *control,
                                    bcm_field_group_t gid);


STATIC int _acl_field_entry_create(_acl_field_group_t *group,
                                   bcma_acl_t *list,
                                   bcma_acl_rule_t *rule);
STATIC int _acl_field_entry_qualify_vlan(_acl_field_group_t *group,
                                         bcma_acl_rule_t *rule,
                                         bcm_field_entry_t eid,
                                         _acl_field_entry_t *entry);
#define _ACL_FIELD_L4_SRC  1
#define _ACL_FIELD_L4_DST  2

STATIC int _acl_field_range_check_create(uint32 flags, bcm_l4_port_t port_min,
                                         bcm_l4_port_t port_max,
                                         _acl_field_range_check_t **range_p);
STATIC int _acl_field_range_check_find(uint32 flags, bcm_l4_port_t port_min,
                                       bcm_l4_port_t port_max,
                                       _acl_field_range_check_t **range_p) ;
STATIC int _acl_field_range_check_destroy(_acl_field_entry_t *entry);

STATIC int _acl_field_entry_qualify_l4srcport(_acl_field_group_t *group,
                                              bcma_acl_rule_t *rule,
                                              bcm_field_entry_t eid,
                                              _acl_field_entry_t *entry);
STATIC int _acl_field_entry_qualify_l4dstport(_acl_field_group_t *group,
                                              bcma_acl_rule_t *rule,
                                              bcm_field_entry_t eid,
                                              _acl_field_entry_t *entry);

STATIC _acl_field_entry_t *_acl_field_entry_alloc(bcma_acl_rule_t *rule,
                                                  bcm_field_entry_t eid,
                                                  _acl_field_group_t *group);

STATIC int _acl_field_entry_remove_all( _acl_field_group_t *group);
STATIC int _acl_field_entry_remove(_acl_field_group_t *group,
                                    bcm_field_entry_t eid);
STATIC int _acl_field_entry_destroy_all(_acl_field_group_t *group);
STATIC int _acl_field_entry_destroy(_acl_field_group_t *group,
                                    bcm_field_entry_t eid);

STATIC int _acl_field_entry_action_add(bcma_acl_rule_t *rule,
                                       bcm_field_entry_t eid);


STATIC int _acl_field_rule_qset_get(bcma_acl_rule_t *rule,
                                    bcm_field_qset_t *qset);
STATIC int _acl_field_rule_entry_find(bcma_acl_rule_id_t rule_id,
                                      _acl_field_group_t **group_p,
                                      bcm_field_entry_t *eid);

#ifdef BROADCOM_DEBUG 
STATIC int _acl_field_group_show(_acl_field_group_t *group);
STATIC int _acl_field_entry_show(_acl_field_entry_t *entry);
#endif /* BROADCOM_DEBUG */
 

/* ACL Field Control Globals */

static _acl_field_control_t *acl_field_control;


/* ACL Field Function Definitions */


/* Section: Constructor and destructor for ACL module */

/*
 * Function: _acl_field_init
 *
 * Purpose:
 *     Initialize the ACL Field sub-module.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE    - Success
 *     BCM_E_MEMORY  - ACL field control allocation failure
 */
int
_acl_field_init(void)
{
    int                   retval = BCM_E_NONE;
    _acl_field_control_t  *field_control;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_init()\n")));

    /* Detach first if ACL has been previously initialized. */
    if (acl_field_control != NULL) {
        BCM_IF_ERROR_RETURN(_acl_field_detach());
    }

    /* Allocate and initalize Field Control struct. */
    field_control = sal_alloc(sizeof(_acl_field_control_t),
                              "ACL Field Control");
    if (field_control == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: allocation failure for ACL Field control.\n")));
        return BCM_E_MEMORY;
    }
    sal_memset(field_control, 0, sizeof(_acl_field_control_t));

    /* Initialize the local group info. */
    field_control->prio_prev   = ACL_PRIO_GROUP_DEFAULT;
    field_control->group_list  = NULL;

    acl_field_control = field_control;

    return retval;
}

/*
 * Function: _acl_field_detach
 *
 * Purpose:
 *     Remove any lists and free lock and control.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - Device not initialized
 */
int
_acl_field_detach(void)
{
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_detach()\n")));

    if (acl_field_control == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: detaching that is not initialized.\n")));
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(_acl_field_group_destroy_all())) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Failure in _acl_field_group_destroy_all()\n")));
    }

    sal_free(acl_field_control);
    acl_field_control = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _acl_field_group_create
 *
 * Purpose:
 *     Create a new Field group with priority (slice) lower than the last one.
 *
 * Parameters:
 *     control   - field control struct
 *     qset      - Qualifier set
 *     ret_group - (OUT) pointer to newly created group
 *
 * Returns:
 *     BCM_E_NONE    - Success
 *     BCM_E_MEMORY  - _acl_field_group_t allocation failure
 *     BCM_E_XXX     - from bcmx_field_group_create()
 */
STATIC int
_acl_field_group_create(_acl_field_control_t *field_control,
                        bcm_field_qset_t qset,
                        _acl_field_group_t **ret_group)
{
    bcm_field_group_t     gid;
    int                   retval;
    _acl_field_group_t    *group;
    _acl_field_group_t    *iter;

    assert(field_control != NULL);
    assert(ret_group     != NULL);

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_group_create()\n")));

    /* Allocate a ACL field group. */
    group = sal_alloc(sizeof(_acl_field_group_t), "ACL field group");
    if (group == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Allocation error for ACL field group\n")));
        return BCM_E_MEMORY;
    }
    sal_memset(group, 0, sizeof(_acl_field_group_t));
    group->prio_least = ACL_PRIO_ENTRY_DEFAULT;

    /* Create the actual Field group. */
    retval = BCM_E_RESOURCE;

    assert(field_control->prio_prev <= ACL_PRIO_GROUP_DEFAULT);
    while (field_control->prio_prev > 0 && BCM_FAILURE(retval)) {
        LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                    (BSL_META("ACL Calling FP group create pri=%d\n"),
                     field_control->prio_prev));
        retval = bcmx_field_group_create_mode(qset, --field_control->prio_prev,
                                         bcmFieldGroupModeAuto, &gid);
    }
    if (BCM_FAILURE(retval)){
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Group creation error (prio=%d, gid=%d)\n"),
                   field_control->prio_prev, gid));
        return retval;
    }

    group->gid = gid;
    iter = field_control->group_list;

    /* Put it at head if there are no groups. */
    if (iter == NULL) {
        field_control->group_list = group;
        *ret_group          = group;
        return BCM_E_NONE;
    }

    /* Put the group at the tail of the control group list.*/
    while (iter->next != NULL) {
        iter = iter->next;
    }

    iter->next = group;
    *ret_group = group;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _acl_field_group_remove_all
 *
 * Purpose:
 *     Destroy all Field group used by ACL.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Group ID not found 
 *     BCM_E_XXX       - From bcmx_field_entry_remove()
 */
STATIC int
_acl_field_group_remove_all(void) {
    int                   retval;
    _acl_field_group_t    *group;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_group_remove_all()\n")));
    assert(acl_field_control != NULL);

    /* Traverse the group list, removing all entries in each group. */
    group = acl_field_control->group_list;
    while (group != NULL) {
        /* Remove any entries in group.*/
        retval = _acl_field_entry_remove_all(group);
        if (BCM_FAILURE(retval)) {
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                      (BSL_META("ACL Error: Failed to remove entries in group ID=%d\n"),
                       group->gid));
            return retval;
        }
        group = group->next;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _acl_field_group_destroy_all
 *
 * Purpose:
 *     Destroy all Field groups used by ACL. This deallocates all software
 *     strutures for field groups and their entries.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Group ID not found 
 *     BCM_E_XXX       - from _acl_field_group_destroy()
 */
STATIC int
_acl_field_group_destroy_all(void) {
    int                   retval;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_group_destroy_all()\n")));
    assert(acl_field_control != NULL);

    while (acl_field_control->group_list != NULL) {
        retval = _acl_field_group_destroy(acl_field_control,
                                          acl_field_control->group_list->gid);
        if (BCM_FAILURE(retval)) {
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                      (BSL_META("ACL Error: _acl_field_group_destroy_all() failure\n")));
            return retval;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _acl_field_group_destroy
 *
 * Purpose:
 *     Destroy a Field group used by ACL. This deallocates the field group and
 *     entry structures while leaving the hardware programming alone.
 *
 * Parameters:
 *     control - ACL field control info
 *     gid     - group to be destroyed
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Group ID not found 
 *     BCM_E_XXX       - from bcmx_field_group_destroy()
 */
STATIC int
_acl_field_group_destroy(_acl_field_control_t *control,
                         bcm_field_group_t gid) {
    int                 retval;
    _acl_field_group_t  *group;
    _acl_field_group_t  *prev = NULL;


    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_group_destroy(gid=%d)\n"),
               gid));
    assert(control != NULL);

    /* If the group list is empty, group ID can't be found. */
    if (control->group_list == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: No groups available to delete?\n")));
        return BCM_E_NOT_FOUND;
    }
    /* Scan group list, looking for matching Group ID. */
    group = control->group_list;

    while (group != NULL) {
        if (group->gid == gid) {
            /* Destroy any entries in group.*/
            retval = _acl_field_entry_destroy_all(group);
            if (BCM_FAILURE(retval)) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: Failed to destroy entries in group ID=%d\n"),
                           group->gid));
                return retval;
            }

            /* Destroy the group itself.*/
            if (prev == NULL) {
                control->group_list = group->next;
            } else {
                prev->next = group->next;
            }
            sal_free(group);

            retval = bcmx_field_group_destroy(gid);
            if (BCM_FAILURE(retval)) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: group ID=%d not destroyed\n"),
                           gid));
                return retval;
            }
            return BCM_E_NONE;
        }

        prev  = group;
        group = group->next;
    }

    LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL Error: group ID=%d not found to be destroyed\n"),
               gid));
    return BCM_E_NOT_FOUND;
}

#define _ACL_FIELD_ENTRY_CREATE_RECOVER(_rv_,_grp_)    \
    do {                                               \
        if ((_rv_) < 0) {                              \
            _acl_field_entry_destroy_all(_grp_);       \
            return (_rv_);                             \
        }                                              \
    } while (0);

/*
 * Function: _acl_field_entry_create
 *
 * Purpose:
 *
 * Parameters:
 *     group - ACL field group
 *     list  - ACL list that contains rule
 *     rule  - ACL rule
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_MEMORY - Field entry allocation failure
 */
STATIC int
_acl_field_entry_create(_acl_field_group_t *group,
                        bcma_acl_t *list,
                        bcma_acl_rule_t *rule)
{
    int                 retval;
    bcm_field_entry_t   eid;
    _acl_field_entry_t  *entry;
    bcm_mac_t           mac_mask = _BCMA_ACL_MAC_ALL_ONES;
    bcmx_lplist_t       lplist_full;

    assert(group != NULL);
    assert(list  != NULL);
    assert(rule  != NULL);

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_create(gid=%d, list_id=%d, rule_id=%d)\n"),
               group->gid, list->list_id, rule->rule_id));
    ACL_FIELD_IS_INIT();
    BCM_IF_ERROR_RETURN(bcmx_field_entry_create(group->gid, &eid));

    /* Set the priority of the entry. */
    retval = bcmx_field_entry_prio_set(eid, --group->prio_least);
    if (BCM_FAILURE(retval)) {
        bcmx_field_entry_destroy(eid);
        return retval;
    }

    bcmx_lplist_init(&lplist_full, 0, 0);
    bcmx_port_lplist_populate(&lplist_full, BCMX_PORT_LP_ALL);

    entry = _acl_field_entry_alloc(rule, eid, group);
    if (entry == NULL) {
        bcmx_lplist_free(&lplist_full);
        bcmx_field_entry_destroy(eid);
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: _acl_field_entry_t allocation failure\n")));
        return BCM_E_MEMORY;
    }

    /* Add actions to entry. */
    retval = _acl_field_entry_action_add(rule, eid);
    _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);

    /* Add logical port qualification, if needed. */
    if (!bcmx_lplist_eq(&list->lplist, &lplist_full)) {
        retval = bcmx_field_qualify_InPorts(eid, list->lplist);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on Source MAC. */
    if (rule->flags & BCMA_ACL_RULE_SRC_MAC) {
        retval = bcmx_field_qualify_SrcMac(eid, rule->src_mac, mac_mask);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on Destination MAC */
    if (rule->flags & BCMA_ACL_RULE_DST_MAC) {
        retval = bcmx_field_qualify_DstMac(eid, rule->dst_mac, mac_mask);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on Source IPv4 */
    if (rule->flags & BCMA_ACL_RULE_SRC_IP4) {
        retval = bcmx_field_qualify_SrcIp(eid, rule->src_ip,
                                          rule->src_ip_mask);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on Destination IPv4 */
    if (rule->flags & BCMA_ACL_RULE_DST_IP4) {
        retval = bcmx_field_qualify_DstIp(eid, rule->dst_ip, rule->dst_ip_mask);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on SrcIp6 */
    if (rule->flags & BCMA_ACL_RULE_SRC_IP6) {
        retval = bcmx_field_qualify_SrcIp6(eid, rule->src_ip6,
                                           rule->src_ip6_mask);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on DstIp6 */
    if (rule->flags & BCMA_ACL_RULE_DST_IP6) {
        retval = bcmx_field_qualify_DstIp6(eid, rule->dst_ip6,
                                           rule->dst_ip6_mask);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on Ethertype. */
    if (rule->flags & BCMA_ACL_RULE_ETHERTYPE) {
        retval = bcmx_field_qualify_EtherType(eid, rule->ether_type,
                                              ACL_MASK_ETHERTYPE);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on IP Protocol. */
    if (rule->flags & BCMA_ACL_RULE_IPPROTOCOL) {
        retval = bcmx_field_qualify_IpProtocol(eid, rule->ip_protocol,
                                               ACL_MASK_IPPROTOCOL);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* For qualifiers that have ranges, */

    /* Qualify on VLAN range. */
    if (rule->flags & BCMA_ACL_RULE_VLAN) {
        retval = _acl_field_entry_qualify_vlan(group, rule, eid, entry);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on L4 Source Port range. */
    if (rule->flags & BCMA_ACL_RULE_L4_SRC_PORT) {
        retval = _acl_field_entry_qualify_l4srcport(group, rule, eid, entry);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }

    /* Qualify on L4 Destination Port range. */
    if (rule->flags & BCMA_ACL_RULE_L4_DST_PORT) {
        retval =_acl_field_entry_qualify_l4dstport(group, rule, eid, entry);
        _ACL_FIELD_ENTRY_CREATE_RECOVER(retval, group);
    }
 
    bcmx_lplist_free(&lplist_full);
    return BCM_E_NONE;
}
#undef _ACL_FIELD_ENTRY_CREATE_RECOVER

/*
 * Function: _acl_field_entry_qualify_vlan
 *
 * Purpose:
 *     Qualify on VLAN range.
 *
 * Parameters:
 *     group - ACL field group
 *     rule  - ACL rule
 *     eid   - Entry ID
 *     entry - 
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Field entry allocation failure
 */
STATIC int
_acl_field_entry_qualify_vlan(_acl_field_group_t *group, bcma_acl_rule_t *rule,
                              bcm_field_entry_t eid, _acl_field_entry_t *entry)
{
    acl_node_t          *list;
    acl_node_t          *node_cur;
    int                 count;
    int                 idx;
    bcm_field_entry_t   eid_copy;
    int                 retval;
    _acl_rule_link_t    *rule_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_qualify_vlan(gid=%d, rule_id=%d)\n"),
               group->gid, rule->rule_id));
    /* Get list of range data/mask pairs. */
    acl_range_to_list(rule->vlan_min, rule->vlan_max, &list, &count);
    node_cur = list; 

    /* Qualify on first element with entry that already exists. */
    BCM_IF_ERROR_RETURN(bcmx_field_qualify_OuterVlan(eid, list->data,
                                                     list->mask));
    node_cur = node_cur->next;

    for (idx = 1; idx < count; idx++) {
        /* Create enough entries to cover range. */
        BCM_IF_ERROR_RETURN(bcmx_field_entry_copy(eid, &eid_copy));
        entry = _acl_field_entry_alloc(rule, eid_copy, group);
        if (entry == NULL) {
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                      (BSL_META("ACL Error: _acl_field_entry_t allocation failure\n")));
            return BCM_E_MEMORY;
        }

        /* Qualify on entries with each data/mask pair from list. */
        retval = bcmx_field_qualify_OuterVlan(eid_copy, node_cur->data,
                                              node_cur->mask);
        if (BCM_FAILURE(retval)) {
            rule_link = _acl_rule_link_find(rule->rule_id);
            rule_link->entries--;
            sal_free(entry);
            return retval;
        }
        node_cur = node_cur->next;
    }

    /* Destroy list */
    return acl_range_destroy(list, count);
}

/*
 * Function: _acl_field_entry_qualify_l4srcport
 *
 * Purpose:
 *     Qualify on L4 Source Port range.
 *
 * Parameters:
 *     group - ACL field group
 *     rule  - ACL rule
 *     eid   - Entry ID
 *     entry - ACL field entry
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Field entry allocation failure
 */
STATIC int
_acl_field_entry_qualify_l4srcport(_acl_field_group_t *group,
                                   bcma_acl_rule_t *rule,
                                   bcm_field_entry_t eid,
                                   _acl_field_entry_t *entry)
{
    _acl_field_range_check_t   *range;  
    acl_node_t      *list;
    acl_node_t      *node_cur;
    int             count, org_count;
    int             idx, rule_idx;
    bcm_field_entry_t eid_copy, eid_cur=0;
    _acl_rule_link_t      *rule_link;
    _acl_field_entry_t     *entry_start;
    int rv = BCM_E_NONE;

   LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("ACL _acl_field_entry_qualify_l4srcport(gid=%d, rule_id=%d)\n"),
              group->gid, rule->rule_id));

    /* Create the necessary Field range checker. */
    rv = _acl_field_range_check_create(BCM_FIELD_RANGE_SRCPORT,
                                      rule->src_port_min,
                                      rule->src_port_max, &range);
    if (rv == BCM_E_NONE) {
        entry->range = range;

        /* Qualify the entry on that range checker. */
        return bcmx_field_qualify_RangeCheck(eid, range->range_id, 0);
    }

     /* Create ranger failed */
     
    /* Get list of range data/mask pairs. */
    acl_range_to_list(rule->src_port_min, rule->src_port_max, &list,
                          &count);
    /* Get the original total number of entries for this rule */
    rule_link = _acl_rule_link_find(rule->rule_id);
    org_count = rule_link->entries;
    /* Move to the first  entry pointer */
    entry_start = group->entry_list;
    while (entry_start != NULL) {
        if (entry_start->rule->rule_id == rule->rule_id) {
            eid_cur = entry_start->eid;
            break;
        }
        else {
            entry_start = entry_start->next;
        }
    }

    for (rule_idx = 0; rule_idx < org_count; rule_idx++) {
        node_cur = list;

        /* Qualify on first element with entry that already exists. */
       rv = bcmx_field_qualify_L4SrcPort(eid_cur, 
                                 list->data, list->mask);
        if (rv < 0) {
            goto srcport_error;
        }
        node_cur = node_cur->next;
            
        for (idx = 1; idx < count; idx++) {
            /* Create enough entries to cover range. */
            rv = bcmx_field_entry_copy(eid_cur, &eid_copy);
            if (rv < 0) {
                goto srcport_error;
            }
            entry = _acl_field_entry_alloc(rule, eid_copy, group);
            if (entry == NULL) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: _acl_field_entry_t allocation failure\n")));
                rv =  BCM_E_MEMORY;
                goto srcport_error;
            }

            /* Qualify on entries with each data/mask pair from list. */
           rv = bcmx_field_qualify_L4SrcPort(eid_copy, node_cur->data,
                                            node_cur->mask);
            if (rv < 0) {
                goto srcport_error;
            }

            node_cur = node_cur->next;
        }
        if (entry_start->next != NULL) {
            entry_start = entry_start->next;
            eid_cur = entry_start->eid;
        }
    }
    

    /* Destroy list */
    return acl_range_destroy(list, count);

srcport_error :
    acl_range_destroy(list, count);
    return rv;   
}

/*
 * Function: _acl_field_range_check_create
 *
 * Purpose:
 *
 * Parameters:
 *     flags - Direction flags (source/destination)
 *     port_min - minimum L4 port value
 *     port_max - maximum L4 port value
 *     range_p - (OUT) pointer to ACL range created by this function
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Field range allocation failure
 */

STATIC int
_acl_field_range_check_create(uint32 flags, bcm_l4_port_t port_min,
                              bcm_l4_port_t port_max,
                              _acl_field_range_check_t **range_p) 
{
    bcm_field_range_t   range_id;
    int                 retval;

    assert(range_p != NULL);

    /* Check for existing range checker that matches. */
    retval = _acl_field_range_check_find(flags, port_min, port_max, range_p);
    if (BCM_SUCCESS(retval)) {
        (*range_p)->references += 1;
        return BCM_E_NONE;
    }

    /* Create new Field range checker. */
    BCM_IF_ERROR_RETURN(
        bcmx_field_range_create(&range_id, flags, port_min, port_max));

    /* Create the ACL range node */ 
    *range_p = sal_alloc(sizeof(_acl_field_range_check_t),
                         "ACL FIELD range check");
    if (*range_p == NULL) {
        bcmx_field_range_destroy(range_id);
        return BCM_E_MEMORY;
    }

    /* Initialize range check values. */
    (*range_p)->range_id   = range_id;
    (*range_p)->references = 1;
    (*range_p)->next       = acl_field_control->ranges;

    return BCM_E_NONE;
}

STATIC int
_acl_field_range_check_find(uint32 flags, bcm_l4_port_t port_min,
                            bcm_l4_port_t port_max,
                            _acl_field_range_check_t **range_p) 
{
    uint32                    flags_temp;
    bcm_l4_port_t             port_min_temp;
    bcm_l4_port_t             port_max_temp;
    _acl_field_range_check_t  *range;

    /* Traverse list of range checkers while looking for a match. */
    for (range = acl_field_control->ranges; range != NULL; range = range->next)
    {
        /* Get the  min, max & flags from Field. */
        BCM_IF_ERROR_RETURN(
            bcmx_field_range_get(range->range_id, &flags_temp, &port_min_temp,
                                 &port_max_temp));
        /* If flags, min, and max match, return this range. */
        if (flags_temp == flags && port_max_temp == port_max &&
            port_min_temp == port_min) {
            *range_p = range;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function: _acl_field_range_check_destroy
 *
 * Purpose:
 *     
 *
 * Parameters:
 *     group - ACL field group
 *     rule  - ACL rule
 *     eid   - Entry ID
 *     entry - ACL field entry
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Field entry allocation failure
 */
STATIC int
_acl_field_range_check_destroy(_acl_field_entry_t *entry) {
    _acl_field_range_check_t   *range_cur, *range_prev = NULL;

    if (entry->range == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* Decrement reference count. If non-zero, just return. */
    entry->range->references--;
    if (entry->range->references > 0) {
        entry->range = NULL;
        return BCM_E_NONE;
    }

    /* Find the range in the control's list. */
    for (range_cur = acl_field_control->ranges;
         range_cur != NULL;
	 range_cur = range_cur->next) {
        if (range_cur->range_id == entry->range->range_id) {
            break;
        } 
        range_prev = range_cur;
    }

    if (range_cur == NULL || range_cur->references != 0) {
        return BCM_E_INTERNAL;
    }

    /* Point around range node. */
    if (range_prev == NULL) {
        acl_field_control->ranges = range_cur->next;
    } else {
        range_prev->next = range_cur->next;
    }

    entry->range = NULL;
    bcmx_field_range_destroy(range_cur->range_id);
    sal_free(range_cur);

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_entry_qualify_l4dstport
 *
 * Purpose:
 *     Qualify on L4 Source Port range.
 *
 * Parameters:
 *     group - ACL field group
 *     rule  - ACL rule
 *     eid   - Entry ID
 *     entry - ACL field entry
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Field entry allocation failure
 */
STATIC int
_acl_field_entry_qualify_l4dstport(_acl_field_group_t *group,
                                   bcma_acl_rule_t *rule,
                                   bcm_field_entry_t eid,
                                   _acl_field_entry_t *entry)
{
    _acl_field_range_check_t   *range;
    acl_node_t      *list;
    acl_node_t      *node_cur;
    int             count, org_count;
    int             idx, rule_idx;
    bcm_field_entry_t eid_copy, eid_cur=0;
    _acl_rule_link_t      *rule_link;
    _acl_field_entry_t     *entry_start;
    int rv = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_qualify_l4dstport(gid=%d, rule_id=%d)\n"),
               group->gid, rule->rule_id));     
    /* Create the necessary Field range checker. */
    rv = _acl_field_range_check_create(BCM_FIELD_RANGE_DSTPORT,
                                      rule->dst_port_min,
                                      rule->dst_port_max, &range);
    if (rv == BCM_E_NONE) {
        entry->range = range;

        /* Qualify the entry on that range checker. */
        return bcmx_field_qualify_RangeCheck(eid, range->range_id, 0);
    }

    /* Create ranger failed */

    /* Get list of range data/mask pairs. */
    acl_range_to_list(rule->dst_port_min, rule->dst_port_max, &list,
                          &count);
    /* Get the original total number of entries for this rule */
    rule_link = _acl_rule_link_find(rule->rule_id);
    org_count = rule_link->entries;
    /* Move to the first  entry pointer */
    entry_start = group->entry_list;
    while (entry_start != NULL) {
        if (entry_start->rule->rule_id == rule->rule_id) {
            eid_cur = entry_start->eid;
            break;
        }
        else {
            entry_start = entry_start->next;
        }
    }
    for (rule_idx = 0; rule_idx < org_count; rule_idx++) {
        node_cur = list;

        /* Qualify on first element with entry that already exists. */
       rv = bcmx_field_qualify_L4DstPort(eid_cur, 
                                 list->data, list->mask);
        if (rv < 0) {
            goto dstport_error;
        }
        node_cur = node_cur->next;
            
        for (idx = 1; idx < count; idx++) {
            /* Create enough entries to cover range. */
            rv = bcmx_field_entry_copy(eid_cur, &eid_copy);
            if (rv < 0) {
                goto dstport_error;
            }
            entry = _acl_field_entry_alloc(rule, eid_copy, group);
            if (entry == NULL) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: _acl_field_entry_t allocation failure\n")));
                rv =  BCM_E_MEMORY;
                goto dstport_error;
            }

            /* Qualify on entries with each data/mask pair from list. */
           rv = bcmx_field_qualify_L4DstPort(eid_copy, node_cur->data,
                                            node_cur->mask);
            if (rv < 0) {
                goto dstport_error;
            }

            node_cur = node_cur->next;
        }
        if (entry_start->next != NULL) {
            entry_start = entry_start->next;
            eid_cur = entry_start->eid;
        }
    }

    /* Destroy list */
    return acl_range_destroy(list, count);

dstport_error :
    acl_range_destroy(list, count);
    return rv;
}

/*
 * Function: _acl_field_entry_alloc
 *
 * Purpose:
 *
 * Parameters:
 *     rule  - ACL rule
 *     eid   - Entry ID
 *     group - ACL field group
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Field entry allocation failure
 */
STATIC _acl_field_entry_t*
_acl_field_entry_alloc(bcma_acl_rule_t *rule, bcm_field_entry_t eid,
                       _acl_field_group_t *group) 
{
    _acl_field_entry_t  *entry;
    _acl_rule_link_t    *rule_link;

    assert(rule  != NULL);
    assert(group != NULL);
 
    rule_link = _acl_rule_link_find(rule->rule_id);
    if (rule_link == NULL) {
        return NULL;
    }

    entry = sal_alloc(sizeof(_acl_field_entry_t), "ACL Field Entry");
    if (entry == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: _acl_field_entry_t allocation failure\n")));
        return entry;
    }
    sal_memset(entry, 0, sizeof(_acl_field_entry_t));

    entry->eid        = eid;
    entry->rule       = rule;
    entry->prio       = group->prio_least;
    entry->next       = group->entry_list;
    group->entry_list = entry;

    rule_link->entries++;

    return entry;
}

/*
 * Function:
 *     _acl_field_entry_remove_all
 *
 * Purpose:
 *     Remove all entries in a group from hardware.
 *
 * Parameters:
 *     group - group to have entries removed
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_XXX  - From bcmx_field_entry_remove()
 */
STATIC int
_acl_field_entry_remove_all(_acl_field_group_t *group) 
{
    int                 retval;
    _acl_field_entry_t  *entry;

    assert(group   != NULL);

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_remove_all(gid=%d)\n"),
               group->gid));
    entry = group->entry_list;
    while (entry != NULL) {
        retval = bcmx_field_entry_remove(entry->eid);
        if (BCM_FAILURE(retval)) {
           LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                     (BSL_META("ACL Error: Entry ID=%d not removed\n"),
                      entry->eid));
           return retval;
        }
        entry = entry->next;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _acl_field_entry_remove
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
STATIC int
_acl_field_entry_remove(_acl_field_group_t *group,
                         bcm_field_entry_t eid) 
{
    int                 retval;
    _acl_field_entry_t  *entry;

    assert(group != NULL);
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_remove(gid=%d, eid=%d)\n"),
               group->gid, eid));

    /* Scan entry list, looking for matching Entry ID. */
    entry = group->entry_list;
    while (entry != NULL) {
        if (entry->eid == eid) {
            /* Found it so remove it.*/
            retval = bcmx_field_entry_remove(eid);
            if (BCM_FAILURE(retval)) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: Entry ID=%d not removed\n"),
                           eid));
                return retval;
            }
            return BCM_E_NONE;
        }
        entry = entry->next;
    }

    LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL Error: Entry ID=%d not found to be removed\n"),
               eid));
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     _acl_field_entry_destroy_all
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
STATIC int
_acl_field_entry_destroy_all(_acl_field_group_t *group) 
{
    int                 retval;

    assert(group   != NULL);

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_destroy_all(gid=%d)\n"),
               group->gid));

    while (group->entry_list != NULL) {
        retval = _acl_field_entry_destroy(group, group->entry_list->eid);
        if (BCM_FAILURE(retval)) {
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                      (BSL_META("ACL Error: _acl_field_entry_destroy() failure\n")));
            return retval;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _acl_field_entry_destroy
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
STATIC int
_acl_field_entry_destroy(_acl_field_group_t *group,
                         bcm_field_entry_t eid) 
{
    int                 retval;
    _acl_field_entry_t  *entry;
    _acl_field_entry_t  *prev = NULL;

    assert(group != NULL);
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_destroy(gid=%d, eid=%d)\n"),
               group->gid, eid));

    /* If the entry list is empty, Entry ID can't be found. */
    if (group->entry_list == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: No entries available to delete?\n")));
        return BCM_E_NOT_FOUND;
    }

    /* If entry is head of list, point around it and delete. */
    if (group->entry_list->eid == eid) {
        entry = group->entry_list;
        group->entry_list = group->entry_list->next;
        sal_free(entry);

        retval = bcmx_field_entry_destroy(eid);
        if (BCM_FAILURE(retval)) {
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                      (BSL_META("ACL Error: Entry ID=%d not destroyed\n"),
                       eid));
            return retval;
        }
        return BCM_E_NONE;
    }

    /* Scan entry list, looking for matching Entry ID. */
    entry = group->entry_list->next;
    prev  = group->entry_list;
    while (entry != NULL) {
        if (entry->eid == eid) {
            /* Found it so destroy it.*/
            prev->next = entry->next;

            /* Destroy range checker if needed. */
            if (entry->range != NULL) {
                _acl_field_range_check_destroy(entry);
            }

            sal_free(entry);

            retval = bcmx_field_entry_destroy(eid);
            if (BCM_FAILURE(retval)) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: Entry ID=%d not destroyed\n"),
                           eid));
                return retval;
            }
            return BCM_E_NONE;
        }
        prev  = entry;
        entry = entry->next;
    }

    LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL Error: Entry ID=%d not found to be destroyed\n"),
               eid));
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     _acl_field_entry_action_add
 *
 * Purpose:
 *     Extract a rule's actions and add them to the Field entry.
 *
 * Parameters:
 *     rule  - ACL rule
 *     eid   - Field entry ID
 *
 * Returns:
 */
STATIC int
_acl_field_entry_action_add(bcma_acl_rule_t *rule,
                            bcm_field_entry_t eid)
{
    assert(rule);
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_action_add(rule_id=%d, eid=%d)\n"),
               rule->rule_id, eid));

    /* Add Log if needed. */
    if (rule->actions.flags & BCMA_ACL_ACTION_LOG) {
        BCM_IF_ERROR_RETURN(
            bcmx_field_action_add(eid, bcmFieldActionCopyToCpu, 0, 0));
    }

    /* Add Deny if needed. */
    if (rule->actions.flags & BCMA_ACL_ACTION_DENY) {
        BCM_IF_ERROR_RETURN(
            bcmx_field_action_add(eid, bcmFieldActionDrop, 0, 0));
    }

    /* Add Redirect if needed. */
    if (rule->actions.flags & BCMA_ACL_ACTION_REDIR) {
        BCM_IF_ERROR_RETURN(
            bcmx_field_action_add(eid, bcmFieldActionRedirect,
                                  rule->actions.redir_port, 0));
    }

    /* Add Mirror if needed. */
    if (rule->actions.flags & BCMA_ACL_ACTION_MIRROR) {
        BCM_IF_ERROR_RETURN(
            bcmx_field_action_add(eid, bcmFieldActionMirrorIngress,
                                  rule->actions.mirror_port, 0));
    }
    /* Do nothing for Permit. It's enough to have the entry match and prevent
     * later entries from matching. */

    return BCM_E_NONE;
}

/* Section: Validation and Installation */

#define _ACL_FIELD_MERGE_RECOVER(_op_)   \
    do { int _rv_;                       \
        if ((_rv_ = (_op_)) < 0) {       \
            _acl_field_uninstall();      \
            return _rv_;                 \
        }                                \
    } while(0);
/*
 * Function: _acl_field_merge
 *
 * Purpose:
 *    Build a solution ready to install in the hardware. The main purpose of
 *    this is to test if the current set of ACLs will fit in hardware.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_RESOURCE - Not enough hardware resources (groups) to support the
 *                      current ACLs.
 */
int
_acl_field_merge(_acl_control_t *acl_control)
{
    int                   retval;
    _acl_link_t           *acl_link;
    bcma_acl_rule_t       *rule;
    bcm_field_qset_t      qset;
    _acl_field_group_t    *group;
    bcmx_lplist_t         lplist_full;
    _acl_rule_link_t      *rule_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_merge()\n")));
    ACL_FIELD_IS_INIT();

    assert(acl_control != NULL);
    bcmx_lplist_init(&lplist_full, 0, 0);
    bcmx_port_lplist_populate(&lplist_full, BCMX_PORT_LP_ALL);

    /* For each ACL */
    for (acl_link = _acl_first(acl_control);
         acl_link != NULL;
         acl_link = _acl_next(acl_control))
    {
        LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                    (BSL_META("ACL Merging ACL ID=%d\n"),
                     acl_link->list->list_id));

        /* Start at first group. */
        group = acl_field_control->group_list;

        /* For each rule in list...*/
        for (rule = _acl_rule_first(acl_link);
             rule != NULL;
             rule = _acl_rule_next(acl_link))
        {
            LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                        (BSL_META("ACL Merging Rule ID=%d\n"),
                         rule->rule_id));
            /* Initialization of total number field entries used by this rule */
            rule_link = _acl_rule_link_find(rule->rule_id);
            rule_link->entries = 0;

            /* If current group can not support rule, try:
             *    1. add necessary qualifiers to current group if possible.
             *    2. Try the next group.
             *    3. Try creating a new group.
             *    4. If no groups are left, return error for resources.
             */
            _ACL_FIELD_MERGE_RECOVER(_acl_field_rule_qset_get(rule, &qset));
            if (!bcmx_lplist_eq(&acl_link->list->lplist, &lplist_full)) {
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
            }

            retval = BCM_E_RESOURCE;

            while (group != NULL) {
                LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                            (BSL_META("ACL Setting group ID=%d\n"),
                             group->gid));
                retval = bcmx_field_group_set(group->gid, qset);

                if (BCM_SUCCESS(retval)) {
                    break;
                }

                group = group->next;
            }

            if (BCM_FAILURE(retval)) {
                LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                            (BSL_META("ACL Creating another group\n")));
                retval = _acl_field_group_create(acl_field_control, qset,
                                                 &group);
                if (BCM_FAILURE(retval)) {
                    bcmx_lplist_free(&lplist_full);
                    LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                              (BSL_META("ACL Error: Can't create group for rule_id=%d\n"),
                               rule->rule_id));
                    return retval;
                }
                LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                            (BSL_META("ACL Group (gid=%d) created.\n"),
                             group->gid));
            }

            /* Create FP entry(s) in current group to support rule.*/
            retval =_acl_field_entry_create(group, acl_link->list, rule);
            
            if (retval == BCM_E_CONFIG || retval == BCM_E_RESOURCE) {
                LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                            (BSL_META("ACL Creating another group\n")));
                retval = _acl_field_group_create(acl_field_control, qset,
                                                 &group);
                if (BCM_FAILURE(retval)) {
                    bcmx_lplist_free(&lplist_full);
                    LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                              (BSL_META("ACL Error: Can't create group for rule_id=%d\n"),
                               rule->rule_id));
                    return retval;
                }
                LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                            (BSL_META("ACL Group (gid=%d) created.\n"),
                             group->gid));
                _ACL_FIELD_MERGE_RECOVER(
                    _acl_field_entry_create(group, acl_link->list, rule));
            } else {
                _ACL_FIELD_MERGE_RECOVER(retval);
            }
        }
    }

    bcmx_lplist_free(&lplist_full);
    return BCM_E_NONE;
}
#undef _ACL_FIELD_MERGE_RECOVER

/*
 * Function:
 *     _acl_field_rule_qset_get
 *
 * Purpose:
 *     Calculate the Qset needed to support a rule.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int
_acl_field_rule_qset_get(bcma_acl_rule_t *rule, bcm_field_qset_t *qset)
{
    assert(rule  != NULL);
    assert(qset != NULL);

    BCM_FIELD_QSET_INIT(*qset);
        
    if (rule->flags & BCMA_ACL_RULE_L4_SRC_PORT || 
        rule->flags & BCMA_ACL_RULE_L4_DST_PORT) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyRangeCheck);
    }

    if (rule->flags & BCMA_ACL_RULE_SRC_MAC) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifySrcMac);
    }

    if (rule->flags & BCMA_ACL_RULE_DST_MAC) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDstMac);
    }

    if (rule->flags & BCMA_ACL_RULE_SRC_IP4) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifySrcIp);
    }

    if (rule->flags & BCMA_ACL_RULE_DST_IP4) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDstIp);
    }

    if (rule->flags & BCMA_ACL_RULE_SRC_IP6) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifySrcIp6);
    }

    if (rule->flags & BCMA_ACL_RULE_DST_IP6) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDstIp6);
    }

    if (rule->flags & BCMA_ACL_RULE_VLAN) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyOuterVlan);
    }

    if (rule->flags & BCMA_ACL_RULE_IPPROTOCOL) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyIpProtocol);
    }

    if (rule->flags & BCMA_ACL_RULE_ETHERTYPE) {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyEtherType);
    }

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_install
 *
 * Purpose:
 *     Write current ACL solution to hardware. Take working solution calculated
 *     by bcma_acl_merge() and commit it to hardware. This may be an expensive
 *     operation if multiple rules must be shuffled to maintain atomicity.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_XXX  - from bcmx_field_entry_install()
 *     BCM_E_NONE - Success
 */
int
_acl_field_install(void)
{
    int                    retval;
    _acl_field_group_t     *group;
    _acl_field_entry_t     *entry;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_install()\n")));
    ACL_FIELD_IS_INIT();

    assert(acl_field_control != NULL);

    /* For each group. */
    for (group = acl_field_control->group_list;
         group != NULL;
         group = group->next)
    {
        LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                    (BSL_META("ACL: Installing Group ID=%d\n"),
                     group->gid));
        /* For each entry in group. */
        for (entry = group->entry_list; entry != NULL; entry = entry->next) {
            /* Install the entry */
            LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                        (BSL_META("ACL: Installing Entry ID=%d\n"),
                         entry->eid));
            retval = bcmx_field_entry_install(entry->eid);
            if (BCM_FAILURE(retval)) {
                LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                          (BSL_META("ACL Error: ACL Field Entry ID=%d install failure.\n"),
                           entry->eid));
                return retval;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_uninstall
 *
 * Purpose:
 *     Remove the current set of ACLs from the hardware. This will also destroy
 *     the field groups and entries in software. Since the field groups and
 *     entries can always be reconstituted from the ACL rules.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_acl_field_uninstall(void)
{
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_uninstall()\n")));
    ACL_FIELD_IS_INIT();

    /* Remove the Field groups and entries from hardware. */
    if (BCM_FAILURE(_acl_field_group_remove_all())) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Failure in _acl_field_group_remove_all()\n")));
    }

    /* Deallocate the Field groups and entries from software. */
    if (BCM_FAILURE(_acl_field_group_destroy_all())) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Failure in _acl_field_group_destroy_all()\n")));
    }

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_rule_remove
 *
 * Purpose:
 *     Remove a single rule from hardware. In pratice this may mean removing
 *     several Field entries.
 *
 * Parameters:
 *     rule_id - ACL rule to remove from Field hardware
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Rule ID not found
 */
int
_acl_field_rule_remove(bcma_acl_rule_id_t rule_id) 
{
    int                   retval;
    _acl_field_group_t    *group_cur;
    bcm_field_entry_t     eid;
    
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_rule_remove(rule_id=%d)\n"),
               rule_id));
    ACL_FIELD_IS_INIT();

    /* While there are still Field entries that support the Rule ID */

    retval = _acl_field_rule_entry_find(rule_id, &group_cur, &eid);
    while (BCM_SUCCESS(retval)) {
        /* Remove entry from hardware. */
        BCM_IF_ERROR_RETURN(_acl_field_entry_destroy(group_cur, eid));
        BCM_IF_ERROR_RETURN(_acl_field_entry_remove(group_cur, eid));

        retval = _acl_field_rule_entry_find(rule_id, &group_cur,
                                            &eid);
    }

    if (retval != BCM_E_NOT_FOUND) {
        return retval;
    }

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_rule_entry_find
 *
 * Purpose:
 *     Find a Field Entry, along with its group, that supports a Rule ID.
 *
 * Parameters:
 *     rule_id   - rule ID to find
 *     group - (OUT) ACL Field group that has entries supporting rule
 *     eid   - (OUT) Entry ID that supports rule
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Rule ID not found
 */
STATIC int
_acl_field_rule_entry_find(bcma_acl_rule_id_t rule_id,
                           _acl_field_group_t **group_p,
                           bcm_field_entry_t *eid) 
{
    _acl_field_entry_t    *entry;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_rule_entry_find(rule_id=%d)\n"),
               rule_id));
    
    /* For each group. */
    for (*group_p = acl_field_control->group_list;
         *group_p != NULL;
         *group_p = (*group_p)->next)
    {
        /* For each entry in group. */
        for (entry = (*group_p)->entry_list; entry != NULL; entry = entry->next)
        {
            /* If entry references rule. */
            if (entry->rule->rule_id == rule_id) {
                *eid = entry->eid;
                return BCM_E_NONE;
            }
        }
    }

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL END _acl_field_rule_entry_find(rule_id=%d) NOT FOUND\n"),
               rule_id));
    return BCM_E_NOT_FOUND;
}

#ifdef BROADCOM_DEBUG

/*
 * Function: _acl_field_show
 *
 * Purpose:
 *     Display the Field specific data for ACL.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_XXX  - return value from _acl_field_group_show()
 */
int
_acl_field_show(void) {
    _acl_field_group_t     *group;
    int                    group_count = 0;
    
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_show()\n")));
    ACL_FIELD_IS_INIT();

    group = acl_field_control->group_list;
    while (group != NULL) {
        group = group->next;
        group_count++;
    }

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("acl_field={group_numb=%d, "),
              group_count));

    /* Show groups used by ACLs. */
    group = acl_field_control->group_list;

    while (group != NULL) {
        BCM_IF_ERROR_RETURN(_acl_field_group_show(group));
        group = group->next;
    }

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("}\n")));

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_group_show
 *
 * Purpose:
 *     Display a single access control list.
 *
 * Parameters:
 *     group - group to desplay
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - List ID not found
 *     BCM_E_XXX       - return value from bcma_acl_rule_show()
 */
STATIC int
_acl_field_group_show(_acl_field_group_t *group)
{
    _acl_field_entry_t  *entry;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_group_show(gid=%d)\n"),
               group->gid));
    assert(group != NULL);
    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("\tgroup={gid=%d,\n"),
              group->gid));

    /* Show entries in this group.*/
    entry = group->entry_list;
    while (entry != NULL) {
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("\t\t")));
        BCM_IF_ERROR_RETURN(_acl_field_entry_show(entry));
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("\n")));
        entry = entry->next;
    }

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("\t},\n")));

    return BCM_E_NONE;
}

/*
 * Function: _acl_field_entry_show
 *
 * Purpose:
 *     Display a single entry.
 *
 * Parameters:
 *     entry - Field entry struct
 *
 * Returns:
 *     BCM_E_NONE      - Success
 */
STATIC int
_acl_field_entry_show(_acl_field_entry_t *entry)
{
    
    assert(entry != NULL);
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_field_entry_show(eid=%d)\n"),
               entry->eid));
    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("entry={eid=%d, rule_id=%d, pri=%d},"),
              entry->eid,
              entry->rule->rule_id, entry->prio));

    return BCM_E_NONE;
}
#endif /* BROADCOM_DEBUG */
#endif /* INCLUDE_ACL */
