/*! \file bcmcfg_ha_read_data.c
 *
 * BCMCFG HA(High Availability) related functions for reader data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_ha_internal.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/generated/bcmcfg_ha_config.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_HA

/*******************************************************************************
 * Private variables
 */

static bcmcfg_ha_alloc_info_t bcmcfg_ha_read_data;

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Get reader data.
 *
 * \param [out]   read       Reader data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_ha_read_data_get(const bcmcfg_read_data_t **read)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmcfg_read_data_get(read));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcfg_ha_read_data_init(bool warm)
{
    const bcmcfg_read_data_t *read = NULL;
    const char *str = NULL;
    uint32_t req_size = 0;
    uint8_t *alloc_ptr = NULL;
    bcmcfg_ha_header_t *hdr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmcfg_ha_read_data_get(&read));
    if (!warm) {
        while (read) {
            str = read->data;
            if (str) {
                /* Read data size in HA
                 * Length of string: uint32_t.
                 * Raw string data: strlen string.
                 * Nul termination: 1.
                 */
                req_size += sizeof(uint32_t);
                req_size += sal_strlen(str);
                req_size += 1;
            }
            read = read->next;
        }
    } else {
        if (read) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Ignoring configuration in warm boot\n")));
            while (read) {
                str = read->data;
                if (str) {
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META("%s\n"), read->fpath));
                }
                read = read->next;
            }

            SHR_IF_ERR_EXIT(
                bcmcfg_read_data_cleanup());
        }
        req_size = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_ha_gen_alloc(warm, req_size,
                             "BCMCFG_READ_DATA",
                             BCMCFG_HA_SUBID_READ_DATA,
                             "bcmcfgYamlCfg",
                             BCMCFG_HA_SIGN_READ_DATA,
                             &alloc_ptr));
    hdr = (bcmcfg_ha_header_t *) alloc_ptr;
    bcmcfg_ha_read_data.alloc_ptr = alloc_ptr;
    bcmcfg_ha_read_data.size = hdr->size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_read_data_save(bool warm)
{
    const bcmcfg_read_data_t *read = NULL;
    uint8_t *ha_ptr = bcmcfg_ha_read_data.alloc_ptr;
    uint32_t size = bcmcfg_ha_read_data.size;
    uint32_t str_size;
    uint32_t len;
    const char *str = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (size == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_ha_read_data_get(&read));

    ha_ptr += sizeof(bcmcfg_ha_header_t);
    while (read) {
        str = read->data;
        if (str == NULL) {
            read = read->next;
            continue;
        }

        len = sal_strlen(str);

        /* Length */
        str_size = len + 1;

        sal_memcpy(ha_ptr, &str_size, sizeof(uint32_t));

        ha_ptr += sizeof(uint32_t);

        sal_memcpy(ha_ptr, str, len);

        ha_ptr += len;

        *ha_ptr = '\0';

        ha_ptr += 1;

        read = read->next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_read_data_restore(bool warm)
{
    uint8_t *ha_ptr = bcmcfg_ha_read_data.alloc_ptr;
    uint32_t size = bcmcfg_ha_read_data.size;
    uint32_t rsize = 0;
    uint32_t len;
    char *str = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (size == 0) {
        SHR_EXIT();
    }

    ha_ptr += sizeof(bcmcfg_ha_header_t);

    while (size > rsize) {
        sal_memcpy(&len, &ha_ptr[0], sizeof(uint32_t));
        rsize += sizeof(uint32_t);
        ha_ptr += sizeof(uint32_t);

        SHR_ALLOC(str, len, "bcmcfgReadDataRecovery");
        SHR_NULL_CHECK(str, SHR_E_MEMORY);
        sal_memcpy(str, ha_ptr, len);

        rsize += len;
        ha_ptr += len;

        /* Ignore return code, continue */
        (void) bcmcfg_string_parse(str);

        SHR_FREE(str);
    }

exit:
    SHR_FUNC_EXIT();
}

