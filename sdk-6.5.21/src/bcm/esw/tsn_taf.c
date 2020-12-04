/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 * Manage common functionality for TSN TAF implementation.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tsn.h>
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/tsn_taf.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_WARM_BOOT_SUPPORT)
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
typedef struct bcmi_tsn_taf_control_s {
    sal_mutex_t taf_mutex;
    const bcmi_tsn_taf_dev_info_t *taf_dev_info;
} bcmi_tsn_taf_control_t;

/* TSN stat control */
STATIC bcmi_tsn_taf_control_t *taf_control[BCM_MAX_NUM_UNITS];

/* TSN calendar profile */
STATIC bcmi_tsn_taf_profile_t \
    *taf_profile_pending[SOC_MAX_NUM_DEVICES][BCMI_TSN_TAF_GATE_NUM_MAX];
STATIC bcmi_tsn_taf_profile_t \
    *taf_profile_active[SOC_MAX_NUM_DEVICES][BCMI_TSN_TAF_GATE_NUM_MAX];

/* helper macro */
#define TSN_TAF_LOCK(tc) \
            sal_mutex_take((tc)->taf_mutex, sal_mutex_FOREVER)
#define TSN_TAF_UNLOCK(tc) \
            sal_mutex_give((tc)->taf_mutex)

#define TSN_TAF_INIT_ERROR_WITH_CLEAN(_op_)             \
        do {                                            \
            int _rv_ = (_op_);                          \
            if (BCM_FAILURE(_rv_)) {                    \
                bcmi_esw_tsn_taf_cleanup(unit);         \
                return (_rv_);                          \
            }                                           \
        } while (0)

