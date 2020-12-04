/*! \file bcmcth_mon_flowtracker_learn_event_log.c
 *
 * BCMCTH MON_FLOWTRACKER_LEARN_EVENT_LOG LT utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bsl/bsl_enum.h>

#include <bcmltd/id/bcmltd_common_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>

#include <bcmcth/generated/flowtracker_ha.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_event_log.h>
/*******************************************************************************
 * Defines
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/* Macros for operating on current event log index */
#define FT_LEARN_EVENT_LOG_CURR_IDX(unit)\
 (ft_learn_evt_log_info[unit]->ft_learn_evt_log_curr_idx)

#define FT_LEARN_EVENT_LOG_CURR_IDX_INCR(unit, max_entries)\
    (FT_LEARN_EVENT_LOG_CURR_IDX(unit) = \
    (FT_LEARN_EVENT_LOG_CURR_IDX(unit) +1)%max_entries)

/*
 * Macros for taking and releasing mutex.
 * Ensure the calling function has a variable named rv.
 */
#define FT_LEARN_EVENT_LOG_INFO_MUTEX_TAKE(unit) \
    rv = sal_mutex_take(ft_learn_evt_log_info_mutex[unit],\
                        ft_learn_evt_log_info_mutex_timeout);\
    if (rv < 0) {\
        LOG_VERBOSE(BSL_LOG_MODULE,\
               (BSL_META_U(unit, "Unit%d:\
               %s mutex take failed\n"),\
                unit, __FUNCTION__));\
        SHR_ERR_EXIT(SHR_E_INTERNAL);\
    }

#define FT_LEARN_EVENT_LOG_INFO_MUTEX_GIVE(unit)\
    rv = sal_mutex_give(ft_learn_evt_log_info_mutex[unit]);\
    if (rv < 0) {\
        LOG_VERBOSE(BSL_LOG_MODULE,\
               (BSL_META_U(unit, "Unit%d:\
               %s mutex give failed\n"),\
                unit, __FUNCTION__));\
        SHR_ERR_EXIT(SHR_E_INTERNAL);\
    }
/*******************************************************************************
 * Global variables
 */
