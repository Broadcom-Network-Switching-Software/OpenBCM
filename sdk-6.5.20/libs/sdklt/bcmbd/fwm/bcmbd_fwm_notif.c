/*! \file bcmbd_fwm_notif.c
 *
 * Functions of notification driver of Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_util.h>
#include <bcmdrd_config.h>
#include <bcmbd/bcmbd_fwm.h>
#include "./bcmbd_fwm_internal.h"

/* Log source for this component. */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_FWM

/* Firmware notification database. */
static bcmbd_fwm_fw_notifications_t *bcmbd_fwm_notif_db[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Internal functions
 */
/* Initialize notification database. */
int
bcmbd_fwm_notif_init(int unit)
{
    int i;

    bcmbd_fwm_fw_notifications_t *fw_notifications = NULL;
    bcmbd_fwm_fw_notif_t *notif;

    SHR_FUNC_ENTER(unit);

    fw_notifications = sal_alloc(sizeof(bcmbd_fwm_fw_notifications_t),
                                 "bcmbdFwmNotify");
    SHR_NULL_CHECK(fw_notifications, SHR_E_MEMORY);

    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        sal_memset(&fw_notifications->fw_notif[i], 0,
                   sizeof(bcmbd_fwm_fw_notif_t));
        notif = &fw_notifications->fw_notif[i];


        notif->index = -1;
        sal_memset(notif->fw_feature, 0,
                   sizeof(char) * BCMBD_FWM_FWINFO_FW_TYPE_MAX);
        notif->loaded_notif_cb = NULL;
        notif->started_notif_cb = NULL;
        notif->stopped_notif_cb = NULL;
    }

    fw_notifications->notif_mutex = sal_mutex_create("bcmbdFwmNotif");
    SHR_NULL_CHECK(fw_notifications->notif_mutex, SHR_E_MEMORY);

    /* Update to core database. */
    bcmbd_fwm_notif_db[unit] = fw_notifications;

exit:
    if (SHR_FUNC_ERR() && fw_notifications) {
        if (fw_notifications->notif_mutex) {
            sal_mutex_destroy(fw_notifications->notif_mutex);
        }
        sal_free(fw_notifications);
    }
    SHR_FUNC_EXIT();
}

/* Cleanup notification database. */
int
bcmbd_fwm_notif_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmbd_fwm_notif_db[unit]) {
        if (bcmbd_fwm_notif_db[unit]->notif_mutex) {
            sal_mutex_destroy(bcmbd_fwm_notif_db[unit]->notif_mutex);
        }
        sal_free(bcmbd_fwm_notif_db[unit]);
        bcmbd_fwm_notif_db[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/* Notification operation. */
int
bcmbd_fwm_notif_op(int unit, const char *feature, int instance,
                   bcmbd_fwm_notif_option_t event, void *user_data)
{
    int found_slot;
    int i;
    bcmbd_fwm_fw_notif_cb_f cb_fun;

    bcmbd_fwm_fw_notifications_t *fw_notifications = NULL;
    bcmbd_fwm_fw_notif_t *notif;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(feature, SHR_E_PARAM);

    fw_notifications = bcmbd_fwm_notif_db[unit];
    SHR_NULL_CHECK(fw_notifications, SHR_E_INIT);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Firmware notification %s.%d event(%d).\n"),
                            feature, instance, event));

    SHR_NULL_CHECK(fw_notifications->notif_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fw_notifications->notif_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    found_slot = -1;
    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        notif = &fw_notifications->fw_notif[i];
        if (!sal_strcasecmp(feature, notif->fw_feature)) {
            found_slot = i;
            break;
        }
    }

    if (found_slot < 0) {
        /* No notifications registered for this firmware. */
        SHR_EXIT();
    }

    switch (event) {
        case BCMBD_FWM_FW_NOTIF_OPTION_LOADED:
            cb_fun = notif->loaded_notif_cb;
            break;
        case BCMBD_FWM_FW_NOTIF_OPTION_STARTED:
            cb_fun = notif->started_notif_cb;
            break;
        case BCMBD_FWM_FW_NOTIF_OPTION_STOPPED:
            cb_fun = notif->stopped_notif_cb;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (cb_fun) {
        cb_fun(unit, feature, instance, event, user_data);
    }

exit:
    if (locked && fw_notifications->notif_mutex) {
        sal_mutex_give(fw_notifications->notif_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Register notification function. */
int
bcmbd_fwm_notif_register(int unit, const char *feature,
                         bcmbd_fwm_notif_option_t event,
                         bcmbd_fwm_fw_notif_cb_f cb, void *user_data)
{
    int i;

    bcmbd_fwm_fw_notifications_t *fw_notifications = NULL;
    bcmbd_fwm_fw_notif_t *notif;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(feature, SHR_E_PARAM);

    fw_notifications = bcmbd_fwm_notif_db[unit];
    SHR_NULL_CHECK(fw_notifications, SHR_E_INIT);

    SHR_NULL_CHECK(fw_notifications->notif_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fw_notifications->notif_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    /* Find the matched feature slot. */
    notif = NULL;
    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        bcmbd_fwm_fw_notif_t *ntf = &fw_notifications->fw_notif[i];
        if (!sal_strcasecmp(feature, ntf->fw_feature)) {
            notif = ntf;
            notif->index = i;
            break;
        }
    }

    /* If not found, find an empty slot */
    if (notif == NULL) {
        for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
            bcmbd_fwm_fw_notif_t *ntf = &fw_notifications->fw_notif[i];
            if (notif->index < 0) {
                notif = ntf;
                notif->index = i;
                break;
            }
        }
    }

    /* Cannot register if there is no empty slot. */
    if (notif == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    sal_strncpy(notif->fw_feature, feature, BCMBD_FWM_FWINFO_FW_TYPE_MAX - 1);
    notif->fw_feature[BCMBD_FWM_FWINFO_FW_TYPE_MAX - 1] = 0;
    switch (event) {
        case BCMBD_FWM_FW_NOTIF_OPTION_LOADED:
            notif->loaded_notif_cb = cb;
            break;
        case BCMBD_FWM_FW_NOTIF_OPTION_STARTED:
            notif->started_notif_cb = cb;
            break;
        case BCMBD_FWM_FW_NOTIF_OPTION_STOPPED:
            notif->stopped_notif_cb = cb;
            break;
        default:
            /* No operation. */
            break;
    }

exit:
    if (locked && fw_notifications->notif_mutex) {
        sal_mutex_give(fw_notifications->notif_mutex);
    }
    SHR_FUNC_EXIT();
}
