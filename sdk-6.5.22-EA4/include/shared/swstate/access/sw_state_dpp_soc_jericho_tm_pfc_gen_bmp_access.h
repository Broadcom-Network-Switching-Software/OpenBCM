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

#ifndef _SHR_SW_STATE_DPP_SOC_JERICHO_TM_PFC_GEN_BMP_ACCESS_H_
#define _SHR_SW_STATE_DPP_SOC_JERICHO_TM_PFC_GEN_BMP_ACCESS_H_

/********************************* access calbacks definitions *************************************/
/* this set of callbacks, are the callbacks used in the access calbacks struct 'sw_state_cbs_t' to */
/* access the data in 'sw_state_t'.                                                                */
/* the calbacks are inserted into the access struct by 'sw_state_access_cb_init'.                  */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_set */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_set_cb)(
    int unit, CONST SOC_TMC_FC_PFC_GEN_BMP_INFO *pfc_gen_bmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_get */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_get_cb)(
    int unit, SOC_TMC_FC_PFC_GEN_BMP_INFO *pfc_gen_bmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_alloc */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_alloc_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_is_allocated */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_is_allocated_cb)(
    int unit, uint8 *is_allocated);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_free */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_free_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_verify */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_verify_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_set */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_set_cb)(
    int unit, int nif_pfc_gen_bmp_used_idx_0, uint32 nif_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_get */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_get_cb)(
    int unit, int nif_pfc_gen_bmp_used_idx_0, uint32 *nif_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_verify */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_verify_cb)(
    int unit, int nif_pfc_gen_bmp_used_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_set */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_set_cb)(
    int unit, int cal_pfc_gen_bmp_used_idx_0, uint32 cal_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_get */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_get_cb)(
    int unit, int cal_pfc_gen_bmp_used_idx_0, uint32 *cal_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_verify */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_verify_cb)(
    int unit, int cal_pfc_gen_bmp_used_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_set */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_set_cb)(
    int unit, int ilkn_inb_pfc_gen_bmp_used_idx_0, uint32 ilkn_inb_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_get */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_get_cb)(
    int unit, int ilkn_inb_pfc_gen_bmp_used_idx_0, uint32 *ilkn_inb_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_verify */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_verify_cb)(
    int unit, int ilkn_inb_pfc_gen_bmp_used_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_set */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_set_cb)(
    int unit, int mub_pfc_gen_bmp_used_idx_0, uint32 mub_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_get */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_get_cb)(
    int unit, int mub_pfc_gen_bmp_used_idx_0, uint32 *mub_pfc_gen_bmp_used);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_verify */
typedef int (*sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_verify_cb)(
    int unit, int mub_pfc_gen_bmp_used_idx_0);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

/*********************************** access calbacks struct ****************************************/
/* this set of structs, rooted at 'sw_state_cbs_t' define the access layer for the entire SW state.*/
/* use this tree to alloc/free/set/get fields in the sw state rooted at 'sw_state_t'.              */
/* NOTE: 'sw_state_t' data should not be accessed directly.                                        */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_cbs_s {
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_set_cb set;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_get_cb get;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_verify_cb verify;
} sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_cbs_s {
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_set_cb set;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_get_cb get;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_verify_cb verify;
} sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_cbs_s {
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_set_cb set;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_get_cb get;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_verify_cb verify;
} sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_cbs_s {
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_set_cb set;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_get_cb get;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_verify_cb verify;
} sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_cbs_t;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
typedef struct sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cbs_s {
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_set_cb set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_get_cb get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_alloc_cb alloc;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_is_allocated_cb is_allocated;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_free_cb free;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#if defined(BCM_PETRA_SUPPORT)
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_verify_cb verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_nif_pfc_gen_bmp_used_cbs_t nif_pfc_gen_bmp_used;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cal_pfc_gen_bmp_used_cbs_t cal_pfc_gen_bmp_used;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_ilkn_inb_pfc_gen_bmp_used_cbs_t ilkn_inb_pfc_gen_bmp_used;
    sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_mub_pfc_gen_bmp_used_cbs_t mub_pfc_gen_bmp_used;
} sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_cbs_t;

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

int sw_state_dpp_soc_jericho_tm_pfc_gen_bmp_access_cb_init(int unit);

#endif /* _SHR_SW_STATE_DPP_SOC_JERICHO_TM_PFC_GEN_BMP_ACCESS_H_ */
