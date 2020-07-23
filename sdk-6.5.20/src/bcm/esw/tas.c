/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tas.c
 * Purpose:     TAS driver
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/tas.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#if defined(INCLUDE_PTP)
#include <bcm_int/common/ptp.h>
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
#include <bcm_int/esw/greyhound2.h>
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_TAS_SUPPORT
/*
 * Internal TAS profile control structure.
 * To keep the software database from bcm api calls and corresponding
 * remapped hardware data which should be programed to HW.
 */
typedef struct tas_profile_ctrl_s {
    sal_mutex_t                     profile_mutex;
    bcm_cosq_tas_profile_id_t       pid;
    bcm_cosq_tas_profile_state_t    pstate;
    bcm_port_t                      port; /* logical port */
    int                             hw_index; /* hw profile id */
    int                             num_user_entries; /* entries form API */
    bcm_cosq_tas_entry_t            *user_entries;
    int                             num_hw_entries; /* remapped HW entries */
    bcm_cosq_tas_entry_t            *hw_entries;
    bcm_ptp_timestamp_t             base_time;
    bcm_ptp_timestamp_t             config_change_time;
    uint32                          cycle_time;
    uint32                          cycle_extension;

    struct tas_profile_ctrl_s       *next;   /* For storing in a linked-list  */
} tas_profile_ctrl_t;

/* Mutex macro to protect tas_profile */
#define TAS_PROFILE_LOCK(tp) \
        do {\
            if ((tp)->profile_mutex) { \
                sal_mutex_take((tp)->profile_mutex, sal_mutex_FOREVER); \
            }\
        } while (0)
#define TAS_PROFILE_UNLOCK(tp) \
        do {\
            if ((tp)->profile_mutex) { \
                sal_mutex_give((tp)->profile_mutex); \
            }\
        } while (0)

/* Define the maximum profile_cnt per profile list */
#define MAX_TAS_PROFILE_CNT     (10)

/* The soc properties which will be referenced for TAS profile entry remap */
typedef struct tas_prop_info_s {
    uint8 txoverrun; /* tas_calendar_auto_adjust_for_txoverrun */
    uint8 holdadvance; /* tas_calendar_auto_adjust_for_holdadvance */
    int maxsdu[BCM_COS_COUNT]; /* tas_calendar_auto_adjust_ref_maxsdu */
} tas_prop_info_t;

/*
 * Internal TAS control structure.
 */
typedef struct bcmi_esw_tas_control_s {
    sal_mutex_t                 tas_mutex;
    int                         profile_list_count; /* Max profile list count. */
    tas_profile_ctrl_t          **profile_list; /* Profiles list.
                                                 * Two-dimension list, the first
                                                 * point to port, the second
                                                 * point to profile associated
                                                 * with the port.
                                                 */
    bcm_cosq_tas_profile_id_t   *active_pid;    /* Active profile id */
    bcm_cosq_tas_profile_id_t   *process_pid;   /* Process(commit or pending)
                                                 * profile id
                                                 */
    tas_prop_info_t             *tas_properties; /* Port basis software cache
                                                  * of soc properties.
                                                  */
    /* device specific drivers */
    const tas_drv_t             *tas_drvs;
} bcmi_esw_tas_control_t;

/* TAS control to keep unit-base tas information */
STATIC bcmi_esw_tas_control_t *tas_control[SOC_MAX_NUM_DEVICES] = {NULL};

/* Check if tas_control has been initialized. */
#define TAS_IS_INIT(unit)                                      \
    do {                                                            \
        if (tas_control[unit] == NULL) {                        \
            LOG_ERROR(BSL_LS_BCM_TAS,                               \
                      (BSL_META_U(unit,                             \
                                  "TAS Error: Module not initialized\n"))); \
            return (BCM_E_INIT);                                    \
        }                                                           \
    } while (0)

/* Mutex macro to protect tas_control[] */
#define TAS_LOCK(tc) \
        sal_mutex_take((tc)->tas_mutex, sal_mutex_FOREVER)
#define TAS_UNLOCK(tc) \
        sal_mutex_give((tc)->tas_mutex)

/*
 * Helper macro to insert the new profile (_inserted_ptr_) to
 * tas_control[]->profile_list[_index_]
 */
#define TAS_PROFILE_INSERT(_profile_ptr_, _inserted_ptr_, _index_)    \
    do {                                                     \
        (_inserted_ptr_)->next = (_profile_ptr_)[(_index_)];  \
        (_profile_ptr_)[(_index_)] = (_inserted_ptr_);        \
    } while (0)

/*
 * Helper macro to remove the profile (_removed_ptr_) from
 * tas_control[]->profile_list[_index_]
 */
#define TAS_PROFILE_REMOVE(_profile_ptr_, _entry_type_, _removed_ptr_, _index_)\
   do {                                                    \
       _entry_type_ *_prev_ent_ = NULL;                    \
       _entry_type_ *_lkup_ent_ = (_profile_ptr_)[(_index_)]; \
       while (NULL != _lkup_ent_) {                        \
           if (_lkup_ent_ != (_removed_ptr_))  {           \
               _prev_ent_ = _lkup_ent_;                    \
               _lkup_ent_ = _lkup_ent_->next;              \
               continue;                                   \
           }                                               \
           if (_prev_ent_ != NULL) {                        \
               _prev_ent_->next = (_removed_ptr_)->next;   \
           } else {                                        \
               (_profile_ptr_)[(_index_)] = (_removed_ptr_)->next; \
           }                                               \
           break;                                          \
       }                                                   \
   } while (0)

/*
 * Helper macro to remove all the profiles from
 * tas_control[]->profile_list[_index_]
 */
#define TAS_PROFILE_REMOVE_ALL(_profile_ptr_, _entry_type_, _index_)       \
    do {                                                    \
        _entry_type_ *_node_ = NULL;                        \
        _entry_type_ *_head_ = (_profile_ptr_)[(_index_)];  \
        while (NULL != _head_) {                            \
            _node_ = _head_;                                \
            _head_ = _head_->next;                          \
            sal_free(_node_);                               \
            _node_ = NULL;                                  \
        }                                                   \
   } while (0)

/*
 * Helper macro to iterate the profile entry from
 * tas_control[]->profile_list[_index_]
 */
#define TAS_PROFILE_ITER(_profile_ptr_, _entry_ptr_, _index_) \
    for ((_entry_ptr_) = (_profile_ptr_)[(_index_)]; (_entry_ptr_) != NULL; \
         (_entry_ptr_) = (_entry_ptr_)->next)

/* String corresponding to the enum in bcm_cosq_tas_profile_state_t */
static const char *pstate_string[] =
{
    "Init",
    "Committed",
    "Pending",
    "Active",
    "Expired",
    "Error"
};

/* Composition of tas profile id */
/*
 *  port : max 8 bits (support 256 ports)
 *  idx : max  16 bits (0~65535)
 */
#define TAS_PID_PORT_START_BIT  24
#define TAS_PID_PORT_END_BIT    31
#define TAS_PID_IDX_START_BIT 0
#define TAS_PID_IDX_END_BIT   15

#define TAS_PID_PORT_MASK \
    ((1 << (TAS_PID_PORT_END_BIT - TAS_PID_PORT_START_BIT + 1)) - 1)
#define TAS_PID_IDX_MASK \
    ((1 << (TAS_PID_IDX_END_BIT - TAS_PID_IDX_START_BIT + 1)) - 1)

/*
 * Function:
 *     tas_control_get
 * Purpose:
 *     Get the tas_control[unit] pointer
 * Parameters:
 *     unit             - (IN) unit number
 *     tc               - (OUT) pointer to tas_control[unit]
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_control_get(int unit, bcmi_esw_tas_control_t **tc)
{
    /* Input parameters check. */
    if (NULL == tc) {
        return (BCM_E_PARAM);
    }
    /* Make sure system was initialized. */
    TAS_IS_INIT(unit);

    /* Fill tas control structure. */
    *tc = tas_control[unit];

    return (BCM_E_NONE);
}

/*
 * Function:
 *    tas_profile_ctrl_get
 * Purpose:
 *    Get the internal profile information by specifying profile id
 * Parameters:
 *     unit             - (IN) unit number
 *     pid              - (IN) unit number
 *     profile_p        - (IN) pointer to internal profile
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_ctrl_get(
    int unit,
    bcm_cosq_tas_profile_id_t pid,
    tas_profile_ctrl_t **profile_p)
{
    bcmi_esw_tas_control_t *tc = NULL;
    tas_profile_ctrl_t *tp = NULL;
    bcm_port_t  lport;

    if (NULL == profile_p) {
        return (BCM_E_PARAM);
    }
    /* Get unit TAS control structure. */
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    lport = (pid >> TAS_PID_PORT_START_BIT) & TAS_PID_PORT_MASK;
    TAS_LOCK(tc);
    /* Iterate over the profiles linked-list looking for a matching id */
    tp = tc->profile_list[lport];
    while (tp != NULL) {
        if (tp->pid == pid) {
            *profile_p = tp;
            TAS_UNLOCK(tc);
            return (BCM_E_NONE);
        }
        tp = tp->next;
    }
    TAS_UNLOCK(tc);
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     tas_gport_validate
 * Purpose:
 *     gport parameter check and get the corresponding lport
 * Parameters:
 *     unit             - (IN) unit number
 *     gport            - (IN) gport
 *     local_port       - (OUT) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_gport_validate(int unit, bcm_gport_t gport, bcm_port_t *local_port)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, local_port));
    if (!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), *local_port)) {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
}