/* helper macro to execute dispatched specific device function */
#define TSN_TAF_FUNC(_name_, _args_)                  \
        ((NULL == taf_control[unit]->taf_dev_info->   \
          tsn_taf_##_name_) ? BCM_E_INIT :            \
         (taf_control[unit]->taf_dev_info->           \
          tsn_taf_##_name_)_args_)

#define TSN_TAF_DATA(_name_)                          \
         (taf_control[unit]->taf_dev_info->           \
          tsn_taf_##_name_)

/*
 * Function:
 *    bcmi_esw_tsn_taf_instance_get
 * Description:
 *    Helper function to retrieve TAF control pointer
 * Parameters:
 *    unit  - (IN) BCM device number
 *    tc   - (OUT) TAF control data structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_instance_get(
    int unit,
    bcmi_tsn_taf_control_t **tc)
{
    if (NULL == tc) {
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_tsn_taf)) {
        return BCM_E_UNAVAIL;
    }

    if (NULL == taf_control[unit]) {
        return BCM_E_INIT;
    }

    *tc = taf_control[unit];
    return BCM_E_NONE;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *    bcm_esw_tsn_taf_gate_create
 * Description:
 *    Allocate TAF gate
 * Parameters:
 *    unit          - (IN) Unit number
 *    flags         - (IN) flags, BCM_TSN_TAF_XXXX
 *    taf_gate_id   - (OUT) taf gate
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_create(int unit, int flags, int *taf_gate_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_create, (unit, flags, taf_gate_id));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_taf_gate_traverse
 * Description:
 *    Allocate TAF gate
 * Parameters:
 *    unit          - (IN) Unit number
 *     cb           - (IN) A pointer to callback function to call
 *                         for each taf gate.
 *     user_data    - (IN) Pointer to user data to supply in the callback
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_traverse(
    int unit,
    bcm_tsn_taf_gate_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_traverse, (unit, cb, user_data));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_taf_gate_destroy
 * Description:
 *    Free TAF gate
 * Parameters:
 *    unit          - (IN) Unit number
 *    taf_gate_id   - (IN) taf gate
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_destroy(int unit, int taf_gate_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_destroy, (unit, taf_gate_id));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_taf_status_get
 * Description:
 *    Get the TAF gate status
 * Parameters:
 *    unit          - (IN) Unit number
 *    taf_gate_id   - (IN) taf gate
 *    type          - (IN) status type
 *    arg           - (OUT)parameter
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_taf_status_get(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_status_t type,
    uint32 *arg)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(status_get, (unit, taf_gate_id, type, arg));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_port_control_set
 * Description:
 *    Set various TAF configurations at specific port
 * Parameters:
 *    unit          - (IN) Unit number
 *    gport         - (IN) gport
 *    type          - (IN) control type
 *    arg           - (IN) parameter
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_taf_port_control_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_control_t type,
    uint32 arg)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(port_control_set, (unit, gport, type, arg));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_port_control_get
 * Description:
 *    Get various TAF configurations at specific port
 * Parameters:
 *    unit          - (IN) Unit number
 *    gport         - (IN) gport
 *    type          - (IN) control type
 *    arg           - (OUT) parameter
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_taf_port_control_get(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_control_t type,
    uint32 *arg)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(port_control_get, (unit, gport, type, arg));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_pri_map_valid
 * Purpose:
 *      helper function to check config valid or not
 * Parameters:
 *      config - (IN) Priority Map configuration
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
bcmi_esw_tsn_taf_gate_pri_map_valid(
    int unit,
    bcm_tsn_pri_map_config_t *config)
{
    int i;
    int no_assign_any_gate = 1;

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    if (config->map_type != bcmTsnPriMapTypeTafGate) {
        return BCM_E_PARAM;
    }

    if (config->num_map_entries != TSN_TAF_DATA(gate_intpri_num)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < config->num_map_entries; i++) {
        uint32 valid_flags;

        valid_flags = BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS |
                      BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT;
        if (config->map_entries[i].flags & (~valid_flags)) {
            return BCM_E_PARAM;
        }
        if (config->map_entries[i].flags &
            BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS) {
            no_assign_any_gate = 0;
            break;
        }
        if (config->map_entries[i].flags &
            BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT) {
            no_assign_any_gate = 0;
            break;
        }
    }

    if (1 == no_assign_any_gate) {
        return BCM_E_PARAM;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_pri_map_set
 * Purpose:
 *      Set the Priority Map configuration to hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 *      config - (OUT) Priority Map configuration
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_gate_pri_map_set(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_taf_control_t *tc;
    int i;

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_gate_pri_map_valid(unit, config));

    TSN_TAF_LOCK(tc);
    /* check all taf_gate id has been allocated first */
    for (i = 0; i < config->num_map_entries; i++) {

        if (config->map_entries[i].flags &
            BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS) {
            rv = TSN_TAF_FUNC(gate_check,
                    (unit, config->map_entries[i].taf_gate_express));
            if (BCM_FAILURE(rv)) {
                break;
            }
        }

        if (config->map_entries[i].flags &
            BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT) {
            rv = TSN_TAF_FUNC(gate_check,
                    (unit, config->map_entries[i].taf_gate_preempt));
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    /* set priority map */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_pri_map_set, (unit, sw_idx, config));
    }
    TSN_TAF_UNLOCK(tc);

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_pri_map_get
 * Purpose:
 *      Get the Priority Map configuration to hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 *      config - (OUT) Priority Map configuration
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_gate_pri_map_get(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_pri_map_get, (unit, sw_idx, config));
    TSN_TAF_UNLOCK(tc);

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit   - (IN) Unit being initialized
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_esw_tsn_taf_gate_pri_map_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_taf_control_t *tc;
    bcm_tsn_pri_map_config_t config;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_pri_map_get, (unit, sw_idx, &config));
    TSN_TAF_UNLOCK(tc);

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_tsn_taf_gate_pri_map_valid(unit, &config);
    }
    return rv;
}


/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_pri_map_delete
 * Purpose:
 *      Delete the Priority Map configuration on hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_gate_pri_map_delete(
    int unit,
    uint32 sw_idx)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_pri_map_delete, (unit, sw_idx));
    TSN_TAF_UNLOCK(tc);

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_pri_map_hw_idx_get
 * Purpose:
 *      Get HW memory base index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_index - (OUT) HW memory base index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_gate_pri_map_hw_idx_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_idx)
{
    int rv = BCM_E_UNAVAIL;

    /* simple sw/hw index conversion need not be protected by TAF lock
     * invoke device-dispatched function directly
     */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
       rv = bcmi_gh2_taf_gate_pri_map_hw_idx_get(unit, sw_idx, hw_idx);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_pri_map_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_index - (IN) HW memory base index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_gate_pri_map_sw_idx_get(
    int unit,
    uint32 hw_idx,
    uint32 *sw_idx)
{
    int rv = BCM_E_UNAVAIL;

    /* simple sw/hw index conversion need not be protected by TAF lock
     * invoke device-dispatched function directly
     */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
       rv = bcmi_gh2_taf_gate_pri_map_sw_idx_get(unit, hw_idx, sw_idx);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    return rv;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_create
 * Purpose:
 *     Create a profile by specifying the profile settings including calendar
 *     table, ptp time information for PTP mode.
 *     A profile id will be assigned along with the settings.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     profile          - (IN) Pointer to profile structure which specifies
 *                             entries in calendar table, ptp time information
 *                             for PTP mode
 *     pid              - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t *profile,
    bcm_tsn_taf_profile_id_t *pid)
{
    int rv = BCM_E_UNAVAIL;

 #if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    bcmi_tsn_taf_profile_t *profile_i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);

    /* parameter check */
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 calendar_param_check, (unit, taf_gate, profile));
    }

    /* create a internal profile with a new pid */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 calendar_profile_create, (unit, profile, pid, &profile_i));
    }

    /* increase the ref_cnt for maxbyte and CosQ profile
     * which are specified by the user
     */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 calendar_resource_alloc, (unit, taf_gate, profile));
    }

    /* copy the user data into internal profile database
     * , and set the status as "Init"
     */
    if (BCM_SUCCESS(rv)) {
        profile_i->taf_gate = taf_gate;
        profile_i->status = bcmTsnTafProfileInit;
        COMPILER_64_SET(profile_i->config_change_time.seconds, 0, 0);
        profile_i->config_change_time.nanoseconds = 0;
        sal_memcpy(&(profile_i->data), profile, sizeof(bcm_tsn_taf_profile_t));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_set
 * Purpose:
 *     Modify the profile information
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     pid              - (IN) profile id
 *     profile          - (IN) pointer to profile structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t *profile)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    int schedule_exist;
    bcmi_tsn_taf_control_t *tc;
    bcmi_tsn_taf_profile_t *profile_i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (pid < 0 || pid >= BCMI_TSN_TAF_PROFILE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    TSN_TAF_LOCK(tc);

    /* For the new input, check the parameter */
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_param_check, (unit, taf_gate, profile));
    }

    /* Get the previous internal data */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_profile_get, (unit, pid, &profile_i));
    }

    if (BCM_SUCCESS(rv)) {
        if (profile_i->taf_gate != taf_gate) {
            rv = BCM_E_PARAM;
        }
    }

    /* cannot change if this profile has been operated in
     * Pending or Active status
     */
    if (BCM_SUCCESS(rv)) {
        if (profile_i->status == bcmTsnTafProfilePending ||
            profile_i->status == bcmTsnTafProfileActive) {
            rv = BCM_E_BUSY;
        }
    }

    /* if this profile has already be scheduled as Committed status
     *  ==> try to re-create a new schedule entry
     */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(schedule_exist, (unit, profile_i, &schedule_exist));
    }

    if (BCM_SUCCESS(rv) && 1 == schedule_exist &&
        bcmTsnTafProfileCommitted == profile_i->status) {
#if 1
        rv = TSN_TAF_FUNC(schedule_insert,
                          (unit, taf_gate,
                           &(profile->ptp_base_time),
                           profile_i));
#endif

    }

    /* increase the ref_cnt for the new maxbyte and CosQ profile id
     * which are specified by the user
     */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 calendar_resource_alloc, (unit, taf_gate, profile));
    }

    /* decrease the ref_cnt for the previous maxbyte and CosQ profile id */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 calendar_resource_free, (unit, taf_gate, &(profile_i->data)));
    }

    /* reset its status as Init */
    if (BCM_SUCCESS(rv)) {
        profile_i->status = bcmTsnTafProfileInit;
        sal_memcpy(&(profile_i->data), profile, sizeof(bcm_tsn_taf_profile_t));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_get
 * Purpose:
 *     Get the profile information by specifying the profile id.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     pid              - (IN) profile id
 *     profile          - (OUT) pointer to profile structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t *profile)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    bcmi_tsn_taf_profile_t *profile_i;

    if (NULL == profile) {
        return BCM_E_PARAM;
    }
    bcm_tsn_taf_profile_t_init(profile);

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (pid < 0 || pid >= BCMI_TSN_TAF_PROFILE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_profile_get, (unit, pid, &profile_i));
    }

    if (BCM_SUCCESS(rv)) {
        if (profile_i->taf_gate != taf_gate) {
            rv = BCM_E_PARAM;
        }
    }

    if (BCM_SUCCESS(rv)) {
        sal_memcpy(profile, &(profile_i->data), sizeof(bcm_tsn_taf_profile_t));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *     bcmi_esw_tsn_taf_profile_pick_pending
 * Purpose:
 *     helper function for ptp-mode
 *     pick a candidate from timeline to act in pending status
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     pid              - (IN) profile id
 *     status           - (OUT) pointer to profile status structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_profile_pick_pending(
    int unit,
    int taf_gate)
{
    bcmi_tsn_taf_profile_t *candidate = NULL;

    if (NULL != taf_profile_pending[unit][taf_gate]) {
        /* skip this if there has already existed a pending entry */
        return BCM_E_NONE;
    }

#if 1
    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(
            schedule_pick, (unit, taf_gate,
                            taf_profile_active[unit][taf_gate], &candidate)));
#endif



    if (NULL != candidate) {
        /* kick off this schedule according to the schedule time
         * if candidate existes
         */
        BCM_IF_ERROR_RETURN(
            TSN_TAF_FUNC(calendar_kickoff,
                         (unit, taf_gate, candidate)));

        /* record this schedule time as final config change time in database */
        BCM_IF_ERROR_RETURN(
            TSN_TAF_FUNC(schedule_get,
                         (unit, candidate, &(candidate->config_change_time))));

        /* remove schedule and change its status to pending */
        BCM_IF_ERROR_RETURN(
            TSN_TAF_FUNC(schedule_remove,
                         (unit, taf_gate, candidate)));
        candidate->status = bcmTsnTafProfilePending;

        /* udpate current pending profile */
        taf_profile_pending[unit][taf_gate] = candidate;
    }
    return BCM_E_NONE;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_commit
 * Purpose:
 *     Commit the profile to indicate the profile is ready to write to hardware.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     pid              - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    uint32 use_ptp;
    bcmi_tsn_taf_control_t *tc;
    bcmi_tsn_taf_profile_t *profile_i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (pid < 0 || pid >= BCMI_TSN_TAF_PROFILE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_profile_get, (unit, pid, &profile_i));
    }

    if (BCM_SUCCESS(rv)) {
        if (profile_i->taf_gate != taf_gate) {
            rv = BCM_E_PARAM;
        }
    }

    if (BCM_SUCCESS(rv)) {
        if (profile_i->status != bcmTsnTafProfileInit) {
            /* we only accept to commit the profile with init state */
            rv = BCM_E_CONFIG;
        }
    }

    if (BCM_SUCCESS(rv)) {
        uint32 taf_enable;

        rv = TSN_TAF_FUNC(gate_control_get,
                (unit, taf_gate, bcmTsnTafControlEnable, &taf_enable));
        if (BCM_SUCCESS(rv) && 0 == taf_enable) {
            /* need to enable TAF on this gate first before commit profile */
            rv = BCM_E_CONFIG;
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_control_get,
                (unit, taf_gate, bcmTsnTafControlUsePTPTime, &use_ptp));
    }

    if (BCM_SUCCESS(rv)) {
        if (1 == use_ptp) {
            uint32 is_locked;
            /* if this taf gate is operated in PTP mode,
             * create and insert a schedule entry into timeline
             * , set its state as commit then wait scheduling
             */


            /* if clock does not be locked, cannot commit */
            if (BCM_SUCCESS(rv)) {
                rv = TSN_TAF_FUNC(global_control_get,
                                  (unit, bcmTsnTafControlTAFPTPLock,
                                   &is_locked));
            }

            if (BCM_SUCCESS(rv) && 0 == is_locked) {
                LOG_WARN(BSL_LS_BCMAPI_TSN,
                         (BSL_META_U(unit, "need to ensure TAF ptp lock\
                                            before commit\n")));
                rv = BCM_E_CONFIG;
            }

            /* insert this new schedule */
            if (BCM_SUCCESS(rv)) {
#if 1
                rv = TSN_TAF_FUNC(
                        schedule_insert, (unit, taf_gate,
                                          &(profile_i->data.ptp_base_time),
                                          profile_i));
#endif

            }

            /* try to see if there is any profile could be selected
             * as pending status
             */
            if (BCM_SUCCESS(rv)) {
                profile_i->status = bcmTsnTafProfileCommitted;
                rv = bcmi_esw_tsn_taf_profile_pick_pending(
                        unit,
                        taf_gate);
            }


        } else {
            /* if this taf gate is operated in non-PTP mode,
             *  setup into hw directly, and enter pending state
             */
            if (NULL != taf_profile_pending[unit][taf_gate]) {
                rv = BCM_E_BUSY;
            }
            if (BCM_SUCCESS(rv)) {
                rv = TSN_TAF_FUNC(calendar_kickoff,
                        (unit, taf_gate, profile_i));
            }
            if (BCM_SUCCESS(rv)) {
                profile_i->status = bcmTsnTafProfilePending;
                taf_profile_pending[unit][taf_gate] = profile_i;
            }
        }
    }

    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *     bcmi_esw_tsn_taf_profile_destroy
 * Purpose:
 *     helper function to destroy profile
 */
