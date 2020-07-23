/*! \file bcmltd_field_value_to_symbol.c
 *
 * Return symbol name for symbol type field value.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_internal.h>
#include <bcmltd/bcmltd_table.h>

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Get field symbol for field value
 *
 * \param [in]  flags        Pointer to field default data.
 * \param [in]  edata        Enumeration data.
 * \param [in]  val          Field value.
 * \param [out] sym          Field symbol.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
field_value_to_symbol(uint32_t flags,
                      const bcmltd_enum_type_t *edata,
                      uint32_t val,
                      const char **sym)
{
    uint32_t i;
    int rv = SHR_E_UNAVAIL;

    do {

        if (!(flags & BCMLTD_FIELD_F_ENUM)) {
            /* This field is not an enum. */
            break;
        }

        if (!edata) {
            /* Not common enum. */
            rv = SHR_E_UNAVAIL;
            break;
        }

        for (i=0; i < edata->num_sym; i++) {
            if (((uint32_t)(edata->sym[i].val)) == val) {
                *sym = edata->sym[i].name;
                break;
            }
        }
        rv = (i < edata->num_sym) ? SHR_E_NONE : SHR_E_UNAVAIL;

    } while (0);

    return rv;
}

/*******************************************************************************
 * Public functions
 */
int
bcmltd_field_value_to_symbol(bcmltd_sid_t sid,
                             bcmltd_fid_t fid,
                             uint32_t val,
                             const char **sym)
{
    int rv = SHR_E_UNAVAIL;
    const bcmltd_table_rep_t *tbl;

    do {

        if (sym == NULL) {
            break;
        }

        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            break;
        }

        /* FID out of range */
        if (fid >= tbl->fields) {
            break;
        }

        rv = field_value_to_symbol(tbl->field[fid].flags,
                                  tbl->field[fid].edata,
                                  val,
                                  sym);

    } while (0);

    return rv;
}
