/*! \file stg.c
 *
 * STG management.
 * This file contains the management for STG.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcm/types.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/stg.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/stg_int.h>

#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_STG

/* STG user type. */
#define LTSW_STG_USER_VLAN         1
#define LTSW_STG_USER_VPN          2

/* Invalid user id. */
#define LTSW_STG_USER_INVALID_IDX  -1

/* VLAN STG information. */
typedef struct ltsw_stg_info_s {
    /* TRUE if STG module has been initialized. */
    int initialized;

    /* STG table min index. */
    bcm_stg_t stg_min;

    /* STG table max index. */
    bcm_stg_t stg_max;

    /* Default STG. */
    bcm_stg_t *def_stg;

    /* Bitmap of allocated STGs. */
    SHR_BITDCL *stg_bmp;

    /*
     * STG map -
     * keep a linked list of users(VLAN/VPN) in each STG.
     * uidx_first: Indexed by STG ID.
     * uidx_first[stg] is the first user(VLAN/VPN) index in a STG.
     */
    int *uidx_first;

    /*
     * uidx_next: Indexed by stg user index.
     * uidx_next[user_idx] contains the next user(VLAN/VPN) index in the same
     * STG.
     */
    int *uidx_next;

    /* User count of VLAN. */
    int vlan_count;

    /* User count of VPN. */
    int vpn_count;

    /* Offset of VPN in user index. */
    int vpn_offset;

    /* Protection mutex. */
    sal_mutex_t mutex;
} ltsw_stg_info_t;

static ltsw_stg_info_t ltsw_stg_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* STG information. */
#define STG_INFO(u) (&ltsw_stg_info[u])

/* Check if STG module is initialized. */
#define STG_INIT_CHECK(u)                    \
    do {                                     \
        ltsw_stg_info_t *info = STG_INFO(u); \
        if (!info->initialized) {            \
            SHR_ERR_EXIT(SHR_E_INIT); \
        }                                    \
    } while(0)

/* Check STG ID. */
#define STG_ID_CHECK(u, stg)                      \
    do {                                          \
        ltsw_stg_info_t *info = STG_INFO(u);      \
        if ((stg) < 0 || (stg) > info->stg_max) { \
            SHR_ERR_EXIT(SHR_E_BADID);     \
        }                                         \
    } while(0)

/* STG Lock. */
#define STG_LOCK(u) \
    sal_mutex_take(STG_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define STG_UNLOCK(u) \
    sal_mutex_give(STG_INFO(u)->mutex)

/******************************************************************************
* Private functions
 */

/*!
 * \brief Retrieve STG user index per STG user and type.
 *
 * This function is used to retrieve STG user index per STG user and type.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  user                 STG user ID.
 * \param [in]  user_type            STG user type.
 *
 * \retval value                     STG User index.
 */
static int
stg_user_idx_get(int unit, int user, int user_type)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int uidx = 0;

    switch (user_type) {
    case LTSW_STG_USER_VPN:
        BCMI_LTSW_VIRTUAL_VPN_GET(uidx,
                                  BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI,
                                  user);
        uidx = uidx + si->vlan_count - si->vpn_offset;
        break;
    default:
        uidx = user;
        break;
    }

    return uidx;
}

/*!
 * \brief Retrieve STG user type per user index.
 *
 * This function is used to retrieve STG user type from user index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  user_idx             STG user index.
 *
 * \retval value                     STG user type.
 */
static int
stg_user_type_get(int unit, int user_idx)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int user_type = LTSW_STG_USER_VLAN;
    int max_vlan_uidx = 0;
    int max_vpn_uidx = 0;

    max_vlan_uidx = si->vlan_count - 1;
    max_vpn_uidx = si->vlan_count + si->vpn_count - 1;

    if (si->vlan_count) {
        if (user_idx <= max_vlan_uidx) {
            user_type = LTSW_STG_USER_VLAN;
            return user_type;
        }
    }

    if (si->vpn_count) {
        if ((user_idx > max_vlan_uidx) && (user_idx <= max_vpn_uidx)) {
            user_type = LTSW_STG_USER_VPN;
            return user_type;
        }
    }

    return user_type;
}

