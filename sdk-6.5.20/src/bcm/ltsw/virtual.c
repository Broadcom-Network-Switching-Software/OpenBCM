/*! \file virtual.c
 *
 * Virtual management.
 * This file contains the management for VPN & VP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/mbcm/virtual.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/trunk.h>

#include <bsl/bsl.h>
#include <sal/sal_mutex.h>
#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/*!
 * \brief Vp entry list.
 */
typedef struct ltsw_virtual_vp_info_list_s {

    /* Next index. */
    uint16_t *next;
} ltsw_virtual_vp_info_list_t;

/*!
 * \brief Vp user info.
 */
typedef struct ltsw_virtual_vp_user_info_s {
    /* Flags. */
    uint16_t *flags;

    /* Pipe bitmap. */
    uint16_t *pbmp;

    /* heads. */
    uint16_t *head;
} ltsw_virtual_vp_user_info_t;

/*!
 * \brief Vp entry management.
 */
typedef struct ltsw_virtual_vp_manage_s {

    /* Valid user bitmap*/
    uint16_t user_bmp;

    /*! Protection mutex. */
    sal_mutex_t vp_user_mutex;

    /* Vp entry list.*/
    ltsw_virtual_vp_info_list_t vp_list[BCMI_LTSW_VIRTUAL_VP_USER_CNT];

    /* Vp info of certain user.*/
    ltsw_virtual_vp_user_info_t vp_user_info[BCMI_LTSW_VIRTUAL_VP_USER_CNT];
}ltsw_virtual_vp_manage_t;

/*!
 * \brief Defines Software bookkeeping for virtual resource related information.
 *
 * This data structure defines software bookkeeping for virtual resource
 * related information.
 */
typedef struct ltsw_virtual_info_s {
    /*! Data structure initialized or not. */
    uint8_t initialized;

    /*! Protection mutex. */
    sal_mutex_t virtual_mutex;

    /*! VFI table min index */
    uint32_t vfi_min;

    /*! VFI table max index */
    uint32_t vfi_max;

    /*! VP table min index */
    uint32_t vp_min;

    /*! VP table max index */
    uint32_t vp_max;

    /*! L2_IIF table min index */
    uint32_t l2_iif_min;

    /*! L2_IIF table max index */
    uint32_t l2_iif_max;

    /*! Global VFI bitmap */
    SHR_BITDCL *vfi_bitmap;

    /*! MPLS VFI bitmap */
    SHR_BITDCL *mpls_vfi_bitmap;

    /*! Flow VFI bitmap */
    SHR_BITDCL *flow_vfi_bitmap;

    /*! VP info */
    bcmi_ltsw_virtual_vp_info_t *vp_info;

    /*! Virtual vp info management */
    ltsw_virtual_vp_manage_t vp_manage;

    /*! egress object mapping of VP. */
    bcm_if_t       *egress_if_map;

    /*! Egress adaptation key of VP. */
    uint32_t       *egress_adapt_key;
} ltsw_virtual_info_t;

/*!
 * \Virtual book keeping information.
 */
static ltsw_virtual_info_t   ltsw_virtual_info[BCM_MAX_NUM_UNITS];

#define VIRTUAL_INFO(_unit_)      (&ltsw_virtual_info[_unit_])

#define VIRTUAL_LOCK(_unit_) \
    sal_mutex_take(VIRTUAL_INFO(_unit_)->virtual_mutex, SAL_MUTEX_FOREVER)
#define VIRTUAL_UNLOCK(_unit_) \
    sal_mutex_give(VIRTUAL_INFO(_unit_)->virtual_mutex)

#define VIRTUAL_VP_USER_LOCK(_unit_) \
    do { \
        if (VIRTUAL_INFO(_unit_)->vp_manage.vp_user_mutex) { \
            sal_mutex_take(VIRTUAL_INFO(_unit_)->vp_manage.vp_user_mutex, \
                           SAL_MUTEX_FOREVER); \
        } \
    } while(0)
#define VIRTUAL_VP_USER_UNLOCK(_unit_) \
    do { \
        if (VIRTUAL_INFO(_unit_)->vp_manage.vp_user_mutex) { \
            sal_mutex_give(VIRTUAL_INFO(_unit_)->vp_manage.vp_user_mutex); \
        } \
    } while(0)

#define VIRTUAL_INITIALIZED(_unit_) VIRTUAL_INFO(_unit_)->initialized


#define VIRTUAL_INIT_CHECK(_unit_)                      \
    do {                                                \
        ltsw_virtual_info_t *vi = VIRTUAL_INFO(_unit_); \
        if (vi->initialized == FALSE) {                 \
            SHR_ERR_EXIT(SHR_E_INIT);            \
        }                                               \
    } while(0)

/*!
 *\ VFI table usage bitmap operations
 */
#define VIRTUAL_VFI_USED_GET(_unit_, _vfi_) \
                ((VIRTUAL_INFO(_unit_)->vfi_bitmap) ? \
                SHR_BITGET(VIRTUAL_INFO(_unit_)->vfi_bitmap, (_vfi_)) : 0)
#define VIRTUAL_VFI_USED_SET(_unit_, _vfi_) \
                SHR_BITSET(VIRTUAL_INFO((_unit_))->vfi_bitmap, (_vfi_))
#define VIRTUAL_VFI_USED_CLR(_unit_, _vfi_) \
                SHR_BITCLR(VIRTUAL_INFO((_unit_))->vfi_bitmap, (_vfi_))

#define VIRTUAL_MPLS_VFI_USED_GET(_unit_, _vfi_) \
                ((VIRTUAL_INFO(_unit_)->mpls_vfi_bitmap) ? \
                SHR_BITGET(VIRTUAL_INFO(_unit_)->mpls_vfi_bitmap, (_vfi_)) : 0)
#define VIRTUAL_MPLS_VFI_USED_SET(_unit_, _vfi_) \
                SHR_BITSET(VIRTUAL_INFO((_unit_))->mpls_vfi_bitmap, (_vfi_))
#define VIRTUAL_MPLS_VFI_USED_CLR(_unit_, _vfi_) \
                SHR_BITCLR(VIRTUAL_INFO((_unit_))->mpls_vfi_bitmap, (_vfi_))

#define VIRTUAL_FLOW_VFI_USED_GET(_unit_, _vfi_) \
                ((VIRTUAL_INFO(_unit_)->flow_vfi_bitmap) ? \
                SHR_BITGET(VIRTUAL_INFO(_unit_)->flow_vfi_bitmap, (_vfi_)) : 0)
#define VIRTUAL_FLOW_VFI_USED_SET(_unit_, _vfi_) \
                SHR_BITSET(VIRTUAL_INFO((_unit_))->flow_vfi_bitmap, (_vfi_))
#define VIRTUAL_FLOW_VFI_USED_CLR(_unit_, _vfi_) \
                SHR_BITCLR(VIRTUAL_INFO((_unit_))->flow_vfi_bitmap, (_vfi_))

#define VIRTUAL_VP_LIST(_unit_, _user_id_) \
    (ltsw_virtual_info[_unit_].vp_manage.vp_list[_user_id_].next)
#define VIRTUAL_VP_HEAD(_unit_, _user_id_) \
    (ltsw_virtual_info[_unit_].vp_manage.vp_user_info[_user_id_].head)
#define VIRTUAL_VP_USER_FLAG(_unit_, _user_id_) \
    (ltsw_virtual_info[_unit_].vp_manage.vp_user_info[_user_id_].flags)
#define VIRTUAL_VP_USER_PBMP(_unit_, _user_id_) \
    (ltsw_virtual_info[_unit_].vp_manage.vp_user_info[_user_id_].pbmp)


