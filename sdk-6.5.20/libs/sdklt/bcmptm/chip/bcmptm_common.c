/*! \file bcmptm_common.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_ptrm_table.h>
#include "./bcmptm_common.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC

#define BCMPTM_WAL_TRANS_MAX_COUNT 1448
#define BCMPTM_WAL_MSG_MAX_COUNT 1448
#define BCMPTM_WAL_WORDS_BUF_MAX_COUNT 491520
#define BCMPTM_WAL_UNDO_WORDS_BUF_MAX_COUNT 491520
#define BCMPTM_WAL_OPS_INFO_MAX_COUNT 49152
#define BCMPTM_WAL_UNDO_OPS_INFO_MAX_COUNT 49152
#define BCMPTM_WAL_MAX_WORDS_IN_MSG 352 /* 16 max size writes (22 words each) */
/* Max number of cwords in any Write or Read op */
#define BCMPTM_WAL_MAX_CWORDS_WSIZE 2


/*******************************************************************************
 * Typedefs
 */

static bcmptm_ptrm_table_t *sorted_ptrm_tbl[BCMDRD_CONFIG_MAX_UNITS];
static size_t sorted_ptrm_tbl_count[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private variables
 */
static bool ptm_ready[BCMDRD_CONFIG_MAX_UNITS];
static fn_ptr_all_t *fn_ptr_all[BCMDRD_CONFIG_MAX_UNITS];

static const bcmptm_ptrm_table_data_t
*bcmptm_ptrm_unit_table_data[BCMDRD_CONFIG_MAX_UNITS];

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { bcmptm_##_bc##_fnptr_attach }, /* 8th param is _bc (SW Base Cfg) */
static struct {
    fnptr_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/* Array of device specific fnptr_detach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { bcmptm_##_bc##_fnptr_detach }, /* 8th param is _bc (SW Base Cfg) */
static struct {
    fnptr_detach_f detach;
} list_dev_detach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};


/*! Array of variant specific attach functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_ptrm_table_data,

static const bcmptm_ptrm_table_data_t *bcmptm_ptrm_variant_table_data[] = {
    NULL, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL /* end-of-list */
};

/*! Define stub functions for UFT base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
int _bc##_ptm_uft_var_drv_attach(bcmptm_uft_var_drv_t *drv) {return SHR_E_NONE;}
#include <bcmdrd/bcmdrd_devlist.h>

/*! Array of device variant specific for uft pointers. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ptm_uft_var_drv_attach },
static struct {
    uft_var_drv_attach_f attach;
} uft_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmptm_uft_var_drv_t *uft_var_drv[BCMDRD_CONFIG_MAX_UNITS];

/*! Define stub functions for SBR base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
int _bc##_ptm_sbr_var_drv_attach(bcmptm_sbr_var_drv_t *drv) {return SHR_E_NONE;}
#include <bcmdrd/bcmdrd_devlist.h>

/*! Array of device variant specific for SBR pointers. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ptm_sbr_var_drv_attach },
static struct {
    sbr_var_drv_attach_f attach;
} sbr_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmptm_sbr_var_drv_t *sbr_var_drv[BCMDRD_CONFIG_MAX_UNITS];

/* Define stub functions for base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
int _bc##_ptm_var_attach(int unit) {return SHR_E_NONE;}
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

/* Array of device variant specific for ALPM pointers. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ptm_var_attach },
static struct {
    ptm_var_attach_f attach;
} ptm_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

/*******************************************************************************
 * Private Functions
 */
static int ptrm_table_comp(const void *x, const void *y) {
    bcmptm_ptrm_table_t *p1 = (bcmptm_ptrm_table_t *)x;
    bcmptm_ptrm_table_t *p2 = (bcmptm_ptrm_table_t *)y;

    return (int)p1->sid - (int)p2->sid;
}

static int
uft_var_drv_init(int unit)
{
    size_t size;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(uft_var_drv[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for func ptrs for this unit. */
    size = sizeof(bcmptm_uft_var_drv_t);
    uft_var_drv[unit] = sal_alloc(size, "bcmptmUftVarDrv");
    SHR_NULL_CHECK(uft_var_drv[unit], SHR_E_MEMORY);
    sal_memset(uft_var_drv[unit], 0, size);

exit:
    SHR_FUNC_EXIT();
}

static int
uft_var_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(uft_var_drv[unit], SHR_E_INIT);

    /* Release space allocated for func ptrs */
    sal_free(uft_var_drv[unit]);
    uft_var_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}


static int
uft_var_drv_attach(int unit,
                   bcmlrd_variant_t variant)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (uft_variant_attach[variant].attach(uft_var_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}

static int
sbr_var_drv_init(int unit)
{
    size_t size;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(sbr_var_drv[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for func ptrs for this unit. */
    size = sizeof(bcmptm_sbr_var_drv_t);
    sbr_var_drv[unit] = sal_alloc(size, "bcmptmSbrVarDrv");
    SHR_NULL_CHECK(sbr_var_drv[unit], SHR_E_MEMORY);
    sal_memset(sbr_var_drv[unit], 0, size);

exit:
    SHR_FUNC_EXIT();
}

static int
sbr_var_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sbr_var_drv[unit], SHR_E_INIT);

    /* Release space allocated for func ptrs */
    sal_free(sbr_var_drv[unit]);
    sbr_var_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
sbr_var_drv_attach(int unit,
                   bcmlrd_variant_t variant)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (sbr_variant_attach[variant].attach(sbr_var_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}

static int
ptm_var_drv_attach(int unit,
                   bcmlrd_variant_t variant)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ptm_variant_attach[variant].attach(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_fnptr_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific fnptr_attach */
    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(unit, fn_ptr_all[unit]));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_fnptr_init(int unit)
{
    size_t size;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(fn_ptr_all[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for all func ptrs for this unit */
    size = sizeof(fn_ptr_all_t);
    SHR_ALLOC(fn_ptr_all[unit], size, "bcmptmAllFnPtrs");
    SHR_NULL_CHECK(fn_ptr_all[unit], SHR_E_MEMORY);
    sal_memset(fn_ptr_all[unit], 0, size); /* clean slate */

    SHR_IF_ERR_EXIT(bcmptm_fnptr_attach(unit));

    ptm_ready[unit] = FALSE;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_fnptr_cleanup(int unit)
{
    bcmdrd_dev_type_t dev_type;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fn_ptr_all[unit], SHR_E_INTERNAL);

    /* Perform device-specific fnptr_detach */
    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_IF_ERR_EXIT
        (list_dev_detach[dev_type].detach(unit));

    /* Release space allocated for dev and variant func ptrs */
    SHR_FREE(fn_ptr_all[unit]);

    ptm_ready[unit] = FALSE;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_var_data_init(int unit)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform uft variant-specific data attach. */
    SHR_IF_ERR_EXIT(uft_var_drv_init(unit));
    SHR_IF_ERR_EXIT(uft_var_drv_attach(unit, variant));

    /* Perform SBR variant-specific data attach. */
    SHR_IF_ERR_EXIT(sbr_var_drv_init(unit));
    SHR_IF_ERR_EXIT(sbr_var_drv_attach(unit, variant));

    /* Perform variant-specific data attach */
    bcmptm_ptrm_unit_table_data[unit] = bcmptm_ptrm_variant_table_data[variant];

    /* Perform PTM variant-specific driver attach. */
    SHR_IF_ERR_EXIT(ptm_var_drv_attach(unit, variant));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_var_data_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(uft_var_drv_cleanup(unit));
    SHR_IF_ERR_EXIT(sbr_var_drv_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_group_info_get(int unit, uint32_t group_id,
                         uint32_t *pt_count, bool *bank_start_en,
                         const bcmdrd_sid_t **pt_list)
{
    fn_pt_group_info_get_t my_fn;
    if (!pt_count || !pt_list) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_group_info_get) == NULL) {
        if (group_id == BCMPTM_PT_GROUP_DEFAULT) {
            *pt_count = 1; /* Only one PT group (DEFAULT PT group) */
            *pt_list = NULL; /* We don't have list of SIDs in DEFAULT group */
            if (bank_start_en) {
                *bank_start_en = FALSE; /* default group does not support
                                           bank_start */
            }
            return SHR_E_NONE;
        } else {
            *pt_count = 0; /* No PTs in non-DEFAULT group */
            *pt_list = NULL; /* So list of PTs is empty */
            if (bank_start_en) {
                *bank_start_en = FALSE;
            }
            return SHR_E_INTERNAL; /* Should not be querying for list of PTs
                                      when PT group_count is 1 */
        }
    } else {
        return my_fn(unit, group_id, pt_count, bank_start_en, pt_list);
    }
}

int
bcmptm_pt_group_index_count_set(int unit, uint32_t group_id,
                                bcmptm_index_count_set_info_t
                                *index_count_set_info,
                                uint8_t *bank_count, uint8_t *start_bank_enum)
{
    fn_pt_group_index_count_set_t my_fn;
    if (!bank_count) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_group_index_count_set) == NULL) {
        if (group_id == BCMPTM_PT_GROUP_DEFAULT) {
            return SHR_E_NONE; /* Device with only one group (DEFAULT) where all
                                  PTs have fixed size, so nothing to adjust. */
        } else {
            return SHR_E_INTERNAL; /* Unsupported group */
        }
    } else {
        return my_fn(unit, group_id, index_count_set_info,
                     bank_count, start_bank_enum);
    }
}

int
bcmptm_pt_index_min(int unit, bcmdrd_sid_t sid)
{
    fn_pt_index_min_get_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_pt_index_min_get) == NULL) {
        /* Return min based on bcmdrd values */
        return bcmdrd_pt_index_min(unit, sid);
    } else {
        bool for_ser = FALSE;
        return my_fn(unit, sid, for_ser);
    }
}

int
bcmptm_pt_index_max(int unit, bcmdrd_sid_t sid)
{
    fn_pt_index_max_get_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_pt_index_max_get) == NULL) {
        /* Return max based on bcmdrd values */
        return bcmdrd_pt_index_max(unit, sid);
    } else {
        bool for_ser = FALSE;
        return my_fn(unit, sid, for_ser);
    }
}

uint32_t
bcmptm_pt_index_count(int unit,
                      bcmdrd_sid_t sid)
{
    int index_min, index_max;
    uint32_t index_count = 0;
    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return 0;
    }
    index_min = bcmptm_pt_index_min(unit, sid);
    index_max = bcmptm_pt_index_max(unit, sid);
    if ((index_min) < 0 || (index_max < 0) || (index_min > index_max)) {
        index_count = 0;
    } else {
        index_count = (1 + index_max - index_min);
    }
    return index_count;
}

