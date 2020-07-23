/** \file algo_l3.c
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/mdb/mdb_init.h>

/**
 * }
 */
/**
* Defines:
* {
*/
#define L3_MYMAC_TABLE_SIZE             64

/**
 * }
 */
/**
* Macro:
* {
*/
/** Indicate whether the ECMP index is in the extended range of IDs. */
#define ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) (ecmp_index > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
/** Receive the bank ID to which the ECMP index belongs */
#define ALGO_L3_ECMP_BANK_GET(unit, ecmp_index) (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ? \
        ((ecmp_index - dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit)) / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)) : \
        (ecmp_index / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)))
/*
 * Return the needed resource name based on the range to which the ECMP ID belongs - basic or extended
 */
#define ALGO_L3_ECMP_GROUP_RESOURCE(unit, ecmp_index) (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ? DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP : DNX_ALGO_L3_RES_ECMP_GROUP)

#define ALGO_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, exec)\
    (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ?\
        DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP_CB.exec :\
        DNX_ALGO_L3_RES_ECMP_GROUP_CB.exec)
/*
 * Returns the logical start address given the bank ID.
 * As the logical address is counted in FECs (not super FECS) the address is the NOF banks FEC capacity multiply the bank ID and each
 * big bank till this bank adds another two FECs.
 */
#define ALGO_L3_GET_START_LOGICAL_ADDRESS_FROM_BANK_ID(unit,bank_id) ((bank_id * dnx_data_l3.fec.bank_size_get(unit)) + 2 * ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(bank_id))

/**
 * }
 */
/**
* Structures:
* {
*/
/**
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/*******************************************
 * FUNCTIONS related to EGR_POINTED        *
 */
/*
 * {
 */
 /**
 * \brief - Initialize EGR_POINTED resources. \n
 *  The function will initialize and create all required EGR_POINTED res-manager.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * Assume Resource Manager is initialized \see dnx_algo_res_init
 *   * Indirect input: DNX-Data information such as nof EGR_POINTED objects.
 *   * Initialize Res manager pools of EGR_POINTED module see the \n
 *     dnx_egr_pointed_algo_res_init .c file which pools are initialized.
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_egr_pointed_res_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * EGR_POINTED resource management - Allocate
     */
    /*
     * For now, do not use element 0. Reserve it for exceptional cases
     */
    data.first_element = dnx_data_l3.egr_pointed.first_egr_pointed_id_get(unit);
    data.nof_elements = dnx_data_l3.egr_pointed.nof_egr_pointed_ids_get(unit) - 1;
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_EGR_POINTED_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * data.desc = "EGR_POINTED - Virtual egress object id which do not use GLEM";
     */
    SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.egr_pointed_res_manager.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - DeInitialize EGR_POINTED resources. \n
 *  The function will deinitialize all recources allocated by the
 *  Init function
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_egr_pointed_res_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add deinit function
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   allocate resource pool for EGR_POINTED
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_algo_l3_egr_pointed_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init alloc manager for virtual egr_pointed IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_egr_pointed_res_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   deallocate resource pool for EGR_POINTED
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_algo_l3_egr_pointed_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * De-init alloc manager for virtual egr_pointed-port IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_egr_pointed_res_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/**
 * \brief - Structure used for the allocation of FECs.
 */
typedef struct
{
    /** Type of the super FEC - indicating with/without Protection/Statistics */
    dbal_enum_value_field_fec_resource_type_e super_fec_type_tag;
    /** Id of the sub-resource in the FEC resource (0 to 29) */
    uint32 sub_resource_index;
} algo_dnx_l3_fec_data_t;

/**
 * \brief - number of EXEM VRRP result types.
 * Currently the following types are supported - VRID(legacy VRRP), port based, VSI based
 * Port+protocol ignore
 */
#define ALGO_L3_VRRP_NOF_EXEM_TYPES        (5)
/** The ID of the FEC sub-resouce index which has values in the ECMP and the FEC ranges. */
#define DNX_ALGO_L3_SHARED_RANGE_FEC_BANK  (1)

void
dnx_algo_l3_print_ingress_mymac_prefix_entry_cb(
    int unit,
    const void *data)
{
    bcm_mac_t *mac = (bcm_mac_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_MAC, "MAC addr", *mac, "ingress_mymac_entry_prefix",
                                   NULL);
    SW_STATE_PRETTY_PRINT_FINISH();
}

