/**
 * \file algo_port_tpid.c
 * Internal DNX Port TPID Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/port_tpid_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <shared/swstate/sw_state_resmgr.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>

/**
 * \brief -
 * Internal function for printing INGRESS LLVP Template Profile.
 * Per data entry in the template which is accessed by the key
 * {is_prio, outer_tpid, inner_tpid} it would print that LLVP
 * data.
 *
 * \param [in] unit - relevant unit.
 * \param [in] data - pointer to ingress Port TPID Template
 *        profile.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_algo_port_tpid_print_ingress_cb
 */
static shr_error_e
algo_port_tpid_print_ingress(
    int unit,
    const void *data)
{

    int prio_indx;
    int outer_tpid_indx;
    int inner_tpid_indx;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Need this to fix compilation because both macros contain variables as well as statements
     */
    {
        SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

        /*
         * Print every LLVP data entry in the profile in the following format
         */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "",
                                       "keys ={is_prio, outer_tpid, inner_tpid}, data = {the LLVP entry data}\n",
                                       NULL, NULL);

        /*
         * LLVP is_prio key possible cases:
         *  1. outer tag is priority tag (0).
         *  2. outer tag is not priority tag (1).
         */
        for (prio_indx = 0; prio_indx < 2; prio_indx++)
        {
            for (outer_tpid_indx = 0; outer_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; outer_tpid_indx++)
            {
                for (inner_tpid_indx = 0; inner_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; inner_tpid_indx++)
                {
                    /*
                     * Takes the bits out of the buffer and print it:
                     */
                    dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
                    int offset;
                    uint32 llvp_data_entry = 0;

                    llvp_parser_info.is_outer_prio = prio_indx;
                    llvp_parser_info.outer_tpid = outer_tpid_indx;
                    llvp_parser_info.inner_tpid = inner_tpid_indx;

                    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_parser_info_to_ingress_llvp_index
                                    (unit, &llvp_parser_info, &offset));

                    /*
                     * SHR_BITCOPY_RANGE(_dest, _dest_offset,_src, _src_offset, _num_bits)
                     */
                    SHR_BITCOPY_RANGE(&llvp_data_entry, 0, data, offset * DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT,
                                      DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT);

                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "", "keys = ", NULL, NULL);
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "prio_indx", prio_indx,
                                                   "Indicates whether outer tag is priority tag (0) or not (1)", NULL);
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "outer_tpid_indx",
                                                   outer_tpid_indx, "Outer TPID index", NULL);
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "inner_tpid_indx",
                                                   inner_tpid_indx, "Inner TPID index", NULL);
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "data = ", llvp_data_entry,
                                                   "LLVP data entry", "0x%X");
                }
            }
        }

        SW_STATE_PRETTY_PRINT_FINISH();
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * Internal function for printing EGRESS LLVP Template Profile.
 * Per each data entry in the template which is accessed by the
 * key {outer_tpid, is_prio, inner_tpid} it would print that
 * LLVP data.
 *
 * \param [in] unit - relevant unit.
 * \param [in] data - pointer to egress Port TPID Template
 *        profile.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  dnx_algo_port_tpid_print_egress_cb
 */
static shr_error_e
algo_port_tpid_print_egress(
    int unit,
    const void *data)
{
    int outer_tpid_indx;
    int prio_indx;
    int inner_tpid_indx;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Need this to fix compilation because both macros contain variables as well as statements
     */
    {
        SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

        /*
         * Print every LLVP data entry in the profile in the following format
         */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "",
                                       "keys ={outer_tpid, is_prio, inner_tpid}, data = {the LLVP entry data}\n",
                                       NULL, NULL);

        /*
         * LLVP is_prio key possible cases:
         *  1. outer tag is priority tag (0).
         *  2. outer tag is not priority tag (1).
         */
        for (prio_indx = 0; prio_indx < 2; prio_indx++)
        {
            for (outer_tpid_indx = 0; outer_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; outer_tpid_indx++)
            {
                for (inner_tpid_indx = 0; inner_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; inner_tpid_indx++)
                {
                    /*
                     * Takes the bits out of the buffer and print it:
                     */
                    dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
                    int offset;
                    uint32 llvp_data_entry = 0;

                    llvp_parser_info.is_outer_prio = prio_indx;
                    llvp_parser_info.outer_tpid = outer_tpid_indx;
                    llvp_parser_info.inner_tpid = inner_tpid_indx;

                    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_parser_info_to_egress_llvp_index
                                    (unit, &llvp_parser_info, &offset));

                    /*
                     * SHR_BITCOPY_RANGE(_dest, _dest_offset,_src, _src_offset, _num_bits)
                     */
                    SHR_BITCOPY_RANGE(&llvp_data_entry, 0, data, offset * DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT,
                                      DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT);

                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "", "keys = ", NULL, NULL);
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "outer_tpid_indx",
                                                   outer_tpid_indx, "Outer TPID index", NULL);
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "prio_indx", prio_indx,
                                                   "Indicates whether outer tag is priority tag (0) or not (1)", NULL);
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "inner_tpid_indx",
                                                   inner_tpid_indx, "Inner TPID index", NULL);
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "data = ", llvp_data_entry,
                                                   "LLVP data entry", "0x%X");
                }
            }
        }

        SW_STATE_PRETTY_PRINT_FINISH();
    }