int
bcmptm_scor_pt_index_min(int unit, bcmdrd_sid_t sid)
{
    fn_pt_index_min_get_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_pt_index_min_get) == NULL) {
        /* Return min based on bcmdrd values */
        return bcmdrd_pt_index_min(unit, sid);
    } else {
        bool for_ser = TRUE;
        return my_fn(unit, sid, for_ser);
    }
}

int
bcmptm_scor_pt_index_max(int unit, bcmdrd_sid_t sid)
{
    fn_pt_index_max_get_t my_fn;
    if ((my_fn = fn_ptr_all[unit]->fn_pt_index_max_get) == NULL) {
        /* Return max based on bcmdrd values */
        return bcmdrd_pt_index_max(unit, sid);
    } else {
        bool for_ser = TRUE;
        return my_fn(unit, sid, for_ser);
    }
}

uint32_t
bcmptm_scor_pt_index_count(int unit, bcmdrd_sid_t sid)
{
    int index_min, index_max;
    uint32_t index_count = 0;
    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return 0;
    }
    index_min = bcmptm_scor_pt_index_min(unit, sid);
    index_max = bcmptm_scor_pt_index_max(unit, sid);
    if ((index_min) < 0 || (index_max < 0) || (index_min > index_max)) {
        index_count = 0;
    } else {
        index_count = (1 + index_max - index_min);
    }
    return index_count;
}