/*!
 * \brief Retrieve STG user per STG user index.
 *
 * This function is used to retrieve STG user per STG user index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  user_idx             STG user index.
 *
 * \retval value                     STG user ID.
 */
static int
stg_user_get(int unit, int user_idx)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int utype = 0;
    int user = 0;

    utype = stg_user_type_get(unit, user_idx);

    switch (utype) {
    case LTSW_STG_USER_VPN:
        user = user_idx - si->vlan_count + si->vpn_offset;
        BCMI_LTSW_VIRTUAL_VPN_SET(user,
                                  BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI,
                                  user);
        break;
    default:
        user = user_idx;
        break;
    }

    return user;
}

/*!
 * \brief Add user to STG linked list.
 *
 * This function is used to add user to STG linked list.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 * \param [in]  vid                  VLAN or VPN.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
stg_user_map_add(int unit, bcm_stg_t stg, int user, int user_type)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int uidx = 0;

    uidx = stg_user_idx_get(unit, user, user_type);
    si->uidx_next[uidx] = si->uidx_first[stg];
    si->uidx_first[stg]= uidx;

    return;
}

/*!
 * \brief Remove user from STG linked list.
 *
 * This function is used to remove user from STG linked list.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 * \param [in]  vid                  VLAN/VPN ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
stg_user_map_delete(int unit, bcm_stg_t stg, int user, int user_type)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int *uidx_local = NULL;
    int uidx = 0;

    uidx = stg_user_idx_get(unit, user, user_type);
    uidx_local = &si->uidx_first[stg];

    while (*uidx_local != LTSW_STG_USER_INVALID_IDX) {
        if (*uidx_local == uidx) {
            *uidx_local = si->uidx_next[*uidx_local];
        } else {
            uidx_local = &si->uidx_next[*uidx_local];
        }
    }

    return;
}

/*!
 * \brief Cleanup the VLAN STG information structures.
 *
 * This function is used to cleanup the VLAN STG information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
stg_info_cleanup(int unit)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    /* Destory mutex. */
    if (si->mutex) {
        sal_mutex_destroy(si->mutex);
        si->mutex = NULL;
    }

    /* Cleanup the resource of STG bitmap. */
    SHR_FREE(si->stg_bmp);
    SHR_FREE(si->uidx_first);
    SHR_FREE(si->uidx_next);

    sal_memset(si, 0, sizeof(ltsw_stg_info_t));

    return;
}

