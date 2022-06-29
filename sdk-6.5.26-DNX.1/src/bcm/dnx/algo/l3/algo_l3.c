/** \file algo_l3.c
 *
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/**
* INCLUDE FILES:
* {
*/
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

/**
 * }
 */
/**
* Defines:
*/
#define L3_MYMAC_TABLE_SIZE             64

/**
 * }
 */
/**
* Macro:
* {
*/
/** Receive the bank ID to which the ECMP index belongs */
#define ALGO_L3_ECMP_BANK_GET(unit, ecmp_index) (DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ? \
        ((ecmp_index - dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit)) / dnx_data_l3.ecmp.ecmp_bank_size_get(unit)) : \
        (ecmp_index / dnx_data_l3.ecmp.ecmp_bank_size_get(unit)))
/** Returns the first element in the specified bank of the extended ECMP range */
#define ALGO_L3_ECMP_EXTENDED_GROUP_GET_FIRST_ELEMENT_IN_BANK(unit, bank)\
    (dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit) + (bank * dnx_data_l3.ecmp.ecmp_bank_size_get(unit)))
/*
 * Returns the logical start address given the bank ID.
 * As the logical address is counted in FECs (not super FECS) the address is the NOF banks FEC capacity multiply the bank ID and each
 * big bank till this bank adds another two FECs.
 */
#define ALGO_L3_GET_START_LOGICAL_ADDRESS_FROM_BANK_ID(unit,bank_id) ((bank_id * dnx_data_l3.fec.bank_size_get(unit)) + 2 * ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(bank_id))

/** Retrieve the next even FEC ID */
#define ALGO_L3_GET_NEXT_EVEN_ID(index) (index + (index & 0x1))
#define ALGO_L3_GET_PREV_EVEN_ID(index) (index - (index & 0x1))

/** The MACROs are used in the FEC algorithm that search for a free number of FECs.
 * They are used to calculate a free range between two allocated elements considering the protection.
 */
#define ALGO_L3_GET_NEXT_FEC_ID(protected, index) ((protected) ? ALGO_L3_GET_NEXT_EVEN_ID(index) : index)
#define ALGO_L3_GET_PREV_FEC_ID(protected, index) ((protected) ? ALGO_L3_GET_PREV_EVEN_ID(index) : index)

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
 *************/
/*
 * {
 */

/*******************************************
 * FUNCTIONS related to EGR_POINTED        *
 *******************************************/
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
    uint32 super_fec_type_tag;
    /** Id of the sub-resource in the FEC resource (bank_id) */
    uint32 sub_resource_index;
} algo_dnx_l3_fec_data_t;

void
dnx_algo_l3_print_vrrp_entry_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    algo_dnx_l3_vrrp_tcam_key_t *tcam_info = (algo_dnx_l3_vrrp_tcam_key_t *) data;
#endif

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
#ifdef DNX_SW_STATE_DIAGNOSTIC
    dnx_l3_ecmp_profile_t *ecmp_profile = (dnx_l3_ecmp_profile_t *) data;
#endif

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Member table ID",
                                   ecmp_profile->consistent_hashing_id, "Member base", "0x%02X");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Group size", ecmp_profile->group_size,
                                   "Size of ECMP group", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Statistics object ID",
                                   ecmp_profile->statistic_object_id, "Stat object ID", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Statistics object profile",
                                   ecmp_profile->statistic_object_profile, "Stat object profile", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Protection", ecmp_profile->protection_flag,
                                   "Protection Flag", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "Is using UDEP",
                                   ecmp_profile->user_defined_ecmp_profile, "User-defined profile is used", NULL);
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
    dnx_algo_lif_local_outlif_resource_extra_info_t init_info = { 0 };

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

