/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Single entry point initialization of SDKLT-based application.
 */

#include <soc/cmext.h>
#include <soc/defs.h>

#ifdef BCM_LTSW_SUPPORT

#include <soc/ltsw/drv.h>
#include <soc/ltsw/variant.h>
#include <sal/core/alloc.h>

#include <bsl/bsl.h>
#include <appl/diag/parse.h>
#include <appl/diag/ltsw/ha.h>
#include <appl/diag/ltsw/bslmgmt.h>
#include <appl/diag/ltsw/issu/issumgmt.h>
#include <appl/diag/sysconf_ltsw.h>

#include <bcma/sys/bcma_sys_probe.h>
#include <bcma/sys/bcma_sys_conf_hybrid_sdk.h>
#include <bcmdrd/bcmdrd_dev.h>

/* Maximum length of string parameters from the command line */
#define MAX_STR_PARAM_LEN       255

#define upper_to_lower(c) ((c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c)

static int sysconf_ltsw_probe_done;

static int cmdev2ltdev[SOC_MAX_NUM_DEVICES] = {-1};

static int warmboot = 0;

static int warmexit = 0;

/*
 * Indicates if the HA file need to be saved or compared to previously saved
 * content. The meaning of this field is dependent on the values for
 * keep_ha_file and warm_boot. If both false then the value of this variable
 * has no meaning.
 */
static bool ha_check = false;

/* Store the name of the output file where the HA state will be written to */
static char ha_dump_file[MAX_STR_PARAM_LEN + 1];

static int
str_opt_set(char *str_opt, const char *new_val)
{
    size_t len;

    if (new_val) {
        len = sal_strlen(new_val);
        if (len == 0 || len > MAX_STR_PARAM_LEN) {
            return -1;
        }
        sal_memcpy(str_opt, new_val, len + 1);
    }
    return 0;
}

int
sysconf_ltsw_init(void)
{
    int rv;
    /* Initialize SDKLT CLI stage. */
    rv = appl_ltsw_bslmgmt_redir_hook_set(bcma_sys_conf_hybrid_sdk_cli_redir_bsl);
    if (rv < 0) {
        cli_out("ERROR: BSL redirection hook set fail!\n");
    }
    return bcma_sys_conf_hybrid_sdk_init(BCMA_INIT_STAGE_CLI);
}

int
sysconf_ltsw_probe(void)
{
    int ngdx;
    int ndev;
    int cm_dev;
    bcmdrd_dev_id_t id;

    if (sysconf_ltsw_probe_done) {
        cli_out("sysconf_ltsw_probe: cannot probe more than once\n");
        return -1;
    }

    sal_memset(cmdev2ltdev, -1, sizeof(cmdev2ltdev));

    /* Probe ltsw devices, create device instances in SDKLT DRD component. */
    ndev = bcma_sys_conf_hybrid_sdk_init(BCMA_INIT_STAGE_PROBE);
    if (ndev < 0) {
        cli_out("ERROR: LTSW device probe failed\n");
        return -1;
    }

    /* Iterate over LTSW devices */
    for (ngdx = 0; ngdx < ndev; ngdx++) {

        if (bcmdrd_dev_id_get(ngdx, &id) < 0) {
            cli_out("ERROR: Failed to get device info from DRD (ngdx %d).\n",
                    ngdx);
            continue;
        }

        cli_out("NGBDE unit %d (PCI), "
                "Dev 0x%04x, Rev 0x%02x, Chip %s, Driver LTSW\n",
                ngdx, id.device_id, id.revision,
                soc_cm_device_name_get(id.device_id, id.revision));

        cm_dev = soc_cm_device_create(id.device_id,
                                      id.revision,
                                      NULL);
        if (cm_dev < 0) {
            cli_out("ERROR: Failed to create CM device for NGBDE unit %d\n",
                    ngdx);
            continue;
        }
        /* Keep associated DRD device handle in CM. */
        soc_cm_dev_num_set(cm_dev, ngdx);
        cmdev2ltdev[cm_dev] = ngdx;
        sysconf_ltsw_probe_done++;
    }

    return 0;
}

int
sysconf_ltsw_ha_init(void)
{
    int i, rv;

    rv = appl_ltsw_ha_gen_open(DEFAULT_HA_GEN_FILE_SIZE,
                               1, warmboot);
    if (SHR_FAILURE(rv)) {
        cli_out("HA Generic file initialization failed.\n");
        return rv;
    }

    for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
        if (cmdev2ltdev[i] == -1) {
            continue;
        }
        rv = appl_ltsw_ha_unit_open(cmdev2ltdev[i], DEFAULT_HA_FILE_SIZE,
                                    1, warmboot);
        if (SHR_FAILURE(rv)) {
            cli_out("Unit %d: HA initialization failed.\n", i);
            return rv;
        }
    }

    return 0;
}