bcmdrd_ser_resp_t
bcmptm_pt_ser_resp(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_pt_ser_info_t ser_info;

    if (SHR_SUCCESS(bcmdrd_pt_ser_info_get(unit, sid, &ser_info))) {
        return ser_info.resp;
    }
    return BCMDRD_SER_RESP_NONE;
}

/*
 * retval SHR_E_NONE - Interpretation successful. Nothing unexpected.
 *                     (SER correction may be needed)
 * retval else - Error/Unexpected
 *
 * Refer to format of ISM_RSP_WORD in regsfile
 * Not all chips can produce all errors.
 */
int
bcmptm_rsp_info_get(int unit,
                    bcmptm_op_type_t req_op,
                    bcmbd_tbl_resp_info_t *bd_rsp_info,
                    bcmptm_rsp_info_t *rsp_info) /* out */
{
    uint32_t bd_rsp_info_word;
    uint32_t bd_rsp_type;
    uint32_t bd_rsp_err_info;
    int tmp_rv;
    fn_rsp_info_get_t my_fn;
    SHR_FUNC_ENTER(unit);

    if ((my_fn = fn_ptr_all[unit]->fn_rsp_info_get) == NULL) { /* default */
        SHR_NULL_CHECK(bd_rsp_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);

        sal_memset(rsp_info, 0, sizeof(bcmptm_rsp_info_t)); /* default */

        bd_rsp_info_word      = bd_rsp_info->resp_word;
        bd_rsp_type           = (bd_rsp_info_word & 0xF0000000) >> 28; /* 31:28 */
        bd_rsp_err_info       = (bd_rsp_info_word & 0x0F000000) >> 24; /* 27:24 */
        /* rsvd               = (bd_rsp_info_word & 0x00F00000) >> 20;    23:20 */
        rsp_info->entry_index = (bd_rsp_info_word & 0x000FFFFF);       /* 19: 0 */

        switch (bd_rsp_type) {
            case 0: /* FOUND - possible for LOOKUP */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_LOOKUP? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NONE;
                break;

            case 1: /* NOT_FOUND - possible for LOOKUP, DELETE */
                SHR_IF_ERR_EXIT(req_op == BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NOT_FOUND;
                break;

            case 2: /* FULL - possible for INSERT */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_FULL;
                break;

            case 3: /* INSERTED - possible for INSERT */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NONE;
                break;

            case 4: /* REPLACED - possible for INSERT */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_INSERT? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                rsp_info->replaced = TRUE;
                tmp_rv = SHR_E_NONE;
                break;

            case 5: /* DELETED - possible for DELETE */
                SHR_IF_ERR_EXIT(req_op != BCMPTM_OP_DELETE? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_NONE;
                break;

            case 6: /* L2_MOD_FIFO_FULL - possible for INSERT, DELETE */
                SHR_IF_ERR_EXIT(req_op == BCMPTM_OP_LOOKUP? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                tmp_rv = SHR_E_BUSY;
                break;

            case 15: /* ERROR */
                switch (bd_rsp_err_info) {
                    case 0: /* error in hash table */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_lp_word = FALSE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 1: /* error in LP table */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_lp_word = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 2: /* error in action_table_A */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_action_table_a = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 3: /* error in remap_table_A */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_remap_table_a = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 4: /* error in action_table_B */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_action_table_b = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    case 5: /* error in remap_table_B */
                        rsp_info->ser_retry = TRUE;
                        rsp_info->error_in_remap_table_b = TRUE;
                        tmp_rv = SHR_E_NONE;
                        break;
                    default:
                        tmp_rv = SHR_E_INTERNAL;
                        break;
                }
                break; /* ERROR */

            /* Unexpected types - treat as errors */
            case 7:  /* RESERVED_7 */
            case 8:  /* RESERVED_8 */
            case 14: /* PIPELINES_OUT_OF_SYNC */
            default:
                tmp_rv = SHR_E_INTERNAL;
                break;
        } /* rsp_type */
        SHR_ERR_EXIT(tmp_rv);
    } else {
        SHR_ERR_EXIT(
            my_fn(unit, req_op, bd_rsp_info, rsp_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_info_get(int unit, bcmptm_wal_info_t *wal_info) /* out */
{
    fn_wal_info_get_t my_fn;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(wal_info, SHR_E_PARAM);
    sal_memset(wal_info, 0, sizeof(bcmptm_wal_info_t));

    /* Get values from chip-specific function */
    if ((my_fn = fn_ptr_all[unit]->fn_wal_info_get) != NULL) {
        SHR_IF_ERR_EXIT(my_fn(unit, wal_info));
    }

    /* Use 'common' values for params that are 0s.
     *
     * This will allow chip-specific functions to return only exception values.
     * Rest of them will be filled with 'common' values.
     *
     * This will also fill 'common' values for case when there is no
     * chip-specific function. */
    if (!wal_info->trans_max_count) {
        wal_info->trans_max_count = BCMPTM_WAL_TRANS_MAX_COUNT;
    }

    if (!wal_info->msg_max_count) {
        wal_info->msg_max_count = BCMPTM_WAL_MSG_MAX_COUNT;
    }

    if (!wal_info->words_buf_max_count) {
        wal_info->words_buf_max_count = BCMPTM_WAL_WORDS_BUF_MAX_COUNT;
    }

    if (!wal_info->undo_words_buf_max_count) {
        wal_info->undo_words_buf_max_count = BCMPTM_WAL_UNDO_WORDS_BUF_MAX_COUNT;
    }

    if (!wal_info->ops_info_max_count) {
        wal_info->ops_info_max_count = BCMPTM_WAL_OPS_INFO_MAX_COUNT;
    }

    if (!wal_info->undo_ops_info_max_count) {
        wal_info->undo_ops_info_max_count = BCMPTM_WAL_UNDO_OPS_INFO_MAX_COUNT;
    }

    if (!wal_info->max_words_in_msg) {
        wal_info->max_words_in_msg = BCMPTM_WAL_MAX_WORDS_IN_MSG;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_attr_is_alpm(int unit, bcmdrd_sid_t sid, bool *is_alpm)
{
    fn_pt_attr_is_alpm_t my_fn;
    if (is_alpm == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_attr_is_alpm) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, is_alpm);
    }
}

int
bcmptm_pt_overlay_info_get(int unit, bcmdrd_sid_t sid,
                           const bcmptm_overlay_info_t **oinfo)
{
    fn_pt_overlay_info_get_t my_fn;
    if (oinfo == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_overlay_info_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, oinfo);
    }
}

static const bcmptm_dev_info_t default_dev_info = {
    .need_smc_msg = FALSE,
    .need_emc_msg = FALSE,
    .num_cwords = 2,
};
int
bcmptm_dev_info_get(int unit, const bcmptm_dev_info_t **dev_info)
{
    fn_dev_info_get_t my_fn;
    if (dev_info == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_dev_info_get) == NULL) {
        *dev_info = &default_dev_info;
        return SHR_E_NONE;
    } else {
        return my_fn(unit, dev_info);
    }
}

int
bcmptm_pt_overlay_mult_info_get(int unit, bcmdrd_sid_t n_sid,
                                bcmdrd_sid_t w_sid,
                                const bcmptm_overlay_info_t **oinfo)
{
    fn_pt_overlay_mult_info_get_t my_fn;
    if (oinfo == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_overlay_mult_info_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, n_sid, w_sid, oinfo);
    }
}

int
bcmptm_pt_wo_aggr_info_get(int unit, bcmdrd_sid_t sid,
                           bcmptm_wo_aggr_info_t *wo_aggr_info)
{
    fn_pt_wo_aggr_info_get_t my_fn;
    /* ok for wb_aggr_info to be NULL */
    if ((my_fn = fn_ptr_all[unit]->fn_pt_wo_aggr_info_get) == NULL) {
        return SHR_E_UNAVAIL; /* This sid is not wo_aggr */
    } else {
        return my_fn(unit, sid, wo_aggr_info);
    }
}

int
bcmptm_pt_tcam_km_format(int unit, bcmdrd_sid_t sid,
                         bcmptm_pt_tcam_km_format_t *km_format)
{
    fn_pt_tcam_km_format_t my_fn;
    if (km_format == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_tcam_km_format) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, km_format);
    }
}

int
bcmptm_hw_does_km_to_xy(int unit, bool *hw_does_km_to_xy)
{
    fn_hw_does_km_to_xy_t my_fn;
    if (hw_does_km_to_xy == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_hw_does_km_to_xy) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, hw_does_km_to_xy);
    }
}