/**
 * \brief
 *   Allocate the resources for User-defined ECMP profile
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_l3_ecmp_user_profile_create(
    int unit)
{
    dnx_algo_res_create_data_t create_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 1;
    create_data.flags = 0;
    /*
     * we define the nof elements to be nof elements minus one because we trim the first allocation index,
     * this leaves us with one profile less.
     */
    create_data.nof_elements = L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit);
    sal_strncpy(create_data.name, DNX_ALGO_L3_ECMP_USER_PROFILE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_res_mngr.create(unit, &create_data, NULL));
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.alloc(unit, L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit) + 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    dbal_result_type_t rif_result_type,
    dbal_tables_e table_id)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = rif_result_type;
    outlif_info.logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;

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
    dbal_result_type_t rif_result_type,
    uint32 fixed_entry_size)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RIF_BASIC;
    outlif_info.dbal_result_type = rif_result_type;
    outlif_info.logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.allocate_single
                    (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &outlif_info, &rif_id));

    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                        (unit, &outlif_info, rif_id, DBAL_TABLE_EEDB_RIF_BASIC, rif_result_type, fixed_entry_size));

        /*
         * Update the egress global lif counter, because we're adding this rif to the GLEM.
         */
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, 1));
    }

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

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RIF_BASIC;
    outlif_info.logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_rif_intf_to_hw_resources(unit, rif_id, &rif_hw_resources));
    outlif_info.dbal_result_type = rif_hw_resources.outlif_dbal_result_type;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, &outlif_info, &rif_size));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.free_several(unit, rif_size, rif_id, NULL));

    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, rif_id));

        /*
         * Update the egress global lif counter, because we're removing this rif from the GLEM.
         */
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, -1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Resource manager for RIF allocation.
 * The manager is used to check if a RIF has been created.
 *
 * \param [in] unit - the unit number
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_algo_l3_intf_rif_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t resource_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&resource_data, 0, sizeof(resource_data));

    resource_data.first_element = L3_INTF_FIRST_NON_DEFAULT_RIF_ID;
    resource_data.flags = 0;
    resource_data.nof_elements = dnx_data_l3.rif.nof_rifs_get(unit) - L3_INTF_FIRST_NON_DEFAULT_RIF_ID;
    sal_strncpy(resource_data.name, DNX_ALGO_L3_INTF_RIF_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.intf_rif_res_mngr.create(unit, &resource_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_vrrp_capacity_get(
    int unit,
    int *capacity)
{
    const dnx_data_l3_vrrp_data_bases_t *info;
    int database_iter, zero_capacity = 0;
    int capacity_temp[DNX_DATA_MAX_L3_VRRP_NOF_DBS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    *capacity = UTILEX_I32_MAX;

    /** A list with databases*/
    info = dnx_data_l3.vrrp.data_bases_get(unit);

    for (database_iter = 0; database_iter < dnx_data_l3.vrrp.nof_dbs_get(unit); database_iter++)
    {
        /** Get database capacity*/
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, info->data_base[database_iter], &capacity_temp[database_iter]));
        /** Check if there is a database with 0 capacity */
        if (capacity_temp[database_iter] == 0)
        {
            zero_capacity++;
        }
        else
        {
            /** If all databases have some capacity, we can fill them till the smallest one will get full
             * so we are taking the MIN value of the capacities*/
            *capacity = UTILEX_MIN(*capacity, capacity_temp[database_iter]);
        }
    }

    /** In case all databases have 0 capacity*/
    if (zero_capacity == dnx_data_l3.vrrp.nof_dbs_get(unit))
    {
        *capacity = 0;
    }

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
            template_data.first_profile = dnx_data_l3.vrrp.nof_tcam_entries_used_by_exem_get(unit);
            template_data.nof_profiles =
                dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit) -
                dnx_data_l3.vrrp.nof_tcam_entries_used_by_exem_get(unit);
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
        int capacity = 0;

        SHR_IF_ERR_EXIT(dnx_algo_l3_vrrp_capacity_get(unit, &capacity));

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
    dnx_algo_template_create_data_t data;
    dnx_algo_res_create_data_t create_data;
    consistent_hashing_cbs_t ecmp_const_hash_cb_funcs;
    sw_state_ll_init_info_t init_info;
    uint32 consistent_hashing_manager_handle;
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
                     dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit),
                     L3_ECMP_NOF_CONSISTENT_BANKS(unit),
                     DNX_ALGO_L3_ECMP_CONSISTENT_MANAGER_HANDLE, &ecmp_const_hash_cb_funcs,
                     &consistent_hashing_manager_handle));

    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.set(unit, consistent_hashing_manager_handle));

    /*
     * Init the multihead linked list (SW state).
     */
    sal_memset(&init_info, 0, sizeof(init_info));
    init_info.max_nof_elements =
        dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit) *
        dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit) + L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit);

    init_info.nof_heads = L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit);

    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.create_empty(unit, &init_info));

    /*
     * ALLOC MNGR - ECMP GROUP
     */
    {
        resource_tag_bitmap_utils_extra_arg_create_info_t extra_create_info;

        sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
        extra_create_info.grains_size[0] = dnx_data_l3.ecmp.ecmp_bank_size_get(unit);
        extra_create_info.max_tags_value[0] = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit) - 1;

        sal_memset(&create_data, 0, sizeof(create_data));
        create_data.first_element = 1;
        create_data.flags =
            DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM | RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS |
            RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;
        /*
         * we define the nof elements to be nof elements minus one because we trim the first allocation index,
         * this leaves us with one less ECMPs in basic mode
         */
        create_data.nof_elements = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit) - 1;
        create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP;
        sal_strncpy(create_data.name, DNX_ALGO_L3_RES_ECMP_GROUP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.create(unit, &create_data, &extra_create_info));
    }
    /*
     * ALLOC MNGR - ECMP EXTENDED GROUP
     */
    {
        resource_tag_bitmap_utils_extra_arg_create_info_t extra_create_info;

        sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
        extra_create_info.grains_size[0] = dnx_data_l3.ecmp.ecmp_bank_size_get(unit);
        /** The tag value is added a 'no hierarchy' tag that represents that this grain is unavailable for allocation */
        extra_create_info.max_tags_value[0] = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

        sal_memset(&create_data, 0, sizeof(create_data));
        create_data.first_element = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
        create_data.flags =
            DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM | RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;
        create_data.nof_elements = dnx_data_l3.ecmp.nof_ecmp_get(unit) - dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
        create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP;
        sal_strncpy(create_data.name, DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.create(unit, &create_data, &extra_create_info));
    }

    /** Allocate the max NOF tables possible which is the total members memory size divided by the smallest table size */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_fec_members_table.alloc(unit, L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit),
                                                              L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES));

    ecmp_db_info.ecmp_group_size_minus_one.alloc(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_create(unit));
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

/** Remap the structure from dnx_l3_ecmp_user_profile_t to l3_ecmp_user_profile_t.*/
static void
dnx_algo_l3_ecmp_user_profile_sw_structure_get(
    dnx_l3_ecmp_user_profile_t * ecmp_profile_src,
    l3_ecmp_user_profile_t * ecmp_profile_dst)
{
    ecmp_profile_dst->ecmp_mode = ecmp_profile_src->ecmp_mode;
    ecmp_profile_dst->consistent_members_table_addr = ecmp_profile_src->consistent_members_table_addr;
    ecmp_profile_dst->tunnel_priority_mode = ecmp_profile_src->tunnel_priority_mode;
    ecmp_profile_dst->tunnel_priority_map_profile = ecmp_profile_src->tunnel_priority_map_profile;
    ecmp_profile_dst->rpf_mode = ecmp_profile_src->rpf_mode;
    ecmp_profile_dst->hierarchy = ecmp_profile_src->hierarchy;
    ecmp_profile_dst->chm_alloc_state = ecmp_profile_src->chm_alloc_state;
    ecmp_profile_dst->group_size = ecmp_profile_src->group_size;
}