/*!
 * \brief Initialize the VLAN STG information structures.
 *
 * This function is used to initialize the VLAN STG information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_info_init(int unit)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int wb_flag = bcmi_warmboot_get(unit);
    uint32_t alloc_size = 0;
    int user_count = 0;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Cleanup previous resource. */
    if (si->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stg_detach(unit));
    }

    /* Create mutex. */
    if (NULL == si->mutex) {
        si->mutex = sal_mutex_create("STG mutex");
        SHR_NULL_CHECK(si->mutex, SHR_E_MEMORY);
    }

    /* Dispatch for VLAN init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_size_get(unit,
                                &si->stg_min,
                                &si->stg_max,
                                &si->vlan_count,
                                &si->vpn_count,
                                &si->vpn_offset));

    /* Allocate the resource of VLAN STG bitmap. */
    alloc_size = si->stg_max + 1;
    SHR_ALLOC(si->stg_bmp,
              alloc_size,
              "ltswstgBmp");
    SHR_NULL_CHECK(si->stg_bmp, SHR_E_MEMORY);
    sal_memset(si->stg_bmp, 0, alloc_size);

    /* Allocate the resource of first user(VLAN/VPN etc.) in a STG.  */
    alloc_size = (si->stg_max + 1) * sizeof(int);
    SHR_ALLOC(si->uidx_first,
              alloc_size,
              "ltswstgVfirst");
    SHR_NULL_CHECK(si->uidx_first, SHR_E_MEMORY);
    for (idx = 0; idx < (si->stg_max + 1); idx++) {
        si->uidx_first[idx] = LTSW_STG_USER_INVALID_IDX;
    }

    /* Allocate the resource of next user(VLAN/VPN etc.) in a STG. */
    user_count = si->vlan_count + si->vpn_count;
    alloc_size = user_count * sizeof(int);
    SHR_ALLOC(si->uidx_next,
              alloc_size,
              "ltswstgVnext");
    SHR_NULL_CHECK(si->uidx_next, SHR_E_MEMORY);
    for (idx = 0; idx < user_count; idx++) {
        si->uidx_next[idx] = LTSW_STG_USER_INVALID_IDX;
    }

    /* Allocate HA resource for default STG. */
    alloc_size = sizeof(bcm_stg_t);
    si->def_stg = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_STG,
                                         BCMINT_STG_SUB_COMP_ID_HA_DEF,
                                         "bcmStgDflt",
                                         &alloc_size);
    SHR_NULL_CHECK(si->def_stg, SHR_E_MEMORY);

    /* Set default STG. */
    if (!wb_flag) {
        *(si->def_stg) = BCM_STG_DEFAULT;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!wb_flag && si->def_stg) {
            (void) bcmi_ltsw_ha_mem_free(unit, si->def_stg);
            si->def_stg = NULL;
        }
        stg_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a specified STG.
 *
 * This function is used to create a specified STG.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_create_with_id(int unit, bcm_stg_t stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Check if STG is existing. */
    if (SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
    }

    /* Create STG with all ports with DISABLED STP state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_stp_init(unit, stg));
    SHR_BITSET(si->stg_bmp, stg);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a STG.
 *
 * This function is used to create a STG.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_create(int unit, bcm_stg_t *stg_ptr)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_stg_t stg = 0;

    SHR_FUNC_ENTER(unit);

    /* Get a free STG. */
    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (!SHR_BITGET(si->stg_bmp, stg)) {
            break;
        }
    }

    /* Can not find a free STG. */
    if (stg > si->stg_max) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
    }

    /* Create STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (stg_create_with_id(unit, stg));

    *stg_ptr = stg;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a VLAN into a specified STG.
 *
 * This function is used to add a VLAN into a specified STG.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 * \param [in]  vid                  VLAN/VPN ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_stg_t old_stg = 0;
    int utype = 0;

    SHR_FUNC_ENTER(unit);

    /* Check if STG is existing. */
    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get STG per VLAN/VPN. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_get(unit, vid, &old_stg));

    /* Set VLAN/VPN into STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_set(unit, vid, stg));

    /* Set VLAN/STG Mapping list. */
    utype = BCMI_LTSW_VIRTUAL_VPN_IS_SET(vid) ?
            LTSW_STG_USER_VPN : LTSW_STG_USER_VLAN;
    stg_user_map_delete(unit, old_stg, vid, utype);
    stg_user_map_add(unit, stg, vid, utype);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a VLAN from a specified STG.
 *
 * This function is used to remove a VLAN from a specified STG.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 * \param [in]  vid                  VLAN/VPN ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_stg_t old_stg  = 0;
    int utype = 0;

    SHR_FUNC_ENTER(unit);

    /* Check if STG is existing. */
    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get STG per VLAN/VPN.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_get(unit, vid, &old_stg));
    if (stg != old_stg) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Set VLAN/VPN into default STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_set(unit, vid, *(si->def_stg)));

    /* Set VLAN/STG Mapping list. */
    utype = BCMI_LTSW_VIRTUAL_VPN_IS_SET(vid) ?
            LTSW_STG_USER_VPN : LTSW_STG_USER_VLAN;
    stg_user_map_delete(unit, stg, vid, utype);
    stg_user_map_add(unit, *(si->def_stg), vid, utype);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove all VLAN from a specified STG.
 *
 * This function is used to remove all VLAN from a specified STG, and put all
 * the VLANs into the default STG.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_vlan_remove_all(int unit, bcm_stg_t stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_vlan_t vlan_vpn = 0;
    int uidx = 0;

    SHR_FUNC_ENTER(unit);

    /* Check if STG is existing. */
    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Traverse all VLAN/VFI to be removed. */
    while ((uidx = si->uidx_first[stg]) != LTSW_STG_USER_INVALID_IDX) {

        /* Get VLAN or VPN. */
        vlan_vpn = stg_user_get(unit, uidx);

        /* Remove the VLAN/VPN from STG. */
        SHR_IF_ERR_VERBOSE_EXIT
            (stg_vlan_remove(unit, stg, vlan_vpn));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a VLAN from STG when the VLAN is destroyed.
 *
 * This function is used to remove a VLAN from STG when the VLAN is destroyed.
 * And the VLAN will NOT placed in the default spanning tree group.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN/VPN ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_vlan_destroy(int unit, bcm_vlan_t vid)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_stg_t stg = 0;
    int utype = 0;

    SHR_FUNC_ENTER(unit);

    /* Get VLAN STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_get(unit, vid, &stg));

    /* Remove VLAN from the STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (stg_vlan_remove(unit, stg, vid));

    /* VLAN should not be in the mapping of default STG. */
    utype = BCMI_LTSW_VIRTUAL_VPN_IS_SET(vid) ?
            LTSW_STG_USER_VPN : LTSW_STG_USER_VLAN;
    stg_user_map_delete(unit, *(si->def_stg), vid, utype);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a specified STG.
 *
 * This function is used to destroy a specified STG.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  stg                  STG index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_destroy(int unit, bcm_stg_t stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Check if STG is existing. */
    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Remove all VLAN/VPN from the STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (stg_vlan_remove_all(unit, stg));

    /* Destroy STG with DISABLED STP state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_stp_clear(unit, stg));
    SHR_BITCLR(si->stg_bmp, stg);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Return a list of defined STGs.
 *
 * This function is used to return a list of VLANs in a specified STG.
 *
 * \param [in]    unit   Unit number.
 * \param [out]   list   STG array.
 * \param [out]   count  Number of entries in array will be stored.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
stg_list(int unit, bcm_stg_t **list, int *count)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int alloc_size = 0;
    int stg_count = 0;
    bcm_stg_t stg;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    *count = 0;
    *list = NULL;

    SHR_BITCOUNT_RANGE(si->stg_bmp, stg_count,
                       si->stg_min, (si->stg_max - si->stg_min + 1));

    if (stg_count == 0) {
        SHR_EXIT();
    }

    *count = stg_count;
    alloc_size = stg_count * sizeof(bcm_stg_t);

    SHR_ALLOC(*list, alloc_size, "ltswstgList");
    SHR_NULL_CHECK(*list, SHR_E_MEMORY);

    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (SHR_BITGET(si->stg_bmp, stg)) {
            (*list)[i++] = stg;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare VLAN or VPN.
 *
 * This function is used to compare VLAN or VPN.
 *
 * \param [in] a    First VLAN or VPN.
 * \param [in] b    Second VLAN or VPN.
 *
 * \retval -1       a is less than b.
 * \retval 0        a equals b.
 * \retval 1        a is more than b.
 */
static int
stg_vid_compare(const void *a, const void *b)
{
    bcm_vlan_t vid_a, vid_b;

    vid_a = *(bcm_vlan_t *)a;
    vid_b = *(bcm_vlan_t *)b;

    if (vid_a < vid_b) {
        return -1;
    }

    if (vid_a > vid_b) {
        return 1;
    }

    return 0;
}

/*!
 * \brief Return a list of VLANs in a specified STG.
 *
 * This function is used to return a list of VLANs in a specified STG.
 *
 * \param [in]    unit   Unit number.
 * \param [in]    stg    STG ID to list.
 * \param [out]   list   VLAN array.
 * \param [out]   count  Number of entries in array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
stg_vlan_list(int unit, bcm_stg_t stg, bcm_vlan_t **list, int *count)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_vlan_t vlan_vpn = 0;
    int alloc_size = 0;
    int uidx = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* Check if STG is existing. */
    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *count = 0;
    *list = NULL;

    uidx = si->uidx_first[stg];

    while (uidx != LTSW_STG_USER_INVALID_IDX) {
        (*count)++;
        uidx = si->uidx_next[uidx];
    }

    if (*count == 0) {
        SHR_EXIT();
    }

    alloc_size = (*count) * sizeof(bcm_vlan_t);

    SHR_ALLOC(*list, alloc_size, "ltswstgVidList");
    SHR_NULL_CHECK(*list, SHR_E_MEMORY);

    uidx = si->uidx_first[stg];

    while (uidx != LTSW_STG_USER_INVALID_IDX) {

        vlan_vpn = stg_user_get(unit, uidx);
        (*list)[i++] = vlan_vpn;

        uidx = si->uidx_next[uidx];
    }

    sal_qsort(*list, *count, sizeof(bcm_vlan_t), stg_vid_compare);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STP state for a port in STG containing the specified VLAN or VPN.
 *
 * This function is used to set STP state for a port in STG containing the
 * specified VLAN or VPN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID or VPN ID.
 * \param [in]  port                 Port number.
 * \param [in]  stp_state            Spanning Tree State of port.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_vlan_stp_set(int unit, bcm_vlan_t vid, bcm_port_t port, int stp_state)
{
    bcm_stg_t stg = 0;

    SHR_FUNC_ENTER(unit);

    /* Get STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_get(unit, vid, &stg));

    /* Set STP state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_stp_set(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STP state for a port in STG containing the specified VLAN or VPN.
 *
 * This function is used to get STP state for a port in STG containing the
 * specified VLAN or VPN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID or VPN ID.
 * \param [in]  port                 Port number.
 * \param [in]  stp_state            Spanning Tree State of port.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_vlan_stp_get(int unit, bcm_vlan_t vid, bcm_port_t port, int *stp_state)
{
    bcm_stg_t stg = 0;

    SHR_FUNC_ENTER(unit);

    /* Get STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_vlan_get(unit, vid, &stg));

    /* Get STP state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_stp_get(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the spanning tree state for a port in specified STG.
 *
 * This function is used to get the STP state for a port in specified STG.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID.
 * \param [in]    port        Port number.
 * \param [out]   stp_state   STP state.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get STP state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_stp_get(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the spanning tree state for a port in specified STG.
 *
 * This function is used to set the STP state for a port in specified STG.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID.
 * \param [in]    port        Port number.
 * \param [in]    stp_state   Spanning Tree State of port.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!SHR_BITGET(si->stg_bmp, stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Set STP state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_stp_set(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STP state on all STGs containing the specified port.
 *
 * This function is used to set STP state on all STGs containing the specified
 * port.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 Port number.
 * \param [in]  stp_state            Spanning Tree State of port.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
stg_stp_set_all(int unit, bcm_port_t port, int stp_state)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    bcm_stg_t stg = 0;

    SHR_FUNC_ENTER(unit);

    /* Traverse all STG to be update. */
    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (SHR_BITGET(si->stg_bmp, stg)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_stg_stp_set(unit, stg, port, stp_state));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize the STG module according to Initial System Configuration.
 *
 * \param [in]    unit     Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_init(int unit)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Initialize STG information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (stg_info_init(unit));

    /* Dispatch for STG init */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_stg_init(unit), SHR_E_UNAVAIL);

    if (bcmi_warmboot_get(unit)) {
        /* Recover SW state (STG usage bitmap and map) during warm boot. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_stg_reinit(unit, si->stg_bmp,
                                  si->uidx_first, si->uidx_next));
    } else {
        /* Legacy behavior - Reserve STG 0. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_stg_stp_init(unit, 0));
        SHR_BITSET(si->stg_bmp, 0);

        /* Create default STG. */
        SHR_IF_ERR_VERBOSE_EXIT
            (stg_create_with_id(unit, *(si->def_stg)));
        stg_user_map_add(unit, *(si->def_stg),
                         BCM_VLAN_DEFAULT, LTSW_STG_USER_VLAN);
    }

    si->initialized = 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all STGs.
 *
 * \param [in]      unit             Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stg_init(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a STG, using a specified ID.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      stg              STG to create.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_create_id(int unit, bcm_stg_t stg)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters.*/
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    /* Create STG. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_create_with_id(unit, stg));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a STG, picking an unused ID and returning it.
 *
 * \param [in]      unit             Unit number.
 * \param [out]     stg_ptr          STG ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_create(int unit, bcm_stg_t *stg_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters.*/
    STG_INIT_CHECK(unit);
    SHR_NULL_CHECK(stg_ptr, SHR_E_PARAM);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_create(unit, stg_ptr));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an STG.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      stg              STG ID to be destroyed.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_destroy(int unit, bcm_stg_t stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Check parameters.*/
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    /* Legacy behavior - STG 0 should not be destroyed. */
    if (stg < si->stg_min) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BADID);
    }

    /* Legacy behavior - Default STG should not be destroyed. */
    if (stg == *(si->def_stg)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Destroy STG. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_destroy(unit, stg));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return a list of defined STGs.
 *
 * \param [in]    unit   Unit number.
 * \param [out]   list   Place where pointer to return array will be stored.
 *                       which will be NULL if there are zero STGs returned.
 * \param [out]   count  Place where number of entries in array will be stored,
 *                       which will be 0 if there are zero STGs returned.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_list(int unit, bcm_stg_t **list, int *count)
{
    SHR_FUNC_ENTER(unit);

    STG_INIT_CHECK(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);
    SHR_NULL_CHECK(list, SHR_E_PARAM);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_list(unit, list, count));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a list returned by bcm_stg_list.
 *
 * \param [in]    unit   Unit number.
 * \param [in]    list   Place where pointer to return array will be stored.
 *                       which will be NULL if there are zero STGs returned.
 * \param [in]    count  Place where number of entries in array will be stored,
 *                       which will be 0 if there are zero STGs returned.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_list_destroy(int unit, bcm_stg_t *list, int count)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(count);

    SHR_FREE(list);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Return a list of VLANs in a specified STG.
 *        The caller is responsible for freeing the memory that is
 *        returned, using bcm_stg_vlan_list_destroy().
 *
 * \param [in]    unit   Unit number.
 * \param [in]    stg    STG ID to list.
 * \param [out]   list   Place where pointer to return array will be stored,
 *                       which will be NULL if there are zero STGs returned.
 * \param [out]   count  Place where number of entries in array will be stored,
 *                       which will be 0 if there are zero STGs returned.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_vlan_list(int unit, bcm_stg_t stg, bcm_vlan_t **list, int *count)
{
    SHR_FUNC_ENTER(unit);

    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);
    SHR_NULL_CHECK(list, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_list(unit, stg, list, count));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a list returned by bcm_stg_vlan_list.
 *
 * \param [in]    unit   Unit number.
 * \param [in]    list   Pointer to VLAN array to be destroyed.
 * \param [in]    count  Number of entries in array.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_vlan_list_destroy(int unit, bcm_vlan_t *list, int count)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(count);

    SHR_FREE(list);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Returns the default STG for the chip, usually 0 or 1 depending
 *        on the architecture.
 *
 * \param [in]    unit      Unit number.
 * \param [out]   stg_ptr   STG ID for default.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_default_get(int unit, bcm_stg_t *stg_ptr)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    SHR_NULL_CHECK(stg_ptr, SHR_E_PARAM);

    *stg_ptr = *(si->def_stg);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Changes the default STG for the chip.
 *
 * \param [in]    unit      Unit number.
 * \param [in]    stg       STG ID for default.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_default_set(int unit, bcm_stg_t stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    STG_LOCK(unit);

    /* STG should be in use. */
    if (!SHR_BITGET(si->stg_bmp, stg)) {
        STG_UNLOCK(unit);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Set STG. */
    *(si->def_stg) = stg;

    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Spanning tree state for a port in specified STG.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID.
 * \param [in]    port        Port number.
 * \param [out]   stp_state   STG ID for defaultPointer to
 *                            where Spanning Tree State is stored.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);
    SHR_NULL_CHECK(stp_state, SHR_E_PARAM);

    /* Get STP state. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_stp_get(unit, stg, port, stp_state));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the Spanning tree state for a port in specified STG.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID.
 * \param [in]    port        Port number.
 * \param [in]    stp_state   Spanning Tree State of port.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    /* Set STP state. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_stp_set(unit, stg, port, stp_state));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a VLAN to a spanning tree group.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID to use.
 * \param [in]    vid         VLAN id to be added to STG.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    /* Add a VLAN or VPN into STG. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_add(unit, stg, vid));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a VLAN from a spanning tree group.
 *        The VLAN is placed in the default spanning tree group.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID.
 * \param [in]    vid         VLAN id to be added to STG.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    /* Delete a VLAN or VPN from STG. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_remove(unit, stg, vid));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove all VLAN from a spanning tree group.
 *        The VLANs are placed in the default spanning tree group.
 *
 * \param [in]    unit        Unit number.
 * \param [in]    stg         STG ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_vlan_remove_all(int unit, bcm_stg_t stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);
    STG_ID_CHECK(unit, stg);

    /* Legacy behavior - NULL operation for default STG. */
    if (stg == *(si->def_stg)) {
        SHR_EXIT();
    }

    /* Delete a VLAN/VPN from STG. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_remove_all(unit, stg));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum number of STG groups the chip supports.
 *
 * \param [in]    unit        Unit number.
 * \param [out]   max_stg     max number of STG groups supported by this chip.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_stg_count_get(int unit, int *max_stg)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    STG_INIT_CHECK(unit);
    SHR_NULL_CHECK(max_stg, SHR_E_PARAM);

    *max_stg = si->stg_max;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the STG module.
 *
 * This function is used to de-initialize the STG module.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_stg_detach(int unit)
{
    ltsw_stg_info_t *si = STG_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (si->initialized == 0) {
        SHR_EXIT();
    }

    STG_LOCK(unit);

    si->initialized = 0;

    /* Dispatch for STG detach. */
    SHR_IF_ERR_CONT
        (mbcm_ltsw_stg_detach(unit, *(si->def_stg)));

    STG_UNLOCK(unit);

    /* Cleanup the STG information. */
    stg_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STG for a specified VLAN.
 *
 * This function is used to get STG for a specified VLAN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN/VPN ID.
 * \param [out] stg                  STG ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_stg_vlan_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);

    /* Get VLAN STG. */
    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_stg_vlan_get(unit, vid, stg));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a VLAN from STG when the VLAN is destroyed.
 *
 * This function is used to remove a VLAN from STG when the VLAN is destroyed.
 * And the VLAN will NOT placed in the default spanning tree group.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN/VPN ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_stg_vlan_destroy(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    /* Check parameters. */
    STG_INIT_CHECK(unit);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_destroy(unit, vid));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STP state on all STGs containing the specified port.
 *
 * This function is used to set STP state on all STGs containing the specified
 * port.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 Port number.
 * \param [in]  stp_state            Spanning Tree State of port.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_stg_stp_set_all(int unit, bcm_port_t port, int stp_state)
{
    SHR_FUNC_ENTER(unit);

    STG_INIT_CHECK(unit);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_stp_set_all(unit, port, stp_state));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STP state for a port in STG containing the specified VLAN or VPN.
 *
 * This function is used to set STP state for a port in STG containing the
 * specified VLAN or VPN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID or VPN ID.
 * \param [in]  port                 Port number.
 * \param [in]  stp_state            Spanning Tree State of port.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_stg_vlan_stp_set(int unit,
                           bcm_vlan_t vid,
                           bcm_port_t port,
                           int stp_state)
{
    SHR_FUNC_ENTER(unit);

    STG_INIT_CHECK(unit);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_stp_set(unit, vid, port, stp_state));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STP state for a port in STG containing the specified VLAN or VPN.
 *
 * This function is used to get STP state for a port in STG containing the
 * specified VLAN or VPN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID or VPN ID.
 * \param [in]  port                 Port number.
 * \param [in]  stp_state            Spanning Tree State of port.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_stg_vlan_stp_get(int unit,
                           bcm_vlan_t vid,
                           bcm_port_t port,
                           int *stp_state)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stp_state, SHR_E_PARAM);

    STG_INIT_CHECK(unit);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (stg_vlan_stp_get(unit, vid, port, stp_state));
    STG_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port STP state for default VLAN to DISABLE.
 *
 * This function is used to set port STP state for default VLAN to DISABLE.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_stg_port_detach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_vlan_stp_set(unit, BCM_VLAN_DEFAULT,
                                    port, BCM_STG_STP_DISABLE));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port STP state for default VLAN to FORWARDING.
 *
 * This function is used to set port STP state for default VLAN to FORWARDING.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_stg_port_attach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_vlan_stp_set(unit, BCM_VLAN_DEFAULT,
                                    port, BCM_STG_STP_FORWARD));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STG check on a given port.
 *
 * This function is used to set STG check on a given port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [in] disable           Disable or enable.
 * \param [in] flags             Direction flags.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_stg_port_check_set(int unit, bcm_port_t port,
                             int disable, uint16_t flags)
{
    SHR_FUNC_ENTER(unit);

    STG_INIT_CHECK(unit);

    STG_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_stg_port_check_set(unit, port, disable, flags));
    STG_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Display STG software structure information.
 *
 * This function is used to display STG software structure information.
 *
 * \retval None
 */