int
bcmptm_tcam_corrupt_bits_enable(int unit, bool *tc_en)
{
    fn_tcam_corrupt_bits_enable_t my_fn;
    if (tc_en == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_tcam_corrupt_bits_enable) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, tc_en);
    }
}

int
bcmptm_pt_needs_ltid(int unit, bcmdrd_sid_t sid, bool *ltid_en)
{
    fn_pt_needs_ltid_t my_fn;
    if (ltid_en == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_needs_ltid) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, ltid_en);
    }
}

int
bcmptm_pt_needs_dfid(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes)
{
    fn_pt_needs_dfid_t my_fn;
    if (dfid_bytes == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_needs_dfid) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, dfid_bytes);
    }
}

int
bcmptm_pt_in_must_cache_list(int unit, bcmdrd_sid_t sid, bool *must_cache)
{
    fn_pt_in_must_cache_list_t my_fn;
    if (must_cache == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_in_must_cache_list) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, must_cache);
    }
}

int
bcmptm_pt_in_dont_cache_list(int unit, bcmdrd_sid_t sid, bool *dont_cache)
{
    fn_pt_in_dont_cache_list_t my_fn;
    if (dont_cache == NULL) {
        return SHR_E_PARAM;
    }
    if ((my_fn = fn_ptr_all[unit]->fn_pt_in_dont_cache_list) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, dont_cache);
    }
}

