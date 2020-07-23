/*! \file mpls.c
 *
 * BCM level APIs for MPLS (Multiple Protocol Label Switching).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/init.h>
#include <bcm_int/ltsw/mbcm/mpls.h>
#include <bcm_int/ltsw/mpls_int.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/failover.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/generated/mpls_ha.h>
#include <bcm_int/ltsw/issu.h>

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MPLS

/* VPWS vpn member action. */
typedef enum vpws_vpn_member_act_e {
    ACT_INVALID        = 0,   /* Invalid action. */
    ACT_ADD_ACC        = 1,   /* Add the access port. */
    ACT_ADD_PRIMARY    = 2,   /* Add the primary port. */
    ACT_ADD_BACKUP     = 3,   /* Add the backup port. */
    ACT_UPD_ACC        = 4,   /* Update the access port. */
    ACT_UPD_PRIMARY    = 5,   /* Update the primary port. */
    ACT_UPD_BACKUP     = 6,   /* Update the backup port. */
    ACT_DEL_ACC        = 7,   /* Delete the access port. */
    ACT_DEL_PRIMARY    = 8,   /* Delete the primary port. */
    ACT_DEL_BACKUP     = 9    /* Delete the backup port. */
} vpws_vpn_member_act_t;

/* MPLS information. */
typedef struct ltsw_mpls_info_s {

    /* Initialized flag. */
    int                    initialized;

    /* Protection mutex. */
    sal_mutex_t            mutex;

    /* VPWS VPN min.  */
    int                    vpws_vpn_min;

    /* VPWS VPN max.  */
    int                    vpws_vpn_max;

    /* VPWS vpn usage bitmap.  */
    SHR_BITDCL             *vpws_vpn_bmp;

    /* VPWS vpn info.  */
    vpws_vpn_member_info_t *vpws_vpn_member_info;

} ltsw_mpls_info_t;

/* Software bookkeeping for MPLS information. */
static ltsw_mpls_info_t ltsw_mpls_info[BCM_MAX_NUM_UNITS];

/* MPLS info. */
#define MPLS_INFO(_u_) (&ltsw_mpls_info[_u_])

/* VPWS VPN index min.  */
#define MPLS_VPWS_VPN_MIN(_u_)     (MPLS_INFO((_u_))->vpws_vpn_min)

/* VPWS VPN index max.  */
#define MPLS_VPWS_VPN_MAX(_u_)     (MPLS_INFO((_u_))->vpws_vpn_max)

/* VPWS VPN usage bitmap operations. */
#define MPLS_VPWS_VPN_USED_GET(_u_, _idx_) \
    SHR_BITGET(MPLS_INFO(_u_)->vpws_vpn_bmp, (_idx_))

#define MPLS_VPWS_VPN_USED_SET(_u_, _idx_) \
    SHR_BITSET(MPLS_INFO((_u_))->vpws_vpn_bmp, (_idx_))

#define MPLS_VPWS_VPN_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(MPLS_INFO((_u_))->vpws_vpn_bmp, (_idx_))

/* MPLS mutex lock. */
#define MPLS_LOCK(_u_) \
    sal_mutex_take(MPLS_INFO(_u_)->mutex, SAL_MUTEX_FOREVER);

/* MPLS mutex unlock. */
#define MPLS_UNLOCK(_u_) \
    sal_mutex_give(MPLS_INFO(_u_)->mutex);

/* VPWS VPN member info.  */
#define VPWS_VPN_MEM_INFO(_u_, _idx_) \
    (&(MPLS_INFO((_u_))->vpws_vpn_member_info[_idx_]))

/* Check if MPLS module is initialized. */
#define MPLS_INIT_CHECK(_u_)                     \
    do {                                         \
        ltsw_mpls_info_t *mi = MPLS_INFO(_u_); \
        if (!mi->initialized) {                \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        }                                        \
    } while(0)

#define VALID_VLAN_ID(_id_)    ((_id_ > BCM_VLAN_MIN) && (_id_ <= BCM_VLAN_MAX))


/* MPLS feature check. */
#define MPLS_FEATURE_CHECK(_u_)                     \
    do {                                            \
        if (!ltsw_feature(_u_, LTSW_FT_MPLS)) {     \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL); \
        }                                           \
    } while(0)

/* MPLS special label feature check. */
#define MPLS_SPECIAL_LABEL_FEATURE_CHECK(_u_)                 \
    do {                                                      \
        if (!ltsw_feature(_u_, LTSW_FT_MPLS_SPECIAL_LABEL)) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);           \
        }                                                     \
    } while(0)

/* MPLS VPN ID check. */
#define MPLS_VPN_ID_CHECK(_u_,_vpn_)                           \
    do {                                                       \
        if ((!BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(_vpn_)) &&         \
            (!BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(_vpn_))) {         \
            if (ltsw_feature(_u_, LTSW_FT_MPLS_VLAN_DOMAIN)) { \
                if (!VALID_VLAN_ID(_vpn_)) {                   \
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);      \
                }                                              \
            } else {                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);          \
            }                                                  \
        }                                                      \
    } while(0)


/* Invalid mpls label. */
#define LTSW_MPLS_LBL_MAX               (0xfffff)
#define LTSW_MPLS_LABEL_ID_VALID(_label_)             \
             ((_label_) <= LTSW_MPLS_LBL_MAX)
#define LTSW_INVALID_MPLS_LABEL         (LTSW_MPLS_LBL_MAX + 1)
#define LTSW_MPLS_LBL_VALID(_label_)    (_label_ <= LTSW_MPLS_LBL_MAX)

#define LTSW_MPLS_LABEL_MASK_MAX        (0xfffff)
#define LTSW_MPLS_LABEL_MASK_VALID(_mask_)            \
             (((_mask_) >= 0) && \
             ((_mask_) <= LTSW_MPLS_LABEL_MASK_MAX))

#define LTSW_MPLS_EXP_MAX          (0x7)
#define LTSW_MPLS_EXP_VALID(exp)                      \
             ((exp) <= LTSW_MPLS_EXP_MAX)

#define LTSW_MPLS_EXP_MASK_MAX        (0x7)
#define LTSW_MPLS_EXP_MASK_VALID(_mask_)              \
             (((_mask_) >= 0) && \
             ((_mask_) <= BCMI_MPLS_EXP_MASK_MAX))

#define LTSW_MPLS_TTL_MAX          (0xFF)
#define LTSW_MPLS_TTL_VALID(ttl)                      \
             ((ttl) <= LTSW_MPLS_TTL_MAX)

#define LTSW_MPLS_TTL_MASK_MAX        (0xff)
#define LTSW_MPLS_TTL_MASK_VALID(_mask_) \
             (((_mask_) >= 0) &&         \
             ((_mask_) <= BCMI_MPLS_TTL_MASK_MAX))

#define MPLS_SPECIAL_LABEL_INFO_CHECK(_info_)                \
    do {                                                     \
        if (!LTSW_MPLS_LABEL_ID_VALID(_info_.label_value)) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);            \
        }                                                    \
        if (!LTSW_MPLS_EXP_VALID(_info_.exp)) {              \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);            \
        }                                                    \
        if (!LTSW_MPLS_TTL_VALID(_info_.ttl)) {              \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);            \
        }                                                    \
        if (_info_.label_mask > LTSW_MPLS_LABEL_MASK_MAX) {  \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);            \
        }                                                    \
        if (_info_.exp_mask > LTSW_MPLS_EXP_MASK_MAX) {      \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);            \
        }                                                    \
        if (_info_.ttl_mask > LTSW_MPLS_TTL_MASK_MAX) {      \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);            \
        }                                                    \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Cleanup the MPLS information.
 *
 * This function is used to cleanup the MPLS information.
 *
 * \param [in]  unit    Unit number.
 *
 * \retval None.
 */
static void
mpls_info_cleanup(int unit)
{
    ltsw_mpls_info_t *mi = MPLS_INFO(unit);

    /* Destory mutex. */
    if (mi->mutex != NULL) {
        sal_mutex_destroy(mi->mutex);
        mi->mutex = NULL;
    }

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        return;
    }

    sal_memset(mi, 0, sizeof(ltsw_mpls_info_t));

    return;
}