/* Software cache to keep the soc propery for TAS profile entry remap */
STATIC int
bcmi_esw_tas_property_cache_init(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    int rv = BCM_E_NONE;
    int p, c;
#ifndef NDEBUG
    int len;
#endif /* !NDEBUG */
    char *str, *p_str;
    tas_prop_info_t  *tas_prop;
    char prop[SOC_PROPERTY_NAME_MAX];
    int default_sdu = 0;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    TAS_LOCK(tc);
    /* Use the max index to keep w/o suffix value */
    tas_prop = &tc->tas_properties[tc->profile_list_count];
    tas_prop->txoverrun =
        soc_property_get(unit, spn_TAS_CALENDAR_AUTO_ADJUST_FOR_TXOVERRUN, 0);
    tas_prop->holdadvance =
        soc_property_get(unit, spn_TAS_CALENDAR_AUTO_ADJUST_FOR_HOLDADVANCE, 0);
    /* Get the default maxsdu size */
    default_sdu =
        soc_property_get(unit, spn_TAS_CALENDAR_AUTO_ADJUST_REF_MAXSDU, 1522);
    for (p = 0; p < tc->profile_list_count; p++) {
        tas_prop = &tc->tas_properties[p];
        /* tas_calendar_auto_adjust_for_txoverrun<_port#> */
        tas_prop->txoverrun =
            (uint8)soc_property_port_get(
                unit, (p + 1), spn_TAS_CALENDAR_AUTO_ADJUST_FOR_TXOVERRUN, 0);
        /* tas_calendar_auto_adjust_for_holdadvance<_port#> */
        tas_prop->holdadvance =
            (uint8)soc_property_port_get(
                unit, (p + 1), spn_TAS_CALENDAR_AUTO_ADJUST_FOR_HOLDADVANCE, 0);
        /* tas_calendar_auto_adjust_ref_maxsdu<_port#_cos#> */
        /* try "name_port%d_cos%d" for explicit match first */
        for (c = 0; c < BCM_COS_COUNT; c++) {
#ifndef NDEBUG
            len =
#endif /* !NDEBUG */
            sal_snprintf(prop, SOC_PROPERTY_NAME_MAX, "%s_port%d_cos%d",
                         spn_TAS_CALENDAR_AUTO_ADJUST_REF_MAXSDU, p, c);
            assert(len < SOC_PROPERTY_NAME_MAX);
            str = soc_property_get_str(unit, prop);
            if (str != NULL) {
                tas_prop->maxsdu[c] = _shr_ctoi(str);
            } else {
                /* try "name_port%d" for explicit match */
                sal_snprintf(prop, SOC_PROPERTY_NAME_MAX, "%s_port%d",
                             spn_TAS_CALENDAR_AUTO_ADJUST_REF_MAXSDU, p);
                p_str = soc_property_get_str(unit, prop);
                if (p_str != NULL) {
                    tas_prop->maxsdu[c] = _shr_ctoi(p_str);
                } else {
                    /* giving default value */
                    tas_prop->maxsdu[c] = default_sdu;
                }
            }
        }
    }
    TAS_UNLOCK(tc);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tas_property_get
 * Purpose:
 *     Get the soc property value by specifying soc property
 *     with optional suffix (port and cos)
 *     If the property is system wise setting or without optional suffix
 *     specified, the value will be returned by ignoring the lport and cos
 *     parameters.
 * Parameters:
 *     unit             - (IN) unit number
 *     tas_prop         - (IN) soc property type (TAS_PROP_XXX)
 *     port             - (IN) lport
 *     cos              - (IN) cosq
 *     value            - (OUT) value
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_property_get(
    int unit,
    int tas_prop,
    int port,
    int cos,
    int *value)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    int p = port;
    int c = cos;
    tas_prop_info_t  *prop_info;

    if (value == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    if (port == -1) {
        p = 0;
    }
    if ((p < 0) || (p >= tc->profile_list_count)) {
        return BCM_E_PARAM;
    }
    if (cos == -1) {
        c = 0;
    }
    if ((c < 0) || (c >= BCM_COS_COUNT)) {
        return BCM_E_PARAM;
    }
    prop_info = &tc->tas_properties[p];
    switch (tas_prop) {
    case TAS_PROP_TXOVERRUN:
        /* won't reference cos */
        *value = (int)prop_info->txoverrun;
        break;
    case TAS_PROP_HOLDADVANCE:
        /* won't reference cos */
        *value = (int)prop_info->holdadvance;
        break;
    case TAS_PROP_MAXSDU:
        /* Valid when txoverrun is considered */
        if (prop_info->txoverrun) {
            if ((port == -1) || (cos == -1)) {
                /* if port or cos is not specified,
                 * always return default value.
                 */
                *value = 1522;
            } else {
                *value = prop_info->maxsdu[c];
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tas_control_set
 * Purpose:
 *     Set various configurations for TAS.
 *     Internal function to dispatch to device specific function.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas control type defined in bcm_cos_tas_control_t
 *     arg              - (IN) tas control value
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Most of the TAS configurations are on port basis. For some configuration
 *     which is on system basis, the gport should assign to -1.
 */
int
bcmi_esw_tas_control_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_control_t type,
    int arg)
{
    bcm_port_t lport = -1;
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;

    if (type == bcmCosqTASControlTASPTPLock) {
        /* bcmCosqTASControlTASPTPLock only take gport=-1 */
        if (port != BCM_GPORT_INVALID) {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &lport));
    }

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_control_set != NULL) {
        return td->tas_control_set(unit, lport, type, arg);
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmi_esw_tas_control_get
 * Purpose:
 *     Get various configurations for TAS.
 *     Internal function to dispatch to device specific function.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas control type defined in bcm_cos_tas_control_t
 *     arg              - (OUT) tas control value
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Most of the TAS configurations are on port basis. For some configuration
 *     which is on system basis, the gport should assign to -1.
 */
int
bcmi_esw_tas_control_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_control_t type,
    int *arg)
{
    bcm_port_t lport = -1;
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;

    if (type == bcmCosqTASControlTASPTPLock) {
        /* bcmCosqTASControlTASPTPLock only take gport=-1 */
        if (port != BCM_GPORT_INVALID) {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &lport));
    }
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_control_get != NULL) {
        return td->tas_control_get(unit, lport, type, arg);
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tas_status_get
 * Purpose:
 *     Get TAS status by specifying the type.
 *     Internal function to dispatch to device specific function.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas status type defined in bcm_cos_tas_status_t
 *     arg              - (OUT) tas status value
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_status_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_status_t type,
    int *arg)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    bcm_port_t lport;
    const tas_drv_t *td;

    BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &lport));
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_status_get != NULL) {
        return td->tas_status_get(unit, lport, type, arg);
    }
    return BCM_E_UNAVAIL;
}

/* Time helper function */
#define TAS_PTP_STACK_ID    (0)
#define TAS_PTP_CLOCK_NUM   (0)
/*
 * Function:
 *     bcmi_tas_current_time
 * Purpose:
 *     Get the current PTP time
 * Parameters:
 *     unit             - (IN) unit number
 *     current_time     - (OUT) current time
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_tas_current_time(int unit, bcm_ptp_timestamp_t *current_time)
{
    int     rv = BCM_E_UNAVAIL;
    char    s[32];

    if (current_time == NULL) {
        return BCM_E_PARAM;
    }
#if defined(INCLUDE_PTP)
    if (soc_feature(unit, soc_feature_ptp) ||
        soc_feature(unit, soc_feature_use_local_1588)) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            bcm_time_tod_t tod;
            uint32 stages;

            stages = BCM_TIME_STAGE_INGRESS_FLOWTRACKER;
            tod.time_format = bcmTimeFormatPTP;
            rv = bcm_esw_time_tod_get(unit, stages, &tod);

            current_time->seconds = tod.ts.seconds;
            current_time->nanoseconds = tod.ts.nanoseconds;
        } else
#endif
        {
            rv = bcm_esw_ptp_clock_time_get(unit, TAS_PTP_STACK_ID,
                                            TAS_PTP_CLOCK_NUM, current_time);
        }
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
    if (bsl_check(bslLayerBcm, bslSourceTas, bslSeverityDebug, unit)) {
        _shr_format_uint64_hexa_string(current_time->seconds, s);
        LOG_DEBUG(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit, "Time stamp: sec: %s ns %09u\n"),
                   s, (unsigned)current_time->nanoseconds));
    }
    return rv;
}

/*
 * Function:
 *     bcmi_tas_ptp_to_ns
 * Purpose:
 *     Helper function to covert ptp to nanoseconds.
 * Parameters:
 *     ptp_time         - (IN) ptp time in bcm_ptp_timestamp_t format
 * Returns:
 *     time in nanoseconds
 */
uint64
bcmi_tas_ptp_to_ns(bcm_ptp_timestamp_t ptp_time)
{
    uint64 time;
    uint32 one_sec = TAS_PTP_TIME_NANOSEC_MAX;

    COMPILER_64_COPY(time, ptp_time.seconds);
    COMPILER_64_UMUL_32(time, one_sec);
    COMPILER_64_ADD_32(time, ptp_time.nanoseconds);

    return time;
}

/*
 * Function:
 *     bcmi_tas_ns_to_ptp
 * Purpose:
 *     Helper function to covert nanoseconds to PTP
 * Parameters:
 *     ns               - (IN) time in nanoseconds
 *     ptp_time         - (OUT) ptp time in bcm_ptp_timestamp_t format
 */
