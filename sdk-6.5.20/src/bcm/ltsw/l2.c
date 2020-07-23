/*! \file l2.c
 *
 * L2 Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/l2_int.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/mbcm/l2.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/generated/l2_ha.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <sal/sal_thread.h>
#include <sal/sal_sleep.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L2

/* Max length of L2 age thread name */
#define L2_AGE_NAME_MAX_LEN 16

/* L2 age thread priority */
#define L2_AGE_THREAD_PRI 50

/* L2 age max timer value */
#define L2_AGE_MAX_TIMER 0x7fffffff

/* Max positive value in seconds for sal_sem_take */
#define L2_AGE_MAX_POSITIVE_VALUE 2147

/* L2 age exit timeout */
#define L2_AGE_THREAD_EXIT_TIMEOUT 10000000

/* L2 age pause timeout */
#define L2_AGE_THREAD_PAUSE_TIMEOUT 10000000

/*
 * L2 age info data structure
 */
typedef struct ltsw_l2_age_info_s {
    /* L2 age sempahore */
    sal_sem_t notify;

    /* Interval between L2 table age passes */
    uint32_t *interval;

    /* L2 age enable */
    bool enable;

    /* L2 age had been paused */
    bool paused;

    /* L2 age task id */
    sal_thread_t pid;

    /* L2 age task name */
    char name[L2_AGE_NAME_MAX_LEN];

    /* Exit thread */
    bool exit;
} ltsw_l2_age_info_t;

/*
 * Max callback number.
 */
#define L2_CB_MAX 5

/*
 * L2 callback info structure.
 */
typedef struct l2_cb_info_s {
    /* User callback function */
    bcm_l2_addr_callback_t fn;

    /* User associated data */
    void *fn_data;
} l2_cb_info_t;

/*
 * L2 callback control structure.
 */
typedef struct l2_cb_ctrl_s {
    /* User callback array */
    l2_cb_info_t cb[L2_CB_MAX];

    /* Callback number */
    int cb_count;
} l2_cb_ctrl_t;

#define CB_ITER(_i) \
    for ((_i) = 0; (_i) < L2_CB_MAX; (_i)++)

/*! Bitmap type */
typedef enum {
    /*! Port bitmap */
    BMP_PORT,

    /*! VP bitmap */
    BMP_VP,

    BMP_MAX
} l2_bmp_type;

/*
* L2 freeze info structure.
*/
typedef struct l2_freeze_info_s {
    /* L2 is frozen or not */
    bool frozen;

    /* L2 h/w is frozen or not */
    bool hw_frozen;

    /* Port bitmap that will be forbidden learning */
    bcm_pbmp_t pbmp;

    /* VP bitmap that will be forbidden learning */
    SHR_BITDCL *vpbmp;
} l2_freeze_info_t;

/*
 * L2 information.
 */
typedef struct ltsw_l2_info_s {

    /* L2 module initialized. */
    int inited;

    /* Mutex lock. */
    sal_mutex_t mutex;

    /* Mutex lock for station control info */
    sal_mutex_t sc_mutex;

    /* L2 station table size. For some chips,
     * it not only can be used as underlay table, but also can be
     * used as overlay table.
     */
    int hybrid_tbl_sz;

    /* underlay station table size. */
    int underlay_tbl_sz;

    /* Station control info. */
    bcmint_l2_station_control_t *sc;

    /* L2 age info */
    ltsw_l2_age_info_t l2age;

    /* Control structure of L2 callback */
    l2_cb_ctrl_t l2_cb;

    /* Freeze info */
    l2_freeze_info_t l2_frz_info;
} ltsw_l2_info_t;

static ltsw_l2_info_t ltsw_l2_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* L2 information */
#define L2_INFO(u)        (&ltsw_l2_info[u])

/* Check if L2 module is initialized */
#define L2_INIT_CHECK(u)                                \
    do {                                                \
        ltsw_l2_info_t *l2i = L2_INFO(u);               \
        if (l2i->inited == false) {                     \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);        \
        }                                               \
    } while(0)

/* L2 Lock */
#define L2_LOCK(u) \
        sal_mutex_take(L2_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define L2_UNLOCK(u) \
        sal_mutex_give(L2_INFO(u)->mutex);

/* L2 Station Definitions */
#define STATION_INFO(u)    (L2_INFO(u)->sc)

/* L2 Station Control Lock */
#define L2_SC_LOCK(u) \
        sal_mutex_take(L2_INFO(u)->sc_mutex, SAL_MUTEX_FOREVER)

#define L2_SC_UNLOCK(u) \
        sal_mutex_give(L2_INFO(u)->sc_mutex)

/* L2 station table size. For some chips,
 * it not only can be used as underlay table, but also can be
 * used as overlay table.
 */
#define HYBRID_STATION_SIZE(u)  (L2_INFO(u)->hybrid_tbl_sz)

/* L2 underlay station table size */
#define UNDERLAY_STATION_SIZE(u)  (L2_INFO(u)->underlay_tbl_sz)

/* Check whether L2 my station data is an overlay entry. */
#define HSDK_L2_STATION_OVERLAY(station) \
    ((((station)->flags & BCM_L2_STATION_UNDERLAY) == 0) && \
      BCMI_LTSW_VIRTUAL_VPN_VFI_IS_SET((station)->vlan))

/* L2 callback structure */
#define L2_CB(u) (L2_INFO(u)->l2_cb)

/* L2 Freeze info */
#define L2_FREEZE_INFO(u) (&(L2_INFO(u)->l2_frz_info))

/* L2 is frozen */
#define L2_IS_FROZEN(u) (L2_FREEZE_INFO(u)->frozen)

/* HW L2 is frozen */
#define L2_IS_HW_FROZEN(u) (L2_FREEZE_INFO(u)->hw_frozen)

/* L2 freeze port bitmap */
#define L2_FREEZE_PBMP(u) &(L2_FREEZE_INFO(u)->pbmp)

/* L2 freeze vp bitmap */
#define L2_FREEZE_VPBMP(u) (L2_FREEZE_INFO(u)->vpbmp)

/******************************************************************************
 * Private functions
 */
/*!
 * \brief L2 age thread.
 *
 * \param [in]  unit          Unit number.
 */
static void
l2_age_thread(void *arg)
{
    int unit = (unsigned long)(arg);
    ltsw_l2_info_t *l2i = L2_INFO(unit);
    int iter = 0, m, c, r, rv;
    uint32_t interval;
    sal_usecs_t stime, etime;

    while ((interval = *l2i->l2age.interval) != 0) {
        if (l2i->l2age.exit == true) {
            goto exit;
        }

        if (!iter) {
            goto age_delay;
        }

        stime = sal_time_usecs();

        if (!l2i->l2age.enable) {
            l2i->l2age.paused = true;
            goto age_delay;
        }

        /* Age process */
        rv = mbcm_ltsw_l2_age(unit);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "l2_age_thread: unit=%d: process age failed"
                                    " rv=%d\n"),
                         unit, rv));
            goto age_delay;
        }

        etime = sal_time_usecs();
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "l2_age_thread: unit=%d: done in %d usec\n"),
                     unit, SAL_USECS_SUB(etime, stime)));
age_delay:
        rv = -1; /* timeout */
        if (!l2i->l2age.enable) {
            l2i->l2age.paused = true;
        }
        if (interval > L2_AGE_MAX_POSITIVE_VALUE) {
            m = (interval / L2_AGE_MAX_POSITIVE_VALUE) * 1000;
            r = (interval % L2_AGE_MAX_POSITIVE_VALUE) * 1000000;

            /* Request to exit */
            if (l2i->l2age.exit == true) {
                goto exit;
            }

            for (c = 0; c < m; c++) {
                rv = sal_sem_take(l2i->l2age.notify,
                                  L2_AGE_MAX_POSITIVE_VALUE * 1000);
                /* age interval is changed */
                if (rv == 0 || interval != *l2i->l2age.interval) {
                    break;
                }
            }

            /* Request to exit */
            if (l2i->l2age.exit == true) {
                goto exit;
            }

            /*
            * Age timer is reset.
            * rv == 0 is to indicate user resets the timer to same value
            */
            if (*l2i->l2age.interval &&
                (rv == 0 || interval != *l2i->l2age.interval)) {
                interval = *l2i->l2age.interval;
                goto age_delay;
            } else if (r) {
                 /* age interval is not changed */
                (void)sal_sem_take(l2i->l2age.notify, r);
            }
        } else {
            /* Request to exit */
            if (l2i->l2age.exit == true) {
                goto exit;
            }
            rv = sal_sem_take(l2i->l2age.notify, interval * 1000000);

            /* Request to exit */
            if (l2i->l2age.exit == true) {
                goto exit;
            }

            /*
            * Age timer is reset.
            * rv == 0 is to indicate user resets the timer to same value
            */
            if (*l2i->l2age.interval &&
                (rv == 0 || interval != *l2i->l2age.interval)) {
                interval = *l2i->l2age.interval;
                goto age_delay;
            }
        }
        iter++;
    }

exit:
    LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "l2_age_thread: exit\n")));
    l2i->l2age.pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*!
 * \brief Start age thread.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_MEMORY       Memory error.
 */
static int
l2_age_start(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);
    uint32 pri;

    SHR_FUNC_ENTER(unit);

    sal_snprintf(l2i->l2age.name, L2_AGE_NAME_MAX_LEN,
                 "bcmL2age.%d", unit);

    pri = bcmi_ltsw_property_get(unit,
                                 BCMI_CPN_L2AGE_THREAD_PRI,
                                 L2_AGE_THREAD_PRI);
    l2i->l2age.pid = sal_thread_create(l2i->l2age.name,
                                       SAL_THREAD_STKSZ,
                                       pri,
                                       l2_age_thread,
                                       (void *)(unsigned long)(unit));
    if (l2i->l2age.pid == SAL_THREAD_ERROR) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stop L2 age thread if any.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_TIMEOUT      Timeout.
 */
