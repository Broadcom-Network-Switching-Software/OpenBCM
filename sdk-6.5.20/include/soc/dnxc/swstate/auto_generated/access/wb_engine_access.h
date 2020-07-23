
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __WB_ENGINE_ACCESS_H__
#define __WB_ENGINE_ACCESS_H__

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/auto_generated/types/wb_engine_types.h>
#include <soc/wb_engine.h>



typedef int (*sw_state_wb_engine_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*sw_state_wb_engine_init_cb)(
    int unit);


typedef int (*sw_state_wb_engine_buffer_alloc_cb)(
    int unit, uint32 engine_id, uint32 nof_instances_to_alloc_1);


typedef int (*sw_state_wb_engine_buffer_instance_set_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, uint32 instance_idx_0, DNX_SW_STATE_BUFF instance);


typedef int (*sw_state_wb_engine_buffer_instance_get_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, uint32 instance_idx_0, DNX_SW_STATE_BUFF *instance);


typedef int (*sw_state_wb_engine_buffer_instance_alloc_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, uint32 _nof_bytes_to_alloc);


typedef int (*sw_state_wb_engine_buffer_instance_memread_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, uint8 *_dst, uint32 _offset, size_t _len);


typedef int (*sw_state_wb_engine_buffer_instance_memwrite_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, const uint8 *_src, uint32 _offset, size_t _len);


typedef int (*sw_state_wb_engine_buffer_instance_memcmp_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result);


typedef int (*sw_state_wb_engine_buffer_instance_memset_cb)(
    int unit, uint32 engine_id, uint32 buffer_id, uint32 _offset, size_t _len, int _value);




typedef struct {
    sw_state_wb_engine_buffer_instance_set_cb set;
    sw_state_wb_engine_buffer_instance_get_cb get;
    sw_state_wb_engine_buffer_instance_alloc_cb alloc;
    sw_state_wb_engine_buffer_instance_memread_cb memread;
    sw_state_wb_engine_buffer_instance_memwrite_cb memwrite;
    sw_state_wb_engine_buffer_instance_memcmp_cb memcmp;
    sw_state_wb_engine_buffer_instance_memset_cb memset;
} sw_state_wb_engine_buffer_instance_cbs;


typedef struct {
    sw_state_wb_engine_buffer_alloc_cb alloc;
    
    sw_state_wb_engine_buffer_instance_cbs instance;
} sw_state_wb_engine_buffer_cbs;


typedef struct {
    sw_state_wb_engine_is_init_cb is_init;
    sw_state_wb_engine_init_cb init;
    
    sw_state_wb_engine_buffer_cbs buffer;
} sw_state_wb_engine_cbs;





extern sw_state_wb_engine_cbs sw_state_wb_engine;
#endif  

#endif 
