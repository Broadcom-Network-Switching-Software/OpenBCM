/*! \file bcmcth_dlb_attach.c
 *
 * BCMCTH Dynamic Load Balance Driver attach/detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmcth/bcmcth_dlb.h>
#include <bcmcth/bcmcth_dlb_drv.h>
#include <bcmcth/bcmcth_dlb_util.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
* Public functions
 */

int
bcmcth_dlb_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t  dev_type;
    const bcmlrd_map_t *ecmp_map = NULL;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_t *trunk_map = NULL;
    bcmlrd_table_desc_t map_dest[1];
    size_t              map_dest_cnt = COUNTOF(map_dest);
    int                 rv = SHR_E_NONE;
    int                 rv1 = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    rv = bcmlrd_map_get(unit, DLB_TRUNKt, &trunk_map);
    rv1 = bcmlrd_map_get(unit, DLB_ECMPt, &ecmp_map);

    if ((SHR_SUCCESS(rv) && trunk_map) ||
        (SHR_SUCCESS(rv1) && ecmp_map)) {
        /* Initialize device-specific driver */
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_drv_init(unit));
    }

    if (ecmp_map) {

        /* Initialize the DLB ECMP component storage */
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_ecmp_resources_alloc(unit, warm));

        /* Initialize DLB ECMP chip level info. */
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_ecmp_init(unit, warm));

        /* Initialize IMM drivers */
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_ecmp_imm_init(unit));

        if (SHR_FAILURE(bcmlrd_map_get(unit, DLB_ECMP_PORT_CONTROLt, &map))) {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT
            (bcmcth_dlb_ecmp_pctl_imm_init(unit));

        rv = bcmlrd_map_get(unit, DLB_ECMP_STATSt, &map);
        if (SHR_SUCCESS(rv) && map) {
            rv = bcmlrd_mapped_dest_get(unit, DLB_ECMP_STATSt, map_dest_cnt,
                                        map_dest, &map_dest_cnt);
            if (SHR_SUCCESS(rv)) {
                if (map_dest[0].kind == BCMLRD_MAP_CUSTOM) {
                    SHR_IF_ERR_EXIT
                        (bcmcth_dlb_ecmp_stats_resources_alloc(unit, warm));
                }
            }
        }
    }
    if (trunk_map) {

        /* Initialize the DLB TRUNK component storage */
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_trunk_resources_alloc(unit, warm));

        /* Initialize DLB TRUNK IMM drivers */
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_trunk_imm_init(unit));

        if (SHR_FAILURE(bcmlrd_map_get(unit, DLB_TRUNK_PORT_CONTROLt, &map))) {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT
            (bcmcth_dlb_trunk_pctl_imm_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_detach(int unit)
{
    const bcmlrd_map_t *ecmp_map = NULL;
    const bcmlrd_map_t *trunk_map = NULL;
    const bcmlrd_map_t *ecmp_stats_map = NULL;
    bcmlrd_table_desc_t map_dest[1];
    size_t              map_dest_cnt = COUNTOF(map_dest);
    int                 rv;

    SHR_FUNC_ENTER(unit);

    if (SHR_SUCCESS(bcmlrd_map_get(unit, DLB_ECMPt, &ecmp_map))) {
        SHR_IF_ERR_EXIT
            (bcmcth_dlb_ecmp_cleanup(unit));
        SHR_IF_ERR_CONT
            (bcmcth_dlb_ecmp_resources_free(unit));
    }

    if (SHR_SUCCESS(bcmlrd_map_get(unit,  DLB_TRUNKt, &trunk_map))) {
        SHR_IF_ERR_CONT
            (bcmcth_dlb_trunk_resources_free(unit));
    }

    rv = bcmlrd_map_get(unit, DLB_ECMP_STATSt, &ecmp_stats_map);
    if (SHR_SUCCESS(rv) && ecmp_stats_map) {
        rv = bcmlrd_mapped_dest_get(unit, DLB_ECMP_STATSt, map_dest_cnt,
                                    map_dest, &map_dest_cnt);
        if (SHR_SUCCESS(rv)) {
            if (map_dest[0].kind == BCMLRD_MAP_CUSTOM) {
                SHR_IF_ERR_EXIT
                    (bcmcth_dlb_ecmp_stats_resources_free(unit));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
