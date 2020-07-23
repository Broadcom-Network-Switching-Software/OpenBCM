

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FAILOVER

#include <bcm/failover.h>
#include <bcm/error.h>

#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <shared/shrextend/shrextend_debug.h>




static shr_error_e
dnx_failover_fec_hierarchy_set(
    int unit,
    int fec_protection_pointer,
    dbal_enum_value_field_hierarchy_level_e fec_hierarchy)
{
    uint32 entry_handle_id;
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    bank_id =
        fec_protection_pointer / (dnx_data_failover.path_select.fec_size_get(unit) /
                                  dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FAILOVER_FEC_PATH_SELECT_BANK_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, INST_SINGLE, fec_hierarchy);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_failover_fec_hierarchy_get(
    int unit,
    int fec_protection_pointer,
    dbal_enum_value_field_hierarchy_level_e * fec_hierarchy)
{
    uint32 entry_handle_id;
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    bank_id =
        fec_protection_pointer / (dnx_data_failover.path_select.fec_size_get(unit) /
                                  dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FAILOVER_FEC_PATH_SELECT_BANK_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, INST_SINGLE, fec_hierarchy);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_failover_id_range_verify(
    int unit,
    int failover_type,
    int protection_pointer)
{
    SHR_FUNC_INIT_VARS(unit);

    if (failover_type == DNX_FAILOVER_TYPE_INGRESS)
    {
        SHR_RANGE_VERIFY(protection_pointer, 0, dnx_data_failover.path_select.ingress_size_get(unit) - 1, _SHR_E_PARAM,
                         "ingress failover-id out of bound.\n");
        if (protection_pointer == dnx_data_failover.path_select.ing_no_protection_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "failover id %d pre-allocated for ingress no protection.\n", protection_pointer);
        }
    }
    else if (failover_type == DNX_FAILOVER_TYPE_ENCAP)
    {
        SHR_RANGE_VERIFY(protection_pointer, 0, dnx_data_failover.path_select.egress_size_get(unit) - 1, _SHR_E_PARAM,
                         "egress failover-id out of bound.\n");
        if (protection_pointer == dnx_data_failover.path_select.egr_no_protection_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "failover id %d pre-allocated for egress no protection.\n", protection_pointer);
        }
    }
    else if (failover_type == DNX_FAILOVER_TYPE_FEC)
    {
        SHR_RANGE_VERIFY(protection_pointer, 0, dnx_data_failover.path_select.fec_size_get(unit) - 1, _SHR_E_PARAM,
                         "FEC failover-id out of bound.\n");
        if (protection_pointer == dnx_data_failover.path_select.fec_no_protection_get(unit)
            || protection_pointer == dnx_data_failover.path_select.fec_facility_protection_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "failover id %d pre-allocated for FEC no protection / facility protection.\n",
                         protection_pointer);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected failover type %d.\n", failover_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_failover_id_verify(
    int unit,
    bcm_failover_t failover_id)
{
    int protection_ptr, failover_type;
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    DNX_FAILOVER_TYPE_GET(failover_type, failover_id);
    DNX_FAILOVER_ID_GET(protection_ptr, failover_id);

    
    SHR_IF_ERR_EXIT(dnx_failover_id_range_verify(unit, failover_type, protection_ptr));

    
    if (failover_type == DNX_FAILOVER_TYPE_FEC)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.is_allocated(unit, protection_ptr, &is_allocated));
    }
    else if (failover_type == DNX_FAILOVER_TYPE_INGRESS)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.ing_path_select.is_allocated(unit, protection_ptr, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_failover_db.egr_path_select.is_allocated(unit, protection_ptr, &is_allocated));
    }

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "protection pointer wasn't allocated %d. use bcm_failover_create() to allocate\n",
                     protection_ptr);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_failover_fec_hw_init(
    int unit)
{
    uint32 entry_handle_id;
    uint8 is_facility_accelerated;
    int no_protection_ptr;
    int facility_protection_ptr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_failover_fec_hierarchy_set
                    (unit, dnx_data_failover.path_select.fec_no_protection_get(unit),
                     DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FAILOVER_FEC_PATH_SELECT_CONFIG, &entry_handle_id));

    
    no_protection_ptr = dnx_data_failover.path_select.fec_no_protection_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NO_PROTECTION_PTR, INST_SINGLE, no_protection_ptr);

    
    facility_protection_ptr = dnx_data_failover.path_select.fec_facility_protection_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_PROTECTION_PTR, INST_SINGLE,
                                 facility_protection_ptr);

    
    is_facility_accelerated = dnx_data_failover.facility.feature_get(unit,
                                                                     dnx_data_failover_facility_accelerated_mode_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_ACCELERATED_MODE_ENABLE, INST_SINGLE,
                                 is_facility_accelerated);

    
    
    
    
    {
        
        uint32 facility_logic_array[] =
            { 0xffff0000, 0x00000000, 0xaa2aaa2a, 0xf000f000, 0xffff0000, 0x00000000, 0x2a2a2a2a, 0x00000000,
            0xffff0000, 0x00000000, 0xea2aea2a, 0xf000f000, 0xffff0000, 0x00000000, 0xaa2aaa2a, 0x00000000
        };

        uint32 facility_trap_logic_array[] =
            { 0x00000000, 0x00000000, 0xc000c000, 0xf000f000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
        };

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_LOGIC, INST_SINGLE,
                                         facility_logic_array);

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_TRAP_LOGIC, INST_SINGLE,
                                         facility_trap_logic_array);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_failover_init(
    int unit)
{
    uint8 tag = 0;
    int no_protection_element, facility_element;
    SHR_FUNC_INIT_VARS(unit);

    
    
    no_protection_element = dnx_data_failover.path_select.ing_no_protection_get(unit);
    SHR_IF_ERR_EXIT(algo_failover_db.ing_path_select.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL,
                                                                     &no_protection_element));

    
    
    no_protection_element = dnx_data_failover.path_select.egr_no_protection_get(unit);
    SHR_IF_ERR_EXIT(algo_failover_db.egr_path_select.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL,
                                                                     &no_protection_element));

    
    
    no_protection_element = dnx_data_failover.path_select.fec_no_protection_get(unit);
    SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &tag,
                                                                     &no_protection_element));

    facility_element = dnx_data_failover.path_select.fec_facility_protection_get(unit);
    SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &tag,
                                                                     &facility_element));

    
    SHR_IF_ERR_EXIT(dnx_failover_fec_hw_init(unit));

