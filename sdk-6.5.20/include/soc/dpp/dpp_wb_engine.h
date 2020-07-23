/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 
#ifndef _SOC_DPP_WB_ENGINE_H_
#define _SOC_DPP_WB_ENGINE_H_

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/port_sw_db.h>

#endif 

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/port_map.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/TMC/tmc_api_general.h>
#endif 

#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/dpp_wb_engine_defs.h>
#include <shared/bitop.h>
#include <bcm_int/dpp/ipmc.h>


#define SOC_DPP_WB_ENGINE_DECLARATIONS_BEGIN\
    switch (buffer_id) {\
        case SOC_DPP_OLD_BUFFERS_DECS:\
            \
            break;\
        case SOC_DPP_WB_ENGINE_NOF_BUFFERS:\
            return SOC_E_INTERNAL;

#define SOC_DPP_WB_ENGINE_DECLARATIONS_END\
            break;\
        default:\
            return SOC_E_INTERNAL;\
    }












#define SOC_DPP_WB_ENGINE_ADD_BUFF(_buff, _buff_string, _upgrade_func, _version, _is_added_after_release)\
        break;\
    case _buff:\
        SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, NULL, _version, 0x1 , _is_added_after_release)\
        SOCDNX_IF_ERR_EXIT(rv);\
        SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET;


#define SOC_DPP_WB_ENGINE_ADD_BUFF_WITH_MEMORY_DUPLICATIONS(_buff, _buff_string, _upgrade_func, _version, _is_added_after_release)\
        break;\
    case _buff:\
        SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, NULL, _version, 0x0 , _is_added_after_release)\
        SOCDNX_IF_ERR_EXIT(rv);\
        SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET;


#define SOC_DPP_WB_ENGINE_ADD_VAR(_var, _var_string, _buffer, _data_size, _version_added)\
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, NULL, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_ARR(_var, _var_string, _buffer, _data_size, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, NULL, _arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_2D_ARR(_var, _var_string, _buffer, _data_size, _outer_arr_length, _inner_arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, NULL, _outer_arr_length, _inner_arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);











#define SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, _var_id, _data_ptr, _outer_idx, _inner_idx)\
    SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _outer_idx, _inner_idx)

#define SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, _var_id, _data_ptr, _outer_idx, _inner_idx)\
    SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _outer_idx, _inner_idx)

#define SOC_DPP_WB_ENGINE_SET_ARR(unit, _var_id, _data_ptr, _idx)\
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _idx)

#define SOC_DPP_WB_ENGINE_GET_ARR(unit, _var_id, _data_ptr, _idx)\
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _idx)

#define SOC_DPP_WB_ENGINE_SET_VAR(unit, _var_id, _data_ptr)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr)

#define SOC_DPP_WB_ENGINE_GET_VAR(unit, _var_id, _data_ptr)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr)



#define SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, _var_id, _data_ptr, _inner_idx, _cpy_length) \
    soc_wb_engine_array_range_set_or_get(unit, SOC_WB_ENGINE_PRIMARY, (_var_id), (_inner_idx), (_cpy_length), 1, (uint8 *)(_data_ptr))

#define SOC_DPP_WB_ENGINE_MEMCPY_ARR_GET(unit, _var_id, _data_ptr, _inner_idx, _cpy_length) \
    soc_wb_engine_array_range_set_or_get(unit, SOC_WB_ENGINE_PRIMARY, (_var_id), (_inner_idx), (_cpy_length), 0, (uint8 *)(_data_ptr))

#define SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, _var_id, _val) \
    soc_wb_engine_array_set(unit, SOC_WB_ENGINE_PRIMARY, (_var_id), (_val))

#ifdef BCM_ARAD_SUPPORT


int soc_dpp_wb_engine_state_init_port(int unit);

int soc_dpp_wb_engine_Arad_pp_sw_db_get(ARAD_PP_SW_DB *sw_db);

#endif 



typedef struct _bcm_dpp_vlan_translate_tag_action_s {
    bcm_vlan_action_t vid_action;
    bcm_vlan_action_t pcp_action;
    bcm_vlan_tpid_action_t tpid_action;
    uint16 tpid_val;
} _bcm_dpp_vlan_translate_tag_action_t;

typedef struct _bcm_dpp_vlan_translate_action_s {
    _bcm_dpp_vlan_translate_tag_action_t outer;
    _bcm_dpp_vlan_translate_tag_action_t inner;
    uint32 packet_is_tagged_after_eve;
} _bcm_dpp_vlan_translate_action_t;


int soc_dpp_wb_engine_init_buffer(int unit, int buffer_id);
int soc_dpp_wb_engine_deinit(int unit);
int soc_dpp_wb_engine_init(int unit);
int soc_dpp_wb_engine_sync(int unit);










#define DPP_WB_ENGINE_NOF_LIFS (SOC_DPP_CONFIG(unit))->l2.nof_lifs




#ifdef BCM_ARAD_SUPPORT
extern ARAD_SW_DB Arad_sw_db;
  
#ifdef BCM_WARM_BOOT_SUPPORT   

#endif                                                            
#endif                                                              
  

typedef enum
{
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO                 =  8,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP            = 42,
  

    SOC_DPP_WB_ENGINE_NOF_BUFFERS                                    

} SOC_DPP_WB_ENGINE_BUFFER;



typedef enum
{
    
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_1),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_2),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_3),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_4),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_5),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_6),

    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_1),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_2),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_3),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_4),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_5),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MEM_LL),

    COMPLEX_DS_PAT_TREE_AGREGATION(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_LPMS),

    
    SOC_DPP_WB_ENGINE_VAR_NOF_VARS

} SOC_DPP_WB_ENGINE_VAR;

#endif 
