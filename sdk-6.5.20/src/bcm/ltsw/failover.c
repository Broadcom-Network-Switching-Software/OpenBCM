/*! \file failover.c
 *
 * BCM level APIs for failover.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* SDK6 API header file */
#include <bcm/error.h>
#include <bcm/failover.h>
#include <bcm/l3.h>

/* Mixed header file  */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/failover.h>
#include <bcm_int/ltsw/failover_int.h>
#include <bcm_int/ltsw/mbcm/failover.h>
#include <bcm_int/ltsw/property.h>

/* SDKLT header file */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FAILOVER

/*!
 * \brief Software book keeping for failover related information
 */
typedef struct ltsw_failover_bookkeeping_s {
    /*! Data structure initialized or not. */
    int             initialized;

    /*! Protection mutex. */
    sal_mutex_t     failover_mutex;
} ltsw_failover_bookkeeping_t;

static ltsw_failover_bookkeeping_t ltsw_failover_bk_info[BCM_MAX_NUM_UNITS];

#define LTSW_FAILOVER_LOCK(unit) \
    do { \
        if (LTSW_FAILOVER_INFO(unit)->failover_mutex) { \
            sal_mutex_take(LTSW_FAILOVER_INFO(unit)->failover_mutex, SAL_MUTEX_FOREVER); \
        } \
    } while (0)

#define LTSW_FAILOVER_UNLOCK(unit) \
    do { \
        if (LTSW_FAILOVER_INFO(unit)->failover_mutex) { \
            sal_mutex_give(LTSW_FAILOVER_INFO(unit)->failover_mutex); \
        } \
    } while (0)

#define LTSW_FAILOVER_INFO(_unit_)   (&ltsw_failover_bk_info[_unit_])

#define LTSW_FAILOVER_INITIALIZED(_unit_) LTSW_FAILOVER_INFO(_unit_)->initialized

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Create protection mutex for failover module.
 *
 * \param [in] unit         Unit Number.
 *
 * \retval SHR_E_NONE       Success.
 * \retval !SHR_E_NONE      Failure.
 */
