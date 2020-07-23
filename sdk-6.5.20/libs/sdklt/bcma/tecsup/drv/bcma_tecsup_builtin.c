/*! \file bcma_tecsup_script.c
 *
 * Built-in mode driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_hash_str.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmlrd/bcmlrd_conf.h>

#include <bcma/tecsup/bcma_tecsup_drv.h>
#include <bcma/tecsup/bcma_tecsup_builtin.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TECSUP

/*******************************************************************************
 * Local definitions
 */

/*! Variant-specific attach function type. */
typedef bcma_tecsup_builtin_feature_t *
(*tecsup_feature_list_get_f)(int unit);

typedef bcma_tecsup_builtin_cmd_t *
(*tecsup_category_list_get_f)(int unit);

/*! Array of device-specific data retrieval functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bd##_bcma_tecsup_feature_list_get, \
      _bd##_bcma_tecsup_category_list_get },
static struct {
    tecsup_feature_list_get_f feature_list_get;
    tecsup_category_list_get_f category_list_get;
} dev_list[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! Array of variant-specific data retrieval functions */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_bcma_tecsup_feature_list_get, \
      _bd##_vu##_va##_bcma_tecsup_category_list_get },

static struct {
    tecsup_feature_list_get_f feature_list_get;
    tecsup_category_list_get_f category_list_get;
} variant_list[] = {
    { 0, 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0, 0 } /* end-of-list */
};

/*! Command scope types. */
typedef enum tecsup_builtin_cmd_scope_e {
    /*! Device-specific commands, i.e. all varinats. */
    TECSUP_BUILTIN_DEV = 0,

    /*! Variant-specific commands. */
    TECSUP_BUILTIN_VAR,

    /*! Must be the last. */
    TECSUP_BUILTIN_COUNT,
} tecsup_builtin_cmd_scope_t;

/*! Database object. */
typedef struct tecsup_builtin_db_s {
    bcma_tecsup_builtin_feature_t *feature[TECSUP_BUILTIN_COUNT];
} tecsup_builtin_db_t;

#define TECSUP_ELEMENT_NUM_MAX 128


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the device-specific feature list.
 *
 * \param [in] int Unit number.
 *
 * \returns Pointer to the device-specific list or NULL if none.
 */
static bcma_tecsup_builtin_feature_t *
tecsup_dev_feature_list_get(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return NULL;
    }

    return dev_list[dev_type].feature_list_get(unit);
}

/*!
 * \brief Get the device-specific category list.
 *
 * \param [in] int Unit number.
 *
 * \returns Pointer to the device-specific list or NULL if none.
 */
static bcma_tecsup_builtin_cmd_t *
tecsup_dev_category_list_get(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return NULL;
    }

    return dev_list[dev_type].category_list_get(unit);
}

/*!
 * \brief Get the variant-specific feature list.
 *
 * This function returns the variant-specific feature list.
 *
 * If there is no variant-specific list or the list is identical to the
 * device-specific list, it returns NULL.
 *
 * \param [in] int Unit number.
 *
 * \returns Pointer to the variant-specific list or NULL if none.
 */
static bcma_tecsup_builtin_feature_t *
tecsup_var_feature_list_get(int unit)
{
    bcma_tecsup_builtin_feature_t *dev_feature_list, *var_feature_list;
    bcmlrd_variant_t variant;

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return NULL;
    }

    dev_feature_list = tecsup_dev_feature_list_get(unit);
    var_feature_list = variant_list[variant].feature_list_get(unit);

    if (var_feature_list == dev_feature_list) {
        return NULL;
    } else {
        return var_feature_list;
    }
}

/*!
 * \brief Get the variant-specific category list.
 *
 * This function returns the variant-specific category list.
 *
 * If there is no variant-specific list or the list is identical to the
 * device-specific list, it returns NULL.
 *
 * \param [in] int Unit number.
 *
 * \returns Pointer to the variant-specific list or NULL if none.
 */
static bcma_tecsup_builtin_cmd_t *
tecsup_var_category_list_get(int unit)
{
    bcma_tecsup_builtin_cmd_t *dev_category_list, *var_category_list;
    bcmlrd_variant_t variant;

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return NULL;
    }

    dev_category_list = tecsup_dev_category_list_get(unit);
    var_category_list = variant_list[variant].category_list_get(unit);

    if (var_category_list == dev_category_list) {
        return NULL;
    } else {
        return var_category_list;
    }
}

