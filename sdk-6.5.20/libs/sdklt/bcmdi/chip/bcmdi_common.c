/*! \file bcmdi_common.c
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
#include <sal/sal_assert.h>
#include <sal/sal_date.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_util.h>
#include <bcmissu/issu_api.h>
#include <bcmdrd/bcmdrd.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmdi/bcmdi_internal.h>
#include <bcmdi/generated/bcmdi_ha.h>
#include <bcmptm/bcmlrd_ptrm.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMDI_FLEXCODE

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static bcmdi_dvc_drv_t *dvc_drv[BCMDRD_CONFIG_MAX_UNITS];
static bcmdi_flc_drv_t *flc_drv[BCMDRD_CONFIG_MAX_UNITS];

/* Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_di_dvc_drv_attach },
static struct {
    dvc_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific flexcode file pointers */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_di_flc_drv_attach },
static struct {
    flc_drv_attach_f attach;
} list_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

/*******************************************************************************
 * Private Functions
 */
static int
dvc_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(unit, dvc_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Store current DeviceCode information
 *
 * Store current DeviceCode loading information to HA memory
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
dvc_info_ha_store(int unit) {

    dvc_ha_info_t *ha_info;
    bcmdi_dvc_status_t *status;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(dvc_drv[unit]->ha_info, SHR_E_INIT);
    SHR_NULL_CHECK(&dvc_drv[unit]->status, SHR_E_INIT);

    ha_info = (dvc_ha_info_t*) dvc_drv[unit]->ha_info;
    status = &dvc_drv[unit]->status;

    ha_info->loaded = status->loaded;
    ha_info->fid = status->file_info.fid;
    ha_info->entry_num = status->file_info.entry_num;
    sal_strcpy(ha_info->chip_id, status->file_info.chip_id);
    sal_strcpy(ha_info->chip_rev, status->file_info.chip_rev);
    sal_strcpy(ha_info->desc, status->file_info.desc);

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Retrieve last loaded DeviceCode information
 *
 * Retrieve last loaded DeviceCode information from HA memory
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
dvc_info_ha_retrieve(int unit) {

    dvc_ha_info_t *ha_info;
    bcmdi_dvc_status_t *status;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(dvc_drv[unit]->ha_info, SHR_E_INIT);
    SHR_NULL_CHECK(&dvc_drv[unit]->status, SHR_E_INIT);

    ha_info = (dvc_ha_info_t*)dvc_drv[unit]->ha_info;
    status = &dvc_drv[unit]->status;

    status->loaded = ha_info->loaded;
    status->file_info.fid = ha_info->fid;
    status->file_info.entry_num = ha_info->entry_num;
    sal_strcpy(status->file_info.chip_id, ha_info->chip_id);
    sal_strcpy(status->file_info.chip_rev, ha_info->chip_rev);
    sal_strcpy(status->file_info.desc, ha_info->desc);

exit:
    SHR_FUNC_EXIT();
}

static int
dvc_ha_res_alloc(int unit, bool warm) {
    uint32_t length;

    SHR_FUNC_ENTER(unit);
    length = sizeof(dvc_ha_info_t);
    dvc_drv[unit]->ha_info =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_DI_COMP_ID,
                         BCMDI_HA_SUBID_DVC_INFO,
                         "bcmdiDvcInfo",
                         &length);
    SHR_NULL_CHECK (dvc_drv[unit]->ha_info, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(dvc_drv[unit]->ha_info, 0, length);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_DI_COMP_ID,
                                   BCMDI_HA_SUBID_DVC_INFO,
                                   0,
                                   length, 1,
                                   DVC_HA_INFO_T_ID);
    } else {
        dvc_info_ha_retrieve(unit);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
flc_drv_attach(int unit)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_IF_ERR_EXIT
        (list_variant_attach[variant].attach(flc_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}

static int
flc_info_ha_done(int unit)
{
#define FLC_INFO_STR_LEN     0x20
#define FLC_INFO_STR_HEX     0x10
#define FLC_INFO_DATA_WORDS  0x5
#define FLC_INFO_DATA_SHIFT  0x2
    int idx, len;
    char *str = NULL, *ptr = NULL;
    uint32_t ha_base, data[FLC_INFO_DATA_WORDS];
    bcmdi_flc_status_t *status;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(&flc_drv[unit]->status, SHR_E_INIT);

    if (!dvc_drv[unit]->buff_base) {
        SHR_EXIT();
    }

    sal_memset(data, 0, sizeof(data));

    status = &flc_drv[unit]->status;
    len = sal_strlen(status->file_info.fid);
    if (len > 0) {
        assert(len == FLC_INFO_STR_LEN);

        str = sal_strdup(status->file_info.fid);
        SHR_NULL_CHECK(str, SHR_E_INIT);

        for (idx = 1; idx < FLC_INFO_DATA_WORDS; idx++) {
            ptr = str + len - (idx << (FLC_INFO_DATA_SHIFT + 1));
            data[idx] = (uint32_t)sal_strtoul(ptr, NULL, FLC_INFO_STR_HEX);
            data[0] ^= data[idx];
            *ptr = '\0';
        }
    }

    ha_base = dvc_drv[unit]->buff_base(unit);
    for (idx = 0; idx < FLC_INFO_DATA_WORDS; idx++) {
        uint32_t offset = ha_base + SHR_WORDS2BYTES(idx);
        SHR_IF_ERR_EXIT
            (BCMDRD_DEV_WRITE32(unit, offset, data[idx]));
    }

exit:
    if (str) {
        sal_free(str);
    }
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Store current Flexcode information
 *
 * Store current Flexcode loading information to HA memory
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
flc_info_ha_store(int unit) {

    flc_ha_info_t *ha_info;
    bcmdi_flc_status_t *status;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(flc_drv[unit]->ha_info, SHR_E_INIT);
    SHR_NULL_CHECK(&flc_drv[unit]->status, SHR_E_INIT);

    ha_info = (flc_ha_info_t*) flc_drv[unit]->ha_info;
    status = &flc_drv[unit]->status;

    ha_info->loaded = status->loaded;
    ha_info->be = status->file_info.be;
    ha_info->f_size = status->file_info.f_size;
    ha_info->entry_num = status->file_info.entry_num;
    sal_strcpy(ha_info->chip_id, status->file_info.chip_id);
    sal_strcpy(ha_info->chip_rev, status->file_info.chip_rev);
    sal_strcpy(ha_info->fid, status->file_info.fid);
    sal_strcpy(ha_info->app, status->file_info.app);
    sal_strcpy(ha_info->app_ver, status->file_info.app_ver);
    sal_strcpy(ha_info->app_build_time, status->file_info.app_build_time);
    sal_strcpy(ha_info->pid, status->file_info.pid);
    sal_strcpy(ha_info->ncs_ver, status->file_info.ncs_ver);
    sal_strcpy(ha_info->desc, status->file_info.desc);

    SHR_IF_ERR_EXIT
        (flc_info_ha_done(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Retrieve last loaded Flexcode information
 *
 * Retrieve last loaded Flexcode information from HA memory
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
flc_info_ha_retrieve(int unit) {

    flc_ha_info_t *ha_info;
    bcmdi_flc_status_t *status;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(flc_drv[unit]->ha_info, SHR_E_INIT);
    SHR_NULL_CHECK(&flc_drv[unit]->status, SHR_E_INIT);

    ha_info = (flc_ha_info_t*)flc_drv[unit]->ha_info;
    status = &flc_drv[unit]->status;

    status->loaded = ha_info->loaded;
    status->file_info.be = ha_info->be;
    status->file_info.f_size = ha_info->f_size;
    status->file_info.entry_num = ha_info->entry_num;
    sal_strcpy(status->file_info.chip_id, ha_info->chip_id);
    sal_strcpy(status->file_info.chip_rev, ha_info->chip_rev);
    sal_strcpy(status->file_info.fid, ha_info->fid);
    sal_strcpy(status->file_info.app, ha_info->app);
    sal_strcpy(status->file_info.app_ver, ha_info->app_ver);
    sal_strcpy(status->file_info.app_build_time, ha_info->app_build_time);
    sal_strcpy(status->file_info.ncs_ver, ha_info->ncs_ver);
    sal_strcpy(status->file_info.pid, ha_info->pid);
    sal_strcpy(status->file_info.desc, ha_info->desc);

exit:
    SHR_FUNC_EXIT();
}

static int
flc_ha_res_alloc(int unit, bool warm) {
    uint32_t length;

    SHR_FUNC_ENTER(unit);
    length = sizeof(flc_ha_info_t);
    flc_drv[unit]->ha_info =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_DI_COMP_ID,
                         BCMDI_HA_SUBID_FLC_INFO,
                         "bcmdiFlcInfo",
                         &length);
    SHR_NULL_CHECK (flc_drv[unit]->ha_info, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(flc_drv[unit]->ha_info, 0, length);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_DI_COMP_ID,
                                   BCMDI_HA_SUBID_FLC_INFO,
                                   0,
                                   length, 1,
                                   FLC_HA_INFO_T_ID);
    } else {
        flc_info_ha_retrieve(unit);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
flc_get_file(int unit, char *type, const bcmdi_flc_file_t **file)
{
    uint8_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(type, SHR_E_PARAM);
    SHR_NULL_CHECK(file, SHR_E_PARAM);

    if (flc_drv[unit]->file_list != NULL) {
        for (i = 0; i < *flc_drv[unit]->file_count; i++) {
            if (sal_strcmp(type, flc_drv[unit]->file_list[i].type) == 0) {
                *file = flc_drv[unit]->file_list[i].file;
                SHR_EXIT();
            }
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmdi_devicecode_drv_init(int unit, bool warm)
{
    size_t size;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(dvc_drv[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for func ptrs for this unit */
    size = sizeof(bcmdi_dvc_drv_t);
    dvc_drv[unit] = sal_alloc(size, "bcmdiDvcDrv");
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_MEMORY);
    sal_memset(dvc_drv[unit], 0, size);

    SHR_IF_ERR_EXIT(dvc_ha_res_alloc(unit, warm));

    SHR_IF_ERR_EXIT(dvc_drv_attach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);

    /* Release space allocated for func ptrs */
    sal_free(dvc_drv[unit]);
    dvc_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_file_info_get(int unit, const bcmdi_dvc_file_info_t **info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);

    if (dvc_drv[unit]->file != NULL) {
        *info = dvc_drv[unit]->file->info;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_entry_list_get(int unit, const bcmdi_dvc_file_entry_t **entry)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);

    if (dvc_drv[unit]->file != NULL) {
        *entry = dvc_drv[unit]->file->entry_list;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_entry_vals_list_get(int unit, const uint8_t **entry_vals)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);

    if (dvc_drv[unit]->file != NULL) {
        *entry_vals = dvc_drv[unit]->file->entry_vals;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_status_update(int unit, const bcmdi_dvc_file_info_t *file_info, bool loaded)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(&dvc_drv[unit]->status, SHR_E_INIT);

    dvc_drv[unit]->status.loaded = loaded;
    if (loaded) {
        sal_memcpy(&dvc_drv[unit]->status.file_info, file_info,
                   sizeof(bcmdi_dvc_file_info_t));
    }

    SHR_IF_ERR_EXIT
        (dvc_info_ha_store(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_status_get(int unit, bcmdi_dvc_status_t **status) {
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK(&dvc_drv[unit]->status, SHR_E_INIT);

    *status = &dvc_drv[unit]->status;

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_extra(int unit, bool warm) {
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);

    if (dvc_drv[unit]->extra != NULL) {
        if (warm) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Skip extra device initialization (warm-boot)\n")));
            SHR_EXIT();
        }
        dvc_drv[unit]->extra(unit);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Extra device initialization done\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_devicecode_extra_post(int unit, bool warm) {
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dvc_drv[unit], SHR_E_INIT);

    if (dvc_drv[unit]->extra_post != NULL) {
        if (warm) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Skip post-FlexCode device initialization (warm-boot)\n")));
            SHR_EXIT();
        }
        dvc_drv[unit]->extra_post(unit);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Post-FlexCode device initialization done\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_drv_init(int unit, bool warm)
{
    size_t size;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(flc_drv[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for func ptrs for this unit */
    size = sizeof(bcmdi_flc_drv_t);
    flc_drv[unit] = sal_alloc(size, "bcmdiFlcDrv");
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_MEMORY);
    sal_memset(flc_drv[unit], 0, size);

    SHR_IF_ERR_EXIT(flc_ha_res_alloc(unit, warm));

    SHR_IF_ERR_EXIT(flc_drv_attach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);

    /* Release space allocated for func ptrs */
    sal_free(flc_drv[unit]);
    flc_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_is_supported(int unit)
{
    if (flc_drv[unit] == NULL) {
        return SHR_E_INIT;
    } else {
        return flc_drv[unit]->supported;
    }
}


int
bcmdi_flexcode_file_list_get(int unit, uint8_t *file_count, const bcmdi_flc_file_list_t **file_list)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);

    *file_list = flc_drv[unit]->file_list;
    *file_count = *flc_drv[unit]->file_count;

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_file_info_get(int unit, char *type, const bcmdi_flc_file_info_t **info)
{
    const bcmdi_flc_file_t *file;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_EXIT(flc_get_file(unit, type, &file));

    if (file != NULL) {
        *info = file->info;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_entry_list_get(int unit, char *type, const bcmdi_flc_file_entry_t **entry)
{
    const bcmdi_flc_file_t *file;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_EXIT(flc_get_file(unit, type, &file));

    if (file != NULL) {
        *entry = file->entry_list;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_entry_vals_list_get(int unit, char *type, const uint8_t **entry_vals)
{
    const bcmdi_flc_file_t *file;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_EXIT(flc_get_file(unit, type, &file));

    if (file != NULL) {
        *entry_vals = file->entry_vals;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_file_auth_get(int unit, const bcmdi_flc_file_auth_t **auth)
{
    const bcmdi_flc_file_t *file;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_EXIT(flc_get_file(unit, "DEFAULT", &file));

    if (file != NULL) {
        *auth = file->auth;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_status_update(int unit, const bcmdi_flc_file_info_t *file_info, bool loaded)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK (&flc_drv[unit]->status, SHR_E_INIT);

    flc_drv[unit]->status.loaded = loaded;
    if (loaded) {
        sal_memcpy(&flc_drv[unit]->status.file_info, file_info,
                   sizeof(bcmdi_flc_file_info_t));
    }

    SHR_IF_ERR_EXIT
        (flc_info_ha_store(unit));

exit:
    SHR_FUNC_EXIT();
}


int
bcmdi_flexcode_status_get(int unit, bcmdi_flc_status_t **status) {
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flc_drv[unit], SHR_E_INIT);
    SHR_NULL_CHECK (&flc_drv[unit]->status, SHR_E_INIT);

    *status = &flc_drv[unit]->status;

exit:
    SHR_FUNC_EXIT();
}


int
bcmdi_flexcode_pt_index_get(int unit, char *type, bcmdrd_sid_t pt_sid,
                            uint32_t *index_count, const uint32_t **index_list)
{
    const bcmdi_flc_file_t *file = NULL;
    const bcmdi_flc_sid_info_entry_t *pt_sid_info = NULL;
    uint32_t pt_sid_info_count;
    const uint32_t *pt_index_list = NULL;
    int index = -1;

    SHR_FUNC_ENTER(unit);

    /* Get Flexcode file */
    SHR_IF_ERR_EXIT(flc_get_file(unit, type, &file));

    if (file != NULL) {
        pt_sid_info = file->sid_info;
        pt_sid_info_count = file->sid_info_count;
        pt_index_list = file->sid_index_data;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Search for PT id */
    BIN_SEARCH(pt_sid_info, .sid, pt_sid, pt_sid_info_count, index)

    /* Return index data if match found */
    if (index != -1) {
        *index_count = pt_sid_info[index].index_count;
        *index_list = &pt_index_list[pt_sid_info[index].start_index];
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