static int
l2_age_stop(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);
    uint32_t timeout = 0;

    SHR_FUNC_ENTER(unit);

    if (l2i->l2age.pid != SAL_THREAD_ERROR) {
        l2i->l2age.exit = true;
        sal_sem_give(l2i->l2age.notify);

        while (l2i->l2age.pid != SAL_THREAD_ERROR) {
            sal_usleep(1000);
            timeout += 1000;
            if (timeout > L2_AGE_THREAD_EXIT_TIMEOUT) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "L2 age thread did not exit!\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_TIMEOUT);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pause L2 age thread.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_TIMEOUT      Timeout.
 */
static int
l2_age_pause(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);
    uint32_t timeout = 0;

    SHR_FUNC_ENTER(unit);

    if (l2i->l2age.pid != SAL_THREAD_ERROR) {
        l2i->l2age.enable = false;
        l2i->l2age.paused = false;
        sal_sem_give(l2i->l2age.notify);

        while ((l2i->l2age.paused == false) && (l2i->l2age.exit == false)) {
            /*
             * Release the semaphore to avoid the age thread does not get
             * the semaphore when we are pausing it.
             */
            sal_sem_give(l2i->l2age.notify);
            sal_usleep(1000);
            timeout += 1000;
            if (timeout > L2_AGE_THREAD_PAUSE_TIMEOUT) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "L2 age thread is not paused!\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_TIMEOUT);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Resume  L2 age thread if any.
 *
 * \param [in]  unit          Unit number.
 */
static void
l2_age_resume(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    if (l2i->l2age.pid != SAL_THREAD_ERROR) {
        l2i->l2age.enable = true;
        l2i->l2age.paused = false;
    }
}

/*!
 * \brief Dump L2 age info.
 *
 * \param [in] unit Unit Number.
 */
static void
l2_age_info_dump(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    LOG_CLI((BSL_META_U(unit,
                        "  L2 AGE information:\n")));

    LOG_CLI((BSL_META_U(unit,
                        "  Notify              : %p\n"),
                        (void *)l2i->l2age.notify));
    LOG_CLI((BSL_META_U(unit,
                        "  Interval            : %8d\n"), l2i->l2age.interval ?
                        *l2i->l2age.interval : 0));
    LOG_CLI((BSL_META_U(unit,
                        "  Enable              : %8s\n"), l2i->l2age.enable ?
                        "True" : "False"));
    LOG_CLI((BSL_META_U(unit,
                        "  Pause               : %8s\n"), l2i->l2age.paused ?
                        "True" : "False"));
    LOG_CLI((BSL_META_U(unit,
                        "  Thread id           : %p\n"),
                        (void *)l2i->l2age.pid));
    LOG_CLI((BSL_META_U(unit,
                        "  Thread name         : %20s\n"), l2i->l2age.name));
    LOG_CLI((BSL_META_U(unit,
                        "  Exit                : %8s\n"), l2i->l2age.exit ?
                        "True" : "False"));
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    return;
}

/*!
 * \brief Dump L2 callback info.
 *
 * \param [in] unit Unit Number.
 */
static void
l2_cb_info_dump(int unit)
{
    int i;
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    LOG_CLI((BSL_META_U(unit,
                        "  L2 Callback information:\n")));
    LOG_CLI((BSL_META_U(unit,
                        "  Callback num        : %8d\n"), l2i->l2_cb.cb_count));

    for (i = 0; i < L2_CB_MAX; i++) {
        if (l2i->l2_cb.cb[i].fn) {
            LOG_CLI((BSL_META_U(unit,
                                "  Function       : %p\n"),
                                (void *)l2i->l2_cb.cb[i].fn));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    return;
}

void
bcmint_l2_event_cb(int unit, const char *event, void *notif_info,
                   void *user_data);

/*!
 * \brief Clear L2 callback.
 *
 * This function is to clear L2 callback info and unregister L2 event function
 * from event manager.
 *
 * \param [in] unit Unit number.
 */
static void
l2_cb_clear(int unit)
{
    l2_cb_ctrl_t *l2_cb_ctrl = &L2_CB(unit);
    int i;

    /* No callback, do nothing. */
    if (l2_cb_ctrl->cb_count == 0) {
        return;
    }

    CB_ITER(i) {
        if (l2_cb_ctrl->cb[i].fn) {
            l2_cb_ctrl->cb[i].fn = NULL;
            l2_cb_ctrl->cb[i].fn_data = NULL;
            l2_cb_ctrl->cb_count--;
        }
    }

    /* Unregister local L2 event function from event manager. */
    if (l2_cb_ctrl->cb_count == 0) {
        (void)bcmi_ltsw_event_unregister(unit,
                                         BCMI_LTSW_EV_L2_NOTIF,
                                         &bcmint_l2_event_cb);
    }
    return;
}

/*!
 * \brief Clean up L2 module software database.
 *
 * \param [in]  unit          Unit number.
 */
static void
l2_info_cleanup(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    l2_cb_clear(unit);

    if (l2i->l2_frz_info.vpbmp) {
        sal_free(l2i->l2_frz_info.vpbmp);
        l2i->l2_frz_info.vpbmp = NULL;
    }

    if (l2i->l2age.pid == SAL_THREAD_ERROR && l2i->l2age.notify) {
        sal_sem_destroy(l2i->l2age.notify);
        l2i->l2age.notify = NULL;
    }

    if (l2i->sc_mutex) {
        sal_mutex_destroy(l2i->sc_mutex);
        l2i->sc_mutex = NULL;
    }

    if (l2i->mutex) {
        sal_mutex_destroy(l2i->mutex);
        l2i->mutex = NULL;
    }

    return;
}

/*!
 * \brief This function is to add port/vp into L2 freeze port/vp bitmap.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   type            Bitmap type.
 * \param [in]   value           Port id or vp id.
 */
static void
l2_freeze_bmp_set(int unit, l2_bmp_type type, int value)
{
    bcm_pbmp_t *pbmp = L2_FREEZE_PBMP(unit);
    SHR_BITDCL *vpbmp = L2_FREEZE_VPBMP(unit);

    if (type == BMP_PORT) {
        BCM_PBMP_PORT_ADD(*pbmp, value);
    } else if (type == BMP_VP) {
        if (vpbmp != NULL) {
            SHR_BITSET(vpbmp, value);
        }
    }
}

/*!
 * \brief This function is to remove port/vp from L2 freeze port/vp bitmap.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   type            Bitmap type.
 * \param [in]   value           Port id or vp id.
 */
static void
l2_freeze_bmp_clear(int unit, l2_bmp_type type, int value)
{
    bcm_pbmp_t *pbmp = L2_FREEZE_PBMP(unit);
    SHR_BITDCL *vpbmp = L2_FREEZE_VPBMP(unit);

    if (type == BMP_PORT) {
        BCM_PBMP_PORT_REMOVE(*pbmp, value);
    } else if (type == BMP_VP) {
        if (vpbmp != NULL) {
            SHR_BITCLR(vpbmp, value);
        }
    }
}

/*!
 * \brief This function is to check if port/vp is in L2 freeze port/vp bitmap.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   type            Bitmap type.
 * \param [in]   value           Port id.
 *
 * \retval True Yes.
 * \retval False No.
 */
static bool
l2_freeze_bmp_check(int unit, l2_bmp_type type, int value)
{
    bcm_pbmp_t *pbmp = L2_FREEZE_PBMP(unit);
    SHR_BITDCL *vpbmp = L2_FREEZE_VPBMP(unit);
    bool exist = false;

    if (type == BMP_PORT) {
        if (BCM_PBMP_MEMBER(*pbmp, value)) {
            exist = true;
        }
    } else if (type == BMP_VP) {
        if (vpbmp != NULL && SHR_BITGET(vpbmp, value)) {
            exist = true;
        }
    }
    return exist;
}

/*!
 * \brief During Warmboot, L2 learn can be frozen per port.
 * Sync H/W L2 learn status to S/W.
 *
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
l2_learn_port_bitmap_recover(int unit)
{
    bcm_port_t port, gport;
    uint32_t flags, vp_num = 0, vp;
    int rv;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        rv = bcm_ltsw_port_learn_get(unit, port, &flags);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        } else if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }
        if ((flags & BCM_PORT_LEARN_ARL) == 0) {
            l2_freeze_bmp_set(unit, BMP_PORT, port);
        }
    }
    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_num_get(unit, &vp_num));
    }
    for (vp = 0; vp < vp_num; vp++) {
        rv = bcmi_ltsw_virtual_vp_encode_gport(unit, vp, &gport);
        if (rv == SHR_E_PARAM) {
            continue;
        } else if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_learn_get(unit, gport, &flags));

        if ((flags & BCM_PORT_LEARN_ARL) == 0) {
            l2_freeze_bmp_set(unit, BMP_VP, gport);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize L2 module software database.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
l2_info_init(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t tbl_sz = 0, alloc_sz = 0;
    uint32_t overlay_tbl_sz = 0, underlay_tbl_sz = 0;
    int warm = bcmi_warmboot_get(unit);
    bool detached = false;  /* L2 is detached */
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    int rv, override_freeze = 0;

    SHR_FUNC_ENTER(unit);

    if (l2i->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l2_detach(unit));
    }
    detached = true;

    if (l2i->mutex == NULL) {
        l2i->mutex = sal_mutex_create("l2_info_mutex");
        SHR_NULL_CHECK(l2i->mutex, SHR_E_MEMORY);
    }

    if (l2i->sc_mutex == NULL) {
        l2i->sc_mutex = sal_mutex_create("l2_sc_mutex");
        SHR_NULL_CHECK(l2i->sc_mutex, SHR_E_MEMORY);
    }
    rv = mbcm_ltsw_l2_overlay_station_size_get(unit, &overlay_tbl_sz);
    if (rv == SHR_E_UNAVAIL) {
        overlay_tbl_sz = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Initialize station control info */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_size_get(unit, &underlay_tbl_sz));
    l2i->hybrid_tbl_sz = overlay_tbl_sz;
    l2i->underlay_tbl_sz = underlay_tbl_sz;

    tbl_sz = underlay_tbl_sz + overlay_tbl_sz;
    if (tbl_sz != 0) {
        ha_instance_size = sizeof(bcmint_l2_station_control_t);
        ha_array_size = tbl_sz;
        ha_req_size = ha_instance_size * ha_array_size;
        ha_alloc_size = ha_req_size;
        l2i->sc = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_L2,
                                         BCMINT_L2_SUB_COMP_ID_STATION_CTRL,
                                         "bcmL2StationCtrl",
                                         &ha_alloc_size);
        SHR_NULL_CHECK(l2i->sc, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);

        if (!warm) {
            sal_memset(l2i->sc, 0, ha_alloc_size);
        }
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                 BCMI_HA_COMP_ID_L2,
                 BCMINT_L2_SUB_COMP_ID_STATION_CTRL,
                 0, ha_instance_size, ha_array_size,
                 BCMINT_L2_STATION_CONTROL_T_ID);
        if (rv != SHR_E_EXISTS) {
           SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    /* Initialize L2 age info */
    ha_alloc_size = sizeof(l2i->l2age.interval);
    l2i->l2age.interval = bcmi_ltsw_ha_mem_alloc(
                              unit,
                              BCMI_HA_COMP_ID_L2,
                              BCMINT_L2_SUB_COMP_ID_AGE_INTERVAL,
                              "bcmL2AgeInterval",
                              &ha_alloc_size);
    l2i->l2age.pid = SAL_THREAD_ERROR;
    /* Create L2 age semaphore */
    if (l2i->l2age.notify == NULL) {
        l2i->l2age.notify = sal_sem_create("l2Age timer", SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(l2i->l2age.notify, SHR_E_MEMORY);
    }
    sal_memset(l2i->l2age.name, 0, L2_AGE_NAME_MAX_LEN);
    l2i->l2age.enable = false;
    l2i->l2age.exit = false;
    if (warm) {
        if (*l2i->l2age.interval) {
            l2i->l2age.enable = true;
            SHR_IF_ERR_VERBOSE_EXIT
                (l2_age_start(unit));
        }
    } else {
        *l2i->l2age.interval = 0;
    }

    /* Initialize cb structure */
    sal_memset(&l2i->l2_cb, 0, sizeof(l2_cb_ctrl_t));

    /* Initialize L2 freeze info */
    sal_memset(&l2i->l2_frz_info, 0, sizeof(l2_freeze_info_t));
    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_num_get(unit, &tbl_sz));
    }
    if (tbl_sz != 0) {
        alloc_sz = SHR_BITALLOCSIZE(tbl_sz);
        l2i->l2_frz_info.vpbmp = sal_alloc(alloc_sz, "ltswL2FreezeVpBmp");
        SHR_NULL_CHECK(l2i->l2_frz_info.vpbmp, SHR_E_MEMORY);
        sal_memset(l2i->l2_frz_info.vpbmp, 0, alloc_sz);
    }
    BCM_PBMP_CLEAR(l2i->l2_frz_info.pbmp);

    if (warm) {
        rv = mbcm_ltsw_l2_frozen_cml_recover(unit, &override_freeze);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
        l2i->l2_frz_info.hw_frozen = override_freeze ? true : false;

        SHR_IF_ERR_VERBOSE_EXIT
            (l2_learn_port_bitmap_recover(unit));
        if (BCM_PBMP_NOT_NULL(l2i->l2_frz_info.pbmp)) {
            l2i->l2_frz_info.hw_frozen = true;
        }
    }

exit:
    if (SHR_FUNC_ERR() && detached) {
        if (warm && l2i->l2age.pid != SAL_THREAD_ERROR) {
            (void)l2_age_stop(unit);
        }

        if (!warm && l2i->l2age.interval) {
            (void)bcmi_ltsw_ha_mem_free(unit, l2i->l2age.interval);
        }

        l2_info_cleanup(unit);

        if (!warm && l2i->sc) {
            (void)bcmi_ltsw_ha_mem_free(unit, l2i->sc);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate L2 addr parameters.
 *
 * \param [in]  unit          Unit number.
 * \param [out] l2addr        L2 address structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
l2_addr_params_validate(int unit, bcm_l2_addr_t *l2addr)
{
    SHR_FUNC_ENTER(unit);

    if (l2addr == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (l2addr->cos_dst < BCM_PRIO_MIN) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (l2addr->flags & BCM_L2_MIRROR) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        if (BCM_MAC_IS_MCAST(l2addr->mac)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (l2addr->tsn_flowset || l2addr->sr_flowset ||
        l2addr->flags & BCM_L2_SR_SAN_DEST) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_trunk_id_validate(unit, l2addr->tgid));
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function for converting delete flags to replace flags.
 *
 * \param [in]   unit           Unit number.
 * \param [in]   flags          Delete flags.
 * \param [out]  repl_flags     Replace flags.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval SHR_E_UNAVAIL        Feature unavailable.
 */
static int
l2_delete_replace_flags_convert(int unit, uint32_t flags,
                                uint32_t *repl_flags)
{
    uint32_t tmp_flags = BCM_L2_REPLACE_DELETE;

    SHR_FUNC_ENTER(unit);

    /* Age feature only supported by bulk Control */
    if (flags & BCM_L2_REPLACE_AGE){
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (flags & BCM_L2_DELETE_PENDING) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (flags & BCM_L2_DELETE_STATIC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_STATIC;
    }

    if (flags & BCM_L2_DELETE_NO_CALLBACKS) {
        tmp_flags |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    *repl_flags = tmp_flags;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to setup replace destination.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   module          Module id.
 * \param [in]   port            Port number.
 * \param [in]   trunk           Trunk group id.
 * \param [in]   is_trunk        Destination is trunk or not.
 * \param [out]  dest            Replace destination structure pointer.
 *
 * \retval SHR_E_NONE            No errors.
 */
static int
l2_replace_dest_setup(int unit, bcm_module_t module, bcm_port_t port,
                      bcm_trunk_t trunk, int is_trunk,
                      bcmint_l2_replace_dest_t *dest)
{
    int max_modid = 0, use_gport = 0;
    SHR_FUNC_ENTER(unit);

    /* If all are -1, means ignore the replacement with new destination */
    if (module == -1 && port == -1 && trunk == -1) {
        dest->module = dest->port = dest->trunk = dest->vp = -1;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &use_gport));

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            dest->port = port;
            /* We don't need vp to match. */
            dest->vp = -1;
            dest->trunk = -1;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, port, &dest->module,
                                              &dest->port, &dest->trunk,
                                              &dest->vp));
            if (use_gport) {
                dest->port = port;
            }
        }
    } else if (is_trunk) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_id_validate(unit, trunk));
        dest->trunk = trunk;
        dest->vp = -1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, module, port,
                                      &dest->module, &dest->port));
        max_modid = bcmi_ltsw_dev_max_modid(unit);
        if (!(dest->module >= 0 && dest->module <= max_modid)) {
             SHR_ERR_EXIT(SHR_E_BADID);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));

        dest->port = port;
        dest->trunk = -1;
        dest->vp = -1;
        if (use_gport) {
            BCM_GPORT_MODPORT_SET(dest->port, module, port);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function for l2 replace op.
 *
 * \param [in]  unit            Unit number
 * \param [in]  flags           Replace flags.
 * \param [in]  int_flags       Internal flags.
 * \param [in]  match_addr      Match l2 address structure.
 * \param [in]  new_module      New module id.
 * \param [in]  new_port        New port number.
 * \param [in]  new_trunk       New trunk group id.
 * \param [in]  trav_fn         Traverse callback function.
 * \param [in]  user_data       User data.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval SHR_E_PARAM          Invalid parameter.
 */
static int
l2_replace(int unit, uint32_t flags, uint32_t int_flags,
           bcm_l2_addr_t *match_addr,
           bcm_module_t new_module,
           bcm_port_t new_port,
           bcm_trunk_t new_trunk,
           bcm_l2_traverse_cb trav_fn,
           void *user_data)
{
    bcmint_l2_replace_t   rep_st;
    int max_modid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&rep_st, 0, sizeof(bcmint_l2_replace_t));

    rep_st.trav_fn = trav_fn;
    rep_st.user_data = user_data;
    rep_st.flags = flags;
    rep_st.int_flags = int_flags;

    if ((int_flags & BCMINT_L2_REPLACE_IGNORE_PORTID) &&
        !(flags & BCM_L2_REPLACE_DELETE) &&
        !(flags & BCM_L2_REPLACE_MATCH_DEST)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(flags & BCM_L2_REPLACE_DELETE) &&
        !(int_flags & BCMINT_L2_REPLACE_CLEAR_HIT_ONLY)) {
        SHR_IF_ERR_VERBOSE_EXIT(l2_replace_dest_setup(unit,
                                new_module, new_port, new_trunk,
                                flags & BCM_L2_REPLACE_NEW_TRUNK,
                                &rep_st.new_dest));
    }

    if (match_addr != NULL) {
        rep_st.key_l2_flags = match_addr->flags;

        if (flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (int_flags & BCMINT_L2_REPLACE_IGNORE_PORTID) {
                max_modid = bcmi_ltsw_dev_max_modid(unit);
                if (!(match_addr->modid >= 0 &&
                      match_addr->modid <= max_modid)) {
                     SHR_ERR_EXIT(SHR_E_BADID);
                }

                rep_st.match_dest.vp = -1;
                rep_st.match_dest.trunk = -1;
                rep_st.match_dest.port = -1;
                rep_st.match_dest.module = match_addr->modid;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(l2_replace_dest_setup
                                       (unit, match_addr->modid,
                                        match_addr->port,
                                        match_addr->tgid,
                                        match_addr->flags & BCM_L2_TRUNK_MEMBER,
                                        &rep_st.match_dest));
            }

            if (rep_st.match_dest.vp != -1) {
                /* Use port to match instead. */
                return SHR_E_UNAVAIL;
            }
        }

        if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
            if (match_addr->vid > BCM_VLAN_MAX &&
                !bcmi_ltsw_virtual_vlan_vpn_valid(unit, match_addr->vid)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            rep_st.key_vlan = match_addr->vid;
        }

        if (flags & BCM_L2_REPLACE_MATCH_MAC) {
            /* When replace L2 by MAC, key type must be provided. */
            /* Key type is 0 by default for L2 Bridge. */
            sal_memcpy(&rep_st.key_mac, match_addr->mac, sizeof(bcm_mac_t));
            if (flags & BCM_L2_REPLACE_VPN_TYPE) {
                return SHR_E_UNAVAIL;
            }
        }

        if (flags & BCM_L2_REPLACE_MATCH_CLASS_ID) {
            rep_st.key_class_id = match_addr->group;
        }
    } else {
        if (flags & BCM_L2_REPLACE_VPN_TYPE) {
            return SHR_E_UNAVAIL;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_addr_replace(unit, &rep_st));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a station id.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  overlay    Overlay L2 my station table.
 * \param [out] sid           L2 station id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_RESOURCE     No resource.
 */
static int
l2_station_id_alloc(int unit, bool overlay, int *sid)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    int tbl_sz, i, offset = 0;

    SHR_FUNC_ENTER(unit);

    if (overlay && HYBRID_STATION_SIZE(unit) <= 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (overlay && HYBRID_STATION_SIZE(unit) > 0) {
        offset = UNDERLAY_STATION_SIZE(unit);
    }
    tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);

    for (i = offset; i < tbl_sz; i++) {
        if (sc[i].installed == false) {
            break;
        }
    }
    if (i == tbl_sz) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    /* Found a free id */
    *sid = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a station id.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  sid           L2 station id.
 */
static void
l2_station_id_free(int unit, int sid)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    int tbl_sz;

    tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);
    if (sid >= tbl_sz || sid < 0) {
        return;
    }
    sc[sid].installed = false;
    return;
}

/*!
 * \brief Search a station entry from database with given key.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  l2station     L2 station structure.
 * \param [out] tnl           The entry is added by tunnel API or not.
 * \param [out] sid           L2 station id.
 *
 * \retval SHR_E_NONE         Found.
 * \retval SHR_E_NOT_FOUND    Not found.
 */
static int
l2_station_search(int unit, bcm_l2_station_t *l2station, bool *tnl, int *sid)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    int tbl_sz = 0, i, equal, offset = 0;
    bool underlay;

    SHR_FUNC_ENTER(unit);

    underlay = HSDK_L2_STATION_OVERLAY(l2station) ? 0 : 1;

    if (HSDK_L2_STATION_OVERLAY(l2station) &&
        HYBRID_STATION_SIZE(unit) > 0) {
        offset = UNDERLAY_STATION_SIZE(unit);
    }
    tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);
    for (i = offset; i < tbl_sz; i++) {
        /* Skip unused entry */
        if (sc[i].installed == false) {
            continue;
        }

        /* Compare key */
        equal = sal_memcmp(l2station->dst_mac,
                           sc[i].dst_mac, sizeof(bcm_mac_t)) ? 0 : 1;
        if (equal == 0) {
            continue;
        }
        equal = sal_memcmp(l2station->dst_mac_mask,
                           sc[i].dst_mac_mask, sizeof(bcm_mac_t)) ? 0 : 1;
        if (equal == 0) {
            continue;
        }
        if (l2station->vlan != sc[i].vlan ||
            l2station->vlan_mask!= sc[i].vlan_mask) {
            continue;
        }

        if (l2station->inner_vlan != sc[i].ivlan ||
            l2station->inner_vlan_mask!= sc[i].ivlan_mask) {
            continue;
        }

        if (l2station->vfi != sc[i].vfi ||
            l2station->vfi_mask != sc[i].vfi_mask) {
            continue;
        }
        if (l2station->src_port != sc[i].src_port ||
            l2station->src_port_mask != sc[i].src_port_mask) {
            continue;
        }
        if (underlay != sc[i].underlay) {
            continue;
        }
        /* found */
        break;
    }
    if (i == tbl_sz) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    /* Found a matched id */
    *sid = i;
    *tnl = sc[i].tnl;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a station entry into database.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  sid           L2 station id.
 * \param [in]  l2station     L2 station structure.
 * \param [in]  tnl           The entry is added by tunnel API or not.
 *
 * \retval SHR_E_NONE         No error.
 */
static int
l2_station_add(int unit, int sid, bcm_l2_station_t *l2station, bool tnl)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    bool underlay;

    underlay = HSDK_L2_STATION_OVERLAY(l2station) ? 0 : 1;

    sal_memset(&sc[sid], 0, sizeof(bcmint_l2_station_control_t));
    sal_memcpy(sc[sid].dst_mac, l2station->dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(sc[sid].dst_mac_mask, l2station->dst_mac_mask,
               sizeof(bcm_mac_t));
    sc[sid].vlan          = l2station->vlan;
    sc[sid].vlan_mask     = l2station->vlan_mask;
    sc[sid].ivlan         = l2station->inner_vlan;
    sc[sid].ivlan_mask    = l2station->inner_vlan_mask;
    sc[sid].vfi           = l2station->vfi;
    sc[sid].vfi_mask      = l2station->vfi_mask;
    sc[sid].src_port      = l2station->src_port;
    sc[sid].src_port_mask = l2station->src_port_mask;
    sc[sid].sid           = sid;
    sc[sid].tnl           = tnl;
    sc[sid].installed     = true;
    sc[sid].forwarding_domain = l2station->forwarding_domain;
    sc[sid].underlay = underlay;
    sc[sid].use_underlay_flag = (l2station->flags & BCM_L2_STATION_UNDERLAY) ? true : false;
    return SHR_E_NONE;
}

/*!
 * \brief Get station key from database with given station id.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  sid           L2 station id.
 * \param [out] l2station     L2 station structure.
 * \param [out] int_flags     Internal flags
 * \param [out] use_underlay_flag User configures station entry with BCM_L2_STATION_UNDERLAY.
 *
 * \retval SHR_E_NONE         No error.
 */
static int
l2_station_get(int unit, int sid, bcm_l2_station_t *l2station,
               int *int_flags, bool *use_underlay_flag)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    int tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);

    SHR_FUNC_ENTER(unit);

    if (tbl_sz == 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (sid >= tbl_sz) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (sc[sid].installed == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memset(l2station, 0, sizeof(bcm_l2_station_t));
    sal_memcpy(l2station->dst_mac, sc[sid].dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(l2station->dst_mac_mask, sc[sid].dst_mac_mask,
               sizeof(bcm_mac_t));
    l2station->vlan            = sc[sid].vlan;
    l2station->vlan_mask       = sc[sid].vlan_mask;
    l2station->inner_vlan      = sc[sid].ivlan;
    l2station->inner_vlan_mask = sc[sid].ivlan_mask;
    l2station->vfi             = sc[sid].vfi;
    l2station->vfi_mask        = sc[sid].vfi_mask;
    l2station->src_port        = sc[sid].src_port;
    l2station->src_port_mask   = sc[sid].src_port_mask;
    l2station->forwarding_domain = sc[sid].forwarding_domain;
    if (sc[sid].underlay && HYBRID_STATION_SIZE(unit) > 0) {
        l2station->flags |= BCM_L2_STATION_UNDERLAY;
    }
    if (sid >= UNDERLAY_STATION_SIZE(unit)) {
        *int_flags |= BCMINT_L2_STATION_OVERLAY;
    }
    if (use_underlay_flag != NULL) {
        *use_underlay_flag = sc[sid].use_underlay_flag;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse station database and call customer's callback if any.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  trav_fn       Customer's callback.
 * \param [in]  user_data     Customer's cookie.
 *
 * \retval SHR_E_NONE         No error.
 */
static int
l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn,
                    void *user_data)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    bcm_l2_station_t station;
    int tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);
    int int_flags = 0, i;
    bool use_underlay_flag = false;

    SHR_FUNC_ENTER(unit);

    if (trav_fn == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < tbl_sz; i++) {
        /* Skip unused entry */
        if (sc[i].installed == false) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_station_get(unit, i, &station, &int_flags, &use_underlay_flag));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l2_station_get(unit, &station, int_flags));

        if (use_underlay_flag == 0) {
            station.flags &= ~BCM_L2_STATION_UNDERLAY;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (trav_fn(unit, &station, user_data));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all station entries from both database and H/W.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  flags         Internal L2 station flags.
 * \param [in]  tnl           Only delete entries added by tunnel API.
 *
 * \retval SHR_E_NONE         No error.
 */
static int
l2_station_delete_all(int unit, bool tnl)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    bcm_l2_station_t station;
    int tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);
    int int_flags = 0, i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < tbl_sz; i++) {
        /* Skip unused entry */
        if (sc[i].installed == false) {
            continue;
        }

        if (tnl && !(sc[i].tnl)) {
            continue;
        }

        /* Get key by id */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_station_get(unit, i, &station, &int_flags, NULL));

        /* Delete entry from database */
        l2_station_id_free(unit, i);

        /* Delete entry from H/W */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l2_station_delete(unit, &station, int_flags));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate L2 station parameters.
 *
 * \param [in]  unit          Unit number.
 * \param [out] l2station     L2 station structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