exit:
    SHR_FUNC_EXIT;

}

/*
* See header file algo_port_tpid.h for description.
*/
shr_error_e
dnx_port_tpid_tag_struct_hw_resource_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    uint32 current_tag;
    uint32 nof_vlan_tag_formats;
    resource_tag_bitmap_tag_info_t tag_info;

    /*
     * Since HW TAG-Struct is divided to ranges (UNTAG rang, S_TAG range, C_TAG range, etc),
     * need to define to the advanced resource allocarion manager each range the high and low bounds:
     */
    int tag_struct_range_high[bcmNofTagStructType] = {
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_2,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_4, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_8,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_8, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_4,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_2,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_2
    };
    int tag_struct_range_low[bcmNofTagStructType] = {
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_1, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_1,
        DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_1
    };

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tag_info, 0, sizeof(tag_info));

    /*
     * Create a res_tag_bitmap to manage the entries.
     * Since we have several different potential sizes for each tag, we'll use a grain of size 1.
     * There's one tag for every struct tag, and one extra for unused entries.
     */
    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = 1;
    extra_create_info.max_tag_value = bcmNofTagStructType + 1;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = 0;
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, &res_tag_bitmap_create_info, &extra_create_info, nof_elements,
                     alloc_flags));

    /*
     * Set the ranges.
     */
    for (current_tag = 0; current_tag < bcmNofTagStructType; current_tag++)
    {
        tag_info.tag = current_tag;
        tag_info.force_tag = TRUE;
        tag_info.element = tag_struct_range_low[current_tag];
        tag_info.nof_elements = tag_struct_range_high[current_tag] - tag_struct_range_low[current_tag] + 1;

        SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, *res_tag_bitmap, &tag_info));
    }

    /*
     * Set the unused entries.
     * Since the HW TAG-Struct is 5 bits (32 options) but currently "NOF HW TAg-Struct" is less than 32,
     * define the last range as "invalid" TAG-Struct ("bcmNofTagStructType") so no one can allocate it.
     */
    nof_vlan_tag_formats = dnx_data_vlan.vlan_translate.nof_vlan_tag_formats_get(unit);

    tag_info.tag = bcmNofTagStructType;
    tag_info.force_tag = TRUE;
    tag_info.element = DBAL_NOF_ENUM_INCOMING_TAG_STRUCTURE_VALUES;
    tag_info.nof_elements = nof_vlan_tag_formats - DBAL_NOF_ENUM_INCOMING_TAG_STRUCTURE_VALUES + 1;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, *res_tag_bitmap, &tag_info));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_tpid.h for description.
*/
shr_error_e
dnx_port_tpid_tag_struct_hw_resource_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    bcm_port_tag_struct_type_t tag_struct_type;
    SHR_FUNC_INIT_VARS(unit);

    tag_struct_type = *(bcm_port_tag_struct_type_t *) extra_arguments;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.flags = 0;
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        res_tag_bitmap_alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
    }
    res_tag_bitmap_alloc_info.tag = tag_struct_type;
    res_tag_bitmap_alloc_info.count = 1;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_tpid.h for description.