STATIC int
bcmi_esw_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    void *user_data)
{
    bcmi_tsn_taf_profile_t *profile_i;
    int schedule_exist;

    if (pid < 0 || pid >= BCMI_TSN_TAF_PROFILE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(calendar_profile_get, (unit, pid, &profile_i)));

    if (profile_i->taf_gate != taf_gate) {
        return BCM_E_PARAM;
    }

    if (profile_i->status == bcmTsnTafProfilePending ||
        profile_i->status == bcmTsnTafProfileActive) {
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(schedule_exist, (unit, profile_i, &schedule_exist)));

    if (1 == schedule_exist) {
        BCM_IF_ERROR_RETURN(
            TSN_TAF_FUNC(schedule_remove, (unit, taf_gate, profile_i)));
    }

    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(calendar_resource_free,
                     (unit, taf_gate, &(profile_i->data))));

    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(calendar_profile_destroy, (unit, pid)));

    return BCM_E_NONE;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_destroy
 * Purpose:
 *     Destroy the profile and associated resources.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     pid              - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_tsn_taf_profile_destroy(unit, taf_gate, pid, NULL);
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}


/*
 * Function:
 *     bcm_esw_tsn_taf_profile_destroy_all
 * Purpose:
 *     Destroy all the profile associated with the taf_gate.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_destroy_all(
    int unit,
    int taf_gate)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_profile_traverse,
                (unit, taf_gate, bcmi_esw_tsn_taf_profile_destroy, NULL));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *     bcmi_esw_tsn_taf_profile_reset
 * Purpose:
 *     helper function to reset profile back to init status (is_init = 1)
 *                                   or fall into error status (is_init = 0)
 *     And need to cancel all schedule for commited profile
 *
 *                      is_init = 1     is_init = 0
 *      INIT                INIT            INIT
 *      ERROR               INIT            ERROR
 *      EXPIRED             INIT            EXPIRED
 *      COMMITTED           INIT            ERROR
 *      PENDING             INIT            ERROR
 *      ACTIVE              INIT            ERROR
 */
STATIC int
bcmi_esw_tsn_taf_profile_reset(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    void *user_data)
{
    bcmi_tsn_taf_profile_t *profile_i;
    int is_init;

    if (pid < 0 || pid >= BCMI_TSN_TAF_PROFILE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (NULL == user_data) {
        return BCM_E_INTERNAL; /* should not happen */
    }
    is_init = *((int *)user_data);

    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(calendar_profile_get, (unit, pid, &profile_i)));

    if (profile_i->taf_gate != taf_gate) {
        return BCM_E_PARAM;
    }

    switch (profile_i->status) {
        case bcmTsnTafProfileInit:
            /* skip profile with init status */
            break;
        case bcmTsnTafProfileExpired:
        case bcmTsnTafProfileError: /* fall through */
            if (1 == is_init) {
                profile_i->status = bcmTsnTafProfileInit;
            }
            break;
        case bcmTsnTafProfileCommitted:
            /* commit status only for ptp mode
             * so there should exist schedule entry
             */
             BCM_IF_ERROR_RETURN(
                 TSN_TAF_FUNC(schedule_remove, (unit, taf_gate, profile_i)));
             profile_i->status = (1 == is_init) ? bcmTsnTafProfileInit
                                                : bcmTsnTafProfileError;
             break;
        case bcmTsnTafProfilePending:
            if (taf_profile_pending[unit][taf_gate] != profile_i) {
                return BCM_E_INTERNAL;
            }
            taf_profile_pending[unit][taf_gate] = NULL;
            profile_i->status = (1 == is_init) ? bcmTsnTafProfileInit
                                               : bcmTsnTafProfileError;
            break;
        case bcmTsnTafProfileActive:
            if (taf_profile_active[unit][taf_gate] != profile_i) {
                return BCM_E_INTERNAL;
            }
            taf_profile_active[unit][taf_gate] = NULL;
            profile_i->status = (1 == is_init) ? bcmTsnTafProfileInit
                                               : bcmTsnTafProfileError;
            break;
        default:
            return BCM_E_INTERNAL; /* unknown status? */
    }

    return BCM_E_NONE;
}