/* HA subcomponent id for VIRTUAL module. */
typedef enum virtual_ha_subcomponent_id_e {
    VIRTUAL_SUB_COMP_ID_VFI_BITMAP      = 0,  /* Vfi bitmap. */
    VIRTUAL_SUB_COMP_ID_MPLS_VFI_BITMAP = 1,  /* MPLS vfi bitmap. */
    VIRTUAL_SUB_COMP_ID_FLOW_VFI_BITMAP = 2,  /* FLOW vfi bitmap. */
    VIRTUAL_SUB_COMP_ID_VP_INFO         = 3,  /* VP info. */
    VIRTUAL_SUB_COMP_ID_EGRESS_IF_MAP   = 4,  /* egress object mapping of VP. */
    VIRTUAL_SUB_COMP_ID_USER_L2_IF      = 5,  /* Vp user l2_if info. */
    VIRTUAL_SUB_COMP_ID_EGR_ADAPT_KEY   = 6,  /* Egress adaptation key of VP. */
    VIRTUAL_SUB_COMP_ID_COUNT           = 7
} virtual_ha_subcomponent_id_t;

/*!
 * \brief Information for key sync.
 */
typedef struct egress_adapt_key_sync_s {
    /*! Gport number. */
    int vp;

    /*! Lookup key. */
    int key_type;

    /*! Sync status. */
    int synced;
} egress_adapt_key_sync_t;

/******************************************************************************
* Private functions
 */