exit:
    SHR_FUNC_EXIT;

}




shr_error_e
dnx_failover_id_valid_verify(
    int unit,
    int expected_type,
    dbal_enum_value_field_hierarchy_level_e expected_hierarchy,
    bcm_failover_t failover_id)
{
    int protection_ptr;
    uint32 actual_hierarchy = 0;
    int actual_type;
    SHR_FUNC_INIT_VARS(unit);

    
    DNX_FAILOVER_TYPE_GET(actual_type, failover_id);
    DNX_FAILOVER_ID_GET(protection_ptr, failover_id);

    
    if (actual_type == DNX_FAILOVER_TYPE_FEC)
    {
        SHR_IF_ERR_EXIT(dnx_failover_fec_hierarchy_get(unit, protection_ptr, &actual_hierarchy));
    }

    
    if (expected_type != actual_type || expected_hierarchy != actual_hierarchy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected failover-id (type or hierarchy). got 0%x\n", failover_id);
    }
    
    SHR_IF_ERR_EXIT(dnx_failover_id_verify(unit, failover_id));

exit:
    SHR_FUNC_EXIT;
}




int
bcm_dnx_failover_set(
    int unit,
    bcm_failover_t failover_id,
    int enable)
{
    uint32 entry_handle_id;
    int failover_type, protection_pointer;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_failover_id_verify(unit, failover_id));

    
    DNX_FAILOVER_TYPE_GET(failover_type, failover_id);
    DNX_FAILOVER_ID_GET(protection_pointer, failover_id);

    
    
    switch (failover_type)
    {
        case DNX_FAILOVER_TYPE_INGRESS:
            table_id = DBAL_TABLE_FAILOVER_INGRESS_PATH_SELECT;
            break;
        case DNX_FAILOVER_TYPE_ENCAP:
            table_id = DBAL_TABLE_FAILOVER_EGRESS_PATH_SELECT;
            break;
        case DNX_FAILOVER_TYPE_FEC:
            table_id = DBAL_TABLE_FAILOVER_FEC_PATH_SELECT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected failover type %d.\n", failover_type);
            break;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, protection_pointer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PROTECTION_PATH_PRIMARY :
                                 DBAL_ENUM_FVAL_PROTECTION_PATH_SECONDARY);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}



