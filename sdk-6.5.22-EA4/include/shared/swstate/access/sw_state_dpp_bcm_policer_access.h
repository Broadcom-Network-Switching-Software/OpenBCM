/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 * search for 'sw_state_cbs_t' for the root of the struct
 */

#ifndef _SHR_SW_STATE_DPP_BCM_POLICER_ACCESS_H_
#define _SHR_SW_STATE_DPP_BCM_POLICER_ACCESS_H_

/********************************* access calbacks definitions *************************************/
/* this set of callbacks, are the callbacks used in the access calbacks struct 'sw_state_cbs_t' to */
/* access the data in 'sw_state_t'.                                                                */
/* the calbacks are inserted into the access struct by 'sw_state_access_cb_init'.                  */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_set */
typedef int (*sw_state_dpp_bcm_policer_set_cb)(
    int unit, int policer_idx_0, CONST _dpp_policer_state_t *policer);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_get */
typedef int (*sw_state_dpp_bcm_policer_get_cb)(
    int unit, int policer_idx_0, _dpp_policer_state_t *policer);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_alloc */
typedef int (*sw_state_dpp_bcm_policer_alloc_cb)(
    int unit, int nof_instances_to_alloc);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_is_allocated */
typedef int (*sw_state_dpp_bcm_policer_is_allocated_cb)(
    int unit, uint8 *is_allocated);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_free */
typedef int (*sw_state_dpp_bcm_policer_free_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_verify */
typedef int (*sw_state_dpp_bcm_policer_verify_cb)(
    int unit, int policer_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_policer_group_set */
typedef int (*sw_state_dpp_bcm_policer_policer_group_set_cb)(
    int unit, int policer_idx_0, int policer_group_idx_0, CONST _dpp_policer_group_info_t *policer_group);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_policer_group_get */
typedef int (*sw_state_dpp_bcm_policer_policer_group_get_cb)(
    int unit, int policer_idx_0, int policer_group_idx_0, _dpp_policer_group_info_t *policer_group);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_policer_group_verify */
typedef int (*sw_state_dpp_bcm_policer_policer_group_verify_cb)(
    int unit, int policer_idx_0, int policer_group_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_policer_group_mode_set */
typedef int (*sw_state_dpp_bcm_policer_policer_group_mode_set_cb)(
    int unit, int policer_idx_0, int policer_group_idx_0, bcm_policer_group_mode_t mode);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_policer_group_mode_get */
typedef int (*sw_state_dpp_bcm_policer_policer_group_mode_get_cb)(
    int unit, int policer_idx_0, int policer_group_idx_0, bcm_policer_group_mode_t *mode);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_policer_policer_group_mode_verify */
typedef int (*sw_state_dpp_bcm_policer_policer_group_mode_verify_cb)(
    int unit, int policer_idx_0, int policer_group_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

/*********************************** access calbacks struct ****************************************/
/* this set of structs, rooted at 'sw_state_cbs_t' define the access layer for the entire SW state.*/
/* use this tree to alloc/free/set/get fields in the sw state rooted at 'sw_state_t'.              */
/* NOTE: 'sw_state_t' data should not be accessed directly.                                        */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_policer_policer_group_mode_cbs_s {
    sw_state_dpp_bcm_policer_policer_group_mode_set_cb set;
    sw_state_dpp_bcm_policer_policer_group_mode_get_cb get;
    sw_state_dpp_bcm_policer_policer_group_mode_verify_cb verify;
} sw_state_dpp_bcm_policer_policer_group_mode_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_policer_policer_group_cbs_s {
    sw_state_dpp_bcm_policer_policer_group_set_cb set;
    sw_state_dpp_bcm_policer_policer_group_get_cb get;
    sw_state_dpp_bcm_policer_policer_group_verify_cb verify;
    sw_state_dpp_bcm_policer_policer_group_mode_cbs_t mode;
} sw_state_dpp_bcm_policer_policer_group_cbs_t;

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_policer_cbs_s {
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_policer_set_cb set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_policer_get_cb get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_policer_alloc_cb alloc;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_policer_is_allocated_cb is_allocated;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_policer_free_cb free;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_policer_verify_cb verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
    sw_state_dpp_bcm_policer_policer_group_cbs_t policer_group;
} sw_state_dpp_bcm_policer_cbs_t;

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

int sw_state_dpp_bcm_policer_access_cb_init(int unit);

#endif /* _SHR_SW_STATE_DPP_BCM_POLICER_ACCESS_H_ */
