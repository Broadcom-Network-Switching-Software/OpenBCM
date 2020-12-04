/*! \file bcma_tecsupcmd_tecsup.c
 *
 * CLI 'techsupport' command handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/tecsup/bcma_tecsup_header.h>
#include <bcma/tecsup/bcma_tecsup_drv.h>
#include <bcma/tecsup/bcma_tecsupcmd_tecsup.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TECSUP

/*******************************************************************************
 * Local defines
 */

/* Default value */
#define TECSUP_PATH_DEFAULT "./script"
#define TECSUP_FEATURE_WILDCARD "*"
#define TECSUP_FEATURE_NUM_MAX 64

/* Command operating mode */
typedef enum tecsup_mode_e {
    TECSUP_MODE_BUILTIN = 0,
    TECSUP_MODE_SCRIPT,
    TECSUP_MODE_COUNT
} tecsup_mode_t;

/* Dump flag */
typedef enum tecsup_dump_flag_e {
    TECSUP_DT_LIST = 0,
    TECSUP_DF_COUNT
} tecsup_dump_flag_t;

/* Operating name and value mapping */
static bcma_cli_parse_enum_t tecsup_mode_map[] = {
    { "builtin", TECSUP_MODE_BUILTIN },
    { "script", TECSUP_MODE_SCRIPT },
    { NULL, 0 }
};

/* Techsupport configurations */
static bcma_tecsup_cfg_t tecsup_cfg;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get enum name from value.
 *
 * \param [in] map Enum map.
 * \param [in] val Enum value.
 *
 * \return The enum name or NULL when not found.
 */
static const char *
tecsup_enum_name_get(bcma_cli_parse_enum_t *map, int val)
{
    int idx = 0;

    while (map[idx].name) {
        if (val == map[idx].val) {
            return  map[idx].name;
        }
        idx++;
    }

    return NULL;
}

/*!
 * \brief Get the tecsup driver.
 *
 * \param [in] mode Operating mode.
 *
 * \return The tecsup driver.
 */
static bcma_tecsup_drv_t *
tecsup_drv_get(int mode)
{
    if (mode == TECSUP_MODE_BUILTIN) {
        return bcma_tecsup_drv_builtin_get();
    } else if (mode == TECSUP_MODE_SCRIPT) {
        return bcma_tecsup_drv_script_get();
    } else {
        return NULL;
    }
}

/*!
 * \brief To search if a string is in the given list.
 *
 * \param [in] list List of strings.
 * \param [in] str String to lookup.
 *
 * \return The index of str in the list, or -1 if not found.
 */
static int
tecsup_str_lookup(bcma_tecsup_str_t *list, const char *str)
{
    int idx = 0;

    /* Assume that the list is terminatd with an empty string */
    while (sal_strlen(list[idx].str)) {
        if (sal_strcmp(list[idx].str, str) == 0) {
            return idx;
        }
        idx++;
    }

    return -1;
}

/*!
 * \brief Show command string.
 *
 * \param [in] cmd CLI command string.
 * \param [in] cookie User data.
 */
static void
tecsup_cmd_print(const char *cmd, void *cookie)
{
    cli_out("    %s\n", cmd);
    return;
}

/*!
 * \brief Execute CLI command.
 *
 * \param [in] cmd CLI command string.
 * \param [in] cookie Caller's bcma_cli_t instance.
 */
static void
tecsup_cmd_process(const char *cmd, void *cookie)
{
    bcma_cli_t *cli = cookie;

    bcma_cli_cmd_process(cli, cmd);
    return;
}

/*!
 * \brief Execute the dump commands.
 *
 * \param [in] drv Operating driver.
 * \param [in] feature debug feature name.
 * \param [in] cli CLI object.
 * \param [in] cflags Debug categories.
 * \param [in] dflags Dump flags.
 *
 * \retval 0 No errors.
 * \retval -1 Fail to execute the script.
 */