void
bcmi_tas_ns_to_ptp(uint64 ns, bcm_ptp_timestamp_t *ptp_time)
{
    uint64 one_sec, sec;
    uint32 nsec;

    if (ptp_time == NULL) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META("ptp_time ptr is NULL !\n")));
        return;
    }
    /* temp parameter to do 64bit operation */
    COMPILER_64_SET(one_sec, 0, TAS_PTP_TIME_NANOSEC_MAX);
    COMPILER_64_COPY(sec, ns);

    /* sec = ns/TAS_PTP_TIME_NANOSEC_MAX */
    COMPILER_64_UDIV_64(sec, one_sec);
    ptp_time->seconds = sec;

    /* nsec = ns % TAS_PTP_TIME_NANOSEC_MAX; */
    /* mod(x,y) = x - y*int(x/y) */
    COMPILER_64_UMUL_32(sec, TAS_PTP_TIME_NANOSEC_MAX);
    COMPILER_64_SUB_64(ns, sec);
    COMPILER_64_TO_32_LO(nsec, ns);

    ptp_time->nanoseconds = nsec;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_delay_param_get
 * Purpose:
 *     Helper function to get the device-specific delay paramaters which are
 *     used to do the profile entry remap.
 *
 * Parameters:
 *     unit              - (IN) unit number
 *     tas_delay_param   - (IN) TAS_DPARAM_xxx to specify which delay param
 *     port              - (IN) logical port
 *     qmap              - (IN) queue bitmap, optional and valid for type
 *                         TAS_DPARAM_TGCR_DYNAMIC.
 *     value             - (OUT) value in ns
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_delay_param_get(
    int unit,
    int tas_delay_param,
    bcm_port_t port,
    uint32 qmap,
    int *value)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_profile_delay_param_get != NULL) {
        return td->tas_profile_delay_param_get(unit, tas_delay_param, port,
                                               qmap, value);
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmi_tas_profile_entries_remap
 * Purpose:
 *     Entry remap function.
 *     To mitigate the Tx overrun on gate closure and Hold-Advance when
 *     preemption is enabled, extra entries might be added.
 *     And the remap algorithm is general but the delay parmeters
 *     are device dependent.
 *
 * Parameters:
 *     unit              - (IN) unit number
 *     lport             - (IN) logical port
 *     num_entries       - (IN) numbers of entries in tas entries
 *     entries           - (IN) tas entries
 *     num_new_entries   - (OUT) numbers of remapped entries
 *     new_entries       - (OUT) remapped tas entries
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_profile_entries_remap(
    int unit,
    bcm_port_t lport,
    int num_entries,
    bcm_cosq_tas_entry_t *entries,
    int *num_new_entries,
    bcm_cosq_tas_entry_t **new_entries)
{
    bcm_cosq_tas_entry_t *hw_entries = NULL, *tas_entry, *n_tas_entry;
    bcm_gport_t gport;
    uint8 preemption_enable = 0, last_valid_entry;
    int rv = BCM_E_UNAVAIL;
    int prop_tgcr, prop_tha, tgcr, tgcr_txpkt, tha, tgcr_ticks, tha_ticks;
    int tgcr_fix;
    int i, num_hw_entries, ticks, extra_enties, hw_index;
    uint32 status, preempt_bmp = 0, o_phold = 0, phold = 0;
    uint32 o_qstate, qstate, xor_qstate, qdiff_tgcr, qdiff_tha;

    if (num_new_entries == NULL) {
        return BCM_E_PARAM;
    }
    if (new_entries == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, lport, &gport));

    /* Get the soc property */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_property_get(unit, TAS_PROP_TXOVERRUN, lport,
                                  -1, &prop_tgcr));
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_property_get(unit, TAS_PROP_HOLDADVANCE, lport,
                                  -1, &prop_tha));
    if ((prop_tgcr == 0) && (prop_tha == 0)) {
        /* We don't need to do entry remap, simply copy and return */
        /* Create remap entries */
        num_hw_entries = num_entries;
        TAS_ALLOC(unit, hw_entries, bcm_cosq_tas_entry_t,
                  sizeof(bcm_cosq_tas_entry_t) * num_hw_entries,
                  "tas profile hw entries", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        sal_memcpy(hw_entries, entries,
                   sizeof(bcm_cosq_tas_entry_t) * num_hw_entries);
        *new_entries = hw_entries;
        *num_new_entries = num_hw_entries;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_cosq_tas_control_get(
            unit, gport, bcmCosqTASControlGateControlTickInterval,
            &ticks));

    /* Check if Preemption is active */
    if (soc_feature(unit, soc_feature_preemption)) {
        status = bcmPortPreemptStatusTxInactive;
        rv = bcm_esw_port_preemption_status_get(unit, gport,
                                                bcmPortPreemptStatusTx,
                                                &status);
        if (BCM_SUCCESS(rv) && (status == bcmPortPreemptStatusTxActive)) {
            preemption_enable = 1;
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_preemption_control_get(
                    unit, gport, bcmPortPreemptControlQueueBitmap,
                    &preempt_bmp));
        }
    }

    /* Calculate the number of the extra entries */
    /* initialize the old entry state o_qstate = the entry[0] state */
    o_qstate = entries[0].state;
    extra_enties = 0;
    last_valid_entry = 0;
    for (i = 1; i < num_entries; i++) {
        tas_entry = &entries[i];
        if ((tas_entry->ticks == BCM_COSQ_TAS_ENTRY_TICKS_GO_FIRST) ||
            (last_valid_entry)) {
            /* Do not compare with the entry with interval == 0 where the
             * entry state will not be referenced.
             */
            break;
        }
        if (tas_entry->ticks == BCM_COSQ_TAS_ENTRY_TICKS_STAY) {
            /*
             * For ticks stay entry, its state will be referenced.
             * Still need to do the remap. But should leave in the
             * next loop.
             */
            last_valid_entry = 1;
        }
        qstate = tas_entry->state;
        /* Get the 0->1 or 1->0 change bitmap */
        xor_qstate = o_qstate ^ qstate;
        if (prop_tgcr) {
            /* check if any 1 -> 0 change */
            qdiff_tgcr = xor_qstate & o_qstate;
            if (qdiff_tgcr) {
                extra_enties++;
                o_qstate = qstate;
                continue;
            }
        }
        if (preemption_enable) {
            if (prop_tha) {
                /* Tha is required only when preemption is enabled. */
                /* check if any 0 -> 1 change on express queue */
                qdiff_tha = xor_qstate & qstate;
                qdiff_tha &= ~preempt_bmp;
                if (qdiff_tha) {
                    extra_enties++;
                    o_qstate = qstate;
                    continue;
                }
            }
        }
        o_qstate = qstate;
    }

    if (prop_tgcr) {
        /* Get the fixed tgcr value exclude tgcr_txpkt value which depends on
         * the queue */
        BCM_IF_ERROR_RETURN(
            bcmi_esw_tas_profile_delay_param_get(unit, TAS_DPARAM_TGCR_FIXED,
                                                 lport, 0, &tgcr_fix));
    }
    if (prop_tha && preemption_enable) {
        /* Get the Tha value */
        BCM_IF_ERROR_RETURN(
            bcmi_esw_tas_profile_delay_param_get(unit, TAS_DPARAM_THA,
                                                 lport, 0, &tha));
    }
    /* Create remap entries */
    num_hw_entries = num_entries + extra_enties;
    TAS_ALLOC(unit, hw_entries, bcm_cosq_tas_entry_t,
              sizeof(bcm_cosq_tas_entry_t) * num_hw_entries,
              "tas profile hw entries", 0, rv);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    /* Assign the remap entires */
    /* Initialize the old entry state o_qstate = the entry[0] state */
    o_qstate = entries[0].state;
    hw_index = 0;
    last_valid_entry = 0;
    /* Start from user entry[1] */
    for (i = 1; i < num_entries; i++) {
        o_phold = 0;
        phold = 0;
        qdiff_tgcr = 0;
        qdiff_tha = 0;
        /*
         * Remap decision : according to the difference between two continuous
         * user entries.
         */
        n_tas_entry = &entries[i];
        tas_entry = &entries[i - 1];
        if (last_valid_entry == 1) {
            /* When the last valid_entry hit, leave the loop */
            break;
        }
        if (n_tas_entry->ticks == BCM_COSQ_TAS_ENTRY_TICKS_GO_FIRST) {
            /*
             * Do not compare with the entry with interval == 0 where the
             * entry state will not be referenced.
             * Directly write i-1 entry and leave the for loop.
             */
            sal_memcpy(&hw_entries[hw_index], tas_entry,
                       sizeof(bcm_cosq_tas_entry_t));
            hw_index++;
            break;
        } else {
            if (n_tas_entry->ticks == BCM_COSQ_TAS_ENTRY_TICKS_STAY) {
                /*
                 * For ticks stay entry, its state will be referenced.
                 * Still need to do the remap. But should leave in the
                 * next loop.
                 */
                last_valid_entry = 1;
            }
            /* Get the next user entry state */
            qstate = n_tas_entry->state;
            /* Any 0->1 or 1->0 change bitmap */
            xor_qstate = o_qstate ^ qstate;
            if (prop_tgcr) {
                /*
                 * To consider Tgcr case :
                 *   Any gate open to close (1 -> 0) change need the Tgcr
                 */
                qdiff_tgcr = xor_qstate & o_qstate;
            }
            if (preemption_enable) {
                /*
                 * When preemption is enabled, any expess queue is open,
                 * the phold should be 1.
                 */
                o_phold = o_qstate & ~preempt_bmp;
                if (prop_tha) {
                    /*
                     * To consider Tha case :
                     *   Tha is required only when preemption is enabled.
                     *   Any express queue gate from close to open need Tha.
                     *   If any express queue has gate from open to close,
                     *   we need to consider Tgcr instead.
                     */
                    if (qdiff_tgcr & ~preempt_bmp) {
                        qdiff_tha = 0;
                    } else {
                        qdiff_tha = xor_qstate & qstate;
                        qdiff_tha &= ~preempt_bmp;
                    }
                }
                if (qdiff_tgcr) {
                    /*
                     * Any express queue has 1->0 change : set phold request
                     * to prevent preemptable traffic tx overrun
                     */
                     phold = qdiff_tgcr & ~preempt_bmp;
                }
            }
        }

        /* hw_index used to program to remapped(hw) table */
        if (hw_index >= num_hw_entries) {
            /* There must have something wrong that the hw_index exceed
             * num_hw_entries. */
            TAS_FREE(unit, hw_entries, 0);
            return BCM_E_INTERNAL;
        }
        /* Tha has higher precedence */
        if (qdiff_tha) {
            /* Validate the Tha against the entry->ticks */
            tha_ticks = (tha + ticks - 1) / ticks;
            if (tha_ticks >= tas_entry->ticks) {
                LOG_ERROR(BSL_LS_BCM_TAS,
                          (BSL_META_U(unit, "System required Hold Advance time"
                                      " Tha tick %d beyond the calendar table "
                                      "entry[%d]ticks %d\n"),
                           tha_ticks, i, tas_entry->ticks));
                TAS_FREE(unit, hw_entries, 0);
                return BCM_E_PARAM;
            }
            /*
             * Adjust the entries :
             * new entry state = previous entry state
             * previous entry tick = Ti - Tha
             * new entry tick = Tha
             * previous entry hold_request = o_phold
             * new entry hold_request = 1
             */
            hw_entries[hw_index].ticks = tas_entry->ticks - tha_ticks;
            hw_entries[hw_index].state = tas_entry->state;
            if (o_phold) {
                hw_entries[hw_index].flags = tas_entry->flags |
                                             BCM_COSQ_TAS_ENTRY_F_PREEMPT;
            } else {
                hw_entries[hw_index].flags = tas_entry->flags &
                                             ~BCM_COSQ_TAS_ENTRY_F_PREEMPT;
            }
            hw_index++;
            hw_entries[hw_index].ticks = tha_ticks;
            hw_entries[hw_index].state = tas_entry->state;
            hw_entries[hw_index].flags = tas_entry->flags |
                                         BCM_COSQ_TAS_ENTRY_F_PREEMPT;
        } else if (qdiff_tgcr) {
            /*
             * Tgcr dynamic value need to include the tgcr_txpkt vlaue which is
             * depending on the specified queue.
             */
            rv = bcmi_esw_tas_profile_delay_param_get(
                    unit, TAS_DPARAM_TGCR_DYNAMIC, lport, qdiff_tgcr,
                    &tgcr_txpkt);
            if (BCM_FAILURE(rv)) {
                TAS_FREE(unit, hw_entries, 0);
                return rv;
            }

            tgcr = tgcr_fix + tgcr_txpkt;
            tgcr_ticks = (tgcr + ticks - 1) / ticks;
            /* Validate the Tgcr against the entry->ticks */
            if (tgcr_ticks >= tas_entry->ticks) {
                LOG_ERROR(BSL_LS_BCM_TAS,
                          (BSL_META_U(unit, "System required gate close "
                                      "response time Tgcr tick %d beyond the "
                                      "calendar table entry[%d]ticks %d\n"),
                           tgcr_ticks, i, tas_entry->ticks));
                TAS_FREE(unit, hw_entries, 0);
                return BCM_E_PARAM;
            }
            /*
             * Adjust the entries :
             * new entry state = 0 (all closed)
             * previous entry tick = Ti - Tgcr
             * new entry tick = tgcr
             * previous entry hold_request = o_phold
             * new entry hold_request = phold
             */
            hw_entries[hw_index].ticks = tas_entry->ticks - tgcr_ticks;
            hw_entries[hw_index].state = tas_entry->state;
            if (o_phold) {
                hw_entries[hw_index].flags = tas_entry->flags |
                                             BCM_COSQ_TAS_ENTRY_F_PREEMPT;
            } else {
                hw_entries[hw_index].flags = tas_entry->flags &
                                             ~BCM_COSQ_TAS_ENTRY_F_PREEMPT;
            }
            hw_index++;
            hw_entries[hw_index].ticks = tgcr_ticks;
            hw_entries[hw_index].state = 0;
            if (phold) {
                hw_entries[hw_index].flags = tas_entry->flags |
                                             BCM_COSQ_TAS_ENTRY_F_PREEMPT;
            } else {
                hw_entries[hw_index].flags = tas_entry->flags &
                                             ~BCM_COSQ_TAS_ENTRY_F_PREEMPT;
            }
        } else {
            /* no need to adjust */
            sal_memcpy(&hw_entries[hw_index], tas_entry,
                       sizeof(bcm_cosq_tas_entry_t));
        }
        hw_index++;
        o_qstate = qstate;
    }

    if (hw_index >= num_hw_entries) {
        /*
         * There must have something wrong that the hw_index exceed
         * num_hw_entries.
         */
        TAS_FREE(unit, hw_entries, 0);
        return BCM_E_INTERNAL;
    }
    /* Program the last effective entry.
     * We do not compare the last entry with the first one
     */
    if (hw_index == 0) {
        /* Consider the case when num_entries == 1 */
        tas_entry = &entries[0];
    } else {
        /* Update the last entry to HW table */
        if (i < num_entries) {
            /* GO_FIRST or STAY break case */
            tas_entry = &entries[i];
        } else {
            tas_entry = &entries[num_entries - 1];
        }
    }
    sal_memcpy(&hw_entries[hw_index], tas_entry,
               sizeof(bcm_cosq_tas_entry_t));

    *new_entries = hw_entries;
    *num_new_entries = num_hw_entries;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_tas_profile_idx_get
 * Purpose:
 *     Get the inactive hw index. We need to program to the inactive sets
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *     config_idx       - (OUT) inactive hw index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_profile_idx_get(int unit, bcm_port_t lport, int *config_idx)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_profile_idx_get != NULL) {
        rv = td->tas_profile_idx_get(unit, lport, config_idx);
    }
    return rv;
}