l2_station_params_validate(int unit, int *station_id,
                           bcm_l2_station_t *l2station)
{
    int tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);
    bool underlay;
    uint32_t vfi;

    SHR_FUNC_ENTER(unit);

    if (tbl_sz == 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (l2station == NULL || station_id == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((l2station->flags & BCM_L2_STATION_REPLACE) &&
        !(l2station->flags & BCM_L2_STATION_WITH_ID)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (l2station->flags & BCM_L2_STATION_WITH_ID) {
        underlay = HSDK_L2_STATION_OVERLAY(l2station) ? false : true;
        if (underlay) {
            if (*station_id < 0 ||
                *station_id >= tbl_sz) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            if (*station_id >= tbl_sz || *station_id < UNDERLAY_STATION_SIZE(unit)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }
    SHR_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, l2station->vlan, &vfi));

    SHR_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, l2station->vlan_mask, &vfi));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update station table by vlan id.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           Vlan id
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
 static int
 l2_station_update_by_vlan(int unit, int vid)
{
    bcmint_l2_station_control_t *sc = STATION_INFO(unit);
    bcm_l2_station_t station;
    uint32_t vfi;
    int i, tbl_sz = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);
    int int_flags = 0;
    bcm_vlan_t vlan_to_check = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < tbl_sz; i++) {
        /* Skip unused entry */
        if (sc[i].installed == false) {
            continue;
        }

        /* Get key by id */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_station_get(unit, i, &station, &int_flags, NULL));

        /* Trust forwarding_domain if it is non-zero. */
        if (station.forwarding_domain) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_vfi_idx_get(unit, station.forwarding_domain,
                                               &vfi));
            vlan_to_check = vfi;
        } else if (station.vlan & station.vlan_mask) {
            vlan_to_check = station.vlan;
        } else if (station.vfi & station.vfi_mask) {
            vlan_to_check = station.vfi;
        } else {
            continue;
        }
        if (vid != vlan_to_check) {
            continue;
        }

        station.flags |= BCM_L2_STATION_REPLACE;

        /* Update entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l2_station_add(unit, &station, int_flags));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tuple hash function.
 *
 * This function is used to generate the index to tuple hash table. The hash algorithm is
 * based on 64 bit mix-functions
 *
 * \param [in]  tuple             Element tuple.
 *
 * \retval Hash index.
 */
