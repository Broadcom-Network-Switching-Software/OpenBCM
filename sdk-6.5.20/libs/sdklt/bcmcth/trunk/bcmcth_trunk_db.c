/*! \file bcmcth_trunk_db.c
 *
 * bcmcth_trunk database
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_pt.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_alloc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmcth/bcmcth_trunk_db.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_vp_db_internal.h>
#include <bcmcth/bcmcth_trunk_vp_imm.h>
/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*! Configuration info for TRUNK component. */
typedef struct bcmcth_trunk_cfg_s {
    /*! Indicate whether fast trunk mode is enabled. */
    bool                fast_enable;
} bcmcth_trunk_cfg_t;

static bcmcth_trunk_cfg_t *bcmcth_trunk_cfg_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Local definitions
 */
static bcmcth_trunk_vp_control_t *trunk_vp_ctrl[BCMDRD_CONFIG_MAX_UNITS] = {0};

#define BCMCTH_TRUNK_CFG_INFO(_u_) (bcmcth_trunk_cfg_info[_u_])

#define BCMCTH_TRUNK_FAST_ENABLE(_u_) \
    (BCMCTH_TRUNK_CFG_INFO(_u_)->fast_enable)

/* Check that bcmcth_trunk has been initialized */
#define BCMCTH_TRUNK_INIT(_u_)  \
    (BCMCTH_TRUNK_CFG_INFO(_u_) != NULL)

#define BCMCTH_NUM_MAP_DEST 1

/*!
 * \brief Allocates TRUNK_VP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_trunk_vp_resources_alloc(int unit)
{
    uint32_t size = 0;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmcth_trunk_vp_resources_alloc.\n")));

    if (!trunk_vp_ctrl[unit]) {
        BCMCTH_TRUNK_VP_ALLOC
            (trunk_vp_ctrl[unit],
             sizeof(bcmcth_trunk_vp_control_t),
             "bcmcthTrunkVpSysEcmpCtrl");
        size = sizeof(bcmcth_trunk_vp_control_t);
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "TRUNK_VP control is %"PRIu32".\n"),
                  size));

        /* Create the TRUNK_VP CTH control structure synchronization lock. */
        trunk_vp_ctrl[unit]->trunk_vp_lock =
            sal_mutex_create("bcmcthTrunkVpCtrlLock");
        if (!trunk_vp_ctrl[unit]->trunk_vp_lock) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Free TRUNK_VP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_trunk_vp_resources_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmcth_trunk_vp_resources_free.\n")));

    if (trunk_vp_ctrl[unit]) {
        if (trunk_vp_ctrl[unit]->trunk_vp_lock) {
            sal_mutex_destroy(trunk_vp_ctrl[unit]->trunk_vp_lock);
        }
        BCMCTH_TRUNK_VP_FREE(trunk_vp_ctrl[unit]);
    }
    SHR_FUNC_EXIT();
}


bool
bcmcth_trunk_fast_check(int unit)
{
    return BCMCTH_TRUNK_FAST_ENABLE(unit);
}