void
dnx_algo_l3_print_vrrp_entry_cb(
    int unit,
    const void *data)
{
    algo_dnx_l3_vrrp_tcam_key_t *tcam_info = (algo_dnx_l3_vrrp_tcam_key_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

     /* coverity[overrun-buffer-val:FALSE]  */
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_MAC, "MAC addr", tcam_info->mac_da,
                                   "Destination mac address", NULL);
     /* coverity[overrun-buffer-val:FALSE]  */
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_MAC, "MAC addr mask", tcam_info->mac_da_mask,
                                   "Destination mac address mask", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Protocol group", tcam_info->protocol_group,
                                   "Protocol group", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Protocol group mask",
                                   tcam_info->protocol_group, "Protocol group mask", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

void
dnx_algo_l3_print_ecmp_profiles_cb(
    int unit,
    const void *data)
{
    dnx_l3_ecmp_profile_t *ecmp_profile = (dnx_l3_ecmp_profile_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Member table ID",
                                   ecmp_profile->members_table_id, "Member base", "0x%02X");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Group size", ecmp_profile->group_size,
                                   "Size of ECMP group", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Statistics object ID",
                                   ecmp_profile->statistic_object_id, "Stat object ID", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Statistics object profile",
                                   ecmp_profile->statistic_object_profile, "Stat object profile", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Stateful", ecmp_profile->stateful, "Stateful",
                                   NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Protection", ecmp_profile->protection_flag,
                                   "Protection Flag", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief
 * Create Resource manager for RIF
 *
 * \param [in] unit - the unit number
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_algo_l3_rif_resource_create(
    int unit)
{

    dnx_algo_res_create_data_t res_data;
    int init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&res_data, 0, sizeof(res_data));
    res_data.first_element = 1;
    res_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    res_data.nof_elements = dnx_data_l3.rif.nof_rifs_get(unit) - 1;
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_OUTLIF;
    sal_strncpy(res_data.name, DNX_ALGO_L3_LIF_LOCAL_OUTLIF, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_rif_init_info_get(unit, &init_info));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.create(unit, &res_data, &init_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type,
    dbal_tables_e table_id)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = rif_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.
                    rif_res_manager.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &outlif_info, &rif_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                    (unit, &outlif_info, rif_id, table_id, rif_result_type, 0));

    /*
     * Update the egress global lif counter, because we're adding this rif to the GLEM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_allocate(
    int unit,
    int rif_id,
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type,
    uint32 fixed_entry_size)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RIF_BASIC;
    outlif_info.dbal_result_type = rif_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.
                    rif_res_manager.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &outlif_info, &rif_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                    (unit, &outlif_info, rif_id, DBAL_TABLE_EEDB_RIF_BASIC, rif_result_type, fixed_entry_size));

    /*
     * Update the egress global lif counter, because we're adding this rif to the GLEM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_free(
    int unit,
    int rif_id)
{
    lif_mngr_local_outlif_info_t outlif_info;
    int rif_size;
    dnx_algo_gpm_rif_hw_resources_t rif_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_rif_intf_to_hw_resources(unit, rif_id, &rif_hw_resources));

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RIF_BASIC;
    outlif_info.dbal_result_type = rif_hw_resources.outlif_dbal_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, &outlif_info, &rif_size));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, rif_id));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.free_several(unit, rif_size, rif_id));

    /*
     * Update the egress global lif counter, because we're removing this rif from the GLEM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, -1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Template manager for the ingress MyMac prefixes table, with the following properties:
 * - entries : a 38 bits MyMac prefix which can be use by multiple VSIs.
 * - key : the key is an index to a MyMac prefixes table , the first entry is for a "none MyMac"
 *   VSIs so keys values are ranging between 1-MAX.
 * - max references per prefix is the number of VSIs as basically all the VSIs can point to the same MyMac prefix.
 * - The prefix allocation is performed through the ingress interface create/delete interface. Each time an ingress
 *   interface is created the matching MyMac prefix reference counter increases if a matching entry exists, if not a
 *   new entry is created (assuming the table isn't full). In case an ingress interface is deleted, the matching MyMac
 *   prefix reference counter decreases and if it reaches zero the entry is removed from the table.
 * \par DIRECT_INPUT:
 *   \param [in] unit - the unit number
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_l3_ingress_mymac_prefix_entry_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    int nof_vsis;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    /*
     * Set a template for the MyMac prefix table
     */

    sal_memset(&data, 0, sizeof(data));
    /*
     * The MyMac prefix table contains 64 entries, the first entry is for none MyMac termination VSIs so the
     * template manager will start from the second entry and will use the remaining 63 entries for terminating
     * MyMac prefixes.
     */
    data.data_size = L3_MAC_ADDR_SIZE_IN_BYTES;
    data.first_profile = 1;
    data.nof_profiles = dnx_data_l3.fwd.nof_my_mac_prefixes_get(unit) - 1;
    /*
     * Each VSI can point once to the MyMac prefixes table, so the maximal number of references is the VSIs number.
     */
    data.max_references = nof_vsis;
    sal_strncpy(data.name, DNX_ALGO_L3_INGRESS_MYMAC_PREFIX_TABLE_RESOURCE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Template manager for the VRRP (multiple MyMac) TCAM table, with the following properties:
 * - entries : a 48 bits MAC DA and 2 bits of protocol group. Can be used by multiple My Mac (VRRP) rules
 * - key : the key is an index to VRRP TCAM table.
 *   So keys values are ranging between 0-TCAM_TABLE_SIZE.
 * - max references per TCAM entry is the number of VSIs + EXEM table size (depending on the application).
 * - The prefix allocation is performed through the ingress interface create/delete interface. Each time an ingress
 *   interface is created the matching MyMac attributes reference counter increases if a matching entry exists, if not a
 *   new entry is created (assuming the table isn't full). In case an ingress interface is deleted, the matching MyMac
 *   prefix reference counter decreases and if it reaches zero the entry is removed from the table.
 *
 * \param [in] unit - the unit number
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_l3_vrrp_init(
    int unit)
{
    dnx_algo_template_create_data_t template_data;
    dnx_algo_res_create_data_t resource_data;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * The VRRP TCAM table is split into 2 tables. The first half is used for VSI based multiple my mac, the second half
     * is used for EXEM based multiple my mac.
     */

    /*
     * Create a template for VSI based VRRP MyMac table
     */
    {
        int nof_vsis;
        SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
        sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));

        template_data.data_size = sizeof(algo_dnx_l3_vrrp_tcam_key_t);
        /*
         * Since EXEM entries in the TCAM are masked out, we need to make sure they won't match for non-exem entries.
         * This will be done by invalidating the "da profiles" LSBs of that entries
         * (these LSBs are used to test whether the TCAM entry is a member of a certain VSI by comparing it to the VRID
         * bitmap inside the VSI).
         */
        {
            template_data.first_profile = ALGO_L3_VRRP_NOF_EXEM_TYPES;
            template_data.nof_profiles = dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit) - ALGO_L3_VRRP_NOF_EXEM_TYPES;
        }
        /*
         * Each VSI can point once to the MyMac prefixes table, so the maximal number of references is the VSIs number.
         */
        template_data.max_references = nof_vsis;

        sal_strncpy(template_data.name, DNX_ALGO_L3_VRRP_VSI_TCAM_TABLE_RESOURCE,
                    DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.create(unit, &template_data, NULL));

    }

    /*
     * Create a template and resource manager for  EXEM based VRRP MyMac table in case EXEM exists
     */
    {
        int capacity;
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, &capacity));

        if (capacity > 0)
        {

            /** template manager */
            {
                sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));

                template_data.data_size = sizeof(algo_dnx_l3_vrrp_tcam_key_t);
                template_data.first_profile = dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit);
                template_data.nof_profiles = dnx_data_l3.vrrp.nof_tcam_entries_get(unit) -
                    dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit);
                template_data.max_references = capacity;

                sal_strncpy(template_data.name, DNX_ALGO_L3_VRRP_EXEM_TCAM_TABLE_RESOURCE,
                            DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

                SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.create(unit, &template_data, NULL));
            }

            /** Resource manager */
            {
                sal_memset(&resource_data, 0, sizeof(resource_data));

                resource_data.first_element = 0;
                resource_data.flags = 0;
                resource_data.nof_elements = capacity;
                sal_strncpy(resource_data.name, DNX_ALGO_L3_VRRP_EXEM_STATION_ID_RESOURCE,
                            DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

                SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.create(unit, &resource_data, NULL));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Initialize the allocation manger of the ECMP tunnel priority.
 */
static shr_error_e
dnx_algo_l3_ecmp_tunnel_priority_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    int hier_iter;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.tp_modes.alloc(unit));

        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

        data.first_element = 0;
        data.nof_elements = dnx_data_l3.ecmp.nof_tunnel_priority_map_profiles_get(unit);
        data.flags = 0;
        sal_strncpy_s(data.name, DNX_ALGO_L3_ECMP_CONSISTENT_MANAGER_HANDLE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        for (hier_iter = 0; hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hier_iter++)
        {
            SHR_IF_ERR_EXIT(algo_l3_db.tp_modes.
                            l3_ecmp_tunnel_priority_modes_res_mngr.create(unit, hier_iter, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initialize all the resource managers that are needed for the ECMP  which are:
 *  Profiles management for each hierarchy (each hierarchy have 1k profiles available).
 *  consistent hashing manager - to manage the consisting hashing tables.
 *  SW state array of linked list - to track all the ECMP profiles that uses consistent members tables for quick
 *                 table address update if needed.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_l3_ecmp_init(
    int unit)
{
    int hierarchy_iter;
    int ecmp_index = 0;
    int idx;
    dnx_algo_template_create_data_t data;
    dnx_algo_res_create_data_t create_data;
    consistent_hashing_cbs_t ecmp_const_hash_cb_funcs;
    sw_state_ll_init_info_t init_info;
    uint32 consistent_hashing_manager_handle;
    uint32 nof_ecmp_banks;
    char *ecmp_profiles_handle_name;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Template mangers allocation for ECMP profiles.
     *
     * Each ECMP hierarchy have a different bank of ECMP profile to manage (1k each) so each hierarchy profile
     * resources have its own template manager instance.
     */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.alloc(unit));

    for (hierarchy_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hierarchy_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hierarchy_iter++)
    {
        sal_memset(&data, 0, sizeof(data));

        data.data_size = sizeof(dnx_l3_ecmp_profile_t);
        data.first_profile = 0;
        data.nof_profiles = dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit);
        data.max_references = dnx_data_l3.ecmp.nof_ecmp_get(unit);

        SHR_IF_ERR_EXIT(dnx_l3_ecmp_hierarchy_profile_resource_get(unit, hierarchy_iter, &ecmp_profiles_handle_name));

        sal_strncpy(data.name, ecmp_profiles_handle_name, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.create(unit, hierarchy_iter, &data, NULL));

    }

    /*
     * Init the ECMP SW state structure
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.init(unit));

    /*
     * consistent hashing manager Initialization
     */
    ecmp_const_hash_cb_funcs.nof_resources_per_profile_type_get = &ecmp_chm_get_nof_resources_from_table_size;
    ecmp_const_hash_cb_funcs.profile_type_per_nof_resources_get = &ecmp_chm_get_table_size_from_nof_resources;
    ecmp_const_hash_cb_funcs.calendar_set = &ecmp_chm_members_table_set;
    ecmp_const_hash_cb_funcs.calendar_get = &ecmp_chm_members_table_get;
    ecmp_const_hash_cb_funcs.profile_move = &ecmp_chm_update_members_table_offset;
    ecmp_const_hash_cb_funcs.profile_assign = &ecmp_chm_update_ecmp_profile_with_members_table_offset;
    ecmp_const_hash_cb_funcs.profile_offset_get = &ecmp_chm_members_table_offset_get;
    ecmp_const_hash_cb_funcs.calendar_entries_in_profile_get = &ecmp_chm_entries_in_profile_get_cb;

    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                    (unit,
                     L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit),
                     DNX_ALGO_L3_ECMP_CONSISTENT_MANAGER_HANDLE, &ecmp_const_hash_cb_funcs,
                     &consistent_hashing_manager_handle));

    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.set(unit, consistent_hashing_manager_handle));

    /*
     * Init the multihead linked list (SW state)
     */
    sal_memset(&init_info, 0, sizeof(init_info));
    init_info.max_nof_elements =
        dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit) *
        dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    init_info.expected_nof_elements =
        dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit) *
        dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    init_info.nof_heads = L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit);

    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.create_empty(unit, &init_info));

    /*
     * ALLOC MNGR - ECMP GROUP and ECMP EXTENDED GROUP
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 0;
    create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.nof_elements = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP;
    sal_strncpy(create_data.name, DNX_ALGO_L3_RES_ECMP_GROUP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.create(unit, &create_data, NULL));

    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
    create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.nof_elements = dnx_data_l3.ecmp.nof_ecmp_get(unit) - dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP;
    sal_strncpy(create_data.name, DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.create(unit, &create_data, NULL));

    /** Set the hierarchy value of all banks to default - 1st hierarchy. */
    nof_ecmp_banks =
        dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit) + dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit);
    for (idx = 0; idx < nof_ecmp_banks; idx++)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, idx, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1));
    }
    /** Allocate ECMP ID = 0 at initialization so that it cannot be allocated by user */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                  DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1,
                                                                  &ecmp_index));

    /** Allocate the max NOF tables possible which is the total members memory size divided by the smallest table size */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_fec_members_table.alloc(unit, L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit),
                                                              L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES));

    ecmp_db_info.ecmp_group_size_minus_one.alloc(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_tunnel_priority_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Allocate resource pool for VIP ECMP VIP ID
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_l3_vip_ecmp_resource_create(
    int unit)
{
    dnx_algo_res_create_data_t res_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init VIP ECMP VIP_ID resource manager
     */
    sal_memset(&res_data, 0, sizeof(res_data));
    res_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC;
    res_data.first_element = 0;
    res_data.nof_elements = dnx_data_l3.vip_ecmp.nof_vip_ecmp_get(unit);
    sal_strncpy(res_data.name, DNX_ALGO_L3_VIP_ECMP_TABLE_RESOURCE, sizeof(res_data.name));
    SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.create(unit, &res_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_l3_nof_fec_banks_get(
    int unit,
    uint32 *fec_banks)
{
    uint32 max_fec_id_value;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id_value));

    *fec_banks = ((max_fec_id_value + 1) / dnx_data_l3.fec.bank_size_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initialize all the resource managers that are needed for the ECMP  which are:
 *  Profiles management for each hierarchy (each hierarchy have 1k profiles available).
 *  consistent hashing manager - to manage the consisting hashing tables.
 *  SW state array of linked list - to track all the ECMP profiles that uses consistent members tables for quick
 *                 table address update if needed.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_l3_fec_init(
    int unit)
{
    dnx_algo_res_create_data_t create_data;
    uint32 sub_resource_id;
    uint32 nof_banks;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_nof_fec_banks_get(unit, &nof_banks));

    if (nof_banks > 0)
    {

        sal_memset(&create_data, 0, sizeof(dnx_algo_res_create_data_t));
        create_data.first_element = 0;
        create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
        create_data.nof_elements = dnx_data_l3.fec.bank_size_round_up_get(unit);
        create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_FEC;
        sal_strncpy(create_data.name, DNX_ALGO_L3_RES_FEC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.alloc(unit, nof_banks));
        for (sub_resource_id = 0; sub_resource_id < nof_banks; sub_resource_id++)
        {
            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.create
                            (unit, sub_resource_id, &create_data, NULL));
        }

        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.alloc(unit));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deinit all the algo component that were allocated in the init (dnx_algo_l3_ecmp_init)
 *  Profiles management for each hierarchy (each hierarchy have 1k profiles available).
 *  consistent hashing manager - to manage the consisting hashing tables.
 *  SW state array of linked list - to track all the ECMP profiles that uses consistent members tables for quick
 *                 table address update if needed.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_l3_ecmp_deinit(
    int unit)
{
    uint32 consistent_hashing_manager_handle;
    uint8 is_init = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinit of the consistent hashing manager.
     */
    SHR_IF_ERR_CONT(ecmp_db_info.is_init(unit, &is_init));

    if (is_init == TRUE)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, consistent_hashing_manager_handle));
    }

    /*
     * sw state module deinit is done automatically at device deinit
     */

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_init(
    int unit)
{
    uint8 is_init;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init sw state
     */
    SHR_IF_ERR_EXIT(algo_l3_db.is_init(unit, &is_init));
    if (!is_init)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.init(unit));
    }

    /*
     * Initialize the source address table template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_source_address_table_init(unit));

    /*
     * Initialize ingress mymac prefix template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ingress_mymac_prefix_entry_template_create(unit));

    /*
     * Initialize RIF resource manager.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_rif_resource_create(unit));
    /*
     * Initialize VRRP (multiple MyMac) template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_vrrp_init(unit));

    /*
     * Initialize ECMP template tables resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_init(unit));

    /** Initialise the FEC resource */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_init(unit));

    /** Initialise the EGR_POINTED resource */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_init(unit));

    /** Initialise the EGR_POINTED tables */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_tables_init(unit));

    /*
     * Initialize VIP ECMP template tables resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_vip_ecmp_resource_create(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    /*
     * Resource and template manager don't require deinitialization per instance.
     */

    /*
     * Deinitialize the source address table allocation algorithm.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_source_address_table_deinit(unit));
    /*
     * Deinitialize ECMP resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_deinit(unit));
    /*
     * Deinitialize EGR_POINTED resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_deinit(unit));
    /*
     * Deinitialize EGR_POINTED tables.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_tables_deinit(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_l3_ecmp_bitmap_create(
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
     * set grain size to be equal to the minimum number of ECMPs that are in the same hierarchy
     */
    grain_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);

    max_tag_value = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = grain_size;
    extra_create_info.max_tag_value = max_tag_value;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = create_data->first_element;
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, module_id, res_tag_bitmap,
                                               &res_tag_bitmap_create_info, &extra_create_info, nof_elements,
                                               alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_l3_ecmp_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags;
    uint32 stage;
    uint32 nof_elements;
    uint32 current_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 tag;
    uint32 ecmp_bank_size;
    uint32 first_elem_in_bank = 0;
    uint32 nof_banks;
    uint32 nof_banks_basic_mode;
    uint32 nof_banks_ext_mode;
    uint8 with_id;
    int ecmp_bank_valid = -1;
    const int bank_invalid = -1;
    int idx;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    resource_tag_bitmap_tag_info_t tag_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Minimum number of consecutive ECMPs in the same hierarchy which belong to a single bank */
    ecmp_bank_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
    /** Number of ECMP banks in basic range (16). Found by dividing the nof groups in basic range by size of bank */
    nof_banks_basic_mode = dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit);
    /** Number of ECMP banks in extended range. Found by dividing the nof groups in extended range by size of bank */
    nof_banks_ext_mode = dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    if (extra_arguments != NULL)
    {
        tag = *((uint32 *) extra_arguments);
    }
    else
    {
        tag = 0;
    }

    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);

    /*
     * Different actions need to be carried out depending on the absence/presence of the WITH_ID flag.
     * If allocating without ID, an appropriate bank for the allocation of the ECMP ID needs to be found.
     */
    if (with_id != 0)
    {
        /*
         * Define the bank ID(stage) for the given range.
         * If in extended range, need to subtract the number of IDs in the basic range in order to receive the actual value.
         */
        stage = ALGO_L3_ECMP_BANK_GET(unit, *element);
        first_elem_in_bank = ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element) ?
            ((nof_banks_basic_mode + stage) * ecmp_bank_size) : (stage * ecmp_bank_size);

        /*
         * Receive the number of allocated elements in the bank.
         */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_nof_used_elements_in_grain_get
                        (unit, module_id, res_tag_bitmap, stage, &nof_elements));
        /** Make sure that in case we are in the bank of ECMP ID 0, the number of elements will be correct */
        if (stage == 0 && !ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
        {
            nof_elements--;
        }
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, *element, &current_hierarchy));

        /*
         * If the hierarchy indicates NO_HIERARCHY that means that the bank is currently unavailable.
         */
        if (current_hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "The bank %d to which the ECMP ID %d belongs to is unavailable \n", stage,
                         *element);
        }

        /*
         * If the bank is already full, we cannot allocate any more elements in it.
         */
        if (nof_elements == dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "The bank %d to which the ECMP ID %d belongs to is full \n", stage, *element);
        }
    }
    else
    {
        /** Set the stage to an invalid bank ID (the total number of banks) - it will be redefined later. */
        stage = dnx_data_l3.ecmp.nof_ecmp_get(unit) / ecmp_bank_size;
        /** Define the nof_banks based on the range the ID belongs to - basic or extended */
        nof_banks = ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element) ? nof_banks_ext_mode : nof_banks_basic_mode;
        /*
         * Iterate over all banks for the given range (basic or extended) to find out an appropriate one for allocation.
         */
        for (idx = 0; idx < nof_banks; idx++)
        {
            /** Define the first element in the bank (0, 2048, 4096, 6144, etc.) based on the current iteration */
            first_elem_in_bank = ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element) ?
                ((nof_banks_basic_mode + idx) * ecmp_bank_size) : (idx * ecmp_bank_size);
            /*
             * Receive the number of allocated elements in the bank and the hierarchy defined for this bank.
             */
            SHR_IF_ERR_EXIT(resource_tag_bitmap_nof_used_elements_in_grain_get
                            (unit, module_id, res_tag_bitmap, idx, &nof_elements));
            /** Make sure that in case we are in the bank of ECMP ID 0, the number of elements will be correct */
            if (idx == 0 && !ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
            {
                nof_elements--;
            }
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, first_elem_in_bank, &current_hierarchy));
            /** Skip the bank if its hierarchy is marked as NO_HIERARCHY*/
            if (current_hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
            {
                continue;
            }
            /*
             * If the number of used elements in the bank is 0 mark it once as a valid ID for allocation.
             * This ID will be used in case no other bank is found that is partially taken and is with the same hier.
             */
            if (nof_elements == 0 && ecmp_bank_valid == -1)
            {
                ecmp_bank_valid = idx;
            }

            /*
             * If the hierarchy of this bank matches the current element hierarchy and is partially taken,
             * this bank is taken with preference to others.
             */
            if ((current_hierarchy == tag) && (nof_elements > 0)
                && (nof_elements < dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)))
            {
                stage = idx;
                break;
            }
        }

        /** If a valid partially taken bank was found, update the first_elem of the bank and the current element */
        if (stage != (dnx_data_l3.ecmp.nof_ecmp_get(unit) / ecmp_bank_size))
        {
            first_elem_in_bank = stage * ecmp_bank_size;
            *element = first_elem_in_bank;
        }
        /** Partially taken bank with the same hierarchy was not found but a free bank was found */
        else if (ecmp_bank_valid != bank_invalid)
        {
            nof_elements = 0;
            stage = ecmp_bank_valid;
            first_elem_in_bank = stage * ecmp_bank_size;
            *element = first_elem_in_bank;
        }
        /** If no free or valid bank was found, an error is returned. */
        else
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "No valid bank was found for allocating and ECMP ID without ID\n");
        }
    }

    /*
     * Translate the allocation flags.
     * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
     * different set of flags.
     */
    internal_flags = RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    internal_flags |= (with_id) ? RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.count = 1;
    res_tag_bitmap_alloc_info.tag = tag;

    /*
     * If adding the first element in a stage we need to carry out additional configuration in cases:which is part of the extended range
     *      * ECMP ID is in the extended range;
     *      * ECMP ID is in the first bank of the basic range resource.
     */
    if (nof_elements == 0)
    {
        /*
         * In case the ECMP ID is in the first bank of the basic range, the ECMP ID 0 that is allocated on init
         * may be in an incorrect hierarchy. We need to update the hierarchy of element 0.
         */
        if (stage == 0 && !ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
        {
            int ecmp_init_id = 0;
            resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
            uint32 internal_flags_init = RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
            sal_memset(&res_tag_bitmap_free_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
            res_tag_bitmap_free_info.count = 1;
            SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free
                            (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, ecmp_init_id));
            res_tag_bitmap_alloc_info.flags = internal_flags_init;
            SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc
                            (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, &ecmp_init_id));
        }
        /*
         * In case the ECMP ID is in the extended range. which uses the same memory as the extended range of ECMP
         * groups, we need to specify to the failover that we reserve these banks so that they are not overwritten.
         */
        if (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
        {
            SHR_IF_ERR_EXIT(dnx_algo_failover_fec_bank_for_ecmp_extended_allocate(unit, stage));
        }

        tag_info.tag = tag;
        tag_info.force_tag = FALSE;
        tag_info.element = first_elem_in_bank;
        tag_info.nof_elements = ecmp_bank_size;
        /** Set the hierarchy for this bank in case we are allocating the first element in it. */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap, &tag_info));
    }

    res_tag_bitmap_alloc_info.flags = internal_flags;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

    /*
     * Update sw state ecmp_nof_used_elements_per_grain variable.
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.inc(unit,
                                                                      (*element /
                                                                       dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)),
                                                                      res_tag_bitmap_alloc_info.count));
    SHR_IF_ERR_EXIT(ecmp_db_info.
                    ecmp_hierarchy.set(unit, (*element / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)), tag));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    uint32 stage;
    uint32 nof_elements;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
    uint32 tag;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Define the bank ID for the given range.
     * If in extended range, need to subtract the number of IDs in the basic range in order to receive the actual value.
     */
    stage = ALGO_L3_ECMP_BANK_GET(unit, element);

    /**
     * Call the main function to clear the element from the bitmap
     */
    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.count = 1;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));

    /*
     * Update sw state ecmp_nof_used_elements_per_grain variable.
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.dec(unit,
                                                                      (element /
                                                                       dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)),
                                                                      res_tag_bitmap_free_info.count));
    /**
     * Receive the number of allocated elements for this stage.
     */
    SHR_IF_ERR_EXIT(resource_tag_bitmap_nof_used_elements_in_grain_get
                    (unit, module_id, res_tag_bitmap, stage, &nof_elements));

    /*
     * If the ecmp group belongs to the extended range of entries (32k to 40k) and is the last one in the bank,
     * then we need to free the shared bank of memory between the failover and the ECMP.
     */
    if (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, element) && nof_elements == 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_failover_fec_bank_for_ecmp_extended_free(unit, stage));
    }

    /*
     * If we are deleting an element from the first bank in the basic range,
     * we need to verify if the bank doesn't have only ID 0 allocated in it.
     */
    if (!ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, element) && (stage == 0))
    {
        nof_elements--;
        /*
         * If only ID 0 is allocated in bank ID 0,
         * then we need to reset the hierarchy of the first bank to its initial value.
         */
        if (nof_elements == 0)
        {
            int ecmp_init_id = 0;
            uint32 internal_flags;
            resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
            tag = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
            SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free(unit,
                                                              module_id, res_tag_bitmap, res_tag_bitmap_free_info,
                                                              ecmp_init_id));
            internal_flags = RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
            sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
            res_tag_bitmap_alloc_info.flags = internal_flags;
            res_tag_bitmap_alloc_info.count = 1;
            res_tag_bitmap_alloc_info.tag = tag;
            SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc
                            (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, &ecmp_init_id));
        }
    }
    /** Set the hierarchy value to default in case deleting the last element in the bank. */
    if (nof_elements == 0)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, element / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit),
                                                        DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_hierarchy_get(
    int unit,
    int ecmp_index,
    uint32 *hierarchy)
{
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Receive the value of the bitmap handle (algo_instance_id).
     * It is saved in the SW state and is received based on whether the ECMP ID is in basic or extended range.
     * If the ECMP index is in extended mode, then we need to update the ecmp_index
     * to be in the 0 - 8k range and not 32k - 40k range.
     */

    bank_id = ecmp_index / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.get(unit, bank_id, hierarchy));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_group_is_alone_in_bank(
    int unit,
    int ecmp_id,
    int *bank_id,
    int *is_alone_in_bank)
{
    uint32 nof_elements = 0;

    SHR_FUNC_INIT_VARS(unit);

    *is_alone_in_bank = 0;

    /*
     * Define the bank ID for the given range.
     * If in extended range, need to subtract the number of IDs in the basic range in order to receive the actual value.
     *
     * Receive the value of the bitmap handle (algo_instance_id).
     * It is saved in the SW state and is received based on whether the ECMP ID is in basic or extended range.
     */

    *bank_id = ecmp_id / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
    /**
     * Receive the current number of elements for the given bank
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.get(unit, *bank_id, &nof_elements));
    if (!ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_id) && (*bank_id == 0))
    {
        nof_elements--;
    }
    /**
     * If there is only a single element in the bank, return indication that the element is first in bank
     */
    if (nof_elements == 1)
    {
        *is_alone_in_bank = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_is_allocated(
    int unit,
    int ecmp_index,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Receive indication whether the group is allocated.
     * Based on which range the ECMP group ID belongs to, a different resource will be accessed.
     */
    SHR_IF_ERR_EXIT(ALGO_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, is_allocated(unit, ecmp_index, is_allocated)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_allocate(
    int unit,
    int *ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags,
    uint32 hierarchy)
{
    uint32 alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = (_SHR_IS_FLAG_SET(ecmp_flags, BCM_L3_WITH_ID)) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    /*
     * Allocate the group in the needed resource.
     * The resource is selected based on which range the ECMP ID belongs to.
     */
    if (!_SHR_IS_FLAG_SET(ecmp_flags, BCM_L3_WITH_ID))
    {
        if (_SHR_IS_FLAG_SET(ecmp_group_flags, BCM_L3_ECMP_EXTENDED))
        {
            *ecmp_index = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            ecmp_extended_res_manager.allocate_single(unit, alloc_flags, &hierarchy, ecmp_index));
        }
        else
        {
            *ecmp_index = 0;
            SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.allocate_single(unit, alloc_flags, &hierarchy, ecmp_index));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ALGO_L3_ECMP_GROUP_RESOURCE_CB
                        (unit, *ecmp_index, allocate_single(unit, alloc_flags, &hierarchy, ecmp_index)));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_deallocate(
    int unit,
    int ecmp_index)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deallocate the ECMP group from the needed resource.
     * The resource is selected based on which range the ECMP ID belongs to.
     */
    SHR_IF_ERR_EXIT(ALGO_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, free_single(unit, ecmp_index)));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_extended_bank_for_failover_allocate(
    int unit,
    int bank_id)
{
    uint32 ecmp_bank_size;
    uint32 nof_elements;

    SHR_FUNC_INIT_VARS(unit);

    ecmp_bank_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);

    /*
     *  Modify the bank ID so that the valid banks for extended range will be from 16 to 19
     */
    bank_id = bank_id + (dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit) / ecmp_bank_size);

    /** Receive number of elements allocated in the Extended range resource */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.get(unit, bank_id, &nof_elements));
    /** If the bank is not empty regarding ECMP, FEC allocation cannot reserve this bank. */
    if (nof_elements != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Cannot reserve shared bank %d for the use of FEC protection as it not empty\n",
                     bank_id);
    }
    /*
     *  marked with NO_HIERARCHY indicating unavailable bank for allocation.
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, bank_id, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_extended_bank_for_failover_destroy(
    int unit,
    int bank_id)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Modify the bank ID so that the valid banks for extended range will be from 16 to 19
     */
    bank_id = bank_id + (dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit) /
                         dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit));

    /** Mark the bank as available for allocation. */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, bank_id, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
