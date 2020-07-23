/*! \file trunk.c
 *
 * BCM level APIs for trunking (a.k.a. Port Aggregation).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/error.h>
#include <bcm/trunk.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/trunk_int.h>
#include <bcm_int/ltsw/generated/trunk_ha.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/mbcm/trunk.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/l3_egress.h>

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_TRUNK

/*!
 * \brief Trunk database.
 */
typedef struct ltsw_trunk_db_s {

    /*! Trunk module lock. */
    sal_mutex_t lock;

    /*! Trunk public database. */
    bcmint_trunk_pub_info_t t_pub;

    /*! Trunk private database. */
    bcmint_trunk_priv_info_t *t_priv;

} ltsw_trunk_db_t;

/*!
 * \brief One trunk control entry for each SOC device containing trunk
 * book keeping information for that device.
 */
static ltsw_trunk_db_t bcm_trunk_db[BCM_MAX_NUM_UNITS];

/*! Trunk database. */
#define TRUNK_DB(unit) (&bcm_trunk_db[unit])

/*! Trunk public information. */
#define TRUNK_PUB(unit) \
    (&TRUNK_DB(unit)->t_pub)

/*! Trunk private information. */
#define TRUNK_PRIV(unit, tid) \
    (&TRUNK_DB(unit)->t_priv[tid])

/*! Current configured trunk information. */
#define TRUNK_INFO(unit, tid) \
    (TRUNK_PRIV(unit, tid)->t_info)

/*! Front panel trunk number per unit. */
#define TRUNK_NUM_FP_GROUPS(unit) \
    (TRUNK_PUB(unit)->ngroups_fp)

/*! Fabric trunk number per unit. */
#define TRUNK_NUM_FABRIC_GROUPS(unit) \
    (TRUNK_PUB(unit)->ngroups_fabric)

/*! VPLAG number per unit. */
#define TRUNK_NUM_VIRTUAL_GROUPS(unit) \
    (TRUNK_PUB(unit)->ngroups_vplag)

/*! Total trunk group number per unit. */
#define TRUNK_NUM_GROUPS(unit) \
    (TRUNK_NUM_FP_GROUPS(unit) + TRUNK_NUM_FABRIC_GROUPS(unit) + TRUNK_NUM_VIRTUAL_GROUPS(unit))

/*! Total front panel trunk max number of ports. */
#define TRUNK_NUM_FP_PORTS(unit) \
    (TRUNK_PUB(unit)->nports_fp)

/*! Total fabric trunk max number of ports. */
#define TRUNK_NUM_FABRIC_PORTS(unit) \
    (TRUNK_PUB(unit)->nports_fabric)

/*! Total VPLAG max number of virtual ports. */
#define TRUNK_NUM_VIRTUAL_PORTS(unit) \
    (TRUNK_PUB(unit)->nvps_vplag)

/*! Total VPLAG max number of weighted virtual ports. */
#define TRUNK_NUM_WEIGHTED_VIRTUAL_PORTS(unit) \
    (TRUNK_PUB(unit)->nvps_weighted_vplag)

/*! The base virtual port index for VPLAG. */
#define TRUNK_VIRTUAL_BASE_INDEX(unit) \
    (TRUNK_PUB(unit)->base_vplag_idx)

/*! Total max number of failto ports for front panel trunk. */
#define TRUNK_NUM_FP_FTPORTS(unit) \
    (TRUNK_PUB(unit)->nftports_fp)

/*! Total max number of failto ports for fabric trunk. */
#define TRUNK_NUM_FABRIC_FTPORTS(unit) \
    (TRUNK_PUB(unit)->nftports_fabric)

