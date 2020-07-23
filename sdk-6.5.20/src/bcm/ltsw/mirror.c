/*! \file mirror.c
 *
 * Mirror Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/mirror_int.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/mod.h>
#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/mbcm/mirror.h>
#include <bcm_int/ltsw/generated/mirror_ha.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MIRROR

/*
 * Data structure to save Mirror destination configuration.
 */
typedef struct mirror_dest_cfg_s {

    /* Mirror destination. */
    bcm_mirror_destination_t mirror_dest;

    /* Reference count. */
    int ref_cnt;

} mirror_dest_cfg_t;

/*
 * Data structure to save Cosq MOD control data.
 */
typedef struct mirror_cosq_mod_ctrl_s {

    /*
     * Mirror destination ids for MOD.
     * The size equals to MIRROR_MOD_DEST_NUM.
     */
    int *mod_mirror_dest_id;

    /*
     * This stores the mod dest id array allocated by MOD module, one element
     * indicates a mod dest id (index of TM_MIRROR_ON_DROP_DESTINATION)
     * associated with a specific mirror destination.
     * The size equals to MIRROR_DEST_NUM and the index is mod_mirror_dest_id,
     * mod_dest_id[mod_mirror_dest_id[i]] == bcmi_ltsw_cosq_mod_dest_t.mod_dest_id.
     */
    int *mod_dest_id_map;

} mirror_cosq_mod_ctrl_t;

/*
 * Global Mirror data structure.
 */
typedef struct mirror_cfg_s {

    /* Mirror module is initialized. */
    bool inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Mirror control data. */
    bcmint_mirror_ctrl_info_t ctrl_data;

    /* Mirror destination. */
    mirror_dest_cfg_t *mirror_dest;

    /* Mirror session. */
    bcmint_mirror_session_cfg_t *session;

    /* Mirror container. */
    bcmint_mirror_container_cfg_t *container;

    /* CoSQ MOD control data. */
    mirror_cosq_mod_ctrl_t cosq_mod_ctrl;

    /*! Sample profile used bitmap. */
    SHR_BITDCL *sample_prf_bmp[bcmintMirrorSampleCount];

} mirror_cfg_t;

/*
 * Static global variable to hold Mirror info.
 */
static mirror_cfg_t mirror_info[BCM_MAX_NUM_UNITS] = {{0}};

/* Mirror info. */
#define MIRROR_INFO(unit) \
    (&mirror_info[unit])

/* Mirror control info. */
#define MIRROR_CTRL(unit) \
    (&MIRROR_INFO(unit)->ctrl_data)

/* Mirror destination number. */
#define MIRROR_DEST_NUM(unit) \
    (MIRROR_CTRL(unit)->dest_num)

/* Mirror container number. */
#define MIRROR_CONTAINER_NUM(unit) \
    (MIRROR_CTRL(unit)->container_num)

/* Mirror container node. */
#define MIRROR_CONTAINER(unit, id) \
    (&MIRROR_INFO(unit)->container[id])

/* Check Mirror container id is valid. */
#define MIRROR_CONTAINER_ID_CHECK(unit, id)                   \
    do {                                                      \
        if ((id) < 0 || (id) >= MIRROR_CONTAINER_NUM(unit)) { \
            SHR_ERR_EXIT(SHR_E_PARAM);                 \
        }                                                     \
    } while (0)

/* Mirror container reference count. */
#define MIRROR_CONTAINER_REF_COUNT(unit, id) \
    (MIRROR_CONTAINER(unit, id)->ref_cnt)

/* Mirror session number. */
#define MIRROR_SESSION_NUM(unit) \
    (MIRROR_CTRL(unit)->session_attrs.session_num)

/* Mirror session node. */
#define MIRROR_SESSION(unit, id) \
    (&MIRROR_INFO(unit)->session[id])

/* Check Mirror session id is valid. */
#define MIRROR_SESSION_ID_CHECK(unit, id)                   \
    do {                                                    \
        if ((id) < 0 || (id) >= MIRROR_SESSION_NUM(unit)) { \
            SHR_ERR_EXIT(SHR_E_PARAM);               \
        }                                                   \
    } while (0)

/* Mirror session model. */
#define MIRROR_SESSION_MODEL(unit) \
    (MIRROR_CTRL(unit)->session_attrs.model)

/* Mirror session array number. */
#define MIRROR_SESSION_ARRAY_NUM(unit) \
    (MIRROR_CTRL(unit)->session_attrs.session_array_num)

/* Mirror session reference count. */
#define MIRROR_SESSION_REF_COUNT(unit, id) \
    (MIRROR_SESSION(unit, id)->ref_cnt)

/* Mirror instance number per session. */
#define MIRROR_INSTANCE_NUM(unit) \
    (MIRROR_CTRL(unit)->session_attrs.instance_num)

/* Mirror instance zero is reserved. */
#define MIRROR_INSTANCE_ZERO_RSVD(unit) \
    (MIRROR_CTRL(unit)->session_attrs.rsvd_instance_zero)

/* Mirror destination number. */
#define MIRROR_DEST_NUM(unit) \
    (MIRROR_CTRL(unit)->dest_num)

/* Mirror destination node. */
#define MIRROR_DEST(unit, id) \
    (&MIRROR_INFO(unit)->mirror_dest[id])

/* Check Mirror destination id is valid. */
#define MIRROR_DEST_ID_CHECK(unit, id) \
    do { \
        if ((id) < 0 || (id) >= MIRROR_DEST_NUM(unit)) { \
            SHR_ERR_EXIT(SHR_E_PARAM); \
        } \
    } while (0)

/* Reference count of Mirror destination node. */
#define MIRROR_DEST_REF_COUNT(unit, id) \
    (MIRROR_DEST(unit, id)->ref_cnt)

/* Destination data of Mirror destination node. */
#define MIRROR_DEST_DATA(unit, id) \
    (&MIRROR_DEST(unit, id)->mirror_dest)

/* MOD destination number. */
#define MIRROR_MOD_DEST_NUM(unit) \
    (MIRROR_CTRL(unit)->mod_dest_num)

/* MOD destinations share a unique encap. */
#define MIRROR_MOD_DEST_SHR_ENCAP(unit) \
    (MIRROR_CTRL(unit)->session_attrs.mod_dest_shared_encap)

/* MOD control data. */
#define MIRROR_MOD_CTRL(unit) \
    (&MIRROR_INFO(unit)->cosq_mod_ctrl)

/* Mirror sample profile number. */
#define MIRROR_SAMPLE_PRF_NUM(unit) \
    (MIRROR_CTRL(unit)->sample_prf_info.prf_num)

/* Reserve Mirror sample profile id 0. */
#define MIRROR_SAMPLE_RSVD_PRF_ID_ZERO(unit) \
    (MIRROR_CTRL(unit)->sample_prf_info.rsvd_prf_id_zero)

/* Mirror sample profile used bitmap. */
#define MIRROR_SAMPLE_PRF_BMP(unit, type) \
    (MIRROR_INFO(unit)->sample_prf_bmp[type])

/* Set Mirror sample profile used bitmap. */
#define MIRROR_SAMPLE_PRF_BMP_USED_SET(unit, type, id) \
    (SHR_BITSET(MIRROR_SAMPLE_PRF_BMP(unit, type), id))

/* Get Mirror sample profile used bitmap. */
#define MIRROR_SAMPLE_PRF_BMP_USED_GET(unit, type, id) \
    (SHR_BITGET(MIRROR_SAMPLE_PRF_BMP(unit, type), id))

/* Clear Mirror sample profile used bitmap. */
#define MIRROR_SAMPLE_PRF_BMP_USED_CLR(unit, type, id) \
    (SHR_BITCLR(MIRROR_SAMPLE_PRF_BMP(unit, type), id))

/* Check whether the device supports sample profile. */
#define MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit) \
    do { \
        if (!MIRROR_SAMPLE_PRF_NUM(unit)) { \
            SHR_ERR_EXIT(SHR_E_UNAVAIL); \
        } \
    } while (0)

/* Check Mirror sample profile id is valid. */
#define MIRROR_SAMPLE_PRF_ID_CHECK(unit, id) \
    do { \
        if ((id) < 0 || (id) >= MIRROR_SAMPLE_PRF_NUM(unit)) { \
            SHR_ERR_EXIT(SHR_E_BADID); \
        } \
    } while (0)

