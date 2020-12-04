/** \file algo_field_key.c
 * 
 *
 * Field procedures for DNX.
 *
 * Will hold the needed algorithm functions for Field module
 * related to the PMF key and FFCs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/field/ffc_alloc_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <sal/appl/sal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>

/*
 * }
 */

/*
 * { Defines
 */

/**
 * Number of 'single-key's in one double key (320 bits)
 */
#define DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE (DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE/DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)

/*
 * } Defines
 */

/*
 * MACROs
 * {
 */

/**
 * \brief
 *   Converts context and key to a the index of an indexed allocation manager.
 *
 * \param [in] _context_id           - The context to allocate in.
 * \param [in] _key_id               - The key to allocate in.
 * \param [in] _nof_keys             - The number of keys per context for the allocation manager pool.
 * \param [in] _first_key            - The first usable key in the allocation manager pool.
 * \param [out] _sub_resource_index  - The index to the allocation manager pool, containing oth key and context.
 * \return
 *   Error if the parameters are invalid.
 */
#define DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(_context_id, _key_id, _nof_keys, _first_key, _sub_resource_index) \
{ \
    if ((((int) (_context_id)) < 0) || (((int) (_key_id)) < 0) || \
        (((int) (_nof_keys)) < 0) || (((int) (_first_key)) < 0)) \
    { \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_KEY_ALLOC_POOL_INDEX: Input agruments must be non negative. " \
                     "_context_id %d _key_id %d _nof_keys %d _first_key %d.\r\n", \
                     (_context_id), (_key_id), (_nof_keys), (_first_key)); \
    } \
    if ((_key_id) < (_first_key)) \
    { \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_KEY_ALLOC_POOL_INDEX: Key %d bellow first key %d.\r\n", \
                     (_key_id), (_first_key)); \
    } \
    if ((_key_id) - (_first_key) >= (_nof_keys)) \
    { \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_KEY_ALLOC_POOL_INDEX: Key %d first key %d, " \
                     "but maximum number of keys is %d.\r\n", \
                     (_key_id), (_first_key), (_nof_keys)); \
    } \
    (_sub_resource_index) = (((_context_id) * (_nof_keys)) + ((_key_id)-(_first_key))); \
}

/*
 * }MACROs
 */

/**
* \brief
*  Init simple alloc algorithm for FFC id for PMF1
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf_1_ffc_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_ffc_ipmf_1_alloc_mngr_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for PMF2
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf_2_ffc_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_ffc;
    data.flags = 0;

    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF_2_FFC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.alloc(unit,
                                                        dnx_data_field.stage.stage_info_get(unit,
                                                                                            DNX_FIELD_STAGE_IPMF2)->nof_contexts));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.create(unit, sub_resource_id, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for IPMF3
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf_3_ffc_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_ffc;

    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF_3_FFC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.alloc(unit,
                                                        dnx_data_field.stage.stage_info_get(unit,
                                                                                            DNX_FIELD_STAGE_IPMF2)->nof_contexts));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for EPMF
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_epmf_ffc_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_ffc;

    sal_strncpy(data.name, DNX_ALGO_FIELD_EPMF_FFC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.alloc(unit,
                                                      dnx_data_field.stage.stage_info_get(unit,
                                                                                          DNX_FIELD_STAGE_EPMF)->nof_contexts));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_contexts;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for IFWD2
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ifwd2_ffc_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_ffc;

    sal_strncpy(data.name, DNX_ALGO_FIELD_IFWD2_FFC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.alloc(unit,
                                                       dnx_data_field.stage.stage_info_get(unit,
                                                                                           DNX_FIELD_STAGE_EXTERNAL)->nof_contexts));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_initial_key_occupation_bmp_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    int context_id;
    int nof_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    int key_id;
    int nof_keys = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    int nof_initial_keys = nof_keys - nof_keys_alloc;
    int first_initial_key = nof_keys_alloc;
    int sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    /*
     * Key size here represents the 160 bit of data we can pass to ipfm2 as initial data buffer
     */
    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    if (sal_strnlen(DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_OCC_BMP, sizeof(data.name)) >= sizeof(data.name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_OCC_BMP string is too large.\r\n");
    }

    sal_strncpy_s(data.name, DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_OCC_BMP, sizeof(data.name));

    /** Need to allocate FFC per context hence creating indexed pool. We also allocate per key. */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_occ_bmp.alloc(unit, (nof_contexts * nof_initial_keys)));
    for (context_id = 0; context_id < nof_contexts; context_id++)
    {
        for (key_id = first_initial_key; key_id < (first_initial_key + nof_initial_keys); key_id++)
        {
            DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_initial_keys, first_initial_key,
                                                        sub_resource_id);
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_occ_bmp.create(unit, sub_resource_id, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_key_occupation_bmp_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    int context_id;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_IPMF1;
    int nof_contexts = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
    int key_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key = DBAL_ENUM_FVAL_FIELD_KEY_A;
    int sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    if (sal_strnlen(DNX_ALGO_FIELD_IPMF1_KEY_OCC_BMP, sizeof(data.name)) >= sizeof(data.name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_IPMF1_KEY_OCC_BMP string is too large.\r\n");
    }

    sal_strncpy_s(data.name, DNX_ALGO_FIELD_IPMF1_KEY_OCC_BMP, sizeof(data.name));

    /** Need to allocate FFC per context hence creating indexed pool. We also allocate per key. */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_key_occ_bmp.alloc(unit, (nof_contexts * nof_keys_alloc)));
    for (context_id = 0; context_id < nof_contexts; context_id++)
    {
        for (key_id = first_key; key_id < (first_key + nof_keys_alloc); key_id++)
        {
            DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_key_occ_bmp.create(unit, sub_resource_id, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf2_key_occupation_bmp_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    int context_id;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_IPMF2;
    int nof_contexts = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
    int key_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key = DBAL_ENUM_FVAL_FIELD_KEY_A;
    int sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    if (sal_strnlen(DNX_ALGO_FIELD_IPMF2_KEY_OCC_BMP, sizeof(data.name)) >= sizeof(data.name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_IPMF2_KEY_OCC_BMP string is too large.\r\n");
    }

    sal_strncpy_s(data.name, DNX_ALGO_FIELD_IPMF2_KEY_OCC_BMP, sizeof(data.name));

    /** Need to allocate FFC per context hence creating indexed pool. We also allocate per key. */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_key_occ_bmp.alloc(unit, (nof_contexts * nof_keys_alloc)));
    for (context_id = 0; context_id < nof_contexts; context_id++)
    {
        for (key_id = first_key; key_id < (first_key + nof_keys_alloc); key_id++)
        {
            DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_key_occ_bmp.create(unit, sub_resource_id, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf3_key_occupation_bmp_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    int context_id;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_IPMF3;
    int nof_contexts = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
    int key_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key = DBAL_ENUM_FVAL_FIELD_KEY_A;
    int sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    if (sal_strnlen(DNX_ALGO_FIELD_IPMF3_KEY_OCC_BMP, sizeof(data.name)) >= sizeof(data.name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_IPMF3_KEY_OCC_BMP string is too large.\r\n");
    }

    sal_strncpy_s(data.name, DNX_ALGO_FIELD_IPMF3_KEY_OCC_BMP, sizeof(data.name));

    /** Need to allocate FFC per context hence creating indexed pool. We also allocate per key. */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_key_occ_bmp.alloc(unit, (nof_contexts * nof_keys_alloc)));
    for (context_id = 0; context_id < nof_contexts; context_id++)
    {
        for (key_id = first_key; key_id < (first_key + nof_keys_alloc); key_id++)
        {
            DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_key_occ_bmp.create(unit, sub_resource_id, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_epmf_key_occupation_bmp_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    int context_id;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_EPMF;
    int nof_contexts = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
    int key_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key = DBAL_ENUM_FVAL_FIELD_KEY_A;
    int sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    if (sal_strnlen(DNX_ALGO_FIELD_EPMF_KEY_OCC_BMP, sizeof(data.name)) >= sizeof(data.name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_EPMF_KEY_OCC_BMP string is too large.\r\n");
    }

    sal_strncpy_s(data.name, DNX_ALGO_FIELD_EPMF_KEY_OCC_BMP, sizeof(data.name));

    /** Need to allocate FFC per context hence creating indexed pool. We also allocate per key. */
    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_key_occ_bmp.alloc(unit, (nof_contexts * nof_keys_alloc)));
    for (context_id = 0; context_id < nof_contexts; context_id++)
    {
        for (key_id = first_key; key_id < (first_key + nof_keys_alloc); key_id++)
        {
            DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
            SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_key_occ_bmp.create(unit, sub_resource_id, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_key_lookup_occupation_bmp_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    if (sal_strnlen(DNX_ALGO_FIELD_KEY_LOOKUP_OCC_BMP, sizeof(data.name)) >= sizeof(data.name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_ALGO_FIELD_KEY_LOOKUP_OCC_BMP string is too large.\r\n");
    }

    sal_strncpy_s(data.name, DNX_ALGO_FIELD_KEY_LOOKUP_OCC_BMP, sizeof(data.name));

    /** Need to allocate FFC per context hence creating indexed pool. We also allocate per key. */
    SHR_IF_ERR_EXIT(algo_field_info_sw.key_lookup_occ_bmp.alloc(unit, DNX_FIELD_STAGE_NOF));
    for (field_stage = 0; field_stage < DNX_FIELD_STAGE_NOF; field_stage++)
    {
        int nof_contexts = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts;
        int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
        data.nof_elements = nof_contexts * nof_keys_alloc;
        if (data.nof_elements != 0)
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.key_lookup_occ_bmp.create(unit, field_stage, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_key_res_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**IPMF_1 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf_1_ffc_init(unit));
    /**IPMF_2 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf_2_ffc_init(unit));
    /**IPMF_3 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf_3_ffc_init(unit));
    /**EPMF FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_epmf_ffc_init(unit));
    /**IFWD2 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ifwd2_ffc_init(unit));

    /**IPMF1 INITIAL KEY OCCUPATION BITMAP FOR BITWISE ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_initial_key_occupation_bmp_init(unit));

    /** KEY OCCUPATION BITMAP FOR BITWISE ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_key_occupation_bmp_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf2_key_occupation_bmp_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf3_key_occupation_bmp_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_epmf_key_occupation_bmp_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_key_lookup_occupation_bmp_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_key_res_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * All sw resources will be deinit together.
     */
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
/*
 * Procedures related to allocation of keys.
 * {
 */
/*
 * Verify procedure for dnx_algo_field_key_alloc_generic().
 * See its header for description of parameters.
 * \remark
 */
static shr_error_e
dnx_algo_field_key_alloc_generic_verify(
    int unit,
    dnx_algo_field_key_flags_e flags,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2) &&
        (field_stage != DNX_FIELD_STAGE_IPMF3) && (field_stage != DNX_FIELD_STAGE_EPMF))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                     dnx_field_stage_e_get_name(field_stage));
    }
    if ((unsigned int) key_length_type >= DNX_FIELD_KEY_LENGTH_TYPE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key length type (%d, larger than max: %d)\r\n",
                     (unsigned int) key_length_type, DNX_FIELD_KEY_LENGTH_TYPE_NOF - 1);
    }
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    if (flags & DNX_ALGO_FIELD_KEY_FLAG_WITH_ID)
    {
        if ((unsigned int) key_id_p->id[0] >= DBAL_NOF_ENUM_FIELD_KEY_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key (%d) while using with_id flag\r\n", (unsigned int) key_id_p->id[0]);
        }
        if ((key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE) &&
            ((unsigned int) key_id_p->id[1] >= DBAL_NOF_ENUM_FIELD_KEY_VALUES))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal second key (%d) while using with_id flag for double key\r\n",
                         (unsigned int) key_id_p->id[1]);
        }
    }
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Direct extraction attempted to allocate not by bit.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Verify procedure for dnx_algo_field_key_bit_range_alloc_generic().
 * See its header for description of parameters.
 * \remark
 */
static shr_error_e
dnx_algo_field_key_bit_range_alloc_generic_verify(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 key_length,
    dnx_algo_field_key_bit_range_align_t * align_info_p,
    int force_bit_selection,
    dnx_field_key_id_t *key_id_p)
{
    uint8 structure_is_valid;
    uint8 nof_available_keys;
    uint8 first_available_key;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(align_info_p, _SHR_E_PARAM, "align_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    /*
     * Deduce the number of elements of the configuration array, keys_availability_info
     * (in sw state), directly from its definition, as created by sw state.
     */
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2) &&
        (field_stage != DNX_FIELD_STAGE_IPMF3) && (field_stage != DNX_FIELD_STAGE_EPMF))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                     dnx_field_stage_e_get_name(field_stage));
    }
    if (key_length < DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key length size (%d, smaller than minimum: %d)\r\n",
                     (unsigned int) key_length, DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH);
    }
    if ((unsigned int) key_length > (unsigned int) DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key length size (%d, larger than max: %d)\r\n", (unsigned int) key_length,
                     DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE);
    }
    /*
     * Verify that we have information for allocating the bits.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.structure_is_valid.get(unit, field_stage, fg_type,
                                                                        &structure_is_valid));
    if (!(structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Uninitialized field group type (%d, %s) under assigned stage (%d, %s)\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.get(unit, field_stage, fg_type,
                                                                        &nof_available_keys));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.get(unit, field_stage, fg_type,
                                                                         &first_available_key));
    if (nof_available_keys <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "field group type\"%s\" is not supported on stage \"%s\".\r\n",
                     dnx_field_group_type_e_get_name(fg_type), dnx_field_stage_e_get_name(field_stage));
    }

    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    if (align_info_p->align_lsb)
    {
        /*
         * Enter if LSB alignment is required for some bit on the bit-range (so the
         * 'key select' mechanism can make it the first in a 32-bits 'chunk')
         */
        if (align_info_p->align_lsb_step <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "LSB Alignment is required but slignment resolution isn't positive (%d).\r\n",
                         align_info_p->align_lsb_step);
        }
        if (align_info_p->lsb_aligned_bit <= ((-1) * align_info_p->align_lsb_step))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Alignment is required but 'lsb_aligned_bit' (%d) is negative less than the reslution (%d).\r\n",
                         align_info_p->lsb_aligned_bit, align_info_p->align_lsb_step);
        }
        if (align_info_p->lsb_aligned_bit >= key_length)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Alignment is required but 'lsb_aligned_bit' (%d) is larger than the number of bits (%d)\r\n",
                         align_info_p->lsb_aligned_bit, key_length);
        }
    }
    if (align_info_p->align_msb)
    {
        /*
         * Enter if MSB alignment is required for the MSB bit on the bit-range (so the
         * 'key select' mechanism can make it the MSB in a 64-bits 'chunk')
         */
        if (align_info_p->align_msb_step <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "MSB Alignment is required but alignment resolution isn't positive (%d).\r\n",
                         align_info_p->align_msb_step);
        }
    }
    if (force_bit_selection)
    {
        if (key_id_p->id[0] < 0 || key_id_p->id[0] >= DBAL_NOF_ENUM_FIELD_KEY_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Requesting specific allocation, but no key provided.\r\n");
        }
        if (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Requesting specific allocation, but more than one key provided.\r\n");
        }
        if (key_id_p->id[0] < first_available_key || key_id_p->id[0] >= first_available_key + nof_available_keys)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Requesting key %d, but allowed keys are %d-%d.\r\n",
                         key_id_p->id[0], first_available_key, first_available_key + nof_available_keys - 1);
        }
        if (key_id_p->nof_bits[0] != key_length)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Requesting specific allocation, Mismatching number of bits (%d and %d).\r\n",
                         key_id_p->nof_bits[0], key_length);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_allocate_size_on_keys(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_key_length_type_e key_length_type,
    int *nof_keys_p,
    int *lsb_first_key_p,
    int *msb_first_key_p,
    int *lsb_second_key_p,
    int *msb_second_key_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_keys_p, _SHR_E_PARAM, "nof_keys_p");
    SHR_NULL_CHECK(lsb_first_key_p, _SHR_E_PARAM, "lsb_first_key_p");
    SHR_NULL_CHECK(msb_first_key_p, _SHR_E_PARAM, "msb_first_key_p");
    SHR_NULL_CHECK(lsb_second_key_p, _SHR_E_PARAM, "lsb_second_key_p");
    SHR_NULL_CHECK(msb_second_key_p, _SHR_E_PARAM, "msb_second_key_p");

    /** Initialize to illegal values. */
    (*nof_keys_p) = 0;
    (*lsb_first_key_p) = -1;
    (*msb_first_key_p) = -2;
    (*lsb_second_key_p) = -1;
    (*msb_second_key_p) = -2;

    switch (key_length_type)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
        {
            /** We present direct table TCAM field groups as TCAM field groups.*/
            if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM || fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
            {
                (*nof_keys_p) = 1;
                (*lsb_first_key_p) = 0;
                (*msb_first_key_p) = (*lsb_first_key_p) + dnx_data_field.tcam.key_size_half_get(unit) - 1;
            }
            else if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB)
            {
                (*nof_keys_p) = 1;
                (*lsb_first_key_p) = dnx_data_field.tcam.key_size_single_get(unit) -
                    dnx_data_field.map.key_size_get(unit);
                (*msb_first_key_p) = (*lsb_first_key_p) + dnx_data_field.map.key_size_get(unit) - 1;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown fg_type %d for direct table payload length.\r\n", fg_type);
            }
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
        {
            (*nof_keys_p) = 1;
            (*lsb_first_key_p) = 0;
            (*msb_first_key_p) = (*lsb_first_key_p) + dnx_data_field.tcam.key_size_half_get(unit) - 1;
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
        {
            (*nof_keys_p) = 1;
            (*lsb_first_key_p) = 0;
            (*msb_first_key_p) = (*lsb_first_key_p) + dnx_data_field.tcam.key_size_single_get(unit) - 1;
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
        {
            (*nof_keys_p) = 2;
            (*lsb_first_key_p) = 0;
            (*msb_first_key_p) = (*lsb_first_key_p) + dnx_data_field.tcam.key_size_single_get(unit) - 1;
            (*lsb_second_key_p) = 0;
            (*msb_second_key_p) = (*lsb_second_key_p) +
                (dnx_data_field.tcam.key_size_double_get(unit) - dnx_data_field.tcam.key_size_single_get(unit)) - 1;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown key_length_type %d\r\n", key_length_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_allocate(
    int unit,
    dnx_algo_field_key_flags_e flags,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 nof_available_keys_on_stage;
    uint8 first_available_key_on_stage;
    uint8 first_available_key;
    uint8 last_available_key;
    dbal_enum_value_field_field_key_e key_id_iter;
    uint8 alloc_successful = FALSE;
    uint8 is_key_allocated;
    uint8 is_any_bit_allocated;
    int key_length_bits_on_first_key;
    int key_length_bits_on_second_key;
    uint8 nof_keys_skip;
    dnx_field_key_id_t key_id_before;
    int nof_keys_to_allocate;
    int lsb_first_key;
    int msb_first_key;
    int lsb_second_key;
    int msb_second_key;
    uint32 key_lsb;
    dnx_field_group_type_e fg_type_effective;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Treat TCAM_DIRECT as TCAM.
     */
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        fg_type_effective = DNX_FIELD_GROUP_TYPE_TCAM;
    }
    else
    {
        fg_type_effective = fg_type;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_alloc_generic_verify
                          (unit, flags, fg_type_effective, field_stage, context_id, key_length_type, key_id_p));

    /** For sanity check later.*/
    sal_memcpy(&key_id_before, key_id_p, sizeof(key_id_before));

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Uninitialized stage (%d, %s): No free keys have been assigned\r\n",
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    /*
     * First, get the configuration structure (for stage and FG type).
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type_effective,
                                                     &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Uninitialized field group type (%d, %s) under assigned stage (%d, %s)\r\n",
                     (unsigned int) fg_type_effective, dnx_field_group_type_e_get_name(fg_type_effective),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Initialized field group type (%d, %s) under assigned stage (%d, %s) was assigned zero keys\r\n",
                     (unsigned int) fg_type_effective, dnx_field_group_type_e_get_name(fg_type_effective),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    /*
     * Set range for available keys on this (stage,fg_type) combination.
     */
    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    /*
     * Now search forward to get a free key.
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Search for key which is not occupied: Procedure %s, line %d %s%s:\r\n",
                 __func__, __LINE__, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "==> fg_type %d, field_stage %d context_id %d key_length_type %d\r\n",
                 fg_type_effective, field_stage, context_id, key_length_type);

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate_size_on_keys(unit, field_stage, fg_type_effective, key_length_type,
                                                                &nof_keys_to_allocate, &lsb_first_key, &msb_first_key,
                                                                &lsb_second_key, &msb_second_key));
    if (nof_keys_to_allocate == 1)
    {
        key_length_bits_on_first_key = msb_first_key - lsb_first_key + 1;
        key_length_bits_on_second_key = 0;
    }
    else if (nof_keys_to_allocate == 2)
    {
        key_length_bits_on_first_key = msb_first_key - lsb_first_key + 1;
        key_length_bits_on_second_key = msb_second_key - lsb_second_key + 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "stage %d fg type %d context_id %d key_length_type %d invalid number of keys %d.\r\n",
                     field_stage, fg_type_effective, context_id, key_length_type, nof_keys_to_allocate);
    }

    /*
     * For 'double key' the step is of the size of a double key. So, for instance,
     * it is possible to get keys A,B and C,D but not B,C
     */
    if (key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
    {
        nof_keys_skip = DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE;
    }
    else
    {
        nof_keys_skip = 1;
    }

    for (key_id_iter = first_available_key; key_id_iter <= last_available_key; key_id_iter += nof_keys_skip)
    {

        if (flags & DNX_ALGO_FIELD_KEY_FLAG_WITH_ID)
        {
            /*
             * In case of WITH_ID we would like to check if the current evaluated key is the same as requested key
             */
            if (key_id_iter < key_id_p->id[0])
            {
                continue;
            }
            if (key_id_iter > key_id_p->id[0])
            {
                break;
            }
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_is_allocated
                        (unit, field_stage, context_id, key_id_iter, &is_key_allocated));
        if (is_key_allocated)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_is_any_occupied
                        (unit, field_stage, context_id, key_id_iter, lsb_first_key, key_length_bits_on_first_key,
                         &is_any_bit_allocated));
        if (is_any_bit_allocated)
        {
            continue;
        }
        if (key_length_bits_on_second_key != 0)
        {
            if (key_id_iter + 1 > last_available_key)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_is_allocated
                            (unit, field_stage, context_id, key_id_iter + 1, &is_key_allocated));
            if (is_key_allocated)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_is_any_occupied
                            (unit, field_stage, context_id, key_id_iter + 1, lsb_second_key,
                             key_length_bits_on_second_key, &is_any_bit_allocated));
            if (is_any_bit_allocated)
            {
                continue;
            }
        }

        /*
         * Allocation found. Allocate the keys.
         */
        key_id_p->id[0] = key_id_iter;
        key_id_p->lsb[0] = lsb_first_key;
        key_id_p->nof_bits[0] = key_length_bits_on_first_key;
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_allocate
                        (unit, field_stage, context_id, key_id_p->id[0]));
        key_lsb = lsb_first_key;
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_allocate
                        (unit, field_stage, context_id, key_length_bits_on_first_key, TRUE, FALSE, key_id_p->id[0],
                         &key_lsb));
        if (key_length_bits_on_second_key != 0)
        {
            key_id_p->id[1] = key_id_iter + 1;
            key_id_p->lsb[1] = lsb_second_key;
            key_id_p->nof_bits[1] = key_length_bits_on_second_key;
            SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_allocate
                            (unit, field_stage, context_id, key_id_p->id[1]));
            key_lsb = lsb_second_key;
            SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_allocate
                            (unit, field_stage, context_id, key_length_bits_on_second_key, TRUE, FALSE, key_id_p->id[1],
                             &key_lsb));
        }
        alloc_successful = TRUE;
        break;
    }
    if (alloc_successful == FALSE)
    {
        if (flags & DNX_ALGO_FIELD_KEY_FLAG_WITH_ID)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Requested Key (%d,%d) could not be allocated for (fg_type,stage)=(%d,%d), "
                         "context_id %d\r\n",
                         key_id_p->id[0], key_id_p->id[1], (unsigned int) fg_type_effective, (unsigned int) field_stage,
                         context_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Could not find key for stage %d fg type %d context_id %d key_length_type %d.\r\n",
                         field_stage, fg_type_effective, context_id, key_length_type);
        }
    }

    /** Sanity checks*/
    {
        if ((unsigned int) (key_id_p->id[0]) >= (unsigned int) DBAL_NOF_ENUM_FIELD_KEY_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For field_stage=%d FG_type=%d context_id=%d, "
                         "key[0] was not allocated due to internal error\r\n",
                         field_stage, fg_type_effective, context_id);
        }
        if (key_length_type > DNX_FIELD_KEY_LENGTH_TYPE_SINGLE)
        {
            if ((unsigned int) (key_id_p->id[1]) >= DBAL_NOF_ENUM_FIELD_KEY_VALUES)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "For field_stage=%d FG_type=%d context_id=%d, "
                             "key[1] was not allocated due to internal error\r\n",
                             field_stage, fg_type_effective, context_id);
            }
        }
        if ((flags & DNX_ALGO_FIELD_KEY_FLAG_WITH_ID) &&
            sal_memcmp(&key_id_before.id, key_id_p->id, sizeof(key_id_before.id)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For field_stage=%d FG_type=%d context_id=%d, "
                         "key_id_p->id changed despite WITH_ID flag.\r\n", field_stage, fg_type_effective, context_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_bit_range_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    uint8 key_length,
    dnx_algo_field_key_bit_range_align_t * align_info_p,
    int force_bit_selection,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 nof_available_keys_on_stage;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    int alignment_placement_found = FALSE;
    /*
     * The bit to be allocated.
     */
    uint32 bit_lsb;
    int alloc_succeed = FALSE;
    dnx_field_key_id_t key_id_before;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_bit_range_alloc_generic_verify
                          (unit, fg_type, field_stage, context_id, key_length, align_info_p, force_bit_selection,
                           key_id_p));

    /** For sanity check later.*/
    sal_memcpy(&key_id_before, key_id_p, sizeof(key_id_before));

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Uninitialized stage (%d, %s): No free keys have been assigned\r\n",
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }

    
    /*
     * First, get the configuration structure (for stage and FG type),
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Uninitialized field group type (%d, %s) under assigned stage (%d, %s)\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Initialized field group type (%d, %s) under assigned stage (%d, %s) was assigned zero keys\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }

    if (force_bit_selection)
    {
        /*
         * Try to allocate the bits provided.
         */
        shr_error_e rv;
        uint8 requested_key;

        requested_key = key_id_p->id[0];

        bit_lsb = key_id_p->lsb[0];

        rv = dnx_algo_field_key_key_occupation_bmp_allocate
            (unit, field_stage, context_id, key_length, TRUE, TRUE, key_id_p->id[0], &bit_lsb);
        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot allocate bits %d-%d in key %d in stage (%d, %s), context %d.\r\n",
                         key_id_p->lsb[0], key_id_p->lsb[0] + key_length - 1, requested_key,
                         (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage), context_id);
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_allocate
                        (unit, field_stage, context_id, key_length, TRUE, FALSE, key_id_p->id[0], &bit_lsb));
        alloc_succeed = TRUE;

    }
    else
    {
        uint8 first_available_key;
        uint8 last_available_key;
        uint8 key_iter;
        /*
         * Set range for available keys on this (stage,fg_type) combination.
         */
        first_available_key = dnx_field_key_alloc_instruction.first_available_key;
        last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;

        /*
         * Now search from first to last to get a free place in a key.
         */
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Search for key which is not occupied: fg_type %d, field_stage %d context_id %d key_length %d\r\n",
                     fg_type, field_stage, context_id, key_length);

        for (key_iter = first_available_key; key_iter <= last_available_key; key_iter++)
        {
            shr_error_e rv;

            if (align_info_p->align_lsb || align_info_p->align_msb)
            {
                int lsb_alignment_step;
                int msb_alignment_step;
                int lsb_aligned_bit;
                uint32 found;

                /*
                 * Makes the step of the alignment not used 1, to make the alignment irrelevant.
                 */
                if (align_info_p->align_lsb)
                {
                    lsb_alignment_step = align_info_p->align_lsb_step;
                    lsb_aligned_bit = align_info_p->lsb_aligned_bit;
                }
                else
                {
                    lsb_alignment_step = 1;
                    lsb_aligned_bit = 0;
                }
                if (align_info_p->align_msb)
                {
                    msb_alignment_step = align_info_p->align_msb_step;
                }
                else
                {
                    msb_alignment_step = 1;
                }
                /*
                 * Allocate at specified bits and just report whether allocation is possible.
                 */
                if (lsb_aligned_bit < 0)
                {
                    /*
                     * Enter if 'aligned_bit' is negative (alignment required on bit preceding
                     * the first).
                     *
                     * Since ABS('aligned_bit') is smaller than one 'step', we simply set
                     * 'bit_lsb' to its absolute value.
                     */
                    bit_lsb = (-lsb_aligned_bit);
                }
                else
                {
                    /*
                     * Enter if 'aligned_bit' is positive or zero (alignment required on first
                     * bit or a bit following the first.
                     */
                    int16 bit_on_multiple;

                    if (lsb_aligned_bit >= key_length)
                    {
                        /*
                         * Caller is asking for alignment of a bit which is beyond the bit-range.
                         * This makes no sense since alignment is required so that caller would
                         * get the following 32 bits as per 'key select'.
                         */
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Caller is asking for %d bits but alignment is required on bit %d\r\n"
                                     "which is beyond this bit-range.\r\n",
                                     (unsigned int) key_length, (unsigned int) lsb_aligned_bit);
                    }
                    /*
                     * Find the first multiple of 16 which is larger than the required 'aligned_bit'.
                     * Then, we shall try to allocate starting from that multiple minus 'aligned_bit'
                     * (It it possible to use 'and' operation and simplify the code but we are trying to not
                     * assume the step is a power of 2).
                     */
                    found = FALSE;
                    for (bit_on_multiple = 0;
                         bit_on_multiple < DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT;
                         bit_on_multiple += lsb_alignment_step)
                    {
                        if (bit_on_multiple >= lsb_aligned_bit)
                        {
                            found = TRUE;
                            break;
                        }
                    }
                    if (found == FALSE)
                    {
                        /*
                         * Somehow, we failed to find aligned location which is larger than specified
                         * aligned bit. Quit with error.
                         */
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Failed to find aligned bit larger than %d while searching key up to bit %d.\r\n",
                                     (unsigned int) lsb_aligned_bit,
                                     (unsigned int) (DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT - 1));
                    }
                    /*
                     * Now 'bit_on_multiple' is the closest aligned bit which is larger than 'aligned_bit'.
                     * Set 'bit_lsb' to start search with 'bit_on_multiple' as the first candidate.
                     */
                    bit_lsb = (bit_on_multiple - lsb_aligned_bit);
                }
                {
                    found = FALSE;
                    /*
                     * Initial 'bit_lsb' is set in the clause above.
                     */
                    for (; bit_lsb < DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT; bit_lsb += lsb_alignment_step)
                    {
                        /*
                         * Check if the msb alignment requirement is achieved.
                         */
                        if (align_info_p->align_msb && ((bit_lsb + key_length) % msb_alignment_step != 0))
                        {
                            continue;
                        }
                        alignment_placement_found = TRUE;
                        rv = dnx_algo_field_key_key_occupation_bmp_allocate
                            (unit, field_stage, context_id, key_length, TRUE, TRUE, key_iter, &bit_lsb);
                        if (rv != _SHR_E_NONE)
                        {
                            /*
                             * On failing to allocate required bit-range, skip to the next 'step'.
                             */
                            continue;
                        }
                        else
                        {
                            found = TRUE;
                            break;
                        }
                    }
                    if (!found)
                    {
                        /*
                         * On failing to allocate required bit-range at required alignment,
                         * skip to the next key.
                         */
                        continue;
                    }
                    /*
                     * Allocate at specified bits.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_allocate
                                    (unit, field_stage, context_id, key_length, TRUE, FALSE, key_iter, &bit_lsb));
                }
            }
            else
            {
                /*
                 * Reach here if no alignment is required.
                 */
                rv = dnx_algo_field_key_key_occupation_bmp_allocate
                    (unit, field_stage, context_id, key_length, FALSE, TRUE, key_iter, &bit_lsb);
                if (rv != _SHR_E_NONE)
                {
                    /*
                     * On failing to allocate required bit-range, skip to the next key.
                     */
                    continue;
                }
                /*
                 * At this point, allocation was successful at required alignment. 'bit_lsb'
                 * contains the location of the LS bit on bit-range.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_allocate
                                (unit, field_stage, context_id, key_length, FALSE, FALSE, key_iter, &bit_lsb));
            }
            /*
             * Succeeded to allocate.
             */
            alloc_succeed = TRUE;
            key_id_p->id[0] = key_iter;
            break;
        }
        if ((align_info_p->align_lsb || align_info_p->align_msb) && alignment_placement_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Failed to find a placement that fulfills alignment options, occupied or not.\r\n");
        }
    }
    if (alloc_succeed)
    {
        key_id_p->lsb[0] = bit_lsb;
        key_id_p->nof_bits[0] = key_length;
        key_id_p->id[1] = DNX_FIELD_KEY_ID_INVALID;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot allocate %d bits in stage (%d, %s), context %d.\r\n",
                     key_length, (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage), context_id);
    }
    /** Sanity check. */
    if (force_bit_selection && sal_memcmp(&key_id_before, key_id_p, sizeof(key_id_before)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "For field_stage=%d FG_type=%d context_id=%d, key_id_p changed despiteforce_bit_selection.\r\n",
                     field_stage, fg_type, context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify procedure for dnx_algo_field_key_free_generic().
 * See its header for description of parameters.
 * \remark
 *  * None
 */
static shr_error_e
dnx_algo_field_key_free_generic_verify(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    uint8 nof_available_keys_on_stage;
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 first_available_key;
    uint8 last_available_key;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    DNX_FIELD_KEY_ID_VERIFY(key_id_p->id[0]);
    if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2) &&
        (field_stage != DNX_FIELD_STAGE_IPMF3) && (field_stage != DNX_FIELD_STAGE_EPMF))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                     dnx_field_stage_e_get_name(field_stage));
    }
    if ((key_id_p->id[1] >= DBAL_NOF_ENUM_FIELD_KEY_VALUES) && (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key[1] identified (%d, larger than max: %d)\r\n", key_id_p->id[1],
                     DBAL_NOF_ENUM_FIELD_KEY_VALUES - 1);
    }
    if ((key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID) && (key_id_p->id[1] != (key_id_p->id[0] + 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Second key (%d) isn't one above first key (%d).\r\n",
                     key_id_p->id[1], key_id_p->id[0]);
    }
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input stage was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Uninitialized stage (%d): No free keys have been assigned\r\n",
                     (unsigned int) field_stage);
    }
    /*
     * Get the configuration structure.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Uninitialized field group type (%d) under assigned stage (%d)\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Initialized field group (%d) under assigned stage (%d) was assigned zero keys\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }

    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    if (key_id_p->id[0] < (dbal_enum_value_field_field_key_e) first_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is smaller than allowed (%d). Illegal. Quit\r\n",
                     key_id_p->id[0], (unsigned int) first_available_key);
    }
    if (key_id_p->id[0] > (dbal_enum_value_field_field_key_e) last_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is larger than allowed (%d). Illegal. Quit\r\n",
                     key_id_p->id[0], (unsigned int) last_available_key);
    }
    if (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
    {
        if (key_id_p->id[1] < (dbal_enum_value_field_field_key_e) first_available_key)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Input second key_id (%d) is smaller than allowed (%d). Illegal. Quit\r\n",
                         key_id_p->id[1], (unsigned int) first_available_key);
        }
        if (key_id_p->id[1] > (dbal_enum_value_field_field_key_e) last_available_key)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Input second key_id (%d) is larger than allowed (%d). Illegal. Quit\r\n",
                         key_id_p->id[1], (unsigned int) last_available_key);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Verify procedure for dnx_algo_field_key_bit_range_free_generic().
 * See its header for description of parameters.
 * \remark
 *  * None
 */
