/*! \file bcmlrd_enum_find_symbol_type.c
 *
 * Return information for LTL enum symbol by type name.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmltd/chip/bcmltd_common_enumpool.h>

/*******************************************************************************
 * Local definitions
 */

/* Declare enum externs. */
#define VARSYM(bd,vu,va) bd##vu##va##_lrd_enum_by_type
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
    extern const bcmltd_enum_by_type_t VARSYM(_bd,_vu,_va);
#include <bcmlrd/chip/bcmlrd_variant.h>

/* Define enum list. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
    &VARSYM(_bd,_vu,_va),
static const bcmltd_enum_by_type_t *bcmlrd_enum_by_type[] = {
    0,    /* dummy entry for type 'none' */
#include <bcmlrd/chip/bcmlrd_variant.h>
    0     /* end-of-list */
};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief bsearch function for bcmltd_enum_type_t names.
 *
 * sal_bsearch() callback to match a enum type name.
 *
 * \param [in]  key     Enum scalar value.
 * \param [in]  elem    Enum type element to search.
 *
 * \retval !0           Name not found.
 * \retval 0            Name found.
 */

static int
bcmlrd_search_symbol_type(const void *key, const void *elem)
{
    const char *name = (const char *)key;
    const bcmltd_enum_type_t *enum_type =
        (const bcmltd_enum_type_t *)elem;

    return sal_strcmp(name, enum_type->name);
}

/*!
 * \brief Find symbol type name in enum database.
 *
 * Find a symbol type name in the given enum symbol database.
 *
 * \param [in]  enum_by_type    LTL enum symbol database.
 * \param [in]  name            Enum type name.
 *
 * \retval !NULL        Enum symbol data.
 * \retval NULL         Enum type name not found.
 */

static const bcmltd_enum_type_t *
bcmlrd_find_symbol_type(const bcmltd_enum_by_type_t *enum_by_type,
                        const char *name)
{
    return (const bcmltd_enum_type_t *)
        sal_bsearch(name,
                    enum_by_type->enum_type,
                    enum_by_type->num_enum_type,
                    sizeof(enum_by_type->enum_type[0]),
                    bcmlrd_search_symbol_type);
}


/*******************************************************************************
 * Public functions
 */

int
bcmlrd_enum_find_symbol_type(int unit,
                             const char *name,
                             const bcmltd_enum_type_t **enum_type_p)
{
    int rv = SHR_E_UNAVAIL;
    const bcmltd_enum_type_t *enum_type;
    const bcmltd_enum_by_type_t *enum_by_type;
    bcmlrd_variant_t variant;


    do {
        if (name == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        if (enum_type_p == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        enum_type = bcmlrd_find_symbol_type(&bcmltd_common_enum_by_type,
                                            name);
        if (!enum_type) {
            /* No common type with 'name'; try mapped types. */
            variant = bcmlrd_variant_get(unit);
            enum_by_type = bcmlrd_enum_by_type[variant];
            if (enum_by_type == NULL) {
                /* No mapped types by that name either. */
                break;
            }
            enum_type = bcmlrd_find_symbol_type(enum_by_type, name);
        }

        if (!enum_type) {
            variant = bcmlrd_base_get(unit);
            enum_by_type = bcmlrd_enum_by_type[variant];
            if (enum_by_type == NULL) {
                /* No mapped types by that name either. */
                break;
            }
            enum_type = bcmlrd_find_symbol_type(enum_by_type, name);
        }

        if (enum_type) {
            *enum_type_p = enum_type;
            rv = SHR_E_NONE;
        }

    } while (0);

    return rv;
}