int
bcm_dnx_failover_get(
    int unit,
    bcm_failover_t failover_id,
    int *enable)
{
    uint32 entry_handle_id;
    int failover_type, protection_pointer;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_failover_id_verify(unit, failover_id));
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    
    DNX_FAILOVER_TYPE_GET(failover_type, failover_id);
    DNX_FAILOVER_ID_GET(protection_pointer, failover_id);

    
    
    switch (failover_type)
    {
        case DNX_FAILOVER_TYPE_INGRESS:
            table_id = DBAL_TABLE_FAILOVER_INGRESS_PATH_SELECT;
            break;
        case DNX_FAILOVER_TYPE_ENCAP:
            table_id = DBAL_TABLE_FAILOVER_EGRESS_PATH_SELECT;
            break;
        case DNX_FAILOVER_TYPE_FEC:
            table_id = DBAL_TABLE_FAILOVER_FEC_PATH_SELECT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected failover type %d.\n", failover_type);
            break;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, protection_pointer);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, (uint32 *) enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    *enable = (*enable == DBAL_ENUM_FVAL_PROTECTION_PATH_PRIMARY) ? 1 : 0;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_failover_create_verify(
    int unit,
    uint32 flags,
    bcm_failover_t * failover_id)
{
    uint32 supported_flags;
    int is_fec, is_ingress, is_egress, failover_type;
    int protection_pointer;

    SHR_FUNC_INIT_VARS(unit);

    
    supported_flags =
        BCM_FAILOVER_INGRESS | BCM_FAILOVER_ENCAP | BCM_FAILOVER_FEC | BCM_FAILOVER_FEC_2ND_HIERARCHY |
        BCM_FAILOVER_FEC_3RD_HIERARCHY | BCM_FAILOVER_WITH_ID;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    
    is_ingress = (flags & BCM_FAILOVER_INGRESS) ? 1 : 0;
    is_egress = (flags & BCM_FAILOVER_ENCAP) ? 1 : 0;
    is_fec = (flags & BCM_FAILOVER_FEC) ? 1 : 0;
    if (is_ingress + is_egress + is_fec != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "failover id can be created for specific type only.\n");
    }
    else if (is_ingress)
    {
        failover_type = DNX_FAILOVER_TYPE_INGRESS;
    }
    else if (is_egress)
    {
        failover_type = DNX_FAILOVER_TYPE_ENCAP;
    }
    else        
    {
        failover_type = DNX_FAILOVER_TYPE_FEC;
    }

    
    if (is_fec)
    {
        int is_second, is_third;
        is_second = (flags & BCM_FAILOVER_FEC_2ND_HIERARCHY) ? 1 : 0;
        is_third = (flags & BCM_FAILOVER_FEC_3RD_HIERARCHY) ? 1 : 0;
        if (is_second + is_third > 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "failover id can be created for specific FEC hierarchy only.\n");
        }
    }

    
    SHR_NULL_CHECK(failover_id, _SHR_E_PARAM, "A valid pointer for failover_id is needed!");

    
    if (flags & BCM_FAILOVER_WITH_ID)
    {
        DNX_FAILOVER_ID_GET(protection_pointer, *failover_id);
        SHR_IF_ERR_EXIT(dnx_failover_id_range_verify(unit, failover_type, protection_pointer));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_failover_create(
    int unit,
    uint32 flags,
    bcm_failover_t * failover_id)
{
    int is_fec, is_ingress, is_egress;
    int failover_type;
    uint8 hierarchy = 0;
    uint32 alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_failover_create_verify(unit, flags, failover_id));

    
    is_ingress = (flags & BCM_FAILOVER_INGRESS) ? 1 : 0;
    is_egress = (flags & BCM_FAILOVER_ENCAP) ? 1 : 0;
    is_fec = (flags & BCM_FAILOVER_FEC) ? 1 : 0;
    if (is_ingress)
    {
        failover_type = DNX_FAILOVER_TYPE_INGRESS;
    }
    else if (is_egress)
    {
        failover_type = DNX_FAILOVER_TYPE_ENCAP;
    }
    else        
    {
        failover_type = DNX_FAILOVER_TYPE_FEC;
    }
    if (is_fec)
    {
        hierarchy = (flags & BCM_FAILOVER_FEC_2ND_HIERARCHY) ? 1 : hierarchy;
        hierarchy = (flags & BCM_FAILOVER_FEC_3RD_HIERARCHY) ? 2 : hierarchy;
    }
    if (flags & BCM_FAILOVER_WITH_ID)
    {
        DNX_FAILOVER_ID_GET(*failover_id, *failover_id);
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {
        *failover_id = 0;
        alloc_flags = 0;
    }

    
    if (failover_type == DNX_FAILOVER_TYPE_FEC)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.allocate_single(unit, alloc_flags, &hierarchy, failover_id));
    }
    else if (failover_type == DNX_FAILOVER_TYPE_INGRESS)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.ing_path_select.allocate_single(unit, alloc_flags, NULL, failover_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_failover_db.egr_path_select.allocate_single(unit, alloc_flags, NULL, failover_id));
    }

    
    if (is_fec)
    {
        SHR_IF_ERR_EXIT(dnx_failover_fec_hierarchy_set(unit, *failover_id, hierarchy));
    }

    
    DNX_FAILOVER_SET(*failover_id, *failover_id, failover_type);

    
    SHR_IF_ERR_EXIT(bcm_dnx_failover_set(unit, *failover_id, 1));

exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_failover_destroy(
    int unit,
    bcm_failover_t failover_id)
{
    int protection_ptr, failover_type;
    uint32 last_bank_start_index;
    int nof_elements;
    uint32 bank_size, bank_start_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    

    
    SHR_IF_ERR_EXIT(bcm_dnx_failover_set(unit, failover_id, 1));

    
    DNX_FAILOVER_TYPE_GET(failover_type, failover_id);
    DNX_FAILOVER_ID_GET(protection_ptr, failover_id);

    
    if (failover_type == DNX_FAILOVER_TYPE_FEC)
    {
        bank_size = dnx_data_failover.path_select.fec_bank_size_get(unit);
        SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.free_single(unit, protection_ptr));

        
        last_bank_start_index = dnx_data_failover.path_select.fec_size_get(unit) - bank_size;
        if (protection_ptr >= last_bank_start_index)
        {
            SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.nof_allocated_elements_in_range_get(unit,
                                                                                                 last_bank_start_index,
                                                                                                 bank_size,
                                                                                                 &nof_elements));
            if (nof_elements <= 2)
            {
                SHR_IF_ERR_EXIT(dnx_failover_fec_hierarchy_set
                                (unit, protection_ptr, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY));
            }
        }
        else
        {
            
            bank_start_index = protection_ptr / bank_size * bank_size;

            SHR_IF_ERR_EXIT(algo_failover_db.fec_path_select.nof_allocated_elements_in_range_get(unit,
                                                                                                 bank_start_index,
                                                                                                 bank_size,
                                                                                                 &nof_elements));
            if (nof_elements == 0)
            {
                SHR_IF_ERR_EXIT(dnx_failover_fec_hierarchy_set(unit, protection_ptr, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1    ));
            }
        }
    }
    else if (failover_type == DNX_FAILOVER_TYPE_INGRESS)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.ing_path_select.free_single(unit, protection_ptr));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_failover_db.egr_path_select.free_single(unit, protection_ptr));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
