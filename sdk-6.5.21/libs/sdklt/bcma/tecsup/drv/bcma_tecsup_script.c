/*! \file bcma_tecsup_script.c
 *
 * Script mode driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_hash_str.h>

#include <bcmdrd/bcmdrd.h>

#include <bcmlrd/bcmlrd_conf.h>

#include <bcma/io/bcma_io_dir.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/tecsup/bcma_tecsup_drv.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TECSUP

/*******************************************************************************
 * Local defines
 */

#define TECSUP_ELEMENT_NUM_MAX 128
#define TECSUP_DIR_NUM_MAX 2
#define TECSUP_VARIANTS_DIR "variants"

/* Debug entry */
typedef struct tecsup_script_entry_s {
    char data[BCMA_TECSUP_STR_LEN_MAX];
    struct tecsup_script_entry_s *next;
} tecsup_script_entry_t;

/* Debug database */
typedef struct tecsup_script_tbl_s {
    char name[BCMA_TECSUP_STR_LEN_MAX];
    tecsup_script_entry_t entry_list;
    struct tecsup_script_tbl_s *next;
} tecsup_script_tbl_t;

/* Hash structure */
typedef struct tecsup_script_hash_s {
    shr_hash_str_hdl hdl;
    bcma_tecsup_str_t *key_buf;
    int buf_idx;
} tecsup_script_hash_t;

typedef tecsup_script_tbl_t tecsup_script_db_t;

