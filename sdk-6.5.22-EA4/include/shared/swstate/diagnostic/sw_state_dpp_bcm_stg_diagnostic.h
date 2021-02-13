/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 * search for 'sw_state_diagnostic_cbs_t' for the root of the struct
 */

#ifndef _SHR_SW_STATE_DPP_BCM_STG_DIAGNOSTIC_H_
#define _SHR_SW_STATE_DPP_BCM_STG_DIAGNOSTIC_H_

#ifdef BCM_WARM_BOOT_API_TEST
/********************************* diagnostic calbacks definitions *************************************/
/* this set of callbacks, are the callbacks used in the diagnostic calbacks struct 'sw_state_cbs_t' to */
/* access the data in 'sw_state_t'.                                                                */
/* the calbacks are inserted into the diagnostic struct by 'sw_state_diagnostic_cb_init'.                  */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_dump */
typedef int (*sw_state_dpp_bcm_stg_dump_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_init_dump */
typedef int (*sw_state_dpp_bcm_stg_init_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_min_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_min_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_max_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_max_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_defl_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_defl_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_bitmap_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_bitmap_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_enable_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_enable_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_state_h_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_state_h_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_state_l_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_state_l_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_stg_count_dump */
typedef int (*sw_state_dpp_bcm_stg_stg_count_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_vlan_first_dump */
typedef int (*sw_state_dpp_bcm_stg_vlan_first_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_bcm_stg_vlan_next_dump */
typedef int (*sw_state_dpp_bcm_stg_vlan_next_dump_cb)(
    int unit, int stg_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

/*********************************** diagnostic calbacks struct ****************************************/
/* this set of structs, rooted at 'sw_state_cbs_t' define the diagnostic layer for the entire SW state.*/
/* use this tree to dump fields in the sw state rooted at 'sw_state_t'.              */
/* NOTE: 'sw_state_t' data should not be accessed directly.                                        */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_init_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_init_dump_cb dump;
} sw_state_dpp_bcm_stg_init_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_min_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_min_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_min_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_max_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_max_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_max_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_defl_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_defl_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_defl_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_bitmap_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_bitmap_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_bitmap_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_enable_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_enable_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_enable_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_state_h_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_state_h_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_state_h_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_state_l_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_state_l_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_state_l_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_stg_count_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_stg_count_dump_cb dump;
} sw_state_dpp_bcm_stg_stg_count_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_vlan_first_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_vlan_first_dump_cb dump;
} sw_state_dpp_bcm_stg_vlan_first_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_vlan_next_diagnostic_cbs_s {
    sw_state_dpp_bcm_stg_vlan_next_dump_cb dump;
} sw_state_dpp_bcm_stg_vlan_next_diagnostic_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_bcm_stg_diagnostic_cbs_s {
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_bcm_stg_dump_cb dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
    sw_state_dpp_bcm_stg_init_diagnostic_cbs_t init;
    sw_state_dpp_bcm_stg_stg_min_diagnostic_cbs_t stg_min;
    sw_state_dpp_bcm_stg_stg_max_diagnostic_cbs_t stg_max;
    sw_state_dpp_bcm_stg_stg_defl_diagnostic_cbs_t stg_defl;
    sw_state_dpp_bcm_stg_stg_bitmap_diagnostic_cbs_t stg_bitmap;
    sw_state_dpp_bcm_stg_stg_enable_diagnostic_cbs_t stg_enable;
    sw_state_dpp_bcm_stg_stg_state_h_diagnostic_cbs_t stg_state_h;
    sw_state_dpp_bcm_stg_stg_state_l_diagnostic_cbs_t stg_state_l;
    sw_state_dpp_bcm_stg_stg_count_diagnostic_cbs_t stg_count;
    sw_state_dpp_bcm_stg_vlan_first_diagnostic_cbs_t vlan_first;
    sw_state_dpp_bcm_stg_vlan_next_diagnostic_cbs_t vlan_next;
} sw_state_dpp_bcm_stg_diagnostic_cbs_t;

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

int sw_state_dpp_bcm_stg_diagnostic_cb_init(int unit);

#endif /* BCM_WARM_BOOT_API_TEST */

#endif /* _SHR_SW_STATE_DPP_BCM_STG_DIAGNOSTIC_H_ */