/*! Cause a routine to return SHR_E_INIT if trunking subsystem is not initialized. */
#define TRUNK_INIT(unit) \
    do { \
        if (((TRUNK_NUM_FP_GROUPS(unit) <= 0) && \
            (TRUNK_NUM_FABRIC_GROUPS(unit) <= 0)) || \
            (TRUNK_DB(unit)->t_priv == NULL)) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/*! Make sure TID is within valid range. */
#define TRUNK_CHECK(unit, tid) \
    do { \
        if ((tid) < 0 || (tid) >= TRUNK_NUM_GROUPS(unit)) { \
            SHR_ERR_EXIT(SHR_E_PARAM); \
        } \
    } while (0)

/*! Valid front panel trunk number range. */
#define TRUNK_FP_TID(unit, tid) \
    ((tid) >= 0 && (tid) < TRUNK_NUM_FP_GROUPS(unit))

/*! Valid fabric trunk number range. */
#define TRUNK_FABRIC_TID(unit, tid)             \
    ((tid) >= TRUNK_NUM_FP_GROUPS(unit) && \
     (tid) < (TRUNK_NUM_FP_GROUPS(unit) + TRUNK_NUM_FABRIC_GROUPS(unit)))

/*! Valid fabric trunk number range. */
#define TRUNK_VIRTUAL_TID(unit, tid)             \
    ((tid) >= (TRUNK_NUM_FP_GROUPS(unit) + TRUNK_NUM_FABRIC_GROUPS(unit)) && \
     (tid) < TRUNK_NUM_GROUPS(unit))

/*! Trunk mutax lock. */
#define TRUNK_LOCK(unit) \
    do { \
        if (TRUNK_DB(unit)->lock) { \
            sal_mutex_take(TRUNK_DB(unit)->lock, SAL_MUTEX_FOREVER); \
        } \
    } while (0)

/*! Trunk mutax unlock. */
#define TRUNK_UNLOCK(unit) \
    do { \
        if (TRUNK_DB(unit)->lock) { \
            sal_mutex_give(TRUNK_DB(unit)->lock); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Create protection mutex for trunk module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (TRUNK_DB(unit)->lock == NULL) {
        TRUNK_DB(unit)->lock = sal_mutex_create("bcmTrunkCntlMutex");
        SHR_NULL_CHECK(TRUNK_DB(unit)->lock, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for trunk module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
ltsw_trunk_lock_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (TRUNK_DB(unit)->lock != NULL) {
        sal_mutex_destroy(TRUNK_DB(unit)->lock);
        TRUNK_DB(unit)->lock = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the trunk public information.
 *
 * This function initializes the trunk public information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_pub_info_init(int unit)
{
    bcmint_trunk_pub_info_t *t_pub = TRUNK_PUB(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(t_pub, 0, sizeof(*t_pub));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_pub_info_get(unit, t_pub));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the trunk public information.
 *
 * This function detaches the trunk public information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_pub_info_detach(int unit)
{
    bcmint_trunk_pub_info_t *t_pub = TRUNK_PUB(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(t_pub, 0, sizeof(*t_pub));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the private informaiton data structure.
 *
 * This function initializes the private informaiton data structure.
 *
 * \param [in] t_priv Trunk Private Information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static void
ltsw_trunk_priv_info_t_init(bcmint_trunk_priv_info_t *t_priv)
{
    if (t_priv) {
        bcmint_trunk_info_t *t_info = t_priv->t_info;
        sal_memset(t_info, 0, sizeof(*t_info));
        t_info->tid = BCM_TRUNK_INVALID;
        t_info->in_use = FALSE;
        t_info->psc = BCM_TRUNK_PSC_PORTFLOW;
        t_info->l2_dest = -1;
        t_info->dlf_index = BCM_TRUNK_UNSPEC_INDEX;
        t_info->member_count = 0;
        t_info->dlb_id = -1;
    }
}

/*!
 * \brief Initialize the trunk private informaiton.
 *
 * This function initializes the trunk private informaiton.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Initialize the trunk database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 */
static int
ltsw_trunk_priv_info_init(int unit)
{
    bcmint_trunk_priv_info_t *t_priv;
    int tid, offset, array_count;
    uint32_t alloc_size = 0;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *ptr = NULL, *ha_ptr = NULL, *ha_ptr_tmp;
    bcmint_trunk_info_t *t_info;
    int warm = bcmi_warmboot_get(unit);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (TRUNK_NUM_GROUPS(unit) > 0) {
        alloc_size = TRUNK_NUM_GROUPS(unit) * sizeof(bcmint_trunk_priv_info_t);
        SHR_ALLOC(ptr, alloc_size, "bcmTrunkPrivInfo");
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        sal_memset(ptr, 0, alloc_size);
        TRUNK_DB(unit)->t_priv = ptr;
        ptr = NULL;

        ha_array_size = TRUNK_NUM_GROUPS(unit);
        ha_instance_size = sizeof(bcmint_trunk_info_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        ha_ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_TRUNK,
                                     BCMINT_TRUNK_PRIV_INFO_T_INFO_SUB_COMP_ID,
                                     "bcmTrunkInfo",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(ha_ptr, 0, ha_alloc_size);
        }
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_TRUNK,
                                     BCMINT_TRUNK_PRIV_INFO_T_INFO_SUB_COMP_ID,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_TRUNK_INFO_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        t_info = ha_ptr;
        for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
            TRUNK_INFO(unit, tid) = &t_info[tid];
        }
        ha_ptr = NULL;

        ha_array_size = 0;
        ha_array_size += TRUNK_NUM_FP_GROUPS(unit) *
                         TRUNK_NUM_FP_PORTS(unit);
        ha_array_size += TRUNK_NUM_FABRIC_GROUPS(unit) *
                         TRUNK_NUM_FABRIC_PORTS(unit);
        ha_array_size += TRUNK_NUM_VIRTUAL_GROUPS(unit) *
                         TRUNK_NUM_VIRTUAL_PORTS(unit);
        ha_instance_size = sizeof(bcmint_trunk_member_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        ha_ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_TRUNK,
                                     BCMINT_TRUNK_PRIV_INFO_MEMBER_SUB_COMP_ID,
                                     "bcmTrunkMember",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(ha_ptr, 0, ha_alloc_size);
        }
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_TRUNK,
                                     BCMINT_TRUNK_PRIV_INFO_MEMBER_SUB_COMP_ID,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_TRUNK_MEMBER_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        ha_ptr_tmp = ha_ptr;
        for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
            t_priv = TRUNK_PRIV(unit, tid);

            if (!warm) {
                ltsw_trunk_priv_info_t_init(t_priv);
            }
            if (TRUNK_FP_TID(unit, tid)) {
                array_count = TRUNK_NUM_FP_PORTS(unit);
            } else if (TRUNK_FABRIC_TID(unit, tid)) {
                array_count = TRUNK_NUM_FABRIC_PORTS(unit);
            } else if (TRUNK_VIRTUAL_TID(unit, tid)) {
                array_count = TRUNK_NUM_VIRTUAL_PORTS(unit);
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            offset = array_count * sizeof(bcmint_trunk_member_t);
            t_priv->member_array = ha_ptr_tmp;
            ha_ptr_tmp += offset;
            if (ha_ptr != NULL) {
                ha_ptr = NULL;
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (TRUNK_DB(unit)->t_priv) {
            t_priv = TRUNK_PRIV(unit, 0);
            if (t_priv->t_info && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, t_priv->t_info);
            }
            if (t_priv->member_array && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, t_priv->member_array);
            }
            SHR_FREE(TRUNK_DB(unit)->t_priv);
        }
        if (ptr != NULL) {
            SHR_FREE(ptr);
        }
        if (ha_ptr != NULL && !warm) {
            (void)bcmi_ltsw_ha_mem_free(unit, ptr);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the trunk module DLB functionality.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
ltsw_trunk_dlb_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Do not initialize the HW during warm boot process. */
    if (warm) {
        SHR_EXIT();
    }

    /* De-Initialize the DLB component. */
    rv = bcmi_ltsw_dlb_type_deinit(unit, bcmi_dlb_type_fp_trunk);
    if (rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmi_ltsw_dlb_type_deinit(unit, bcmi_dlb_type_fabric_trunk);
    if (rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Initialize the DLB component. */
    rv = bcmi_ltsw_dlb_type_init(unit, bcmi_dlb_type_fp_trunk);
    if (rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmi_ltsw_dlb_type_init(unit, bcmi_dlb_type_fabric_trunk);
    if (rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the trunk database.
 *
 * This function initializes the trunk database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Initialize the trunk database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 */
static int
ltsw_trunk_db_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_pub_info_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_priv_info_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the trunk database.
 *
 * This function initialize the trunk database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
ltsw_trunk_db_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_pub_info_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all the trunk related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
ltsw_trunk_clear(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    /* Do not clear the HW during warm boot process. */
    if (warm) {
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the trunk module.
 *
 * Initial trunk database data structure and
 * clear the trunk related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
ltsw_trunk_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_LOCK(unit);

    /* Initialize the DLB component. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_init(unit));

    /* Trunk Database initialization. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_db_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_init(unit));

    /* Clear trunk Logical tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_clear(unit));

exit:
    TRUNK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the trunk module.
 *
 * Detach trunk database data structure.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_db_detach(unit));

exit:
    TRUNK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert type-based trunk identifier to trunk identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] t_type Trunk Type.
 * \param [in] t_id Type-based Trunk Identifier.
 * \param [out] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_type_id2trunk_id(
    int unit,
    bcmint_trunk_type_t t_type,
    bcmint_trunk_id_t t_id,
    bcm_trunk_t *tid)
{
    bcm_trunk_t trunk_id;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);

    switch (t_type) {
        case bcmintTrunkTypeFrontPanel:
            trunk_id = t_id;
            if (!TRUNK_FP_TID(unit, trunk_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmintTrunkTypeFabric:
            trunk_id = t_id + TRUNK_NUM_FP_GROUPS(unit);
            if (!TRUNK_FABRIC_TID(unit, trunk_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmintTrunkTypeVplag:
            trunk_id = t_id - TRUNK_VIRTUAL_BASE_INDEX(unit) + \
                       TRUNK_NUM_FP_GROUPS(unit) + \
                       TRUNK_NUM_FABRIC_GROUPS(unit);
            if (!TRUNK_VIRTUAL_TID(unit, trunk_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tid) {
        *tid = trunk_id;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert trunk identifier to type-based trunk identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] t_type Trunk Type.
 * \param [out] t_id Type-based Trunk Identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_id2type_id(
    int unit,
    bcm_trunk_t tid,
    bcmint_trunk_type_t *t_type,
    bcmint_trunk_id_t *t_id)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    if (TRUNK_FP_TID(unit, tid)) {
        if (t_type) {
            *t_type = bcmintTrunkTypeFrontPanel;
        }
        if (t_id) {
            *t_id = tid;
        }
    } else if (TRUNK_FABRIC_TID(unit, tid)) {
        if (t_type) {
            *t_type = bcmintTrunkTypeFabric;
        }
        if (t_id) {
            *t_id = tid - TRUNK_NUM_FP_GROUPS(unit);
        }
    } else if (TRUNK_VIRTUAL_TID(unit, tid)) {
        if (t_type) {
            *t_type = bcmintTrunkTypeVplag;
        }
        if (t_id) {
            *t_id = tid - \
                    TRUNK_NUM_FP_GROUPS(unit) - \
                    TRUNK_NUM_FABRIC_GROUPS(unit) + \
                    TRUNK_VIRTUAL_BASE_INDEX(unit);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the trunk Id to L2 destination.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid The trunk ID.
 * \param [out] l2_dest L2 destination.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_tid_to_l2_dest(
    int unit,
    bcm_trunk_t tid,
    int *l2_dest)
{
    bcmint_trunk_id_t t_id;
    bcmint_trunk_type_t t_type;
    bcmint_trunk_priv_info_t *t_priv = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_LOCK(unit);

    t_priv = TRUNK_PRIV(unit, tid);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_type_id_to_l2_dest(unit, t_type, t_id, t_priv, l2_dest));

exit:
    TRUNK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the  L2 egress interface Id to trunk Id.
 *
 * \param [in] unit Device unit number.
 * \param [in] l2_eif L2 egress interface.
 * \param [out] tid The trunk ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_l2_eif_to_tid(
    int unit,
    int l2_eif,
    bcm_trunk_t *tid)
{
    int i, max_tid;
    int rv = SHR_E_NOT_FOUND;
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_LOCK(unit);

    max_tid = TRUNK_NUM_FP_GROUPS(unit) - 1;

    for (i = 0; i <= max_tid; i++) {
        t_info = TRUNK_INFO(unit, i);
        if (t_info->tid == BCM_TRUNK_INVALID) {
            continue;
        }
        if (t_info->l2_dest < 0) {
            rv = SHR_E_UNAVAIL;
            break;
        }
        if (t_info->l2_dest == l2_eif) {
            rv = SHR_E_NONE;
            break;
        }
    }
    if (i > max_tid) {
        rv = SHR_E_NOT_FOUND;
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    *tid = i;

exit:
    TRUNK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the trunk Id to VPLAG Id.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] vp_lag_vp The VPLAG vp Id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_tid_to_vp_lag_vp(
    int unit,
    bcm_trunk_t tid,
    int *vp_lag_vp)
{
    bcmint_trunk_info_t *t_info = NULL;
    bcmint_trunk_id_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));
    if (t_type != bcmintTrunkTypeVplag) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    t_info = TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *vp_lag_vp = t_info->l2_dest;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the VPLAG Id to trunk Id.
 *
 * \param [in] unit Unit Number.
 * \param [in] vp_lag_vp The VPLAG vp Id.
 * \param [out] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_vp_lag_vp_to_tid(
    int unit,
    int vp_lag_vp,
    bcm_trunk_t *tid)
{
    bcmint_trunk_info_t *t_info = NULL;
    bcmint_trunk_id_t t_id = vp_lag_vp;
    bcmint_trunk_type_t t_type = bcmintTrunkTypeVplag;
    bcm_trunk_t trunk_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_type_id2trunk_id(unit, t_type, t_id, &trunk_id));

    t_info = TRUNK_INFO(unit, trunk_id);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    if (vp_lag_vp != t_info->l2_dest) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tid) {
        *tid = trunk_id;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare two psc values and return the result.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] psc0 First Port Selection Criteria.
 * \param [in] psc1 Second Port Selection Criteria.
 * \param [in] exact Exact match.
 * \param [out] result TRUE/False: Same or Not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_psc_compare(
    int unit,
    bcm_trunk_t tid,
    int psc0,
    int psc1,
    int exact,
    bool *result)
{
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(result, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_psc_compare(unit, t_type, psc0, psc1, exact, result));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the software data structure for the specified trunk ID.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid The trunk ID.
 * \param [in] t_priv Trunk private information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_create(
    int unit,
    bcm_trunk_t tid,
    bcmint_trunk_priv_info_t *t_priv)
{
    bcmint_trunk_id_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_create(unit, t_id, t_type, t_priv));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the software data structure with the specified trunk ID.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_create_id(
    int unit,
    bcm_trunk_t tid)
{
    bcmint_trunk_priv_info_t *t_priv = NULL;
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_CHECK(unit, tid);
    t_priv = TRUNK_PRIV(unit, tid);
    t_info = TRUNK_INFO(unit, tid);

    TRUNK_LOCK(unit);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_IF_ERR_CONT
            (ltsw_trunk_create(unit, tid, t_priv));
        if (SHR_FUNC_ERR()) {
            TRUNK_UNLOCK(unit);
            SHR_EXIT();
        }
        t_info->tid = tid;
    } else {
        TRUNK_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the software data structure by the specified trunk Identifier.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] t_priv Trunk private information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_destroy(
    int unit,
    bcm_trunk_t tid,
    bcmint_trunk_priv_info_t *t_priv)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_destroy(unit, t_id, t_type, t_priv));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Judge the gport is channelized access port or not.
 *
 * \param [in] unit Device unit number.
 * \param [in] gport Member gport.
 *
 * \retval true Gport is a channelized access port.
 * \retval false Gport is not a channelized access port.
 */
static bool
ltsw_trunk_gport_is_channelized_access_port(
    int unit,
    bcm_gport_t gport)
{
    int rv;
    bool is_channelized_access_port;

    rv = mbcm_ltsw_trunk_gport_is_channelized_access_port(unit, gport,
                                                   &is_channelized_access_port);
    if (SHR_FAILURE(rv)) {
        is_channelized_access_port = false;
    }

    return is_channelized_access_port;
}

/*!
 * \brief Uniform the member gport format.
 *
 * \param [in] unit Device unit number.
 * \param [in] src_gport Source Gport.
 * \param [out] dst_gport Destination Gport.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_uniform_gport(
    int unit,
    bcm_gport_t src_gport,
    bcm_gport_t *dst_gport)
{
    int modid, modport;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(src_gport)) {
        if (BCM_GPORT_IS_MODPORT(src_gport)) {
            *dst_gport = src_gport;
            SHR_EXIT();
        } else if (ltsw_trunk_gport_is_channelized_access_port(unit, src_gport)) {
            *dst_gport = src_gport;
            SHR_EXIT();
        } else {
            int tgid, id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, src_gport,
                                              &modid, &modport, &tgid, &id));
            if ((tgid != -1) || (id != -1)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (dst_gport) {
        BCM_GPORT_MODPORT_SET(*dst_gport, modid, modport);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Uniform the gport format for trunk members.
 *
 * \param [in] unit Device unit number.
 * \param [in] member_count Trunk Member Count.
 * \param [out] member_array Trunk Member Array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_uniform_member(
    int unit,
    int member_count,
    bcm_trunk_member_t *member_array)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < member_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_uniform_gport(unit, member_array[i].gport,
                                      &member_array[i].gport));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore the gport format.
 *
 * \param [in] unit Device unit number.
 * \param [in] src_gport Source Gport.
 * \param [out] dst_gport Destination Gport.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_restore_gport(
    int unit,
    bcm_gport_t src_gport,
    bcm_gport_t *dst_gport)
{
    int modid, port, is_local_modid, is_higig_port;
    bcm_gport_t gport;

    SHR_FUNC_ENTER(unit);

    if (ltsw_trunk_gport_is_channelized_access_port(unit, src_gport)) {
        *dst_gport = src_gport;
        SHR_EXIT();
    }

    modid = BCM_GPORT_MODPORT_MODID_GET(src_gport);
    port = BCM_GPORT_MODPORT_PORT_GET(src_gport);
    gport = src_gport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_modid_is_local(unit, modid, &is_local_modid));
    if (is_local_modid) {
        is_higig_port = bcmi_ltsw_port_is_type(unit,
                                               port,
                                               BCMI_LTSW_PORT_TYPE_HG);
        if (is_higig_port) {
            bcmi_ltsw_gport_info_t gport_info;
            gport_info.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            gport_info.port = port;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_construct(unit, &gport_info, &gport));
        }
    }

    if (dst_gport) {
        *dst_gport = gport;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore the gport format for trunk members.
 *
 * \param [in] unit Device unit number.
 * \param [in] member_count Trunk Member Count.
 * \param [out] member_array Trunk Member Array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_restore_member(
    int unit,
    int member_count,
    bcm_trunk_member_t *member_array)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < member_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_restore_gport(unit, member_array[i].gport,
                                      &member_array[i].gport));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Extract the failover flags from trunk information flags.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] flags Trunk Information Flags.
 * \param [out] fail_flags Failover Flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_failover_flags_extract(
    int unit,
    bcm_trunk_t tid,
    uint32_t flags,
    uint32_t *fail_flags)
{
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fail_flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_failover_flags_extract(unit, flags, t_type,
                                                fail_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk failover configuration.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] gport Generic Port.
 * \param [in] psc Failover Port Selection Criteria.
 * \param [in] ftp_count Failto Port Count.
 * \param [in] ftp Failto Port Array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_failover_set(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t gport,
    int psc,
    int ftp_count,
    bcm_gport_t *ftp)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_failover_psc_validate(unit, t_type, psc));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_failover_set(unit, t_id, t_type, gport,
                                      psc, ftp_count, ftp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk failover configuration.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] gport Generic Port.
 * \param [out] psc Failover Port Selection Criteria.
 * \param [in] ftp_max Max Allowed Number of Failto Port.
 * \param [out] ftp Failto Port Array.
 * \param [out] ftp_count Actual Failto Port Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_failover_get(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t gport,
    int *psc,
    int ftp_max,
    bcm_gport_t *ftp,
    int *ftp_count)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_failover_get(unit, t_id, t_type, gport,
                                      psc, ftp_max, ftp, ftp_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk failover configuration.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] psc Failover Port Selection Criteria.
 * \param [in] fail_flags Failover Flags.
 * \param [in] fail_port Failover Port.
 * \param [in] psc Failover Port Selection Criteria.
 * \param [in] member_count Max Number of Trunk Member.
 * \param [in] member_array Trunk Member Array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_failover_update(
    int unit,
    bcm_trunk_t tid,
    int psc,
    uint32_t fail_flags,
    bcm_gport_t fail_port,
    int member_count,
    bcm_trunk_member_t *member_array)
{
    int ftp_max;
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_failover_psc_validate(unit, t_type, psc));

    if (TRUNK_FABRIC_TID(unit, tid)) {
        ftp_max = TRUNK_NUM_FABRIC_FTPORTS(unit);
    } else if (TRUNK_FP_TID(unit, tid)) {
        ftp_max = TRUNK_NUM_FP_FTPORTS(unit);
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_failover_update(unit, t_id, t_type,
                                         psc, fail_flags, fail_port, ftp_max,
                                         member_count, member_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the trunk input parameters.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Max Number of Trunk Member.
 * \param [in] member_array Trunk Member Array.
 * \param [in] op Trunk Member Operation Code.
 * \param [in] member Trunk Member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_params_validate(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array,
    int op,
    bcm_trunk_member_t *member)
{
    bcmint_trunk_info_t *t_info = TRUNK_INFO(unit, tid);
    bcmint_trunk_type_t t_type;
    bcm_trunk_t t_id;

    SHR_FUNC_ENTER(unit);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    if (trunk_info->psc <= 0) {
        trunk_info->psc = BCM_TRUNK_PSC_PORTFLOW;
    }

    if ((trunk_info->dlf_index != trunk_info->mc_index) ||
        (trunk_info->dlf_index != trunk_info->ipmc_index)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (member_count > 0) {
        if (member_array == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (member_array != NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Check number of ports in trunk group */
    if (TRUNK_FP_TID(unit, tid)) {
        if (member_count > TRUNK_NUM_FP_PORTS(unit)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (TRUNK_FABRIC_TID(unit, tid)) {
        if (member_count > TRUNK_NUM_FABRIC_PORTS(unit)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (TRUNK_VIRTUAL_TID(unit, tid)) {
        if (trunk_info->flags & BCM_TRUNK_FLAG_WEIGHTED) {
            if (member_count > TRUNK_NUM_WEIGHTED_VIRTUAL_PORTS(unit)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            if (member_count > TRUNK_NUM_VIRTUAL_PORTS(unit)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_params_validate(unit, t_id, t_type, trunk_info,
                                         member_count, member_array,
                                         op, member));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare trunk member gport.
 *
 * Internal utility routine for sorting on trunk member gport.
 *
 * \param [in] a First Trunk Member.
 * \param [in] b Second Trunk Member..
 *
 * \retval -1 a is less than b.
 * \retval 0 a equals b.
 * \retval 1 a is more than b.
 */
static int
ltsw_trunk_sort_compare(
    const void *a,
    const void *b)
{
    bcm_trunk_member_t *d1 = (bcm_trunk_member_t *)a;
    bcm_trunk_member_t *d2 = (bcm_trunk_member_t *)b;

    return (d1->gport < d2->gport ? -1 :
            d1->gport > d2->gport ? 1 : 0);
}

/*!
 * \brief Sort members in a trunk group.
 *
 * \param [in] member_count Max Number of Trunk Member.
 * \param [in] member_array Trunk Member Array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_sort(
    int member_count,
    bcm_trunk_member_t **member_array)
{
    if (member_count > 0) {
        if (NULL == *member_array) {
            return SHR_E_PARAM;
        }
    } else {
        return SHR_E_NONE;
    }

    /* Sort members in the accending order */
    sal_qsort(*member_array, member_count,
              sizeof(bcm_trunk_member_t), ltsw_trunk_sort_compare);

    return SHR_E_NONE;
}

/*!
 * \brief Release the trunk update information.
 *
 * \param [in] t_update Trunk Update Information.
 */
static void
ltsw_trunk_update_info_release(bcmint_trunk_update_info_t *t_update)
{
    if (t_update) {
        SHR_FREE(t_update->leaving_members);
        SHR_FREE(t_update->staying_members);
        SHR_FREE(t_update->joining_members);
        sal_memset(t_update, 0, sizeof(*t_update));
    }
}

/*!
 * \brief Generate trunk update information.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Trunk Member Count.
 * \param [in] member_array Trunk Member Array.
 * \param [in] t_priv Trunk private information.
 * \param [in] t_update Trunk update information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_update_info_generate(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array,
    bcmint_trunk_priv_info_t *t_priv,
    bcmint_trunk_update_info_t *t_update)
{
    int i, j, alloc_sz;
    bcm_gport_t gport;
    bcm_gport_t *tmp_old_member = NULL, *tmp_new_member = NULL;
    bcmint_trunk_info_t *t_info = NULL;
    bcm_trunk_member_t *tmp_member_array = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(t_priv, SHR_E_PARAM);
    SHR_NULL_CHECK(t_update, SHR_E_PARAM);

    t_info = t_priv->t_info;

    if (t_info->member_count > 0) {
        alloc_sz = sizeof(bcm_gport_t) * t_info->member_count;

        SHR_ALLOC(t_update->leaving_members, alloc_sz, "bcmTrunkLeavingMbrs");
        SHR_NULL_CHECK(t_update->leaving_members, SHR_E_MEMORY);
        sal_memset(t_update->leaving_members, 0, alloc_sz);
        t_update->num_leaving = 0;

        SHR_ALLOC(t_update->staying_members, alloc_sz, "bcmTrunkStayingMbrs");
        SHR_NULL_CHECK(t_update->staying_members, SHR_E_MEMORY);
        sal_memset(t_update->staying_members, 0, alloc_sz);
        t_update->num_staying = 0;

        SHR_ALLOC(tmp_old_member, alloc_sz, "bcmTrunkTmpOldMbrs");
        SHR_NULL_CHECK(tmp_old_member, SHR_E_MEMORY);
        sal_memset(tmp_old_member, 0, alloc_sz);
    }

    if (member_count > 0) {
        alloc_sz = sizeof(bcm_gport_t) * member_count;

        SHR_ALLOC(t_update->joining_members, alloc_sz, "bcmTrunkJoiningMbrs");
        SHR_NULL_CHECK(t_update->joining_members, SHR_E_MEMORY);
        sal_memset(t_update->joining_members, 0, alloc_sz);
        t_update->num_joining = 0;

        SHR_ALLOC(tmp_new_member, alloc_sz, "bcmTrunkTmpOldMbrs");
        SHR_NULL_CHECK(tmp_new_member, SHR_E_MEMORY);
        sal_memset(tmp_new_member, 0, alloc_sz);
    }

    for (i = 0; i < member_count; i++) {
        gport = member_array[i].gport;

        for (j = 0; j < t_info->member_count; j++) {
            if (gport == t_priv->member_array[j].gport &&
                !tmp_old_member[j]) {
                break;
            }
        }

        if (j == t_info->member_count) {
            t_update->joining_members[t_update->num_joining++] = gport;
        } else {
            t_update->staying_members[t_update->num_staying++] = gport;
            tmp_new_member[i] = 1;
            tmp_old_member[j] = 1;
        }
    }
    if (t_info->flags & BCM_TRUNK_FLAG_WEIGHTED) {
        bcm_trunk_t t_id;
        bcmint_trunk_type_t t_type;

        alloc_sz = sizeof(bcm_trunk_member_t) * t_info->member_count;
        SHR_ALLOC(tmp_member_array, alloc_sz, "bcmTrunkWeightedMbrArray");
        SHR_NULL_CHECK(tmp_member_array, SHR_E_MEMORY);
        sal_memset(tmp_member_array, 0, alloc_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_trunk_get(unit, t_id, t_type, t_priv, NULL,
                                 t_info->member_count, tmp_member_array,
                                 &t_info->member_count));
    }

    for (i = 0; i < t_info->member_count; i++) {
        if (t_info->flags & BCM_TRUNK_FLAG_WEIGHTED) {
            gport = tmp_member_array[i].gport;
        } else {
            gport = t_priv->member_array[i].gport;
        }
        if (!tmp_old_member[i]) {
            t_update->leaving_members[t_update->num_leaving++] = gport;
        }
    }

    if (TRUNK_FABRIC_TID(unit, tid)) {
        t_update->ftp_max = TRUNK_NUM_FABRIC_FTPORTS(unit);
    } else if (TRUNK_FP_TID(unit, tid)) {
        t_update->ftp_max = TRUNK_NUM_FP_FTPORTS(unit);
    }

exit:
    SHR_FREE(tmp_old_member);
    SHR_FREE(tmp_new_member);
    SHR_FREE(tmp_member_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the trunk configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Trunk Member Count.
 * \param [in] member_array Trunk Member Array.
 * \param [in] op Trunk Member Operation Code.
 * \param [in] member Trunk Member.
 * \param [in] t_priv Trunk private information.
 * \param [in] t_update Trunk update information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_update(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array,
    int op,
    bcm_trunk_member_t *member,
    bcmint_trunk_priv_info_t *t_priv,
    bcmint_trunk_update_info_t *t_update)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_update(unit, t_id, t_type, trunk_info,
                                member_count, member_array, op, member,
                                t_priv, t_update));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Sync the trunk configuration to trunk private information database.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Trunk Member Count.
 * \param [in] member_array Trunk Member Array.
 * \param [out] t_priv Trunk private information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_update_sync(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array,
    bcmint_trunk_priv_info_t *t_priv)
{
    bcmint_trunk_info_t *t_info = t_priv->t_info;
    int i, l2_dest, dlb_id;
    uint32_t fail_flags;
    SHR_FUNC_ENTER(unit);

    if (member_count > 0) {
        t_info->in_use = TRUE;
        t_info->flags = trunk_info->flags;
        t_info->psc = trunk_info->psc;
        t_info->ipmc_psc = trunk_info->ipmc_psc;
        t_info->dlf_index = trunk_info->dlf_index;
        t_info->dynamic_size = trunk_info->dynamic_size;
        t_info->dynamic_age = trunk_info->dynamic_age;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_failover_flags_extract(unit, tid, trunk_info->flags,
                                               &fail_flags));
        if (trunk_info->flags & BCM_TRUNK_FLAG_WEIGHTED) {
            for (i = 0; i < t_info->member_count; i++) {
                t_priv->member_array[i].flags = 0;
                t_priv->member_array[i].fail_flags = 0;
                t_priv->member_array[i].gport = 0;
                t_priv->member_array[i].dynamic_scaling_factor = 0;
                t_priv->member_array[i].dynamic_load_weight = 0;
                t_priv->member_array[i].dynamic_queue_size_weight = 0;
            }
            t_info->member_count = member_count;
        } else {
            t_info->member_count = member_count;
            for (i = 0; i < t_info->member_count; i++) {
                t_priv->member_array[i].flags = member_array[i].flags;
                t_priv->member_array[i].fail_flags = fail_flags;
                t_priv->member_array[i].gport = member_array[i].gport;
                t_priv->member_array[i].dynamic_scaling_factor =
                    member_array[i].dynamic_scaling_factor;
                t_priv->member_array[i].dynamic_load_weight =
                    member_array[i].dynamic_load_weight;
                t_priv->member_array[i].dynamic_queue_size_weight =
                    member_array[i].dynamic_queue_size_weight;
            }
        }
    } else {
        l2_dest = t_info->l2_dest;
        dlb_id = t_info->dlb_id;
        ltsw_trunk_priv_info_t_init(t_priv);
        t_info->tid = tid;
        t_info->psc = trunk_info->psc;
        t_info->l2_dest = l2_dest;
        t_info->ipmc_psc = trunk_info->ipmc_psc;
        t_info->dynamic_size = trunk_info->dynamic_size;
        t_info->dynamic_age = trunk_info->dynamic_age;
        t_info->dlb_id = dlb_id;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Modify the trunk configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Trunk Member Count.
 * \param [in] member_array Trunk Member Array.
 * \param [in] op Trunk Member Operation Code.
 * \param [in] member Trunk Member.
 * \param [in] t_priv Trunk private information.
 * \param [in] t_update Trunk update information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_modify(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array,
    int op,
    bcm_trunk_member_t *member,
    bcmint_trunk_priv_info_t *t_priv,
    bcmint_trunk_update_info_t *t_update)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_update(unit, tid, trunk_info,
                           member_count, member_array, op, member,
                           t_priv, t_update));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_update_sync(unit, tid, trunk_info,
                               member_count, member_array,
                               t_priv));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Notify the trunk modificatin to other components.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] t_update Trunk update information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_notify(
    int unit,
    bcm_trunk_t tid,
    bcmint_trunk_update_info_t *t_update)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_notify(unit, t_id, t_type));

    if (t_update->num_leaving) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_ltsw_l3_egress_trunk_member_delete(unit, tid,
                                                     t_update->num_leaving,
                                                     t_update->leaving_members),
             SHR_E_INIT);
    }

    if (t_update->num_joining) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_ltsw_l3_egress_trunk_member_add(unit, tid,
                                                  t_update->num_joining,
                                                  t_update->joining_members),
             SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Trunk Member Count.
 * \param [in] member_array Trunk Member Array.
 * \param [in] op Trunk Member Operation Code.
 * \param [in] member Trunk Member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array,
    int op,
    bcm_trunk_member_t *member)
{
    bcmint_trunk_priv_info_t *t_priv = TRUNK_PRIV(unit, tid);
    bcmint_trunk_update_info_t t_update;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    sal_memset(&t_update, 0, sizeof(t_update));

    /* Validate the trunk information. */
    SHR_IF_ERR_EXIT
        (ltsw_trunk_params_validate(unit, tid, trunk_info,
                                    member_count, member_array, op, member));

    if (TRUNK_FP_TID(unit, tid) || TRUNK_FABRIC_TID(unit, tid)) {
        /* Uniform trunk member gport to modport format. */
        SHR_IF_ERR_EXIT
            (ltsw_trunk_uniform_member(unit, member_count, member_array));

        /* Sort trunk members if necessary. */
        if (trunk_info->flags & BCM_TRUNK_FLAG_MEMBER_SORT) {
            SHR_IF_ERR_EXIT
                (ltsw_trunk_sort(member_count, &member_array));
        }
    }

    /* Modify the trunk. */
    TRUNK_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (ltsw_trunk_update_info_generate(unit, tid, trunk_info,
                                         member_count, member_array,
                                         t_priv, &t_update));

    SHR_IF_ERR_EXIT
        (ltsw_trunk_modify(unit, tid, trunk_info,
                           member_count, member_array, op, member,
                           t_priv, &t_update));

    TRUNK_UNLOCK(unit);
    locked = false;

    /* Trunk update notification. */
    SHR_IF_ERR_EXIT
        (ltsw_trunk_notify(unit, tid, &t_update));

exit:
    if (locked) {
        TRUNK_UNLOCK(unit);
    }
    ltsw_trunk_update_info_release(&t_update);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] trunk_info Trunk Information.
 * \param [in] member_max Max Number of Trunk Members.
 * \param [out] member_array Trunk Member Array.
 * \param [out] member_count Trunk Member Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_max,
    bcm_trunk_member_t *member_array,
    int *member_count)
{
    bcmint_trunk_priv_info_t *t_priv = TRUNK_PRIV(unit, tid);
    bcmint_trunk_info_t *t_info = TRUNK_INFO(unit, tid);
    int i, member_cnt;
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    TRUNK_LOCK(unit);

    if (trunk_info) {
        trunk_info->flags = t_info->flags;
        trunk_info->psc = t_info->psc;
        trunk_info->ipmc_psc = t_info->ipmc_psc;
        trunk_info->dlf_index = t_info->dlf_index;
        trunk_info->mc_index = t_info->dlf_index;
        trunk_info->ipmc_index = t_info->dlf_index;
        trunk_info->dynamic_size = t_info->dynamic_size;
        trunk_info->dynamic_age = t_info->dynamic_age;
    }

    member_cnt = t_info->member_count;

    if (member_max > 0) {
        if (member_max < member_cnt) {
            member_cnt = member_max;
        }
        if (!(t_info->flags & BCM_TRUNK_FLAG_WEIGHTED)) {
            for (i = 0; i < member_cnt; i++) {
                member_array[i].flags = t_priv->member_array[i].flags;
                member_array[i].gport = t_priv->member_array[i].gport;
                member_array[i].dynamic_scaling_factor =
                    t_priv->member_array[i].dynamic_scaling_factor;
                member_array[i].dynamic_load_weight =
                    t_priv->member_array[i].dynamic_load_weight;
                member_array[i].dynamic_queue_size_weight =
                    t_priv->member_array[i].dynamic_queue_size_weight;
            }
            if (TRUNK_FP_TID(unit, tid) || TRUNK_FABRIC_TID(unit, tid)) {
                SHR_IF_ERR_CONT
                    (ltsw_trunk_restore_member(unit, member_cnt, member_array));
                if (SHR_FUNC_ERR()) {
                    TRUNK_UNLOCK(unit);
                    SHR_EXIT();
                }
            }
        }
    }

    SHR_IF_ERR_CONT
        (mbcm_ltsw_trunk_get(unit, t_id, t_type, t_priv, trunk_info,
                             member_max, member_array, &member_cnt));
    if (SHR_FUNC_ERR()) {
        TRUNK_UNLOCK(unit);
        SHR_EXIT();
    }

    TRUNK_UNLOCK(unit);

    if (member_count) {
        *member_count = member_cnt;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable fabric trunk resolution.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] o_type Fabric trunk override Type.
 * \param [in] o_index Fabric trunk override Type based index.
 * \param [in] enable Enable/Disable fabric trunk resolution.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_override_set(
    int unit,
    bcm_trunk_t tid,
    bcmint_trunk_override_type_t o_type,
    int o_index,
    int enable)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    if (t_type != bcmintTrunkTypeFabric) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_override_set(unit, t_id, o_type, o_index, enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get fabric trunk resolution enable/disable status.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] o_type Fabric trunk override Type.
 * \param [in] o_index Fabric trunk override Type based index.
 * \param [out] enable Enable/Disable fabric trunk resolution.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_override_get(
    int unit,
    bcm_trunk_t tid,
    bcmint_trunk_override_type_t o_type,
    int o_index,
    int *enable)
{
    bcm_trunk_t t_id;
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    if (t_type != bcmintTrunkTypeFabric) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_override_get(unit, t_id, o_type, o_index, enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set trunk member port status.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] status Trunk member link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_member_status_set(
    int unit,
    bcm_port_t port,
    int status)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_member_status_set(unit, port, status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get trunk member port status.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] status Trunk member link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_member_status_get(
    int unit,
    bcm_port_t port,
    int *status)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_member_status_get(unit, port, status));

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Find the trunk identifier with a member gport.
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Trunk Member Gport.
 * \param [out] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_find(
    int unit,
    bcm_gport_t gport,
    bcm_trunk_t *tid)
{
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_MODPORT(gport)) {
        bcmint_trunk_id_t t_id;
        bcmint_trunk_type_t t_type;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_trunk_find(unit, gport, &t_id, &t_type));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_type_id2trunk_id(unit, t_type, t_id, tid));
    } else {
        int vp_lag_vp;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_trunk_find_virtual(unit, gport, &vp_lag_vp));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_vp_lag_vp_to_tid(unit, vp_lag_vp, tid));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk dlb type.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] dlb_type Trunk DLB type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_type_get(
    int unit,
    bcm_trunk_t tid,
    bcmi_ltsw_dlb_type_t *dlb_type)
{
    bcmint_trunk_type_t t_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, NULL));

    switch (t_type) {
        case bcmintTrunkTypeFrontPanel:
            *dlb_type = bcmi_dlb_type_fp_trunk;
            break;
        case bcmintTrunkTypeFabric:
            *dlb_type = bcmi_dlb_type_fabric_trunk;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk dlb identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] dlb_id Trunk DLB Identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_id_get(
    int unit,
    bcm_trunk_t tid,
    int *dlb_id)
{
    bcmint_trunk_id_t t_id;
    bcmint_trunk_type_t t_type;
    bcmint_trunk_priv_info_t *t_priv = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_LOCK(unit);

    t_priv = TRUNK_PRIV(unit, tid);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, &t_type, &t_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_find_dlb_id(unit, t_type, t_id, t_priv, dlb_id));

exit:
    TRUNK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] dlb_mon_cfg Trunk DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_mon_config_set(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_mon_cfg_t *dlb_mon_cfg)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_monitor_t dlb_monitor;
    int dlb_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_type_get(unit, tid, &dlb_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_id_get(unit, tid, &dlb_id));

    dlb_monitor.enable = dlb_mon_cfg->enable;
    dlb_monitor.sample_rate = dlb_mon_cfg->sample_rate;
    switch (dlb_mon_cfg->action) {
        case BCM_TRUNK_DLB_MON_ACTION_NONE:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_ACTION_NONE;
            break;
        case BCM_TRUNK_DLB_MON_ACTION_TRACE:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_TRACE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_monitor_set(unit, dlb_type, dlb_id, &dlb_monitor));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] dlb_mon_cfg Trunk DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_mon_config_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_mon_cfg_t *dlb_mon_cfg)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_monitor_t dlb_monitor;
    int dlb_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_type_get(unit, tid, &dlb_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_id_get(unit, tid, &dlb_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_monitor_get(unit, dlb_type, dlb_id, &dlb_monitor));

    dlb_mon_cfg->enable = dlb_monitor.enable;
    dlb_mon_cfg->sample_rate = dlb_monitor.sample_rate;
    switch (dlb_monitor.action) {
        case BCMI_LTSW_DLB_MON_ACTION_NONE:
            dlb_mon_cfg->action = BCM_TRUNK_DLB_MON_ACTION_NONE;
            break;
        case BCMI_LTSW_DLB_MON_TRACE:
            dlb_mon_cfg->action = BCM_TRUNK_DLB_MON_ACTION_TRACE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk dlb statistics.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] type Trunk DLB statistics type.
 * \param [in] value Trunk DLB statistic value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_stat_set(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_stat_t type,
    uint64_t value)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_stat_t dlb_stat;
    int dlb_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_type_get(unit, tid, &dlb_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_id_get(unit, tid, &dlb_id));

    dlb_stat.fail_count = (uint64_t)0xffffffff;
    dlb_stat.port_reassignment_count = (uint64_t)0xffffffff;

    switch (type) {
        case bcmTrunkDlbStatFailPackets:
            dlb_stat.fail_count = value;
            break;
        case bcmTrunkDlbStatPortReassignmentCount:
            dlb_stat.port_reassignment_count = value;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_stat_set(unit, dlb_type, dlb_id, &dlb_stat));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk dlb statistics.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] type Trunk DLB statistics type.
 * \param [out] value Trunk DLB statistic value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_stat_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_stat_t type,
    uint64_t *value)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_stat_t dlb_stat;
    int dlb_id;

    SHR_FUNC_ENTER(unit);

    sal_memset(&dlb_stat, 0, sizeof(dlb_stat));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_type_get(unit, tid, &dlb_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_id_get(unit, tid, &dlb_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_stat_get(unit, dlb_type, dlb_id, &dlb_stat));

    switch (type) {
        case bcmTrunkDlbStatFailPackets:
            *value = dlb_stat.fail_count;
            break;
        case bcmTrunkDlbStatPortReassignmentCount:
            *value = dlb_stat.port_reassignment_count;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk dlb statistics in sync mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] type Trunk DLB statistics type.
 * \param [out] value Trunk DLB statistic value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_trunk_dlb_stat_sync_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_stat_t type,
    uint64_t *value)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_stat_t dlb_stat;
    int dlb_id;

    SHR_FUNC_ENTER(unit);

    sal_memset(&dlb_stat, 0, sizeof(dlb_stat));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_type_get(unit, tid, &dlb_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_id_get(unit, tid, &dlb_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_stat_sync_get(unit, dlb_type, dlb_id, &dlb_stat));

    switch (type) {
        case bcmTrunkDlbStatFailPackets:
            *value = dlb_stat.fail_count;
            break;
        case bcmTrunkDlbStatPortReassignmentCount:
            *value = dlb_stat.port_reassignment_count;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_ltsw_trunk_id_validate(
    int unit,
    bcm_trunk_t tid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_id2type_id(unit, tid, NULL, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_tid_to_l2_if(
    int unit,
    bcm_trunk_t tid,
    int *l2_if)
{
    int l2_dest;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    SHR_NULL_CHECK(l2_if, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_tid_to_l2_dest(unit, tid, &l2_dest));

    *l2_if = l2_dest;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_l2_if_to_tid(
    int unit,
    int l2_if,
    bcm_trunk_t *tid)
{
    int l2_eif = l2_if;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    SHR_NULL_CHECK(tid, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_l2_eif_to_tid(unit, l2_eif, tid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_tid_to_vp_lag_vp(
    int unit,
    bcm_trunk_t tid,
    int *vp_lag_vp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_tid_to_vp_lag_vp(unit, tid, vp_lag_vp));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_vp_lag_member_update(
    int unit,
    int vp_lag_vp,
    bcm_gport_t gport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_vp_lag_member_update(unit, vp_lag_vp, gport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_vp_lag_vp_to_tid(
    int unit,
    int vp_lag_vp,
    bcm_trunk_t *tid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_vp_lag_vp_to_tid(unit, vp_lag_vp, tid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_vp_lag_vp_learn_get(
    int unit,
    bcm_trunk_t tid,
    uint32_t *flags)
{
    int vp_lag_vp;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    TRUNK_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_trunk_tid_to_vp_lag_vp(unit, tid, &vp_lag_vp));
    if (SHR_FUNC_ERR()) {
        TRUNK_UNLOCK(unit);
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT
        (mbcm_ltsw_trunk_vp_lag_vp_learn_get(unit, vp_lag_vp, flags));
    if (SHR_FUNC_ERR()) {
        TRUNK_UNLOCK(unit);
        SHR_EXIT();
    }

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_vp_lag_vp_learn_set(
    int unit,
    bcm_trunk_t tid,
    uint32_t flags)
{
    int vp_lag_vp;

    SHR_FUNC_ENTER(unit);

    TRUNK_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_trunk_tid_to_vp_lag_vp(unit, tid, &vp_lag_vp));
    if (SHR_FUNC_ERR()) {
        TRUNK_UNLOCK(unit);
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT
        (mbcm_ltsw_trunk_vp_lag_vp_learn_set(unit, vp_lag_vp, flags));
    if (SHR_FUNC_ERR()) {
        TRUNK_UNLOCK(unit);
        SHR_EXIT();
    }

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_local_members_get(
    int unit,
    bcm_trunk_t tid,
    int local_member_max,
    bcm_port_t *local_member_array,
    int *local_member_count)
{
    int member_count;
    bcm_trunk_member_t *member_array = NULL;
    int i;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    int modid_local;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (local_member_max < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((local_member_max == 0) && (NULL != local_member_array)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((local_member_max > 0) && (NULL == local_member_array)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(local_member_count, SHR_E_PARAM);

    *local_member_count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_trunk_id_validate(unit, tid));

    /* First, get the number of trunk members. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_trunk_get(unit, tid, NULL, 0, NULL, &member_count));

    if (0 == member_count) {
        SHR_EXIT();
    }

    SHR_ALLOC(member_array, sizeof(bcm_trunk_member_t) * member_count,
              "bcmTrunkMemberArray");
    SHR_NULL_CHECK(member_array, SHR_E_MEMORY);
    sal_memset(member_array, 0, sizeof(bcm_trunk_member_t) * member_count);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_trunk_get(unit, tid, NULL, member_count,
                            member_array, &member_count));

    for (i = 0; i < member_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, member_array[i].gport,
                                          &mod_out, &port_out,
                                          &tgid_out, &id_out));
        rv = bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local);
        if (SHR_FAILURE(rv) || !modid_local) {
            continue;
        }
        if (NULL != local_member_array) {
            local_member_array[*local_member_count] = port_out;
        }
        (*local_member_count)++;
        if (*local_member_count == local_member_max) {
            break;
        }
    }

exit:
    if (member_array) {
        SHR_FREE(member_array);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_find_dlb_id(
    int unit,
    bcm_trunk_t tid,
    int *dlb_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_id_get(unit, tid, dlb_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_sc_hash_control_set(
    int unit,
    uint32_t arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_sc_hash_control_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_sc_hash_control_get(
    int unit,
    uint32_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_sc_hash_control_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_sc_lb_control_unicast_set(
    int unit,
    uint32_t arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_sc_lb_control_unicast_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_sc_lb_control_unicast_get(
    int unit,
    uint32_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_sc_lb_control_unicast_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_sc_lb_control_nonunicast_set(
    int unit,
    uint32_t arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_sc_lb_control_nonunicast_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_trunk_sc_lb_control_nonunicast_get(
    int unit,
    uint32_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_sc_lb_control_nonunicast_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

void
bcmi_ltsw_trunk_sw_dump(int unit)
{
    bcmint_trunk_pub_info_t *t_pub;
    bcmint_trunk_priv_info_t *t_priv;
    bcmint_trunk_info_t *t_info;
    bcmint_trunk_member_t *member_array;
    int i, j;

    t_pub = TRUNK_PUB(unit);
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information TRUNK - Unit %d\n"),
             unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Front panel trunk groups : %d\n"),
             t_pub->ngroups_fp));
    LOG_CLI((BSL_META_U(unit,
                        "  Front panel trunk max ports : %d\n"),
             t_pub->nports_fp));
    LOG_CLI((BSL_META_U(unit,
                        "  Front panel trunk max failto ports : %d\n"),
             t_pub->nftports_fp));
    LOG_CLI((BSL_META_U(unit,
                        "  Fabric trunk groups : %d\n"),
             t_pub->ngroups_fabric));
    LOG_CLI((BSL_META_U(unit,
                        "  Fabric trunk max ports : %d\n"),
             t_pub->nports_fabric));
    LOG_CLI((BSL_META_U(unit,
                        "  Fabric trunk max failto ports : %d\n"),
             t_pub->nftports_fabric));
    LOG_CLI((BSL_META_U(unit,
                        "  Virtual groups : %d\n"),
             t_pub->ngroups_vplag));
    LOG_CLI((BSL_META_U(unit,
                        "  Virtual max ports : %d\n"),
             t_pub->nvps_vplag));

    LOG_CLI((BSL_META_U(unit,
                        "  Virtual max weighted ports : %d\n"),
             t_pub->nvps_weighted_vplag));

    for (i = 0; i < TRUNK_NUM_GROUPS(unit); i++) {
        t_priv = TRUNK_PRIV(unit, i);
        t_info = TRUNK_INFO(unit, i);

        if (t_info->tid == BCM_TRUNK_INVALID) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "  Trunk %d\n"), i));
        LOG_CLI((BSL_META_U(unit,
                            "      ID              : %d\n"),
                 t_info->tid));
        LOG_CLI((BSL_META_U(unit,
                            "      in use          : %d\n"),
                 t_info->in_use));
        LOG_CLI((BSL_META_U(unit,
                            "      t_info          :\n")));
        LOG_CLI((BSL_META_U(unit,
                            "            flags     : 0x%08x\n"),
                 t_info->flags));
        LOG_CLI((BSL_META_U(unit,
                            "            psc       : %d\n"),
                 t_info->psc));
        LOG_CLI((BSL_META_U(unit,
                            "            l2_dest   : %d\n"),
                 t_info->l2_dest));
        LOG_CLI((BSL_META_U(unit,
                            "            ipmc_psc  : %d\n"),
                 t_info->ipmc_psc));
        LOG_CLI((BSL_META_U(unit,
                            "            dlf_index : %d\n"),
                 t_info->dlf_index));
        LOG_CLI((BSL_META_U(unit,
                            "            dynamic_size : %d\n"),
                 t_info->dynamic_size));
        LOG_CLI((BSL_META_U(unit,
                            "            dynamic_age  : %d\n"),
                 t_info->dynamic_age));

        LOG_CLI((BSL_META_U(unit,
                            "            dlb_id    : %d\n"),
                 t_info->dlb_id));

        LOG_CLI((BSL_META_U(unit,
                            "      member_count    : %d\n"),
                 t_info->member_count));
        if (!(t_info->flags & BCM_TRUNK_FLAG_WEIGHTED)) {
            member_array = t_priv->member_array;
            for (j = 0; j < t_info->member_count; j++) {
                LOG_CLI((BSL_META_U(unit,
                                    "          member      : %d\n"), j));
                LOG_CLI((BSL_META_U(unit,
                                    "              flags   : 0x%08x\n"),
                         member_array[j].flags));
                LOG_CLI((BSL_META_U(unit,
                                    "              fail_flags   : 0x%08x\n"),
                         member_array[j].fail_flags));
                LOG_CLI((BSL_META_U(unit,
                                    "              gport   : 0x%08x\n"),
                         member_array[j].gport));
                LOG_CLI((BSL_META_U(unit,
                                    "              dynamic_scaling_factor : %d\n"),
                         member_array[j].dynamic_scaling_factor));
                LOG_CLI((BSL_META_U(unit,
                                    "              dynamic_load_weight    : %d\n"),
                         member_array[j].dynamic_load_weight));
                LOG_CLI((BSL_META_U(unit,
                                    "              dynamic_queue_size_weight : %d\n"),
                         member_array[j].dynamic_queue_size_weight));
                LOG_CLI((BSL_META_U(unit,
                                    "              failover_psc : %d\n"),
                         member_array[j].failover_psc));
            }
        }
    }
}

/*******************************************************************************
 * Public External HSDK functions
 */

/*!
 * \brief Initialize the trunk module.
 *
 * Initial trunk database data structure and
 * clear the trunk related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_lock_create(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the trunk module.
 *
 * Detach trunk database data structure.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcm_ltsw_trunk_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_lock_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a trunk.
 *
 * Create a trunk w/o a trunk identifier.
 * if without, return a trunk identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags Trunk Create Flags.
 * \param [out] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_create(
    int unit,
    uint32_t flags,
    bcm_trunk_t *tid)
{
    bcm_trunk_t i, max_tid, min_tid;
    bcmint_trunk_priv_info_t *t_priv = NULL;
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    SHR_NULL_CHECK(tid, SHR_E_PARAM);

    if (flags & BCM_TRUNK_FLAG_VP) {
        if (TRUNK_NUM_VIRTUAL_GROUPS(unit) == 0) {
            SHR_ERR_EXIT(SHR_E_FULL);
        } else {
            min_tid = TRUNK_NUM_FP_GROUPS(unit) + TRUNK_NUM_FABRIC_GROUPS(unit);
            max_tid = min_tid + TRUNK_NUM_VIRTUAL_GROUPS(unit) - 1;
        }
    } else {
        if (TRUNK_NUM_FP_GROUPS(unit) == 0) {
            min_tid = 0;
            max_tid = min_tid + TRUNK_NUM_FABRIC_GROUPS(unit) - 1;
        } else {
            min_tid = 0;
            max_tid = min_tid + TRUNK_NUM_FP_GROUPS(unit) - 1;
        }
    }

    if (flags & BCM_TRUNK_FLAG_WITH_ID) {
        if (flags & BCM_TRUNK_FLAG_VP) {
            if (*tid < min_tid || *tid > max_tid) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_create_id(unit, *tid));

        SHR_EXIT();
    }

    TRUNK_LOCK(unit);

    for (i = min_tid; i <= max_tid; i++) {
        t_priv = TRUNK_PRIV(unit, i);
        t_info = TRUNK_INFO(unit, i);
        if (t_info->tid == BCM_TRUNK_INVALID) {
            t_info->tid = i;

            *tid = i;

            SHR_IF_ERR_CONT
                (ltsw_trunk_create(unit, i, t_priv));
            if (SHR_FUNC_ERR()) {
                TRUNK_UNLOCK(unit);
                SHR_EXIT();
            }
            break;
        }
    }

    if (i > max_tid) {
        TRUNK_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_FULL);
    }

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk selection criteria.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] psc Port Selection Criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_psc_get(
    int unit,
    bcm_trunk_t tid,
    int *psc)
{
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    SHR_NULL_CHECK(psc, SHR_E_PARAM);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        *psc = 0;
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        *psc = t_info->psc;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk selection criteria.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] psc Port Selection Criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_psc_set(
    int unit,
    bcm_trunk_t tid,
    int psc)
{
    bcmint_trunk_info_t *t_info = NULL;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_array = NULL;
    int member_count, alloc_sz;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (psc <= 0) {
        psc = BCM_TRUNK_PSC_PORTFLOW;
    }

    if (t_info->psc == psc) {
        SHR_EXIT();
    }

    if (!t_info->in_use) {
        t_info->psc = psc;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_get(unit, tid, &trunk_info, 0, NULL, &member_count));

    /* If the new psc is one of the dynamic load balancing modes, but the old
     * psc is not, then return SHR_E_PARAM, since dynamic_size and dynamic_age
     * parameters are not available when calling bcm_trunk_set.
     */
    if ((psc == BCM_TRUNK_PSC_DYNAMIC) ||
        (psc == BCM_TRUNK_PSC_DYNAMIC_ASSIGNED) ||
        (psc == BCM_TRUNK_PSC_DYNAMIC_OPTIMAL)) {
        if ((trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC) &&
            (trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC_ASSIGNED) &&
            (trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC_OPTIMAL)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* If the new psc is the resilient hashing mode, but the old
     * psc is not, then return SHR_E_PARAM, since the dynamic_size
     * parameter is not available when calling bcm_trunk_set.
     */
    if (psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) {
        if (trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC_RESILIENT) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (member_count > 0) {
        alloc_sz = sizeof(bcm_trunk_member_t) * member_count;
        SHR_ALLOC(member_array, alloc_sz, "bcmTrunkMemberArray");
        SHR_NULL_CHECK(member_array, SHR_E_MEMORY);

        SHR_IF_ERR_CONT
            (ltsw_trunk_get(unit, tid, &trunk_info,
                            member_count, member_array, &member_count));
        if (SHR_FUNC_ERR()) {
            SHR_FREE(member_array);
            SHR_EXIT();
        }
    }

    trunk_info.psc = psc;
    /* Don't change failover configuration */
    trunk_info.flags = 0;

    SHR_IF_ERR_CONT
        (ltsw_trunk_set(unit, tid, &trunk_info,
                        member_count, member_array,
                        BCMINT_TRUNK_MEMBER_OP_SET, NULL));

    if (member_count > 0) {
        SHR_FREE(member_array);
    }

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk HW capibility information.
 *
 * \param [in] unit Unit Number.
 * \param [out] chip_info Chip-level Trunk Capibility Information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_chip_info_get(
    int unit,
    bcm_trunk_chip_info_t *chip_info)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);

    chip_info->trunk_group_count = TRUNK_NUM_FP_GROUPS(unit);

    if (TRUNK_NUM_FP_GROUPS(unit) > 0) {
        chip_info->trunk_id_min = 0;
        chip_info->trunk_id_max = TRUNK_NUM_FP_GROUPS(unit) - 1;
    } else {
        chip_info->trunk_id_min = BCM_TRUNK_INVALID;
        chip_info->trunk_id_max = BCM_TRUNK_INVALID;
    }
    chip_info->trunk_ports_max = TRUNK_NUM_FP_PORTS(unit);

    if (TRUNK_NUM_FABRIC_GROUPS(unit) > 0) {
        chip_info->trunk_fabric_id_min = TRUNK_NUM_FP_GROUPS(unit);
        chip_info->trunk_fabric_id_max = TRUNK_NUM_FP_GROUPS(unit) + \
                                         TRUNK_NUM_FABRIC_GROUPS(unit) - 1;
    } else {
        chip_info->trunk_fabric_id_min = BCM_TRUNK_INVALID;
        chip_info->trunk_fabric_id_max = BCM_TRUNK_INVALID;
    }
    chip_info->trunk_fabric_ports_max = TRUNK_PUB(unit)->nports_fabric;

    if (TRUNK_NUM_VIRTUAL_GROUPS(unit) > 0) {
        chip_info->vp_id_min = TRUNK_NUM_FP_GROUPS(unit) + \
                               TRUNK_NUM_FABRIC_GROUPS(unit);
        chip_info->vp_id_max = TRUNK_NUM_FP_GROUPS(unit) + \
                               TRUNK_NUM_FABRIC_GROUPS(unit) + \
                               TRUNK_NUM_VIRTUAL_GROUPS(unit) - 1;
    } else {
        chip_info->vp_id_min = BCM_TRUNK_INVALID;
        chip_info->vp_id_max = BCM_TRUNK_INVALID;
    }
    chip_info->vp_ports_max = TRUNK_PUB(unit)->nvps_vplag;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] trunk_info Trunk Information.
 * \param [in] member_max Max Number of Trunk Members.
 * \param [out] member_array Trunk Member Array.
 * \param [out] member_count Trunk Member Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_max,
    bcm_trunk_member_t *member_array,
    int *member_count)
{
    bcm_trunk_info_t local_trunk_info;
    int local_member_count;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    if ((member_max > 0) && (NULL == member_array)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    bcm_trunk_info_t_init(&local_trunk_info);
    local_member_count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_get(unit, tid, &local_trunk_info,
                        member_max, member_array, &local_member_count));

    if (trunk_info) {
        *trunk_info = local_trunk_info;
    }

    if (member_count) {
        *member_count = local_member_count;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] trunk_info Trunk Information.
 * \param [in] member_count Trunk Member Count.
 * \param [in] member_array Trunk Member Array.
 * \param [in] op Trunk Member Operation Code.
 * \param [in] member Trunk Member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array)
{
    bcm_trunk_info_t local_trunk_info;
    bcm_trunk_member_t *local_member_array = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(trunk_info, SHR_E_PARAM);

    if (TRUNK_VIRTUAL_TID(unit, tid) &&
        trunk_info->flags & BCM_TRUNK_FLAG_WEIGHTED) {
        SHR_IF_ERR_CONT
            (ltsw_trunk_set(unit, tid, trunk_info,
                            member_count, member_array,
                            BCMINT_TRUNK_MEMBER_OP_SET, NULL));
    } else {
        sal_memcpy(&local_trunk_info, trunk_info, sizeof(bcm_trunk_info_t));

        if (member_count > 0) {
            SHR_ALLOC(local_member_array,
                      sizeof(bcm_trunk_member_t) * member_count,
                      "bcmTrunkMemeberArray");
            SHR_NULL_CHECK(local_member_array, SHR_E_MEMORY);
            sal_memcpy(local_member_array, member_array,
                       sizeof(bcm_trunk_member_t) * member_count);
        }

        SHR_IF_ERR_CONT
            (ltsw_trunk_set(unit, tid, &local_trunk_info,
                            member_count, local_member_array,
                            BCMINT_TRUNK_MEMBER_OP_SET, NULL));

        if (member_count > 0) {
            SHR_FREE(local_member_array);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the trunk by a specified trunk Identifier.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_destroy(
    int unit,
    bcm_trunk_t tid)
{
    bcmint_trunk_priv_info_t *t_priv = NULL;
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_priv = TRUNK_PRIV(unit, tid);
    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    TRUNK_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_trunk_destroy(unit, tid, t_priv));
    if (SHR_FUNC_ERR()) {
        TRUNK_UNLOCK(unit);
        SHR_EXIT();
    }

    ltsw_trunk_priv_info_t_init(t_priv);

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief GET VPLAG port Id for VPLAG trunk.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] vplag_port_id VPLAG Port Id
 *
 * \retval BCM_E_NONE Success.
 * \retval BCM_E_PARAM Incorrect LAG Id.
 * \retval BCM_E_NOT_FOUND LAG Id not used.
 */
int
bcm_ltsw_trunk_vplag_get(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t *vplag_port_id)
{
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    SHR_NULL_CHECK(vplag_port_id, SHR_E_PARAM);

    if (!TRUNK_VIRTUAL_TID(unit, tid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    t_info = TRUNK_INFO(unit, tid);

    TRUNK_LOCK(unit);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        TRUNK_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    BCM_GPORT_VPLAG_PORT_ID_SET(*vplag_port_id, t_info->l2_dest);

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk override over non-UniCast.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] enable TRUE if enabled, FALSE if disabled.
 *
 * \retval SHR_E_UNAVAIL Not Support.
 */
int
bcm_ltsw_trunk_override_non_ucast_set(
    int unit,
    bcm_trunk_t tid,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_override_set(unit, tid,
                                 bcmintTrunkOverrideTypeNonUcast,
                                 -1, enable));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk override over non-UniCast.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] enable TRUE if enabled, FALSE if disabled.
 *
 * \retval SHR_E_UNAVAIL Not Support.
 */
int
bcm_ltsw_trunk_override_non_ucast_get(
    int unit,
    bcm_trunk_t tid,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_override_get(unit, tid,
                                 bcmintTrunkOverrideTypeNonUcast,
                                 -1, enable));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find trunk identifier that contains the given system port
 *
 * \param [in] unit Unit Number.
 * \param [in] modid Module Identifier.
 * \param [in] gport Generic Port Number.
 * \param [out] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE Find sucessfully.
 * \retval SHR_E_NOT_FOUND Not found.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_find(
    int unit,
    bcm_module_t modid,
    bcm_gport_t gport,
    bcm_trunk_t *tid)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    SHR_NULL_CHECK(tid, SHR_E_PARAM);

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_TRUNK(gport)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
        }
        rv = ltsw_trunk_uniform_gport(unit, gport, &gport);
        if (rv != SHR_E_PARAM) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    } else {
        if (modid == BCM_MODID_INVALID) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BADID);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, gport, &gport));
        BCM_GPORT_MODPORT_SET(gport, modid, gport);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_find(unit, gport, tid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign the failto port list for a specific trunk port.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] fail_port Failover Port Number.
 * \param [in] psc Port Selection Criteria.
 * \param [in] flags Trunk Failover Flags.
 * \param [in] count Failto Member Count.
 * \param [in] fail_to_array Failto Member Array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_failover_set(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t fail_port,
    int psc,
    uint32_t flags,
    int count,
    bcm_gport_t *fail_to_array)
{
    bcmint_trunk_priv_info_t *t_priv = NULL;
    bcmint_trunk_info_t *t_info = NULL;
    bcm_module_t modid;
    bcm_port_t port;
    int i, j, alloc_sz;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_priv = TRUNK_PRIV(unit, tid);
    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (BCM_GPORT_IS_SET(fail_port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, fail_port, &port));
    } else {
        if (TRUNK_FABRIC_TID(unit, tid)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_stk_my_modid_get(unit, &modid));
            port = fail_port;

            BCM_GPORT_MODPORT_SET(fail_port, modid, port);
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_uniform_gport(unit, fail_port, &fail_port));

    if (psc <= 0) {
        psc = BCM_TRUNK_PSC_PORTFLOW;
    }
    if ((flags == 0) &&
        ((count < 0) || ((count > 0) && (fail_to_array == NULL)))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Where is our port in the trunk list? */
    for (i = 0; i < t_info->member_count; i++) {
        if (t_priv->member_array[i].flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            continue;
        }

        if (t_priv->member_array[i].gport == fail_port) {
            break;
        }
    }

    /* Port is not in the trunk! */
    if ((0 != count) && (i == t_info->member_count)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    TRUNK_LOCK(unit);

    if (flags) {
        bcm_trunk_member_t *member_array = NULL;

        if (t_info->member_count > 0) {
            alloc_sz = sizeof(bcm_trunk_member_t) * t_info->member_count;
            SHR_ALLOC(member_array, alloc_sz, "bcmTrunkMember");
            if (member_array == NULL) {
                TRUNK_UNLOCK(unit);
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(member_array, 0, alloc_sz);

            for (j = 0; j < t_info->member_count; j++) {
                member_array[j].flags = t_priv->member_array[j].flags;
                member_array[j].gport = t_priv->member_array[j].gport;
                member_array[j].dynamic_scaling_factor =
                    t_priv->member_array[j].dynamic_scaling_factor;
                member_array[j].dynamic_load_weight =
                    t_priv->member_array[j].dynamic_load_weight;
                member_array[j].dynamic_queue_size_weight =
                    t_priv->member_array[j].dynamic_queue_size_weight;
            }
        }

        SHR_IF_ERR_CONT
            (ltsw_trunk_failover_update(unit, tid, psc, flags, fail_port,
                                        t_info->member_count, member_array));

        if (t_info->member_count > 0) {
            SHR_FREE(member_array);
        }

        if (SHR_FUNC_ERR()) {
            TRUNK_UNLOCK(unit);
            SHR_EXIT();
        }
    } else {
        bcm_gport_t *ftp = NULL;

        if (count > 0) {
            if ((TRUNK_FP_TID(unit, tid) &&
                 (count > TRUNK_PUB(unit)->nports_fp)) ||
                (TRUNK_FABRIC_TID(unit, tid) &&
                 (count > TRUNK_PUB(unit)->nports_fabric)) ) {
                TRUNK_UNLOCK(unit);
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            alloc_sz = sizeof(bcm_gport_t) * count;
            SHR_ALLOC(ftp, alloc_sz, "bcmTrunkGport");
            if (ftp == NULL) {
                TRUNK_UNLOCK(unit);
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(ftp, 0, alloc_sz);

            for (j = 0; j < count; j++) {
                SHR_IF_ERR_CONT
                    (ltsw_trunk_uniform_gport(unit, fail_to_array[j], &ftp[j]));
                if (SHR_FUNC_ERR()) {
                    SHR_FREE(ftp);
                    TRUNK_UNLOCK(unit);
                    SHR_EXIT();
                }
            }
        }

        SHR_IF_ERR_CONT
            (ltsw_trunk_failover_set(unit, tid, fail_port, psc, count, ftp));

        if (count > 0) {
            SHR_FREE(ftp);
        }
    }

    t_priv->member_array[i].failover_psc = count ? psc : 0;
    t_priv->member_array[i].fail_flags = flags;

    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the failto port list for a specific trunk port.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] fail_port Failover Port Number.
 * \param [out] psc Port Selection Criteria.
 * \param [out] flags Trunk Failover Flags.
 * \param [in] array_size Failto Gport Array Size.
 * \param [out] fail_to_array Failto Gport Array.
 * \param [out] array_count Failto Gport Array Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_failover_get(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t failport,
    int *psc,
    uint32_t *flags,
    int array_size,
    bcm_gport_t *fail_to_array,
    int *array_count)
{
    bcmint_trunk_priv_info_t *t_priv = NULL;
    bcmint_trunk_info_t *t_info = NULL;
    bcm_module_t modid;
    bcm_port_t port;
    int local_psc;
    int i;
    bool result;
    uint32_t fail_flags;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_priv = TRUNK_PRIV(unit, tid);
    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (BCM_GPORT_IS_SET(failport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, failport, &port));
    } else {
        if (TRUNK_FABRIC_TID(unit, tid)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_stk_my_modid_get(unit, &modid));
            port = failport;

            BCM_GPORT_MODPORT_SET(failport, modid, port);
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_uniform_gport(unit, failport, &failport));

    if ((psc == NULL) || (flags == NULL) || (array_size < 0) ||
        (fail_to_array == NULL) || (array_count == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Where is our port in the trunk list? */
    for (i = 0; i < t_info->member_count; i++) {
        if (t_priv->member_array[i].flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            continue;
        }

        if (t_priv->member_array[i].gport == failport) {
            break;
        }
    }

    /* Port is not in the trunk! */
    if (i == t_info->member_count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    fail_flags = t_priv->member_array[i].fail_flags;

    TRUNK_LOCK(unit);

    if (fail_flags) {
        /* Just return the flags value */
        *flags = fail_flags;
        *array_count = 0;

        SHR_IF_ERR_CONT
            (ltsw_trunk_failover_get(unit, tid, failport, &local_psc,
                                     0, NULL, NULL));
    } else {
        *flags = 0;

        SHR_IF_ERR_CONT
            (ltsw_trunk_failover_get(unit, tid, failport, &local_psc,
                                     array_size, fail_to_array, array_count));
    }

    TRUNK_UNLOCK(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    result = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_psc_compare(unit, tid, t_info->psc, local_psc, false,
                                &result));

    if (result) {
        *psc = t_info->psc;
    } else {
        *psc = t_priv->member_array[i].failover_psc;
    }

    if (*array_count > 0 && array_size > 0) {
        for (i = 0; i < *array_count; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_trunk_restore_gport(unit, fail_to_array[i],
                                          &fail_to_array[i]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to a trunk group.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] member Trunk Member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_member_add(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_member_t *member)
{
    int alloc_sz;
    bcm_trunk_info_t trunk_info;
    int member_count;
    bcm_trunk_member_t *member_array = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(member, SHR_E_PARAM);

    /* Get number of trunk group's existing member ports */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_get(unit, tid, NULL, 0, NULL, &member_count));

    if (TRUNK_FP_TID(unit, tid) || TRUNK_FABRIC_TID(unit, tid)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_uniform_member(unit, 1, member));
    }
    /* Allocate member array to accomodate existing members and
     * the new member to be added.
     */
    alloc_sz = sizeof(bcm_trunk_member_t) * (member_count + 1);
    SHR_ALLOC(member_array, alloc_sz, "bcmTrunkMemberArray");
    if (member_array == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(member_array, 0, alloc_sz);

    /* Retrieve existing member ports */
    SHR_IF_ERR_CONT
        (ltsw_trunk_get(unit, tid, &trunk_info,
                        member_count + 1, member_array, &member_count));
    if (SHR_FUNC_ERR()) {
        SHR_FREE(member_array);
        SHR_EXIT();
    }

    /* Add given member to group */
    sal_memcpy(&member_array[member_count], member, sizeof(bcm_trunk_member_t));

    SHR_IF_ERR_CONT
        (ltsw_trunk_set(unit, tid, &trunk_info,
                        member_count + 1, member_array,
                        BCMINT_TRUNK_MEMBER_OP_ADD, member));

    SHR_FREE(member_array);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a member from a trunk group.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] member Trunk Member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_member_delete(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_member_t *member)
{
    int i, k;
    int alloc_sz;
    bcm_trunk_info_t trunk_info;
    int member_count;
    bcm_trunk_member_t *member_array = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(member, SHR_E_PARAM);

    /* Get number of trunk group's existing member ports */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_get(unit, tid, NULL, 0, NULL, &member_count));

    if (TRUNK_FP_TID(unit, tid) || TRUNK_FABRIC_TID(unit, tid)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_trunk_uniform_member(unit, 1, member));
    }

    if (member_count == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Allocate member array to get existing members */
    alloc_sz = sizeof(bcm_trunk_member_t) * member_count;
    SHR_ALLOC(member_array, alloc_sz, "bcmTrunkMemberArray");
    if (member_array == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(member_array, 0, alloc_sz);

    /* Retrieve existing member ports */
    SHR_IF_ERR_CONT
        (ltsw_trunk_get(unit, tid, &trunk_info,
                        member_count + 1, member_array, &member_count));
    if (SHR_FUNC_ERR()) {
        SHR_FREE(member_array);
        SHR_EXIT();
    }

    if (TRUNK_FP_TID(unit, tid) || TRUNK_FABRIC_TID(unit, tid)) {
        SHR_IF_ERR_CONT
            (ltsw_trunk_uniform_member(unit, member_count, member_array));
        if (SHR_FUNC_ERR()) {
            SHR_FREE(member_array);
            SHR_EXIT();
        }
    }

    /* Find the index of the given member in the array */
    for (i = 0; i < member_count; i++) {
        if (member_array[i].gport == member->gport) {
            break;
        }
    }

    if (i == member_count) {
        SHR_FREE(member_array);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Delete given member from array, and shift the remaining
     * elements of the array.
     */
    for (k = i; k < member_count - 1; k++) {
        sal_memcpy(&member_array[k], &member_array[k+1],
                   sizeof(bcm_trunk_member_t));
    }

    member_count--;

    if (0 == member_count) {
        SHR_IF_ERR_CONT
            (ltsw_trunk_set(unit, tid, &trunk_info, 0, NULL,
                            BCMINT_TRUNK_MEMBER_OP_DELETE, member));
    } else {
        SHR_IF_ERR_CONT
            (ltsw_trunk_set(unit, tid, &trunk_info, member_count, member_array,
                            BCMINT_TRUNK_MEMBER_OP_DELETE, member));
    }

    SHR_FREE(member_array);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove all members of a trunk group.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_member_delete_all(
    int unit,
    bcm_trunk_t tid)
{
    bcm_trunk_info_t trunk_info;
    int member_count;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_trunk_get(unit, tid, &trunk_info, 0, NULL, &member_count));

    if (member_count > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_trunk_set(unit, tid, &trunk_info, 0, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set trunk member status.
 *
 * \param [in] unit Unit Number.
 * \param [in] member Trunk member GPORT ID.
 * \param [in] status Trunk member link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_member_status_set(
    int unit,
    bcm_gport_t member,
    int status)
{
    int port;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, member, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_member_status_set(unit, port, status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get trunk member status.
 *
 * \param [in] unit Unit Number.
 * \param [in] member Trunk member GPORT ID.
 * \param [out] status Trunk member link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_member_status_get(
    int unit,
    bcm_gport_t member,
    int *status)
{
    int port;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    SHR_NULL_CHECK(status, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, member, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_member_status_get(unit, port, status));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_trunk_dlb_mon_config_set(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_mon_cfg_t *dlb_mon_cfg)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(dlb_mon_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_mon_config_set(unit, tid, dlb_mon_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_trunk_dlb_mon_config_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_mon_cfg_t *dlb_mon_cfg)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(dlb_mon_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_mon_config_get(unit, tid, dlb_mon_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the DLB statistics based on the trunk Id.
 *
 * \param [in] unit Unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] type Trunk DLB statistics type.
 * \param [in] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_trunk_dlb_stat_set(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_stat_t type,
    uint64_t value)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_stat_set(unit, tid, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the DLB statistics from the trunk Id.
 *
 * \param [in] unit Unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] type Trunk DLB statistics type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_trunk_dlb_stat_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_stat_t type,
    uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_stat_get(unit, tid, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the DLB statistics from the trunk Id with sync mode.
 *
 * \param [in] unit Unit number.
 * \param [in] tid Trunk Identifier.
 * \param [in] type Trunk DLB statistics type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_trunk_dlb_stat_sync_get(
    int unit,
    bcm_trunk_t tid,
    bcm_trunk_dlb_stat_t type,
    uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_trunk_dlb_stat_sync_get(unit, tid, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Expand a port bitmap to include all associated trunk member ports.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] pbmp Pointer to returned expanded port bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_bitmap_expand(
    int unit,
    bcm_pbmp_t *pbmp)
{
    bcmint_trunk_pub_info_t *t_pub = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);

    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    t_pub = TRUNK_PUB(unit);

    TRUNK_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_bitmap_expand(unit, t_pub, pbmp));

exit:
    TRUNK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add the specified trunk group to an existing MAC multicast entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] t_id Trunk Identifier.
 * \param [in] vid VLAN ID.
 * \param [in] mac MAC address.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INIT Trunk module has not been initialized.
 * \retval SHR_E_BADID Invalid or unsupported trunk ID.
 * \retval SHR_E_NOT_FOUND Specified trunk ID is not currently defined.
 */
int
bcm_ltsw_trunk_mcast_join(
    int unit,
    bcm_trunk_t t_id,
    bcm_vlan_t vid,
    bcm_mac_t mac)
{
    bcmint_trunk_priv_info_t *t_priv;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, t_id);

    t_priv = TRUNK_PRIV(unit, t_id);
    if (t_priv->t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND);
    }

    if (!t_priv->t_info->in_use) {
        SHR_EXIT();
    }

    TRUNK_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_trunk_mcast_join(unit,
                                    t_id,
                                    vid,
                                    mac));
    TRUNK_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach stat counter ID to a given trunk ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_stat_attach(int unit,
                           bcm_trunk_t tid,
                           uint32_t stat_counter_id)
{
    bcmint_trunk_info_t *t_info = NULL;
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    pre_ci.direction = ci.direction;

    TRUNK_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_info_get(unit, tid, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_attach(unit, tid, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        TRUNK_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach stat counter ID to a given trunk ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_stat_detach(int unit, bcm_trunk_t tid, uint32_t stat_counter_id)
{
    bcmint_trunk_info_t *t_info = NULL;
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t ctr_id;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    TRUNK_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_info_get(unit, tid, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_detach(unit, tid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        TRUNK_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter ID if associated with given trunk ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_stat_id_get(int unit, bcm_trunk_t tid, uint32_t *stat_counter_id)
{
    bcmint_trunk_info_t *t_info = NULL;
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    SHR_NULL_CHECK(stat_counter_id, SHR_E_PARAM);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    TRUNK_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_info_get(unit, tid, &ci));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    if (locked) {
        TRUNK_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set object for a given trunk ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [in] value Object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_flexctr_object_set(int unit, bcm_trunk_t tid, uint16_t value)
{
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_object_set(unit, tid, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get object for a given trunk ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid Trunk Identifier.
 * \param [out] value Object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_trunk_flexctr_object_get(int unit, bcm_trunk_t tid, uint16_t *value)
{
    bcmint_trunk_info_t *t_info = NULL;

    SHR_FUNC_ENTER(unit);

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    t_info = TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_trunk_flexctr_object_get(unit, tid, value));

exit:
    SHR_FUNC_EXIT();
}
