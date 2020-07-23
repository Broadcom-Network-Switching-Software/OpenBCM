/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Internal regex function prototypes.
 */

#ifndef _BCM_INT_BREGEX_H_
#define _BCM_INT_BREGEX_H_
#include <soc/defs.h>

#if defined(INCLUDE_REGEX)
#include <bcm/debug.h>
#include <bcm_int/esw/field.h>
#include <sal/core/sync.h>

/*
 * These features are not implemented yet.  These identifiers should be removed once the
 * feature is complete.
 */
#undef ESW_REGEX_STAT_SUPPORT

/*
 * This feature is not supported and has not been tested.  It will require the Flow
 * Tracker to use a second pool of meters.  Since meter pools are limited (only 8 on TR3
 * and HX4) and also used by the Field Processor, only one meter pool is reserved for use
 * by the Flow Tracker.
 */
#undef ESW_REGEX_HIERARCHICAL_METERS

typedef struct _regex_policy_s _regex_policy_t;

#define _REGEX_INVALID_INDEX            -1

#define _REGEX_DEFAULT_POLICY_ID        0
/*
 * Policy status flags.
 */

/* Software entry differs from one in hardware. */
#define _REGEX_POLICY_DIRTY                      (1 << 0)

/* Entry has an ingress Mirror-To-Port reserved. */
#define _REGEX_POLICY_MTP_ING0                   (1 << 4)

/* Entry has an ingress 1 Mirror-To-Port reserved. */
#define _REGEX_POLICY_MTP_ING1                   (1 << 5)

/* Entry has an egress Mirror-To-Port reserved. */
#define _REGEX_POLICY_MTP_EGR0                   (1 << 6)

/* Entry has an egress 1 Mirror-To-Port reserved. */
#define _REGEX_POLICY_MTP_EGR1                   (1 << 7)

/* Field entry installed in hw. */
#define _REGEX_POLICY_INSTALLED                  (1 << 9)

/* Treat all packets as green. */
#define _REGEX_POLICY_COLOR_INDEPENDENT          (1 << 10)

/*
 * Action flags.
 */
/* Action valid. */
#define _REGEX_ACTION_VALID              (1 << 0)
/* Reinstall is required. */
#define _REGEX_ACTION_DIRTY              (1 << 1)
/* Remove action from hw. */
#define _REGEX_ACTION_RESOURCE_FREE      (1 << 2)
/* Action was updated free previously used hw resources. */
#define _REGEX_ACTION_OLD_RESOURCE_FREE  (1 << 3)

#define _REGEX_ACTION_MODIFY             (1 << 4)

#define _REGEX_ACTION_HW_FREE (_REGEX_ACTION_RESOURCE_FREE | \
                               _REGEX_ACTION_OLD_RESOURCE_FREE)

/* policy table Cos queue priority hw modes */
#define _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE   0x1
#define _REGEX_ACTION_UCAST_QUEUE_NEW_MODE         0x8
#define _REGEX_ACTION_MCAST_QUEUE_NEW_MODE         0x9
/* Action set unicast queue value */
#define _REGEX_ACTION_UCAST_QUEUE_SET(q) ((q) & (0xf))
/* Action set multicast queue value */
#define _REGEX_ACTION_MCAST_QUEUE_SET(q) (((q) & 0x7) << 4)
/* Action get unicast queue value */
#define _REGEX_ACTION_UCAST_QUEUE_GET(q) ((q) & (0xf))
/* Action get multicast queue value */
#define _REGEX_ACTION_MCAST_QUEUE_GET(q) (((q) >> 4) & 0x7)
/* Action set both unicast and multicast queue value */
#define _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u,m) ((_REGEX_ACTION_UCAST_QUEUE_SET(u)) | \
                                                  (_REGEX_ACTION_MCAST_QUEUE_SET(m)))

/*
 * Policer macros
 */
#define _REGEX_POLICER_MAX_NUM         256

#if defined(ESW_REGEX_HIERARCHICAL_METERS)
#define _REGEX_POLICER_LEVEL_COUNT 2
#else
#define _REGEX_POLICER_LEVEL_COUNT 1
#endif

/*
 * Macros for packing and unpacking L3 information for field actions
 * into and out of a uint32 value.
 */

#define MAX_CNT_BITS 10

#define _REGEX_L3_ACTION_PACK_ECMP(ecmp_ptr, ecmp_cnt) \
    (0x80000000 | ((ecmp_ptr) << MAX_CNT_BITS) | (ecmp_cnt))