int
bcmptm_pt_attr_is_cacheable(int unit, bcmdrd_sid_t sid, bool *cache_en)
{
    bcmdrd_ser_resp_t ser_resp;
    bool in_must_cache_list, in_dont_cache_list;
    SHR_FUNC_ENTER(unit);

    if (cache_en == NULL) {
        return SHR_E_PARAM;
    }
    *cache_en = FALSE; /* default */

    /* Check dont_cache_list */
    SHR_IF_ERR_EXIT(bcmptm_pt_in_dont_cache_list(unit, sid,
                                                 &in_dont_cache_list));
    if (in_dont_cache_list) {
        *cache_en = FALSE;
        SHR_EXIT();
    }

    /* Check must_cache_list */
    SHR_IF_ERR_EXIT(bcmptm_pt_in_must_cache_list(unit, sid,
                                                 &in_must_cache_list));
    if (in_must_cache_list) {
        *cache_en = TRUE;
        SHR_EXIT();
    }

    /* Exclude PTs with ser_resp_entry_clear */
    ser_resp = bcmptm_pt_ser_resp(unit, sid);
    if (ser_resp == BCMDRD_SER_RESP_ENTRY_CLEAR) {
        *cache_en = FALSE;
        SHR_EXIT();
    }

    /* Do as regsfile says */
    if (bcmdrd_pt_is_cacheable(unit, sid)) {
        *cache_en = TRUE;
    } else {
        *cache_en = FALSE;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid:%0d(%s) is declared non-cacheable in regsfile\n"),
             sid,
             bcmdrd_pt_sid_to_name(unit, sid)));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_tcam_memreg_oper(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_sid_type_t sid_type,
                             bcmptm_rm_tcam_sid_oper_t sid_oper,
                             bcmptm_rm_tcam_sid_info_t *sid_info)
{
    fn_pt_rm_tcam_memreg_oper_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_rm_tcam_memreg_oper) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ltid, sid_type, sid_oper, sid_info);
    }
}

int
bcmptm_rm_tcam_prio_only_hw_key_info_init(int unit,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            uint8_t num_entries_per_index,
                            void *hw_field_list)
{
    fn_pt_rm_tcam_prio_only_hw_key_info_init_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_rm_tcam_prio_only_hw_key_info_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ltid, ltid_info, num_entries_per_index, hw_field_list);
    }
}

const bcmptm_ser_data_driver_t *
bcmptm_ser_drv_get(int unit)
{
    fn_ser_data_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_ser_data_driver_register) == NULL) {
        return NULL;
    } else {
        return my_fn(unit);
    }
}

const bcmptm_ctr_flex_data_driver_t *
bcmptm_ctr_flex_data_driver_register(int unit)
{
    fn_ctr_flex_data_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_ctr_flex_data_driver_register) == NULL) {
        return NULL;
    } else {
        return my_fn(unit);
    }
}