/*
 * Function:
 *     bcmi_tas_profile_entries_write
 * Purpose:
 *     Program the profile entries to HW
 * Parameters:
 *     unit              - (IN) unit number
 *     lport             - (IN) logical port
 *     config_idx        - (IN) hw config idx
 *     entries           - (IN) profile entries
 *     num_entries       - (IN) number of entries
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_profile_entries_write(
    int unit,
    bcm_port_t lport,
    int config_idx,
    bcm_cosq_tas_entry_t *entries,
    int num_entries)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_profile_entries_write != NULL) {
        rv = td->tas_profile_entries_write(unit, lport, config_idx, entries,
                                           num_entries);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_tas_profile_schedule_config
 * Purpose:
 *      Device specific function to schedule the proper time to program the
 *      time information to HW. Also provide the callback function to call when
 *      the time information programmed to HW and kick-off the config change.
 *      In the callback function, the profile state should be changed to
 *      "committed".
 * Parameters:
 *     unit              - (IN) unit number
 *     lport             - (IN) logical port
 *     base_time         - (IN) new base time
 *     cycle_time        - (IN) new cycle time
 *     cycle_extension   - (IN) new cycle extension
 *     cb                - (IN) cb function to call when the time information
 *                              programmed to HW and kick-off the
 *                              config change.
 *     fn_data           - (IN) fn_data to supply in the callback
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_profile_schedule_config(int unit, bcm_port_t lport,
                                 bcm_ptp_timestamp_t base_time,
                                 uint32 cycle_time, uint32 cycle_extension,
                                 profile_commit_cb_fn cb, void *fn_data)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_profile_schedule_config != NULL) {
        rv = td->tas_profile_schedule_config(unit, lport, base_time,
                                             cycle_time, cycle_extension,
                                             cb, fn_data);
    }
    return rv;
}

/*
 * Function:
 *  bcmi_tas_profile_change
 * Purpose:
 *  Issue the config change to HW.
 *  Used for non-PTP mode.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_profile_change(int unit, bcm_port_t lport)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_profile_change != NULL) {
        rv = td->tas_profile_change(unit, lport);
    }
    return rv;
}

/*
 * Function:
 *     tas_pid_get
 * Purpose:
 *     Get the profile id by specifying port
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *     pid              - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_pid_get(int unit, bcm_port_t lport, bcm_cosq_tas_profile_id_t *pid)
{
    int max_count; /* Maximum number of pid to try */
    static uint32 last_allocated_pid = 0;
    tas_profile_ctrl_t *tp;
    bcm_cosq_tas_profile_id_t temp_pid;
    int rv = BCM_E_NONE;

    /* pid index should be 1 ~ TAS_PID_IDX_MASK */
    max_count = TAS_PID_IDX_MASK;

    while (max_count--) {
        /* increse the pid index */
        last_allocated_pid++;
        /* Handle the wrap case */
        if (TAS_PID_IDX_MASK == last_allocated_pid) {
            last_allocated_pid = 1;
        }
        /* Check the pid is used or not */
        temp_pid = last_allocated_pid | (lport << TAS_PID_PORT_START_BIT);
        rv = tas_profile_ctrl_get(unit, temp_pid, &tp);
        if (BCM_E_NOT_FOUND == rv) {
            /* Not used by others, take this as for the new profile */
            *pid = temp_pid;
            return (BCM_E_NONE);
        }
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }
    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *     tas_profile_profile_param_validate
 * Purpose:
 *     Parameter check for bcmi_esw_tas_profile_create
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport
 *     profile          - (IN) bcm profile structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_profile_param_validate(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_t *profile)
{
    bcm_ptp_timestamp_t *base_time;
    uint32 cycle_time, cycle_extension;
    int ptp_mode, gclt_size;
    bcmi_esw_tas_control_t  *tc = NULL;
    const tas_drv_t *td;

    /* TAS(bcmCosqTASControlTASEnable) is not required during profile create
     * Check ptp_mode for time parameter validation.
     * PTP lock (bcmCosqTASControlTASPTPLock) when ptp_mode = 1 is not required
     * during profile create.
     */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_control_get(unit, port, bcmCosqTASControlUsePTPTime,
                                 &ptp_mode));
    base_time = &profile->ptp_base_time;
    cycle_time = profile->ptp_cycle_time;
    cycle_extension = profile->ptp_max_cycle_extension;
    if (ptp_mode) {
        /* ptp_base should be non-zeros. */
        if (COMPILER_64_IS_ZERO(base_time->seconds) &&
            (base_time->nanoseconds == 0)) {
            return BCM_E_PARAM;
        }
        /* cycle_time should > cycle extension time */
        if (cycle_extension >= cycle_time) {
            return BCM_E_PARAM;
        }
    } else {
        /* ptp_base should be zeros. */
        if (!COMPILER_64_IS_ZERO(base_time->seconds) ||
            (base_time->nanoseconds != 0) ||
            (cycle_time != 0) ||
            (cycle_extension != 0)) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_status_get(unit, port, bcmCosqTASStatusMaxCalendarEntries,
                                &gclt_size));
    /* profile->num_entries should <= device GCLT size */
    if (profile->num_entries > gclt_size) {
        return BCM_E_PARAM;
    }
    /* profile->num_entries should > 0 */
    if (profile->num_entries <= 0) {
        return BCM_E_PARAM;
    }
    /* The first entry should not specify GO_FIRST. */
    if (profile->entries[0].ticks ==
        BCM_COSQ_TAS_ENTRY_TICKS_GO_FIRST) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    td = tc->tas_drvs;
    if (td->tas_profile_param_check != NULL) {
        return td->tas_profile_param_check(unit, profile);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     tas_profile_create
 * Purpose:
 *    The internal profile resource is created in this function.
 *
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *     profile          - (IN) bcm profile
 *     pid              - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_create(
    int unit,
    bcm_port_t lport,
    bcm_cosq_tas_profile_t *profile,
    bcm_cosq_tas_profile_id_t *pid)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp;
    int num_entries, count = 0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    TAS_LOCK(tc);

    TAS_PROFILE_ITER(tc->profile_list, tp, lport) {
        count++;
    }

    if (count >= MAX_TAS_PROFILE_CNT) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit, "Exceed supported profile count %d\n"),
                   MAX_TAS_PROFILE_CNT));
        TAS_UNLOCK(tc);
        return BCM_E_RESOURCE;
    }
    /* Create profile structure */
    TAS_ALLOC(unit, tp, tas_profile_ctrl_t,
              sizeof(tas_profile_ctrl_t),
              "tas profile", 0, rv);
    if (BCM_FAILURE(rv)) {
        TAS_UNLOCK(tc);
        return rv;
    }
    /* Get profile id */
    rv = tas_pid_get(unit, lport, pid);
    if (BCM_FAILURE(rv)) {
        TAS_FREE(unit, tp, 0);
        TAS_UNLOCK(tc);
        return rv;
    }
    /* Create profile lock */
    tp->profile_mutex = sal_mutex_create("tas_profile.lock");
    if (NULL == tp->profile_mutex) {
        TAS_FREE(unit, tp, 0);
        TAS_UNLOCK(tc);
        return BCM_E_RESOURCE;
    }

    num_entries = profile->num_entries;
    /* Create tp->user_entries */
    TAS_ALLOC(unit, tp->user_entries, bcm_cosq_tas_entry_t,
              sizeof(bcm_cosq_tas_entry_t) * num_entries,
              "tas profile entries", 0, rv);
    if (BCM_FAILURE(rv)) {
        sal_mutex_destroy(tp->profile_mutex);
        TAS_FREE(unit, tp, 0);
        TAS_UNLOCK(tc);
        return rv;
    }

    /* Initialize profile id. */
    tp->port = lport;
    tp->pid = *pid;
    tp->pstate = bcmCosqTASProfileInit;
    tp->hw_index = -1;
    tp->num_user_entries = num_entries;

    /* copy valid profile->entries to tp->user_entries */
    sal_memcpy(tp->user_entries, profile->entries,
               sizeof(bcm_cosq_tas_entry_t) * num_entries);

    sal_memcpy(&tp->base_time, &profile->ptp_base_time,
               sizeof(bcm_ptp_timestamp_t));

    tp->cycle_time = profile->ptp_cycle_time;
    tp->cycle_extension = profile->ptp_max_cycle_extension;

    /* Insert profile to tas profile set */
    TAS_PROFILE_INSERT(tc->profile_list, tp, lport);

    TAS_UNLOCK(tc);

    return BCM_E_NONE;
}

