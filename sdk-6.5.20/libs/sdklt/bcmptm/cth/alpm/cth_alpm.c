/*! \file cth_alpm.c
 *
 * Custom Handler for ALPM tables
 *
 * This file contains the main function for CTH ALPM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "cth_alpm_device.h"
#include "cth_alpm_usage.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHALPM
#define CTH_ALPM_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)
#define SHR_RETURN      SHR_ERR_EXIT
#define LEVEL2 1
#define LEVEL3 2
#define CTH_ALPM_FIELD_COUNT_MAX  40 /* Big enough */

#define STATE_NORMAL    0
#define STATE_ERROR     1

#define HIT_MODE_DISABLE            0x0
#define HIT_MODE_FORCE_CLEAR        0x1
#define HIT_MODE_FORCE_SET          0x2

#define L1_DB0 0
#define L1_DB1 1    /* Valid only if dual_mode, indicating Global */
#define L1_DBS 2
/*******************************************************************************
 * Private variables
 */

static shr_famm_hdl_t cth_alpm_famm_hdl[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

static bool cth_alpm_supported[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Free fields buffer used for imm operaion.
 *
 * \param [in] u Unit number.
 * \param [in] field Fields buffer
 *
 * \retval nothing
 */
static void
fields_free(int u, bcmltd_field_t **field)
{
    if (field) {
        if (field[0]) {
            sal_free(field[0]);
        }
        sal_free(field);
    }
}

/*!
 * \brief Allocate fields buffer used for imm operaion.
 *
 * \param [in] u Unit number.
 * \param [in] count Count of fields
 *
 * \retval Fields buffer
 */
static bcmltd_field_t **
fields_alloc(int u, size_t count)
{
    uint32_t idx = 0;
    bcmltd_field_t **field_ptrs = NULL;

    field_ptrs = sal_alloc(count * sizeof(bcmltd_field_t *),
                           "bcmptmCthalpmFptr");
    if (field_ptrs == NULL) {
        return NULL;
    }

    field_ptrs[0] = NULL;
    field_ptrs[0] = sal_alloc(count * sizeof(bcmltd_field_t),
                              "bcmptmCthalpmFptrs");
    if (field_ptrs[0] == NULL) {
        sal_free(field_ptrs);
        return NULL;
    }
    sal_memset(field_ptrs[0], 0, count * sizeof(bcmltd_field_t));

    for (idx = 1; idx < count; idx++) {
        field_ptrs[idx] = field_ptrs[0] + idx;
    }
    return field_ptrs;
}

static int
cth_alpm_level1_blocks_get(int u, int m)
{
    const cth_alpm_device_info_t *alpm_dev;

    alpm_dev = bcmptm_cth_alpm_device_info_get(u, m);
    return alpm_dev->spec.l1_blocks;
}

static int
int_cmp(const void *e1, const void *e2)
{
    return (*((int *)e1) - *((int  *)e2));
}

static int
cth_alpm_level1_blocks_map(int u, int m, bcmptm_cth_alpm_control_t *ctrl)
{
    int i, j;
    int num_l1_blocks;
    int marked[ALPM_L1_BLOCKS_MAX] = {0};
    int banks_per_block = 0;
    int bank_bitmap_per_block = 0;

    num_l1_blocks = cth_alpm_level1_blocks_get(u, m);

    /* For devices with l1_bank_bitmap */
    if (ctrl->tot.l1_bank_bitmap) {
        if (m == ALPM_0) {
            banks_per_block = 4;
            bank_bitmap_per_block = 0xF;
        } else {
            banks_per_block = 2;
            bank_bitmap_per_block = 0x3;
        }
    }
    for (i = 0; i < num_l1_blocks; i++) {
        /*
         * Set large value to DB of disable block,
         * so that the disable block can be excluded after sorting.
         */
        if (banks_per_block &&
            (ctrl->tot.l1_bank_bitmap &
                      (bank_bitmap_per_block << (i * banks_per_block))) == 0) {
            ctrl->l1_phy_db[i] |= 0x8000;
        }
        ctrl->l1_db[i] = ctrl->l1_phy_db[i];
    }

    sal_qsort(&ctrl->l1_db[0], num_l1_blocks, sizeof(ctrl->l1_db[0]), int_cmp);

    ctrl->l1_blocks_map_en = 0;
    for (i = 0; i < num_l1_blocks; i++) {
        for (j = 0; j < num_l1_blocks; j++) {
            if (ctrl->l1_db[i] == ctrl->l1_phy_db[j] &&
                marked[j] == 0) {
                ctrl->l1_blocks_l2p[i] = j;
                ctrl->l1_blocks_p2l[j] = i;
                marked[j] = 1;
                if (i != j) {
                    ctrl->l1_blocks_map_en = 1;
                }
                break;
            }
        }
    }

    if (banks_per_block) {
        for (i = 0; i < num_l1_blocks; i++) {
            /* Remove that bit indication */
            ctrl->l1_db[i] &= ~0x8000;
            ctrl->l1_phy_db[i] &= ~0x8000;
        }
    }

    for (i = 0; i < num_l1_blocks; i++) {
        j = ctrl->l1_blocks_l2p[i];
        ctrl->l1_key_input[i] = ctrl->l1_phy_key_input[j];
    }

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        cli_out("num_l1_blocks: %d avail_l1_blocks: %d mapping_en: %d",
                num_l1_blocks,
                banks_per_block ? ctrl->tot.num_l1_banks / banks_per_block :
                                  num_l1_blocks,
                ctrl->l1_blocks_map_en);
        cli_out("\nlogical_db: ");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", ctrl->l1_db[i]);
        }
        cli_out("\nphysical_db: ");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", ctrl->l1_phy_db[i]);
        }
        cli_out("\nlogical_key_input: ");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", ctrl->l1_key_input[i]);
        }
        cli_out("\nphysical_key_input: ");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", ctrl->l1_phy_key_input[i]);
        }
        cli_out("\n%16s", "l1_blocks_l2p: ");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", i);
        }
        cli_out("\n%16s", "");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", ctrl->l1_blocks_l2p[i]);
        }
        cli_out("\n%16s", "l1_blocks_p2l: ");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", i);
        }
        cli_out("\n%16s", "");
        for (i = 0; i < num_l1_blocks; i++) {
            cli_out("%d ", ctrl->l1_blocks_p2l[i]);
        }
        cli_out("\n");
    }

    return SHR_E_NONE;
}