int
bcmptm_rm_alpm_driver_register(int unit, bcmptm_rm_alpm_driver_t *drv)
{
    fn_alpm_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_alpm_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_cth_alpm_driver_register(int unit, bcmptm_cth_alpm_driver_t *drv)
{
    fn_cth_alpm_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_cth_alpm_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmlrd_lt_mreq_info_init(int unit)
{
    bcmptm_ptrm_table_t *tmp_sorted_ptrm_tbl = NULL;
    size_t table_data_count;
    size_t sorted_data_count;
    size_t ptrm_table_data_size;
    size_t i;

    SHR_FUNC_ENTER(unit);

    table_data_count = bcmptm_ptrm_unit_table_data[unit]->table_data_count;
    if (table_data_count == 0) {
        SHR_EXIT();
    }

    if (sorted_ptrm_tbl[unit]) {
        /* There is only one call expected per unit, so this pointer must be
         * NULL */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * This section required since the auto generated code defines the
     * array ptrm_table as const which therefore can not be sorted. We
     * therefore copying this array so we can sort the copy.
     */

    ptrm_table_data_size =
        table_data_count *
        sizeof(bcmptm_ptrm_unit_table_data[unit]->table_data[0]);
    SHR_ALLOC(tmp_sorted_ptrm_tbl, ptrm_table_data_size, "bcmptmPtrmTable");
    SHR_NULL_CHECK(tmp_sorted_ptrm_tbl, SHR_E_MEMORY);
    sal_memset(tmp_sorted_ptrm_tbl, 0, ptrm_table_data_size);

    /* Iterate across the table data, eliminating any unmapped
       tables. */
    sorted_data_count = 0;
    for (i = 0; i < table_data_count; i++) {
        const bcmptm_ptrm_table_t *tbl =
            &bcmptm_ptrm_unit_table_data[unit]->table_data[i];
        const bcmlrd_map_t *map;

        if (SHR_SUCCESS(bcmlrd_map_get(unit, tbl->sid, &map))) {
            tmp_sorted_ptrm_tbl[sorted_data_count] = *tbl;
            sorted_data_count++;
        }
    }
    sorted_ptrm_tbl_count[unit] = sorted_data_count;

    /* Now sort the array */
    sal_qsort(tmp_sorted_ptrm_tbl,
              sorted_ptrm_tbl_count[unit],
              sizeof(tmp_sorted_ptrm_tbl[0]),
              ptrm_table_comp);

    sorted_ptrm_tbl[unit] = tmp_sorted_ptrm_tbl;

exit:
    SHR_FUNC_EXIT();
}

int
bcmlrd_lt_mreq_info_cleanup(int unit)
{
    if (sorted_ptrm_tbl[unit]) {
        SHR_FREE(sorted_ptrm_tbl[unit]);
        return SHR_E_NONE;
    } else {
       if (bcmptm_ptrm_unit_table_data[unit]->table_data_count == 0) {
            /* There is no data in PTRM table */
            return SHR_E_NONE;
        } else {
            /* Expected the sorted table array to have some data */
            return SHR_E_INTERNAL;
        }
    }
}

int
bcmlrd_lt_mreq_info_get(int unit,  bcmltd_sid_t req_ltid,
                        const lt_mreq_info_t **lt_mreq_info)
{
    int idx = -1;
    bcmptm_ptrm_table_t *tmp_sorted_ptrm_tbl;

    SHR_FUNC_ENTER(unit);
    tmp_sorted_ptrm_tbl = sorted_ptrm_tbl[unit];

    /* Param check */
    SHR_NULL_CHECK(lt_mreq_info, SHR_E_PARAM);
    if (tmp_sorted_ptrm_tbl == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL); /* without error msg */
    }

    if (bcmptm_ptrm_unit_table_data[unit]->table_data_count == 0) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL); /* without error msg */
    }

    BIN_SEARCH(tmp_sorted_ptrm_tbl,
               .sid,
               req_ltid,
               sorted_ptrm_tbl_count[unit] - 1,
               idx);

    if (idx == -1) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL); /* without error msg */
    } else {
        *lt_mreq_info = tmp_sorted_ptrm_tbl[idx].handler;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_cci_ctrtype_get(int unit,
                          bcmdrd_sid_t sid)
{
    fn_pt_cci_ctrtype_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctrtype_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_pt_cci_ctr_sym_map(int unit,
                          bcmptm_cci_ctr_info_t *ctr_info)
{
    fn_pt_cci_ctr_sym_map_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_sym_map) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ctr_info);
    }
}

int
bcmptm_pt_cci_frmt_sym_get(int unit, int index, int field,
                           bcmdrd_sid_t *sid, size_t *width)
{
    fn_pt_cci_frmt_sym_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_frmt_sym_get) == NULL) {
        return SHR_E_NONE;
    } else {
        return my_fn(unit, index, field, sid, width);
    }
}

