/** \file init_cb_wrappers.h
 * 
 * The DNX init sequence uses CB functions for init and deinit steps. new functions are written according the required 
 * definitions, old ones however are wrapped and placed in this file.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INIT_CB_WRAPPERS_H_INCLUDED
/*
 * { 
 */
#define INIT_CB_WRAPPERS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/init/init.h>

/**
 * \brief - init Done step, used to update relevant modules that 
 *        the init was done ( should be used only for this
 *        purpose ) if this function starts to explode out of
 *        control need to split into actual different steps.
 */
shr_error_e dnx_init_done_init(
    int unit);

/**
 * \brief - currently place holder, probably won't be 
 *        implemented at all.
 */
shr_error_e dnx_init_done_deinit(
    int unit);

/**
 * \brief - wrapper for soc feature init function
 */
shr_error_e dnx_init_feature_init(
    int unit);

/**
 * \brief - wrapper for family set function
 */
shr_error_e dnx_init_family_init(
    int unit);

#ifdef ADAPTER_SERVER_MODE
/**
 * \brief - wrapper for adapter_reg_access_init
 */
shr_error_e dnx_init_adapter_reg_access_init(
    int unit);

/**
 * \brief - wrapper for adapter_reg_access_deinit
 */
shr_error_e dnx_init_adapter_reg_access_deinit(
    int unit);
#endif

/**
 * \brief - wrapper for dnx_init_mpls_module_init
 */
shr_error_e dnx_init_mpls_module_init(
    int unit);

/**
 * \brief - place holder for mpls module deinit func
 * once implemented
 */
shr_error_e dnx_init_mpls_module_deinit(
    int unit);

/**
 * \brief - HW overwrites - Init
 */
shr_error_e dnx_init_hw_overwrite_init(
    int unit);

/**
 * \brief - Hard reset -Init
 */
shr_error_e dnx_init_hard_reset_init(
    int unit);
/**
 * \brief - Core clock verify
 */
shr_error_e dnx_init_core_clock_verify(
    int unit);

/**
 * \brief - wrapper for MDB init
 */
shr_error_e dnx_dbal_mdb_init(
    int unit);

/**
 * \brief - wrapper for MDB init
 */
shr_error_e dnx_dbal_pemla_init(
    int unit);

/**
 * \brief - wrapper for MDB init
 */
shr_error_e dnx_dbal_sw_access_init(
    int unit);

/**
 * \brief - wrapper for DBAL MDB access init
 */
shr_error_e dnx_dbal_mdb_deinit(
    int unit);

/**
 * \brief - wrapper for DBAL pemla access init
 */
shr_error_e dnx_dbal_pemla_deinit(
    int unit);

/**
 * \brief - wrapper for DBAL SW access init
 */
shr_error_e dnx_dbal_sw_access_deinit(
    int unit);

/*
 * } 
 */
#endif /* INIT_CB_WRAPPERS_H_INCLUDED */
