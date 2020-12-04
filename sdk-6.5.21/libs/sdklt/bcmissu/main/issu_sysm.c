/*! \file issu_sysm.c
 *
 * Broadcom System manager API
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_dl.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm_state_mon.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmissu/bcmissu_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_CTRL

typedef struct {
    sal_dl_t dl_handle;
    bcmissu_version_udt_f ver_upgrade;
    bcmissu_data_resize_f data_resize;
    bcmissu_data_upgrade_f data_upgrade;
    bcmissu_data_upgrade_f comp_upgrade;
    bcmissu_ltid_to_current_f ltid_to_current;
    bcmissu_fid_to_current_f fid_to_current;
    sal_dl_t native_dl_handle;
    bcmissu_version_udt_f native_ver_upgrade;
    bcmissu_data_resize_f native_data_resize;
    bcmissu_data_upgrade_f native_data_upgrade;
    bcmissu_dll_enum_changed_f enum_changed;
    bcmissu_dll_enum_val_to_current_f enum_upgrade;
    bcmissu_dll_lt_has_changed_f has_lt_changed;
} issu_dll_export_t;

typedef void (*orphan)(void);

static issu_dll_export_t dlls_info;

static bool issu_active = false;

static sal_mutex_t global_unit_sync;

/* This array of functions forces the linker to link these functions in */
const orphan bcmlrd_needed_funcs[] = {
    (orphan)bcmlrd_table_field_name_to_idx
};

/*******************************************************************************
 * Private functions
 */
static int connect_to_issu_dll(const char *dll_name, bool native)
{
    sal_dl_t dl_handle;
    void *sym;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* Start by connecting to the struct DB dynamic library */
    dl_handle = sal_dl_open(dll_name);
    if (!dl_handle) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Failed loading ISSU DLL Error\n")));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (native) {
        dlls_info.native_dl_handle = dl_handle;
    } else {
        dlls_info.dl_handle = dl_handle;
    }

    sym = sal_dl_sym(dl_handle, "bcmissu_version_update");
    SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
    if (native) {
        dlls_info.native_ver_upgrade = (bcmissu_version_udt_f)(uintptr_t)sym;
    } else {
        dlls_info.ver_upgrade = (bcmissu_version_udt_f)(uintptr_t)sym;
    }

    sym = sal_dl_sym(dl_handle, "bcmissu_data_resize");
    SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
    if (native) {
        dlls_info.native_data_resize = (bcmissu_data_resize_f)(uintptr_t)sym;
    } else {
        dlls_info.data_resize = (bcmissu_data_resize_f)(uintptr_t)sym;
    }

    sym = sal_dl_sym(dl_handle, "bcmissu_data_upgrade");
    SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
    if (native) {
        dlls_info.native_data_upgrade = (bcmissu_data_upgrade_f)(uintptr_t)sym;
    } else {
        dlls_info.data_upgrade = (bcmissu_data_upgrade_f)(uintptr_t)sym;
    }

    if (!native) {
        sym = sal_dl_sym(dl_handle, "bcmissu_comp_upgrade");
        SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
        dlls_info.comp_upgrade = (bcmissu_data_upgrade_f)(uintptr_t)sym;

        sym = sal_dl_sym(dl_handle, "bcmissu_dll_ltid_to_current");
        SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
        dlls_info.ltid_to_current = (bcmissu_ltid_to_current_f)(uintptr_t)sym;

        sym = sal_dl_sym(dl_handle, "bcmissu_dll_fid_to_current");
        SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
        dlls_info.fid_to_current = (bcmissu_fid_to_current_f)(uintptr_t)sym;

        sym = sal_dl_sym(dl_handle, "bcmissu_dll_enum_has_changed");
        SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
        dlls_info.enum_changed = (bcmissu_dll_enum_changed_f)(uintptr_t)sym;

        sym = sal_dl_sym(dl_handle, "bcmissu_dll_enum_val_to_current");
        SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
        dlls_info.enum_upgrade = (bcmissu_dll_enum_val_to_current_f)(uintptr_t)sym;

        sym = sal_dl_sym(dl_handle, "bcmissu_dll_lt_has_changed");
        SHR_NULL_CHECK(sym, SHR_E_INTERNAL);
        dlls_info.has_lt_changed = (bcmissu_dll_lt_has_changed_f)(uintptr_t)sym;
    }

