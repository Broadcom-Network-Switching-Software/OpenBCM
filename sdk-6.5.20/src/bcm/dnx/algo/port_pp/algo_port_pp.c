/**
 * \file algo_port_pp.c
 * Internal DNX Port PP Managment APIs
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
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

/**
 * }
 */

/**
 * Force Forward
 * {
 */

/**
 * \brief - create the template manager used to track the traps used in force forward
 */
static shr_error_e
dnx_algo_port_pp_force_forward_init(
    int unit)
{
    dnx_algo_template_create_data_t create_info;
    uint32 force_forward_template_data;
    int nof_cores, nof_traps;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** int vars */
    sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));
    force_forward_template_data = -1;

    /** create the template manager used to track the traps used in force forward */
    create_info.flags = 0;
    create_info.first_profile = 0;
    create_info.nof_profiles = dnx_data_port.general.nof_pp_ports_get(unit);
    create_info.max_references = dnx_data_port.general.nof_pp_ports_get(unit);
    create_info.default_profile = 0;
    create_info.data_size = sizeof(uint32);
    create_info.default_data = &force_forward_template_data;
    sal_strncpy(create_info.name, DNX_PORT_PP_FORCE_FORWARD_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.mngr.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.mngr.create(unit, core_id, &create_info, NULL));
    }

    /** allocate additional array to store the trap ids */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_traps = dnx_data_port.general.nof_pp_ports_get(unit);
    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.trap_id.alloc(unit, nof_cores, nof_traps));

exit:
    SHR_FUNC_EXIT;
}

/**see .h file */
void
dnx_algo_port_pp_force_forward_template_cb(
    int unit,
    const void *data)
{
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "force forward destination",
                                   *((uint32 *) data), "force forward destination", NULL);
    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief -
 *   Intialize Port PP SIT profile algorithms.
 *   The function creats SIT profile Template profiles.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_algo_port_pp_init
 */