/* Check if Mirror module is initialized. */
#define MIRROR_INIT_CHECK(unit) \
    do { \
        mirror_cfg_t *mirror_cfg = MIRROR_INFO(unit); \
        if (mirror_cfg->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* Take Mirror lock. */
#define MIRROR_LOCK(unit) \
    sal_mutex_take(MIRROR_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give Mirror lock. */
#define MIRROR_UNLOCK(unit) \
    sal_mutex_give(MIRROR_INFO(unit)->mutex);

/* Mirror container status enum. */
typedef enum mirror_container_status_e {

    /* Mirror container is free. */
    MIRROR_FREE = 0,

    /* Mirror container is occupied by others. */
    MIRROR_OCCUPIED = 1,

    /* Mirror container does not support this user type. */
    MIRROR_UNAVAILABLE = 2
} mirror_container_status_t;

/* Mirror source is egress mirroring type or not. */
#define MIRROR_SOURCE_IS_EGR_TYPE(type) \
    ((type == BCMINT_MIRROR_ST_F_PORT_EGR || \
      type == BCMINT_MIRROR_ST_F_FP_EGR) ? true : false)

/* Mirror source is cosq_mod mirroring type or not. */
#define MIRROR_SOURCE_IS_COSQ_MOD_TYPE(type) \
    ((type == BCMINT_MIRROR_ST_F_COSQ_MOD) ? true : false)

/******************************************************************************
 * Private functions
 */
static int
mirror_container_enable_get(int unit, bcmint_mirror_source_t *source,
                            int *session_id, int *enable);

static int
mirror_session_id_get_by_dest(int unit, bcmint_mirror_source_t *source,
                              int dest_id, int container_id,
                              int *count, int *session_id);

static int
mirror_dest_recover(int unit);

static int
mod_mirror_dest_id_bk_add(int unit, int dest_id);

static int
mod_mirror_dest_id_bk_delete(int unit, int dest_id);

static int
mod_mirror_dest_id_bk_cnt(int unit, int* count);

/*!
 * \brief Create Mirror lock.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* Create mutex. */
    if (MIRROR_INFO(unit)->mutex == NULL) {
        MIRROR_INFO(unit)->mutex = sal_mutex_create("Mirror mutex");
        SHR_NULL_CHECK(MIRROR_INFO(unit)->mutex, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy Mirror lock.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_lock_destroy(int unit)
{
    if (MIRROR_INFO(unit)->mutex) {
        sal_mutex_destroy(MIRROR_INFO(unit)->mutex);
        MIRROR_INFO(unit)->mutex = NULL;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Init Mirror control info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_ctrl_info_init(int unit)
{
    mirror_cfg_t *mirror_cfg = MIRROR_INFO(unit);
    bcmint_mirror_ctrl_info_t mirror_ctrl;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mirror_cfg, SHR_E_MEMORY);

    sal_memset(&mirror_ctrl, 0x0, sizeof(bcmint_mirror_ctrl_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_ctrl_info_init(unit, &mirror_ctrl));
    sal_memcpy(&mirror_cfg->ctrl_data,
               &mirror_ctrl,
               sizeof(bcmint_mirror_ctrl_info_t));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deinit Mirror control info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_ctrl_info_deinit(int unit)
{
    bcmint_mirror_ctrl_info_t *mirror_ctrl = MIRROR_CTRL(unit);

    if (mirror_ctrl) {
        SHR_FREE(mirror_ctrl->mc_st_flags);
        sal_memset(mirror_ctrl, 0, sizeof(bcmint_mirror_ctrl_info_t));
    }
    return SHR_E_NONE;
}

/*!
 * \brief Init Mirror bookkeeping info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_bk_init(int unit)
{
    uint32_t alloc_size = 0, req_size = 0;
    void *ptr = NULL, *ptr_tmp = NULL, *ptr_mod_mirror_dest = NULL,
         *ptr_mod_dest_id_map = NULL;
    mirror_cfg_t *mirror_cfg = MIRROR_INFO(unit);
    int warm = bcmi_warmboot_get(unit), i;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    int rv, num = 0;
    void *ptr_sample[bcmintMirrorSampleCount];

    SHR_FUNC_ENTER(unit);

    /* Init local variable. */
    for (i = 0; i < bcmintMirrorSampleCount; i++) {
        ptr_sample[i] = NULL;
    }

    /* Allocate HA memory for mirror session. */
    ha_instance_size = sizeof(bcmint_mirror_session_cfg_t);
    ha_array_size = MIRROR_SESSION_NUM(unit);
    req_size = ha_instance_size * ha_array_size;
    alloc_size = req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_MIRROR,
                                 BCMINT_MIRROR_SUB_COMP_ID_MIRROR_SESSION,
                                 "bcmMirrorSession",
                                 &alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((alloc_size < req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    mirror_cfg->session = (bcmint_mirror_session_cfg_t *)ptr;
    if (!warm) {
        sal_memset(ptr, 0, alloc_size);
        /* -1 indicates no encap. */
        for (i = 0; i < MIRROR_SESSION_NUM(unit); i++) {
            mirror_cfg->session[i].encap_id = -1;
        }
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
             BCMI_HA_COMP_ID_MIRROR,
             BCMINT_MIRROR_SUB_COMP_ID_MIRROR_SESSION,
             0, ha_instance_size, ha_array_size,
             BCMINT_MIRROR_SESSION_CFG_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Allocate HA memory for mirror container. */
    ha_instance_size = sizeof(bcmint_mirror_container_cfg_t);
    ha_array_size = MIRROR_CONTAINER_NUM(unit);
    req_size = ha_instance_size * ha_array_size;
    alloc_size = req_size;
    ptr_tmp = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_MIRROR,
                                     BCMINT_MIRROR_SUB_COMP_ID_MIRROR_CONTAINER,
                                     "bcmMirrorContainer",
                                     &alloc_size);
    SHR_NULL_CHECK(ptr_tmp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((alloc_size < req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr_tmp, 0, alloc_size);
    }
    mirror_cfg->container = (bcmint_mirror_container_cfg_t *)ptr_tmp;

    rv = bcmi_ltsw_issu_struct_info_report(unit,
             BCMI_HA_COMP_ID_MIRROR,
             BCMINT_MIRROR_SUB_COMP_ID_MIRROR_CONTAINER,
             0, ha_instance_size, ha_array_size,
             BCMINT_MIRROR_CONTAINER_CFG_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Alloc heap memory for mirror destination. */
    alloc_size = MIRROR_DEST_NUM(unit) * sizeof(mirror_dest_cfg_t);
    mirror_cfg->mirror_dest = sal_alloc(alloc_size, "ltswMirrorDest");
    SHR_NULL_CHECK(mirror_cfg->mirror_dest, SHR_E_MEMORY);
    sal_memset(mirror_cfg->mirror_dest, 0, alloc_size);

    /* Allocate HA memory for mod destination id. */
    alloc_size = MIRROR_MOD_DEST_NUM(unit) * sizeof(int);
    ptr_mod_mirror_dest = bcmi_ltsw_ha_mem_alloc(unit,
                       BCMI_HA_COMP_ID_MIRROR,
                       BCMINT_MIRROR_SUB_COMP_ID_MIRROR_MOD_MIRROR_DEST_ID,
                       "bcmMirrorModDestId",
                       &alloc_size);
    SHR_NULL_CHECK(ptr_mod_mirror_dest, SHR_E_MEMORY);
    MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id = (int *)ptr_mod_mirror_dest;
    if (!warm) {
        for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
            MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] = -1;
        }
    }

    /* Allocate HA memory for mod destination id mappings. */
    alloc_size = MIRROR_DEST_NUM(unit) * sizeof(int);
    ptr_mod_dest_id_map = bcmi_ltsw_ha_mem_alloc(unit,
                         BCMI_HA_COMP_ID_MIRROR,
                         BCMINT_MIRROR_SUB_COMP_ID_MIRROR_MOD_DEST_ID_MAP,
                         "bcmMirrorModDestIdMap",
                         &alloc_size);
    SHR_NULL_CHECK(ptr_mod_dest_id_map, SHR_E_MEMORY);
    MIRROR_MOD_CTRL(unit)->mod_dest_id_map = (int *)ptr_mod_dest_id_map;
    if (!warm) {
        for (i = 0; i < MIRROR_DEST_NUM(unit); i++) {
            MIRROR_MOD_CTRL(unit)->mod_dest_id_map[i] = -1;
        }
    }

    /*
     * Allocate HA memory for sample profile bitmap only when
     * the device supports sample profile.
     */
    num = MIRROR_SAMPLE_PRF_NUM(unit);
    if (num != 0) {
        for (i = 0; i < bcmintMirrorSampleCount; i++) {
            req_size = SHR_BITALLOCSIZE(num);
            alloc_size = req_size;
            ptr_sample[i] = bcmi_ltsw_ha_mem_alloc(unit,
                     BCMI_HA_COMP_ID_MIRROR,
                     BCMINT_MIRROR_SUB_COMP_ID_MIRROR_ING_SAMPLE_PRF_BMP + i,
                     "bcmMirrorIngSamplePrfBmp",
                     &alloc_size);
            SHR_NULL_CHECK(ptr_sample[i], SHR_E_MEMORY);
            SHR_IF_ERR_VERBOSE_EXIT
                ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
            if (!warm) {
                sal_memset(ptr_sample[i], 0, alloc_size);
            }
            mirror_cfg->sample_prf_bmp[i] = ptr_sample[i];
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(mirror_cfg->mirror_dest);
        if (!warm && ptr_tmp) {
            (void)bcmi_ltsw_ha_mem_free(unit, ptr_tmp);
            mirror_cfg->container = NULL;
        }
        if (!warm && ptr) {
            (void)bcmi_ltsw_ha_mem_free(unit, ptr);
            mirror_cfg->session = NULL;
        }
        if (!warm && ptr_mod_mirror_dest) {
            (void)bcmi_ltsw_ha_mem_free(unit, ptr_mod_mirror_dest);
            MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id = NULL;
        }
        if (!warm && ptr_mod_dest_id_map) {
            (void)bcmi_ltsw_ha_mem_free(unit, ptr_mod_dest_id_map);
            MIRROR_MOD_CTRL(unit)->mod_dest_id_map = NULL;
        }
        if (!warm) {
            for (i = 0; i < bcmintMirrorSampleCount; i++) {
                (void)bcmi_ltsw_ha_mem_free(unit, ptr_sample[i]);
                mirror_cfg->sample_prf_bmp[i] = NULL;
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add Mirror encap entry and return the encap index.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress mirroring.
 * \param [in] mirror_dest Mirror destination pointer.
 * \param [out] index Mirror encap index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_encap_add(int unit, bool egr,
                 bcm_mirror_destination_t *mirror_dest, int *index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_encap_add(unit, egr, mirror_dest, index));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete Mirror encap entry.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination pointer.
 * \param [in] index Mirror encap index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_encap_delete(int unit, bcm_mirror_destination_t *mirror_dest, int index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_encap_delete(unit, mirror_dest, index));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search Mirror encap index by Mirror session.
 *
 * \param [in] unit Unit number.
 * \param [in] session_id Mirror session id.
 * \param [out] index Mirror encap index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_encap_search(int unit, int session_id, int *index)
{
    if (index == NULL) {
        return SHR_E_PARAM;
    }
    *index = MIRROR_SESSION(unit, session_id)->encap_id;
    return SHR_E_NONE;
}

/*!
 * \brief Get Mirror destination for given encap index.
 *
 * \param [in] unit Unit number.
 * \param [in] index Mirror encap index.
 * \param [out] mirror_dest Mirror destination pointer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_encap_get(int unit, int index, bcm_mirror_destination_t *mirror_dest)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_encap_get(unit, index, mirror_dest));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a Mirror destination buffer.
 *
 * \param [in] unit Unit number.
 * \param [out] mirror_dest Mirror destination buffer pointer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_destination_alloc(int unit, bcm_mirror_destination_t **mirror_dest)
{
    int alloc_size;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mirror_dest, SHR_E_PARAM);

    alloc_size = sizeof(bcm_mirror_destination_t);

    *mirror_dest = sal_alloc(alloc_size, "ltswMirrorDestBuffer");
    SHR_NULL_CHECK(*mirror_dest, SHR_E_MEMORY);
    sal_memset(*mirror_dest, 0, alloc_size);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a Mirror destination buffer.
 *
 * \param [out] mirror_dest Mirror destination buffer pointer.
 */
static void
mirror_destination_free(bcm_mirror_destination_t **mirror_dest)
{
    if (mirror_dest && *mirror_dest) {
        sal_free(*mirror_dest);
        *mirror_dest = NULL;
    }
}

/*!
 * \brief Reference a Mirror destination id.
 *
 * \param [in] unit Unit number.
 * \param [in] id Destination id.
 * \param [in] count Increased count.
 */
static void
mirror_dest_refer(int unit, int id, uint32_t count)
{
    MIRROR_DEST_REF_COUNT(unit, id) += count;
}

/*!
 * \brief Dereference a Mirror destination id.
 *
 * \param [in] unit Unit number.
 * \param [in] id Destination id.
 * \param [in] count Decreased count.
 */
static void
mirror_dest_derefer(int unit, int id, uint32_t count)
{
    MIRROR_DEST_REF_COUNT(unit, id) -= count;
}

/*!
 * \brief Compare two Mirror destinations data.
 *
 * \param [in] mirr1 Mirror destination to be compared.
 * \param [in] mirr2 Mirror destination to be compared.
 *
 * \retval return 0 if they are equal, otherwise false.
 */
static bool
mirror_dest_compare(bcm_mirror_destination_t *mirr1,
                    bcm_mirror_destination_t *mirr2)
{
    bcm_mirror_destination_t mirror_tmp1, mirror_tmp2;
    bool equal;

    mirror_tmp1 = *mirr1;
    mirror_tmp2 = *mirr2;

    /* Clear Mirror destination id. */
    mirror_tmp1.mirror_dest_id = 0;
    mirror_tmp2.mirror_dest_id = 0;

    /* Clear invalid flags. */
    mirror_tmp1.flags &= ~(BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_WITH_ID);
    mirror_tmp2.flags &= ~(BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_WITH_ID);
    equal = sal_memcmp(&mirror_tmp1,
                       &mirror_tmp2,
                       sizeof(bcm_mirror_destination_t));
    return equal;
}


/*!
 * \brief Allocate a free destination id from destination pool.
 *
 * \param [in] unit Unit number.
 * \param [out] id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_id_alloc(int unit, int *id)
{
    int i;

    for (i = 0; i < MIRROR_DEST_NUM(unit); i++) {
        if (MIRROR_DEST_REF_COUNT(unit, i)) {
            continue;
        }
        *id = i;
        return SHR_E_NONE;
    }
    return SHR_E_RESOURCE;
}

/*!
 * \brief Add a Mirror destination to destination pool.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_add(int unit, bcm_mirror_destination_t *mirror_dest)
{
    int i;

    SHR_FUNC_ENTER(unit);

    i = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
    MIRROR_DEST_ID_CHECK(unit, i);

    *(MIRROR_DEST_DATA(unit, i)) = *mirror_dest;
    mirror_dest_refer(unit, i, 1);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update a Mirror destination for given id in destination pool.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_update(int unit, bcm_mirror_destination_t *mirror_dest)
{
    int i;

    SHR_FUNC_ENTER(unit);

    i = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
    MIRROR_DEST_ID_CHECK(unit, i);

    /*
     * User passes a un-created mirror destination id with WITH_ID and
     * REPLACE flags, go ahead to create destination to keep same behavior
     * as legacy chips.
     */
    if (MIRROR_DEST_REF_COUNT(unit, i) == 0) {
        mirror_dest_refer(unit, i, 1);
    }

    *(MIRROR_DEST_DATA(unit, i)) = *mirror_dest;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Mirror destination for given id from destination pool.
 *
 * \param [in] unit Unit number.
 * \param [in] id Destination id.
 * \param [out] mirror_dest Mirror destination.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_get(int unit, int id, bcm_mirror_destination_t *mirror_dest)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_DEST_ID_CHECK(unit, id);

    if (MIRROR_DEST_REF_COUNT(unit, id) == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *mirror_dest = *(MIRROR_DEST_DATA(unit, id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Mirror destination id.
 *
 * \param [in] unit Unit number.
 * \param [in] id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_delete(int unit, int id)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_DEST_ID_CHECK(unit, id);

    /* The destination is still being used by others. */
    if (MIRROR_DEST_REF_COUNT(unit, id) > 1) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    } else if (MIRROR_DEST_REF_COUNT(unit, id) <= 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    mirror_dest_derefer(unit, id, 1);
    sal_memset(MIRROR_DEST_DATA(unit, id), 0, sizeof(bcm_mirror_destination_t));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse Mirror destinations and call user's callback.
 *
 * \param [in] unit Unit number.
 * \param [in] cb User callback.
 * \param [in] user_data User cookie
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_traverse(int unit, bcm_mirror_destination_traverse_cb cb,
                     void *user_data)
{
    int i;
    bcm_mirror_destination_t mirror_dest;

    if (cb == NULL) {
        return SHR_E_PARAM;
    }

    for (i = 0; i < MIRROR_DEST_NUM(unit); i++) {
        if (MIRROR_DEST_REF_COUNT(unit, i) == 0) {
            continue;
        }
        mirror_dest = *(MIRROR_DEST_DATA(unit, i));

        (void)(*cb)(unit, &mirror_dest, user_data);
    }
    return SHR_E_NONE;
}

/*!
 * \brief Search a Mirror destination for given MTP or destination data.
 *
 * \param [in] unit Unit number.
 * \param [in] port MTP port.
 * \param [in] mirror Mirror destination to be compared, NULL for not compared.
 * \param [out] mirror_dest Matched Mirror destination.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_search(int unit, bcm_gport_t port,
                   bcm_mirror_destination_t *mirror,
                   bcm_mirror_destination_t *mirror_dest)
{
    int i;

    for (i = 0; i < MIRROR_DEST_NUM(unit); i++) {
        if (MIRROR_DEST_REF_COUNT(unit, i) == 0) {
            continue;
        }
        if (port == MIRROR_DEST_DATA(unit, i)->gport) {
            if (mirror != NULL) {
                if (mirror_dest_compare(mirror,
                                        MIRROR_DEST_DATA(unit, i)) != 0) {
                    continue;
                }
            }
            *mirror_dest = *(MIRROR_DEST_DATA(unit, i));
            return SHR_E_NONE;
        }
    }
    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Mirror destination gport validate.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Mirror destination gport.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_gport_validate(int unit, bcm_gport_t gport)
{
    int id;
    int modid;
    bcm_port_t port;
    bcm_trunk_t trunk_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit,
                                      gport,
                                      &modid,
                                      &port,
                                      &trunk_id,
                                      &id));

    if ((0 == BCM_GPORT_IS_MODPORT(gport)) &&
        (0 == BCM_GPORT_IS_TRUNK(gport)) &&
        (0 == BCM_GPORT_IS_DEVPORT(gport))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Mirror destination parameter validate.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_validate(int unit, bcm_mirror_destination_t *mirror_dest)
{
    int id;

    SHR_FUNC_ENTER(unit);

    if (mirror_dest == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((mirror_dest->truncate != BCM_MIRROR_PAYLOAD_TRUNCATE) &&
        (mirror_dest->truncate !=
         BCM_MIRROR_PAYLOAD_TRUNCATE_AND_ZERO) &&
        (mirror_dest->truncate != BCM_MIRROR_PAYLOAD_DO_NOT_TRUNCATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) &&
        (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_L2)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Validate destination gport. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_gport_validate(unit, mirror_dest->gport));

    if ((mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) &&
        !(mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID) {
        if (BCM_GPORT_IS_MIRROR(mirror_dest->mirror_dest_id)) {
            id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
            /* Check Mirror destination id. */
            if (id < 0 || id >= MIRROR_DEST_NUM(unit)) {
                SHR_ERR_EXIT(SHR_E_BADID);
            }

            /* The destination already exists in pool. */
            if (MIRROR_DEST_REF_COUNT(unit, id) &&
                !(mirror_dest->flags & BCM_MIRROR_DEST_REPLACE)) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
        } else {
            SHR_ERR_EXIT(SHR_E_BADID);
        }
    }

    /* Chip specific destination validation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_destination_validate(unit, mirror_dest));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a Mirror destination and add it to destination pool.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_create(int unit, bcm_mirror_destination_t *mirror_dest)
{
    int id, rv = SHR_E_NONE;
    bcm_mirror_destination_t *mirror_dest_tmp = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate Mirror destination parameter. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_validate(unit, mirror_dest));

    if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_update(unit, mirror_dest));
    } else {
        if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID) {
            id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_destination_alloc(unit, &mirror_dest_tmp));
            /* Avoid duplicated Mirror destination. */
            rv = mirror_dest_search(unit,
                                    mirror_dest->gport,
                                    mirror_dest,
                                    mirror_dest_tmp);
            if (rv == SHR_E_NONE) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }

            /* Allocate Mirror destination id. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_id_alloc(unit, &id));
        }

        BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, id);
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_add(unit, mirror_dest));
    }

    /* Clear invalid flags. */
    id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
    MIRROR_DEST_DATA(unit, id)->flags &= ~BCM_MIRROR_DEST_WITH_ID;

exit:
    if (mirror_dest_tmp != NULL) {
        mirror_destination_free(&mirror_dest_tmp);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert decimal to hexadecimal.
 *
 * \param [in] digit Decimal digital.
 *
 * \retval Hexadecimal digital.
 */
static int
mirror_i2xdigit(int digit)
{
    digit &= 0xf;

    return (digit > 9) ? digit - 10 + 'a' : digit + '0';
}

/*!
 * \brief Convert MAC address to string.
 *
 * \param [in] macaddr MAC address.
 * \param [in] buf String.
 */
static void
mirror_fmt_macaddr(char *buf, bcm_mac_t macaddr)
{
    int i;

    if (buf == NULL) {
        return;
    }

    for (i = 0; i <= 5; i++) {
        *buf++ = mirror_i2xdigit(macaddr[i] >> 4);
        *buf++ = mirror_i2xdigit(macaddr[i]);
        *buf++ = ':';
    }

    *--buf = 0;
}

/*!
 * \brief Convert IPv6 address to string.
 *
 * \param [in] ipaddr IPv6 address.
 * \param [in] buf String.
 */
static void
mirror_fmt_ip6addr(char *buf, bcm_ip6_t ipaddr)
{
    if (buf == NULL) {
        return;
    }

    sal_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                (((uint16_t)ipaddr[0] << 8) | ipaddr[1]),
                (((uint16_t)ipaddr[2] << 8) | ipaddr[3]),
                (((uint16_t)ipaddr[4] << 8) | ipaddr[5]),
                (((uint16_t)ipaddr[6] << 8) | ipaddr[7]),
                (((uint16_t)ipaddr[8] << 8) | ipaddr[9]),
                (((uint16_t)ipaddr[10] << 8) | ipaddr[11]),
                (((uint16_t)ipaddr[12] << 8) | ipaddr[13]),
                (((uint16_t)ipaddr[14] << 8) | ipaddr[15]));
}

/*!
 * \brief Dump Mirror destination info.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination.
 */
static void
mirror_dest_dump(int unit, bcm_mirror_destination_t *mirror_dest)
{
    int i;
    char ip6_str[33];
    char mac_str[18];
    bcm_mirror_destination_t *mdest = NULL;

    mdest = mirror_dest;
    if (mdest == NULL) {
        return;
    }

    i = BCM_GPORT_MIRROR_GET(mdest->mirror_dest_id);
    LOG_CLI((BSL_META_U(unit,
                        "  Mirror dest: 0x%08x  Ref count: %4d\n"),
             mdest->mirror_dest_id,
             MIRROR_DEST_REF_COUNT(unit, i)));
    LOG_CLI((BSL_META_U(unit,
                        "              MTP Gport : 0x%08x\n"),
             mdest->gport));
    LOG_CLI((BSL_META_U(unit,
                        "              TOS       : 0x%02x\n"),
             mdest->tos));
    LOG_CLI((BSL_META_U(unit,
                        "              TTL       : 0x%02x\n"),
             mdest->ttl));
    LOG_CLI((BSL_META_U(unit,
                        "              IP Version: 0x%02x\n"),
             mdest->version));
    if (mdest->version == 4) {
        LOG_CLI((BSL_META_U(unit,
                            "              Src IP    : 0x%08x\n"),
                 mdest->src_addr));
        LOG_CLI((BSL_META_U(unit,
                            "              Dest IP   : 0x%08x\n"),
                 mdest->dst_addr));
    } else if (mdest->version == 6) {
        mirror_fmt_ip6addr(ip6_str, mdest->src6_addr);
        LOG_CLI((BSL_META_U(unit,
                            "              Src IP    : %-42s\n"),
                 ip6_str));
        mirror_fmt_ip6addr(ip6_str, mdest->dst6_addr);
        LOG_CLI((BSL_META_U(unit,
                            "              Dest IP   : %-42s\n"),
                 ip6_str));
    }
    mirror_fmt_macaddr(mac_str, mdest->src_mac);
    LOG_CLI((BSL_META_U(unit,
                        "              Src MAC   : %-18s\n"),
             mac_str));
    mirror_fmt_macaddr(mac_str, mdest->dst_mac);
    LOG_CLI((BSL_META_U(unit,
                        "              Dest MAC  : %-18s\n"),
             mac_str));
    LOG_CLI((BSL_META_U(unit,
                        "              Flow label: 0x%08x\n"),
             mdest->flow_label));
    LOG_CLI((BSL_META_U(unit,
                        "              TPID      : 0x%04x\n"),
             mdest->tpid));
    LOG_CLI((BSL_META_U(unit,
                        "              VLAN      : 0x%04x\n"),
             mdest->vlan_id));
    LOG_CLI((BSL_META_U(unit,
                        "              Flags     :")));
    if (mdest->flags & BCM_MIRROR_DEST_REPLACE) {
        LOG_CLI((BSL_META_U(unit,
                            "  Replace")));
    }
    if (mdest->flags & BCM_MIRROR_DEST_WITH_ID) {
        LOG_CLI((BSL_META_U(unit,
                            "  ID provided")));
    }
    if (mdest->flags & BCM_MIRROR_DEST_TUNNEL_L2) {
        LOG_CLI((BSL_META_U(unit,
                            "  L2 tunnel")));
    }
    if (mdest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) {
        if (mdest->flags & BCM_MIRROR_DEST_TUNNEL_WITH_SEQ) {
            LOG_CLI((BSL_META_U(unit,
                            "  IP GRE tunnel with sequence number")));
        } else {
            LOG_CLI((BSL_META_U(unit,
                            "  IP GRE tunnel")));
        }
    }
    if (mdest->flags & BCM_MIRROR_DEST_PAYLOAD_UNTAGGED) {
        LOG_CLI((BSL_META_U(unit,
                            "  Untagged payload")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
}

/*!
 * \brief Dump all valid Mirror destinations.
 *
 * \param [in] unit Unit number.
 */
static void
mirror_dest_dump_all(int unit)
{
    int i;
    for (i = 0; i < MIRROR_DEST_NUM(unit); i++) {
        if (MIRROR_DEST_REF_COUNT(unit, i) == 0) {
            continue;
        }
        mirror_dest_dump(unit, MIRROR_DEST_DATA(unit, i));
    }
}

/*!
 * \brief Get All Mirror destination ids applied on a source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [out] dest_id Mirror destination id array.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_id_get_by_source(int unit, bcmint_mirror_source_t *source,
                             int *dest_id)
{
    int *session_id = NULL, mod_mirror_dest_id = 0, enable = 0, i = 0,
        alloc_sz = 0, count = 0, ep_recirc_instance_id = 0;
    bool cosq_mod = false;
    SHR_FUNC_ENTER(unit);

    cosq_mod = MIRROR_SOURCE_IS_COSQ_MOD_TYPE(source->type);

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    session_id = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(session_id, SHR_E_MEMORY);
    sal_memset(session_id, 0xff, alloc_sz);

    if (cosq_mod) {
        for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
            mod_mirror_dest_id = MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i];
            if (mod_mirror_dest_id == -1) {
                continue;
            }
            dest_id[count] = mod_mirror_dest_id;
            count++;
        }
    } else if (source->type == BCMINT_MIRROR_ST_F_EP_RECIRC) {
        /*
         * Get instance id from profile entry.
         * For EP Recirculate, mirror session is fixed to index 0.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_get
                (unit, &ep_recirc_instance_id));
        if (ep_recirc_instance_id != -1) {
            dest_id[0] =
                MIRROR_SESSION(unit, ep_recirc_instance_id)->mirror_dest_id;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_enable_get(unit, source, session_id, &enable));
        for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
            if (session_id[i] != -1 && (enable & (1 << i))) {
                if (MIRROR_SESSION_REF_COUNT(unit, session_id[i]) == 0) {
                    continue;
                }
                dest_id[i] = MIRROR_SESSION(unit, session_id[i])->mirror_dest_id;
            }
        }
    }

exit:
    SHR_FREE(session_id);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all Mirror destination ids that need to be checked.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [out] dest_id Mirror destination id array.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_id_get_all(int unit, bcmint_mirror_source_t *source, int *dest_id)
{
    bcmint_mirror_source_t source_tmp;
    SHR_FUNC_ENTER(unit);

    source_tmp = *source;

    /*
     * Unlike port and VP mirroring, each trace/drop event does not
     * have its own mirror container field on source table.
     * Eight containers are shared by all trace events, and only one
     * container(any of 8) is supported for drop event, so need to get
     * all applied destinations on all containers for trace/drop event.
     */
    if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
        if (source_tmp.type == BCMINT_MIRROR_ST_F_TRACE_EVENT) {
            source_tmp.trace_event = bcmPktTraceEventCount;
        } else if (source_tmp.type == BCMINT_MIRROR_ST_F_DROP_EVENT) {
            source_tmp.drop_event = bcmPktDropEventCount;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source_tmp, dest_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check Mirror source dest id.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror source.
 * \param [in] dest Mirror destinations already applied.
 * \param [in] dest_id Mirror destination id to be attached.
 * \param [out] status Mirror container status.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_source_dest_id_check(int unit, bcmint_mirror_source_t *source,
                            int *dest, int dest_id, int *status)
{
    int i, *dest_tmp = NULL, size = 0;
    bool egr = false, free = false;
    bcmint_mirror_source_t source_tmp;
    int rv, count = 0, session_id = 0;
    SHR_FUNC_ENTER(unit);

    source_tmp = *source;
    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);

    if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
        size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
        dest_tmp = sal_alloc(size, "ltswMirrorDest");
        SHR_NULL_CHECK(dest_tmp, SHR_E_MEMORY);
        sal_memset(dest_tmp, 0xff, size);

        if (source_tmp.type == BCMINT_MIRROR_ST_F_TRACE_EVENT ||
            source_tmp.type == BCMINT_MIRROR_ST_F_DROP_EVENT) {
            /*
             * Unlike port and VP mirroring, each trace/drop event does not
             * have its own mirror container field on source table.
             * Eight containers are shared by all trace events, and only one
             * container(any of 8) is supported for drop event, so need to get
             * all applied destinations on all containers for trace/drop event.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_id_get_all(unit, &source_tmp, dest_tmp));
        } else {
            sal_memcpy(dest_tmp, dest, size);
        }
        for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
            status[i] = MIRROR_FREE;
            /* Don't support such source type. */
            if (!(MIRROR_CTRL(unit)->mc_st_flags[i] & source->type)) {
                status[i] = MIRROR_UNAVAILABLE;
                continue;
            }

            /* Ingress/egress does not match. */
            if (MIRROR_CONTAINER_REF_COUNT(unit, i) &&
                MIRROR_CONTAINER(unit, i)->egr != egr) {
                status[i] = MIRROR_OCCUPIED;
                continue;
            }

            /*
             * Check if it is a duplicated operation on same source
             * So use dest id applied on local container of source.
             * EP Recirculate profiles can use the same dest id.
             */
            if ((MIRROR_CONTAINER_REF_COUNT(unit, i) &&
                 dest[i] == dest_id) &&
                (MIRROR_CONTAINER(unit, i)->egr == egr) &&
                (source->type != BCMINT_MIRROR_ST_F_EP_RECIRC)) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }

            /* Container has been used for other destination. */
            if (MIRROR_CONTAINER_REF_COUNT(unit, i) &&
                (dest_tmp[i] != -1 && dest_tmp[i] != dest_id)) {
                status[i] = MIRROR_OCCUPIED;
            } else if ((source->type == BCMINT_MIRROR_ST_F_TRACE_EVENT ||
                        source->type == BCMINT_MIRROR_ST_F_DROP_EVENT ||
                        source->type == BCMINT_MIRROR_ST_F_EP_RECIRC) &&
                       (MIRROR_CONTAINER_REF_COUNT(unit, i) &&
                        (dest_tmp[i] != -1 && dest_tmp[i] == dest_id))) {
                status[i] = MIRROR_FREE;
                free = true;
                continue;
            }

            /*
             * Some source types support to configure only one container,
             * like ingress sampler, drop event, etc.
             */
            if ((source->type == BCMINT_MIRROR_ST_F_SAMPLE_INGRESS ||
                 source->type == BCMINT_MIRROR_ST_F_SAMPLE_FLEX ||
                 source->type == BCMINT_MIRROR_ST_F_DROP_EVENT) &&
                dest_tmp[i] != -1 && dest_tmp[i] != dest_id) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }

            if (MIRROR_CONTAINER_REF_COUNT(unit, i) &&
                MIRROR_CONTAINER(unit, i)->source != 0) {
                /*
                 * If two rules(different source types with different
                 * destinations) share the same container, when one packet
                 * hits them both, there will be only one rule could
                 * generate mirror copy since one container can output only
                 * ONE MTP per packet.
                 * To avoid this case, the container cannot be shared between
                 * different source types.
                 */
                if (source->type != MIRROR_CONTAINER(unit, i)->source) {
                    status[i] = MIRROR_OCCUPIED;
                    continue;
                } else {
                    /*
                     * As for some source types(like IFP), if two rules
                     * (same source type with different destinations,
                     * like IFP rule1, IFP rule 2) share the same container,
                     * when one packet hits them both, there will
                     * be only one rule could generate mirror copy since
                     * one container can output only ONE MTP per packet.
                     * To avoid this case, the container cannot be shared in
                     * such source type.
                     * Port mirroring has no such issue since one packet
                     * cannot hits two different ingress ports.
                     * Also does VP mirroring, etc.
                     */

                    /*
                     * Different IFP entries with same dest id can share
                     * a same container id.
                     */
                    if (source->type == BCMINT_MIRROR_ST_F_FP_ING ||
                        source->type == BCMINT_MIRROR_ST_F_FP_EGR) {
                        rv = mirror_session_id_get_by_dest(unit, source,
                                                           dest_id,
                                                           i,
                                                           &count,
                                                           &session_id);
                        if (SHR_SUCCESS(rv) && count != 0) {
                            status[i] = MIRROR_FREE;
                            free = true;
                            continue;
                        }
                    }

                    if (((BCMINT_MIRROR_ST_F_VP_ING |
                          BCMINT_MIRROR_ST_F_PORT_EGR |
                          BCMINT_MIRROR_ST_F_PORT_ING |
                          BCMINT_MIRROR_ST_F_SAMPLE_INGRESS)
                          & source->type) == 0) {
                        status[i] = MIRROR_OCCUPIED;
                        continue;
                    }
                }
            }

            if (dest_tmp[i] == -1) {
                status[i] = MIRROR_FREE;
                free = true;
            }
        }
        if (free == false) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    SHR_FREE(dest_tmp);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate input parameters for mirror destination attach/detach.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port id.
 * \param [in] flags BCM_MIRROR_PORT_*.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_port_dest_params_validate(int unit, bcm_port_t port, uint32 flags)
{
    int vp = -1;
    SHR_FUNC_ENTER(unit);

    if (port != -1) {
        if (BCM_GPORT_IS_SET(port)) {
            /* Get vp from gport. */
            if (BCM_GPORT_IS_MPLS_PORT(port) ||
                BCM_GPORT_IS_FLOW_PORT(port)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));
            }
            if (vp == -1) {
                /* Get local port from gport. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_validate(unit, port, &port));
            }
        } else {
            /* Validate local port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
        }
    }

    /* Don't support egress true mirroring. */
    if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!(flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((flags & BCM_MIRROR_PORT_SFLOW) && (flags & BCM_MIRROR_PORT_EGRESS)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a Mirror session id.
 *
 * Mirror flow:
 * Source --> Container --> Session ---> Encap
 *
 * Mirror session model: exclusive and shared.
 * Exclusive model, indicates Mirror session is exclusive to Mirror container.
 * In this model, the Mirror session is divided into multiple different Mirror
 * session arrays, and each Mirror session array can be (and only be) used by
 * one Mirror container. The Mirror container is always one-to-one mapping with
 * Mirror session array. Each Mirror session array has several entries
 * (16 in TD4), which is called Mirror instance.
 *
 * The mapping of container to session array in TD4 is like:
 * Container 'n' is used as index to select one of 16 instance entries in
 * session array 'n'.
 *      Container           Session array
 *      +-------+           +------------+
 *      |   0   |---------> |            |
 *      |-------|           |            |
 *      |   ..  |           |            |
 *      |-------|           |------------|
 *      |   ..  |           | instance 0 |
 *      |-------|           |    ...     |
 *      |   n   |---------> |    ...     |
 *      |-------|           | instance 15|
 *      |   ..  |           |------------|
 *      |-------|           |            |
 *      |   7   |---------> |            |
 *      +-------+           +------------+
 *
 * Shared model, indicates session array num is 1, and it can be shared by
 * different containers.
 * The mapping of container to session array(in TH4) is like:
 *      Container           Session array
 *      +-------+
 *      |   0   |---+
 *      |-------|   |
 *      |   ..  |   |
 *      |-------|   |       +------------+
 *      |   ..  |   +-----> | instance 0 |
 *      |-------|           |    ...     |
 *      |   n   |---------> |    ...     |
 *      |-------|           | instance 7 |
 *      |   ..  |   +-----> +------------+
 *      |-------|   |
 *      |   4   |---+
 *      +-------+
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] status Mirror source container status.
 * \param [in] dest_id Mirror destination id.
 * \param [out] session_id Mirror session id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_id_alloc(int unit, bcmint_mirror_source_t *source,
                        int *status,
                        int dest_id,
                        int *session_id)
{
    int i, j, session_id_tmp = -1, free_session = -1;
    bool egr = false, found = false;
    bcmint_mirror_session_model_t model = MIRROR_SESSION_MODEL(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(source, SHR_E_PARAM);
    SHR_NULL_CHECK(session_id, SHR_E_PARAM);

    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);

    /*
     * Traverse Mirror session array from max to min for egress mirroring,
     * in order to use it more fully.
     */
    /* Traverse Mirror session array. */
    for (i = egr ? MIRROR_SESSION_ARRAY_NUM(unit) - 1 : 0;
         egr ? i >= 0 : i < MIRROR_SESSION_ARRAY_NUM(unit);
         egr ? i-- : i++) {

        if (model == bcmintMirrorSessionModelExclusive &&
            status[i] != MIRROR_FREE) {
            continue;
        }

        /*
         * Traverse Mirror instance in one array to find a free one
         * or matched one.
         */
        session_id_tmp = i * MIRROR_INSTANCE_NUM(unit);
        for (j = 0; j < MIRROR_INSTANCE_NUM(unit); j++, session_id_tmp++) {
            /* Instance 0 is reserved. */
            if (MIRROR_INSTANCE_ZERO_RSVD(unit) && j == 0) {
                continue;
            }
            if (MIRROR_SESSION_REF_COUNT(unit, session_id_tmp) &&
                MIRROR_SESSION(unit, session_id_tmp)->mirror_dest_id ==
                dest_id &&
                MIRROR_SESSION(unit, session_id_tmp)->egr == egr) {
                /* Find a matched session. */
                found = true;
                break;
            } else if (model == bcmintMirrorSessionModelExclusive &&
                       source->type == BCMINT_MIRROR_ST_F_PORT_EGR &&
                       MIRROR_SESSION_REF_COUNT(unit, session_id_tmp) &&
                       MIRROR_SESSION(unit, session_id_tmp)->mirror_dest_id !=
                       dest_id) {
                /* Container is occupied by other ports. */
                break;
            } else if (MIRROR_SESSION_REF_COUNT(unit, session_id_tmp) == 0 &&
                       free_session == -1) {
                /* Record first free session. */
                free_session = session_id_tmp;
            }
        }
        if (found) {
            break;
        }
    }
    if (found) {
        *session_id = session_id_tmp;
    } else if (free_session != -1) {
        *session_id = free_session;
    } else {
        /* All sessions are occupied. */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a Mirror session node to database.
 *
 * \param [in] unit Unit number.
 * \param [in] session_id Mirror session id.
 * \param [in] egr Indicate egress mirroring.
 * \param [in] cosq_mod Indicate CoSQ MOD mirroring.
 * \param [in] encap_id Encap id.
 * \param [in] dest_id Mirror destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_add(int unit, int session_id, bool egr,
                   bool cosq_mod, int encap_id, int dest_id)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_SESSION_ID_CHECK(unit, session_id);

    MIRROR_SESSION_REF_COUNT(unit, session_id)++;
    MIRROR_SESSION(unit, session_id)->egr = egr;
    MIRROR_SESSION(unit, session_id)->cosq_mod = cosq_mod;
    MIRROR_SESSION(unit, session_id)->encap_id = encap_id;
    MIRROR_SESSION(unit, session_id)->mirror_dest_id = dest_id;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update a Mirror session node in database.
 *
 * \param [in] unit Unit number.
 * \param [in] session_id Mirror session id.
 * \param [in] egr Indicate egress mirroring.
 * \param [in] cosq_mod Indicate CoSQ MOD mirroring.
 * \param [in] encap_id Encap id.
 * \param [in] dest_id Mirror destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_update(int unit, int session_id, bool egr,
                      bool cosq_mod, int encap_id, int dest_id)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_SESSION_ID_CHECK(unit, session_id);

    MIRROR_SESSION(unit, session_id)->egr = egr;
    MIRROR_SESSION(unit, session_id)->cosq_mod = cosq_mod;
    MIRROR_SESSION(unit, session_id)->encap_id = encap_id;
    MIRROR_SESSION(unit, session_id)->mirror_dest_id = dest_id;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Mirror session node from database.
 *
 * \param [in] unit Unit number.
 * \param [in] session_id Mirror session id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_delete(int unit, int session_id)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_SESSION_ID_CHECK(unit, session_id);

    if (MIRROR_SESSION_REF_COUNT(unit, session_id) <= 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    MIRROR_SESSION_REF_COUNT(unit, session_id)--;

    if (MIRROR_SESSION_REF_COUNT(unit, session_id) == 0) {
        MIRROR_SESSION(unit, session_id)->egr = false;
        MIRROR_SESSION(unit, session_id)->cosq_mod = false;
        MIRROR_SESSION(unit, session_id)->encap_id = -1;
        MIRROR_SESSION(unit, session_id)->mirror_dest_id = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search Mirror session id with given parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest Mirror destinations already applied. NULL means no check.
 * \param [in] dest_id Mirror destination id.
 * \param [out] session_id Mirror session id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_id_search(int unit, bcmint_mirror_source_t *source,
                         int *dest,
                         int dest_id,
                         int *session_id)
{
    int i, j, session_id_tmp = -1;
    bool egr = false, found = false;
    bcmint_mirror_session_model_t model = MIRROR_SESSION_MODEL(unit);

    SHR_FUNC_ENTER(unit);

    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);

    /* For EP Recirculate, mirror session is fixed to index 0. */
    session_id_tmp = 0;;
    if (source->type == BCMINT_MIRROR_ST_F_EP_RECIRC) {
        for (i = 0; i < MIRROR_INSTANCE_NUM(unit); i++, session_id_tmp++) {
            /* Instance 0 is reserved. */
            if (MIRROR_INSTANCE_ZERO_RSVD(unit) && i == 0) {
                continue;
            }
            if (MIRROR_SESSION_REF_COUNT(unit, session_id_tmp)) {
                /* Find a matched session. */
                found = true;
                break;
            }
        }
        if (found) {
            *session_id = session_id_tmp;
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }


    for (i = egr ? MIRROR_SESSION_ARRAY_NUM(unit) - 1 : 0;
         egr ? i >= 0 : i < MIRROR_SESSION_ARRAY_NUM(unit);
         egr ? i-- : i ++) {

        if (model == bcmintMirrorSessionModelExclusive) {
            /* Skip unused container. */
            if (MIRROR_CONTAINER_REF_COUNT(unit, i) == 0) {
                continue;
            }
            /*
             * Skip the container where applied dest_id
             * and egress is not matched.
             */
            if (dest &&
                !(MIRROR_CONTAINER(unit, i)->egr == egr &&
                  dest[i] == dest_id)) {
                continue;
            }
        }

        /* Traverse Mirror session. */
        session_id_tmp = i * MIRROR_INSTANCE_NUM(unit);
        for (j = 0; j < MIRROR_INSTANCE_NUM(unit); j++, session_id_tmp++) {
            /* Instance 0 is reserved. */
            if (MIRROR_INSTANCE_ZERO_RSVD(unit) && j == 0) {
                continue;
            }
            if (MIRROR_SESSION_REF_COUNT(unit, session_id_tmp) &&
                MIRROR_SESSION(unit, session_id_tmp)->mirror_dest_id ==
                dest_id &&
                MIRROR_SESSION(unit, session_id_tmp)->egr == egr) {
                /* Find a matched session. */
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (found) {
        *session_id = session_id_tmp;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a Mirror session.
 *
 * This function is to allocate Mirror session id, add Mirror session node
 * into S/W database and Mirror session entry into H/W table
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] mirror_dest Mirror destination structure.
 * \param [in] encap_id Mirror encap id, -1 indicates no encapsulation.
 * \param [out] session_id Mirror session id.
 * \param [out] alloc Indicate a new mirror session is allocated. For an update
 *                    operation, it is false.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_create(int unit, bcmint_mirror_source_t *source,
                      bcm_mirror_destination_t *mirror_dest,
                      int encap_id, int *session_id, bool *alloc)
{
    int session_id_tmp = -1, dest_id = -1, *dest = NULL, size = 0;
    bool cosq_mod = false;
    bool egr = false, session_allocated = false, update = false;
    bcmint_mirror_session_entry_t session;
    bcm_port_t local_port, sys_port;
    int *status = NULL, rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);
    cosq_mod = MIRROR_SOURCE_IS_COSQ_MOD_TYPE(source->type);
    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);

    dest = sal_alloc(size, "ltswMirrorDest");
    status = sal_alloc(size, "ltswMirrorContainerStatus");

    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    SHR_NULL_CHECK(status, SHR_E_MEMORY);

    sal_memset(dest, 0xff, size);
    sal_memset(status, 0, size);

    /* Get destinations applied on the source. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, source, dest));

    if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) {
        /* Search Mirror session id. */
        rv = mirror_session_id_search(unit,
                                      source,
                                      dest,
                                      dest_id,
                                      &session_id_tmp);
        if (SHR_SUCCESS(rv)) {
            update = true;
        } else if (rv != SHR_E_NOT_FOUND) {
            /* When mirror session is not found, create a new one. */
            SHR_ERR_EXIT(rv);
        }
    }

    if (update) {
        /* Update Mirror session in S/W database. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_session_update(unit,
                                   session_id_tmp,
                                   egr,
                                   cosq_mod,
                                   encap_id,
                                   dest_id));
    } else {
        /* Mirror container dest id check. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_source_dest_id_check(unit, source, dest, dest_id, status));
        /* Allocate Mirror session id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_session_id_alloc(unit,
                                     source,
                                     status,
                                     dest_id,
                                     &session_id_tmp));

        /* Add Mirror session to S/W database. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_session_add(unit, session_id_tmp, egr,
                                cosq_mod, encap_id, dest_id));
        session_allocated = true;

    }

    /* Get local port from Mirror gport. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_local_port_get(unit,
                                         mirror_dest->gport,
                                         NULL,
                                         &local_port));

    rv = bcmi_ltsw_port_to_sys_port(unit, mirror_dest->gport, &sys_port);
    /* Not all devices support system port.*/
    if (SHR_E_UNAVAIL == rv) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(&session, 0, sizeof(bcmint_mirror_session_entry_t));
    session.valid = true;
    session.egr = egr;
    session.cosq_mod = cosq_mod;
    /* Index 0 is reserved for non-encap during mirror_init. */
    session.encap_ena = (encap_id == 0) ? false : true;
    session.encap_id = encap_id;
    session.local_port = local_port;
    session.sys_port = sys_port;
    session.mc_cos = mirror_dest->cosq;
    session.entry_pri = mirror_dest->duplicate_pri;
    session.truncate = mirror_dest->truncate;
    session.timestamp_mode = mirror_dest->timestamp_mode;
    session.multi_dip_group_id = mirror_dest->multi_dip_group;

    /* Add Mirror session entry to H/W table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_session_add(unit, session_id_tmp, &session));

    /* Return session id for other usage. */
    *session_id = session_id_tmp;
    *alloc = session_allocated;
exit:
    if (SHR_FUNC_ERR()) {
        if (session_allocated) {
            (void)mirror_session_delete(unit, session_id_tmp);
        }
    }
    SHR_FREE(dest);
    SHR_FREE(status);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a Mirror session.
 *
 * This function is to delete Mirror session entry from S/W database, if ref_cnt
 * of this session is zero, delete Mirror session entry from H/W table
 *
 * \param [in] unit Unit number.
 * \param [in] session_id Mirror session id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_destroy(int unit, int session_id)
{
    bool egr, cosq_mod;
    SHR_FUNC_ENTER(unit);

    egr = MIRROR_SESSION(unit, session_id)->egr;
    cosq_mod = MIRROR_SESSION(unit, session_id)->cosq_mod;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_session_delete(unit, session_id));

    if (MIRROR_SESSION_REF_COUNT(unit, session_id) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mirror_session_delete(unit, session_id, egr, cosq_mod));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mirror session id by mirror dest id.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest_id Mirror destination id.
 * \param [in] container_id Search in this container, -1 means not care.
 * \param [out] count The count of associated mirror sessions.
 * \param [out] session_id Mirror session id array.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_session_id_get_by_dest(int unit, bcmint_mirror_source_t *source,
                              int dest_id,
                              int container_id,
                              int *count,
                              int *session_id)
{
    int i, j, session_id_tmp = -1, cnt = 0;
    bool egr = false;
    bcmint_mirror_session_model_t model = MIRROR_SESSION_MODEL(unit);

    SHR_FUNC_ENTER(unit);

    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);

    for (i = egr ? MIRROR_SESSION_ARRAY_NUM(unit) - 1 : 0;
         egr ? i >= 0 : i < MIRROR_SESSION_ARRAY_NUM(unit);
         egr ? i-- : i ++) {
        if (model == bcmintMirrorSessionModelExclusive) {
            /* Skip unused container. */
            if (MIRROR_CONTAINER_REF_COUNT(unit, i) == 0) {
                continue;
            }
            /* Search in specific container.*/
            if (container_id != -1 && i != container_id) {
                continue;
            }
        }

        /* Traverse Mirror session. */
        session_id_tmp = i * MIRROR_INSTANCE_NUM(unit);
        for (j = 0; j < MIRROR_INSTANCE_NUM(unit); j++, session_id_tmp++) {
            /* Instance 0 is reserved. */
            if (MIRROR_INSTANCE_ZERO_RSVD(unit) && j == 0) {
                continue;
            }
            if (MIRROR_SESSION_REF_COUNT(unit, session_id_tmp) &&
                MIRROR_SESSION(unit, session_id_tmp)->mirror_dest_id ==
                dest_id &&
                MIRROR_SESSION(unit, session_id_tmp)->egr == egr) {
                if (session_id) {
                    session_id[cnt] = session_id_tmp;
                }
                cnt++;
                /* One session array does not allow two same dest. */
                break;
            }
        }
    }
    if (cnt == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    *count = cnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump Mirror sessions.
 *
 * \param [in] unit Unit number.
 */
static void
mirror_session_dump(int unit)
{
    int i;

    for (i = 0; i < MIRROR_SESSION_NUM(unit); i++) {
        if (MIRROR_SESSION_REF_COUNT(unit, i) == 0) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "  Mirror session: %4d  Ref count: %4d\n"),
                 i, MIRROR_SESSION_REF_COUNT(unit, i)));
        LOG_CLI((BSL_META_U(unit,
                            "              EGR            : %s\n"),
                 MIRROR_SESSION(unit, i)->egr ? "True" : "False"));
        LOG_CLI((BSL_META_U(unit,
                            "              Encap id       : %4d\n"),
                 MIRROR_SESSION(unit, i)->encap_id));
        LOG_CLI((BSL_META_U(unit,
                            "              Mirror dest id : %4d\n"),
                 MIRROR_SESSION(unit, i)->mirror_dest_id));
    }
}

/*!
 * \brief Allocate a Mirror container id.
 *
 * Mirror container is a container that can carry multiple mirror rules.
 * It can be enabled per-source(like port, svp, fp action, etc), but one mirror
 * container can only be used for a single source each time for a packet.
 * That means when a mirror container is enabled on several sources, and one
 * packet hit them at the same time, only one source will take effect.
 * The number of containers determines how many different mirror copies
 * that chip can generate per packet.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest_id Mirror destination id.
 * \param [out] container_id Mirror container id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_container_id_alloc(int unit, bcmint_mirror_source_t *source, int dest_id,
                          int *container_id)
{
    bcm_mirror_destination_t *mirror_dest = NULL;
    int i, free_container = -1;
    bool egr = false;
    int *dest = NULL, alloc_sz = 0;

    SHR_FUNC_ENTER(unit);

    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);
    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    /* Get the destinations applied on containers. */
    (void)mirror_dest_id_get_by_source(unit, source, dest);

    /* Traverse Mirror container. */
    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {

        /* Don't support such source type. */
        if (!(source->type & MIRROR_CTRL(unit)->mc_st_flags[i])) {
            continue;
        }

        /* Container is occupied by other soucre. */
        if (MIRROR_CONTAINER_REF_COUNT(unit, i)) {
            if (~(source->type) & MIRROR_CONTAINER(unit, i)->source) {
                continue;
            }
        }

        if (MIRROR_CONTAINER_REF_COUNT(unit, i)) {
            /* Ingress/egress does not match. */
            if (MIRROR_CONTAINER(unit, i)->egr != egr) {
                continue;
            }

            if (MIRROR_CONTAINER(unit, i)->egr) {
                if (MIRROR_CONTAINER(unit, i)->dest_id != dest_id) {
                    continue;
                }
            }

            /* Container has been used for other Mirror destination. */
            if (dest[i] != -1 && dest[i] != dest_id) {
                continue;
            } else if (dest[i] == dest_id) {
                /* Match same container. */
                mirror_dest = MIRROR_DEST_DATA(unit, dest_id);
                if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) {
                    *container_id = i;
                    SHR_EXIT();
                } else {
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                }
            } else if (dest[i] == -1) {
                /*
                 * Each port has its own container settings, so even if the
                 * container has been referenced by a port, another port
                 * can still config its own container value.
                 */
                if ((source->type == BCMINT_MIRROR_ST_F_PORT_ING) ||
                    (source->type == BCMINT_MIRROR_ST_F_PORT_EGR) ||
                    (source->type == BCMINT_MIRROR_ST_F_FP_ING)) {
                    if (free_container == -1) {
                        /* Record first free container. */
                        free_container = i;
                    }
                }
            }
        } else if (free_container == -1) {
             /* Record first free container. */
            free_container = i;
        }
    }

    /* Container is used up. */
    if (free_container == -1) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    *container_id = free_container;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Mirror container software info.
 *
 * \param [in] unit Unit number.
 * \param [in] container_id Mirror container id.
 * \param [in] egr Used for egress mirroring or not.
 * \param [in] source_type Source type.
 */
static void
mirror_container_set(int unit, bcmint_mirror_session_model_t session_model,
                     int container_id, int dest_id, bool egr,
                     uint32_t source_type)
{
    MIRROR_CONTAINER(unit, container_id)->egr = egr;
    MIRROR_CONTAINER_REF_COUNT(unit, container_id)++;
    MIRROR_CONTAINER(unit, container_id)->source |= source_type;
    if (egr &&
        (session_model == bcmintMirrorSessionModelShared)) {
        MIRROR_CONTAINER(unit, container_id)->dest_id = dest_id;
    }

    return;
}

/*!
 * \brief Free a Mirror container id.
 *
 * \param [in] unit Unit number.
 * \param [in] container_id Mirror container id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_container_id_free(int unit, int container_id)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_CONTAINER_ID_CHECK(unit, container_id);

    if (MIRROR_CONTAINER_REF_COUNT(unit, container_id) <= 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    MIRROR_CONTAINER_REF_COUNT(unit, container_id)--;
    if (MIRROR_CONTAINER_REF_COUNT(unit, container_id) == 0) {
        MIRROR_CONTAINER(unit, container_id)->egr = 0;
        MIRROR_CONTAINER(unit, container_id)->source = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search Mirror container id.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest_id Mirror destination id.
 * \param [out] container_id Mirror container id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_container_id_search(int unit, bcmint_mirror_source_t *source,
                           int dest_id, int *container_id)
{
    int i;
    bool egr = false;
    int *dest = NULL, alloc_sz = 0;

    SHR_FUNC_ENTER(unit);

    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);
    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    /* Get the destinations applied on containers. */
    (void)mirror_dest_id_get_by_source(unit, source, dest);

    /* Traverse Mirror container. */
    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {

        if (MIRROR_CONTAINER_REF_COUNT(unit, i)) {
            if (MIRROR_CONTAINER(unit, i)->egr == egr &&
                dest[i] == dest_id) {
                break;
            }
        }
    }

    if (i >= MIRROR_CONTAINER_NUM(unit)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    *container_id = i;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable a Mirror container on an event source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] container_id Mirror container id.
 * \param [in] session_id Mirror session id associated with the container.
 * \param [in] enable Enable/Disable container.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_event_container_enable_set(int unit, bcmint_mirror_source_t *source,
                                  int container_id, int session_id, bool enable)
{
    bcmint_mirror_source_t source_tmp;
    int *instance = NULL, ena = 0, mc_bmp = 0, size = 0;
    SHR_FUNC_ENTER(unit);

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    instance = sal_alloc(size, "ltswMirrorInstance");
    SHR_NULL_CHECK(instance, SHR_E_MEMORY);
    sal_memset(instance, 0xff, size);

    source_tmp = *source;
    if (source_tmp.type == BCMINT_MIRROR_ST_F_TRACE_EVENT) {
        source_tmp.trace_event = bcmPktTraceEventCount;
    } else if (source_tmp.type == BCMINT_MIRROR_ST_F_DROP_EVENT) {
        source_tmp.drop_event = bcmPktDropEventCount;
    }
    /* Get all containers' status. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_enable_get(unit,
                                     &source_tmp,
                                     &ena,
                                     instance));
    if (enable) {
        mc_bmp = 1 << container_id;
        ena = 1 << container_id;
        instance[container_id] = session_id % MIRROR_INSTANCE_NUM(unit);
    } else {
        mc_bmp = 1 << container_id;
        ena = 0;
        /*
         * For trace and drop event mirroring, Mirror container is shared,
         * Only when container is not used by any events, we can clear it.
         */
        if (MIRROR_CONTAINER_REF_COUNT(unit, container_id) == 1) {
            instance[container_id] = 0;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_enable_set(unit,
                                     source,
                                     mc_bmp,
                                     ena,
                                     instance));
exit:
    SHR_FREE(instance);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable a Mirror container on a source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] container_id Mirror container id.
 * \param [in] session_id Mirror session id associated with the container.
 * \param [in] enable Enable/Disable container.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_container_enable_set(int unit, bcmint_mirror_source_t *source,
                            int container_id, int session_id, bool enable)
{
    int *instance = NULL, ena = 0, mc_bmp = 0, size = 0;
    SHR_FUNC_ENTER(unit);

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    instance = sal_alloc(size, "ltswMirrorInstance");
    SHR_NULL_CHECK(instance, SHR_E_MEMORY);
    sal_memset(instance, 0xff, size);

    if (!(source->type & MIRROR_CTRL(unit)->mc_st_flags[container_id])) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (source->type == BCMINT_MIRROR_ST_F_TRACE_EVENT ||
        source->type == BCMINT_MIRROR_ST_F_DROP_EVENT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_event_container_enable_set(unit,
                                               source,
                                               container_id,
                                               session_id,
                                               enable));
    } else {
        if (enable) {
            mc_bmp = 1 << container_id;
            ena = 1 << container_id;
            instance[container_id] = session_id % MIRROR_INSTANCE_NUM(unit);
        } else {
            mc_bmp = 1 << container_id;
            ena = 0;
            instance[container_id] = 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mirror_enable_set(unit,
                                         source,
                                         mc_bmp,
                                         ena,
                                         instance));
    }


exit:
    SHR_FREE(instance);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get container enable status on a source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [out] session_id Mirror session ids applied on all containers.
 * \param [out] enable Container is enabled, bit n with 1 indicates container n
 *                     is enabled.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_container_enable_get(int unit, bcmint_mirror_source_t *source,
                            int *session_id, int *enable)
{
    int i = 0, ena = 0, *instance = NULL, size = 0;
    bool instance_found = false;
    SHR_FUNC_ENTER(unit);

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    instance = sal_alloc(size, "ltswMirrorInstance");
    SHR_NULL_CHECK(instance, SHR_E_MEMORY);
    sal_memset(instance, 0xff, size);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_enable_get(unit,
                                     source,
                                     &ena,
                                     instance));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        /* Don't support such source type. */
        if (!(source->type & MIRROR_CTRL(unit)->mc_st_flags[i])) {
            continue;
        }
        instance_found = true;
        if (ena & (1 << i)) {
            if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
                session_id[i] = i * MIRROR_INSTANCE_NUM(unit) + instance[i];
            } else if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelShared) {
                session_id[i] = instance[i];
            }
        }
    }
    *enable = ena;

    /* No instance support the source type can be found. */
    if (!instance_found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }


exit:
    SHR_FREE(instance);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump Mirror containers.
 *
 * \param [in] unit Unit number.
 */
static void
mirror_container_dump(int unit)
{
    int i;

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (MIRROR_CONTAINER_REF_COUNT(unit, i) == 0) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "  Mirror container: %4d  Ref count: %4d\n"),
                 i, MIRROR_CONTAINER_REF_COUNT(unit, i)));
        LOG_CLI((BSL_META_U(unit,
                            "              EGR            : %s\n"),
                 MIRROR_CONTAINER(unit, i)->egr ? "True" : "False"));
        LOG_CLI((BSL_META_U(unit,
                            "              Source         : %8x\n"),
                 MIRROR_CONTAINER(unit, i)->source));
    }
}

/*!
 * \brief Recover Mirror-on-drop destination pool.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_mod_recover(int unit)
{
    int dest_id = -1;
    bcm_mirror_destination_t *mirror_dest = NULL;
    bcmi_ltsw_cosq_mod_dest_t mod_dest;
    int my_modid;

    SHR_FUNC_ENTER(unit);

    dest_id = MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[0];
    /* No MOD destination was created, return success. */
    if (dest_id == -1) {
        SHR_EXIT();
    }

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get my modid. */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));

    /* Get mirror-on-drop destination. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_dest_get(unit, &mod_dest));

    mirror_dest->flags = BCM_MIRROR_DEST_WITH_ID;
    BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, dest_id);
    BCM_GPORT_MODPORT_SET(mirror_dest->gport, my_modid, mod_dest.port);
    mirror_dest->cosq = mod_dest.cosq;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_get(unit, mod_dest.encap_id, mirror_dest));
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_add(unit, mirror_dest));
    /* Remove with_id flag to keep the same as before warmboot. */
    MIRROR_DEST_DATA(unit, dest_id)->flags &= ~BCM_MIRROR_DEST_WITH_ID;
    mirror_dest_refer(unit, dest_id, 1);

exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover Mirror-on-drop destinations which share the same encap.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_shr_encap_mod_recover(int unit)
{
    int i, dest_id = -1, mod_instance_id = -1, encap_index = -1, count = 0;
    bcm_mirror_destination_t *mirror_dest = NULL;
    bcmi_ltsw_cosq_mod_dest_t mod_dest;
    int my_modid;

    SHR_FUNC_ENTER(unit);

    /* No MOD destination was created, return success. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mod_mirror_dest_id_bk_cnt(unit, &count));
    if (count == 0) {
        SHR_EXIT();
    }

    /* Get Mirror instance id for MOD */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_instance_id_get(unit, &mod_instance_id));

    /* Search encap_index with mod_instance_id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_search(unit, mod_instance_id, &encap_index));

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
        dest_id = MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i];
        if (dest_id == -1) {
            continue;
        }
        /* Get MOD destination id. */
        mod_dest.mod_dest_id = MIRROR_MOD_CTRL(unit)->mod_dest_id_map[dest_id];

        /* Get my modid. */
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_modid));

        /* Get MOD destination. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_mod_dest_get(unit, &mod_dest));

        sal_memset(mirror_dest, 0, sizeof(bcm_mirror_destination_t));

        mirror_dest->flags = BCM_MIRROR_DEST_WITH_ID;
        BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, dest_id);
        BCM_GPORT_MODPORT_SET(mirror_dest->gport, my_modid, mod_dest.port);
        mirror_dest->cosq = mod_dest.cosq;

        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_encap_get(unit, encap_index, mirror_dest));
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_add(unit, mirror_dest));
        mirror_dest_refer(unit, dest_id, 1);
    }

exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover Mirror destination pool.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_recover(int unit)
{
    int i, dest_id = -1, encap_id = -1, my_modid = 0, use_gport = 0;
    bcm_mirror_destination_t *mirror_dest = NULL;
    bcmint_mirror_session_entry_t session;
    bcm_gport_t gport;
    int ref_cnt = 0;

    SHR_FUNC_ENTER(unit);

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    SHR_IF_ERR_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));

    (void)bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &use_gport);

    for (i = 0; i < MIRROR_SESSION_NUM(unit); i++) {
        if (MIRROR_SESSION_REF_COUNT(unit, i) == 0) {
            continue;
        }
        sal_memset(mirror_dest, 0, sizeof(bcm_mirror_destination_t));
        dest_id = MIRROR_SESSION(unit, i)->mirror_dest_id;
        encap_id = MIRROR_SESSION(unit, i)->encap_id;
        ref_cnt = MIRROR_SESSION_REF_COUNT(unit, i);

        /* Mirror destination has been created before. */
        if (MIRROR_DEST_REF_COUNT(unit, dest_id) != 0) {
            mirror_dest_refer(unit, dest_id, ref_cnt);
            continue;
        }

        mirror_dest->flags = BCM_MIRROR_DEST_WITH_ID;
        BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, dest_id);

        sal_memset(&session, 0, sizeof(bcmint_mirror_session_entry_t));
        session.egr = MIRROR_SESSION(unit, i)->egr;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_mirror_session_get(unit, i, &session));

        if (session.sys_port == 0) {
            if (use_gport) {
                BCM_GPORT_MODPORT_SET(gport, my_modid, session.local_port);
            } else {
                gport = session.local_port;
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_sys_port_to_port(unit,
                                            session.sys_port,
                                            &gport));
        }
        mirror_dest->gport = gport;
        mirror_dest->duplicate_pri = session.entry_pri;

        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_encap_get(unit, encap_id, mirror_dest));
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_add(unit, mirror_dest));
        /* Set ref_cnt for destination. */
        mirror_dest_refer(unit, dest_id, ref_cnt);
        /* Remove with_id flag to keep the same as before warmboot. */
        MIRROR_DEST_DATA(unit, dest_id)->flags &= ~BCM_MIRROR_DEST_WITH_ID;
    }

    /* Recover Mirror-on-drop destination. */
    if (MIRROR_MOD_DEST_SHR_ENCAP(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_shr_encap_mod_recover(unit));

    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_mod_recover(unit));
    }

exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach a Mirror destination to a source.
 *
 * This function is to attach a Mirror destination to a source, including
 * allocate Mirror session id and add session table, allocate Mirror container
 * and enable Mirror container, etc.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest_id Destination id.
 * \param [out] container_id Allocated Mirror container id. Null means not care.
 * \param [out] session_id Allocated Mirror session id. Null means not care.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_attach(int unit, bcmint_mirror_source_t *source, int dest_id,
                   int *container_id, int *session_id)
{
    int session_id_tmp = -1, container_id_tmp = -1;
    bcm_mirror_destination_t *mirror_dest = NULL;
    int encap_index = -1;
    bool container_allocated = false, ref_cnt_increased = false, egr = false;
    bool session_allocated = false;

    SHR_FUNC_ENTER(unit);

    egr = MIRROR_SOURCE_IS_EGR_TYPE(source->type);

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get Mirror destination with id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         dest_id,
                         mirror_dest));

    /* Add Mirror encap entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_add(unit, egr, mirror_dest, &encap_index));

    /* Create Mirror session table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_session_create(unit,
                               source,
                               mirror_dest,
                               encap_index,
                               &session_id_tmp,
                               &session_allocated));

    if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
        container_id_tmp = session_id_tmp / MIRROR_INSTANCE_NUM(unit);
    } else if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelShared) {
        /* Allocate Mirror container id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_id_alloc(unit,
                                       source,
                                       dest_id,
                                       &container_id_tmp));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    mirror_container_set(unit, MIRROR_SESSION_MODEL(unit),
                         container_id_tmp, dest_id, egr, source->type);
    container_allocated = true;

    /* For field source, field module programs mirror index itself. */
    if (source->type == BCMINT_MIRROR_ST_F_FP_ING ||
        source->type == BCMINT_MIRROR_ST_F_FP_EGR ||
        source->type == BCMINT_MIRROR_ST_F_FIELD_DEST) {
        if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelShared) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_container_enable_set(unit,
                                             source,
                                             container_id_tmp,
                                             session_id_tmp,
                                             true));
        }
    } else if (source->type == BCMINT_MIRROR_ST_F_EP_RECIRC) {
        /*
         * For EP Recirculate, mirror session is fixed to index 0,
         * so instance id == session_id_tmp
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_set
                (unit, source->ep_recirc_profile_id, session_id_tmp));
    } else {
        /* Configure Mirror container. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_enable_set(unit,
                                         source,
                                         container_id_tmp,
                                         session_id_tmp,
                                         true));
    }

    /* Increase ref_cnt only when new attaching. */
    if (session_allocated) {
        mirror_dest_refer(unit, dest_id, 1);
        ref_cnt_increased = true;
    }

    if (session_id != NULL) {
        *session_id = session_id_tmp;
    }

    if (container_id != NULL) {
        *container_id = container_id_tmp;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (ref_cnt_increased) {
            /* coverity[dead_error_line : FALSE] */
            mirror_dest_derefer(unit, dest_id, 1);
        }
        if (container_allocated) {
            (void)mirror_container_id_free(unit, container_id_tmp);
        }
    }
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach one specific Mirror destination on a source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_detach_by_id(int unit, bcmint_mirror_source_t *source, int dest_id)
{
    int session_id = -1, container_id = -1, instance_id = -1;
    int encap_index = -1;
    bcm_mirror_destination_t *mirror_dest = NULL;
    int size, *dest = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get Mirror destination with id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         dest_id,
                         mirror_dest));

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(size, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, size);

    /* Get destinations applied on the source. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, source, dest));

    /* Search Mirror session id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_session_id_search(unit,
                                  source,
                                  dest,
                                  dest_id,
                                  &session_id));

    /* Search encap index with session id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_search(unit, session_id, &encap_index));

    if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
        container_id = session_id / MIRROR_INSTANCE_NUM(unit);
    } else if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelShared) {
        /* Search Mirror container id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_id_search(unit,
                                        source,
                                        dest_id,
                                        &container_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (source->type == BCMINT_MIRROR_ST_F_EP_RECIRC) {
        /*
         * If any EP Recirculate profile entry is still using the mirror
         * instance, cannot detach it
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_get
                (unit, &instance_id));

        if (instance_id != -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (!(source->type == BCMINT_MIRROR_ST_F_FP_ING ||
                 source->type == BCMINT_MIRROR_ST_F_FP_EGR ||
                 source->type == BCMINT_MIRROR_ST_F_FIELD_DEST)) {
        /*
         * For field source, field module programs mirror index itself.
         * Configure Mirror container for other sources.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_enable_set(unit,
                                         source,
                                         container_id,
                                         session_id,
                                         false));
    }

    /* Free Mirror container. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_container_id_free(unit, container_id));

    /* Delete Mirror encap entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_delete(unit, mirror_dest, encap_index));

    /* Destroy Mirror session table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_session_destroy(unit, session_id));
    mirror_dest_derefer(unit, dest_id, 1);
exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach a/all Mirror destination(s) on a source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror container source.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_detach(int unit, bcmint_mirror_source_t *source, int dest_id)
{
    int i, *dest = NULL, alloc_sz = 0;

    SHR_FUNC_ENTER(unit);

    if (dest_id == -1) {
        alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
        dest = sal_alloc(alloc_sz, "ltswMirrorDest");
        SHR_NULL_CHECK(dest, SHR_E_MEMORY);
        sal_memset(dest, 0xff, alloc_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_id_get_by_source(unit, source, dest));

        for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
            if (dest[i] == -1) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_detach_by_id(unit, source, dest[i]));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_detach_by_id(unit, source, dest_id));
    }

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a sFlow source.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_sflow_attach(int unit, bcm_port_t port, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_SAMPLER;
    source.port = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a sFlow source.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_sflow_detach(int unit, bcm_port_t port, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_SAMPLER;
    source.port = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a sflow source.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_sflow_get(int unit, bcm_port_t port, int mirror_dest_size,
                      bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_SAMPLER;
    source.port = port;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to an INT source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_int_attach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_INT;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from an INT source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_int_detach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_INT;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on an INT source.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_int_get(int unit, int mirror_dest_size,
                    bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_INT;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to an ELEPHANT source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_elephant_attach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_ELEPHANT;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from an ELEPHANT source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_elephant_detach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_ELEPHANT;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on an ELEPHANT source.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_elephant_get(int unit, int mirror_dest_size,
                         bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_ELEPHANT;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a DLB_MONITOR source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_dlb_monitor_attach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_DLB_MONITOR;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a DLB_MONITOR source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_dlb_monitor_detach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_DLB_MONITOR;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a DLB_MONITOR source.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_dlb_monitor_get(int unit, int mirror_dest_size,
                            bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_DLB_MONITOR;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in] id EP Recirculate profile id
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_ep_recirc_profile_attach(int unit, int id, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_EP_RECIRC;
    source.ep_recirc_profile_id = id;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in] id EP Recirculate profile id
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_ep_recirc_profile_detach(int unit, int id, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_EP_RECIRC;
    source.ep_recirc_profile_id = id;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to an ingress mirror-on-drop source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_ingress_mod_attach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_ING_MOD;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from an ingress mirror-on-drop source.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_ingress_mod_detach(int unit, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_ING_MOD;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on an
 *        ingress mirror-on-drop source.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_ingress_mod_get(int unit, int mirror_dest_size,
                            bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_ING_MOD;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a trace event source.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_trace_event_attach(int unit, bcm_pkt_trace_event_t trace_event,
                               int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_TRACE_EVENT;
    source.trace_event = trace_event;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a trace event source.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_trace_event_detach(int unit, bcm_pkt_trace_event_t trace_event,
                               int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_TRACE_EVENT;
    source.trace_event = trace_event;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a trace event source.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_trace_event_get(int unit, bcm_pkt_trace_event_t trace_event,
                            int mirror_dest_size,
                            bcm_gport_t *mirror_dest,
                            int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_TRACE_EVENT;
    source.trace_event = trace_event;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a drop event source.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_drop_event_attach(int unit, bcm_pkt_drop_event_t drop_event,
                              int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_DROP_EVENT;
    source.drop_event = drop_event;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a drop event source.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_drop_event_detach(int unit, bcm_pkt_drop_event_t drop_event,
                              int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_DROP_EVENT;
    source.drop_event = drop_event;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a drop event source.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_drop_event_get(int unit, bcm_pkt_drop_event_t drop_event,
                           int mirror_dest_size,
                           bcm_gport_t *mirror_dest,
                           int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_DROP_EVENT;
    source.drop_event = drop_event;

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_port_attach(int unit, bcm_port_t port, uint32 flags, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));

    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            source.type = BCMINT_MIRROR_ST_F_VP_ING;
            source.port = port;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
            source.type = BCMINT_MIRROR_ST_F_PORT_ING;
            source.port = port;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
    }

    if (flags & BCM_MIRROR_PORT_EGRESS) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
            source.type = BCMINT_MIRROR_ST_F_PORT_EGR;
            source.port = port;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_port_detach(int unit, bcm_port_t port, uint32 flags, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));

    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            source.type = BCMINT_MIRROR_ST_F_VP_ING;
            source.port = port;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
            source.type = BCMINT_MIRROR_ST_F_PORT_ING;
            source.port = port;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_detach(unit, &source, dest_id));
    }

    if (flags & BCM_MIRROR_PORT_EGRESS) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
            source.type = BCMINT_MIRROR_ST_F_PORT_EGR;
            source.port = port;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_detach(unit, &source, dest_id));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a port source.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_port_get(int unit, bcm_port_t port, uint32_t flags,
                     int mirror_dest_size, bcm_gport_t *mirror_dest,
                     int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            source.type = BCMINT_MIRROR_ST_F_VP_ING;
            source.port = port;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
            source.type = BCMINT_MIRROR_ST_F_PORT_ING;
            source.port = port;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_id_get_by_source(unit, &source, dest));

        for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
            if (dest[i] == -1) {
                continue;
            }
            if (count < mirror_dest_size) {
                if (mirror_dest != NULL) {
                    BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
                }
                count++;
            }
        }
    }

    if ((flags & BCM_MIRROR_PORT_EGRESS) &&
        (count < mirror_dest_size)) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_FLOW_PORT(port)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &port));
            source.type = BCMINT_MIRROR_ST_F_PORT_EGR;
            source.port = port;
        }
        sal_memset(dest, 0xff, alloc_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_id_get_by_source(unit, &source, dest));
        for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
            if (dest[i] == -1) {
                continue;
            }
            if (count < mirror_dest_size) {
                if (mirror_dest != NULL) {
                    BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
                }
                count++;
            }
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach a Mirror-on-drop destination.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_mod_attach(int unit, int dest_id)
{
    bcm_mirror_destination_t *mirror_dest = NULL;
    int encap_index = -1;
    bool ref_cnt_increased = false, egr = false;
    bcmi_ltsw_cosq_mod_dest_t mod_dest;
    bcm_port_t local_port;

    SHR_FUNC_ENTER(unit);

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get Mirror destination with id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         dest_id,
                         mirror_dest));

    /* Add Mirror encap entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_add(unit, egr, mirror_dest, &encap_index));

    /* Get local port from Mirror gport. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_local_port_get(unit,
                                         mirror_dest->gport,
                                         NULL,
                                         &local_port));

    sal_memset(&mod_dest, 0, sizeof(bcmi_ltsw_cosq_mod_dest_t));
    mod_dest.port = local_port;
    mod_dest.cosq = mirror_dest->cosq;
    mod_dest.encap_id = encap_index;

    /* Add mirror-on-drop destination. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_dest_add(unit, &mod_dest));

    /* Increase ref_cnt only when new attaching. */
    if (!(mirror_dest->flags & BCM_MIRROR_DEST_REPLACE)) {
        mirror_dest_refer(unit, dest_id, 1);
        ref_cnt_increased = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mod_mirror_dest_id_bk_add(unit, dest_id));

exit:
    if (SHR_FUNC_ERR()) {
        if (ref_cnt_increased) {
            /* coverity[dead_error_line : FALSE] */
            mirror_dest_derefer(unit, dest_id, 1);
        }
    }
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach a Mirror-on-drop destination.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_mod_detach(int unit, int dest_id)
{
    int encap_index = -1, dest_id_tmp;
    bcm_mirror_destination_t *mirror_dest = NULL;
    bcmi_ltsw_cosq_mod_dest_t mod_dest;

    SHR_FUNC_ENTER(unit);

    dest_id_tmp = MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[0];
    if (dest_id != -1 && dest_id != dest_id_tmp) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get Mirror destination with id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         dest_id_tmp,
                         mirror_dest));

    /* Get mirror-on-drop destination. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_dest_get(unit, &mod_dest));
    encap_index = mod_dest.encap_id;

    /* Delete mirror-on-drop destination. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_dest_delete(unit, &mod_dest));

    /* Delete Mirror encap entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_encap_delete(unit, mirror_dest, encap_index));

    mirror_dest_derefer(unit, dest_id_tmp, 1);
    SHR_IF_ERR_VERBOSE_EXIT
        (mod_mirror_dest_id_bk_delete(unit, dest_id_tmp));

exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve applied CoSQ Mirror-on-drop destinations.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_cosq_mod_get(int unit, int mirror_dest_size,
                         bcm_gport_t *mirror_dest,
                         int *mirror_dest_count)
{
    int i, dest_id_tmp = -1, count = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
        if (MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            dest_id_tmp = MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i];
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest_id_tmp);
            count++;
        }
    }
    *mirror_dest_count = count;

    if (dest_id_tmp == -1) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add mirror destination on a field destination entry.
 *
 * \param [in] unit Unit number.
 * \param [in] match Field destination match.
 * \param [in] dest_id Destination id.
 * \param [out] info Returned Mirror info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_field_destination_attach(int unit,
                                     bcm_field_destination_match_t *match,
                                     int dest_id,
                                     bcmi_ltsw_mirror_info_t *info)
{
    bcmint_mirror_source_t source;
    int session_id;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_FIELD_DEST;
    sal_memcpy(&source.field_dest_match, match,
               sizeof(bcm_field_destination_match_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, &session_id));
    info->mirror_cont = session_id / MIRROR_INSTANCE_NUM(unit);
    info->mirror_index = session_id % MIRROR_INSTANCE_NUM(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the mirror destination on a field destination entry.
 *
 * \param [in] unit Unit number.
 * \param [in] match Field destination match.
 * \param [out] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_field_destination_get(int unit,
                                  bcm_field_destination_match_t *match,
                                  int *dest_id)
{
    bcmint_mirror_source_t source;
    int *dest = NULL, size, i;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(dest_id, SHR_E_PARAM);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_FIELD_DEST;
    sal_memcpy(&source.field_dest_match, match,
               sizeof(bcm_field_destination_match_t));

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(size, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, size);

    /* Get destinations applied on the source. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] != -1) {
            break;
        }
    }
    *dest_id = dest[i];

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete mirror destination applied on a field destination entry.
 *
 * \param [in] unit Unit number.
 * \param [in] match Field destination match.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_field_destination_detach(int unit,
                                     bcm_field_destination_match_t *match)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_FIELD_DEST;
    sal_memcpy(&source.field_dest_match, match,
               sizeof(bcm_field_destination_match_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, -1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Bookkeeping a Mirror destination id for CoSQ MOD.
 *
 * \param [in] unit Unit number.
 * \param [in] id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mod_mirror_dest_id_bk_add(int unit, int dest_id)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
        if (MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] != -1) {
            continue;
        }
        MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] = dest_id;
        SHR_EXIT();
    }
    SHR_ERR_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a bookkeeping Mirror destination id for CoSQ MOD.
 *
 * \param [in] unit Unit number.
 * \param [in] id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mod_mirror_dest_id_bk_delete(int unit, int dest_id)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
        if (MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] == dest_id) {
            MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] = -1;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get count of bookkeeping Mirror destination ids for CoSQ MOD.
 *
 * \param [in] unit Unit number.
 * \param [out] count Count for MOD destination ids.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mod_mirror_dest_id_bk_cnt(int unit, int* count)
{
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    *count = 0;
    for (i = 0; i < MIRROR_MOD_DEST_NUM(unit); i++) {
        if (MIRROR_MOD_CTRL(unit)->mod_mirror_dest_id[i] != -1) {
            *count = *count + 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach a Mirror-on-drop destination with a shared encap.
 *
 * \param [in] unit Unit number.
 * \param [in] mod_profile_id MOD profile id.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_shr_encap_mod_attach(int unit, int mod_profile_id, int dest_id)
{
    bcm_mirror_destination_t *mirror_dest = NULL;
    int session_id_tmp = -1, encap_index = -1, mod_instance_id, rv;
    bool ref_cnt_increased = false, egr = false;
    bool session_allocated = false;
    bcmi_ltsw_cosq_mod_dest_t mod_dest;
    bcm_port_t local_port;
    bcmint_mirror_source_t source;
    bcmi_ltsw_cosq_port_map_info_t info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_COSQ_MOD;

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get Mirror destination with id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         dest_id,
                         mirror_dest));

    /* Get local port from Mirror gport. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_local_port_get(unit,
                                         mirror_dest->gport,
                                         NULL,
                                         &local_port));

    if (ltsw_feature(unit, LTSW_FT_MMU_MOD_MC_COS_ONLY)) {
        /* Get UC/MC queue number. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &info));

        /*
         * Only mc queue is used for TM Mirror-on-drop.
         * Check if the cosq exceeds the maximum queue number.
         */
        if ((mirror_dest->cosq < info.num_uc_q) ||
            (mirror_dest->cosq >= (info.num_uc_q + info.num_mc_q))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    sal_memset(&mod_dest, 0, sizeof(bcmi_ltsw_cosq_mod_dest_t));
    mod_dest.port = local_port;
    mod_dest.cosq = mirror_dest->cosq;
    mod_dest.mod_profile_id = mod_profile_id;

    /*
     * Create the shared mirror instance for MOD dests,
     * which can be used as index to get shared encap.
     */

    /* Get Mirror instance id for MOD */
    rv = bcmi_ltsw_cosq_mod_instance_id_get(unit, &mod_instance_id);
    if (rv == SHR_E_NOT_FOUND) {
        /* Add Mirror encap entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_encap_add(unit, egr, mirror_dest, &encap_index));

        /* Create Mirror session table for MOD. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_session_create(unit,
                                   &source,
                                   mirror_dest,
                                   encap_index,
                                   &session_id_tmp,
                                   &session_allocated));

        mod_instance_id = session_id_tmp;
    } else if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    mod_dest.instance_id = mod_instance_id;

    /* Add mirror-on-drop destination. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_dest_add(unit, &mod_dest));

    /*
     * Now mod_dest_id has been allocated in MOD module,
     * bookkeeping for mod_dest delete/get.
     */
    MIRROR_MOD_CTRL(unit)->mod_dest_id_map[dest_id] = mod_dest.mod_dest_id;

    /* Increase ref_cnt only when new attaching. */
    if (!(mirror_dest->flags & BCM_MIRROR_DEST_REPLACE)) {
        mirror_dest_refer(unit, dest_id, 1);
        ref_cnt_increased = true;
    }

    /* Bookkeeping dest_id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mod_mirror_dest_id_bk_add(unit, dest_id));

exit:
    if (SHR_FUNC_ERR()) {
        if (ref_cnt_increased) {
            /* coverity[dead_error_line : FALSE] */
            mirror_dest_derefer(unit, dest_id, 1);
        }
    }
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach a Mirror-on-drop destination with a shared encap.
 *
 * \param [in] unit Unit number.
 * \param [in] mod_profile_id MOD profile id.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_shr_encap_mod_detach(int unit, int mod_profile_id, int dest_id)
{
    int encap_index = -1, mod_dest_id = -1, mod_instance_id = -1;
    int mod_dest_count = 0;
    bcm_mirror_destination_t *mirror_dest = NULL;
    bcmi_ltsw_cosq_mod_dest_t mod_dest;

    SHR_FUNC_ENTER(unit);

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    /* Get Mirror destination with id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         dest_id,
                         mirror_dest));

    /* Get mirror-on-drop destination. */
    mod_dest_id = MIRROR_MOD_CTRL(unit)->mod_dest_id_map[dest_id];
    if (mod_dest_id == -1) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    mod_dest.mod_dest_id = mod_dest_id;

    /* Get Mirror instance id for MOD */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_instance_id_get(unit, &mod_instance_id));
    mod_dest.instance_id = mod_instance_id;

    mod_dest.mod_profile_id = mod_profile_id;

    /* Delete mirror-on-drop destination. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_dest_delete(unit, &mod_dest));

    mirror_dest_derefer(unit, dest_id, 1);
    SHR_IF_ERR_VERBOSE_EXIT
        (mod_mirror_dest_id_bk_delete(unit, dest_id));

    /* If no mod_dest remains, delete Mirror instance and encap entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mod_mirror_dest_id_bk_cnt(unit, &mod_dest_count));
    if (mod_dest_count == 0) {
        encap_index = MIRROR_SESSION(unit, mod_instance_id)->encap_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_encap_delete(unit, mirror_dest, encap_index));
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_session_destroy(unit, mod_instance_id));
    }

exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a sample profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type Source type.
 * \param [in] id Sample profile id.
 * \param [in] dest_id Destination id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_sample_profile_attach(int unit, bcm_mirror_source_type_t type,
                                  uint32_t id, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.sample_profile_id = id;

    if (type == bcmMirrorSourceTypeSampleIngress) {
        source.type = BCMINT_MIRROR_ST_F_SAMPLE_INGRESS;
    } else if (type == bcmMirrorSourceTypeSampleFlex) {
        source.type = BCMINT_MIRROR_ST_F_SAMPLE_FLEX;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a sample profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type Source type.
 * \param [in] id Sample profile id.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_sample_profile_detach(int unit, bcm_mirror_source_type_t type,
                                  uint32_t id, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.sample_profile_id = id;

    if (type == bcmMirrorSourceTypeSampleIngress) {
        source.type = BCMINT_MIRROR_ST_F_SAMPLE_INGRESS;
    } else if (type == bcmMirrorSourceTypeSampleFlex) {
        source.type = BCMINT_MIRROR_ST_F_SAMPLE_FLEX;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a sample profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type Source type.
 * \param [in] id Sample profile id.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_sample_profile_get(int unit, bcm_mirror_source_type_t type,
                               uint32_t id, int mirror_dest_size,
                               bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int i, *dest = NULL, alloc_sz = 0, count = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.sample_profile_id = id;

    if (type == bcmMirrorSourceTypeSampleIngress) {
        source.type = BCMINT_MIRROR_ST_F_SAMPLE_INGRESS;
    } else if (type == bcmMirrorSourceTypeSampleFlex) {
        source.type = BCMINT_MIRROR_ST_F_SAMPLE_FLEX;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    alloc_sz = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(alloc_sz, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (dest[i] == -1) {
            continue;
        }
        if (count < mirror_dest_size) {
            BCM_GPORT_MIRROR_SET(mirror_dest[count], dest[i]);
            count++;
        }
    }
    *mirror_dest_count = count;

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a Mirror destination to a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] mirror_dest_id Mirror destination gport id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_port_dest_add(int unit, bcm_port_t port, uint32 flags,
                     bcm_gport_t mirror_dest_id)
{
    int dest_id = -1;
    SHR_FUNC_ENTER(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    /* Attach a mirror destination to a given source. */
    if (flags & BCM_MIRROR_PORT_SFLOW) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_sflow_attach(unit, port, dest_id));
    } else if (flags & BCM_MIRROR_PORT_INT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_int_attach(unit, dest_id));
    } else if (flags & BCM_MIRROR_PORT_ELEPHANT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_elephant_attach(unit, dest_id));
    } else if (flags & BCM_MIRROR_PORT_DLB_MONITOR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_dlb_monitor_attach(unit, dest_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_port_attach(unit, port, flags, dest_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Mirror destination from a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] mirror_dest_id Mirror destination gport id,
 *                            -1 indicates delete all.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_port_dest_delete(int unit, bcm_port_t port, uint32 flags,
                        bcm_gport_t mirror_dest_id)
{
    int dest_id = -1;
    SHR_FUNC_ENTER(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    /* Detach a mirror destination from a given source. */
    if (flags & BCM_MIRROR_PORT_SFLOW) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_sflow_detach(unit, port, dest_id));
    } else if (flags & BCM_MIRROR_PORT_INT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_int_detach(unit, dest_id));
    } else if (flags & BCM_MIRROR_PORT_ELEPHANT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_elephant_detach(unit, dest_id));
    } else if (flags & BCM_MIRROR_PORT_DLB_MONITOR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_dlb_monitor_detach(unit, dest_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_port_detach(unit, port, flags, dest_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a source.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_port_dest_get(int unit, bcm_port_t port, uint32 flags,
                     int mirror_dest_size,
                     bcm_gport_t *mirror_dest,
                     int *mirror_dest_count)
{
    SHR_FUNC_ENTER(unit);

    /* Get a mirror destination from a given source. */
    if (flags & BCM_MIRROR_PORT_SFLOW) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_sflow_get(unit,
                                   port,
                                   mirror_dest_size,
                                   mirror_dest,
                                   mirror_dest_count));
    } else if (flags & BCM_MIRROR_PORT_INT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_int_get(unit,
                                 mirror_dest_size,
                                 mirror_dest,
                                 mirror_dest_count));
    } else if (flags & BCM_MIRROR_PORT_ELEPHANT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_elephant_get(unit,
                                      mirror_dest_size,
                                      mirror_dest,
                                      mirror_dest_count));
    } else if (flags & BCM_MIRROR_PORT_DLB_MONITOR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_dlb_monitor_get(unit,
                                         mirror_dest_size,
                                         mirror_dest,
                                         mirror_dest_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_port_get(unit,
                                  port,
                                  flags,
                                  mirror_dest_size,
                                  mirror_dest,
                                  mirror_dest_count));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a Mirror destination to a Mirror source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror source.
 * \param [in] mirror_dest_id Mirror destination gport id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_source_dest_add(int unit, bcm_mirror_source_t *source,
                       bcm_gport_t mirror_dest_id)
{
    int dest_id = -1;
    uint32_t flags_tmp = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(source, SHR_E_PARAM);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    if (source->type == bcmMirrorSourceTypePktTraceEvent) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_trace_event_attach(unit,
                                            source->trace_event,
                                            dest_id));
    } else if (source->type == bcmMirrorSourceTypePktDropEvent) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_drop_event_attach(unit,
                                           source->drop_event,
                                           dest_id));
    } else if (source->type == bcmMirrorSourceTypePort) {
        if (source->flags & BCM_MIRROR_SOURCE_INGRESS) {
            flags_tmp |= BCM_MIRROR_PORT_INGRESS;
        }
        if (source->flags & BCM_MIRROR_SOURCE_EGRESS) {
            flags_tmp |= BCM_MIRROR_PORT_EGRESS;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_port_attach(unit, source->port, flags_tmp, dest_id));
    } else if (source->type == bcmMirrorSourceTypeCosqMod) {
        if (MIRROR_MOD_DEST_SHR_ENCAP(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_shr_encap_mod_attach(unit, source->mod_profile_id, dest_id));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_mod_attach(unit, dest_id));
        }
    } else if (source->type == bcmMirrorSourceTypeSampleIngress ||
               source->type == bcmMirrorSourceTypeSampleFlex) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_sample_profile_attach(unit, source->type,
                                               source->sample_profile_id,
                                               dest_id));
    } else if (source->type == bcmMirrorSourceTypeIngressMod) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_ingress_mod_attach(unit, dest_id));
    } else if (source->type == bcmMirrorSourceTypeEpRecirc) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_ep_recirc_profile_attach(unit,
                                                  source->ep_recirc_profile_id,
                                                  dest_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Mirror destination from a Mirror source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror source.
 * \param [in] mirror_dest_id Mirror destination gport id,
 *                            -1 indicates delete all.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_source_dest_delete(int unit, bcm_mirror_source_t *source,
                          bcm_gport_t mirror_dest_id)
{
    int dest_id = -1;
    uint32_t flags_tmp = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(source, SHR_E_PARAM);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    if (source->type == bcmMirrorSourceTypePktTraceEvent) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_trace_event_detach(unit,
                                            source->trace_event,
                                            dest_id));
    } else if (source->type == bcmMirrorSourceTypePktDropEvent) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_drop_event_detach(unit,
                                           source->drop_event,
                                           dest_id));
    } else if (source->type == bcmMirrorSourceTypePort) {
        if (source->flags & BCM_MIRROR_SOURCE_INGRESS) {
            flags_tmp |= BCM_MIRROR_PORT_INGRESS;
        }
        if (source->flags & BCM_MIRROR_SOURCE_EGRESS) {
            flags_tmp |= BCM_MIRROR_PORT_EGRESS;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_port_detach(unit, source->port, flags_tmp, dest_id));
    } else if (source->type == bcmMirrorSourceTypeCosqMod) {
        if (MIRROR_MOD_DEST_SHR_ENCAP(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_shr_encap_mod_detach(unit, source->mod_profile_id, dest_id));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_mod_detach(unit, dest_id));
        }
    } else if (source->type == bcmMirrorSourceTypeSampleIngress ||
               source->type == bcmMirrorSourceTypeSampleFlex) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_sample_profile_detach(unit, source->type,
                                               source->sample_profile_id,
                                               dest_id));
    } else if (source->type == bcmMirrorSourceTypeIngressMod) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_ingress_mod_detach(unit, dest_id));
    } else if (source->type == bcmMirrorSourceTypeEpRecirc) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_ep_recirc_profile_detach(unit,
                                                  source->ep_recirc_profile_id,
                                                  dest_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Mirror destination gport ids applied on a Mirror source.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror source.
 * \param [in] mirror_dest_size Preallocated mirror_dest array size.
 * \param [out] mirror_dest Applied Mirror destination gport ids.
 * \param [out] mirror_dest_count Actual number of Mirror destinations applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_source_dest_get(int unit, bcm_mirror_source_t *source,
                       int mirror_dest_size,
                       bcm_gport_t *mirror_dest,
                       int *mirror_dest_count)
{
    uint32_t flags_tmp = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(source, SHR_E_PARAM);

    if (source->type == bcmMirrorSourceTypePktTraceEvent) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_trace_event_get(unit,
                                         source->trace_event,
                                         mirror_dest_size,
                                         mirror_dest,
                                         mirror_dest_count));
    } else if (source->type == bcmMirrorSourceTypePktDropEvent) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_drop_event_get(unit,
                                        source->drop_event,
                                        mirror_dest_size,
                                        mirror_dest,
                                        mirror_dest_count));
    } else if (source->type == bcmMirrorSourceTypePort) {
        if (source->flags & BCM_MIRROR_SOURCE_INGRESS) {
            flags_tmp |= BCM_MIRROR_PORT_INGRESS;
        }
        if (source->flags & BCM_MIRROR_SOURCE_EGRESS) {
            flags_tmp |= BCM_MIRROR_PORT_EGRESS;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_port_get(unit,
                                  source->port,
                                  flags_tmp,
                                  mirror_dest_size,
                                  mirror_dest,
                                  mirror_dest_count));
    } else if (source->type == bcmMirrorSourceTypeCosqMod) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_cosq_mod_get(unit,
                                      mirror_dest_size,
                                      mirror_dest,
                                      mirror_dest_count));
    } else if (source->type == bcmMirrorSourceTypeSampleIngress ||
               source->type == bcmMirrorSourceTypeSampleFlex) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_sample_profile_get(unit, source->type,
                                            source->sample_profile_id,
                                            mirror_dest_size,
                                            mirror_dest,
                                            mirror_dest_count));
    } else if (source->type == bcmMirrorSourceTypeIngressMod) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_ingress_mod_get(unit,
                                         mirror_dest_size,
                                         mirror_dest,
                                         mirror_dest_count));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate input parameters for mirror source add/delete.
 *
 * \param [in] unit Unit number.
 * \param [in] source Mirror source.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_source_params_validate(int unit, bcm_mirror_source_t *source)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(source, SHR_E_PARAM);

    if (source->type >= bcmMirrorSourceTypeCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        if (source->type == bcmMirrorSourceTypePktTraceEvent &&
            source->trace_event >= bcmPktTraceEventCount) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else if (source->type == bcmMirrorSourceTypePktDropEvent &&
                   source->drop_event >= bcmPktDropEventCount) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (source->type == bcmMirrorSourceTypeSampleIngress ||
            source->type == bcmMirrorSourceTypeSampleFlex) {
            if (source->sample_profile_id >= MIRROR_SAMPLE_PRF_NUM(unit) ||
                source->sample_profile_id == 0) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct Mirror destination gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] modid Module id.
 * \param [in] flags BCM_MIRROR_PORT_* flags.
 * \param [out] gport Destination gport.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_gport_construct(int unit, bcm_port_t port, bcm_module_t modid,
                            uint32_t flags, bcm_gport_t *gport)
{
    int rv, modid_tmp, max_modid;
    bcmi_ltsw_gport_info_t gport_info;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(port)) {
        *gport = port;
    } else {
        if (flags & BCM_MIRROR_PORT_DEST_TRUNK) {
            gport_info.gport_type = _SHR_GPORT_TYPE_TRUNK;
            gport_info.tgid = port;
        } else {
            if (modid == -1) {
                rv = bcm_ltsw_stk_my_modid_get(unit, &modid_tmp);
                if (SHR_E_UNAVAIL == rv || modid_tmp < 0) {
                    modid_tmp = 0;
                    rv = SHR_E_NONE;
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } else {
                max_modid = bcmi_ltsw_dev_max_modid(unit);
                if (!(modid >= 0 && modid <= max_modid)) {
                     SHR_ERR_EXIT(SHR_E_PARAM);
                }
                modid_tmp = modid;
            }
            gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            gport_info.modid = modid_tmp;
            gport_info.port = port;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_info, gport));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach Mirror destination to a FP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_id FP entry id.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] dest_id Destination id.
 * \param [out] info Returned Mirror info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_fp_attach(int unit, int entry_id, uint32 flags, int dest_id,
                      bcmi_ltsw_mirror_info_t *info)
{
    bcmint_mirror_source_t source;
    int session_id, container_id = -1;

    SHR_FUNC_ENTER(unit);

    if (info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    if (flags == BCMI_LTSW_MIRROR_INGRESS) {
        source.type = BCMINT_MIRROR_ST_F_FP_ING;
    } else if (flags == BCMI_LTSW_MIRROR_EGRESS) {
        source.type = BCMINT_MIRROR_ST_F_FP_EGR;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    source.entry_id = entry_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_attach(unit, &source, dest_id, &container_id, &session_id));

    if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
        info->mirror_cont = session_id / MIRROR_INSTANCE_NUM(unit);
        info->mirror_index = session_id % MIRROR_INSTANCE_NUM(unit);
    } else if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelShared) {
        info->mirror_cont = container_id;
        info->mirror_index = session_id;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Mirror destination info for given dest id on a FP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_id FP entry id.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] dest_id Destination id.
 * \param [out] info Returned Mirror info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_fp_get(int unit, int entry_id, uint32 flags, int dest_id,
                   bcmi_ltsw_mirror_info_t *info)
{
    bcmint_mirror_source_t source;
    int session_id, container_id, *dest = NULL, size;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    if (flags == BCMI_LTSW_MIRROR_INGRESS) {
        source.type = BCMINT_MIRROR_ST_F_FP_ING;
    } else if (flags == BCMI_LTSW_MIRROR_EGRESS) {
        source.type = BCMINT_MIRROR_ST_F_FP_EGR;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    source.entry_id = entry_id;

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    dest = sal_alloc(size, "ltswMirrorDest");
    SHR_NULL_CHECK(dest, SHR_E_MEMORY);
    sal_memset(dest, 0xff, size);

    /* Get destinations applied on the source. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_id_get_by_source(unit, &source, dest));

    /* Search Mirror session id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_session_id_search(unit,
                                  &source,
                                  dest,
                                  dest_id,
                                  &session_id));
    if (info != NULL) {
        if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
            info->mirror_cont = session_id / MIRROR_INSTANCE_NUM(unit);
            info->mirror_index = session_id % MIRROR_INSTANCE_NUM(unit);
        } else if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelShared) {
            /* Search Mirror container id. */
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_container_id_search(unit,
                                            &source,
                                            dest_id,
                                            &container_id));

            info->mirror_cont = container_id;
            info->mirror_index = session_id;
        }
    }

