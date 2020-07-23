/*! \file bcmlrd_variant_conf.c
 *
 * unit/variant mapping
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_internal.h>

/*******************************************************************************
 * Private data
 */

/* Per-unit variant type. */
static bcmlrd_variant_t bcmlrd_variant_conf[BCMDRD_CONFIG_MAX_UNITS];

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
     _vv,

/* Conversion data for LTL DEVICE_CONFIG_T global enum and per-build
 * bcmlrd_variant_t. This is indexed by bcmlrd_variant_t and the value
 * at that index is the corresponding DEVICE_CONFIG_T symbol value.
 */
static uint64_t bcmlrd_device_variant_t_to_variant_t[] = {
    -1,
#include <bcmlrd/chip/bcmlrd_variant.h>
    -1,
};

/*
 * bcmlrd_variant_t to string name
 */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
     #_ve,

static const char *bcmlrd_variant_string_name[] = {
    "",
#include <bcmlrd/chip/bcmlrd_variant.h>
    "",
};



/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Update BCMLRD variant sensitive data.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  variant         Variant ID.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmlrd_variant_update(int unit, bcmlrd_variant_t variant)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_dev_conf_t *lrd_dev_conf = NULL;
    const bcmlrd_dev_pcm_conf_t *pcm_conf = NULL;
    const bcmlrd_pcm_conf_compact_rep_t *pcm = NULL;
    const bcmlrd_dev_comp_conf_t *comp_conf = NULL;
    bcmlrd_match_id_db_get_t *match_id_ptr_list = NULL;
    const bcmlrd_match_id_db_info_t **match_id_data_info = NULL;

    do {

        lrd_dev_conf = bcmlrd_dev_conf_get();
        if (lrd_dev_conf == NULL) {
            rv = SHR_E_INTERNAL;
            break;
        }

        /* Load PCM configuration. */
        pcm_conf = bcmlrd_dev_pcm_conf_get();
        if (pcm_conf == NULL) {
            rv = SHR_E_INTERNAL;
            break;
        }

        comp_conf = bcmlrd_dev_comp_conf_get();
        if (comp_conf == NULL) {
            rv = SHR_E_INTERNAL;
            break;
        }

        match_id_ptr_list = bcmlrd_dev_match_id_db_get();
        if (match_id_ptr_list == NULL) {
            rv = SHR_E_INTERNAL;
            break;
        }

        match_id_data_info = bcmlrd_dev_match_id_data_all_info_get();
        if (match_id_data_info == NULL) {
            rv = SHR_E_INTERNAL;
            break;
        }

        if (lrd_dev_conf[variant].conf == NULL) {
            /* No configuration for this variant. */
            rv = SHR_E_UNAVAIL;
            break;
        }

        rv = bcmlrd_conf_set(unit, lrd_dev_conf[variant].conf);
        if (SHR_FAILURE(rv)) {
            /* Cannot set variant configuration. */
            break;
        }

        /* Load PCM data if available. */
        pcm = pcm_conf[variant].pcm_conf_get();
        if (pcm != NULL) {
            bcmlrd_unit_pcm_conf_compact[unit] = pcm;
            rv = bcmlrd_unit_pcm_conf_expand(unit);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }

        /* Load component configuration data. */
        bcmlrd_unit_comp_conf[unit] = comp_conf[variant].conf;

        /* Get the function to retrieve match id data. */
        bcmlrd_unit_match_id_db_get_ptr[unit] =
            match_id_ptr_list[variant];

        /* Get the function to retrieve match id data via spec. */
        bcmlrd_unit_match_id_data_info_all[unit] =
            match_id_data_info[variant];

    } while (0);

    return rv;
}

/*******************************************************************************
 * Public functions
 */

bcmlrd_variant_t
bcmlrd_variant_get(int unit)
{
    bcmlrd_variant_t variant = BCMLRD_VARIANT_T_NONE;

    if (BCMDRD_UNIT_VALID(unit)) {
        variant = bcmlrd_variant_conf[unit];
    }

    return variant;
}

bcmlrd_variant_t
bcmlrd_variant_from_device_variant_t(uint64_t device_variant)
{
    bcmlrd_variant_t variant = BCMLRD_VARIANT_T_NONE;
    int i;

    /* Zeroth entry is known to be "NONE" */
    for (i=1; i<COUNTOF(bcmlrd_device_variant_t_to_variant_t); i++) {
        if (bcmlrd_device_variant_t_to_variant_t[i] == device_variant) {
            variant = (bcmlrd_variant_t)i;
            break;
        }
    }

    return variant;
}

uint64_t
bcmlrd_device_variant_t_from_variant(bcmlrd_variant_t variant)
{
    uint64_t value = 0;

    if ((size_t) variant < COUNTOF(bcmlrd_device_variant_t_to_variant_t)) {
        value = bcmlrd_device_variant_t_to_variant_t[variant];
    }

    return value;
}

int
bcmlrd_variant_from_variant_string(const char *variant_string,
                                   bcmlrd_variant_t *variant)
{
    int i;
    int rv = SHR_E_NOT_FOUND;
    size_t buf_len = sal_strlen(variant_string);
    size_t var_len;

    for (i = 0; i < COUNTOF(bcmlrd_variant_string_name); i++) {
        var_len = sal_strlen(bcmlrd_variant_string_name[i]);
        if (var_len != buf_len) {
            continue;
        }

        if (!sal_strncmp(bcmlrd_variant_string_name[i],
                         variant_string,
                         var_len)) {
            *variant = (bcmlrd_variant_t)i;
            rv = SHR_E_NONE;
            break;
        }
    }

    return rv;
}

int
bcmlrd_variant_set(int unit, bcmlrd_variant_t variant)
{
    int rv = SHR_E_PARAM;


    if (variant < BCMLRD_VARIANT_T_COUNT && BCMDRD_UNIT_VALID(unit)) {
        bcmlrd_variant_conf[unit] = variant;
        rv = bcmlrd_variant_update(unit, variant);
    }

    return rv;
}

const char *
bcmlrd_variant_string(bcmlrd_variant_t variant)
{
    const char *str = "";

    if (variant < BCMLRD_VARIANT_T_COUNT) {
        str = bcmlrd_variant_string_name[(unsigned)variant];
    }

    return str;
}

const char *
bcmlrd_variant_name(int unit)
{
    return bcmlrd_variant_string(bcmlrd_variant_get(unit));
}