exit:
    SHR_FUNC_EXIT();
}

static void sysm_state_monitor(int unit,
                               shr_sysm_states_t state,
                               void *context)
{
    if (state == SHR_SYSM_INIT) {
        if (unit >= 0) {
            bcmissu_struct_init(unit, BCMMGMT_ISSU_COMP_ID, true);
            dlls_info.data_resize(unit);
        } else {
            bcmissu_struct_init(BCMISSU_NO_UNIT, BCMMGMT_ISSU_COMP_ID, true);
            dlls_info.data_resize(BCMISSU_NO_UNIT);
        }
    } else if (state == SHR_SYSM_COMP_CONFIG) {
        if (unit >= 0) {
            dlls_info.data_upgrade(unit);
        } else {
            dlls_info.data_upgrade(BCMISSU_NO_UNIT);
        }
    } else if (state == SHR_SYSM_PRE_CONFIG) {
        if (unit >= 0) {
            dlls_info.comp_upgrade(unit);
        } else {
            dlls_info.comp_upgrade(BCMISSU_NO_UNIT);
        }
    }
}

static int detached_from_dll(bool native) {
    sal_dl_t dl_handle;

    if (native) {
        dlls_info.native_data_resize = NULL;
        dlls_info.native_data_upgrade = NULL;
        dlls_info.native_ver_upgrade = NULL;
        dl_handle = dlls_info.native_dl_handle;
        dlls_info.native_dl_handle = NULL;
    } else {
        dlls_info.ver_upgrade = NULL;
        dlls_info.ltid_to_current = NULL;
        dlls_info.fid_to_current = NULL;
        dlls_info.enum_changed = NULL;
        dlls_info.enum_upgrade = NULL;
        dl_handle = dlls_info.dl_handle;
        dlls_info.dl_handle = NULL;

        /* Unregister system state monitoring */
        shr_sysm_state_monitor_unregister(sysm_state_monitor);
    }

    /* If DLL is not loaded simply return okay. */
    if (!dl_handle) {
        return SHR_E_NONE;
    }
    if (sal_dl_close(dl_handle) != 0) {
        return SHR_E_FAIL;
    }
    return SHR_E_NONE;
}