int
bcmptm_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                   int *index_min, int *index_max)
{
    fn_pt_cci_frmt_index_from_port_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_frmt_index_from_port) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, port, index_min, index_max);
    }
}

bool
bcmptm_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index)
{
    fn_pt_cci_index_valid_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_index_valid) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, tbl_inst, index);
    }
}

int
bcmptm_pt_cci_ctr_evict_control_get(int unit, bcmdrd_sid_t sid,
                                    bcmdrd_sid_t *ctrl_sid)
{
    fn_pt_cci_ctr_evict_control_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_control_get) == NULL) {
        return SHR_E_NONE;
    } else {
        return my_fn(unit, sid, ctrl_sid);
    }
}

int
bcmptm_pt_cci_ctr_evict_enable_field_get(int unit, bcmdrd_fid_t *fid)
{
    fn_pt_cci_ctr_evict_enable_field_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_enable_field_get)
                                                                      == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit,fid);
    }
}

int
bcmptm_pt_cci_ctr_evict_clr_on_read_field_get(int unit, bcmdrd_fid_t *fid)
{
    fn_pt_cci_ctr_evict_clr_on_read_field_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_clr_on_read_field_get)
                                                                      == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fid);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_default_enable_list_get(int unit,
                                                size_t *size,
                                                bcmdrd_sid_t *sid)
{
    fn_pt_cci_ctr_evict_default_enable_list_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_default_enable_list_get)
                                                                     == NULL) {
        if (size != NULL) {
            *size = 0;
            return SHR_E_NONE;
        } else {
            return SHR_E_PARAM;
        }
    } else {
        return my_fn(unit, size, sid);
    }
}

bool
bcmptm_pt_cci_evict_cor_supported(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_ctr_evict_cor_supported_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_cor_supported) == NULL) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM) ||
            bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        return my_fn(unit, sid);
    }
}

bool
bcmptm_pt_cci_ctr_col_dma_supported(int unit)
{
    fn_pt_cci_ctr_col_dma_supported_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_col_dma_supported) == NULL) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        return my_fn(unit);
    }
}

bool
bcmptm_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_ctr_is_bypass_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_is_bypass) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_uft_driver_register(int unit, uft_driver_t *drv)
{
    fn_uft_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_uft_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_uft_be_driver_register(int unit, uft_be_driver_t *drv)
{
    fn_uft_be_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_uft_be_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_uft_var_info_get(int unit, const bcmptm_uft_var_drv_t **info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(uft_var_drv[unit], SHR_E_INIT);

    *info = uft_var_drv[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_driver_register(int unit, sbr_driver_t *drv)
{
    fn_sbr_driver_register_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_sbr_driver_register) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, drv);
    }
}

int
bcmptm_sbr_var_info_get(int unit, const bcmptm_sbr_var_drv_t **info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sbr_var_drv[unit], SHR_E_INIT);

    *info = sbr_var_drv[unit];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dyn_lt_mreq_info_init(int unit,
                                     uint32_t trans_id,
                                     bcmltd_sid_t ltid,
                                     bool warm)
{
    fn_pt_dyn_lt_mreq_info_init_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_dyn_lt_mreq_info_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, ltid, warm);
    }
}

int
bcmptm_rm_tcam_dynamic_lt_validate(int unit, bcmltd_sid_t ltid)
{
    fn_pt_dynamic_lt_validate_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_dynamic_lt_validate) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ltid);
    }
}

void
bcmptm_ptm_ready_set(int unit, bool ready)
{
    ptm_ready[unit] = ready;
}

bool
bcmptm_ptm_is_ready(int unit)
{
    return ptm_ready[unit];
}

int
bcmptm_do_ha_alloc(int unit, bool warm, uint32_t in_req_size,
                   const char *submod_name, uint8_t submod_id,
                   uint32_t ref_sign, uint32_t **alloc_ptr)
{
    uint32_t req_size, alloc_size, obs_sign;
    SHR_FUNC_ENTER(unit);
    *alloc_ptr = NULL;
    req_size = in_req_size + sizeof(uint32_t); /* 1 extra word for signature */
    alloc_size = req_size;
    *alloc_ptr = shr_ha_mem_alloc(unit,
                                  BCMMGMT_PTM_COMP_ID,
                                  submod_id,
                                  submod_name,
                                  &alloc_size);
    SHR_NULL_CHECK(*alloc_ptr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm) {
        obs_sign = **alloc_ptr;
        SHR_IF_ERR_MSG_EXIT(
            obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(unit, "WB: %s: signature mismatch, "
                        "exp=0x%8x, obs=0x%8x\n"),
             submod_name, ref_sign, obs_sign));
    } else { /* init ref_sign */
        sal_memset(*alloc_ptr, 0, alloc_size);
        **alloc_ptr = ref_sign;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "%s: req bytes = %0u, alloc bytes = %0u, \n"),
         submod_name, req_size, alloc_size));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_cci_fifodma_chan_get(int unit)
{
    fn_pt_cci_fifodma_chan_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_fifodma_chan_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit);
    }
}