exit:
    SHR_FREE(dest);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Mirror destination from a FP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_id FP entry id.
 * \param [in] flags BCM_MIRROR_PORT_*.
 * \param [in] dest_id Destination id, -1 indicates detach all destinations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_dest_fp_detach(int unit, int entry_id, uint32 flags, int dest_id)
{
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    if (flags == BCMI_LTSW_MIRROR_INGRESS) {
        source.type = BCMINT_MIRROR_ST_F_FP_ING;
    } else if (flags == BCMI_LTSW_MIRROR_EGRESS) {
        source.type = BCMINT_MIRROR_ST_F_FP_EGR;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    source.entry_id = entry_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_detach(unit, &source, dest_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set mirroring configuration for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [in] dest_mod Module of MTP.
 * \param [in] dest_port Mirror-to-port.
 * \param [in] flags BCM_MIRROR_PORT_* flags.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_port_set(int unit, bcm_port_t port, bcm_module_t dest_mod,
                bcm_port_t dest_port, uint32_t flags)
{
    bcm_gport_t gport;
    bool dest_created = false;
    int rv, id = -1;
    bcm_mirror_destination_t *mirror_dest = NULL;
    SHR_FUNC_ENTER(unit);

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    if (BCM_GPORT_IS_MIRROR(dest_port)) {
        /* Get Mirror destination from input gport. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_get(unit,
                             BCM_GPORT_MIRROR_GET(dest_port),
                             mirror_dest));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_gport_construct(unit,
                                         dest_port,
                                         dest_mod,
                                         flags,
                                         &gport));
        rv = mirror_dest_search(unit, gport, NULL, mirror_dest);
        if (rv == SHR_E_NOT_FOUND) {
            if (flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
                /* Create destination with modid and port. */
                mirror_dest->gport = gport;
                SHR_IF_ERR_VERBOSE_EXIT
                    (mirror_dest_create(unit, mirror_dest));
                dest_created = true;
            } else {
                SHR_EXIT();
            }
        }
    }

    /* Enable/Disable ingress mirroring. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        /*
         * Since this function is set, not add, it is sufficient if
         * the destination already exists. Skip exists error code.
         */
        rv = mirror_port_dest_add(unit,
                                  port,
                                  BCM_MIRROR_PORT_INGRESS,
                                  mirror_dest->mirror_dest_id);
        if (rv == SHR_E_EXISTS) {
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /*
         * If already delete, then return success.
         */
        rv = mirror_port_dest_delete(unit,
                                     port,
                                     BCM_MIRROR_PORT_INGRESS,
                                     mirror_dest->mirror_dest_id);
        if (rv == SHR_E_NOT_FOUND) {
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Enable/Disable egress mirroring. */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        /*
         * Since this function is set, not add, it is sufficient if
         * the destination already exists. Skip exists error code.
         */
        rv = mirror_port_dest_add(unit,
                                  port,
                                  BCM_MIRROR_PORT_EGRESS,
                                  mirror_dest->mirror_dest_id);
        if (rv == SHR_E_EXISTS) {
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /*
         * If already delete, then return success.
         */
        rv = mirror_port_dest_delete(unit,
                                     port,
                                     BCM_MIRROR_PORT_EGRESS,
                                     mirror_dest->mirror_dest_id);
        if (rv == SHR_E_NOT_FOUND) {
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* Delete unused mirror destination. */
    if (BCM_GPORT_IS_MIRROR(mirror_dest->mirror_dest_id)) {
        id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
        if (MIRROR_DEST_REF_COUNT(unit, id) <= 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_dest_delete(unit, id));
        }
    }

exit:
    if (SHR_FUNC_ERR() && (dest_created == true)) {
        id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
        (void)mirror_dest_delete(unit, id);
    }
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mirroring configuration for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port value.
 * \param [out] dest_mod Module of MTP.
 * \param [out] dest_port Mirror-to-port.
 * \param [out] flags BCM_MIRROR_PORT_* flags.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_port_get(int unit, bcm_port_t port, bcm_module_t *dest_mod,
                bcm_port_t *dest_port, uint32_t *flags)
{
    bcm_gport_t mirror_dest_id = -1, mirror_dest_id_tmp = -1;
    int use_gport, mirror_dest_count = 0, modid, port_id, trunk_id, vp;
    bcm_mirror_destination_t *mirror_dest = NULL;

    SHR_FUNC_ENTER(unit);

    *flags = 0;

    /* Allocate a Mirror destination buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_destination_alloc(unit, &mirror_dest));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &use_gport));

    /* Read port ingress mirroring destination ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_port_dest_get(unit,
                              port,
                              BCM_MIRROR_PORT_INGRESS,
                              1,
                              &mirror_dest_id,
                              &mirror_dest_count));

    if (mirror_dest_count) {
        *flags |= BCM_MIRROR_PORT_INGRESS;
    }

    mirror_dest_count = 0;
    /* Read port egress mirroring destination ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_port_dest_get(unit,
                              port,
                              BCM_MIRROR_PORT_EGRESS,
                              1,
                              &mirror_dest_id_tmp,
                              &mirror_dest_count));
    if (mirror_dest_count) {
        *flags |= BCM_MIRROR_PORT_EGRESS;
        mirror_dest_id = mirror_dest_id_tmp;
    }

    if (mirror_dest_id != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_get(unit,
                             BCM_GPORT_MIRROR_GET(mirror_dest_id),
                             mirror_dest));

        if (use_gport) {
            *dest_port = mirror_dest->gport;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit,
                                              mirror_dest->gport,
                                              &modid,
                                              &port_id,
                                              &trunk_id,
                                              &vp));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_modport_hw2api_map(unit,
                                              modid,
                                              port_id,
                                              dest_mod,
                                              dest_port));
        }
    }

exit:
    if (mirror_dest != NULL) {
        mirror_destination_free(&mirror_dest);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set mirror configuration on a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Local port number.
 * \param [in] enable True means adding, false means removing port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
mirror_flex_port_set(int unit, bcm_port_t port, bool enable)
{
    int i;
    bcmint_mirror_source_t source;

    SHR_FUNC_ENTER(unit);

    sal_memset(&source, 0, sizeof(bcmint_mirror_source_t));
    source.type = BCMINT_MIRROR_ST_F_PORT_ING;
    source.port = port;
    /* Disable ingress mirroring. */
    for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (MIRROR_CONTAINER_REF_COUNT(unit, i) == 0) {
            continue;
        }
        /* Don't support such source type. */
        if (!(MIRROR_CTRL(unit)->mc_st_flags[i] & source.type)) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_enable_set(unit, &source, i, 0, false));
    }

    /* Disable egress mirroring. */
    source.type = BCMINT_MIRROR_ST_F_PORT_EGR;
    source.port = port;
     for (i = 0; i < MIRROR_CONTAINER_NUM(unit); i++) {
        if (MIRROR_CONTAINER_REF_COUNT(unit, i) == 0) {
            continue;
        }
        /* Don't support such source type. */
        if (!(MIRROR_CTRL(unit)->mc_st_flags[i] & source.type)) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_container_enable_set(unit, &source, i, 0, false));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat for a mirror stat object.
 *
 * \param [in] unit Unit number.
 * \param [in] object Mirror stat object.
 * \param [in] sync Sync mode.
 * \param [out] value Stat value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_stat_get(int unit, bcm_mirror_stat_object_t *object, bool sync,
                uint64_t *value)
{
    int dest_id, *session_id = NULL, rv;
    int size = 0, i = 0, count = 0;
    bcmint_mirror_source_t source;
    uint64_t value_tmp = 0LL, val = 0LL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(object, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    session_id = sal_alloc(size, "ltswMirrorSession");
    SHR_NULL_CHECK(session_id, SHR_E_MEMORY);
    sal_memset(session_id, 0xff, size);

    dest_id = BCM_GPORT_MIRROR_GET(object->mirror_dest_id);
    MIRROR_DEST_ID_CHECK(unit, dest_id);

    if (object->source_flags & BCM_MIRROR_SOURCE_INGRESS) {
        source.type = BCMINT_MIRROR_ST_F_PORT_ING;
        source.port = BCM_PORT_INVALID;
        /* Search Mirror session id. */
        rv = mirror_session_id_get_by_dest(unit,
                                           &source,
                                           dest_id,
                                           -1,
                                           &count,
                                           session_id);
        if (rv == SHR_E_NONE) {
            for (i = 0; i < count; i++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_mirror_session_stat_get(unit,
                                                       session_id[i],
                                                       sync,
                                                       &value_tmp));
                val += value_tmp;
            }
        } else if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    if (object->source_flags & BCM_MIRROR_SOURCE_EGRESS) {
        source.type = BCMINT_MIRROR_ST_F_PORT_EGR;
        source.port = BCM_PORT_INVALID;
        /* Search Mirror session id. */
        rv = mirror_session_id_get_by_dest(unit,
                                           &source,
                                           dest_id,
                                           -1,
                                           &count,
                                           session_id);
        if (rv == SHR_E_NONE) {
            for (i = 0; i < count; i++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_mirror_session_stat_get(unit,
                                                       session_id[i],
                                                       sync,
                                                       &value_tmp));
                val += value_tmp;
            }
        } else if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    *value = val;

exit:
    SHR_FREE(session_id);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set stat for a mirror stat object.
 *
 * \param [in] unit Unit number.
 * \param [in] object Mirror stat object.
 * \param [in] value Stat value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_stat_set(int unit, bcm_mirror_stat_object_t *object, uint64_t value)
{
    int dest_id, rv, count = 0, i;
    int *session_id = NULL;
    int size = 0;
    bcmint_mirror_source_t source;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(object, SHR_E_PARAM);

    size = MIRROR_CONTAINER_NUM(unit) * sizeof(int);
    session_id = sal_alloc(size, "ltswMirrorSession");
    SHR_NULL_CHECK(session_id, SHR_E_MEMORY);
    sal_memset(session_id, 0xff, size);

    dest_id = BCM_GPORT_MIRROR_GET(object->mirror_dest_id);
    MIRROR_DEST_ID_CHECK(unit, dest_id);

    if (object->source_flags & BCM_MIRROR_SOURCE_INGRESS) {
        source.type = BCMINT_MIRROR_ST_F_PORT_ING;
        source.port = BCM_PORT_INVALID;
        /* Search Mirror session id. */
        rv = mirror_session_id_get_by_dest(unit,
                                           &source,
                                           dest_id,
                                           -1,
                                           &count,
                                           session_id);
        if (rv == SHR_E_NONE) {
            for (i = 0; i < count; i++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_mirror_session_stat_set(unit,
                                                       session_id[i],
                                                       value));
            }
        } else if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    if (object->source_flags & BCM_MIRROR_SOURCE_EGRESS) {
        source.type = BCMINT_MIRROR_ST_F_PORT_EGR;
        source.port = BCM_PORT_INVALID;
        /* Search Mirror session id. */
        rv = mirror_session_id_get_by_dest(unit,
                                           &source,
                                           dest_id,
                                           -1,
                                           &count,
                                           session_id);
        if (rv == SHR_E_NONE) {
            for (i = 0; i < count; i++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_mirror_session_stat_set(unit,
                                                       session_id[i],
                                                       value));
            }
        } else if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FREE(session_id);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a sample profile id.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Sample type.
 * \param [out] id Profile id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_sample_profile_id_alloc(int unit, bcmint_mirror_sample_type_t type,
                               int *id)
{
    int i, size;
    SHR_BITDCL *bitmap = NULL;
    SHR_FUNC_ENTER(unit);

    bitmap = MIRROR_SAMPLE_PRF_BMP(unit, type);
    size = MIRROR_SAMPLE_PRF_NUM(unit);
    SHR_NULL_CHECK(bitmap, SHR_E_INIT);

    for (i = 0; i < size; i++) {
        if (i == 0 && MIRROR_SAMPLE_RSVD_PRF_ID_ZERO(unit)) {
            continue;
        }

        if (!SHR_BITGET(bitmap, i)) {
            break;
        }
    }

    if (i >= size) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    *id = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hold a sample profile id.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Sample type.
 * \param [in] id Profile id.
 */
static void
mirror_sample_profile_id_hold(int unit, bcmint_mirror_sample_type_t type, int id)
{
    if (id >= MIRROR_SAMPLE_PRF_NUM(unit)) {
        return;
    }
    MIRROR_SAMPLE_PRF_BMP_USED_SET(unit, type, id);
    return;
}

/*!
 * \brief Free a sample profile id.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Sample type.
 * \param [out] id Profile id.
 */
static void
mirror_sample_profile_id_free(int unit, bcmint_mirror_sample_type_t type, int id)
{
    if (id >= MIRROR_SAMPLE_PRF_NUM(unit)) {
        return;
    }
    MIRROR_SAMPLE_PRF_BMP_USED_CLR(unit, type, id);
    return;
}

/*!
 * \brief Create a sample profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] options Options flags.
 * \param [in] type Sample type.
 * \param [out] profile_id Profile id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_sample_profile_create(int unit, uint32_t options,
                             bcmint_mirror_sample_type_t type, int *profile_id)
{
    int id;
    SHR_FUNC_ENTER(unit);

    if (options & BCM_MIRROR_SAMPLE_PROFILE_OPTIONS_WITH_ID) {
        id = *profile_id;
        MIRROR_SAMPLE_PRF_ID_CHECK(unit, id);

        if (MIRROR_SAMPLE_PRF_BMP_USED_GET(unit, type, id)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        /* Mark profile id is used. */
        mirror_sample_profile_id_hold(unit, type, id);
    } else {
        /* Allocate sample profile id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_sample_profile_id_alloc(unit, type, &id));

        /* Mark profile id is used. */
        mirror_sample_profile_id_hold(unit, type, id);
        *profile_id = id;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a sample profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Sample type.
 * \param [in] profile_id Profile id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
mirror_sample_profile_destroy(int unit, bcmint_mirror_sample_type_t type,
                              int profile_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_SAMPLE_PRF_ID_CHECK(unit, profile_id);

    if (!MIRROR_SAMPLE_PRF_BMP_USED_GET(unit, type, profile_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Free sample profile id. */
    mirror_sample_profile_id_free(unit, type, profile_id);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump Mirror sample profile used bitmap.
 *
 * \param [in] unit Unit number.
 */
static void
mirror_sample_profile_bmp_dump(int unit)
{
    int i, j, size;
    SHR_BITDCL *bmp;

    for (i = 0; i < bcmintMirrorSampleCount; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "  Mirror %s sample: "),
                 i ? "Flex" : "Ingress"));
        bmp = MIRROR_SAMPLE_PRF_BMP(unit, i);
        size = SHRi_BITDCLSIZE(MIRROR_SAMPLE_PRF_NUM(unit));
        for (j = 0; j < size; j++) {
            LOG_CLI((BSL_META_U(unit,"%8x "), bmp[j]));
        }

        LOG_CLI((BSL_META_U(unit, "\n")));
    }
}

/******************************************************************************
 * Public Functions
 */

int
bcmi_ltsw_mirror_egress_sampler_rate_range_max(int unit,  int *range_max)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_max, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_egress_sampler_rate_range_max(unit, range_max));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* If not initialized, return success. */
    if (MIRROR_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    MIRROR_INFO(unit)->inited = false;

    /* Chip specific mirror deinit. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_misc_deinit(unit));

    /* Deinit Mirror encap profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_deinit(unit));

    /* Deinit Mirror control info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_ctrl_info_deinit(unit));

    /* Destroy mutex. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_lock_destroy(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_port_attach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_flex_port_set(unit, port, true));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_port_detach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_flex_port_set(unit, port, false));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_sampler_seed_set(int unit,
                                  bcmi_ltsw_mirror_sampler_seed_type_t type,
                                  int value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    if (type < bcmiSamplerSeedIngress ||
        type >= bcmiSamplerSeedCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sampler_seed_set(unit, type, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_sampler_seed_get(int unit,
                                  bcmi_ltsw_mirror_sampler_seed_type_t type,
                                  int *value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    if (type < bcmiSamplerSeedIngress ||
        type >= bcmiSamplerSeedCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sampler_seed_get(unit, type, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_sample_version_set(int unit, int value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sample_version_set(unit, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_sample_version_get(int unit, int *value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sample_version_get(unit, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_psamp_ipfix_version_set(int unit, int value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_psamp_ipfix_version_set(unit, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_psamp_ipfix_version_get(int unit, int *value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_psamp_ipfix_version_get(unit, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_psamp_epoch_set(int unit, int value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_psamp_epoch_set(unit, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_psamp_epoch_get(int unit, int *value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_psamp_epoch_get(unit, value));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_uc_cos_set(int unit, int uc_cos)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_uc_cos_set(unit, uc_cos));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_uc_cos_get(int unit, int *uc_cos)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    SHR_NULL_CHECK(uc_cos, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_uc_cos_get(unit, uc_cos));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_mc_cos_set(int unit, int mc_cos)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_mc_cos_set(unit, mc_cos));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_mc_cos_get(int unit, int *mc_cos)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    SHR_NULL_CHECK(mc_cos, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_mc_cos_get(unit, mc_cos));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_cpu_cos_set(int unit, int cpu_cos)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_cpu_cos_set(unit, cpu_cos));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_cpu_cos_get(int unit, int *cpu_cos)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    SHR_NULL_CHECK(cpu_cos, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_cpu_cos_get(unit, cpu_cos));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_invalid_vlan_drop_set(int unit, int enable)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_invalid_vlan_drop_set(unit, enable));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_invalid_vlan_drop_get(int unit, int *enable)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_invalid_vlan_drop_get(unit, enable));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_pkt_checks_set(int unit, int enable)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_pkt_checks_set(unit, enable));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_pkt_checks_get(int unit, int *enable)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_pkt_checks_get(unit, enable));

exit:
    if (locked == true) {
        MIRROR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_field_dest_add(int unit, int entry_id, int flags,
                                bcm_gport_t mirror_dest,
                                bcmi_ltsw_mirror_info_t *info)
{
    int dest_id = -1, rv;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest);

    MIRROR_LOCK(unit);
    rv = mirror_dest_fp_attach(unit,
                               entry_id,
                               flags,
                               dest_id,
                               info);
    MIRROR_UNLOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_field_dest_get(int unit, int entry_id, int flags,
                                bcm_gport_t mirror_dest,
                                bcmi_ltsw_mirror_info_t *info)
{
    int dest_id = -1, rv;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest);

    MIRROR_LOCK(unit);
    rv = mirror_dest_fp_get(unit,
                            entry_id,
                            flags,
                            dest_id,
                            info);
    MIRROR_UNLOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_field_dest_delete(int unit, int entry_id, int flags,
                                   bcm_gport_t mirror_dest)
{
    int dest_id = -1, rv;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest);

    MIRROR_LOCK(unit);
    rv = mirror_dest_fp_detach(unit, entry_id, flags, dest_id);
    MIRROR_UNLOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_dest_id_get(int unit, bcmi_ltsw_mirror_info_t *info,
                             bcm_gport_t *mirror_dest, int *flags)
{
    int session_id, dest_id;
    bool egress;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(mirror_dest, SHR_E_PARAM);

    if (info->mirror_cont >= bcmiMirrorConLast ||
        info->mirror_index >= MIRROR_INSTANCE_NUM(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (MIRROR_SESSION_MODEL(unit) == bcmintMirrorSessionModelExclusive) {
        session_id = info->mirror_cont * MIRROR_INSTANCE_NUM(unit) +
                     info->mirror_index;
    } else {
        session_id = info->mirror_index;
    }

    MIRROR_SESSION_ID_CHECK(unit, session_id);

    MIRROR_LOCK(unit);
    if (MIRROR_SESSION_REF_COUNT(unit, session_id) <= 0) {
        MIRROR_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    dest_id = MIRROR_SESSION(unit, session_id)->mirror_dest_id;
    egress = MIRROR_SESSION(unit, session_id)->egr;
    MIRROR_UNLOCK(unit);

    BCM_GPORT_MIRROR_SET(*mirror_dest, dest_id);
    if (flags) {
        *flags = egress ? BCMI_LTSW_MIRROR_EGRESS : BCMI_LTSW_MIRROR_INGRESS;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_field_destination_add(int unit,
                                       bcm_field_destination_match_t *match,
                                       bcm_gport_t mirror_dest,
                                       bcmi_ltsw_mirror_info_t *info)
{
    int dest_id = -1, rv;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    dest_id = BCM_GPORT_MIRROR_GET(mirror_dest);

    MIRROR_LOCK(unit);
    rv = mirror_dest_field_destination_attach(unit,
                                              match,
                                              dest_id,
                                              info);
    MIRROR_UNLOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_field_destination_get(int unit,
                                       bcm_field_destination_match_t *match,
                                       bcm_gport_t *mirror_dest)
{
    int dest_id = -1, rv;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(mirror_dest, SHR_E_PARAM);
    MIRROR_LOCK(unit);
    rv = mirror_dest_field_destination_get(unit,
                                           match,
                                           &dest_id);
    MIRROR_UNLOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    BCM_GPORT_MIRROR_SET(*mirror_dest, dest_id);
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_field_destination_delete(int unit,
                                          bcm_field_destination_match_t *match)
{
    int rv;
    SHR_FUNC_ENTER(unit);
    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    rv = mirror_dest_field_destination_detach(unit, match);
    MIRROR_UNLOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_mirror_session_ref_cnt_get(int unit, int id, int *cnt)
{
    SHR_FUNC_ENTER(unit);
    MIRROR_SESSION_ID_CHECK(unit, id);

    *cnt = MIRROR_SESSION_REF_COUNT(unit, id);
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_mirror_ingress_mod_event_map_info_get(
    int unit,
    bcmint_mirror_ingress_mod_event_map_info_t **ingress_mod_event_map_tab,
    int *map_tab_size)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ingress_mod_event_map_tab, SHR_E_MEMORY);

    if (MIRROR_CTRL(unit)->ingress_mod_event_map_tab == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *ingress_mod_event_map_tab = MIRROR_CTRL(unit)->ingress_mod_event_map_tab;
    *map_tab_size = MIRROR_CTRL(unit)->ingress_mod_event_map_tab_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_local_port_get(int unit, bcm_gport_t gport,
                                bool *is_remote,
                                bcm_port_t *local_port)
{
    int my_modid, modid;
    bcm_trunk_t trunk_id;
    bcm_gport_t local_modport;
    int id;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(local_port, SHR_E_PARAM);

    if (is_remote) {
        *is_remote = false;
    }

    if (BCM_GPORT_IS_MODPORT(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);

        if (modid != my_modid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stk_remote_port_modport_get(unit,
                                                       gport,
                                                       &local_modport));
            if (BCM_GPORT_IS_SET(local_modport)) {
                *local_port = BCM_GPORT_MODPORT_PORT_GET(local_modport);
            } else {
                *local_port = local_modport;
            }

            if (is_remote) {
                *is_remote = true;
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit,
                                              gport,
                                              &modid,
                                              local_port,
                                              &trunk_id,
                                              &id));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit,
                                          gport,
                                          &modid,
                                          local_port,
                                          &trunk_id,
                                          &id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_destination_create(int unit,
                                   bcm_mirror_destination_t *mirror_dest)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_dest_create(unit, mirror_dest));

    MIRROR_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_destination_destroy(int unit, bcm_gport_t mirror_dest_id)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_delete(unit, BCM_GPORT_MIRROR_GET(mirror_dest_id)));
exit:
    if (locked) {
        MIRROR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_destination_get(int unit, bcm_gport_t mirror_dest_id,
                                bcm_mirror_destination_t *mirror_dest)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    if (mirror_dest == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);
    locked = true;

    if (BCM_GPORT_INVALID == mirror_dest_id) {
        SHR_IF_ERR_EXIT(mirror_dest_search(unit,
                                           mirror_dest->gport,
                                           NULL,
                                           mirror_dest));

        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_get(unit,
                         BCM_GPORT_MIRROR_GET(mirror_dest_id),
                         mirror_dest));
exit:
    if (locked) {
        MIRROR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_destination_traverse(int unit,
                                     bcm_mirror_destination_traverse_cb cb,
                                     void *user_data)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_dest_traverse(unit, cb, user_data));
exit:
    if (locked) {
        MIRROR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_egress_get(int unit, bcm_port_t port, int *val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_egress_set(int unit, bcm_port_t port, int val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_ingress_get(int unit, bcm_port_t port, int *val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_ingress_set(int unit, bcm_port_t port, int val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* If Mirror is already initialized, detach it first. */
    if (MIRROR_INFO(unit)->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mirror_detach(unit));
    }

    /* Create mutex. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_lock_create(unit));

    /* Init Mirror control info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_ctrl_info_init(unit));

    /* Init Mirror bookkeeping info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_bk_init(unit));

    /* Chip specific mirror init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_misc_init(unit));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_dest_recover(unit));
    }

    MIRROR_INFO(unit)->inited = true;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_mode_get(int unit, int *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_mode_set(int unit, int mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_port_dest_add(int unit, bcm_port_t port, uint32 flags,
                              bcm_gport_t mirror_dest_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_port_dest_params_validate(unit, port, flags));

    MIRROR_DEST_ID_CHECK(unit, BCM_GPORT_MIRROR_GET(mirror_dest_id));

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_port_dest_add(unit, port, flags, mirror_dest_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_port_dest_delete(int unit, bcm_port_t port, uint32 flags,
                                 bcm_gport_t mirror_dest_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_port_dest_params_validate(unit, port, flags));

    MIRROR_DEST_ID_CHECK(unit, BCM_GPORT_MIRROR_GET(mirror_dest_id));

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_port_dest_delete(unit, port, flags, mirror_dest_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_port_dest_delete_all(int unit, bcm_port_t port, uint32 flags)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_port_dest_params_validate(unit, port, flags));

    /* Do not support delete all for sFlow, INT, ELEPHANT and DLB_MONITOR. */
    if (flags & (BCM_MIRROR_PORT_SFLOW | BCM_MIRROR_PORT_INT |
        BCM_MIRROR_PORT_ELEPHANT | BCM_MIRROR_PORT_DLB_MONITOR)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_port_dest_delete(unit, port, flags, -1));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_port_dest_get(int unit, bcm_port_t port, uint32 flags,
                              int mirror_dest_size,
                              bcm_gport_t *mirror_dest,
                              int *mirror_dest_count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_port_dest_params_validate(unit, port, flags));

    if (mirror_dest_count == NULL ||
        (mirror_dest_size != 0 && NULL == mirror_dest)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_port_dest_get(unit,
                              port,
                              flags,
                              mirror_dest_size,
                              mirror_dest,
                              mirror_dest_count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_port_get(int unit, bcm_port_t port, bcm_module_t *dest_mod,
                         bcm_port_t *dest_port, uint32 *flags)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    if (flags == NULL || dest_mod == NULL || dest_port == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate local port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_port_get(unit, port, dest_mod, dest_port, flags));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_port_set(int unit, bcm_port_t port, bcm_module_t dest_mod,
                         bcm_port_t dest_port, uint32 flags)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    /* Validate local port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    /* If mirroring is completely disabled, remove all mirror destinations. */
    if (flags == 0 && dest_mod == -1 && dest_port == -1) {
        flags = BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_mirror_port_dest_delete_all(unit, port, flags));

        SHR_EXIT();
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_port_set(unit, port, dest_mod, dest_port, flags));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_to_get(int unit, bcm_port_t *port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_to_set(int unit, bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_mirror_vlan_get(int unit, bcm_port_t port, uint16 *tpid, uint16 *vlan)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_vlan_get(unit, port, tpid, vlan));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_vlan_set(int unit, bcm_port_t port, uint16 tpid, uint16 vlan)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_vlan_set(unit, port, tpid, vlan));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_stat_get(int unit, bcm_mirror_stat_object_t *object,
                         bcm_mirror_stat_t type,
                         uint64 *value)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    if (type >= bcmMirrorStatCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_stat_get(unit, object, false, value));

    MIRROR_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_stat_sync_get(int unit, bcm_mirror_stat_object_t *object,
                              bcm_mirror_stat_t type,
                              uint64 *value)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    if (type >= bcmMirrorStatCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_stat_get(unit, object, true, value));

    MIRROR_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_stat_set(int unit, bcm_mirror_stat_object_t *object,
                         bcm_mirror_stat_t type,
                         uint64 value)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    if (type >= bcmMirrorStatCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_stat_set(unit, object, value));

    MIRROR_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_source_dest_add(int unit, bcm_mirror_source_t *source,
                                bcm_gport_t mirror_dest_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_DEST_ID_CHECK(unit, BCM_GPORT_MIRROR_GET(mirror_dest_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_source_params_validate(unit, source));

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_source_dest_add(unit, source, mirror_dest_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_source_dest_delete(int unit, bcm_mirror_source_t *source,
                                   bcm_gport_t mirror_dest_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_DEST_ID_CHECK(unit, BCM_GPORT_MIRROR_GET(mirror_dest_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_source_params_validate(unit, source));

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_source_dest_delete(unit, source, mirror_dest_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_source_dest_delete_all(int unit, bcm_mirror_source_t *source)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_source_params_validate(unit, source));

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_source_dest_delete(unit, source, -1));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_source_dest_get_all(int unit, bcm_mirror_source_t *source,
                                    int array_size,
                                    bcm_gport_t *mirror_dest,
                                    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_source_params_validate(unit, source));

    if (count == NULL ||
        (array_size != 0 && NULL == mirror_dest)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mirror_source_dest_get(unit,
                                source,
                                array_size,
                                mirror_dest,
                                count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_payload_zero_control_multi_get(int unit,
        bcm_mirror_payload_zero_info_t *pyld_zero_cfg,
        int num_elems)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_payload_zero_control_multi_get(unit,
                                                         pyld_zero_cfg,
                                                         num_elems));
    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_payload_zero_control_multi_set(int unit,
        bcm_mirror_payload_zero_info_t *pyld_zero_cfg,
        int num_elems)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_payload_zero_control_multi_set(unit,
                                                         pyld_zero_cfg,
                                                         num_elems));
    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_payload_zero_profile_create(int unit,
        bcm_mirror_payload_zero_offsets_t *mirror_payload_offset_info,
        uint32 flags,
        uint32 *profile_index)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_payload_zero_profile_create(unit,
                                                      mirror_payload_offset_info,
                                                      flags,
                                                      profile_index));
    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_sample_profile_create(int unit, uint32 options,
                                      bcm_mirror_sample_type_t type,
                                      int *profile_id)
{
    bcmint_mirror_sample_type_t sample_type;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit);

    sample_type = type;
    if (sample_type >= bcmintMirrorSampleCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_sample_profile_create(unit, options, sample_type, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_payload_zero_profile_destroy(int unit,
    uint32 flags,
    uint32 profile_index)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_payload_zero_profile_destroy(unit,
                                                       flags,
                                                       profile_index));
    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_sample_profile_destroy(int unit, bcm_mirror_sample_type_t type,
                                       int profile_id)
{
    bcmint_mirror_sample_type_t sample_type;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit);

    sample_type = type;
    if (sample_type >= bcmintMirrorSampleCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mirror_sample_profile_destroy(unit, sample_type, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_sample_profile_set(int unit, bcm_mirror_sample_type_t type,
                                   int profile_id,
                                   bcm_mirror_sample_profile_t *profile)
{
    bcmint_mirror_sample_type_t sample_type;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit);

    sample_type = type;
    if (sample_type >= bcmintMirrorSampleCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sample_profile_set(unit, sample_type,
                                             profile_id,
                                             profile));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_sample_profile_get(int unit, bcm_mirror_sample_type_t type,
                                   int profile_id,
                                   bcm_mirror_sample_profile_t *profile)
{
    bcmint_mirror_sample_type_t sample_type;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit);

    sample_type = type;
    if (sample_type >= bcmintMirrorSampleCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sample_profile_get(unit, sample_type,
                                             profile_id,
                                             profile));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_sample_stat_attach(int unit, bcm_mirror_sample_type_t type,
                                   uint32 stat_counter_id)
{
    bcmint_mirror_sample_type_t sample_type;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit);

    sample_type = type;
    if (sample_type >= bcmintMirrorSampleCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sample_stat_attach(unit, sample_type,
                                             stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_sample_stat_detach(int unit, bcm_mirror_sample_type_t type,
                                   uint32 stat_counter_id)
{
    bcmint_mirror_sample_type_t sample_type;
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);
    MIRROR_SAMPLE_PRF_SUPPORT_CHECK(unit);

    sample_type = type;
    if (sample_type >= bcmintMirrorSampleCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_sample_stat_detach(unit, sample_type,
                                             stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_group_create(
    int unit,
    bcm_mirror_ingress_mod_event_t *event_array,
    int num_events,
    bcm_mirror_ingress_mod_event_group_t *group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_group_create(unit,
                                                         event_array,
                                                         num_events,
                                                         group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_group_destroy(
    int unit,
    bcm_mirror_ingress_mod_event_group_t group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_group_destroy(unit, group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_group_get_all(
    int unit,
    bcm_mirror_ingress_mod_event_group_t *group_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_group_get_all(unit,
                                                          group_array,
                                                          count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_group_get(
    int unit,
    bcm_mirror_ingress_mod_event_group_t group,
    bcm_mirror_ingress_mod_event_t *event_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_group_event_get(unit,
                                                            group,
                                                            event_array,
                                                            count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_create(
    int unit,
    bcm_mirror_ingress_mod_event_profile_t* profile,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_create(unit,
                                                           profile,
                                                           profile_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_destroy(unit, profile_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_get_all(
    int unit,
    int *profile_id_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_get_all(unit,
                                                            profile_id_array,
                                                            count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_get(
    int unit,
    int profile_id,
    bcm_mirror_ingress_mod_event_profile_t* profile)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_cfg_get(unit,
                                                            profile_id,
                                                            profile));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_group_add(
    int unit,
    int profile_id,
    bcm_mirror_ingress_mod_event_group_t group,
    int hit)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_group_add(unit,
                                                              profile_id,
                                                              group,
                                                              hit));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_group_delete(
    int unit,
    int profile_id,
    bcm_mirror_ingress_mod_event_group_t group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_group_delete(unit,
                                                                 profile_id,
                                                                 group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_group_get_all(
    int unit,
    int profile_id,
    bcm_mirror_ingress_mod_event_group_t *group_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_group_get_all(unit,
                                                                  profile_id,
                                                                  group_array,
                                                                  count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_ingress_mod_event_profile_group_get(
    int unit,
    int profile_id,
    bcm_mirror_ingress_mod_event_group_t group,
    int *hit)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_ingress_mod_event_profile_group_get(unit,
                                                              profile_id,
                                                              group,
                                                              hit));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv4_create(
    int unit,
    bcm_mirror_multi_dip_entry_num_t num_entries,
    bcm_ip_t *ipv4_addr_array,
    bcm_mirror_multi_dip_group_t *group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv4_create(unit,
                                                      num_entries,
                                                      ipv4_addr_array,
                                                      group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv4_destroy(
    int unit,
    bcm_mirror_multi_dip_group_t group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv4_destroy(unit, group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv4_get_all(
    int unit,
    bcm_mirror_multi_dip_group_t *ipv4_group_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv4_get_all(unit,
                                                       ipv4_group_array,
                                                       count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv4_get(
    int unit,
    bcm_mirror_multi_dip_group_t group,
    bcm_ip_t *ipv4_addr_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv4_get(unit,
                                                   group,
                                                   ipv4_addr_array,
                                                   count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv6_create(
    int unit,
    bcm_mirror_multi_dip_entry_num_t num_entries,
    bcm_ip6_t *ipv6_addr_array,
    bcm_mirror_multi_dip_group_t *group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv6_create(unit,
                                                      num_entries,
                                                      ipv6_addr_array,
                                                      group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv6_destroy(
    int unit,
    bcm_mirror_multi_dip_group_t group)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv6_destroy(unit, group));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv6_get_all(
    int unit,
    bcm_mirror_multi_dip_group_t *ipv6_group_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv6_get_all(unit,
                                                       ipv6_group_array,
                                                       count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_multi_dip_group_ipv6_get(
    int unit,
    bcm_mirror_multi_dip_group_t group,
    bcm_ip6_t *ipv6_addr_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_multi_dip_group_ipv6_get(unit,
                                                   group,
                                                   ipv6_addr_array,
                                                   count));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_truncate_length_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcm_mirror_truncate_length_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (profile->mode == bcmMirrorTruncateDefault &&
        profile->adjustment_length != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_truncate_length_profile_create(unit,
                                                         options,
                                                         profile_id,
                                                         profile));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_truncate_length_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_truncate_length_profile_destroy(unit,
                                                          profile_id));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_truncate_length_profile_get(
    int unit,
    int profile_id,
    bcm_mirror_truncate_length_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_truncate_length_profile_get(unit,
                                                      profile_id,
                                                      profile));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_global_truncate_set(
    int unit,
    bcm_mirror_global_truncate_t *global_truncate)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_global_truncate_set(unit, global_truncate));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_mirror_global_truncate_get(
    int unit,
    bcm_mirror_global_truncate_t *global_truncate)
{
    SHR_FUNC_ENTER(unit);

    MIRROR_INIT_CHECK(unit);

    MIRROR_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_mirror_global_truncate_get(unit, global_truncate));

    MIRROR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_mirror_switch_control_set(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_mirror_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_mirror_switch_control_get(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}


void
bcmi_ltsw_mirror_sw_dump(int unit)
{
    int i;
    mirror_cfg_t *mi = MIRROR_INFO(unit);
    const char *str[] = BCMINT_MIRROR_SESSION_MODEL_STR;

    if (!mi->inited) {
        LOG_CLI((BSL_META_U(unit,
                            "\nMIRROR module isn't initialized: Unit %d\n"),
                            unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information MIRROR - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init       : %4d\n"), mi->inited));
    LOG_CLI((BSL_META_U(unit,
                        "  Dest num       : %4d\n"), mi->ctrl_data.dest_num));
    LOG_CLI((BSL_META_U(unit,
                        "  Container num       : %4d\n"),
                        mi->ctrl_data.container_num));
    LOG_CLI((BSL_META_U(unit,
                        "  Session model       : %s\n"),
                        str[mi->ctrl_data.session_attrs.model]));
    LOG_CLI((BSL_META_U(unit,
                        "  Session array num       : %4d\n"),
                        mi->ctrl_data.session_attrs.session_array_num));
    LOG_CLI((BSL_META_U(unit,
                        "  Session instance num       : %4d\n"),
                        mi->ctrl_data.session_attrs.instance_num));
    LOG_CLI((BSL_META_U(unit,
                        "  Session num       : %4d\n"),
                        mi->ctrl_data.session_attrs.session_num));
    LOG_CLI((BSL_META_U(unit,
                        "  Session Reserve instance zero       : %s\n"),
                        mi->ctrl_data.session_attrs.rsvd_instance_zero ?
                        "True":"False"));
    LOG_CLI((BSL_META_U(unit,
                        "  Sample profile num       : %4d\n"),
                        mi->ctrl_data.sample_prf_info.prf_num));
    LOG_CLI((BSL_META_U(unit,
                        "  Reserve sample profile zero       : %s\n"),
                        mi->ctrl_data.sample_prf_info.rsvd_prf_id_zero ?
                        "True":"False"));
    for (i = 0; i < mi->ctrl_data.container_num; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "      MC[%d]       : 0x%08x\n"),
                            i, mi->ctrl_data.mc_st_flags[i]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "----------------%20s----------------\n"),
                        "Mirror destinations"));
    mirror_dest_dump_all(unit);

    LOG_CLI((BSL_META_U(unit,
                        "----------------%20s----------------\n"),
                        "Mirror sessions"));
    mirror_session_dump(unit);

    LOG_CLI((BSL_META_U(unit,
                        "----------------%20s----------------\n"),
                        "Mirror containers"));
    mirror_container_dump(unit);

    LOG_CLI((BSL_META_U(unit,
                        "  Mirror-on-drop destination num       : %4d\n"),
                        mi->ctrl_data.mod_dest_num));
    for (i = 0; i < mi->ctrl_data.mod_dest_num; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "  Mirror destination ids for MOD: dest[%4d]      : 0x%08x\n"),
                            i, mi->cosq_mod_ctrl.mod_mirror_dest_id[i]));
    }

    mirror_sample_profile_bmp_dump(unit);

    mbcm_ltsw_mirror_multi_dip_dump(unit);

    return;
}

