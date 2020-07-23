/*! \file ifa.c
 *
 * IFA Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/ifa.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/ifa_uc.h>

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_IFA

typedef struct ltsw_ifa_info_s {
    /* IFA initialized flag. */
    int initialized;

    /*! IFA module lock. */
    sal_mutex_t ifa_lock;
} ltsw_ifa_info_t;

static ltsw_ifa_info_t ltsw_ifa_info[BCM_MAX_NUM_UNITS];
#define IFA_INFO(unit) (&ltsw_ifa_info[unit])

/*! Cause a routine to return SHR_E_INIT if IFA subsystem is not initialized. */
#define IFA_INIT_CHECK(unit) \
    do { \
        if (IFA_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0) \

/*! IFA mutex lock. */
#define IFA_LOCK(unit) \
    do { \
        if (IFA_INFO(unit)->ifa_lock) { \
            sal_mutex_take(IFA_INFO(unit)->ifa_lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        }\
    } while (0)

/*! IFA mutex unlock. */
#define IFA_UNLOCK(unit) \
    do { \
        if (IFA_INFO(unit)->ifa_lock) { \
            sal_mutex_give(IFA_INFO(unit)->ifa_lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

int
bcmint_ifa_lt_get(
    int unit,
    bcmint_ifa_lt_id_t lt_id,
    const bcmint_ifa_lt_t **lt_info)
{
    bcmint_ifa_lt_db_t lt_db;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ifa_lt_db_get(unit, &lt_db));

    if (!(lt_db.lt_bmp & (1 << lt_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *lt_info = &lt_db.lts[lt_id];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get IFA header.
 *
 * \param [in] unit Unit number.
 * \param [in] egr_ifa L3 egress IFA data.
 * \param [out] header IFA header.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmi_ltsw_ifa_header_from_l3_egr(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    bcm_ifa_header_t *header)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(egr_ifa, SHR_E_PARAM);
    SHR_NULL_CHECK(header, SHR_E_PARAM);

    header->ip_protocol = egr_ifa->ip_protocol;
    header->max_length = egr_ifa->max_length;
    header->hop_limit = egr_ifa->hop_limit;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup IFA module software database.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
ltsw_ifa_info_cleanup(int unit)
{
    if (ltsw_ifa_info[unit].ifa_lock != NULL) {
        sal_mutex_destroy(ltsw_ifa_info[unit].ifa_lock);
        ltsw_ifa_info[unit].ifa_lock = NULL;
    }
}

/*!
 * \brief Initialize IFA module software database.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
ltsw_ifa_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_ifa_info[unit].ifa_lock == NULL) {
        ltsw_ifa_info[unit].ifa_lock
            = sal_mutex_create("bcmLtswIfaMutex");
       SHR_NULL_CHECK(ltsw_ifa_info[unit].ifa_lock, SHR_E_MEMORY);
    }

exit:
    if (SHR_FUNC_ERR()) {
        ltsw_ifa_info_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initialize IFA module.
 *
 * This function has to be called before any other IFA functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (IFA_INFO(unit)->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_ifa_detach(unit));
    }

    /* Initialize IFA module software database. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_ifa_info_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ifa_init(unit));

    /* Initialize the EApp. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_ifa_uc_init(unit));

    IFA_INFO(unit)->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize IFA module.
 *
 * This function has to be called before any other IFA functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Detach successfully.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (IFA_INFO(unit)->initialized == FALSE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ifa_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_ifa_uc_detach(unit));

    IFA_INFO(unit)->initialized = FALSE;

    ltsw_ifa_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Specify IFA switch control behaviors.
 *
 * \param [in] unit Unit number.
 * \param [in] control The desired configuration parameter to modify.
 * \param [in] value The value with which to set the parameter.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_control_set(
    int unit,
    bcmi_ltsw_ifa_control_t control,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);


    IFA_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_ifa_control_set(unit, control, value));

    IFA_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve IFA switch control behaviors.
 *
 * \param [in] unit Unit number.
 * \param [in] control The desired configuration parameter to retrieve.
 * \param [out] value Pointer to where the retrieved value will be written.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_control_get(
    int unit,
    bcmi_ltsw_ifa_control_t control,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);


    SHR_NULL_CHECK(value, SHR_E_PARAM);

    IFA_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_ifa_control_get(unit, control, value));

    IFA_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the IFA header.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options
 * \param [in/out] header_id Header ID pointing to IFA header
 * \param [in] header  IFA header
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_header_create(
    int unit,
    uint32 options,
    int *header_id,
    bcm_ifa_header_t *header)
{
    bcmi_ltsw_l3_egr_ifa_t l3_egr_ifa;
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(header_id, SHR_E_PARAM);
    SHR_NULL_CHECK(header, SHR_E_PARAM);

    sal_memset(&l3_egr_ifa, 0, sizeof(bcmi_ltsw_l3_egr_ifa_t));
    l3_egr_ifa.ip_protocol = header->ip_protocol;
    l3_egr_ifa.max_length = header->max_length;
    l3_egr_ifa.hop_limit = header->hop_limit;

    if (options & BCM_IFA_HEADER_OPTIONS_WITH_ID)
    {
        l3_egr_ifa.flags |= BCM_L3_WITH_ID;
    }

    if (options & BCM_IFA_HEADER_OPTIONS_REPLACE) {
        l3_egr_ifa.flags |= BCM_L3_REPLACE;
    }

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_ifa_create(unit, &l3_egr_ifa, header_id));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA header.
 *
 * \param [in] unit Unit Number
 * \param [in] header_id Header ID pointing to IFA header
 * \param [out] header IFA header
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_header_get(
    int unit,
    int header_id,
    bcm_ifa_header_t *header)
{
    bcmi_ltsw_l3_egr_ifa_t l3_egr_ifa;
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(header, SHR_E_PARAM);

    IFA_LOCK(unit);
    ifa_locked = true;

    sal_memset(&l3_egr_ifa, 0, sizeof(bcmi_ltsw_l3_egr_ifa_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_ifa_get(unit, header_id, &l3_egr_ifa));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_header_from_l3_egr(unit, &l3_egr_ifa, header));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the IFA header.
 *
 * \param [in] unit Unit Number
 * \param [in] header_id Header ID
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_header_destroy(int unit, int header_id)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_ifa_destroy(unit, header_id));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all IFA headers.
 *
 * \param [in] unit Unit Number
 * \param [in] cb Callback function
 * \param [in] user_data User data to be passed to callback function
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_header_traverse(
    int unit,
    bcm_ifa_header_traverse_cb cb,
    void *user_data)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_ifa_traverse(unit, cb,
             bcmi_ltsw_ifa_header_from_l3_egr, user_data));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the IFA EApp config info.
 *
 * Set the IFA termination node EApp config info.
 *
 * \param [in] unit Unit number.
 * \param [in] options Options bitmap of BCM_IFA_CONFIG_INFO_XXX.
 * \param [in] config_info Configuration info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_config_info_set(int unit, uint32 options,
                             bcm_ifa_config_info_t *config_data)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_config_info_set(unit, config_data));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA EApp config info.
 *
 * Get the IFA egress node EApp config info.
 *
 * \param [in] unit Unit number.
 * \param [out] config_info Configuration info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_config_info_get(int unit, bcm_ifa_config_info_t *config_info)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_config_info_get(unit, config_info));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the IFA EApp stats.
 *
 * Set the IFA egress node EApp stats.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_data Statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_stat_info_set(int unit, bcm_ifa_stat_info_t *stat_data)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_ifa_uc_stats_set(unit, stat_data));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA EApp stats.
 *
 * Get the IFA egress node EApp stats.
 *
 * \param [in] unit Unit number.
 * \param [out] stat_data Statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_stat_info_get(int unit, bcm_ifa_stat_info_t *stat_data)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_ifa_uc_stats_get(unit, stat_data));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the IFA IPFIX template.
 *
 * Create the IPFIX template used to export IFA records to collector.
 *
 * \param [in] unit Unit number.
 * \param [in] options Options.
 * \param [inout] Id Template Id.
 * \param [in] set_id IPFIX Set ID.
 * \param [in] num_export_elements Number of export elements.
 * \param [in] export_elements List of export elements.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_export_template_create(int unit, uint32 options,
                                    bcm_ifa_export_template_t *id, uint16_t set_id,
                                    int num_export_elements,
                                    bcm_ifa_export_element_info_t *list_of_export_elements)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_template_create(unit, options, id, set_id,
                                          num_export_elements,
                                          list_of_export_elements));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA IPFIX template.
 *
 * Get the IPFIX template that is used to export IFA records to collector.
 *
 * \param [in] unit Unit number.
 * \param [in] Id Template Id.
 * \param [out] set_id IPFIX Set ID.
 * \param [in] max_size Size of the export_elements array.
 * \param [out] export_elements List of export elements.
 * \param [out] num_export_elements Number of export elements.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_export_template_get(int unit, bcm_ifa_export_template_t id,
                                 uint16_t *set_id, int max_size,
                                 bcm_ifa_export_element_info_t *list_of_export_elements,
                                 int *list_size)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_template_get(unit, id, set_id, max_size,
                                       list_of_export_elements, list_size));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the IFA IPFIX template.
 *
 * Destroy the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] Id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_export_template_destroy(int unit, bcm_ifa_export_template_t id)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_ifa_uc_template_destroy(unit, id));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach the IFA collector.
 *
 * Attach the IFA IPFIX collector.
 *
 * \param [in] unit Unit number.
 * \param [in] collector_id Collector ID
 * \param [in] export_profile_id Export profile ID
 * \param [in] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_collector_attach(int unit,
                              bcm_collector_t collector_id,
                              int export_profile_id,
                              bcm_ifa_export_template_t template_id)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_collector_attach(unit, collector_id,
                                           export_profile_id, template_id));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA collector details.
 *
 * Get the attached IFA collector, export profile and IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [out] collector_id Collector ID
 * \param [out] export_profile_id Export profile ID
 * \param [out] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_collector_attach_get(int unit,
                                  bcm_collector_t *collector_id,
                                  int *export_profile_id,
                                  bcm_ifa_export_template_t *template_id)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_collector_attach_get(unit, collector_id,
                                               export_profile_id, template_id));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the IFA collector.
 *
 * Detach the IFA collector.
 *
 * \param [in] unit Unit number.
 * \param [out] collector_id Collector ID
 * \param [out] export_profile_id Export profile ID
 * \param [out] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_collector_detach(int unit,
                              bcm_collector_t collector_id,
                              int export_profile_id,
                              bcm_ifa_export_template_t template_id)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_collector_detach(unit, collector_id,
                                           export_profile_id, template_id));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the template transmit configuration.
 *
 * Set the template transmit configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] template_id Template ID
 * \param [in] collector_id Collector ID
 * \param [in] config Template transmit config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_template_transmit_config_set(int unit,
                                          bcm_ifa_export_template_t template_id,
                                          bcm_collector_t collector_id,
                                          bcm_ifa_template_transmit_config_t *config)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_template_transmit_config_set(unit, template_id,
                                                       collector_id, config));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the template transmit configuration.
 *
 * Get the template transmit configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] template_id Template ID
 * \param [in] collector_id Collector ID
 * \param [out] config Template transmit config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ifa_template_transmit_config_get(int unit,
                                          bcm_ifa_export_template_t template_id,
                                          bcm_collector_t collector_id,
                                          bcm_ifa_template_transmit_config_t *config)
{
    bool ifa_locked = false;

    SHR_FUNC_ENTER(unit);

    IFA_INIT_CHECK(unit);

    IFA_LOCK(unit);
    ifa_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_uc_template_transmit_config_get(unit, template_id,
                                                       collector_id, config));

exit:
    if (ifa_locked) {
        IFA_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
