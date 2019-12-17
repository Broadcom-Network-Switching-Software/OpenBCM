/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        acl.c
 * Purpose:     
 *     Advanced ContentAware Enhanced Software (ACES) implementation.
 */

#if defined(INCLUDE_ACL)

#include <shared/bsl.h>

#include <bcm/error.h>

#include <sal/core/libc.h>

#include <shared/util.h>

#include <bcmx/lport.h>
#include <bcmx/lplist.h>

#include <appl/acl/acl.h>
#include <appl/diag/system.h>

#include "acl_field.h"
#include "acl_util.h"

/* Macros */

/*
 * Macro: ACL_IS_INIT (internal)
 *
 * Purpose:
 *     Confirm that the ACL functions are initialized.
 *
 * Parameters:
 *
 * Notes:
 *     Results in return(BCM_E_UNAVAIL), or
 *     return(BCM_E_INIT) if fails.
 */
#define ACL_IS_INIT() do {                                              \
        if (_acl_control == NULL) {                                     \
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,                       \
                      (BSL_META("ACL Error: ACL not initialized\n")));  \
            return BCM_E_INIT;                                          \
        }                                                               \
    } while (0)

/*
 * Macro:
 *     ACL_LOCK/ACL_UNLOCK  (internal)
 * Purpose:
 *     Lock the ACL module.
 *
 * Parameters:
 */
#define ACL_LOCK() \
    sal_mutex_take(_acl_control->acl_lock, sal_mutex_FOREVER)

#define ACL_UNLOCK(_c_) \
    sal_mutex_give((_c_)->acl_lock)


#define ACL_RETVAL_REPLACE(_retval_temp, _retval)                      \
        if ((_retval_temp) == BCM_E_INIT) {                            \
            (_retval_temp) = BCM_E_UNAVAIL;                            \
        }                                                              \
        if ((_retval_temp) < 0 && (_retval_temp) != BCM_E_UNAVAIL) {   \
            (_retval) = (_retval_temp);                                \
        } 



/* Local Prototypes */
STATIC int _acl_rule_validate (const bcma_acl_rule_t *rule);
STATIC _acl_link_t* _acl_find(bcma_acl_list_id_t list_id);
STATIC _acl_link_t* _acl_link_alloc(void);

STATIC _acl_rule_link_t *_acl_rule_find(bcma_acl_rule_id_t rule_id);
STATIC _acl_rule_link_t *_acl_rule_link_alloc(void);

STATIC int _acl_sort(void);
STATIC int _acl_cmp(void *a, void *b);
STATIC int _acl_merge(void);
STATIC int _acl_range_reduce(uint16 min, uint16 max, acl_node_t **list);

/* Control Globals */
static _acl_control_t *_acl_control;

#ifdef BROADCOM_DEBUG
static char *bcma_acl_ip_protocol_text[256] = {
  "HOPBYHOP",    "ICMP",         "IGMP",         "GGP",    
  "IP",          "ST",           "TCP",          "UCL",
  "EGP",         "IGP",          "BBN-RCC-MON",  "NVP_II",
  "PUP",         "ARGUS",        "EMCON",        "XNET",
  "CHAOS",       "UDP",          "MUX",          "DCN_MEAS",
  "HMP",         "PRM",          "XNS_IDP",      "TRUNK-1",
  "TRUNK-2",     "LEAF1",        "LEAF2",        "RDP",
  "IRTP",        "ISO_TP4",      "NETBLT",       "MFE-NSP",
  "MERIT_INP",   "SEP",          "3PC",          "IDRP",
  "XTP",         "DDP",          "IDPR_CMTP",    "TP++",
  "IL",          "SIP",          "SDRP",         "SIP-SR",
  "SIP-FRAG",    "IDRP",         "RSVP",         "GRE",
  "MHRP",        "BNA",          "SIPP_ESP",     "SIPP_AH",
  "I-NLSP",      "SWIPE",        "NHRP",         "",
  "",            "",             "",             "", 
  "",            "AHIP",         "CFTP",         "HI",
  "SAT_EXPAK",   "KRYPTOLAN",    "RVD",          "IPPC",
  "ADFS",        "SAT_MON",      "VISA",         "IPCV",
  "CPNX",        "CPHB",         "WSN",          "PVP",
  "BR_SAT_MON",  "SUN_ND",       "WB_MON",       "WB_EXPAK",
  "ISO_IP",      "VMTP",         "SECURE_VMTP",  "VINES",
  "TTP",         "NSFNET_IGP",   "DGP",          "TCF",
  "IGRP",        "OSPFIGP",      "SPRITE_RPC",   "LARP",
  "MTP",         "AX_25",        "IPIP",         "MICP",
  "SCC_SP",      "ETHERIP",      "ENCAP",        "APES",
  "GMTP",        "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "", 
  "",      "",       "",       "ANY"
};
#endif /* BROADCOM_DEBUG */

/* Section: Constructor and destructor for ACL (ACES) module */

/*
 * Function: bcma_acl_init
 *
 * Purpose:
 *     Initializes ACES.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Allocation failure
 */
int
bcma_acl_init(void) 
{
    _acl_control_t      *control;
    int                 retval = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_init()\n")));

    /* Detach first if it has been previously initialized. */
    if (_acl_control != NULL) {
        BCM_IF_ERROR_RETURN(bcma_acl_uninstall());
        BCM_IF_ERROR_RETURN(bcma_acl_detach());
    }

    control = sal_alloc(sizeof(_acl_control_t), "ACL Control");
    if (control == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: allocation failure for ACL control.\n")));
        return BCM_E_MEMORY;
    }
    sal_memset(control, 0, sizeof(_acl_control_t));

    control->acl_lock = sal_mutex_create("ACL_control.lock");
    if (control->acl_lock == NULL) {
        sal_free(control);
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: creation failure for ACL lock.\n")));
        return BCM_E_MEMORY;
    }

    /* Initialize control structure. */
    _acl_control = control;
    ACL_LOCK();
    control->head.next = NULL;

#ifdef BCM_FIELD_SUPPORT
    {
    int retval_temp;
    retval_temp = _acl_field_init();
    ACL_RETVAL_REPLACE(retval_temp, retval);
    }
#endif /* BCM_FIELD_SUPPORT */

    ACL_UNLOCK(_acl_control);

    return retval;
}

