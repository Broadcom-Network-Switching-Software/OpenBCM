/*! \file bcmmgmt_core.c
 *
 * SDK core start and stop APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_hash_str.h>
#include <shr/shr_fmm.h>
#include <shr/shr_ha.h>
#include <shr/shr_lmem_mgr.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcfg/bcmcfg.h>

#include <bcmha/bcmha_mem.h>

#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmmgmt/bcmmgmt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Local definitions
 */

static bool core_init = false;

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_core_init(void)
{
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("Initializing core utilities\n")));

    if (core_init) {
        return SHR_E_NONE;
    }

    if (shr_lmm_global_init() != SHR_E_NONE) {
        return SHR_E_FAIL;
    }

    if (shr_hash_str_init() < 0) {
        return SHR_E_FAIL;
    }

    if (shr_fmm_init() != SHR_E_NONE) {
        return SHR_E_FAIL;
    }

    if (shr_famm_init() != SHR_E_NONE) {
        return SHR_E_FAIL;
    }

    core_init = true;

    return SHR_E_NONE;
}

int
bcmmgmt_core_cleanup(void)
{
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("Cleaning up core utilities\n")));

    shr_hash_str_delete();

    shr_fmm_delete();

    shr_famm_cleanup();

    if (shr_lmm_global_shutdown() != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Not all local memory manager instances were "
                           "freed\n")));
    }
    core_init = false;

    /* Clean list of callbacks */
    bcmmgmt_shutdown_cb_unregister(NULL);

    return SHR_E_NONE;
}

int
bcmmgmt_core_config_load(const char *conf_file)
{
    int rv;

    if (!core_init) {
        return SHR_E_INIT;
    }
    if (conf_file == NULL || *conf_file == '\0') {
        return SHR_E_NONE;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("Loading configuration file %s\n"), conf_file));

    rv = bcmcfg_file_parse(conf_file);
    if (SHR_FAILURE(rv)) {
        switch (rv) {
        case SHR_E_UNAVAIL:
            /* Warning for YAML parser unavailable */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Feature not support for configuration file "
                               "%s\n"), conf_file));
            break;
        default:
            return SHR_E_CONFIG;
        }
    }

    return SHR_E_NONE;
}

int
bcmmgmt_core_config_parse(const char *yml_str)
{
    int rv;

    if (!core_init) {
        return SHR_E_INIT;
    }
    if (yml_str == NULL || *yml_str == '\0') {
        return SHR_E_NONE;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("Parsing configuration string (%u characters)\n"),
              (unsigned int)sal_strlen(yml_str)));

    rv = bcmcfg_string_parse(yml_str);
    if (SHR_FAILURE(rv)) {
        return SHR_E_CONFIG;
    }

    return SHR_E_NONE;
}

int
bcmmgmt_core_start(bool warm, const bcmmgmt_comp_list_t *comp_list)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("Starting System Manager (%s-start)\n"),
              warm ? "warm" : "cold"));

    if (!core_init) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmha_mem_init(BCMHA_GEN_UNIT, NULL, NULL, NULL, NULL, 0));

    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmha_mem_reset(BCMHA_GEN_UNIT));
    }

    SHR_IF_ERR_EXIT
        (bcmmgmt_sysm_components_start(warm, comp_list));

    SHR_IF_ERR_EXIT
        (bcmmgmt_pkt_core_init());

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmha_mem_cleanup(BCMHA_GEN_UNIT);
    }
    SHR_FUNC_EXIT();
}

int
bcmmgmt_core_internal_stop(bool graceful, bool keep_netif)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("Stopping System Manager (%sgraceful)\n"),
              graceful ? "" : "non-"));

    /* Notify any registered callbacks */
    bcmmgmt_shutdown_cb_execute(-1);

    if (!keep_netif) {
        SHR_IF_ERR_CONT
            (bcmmgmt_pkt_core_cleanup());
    }

    SHR_IF_ERR_CONT
        (bcmmgmt_sysm_components_stop(graceful));

    SHR_IF_ERR_CONT
        (bcmha_mem_cleanup(BCMHA_GEN_UNIT));

    SHR_FUNC_EXIT();
}

int
bcmmgmt_core_stop(bool graceful)
{
    return bcmmgmt_core_internal_stop(graceful, false);
}