/** Remap the structure from l3_ecmp_user_profile_t to dnx_l3_ecmp_user_profile_t.*/
static void
dnx_algo_l3_ecmp_user_profile_hw_structure_get(
    l3_ecmp_user_profile_t * ecmp_profile_src,
    dnx_l3_ecmp_user_profile_t * ecmp_profile_dst)
{
    sal_memset(ecmp_profile_dst, 0, sizeof(dnx_l3_ecmp_user_profile_t));
    ecmp_profile_dst->ecmp_mode = ecmp_profile_src->ecmp_mode;
    ecmp_profile_dst->consistent_members_table_addr = ecmp_profile_src->consistent_members_table_addr;
    ecmp_profile_dst->tunnel_priority_mode = ecmp_profile_src->tunnel_priority_mode;
    ecmp_profile_dst->tunnel_priority_map_profile = ecmp_profile_src->tunnel_priority_map_profile;
    ecmp_profile_dst->rpf_mode = ecmp_profile_src->rpf_mode;
    ecmp_profile_dst->hierarchy = ecmp_profile_src->hierarchy;
    ecmp_profile_dst->chm_alloc_state = ecmp_profile_src->chm_alloc_state;
    ecmp_profile_dst->group_size = ecmp_profile_src->group_size;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ecmp_user_profile_allocate(
    int unit,
    int *user_profile_id,
    dnx_l3_ecmp_user_profile_t * ecmp_profile)
{
    uint32 alloc_flags = 0;
    uint8 is_allocated = FALSE;
    int nof_free_elements = 0;
    l3_ecmp_user_profile_t ecmp_profile_data_temp;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&ecmp_profile_data_temp, 0, sizeof(l3_ecmp_user_profile_t));

    if (*user_profile_id != -1)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_res_mngr.is_allocated(unit, *user_profile_id, &is_allocated));
    }
    if (is_allocated == FALSE)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_res_mngr.nof_free_elements_get(unit, &nof_free_elements));
        if (nof_free_elements == 0)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "Cannot create ECMP UDEP resource entry as table is full.");
        }
        SHR_IF_ERR_EXIT(ecmp_db_info.
                        ecmp_user_profile_res_mngr.allocate_single(unit, alloc_flags, NULL, user_profile_id));
    }
    else
    {
        /** Get structure from SW state, keep nof_references */
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.get(unit, *user_profile_id, &ecmp_profile_data_temp));
    }

    dnx_algo_l3_ecmp_user_profile_sw_structure_get(ecmp_profile, &ecmp_profile_data_temp);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.set(unit, *user_profile_id, &ecmp_profile_data_temp));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ecmp_user_profile_update_ref_count(
    int unit,
    int user_profile_id,
    uint8 increment)
{
    l3_ecmp_user_profile_t ecmp_profile_data_temp;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.get(unit, user_profile_id, &ecmp_profile_data_temp));
    if (increment)
    {
        ecmp_profile_data_temp.nof_references++;
    }
    else
    {
        ecmp_profile_data_temp.nof_references--;
    }
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.set(unit, user_profile_id, &ecmp_profile_data_temp));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ecmp_user_profile_update_group_size(
    int unit,
    int user_profile_id,
    uint32 group_size)
{
    l3_ecmp_user_profile_t ecmp_profile_data_temp;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.get(unit, user_profile_id, &ecmp_profile_data_temp));
    ecmp_profile_data_temp.group_size = group_size;
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.set(unit, user_profile_id, &ecmp_profile_data_temp));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ecmp_user_profile_is_allocated(
    int unit,
    int user_profile_id,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_res_mngr.is_allocated(unit, user_profile_id, is_allocated));
exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ecmp_user_profile_destroy(
    int unit,
    int user_profile_id)
{
    l3_ecmp_user_profile_t ecmp_profile_data_temp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.get(unit, user_profile_id, &ecmp_profile_data_temp));
    if (ecmp_profile_data_temp.nof_references > 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Unable to delete an ECMP user-defined profile %d as there are references to it.\n",
                     user_profile_id);
    }

    sal_memset(&ecmp_profile_data_temp, 0, sizeof(l3_ecmp_user_profile_t));
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.set(unit, user_profile_id, &ecmp_profile_data_temp));
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_res_mngr.free_single(unit, user_profile_id, NULL));
exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ecmp_user_profile_data_get(
    int unit,
    int user_profile_id,
    dnx_l3_ecmp_user_profile_t * ecmp_profile_data)
{
    l3_ecmp_user_profile_t ecmp_profile_data_temp;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_user_profile_data.get(unit, user_profile_id, &ecmp_profile_data_temp));
    dnx_algo_l3_ecmp_user_profile_hw_structure_get(&ecmp_profile_data_temp, ecmp_profile_data);
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

/*
 * See .h file
 */
shr_error_e
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

    /*
     * Initialized the SW state that will hold the physical database according to hierarchy
     */
    {
        uint32 hier;
        const dnx_data_l3_fec_fec_physical_db_t *info;
        SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fec_db.alloc(unit));

        /*
         * Get all the FEC physical DBs
         */
        info = dnx_data_l3.fec.fec_physical_db_get(unit);
        for (hier = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
             hier < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hier++)
        {
            SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.
                            fec_db.set(unit, DNX_L3_FEC_DIRECTION_NA, hier, info->physical_table[hier]));
        }
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

