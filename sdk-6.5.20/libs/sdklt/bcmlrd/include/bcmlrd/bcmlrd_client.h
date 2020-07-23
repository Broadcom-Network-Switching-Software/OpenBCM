/*! \file bcmlrd_client.h
 *
 * \brief Logical Table Resource Database Client API interfaces
 *
 * APIs tailored for Client API needs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_CLIENT_H
#define BCMLRD_CLIENT_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

/*!
 * \brief Logical table attribute
 *
 * High level table attributes
 */
typedef struct bcmlrd_table_attrib_s {
    /*! Table ID. */
    bcmlrd_sid_t id;


    enum {
        HASH,           /*!< Hash or Exact Match memory. */
        INDEX,          /*!< Indexed memory. */
        TCAM            /*!< TCAM memory. */
    } type;             /*!< Memory associated with the table. */

    /*! Maximum number of entries. */
    size_t    max_entries;

    enum {
        READ_ONLY,      /*!< Read-only table. */
        READ_WRITE      /*!< Read-write table. */
    } access_type;      /*!< Table access type. */

    /*! Number of fields in table. */
    size_t    number_of_fields;

    /*! True for interactive table. */
    bool interactive;
} bcmlrd_table_attrib_t;

/*!
 * \brief Logical table field information.
 *
 * Logical table field information suitable for client APIs.
 */
typedef bcmltd_field_def_t bcmlrd_client_field_info_t;

/*!
 * \brief Get all table attributes for a table.
 *
 * Get a list of all table attributes for a given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Table name.
 * \param [out] attrib          Attributes.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_attributes_get(int unit,
                            const char *table_name,
                            bcmlrd_table_attrib_t *attrib);

/*!
 * \brief Get the field definition for a table.
 *
 * Get a field definition for a given table.
 *
 * \param [in]  unit                    Unit number.
 * \param [in]  table_name              Table name.
 * \param [in]  number_of_fields        Number of fields alloc'ed in fields_info.
 * \param [out] fields_info             Array of field information.
 * \param [out] actual                  Number of fields in fields_info.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_fields_def_get(int unit,
                            const char *table_name,
                            size_t number_of_fields,
                            bcmlrd_client_field_info_t *fields_info,
                            size_t *actual);

/*!
 * \brief Get the field definition for a table.
 *
 * Get a table definition for a given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  num             Table name.
 * \param [out] names           Array of name pointers.
 * \param [out] actual          Number of names returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_names_get(int unit, size_t num, const char *names[],size_t *actual);

/*!
 * \brief Convert field symbol string to value.
 *
 * Convert a field symbol string to a uint32_t value.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  sym             Field symbol.
 * \param [out] val             Field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_symbol_to_value(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             const char *sym,
                             uint32_t *val);

/*!
 * \brief Convert field value to symbol string.
 *
 * Convert a symbol field uint32_t vlaue to symbol.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  val             Field value.
 * \param [out] sym             Field symbol.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_value_to_symbol(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             uint32_t val,
                             const char **sym);


/*!
 * \brief Get number of symbol values for a field.
 *
 * Get the number of symbol values for a given field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [out] count           Number of symbols.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_symbols_count_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               size_t *count);


/*!
 * \brief Get symbol values for a field.
 *
 * Get all of the symbol values for a given field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  num             Size of symbol map array.
 * \param [out] symbol_map      Array of symbol maps.
 * \param [out] actual          Number of symbol maps returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_symbols_get(int unit,
                         bcmlrd_sid_t sid,
                         bcmlrd_fid_t fid,
                         size_t num,
                         shr_enum_map_t symbol_map[],
                         size_t *actual);

/*!
 * \brief PT mapping info.
 *
 * High level table attributes
 */
typedef struct bcmlrd_pt_map_info_s {
    /*! Mapping type name. Direct/Transform */
    const char *map_type_name;

    /*! Internal field type. */
    const char *int_field_type;

    /*! Physical table ID. */
    uint32_t pt_id;

    /*! Table Map field descriptor. */
    bcmlrd_field_desc_t field;
} bcmlrd_pt_map_info_t;

/*!
 * \brief Get possible maximum number of mapped PTs.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [out] num_pt          Number of PT.
 *
 * \retval SHR_E_NONE           Success.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmlrd_table_pt_list_num_get(int unit,
                             bcmlrd_sid_t sid,
                             uint32_t *num_pt);

/*!
 * \brief Get mapped PT IDs.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  max_pt_count    Possible maximun number of PT.
 * \param [out] ptid_list       List of PT IDs.
 * \param [out] actual_pt_count Actual number of mapped PT.
 *
 * \retval SHR_E_NONE           Success.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmlrd_table_pt_list_get(int unit,
                         bcmlrd_sid_t sid,
                         uint32_t max_pt_count,
                         bcmdrd_sid_t *ptid_list,
                         uint32_t *actual_pt_count);

/*!
 * \brief Function to get number of mapped PT entries.
 *
 * This routine gets number of mapped PT entries.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  fid             Logical field ID.
 * \param [out] num_entries     Number of entries.
 *
 * \retval SHR_E_NONE           Success.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmlrd_field_pt_map_info_num_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmlrd_fid_t fid,
                                 uint32_t *num_entries);

/*!
 * \brief Function to get mapped PT entries.
 *
 * This routine gets mapped PT entries.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  fid             Logical field ID.
 * \param [in]  num_entries     Maximum number of bcmlrd_pt_map_info_t
 *                              entries in output array.
 * \param [out] ret_entries     Number of bcmlrd_pt_map_info_t entries
 *                              returned.
 * \param [out] entries         Destination PT entries.
 *
 * \retval SHR_E_NONE           Success.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmlrd_field_pt_map_info_get(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             uint32_t num_entries,
                             uint32_t *ret_entries,
                             bcmlrd_pt_map_info_t *entries);

/*!
 * \brief Function to check if the physical field is magic field.
 *
 * This routine checks if the physical field is magic field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  pt_fid          Physical Field ID.
 *
 * \retval TRUE                 Given field ID is magic field.
 * \retval FALSE                Given field ID is not magic field.
 */
extern bool
bcmlrd_field_is_magic_field(int unit,
                            uint32_t pt_fid);

/*!
 * \brief Function to get magic field name.
 *
 * This routine gets magic field name.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  pt_fid          Physical Field ID.
 *
 * \retval TRUE                 Given field ID is magic field.
 * \retval FALSE                Given field ID is not magic field.
 */
extern const char*
bcmlrd_field_magic_field_name_get(int unit,
                                  uint32_t pt_fid);

/*!
 * \brief Function to check if the physical field is internal field.
 *
 * This routine checks if the physical field is internal field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  pt_fid          Physical Field ID.
 *
 * \retval TRUE                 Given field ID is internal field.
 * \retval FALSE                Given field ID is not internal field.
 */
extern bool
bcmlrd_field_is_internal_field(int unit,
                               uint32_t pt_fid);

/*!
 * \brief Get symbol name for given field.
 *
 * This routine gets the symbol type name for a given table and field.
 *
 * \param [in]  unit Unit number.
 * \param [in]  sid  Logical table ID.
 * \param [in]  fid  Field ID.
 * \param [out] name enum type name.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM   Output pointer NULL, field not a symbol.
 * \retval SHR_E_UNAVAIL sid or fid is invalid.
 */
extern int
bcmlrd_enum_symbol_name(int unit,
                        uint32_t sid,
                        uint32_t fid,
                        const char **name);

#endif /* BCMLRD_CLIENT_H */