*/
shr_error_e
algo_port_tpid_init(
    int unit)
{
    dnx_algo_template_create_data_t data_template_mgr;
    dnx_algo_res_create_data_t data_res_mgr;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for Port TPID profile
     */

    sal_memset(&data_template_mgr, 0, sizeof(data_template_mgr));

    data_template_mgr.first_profile = 1;

    /*
     * There is going to be a HW change - the profile would be 3 or 4 bits.
     * After ARCH/ASIC defines it properly - read SOC properties / DNX data etc.
     */

    /*
     * Note:
     * First Ingress profile (0) is used for Ingress Native AC default and is not managed by the template management.
     */
    data_template_mgr.nof_profiles =
        (dnx_data_port_pp.
         general.nof_ingress_llvp_profiles_get(unit) << (dnx_data_vlan.llvp.ext_vcp_en_get(unit) ? 1 : 0)) - 1;
    data_template_mgr.max_references =
        dnx_data_port.general.nof_pp_ports_get(unit) * dnx_data_device.general.nof_cores_get(unit);
    data_template_mgr.data_size = DNX_PORT_TPID_CLASS_TEMPLATE_SIZE_NUM_OF_BYTES;
    sal_strncpy(data_template_mgr.name, DNX_ALGO_PORT_TPID_LLVP_PROFILE_INGRESS_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.create(unit, &data_template_mgr, NULL));

    /*
     * Note:
     * First Egress profile (0) is used for Egress Native AC default and is not managed by the template management.
     * Last Egress profile (7) is used for Egress IP Tunnel and is not managed by the template management.
     */
    data_template_mgr.nof_profiles = dnx_data_port_pp.general.nof_egress_llvp_profiles_get(unit) - 2;
    sal_strncpy(data_template_mgr.name, DNX_ALGO_PORT_TPID_LLVP_PROFILE_EGRESS_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.create(unit, &data_template_mgr, NULL));

    /*
     * Set a template for egress acceptable frame type profile
     */
    data_template_mgr.first_profile = 0;
    data_template_mgr.nof_profiles = dnx_data_port_pp.general.nof_egress_acceptable_frame_type_profile_get(unit);
    data_template_mgr.data_size = sizeof(acceptable_frame_type_template_t);
    sal_strncpy(data_template_mgr.name, DNX_ALGO_EGRESS_ACCEPTABLE_FRAME_TYPE_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_acceptable_frame_type_profile.create(unit, &data_template_mgr, NULL));

    SHR_IF_ERR_EXIT(port_tpid_db.init(unit));

    sal_memset(&data_res_mgr, 0, sizeof(data_res_mgr));
    /*
     * SW TAG-Struct Ingress Allocation manager:
     */
    data_res_mgr.first_element = 0;
    data_res_mgr.nof_elements = dnx_data_vlan.vlan_translate.nof_vlan_tag_formats_get(unit);
    sal_strncpy(data_res_mgr.name, DNX_ALGO_PORT_TPID_SW_TAG_STRUCT_INGRESS, SW_STATE_RES_NAME_MAX_LENGTH - 1);
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.create
                    (unit, &data_res_mgr, NULL));

    /*
     * SW TAG-Struct Egress Allocation manager:
     */
    sal_strncpy(data_res_mgr.name, DNX_ALGO_PORT_TPID_SW_TAG_STRUCT_EGRESS, SW_STATE_RES_NAME_MAX_LENGTH - 1);
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.create
                    (unit, &data_res_mgr, NULL));

    /*
     * SW TAG-Struct symmetric Allocation manager:
     */
    sal_strncpy(data_res_mgr.name, DNX_ALGO_PORT_TPID_SW_TAG_STRUCT_SYMMETRIC, SW_STATE_RES_NAME_MAX_LENGTH - 1);
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.create
                    (unit, &data_res_mgr, NULL));

    /*
     * Initialize the symmetric indication bitmap. 
     */
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication.alloc_bitmap(unit));

    /*
     * HW TAG-Struct Ingress Allocation manager:
     */
    data_res_mgr.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    data_res_mgr.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_PORT_TPID_TAG_STRUCT;
    sal_strncpy(data_res_mgr.name, DNX_ALGO_PORT_TPID_HW_TAG_STRUCT_INGRESS, SW_STATE_RES_NAME_MAX_LENGTH - 1);
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_ingress.create
                    (unit, &data_res_mgr, NULL));

    /*
     * HW TAG-Struct Egress Allocation manager:
     */
    sal_strncpy(data_res_mgr.name, DNX_ALGO_PORT_TPID_HW_TAG_STRUCT_EGRESS, SW_STATE_RES_NAME_MAX_LENGTH - 1);
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_egress.create(unit, &data_res_mgr, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_tpid.h for description.
*/
shr_error_e
algo_port_tpid_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

/*
 * Sw state will be deinit internal.
 */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_tpid.h for description.
*/
void
dnx_algo_port_tpid_print_ingress_cb(
    int unit,
    const void *data)
{
    shr_error_e rv;

    rv = algo_port_tpid_print_ingress(unit, data);

    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META_U(0, "algo_port_tpid_print_ingress_cb: returned ERROR %d\n"), rv));

    }
}

/*
* See header file algo_port_tpid.h for description.
*/
void
dnx_algo_port_tpid_print_egress_cb(
    int unit,
    const void *data)
{
    shr_error_e rv;

    rv = algo_port_tpid_print_egress(unit, data);

    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META_U(0, "algo_port_tpid_print_egress_cb: returned ERROR %d\n"), rv));

    }
}
