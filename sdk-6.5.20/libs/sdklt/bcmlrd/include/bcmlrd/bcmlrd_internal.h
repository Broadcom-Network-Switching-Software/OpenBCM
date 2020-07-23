/*! \file bcmlrd_internal.h
 *
 * \brief Logical Resource Database internal definitions
 *
 * Internal data structures and definitions
 *
 * Not intended for callers outside of LRD.
 *
 * Not optimized for space.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_INTERNAL_H
#define BCMLRD_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_internal.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_default_table_conf.h>

/*!
 * \brief Default Global Device PCM configuration.
 *
 * Default per-device device PCM configurations.
 */
extern bcmlrd_dev_pcm_conf_t bcmlrd_default_dev_pcm_conf[];

/*!
 * \brief Default list of function pointers to retrieve match id data.
 *
 * Default per-variant function to retrieve match id data.
 */
extern bcmlrd_match_id_db_get_t bcmlrd_default_match_id_db_get_ptrs[];

/*!
 * \brief Default list of function pointers to retrieve match id data.
 *
 * Default per-variant function to retrieve match id data.
 */
extern const bcmlrd_match_id_db_info_t *bcmlrd_default_match_id_data_info_all[];

/*!
 * \brief Compact version of PCM configuration per unit.
 *
 * Configurations for each attached unit, or NULL if there is no configuration
 */