/*
 * Function: bcma_acl_detach
 *
 * Purpose:
 *     Remove any lists and free lock and control.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_INIT - ACL module not initialized
 *     BCM_E_XXX  - from bcma_acl_remove()
 */
int
bcma_acl_detach(void)
{
    _acl_control_t      *control;
    int                 retval = BCM_E_NONE, retval_temp;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_detach()\n")));
    ACL_IS_INIT();

    control = _acl_control;

    ACL_LOCK();

    /* Do hardware specific detach. */
#ifdef BCM_FIELD_SUPPORT
    retval_temp = _acl_field_detach();
    ACL_RETVAL_REPLACE(retval_temp, retval);
#endif /* BCM_FIELD_SUPPORT */

    /* Remove lists . */
    while (_acl_control->head.next != NULL && BCM_SUCCESS(retval)) {
        retval_temp = bcma_acl_remove(_acl_control->head.next->list->list_id);
        ACL_RETVAL_REPLACE(retval_temp, retval);
    }

    _acl_control = NULL;

    ACL_UNLOCK(control);

    sal_mutex_destroy(control->acl_lock);
    sal_free(control);

    return retval;
}

/* Section: List accessor functions. */

/*
 * Function: bcma_acl_add
 *
 * Purpose:
 *     Adds a new ACL to ACES.
 *
 * Parameters:
 *     list - pointer to ACL data
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_INIT   - ACL module not initialized
 *     BCM_E_EXISTS - Duplicate list ID already exists
 *     BCM_E_MEMORY - Allocation failure for list link.
 */
int
bcma_acl_add(bcma_acl_t *list)
{
    _acl_link_t         *list_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_add()\n")));
    ACL_IS_INIT();

    if (list == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: null list pointer in bcma_acl_add()\n")));
        return BCM_E_PARAM;
    }

    /* Confirm that list ID does not already exist. */
    if (_acl_find(list->list_id) != NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: List ID=%d already exists\n"),
                   list->list_id));
        return BCM_E_EXISTS;
    }

    /* Allocate a node for linked-list of ACLs and copy list. */
    list_link = _acl_link_alloc();
    if (list_link == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: allocation failure for new list node.\n")));
        return BCM_E_MEMORY;
    }
    list_link->list = list;

    /* Link the new list and add it to the linked-list. */
    ACL_LOCK();

    /* Test for no lists. */
    if (_acl_control->head.next == NULL) {
        _acl_control->head.next = list_link;
        list_link->prev = &_acl_control->head;
    } else {
        list_link->next = _acl_control->head.next;
        /* Insert at head. */
        if (_acl_control->head.next != NULL) {
            _acl_control->head.next->prev = list_link;
        }

        _acl_control->head.next = list_link;
        list_link->prev         = &_acl_control->head;
    }

    _acl_control->cur   = list_link;
    ACL_UNLOCK(_acl_control);

    return BCM_E_NONE;
}

/*
 * Function: bcma_acl_rule_add
 *
 * Purpose:
 *     Add a rule to a list of rules.
 *
 * Parameters:
 *     list_id  - List ID
 *     *rule - rule to be added to List
 *
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_INIT       - ACL module not initialized
 *     BCM_E_PARAM      - NULL rule pointer.
 *     BCM_E_NOT_FOUND  - List ID not found 
 *     BCM_E_MEMORY     - Allocation failure for rule.
 *     BCM_E_EXISTS     - Rule ID already exists.
 */
int
bcma_acl_rule_add(bcma_acl_list_id_t list_id, bcma_acl_rule_t *rule)
{
    _acl_link_t         *list_link;
    _acl_rule_link_t    *rule_link_iter = NULL,
                        *rule_link      = NULL;
    
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_add(list_id=%d, rule=%p)\n"),
               list_id, (void *)rule));
    ACL_IS_INIT();

    BCM_IF_ERROR_RETURN(_acl_rule_validate(rule));

    ACL_LOCK();
    /* Find the list to add this rule to. */
    list_link = _acl_find(list_id);
    if (list_link == NULL) {
        ACL_UNLOCK(_acl_control);
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: List ID=%d not found\n"),
                   list_id));
        return BCM_E_NOT_FOUND;
    }

    if (_acl_rule_find(rule->rule_id) != NULL) {
        ACL_UNLOCK(_acl_control);
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Rule ID=%d already exists\n"),
                   rule->rule_id));
        return BCM_E_EXISTS;
    }

    /* Make a copy of the input rule. */
    /* Create a rule link. */
    rule_link = _acl_rule_link_alloc();
    if (rule_link == NULL) {
        ACL_UNLOCK(_acl_control);
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Allocation failure in _acl_rule_link_alloc()\n")));
        return BCM_E_MEMORY;
    }
    rule_link->rule = rule;
    rule_link->next = NULL;

    /* If list has no rules, add new rule link to the head. */
    if (list_link->rules == NULL) {
        list_link->rules = rule_link;
        ACL_UNLOCK(_acl_control);
        LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL bcma_acl_rule_add() END\n")));
        return BCM_E_NONE;
    }
    
    /* Find the tail. */
    rule_link_iter = list_link->rules;
    while (rule_link_iter->next != NULL) {
         rule_link_iter = rule_link_iter->next;
    }

    /* Put new rule link at tail of linked-list */
    rule_link_iter->next = rule_link;

    /* Make this the current rule (for iterator functions). */
    list_link->cur       = rule_link;

    ACL_UNLOCK(_acl_control);

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_add() END\n")));
    return BCM_E_NONE;
}

#define _ACL_L4_MIN  (0)
#define _ACL_L4_MAX  ((1 << 16) - 1)

