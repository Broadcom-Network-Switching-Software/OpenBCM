/*! \file config.c
 *
 * Configuration management.
 * This file contains yaml event callback and database for configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <soc/ltsw/config.h>

#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_read_handler.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#define CONFIG_YAML_LEVEL_MAX BCMCFG_MAX_LEVEL
#define CONFIG_UNIT_MAX BCMDRD_CONFIG_MAX_UNITS
#define CONFIG_PORT_MAX BCMDRD_CONFIG_MAX_PORTS

#define CONFIG_HASH_MAX_COUNT 1024
#define CONFIG_NAME_MAX 128


/* Configuration information structure. */
typedef struct config_fld_s {
    /* Next property field with same hash. */
    struct config_fld_s *next;

    /* Configuration property name. */
    char *name;

    /* Configuration property value. */
    char *value;
} config_fld_t;

/* Configuration database. */
typedef struct config_db_s {
    /* Protection mutex. */
    sal_mutex_t mutex;

    /* Property field data. */
    config_fld_t *fld[CONFIG_HASH_MAX_COUNT];
} config_db_t;

/* Configuration module informaiton. */
typedef struct config_info_s {
    /* Initialized flag. */
    int inited;

    /* Property field data. */
    config_db_t config_db[CONFIG_UNIT_MAX];
} config_info_t;

/* static */
static config_info_t config_info = { 0 };

/* State of configuration table for Yaml parser. */
typedef enum config_tbl_state_e {
    /* Unit - can transit to TABLE state. */
    UNIT,

    /* Table - can transit to FIELD state or KEY state. */
    TABLE,

    /* Key - can transit to FIELD state. */
    KEY,

    /* Field. */
    FIELD
} config_tbl_state_t;

/* Type string of configuration table. */
static const char *config_tbl_type_str[] = {
    "global",
    "port"
};

/* Type of configuration table. */
typedef enum config_tbl_type_e {
    /* Global configuration in an unit. */
    CONFIG_TBL_GLOBAL,

    /* Per-port configuration in an unit. */
    CONFIG_TBL_PORT,

    /* Count as a constraint. */
    CONFIG_TBL_COUNT
} config_tbl_type_t;

/* Configuration read stack information. */
typedef struct config_stk_info_s {
    /* Table parser state. */
    config_tbl_state_t state;
} config_stk_info_t;

/* Range Information used for key range specification. */
typedef struct config_key_range_s {
    /* Min key value. */
    uint32_t min;

    /* Max key value. */
    uint32_t max;

    /* Step value. */
    uint32_t step;

    /* Count. */
    uint32_t count;

    /* True if any key value is available. */
    bool asterisk;

    /* Next range. */
    struct config_key_range_s *next;
} config_key_range_t;

/*
 * Configuration key field sequence parsing mode.
 *
 * Single element mode
 *   a. Single element without square bracket.
 *      2                   # 2
 *   b. All digits array within first square bracket are treated as collection
 *      of single elements.
 *      [9, 12, 15]         # 9, 12, 15
 *      [9, 12]             # 9, 12
 *
 * Range mode with two square brackets
 *   c. range without step (2 element within 2nd square bracket, [min, max])
 *      [[9, 12], [14, 16]] # 9, 10, 11, 12, 14, 15, 16
 *   d. range with step (3 element within 2nd square bracket, [min, max, step])
 *      [[9, 18, 3],
 *       [22, 26, 2]]       # 9, 12, 15, 18, 22, 24, 26
 *   e. b + c
 *      [9, [12, 15], 18]   # 9, 12, 13, 14, 15, 18
 *   f. b + d
 *      [9, [12, 21, 3]]    # 9, 12, 15, 18, 21
 *   g. c + d
 *      [[9, 12],
 *       [15, 21, 3]]       # 9, 10, 11, 12, 15, 18, 21
 *   h. b + c + d
 *      [[9, 12], 13,
 *       [15, 21, 3], 23]   # 9, 10, 11, 12, 13, 15, 18, 21, 23
 */
typedef enum config_key_seq_mode_e {
    /* Mode for single element. */
    CONFIG_KEY_SEQ_MODE_SINGLE,

    /* Mode for key field range specification. */
    CONFIG_KEY_SEQ_MODE_RANGE
} config_key_seq_mode_t;

/* Configuraiton table parser user data. */
typedef struct config_tbl_user_s {
    /* Reader stack. */
    config_stk_info_t info[CONFIG_YAML_LEVEL_MAX];

    /* Unit set. */
    bool unit[CONFIG_UNIT_MAX];

    /* Current key range list of configuration table. */
    config_key_range_t *key_range_list;

    /* Table type. */
    config_tbl_type_t type;

    /* Current mode of parsing sequence event for key field. */
    config_key_seq_mode_t key_seq_mode;

    /* Current key range of configuration table. */
    config_key_range_t key_range;

    /* Current field name of configuration property. */
    char *fld_name;
} config_tbl_user_t;

/* Parse information. */
static bcmcfg_parse_info_t config_parse_info = {
    .mode = PARSE_MODE_GEN,
    .target_unit = BSL_UNIT_UNKNOWN
};

/* Macro to check config mode is valid */
#define CONFIG_MODE_VALID (PARSE_MODE_GEN == config_parse_info.mode)

/* Declarations of the functions and structure for yaml read handler. */
static int
config_read_setup(bool start, bcmcfg_parse_info_t *info, void *user_data);

static int
config_read_scalar(const char *value,
                   bcmcfg_read_context_t *context,
                   void *user_data);

static int
config_read_map(bool start,
                bcmcfg_read_context_t *context,
                void *user_data);

static int
config_read_seq(bool start,
                bcmcfg_read_context_t *context,
                void *user_data);

static int
config_read_doc(bool start,
                bcmcfg_read_context_t *context,
                void *user_data);

static int
config_read_abort(bcmcfg_read_context_t *context,
                  void *user_data);

static config_tbl_user_t config_tbl_user;

/* Yaml read handler. */
const bcmcfg_read_handler_t config_read_hdl = {
    .key       = "bcm_device",
    .setup     = config_read_setup,
    .scalar    = config_read_scalar,
    .map       = config_read_map,
    .seq       = config_read_seq,
    .doc       = config_read_doc,
    .abort     = config_read_abort,
    .user_data = (void *)&config_tbl_user
};

