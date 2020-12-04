/*! \file bcmdrd_sym_ser.c
 *
 * BCMDRD routines related to SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_assert.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_ser.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

const void *
bcmdrd_sym_ser_data_get(const bcmdrd_symbols_t *symbols)
{
    if (symbols == NULL) {
        return NULL;
    }
    return symbols->ser_data;
}

int
bcmdrd_sym_ser_profile_offset_get(const bcmdrd_symbol_t *symbol)
{
    if (symbol == NULL) {
        return -1;
    }
    return BCMDRD_SYM_EP_OFFSET_GET(symbol->pfinfo);
}

static const uint32_t *
ser_profile_get(const bcmdrd_ser_data_t *ser_data, int sympf_offset)
{
    const uint32_t *prfs;

    /* Offset 0 is reserved for symbols without SER defined. */
    if (ser_data == NULL || sympf_offset <= 0) {
        return NULL;
    }

    /* Get the ser array of the whole symbols. */
    prfs = ser_data->sym_profiles;;
    if (prfs == NULL) {
        return NULL;
    }

    return prfs + sympf_offset;
}

const uint32_t *
bcmdrd_sym_ser_info_profile_get(const bcmdrd_ser_data_t *ser_data,
                                int sympf_offset)
{
    const uint32_t *info_pfs;
    const uint32_t *sym_pf;
    int offset;

    sym_pf = ser_profile_get(ser_data, sympf_offset);
    if (sym_pf == NULL) {
        return NULL;
    }
    /* Get the offset in SER information profiles. */
    offset = BCMDRD_SER_PF_INFO_GET(*sym_pf);

    /* Get SER information profiles */
    info_pfs = ser_data->info_profiles;
    if (info_pfs == NULL) {
        return NULL;
    }

    return info_pfs + offset;
}

const uint32_t *
bcmdrd_sym_ser_en_ctrl_profile_get(const bcmdrd_ser_data_t *ser_data,
                                   int sympf_offset,
                                   bcmdrd_ser_en_type_t type)
{
    const uint32_t *en_pfs;
    const uint32_t *sym_pf;
    int offset = -1;
    uint32_t enctrl;

    sym_pf = ser_profile_get(ser_data, sympf_offset);
    if (sym_pf == NULL) {
        return NULL;
    }

    if ((bcmdrd_ser_en_ctrl_type_t)type >= BCMDRD_SER_EN_CTRL_TYPE_MAX) {
        return NULL;
    }

    /* Get enable control profiles for the given type. */
    en_pfs = ser_data->en_profiles[type];
    if (en_pfs == NULL) {
        return NULL;
    }

    /*
     * Check whether the specified SER enable type profile is
     * encoded for this symbol.
     */
    do {
        if (BCMDRD_SER_PF_ENCTRL0(*sym_pf)) {
            enctrl = BCMDRD_SER_PF_ENCTRL0_GET(*sym_pf);
            if (BCMDRD_SER_PF_ENCTRL_TYPE_GET(enctrl) == type) {
                offset = BCMDRD_SER_PF_ENCTRL_IDX_GET(enctrl);
                break;
            }
        }
        if (BCMDRD_SER_PF_ENCTRL1(*sym_pf)) {
            enctrl = BCMDRD_SER_PF_ENCTRL1_GET(*sym_pf);
            if (BCMDRD_SER_PF_ENCTRL_TYPE_GET(enctrl) == type) {
                offset = BCMDRD_SER_PF_ENCTRL_IDX_GET(enctrl);
                break;
            }
        }
    } while (!BCMDRD_SER_PF_LAST(*sym_pf++));

    return (offset < 0) ? NULL : en_pfs + offset;
}

const bcmdrd_ser_rmr_t *
bcmdrd_sym_ser_rmr_get(const bcmdrd_ser_data_t *ser_data,
                       bcmdrd_ser_rmr_id_t rid)
{
    if (ser_data == NULL) {
        return NULL;
    }

    if (rid >= ser_data->rmrs_size) {
        return NULL;
    }

    return ser_data->rmrs + rid;
}

const bcmdrd_ser_rmr_t *
bcmdrd_ser_rmr_find(const char *name, const bcmdrd_ser_data_t *ser_data,
                    bcmdrd_ser_rmr_id_t *rid)
{
    const bcmdrd_ser_rmr_t *rmr;
    uint32_t idx;

    if (ser_data == NULL || ser_data->rmrs == NULL || name == NULL) {
        return NULL;
    }
    rmr = ser_data->rmrs;

    for (idx = 0; idx < ser_data->rmrs_size; idx++) {
        if (sal_strcmp(rmr->name, name) == 0) {
            break;
        }
        rmr++;
    }

    if (idx >= ser_data->rmrs_size) {
        rmr = NULL;
    } else if (rid) {
        *rid = idx;
    }
    return rmr;
}

int
bcmdrd_ser_rmr_profile_offset_get(const bcmdrd_ser_rmr_t *rmr)
{
    if (rmr == NULL) {
        return -1;
    }
    return BCMDRD_SER_RMR_EP_OFFSET_GET(rmr->pfinfo);
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