/* control data for bcmi_esw_tsn_taf_profile_reset_all */
#define BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_NON_PTP (0)
#define BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_PTP     (1)
#define BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_BOTH    (2)

typedef struct bcmi_tsn_taf_profile_reset_control_s {
    int is_init; /* 1 : reset status back to init status
                  * 0 : fall into error status
                  */
    int active_mode; /* BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_XXX */
    int need_event; /* 1 : need raise event
                     * 0 : need not raise event
                     */
    bcm_tsn_taf_event_type_t event_type; /* event type if need_event = 1 */
} bcmi_tsn_taf_profile_reset_control_t;

/*
 * Function:
 *     bcmi_esw_tsn_taf_profile_reset_all
 * Purpose:
 *     Reset all the profile associated with the taf_gate.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     is_init          - (IN) 1 : reset status back to init status
 *                             0 : fall into error status
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_taf_profile_reset_all(
    int unit,
    int taf_gate,
    void *control_data)
{
    bcmi_tsn_taf_profile_reset_control_t *control_ptr;

    if (NULL == control_data) {
        return BCM_E_PARAM;
    }
    control_ptr = (bcmi_tsn_taf_profile_reset_control_t *)control_data;

    /* gate mode check, skip handling if not match */
    if (control_ptr->active_mode !=
        BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_BOTH) {

        uint32 use_ptp;

        BCM_IF_ERROR_RETURN(
            TSN_TAF_FUNC(gate_control_get,
                (unit, taf_gate, bcmTsnTafControlUsePTPTime, &use_ptp)));

        if (control_ptr->active_mode ==
            BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_NON_PTP &&
            1 == use_ptp) {
            return BCM_E_NONE;
        }

        if (control_ptr->active_mode ==
            BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_PTP &&
            0 == use_ptp) {
            return BCM_E_NONE;
        }
    }

    /* reset profile status */
    BCM_IF_ERROR_RETURN(
        TSN_TAF_FUNC(calendar_profile_traverse,
            (unit, taf_gate, bcmi_esw_tsn_taf_profile_reset,
             &(control_ptr->is_init))));

    /* raise event if need */
    if (1 == control_ptr->need_event) {
        BCM_IF_ERROR_RETURN(
            TSN_TAF_FUNC(event_notify,
                (unit, taf_gate, control_ptr->event_type)));
    }

    return BCM_E_NONE;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_traverse
 * Purpose:
 *     Traverse the set of profiles associated with the taf gate.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     cb               - (IN) A pointer to callback function to call
 *                             for each profile in the specified taf gate.
 *     user_data        - (IN) Pointer to user data to supply in the callback
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_traverse(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_profile_traverse, (unit, taf_gate, cb,
                                                      user_data));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_profile_status_get
 * Purpose:
 *     Get the profile status like profile state, config change time and
 *     entries in the hardware calendar table.
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate         - (IN) TAF gate
 *     pid              - (IN) profile id
 *     status           - (OUT) pointer to profile status structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t *status)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    bcmi_tsn_taf_profile_t *profile_i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (pid < 0 || pid >= BCMI_TSN_TAF_PROFILE_NUM_MAX || NULL == status) {
        return BCM_E_PARAM;
    }
    bcm_tsn_taf_profile_status_t_init(status);

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(calendar_profile_get, (unit, pid, &profile_i));
    }

    if (BCM_SUCCESS(rv)) {
        if (profile_i->taf_gate != taf_gate) {
            rv = BCM_E_PARAM;
        }
    }

    if (BCM_SUCCESS(rv)) {
        status->profile_state = profile_i->status;
        status->num_entries = profile_i->data.num_entries;
        COMPILER_64_COPY(status->config_change_time.seconds,
                         profile_i->config_change_time.seconds);
        status->config_change_time.nanoseconds =
            profile_i->config_change_time.nanoseconds;
        sal_memcpy(status->entries, profile_i->data.entries,
                   sizeof(bcm_tsn_taf_entry_t) *
                   BCM_TSN_TAF_CALENDAR_TABLE_SIZE);
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_taf_control_set
 * Description:
 *    Set various TAF configurations at specific gate
 * Parameters:
 *    unit          - (IN) Unit number
 *    taf_gate_id   - (IN) taf gate (-1 for global setting)
 *    type          - (IN) control type
 *    arg           - (IN) parameter
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_taf_control_set(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_control_t type,
    uint32 arg)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);

    if (bcmTsnTafControlTAFPTPLock == type) {
        /* PTP Lock is a global setting */
        if (taf_gate_id != -1) {
            rv = BCM_E_PARAM;
        } else {
            rv = TSN_TAF_FUNC(global_control_set, (unit, type, arg));
        }

        /* if ptp clock is out-of-snyc during ptp mode
         *  1. let all profile enter into error status
         *  2. raise out-of-sync event
         */
        if (BCM_SUCCESS(rv) && 0 == arg) {
            bcmi_tsn_taf_profile_reset_control_t control_data;

            sal_memset(&control_data, 0, sizeof(control_data));
            control_data.is_init = 0; /* reset to error status */
            control_data.active_mode =
                BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_PTP;
            control_data.need_event = 1;
            control_data.event_type = bcmTsnTafEventTAFPTPOutOfSyncErr;
            rv = TSN_TAF_FUNC(gate_traverse,
                    (unit, bcmi_esw_tsn_taf_profile_reset_all, &control_data));
        }
    } else {
        int need_to_reset = 0;

        /* For the other control type, check gate id first */
        rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

        if (BCM_SUCCESS(rv)) {
            if (type == bcmTsnTafControlEnable ||
                type == bcmTsnTafControlUsePTPTime) {

                uint32 prev_arg;

                rv = TSN_TAF_FUNC(
                         gate_control_get,
                            (unit, taf_gate_id, type, &prev_arg));

                if (BCM_SUCCESS(rv)) {
                    if (type == bcmTsnTafControlEnable) {
                        /* Reset all profiles after disable TAF */
                        if (prev_arg != arg && arg == 0) {
                            need_to_reset = 1;
                        }
                    } else if (type == bcmTsnTafControlUsePTPTime) {
                        /* If the user wants to change between ptp and
                         * non-ptp mode, need to disable TAF first
                         */
                        uint32 taf_enable;

                        rv = TSN_TAF_FUNC(
                                gate_control_get,
                                    (unit, taf_gate_id,
                                     bcmTsnTafControlEnable, &taf_enable));

                        if (BCM_SUCCESS(rv) && prev_arg != arg &&
                            1 == taf_enable) {
                            LOG_WARN(
                                BSL_LS_BCMAPI_TSN,
                                (BSL_META_U(unit, "need to disable TAF first\
                                                   before change PTP mode\n")));
                            rv = BCM_E_CONFIG;
                        }
                    }
                }
            }
        }

        if (BCM_SUCCESS(rv)) {
            rv = TSN_TAF_FUNC(gate_control_set, (unit, taf_gate_id, type, arg));
        }
        if (BCM_SUCCESS(rv) && 1 == need_to_reset) {
            bcmi_tsn_taf_profile_reset_control_t control_data;

            sal_memset(&control_data, 0, sizeof(control_data));
            control_data.is_init = 1; /* reset to init status */
            control_data.active_mode =
                BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_BOTH;
            control_data.need_event = 0;
            rv = bcmi_esw_tsn_taf_profile_reset_all(unit, taf_gate_id,
                                                    &control_data);
        }
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_taf_control_get
 * Description:
 *    Get various TAF configurations at specific gate
 * Parameters:
 *    unit          - (IN) Unit number
 *    taf_gate_id   - (IN) taf gate
 *    type          - (IN) control type
 *    arg           - (OUT)parameter
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_taf_control_get(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_control_t type,
    uint32 *arg)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    if (bcmTsnTafControlTAFPTPLock == type) {
        /* PTP Lock is a global setting */
        if (taf_gate_id != -1) {
            rv = BCM_E_PARAM;
        } else {
            rv = TSN_TAF_FUNC(global_control_get, (unit, type, arg));
        }
    } else {
        /* For the other control type, check gate id first */
        rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

        if (BCM_SUCCESS(rv)) {
            rv = TSN_TAF_FUNC(gate_control_get, (unit, taf_gate_id, type, arg));
        }
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

#if defined(BCM_TSN_SUPPORT)
STATIC int
bcmi_esw_tsn_taf_interrupt_handle(
    int unit
    )
{
    int rv = BCM_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    bcmi_tsn_taf_control_t *tc;
    int need_to_handle, taf_gate, start_handle;
    SHR_BITDCL gate_bitmap[_SHR_BITDCLSIZE(si->tsn_taf_gate_num)];

    /* Start to handle interrupt after all modules are inited
     * Consider that some interrupt signal are shared with others in init phase
     */
    if (soc == NULL || !(soc->soc_flags & SOC_F_ALL_MODULES_INITED)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));


    TSN_TAF_LOCK(tc);

    /* start to handle */
    start_handle = 0;
    rv = TSN_TAF_FUNC(interrupt_handle_start, (unit));

    /* handle profile switch done */
    if (BCM_SUCCESS(rv)) {
        start_handle = 1;
        rv = TSN_TAF_FUNC(
                interrupt_handle_switch,
                    (unit, &need_to_handle,
                     gate_bitmap, _SHR_BITDCLSIZE(si->tsn_taf_gate_num)));
    }

    if (BCM_SUCCESS(rv) && need_to_handle) {
        SHR_BIT_ITER(gate_bitmap, si->tsn_taf_gate_num, taf_gate) {
            uint32 use_ptp;
            bcmi_tsn_taf_profile_t *p_act = taf_profile_active[unit][taf_gate];
            bcmi_tsn_taf_profile_t *p_ped = taf_profile_pending[unit][taf_gate];

            if (NULL != p_act) {
                p_act->status = bcmTsnTafProfileExpired;
                taf_profile_active[unit][taf_gate] = NULL;
            }

            if (NULL == p_ped) {
                LOG_WARN(BSL_LS_BCMAPI_TSN,
                          (BSL_META_U(unit, "bcmi_esw_tsn_taf_interrupt_handle:\
                                             no pending profile ?\n")));
                rv = BCM_E_INTERNAL; /* there is no any pending profile ? */
                break;
            }
            p_ped->status = bcmTsnTafProfileActive;
            taf_profile_pending[unit][taf_gate] = NULL;
            taf_profile_active[unit][taf_gate] = p_ped;

            rv = TSN_TAF_FUNC(gate_control_get,
                (unit, taf_gate, bcmTsnTafControlUsePTPTime, &use_ptp));
            if (BCM_FAILURE(rv)) {
                break;
            }

            /* for ptp-mode, check whether to pick a new candidate profile
             * to be pending
             */
            if (1 == use_ptp) {
                rv = bcmi_esw_tsn_taf_profile_pick_pending(
                        unit,
                        taf_gate);
                if (BCM_FAILURE(rv)) {
                   break;
                }
            }
        }
    }

    /* handle error of active profile */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                interrupt_handle_error,
                    (unit, &need_to_handle,
                     gate_bitmap, _SHR_BITDCLSIZE(si->tsn_taf_gate_num)));
    }
    if (BCM_SUCCESS(rv) && need_to_handle) {
        bcmi_tsn_taf_profile_reset_control_t control_data;

        sal_memset(&control_data, 0, sizeof(control_data));
        control_data.is_init = 0; /* reset to error status */
        control_data.active_mode =
            BCMI_TSN_TAF_PROFILE_RESET_CONTROL_MODE_BOTH;
        control_data.need_event = 0;

        SHR_BIT_ITER(gate_bitmap, si->tsn_taf_gate_num, taf_gate) {
            rv = bcmi_esw_tsn_taf_profile_reset_all(unit, taf_gate,
                                                    &control_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    /* handle Tod Window event */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(interrupt_handle_tod, (unit, &need_to_handle));
    }
    if (BCM_SUCCESS(rv) && need_to_handle) {
        for (taf_gate = 0; taf_gate < si->tsn_taf_gate_num; taf_gate++) {
            uint32 use_ptp;

            rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));
            if (BCM_FAILURE(rv)) {
                rv = BCM_E_NONE;
                continue; /* skip invalid gate */
            }

            rv = TSN_TAF_FUNC(gate_control_get,
                    (unit, taf_gate, bcmTsnTafControlUsePTPTime, &use_ptp));
            if (BCM_FAILURE(rv)) {
               break;
            }

            if (0 == use_ptp) {
                continue; /* skip the gate operated in non-ptp mode */
            }

            /* check whether to pick a new candidate to pending status */
            rv = bcmi_esw_tsn_taf_profile_pick_pending(
                    unit,
                    taf_gate);
            if (BCM_FAILURE(rv)) {
               break;
            }
        }
    }

    /* invoke user registered callback */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(interrupt_handle_user_cb, (unit));
    }

    /* finish handling no matter rv is BCM_E_NONE or not */
    if (1 == start_handle) {
        (void)TSN_TAF_FUNC(interrupt_handle_finish, (unit));
    }

    TSN_TAF_UNLOCK(tc);
    return rv;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_taf_event_register
 * Purpose:
 *     The callback function registration by specifying port or traffic class to
 *     handle the TAF events.
 * Parameters:
 *     unit             - (IN) unit number
 *     event_types      - (IN) The set of TAF events for which the specified
 *                             callback should be invoked.
 *     taf_gate         - (IN) taf gate
 *     cb               - (IN) callback function
 *     user_data        - (IN) Pinter to user data to supply in the callback.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_event_register(
    int unit,
    bcm_tsn_taf_event_types_t event_types,
    int taf_gate,
    bcm_tsn_taf_event_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 event_register, (unit, taf_gate, event_types, cb, user_data));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_event_unregister
 * Purpose:
 *     The callback function unregistration of TAF events by specifying port or
 *     traffic class.
 * Parameters:
 *     unit             - (IN) unit number
 *     event_types      - (IN) The set of TAF events for which the specified
 *                             callback should be invoked.
 *     taf_gate         - (IN) taf gate
 *     cb               - (IN) callback function
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_event_unregister(
    int unit,
    bcm_tsn_taf_event_types_t event_types,
    int taf_gate,
    bcm_tsn_taf_event_cb cb)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                 event_unregister, (unit, taf_gate, event_types, cb));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_gate_max_bytes_profile_create
 * Purpose:
 *     Create profile of maximum bytes that pass through the TAF gate
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate_id      - (IN) taf gate
 *     max_bytes        - (IN) maximum bytes
 *     profile_id       - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_max_bytes_profile_create(
    int unit,
    int taf_gate_id,
    uint64 max_bytes,
    int *profile_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                gate_max_bytes_profile_create,
                    (unit, taf_gate_id, max_bytes, profile_id));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_gate_max_bytes_profile_set
 * Purpose:
 *     Set profile of maximum bytes that pass through the TAF gate
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate_id      - (IN) taf gate
 *     profile_id       - (IN) profile id
 *     max_bytes        - (IN) maximum bytes
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_max_bytes_profile_set(
    int unit,
    int taf_gate_id,
    int profile_id,
    uint64 max_bytes)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                gate_max_bytes_profile_set,
                    (unit, taf_gate_id, profile_id, max_bytes));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_gate_max_bytes_profile_get
 * Purpose:
 *     Get profile of maximum bytes that pass through the TAF gate
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate_id      - (IN) taf gate
 *     profile_id       - (IN) profile id
 *     max_bytes        - (OUT) maximum bytes
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_max_bytes_profile_get(
    int unit,
    int taf_gate_id,
    int profile_id,
    uint64 *max_bytes)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                gate_max_bytes_profile_get,
                    (unit, taf_gate_id, profile_id, max_bytes));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_gate_max_bytes_profile_destroy
 * Purpose:
 *     Destroy profile of maximum bytes that pass through the TAF gate
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate_id      - (IN) taf gate
 *     profile_id       - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_max_bytes_profile_destroy(
    int unit,
    int taf_gate_id,
    int profile_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                gate_max_bytes_profile_destroy,
                    (unit, taf_gate_id, profile_id));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_gate_max_bytes_profile_traverse
 * Purpose:
 *     Traverse profile of maximum bytes that pass through the TAF gate
 * Parameters:
 *     unit             - (IN) unit number
 *     taf_gate_id      - (IN) taf gate
 *     cb               - (IN) callback function
 *     user_data        - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_gate_max_bytes_profile_traverse(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_max_bytes_profile_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(
                gate_max_bytes_profile_traverse,
                    (unit, taf_gate_id, cb, user_data));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_cosq_mapping_profile_create
 * Purpose:
 *     Create TAF Cos mapping profile
 * Parameters:
 *     unit             - (IN) unit number
 *     cosq_profile     - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_cosq_mapping_profile_create(
    int unit,
    int *cosq_profile)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(
             cosq_mapping_profile_create, (unit, cosq_profile));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_cosq_mapping_profile_set
 * Purpose:
 *     Set TAF Cos mapping profile
 * Parameters:
 *     unit             - (IN) unit number
 *     cosq_profile     - (IN) profile id
 *     priority         - (IN) internal priority
 *     cosq             - (IN) CosQ id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_cosq_mapping_profile_set(
    int unit,
    int cosq_profile,
    bcm_cos_t priority,
    bcm_cos_queue_t cosq)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(
             cosq_mapping_profile_set, (unit, cosq_profile, priority, cosq));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_cosq_mapping_profile_get
 * Purpose:
 *     Get TAF Cos mapping profile
 * Parameters:
 *     unit             - (IN) unit number
 *     cosq_profile     - (IN) profile id
 *     priority         - (IN) internal priority
 *     cosq             - (OUT)CosQ id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_cosq_mapping_profile_get(
    int unit,
    int cosq_profile,
    bcm_cos_t priority,
    bcm_cos_queue_t *cosq)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(
             cosq_mapping_profile_get, (unit, cosq_profile, priority, cosq));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_cosq_mapping_profile_destroy
 * Purpose:
 *     Destroy TAF Cos mapping profile
 * Parameters:
 *     unit             - (IN) unit number
 *     cosq_profile     - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_cosq_mapping_profile_destroy(
    int unit,
    int cosq_profile)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(
             cosq_mapping_profile_destroy, (unit, cosq_profile));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_taf_cosq_mapping_profile_traverse
 * Purpose:
 *     Traverse TAF Cos mapping profile
 * Parameters:
 *     unit             - (IN) unit number
 *     cb               - (IN) callback function
 *     user_data        - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_taf_cosq_mapping_profile_traverse(
    int unit,
    bcm_tsn_taf_cosq_mapping_profile_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(
             cosq_mapping_profile_traverse, (unit, cb, user_data));
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_set
 * Description:
 *      Set 64-bit counter value for specified TAF gate statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      stat             - (IN) TAF gate stat types
 *      val              - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_set(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat,
    uint64 val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, 1, &stat, &val,
                           bcmiTsnStatCounterActionWrite));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_set32
 * Description:
 *      Set lower 32-bit counter value for specified TAF gate statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      stat             - (IN) TAF gate stat types
 *      val              - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_set32(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat,
    uint32 val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    uint64 value64;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    COMPILER_64_SET(value64, 0, val);

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, 1, &stat, &value64,
                           bcmiTsnStatCounterActionWrite));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_get
 * Description:
 *      Get 64-bit counter value for specified TAF gate statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      stat             - (IN) TAF gate stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_get(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, 1, &stat, val,
                           bcmiTsnStatCounterActionRead));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_get32
 * Description:
 *      Get lower 32-bit counter value for specified TAF gate statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      stat             - (IN) TAF gate stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_get32(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    uint64 value64;

    if (NULL == val) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, 1, &stat, &value64,
                           bcmiTsnStatCounterActionRead));
    }
    TSN_TAF_UNLOCK(tc);

    if (BCM_SUCCESS(rv)) {
        *val = COMPILER_64_LO(value64);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_multi_set
 * Description:
 *      Set 64-bit counter value for multiple TAF gate statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      nstat            - (IN) number of element in stat_arr and value_arr
 *      stat_arr         - (IN) TAF gate stat types
 *      value_arr        - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_multi_set(
    int unit,
    int taf_gate_id,
    int nstat,
    bcm_tsn_taf_gate_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    if (nstat < 0 || nstat > bcmTsnTafGateStatCount) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, nstat, stat_arr, value_arr,
                           bcmiTsnStatCounterActionWrite));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_multi_set32
 * Description:
 *      Set lower 32-bit counter value for multiple TAF gate statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      nstat            - (IN) number of element in stat_arr and value_arr
 *      stat_arr         - (IN) TAF gate stat types
 *      value_arr        - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_multi_set32(
    int unit,
    int taf_gate_id,
    int nstat,
    bcm_tsn_taf_gate_stat_t *stat_arr,
    uint32 *value_arr)
{

    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    uint64 *value64_arr = NULL;
    int i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (NULL == value_arr) {
        return BCM_E_PARAM;
    }

    if (nstat < 0 || nstat > bcmTsnTafGateStatCount) {
        return BCM_E_PARAM;
    }

    value64_arr = sal_alloc(nstat * sizeof(uint64), "value64_arr");
    if (NULL == value64_arr) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < nstat ; i++) {
        COMPILER_64_SET(value64_arr[i], 0, value_arr[i]);
    }

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, nstat, stat_arr, value64_arr,
                           bcmiTsnStatCounterActionWrite));
    }
    TSN_TAF_UNLOCK(tc);
    sal_free(value64_arr);
