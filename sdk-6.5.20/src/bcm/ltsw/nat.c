/*! \file nat.c
 *
 * NAT Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/nat.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/nat.h>

#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/nat.h>
#include <bcm_int/ltsw/mbcm/nat.h>

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>


/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* Nat information. */
typedef struct ltsw_nat_info_s {

    /* Nat module initialized. */
    int                       inited;

    /* Nat mutex lock. */
    sal_mutex_t               mutex;

    /* Minimum value for egress NAT ID. */
    int                       e_nat_id_min;

    /* Maximum value for egress NAT ID. */
    int                       e_nat_id_max;

} ltsw_nat_info_t;

/* Static global variable of NAT info. */
static ltsw_nat_info_t ltsw_nat_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* NAT information. */
#define NAT_INFO(u)        (&ltsw_nat_info[u])

/* Check if MULTICAST module is initialized. */
#define NAT_INIT_CHECK(u)                               \
    do {                                                \
        if (!ltsw_feature(unit, LTSW_FT_NAT)) {         \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);     \
        }                                               \
        if (NAT_INFO(u)->inited == false) {             \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);        \
        }                                               \
    } while(0)

/* NAT Lock. */
#define NAT_LOCK(u) \
        sal_mutex_take(NAT_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define NAT_UNLOCK(u) \
        sal_mutex_give(NAT_INFO(u)->mutex);

#define NAT_REALM_ID_MIN 1
#define NAT_REALM_ID_MAX 3

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Clean up NAT module software database.
 *
 * \param [in] unit Unit number.
 */
static void
nat_info_cleanup(int unit)
{
    ltsw_nat_info_t *nat_i = NAT_INFO(unit);

    if (nat_i->mutex) {
        sal_mutex_destroy(nat_i->mutex);
        nat_i->mutex = NULL;
    }

    return;
}

/*!
 * \brief Initialize NAT module software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
nat_info_init(int unit)
{
    ltsw_nat_info_t *nat_i = NAT_INFO(unit);
    bcmint_nat_pub_info_t nat_pub;

    SHR_FUNC_ENTER(unit);

    if (nat_i->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_nat_deinit(unit));
    }

    if (nat_i->mutex == NULL) {
        nat_i->mutex = sal_mutex_create("NatInfoMutex");
        SHR_NULL_CHECK(nat_i->mutex, SHR_E_MEMORY);
    }

    /* Initialize public table size. */
    sal_memset(&nat_pub, 0, sizeof(nat_pub));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_nat_pub_info_get(unit, &nat_pub));
    nat_i->e_nat_id_min = nat_pub.e_nat_id_min;
    nat_i->e_nat_id_max = nat_pub.e_nat_id_max;

