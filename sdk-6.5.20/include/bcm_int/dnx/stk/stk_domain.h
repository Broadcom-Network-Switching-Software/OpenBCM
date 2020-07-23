/** \file stk/stk_domain.h
 *
 * Stacking domain procedures.
 *
 * modue id, domain id ....
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DNX_STK_DOMAIN_H_INCLUDED
/*
 * {
 */
#define DNX_STK_DOMAIN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/stack.h>

#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief 
 * Map and unmap module id to domain id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - module id 
 * \param [in] tm_domain - domain id
 * \param [in] is_exist - 1 for add, 0 for delete
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_domain_modid_to_domain_set(
    int unit,
    uint32 modid,
    uint32 tm_domain,
    uint32 is_exist);

/**
 * \brief 
 * Check the mapping exist or not by given a domain id and a module id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - module id 
 * \param [in] tm_domain - domain id
 * \param [out] is_exist - flag for mapping exist
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_domain_modid_to_domain_get(
    int unit,
    uint32 modid,
    uint32 tm_domain,
    uint32 *is_exist);

/**
 * \brief 
 * Map and unmap domain id to stack trunk id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - domain id 
 * \param [in] stk_tid - stack trunk id
 * \param [in] is_exist - 1 for add, 0 for delete
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_domain_stk_domain_to_trunk_set(
    int unit,
    uint32 tm_domain,
    uint32 stk_tid,
    uint32 is_exist);

/**
 * \brief 
 * Check the mapping exist or not by given a domain id and a stack trunk id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - domain id 
 * \param [in] stk_tid - stack trunk id
 * \param [out] is_exist - 1 for add, 0 for delete
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_domain_stk_domain_to_trunk_get(
    int unit,
    uint32 tm_domain,
    uint32 stk_tid,
    uint32 *is_exist);

/**
 * \brief 
 * Check the same domain or not by given a local module id and a sytem port module id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] local_modid - local module id 
 * \param [in] sysport_modid - sytem port module id
 * \param [out] is_same_domain - 1 for the same domian; 0 for different domain
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_domain_stk_modid_to_domain_find(
    int unit,
    uint32 local_modid,
    uint32 sysport_modid,
    uint32 *is_same_domain);

/**
 * \brief 
 * Get the tm domain id by given a module id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - module id 
 * \param [out] tm_domain - domain id
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_domain_stk_domain_modid_get(
    int unit,
    uint32 modid,
    int *tm_domain);

/**
 * \brief - Verify tm domain id.
 */
int dnx_stk_domain_tm_domain_verify(
    int unit,
    int tm_domain);

/**
 * \brief - Verify module id.
 */
int dnx_stk_domain_modid_verify(
    int unit,
    int modid);

#endif /* DNX_STK_DOMAIN_H_INCLUDED */