static shr_error_e
dnx_algo_field_key_bit_range_free_generic_verify(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    uint8 nof_available_keys_on_stage;
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 first_available_key;
    uint8 last_available_key;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    DNX_FIELD_KEY_ID_VERIFY(key_id_p->id[0]);
    if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2) &&
        (field_stage != DNX_FIELD_STAGE_IPMF3) && (field_stage != DNX_FIELD_STAGE_EPMF))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                     dnx_field_stage_e_get_name(field_stage));
    }
    if (key_id_p->lsb[0] >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Illegal offset within key (%d, larger than max: %d)\r\n",
                     key_id_p->lsb[0], DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE - 1);
    }
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input stage was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Uninitialized stage (%d): No free keys have been assigned\r\n", (unsigned int) field_stage);
    }
    /*
     * Get the configuration structure.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Uninitialized field group type (%d) under assigned stage (%d)\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Initialized field group (%d) under assigned stage (%d) was assigned zero keys\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    if (key_id_p->id[0] < (dbal_enum_value_field_field_key_e) first_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is smaller than allowed (%d). Illegal. Quit\r\n",
                     key_id_p->id[0], (unsigned int) first_available_key);
    }
    if (key_id_p->id[0] > (dbal_enum_value_field_field_key_e) last_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is larger than allowed (%d). Illegal. Quit\r\n",
                     key_id_p->id[0], (unsigned int) first_available_key);
    }

    if (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "More than one key provided.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_free(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    int nof_keys_to_deallocate;
    int lsb_first_key;
    int msb_first_key;
    int lsb_second_key;
    int msb_second_key;
    dnx_field_group_type_e fg_type_effective;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_free_generic_verify(unit, fg_type, field_stage, context_id, key_id_p));

    /*
     * Treat TCAM_DIRECT as TCAM.
     */
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        fg_type_effective = DNX_FIELD_GROUP_TYPE_TCAM;
    }
    else
    {
        fg_type_effective = fg_type;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Deallocate: field_stage %d context_id %d key_id %d%s\r\n",
                 field_stage, context_id, key_id_p->id[0], EMPTY);

    /*
     * Get the bits on the keys.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate_size_on_keys(unit, field_stage, fg_type_effective, key_length_type,
                                                                &nof_keys_to_deallocate, &lsb_first_key, &msb_first_key,
                                                                &lsb_second_key, &msb_second_key));
    /** Sanity checks*/
    if ((nof_keys_to_deallocate < 1) || (nof_keys_to_deallocate > 2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage %d fg_type %d, key_length_type %d, has %d keys.\r\n",
                     field_stage, fg_type_effective, key_length_type, nof_keys_to_deallocate);
    }
    if ((nof_keys_to_deallocate == 1) && (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage %d fg_type %d, key_length_type %d, has %d keys, "
                     "but given key_id has 2.\r\n", field_stage, fg_type_effective, key_length_type,
                     nof_keys_to_deallocate);
    }
    if ((nof_keys_to_deallocate == 2) && (key_id_p->id[1] == DNX_FIELD_KEY_ID_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage %d fg_type %d, key_length_type %d, has %d keys, "
                     "but given key_id has 1.\r\n", field_stage, fg_type_effective, key_length_type,
                     nof_keys_to_deallocate);
    }

    /*
     * Deallocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_deallocate
                    (unit, field_stage, context_id, msb_first_key - lsb_first_key + 1, key_id_p->id[0], lsb_first_key));
    SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_deallocate
                    (unit, field_stage, context_id, key_id_p->id[0]));
    if (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_deallocate
                        (unit, field_stage, context_id, msb_second_key - lsb_second_key + 1, key_id_p->id[1],
                         lsb_second_key));
        SHR_IF_ERR_EXIT(dnx_algo_field_key_key_lookup_occupation_bmp_deallocate
                        (unit, field_stage, context_id, key_id_p->id[1]));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_bit_range_free(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_bit_range_free_generic_verify
                          (unit, fg_type, field_stage, context_id, key_id_p));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free bit-range on key: field_stage %s context_id %d key_id %d bit_range_offset_within_key %d\r\n",
                 dnx_field_stage_e_get_name(field_stage), context_id, key_id_p->id[0], key_id_p->lsb[0]);

    SHR_IF_ERR_EXIT(dnx_algo_field_key_key_occupation_bmp_deallocate
                    (unit, field_stage, context_id, key_id_p->nof_bits[0], key_id_p->id[0], key_id_p->lsb[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_field.h
 */