int
bcmptm_pt_cci_ctr_evict_fifo_enable_sym_get(int unit,
                                            bcmdrd_sid_t *sid,
                                            bcmdrd_fid_t *fid)
{
    fn_pt_cci_ctr_evict_fifo_enable_sym_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_fifo_enable_sym_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid, fid);
    }
}

int
bcmptm_pt_cci_ctr_evict_fifo_sym_get(int unit, bcmdrd_sid_t *sid)
{
    fn_pt_cci_ctr_evict_fifo_sym_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_fifo_sym_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, sid);
    }
}

int
bcmptm_pt_cci_ctr_evict_fifo_entry_get(int unit,
                                       uint32_t *evict,
                                       uint32_t *pool,
                                       bcmptm_cci_ctr_info_t *info)
{
    fn_pt_cci_ctr_evict_fifo_entry_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_fifo_entry_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, evict, pool, info);
    }
}

uint32_t
bcmptm_pt_cci_ctr_evict_fifo_buf_size_get(int unit)
{
    fn_pt_cci_ctr_evict_fifo_buf_size_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_fifo_buf_size_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_field_num_get(int unit,
                                      bcmptm_cci_ctr_mode_t ctr_mode,
                                      size_t *num)
{
    fn_pt_cci_ctr_evict_field_num_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_field_num_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ctr_mode, num);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_field_width_get(int unit,
                                       bcmptm_cci_ctr_mode_t ctr_mode,
                                       size_t *width)
{
    fn_pt_cci_ctr_evict_field_width_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_field_width_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ctr_mode, width);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_normal_val_get(int unit,
                                       size_t fld_width,
                                       uint32_t *entry,
                                       size_t size_e,
                                       uint64_t *ctr,
                                       size_t size_ctr)
{
    fn_pt_cci_ctr_evict_normal_val_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_normal_val_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fld_width, entry, size_e, ctr, size_ctr);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_wide_val_get(int unit,
                                     size_t fld_width,
                                     uint32_t *entry,
                                     size_t size_e,
                                     uint64_t *ctr,
                                     size_t size_ctr)
{
    fn_pt_cci_ctr_evict_wide_val_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_wide_val_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fld_width, entry, size_e, ctr, size_ctr);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_slim_val_get(int unit,
                                     size_t fld_width,
                                     uint32_t *entry,
                                     size_t size_e,
                                     uint32_t *ctr,
                                     size_t size_ctr)
{
    fn_pt_cci_ctr_evict_slim_val_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_slim_val_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fld_width, entry, size_e, ctr, size_ctr);
    }
}
extern int
bcmptm_pt_cci_ctr_evict_normal_double_val_get(int unit,
                                     size_t fld_width,
                                     uint32_t *entry,
                                     size_t size_e,
                                     uint64_t *ctr,
                                     size_t size_ctr)
{
    fn_pt_cci_ctr_evict_normal_double_val_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_normal_double_val_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fld_width, entry, size_e, ctr, size_ctr);
    }
}

uint32_t
bcmptm_pt_cci_reg_addr_gap_get(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_reg_addr_gap_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_reg_addr_gap_get) == NULL) {
        return BCMPTM_CCI_REG_ADDR_GAP_DEFAULT;
    } else {
        return my_fn(unit, sid);
    }
}

bool
bcmptm_pt_cci_reg_new_block(int unit, bcmdrd_sid_t sid)
{
    fn_pt_cci_reg_new_block_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_reg_new_block) == NULL) {
        return FALSE;
    } else {
        return my_fn(unit, sid);
    }
}

extern int
bcmptm_pt_cci_ctr_evict_update_mode_num_get(int unit,
                                            size_t *num)
{
    fn_pt_cci_ctr_evict_update_mode_num_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_pt_cci_ctr_evict_update_mode_num_get) == NULL) {
        if (num == NULL) {
            return SHR_E_PARAM;
        } else {
            *num = 1;
            return SHR_E_NONE;
        }
    } else {
        return my_fn(unit, num);
    }
}

extern int
bcmptm_mutex_take(int unit, sal_mutex_t mtx, int usec)
{
    int final_usec = usec;
    if (usec != SAL_MUTEX_FOREVER && !bcmdrd_feature_is_real_hw(unit)) {
        final_usec = 100 * usec;
    }
    return sal_mutex_take(mtx, final_usec);
}

int
bcmptm_hash_def_bank_info_get(int unit,
                              uint32_t mask,
                              bcmptm_rm_hash_def_bank_info_t *banks_info)
{
    fn_rm_hash_def_bank_info_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_rm_hash_def_bank_info_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, mask, banks_info);
    }
}

int
bcmptm_hash_table_info_get(int unit,
                           const bcmptm_rm_hash_table_info_t **tbl_info,
                           uint8_t *num_tbl_info)
{
    fn_rm_hash_table_info_get_t my_fn;

    if ((my_fn = fn_ptr_all[unit]->fn_rm_hash_table_info_get) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, tbl_info, num_tbl_info);
    }
}

