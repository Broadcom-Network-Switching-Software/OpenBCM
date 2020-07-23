/*! \file ser_inject_and_test.c
 *
 * Functions for SER inject and test
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_symbols.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_libc.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "ser_inject_and_test.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_pt_control.h"
#include "ser_lt_ser_stats.h"
#include "ser_lt_ser_log.h"
#include "ser_lt_ser_pt_status.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Max numer of fields which are injected SER error */
#define  MAX_NUM_SER_INJECT_FIELD     10

/* Max field number of PTs */
#define  MAX_PT_FIELD_NUM             200

/******************************************************************************
 * Private Functions
 */
/* Get a field to be injected SER error. */
static int
bcmptm_ser_sram_test_fid_get(int unit, bcmdrd_sid_t sid,
                             bcmdrd_fid_t *fid, int double_bit)
{
    char *field_name[] = {
                           "ECC",
                           "ECC_0",
                           "EVEN_PARITY",
                           "PARITY",
                           "EVEN_PARITY_0",
                           "EVEN_PARITY_LOWER",
                           "PARITY_0",
                           "ECCP",
                           "ECC0",
                           "DATA",
                           "DATA_0"
                          };
    size_t  i = 0, count = sizeof(field_name) / sizeof(field_name[0]);
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    const bcmdrd_symbols_t *symbols;
    int rv = SHR_E_NONE;
    size_t num_fields = 0;
    bcmdrd_fid_t field_list[MAX_PT_FIELD_NUM];

    SHR_FUNC_ENTER(unit);

    symbols = bcmdrd_dev_symbols_get(unit, 0);

    for (i = 0; i < count; i++) {
        symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid, field_name[i], &finfo);
        if (NULL != symbol) {
            /* needs multi-bit field */
            if (double_bit && ((finfo.maxbit - finfo.minbit) < 1)) {
                continue;
            }
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "The SER error will be injected to field [%s] of reg [%s]\n"),
                       finfo.name, bcmdrd_pt_sid_to_name(unit, sid)));
            *fid = finfo.fid;
            rv = SHR_E_NONE;
            SHR_ERR_EXIT(rv);
        }
    }
    rv = bcmdrd_pt_fid_list_get(unit, sid, MAX_PT_FIELD_NUM, field_list, &num_fields);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* select first multi-bit field or last 1-bit field */
    for (i = 0; i < num_fields; i++) {
        rv = bcmdrd_pt_field_info_get(unit, sid, field_list[i], &finfo);
        SHR_IF_ERR_EXIT(rv);
        if (finfo.maxbit - finfo.minbit >= 1) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "The SER error will be injected to field [%s] of reg [%s]\n"),
                       finfo.name, bcmdrd_pt_sid_to_name(unit, sid)));
            *fid = field_list[i];
            rv = SHR_E_NONE;
            SHR_ERR_EXIT(rv);
        }
    }
    if (double_bit) {
        /* not found two bit field */
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "%s has no field which width is two or more bit\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        rv = SHR_E_NOT_FOUND;
        SHR_ERR_EXIT(rv);
    }
    if (i == num_fields) {
        i -= 1;
    }
    *fid = field_list[i];

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to find field of %s to be injected SER error\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/* Get a field of a TCAM to be injected SER error. */
static int
bcmptm_ser_tcam_test_fid_get(int unit, bcmdrd_sid_t sid, int phy_idx, int tbl_inst,
                             uint32_t flags, bcmdrd_fid_t *fid, int *test_fld_num)
{
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    char *key_name[] = { "LWR_TCAM_ECCP_KEY",
                         "TCAM_ECCP_KEY",
                         "KEY",
                         "KEY0",
                         "KEY1",
                         "KEY0_LWR"
                       };
    char *mask_name[] = { "LWR_TCAM_ECCP_MASK",
                          "TCAM_ECCP_MASK",
                          "MASK",
                          "MASK0",
                          "MASK1",
                          "MASK0_LWR"
                        };
    char *valid_name[] = { "VALID",
                           "VALID0",
                           "VALID0_LWR",
                           "BASE_VALID",
                           "BASE_VALID_1",
                           "BASE_VALID_2",
                           "LWR_VALID0"
                         };
    int i = 0, key_count = COUNTOF(key_name), rv = SHR_E_NONE;
    int valid_count = COUNTOF(valid_name);
    uint32_t mem_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t mask_fbuf[BCMDRD_MAX_PT_WSIZE];
    const bcmdrd_symbols_t *symbols = NULL;

    SHR_FUNC_ENTER(unit);

    *test_fld_num = 0;
    symbols = bcmdrd_dev_symbols_get(unit, 0);

    if ((BCMPTM_SER_INJECT_ERROR_KEY_MASK & flags) == 0) {
        for (i = 0; i < valid_count; i++) {
            symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid,
                                                       valid_name[i], &finfo);
            if (NULL != symbol) {
                fid[0] = finfo.fid;
                *test_fld_num = 1;
                rv = SHR_E_NONE;
                SHR_ERR_EXIT(rv);
            } else {
                continue;
            }
        }
    } else  {
        /* for BCMPTM_SER_INJECT_ERROR_KEY_MASK */
        for (i = 0; i < key_count; i++) {
            symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid,
                                                       key_name[i], &finfo);
            if (NULL == symbol) {
                continue;
            }
            fid[0] = finfo.fid;
            symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid,
                                                       mask_name[i], &finfo);
            if (NULL == symbol) {
                /* it is abnormal when tcams have key field, but have not mask field. */
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            sal_memset(mem_data, 0, sizeof(mem_data));
            sal_memset(mask_fbuf, 0, sizeof(mask_fbuf));

            rv = bcmptm_ser_pt_read(unit, sid, phy_idx, tbl_inst, -1,
                                    mem_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            rv = bcmdrd_pt_field_get(unit, sid, mem_data, finfo.fid, mask_fbuf);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            /* if first bit of mask field is 1, the bit needs not to flip */
            if ((mask_fbuf[0] & 0x1) == 1) {
                *test_fld_num = 1;
            } else {
                fid[1] = finfo.fid;
                *test_fld_num = 2;
            }
            SHR_EXIT();
        }
    }
    /* not found */
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/* Generate SER error. */
static void
bcmptm_ser_error_data_generate(uint32_t *fbuf, int double_bit,
                               uint32_t bit_interleave)
{
    if((fbuf[0] & 0x1) == 1) {
        fbuf[0] &= 0xFFFFFFFE;
    } else {
        fbuf[0] |= 0x1;
    }
    if (0 == double_bit) {
        return;
    }

    if (bit_interleave == 0) {
        bit_interleave = 2;
    } else {
        bit_interleave = (bit_interleave << 2);
    }

    if ((fbuf[0] & bit_interleave) == bit_interleave) {
        fbuf[0] &= ~(bit_interleave);
    } else {
        fbuf[0] |= bit_interleave;
    }
}