/**
 * \brief
 * Initialize all the resource managers that are needed for the anti-spoofing 
 *
 * \param [in] unit - the unit number
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_algo_l3_anti_spoofing_init(
    int unit)
{
    int width_of_compress_id = 0;
    uint32 compress_id_accuped = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_anti_spoofing_support))
    {
        /*
         * Initialize l3 anti-spoofing mac/ip6 SIP compress ID bitmaps
         */
        width_of_compress_id = 1 + dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB,
                                                                           DBAL_FIELD_COMPRESSED_MAC_ID);
        SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_id_alloc.
                        mac_compress_id_bitmap.alloc_bitmap(unit, width_of_compress_id));

        SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_ref.alloc(unit, width_of_compress_id));

        /*
         * Initialize l3 anti-spoofing mac/ip6 SIP compress reference counters
         */
        width_of_compress_id = 1 + dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION,
                                                                           DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID);
        SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_id_alloc.
                        ipv6_msb32_compress_id_bitmap.alloc_bitmap(unit, width_of_compress_id));

        SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_ref.alloc(unit, width_of_compress_id));

        /*
         * accupy the id 0 of mac_compress_id and ipv6_msb32_compress_id, it is not expected to be used
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_id_allocate(unit, &compress_id_accuped));
        SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_id_allocate(unit, &compress_id_accuped));
    }
exit:
    DBAL_FUNC_FREE_VARS;
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
     * Initialize RIF res manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_intf_rif_res_mngr_init(unit));
    /*
     * Initialize the source address table template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_source_address_table_init(unit));

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
    /*
     * Initialize the FEC resource
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_init(unit));
    /*
     * Initialize the EGR_POINTED resource
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        /** in case of flow, no need to create this resource */
        SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_init(unit));
    }

    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_tables_init(unit));

    if (dnx_data_l3.vip_ecmp.feature_get(unit, dnx_data_l3_vip_ecmp_supported))
    {
        /*
         * Initialize VIP ECMP template tables resources.
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_vip_ecmp_resource_create(unit));
    }

    /*
     * Initialize the anti spoofing resource
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_anti_spoofing_init(unit));
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

/**
 * \brief
 *  This function marks a bank in extended ECMP allocation manager as invalid- unavailable for allocation
 * \param [in] unit -
 *   The unit number.
 * \param [in] bank_id -
 *   The bunk ID (index).
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_algo_l3_ecmp_extended_mark_bank_as_invalid(
    int unit,
    uint32 bank_id)
{
    uint32 alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    int ecmp_index;
    resource_tag_bitmap_utils_extra_arg_alloc_info_t alloc_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Allocating the first ECMP ID in the requested bank only in the resource manager using the 'no hierarchy' tag.
     * Allocating with the 'no hierarchy' tag is done so that the allocation manager will not allocate valid
     * entries to the bank (valid entries will always have hierarchy)
     */
    sal_memset(&alloc_info, 0, sizeof(resource_tag_bitmap_utils_extra_arg_alloc_info_t));
    alloc_info.tags[0] = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    ecmp_index = ALGO_L3_ECMP_EXTENDED_GROUP_GET_FIRST_ELEMENT_IN_BANK(unit, bank_id);
    SHR_IF_ERR_EXIT(ecmp_db_info.
                    ecmp_extended_res_manager.allocate_single(unit, alloc_flags, &alloc_info, &ecmp_index));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  This function marks a bank in extended ECMP allocation manager as valid- available for allocation
 *  This function undoes the actions of the function dnx_algo_l3_ecmp_extended_mark_bank_as_invalid
 * \param [in] unit -
 *   The unit number.
 * \param [in] bank_id -
 *   The bunk ID (index).
 * \return
 *   shr_error_e
 * \see
 *   dnx_algo_l3_ecmp_extended_mark_bank_as_invalid
 */
static shr_error_e
dnx_algo_l3_ecmp_extended_mark_bank_as_valid(
    int unit,
    uint32 bank_id)
{
    int ecmp_index;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Freeing the first ECMP ID in the requested bank (the entry was previously allocated for the purpose of marking
     * this bank as invalid) only the resource manager so that the bank would have no tag.
     */
    ecmp_index = ALGO_L3_ECMP_EXTENDED_GROUP_GET_FIRST_ELEMENT_IN_BANK(unit, bank_id);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.free_single(unit, ecmp_index, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_info_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    void *tag_info)
{
    uint32 nof_elements_per_grain = 0;
    uint32 grain_indx;
    SHR_FUNC_INIT_VARS(unit);

    grain_indx = ALGO_L3_ECMP_BANK_GET(unit, ((resource_tag_bitmap_utils_tag_info_t *) tag_info)->element);

    /** get tag information */
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, node_id, (resource_tag_bitmap_t *) res_tag_bitmap, tag_info));
    /** get nof used elements information */
    if (((resource_tag_bitmap_utils_tag_info_t *) tag_info)->tags[0] != DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_nof_used_elements_in_grain_get(unit, node_id, (resource_tag_bitmap_t) res_tag_bitmap, grain_indx, 0 /* tag_level 
                                                                                                                                                 */ ,
                                                                           &nof_elements_per_grain));
    }
    ((resource_tag_bitmap_utils_tag_info_t *) tag_info)->nof_elements = nof_elements_per_grain;

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
    resource_tag_bitmap_utils_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Receive the value of the bitmap handle (algo_instance_id).
     * It is saved in the SW state and is received based on whether the ECMP ID is in basic or extended range.
     * If the ECMP index is in extended mode, then we need to update the ecmp_index
     * to be in the 0 - 8k range and not 32k - 40k range.
     */
    
    tag_info.element = ecmp_index;
    SHR_IF_ERR_EXIT(DNX_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, advanced_algorithm_info_get(unit, &tag_info)));
    *hierarchy = tag_info.tags[0];

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
    SHR_IF_ERR_EXIT(DNX_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, is_allocated(unit, ecmp_index, is_allocated)));
    if (DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) && is_allocated)
    {
        uint32 hierarchy;
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, &hierarchy));
        /**
         * Hierarchy of 'no hierarchy' implies that the entry is not a valid entry
         * (was allocated for allocation manager reasons)
         */
        if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
        {
            *is_allocated = FALSE;
        }
    }
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
    resource_tag_bitmap_utils_extra_arg_alloc_info_t alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = (_SHR_IS_FLAG_SET(ecmp_flags, BCM_L3_WITH_ID)) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;

    sal_memset(&alloc_info, 0, sizeof(resource_tag_bitmap_utils_extra_arg_alloc_info_t));
    alloc_info.tags[0] = hierarchy;

    if (_SHR_IS_FLAG_SET(ecmp_group_flags, BCM_L3_ECMP_EXTENDED))
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.
                        ecmp_extended_res_manager.allocate_single(unit, alloc_flags, &alloc_info, ecmp_index));
        /*
         * If we are allocating a valid entry in an extended range bank for the first time we need to alert the failover
         * that this bank is occupied by ECMP
         */
        if (hierarchy != DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY
            && DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *ecmp_index))
        {
            /** check to see if the allocation is the first allocation */
            resource_tag_bitmap_utils_tag_info_t tag_info;
            tag_info.element = *ecmp_index;
            
            SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.advanced_algorithm_info_get(unit, &tag_info));

            if (tag_info.nof_elements == 1)
            {
                int bank_id = ALGO_L3_ECMP_BANK_GET(unit, *ecmp_index);
                /** Mark memory as ECMP in failover allocation manager */
                SHR_IF_ERR_EXIT(dnx_algo_failover_fec_bank_for_ecmp_extended_allocate(unit, bank_id));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.allocate_single(unit, alloc_flags, &alloc_info, ecmp_index));
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
    uint32 hierarchy;
    resource_tag_bitmap_utils_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, &hierarchy));
    tag_info.element = ecmp_index;
    SHR_IF_ERR_EXIT(DNX_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, advanced_algorithm_info_get(unit, &tag_info)));
    /*
     * Deallocate the ECMP group from the needed resource.
     * The resource is selected based on which range the ECMP ID belongs to.
     */
    SHR_IF_ERR_EXIT(DNX_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, free_single(unit, ecmp_index, NULL)));

    /*
     * If the ECMP group is valid, belongs to the extended range of entries (32k to 40k) and is the last one in the bank,
     * then we need to free the shared bank of memory between the failover and the ECMP.
     */
    if (hierarchy != DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY
        && DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) && tag_info.nof_elements == 1)
    {
        int bank_id = ALGO_L3_ECMP_BANK_GET(unit, ecmp_index);
        SHR_IF_ERR_EXIT(dnx_algo_failover_fec_bank_for_ecmp_extended_free(unit, bank_id));
    }
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
    resource_tag_bitmap_utils_tag_info_t tag_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Receive number of elements allocated in the Extended range resource */
    sal_memset(&tag_info, 0, sizeof(tag_info));
    tag_info.element = ALGO_L3_ECMP_EXTENDED_GROUP_GET_FIRST_ELEMENT_IN_BANK(unit, bank_id);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.advanced_algorithm_info_get(unit, &tag_info));
    /** If the bank is not empty regarding ECMP, FEC protection cannot reserve this bank. */
    if (tag_info.nof_elements != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Cannot reserve shared bank %d for the use of FEC protection as it not empty\n",
                     bank_id);
    }
    /*
     *  marked with NO_HIERARCHY indicating unavailable bank for allocation.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_extended_mark_bank_as_invalid(unit, bank_id));
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
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_extended_mark_bank_as_valid(unit, bank_id));
exit:
    SHR_FUNC_EXIT;
}
/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
algo_l3_fec_hierarchy_stage_map_get(
    int unit,
    uint32 fec_index,
    uint32 *hierarchy_p)
{
    uint32 bank_id;
    mdb_physical_table_e fec_db;
    SHR_FUNC_INIT_VARS(unit);

    if (fec_index < dnx_data_l3.fec.nof_fecs_get(unit))
    {
        /*
         * Get bank index based on FEC ID
         */
        bank_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_index);
        /*
         * Get FEC physical database
         */
        SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_db_map.get(unit, bank_id, &fec_db));
        /*
         * Check hierarchy
         */
        *hierarchy_p = dnx_data_l3.fec.fec_tables_info_get(unit, fec_db)->hierarchy;
        /*
         * In case the FEC index is in the FECZ range check the hierarchy value from the SW state
         */
        if ((dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_2)
            && ((*hierarchy_p) == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY))
        {
            uint32 range_start = 0;
            uint32 range_size = 0;
            uint8 hierarchy_temp;
            /*
             * Get FEC ranges per database
             */
            SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &range_start, &range_size));
            if ((fec_index >= range_start) && (fec_index < (range_start + range_size)))
            {
                SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fecz.
                                hierarchy.get(unit, fec_index - range_start, &hierarchy_temp));
                *hierarchy_p = hierarchy_temp;
            }
        }

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
algo_l3_fec_db_stage_map_get(
    int unit,
    uint32 fec_index,
    mdb_physical_table_e * fec_db)
{
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);

    if (fec_index < dnx_data_l3.fec.nof_fecs_get(unit))
    {
        bank_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_index);
        SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_db_map.get(unit, bank_id, fec_db));
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
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags)
{
    int grain_size = 2;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_utils_extra_arg_create_info_t extra_create_info;
    resource_tag_bitmap_t *resource = (resource_tag_bitmap_t *) res_tag_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grains_size[0] = grain_size;
    extra_create_info.max_tags_value[0] = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    sal_strncpy(res_tag_bitmap_create_info.name, create_data->name, sizeof(res_tag_bitmap_create_info.name));
    sal_strncpy(res_tag_bitmap_create_info.advanced_algo_name, create_data->advanced_algo_name,
                sizeof(res_tag_bitmap_create_info.advanced_algo_name));
    res_tag_bitmap_create_info.first_element = create_data->first_element;
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;
    res_tag_bitmap_create_info.advanced_algorithm = create_data->advanced_algorithm;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, node_id, resource, &res_tag_bitmap_create_info,
                                               &extra_create_info, alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_allocate(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    algo_dnx_l3_fec_data_t fec_alloc_data;
    uint32 grain_size = 2;
    uint8 with_id;
    resource_tag_bitmap_utils_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    SHR_NULL_CHECK(extra_arguments, _SHR_E_PARAM, "extra_arguments");

    fec_alloc_data = *((algo_dnx_l3_fec_data_t *) extra_arguments);
    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_utils_alloc_info_t));
    /*
     * If allocating in shared bank and without ID,
     * then the ALLOC_IN_RANGE flag needs to be set in order to be able to allocate without ID
     * in the range between the end of the ECMP range and the end of the shared bank.
     */
    if (!with_id && (fec_alloc_data.sub_resource_index == DNX_ALGO_L3_SHARED_RANGE_FEC_BANK))
    {
        /** range_start is the end of the ECMP range */
        res_tag_bitmap_alloc_info.range_start =
            dnx_data_l3.ecmp.nof_ecmp_get(unit) -
            (MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, fec_alloc_data.sub_resource_index));
        /** range_end is the start of the next bank*/
        res_tag_bitmap_alloc_info.range_end = MDB_CALC_FEC_BANK_SIZE(unit, fec_alloc_data.sub_resource_index);
        flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    }

    res_tag_bitmap_alloc_info.count = 1;

    /*
     * Check the super fec types, in case of protection the flag RESOURCE_TAG_BITMAP_ALLOC_ALIGN should be set
     */
    if (dnx_data_l3.fec.fec_resource_info_get(unit, fec_alloc_data.super_fec_type_tag)->is_protection_type)
    {
        res_tag_bitmap_alloc_info.count = grain_size;
        res_tag_bitmap_alloc_info.align = grain_size;
        /** Flag guarantees that in case of protection, the first entry in the pair will be even. */
        flags |= RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
    }

    res_tag_bitmap_alloc_info.tags[0] = fec_alloc_data.super_fec_type_tag;

    if (!with_id)
    {
        uint32 temp_flags = flags | RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG | RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
        int rv;
        res_tag_bitmap_alloc_info.flags = temp_flags;
        rv = resource_tag_bitmap_alloc(unit, node_id, (resource_tag_bitmap_t) res_tag_bitmap,
                                       &res_tag_bitmap_alloc_info, element);
        if (rv == _SHR_E_NONE)
        {
            flags |= RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
        }
    }
    res_tag_bitmap_alloc_info.flags = flags;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc
                    (unit, node_id, (resource_tag_bitmap_t) res_tag_bitmap, &res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_free(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    void *extra_argument)
{
    resource_tag_bitmap_utils_tag_info_t tag_info;
    uint32 super_fec_type;
    uint32 grain_size = 2;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Call the main function to clear the element from the bitmap
     */
    sal_memset(&tag_info, 0, sizeof(tag_info));
    tag_info.element = element;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, node_id, res_tag_bitmap, &tag_info));

    super_fec_type = tag_info.tags[0];

    /*
     * If we are freeing a FEC with protection, then we free the pair it belongs to
     * instead of the two elements separately.
     */
    if (dnx_data_l3.fec.fec_resource_info_get(unit, super_fec_type)->is_protection_type)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_free_several(unit, node_id, res_tag_bitmap, grain_size, element, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_free(unit, node_id, res_tag_bitmap, element, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the super FEC type of the FEC pair it is being allocated in.
 *  \param [in] unit - The unit number.
 *  \param [in] fec_index - The new FEC ID
 *  \param [in] fec_resource_type - The super FEC resource type of the new FEC
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
    dbal_enum_value_field_fec_resource_type_e old_fec_resource_type;
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
    sub_resource_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_index);
    /** Receive indication whether the entry is allocated. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.is_allocated
                    (unit, sub_resource_id, fec_in_pair - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id),
                     &is_allocated));
    if (is_allocated == TRUE)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.get
                        (unit, sub_resource_id,
                         (fec_index - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id)) / grain_size,
                         &old_fec_resource_type));
        if (old_fec_resource_type != fec_resource_type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Super FEC resource type for pair of FEC value %u is already %u, but user provided %u\n",
                         fec_index, old_fec_resource_type, fec_resource_type);
        }
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
    mdb_physical_table_e fec_db,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type)
{
    uint32 alloc_flags = 0;
    /** Size in number of entries of the current FEC bank */
    uint32 fec_bank_size = dnx_data_l3.fec.bank_size_get(unit);
    uint32 sub_resource_id;
    uint32 hierarchy = 0;
    uint32 subr_id;
    /** number of free elements per given bank */
    int nof_free_elements = 0;
    int rv = BCM_E_NONE;
    /** data for a cluster which will be updated */
    algo_dnx_l3_fec_data_t fec_alloc_data;
    mdb_physical_table_e current_db;

    SHR_FUNC_INIT_VARS(unit);

    alloc_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_L3_FEC_WITH_ID)) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;

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
        sub_resource_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, *fec_index);

        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_type_verify(unit, *fec_index, fec_resource_type));

        /*
         * check if current database matches already defined database.
         * If it doesn't then an error will be returned.
         */
        SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_db_map.get(unit, sub_resource_id, &current_db));

        if (fec_db != current_db)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected database for bank %d is %d, instead received %d\n",
                         sub_resource_id, current_db, fec_db);
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

        SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &range_start, &range_size));

        range_end = (range_start + range_size) / dnx_data_l3.fec.bank_size_get(unit);
        /** Limit the without ID range to its valid range in case the allocation starts outside that range */
        range_start =
            UTILEX_MAX(range_start / dnx_data_l3.fec.bank_size_get(unit), dnx_data_l3.fec.first_shared_bank_get(unit));
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
             * If the sub-resource have the same DB and room for the current allocation
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
        if (subr_id < range_end)
        {
            sub_resource_id = subr_id;
        }
        else
        {
            /*
             * If the subr_id variable equals the number of sub-resources
             * then we have reached to the end of the loop and we didn't find a valid sub-resource.
             */
            SHR_ERR_EXIT(_SHR_E_FULL, "No valid bank for without ID allocation for DB %d found\n", hierarchy + 1);
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

    sub_resource_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_index);

    fec_index = fec_index - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

    /** Deallocate the FEC entry from the resource. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                    fec_res_manager.free_single(unit, sub_resource_id, fec_index, NULL));

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

    sub_resource_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_index);

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
    dbal_enum_value_field_fec_resource_type_e * super_fec_resource_type)
{
    uint32 sub_resource_id;
    uint32 grain_size = 2;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_index);

    fec_index -= MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, sub_resource_id);

    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.get
                    (unit, sub_resource_id, fec_index / grain_size, super_fec_resource_type));

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
    dbal_enum_value_field_fec_resource_type_e super_fec_resource_type;
    SHR_FUNC_INIT_VARS(unit);

    fec_index = BCM_L3_ITF_VAL_GET(fec_index);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_super_fec_type_get(unit, fec_index, &super_fec_resource_type));

    /*
     * If the super FEC type is one of the protected types then this FEC is part of a protection pair.
     */
    if (dnx_data_l3.fec.fec_resource_info_get(unit, super_fec_resource_type)->is_protection_type)
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
    mdb_physical_table_e fec_db,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type,
    int *start_if_id)
{
    uint8 is_protected = 0;
    uint32 range_start;
    uint32 range_size;
    uint32 range_end = 0;
    uint32 fec_bank_size;
    int subr_id;
    int increment;
    int free_consecutive_fecs = 0;
    int start_if_id_temp = -1;
    int start_if_id_temp_global = -1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &range_start, &range_size));
    /** Limit the without ID range to its valid range in case the allocation starts outside that range */
    range_start =
        UTILEX_MAX(range_start / dnx_data_l3.fec.bank_size_get(unit),
                   dnx_data_l3.fec.first_bank_without_id_alloc_get(unit));
    range_size /= dnx_data_l3.fec.bank_size_get(unit);
    *start_if_id = -1;

    is_protected = dnx_data_l3.fec.fec_resource_info_get(unit, fec_resource_type)->is_protection_type;
    nof_members = nof_members << is_protected;
    increment = 1 << is_protected;

    /** Iterate over the FEC banks*/
    for (subr_id = range_start; (subr_id < (range_start + range_size) && (*start_if_id == -1)); subr_id++)
    {

        int next_fec;
        int prev_fec = -1;
        int free_fec_range_size = 0;
        int nof_free_elements = 0;

        fec_bank_size = MDB_CALC_FEC_BANK_SIZE(unit, subr_id);

        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                        fec_res_manager.nof_free_elements_get(unit, subr_id, &nof_free_elements));

        for (next_fec = 0; nof_free_elements != 0; next_fec += increment)
        {
            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.get_next(unit, subr_id, &next_fec));

            range_end =
                (next_fec ==
                 DNX_ALGO_RES_ILLEGAL_ELEMENT) ? fec_bank_size : (ALGO_L3_GET_PREV_FEC_ID(is_protected, next_fec));
            free_fec_range_size = range_end - (ALGO_L3_GET_NEXT_FEC_ID(is_protected, (prev_fec + 1)));

            /** If the requested FEC range is found in consecutive banks, return global FEC index.*/
            if (((free_fec_range_size + free_consecutive_fecs) >= nof_members) && (start_if_id_temp != -1))
            {
                *start_if_id = start_if_id_temp_global;
                break;
            }
            /** If the requested FEC range is found in a bank, return FEC index within a FEC bank.*/
            if (free_fec_range_size >= nof_members)
            {
                start_if_id_temp = (ALGO_L3_GET_NEXT_FEC_ID(is_protected, (prev_fec + 1)));
                start_if_id_temp_global = MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, subr_id) + start_if_id_temp;
                *start_if_id = start_if_id_temp_global;
                break;
            }
            /** Set a temporary FEC index when the requested FEC range is not found in the bank,
             *  but there are some free elements in the bank. Continue searching in the next bank.*/
            if ((next_fec == DNX_ALGO_RES_ILLEGAL_ELEMENT) && (start_if_id_temp == -1))
            {
                start_if_id_temp = (ALGO_L3_GET_NEXT_FEC_ID(is_protected, (prev_fec + 1)));
                start_if_id_temp_global = MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, subr_id) + start_if_id_temp;
            }
            /** In case the requested FEC range is not found in consecutive FEC banks, reset the temporary FEC index.*/
            if ((next_fec != DNX_ALGO_RES_ILLEGAL_ELEMENT)
                && ((free_fec_range_size + free_consecutive_fecs) < nof_members))
            {
                start_if_id_temp = -1;
                start_if_id_temp_global = -1;
                free_consecutive_fecs = 0;
            }
            if (next_fec == DNX_ALGO_RES_ILLEGAL_ELEMENT)
            {
                break;
            }
            prev_fec = next_fec;
        }
        /** Calculate the number of free consecutive FECs*/
        free_consecutive_fecs += free_fec_range_size;
    }

    if (*start_if_id == -1)
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
    mdb_physical_table_e mdb_physical_fec_db,
    int *nof_clusters,
    mdb_cluster_alloc_info_t * clusters)
{
    mdb_cluster_alloc_info_t clusters_found[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS];
    dbal_physical_tables_e dbal_physical_fec_table_id;
    int idx;
    int cl_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get information for the allocated clusters for the current FEC table */
    dbal_physical_fec_table_id = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_fec_db)->physical_to_logical;
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
    ecmp_profile.consistent_hashing_id = *profile_index;

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
    int *profile_index)
{
    uint32 hierarchy_iter;
    int index = *profile_index;
    SHR_FUNC_INIT_VARS(unit);

    for (hierarchy_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hierarchy_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hierarchy_iter++)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.get_next(unit, hierarchy_iter, &index));

        if (index != DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            *profile_index = index;
            break;
        }
        else
        {
            index = *profile_index;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_convert_fecz_id(
    int unit,
    uint32 fecz_id,
    uint32 *relative_fecz_id)
{

    uint32 range_start, range_size;
    mdb_physical_table_e fec_db;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_fec_db_stage_map_get(unit, fecz_id, &fec_db));
    SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &range_start, &range_size));
    *relative_fecz_id = fecz_id - range_start;

