/*! \file bcma_tecsup_drv.h
 *
 * Techsupport driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TECSUP_DRV_H
#define BCMA_TECSUP_DRV_H

#include <bcma/cli/bcma_cli_parse.h>

/*! Maximum string size. */
#define BCMA_TECSUP_STR_LEN_MAX 128

/*! Opaque database handle. */
typedef struct bcma_tecsup_db_s *bcma_tecsup_db_t;

/*! String buffer object. */
typedef struct bcma_tecsup_str_s {
    /*! String data. */
    char str[BCMA_TECSUP_STR_LEN_MAX];
} bcma_tecsup_str_t;

/*! Techsupport configuration. */
typedef struct bcma_tecsup_cfg_s {
    /*! Operating mode. */
    int mode;

    /*! Path to the script directory. */
    char *script_path;
} bcma_tecsup_cfg_t;

/*! Techsupport device-specific data. */
typedef struct bcma_tecsup_dev_data_s {
    /*! Source address. */
    uint32_t src;
} bcma_tecsup_dev_data_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_bcma_tecsup_dev_data_get(int unit, uint32_t sel, bcma_tecsup_dev_data_t *data);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the device-specific data list.
 *
 * To get the device feature or category list.
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
typedef int
(*bcma_bcma_tecsup_list_get_f)(int unit,
                               bcma_tecsup_cfg_t *cfg,
                               int max_num,
                               bcma_tecsup_str_t *list,
                               int *act_num);

/*!
 * \brief Create tecsup database.
 *
 * \param [in] desc Description of the database.
 *
 * \return The pointer to the database object or NULL when failed to create.
 */
typedef bcma_tecsup_db_t
(*bcma_tecsup_db_create_f)(char *decs);

/*!
 * \brief Destroy tecsup database.
 *
 * \param [in] db Database to be destroyed.
 *
 * \retval 0 Success.
 * \retval -1 Failure.
 */
typedef int
(*bcma_tecsup_db_destroy_f)(bcma_tecsup_db_t db);

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
typedef int
(*bcma_bcma_tecsup_feature_load_f)(int unit,
                                   const char *feature,
                                   bcma_tecsup_cfg_t *cfg,
                                   bcma_tecsup_str_t *cat_names,
                                   bcma_tecsup_db_t db);

/*!
 * \brief Callback function for entry traverse.
 *
 * \param [in] cmd CLI command string.
 * \param [in] cookie User data.
 */
typedef void
(*bcma_tecsup_db_traverse_cb_f)(const char *cmd, void *cookie);

/*!
 * \brief Traverse the debug entries of the given category.
 *
 * \param [in] db Database.
 * \param [in] cat Category name.
 * \param [in] cb Callback function.
 * \param [in] cookie User data.
 *
 * \retval 0 Success.
 * \retval -1 Failure.
 */
typedef int
(*bcma_tecsup_category_traverse_f)(bcma_tecsup_db_t db,
                                   const char *cat,
                                   bcma_tecsup_db_traverse_cb_f cb,
                                   void *cookie);

/*!
 * \brief Operating mode driver.
 */
typedef struct bcma_tecsup_drv_s {
    /*! Get the feature list. */
    bcma_bcma_tecsup_list_get_f feature_list_get;

    /*! Get the category list. */
    bcma_bcma_tecsup_list_get_f category_list_get;

    /*! Create database. */
    bcma_tecsup_db_create_f create;

    /*! Destroy database. */
    bcma_tecsup_db_destroy_f destroy;

    /*! Load debug commands for a given feature into database. */
    bcma_bcma_tecsup_feature_load_f load;

    /*! Traverse commands of a given category in database. */
    bcma_tecsup_category_traverse_f traverse;

} bcma_tecsup_drv_t;

/*!
 * \brief Get the driver for built-in mode.
 *
 * \return Built-in mode driver pointer.
 */
extern bcma_tecsup_drv_t *
bcma_tecsup_drv_builtin_get(void);

/*!
 * \brief Get the driver for script mode.
 *
 * \return Script mode driver pointer.
 */
extern bcma_tecsup_drv_t *
bcma_tecsup_drv_script_get(void);

#endif /* BCMA_TECSUP_DRV_H */