/*
 * Function:
 *     tas_profile_id_validate
 * Purpose:
 *     Validate the profile id
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport
 *     pid              - (IN) profile id
 *     lport            - (OUT) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_id_validate(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_port_t *lport)
{
    bcm_port_t local_port;

    BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &local_port));

    if (lport != NULL) {
        *lport = local_port;
    }
    if (local_port != ((pid >> TAS_PID_PORT_START_BIT) & TAS_PID_IDX_MASK)) {
        LOG_DEBUG(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit,
                             "Profile id is not associated with the port.")));
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     tas_gclt_entries_remap
 * Purpose:
 *     Remap the profile user entries and update the rempped one
 *     to internal profile structure
 * Parameters:
 *     unit             - (IN) unit number
 *     pid              - (IN) pid
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_gclt_entries_remap(int unit, bcm_cosq_tas_profile_id_t pid)
{
    tas_profile_ctrl_t  *tp = NULL;
    bcm_cosq_tas_entry_t    *hw_entries;
    int                     num_hw_entries;
    int                     rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(tas_profile_ctrl_get(unit, pid, &tp));

    rv = bcmi_tas_profile_entries_remap(unit, tp->port,
                                        tp->num_user_entries,
                                        tp->user_entries,
                                        &num_hw_entries,
                                        &hw_entries);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    tp->num_hw_entries = num_hw_entries;
    tp->hw_entries = hw_entries;

    return rv;
}

/*
 * Function:
 *     tas_profile_commit_validate
 * Purpose:
 *     Function to do below validation before commit the profile
 *      1) Check HW configure (TAS enable? PTP mode/locked? )
 *      2) Do entry remap according to the soc property
 *      3) Check pid status(resource)
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport
 *     tp               - (IN) Internal profile structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_commit_validate(
    int unit,
    bcm_gport_t port, tas_profile_ctrl_t *tp)
{
    int tas_enable, ptp_mode, ptp_lock;
    int rv = BCM_E_NONE;

    /* Check software resource is available */
    if (tp->pstate != bcmCosqTASProfileInit) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit, "Unable to commit profile in state %s\n"),
                   pstate_string[tp->pstate]));
        return BCM_E_CONFIG;
    }

    /* Check tas enable */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_control_get(unit, port, bcmCosqTASControlTASEnable,
                                 &tas_enable));

    /* check ptp_mode and ptp_lock */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_control_get(unit, port, bcmCosqTASControlUsePTPTime,
                                 &ptp_mode));
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tas_control_get(unit, -1, bcmCosqTASControlTASPTPLock,
                                 &ptp_lock));
    if (ptp_mode) {
        if (!ptp_lock) {
            LOG_ERROR(BSL_LS_BCM_TAS,
                      (BSL_META_U(unit, "PTP need to be locked to run "
                                  "PTP mode.\n")));
            return BCM_E_CONFIG;
        }
        /*
         * Need to validate basetime compare against currnt time
         * when GateEnable = TRUE
         */
        if (tas_enable) {
            const tas_drv_t         *td;
            bcmi_esw_tas_control_t  *tc = NULL;

            BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
            td = tc->tas_drvs;
            rv = BCM_E_NONE;
            if (td->tas_profile_basetime_check != NULL) {
                rv = td->tas_profile_basetime_check(unit, tp->base_time);
            }
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    } else {
        /* For non-PTP mode tas has to be enabled */
        if (!tas_enable) {
            LOG_ERROR(BSL_LS_BCM_TAS,
                      (BSL_META_U(unit, "TAS is not enabled !!\n")));
            return BCM_E_DISABLED;
        }
    }
    /*
     * Prepare the HW profile(gclt) entries
     * Allocate hw ertries resource. And do entry remap if requested
     * from soc property.
     */
    BCM_IF_ERROR_RETURN(
        tas_gclt_entries_remap(unit, tp->pid));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_tas_profile_event_notify
 * Purpose:
 *    Notified by chip tas interrupt handler
 *
 *    Or notified when TAS is disabled to update the profile state.
 *        active -> expired, commit/pending -> error
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *     enable           - (IN) tas is enable or disabled
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_profile_event_notify(
    int unit,
    bcm_port_t lport,
    bcmi_tas_profile_event_type_t event)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    bcm_cosq_tas_profile_id_t   active_pid, process_pid;
    tas_profile_ctrl_t  *tp_pending = NULL;
    tas_profile_ctrl_t  *tp_active = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    TAS_LOCK(tc);
    active_pid = tc->active_pid[lport];
    process_pid = tc->process_pid[lport];
    TAS_UNLOCK(tc);
    if (active_pid) {
        rv = tas_profile_ctrl_get(unit, active_pid, &tp_active);
        if (BCM_FAILURE(rv)) {
            TAS_UNLOCK(tc);
            return rv;
        }
    }
    if (process_pid) {
        rv = tas_profile_ctrl_get(unit, process_pid, &tp_pending);
        if (BCM_FAILURE(rv)) {
            TAS_UNLOCK(tc);
            return rv;
        }
    }

    switch (event) {
    case bcmiTASProfileEventProfileChange:
        /* Must have the valid process_pid */
        if (!process_pid) {
            return BCM_E_INTERNAL;
        }
        TAS_PROFILE_LOCK(tp_pending);
        tp_pending->pstate = bcmCosqTASProfileActive;
        TAS_PROFILE_UNLOCK(tp_pending);
        if (active_pid) {
            TAS_PROFILE_LOCK(tp_active);
            tp_active->pstate = bcmCosqTASProfileExpired;
            tp_active->hw_index = -1;
            TAS_PROFILE_UNLOCK(tp_active);
        }
        TAS_LOCK(tc);
        tc->active_pid[lport] = process_pid;
        tc->process_pid[lport] = 0;
        TAS_UNLOCK(tc);
        break;
    case bcmiTASProfileEventBaseTimeErr:
        /* HW detects that the base time for new scheduler is already passed.
         * It'll abort the change command and continue the old schedule.
         * So keep the active pid, update the pending pid to error state.
         * Must have the valid process_pid
         */
        if (!process_pid) {
            return BCM_E_INTERNAL;
        }
        TAS_PROFILE_LOCK(tp_pending);
        tp_pending->pstate = bcmCosqTASProfileError;
        tp_pending->hw_index = -1;
        TAS_PROFILE_UNLOCK(tp_pending);
        TAS_LOCK(tc);
        tc->process_pid[lport] = 0;
        TAS_UNLOCK(tc);
        break;
    case bcmiTASProfileEventNextCycleTimeErr:
        /* HW detects the next cycle start time is passed.
         * It'll go back to ERR_GATE_STATE after finish the current cycle.
         * So update the active to error.
         */
    case bcmiTASProfileEventECCErr:
        /* HW detect that 2-bit ecc error when read the GCLT.
         * It'll stop execute GCLT and back to ERR_GATE_STATE.
         */
    case bcmiTASProfileEventPtrErr:
        /* HW detect the read pointer over the GCLT size.
         * It'll stop execute GCLT and back to ERR_GATE_STATE.
         * Above cases must have the valid active_pid
         */
        if (!active_pid) {
            return BCM_E_INTERNAL;
        }
        TAS_PROFILE_LOCK(tp_active);
        tp_active->pstate = bcmCosqTASProfileError;
        tp_active->hw_index = -1;
        TAS_PROFILE_UNLOCK(tp_active);
        TAS_LOCK(tc);
        tc->active_pid[lport] = 0;
        TAS_UNLOCK(tc);
        break;
    case bcmiTASProfileEventTASDisable:
        /* Software prgrame the TAS disable via bcm_cosq_tas_control_set. */
        if (process_pid) {
            TAS_PROFILE_LOCK(tp_pending);
            tp_pending->pstate = bcmCosqTASProfileError;
            tp_pending->hw_index = -1;
            TAS_PROFILE_UNLOCK(tp_pending);
        }
        if (active_pid) {
            TAS_PROFILE_LOCK(tp_active);
            tp_active->pstate = bcmCosqTASProfileExpired;
            tp_active->hw_index = -1;
            TAS_PROFILE_UNLOCK(tp_active);
        }
        TAS_LOCK(tc);
        tc->active_pid[lport] = 0;
        tc->process_pid[lport] = 0;
        TAS_UNLOCK(tc);
        break;
    default:
        break;
    }

    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_UP(unit, lport,
                           "tas_profile_event_notify profile_event %d \n"
                           "Active profile(pid 0x%08x) state = %s \n"
                           "Pending profile (pid 0x%08x) state = %s \n"),
               event, active_pid,
               active_pid ? pstate_string[tp_active->pstate] : "N/A",
               process_pid,
               process_pid ? pstate_string[tp_pending->pstate] : "N/A"));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_tas_commit_callback
 * Purpose:
 *      The callback function to notify the information (config_change_time) of
 *      the scheduled commit-profile.
 *      The profile status will be updated accordingly.
 * Parameters:
 *     unit                 - (IN) unit number
 *     lport                - (IN) logical port
 *     config_change_time   - (IN) config change time
 *     fn_data              - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_tas_commit_callback(
    int unit,
    bcm_port_t lport,
    bcm_ptp_timestamp_t config_change_time,
    void *fn_data)
{
    bcm_cosq_tas_profile_id_t pid;
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    pid = tc->process_pid[lport];
    TAS_UNLOCK(tc);
    if (!pid) {
        LOG_DEBUG(BSL_LS_BCM_TAS,
                  (BSL_META_UP(unit, lport,
                               "The commit profile pid is dismissed !")));
        return BCM_E_INTERNAL;
    }
    rv = tas_profile_ctrl_get(unit, pid, &tp);
    if (BCM_FAILURE(rv)) {
        TAS_UNLOCK(tc);
        return rv;
    }
    TAS_PROFILE_LOCK(tp);
    sal_memcpy(&tp->config_change_time, &config_change_time,
               sizeof(bcm_ptp_timestamp_t));
    tp->pstate = bcmCosqTASProfilePending;
    TAS_PROFILE_UNLOCK(tp);
    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_UP(unit, lport,
                           "bcmi_tas_commit_callback pid 0x%08x state %s\n"),
               pid, pstate_string[tp->pstate]));

    return BCM_E_NONE;
}

/*
 * Function:
 *     tas_profile_schedule
 * Purpose:
 *     Schedule the process to handle the committed profile.
 *     For PTP mode, invoke the device specific schedule function to handle the
 *     the device progamming constrain on time configurations.
 *     For non-PTP mode, invoke the 'profile change' function.
 * Parameters:
 *     unit             - (IN) unit number
 *     tp               - (IN) internal profile
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_schedule(int unit, tas_profile_ctrl_t  *tp)
{
    int rv = BCM_E_UNAVAIL;

    if (tp->cycle_time) {
        rv = bcmi_tas_profile_schedule_config(unit, tp->port, tp->base_time,
                                              tp->cycle_time,
                                              tp->cycle_extension,
                                              bcmi_tas_commit_callback,
                                              NULL);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        /* Issue config change */
        rv = bcmi_tas_profile_change(unit, tp->port);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TAS,
                      (BSL_META_UP(unit, tp->port,
                                   "Fail to issue config change !!\n")));
            tp->pstate = bcmCosqTASProfileError;
            tp->hw_index = -1;
            return rv;
        }
        tp->pstate = bcmCosqTASProfilePending;
    }

    return rv;
}