algo_l3_fec_hierarchy_stage_map_get(
    int unit,
    uint32 fec_index,
    uint32 *hierarchy_p)
{
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);

    if (fec_index < dnx_data_l3.fec.nof_fecs_get(unit))
    {
        bank_id = DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
        SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.fec_hierarchy_map.get(unit, bank_id, hierarchy_p));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "fec_index(%x) is out of nof_fecs range(%x)\n", fec_index,
                     dnx_data_l3.fec.nof_fecs_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    int grain_size = 2;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = grain_size;
    extra_create_info.max_tag_value = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

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
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags = 0;
    algo_dnx_l3_fec_data_t fec_alloc_data;
    uint32 grain_size = 2;
    uint8 with_id;
    uint8 alloc_simulation;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    SHR_NULL_CHECK(extra_arguments, _SHR_E_PARAM, "extra_arguments");

    fec_alloc_data = *((algo_dnx_l3_fec_data_t *) extra_arguments);

    /*
     * Translate the allocation flags.
     * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the resource_tag_bitmap_alloc_info_t uses a
     * different set of flags.
     */
    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);
    alloc_simulation = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_SIMULATION);
    internal_flags |= (with_id) ? RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;
    internal_flags |= (alloc_simulation) ? RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    /*
     * If allocating in bank ID 1 and without ID,
     * then the ALLOC_IN_RANGE flag needs to be set in order to be able to allocate without ID
     * in the range between the end of the ECMP range and the end of the 2nd FEC bank (40960 - 52430).
     */
    if (!with_id && (fec_alloc_data.sub_resource_index == DNX_ALGO_L3_SHARED_RANGE_FEC_BANK))
    {
        /** range_start is the end of the ECMP range */
        res_tag_bitmap_alloc_info.range_start =
            dnx_data_l3.ecmp.total_nof_ecmp_get(unit) - dnx_data_l3.fec.bank_size_round_up_get(unit);
        /** range_end is the start of the 3rd bank (bank ID 2)*/
        res_tag_bitmap_alloc_info.range_end = dnx_data_l3.fec.bank_size_get(unit);
        internal_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    }

    res_tag_bitmap_alloc_info.count = 1;
    if (fec_alloc_data.super_fec_type_tag == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || fec_alloc_data.super_fec_type_tag == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        res_tag_bitmap_alloc_info.count = grain_size;
        res_tag_bitmap_alloc_info.align = grain_size;
        /** Flag guarantees that in case of protection, the first entry in the pair will be even. */
        internal_flags |= RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
    }

    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.tag = fec_alloc_data.super_fec_type_tag;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
    resource_tag_bitmap_tag_info_t tag_info;
    uint32 super_fec_type;
    uint32 internal_flags = 0;
    uint32 grain_size = 2;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Call the main function to clear the element from the bitmap
     */
    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.count = 1;

    sal_memset(&tag_info, 0, sizeof(tag_info));
    tag_info.element = element;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

    super_fec_type = tag_info.tag;

    /*
     * If we are freeing a FEC with protection, then we free the pair it belongs to
     * instead of the two elements separately.
     */
    if (super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        res_tag_bitmap_free_info.count = grain_size;
        res_tag_bitmap_free_info.align = grain_size;
        /** Flag guarantees that in case of protection, the first entry in the pair will be even. */
        internal_flags |= RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
    }
    res_tag_bitmap_free_info.flags = internal_flags;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the super FEC type of the FEC pair it is being allocated in.
 *  \param [in] unit - The unit number.
 *  \param [in] fec_index - The new FEC ID
 *  \param [in] fec_resource_type - The super FEC type of the new FEC
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \see
 *  dnx_algo_l3_fec_allocate
 */