exit:
    SHR_FUNC_EXIT;
}

/** The max value of DBAL_FIELD_xxx */
uint32
dnx_algo_l3_compress_field_max_size_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id)
{
    int rv = 0;
    int compress_id_max_size;

    rv = dbal_tables_field_max_value_get(unit, table_id, field_id, FALSE, 0, 0, &compress_id_max_size);
    return (rv != _SHR_E_NONE) ? 0 : compress_id_max_size;
}
/**
 * \brief
 * Find an mac_compress_id that wasn't assigned already, and mark it as assigned.
 */
shr_error_e
dnx_algo_l3_mac_compress_id_allocate(
    int unit,
    uint32 *mac_compress_id)
{
    int start_of_mac_compress = 0;
    int end_of_mac_compress = 0;
    uint32 i = 0;
    uint8 id_used = 0;

    SHR_FUNC_INIT_VARS(unit);
    end_of_mac_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB,
                                                DBAL_FIELD_COMPRESSED_MAC_ID);

    /*
     * Get the used mac_compress_id bitmap, and search for an available bank.
     */

    for (i = start_of_mac_compress; i <= end_of_mac_compress; i++)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_id_alloc.mac_compress_id_bitmap.bit_get(unit, i, &id_used));
        if (!id_used)
        {
            break;
        }
    }

    if (i == end_of_mac_compress + 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sanity failed: a mac_compress_id is supposed to be available, but none was found.");
    }

    /*
     * Set the mac_compress_id as in use.
     */
    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_id_alloc.mac_compress_id_bitmap.bit_set(unit, i));

    *mac_compress_id = i;

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Free an mac_compress_id, and mark it as unassigned.
 */