#define _ACL_L4_PORT_VALIDATE(_port_)                                   \
    do {                                                                \
        if ((_port_) < _ACL_L4_MIN || _ACL_L4_MAX < (_port_)) {         \
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,                       \
                      (BSL_META("ACL Error: L4 port=%d out of range\n"), \
                       (_port_)));                                      \
            return BCM_E_PARAM;                                         \
        }                                                               \
    } while (0)

/*
 * Function:
 *    _acl_rule_validate
 *
 * Purpose:
 *     Confirms fields within the rule are in acceptible ranges.
 *
 * Parameters:
 *     rule - rule structure to be evaluated.
 *
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_PARAM      - Out-of-range field(s) in rule
 */
STATIC int
_acl_rule_validate(const bcma_acl_rule_t *rule)
{
    if (rule == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: rule=NULL\n")));
        return BCM_E_PARAM;
    }

    _ACL_L4_PORT_VALIDATE(rule->src_port_min);
    _ACL_L4_PORT_VALIDATE(rule->src_port_max);
    _ACL_L4_PORT_VALIDATE(rule->dst_port_min);
    _ACL_L4_PORT_VALIDATE(rule->dst_port_max);

    return BCM_E_NONE;
}
#undef _ACL_L4_MIN
#undef _ACL_L4_MAX
#undef _ACL_L4_PORT_VALIDATE

/*
 * Function: bcma_acl_rule_remove
 *
 * Purpose:
 *     Remove an rule from a list.
 *
 * Parameters:
 *     list_id  - List ID where rule exists
 *     rule_id  - Rule ID to be removed
 *
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_INIT       - ACL module not initialized
 *     BCM_E_NOT_FOUND  - List ID or Rule ID not found 
 */
int
bcma_acl_rule_remove(bcma_acl_list_id_t list_id, bcma_acl_rule_id_t rule_id)
{
    _acl_link_t         *list_link;
    _acl_rule_link_t    *rule_link_cur, *rule_link_prev;
    int                 retval = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_remove(list_id=%d, rule_id=%d)\n"),
               list_id, rule_id));
    ACL_IS_INIT();

    ACL_LOCK();
    list_link = _acl_find(list_id);

    if (list_link == NULL) {
        ACL_UNLOCK(_acl_control);
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: List ID=%d not found\n"),
                   list_id));
        return BCM_E_NOT_FOUND;
    }

    /* Traverse the list of rule links, looking for matching RID to remove. */
    for (rule_link_prev = NULL, rule_link_cur = list_link->rules;
         rule_link_cur != NULL;
         rule_link_prev = rule_link_cur, rule_link_cur = rule_link_cur->next) {
        if (rule_link_cur->rule->rule_id == rule_id) {
            /* Point around old rule_link */
            if (rule_link_prev == NULL) {
                /* At head of list. */
                list_link->rules = rule_link_cur->next;
            } else {
                /* Somewhere within list. */
                rule_link_prev->next = rule_link_cur->next;
            }
            /* Remove the Rule ID from hardware. */
#ifdef BCM_FIELD_SUPPORT
            {
            int retval_temp;
            retval_temp = _acl_field_rule_remove(rule_id);
            ACL_RETVAL_REPLACE(retval_temp, retval);
            }
#endif /* BCM_FIELD_SUPPORT */

            /* Remove the rule_link from software. */
            sal_free(rule_link_cur);
            ACL_UNLOCK(_acl_control);
            return retval;
        }
    }

    ACL_UNLOCK(_acl_control);
    LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL Error: Rule ID=%d not found in List ID=%d\n"),
               rule_id, list_id));
    return BCM_E_NOT_FOUND;
}

/*
 * Function: bcma_acl_rule_get
 *
 * Purpose:
 *     Accessor function to get a Rule
 *
 * Parameters:
 *     rule_id - Rule ID
 *     rule    - (OUT) returned rule info
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_INIT      - ACL module not initialized
 *     BCM_E_PARAM     - rule pointer is NULL
 *     BCM_E_NOT_FOUND - Rule ID not found
 */
int
bcma_acl_rule_get(bcma_acl_rule_id_t rule_id, bcma_acl_rule_t **rule)
{
    _acl_rule_link_t    *rule_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_get(rule_id=%d, rule=%p)\n"), 
               rule_id, (void *)rule));
    ACL_IS_INIT();

    if (*rule == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: *rule is NULL\n")));
        return BCM_E_PARAM;
    }

    rule_link = _acl_rule_find(rule_id);

    if (rule_link == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Rule ID=%d not found\n"),
                   rule_id));
        return BCM_E_NOT_FOUND;
    }

    *rule = rule_link->rule;

    return BCM_E_NONE;
}

/*
 * Function: bcma_acl_remove
 *
 * Purpose:
 *     Remove ACL from list of ACLs. This removes the Filter and Field entries
 *     used to support any rules in this list. Then it frees internal link
 *     memory used by ACL, including the rule links in the list. 
 *
 * Parameters:
 *     list_id  - List ID to be removed
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_INIT      - ACL module not initialized
 *     BCM_E_NOT_FOUND - List ID not found
 */
int
bcma_acl_remove(bcma_acl_list_id_t list_id)
{
    _acl_link_t         *link_cur;
    int                 retval;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_remove(list_id=%d)\n"),
               list_id));
    ACL_IS_INIT();

    ACL_LOCK();
    link_cur = _acl_find(list_id);
    if (link_cur == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: List ID=%d not found\n"),
                   list_id));
        ACL_UNLOCK(_acl_control);
        return BCM_E_NOT_FOUND;
    }

    /* Remove rules in list. */
    while (link_cur->rules != NULL) {
        retval = bcma_acl_rule_remove(list_id, link_cur->rules->rule->rule_id);
        if (BCM_FAILURE(retval)) {
            ACL_UNLOCK(_acl_control);
            return retval;
        }
    }

    /* Remove the list link node itself. */
    if (link_cur->next != NULL) {
        link_cur->next->prev = link_cur->prev;
    }
    link_cur->prev->next = link_cur->next;
    sal_free(link_cur);

    ACL_UNLOCK(_acl_control);
    return BCM_E_NONE;
}

