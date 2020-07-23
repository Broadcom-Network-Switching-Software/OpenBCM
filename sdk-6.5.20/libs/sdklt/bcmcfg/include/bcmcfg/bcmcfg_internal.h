/*! \file bcmcfg_internal.h
 *
 *  \brief BCMCFG internal interfaces
 *
 * These APIs allow alternative component scanners to be tested.
 *
 * Not intended for production use.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_INTERNAL_H
#define BCMCFG_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmcfg/bcmcfg_types.h>

/*!
 * \brief Component configuration.
 *
 * Current component configuration used by the component reader.  Test
 * applications must set this value to the component configuration
 * being tested.
 */
extern const bcmcfg_comp_scanner_conf_t *bcmcfg_component_conf;

/*!
 * \brief Allocate working storage for component data.
 *
 * Allocate working storage for component data associated with the
 * component scanner configuration. This must be called before assigning
 * the scanner to bcmcfg_component_conf. Call bcmcfg_read_component_free()
 * to free the working storage.
 *
 * \param [in]  conf            Component scanner configuration.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmcfg_read_component_alloc(const bcmcfg_comp_scanner_conf_t *conf);

/*!
 * \brief Free working storage for component data.
 *
 * Free working storage for component data allocated with
 * bcmcfg_read_component_alloc(). This must be called after all
 * operations on the component scanner are complete.
 *
 * \param [in]  conf            Component scanner configuration.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmcfg_read_component_free(const bcmcfg_comp_scanner_conf_t *conf);

/*!
 * \brief Set software component default.
 *
 * Set software component default.
 *
 * \param [in] conf Scanner configuration.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_read_component_default_set(const bcmcfg_comp_scanner_conf_t *conf);

/*!
 * \brief Get component default configuration.
 *
 * Get component default configuration.
 *
 * \param [in]  scanner         Scanner configuration.
 * \param [in]  size            Component structure size.
 * \param [out] config          Configuration.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_PARAM          Parameter error.
 */
extern int
bcmcfg_comp_scanner_default_config_get(const bcmcfg_comp_scanner_t *scanner,
                                       size_t size,
                                       void *config);

/*!
 * \brief Get component configuration.
 *
 * Get component configuration.
 *
 * \param [in]  scanner         Scanner configuration.
 * \param [in]  size            Component structure size.
 * \param [out] config          Configuration.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_PARAM          Parameter error.
 */
extern int
bcmcfg_comp_scanner_config_get(const bcmcfg_comp_scanner_t *scanner,
                               size_t size,
                               void *config);

/*!
 * \brief Set component configuration.
 *
 * Set component configuration.
 *
 * \param [in]  scanner         Scanner configuration.
 * \param [in]  size            Component structure size.
 * \param [in]  config          Configuration.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_PARAM          Parameter error.
 */
extern int
bcmcfg_comp_scanner_config_set(const bcmcfg_comp_scanner_t *scanner,
                               size_t size,
                               void *config);

/*!
 * \brief Set the value for given field in the config logical table.
 *
 * Set the value to the given field in the config logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [in]  start_idx       Start index of array field.
 * \param [in]  count           Number of field values allocated.
 * \param [out] field           List of field values.
 * \param [out] actual          Number of field values returned.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_UNAVAIL        Device/Table/Fields does not exist.
 * \retval SHR_E_PARAM          Table is not of type config table.
 */
extern int
bcmcfg_table_set_internal(int unit,
                          bcmltd_sid_t sid,
                          bcmltd_sid_t fid,
                          size_t start_idx,
                          size_t count,
                          uint64_t *field,
                          size_t *actual);

/*!
 * \brief Return all fields for a config logical table
 *
 * If a field in a config logical table (which has a "config: TRUE"
 * attribute) was set, return all the fields that were set starting at
 * fid. Return an error if the sid is not a config logical table, or
 * the config logical table was not set at all.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [in]  start_idx       Start index of array field.
 * \param [in]  count           Number of field values allocated.
 * \param [out] field           List of field values.
 * \param [out] actual          Number of field values returned.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_table_get_internal(int unit,
                          bcmltd_sid_t sid,
                          bcmltd_sid_t fid,
                          size_t start_idx,
                          size_t count,
                          uint64_t *field,
                          size_t *actual);

/*!
 * Config data source type.
 */