#define _REGEX_L3_ACTION_UNPACK_ECMP_MODE(cookie) \
    ((cookie) >> 31)
#define _REGEX_L3_ACTION_UNPACK_ECMP_PTR(cookie) \
    (((cookie) & 0x7fffffff) >> MAX_CNT_BITS)
#define _REGEX_L3_ACTION_UNPACK_ECMP_CNT(cookie) \
    ((cookie) & ~(0xffffffff << MAX_CNT_BITS))

#define _REGEX_L3_ACTION_PACK_NEXT_HOP(next_hop) \
    ((next_hop) & 0x7fffffff)
#define _REGEX_L3_ACTION_UNPACK_NEXT_HOP(cookie) \
    ((cookie) & 0x7fffffff)

/* Action conflict check macro. */
#define _REGEX_POLICY_ACTIONS_CONFLICT(_val_)    \
    if (action == _val_) {                       \
        return (BCM_E_CONFIG);                   \
    }

#define RegexPolicyMax(_unit_, _mem_, _field_)                          \
    ((soc_mem_field_length((_unit_), (_mem_) , (_field_)) < 32) ?       \
     ((1 << soc_mem_field_length((_unit_), (_mem_), (_field_))) - 1) :  \
     (0xFFFFFFFFUL))

#define RegexPolicyGet(_unit_, _mem_, _entry_, _field_)             \
    soc_mem_field32_get((_unit_), (_mem_), (_entry_), (_field_))

#define RegexPolicySet(_unit_, _mem_, _entry_, _field_, _value_)        \
    soc_mem_field32_set((_unit_), (_mem_), (_entry_), (_field_), (_value_))

#define RegexPolicyCheck(_unit_, _mem_, _field_, _value_)               \
    if ((uint32)(_value_) > (uint32)RegexPolicyMax((_unit_), (_mem_), (_field_))) { \
        LOG_ERROR(BSL_LS_BCM_REGEX,                                     \
                  (BSL_META_U(unit,                                     \
                              "FT(unit %d) Error: Policy _value_ %d > %d (max) mem (%d) policy (%d).\n"), \
                   _unit_, (_value_),                                   \
                   (uint32)RegexPolicyMax((_unit_), (_mem_), (_field_)), \
                   (_mem_), (_field_)));                                \
        return (BCM_E_PARAM);                                           \
    }

#define _REGEX_POLICER_ID_ENCODE(u,idx)                             \
    ((((u) & 0xff) << 16) | ((idx) & 0xffff) |                      \
     (BCM_POLICER_TYPE_REGEX << BCM_POLICER_TYPE_SHIFT))

#define _REGEX_POLICER_ID_TO_INDEX(pid) ((pid) & 0xffff)

#define _FT_ACTION_PARAM_SZ             (4)

/*
 * Typedef:
 *     _regex_policy_action_t
 * Purpose:
 *     This is the real action storage structure that is hidden behind
 *     the opaque handle bcm_field_action_t.
 */
typedef struct _regex_policy_action_s {
    bcm_field_action_t     action;       /* Action type                  */
    uint32                 param[_FT_ACTION_PARAM_SZ];
                                         /* Action specific parameters   */
    int                    hw_index;     /* Allocated hw resource index. */
    int                    old_index;    /* Hw resource to be freed, in  */
                                         /* case action parameters were  */
                                         /* modified.                    */
    uint8                  flags;        /* Field action flags.          */
    struct _regex_policy_action_s *next;
} _regex_policy_action_t;

/*
 * Typedef:
 *     _regex_policy_t
 * Purpose:
 *     This is the physical entry structure, hidden behind the logical
 *     bcm_regex_policy_t handle.
 * Notes:
 *     Entries are stored in an array in the _bcm_tr3_regex_device_info_t
 *     structure.
 *
 *     Each entry can use 0 or 1 counters. Multiple entries may use the
 *     same counter. The only requirement is that the counter be within
 *     the same slice as the entry.  I.e., cannot share counters with the FP
 *     since the flow table is in its own slice.
 *
 *     Similarly each entry can use 0 or 1 meter. Multiple entries may use
 *     the same meter. The only requirement is that the meter be within
 *     the same slice as the entry.
 */
