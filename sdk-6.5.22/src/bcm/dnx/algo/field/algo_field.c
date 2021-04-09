/** \file algo_field.c
 * 
 *
 * Field procedures for DNX.
 *
 * Will hold the needed algorithm functions for Field module
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_action_access.h>

/*
 * }
 */

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_group_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_group_t * fg_id_p)
{
    int alloc_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");

    /**In case WITH_ID flag is set*/
    alloc_id = *fg_id_p;
    SHR_IF_ERR_EXIT(algo_field_info_sw.field_group_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));
    /**In case WITH_ID flag was not set*/
    *fg_id_p = alloc_id;
exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_group_id_deallocate(
    int unit,
    dnx_field_group_t fg_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.field_group_id.free_single(unit, fg_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_sexem_app_db_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_app_db_id_t * app_db_id_p)
{
    int alloc_id;
    SHR_FUNC_INIT_VARS(unit);

    /**In case WITH_ID flag is set*/
    alloc_id = *app_db_id_p;

    SHR_IF_ERR_EXIT(algo_field_info_sw.sexem_app_db_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));
    /**In case WITH_ID flag was not set*/
    *app_db_id_p = alloc_id;
exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_sexem_app_db_id_deallocate(
    int unit,
    dnx_field_app_db_id_t app_db_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.sexem_app_db_id.free_single(unit, app_db_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_lexem_app_db_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_app_db_id_t * app_db_id_p)
{
    int alloc_id;
    SHR_FUNC_INIT_VARS(unit);

    /**In case WITH_ID flag is set*/
    alloc_id = *app_db_id_p;
    SHR_IF_ERR_EXIT(algo_field_info_sw.lexem_app_db_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));
    /**In case WITH_ID flag was not set*/
    *app_db_id_p = alloc_id;
exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_lexem_app_db_id_deallocate(
    int unit,
    dnx_field_app_db_id_t app_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.lexem_app_db_id.free_single(unit, app_db_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_ace_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_ace_id_t * ace_id_p)
{
    int alloc_id;
    int found_alloc;
    uint8 is_alloc;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If we allocate WITH_ID, allocate directly.
     */
    if (alloc_flags & DNX_ALGO_RES_ALLOCATE_WITH_ID)
    {
        alloc_id = *ace_id_p;
        SHR_IF_ERR_EXIT(algo_field_info_sw.ace_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));
    }
    else
    {
        /*
         * If we allocate without ID, only allocate within the range of the ACL entension.
         */
        found_alloc = FALSE;
        for (*ace_id_p = dnx_data_field.ace.ace_id_pmf_alloc_first_get(unit);
             *ace_id_p <= dnx_data_field.ace.ace_id_pmf_alloc_last_get(unit); (*ace_id_p)++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, *ace_id_p, &is_alloc));
            if (is_alloc == FALSE)
            {
                alloc_id = *ace_id_p;
                SHR_IF_ERR_EXIT(algo_field_info_sw.
                                ace_id.allocate_single(unit, (alloc_flags | DNX_ALGO_RES_ALLOCATE_WITH_ID), NULL,
                                                       &alloc_id));
                found_alloc = TRUE;
                break;
            }
        }
        if (found_alloc == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "No free ACE format ID of type ACE found.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_ace_id_deallocate(
    int unit,
    dnx_field_ace_id_t ace_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_id.free_single(unit, ace_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_ace_key_allocate(
    int unit,
    int alloc_flags,
    dnx_field_ace_key_t * ace_key_p)
{
    int alloc_id;
    SHR_FUNC_INIT_VARS(unit);

    /**In case WITH_ID flag is set*/
    alloc_id = *ace_key_p;
    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_key.allocate_single(unit, alloc_flags, NULL, &alloc_id));
    /**In case WITH_ID flag was not set*/
    *ace_key_p = alloc_id;

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_ace_key_deallocate(
    int unit,
    dnx_field_ace_key_t ace_key)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_key.free_single(unit, ace_key, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_entry_tcam_access_id_allocate(
    int unit,
    int alloc_flags,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.entry_tcam_access_id.allocate_single(unit, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_entry_tcam_access_id_is_allocated(
    int unit,
    int entry_access_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.entry_tcam_access_id.is_allocated(unit, entry_access_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_entry_tcam_access_id_deallocate(
    int unit,
    int entry_access_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.entry_tcam_access_id.free_single(unit, entry_access_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_kbp_opcode_cs_profile_id_allocate(
    int unit,
    int alloc_flags,
    int *cs_prodfile_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_prodfile_id_p, _SHR_E_PARAM, "cs_prodfile_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    kbp_opcode_cs_profile_id.allocate_single(unit, alloc_flags, NULL, cs_prodfile_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_kbp_opcode_cs_profile_id_deallocate(
    int unit,
    int cs_prodfile_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.kbp_opcode_cs_profile_id.free_single(unit, cs_prodfile_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Procedures related to 'FEM machine'
 * {
 */

/**
* \brief
*  Init simple alloc algorithm for FEM_PGM_ID (FEM program) For PMFA
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * We allocate all fem programs available in the system (16 * 4).
*     They are indexed by 'fem_id'.
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_pmf_a_fem_pgm_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = FIRST_AVAILABLE_FEM_PROGRAM;
    data.nof_elements =
        dnx_data_field.stage.stage_info_get(unit,
                                            DNX_FIELD_STAGE_IPMF1)->nof_fem_programs - FIRST_AVAILABLE_FEM_PROGRAM;
    /*
     * Need to allocate 'FEM program' per 'FEM id' hence creating indexed pool
     * There are 16 FEMs
     * The first 16 are also used since it is possible to 'disconnect' an entry of
     * the 'context' table by setting an illegal 'fem key select'. (I.e., there is
     * no need to reserve 'default' 'fem program').
     */
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_PMF_A_FEM_PGM_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_pgm_id.alloc(unit,
                                                               dnx_data_field.stage.stage_info_get(unit,
                                                                                                   DNX_FIELD_STAGE_IPMF1)->nof_fem_id));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_pgm_id.create(unit, sub_resource_id, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple alloc algorithm for FEM_PGM_ID (FEM program) For PMFA
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_res_mngr_pmf_a_fem_pgm_id_init()
*/
static shr_error_e
dnx_algo_field_res_mngr_pmf_a_fem_pgm_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state should be destroyed together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FEM_PGM_ID (FEM program) For PMFA
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * We allocate all fem extractions/actions available in the system (16 * 4).
*     They are indexed by 'fem_id'.
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_pmf_a_fem_map_index_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    /*
     * Need to allocate 'FEM map index' per 'FEM id' hence creating indexed pool
     * There are 16 FEMs
     */
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_PMF_A_FEM_MAP_INDEX, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_map_index.alloc(unit,
                                                                  dnx_data_field.stage.stage_info_get(unit,
                                                                                                      DNX_FIELD_STAGE_IPMF1)->nof_fem_id));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_map_index.create(unit, sub_resource_id, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple alloc algorithm for FEM_PGM_ID (FEM program) For PMFA
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_res_mngr_pmf_a_fem_map_index_init()
*/
static shr_error_e
dnx_algo_field_res_mngr_pmf_a_fem_map_index_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state should be destroyed together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/**
* \brief
*  Init simple alloc algorithm for pmf user qualifier ID
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_user_qual_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.qual.user_nof_get(unit);
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_USER_QUAL_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_qual_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for pmf user action ID
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_user_action_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.action.user_nof_get(unit);
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_USER_ACTION_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_action_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple alloc algorithm for pmf user qualifier ID
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_user_qual_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state will be deinit together.
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple alloc algorithm for pmf user action ID
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_user_action_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state will be deinit together.
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple allocation algorithm for DataBase ID alloction
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_group_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.group.nof_fgs_get(unit);
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_GROUP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.field_group_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple allocation algorithm for DataBase ID alloction
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_group_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple allocation algorithm for APP_DB_ID for SEXEM.
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_sexem_app_db_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = dnx_data_field.exem.small_min_app_db_id_range_get(unit);
    data.nof_elements =
        dnx_data_field.exem.small_max_app_db_id_range_get(unit) -
        dnx_data_field.exem.small_min_app_db_id_range_get(unit) + 1;
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_SEXEM_APP_DB_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.sexem_app_db_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple allocation algorithm for APP_DB_ID for SEXEM.
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_sexem_app_db_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple allocation algorithm for APP_DB_ID for LEXEM.
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_lexem_app_db_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = dnx_data_field.exem.large_min_app_db_id_range_get(unit);
    data.nof_elements =
        dnx_data_field.exem.large_max_app_db_id_range_get(unit) -
        dnx_data_field.exem.large_min_app_db_id_range_get(unit) + 1;
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_LEXEM_APP_DB_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.lexem_app_db_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple allocation algorithm for APP_DB_ID for LEXEM.
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_lexem_app_db_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple allocation algorithm for ACE ID.
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ace_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = dnx_data_field.ace.min_ace_id_dynamic_range_get(unit);
    data.nof_elements = dnx_data_field.ace.nof_ace_id_get(unit) - dnx_data_field.ace.min_ace_id_dynamic_range_get(unit);
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_ACE_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple allocation algorithm for ACE ID.
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ace_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple allocation algorithm for ACE key (PPMC key).
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ace_key_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    

    sal_memset(&data, 0, sizeof(data));
    data.first_element = dnx_data_field.ace.min_key_range_pmf_get(unit);
    
    data.nof_elements =
        MIN(dnx_data_field.ace.max_key_range_pmf_get(unit) - dnx_data_field.ace.min_key_range_pmf_get(unit) + 1,
            DNXC_SW_STATE_ALLOC_MAX_BYTES);
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_ACE_KEY, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_key.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple allocation algorithm for ACE key (PPMC key).
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ace_key_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This is init for Pool that being used from entry access id allocation
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * The access_id is being allocated per stage
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_entry_tcam_access_id_init(
    int unit)
{
    uint32 nof_tcam_entries_per_core;
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    nof_tcam_entries_per_core = (dnx_data_field.tcam.nof_big_banks_get(unit) *
                                 dnx_data_field.tcam.nof_big_bank_lines_get(unit)) +
        (dnx_data_field.tcam.nof_small_banks_get(unit) * dnx_data_field.tcam.nof_small_bank_lines_get(unit));
    data.nof_elements = nof_tcam_entries_per_core * dnx_data_device.general.nof_cores_get(unit);
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_ENTRY_TCAM_ACCESS_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.entry_tcam_access_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This is de-init for Pool that being used from entry access id allocation
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * The access_id is being allocated per stage
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_entry_tcam_access_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This is init for Pool that being used for CS profile ID per OPCODE
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_kbp_opcode_cs_profile_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    /** We do not allocate profile 0 as it is the default value.*/
    data.first_element = 1;
    /** The number of profiles available in the bit number minus 0.*/
    data.nof_elements = SAL_UPTO_BIT(dnx_data_field.external_tcam.size_apptype_profile_id_get(unit));
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FIELD_KBP_OPCODE_CS_PROFILE_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.kbp_opcode_cs_profile_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This is de-init for Pool that being used for CS profile ID per OPCODE
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_kbp_opcode_cs_profile_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple allocation algorithm for DataBase ID alloction
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_flush_profile_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.exem.small_nof_flush_profiles_get(unit);
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_FLUSH_PROFILE_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_field_info_sw.flush_profile_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DE-init simple allocation algorithm for DataBase ID alloction
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_flush_profile_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state will be deinit together.
     */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the resource manager of field module
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**PMF_A 'actions' (extractions*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_pmf_a_fem_map_index_init(unit));
    /**PMF_A PROGRAM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_pmf_a_fem_pgm_id_init(unit));
    /**USER_QUAL_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_user_qual_id_init(unit));
    /**USER_ACTION_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_user_action_id_init(unit));
    /**FIELD_GROUP_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_group_id_init(unit));
    /**SEXEM_APP_DB_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_sexem_app_db_id_init(unit));
    /**LEXEM_APP_DB_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_lexem_app_db_id_init(unit));
    if (dnx_data_field.ace.supported_get(unit))
    {
        /**ACE_ID*/
        SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ace_id_init(unit));
        /**ACE_KEY*/
        SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ace_key_init(unit));
    }
    /**ENTRY ACCESS_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_entry_tcam_access_id_init(unit));
    /**KBP CS profile*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_kbp_opcode_cs_profile_id_init(unit));
    if (dnx_data_field.exem_learn_flush_machine.supported_get(unit))
    {
        /**SEXEM Flush profile*/
        SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_flush_profile_id_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  De-init the resource manager of field module
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**PMF_A 'actions' (extractions)*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_pmf_a_fem_map_index_deinit(unit));
    /**PMF_A FEM_PROGRAM*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_pmf_a_fem_pgm_id_deinit(unit));
    /**USER_QUAL_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_user_qual_id_deinit(unit));
    /**USER_ACTION_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_user_action_id_deinit(unit));
    /**FIELD_GROUP_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_group_id_deinit(unit));
    /**SEXEM_APP_DB_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_sexem_app_db_id_deinit(unit));
    /**LEXEM_APP_DB_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_lexem_app_db_id_deinit(unit));
    /**ACE_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ace_id_deinit(unit));
    /**ACE_KEY*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ace_key_deinit(unit));
    /**ENTRY ACCESS_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_entry_tcam_access_id_deinit(unit));
    /**KBP CS profile*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_kbp_opcode_cs_profile_id_deinit(unit));
    /**SEXEM Flush profile*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_flush_profile_id_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see algo_field.h
 */
shr_error_e
dnx_algo_field_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init the algo_field_info_sw sw state structure.
     */
    SHR_IF_ERR_EXIT(algo_field_info_sw.init(unit));
    /*
     * Init field resource manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_init(unit));
    /*
     * Init field context resource manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_context_res_mngr_init(unit));
    /*
     * Init field Key and FFC resource manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_key_res_mngr_init(unit));
    /*
     * Init field Action SW state.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_init(unit));
    /*
     * Init template managers will be added here
     */

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Deinit all need algorithm for Field module functionality
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_field_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * De-Init field FES resource manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_deinit(unit));
    /*
     * De-Init field Context resource manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_context_res_mngr_deinit(unit));
    /*
     * De-Init field Key and FFC resource manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_key_res_mngr_deinit(unit));
    /*
     * De-Init field Action SW state.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_state_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
* See algo_field.h
*/
shr_error_e
dnx_algo_field_group_is_allocated(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    /** Check if given fg_id is already allocated */
    SHR_IF_ERR_EXIT(algo_field_info_sw.field_group_id.is_allocated(unit, fg_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See algo_field.h
*/
shr_error_e
dnx_algo_field_ace_id_is_allocated(
    int unit,
    dnx_field_ace_id_t ace_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    /** Check if given ACE ID is already allocated */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_id.is_allocated(unit, ace_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See algo_field.h
*/
shr_error_e
dnx_algo_field_ace_key_is_allocated(
    int unit,
    dnx_field_ace_key_t ace_key,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    /** Check if given ACE key is already allocated */
    SHR_IF_ERR_EXIT(algo_field_info_sw.ace_key.is_allocated(unit, ace_key, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See algo_field.h
*/
shr_error_e
dnx_algo_field_kbp_opcode_cs_profile_id_is_allocated(
    int unit,
    int cs_prodfile_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    /** Check if given opcode cs profile is already allocated */
    SHR_IF_ERR_EXIT(algo_field_info_sw.kbp_opcode_cs_profile_id.is_allocated(unit, cs_prodfile_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See algo_field.h
*/
shr_error_e
dnx_algo_field_flush_profile_is_allocated(
    int unit,
    int flush_profile_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    /** Check if given flush_profile_id is already allocated */
    SHR_IF_ERR_EXIT(algo_field_info_sw.flush_profile_id.is_allocated(unit, flush_profile_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}