static shr_error_e
dnx_algo_l3_fec_allocation_type_verify(
    int unit,
    int fec_index,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type)
{
    dbal_enum_value_field_fec_resource_type_e super_fec_type;
    uint32 grain_size = 2;
    uint32 sub_resource_id;
    uint8 is_allocated = 0;
    int fec_in_pair;
    SHR_FUNC_INIT_VARS(unit);

    if (L3_FEC_ID_IS_ODD_FEC_ID(fec_index))
    {
        fec_in_pair = L3_FEC_ID_TO_EVEN_FEC_ID(fec_index);
    }
    else
    {
        fec_in_pair = L3_FEC_ID_TO_ODD_FEC_ID(fec_index);
    }

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    /** Receive indication whether the entry is allocated. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.is_allocated
                    (unit, sub_resource_id, fec_in_pair - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id),
                     &is_allocated));
    if (is_allocated == TRUE)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.get
                        (unit, sub_resource_id,
                         (fec_index - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id)) / grain_size,
                         &super_fec_type));
        if (super_fec_type != fec_resource_type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Super FEC type for pair of FEC value %u is already %u, but user provided %u\n",
                         fec_index, super_fec_type, fec_resource_type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*See l3_fec.h for prototype define*/
shr_error_e
dnx_algo_l3_fec_allocation_info_get(
    int unit,
    int hierarchy,
    uint32 *range_start,
    uint32 *range_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((hierarchy < DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1)
        || (hierarchy > DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hierarchy %d is out range, must be between 0-2 .\n", hierarchy);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.fec_id_range_start.get(unit, hierarchy, range_start));

    SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.fec_id_range_size.get(unit, hierarchy, range_size));

    if (*range_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "There isn't any allocation for the requested hierarchy %d .\n", hierarchy);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type)
{
    uint32 alloc_flags = 0;
    /** Size in number of entries of the current FEC bank */
    uint32 fec_bank_size = dnx_data_l3.fec.bank_size_get(unit);
    uint32 sub_resource_id;
    uint32 current_hierarchy;
    uint32 subr_id;
    /** total number of sub-resources in the FEC resource */
    uint32 nof_sub_resources;
    /** number of free elements per given bank */
    int nof_free_elements = 0;
    int rv = BCM_E_NONE;
    /** data for a cluster which will be updated */
    algo_dnx_l3_fec_data_t fec_alloc_data;

    SHR_FUNC_INIT_VARS(unit);

    /** Use field logical_start_address as indication whether the cluster is valid - if it equals -1, then it is not */
    SHR_IF_ERR_EXIT(dnx_algo_l3_nof_fec_banks_get(unit, &nof_sub_resources));

    alloc_flags = (_SHR_IS_FLAG_SET(flags, DNX_ALGO_L3_FEC_WITH_ID)) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;

    /*
     * Based on the WITH_ID flag different set of actions are executed:
     * If allocating without ID then we need to find an empty sub-resource or one configured for the current hierarchy.
     * If allocating WITH_ID, check that there are empty slots in the sub-resource and that the hierarchies match.
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_L3_FEC_WITH_ID))
    {
        /*
         * Receive the ID of the sub-resource to which the FEC belongs -
         * divide the physical FEC to the number of physical FECs per sub-resource.
         */
        sub_resource_id =
            DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, *fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_type_verify(unit, *fec_index, fec_resource_type));

        /*
         * check if current element hierarchy matches already defined hierarchy.
         * If it doesn't then an error will be returned.
         */
        SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.
                        fec_hierarchy_map.get(unit, sub_resource_id, &current_hierarchy));

        if (hierarchy != current_hierarchy)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected hierarchy for bank %d is %d, instead received %d\n",
                         sub_resource_id, current_hierarchy + 1, hierarchy + 1);
        }

        /*
         * FEC index is received in the range of 0 - 768K. It needs to be mapped to an ID in the sub-resource.
         * This is calculated by subtracting the starting FEC ID of this bank.
         */
        *fec_index = *fec_index - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

    }
    else
    {
        uint32 range_start, range_end, range_size;

        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, hierarchy, &range_start, &range_size));

        range_end = (range_start + range_size) / dnx_data_l3.fec.bank_size_get(unit);
        /** Limit the without ID range to its valid range in case the allocation starts outside that range */
        range_start =
            UTILEX_MAX(range_start / dnx_data_l3.fec.bank_size_get(unit),
                       dnx_data_l3.fec.first_bank_without_id_alloc_get(unit));
        /*
         * Find a valid sub-resource ID to which to add the new FEC. Compare free slots and defined hierarchy.
         */
        for (subr_id = range_start; subr_id < range_end; subr_id++)
        {
            fec_bank_size = MDB_CALC_FEC_BANK_SIZE(unit, subr_id);

            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                            fec_res_manager.nof_free_elements_get(unit, subr_id, &nof_free_elements));

            /** Limit the size of resource FEC bank with the gap to the MAX bank size possible */
            nof_free_elements -= dnx_data_l3.fec.bank_size_round_up_get(unit) - fec_bank_size;

            /*
             * If the sub-resource have the same hierarchy and room for the current allocation
             */
            if (nof_free_elements > 0)
            {
                /** This sub-resource is already in use for this hierarchy. */
                if (nof_free_elements < fec_bank_size / 2)
                {
                    fec_alloc_data.super_fec_type_tag = fec_resource_type;
                    fec_alloc_data.sub_resource_index = subr_id;
                    /*
                     * Need to find an empty grain in the sub-resource. To do that we will use the allocation itself
                     * using the ALLOCATE_SIMULATION flag.
                     * If it passes then the sub-resource is valid for this allocation.
                     */
                    rv = algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.allocate_single
                        (unit, subr_id, (alloc_flags | DNX_ALGO_RES_ALLOCATE_SIMULATION), &fec_alloc_data, fec_index);
                    if (rv == BCM_E_NONE)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }

            }
        }

        /*
         * If we found a partially taken sub-resource for this hierarchy,
         * then it is the one which is chosen for the current allocation.
         * If we didn't find a partially taken sub-resource but we found a free sub-resource
         * (subres_found is not default value), then we will allocate this sub-resource for the use of the current hier.
         * If we didn't find a free resource or a partially taken one, an error will be returned.
         */
        if (subr_id < nof_sub_resources)
        {
            sub_resource_id = subr_id;
        }
        else
        {
            /*
             * If the subr_id variable equals the number of sub-resources
             * then we have reached to the end of the loop and we didn't find a valid sub-resource.
             */
            SHR_ERR_EXIT(_SHR_E_FULL, "No valid bank for without ID allocation for hierarchy %d found\n",
                         hierarchy + 1);
        }
    }

    fec_alloc_data.super_fec_type_tag = fec_resource_type;
    fec_alloc_data.sub_resource_index = sub_resource_id;
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                    fec_res_manager.allocate_single(unit, sub_resource_id, alloc_flags, &fec_alloc_data, fec_index));
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.
                    fec_resource_type.set(unit, sub_resource_id, (*fec_index / 2), fec_resource_type));

    *fec_index = *fec_index + MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_deallocate(
    int unit,
    int fec_index)
{
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

    fec_index = fec_index - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

    /** Deallocate the FEC entry from the resource. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.free_single(unit, sub_resource_id, fec_index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_is_allocated(
    int unit,
    int fec_index,
    uint8 *is_allocated)
{
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

    fec_index -= MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

    /** Receive indication whether the entry is allocated. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.is_allocated
                    (unit, sub_resource_id, fec_index, is_allocated));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_super_fec_type_get(
    int unit,
    int fec_index,
    dbal_enum_value_field_fec_resource_type_e * super_fec_type)
{
    uint32 sub_resource_id;
    uint32 grain_size = 2;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

    fec_index -= MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.get
                    (unit, sub_resource_id, fec_index / grain_size, super_fec_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_protection_state_get(
    int unit,
    int fec_index,
    uint8 *is_protected)
{
    dbal_enum_value_field_fec_resource_type_e super_fec_type;
    SHR_FUNC_INIT_VARS(unit);

    fec_index = BCM_L3_ITF_VAL_GET(fec_index);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_super_fec_type_get(unit, fec_index, &super_fec_type));

    /*
     * If the supr FEC type is one of the protected types then this FEC is part of a protection pair.
     */
    if (super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        *is_protected = TRUE;
    }
    else
    {
        *is_protected = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** Refer to .h file for more information */
shr_error_e
dnx_algo_l3_fec_allocation_get(
    int unit,
    int nof_members,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type,
    int *start_if_id)
{
    int subr_id;
    uint32 range_start;
    uint32 range_size;
    uint32 fec_bank_size;
    int nof_free_elements = 0;
    int next_fec = 0;
    int prev_fec;
    uint8 with_prot = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, hierarchy, &range_start, &range_size));
    /** Limit the without ID range to its valid range in case the allocation starts outside that range */
    range_start =
        UTILEX_MAX(range_start / dnx_data_l3.fec.bank_size_get(unit),
                   dnx_data_l3.fec.first_bank_without_id_alloc_get(unit));
    range_size /= dnx_data_l3.fec.bank_size_get(unit);
    *start_if_id = -1;
    if (fec_resource_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || fec_resource_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        nof_members = nof_members << 1;
        with_prot = 1;
    }
    for (subr_id = range_start; subr_id < (range_start + range_size); subr_id++)
    {
        fec_bank_size = MDB_CALC_FEC_BANK_SIZE(unit, subr_id);
        prev_fec = -1;
        next_fec = 0;
        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                        fec_res_manager.nof_free_elements_get(unit, subr_id, &nof_free_elements));

        /** Limit the size of resource FEC bank with the gap to the MAX bank size possible */
        nof_free_elements -= dnx_data_l3.fec.bank_size_round_up_get(unit) - fec_bank_size;

        if (nof_free_elements >= nof_members)
        {
            for (;; next_fec++)
            {
                uint32 range_end = 0;
                SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                                fec_res_manager.get_next(unit, subr_id, &next_fec));
                range_end = (next_fec == DNX_ALGO_RES_ILLEGAL_ELEMENT) ? fec_bank_size : next_fec;

                if (with_prot && ((range_end & 0) - (prev_fec + 1) >= nof_members))
                {
                    *start_if_id = (prev_fec + 2) & 0;
                    break;
                }
                else if (with_prot == 0 && (range_end - (prev_fec + 1) >= nof_members))
                {
                    *start_if_id = prev_fec + 1;
                    break;
                }
                if (next_fec == DNX_ALGO_RES_ILLEGAL_ELEMENT)
                {
                    break;
                }
                prev_fec = next_fec;
            }
            if (*start_if_id != -1)
            {
                break;
            }
        }
    }

    if (*start_if_id != -1)
    {
        *start_if_id = MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, subr_id) + *start_if_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not find a valid range of %d FECs for allocation\n", nof_members);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