struct _regex_policy_s {
    bcm_regex_policy_t      eid;        /* BCM unit unique entry identifier   */
    uint32                  flags;      /* _REGEX_POLICY_xxx flags            */
    _regex_policy_action_t *actions;    /* linked list of actions for entry   */
    _field_entry_stat_t     statistic;  /* Statistics collection entity.      */
                                        /* Policers attached to the entry.    */
    _field_entry_policer_t  policer[_REGEX_POLICER_LEVEL_COUNT];
};

typedef struct _bcm_esw_regex_functions_s {
    int (*policy_index_min_get)(int unit);
    int (*policy_index_max_get)(int unit);
    /* These next "check" functions should return the same answer as the similar FP
     * functions for the ingress stage. */
    int (*policy_action_support_check)(int unit,
                                       bcm_field_action_t  action);
    int (*policy_action_conflict_check)(int unit,
                                        bcm_field_action_t action,
                                        bcm_field_action_t action1);
    int (*policy_action_params_check)(int unit, _regex_policy_t *p_ent,
                                      _regex_policy_action_t *pa);
    int (*policy_install)(int unit, _regex_policy_t *p_ent);
    int (*policy_remove)(int unit, _regex_policy_t *p_ent);
    int (*policer_install)(int unit, _field_policer_t *f_pl);
    int (*redirect_profile_alloc)(int unit, _regex_policy_t *p_ent,
                                  _regex_policy_action_t *pa);
    int (*redirect_profile_delete)(int unit, int index);
    int (*redirect_profile_ref_count_get)(int unit, int index, int *ref_count);

} _bcm_esw_regex_functions_t;

typedef struct _bcm_esw_regex_device_info_s {

    sal_mutex_t                  regex_esw_mutex;
    _regex_policy_t            **policies;
    int                          num_policies;
    int                          policy_index_min;
    int                          policy_index_max;
    _field_policer_t            *policers[_REGEX_POLICER_MAX_NUM];
    int                          meter_base_hw_index;
    int                          meter_pool;
    _bcm_esw_regex_functions_t  *functions;
    uint32                       flags;
    uint32                       report_buffer_size;      /* The number of entries which
                                                           * the report buffer can
                                                           * hold. */
} _bcm_esw_regex_device_info_t;

/*
 * These are various defines used for accessing the FT_SESSION and
 * FT_SESSION_IPV6 tables.
 */
#define _REGEX_SESSION_DIRECTION_FORWARD 0x0        /* Value indicates
                                                       FT_SESSION entries
                                                       stored in the forward
                                                       direction. */
#define _REGEX_SESSION_DIRECTION_REVERSE 0x1        /* Value indicates
                                                       FT_SESSION entries
                                                       stored in the forward
                                                       direction. */
#define _REGEX_SESSION_AGE_PROFILE_TICKS_US 10000      /* Value indicates number
                                                       of us in an
                                                       AGE_PROFILE tick. */
#define _REGEX_SESSION_KEY_TYPE_RESERVED 0x0        /* Value indicates
                                                       FT_SESSION reserved
                                                       KEY_TYPE. */
#define _REGEX_SESSION_KEY_TYPE_IPV4     0x1        /* Value indicates
                                                       FT_SESSION IPV4
                                                       KEY_TYPE. */
#define _REGEX_SESSION_KEY_TYPE_IPV6_UPPER 0x2        /* Value indicates
                                                       FT_SESSION IPV6_UPPER
                                                       KEY_TYPE. */
#define _REGEX_SESSION_KEY_TYPE_IPV6_LOWER 0x3        /* Value indicates
                                                       FT_SESSION IPV6_LOWER
                                                       KEY_TYPE. */

/*
Defines for the flow states
*/

#define _REGEX_SESSION_FLOW_STATE_VALIDATE  0x0
#define _REGEX_SESSION_FLOW_STATE_NEW       0x1
#define _REGEX_SESSION_FLOW_STATE_CMDWAIT   0x2
#define _REGEX_SESSION_FLOW_STATE_ACTIVE    0x3
#define _REGEX_SESSION_FLOW_STATE_HOLDWAIT  0x4
#define _REGEX_SESSION_FLOW_STATE_RETIRED   0x5
#define _REGEX_SESSION_FLOW_STATE_INVALID   0x6
#define _REGEX_SESSION_FLOW_STATE_RESERVED  0x7 /* Should not be used. */

/*
Offsets and lengths for various sub-fields in the FT_SESSION key fields
*/