void
bcmi_ltsw_stg_sw_dump(int unit)
{
    ltsw_stg_info_t *si = STG_INFO(unit);
    int count = 0;
    int uidx = 0;
    int user = 0;
    int i = 0;

    if (!si->initialized) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information STG - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init       : %4d\n"), si->initialized));
    LOG_CLI((BSL_META_U(unit,
                        "  STG range  : %4d-%4d\n"), si->stg_min, si->stg_max));
    LOG_CLI((BSL_META_U(unit,
                        "  Default STG: %4d\n"), *(si->def_stg)));
    LOG_CLI((BSL_META_U(unit,
                        "  VLAN Count : %4d\n"), si->vlan_count));
    LOG_CLI((BSL_META_U(unit,
                        "  VPN Count  : %4d\n"), si->vpn_count));

    /* Print STG list sorted */
    for (i = si->stg_min; i < si->stg_max; i++) {
        if (!SHR_BITGET(si->stg_bmp, i)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit, "  STG %4d:   "), i));
        count = 0;

        uidx = si->uidx_first[i];

        while (uidx != LTSW_STG_USER_INVALID_IDX) {
            if ((count > 0) && (!(count % 10))) {
                LOG_CLI((BSL_META_U(unit, "\n              ")));
            }

            user = stg_user_get(unit, uidx);

            LOG_CLI((BSL_META_U(unit, " %4d(%d)"),
                                user, stg_user_type_get(unit, uidx)));

            count++;

            uidx = si->uidx_next[uidx];
        }

        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    (void) mbcm_ltsw_stg_info_dump(unit);

    return;
}

