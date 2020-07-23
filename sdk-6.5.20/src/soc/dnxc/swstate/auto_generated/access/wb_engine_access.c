
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/auto_generated/access/wb_engine_access.h>
#include <soc/dnxc/swstate/auto_generated/diagnostic/wb_engine_diagnostic.h>



sw_state_wb_engine_t* sw_state_wb_engine_dummy = NULL;



int
sw_state_wb_engine_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]),
        "sw_state_wb_engine[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        WB_ENGINE_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        sw_state_wb_engine_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sw_state_wb_engine_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]),
        "sw_state_wb_engine[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_wb_engine_info,
        SW_STATE_WB_ENGINE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(sw_state_wb_engine_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_alloc(int unit, uint32 engine_id, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        WB_ENGINE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        sw_state_wb_engine_buffer_t,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sw_state_wb_engine_buffer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        "sw_state_wb_engine[%d]->buffer[%d]",
        unit, engine_id,
        nof_instances_to_alloc_1 * sizeof(sw_state_wb_engine_buffer_t) / sizeof(*((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_wb_engine_info,
        SW_STATE_WB_ENGINE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_set(int unit, uint32 engine_id, uint32 buffer_id, uint32 instance_idx_0, DNX_SW_STATE_BUFF instance)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        instance_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        WB_ENGINE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance[instance_idx_0],
        instance,
        DNX_SW_STATE_BUFF,
        0,
        "sw_state_wb_engine_buffer_instance_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        WB_ENGINE_MODULE_ID,
        &instance,
        "sw_state_wb_engine[%d]->buffer[%d][%d].instance[%d]",
        unit, engine_id, buffer_id, instance_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_wb_engine_info,
        SW_STATE_WB_ENGINE_BUFFER_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_get(int unit, uint32 engine_id, uint32 buffer_id, uint32 instance_idx_0, DNX_SW_STATE_BUFF *instance)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        instance_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        WB_ENGINE_MODULE_ID);

    *instance = ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance[instance_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        WB_ENGINE_MODULE_ID,
        &((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance[instance_idx_0],
        "sw_state_wb_engine[%d]->buffer[%d][%d].instance[%d]",
        unit, engine_id, buffer_id, instance_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_wb_engine_info,
        SW_STATE_WB_ENGINE_BUFFER_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_alloc(int unit, uint32 engine_id, uint32 buffer_id, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        WB_ENGINE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sw_state_wb_engine_buffer_instance_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        "sw_state_wb_engine[%d]->buffer[%d][%d].instance",
        unit, engine_id, buffer_id,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_wb_engine_info,
        SW_STATE_WB_ENGINE_BUFFER_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_memread(int unit, uint32 engine_id, uint32 buffer_id, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        (_offset + _len - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        WB_ENGINE_MODULE_ID);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        _offset,
        _len,
        0,
        "sw_state_wb_engine_buffer_instance_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        "sw_state_wb_engine[%d]->buffer[%d][%d].instance",
        unit, engine_id, buffer_id, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_wb_engine_info,
        SW_STATE_WB_ENGINE_BUFFER_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_memwrite(int unit, uint32 engine_id, uint32 buffer_id, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        (_offset + _len - 1));

    DNX_SW_STATE_MEMWRITE(
        unit,
        WB_ENGINE_MODULE_ID,
        _src,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        _offset,
        _len,
        0,
        "sw_state_wb_engine_buffer_instance_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_memcmp(int unit, uint32 engine_id, uint32 buffer_id, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        (_offset + _len - 1));

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        _offset,
        _len,
        cmp_result,
        0,
        "sw_state_wb_engine_buffer_instance_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_memset(int unit, uint32 engine_id, uint32 buffer_id, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        WB_ENGINE_MODULE_ID,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        WB_ENGINE_MODULE_ID,
        sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        engine_id,
        SOC_WB_ENGINE_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id],
        buffer_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        (_offset + _len - 1));

    DNX_SW_STATE_MEMSET(
        unit,
        WB_ENGINE_MODULE_ID,
        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[engine_id][buffer_id].instance,
        _offset,
        _value,
        _len,
        0,
        "sw_state_wb_engine_buffer_instance_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}





sw_state_wb_engine_cbs sw_state_wb_engine = 	{
	
	sw_state_wb_engine_is_init,
	sw_state_wb_engine_init,
		{
		
		sw_state_wb_engine_buffer_alloc,
			{
			
			sw_state_wb_engine_buffer_instance_set,
			sw_state_wb_engine_buffer_instance_get,
			sw_state_wb_engine_buffer_instance_alloc,
			sw_state_wb_engine_buffer_instance_memread,
			sw_state_wb_engine_buffer_instance_memwrite,
			sw_state_wb_engine_buffer_instance_memcmp,
			sw_state_wb_engine_buffer_instance_memset}
		}
	}
;
#endif  
#undef BSL_LOG_MODULE
