/*! \file vlan.c
 *
 * VLAN management.
 * This file contains the management for VLAN.
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
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/mbcm/vlan.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/vlan_int.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/generated/vlan_ha.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

/*
 * \brief VLAN information.
 */
typedef struct ltsw_vlan_info_s {
    /* VLAN initialized flag. */
    int                    initialized;

    /* Default VLAN. */
    bcm_vlan_t             *def_vlan;

    /* Bitmap of existing VLANs. */
    SHR_BITDCL             *vlan_bmp;

    /* Default flood mode. */
    uint8_t *flood_mode;

    /* Bitmap of pre-configuratioin VLANs. */
    SHR_BITDCL             *pre_cfg_vlan_bmp;

    /* Auto stack flag. */
    int                    vlan_auto_stack;

    /* Port type in default VLAN. */
    int                    default_port_type;

    /* Protection mutex. */
    sal_mutex_t            mutex;

    /* Init all VLAN entries during init stage.*/
    uint8_t                *vlan_entry_inited;
} ltsw_vlan_info_t;

static ltsw_vlan_info_t ltsw_vlan_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* VLAN information */
#define VLAN_INFO(u)        (&ltsw_vlan_info[u])

/* VLAN Lock */
#define VLAN_LOCK(u) \
    sal_mutex_take(VLAN_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define VLAN_UNLOCK(u) \
    sal_mutex_give(VLAN_INFO(u)->mutex)

/* Check if VLAN module is initialized. */
#define VLAN_INIT_CHECK(u)                     \
    do {                                       \
        ltsw_vlan_info_t *info = VLAN_INFO(u); \
        if (!info->initialized) {              \
            SHR_ERR_EXIT(SHR_E_INIT);   \
        }                                      \
    } while(0)

/* Check VLAN ID. */
#define VLAN_ID_CHECK(vid)                     \
    do {                                       \
        if (vid > BCM_VLAN_MAX ||              \
            vid <= BCM_VLAN_MIN) {             \
            SHR_ERR_EXIT(SHR_E_PARAM);  \
        }                                      \
    } while(0)

#define VLAN_OTPID_MAX_NUM 4

/*!
 * \brief VLAN outer TPID information.
 *
 * This data structure is used to identify the VLAN outer TPID information.
 */
typedef struct ltsw_vlan_otpid_s {
    /*! TPID value. */
    uint16_t               tpid;
    /*! Reference counter. */
    int                    ref_count;
} ltsw_vlan_otpid_t;

/*!
 * \brief VLAN TPID information.
 *
 * This data structure is used to identify the VLAN TPID information.
 */
typedef struct ltsw_vlan_otpid_info_s {
    /*! TRUE if VLAN TPID module has been initialized. */
    int                    initialized;
    /*! Outer TPID array. */
    ltsw_vlan_otpid_t      *otpids;
    /*! Protection mutex. */
    sal_mutex_t            mutex;
} ltsw_vlan_otpid_info_t;

static ltsw_vlan_otpid_info_t ltsw_vlan_otpid_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* TPID information */
#define VLAN_OTPID_INFO(u) (&ltsw_vlan_otpid_info[u])

#define VLAN_OTPID_ENTRY(u, idx) \
    ((VLAN_OTPID_INFO(u)->otpids)[idx].tpid)

#define VLAN_OTPID_REF_COUNT(u, idx) \
    ((VLAN_OTPID_INFO(u)->otpids)[idx].ref_count)

/* TPID Lock */
#define VLAN_OTPID_LOCK(u) \
    sal_mutex_take(VLAN_OTPID_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define VLAN_OTPID_UNLOCK(u) \
    sal_mutex_give(VLAN_OTPID_INFO(u)->mutex)

/*!
 * \brief payload outer TPID information.
 */

#define VLAN_PAYLOAD_OTPID_MAX_NUM 4

static ltsw_vlan_otpid_info_t ltsw_vlan_payload_otpid_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* payload outer TPID information */
#define VLAN_PAYLOAD_OTPID_INFO(u) (&ltsw_vlan_payload_otpid_info[u])

#define VLAN_PAYLOAD_OTPID_ENTRY(u, idx) \
    ((VLAN_PAYLOAD_OTPID_INFO(u)->otpids)[idx].tpid)

#define VLAN_PAYLOAD_OTPID_REF_COUNT(u, idx) \
    ((VLAN_PAYLOAD_OTPID_INFO(u)->otpids)[idx].ref_count)

/* payload outer TPID Lock */
#define VLAN_PAYLOAD_OTPID_LOCK(u) \
    sal_mutex_take(VLAN_PAYLOAD_OTPID_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define VLAN_PAYLOAD_OTPID_UNLOCK(u) \
    sal_mutex_give(VLAN_PAYLOAD_OTPID_INFO(u)->mutex)

/*!
 * \brief VLAN ITPID information.
 *
 * This data structure is used to identify the VLAN ITPID information.
 */
typedef struct ltsw_vlan_itpid_info_s {
    /*! TRUE if VLAN ITPID module has been initialized. */
    int  initialized;
    /*! Inner TPID. */
    uint16_t  itpid;
    /*! Protection mutex. */
    sal_mutex_t  mutex;
} ltsw_vlan_itpid_info_t;

static ltsw_vlan_itpid_info_t ltsw_vlan_itpid_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* ITPID information */
#define VLAN_ITPID_INFO(u) (&ltsw_vlan_itpid_info[u])

/* ITPID Lock */
#define VLAN_ITPID_LOCK(u) \
    sal_mutex_take(VLAN_ITPID_INFO(u)->mutex, SAL_MUTEX_FOREVER)
#define VLAN_ITPID_UNLOCK(u) \
    sal_mutex_give(VLAN_ITPID_INFO(u)->mutex)


/******************************************************************************
* Private functions
 */

/*!
 * \brief Cleanup the VLAN information structures.
 *
 * This function is used to cleanup the VLAN information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
vlan_info_cleanup(int unit)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);

    /* Destory mutex */
    if (vi->mutex) {
        sal_mutex_destroy(vi->mutex);
        vi->mutex = NULL;
    }

    /* Cleanup the resource of VLAN bitmap */
    SHR_FREE(vi->vlan_bmp);

    sal_memset(vi, 0, sizeof(ltsw_vlan_info_t));

    return;
}

/*!
 * \brief Initialize the VLAN information structures.
 *
 * This function is used to initialize the VLAN information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_info_init(int unit)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);
    int wb_flag = bcmi_warmboot_get(unit);
    uint32_t alloc_size = 0;
    int auto_stack = 0;
    bcmint_vlan_config_info_t *tmp_ha_info = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);

    /* Cleanup previous resource */
    if (vi->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_detach(unit));
    }

    /* Create mutex */
    if (NULL == vi->mutex) {
        vi->mutex = sal_mutex_create("VLAN mutex");
        SHR_NULL_CHECK(vi->mutex, SHR_E_MEMORY);
    }

    /* Allocate the resource of VLAN bitmap */
    alloc_size = SHR_BITALLOCSIZE(BCM_VLAN_COUNT);
    SHR_ALLOC(vi->vlan_bmp,
              alloc_size, "Active vlan bitmap");
    SHR_NULL_CHECK(vi->vlan_bmp, SHR_E_MEMORY);
    sal_memset(vi->vlan_bmp, 0, alloc_size);

    alloc_size = sizeof(bcmint_vlan_config_info_t);
    tmp_ha_info = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_VLAN,
                                         BCMINT_VLAN_SUB_COMP_ID_HA_CFG,
                                         "bcmVlanCfg",
                                         &alloc_size);
    SHR_NULL_CHECK(tmp_ha_info, SHR_E_MEMORY);
    if (!wb_flag) {
        sal_memset(tmp_ha_info, 0, alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
              BCMI_HA_COMP_ID_VLAN,
              BCMINT_VLAN_SUB_COMP_ID_HA_CFG,
              0, alloc_size, 1,
              BCMINT_VLAN_CONFIG_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    vi->def_vlan = &(tmp_ha_info->def_vlan);
    vi->pre_cfg_vlan_bmp = tmp_ha_info->pre_cfg_vlan_bmp;
    vi->vlan_entry_inited = &(tmp_ha_info->vlan_entry_inited);
    vi->flood_mode = &(tmp_ha_info->flood_mode);

    if (!wb_flag) {
        *(vi->flood_mode) = BCM_VLAN_MCAST_FLOOD_UNKNOWN;
    }

    /*
     * Get the configuration property for port type in default VLAN.
     * Legacy Compile Marcos BCM_VLAN_NO_DEFAULT_ETHER/BCM_VLAN_NO_DEFAULT_CPU
     * have been replaced by it.
     */
    vi->default_port_type =
        bcmi_ltsw_property_get(unit, BCMI_CPN_VLAN_DEFAULT_PORT, 0);

    if (ltsw_feature(unit, LTSW_FT_STK)) {
        /* Get the configuration property for vlan auto stack. */
        auto_stack = bcmi_ltsw_property_get(unit, BCMI_CPN_VLAN_AUTO_STACK, 0);
        if (auto_stack == 1) {
            vi->vlan_auto_stack = 1;
        } else if (auto_stack == 2) {
            vi->vlan_auto_stack = 0;
        } else {
            vi->vlan_auto_stack = 1;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!wb_flag && tmp_ha_info) {
            (void) bcmi_ltsw_ha_mem_free(unit, tmp_ha_info);
            tmp_ha_info = NULL;
            vi->def_vlan = NULL;
            vi->pre_cfg_vlan_bmp = NULL;
        }

        vlan_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup the VLAN ITPID information structures.
 *
 * This function is used to cleanup the VLAN ITPID information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
vlan_itpid_info_cleanup(int unit)
{
    ltsw_vlan_itpid_info_t *itpid = VLAN_ITPID_INFO(unit);

    /* Destory mutex */
    if (itpid->mutex) {
        sal_mutex_destroy(itpid->mutex);
        itpid->mutex = NULL;
    }
    sal_memset(itpid, 0, sizeof(ltsw_vlan_itpid_info_t));
    return;
}

/*!
 * \brief Cleanup the VLAN TPID information structures.
 *
 * This function is used to cleanup the VLAN TPID information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
vlan_otpid_info_cleanup(int unit)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);

    /* Destory mutex */
    if (otpid->mutex) {
        sal_mutex_destroy(otpid->mutex);
        otpid->mutex = NULL;
    }

    /* Cleanup the resource of VLAN TPID */
    SHR_FREE(otpid->otpids);

    sal_memset(otpid, 0, sizeof(ltsw_vlan_otpid_info_t));

    return;
}

/*!
 * \brief Initialize the outer VLAN TPID information.
 *
 * This function is used to initialize the outer VLAN TPID information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_info_init(int unit)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);
    int                     alloc_size  = 0;

    SHR_FUNC_ENTER(unit);

    /* Cleanup previous resource */
    if (otpid->initialized) {
        bcmi_ltsw_vlan_otpid_detach(unit);
    }

    /* Create mutex */
    if (NULL == otpid->mutex) {
        otpid->mutex = sal_mutex_create("VLAN TPID mutex");
        SHR_NULL_CHECK(otpid->mutex , SHR_E_MEMORY);
    }

    /*
     * Allocate memory to cache VLAN_OUTER_TPID entries.
     * ING_OUTER_TPID entries are always the same as EGR_OUTER_TPID entries.
     */
    alloc_size = sizeof(ltsw_vlan_otpid_t) * VLAN_OTPID_MAX_NUM;
    SHR_ALLOC(otpid->otpids, alloc_size, "VLAN OTPID Array");
    SHR_NULL_CHECK(otpid->otpids, SHR_E_MEMORY);
    sal_memset(otpid->otpids, 0, alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        vlan_otpid_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup the payload outer TPID information structures.
 *
 * This function is used to cleanup the payload outer TPID information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
vlan_payload_otpid_info_cleanup(int unit)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_PAYLOAD_OTPID_INFO(unit);

    /* Destory mutex */
    if (otpid->mutex) {
        sal_mutex_destroy(otpid->mutex);
        otpid->mutex = NULL;
    }

    /* Cleanup the resource of payload outer TPID */
    SHR_FREE(otpid->otpids);

    sal_memset(otpid, 0, sizeof(ltsw_vlan_otpid_info_t));

    return;
}

/*!
 * \brief Initialize the payload outer TPID information.
 *
 * This function is used to initialize the payload outer TPID information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_payload_otpid_info_init(int unit)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_PAYLOAD_OTPID_INFO(unit);
    int                     alloc_size  = 0;

    SHR_FUNC_ENTER(unit);

    /* Cleanup previous resource */
    if (otpid->initialized) {
        bcmi_ltsw_vlan_payload_otpid_detach(unit);
    }

    /* Create mutex */
    if (NULL == otpid->mutex) {
        otpid->mutex = sal_mutex_create("PAYLOAD OTPID mutex");
        SHR_NULL_CHECK(otpid->mutex , SHR_E_MEMORY);
    }

    /*
     * Allocate memory to cache VLAN_PAYLOAD_OUTER_TPID entries.
     */
    alloc_size = sizeof(ltsw_vlan_otpid_t) * VLAN_PAYLOAD_OTPID_MAX_NUM;
    SHR_ALLOC(otpid->otpids, alloc_size, "PAYLOAD OTPID Array");
    SHR_NULL_CHECK(otpid->otpids, SHR_E_MEMORY);
    sal_memset(otpid->otpids, 0, alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        vlan_payload_otpid_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate vlan port bitmap parameters.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN id.
 * \param [in]  pbmp          Port bitmap for members of VLAN.
 * \param [in]  ubmp          Untagged members of VLAN.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
vlan_port_params_validate(int unit, bcm_vlan_t vid,
                          bcm_pbmp_t pbmp, bcm_pbmp_t ubmp)
{
    bcm_pbmp_t vlan_pbmp, vlan_ubmp, pbm;
    SHR_FUNC_ENTER(unit);

    if (0 == SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check if pbmp and ubmp are valid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbm));
    BCM_PBMP_ASSIGN(vlan_pbmp, pbm);
    BCM_PBMP_ASSIGN(vlan_ubmp, pbm);
    BCM_PBMP_AND(vlan_pbmp, pbmp);
    BCM_PBMP_AND(vlan_ubmp, ubmp);
    if(!(BCM_PBMP_EQ(vlan_pbmp, pbmp) && BCM_PBMP_EQ(vlan_ubmp, ubmp))) {
         SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add member ports into VLAN.
 *
 * This function is used to add member ports into VLAN.
 *
 * \param [in] unit          Unit number.
 * \param [in] vid           VLAN id.
 * \param [in] ing_pbmp      Ingress member ports.
 * \param [in] egr_pbmp      Egress member ports.
 * \param [in] egr_ubmp      Egress untagged member ports.
 * \param [in] flags         BCM_VLAN_PORT_XXX flags.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
static int
vlan_port_add(int unit, bcm_vlan_t vid, bcm_pbmp_t ing_pbmp,
              bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp, int flags)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t stk_pbm, stack_cur, stack_inactive;

    SHR_FUNC_ENTER(unit);

    /* No such thing as untagged CPU. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_CPU, &pbm));
    BCM_PBMP_REMOVE(egr_ubmp, pbm);

    /* Remove internal loopback port.*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbm));
    BCM_PBMP_REMOVE(egr_pbmp, pbm);
    BCM_PBMP_REMOVE(ing_pbmp, pbm);
    BCM_PBMP_REMOVE(egr_ubmp, pbm);

    /*
     * Remove stack ports from egress untagged bitmap.
     * Remove inactive stack ports from ingress/egress bitmap.
     */
    if (ltsw_feature(unit, LTSW_FT_STK)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_pbmp_get(unit, &stack_cur, &stack_inactive,
                                    NULL));
        BCM_PBMP_ASSIGN(stk_pbm, stack_cur);
        BCM_PBMP_REMOVE(stk_pbm, stack_inactive);
        BCM_PBMP_REMOVE(egr_ubmp, stk_pbm);
        BCM_PBMP_REMOVE(ing_pbmp, stack_inactive);
        BCM_PBMP_REMOVE(egr_ubmp, stack_inactive);
        BCM_PBMP_REMOVE(egr_pbmp, stack_inactive);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_port_add(unit, vid, ing_pbmp,
                                 egr_pbmp, egr_ubmp, flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve member ports from VLAN.
 *
 * This function is used to retrieve member ports from VLAN.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 * \param [out]  pbmp     Port members.
 * \param [out]  ubmp     Untagged port members.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_port_get(int unit, bcm_vlan_t vid, bcm_pbmp_t *pbmp, bcm_pbmp_t *ubmp)
{
    bcm_pbmp_t ing_pbmp;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    if (0 == SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    BCM_PBMP_CLEAR(ing_pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_port_get(unit, vid, &ing_pbmp, pbmp, ubmp));

    BCM_PBMP_OR(*pbmp, ing_pbmp);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Main body of bcm_vlan_list() and bcm_vlan_list_by_pbmp().
 *         Assumes locking already done.
 * \param [in]   list_all    if TRUE, :lists all ports and ignores list_pbmp.
 * \param [in]   list_pbmp   if list_all is FALSE, lists only VLANs containing
 *                           any of the ports in list_pbmp.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_list(int unit, bcm_vlan_data_t **listp, int *countp,
          int list_all, bcm_pbmp_t list_pbmp)
{
    bcm_vlan_data_t     *list = NULL;
    int                 count, i = 0, idx, valid_count = 0;
    bcm_pbmp_t          pbmp, ubmp, tbmp;

    SHR_FUNC_ENTER(unit);
    *countp = 0;
    *listp = NULL;

    if (!list_all && BCM_PBMP_IS_NULL(list_pbmp)) {
        SHR_EXIT();
    }

    SHR_BITCOUNT_RANGE(VLAN_INFO(unit)->vlan_bmp, count, 0, BCM_VLAN_COUNT);

    if (count == 0) {
        SHR_EXIT();
    }

    SHR_ALLOC(list, count * sizeof (list[0]), "vlan_list");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);

    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
        if (0 == SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, idx)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_port_get(unit, idx, &pbmp, &ubmp));
        valid_count++;

        BCM_PBMP_ASSIGN(tbmp, list_pbmp);
        BCM_PBMP_AND(tbmp, pbmp);
        if (list_all || BCM_PBMP_NOT_NULL(tbmp)) {
            list[i].vlan_tag = idx;
            BCM_PBMP_ASSIGN(list[i].port_bitmap, pbmp);
            BCM_PBMP_ASSIGN(list[i].ut_port_bitmap, ubmp);
            i++;
        }
        if (valid_count == count)   {
            break;
        }
    }
    assert(!list_all || i == count);
    *countp = i;
    *listp = list;
    SHR_FUNC_EXIT();

exit:
    SHR_FREE(list);
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize VLAN module.
 *
 * This function is used to initialize VLAN module.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcm_ltsw_vlan_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize VLAN information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_info_init(unit));

    /* Dispatch for VLAN init */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_init(unit));

    VLAN_INFO(unit)->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add ports to the specified vlan.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 * \param [in]   pbmp     Port bitmap for members of VLAN.
 * \param [in]   ubmp     Untagged members of VLAN.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_port_add(int unit, bcm_vlan_t vid,
                       bcm_pbmp_t pbmp, bcm_pbmp_t ubmp)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_port_params_validate(unit, vid, pbmp, ubmp));

    VLAN_LOCK(unit);
   SHR_IF_ERR_CONT
        (vlan_port_add(unit, vid, pbmp, pbmp, ubmp, 0));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieves a list of the member ports of an existing VLAN.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 * \param [out]  pbmp     Port bitmap for members of VLAN.
 * \param [out]  ubmp     Untagged members of VLAN.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_port_get(int unit, bcm_vlan_t vid,
                       bcm_pbmp_t *pbmp, bcm_pbmp_t *ubmp)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_LOCK(unit);

    SHR_IF_ERR_CONT
        (vlan_port_get(unit, vid, pbmp, ubmp));

    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove ports from a specified vlan.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 * \param [in]   pbmp     port bitmap for ports to remove.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
 int
bcm_ltsw_vlan_port_remove(int unit, bcm_vlan_t vid, bcm_pbmp_t pbmp)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);
    if (0 == SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_port_remove(unit, vid, pbmp, pbmp, pbmp));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get per VLAN configuration.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   valid_fields    Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [out]   control        VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_control_vlan_selective_get(int unit, bcm_vlan_t vlan,
                                         uint32_t valid_fields,
                                         bcm_vlan_control_vlan_t *control)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(control, SHR_E_PARAM);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_control_vlan_get(unit, vlan, valid_fields, control));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set per VLAN configuration.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   valid_fields    Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]   control         VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_control_vlan_selective_set(int unit, bcm_vlan_t vlan,
                                         uint32_t valid_fields,
                                         bcm_vlan_control_vlan_t *control)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(control, SHR_E_PARAM);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_control_vlan_set(unit, vlan, valid_fields, control));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate and configure a VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_create(int unit, bcm_vlan_t vid)
{
    bcm_stg_t  def_stg = 0;
    int vlan_locked = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);
    if (SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid)) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    VLAN_LOCK(unit);
    vlan_locked = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_create(unit, vid));

    SHR_BITSET(VLAN_INFO(unit)->vlan_bmp, vid);
    SHR_BITCLR(VLAN_INFO(unit)->pre_cfg_vlan_bmp, vid);

    /* Add stack ports to VLAN if VLAN is auto_stacked.*/
    if (bcmint_vlan_info_auto_stack_get(unit)) {
        bcm_pbmp_t stacked, empty_pbmp;

        BCM_PBMP_CLEAR(empty_pbmp);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_pbmp_get(unit, &stacked, NULL, NULL));
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_port_add(unit, vid, stacked, stacked, empty_pbmp, 0));
    }

    VLAN_UNLOCK(unit);
    vlan_locked = 0;

    /* Add the vlan into a deafault STG.*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stg_default_get(unit, &def_stg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stg_vlan_add(unit, def_stg, vid));

exit:
    if (vlan_locked) {
        VLAN_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deallocate VLAN from Switch.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_destroy(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);
    if (vid == *(VLAN_INFO(unit)->def_vlan)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }
    if (0 == SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Remove VLAN from STG. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_vlan_destroy(unit, vid));

    /* Destroy VLAN. */
    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_destroy(unit, vid));
    if (SHR_FUNC_ERR()) {
        VLAN_UNLOCK(unit);
        SHR_EXIT();
    }

    SHR_BITCLR(VLAN_INFO(unit)->vlan_bmp, vid);
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all VLANs except the default VLAN
 *
 * \param [in]   unit            Unit number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_destroy_all(int unit)
{
    bcm_vlan_t vid;
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    for (vid = BCM_VLAN_MIN; vid < BCM_VLAN_COUNT; vid++) {
        if ((vid != *(VLAN_INFO(unit)->def_vlan)) &&
            (SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid))) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_vlan_destroy(unit, vid));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Returns an array of all defined VLANs and their port bitmaps.
 *
 * \param [in]   unit     Unit number.
 * \param [out]  listp    Place where pointer to return array will be stored,
 *                        which will be NULL if there are zero VLANs defined.
 * \param [out]  countp   Place where num of entries in array will be stored,
 *                        which will be 0 if there are zero VLANs defined.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp)
{
    bcm_pbmp_t      empty_pbm;
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    BCM_PBMP_CLEAR(empty_pbm);
    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (vlan_list(unit, listp, countp, TRUE, empty_pbm));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a list returned by bcm_vlan_list.
 *        Also works for the zero-VLAN case (NULL list).
 *
 * \param [in]   unit     Unit number.
 * \param [in]   list     List returned by bcm_vlan_list.
 * \param [in]   count    Count returned by bcm_vlan_list.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_list_destroy(int unit, bcm_vlan_data_t *list, int count)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
        SHR_FREE(list);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Returns an array of defined VLANs and port bitmaps.
 *        Only VLANs that containing any of the specified ports are listed.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   pbmp     Bitmap of ports.
 * \param [out]  listp    Place where pointer to return array will be stored,
 *                        which will be NULL if there are zero VLANs defined.
 * \param [out]  countp   Place where num of entries in array will be stored,
 *                        which will be 0 if there are zero VLANs defined.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_list_by_pbmp(int unit, bcm_pbmp_t pbmp,
                           bcm_vlan_data_t **listp, int *countp)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (vlan_list(unit, listp, countp, FALSE, pbmp));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get per VLAN block configuration.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 * \param [out]  block    VLAN specific traffic blocking structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);
    SHR_NULL_CHECK(block, SHR_E_PARAM);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_block_get(unit, vid, block));
    VLAN_UNLOCK(unit);