/*
 * Function: bcma_acl_get
 *
 * Purpose:
 *     Accessor function for a ACL
 *
 * Parameters:
 *     list_id   - List ID
 *     list  - (OUT) returned list info
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_INIT      - ACL module not initialized
 *     BCM_E_PARAM     - list pointer is NULL
 *     BCM_E_NOT_FOUND - List ID not found
 */
int
bcma_acl_get(bcma_acl_list_id_t list_id, bcma_acl_t *list)
{
    _acl_link_t         *list_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_get(list_id=%d, list=%p)\n"),
               list_id,
               (void *)list));
    ACL_IS_INIT();

    if (list == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: *list is NULL\n")));
        return BCM_E_PARAM;
    }

    list_link = _acl_find(list_id);
    if (list_link == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: List ID=%d not found\n"),
                   list_id));
        return BCM_E_NOT_FOUND;
    }

    sal_memcpy(list, &list_link->list, sizeof(bcma_acl_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _acl_control_find
 *
 * Purpose:
 *     Find a ACL control
 *
 * Parameters:
 *
 * Returns:
 *     pointer to _acl_control_t on success
 *     NULL on failure
 */
_acl_control_t *
_acl_control_find(void) {
    return _acl_control;
}

/*
 * Function:
 *     _acl_first
 *
 * Purpose:
 *     Return the first link in list of ACLs. Along with _acl_next(),
 *     it is meant to be used in a for() loop as the initializer.
 *
 * Parameters:
 *     *control - ACES control
 *
 * Returns:
 *     pointer to the first _acl_link_t structure.
 *     NULL on failure
 */
_acl_link_t *
_acl_first(_acl_control_t *control) {
    assert(control != NULL);
    assert(control != (_acl_control_t*)0xffffffff);

    control->cur = control->head.next;
    return control->cur;
}
/*
 * Function:
 *     _acl_next
 *
 * Purpose:
 *     Return the next link in list of ACLs.
 *
 * Parameters:
 *     control - ACES control
 *
 * Returns:
 *     pointer to the first _acl_link_t structure.
 *     NULL at end of list
 */

_acl_link_t *
_acl_next(_acl_control_t *control) {
    assert(control != NULL);
    assert(control != (_acl_control_t*)0xffffffff);
    assert(control->cur != NULL);

    control->cur = control->cur->next;
    return control->cur;
}


/*
 * Function:
 *     _acl_find
 *
 * Purpose:
 *     Find an ACL given on its ID. 
 *
 * Parameters:
 *     list_id  - ACL identifier
 *
 * Returns:
 *     pointer to bcma_acl_struct on success
 *     NULL on failure
 */
STATIC _acl_link_t*
_acl_find(bcma_acl_list_id_t list_id) {
    _acl_link_t         *acl_iter;

    acl_iter = _acl_control->head.next;

    while (acl_iter != NULL) {
        if (acl_iter->list->list_id == list_id) {
            return acl_iter;
        }
        acl_iter = acl_iter->next;
    }

    return NULL;
}

/*
 * Function:
 *     _acl_link_alloc
 *
 * Purpose:
 *     Allocates the space for a list node and performs a deep copy of input
 *     list.
 *
 * Parameters:
 *     list - pointer to Access Control List to add to ACL.
 *
 * Returns:
 *     pointer to _acl_link_t struct on success
 *     NULL on failure
 */
STATIC _acl_link_t*
_acl_link_alloc(void) {
    _acl_link_t         *list_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_link_alloc()\n")));
    list_link = sal_alloc(sizeof(_acl_link_t), "ACL list node");
    if (list_link == NULL) {
        return list_link;
    }

    sal_memset(list_link, 0, sizeof(_acl_link_t));
    list_link->rules = NULL;

    return list_link;
}

/*
 * Function:
 *     _acl_rule_find
 *
 * Purpose:
 *     Find a rule link given an Rule ID. This searches all lists
 *     and returns the first rule link that has a matching rule ID.
 *
 * Parameters:
 *     rule_id  - Rule identifier
 *
 * Returns:
 *     pointer to _acl_rule_link_t on success
 *     NULL on failure
 */
STATIC _acl_rule_link_t*
_acl_rule_find(bcma_acl_rule_id_t rule_id) {
    _acl_link_t         *list_link = NULL;
    _acl_rule_link_t    *rule_link = NULL;

    /* Traverse the lists. */
    for (list_link = _acl_control->head.next;
         list_link != NULL;
         list_link = list_link->next) {
        for (rule_link = list_link->rules;
             rule_link != NULL;
             rule_link = rule_link->next) {
            if (rule_link->rule->rule_id == rule_id) {
                break;
            }
        }
    }

    return rule_link;
}

/*
 * Function:
 *     _acl_rule_first
 *
 * Purpose:
 *     Get the first rule in the list. This also moves the current rule pointer
 *     to the first rule.
 *
 * Parameters:
 *     list_link - Wrapper to ACL where rule is.
 *
 * Returns:
 *     pointer to _acl_rule_link_t on success
 *     NULL on failure
 */
bcma_acl_rule_t *
_acl_rule_first(_acl_link_t *list_link)
{
    assert(list_link != NULL);

    list_link->cur = list_link->rules;

    if (list_link->cur != NULL) {
        return list_link->rules->rule;
    }

    return NULL;
}

/*
 * Function:
 *     _acl_rule_next
 *
 * Purpose:
 *     Get the next rule in the list. This also increments the current rule
 *     pointer.
 *
 * Parameters:
 *     list_link - Wrapper to ACL where rule is.
 *
 * Returns:
 *     pointer to _acl_rule_link_t on success
 *     NULL on failure
 */
bcma_acl_rule_t *
_acl_rule_next(_acl_link_t *list_link)
{
    assert(list_link != NULL);
    assert(list_link->cur != NULL);

    list_link->cur = list_link->cur->next;

    if (list_link->cur != NULL) {
        return list_link->cur->rule;
    } 

    return NULL;
}

_acl_rule_link_t *
_acl_rule_link_find(bcma_acl_rule_id_t rule_id) 
{
    _acl_link_t           *acl_link;
    _acl_rule_link_t      *rule_link;

    /* For each ACL */
    for (acl_link = _acl_first(_acl_control);
        acl_link != NULL;
        acl_link = _acl_next(_acl_control))
    {
        /* For each rule_link in list...*/
        for (rule_link = acl_link->rules;
            rule_link != NULL;
            rule_link = rule_link->next)
        {
            if (rule_link->rule->rule_id == rule_id) {
                return rule_link;
            }
 
        }
    }

    return NULL;
}

/*
 * Function:
 *    _acl_rule_link_alloc
 *
 * Purpose:
 *    Allocate a rule link and copy the source rule data into it.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     pointer to newly allocated duplicate rule
 *     NULL on allocation failure 
 *
 * Note:
 *     next pointer of copy is NULL
 */
STATIC _acl_rule_link_t *
_acl_rule_link_alloc(void) {
    _acl_rule_link_t    *rule_link;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_rule_link_alloc()\n")));
    rule_link = sal_alloc(sizeof(_acl_rule_link_t), "ACL rule link");
    if (rule_link == NULL) {
        return NULL;
    }
    sal_memset(rule_link, 0, sizeof(_acl_rule_link_t));

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_rule_link_alloc() returning=%p\n"),
               (void *)rule_link));
    return rule_link;
}