/* Mutex for protecting the event log info structure per unit */
static sal_mutex_t ft_learn_evt_log_info_mutex[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/* The event log information per unit. */
static bcmcth_mon_ft_learn_evt_log_info_t *ft_learn_evt_log_info[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/* The mutex timeout in usecs */
static const int ft_learn_evt_log_info_mutex_timeout = 1000000; /* 1 second */

/* The total number of MON_FLOWTRACKER_LEARN_EVENT_LOG LT entries */
static uint32_t ft_learn_event_log_num_entries = 0;

/******************************************************************************
* Private functions
 */
/*!
 * \brief Get the current entry index to update the event.
 *
 * \param [in]  unit Unit number.
 * \param [out] evt_log_idx current entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_mon_ft_learn_event_log_curr_idx_get(int unit, uint32_t *evt_log_idx)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    FT_LEARN_EVENT_LOG_INFO_MUTEX_TAKE(unit);

    *evt_log_idx = FT_LEARN_EVENT_LOG_CURR_IDX(unit);

    FT_LEARN_EVENT_LOG_INFO_MUTEX_GIVE(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Increment the current entry index and return it via evt_log_idx.
 *
 * \param [in]  unit Unit number.
 * \param [out] evt_log_idx current entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_mon_ft_learn_event_log_curr_idx_incr(int unit, uint32_t *evt_log_idx)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    FT_LEARN_EVENT_LOG_INFO_MUTEX_TAKE(unit);

    FT_LEARN_EVENT_LOG_CURR_IDX_INCR(unit, ft_learn_event_log_num_entries);
    *evt_log_idx = FT_LEARN_EVENT_LOG_CURR_IDX(unit);

    FT_LEARN_EVENT_LOG_INFO_MUTEX_GIVE(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the MON_FLOWTRACKER_EVENT_LOG_STATUS entry with the given
 *        entry information.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_data Entry information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_mon_ft_learn_event_log_status_entry_update(
                            int unit,
                            uint32_t evt_log_idx)
{
    uint32_t field_cnt = MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUSt_FIELD_COUNT;
    bcmltd_fields_t lt_flds = {0};
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &lt_flds));

    lt_flds.count = field_cnt;

    /* Data */
    lt_flds.field[0]->id =
    MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUSt_MON_FLOWTRACKER_LEARN_EVENT_LOG_IDf;
    lt_flds.field[0]->data = evt_log_idx;

    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_update(unit, true,
                MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUSt, &lt_flds));

exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &lt_flds);
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */
/*!
 * \brief Update the next entry with the given entry information.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_data Entry information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_ft_learn_event_log_entry_update(
                            int unit,
                            bcmcth_mon_flowtracker_event_log_entry_t *entry_data)
{
    uint32_t field_cnt = MON_FLOWTRACKER_LEARN_EVENT_LOGt_FIELD_COUNT;
    bcmltd_fields_t lt_flds = {0};
    uint32_t evt_log_idx = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &lt_flds));

    lt_flds.count = field_cnt;
    /* Key = EVENT_LOG_ID */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_mon_ft_learn_event_log_curr_idx_get(unit, &evt_log_idx));
    lt_flds.field[0]->id =
        MON_FLOWTRACKER_LEARN_EVENT_LOGt_MON_FLOWTRACKER_LEARN_EVENT_LOG_IDf;
    lt_flds.field[0]->data = evt_log_idx;

    /* Data */

    /* PIPE */
    lt_flds.field[1]->id = MON_FLOWTRACKER_LEARN_EVENT_LOGt_PIPEf;
    lt_flds.field[1]->data = entry_data->pipe;

    /* EXACT MATCH INDEX */
    lt_flds.field[2]->id =
        MON_FLOWTRACKER_LEARN_EVENT_LOGt_EXACT_MATCH_INDEXf;
    lt_flds.field[2]->data = entry_data->exact_match_idx;

    /* GROUP ID */
    lt_flds.field[3]->id =
        MON_FLOWTRACKER_LEARN_EVENT_LOGt_MON_FLOWTRACKER_GROUP_IDf;
    lt_flds.field[3]->data = entry_data->mon_flowtracker_group_id;

    /* MODE */
    lt_flds.field[4]->id = MON_FLOWTRACKER_LEARN_EVENT_LOGt_MODEf;
    lt_flds.field[4]->data = entry_data->mode;


    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_update(unit, true,
                MON_FLOWTRACKER_LEARN_EVENT_LOGt, &lt_flds));


    /*
     * Update the MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUS with the
     * current log index
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_learn_event_log_status_entry_update(
                                                            unit, evt_log_idx));
    /* Update is successful. Increment the curr event log index */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_learn_event_log_curr_idx_incr(unit,
                                                                &evt_log_idx));
exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &lt_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dummy insert function for MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUS entry since the
 *        bcmcth_mon_ft_learn_event_log_entry_update will only be doing update.
 *        NOTE: Should not be called during Warmboot.
 *        NOTE: Should be called during module init and not dynamically.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_ft_learn_event_log_status_dummy_entry_insert(int unit)
{
    uint32_t field_cnt = MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUSt_FIELD_COUNT;
    bcmltd_fields_t lt_flds = {0};
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &lt_flds));

    lt_flds.count = field_cnt;


    /* Data - Fill with default values */

    lt_flds.field[0]->id =
    MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUSt_MON_FLOWTRACKER_LEARN_EVENT_LOG_IDf;
    lt_flds.field[0]->data = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_entry_insert(unit, MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUSt,
                &lt_flds));

exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &lt_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dummy insert function for all entries since the interrupt handler
 *        will only be doing update.
 *        NOTE: Should not be called during Warmboot.
 *        NOTE: Should be called during module init and not dynamically.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_ft_learn_event_log_dummy_entries_insert(int unit)
{
    uint32_t field_cnt = MON_FLOWTRACKER_LEARN_EVENT_LOGt_FIELD_COUNT;
    bcmltd_fields_t lt_flds = {0};
    uint32_t i;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &lt_flds));

    lt_flds.count = field_cnt;

    for (i = 0; i < ft_learn_event_log_num_entries; i++) {
        /* Key = EVENT_LOG_ID */
        lt_flds.field[0]->id =
            MON_FLOWTRACKER_LEARN_EVENT_LOGt_MON_FLOWTRACKER_LEARN_EVENT_LOG_IDf;
        lt_flds.field[0]->data = i;

        /* Data - Fill with default values */

        /* PIPE */
        lt_flds.field[1]->id = MON_FLOWTRACKER_LEARN_EVENT_LOGt_PIPEf;
        lt_flds.field[1]->data = 0;

        /* EXACT MATCH INDEX */
        lt_flds.field[2]->id =
            MON_FLOWTRACKER_LEARN_EVENT_LOGt_EXACT_MATCH_INDEXf;
        lt_flds.field[2]->data = 0;

        /* GROUP ID */
        lt_flds.field[3]->id =
            MON_FLOWTRACKER_LEARN_EVENT_LOGt_MON_FLOWTRACKER_GROUP_IDf;
        lt_flds.field[3]->data = 0;

        /* MODE */
        lt_flds.field[4]->id = MON_FLOWTRACKER_LEARN_EVENT_LOGt_MODEf;
        lt_flds.field[4]->data =
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_SINGLE;


        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_entry_insert(unit, MON_FLOWTRACKER_LEARN_EVENT_LOGt,
                                &lt_flds));
    }

exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &lt_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the datastructures allocated for the event log library.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
void
bcmcth_mon_ft_learn_event_log_deinit(int unit)
{
    /* Only destroy the mutex */
    sal_mutex_destroy(ft_learn_evt_log_info_mutex[unit]);
    ft_learn_evt_log_info_mutex[unit] = NULL;
}

/*!
 * \brief Initialize the datastructures required for the event log library.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_ft_learn_event_log_init(int unit, bool warm)
{
    char *mutex_desc = "bcmCthMonFtEvtLogInfoMutex";
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    SHR_FUNC_ENTER(unit);

    /* Allocate event log info for the given unit from HA memory */
    ha_alloc_size = sizeof(bcmcth_mon_ft_learn_evt_log_info_t);
    ha_req_size = ha_alloc_size;
    ft_learn_evt_log_info[unit] = shr_ha_mem_alloc(
                unit,
                BCMMGMT_MON_COMP_ID,
                BCMMON_FLOWTRACKER_LEARN_EVENT_LOG_INFO_SUB_COMP_ID,
                "bcmcthMonFtLearnEvtLogInfo",
                &ha_alloc_size);
    SHR_NULL_CHECK(ft_learn_evt_log_info[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ft_learn_evt_log_info[unit], 0, ha_alloc_size);
        /* ISSU support */
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(
                unit, BCMMGMT_MON_COMP_ID,
                BCMMON_FLOWTRACKER_LEARN_EVENT_LOG_INFO_SUB_COMP_ID, 0,
                ha_req_size,
                1,
                BCMCTH_MON_FT_LEARN_EVT_LOG_INFO_T_ID));
    }

    /* Create mutex for the given unit to protect the event log info */
    ft_learn_evt_log_info_mutex[unit] = sal_mutex_create(mutex_desc);
    SHR_NULL_CHECK(ft_learn_evt_log_info_mutex[unit], SHR_E_MEMORY);

    /* Get the total number of entries in the LT */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlrd_map_table_attr_get(unit, MON_FLOWTRACKER_LEARN_EVENT_LOGt,
                                  BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                  &ft_learn_event_log_num_entries));

exit:
    if (SHR_FUNC_ERR()) {
        bcmcth_mon_ft_learn_event_log_deinit(unit);
    }
    SHR_FUNC_EXIT();
}
