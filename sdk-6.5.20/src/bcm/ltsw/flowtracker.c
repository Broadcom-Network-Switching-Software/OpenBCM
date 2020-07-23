/*! \file flowtracker.c
 *
 * BCM level APIs for Flowtracker.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/flowtracker.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/flowtracker_int.h>
#include <bcm_int/ltsw/mbcm/flowtracker.h>

#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_FLOWTRACKER

/* Flowtracker database. */
typedef struct ltsw_flowtracker_info_s {
    /* Flowtracker module initialized flag. */
    int initialized;

    /* Flowtracker module lock. */
    sal_mutex_t lock;
} ltsw_flowtracker_info_t;

static ltsw_flowtracker_info_t ltsw_flowtracker_info[BCM_MAX_NUM_UNITS];

#define FLOWTRACKER_INFO(unit) (&ltsw_flowtracker_info[unit])

#define FLOWTRACKER_INIT_CHECK(unit) \
    do { \
        if (FLOWTRACKER_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define FLOWTRACKER_LOCK(unit) \
    do { \
        if (FLOWTRACKER_INFO(unit)->lock) { \
            sal_mutex_take(FLOWTRACKER_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define FLOWTRACKER_UNLOCK(unit) \
    do { \
        if (FLOWTRACKER_INFO(unit)->lock) { \
            sal_mutex_give(FLOWTRACKER_INFO(unit)->lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Free resources of Flowtracker module.
 *
 * Free Flowtracker module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static
int ltsw_flowtracker_resource_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_flowtracker_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief De-initialize the Flowtracker module.
 *
 * Free Flowtracker module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_detach(int unit)
{
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    if (FLOWTRACKER_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    FLOWTRACKER_LOCK(unit);
    lock = 1;

    FLOWTRACKER_INFO(unit)->initialized = 0;


    /* Call common detach routine */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_detach_common(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flowtracker_resource_free(unit));

exit:
    if (lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the Flowtracker module.
 *
 * Initialize Flowtracker database and Flowtracker related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_detach(unit));

    if (FLOWTRACKER_INFO(unit)->lock == NULL) {
        FLOWTRACKER_INFO(unit)->lock = sal_mutex_create("bcmLtswFlowtrackerMutex");
        SHR_NULL_CHECK(FLOWTRACKER_INFO(unit)->lock, SHR_E_MEMORY);
    }

    /* Call common init routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_init_common(unit));

    /* Call device specific init routine */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_flowtracker_init(unit));

    FLOWTRACKER_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        ltsw_flowtracker_resource_free(unit);

        if (FLOWTRACKER_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(FLOWTRACKER_INFO(unit)->lock);
            FLOWTRACKER_INFO(unit)->lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the match criteria for an elephant action.
 *
 * \param [in] unit Unit Number.
 * \param [in] action Match action.
 * \param [in] match_types Match types.
 * \param [in] match_data Match data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_elephant_action_match_set(
    int unit,
    bcm_flowtracker_elephant_match_action_t action,
    uint32_t match_types,
    bcm_flowtracker_elephant_match_data_t *match_data)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_action_match_set(unit, action,
                                                         match_types,
                                                         match_data));

exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the match criteria for an elephant action.
 *
 * \param [in] unit Unit Number.
 * \param [in] action Match action.
 * \param [out] match_types Match types.
 * \param [out] match_data Match data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_elephant_action_match_get(
    int unit,
    bcm_flowtracker_elephant_match_action_t action,
    uint32_t *match_types,
    bcm_flowtracker_elephant_match_data_t *match_data)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_action_match_get(unit, action,
                                                         match_types,
                                                         match_data));

exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the internal priority remap for elephant flows.
 *
 * \param [in] unit Unit Number.
 * \param [in] int_pri Internal priority.
 * \param [in] new_int_pri New internal priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_elephant_int_pri_remap_set(
    int unit,
    bcm_cos_t int_pri,
    bcm_cos_t new_int_pri)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_int_pri_remap_set(unit, int_pri,
                                                          new_int_pri));

exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the internal priority remap from elephant flows.
 *
 * \param [in] unit Unit Number.
 * \param [in] int_pri Internal priority.
 * \param [out] new_int_pri New internal priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_elephant_int_pri_remap_get(
    int unit,
    bcm_cos_t int_pri,
    bcm_cos_t *new_int_pri)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_int_pri_remap_get(unit, int_pri,
                                                          new_int_pri));

exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_hash_config_get
 * Purpose:
 *      Set the hashing configuration
 * Parameters:
 *      unit              - (IN) BCM device number
 *      hash_table        - (IN) Hash table
 *      instance_num      - (IN) Hash table instance number
 *      bank_num          - (IN) Hash table bank number
 *      hash_type         - (IN) Hash type
 *      right_rotate_bits - (IN) Number of bits the hash result should be
 *                               right rotated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_elephant_hash_config_get(
    int unit,
    bcm_flowtracker_elephant_hash_table_t hash_table,
    int instance_num,
    int bank_num,
    bcm_flowtracker_elephant_hash_type_t *hash_type,
    int *right_rotate_bits)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_hash_config_get(unit,
                                                        hash_table,
                                                        instance_num,
                                                        bank_num,
                                                        hash_type,
                                                        right_rotate_bits));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_hash_config_set
 * Purpose:
 *      Set the hashing configuration
 * Parameters:
 *      unit              - (IN) BCM device number
 *      hash_table        - (IN) Hash table
 *      instance_num      - (IN) Hash table instance number
 *      bank_num          - (IN) Hash table bank number
 *      hash_type         - (IN) Hash type
 *      right_rotate_bits - (IN) Number of bits the hash result should be
 *                               right rotated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_elephant_hash_config_set(
    int unit,
    bcm_flowtracker_elephant_hash_table_t hash_table,
    int instance_num,
    int bank_num,
    bcm_flowtracker_elephant_hash_type_t hash_type,
    int right_rotate_bits)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_hash_config_set(unit,
                                                        hash_table,
                                                        instance_num,
                                                        bank_num,
                                                        hash_type,
                                                        right_rotate_bits));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_control_get
 * Purpose:
 *      Get elephant controls
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      type        - (IN)  Control type
 *      arg         - (OUT) Control arg
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_elephant_control_get(
    int unit,
    bcm_flowtracker_elephant_control_t type,
    int *arg)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_control_get(unit,
                                                    type,
                                                    arg));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_control_set
 * Purpose:
 *      Set elephant controls
 * Parameters:
 *      unit        - (IN) BCM device number
 *      type        - (IN) Control type
 *      arg         - (IN) Control arg
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_elephant_control_set(
    int unit,
    bcm_flowtracker_elephant_control_t type,
    int arg)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_control_set(unit,
                                                    type,
                                                    arg));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the elephant counters.
 *
 * \param [in] unit Unit Number.
 * \param [out] stats Elephant statistics.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flowtracker_elephant_stats_sync_get(
    int unit,
    bcm_flowtracker_elephant_stats_t *stats)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_stats_sync_get(unit, stats));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_elephant_stats_get
 * Purpose:
 *      Get the elephant statistics
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      stats             - (OUT) Elephant statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_elephant_stats_get(
    int unit,
    bcm_flowtracker_elephant_stats_t *stats)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_stats_get(unit,
                                                  stats));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_elephant_stats_set
 * Purpose:
 *      Set the elephant statistics
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      stats             - (IN)  Elephant statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_elephant_stats_set(
    int unit,
    bcm_flowtracker_elephant_stats_t *stats)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flowtracker_elephant_stats_set(unit,
                                                  stats));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_create
 * Purpose:
 *      Create flowtracker group
 * Parameters:
 *      unit              - (IN)     BCM device number
 *      options           - (IN)     Group creation options
 *      flow_group_id     - (IN/OUT) Flowtracker group ID
 *      flow_group_info   - (IN)     Flowtracker group information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_t *flow_group_id,
    bcm_flowtracker_group_info_t *flow_group_info)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_create_common(unit,
                                                  options,
                                                  flow_group_id,
                                                  flow_group_info));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_actions_get
 * Purpose:
 *      Get list of actions applied on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      max_actions   - (IN) Maximum number of actions that can be
 *                           accomodated in the list.
 *      action_list   - (OUT) Action list.
 *      num_actions   - (OUT) Actual number of actions in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_actions_get(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int max_actions,
                               bcm_flowtracker_group_action_info_t *action_list,
                               int *num_actions)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_actions_get_common(
                                                  unit,
                                                  flow_group_id,
                                                  flags,
                                                  max_actions,
                                                  action_list,
                                                  num_actions));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_actions_set
 * Purpose:
 *      Set list of actions on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      num_actions   - (IN) Number of actions in the list.
 *      action_list   - (IN) Action list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_actions_set(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int num_actions,
                               bcm_flowtracker_group_action_info_t *action_list)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_actions_set_common(
                                                  unit,
                                                  flow_group_id,
                                                  flags,
                                                  num_actions,
                                                  action_list));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_age_timer_set
 * Purpose:
 *      Set aging timer interval in ms on the flow group. Aging interval has to
 *      be configured in steps of 1sec with a minimum of 1sec. Default value of
 *      1 minute.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      aging_interval_ms   - (IN) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_age_timer_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_age_timer_set_common(
                                                  unit,
                                                  id,
                                                  aging_interval_ms));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcm_ltsw_flowtracker_group_age_timer_get
 * Purpose:
 *      Get aging timer interval in ms set on the flow group.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      aging_interval_ms   - (OUT) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_age_timer_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_age_timer_get_common(
                                                  unit,
                                                  id,
                                                  aging_interval_ms));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_clear
 * Purpose:
 *      Clear a flow group's flow entries.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      flags         - (IN)  Clear params
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_clear(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_clear_common(
                                                  unit,
                                                  id,
                                                  flags));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_collector_attach_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_collector_attach_get_all(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_list_size,
        bcm_flowtracker_collector_t *list_of_collectors,
        int *list_of_export_profile_ids,
        bcm_flowtracker_export_template_t *list_of_templates,
        int *list_size)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_collector_attach_get_all_common
         (unit, flow_group_id, max_list_size, list_of_collectors,
          list_of_export_profile_ids, list_of_templates, list_size));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_data_get
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_data_get(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_flow_key_t *flow_key,
        bcm_flowtracker_flow_data_t *flow_data)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_data_get_common(
                                                  unit,
                                                  flow_group_id,
                                                  flow_key,
                                                  flow_data));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_destroy
 * Purpose:
 *      Destroy a flowtracker flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_destroy(
        int unit,
        bcm_flowtracker_group_t id)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_destroy_common(
                                                  unit,
                                                  id));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_export_trigger_set
 * Purpose:
 *  Set export trigger information of the flow group with ID
 *
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      export_trigger_info - (IN)  Export trigger information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_export_trigger_set_common(
                                                  unit,
                                                  id,
                                                  export_trigger_info));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_export_trigger_get
 * Purpose:
 *  Get export trigger information of the flow group with ID
 *
 * Parameters:
 *      unit                - (IN)   BCM device number
 *      id                  - (IN)   Flow group Id
 *      export_trigger_info - (OUT)  Export trigger information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_export_trigger_get_common(
                                                  unit,
                                                  id,
                                                  export_trigger_info));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_flow_count_get
 * Purpose:
 *      Get the number of flows learnt in the flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 *      flow_count       - (OUT) Number of flows learnt
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_count_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_count)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_flow_count_get_common(
                                                  unit,
                                                  id,
                                                  flow_count));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_flow_limit_set
 * Purpose:
 *      Set flow limit on the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (IN) Max number of flows that can be learnt on the group
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_limit_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_flow_limit_set_common(
                                                  unit,
                                                  id,
                                                  flow_limit));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_ft_flow_group_flow_limit_get
 * Purpose:
 *      Get flow limit of the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (OUT) Flow limit
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_limit_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_flow_limit_get_common(
                                                  unit,
                                                  id,
                                                  flow_limit));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_get
 * Purpose:
 *      Get flowtracker flow group information
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      id                - (IN)  Group Id
 *      flow_group_info   - (OUT) Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_get_common(
                                              unit,
                                              id,
                                              flow_group_info));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_get_all
 * Purpose:
 *      Get list of all flow groups created
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      max_size          - (IN)  Size of the flow group list array
 *      flow_group_list   - (OUT) List of flow groups created
 *      list_size         - (OUT) Number of flow grups in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_get_all(
        int unit,
        int max_size,
        bcm_flowtracker_group_t *flow_group_list,
        int *list_size)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_get_all_common(
                                              unit,
                                              max_size,
                                              flow_group_list,
                                              list_size));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_tracking_params_set
 * Purpose:
 *      Set tracking parameter for this flowtracker group
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Flow group Id
 *      num_tracking_params     - (IN)  Number of tracking params
 *      list_of_tracking_params - (IN)  Array of tracking param info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_tracking_params_set(int unit,
                                bcm_flowtracker_group_t id,
                                int num_tracking_params,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_tracking_params_set_common(
                                                  unit,
                                                  id,
                                                  num_tracking_params,
                                                  list_of_tracking_params));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_tracking_params_get
 * Purpose:
 *      Get tracking parameter for this flowtracker group
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Flow group Id
 *      max_size                - (IN)  Maximum number of tracking params
 *                                      that the array can hold
 *      list_of_tracking_params - (OUT) Array of tracking param info
 *      list_size               - (OUT) Actual number of tracking params filled
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_tracking_params_get(int unit,
                                bcm_flowtracker_group_t id,
                                int max_size,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params,
                                int *list_size)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_tracking_params_get_common(
                                                  unit,
                                                  id,
                                                  max_size,
                                                  list_of_tracking_params,
                                                  list_size));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_template_transmit_config_set
 * Purpose:
 *      Set the template set transmit configuration
 * Parameters:
 *      unit         - (IN) BCM device number
 *      template_id  - (IN) Template Id
 *      collector_id - (IN) Collector Id
 *      config       - (IN) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_template_transmit_config_set(
        int unit,
        bcm_flowtracker_export_template_t template_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_template_transmit_config_t *config)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_template_transmit_config_set_common(unit,
                                                             template_id,
                                                             collector_id,
                                                             config));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_template_transmit_config_get
 * Purpose:
 *      Get the template set transmit configuration
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      template_id  - (IN)  Template Id
 *      collector_id - (IN)  Collector Id
 *      config       - (OUT) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_template_transmit_config_get(
        int unit,
        bcm_flowtracker_export_template_t template_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_template_transmit_config_t *config)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_template_transmit_config_get_common(unit,
                                                             template_id,
                                                             collector_id,
                                                             config));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_elephant_profile_attach
 * Purpose:
 *       Attach a flow group with an elephant profile.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      profile_id    - (IN) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_elephant_profile_attach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t profile_id)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_elephant_profile_attach_common(unit,
                                                                   flow_group_id,
                                                                   profile_id));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_elephant_profile_attach_get
 * Purpose:
 *       Get the elephant profile Id attached with a flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      profile_id    - (OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_elephant_profile_attach_get(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t *profile_id)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_elephant_profile_attach_get_common(unit,
                                                                       flow_group_id,
                                                                       profile_id));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_elephant_profile_detach
 * Purpose:
 *       Detach a flow group with an elephant profile.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_elephant_profile_detach(int unit,
        bcm_flowtracker_group_t flow_group_id)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_elephant_profile_detach_common(unit, flow_group_id));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_collector_attach
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_collector_attach(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_collector_attach_common(unit,
                                                            flow_group_id,
                                                            collector_id,
                                                            export_profile_id,
                                                            template_id ));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_collector_detach
 * Purpose:
 *      Disassociate flow group from a collector and an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_collector_detach(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id)
{

    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_collector_detach_common
            (unit, flow_group_id, collector_id,
             export_profile_id, template_id));

exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_profile_create
 * Purpose:
 *      Create flowtracker elephant profile
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      options      - (IN)     Elephant profile creation options.
 *      profile      - (IN)     Elephant profile information
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_elephant_profile_create(
                                int unit,
                                uint32 options,
                                bcm_flowtracker_elephant_profile_info_t *profile,
                                bcm_flowtracker_elephant_profile_t *profile_id)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_elephant_profile_create_common
        (unit, options, profile, profile_id));

exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_profile_destroy
 * Purpose:
 *       Destroy a flowtracker elephant profile.
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_elephant_profile_destroy(
                                  int unit,
                                  bcm_flowtracker_elephant_profile_t profile_id)
{

    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_elephant_profile_destroy_common
            (unit, profile_id));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_elephant_profile_get
 * Purpose:
 *       Get flowtracker elephant profile information.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      profile_id   - (IN)  Elephant profile id
 *      profile      - (OUT) Elephant profile information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_elephant_profile_get(
                                int unit,
                                bcm_flowtracker_elephant_profile_t profile_id,
                                bcm_flowtracker_elephant_profile_info_t *profile)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_elephant_profile_get_common(unit, profile_id,
                                                          profile));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *      _bcm_th_ft_elephant_profile_get_all
 * Purpose:
 *       Get the list of all flowtracker elephant profiles configured.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      max           - (IN)  Max number of profile information that can be
 *                            accomodated within profile_list.
 *      profile_list  - (OUT) Elephant profile information
 *      count         - (OUT) Actual number of elephant profiles
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_elephant_profile_get_all(
                           int unit,
                           int max,
                           bcm_flowtracker_elephant_profile_t *profile_list,
                           int *count)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_elephant_profile_get_all_common(unit,
            max, profile_list, count));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_export_template_create
 * Purpose:
 *      Create an export template
 * Parameters:
 *      unit                    - (IN)    BCM device number
 *      options                 - (IN)    Template create options
 *      id                      - (INOUT) Template Id
 *      set_id                  - (IN)    set_id to be used for the template
 *      num_export_elements     - (IN)    Number of elements in the template
 *      list_of_export_elements - (IN)    Export element list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_export_template_create(
                  int unit,
                  uint32 options,
                  bcm_flowtracker_export_template_t *id,
                  uint16 set_id,
                  int num_export_elements,
                  bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_export_template_create_common(unit, options, id,
            set_id, num_export_elements, list_of_export_elements));

    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *      bcm_ltsw_flowtracker_export_template_destroy
 * Purpose:
 *      Destroy a flowtracker export template
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_export_template_destroy(int unit,
                                       bcm_flowtracker_export_template_t id)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_export_template_destroy_common(unit, id));

    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_export_template_get
 * Purpose:
 *      Get a flowtracker export template with ID.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 *      set_id                  - (OUT) Set Id of the template
 *      max_size                - (IN)  Size of the export element list array
 *      list_of_export_elements - (OUT) Export element list
 *      list_size               - (OUT) Number of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_export_template_get(
                  int unit,
                  bcm_flowtracker_export_template_t id,
                  uint16 *set_id,
                  int max_size,
                  bcm_flowtracker_export_element_info_t *list_of_export_elements,
                  int *list_size)
{
    int flowtracker_lock = 0;
    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_export_template_get_common(unit, id,
            set_id, max_size, list_of_export_elements, list_size));

    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_add
 * Purpose:
 *      Add a new static flow to the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      flow_key                - (IN)  Packet attributes identifying a flow.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_flow_key_t *flow_key)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_static_flow_add_common(
                                                  unit,
                                                  flow_group_id,
                                                  flow_key));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_delete
 * Purpose:
 *      Delete an existing static flow from the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      flow_key                - (IN)  Packet attributes identifying a flow.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_flow_key_t *flow_key)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_static_flow_delete_common(
                                                  unit,
                                                  flow_group_id,
                                                  flow_key));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_delete_all
 * Purpose:
 *      Delete all existing static flows from the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_delete_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_static_flow_delete_all_common(
                                                  unit,
                                                  flow_group_id));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_get_all
 * Purpose:
 *      Get all existing static flows from the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      max_size                - (IN)  Maximum number of flow keys that can
 *                                      be filled in the flow_key_arr array.
 *      flow_key_arr            - (OUT) Array of flow keys of static flows
 *                                      in the flow group
 *      list_size               - (OUT) Actual number of flow keys in the array.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_size,
    bcm_flowtracker_flow_key_t *flow_key_arr,
    int *list_size)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_static_flow_get_all_common(
                                                  unit,
                                                  flow_group_id,
                                                  max_size,
                                                  flow_key_arr,
                                                  list_size));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_flow_delete
 * Purpose:
 *      Delete a HW learnt flow.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for delete action.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_group_flow_action_info_t *action_info)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_flow_delete_common(
                                                  unit,
                                                  flow_group_id,
                                                  action_info));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_flow_action_update
 * Purpose:
 *      Update actions of a HW learnt flow.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for update action.
 *      num_actions             - (IN)  Number of actions to update
 *      action_list             - (IN)  List of actions to update.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_action_update(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_group_flow_action_info_t *action_info,
    int num_actions,
    bcm_flowtracker_group_action_info_t *action_list)
{
    int flowtracker_lock = 0;

    SHR_FUNC_ENTER(unit);

    FLOWTRACKER_INIT_CHECK(unit);
    FLOWTRACKER_LOCK(unit);

    flowtracker_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_flow_action_update_common(
                                                  unit,
                                                  flow_group_id,
                                                  action_info,
                                                  num_actions,
                                                  action_list));
exit:
    if (flowtracker_lock == 1) {
        FLOWTRACKER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