/*******************************************************************************
 * Driver functions
 */

/*!
 * \brief Get the feature list.
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
tecsup_builtin_feature_list_get(int unit,
                                bcma_tecsup_cfg_t *cfg,
                                int max_num,
                                bcma_tecsup_str_t *list,
                                int *act_num)
{
    int cnt = 0, idx;
    shr_hash_str_hdl hash;
    char *key;
    bcma_tecsup_builtin_feature_t *builtin_list[TECSUP_BUILTIN_COUNT], *ent;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_hash_str_dict_alloc(TECSUP_ELEMENT_NUM_MAX, &hash));

    sal_memset(builtin_list, 0, sizeof(builtin_list));
    builtin_list[TECSUP_BUILTIN_DEV] = tecsup_dev_feature_list_get(unit);
    builtin_list[TECSUP_BUILTIN_VAR] = tecsup_var_feature_list_get(unit);

    if (!builtin_list[TECSUP_BUILTIN_DEV] &&
        !builtin_list[TECSUP_BUILTIN_VAR]) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    for (idx = 0; idx < COUNTOF(builtin_list); idx++) {
        ent = builtin_list[idx];
        if (!ent) {
            continue;
        }
        while (ent->name) {
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (shr_hash_str_dict_insert(hash, ent->name, ent->name),
                 SHR_E_EXISTS);
            ent++;
        }
    }

    SHR_IF_ERR_EXIT
        (shr_hash_str_get_first(hash, &key, NULL));
    if (cnt < max_num) {
        sal_strlcpy(list[cnt].str, key, sizeof(list[cnt].str));
    }
    cnt++;

    while (SHR_SUCCESS(shr_hash_str_get_next(hash, &key, NULL))) {
        if (cnt < max_num) {
            sal_strlcpy(list[cnt].str, key, sizeof(list[cnt].str));
        }
        cnt++;
    }

    if (act_num) {
        *act_num = cnt;
    }

exit:
    shr_hash_str_dict_free(hash);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the category list.
 *
 * \param [in] int Unit number.
 * \param [in] cfg Techsupport configuration.
 * \param [in] max_num The maximum number of categories.
 * \param [out] list Buffer to save the category names.
 * \param [out] act_num The actual number of categories..
 *
 * \retval 0 No errors.
 * \retval -1 Fail to get the features.
 */
static int
tecsup_builtin_category_list_get(int unit,
                                 bcma_tecsup_cfg_t *cfg,
                                 int max_num,
                                 bcma_tecsup_str_t *list,
                                 int *act_num)
{
    int cnt = 0, idx, cidx;
    shr_hash_str_hdl hash;
    char *key;
    bcma_tecsup_builtin_cmd_t *builtin_list[TECSUP_BUILTIN_COUNT], ent;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_hash_str_dict_alloc(TECSUP_ELEMENT_NUM_MAX, &hash));

    sal_memset(builtin_list, 0, sizeof(builtin_list));
    builtin_list[TECSUP_BUILTIN_DEV] = tecsup_dev_category_list_get(unit);
    builtin_list[TECSUP_BUILTIN_VAR] = tecsup_var_category_list_get(unit);

    if (!builtin_list[TECSUP_BUILTIN_DEV] &&
        !builtin_list[TECSUP_BUILTIN_VAR]) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    for (idx = 0; idx < COUNTOF(builtin_list); idx++) {
        if (!builtin_list[idx]) {
            continue;
        }

        cidx = 0;
        while ((ent = builtin_list[idx][cidx]) != NULL) {
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (shr_hash_str_dict_insert(hash, ent, ent),
                 SHR_E_EXISTS);
            cidx++;
        }
    }

    SHR_IF_ERR_EXIT
        (shr_hash_str_get_first(hash, &key, NULL));
    if (cnt < max_num) {
        sal_strlcpy(list[cnt].str, key, sizeof(list[cnt].str));
    }
    cnt++;

    while (SHR_SUCCESS(shr_hash_str_get_next(hash, &key, NULL))) {
        if (cnt < max_num) {
            sal_strlcpy(list[cnt].str, key, sizeof(list[cnt].str));
        }
        cnt++;
    }

    if (act_num) {
        *act_num = cnt;
    }

