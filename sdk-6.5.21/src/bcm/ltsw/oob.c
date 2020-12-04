/*! \file oob.c
 *
 * OOB Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/oob.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/oob.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/mbcm/oob.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_OOB

/*!
 * \brief OOB information data structure.
 */
typedef struct oob_info_s {

    /*! Module is initialized. */
    bool inited;

    /*! Module lock. */
    sal_mutex_t mutex;

} oob_info_t;

/*
 * Static global variable to hold OOB info.
 */
static oob_info_t oob_info[BCM_MAX_NUM_UNITS] = {{0}};

/* OOB info. */
#define OOB_INFO(unit) \
    (&oob_info[unit])

/* Check if OOB module is initialized. */
#define OOB_INIT_CHECK(unit) \
    do { \
        oob_info_t *oob = OOB_INFO(unit); \
        if (oob->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* OOBFC feature check. */
#define OOB_FEATURE_CHECK(_u_)                      \
    do {                                            \
        if (!ltsw_feature(_u_, LTSW_FT_OOB_FC)) {   \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL); \
        }                                           \
    } while(0)

/* Take OOB lock. */
#define OOB_LOCK(unit) \
    sal_mutex_take(OOB_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give OOB lock. */
#define OOB_UNLOCK(unit) \
    sal_mutex_give(OOB_INFO(unit)->mutex)

/******************************************************************************
 * Private functions
 */



/******************************************************************************
 * Public functions
 */

int
bcmi_ltsw_oob_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    /* If not initialized, return success. */
    if (OOB_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    OOB_INFO(unit)->inited = false;

    OOB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_detach(unit));

    OOB_UNLOCK(unit);
    locked = false;

    if (OOB_INFO(unit)->mutex) {
        sal_mutex_destroy(OOB_INFO(unit)->mutex);
        OOB_INFO(unit)->mutex = NULL;
    }

exit:
    if (locked) {
        OOB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_oob_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (OOB_INFO(unit)->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_oob_detach(unit));
    }

    if (OOB_INFO(unit)->mutex == NULL) {
        OOB_INFO(unit)->mutex = sal_mutex_create("OOB mutex");
        SHR_NULL_CHECK(OOB_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_init(unit));
    OOB_INFO(unit)->inited = true;
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_queue_profile_set(int unit, int profile_id, int count,
                                     bcm_oob_fc_tx_queue_config_t *config)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_queue_profile_set(unit, profile_id, count, config);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_queue_profile_get(int unit, int profile_id, int max_count,
                                     bcm_oob_fc_tx_queue_config_t *config,
                                     int *count)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_queue_profile_get(unit,
                                            profile_id,
                                            max_count,
                                            config,
                                            count);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_port_mapping_set(int unit, int count, int *port_array,
                                    int *oob_port_array)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(port_array, SHR_E_PARAM);
    SHR_NULL_CHECK(oob_port_array, SHR_E_PARAM);

    if (count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_port_mapping_set(unit,
                                           count,
                                           port_array,
                                           oob_port_array);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_port_mapping_get(int unit, int max_count, int *port_array,
                                    int *oob_port_array, int *count)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(port_array, SHR_E_PARAM);
    SHR_NULL_CHECK(oob_port_array, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_port_mapping_get(unit,
                                           max_count,
                                           port_array,
                                           oob_port_array,
                                           count);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_config_set(int unit, bcm_oob_fc_tx_config_t *config)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_config_set(unit, config);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_config_get(int unit, bcm_oob_fc_tx_config_t *config)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_config_get(unit, config);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_oob_fc_tx_info_get(int unit, bcm_oob_fc_tx_info_t *info)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    OOB_LOCK(unit);
    rv = mbcm_ltsw_oob_tx_info_get(unit, info);
    OOB_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_oob_egr_profile_id_set(int unit, bcm_port_t port, int profile_id)
{
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_egr_profile_id_set(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_oob_egr_profile_id_get(int unit, bcm_port_t port, int *profile_id)
{
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_egr_profile_id_get(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_oob_egr_profile_num_get(int unit, int *num)
{
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(num, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_egr_profile_num_get(unit, num));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_oob_port_control_set(int unit, bcm_port_t port,
                               int egress, int enable)
{
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    if (enable != 0 && enable != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (egress != 0 && egress != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_port_control_set(unit, port, egress, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_oob_port_control_get(int unit, bcm_port_t port,
                               int egress, int *enable)
{
    SHR_FUNC_ENTER(unit);
    OOB_FEATURE_CHECK(unit);
    OOB_INIT_CHECK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);
    if (egress != 0 && egress != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_oob_port_control_get(unit, port, egress, enable));

exit:
    SHR_FUNC_EXIT();
}

