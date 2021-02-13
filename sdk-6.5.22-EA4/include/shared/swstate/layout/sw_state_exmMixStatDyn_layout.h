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
#ifndef _SHR_sw_state_exmMixStatDyn_H_
#define _SHR_sw_state_exmMixStatDyn_H_


#define SW_STATE_EXMMIXSTATDYN_STATICARRDYNPTR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_EXMMIXSTATDYN_STATICARRDYNARR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_EXMMIXSTATDYN_STATICARRDYNARRPTR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_EXMMIXSTATDYN_STATICARRARRDYNARRARR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_EXMMIXSTATDYN_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_EXMMIXSTATDYN_STATICARRDYNPTR_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_EXMMIXSTATDYN_STATICARRDYNARR_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_EXMMIXSTATDYN_STATICARRDYNARRPTR_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_EXMMIXSTATDYN_STATICARRARRDYNARRARR_LAYOUT_NOF_MAX_LAYOUT_NODES) 

#ifdef _SHR_SW_STATE_EXM
int sw_state_exmMixStatDyn_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_exmMixStatDyn_staticArrDynPtr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_exmMixStatDyn_staticArrDynArr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_exmMixStatDyn_staticArrDynArrPtr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_exmMixStatDyn_staticArrArrDynArrArr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 

#endif /* _SHR_sw_state_exmMixStatDyn_H_ */
