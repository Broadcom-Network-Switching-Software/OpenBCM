
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/stif_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/stif_diagnostic.h>



dnx_stif_db_t* dnx_stif_db_dummy = NULL;



int
dnx_stif_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID]),
        "dnx_stif_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        STIF_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_stif_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_stif_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID]),
        "dnx_stif_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_stif_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_instance_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance,
        dnx_instance_info_t*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_stif_db_instance_alloc");

    for(uint32 instance_idx_0 = 0;
         instance_idx_0 < nof_instances_to_alloc_0;
         instance_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0],
        dnx_instance_info_t,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_stif_db_instance_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance,
        "dnx_stif_db[%d]->instance",
        unit,
        nof_instances_to_alloc_1 * sizeof(dnx_instance_info_t) / sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance)+(nof_instances_to_alloc_1 * sizeof(dnx_instance_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[def_val_idx[0]][def_val_idx[1]].logical_port,
        -1);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_instance_logical_port_set(int unit, uint32 instance_idx_0, uint32 instance_idx_1, bcm_port_t logical_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance,
        instance_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0],
        instance_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0][instance_idx_1].logical_port,
        logical_port,
        bcm_port_t,
        0,
        "dnx_stif_db_instance_logical_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        STIF_MODULE_ID,
        &logical_port,
        "dnx_stif_db[%d]->instance[%d][%d].logical_port",
        unit, instance_idx_0, instance_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_INSTANCE_LOGICAL_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_instance_logical_port_get(int unit, uint32 instance_idx_0, uint32 instance_idx_1, bcm_port_t *logical_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance,
        instance_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        logical_port);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0],
        instance_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    *logical_port = ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0][instance_idx_1].logical_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        STIF_MODULE_ID,
        &((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[instance_idx_0][instance_idx_1].logical_port,
        "dnx_stif_db[%d]->instance[%d][%d].logical_port",
        unit, instance_idx_0, instance_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_INSTANCE_LOGICAL_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping,
        dnx_source_to_logical_port_mapping_t*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_stif_db_source_mapping_alloc");

    for(uint32 source_mapping_idx_0 = 0;
         source_mapping_idx_0 < nof_instances_to_alloc_0;
         source_mapping_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0],
        dnx_source_to_logical_port_mapping_t,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_stif_db_source_mapping_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping,
        "dnx_stif_db[%d]->source_mapping",
        unit,
        nof_instances_to_alloc_1 * sizeof(dnx_source_to_logical_port_mapping_t) / sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping)+(nof_instances_to_alloc_1 * sizeof(dnx_source_to_logical_port_mapping_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_SOURCE_MAPPING_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[def_val_idx[0]][def_val_idx[1]].first_port,
        -1);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[def_val_idx[0]][def_val_idx[1]].second_port,
        -1);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_first_port_set(int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t first_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping,
        source_mapping_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0],
        source_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0][source_mapping_idx_1].first_port,
        first_port,
        bcm_port_t,
        0,
        "dnx_stif_db_source_mapping_first_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        STIF_MODULE_ID,
        &first_port,
        "dnx_stif_db[%d]->source_mapping[%d][%d].first_port",
        unit, source_mapping_idx_0, source_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_SOURCE_MAPPING_FIRST_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_first_port_get(int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t *first_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping,
        source_mapping_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        first_port);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0],
        source_mapping_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    *first_port = ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0][source_mapping_idx_1].first_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        STIF_MODULE_ID,
        &((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0][source_mapping_idx_1].first_port,
        "dnx_stif_db[%d]->source_mapping[%d][%d].first_port",
        unit, source_mapping_idx_0, source_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_SOURCE_MAPPING_FIRST_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_second_port_set(int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t second_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping,
        source_mapping_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0],
        source_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0][source_mapping_idx_1].second_port,
        second_port,
        bcm_port_t,
        0,
        "dnx_stif_db_source_mapping_second_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        STIF_MODULE_ID,
        &second_port,
        "dnx_stif_db[%d]->source_mapping[%d][%d].second_port",
        unit, source_mapping_idx_0, source_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_SOURCE_MAPPING_SECOND_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_second_port_get(int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t *second_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        STIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        STIF_MODULE_ID,
        sw_state_roots_array[unit][STIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping,
        source_mapping_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        second_port);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0],
        source_mapping_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        STIF_MODULE_ID,
        ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        STIF_MODULE_ID);

    *second_port = ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0][source_mapping_idx_1].second_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        STIF_MODULE_ID,
        &((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[source_mapping_idx_0][source_mapping_idx_1].second_port,
        "dnx_stif_db[%d]->source_mapping[%d][%d].second_port",
        unit, source_mapping_idx_0, source_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        STIF_MODULE_ID,
        dnx_stif_db_info,
        DNX_STIF_DB_SOURCE_MAPPING_SECOND_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_stif_db_cbs dnx_stif_db = 	{
	
	dnx_stif_db_is_init,
	dnx_stif_db_init,
		{
		
		dnx_stif_db_instance_alloc,
			{
			
			dnx_stif_db_instance_logical_port_set,
			dnx_stif_db_instance_logical_port_get}
		}
	,
		{
		
		dnx_stif_db_source_mapping_alloc,
			{
			
			dnx_stif_db_source_mapping_first_port_set,
			dnx_stif_db_source_mapping_first_port_get}
		,
			{
			
			dnx_stif_db_source_mapping_second_port_set,
			dnx_stif_db_source_mapping_second_port_get}
		}
	}
;
#undef BSL_LOG_MODULE