extern const bcmlrd_pcm_conf_compact_rep_t *
bcmlrd_unit_pcm_conf_compact[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Expanded version of PCM configuration per unit.
 *
 * Configurations for each attached unit, or NULL if there is no configuration
 */

extern bcmlrd_pcm_conf_rep_t *
bcmlrd_unit_pcm_conf[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Global Device configuration.
 *
 * Per-device device map configurations. This interface is for unit
 * test purposes only, not for production use.
 */
extern bcmlrd_dev_conf_t const *bcmlrd_dev_conf;

/*!
 * \brief Default Global Device map configuration.
 *
 * Default per-device device map configurations.
 */
extern bcmlrd_dev_conf_t bcmlrd_default_dev_conf[];

/*!
 * \brief Default Global Device table and map configuration.
 *
 * Default per-device device table and map configurations.
 */
extern bcmlrd_device_conf_t bcmlrd_default_device_conf;

/*!
 * \brief Default Global Device component configuration.
 *
 * Default per-device device component configurations.
 */
extern bcmlrd_dev_comp_conf_t bcmlrd_default_dev_comp_conf[];

/*!
 * \brief Match ID get function per unit.
 *
 * Function to get the match id data for the sorted by unit.
 */
extern bcmlrd_match_id_db_get_t
bcmlrd_unit_match_id_db_get_ptr[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Match ID get function per unit.
 *
 * Function to get the match id data for the sorted by unit.
 */
extern const bcmlrd_match_id_db_info_t *
bcmlrd_unit_match_id_data_info_all[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Compoenent configuration per unit.
 *
 * Configurations for each attached unit, or NULL if there is no configuration
 */

extern const bcmlrd_dev_comp_t *
bcmlrd_unit_comp_conf[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Get a configuration for a unit.
 *
 * Get a pointer to a configuration for the given unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval non-NULL     Pointer to configuration.
 * \retval NULL         Unit number invalid.
 */

extern const bcmlrd_map_conf_rep_t *
bcmlrd_unit_conf_get(int unit);

/*!
 * \brief Get a PCM configuration for a unit.
 *
 * Get a pointer to a PCM configuration for the given unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval non-NULL     Pointer to configuration.
 * \retval NULL         Unit number invalid.
 */

extern bcmlrd_pcm_conf_rep_t *
bcmlrd_unit_pcm_conf_get(int unit);

/*!
 * \brief Probe attached units for a configuration
 *
 * \retval SHR_E_NONE   No errors during probe.
 */
extern int
bcmlrd_unit_init(void);

/*!
 * \brief Cleanup LRD attached unit information.
 *
 * \retval SHR_E_NONE   No errors during cleanup.
 */
extern int
bcmlrd_unit_cleanup(void);

/*!
 * \brief Configuration/Unit map.
 *
 * Configurations for each attached unit, or NULL if there is no configuration
 */

extern const bcmlrd_map_conf_rep_t *
bcmlrd_unit_conf[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Return the table type flags a table.
 *
 * \param [in] sid              Table index.
 *
 * \retval                      Table type.
 */

static inline uint32_t
bcmlrd_table_type(bcmlrd_sid_t sid)
{
    return (bcmltd_table_conf->table[sid]->flags & BCMLRD_TABLE_F_TYPE_MASK);
}

/*!
 * \brief Field data type.
 */
typedef enum bcmlrd_field_data_type_e {
    /*! Min. */
    BCMLRD_FIELD_DATA_TYPE_MIN,

    /*! Max. */
    BCMLRD_FIELD_DATA_TYPE_MAX,

    /*! Default. */
    BCMLRD_FIELD_DATA_TYPE_DEFAULT,

} bcmlrd_field_data_type_t;

/*!
 * \brief Get field data by field definition information and data type.
 *
 * This routine returns the field data for the given field definition
 * information and data type.
 *
 * \param [in] fdef Field definition.
 * \param [in] type Field data type.
 * \param [in] num_alloc Number of elements allocated.
 * \param [out] field_data Pointer to field data.
 * \param [out] num_actual Number of actual elements returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_data_get_by_def_type(const bcmlrd_field_def_t *fdef,
                                  bcmlrd_field_data_type_t type,
                                  uint32_t num_alloc,
                                  uint64_t *field_data,
                                  uint32_t *num_actual);

/*!
 * \brief Get field data by data type.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  type            Field data type.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_data      Pointer to field data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_data_get_by_type(int unit,
                              const bcmlrd_sid_t sid,
                              const bcmlrd_fid_t fid,
                              const bcmlrd_field_data_type_t type,
                              const uint32_t  num_alloc,
                              uint64_t *field_data,
                              uint32_t *num_actual);

/*!
 * \brief Check if logical field is unmapped.
 *
 * This routine checks if given logical field ID is unmapped.
 * It assumes that the field ID is valid (i.e. within range).
 *
 * \param [in] map     Table map.
 * \param [in] fid     Field ID.
 *
 * \retval TRUE  Field is not mapped.
 * \retval FALSE Field is mapped.
 */
static inline bool
bcmlrd_field_is_unmapped_internal(const bcmlrd_map_t *map,
                                  bcmlrd_fid_t fid)
{
    return (!!(map->field_data->field[fid].flags & BCMLRD_FIELD_F_UNMAPPED));
}

/*!
 * \brief Get device PCM conf data.
 *
 * Get device PCM conf data.
 *
 * \retval Pointer to the current dev conf data.
 */
extern const bcmlrd_dev_pcm_conf_t*
bcmlrd_dev_pcm_conf_get(void);

/*!
 * \brief Expand the PCM compact structure.
 *
 * Expand the generated PCM compact structure to sort the
 * PCM information per LT.
 * bcmlrd_unit_pcm_conf would be populated by this function.
 *
 * \param [in]  unit        Unit number.
 *
 * \retval SHR_E_NONE       Success.
 * \retval SHR_E_UNAVAIL    Compact structure is not available.
 * \retval SHR_E_MEMORY     Memory not available for expanded structure.
 * \retval SHR_E_INTERNAL   Generated LTID is greater than table count.
 */
extern int
bcmlrd_unit_pcm_conf_expand(int unit);

/*!
 * \brief Get function pointer to get match id data.
 *
 * Get function pointer to get match id data.
 *
 * \retval Pointer to the function returning match id data.
 */
extern bcmlrd_match_id_db_get_t*
bcmlrd_dev_match_id_db_get(void);

/*!
 * \brief Get function pointer to get match id data.
 *
 * Get function pointer to get match id data.
 *
 * \retval Pointer to the function returning match id data.
 */
extern const bcmlrd_match_id_db_info_t **
bcmlrd_dev_match_id_data_all_info_get(void);

/*!
 * \brief Get device component conf data.
 *
 * Get device component conf data.
 *
 * \retval Pointer to the current dev conf data.
 */
extern const bcmlrd_dev_comp_conf_t *
bcmlrd_dev_comp_conf_get(void);

/*!
 * \brief Get the function to retrieve match id data for a unit.
 *
 * Pointer to the function to retrieve match id data for a unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval non-NULL     Pointer to configuration.
 * \retval NULL         Unit number invalid.
 */

extern bcmlrd_match_id_db_get_t
bcmlrd_unit_match_id_db_ptr_get(int unit);

/*!
 * \brief Get the function to retrieve match id data for a unit.
 *
 * Pointer to the function to retrieve match id data for a unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval non-NULL     Pointer to configuration.
 * \retval NULL         Unit number invalid.
 */

extern const bcmlrd_match_id_db_info_t *
bcmlrd_unit_match_id_data_all_info_get(int unit);

/*!
 * \brief Get a component configuration for a unit.
 *
 * Get a pointer to a component configuration for the given unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval non-NULL     Pointer to configuration.
 * \retval NULL         Unit number invalid.
 */
extern const bcmlrd_dev_comp_t *
bcmlrd_unit_comp_conf_get(int unit);

#endif /* BCMLRD_INTERNAL_H */
