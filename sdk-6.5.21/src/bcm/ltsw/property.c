/*! \file property.c
 *
 * Configuration property inferface.
 * This file contains the stub interfaces of configuration property.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/property.h>
#include <soc/ltsw/config.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* HA signature for property. */
#define PROPERTY_HA_SIGNATURE (0xCFAA0001)

/* brief Property information. */
typedef struct property_info_s {
    /* HA buffer. */
    void *ha_buffer;
} property_info_t;

static property_info_t property_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

#define PROPERTY_HA_SUB_COMP_ID_DEF 0

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public internal HSDK functions
 */

char *
bcmi_ltsw_property_str_get(int unit, const char *name)
{
    return soc_ltsw_config_str_get(bcmi_ltsw_dev_dunit(unit), name);
}

uint32_t
bcmi_ltsw_property_get(int unit, const char *name, uint32_t def)
{
    uint32_t val = 0;
    char *end = NULL;
    char *str = NULL;

    if ((str = bcmi_ltsw_property_str_get(unit, name)) == NULL) {
        return def;
    }

    val = sal_strtoul(str, &end, 0);
    if (*str && *end) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Unable to convert %s\n"), str));
        val = def;
    }

    return val;
}

char *
bcmi_ltsw_property_port_str_get(int unit,
                                int port,
                                const char *name)
{
    return soc_ltsw_config_port_str_get(bcmi_ltsw_dev_dunit(unit), port, name);
}

uint32_t
bcmi_ltsw_property_port_get(int unit, int port,
                            const char *name,
                            uint32_t def)
{
    uint32_t val = 0;
    char *end = NULL;
    char *str = NULL;

    if ((str = bcmi_ltsw_property_port_str_get(unit, port, name)) == NULL) {
        return def;
    }

    val = sal_strtoul(str, &end, 0);
    if (*str && *end) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Unable to convert %s\n"), str));
        val = def;
    }

    return val;
}

int
bcmi_ltsw_property_init(int unit)
{
    property_info_t *pi = &property_info[unit];
    int size = 0;
    uint32_t alloc_size = 0;
    uint32_t sign = 0;
    uint32_t *data_buffer= NULL;
    int dunit;
    int wb;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    wb = bcmi_warmboot_get(unit);

    if (!wb) {
        SHR_IF_ERR_EXIT
             (soc_ltsw_config_ha_size_get(dunit, &size));

        /* 2 extra word for signature and buffer length. */
        alloc_size = size + sizeof(uint32_t) * 2;
    }

    /* Allocate HA memory. */
    pi->ha_buffer = bcmi_ltsw_ha_mem_alloc(unit,
                                           BCMI_HA_COMP_ID_PROPERTY,
                                           PROPERTY_HA_SUB_COMP_ID_DEF,
                                           "bcmPropertyDef",
                                           &alloc_size);

    SHR_NULL_CHECK(pi->ha_buffer, SHR_E_MEMORY);
    if (wb) {
        sign = *((uint32_t *)(pi->ha_buffer));
        SHR_IF_ERR_MSG_EXIT
            (sign != PROPERTY_HA_SIGNATURE ? SHR_E_FAIL : SHR_E_NONE,
             (BSL_META_U(unit, "WB: signature mismatch, "
                         "exp=0x%8x, obs=0x%8x\n"),
                         PROPERTY_HA_SIGNATURE, sign));

        size = *((uint32_t *)(pi->ha_buffer) + 1);
        data_buffer = (uint32_t *)(pi->ha_buffer) + 2;

        /* Restore property data from HA memory. */
        SHR_IF_ERR_EXIT
            (soc_ltsw_config_ha_restore(dunit,
                                        (void *)data_buffer,
                                        (int)size));
    } else {
        sal_memset(pi->ha_buffer, 0, alloc_size);
        *((uint32_t *)(pi->ha_buffer)) = PROPERTY_HA_SIGNATURE;
        *((uint32_t *)(pi->ha_buffer) + 1) = size;
        data_buffer = (uint32_t *)(pi->ha_buffer) + 2;

        /* Save property data into HA memory. */
        SHR_IF_ERR_EXIT
            (soc_ltsw_config_ha_save(dunit,
                                     (void *)data_buffer,
                                     (int)size));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!wb && pi->ha_buffer) {
            (void) bcmi_ltsw_ha_mem_free(unit, pi->ha_buffer);
            pi->ha_buffer = NULL;
        }
    }
    SHR_FUNC_EXIT();

}