/* Section: Installation */
/*
 * Function: _acl_sort
 *
 * Purpose:
 *     Sorting of link list of ACLs into descending ordr of priorities. This
 *     is necessary because priority values can be changed by the client code.
 *     Even if they're inserted in order, the priorities are not guarranteed
 *     to be correct at install time.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_MEMORY   - Allocation failure
 */
STATIC int
_acl_sort(void)
{
    _acl_link_t         *array;   /* Array of links to be sorted. */
    int                 count = 0;  /* Number of ACLs. */
    int                 idx;

    /* Count the number of ACLs. */
    for (_acl_control->cur = _acl_control->head.next, count = 0;
         _acl_control->cur != NULL;
         _acl_control->cur = _acl_control->cur->next, count++) {
    }

    /* Create an array of pointers to _acl_link_t structures. */
    array = (_acl_link_t*)sal_alloc(sizeof(_acl_link_t) * count,
                                     "ACL_sort_array"); 
    if (array == NULL) {
        return BCM_E_MEMORY;
    }

    /* Copy to the array for sorting */
    _acl_control->cur = &_acl_control->head;
    for (idx = 0; idx < count; idx++) {
        assert(_acl_control->cur != NULL);
        sal_memcpy(&array[idx], _acl_control->cur->next, sizeof(_acl_link_t));

        /* We borrow the 'prev' pointer in the sorting array for
         * identifying the element 
         */
        array[idx].prev = _acl_control->cur->next;

        _acl_control->cur = _acl_control->cur->next;
    }

    /* Sort the array. */
    _shr_sort(array, count, sizeof(_acl_link_t), _acl_cmp);

    /* Rewire the linked list in sorted order. */
    _acl_control->cur = &_acl_control->head;
    if (count) {
        /* 
         * head->next should be set to the element with highest 
         * priority after sorted.
         */
        _acl_control->cur->next = array[0].prev;
     }

    /* Re-construct the chain */
    for (idx = 0; idx < count; idx++) {
        sal_memcpy(_acl_control->cur->next, &array[idx], sizeof(_acl_link_t));
        if (idx == 0) {
            _acl_control->cur->next->prev = &_acl_control->head;
        } else {
            /* Remember we borrow the 'prev' to locate the actual element */
            _acl_control->cur->next->prev = array[idx - 1].prev;
        }
        if (idx < count - 1) {
            /* Remember we borrow the 'prev' to locate the actual element */
            _acl_control->cur->next->next = array[idx + 1].prev;
        } else {
            _acl_control->cur->next->next = NULL;
        }
        _acl_control->cur = _acl_control->cur->next;
    }

    /* Destroy the array. */
    sal_free(array);

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL END _acl_sort()\n")));

    return BCM_E_NONE;
}

/*
 * Function: _acl_cmp
 *
 * Purpose:
 *     Callback function for _shr_sort(). This compares two lists and returns
 *     the usual Unix -1, 0, or +1 values. The only difference is that the -1
 *     and +1 are reversed so the list will be arranged highest priority first.
 *
 * Parameters:
 *     a - Pointer to first list link
 *     b - Pointer to second list link
 *
 * Returns:
 *     -1  - if A has higher priority than B 
 *      0  - if A and B have equal priority
 *     +1  - if B has higher priority than A
 */
STATIC int
_acl_cmp(void *a, void *b)
{
    _acl_link_t        *list_first, *list_second;

    list_first  = (_acl_link_t*)a;
    list_second = (_acl_link_t*)b;
    
    if (list_first->list->prio > list_second->list->prio) {
        return -1;
    } else if (list_first->list->prio < list_second->list->prio) {
        return 1;
    }

    return 0;
}

/*
 * Function: _acl_merge
 *
 * Purpose:
 *    Build a solution ready to install in the hardware. The main purpose of
 *    this is to test if the current set of ACLs will fit in hardware.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INIT     - ACL module not initialized
 *     BCM_E_RESOURCE - Insufficient hardware resources for current Lists.
 */
STATIC int
_acl_merge(void)
{
    int                 retval = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL _acl_merge()\n")));
    ACL_IS_INIT();

    ACL_LOCK();

#ifdef BCM_FIELD_SUPPORT
    {
    int retval_temp;
    
    retval_temp = _acl_field_merge(_acl_control);
    ACL_RETVAL_REPLACE(retval_temp, retval);
    }
#endif /* BCM_FIELD_SUPPORT */

    ACL_UNLOCK(_acl_control);
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL END _acl_merge()\n")));

    return retval;
}

