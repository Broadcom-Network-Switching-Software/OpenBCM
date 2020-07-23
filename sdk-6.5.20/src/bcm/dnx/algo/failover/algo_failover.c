/** \file algo_failover.c
 *
 * Wrapper functions for tag bitmap allocation manager used by FEC PATH SELECT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FAILOVER
/*
 * INCLUDE FILES:
 * {
 */
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/failover/failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_path_select_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    int grain_size;
    uint8 max_tag_value;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    /*
     * set grain size to be equal to bank size 
     */
    grain_size =
        dnx_data_failover.path_select.fec_size_get(unit) /
        dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit);
    max_tag_value = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = grain_size;
    extra_create_info.max_tag_value = max_tag_value;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = create_data->first_element;
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, module_id, res_tag_bitmap, &res_tag_bitmap_create_info,
                                               &extra_create_info, nof_elements, alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculates the number of entries allocated in ecmp bank reagen in order to decide
 *          whether ecmp bank need to be allocated,destroyed or untauched.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ecmp_bank - ecmp bank index
 * \param [in] module_id -
 * \param [in] res_tag_bitmap -
 * \param [in] bank_occupancy - nof allocated elements
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_failover_fec_ecmp_bank_occupancy_get(
    int unit,
    uint32 ecmp_bank,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 *bank_occupancy)
{
    uint32 grain_index, base_grain_idx;
    uint32 nof_elements;
    SHR_FUNC_INIT_VARS(unit);

    *bank_occupancy = 0;

    /*
     * Receives the number of allocated elements for all fec banks parallel to a ecmp bank.
     */
    base_grain_idx = ecmp_bank * dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit);
    for (grain_index = base_grain_idx;
         grain_index < base_grain_idx + dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit); grain_index++)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_nof_used_elements_in_grain_get
                        (unit, module_id, res_tag_bitmap, grain_index, &nof_elements));
        /*
         * for the last bank 2 entries should be reduced for 2 last reserved entries
         */
        if (grain_index == dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit) - 1)
        {
            nof_elements -= 2;
        }
        *bank_occupancy += nof_elements;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_path_select_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags;
    uint8 with_id;
    uint8 alloc_simulation;
    uint8 tag;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    uint32 ecmp_bank, ecmp_bank_occ;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    SHR_NULL_CHECK(extra_arguments, _SHR_E_PARAM, "extra_arguments");

    tag = *((uint8 *) extra_arguments);

    /*
     * Translate the allocation flags.
     * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
     * different set of flags.
     */
    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);
    alloc_simulation = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_SIMULATION);
    internal_flags = 0;
    internal_flags |= (with_id) ? RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;
    internal_flags |= (alloc_simulation) ? RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.count = 1;
    res_tag_bitmap_alloc_info.tag = tag;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

    /*
     * for the last 2 elements, ecmp bank does not need to be updated.
     */
    if (*element >= dnx_data_failover.path_select.fec_facility_protection_get(unit))
    {
        SHR_EXIT();
    }

    /*
     * allocate ecmp bank if this is the first entry allocated at the parallel ecmp bank
     */
    ecmp_bank = *element / dnx_data_failover.path_select.fec_bank_size_get(unit) /
        dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_failover_fec_ecmp_bank_occupancy_get
                    (unit, ecmp_bank, module_id, res_tag_bitmap, &ecmp_bank_occ));
    if (ecmp_bank_occ == 1)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_extended_bank_for_failover_allocate(unit, ecmp_bank));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_path_select_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    uint32 ecmp_bank, ecmp_bank_occ;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_free(unit, module_id, res_tag_bitmap, element));

    /*
     * ecmp bank size calculation
     */
    ecmp_bank = element / dnx_data_failover.path_select.fec_bank_size_get(unit) /
        dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit);

    /*
     * destroy the parallel ecmp bank if it's empty
     */
    SHR_IF_ERR_EXIT(dnx_algo_failover_fec_ecmp_bank_occupancy_get
                    (unit, ecmp_bank, module_id, res_tag_bitmap, &ecmp_bank_occ));
    if (ecmp_bank_occ == 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_extended_bank_for_failover_destroy(unit, ecmp_bank));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_path_select_bitmap_several_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags;
    uint8 with_id;
    uint8 alloc_simulation;
    uint8 tag;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    SHR_NULL_CHECK(extra_arguments, _SHR_E_PARAM, "extra_arguments");

    tag = *((uint8 *) extra_arguments);

    /*
     * Translate the allocation flags.
     * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
     * different set of flags.
     */
    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);
    alloc_simulation = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_SIMULATION);
    internal_flags = 0;
    internal_flags |= (with_id) ? RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;
    internal_flags |= (alloc_simulation) ? RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.count = nof_elements;
    res_tag_bitmap_alloc_info.tag = tag;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_path_select_bitmap_several_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int element)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_free_several(unit, module_id, res_tag_bitmap, nof_elements, element));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_bank_for_ecmp_extended_allocate(
    int unit,
    int bank_id)
{
    uint32 alloc_flags;
    int failover_id;
    uint8 hierarchy;
    uint32 nof_elements;
    SHR_FUNC_INIT_VARS(unit);

    failover_id = bank_id * dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit) *
        dnx_data_failover.path_select.fec_bank_size_get(unit);
    alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    nof_elements = dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit) *
        dnx_data_failover.path_select.fec_bank_size_get(unit);
    /*
     * for the last bank, 2 entries should be reduced for 2 last reserved entries
     */
    if (bank_id == dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit) - 1)
    {
        nof_elements -= 2;
    }
    SHR_IF_ERR_EXIT(algo_failover_db.
                    fec_path_select.allocate_several(unit, alloc_flags, nof_elements, &hierarchy, &failover_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_fec_bank_for_ecmp_extended_free(
    int unit,
    int bank_id)
{
    int failover_id;
    uint32 nof_elements;
    SHR_FUNC_INIT_VARS(unit);

    failover_id = bank_id * dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit) *
        dnx_data_failover.path_select.fec_bank_size_get(unit);
    nof_elements = dnx_data_l3.ecmp.nof_fec_banks_per_extended_ecmp_bank_get(unit) *
        dnx_data_failover.path_select.fec_bank_size_get(unit);
    /*
     * for the last bank, 2 entries should be reduced for 2 last reserved entries
     */
    if (bank_id == dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit) - 1)
    {
        nof_elements -= 2;
    }
    SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.free_several(unit, nof_elements, failover_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_failover_res_name_get(
    int unit,
    int failover_type,
    char **name)
{
    SHR_FUNC_INIT_VARS(unit);

    if (failover_type == DNX_FAILOVER_TYPE_INGRESS)
    {
        *name = DNX_ALGO_FAILOVER_RES_ING_PATH_SELECT;
    }
    else if (failover_type == DNX_FAILOVER_TYPE_ENCAP)
    {
        *name = DNX_ALGO_FAILOVER_RES_EGR_PATH_SELECT;
    }
    else if (failover_type == DNX_FAILOVER_TYPE_FEC)
    {
        *name = DNX_ALGO_FAILOVER_RES_FEC_PATH_SELECT;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected failover type %d .\n", failover_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** } **/

/*
 * Init-Deinit
 * {
 */
/*
 * See .h
 */
shr_error_e
dnx_algo_failover_init(
    int unit)
{
    dnx_algo_res_create_data_t create_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_failover_db.init(unit));
    /*
     * ALLOC MNGR - INGRESS PATH SELECT
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_failover.path_select.ingress_size_get(unit);
    sal_strncpy(create_data.name, DNX_ALGO_FAILOVER_RES_ING_PATH_SELECT, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_failover_db.ing_path_select.create(unit, &create_data, NULL));

    /*
     * ALLOC MNGR - EGRESS PATH SELECT
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_failover.path_select.egress_size_get(unit);
    sal_strncpy(create_data.name, DNX_ALGO_FAILOVER_RES_EGR_PATH_SELECT, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_failover_db.egr_path_select.create(unit, &create_data, NULL));

    /*
     * ALLOC MNGR - FEC PATH SELECT
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_failover.path_select.fec_size_get(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FAILOVER_FEC_PATH_SELECT;
    sal_strncpy(create_data.name, DNX_ALGO_FAILOVER_RES_FEC_PATH_SELECT, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;

}

/*
 * }
 * Init-Deinit
 */

/**
 * }
 */