static int
uft_bank_id_map(int u, uint32_t in_id, uint32_t *out_id)
{
    uint8_t i;
    uint32_t val;
    int rv = SHR_E_NOT_FOUND;
    char *mini_ufts[] = {
        "UFT_MINI_BANK0",
        "UFT_MINI_BANK1",
        "UFT_MINI_BANK2",
        "UFT_MINI_BANK3",
        "UFT_MINI_BANK4",
        "UFT_MINI_BANK5"
    };
    char *ufts[] = {
        "UFT_BANK0",
        "UFT_BANK1",
        "UFT_BANK2",
        "UFT_BANK3",
        "UFT_BANK4",
        "UFT_BANK5",
        "UFT_BANK6",
        "UFT_BANK7"
    };

    SHR_FUNC_ENTER(u);
    for (i = 0; i < COUNTOF(mini_ufts); i++) {
        rv = bcmlrd_field_symbol_to_value(u,
                                          DEVICE_EM_GROUPt,
                                          DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf,
                                          mini_ufts[i],
                                          &val);
        if (rv == SHR_E_NOT_FOUND) {
            break;
        }
        SHR_IF_ERR_EXIT(rv);
        if (val == in_id) {
            *out_id = i;
            break;
        }
    }

    if (i == COUNTOF(mini_ufts) || rv == SHR_E_NOT_FOUND) {
        for (i = 0; i < COUNTOF(ufts); i++) {
            SHR_IF_ERR_EXIT(
                bcmlrd_field_symbol_to_value(u,
                                             DEVICE_EM_GROUPt,
                                             DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf,
                                             ufts[i],
                                             &val));
            if (val == in_id) {
                *out_id = i;
                break;
            }
        }
        if (i == COUNTOF(ufts)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get bank info from uft.
 *
 * \param [in] u Unit number.
 * \param [out] control ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
uft_bank_info_get(int u, int m, bcmptm_cth_alpm_control_t *alpm_ctrl)
{
    int b;
    const cth_alpm_device_info_t *dev_info;
    uint32_t val;
    uint8_t i;
    bcmptm_pt_banks_info_t banks_info;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_cth_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    sal_memset(&alpm_ctrl->tot, 0, sizeof(alpm_ctrl->tot));
    if (dev_info->l1_tcams_shared) {
        for (b = 0; b < ALPM_L1_BANKS_MAX; b++) {
            if (dev_info->spec.l1_sid[b] != INVALIDm) {
                bcmptm_cth_uft_bank_info_get_from_rmalpm(u, dev_info->spec.l1_sid[b],
                                                         &banks_info);
                alpm_ctrl->tot.num_l1_banks += banks_info.bank_cnt;
                if (banks_info.bank_cnt == 1) {
                    alpm_ctrl->tot.l1_bank_bitmap |= 1 << b;
                }
            }
        }
    }
    SHR_IF_ERR_EXIT(
        cth_alpm_level1_blocks_map(u, m, alpm_ctrl));

    for (b = 0; b < ALPM_LN_BANKS_MAX; b++) {
        if (dev_info->spec.l2_sid[b] != INVALIDm) {
            bcmptm_cth_uft_bank_info_get_from_rmalpm(u,dev_info->spec.l2_sid[b],
                                                     &banks_info);
            alpm_ctrl->tot.num_l2_banks += banks_info.bank_cnt;
            if (banks_info.bank_cnt == 1) {
                /* For device that has separate pt_sid per bank */
                alpm_ctrl->tot.l2_bank_bitmap |= 1 << b;
            } else {
                for (i = 0; i < banks_info.bank_cnt; i++) {
                    SHR_IF_ERR_EXIT(
                        uft_bank_id_map(u, banks_info.bank[i].bank_id, &val));
                    alpm_ctrl->tot.l2_bank_bitmap |= 1 << val;
                }
            }
        }
        if (dev_info->spec.l3_sid[b] != INVALIDm) {
            bcmptm_cth_uft_bank_info_get_from_rmalpm(u, dev_info->spec.l3_sid[b],
                                                     &banks_info);
            alpm_ctrl->tot.num_l3_banks += banks_info.bank_cnt;
            if (banks_info.bank_cnt == 1) {
                /* For device that has separate pt_sid per bank */
                alpm_ctrl->tot.l3_bank_bitmap |= 1 << b;
            } else {
                for (i = 0; i < banks_info.bank_cnt; i++) {
                    SHR_IF_ERR_EXIT(
                        uft_bank_id_map(u, banks_info.bank[i].bank_id, &val));
                    alpm_ctrl->tot.l3_bank_bitmap |= 1 << val;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read with mreq
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] trans_id Transaction identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_mreq_read(int unit, bcmltd_sid_t lt_id, uint32_t trans_id,
                   bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                    BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    trans_id, NULL, NULL,
                                    entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write with mreq
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] trans_id Transaction identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_mreq_write(int u, bcmltd_sid_t lt_id, uint32_t trans_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(u);
    pt_info.index = index;
    pt_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(u, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read with ireq
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_ireq_read(int unit, bcmltd_sid_t lt_id,
                   bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info, NULL,
                              rsp_entry_wsize, lt_id,
                              entry_data,&rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write with ireq
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_ireq_write(int u, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(u);
    pt_info.index = index;
    pt_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_write(u, 0, pt_id, &pt_info, NULL,
                              entry_data, lt_id, &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse alpm control structure member from field and field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid  Field ID.
 * \param [in] fval Field value
 * \param [out] ctrl ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_field_parse_mtop(int u, uint32_t fid, uint64_t fval,
                                  bcmptm_cth_alpm_control_t *ctrl)
{
    switch (fid) {
    case L3_ALPM_CONTROL_MTOPt_ALPM_MODEf:
        ctrl->alpm_mode = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_NUM_DB_0_LEVELSf:
        ctrl->db0_levels = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_NUM_DB_1_LEVELSf:
        ctrl->db1_levels = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_NUM_DB_2_LEVELSf:
        ctrl->db2_levels = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_NUM_DB_3_LEVELSf:
        ctrl->db3_levels = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_0f:
        ctrl->l1_phy_key_input[0] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_1f:
        ctrl->l1_phy_key_input[1] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_2f:
        ctrl->l1_phy_key_input[2] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_3f:
        ctrl->l1_phy_key_input[3] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_4f:
        ctrl->l1_phy_key_input[4] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_5f:
        ctrl->l1_phy_key_input[5] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_6f:
        ctrl->l1_phy_key_input[6] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_7f:
        ctrl->l1_phy_key_input[7] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_0f:
        ctrl->l1_phy_db[0] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_1f:
        ctrl->l1_phy_db[1] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_2f:
        ctrl->l1_phy_db[2] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_3f:
        ctrl->l1_phy_db[3] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_4f:
        ctrl->l1_phy_db[4] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_5f:
        ctrl->l1_phy_db[5] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_6f:
        ctrl->l1_phy_db[6] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_7f:
        ctrl->l1_phy_db[7] = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DESTINATIONf:
        ctrl->destination = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DESTINATION_MASKf:
        ctrl->destination_mask = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DESTINATION_TYPE_MATCHf:
        ctrl->destination_type_match = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_DESTINATION_TYPE_NON_MATCHf:
        ctrl->destination_type_non_match = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_OPERATIONAL_STATEf:
        break;

    case L3_ALPM_CONTROL_MTOPt_IPV4_UC_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_uc_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV4_UC_VRF_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_uc_vrf_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_uc_override_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV6_UC_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_uc_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV6_UC_VRF_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_uc_vrf_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_uc_override_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_comp_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_comp_sbr = fval;
        break;
    case L3_ALPM_CONTROL_MTOPt_LARGE_VRFf:
        ctrl->large_vrf = fval;
        break;
    default:
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Parse alpm control structure member from field and field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid  Field ID.
 * \param [in] fval Field value
 * \param [out] ctrl ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_field_parse(int u, int m, uint32_t fid, uint64_t fval,
                             bcmptm_cth_alpm_control_t *ctrl)
{
    if (m) {
        return cth_alpm_control_field_parse_mtop(u, fid, fval, ctrl);
    }
    switch (fid) {
    case L3_ALPM_CONTROLt_ALPM_MODEf:
        ctrl->alpm_mode = fval;
        break;
    case L3_ALPM_CONTROLt_NUM_DB_0_LEVELSf:
        ctrl->db0_levels = fval;
        break;
    case L3_ALPM_CONTROLt_NUM_DB_1_LEVELSf:
        ctrl->db1_levels = fval;
        break;
    case L3_ALPM_CONTROLt_NUM_DB_2_LEVELSf:
        ctrl->db2_levels = fval;
        break;
    case L3_ALPM_CONTROLt_NUM_DB_3_LEVELSf:
        ctrl->db3_levels = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_0f:
        ctrl->l1_phy_key_input[0] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_1f:
        ctrl->l1_phy_key_input[1] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_2f:
        ctrl->l1_phy_key_input[2] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_3f:
        ctrl->l1_phy_key_input[3] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_4f:
        ctrl->l1_phy_key_input[4] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_5f:
        ctrl->l1_phy_key_input[5] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_6f:
        ctrl->l1_phy_key_input[6] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_7f:
        ctrl->l1_phy_key_input[7] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_8f:
        ctrl->l1_phy_key_input[8] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_9f:
        ctrl->l1_phy_key_input[9] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_0f:
        ctrl->l1_phy_db[0] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_1f:
        ctrl->l1_phy_db[1] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_2f:
        ctrl->l1_phy_db[2] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_3f:
        ctrl->l1_phy_db[3] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_4f:
        ctrl->l1_phy_db[4] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_5f:
        ctrl->l1_phy_db[5] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_6f:
        ctrl->l1_phy_db[6] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_7f:
        ctrl->l1_phy_db[7] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_8f:
        ctrl->l1_phy_db[8] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_9f:
        ctrl->l1_phy_db[9] = fval;
        break;
    case L3_ALPM_CONTROLt_HIT_MODEf:
        ctrl->hit_support = fval;
        break;
    case L3_ALPM_CONTROLt_DESTINATIONf:
        ctrl->destination = fval;
        break;
    case L3_ALPM_CONTROLt_DESTINATION_MASKf:
        ctrl->destination_mask = fval;
        break;
    case L3_ALPM_CONTROLt_DESTINATION_TYPE_MATCHf:
        ctrl->destination_type_match = fval;
        break;
    case L3_ALPM_CONTROLt_DESTINATION_TYPE_NON_MATCHf:
        ctrl->destination_type_non_match = fval;
        break;
    case L3_ALPM_CONTROLt_OPERATIONAL_STATEf:
        break;

    case L3_ALPM_CONTROLt_IPV4_UC_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_uc_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV4_UC_VRF_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_uc_vrf_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_uc_override_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV6_UC_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_uc_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV6_UC_VRF_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_uc_vrf_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_uc_override_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv4_comp_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXf:
        ctrl->ipv6_comp_sbr = fval;
        break;
    case L3_ALPM_CONTROLt_LARGE_VRFf:
        ctrl->large_vrf = fval;
        break;
    case L3_ALPM_CONTROLt_COMP_KEY_TYPEf:
        ctrl->comp_key_type = fval;
        break;
    default:
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*!
 * \brief L3_ALPM_CONTROLt input fields parsing.
 *
 * Parse IMM L3_ALPM_CONTROLt input fields.
 *
 * \param [in] u Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] ctrl ALPM control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to parse.
 */
static int
cth_alpm_control_list_fields_parse(int u, int m,
                                   const bcmltd_field_t *data,
                                   bcmptm_cth_alpm_control_t *ctrl)
{
    const bcmltd_field_t *gen_field = NULL;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(u);
    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        SHR_IF_ERR_EXIT(
            cth_alpm_control_field_parse(u, m, fid, fval, ctrl));
        gen_field = gen_field->next;
    }
    SHR_IF_ERR_EXIT(
        cth_alpm_level1_blocks_map(u, m, ctrl));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROLt input fields parsing.
 *
 * Parse IMM L3_ALPM_CONTROLt input fields.
 *
 * \param [in] u Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] ctrl ALPM control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to parse.
 */
static int
cth_alpm_control_array_fields_parse(int u, int m,
                                    const bcmltd_fields_t *fields,
                                    bcmptm_cth_alpm_control_t *ctrl)
{
    uint32_t fid, idx;
    uint64_t fval;

    SHR_FUNC_ENTER(u);
    /* Parse data field. */
    for (idx = 0; idx < fields->count; idx++) {
        fid = fields->field[idx]->id;
        fval = fields->field[idx]->data;
        SHR_IF_ERR_EXIT(
            cth_alpm_control_field_parse(u, m, fid, fval, ctrl));
    }
    SHR_IF_ERR_EXIT(
        cth_alpm_level1_blocks_map(u, m, ctrl));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup from imm table and parse to alpm control structure.
 *
 * \param [in] u Unit number.
 * \param [out] ctrl Parsed ALPM control parameters
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
static int
cth_alpm_control_lookup_and_parse(int u, int m, bcmptm_cth_alpm_control_t *ctrl)
{
    bcmltd_fields_t input, output;

    SHR_FUNC_ENTER(u);
    input.count = 0; /* No key */
    input.field = NULL;
    output.count = CTH_ALPM_FIELD_COUNT_MAX;
    output.field = fields_alloc(u, output.count);
    if (m) {
        SHR_IF_ERR_EXIT(
            bcmimm_entry_lookup(u, L3_ALPM_CONTROL_MTOPt, &input, &output));
    } else {
        SHR_IF_ERR_EXIT(
            bcmimm_entry_lookup(u, L3_ALPM_CONTROLt, &input, &output));
    }
    SHR_IF_ERR_EXIT(
        cth_alpm_control_array_fields_parse(u, m, &output, ctrl));
exit:
    fields_free(u, output.field);
    SHR_FUNC_EXIT();
}


/*!
 * \brief L3_ALPM_CONTROLt parsed fields Validation.
 *
 * Validate L3_ALPM_CONTROLt parsed fields.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl Parsed ALPM control parameters
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 */
static int
cth_alpm_control_validate(int u, int m, bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_device_control_validate(u, m, 1, ctrl));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief L3_ALPM_CONTROLt parsed fields Validation.
 *
 * Validate L3_ALPM_CONTROLt parsed fields together with UFT banks.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl Parsed ALPM control parameters
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 */
static int
cth_alpm_control_validate2(int u, int m, bcmptm_cth_alpm_control_t *ctrl)
{

    SHR_FUNC_ENTER(u);

    ctrl->reason = ALPM_ERROR_REASON_BANKS; /* default error reason */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_cth_alpm_device_control_validate(u, m, 2, ctrl));

    /* Verify that levels match with banks */
    if (ctrl->tot.num_l3_banks == 0) {
        if (ctrl->db0_levels >= 3 ||
            ctrl->db1_levels >= 3 ||
            ctrl->db2_levels >= 3 ||
            ctrl->db3_levels >= 3) {
            SHR_RETURN(SHR_E_PARAM);
        }
    }
    if (ctrl->tot.num_l2_banks == 0) {
        if (ctrl->db0_levels >= 2 ||
            ctrl->db1_levels >= 2 ||
            ctrl->db2_levels >= 2 ||
            ctrl->db3_levels >= 2) {
            SHR_RETURN(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cth_alpm_update_read_only_fields(int u, int m, bcmptm_cth_alpm_control_t *ctrl,
                                 uint8_t error)
{
    uint8_t idx;
    uint8_t reason;
    bcmltd_fields_t input, output;
    bcmltd_sid_t sid;
    uint32_t state_fid, mode_fid;

    SHR_FUNC_ENTER(u);

    if (m) {
        sid = L3_ALPM_CONTROL_MTOPt;
        state_fid = L3_ALPM_CONTROL_MTOPt_OPERATIONAL_STATEf;
        mode_fid = L3_ALPM_CONTROL_MTOPt_ALPM_MODEf;
    } else {
        sid = L3_ALPM_CONTROLt;
        state_fid = L3_ALPM_CONTROLt_OPERATIONAL_STATEf;
        mode_fid = L3_ALPM_CONTROLt_ALPM_MODEf;
    }

    input.count = 0; /* No key */
    input.field = NULL;
    output.count = CTH_ALPM_FIELD_COUNT_MAX;
    output.field = fields_alloc(u, output.count);
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(u, sid, &input, &output));

    if (error == STATE_ERROR) {
        reason = ctrl->reason;
    } else {
        reason = ALPM_ERROR_REASON_NONE;
    }

    for (idx = 0; idx < output.count; idx++) {
        if (output.field[idx]->id == state_fid) {
            output.field[idx]->data = reason;
        }
        if (output.field[idx]->id == mode_fid) {
            /* We currently only expose COMBINED or PARALLEL mode. */
            output.field[idx]->data = ctrl->alpm_mode & ALPM_MODE_PARALLEL ? 1 : 0;
        }
    }
    SHR_IF_ERR_EXIT(
        bcmimm_entry_update(u, false, sid, &output));

exit:
    fields_free(u, output.field);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Write pt of l3 alpm control into hardware.
 *
 * Parse IMM L3_ALPM_CONTROLt input fields.
 *
 * \param [in] u Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] trans_id Transaction identifier.
 * \param [in] ctrl ALPM control
 * \param [in] is_init True if init.
 * \param [in] warm True if warmboot.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_write(int u, int m, bcmltd_sid_t lt_id,
                       uint32_t trans_id,
                       bcmptm_cth_alpm_control_t *ctrl,
                       bool is_init,
                       bool warm)
{
    cth_alpm_control_pt_t pts;
    char *evm_name[ALPMS] = {
        "bcmptmEvAlpmCtrlUpdate",
        "bcmptmEvMtopAlpmCtrlUpdate"
    };

    SHR_FUNC_ENTER(u);
    /* Get entry count */
    sal_memset(&pts, 0, sizeof(pts));
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_device_control_encode(u, m, ctrl, &pts));

    /* Allocate physical entries and encode, then write */
    if (pts.count) {
        int size, i;

        size = pts.count * sizeof(bcmdrd_sid_t);
        CTH_ALPM_ALLOC(pts.sid, size, "bcmptmCthalpmSids");
        sal_memset(pts.sid, 0, size);

        size = pts.count * sizeof(alpm_control_entry_t);
        CTH_ALPM_ALLOC(pts.entry, size, "bcmptmCthalpmEntries");
        sal_memset(pts.entry, 0, size);

        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_device_control_encode(u, m, ctrl, &pts));

        for (i = 0; i < pts.count; i++) {
            if (is_init) {
                SHR_IF_ERR_EXIT(
                    cth_alpm_ireq_read(u, lt_id,
                                       pts.sid[i], 0,
                                       (uint32_t *)&pts.entry[i]));
            } else {
                SHR_IF_ERR_EXIT(
                    cth_alpm_mreq_read(u, lt_id, trans_id,
                                       pts.sid[i], 0,
                                       (uint32_t *)&pts.entry[i]));
            }
        }

        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_device_control_encode(u, m, ctrl, &pts));

        /*
         * Notify before making any real changes, such that
         * if such changes are not allowed, we will abort before
         * causing any damage.
         */
        ctrl->rv = SHR_E_NONE;
        bcmevm_publish_event_notify(u, evm_name[m], (uintptr_t)ctrl);
        SHR_IF_ERR_MSG_EXIT(
            ctrl->rv, (BSL_META_U(u, "L3_ALPM_CONTROL operation aborting.\n")));

        for (i = 0; i < pts.count; i++) {
            if (is_init) {
                if (!warm) {
                    SHR_IF_ERR_EXIT(
                        cth_alpm_ireq_write(u, lt_id,
                                            pts.sid[i], 0,
                                            (uint32_t *)&pts.entry[i]));
                }
            } else {
                SHR_IF_ERR_EXIT(
                    cth_alpm_mreq_write(u, lt_id, trans_id,
                                        pts.sid[i], 0,
                                        (uint32_t *)&pts.entry[i]));
            }
        }

    }

exit:
    SHR_FREE(pts.sid);
    SHR_FREE(pts.entry);

    SHR_FUNC_EXIT();
}



/*!
 * \brief L3_ALPM_CONTROL logical table validate callback function.
 *
 * Validate the field values of L3_ALPM_CONTROLt logical table entry.
 *
 * \param [in] u Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid field value.
 */
static int
cth_alpm_control_mtop_imm_validate(int u,
                                   bcmltd_sid_t sid,
                                   bcmimm_entry_event_t event,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context)
{
    bcmptm_cth_alpm_control_t alpm_ctrl;


/*
 * L3_ALPM_CONTROL is a pre-populate LT.
 * It is prepopulated by an internal insert.
 * Users can issue following op codes:
 * INSERT - SHR_E_EXISTS (as entry is always in place)
 * UPDATE - SHR_E_NONE (unless verification error, consider this as SET)
 * LOOKUP - SHR_E_NONE (consider this as GET)
 * DELETE - SHR_E_NONE (not true DELETE, consider this as RESET)
 */

    SHR_FUNC_ENTER(u);
    switch (event) {
    case BCMIMM_ENTRY_INSERT: /* BCMIMM should return SHR_E_EXITS */
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, ALPM_1, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_list_fields_parse(u, ALPM_1, data, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_validate(u, ALPM_1, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, ALPM_1, &alpm_ctrl));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_ERR_EXIT(SHR_E_FAIL);
        /*
         * SHR_IF_ERR_EXIT(
         *     bcmptm_cth_alpm_control_reset(u));
         */
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROL IMM table change callback function for staging.
 *
 * Handle L3_ALPM_CONTROL IMM table change events.
 *
 * \param [in] u Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] key Linked list of the key fields identifying the entry.
 * \param [in] data Linked list of the data fields in the modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
cth_alpm_control_mtop_imm_stage(int u,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                bcmimm_entry_event_t event,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                void *context,
                                bcmltd_fields_t *output_fields)
{
    bcmptm_cth_alpm_control_t ctrl;
    uint8_t error;

    SHR_FUNC_ENTER(u);
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, ALPM_1, &ctrl));
        (void)uft_bank_info_get(u, ALPM_1, &ctrl);
        if (SHR_FAILURE(cth_alpm_control_validate2(u, ALPM_1, &ctrl))) {
            error = STATE_ERROR;
        } else {
            error = STATE_NORMAL;
        }
        if (error == STATE_NORMAL) {
            SHR_IF_ERR_EXIT(
                cth_alpm_control_write(u, ALPM_1, sid, trans_id, &ctrl, false, false));
        } else {
            LOG_VERBOSE(BSL_LS_BCMPTM_CTHALPM,
                        (BSL_META("Changes not published\n")));
        }
        SHR_IF_ERR_EXIT(
            cth_alpm_update_read_only_fields(u, ALPM_1, &ctrl, error));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, ALPM_1, &ctrl));
        break;
    default:
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to L3_ALPM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t cth_alpm_control_mtop_imm_cb = {

    /*! Validate function. */
    .validate = cth_alpm_control_mtop_imm_validate,

    /*! Staging function. */
    .stage = cth_alpm_control_mtop_imm_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};



/*!
 * \brief L3_ALPM_CONTROL logical table validate callback function.
 *
 * Validate the field values of L3_ALPM_CONTROLt logical table entry.
 *
 * \param [in] u Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid field value.
 */
static int
cth_alpm_control_imm_validate(int u,
                              bcmltd_sid_t sid,
                              bcmimm_entry_event_t event,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              void *context)
{
    bcmptm_cth_alpm_control_t alpm_ctrl;


/*
 * L3_ALPM_CONTROL is a pre-populate LT.
 * It is prepopulated by an internal insert.
 * Users can issue following op codes:
 * INSERT - SHR_E_EXISTS (as entry is always in place)
 * UPDATE - SHR_E_NONE (unless verification error, consider this as SET)
 * LOOKUP - SHR_E_NONE (consider this as GET)
 * DELETE - SHR_E_NONE (not true DELETE, consider this as RESET)
 */

    SHR_FUNC_ENTER(u);
    switch (event) {
    case BCMIMM_ENTRY_INSERT: /* BCMIMM should return SHR_E_EXITS */
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, ALPM_0, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_list_fields_parse(u, ALPM_0, data, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_validate(u, ALPM_0, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, ALPM_0, &alpm_ctrl));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_ERR_EXIT(SHR_E_FAIL);
        /*
         * SHR_IF_ERR_EXIT(
         *     bcmptm_cth_alpm_control_reset(u));
         */
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROL IMM table change callback function for staging.
 *
 * Handle L3_ALPM_CONTROL IMM table change events.
 *
 * \param [in] u Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] key Linked list of the key fields identifying the entry.
 * \param [in] data Linked list of the data fields in the modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
cth_alpm_control_imm_stage(int u,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context,
                           bcmltd_fields_t *output_fields)
{
    bcmptm_cth_alpm_control_t ctrl;
    uint8_t error;

    SHR_FUNC_ENTER(u);
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, ALPM_0, &ctrl));
        (void)uft_bank_info_get(u, ALPM_0, &ctrl);
        if (SHR_FAILURE(cth_alpm_control_validate2(u, ALPM_0, &ctrl))) {
            error = STATE_ERROR;
        } else {
            error = STATE_NORMAL;
        }
        if (error == STATE_NORMAL) {
            SHR_IF_ERR_EXIT(
                cth_alpm_control_write(u, ALPM_0, sid, trans_id, &ctrl, false, false));
        } else {
            LOG_VERBOSE(BSL_LS_BCMPTM_CTHALPM,
                        (BSL_META("Changes not published\n")));
        }
        SHR_IF_ERR_EXIT(
            cth_alpm_update_read_only_fields(u, ALPM_0, &ctrl, error));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, ALPM_0, &ctrl));
        break;
    default:
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to L3_ALPM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t cth_alpm_control_imm_cb = {

    /*! Validate function. */
    .validate = cth_alpm_control_imm_validate,

    /*! Staging function. */
    .stage = cth_alpm_control_imm_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


static int
cth_alpm_control_imm_register(int u, int m)
{
    SHR_FUNC_ENTER(u);
    if (m == ALPM_0) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(u,
                                L3_ALPM_CONTROLt,
                                &cth_alpm_control_imm_cb,
                                NULL));
    } else {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(u,
                                L3_ALPM_CONTROL_MTOPt,
                                &cth_alpm_control_mtop_imm_cb,
                                NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

static void
handle_uft_group_commit_event_mtop(int u, const char *event, uint64_t ev_data)
{
    int rv;
    bcmptm_cth_alpm_control_t alpm_ctrl;
    uint8_t error;

    (void)bcmptm_cth_alpm_control_get(u, ALPM_1, &alpm_ctrl);
    (void)uft_bank_info_get(u, ALPM_1, &alpm_ctrl);
    if (SHR_FAILURE(cth_alpm_control_validate2(u, ALPM_1, &alpm_ctrl))) {
        error = STATE_ERROR;
    } else {
        error = STATE_NORMAL;
    }
    if (error == STATE_NORMAL) {
        rv = cth_alpm_control_write(u, ALPM_1,
                                    BCMPTM_UFT_EV_INFO_LTID_GET(ev_data),
                                    BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data),
                                    &alpm_ctrl, false, false);
        if (rv == SHR_E_BUSY) {
            error = STATE_ERROR;
            alpm_ctrl.reason = ALPM_ERROR_REASON_IN_USE;
        } else if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCMPTM_CTHALPM,
                      (BSL_META("Fail to write alpm control upon uft group commit\n")));
            return;
        }
    } else {
        LOG_VERBOSE(BSL_LS_BCMPTM_CTHALPM,
                    (BSL_META("Changes not published\n")));
    }
    (void)cth_alpm_update_read_only_fields(u, ALPM_1, &alpm_ctrl, error);
    (void)bcmptm_cth_alpm_control_set(u, ALPM_1, &alpm_ctrl);
}

static void
handle_uft_group_commit_event(int u, const char *event, uint64_t ev_data)
{
    int rv;
    bcmptm_cth_alpm_control_t alpm_ctrl;
    uint8_t error;

    (void)bcmptm_cth_alpm_control_get(u, ALPM_0, &alpm_ctrl);
    (void)uft_bank_info_get(u, ALPM_0, &alpm_ctrl);
    if (SHR_FAILURE(cth_alpm_control_validate2(u, ALPM_0, &alpm_ctrl))) {
        error = STATE_ERROR;
    } else {
        error = STATE_NORMAL;
    }
    if (error == STATE_NORMAL) {
        rv = cth_alpm_control_write(u, ALPM_0,
                                    BCMPTM_UFT_EV_INFO_LTID_GET(ev_data),
                                    BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data),
                                    &alpm_ctrl, false, false);
        if (rv == SHR_E_BUSY) {
            error = STATE_ERROR;
            alpm_ctrl.reason = ALPM_ERROR_REASON_IN_USE;
        } else if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCMPTM_CTHALPM,
                      (BSL_META("Fail to write alpm control upon uft group commit\n")));
            return;
        }
    } else {
        LOG_VERBOSE(BSL_LS_BCMPTM_CTHALPM,
                    (BSL_META("Changes not published\n")));
    }
    (void)cth_alpm_update_read_only_fields(u, ALPM_0, &alpm_ctrl, error);
    (void)bcmptm_cth_alpm_control_set(u, ALPM_0, &alpm_ctrl);
}

static int
cth_alpm_control_default_init(int u, int m, bool warm)
{
    const cth_alpm_device_info_t *dev_info;
    bcmptm_cth_alpm_control_t default_control;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_cth_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    default_control = dev_info->spec.default_control;
    if (warm) {
        /*
         * It appears some of the fields may not be recorded and parsed in
         * warmboot, so we need first set it with device default.
         */
        SHR_IF_ERR_EXIT(
            cth_alpm_control_lookup_and_parse(u, m, &default_control));
    }
    SHR_IF_ERR_EXIT(
        uft_bank_info_get(u, m, &default_control));
    SHR_IF_ERR_EXIT(
        cth_alpm_control_validate(u, m, &default_control));

    if (warm) {
        /* Don't care validation result in case of warmboot. */
        (void)cth_alpm_control_validate2(u, m, &default_control);
    } else {
        SHR_IF_ERR_EXIT(
            cth_alpm_control_validate2(u, m, &default_control));
    }
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_control_default_set(u, m, &default_control));
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Pre-insert the L3_ALPM_CONTROL LT with default values.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl Default ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_mtop_pre_insert(int u, bcmptm_cth_alpm_control_t *ctrl)
{
    bcmltd_fields_t fields = {0};
    uint32_t f_cnt = 0;
    bool unmapped;

    SHR_FUNC_ENTER(u);
    if (!cth_alpm_famm_hdl[u][ALPM_1]) {
        SHR_IF_ERR_EXIT(
            shr_famm_hdl_init(CTH_ALPM_FIELD_COUNT_MAX,
                              &cth_alpm_famm_hdl[u][ALPM_1]));
    }
    fields.field = shr_famm_alloc(cth_alpm_famm_hdl[u][ALPM_1], CTH_ALPM_FIELD_COUNT_MAX);
    if (fields.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    f_cnt = 0;
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROL_MTOPt,
                                 L3_ALPM_CONTROL_MTOPt_ALPM_MODEf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_ALPM_MODEf;
        fields.field[f_cnt++]->data = ctrl->alpm_mode;
    }

    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_NUM_DB_0_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db0_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_NUM_DB_1_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db1_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_NUM_DB_2_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db2_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_NUM_DB_3_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db3_levels;

    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_0f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[0];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_1f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[1];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_2f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[2];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_3f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[3];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_4f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[4];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_5f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[5];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_6f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[6];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_KEY_INPUT_LEVEL_1_BLOCK_7f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[7];

    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_0f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[0];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_1f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[1];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_2f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[2];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_3f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[3];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_4f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[4];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_5f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[5];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_6f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[6];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DB_LEVEL_1_BLOCK_7f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_db[7];

    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROL_MTOPt,
                                 L3_ALPM_CONTROL_MTOPt_IPV4_UC_STRENGTH_PROFILE_INDEXf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV4_UC_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_uc_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV4_UC_VRF_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_uc_vrf_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_uc_override_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV6_UC_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_uc_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV6_UC_VRF_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_uc_vrf_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_uc_override_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_comp_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_comp_sbr;
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROL_MTOPt,
                                 L3_ALPM_CONTROL_MTOPt_LARGE_VRFf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_LARGE_VRFf;
        fields.field[f_cnt++]->data = ctrl->large_vrf;
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROL_MTOPt,
                                 L3_ALPM_CONTROL_MTOPt_DESTINATIONf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DESTINATIONf;
        fields.field[f_cnt++]->data = ctrl->destination;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DESTINATION_MASKf;
        fields.field[f_cnt++]->data = ctrl->destination_mask;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DESTINATION_TYPE_MATCHf;
        fields.field[f_cnt++]->data = ctrl->destination_type_match;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_DESTINATION_TYPE_NON_MATCHf;
        fields.field[f_cnt++]->data = ctrl->destination_type_non_match;
    }

    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROL_MTOPt_OPERATIONAL_STATEf;
    fields.field[f_cnt++]->data = STATE_NORMAL;
    fields.count = f_cnt;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_insert(u, L3_ALPM_CONTROL_MTOPt, &fields));
exit:
    if (fields.field) {
        shr_famm_free(cth_alpm_famm_hdl[u][ALPM_1], fields.field, CTH_ALPM_FIELD_COUNT_MAX);
    }
    if (cth_alpm_famm_hdl[u][ALPM_1]) {
        shr_famm_hdl_delete(cth_alpm_famm_hdl[u][ALPM_1]);
        cth_alpm_famm_hdl[u][ALPM_1] = 0;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pre-insert the L3_ALPM_CONTROL LT with default values.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl Default ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_pre_insert(int u, int m, bcmptm_cth_alpm_control_t *ctrl)
{
    bcmltd_fields_t fields = {0};
    uint32_t f_cnt = 0;
    bool unmapped;

    SHR_FUNC_ENTER(u);

    if (m) {
        SHR_IF_ERR_EXIT(
            cth_alpm_control_mtop_pre_insert(u, ctrl));
        SHR_EXIT();
    }

    if (!cth_alpm_famm_hdl[u][m]) {
        SHR_IF_ERR_EXIT(
            shr_famm_hdl_init(CTH_ALPM_FIELD_COUNT_MAX,
                              &cth_alpm_famm_hdl[u][m]));
    }
    fields.field = shr_famm_alloc(cth_alpm_famm_hdl[u][m], CTH_ALPM_FIELD_COUNT_MAX);
    if (fields.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    f_cnt = 0;
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_ALPM_MODEf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_ALPM_MODEf;
        fields.field[f_cnt++]->data = ctrl->alpm_mode;
    }

    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_NUM_DB_0_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db0_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_NUM_DB_1_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db1_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_0f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[0];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_1f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[1];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_2f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[2];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_3f;
    fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[3];
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_0f, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_0f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[0];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_1f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[1];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_2f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[2];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_3f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[3];
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_4f, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_NUM_DB_2_LEVELSf;
        fields.field[f_cnt++]->data = ctrl->db2_levels;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_NUM_DB_3_LEVELSf;
        fields.field[f_cnt++]->data = ctrl->db3_levels;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_4f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[4];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_5f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[5];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_6f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[6];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_7f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[7];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_4f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[4];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_5f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[5];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_6f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[6];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_7f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[7];
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_8f, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_8f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[8];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_9f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_db[9];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_8f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[8];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_9f;
        fields.field[f_cnt++]->data = ctrl->l1_phy_key_input[9];
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_IPV4_UC_STRENGTH_PROFILE_INDEXf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV4_UC_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_uc_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV4_UC_VRF_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_uc_vrf_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_uc_override_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV6_UC_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_uc_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV6_UC_VRF_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_uc_vrf_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_uc_override_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv4_comp_sbr;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXf;
        fields.field[f_cnt++]->data = ctrl->ipv6_comp_sbr;
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_HIT_MODEf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_HIT_MODEf;
        fields.field[f_cnt++]->data = ctrl->hit_support;
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_LARGE_VRFf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_LARGE_VRFf;
        fields.field[f_cnt++]->data = ctrl->large_vrf;
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_COMP_KEY_TYPEf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_COMP_KEY_TYPEf;
        fields.field[f_cnt++]->data = ctrl->comp_key_type;
    }
    SHR_IF_ERR_EXIT(
        bcmlrd_field_is_unmapped(u, L3_ALPM_CONTROLt,
                                 L3_ALPM_CONTROLt_DESTINATIONf, &unmapped));
    if (!unmapped) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DESTINATIONf;
        fields.field[f_cnt++]->data = ctrl->destination;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DESTINATION_MASKf;
        fields.field[f_cnt++]->data = ctrl->destination_mask;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DESTINATION_TYPE_MATCHf;
        fields.field[f_cnt++]->data = ctrl->destination_type_match;
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DESTINATION_TYPE_NON_MATCHf;
        fields.field[f_cnt++]->data = ctrl->destination_type_non_match;
    }

    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_OPERATIONAL_STATEf;
    fields.field[f_cnt++]->data = STATE_NORMAL;
    fields.count = f_cnt;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_insert(u, L3_ALPM_CONTROLt, &fields));
exit:
    if (fields.field) {
        shr_famm_free(cth_alpm_famm_hdl[u][m], fields.field, CTH_ALPM_FIELD_COUNT_MAX);
    }
    if (cth_alpm_famm_hdl[u][m]) {
        shr_famm_hdl_delete(cth_alpm_famm_hdl[u][m]);
        cth_alpm_famm_hdl[u][m] = 0;
    }
    SHR_FUNC_EXIT();
}

static bool
cth_alpm_device_is_supported(int u, int m)
{
    return cth_alpm_supported[u][m];
}

static int
cth_alpm_init(int u, int m, bool warm)
{
    int rv;
    bcmptm_cth_alpm_control_t ctrl;
    const bcmlrd_map_t *map;
    const cth_alpm_device_info_t *dev_info;
    bcmlrd_sid_t control_lt[ALPMS] = {
        L3_ALPM_CONTROLt,
        L3_ALPM_CONTROL_MTOPt
    };
    bcmevm_event_cb *evm_cb[ALPMS] = {
        handle_uft_group_commit_event,
        handle_uft_group_commit_event_mtop
    };

    SHR_FUNC_ENTER(u);

    map = NULL;
    rv = bcmlrd_map_get(u, control_lt[m], &map);
    if (!(SHR_SUCCESS(rv) && map)) {
        cth_alpm_supported[u][m] = false;
        SHR_EXIT();
    }

    rv = bcmptm_cth_alpm_device_init(u, m);
    if (SHR_SUCCESS(rv)) {
        dev_info = bcmptm_cth_alpm_device_info_get(u, m);
        SHR_NULL_CHECK(dev_info, SHR_E_INIT);
        cth_alpm_supported[u][m] = true;
    } else {
        cth_alpm_supported[u][m] = false;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        cth_alpm_control_default_init(u, m, warm));
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_control_default_get(u, m, &ctrl));
    /* Simply to get db0 db1 ... info from tot. */
    rv = cth_alpm_control_write(u, m, L3_ALPM_CONTROLt, 0,
                                &ctrl, true, warm);
    if (!warm) {
        SHR_IF_ERR_EXIT(rv);
    }

    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, m, &ctrl));
        if (!warm) {
            SHR_IF_ERR_EXIT(
                cth_alpm_control_pre_insert(u, m, &ctrl));
            SHR_IF_ERR_EXIT(
                bcmptm_cth_alpm_usage_pre_insert(u, m));
        }
    }

    SHR_IF_ERR_EXIT(
        cth_alpm_control_imm_register(u, m));
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_usage_imm_register(u, m));

    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(u,
            BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT, evm_cb[m]));
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(u,
            BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT, evm_cb[m]));
    SHR_IF_ERR_EXIT(rv);