#define _REGEX_KEY_FIELD_LENGTH                104

#define _REGEX_KEY_FIELD_V4_OFFSET_PORT_1      0
#define _REGEX_KEY_FIELD_V4_OFFSET_PORT_2      16
#define _REGEX_KEY_FIELD_V4_OFFSET_PROTOCOL    32
#define _REGEX_KEY_FIELD_V4_OFFSET_IP_1        40
#define _REGEX_KEY_FIELD_V4_OFFSET_IP_2        72

#define _REGEX_KEY_FIELD_V4_LENGTH_PORT_1      16
#define _REGEX_KEY_FIELD_V4_LENGTH_PORT_2      16
#define _REGEX_KEY_FIELD_V4_LENGTH_PROTOCOL    8
#define _REGEX_KEY_FIELD_V4_LENGTH_IP_1        32
#define _REGEX_KEY_FIELD_V4_LENGTH_IP_2        32

#define _REGEX_KEY_UPPER_LENGTH                104
#define _REGEX_KEY_LOWER_LENGTH                192

#define _REGEX_KEY_LOWER_V6_OFFSET_PORT_1      0
#define _REGEX_KEY_LOWER_V6_OFFSET_PORT_2      16
#define _REGEX_KEY_LOWER_V6_OFFSET_PROTOCOL    32
#define _REGEX_KEY_LOWER_V6_OFFSET_IP_1        40
#define _REGEX_KEY_LOWER_V6_OFFSET_IP_2        168
#define _REGEX_KEY_UPPER_V6_OFFSET_IP_2        0

#define _REGEX_KEY_LOWER_V6_LENGTH_PORT_1      16
#define _REGEX_KEY_LOWER_V6_LENGTH_PORT_2      16
#define _REGEX_KEY_LOWER_V6_LENGTH_PROTOCOL    8
#define _REGEX_KEY_LOWER_V6_LENGTH_IP_1        128
#define _REGEX_KEY_LOWER_V6_LENGTH_IP_2        24
#define _REGEX_KEY_UPPER_V6_LENGTH_IP_2        104

#define _REGEX_PROTOCOL_PORT_LENGTH_V4  (_REGEX_KEY_FIELD_V4_LENGTH_PORT_1 + \
        _REGEX_KEY_FIELD_V4_LENGTH_PORT_2 + _REGEX_KEY_FIELD_V4_LENGTH_PROTOCOL)

#define _REGEX_PROTOCOL_PORT_LENGTH_V6  (_REGEX_KEY_LOWER_V6_LENGTH_PORT_1 + \
        _REGEX_KEY_LOWER_V6_LENGTH_PORT_2 + _REGEX_KEY_LOWER_V6_LENGTH_PROTOCOL)

/*
If set, the routines that get the FT Session data will also get
the key data.  This is not normally necessary in a "get" call,
since the key data must already be known to get the entry.
However, in a traverse call, the key data is not already known,
and is different in each entry.
 
To use externally, this should be defined in bcm.papi.bregex 
rather than in this file. 
*/
#define _REGEX_GET_KEY_FROM_HW_ENTRY           0x1
#define _REGEX_ENTRY_TYPE_IS_IPV6              0x2

int _bcm_regex_policy_action_dest_check(int unit, _regex_policy_action_t *pa);

void _bcm_esw_regex_policy_action_t_init(_regex_policy_action_t *pa);
void _bcm_esw_regex_policy_t_init(_regex_policy_t *policy);
void _bcm_esw_regex_device_info_t_init(_bcm_esw_regex_device_info_t *info);

int _bcm_regex_policy_set_l3_nh_resolve(int unit, int value,
                                        uint32 *flags, int *nh_ecmp_id);

int _bcm_regex_policer_get(int unit, bcm_policer_t pid,
                           _field_policer_t **policer_p);
int _bcm_esw_regex_policer_create(int unit, bcm_policer_config_t *pol_cfg,
                                  bcm_policer_t *pid);
int _bcm_esw_regex_policer_destroy(int unit, bcm_policer_t policer_id);
int _bcm_esw_regex_policer_destroy_all(int unit);
int _bcm_esw_regex_policer_traverse(int unit, bcm_policer_traverse_cb cb,
                                    void *user_data);

int _bcm_esw_regex_report_reset(int unit);

#endif  /* defined(INCLUDE_REGEX) */

#endif /* _BCM_INT_BREGEX_H_ */
