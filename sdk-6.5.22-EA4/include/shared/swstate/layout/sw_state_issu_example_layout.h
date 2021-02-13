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
#ifndef _SHR_sw_state_issu_example_H_
#define _SHR_sw_state_issu_example_H_


#define SW_STATE_ISSU_EXAMPLE_O1234567890123456789012345678901234567890123456789_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_MYUINT8_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_MYUINT16_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_MYARRAY_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_MYARRAY2D_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_MYUINT32_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_MYUINT64_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_ISSU_EXAMPLE_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_ISSU_EXAMPLE_O1234567890123456789012345678901234567890123456789_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_ISSU_EXAMPLE_MYUINT8_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_ISSU_EXAMPLE_MYUINT16_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_ISSU_EXAMPLE_MYARRAY_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_ISSU_EXAMPLE_MYARRAY2D_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_ISSU_EXAMPLE_MYUINT32_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_ISSU_EXAMPLE_MYUINT64_LAYOUT_NOF_MAX_LAYOUT_NODES) 

#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_o1234567890123456789012345678901234567890123456789_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_myUint8_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_myUint16_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_myArray_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_myArray2D_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#ifndef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_myUint32_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
#ifdef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
int sw_state_issu_example_myUint64_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#endif /* _SHR_sw_state_issu_example_H_ */
