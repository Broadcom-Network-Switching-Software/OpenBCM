/** \file vswitch_vpn.h 
 * General Vswitch utility functionality for DNX. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef VSWITCH_VPN_H_INCLUDED
/** { */
#define VSWITCH_VPN_H_INCLUDED

#define VSI_USAGE_VALUE_SET 1   /* value to set usage */
#define VSI_USAGE_VALUE_CLEAR 0 /* value to clear usage */

typedef enum _dnx_vsi_type_s
{
    _bcmDnxVsiTypeNone,         /* None of VPN */
    _bcmDnxVsiTypeAny,          /* Any VPN */
    _bcmDnxVsiTypeMpls,         /* MPLS VPN */
    _bcmDnxVsiTypeVswitch,      /* VSWITCH VSI */
    _bcmDnxVsiTypeMim,          /* MIM VSI */
    _bcmDnxVsiTypeL2gre,        /* L2GRE VSI */
    _bcmDnxVsiTypeVxlan,        /* VXLAN */
    _bcmDnxVsiTypeAll,          /* All */
    _bcmDnxVsiTypeCount
} dnx_vsi_type_e;

/**
 * \brief
 *      Allocate a vsi for type.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in,out] vsi - vsi for set/clear \n
 *      VSI is not allocated, use this VSI; otherwise allocate a new one \n
 * \param [in] flags - DNX_ALGO_RES_ALLOCATE_xxx \n
 * \param [in] type - see dnx_vsi_type_e \n
 * \param [out] was_replaced - indicate if used by other type. \n
 * \return
 *      * Zero in case of NO ERROR.
 *      * Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Allocate a VSI and set usage type in SW table.
 * \see
 *      * none
 */

shr_error_e dnx_vswitch_vsi_usage_alloc(
    int unit,
    uint32 flags,
    dnx_vsi_type_e type,
    int *vsi,
    uint8 *was_replaced);

/**
 * \brief
 *      De-allocate a vsi for type.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vsi - vsi for set/clear \n
 * \param [in] type - see dnx_vsi_type_e \n
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Free a VSI and clear usage type in SW table.
 * \see
 *      * none
 */
shr_error_e dnx_vswitch_vsi_usage_dealloc(
    int unit,
    dnx_vsi_type_e type,
    int vsi);

/**
 * \brief
 *      Get a VSI usage according to input type.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vsi - vsi for get operation \n
 * \param [in] type - see dnx_vsi_type_e, only support for following type \n
 *          _bcmDnxVsiTypeMpls: used for MPSL VPN \n
 *          _bcmDnxVsiTypeVswitch \n
 *          _bcmDnxVsiTypeL2gre \n
 *          _bcmDnxVsiTypeMim \n
 *          _bcmDnxVsiTypeVxlan \n
 * \param [out] value - usage of this type on the VSI. 
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * This function only support to get/check VSI for single type.
 * \see
 *      * none
 */

shr_error_e dnx_vswitch_vsi_usage_per_type_set(
    int unit,
    int vsi,
    dnx_vsi_type_e type,
    uint8 value);

/**
 * \brief
 *      Get a VSI usage according to input type.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vsi - vsi for get operation \n
 * \param [in] type - see dnx_vsi_type_e, only support for following type \n
 *          _bcmDnxVsiTypeMpls: used for MPSL VPN \n
 *          _bcmDnxVsiTypeVswitch \n
 *          _bcmDnxVsiTypeL2gre \n
 *          _bcmDnxVsiTypeMim \n
 *          _bcmDnxVsiTypeVxlan \n
 * \param [out] is_used - usage of this type on the VSI. 
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * This function only support to get/check VSI for single type.
 * \see
 *      * none
 */
shr_error_e dnx_vswitch_vsi_usage_per_type_get(
    int unit,
    int vsi,
    dnx_vsi_type_e type,
    uint8 *is_used);

/**
 * \brief
 *      check all usages of this VSI except the input "type". 
 *      It's useful to check if VSI is empty or only used for the input "type".
 *      When (type==_bcmDnxVsiTypeNone), this function will check if VSI is empty.
 *      If VSI is used for VxLAN VPN, call type with "_bcmDnxVsiTypeVxlan", 
 *      then function will check if this VSI also used by other type of services(exclude VxLAN VPN).
 *
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vsi - vsi for set/clear \n
 * \param [in] type - see dnx_vsi_type_e, only support for following type \n
 *          _bcmDnxVsiTypeNone  - check all type of usages. \n
 *          _bcmDnxVsiTypeMpls \n
 *          _bcmDnxVsiTypeVswitch \n
 *          _bcmDnxVsiTypeL2gre \n
 *          _bcmDnxVsiTypeMim \n
 *          _bcmDnxVsiTypeVxlan \n
 * \param [out] *used - indicate if used by other type service. 
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Check all types of usage except one/none.
 * \see
 *      * none
 */
shr_error_e dnx_vswitch_vsi_usage_check_except_type(
    int unit,
    int vsi,
    dnx_vsi_type_e type,
    uint8 *used);

/** } */
#endif
