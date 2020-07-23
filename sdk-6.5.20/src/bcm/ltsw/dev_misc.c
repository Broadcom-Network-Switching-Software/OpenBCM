/*! \file dev_misc.c
 *
 * Device misc configurations.
 * This file contains misc configurations to make the device behavior work as expected.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/control.h>

#include <bcm_int/ltsw/dev_misc_int.h>
#include <bcm_int/ltsw/dev_misc.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/dev_misc.h>
#include <bcm_int/ltsw/ser.h>

#include <shr/shr_debug.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

bcmint_ltsw_dev_misc_data_t dev_misc[BCM_MAX_NUM_UNITS] = {{0}};

/******************************************************************************
* Private functions
 */
/*!
 * \brief Set the given LT entry data.
 *
 * \param [in]    unit     Unit number.
 * \param [in]    lt         LT data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
ltsw_dev_misc_entry_hdl_set(
    int unit,
    bcmlt_entry_handle_t ent_hdl,
    bcmint_ltsw_dev_misc_entry_t *entry)
{
    bcmint_ltsw_dev_misc_field_t *fld = NULL;
    int  fld_idx;

    SHR_FUNC_ENTER(unit);


    for (fld_idx = 0; fld_idx < entry->field_count; fld_idx++) {
        fld = &(entry->field_data[fld_idx]);
        if ((fld->flags & BCMINT_LTSW_DEV_MISC_FLD_SYM) &&
            (fld->flags & BCMINT_LTSW_DEV_MISC_FLD_ARRAY)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(ent_hdl,
                                                    fld->name,
                                                    fld->index,
                                                    &(fld->value.str),
                                                    1));
        } else if (fld->flags & BCMINT_LTSW_DEV_MISC_FLD_SYM) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(ent_hdl,
                                              fld->name,
                                              fld->value.str));
        } else if (fld->flags & BCMINT_LTSW_DEV_MISC_FLD_ARRAY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(ent_hdl,
                                             fld->name,
                                             fld->index,
                                             &(fld->value.scalar),
                                             1));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld->name, fld->value.scalar));
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the LT  of device misc configurations.
 *
 * \param [in]    unit     Unit number.
 * \param [in]    lt         LT data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
ltsw_dev_misc_lt_set(
    int unit,
    bcmlt_transaction_hdl_t trans_hdl,
    bcmint_ltsw_dev_misc_lt_t *lt)
{
    bcmint_ltsw_dev_misc_entry_t *entry = NULL;
    int entry_idx;
    int dunit = 0;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    for (entry_idx = 0; entry_idx < lt->entry_count; entry_idx++) {
        entry= &(lt->entry_data[entry_idx]);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, lt->name, &ent_hdl));

        SHR_IF_ERR_EXIT
            (ltsw_dev_misc_entry_hdl_set(unit, ent_hdl, entry));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_INSERT,
                                         ent_hdl));
        ent_hdl = BCMLT_INVALID_HDL;
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Load the device misc configurations.
 *
 * \param [in]    unit              Unit number.
 * \param [in]    misc_data     Device misc configurations.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
ltsw_dev_misc_load(int unit, bcmint_ltsw_dev_misc_data_t *misc_data)
{
    bcmlt_transaction_hdl_t trans_hdl;
    bcmint_ltsw_dev_misc_lt_t   *lt;
    int lt_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    for (lt_idx = 0; lt_idx < misc_data->lt_count; lt_idx++) {
        lt = &(misc_data->lt_data[lt_idx]);
        SHR_IF_ERR_EXIT
            (ltsw_dev_misc_lt_set(unit, trans_hdl, lt));
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    

exit:
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }

    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public functions
 */
/*!
 * \brief Clean up the device misc configurations.
 *
 * This function is used to clean up the device misc configuations.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_dev_misc_cleanup(int unit)
{
    bcmint_ltsw_dev_misc_data_t *misc_data = &dev_misc[unit];
    bcmint_ltsw_dev_misc_lt_t   *lt = NULL;
    int lt_idx;

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    for (lt_idx = 0; lt_idx < misc_data->lt_count; lt_idx++) {
        lt = &(misc_data->lt_data[lt_idx]);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, lt->name));
    }

    sal_memset(&dev_misc[unit], 0, sizeof(bcmint_ltsw_dev_misc_data_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the device misc behaviors.
 *
 * \param [in]    unit     Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcmi_ltsw_dev_misc_init(int unit)
{
    int warmboot = bcmi_warmboot_get(unit);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_misc_cleanup(unit));

    rv = mbcm_ltsw_dev_misc_db_get(unit, &dev_misc[unit]);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if (warmboot) {
        SHR_EXIT();
    }

    if (SHR_E_UNAVAIL != rv) {
        SHR_IF_ERR_EXIT
            (ltsw_dev_misc_load(unit, &dev_misc[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}