shr_error_e
dnx_algo_l3_mac_compress_id_free(
    int unit,
    uint32 mac_compress_id)
{
    uint8 id_used = 0;
    int max_of_mac_compress = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_of_mac_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB,
                                                DBAL_FIELD_COMPRESSED_MAC_ID);
    if (mac_compress_id > max_of_mac_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the input mac_compress_id is out of valid range [1,%d]\n", max_of_mac_compress);
    }
    /*
     * Get the used mac_compress_id bitmap, and search for an available bank.
     */

    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_id_alloc.mac_compress_id_bitmap.bit_get(unit, mac_compress_id, &id_used));
    if (!id_used)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sanity failed: the mac_compress_id %d has not been used, don't need to free.", mac_compress_id);

    }

    /*
     * Set the mac_compress_id as not in use.
     */
    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_id_alloc.mac_compress_id_bitmap.bit_clear(unit, mac_compress_id));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_mac_compress_update_ref_count(
    int unit,
    uint32 user_id,
    uint8 increment)
{
    uint32 nof_ref;
    int max_of_mac_compress = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_of_mac_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB,
                                                DBAL_FIELD_COMPRESSED_MAC_ID);

    if (user_id > max_of_mac_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the input mac_compress_id is out of valid range [1,%d]\n", max_of_mac_compress);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_ref.get(unit, user_id, &nof_ref));

    if (increment > 0 && nof_ref < 0xffffffff)
    {
        ++nof_ref;
    }
    else if (increment == 0 && nof_ref > 0)
    {
        --nof_ref;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the nof_ref %d is out of valid range for the operation %d\n", nof_ref, increment);
    }
    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_ref.set(unit, user_id, nof_ref));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_mac_compress_ref_get(
    int unit,
    uint32 user_id,
    uint32 *nof_ref)
{
    int max_of_mac_compress = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_of_mac_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB,
                                                DBAL_FIELD_COMPRESSED_MAC_ID);

    if (user_id > max_of_mac_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the input mac_compress_id is out of valid range [1,%d]\n", max_of_mac_compress);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_ref.get(unit, user_id, nof_ref));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_mac_compress_ref_clear(
    int unit,
    uint32 user_id)
{
    int max_of_mac_compress = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_of_mac_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB,
                                                DBAL_FIELD_COMPRESSED_MAC_ID);

    if (user_id > max_of_mac_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the input mac_compress_id is out of valid range [1,%d]\n", max_of_mac_compress);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.mac_compress_ref.set(unit, user_id, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Find an ip6_msb32_compress_id that wasn't assigned already, and mark it as assigned.
 */
shr_error_e
dnx_algo_l3_ip6_msb32_compress_id_allocate(
    int unit,
    uint32 *ip6_msb32_compress_id)
{
    int start_of_ip6_msb32_compress = 0;
    int end_of_ip6_msb32_compress = 0;
    uint32 i = 0;
    uint8 id_used = 0;

    SHR_FUNC_INIT_VARS(unit);
    end_of_ip6_msb32_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION,
                                                DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID);

    /*
     * Get the used ipv6_msb32_compress_id bitmap, and search for an available bank.
     */

    for (i = start_of_ip6_msb32_compress; i <= end_of_ip6_msb32_compress; i++)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_id_alloc.
                        ipv6_msb32_compress_id_bitmap.bit_get(unit, i, &id_used));
        if (!id_used)
        {
            break;
        }
    }

    if (i == end_of_ip6_msb32_compress + 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sanity failed: an ip6_msb32_compress_id is supposed to be available, but none was found.");
    }

    /*
     * Set the ipv6_msb32_compress_id as in use.
     */
    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_id_alloc.ipv6_msb32_compress_id_bitmap.bit_set(unit, i));

    *ip6_msb32_compress_id = i;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Free an ip6_msb32_compress_id, and mark it as unassigned.
 */