/*
 * Function: bcma_acl_install
 *
 * Purpose:
 *     Write current ACL solution to hardware. Take working solution calculated
 *     by _acl_merge() and commit it to hardware. This may be an expensive
 *     operation if multiple rules must be shuffled to maintain atomicity.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INIT     - ACL module not initialized
 *     BCM_E_RESOURCE - Insufficient hardware resources to support current
 *                      Lists.
 *     BCM_E_MEMORY   - Allocation failure
 */
int
bcma_acl_install(void)
{
    int                 retval = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_install()\n")));
    ACL_IS_INIT();

    ACL_LOCK();

    BCM_IF_ERROR_RETURN(bcma_acl_uninstall());
    BCM_IF_ERROR_RETURN(_acl_sort());
    BCM_IF_ERROR_RETURN(_acl_merge());

#ifdef BCM_FIELD_SUPPORT
    {
    int retval_temp;
    retval_temp = _acl_field_install();
    ACL_RETVAL_REPLACE(retval_temp, retval);
    }
#endif /* BCM_FIELD_SUPPORT */

    ACL_UNLOCK(_acl_control);

    return retval;
}

/*
 * Function: bcma_acl_uninstall
 *
 * Purpose:
 *     Remove the current set of ACLs from the hardware.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INIT     - ACL module not initialized
 */
int
bcma_acl_uninstall(void)
{
    int                 retval = BCM_E_NONE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_uninstall()\n")));
    ACL_IS_INIT();
    ACL_LOCK();

#ifdef BCM_FIELD_SUPPORT
    {
    int retval_temp;
    retval_temp = _acl_field_uninstall();
    ACL_RETVAL_REPLACE(retval_temp, retval);
    }
#endif /* BCM_FIELD_SUPPORT */

    ACL_UNLOCK(_acl_control);
    return retval;
}

/*
 * Function: acl_range_to_list
 *
 * Purpose:
 *     Generate a mininal linked list of data/mask pairs that cover the
 *     specified range.
 *
 * Parameters:
 *     min   - lower limit of range
 *     max   - upper limit of range
 *     list  - (OUT) pointer to head of list
 *     count - (OUT) number of nodes in list
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Allocation failure.
 */
int
acl_range_to_list(uint16 min, uint16 max, acl_node_t **list_p, int *count) {
    acl_node_t          **list_temp;
    int                 idx;
    
    if (list_p == NULL || count == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: list or count is NULL\n")));
        return BCM_E_PARAM;
    }
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL acl_range_to_list(min=%d, max=%d)\n"),
               min, max));
    *list_p = NULL;

    *count = _acl_range_reduce(min, max, (acl_node_t **)NULL);

    /* Allocate a temporary array of pointers to nodes. */
    list_temp = (acl_node_t **)sal_alloc(*count * sizeof(acl_node_t *),
                                         "ACL temp_list");
    if (list_temp == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(list_temp, 0, *count * sizeof(acl_node_t *));

    /* Allocate nodes. */
    for (idx = 0; idx < *count; idx++) {
        list_temp[idx] = (acl_node_t *)sal_alloc(sizeof(acl_node_t),
                         "ACL range node");
        sal_memset(list_temp[idx], 0, sizeof(acl_node_t));
        if (list_temp[idx] == NULL) {
            return BCM_E_MEMORY;
        }

        /* Link nodes into list. */
        if (idx > 0) {
            list_temp[idx - 1]->next = list_temp[idx];
        }
    }

    *list_p = list_temp[0];
    (void) _acl_range_reduce(min, max, list_temp);

    sal_free(list_temp);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _acl_range_reduce
 *
 * Purpose:
 *     Create a minimum set of data/mask nodes given a min and max range.
 *     This function needs to be called twice. First it should be called with
 *     list==NULL to establish the count. Then it should be called to fill
 *     in the list array with data/mask nodes.
 *
 * Parameters:
 *     min   - lower limit of range
 *     max   - upper limit of range
 *     list  - (OUT) pointer to head of list of nodes
 *
 * Returns:
 *     number of element data/mask nodes
 */
STATIC int
_acl_range_reduce(uint16 min, uint16 max, acl_node_t **list)
{
    int         nentry;
    uint16      temp, mask;

    nentry = 0;

    while (min <= max) {
        /* count low order 0 bits in min */
        if (min == 0) {
            mask = 0xffff;
        } else {
            for (temp = min, mask = 0; (temp & 1) == 0; temp >>= 1) {
                mask = (mask << 1) | 1;
            }
        }
        temp = (min & ~mask) | mask;
        if (temp <= max) {
            ;
        } else {
            while (temp > max) {
                mask >>= 1;
                temp = (min & ~mask) | mask;
            }
        }
        if (list) {
            list[nentry]->data = min;
            list[nentry]->mask = ~mask & 0xffff;
        }
        nentry += 1;
        if (temp >= max) {
            break;
        }
        min = temp + 1;
    }

    return nentry;
}

/*
 * Function: acl_range_destroy
 *
 * Purpose:
 *     Deallocate memory used by previously allocated range list of nodes.
 *
 * Parameters:
 *     list  - pointer to head of list of nodes
 *     count - number of nodes in list
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - null list or count is wrong
 */
int acl_range_destroy(acl_node_t *list, int count) {
    acl_node_t          *node_cur  = NULL;
    acl_node_t          *node_prev = NULL;

    if (list == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: list is NULL\n")));
        return BCM_E_PARAM;
    }

    node_cur = list;
    while (count > 0) {
        node_prev = node_cur;
        if (node_prev == NULL) {
            LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                      (BSL_META("ACL Error: count is wrong\n")));
            return BCM_E_PARAM;
        }
        node_cur = node_prev->next;
        sal_free(node_prev);
        count--;
    }

    /* Confirm that list is empty. */
    if (node_cur != NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: count is wrong\n")));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#ifdef BROADCOM_DEBUG
/*
 * Function: bcma_acl_show
 *
 * Purpose:
 *     Display access control lists currently in ACES.
 *
 * Parameters:
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_XXX  - return value from bcma_acl_list_show()
 */
int
bcma_acl_show(void)
{
    int                 retval = BCM_E_NONE;
    _acl_link_t         *list_link;
    
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_show()\n")));
    ACL_IS_INIT();

    for (list_link = _acl_control->head.next;
         list_link != NULL;
         list_link = list_link->next) {
        BCM_IF_ERROR_RETURN(bcma_acl_list_show(list_link->list->list_id));
    }

#ifdef BCM_FIELD_SUPPORT
    {
    int retval_temp;
    retval_temp = _acl_field_show();
    ACL_RETVAL_REPLACE(retval_temp, retval);
    }
#endif /* BCM_FIELD_SUPPORT */

    if (BCM_FAILURE(retval)) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Device specific acl_show() failed\n")));
        return retval;
    }

    return BCM_E_NONE;
}

#define ACL_SHOW_RANGE(rule, name, flag) do {                           \
        if ((rule)->flags & (flag)) {                                   \
            switch (flag) {                                             \
            case BCMA_ACL_RULE_L4_SRC_PORT:                             \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,                    \
                         (BSL_META("%s=%d-%d, "),                       \
                          (name), (rule)->src_port_min,                 \
                          (rule)->src_port_max));                       \
                break;                                                  \
            case BCMA_ACL_RULE_L4_DST_PORT:                             \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,                    \
                         (BSL_META("%s=%d-%d, "),                       \
                          (name), (rule)->dst_port_min,                 \
                          (rule)->dst_port_max));                       \
                break;                                                  \
            case BCMA_ACL_RULE_VLAN:                                    \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,                    \
                         (BSL_META("%s=%d-%d, "),                       \
                          (name), (rule)->vlan_min,                     \
                          (rule)->vlan_max));                           \
                break;                                                  \
            }                                                           \
        }                                                               \
    } while (0)