/*!
 * \brief Initialize the MPLS information.
 *
 * This function is used to initialize the MPLS information.
 *
 * \param [in]  unit      Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_info_init(int unit)
{
    ltsw_mpls_info_t *mi = MPLS_INFO(unit);
    int vpws_vpn_cnt;
    int vpn_idx = 0;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    int warm = bcmi_warmboot_get(unit);
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Create mutex. */
    if (mi->mutex == NULL) {
        mi->mutex = sal_mutex_create("MPLS mutex");
        SHR_NULL_CHECK(mi->mutex, SHR_E_MEMORY);
    }

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_vpws_info_get(unit, &(mi->vpws_vpn_min),
                                      &(mi->vpws_vpn_max)));
    vpws_vpn_cnt     = mi->vpws_vpn_max + 1 - mi->vpws_vpn_min;
    ha_req_size      = SHR_BITALLOCSIZE(vpws_vpn_cnt);
    ha_alloc_size    = ha_req_size;
    mi->vpws_vpn_bmp = bcmi_ltsw_ha_mem_alloc(unit,
                                              BCMI_HA_COMP_ID_MPLS,
                                              bcmLtswMplsHaVpwsVpnBmp,
                                              "bcmLtswMplsHaVpwsVpnBmp",
                                              &ha_alloc_size);
    SHR_NULL_CHECK(mi->vpws_vpn_bmp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(mi->vpws_vpn_bmp, 0, ha_alloc_size);
    }

    ha_req_size      = sizeof(vpws_vpn_member_info_t) * vpws_vpn_cnt;
    ha_alloc_size    = ha_req_size;
    mi->vpws_vpn_member_info = bcmi_ltsw_ha_mem_alloc(unit,
                                              BCMI_HA_COMP_ID_MPLS,
                                              bcmLtswMplsHaVpwsVpnMemInfo,
                                              "bcmLtswMplsHaVpwsVpnMemInfo",
                                              &ha_alloc_size);
    SHR_NULL_CHECK(mi->vpws_vpn_member_info, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(mi->vpws_vpn_member_info, 0, ha_alloc_size);
        for (vpn_idx = 0; vpn_idx < vpws_vpn_cnt; vpn_idx++) {
            mi->vpws_vpn_member_info[vpn_idx].state      = STATE_INIT;
            mi->vpws_vpn_member_info[vpn_idx].acc_id     = BCM_GPORT_INVALID;
            mi->vpws_vpn_member_info[vpn_idx].acc_id2    = BCM_GPORT_INVALID;
            mi->vpws_vpn_member_info[vpn_idx].primary_id = BCM_GPORT_INVALID;
            mi->vpws_vpn_member_info[vpn_idx].backup_id  = BCM_GPORT_INVALID;
        }
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                           BCMI_HA_COMP_ID_MPLS,
                                           bcmLtswMplsHaVpwsVpnMemInfo,
                                           0, sizeof(vpws_vpn_member_info_t),
                                           vpws_vpn_cnt,
                                           VPWS_VPN_MEMBER_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm) {
            if (mi->vpws_vpn_bmp) {
                (void)bcmi_ltsw_ha_mem_free(unit, mi->vpws_vpn_bmp);
            }

            if (mi->vpws_vpn_member_info) {
                (void)bcmi_ltsw_ha_mem_free(unit, mi->vpws_vpn_member_info);
            }
        }

        mpls_info_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a VPWS VPN index.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn_idx    VPN index.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_vpws_vpn_alloc(int unit, int  *vpn_idx)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = MPLS_VPWS_VPN_MIN(unit); i <= MPLS_VPWS_VPN_MAX(unit); i++) {
        if (!MPLS_VPWS_VPN_USED_GET(unit, i)) {
            break;
        }
    }
    if (i > MPLS_VPWS_VPN_MAX(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    *vpn_idx = i;
    MPLS_VPWS_VPN_USED_SET(unit, *vpn_idx);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an MPLS tunnel terminator.
 *
 * \param [in] unit       Unit number.
 * \param [in] info       MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_switch_add(unit, info));

exit:

    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an MPLS tunnel terminator.
 *
 * \param [in] unit       Unit number.
 * \param [in] info       MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_switch_delete(unit, info));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS tunnel terminator.
 *
 * \param [in] unit       Unit number.
 * \param [in] info       MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_switch_get(unit, info));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all MPLS tunnel terminators.
 *
 * \param [in] unit       Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_tunnel_switch_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_switch_delete_all(unit));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all MPLS tunnel terminators.
 *
 * \param [in] unit         Unit number.
 * \param [in] cb           User callback function, called once per terminator.
 * \param [in] user_data    Cookie.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
mpls_tunnel_switch_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_switch_traverse(unit, cb, user_data));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an MPLS tunnel initiator.
 *
 * \param [in] unit            Unit number.
 * \param [in] tunnel_encap    MPLS tunnel encap info.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mpls_tunnel_encap_create(
    int unit,
    uint32_t options,
    bcm_mpls_tunnel_encap_t *tunnel_encap)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_encap_create(unit, options, tunnel_encap));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS tunnel initiator.
 *
 * \param [in] unit                Unit number.
 * \param [in/out] tunnel_encap    MPLS tunnel initiator info.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
mpls_tunnel_encap_get(
    int unit,
    bcm_mpls_tunnel_encap_t *tunnel_encap)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_encap_get(unit, tunnel_encap));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an MPLS tunnel initiator.
 *
 * \param [in] unit            Unit number.
 * \param [in] tunnel_encap    MPLS tunnel encap ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mpls_tunnel_encap_destroy(
    int unit,
    bcm_gport_t tunnel_id)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_encap_destroy(unit, tunnel_id));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all MPLS tunnel initiators.
 *
 * \param [in] unit       Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_tunnel_encap_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_encap_destroy_all(unit));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an MPLS tunnel initiator.
 *
 * \param [in] unit            Unit number.
 * \param [in] tunnel_encap    MPLS tunnel encap info.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mpls_tunnel_initiator_clear(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_initiator_clear(unit, intf));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS tunnel initiator.
 *
 * \param [in] unit                Unit number.
 * \param [in/out] tunnel_encap    MPLS tunnel initiator info.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
mpls_tunnel_initiator_clear_all(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_initiator_clear_all(unit));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an MPLS tunnel initiator.
 *
 * \param [in] unit            Unit number.
 * \param [in] tunnel_encap    MPLS tunnel encap ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_mpls_egress_label_t *label_array,
    int *label_count)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_initiator_get(unit, intf, label_max,
                                          label_array, label_count));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all MPLS tunnel initiators.
 *
 * \param [in] unit       Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t *label_array)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_initiator_set(unit, intf, num_labels,
                                          label_array));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate the VPN ID.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn        VPN  ID.
 *
 * \retval SHR_E_NONE     VPN ID is valid.
 * \retval !SHR_E_NONE    VPN ID is not valid.
 */
static int
mpls_vpn_id_validate(int unit, bcm_vpn_t vpn)
{
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx, BCM_VPN_TYPE_MPLS_VPWS, vpn);
        if ((vpn_idx < MPLS_VPWS_VPN_MIN(unit)) ||
            (vpn_idx > MPLS_VPWS_VPN_MAX(unit))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
        if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, vpn)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx, BCM_VPN_TYPE_MPLS_VPLS, vpn);
        if (!bcmi_ltsw_virtual_vfi_used_get(unit, vpn_idx,
                                           BCMI_LTSW_VFI_TYPE_MPLS)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the VPWS VPN member information.
 *
 * \param [in] unit         Unit number.
 * \param [in] vpn          VPN ID.
 * \param [out] mem_info    VPN member info.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
vpws_vpn_member_info_get(
    int unit, bcm_vpn_t vpn, vpws_vpn_member_info_t *mem_info)
{
    int vpn_idx;

    SHR_FUNC_ENTER(unit);
    BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                              BCM_VPN_TYPE_MPLS_VPWS, vpn);

    mem_info->acc_id      = VPWS_VPN_MEM_INFO(unit, vpn_idx)->acc_id;
    mem_info->acc_id2     = VPWS_VPN_MEM_INFO(unit, vpn_idx)->acc_id2;
    mem_info->primary_id  = VPWS_VPN_MEM_INFO(unit, vpn_idx)->primary_id;
    mem_info->backup_id   = VPWS_VPN_MEM_INFO(unit, vpn_idx)->backup_id;
    mem_info->state       = VPWS_VPN_MEM_INFO(unit, vpn_idx)->state;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the VPWS VPN member information.
 *
 * \param [in] unit         Unit number.
 * \param [in] vpn          VPN ID.
 * \param [out] mem_info    VPN member info.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static void
vpws_vpn_member_info_set(
    int unit, bcm_vpn_t vpn, vpws_vpn_member_info_t mem_info)
{
    int vpn_idx;

    BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                              BCM_VPN_TYPE_MPLS_VPWS, vpn);

    VPWS_VPN_MEM_INFO(unit, vpn_idx)->acc_id      = mem_info.acc_id;
    VPWS_VPN_MEM_INFO(unit, vpn_idx)->acc_id2     = mem_info.acc_id2;
    VPWS_VPN_MEM_INFO(unit, vpn_idx)->primary_id  = mem_info.primary_id;
    VPWS_VPN_MEM_INFO(unit, vpn_idx)->backup_id   = mem_info.backup_id;
    VPWS_VPN_MEM_INFO(unit, vpn_idx)->state       = mem_info.state;
    return;
}