exit:
     SHR_FUNC_EXIT();
}

 /*!
 * \brief Set per VLAN block configuration.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 * \param [in]   block    VLAN specific traffic blocking structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);
    SHR_NULL_CHECK(block, SHR_E_PARAM);
    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_block_set(unit, vid, block));
    VLAN_UNLOCK(unit);
exit:
     SHR_FUNC_EXIT();
}

/*!
 * \brief Set a one or more VLAN control vlan flags on a vlan_vector.
 *
 * \param [in]   unit             Unit number.
 * \param [in]   vlan_vector      Vlan vector.
 * \param [in]   flags_mask       Valid fields bitmask for vlan control.
 * \param [in]   flags_value      Flags value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_vector_flags_set(int unit, bcm_vlan_vector_t vlan_vector,
                               uint32_t flags_mask, uint32_t flags_value)
{
    bcm_vlan_t              vid;
    bcm_vlan_control_vlan_t control;
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    for (vid = BCM_VLAN_MIN + 1; vid < BCM_VLAN_MAX; vid++) {
        if (BCM_VLAN_VEC_GET(vlan_vector, vid)) {
            sal_memset(&control, 0, sizeof(bcm_vlan_control_vlan_t));
            if (SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid) ||
                SHR_BITGET(VLAN_INFO(unit)->pre_cfg_vlan_bmp, vid)) {
                SHR_IF_ERR_CONT
                    (mbcm_ltsw_vlan_control_vlan_get(unit, vid,
                         BCM_VLAN_CONTROL_VLAN_ALL_MASK, &control));
                if (SHR_FUNC_ERR()) {
                    break;
                }
            } else {
                continue;
            }
            control.flags = (~flags_mask & control.flags) |
                            (flags_mask & flags_value);
            SHR_IF_ERR_CONT
                (mbcm_ltsw_vlan_control_vlan_set(unit, vid,
                                  BCM_VLAN_CONTROL_VLAN_ALL_MASK, &control));
            if (SHR_FUNC_ERR()) {
                break;
            }
        }
    }
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get miscellaneous port-specific VLAN options.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   port     Port ID.
 * \param [in]   type     VLAN port control type.
 * \param [out]  arg      Type value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_control_port_get(int unit, bcm_port_t port,
                               bcm_vlan_control_port_t type, int * arg)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_control_port_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Set miscellaneous port-specific VLAN options.
*
* \param [in]   unit     Unit number.
* \param [in]   port     Port ID.
* \param [in]   type     VLAN port control type.
* \param [in]   arg      Type value.
*
* \retval SHR_E_NONE     No errors.
* \retval !SHR_E_NONE    Failure.
*/
int
bcm_ltsw_vlan_control_port_set(int unit, bcm_port_t port,
                               bcm_vlan_control_port_t type, int arg)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_control_port_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get miscellaneous VLAN-specific chip options.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   type     A value from bcm_vlan_control_t enumeration list.
 * \param [out]  arg      state whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_control_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set miscellaneous VLAN-specific chip options.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   type     A value from bcm_vlan_control_t enumeration list.
 * \param [in]   arg      state whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
 int