shr_error_e
dnx_algo_l3_ip6_msb32_compress_id_free(
    int unit,
    uint32 ip6_msb32_compress_id)
{
    uint8 id_used = 0;
    int max_of_ip6_msb32_compress = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_of_ip6_msb32_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION,
                                                DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID);
    if (ip6_msb32_compress_id > max_of_ip6_msb32_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "the input mac_compress_id is out of valid range [1,%d]\n", max_of_ip6_msb32_compress);
    }
    /*
     * Get the used mac_compress_id bitmap, and search for an available bank.
     */

    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_id_alloc.
                    ipv6_msb32_compress_id_bitmap.bit_get(unit, ip6_msb32_compress_id, &id_used));
    if (!id_used)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sanity failed: the ip6_msb32_compress_id %d has not been used, don't need to free.",
                     ip6_msb32_compress_id);
    }

    /*
     * Set the mac_compress_id as not in use.
     */
    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_id_alloc.
                    ipv6_msb32_compress_id_bitmap.bit_clear(unit, ip6_msb32_compress_id));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ip6_msb32_compress_update_ref_count(
    int unit,
    uint32 user_id,
    uint8 increment)
{
    uint32 nof_ref;
    int max_of_ip6_msb32_compress = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_of_ip6_msb32_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION,
                                                DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID);

    if (user_id > max_of_ip6_msb32_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "the input mac_compress_id is out of valid range [1,%d]\n", max_of_ip6_msb32_compress);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_ref.get(unit, user_id, &nof_ref));
    if (increment > 0 && nof_ref < 0xffffffff)
    {
        ++nof_ref;
    }
    else if (increment == 0 && nof_ref > 0)
    {
        --nof_ref;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the nof_ref %d is out of valid range for the operation %d\n", nof_ref, increment);
    }
    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_ref.set(unit, user_id, nof_ref));

exit:
    SHR_FUNC_EXIT;
}
/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ip6_msb32_compress_ref_get(
    int unit,
    uint32 user_id,
    uint32 *nof_ref)
{
    int max_of_ip6_msb32_compress = 0;

    SHR_FUNC_INIT_VARS(unit);

    max_of_ip6_msb32_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION,
                                                DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID);
    if (user_id > max_of_ip6_msb32_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "the input mac_compress_id is out of valid range [1,%d]\n", max_of_ip6_msb32_compress);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_ref.get(unit, user_id, nof_ref));

exit:
    SHR_FUNC_EXIT;
}
/** Refer to the .h file */
shr_error_e
dnx_algo_l3_ip6_msb32_compress_ref_clear(
    int unit,
    uint32 user_id)
{
    int max_of_ip6_msb32_compress = 0;

    SHR_FUNC_INIT_VARS(unit);

    max_of_ip6_msb32_compress =
        dnx_algo_l3_compress_field_max_size_get(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION,
                                                DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID);
    if (user_id > max_of_ip6_msb32_compress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "the input mac_compress_id is out of valid range [1,%d]\n", max_of_ip6_msb32_compress);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.ipv6_msb32_compress_ref.set(unit, user_id, 0));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