int
sysconf_ltsw_ha_deinit(void)
{
    int i, rv;

    for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
        if (cmdev2ltdev[i] == -1) {
            continue;
        }
        rv = appl_ltsw_ha_unit_close(cmdev2ltdev[i], 1, warmexit);
        if (SHR_FAILURE(rv)) {
            cli_out("Unit %d: HA de-initialization failed.\n", i);
        }
    }

    rv = appl_ltsw_ha_gen_close(1, warmexit);
    if (SHR_FAILURE(rv)) {
        cli_out("HA Generic file de-initialization failed.\n");
    }

    return 0;
}

int
sysconf_ltsw_ha_state_dump(void)
{
    int i, rv;

    if (ha_check && warmexit) {
        for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
            if (cmdev2ltdev[i] == -1) {
                continue;
            }
            rv = appl_ltsw_ha_unit_state_dump(cmdev2ltdev[i], ha_dump_file);
            if (SHR_FAILURE(rv)) {
                cli_out("Unit %d: HA state dump failed.\n", i);
            }
        }
    }

    return 0;
}

char *
sysconf_ltsw_ha_state_path(void)
{
    return ha_dump_file;
}

int
sysconf_ltsw_ha_state_comp(void)
{
    int i, rv;

    if (ha_check && warmboot) {
        for (i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
            if (cmdev2ltdev[i] == -1) {
                continue;
            }
            rv = appl_ltsw_ha_unit_state_comp(cmdev2ltdev[i], ha_dump_file);
            if (SHR_FAILURE(rv)) {
                cli_out("Unit %d: HA state comparison failed.\n", i);
            }
        }
    }

    return 0;
}

int
sysconf_ltsw_issu_start_version_set(const char *start_ver)
{
    return issumgmt_start_version_set(start_ver);
}

int
sysconf_ltsw_issu_current_version_set(const char *current_ver)
{
    return issumgmt_current_version_set(current_ver);
}

int
sysconf_ltsw_issu_upgrade_start(void)
{
    int rv = 0;
    if (warmboot) {
        rv = issumgmt_upgrade_start();
    }
    return rv;
}

int
sysconf_ltsw_issu_upgrade_done(void)
{
    int rv = 0;
    if (warmboot) {
        rv = issumgmt_upgrade_done();
    }
    return rv;
}

int
sysconf_ltsw_config_init(void)
{
    int rv = 0;

    rv = soc_cm_config_init();
    if (rv < 0) {
        cli_out("Configuration initialization failed.\n");
    }

    return rv;
}

int
sysconf_ltsw_core_start(void)
{
    if (sysconf_ltsw_probe_done <= 0) {
        return 0;
    }

    if (bcma_sys_conf_hybrid_sdk_init(BCMA_INIT_STAGE_ATTACH) < 0) {
        cli_out("sysconf_ltsw_core_start: bcm ltsw device core start failed.\n");
        return -1;
    }

    return 0;
}

int
sysconf_ltsw_attach(int unit)
{
    soc_cm_device_vectors_t vectors;

    /* HAL for LTSW is handled in DRD in sdklt. LTSW should never use
     * cm function vector, so just leave it with 0. */
    sal_memset(&vectors, 0, sizeof(soc_cm_device_vectors_t));
    if (soc_cm_device_init(unit, &vectors)) {
        cli_out("sysconf_attach: bcm ltsw device init failed\n");
        return -1;
    }
    return 0;
}

int
sysconf_ltsw_detach(int unit)
{
    sysconf_ltsw_dev_env_unset(unit);

    if (sysconf_ltsw_dev_deinit(unit) < 0) {
        cli_out("sysconf_detach: ltsw device detach failed\n");
        return -1;
    }
    if (soc_cm_device_destroy(unit) < 0) {
        cli_out("sysconf_detach: soc_cm_device_destroy failed\n");
        return -1;
    }

    sysconf_ltsw_probe_done--;

    if (!sysconf_ltsw_probe_done) {
        bcma_sys_conf_hybrid_sdk_cleanup(BCMA_INIT_STAGE_ATTACH);
        bcma_sys_conf_hybrid_sdk_cleanup(BCMA_INIT_STAGE_PROBE);
        if (sysconf_ltsw_ha_state_dump() < 0) {
            return -1;
        }
        if (sysconf_ltsw_ha_deinit() < 0) {
            return -1;
        }
        sal_memset(cmdev2ltdev, -1, sizeof(cmdev2ltdev));
    }

    return 0;
}

