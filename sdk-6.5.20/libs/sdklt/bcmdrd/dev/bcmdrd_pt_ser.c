/*! \file bcmdrd_pt_ser.c
 *
 * BCMDRD PT SER routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt_ser.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

static void
ser_en_ctrl_get(const uint32_t *en_pf, bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    sal_memset(en_ctrl, 0, sizeof(*en_ctrl));

    en_ctrl->sid = BCMDRD_SER_ENCTRL_REG_GET(*en_pf);
    en_ctrl->fid = BCMDRD_SER_ENCTRL_FIELD_GET(*en_pf);
    if (BCMDRD_SER_ENCTRL_EXT(*en_pf)) {
        en_ctrl->fmask = 1 << BCMDRD_SER_ENCTRL_EXT_BITPOS_GET(*en_pf);
    }
}

static void
ser_info_get(const uint32_t *info_pf, bcmdrd_pt_ser_info_t *info)
{
    sal_memset(info, 0, sizeof(*info));

    info->type = BCMDRD_SER_INFO_TYPE_GET(*info_pf);
    info->num_xor_banks = BCMDRD_SER_INFO_NUM_XOR_BANKS_GET(*info_pf);
    info->drop_pkt = BCMDRD_SER_INFO_DROP_PKT_GET(*info_pf);
    info->resp = BCMDRD_SER_INFO_RESP_GET(*info_pf);
}

int
bcmdrd_pt_ser_en_ctrl_get(int unit,
                          bcmdrd_ser_en_type_t type,
                          bcmdrd_sid_t sid,
                          bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_ser_data_t *ser_data;
    int sympf_offset;
    const uint32_t *en_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }

    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    if (en_ctrl == NULL) {
        return SHR_E_PARAM;
    }

    ser_data = bcmdrd_sym_ser_data_get(symbols);
    sympf_offset = bcmdrd_sym_ser_profile_offset_get(symbol);

    en_pf = bcmdrd_sym_ser_en_ctrl_profile_get(ser_data, sympf_offset, type);
    if (en_pf == NULL) {
        return SHR_E_UNAVAIL;
    }

    ser_en_ctrl_get(en_pf, en_ctrl);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_drop_pkt_ctrl_get(int unit, bcmdrd_sid_t sid,
                                bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    return bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_CTRL_TYPE_DROP_PKT,
                                     sid, en_ctrl);
}

int
bcmdrd_pt_ser_info_get(int unit, bcmdrd_sid_t sid,
                       bcmdrd_pt_ser_info_t *info)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_ser_data_t *ser_data;
    int sympf_offset;
    const uint32_t *info_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }

    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    if (info == NULL) {
        return SHR_E_PARAM;
    }

    ser_data = bcmdrd_sym_ser_data_get(symbols);
    sympf_offset = bcmdrd_sym_ser_profile_offset_get(symbol);

    info_pf = bcmdrd_sym_ser_info_profile_get(ser_data, sympf_offset);
    if (info_pf == NULL) {
        return SHR_E_UNAVAIL;
    }

    ser_info_get(info_pf, info);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_rmr_id_list_get(int unit, size_t list_max,
                              bcmdrd_ser_rmr_id_t *rid_list, size_t *num_rid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    uint32_t idx;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_UNAVAIL;
    }
    if (num_rid == NULL) {
        return SHR_E_PARAM;
    }
    *num_rid = ser_data->rmrs_size;

    if (list_max == 0) {
        return SHR_E_NONE;
    }
    if (rid_list == NULL) {
        return SHR_E_PARAM;
    }

    for (idx = 0; idx < ser_data->rmrs_size; idx++) {
        if (idx >= list_max) {
            break;
        }
        rid_list[idx] = idx;
    }

    return SHR_E_NONE;
}

extern int
bcmdrd_pt_ser_rmr_en_ctrl_get(int unit,
                              bcmdrd_ser_en_type_t type,
                              bcmdrd_ser_rmr_id_t rid,
                              bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    const bcmdrd_ser_rmr_t *rmr;
    int sympf_offset;
    const uint32_t *en_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_UNAVAIL;
    }
    rmr = bcmdrd_sym_ser_rmr_get(ser_data, rid);
    if (rmr == NULL) {
        return SHR_E_PARAM;
    }

    if (en_ctrl == NULL) {
        return SHR_E_PARAM;
    }

    sympf_offset = bcmdrd_ser_rmr_profile_offset_get(rmr);

    en_pf = bcmdrd_sym_ser_en_ctrl_profile_get(ser_data, sympf_offset, type);
    if (en_pf == NULL) {
        return SHR_E_UNAVAIL;
    }

    ser_en_ctrl_get(en_pf, en_ctrl);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_rmr_info_get(int unit, bcmdrd_ser_rmr_id_t rid,
                           bcmdrd_pt_ser_info_t *info)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    const bcmdrd_ser_rmr_t *rmr;
    int sympf_offset;
    const uint32_t *info_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_UNAVAIL;
    }
    rmr = bcmdrd_sym_ser_rmr_get(ser_data, rid);
    if (rmr == NULL) {
        return SHR_E_PARAM;
    }

    if (info == NULL) {
        return SHR_E_PARAM;
    }

    sympf_offset = bcmdrd_ser_rmr_profile_offset_get(rmr);

    info_pf = bcmdrd_sym_ser_info_profile_get(ser_data, sympf_offset);
    if (info_pf == NULL) {
        return SHR_E_UNAVAIL;
    }

    ser_info_get(info_pf, info);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_name_to_rmr(int unit, const char *name,
                          bcmdrd_ser_rmr_id_t *rid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    const bcmdrd_ser_rmr_t *rmr;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_UNAVAIL;
    }

    rmr = bcmdrd_ser_rmr_find(name, ser_data, rid);
    if (rmr == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

const char *
bcmdrd_pt_ser_rmr_to_name(int unit, bcmdrd_ser_rmr_id_t rid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return NULL;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return NULL;
    }
    if (rid >= ser_data->rmrs_size) {
        return NULL;
    }
    return ser_data->rmrs[rid].name;
}

#endif