algo_l3_fec_mdb_table_resources_get(
    int unit,
    uint32 hierarchy,
    int *nof_clusters,
    mdb_cluster_alloc_info_t * clusters)
{
    mdb_cluster_alloc_info_t clusters_found[MDB_MAX_NOF_CLUSTERS];
    dbal_physical_tables_e dbal_physical_fec_table_id;
    int idx;
    int cl_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Choose the physical FEC table according to the input flags. */
    if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2)
    {
        dbal_physical_fec_table_id = DBAL_PHYSICAL_TABLE_FEC_2;
    }
    else if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3)
    {
        dbal_physical_fec_table_id = DBAL_PHYSICAL_TABLE_FEC_3;
    }
    else
    {
        dbal_physical_fec_table_id = DBAL_PHYSICAL_TABLE_FEC_1;
    }

    SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_physical_fec_table_id, clusters_found, nof_clusters));

    /** Iterate over all clusters for the current FEC table */
    for (idx = 0; idx < *nof_clusters; idx++)
    {
        /** If the table Id doesn't match, then we skip this iteration */
        if (clusters_found[idx].dbal_physical_table_id != dbal_physical_fec_table_id)
        {
            continue;
        }
        sal_memcpy(&clusters[cl_id], &clusters_found[idx], sizeof(mdb_cluster_alloc_info_t));
        cl_id++;
    }

    *nof_clusters = cl_id;
exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_is_allocated(
    int unit,
    int index,
    uint8 *is_allocated)
{
    int hierarchy_iter;
    int ref_count = 0;
    dnx_l3_ecmp_profile_t ecmp_profile;

    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = FALSE;
    /** Iterate over the three hierarchies to find where we have references to this profile. */
    for (hierarchy_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hierarchy_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hierarchy_iter++)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                        (unit, hierarchy_iter, index, &ref_count, &ecmp_profile));
        /** If there are references to this profile data, then it is considered as allocated. */
        if (ref_count > 0)
        {
            *is_allocated = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_allocate(
    int unit,
    int *profile_index)
{
    uint8 first_reference;

    dnx_l3_ecmp_profile_t ecmp_profile;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;

    SHR_FUNC_INIT_VARS(unit);
    /** Set some unique values for the ECMP profile so that the template manager doesn't consider it as existent. */
    sal_memset(&ecmp_profile, 0, sizeof(dnx_l3_ecmp_profile_t));
    ecmp_profile.group_size = *profile_index % dnx_data_l3.ecmp.max_group_size_get(unit);
    ecmp_profile.members_table_id = *profile_index;

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.allocate_single
                    (unit, hierarchy, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &ecmp_profile, NULL,
                     profile_index, &first_reference));
exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_free(
    int unit,
    int profile_index)
{
    uint8 last_reference;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.free_single
                    (unit, hierarchy, profile_index, &last_reference));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_get_next(
    int unit,
    uint32 hierarchy,
    int *profile_index)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.get_next(unit, hierarchy, profile_index));
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