bcm_ltsw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_control_set(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get per VLAN configuration.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 * \param [out]  control         VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                               bcm_vlan_control_vlan_t *control)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(control, SHR_E_PARAM);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_control_vlan_get(unit, vid,
                                         BCM_VLAN_CONTROL_VLAN_ALL_MASK,
                                         control));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set per VLAN configuration.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 * \param [in]   control         VLAN control structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
/* coverity[pass_by_value] */
                               bcm_vlan_control_vlan_t control)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_control_vlan_set(unit, vid,
                                         BCM_VLAN_CONTROL_VLAN_ALL_MASK,
                                         &control));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the default VLAN ID.
 *
 * \param [in]   unit            Unit number.
 * \param [out]  vid_ptr         Target to receive the VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_default_get(int unit, bcm_vlan_t *vid_ptr)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(vid_ptr, SHR_E_PARAM);

    *vid_ptr = *(VLAN_INFO(unit)->def_vlan);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the default VLAN ID.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             The new default VLAN.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_vlan_default_set(int unit, bcm_vlan_t vid)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);

    VLAN_LOCK(unit);
    if (!SHR_BITGET(vi->vlan_bmp, vid)) {
        VLAN_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    *(vi->def_vlan) = vid;
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a Gport to the specified vlan. Adds WLAN/VLAN/NIV/Extender ports to
 *        broadcast, multicast and unknown unicast groups.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [in]   flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, int flags)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_gport_add(unit, vlan, port, flags));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Gport from the specified vlan.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_gport_delete(int unit, bcm_vlan_t vlan, bcm_gport_t port)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_gport_delete(unit, vlan, port,
                                     BCM_VLAN_GPORT_ADD_UNTAGGED));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all Gports from the specified vlan.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_gport_delete_all(unit, vlan));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Gport from the specified vlan with flag.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [out] flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_gport_extended_delete(int unit, bcm_vlan_t vlan,
                                    bcm_gport_t port, int flags)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_gport_delete(unit, vlan, port, flags));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a virtual or physical port from the specified VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [out]  flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_gport_get(int unit, bcm_vlan_t vlan,
                        bcm_gport_t port, int *flags)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_gport_get(unit, vlan, port, flags));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all virtual and physical ports from the specified VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   array_max       Max array size.
 * \param [out]  gport_array     Gport id array.
 * \param [out]  flags_array     BCM_VLAN_PORT_XXX array.
 * \param [out]  array_size      Output array size.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
                            bcm_gport_t *gport_array, int *flags_array,
                            int* array_size)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);
    if ((NULL == array_size) || (array_max <= 0)){
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_gport_get_all(unit, vlan, array_max,
                                      gport_array, flags_array, array_size));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the port's default vlan tag actions.
 *
 * \param [in]      unit       Unit number.
 * \param [in]      port       Port number.
 * \param [in]      action     Vlan tag actions.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_port_default_action_set(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_port_action_set(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the port's default vlan tag actions.
 *
 * \param [in]      unit       Unit number.
 * \param [in]      port       Port number.
 * \param [out]     action     Vlan tag actions.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_port_default_action_get(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_port_action_get(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the port's default vlan tag actions.
 *
 * \param [in]      unit       Unit number.
 * \param [in]      port       Port number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_port_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the port's default egr vlan tag actions.
 *
 * \param [in]      unit       Unit number.
 * \param [in]      port       Port number.
 * \param [in]      action     Vlan tag actions.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                             bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_egr_vlan_port_action_set(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the port's default egr vlan tag actions.
 *
 * \param [in]      unit       Unit number.
 * \param [in]      port       Port number.
 * \param [out]     action     Vlan tag actions.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                             bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_egr_vlan_port_action_get(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the port's default egr vlan tag actions.
 *
 * \param [in]      unit       Unit number.
 * \param [in]      port       Port number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_egr_vlan_port_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Valicate vlan stat parameters.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      vlan             VLAN Id.
 * \param [in]      cos              COS or priority.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
vlan_stat_params_validate(int unit, bcm_vlan_t vlan, bcm_cos_t cos)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, vlan)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Only MPLS VPN supported. */
    if (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(vlan)) {
        uint32_t vfi;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_idx_get(unit, vlan, &vfi));
        if(!bcmi_ltsw_virtual_vfi_used_get(unit, vfi,
                                           BCMI_LTSW_VFI_TYPE_MPLS)){
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (cos != BCM_COS_INVALID) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given vlan.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      vlan             VLAN Id.
 * \param [in]      stat_counter_id  Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_stat_attach(int unit, bcm_vlan_t vlan, uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info = {0};

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_stat_params_validate(unit, vlan, BCM_COS_INVALID));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               stat_counter_id,
                                               &counter_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_stat_attach(unit, vlan, &counter_info));

    if (SHR_FUNC_ERR()) {
        (void)
        bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                          stat_counter_id);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries to given vlan with a given stat counter id.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      vlan             VLAN Id.
 * \param [in]      stat_counter_id  Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_stat_detach_with_id(int unit, bcm_vlan_t vlan,
                                  uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info = {0};

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_stat_params_validate(unit, vlan, BCM_COS_INVALID));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               stat_counter_id,
                                               &counter_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_stat_detach(unit, vlan, &counter_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           stat_counter_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id for given VLAN id and stat type.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      vlan             VLAN Id.
 * \param [in]      stat             Stat type.
 * \param [out]     stat_counter_id  Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_stat_id_get(int unit, bcm_vlan_t vlan, bcm_vlan_stat_t stat,
                          uint32 *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t info[2];
    uint32_t num_info;
    int i;

    SHR_FUNC_ENTER(unit);

    i = ((stat == bcmVlanStatIngressPackets) ||
         (stat == bcmVlanStatIngressBytes)) ? 0 : 1;

    sal_memset(info, 0x0, sizeof(info));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_stat_get(unit, vlan, info, &num_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &info[i], stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the VTABLE STG for the specified vlan.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      vid              VLAN Id.
 * \param [out]     stg_ptr          Pointer to store stgid for return.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(stg_ptr, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_vlan_get(unit, vid, stg_ptr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the VTABLE STG for the specified vlan.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      vid              VLAN Id.
 * \param [in]      stg              New spanning tree group number for VLAN.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);

    SHR_IF_ERR_CONT
        (bcm_ltsw_stg_vlan_add(unit, stg, vid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STP state for a port in STG containing the specified VLAN or VPN.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID or VPN ID.
 * \param [in] port      Port Number.
 * \param [out]stp_state Spanning Tree State of port.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_vlan_stp_get(int unit, bcm_vlan_t vid, bcm_port_t port, int *stp_state)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(stp_state, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_vlan_stp_get(unit, vid, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STP state for a port in STG containing the specified VLAN or VPN.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID or VPN ID.
 * \param [in] port      Port Number.
 * \param [in] stp_state Spanning Tree State of port.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_vlan_stp_set(int unit, bcm_vlan_t vid, bcm_port_t port, int stp_state)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_vlan_stp_set(unit, vid, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry to ingress VLAN translation table.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      key_type         Key Type : bcmVlanTranslateKey*.
 * \param [in]      outer_vlan       Packet outer VLAN ID.
 * \param [in]      inner_vlan       Packet inner VLAN ID.
 * \param [in]      action           Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_add(int unit,
                                   bcm_gport_t port,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_action_add(unit, key_type, port,
                                             outer_vlan, inner_vlan, action));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry to ingress VLAN translation table.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      key_type         Key Type : bcmVlanTranslateKey*.
 * \param [in]      outer_vlan       Packet outer VLAN ID.
 * \param [in]      inner_vlan       Packet inner VLAN ID.
 * \param [out]     action           Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_get(int unit, bcm_gport_t port,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_action_get(unit, key_type, port,
                                             outer_vlan, inner_vlan, action));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a vlan translate lookup entry.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      key_type         Key Type : bcmVlanTranslateKey*.
 * \param [in]      outer_vlan       Packet outer VLAN ID.
 * \param [in]      inner_vlan       Packet inner VLAN ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_delete(int unit,
                                      bcm_gport_t port,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_action_delete(unit, key_type, port,
                                                outer_vlan, inner_vlan));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all translate entries
 *        and call given callback with action structure.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      cb               Call back function.
 * \param [in]      user_data        User provided data to pass to a call back.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_traverse(
    int unit,
    bcm_vlan_translate_action_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_action_traverse(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Valicate vlan range vlan parameters.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
vlan_range_validate(int unit,
                    bcm_vlan_t outer_vlan_low, bcm_vlan_t outer_vlan_high,
                    bcm_vlan_t inner_vlan_low, bcm_vlan_t inner_vlan_high)
{
    SHR_FUNC_ENTER(unit);

    /* BCM_VLAN_INVALID is valid input. */
    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        VLAN_ID_CHECK(inner_vlan_low);
        VLAN_ID_CHECK(inner_vlan_high);
        VLAN_ID_CHECK(outer_vlan_low);
        VLAN_ID_CHECK(outer_vlan_high);
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        VLAN_ID_CHECK(outer_vlan_low);
        VLAN_ID_CHECK(outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        VLAN_ID_CHECK(inner_vlan_low);
        VLAN_ID_CHECK(inner_vlan_high);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry to the VLAN Translation table, which assigns
 *        VLAN actions for packets matching within the VLAN range(s).
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 * \param [in]      action           Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_range_add(int unit, bcm_gport_t port,
                                         bcm_vlan_t outer_vlan_low,
                                         bcm_vlan_t outer_vlan_high,
                                         bcm_vlan_t inner_vlan_low,
                                         bcm_vlan_t inner_vlan_high,
                                         bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);
    VLAN_INIT_CHECK(unit);

    if (port == BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_validate(unit, outer_vlan_low, outer_vlan_high,
                             inner_vlan_low, inner_vlan_high));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_translate_action_range_add(unit, port,
                                    outer_vlan_low, outer_vlan_high,
                                    inner_vlan_low, inner_vlan_high, action));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from the VLAN Translation table
 *        for the specified VLAN range(s).
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_range_delete(int unit, bcm_gport_t port,
                                            bcm_vlan_t outer_vlan_low,
                                            bcm_vlan_t outer_vlan_high,
                                            bcm_vlan_t inner_vlan_low,
                                            bcm_vlan_t inner_vlan_high)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    if (port == BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_validate(unit, outer_vlan_low, outer_vlan_high,
                             inner_vlan_low, inner_vlan_high));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_translate_action_range_delete(unit, port,
                                     outer_vlan_low, outer_vlan_high,
                                     inner_vlan_low, inner_vlan_high));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all VLAN range entries from the VLAN Translation table.
 *
 * \param [in]      unit             Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_range_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_translate_action_range_delete_all(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverses over VLAN Translation table and call provided callback
 *        with valid entries.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      cb               Call back function.
 * \param [in]      user_data        Pointer to user specific data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_action_range_traverse(
     int                                         unit,
     bcm_vlan_translate_action_range_traverse_cb cb,
     void                                        *user_data)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_translate_action_range_traverse(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry to egress VLAN translation table.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port_class       Group ID of ingress port.
 * \param [in]      outer_vlan       Packet outer VLAN ID.
 * \param [in]      inner_vlan       Packet inner VLAN ID.
 * \param [in]      action           Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_egress_action_add(int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_action_add(unit, port_class,
                                             outer_vlan, inner_vlan, action));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry to egress VLAN translation table.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port_class       Group ID of ingress port.
 * \param [in]      outer_vlan       Packet outer VLAN ID.
 * \param [in]      inner_vlan       Packet inner VLAN ID.
 * \param [out]     action           Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_egress_action_get(int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_action_get(unit, port_class,
                                             outer_vlan, inner_vlan, action));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an egress vlan translate lookup entry..
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port_class       Group ID of ingress port.
 * \param [in]      outer_vlan       Packet outer VLAN ID.
 * \param [in]      inner_vlan       Packet inner VLAN ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_egress_action_delete(int unit, int port_class,
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_action_delete(unit, port_class,
                                                outer_vlan, inner_vlan));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all translate entries and call given callback with
 *        action structure.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      cb               Call back function.
 * \param [in]      user_data        User provided data to pass to a call back.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_vlan_translate_egress_action_traverse(
    int unit,
    bcm_vlan_translate_egress_action_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_action_traverse(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get VLAN mcast flood mode.
 *
 * This function is used to get vlan flood mode.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] mode             VLAN flood mode.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_mcast_flood_get(int unit, bcm_vlan_t vid,
                              bcm_vlan_mcast_flood_t *mode)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_mcast_flood_get(unit, vid, mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set VLAN mcast flood mode.
 *
 * This function is used to set vlan flood mode.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] mode              VLAN flood mode.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_mcast_flood_set(int unit, bcm_vlan_t vid,
                              bcm_vlan_mcast_flood_t mode)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_mcast_flood_set(unit, vid, mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a VLAN cross connect entry.
 *
 * This function is used to add a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] port_1            First port in the cross-connect.
 * \param [in] port_2            Second port in the cross-connect.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_cross_connect_add(int unit,
                                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                                bcm_gport_t port_1, bcm_gport_t port_2)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(outer_vlan);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_cross_connect_add(unit,
                                          outer_vlan, inner_vlan,
                                          port_1, port_2));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Delete a VLAN cross connect entry.
*
* This function is used to delete a VLAN cross connect entry.
*
* \param [in] unit              Unit number.
* \param [in] outer_vlan        Outer VLAN ID.
* \param [in] inner_vlan        Inner VLAN ID.
*
* \retval SHR_E_NONE            No errors.
* \retval !SHR_E_NONE           Failure.
*/
int
bcm_ltsw_vlan_cross_connect_delete(int unit,
                                   bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(outer_vlan);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_cross_connect_delete(unit,
                                             outer_vlan, inner_vlan));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Delete all VLAN cross connect entries.
*
* This function is used to delete all VLAN cross connect entries.
*
* \param [in] unit              Unit number.
*
* \retval SHR_E_NONE            No errors.
* \retval !SHR_E_NONE           Failure.
*/
int
bcm_ltsw_vlan_cross_connect_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_cross_connect_delete_all(unit));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Traverse VLAN cross connect entries.
*
* This function is used to traverse VLAN cross connect entries.
*
* \param [in] unit              Unit number.
* \param [in] cb                Callback function.
* \param [in] user_data         User data to be passed to callback function.
*
* \retval SHR_E_NONE            No errors.
* \retval !SHR_E_NONE           Failure.
*/
int
bcm_ltsw_vlan_cross_connect_traverse(int unit,
                                     bcm_vlan_cross_connect_traverse_cb cb,
                                     void *user_data)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_cross_connect_traverse(unit, cb, user_data));
    VLAN_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex-counter into a given ingress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_flexctr_attach(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_flexctr_attach(unit, key_type, port,
                                                 outer_vlan, inner_vlan,
                                                 config));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach flex-counter from a given ingress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_flexctr_detach(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_flexctr_detach(unit, key_type, port,
                                                 outer_vlan, inner_vlan,
                                                 config));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach all flex-counter from a given ingress vlan translation table.
 *
 * This function is used to detach all count IDs and object values from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_flexctr_detach_all(int unit,
                                           bcm_gport_t port,
                                           bcm_vlan_translate_key_t key_type,
                                           bcm_vlan_t outer_vlan,
                                           bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_flexctr_detach(unit, key_type, port,
                                                 outer_vlan, inner_vlan,
                                                 NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex-counter from a given ingress vlan translation table.
 *
 * This function is used to get the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_flexctr_get(int unit,
                                    bcm_gport_t port,
                                    bcm_vlan_translate_key_t key_type,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    int array_size,
                                    bcm_vlan_flexctr_config_t *config_array,
                                    int *count)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    if ((count == NULL) ||
        (array_size < 0) ||
        ((array_size > 0) && (config_array == NULL))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_flexctr_get(unit, key_type, port,
                                              outer_vlan, inner_vlan,
                                              array_size, config_array, count));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex-counter into a given egress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port_class        Port class.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_egress_flexctr_attach(int unit,
                                              int port_class,
                                              bcm_vlan_t outer_vlan,
                                              bcm_vlan_t inner_vlan,
                                              bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_flexctr_attach(unit, port_class,
                                                 outer_vlan, inner_vlan,
                                                 config));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach flex-counter from a given egress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port_class        Port class.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_egress_flexctr_detach(int unit,
                                              int port_class,
                                              bcm_vlan_t outer_vlan,
                                              bcm_vlan_t inner_vlan,
                                              bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_flexctr_detach(unit, port_class,
                                                 outer_vlan, inner_vlan,
                                                 config));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach all flex-counter from a given egress vlan translation table.
 *
 * This function is used to detach all count IDs and object values from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port_class        Port class.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_egress_flexctr_detach_all(int unit,
                                                  int port_class,
                                                  bcm_vlan_t outer_vlan,
                                                  bcm_vlan_t inner_vlan)

{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_flexctr_detach(unit, port_class,
                                                 outer_vlan, inner_vlan,
                                                 NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex-counter from a given egress vlan translation table.
 *
 * This function is used to get the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port_class        Port class.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_vlan_translate_egress_flexctr_get(int unit,
                                           int port_class,
                                           bcm_vlan_t outer_vlan,
                                           bcm_vlan_t inner_vlan,
                                           int array_size,
                                           bcm_vlan_flexctr_config_t *config_array,
                                           int *count)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    if ((count == NULL) ||
        (array_size < 0) ||
        ((array_size > 0) && (config_array == NULL))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_xlate_flexctr_get(unit, port_class,
                                              outer_vlan, inner_vlan,
                                              array_size, config_array, count));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the VLAN module.
 *
 * This function is used to de-initialize the VLAN module.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (VLAN_INFO(unit)->initialized == FALSE) {
        SHR_EXIT();
    }

    VLAN_LOCK(unit);

    VLAN_INFO(unit)->initialized = FALSE;

    /* Dispatch for VLAN detach */
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_detach(unit));

    VLAN_UNLOCK(unit);

    /* Cleanup the VLAN information. */
    vlan_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the VLAN outer TPID information.
 *
 * This function is used to initialize the VLAN information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_otpid_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize VLAN outer information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_otpid_info_init(unit));

    /* Dispatch for VLAN outer TPID init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_otpid_init(unit));

    VLAN_OTPID_INFO(unit)->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize VLAN outer TPID information.
 *
 * This function is used to de-initialize VLAN outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_otpid_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (VLAN_OTPID_INFO(unit)->initialized == FALSE) {
        SHR_EXIT();
    }

    VLAN_OTPID_LOCK(unit);

    VLAN_OTPID_INFO(unit)->initialized = FALSE;

    /* Dispatch for VLAN outer TPID detach. */
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_otpid_detach(unit));

    VLAN_OTPID_UNLOCK(unit);

    /* Cleanup the VLAN outer TPID information. */
    vlan_otpid_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get dafault TPID.
 *
 * This function is used to get dafault TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Default tpid.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_default_get(int unit, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    *tpid = 0x8100;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a new outer TPID entry.
 *
 * This function is used to add a new outer TPID entry. If a same TPID
 * already exists, simplely increase the reference count of the cached entry.
 * Otherwise, add the entry to the cached table and write the new entry into LT.
 * Only four distinct TPID values are currently supported.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be added.
 * \param [out] index                Index where the the new TPID is added.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_entry_add(int unit, uint16_t tpid, int *index)
{
    ltsw_vlan_otpid_info_t *otpid     = VLAN_OTPID_INFO(unit);
    int                    i          = 0;
    int                    free_index = 0;

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_OTPID_LOCK(unit);

    /* Search for an existing entry.*/
    free_index = -1;
    for (i = 0; i < VLAN_OTPID_MAX_NUM; i++) {
        if (VLAN_OTPID_ENTRY(unit, i) == tpid) {
            VLAN_OTPID_REF_COUNT(unit, i)++;
            *index = i;
            SHR_EXIT();
        }
        if (VLAN_OTPID_REF_COUNT(unit, i) == 0) {
            free_index = i;
        }
    }

    if (free_index < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    /* Dispatch for VLAN add if a free entry is available */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_otpid_add(unit, free_index, tpid));

    *index = free_index;

exit:
    if (!SHR_FUNC_VAL_IS(SHR_E_INIT)) {
        VLAN_OTPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get outer TPID value for a specified entry index.
 *
 * This function is used to get outer TPID value for a specified entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Index where the the outer TPID is gotten.
 * \param [out] tpid                 Outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_entry_get(int unit, int index, uint16_t *tpid)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if ((index < 0) || (index >= VLAN_OTPID_MAX_NUM) ||
        (VLAN_OTPID_REF_COUNT(unit, index) <= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *tpid = VLAN_OTPID_ENTRY(unit, index);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete outer TPID entry by index.
 *
 * This function is used to delete outer TPID entry by index. For legacy
 * behavior,  LT/HW TPID enties are not touched during the deletion.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                TPID entry Index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_entry_delete(int unit, int index)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_OTPID_LOCK(unit);

    if ((index < 0) || (index >= VLAN_OTPID_MAX_NUM) ||
        (VLAN_OTPID_REF_COUNT(unit, index) <= 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    VLAN_OTPID_REF_COUNT(unit, index)--;

exit:
    if (!SHR_FUNC_VAL_IS(SHR_E_INIT)) {
        VLAN_OTPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get outer entry index for a specified TPID value.
 *
 * This function is used to get outer entry index for a specified TPID value.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Outer TPID.
 * \param [out] index                Outer TPID entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_entry_lookup(int unit, uint16_t tpid, int *index)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);
    int                    i      = 0;

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    for (i = 0; i < VLAN_OTPID_MAX_NUM; i++) {
        if (VLAN_OTPID_ENTRY(unit, i) == tpid) {
            *index = i;
            break;
        }
    }

    if (i == VLAN_OTPID_MAX_NUM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add reference count of outer TPID.
 *
 * This function is used to add reference count of outer TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Reference count depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_entry_ref_count_add(int unit, int index, int count)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_OTPID_LOCK(unit);

    if ((index < 0) || (index >= VLAN_OTPID_MAX_NUM) ||
        ((VLAN_OTPID_REF_COUNT(unit, index) + count) < 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    VLAN_OTPID_REF_COUNT(unit, index) += count;

exit:
    if (!SHR_FUNC_VAL_IS(SHR_E_INIT)) {
        VLAN_OTPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lock outer VLAN TPID table.
 *
 * This function is used to lock outer VLAN TPID table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_lock(int unit)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_OTPID_LOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Unlock outer VLAN TPID table.
 *
 * This function is used to unlock outer VLAN TPID table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_otpid_unlock(int unit)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_OTPID_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the payload outer TPID information.
 *
 * This function is used to initialize the payload outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize payload outer TPID information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_payload_otpid_info_init(unit));

    /* Dispatch for payload outer TPID init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_payload_otpid_init(unit));

    VLAN_PAYLOAD_OTPID_INFO(unit)->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize payload outer TPID information.
 *
 * This function is used to de-initialize payload outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (VLAN_PAYLOAD_OTPID_INFO(unit)->initialized == FALSE) {
        SHR_EXIT();
    }

    VLAN_PAYLOAD_OTPID_LOCK(unit);

    VLAN_PAYLOAD_OTPID_INFO(unit)->initialized = FALSE;

    /* Dispatch for payload outer TPID detach. */
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_payload_otpid_detach(unit));

    VLAN_PAYLOAD_OTPID_UNLOCK(unit);

    /* Cleanup the payload outer TPID information. */
    vlan_payload_otpid_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get dafault payload outer TPID.
 *
 * This function is used to get dafault payload outer TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Default tpid.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_default_get(int unit, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    *tpid = 0x8100;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a new payload outer TPID entry.
 *
 * This function is used to add a new payload outer TPID entry. If a same TPID
 * already exists, simplely increase the reference count of the cached entry.
 * Otherwise, add the entry to the cached table and write the new entry into LT.
 * Only four distinct TPID values are currently supported.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be added.
 * \param [out] index                Index where the the new TPID is added.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_entry_add(int unit, uint16_t tpid, int *index)
{
    ltsw_vlan_otpid_info_t *otpid     = VLAN_PAYLOAD_OTPID_INFO(unit);
    int                    i          = 0;
    int                    free_index = 0;

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_PAYLOAD_OTPID_LOCK(unit);

    /* Search for an existing entry.*/
    free_index = -1;
    for (i = 0; i < VLAN_PAYLOAD_OTPID_MAX_NUM; i++) {
        if (VLAN_PAYLOAD_OTPID_ENTRY(unit, i) == tpid) {
            VLAN_PAYLOAD_OTPID_REF_COUNT(unit, i)++;
            *index = i;
            SHR_EXIT();
        }
        if (VLAN_PAYLOAD_OTPID_REF_COUNT(unit, i) == 0) {
            free_index = i;
        }
    }

    if (free_index < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    /* Dispatch for payload outer TPID add if a free entry is available */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_payload_otpid_add(unit, free_index, tpid));

    *index = free_index;

exit:
    if (!SHR_FUNC_VAL_IS(SHR_E_INIT)) {
        VLAN_PAYLOAD_OTPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get payload outer TPID value for a specified entry index.
 *
 * This function is used to get payload outer TPID value for a specified entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Index where the the payload outer TPID is gotten.
 * \param [out] tpid                 Payload outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_entry_get(int unit, int index, uint16_t *tpid)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_PAYLOAD_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if ((index < 0) || (index >= VLAN_PAYLOAD_OTPID_MAX_NUM) ||
        (VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index) <= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *tpid = VLAN_PAYLOAD_OTPID_ENTRY(unit, index);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete outer TPID entry by index.
 *
 * This function is used to delete payload outer TPID entry by index. For legacy
 * behavior,  LT/HW TPID enties are not touched during the deletion.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                TPID entry Index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_entry_delete(int unit, int index)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_PAYLOAD_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_PAYLOAD_OTPID_LOCK(unit);

    if ((index < 0) || (index >= VLAN_PAYLOAD_OTPID_MAX_NUM) ||
        (VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index) <= 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index)--;

exit:
    if (!SHR_FUNC_VAL_IS(SHR_E_INIT)) {
        VLAN_PAYLOAD_OTPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get out the entry index for a specified payload outer TPID value.
 *
 * This function is used to get outer entry index for a specified payload outer TPID value.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Payload outer TPID.
 * \param [out] index              Payload outer TPID entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_entry_lookup(int unit, uint16_t tpid, int *index)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_PAYLOAD_OTPID_INFO(unit);
    int                    i      = 0;

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    for (i = 0; i < VLAN_PAYLOAD_OTPID_MAX_NUM; i++) {
        if (VLAN_PAYLOAD_OTPID_ENTRY(unit, i) == tpid) {
            *index = i;
            break;
        }
    }

    if (i == VLAN_PAYLOAD_OTPID_MAX_NUM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add reference count of payload outer TPID.
 *
 * This function is used to add reference count of payload outer TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Reference count depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_payload_otpid_entry_ref_count_add(int unit, int index, int count)
{
    ltsw_vlan_otpid_info_t *otpid = VLAN_PAYLOAD_OTPID_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!otpid->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_PAYLOAD_OTPID_LOCK(unit);

    if ((index < 0) || (index >= VLAN_PAYLOAD_OTPID_MAX_NUM) ||
        ((VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index) + count) < 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index) += count;

exit:
    if (!SHR_FUNC_VAL_IS(SHR_E_INIT)) {
        VLAN_PAYLOAD_OTPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize VLAN inner TPID information.
 *
 * This function is used to de-initialize VLAN inner TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_itpid_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (VLAN_ITPID_INFO(unit)->initialized == FALSE) {
        SHR_EXIT();
    }

    VLAN_ITPID_LOCK(unit);
    VLAN_ITPID_INFO(unit)->initialized = FALSE;
    VLAN_ITPID_UNLOCK(unit);

    /* Cleanup the VLAN inner TPID information. */
    vlan_itpid_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief initialize the VLAN inner TPID information.
 *
 * This function is used to initialize the VLAN information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_vlan_itpid_init(int unit)
{
    ltsw_vlan_itpid_info_t *itpid = VLAN_ITPID_INFO(unit);
    SHR_FUNC_ENTER(unit);

    /* Cleanup previous resource */
    if (itpid->initialized) {
        bcmi_ltsw_vlan_itpid_detach(unit);
    }

    /* Create mutex */
    if (NULL == itpid->mutex) {
        itpid->mutex = sal_mutex_create("VLAN ITPID mutex");
        SHR_NULL_CHECK(itpid->mutex, SHR_E_MEMORY);
    }

    itpid->itpid = 0;
    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_vlan_itpid_get(unit, &(itpid->itpid)));
    }
    itpid->initialized = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        vlan_itpid_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set global inner TPID.
 *
 * This function is used to set global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_itpid_set(int unit, uint16_t tpid)
{
    int locked = 0;
    SHR_FUNC_ENTER(unit);

    if (!VLAN_ITPID_INFO(unit)->initialized) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    VLAN_ITPID_LOCK(unit);
    locked = 1;

    if (tpid == VLAN_ITPID_INFO(unit)->itpid) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_itpid_set(unit, tpid));

    VLAN_ITPID_INFO(unit)->itpid = tpid;
exit:
    if (locked) {
        VLAN_ITPID_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get global inner TPID.
 *
 * This function is used to get global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] tpid                 Inner TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_itpid_get(int unit, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    if (VLAN_ITPID_INFO(unit)->initialized) {
        *tpid = VLAN_ITPID_INFO(unit)->itpid;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set vlan default flood mode.
 *
 * This function is used to set vlan create default flood mode.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  mode                 VLAN multicast flood mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_flood_default_set(int unit, bcm_vlan_mcast_flood_t mode)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    VLAN_LOCK(unit);

    *(vi->flood_mode) = (uint8_t)mode;

    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get vlan default flood mode.
 *
 * This function is used to get vlan create default flood mode.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] mode                 VLAN multicast flood mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_flood_default_get(int unit, bcm_vlan_mcast_flood_t *mode)
{
    ltsw_vlan_info_t  *vi = VLAN_INFO(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    *mode = (bcm_vlan_mcast_flood_t)*(vi->flood_mode);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry to ingress VLAN translation table.
 *
 * This function is used to add an entry to ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_xlate_add(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_add(unit, key_type, port,
                                      outer_vlan, inner_vlan, cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update an entry in ingress VLAN translation table.
 *
 * This function is used to update an entry in ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_xlate_update(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_update(unit, key_type, port,
                                         outer_vlan, inner_vlan, cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from ingress VLAN translation table.
 *
 * This function is used to get an entry from ingress VLAN translation table.
 * If fld_bmp is not specified (zero), all fields will be retrieved.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] cfg              Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_xlate_get(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_get(unit, key_type, port,
                                      outer_vlan, inner_vlan, cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from ingress VLAN translation table.
 *
 * This function is used to delete an entry from ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_xlate_delete(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_delete(unit, key_type, port,
                                         outer_vlan, inner_vlan));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_xlate_traverse(int unit,
                                  bcmi_ltsw_vlan_ing_xlate_traverse_cb cb,
                                  void *user_data)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_xlate_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set force VLAN port type into VLAN table.
 *
 * This function is used to set force VLAN port type into VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [in] port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_force_port_set(int unit, int vlan, int port_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_force_port_set(unit, vlan, port_type));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get force VLAN port type from VLAN table.
 *
 * This function is used to get force VLAN port type into VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [out]port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_force_port_get(int unit, int vlan, int *port_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(port_type, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_force_port_get(unit, vlan, port_type));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update all VLAN information to L2 station.
 *
 * This function is used to update all VLAN information to L2 station, and it is
 * only called by VLAN or L2 STATION during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_l2_station_update_all(int unit,
                                     bcmi_ltsw_vlan_l2_station_update_cb cb,
                                     void *user_data)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!vi->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_l2_station_update_all(unit, cb, user_data));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update all VLAN information to L3 interface.
 *
 * This function is used to update all VLAN information to L3 interface, and it
 * is only called by L3 INTF module during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
 int
 bcmi_ltsw_vlan_l3_intf_update_all(int unit,
                                   bcmi_ltsw_vlan_l3_intf_update_cb cb,
                                   void *user_data)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (!vi->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_l3_intf_update_all(unit, cb, user_data));
    VLAN_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a port into default vlan.
 *
 * This function is used to add a port into default vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_port_attach(int unit, bcm_port_t port)
{
    ltsw_vlan_info_t *vi = &ltsw_vlan_info[unit];
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    if (vi->default_port_type == 2 || vi->default_port_type == 3) {
        /* No Ethernet port in default VLAN. */
        SHR_EXIT();
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_port_params_validate(unit, BCM_VLAN_DEFAULT, pbmp, pbmp));

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (vlan_port_add(unit, BCM_VLAN_DEFAULT, pbmp, pbmp, pbmp, 0));
    VLAN_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_def_vlan_grp_update(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a port into default vlan.
 *
 * This function is used to remove a port into default vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_port_detach(int unit, bcm_port_t port)
{
    ltsw_vlan_info_t *vi = &ltsw_vlan_info[unit];
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    if (vi->default_port_type == 2 || vi->default_port_type == 3) {
        /* No Ethernet port in default VLAN. */
        SHR_EXIT();
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    VLAN_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_vlan_port_remove(unit, BCM_VLAN_DEFAULT, pbmp, pbmp, pbmp));
    VLAN_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_def_vlan_grp_update(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set VLAN membership check on a given port.
 *
 * This function is used to set VLAN membership check on a given port.
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
bcmi_ltsw_vlan_port_check_set(int unit, bcm_port_t port,
                              int disable, uint16_t flags)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_port_check_set(unit, port, disable, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete VLAN profile entry per index.
 *
 * This function is used to delete VLAN profile entry per index.
 *
 * \param [in] unit              Unit number.
 * \param [in] profile_hdl       Profile handle.
 * \param [in] index             Profile entry index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_profile_delete(int unit,
                              bcmi_ltsw_profile_hdl_t profile_hdl,
                              int index)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_profile_delete(unit, profile_hdl, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Display VLAN software structure information.
 *
 * This function is used to display VLAN software structure information.
 *
 * \retval None
 */
void
bcmi_ltsw_vlan_sw_dump(int unit)
{
    ltsw_vlan_info_t *vi = VLAN_INFO(unit);
    int valid_count = 0;
    int count = 0;
    int i = 0;

    if (!vi->initialized) {
        return;
    }

    SHR_BITCOUNT_RANGE(vi->vlan_bmp, count, 0, BCM_VLAN_COUNT);

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information VLAN - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init               : %4d\n"), vi->initialized));
    LOG_CLI((BSL_META_U(unit,
                        "  Default VLAN       : %4d\n"), *(vi->def_vlan)));
    LOG_CLI((BSL_META_U(unit,
                        "  VLAN Count         : %4d\n"), count));
    LOG_CLI((BSL_META_U(unit,
                        "  VLAN entry inited  : %4d\n"), *(vi->vlan_entry_inited)));

    /* Print VLAN list sorted. */
    LOG_CLI((BSL_META_U(unit,
                        "  List               :")));
    for (i = BCM_VLAN_MIN; i <= BCM_VLAN_MAX; i++) {
        if (SHR_BITGET(vi->vlan_bmp, i) == 0) {
            continue;
        }
        if ((valid_count > 0) && (!(valid_count % 10))) {
            LOG_CLI((BSL_META_U(unit, "\n              ")));
        }
        LOG_CLI((BSL_META_U(unit, " %4d"), i));
        valid_count++;
        if (count == valid_count) {
            break;
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

    SHR_BITCOUNT_RANGE(vi->pre_cfg_vlan_bmp, count, 0, BCM_VLAN_COUNT);

    LOG_CLI((BSL_META_U(unit,
                        "  Pre-cfg VLAN Count : %4d\n"), count));

    /* Print VLAN list sorted. */
    LOG_CLI((BSL_META_U(unit,
                        "  List               :")));
    valid_count = 0;
    for (i = BCM_VLAN_MIN; i <= BCM_VLAN_MAX; i++) {
        if (SHR_BITGET(vi->pre_cfg_vlan_bmp, i) == 0) {
            continue;
        }
        if ((valid_count > 0) && (!(valid_count % 10))) {
            LOG_CLI((BSL_META_U(unit, "\n                    ")));
        }
        LOG_CLI((BSL_META_U(unit, " %4d"), i));
        valid_count++;
        if (count == valid_count) {
            break;
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

    /* Print OTPID list sorted. */
    LOG_CLI((BSL_META_U(unit,
                        "  OTPID Count        : %4d\n"), VLAN_OTPID_MAX_NUM));
    LOG_CLI((BSL_META_U(unit,
                        "  List               :")));
    for (i = 0; i < VLAN_OTPID_MAX_NUM; i++) {
        LOG_CLI((BSL_META_U(unit,
                            " 0x%4x (ref_count = %4d)"),
                            VLAN_OTPID_ENTRY(unit, i),
                            VLAN_OTPID_REF_COUNT(unit, i)));
        if (i < VLAN_OTPID_MAX_NUM - 1) {
            LOG_CLI((BSL_META_U(unit, "\n                      ")));
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n")));

    /* Display VLAN structure information. */
    (void) mbcm_ltsw_vlan_info_dump(unit);

    return;
}

/*!
 * \brief Check if VLAN exists.
 *
 * This function is used to check if VLAN exists.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   vid                  VLAN ID.
 * \param [out]  exist                Exist.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_exist_check(int unit, bcm_vlan_t vid, int *exist)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(exist, SHR_E_PARAM);
    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);
    *exist = SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieves the number of VLAN created in the system.
 *
 * This function is used to retrieves the number of VLAN created in the system.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  Vlan Count.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_vlan_count_get(int unit, int *arg)
{
    int count = 0;
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_BITCOUNT_RANGE(VLAN_INFO(unit)->vlan_bmp, count, 0, BCM_VLAN_COUNT);
    *arg = count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port bitmap of default VLAN.
 *
 * This function can only be called during init to get pbmp of default VLAN.
 *
 * \param [in]   unit               Unit Number.
 * \param [in]   pbmp               Port bitmap.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_vlan_def_pbmp_get(int unit, bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);
    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_port_get(unit, *(VLAN_INFO(unit)->def_vlan), pbmp, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port bitmap of reserved default VLAN flooding group.
 *
 * This function is used to get port pbmp of default VLAN flooding group.
 *
 * \param [in]   unit               Unit Number.
 * \param [in]   pbmp               Port bitmap.
 *
 * \retval SHR_E_NONE               No errors.
 * \retval !SHR_E_NONE              Failure.
 */
int
bcmi_ltsw_vlan_def_flood_port_get(int unit, bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_def_flood_port_get(unit, pbmp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VLAN membership when port stack mode changes.
 *
 * This function is used to update VLAN member when port stack mode changes.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  flags                Flags.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_vlan_stk_update(int unit, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    if (bcmint_vlan_info_auto_stack_get(unit)) {
        bcm_pbmp_t add_ports, remove_ports, null_pbmp;
        int add = 0, remove = 0;
        bcm_vlan_t vid;
        bcm_pbmp_t stk_cur, stk_pre;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_pbmp_get(unit, &stk_cur, NULL, &stk_pre));
        BCM_PBMP_CLEAR(null_pbmp);
        BCM_PBMP_ASSIGN(add_ports, stk_cur);
        BCM_PBMP_REMOVE(add_ports, stk_pre);
        BCM_PBMP_ASSIGN(remove_ports, stk_pre);
        BCM_PBMP_REMOVE(remove_ports, stk_cur);

        if (BCM_PBMP_NOT_NULL(add_ports)) {
            add = 1;
        }
        if (BCM_PBMP_NOT_NULL(remove_ports)) {
            remove = 1;
        }
        if (!add && !remove) {
            SHR_EXIT();
        }

        VLAN_LOCK(unit);
        for (vid = BCM_VLAN_MIN; vid < BCM_VLAN_MAX; vid++) {
            if (0 == SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid)) {
                continue;
            }
            if (add) {
                SHR_IF_ERR_CONT
                    (vlan_port_add(unit, vid,
                                   add_ports, add_ports, null_pbmp, 0));
            }
            if (SHR_FUNC_ERR()) {
                break;
            }

            if (remove) {
                SHR_IF_ERR_CONT
                    (mbcm_ltsw_vlan_port_remove
                         (unit, vid, remove_ports, remove_ports, null_pbmp));
            }
            if (SHR_FUNC_ERR()) {
                break;
            }
        }
        VLAN_UNLOCK(unit);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VLAN range index when trunk member changes.
 *
 * This function is used to update VLAN range index when trunk member changes.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  gport                Original trunk member gport.
 * \param [in]  num_joining          Number of joining ports in trunk.
 * \param [in]  joining_arr          Joining ports in trunk.
 * \param [in]  num_leaving          Number of leaving ports in trunk.
 * \param [in]  leaving_arr          Leaving ports in trunk.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_vlan_range_update(int unit, bcm_gport_t gport,
                            int num_joining, bcm_gport_t *joining_arr,
                            int num_leaving, bcm_gport_t *leaving_arr)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    if (((num_leaving > 0) && (NULL == leaving_arr)) ||
        ((num_joining > 0) && (NULL == joining_arr))) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_range_update(unit, gport,
                num_joining, joining_arr, num_leaving, leaving_arr));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ingress vlan qos id.
 *
 * This function is used to set ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_qos_set(int unit, bcm_vlan_t vid, int id)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_qos_set(unit, vid, id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress vlan qos id.
 *
 * This function is used to get ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_ing_qos_get(int unit, bcm_vlan_t vid, int *id)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_ing_qos_get(unit, vid, id));

exit:
    SHR_FUNC_EXIT()
}

/*!
 * \brief Set egress vlan qos id.
 *
 * This function is used to set egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_egr_qos_set(int unit, bcm_vlan_t vid, int id)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_qos_set(unit, vid, id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress vlan qos id.
 *
 * This function is used to get egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_egr_qos_get(int unit, bcm_vlan_t vid, int *id)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    VLAN_ID_CHECK(vid);
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_egr_qos_get(unit, vid, id));

exit:
    SHR_FUNC_EXIT()
}

/*!
 * \brief Set vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value to set the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_control_set(int unit,
                           bcmi_ltsw_vlan_control_t type,
                           int value)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_internal_control_set(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value pointer to get the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_control_get(int unit,
                           bcmi_ltsw_vlan_control_t type,
                           int *value)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_internal_control_get(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update vlan xlate pipe sw info.
 *
 * \param [in] unit              Unit number.
 * \param [in] pg                Port group.
 * \param [in] ingress           Ingress/egress.
 * \param [in] pipe              Pipe.
 * \param [in] op                Op code: Insert/Delete.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_xlate_pipe_update(int unit, bcm_port_t pg, int ingress,
                                 int pipe, bcmi_ltsw_vlan_table_opcode_t op)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_xlate_pipe_update(unit, pg, ingress, pipe, op));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STG into a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] ingress   Ingress or egress.
 * \param [in] stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_stg_set(unit, vid, ingress, stg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STG from a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] ingress   Ingress or egress.
 * \param [out]stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_stg_get(unit, vid, ingress, stg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Notify VLAN membership check information for L2 tunnel case.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] add       Add or delete.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_vlan_check_info_notify(int unit, bcm_vlan_t vid, int add)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_vlan_check_info_notify(unit, vid, add),
         SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Check if egr xlate pipe mode is supported.
 *
 * \param [in] unit              Unit number.
 * \param [in] support           Support or not.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_xlate_egr_pipe_check(int unit, int *support)
{
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int i, rv;
    int dunit;
    uint64_t value;
    bcmint_vlan_lt_id_t lt_id[] = {
        BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_PIPE_CONFIG,
        BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW_PIPE_CONFIG
    };
    SHR_FUNC_ENTER(unit);

    *support = 0;
    dunit = bcmi_ltsw_dev_dunit(unit);
    for (i = 0; i < COUNTOF(lt_id); i++) {
        rv = bcmint_vlan_lt_get(unit, lt_id[i], &lt_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        flds = lt_info->flds;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, ent_hdl, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        if (value) {
            *support = (int)value;
            break;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(ent_hdl));
        ent_hdl = BCMLT_INVALID_HDL;
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get default vlan membership ports profile id.
 *
 * This function is used to get default vlan membership ports profile id.
 *
 * \param [in] unit              Unit number.
 * \param [in] type             Default vlan membership ports profile type.
 * \param [out] profile_id    Default vlan membership ports profile id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_mshp_default_profile_get(int unit,
                                        bcmi_ltsw_profile_hdl_t type,
                                        int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    VLAN_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_mshp_default_profile_get(unit, type, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse vlan table to get needed info.
 *
 * This function is used to traverse vlan table without lock.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcmi_ltsw_vlan_traverse(int unit,
                        bcmi_ltsw_vlan_traverse_cb cb, void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_traverse(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get maximum number of outer TPID.
 *
 * This function is used to get maximum number of outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  max_num              Max TPID Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_otpid_max_num(int unit, int *max_num)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(max_num, SHR_E_PARAM);

    *max_num = VLAN_OTPID_MAX_NUM;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set outer TPID value.
 *
 * This function is used to set outer TPID value only for VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index                Outer TPID entry index.
 * \param [in]  tpid                 Outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_otpid_info_tpid_set(int unit, int index, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    if ((index < 0) || (index >= VLAN_OTPID_MAX_NUM)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    VLAN_OTPID_ENTRY(unit, index) = tpid;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add reference count of outer TPID.
 *
 * This function is used to set reference count of outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index                Outer TPID entry index.
 * \param [in]  count                Reference count.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_otpid_info_ref_count_add(int unit, int index, int count)
{
    SHR_FUNC_ENTER(unit);

    if ((index < 0) || (index >= VLAN_OTPID_MAX_NUM) ||
        ((VLAN_OTPID_REF_COUNT(unit, index) + count) < 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    VLAN_OTPID_REF_COUNT(unit, index) += count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get maximum number of payload outer TPID.
 *
 * This function is used to get maximum number of payload outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                      Unit Number.
 * \param [in]  max_num              Max TPID Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_payload_otpid_max_num(int unit, int *max_num)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(max_num, SHR_E_PARAM);

    *max_num = VLAN_PAYLOAD_OTPID_MAX_NUM;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set payload outer TPID value.
 *
 * This function is used to set payload outer TPID value only for VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index              Payload outer TPID entry index.
 * \param [in]  tpid                 Payload outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_payload_otpid_info_tpid_set(int unit, int index, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    if ((index < 0) || (index >= VLAN_PAYLOAD_OTPID_MAX_NUM)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    VLAN_PAYLOAD_OTPID_ENTRY(unit, index) = tpid;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add reference count of payload outer TPID.
 *
 * This function is used to set reference count of payload outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index               Payload outer TPID entry index.
 * \param [in]  count                Reference count.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_payload_otpid_info_ref_count_add(int unit, int index, int count)
{
    SHR_FUNC_ENTER(unit);

    if ((index < 0) || (index >= VLAN_PAYLOAD_OTPID_MAX_NUM) ||
        ((VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index) + count) < 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    VLAN_PAYLOAD_OTPID_REF_COUNT(unit, index) += count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set defualt VLAN information.
 *
 * This function is used to set default vlan into VLAN information.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  def_vlan             Defualt VLAN.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmint_vlan_info_default_vlan_set(int unit, bcm_vlan_t def_vlan)
{
    SHR_FUNC_ENTER(unit);

    *(VLAN_INFO(unit)->def_vlan) = def_vlan;
    SHR_BITSET(VLAN_INFO(unit)->vlan_bmp, def_vlan);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get VLAN auto stack flag.
 *
 * This function is used to get VLAN auto stack flag.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval VLAN auto stack flag.
 */
int
bcmint_vlan_info_auto_stack_get(int unit)
{
    return (VLAN_INFO(unit)->vlan_auto_stack);
}

/*!
 * \brief Get port type in default VLAN.
 *
 * This function is used to get port type in default VLAN.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval Port type in default VLAN.
 */
int
bcmint_vlan_info_default_port_type_get(int unit)
{
    return (VLAN_INFO(unit)->default_port_type);
}

/*!
 * \brief Check if the VLAN is pre-configured.
 *
 * This function is used to check if the VLAN is pre-configured.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval Zero                      Not pre-configured VLAN.
 * \retval None-zero                 Pre-configured VLAN.
 */
int
bcmint_vlan_info_pre_cfg_get(int unit, bcm_vlan_t vid)
{
    return (SHR_BITGET(VLAN_INFO(unit)->pre_cfg_vlan_bmp, vid) ? 1 : 0);
}

/*!
 * \brief Set the bitmap for pre-configured VLAN.
 *
 * This function is used to set the bitmap for pre-configured VLAN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval None
 */
void
bcmint_vlan_info_pre_cfg_set(int unit, bcm_vlan_t vid)
{
    SHR_BITSET(VLAN_INFO(unit)->pre_cfg_vlan_bmp, vid);

    return;
}

/*!
 * \brief Check if VLAN exists.
 *
 * This function is used to check if VLAN exists.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval Zero                      VLAN doesn't exist.
 * \retval None-zero                 VLAN exists.
 */
int
bcmint_vlan_info_exist_check(int unit, bcm_vlan_t vid)
{
    return (SHR_BITGET(VLAN_INFO(unit)->vlan_bmp, vid) ? 1 : 0);
}

/*!
 * \brief Set the bitmap of existing VLANs.
 *
 * This function is used to set the bitmap of existing VLANs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval None
 */
void
bcmint_vlan_info_set(int unit, bcm_vlan_t vid)
{
    SHR_BITSET(VLAN_INFO(unit)->vlan_bmp, vid);
    return;
}

/*!
 * \brief Set the vlan entry init state
 *
 * This function is used to set the vlan entry init state.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  inited               Init state.
 *
 * \retval None
 */
void
bcmint_vlan_entry_init_state_set(int unit, uint8_t inited)
{
    *(VLAN_INFO(unit)->vlan_entry_inited) = inited;
    return;
}

/*!
 * \brief Get the vlan entry init state
 *
 * This function is used to get the vlan entry init state.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval Zero                      VLAN entry is not inited.
 * \retval None-zero                 VLAN enty is inited.
 */
int
bcmint_vlan_entry_init_state_get(int unit)
{
    return (*(VLAN_INFO(unit)->vlan_entry_inited) ? 1 : 0);
}

/*!
 * \brief Get per npl  LT info.
 *
 * This function is used to get vlan lt info based on NPL version.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   lt_id                VLAN LT ID.
 * \param [out]  lt_info              LT info.
 *
 * \retval None
 */
int
bcmint_vlan_lt_get(int unit,
                   bcmint_vlan_lt_id_t lt_id,
                   const bcmint_vlan_lt_t **lt_info)
{
    bcmint_vlan_lt_db_t lt_db;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_lt_db_get(unit, &lt_db));

    if (!(lt_db.lt_bmp & (1 << lt_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *lt_info = &lt_db.lts[lt_id];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if vlan xlate tables share same pt resource.
 *
 * This function is used to check if vlan xlate tables share same pt resource.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   lt_id                VLAN LT ID.
 * \param [out]  share                share.
 *
 * \retval None
 */
int
bcmint_vlan_xlate_table_share(int unit, vlan_xlate_table_id_t lt_id, int *share)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_xlate_table_share(unit, lt_id, share));

exit:
    SHR_FUNC_EXIT();
}