exit:
    if (cth_alpm_supported[u][m] == false) {
        bcmptm_cth_alpm_device_cleanup(u, m);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_cth_alpm_init(int u, bool warm)
{
    int m = ALPM_0;


    SHR_FUNC_ENTER(u);
    do {
        SHR_IF_ERR_EXIT(
            cth_alpm_init(u, m, warm));
    } while (++m < ALPMS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_stop(int u)
{
    int m = ALPM_0;
    bcmevm_event_cb *evm_cb[ALPMS] = {
        handle_uft_group_commit_event,
        handle_uft_group_commit_event_mtop
    };
    SHR_FUNC_ENTER(u);

    do {
        if (cth_alpm_device_is_supported(u, m)) {
            SHR_IF_ERR_EXIT(
                bcmevm_unregister_published_event(u,
                                                  BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                                  evm_cb[m]));
            SHR_IF_ERR_EXIT(
                bcmevm_unregister_published_event(u,
                                                  BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT,
                                                  evm_cb[m]));

        }
    } while (++m < ALPMS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_cleanup(int u, bool warm)
{
    int m = ALPM_0;

    SHR_FUNC_ENTER(u);
    do {
        if (cth_alpm_device_is_supported(u, m)) {
            bcmptm_cth_alpm_device_cleanup(u, m);
        }
    } while (++m < ALPMS);
    SHR_FUNC_EXIT();
}