shr_error_e
dnx_algo_field_key_alloc_sw_state_init(
    int unit)
{
    /*
     * Array carrying the stages which are initialized by this procedure.
     * Used for looping on all supported stages.
     */
    dnx_field_stage_e supported_stages[DNX_FIELD_STAGE_NOF];
    /*
     * Number of meaningful entries in supported_stages[]
     */
    uint32 num_supported_stages;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_keys_per_stage_allocation_sw.init(unit));
    {
        /*
         * Initialize key allocation setup.
         */
        dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
        dnx_field_key_alloc_stage_instruction_info_t dnx_field_key_alloc_stage_instruction_info;
        uint32 nof_stages, nof_group_types, stage_index, group_type_index;
        /*
         * Deduce the number of elements of the configuration array, keys_availability_info
         * (in sw state), directly from its definition, as created by sw state.
         */
        nof_group_types =
            sizeof(dnx_field_key_alloc_stage_instruction_info.
                   keys_availability_stage_info[0].keys_availability_group_info) /
            sizeof(dnx_field_key_alloc_stage_instruction_info.
                   keys_availability_stage_info[0].keys_availability_group_info[0]);
        nof_stages =
            sizeof(dnx_field_key_alloc_stage_instruction_info.keys_availability_stage_info) /
            sizeof(dnx_field_key_alloc_stage_instruction_info.keys_availability_stage_info[0]);
        /*
         * This is not required but it is more elegant.
         */
        sal_memset(&dnx_field_key_alloc_instruction, 0, sizeof(dnx_field_key_alloc_instruction));
        /*
         * Create an entry which is not valid.
         */
        dnx_field_key_alloc_instruction.structure_is_valid = FALSE;
        /*
         * Make sure all configuration entries are marked as 'invalid'
         * We assume here that stages start from '0' and go up sequentially.
         */
        for (stage_index = 0; stage_index < nof_stages; stage_index++)
        {
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.range_fill(unit, stage_index, 0,
                                                                    nof_group_types, dnx_field_key_alloc_instruction));
            /*
             * Initialize number of keys on all stages to '0'. Fill the operational
             * values right below.
             */
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.set(unit, stage_index, 0));
        }
        /*
         * Now fill in valid configurations.
         */
        num_supported_stages = 0;
        {
            stage_index = DNX_FIELD_STAGE_IPMF1;
            /**********************************
             * Set DNX_FIELD_STAGE_IPMF1:
 */
            supported_stages[num_supported_stages++] = stage_index;
            /** Total number of keys on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.set(unit, stage_index,
                                                                                         dnx_data_field.stage.
                                                                                         stage_info_get(unit,
                                                                                                        stage_index)->
                                                                                         nof_keys_alloc));
            /** First key on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            /** Total number of keys for TCAM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_tcam));
            /** First key for TCAM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * By hardware options, EXEM key may be any key from A to E. For this system, code specifies one of
             * these keys to be used for EXEM. This key is indicated in DNX DATA for iPMF1 by 'large_ipmf1_key'
             */
            /** Total number of keys for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_exem));
            /** First key for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_A +
                                                                                 dnx_data_field.exem.
                                                                                 large_ipmf1_key_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));

            /*
             * MDB_DT
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
            /** Total number of keys for MDB_DT.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_mdb_dt));
            /** First key for MDB_DT.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_A +
                                                                                 dnx_data_field.map.
                                                                                 ipmf1_key_select_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
        }
        {
            stage_index = DNX_FIELD_STAGE_IPMF2;
            /**********************************
             * Set DNX_FIELD_STAGE_IPMF2:
 */
            supported_stages[num_supported_stages++] = stage_index;
            /** Total number of key on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.set(unit, stage_index,
                                                                                         dnx_data_field.stage.
                                                                                         stage_info_get(unit,
                                                                                                        stage_index)->
                                                                                         nof_keys_alloc));
            /** First key on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_F));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            /** Total number of keys for TCAM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_tcam));
            /** First key for TCAM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_F));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * By hardware options, EXEM key may be any key from F to J. For this system, code specifies one of
             * these keys to be used for EXEM. This key for iPMF2 is indicated in DNX DATA 'small_ipmf2_key'
             */
            /** Total number of keys for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_exem));
            /** First key for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_F +
                                                                                 dnx_data_field.exem.
                                                                                 small_ipmf2_key_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * DIRECT_EXTRACTION
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_dir_ext));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_I));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * MDB_DT
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
            /** Total number of keys for MDB_DT.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_mdb_dt));
            /** First key for MDB_DT.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_F +
                                                                                 dnx_data_field.map.
                                                                                 ipmf2_key_select_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));

            /*
             * STATE_TABLE
             */
            if (dnx_data_field.state_table.state_table_stage_key_get(unit) == DNX_FIELD_STATE_TABLE_SOURCE_IPMF2_KEY_J)
            {
                group_type_index = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                nof_available_keys.set(unit, stage_index, group_type_index, 1));
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                first_available_key.set(unit, stage_index, group_type_index,
                                                        DBAL_ENUM_FVAL_FIELD_KEY_J));
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            }
        }
        {
            stage_index = DNX_FIELD_STAGE_IPMF3;
            /**********************************
             * Set DNX_FIELD_STAGE_IPMF3:
 */
            supported_stages[num_supported_stages++] = stage_index;
            /** Total number of keys on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.set(unit, stage_index,
                                                                                         dnx_data_field.stage.
                                                                                         stage_info_get(unit,
                                                                                                        stage_index)->
                                                                                         nof_keys_alloc));
            /** First key on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            /** Total number of keys for TCAM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_tcam));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * The EXEM key for IPMF3 cannot be changed, unlike in IPMF1/2.
             */
            /** Total number of keys for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_exem));
            /** First key for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_C));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * DIRECT_EXTRACTION
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_dir_ext));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * MDB_DT
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
            /** Total number of keys for MDB_DT.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_mdb_dt));
            /** First key for MDB_DT.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_A +
                                                                                 dnx_data_field.map.
                                                                                 ipmf3_key_select_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));

        }
        {
            stage_index = DNX_FIELD_STAGE_EPMF;
            /**********************************
             * Set DNX_FIELD_STAGE_EPMF:
 */
            supported_stages[num_supported_stages++] = stage_index;
            /** Total number of keys on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.set(unit, stage_index,
                                                                                         dnx_data_field.stage.
                                                                                         stage_info_get(unit,
                                                                                                        stage_index)->
                                                                                         nof_keys_alloc));
            /** First key on stage.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            /** Total number of keys for TCAM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_tcam));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_B));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * The EXEM key for EPMF cannot be changed, unlike in IPMF1/2.
             */
            /** Total number of keys for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                dnx_data_field.stage.
                                                                                stage_info_get(unit,
                                                                                               stage_index)->
                                                                                nof_keys_alloc_for_exem));
            /** First key for EXEM.*/
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));

            if (dnx_data_field.features.dir_ext_epmf_get(unit))
            {
                /*
                 * DIRECT_EXTRACTION
                 */
                group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                    dnx_data_field.stage.
                                                                                    stage_info_get(unit,
                                                                                                   stage_index)->
                                                                                    nof_keys_alloc_for_dir_ext));
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                first_available_key.set(unit, stage_index, group_type_index,
                                                        DBAL_ENUM_FVAL_FIELD_KEY_D));
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            }
        }
    }
    {
        dnx_field_stage_e stage_index;
        dnx_field_context_t pmf_program_index;
        uint8 nof_keys;
        dnx_field_stage_e field_stage;

        /*
         * Assign number of half-keys per PMF program per stage.
         * Assign all half-keys and 'free'.
         */
        for (stage_index = 0; stage_index < num_supported_stages; stage_index++)
        {
            field_stage = supported_stages[stage_index];
            for (pmf_program_index = 0; pmf_program_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS;
                 pmf_program_index++)
            {
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                                nof_available_keys_on_stage.get(unit, field_stage, &nof_keys));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf_1_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint8 ranges,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.allocate_single(unit, context_id, alloc_flags, &ranges, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf_2_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf_3_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}
/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_epmf_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}
/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ifwd2_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ifwd2_ffc_is_allocated(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.is_allocated(unit, context_id, ffc_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ifwd2_ffc_num_of_elements_get(
    int unit,
    dnx_field_context_t context_id,
    int *nof_elements_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_elements_p, _SHR_E_PARAM, "nof_elements_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.nof_free_elements_get(unit, context_id, nof_elements_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int force_position,
    dbal_enum_value_field_field_key_e * key_id_p,
    int *bit_lsb_p)
{
    int sub_resource_id;
    int nof_keys = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    int nof_initial_keys = nof_keys - nof_keys_alloc;
    int first_initial_key = nof_keys_alloc;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(bit_lsb_p, _SHR_E_PARAM, "bit_lsb_p");

    if (force_position)
    {
        /*
         * Try to allocate in psecific location.
         */
        DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, (*key_id_p), nof_initial_keys, first_initial_key,
                                                    sub_resource_id);
        SHR_IF_ERR_EXIT(algo_field_info_sw.
                        ipmf1_initial_key_occ_bmp.allocate_several(unit, sub_resource_id, DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                   size, NULL, bit_lsb_p));
    }
    else
    {
        shr_error_e rv;
        int alloc_offset = 0;
        int key_id;
        int alloc_successful = FALSE;
        /*
         * Try to allocate in each key one by one.
         */
        for (key_id = first_initial_key; key_id < first_initial_key + nof_initial_keys; key_id++)
        {
            /*
             * Try to allocate in key.
             */
            DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_initial_keys, first_initial_key,
                                                        sub_resource_id);
            rv = algo_field_info_sw.ipmf1_initial_key_occ_bmp.allocate_several(unit, sub_resource_id,
                                                                               DNX_ALGO_RES_ALLOCATE_SIMULATION, size,
                                                                               NULL, &alloc_offset);
            /*
             * If successful - update the parameters
             */
            if (rv == _SHR_E_NONE)
            {
                /*
                 * Succeeded to test for allocation in Key - now allocate it.
                 */
                SHR_IF_ERR_EXIT(algo_field_info_sw.
                                ipmf1_initial_key_occ_bmp.allocate_several(unit, sub_resource_id, 0, size, NULL,
                                                                           &alloc_offset));
                (*key_id_p) = key_id;
                (*bit_lsb_p) = alloc_offset;
                alloc_successful = TRUE;
                break;
            }
            else
            {
                /*
                 * If we failed to allocate, move to the next key.
                 * Print a debug log of the reason for the failure.
                 * Note we do not verify that the failure is _SHR_E_RESOURCE, as we are not confident that is what the
                 * allocation manager will return.
                 */
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Failed to allocate initial key size 0x%X field_stage ipmf1 context %d "
                             "key %d error %d. Moving on to next key.\n", size, context_id, key_id, rv);
            }
        }

        if (alloc_successful == FALSE)
        {
            /*
             * Failed to allocate initial key.
             */
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "Failed to allocate initial key for size 0x%X field_stage ipmf1 context %d.\n",
                         size, context_id);
        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Allocated initial key for size 0x%X field_stage ipmf1 dest_offset 0x%X key_id %d. Forced %d\n",
                 size, (*bit_lsb_p), (*key_id_p), force_position);

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    dbal_enum_value_field_field_key_e key_id,
    int initial_offset)
{
    int sub_resource_id;
    int nof_keys = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    int nof_initial_keys = nof_keys - nof_keys_alloc;
    int first_initial_key = nof_keys_alloc;
    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free bmp in initial key: context_id (%d) key_id %d size %d offset in key %d\r\n",
                 context_id, key_id, size, initial_offset);

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_initial_keys, first_initial_key,
                                                sub_resource_id);
    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    ipmf1_initial_key_occ_bmp.free_several(unit, sub_resource_id, size, initial_offset));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_key_occupation_bmp_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 size,
    int force_position_within_key,
    int just_check_possibility,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *bit_lsb_p)
{
    int sub_resource_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key;
    uint32 flags = 0;
    shr_error_e rv;
    int alloc_offset = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bit_lsb_p, _SHR_E_PARAM, "bit_lsb_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        first_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    }
    else
    {
        first_key = 0;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Attempting to allocate key size 0x%X field_stage %d context %d key %d.\n",
                 size, field_stage, context_id, key_id);

    if (force_position_within_key)
    {
        /*
         * Try to allocate in specific location within the key.
         */
        flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
        alloc_offset = (*bit_lsb_p);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Forcing position %d.%s%s%s\n", alloc_offset, EMPTY, EMPTY, EMPTY);
    }

    if (just_check_possibility)
    {
        /*
         * Do not allocate.
         */
        flags |= DNX_ALGO_RES_ALLOCATE_SIMULATION;
    }

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            rv = algo_field_info_sw.ipmf1_key_occ_bmp.allocate_several(unit, sub_resource_id, flags, size, NULL,
                                                                       &alloc_offset);
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            rv = algo_field_info_sw.ipmf2_key_occ_bmp.allocate_several(unit, sub_resource_id, flags, size, NULL,
                                                                       &alloc_offset);
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            rv = algo_field_info_sw.ipmf3_key_occ_bmp.allocate_several(unit, sub_resource_id, flags, size, NULL,
                                                                       &alloc_offset);
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            rv = algo_field_info_sw.epmf_key_occ_bmp.allocate_several(unit, sub_resource_id, flags, size, NULL,
                                                                      &alloc_offset);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field stage %d.\n", field_stage);
        }
    }
    SHR_SET_CURRENT_ERR(rv);
    if (rv == _SHR_E_NONE)
    {

        if (just_check_possibility)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Allocated possible for size 0x%X field_stage %d dest_offset 0x%X key_id %d.\n",
                         size, field_stage, alloc_offset, key_id);
        }
        else
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Allocated done for size 0x%X field_stage %d dest_offset 0x%X key_id %d.\n",
                         size, field_stage, alloc_offset, key_id);
        }
        (*bit_lsb_p) = alloc_offset;
    }
    else
    {
        if (just_check_possibility)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Allocation not possible size 0x%X field_stage %d context %d key %d.\n",
                         size, field_stage, context_id, key_id);
        }
        else
        {
            SHR_ERR_EXIT(rv,
                         "Allocation failed size 0x%X field_stage %d context %d key %d.\n",
                         size, field_stage, context_id, key_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_key_occupation_bmp_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 size,
    dbal_enum_value_field_field_key_e key_id,
    uint32 bit_lsb)
{
    int sub_resource_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key;
    SHR_FUNC_INIT_VARS(unit);

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        first_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    }
    else
    {
        first_key = 0;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free in key bmp: context_id (%d) key_id %d size %d offset in key %d\r\n",
                 context_id, key_id, size, bit_lsb);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Stage %d.%s%s%s\r\n", field_stage, EMPTY, EMPTY, EMPTY);

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_key_occ_bmp.free_several(unit, sub_resource_id, size, bit_lsb));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_key_occ_bmp.free_several(unit, sub_resource_id, size, bit_lsb));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_key_occ_bmp.free_several(unit, sub_resource_id, size, bit_lsb));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_key_occ_bmp.free_several(unit, sub_resource_id, size, bit_lsb));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field stage %d.\n", field_stage);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_key_occupation_bmp_is_any_occupied(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    int32 bit_lsb,
    uint32 size,
    uint8 *is_any_bit_occupied_p)
{
    int sub_resource_id;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key;
    int nof_allocated_bits;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_any_bit_occupied_p, _SHR_E_PARAM, "is_any_bit_occupied_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        first_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    }
    else
    {
        first_key = 0;
    }

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, sub_resource_id);
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.
                            ipmf1_key_occ_bmp.nof_allocated_elements_in_range_get(unit, sub_resource_id, bit_lsb, size,
                                                                                  &nof_allocated_bits));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.
                            ipmf2_key_occ_bmp.nof_allocated_elements_in_range_get(unit, sub_resource_id, bit_lsb, size,
                                                                                  &nof_allocated_bits));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.
                            ipmf3_key_occ_bmp.nof_allocated_elements_in_range_get(unit, sub_resource_id, bit_lsb, size,
                                                                                  &nof_allocated_bits));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.
                            epmf_key_occ_bmp.nof_allocated_elements_in_range_get(unit, sub_resource_id, bit_lsb, size,
                                                                                 &nof_allocated_bits));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field stage %d.\n", field_stage);
        }
    }
    if (nof_allocated_bits == 0)
    {
        (*is_any_bit_occupied_p) = FALSE;
    }
    else
    {
        (*is_any_bit_occupied_p) = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_key_lookup_occupation_bmp_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id)
{
    int context_key_combo;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key;
    SHR_FUNC_INIT_VARS(unit);

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        first_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    }
    else
    {
        first_key = 0;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Attempting to allocate key lookup field_stage %d context %d key %d.%s\n",
                 field_stage, context_id, key_id, EMPTY);

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, context_key_combo);
    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    key_lookup_occ_bmp.allocate_single(unit, field_stage, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL,
                                                       &context_key_combo));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_key_lookup_occupation_bmp_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id)
{
    int context_key_combo;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key;
    SHR_FUNC_INIT_VARS(unit);

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        first_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    }
    else
    {
        first_key = 0;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free key lookup: stage %d context_id %d key_id %d.%s\r\n", field_stage, context_id, key_id, EMPTY);

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, context_key_combo);
    SHR_IF_ERR_EXIT(algo_field_info_sw.key_lookup_occ_bmp.free_single(unit, field_stage, context_key_combo));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_key_lookup_occupation_bmp_is_allocated(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint8 *is_allocated_p)
{
    int context_key_combo;
    int nof_keys_alloc = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
    int first_key;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        first_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    }
    else
    {
        first_key = 0;
    }

    DNX_ALGO_FIELD_KEY_ALLOC_SUB_RESOURCE_INDEX(context_id, key_id, nof_keys_alloc, first_key, context_key_combo);
    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    key_lookup_occ_bmp.is_allocated(unit, field_stage, context_key_combo, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_user_qual_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_qual_id_t * user_qual_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_qual_id_p, _SHR_E_PARAM, "user_qual_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_qual_id.allocate_single(unit, alloc_flags, NULL, (int *) user_qual_id_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_user_action_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_action_id_t * user_action_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_action_id_p, _SHR_E_PARAM, "user_action_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    user_action_id.allocate_single(unit, alloc_flags, NULL, (int *) user_action_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf_1_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf_2_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}
/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf_3_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_epmf_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ifwd2_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}
/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_user_qual_id_deallocate(
    int unit,
    dnx_field_qual_id_t user_qual_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_qual_id.free_single(unit, user_qual_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_user_action_id_deallocate(
    int unit,
    dnx_field_action_id_t user_action_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_action_id.free_single(unit, user_action_id));

exit:
    SHR_FUNC_EXIT;
}