/*
 * Function:
 *      tas_profile_commit
 * Purpose:
 *      Program the GCLT only.
 *      Config change and time releated configuration are not in this function.
 * Parameters:
 *     unit             - (IN) unit number
 *     tp               - (IN) Internal profile structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_commit(int unit, tas_profile_ctrl_t  *tp)
{
    int config_idx;
    int rv = BCM_E_NONE;

    rv = bcmi_tas_profile_idx_get(unit, tp->port, &config_idx);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = bcmi_tas_profile_entries_write(unit, tp->port, config_idx,
                                        tp->hw_entries, tp->num_hw_entries);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    tp->pstate = bcmCosqTASProfileCommitted;
    tp->hw_index = config_idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *      tas_profile_get
 * Purpose:
 *      Helper function for bcmi_esw_tas_profile_get.
 *      Get the bcm profile information from internal profile.
 * Parameters:
 *     unit             - (IN) unit number
 *     tp               - (IN) internal profile
 *     profile          - (OUT) bcm profile
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_get(
    int unit,
    tas_profile_ctrl_t *tp,
    bcm_cosq_tas_profile_t *profile)
{
    if (profile == NULL) {
        return BCM_E_PARAM;
    }
    bcm_cosq_tas_profile_t_init(profile);
    profile->num_entries = tp->num_user_entries;
    sal_memcpy(&profile->entries, tp->user_entries,
               tp->num_user_entries * sizeof(bcm_cosq_tas_entry_t));
    sal_memcpy(&profile->ptp_base_time, &tp->base_time,
               sizeof(bcm_ptp_timestamp_t));
    profile->ptp_cycle_time = tp->cycle_time;
    profile->ptp_max_cycle_extension = tp->cycle_extension;

    return BCM_E_NONE;
}

/*
 * Function:
 *     tas_profile_set
 * Purpose:
 *      Helper function for bcmi_esw_tas_profile_set
 *      Configure the internal profile database(specified via pid) according to
 *      user configured bcm profile information.
 * Parameters:
 *     unit             - (IN) unit number
 *     tp               - (IN) internal profile
 *     profile          - (IN) bcm profile
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_set(
    int unit,
    tas_profile_ctrl_t *tp,
    bcm_cosq_tas_profile_t *profile)
{
    int rv;

    /* Check the profile state */
    if ((tp->pstate == bcmCosqTASProfileCommitted) ||
        (tp->pstate == bcmCosqTASProfilePending) ||
        (tp->pstate == bcmCosqTASProfileActive)) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_UP(unit, tp->port, "Unable to set profile in state "
                               "%s\n"), pstate_string[tp->pstate]));
        return BCM_E_CONFIG;
    }

    if (profile->num_entries != tp->num_user_entries) {
        TAS_FREE(unit, tp->user_entries, 0);
        tp->user_entries = NULL;
        TAS_ALLOC(unit, tp->user_entries, bcm_cosq_tas_entry_t,
                  sizeof(bcm_cosq_tas_entry_t) * profile->num_entries,
                  "tas profile entries", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    tp->pstate = bcmCosqTASProfileInit;
    tp->num_user_entries = profile->num_entries;
    /* copy valid profile->entries to tp->user_entries */
    sal_memcpy(tp->user_entries, profile->entries,
               sizeof(bcm_cosq_tas_entry_t) * profile->num_entries);
    sal_memcpy(&tp->base_time, &profile->ptp_base_time,
               sizeof(bcm_ptp_timestamp_t));

    tp->cycle_time = profile->ptp_cycle_time;
    tp->cycle_extension = profile->ptp_max_cycle_extension;

    return BCM_E_NONE;
}

/*
 * Function:
 *     tas_profile_destroy
 * Purpose:
 *     Helper function for bcmi_esw_tas_profile_destroy.
 *     Destroy and free the resource of the internal profile.
 * Parameters:
 *     unit             - (IN) unit number
 *     tp               - (IN) internal profile
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_destroy(int unit, tas_profile_ctrl_t  *tp)
{
    TAS_PROFILE_LOCK(tp);
    if ((tp->pstate == bcmCosqTASProfileCommitted) ||
        (tp->pstate == bcmCosqTASProfilePending) ||
        (tp->pstate == bcmCosqTASProfileActive)) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_UP(unit, tp->port, "Unable to destroy profile in "
                               "state %s\n"),
                   pstate_string[tp->pstate]));
        TAS_PROFILE_UNLOCK(tp);
        return BCM_E_CONFIG;
    }

    if (tp->user_entries) {
        TAS_FREE(unit, tp->user_entries, 0);
        tp->user_entries = NULL;
    }
    if (tp->hw_entries) {
        TAS_FREE(unit, tp->hw_entries, 0);
        tp->hw_entries = NULL;
    }
    TAS_PROFILE_UNLOCK(tp);
    if (tp->profile_mutex) {
        sal_mutex_destroy(tp->profile_mutex);
        tp->profile_mutex = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      tas_profile_status_get
 * Purpose:
 *      Helper function to get the profile status by specifing the pid.
 *      Collect the bcm profile status from internal profile database.
 * Parameters:
 *     unit             - (IN) unit number
 *     tp               - (IN) internal profile
 *     status           - (OUT) profile status
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_status_get(
    int unit,
    tas_profile_ctrl_t *tp,
    bcm_cosq_tas_profile_status_t *status)
{
    if (status == NULL) {
        return BCM_E_PARAM;
    }
    bcm_cosq_tas_profile_status_t_init(status);
    status->profile_state = tp->pstate;
    status->num_entries = tp->num_hw_entries;
    if (status->num_entries > 0) {
        sal_memcpy(&status->entries, tp->hw_entries,
                   tp->num_hw_entries * sizeof(bcm_cosq_tas_entry_t));
    }

    sal_memcpy(&status->config_change_time, &tp->config_change_time,
               sizeof(bcm_ptp_timestamp_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_create
 * Purpose:
 *     Create a profile by specifying the profile settings including calendar
 *     table, ptp time information for PTP mode.
 *     A profile id will be assigned along with the settings.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     profile          - (IN) Pointer to profile structure which specifies
 *                             entries in calendar table, ptp time information
 *                             for PTP mode
 *     pid              - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_create(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_t *profile,
    bcm_cosq_tas_profile_id_t *pid)
{
    bcm_port_t lport;
    bcmi_esw_tas_control_t  *tc = NULL;
    int rv;

    if ((pid == NULL) || (profile == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    /* parameter validate - port */
    BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &lport));
    /* parameter validate - content of the profile structure */
    rv = tas_profile_profile_param_validate(unit, port, profile);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Create the profile and get the profile id */
    rv = tas_profile_create(unit, lport, profile, pid);

    return rv;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_commit
 * Purpose:
 *     Commit the profile to indicate the profile is ready to write to hardware.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_commit(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    int rv;
    bcm_port_t lport;
    tas_profile_ctrl_t *tp;

    /* parameter validate - port and pid */
    BCM_IF_ERROR_RETURN(
        tas_profile_id_validate(unit, port, pid, &lport));
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    /* Check hardware resource is available to do the commit.
     * Currently, only allow one profile in process (either commit or pending)
     *  at one time.
     */
    if (tc->process_pid[lport] != 0) {
        TAS_UNLOCK(tc);
        return BCM_E_RESOURCE;
    }
    tc->process_pid[lport] = pid;
    rv = tas_profile_ctrl_get(unit, pid, &tp);
    if (BCM_FAILURE(rv)) {
        TAS_UNLOCK(tc);
        return rv;
    }
    TAS_UNLOCK(tc);

    TAS_PROFILE_LOCK(tp);
    rv = tas_profile_commit_validate(unit, port, tp);
    if (BCM_FAILURE(rv)) {
        tp->pstate = bcmCosqTASProfileError;
        TAS_PROFILE_UNLOCK(tp);
        TAS_LOCK(tc);
        tc->process_pid[lport] = 0;
        TAS_UNLOCK(tc);
        return rv;
    }

    rv = tas_profile_commit(unit, tp);
    if (BCM_FAILURE(rv)) {
        tp->pstate = bcmCosqTASProfileError;
        TAS_PROFILE_UNLOCK(tp);
        TAS_LOCK(tc);
        tc->process_pid[lport] = 0;
        TAS_UNLOCK(tc);
        return rv;
    }

    rv = tas_profile_schedule(unit, tp);
    if (BCM_FAILURE(rv)) {
        tp->pstate = bcmCosqTASProfileError;
        TAS_PROFILE_UNLOCK(tp);
        TAS_LOCK(tc);
        tc->process_pid[lport] = 0;
        TAS_UNLOCK(tc);
        return rv;
    }

    TAS_PROFILE_UNLOCK(tp);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_destroy
 * Purpose:
 *     Destroy the profile and associated resources.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_destroy(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    bcm_port_t  lport;
    tas_profile_ctrl_t *tp;
    bcmi_esw_tas_control_t  *tc = NULL;

    BCM_IF_ERROR_RETURN(
        tas_profile_id_validate(unit, port, pid, &lport));

    BCM_IF_ERROR_RETURN(tas_profile_ctrl_get(unit, pid, &tp));
    BCM_IF_ERROR_RETURN(tas_profile_destroy(unit, tp));

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    TAS_LOCK(tc);
    TAS_PROFILE_REMOVE(tc->profile_list, tas_profile_ctrl_t, tp, lport);
    TAS_FREE(unit, tp, 0);
    TAS_UNLOCK(tc);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_destroy_all
 * Purpose:
 *     Destroy all the profile associated with the port.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_destroy_all(
    int unit,
    bcm_gport_t port)
{
    bcm_port_t lport;
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t *tp;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &lport));
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    TAS_PROFILE_ITER(tc->profile_list, tp, lport) {
        TAS_PROFILE_LOCK(tp);
        rv = tas_profile_destroy(unit, tp);
        if (BCM_FAILURE(rv)) {
            TAS_PROFILE_UNLOCK(tp);
            TAS_UNLOCK(tc);
            return rv;
        }
        TAS_PROFILE_REMOVE(tc->profile_list, tas_profile_ctrl_t, tp, lport);
        TAS_PROFILE_UNLOCK(tp);
        TAS_FREE(unit, tp, 0);
    }

    TAS_UNLOCK(tc);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_get
 * Purpose:
 *     Get the profile information by specifying the profile id.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 *     profile          - (OUT) pointer to profile structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t *profile)
{
    tas_profile_ctrl_t      *tp;
    int rv;

    if (profile == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        tas_profile_id_validate(unit, port, pid, NULL));
    BCM_IF_ERROR_RETURN(tas_profile_ctrl_get(unit, pid, &tp));
    TAS_PROFILE_LOCK(tp);
    rv = tas_profile_get(unit, tp, profile);
    TAS_PROFILE_UNLOCK(tp);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_set
 * Purpose:
 *     Modify the profile information
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 *     profile          - (IN) pointer to profile structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t *profile)
{
    tas_profile_ctrl_t  *tp = NULL;
    int rv;

    if (profile == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        tas_profile_id_validate(unit, port, pid, NULL));
    BCM_IF_ERROR_RETURN(
        tas_profile_profile_param_validate(unit, port, profile));

    BCM_IF_ERROR_RETURN(tas_profile_ctrl_get(unit, pid, &tp));
    TAS_PROFILE_LOCK(tp);
    rv = tas_profile_set(unit, tp, profile);
    TAS_PROFILE_UNLOCK(tp);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_traverse
 * Purpose:
 *     Traverse the set of profiles associated with the specified port.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     cb               - (IN) A pointer to callback function to call
 *                             for each profile in the specified gport.
 *     user_data        - (IN) Pointer to user data to supply in the callback
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_traverse(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_traverse_cb cb,
    void *user_data)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp = NULL;
    bcm_port_t lport;
    int rv;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tas_gport_validate(unit, port, &lport));
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    TAS_PROFILE_ITER(tc->profile_list, tp, lport) {
        TAS_PROFILE_LOCK(tp);
        rv = cb(unit, port, tp->pid, user_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TAS,
                      (BSL_META_UP(unit, lport, "Error on user call back with "
                       "pid 0x%x \n"), tp->pid));
            TAS_PROFILE_UNLOCK(tp);
            TAS_UNLOCK(tc);
            return rv;
        }
        TAS_PROFILE_UNLOCK(tp);
    }
    TAS_UNLOCK(tc);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tas_profile_status_get
 * Purpose:
 *     Get the profile status like profile state, config change time and
 *     entries in the hardware calendar table.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 *     status           - (OUT) pointer to profile status structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_profile_status_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_status_t *status)
{
    int rv;
    tas_profile_ctrl_t  *tp = NULL;

    BCM_IF_ERROR_RETURN(
        tas_profile_id_validate(unit, port, pid, NULL));
    BCM_IF_ERROR_RETURN(tas_profile_ctrl_get(unit, pid, &tp));
    TAS_PROFILE_LOCK(tp);
    rv = tas_profile_status_get(unit, tp, status);
    TAS_PROFILE_UNLOCK(tp);
    return rv;
}