static shr_error_e
algo_port_sit_profile_template_init(
    int unit)
{
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for Port SIT profile
     */

    sal_memset(&data, 0, sizeof(data));

    data.first_profile = 0;

    data.nof_profiles = dnx_data_port_pp.general.nof_egress_sit_profile_get(unit);
    data.max_references = dnx_data_port.general.nof_pp_ports_get(unit) * dnx_data_device.general.nof_cores_get(unit);
    data.data_size = sizeof(dnx_sit_profile_t);
    sal_strncpy(data.name, DNX_ALGO_EGRESS_PP_PORT_SIT_PROFILE_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_pp_port_sit_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file algo_port_pp.h for description.
 */
void
dnx_algo_port_esem_access_cmd_template_print_cb(
    int unit,
    const void *data)
{
    uint8 esem_access_id, app_db_id, esem_offset, default_profile, index;
    dnx_esem_cmd_data_t *esem_cmd_data = (dnx_esem_cmd_data_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    for (index = 0; index < ESEM_ACCESS_IF_COUNT; index++)
    {
        if (esem_cmd_data->esem[index].valid == TRUE)
        {
            esem_access_id = index + 1;
            app_db_id = esem_cmd_data->esem[index].app_db_id;
            esem_offset = esem_cmd_data->esem[index].designated_offset;
            default_profile = esem_cmd_data->esem[index].default_result_profile;

            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8,
                                           "esem_access_if", esem_access_id, NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "app_db_id", app_db_id, NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8,
                                           "designated_offset", esem_offset, NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8,
                                           "default_result_profile", default_profile, NULL, NULL);
        }
    }
    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
 * \brief -
 *   Create and Intialize the algorithm for esem access commands per template.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   None
 */
static shr_error_e
algo_port_esem_access_cmd_template_init(
    int unit)
{
    dnx_algo_template_create_data_t template_data;
    dnx_esem_cmd_data_t esem_cmd_data;
    smart_template_create_info_t extra_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize the templates data/properties
     */
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    template_data.data_size = sizeof(dnx_esem_cmd_data_t);
    template_data.default_data = &esem_cmd_data;
    template_data.first_profile = 0;
    template_data.default_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    template_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE |
        DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    template_data.nof_profiles = dnx_data_esem.access_cmd.nof_cmds_get(unit);
    template_data.max_references = dnx_data_port.general.nof_pp_ports_get(unit) +
        dnx_data_lif.out_lif.nof_local_out_lifs_get(unit) + 1;
    /*
     * Use the smart template algorithm to allocate template and resource bitmap for range allocation
     * and for even distribution
     */
    template_data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE_EVEN_DISTRIBUTION;
    sal_strncpy(template_data.name, DNX_ALGO_TEMPLATE_ESEM_ACCESS_CMD, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Extra info that the SMART_TEMPLATE_EVEN_DISTRIBUTION algorithm requires
     */
    extra_info.resource_flags =
        RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG | RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;
    extra_info.resource_create_info.grain_size =
        dnx_data_esem.access_cmd.nof_cmds_get(unit) / DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS;
    extra_info.resource_create_info.max_tag_value = extra_info.resource_create_info.grain_size;
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.create(unit, &template_data, (void *) &extra_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   The function destroy esem-accesee-cmd template.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  algo_port_esem_access_cmd_template_init
 */
static shr_error_e
algo_port_esem_access_cmd_template_deinit(
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

/**
 * \brief -
 *   Create and Intialize the algorithm for esem default result profile.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  dnx_algo_port_pp_init
 */
static shr_error_e
algo_port_esem_default_result_profile_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * ESEM-DEFAULT-RESULT_PROFILE resource management
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    data.first_element = 0;
    data.nof_elements = dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit);
    sal_strncpy(data.name, DNX_ALGO_ESEM_DEFAULT_RESULT_PROFILE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  The function destroy the esem-default-result-profile resource.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  * None
 *
 * \see
 *  algo_port_match_esem_default_result_profile_init
 *  dnx_algo_port_pp_deinit
 */
static shr_error_e
algo_port_esem_default_result_profile_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Template Manager for LB PP Port Profile enablers:
 */
static shr_error_e
dnx_algo_pp_port_lb_profile_template_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    uint32 default_data;
    SHR_FUNC_INIT_VARS(unit);

    default_data = 0;
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = sizeof(uint32);
    data.default_data = &default_data;
    data.default_profile = DEFAULT_PP_PORT_LB_PROFILE;
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = 4;
    data.max_references = dnx_data_port.general.nof_pp_ports_get(unit) * dnx_data_device.general.nof_cores_get(unit);
    sal_strncpy(data.name, DNX_ALGO_PP_PORT_LB_LAYER_PROFILE_ENABLERS, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.pp_port_lb_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Allocate array to store recycle_app information
 */
static shr_error_e
dnx_algo_pp_port_recycle_app_init(
    int unit)
{
    int nof_cores = 0, core_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** allocate array to store recycle_app information */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        SHR_IF_ERR_EXIT(algo_port_pp_db.recycle_app.alloc(unit, core_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See header file algo_port_pp.h for description.
 */

void
dnx_algo_pp_port_lb_profile_print_entry_cb(
    int unit,
    const void *data)
{
    uint32 *layers_disable = (uint32 *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Layers Disable", *layers_disable, NULL, NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * }
 */

/**
* Global Functions:
* {
*/

/*
* See header file algo_port_pp.h for description.
*/
shr_error_e
dnx_algo_port_pp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_port_pp_db.init(unit));

    /*
     * Initialize force forward sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_force_forward_init(unit));

    /*
     * Initialize Port TPID Template profile
     */
    SHR_IF_ERR_EXIT(algo_port_tpid_init(unit));

    /*
     * Initialize Port SIT profile template
     */
    SHR_IF_ERR_EXIT(algo_port_sit_profile_template_init(unit));

    /*
     * Initialize ESEM access command template
     */
    SHR_IF_ERR_EXIT(algo_port_esem_access_cmd_template_init(unit));

    /*
     * Initialize ESEM default result profile resource
     */
    SHR_IF_ERR_EXIT(algo_port_esem_default_result_profile_init(unit));

    /*
     * Initialize pp port lb enablers vector template
     */
    SHR_IF_ERR_EXIT(dnx_algo_pp_port_lb_profile_template_init(unit));
    /*
     * Initialize pp port recycle app
     */
    SHR_IF_ERR_EXIT(dnx_algo_pp_port_recycle_app_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_pp.h for description.
*/
shr_error_e
dnx_algo_port_pp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_port_tpid_deinit(unit));

    /*
     * Destroy ESEM access command template
     */
    SHR_IF_ERR_EXIT(algo_port_esem_access_cmd_template_deinit(unit));

    /*
     * Destroy ESEM default result profile resource
     */
    SHR_IF_ERR_EXIT(algo_port_esem_default_result_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_pp.h for description.
*/

void
dnx_algo_egress_sit_profile_template_print_cb(
    int unit,
    const void *data)
{
    dnx_sit_profile_t *sit_profile = (dnx_sit_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "tag_type", sit_profile->tag_type,
                                   "Sit tag type, see dbal_enum_value_field_sit_tag_type_e", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "tpid", sit_profile->tpid,
                                   "Tpid value of the sit tag", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "pcp_dei_src",
                                   sit_profile->pcp_dei_src,
                                   "pcp dei value source, see dbal_enum_value_field_sit_pcp_dei_src_e", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "vid_src",
                                   sit_profile->vid_src,
                                   "vid value source, see dbal_enum_value_field_sit_vid_src_e", NULL);
    SW_STATE_PRETTY_PRINT_FINISH();

}

/*
* See header file algo_port_pp.h for description.
*/
void
dnx_algo_egress_acceptable_frame_type_profile_template_print_cb(
    int unit,
    const void *data)
{
    int outer_tpid_indx;
    int inner_tpid_indx;

    /*
     * Print every acceptable frame type data entry in the profile in the following format
     */
    DNX_SW_STATE_PRINT(unit, "keys ={outer_tpid, inner_tpid}, data = {the acceptable frame type entry data}\n");

    /*
     * Priority tag is not considered here because only outer_tpid/inner_tpid is used to access acceptable frame type table
     */
    for (outer_tpid_indx = 0; outer_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; outer_tpid_indx++)
    {
        for (inner_tpid_indx = 0; inner_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; inner_tpid_indx++)
        {
            /*
             * Takes the bit out of the template and print it:
             */
            acceptable_frame_type_template_t *template_data;
            /*
             * see dnx_sw_state_dump.h for more information
             */
            DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, inner_tpid_indx);
            template_data = (acceptable_frame_type_template_t *) data;

            DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, outer_tpid_indx, " ");
            DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, inner_tpid_indx, " ");
            DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit,
                                                  template_data->acceptable_frame_type_template[outer_tpid_indx]
                                                  [inner_tpid_indx], " ");

            DNX_SW_STATE_PRINT(unit, "keys ={%d, %d}, data = 0x%X\n", outer_tpid_indx, inner_tpid_indx,
                               template_data->acceptable_frame_type_template[outer_tpid_indx][inner_tpid_indx]);
        }
        /*
         * see dnx_sw_state_dump.h for more information
         */
        DNX_SW_STAET_DUMP_END_OF_STRIDE(unit);
    }
}

/**
 * }
 */