/*!
 * \brief VPWS port operation when there is no port exsisting in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_init_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_ADD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        mem_info.acc_id = mpls_port->mpls_port_id;
        mem_info.state  = STATE_ACC;
        break;
    case ACT_ADD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        mem_info.primary_id = mpls_port->mpls_port_id;
        mem_info.state      = STATE_PRIMARY;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VPWS port operation when the access port exsists in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_acc_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_ADD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mem_info.acc_id,
                                        mpls_port->mpls_port_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mpls_port->mpls_port_id,
                                        mem_info.acc_id));
        mem_info.primary_id = mpls_port->mpls_port_id;
        mem_info.state      = STATE_ACC_PRIMARY;
        break;
    case ACT_UPD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_DEL_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.acc_id = BCM_GPORT_INVALID;
        mem_info.state  = STATE_INIT;
        break;
    case ACT_ADD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mem_info.acc_id,
                                        mpls_port->mpls_port_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mpls_port->mpls_port_id,
                                        mem_info.acc_id));
        mem_info.acc_id2 = mpls_port->mpls_port_id;
        mem_info.state      = STATE_ACC_ACC;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VPWS port operation when two access port exsists in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_acc_acc_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_UPD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_DEL_ACC:
        if (mpls_port_id == mem_info.acc_id) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_mpls_port_detach(unit, mem_info.acc_id2));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_mpls_port_detach(unit, mem_info.acc_id));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));

        if (mpls_port_id == mem_info.acc_id) {
            mem_info.acc_id  = mem_info.acc_id2;
        }
        mem_info.acc_id2 = BCM_GPORT_INVALID;
        mem_info.state  = STATE_ACC;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VPWS port operation when the primary port exsists in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_primary_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_ADD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mpls_port->mpls_port_id,
                                        mem_info.primary_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mem_info.primary_id,
                                        mpls_port->mpls_port_id));
        mem_info.acc_id = mpls_port->mpls_port_id;
        mem_info.state  = STATE_ACC_PRIMARY;
        break;
    case ACT_ADD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        mem_info.backup_id  = mem_info.primary_id;
        mem_info.primary_id = mpls_port->mpls_port_id;
        mem_info.state      = STATE_PRIMARY_BACKUP;
        break;
    case ACT_ADD_BACKUP:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        mem_info.backup_id = mpls_port->mpls_port_id;
        mem_info.state     = STATE_PRIMARY_BACKUP;
        break;
    case ACT_UPD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_DEL_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.primary_id = BCM_GPORT_INVALID;
        mem_info.state      = STATE_INIT;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VPWS port operation when the access and primary port exsist in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_acc_primary_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_ADD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mem_info.acc_id,
                                        mpls_port->mpls_port_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mpls_port->mpls_port_id,
                                        mem_info.acc_id));

        mem_info.backup_id  = mem_info.primary_id;
        mem_info.primary_id = mpls_port->mpls_port_id;
        mem_info.state      = STATE_ALL;
        break;
    case ACT_ADD_BACKUP:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mpls_port->mpls_port_id,
                                        mem_info.acc_id));

        mem_info.backup_id = mpls_port->mpls_port_id;
        mem_info.state     = STATE_ALL;
        break;
    case ACT_UPD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_UPD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_DEL_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_detach(unit, mem_info.primary_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.acc_id = BCM_GPORT_INVALID;
        mem_info.state  = STATE_PRIMARY;
        break;
    case ACT_DEL_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_detach(unit, mem_info.acc_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.primary_id = BCM_GPORT_INVALID;
        mem_info.state      = STATE_ACC;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VPWS port operation when the primary and backup port exsist in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_primary_backup_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_ADD_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mpls_port->mpls_port_id,
                                        mem_info.primary_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_attach(unit, mem_info.primary_id,
                                        mpls_port->mpls_port_id));

        mem_info.acc_id = mpls_port->mpls_port_id;
        mem_info.state  = STATE_ALL;
        break;
    case ACT_UPD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_UPD_BACKUP:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        break;
    case ACT_DEL_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));

        mem_info.primary_id = mem_info.backup_id;
        mem_info.backup_id  = BCM_GPORT_INVALID;
        mem_info.state      = STATE_PRIMARY;
        break;
    case ACT_DEL_BACKUP:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.backup_id = BCM_GPORT_INVALID;
        mem_info.state     = STATE_PRIMARY;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VPWS port operation when all the three ports exsist in the VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] action           VPWS port member actions.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port object.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_state_all_op(
    int unit, vpws_vpn_member_act_t action,
    bcm_vpn_t vpn, bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (action) {
    case ACT_UPD_ACC:
    case ACT_UPD_PRIMARY:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));
        break;
    case ACT_UPD_BACKUP:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

        break;
    case ACT_DEL_ACC:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_detach(unit, mem_info.primary_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_detach(unit, mem_info.backup_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.acc_id = BCM_GPORT_INVALID;
        mem_info.state  = STATE_PRIMARY_BACKUP;
        break;
    case ACT_DEL_PRIMARY:
        vp = BCM_GPORT_MPLS_PORT_ID_GET(mem_info.backup_id);
        if (BCMINT_MPLS_GPORT_FAILOVER_VPLESS_GET(vp)) {
            /* For vpless case, the failover port should be deleted first. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_mpls_port_delete(unit, vpn, mem_info.backup_id));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_mpls_port_attach(unit, mem_info.acc_id,
                                            mem_info.backup_id));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        if (BCMINT_MPLS_GPORT_FAILOVER_VPLESS_GET(vp)) {
            mem_info.primary_id = BCM_GPORT_INVALID;
            mem_info.state      = STATE_ACC;
        } else {
            mem_info.primary_id = mem_info.backup_id;
            mem_info.state      = STATE_ACC_PRIMARY;
        }
        mem_info.backup_id  = BCM_GPORT_INVALID;
        break;
    case ACT_DEL_BACKUP:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));
        mem_info.backup_id  = BCM_GPORT_INVALID;
        mem_info.state      = STATE_ACC_PRIMARY;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    vpws_vpn_member_info_set(unit, vpn, mem_info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a VPWS port in a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_get(
    int unit, bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    bcm_mpls_port_t  fo_mpls_port;
    vpws_vpn_member_info_t mem_info;
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx, BCM_VPN_TYPE_MPLS_VPWS, vpn);
    if ((vpn_idx < MPLS_VPWS_VPN_MIN(unit)) ||
        (vpn_idx > MPLS_VPWS_VPN_MAX(unit))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!MPLS_VPWS_VPN_USED_GET(unit, vpn_idx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_port_get(unit, vpn, mpls_port_id, mpls_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));
    if ((mem_info.primary_id == mpls_port_id) &&
        (mem_info.backup_id  != BCM_GPORT_INVALID)){
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_port_get(unit, vpn, mem_info.backup_id,
                                     &fo_mpls_port));
        if (!(fo_mpls_port.flags & BCM_MPLS_PORT_FAILOVER)) {
            mpls_port->failover_port_id = mem_info.backup_id;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate if failover_egr_if can be
 * the failover egress object of egr_if.
 *
 * \param [in] unit               Unit number.
 * \param [in] egr_if             Egress object ID.
 * \param [in] failover_egr_if    Failover egress object ID.
 *
 * \retval SHR_E_NONE             No errors.
 * \retval !SHR_E_NONE            Failure.
 */
static int
mpls_port_failover_validate(
    int unit, bcm_if_t egr_if, bcm_if_t failover_egr_if)
{
    return bcmi_ltsw_failover_validate(unit, egr_if, failover_egr_if);
}