typedef enum bcmcfg_config_src_type_s {
    /*! Config data set by reader(config string or yml file). */
    BCMCFG_CONFIG_SRC_TYPE_READER = 1,

    /*! Config data set by bcmcfg config set API. */
    BCMCFG_CONFIG_SRC_TYPE_API
} bcmcfg_config_src_type_t;

/*!
 * Config field data.
 */
typedef struct bcmcfg_config_data_s {
    /*! Locus (file or string). */
    const char *locus;

    /*! Reader line number. */
    int line;

    /*! Reader column number. */
    int column;

    /*! The array index for array filed. */
    uint32_t index;

    /*! Config data. */
    uint64_t value;

    /*! Config data source type. */
    bcmcfg_config_src_type_t config_src;
} bcmcfg_config_data_t;

/*!
 * \brief mark config lt initialization is done.
 *
 * mark config lt initialization is done.
 *
 * \param [in]     unit         unit number.
 */
extern void
bcmcfg_lt_init_done(int unit);

/*!
 * \brief Check if config lt initialization is done.
 *
 * Check if config lt initialization is done.
 *
 * \param [in]     unit         unit number.
 */
extern bool
bcmcfg_lt_init_is_done(int unit);

/*!
 * \brief Check if config init is in ISSU upgrade.
 *
 * Check if config init is in ISSU upgrade.
 *
 * \param [in]     unit         unit number.
 */
extern bool
bcmcfg_lt_init_is_issu(int unit);

/*!
 * \brief BCMCFG config table information initialization.
 *
 * Initialize the config table information.
 * This function cleans up the previous config field settings.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     keep         Flag to keep reader config.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmcfg_config_table_init(int unit, bool keep);

/*!
 * \brief Allocate memory for config data.
 *
 * Allocate memory for config data for the given unit.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table sid.
 * \param [out]    data_ptr     Pointer to config data.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval SHR_E_UNAVAIL    Table sid not in valid range.
 * \retval SHR_E_MEMORY     Error in allocating memory for config data.
 */
extern int
bcmcfg_config_table_field_alloc(int unit,
                                bcmltd_sid_t sid,
                                bcmcfg_config_data_t **data_ptr);


/*!
 * \brief Get field array index.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table sid.
 * \param [in]     fid          Logical field ID.
 * \param [in]     fidx         Logical field index.
 * \param [out]    field_index  Field array index.
 *
 * \retval SHR_E_NONE       Operation successful.
 */

int
bcmcfg_tbl_user_config_field_index_get(int unit,
                                       bcmltd_sid_t sid,
                                       bcmltd_fid_t fid,
                                       uint32_t fidx,
                                       uint32_t *field_index);
/*!
 * \brief Get pointer to config table user data.
 *
 * Get pointer to config table user data.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table sid.
 * \param [out]    config       Pointer to config data.
 *
 * \retval SHR_E_NONE       Operation successful.
 */

extern int
bcmcfg_tbl_user_config_get(int unit,
                           bcmltd_sid_t sid,
                           bcmcfg_config_data_t **config);

/*!
 * \brief Set config table user data.
 *
 * Set config table user data.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table sid.
 * \param [in]     config       Config data.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval SHR_E_PARAM      If config data is NULL.
 */

extern int
bcmcfg_tbl_user_config_set(int unit,
                           bcmltd_sid_t sid,
                           bcmcfg_config_data_t *config);

/*!
 * \brief Config table user data hook.
 *
 * Hook to handle special config data actions.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table sid.
 * \param [in]     fid          Logical table fid.
 * \param [in]     value        Config value.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval SHR_E_PARAM      If config data is NULL.
 */

extern int
bcmcfg_tbl_user_config_hook(int unit,
                            bcmltd_sid_t sid,
                            bcmltd_fid_t fid,
                            uint64_t value);

/*!
 * \brief Set cached config value by ID.
 *
 * Set cached config value by logical table and field ID.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table sid.
 * \param [in]     fid          Field id.
 * \param [in]     fidx         Field index.
 * \param [in]     value        Field value.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval SHR_E_MEMORY     Memory allocation failure.
 */
