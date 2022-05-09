/*
 * ! \file ctest_dnxc_util.h Contains common dnxc ctest utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef CTEST_DNXC_UTILS_H_INCLUDED
#define CTEST_DNXC_UTILS_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_rhlist.h>
#include <bcm/port.h>

#define MAX_NUM_SOC_PROPERTIES 200
/* chars need to represent max uint32 = 0xffffffff */
#define MAX_CHARS_UINT32 11

#define TEST_PASSED(test_name){\
    LOG_CLI((BSL_META("\n" \
                      "*************************************************************\n" \
                      "*** TEST  %-36s - PASSED - ***\n" \
                      "*************************************************************\n\n"),test_name));\
}
#define TEST_STARTED(test_name){\
    LOG_CLI((BSL_META("\n" \
                      "*************************************************************\n" \
                      "*** TEST  %-35s - STARTED - ***\n" \
                      "*************************************************************\n\n"),test_name));\
}
#define TEST_FAILED(test_name){\
    LOG_CLI((BSL_META("\n"                                                           \
                      "*************************************************************\n" \
                      "*** TEST  %-36s - FAILED - ***\n" \
                      "*************************************************************\n\n"),test_name));\
}

#define TEST_FAILED_WITH_ERROR(unit, test_name, dump_func, error, ...){\
    TEST_FAILED(test_name);\
    dump_func;\
    LOG_CLI((BSL_META("\n\n")));\
    SHR_ERR_EXIT(error, __VA_ARGS__);\
    LOG_CLI((BSL_META("\n\n")));\
}

shr_error_e ctest_dnxc_init_deinit(
    int unit,
    char *command);

/**
 * \brief Control structure for command keyword definition, provided by framework
 * Value can provided as string or integer.
 * In case of integer value, please put "" in value field
 */
typedef struct
{
    char *property;
    char *value;
    int int_value;
} ctest_soc_property_t;

typedef struct
{
    char property[RHNAME_MAX_SIZE];
    char value[RHNAME_MAX_SIZE];
} ctest_soc_property_arr_t;

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
 * \brief Add soc properties to list according to provided names/values
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
shr_error_e ctest_dnxc_soc_add(
    int unit,
    ctest_soc_property_t * ctest_soc_property_p,
    char *soc_name,
    char *soc_value);

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
 * \brief
 *   Return soc properties to the state previous to ctest_dnxc_set_soc_properties without running
 *   init_deinit to apply it
 * \param [in] unit - device ID
 * \param [out] ctest_soc_set_h - handle that points to the structure keeping record of previous so property state
 * \return
 *   usual shr_error_e
 * \remark
 * \see
 *   ctest_dnxc_reset_soc_properties
 */

shr_error_e ctest_dnxc_reset_soc_properties_no_init(
    int unit,
    rhhandle_t ctest_soc_property_set_h);

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

/**
 * \brief - Save port resource and enable and detach ports
 * 
 * \param [in] unit - unit
 * \param [in] pbmp - bitmap of ports to be detached
 * \param [in] ports_resources - array to be filled with port resources
 * \param [in] ports_enable_state - array to be filled with port enable statuses
 * \param [out] detached_pbmp - detached pbmp (returned by bcm_port_detach)
 * \return shr_error_e 
 * \remark - need to allocate memory for ports_resources and ports_enable_state
 */
shr_error_e ctest_dnxc_port_save_resource_and_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_port_resource_t * ports_resources,
    int *ports_enable_state,
    bcm_pbmp_t * detached_pbmp);

/**
 * \brief - Probe ports and restore resources and enable status
 * 
 * \param [in] unit - unit
 * \param [in] detached_pbmp - bitmap of detached ports
 * \param [in] ports_resources - array of port resources
 * \param [in] ports_enable_state - array of enable statuses
 * \return shr_error_e 
 * \remark - need to have enough members in ports_resources and ports_enable_state
 */
shr_error_e ctest_dnxc_port_probe_and_restore_resource(
    int unit,
    bcm_pbmp_t detached_pbmp,
    bcm_port_resource_t * ports_resources,
    int *ports_enable_state);

#endif /* CTEST_DNXC_UTILS_H_INCLUDED */