/* Print hint message of SER injection operation. */
static int
bcmptm_ser_injection_hint_msg_print(int unit, bcmdrd_sid_t sid,
                                    int double_bit, int ecc_checking)
{
    if (double_bit) {
       LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Inject 2bit ser error to SRAM[%s]\n"),
                 bcmdrd_pt_sid_to_name(unit, sid)));
       if (ecc_checking == 0) {
           LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SRAM[%s] only supports 1bit parity protection\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
           return SHR_E_CONFIG;
       }
   } else {
       LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Inject 1bit ser error to SRAM[%s]\n"),
                 bcmdrd_pt_sid_to_name(unit, sid)));
       if (ecc_checking == 1) {
           LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SRAM[%s] supports 2bit ECC protection\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
       }
   }
   return SHR_E_NONE;
}

/* Print hint message of SER validation operation. */
static int
bcmptm_ser_validation_hint_msg_print(int unit, bcmdrd_sid_t sid, int phy_idx,
                                     int double_bit, int ecc_checking, int rv)
{
    uint32_t ser_checking_enable = 0;
    /*
    * sometime SERC finds SER error of one PT reported again and again,
    * then SERC close parity checking.
    */
    ser_checking_enable = bcmptm_ser_pt_control_field_val_get
        (unit, sid, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    if (ser_checking_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Parity checking of [%s] is disabled by SERC thread!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "So this PT should be read successfully!\n")));
            return SHR_E_FAIL;
        } else {
            return SHR_E_NONE;
        }
    }

    /* (0 == 0): parity protect or (1 == 1):ecc protect */
    if (double_bit == ecc_checking) {
        if (SHR_SUCCESS(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "[%s][%d] should not be read successfully\n"),
                      bcmdrd_pt_sid_to_name(unit, sid), phy_idx));
            return SHR_E_FAIL;
        }
    } else {
        if (double_bit == 1) {
            /* two bit error in memory which owns parity checking */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "There is a 2bit error in [%s][%d].\n"),
                      bcmdrd_pt_sid_to_name(unit, sid), phy_idx));
        } else {
            /* one bit error in memory which owns ECC checking */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "There is a 1bit error in [%s][%d].\n"),
                      bcmdrd_pt_sid_to_name(unit, sid), phy_idx));

            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "For [%s], single bit error entry cannot"
                                     " be read successfully\n"),
                          bcmdrd_pt_sid_to_name(unit, sid)));
                rv = SHR_E_NONE;
            }
        }
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }
    return SHR_E_NONE;
}

