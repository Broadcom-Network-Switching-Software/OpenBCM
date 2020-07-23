/* 
*  
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _SOC_DPP_WB_ENGINE_DEFS_H_
#define _SOC_DPP_WB_ENGINE_DEFS_H_

#include <soc/wb_engine.h>
extern soc_port_unit_info_t        ports_unit_info[SOC_MAX_NUM_DEVICES];
extern soc_port_core_info_t        core_info[SOC_MAX_NUM_DEVICES][SOC_DPP_DEFS_MAX(NOF_CORES)];

#define SOC_DPP_OLD_BUFFERS_DECS\
         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP

#define COMPLEX_DS_ARR_MEM_ALLOCATOR(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_MEM_ALLOCATOR_INNER_VARS_NUM - 1)


#define COMPLEX_DS_OCC_BM(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_OCC_BM_INNER_VARS_NUM - 1)
#define COMPLEX_DS_PAT_TREE_AGREGATION(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_PAT_TREE_AGREGATION_INNER_VARS_NUM - 1)



#define SOC_DPP_WB_ENGINE_VAR_NONE -1

#define SOC_DPP_WB_ENGINE (SOC_WB_ENGINE_PRIMARY)

typedef enum
{
    

    
    WB_ENGINE_MEM_ALLOCATOR_FREE_LIST, 
    WB_ENGINE_MEM_ALLOCATOR_CACHE_ENABLED, 
    WB_ENGINE_MEM_ALLOCATOR_FREE_LIST_CACHE, 
    WB_ENGINE_MEM_ALLOCATOR_NOF_UPDATES, 

    
    WB_ENGINE_MEM_ALLOCATOR_ARRAY,
    WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW,
    WB_ENGINE_MEM_ALLOCATOR_ARRAY_CACHE,
    WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE,
    WB_ENGINE_MEM_ALLOCATOR_UPDATE_INDEXES,

    WB_ENGINE_MEM_ALLOCATOR_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_MEM_ALLOCATOR_INNER_VAR;

typedef enum
{
    

    
    WB_ENGINE_GROUP_MEM_LL_CACHE_ENABLED, 

    
    WB_ENGINE_GROUP_MEM_LL_GROUPS,
    WB_ENGINE_GROUP_MEM_LL_MEMBERS,
    WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE,
    WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE,

    WB_ENGINE_GROUP_MEM_LL_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_GROUP_MEM_LL_INNER_VAR;

typedef enum
{
    

    
    WB_ENGINE_OCC_BM_CACHE_ENABLED, 

    
    WB_ENGINE_OCC_BM_LEVELS_BUFFER, 
    WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, 

    WB_ENGINE_OCC_BM_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_OCC_BM_INNER_VAR;


typedef enum
{
    

    
    

    
    WB_ENGINE_PAT_TREE_AGREGATION_ROOT, 
    WB_ENGINE_PAT_TREE_AGREGATION_CACHE_ENABLED, 
    WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, 
    WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
    WB_ENGINE_PAT_TREE_AGREGATION_CURRENT_NODE_PLACE, 

    
    WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY, 
    COMPLEX_DS_OCC_BM(WB_ENGINE_PAT_TREE_AGREGATION_MEMORY_USE), 
    WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, 

    WB_ENGINE_PAT_TREE_AGREGATION_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_PAT_TREE_AGREGATION_INNER_VAR;




#define SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(_buff, _buff_string, _upgrade_func, _version)\
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, NULL, _version, 0x0 , SOC_WB_ENGINE_PRE_RELEASE)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);




#define SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _inner_jump, _version_added) \
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, _inner_jump, _version_added, 0xff, NULL)









#ifdef BCM_ARAD_SUPPORT
#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(_var, _var_string, _buffer, _mem_allocator, _entry_size, _nof_entries, _support_caching, _version_added) \
    if(_buffer == buffer_id && _support_caching == TRUE )               \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_FREE_LIST_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_PTR), \
                                  &(_mem_allocator.arr_mem_allocator_data.free_list_cache), \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_ARRAY_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY), \
                                  _mem_allocator.arr_mem_allocator_data.array_cache, \
                                  _nof_entries,                         \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(uint32),                       \
                                  _mem_allocator.arr_mem_allocator_data.mem_shadow_cache, \
                                  _entry_size * _nof_entries,           \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_UPDATE_INDEXES, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY), \
                                  _mem_allocator.arr_mem_allocator_data.update_indexes, \
                                  _nof_entries,                         \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_FREE_LIST, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_PTR),    \
                              &(_mem_allocator.arr_mem_allocator_data.free_list), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_CACHE_ENABLED, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_mem_allocator.arr_mem_allocator_data.cache_enabled), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_NOF_UPDATES, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint32),                           \
                              &(_mem_allocator.arr_mem_allocator_data.nof_updates), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_ARRAY,     \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY),  \
                              _mem_allocator.arr_mem_allocator_data.array, \
                              _nof_entries,                             \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint32),                           \
                              _mem_allocator.arr_mem_allocator_data.mem_shadow, \
                              _entry_size * _nof_entries,               \
                              _version_added)


#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(_var, _var_string, _buffer, _group_mem_ll, _nof_groups, _nof_elements, _support_caching, _version_added) \
    if(_buffer == buffer_id && _support_caching == TRUE )               \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY), \
                                  _group_mem_ll->group_members_data.groups_cache, \
                                  _nof_groups,                          \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY), \
                                  _group_mem_ll->group_members_data.members_cache, \
                                  _nof_elements,                        \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_CACHE_ENABLED, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_group_mem_ll->group_members_data.cache_enabled), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_GROUPS,     \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY), \
                              _group_mem_ll->group_members_data.groups, \
                              _nof_groups,                              \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_MEMBERS,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY), \
                              _group_mem_ll->group_members_data.members, \
                              _nof_elements,                            \
                              _version_added)


#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(_var, _var_string, _buffer, _occ_bm, _array_size, _support_cache, _version_added) \
    if(_buffer == buffer_id && _support_cache == TRUE )                 \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(uint8),                        \
                                  _occ_bm->levels_cache_buffer,         \
                                  _array_size,                          \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_OCC_BM_CACHE_ENABLED,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_occ_bm->cache_enabled),                \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_OCC_BM_LEVELS_BUFFER,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              _occ_bm->levels_buffer,                   \
                              _array_size,                              \
                              _version_added);                          

#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_PAT_TREE_AGREGATION(_var, _var_string, _buffer, _pat_tree_0, _nof_pat_trees, _pat_tree_size, \
                                                             _pat_tree_support_cache, _occ_bm_array_size, _occ_bm_support_cache, _version_added) \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_ROOT, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_PLACE), \
                                          &(_pat_tree_0.pat_tree_data.root), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_ENABLED, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(uint8),                \
                                          &(_pat_tree_0.pat_tree_data.cache_enabled), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_PLACE), \
                                          &(_pat_tree_0.pat_tree_data.root_cache), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_KEY), \
                                          &(_pat_tree_0.pat_tree_data.cache_change_head), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_CURRENT_NODE_PLACE, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_PLACE), \
                                          &(_pat_tree_0.pat_tree_data.current_node_place), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(SOC_SAND_PAT_TREE_NODE),           \
                              _pat_tree_0.pat_tree_data.tree_memory,    \
                              _pat_tree_size,                           \
                              _version_added);                          \
    if(_buffer == buffer_id && _pat_tree_support_cache == TRUE )        \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(SOC_SAND_PAT_TREE_NODE),       \
                                  _pat_tree_0.pat_tree_data.tree_memory_cache, \
                                  _pat_tree_size,                       \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(_var + WB_ENGINE_PAT_TREE_AGREGATION_MEMORY_USE, \
                                            _var_string,                \
                                            _buffer,                    \
                                            _pat_tree_0.pat_tree_data.memory_use_ptr, \
                                            _occ_bm_array_size,         \
                                            _occ_bm_support_cache,      \
                                            _version_added)                           


#endif 



#define SOC_DPP_ADD_ARAD_BUFFS_DEPRECATED_CODE \
                                                                                                                                     \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO, "ipv4_info" , NULL, VERSION(1));                                                   \
                                                                                                                                                                               \
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {                                                                             \
        SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP, "rif to lif group map" , NULL, VERSION(1));                               \
    }
    
#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET\
    \
    if (SOC_WARM_BOOT(unit) && buffer_is_dynamic) {\
        rv = soc_wb_engine_buffer_dynamic_vars_state_get(unit, SOC_DPP_WB_ENGINE, buffer_id, &buffer_header);\
        SOCDNX_IF_ERR_EXIT(rv);\
    }
#else 
#define SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET\
    \
    if (SOC_WARM_BOOT(unit) && buffer_is_dynamic) {\
        rv = SOC_E_NONE;\
        SOCDNX_IF_ERR_EXIT(rv);\
    }
#endif 


#define SOC_DPP_ADD_ARAD_VARS_DEPRECATED_CODE \
                                                                                                                                                                                                                    \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_1,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_1",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_2,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_2",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_3,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_3",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_4,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_4",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_5,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_5",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_6,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_6",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                    \
    if (buffer_id == SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO &&                                                                                                                                                                                      \
        PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.flags & ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG)                                                                                                                                                               \
    {                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                          \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_1,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_1",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_2,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_2",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_3,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_3",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_4,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_4",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_5,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_5",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_PAT_TREE_AGREGATION(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_LPMS,                                                                                                                                               \
                                                         "ipv4_info_lpms",                                                                                                                                                                                    \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0],                                                                                                                                              \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.nof_lpms,                                                                                                                                             \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].tree_size,                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].support_cache,                                                                                                                                \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].pat_tree_data.memory_use_ptr->buffer_size,                                                                                                        \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].pat_tree_data.memory_use_ptr->support_cache,                                                                                                      \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                             \
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {                                                                                                                                                            \
      if (buffer_id == SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP) {                                                                                                                                                                               \
        SOC_SAND_GROUP_MEM_LL_INFO *group_info_ptr = &(PP_SW_DB_DEVICE->rif_to_lif_group_map->group_info);                                                                                                                                                    \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(                                                                                                                                                                                                     \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MEM_LL,                                                                                                                                      \
                                                         "rif_to_lif_group_mem_ll",                                                                                                                                                                           \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP,                                                                                                                                               \
                                                         group_info_ptr,                                                                                                                                                                                      \
                                                         group_info_ptr->nof_groups,                                                                                                                                                                          \
                                                         group_info_ptr->nof_elements,                                                                                                                                                                        \
                                                         group_info_ptr->support_caching,                                                                                                                                                                     \
                                                         VERSION(1));                                                                                                                                                                                         \
      }                                                                                                                                                                                                                                                       \
    }

#ifdef BCM_ARAD_SUPPORT
#define SOC_DPP_ALL_ARAD_VARS_DEPRECATED_CODE\
    SOC_DPP_ADD_ARAD_VARS_DEPRECATED_CODE 
#else 
#define SOC_DPP_ALL_ARAD_VARS_DEPRECATED_CODE
#endif 



#define SOC_DPP_WB_ENGINE_DEPRECATED_CODE\
    \
    \
    SOC_DPP_ADD_ARAD_BUFFS_DEPRECATED_CODE;      \
    \
\
    SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET;\
\
    \
    \
    SOC_DPP_ALL_ARAD_VARS_DEPRECATED_CODE;


#endif 