/*!
 * \brief Add a VPWS port to a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpws_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    vpws_vpn_member_info_t mem_info;
    vpws_vpn_member_act_t action;
    bcm_mpls_port_t p_port;
    bcm_mpls_port_t  fo_port;

    SHR_FUNC_ENTER(unit);

    bcm_mpls_port_t_init(&fo_port);

    /* Validate the encap ID for failover case. */
    if ((mpls_port->flags & BCM_MPLS_PORT_NETWORK) &&
        (!mbcm_ltsw_mpls_port_validate(unit, mpls_port->failover_port_id))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_get(unit, vpn, mpls_port->failover_port_id, &fo_port));

        if (mpls_port->flags & BCM_MPLS_PORT_FAILOVER) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mpls_port_failover_validate(unit, fo_port.encap_id,
                                             mpls_port->encap_id));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (mem_info.state) {
    case STATE_INIT:
        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            action = ACT_ADD_PRIMARY;
        } else {
            action = ACT_ADD_ACC;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_init_op(unit, action, vpn, -1, mpls_port));
        break;
    case STATE_ACC:
        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            action = ACT_ADD_PRIMARY;
        } else {
            if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                if (mpls_port->mpls_port_id != mem_info.acc_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
                }
                action = ACT_UPD_ACC;
            } else {
                if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) {
                    if (mpls_port->mpls_port_id == mem_info.acc_id) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
                    }
                }
                action = ACT_ADD_ACC;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_acc_op(unit, action, vpn, -1, mpls_port));
        break;
    case STATE_ACC_ACC:
        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if ((mpls_port->mpls_port_id != mem_info.acc_id) &&
            (mpls_port->mpls_port_id != mem_info.acc_id2)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        action = ACT_UPD_ACC;
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_acc_acc_op(unit, action, vpn, -1, mpls_port));
        break;
    case STATE_PRIMARY:
        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                if (mpls_port->mpls_port_id != mem_info.primary_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
                }
                action = ACT_UPD_PRIMARY;
            } else {
                if (mpls_port->failover_port_id != mem_info.primary_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }

                bcm_mpls_port_t_init(&p_port);
                SHR_IF_ERR_VERBOSE_EXIT
                    (vpws_port_get(unit, vpn,
                                   mem_info.primary_id, &p_port));

                if (mpls_port->flags & BCM_MPLS_PORT_FAILOVER) {
                    action = ACT_ADD_BACKUP;
                } else {
                    /*
                     * A new network port is added.
                     * Then the old primary becomes the backup port.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mpls_port_failover_validate(unit,
                                                     mpls_port->encap_id,
                                                     fo_port.encap_id));
                    action = ACT_ADD_PRIMARY;
                }
            }
        } else {
            if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            action = ACT_ADD_ACC;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_primary_op(unit, action, vpn, -1, mpls_port));
        break;
    case STATE_ACC_PRIMARY:
        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                if (mpls_port->mpls_port_id != mem_info.primary_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
                }
                action = ACT_UPD_PRIMARY;
            } else {
                if (mpls_port->flags & BCM_MPLS_PORT_FAILOVER) {
                    if (mpls_port->failover_port_id != mem_info.primary_id) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mpls_port_failover_validate(unit,
                                                     fo_port.encap_id,
                                                     mpls_port->encap_id));
                    action = ACT_ADD_BACKUP;
                } else if (mpls_port->failover_port_id == mem_info.primary_id) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mpls_port_failover_validate(unit,
                                                     mpls_port->encap_id,
                                                     fo_port.encap_id));
                    action = ACT_ADD_PRIMARY;
                } else {
                    action = ACT_ADD_BACKUP;
                }
            }
        } else {
            if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            action = ACT_UPD_ACC;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_acc_primary_op(unit, action, vpn, -1, mpls_port));
        break;
    case STATE_PRIMARY_BACKUP:
        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (mpls_port->flags & BCM_MPLS_PORT_FAILOVER) {
                if (mpls_port->mpls_port_id != mem_info.backup_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                if (mpls_port->failover_port_id != mem_info.primary_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }

                action = ACT_UPD_BACKUP;
            } else {
                if (mpls_port->mpls_port_id == mem_info.primary_id) {
                    action = ACT_UPD_PRIMARY;
                }else if (mpls_port->mpls_port_id == mem_info.backup_id) {
                    action = ACT_UPD_BACKUP;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            }
        } else {
            if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            action = ACT_ADD_ACC;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_primary_backup_op(unit, action,
                                               vpn, -1, mpls_port));
        break;
    case STATE_ALL:
        if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            if (mpls_port->flags & BCM_MPLS_PORT_FAILOVER) {
                if (mpls_port->mpls_port_id != mem_info.backup_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                if (mpls_port->failover_port_id != mem_info.primary_id) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                action = ACT_UPD_BACKUP;
            } else {
                if (mpls_port->mpls_port_id == mem_info.primary_id) {
                    action = ACT_UPD_PRIMARY;
                }else if (mpls_port->mpls_port_id == mem_info.backup_id) {
                    action = ACT_UPD_BACKUP;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            }
        } else {
            action = ACT_UPD_ACC;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_all_op(unit, action, vpn, -1, mpls_port));
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all VPLS ports in a VPN.
 *
 * \param [in] unit          Unit number.
 * \param [in] vpn           VPN ID.
 * \param [in] port_max      Maximum number of ports in array.
 * \param [in] port_array    Array of mpls ports.
 * \param [in] port_count    Number of ports returned in array.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
static int
vpls_port_get_all(
    int unit, bcm_vpn_t vpn, int port_max,
    bcm_mpls_port_t *port_array, int *port_count)
{
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_vpn_id_validate(unit, vpn));
    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                  BCM_VPN_TYPE_MPLS_VPLS, vpn);
        if (!bcmi_ltsw_virtual_vfi_used_get(unit, vpn_idx,
                                            BCMI_LTSW_VFI_TYPE_MPLS)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_vpls_port_get_all(unit, vpn, port_max,
                                          port_array, port_count));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all VPWS ports in a VPN.
 *
 * \param [in] unit          Unit number.
 * \param [in] vpn           VPN ID.
 * \param [in] port_max      Maximum number of ports in array.
 * \param [in] port_array    Array of mpls ports.
 * \param [in] port_count    Number of ports returned in array.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
static int
vpws_port_get_all(
    int unit, bcm_vpn_t vpn, int port_max,
    bcm_mpls_port_t *port_array, int *port_count)
{
    vpws_vpn_member_info_t mem_info;
    bcm_gport_t mpls_port_id;
    int count = 0;
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx, BCM_VPN_TYPE_MPLS_VPWS, vpn);
    if ((vpn_idx < MPLS_VPWS_VPN_MIN(unit)) ||
        (vpn_idx > MPLS_VPWS_VPN_MAX(unit))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!MPLS_VPWS_VPN_USED_GET(unit, vpn_idx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    mpls_port_id = mem_info.acc_id;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        if (count < port_max) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vpws_port_get(unit, vpn, mpls_port_id, &port_array[count]));
            count++;
        }
    }

    mpls_port_id = mem_info.primary_id;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        if (count < port_max) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vpws_port_get(unit, vpn, mpls_port_id, &port_array[count]));
            count++;
        }
    }

    mpls_port_id = mem_info.backup_id;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        if (count < port_max) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vpws_port_get(unit, vpn, mpls_port_id, &port_array[count]));
            count++;
        }
    }

    *port_count = count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a VPLS port from a VPN.
 *
 * \param [in] unit            Unit number.
 * \param [in] vpn             VPN ID.
 * \param [in] mpls_port_id    mpls port ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
vpls_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                  BCM_VPN_TYPE_MPLS_VPLS, vpn);
        if (!bcmi_ltsw_virtual_vfi_used_get(unit, vpn_idx,
                                            BCMI_LTSW_VFI_TYPE_MPLS)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_port_delete(unit, vpn, mpls_port_id));

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Delete a VPWS port from a VPN.
 *
 * \param [in] unit            Unit number.
 * \param [in] vpn             VPN ID.
 * \param [in] mpls_port_id    mpls port ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
vpws_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
    vpws_vpn_member_info_t mem_info;
    vpws_vpn_member_act_t action;
    int vp;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
    if (vp == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    switch (mem_info.state) {
    case STATE_INIT:
        rv = SHR_E_NOT_FOUND;
        break;
    case STATE_ACC:
        if (mpls_port_id != mem_info.acc_id) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        action = ACT_DEL_ACC;
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_acc_op(unit, action, vpn, mpls_port_id, NULL));
        break;
    case STATE_ACC_ACC:
        if ((mpls_port_id != mem_info.acc_id) &&
            (mpls_port_id != mem_info.acc_id2)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        action = ACT_DEL_ACC;
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_acc_acc_op(unit, action, vpn, mpls_port_id, NULL));
        break;
    case STATE_PRIMARY:
        if (mpls_port_id != mem_info.primary_id) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        action = ACT_DEL_PRIMARY;
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_primary_op(unit, action, vpn, mpls_port_id, NULL));
        break;
    case STATE_ACC_PRIMARY:
        if (mpls_port_id == mem_info.acc_id) {
            action = ACT_DEL_ACC;
        } else if (mpls_port_id == mem_info.primary_id) {
            action = ACT_DEL_PRIMARY;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_acc_primary_op(unit, action, vpn,
                                            mpls_port_id, NULL));
        break;
    case STATE_PRIMARY_BACKUP:
        if (mpls_port_id == mem_info.primary_id) {
            action = ACT_DEL_PRIMARY;
        } else if (mpls_port_id == mem_info.backup_id) {
            action = ACT_DEL_BACKUP;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_primary_backup_op(unit, action, vpn,
                                               mpls_port_id, NULL));
        break;
    case STATE_ALL:
        if (mpls_port_id == mem_info.acc_id) {
            action = ACT_DEL_ACC;
        } else if (mpls_port_id == mem_info.primary_id) {
            action = ACT_DEL_PRIMARY;
        } else if (mpls_port_id == mem_info.backup_id) {
            action = ACT_DEL_BACKUP;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_state_all_op(unit, action, vpn, mpls_port_id, NULL));
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all VPWS ports from a VPN.
 *
 * \param [in] unit            Unit number.
 * \param [in] vpn             VPN ID.
 * \param [in] mpls_port_id    mpls port ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
vpws_port_delete_all(int unit, bcm_vpn_t vpn)
{
    vpws_vpn_member_info_t mem_info;
    bcm_gport_t mpls_port_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vpws_vpn_member_info_get(unit, vpn, &mem_info));

    mpls_port_id = mem_info.acc_id;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete(unit, vpn, mpls_port_id));
    }

    mpls_port_id = mem_info.acc_id2;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete(unit, vpn, mpls_port_id));
    }

    /*
     * Failover port must be deleted first as it will be deleted when the primary
     * port is deleted if the failover port is
     * created with BCM_MPLS_PORT_FAILOVER.
     */
    mpls_port_id = mem_info.backup_id;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete(unit, vpn, mpls_port_id));
    }

    mpls_port_id = mem_info.primary_id;
    if (mpls_port_id != BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete(unit, vpn, mpls_port_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a VPLS port in a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in] mpls_port_id     mpls port ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpls_port_get(
    int unit, bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id, bcm_mpls_port_t *mpls_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_port_get(unit, vpn, mpls_port_id, mpls_port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a VPLS port to a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
vpls_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    bcm_mpls_port_t  fo_port;

    SHR_FUNC_ENTER(unit);

    /* Validate the encap ID for failover case. */
    if ((mpls_port->flags & BCM_MPLS_PORT_NETWORK) &&
        (!mbcm_ltsw_mpls_port_validate(unit, mpls_port->failover_port_id))) {
        bcm_mpls_port_t_init(&fo_port);
        SHR_IF_ERR_VERBOSE_EXIT
            (vpls_port_get(unit, vpn, mpls_port->failover_port_id, &fo_port));

        if (mpls_port->flags & BCM_MPLS_PORT_FAILOVER) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mpls_port_failover_validate(unit, fo_port.encap_id,
                                             mpls_port->encap_id));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mpls_port_failover_validate(unit, mpls_port->encap_id,
                                             fo_port.encap_id));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_port_add(unit, vpn, mpls_port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an MPLS port to a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
mpls_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_add(unit, vpn, mpls_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpls_port_add(unit, vpn, mpls_port));
    }

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS port in a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
static int
mpls_port_get(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_get(unit, vpn, mpls_port->mpls_port_id, mpls_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpls_port_get(unit, vpn, mpls_port->mpls_port_id, mpls_port));
    }

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all MPLS ports in a VPN.
 *
 * \param [in] unit          Unit number.
 * \param [in] vpn           VPN ID.
 * \param [in] port_max      Maximum number of ports in array.
 * \param [in] port_array    Array of mpls ports.
 * \param [in] port_count    Number of ports returned in array.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
int
mpls_port_get_all(
    int unit, bcm_vpn_t vpn, int port_max,
    bcm_mpls_port_t *port_array, int *port_count)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_get_all(unit, vpn, port_max, port_array, port_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpls_port_get_all(unit, vpn, port_max, port_array, port_count));
    }

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an MPLS port from a VPN.
 *
 * \param [in] unit            Unit number.
 * \param [in] vpn             VPN ID.
 * \param [in] mpls_port_id    mpls port ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mpls_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete(unit, vpn, mpls_port_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpls_port_delete(unit, vpn, mpls_port_id));
    }

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all MPLS ports in a VPN.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn        VPN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_port_delete_all_by_vpn(int unit, bcm_vpn_t vpn)
{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete_all(unit, vpn));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_vpls_port_delete_by_vpn(unit, vpn));
    }

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add identifier entry of special label.
 *
 * \param [in] unit          Unit number.
 * \param [in] label_info    Label information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