/*
 * TLV MESSAGE for HA usage
 *
 * +------------------------------------------+--------------+
 * |                  TLV(s)                  | TLV_TYPE_END |
 * +------------------------------------------+--------------+
 *
 *
 * TLV
 *  0    7 8           23 24
 * +------+--------------+-----------------------------------+
 * | TYPE |    LENGTH    |              VALUE                |
 * +------+--------------+-----------------------------------+
 *                       |<------------- LENGTH------------->|
 *
 *
 * TLV Message
 * - A TLV message contains 0 or more TLV elements.
 * - A TLV message is terminated by a special TLV with CONFIG_TLV_MSG_TYPE_END.
 *
 * TLV Fields
 * Type   - indicates type of information, defined as "config_tlv_msg_type_t".
 * Length - is the size, in bytes, of the data to follow for this element,
 *          defined as "config_tlv_msg_length_t".
 * Value  - is the variable sized set of bytes which contains
 *          data for this element.
 */

/* TLV Message Type for Configuration Field Name. */
#define CONFIG_TLV_MSG_TYPE_FLD_NAME (0xa)
/* TLV Message Type for Configuration Field Value. */
#define CONFIG_TLV_MSG_TYPE_FLD_VALUE (0xb)
/* TLV type to indicates end of message. */
#define CONFIG_TLV_MSG_TYPE_END (0xfe)

/* Typedef config_tlv_msg_type_t */
typedef uint8_t  config_tlv_msg_type_t;
/* Typedef config_tlv_msg_length_t */
typedef uint16_t config_tlv_msg_length_t;
/* Size of TLV END Message */
#define CONFIG_TLV_MSG_END_SIZE (sizeof(uint8_t))

/* Macro to check message type is valid */
#define CONFIG_TLV_MSG_TYPE_VALID(type) \
    ((type == CONFIG_TLV_MSG_TYPE_FLD_NAME) || \
     (type == CONFIG_TLV_MSG_TYPE_FLD_VALUE))

/* Configuration TLV Message data structure */
typedef struct config_tlv_msg_s {
    /* Start of message in buffer */
    uint8_t *start;

    /* End of message in buffer */
    uint8_t *end;

    /* End of buffer */
    uint8_t *buffer_end;

    /* Current ptr in buffer to write/read */
    uint8_t *cur_ptr;

    /* Ptr to current TLV length in buffer */
    uint8_t *tlv_length_ptr;

    /* Length for current TLV */
    config_tlv_msg_length_t tlv_length;

    /* Bytes left to read in current TLV */
    int tlv_left;
} config_tlv_msg_t;

/*
 * Port configuration database structure. This is mainly used for
 * config show command.
 */
typedef struct config_port_db_s {

    /* Port field data. */
    config_fld_t *fld[CONFIG_PORT_MAX + 1];
} config_port_db_t;

static config_port_db_t config_port_db[CONFIG_UNIT_MAX];

/* Configuration Lock. */
#define CONFIG_LOCK(u) \
    do { \
        config_db_t *db = &(config_info.config_db[u]); \
        if (db->mutex) { \
            sal_mutex_take(db->mutex, SAL_MUTEX_FOREVER); \
        } \
    } while (0)

#define CONFIG_UNLOCK(u) \
    do { \
        config_db_t *db = &(config_info.config_db[u]); \
        if (db->mutex) { \
            sal_mutex_give(db->mutex); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Hashing string function.
 *
 * \param [in] str String.
 *
 * \retval Hash id.
 */
static int
config_field_hash(const char *str)
{
    uint32_t h = 5381;
    int i = 0;

    while(str[i] != 0 ) {
        h = ((h << 5) + h) + str[i];
        i++;
    }

    h = (((h << 5) + h) + i) % CONFIG_HASH_MAX_COUNT;

    return h;
}

/*!
 * \brief Find configuration property field.
 *
 * \param [in] unit Unit number.
 * \param [in] name Property field name.
 *
 * \return Pointer to property field.
 */
static config_fld_t *
config_field_find(int unit, const char *name)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *fld = NULL;
    int hash = config_field_hash(name);

    CONFIG_LOCK(unit);

    fld = cd->fld[hash];

    while (fld != NULL) {
        if (sal_strcmp(fld->name, name) == 0) {
            break;
        }
        fld = fld->next;
    }

    CONFIG_UNLOCK(unit);

    return fld;
}

/*!
 * \brief Insert a configuration field.
 *
 * \param [in] unit         Unit number.
 * \param [in] name         Field name string.
 * \param [in] value        Field value string.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_field_insert(int unit,
                    const char *name,
                    const char *value)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *found_fld = NULL;
    config_fld_t *fld = NULL;
    char *fld_value = NULL;
    int hash = config_field_hash(name);
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    CONFIG_LOCK(unit);

    /* Try to find an existing configuration field. */
    found_fld = cd->fld[hash];

    while (found_fld != NULL) {
        if (sal_strcmp(found_fld->name, name) == 0) {
            break;
        }
        found_fld = found_fld->next;
    }

    if (found_fld != NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Overwriting duplicate config: %s = %s.\n"),
                              name, value));

        alloc_size = sal_strlen(value) + 1;
        SHR_ALLOC(fld_value, alloc_size, "ltswCfgValue");
        SHR_NULL_CHECK(fld_value, SHR_E_MEMORY);
        sal_memcpy(fld_value, value, alloc_size);

        SHR_FREE(found_fld->value);
        found_fld->value = fld_value;
        SHR_EXIT();
    }

    /* Insert new a configuration field. */
    alloc_size = sizeof(config_fld_t);
    SHR_ALLOC(fld, alloc_size, "ltswCfgData");
    SHR_NULL_CHECK(fld, SHR_E_MEMORY);
    sal_memset(fld, 0, alloc_size);

    alloc_size = sal_strlen(name) + 1;
    SHR_ALLOC(fld->name, alloc_size, "ltswCfgName");
    SHR_NULL_CHECK(fld->name, SHR_E_MEMORY);
    sal_memcpy(fld->name, name, alloc_size);

    alloc_size = sal_strlen(value) + 1;
    SHR_ALLOC(fld->value, alloc_size, "ltswCfgValue");
    SHR_NULL_CHECK(fld->value, SHR_E_MEMORY);
    sal_memcpy(fld->value, value, alloc_size);

    fld->next = cd->fld[hash];
    cd->fld[hash] = fld;

exit:
    if (SHR_FUNC_ERR()) {
        if (fld != NULL) {
            SHR_FREE(fld->name);
            SHR_FREE(fld->value);
            SHR_FREE(fld);
        }
        SHR_FREE(fld_value);
    }
    CONFIG_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all configuration property fields.
 *
 * \param [in] unit Unit number.
 *
 * \return None.
 */