static int
tuple_hash(bcmint_l2_vlan_gport_tuple_t tuple)
{
    int a, b;
    uint64_t key;

    a = tuple.vlan;
    b = tuple.gport;

    key = ((uint64_t)a << 32) | b;
    key = (~key) + (key << 21);
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key % BCMINT_L2_MAX_HASH_TUPLE_COUNT;
}

/*!
 * \brief Create a (vlan, gport) tuple
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tuple_ctrl    Tuple control info.
 * \param [in]  tuple         Tuple info.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_MEMORY       Out of memory.
 */
static int
tuple_create(int unit, bcmint_l2_tuple_ctrl_t *tuple_ctrl,
             bcmint_l2_vlan_gport_tuple_t tuple)
{
    bcmint_l2_tuple_list_t *tuple_node;
    int h = tuple_hash(tuple);

    SHR_FUNC_ENTER(unit);

    tuple_node = tuple_ctrl->tuple_list[h];
    while (tuple_node != NULL) {
        if (tuple_node->tuple.vlan == tuple.vlan &&
            tuple_node->tuple.gport == tuple.gport) {
            break;
        }
        tuple_node = tuple_node->next;
    }

    if (tuple_node != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    tuple_node = sal_alloc(sizeof(bcmint_l2_tuple_list_t),"ltswl2Tuple");
    SHR_NULL_CHECK(tuple_node, SHR_E_MEMORY);
    tuple_node->tuple = tuple;
    tuple_node->next = tuple_ctrl->tuple_list[h];
    tuple_ctrl->tuple_list[h] = tuple_node;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find if a tuple exists in hash table
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tuple_ctrl    Tuple control info.
 * \param [in]  tuple         Tuple info.
 *
 * \retval True Yes.
 * \retval False No.
 */
static bool
tuple_find(int unit, bcmint_l2_tuple_ctrl_t *tuple_ctrl,
           bcmint_l2_vlan_gport_tuple_t tuple)
{
    bcmint_l2_tuple_list_t *tuple_node;
    int h = tuple_hash(tuple);
    bool found = false;

    tuple_node = tuple_ctrl->tuple_list[h];
    while (tuple_node != NULL) {
        if (tuple_node->tuple.vlan == tuple.vlan &&
            tuple_node->tuple.gport == tuple.gport) {
            found = true;
            break;
        }
        tuple_node = tuple_node->next;
    }

    return found;
}

/*!
 * \brief Cleanup tuple control info
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tuple_ctrl    Tuple control info.
 */
static void
tuple_cleanup(int unit, bcmint_l2_tuple_ctrl_t *tuple_ctrl)
{
    bcmint_l2_tuple_list_t *tuple_node;
    int i;

    for (i = 0; i < BCMINT_L2_MAX_HASH_TUPLE_COUNT; i++) {
        while (tuple_ctrl->tuple_list[i] != NULL) {
            tuple_node = tuple_ctrl->tuple_list[i];
            tuple_ctrl->tuple_list[i] = tuple_node->next;
            sal_free(tuple_node);
        }
    }
}

/*!
 * \brief Validate tuple parameters
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tuple         Tuple.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Parameter error.
 */
static int
tuple_params_validate(int unit, bcmint_l2_vlan_gport_tuple_t tuple)
{
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    int modid, port, trunk_id, id, rv;
    bcmi_ltsw_virtual_vp_info_t vp_info;
    bool vp_checked = false;

    SHR_FUNC_ENTER(unit);
    vlan = tuple.vlan;
    gport = tuple.gport;

    if (vlan == BCM_VLAN_INVALID && gport == BCM_GPORT_INVALID) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (vlan != BCM_VLAN_INVALID) {
        if (vlan > BCM_VLAN_MAX && !BCMI_LTSW_VIRTUAL_VPN_VFI_IS_SET(vlan)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* If VPN is set, check if vp is used or not. */
    if (BCMI_LTSW_VIRTUAL_VPN_VFI_IS_SET(vlan)) {
        vp_checked = true;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit,
                                           gport,
                                           &id));
        rv = bcmi_ltsw_virtual_vp_info_get(unit, id, &vp_info);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (gport != BCM_GPORT_INVALID) {
        if (BCM_GPORT_IS_MODPORT(gport) ||
            BCM_GPORT_IS_TRUNK(gport)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, gport, &modid,
                                              &port, &trunk_id, &id));

            if (BCM_GPORT_IS_TRUNK(gport)) {
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmi_ltsw_trunk_id_validate(unit, trunk_id));
            }
        } else {
            if (!vp_checked) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_validate(unit,
                                                   gport,
                                                   &id));
                rv = bcmi_ltsw_virtual_vp_info_get(unit, id, &vp_info);
                if (SHR_FAILURE(rv)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage tuple control info
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tuple_ctrl    Tuple control info.
 * \param [in]  flags         API flags.
 * \param [in]  num_pairs     Num of array.
 * \param [in]  vlan          Vlan array.
 * \param [in]  gport         Gport array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_MEMORY       Out of memory.
 */
static int
tuple_ctrl_stage(int unit, bcmint_l2_tuple_ctrl_t *tuple_ctrl,
                 uint32_t flags, int num_pairs, bcm_vlan_t *vlan,
                 bcm_gport_t *gport)
{
    bcmint_l2_vlan_gport_tuple_t tuple;
    int i;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_pairs; i++) {
        if (!(tuple_ctrl->int_flags & BCMINT_L2_DONT_CARE_VLAN) &&
            vlan[i] == BCM_VLAN_INVALID) {
            tuple_ctrl->int_flags |= BCMINT_L2_DONT_CARE_VLAN;
        }
        if (!(tuple_ctrl->int_flags & BCMINT_L2_DONT_CARE_GPORT) &&
            gport[i] == BCM_GPORT_INVALID) {
            tuple_ctrl->int_flags |= BCMINT_L2_DONT_CARE_GPORT;
        }
        tuple.vlan = vlan[i];
        tuple.gport = gport[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (tuple_params_validate(unit, tuple));
        SHR_IF_ERR_VERBOSE_EXIT
            (tuple_create(unit, tuple_ctrl, tuple));
    }
    tuple_ctrl->flags = flags;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Secondary callback function to call customer's callbacks.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   l2addr          L2 address structure.
 * \param [in]   add             add operation.
 * \param [in]   userdata        User data pointer.
 */
static void
l2_addr_callback(int unit, bcm_l2_addr_t *l2addr, int add, void *userdata)
{
    l2_cb_ctrl_t *l2_cb_ctrl = &L2_CB(unit);
    int i;

    CB_ITER(i) {
        if (l2_cb_ctrl->cb[i].fn) {
            l2_cb_ctrl->cb[i].fn(unit, l2addr, add, l2_cb_ctrl->cb[i].fn_data);
        }
    }
    return;
}

/*!
 * \brief Disable L2 learning on all ports & vps.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
l2_learn_freeze(int unit)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_gport_t gport;
    uint32_t flags, vp_num = 0;
    int rv, vp, l2_learn_mode = 0;

    SHR_FUNC_ENTER(unit);

    rv = mbcm_ltsw_l2_frozen_cml_save(unit, &l2_learn_mode);
    if (rv != SHR_E_UNAVAIL && l2_learn_mode) {
        /* exit */
        SHR_IF_ERR_EXIT(rv);
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        rv = bcm_ltsw_port_learn_get(unit, port, &flags);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        } else if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }
        if (flags & BCM_PORT_LEARN_ARL) {
            /* Record port/vp in bitmap */
            l2_freeze_bmp_set(unit, BMP_PORT, port);

            /* Clear learn bit */
            flags &= ~BCM_PORT_LEARN_ARL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_learn_set(unit, port, flags));
        }
    }

    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_num_get(unit, &vp_num));
    }

    for (vp = 0; vp < vp_num; vp++) {
        rv = bcmi_ltsw_virtual_vp_encode_gport(unit, vp, &gport);
        if (rv == SHR_E_PARAM) {
            continue;
        } else if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_learn_get(unit, gport, &flags));
        if (flags & BCM_PORT_LEARN_ARL) {
            /* Record port/vp in bitmap */
            l2_freeze_bmp_set(unit, BMP_VP, vp);

            /* Clear learn bit */
            flags &= ~BCM_PORT_LEARN_ARL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_learn_set(unit, gport, flags));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore L2 learning on the ports & vps which were frozen before.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