mpls_special_label_identifier_add(
    int unit,
    bcm_mpls_special_label_t label_info)

{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_special_label_identifier_add(unit, label_info));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get identifier entry of special label.
 *
 * \param [in] unit              Unit number.
 * \param [in/out] label_info    Label information.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_special_label_identifier_get(
    int unit,
    bcm_mpls_special_label_t *label_info)

{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_special_label_identifier_get(unit, label_info));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete identifier entry of special label.
 *
 * \param [in] unit          Unit number.
 * \param [in] label_info    Label information.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_special_label_identifier_delete(
    int unit,
    bcm_mpls_special_label_t label_info)

{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_special_label_identifier_delete(unit, label_info));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all identifier entries of special labels.
 *
 * \param [in] unit    Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_special_label_identifier_delete_all(int unit)

{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_EXIT();
    }

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_special_label_identifier_delete_all(unit));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse and call user call back for all special labels identifiers.
 *
 * \param [in] unit         Unit number.
 * \param [in] cb           user callback.
 * \param [in] user data    user data.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_special_label_identifier_traverse(
    int unit,
    bcm_mpls_special_label_identifier_traverse_cb cb,
    void *user_data)

{
    SHR_FUNC_ENTER(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_special_label_identifier_traverse(unit, cb, user_data));

exit:
    MPLS_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the MPLS VPN.
 *
 * \param [in] unit       Unit number.
 * \param [in] info       VPN configuration info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_vpn_id_create(int unit, bcm_mpls_vpn_config_t *info)
{
    int vpn_idx;
    int locked = 0;
    int vfi_allocated = 0;

    SHR_FUNC_ENTER(unit);

    if (info->flags & BCM_MPLS_VPN_L3) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (info->flags & BCM_MPLS_VPN_REPLACE) {
        if (!(info->flags & BCM_MPLS_VPN_WITH_ID)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if (info->flags & BCM_MPLS_VPN_VPWS) {

        MPLS_LOCK(unit);
        locked = 1;
        if (info->flags & BCM_MPLS_VPN_WITH_ID) {
            if (!BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(info->vpn)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                      BCM_VPN_TYPE_MPLS_VPWS, info->vpn);
            if ((vpn_idx < MPLS_VPWS_VPN_MIN(unit)) ||
                (vpn_idx > MPLS_VPWS_VPN_MAX(unit))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (MPLS_VPWS_VPN_USED_GET(unit, vpn_idx)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
            MPLS_VPWS_VPN_USED_SET(unit, vpn_idx);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mpls_vpws_vpn_alloc(unit, &vpn_idx));
        }
        BCMI_LTSW_VIRTUAL_VPN_SET(info->vpn, BCM_VPN_TYPE_MPLS_VPWS, vpn_idx);
        SHR_EXIT();
    } else if (info->flags & BCM_MPLS_VPN_VPLS) {
        if (info->flags & BCM_MPLS_VPN_WITH_ID) {
            if (!BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(info->vpn)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, info->vpn)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                      BCM_VPN_TYPE_MPLS_VPLS, info->vpn);
            if (bcmi_ltsw_virtual_vfi_used_get(unit, vpn_idx,
                                               BCMI_LTSW_VFI_TYPE_MPLS)) {
                if (!(info->flags & BCM_MPLS_VPN_REPLACE)) {
                    return SHR_E_EXISTS;
                }
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_virtual_vfi_alloc_with_id(
                        unit, BCMI_LTSW_VFI_TYPE_MPLS, vpn_idx));
                vfi_allocated = 1;
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_vfi_alloc(
                    unit, BCMI_LTSW_VFI_TYPE_MPLS, &vpn_idx));
            vfi_allocated = 1;
        }

        /* ING_VFI_TABLE has been inserted by bcmi_ltsw_virtual_vfi_alloc. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_vfi_op(unit, BCMLT_OPCODE_UPDATE, vpn_idx, info));
        BCMI_LTSW_VIRTUAL_VPN_SET(info->vpn, BCM_VPN_TYPE_MPLS_VPLS, vpn_idx);
    }

exit:

    if (SHR_FUNC_ERR()) {
        if (vfi_allocated) {
            (void)bcmi_ltsw_virtual_vfi_free(unit,
                                             BCMI_LTSW_VFI_TYPE_MPLS, vpn_idx);
        }
    }
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the MPLS VPN.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn        VPN ID.
 * \param [out] info      VPN  info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_vpn_id_get(int unit, bcm_vpn_t vpn, bcm_mpls_vpn_config_t *info)
{
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {

        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                  BCM_VPN_TYPE_MPLS_VPWS, vpn);
        if ((vpn_idx < MPLS_VPWS_VPN_MIN(unit)) ||
            (vpn_idx > MPLS_VPWS_VPN_MAX(unit))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!MPLS_VPWS_VPN_USED_GET(unit, vpn_idx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        BCMI_LTSW_VIRTUAL_VPN_SET(info->vpn, BCM_VPN_TYPE_MPLS_VPWS, vpn_idx);
        info->flags |= BCM_MPLS_VPN_VPWS;
        SHR_EXIT();
    } else if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
        if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, vpn)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                  BCM_VPN_TYPE_MPLS_VPLS, vpn);
        if (!bcmi_ltsw_virtual_vfi_used_get(unit, vpn_idx,
                                            BCMI_LTSW_VFI_TYPE_MPLS)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_vfi_op(unit, BCMLT_OPCODE_LOOKUP, vpn_idx, info));
        BCMI_LTSW_VIRTUAL_VPN_SET(info->vpn, BCM_VPN_TYPE_MPLS_VPLS, vpn_idx);
        info->flags |= BCM_MPLS_VPN_VPLS;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all MPLS VPNs.
 *
 * \param [in] unit          Unit number.
 * \param [in] cb            User callback function, called once per VPN.
 * \param [out] user_data    Cookie.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
static int
mpls_vpn_traverse(
    int unit,
    bcm_mpls_vpn_traverse_cb cb,
    void *user_data)
{
    int idx, rv = SHR_E_NONE;
    int vpn;
    bcm_mpls_vpn_config_t info;
    uint32_t vfi_min, vfi_max;

    SHR_FUNC_ENTER(unit);

    /* Traverse VPWS VPNs. */
    for (idx = MPLS_VPWS_VPN_MIN(unit); idx <= MPLS_VPWS_VPN_MAX(unit); idx++) {
        if (!MPLS_VPWS_VPN_USED_GET(unit, idx)) {
            continue;
        }

        BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCM_VPN_TYPE_MPLS_VPWS, idx);
        bcm_mpls_vpn_config_t_init(&info);
        SHR_IF_ERR_CONT
            (mpls_vpn_id_get(unit, vpn, &info));

        rv = cb(unit, &info, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    /* Traverse VPLS VPNs. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_range_get(unit, &vfi_min, &vfi_max));
    for (idx = vfi_min; idx <= vfi_max; idx++) {
        if (!bcmi_ltsw_virtual_vfi_used_get(unit, idx,
                                            BCMI_LTSW_VFI_TYPE_MPLS)) {
            continue;
        }

        BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCM_VPN_TYPE_MPLS_VPLS, idx);
        bcm_mpls_vpn_config_t_init(&info);
        SHR_IF_ERR_CONT
            (mpls_vpn_id_get(unit, vpn, &info));

        rv = cb(unit, &info, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the MPLS VPN.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn        VPN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_vpn_id_destroy(int unit, bcm_vpn_t vpn)
{
    int vpn_idx;

    SHR_FUNC_ENTER(unit);

    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                  BCM_VPN_TYPE_MPLS_VPWS, vpn);
        if ((vpn_idx < MPLS_VPWS_VPN_MIN(unit)) ||
            (vpn_idx > MPLS_VPWS_VPN_MAX(unit))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!MPLS_VPWS_VPN_USED_GET(unit, vpn_idx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vpws_port_delete_all(unit, vpn));
        MPLS_VPWS_VPN_USED_CLR(unit, vpn_idx);
    } else if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
        if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, vpn)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        BCMI_LTSW_VIRTUAL_VPN_GET(vpn_idx,
                                  BCM_VPN_TYPE_MPLS_VPLS, vpn);

        if (!bcmi_ltsw_virtual_vfi_used_get(unit, vpn_idx,
                                            BCMI_LTSW_VFI_TYPE_MPLS)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_vpls_port_delete_by_vpn(unit, vpn));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_free(unit,
                                        BCMI_LTSW_VFI_TYPE_MPLS, vpn_idx));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all MPLS VPNs.
 *
 * \param [in] unit       Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_vpn_id_destroy_all(int unit)
{
    int idx;
    bcm_vpn_t vpn;
    uint32_t vfi_min, vfi_max;

    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_EXIT();
    }

    /* Destroy all VPWS VPNs. */
    for (idx = MPLS_VPWS_VPN_MIN(unit); idx <= MPLS_VPWS_VPN_MAX(unit); idx++) {
        if (!MPLS_VPWS_VPN_USED_GET(unit, idx)) {
            continue;
        }

        BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCM_VPN_TYPE_MPLS_VPWS, idx);
        SHR_IF_ERR_VERBOSE_EXIT
            (mpls_vpn_id_destroy(unit, vpn));
    }


    /* Destroy all VPLS VPNs. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_vpls_port_delete_all(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_range_get(unit, &vfi_min, &vfi_max));
    for (idx = vfi_min; idx <= vfi_max; idx++) {
        if (!bcmi_ltsw_virtual_vfi_used_get(unit, idx,
                                            BCMI_LTSW_VFI_TYPE_MPLS)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_free(unit,
                                        BCMI_LTSW_VFI_TYPE_MPLS, idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable the MPLS on all ports.
 *
 * \param [in] unit        Unit number.
 * \param [in] enable    Enable indicator.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
mpls_enable(int unit, int enable)
{
    bcm_pbmp_t  pbmp_all;
    bcm_port_t port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp_all));

    BCM_PBMP_ITER(pbmp_all, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_control_set(unit, port,
                                       bcmPortControlMpls, !!enable));
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
/*!
 * \brief Display MPLS software structure information.
 *
 * This function is used to display MPLS software structure information.
 *
 * \retval None
 */
void
bcmi_ltsw_mpls_sw_dump(int unit)
{
    ltsw_mpls_info_t *mi = MPLS_INFO(unit);
    int i = 0, count;
    vpws_vpn_member_info_t  *mem_info;
    bcm_vpn_t vpn;
    const char *mem_state[] = {"Invalid",
                               "Init",
                               "Access",
                               "Access_access",
                               "Primary",
                               "Acc_primary",
                               "Primary_backup",
                               "All"};

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        return;
    }

    if (!mi->initialized) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information MPLS - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init               : %4d\n"), mi->initialized));
    LOG_CLI((BSL_META_U(unit,
                        "  vpws_vpn_min       : %4d\n"), mi->vpws_vpn_min));
    LOG_CLI((BSL_META_U(unit,
                        "  vpws_vpn_max       : %4d\n"), mi->vpws_vpn_max));

    LOG_CLI((BSL_META_U(unit,
                        "  vpws vpn created list:\n")));

    count = 0;
    for (i = mi->vpws_vpn_min; i <= mi->vpws_vpn_max; i++) {
        if (!SHR_BITGET(mi->vpws_vpn_bmp, i)) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit, "  %4d"), i));
        count++;
        if (count % 10 == 0) {
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nvpws vpn member info:\n")));
    LOG_CLI((BSL_META_U(unit,
                  "  VPN     State    Acc port  Acc port2  Primary port  Backup port\n")));
    count = 0;
    for (i = mi->vpws_vpn_min; i <= mi->vpws_vpn_max; i++) {
        mem_info = &(mi->vpws_vpn_member_info[i]);

        if (mem_info->state == STATE_INIT) {
            continue;
        }
        BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCM_VPN_TYPE_MPLS_VPWS, i);
        LOG_CLI((BSL_META_U(unit,
                     "  %#4x   %s    "), vpn, mem_state[mem_info->state]));

        if (mem_info->acc_id != BCM_GPORT_INVALID) {
            LOG_CLI((BSL_META_U(unit, "  %#x"), mem_info->acc_id));
        } else {
            LOG_CLI((BSL_META_U(unit, "        ")));
        }

        if (mem_info->acc_id2 != BCM_GPORT_INVALID) {
            LOG_CLI((BSL_META_U(unit, "  %#x"), mem_info->acc_id2));
        } else {
            LOG_CLI((BSL_META_U(unit, "        ")));
        }

        if (mem_info->primary_id != BCM_GPORT_INVALID) {
            LOG_CLI((BSL_META_U(unit, "  %#x"), mem_info->primary_id));
        } else {
            LOG_CLI((BSL_META_U(unit, "        ")));
        }

        if (mem_info->backup_id != BCM_GPORT_INVALID) {
            LOG_CLI((BSL_META_U(unit, "  %#x"), mem_info->backup_id));
        } else {
            LOG_CLI((BSL_META_U(unit, "        ")));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Dump device-specific SW MPLS info. */
    (void)mbcm_ltsw_mpls_sw_dump(unit);

    return;
}

/*!
 * \brief Get MPLS global control.
 *
 * \param [in]    unit Unit Number.
 * \param [in]    control_type Control type.
 * \param [in]    value Control value
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcmi_ltsw_mpls_global_control_set(
    int unit,
    bcmi_ltsw_mpls_global_ctrl_t control_type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_global_control_set(unit, control_type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get MPLS global control.
 *
 * \param [in]     unit Unit Number.
 * \param [in]     control_type Control type.
 * \param [out]    value Control value
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcmi_ltsw_mpls_global_control_get(
    int unit,
    bcmi_ltsw_mpls_global_ctrl_t control_type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_global_control_get(unit, control_type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add dst mac into MAC_DA profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] mac Dst MAC
 * \param [out] index Profile index.
 */
int
bcmi_ltsw_mpls_tnl_dst_mac_profile_add(
    int unit,
    uint64_t mac,
    int *index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_dst_mac_profile_add(unit, mac, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete dst mac from MAC_DA profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile index.
 */
int
bcmi_ltsw_mpls_tnl_dst_mac_profile_del(
    int unit,
    int index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_dst_mac_profile_del(unit, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get dst mac from MAC_DA profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile index.
 * \param [out] mac Dst MAC
 */
int
bcmi_ltsw_mpls_tnl_dst_mac_profile_get(
    int unit,
    int index,
    uint64_t *mac)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_tnl_dst_mac_profile_get(unit, index, mac));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set mpls global label range.
 *
 * \param [in]    unit Unit Number.
 * \param [in]    range_num Range number. 0 : First range, 1 : Second range.
 * \param [in]    range_min Range min indicator. 0 : Range max, 1: Range min
 * \param [in]    label_val Label value
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcmi_ltsw_mpls_global_label_range_set(
    int unit,
    int range_num,
    int range_min,
    int label_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_global_label_range_set(unit, range_num,
                                               range_min, label_val));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get mpls global label range.
 *
 * \param [in]     unit Unit Number.
 * \param [in]     range_num Range number. 0 : First range, 1 : Second range.
 * \param [in]     min_label Max label value.
 * \param [out]    max_label Max label value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcmi_ltsw_mpls_global_label_range_get(
    int unit,
    int range_num,
    int *min_label,
    int *max_label)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_global_label_range_get(unit, range_num,
                                               min_label, max_label));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add MPLS encap labels.
 *
 * \param [in] unit Unit Number.
 * \param [in] label_info  Encap label info.
 * \param [out] label_index VC label index.
 */
int
bcmi_ltsw_mpls_encap_label_add(
    int unit,
    bcmi_ltsw_mpls_encap_label_info_t *label_info,
    int *label_index)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    if (label_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!LTSW_MPLS_LBL_VALID(label_info->vc_label)) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!LTSW_MPLS_EXP_VALID(label_info->exp)) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!LTSW_MPLS_TTL_VALID(label_info->ttl)) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_encap_label_add(unit, label_info, label_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get MPLS encap labels.
 *
 * \param [in] unit Unit Number.
 * \param [in] label_index VC label index.
 * \param [out] label_info  Encap label info.
 */
int
bcmi_ltsw_mpls_encap_label_get(
    int unit,
    int label_index,
    bcmi_ltsw_mpls_encap_label_info_t *label_info
    )
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    if (label_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_encap_label_get(unit, label_index, label_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete MPLS encap labels.
 *
 * \param [in] unit Unit Number.
 * \param [in] label_index VC label index.
 */
int
bcmi_ltsw_mpls_encap_label_delete(
    int unit,
    int label_index
    )
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_encap_label_delete(unit, label_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Increase reference count of VC label index.
 *
 * \param [in] unit Unit Number.
 * \param [in] label_index VC label index.
 */
int
bcmi_ltsw_mpls_encap_label_ref_inc(
    int unit,
    int label_index
    )
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_encap_label_ref_inc(unit, label_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Match the match rule of mpls port ID to vp_lag_vp.
 *
 * \param [in] unit Unit Number.
 * \param [in] mpls_port_id mpls port ID.
 * \param [in] vp_lag_vp VPLAG vp ID.
 */
int
bcmi_ltsw_mpls_port_source_vp_lag_set(
    int unit,
    bcm_gport_t mpls_port_id,
    int vp_lag_vp)

{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_port_match_vp_replace(unit, mpls_port_id, vp_lag_vp));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the VP matched to the match rule of mpls port ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] mpls_port_id mpls port ID.
 * \param [out] vp_lag_vp VPLAG vp ID.
 */
int
bcmi_ltsw_mpls_port_source_vp_lag_get(
    int unit,
    bcm_gport_t mpls_port_id,
    int *vp_lag_vp)

{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    if (vp_lag_vp == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_port_match_vp_get(unit, mpls_port_id, vp_lag_vp));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore the match rule of mpls port ID to itself.
 *
 * \param [in] unit Unit Number.
 * \param [in] mpls_port_id mpls port ID.
 * \param [in] vp_lag_vp VPLAG vp ID.
 */
int
bcmi_ltsw_mpls_port_source_vp_lag_clr(
    int unit,
    bcm_gport_t mpls_port_id,
    int vp_lag_vp)

{
    int vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);

    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_port_match_vp_replace(unit, mpls_port_id, vp));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mpls_port_match_set(
    int unit,
    bcm_gport_t mpls_port_id,
    bcmi_ltsw_mpls_port_match_t *match)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_vpn_id_validate(unit, match->vpn));

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_port_match_set(unit, mpls_port_id, match));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mpls_port_match_clear(
    int unit,
    bcm_gport_t mpls_port_id,
    bcmi_ltsw_mpls_port_match_t *match)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_vpn_id_validate(unit, match->vpn));

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_port_match_clear(unit, mpls_port_id, match));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mpls_port_match_multi_get(
    int unit,
    bcm_gport_t mpls_port_id,
    int size,
    bcmi_ltsw_mpls_port_match_t *match_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT(
        mbcm_ltsw_mpls_port_match_multi_get(unit, mpls_port_id,
                                            size, match_array, count));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_mpls_lt_get(
    int unit,
    bcmint_mpls_lt_id_t lt_id,
    const bcmint_mpls_lt_t **lt_info)
{
    bcmint_mpls_lt_db_t lt_db;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_lt_db_get(unit, &lt_db));

    if (!(lt_db.lt_bmp & (1 << lt_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *lt_info = &lt_db.lts[lt_id];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize MPLS module.
 *
 * This function is used to initialize MPLS module.
 *
 * \param [in]  unit       Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_mpls_cleanup(int unit)
{
    ltsw_mpls_info_t *mi = MPLS_INFO(unit);
    int bcm_init = bcmi_ltsw_init_state_get(unit);
    int locked = 0;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (mi->initialized == FALSE) {
        SHR_EXIT();
    }

    MPLS_LOCK(unit);

    locked = 1;

    if (warm) {
        SHR_EXIT();
    }

    mi->initialized = FALSE;

    

    if (!bcm_init) {
        /* Destroy all terminators. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mpls_tnl_switch_delete_all(unit));

        if (!ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
            /* Destroy all initiators. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_mpls_tnl_encap_destroy_all(unit));

            /* Destroy port matches via bcm_port_match_add. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_mpls_port_match_delete_all(unit));

            /* Destroy all VPNs. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mpls_vpn_id_destroy_all(unit));

            /* Delete all special labels. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mpls_special_label_identifier_delete_all(unit));
        }
    }

    /* Device-specific MPLS detach. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_detach(unit));

    locked = 0;

    MPLS_UNLOCK(unit);

    mpls_info_cleanup(unit);

    /* Disable the MPLS. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_enable(unit, FALSE));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize MPLS module.
 *
 * This function is used to initialize MPLS module.
 *
 * \param [in]  unit      Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_init(int unit)
{
    ltsw_mpls_info_t *mi = MPLS_INFO(unit);
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (mi->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_mpls_cleanup(unit));
    }

    /* Initialize generic MPLS information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_info_init(unit));

    /* Device-specific MPLS init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_init(unit));

    if (!warm) {
        /* Enable the MPLS. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mpls_enable(unit, TRUE));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_global_label_range_set(unit, 0, 1, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_global_label_range_set(unit, 0, 0, ((1 << 20) - 1)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_global_label_range_set(unit, 1, 1, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_global_label_range_set(unit, 1, 0, ((1 << 20) - 1)));
    }

    mi->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an MPLS tunnel terminator.
 *
 * \param [in] unit        Unit number.
 * \param [in] info        MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    if (!LTSW_MPLS_LBL_VALID(info->label)) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_switch_add(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an MPLS tunnel terminator.
 *
 * \param [in] unit        Unit number.
 * \param [in] info        MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    if (info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!LTSW_MPLS_LBL_VALID(info->label)) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_switch_delete(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS tunnel terminator.
 *
 * \param [in] unit        Unit number.
 * \param [in] info        MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_switch_get(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all MPLS tunnel terminators.
 *
 * \param [in] unit        Unit number.
 * \param [in] info        MPLS tunnel terminator info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_mpls_tunnel_switch_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_switch_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse MPLS tunnel terminators.
 *
 * \param [in] unit          Unit number.
 * \param [in] cb            User callback function, called once per terminator.
 * \param [out] user_data    Cookie.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
int
bcm_ltsw_mpls_tunnel_switch_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_switch_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the MPLS tunnel initiator parameters for an L3 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf The egress L3 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_mpls_tunnel_initiator_clear(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_initiator_clear(unit, intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all the MPLS tunnel initiator parameters all L3 Interfaces.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_mpls_tunnel_initiator_clear_all(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_initiator_clear_all(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the the MPLS tunnel initiator parameters for an L3 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf The egress L3 interface.
 * \param [in] label_max Number of entries in label_array.
 * \param [out] label_array MPLS header information.
 * \param [out] label_count Actual number of labels returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_mpls_egress_label_t *label_array,
    int *label_count)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_initiator_get(unit, intf, label_max,
                                   label_array, label_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the MPLS tunnel initiator parameters for an L3 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf The egress L3 interface.
 * \param [in] num_labels Number of entries in label_array.
 * \param [in] label_array MPLS header information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t *label_array)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_initiator_set(unit, intf, num_labels, label_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an MPLS tunnel initiator.
 *
 * \param [in] unit            Unit number.
 * \param [in] options         Operation options.
 * \param [in] tunnel_encap    MPLS tunnel initiator info.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_mpls_tunnel_encap_create(
    int unit,
    uint32_t options,
    bcm_mpls_tunnel_encap_t *tunnel_encap)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(tunnel_encap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_encap_create(unit, options, tunnel_encap));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS tunnel initiator.
 *
 * \param [in] unit                Unit number.
 * \param [in/out] tunnel_encap    MPLS tunnel initiator info.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcm_ltsw_mpls_tunnel_encap_get(
    int unit,
    bcm_mpls_tunnel_encap_t *tunnel_encap)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(tunnel_encap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_encap_get(unit, tunnel_encap));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an MPLS tunnel initiator.
 *
 * \param [in] unit         Unit number.
 * \param [in] tunnel_id    MPLS tunnel initiator ID.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
 bcm_ltsw_mpls_tunnel_encap_destroy(
    int unit,
    bcm_gport_t tunnel_id)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_encap_destroy(unit, tunnel_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all MPLS tunnel initiators.
 *
 * \param [in] unit        Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
 bcm_ltsw_mpls_tunnel_encap_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_tunnel_encap_destroy_all(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the MPLS VPN.
 *
 * \param [in] unit       Unit number.
 * \param [out] info      VPN  info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_vpn_id_create(int unit, bcm_mpls_vpn_config_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_EXIT();
    }

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_vpn_id_create(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the MPLS VPN.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn        VPN ID.
 * \param [out] info      VPN  info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_vpn_id_get(int unit, bcm_vpn_t vpn, bcm_mpls_vpn_config_t *info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT
        (mpls_vpn_id_get(unit, vpn, info));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the MPLS VPN.
 *
 * \param [in] unit       Unit number.
 * \param [in] vpn        VPN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_vpn_id_destroy(int unit, bcm_vpn_t vpn)
{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);
    locked = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_vpn_id_destroy(unit, vpn));

exit:
    if (locked) {
        MPLS_LOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all MPLS VPNs.
 *
 * \param [in] unit       Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_vpn_id_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT
        (mpls_vpn_id_destroy_all(unit));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse MPLS VPNs.
 *
 * \param [in] unit          Unit number.
 * \param [in] cb            User callback function, called once per VPN.
 * \param [out] user_data    Cookie.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
int
bcm_ltsw_mpls_vpn_traverse(
    int unit,
    bcm_mpls_vpn_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    MPLS_LOCK(unit);

    SHR_IF_ERR_CONT
        (mpls_vpn_traverse(unit, cb, user_data));

    MPLS_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an MPLS port to a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(mpls_port, SHR_E_PARAM);

    if ((mpls_port->flags & BCM_MPLS_PORT_FAILOVER) &&
        !(mpls_port->flags & BCM_MPLS_PORT_NETWORK)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    MPLS_VPN_ID_CHECK(unit, vpn);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_vpn_id_validate(unit, vpn));

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_port_add(unit, vpn, mpls_port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an MPLS port from a VPN.
 *
 * \param [in] unit             Unit number.
 * \param [in] vpn              VPN ID.
 * \param [in/out] mpls_port    mpls port information.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_port_get(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(mpls_port, SHR_E_PARAM);

    MPLS_VPN_ID_CHECK(unit, vpn);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_port_get(unit, vpn, mpls_port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all MPLS ports in a VPN.
 *
 * \param [in] unit          Unit number.
 * \param [in] vpn           VPN ID.
 * \param [in] port_max      Maximum number of ports in array.
 * \param [in] port_array    Array of mpls ports.
 * \param [in] port_count    Number of ports returned in array.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
int
bcm_ltsw_mpls_port_get_all(
    int unit, bcm_vpn_t vpn, int port_max,
    bcm_mpls_port_t *port_array, int *port_count)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    if ((port_max > 0) && (port_array == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(port_count, SHR_E_PARAM);

    MPLS_VPN_ID_CHECK(unit, vpn);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_port_get_all(unit, vpn, port_max, port_array, port_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an MPLS port from a VPN.
 *
 * \param [in] unit            Unit number.
 * \param [in] vpn             VPN ID.
 * \param [in] mpls_port_id    mpls port ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_mpls_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_VPN_ID_CHECK(unit, vpn);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_port_delete(unit, vpn, mpls_port_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all MPLS ports from a VPN.
 *
 * \param [in] unit            Unit number.
 * \param [in] vpn             VPN ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_mpls_port_delete_all(int unit, bcm_vpn_t vpn)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_VPN_ID_CHECK(unit, vpn);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_port_delete_all_by_vpn(unit, vpn));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add identifier entry of special label.
 *
 * \param [in] unit          Unit number.
 * \param [in] label_type    Not used.
 * \param [in] label_info    Label information.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_special_label_identifier_add(
    int unit,
    bcm_mpls_special_label_type_t label_type,
    bcm_mpls_special_label_t label_info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_SPECIAL_LABEL_FEATURE_CHECK(unit);

    MPLS_SPECIAL_LABEL_INFO_CHECK(label_info);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_special_label_identifier_add(unit, label_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get identifier entry of special label.
 *
 * \param [in] unit          Unit number.
 * \param [in] label_type    Not used.
 * \param [in] label_info    Label information.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_special_label_identifier_get(
    int unit,
    bcm_mpls_special_label_type_t label_type,
    bcm_mpls_special_label_t *label_info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_SPECIAL_LABEL_FEATURE_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_special_label_identifier_get(unit, label_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete identifier entry of special label.
 *
 * \param [in] unit          Unit number.
 * \param [in] label_type    Not used.
 * \param [in] label_info    Label information.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_special_label_identifier_delete(
    int unit,
    bcm_mpls_special_label_type_t label_type,
    bcm_mpls_special_label_t label_info)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_SPECIAL_LABEL_FEATURE_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_special_label_identifier_delete(unit, label_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all identifier entries of special labels.
 *
 * \param [in] unit    Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_special_label_identifier_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_SPECIAL_LABEL_FEATURE_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_special_label_identifier_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse and call user call back for all special labels identifiers.
 *
 * \param [in] unit         Unit number.
 * \param [in] cb           user callback.
 * \param [in] user_data    user data.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_mpls_special_label_identifier_traverse(
    int unit,
    bcm_mpls_special_label_identifier_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_L2_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_INIT_CHECK(unit);

    MPLS_SPECIAL_LABEL_FEATURE_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mpls_special_label_identifier_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counters entries to the given mpls label and gport.
 *
 * \param [in] unit                    Unit number.
 * \param [in] label                   MPLS Label.
 * \param [in] port                   MPLS Gport.
 * \param [in] stat_counter_id  Stat Counter ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int bcm_ltsw_mpls_label_stat_attach(
    int              unit,
    bcm_mpls_label_t label,
    bcm_gport_t      port,
    uint32           stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MPLS_INIT_CHECK(unit);

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if (ci.source != bcmFlexctrSourceMplsLabel) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction = ci.direction;

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_flexctr_info_get(unit,
                                               label,
                                               port,
                                               &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mpls_label_flexctr_attach(unit, label, port, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}



/*!
 * \brief Detach counters entries to the given mpls label and gport.
 *
 * \param [in] unit            Unit number.
 * \param [in] label           MPLS Label.
 * \param [in] port           MPLS Gport.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int bcm_ltsw_mpls_label_stat_detach(
    int              unit,
    bcm_mpls_label_t label,
    bcm_gport_t      port)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bool locked = false;
    uint32_t ctr_id;

    SHR_FUNC_ENTER(unit);

    MPLS_INIT_CHECK(unit);

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_flexctr_info_get(unit,
                                               label,
                                               port,
                                               &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_flexctr_detach(unit, label, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get stat counter id associated with the given mpls label and gport.
 *
 * \param [in] unit                      Unit number.
 * \param [in] label                     MPLS Label.
 * \param [in] port                     MPLS Gport.
 * \param [in] stat                      Type of the counter to retrieve.
 * \param [out] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int bcm_ltsw_mpls_label_stat_id_get(
    int                  unit,
    bcm_mpls_label_t     label,
    bcm_gport_t          port,
    bcm_mpls_stat_t      stat,
    uint32               *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MPLS_INIT_CHECK(unit);

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_flexctr_info_get(unit,
                                               label,
                                               port,
                                               &ci));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set flex counter object value for the given MPLS label and gport.
 *
 * \param [in] unit Unit number.
 * \param [in] label MPLS label.
 * \param [in] port Gport.
 * \param [in] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_mpls_label_flexctr_object_set(
    int unit,
    bcm_mpls_label_t label,
    bcm_gport_t port,
    uint32_t value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_flexctr_object_set(unit,
                                                 label,
                                                 port,
                                                 value));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter object value for the given MPLS label and gport.
 *
 * \param [in] unit Unit number.
 * \param [in] label MPLS label.
 * \param [in] port Gport.
 * \param [in] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_mpls_label_flexctr_object_get(
    int unit,
    bcm_mpls_label_t label,
    bcm_gport_t port,
    uint32_t *value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_flexctr_object_get(unit,
                                                 label,
                                                 port,
                                                 value));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an MPLS control packet policy.
 *
 * \param [in] unit             Unit number.
 * \param [in] options          Operation options.
 * \param [in] match            MPLS control packet match.
 * \param [in] action           MPLS control packet action.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_control_pkt_add(
    int unit,
    uint32 options,
    bcm_mpls_control_pkt_match_t *match,
    bcm_mpls_control_pkt_action_t *action)

{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_MPLS_CONTROL_PKT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_control_pkt_add(unit, options, match, action));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an MPLS control packet policy.
 *
 * \param [in] unit             Unit number.
 * \param [in/out] match        MPLS control packet match.
 * \param [out] action          MPLS control packet action.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_control_pkt_get(
    int unit,
    bcm_mpls_control_pkt_match_t *match,
    bcm_mpls_control_pkt_action_t *action)

{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_MPLS_CONTROL_PKT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_control_pkt_get(unit, match, action));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an MPLS control packet policy.
 *
 * \param [in] unit             Unit number.
 * \param [in] match            MPLS control packet match.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_control_pkt_delete(
    int unit,
    bcm_mpls_control_pkt_match_t *match)

{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_MPLS_CONTROL_PKT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_control_pkt_delete(unit, match));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all MPLS control packet policies.
 *
 * \param [in] unit             Unit number.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_control_pkt_delete_all(int unit)

{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_MPLS_CONTROL_PKT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_control_pkt_delete_all(unit));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all MPLS control packet policies and call user's callback.
 *
 * \param [in] unit             Unit number.
 * \param [in] cb               User callback.
 * \param [in] user_data        User data.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_control_pkt_traverse(
    int unit,
    bcm_mpls_control_pkt_traverse_cb cb,
    void *user_data)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_MPLS_CONTROL_PKT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_control_pkt_traverse(unit, cb, user_data));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get MPLS PW Stats.
 *
 * \param [in] unit             Unit number.
 * \param [in] label            MPLS label.
 * \param [in] port            MPLS port.
 * \param [in] stat            Stat type.
 * \param [out] val            Stat value.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_label_stat_get(
    int unit,
    bcm_mpls_label_t label,
    bcm_gport_t port,
    bcm_mpls_stat_t stat,
    uint64 *val)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);
    locked = true;

    SHR_NULL_CHECK(val, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_stat_get(unit, label, port, stat, val));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear MPLS PW Stats.
 *
 * \param [in] unit             Unit number.
 * \param [in] label            MPLS label.
 * \param [in] port            MPLS port.
 * \param [in] stat            Stat type.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_mpls_label_stat_clear(
    int unit,
    bcm_mpls_label_t label,
    bcm_gport_t port,
    bcm_mpls_stat_t stat)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MPLS_FEATURE_CHECK(unit);

    MPLS_INIT_CHECK(unit);

    MPLS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_label_stat_clear(unit, label, port, stat));

exit:
    if (locked) {
        MPLS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