static void
config_field_delete_all(int unit)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *fld = NULL;
    int i = 0;

    for (i = 0; i < CONFIG_HASH_MAX_COUNT; i++) {
       while (cd->fld[i] != NULL) {
           fld = cd->fld[i];
           cd->fld[i] = fld->next;
           SHR_FREE(fld->name);
           SHR_FREE(fld->value);
           SHR_FREE(fld);
       }
    }

    return;
}

/*!
 * \brief Translate configuration field name.
 *
 * \param [in] type     Type of configuration table.
 * \param [in] asterisk Any key value.
 * \param [in] key      Key value.
 * \param [in] fld_name Field name string.
 * \param [out]db_name  New field name string stored in configuration DB.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
config_field_name_translate(config_tbl_type_t type,
                            bool asterisk,
                            int key,
                            const char *fld_name,
                            char *db_name)
{
    int size = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (asterisk) {
        size = sal_snprintf(db_name, CONFIG_NAME_MAX, "%s_%s_%s", fld_name,
                            config_tbl_type_str[type], "*");
    } else {
        size = sal_snprintf(db_name, CONFIG_NAME_MAX, "%s_%s_%d", fld_name,
                            config_tbl_type_str[type], key);
    }

    if (size >= CONFIG_NAME_MAX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unsupported string length for %s_%s%d.)!\n"),
                  fld_name, config_tbl_type_str[type], key));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup configuration module information.
 *
 * \param  None
 *
 * \retval None
 */
static void
config_info_cleanup(void)
{
    int unit = 0;
    config_db_t *cd = NULL;

    config_info.inited = 0;

    /* Initialize configuration database. */
    for (unit = 0; unit < CONFIG_UNIT_MAX; unit++) {
        CONFIG_LOCK(unit);
        config_field_delete_all(unit);
        CONFIG_UNLOCK(unit);

        cd = &(config_info.config_db[unit]);
        if (cd->mutex) {
            sal_mutex_destroy(cd->mutex);
            cd->mutex = NULL;
        }
    }

    /* Initialize configuration information. */
    sal_memset(&config_info, 0, sizeof(config_info_t));

    return;
}