l2_learn_restore(int unit)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_gport_t gport;
    uint32_t flags, vp_num = 0;
    int rv, vp, l2_learn_mode = 0;

    SHR_FUNC_ENTER(unit);

    rv = mbcm_ltsw_l2_frozen_cml_restore(unit, &l2_learn_mode);
    if (BCM_PBMP_IS_NULL(L2_FREEZE_INFO(unit)->pbmp)) {
        /* exit */
        SHR_IF_ERR_EXIT(rv);
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        if (l2_freeze_bmp_check(unit, BMP_PORT, port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_learn_get(unit, port, &flags));
            /* Or learn bit */
            flags |= BCM_PORT_LEARN_ARL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_learn_set(unit, port, flags));
            l2_freeze_bmp_clear(unit, BMP_PORT, port);
        }
    }

    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_num_get(unit, &vp_num));
    }

    for (vp = 0; vp < vp_num; vp++) {
        if (l2_freeze_bmp_check(unit, BMP_VP, vp)) {
            rv = bcmi_ltsw_virtual_vp_encode_gport(unit, vp, &gport);
            if (rv == SHR_E_PARAM) {
                continue;
            } else if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT(rv);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_learn_get(unit, gport, &flags));
            /* Or learn bit */
            flags |= BCM_PORT_LEARN_ARL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_learn_set(unit, gport, flags));
            l2_freeze_bmp_clear(unit, BMP_VP, vp);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
/*!
 * \brief Public function used to find if a tuple exists
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tuple_ctrl    Tuple control info.
 * \param [in]  tuple         Tuple info.
 *
 * \retval True Yes.
 * \retval False No.
 */
bool
bcmint_l2_tuple_find(int unit, bcmint_l2_tuple_ctrl_t *tuple_ctrl,
                     bcmint_l2_vlan_gport_tuple_t tuple)
{
    return tuple_find(unit, tuple_ctrl, tuple);
}

/*!
 * \brief Get amount of customer's callbacks.
 *
 * \param [in]   unit            Unit number.
 *
 * \retval Amount of callbacks.
 */
int
bcmint_l2_callback_num(int unit)
{
    l2_cb_ctrl_t *l2_cb_ctrl = &L2_CB(unit);
    return l2_cb_ctrl->cb_count;
}

/*!
 * \brief L2 event callback function.
 *
 * This function is registered to event manager, and it is used to handle the L2 events
 * generated by API calling, L2 age thread, L2 learning callback, which will make l2 table
 * change. In this callback function, customer's registered callbacks via
 * bcm_l2_addr_register API will be called one by one
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   L2 event info.
 * \param [in]   user_data    User data.
 */