/* Array of device variant specific data */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { #_va },

static struct {
    const char *name;
} list_variant[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate a hashed-string list.
 *
 * \param [in] num Number of elements.
 *
 * \returns Pointer to the hash or NULL.
 */
static tecsup_script_hash_t *
tecsup_script_hash_alloc(size_t num)
{
    tecsup_script_hash_t *hash;

    hash = sal_alloc(sizeof(*hash), "bcmaTecsupHash");
    if (!hash) {
        return NULL;
    }

    if (SHR_FAILURE(shr_hash_str_dict_alloc(num, &hash->hdl))) {
        sal_free(hash);
        return NULL;
    }

    hash->buf_idx = 0;
    hash->key_buf = sal_alloc(num * sizeof(*(hash->key_buf)),
                              "bcmaTecsupHashKeyBuf");
    if (!hash->key_buf) {
        shr_hash_str_dict_free(hash->hdl);
        sal_free(hash);
        return NULL;
    }

    return hash;
}

/*!
 * \brief Free a hashed-string list.
 *
 * \param [in] hash Hash string list.
 */
static void
tecsup_script_hash_free(tecsup_script_hash_t *hash)
{
    if (!hash) {
        return;
    }

    if (hash->key_buf) {
        sal_free(hash->key_buf);
    }
    shr_hash_str_dict_free(hash->hdl);
    sal_free(hash);
}

/*!
 * \brief Insert a string into the hashed-string list.
 *
 * \param [in] hash Hash string list.
 * \param [in] key String to insert.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Errors.
 */
static int
tecsup_script_hash_insert(tecsup_script_hash_t *hash, const char *key)
{
    int rv;
    size_t buf_sz;
    char *buf, *val;

    if (!hash) {
        return SHR_E_PARAM;
    }

    buf_sz = sizeof(*(hash->key_buf));
    if (sal_strlen(key) >= buf_sz) {
        return SHR_E_PARAM;
    }

    rv = shr_hash_str_dict_lookup(hash->hdl, key, (void**)&val);
    if (rv != SHR_E_NOT_FOUND) {
        return rv;
    }

    buf = hash->key_buf[hash->buf_idx].str;
    sal_strlcpy(buf, key, buf_sz);
    rv = shr_hash_str_dict_insert(hash->hdl, buf, buf);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    hash->buf_idx++;

    return SHR_E_NONE;
}

/*!
 * \brief Get all strings from the hashed-string list.
 *
 * \param [in] hash Hash string list.
 * \param [out] key Strings in the list.
 * \param [out] cnt Number of strings in the list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Errors.
 */
static int
tecsup_script_hash_keys_get(tecsup_script_hash_t *hash,
                            const bcma_tecsup_str_t **keys, int *cnt)
{
    if (!hash) {
        return SHR_E_FAIL;
    }

    *cnt = hash->buf_idx;
    *keys = hash->key_buf;

    return SHR_E_NONE;
}

/*!
 * \brief Get the variant name.
 *
 * \param [in] unit Unit number.
 *
 * \return The variant name or empty string for non-variant device.
 */
static const char *
tecsup_variant_name_get(int unit)
{
    bcmlrd_variant_t variant;

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return "";
    }

    return list_variant[variant].name;
}

/*!
 * \brief Get the directory paths to the device and variant scripts.
 *
 * \param [in] int Unit number.
 * \param [in] script_root Root path to the scripts.
 * \param [out] dlist Directory list to the scripts.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Errors.
 */
static int
tecsup_script_dir_list_get(int unit, const char *script_root,
                           tecsup_script_hash_t *dlist)
{
    const char *dev_type, *variant;
    char dpath[BCMA_TECSUP_STR_LEN_MAX], vpath[BCMA_TECSUP_STR_LEN_MAX];

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type_str(unit);
    if (sal_strlen(dev_type) == 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Get the path to the device-specific script */
    sal_snprintf(dpath, sizeof(dpath),
                 "%s/%s/", script_root, dev_type);
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_insert(dlist, dpath));

    variant = tecsup_variant_name_get(unit);
    if (sal_strlen(variant) == 0) {
        SHR_EXIT();
    }

    /* Get the path to the variant-specific script */
    sal_snprintf(vpath, sizeof(vpath),
                 "%s/%s/%s/%s/",
                 script_root, dev_type, TECSUP_VARIANTS_DIR, variant);
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_insert(dlist, vpath));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the given path is a directory.
 *
 * \param [in] path Path to check.
 *
 * \retval true The path is a directory.
 * \retval false The path is not a directory.
 */
static bool
tecsup_isdir(const char *path)
{
    bcma_io_dir_handle_t dh;

    dh = bcma_io_dir_open(path);
    if (!dh) {
        return false;
    }

    bcma_io_dir_close(dh);
    return true;
}

/*!
 * \brief Get the directory entry names.
 *
 * This funcion will retrieve the directory or file names under the given path.
 *
 * The entry name which is starting with "." will be ignored by this function.
 *
 * \param [in] path Path to lookup.
 * \param [in] dtype Entry type to get. True for directory, false for file.
 * \param [out] name_list Buffer to save the names.
 *
 * \retval 0 Success.
 * \retval -1 Failed to get the sub-directory names.
 */
static int
tecsup_dirent_get(const char *path, bool dtype, tecsup_script_hash_t *name_list)
{
    bcma_io_dir_handle_t dh;
    char dname[BCMA_TECSUP_STR_LEN_MAX];

    if (!path || !name_list) {
        return -1;
    }

    dh = bcma_io_dir_open(path);
    if (!dh) {
        return -1;
    }

    while (bcma_io_dir_read(dh, sizeof(dname), dname) == 0) {
        char subpath[BCMA_TECSUP_STR_LEN_MAX * 2];
        char *c = dname;

        /* Skip the entry which name is starting with "." */
        if (*c == '.') {
            continue;
        }

        /* Skip the unwantted entry */
        sal_snprintf(subpath, sizeof(subpath), "%s/%s", path, dname);
        if (dtype != tecsup_isdir(subpath)) {
            continue;
        }

        /* Skip the reserved directory name */
        if (dtype == true &&
            sal_strcmp(dname, TECSUP_VARIANTS_DIR) == 0) {
            continue;
        }

        /* Add to the output list */
        tecsup_script_hash_insert(name_list, dname);
    }

    return bcma_io_dir_close(dh);
}

/*!
 * \brief Add an entry into tecsup table.
 *
 * \param [in] tbl Database table.
 * \param [in] data Entry data.
 *
 * \retval 0 No errors.
 * \retval -1 Failed to add entry.
 */
static int
tecsup_debug_entry_add(tecsup_script_entry_t *tbl, const char *data)
{
    tecsup_script_entry_t *new, *cur;

    new = sal_alloc(sizeof(*new), "bcmaTecsupNewEntry");
    if (!new) {
        cli_out("%sFailed to allocate memory\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }
    sal_memset(new, 0, sizeof(*new));
    sal_strlcpy(new->data, data, sizeof(new->data));

    cur = tbl;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = new;

    return 0;
}

/*!
 * \brief Add a table into tecsup database.
 *
 * \param [in] db Database.
 * \param [in] cat_name Category name.
 *
 * \return The pointer to the entry list for the new table.
 */
static tecsup_script_entry_t *
tecsup_debug_table_add(tecsup_script_db_t *db, const char *cat_name)
{
    tecsup_script_tbl_t *new, *cur, *next;

    cur = db;
    while ((next = cur->next) != NULL) {
        if (sal_strncmp(next->name, cat_name, sal_strlen(cat_name)) == 0) {
            return &next->entry_list;
        }
        cur = cur->next;
    }

    new = sal_alloc(sizeof(*new), "bcmaTecsupNewTable");
    if (!new) {
        cli_out("%sFailed to allocate memory\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return NULL;
    }
    sal_memset(new, 0, sizeof(*new));
    sal_strlcpy(new->name, cat_name, sizeof(new->name));
    cur->next = new;

    return &new->entry_list;
}

/*!
 * \brief Load a debug script of the given category to database.
 *
 * \param [in] unit Unit number.
 * \param [in] path Path to the script file.
 * \param [in] cat Categoy to add the new debug items.
 * \param [out] db Database.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to load script to database.
 */
static int
tecsup_script_category_load(int unit, const char *path, const char *cat_name,
                            tecsup_script_db_t *db)
{
    bcma_io_file_handle_t fh;
    char entry[BCMA_TECSUP_STR_LEN_MAX], *c;
    tecsup_script_entry_t *tbl;

    SHR_FUNC_ENTER(unit);

    fh = bcma_io_file_open(path, "r");
    if (!fh) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    tbl = tecsup_debug_table_add(db, cat_name);
    if (!tbl) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    while (bcma_io_file_gets(fh, entry, sizeof(entry)) != NULL) {
        /* Remove new line */
        if ((c = sal_strrchr(entry, '\n')) != NULL) {
            *c = '\0';
        }
        tecsup_debug_entry_add(tbl, entry);
    }

exit:
    if (fh) {
        bcma_io_file_close(fh);
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Driver functions
 */

/*!
 * \brief Get the category list.
 *
 * The function gets the script file names under \c cfg.script_path, and uses
 * the names as the category list.
 *
 * \param [in] int Unit number.
 * \param [in] cfg Techsupport configuration.
 * \param [in] max_num The maximum number of categories.
 * \param [out] list Buffer to save the category names.
 * \param [out] act_num The actual number of categories.
 *
 * \retval 0 No errors.
 * \retval -1 Fail to get the categories.
 */
static int
tecsup_script_category_list_get(int unit,
                                bcma_tecsup_cfg_t *cfg,
                                int max_num,
                                bcma_tecsup_str_t *list,
                                int *act_num)
{
    shr_pb_t *pb = NULL;
    tecsup_script_hash_t *dirs = NULL, *features = NULL, *categories = NULL;
    const bcma_tecsup_str_t *dnames, *fnames, *cnames;
    int dcnt, fcnt, ccnt, ocnt;
    int didx, fidx, cidx;

    SHR_FUNC_ENTER(unit);

    pb = shr_pb_create();
    features = tecsup_script_hash_alloc(TECSUP_ELEMENT_NUM_MAX);
    SHR_NULL_CHECK(features, SHR_E_MEMORY);
    categories = tecsup_script_hash_alloc(TECSUP_ELEMENT_NUM_MAX);
    SHR_NULL_CHECK(categories, SHR_E_MEMORY);

    /* Get script dir list */
    dirs = tecsup_script_hash_alloc(TECSUP_DIR_NUM_MAX);
    SHR_NULL_CHECK(dirs, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (tecsup_script_dir_list_get(unit, cfg->script_path, dirs));
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_keys_get(dirs, &dnames, &dcnt));

    /* Get category list */
    for (didx = 0; didx < dcnt; didx++) {
        shr_pb_mark(pb, 0);
        shr_pb_reset(pb);
        shr_pb_printf(pb, "%s", dnames[didx].str);
        shr_pb_mark(pb, -1);

        if (tecsup_dirent_get(shr_pb_str(pb), true, features) < 0) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (tecsup_script_hash_keys_get(features, &fnames, &fcnt));
        for (fidx = 0; fidx < fcnt; fidx++) {
            shr_pb_reset(pb);
            shr_pb_printf(pb, "%s", fnames[fidx].str);
            tecsup_dirent_get(shr_pb_str(pb), false, categories);
        }
    }

    /* Fill the output buffer */
    ocnt = 0;
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_keys_get(categories, &cnames, &ccnt));
    for (cidx = 0; cidx < ccnt; cidx++) {
        char *ptr = sal_strstr(cnames[cidx].str, ".soc");
        if (!ptr) {
            continue;
        }
        *ptr = '\0';
        if (ocnt < max_num) {
            sal_strlcpy(list[ocnt].str, cnames[cidx].str,
                        sizeof(list[ocnt].str));
        }
        ocnt++;
    }
    *act_num = ocnt;

exit:
    tecsup_script_hash_free(categories);
    tecsup_script_hash_free(features);
    tecsup_script_hash_free(dirs);
    shr_pb_destroy(pb);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the feature list.
 *
 * The function gets the folder names under \c cfg.script_path, and uses
 * the names as the feature list.
 *
 * \param [in] int Unit number.
 * \param [in] cfg Techsupport configuration.
 * \param [in] max_num The maximum number of features.
 * \param [out] list Buffer to save the feature names.
 * \param [out] act_num The actual number of features.
 *
 * \retval 0 No errors.
 * \retval -1 Fail to get the features.
 */
static int
tecsup_script_feature_list_get(int unit,
                               bcma_tecsup_cfg_t *cfg,
                               int max_num,
                               bcma_tecsup_str_t *list,
                               int *act_num)
{
    tecsup_script_hash_t *dirs = NULL, *features = NULL;
    const bcma_tecsup_str_t *dnames, *fnames;
    int dcnt, fcnt, ocnt;
    int didx, fidx;

    SHR_FUNC_ENTER(unit);

    features = tecsup_script_hash_alloc(TECSUP_ELEMENT_NUM_MAX);
    SHR_NULL_CHECK(features, SHR_E_MEMORY);

    /* Get script dir list */
    dirs = tecsup_script_hash_alloc(TECSUP_DIR_NUM_MAX);
    SHR_NULL_CHECK(dirs, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (tecsup_script_dir_list_get(unit, cfg->script_path, dirs));
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_keys_get(dirs, &dnames, &dcnt));

    /* Get feature list */
    for (didx = 0; didx < dcnt; didx++) {
        tecsup_dirent_get(dnames[didx].str, true, features);
    }

    /* Fill the output buffer */
    ocnt = 0;
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_keys_get(features, &fnames, &fcnt));
    for (fidx = 0; fidx < fcnt; fidx++) {
        if (ocnt < max_num) {
            sal_strlcpy(list[ocnt].str, fnames[fidx].str,
                        sizeof(list[ocnt].str));
        }
        ocnt++;
    }
    *act_num = ocnt;

exit:
    tecsup_script_hash_free(features);
    tecsup_script_hash_free(dirs);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create tecsup database.
 *
 * \param [in] desc Description of the database.
 *
 * \return Pointer to the created DB.
 */
static bcma_tecsup_db_t
tecsup_script_db_create(char *desc)
{
    tecsup_script_db_t *db;

    db = sal_alloc(sizeof(*db), desc);
    if (db) {
        sal_memset(db, 0, sizeof(*db));
    }

    return (bcma_tecsup_db_t)db;
}

/*!
 * \brief Destroy tecsup database.
 *
 * \param [in] db Database to be destroyed.
 *
 * \return Always 0.
 */
static int
tecsup_script_db_destroy(bcma_tecsup_db_t tsdb)
{
    tecsup_script_db_t *db = (tecsup_script_db_t *)tsdb;
    tecsup_script_tbl_t *tbl_prev, *tbl_cur;
    tecsup_script_entry_t *ent_prev, *ent_cur;

    if (!db) {
        return 0;
    }

    tbl_cur = db;
    while (tbl_cur) {
        ent_cur = tbl_cur->entry_list.next;
        while (ent_cur) {
            ent_prev = ent_cur;
            ent_cur = ent_cur->next;
            sal_free(ent_prev);
        }
        tbl_prev = tbl_cur;
        tbl_cur = tbl_cur->next;
        sal_free(tbl_prev);
    }

    return 0;
}

/*!
 * \brief Load debug scripts of the given feature to database.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Feature name.
 * \param [in] cfg Techsupport configuration.
 * \param [in] cat_names Categories to be loaded.
 * \param [out] db Database.
 *
 * \retval 0 No errors.
 * \retval -1 Failed to load scripts to database.
 */
static int
tecsup_script_feature_load(int unit,
                           const char *feature,
                           bcma_tecsup_cfg_t *cfg,
                           bcma_tecsup_str_t *cat_names,
                           bcma_tecsup_db_t tsdb)
{
    shr_pb_t *pb;
    tecsup_script_hash_t *dirs = NULL;
    const bcma_tecsup_str_t *dnames, *cname;
    int didx, dcnt;
    tecsup_script_db_t *db = (tecsup_script_db_t *)tsdb;

    SHR_FUNC_ENTER(unit);

    pb = shr_pb_create();

    if (!db || !cfg) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get script dir list */
    dirs = tecsup_script_hash_alloc(TECSUP_DIR_NUM_MAX);
    SHR_NULL_CHECK(dirs, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (tecsup_script_dir_list_get(unit, cfg->script_path, dirs));
    SHR_IF_ERR_EXIT
        (tecsup_script_hash_keys_get(dirs, &dnames, &dcnt));

    for (didx = 0; didx < dcnt; didx++) {
        shr_pb_mark(pb, 0);
        shr_pb_reset(pb);

        shr_pb_printf(pb, "%s", dnames[didx].str);
        shr_pb_mark(pb, -1);
        if (!tecsup_isdir(shr_pb_str(pb))) {
            continue;
        }

        cname = cat_names;
        while (sal_strlen(cname->str)) {
            shr_pb_reset(pb);
            shr_pb_printf(pb, "%s/%s.soc", feature, cname->str);
            tecsup_script_category_load(unit, shr_pb_str(pb), cname->str, db);

            cname++;
        }
    }

exit:
    shr_pb_destroy(pb);
    tecsup_script_hash_free(dirs);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Execute the debug script for the given feature category.
 *
 * \param [in] tsdb Database.
 * \param [in] cat Category type.
 * \param [in] cb Callback function.
 * \param [in] cookie User data.
 *
 * \retval 0 No errors.
 * \retval -1 No entry found.
 */
static int
tecsup_script_category_traverse(bcma_tecsup_db_t tsdb,
                                const char *cat,
                                bcma_tecsup_db_traverse_cb_f cb,
                                void *cookie)
{
    tecsup_script_db_t *db = (tecsup_script_db_t *)tsdb;
    tecsup_script_tbl_t *tbl;
    tecsup_script_entry_t *cur;

    if (!cb) {
        return 0;
    }

    if (!db) {
        return -1;
    }

    tbl = db->next;
    while (tbl) {
        if (sal_strcmp(tbl->name, cat) == 0) {
            break;
        }
        tbl = tbl->next;
    }
    if (!tbl) {
        return -1;
    }

    cur = tbl->entry_list.next;
    while (cur) {
        cb(cur->data, cookie);
        cur = cur->next;
    }

    return 0;
}

static bcma_tecsup_drv_t bcma_tecsup_drv_script = {
    .feature_list_get = tecsup_script_feature_list_get,
    .category_list_get = tecsup_script_category_list_get,
    .create = tecsup_script_db_create,
    .destroy = tecsup_script_db_destroy,
    .load = tecsup_script_feature_load,
    .traverse = tecsup_script_category_traverse,
};


/*******************************************************************************
 * Public functions
 */

bcma_tecsup_drv_t *
bcma_tecsup_drv_script_get()
{
    return &bcma_tecsup_drv_script;
}
