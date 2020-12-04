/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/stg.h>
#include <bcm_int/ltsw/mbcm/stg.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STG

static int
vlan_vfi_stg_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{

    SHR_FUNC_ENTER(unit);

    if (!sal_strcmp(FORWARDs, symbol)) {
        *value = BCM_STG_STP_FORWARD;
    } else if (!sal_strcmp(BLOCKs, symbol)) {
        *value = BCM_STG_STP_BLOCK;
    } else if (!sal_strcmp(LEARNs, symbol)) {
        *value = BCM_STG_STP_LEARN;
    } else if (!sal_strcmp(DISABLEs, symbol)) {
        *value = BCM_STG_STP_DISABLE;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_vfi_stg_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value == BCM_STG_STP_FORWARD) {
        *symbol = FORWARDs;
    } else if (value == BCM_STG_STP_BLOCK || value == BCM_STG_STP_LISTEN) {
        *symbol = BLOCKs;
    } else if (value == BCM_STG_STP_LEARN) {
        *symbol = LEARNs;
    } else if (value == BCM_STG_STP_DISABLE) {
        *symbol = DISABLEs;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* VLAN_STG fields. */
static const bcmint_stg_fld_t
vlan_vfi_stg_entry_flds[BCMINT_LTSW_STG_FLD_VLAN_VFI_STG_CNT] = {
    [BCMINT_LTSW_STG_FLD_VLAN_VFI_STG_ID] = {
        VLAN_STG_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_STG_FLD_VLAN_VFI_STG_STATE] = {
        STATEs,
        vlan_vfi_stg_symbol_to_scalar,
        vlan_vfi_stg_scalar_to_symbol
    },
};

static const bcmint_stg_lt_t
bcm56990_a0_stg_lt[] = {
    [BCMINT_LTSW_STG_VLAN_VFI_STG] = {
        .name = VLAN_STGs,
        .fld_bmp =
            (1 << BCMINT_LTSW_STG_FLD_VLAN_VFI_STG_ID) |
            (1 << BCMINT_LTSW_STG_FLD_VLAN_VFI_STG_STATE),
        .flds = vlan_vfi_stg_entry_flds
    }
};

/******************************************************************************
 * Private functions
 */

static int
bcm56990_a0_ltsw_stg_lt_db_get(
    int unit,
    bcmint_stg_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56990_a0. */
    lt_db->lt_bmp = 1 << BCMINT_LTSW_STG_VLAN_VFI_STG;
    lt_db->lts = bcm56990_a0_stg_lt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stg sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_stg_drv_t bcm56990_a0_stg_sub_drv = {
    .stg_lt_db_get = bcm56990_a0_ltsw_stg_lt_db_get,
};


/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_stg_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stg_drv_set(unit, &bcm56990_a0_stg_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