extern int
bcmcfg_table_set_cached_config_value_by_id(int unit,
                                           bcmltd_sid_t sid,
                                           bcmltd_sid_t fid,
                                           uint32_t fidx,
                                           uint64_t value);

/*!
 * \brief Set cached config string by name.
 *
 * Set cached config string by table name string and field name string.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     table_name   Logical table name.
 * \param [in]     field_name   Field name.
 * \param [in]     field_index  Field index.
 * \param [in]     field_value  Field value string.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval SHR_E_MEMORY     Memory allocation failure.
 */
extern int
bcmcfg_table_set_cached_config_string_by_name(int unit,
                                              const char *table_name,
                                              const char *field_name,
                                              uint32_t field_index,
                                              const char *field_value);

/*!
 * \brief Set cached config by name.
 *
 * Set cached config value by table name string and field name string.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     table_name   Logical table name.
 * \param [in]     field_name   Field name.
 * \param [in]     field_index  Field index.
 * \param [in]     field_value  Field value.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval SHR_E_MEMORY     Memory allocation failure.
 */
extern int
bcmcfg_table_set_cached_config_value_by_name(int unit,
                                             const char *table_name,
                                             const char *field_name,
                                             uint32_t field_index,
                                             uint64_t field_value);

/*!
 * \brief Apply all cached config.
 *
 * Apply all cached configuration.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     warm         Indicates cold or warm start.
 * \param [in]     stage        Initialization stage.
 *
 * \retval SHR_E_NONE       Operation successful.
 * \retval !SHR_E_NONE      Error.
 */
extern int
bcmcfg_table_cached_config_apply(int unit, bool warm,
                                 bcmcfg_init_stage_t stage);

/*!
 * \brief BCMCFG unit config map editing function.
 *
 * BCMCFG unit config map editing function.
 *
 * \param [in]  unit   The device unit number.
 * \param [in]  warm   Indicates cold or warm start.
 *
 * \return SHR_E_NONE  OK
 * \return !SHR_E_NONE Error
 */
int
bcmcfg_unit_conf_edit(int unit,
                      bool warm);


/*!
 * \brief BCMCFG unit config cleanup function.
 *
 * BCMCFG unit config cleanup.
 *
 * \return SHR_E_NONE  OK
 * \return !SHR_E_NONE Error
 */
extern void
bcmcfg_unit_conf_cleanup(void);

/*!
 * \brief Resolve config enums.
 *
 * Resolve config enums. DEPRECATED.
 *
 * \param [in]     unit         Unit number.
 *
 * \retval SHR_E_NONE     Enum resolved.
 * \retval SHR_E_PARAM    Enum resolution failed.
 */

extern int
bcmcfg_resolve_config_enum(int unit);

/*!
 * \brief Set variant to BASE if default variant is unavailable.
 *
 * Set variant to BASE if default variant is unavailable.
 *
 * \param [in]  unit         Unit number.
 * \param [in]  warm         Indicates cold or warm start.
 *
 * \retval SHR_E_NONE     No Error.
 * \retval !SHR_E_NONE    Error.
 */

extern int
bcmcfg_lt_config_base_variant_set(int unit, bool warm);

/*!
 * \brief Load map default for LT config fields
 *
 * Load map default for LT config fields.
 *
 * \param [in]  unit         Unit number.
 * \param [in]  warm         Indicates cold or warm start.
 *
 * \retval SHR_E_NONE     No Error.
 * \retval !SHR_E_NONE    Error.
 */

extern int
bcmcfg_lt_config_default_set(int unit, bool warm);

/*!
 * \brief Range check for LT config fields
 *
 * Range check for LT config fields.
 *
 * \param [in]  unit         Unit number.
 * \param [in]  warm         Indicates cold or warm start.
 *
 * \retval SHR_E_NONE     No Error.
 * \retval !SHR_E_NONE    Error.
 */

extern int
bcmcfg_lt_config_range_check(int unit, bool warm);

/*!
 * \brief Load variant data.
 *
 * Load variant data.
 *
 * \param [in]  unit         Unit number.
 * \param [in]  warm         Indicates cold or warm start.
 *
 * \retval SHR_E_NONE     No Error.
 * \retval !SHR_E_NONE    Error.
 */

extern int
bcmcfg_variant_load(int unit, bool warm);

#endif /* BCMCFG_INTERNAL_H */