exit:
    if (SHR_FUNC_ERR()) {
        nat_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the egress nat object.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Egress nat object pointer.
 * \param [in] check_with_id_flag Whether to check BCM_L3_NAT_EGRESS_WITH_ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nat_egress_key_validate(int unit, bcm_l3_nat_egress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    if (nat_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!(nat_info->flags &
          (BCM_L3_NAT_EGRESS_SNAT | BCM_L3_NAT_EGRESS_DNAT))) {
        /* Must specify at least one of SNAT and DNAT. */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (nat_info->flags & BCM_L3_NAT_EGRESS_SNAT) {
        if ((nat_info->snat_id < NAT_INFO(unit)->e_nat_id_min) ||
            (nat_info->snat_id > NAT_INFO(unit)->e_nat_id_max)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if ((nat_info->flags & BCM_L3_NAT_EGRESS_DNAT)) {
        if ((nat_info->dnat_id < NAT_INFO(unit)->e_nat_id_min) ||
            (nat_info->dnat_id > NAT_INFO(unit)->e_nat_id_max)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the key of ingress nat object.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Ingress nat object pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nat_ingress_key_validate(int unit, bcm_l3_nat_ingress_t *nat_info)
{
    uint32_t max_vrf = 0;

    SHR_FUNC_ENTER(unit);

    if (nat_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_vrf_max_get(unit, &max_vrf));

    if ((nat_info->vrf > max_vrf) ||
        (nat_info->vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the data of ingress nat object.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Ingress nat object pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nat_ingress_data_validate(int unit, bcm_l3_nat_ingress_t *nat_info)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    int nh_idx = 0;

    SHR_FUNC_ENTER(unit);

    if (nat_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((nat_info->realm_id < NAT_REALM_ID_MIN) ||
        (nat_info->realm_id > NAT_REALM_ID_MAX)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (nat_info->nat_id > NAT_INFO(unit)->e_nat_id_max) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (nat_info->flags & BCM_L3_NAT_INGRESS_MULTIPATH) {
        
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, nat_info->nexthop,
                                            &nh_idx, &type));
    if (type != BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Create a NAT translation entry.
 *
 * Notes: Snat_id or dnat_id must be specified since they are key for
 * hash translation table.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Egress nat object pointer.
 *                      Includes flags to specify SNAT/DNAT/NAPT,
 *                      key for translation table and translated IP/Ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_nat_egress_add(
    int unit,
    bcm_l3_nat_egress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    if (nat_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((nat_info->flags & BCM_L3_NAT_EGRESS_WITH_ID) !=
         BCM_L3_NAT_EGRESS_WITH_ID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (nat_egress_key_validate(unit, nat_info));

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_egress_add(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the egress NAT translation entry.
 *
 * Notes: Snat_id or dnat_id must be specified since they are key for
 * hash translation table.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Egress nat object pointer.
 *                      Includes flags to specify SNAT/DNAT/NAPT,
 *                      key for translation table and translated IP/Ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_nat_egress_destroy(
    int unit,
    bcm_l3_nat_egress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (nat_egress_key_validate(unit, nat_info));

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_egress_destroy(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the egress NAT translation entry based on snat_id/dnat_id.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Egress nat object pointer.
 *                      Includes flags to specify SNAT/DNAT/NAPT,
 *                      key snat_id/dnat_id for translation table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_l3_nat_egress_get(
    int unit,
    bcm_l3_nat_egress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (nat_egress_key_validate(unit, nat_info));

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_egress_get(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse through the egress NAT traslation entry specified by
 * flags and invoke a user provided callback for each valid entry.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags to match during traverse.
 * \param [in] start Not used any more.
 * \param [in] end Not used any more.
 * \param [in] cb Caller notification callback.
 * \param [in] user_data User cookie which should be returned in cb.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_l3_nat_egress_traverse(
    int unit,
    uint32 flags,
    uint32 start,
    uint32 end,
    bcm_l3_nat_egress_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(start);
    COMPILER_REFERENCE(end);

    NAT_INIT_CHECK(unit);

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_egress_traverse(unit, flags, cb, user_data));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an ingress NAT table entry.
 *
 * Notes: Snat_id or dnat_id must be specified since they are key for
 * hash translation table.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Ingress nat object pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_nat_ingress_add(int unit, bcm_l3_nat_ingress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (nat_ingress_key_validate(unit, nat_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (nat_ingress_data_validate(unit, nat_info));

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_ingress_add(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ingress NAT table entry.
 *
 * Notes: Snat_id or dnat_id must be specified since they are key for
 * hash translation table.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Ingress nat object pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_nat_ingress_delete(
    int unit,
    bcm_l3_nat_ingress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (nat_ingress_key_validate(unit, nat_info));

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_ingress_delete(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an ingress NAT table entry.
 *
 * Notes: Snat_id or dnat_id must be specified since they are key for
 * hash translation table.
 *
 * \param [in] unit Unit number.
 * \param [in/out] nat_info Ingress nat object pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_nat_ingress_find(
    int unit,
    bcm_l3_nat_ingress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (nat_ingress_key_validate(unit, nat_info));

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_ingress_find(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse through the ingress NAT table specified by flags
 * and invoke a user provided callback for each valid entry.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags to match during traverse.
 * \param [in] start Not used any more.
 * \param [in] end Not used any more.
 * \param [in] cb Caller notification callback.
 * \param [in] user_data User cookie which should be returned in cb.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_l3_nat_ingress_traverse(
    int unit,
    uint32 flags,
    uint32 start,
    uint32 end,
    bcm_l3_nat_ingress_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(start);
    COMPILER_REFERENCE(end);

    NAT_INIT_CHECK(unit);

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_ingress_traverse(unit, flags, cb, user_data));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all the ingress NAT table entries specified by flags.
 *
 * Notes: Snat_id or dnat_id must be specified since they are key for
 * hash translation table.
 *
 * \param [in] unit Unit number.
 * \param [in] nat_info Only flags field is valid. These are used to figure
 *                      out which table to clear.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_l3_nat_ingress_delete_all(
    int unit,
    bcm_l3_nat_ingress_t *nat_info)
{
    SHR_FUNC_ENTER(unit);

    NAT_INIT_CHECK(unit);

    if (nat_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    NAT_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_nat_ingress_delete_all(unit, nat_info));

    NAT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_nat_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (nat_info_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_nat_init(unit));

    NAT_INFO(unit)->inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_nat_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (NAT_INFO(unit)->inited == FALSE) {
        SHR_EXIT();
    }

    NAT_LOCK(unit);
    NAT_INFO(unit)->inited = FALSE;
    NAT_UNLOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_nat_detach(unit));

    nat_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

