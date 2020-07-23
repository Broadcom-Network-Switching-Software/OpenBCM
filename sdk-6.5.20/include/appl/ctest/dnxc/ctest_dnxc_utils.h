/*
 * ! \file ctest_dnxc_util.h Contains common dnxc ctest utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef CTEST_DNXC_UTILS_H_INCLUDED
#define CTEST_DNXC_UTILS_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_rhlist.h>

shr_error_e ctest_dnxc_init_deinit(
    int unit,
    char *command);

/**
 * \brief Control structure for command keyword definition, provided by framework
 */
typedef struct
{
    char *property;
    char *value;
} ctest_soc_property_t;

typedef struct
{
    rhentry_t entry;
    char *value;
} ctest_soc_entry_t;

typedef struct
{
    sal_field_type_e type;
    union
    {
        int32 value_int32;
        uint32 value_uint32;
        char *value_str_ptr;
    } value;
} ctest_cint_argument_t;

/**
 * \brief Modify soc properties according to provided list of names/values and perform deinit/init to bring them into
 *              action
 * \param [in] unit - device ID
 * \param [in] ctest_soc_property_a - pointer to the list of properties - name/value pair,
 *                                                                              last one name should be NULL or empty
 * \param [out] ctest_soc_set_h - place where handle to be used in restore will be put, if the handle is NULL,
 *                                                                              no changes in soc properties happened
 * \return
 *   usual shr_error_e
 * \remark
 *   ctest_dnxc_restore_soc_properties should always be called if the handle is not NULL, it restore the previous state
 *   and frees the resources, it may be called even if it is NULL, nothing bad will happen
 * \see
 *   ctest_dnxc_restore_soc_properties
 */
shr_error_e ctest_dnxc_set_soc_properties(
    int unit,
    ctest_soc_property_t * ctest_soc_property_a,
    rhhandle_t * ctest_soc_set_h);

/**
 * \brief Return soc properties to the state previous to ctest_dnxc_set_soc_properties and invoke deinit/init
 * \param [in] unit - device ID
 * \param [out] ctest_soc_set_h - handle that points to the structure keeping record of previous so property state
 * \return
 *   usual shr_error_e
 * \remark
 * \see
 *   ctest_dnxc_set_soc_properties
 */
shr_error_e ctest_dnxc_restore_soc_properties(
    int unit,
    rhhandle_t ctest_soc_set_h);

/**
 * \brief Load cint source file either from under $SDK/src/examples or as SA file
 *
 * \param [in] unit - device ID
 * \param [in] cint_filename - file name should either include path from $SDK/src/examples
 *                              or to be available as is (absolute or relative to local)
 * \return
 *   usual shr_error_e
 * \remark
 */
shr_error_e ctest_dnxc_cint_load(
    int unit,
    char *cint_filename);

/**
 * \brief Load cint source file either from under $SDK/src/examples or as SA file
 *
 * \param [in] unit - device ID
 * \param [in] main_name - ctest routine to be invoked
 * \param [in] cint_arguments - pointer to array of cint arguments, with type and value per argument, assumption s that
 *                              unit is always the first parameter, so start array from the parameter following unit
 * \param [in] nof_arguments - number of arguments in the array
 * \return
 *   usual shr_error_e
 * \remark
 */
shr_error_e ctest_dnxc_cint_run(
    int unit,
    char *main_name,
    ctest_cint_argument_t * cint_arguments,
    int nof_arguments);

/**
 * \brief Reset cint - actually run cint_reset();
 *
 * \param [in] unit - device ID

 * \return
 *   usual shr_error_e
 * \remark
 */
shr_error_e ctest_dnxc_cint_reset(
    int unit);

/**
 * \brief Perform arbitrary command that can be run inside cint interpreter
 *
 * \param [in] command - command to perform by cint interpreter

 * \return
 *   According to regular shr_error
 * \remark
 */
shr_error_e ctest_dnxc_cint_cmd(
    char *command);

#endif /* CTEST_DNXC_UTILS_H_INCLUDED */