exit:
    shr_hash_str_dict_free(hash);
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
tecsup_builtin_db_create(char *desc)
{
    tecsup_builtin_db_t *db;

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
tecsup_builtin_db_destroy(bcma_tecsup_db_t tsdb)
{
    tecsup_builtin_db_t *db = (tecsup_builtin_db_t *)tsdb;

    if (db) {
        sal_free(db);
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
 * \retval 0 Success.
 * \retval -1 Failure.
 */
static int
tecsup_builtin_feature_load(int unit,
                            const char *feature_name,
                            bcma_tecsup_cfg_t *cfg,
                            bcma_tecsup_str_t *cat_names,
                            bcma_tecsup_db_t tsdb)
{
    int idx;
    bcma_tecsup_builtin_feature_t *feature_list[TECSUP_BUILTIN_COUNT], *feature;
    tecsup_builtin_db_t *db = (tecsup_builtin_db_t *)tsdb;

    if (!db) {
        return -1;
    }

    sal_memset(feature_list, 0, sizeof(feature_list));
    feature_list[TECSUP_BUILTIN_DEV] = tecsup_dev_feature_list_get(unit);
    feature_list[TECSUP_BUILTIN_VAR] = tecsup_var_feature_list_get(unit);

    if (!feature_list[TECSUP_BUILTIN_DEV] &&
        !feature_list[TECSUP_BUILTIN_VAR]) {
        return -1;
    }

    for (idx = 0; idx < COUNTOF(feature_list); idx++) {
        feature = feature_list[idx];
        if (!feature) {
            continue;
        }
        while (feature->name) {
            if (sal_strcmp(feature->name, feature_name) == 0) {
                db->feature[idx] = feature;
                break;
            }
            feature++;
        }
    }

    return 0;
}

/*!
 * \brief Execute the debug dumps for the given feature category.
 *
 * \param [in] tsdb Database.
 * \param [in] cat Category name.
 * \param [in] cb Callback function.
 * \param [in] cookie User data.
 *
 * \retval 0 No errors.
 * \retval -1 No entry found.
 */
static int
tecsup_builtin_category_traverse(bcma_tecsup_db_t tsdb,
                                 const char *cat,
                                 bcma_tecsup_db_traverse_cb_f cb,
                                 void *cookie)
{
    int idx, fidx;
    tecsup_builtin_db_t *db = (tecsup_builtin_db_t *)tsdb;
    bcma_tecsup_builtin_feature_t *feature;
    bcma_tecsup_builtin_cmdlist_t *cmd_list;

    if (!cb) {
        return 0;
    }

    if (!db) {
        return -1;
    }

    if (!db->feature[TECSUP_BUILTIN_DEV] &&
        !db->feature[TECSUP_BUILTIN_VAR]) {
        return -1;
    }

    for (fidx = 0; fidx < COUNTOF(db->feature); fidx++) {
        feature = db->feature[fidx];
        if (!feature) {
            continue;
        }
        cmd_list = feature->cmd_list;
        while (cmd_list && cmd_list->name) {
            if (sal_strcmp(cmd_list->name, cat) == 0) {
                break;
            }
            cmd_list++;
        }

        if (!cmd_list->name) {
            continue;
        }

        idx = 0;
        while (cmd_list->cmds[idx]) {
            cb(cmd_list->cmds[idx], cookie);
            idx++;
        }
    }

    return 0;
}

static bcma_tecsup_drv_t bcma_tecsup_drv_builtin = {
    .feature_list_get = tecsup_builtin_feature_list_get,
    .category_list_get = tecsup_builtin_category_list_get,
    .create = tecsup_builtin_db_create,
    .destroy = tecsup_builtin_db_destroy,
    .load = tecsup_builtin_feature_load,
    .traverse = tecsup_builtin_category_traverse,
};


/*******************************************************************************
 * Public functions
 */

bcma_tecsup_drv_t *
bcma_tecsup_drv_builtin_get()
{
    return &bcma_tecsup_drv_builtin;
}