/*!
 * \brief Destroy vp management info.
 *
 * \param [in] unit      Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_manage_destroy(int unit)
{
    ltsw_virtual_vp_manage_t *vp_mng = &(VIRTUAL_INFO(unit)->vp_manage);
    int i;

    /* Destroy protection mutex. */
    if (vp_mng->vp_user_mutex) {
        sal_mutex_destroy(vp_mng->vp_user_mutex);
        vp_mng->vp_user_mutex = NULL;
    }

    for (i = 0; i < BCMI_LTSW_VIRTUAL_VP_USER_CNT; i++) {
        if (vp_mng->user_bmp & (1 << i)) {
            SHR_FREE(vp_mng->vp_list[i].next);
            SHR_FREE(vp_mng->vp_user_info[i].head);
            vp_mng->user_bmp &= ~(1 << i);
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Free all allocated resource.
 *
 * This function is used to release the virtual information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
virtual_free_resource(int unit)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    (void)virtual_vp_manage_destroy(unit);

    /* Destroy protection mutex. */
    if (vi->virtual_mutex) {
        sal_mutex_destroy(vi->virtual_mutex);
        vi->virtual_mutex = NULL;
    }

    sal_memset(vi, 0, sizeof(ltsw_virtual_info_t));

    return;
}

/*!
 * \brief Initialize VFI hardware tables.
 *
 * This function is used to init related hardware tables.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vfi                  VFI index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
virtual_vfi_init(int unit, int vfi)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_vfi_init(unit, vfi));

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup VFI hardware tables.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vfi_cleanup(int unit, int vfi)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_vfi_cleanup(unit, vfi));

exit:
   SHR_FUNC_EXIT();
}


/*!
 * \brief Add default STG.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vfi_stg_add(int unit, int vfi)
{
    bcm_stg_t stg;
    bcm_vpn_t vpn;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_stg_default_get(unit, &stg));

    BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI, vfi);
    SHR_IF_ERR_EXIT
        (bcm_ltsw_stg_vlan_add(unit, stg, vpn));

exit:
   SHR_FUNC_EXIT();
}


/*!
 * \brief Remove VPN from STG.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vfi_stg_remove(int unit, int vfi)
{
    bcm_vpn_t vpn;

    SHR_FUNC_ENTER(unit);


    BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI, vfi);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_stg_vlan_destroy(unit, vpn));

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Notify member check information on adding/deleting VPN
 *
 * \param [in] unit        Unit number.
 * \param [in] vfi         VFI index.
 * \param [in] add         Add or delete.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
virtual_vfi_check_info_notify(int unit, int vfi, int add)
{
    bcm_vpn_t vpn;

    SHR_FUNC_ENTER(unit);

    BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI, vfi);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_vlan_check_info_notify(unit, vpn, add));

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get pipe bitmap per dvp port group.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   pg        Egr_dvp port group.
 * \param [out]  pbmp      Pipe bitmap.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_pg_pipe_get(int unit, int pg, uint32_t *pbmp)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint16_t *vp_list = VIRTUAL_VP_LIST(unit, BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG);
    uint16_t *vp_head = VIRTUAL_VP_HEAD(unit, BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG);
    uint16_t *flg_if =
        VIRTUAL_VP_USER_FLAG(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    uint16_t *bmp =
        VIRTUAL_VP_USER_PBMP(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    int i, oif;
    uint32_t pp_pbmp = 0, tmp_bmp = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pbmp,SHR_E_PARAM);
    *pbmp = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pp_pipe_bmp(unit, &pp_pbmp));

    for(i = vp_head[pg]; i != 0; i = vp_list[i]) {
        oif = vi->vp_info[i].l2_oif;

        /* When dvp group contains network_vp or null oif, return all pipe.*/
        if (vi->vp_info[i].flags & BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT || !oif) {
            *pbmp = pp_pbmp;
            SHR_EXIT();
        }

        if (flg_if[oif] & BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK) {
            tmp_bmp = bmp[oif];
        } else if (flg_if[oif] & BCMI_LTSW_VIRTUAL_VP_USER_FLAG_REMOTE) {
            *pbmp = pp_pbmp;
            SHR_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_if_to_pipes(unit, oif, &tmp_bmp));
        }
        *pbmp |= tmp_bmp;
        /* Return full pipe in advance.*/
        if (*pbmp == pp_pbmp) {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a node in vp info.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   user_id   User ID.
 * \param [in]   vp        Vp.
 * \param [in]   value     User value.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_info_del(int unit, bcmi_ltsw_virtual_vp_user_t user_id,
                    int vp, uint16_t value)
{
    uint16_t *vp_list = VIRTUAL_VP_LIST(unit, user_id);
    uint16_t *vp_head = VIRTUAL_VP_HEAD(unit, user_id);
    int i, found = 0;
    uint16_t pre_vp = 0;
    SHR_FUNC_ENTER(unit);

    /* Never delete value 0.*/
    if(value == 0) {
        SHR_EXIT();
    }

    for(i = vp_head[value]; i != 0; i = vp_list[i]) {
        if (i == vp) {
            vp_list[pre_vp]= vp_list[i];
            if (i == vp_head[value]) {
                vp_head[value] = vp_list[i];
            }
            vp_list[i] = 0;
            found = 1;
            break;
        }
        pre_vp = i;
    }

    if (!found) {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a node in vp info.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   user_id   User ID.
 * \param [in]   vp        Vp.
 * \param [in]   value     User value.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_info_add(int unit, bcmi_ltsw_virtual_vp_user_t user_id,
                    int vp, uint16_t value)
{
    uint16_t *vp_list = VIRTUAL_VP_LIST(unit, user_id);
    uint16_t *vp_head = VIRTUAL_VP_HEAD(unit, user_id);
    uint16_t pre_value;
    SHR_FUNC_ENTER(unit);

    /* Never add value 0.*/
    if(value == 0) {
        SHR_EXIT();
    }

    pre_value = vp_head[value];
    vp_head[value] = vp;
    vp_list[vp] = pre_value;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init vp management info for certain user.
 *
 * \param [in] unit      Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_manage_init(int unit)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    ltsw_virtual_vp_manage_t *vp_mng = &(vi->vp_manage);
    bcmi_ltsw_virtual_vp_info_t *vp_info = vi->vp_info;
    uint32_t i, alloc_size, req_size;
    uint16_t *list_ptr = NULL, *head_ptr = NULL;
    uint16_t *ptr = NULL, *ptr_2 = NULL;
    int warm = bcmi_warmboot_get(unit);
    int user_size[BCMI_LTSW_VIRTUAL_VP_USER_CNT] ={(vi->l2_iif_max + 1),
                                                   (vi->vp_max + 1)};
    int wb = bcmi_warmboot_get(unit), vp_num, support = 0, user_bmp = 0;
    uint16_t *next_0, *head_0, *next_1, *head_1;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_xlate_egr_pipe_check(unit, &support));
    if (support) {
        user_bmp |= (1 << BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF) |
                    (1 << BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG);
    } else {
        SHR_EXIT();
    }

    if (vp_mng->vp_user_mutex == NULL) {
        vp_mng->vp_user_mutex = sal_mutex_create("virtual vp user mutex");
        if (vp_mng->vp_user_mutex == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    vp_num = vi->vp_max + 1;
    for (i = 0; i < BCMI_LTSW_VIRTUAL_VP_USER_CNT; i++) {
        if (!(user_bmp & (1 << i))) {
            continue;
        }

        /* Alloc vp_list. */
        alloc_size = vp_num * sizeof(uint16_t);
        SHR_ALLOC(list_ptr, alloc_size, "bcmVirtualVpList");
        SHR_NULL_CHECK(list_ptr, SHR_E_MEMORY);
        sal_memset(list_ptr, 0, alloc_size);
        vp_mng->vp_list[i].next = list_ptr;
        list_ptr = NULL;

        /* Alloc vp_head. */
        alloc_size = user_size[i] * sizeof(uint16_t);
        SHR_ALLOC(head_ptr, alloc_size, "bcmVirtualVpHead");
        SHR_NULL_CHECK(head_ptr, SHR_E_MEMORY);
        sal_memset(head_ptr, 0, alloc_size);
        vp_mng->vp_user_info[i].head = head_ptr;
        head_ptr = NULL;

        if (i == BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF) {
            /* Alloc ha for l2 oif flag and pbmp.*/
            req_size = user_size[i] * sizeof(uint16_t);
            req_size += user_size[i] * sizeof(uint16_t);
            alloc_size = req_size;
            ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_VIRTUAL,
                                         VIRTUAL_SUB_COMP_ID_USER_L2_IF,
                                         "bcmVirtualL2ifInfo",
                                         &alloc_size);
            SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
            SHR_IF_ERR_VERBOSE_EXIT
                ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
            if (!warm) {
                sal_memset(ptr, 0, alloc_size);
            }
            ptr_2 = ptr;
            vp_mng->vp_user_info[i].flags = ptr;
            ptr += user_size[i];
            vp_mng->vp_user_info[i].pbmp = ptr;
        }

        /* Update user bitmap. */
        vp_mng->user_bmp |= (1 << i);
    }

    /* Recover list in WB.
     * Currently the only user of vp_info is egr_xlate(l2_oif and dvp_pg).
     * Flags will be recovered in vlan module.
     */
    if (wb && vp_mng->user_bmp) {
        next_0 = vp_mng->vp_list[0].next;
        head_0 = vp_mng->vp_user_info[0].head;
        next_1 = vp_mng->vp_list[1].next;
        head_1 = vp_mng->vp_user_info[1].head;
        for (i = 0; i < vp_num; i++) {
            if (vp_info[i].flags & BCMI_LTSW_VIRTUAL_VP_VALID) {
                if (vp_info[i].l2_oif) {
                    next_0[i] = head_0[vp_info[i].l2_oif];
                    head_0[vp_info[i].l2_oif] = i;
                }
                if (vp_info[i].dvp_pg) {
                    next_1[i] = head_1[vp_info[i].dvp_pg];
                    head_1[vp_info[i].dvp_pg] = i;
                }
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)virtual_vp_manage_destroy(unit);
        if (!wb) {
            (void) bcmi_ltsw_ha_mem_free(unit, ptr_2);
        }
        ptr_2 = NULL;
        ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VP info based on certain VP id.
 *
 * \param [in]   unit        Unit number.
 * \param [in]   vp          VP ID.
 * \param [in]   oif_change  If oif_change.
 * \param [in]   old_l2_oif  old_l2_oif.
 * \param [in]   new_l2_oif  new_l2_oif.
 * \param [in]   pg_change   If pg_change.
 * \param [in]   old_dvp_pg  old_dvp_pg.
 * \param [in]   new_dvp_pg  new_dvp_pg.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_info_update(int unit, int vp,
                       int oif_change, int old_l2_oif, int new_l2_oif,
                       int pg_change, int old_dvp_pg, int new_dvp_pg)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    SHR_FUNC_ENTER(unit);

    /* Update l2_oif sw list and info. */
    if (oif_change) {
        SHR_IF_ERR_VERBOSE_EXIT
            (virtual_vp_info_del(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF,
                                 vp, old_l2_oif));
        SHR_IF_ERR_VERBOSE_EXIT
            (virtual_vp_info_add(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF,
                                 vp, new_l2_oif));
        vi->vp_info[vp].l2_oif = new_l2_oif;
    }

    /* Update dvp port group sw list and info. */
    if (pg_change) {
        SHR_IF_ERR_VERBOSE_EXIT
            (virtual_vp_info_del(unit, BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG,
                                 vp, old_dvp_pg));
        SHR_IF_ERR_VERBOSE_EXIT
            (virtual_vp_info_add(unit, BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG,
                                 vp, new_dvp_pg));
        vi->vp_info[vp].dvp_pg = new_dvp_pg;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback to sync gress adaptation key.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   intf       Egress object ID.
 * \param [in]   info       Egress forwarding destination info.
 * \param [in]   user_data  User data.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
virtual_vp_egress_adapt_key_sync_traverse_cb(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t *info,
    void *user_data)
{
    egress_adapt_key_sync_t *user_data_ptr  = (egress_adapt_key_sync_t *)user_data;
    bcm_if_t encap_id = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * VP is passed as info->port for overlay nexthop.
     * VP is passed as info->encap_id for underlay nexthop.
     */
    encap_id = BCM_GPORT_FLOW_PORT_ID_GET(user_data_ptr->vp);
    if (((user_data_ptr->vp == info->port) || (encap_id == info->encap_id)) && info->intf) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_intf_adapt_lookup_key_set(unit, info->intf,
                                                    user_data_ptr->key_type));
         user_data_ptr->synced = 1;
    }

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize virtual resource management.
 *
 * \param [in]  unit               Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_init(int unit)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    int vfi_min = 0, vfi_max = 0, vp_min = 0, vp_max = 0;
    uint32_t num_vfi, num_vp, alloc_size, req_size;
    int l2_iif_min = 0, l2_iif_max = 0;
    int i;
    void *ptr = NULL;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if (vi->initialized) {
        virtual_free_resource(unit);
    }

    SHR_IF_ERR_EXIT
        (mbcm_virtual_index_get(unit, &vfi_min, &vfi_max, &vp_min,&vp_max,
                                &l2_iif_min, &l2_iif_max));


    if (vi->virtual_mutex == NULL) {
        vi->virtual_mutex = sal_mutex_create("virtual mutex");
        if (vi->virtual_mutex == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    vi->vfi_min = vfi_min;
    vi->vfi_max = vfi_max;
    vi->vp_min = vp_min;
    vi->vp_max = vp_max;
    vi->l2_iif_min = l2_iif_min;
    vi->l2_iif_max = l2_iif_max;

    num_vfi = vi->vfi_max + 1;
    num_vp = vi->vp_max + 1;
    req_size = SHR_BITALLOCSIZE(num_vfi);
    /* Allocate HA memory for VFI bitmap. */
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_VIRTUAL,
                                 VIRTUAL_SUB_COMP_ID_VFI_BITMAP,
                                 "bcmVirtualVfiBmp",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);
    }
    vi->vfi_bitmap = ptr;

    /* Allocate HA memory for MPLS VFI bitmap. */
    alloc_size = req_size;
    req_size = SHR_BITALLOCSIZE(num_vfi);
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_VIRTUAL,
                                 VIRTUAL_SUB_COMP_ID_MPLS_VFI_BITMAP,
                                 "bcmVirtualMplsVfiBmp",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);
    }
    vi->mpls_vfi_bitmap = ptr;

    /* Allocate HA memory for FLOW VFI bitmap. */
    alloc_size = req_size;
    req_size = SHR_BITALLOCSIZE(num_vfi);
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_VIRTUAL,
                                 VIRTUAL_SUB_COMP_ID_FLOW_VFI_BITMAP,
                                 "bcmVirtualFlowVfiBmp",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);
    }
    vi->flow_vfi_bitmap = ptr;

    /* Allocate HA memory for VP info. */
    req_size = num_vp * sizeof(bcmi_ltsw_virtual_vp_info_t);
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_VIRTUAL,
                                 VIRTUAL_SUB_COMP_ID_VP_INFO,
                                 "bcmVirtualVpInfo",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);

        /* Update SW mapping info for reserved VFI. */
        for (i = 0; i < vi->vfi_min; i++) {
            VIRTUAL_VFI_USED_SET(unit, i);
            VIRTUAL_FLOW_VFI_USED_SET(unit, i);
        }
    }
    vi->vp_info = ptr;

    /* Allocate HA memory for VP info. */
    req_size = num_vp * sizeof(bcm_if_t);
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_VIRTUAL,
                                 VIRTUAL_SUB_COMP_ID_EGRESS_IF_MAP,
                                 "bcmVirtualEgrIfMap",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);
    }
    vi->egress_if_map = ptr;

    SHR_IF_ERR_VERBOSE_EXIT
        (virtual_vp_manage_init(unit));

    /* Allocate HA memory for adaptation key info. */
    req_size = num_vp * sizeof(bcm_if_t);
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_VIRTUAL,
                                 VIRTUAL_SUB_COMP_ID_EGR_ADAPT_KEY,
                                 "bcmVirtualEgrAdaptKey",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);
    }
    vi->egress_adapt_key = ptr;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_virtual_init(unit));

    vi->initialized = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        virtual_free_resource(unit);
        if (!warm) {
            if (vi->vfi_bitmap != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit, vi->vfi_bitmap);
                vi->vfi_bitmap = NULL;
            }
            if (vi->mpls_vfi_bitmap != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit, vi->mpls_vfi_bitmap);
                vi->mpls_vfi_bitmap = NULL;
            }
            if (vi->flow_vfi_bitmap != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit, vi->flow_vfi_bitmap);
                vi->flow_vfi_bitmap = NULL;
            }
            if (vi->vp_info != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit, vi->vp_info);
                vi->vp_info = NULL;
            }
            if (vi->egress_if_map != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit, vi->egress_if_map);
                vi->egress_if_map = NULL;
            }
            if (vi->egress_adapt_key != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit, vi->egress_adapt_key);
                vi->egress_adapt_key = NULL;
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup virtual resource management.
 *
 * \param [in]  unit               Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