dnx_failover_l2_protection_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_mpls_port_t * mpls_port,
    dnx_failover_l2_protection_info_t * protection_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((vlan_port != NULL) && (mpls_port != NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't handle both vlan and mpls at the same time, one of them should be NULL");
    }

    
    protection_info->failover_id = (vlan_port == NULL) ? (mpls_port->failover_id) : (vlan_port->failover_id);
    protection_info->failover_port_id = (vlan_port == NULL) ? (mpls_port->failover_port_id) :
        (vlan_port->failover_port_id);
    protection_info->failover_mc_group = (vlan_port == NULL) ? (mpls_port->failover_mc_group) :
        (vlan_port->failover_mc_group);
    protection_info->egress_failover_port_id = (vlan_port == NULL) ? (mpls_port->egress_failover_port_id) :
        (vlan_port->egress_failover_port_id);
    protection_info->egress_failover_id = (vlan_port == NULL) ? (mpls_port->egress_failover_id) :
        (vlan_port->egress_failover_id);
    protection_info->ingress_failover_id = (vlan_port == NULL) ? (mpls_port->ingress_failover_id) :
        (vlan_port->ingress_failover_id);

    
    protection_info->is_fec_protected = (vlan_port == NULL) ? BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) :
        (vlan_port->failover_port_id);

    
    protection_info->is_ingress_mc = _BCM_MULTICAST_IS_SET(protection_info->failover_mc_group) ? TRUE : FALSE;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_failover_l2_protection_info_verify(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_mpls_port_t * mpls_port)
{
    dnx_failover_l2_protection_info_t protection_info;
    int is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = (vlan_port == NULL) ? _SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY) :
        !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY);
    is_egress = (vlan_port == NULL) ? _SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_EGRESS_ONLY) :
        !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY);

    
    SHR_IF_ERR_EXIT(dnx_failover_l2_protection_info_get(unit, vlan_port, mpls_port, &protection_info));

    if (protection_info.failover_id != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "failover_id must be 0." "FEC creation is done in bcm_l3_egress_create() API.\n");
    }

    
    if (protection_info.is_fec_protected)
    {
        
        if (protection_info.is_ingress_mc == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress multicast 1+1 protection and is not supported together with FEC"
                         "protection");
        }

        
        if (is_ingress == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC protection only supported at ingress\n");
        }
    }

    
    if (is_ingress == TRUE)
    {
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(protection_info.ingress_failover_id))
        {
            SHR_IF_ERR_EXIT(dnx_failover_id_valid_verify
                            (unit, DNX_FAILOVER_TYPE_INGRESS, 0, protection_info.ingress_failover_id));
        }
    }
    if (is_egress == TRUE)
    {
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(protection_info.egress_failover_id))
        {
            SHR_IF_ERR_EXIT(dnx_failover_id_valid_verify
                            (unit, DNX_FAILOVER_TYPE_ENCAP, 0, protection_info.egress_failover_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_failover_facility_port_status_set(
    int unit,
    bcm_gport_t gport,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FAILOVER_FACILITY_DESTINATION_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, gport_phy_info.sys_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? TRUE : FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
