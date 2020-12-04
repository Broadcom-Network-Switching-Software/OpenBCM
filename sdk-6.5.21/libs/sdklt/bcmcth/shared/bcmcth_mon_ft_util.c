/*! \file bcmcth_mon_ft_util.c
 *
 * bcmmon utility function
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_mon_sysm.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmcth/bcmcth_mon_flowtracker_util.h>
/*******************************************************************************
 * Defines
 */
/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Alloc LT fields handle
 *
 * \param [in]  unit BCM device number
 * \param [in]  num_fields Number of fields
 * \param [out]  fields Fields alloc handle
 *
 * \return Alloc for specified number of fields
 */
int
bcmcth_mon_ft_util_fields_alloc(int unit, size_t num_fields, bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(fields->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthMonFtFields");

    SHR_NULL_CHECK(fields->field, SHR_E_MEMORY);

    sal_memset(fields->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    fields->count = num_fields;

    for (idx = 0; idx < num_fields; idx++) {
        fields->field[idx] = shr_fmm_alloc();
        SHR_NULL_CHECK(fields->field[idx], SHR_E_MEMORY);
        sal_memset(fields->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmcth_mon_ft_util_fields_free(unit, num_fields, fields);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free LT fields handle
 *
 * \param [in]  unit BCM device number
 * \param [in]  num_fields Number of fields
 * \param [in]  fields Fields to be freed
 *
 * \return Free the fields handle.
 */
int
bcmcth_mon_ft_util_fields_free(int unit, int num_fields, bcmltd_fields_t *fields)
{
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    if (fields->field != NULL) {
        for (idx = 0; idx < num_fields; idx++) {
            if (fields->field[idx] != NULL) {
                shr_fmm_free(fields->field[idx]);
            }
        }
        SHR_FREE(fields->field);
    }

exit:
    SHR_FUNC_EXIT();
}
