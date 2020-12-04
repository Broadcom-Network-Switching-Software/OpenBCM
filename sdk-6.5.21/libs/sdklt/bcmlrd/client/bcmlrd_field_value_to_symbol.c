/*! \file bcmlrd_field_value_to_symbol.c
 *
 * Get the field symbol for a given value.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_map.h>
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
    int rv = SHR_E_PARAM;

    do {

        if (!(flags & BCMLRD_FIELD_F_ENUM)) {
            rv = SHR_E_UNAVAIL;
            /* This field is not an enum. */
            break;
        }

        if (!edata) {
            rv = SHR_E_INTERNAL;
            /* Says it's an enum but no symbol data. */
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


int
bcmlrd_field_value_to_symbol(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             uint32_t val,
                             const char **sym)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_field_data_t *field;

    do {

        if (sym == NULL) {
            break;
        }

        rv = bcmlrd_field_get(unit,sid,fid,&field);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = field_value_to_symbol(field->flags,
                                   field->edata,
                                   val,
                                   sym);

        if (rv == SHR_E_UNAVAIL) {
            rv = bcmltd_field_value_to_symbol(sid, fid,
                                              (uint32_t)val, sym);
        }
    } while (0);

    return rv;
}