/*
 * Function:
 *      tas_profile_destroy_all
 * Purpose:
 *      Helper function for bcmi_esw_tas_profile_destroy_all.
 *      Destroy all allocated resource of the profiles on specified port.
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
tas_profile_destroy_all(int unit, bcm_port_t lport)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp = NULL;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    if (tc->profile_list_count <= lport) {
        return BCM_E_NONE;
    }
    TAS_PROFILE_ITER(tc->profile_list, tp, lport) {
        if (tp->user_entries) {
            TAS_FREE(unit, tp->user_entries, 0);
            tp->user_entries = NULL;
        }
        if (tp->hw_entries) {
            TAS_FREE(unit, tp->hw_entries, 0);
            tp->hw_entries = NULL;
        }
        if (tp->profile_mutex) {
            sal_mutex_destroy(tp->profile_mutex);
            tp->profile_mutex = NULL;
        }
    }
    TAS_PROFILE_REMOVE_ALL(tc->profile_list, tas_profile_ctrl_t, lport);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tas_cleanup
 * Purpose:
 *     Internal function to clear the resource of the tas module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_cleanup(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    bcm_port_t port;
    int rv;

    if (tas_control[unit] != NULL) {
        tc = tas_control[unit];
        if (tc->profile_list) {
            PBMP_ALL_ITER(unit, port) {
                rv = tas_profile_destroy_all(unit, port);
                if (BCM_FAILURE (rv)) {
                    return rv;
                }
            }
        }
        if (tc->active_pid) {
            TAS_FREE(unit, tc->active_pid, 0);
            tc->active_pid = NULL;
        }
        if (tc->process_pid) {
            TAS_FREE(unit, tc->process_pid, 0);
            tc->process_pid = NULL;
        }
        if (tc->tas_properties) {
            TAS_FREE(unit, tc->tas_properties, 0);
            tc->tas_properties = NULL;
        }
        if (tc->tas_mutex != NULL) {
            sal_mutex_destroy(tc->tas_mutex);
            tc->tas_mutex = NULL;
        }
        TAS_FREE(unit, tc, 0);
        tas_control[unit] = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tas_deinit
 * Purpose:
 *     Internal function to free the resource of the tas module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_deinit(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));

    if (tc->tas_drvs) {
        const tas_drv_t   *td = tc->tas_drvs;
        if (td->tas_profile_deinit != NULL) {
            BCM_IF_ERROR_RETURN(td->tas_profile_deinit(unit));
        }
    }

    return bcmi_esw_tas_cleanup(unit);
}


#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
bcmi_esw_tas_scache_alloc(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int alloc_size = 0;
    int profile_alloc_size = 0, profile_list_count;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    profile_list_count = tc->profile_list_count;
    TAS_UNLOCK(tc);
    /* profile list count */
    alloc_size += sizeof(int);
    /* pid */
    profile_alloc_size += sizeof(bcm_cosq_tas_profile_id_t);
    /* pstate */
    profile_alloc_size += sizeof(bcm_cosq_tas_profile_state_t);
    /* profile hw index */
    profile_alloc_size += sizeof(int);
    /* user entries */
    profile_alloc_size += sizeof(int);
    profile_alloc_size += sizeof(bcm_cosq_tas_entry_t) *
                          BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE;
    /* hw entries */
    profile_alloc_size += sizeof(int);
    profile_alloc_size += sizeof(bcm_cosq_tas_entry_t) *
                          BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE;
    /* base time */
    profile_alloc_size += sizeof(bcm_ptp_timestamp_t);
    /* config change time */
    profile_alloc_size += sizeof(bcm_ptp_timestamp_t);
    /* cycle time */
    profile_alloc_size += sizeof(uint32);
    /* cycle extension time */
    profile_alloc_size += sizeof(uint32);

    alloc_size += MAX_TAS_PROFILE_CNT * profile_alloc_size *
                  profile_list_count;

    /* profile cnt, active_pid, process_pid */
    alloc_size += (sizeof(uint16) + sizeof(bcm_cosq_tas_profile_id_t) +
                   sizeof(bcm_cosq_tas_profile_id_t)) * profile_list_count;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ,
                          BCMI_COSQ_WB_SCACHE_PARTITION_TAS);
    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, 1, alloc_size,
                                &scache_ptr, BCMI_COSQ_WB_TAS_VERSION_1_0,
                                NULL));
    return BCM_E_NONE;
}

int
bcmi_esw_tas_sync(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp = NULL;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *temp_ptr;
    int i;
    int rv;
    uint16 profile_cnt; /* profile count per index */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ,
                          BCMI_COSQ_WB_SCACHE_PARTITION_TAS);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                 &scache_ptr, BCMI_COSQ_WB_TAS_VERSION_1_0,
                                 NULL);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit, "Fail to get Scahce ptr !\n")));
        return rv;
    }
    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    /* store profile list count */
    sal_memcpy(scache_ptr, &tc->profile_list_count, sizeof(int));
    scache_ptr += sizeof(int);

    for (i = 0; i < tc->profile_list_count; i++) {
        profile_cnt = 0;
        /* reserve the space for the profile_cnt */
        temp_ptr = scache_ptr;
        scache_ptr += sizeof(uint16);
        TAS_PROFILE_ITER(tc->profile_list, tp, i) {
            /* store pid */
            sal_memcpy(scache_ptr, &tp->pid, sizeof(bcm_cosq_tas_profile_id_t));
            scache_ptr += sizeof(bcm_cosq_tas_profile_id_t);
            /* store profile state */
            sal_memcpy(scache_ptr, &tp->pstate,
                       sizeof(bcm_cosq_tas_profile_state_t));
            scache_ptr += sizeof(bcm_cosq_tas_profile_state_t);
            /* store hw_index */
            sal_memcpy(scache_ptr, &tp->hw_index, sizeof(int));
            scache_ptr += sizeof(int);
            /* store num_user_entries */
            sal_memcpy(scache_ptr, &tp->num_user_entries, sizeof(int));
            scache_ptr += sizeof(int);
            /* store user entries */
            sal_memcpy(scache_ptr, tp->user_entries,
                       tp->num_user_entries * sizeof(bcm_cosq_tas_entry_t));
            scache_ptr += tp->num_user_entries * sizeof(bcm_cosq_tas_entry_t);
            /* store num_hw_entries */
            sal_memcpy(scache_ptr, &tp->num_hw_entries, sizeof(int));
            scache_ptr += sizeof(int);
            if (tp->num_hw_entries) {
                /* store hw entries */
                sal_memcpy(scache_ptr, tp->hw_entries,
                           tp->num_hw_entries * sizeof(bcm_cosq_tas_entry_t));
                scache_ptr += tp->num_hw_entries * sizeof(bcm_cosq_tas_entry_t);
            }
            /* store base_time */
            sal_memcpy(scache_ptr, &tp->base_time, sizeof(bcm_ptp_timestamp_t));
            scache_ptr += sizeof(bcm_ptp_timestamp_t);
            /* store config_change_time */
            sal_memcpy(scache_ptr, &tp->config_change_time,
                       sizeof(bcm_ptp_timestamp_t));
            scache_ptr += sizeof(bcm_ptp_timestamp_t);
            /* store cycle_time */
            sal_memcpy(scache_ptr, &tp->cycle_time, sizeof(uint32));
            scache_ptr += sizeof(uint32);
            /* store cycle_extension */
            sal_memcpy(scache_ptr, &tp->cycle_extension, sizeof(uint32));
            scache_ptr += sizeof(uint32);
            profile_cnt++;
        }
        /* store the profile_cnt to the reseved ptr */
        sal_memcpy(temp_ptr, &profile_cnt, sizeof(uint16));

        /* store active_pid */
        sal_memcpy(scache_ptr, &tc->active_pid[i],
                   sizeof(bcm_cosq_tas_profile_id_t));
        scache_ptr += sizeof(bcm_cosq_tas_profile_id_t);
        /* store process_pid */
        sal_memcpy(scache_ptr, &tc->process_pid[i],
                   sizeof(bcm_cosq_tas_profile_id_t));
        scache_ptr += sizeof(bcm_cosq_tas_profile_id_t);
    }
    TAS_UNLOCK(tc);
    return BCM_E_NONE;
}