/*!
 * \brief Check if STG exists.
 *
 * This function is used to check if STG exists.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  STG ID.
 *
 * \retval Zero                      STG doesn't exist.
 * \retval None-zero                 STG exists.
 */
int
bcmint_stg_exist_check(int unit, bcm_stg_t stg)
{
    return (SHR_BITGET(STG_INFO(unit)->stg_bmp, stg) ? 1 : 0);
}

/*!
 * \brief Get STG user index per STG user.
 *
 * \param [in] unit Unit Number.
 * \param [in] user STG user
 *
 * \retval Value STG user index.
 */
int
bcmint_stg_user_idx_get(int unit, int user)
{
    int utype = 0;

    utype = BCMI_LTSW_VIRTUAL_VPN_IS_SET(user) ?
            LTSW_STG_USER_VPN : LTSW_STG_USER_VLAN;

    return stg_user_idx_get(unit, user, utype);
}

/*!
 * \brief Get per npl LT info.
 *
 * This function is used to get STG lt info based on chip and NPL version.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   lt_id                STG LT ID.
 * \param [out]  lt_info              LT info.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmint_stg_lt_get(int unit,
                  bcmint_stg_lt_id_t lt_id,
                  const bcmint_stg_lt_t **lt_info)
{
    bcmint_stg_lt_db_t lt_db;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_lt_db_get(unit, &lt_db));

    if (!(lt_db.lt_bmp & (1 << lt_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *lt_info = &lt_db.lts[lt_id];

exit:
    SHR_FUNC_EXIT();
}