int
bcmcth_trunk_init(int unit, bool warm)
{
    bool fast_mode = FALSE;
    uint64_t value = 0;
    int rv = SHR_E_NONE;
    int rv1 = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_t *vmap = NULL;
    const bcmlrd_map_t *wmap = NULL;
    bcmlrd_table_desc_t map_dest[BCMCTH_NUM_MAP_DEST];
    size_t actual;
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;

    SHR_FUNC_ENTER(unit);

    if (BCMCTH_TRUNK_INIT(unit)) {
        SHR_EXIT();
    }
    BCMCTH_TRUNK_CFG_INFO(unit) =
        sal_alloc(BCMDRD_CONFIG_MAX_UNITS * sizeof(bcmcth_trunk_cfg_t),
                  "bcmcthTrunkCfg");
    SHR_NULL_CHECK(BCMCTH_TRUNK_CFG_INFO(unit), SHR_E_MEMORY);
    sal_memset(BCMCTH_TRUNK_CFG_INFO(unit), 0,
               BCMDRD_CONFIG_MAX_UNITS * sizeof(bcmcth_trunk_cfg_t));
    sal_memset(map_dest, 0, sizeof(bcmlrd_table_desc_t));
    rv = bcmlrd_map_get(unit, TRUNK_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        rv = bcmcfg_field_get(unit, TRUNK_CONFIGt,
                              TRUNK_CONFIGt_FAST_MODEf, &value);
        if (rv == SHR_E_NONE) {
            fast_mode = !!value;
        }
        BCMCTH_TRUNK_FAST_ENABLE(unit) = fast_mode;
        SHR_IF_ERR_EXIT(bcmcth_trunk_drv_mode_set(unit, fast_mode));

        if (!fast_mode) {
            SHR_IF_ERR_EXIT(bcmcth_trunk_grp_init(unit, warm));
        } else {
            SHR_IF_ERR_EXIT(bcmcth_trunk_fast_grp_init(unit, warm));
        }
    } else {
        rv = bcmlrd_map_get(unit, TRUNK_FASTt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_trunk_fast_grp_init(unit, warm));
            BCMCTH_TRUNK_FAST_ENABLE(unit) = TRUE;
        }
        rv = bcmlrd_map_get(unit, TRUNKt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_trunk_imm_grp_init(unit, warm));
        }
    }

    rv = bcmlrd_map_get(unit, TRUNK_FAILOVERt, &map);
    if (SHR_SUCCESS(rv) && map) {
        rv = bcmlrd_mapped_dest_get(
                                    unit,
                                    TRUNK_FAILOVERt,
                                    BCMCTH_NUM_MAP_DEST,
                                    map_dest,
                                    &actual);
        if (SHR_SUCCESS(rv)) {
            if (map_dest[0].kind == BCMLRD_MAP_CUSTOM) {
                SHR_IF_ERR_EXIT(bcmcth_trunk_failover_init(unit, warm));
            }
        }
    }

    rv = bcmlrd_map_get(unit, TRUNK_SYSTEMt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmcth_trunk_imm_sys_grp_init(unit, warm));
    }

    /* Get the TRUNK device variant driver */
    drv_var = bcmcth_trunk_drv_var_get(unit);

    if (drv_var != NULL) {
        vp_drv_var = drv_var->trunk_vp_var;
        SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);
        rv = bcmlrd_map_get(
                unit,
                vp_drv_var->ids->trunk_vp_ltid,
                &vmap);
        rv1 = bcmlrd_map_get(
                 unit,
                 vp_drv_var->ids->trunk_vp_weighted_ltid,
                 &wmap);
        if (SHR_SUCCESS(rv) || SHR_SUCCESS(rv1)) {
            if (vmap || wmap) {
                SHR_IF_ERR_VERBOSE_EXIT(
                   bcmcth_trunk_vp_resources_alloc(unit));
            }

            if (vmap) {
                SHR_IF_ERR_EXIT(
                   bcmcth_trunk_vp_overlay_group_imm_register(
                              unit,
                              vp_drv_var->ids->trunk_vp_ltid));
            }
            if (wmap) {
                SHR_IF_ERR_EXIT(
                  bcmcth_trunk_vp_overlay_group_imm_register(
                     unit,
                     vp_drv_var->ids->trunk_vp_weighted_ltid));
            }
        }
    }
 exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmcth_trunk_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_cleanup(int unit)
{
    int rv = SHR_E_NONE;
    int rv1 = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_t *vmap = NULL;
    const bcmlrd_map_t *wmap = NULL;
    bcmlrd_table_desc_t map_dest[BCMCTH_NUM_MAP_DEST];
    size_t actual;
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;

    SHR_FUNC_ENTER(unit);

   sal_memset(map_dest, 0, sizeof(bcmlrd_table_desc_t));
    rv = bcmlrd_map_get(unit, TRUNK_FAILOVERt, &map);
    if (SHR_SUCCESS(rv) && map) {
        rv = bcmlrd_mapped_dest_get(unit,
                                    TRUNK_FAILOVERt,
                                    BCMCTH_NUM_MAP_DEST,
                                    map_dest,
                                    &actual);
        if (SHR_SUCCESS(rv)) {
            if (map_dest[0].kind == BCMLRD_MAP_CUSTOM) {
                bcmcth_trunk_failover_cleanup(unit);
            }
        }
    }

    rv = bcmlrd_map_get(unit, TRUNK_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        bcmcth_trunk_grp_cleanup(unit);
        bcmcth_trunk_fast_grp_cleanup(unit);
    } else {
        rv = bcmlrd_map_get(unit, TRUNK_FASTt, &map);
        if (SHR_SUCCESS(rv) && map) {
            bcmcth_trunk_fast_grp_cleanup(unit);
        }
        rv =  bcmlrd_map_get(unit, TRUNKt, &map);
        if (SHR_SUCCESS(rv) && map) {
            bcmcth_trunk_imm_grp_cleanup(unit, FALSE);
        }
    }

    rv = bcmlrd_map_get(unit, TRUNK_SYSTEMt, &map);
    if (SHR_SUCCESS(rv) && map) {
        bcmcth_trunk_imm_sys_grp_cleanup(unit, FALSE);
    }

    /* Get the TRUNK device variant driver */
    drv_var = bcmcth_trunk_drv_var_get(unit);
    if (drv_var != NULL) {
        vp_drv_var = drv_var->trunk_vp_var;
        SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);
        rv = bcmlrd_map_get(unit,
                            vp_drv_var->ids->trunk_vp_ltid,
                            &vmap);
        rv1 = bcmlrd_map_get(unit,
                             vp_drv_var->ids->trunk_vp_weighted_ltid,
                             &wmap);
        if (SHR_SUCCESS(rv) || SHR_SUCCESS(rv1)) {
            if (vmap || wmap) {
                SHR_IF_ERR_VERBOSE_EXIT(
                   bcmcth_trunk_vp_resources_free(unit));
            }
        }
   }
 exit:
    SHR_FREE(BCMCTH_TRUNK_CFG_INFO(unit));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get BCM unit TRUNK_VP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \returns Pointer to BCM unit's \c unit TRUNK_VP control structure.
 */
bcmcth_trunk_vp_control_t *bcmcth_trunk_vp_ctrl_get(int unit)
{
    return trunk_vp_ctrl[unit];
}
