/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
*/
#include <sal/core/libc.h>
#include <shared/swstate/sw_state.h>
#include <shared/error.h>
#include <shared/swstate/sw_state_defs.h>
#include <shared/swstate/sw_state_utils.h>
#include <shared/bsl.h>
#include <shared/swstate/layout/sw_state_layout.h>
#include <shared/swstate/layout/sw_state_dpp_soc_arad_pp_frwrd_ip_layout.h>

#ifdef _ERR_MSG_MODULE_NAME  
  #error "_ERR_MSG_MODULE_NAME redefined"  
#endif  
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_arad_pp_frwrd_ip_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

   int younger_brother_node_id = 0; 
   int offset = 0; 
   ARAD_FRWRD_IP* dummy_struct; 
   SOC_INIT_FUNC_DEFS;
   /* alloc dummy struct */ 
   dummy_struct = sal_alloc(sizeof(ARAD_FRWRD_IP),"ARAD_FRWRD_IP"); 

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "frwrd_ip",             /* name */         
                                   sizeof(ARAD_FRWRD_IP),    /* size of the element's type*/          
                                   1,                 /* nof pointers */ 
                                   "ARAD_FRWRD_IP",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

   /* add node */ 
   _SOC_IF_ERR_EXIT(sw_state_dpp_soc_arad_pp_frwrd_ip_route_key_to_entry_id_layout_node_create(unit, &younger_brother_node_id, next_free_node_id));
   /* connect node (1st child) to its parent */ 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_child(unit, *root_node_id, younger_brother_node_id)); 
   /* update offset */ 
   offset = ((uint8*) &(dummy_struct->route_key_to_entry_id)) - (( uint8*) dummy_struct); 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_update_offset(unit, younger_brother_node_id, offset)); 

#if defined(INCLUDE_KBP)
   /* add node */ 
   _SOC_IF_ERR_EXIT(sw_state_dpp_soc_arad_pp_frwrd_ip_location_tbl_layout_node_create(unit, &younger_brother_node_id, next_free_node_id));
   /* connect node (1st child) to its parent */ 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_child(unit, *root_node_id, younger_brother_node_id)); 
   /* update offset */ 
   offset = ((uint8*) &(dummy_struct->location_tbl)) - (( uint8*) dummy_struct); 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_update_offset(unit, younger_brother_node_id, offset)); 
#endif /* defined(INCLUDE_KBP)*/ 

#if defined(INCLUDE_KBP)
   /* add node */ 
   _SOC_IF_ERR_EXIT(sw_state_dpp_soc_arad_pp_frwrd_ip_kbp_cache_mode_layout_node_create(unit, &younger_brother_node_id, next_free_node_id));
   /* connect node (1st child) to its parent */ 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_child(unit, *root_node_id, younger_brother_node_id)); 
   /* update offset */ 
   offset = ((uint8*) &(dummy_struct->kbp_cache_mode)) - (( uint8*) dummy_struct); 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_update_offset(unit, younger_brother_node_id, offset)); 
#endif /* defined(INCLUDE_KBP)*/ 

#if defined(INCLUDE_KBP)
   /* add node */ 
   _SOC_IF_ERR_EXIT(sw_state_dpp_soc_arad_pp_frwrd_ip_kbp_diag_flag_layout_node_create(unit, &younger_brother_node_id, next_free_node_id));
   /* connect node (1st child) to its parent */ 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_child(unit, *root_node_id, younger_brother_node_id)); 
   /* update offset */ 
   offset = ((uint8*) &(dummy_struct->kbp_diag_flag)) - (( uint8*) dummy_struct); 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_update_offset(unit, younger_brother_node_id, offset)); 
#endif /* defined(INCLUDE_KBP)*/ 

   SOC_EXIT;
 exit:
   /* free dummy struct */ 
   sal_free(dummy_struct); 
   SOC_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_arad_pp_frwrd_ip_route_key_to_entry_id_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

   SOC_INIT_FUNC_DEFS;

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "route_key_to_entry_id",             /* name */         
                                   sizeof(SOC_SAND_HASH_TABLE_PTR),    /* size of the element's type*/          
                                   0,                 /* nof pointers */ 
                                   "SOC_SAND_HASH_TABLE_PTR",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

   SOC_EXIT;
 exit:
   SOC_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_KBP)
int sw_state_dpp_soc_arad_pp_frwrd_ip_location_tbl_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

#if (defined(INCLUDE_KBP))
   int younger_brother_node_id = 0; 
   int offset = 0; 
   ARAD_SW_KBP_HANDLE* dummy_struct; 
#endif /* (defined(INCLUDE_KBP)) */ 
   SOC_INIT_FUNC_DEFS;
   /* alloc dummy struct */ 
   dummy_struct = sal_alloc(sizeof(ARAD_SW_KBP_HANDLE),"ARAD_SW_KBP_HANDLE"); 

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "location_tbl",             /* name */         
                                   sizeof(ARAD_SW_KBP_HANDLE),    /* size of the element's type*/          
                                   1,                 /* nof pointers */ 
                                   "ARAD_SW_KBP_HANDLE",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