/* Validate whether a SER error is corrected or not. */
static int
bcmptm_ser_correction_validate(int unit, bcmdrd_sid_t sid,
                               int double_bit, int ecc_checking, int pt_read_rv)
{
    int rv = SHR_E_NONE;
    uint32_t ser_checking_enable;

    ser_checking_enable = bcmptm_ser_pt_control_field_val_get
        (unit, sid, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    /*
     * Should be failure if there is 1bit error in parity protection PT,
     * or there is 2bit error in ECC protection PT.
     */
    if (SHR_FAILURE(pt_read_rv)) {
        if (ser_checking_enable) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Because SER recovery type of [%s] is"
                                 "[ser_no_operation]."
                                 "So SER event should be reported again\n"),
                      bcmdrd_pt_sid_to_name(unit, sid)));
            rv = SHR_E_NONE;
        } else {
            rv = SHR_E_FAIL;
        }
    } else {
        if (ser_checking_enable) {
            if (double_bit) {
                /* PT with double bit error can not be read successfully */
                rv = SHR_E_FAIL;
            } else if (ecc_checking) {
                /* ECC protection PT with single bit error can be read successfully */
                rv = SHR_E_NONE;
            } else {
                /* Parity protection PT with single bit error can not be read successfully */
                rv = SHR_E_FAIL;
            }
        } else {
            rv = SHR_E_NONE;
        }
    }
    return rv;
}

/* Inject a SER error into a PT. */
static int
bcmptm_ser_error_inject(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                        int tbl_inst, int tbl_copy, int phy_idx, int double_bit)
{
    uint32_t mem_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t bad_data;
    int  rv = SHR_E_NONE;
    uint32_t fbuf[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_sym_field_info_t finfo;
    uint32_t bit_interleave = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(mem_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
    sal_memset(fbuf, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);

    /* Cannot read data from PTcache for some PTs, such as: EGR_VLAN_XLATE_ECCm */
    rv = bcmptm_ser_pt_read(unit, sid, phy_idx, tbl_inst, tbl_copy,
                            mem_data, BCMDRD_MAX_PT_WSIZE,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, sid, mem_data, fid, fbuf);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_test_num_interleave_get(unit, sid, fid, &bit_interleave);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    /* bit flip */
    bcmptm_ser_error_data_generate(fbuf, double_bit, bit_interleave);

    bad_data = fbuf[0];

    rv = bcmdrd_pt_field_set(unit, sid, mem_data, fid, fbuf);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, sid, phy_idx, tbl_inst,
                             tbl_copy, mem_data, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_read(unit, sid, phy_idx, tbl_inst, tbl_copy,
                            mem_data, BCMDRD_MAX_PT_WSIZE,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, sid, mem_data, fid, fbuf);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (bad_data != fbuf[0]) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Read data [0x%x] is not equal to written err-data[0x%x] "
                             "of memory[%s] index [%d]\n"),
                  fbuf[0], bad_data, bcmdrd_pt_sid_to_name(unit, sid), phy_idx));

        rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
        SHR_IF_ERR_EXIT(rv);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to inject SER error to field [%s]\n"),
                  finfo.name));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to inject SER error to memory %s\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/* Disable or enable control register. */