static shr_sysm_rv_t gen_init(shr_sysm_categories_t instance_category,
                              int unit,
                              void *comp_data,
                              bool warm,
                              uint32_t *blocking_component)
{
    /* Create unit syncronization mutex */
    global_unit_sync = sal_mutex_create("bcmissuUnitSync");
    if (!global_unit_sync) {
        return SHR_SYSM_RV_ERROR;
    }
    if (bcmissu_struct_init(BCMISSU_NO_UNIT,
                            BCMMGMT_ISSU_COMP_ID, warm) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t gen_shutdown(shr_sysm_categories_t instance_category,
                                   int unit,
                                   void *comp_data,
                                   bool graceful,
                                   uint32_t *blocking_component)
{
    if (bcmissu_struct_shutdown(BCMISSU_NO_UNIT, graceful) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    bcmissu_struct_cleanup();

    if (global_unit_sync) {
        sal_mutex_destroy(global_unit_sync);
        global_unit_sync = NULL;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t unit_init(shr_sysm_categories_t instance_category,
                               int unit,
                               void *comp_data,
                               bool warm,
                               uint32_t *blocking_component)
{
    if (bcmissu_struct_init(unit, BCMMGMT_ISSU_COMP_ID, warm) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t unit_shutdown(shr_sysm_categories_t instance_category,
                                   int unit,
                                   void *comp_data,
                                   bool graceful,
                                   uint32_t *blocking_component)
{
    if (bcmissu_struct_shutdown(unit, graceful) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*******************************************************************************
 * Public functions - internal to ISSU
 */
sal_mutex_t bcmissu_unit_sync_obj_get()
{
    return global_unit_sync;
}

bool bcmissu_is_active()
{
    return issu_active;
}

/*******************************************************************************
 * ISSU related public functions
 */
int bcmissu_start(void)
{
    shr_sysm_cb_func_set_t funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = gen_init;
    funcs.shutdown = gen_shutdown;
    SHR_IF_ERR_EXIT(shr_sysm_register(BCMMGMT_ISSU_COMP_ID,
                                      SHR_SYSM_CAT_GENERIC,
                                      &funcs,
                                      NULL));

    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = unit_init;
    funcs.shutdown = unit_shutdown;
    SHR_IF_ERR_EXIT(shr_sysm_register(BCMMGMT_ISSU_COMP_ID,
                                      SHR_SYSM_CAT_UNIT,
                                      &funcs,
                                      NULL));
exit:
    SHR_FUNC_EXIT();
}

int bcmissu_upgrade_start(const char *from_ver,  const char *to_ver)
{
    int rv;
    char lib_name[80];
    int len;

    len = sal_snprintf(lib_name, sizeof(lib_name), "libsdkissu.so.%s", to_ver);
    if ((uint32_t)len >= sizeof(lib_name)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Invalid version %s\n"), to_ver));
        return SHR_E_PARAM;
    }

    rv = connect_to_issu_dll(lib_name, false);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    shr_sysm_state_monitor_register(sysm_state_monitor, NULL);
    rv = dlls_info.ver_upgrade(from_ver, to_ver, false);
    if (SHR_SUCCESS(rv)) {
        issu_active = true;
    }
    return rv;
}

int bcmissu_upgrade_done()
{
    issu_active = false;
    return detached_from_dll(false);
}

int bcmissu_ltid_to_current(int unit,
                            uint32_t old_ltid,
                            uint32_t *current_ltid)
{
    if (dlls_info.ltid_to_current) {
        return dlls_info.ltid_to_current(unit, old_ltid, current_ltid);
    } else {
        return SHR_E_INIT;
    }
}

int bcmissu_fid_to_current(int unit,
                           uint32_t old_ltid,
                           uint32_t old_flid,
                           uint32_t *current_flid)
{
    if (dlls_info.fid_to_current) {
        return dlls_info.fid_to_current(unit, old_ltid, old_flid, current_flid);
    } else {
        return SHR_E_INIT;
    }
}

bool bcmissu_enum_has_changed(int unit, const char *enum_type_name)
{
    if (dlls_info.enum_changed) {
        return dlls_info.enum_changed(unit, enum_type_name);
    } else {
        return false;
    }
}

int bcmissu_enum_sym_to_current(
        int unit,
        const char *enum_type_name,
        uint32_t old_val,
        uint32_t *current_val)
{
    if (dlls_info.enum_upgrade) {
        return dlls_info.enum_upgrade(unit, enum_type_name, old_val, current_val);
    } else {
        return SHR_E_INIT;
    }
}

bool bcmissu_lt_has_changed(int unit, uint32_t ltid)
{
    if (dlls_info.has_lt_changed) {
        return dlls_info.has_lt_changed(unit, ltid);
    } else {
        return SHR_E_INIT;
    }
}

int bcmissu_native_unit_init(int unit, shr_ha_mod_id comp_id, bool warm)
{
    return bcmissu_struct_init(unit, comp_id, warm);
}

int bcmissu_native_unit_shutdown(int unit, bool graceful)
{
    int rv;

    rv = bcmissu_validate_unit(unit);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    return bcmissu_struct_shutdown(unit, graceful);
}

int bcmissu_native_upgrade_start(const char *from_ver,
                                 const char *to_ver,
                                 const char *dll_name)
{
    int rv;

    rv = connect_to_issu_dll(dll_name, true);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    return dlls_info.native_ver_upgrade(from_ver, to_ver, true);
}

int bcmissu_native_unit_upgrade(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmissu_validate_unit(unit));

    SHR_IF_ERR_EXIT(dlls_info.native_data_resize(unit));
    SHR_IF_ERR_EXIT(dlls_info.native_data_upgrade(unit));

exit:
    SHR_FUNC_EXIT();
}

int bcmissu_native_upgrade_done(void)
{
    return detached_from_dll(true);
}