int
sysconf_ltsw_dev_inited(int unit)
{
    return bcma_sys_conf_hybrid_sdk_attached(cmdev2ltdev[unit]);
}

int
sysconf_ltsw_dev_init(int unit)
{
    if (bcma_sys_conf_hybrid_sdk_attached(cmdev2ltdev[unit])) {
        bcma_sys_conf_hybrid_sdk_detach(cmdev2ltdev[unit]);
    }
    return bcma_sys_conf_hybrid_sdk_attach(cmdev2ltdev[unit]);
}

int
sysconf_ltsw_dev_deinit(int unit)
{
    if (!bcma_sys_conf_hybrid_sdk_attached(cmdev2ltdev[unit])) {
        cli_out("sysconf_ltsw_deinit: Unit %d is already stopped.\n", unit);
        return 0;
    }
    return bcma_sys_conf_hybrid_sdk_detach(cmdev2ltdev[unit]);
}

int
sysconf_ltsw_dev_override(int unit, const char *dev_name)
{
    return bcma_sys_probe_override(unit, dev_name);
}

void
sysconf_ltsw_dev_env_set(int unit)
{
    const char *variant_name = NULL;
    const char *base_drv_name = NULL;
    char *lower_name = NULL;
    char *lower_bd_name = NULL;
    int i;

    (void)soc_ltsw_variant_name_get(unit, &variant_name);

    lower_name = sal_alloc(sal_strlen(variant_name) + 1, "lower_name");
    if (lower_name != NULL) {
        for (i = 0; i < sal_strlen(variant_name); i++) {
            lower_name[i] = upper_to_lower(variant_name[i]);
        }
        lower_name[i] = '\0';
        var_set("variantname", lower_name, FALSE, FALSE);
        sal_free(lower_name);
    }

    (void)soc_ltsw_base_drv_name_get(unit, &base_drv_name);
    lower_bd_name = sal_alloc(sal_strlen(base_drv_name) + 1, "lower_bd_name");
    if (lower_name != NULL) {
        for (i = 0; i < sal_strlen(base_drv_name); i++) {
            lower_bd_name[i] = upper_to_lower(base_drv_name[i]);
        }
        lower_bd_name[i] = '\0';
        var_set("base_drv_name", lower_bd_name, FALSE, FALSE);
        sal_free(lower_bd_name);
    }

    return;
}

void
sysconf_ltsw_dev_env_unset(int unit)
{
    var_unset("variantname", FALSE, TRUE, FALSE);

    return;
}

int
sysconf_ltsw_cleanup(void)
{
    return bcma_sys_conf_hybrid_sdk_cleanup(BCMA_INIT_STAGE_ALL);
}

int
sysconf_ltsw_warmboot_set(int val)
{
    warmboot = val;
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_WARM_BOOT,
                                               val,
                                               NULL);
}

int
sysconf_ltsw_emulation_set(int val)
{
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_EMULATION,
                                               val,
                                               NULL);
}

int
sysconf_ltsw_config_string_set(int val, const char *valstr)
{
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_CONFIG_STRING,
                                               val,
                                               valstr);
}

int
sysconf_ltsw_config_file_set(int val, const char* valstr)
{
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_CONFIG_FILE,
                                               val,
                                               valstr);
}

int
sysconf_ltsw_warmexit_set(int val)
{
    warmexit = val;
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_KEEP_HA_FILE,
                                               val,
                                               NULL);
}

int
sysconf_ltsw_phymod_sim_set(int val)
{
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_PHYMOD_SIM,
                                               val,
                                               NULL);
}

int
sysconf_ltsw_keep_netif_set(int val)
{
    return bcma_sys_conf_hybrid_sdk_option_set(BCMA_SYS_CONF_OPT_KEEP_NETIF,
                                               val,
                                               NULL);
}

int
sysconf_ltsw_ha_check_set(int val, const char *valstr)
{
    ha_check = (val != 0);
    if (ha_check) {
        if (str_opt_set(ha_dump_file, valstr) < 0) {
            return -1;
        }
    }
    return 0;
}

#endif /* BCM_LTSW_SUPPORT */
