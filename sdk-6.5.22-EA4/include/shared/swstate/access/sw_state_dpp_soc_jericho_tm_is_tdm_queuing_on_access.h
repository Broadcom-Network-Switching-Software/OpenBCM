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

#ifndef _SHR_SW_STATE_DPP_SOC_JERICHO_TM_IS_TDM_QUEUING_ON_ACCESS_H_
#define _SHR_SW_STATE_DPP_SOC_JERICHO_TM_IS_TDM_QUEUING_ON_ACCESS_H_

/********************************* access calbacks definitions *************************************/
/* this set of callbacks, are the callbacks used in the access calbacks struct 'sw_state_cbs_t' to */
/* access the data in 'sw_state_t'.                                                                */
/* the calbacks are inserted into the access struct by 'sw_state_access_cb_init'.                  */
/***************************************************************************************************/

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_set */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_set_cb)(
    int unit, soc_pbmp_t is_tdm_queuing_on);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_get */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_get_cb)(
    int unit, soc_pbmp_t *is_tdm_queuing_on);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_neq */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_neq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_eq */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_eq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_member */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_member_cb)(
    int unit, int _input_port, uint8 *result);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_not_null */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_not_null_cb)(
    int unit, uint8 *result);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_is_null */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_is_null_cb)(
    int unit, uint8 *result);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_count */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_count_cb)(
    int unit, int *result);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_xor */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_xor_cb)(
    int unit, _shr_pbmp_t input_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_remove */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_remove_cb)(
    int unit, _shr_pbmp_t input_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_assign */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_assign_cb)(
    int unit, _shr_pbmp_t input_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_get */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_get_cb)(
    int unit, _shr_pbmp_t *output_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_and */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_and_cb)(
    int unit, _shr_pbmp_t input_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_negate */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_negate_cb)(
    int unit, _shr_pbmp_t input_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_or */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_or_cb)(
    int unit, _shr_pbmp_t input_pbmp);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_clear */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_clear_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_add */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_add_cb)(
    int unit, int _input_port);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_flip */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_flip_cb)(
    int unit, int _input_port);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_remove */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_remove_cb)(
    int unit, int _input_port);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_set */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_port_set_cb)(
    int unit, int _input_port);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_ports_range_add */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_ports_range_add_cb)(
    int unit, int _first_port, int _range);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_fmt */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_pbmp_fmt_cb)(
    int unit, char* _buffer);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* implemented by: sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_verify */
typedef int (*sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_verify_cb)(
    int unit);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

/*********************************** access calbacks struct ****************************************/
/* this set of structs, rooted at 'sw_state_cbs_t' define the access layer for the entire SW state.*/
/* use this tree to alloc/free/set/get fields in the sw state rooted at 'sw_state_t'.              */
/* NOTE: 'sw_state_t' data should not be accessed directly.                                        */
/***************************************************************************************************/


int sw_state_dpp_soc_jericho_tm_is_tdm_queuing_on_access_cb_init(int unit);

#endif /* _SHR_SW_STATE_DPP_SOC_JERICHO_TM_IS_TDM_QUEUING_ON_ACCESS_H_ */