/*!
 * \brief Parse a string to a uint32.
 *
 * \param [in] str          String.
 * \param [out] value       Output value.
 * \param [in] base         Numeric base.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_str_to_uint32(const char *str, uint32_t *value, int base)
{
    uint32_t val;
    char *end;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    val = sal_strtoul(str, &end, base);
    if (*str && *end) {
        /* Invalid conversion */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unable to convert %s\n"), str));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    } else {
        *value = val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by number.
 *
 * \param [out] user     Configuration user data.
 * \param [in] unit         Unit number.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_add_unit(config_tbl_user_t *user, int unit)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Add a unit. */
    if (bcmdrd_dev_exists(unit)) {
        user->unit[unit] = true;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by numeric string.
 *
 * \param [in] user         Configuration user data.
 * \param [in] unit         Unit number.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_add_unit_num(config_tbl_user_t *user, const char *str)
{
    uint32_t value;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (config_str_to_uint32(str, &value, 0));

    SHR_IF_ERR_EXIT
        (config_read_add_unit(user, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add all units that currently exist.
 *
 * \param [in] user         Configuration user data.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_add_all_units(config_tbl_user_t *user)
{
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (unit = 0; unit < CONFIG_UNIT_MAX; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (config_read_add_unit(user, unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add units that correspond to the given name.
 *
 * \param [in] user         Configuration user data.
 * \param [in] str          Unit name.
 * \param [out] found       Name found flag.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_add_unit_name(config_tbl_user_t *user,
                          const char *str,
                          bool *found)
{
    int len = 0;
    int unit = 0;
    int rv = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    len = sal_strlen(str);
    *found = false;

    for (unit = 0; unit < CONFIG_UNIT_MAX; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        if (!sal_strncmp(bcmdrd_dev_name(unit), str, len)) {
            rv = config_read_add_unit(user, unit);
            if (SHR_SUCCESS(rv)) {
                *found = true;
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by string which may be an integer or a device name.
 *
 * \param [in] user         Configuration user data.
 * \param [in] str          Unit string.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_add_unit_str(config_tbl_user_t *user, const char *str)
{
    bool found = false;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!sal_strcmp(str, "*")) {
        SHR_IF_ERR_EXIT
            (config_read_add_all_units(user));
    } else {
        rv = config_read_add_unit_name(user, str, &found);
        if (SHR_SUCCESS(rv) && !found) {
            rv = config_read_add_unit_num(user, str);
        }
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach configuration property field name.
 *
 * \param [in] user         Gobal user data.
 * \param [in] name         Field Name.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_field_attach(config_tbl_user_t *user,
                    const char *name)
{
    uint32_t alloc_size = 0;
    char *str = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (user->fld_name != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    alloc_size = sal_strlen(name) + 1;
    SHR_ALLOC(str, alloc_size, "ltswCfgName");
    SHR_NULL_CHECK(str, SHR_E_MEMORY);
    sal_memcpy(str, name, alloc_size);

    user->fld_name = str;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a configuration field.
 *
 * \param [in] user         Configuration user data.
 * \param [in] value        Field value string.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_field_add(config_tbl_user_t *user,
                 const char *value)
{
    int i = 0;
    int j = 0;
    config_key_range_t *key = NULL;
    char name[CONFIG_NAME_MAX];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (user->fld_name == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Add configuration data into hash table. */
    for (i = 0; i < CONFIG_UNIT_MAX; i++) {

        if (!user->unit[i]) {
            continue;
        }

        if (user->type == CONFIG_TBL_GLOBAL) {
            SHR_IF_ERR_EXIT
                (config_field_insert(i, user->fld_name, value));
        } else {
            if (user->key_range_list == NULL) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }

            key = user->key_range_list;

            while (key) {
                for (j = key->min; j <= key->max; j += key->step) {

                    SHR_IF_ERR_EXIT
                        (config_field_name_translate(user->type, key->asterisk,
                                                     j, user->fld_name, name));

                    SHR_IF_ERR_EXIT
                        (config_field_insert(i, name, value));
                }

                key = key->next;
            }
        }
    }

exit:
    SHR_FREE(user->fld_name);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle a configuration field.
 *
 * \param [in] info         Reader data.
 * \param [in] user         Configuration user data.
 * \param [in] str          Field string.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_field_handle(bcmcfg_read_level_info_t *info,
                    config_tbl_user_t *user,
                    const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (info->state) {
    case BCMCFG_READ_STATE_MAP:
        SHR_IF_ERR_EXIT
            (config_field_attach(user, str));
        break;

    case BCMCFG_READ_STATE_MAP_V:
        SHR_IF_ERR_EXIT
            (config_field_add(user, str));
        break;

    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize key range list.
 *
 * \param [in] user Configuration user data.
 *
 * \retval None.
 */
static void
config_key_range_list_init(config_tbl_user_t *user)
{
    config_key_range_t *range = NULL;

    while (user->key_range_list) {
        range = user->key_range_list;
        user->key_range_list = range->next;
        SHR_FREE(range);
    }

    user->key_range_list = NULL;

    return;
}

/*!
 * \brief Initialize current range information.
 *
 * \param [out] user Configuration user data.
 *
 * \retval None.
 */
static void
config_key_range_init(config_tbl_user_t *user)
{
    sal_memset(&(user->key_range), 0, sizeof(config_key_range_t));
    user->key_seq_mode = CONFIG_KEY_SEQ_MODE_SINGLE;

    return;
}

/*!
 * \brief Initialize current unit set.
 *
 * \param [out] user Configuration user data.
 *
 * \retval None.
 */
static void
config_unit_init(config_tbl_user_t *user)
{
    sal_memset(&(user->unit), 0, sizeof(bool) * CONFIG_UNIT_MAX);

    return;
}

/*!
 * \brief Attach configuration key range.
 *
 * \param [in] user         Configuration user data.
 * \param [in] key_range    Key range.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_key_attach(config_tbl_user_t *user,
                  config_key_range_t *key_range)
{
    uint32_t alloc_size = 0;
    config_key_range_t *key = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (key_range->next != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    alloc_size = sizeof(config_key_range_t);
    SHR_ALLOC(key, alloc_size, "ltswCfgKeyRange");
    SHR_NULL_CHECK(key, SHR_E_MEMORY);

    sal_memcpy(key, key_range, alloc_size);
    key->next = user->key_range_list;
    user->key_range_list = key;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct configuration key range.
 *
 * \param [in] info         Reader data.
 * \param [in] user         Configuration user data.
 * \param [in] str          Field string.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_key_range_construct(bcmcfg_read_level_info_t *info,
                           config_tbl_user_t *user,
                           const char *str)
{
    uint32_t value = 0;
    config_key_range_t *key_range = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    key_range = &(user->key_range);

    if (user->key_seq_mode == CONFIG_KEY_SEQ_MODE_SINGLE) {
        if (key_range->count > 0) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (!sal_strcmp(str, "*")) {
        
        if (user->key_seq_mode != CONFIG_KEY_SEQ_MODE_SINGLE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("%s:%d:%d Symbol element in key range\n"),
                      info->locus, info->line, info->column));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        key_range->asterisk = true;
    } else {
        SHR_IF_ERR_EXIT
            (config_str_to_uint32(str, &value, 0));
        key_range->asterisk = false;
    }

    switch (key_range->count) {
    case 0:
        key_range->min = value;
        key_range->max = value;
        key_range->step = 1;
        key_range->next = NULL;
        break;

    case 1:
        key_range->max = value;
        if (key_range->max < key_range->min) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("%s:%d:%d Min value is greater than max\n"),
                      info->locus, info->line, info->column));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        break;

    case 2:
        if (value < 1) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("%s:%d:%d: Step size must be greater than 1\n"),
                      info->locus, info->line, info->column));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        if (value > (key_range->max - key_range->min)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("%s:%d:%d Step size is too large\n"),
                      info->locus, info->line, info->column));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        key_range->step = value;
        break;

    default:
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("%s:%d:%d Too many elements in key range\n"),
                  info->locus, info->line, info->column));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    key_range->count++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle a configuration key field.
 *
 * \param [in] info         Reader data.
 * \param [in] user         Configuration user data.
 * \param [in] str          Field string.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_key_add(bcmcfg_read_level_info_t *info,
               config_tbl_user_t *user,
               const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Construct configuration key range. */
    SHR_IF_ERR_EXIT
        (config_key_range_construct(info, user, str));

    /*
     * Single mode:
     *     Add key into key range list directly for single mode.
     * Range mode:
     *     To be added into key range list when range information is complete
     *     (seq event handler - config_read_seq())
     */
    if (user->key_seq_mode == CONFIG_KEY_SEQ_MODE_SINGLE) {
        SHR_IF_ERR_EXIT
            (config_key_attach(user, &(user->key_range)));
        config_key_range_init(user);
    }

exit:
    if (SHR_FUNC_ERR()) {
        config_key_range_init(user);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy configuration data from previous level to current level.
 *
 * \param [in] start        True if start, false if stop.
 * \param [in] context      Reader context.
 * \param [in] user         Property data.
 *
 * \retval None
 */
static void
config_read_copy(bool start,
                 bcmcfg_read_context_t *context,
                 config_tbl_user_t *user)
{
    config_stk_info_t *cur_tbl_info;
    config_stk_info_t *prev_tbl_info;

    if (start) {
        /* copy previous user data. */
        cur_tbl_info = user->info + context->level;
        prev_tbl_info = user->info + context->level - 1;
        sal_memcpy(cur_tbl_info, prev_tbl_info, sizeof(config_stk_info_t));
    }

    return;
}

/*!
 * \brief Manage table state transition.
 *
 * \param [in] user         Configuration user data.
 * \param [in] info         Configuration stack information.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static void
config_table_state_next(config_tbl_user_t *user,
                        config_stk_info_t *info)
{
    config_tbl_state_t prev_state;

    prev_state = info->state;

    switch (info->state) {
    case UNIT:
        info->state = TABLE;
        break;

    case TABLE:
        if (user->type == CONFIG_TBL_GLOBAL) {
            info->state = FIELD;
        } else {
            info->state = KEY;
        }
        break;

    case KEY:
        info->state = FIELD;
        break;

    default:
        break;
    }

    if (prev_state != info->state) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Map Read - Tbl_state change: (%d, %d)\n"),
                    prev_state, info->state));
    }

    return;
}

/*!
 * \brief Setup property reader.
 *
 * \param [in] start        True if start, false if stop.
 * \param [in] user_data    Handler user data.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_setup(bool start,
                  bcmcfg_parse_info_t *info,
                  void *user_data)
{
    config_tbl_user_t *user = (config_tbl_user_t *)user_data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    COMPILER_REFERENCE(start);
    SHR_NULL_CHECK(user, SHR_E_PARAM);

    if (info) {
        sal_memcpy(&config_parse_info, info, sizeof(bcmcfg_parse_info_t));
    }

    if (!CONFIG_MODE_VALID) {
        SHR_EXIT();
    }

    SHR_FREE(user->fld_name);
    SHR_FREE(user->key_range_list);
    sal_memset(user, 0, sizeof(config_tbl_user_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handler YAML scalar event.
 *
 * \param [in] value        Scalar value.
 * \param [in] context      Reader context.
 * \param [in] user_data    Handler user data.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_scalar(const char *value,
                   bcmcfg_read_context_t *context,
                   void *user_data)
{
    bcmcfg_read_level_info_t *info = NULL;
    config_tbl_user_t *user = NULL;
    config_stk_info_t *tbl_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(context, SHR_E_PARAM);
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (!CONFIG_MODE_VALID) {
        SHR_EXIT();
    }

    info = context->info + context->level;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("Scalar Read - State: (%d.%d)\n"),
                context->level, info->state));

    user = (config_tbl_user_t *)user_data;
    tbl_info = user->info + context->level;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("Scalar Read - table state %d, scalar value %s\n"),
                tbl_info->state, value));

    switch (tbl_info->state) {
    case UNIT:
        /* Unit value. */
        SHR_IF_ERR_EXIT
            (config_read_add_unit_str(user, value));
        break;

    case TABLE:
        /* Table value. */
        if (!sal_strcmp(value, config_tbl_type_str[CONFIG_TBL_GLOBAL])) {
            user->type = CONFIG_TBL_GLOBAL;
        } else if (!sal_strcmp(value, config_tbl_type_str[CONFIG_TBL_PORT])) {
            user->type = CONFIG_TBL_PORT;
            /* Initialize current key information. */
            config_key_range_init(user);
            config_key_range_list_init(user);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("%s:%d:%d: Property: unsupported table %s.\n"),
                      info->locus, info->line, info->column, value));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        break;

    case KEY:
        /* Key value. */
        SHR_IF_ERR_EXIT
            (config_key_add(info, user, value));
        break;

    case FIELD:
        /* Field value. */
        SHR_IF_ERR_EXIT
            (config_field_handle(info, user, value));
        break;

    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map event.
 *
 * \param [in] start        True if start, false if stop.
 * \param [in] context      Reader context.
 * \param [in] user_data    Handler user data.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
static int
config_read_map(bool start,
                bcmcfg_read_context_t *context,
                void *user_data)
{
    bcmcfg_read_level_info_t *info = NULL;
    bcmcfg_read_level_info_t *prev = NULL;
    config_tbl_user_t *user = NULL;
    config_stk_info_t *tbl_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(context, SHR_E_PARAM);
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (!CONFIG_MODE_VALID) {
        SHR_EXIT();
    }

    info = context->info + context->level;
    prev = context->info + context->level - 1;

    user = (config_tbl_user_t *)user_data;
    config_read_copy(start, context, user);
    tbl_info = user->info + context->level;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("Map Read - State: (%d.%d, %d.%d) Tbl_state: %d\n"),
                context->level - 1, prev->state,
                context->level, info->state, tbl_info->state));

    if (start) {
        /* Try to transit configuration table state. */
        config_table_state_next(user, tbl_info);
    } else {
        /* Try to transit configuration table state. */
        switch (tbl_info->state) {
        case FIELD:
            /* Reset key range list when exiting FIELD state. */
            if (user->type != CONFIG_TBL_GLOBAL) {
                config_key_range_list_init(user);
            }
            break;

        case TABLE:
            /* Reset unit set when exiting TABLE state. */
            config_unit_init(user);
            break;

        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML sequence event.
 *
 * \param [in] start       True if start, false if stop.
 * \param [in] context     Reader context.
 * \param [in] user_data   Handler user data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
config_read_seq(bool start,
                bcmcfg_read_context_t *context,
                void *user_data)
{
    bcmcfg_read_level_info_t *info = NULL;
    bcmcfg_read_level_info_t *prev = NULL;
    config_tbl_user_t *user = NULL;
    config_stk_info_t *tbl_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(context, SHR_E_PARAM);
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (!CONFIG_MODE_VALID) {
        SHR_EXIT();
    }

    info = context->info + context->level;
    prev = context->info + context->level - 1;

    user = (config_tbl_user_t *)user_data;
    config_read_copy(start, context, user);
    tbl_info = user->info + context->level;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("Seq Read - State: (%d.%d, %d.%d) Tbl_state: %d\n"),
                context->level - 1, prev->state,
                context->level, info->state, tbl_info->state));

    /* Handle key range mode within 2nd square brackets*/
    if ((tbl_info->state == KEY) && (prev->state == BCMCFG_READ_STATE_SEQ)) {
        if (start) {
            if (user->key_seq_mode == CONFIG_KEY_SEQ_MODE_RANGE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("%s:%d:%d Too many square brackets > 2.\n"),
                          info->locus, info->line, info->column));
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            } else {
                user->key_seq_mode = CONFIG_KEY_SEQ_MODE_RANGE;
            }
        } else {
            /* Insert key range field entries when exiting key range mode. */
            SHR_IF_ERR_EXIT
                (config_key_attach(user, &(user->key_range)));
            config_key_range_init(user);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML doc event.
 *
 * \param [in] start       True if start, false if stop.
 * \param [in] context     Reader context.
 * \param [in] user_data   Handler user data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
config_read_doc(bool start,
                bcmcfg_read_context_t *context,
                void *user_data)
{
    config_tbl_user_t *user = (config_tbl_user_t *)user_data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(context, SHR_E_PARAM);
    SHR_NULL_CHECK(user, SHR_E_PARAM);

    if (!CONFIG_MODE_VALID) {
        SHR_EXIT();
    }

    if (start) {
        SHR_FREE(user->fld_name);
        SHR_FREE(user->key_range_list);
        sal_memset(user, 0, sizeof(config_tbl_user_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle abort notification.
 *
 * \param [in] context     Reader context.
 * \param [in] user_data   Handler user data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
config_read_abort(bcmcfg_read_context_t *context,
                  void *user_data)
{
    config_tbl_user_t *user = (config_tbl_user_t *)user_data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(context, SHR_E_PARAM);
    SHR_NULL_CHECK(user, SHR_E_PARAM);

    if (!CONFIG_MODE_VALID) {
        SHR_EXIT();
    }

    /* Free dynamic memory. */
    SHR_FREE(user->fld_name);
    SHR_FREE(user->key_range_list);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize a TLV message structure.
 *
 * \param [in] msg    Pointer to TLV message.
 *
 * \retval None
 */
static void
config_tlv_msg_t_init(config_tlv_msg_t *msg)
{
    if (msg != NULL) {
        sal_memset(msg, 0, sizeof(config_tlv_msg_t));
    }

    return;
}

/*!
 * \brief Init TLV message buffer.
 *
 * \param [out] msg       Pointer to TLV message.
 * \param [in] buffer     Pointer to buffer.
 * \param [in] length     Buffer size.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
config_tlv_msg_buffer_init(config_tlv_msg_t *msg, uint8_t *buffer, int length)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(msg, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);

    /* Set pointers to buffer */
    msg->start = msg->end = msg->cur_ptr = buffer;
    msg->buffer_end = buffer + length;
    msg->tlv_length_ptr = NULL;
    msg->tlv_length = 0;
    msg->tlv_left = 0;

exit:
   SHR_FUNC_EXIT();
}


/*!
 * \brief Checks if there is available space in message for given value size.
 *
 * \param [in] msg        Pointer to TLV message.
 * \param [in] size       Size in bytes.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
config_tlv_msg_length_check(config_tlv_msg_t *msg, int size)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Add End-TLV size (only once) */
    if (msg->start == msg->end) {
        size += CONFIG_TLV_MSG_END_SIZE;
    }

    if ((msg->buffer_end - msg->end) < size) {
         SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a new TLV to current message object.
 *
 * \param [in] msg        Pointer to TLV message.
 * \param [in] type       Message type.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
config_tlv_msg_add(config_tlv_msg_t *msg, config_tlv_msg_type_t type)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(msg, SHR_E_PARAM);

    /* Check available space. */
    SHR_IF_ERR_EXIT
        (config_tlv_msg_length_check(msg, sizeof(uint8_t) + sizeof(uint16_t)));

    /* Set TLV type. */
    *((uint8_t *)(msg->cur_ptr)) = type;
    msg->cur_ptr += sizeof(uint8_t);

    /* Set TLV length which will be overwritten by value later. */
    msg->tlv_length_ptr = msg->cur_ptr;
    msg->tlv_length = 0;
    sal_memcpy((void *)msg->cur_ptr,
               (void *)&msg->tlv_length, sizeof(uint16_t));
    msg->cur_ptr += sizeof(uint16_t);

    /* Set End-TLV wich will be overwritten by subsequent TLV msg. */
    *((uint8_t *)(msg->cur_ptr)) = CONFIG_TLV_MSG_TYPE_END;
    msg->end = msg->cur_ptr;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Gets next TLV type and length from given message object.
 *
 * \param [in]  msg       Pointer to TLV message.
 * \param [out] type      Message type.
 * \param [out] length    Message length.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
config_tlv_msg_get(config_tlv_msg_t *msg,
                   config_tlv_msg_type_t *type,
                   config_tlv_msg_length_t *length)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(msg, SHR_E_PARAM);
    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if (msg->tlv_left != 0) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    msg->tlv_length_ptr = NULL;
    msg->tlv_length = 0;
    msg->tlv_left = 0;

    /* Get TLV type. */
    *type = *((uint8_t *)msg->cur_ptr);
    msg->cur_ptr += sizeof(uint8_t);
    if (!CONFIG_TLV_MSG_TYPE_VALID(*type)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get TLV length. */
    msg->tlv_length_ptr = msg->cur_ptr;
    sal_memcpy((void *)&msg->tlv_length,
               (void *)msg->cur_ptr, sizeof(uint16_t));
    msg->cur_ptr += sizeof(uint16_t);

    msg->tlv_left = msg->tlv_length;
    if (length != NULL) {
        *length = msg->tlv_length;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add string data in TLV message.
 *
 * \param [in] msg        Pointer to TLV message.
 * \param [in] value      String value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
config_tlv_msg_string_value_add(config_tlv_msg_t *msg, const char *value)
{
    int value_size = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(msg, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    value_size = sal_strlen(value) + 1;

    /* Check available space. */
    SHR_IF_ERR_EXIT
        (config_tlv_msg_length_check(msg, value_size));

    /* Write string value. */
    sal_memcpy((char *)msg->cur_ptr, value, value_size);
    msg->cur_ptr += value_size;

    /* Update TLV length */
    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_length += value_size;
        sal_memcpy((void *)msg->tlv_length_ptr,
                   (void *)&msg->tlv_length, sizeof(uint16_t));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Set End-TLV wich will be overwritten by subsequent TLV msg. */
    *((uint8_t *)(msg->cur_ptr)) = CONFIG_TLV_MSG_TYPE_END;
    msg->end = msg->cur_ptr;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get string data from TLV message.
 *
 * Get string data from TLV message.
 *
 * \param [in]  msg       Pointer to TLV message.
 * \param [in]  value_max Max size of returning value.
 * \param [out] value     Data value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
config_tlv_msg_string_value_get(config_tlv_msg_t *msg,
                                int value_max,
                                char *value)
{
    int length_left = 0;
    int value_size = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(msg, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    if ((msg->cur_ptr + CONFIG_TLV_MSG_END_SIZE) >= msg->buffer_end) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get unread number of bytes in TLV. */
    if (msg->tlv_length_ptr != NULL) {
        length_left = msg->tlv_left;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (length_left <= 0) {
        /* No more data to read */
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    value_size = sal_strlen((const char *)msg->cur_ptr) + 1;

    if (length_left < value_size) {
        /* Bytes left to read smaller than size */
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    /* Check if value fits */
    if (value_max < value_size) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Get string value. */
    sal_memcpy((char *)value, (char *)msg->cur_ptr, value_size);
    msg->cur_ptr += value_size;

    msg->tlv_left -= value_size;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add configuation field information into HA with TLV format.
 *
 * \param [in] unit       Unit number.
 * \param [in] type       TLV Message type.
 * \param [in] msg        Pointer to TLV message.
 * \param [in] str        String value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
config_ha_field_add(int unit,
                    config_tlv_msg_type_t type,
                    config_tlv_msg_t *msg,
                    const char *str)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (config_tlv_msg_add(msg, type));

    SHR_IF_ERR_EXIT
        (config_tlv_msg_string_value_add(msg, str));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get configuation field information from HA with TLV format.
 *
 * \param [in] unit       Unit number.
 * \param [in] type       TLV Message type.
 * \param [in] msg        Pointer to TLV message.
 * \param [out]str        String value.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
config_ha_field_get(int unit,
                    config_tlv_msg_type_t *type,
                    config_tlv_msg_t *msg,
                    char *str)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (config_tlv_msg_get(msg, type, NULL));

    SHR_IF_ERR_EXIT
        (config_tlv_msg_string_value_get(msg, CONFIG_NAME_MAX, str));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get configuration HA data size.
 *
 * \param [in] unit     Unit number.
 * \param [out] size    HA data size.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
config_ha_size_get(int unit, int *size)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *fld = NULL;
    int i = 0;
    int req_size = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < CONFIG_HASH_MAX_COUNT; i++) {

        for (fld = cd->fld[i]; fld != NULL; fld = fld->next) {
            req_size += sizeof(config_tlv_msg_type_t);
            req_size += sizeof(config_tlv_msg_length_t);
            req_size += sal_strlen(fld->name) + 1;

            req_size += sizeof(config_tlv_msg_type_t);
            req_size += sizeof(config_tlv_msg_length_t);
            req_size += sal_strlen(fld->value) + 1;
        }
    }

    req_size += CONFIG_TLV_MSG_END_SIZE;

    *size = req_size;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Save configuration data into HA with TLV format.
 *
 * \param [in] unit     Device unit number.
 * \param [in] tlv_msg  Pointer to TLV message.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
config_ha_save(int unit, config_tlv_msg_t *tlv_msg)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *fld = NULL;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < CONFIG_HASH_MAX_COUNT; i++) {

        for (fld = cd->fld[i]; fld != NULL; fld = fld->next) {

            SHR_IF_ERR_EXIT
                (config_ha_field_add(unit, CONFIG_TLV_MSG_TYPE_FLD_NAME,
                                     tlv_msg, fld->name));

            SHR_IF_ERR_EXIT
                (config_ha_field_add(unit, CONFIG_TLV_MSG_TYPE_FLD_VALUE,
                                     tlv_msg, fld->value));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore configuration data from HA with TLV format.
 *
 * \param [in] unit     Device unit number.
 * \param [in] tlv_msg  Pointer to TLV message.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
config_ha_restore(int unit, config_tlv_msg_t *tlv_msg)
{
    char name[CONFIG_NAME_MAX] = {'\0'};
    char value[CONFIG_NAME_MAX] = {'\0'};
    config_tlv_msg_type_t type;

    SHR_FUNC_ENTER(unit);

    while ((tlv_msg->cur_ptr + CONFIG_TLV_MSG_END_SIZE) < tlv_msg->buffer_end) {

        SHR_IF_ERR_EXIT
            (config_ha_field_get(unit, &type, tlv_msg , name));

        if (type != CONFIG_TLV_MSG_TYPE_FLD_NAME) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_EXIT
            (config_ha_field_get(unit, &type, tlv_msg, value));

        if (type != CONFIG_TLV_MSG_TYPE_FLD_VALUE) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_EXIT
           (config_field_insert(unit, name, value));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct port configuration database.
 *
 * \param [in] unit Device unit number.
 * \param [in] port Port id.
 * \param [in] name Field name.
 * \param [in] value Field value.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
config_port_db_construct(int unit, int port,
                         char *name, char *value)
{
    config_port_db_t *port_cfg = &config_port_db[unit];
    config_fld_t *found_fld = NULL;
    config_fld_t *fld = NULL;
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (port > CONFIG_PORT_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Try to find an existing configuration field. */
    found_fld = port_cfg->fld[port];

    while (found_fld != NULL) {
        if (sal_strcmp(found_fld->name, name) == 0) {
            break;
        }
        found_fld = found_fld->next;
    }

    if (found_fld != NULL) {
        /* Ignoring duplicate config field. */
        SHR_EXIT();
    }

    /* Insert new a configuration field. */
    alloc_size = sizeof(config_fld_t);
    SHR_ALLOC(fld, alloc_size, "ltswPortCfgData");
    SHR_NULL_CHECK(fld, SHR_E_MEMORY);
    sal_memset(fld, 0, alloc_size);

    alloc_size = sal_strlen(name) + 1;
    SHR_ALLOC(fld->name, alloc_size, "ltswPortCfgName");
    SHR_NULL_CHECK(fld->name, SHR_E_MEMORY);
    sal_memcpy(fld->name, name, alloc_size);

    alloc_size = sal_strlen(value) + 1;
    SHR_ALLOC(fld->value, alloc_size, "ltswPortCfgValue");
    SHR_NULL_CHECK(fld->value, SHR_E_MEMORY);
    sal_memcpy(fld->value, value, alloc_size);

    fld->next = port_cfg->fld[port];
    port_cfg->fld[port] = fld;

exit:
    if (SHR_FUNC_ERR() && (fld != NULL)) {
        SHR_FREE(fld->name);
        SHR_FREE(fld->value);
        SHR_FREE(fld);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all configuration property fields.
 *
 * \param [in] unit Unit number.
 *
 * \return None.
 */
static void
config_port_db_delete_all(int unit)
{
    config_port_db_t *port_cfg = &config_port_db[unit];
    config_fld_t *fld = NULL;
    int i;

    for (i = 0; i < CONFIG_PORT_MAX + 1; i++) {
       while (port_cfg->fld[i] != NULL) {
           fld = port_cfg->fld[i];
           port_cfg->fld[i] = fld->next;
           SHR_FREE(fld->name);
           SHR_FREE(fld->value);
           SHR_FREE(fld);
       }
    }

    return;
}

/*!
 * \brief Traverse port configuration database.
 *
 * Traverse port configuration and print it if any.
 *
 * \param [in] unit Device unit number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
config_port_db_traverse(int unit)
{
    config_port_db_t *port_cfg = &config_port_db[unit];
    config_fld_t *fld = NULL;
    int i, indent = 8;
    bool hp = false, pip = false;

    for (i = 0; i < CONFIG_PORT_MAX + 1; i++) {
        pip = false;
        while (port_cfg->fld[i] != NULL) {
            fld = port_cfg->fld[i];
            port_cfg->fld[i] = fld->next;
            /* Print port header. */
            if (!hp) {
                cli_out("%*s%s:\n", indent, "", "port");
                hp = true;
            }

            /* Print port id. */
            if (!pip) {
                if (i == CONFIG_PORT_MAX) {
                    cli_out("%*s%s:\n", indent + 4, "", "\"*\"");
                } else {
                    cli_out("%*s%d:\n", indent + 4, "", i);
                }
                pip= true;
            }

            /* Print port config. */
            cli_out("%*s%s: %s\n", indent + 8, "", fld->name, fld->value);
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Traverse configuration database.
 *
 * During the traverse, the global configuration will be shown and
 * port database will be constructed.
 *
 * \param [in] unit Device unit number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
config_db_traverse(int unit)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *fld = NULL;
    int i = 0, port = 0, len, indent = 8;
    char find_name[CONFIG_NAME_MAX];
    char name[CONFIG_NAME_MAX];
    char value[CONFIG_NAME_MAX];
    bool gp = false;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < CONFIG_HASH_MAX_COUNT; i++) {
        for (fld = cd->fld[i]; fld != NULL; fld = fld->next) {
            SHR_IF_ERR_VERBOSE_EXIT
                (config_field_name_translate(CONFIG_TBL_PORT, true, port,
                                             NULL, find_name));
            /* Search '_port_*' in field name. */
            if (sal_strstr(fld->name, find_name)) {
                len = sal_strlen(fld->name) - sal_strlen(find_name);
                sal_memcpy(name, fld->name, len);
                name[len] =  '\0';
                sal_memcpy(value, fld->value, CONFIG_NAME_MAX);

                /* Construct port config database. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (config_port_db_construct(unit, CONFIG_PORT_MAX,
                                              name, value));
                continue;
            }

            for (port = CONFIG_PORT_MAX - 1; port >= 0; port--) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (config_field_name_translate(CONFIG_TBL_PORT, false, port,
                                                 NULL, find_name));
                /* Search '_port_i' in field name. */
                if (sal_strstr(fld->name, find_name)) {
                    len = sal_strlen(fld->name) - sal_strlen(find_name);
                    sal_memcpy(name, fld->name, len);
                    name[len] =  '\0';
                    sal_memcpy(value, fld->value, CONFIG_NAME_MAX);

                    /* Construct port config database. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (config_port_db_construct(unit, port, name, value));
                    break;
                }
            }
            if (port >= 0) {
                continue;
            }

            /* Print global header. */
            if (!gp) {
                cli_out("%*s%s:\n", indent, "", "global");
                gp = true;
            }
            /* Print global config. */
            cli_out("%*s%s: %s\n", indent + 4, "", fld->name, fld->value);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public internal HSDK functions
 */

char *
soc_ltsw_config_str_get(int unit, const char *name)
{
    config_fld_t *fld = NULL;

    if (!config_info.inited) {
        return NULL;
    }

    if (name != NULL) {
        fld = config_field_find(unit, name);

        if (fld != NULL) {
            return fld->value;
        }
    }

    return NULL;
}

char *
soc_ltsw_config_port_str_get(int unit, int port, const char *name)
{
    char find_name[CONFIG_NAME_MAX];
    char *str = NULL;

    if (!config_info.inited) {
        return NULL;
    }

    if (name == NULL) {
        return NULL;
    }

    if (SHR_FAILURE
            (config_field_name_translate(CONFIG_TBL_PORT, true,
                                         0, name, find_name))) {
        return NULL;
    }

    if ((str = soc_ltsw_config_str_get(unit, find_name)) != NULL) {
        return str;
    }

    if (SHR_FAILURE
            (config_field_name_translate(CONFIG_TBL_PORT, false,
                                         port, name, find_name))) {
        return NULL;
    }

    if ((str = soc_ltsw_config_str_get(unit, find_name)) != NULL) {
        return str;
    }

    return NULL;
}

int
soc_ltsw_config_str_set(int unit, const char *name, const char *value)
{
    SHR_FUNC_ENTER(unit);

    if (!config_info.inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
       (config_field_insert(unit, name, value));

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_config_port_str_set(int unit,
                             int port,
                             const char *name,
                             const char *value)
{
    bool asterisk;
    char cfg_name[CONFIG_NAME_MAX];

    SHR_FUNC_ENTER(unit);

    if (!config_info.inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    asterisk = (port == -1) ? true : false;

    SHR_IF_ERR_EXIT
        (config_field_name_translate(CONFIG_TBL_PORT, asterisk,
                                     port, name, cfg_name));

    SHR_IF_ERR_EXIT
        (config_field_insert(unit, cfg_name, value));

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_config_init(void)
{
    int unit = 0;
    config_db_t *cd = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (config_info.inited) {
        config_info_cleanup();
    }

    for (unit = 0; unit < CONFIG_UNIT_MAX; unit++) {
        cd = &(config_info.config_db[unit]);
        if (cd->mutex== NULL) {
            cd->mutex = sal_mutex_create("ltswConfigMutex");
            SHR_NULL_CHECK(cd->mutex, SHR_E_MEMORY);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_read_handler_register(&config_read_hdl));

    config_info.inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        config_info_cleanup();
    }
    SHR_FUNC_EXIT();
}

int
soc_ltsw_config_ha_save(int unit, void *buffer, int size)
{
    config_tlv_msg_t tlv_msg;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);

    config_tlv_msg_t_init(&tlv_msg);

    SHR_IF_ERR_EXIT
        (config_tlv_msg_buffer_init(&tlv_msg, (uint8_t *)buffer, size));

    SHR_IF_ERR_EXIT
        (config_ha_save(unit, &tlv_msg));

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_config_ha_restore(int unit, void *buffer, int size)
{
    config_tlv_msg_t tlv_msg;

    SHR_FUNC_ENTER(unit);

    config_tlv_msg_t_init(&tlv_msg);

    SHR_IF_ERR_EXIT
        (config_tlv_msg_buffer_init(&tlv_msg, (uint8_t *)buffer, size));

    SHR_IF_ERR_EXIT
        (config_ha_restore(unit, &tlv_msg));

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_config_ha_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (config_ha_size_get(unit, size));

exit:
    SHR_FUNC_EXIT();
}

void
soc_ltsw_config_sw_dump(int unit)
{
    config_db_t *cd = &(config_info.config_db[unit]);
    config_fld_t *fld = NULL;
    int i = 0;

    if (!config_info.inited) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information CONFIG - Dunit %d\n"), unit));

    for (i = 0; i < CONFIG_HASH_MAX_COUNT; i++) {
        for (fld = cd->fld[i]; fld != NULL; fld = fld->next) {
            LOG_CLI((BSL_META_U(unit,
                                "Config Property: hash %4d, %s = %s\n"),
                                i, fld->name, fld->value));
        }
    }

    return;
}

void
soc_ltsw_show_config(int unit)
{
    int indent = 4;

    /* Print header. */
    cli_out("---\n");
    cli_out("bcm_device:\n");
    cli_out("%*s%d:\n", indent, "", unit);

    CONFIG_LOCK(unit);

    (void)config_db_traverse(unit);

    (void)config_port_db_traverse(unit);

    cli_out("...\n");
    config_port_db_delete_all(unit);

    CONFIG_UNLOCK(unit);
}