static int
ltsw_failover_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (LTSW_FAILOVER_INFO(unit)->failover_mutex == NULL) {
        LTSW_FAILOVER_INFO(unit)->failover_mutex = sal_mutex_create("bcmFailoverMutex");
        SHR_NULL_CHECK(LTSW_FAILOVER_INFO(unit)->failover_mutex, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for failover module.
 *
 * \param [in] unit     Unit Number.
 *
 * \retval None.
 */
static void
ltsw_failover_lock_destroy(int unit)
{
    if (LTSW_FAILOVER_INFO(unit)->failover_mutex != NULL) {
        sal_mutex_destroy(LTSW_FAILOVER_INFO(unit)->failover_mutex);
        LTSW_FAILOVER_INFO(unit)->failover_mutex = NULL;
    }
}

/*!
 * \brief Check if L3 Egress module is available.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_UNAVAIL L3 feature is not supported.
 * \retval SHR_E_INIT L3 egress module is not initialized.
 */
static int
l3_egr_is_available(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_L3)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_DISABLED);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public cross-module functions
 */
/*!
 * \brief Get PROT_NHOP_OFFSET field of LT TNL_CONTROL.
 *
 * \param [in]  unit            Unit number.
 * \param [out] prot_offset     Protection NH offset.
 *
 * \retval      SHR_E_NONE      No errors.
 * \retval      !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_failover_prot_offset_get(int unit, uint16_t *prot_offset)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        {PROT_NHOP_OFFSETs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    /* Get LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TNL_CONTROLs, &lt_entry, NULL, NULL));

    *prot_offset = (uint16_t)fields[0].u.val;

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Validate whether the protected NH intf can be the failover of specified primary NH intf.
 *
 * \param [in] unit          Unit Number.
 * \param [in] intf          Primary NH interface.
 * \param [in] failover_intf Protected NH interface.
 *
 * \retval     SHR_E_NONE No errors.
 * \retval     !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_failover_validate(int unit, bcm_if_t intf, bcm_if_t failover_intf)
{
    uint16_t prot_offset;
    bcm_if_t prot_intf;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    if (prot_offset == 0) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Get prot_intf from primary intf with fixed offset. */
    prot_intf = intf + prot_offset;

    /*
     * Check if the given protected NH intf (failover_intf) equal to the
     * prot_intf (primary intf + prot_offset).
     */
    if (failover_intf != prot_intf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public module internal functions
 */
/*!
 * \brief Set PROT_NHOP_OFFSET field of LT TNL_CONTROL.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  prot_offset     Protection NH offset.
 *
 * \retval      SHR_E_NONE      No errors.
 * \retval      !SHR_E_NONE     Failure.
 */
int
bcmint_failover_prot_offset_set(int unit, uint16_t prot_offset)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        {PROT_NHOP_OFFSETs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    /* Set protection offset. */
    fields[0].u.val = (uint64_t)prot_offset;

    /* Update LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TNL_CONTROLs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
/*!
 * \brief Detach the failover software module.
 *
 * \param [in]  unit      Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_failover_cleanup(int unit)
{
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_EXIT();
    }

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    /* Clear failover related LT tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_failover_clear(unit));

    /* Mark the state as uninitialized. */
    LTSW_FAILOVER_INITIALIZED(unit) = FALSE;

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

    /* Destroy protection mutex. */
    ltsw_failover_lock_destroy(unit);

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the failover software module
 * \param [in]  unit      Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_failover_init(int unit)
{
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (l3_egr_is_available(unit));

    if (LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_failover_cleanup(unit));
    }

    /* Create failover protection mutex. */
    SHR_IF_ERR_EXIT
        (ltsw_failover_lock_create(unit));

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    /* Chip specific failover initializztion. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_failover_init(unit));

    LTSW_FAILOVER_INITIALIZED(unit) = TRUE;

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

exit:
    if (SHR_FUNC_ERR()) {
        if (locked) {
            LTSW_FAILOVER_UNLOCK(unit);
        }
        ltsw_failover_lock_destroy(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the protected NH for the specified primary nexthop (intf).
 * \param [in]  unit    Unit number.
 * \param [in]  intf    Primary NH interface.
 * \param [in]  egr     Egress object to be used as the
 *                      protection nexthop for primary NH.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcm_ltsw_failover_egress_set(int unit, bcm_if_t intf, bcm_l3_egress_t *egr)
{
    uint32_t flags = 0;
    uint16_t prot_offset;
    bcm_if_t prot_intf;
    bcmi_ltsw_l3_egr_obj_type_t type;
    int nh_idx;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

    if (prot_offset == 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Validate intf and get the type of NH. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf, &nh_idx, &type));

    /* Get prot_intf with fixed offset from primary intf. */
    prot_intf = intf + prot_offset;

    /* Create protected egress nh BCM_L3_WITH_ID flags. */
    flags |= (BCM_L3_WITH_ID | BCM_L3_REPLACE);

    /* Here failover_id is used internally only to distinguish
     * between primary_intf and prot_intf egress creation.
     * This flag bit will be clear in bcm_ltsw_l3_egress_create.
     */
    egr->failover_id |= BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON;

    /* Set flags2 for underlay type. */
    if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        egr->flags2 = BCM_L3_FLAGS2_UNDERLAY;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l3_egress_create(unit, flags, egr, &prot_intf));

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the protected NH for the specified primary nexthop (intf).
 * \param [in]  unit    Unit number.
 * \param [in]  intf    Primary NH interface.
 * \param [out] egr     Egress object to be used as the
 *                      protection nexthop for primary NH.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcm_ltsw_failover_egress_get(int unit, bcm_if_t intf, bcm_l3_egress_t *egr)
{
    uint16_t prot_offset;
    bcm_if_t prot_intf;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

    if (prot_offset == 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Get prot_intf with fixed offset from primary intf. */
    prot_intf = intf + prot_offset;

    /* Validate prot_intf and get the corresponding L3 egress obj. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l3_egress_get(unit, prot_intf, egr));

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the protected NH set earlier for the specified
 *        primary nexthop (intf).
 * \param [in]  unit    Unit number.
 * \param [in]  intf    Primary NH interface.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcm_ltsw_failover_egress_clear(int unit, bcm_if_t intf)
{
    uint16_t prot_offset;
    bcm_if_t prot_intf;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

    if (prot_offset == 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Get prot_intf with fixed offset from primary intf. */
    prot_intf = intf + prot_offset;

    /* Disable egress status for prot_intf (i.e., use primary intf). */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_failover_egress_status_set(unit, intf, FALSE));

    /* Destroy prot L3 egress object. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l3_egress_destroy(unit, prot_intf));

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set/Clear the active state of usage of primary/protected NH.
 * \param [in]  unit    Unit number.
 * \param [in]  intf    Primary NH interface.
 * \param [in]  enable  True for use of protected NH,
 *                      False for use of Primary NH
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
*/
int
bcm_ltsw_failover_egress_status_set(int unit, bcm_if_t intf, int enable)
{
    uint16_t prot_offset;
    int nh_idx, base;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Calculate next hop index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf, &nh_idx, &type));

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    /* Validate nh_idx. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    base = (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ? (prot_offset * 2) : 0;

    if (nh_idx >= (prot_offset + base)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Set the enable state of NH. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_failover_egress_status_set(unit, nh_idx, enable));

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the active state of usage of primary/protected NH.
 * \param [in]  unit    Unit number.
 * \param [in]  intf    Primary NH interface.
 * \param [out] enable  True for use of protected NH,
 *                      False for use of Primary NH
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
*/
int
bcm_ltsw_failover_egress_status_get(int unit, bcm_if_t intf, int *enable)
{
    uint16_t prot_offset;
    int nh_idx, base;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check input parameter. */
    if (enable == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Calculate next hop index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf, &nh_idx, &type));

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    /* Validate nh_idx. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    base = (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ? (prot_offset * 2) : 0;

    if (nh_idx >= (prot_offset + base)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the enable state of NH. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_failover_egress_status_get(unit, nh_idx, enable));

    LTSW_FAILOVER_UNLOCK(unit);
    locked = FALSE;

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the protection NH interface for the primary NH interface.
 * \param [in]  unit       Unit number.
 * \param [in]  intf       Primary NH interface.
 * \param [out] prot_intf  Protection NH interface for
 *                         the primary NH interface.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcm_ltsw_failover_egress_protection_get(
    int unit,
    bcm_if_t intf,
    bcm_if_t *prot_intf)
{
    uint16_t prot_offset;
    bcm_l3_egress_t egr;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egr_is_available(unit));

    /*  Make sure module was initialized. */
    if (!LTSW_FAILOVER_INITIALIZED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check input parameter. */
    if (prot_intf == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    bcm_l3_egress_t_init(&egr);

    /* Validate intf and make sure the L3 egress obj exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l3_egress_get(unit, intf, &egr));

    LTSW_FAILOVER_LOCK(unit);
    locked = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    if (prot_offset == 0) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Get prot_intf with fixed offset from primary intf. */
    *prot_intf = intf + prot_offset;

    /* Validate prot_intf and make sure the failover obj exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l3_egress_get(unit, *prot_intf, &egr));

exit:
    if (locked) {
        LTSW_FAILOVER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

