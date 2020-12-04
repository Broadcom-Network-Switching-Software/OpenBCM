/*! \file bcmbd_fwm_fpkg.c
 *
 * Functions of firmware pacakge management of Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_util.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_fwm.h>
#include "./bcmbd_fwm_internal.h"

#ifdef YAML
/* External OSS dependency */
#include <yaml.h>
#endif

/* Log source for this component. */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_FWM

/*******************************************************************************
 * Local definitions
 */
typedef struct bcmbd_fwm_fpkg_s {
    /* Handler of fpkg. */
    int fpkg_id;

    /* Fpkg info. */
    bcmbd_fwm_fwinfo_t *fpkg_info;
} bcmbd_fwm_fpkg_t;

static bcmbd_fwm_fpkg_t fw_package[BCMBD_FWM_FPKG_NUM_MAX];
static uint8_t fw_package_init = 0;
/* Protect FWM firmware package resource. */
static sal_mutex_t fpkg_mutex = NULL;

/* Firmware instance database. */
static bcmbd_fwm_fw_instances_t *bcmbd_fwm_fw_inst_db[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Firmware package internal format definitions
 */
#define FPKG_FORMAT_MAGIC_STR               "BCMFWPKG"
#define FPKG_FORMAT_MAGIC_LEN               8
#define FPKG_FORMAT_VERSION_LEN             2
#define FPKG_FORMAT_FILE_COUNT_LEN          2
#define FPKG_FORMAT_FILE_TYPE_LEN           4
#define FPKG_FORMAT_FILE_TYPE_INFO_STR      "INFO"
#define FPKG_FORMAT_FILE_TYPE_IMAG_STR      "IMAG"
#define FPKG_FORMAT_FILE_TYPE_SIGN_STR      "SIGN"
#define FPKG_FORMAT_FILE_INFO_LEN           8
#define FPKG_FORMAT_FILE_SIZE_LEN           8
#define FPKG_FORMAT_CHECKSUM_LEN            4

/*
 * YAML keywords
 */
#define FPKG_YAML_KEY_FW_TYPE               "firmware-type"
#define FPKG_YAML_KEY_FW_VER                "firmware-version"
#define FPKG_YAML_KEY_CHIPS                 "supported-chips"
#define FPKG_YAML_KEY_DEF_PROC              "default-processor"
#define FPKG_YAML_KEY_DEF_ADDR              "default-address"
#define FPKG_YAML_KEY_PROPS                 "properties"

/*******************************************************************************
 * Internal functions
 */
/*!
 * \brief Calculate checksum from a byte buffer.
 *
 * \param [in] buffer Buffer memory address.
 * \param [in] size Buffer size.
 *
 * \retval Checksum value.
 */
static uint16_t
calc_checksum(const uint8_t *buffer, int size)
{
    int i, checksum = 0;

    if (buffer == NULL || size <= 0) {
        return 0;
    }

    for (i = 0; i < size; i++) {
        checksum = (checksum >> 1) + ((checksum & 1) << 15);
        checksum += buffer[i];
        checksum &= 0xffff;
    }

    return (uint16_t)checksum;
}

#ifdef YAML

/*!
 * \brief Trim trailing spaces on a string.
 *
 * \param [in] str String buffer address.
 * \param [in] slen String length (null terminator is not included).
 *
 * \retval None.
 */
static void
trim_trailing_spaces(char *str, int slen)
{
    if (str == NULL || slen <= 0) {
        return;
    }

    for (; slen > 0; slen--) {
        if (str[slen - 1] == ' ') {
            str[slen - 1] = '\0';
        } else {
            return;
        }
    }
}

/*!
 * \brief Parse YAML content for properties.
 *
 * \param [in] parser YAML Parser object.
 * \param [out] info Firmware package information.
 *
 * \retval SHR_E_NONE No error.
 */
static int
yaml_get_properties(yaml_parser_t *parser, bcmbd_fwm_fwinfo_t *info)
{
    yaml_event_t event;
    bool event_created = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(parser, SHR_E_INTERNAL);
    SHR_NULL_CHECK(info, SHR_E_INTERNAL);

    /* Get the next YAML item */
    if (!yaml_parser_parse(parser, &event)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid YAML value format for properties.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    event_created = true;

    /* The next YAML item should either be a (empty) scalar or mapping */
    if (event.type != YAML_SCALAR_EVENT &&
        event.type != YAML_MAPPING_START_EVENT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Unsupported YAML format for properties.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (event.type == YAML_SCALAR_EVENT) {
        /* Scalar: should be an empty string (no properties) */
        char *val = (char *)event.data.scalar.value;
        if (val == NULL || val[0] != '\0') {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid YAML format for properties.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_EXIT();
    }

    /* else - YAML_MAPPING_START_EVENT: some properties are defined */
    yaml_event_delete(&event);
    event_created = false;
    for(;;) {
        /* Get the next YAML item */
        if (!yaml_parser_parse(parser, &event)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid YAML format for properties.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        event_created = true;
        if (event.type == YAML_SCALAR_EVENT) {
            /* Got property key. For now, it's not stored */
            yaml_event_delete(&event);
            event_created = false;
            /* Get property value */
            if (!yaml_parser_parse(parser, &event)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Invalid YAML for properties.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            event_created = true;
            if (event.type != YAML_SCALAR_EVENT) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Invalid YAML for properties.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* For now, we simply ignore these properties */
            yaml_event_delete(&event);
            event_created = false;
        } else if (event.type == YAML_MAPPING_END_EVENT) {
            /* Done with the properties */
            break;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid YAML value format for chips.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    if (event_created) {
        /* Destroy the event object. */
        yaml_event_delete(&event);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get value of a map item as a number.
 *
 * \param [in] parser YAML Parser object.
 * \param [out] info Firmware package information.
 *
 * \retval SHR_E_NONE No error.
 */
static int
yaml_get_chips(yaml_parser_t *parser, bcmbd_fwm_fwinfo_t *info)
{
    yaml_event_t event;
    bool event_created = false;
    int count = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(parser, SHR_E_INTERNAL);
    SHR_NULL_CHECK(info, SHR_E_INTERNAL);

    /* Get the next YAML item */
    if (!yaml_parser_parse(parser, &event)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid YAML value format for chips.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    event_created = true;

    /* The next YAML item should be a list (sequence) */
    if (event.type != YAML_SEQUENCE_START_EVENT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Unsupported YAML value format for chips.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    yaml_event_delete(&event);
    event_created = false;

    /* Traverse the list */
    for(;;) {
        /* Get the next YAML item */
        if (!yaml_parser_parse(parser, &event)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid YAML value format for chips.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        event_created = true;
        if (event.type == YAML_SCALAR_EVENT) {
            /* Got a supported chip */
            uint32_t id;
            int len;
            char *pstr;
            char *chip = (char *)event.data.scalar.value;
            if (chip == NULL) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "YAML parsing failed.\n")));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            /* Chip ID can be with or without the BCM prefix */
            if (!sal_memcmp(chip, "BCM", 3) || !sal_memcmp(chip, "bcm", 3)) {
                chip += 3;
            }
            len = sal_strlen(chip);
            /*
             * We want to convert to SDK device ID, so we treat the
             * number as heximal. For example, "BCM56000" will be
             * parsed as 0x56000.
             */
            id = (uint32_t)sal_strtoul(chip, &pstr, 16);
            if (pstr != &chip[len]) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Invalid YAML format for chips.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            if ((id & 0xFFFF0000UL) != 0x50000UL) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Invalid chip ID in YAML.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* Convert to SDK device ID */
            id -= 0x50000;
            id += 0x5000;
            /* Store the device ID */
            if (count < BCMBD_FWM_FWINFO_CHIPS_MAX) {
                info->chips[count] = (uint16_t)id;
                count++;
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Too many supported chips in YAML.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            yaml_event_delete(&event);
            event_created = false;
        } else if (event.type == YAML_SEQUENCE_END_EVENT) {
            /* Done with the list */
            break;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid YAML value format for chips.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    if (event_created) {
        /* Destroy the event object. */
        yaml_event_delete(&event);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get value of a map item as a number.
 *
 * \param [in] parser YAML Parser object.
 * \param [in] key Key string (for debug messages).
 * \param [out] number Numeric value from the YAML item.
 *
 * \retval SHR_E_NONE No error.
 */
static int
yaml_get_number(yaml_parser_t *parser, char *key, uint64_t *number)
{
    yaml_event_t event;
    bool event_created = false;
    char *value;
    char *pstr;
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(parser, SHR_E_INTERNAL);
    SHR_NULL_CHECK(key, SHR_E_INTERNAL);
    SHR_NULL_CHECK(number, SHR_E_INTERNAL);

    /* Get the next YAML item */
    if (!yaml_parser_parse(parser, &event)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid YAML value format for key [%s].\n"),
                              key));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    event_created = true;

    /* The next YAML item should be a scalar */
    if (event.type != YAML_SCALAR_EVENT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Unsupported YAML value format for key [%s].\n"),
                              key));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Value check */
    value = (char *)event.data.scalar.value;
    if (value == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "YAML parsing failed.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    len = sal_strlen(value);
    *number = (uint64_t)sal_strtoull(value, &pstr, 0);
    if (pstr != &value[len]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid YAML value format for key [%s].\n"),
                              key));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (event_created) {
        /* Destroy the event object. */
        yaml_event_delete(&event);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill value string of a map item to a buffer.
 *
 * \param [in] parser YAML Parser object.
 * \param [in] key Key string (for debug messages).
 * \param [in] buffer Buffer address to write the string to.
 * \param [in] size Buffer size (max length to write to).
 *
 * \retval SHR_E_NONE No error.
 */
static int
yaml_get_string(yaml_parser_t *parser, char *key, char *buffer, int size)
{
    yaml_event_t event;
    bool event_created = false;
    char *value;
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(parser, SHR_E_INTERNAL);
    SHR_NULL_CHECK(key, SHR_E_INTERNAL);
    SHR_NULL_CHECK(buffer, SHR_E_INTERNAL);
    if (size <= 0) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the next YAML item */
    if (!yaml_parser_parse(parser, &event)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid YAML value format for key [%s].\n"),
                              key));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    event_created = true;

    /* The next YAML item should be a scalar */
    if (event.type != YAML_SCALAR_EVENT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Unsupported YAML value format for key [%s].\n"),
                              key));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Value check */
    value = (char *)event.data.scalar.value;
    if (value == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "YAML parsing failed.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    len = sal_strlen(value);
    if (len >= size) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "YAML value for key [%s] is too long.\n"),
                              key));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Copy to the buffer */
    sal_strncpy(buffer, value, size);

exit:
    if (event_created) {
        /* Destroy the event object. */
        yaml_event_delete(&event);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse YAML data to get firmware information.
 *
 * \param [in] buffer Buffer memory address of the YAML content.
 * \param [in] size Buffer size.
 * \param [out] info Firmware package information.
 *
 * \retval SHR_E_NONE No error.
 */
static int
yaml_parse(const uint8_t *buffer, int size, bcmbd_fwm_fwinfo_t *info)
{
    yaml_parser_t parser;
    yaml_event_t event;
    bool parser_inited = false;
    bool event_created = false;
    bool done = false;
    char *keyword;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Validate parameters */
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (size <= 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Create and initialize the Parser object. */
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, buffer, size);
    parser_inited = true;

    while (!done) {
        /* Get the next event. */
        if (!yaml_parser_parse(&parser, &event)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid YAML in firmware package.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        event_created = true;

        /* Examine top-level events */
        switch (event.type) {
        case YAML_STREAM_END_EVENT:
            /* End of the YAML content */
            done = true;
            break;

        case YAML_SCALAR_EVENT:
            /* Scalar: keyword */
            keyword = (char *)event.data.scalar.value;
            if (keyword == NULL) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "YAML parsing failed.\n")));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (!sal_strcmp(keyword, FPKG_YAML_KEY_FW_TYPE)) {
                SHR_IF_ERR_EXIT
                    (yaml_get_string(&parser, keyword, info->fw_type,
                                     sizeof(info->fw_type)));
            } else if (!sal_strcmp(keyword, FPKG_YAML_KEY_FW_VER)) {
                SHR_IF_ERR_EXIT
                    (yaml_get_string(&parser, keyword, info->fw_version,
                                     sizeof(info->fw_version)));
            } else if (!sal_strcmp(keyword, FPKG_YAML_KEY_CHIPS)) {
                SHR_IF_ERR_EXIT
                    (yaml_get_chips(&parser, info));
            } else if (!sal_strcmp(keyword, FPKG_YAML_KEY_DEF_PROC)) {
                SHR_IF_ERR_EXIT
                    (yaml_get_string(&parser, keyword, info->default_processor,
                                     sizeof(info->default_processor)));
            } else if (!sal_strcmp(keyword, FPKG_YAML_KEY_DEF_ADDR)) {
                SHR_IF_ERR_EXIT
                    (yaml_get_number(&parser, keyword, &info->default_address));
            } else if (!sal_strcmp(keyword, FPKG_YAML_KEY_PROPS)) {
                SHR_IF_ERR_EXIT
                    (yaml_get_properties(&parser, info));
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Invalid YAML keyword: [%s].\n"),
                                      keyword));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        default:
            /* Ignore all other events (stream, document, mapping, etc.) */
            break;
        }

        /* Destroy the event object. */
        yaml_event_delete(&event);
        event_created = false;
    }

    /* Check if any mandatory items are missing */
    if (info->fw_type[0] == 0 ||
        info->fw_version[0] == 0 ||
        info->chips[0] == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Missing information in YAML.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (event_created) {
        /* Destroy the event object. */
        yaml_event_delete(&event);
    }
    if (parser_inited) {
        /* Destroy the Parser object. */
        yaml_parser_delete(&parser);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse internal data structures of a fpkg to get firmware information.
 *
 * \param [in] buffer Buffer memory address.
 * \param [in] size Buffer size.
 * \param [out] info Firmware package information.
 *
 * \retval SHR_E_NONE No error.
 */
static int
bcmbd_fwm_fpkg_parse(const uint8_t *buffer, int size, bcmbd_fwm_fwinfo_t *info)
{
    int ver, sig1, sig2, fcount, f, fsize;
    bool binfo = false, bimag = false, bsign = false;
    char buf[sizeof(uint32_t) * 2 + 1];
    char ibuf[FPKG_FORMAT_FILE_INFO_LEN + 1];
    char sbuf[FPKG_FORMAT_FILE_SIZE_LEN + 1];
    char *pstr;
    int pos = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Validate parameters */
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (size <= FPKG_FORMAT_MAGIC_LEN + FPKG_FORMAT_VERSION_LEN +
                FPKG_FORMAT_FILE_COUNT_LEN + FPKG_FORMAT_CHECKSUM_LEN) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Verify fpkg magic */
    if (sal_memcmp(buffer, FPKG_FORMAT_MAGIC_STR, FPKG_FORMAT_MAGIC_LEN)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid firmware package.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    pos += FPKG_FORMAT_MAGIC_LEN;

    /* Verify fpkg version */
    sal_memcpy(buf, buffer + pos, FPKG_FORMAT_VERSION_LEN);
    buf[FPKG_FORMAT_VERSION_LEN] = 0;
    ver = (int)sal_strtoul(buf, &pstr, 10);
    if (pstr != &buf[FPKG_FORMAT_VERSION_LEN]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid firmware package version.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (ver != 1) {
        /* Only version 1 is supported for now */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Firmware package version not supported.\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    pos += FPKG_FORMAT_VERSION_LEN;

    /* Verify package checksum before parsing remaining structures */
    sal_memcpy(buf, buffer + size - FPKG_FORMAT_CHECKSUM_LEN,
               FPKG_FORMAT_CHECKSUM_LEN);
    buf[FPKG_FORMAT_CHECKSUM_LEN] = 0;
    sig1 = (int)sal_strtoul(buf, &pstr, 16);
    if (pstr != &buf[FPKG_FORMAT_CHECKSUM_LEN]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid or corrupted firmware package.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sig2 = (int)calc_checksum(buffer, size - FPKG_FORMAT_CHECKSUM_LEN);
    if (sig1 != sig2) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Corrupted firmware package.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get file count */
    sal_memcpy(buf, buffer + pos, FPKG_FORMAT_FILE_COUNT_LEN);
    buf[FPKG_FORMAT_FILE_COUNT_LEN] = 0;
    fcount = (int)sal_strtoul(buf, &pstr, 10);
    if (pstr != &buf[FPKG_FORMAT_FILE_COUNT_LEN]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Invalid firmware package.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    pos += FPKG_FORMAT_FILE_COUNT_LEN;

    if (fcount > BCMBD_FWM_FW_FILE_MAX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Exceed maximum firmware package "
                              "file count %d.\n"),
                              BCMBD_FWM_FW_FILE_MAX));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fcount < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Incorrect firmware package "
                              "file count %d.\n"),
                              fcount));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Read contained files */
    for (f = 0; f < fcount; f++) {
        /* Store file type */
        sal_memcpy(buf, buffer + pos, FPKG_FORMAT_FILE_TYPE_LEN);
        buf[FPKG_FORMAT_FILE_TYPE_LEN] = 0;
        pos += FPKG_FORMAT_FILE_TYPE_LEN;
        /* Store file info */
        sal_memcpy(ibuf, buffer + pos, FPKG_FORMAT_FILE_INFO_LEN);
        ibuf[FPKG_FORMAT_FILE_INFO_LEN] = 0;
        trim_trailing_spaces(ibuf, FPKG_FORMAT_FILE_INFO_LEN);
        pos += FPKG_FORMAT_FILE_INFO_LEN;
        /* Store file size */
        sal_memcpy(sbuf, buffer + pos, FPKG_FORMAT_FILE_SIZE_LEN);
        sbuf[FPKG_FORMAT_FILE_SIZE_LEN] = 0;
        pos += FPKG_FORMAT_FILE_SIZE_LEN;
        fsize = (uint32_t)sal_strtoul(sbuf, &pstr, 16);
        if (pstr != &sbuf[FPKG_FORMAT_FILE_SIZE_LEN] || pos + fsize >= size) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid file in firmware package.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Handle different file types */
        if (!sal_strcmp(buf, FPKG_FORMAT_FILE_TYPE_INFO_STR)) {
            /* File type: information */
            if (binfo) {
                /* Duplicated info file (not supported for now) */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Duplicated file contained in firmware "
                                      "package.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            binfo = true;
            /* Only YAML information is supported for now */
            if (sal_strcmp(ibuf, "YAML")) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Invalid info in firmware package.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* Parse YAML content */
            SHR_IF_ERR_EXIT
                (yaml_parse(buffer + pos, fsize, info));
        } else if (!sal_strcmp(buf, FPKG_FORMAT_FILE_TYPE_IMAG_STR)) {
            /* File type: image */
            if (bimag) {
                /* Duplicated info file (not supported for now) */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Duplicated file contained in firmware "
                                      "package.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            bimag = true;
            info->image_ptr = buffer + pos;
            info->image_size = fsize;
            sal_strncpy(info->image_format, ibuf,
                        BCMBD_FWM_FWINFO_IMAGE_TYPE_MAX - 1);
            info->image_format[BCMBD_FWM_FWINFO_IMAGE_TYPE_MAX - 1] = 0;
        } else if (!sal_strcmp(buf, FPKG_FORMAT_FILE_TYPE_SIGN_STR)) {
            /* File type: signature */
            if (bsign) {
                /* Duplicated info file (not supported for now) */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(BSL_UNIT_UNKNOWN,
                                      "Duplicated file contained in firmware "
                                      "package.\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            bsign = true;
            info->signature_ptr = buffer + pos;
            info->signature_size = fsize;
            sal_strncpy(info->signature_type, ibuf,
                        BCMBD_FWM_FWINFO_SIGN_TYPE_MAX - 1);
            info->signature_type[BCMBD_FWM_FWINFO_SIGN_TYPE_MAX - 1] = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Unknown content in firmware package.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        pos += fsize;
    }

    /* Check if all required files are present */
    if (!binfo || !bimag || !bsign) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Missing files in firmware package.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check final size */
    if (pos + FPKG_FORMAT_CHECKSUM_LEN != size) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Format error in firmware package.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

#else /* !YAML */

/*!
 * \brief Parse internal data structures of a fpkg to get firmware information.
 *
 * \param [in] buffer Buffer memory address.
 * \param [in] size Buffer size.
 * \param [out] info Firmware package information.
 *
 * \retval SHR_E_NONE No error.
 */
static int
bcmbd_fwm_fpkg_parse(const uint8_t *buffer, int size, bcmbd_fwm_fwinfo_t *info)
{
    return SHR_E_UNAVAIL;
}

#endif /* YAML */

/* Initialize firmware package database. */
int
bcmbd_fwm_fpkg_init(int unit)
{
    int i;
    bcmbd_fwm_fpkg_t *fpkg;
    bcmbd_fwm_fw_instances_t *instances;
    bcmbd_fwm_fw_inst_t *inst;

    SHR_FUNC_ENTER(unit);

    /* Initialize firmware package database. */
    if (fw_package_init == 0) {
        for (i = 0; i < BCMBD_FWM_FPKG_NUM_MAX; i++) {
            fpkg = &fw_package[i];
            fpkg->fpkg_id = -1;
            fpkg->fpkg_info = NULL;
        }
        fw_package_init = 1;
    }
    if (!fpkg_mutex) {
        fpkg_mutex = sal_mutex_create("bcmbdFwmFpkg");
        SHR_NULL_CHECK(fpkg_mutex, SHR_E_MEMORY);
    }

    /*
     * Initialize firmware instance database.
     *
     * Firmware instance database is updated only when firmware package loaded.
     * Use the firmware package lock(fpkg_mutex) for the resource.
     */
    instances = sal_alloc(sizeof(bcmbd_fwm_fw_instances_t), "bcmbdFwmFwInst");
    SHR_NULL_CHECK(instances, SHR_E_MEMORY);
    sal_memset(instances, 0, sizeof(bcmbd_fwm_fw_instances_t));
    for (i = 0; i < BCMBD_FWM_FW_INST_NUM_MAX; i++) {
        inst = &instances->instance[i];
        inst->fw_instance = -1;
    }
    bcmbd_fwm_fw_inst_db[unit] = instances;

exit:
    if (SHR_FUNC_ERR()) {
        if (fpkg_mutex) {
            sal_mutex_destroy(fpkg_mutex);
            fpkg_mutex = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/* Cleanup firmware package database. */
int
bcmbd_fwm_fpkg_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmbd_fwm_fw_inst_db[unit]) {
        sal_free(bcmbd_fwm_fw_inst_db[unit]);
        bcmbd_fwm_fw_inst_db[unit] = NULL;
    }

    fw_package_init = 0;

    if (fpkg_mutex) {
        sal_mutex_destroy(fpkg_mutex);
        fpkg_mutex = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/* Providing the buffer that stores the whole firmware package image. */
int
bcmbd_fwm_fpkg_open(const uint8_t *buffer, int size, int *fpkg_id)
{
    int slot;
    bcmbd_fwm_fpkg_t *fpkg;
    int locked = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Validate parameters */
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(fpkg_id, SHR_E_PARAM);

    SHR_NULL_CHECK(fpkg_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fpkg_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    /* Find the first available slot. */
    for (slot = 0; slot < BCMBD_FWM_FPKG_NUM_MAX; slot++) {
        fpkg = &fw_package[slot];
        if (fpkg->fpkg_id == -1) {
            break;
        }
    }

    if (slot >= BCMBD_FWM_FPKG_NUM_MAX) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    /*
     * 1. Get the whole firmware package buffer.
     * 2. Verify signature if required.
     * 3. Convert and save a copy of FWM info database.
     * 4. Transfer firmware image to binary if required.
     */
    fpkg->fpkg_info = sal_alloc(sizeof(bcmbd_fwm_fwinfo_t), "bcmbdFwmFpkgInfo");
    if (fpkg->fpkg_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(fpkg->fpkg_info, 0, sizeof(bcmbd_fwm_fwinfo_t));

    /* Parse the fpkg content */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fpkg_parse(buffer, size, fpkg->fpkg_info));

    /* Until now, the entry is successfully allocated. */
    fpkg->fpkg_id = slot;

    /* Return the allocated handler. */
    *fpkg_id = slot;

exit:
    if (locked && fpkg_mutex) {
        sal_mutex_give(fpkg_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Close the associated fpkg handler. */
int
bcmbd_fwm_fpkg_close(int fpkg_id)
{
    bcmbd_fwm_fpkg_t *fpkg;
    int locked = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Range validation. */
    if ((fpkg_id < 0) || (fpkg_id >= BCMBD_FWM_FPKG_NUM_MAX)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(fpkg_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fpkg_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    fpkg = &fw_package[fpkg_id];

    /* The fpkg_id is not created. */
    if (fpkg->fpkg_id == -1) {
        SHR_IF_ERR_EXIT(SHR_E_EMPTY);
    }

    /* Free the opened FWM database. */
    fpkg->fpkg_id = -1;

    if (fpkg->fpkg_info) {
        sal_free(fpkg->fpkg_info);
        fpkg->fpkg_info = NULL;
    }

exit:
    if (locked && fpkg_mutex) {
        sal_mutex_give(fpkg_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Load the firmware image to memory of the specified processor. */
int
bcmbd_fwm_fpkg_load(int unit, int fpkg_id, const char *processor, int *instance)
{
    int i = -1;
    int match = -1;
    bcmbd_fwm_fwinfo_t *fwinfo;
    bcmbd_fwm_proc_t *proc_drv = NULL;
    char *ret_str;
    char proc_name[BCMBD_FWM_PROC_INST_NAME_MAX];
    int proc_idx = -1;
    int inst_idx = -1;
    int set_slot;
    int set_inst_num;
    int last_instance_num;
    bcmbd_fwm_fpkg_t *fpkg;
    bcmbd_fwm_fw_instances_t *instances = NULL;
    bcmbd_fwm_fw_inst_t *inst;
    int locked = 0;
    bcmdrd_dev_id_t my_dev_id;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(instance, SHR_E_PARAM);
    if ((fpkg_id < 0) || (fpkg_id >= BCMBD_FWM_FPKG_NUM_MAX)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(fpkg_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fpkg_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    fpkg = &fw_package[fpkg_id];

    /* The fpkg_id is not created. */
    if (fpkg->fpkg_id == -1) {
        SHR_IF_ERR_EXIT(SHR_E_EMPTY);
    }

    fwinfo = fpkg->fpkg_info;

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_id_get(unit, &my_dev_id));

    instances = bcmbd_fwm_fw_inst_db[unit];
    SHR_NULL_CHECK(instances, SHR_E_INIT);

    /* Check device ID matched and get the processor name. */
    match = -1;
    for (i = 0; i < BCMBD_FWM_FWINFO_CHIPS_MAX; i++) {
        if (my_dev_id.device_id == fwinfo->chips[i]) {
            match = 1;
            if (processor == NULL) {
                /* Use default processor from FPKG information. */
                processor = fwinfo->default_processor;
            }
            ret_str = sal_strchr(processor, '.');
            if (ret_str == NULL) {
                /* No instance specified, assign instance 0. */
                int nchs = sal_snprintf(proc_name, BCMBD_FWM_PROC_INST_NAME_MAX,
                                        "%s.%d", processor, 0);
                if (nchs < 0 || nchs >= BCMBD_FWM_PROC_INST_NAME_MAX) {
                    /* The output is erroneous or truncated */
                    SHR_IF_ERR_EXIT(SHR_E_PARAM);
                }
            } else {
                if (sal_strlen(processor) >= BCMBD_FWM_PROC_INST_NAME_MAX) {
                    SHR_IF_ERR_EXIT(SHR_E_PARAM);
                }
                sal_strncpy(proc_name, processor,
                            BCMBD_FWM_PROC_INST_NAME_MAX - 1);
                proc_name[BCMBD_FWM_PROC_INST_NAME_MAX - 1] = 0;
            }
            break;
        }
    }
    if (match < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "The device 0x%x is not in the firmware "
                              "supported ID list.\n"), my_dev_id.device_id));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Verify image signature - only checksum is supported for now */
    if (!sal_strcmp(fwinfo->signature_type, "checksum")) {
        char buf[FPKG_FORMAT_CHECKSUM_LEN + 1];
        char *pstr;
        int sig1, sig2;
        if (fwinfo->signature_size != FPKG_FORMAT_CHECKSUM_LEN) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Invalid firmware signature.\n")));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        sal_memcpy(buf, fwinfo->signature_ptr, FPKG_FORMAT_CHECKSUM_LEN);
        buf[FPKG_FORMAT_CHECKSUM_LEN] = 0;
        sig1 = (int)sal_strtoul(buf, &pstr, 16);
        if (pstr != &buf[FPKG_FORMAT_CHECKSUM_LEN]) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Invalid firmware signature.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        sig2 = (int)calc_checksum(fwinfo->image_ptr, fwinfo->image_size);
        if (sig1 != sig2) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(BSL_UNIT_UNKNOWN,
                                  "Corrupted firmware image.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Signature type not supported: %s\n"),
                             fwinfo->signature_type));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Write to processor. */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_index_get(unit, proc_name, &proc_idx, &inst_idx));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_drv_get(unit, proc_idx, &proc_drv));

    if (!proc_drv) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Cannot get processor driver of %s.\n"),
                              processor));
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    /* Only BIN format is supported for now */
    if (sal_strcmp(fwinfo->image_format, "BIN")) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(BSL_UNIT_UNKNOWN,
                              "Firmware image format not supported: %s\n"),
                              fwinfo->image_format));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* BIN format: simply write the whole image */
    if (proc_drv->write) {
        SHR_IF_ERR_EXIT
            (proc_drv->write(unit, inst_idx, fwinfo->default_address,
                             fwinfo->image_ptr,
                             fwinfo->image_size));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Processor driver for loading firmware is "
                              "not registered.\n")));
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    /* Find empty entry. Get the existed instance count of the feature. */
    set_slot = -1;
    last_instance_num = -1;
    for (i = 0; i < BCMBD_FWM_FW_INST_NUM_MAX; i++) {
        inst = &instances->instance[i];
        if (!sal_strcasecmp(fwinfo->fw_type, inst->fw_feature) &&
            !sal_strcasecmp(proc_name, inst->proc_instance)) {
            /* Same firmware type and same proc instance, overwrite the slot. */
            set_slot = i;
            set_inst_num = inst->fw_instance;
            break;
        } else {
            /* Find an empty slot */
            if ((set_slot < 0) && (inst->fw_instance < 0)) {
                set_slot = i;
            }

            if (!sal_strcasecmp(fwinfo->fw_type, inst->fw_feature)) {
                if (last_instance_num < inst->fw_instance) {
                    last_instance_num = inst->fw_instance;
                }
            }
            set_inst_num = last_instance_num + 1;
        }
    }

    /* Firmware instance database is full. */
    if (set_slot < 0) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    inst = &instances->instance[set_slot];
    /* Update firmware instance database. */
    sal_strncpy(inst->fw_feature, fwinfo->fw_type,
                BCMBD_FWM_FWINFO_FW_TYPE_MAX);
    inst->fw_instance = set_inst_num;
    sal_strncpy(inst->fw_version, fwinfo->fw_version,
                BCMBD_FWM_FWINFO_FW_VERSION_MAX);
    sal_strncpy(inst->proc_instance, proc_name,
                BCMBD_FWM_PROC_INST_NAME_MAX);

    /* Return the firmware instance ID. */
    *instance = inst->fw_instance;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Created firmware instance #%d %s.%d after "
                            "firmware loaded.\n"),
                            set_slot, inst->fw_feature, inst->fw_instance));

    /* Notify chip driver. */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_notif_op(unit, inst->fw_feature, inst->fw_instance,
                            BCMBD_FWM_FW_NOTIF_OPTION_LOADED, NULL));

exit:
    if (locked && fpkg_mutex) {
        sal_mutex_give(fpkg_mutex);
    }

    SHR_FUNC_EXIT();
}

/* Query firmware information. */
int
bcmbd_fwm_fpkg_info_get(int fpkg_id, bcmbd_fwm_fwinfo_t *info)
{
    bcmbd_fwm_fwinfo_t *fwinfo;
    bcmbd_fwm_fpkg_t *fpkg;
    int locked = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Validate parameters */
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if ((fpkg_id < 0) || (fpkg_id >= BCMBD_FWM_FPKG_NUM_MAX)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(fpkg_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fpkg_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    fpkg = &fw_package[fpkg_id];

    /* The fpkg_id is not created. */
    if (fpkg->fpkg_id == -1) {
        SHR_IF_ERR_EXIT(SHR_E_EMPTY);
    }

    fwinfo = fpkg->fpkg_info;

    sal_memcpy(info, fwinfo, sizeof(bcmbd_fwm_fwinfo_t));

exit:
    if (locked && fpkg_mutex) {
        sal_mutex_give(fpkg_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Query optional properties defined in this firmware. Get by a key. */
int
bcmbd_fwm_fpkg_prop_get(int fpkg_id, const char *key, int buf_size, char *buf)
{
    /* To be implemented. */
    return SHR_E_UNAVAIL;
}

/* Get key of a property specified by index. */
int
bcmbd_fwm_fpkg_prop_key_get(int fpkg_id, int prop_idx, int buf_size, char *buf)
{
    /* To be implemented. */
    return SHR_E_UNAVAIL;
}

/* Get value of a property specified by index. */
int
bcmbd_fwm_fpkg_prop_val_get(int fpkg_id, int prop_idx, int buf_size, char *buf)
{
    /* To be implemented. */
    return SHR_E_UNAVAIL;
}

/* Get the number of firmware instance. */
int
bcmbd_fwm_fw_instance_count_get(int unit, int *count)
{
    int i;
    int inst_cnt;
    bcmbd_fwm_fw_instances_t *instances = NULL;
    bcmbd_fwm_fw_inst_t *inst;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_NULL_CHECK(fpkg_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fpkg_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    instances = bcmbd_fwm_fw_inst_db[unit];
    SHR_NULL_CHECK(instances, SHR_E_INIT);

    inst_cnt = 0;
    for (i = 0; i < BCMBD_FWM_FW_INST_NUM_MAX; i++) {
        inst = &instances->instance[i];
        if (inst->fw_instance >= 0) {
            inst_cnt++;
        }
    }

    *count = inst_cnt;

exit:
    if (locked && fpkg_mutex) {
        sal_mutex_give(fpkg_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Get the firmware instance information. */
int
bcmbd_fwm_fw_instance_get(int unit, int inst_idx, bcmbd_fwm_fwinst_t *fwinst)
{
    bcmbd_fwm_fw_instances_t *instances = NULL;
    bcmbd_fwm_fw_inst_t *inst;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(fwinst, SHR_E_PARAM);
    if ((inst_idx < 0) || (inst_idx >= BCMBD_FWM_FW_INST_NUM_MAX)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(fpkg_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fpkg_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    instances = bcmbd_fwm_fw_inst_db[unit];
    SHR_NULL_CHECK(instances, SHR_E_INIT);

    inst = &instances->instance[inst_idx];

    if (inst->fw_instance < 0) {
        SHR_IF_ERR_EXIT(SHR_E_EMPTY);
    }

    sal_snprintf(fwinst->name, BCMBD_FWM_FWINST_FW_NAME_MAX, "%s.%d",
                 inst->fw_feature, inst->fw_instance);
    sal_strncpy(fwinst->fw_version, inst->fw_version,
                BCMBD_FWM_FWINST_FW_VERSION_MAX - 1);
    fwinst->fw_version[BCMBD_FWM_FWINST_FW_VERSION_MAX - 1] = 0;
    sal_strncpy(fwinst->processor, inst->proc_instance,
                BCMBD_FWM_PROC_INST_NAME_MAX - 1);
    fwinst->processor[BCMBD_FWM_PROC_INST_NAME_MAX - 1] = 0;

exit:
    if (locked && fpkg_mutex) {
        sal_mutex_give(fpkg_mutex);
    }
    SHR_FUNC_EXIT();
}