STATIC int
bcmi_esw_tas_recover(int unit, uint8 *scache_ptr)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp = NULL;
    int i, j;
    uint16 profile_cnt;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    sal_memcpy(&tc->profile_list_count, scache_ptr, sizeof(int));
    scache_ptr += sizeof(int);

    for (i = 0; i < tc->profile_list_count; i++) {
        sal_memcpy(&profile_cnt, scache_ptr, sizeof(uint16));
        scache_ptr += sizeof(uint16);

        for (j = 0; j < profile_cnt; j ++) {
            tp = NULL;
            /* Allocate profile structure */
            TAS_ALLOC(unit, tp, tas_profile_ctrl_t,
                      sizeof(tas_profile_ctrl_t),
                      "tas profile", 0, rv);
            if (BCM_FAILURE(rv)) {
                TAS_UNLOCK(tc);
                return rv;
            }
            /* Create profile lock */
            tp->profile_mutex = sal_mutex_create("tas_profile.lock");
            if (NULL == tp->profile_mutex) {
                TAS_FREE(unit, tp, 0);
                TAS_UNLOCK(tc);
                return BCM_E_RESOURCE;
            }
            /* restore port */
            tp->port = i;
            /* restore pid */
            sal_memcpy(&tp->pid, scache_ptr, sizeof(bcm_cosq_tas_profile_id_t));
            scache_ptr += sizeof(bcm_cosq_tas_profile_id_t);
            /* restore profile state */
            sal_memcpy(&tp->pstate, scache_ptr,
                       sizeof(bcm_cosq_tas_profile_state_t));
            scache_ptr += sizeof(bcm_cosq_tas_profile_state_t);
            /* restore hw_index */
            sal_memcpy(&tp->hw_index, scache_ptr, sizeof(int));
            scache_ptr += sizeof(int);
            /* restore num_user_entries */
            sal_memcpy(&tp->num_user_entries, scache_ptr, sizeof(int));
            scache_ptr += sizeof(int);
            /* Allocate tp->user_entries */
            TAS_ALLOC(unit, tp->user_entries, bcm_cosq_tas_entry_t,
                      sizeof(bcm_cosq_tas_entry_t) * tp->num_user_entries,
                      "tas profile entries", 0, rv);
            if (BCM_FAILURE(rv)) {
                sal_mutex_destroy(tp->profile_mutex);
                TAS_FREE(unit, tp, 0);
                TAS_UNLOCK(tc);
                return rv;
            }
            /* restore user entries */
            sal_memcpy(tp->user_entries, scache_ptr,
                       tp->num_user_entries * sizeof(bcm_cosq_tas_entry_t));
            scache_ptr += tp->num_user_entries * sizeof(bcm_cosq_tas_entry_t);
            /* restore num_hw_entries */
            sal_memcpy(&tp->num_hw_entries, scache_ptr, sizeof(int));
            scache_ptr += sizeof(int);
            if (tp->num_hw_entries) {
                /* Allocate tp->user_entries*/
                TAS_ALLOC(unit, tp->hw_entries, bcm_cosq_tas_entry_t,
                          sizeof(bcm_cosq_tas_entry_t) * tp->num_hw_entries,
                          "tas profile entries", 0, rv);
                if (BCM_FAILURE(rv)) {
                    TAS_FREE(unit, tp->user_entries, 0);
                    tp->user_entries = NULL;
                    sal_mutex_destroy(tp->profile_mutex);
                    TAS_FREE(unit, tp, 0);
                    TAS_UNLOCK(tc);
                    return rv;
                }
                /* restore hw entries */
                sal_memcpy(tp->hw_entries, scache_ptr,
                           tp->num_hw_entries * sizeof(bcm_cosq_tas_entry_t));
                scache_ptr += tp->num_hw_entries * sizeof(bcm_cosq_tas_entry_t);
            }
            /* restore base_time */
            sal_memcpy(&tp->base_time, scache_ptr, sizeof(bcm_ptp_timestamp_t));
            scache_ptr += sizeof(bcm_ptp_timestamp_t);
            /* restore config_change_time */
            sal_memcpy(&tp->config_change_time, scache_ptr,
                       sizeof(bcm_ptp_timestamp_t));
            scache_ptr += sizeof(bcm_ptp_timestamp_t);
            /* restore cycle_time */
            sal_memcpy(&tp->cycle_time, scache_ptr, sizeof(uint32));
            scache_ptr += sizeof(uint32);
            /* restore cycle_extension */
            sal_memcpy(&tp->cycle_extension, scache_ptr, sizeof(uint32));
            scache_ptr += sizeof(uint32);
            /* Insert profile to tas profile set */
            TAS_PROFILE_INSERT(tc->profile_list, tp, i);
        }
        /* store active_pid */
        sal_memcpy(&tc->active_pid[i], scache_ptr,
                   sizeof(bcm_cosq_tas_profile_id_t));
        scache_ptr += sizeof(bcm_cosq_tas_profile_id_t);
        /* store process_pid */
        sal_memcpy(&tc->process_pid[i], scache_ptr,
                   sizeof(bcm_cosq_tas_profile_id_t));
        scache_ptr += sizeof(bcm_cosq_tas_profile_id_t);
    }
    TAS_UNLOCK(tc);
    return BCM_E_NONE;
}

/* The pending profile might become active during the warmboot
 * Update the profile state of the process_id and active_id.
 */
STATIC int
bcmi_esw_tas_wb_profile_update(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    tas_profile_ctrl_t  *tp_active = NULL, *tp_process = NULL;
    int i, config_idx;
    bcm_cosq_tas_profile_id_t active_pid, process_pid;
    int rv;

    BCM_IF_ERROR_RETURN(tas_control_get(unit, &tc));
    TAS_LOCK(tc);
    for (i = 0; i < tc->profile_list_count; i++) {
        if (!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), i)) {
            continue;
        }
        rv = bcmi_tas_profile_idx_get(unit, i, &config_idx);
        if (BCM_FAILURE(rv)) {
            TAS_UNLOCK(tc);
            return rv;
        }
        process_pid = tc->process_pid[i];
        if (process_pid) {
            rv = tas_profile_ctrl_get(unit, tc->process_pid[i], &tp_process);
            if (BCM_FAILURE(rv)) {
                TAS_UNLOCK(tc);
                return rv;
            }
            if (tp_process->hw_index != -1) {
                if (tp_process->hw_index != config_idx) {
                    /*
                     * hw_index != config_idx means the process pid
                     * become active.
                     * Note: Config change time can not be recovered since
                     *       it happened after leaving SDK and before WARMBOOT.
                     */
                    tp_process->pstate = bcmCosqTASProfileActive;

                    active_pid = tc->active_pid[i];
                    if (active_pid) {
                        rv = tas_profile_ctrl_get(unit, tc->active_pid[i],
                                                  &tp_active);
                        if (BCM_FAILURE(rv)) {
                            TAS_UNLOCK(tc);
                            return rv;
                        }
                        tp_active->pstate = bcmCosqTASProfileExpired;
                        tp_active->hw_index = -1;
                    }
                    tc->active_pid[i] = process_pid;
                    tc->process_pid[i] = 0;
                } else {
                    /*
                     * Clean up the process pid profile state which was
                     * committed but not schedule to HW yet.
                     */
                    tp_process->pstate = bcmCosqTASProfileError;
                    tp_process->hw_index = -1;
                    tc->process_pid[i] = 0;
                }
            }
        }
    }
    TAS_UNLOCK(tc);

    return BCM_E_NONE;
}

int
bcmi_esw_tas_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    int stable_size;
    uint8 *tas_scache;
    bcm_ptp_timestamp_t current_time;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* In case  WARM BOOT level 2 store is not configured return from here. */
    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ,
                              BCMI_COSQ_WB_SCACHE_PARTITION_TAS);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                    &tas_scache, BCMI_COSQ_WB_TAS_VERSION_1_0,
                                    NULL));
        BCM_IF_ERROR_RETURN(bcmi_esw_tas_recover(unit, tas_scache));
        BCM_IF_ERROR_RETURN(bcmi_esw_tas_wb_profile_update(unit));
    }

    if (soc_feature(unit, soc_feature_use_local_1588)) {
        /* Get currnet time to initiate the time interface id */
        (void)bcmi_tas_current_time(unit, &current_time);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     bcmi_esw_tas_init
 * Purpose:
 *     Internal function to initialize the software database of the tas module.
 *     Device specific functions should also be registered in this function.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_tas_init(int unit)
{
    bcmi_esw_tas_control_t  *tc = NULL;
    int rv = BCM_E_NONE;
    bcm_pbmp_t  all_pbmp;
    int count, port;
    const tas_drv_t   *td;

    /* clean up the resource */
    if (tas_control[unit] != NULL) {
        rv = bcmi_esw_tas_cleanup(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Allocate TAS control memeory for this unit. */
    TAS_ALLOC(unit, tc, bcmi_esw_tas_control_t,
              sizeof(bcmi_esw_tas_control_t),
              "TAS control", 0, rv);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Set up the unit TAS control structure. */
    tas_control[unit] = tc;

    /* Create protection mutex. */
    tc->tas_mutex = sal_mutex_create("tas_control.lock");
    if (NULL == tc->tas_mutex) {
        bcmi_esw_tas_cleanup(unit);
        return (BCM_E_INTERNAL);
    }

    /* Device specific information is required */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_tas_drv_init(unit, &tc->tas_drvs);
    } else
#endif
    {
        rv = BCM_E_UNAVAIL;
    }
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tas_cleanup(unit);
        return (rv);
    }

    count = 0;
    BCM_PBMP_CLEAR(all_pbmp);
    /* Get the all valid port map */
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
    /* Iterate the bitmap to get the maximum port number */
    BCM_PBMP_ITER(all_pbmp, port) {
        count = port;
    }
    /* Total port counts should be (maximum port number + 1) */
    count++;
    /* port basis profile list */
    tc->profile_list_count = count;
    /* Allocate tc->profile_list */
    TAS_ALLOC(unit, tc->profile_list, tas_profile_ctrl_t *,
              count * sizeof(tas_profile_ctrl_t *),
              "tas profiles set", 0, rv);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tas_cleanup(unit);
        return (BCM_E_MEMORY);
    }
    /* Allocate tc->active_pid */
    TAS_ALLOC(unit, tc->active_pid, bcm_cosq_tas_profile_id_t,
              count * sizeof(bcm_cosq_tas_profile_id_t),
              "tas current pid set", 0, rv);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tas_cleanup(unit);
        return (BCM_E_MEMORY);
    }
    /* Allocate tc->process_pid */
    TAS_ALLOC(unit, tc->process_pid, bcm_cosq_tas_profile_id_t,
              count * sizeof(bcm_cosq_tas_profile_id_t),
              "tas current pid set", 0, rv);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tas_cleanup(unit);
        return (BCM_E_MEMORY);
    }
    /* Allocate property software cache */
    TAS_ALLOC(unit, tc->tas_properties,
              tas_prop_info_t,
              sizeof(tas_prop_info_t) * (tc->profile_list_count + 1),
              "TAS properties cache ", 0, rv);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tas_cleanup(unit);
        return (BCM_E_MEMORY);
    }
    /* Fill up the cache */
    rv = bcmi_esw_tas_property_cache_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tas_cleanup(unit);
        return rv;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        bcmi_esw_tas_scache_alloc(unit);
    }
    if (SOC_WARM_BOOT(unit)) {
        rv = bcmi_esw_tas_reinit(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tas_cleanup(unit);
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    td = tc->tas_drvs;
    /* Allocate/init profile resource and register the profile event callback */
    if (td->tas_profile_init != NULL) {
        rv = td->tas_profile_init(unit, bcmi_tas_profile_event_notify);
    }
    return rv;
}
#endif /* BCM_TAS_SUPPORT */