void
bcmint_l2_event_cb(int unit, const char *event, void *notif_info,
                   void *user_data)
{
    bcmint_l2_event_info_t *l2_event = NULL;
    bcm_l2_addr_t *l2_add = NULL, *l2_del = NULL;
    int add_op, del_op;
    uint32_t flags = 0;

    l2_event = (bcmint_l2_event_info_t *)notif_info;
    if (l2_event == NULL) {
        return;
    }

    switch (l2_event->type) {
        case L2_EVENT_ADD:
            l2_add = &l2_event->add;
            add_op = BCM_L2_CALLBACK_ADD;
            break;

        case L2_EVENT_DELETE:
            l2_del = &l2_event->del;
            del_op = BCM_L2_CALLBACK_DELETE;
            break;

        case L2_EVENT_UPDATE:
            l2_del = &l2_event->del;
            del_op = BCM_L2_CALLBACK_DELETE;

            l2_add = &l2_event->add;
            add_op = BCM_L2_CALLBACK_ADD;
            break;

        case L2_EVENT_LEARN:
            l2_add = &l2_event->add;
            add_op = BCM_L2_CALLBACK_LEARN_EVENT;
            break;

        case L2_EVENT_AGE:
            l2_del = &l2_event->del;
            del_op = BCM_L2_CALLBACK_AGE_EVENT;
            break;

        case L2_EVENT_MOVE:
            l2_del = &l2_event->del;
            del_op = BCM_L2_CALLBACK_DELETE;

            l2_add = &l2_event->add;
            add_op = BCM_L2_CALLBACK_MOVE_EVENT;

            flags = BCM_L2_MOVE;
            if (l2_del->modid != l2_add->modid ||
                l2_del->tgid != l2_add->tgid ||
                l2_del->port != l2_add->port) {
                flags |= BCM_L2_MOVE_PORT;
            }
            l2_del->flags |= flags;
            l2_add->flags |= flags;
            break;

        default:
            return;
    }

    if (l2_del != NULL) {
        l2_addr_callback(unit, l2_del, del_op, NULL);
    }
    if (l2_add != NULL) {
        l2_addr_callback(unit, l2_add, add_op, NULL);
    }
}

/*!
 * \brief This function is to notify assist thread of corresponding L2 event.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   l2_addr_add  Added L2 entry.
 * \param [in]   l2_addr_del  Deleted L2 entry.
 * \param [in]   type         Entry type.
 */
void
bcmint_l2_event_notify(int unit, bcm_l2_addr_t *l2_addr_add,
                       bcm_l2_addr_t *l2_addr_del, bcmint_l2_event_type_t type)
{
    bcmint_l2_event_info_t l2_event;

    if (l2_addr_add != NULL) {
        sal_memcpy(&l2_event.add, l2_addr_add, sizeof(bcm_l2_addr_t));
    }
    if (l2_addr_del != NULL) {
        sal_memcpy(&l2_event.del, l2_addr_del, sizeof(bcm_l2_addr_t));
    }
    l2_event.type = type;

    bcmi_ltsw_event_notify(unit, BCMI_LTSW_EV_L2_NOTIF, (void *)&l2_event,
                           sizeof(l2_event));
}

bool
bcmint_l2_station_db_get(int unit, bcm_l2_station_t *l2station, bool *tnl)
{
    int rv, sid;

    L2_SC_LOCK(unit);

    rv = l2_station_search(unit, l2station, tnl, &sid);

    L2_SC_UNLOCK(unit);

    if (SHR_FAILURE(rv)) {
        return false;
    } else {
        return true;
    }
}

/*!
 * \brief Set force vlan for all L2 entries of associated port
 *
 * \param [in] unit Unit number.
 * \param [in] port Port id.
 * \param [in] vlan Force vlan id.
 * \param [in] flags Force vlan flags.
 */
int
bcmi_ltsw_l2_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                       uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_force_vlan_set(unit, port, vlan, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Public function to stop L2 age thread without breaking current interval
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmi_ltsw_l2_age_stop(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);

    L2_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_age_stop(unit));
exit:
    if (locked) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Public function to resume L2 age thread with current interval
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmi_ltsw_l2_age_resume(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit) || L2_IS_HW_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (*l2i->l2age.interval == 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    L2_LOCK(unit);
    locked = true;

    l2i->l2age.exit = false;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_age_start(unit));
exit:
    if (locked) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Public function to enable/disable global L2 learning
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1 indicates enable, 0 indicates disable.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmi_ltsw_l2_learn_set(int unit, int enable)
{
    return mbcm_ltsw_l2_learn_set(unit, enable);
}

/*!
 * \brief Public function to get global L2 learning setting
 *
 * \param [in] unit Unit number.
 * \param [out] enable Current global L2 learning setting
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmi_ltsw_l2_learn_get(int unit, int *enable)
{
    return mbcm_ltsw_l2_learn_get(unit, enable);
}

/*!
 * \brief Public function to update VLAN info used by station.
 *
 * \param [in] unit Unit number.
 * \param [in] vid VLAN id or vfi.
 * \param [in] flags Flags to indicate update which part of VLAN info.
 * \param [in] vlan_info VLAN info used by station.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmi_ltsw_l2_station_vlan_update(int unit, int vid, int flags,
                                 bcmi_ltsw_l2_station_vlan_t *vlan_info)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    /* Return 0 for no op */
    if (!flags) {
        SHR_EXIT();
    }

    L2_SC_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_vlan_update(unit, vid, flags, vlan_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_update_by_vlan(unit, vid));

exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Public function to freeze L2 H/W activities.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
int
bcmi_ltsw_l2_hw_freeze(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_LOCK(unit);

    if (l2i->l2_frz_info.hw_frozen) {
        SHR_EXIT();
    } else if (l2i->l2_frz_info.frozen == false) {
        /* Freeze L2 learning. */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_learn_freeze(unit));
        /* Pause L2 age thread. */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_age_pause(unit));
    }
    l2i->l2_frz_info.hw_frozen = true;

exit:
    if (l2i->inited) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Public function to thaw L2 H/W activities.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
int
bcmi_ltsw_l2_hw_thaw(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_LOCK(unit);

    /* Ensure L2 H/W is in frozen state. */
    if (l2i->l2_frz_info.hw_frozen == false) {
        SHR_EXIT();
    } else if (l2i->l2_frz_info.frozen == false) {
        /* Resume L2 age thread. */
        l2_age_resume(unit);
        /* Thaw L2 learning. */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_learn_restore(unit));
    }
    l2i->l2_frz_info.hw_frozen = false;

exit:
    if (l2i->inited) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Public function to get L2 H/W frozen state.
 *
 * \param [in] unit Unit number.
 * \param [out] frozen L2 H/W is frozen or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