#define ACL_SHOW_MAC(rule, name, mac_str, flag) do {            \
        if ((rule)->flags & (flag)) {                           \
            if ((flag) == BCMA_ACL_RULE_SRC_MAC) {              \
                format_macaddr((mac_str), (rule)->src_mac);     \
            } else if (flag == BCMA_ACL_RULE_DST_MAC) {         \
                format_macaddr((mac_str), (rule->dst_mac));     \
            }                                                   \
            LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,                \
                     (BSL_META("%s=%s, "),                      \
                      (name), (mac_str)));                      \
        }                                                       \
    } while (0)

#define ACL_SHOW_IP(rule, name, str_p, flag) do {               \
        if ((rule)->flags & flag) {                             \
            if (flag == BCMA_ACL_RULE_SRC_IP4) {                \
                format_ipaddr((str_p), (rule->src_ip));         \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("%s=%s, "),                  \
                          (name), (str_p)));                    \
                format_ipaddr((str_p), (rule->src_ip_mask));    \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("%sMask=%s, "),              \
                          (name), (str_p)));                    \
            }                                                   \
            if (flag == BCMA_ACL_RULE_DST_IP4) {                \
                format_ipaddr((str_p), (rule->dst_ip));         \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("%s=%s, "),                  \
                          (name), (str_p)));                    \
                format_ipaddr((str_p), (rule->dst_ip_mask));    \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("%sMask=%s, "),              \
                          (name), (str_p)));                    \
            }                                                   \
            if (flag == BCMA_ACL_RULE_SRC_IP6) {                \
                format_ip6addr((str_p), (rule->src_ip6));       \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("%s=%s,\n"),                 \
                          (name), (str_p)));                    \
                format_ip6addr((str_p), (rule->src_ip6_mask));  \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("\t\t%sMask=%s, "),          \
                          (name), (str_p)));                    \
            }                                                   \
            if (flag == BCMA_ACL_RULE_DST_IP6) {                \
                format_ip6addr((str_p), (rule->dst_ip6));       \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("%s=%s,\n"),                 \
                          (name), (str_p)));                    \
                format_ip6addr((str_p), (rule->dst_ip6_mask));  \
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,            \
                         (BSL_META("\t\t%sMask=%s, "),          \
                          (name), (str_p)));                    \
            }                                                   \
        }                                                       \
    } while (0)

#define ACL_SHOW_ETHERTYPE(rule, name) do {                             \
        if ((rule)->flags & (BCMA_ACL_RULE_ETHERTYPE)) {                \
            LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,                        \
                     (BSL_META("%s=0x%04x, "),                          \
                      (name), rule->ether_type));                       \
        }                                                               \
    } while (0)

#define ACL_SEPARATOR(first_print) do {                                 \
        if ((first_print) == 1) {                                       \
            (first_print) = 0;                                          \
        } else {                                                        \
            LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,                        \
                     (BSL_META(", ")));                                 \
        }                                                               \
    } while (0)

/*
 * Function: bcma_acl_list_show
 *
 * Purpose:
 *     Display a single access control list.
 *
 * Parameters:
 *     list_id  - List ID
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - List ID not found
 *     BCM_E_XXX       - return value from bcma_acl_rule_show()
 */
int
bcma_acl_list_show(bcma_acl_list_id_t list_id)
{
    int                 count;
    bcmx_lport_t        lport;
    _acl_link_t         *list_link;
    _acl_rule_link_t    *rule_link;
    int                 first_print = TRUE;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_list_show(list_id=%d)\n"),
               list_id));
    ACL_IS_INIT();

    list_link = _acl_find(list_id);
    if (list_link == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: list ID=%d not found\n"),
                   list_id));
        return BCM_E_NOT_FOUND;
    }
    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("acl={id=%d, lplist={"),
              list_link->list->list_id));

    if ((BCMX_LPLIST_IS_EMPTY(&list_link->list->lplist))) {  
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("None")));
    } else {
        count = BCMX_LPLIST_COUNT(&list_link->list->lplist);
        BCMX_LPLIST_ITER(list_link->list->lplist, lport, count) {
            if (first_print == TRUE) {
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                         (BSL_META("%d"),
                          lport));
                first_print = FALSE;
            } else {
                LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                        (BSL_META(", %d"),
                         lport));
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("}")));

    for (rule_link = list_link->rules;
         rule_link != NULL;
         rule_link = rule_link->next) {
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("\n\t")));
        BCM_IF_ERROR_RETURN(bcma_acl_rule_show(rule_link->rule));
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META(",")));
    }
    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("\n}\n")));

    return BCM_E_NONE;
}

