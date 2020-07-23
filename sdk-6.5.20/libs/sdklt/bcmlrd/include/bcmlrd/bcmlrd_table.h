/*! \file bcmlrd_table.h
 *
 * \brief Logical Table Resource Database
 *
 * Logical Table data
 *
 * Data structures are roughly modeled on CDK data. Biggest departure
 * is likely to be the use of "symbol local" field IDs which index
 * into the field structure directly.  This is intended to be used by
 * generated code.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_TABLE_H
#define BCMLRD_TABLE_H

#include <sal/sal_types.h>
#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief Get a count of all table IDs for a unit.
 *
 * Get a count of all logical table IDs for a given unit.
 *
 * \param [in]  unit            Unit number.
 * \param [out] num_sid         Number of table IDs.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_count_get(int unit, size_t *num_sid);

/*!
 * \brief Get all table IDs for a table.
 *
 * Get a list of all table IDs for a given unit.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  list_max        Table IDs allocated in sid_list.
 * \param [out] sid_list        List of table IDs.
 * \param [out] num_sid         Number of table IDs returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_list_get(int unit, size_t list_max,
                      bcmlrd_sid_t *sid_list, size_t *num_sid);

/*!
 * \brief Get the table size for a logical table.
 *
 * Get the table size for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] size            Size.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size);

/*!
 * \brief Get the maximum index for a logical table.
 *
 * Get the maximum index for a logical table.
 *
 * What happens if this is inappropriate for the table? Error?
 * Is this truly a fixed property, or can this be changed on
 * a per-unit basis?
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] max_index       Maximum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index);

/*!
 * \brief Get the minimum index for a logical table.
 *
 * Get the minimum index for a logical table.
 *
 * What happens if this is inappropriate for the table? Error?
 * Is this truly a fixed property, or can this be changed on
 * a per-unit basis?
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] min_index       Minimum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_min_index_get(int unit, bcmlrd_sid_t sid, uint32_t *min_index);

/*!
 * \brief Get the port index range size for a logical table.
 *
 * Get the port index range size for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] size            Size.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_port_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size);

/*!
 * \brief Get the maximum port index for a logical table.
 *
 * Get the maximum port index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] max_index       Maximum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_port_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index);

/*!
 * \brief Get the minimum port index for a logical table.
 *
 * Get the minimum port index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] min_index       Minimum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_port_min_index_get(int unit, bcmlrd_sid_t sid, uint32_t *min_index);

/*!
 * \brief Get the instance index range size for a logical table.
 *
 * Get the instance index range size for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] size            Size.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_instance_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size);

/*!
 * \brief Get the maximum instance index for a logical table.
 *
 * Get the maximum instance index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] max_index       Maximum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_instance_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index);

/*!
 * \brief Get the minimum instance index for a logical table.
 *
 * Get the minimum instance index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] min_index       Minimum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_instance_min_index_get(int unit, bcmlrd_sid_t sid, uint32_t *min_index);


/*!
 * \brief Get a count of all field IDs for a unit and table ID
 *
 * Get a count of fields for a given table ID
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] num_fid         Number of field IDs in table.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_count_get(int unit, bcmlrd_sid_t sid, size_t *num_fid);

/*!
 * \brief Get all field IDs for a table.
 *
 * Get a list of all table IDs for a given unit.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [in]  list_max        Number of fields IDs allocated.
 * \param [out] fid_list        List of field IDs.
 * \param [out] num_fid         Number of field IDs returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_list_get(int unit, bcmlrd_sid_t sid, size_t list_max,
                      bcmlrd_fid_t *fid_list, size_t *num_fid);

/*!
 * \brief Return the index for a table name.
 *
 * Get the table index (SID) for the given table name on the given
 * unit.
 *
 * \param [in] unit             Unit number.
 * \param [in] table_name       Logical Table name.
 *
 * \retval >= 0                 Table index.
 * \retval SHR_E_UNAVAIL        Table not found.
 */

extern int
bcmlrd_table_name_to_idx(int unit, const char *table_name);

/*!
 * \brief Return the table and field index for a table and field names.
 *
 * For the given table and field names, get the table ID and
 * ID of the field within the logical table.
 *
 * \param [in]  unit            Unit Number.
 * \param [in]  table_name      Logical Table name.
 * \param [in]  field_name      Logical Field name.
 * \param [out] sid             Logical Table ID.
 * \param [out] fid             Field ID within the LT.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmlrd_table_field_name_to_idx(int unit,
                               const char *table_name,
                               const char *field_name,
                               bcmltd_sid_t *sid,
                               bcmltd_fid_t *fid);

/*!
 * \brief Get table info for a given table.
 *
 * \param [in ] sid             Table IDs.
 *
 * \retval non-NULL     Table definition.
 * \retval NULL         Illegal Table ID.
 */
extern const bcmlrd_table_rep_t *
bcmlrd_table_get(bcmlrd_sid_t sid);