int
bcmi_ltsw_l2_hw_is_frozen(int unit, int *frozen)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    *frozen = l2i->l2_frz_info.hw_frozen ? true : false;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_size_get(unit, size));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_count_get(int unit, int *count)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_count_get(unit, count));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_view_vlan_update(int unit, bcmi_ltsw_l2_view_vlan_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_view_vlan_update(unit, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_opaque_tag_set(int unit, int opaque_tag_id,
                            int tag_size, int ether_type,
                            int tag_type, int valid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_opaque_tag_set(unit, opaque_tag_id,
                                     tag_size, ether_type,
                                     tag_type, valid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_opaque_tag_get(int unit, int opaque_tag_id,
                            int *tag_size, int *ether_type,
                            int *tag_type, int *valid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_opaque_tag_get(unit, opaque_tag_id,
                                     tag_size, ether_type,
                                     tag_type, valid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_payload_opaque_tag_set(int unit, int opaque_tag_id,
                                    int tag_size, int ether_type,
                                    int tag_type, int valid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_payload_opaque_tag_set(unit, opaque_tag_id,
                                             tag_size, ether_type,
                                             tag_type, valid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_payload_opaque_tag_get(int unit, int opaque_tag_id,
                                    int *tag_size, int *ether_type,
                                    int *tag_type, int *valid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_payload_opaque_tag_get(unit, opaque_tag_id,
                                             tag_size, ether_type,
                                             tag_type, valid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_virtual_info_update(int unit, bcmi_ltsw_l2_virtual_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_virtual_info_update(unit, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_port_detach(int unit, bcm_port_t port)
{
    bcm_l2_addr_t match_addr;
    uint32_t repl_flags;
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &match_addr.modid));
    match_addr.port = port;
    /* Delete static address together. */
    repl_flags = BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_STATIC
               | BCM_L2_REPLACE_DELETE;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}
#if 0
int
bcmi_ltsw_l2_addr_hit_clear(int unit, bool src_hit, bool des_hit)
{
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (src_hit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_replace(unit, BCM_L2_REPLACE_SRC_HIT_CLEAR | BCM_L2_REPLACE_MATCH_STATIC,
                        BCMINT_L2_REPLACE_CLEAR_HIT_ONLY,
                        NULL, 0, 0, 0, NULL, NULL));
    }
    if (des_hit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_replace(unit, BCM_L2_REPLACE_DES_HIT_CLEAR | BCM_L2_REPLACE_MATCH_STATIC,
                        BCMINT_L2_REPLACE_CLEAR_HIT_ONLY,
                        NULL, 0, 0, 0, NULL, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
#endif
/*!
 * \brief Init L2 module.
 *
 * \param [in] unit           Unit number
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_MEMORY       Out of memory.
 */
int
bcm_ltsw_l2_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    /* Initialize L2 module software database. */
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_info_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_init(unit));

    L2_INFO(unit)->inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach L2 module.
 *
 * \param [in] unit           Unit number
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcm_ltsw_l2_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    if (L2_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    L2_LOCK(unit);
    locked = true;

    L2_INFO(unit)->inited = false;

    /* Stop L2 age thread first if any */
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_age_stop(unit));

    L2_UNLOCK(unit);
    locked = false;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_detach(unit));

    l2_info_cleanup(unit);

exit:
    if (locked) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add l2 address entry into device.
 *
 * \param [in] unit           Unit number
 * \param [in] l2addr         L2 address structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Parameter invalid.
 */
int
bcm_ltsw_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_addr_params_validate(unit, l2addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_addr_add(unit, l2addr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get l2 data structure info for given mac and vid from device.
 *
 * \param [in]  unit          Unit number
 * \param [in]  mac           Mac address.
 * \param [in]  vid           Vlan id.
 * \param [out] l2addr        L2 address structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_NOT_FOUND    Entry not found.
 */
int
bcm_ltsw_l2_addr_get(int unit, bcm_mac_t mac, bcm_vlan_t vid,
                     bcm_l2_addr_t *l2addr)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    if (l2addr == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_addr_get(unit, mac, vid, l2addr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete l2 address entry from device.
 *
 * \param [in] unit           Unit number
 * \param [in] mac            Mac address.
 * \param [in] vid            Vlan id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_NOT_FOUND    Entry not found.
 */
int
bcm_ltsw_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_addr_delete(unit, mac, vid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given mac and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  mac           Matching mac address.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_mac(int unit, bcm_mac_t mac,
                               uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_MAC;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given mac, port and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  mac           Matching mac address.
 * \param [in]  mod           Matching module id.
 * \param [in]  port          Matching port number.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_mac_port(int unit, bcm_mac_t mac,
                                    bcm_module_t mod,
                                    bcm_port_t port,
                                    uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));

    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &match_addr.modid));
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_DEST;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given port and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  mod           Matching module id.
 * \param [in]  port          Matching port number.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_port(int unit, bcm_module_t mod,
                                bcm_port_t port,
                                uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;
    uint32_t        int_flags = 0;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    if (port == -1) {
        /* non zero mod id is not used, replace will return error */
        match_addr.modid = mod;
        match_addr.port  = -1;
        int_flags = BCMINT_L2_REPLACE_IGNORE_PORTID;
    } else if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &match_addr.modid));
    } else {
        match_addr.modid = mod;
    }

    match_addr.port = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, int_flags, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given trunk id and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tid           Matching trunk id.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_trunk(int unit, bcm_trunk_t tid,
                                 uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.tgid = tid;
    match_addr.flags = BCM_L2_TRUNK_MEMBER;

    BCM_IF_ERROR_RETURN
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given vlan id and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           Matching vlan id.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_vlan(int unit, bcm_vlan_t vid,
                                uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                     NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_addr_delete_by_vlan_gport_multi(int unit,
                                            uint32_t flags,
                                            int num_pairs,
                                            bcm_vlan_t *vlan,
                                            bcm_gport_t *gport)
{
    bcmint_l2_tuple_ctrl_t tuple_ctrl;
    SHR_FUNC_ENTER(unit);

    sal_memset(&tuple_ctrl, 0, sizeof(bcmint_l2_tuple_ctrl_t));
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    /* Validate the input parameters */
    if (vlan == NULL || gport == NULL || num_pairs == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate flag parameter, no action will be proceeded with extra flags */
    if (flags & ~(BCM_L2_REPLACE_DELETE |
                  BCM_L2_REPLACE_NO_CALLBACKS |
                  BCM_L2_REPLACE_MATCH_STATIC)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (tuple_ctrl_stage(unit, &tuple_ctrl, flags, num_pairs, vlan, gport));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_addr_del_by_tuple(unit, &tuple_ctrl));
exit:
    tuple_cleanup(unit, &tuple_ctrl);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given vlan id, port and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           Matching vlan id.
 * \param [in]  mod           Matching module id.
 * \param [in]  port          Matching port number.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_vlan_port(int unit, bcm_vlan_t vid,
                                     bcm_module_t mod,
                                     bcm_port_t port,
                                     uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;

    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &match_addr.modid));
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L2 address entry matching given vlan id, trunk id and flags.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           Matching vlan id.
 * \param [in]  tid           Matching trunk id.
 * \param [in]  flags         Matching flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_delete_by_vlan_trunk(int unit,
                                      bcm_vlan_t vid,
                                      bcm_trunk_t tid,
                                      uint32_t flags)
{
    bcm_l2_addr_t   match_addr;
    uint32_t        repl_flags;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    match_addr.tgid = tid;
    match_addr.flags = BCM_L2_TRUNK_MEMBER;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                    NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Hit bits of L2 address entries.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  src_hit      Clear source hit bit.
 * \param [in]  des_hit     Clear destination hit bit.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcmi_ltsw_l2_addr_hit_clear(int unit, bool src_hit, bool des_hit)
{
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (src_hit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_replace(unit, BCM_L2_REPLACE_SRC_HIT_CLEAR | BCM_L2_REPLACE_MATCH_STATIC,
                        BCMINT_L2_REPLACE_CLEAR_HIT_ONLY,
                        NULL, 0, 0, 0, NULL, NULL));
    }
    if (des_hit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_replace(unit, BCM_L2_REPLACE_DES_HIT_CLEAR | BCM_L2_REPLACE_MATCH_STATIC,
                        BCMINT_L2_REPLACE_CLEAR_HIT_ONLY,
                        NULL, 0, 0, 0, NULL, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_ctrl_set(int unit, const char *lt_name,
                      const char *lt_fld_name, int enable)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = lt_fld_name;
    fields[0].u.val = enable ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, lt_name, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l2_ctrl_get(int unit, const char *lt_name,
                      const char *lt_fld_name, int *enable)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = lt_fld_name;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, lt_name, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, lt_name, lt_fld_name, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *enable = fields[0].u.val ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Freeze all L2 activities including learning, aging and other S/W ops.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_freeze(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_LOCK(unit);

    if (l2i->l2_frz_info.frozen) {
        SHR_EXIT();
    } else if (l2i->l2_frz_info.hw_frozen == false) {
        /* Freeze L2 operation */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_learn_freeze(unit));
        /* Pause L2 age thread. */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_age_pause(unit));
    }
    l2i->l2_frz_info.frozen = true;

exit:
    if (l2i->inited) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Thaw L2 activities from frozen.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_addr_thaw(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_LOCK(unit);

    /* Ensure L2 is in frozen state */
    if (l2i->l2_frz_info.frozen == false) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    } else if (l2i->l2_frz_info.hw_frozen == false) {
        /* Resume L2 age thread. */
        l2_age_resume(unit);
        /* Thaw L2 learning. */
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_learn_restore(unit));
    }
    l2i->l2_frz_info.frozen = false;

exit:
    if (l2i->inited) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register a callback to get notified when any change happens on L2 table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  callback      Callback function pointer.
 * \param [in]  userdata      User cookie.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_RESOURCE     Register too many callbacks.
 */
int
bcm_ltsw_l2_addr_register(int unit, bcm_l2_addr_callback_t callback,
                          void *userdata)
{
    l2_cb_ctrl_t *l2_cb_ctrl = &L2_CB(unit);
    bool locked = false;
    int i;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_LOCK(unit);
    locked = true;
    /* Search array to check if function is aready registered */
    CB_ITER(i) {
        if (l2_cb_ctrl->cb[i].fn == callback &&
            l2_cb_ctrl->cb[i].fn_data == userdata) {
            SHR_EXIT();
        }
    }
    CB_ITER(i) {
        if (l2_cb_ctrl->cb[i].fn == NULL) {
            l2_cb_ctrl->cb[i].fn = callback;
            l2_cb_ctrl->cb[i].fn_data = userdata;
            l2_cb_ctrl->cb_count++;
            break;
        }
    }

    if (i >= L2_CB_MAX) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else if (l2_cb_ctrl->cb_count > 1) {
        SHR_EXIT();
    }
    L2_UNLOCK(unit);
    locked = false;

    /*
    * Register local L2 event callback function to event manager at the first time
    */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_event_register(unit,
                                  BCMI_LTSW_EV_L2_NOTIF,
                                  &bcmint_l2_event_cb,
                                  NULL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_event_attrib_set(unit,
                                    BCMI_LTSW_EV_L2_NOTIF,
                                    BCMI_LTSW_EVENT_APPL_CALLBACK));

exit:
    if (locked) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregister a previously registered callback routine.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  callback      Callback function pointer.
 * \param [in]  userdata      User cookie.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcm_ltsw_l2_addr_unregister(int unit, bcm_l2_addr_callback_t callback,
                            void *userdata)
{
    l2_cb_ctrl_t *l2_cb_ctrl = &L2_CB(unit);
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_LOCK(unit);
    locked = true;
    CB_ITER(i) {
        if (l2_cb_ctrl->cb[i].fn == callback &&
            l2_cb_ctrl->cb[i].fn_data == userdata) {
            l2_cb_ctrl->cb[i].fn = NULL;
            l2_cb_ctrl->cb[i].fn_data = NULL;
            l2_cb_ctrl->cb_count--;
            break;
        }
    }
    if (i >= L2_CB_MAX) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else if (l2_cb_ctrl->cb_count > 0) {
        SHR_EXIT();
    }
    L2_UNLOCK(unit);
    locked = false;
    /*
    * Unregister local L2 event function from event manager when all callback functions
    * of application are unregistered
    */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_event_unregister(unit,
                                    BCMI_LTSW_EV_L2_NOTIF,
                                    &bcmint_l2_event_cb));

exit:
    if (locked) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse L2 address table and call user's callback for matched entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  trav_fn       User specified callback function.
 * \param [in]  user_data     User specified data.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_traverse(int unit, bcm_l2_traverse_cb trav_fn, void *user_data)
{
    SHR_FUNC_ENTER(unit);

    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_addr_traverse(unit, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear L2 module.
 *
 * \param [in] unit           Unit number
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_MEMORY       Out of memory.
 */
int
bcm_ltsw_l2_clear(int unit)
{
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l2_init(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Replace destination for matched entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  flags         Replace flags.
 * \param [in]  match_addr    Match l2 address structure.
 * \param [in]  new_module    New module id.
 * \param [in]  new_port      New port number.
 * \param [in]  new_trunk     New trunk group id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
int
bcm_ltsw_l2_replace(int unit, uint32_t flags, bcm_l2_addr_t *match_addr,
                    bcm_module_t new_module, bcm_port_t new_port,
                    bcm_trunk_t new_trunk)
{
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);
    if (L2_IS_FROZEN(unit)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (!flags) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE)) &&
        (flags & BCM_L2_REPLACE_NEW_TRUNK)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* The following flags are not supported */
    if ((flags & BCM_L2_REPLACE_VPN_TYPE) ||
        (flags & BCM_L2_REPLACE_VLAN_AND_VPN_TYPE) ||
        (flags & BCM_L2_REPLACE_PENDING) ||
        (flags & BCM_L2_REPLACE_IGNORE_PENDING) ||
        (flags & BCM_L2_REPLACE_LEARN_LIMIT) ||
        (flags & BCM_L2_REPLACE_AGE)) {
        SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

    if ((flags & BCM_L2_REPLACE_DELETE) &&
        (flags & BCM_L2_REPLACE_MATCH_MAC) &&
        ((flags & BCM_L2_REPLACE_MATCH_UC) ||
         (flags & BCM_L2_REPLACE_MATCH_MC))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Matching L2 address allowed to be NULL together with */
    /* BCM_L2_REPLACE_DELETE flag indicating to delete all entries */
    if (NULL == match_addr) {
        if (!(flags & BCM_L2_REPLACE_DELETE) &&
            !(flags & (BCM_L2_REPLACE_SRC_HIT_CLEAR |
                       BCM_L2_REPLACE_DES_HIT_CLEAR))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        flags &= ~(BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_MAC |
                   BCM_L2_REPLACE_MATCH_DEST);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_replace(unit, flags, 0, match_addr, new_module,
                    new_port, new_trunk, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_cache_init(int unit)
{
    return mbcm_ltsw_l2_cache_init(unit);
}

int
bcm_ltsw_l2_cache_set(int unit, int index, bcm_l2_cache_addr_t *addr,
                      int *index_used)
{
    return mbcm_ltsw_l2_cache_set(unit, index, addr, index_used);
}

int
bcm_ltsw_l2_cache_get(int unit, int index, bcm_l2_cache_addr_t *addr)
{
    return mbcm_ltsw_l2_cache_get(unit, index, addr);
}

int
bcm_ltsw_l2_cache_size_get(int unit, int *size)
{
    return mbcm_ltsw_l2_cache_size_get(unit, size);
}

int
bcm_ltsw_l2_cache_delete(int unit, int index)
{
    return mbcm_ltsw_l2_cache_delete(unit, index);
}

int
bcm_ltsw_l2_cache_delete_all(int unit)
{
    return mbcm_ltsw_l2_cache_delete_all(unit);
}

int
bcm_ltsw_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                         bcm_l2_addr_t *cf_array,
                         int cf_max, int *cf_count)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_learn_class_set(int unit, int lclass, int lclass_prio,
                            uint32_t flags)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_learn_class_get(int unit, int lclass, int *lclass_prio,
                            uint32_t *flags)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_learn_port_class_set(int unit, bcm_gport_t port,
                                 int lclass)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_learn_port_class_get(int unit, bcm_gport_t port,
                                 int *lclass)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_learn_limit_set(int unit, bcm_l2_learn_limit_t *limit)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_learn_limit_get(int unit, bcm_l2_learn_limit_t *limit)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_l2_age_timer_set(int unit, int age_seconds)
{
    int interval;
    bool enable, locked = false, frozen = false;
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    /* coverity[result_independent_of_operands] */
    if (age_seconds < 0 || age_seconds > L2_AGE_MAX_TIMER) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L2_LOCK(unit);
    locked = true;

    frozen = l2i->l2_frz_info.frozen;
    enable = age_seconds ? true : false;
    interval = *l2i->l2age.interval;
    if (*l2i->l2age.interval) {
        l2i->l2age.enable = enable;
        if (frozen) {
            l2i->l2age.enable = false;
        }
        *l2i->l2age.interval = age_seconds;
        if (interval != age_seconds) {
            sal_sem_give(l2i->l2age.notify);
        }
    } else {
        /* If age thread is not running, start it */
        if (enable) {
            l2i->l2age.enable = true;
            if (frozen) {
                l2i->l2age.enable = false;
            }
            *l2i->l2age.interval = age_seconds;
            SHR_IF_ERR_VERBOSE_EXIT
                (l2_age_start(unit));
        }
    }

exit:
    if (locked) {
        L2_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_age_timer_get(int unit, int *age_seconds)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    *age_seconds = l2i->l2age.pid != SAL_THREAD_ERROR &&
                   l2i->l2age.enable ?
                   *l2i->l2age.interval : 0;
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_station_add(int unit, int *station_id, bcm_l2_station_t *station)
{
    int sid = -1;
    bool alloc = false, locked = false, tnl = false;
    bool overlay = false, use_underlay_flag;
    int int_flags = 0;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_params_validate(unit, station_id, station));

    L2_SC_LOCK(unit);
    locked = true;

    /* Search database */
    if (SHR_SUCCESS(l2_station_search(unit, station, &tnl, &sid))) {
        if (!((station->flags & BCM_L2_STATION_WITH_ID) &&
              (station->flags & BCM_L2_STATION_REPLACE) &&
              (sid == *station_id))) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        if (station->flags & BCM_L2_STATION_REPLACE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
    overlay = HSDK_L2_STATION_OVERLAY(station);

    if (station->flags & BCM_L2_STATION_WITH_ID) {
        sid = *station_id;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (l2_station_id_alloc(unit, overlay, &sid));
        alloc = true;
        *station_id = sid;
    }
    if (sid >= UNDERLAY_STATION_SIZE(unit)) {
        int_flags |= BCMINT_L2_STATION_OVERLAY;
    }
    use_underlay_flag = (station->flags & BCM_L2_STATION_UNDERLAY) ? true : false;

    if (overlay == 0 && HYBRID_STATION_SIZE(unit) > 0) {
        station->flags |= BCM_L2_STATION_UNDERLAY;
    }
    /* Add entry to LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_add(unit, station, int_flags));

    if (overlay == 0 && use_underlay_flag == 0) {
        station->flags &= ~BCM_L2_STATION_UNDERLAY;
    }
    /* Add entry to database */
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_add(unit, sid, station, false));

exit:
    if (SHR_FUNC_ERR()) {
        if (alloc) {
            l2_station_id_free(unit, sid);
        }
    }
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_station_get(int unit, int station_id, bcm_l2_station_t *station)
{
    bool locked = false;
    int int_flags = 0;
    bool use_underlay_flag = false;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_SC_LOCK(unit);
    locked = true;

    /* Get key from database */
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_get(unit, station_id, station, &int_flags, &use_underlay_flag));

    /* Fetch completed entry from LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_get(unit, station, int_flags));

    if (use_underlay_flag == false) {
        station->flags &= ~BCM_L2_STATION_UNDERLAY;
    }

exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_station_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    *size = HYBRID_STATION_SIZE(unit) + UNDERLAY_STATION_SIZE(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn,
                             void *user_data)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_SC_LOCK(unit);
    locked = true;
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_traverse(unit, trav_fn, user_data));
exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_station_delete(int unit, int station_id)
{
    bcm_l2_station_t station;
    bool locked = false;
    int int_flags = 0;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_SC_LOCK(unit);
    locked = true;

    /* Get key from database */
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_get(unit, station_id, &station, &int_flags, NULL));

    /* Delete entry from database */
    l2_station_id_free(unit, station_id);

    /* Delete entry from LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_delete(unit, &station, int_flags));
exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_station_delete_all(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_SC_LOCK(unit);
    locked = true;
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_delete_all(unit, false));

exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    int sid = -1, flags = BCMINT_L2_STATION_TUNNEL;
    bcm_l2_station_t station;
    bool alloc = false, locked = false, tnl = false;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    /* Validate vlan */
    if (vlan > BCM_VLAN_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L2_SC_LOCK(unit);
    locked = true;

    bcm_l2_station_t_init(&station);
    sal_memcpy(station.dst_mac, mac, sizeof(bcm_mac_t));
    sal_memset(station.dst_mac_mask, 0xff, sizeof(bcm_mac_t));
    station.vlan = vlan;
    station.vlan_mask = 0xfff;

    /* Search database */
    if (SHR_SUCCESS(l2_station_search(unit, &station, &tnl, &sid))) {
        if (tnl) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_id_alloc(unit, 0, &sid));
    alloc = true;

    if (sid >= UNDERLAY_STATION_SIZE(unit)) {
        flags |= BCMINT_L2_STATION_OVERLAY;

        station.flags |= BCM_L2_STATION_UNDERLAY;
    }
    /* Add entry to LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_add(unit, &station, flags));

    station.flags &= ~BCM_L2_STATION_UNDERLAY;

    /* Add entry to database */
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_add(unit, sid, &station, true));

exit:
    if (SHR_FUNC_ERR()) {
        if (alloc) {
            l2_station_id_free(unit, sid);
        }
    }
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    bcm_l2_station_t station;
    int sid = -1, int_flags = 0;
    bool locked = false, tnl = false;

    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    /* Validate vlan */
    if (vlan > BCM_VLAN_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L2_SC_LOCK(unit);
    locked = true;

    bcm_l2_station_t_init(&station);
    sal_memcpy(station.dst_mac, mac, sizeof(bcm_mac_t));
    sal_memset(station.dst_mac_mask, 0xff, sizeof(bcm_mac_t));
    station.vlan = vlan;
    station.vlan_mask = 0xfff;

    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_search(unit, &station, &tnl, &sid));
    /* Can not delete entries added by station_add API */
    if (!tnl) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Delete entry from database */
    l2_station_id_free(unit, sid);

    if (sid >= UNDERLAY_STATION_SIZE(unit)) {
        int_flags |= BCMINT_L2_STATION_OVERLAY;

        station.flags |= BCM_L2_STATION_UNDERLAY;
    }
    /* Delete entry from LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_station_delete(unit, &station, int_flags));
exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l2_tunnel_delete_all(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);
    L2_INIT_CHECK(unit);

    L2_SC_LOCK(unit);
    locked = true;
    SHR_IF_ERR_VERBOSE_EXIT
        (l2_station_delete_all(unit, true));
exit:
    if (locked) {
        L2_SC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

void
bcmi_ltsw_l2_sw_dump(int unit)
{
    ltsw_l2_info_t *l2i = L2_INFO(unit);

    if (!l2i->inited) {
        LOG_CLI((BSL_META_U(unit,
                            "\nL2 module isn't initialized: Unit %d\n"),
                            unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information L2 - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init                : %8d\n"), l2i->inited));
    LOG_CLI((BSL_META_U(unit,
                        "  Mutex               : %p\n"),
                        (void *)l2i->mutex));
    LOG_CLI((BSL_META_U(unit,
                        "  Station Mutex       : %p\n"),
                        (void *)l2i->sc_mutex));
    LOG_CLI((BSL_META_U(unit,
                        "  Underlay station size        : %8d\n"), l2i->underlay_tbl_sz));
    LOG_CLI((BSL_META_U(unit,
                        "  Hybrid station size        : %8d\n"), l2i->hybrid_tbl_sz));
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Dump L2 age info. */
    l2_age_info_dump(unit);

    /* Dump L2 callback info. */
    l2_cb_info_dump(unit);

    /* Dump chip specific L2 info. */
    mbcm_ltsw_l2_sw_dump(unit);
    return;
}

int
bcm_ltsw_l2_port_native(int unit, int modid, int port)
{
    bcm_trunk_t     tgid;
    int             id, islocal, rv;
    uint32_t        flags = 0;

    rv = bcmi_ltsw_modid_is_local(unit, modid, &islocal);
    if (SHR_FAILURE(rv) || islocal != TRUE) {
        return FALSE;
    }
    if (BCM_GPORT_IS_SET(port)) {
        rv = bcmi_ltsw_port_gport_resolve(unit, port, &modid,
                                          &port, &tgid, &id);

        if (SHR_FAILURE(rv) || -1 != id || BCM_TRUNK_INVALID != tgid) {
            return FALSE;
        }
    }
    rv = bcm_ltsw_stk_port_get(unit, port, &flags);
    if (SHR_FAILURE(rv) || (flags & (BCM_STK_ENABLE | BCM_STK_INACTIVE))) {
        return FALSE;
    }

    return TRUE;
}

int
bcm_ltsw_l2_key_dump(int unit, char *pfx, bcm_l2_addr_t *entry, char *sfx)
{
    LOG_CLI((BSL_META_U(unit,
                        "l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
             "%02x%02x%02x%s"), pfx, entry->vid,
             entry->mac[0], entry->mac[1], entry->mac[2],
             entry->mac[3], entry->mac[4], entry->mac[5], sfx));

    return BCM_E_NONE;
}

