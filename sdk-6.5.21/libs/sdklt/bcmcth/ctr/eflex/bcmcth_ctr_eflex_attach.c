/*! \file bcmcth_ctr_eflex_attach.c
 *
 * BCMCTH Enhanced Flex Counter Driver attach/detach APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmcth/bcmcth_ctr_eflex.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*******************************************************************************
 * Internal public functions
 */

int
bcmcth_ctr_eflex_attach(int unit, bool warm)
{
    int rv;
    bcmdrd_dev_type_t dev_type;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get LT sid from table attributes */
    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_EFLEX_ACTION_PROFILE",
                                     &t_attrib);

    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, t_attrib.id, &ctr_eflex_map);

        if (SHR_SUCCESS(rv) && ctr_eflex_map) {
            /* Initialize device-specific driver */
            SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_drv_init(unit));

            /* Initialize internal structures */
            SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_db_init(unit, warm));

            /* Initialize IMM driver */
            SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_imm_init(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_detach(int unit)
{
    int rv;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};

    SHR_FUNC_ENTER(unit);

    /* Get LT sid from table attributes */
    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_EFLEX_ACTION_PROFILE",
                                     &t_attrib);

    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, t_attrib.id, &ctr_eflex_map);

        if (SHR_SUCCESS(rv) && ctr_eflex_map) {
            /* Cleanup internal structures */
            SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_db_cleanup(unit));

            /* Cleanup device-specific driver */
            SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_drv_cleanup(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_state_eflex_attach(int unit, bool warm)
{
    int rv;
    bcmdrd_dev_type_t dev_type;
    const bcmlrd_map_t *state_eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get LT sid from table attributes */
    rv = bcmlrd_table_attributes_get(unit,
                                     "FLEX_STATE_ING_ACTION_PROFILE",
                                     &t_attrib);

    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, t_attrib.id, &state_eflex_map);

        if (SHR_SUCCESS(rv) && state_eflex_map) {
            /* Initialize device-specific driver */
            SHR_IF_ERR_EXIT(bcmcth_state_eflex_drv_init(unit));

            /* Initialize internal structures */
            SHR_IF_ERR_EXIT(bcmcth_state_eflex_db_init(unit, warm));

            /* Initialize IMM driver */
            SHR_IF_ERR_EXIT(bcmcth_state_eflex_imm_init(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_state_eflex_detach(int unit)
{
    int rv;
    const bcmlrd_map_t *state_eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};

    SHR_FUNC_ENTER(unit);

    /* Get LT sid from table attributes */
    rv = bcmlrd_table_attributes_get(unit,
                                     "FLEX_STATE_ING_ACTION_PROFILE",
                                     &t_attrib);

    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, t_attrib.id, &state_eflex_map);

        if (SHR_SUCCESS(rv) && state_eflex_map) {
            /* Cleanup internal structures */
            SHR_IF_ERR_EXIT(bcmcth_state_eflex_db_cleanup(unit));

            /* Cleanup device-specific driver */
            SHR_IF_ERR_EXIT(bcmcth_state_eflex_drv_cleanup(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}