/*!
 * \brief Initialize the Logical Resource Database.
 *
 * Initialize the Logical Resource Database.
 *
 * Must be called first before any other LRD calls.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_init(void);

/*!
 * \brief Clean up the Logical Resource Database.
 *
 * Release any resources allocated by the LRD. No other LRD calls
 * other than bcmlrd_init can be called after this.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_cleanup(void);

/*!
 * \brief Get the number of field indexes for given field width.
 *
 * Return the number of field indexes required for given field width.
 *
 * \param [in] width Field width in bits.
 *
 * \retval Number of field indexes.
 */
uint32_t
bcmlrd_width_field_idx_count_get(uint32_t width);

/*!
 * \brief Get the number of field indexes for given field ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [in]  fid             Field ID.
 *
 * \retval idx_count Number of field indexes.
 */
extern uint32_t
bcmlrd_field_idx_count_get(int unit, bcmlrd_sid_t sid, bcmlrd_fid_t fid);

/*!
 * \brief Get field def information.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [in]  fid             Field ID.
 * \param [out] field_def       Field Def data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_field_def_get(int unit,
                           const bcmlrd_sid_t sid,
                           const bcmlrd_fid_t fid,
                           bcmlrd_field_def_t *field_def);

/*!
 * \brief Get field def information by given table field data
 *        and map field data.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  field_id        Field ID.
 * \param [in]  tbl             Table Field data.
 * \param [in]  map             Map Field data.
 * \param [out] field_def       Field Def data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_def_get(int unit,
                     const bcmlrd_fid_t field_id,
                     const bcmlrd_table_rep_t *tbl,
                     const bcmlrd_map_t *map,
                     bcmlrd_field_def_t *field_def);

/*!
 * \brief Get the interactive flag for a table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] interactive     Interactive flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_interactive_get(int unit, uint32_t sid, bool *interactive);

/*!
 * \brief Get field default data
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_def       Pointer to field default data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int
bcmlrd_field_default_get(int unit,
                         const bcmlrd_sid_t sid,
                         const bcmlrd_fid_t fid,
                         const uint32_t num_alloc,
                         uint64_t* field_def,
                         uint32_t* num_actual);

/*!
 * \brief Get field max data
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_max       Pointer to field max data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int
bcmlrd_field_max_get(int unit,
                     const bcmlrd_sid_t sid,
                     const bcmlrd_fid_t fid,
                     const uint32_t num_alloc,
                     uint64_t* field_max,
                     uint32_t* num_actual);

/*!
 * \brief Get field min data
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_min       Pointer to field min data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int
bcmlrd_field_min_get(int unit,
                     const bcmlrd_sid_t sid,
                     const bcmlrd_fid_t fid,
                     const uint32_t num_alloc,
                     uint64_t* field_min,
                     uint32_t* num_actual);

/*!
 * \brief Function to check if the logical field in unmapped.
 *
 * This routine checks if the logical field is unmapped
 * for the given unit, sid, and fid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical Table symbol ID.
 * \param [in]  fid             Logical Field ID.
 * \param [out] unmapped        Logical field MAP status. TRUE for UNMAPPED.
 *
 * \retval SHR_E_NONE           field MAP status is valid.
 * \retval SHR_E_UNAVAIL        unit/table/field not found.
 */
extern int bcmlrd_field_is_unmapped(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmlrd_fid_t fid,
                                    bool *unmapped);

/*!
 * \brief Return the PCM configuration for the given table.
 *
 * This routine returns the PCM configuration
 * for the given unit, sid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical Table symbol ID.
 * \param [out] pcm_info        PCM configuration.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Unit/Table/PCM configuration not found.
 */
extern int
bcmlrd_table_pcm_conf_get(int unit,
                          bcmlrd_sid_t sid,
                          const bcmlrd_table_pcm_info_t **pcm_info);

/*!
 * \brief Return the match id information.
 *
 * This routine returns the match id information
 * for the given unit, table and field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical Table symbol ID.
 * \param [in]  fid             Logical field symbol ID.
 * \param [out] info        Match ID data.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_PARAM          Invalid unit, sid, fid or info.
 * \retval SHR_E_UNAVAIL        Match ID data is not found.
 */
extern int
bcmlrd_table_match_id_db_get(int unit,
                             const bcmlrd_sid_t sid,
                             const bcmlrd_fid_t fid,
                             const bcmlrd_match_id_db_t **info);

/*!
 * \brief Return the match id information.
 *
 * This routine returns the match id information
 * for the given match id in string format.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  spec            Match ID name.
 * \param [out] info            Match ID data.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_PARAM          Invalid unit, sid, fid or info.
 * \retval SHR_E_NOT_FOUND      Match ID data is not found.
 */
extern int
bcmlrd_table_match_id_data_get(int unit,
                               const char *spec,
                               const bcmlrd_match_id_db_t **info);

