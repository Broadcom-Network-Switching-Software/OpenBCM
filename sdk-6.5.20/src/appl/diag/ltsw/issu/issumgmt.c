/*! \file issumgmt.c
 *
 * The ISSU management interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/switch.h>

#include <shr/shr_debug.h>
#include <bcmissu/issu_api.h>

/******************************************************************************
 * Local definitions
 */

/*! The unknown version. */
#define VERSION_UNKNOWN "unknown"

/*! The start SDK version for ISSU. */
#ifndef SDK_VER_START
#define SDK_VER_START VERSION_UNKNOWN
#endif /* SDK_VER_START */

/*! The start SDKLT version for ISSU. */
#ifndef SDKLT_VER_START
#define SDKLT_VER_START VERSION_UNKNOWN
#endif /* SDKLT_VER_START */

/*! The SDK version for ISSU. */
#ifndef SDK_VER
#define SDK_VER VERSION_UNKNOWN
#endif /* SDK_VER */
/*! The SDKLT version for ISSU. */
#ifndef SDKLT_VER
#define SDKLT_VER VERSION_UNKNOWN
#endif /* SDKLT_VER */

#define SDK_VER_STRINGS_LEN_MAX   64

typedef struct issu_ver_s {
    char start_ver[SDK_VER_STRINGS_LEN_MAX];
    char current_ver[SDK_VER_STRINGS_LEN_MAX];
} issu_ver_t;

typedef struct issu_info_s {
    issu_ver_t sdk;
    issu_ver_t sdklt;
} issu_info_t;

static issu_info_t issu_info = {
    {SDK_VER_START, SDK_VER},
    {SDKLT_VER_START, SDKLT_VER}
};

/******************************************************************************
 * Private functions
 */
static int
issumgmt_version_set(const char *version, char *sdk_ver, char *sdklt_ver)
{
    char *separator = NULL;

    if (sal_strlen(version) >= SDK_VER_STRINGS_LEN_MAX) {
        return -1;
    }

    separator = sal_strchr(version, ':');

    if (separator) {
        sal_strncpy(sdk_ver, version, separator - version);
        sdk_ver[separator - version] = '\0';
        sal_strncpy(sdklt_ver, separator + 1, SDK_VER_STRINGS_LEN_MAX);
    } else {
        sal_strncpy(sdk_ver,
                    version, SDK_VER_STRINGS_LEN_MAX);
        sal_strncpy(sdklt_ver,
                    version, SDK_VER_STRINGS_LEN_MAX);
    }

    return 0;
}

/******************************************************************************
 * Public Functions
 */
int
issumgmt_start_version_set(const char *start_ver)
{
    return issumgmt_version_set(start_ver,
                                issu_info.sdk.start_ver,
                                issu_info.sdklt.start_ver);
}

int
issumgmt_current_version_set(const char *current_ver)
{
    return issumgmt_version_set(current_ver,
                                issu_info.sdk.current_ver,
                                issu_info.sdklt.current_ver);
}

int
issumgmt_upgrade_start(void)
{
    int rv = 0;
    bool skip_sdk = false, skip_sdklt = false;
    issu_ver_t *sdk, *sdklt;
    issu_info_t *issuinfo = &issu_info;

    sdk = &issuinfo->sdk;
    sdklt = &issuinfo->sdklt;

    /* Skip version unknown */
    if (sal_strcmp(sdk->start_ver, VERSION_UNKNOWN) == 0 ||
        sal_strcmp(sdk->current_ver, VERSION_UNKNOWN) == 0) {
        skip_sdk = true;
    }

    /* Skip ISSU while the from version is same to or larger than the to version */
    if (sal_strcmp(sdk->start_ver, sdk->current_ver) >= 0) {
        skip_sdk = true;
    }

    if (!skip_sdk) {
        rv = bcm_switch_issu_upgrade_start(sdk->start_ver, sdk->current_ver);
        if (rv < 0) {
            return rv;
        }
        cli_out("SDK ISSU upgrade: <%s -> %s>\n",
                sdk->start_ver, sdk->current_ver);
    }

    /* Skip version unknown */
    if (sal_strcmp(sdklt->start_ver, VERSION_UNKNOWN) == 0 ||
        sal_strcmp(sdklt->current_ver, VERSION_UNKNOWN) == 0) {
        skip_sdklt = true;
    }

    /* Skip ISSU while the from version is same to or larger than the to version */
    if (sal_strcmp(sdklt->start_ver, sdklt->current_ver) >= 0) {
        skip_sdklt = true;
    }

    if (!skip_sdklt) {
        rv = bcmissu_upgrade_start(sdklt->start_ver, sdklt->current_ver);
        if (rv < 0) {
            return rv;
        }
        cli_out("SDKLT ISSU upgrade: <%s -> %s>\n",
                sdklt->start_ver, sdklt->current_ver);
    }

    return rv;
}

int
issumgmt_upgrade_done(void)
{
    int rv = 0;

    rv = bcmissu_upgrade_done();
    if (rv < 0) {
        return rv;
    }

    rv = bcm_switch_issu_upgrade_done();
    if (rv < 0) {
        return rv;
    }

    return rv;
}