static int
bcmptm_ser_ctrl_reg_disable(int unit, bcmdrd_sid_t sid,
                            bcmdrd_fid_t fid, int val)
{
    int rv = SHR_E_NONE;
    bcmdrd_fid_t fld_array[2] = { INVALIDf, INVALIDf };
    bool val_b = val ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    fld_array[0] = fid;
    fld_array[1] = INVALIDf; /* must */

    rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, 0, fld_array, val_b,
                                            NULL, NULL);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * For PTs which access type is DATA-SPLIT, a kind of field only exists on one instance.
 * SER logic tries to inject SER error to such fields.
 */
static int
bcmptm_ser_data_split_field_get(int unit, bcmdrd_sid_t pt_id, int tbl_inst,
                                bool double_bit, bcmdrd_fid_t *data_split_field)
{
    int rv = SHR_E_NONE;
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    const bcmdrd_symbols_t *symbols;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_pt_acctype_get(unit, pt_id, &acc_type, &acctype_str);
    if (SHR_FAILURE(rv) || !PT_IS_DATA_SPLIT_ACC_TYPE(acctype_str)) {
        SHR_EXIT();
    }
    rv = bcmptm_ser_data_split_mem_test_fid_get(unit, pt_id, tbl_inst,
                                                 data_split_field);
    if (rv == SHR_E_PARAM) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Instance [%d] does not exist on data-split table[%s].\n"),
                  tbl_inst, bcmdrd_pt_sid_to_name(unit, pt_id)));
        SHR_IF_ERR_EXIT(rv);
    } else {
        if (*data_split_field != INVALIDf) {
            symbols = bcmdrd_dev_symbols_get(unit, 0);
            symbol = bcmdrd_sym_field_info_get(symbols, pt_id, *data_split_field, &finfo);
            if (NULL != symbol) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "The SER error will be injected to field [%s] which only in instance [%d]\n"),
                          finfo.name, tbl_inst));
            } else {
                *data_split_field = INVALIDf;
            }
            /* such fields just have 1bit, so inject other fields */
            if (finfo.maxbit == finfo.minbit && double_bit) {
                *data_split_field = INVALIDf;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Get ECC view SID if has, and get FID to be injected SER error. */
static int
bcmptm_ser_injected_sid_and_field_get(int unit, bcmdrd_sid_t sid,
                                      int tbl_inst, int index, int double_bit,
                                      bcmdrd_sid_t *inject_fid)
{
    int ecc_checking = 0, rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* To get SER check type of PT. */
    rv = bcmptm_ser_mem_ecc_info_get(unit, sid, index, NULL, NULL, &ecc_checking);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_injection_hint_msg_print(unit, sid, double_bit, ecc_checking);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    *inject_fid = INVALIDf;
    /* Get field only exists on tbl_inst. */
    rv = bcmptm_ser_data_split_field_get(unit, sid, tbl_inst, double_bit, inject_fid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (*inject_fid == INVALIDf) {
        /* get field used to inject ser error */
        rv = bcmptm_ser_sram_test_fid_get(unit, sid, inject_fid, double_bit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public Functions
 */
int
bcmptm_ser_tcam_ser_inject(int unit, bcmdrd_sid_t pt_id_mapped,
                           int tbl_inst, int phy_idx, uint32_t flags)
{
    int rv = SHR_E_NONE, i = 0, rv_enable = SHR_E_EMPTY;
    int test_fld_num = 0;
    int ecc_checking = 0;
    int double_bit = 0;
    int index = 0;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fids[MAX_NUM_SER_INJECT_FIELD] = { INVALIDf };
    uint32_t ser_enable = 0;

    SHR_FUNC_ENTER(unit);

    ser_enable = bcmptm_ser_pt_control_field_val_get
        (unit, pt_id_mapped, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    if (ser_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Please enable parity or ecc checking for [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id_mapped)));
        rv = SHR_E_DISABLED;
        SHR_IF_ERR_EXIT(rv);
    }
    /* Disable parity checking */
    rv_enable = bcmptm_ser_tcam_pt_ser_enable(unit, pt_id_mapped, 0);
    rv = rv_enable;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* find ser injection fid */
    rv = bcmptm_ser_tcam_test_fid_get(unit, pt_id_mapped, phy_idx,
                                      tbl_inst, flags, fids, &test_fld_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_mem_ecc_info_get(unit, pt_id_mapped, phy_idx, &sid, &index, &ecc_checking);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (ecc_checking && (flags & BCMPTM_SER_INJECT_ERROR_2BIT)) {
        double_bit = 1;
    }

    /* only support 1bit ser error injection */
    /* first modify mask field, then modify key field */
    for (i = test_fld_num - 1; i >= 0 && i < MAX_NUM_SER_INJECT_FIELD; i--) {
        rv = bcmptm_ser_error_inject(unit, pt_id_mapped, fids[i],
                                     tbl_inst, -1, phy_idx, double_bit);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    /* re-enable parity checking */
    if (rv_enable == SHR_E_NONE) {
        (void)bcmptm_ser_tcam_pt_ser_enable(unit, pt_id_mapped, 1);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to inject ser error to TCAM[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id_mapped)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_xor_bank_ser_inject(int unit, bcmdrd_sid_t sid,
                               int tbl_inst, int index, uint32_t flags)
{
    int rv = SHR_E_NONE, rv_enable = SHR_E_EMPTY;
    bcmptm_ser_control_reg_info_t ctrl_reg_info;
    int double_bit = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ? 1 : 0;
    uint32_t ser_enable = 0;
    bcmdrd_fid_t inject_fid = INVALIDf;

    SHR_FUNC_ENTER(unit);

    ctrl_reg_info.xor_wr_reg = INVALIDr;

    ser_enable = bcmptm_ser_pt_control_field_val_get
        (unit, sid, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    if (ser_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Please make sure [%s] supports parity or ecc checking,"
                             " and the checking is enabled\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        rv = SHR_E_DISABLED;
        SHR_IF_ERR_EXIT(rv);
    }
    rv = bcmptm_ser_injected_sid_and_field_get(unit, sid, tbl_inst, index,
                                               double_bit, &inject_fid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv_enable = bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_EC, 0);
    rv = rv_enable;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_sram_ctrl_reg_get(unit, sid, index, &ctrl_reg_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (ctrl_reg_info.xor_wr_reg == INVALIDr) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "There is no XOR bank of SRAM[%s] index [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), index));
        rv = SHR_E_PARAM;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* Write field DEBUG_DISABLE_XOR_WRITEf to 0, need to update XOR bank */
    rv = bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.xor_wr_reg,
                                     ctrl_reg_info.xor_wr_field, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* inject ser error to appointed sid and fid */
    rv = bcmptm_ser_error_inject(unit, sid, inject_fid, tbl_inst,
                                 -1, index, double_bit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Write field DEBUG_DISABLE_XOR_WRITEf to 1, need not to update XOR bank */
    rv = bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.xor_wr_reg,
                                     ctrl_reg_info.xor_wr_field, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* revert ser error injected to appointed sid and fid, there is no SER error in table bank */
    rv = bcmptm_ser_error_inject(unit, sid, inject_fid, tbl_inst,
                                 -1, index, double_bit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    /*
     * Write field DEBUG_DISABLE_XOR_WRITEf to 0 (default value),
     * there is SER error in XOR bank.
     */
    if (ctrl_reg_info.xor_wr_reg != INVALIDr) {
        (void)bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.xor_wr_reg,
                                          ctrl_reg_info.xor_wr_field, 0);
    }
    /* re-enable parity or ecc checking */
    if (rv_enable == SHR_E_NONE) {
        (void)bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_EC, 1);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to inject ser error to XOR bank of SRAM[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_sram_ser_inject(int unit, bcmdrd_sid_t sid,
                           int tbl_inst, int tbl_copy, int index, uint32_t flags)
{
    int rv = SHR_E_NONE, rv_enable = SHR_E_EMPTY;
    int double_bit = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ? 1 : 0;
    bcmptm_ser_control_reg_info_t ctrl_reg_info;
    uint32_t ser_enable = 0;
    bcmdrd_fid_t inject_fid = INVALIDf;

    SHR_FUNC_ENTER(unit);

    ctrl_reg_info.xor_wr_reg = INVALIDr;

    ser_enable = bcmptm_ser_pt_control_field_val_get
        (unit, sid, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    if (ser_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Please make sure [%s] supports parity or ecc checking,"
                             " and the checking is enabled\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        rv = SHR_E_DISABLED;
        SHR_IF_ERR_EXIT(rv);
    }

    rv = bcmptm_ser_injected_sid_and_field_get(unit, sid, tbl_inst, index,
                                               double_bit, &inject_fid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv_enable = bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_EC, 0);
    rv = rv_enable;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    (void)bcmptm_ser_sram_ctrl_reg_get(unit, sid, index, &ctrl_reg_info);
    /* write field DEBUG_DISABLE_XOR_WRITEf to 1 */
    if (ctrl_reg_info.xor_wr_reg != INVALIDr) {
        rv = bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.xor_wr_reg,
                                         ctrl_reg_info.xor_wr_field, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* inject ser error to appointed sid and fid */
    rv = bcmptm_ser_error_inject(unit, sid, inject_fid, tbl_inst,
                                 tbl_copy, index, double_bit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    /* re-enable parity or ecc checking */
    if (rv_enable == SHR_E_NONE) {
        (void)bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_EC, 1);
    }
    if (ctrl_reg_info.xor_wr_reg != INVALIDr) {
        (void)bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.xor_wr_reg,
                                          ctrl_reg_info.xor_wr_field, 0);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to inject ser error to SRAM[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_err_injection_validate(int unit, bcmdrd_sid_t sid,
                                  int tbl_inst, int tbl_copy, int index,
                                  sal_time_t usleep, uint32_t flags)
{
    uint32_t mem_data[BCMDRD_MAX_PT_WSIZE];
    int rv = SHR_E_NONE, pt_read_rv = SHR_E_NONE;
    bcmdrd_sid_t  reported_sid;
    int ecc_checking = 0;
    int double_bit = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ? 1 : 0;
    int err_in_xor_bank = (BCMPTM_SER_INJECT_ERROR_XOR_BANK & flags) ? 1 : 0;
    bcmptm_ser_control_reg_info_t ctrl_reg_info = { INVALIDr, INVALIDf };
    uint32_t fid_lt = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ?
        SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_DOUBLE_BITf :
        SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_SINGLE_BITf;
    bcmptm_ser_recovery_type_t resp_type;

    SHR_FUNC_ENTER(unit);

    /* Get ecc view SID and index */
    rv = bcmptm_ser_mem_ecc_info_get(unit, sid, index, NULL, NULL, &ecc_checking);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ctrl_reg_info.force_reg = INVALIDr;

    if (err_in_xor_bank) {
        rv = bcmptm_ser_sram_ctrl_reg_get(unit, sid, index, &ctrl_reg_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (ctrl_reg_info.force_reg == INVALIDr) {
            /* can't trigger SER error in XOR bank */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Can't trigger XOR bank of [%s] by read operation, because"
                                 " there is no FORCE_XOR_GENf. Pls send packet to trigger it.\n"),
                      bcmdrd_pt_sid_to_name(unit, sid)));
            rv = SHR_E_UNAVAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        rv = bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.force_reg,
                                         ctrl_reg_info.force_field, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* Trigger ser event */
    pt_read_rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, tbl_copy,
                                    mem_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    rv = bcmptm_ser_validation_hint_msg_print
        (unit, sid, index, double_bit, ecc_checking, pt_read_rv);
    SHR_IF_ERR_EXIT(rv);
    /* Wait SERC thread to correct SER error */
    sal_usleep(usleep);

    reported_sid = bcmptm_ser_log_cache_pt_id_get(unit);

    if (sid != reported_sid) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER reported PT name is [%s] but not [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, reported_sid),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    /* Test whether ser error is corrected */
    pt_read_rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, tbl_copy,
                                    mem_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    /* For SER recovery type is NONE */
    resp_type = bcmptm_ser_pt_status_field_val_get(unit, sid, fid_lt);

    if (resp_type == SER_RECOVERY_NO_OPERATION) {
        rv = bcmptm_ser_correction_validate(unit, sid, double_bit,
                                            ecc_checking, pt_read_rv);
    } else {
        rv = pt_read_rv;
    }
    SHR_IF_ERR_EXIT(rv);

 exit:
    if (err_in_xor_bank && (ctrl_reg_info.force_reg != INVALIDr)) {
        (void)bcmptm_ser_ctrl_reg_disable(unit, ctrl_reg_info.force_reg,
                                          ctrl_reg_info.force_field, 0);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to validate ser error of [%s] index [%d] inst [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), index, tbl_inst));
    }
    SHR_FUNC_EXIT();
}