char *
_acl_ip_protocol_name(bcma_acl_ip_protocol_t ip_protocol)
{
    return ip_protocol >= BCMA_ACL_IPPROTO_ANY ?
                          "??" : bcma_acl_ip_protocol_text[ip_protocol];
}

/*
 * Function: bcma_acl_rule_show_id
 *
 * Purpose:
 *     Display a single rule.
 *
 * Parameters:
 *     rule_id  - Rule ID
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Rule ID not found
 */
int
bcma_acl_rule_show_id(bcma_acl_rule_id_t rule_id)
{
    _acl_rule_link_t    *rule_link;
    
    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_show_id(rule_id=%d)\n"),
               rule_id));
    ACL_IS_INIT();

    rule_link = _acl_rule_find(rule_id);
    if (rule_link == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: Rule ID=%d not found\n"),
                   rule_id));
        return BCM_E_NOT_FOUND;
    }

    return bcma_acl_rule_show(rule_link->rule);
}

/*
 * Function: bcma_acl_rule_show
 *
 * Purpose:
 *     Display a single rule given a pointer to the rule struct.
 *
 * Parameters:
 *     rule - pointer to rule structure to be displayed
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - rule pointer is NULL
 */
int
bcma_acl_rule_show(bcma_acl_rule_t *rule)
{
    char            mac_str[MACADDR_STR_LEN + 3];
    char            ip4_str[IPADDR_STR_LEN + 3];
    char            ip6_str[IP6ADDR_STR_LEN + 3];

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_show(rule=%p)\n"),
               (void *)rule));
    ACL_IS_INIT();

    if (rule == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: null rule passed to bcma_acl_rule_show()\n")));
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_rule_show(rule_id=%d)\n"),
               rule->rule_id));

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("rule={id=%d, "),
              rule->rule_id));

    LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                (BSL_META("flags=0x%03x, "),
                 rule->flags));

    /* Show L4 source and destination port ranges. */
    ACL_SHOW_RANGE(rule, "SrcPort", BCMA_ACL_RULE_L4_SRC_PORT);
    ACL_SHOW_RANGE(rule, "DstPort", BCMA_ACL_RULE_L4_DST_PORT);

    /* Show Source and Destination MAC addresses. */
    ACL_SHOW_MAC(rule, "SrcMac", mac_str, BCMA_ACL_RULE_SRC_MAC);
    ACL_SHOW_MAC(rule, "DstMac", mac_str, BCMA_ACL_RULE_DST_MAC);

    /* Show Source IPv4 address */
    ACL_SHOW_IP(rule, "SrcIp4", ip4_str, BCMA_ACL_RULE_SRC_IP4);

    /* Show Destination IPv4 address */
    ACL_SHOW_IP(rule, "DstIp4", ip4_str, BCMA_ACL_RULE_DST_IP4);

    /* Show Source and Destination IPv6 addresses & masks. */
    ACL_SHOW_IP(rule, "SrcIp6", ip6_str, BCMA_ACL_RULE_SRC_IP6);
    ACL_SHOW_IP(rule, "DstIp6", ip6_str, BCMA_ACL_RULE_DST_IP6);

    /* Show Vlan range. */
    ACL_SHOW_RANGE(rule, "Vlan", BCMA_ACL_RULE_VLAN);

    if (rule->ip_protocol != BCMA_ACL_IPPROTO_ANY) {
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("IpProtocol=%s (%d), "),
                  _acl_ip_protocol_name(rule->ip_protocol), rule->ip_protocol));
    }
    
    /* Show EtherType. */
    ACL_SHOW_ETHERTYPE(rule, "EtherType");

    bcma_acl_action_show(&rule->actions);
    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("}")));

    return BCM_E_NONE;
}

/*
 * Function: bcma_acl_action_show
 *
 * Purpose:
 *     Display a set of actions for a rule.
 *
 * Parameters:
 *     action - pointer to action structure to be displayed
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - action pointer is NULL
 */
int
bcma_acl_action_show(bcma_acl_action_t *action)
{
    uint8               first_print = 1;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_action_show(action=%p)\n"),
               (void *)action));
    ACL_IS_INIT();

    if (action == NULL) {
        LOG_ERROR(BSL_LS_APPL_ACCESSCTRLLIST,
                  (BSL_META("ACL Error: null action passed to bcma_acl_action_show()\n")));
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("ACL bcma_acl_action_show(action=%p)\n"),
               (void *)action));

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("action={")));
    LOG_VERBOSE(BSL_LS_APPL_ACCESSCTRLLIST,
                (BSL_META("flags=0x%x, "),
                 action->flags));
    if (action->flags & BCMA_ACL_ACTION_PERMIT) {
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("Permit")));
        first_print = 0;
    }
        
    if (action->flags & BCMA_ACL_ACTION_DENY) {
        ACL_SEPARATOR(first_print);
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("Deny")));
    }

    if (action->flags & BCMA_ACL_ACTION_LOG) {
        ACL_SEPARATOR(first_print);
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("Log")));
    }

    if (action->flags & BCMA_ACL_ACTION_REDIR) {
        ACL_SEPARATOR(first_print);
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("Redirect={lport=%d}"),
                  action->redir_port));
    }

    if (action->flags & BCMA_ACL_ACTION_MIRROR) {
        ACL_SEPARATOR(first_print);
        LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
                 (BSL_META("Mirror={lport=%d}"),
                  action->mirror_port));
    }

    LOG_INFO(BSL_LS_APPL_ACCESSCTRLLIST,
             (BSL_META("}")));
    return BCM_E_NONE;
}
#undef ACL_SEPARATOR

#endif /* BROADCOM_DEBUG */
#endif /* INCLUDE_ACL */

