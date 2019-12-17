/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        acl_util.h
 *     
 * Purpose:     
 *     Access Control List (ACL) utility functions. These
 *     are intended primarily to provide additional support for the CLI code.
 *     and API prototypes.
 */

#ifndef   _ACL_UTIL_H_
#define   _ACL_UTIL_H_

#if defined(INCLUDE_ACL)

#include <sal/core/sync.h>
#include <appl/acl/acl.h>

#define ACL_MASK_ETHERTYPE      (0xffff)
#define ACL_MASK_IPPROTOCOL     (0xff)

/*
 * Typedef: _acl_rule_link_t
 *
 * Purpose:
 *     Linked-list wrapper for public bcma_acl_rule_t type.
 *
 * Fields:
 *     rule  - Public rule parameters
 *     *next - Link to next node rule
 */
typedef struct _acl_rule_link_s {
    bcma_acl_rule_t          *rule;     /* Public rule parameters     */
    uint32                   entries;   /* Number of physical entries */
    struct _acl_rule_link_s  *next;     /* Link to next node rule     */
} _acl_rule_link_t;

/*
 * Typedef: _acl_link_t
 *
 * Purpose:
 *     Linked-list wrapper for public bcma_acl_t type.
 *
 * Fields:
 *     - <bcma_acl_t> list Public data for this list node
 *     - <_acl_rule_link_t> *rules Linked list of rules
 *     - <_acl_rule_link_t> *cur Current rule, used for iterating through list
 *     - <_acl_link_t> *next    Link to next list node
 */
typedef struct _acl_link_s {
    bcma_acl_t               *list;     /* This list node public data */
    _acl_rule_link_t         *rules;    /* Linked list of rules       */
    _acl_rule_link_t         *cur;      /* Current rule               */
    struct _acl_link_s       *prev;     /* Link to previous list node */
    struct _acl_link_s       *next;     /* Link to next list node     */
} _acl_link_t;

/*
 * Typedef: _acl_control_t
 *
 * Purpose:
 *    System-wide ACL data.
 *
 * Fields:
 *    - <_acl_link_t> *lists     Linked-list of ACLs
 *    - <_acl_link_t> *cur       Current ACL; used for iterating over lists
 *    - sal_mutex_t acl_lock     System lock
 */
typedef struct _acl_control_s {
    _acl_link_t         head;           /* Linked-list of ACLs              */
    _acl_link_t         *cur;           /* Current ACL                      */
    sal_mutex_t         acl_lock;       /* System lock                      */
}_acl_control_t;

/*
 * Typedef: acl_node_t
 *
 * Purpose:
 *     Single element in a list of nodes that make up a range. For instance, if
 *     someone qualifies on a range of VLAN IDs, this will get implemented as a
 *     list of data/mask pairs that cover the requested range.
 *
 * Fields:
 *     data -
 *     mask -
 */
typedef struct acl_node_s {
    uint16              data;
    uint16              mask;
    struct acl_node_s   *next;
} acl_node_t;

#define _BCMA_ACL_MAC_ALL_ONES {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

/* Prototypes  */
extern _acl_link_t      *_acl_first(_acl_control_t *control);
extern _acl_link_t      *_acl_next(_acl_control_t *control);
extern bcma_acl_rule_t  *_acl_rule_first(_acl_link_t *list_link);
extern bcma_acl_rule_t  *_acl_rule_next(_acl_link_t *list_link);
extern _acl_rule_link_t *_acl_rule_link_find(bcma_acl_rule_id_t rid);

extern int acl_range_to_list(uint16 min, uint16 max, acl_node_t **list,
                             int *count);
extern int acl_range_destroy(acl_node_t *list, int count);

#endif /* INCLUDE_ACL */

#endif /* _ACL_UTIL_H_ */