#if defined(INCLUDE_KBP)
   /* add node */ 
   _SOC_IF_ERR_EXIT(sw_state_dpp_soc_arad_pp_frwrd_ip_location_tbl_db_entry_layout_node_create(unit, &younger_brother_node_id, next_free_node_id));
   /* connect node (1st child) to its parent */ 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_child(unit, *root_node_id, younger_brother_node_id)); 
   /* update offset */ 
   offset = ((uint8*) &(dummy_struct->db_entry)) - (( uint8*) dummy_struct); 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_update_offset(unit, younger_brother_node_id, offset)); 
#endif /* defined(INCLUDE_KBP)*/ 

   SOC_EXIT;
 exit:
#if (defined(INCLUDE_KBP))
   /* free dummy struct */ 
   sal_free(dummy_struct); 
#endif /* (defined(INCLUDE_KBP)) */ 
   SOC_FUNC_RETURN;
}

#endif /* defined(INCLUDE_KBP)*/ 
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_KBP)
int sw_state_dpp_soc_arad_pp_frwrd_ip_location_tbl_db_entry_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

   SOC_INIT_FUNC_DEFS;

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "db_entry",             /* name */         
                                   sizeof(kbp_entry_handle),    /* size of the element's type*/          
                                   0,                 /* nof pointers */ 
                                   "kbp_entry_handle",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

   SOC_EXIT;
 exit:
   SOC_FUNC_RETURN;
}

#endif /* defined(INCLUDE_KBP)*/ 
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_KBP)
int sw_state_dpp_soc_arad_pp_frwrd_ip_kbp_cache_mode_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

   SOC_INIT_FUNC_DEFS;

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "kbp_cache_mode",             /* name */         
                                   sizeof(uint8),    /* size of the element's type*/          
                                   0,                 /* nof pointers */ 
                                   "uint8",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

   SOC_EXIT;
 exit:
   SOC_FUNC_RETURN;
}

#endif /* defined(INCLUDE_KBP)*/ 
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_KBP)
int sw_state_dpp_soc_arad_pp_frwrd_ip_kbp_diag_flag_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

#if (defined(INCLUDE_KBP))
   int younger_brother_node_id = 0; 
   int offset = 0; 
   ARAD_SW_KBP_DIAG_FLAGS* dummy_struct; 
#endif /* (defined(INCLUDE_KBP)) */ 
   SOC_INIT_FUNC_DEFS;
   /* alloc dummy struct */ 
   dummy_struct = sal_alloc(sizeof(ARAD_SW_KBP_DIAG_FLAGS),"ARAD_SW_KBP_DIAG_FLAGS"); 

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "kbp_diag_flag",             /* name */         
                                   sizeof(ARAD_SW_KBP_DIAG_FLAGS),    /* size of the element's type*/          
                                   0,                 /* nof pointers */ 
                                   "ARAD_SW_KBP_DIAG_FLAGS",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

#if defined(INCLUDE_KBP)
   /* add node */ 
   _SOC_IF_ERR_EXIT(sw_state_dpp_soc_arad_pp_frwrd_ip_kbp_diag_flag_entry_flag_layout_node_create(unit, &younger_brother_node_id, next_free_node_id));
   /* connect node (1st child) to its parent */ 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_child(unit, *root_node_id, younger_brother_node_id)); 
   /* update offset */ 
   offset = ((uint8*) &(dummy_struct->entry_flag)) - (( uint8*) dummy_struct); 
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_update_offset(unit, younger_brother_node_id, offset)); 
#endif /* defined(INCLUDE_KBP)*/ 

   SOC_EXIT;
 exit:
#if (defined(INCLUDE_KBP))
   /* free dummy struct */ 
   sal_free(dummy_struct); 
#endif /* (defined(INCLUDE_KBP)) */ 
   SOC_FUNC_RETURN;
}

#endif /* defined(INCLUDE_KBP)*/ 
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_KBP)
int sw_state_dpp_soc_arad_pp_frwrd_ip_kbp_diag_flag_entry_flag_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id){  

   SOC_INIT_FUNC_DEFS;

   /* update current root node */  
   *root_node_id = *next_free_node_id; 
   /* add node */    
   _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_node_set(unit, 
                                   *root_node_id,                 /* node id var */      
                                   "entry_flag",             /* name */         
                                   sizeof(uint8),    /* size of the element's type*/          
                                   0,                 /* nof pointers */ 
                                   "uint8",              /* param type */
                                   0,                 /* array size 0 */ 
                                   0));             /* array size 1 */ 
   /* update next free node id */ 
    SW_STATE_NODE_ID_CHECK(unit, ++(*next_free_node_id));  

   SOC_EXIT;
 exit:
   SOC_FUNC_RETURN;
}

#endif /* defined(INCLUDE_KBP)*/ 
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
