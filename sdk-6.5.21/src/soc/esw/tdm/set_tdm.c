/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * File:        set_tdm.c
 * Purpose:     TDM algorithm
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
	#include <sdk_drv.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/drv.h>
#endif

size_t stack_size = 0;

#ifndef _TDM_STANDALONE


/* Trident2plus */
tdm_mod_t
* tdm_drv_trident2plus(tdm_mod_t *_tdm)
{
#if ( defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) )
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_td2p_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_td2p_check_ethernet,
        &tdm_pick_vec,
        &tdm_td2p_which_tsc
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_td2p_init,
        &tdm_td2p_pmap_transcription,
        &tdm_td2p_lls_wrapper,
        &tdm_td2p_vbs_wrapper,
        &tdm_td2p_filter,
        &tdm_td2p_parse_mmu_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_td2p_chk,
        &tdm_td2p_free,
        &tdm_td2p_corereq,
        &tdm_td2p_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Trident3 */
tdm_mod_t
* tdm_drv_trident3(tdm_mod_t *_tdm)
{
#ifdef BCM_TRIDENT3_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_td3_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_td3_shim_get_pipe_ethernet,
        &tdm_core_null,
        &tdm_td3_shim_get_port_pm,
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_td3_main_init,
        &tdm_td3_main_transcription,
        &tdm_td3_main_ingress,
        &tdm_core_null,
        &tdm_td3_filter,
        &tdm_td3_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_td3_chk,
        &tdm_td3_main_free,
        &tdm_td3_main_corereq,
        &tdm_td3_main_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Tomahawk */
tdm_mod_t
* tdm_drv_tomahawk(tdm_mod_t *_tdm)
{
#ifdef BCM_TOMAHAWK_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_th_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_th_check_ethernet,
        &tdm_pick_vec,
        &tdm_th_which_tsc
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_th_init,
        &tdm_th_pmap_transcription,
        &tdm_th_scheduler_wrap,
        &tdm_core_null,
        &tdm_th_filter,
        &tdm_th_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_th_chk,
        &tdm_th_free,
        &tdm_th_corereq,
        &tdm_th_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Tomahawk2 */
tdm_mod_t
* tdm_drv_tomahawk2(tdm_mod_t *_tdm)
{
#ifdef BCM_TOMAHAWK2_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_th2_vmap_alloc,
        &tdm_th2_vbs_scheduler,
        &tdm_chip_th2_shim__core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_th2_check_ethernet,
        &tdm_pick_vec,
        &tdm_th2_which_tsc
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_th2_init,
        &tdm_th2_pmap_transcription,
        &tdm_th2_scheduler_wrap,
        &tdm_core_null,
        &tdm_th2_filter_chain,
        &tdm_th2_parse_tdm_tbl_new,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_th2_acc_alloc,
        &tdm_th2_corereq,
        &tdm_th2_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Apache */
tdm_mod_t
* tdm_drv_apache(tdm_mod_t *_tdm)
{
#ifdef BCM_APACHE_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_ap_vmap_alloc,
        &tdm_core_vbs_scheduler,
        &tdm_chip_ap_shim__core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_ap_check_ethernet,
        &tdm_pick_vec,
        &tdm_ap_which_tsc
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_ap_init,
        &tdm_ap_pmap_transcription,
        &tdm_ap_lls_wrapper,
        &tdm_ap_vbs_wrapper,
        &tdm_ap_filter_chain,
        &tdm_ap_parse_mmu_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_ap_chk,
        &tdm_ap_free,
        &tdm_ap_corereq,
        &tdm_ap_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Greyhound2 */
tdm_mod_t *
tdm_drv_greyhound2(tdm_mod_t *_tdm)
{
#ifdef BCM_GREYHOUND2_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_gh2_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_core_ovsb_alloc,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_gh2_check_ethernet,
        &tdm_pick_vec,
        &tdm_gh2_which_tsc
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_gh2_init,
        &tdm_gh2_pmap_transcription,
        &tdm_gh2_scheduler_wrap,
        &tdm_core_null,
        &tdm_gh2_filter_mix,
        &tdm_gh2_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_gh2_chk,
        &tdm_gh2_free,
        &tdm_gh2_corereq,
        &tdm_gh2_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Firelight */
tdm_mod_t *
tdm_drv_firelight(tdm_mod_t *_tdm)
{
#ifdef BCM_FIRELIGHT_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_fl_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_fl_shim_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc_mix,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_fl_shim_get_pipe_ethernet,
        &tdm_pick_vec,
        &tdm_fl_shim_get_port_pm,
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_fl_main_init,
        &tdm_fl_main_transcription,
        &tdm_fl_main_ingress,
        &tdm_core_null,
        &tdm_fl_filter,
        &tdm_fl_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_fl_chk,
        &tdm_fl_main_free,
        &tdm_fl_main_corereq,
        &tdm_fl_main_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Maverick2 */
tdm_mod_t
* tdm_drv_maverick2(tdm_mod_t *_tdm)
{
#ifdef BCM_MAVERICK2_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_mv2_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_mv2_shim_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc_mix,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_mv2_shim_get_pipe_ethernet,
        &tdm_pick_vec,
        &tdm_mv2_shim_get_port_pm,
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_mv2_main_init,
        &tdm_mv2_main_transcription,
        &tdm_mv2_main_ingress,
        &tdm_core_null,
        &tdm_mv2_filter,
        &tdm_mv2_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_mv2_chk,
        &tdm_mv2_main_free,
        &tdm_mv2_main_corereq,
        &tdm_mv2_main_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Helix5 */
tdm_mod_t
* tdm_drv_helix5(tdm_mod_t *_tdm)
{
#ifdef BCM_HELIX5_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_hx5_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_hx5_shim_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_hx5_core_vmap_prealloc,
        &tdm_hx5_core_vmap_alloc_mix,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_hx5_shim_get_pipe_ethernet,
        &tdm_pick_vec,
        &tdm_hx5_shim_get_port_pm,
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_hx5_main_init,
        &tdm_hx5_main_transcription,
        &tdm_hx5_main_ingress,
        &tdm_core_null,
        &tdm_hx5_filter,
        &tdm_hx5_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_hx5_chk,
        &tdm_hx5_main_free,
        &tdm_hx5_main_corereq,
        &tdm_hx5_main_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Firebolt6 */
tdm_mod_t
* tdm_drv_firebolt6(tdm_mod_t *_tdm)
{
#ifdef BCM_FIREBOLT6_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_fb6_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_fb6_shim_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_fb6_shim_core_vmap_prealloc,
        &tdm_core_vmap_alloc_mix,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_fb6_shim_get_pipe_ethernet,
        &tdm_pick_vec,
        &tdm_fb6_shim_get_port_pm,
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_fb6_main_init,
        &tdm_fb6_main_transcription,
        &tdm_fb6_main_ingress,
        &tdm_core_null,
        &tdm_fb6_filter,
        &tdm_fb6_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_fb6_chk,
        &tdm_fb6_main_free,
        &tdm_fb6_main_corereq,
        &tdm_fb6_main_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Hurricane4 */
tdm_mod_t
* tdm_drv_hurricane4(tdm_mod_t *_tdm)
{
#ifdef BCM_HURRICANE4_SUPPORT
    int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
        &tdm_core_init,
        &tdm_core_post,
        &tdm_hr4_proc_cal,
        &tdm_core_vbs_scheduler_wrapper,
        &tdm_hr4_shim_core_vbs_scheduler_ovs,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_acc_alloc,
        &tdm_core_vmap_prealloc,
        &tdm_core_vmap_alloc_mix,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_hr4_shim_get_pipe_ethernet,
        &tdm_pick_vec,
        &tdm_hr4_shim_get_port_pm,
    };
    int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
        &tdm_hr4_main_init,
        &tdm_hr4_main_transcription,
        &tdm_hr4_main_ingress,
        &tdm_core_null,
        &tdm_hr4_filter,
        &tdm_hr4_parse_tdm_tbl,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_core_null,
        &tdm_hr4_chk,
        &tdm_hr4_main_free,
        &tdm_hr4_main_corereq,
        &tdm_hr4_main_post
    };
    TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
    TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
    return _tdm;
}

/* Initialize TDM internal driving functions */
tdm_mod_t
*SOC_SEL_TDM(tdm_soc_t *chip)
{
	tdm_mod_t *_tdm;
	uint16 dev_id = 0;
	uint16 drv_dev_id;
	uint8 rev_id;
	uint16 drv_rev_id;

	_tdm = TDM_ALLOC(sizeof(tdm_mod_t),"TDM constructor allocation");
	if (!_tdm) {
		return NULL;
	}
    TDM_MEMSET(_tdm, 0, sizeof(tdm_mod_t));
	_tdm->_chip_data.soc_pkg = (*chip);

	soc_cm_get_id( _tdm->_chip_data.soc_pkg.unit, &dev_id, &rev_id );
	soc_cm_get_id_driver( dev_id, rev_id, &drv_dev_id, &drv_rev_id );

    /* Handling for HX5 & HR4 SKUs which also have have
       different Ids from the base variant */
    switch(drv_dev_id) {
        case BCM56370_DEVICE_ID:
            _tdm = tdm_drv_helix5(_tdm);
            return _tdm;
        case BCM56275_DEVICE_ID:
            _tdm = tdm_drv_hurricane4(_tdm);
            return _tdm;
    }

	switch (dev_id&0xfff0) {
        /* Trident2plus */
		case (BCM56850_DEVICE_ID&0xfff0):
		case (BCM56860_DEVICE_ID&0xfff0):
		case (BCM56830_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_trident2plus(_tdm);
			break;
        /* Trident3/Maverick2 */
        case (BCM56870_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_trident3(_tdm);
            break;
        /* Maverick2 */
        case (BCM56770_DEVICE_ID & 0xfff0):
            _tdm = tdm_drv_maverick2(_tdm);
            break;
		/* Tomahawk */
        case (BCM56960_DEVICE_ID&0xfff0):
        case BCM56930_DEVICE_ID:
        case (BCM56168_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_tomahawk(_tdm);
			break;
		/* Tomahawk2 */
        case (BCM56970_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_tomahawk2(_tdm);
			break;
		/* Apache */
        case (BCM56560_DEVICE_ID&0xfff0):
		case (BCM56760_DEVICE_ID&0xfff0):
        case (BCM56069_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_apache(_tdm);
			break;
        /* Greyhound2 */
        case (BCM56170_DEVICE_ID&0xfff0):
        case (BCM53570_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_greyhound2(_tdm);
            break;

        case (BCM56070_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_firelight(_tdm);
            break;

	case (BCM56670_DEVICE_ID&0xfff0):
                     {
#ifdef BCM_MONTEREY_SUPPORT
			     int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
				     &tdm_core_init,
				     &tdm_core_post,
				     &tdm_mn_proc_cal,
				     &tdm_core_vbs_scheduler_wrapper,
				     &tdm_core_vbs_scheduler_ovs,
				     &tdm_core_null, /* TDM_CORE_EXEC__EXTRACT */
				     &tdm_core_null, /* TDM_CORE_EXEC__FILTER */
				     &tdm_core_acc_alloc,
				     &tdm_core_vmap_prealloc,
				     &tdm_core_vmap_alloc,
				     &tdm_core_null,
				     &tdm_core_null,
				     &tdm_core_null,
				     &tdm_mn_check_ethernet,
				     &tdm_pick_vec,
				     &tdm_mn_which_tsc
			     };
			     int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
				     &tdm_mn_init,
				     &tdm_mn_pmap_transcription,
				     &tdm_mn_lls_wrapper,
				     &tdm_mn_vbs_wrapper,
				     &tdm_mn_filter_chain,
				     &tdm_mn_parse_mmu_tdm_tbl,
				     &tdm_mn_proc_cal_ancl,
				     &tdm_core_null,
				     &tdm_core_null,
				     &tdm_core_null,
				     &tdm_core_null,
				     &tdm_core_null,
				     &chk_tdm_mn_tbl,
				     &tdm_mn_free,
				     &tdm_mn_corereq,
				     &tdm_mn_post
			     };
			     TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
			     TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif

			}
            break;
        /* Hurricane4 */
        case (BCM56275_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_hurricane4(_tdm);
			break;
        /* Helix5 */
        case (BCM56370_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_helix5(_tdm);
            break;
        /* Firebolt6*/
        case (BCM56470_DEVICE_ID&0xfff0):
            _tdm = tdm_drv_firebolt6(_tdm);
            break;
		default:
			TDM_FREE(_tdm);
			TDM_ERROR1("Unrecognized device ID %0x for TDM scheduling algorithm.\n",dev_id);
			return NULL;
	}

	return _tdm;
}
#endif


tdm_mod_t
*_soc_set_tdm_tbl( tdm_mod_t *_tdm )
{
	int index, tdm_ver_chk[8];

    /* stack_size = (size_t)&index; */

	if (!_tdm) {
		return NULL;
	}
	TDM_BIG_BAR
	TDM_PRINT0("TDM: Release version: ");
	_soc_tdm_ver(tdm_ver_chk);
	TDM_PRINT2("%d%d",tdm_ver_chk[0],tdm_ver_chk[1]);
	for (index=2; index<8; index+=2) {
		TDM_PRINT2(".%d%d",tdm_ver_chk[index],tdm_ver_chk[index+1]);
	}
	TDM_PRINT0("\n"); TDM_SML_BAR

	/* Path virtualized API starting in chip executive */
	return ((_tdm->_chip_exec[TDM_CHIP_EXEC__INIT](_tdm))==PASS)?(_tdm):(NULL);
}