#endif /* BCM_TSN_SUPPORT */

    return rv;

}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_multi_get
 * Description:
 *      Get 64-bit counter value for multiple TAF gate statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TAF gate stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_multi_get(
    int unit,
    int taf_gate_id,
    int nstat,
    bcm_tsn_taf_gate_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv = BCM_E_UNAVAIL;

 #if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    if (nstat < 0 || nstat > bcmTsnTafGateStatCount) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, nstat, stat_arr, value_arr,
                           bcmiTsnStatCounterActionRead));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_multi_get32
 * Description:
 *      Get lower 32-bit counter value for multiple TAF gate statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TAF gate stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_multi_get32(
    int unit,
    int taf_gate_id,
    int nstat,
    bcm_tsn_taf_gate_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    uint64 *value64_arr = NULL;
    int i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (NULL == value_arr) {
        return BCM_E_PARAM;
    }

    if (nstat < 0 || nstat > bcmTsnTafGateStatCount) {
        return BCM_E_PARAM;
    }

    value64_arr = sal_alloc(nstat * sizeof(uint64), "value64_arr");
    if (NULL == value64_arr) {
        return BCM_E_MEMORY;
    }

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, nstat, stat_arr, value64_arr,
                           bcmiTsnStatCounterActionRead));
    }
    TSN_TAF_UNLOCK(tc);

    if (BCM_SUCCESS(rv)) {
        for (i = 0; i < nstat ; i++) {
            value_arr[i] = COMPILER_64_LO(value64_arr[i]);
        }
    }
    sal_free(value64_arr);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_sync_get
 * Description:
 *      Get the specified hardware statistics (64-bit) from the device.
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      stat             - (IN) TAF gate stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_sync_get(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, 1, &stat, val,
                           bcmiTsnStatCounterActionReadSync));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_sync_get32
 * Description:
 *      Get the specified hardware statistics (32-bit) from the device.
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      stat             - (IN) TAF gate stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_sync_get32(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    uint64 value64;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (NULL == val) {
        return BCM_E_PARAM;
    }

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, 1, &stat, &value64,
                           bcmiTsnStatCounterActionReadSync));
    }
    TSN_TAF_UNLOCK(tc);


    if (BCM_SUCCESS(rv)) {
        *val = COMPILER_64_LO(value64);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_sync_multi_get
 * Description:
 *      Get multiple hardware statistics (64-bit) from the device
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TAF gate stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_sync_multi_get(
    int unit,
    int taf_gate_id,
    int nstat,
    bcm_tsn_taf_gate_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;

    if (nstat < 0 || nstat > bcmTsnTafGateStatCount) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, nstat, stat_arr, value_arr,
                           bcmiTsnStatCounterActionReadSync));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_taf_gate_stat_sync_multi_get32
 * Description:
 *      Get multiple hardware statistics (32-bit) from the device
 * Parameters:
 *      unit             - (IN) unit number
 *      taf_gate_id      - (IN) TAF gate id
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TAF gate stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_taf_gate_stat_sync_multi_get32(
    int unit,
    int taf_gate_id,
    int nstat,
    bcm_tsn_taf_gate_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    uint64 *value64_arr = NULL;
    int i;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    if (NULL == value_arr) {
        return BCM_E_PARAM;
    }

    if (nstat < 0 || nstat > bcmTsnTafGateStatCount) {
        return BCM_E_PARAM;
    }

    value64_arr = sal_alloc(nstat * sizeof(uint64), "value64_arr");
    if (NULL == value64_arr) {
        return BCM_E_MEMORY;
    }

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(gate_check, (unit, taf_gate_id));

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(gate_stat_counter_access,
                          (unit, taf_gate_id, nstat, stat_arr, value64_arr,
                           bcmiTsnStatCounterActionReadSync));
    }
    TSN_TAF_UNLOCK(tc);

    if (BCM_SUCCESS(rv)) {
        for (i = 0; i < nstat ; i++) {
            value_arr[i] = COMPILER_64_LO(value64_arr[i]);
        }
    }
    sal_free(value64_arr);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *    bcmi_esw_tsn_taf_gate_stat_sync
 * Description:
 *    Callback function for counter collection on a specific gate
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
bcmi_esw_tsn_taf_gate_stat_sync(
    int unit,
    int taf_gate,
    void *user_data)
{
    return TSN_TAF_FUNC(gate_stat_counter_sync, (unit, taf_gate));
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_counter_cb
 * Description:
 *    Callback function for counter collection
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC void
bcmi_esw_tsn_taf_gate_stat_cb(int unit)
{
    bcmi_tsn_taf_control_t *tc;
    int rv;

    rv = bcmi_esw_tsn_taf_instance_get(unit, &tc);
    if (BCM_FAILURE(rv)) {
        return;
    }

    TSN_TAF_LOCK(tc);
    (void)TSN_TAF_FUNC(gate_traverse,
                       (unit, bcmi_esw_tsn_taf_gate_stat_sync, NULL));
    TSN_TAF_UNLOCK(tc);
}

/* TAF gate Priority Map driver set */
tsn_pri_map_info_t bcmi_taf_gate_pri_map_info = {
    "taf_gate_priority_map",                 /* table_name */
    0,                                       /* table_size,
                                              * init at bcmi_esw_tsn_taf_init */
    0,                                       /* entry_size,
                                              * init at bcmi_esw_tsn_taf_init */
    bcmi_esw_tsn_taf_gate_pri_map_set,       /* tsn_pri_map_set */
    bcmi_esw_tsn_taf_gate_pri_map_get,       /* tsn_pri_map_get */
    bcmi_esw_tsn_taf_gate_pri_map_delete,    /* tsn_pri_map_delete */
    bcmi_esw_tsn_taf_gate_pri_map_hw_idx_get, /* tsn_pri_map_hw_index_get */
    bcmi_esw_tsn_taf_gate_pri_map_sw_idx_get, /* tsn_pri_map_sw_idx_get */
    bcmi_esw_tsn_taf_gate_pri_map_wb_hw_existed  /* tsn_pri_map_wb_hw_existed */
};

#if defined(BCM_WARM_BOOT_SUPPORT)
int
bcmi_esw_tsn_taf_sync(int unit)
{
    int rv = BCM_E_UNAVAIL;

 #if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int scache_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_TAF);

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(get_scache_size, (unit, &scache_size));

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_scache_ptr_get(
                 unit, scache_handle, TRUE,
                 scache_size,
                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    }

    if (BCM_SUCCESS(rv)) {
        rv = TSN_TAF_FUNC(sync, (unit, scache_ptr));
    }
    TSN_TAF_UNLOCK(tc);
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

int
bcmi_esw_tsn_taf_reload(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    bcmi_tsn_taf_control_t *tc;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int scache_size;
    soc_info_t *si = &SOC_INFO(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_TAF);

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_taf_instance_get(unit, &tc));

    TSN_TAF_LOCK(tc);
    rv = TSN_TAF_FUNC(get_scache_size, (unit, &scache_size));

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_scache_ptr_get(
                 unit, scache_handle, FALSE,
                 scache_size,
                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    }

    if (BCM_E_NOT_FOUND == rv) {
        /* Maybe proceed with Level 1 Warm Boot */
        TSN_TAF_UNLOCK(tc);
        return BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {
        if (NULL == scache_ptr) {
            rv = BCM_E_INTERNAL;
        } else {
            rv = TSN_TAF_FUNC(reload, (unit, scache_ptr));
        }
    }

    if (BCM_SUCCESS(rv)) {
        /* after reloading in chip level successfully,
         * all internal profile has already been recoverd
         *  ==> start to recover taf_profile_pending/active in esw layer
         */
        int gate_id, profile_id;

        for (gate_id = 0; gate_id < si->tsn_taf_gate_num; gate_id++) {
            rv = TSN_TAF_FUNC(gate_check, (unit, gate_id));
            if (BCM_FAILURE(rv)) {
                rv = BCM_E_NONE;
                continue; /* skip invalid gate */
            }

            for (profile_id = 0; profile_id < BCMI_TSN_TAF_PROFILE_NUM_MAX;
                 profile_id++) {

                bcmi_tsn_taf_profile_t *profile_i;

                rv = TSN_TAF_FUNC(
                         calendar_profile_get, (unit, profile_id,
                                                &profile_i));
                if (BCM_FAILURE(rv)) {
                    rv = BCM_E_NONE;
                    continue; /* skip invalid profile id */
                } else if (profile_i->taf_gate != gate_id) {
                    continue; /* skip the profile not owned by this gate */
                }

                if (profile_i->status == bcmTsnTafProfilePending) {
                    taf_profile_pending[unit][gate_id] = profile_i;
                } else if (profile_i->status == bcmTsnTafProfileActive) {
                    taf_profile_active[unit][gate_id] = profile_i;
                }
            }
        }
    }

    TSN_TAF_UNLOCK(tc);

    /* register handler to handle interrupt after recover database */
    if (BCM_SUCCESS(rv)) {
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                rv = soc_fl_taf_event_handler_register(
                        unit, bcmi_esw_tsn_taf_interrupt_handle);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = soc_gh2_taf_event_handler_register(
                        unit, bcmi_esw_tsn_taf_interrupt_handle);
            }
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *    bcmi_esw_tsn_taf_cleanup
 * Description:
 *    Clean and free all allocated TAF resources
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
void
bcmi_esw_tsn_taf_cleanup(int unit)
{
    if (taf_control[unit] != NULL) {
        int i;
        soc_info_t *si = &SOC_INFO(unit);

        if (NULL != taf_control[unit]->taf_dev_info) {
            TSN_TAF_FUNC(cleanup, (unit));
        }

        if (NULL != taf_control[unit]->taf_mutex) {
            sal_mutex_destroy(taf_control[unit]->taf_mutex);
            taf_control[unit]->taf_mutex = NULL;
        }

        for (i = 0; i < si->tsn_taf_gate_num; i++) {
            taf_profile_pending[unit][i] = NULL;
            taf_profile_active[unit][i] = NULL;
        }

#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                soc_fl_taf_event_handler_register(unit, NULL);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                soc_gh2_taf_event_handler_register(unit, NULL);
            }
        }
#endif /* BCM_GREYHOUND2_SUPPORT */

        soc_counter_extra_unregister(unit, bcmi_esw_tsn_taf_gate_stat_cb);

        sal_free(taf_control[unit]);
        taf_control[unit] = NULL;
    }
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_init
 * Description:
 *    Initialize and allocate all required resources for TAF
 *    in both coldboot and warmboot
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_taf_init(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_taf_control_t *tc;

    /* Clean up the TAF related resources */
    if (taf_control[unit] != NULL) {
        bcmi_esw_tsn_taf_cleanup(unit);
    }

    /* Allocate TAF resource for this unit. */
    taf_control[unit] = sal_alloc(sizeof(bcmi_tsn_taf_control_t),
                                  "taf control");
    tc = taf_control[unit];
    if (NULL == tc) {
        TSN_TAF_INIT_ERROR_WITH_CLEAN(BCM_E_MEMORY);
    }
    sal_memset(tc, 0, sizeof(bcmi_tsn_taf_control_t));

    /* Get the chip bcmi_tsn_taf_dev_info_t structure. */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_taf_info_init(unit, &tc->taf_dev_info);
        if (BCM_SUCCESS(rv) && NULL == tc->taf_dev_info) {
            TSN_TAF_INIT_ERROR_WITH_CLEAN(BCM_E_INIT);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        TSN_TAF_INIT_ERROR_WITH_CLEAN(BCM_E_UNAVAIL);
    }

    tc->taf_mutex = sal_mutex_create("taf mutex");
    if (NULL == tc->taf_mutex) {
        TSN_TAF_INIT_ERROR_WITH_CLEAN(BCM_E_RESOURCE);
    }

    /* init functionality per-chip */
    TSN_TAF_INIT_ERROR_WITH_CLEAN(
        TSN_TAF_FUNC(init, (unit)));

    /* Register callback function for gate stat counter */
    TSN_TAF_INIT_ERROR_WITH_CLEAN(
        soc_counter_extra_register(unit, bcmi_esw_tsn_taf_gate_stat_cb));

    /* setup primap driver parameter per-chip */
    bcmi_taf_gate_pri_map_info.table_size = TSN_TAF_FUNC(gate_profile_num, (unit));
    bcmi_taf_gate_pri_map_info.entry_size = TSN_TAF_DATA(gate_intpri_num);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_taf_coldboot_init
 * Description:
 *    Initialize and allocate all required resources for TAF in coldboot
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_taf_coldboot_init(int unit)
{
    /* register handler to handle interrupt */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            TSN_TAF_INIT_ERROR_WITH_CLEAN(
                soc_fl_taf_event_handler_register(
                    unit, bcmi_esw_tsn_taf_interrupt_handle));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            TSN_TAF_INIT_ERROR_WITH_CLEAN(
                soc_gh2_taf_event_handler_register(
                    unit, bcmi_esw_tsn_taf_interrupt_handle));
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_TSN_SUPPORT */