/*!
 * \brief Return list of logical tables for given combination of
 * component name and handler name.
 *
 * This function returns the list of logical tables for the given
 * combination of component name and handler name.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  component_name  Component name.
 * \param [in]  handler_name    Handler name.
 * \param [in]  num_tables      Number of allocated entries for table list.
 * \param [out] tables          Pointer to the list of logical tables.
 * \param [out] actual          Actual number of logical tables.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNIT           Invalid unit.
 * \retval SHR_E_PARAM          Invalid input parameters.
 * \retval SHR_E_MEMORY         Actual number of tables is greater than num_tables.
 * \retval SHR_E_UNAVAIL        Given combination of component and handler
 *                              does not have any logical table.
 */
extern int
bcmlrd_custom_tables_get(int unit,
                         const char *component_name,
                         const char *handler_name,
                         size_t num_tables,
                         bcmlrd_sid_t *tables,
                         size_t *actual);

/*!
 * \brief Return count of logical tables for given combination of
 * component name and handler name.
 *
 * This function returns the count of logical tables for the given
 * combination of component name and handler name.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  component_name  Component name.
 * \param [in]  handler_name    Handler name.
 * \param [out] count           Actual number of logical tables.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNIT           Invalid unit.
 * \retval SHR_E_PARAM          Invalid input parameters.
 * \retval SHR_E_UNAVAIL        Given combination of component and handler
 *                              does not have any logical table.
 */
extern int
bcmlrd_custom_tables_count_get(int unit,
                               const char *component_name,
                               const char *handler_name,
                               size_t *count);

/*!
 * \brief Callback for LTM function to retrieve LT in-use count
 *        for CTH logic.
 *
 * \param [in] unit Logical device ID.
 * \param [in] trans_id Transaction identifier.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] inuse_count Return pointer for LT in-use count.
 */
typedef int
(*bcmlrd_table_inuse_count_get_cb)(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t ltid,
                                   uint32_t *inuse_count);

/*!
 * \brief Register a callback for LTM function to provide in-use
 *        count for direct-mapped LTs.
 *
 * PT resource reallocation, which may resize LTs, must check that the
 * affected LTs are empty before eliminating associate resources.
 * For direct-mapped LTs, the LTM is the RM.  This callback permits
 * the lower-layer CTH logic access to this information.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_inuse_get_cb Callback function to fetch in-use count
 *             for a given LT.
 *
 * \retval None
 */
extern void
bcmlrd_table_inuse_count_get_register(int unit,
                          bcmlrd_table_inuse_count_get_cb lt_inuse_get_cb);

/*!
 * \brief Retrieve LT in-use count for CTH logic.
 *
 * This function is the wrapper for the LTM callback to provide
 * the in-use count of a LT.
 *
 * \param [in] unit Logical device ID.
 * \param [in] trans_id Transaction identifier.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] inuse_count Return pointer for LT in-use count.
 */
extern int
bcmlrd_table_inuse_count_get(int unit,
                             uint32_t trans_id,
                             bcmltd_sid_t ltid,
                             uint32_t *inuse_count);

/*!
 * \brief Get local field ID for given table and global field ID.
 *
 * Get local field ID for given table and global field ID.
 *
 * \param [in] unit             Logical device ID.
 * \param [in] sid              Logical Table ID.
 * \param [in] gfid             Logical field global ID.
 * \param [out] fid             Pointer to logical field local ID.
 *
 * \retval SHE_E_NONE           Operation successful.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmlrd_fid_get(int unit,
               bcmltd_sid_t sid,
               bcmltd_gfid_t gfid,
               bcmltd_fid_t *fid);

/*!
 * \brief Get global field ID for given table and local field ID.
 *
 * Get global field ID for given table and local field ID.
 *
 * \param [in] unit             Logical device ID.
 * \param [in] sid              Logical Table ID.
 * \param [in] fid              Logical field local ID.
 * \param [out] gfid            Pointer to logical field global ID.
 *
 * \retval SHE_E_NONE           Operation successful.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmlrd_gfid_get(int unit,
                bcmltd_sid_t sid,
                bcmltd_fid_t fid,
                bcmltd_gfid_t *gfid);

/*!
 * \brief Return the field index for a table ID and field name.
 *
 * For the given table ID and field name, get the field ID.
 *
 * \param [in]  unit            Unit Number.
 * \param [in]  sid             Logical Table ID.
 * \param [in]  field_name      Logical Field name.
 * \param [out] fid             Field ID.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmlrd_field_name_to_idx(int unit,
                         bcmltd_sid_t sid,
                         const char *field_name,
                         bcmltd_fid_t *fid);

/*!
 * \brief Return the field name for a table and field ID.
 *
 * Return the field name for a table and field ID.
 *
 * \param [in]  unit            Unit Number.
 * \param [in]  sid             Logical Table ID.
 * \param [in]  fid             Logical Field ID.
 * \param [out] field_name      Field name.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmlrd_field_idx_to_name(int unit,
                         bcmltd_sid_t sid,
                         bcmltd_fid_t fid,
                         const char **field_name);

#endif /* BCMLRD_TABLE_H */