void
bcmi_ltsw_virtual_cleanup(int unit)
{
    virtual_free_resource(unit);
}

/*!
 * \brief Allocate a group of VPs.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   start     First VP index to allocate from.
 * \param [in]   end       Last PV index to allocate from.
 * \param [in]   count     Num of consecutive VPs to allocate.
 * \param [in]   vp_info   VP configuration.
 * \param [out]  base_vp   Base VP index allocated.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_alloc(int unit, int start, int end, int count,
                           bcmi_ltsw_virtual_vp_info_t vp_info, int *base_vp)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t num_vp;
    uint32_t i, j, vp;
    uint32_t found = 0;
    uint32_t reverse = 0;

    

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    num_vp = vi->vp_max + 1;
    if ((end >= num_vp) || (start >= num_vp)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (vp_info.flags & (~BCMI_LTSW_VIRTUAL_VP_ALL_MASK)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (start > end) {
        reverse = 1;
        end = end > vi->vp_min?end:vi->vp_min;
    } else {
        start = start > vi->vp_min?start:vi->vp_min;
    }

    VIRTUAL_LOCK(unit);

    if (reverse) {
        for (i = start; i >= end;) {
            for (j = 0; j < count; j++) {
                if (vi->vp_info[i - j].flags & BCMI_LTSW_VIRTUAL_VP_VALID) {
                    i -= (j + 1);
                    break;
                }
            }
            if (j == count) {
                break;
            }
        }
        if (i >= end) {
            found = 1;
        }
    } else {
        for (i = start; i <= end;) {
            for (j = 0; j < count; j++) {
                if (vi->vp_info[i + j].flags & BCMI_LTSW_VIRTUAL_VP_VALID) {
                    i += (j + 1);
                    break;
                }
            }
            if (j == count) {
                break;
            }
        }
        if (i <= end) {
            found = 1;
        }
    }

    if (found) {
        *base_vp = i;
        for (j = 0; j < count; j++) {
            if (reverse) {
                vp = i - j;
            } else {
                vp = i + j;
            }

            vi->vp_info[vp].flags = vp_info.flags;
            vi->vp_info[vp].flags |=  BCMI_LTSW_VIRTUAL_VP_VALID;
            vi->vp_info[vp].vpn = vp_info.vpn;
        }
        VIRTUAL_UNLOCK(unit);
        SHR_EXIT();
    } else {
        VIRTUAL_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Free a group of VPs.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   type      VP type.
 * \param [in]   count     Num of consecutive VPs to allocate.
 * \param [in]   base_vp   Base VP index allocated.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_free(int unit, bcmi_ltsw_virtual_vp_type_t type, int count,
                          int base_vp)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    if (type > BCMI_LTSW_VP_TYPE_CNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    VIRTUAL_LOCK(unit);

    /* Check VP type */
    for (i = 0; i < count; i++) {
        if (!(vi->vp_info[base_vp + i].flags & (1 << type))) {
            VIRTUAL_UNLOCK(unit);
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    for (i = 0; i < count; i++) {
        vi->vp_info[base_vp + i].flags = 0;
    }
    VIRTUAL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VP info.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [in]   vp_info   VP configuration.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_used_set(int unit, int vp,
                              bcmi_ltsw_virtual_vp_info_t vp_info)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    if (vp_info.flags & (~BCMI_LTSW_VIRTUAL_VP_ALL_MASK)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (vp < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (vp > vi->vp_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    VIRTUAL_LOCK(unit);
    sal_memcpy(&(vi->vp_info[vp]), &vp_info,
               sizeof(bcmi_ltsw_virtual_vp_info_t));
    vi->vp_info[vp].flags |= BCMI_LTSW_VIRTUAL_VP_VALID;
    VIRTUAL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether a VP is used or not.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [in]   vp_type   VP type.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_used_get(int unit, int vp,
                              bcmi_ltsw_virtual_vp_type_t vp_type)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    if (vp < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (vp > vi->vp_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (vp_type >= BCMI_LTSW_VP_TYPE_CNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(vi->vp_info[vp].flags & BCMI_LTSW_VIRTUAL_VP_VALID)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    if ((vp_type != BCMI_LTSW_VP_TYPE_ANY) &&
        ((vi->vp_info[vp].flags & (1 << vp_type )) == 0)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize bcmi_ltsw_virtual_vp_info_t structure.
 *
 * \param [in-out]   vp_info   VP info structure.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
void
bcmi_ltsw_virtual_vp_info_init (bcmi_ltsw_virtual_vp_info_t *vp_info)
{
    if (vp_info != NULL) {
        sal_memset(vp_info, 0, sizeof (*vp_info));
    }
    return;
}

/*!
 * \brief Get VP info based on VP ID.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [out]  vp_info   VP configuration.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_info_get(int unit, int vp,
                              bcmi_ltsw_virtual_vp_info_t *vp_info)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    if (vp > vi->vp_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    VIRTUAL_LOCK(unit);

    if (!(vi->vp_info[vp].flags & BCMI_LTSW_VIRTUAL_VP_VALID)) {
        VIRTUAL_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memcpy(vp_info, &(vi->vp_info[vp]),
        sizeof(bcmi_ltsw_virtual_vp_info_t));

    VIRTUAL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VP info based on certain VP id.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [in]   user_bmp  User bitmap.
 * \param [in]   flags     Flags about l2 if type.
 * \param [out]  vp_info   VP configuration.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_info_update(int unit, int vp, int flags,
                                 uint32_t user_bmp,
                                 bcmi_ltsw_virtual_vp_info_t *vp_info_new)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint16_t *flg_if =
        VIRTUAL_VP_USER_FLAG(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    uint32_t is_locked = 0, l2_oif_change = 0, dvp_pg_change = 0;
    int old_l2_oif = 0, new_l2_oif = 0, old_dvp_pg = 0, new_dvp_pg = 0;
    uint32_t old_pg_pbmp = 0, new_pg_pbmp = 0, pp_pbmp = 0;
    int sw_update = 0, new_pg_updated = 0;
    SHR_FUNC_ENTER(unit);

    if (!VIRTUAL_INITIALIZED(unit)) {
        SHR_EXIT();
    }

    if ((vp > vi->vp_max) ||
        !(vi->vp_info[vp].flags & BCMI_LTSW_VIRTUAL_VP_VALID)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pp_pipe_bmp(unit, &pp_pbmp));

    VIRTUAL_VP_USER_LOCK(unit);
    is_locked = 1;

    if (!vi->vp_manage.user_bmp){
        SHR_EXIT();
    }

    old_l2_oif = vi->vp_info[vp].l2_oif;
    if (user_bmp & (1 << BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF)){
        new_l2_oif = vp_info_new->l2_oif;
        if (new_l2_oif != old_l2_oif) {
            l2_oif_change = 1;
        }
        if (flags & BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK) {
            flg_if[new_l2_oif] |= BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK;
        } else if (flags & BCMI_LTSW_VIRTUAL_VP_USER_FLAG_REMOTE) {
            flg_if[new_l2_oif] |= BCMI_LTSW_VIRTUAL_VP_USER_FLAG_REMOTE;
        }
    }

    old_dvp_pg = vi->vp_info[vp].dvp_pg;
    if (user_bmp & (1 << BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG)){
        new_dvp_pg = vp_info_new->dvp_pg;
        if (old_dvp_pg != new_dvp_pg) {
            dvp_pg_change = 1;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (virtual_vp_info_update(unit, vp,
                                l2_oif_change, old_l2_oif, new_l2_oif,
                                dvp_pg_change, old_dvp_pg, new_dvp_pg));

    sw_update = 1;

    /* 0 is reserved value for both l2_oif and dvp_group.
     * If no pipe changes in this vp,  update sw only and exit.
     */
    if ((old_dvp_pg == 0 && new_dvp_pg == 0) ||
        (!l2_oif_change && !dvp_pg_change)) {
        SHR_EXIT();
    }

    if (dvp_pg_change && new_dvp_pg) {
        SHR_IF_ERR_VERBOSE_EXIT
            (virtual_vp_pg_pipe_get(unit, new_dvp_pg, &new_pg_pbmp));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_xlate_pipe_update(unit, new_dvp_pg, 0, new_pg_pbmp,
                                              bcmiVlanTableOpSet));
        new_pg_updated = 1;
    }

    if (old_dvp_pg) {
        SHR_IF_ERR_VERBOSE_EXIT
            (virtual_vp_pg_pipe_get(unit, old_dvp_pg, &old_pg_pbmp));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_xlate_pipe_update(unit, old_dvp_pg, 0, old_pg_pbmp,
                                              bcmiVlanTableOpSet));
    }

exit:
    if (SHR_FUNC_ERR() && sw_update) {
        (void)virtual_vp_info_update(unit, vp,
                                     l2_oif_change, new_l2_oif, old_l2_oif,
                                     dvp_pg_change, new_dvp_pg, old_dvp_pg);
        /* vlan_xlate_pipe_update has roll back itself,
         * So do roll back on new pg only.
         */
        if (new_pg_updated) {
            (void)virtual_vp_pg_pipe_get(unit, new_dvp_pg, &new_pg_pbmp);
            (void)bcmi_ltsw_vlan_xlate_pipe_update(unit, new_dvp_pg, 0,
                      new_pg_pbmp, bcmiVlanTableOpSet);
        }
    }

    if (is_locked) {
        VIRTUAL_VP_USER_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VP info based on trunk change.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   tid          Tunk id.
 * \param [in]   del          Whether to delete this trunk.
 * \param [in]   leave_pbmp   Leaving ports pipe bitmap.
 * \param [in]   stay_pbmp    Staying ports pipe bitmap.
 * \param [in]   join_pbmp    Joining ports pipe bitmap.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_trunk_pipe_update(int unit, int l2_oif, int del,
                    uint32_t leave_pbmp, uint32_t stay_pbmp, uint32_t join_pbmp)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint16_t *vp_list = VIRTUAL_VP_LIST(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    uint16_t *vp_head = VIRTUAL_VP_HEAD(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    uint16_t *flg_if =
        VIRTUAL_VP_USER_FLAG(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    uint16_t *bmp =
        VIRTUAL_VP_USER_PBMP(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    uint16_t old_bmp;
    int i, start_hw = 0, k, alloc_size;
    uint32_t leave_diff, join_diff, pg_pbmp = 0;
    uint16_t *pg_array = NULL;
    int pg, is_locked = 0;
    SHR_FUNC_ENTER(unit);

    if (!VIRTUAL_INITIALIZED(unit)){
        SHR_EXIT();
    }
    VIRTUAL_VP_USER_LOCK(unit);
    is_locked = 1;

    if (!vi->vp_manage.user_bmp) {
        SHR_EXIT();
    }

    old_bmp = bmp[l2_oif];
    if (del) {
        leave_pbmp = bmp[l2_oif];
        stay_pbmp = 0;
        join_pbmp = 0;
        /* Update bitmap first to calculate pipe, flag updates at last.*/
        bmp[l2_oif] = 0;
    } else {
        flg_if[l2_oif] |= BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK;
        bmp[l2_oif] = stay_pbmp | join_pbmp;
    }

    leave_diff = leave_pbmp & ~stay_pbmp;
    join_diff = join_pbmp & ~stay_pbmp;

    /* Return E_NONE when pipe does not change. */
    if (!leave_diff && !join_diff) {
        SHR_EXIT();
    }

    alloc_size = (vi->vp_max + 1) * sizeof(uint16_t);
    SHR_ALLOC(pg_array, alloc_size, "bcmVirtuaPgIdArray");
    SHR_NULL_CHECK(pg_array, SHR_E_MEMORY);
    sal_memset(pg_array, 0, alloc_size);

    start_hw = 1;
    for (k = vp_head[l2_oif]; k != 0; k = vp_list[k]) {
        pg = vi->vp_info[k].dvp_pg;
        /* If this pg is updated before, skip. */
        if (pg_array[pg]) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
             (virtual_vp_pg_pipe_get(unit, pg, &pg_pbmp));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_xlate_pipe_update(unit, pg, 0, pg_pbmp,
                                              bcmiVlanTableOpSet));

        /* Record traverse info. */
        pg_array[pg] = 1;
    }

    /* Update flags. */
    if (del) {
        flg_if[l2_oif] &= ~BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK;
    }

exit:

    /* Roll back on error.*/
    if (SHR_FUNC_ERR() && start_hw) {
        bmp[l2_oif] = old_bmp;
        for (i = vp_head[l2_oif]; i != k; i = vp_list[i]) {
            pg = vi->vp_info[i].dvp_pg;
            if (pg_array[pg]) {
                (void)virtual_vp_pg_pipe_get(unit, pg, &pg_pbmp);
                (void)bcmi_ltsw_vlan_xlate_pipe_update(unit, pg, 0, pg_pbmp,
                                                       bcmiVlanTableOpSet);
                pg_array[pg] = 0;
            }
        }
    }

    if (is_locked) {
        VIRTUAL_VP_USER_UNLOCK(unit);
    }
    SHR_FREE(pg_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a VFI.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VFI type.
 * \param [out]  vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_alloc(int unit, bcmi_ltsw_virtual_vfi_type_t type,
                            int *vfi)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    VIRTUAL_LOCK(unit);

    for (i = vi->vfi_min; i <= vi->vfi_max; i++) {
        if (!VIRTUAL_VFI_USED_GET(unit, i)) {
            break;
        }
    }
    if (i > vi->vfi_max) {
        VIRTUAL_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    *vfi = i;
    VIRTUAL_VFI_USED_SET(unit, i);
    switch (type) {
        case BCMI_LTSW_VFI_TYPE_MPLS:
            VIRTUAL_MPLS_VFI_USED_SET(unit, i);
            break;

        case BCMI_LTSW_VFI_TYPE_FLOW:
            VIRTUAL_FLOW_VFI_USED_SET(unit, i);
            break;

        default:
            break;
    }

    SHR_IF_ERR_CONT
        (virtual_vfi_init(unit, i));

    VIRTUAL_UNLOCK(unit);

    SHR_IF_ERR_EXIT
        (virtual_vfi_stg_add(unit, i));

    SHR_IF_ERR_EXIT
        (virtual_vfi_check_info_notify(unit, i, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a VFI with given ID.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VFI type.
 * \param [out]  vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_alloc_with_id(int unit, bcmi_ltsw_virtual_vfi_type_t type,
                                    int vfi)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    VIRTUAL_LOCK(unit);

    if ( vfi < vi->vfi_min || vfi > vi->vfi_max ) {
        VIRTUAL_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (VIRTUAL_VFI_USED_GET(unit, vfi)) {
        VIRTUAL_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    VIRTUAL_VFI_USED_SET(unit, vfi);

    switch (type) {
        case BCMI_LTSW_VFI_TYPE_MPLS:
            VIRTUAL_MPLS_VFI_USED_SET(unit, vfi);
            break;

        case BCMI_LTSW_VFI_TYPE_FLOW:
            VIRTUAL_FLOW_VFI_USED_SET(unit, vfi);
            break;

        default:
            break;
    }

    SHR_IF_ERR_CONT
        (virtual_vfi_init(unit, vfi));

    VIRTUAL_UNLOCK(unit);

    SHR_IF_ERR_EXIT
        (virtual_vfi_stg_add(unit, vfi));

    SHR_IF_ERR_EXIT
        (virtual_vfi_check_info_notify(unit, vfi, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a VFI.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VFI type.
 * \param [in]   vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_free(int unit, bcmi_ltsw_virtual_vfi_type_t type, int vfi)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (virtual_vfi_stg_remove(unit, vfi));

    SHR_IF_ERR_EXIT
        (virtual_vfi_check_info_notify(unit, vfi, 0));

    VIRTUAL_LOCK(unit);

    SHR_IF_ERR_CONT
        (virtual_vfi_cleanup(unit, vfi));

    if (SHR_FUNC_ERR()) {
        VIRTUAL_UNLOCK(unit);
        SHR_EXIT();
    }

    VIRTUAL_VFI_USED_CLR(unit, vfi);
    switch (type) {
        case BCMI_LTSW_VFI_TYPE_MPLS:
            VIRTUAL_MPLS_VFI_USED_CLR(unit, vfi);
            break;

        case BCMI_LTSW_VFI_TYPE_FLOW:
            VIRTUAL_FLOW_VFI_USED_CLR(unit, vfi);
            break;

        default:
            break;
    }
    VIRTUAL_UNLOCK(unit);

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether a VFI is used or not.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   vfi        VFI index.
 * \param [in]   type       VFI type.
 *
 * \retval TRUE   Used.
 * \retval FALSE  Not used.
 */
int
bcmi_ltsw_virtual_vfi_used_get(int unit, int vfi,
                               bcmi_ltsw_virtual_vfi_type_t type)
{
    int used = 0;

    switch (type) {
        case BCMI_LTSW_VFI_TYPE_MPLS:
            used = VIRTUAL_MPLS_VFI_USED_GET(unit, vfi);
            break;

        case BCMI_LTSW_VFI_TYPE_FLOW:
            used = VIRTUAL_FLOW_VFI_USED_GET(unit, vfi);
            break;

        default:
            used = VIRTUAL_VFI_USED_GET(unit, vfi);
            break;
    }

    return used;
}

/*!
 * \brief Get physical VFI index of VPN.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vpn          VPN ID.
 * \param [out]  vfi          Physical VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_idx_get(int unit, bcm_vpn_t vpn, uint32_t *vfi)
{
    uint32_t vpn_type;
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if ((vi->initialized) && BCMI_LTSW_VIRTUAL_VPN_IS_SET(vpn)) {
        if (BCMI_LTSW_VIRTUAL_VPN_IS_L3(vpn)) {
            vpn_type = BCM_VPN_TYPE_MPLS_L3;
        } else if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
            vpn_type = BCM_VPN_TYPE_MPLS_VPLS;
        } else if (BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(vpn)) {
            vpn_type = BCM_VPN_TYPE_MPLS_VPWS;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        BCMI_LTSW_VIRTUAL_VPN_GET(*vfi, vpn_type,  vpn);
        if (*vfi > vi->vfi_max) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (vpn > BCM_VLAN_MAX) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            *vfi = vpn;
        }
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether a VFI based VPN is valid.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vpn          VPN or VLAN ID.
 *
 * \retval TRUE    ID is valid.
 * \retval FALSE   ID is invalid.
 */
int
bcmi_ltsw_virtual_vlan_vpn_valid(int unit, bcm_vpn_t vpn)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t vfi;

    if ((vi && (vi->initialized)) && BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vpn)) {
        BCMI_LTSW_VIRTUAL_VPN_GET(vfi, BCM_VPN_TYPE_MPLS_VPLS, vpn);
        if (vfi <= vi->vfi_max) {
            return TRUE;
        }
    } else {
        if (vpn <= BCM_VLAN_MAX) {
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief Get VPN ID based on physical VFI index.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vfi          Physical VFI index.
 * \param [out]  vpn          VPN ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vpnid_get(int unit, uint32_t vfi, bcm_vpn_t *vpn)
{
    if (vfi <= BCM_VLAN_MAX) {
        *vpn = vfi;
    } else {
        BCM_VPN_MPLS_VPLS_VPN_ID_SET(*vpn, vfi);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get VFI num.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  vfi_num      Num of VFI entires.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_num_get(int unit, uint32_t *vfi_num)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    *vfi_num = vi->vfi_max + 1;

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get VP num.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  vp_num       Num of VP entires.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_num_get(int unit, uint32_t *vp_num)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    *vp_num = vi->vp_max + 1;

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Consruct gport based on VP.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vp           VP ID.
 * \param [out]  gport        gport of VP.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_encode_gport(int unit, int vp, int *gport)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    if (vp > vi->vp_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    flags = vi->vp_info[vp].flags;
    if (flags & BCMI_LTSW_VIRTUAL_VP_VALID) {
        if (flags & BCMI_LTSW_VIRTUAL_VP_TYPE_MPLS) {
            BCM_GPORT_MPLS_PORT_ID_SET(*gport, vp);
        } else if (flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW) {
            BCM_GPORT_FLOW_PORT_ID_SET(*gport, vp);
        } else if (flags & BCMI_LTSW_VIRTUAL_VP_TYPE_VPLAG) {
            BCM_GPORT_VPLAG_PORT_ID_SET(*gport, vp);
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Consruct gport info based on VP.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vp           VP ID.
 * \param [out]  gport_dest   gport info.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_gport_dest_fill(int unit, int vp,
                                     bcmi_ltsw_gport_info_t *gport_dest)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    if (vp > vi->vp_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    flags = vi->vp_info[vp].flags;
    if (flags & BCMI_LTSW_VIRTUAL_VP_VALID) {
        if (flags & BCMI_LTSW_VIRTUAL_VP_TYPE_MPLS) {
            gport_dest->gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
            gport_dest->mpls_id = vp;
        } else if (flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW) {
            gport_dest->gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
            gport_dest->mpls_id = vp;
        } else if (flags & BCMI_LTSW_VIRTUAL_VP_TYPE_VPLAG) {
            gport_dest->gport_type = _SHR_GPORT_TYPE_VP_GROUP;
        } else {
            gport_dest->gport_type = _SHR_GPORT_INVALID;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief  Finds out if GPORT is VFI type virtual port.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        gport ID.
 *
 * \retval  TRUE/FALSE
 */
int
bcmi_ltsw_virtual_vp_is_vfi_type(int unit, bcm_gport_t gport)
{
    int gport_type = 0;

    gport_type = (gport >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK;

    switch(gport_type) {
        case BCM_GPORT_MPLS_PORT:
        case BCM_GPORT_FLOW_PORT:
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

/*!
 * \brief Checks and gets a valid vp.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        gport ID.
 * \param [out]  vp           VP ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_type_vp_get(int unit, bcm_gport_t gport, int *vp)
{
    int gport_type = 0;

    SHR_FUNC_ENTER(unit);

    gport_type = (gport >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK;

    switch(gport_type) {
        case BCM_GPORT_MPLS_PORT:
            *vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
            SHR_EXIT();
            break;
        case BCM_GPORT_FLOW_PORT:
            *vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
            SHR_EXIT();
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get VFI index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        VFI start index.
 * \param [out]  end          VFI end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vfi_range_get(int unit, uint32_t *start, uint32_t *end)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    *start = vi->vfi_min;
    *end = vi->vfi_max;

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get VP index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        VP start index.
 * \param [out]  end          VP end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_range_get(int unit, uint32_t *start, uint32_t *end)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    *start = vi->vp_min;
    *end = vi->vp_max;

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get L2_IIF index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        L2_IIF start index.
 * \param [out]  end          L2_IIF end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_l2_iif_range_get(int unit, uint32_t *start, uint32_t *end)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    *start = vi->l2_iif_min;
    *end = vi->l2_iif_max;

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get L2_IIF num.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  l2_iif_num   Num of L2_IIF entires.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_l2_iif_num_get(int unit, uint32_t *l2_iif_num)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    *l2_iif_num = vi->l2_iif_max + 1;

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_learn_get(unit, port, flags));

exit:

    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_learn_set(unit, port, flags));

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 * \param [in] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                       bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_vlan_action_set(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get virtual port's vlan actions.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  port   Gport.
 * \param [out] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                       bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_vlan_action_get(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reset virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_vlan_action_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_vlan_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the class ID of this port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  port Port number.
 * \param [in]  pclass Classification type.
 * \param [out] pclass_id class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_class_get(int unit, bcm_port_t port,
                                 bcm_port_class_t pclass,
                                 uint32_t *pclass_id)

{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_NULL_CHECK(pclass_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_class_get(unit, port, pclass, pclass_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [in] pclass_id Class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_class_set(int unit, bcm_port_t port,
                                 bcm_port_class_t pclass,
                                 uint32_t pclass_id)

{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_class_set(unit, port, pclass, pclass_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get status of VLAN specified port feature.
 *
 * \param [in]  unit Unit number.
 * \param [in]  port Port number.
 * \param [in]  type Vlan control type.
 * \param [out] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                     bcm_vlan_control_port_t type,
                                     int *arg)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_vlan_ctrl_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/Disable VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [in] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                     bcm_vlan_control_port_t type,
                                     int arg)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_vlan_ctrl_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get status of Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [out]  enable     Bitmap of mirror session status.
 * \param [out]  mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_mirror_ctrl_get(int unit, bcm_port_t port, int *enable,
                                       int *mirror_idx)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);
    SHR_NULL_CHECK(mirror_idx, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_mirror_ctrl_get(unit, port, enable, mirror_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/Disable Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [in]   enable     Bitmap of mirror session status.
 * \param [in]   mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_mirror_ctrl_set(int unit, bcm_port_t port, int enable,
                                       int *mirror_idx)
{
    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_mirror_ctrl_set(unit, port, enable, mirror_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Save the virtual port cascaded state.
 *
 * \param [in] unit         Unit number.
 * \param [in] port         Gport number.
 * \param [in] cascaded  cascaded state.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_cascaded_set(
    int unit, bcm_gport_t port, int cascaded)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    VIRTUAL_LOCK(unit);
    if (cascaded) {
        vi->vp_info[vp].flags |= BCMI_LTSW_VIRTUAL_VP_CASCADED;
    } else {
        vi->vp_info[vp].flags &= ~BCMI_LTSW_VIRTUAL_VP_CASCADED;
    }
    VIRTUAL_UNLOCK(unit);

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress object to port.
 *
 * \param [in] unit        Unit number.
 * \param [in] port        Gport number.
 * \param [in] egress_if   Egress object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_egress_obj_set(int unit, bcm_gport_t port,
                                      bcm_if_t egress_if)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    VIRTUAL_LOCK(unit);
    vi->egress_if_map[vp] = egress_if;
    VIRTUAL_UNLOCK(unit);

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress object from port.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  port        Gport number.
 * \param [out] egress_if   Egress object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_egress_obj_get(int unit, bcm_gport_t port,
                                      bcm_if_t *egress_if)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    VIRTUAL_LOCK(unit);
    *egress_if = vi->egress_if_map[vp];
    VIRTUAL_UNLOCK(unit);

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Check bind status of egress object.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  egress_if   Egress object.
 * \param [out] used        bind status.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_egress_obj_used(int unit, bcm_if_t egress_if, int *used)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    uint32_t vp = 0, start = 0, end = 0;

    SHR_FUNC_ENTER(unit);

    *used = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_range_get(unit, &start, &end));
    for (vp = start; vp < end; vp++) {
        if (vi->egress_if_map[vp] == egress_if) {
            *used = 1;
            SHR_EXIT();
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_dscp_map_mode_set(unit, port, mode));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_dscp_map_mode_get(unit, port, mode));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get VLAN based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr VLAN based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_ptr)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_phb_vlan_map_get(unit, port, map_ptr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set VLAN based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr VLAN based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_ptr)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_phb_vlan_map_set(unit, port, map_ptr));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get DSCP based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr DSCP based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_ptr)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_phb_dscp_map_get(unit, port, map_ptr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set DSCP based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr DSCP based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_ptr)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_phb_dscp_map_set(unit, port, map_ptr));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Get VLAN based PHB remarking mapping pointer.
*
* \param [in] unit Unit number.
* \param [in] port Port number.
* \param [out] map_ptr VLAN based PHB remarking mapping  ID.
*
* \retval SHR_E_NONE              No errors.
* \retval !SHR_E_NONE             Failure.
*/
int
bcmi_ltsw_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                               int *map_ptr)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_phb_vlan_remark_map_get(unit, port, map_ptr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set VLAN based PHB remarking mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr VLAN based PHB remarking mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_phb_vlan_remark_map_set(int unit, bcm_port_t port,
                                               int map_ptr)
{
  int vp;

  SHR_FUNC_ENTER(unit);

  SHR_IF_ERR_EXIT
      (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

  SHR_IF_ERR_EXIT
      (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

  SHR_IF_ERR_EXIT
      (mbcm_virtual_port_phb_vlan_remark_map_set(unit, port, map_ptr));

exit:
  SHR_FUNC_EXIT();
}

int
bcmi_ltsw_virtual_nw_group_num_get(int unit, int *num)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_nw_group_num_get(unit, num));
exit:
  SHR_FUNC_EXIT();
}

/*!
 * \brief Get pipe bitmap for a dvp port group.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  pg        Port goup.
 * \param [out] pipe_bmp  Pipe bitmap.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_vp_group_pipe_get(int unit, int pg, uint32_t *pipe_bmp)
{
    int is_locked = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_bmp, SHR_E_PARAM);
    if (!VIRTUAL_INITIALIZED(unit)){
        *pipe_bmp = 0;
        SHR_EXIT();
    }

    if (pg > (VIRTUAL_INFO(unit)->vp_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    VIRTUAL_VP_USER_LOCK(unit);
    is_locked = 1;

    if (!VIRTUAL_INFO(unit)->vp_manage.user_bmp) {
        *pipe_bmp = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (virtual_vp_pg_pipe_get(unit, pg, pipe_bmp));

exit:
    if (is_locked) {
        VIRTUAL_VP_USER_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up trunk user in vp info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_virtual_vp_user_trunk_cleanup(int unit)
{
    int warm = bcmi_warmboot_get(unit);
    uint16_t *flg_if =
        VIRTUAL_VP_USER_FLAG(unit, BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF);
    int i;
    SHR_FUNC_ENTER(unit);

    if (!VIRTUAL_INITIALIZED(unit) || warm){
        SHR_EXIT();
    }

    if (!(VIRTUAL_INFO(unit)->vp_manage.user_bmp &
          (1 << BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF))) {
          SHR_EXIT();
    }

    for (i = 0; i <= VIRTUAL_INFO(unit)->l2_iif_max; i++) {
        if (flg_if[i] & BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_trunk_pipe_update(unit, i, 1, 0, 0, 0));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [out] value Current value of the port feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_control_get(int unit, bcm_port_t port,
                                   bcm_port_control_t type, int *value)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    SHR_IF_ERR_EXIT
        (mbcm_virtual_port_control_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [in] value Value of the port feature to be set.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_virtual_port_control_set(int unit, bcm_port_t port,
                                   bcm_port_control_t type, int value)
{
  int vp;

  SHR_FUNC_ENTER(unit);

  SHR_IF_ERR_EXIT
      (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

  SHR_IF_ERR_EXIT
      (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

  SHR_IF_ERR_EXIT
      (mbcm_virtual_port_control_set(unit, port, type, value));

exit:
  SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_virtual_port_stat_attach(int unit, bcm_port_t port,
                                   uint32_t stat_counter_id)
{
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_virtual_port_stat_attach(unit, port, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries to given port with a given stat counter id.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_virtual_port_stat_detach_with_id(int unit, bcm_port_t port,
                                           uint32_t stat_counter_id)
{
    int vp;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_virtual_port_stat_detach_with_id(unit, port, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the split horizon network group configuration for
 * a given pair of source and destination network group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source_network_group_id Source network group id.
 * \param [in] config Configuration information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_virtual_network_group_split_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_virtual_network_group_split_config_set(
            unit,
            source_network_group_id,
            config));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the split horizon network group configuration for
 * a given pair of source and destination network group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source_network_group_id Source network group id.
 * \param [out] config Configuration information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_virtual_network_group_split_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_virtual_network_group_split_config_get(
            unit,
            source_network_group_id,
            config));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress adaptation key to port.
 *
 * \param [in] unit        Unit number.
 * \param [in] port        Gport number.
 * \param [in] key_type    Lookup key.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_egress_adapt_key_set(
    int unit,
    bcm_gport_t port,
    int key_type)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    vi->egress_adapt_key[vp] = key_type;

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress adaptation key from port.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  port        Gport number.
 * \param [out] key_type    Lookup key.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_egress_adapt_key_get(
    int unit,
    bcm_gport_t port,
    int *key_type)
{
    ltsw_virtual_info_t *vi = VIRTUAL_INFO(unit);
    int vp;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_type, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    *key_type = vi->egress_adapt_key[vp];

exit:

    SHR_FUNC_EXIT();
}


/*!
 * \brief sync egress adaptation key to HW.
 *
 * \param [in] unit        Unit number.
 * \param [in] port        Gport number.
 * \param [in] egress_if   Egress object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_egress_adapt_key_sync(
    int unit,
    bcm_gport_t port,
    bcm_if_t egress_if)
{
    int vp;
    int local_rv = SHR_E_NONE;
    bcm_l3_egress_t egr;
    int key_type;
    bcm_if_t egress_if_ul = BCM_IF_INVALID;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_port_egress_adapt_key_get(unit, port, &key_type));

    /*
     * Case 1:
     * Sync during port encap, the egress object is known.
     *
     * Case 2:
     * Sync during key configuration, the egress object is unknown.
     *
     * Case 2.1:
     * For underlay egress object, if port encap configured, the mapping
     * between (VP, egress object) has been stored in SW.
     *
     * Case 2.2:
     * For overlay egress object, there is no port encap linkage and
     * mapping is configured in HW during egress object creation.
     */

    if (BCM_IF_INVALID == egress_if) {
        /*
         * Case 2.2
         * Sync overlay egress object for riot and no need to sync underlay.
         */
        egress_adapt_key_sync_t user_data;
        user_data.vp = port;
        user_data.key_type = key_type;
        user_data.synced = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l3_egress_traverse(
                unit,
                virtual_vp_egress_adapt_key_sync_traverse_cb,
                &user_data));

        /*
         * Case 2.1
         * Should be switch case where there is no overlay object.
         * Check whether underlay egress object has been linkaged to VP.
         */
        if (user_data.synced == 0) {
            local_rv = bcmi_ltsw_virtual_port_egress_obj_get(unit, port,
                                                             &egress_if_ul);
            if ((local_rv == SHR_E_NONE) && (egress_if_ul != 0)) {
                egress_if = egress_if_ul;
            }
        }
    }

    /* Sync underlay egress object. */
    if (BCM_IF_INVALID != egress_if) {
        if (local_rv == SHR_E_NONE) {
            bcm_l3_egress_t_init(&egr);
            local_rv = bcm_ltsw_l3_egress_get(unit, egress_if, &egr);
            if ((local_rv == SHR_E_NONE) && egr.intf) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_intf_adapt_lookup_key_set(unit, egr.intf,
                                                            key_type));
            }
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get tunnel index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        Tunnel start index.
 * \param [out]  end          Tunnel end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_tnl_range_get(int unit, uint32_t *start, uint32_t *end)
{
    int tnl_min = 0;
    int tnl_max = 0;

    SHR_FUNC_ENTER(unit);

    VIRTUAL_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_virtual_tnl_index_get(unit, &tnl_min, &tnl_max));
    *start = (uint32_t)tnl_min;
    *end = (uint32_t)tnl_max;

exit:
    SHR_FUNC_EXIT();
}