static int
tecsup_feature_exec(bcma_tecsup_drv_t *drv, const char *feature,
                    bcma_cli_t *cli, bcma_tecsup_str_t *cat_names,
                    uint32_t *dflags)
{
    int unit = cli->cmd_unit;
    int rv, opt_list;
    bcma_tecsup_db_t db;
    bcma_tecsup_str_t *cat_name;

    opt_list = SHR_BITGET(dflags, TECSUP_DT_LIST);

    db = drv->create("bcmaTecsupDB");
    if (!db) {
        cli_out("%sFailed to create database.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    rv = drv->load(unit, feature, &tecsup_cfg, cat_names, db);
    if (rv < 0) {
        cli_out("%sFeature \"%s\" is not supported.\n",
                BCMA_CLI_CONFIG_ERROR_STR, feature);
        drv->destroy(db);
        return -1;
    }

    cli_out("\nFeature: %s\n", feature);
    cat_name = cat_names;
    while (sal_strlen(cat_name->str)) {
        cli_out("\n%s:\n", cat_name->str);

        if (opt_list) {
            rv = drv->traverse(db, cat_name->str, tecsup_cmd_print, NULL);
        } else {
            rv = drv->traverse(db, cat_name->str, tecsup_cmd_process,
                               (void *)cli);
        }
        if (rv < 0) {
            cli_out("    -\n");
        }
        cat_name++;
    }

    drv->destroy(db);
    return 0;
}

/*!
 * \brief Show tecsup configuration.
 *
 * \param [in] cli CLI object.
 *
 * \retval BCMA_CLI_CMD_OK No errors.
 * \retval BCMA_CLI_CMD_FAIL Fail to show the configuration.
 */
static int
tecsup_config_show(bcma_cli_t *cli)
{
    int idx, cnt, sz;
    int unit = cli->cmd_unit;
    const char *mode_name;
    bcma_tecsup_drv_t *drv;
    bcma_tecsup_str_t *list = NULL;

    sz = sizeof(bcma_tecsup_str_t) * TECSUP_FEATURE_NUM_MAX;
    list = sal_alloc(sz, "bcmaTecsupList");
    if (!list) {
        cli_out("%sFailed to allocate memory\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("Configuration:\n");
    mode_name = tecsup_enum_name_get(tecsup_mode_map, tecsup_cfg.mode);
    cli_out("  Operating mode: %s\n", mode_name ? mode_name : "");
    cli_out("  Script path: %s\n", tecsup_cfg.script_path);

    drv = tecsup_drv_get(tecsup_cfg.mode);
    if (!drv) {
        cli_out("%sUnknown operating mode: %d.\n",
                BCMA_CLI_CONFIG_ERROR_STR, tecsup_cfg.mode);
        sal_free(list);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("Categories:\n");
    cnt = 0;
    sal_memset(list, 0, sz);
    drv->category_list_get(unit, &tecsup_cfg, TECSUP_FEATURE_NUM_MAX,
                           list, &cnt);
    if (cnt == 0) {
        cli_out("  No category is available by this device.\n");
    } else {
        for (idx = 0; idx < cnt; idx++) {
            cli_out("  %s\n", list[idx].str);
        }
    }

    cli_out("Features:\n");
    cnt = 0;
    sal_memset(list, 0, sz);
    drv->feature_list_get(unit, &tecsup_cfg, TECSUP_FEATURE_NUM_MAX,
                          list, &cnt);
    if (cnt == 0) {
        cli_out("  No feature is available by this device.\n");
    } else {
        for (idx = 0; idx < cnt; idx++) {
            cli_out("  %s\n", list[idx].str);
        }
    }

    sal_free(list);
    return BCMA_CLI_CMD_OK;
}

static int
tecsup_feature_dump(bcma_cli_t *cli, const char *feature, bool list_only,
                    bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *arg;
    int idx, sz, cnt, unit = cli->cmd_unit;
    SHR_BITDCLNAME(dflags, TECSUP_DF_COUNT);
    SHR_BITDCLNAME(cflags, TECSUP_FEATURE_NUM_MAX);
    shr_pb_t *pb;
    bcma_tecsup_drv_t *drv;
    bcma_tecsup_str_t *feature_list = NULL, *category_list = NULL;

    drv = tecsup_drv_get(tecsup_cfg.mode);
    if (!drv) {
        cli_out("%sUnknown operating mode: %d.\n",
                BCMA_CLI_CONFIG_ERROR_STR, tecsup_cfg.mode);
        return BCMA_CLI_CMD_FAIL;
    }

    sz = sizeof(bcma_tecsup_str_t) * TECSUP_FEATURE_NUM_MAX;
    feature_list = sal_alloc(sz, "bcmaTecsupFeatList");
    category_list = sal_alloc(sz, "bcmaTecsupCateList");
    if (!feature_list | !category_list) {
        cli_out("%sFailed to allocate memory\n", BCMA_CLI_CONFIG_ERROR_STR);
        if (feature_list) {
            sal_free(feature_list);
        }
        if (category_list) {
            sal_free(category_list);
        }
        return BCMA_CLI_CMD_FAIL;
    }

    cnt = 0;
    sal_memset(category_list, 0, sz);
    drv->category_list_get(unit, &tecsup_cfg,
                           TECSUP_FEATURE_NUM_MAX - 1, category_list,
                           &cnt);

    /* Parse options */
    SHR_BITCLR_RANGE(cflags, 0, TECSUP_FEATURE_NUM_MAX);
    while (1) {
        arg = BCMA_CLI_ARG_GET(args);
        if (!arg) {
            break;
        }

        if ((idx = tecsup_str_lookup(category_list, arg)) >= 0) {
            SHR_BITSET(cflags, idx);
        } else {
            cli_out("%sCategory \"%s\" is not supported.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            rv = BCMA_CLI_CMD_FAIL;
            break;
        }
    }

    if (rv == BCMA_CLI_CMD_OK) {
        /* Set display flags */
        SHR_BITCLR_RANGE(dflags, 0, TECSUP_DF_COUNT);
        if (list_only) {
            SHR_BITSET(dflags, TECSUP_DT_LIST);
        }

        /* Get the categories to be dumped */
        if (!SHR_BITNULL_RANGE(cflags, 0, TECSUP_FEATURE_NUM_MAX)) {
            cnt = 0;
            for (idx = 0; idx < TECSUP_FEATURE_NUM_MAX; idx++) {
                if (SHR_BITGET(cflags, idx)) {
                    sal_memcpy(&category_list[cnt], &category_list[idx],
                               sizeof(category_list[cnt]));
                    cnt++;
                }
            }
            /* Use an element with empty string as the end of list */
            sal_memset(&category_list[cnt], 0, sizeof(category_list[0]));
        }

        /* Get features to be dumped */
        sal_memset(feature_list, 0, sz);
        if (sal_strcmp(feature, TECSUP_FEATURE_WILDCARD) == 0) {
            cnt = 0;
            drv->feature_list_get(unit, &tecsup_cfg,
                                  TECSUP_FEATURE_NUM_MAX, feature_list, &cnt);
        } else {
            sal_strlcpy(feature_list[0].str, feature,
                        sizeof(feature_list[0].str));
            cnt = 1;
        }

        /* Show welcome message */
        pb = shr_pb_create();
        bcma_tecsup_header(unit, pb);
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);

        /* Start to dump the information */
        for (idx = 0; idx < cnt; idx++) {
            tecsup_feature_exec(drv, feature_list[idx].str,
                                cli, category_list, dflags);
        }
    }

    if (feature_list) {
        sal_free(feature_list);
    }
    if (category_list) {
        sal_free(category_list);
    }

    return rv;
}


/*******************************************************************************
 * Sub-command handlers
 */

/* Sub-command handler for "config" */
static int
tecsup_cmd_config(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int *mode = &tecsup_cfg.mode;
    char *path = NULL;
    bcma_cli_parse_table_t pt;

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return tecsup_config_show(cli);
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Mode", "enum", mode, tecsup_mode_map);
    bcma_cli_parse_table_add(&pt, "Path", "str", &path, NULL);

    rv = bcma_cli_parse_table_do_args(&pt, args);
    if (rv < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
    }

    if (path) {
        sal_strlcpy(tecsup_cfg.script_path, path, BCMA_TECSUP_STR_LEN_MAX);
    }

    bcma_cli_parse_table_done(&pt);
    return BCMA_CLI_CMD_OK;
}

/* Sub-command handler for "dump" */
static int
tecsup_cmd_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *feature;

    feature = BCMA_CLI_ARG_GET(args);
    if (feature == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    return tecsup_feature_dump(cli, feature, false, args);
}

/* Sub-command handler for "dump" */
static int
tecsup_cmd_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *feature;

    feature = BCMA_CLI_ARG_GET(args);
    if (feature == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    return tecsup_feature_dump(cli, feature, true, args);
}

/* Constant sub-command list. */
static bcma_cli_command_t tecsup_cmds[] = {
    { "ConFiG", tecsup_cmd_config },
    { "dump", tecsup_cmd_dump },
    { "list", tecsup_cmd_list },
};


/*******************************************************************************
 * tecsup command handler
 */

int
bcma_tecsupcmd_tecsup(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int idx;

    if (!tecsup_cfg.script_path) {
        /* Setup the default script path */
        tecsup_cfg.script_path = sal_alloc(BCMA_TECSUP_STR_LEN_MAX,
                                           "bcmaTecsupPath");
        if (!tecsup_cfg.script_path) {
            cli_out("%sFailed to allocate memory\n", BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }

        sal_memset(tecsup_cfg.script_path, 0, BCMA_TECSUP_STR_LEN_MAX);
        sal_strlcpy(tecsup_cfg.script_path, TECSUP_PATH_DEFAULT,
                    BCMA_TECSUP_STR_LEN_MAX);
    }

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0; idx < COUNTOF(tecsup_cmds); idx++) {
        if (bcma_cli_parse_cmp(tecsup_cmds[idx].name, arg, '\0')) {
            return (*tecsup_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}

int
bcma_tecsupcmd_tecsup_cleanup(bcma_cli_t *cli)
{
    if (tecsup_cfg.script_path) {
        sal_free(tecsup_cfg.script_path);
        tecsup_cfg.script_path = NULL;
    }

    return BCMA_CLI_CMD_OK;
}
