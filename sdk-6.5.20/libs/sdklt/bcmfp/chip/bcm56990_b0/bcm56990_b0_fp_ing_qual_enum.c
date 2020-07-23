/*! \file bcm56990_b0_fp_ing_qual_enum.c
 *
 * Some qualifiers in IFP are exposed as enumerations to end user.
 * User provides one of the many enumerated values supported for
 * those qualifers and internally the SDK converts that enumerated
 * value  to data and mask to be configured in h/w. This file contains
 * data and mask set/get functions for all such qualifiers for
 * Tomahawk-4 B0(56990_B0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*
 * \brief Get the key and mask values for the given enum values for
 *  STP(spanning tree) state.
 *
 * \param [in] unit      Logical device id.
 * \param [in] stp_state STP state.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.

 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_stpstate_set(int unit,
                                     int stp_state,
                                     uint32_t *data,
                                     uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (stp_state) {
        case BCMFP_STG_STP_ANY:
            *data = 0x00;
            *mask = 0x00;
            break;
        case BCMFP_STG_STP_DISABLE:
            *data = 0x00;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_BLOCK:
            *data = 0x01;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_LEARN:
            *data = 0x02;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_FWD:
            *data = 0x03;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_LEARN_FWD:
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_STG_STP_LEARN_DISABLE:
            *data = 0x00;
            *mask = 0x01;
            break;
        case BCMFP_STG_STP_FWD_BLOCK:
            *data = 0x01;
            *mask = 0x01;
            break;
        case BCMFP_STG_STP_DISABLE_BLOCK:
            *data = 0x00;
            *mask = 0x02;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}



/*
 * \brief Get the enum value for spanning tree state from the
 *  given data and mask.
 *
 * \param [in] unit        Logical device id.
 * \param [in] data        Qualifier KEY value.
 * \param [in] mask        Qualifier MASK value.
 * \param [out] stp_state  Spanning tree state.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_stpstate_get(int unit,
                                     uint32_t data,
                                     uint32_t mask,
                                     int *stp_state)

{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stp_state, SHR_E_PARAM);

    if ((data == 0x0) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_DISABLE;
    } else  if ((data == 0x1) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_BLOCK;
    } else  if ((data == 0x2) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_LEARN;
    } else  if ((data == 0x3) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_FWD;
    } else  if ((data == 0x2) && (mask == 0x2)) {
        *stp_state = BCMFP_STG_STP_LEARN_FWD;
    } else  if ((data == 0x0) && (mask == 0x1)) {
        *stp_state = BCMFP_STG_STP_LEARN_DISABLE;
    } else  if ((data == 0x1) && (mask == 0x1)) {
        *stp_state = BCMFP_STG_STP_FWD_BLOCK;
    } else  if ((data == 0x0) && (mask == 0x2)) {
        *stp_state = BCMFP_STG_STP_DISABLE_BLOCK;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values
 *  for loopback type.
 *
 * \param [in] unit      Logical device id.
 * \param [in] type      Loopback type enum value.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_loopback_type_set(int unit,
                                          int type,
                                          uint32_t *data,
                                          uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (type) {
        case BCMFP_LOOPBACK_TYPE_GENERIC:
            *data = 0x1;
            *mask = 0x1;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for tunnel type from the given
 *  data and mask.
 *
 * \param [in] unit    Logical device id.
 * \param [in] data    Qualifier KEY value.
 * \param [in] mask    Qualifier MASK value.
 * \param [out] type   Tunnel type enum value.
 *
 * \retval SHR_E_NONE  Success
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_loopback_type_get(int unit,
                                          uint32_t data,
                                          uint32_t mask,
                                          int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    switch (data & mask) {
        case 1:
            *type = BCMFP_LOOPBACK_TYPE_GENERIC;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values
 *  for tunnel type.
 *
 * \param [in] unit      Logical device id.
 * \param [in] type      Tunnel type enum value.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_tunnel_type_set(int unit,
                                        int type,
                                        uint32_t *data,
                                        uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (type) {
        case BCMFP_TNL_TYPE_ANY:
            *data = 0x0;
            *mask = 0x0;
            break;
        case BCMFP_TNL_TYPE_NONE:
            *data = 0x0;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_IP:
            *data = 0x1;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_MPLS:
            *data = 0x2;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_MIM:
            *data = 0x3;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_AUTO_IP4_MC:
            *data = 0x6;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_TRILL:
            *data = 0x7;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_L2GRE:
            *data = 0x8;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_VXLAN:
            *data = 0x9;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_SRV6:
            *data = 0x4;
            *mask = 0xf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for tunnel type from the given
 *  data and mask.
 *
 * \param [in] unit    Logical device id.
 * \param [in] data    Qualifier KEY value.
 * \param [in] mask    Qualifier MASK value.
 * \param [out] type   Tunnel type enum value.
 *
 * \retval SHR_E_NONE  Success
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_tunnel_type_get(int unit,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    switch (data & mask) {
        case 0x1:
            *type = BCMFP_TNL_TYPE_IP;
            break;
        case 0x2:
            *type = BCMFP_TNL_TYPE_MPLS;
            break;
        case 0x3:
            *type = BCMFP_TNL_TYPE_MIM;
            break;
        case 0x4:
            *type = BCMFP_TNL_TYPE_SRV6;
            break;
        case 0x6:
            *type = BCMFP_TNL_TYPE_AUTO_IP4_MC;
            break;
        case 0x7:
            *type = BCMFP_TNL_TYPE_TRILL;
            break;
        case 0x8:
            *type = BCMFP_TNL_TYPE_L2GRE;
            break;
        case 0x9:
            *type = BCMFP_TNL_TYPE_VXLAN;
            break;
        case 0x0:
            if (mask) {
                *type = BCMFP_TNL_TYPE_NONE;
                break;
            }
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values for
 *  MPLS forwarding label action.
 *
 * \param [in] unit      Logical device id.
 * \param [in] action    Forwarding Label Action.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.

 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_mpls_fwd_lbl_action_set(int unit,
                                                int action,
                                                uint32_t *data,
                                                uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    *mask = 0x7;
    switch (action)  {
        case BCMFP_MPLS_ACT_PHP:
            *data = 0x2;
            break;
        case BCMFP_MPLS_ACT_SWAP:
            *data = 0x3;
            break;
        case BCMFP_MPLS_ACT_POP:
            *data = 0x4;
            break;
        case BCMFP_MPLS_ACT_POP_USE_L2_VPN:
            *data = 0x5;
            break;
        case BCMFP_MPLS_ACT_POP_USE_L3_VPN:
            *data = 0x6;
            break;
        case BCMFP_MPLS_ACT_POP_USE_ENTROPY:
            *data = 0x7;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for MPLS forwarding label action
 *  from the given data and mask.
 *
 * \param [in] unit      Logical device id.
 * \param [in] data      Qualifier KEY value.
 * \param [in] mask      Qualifier MASK value.
 * \param [out] action    Enumeration value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
STATIC int
bcm56990_b0_fp_ing_qual_mpls_fwd_lbl_action_get(int unit,
                                                uint32_t data,
                                                uint32_t mask,
                                                int *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    switch (data ) {
        case 0x2:
            *action = BCMFP_MPLS_ACT_PHP;
            break;
        case 0x3:
            *action = BCMFP_MPLS_ACT_SWAP;
            break;
        case 0x4:
            *action = BCMFP_MPLS_ACT_POP;
            break;
        case 0x5:
            *action = BCMFP_MPLS_ACT_POP_USE_L2_VPN;
            break;
        case 0x6:
            *action = BCMFP_MPLS_ACT_POP_USE_L3_VPN;
            break;
        case 0x7:
            *action = BCMFP_MPLS_ACT_POP_USE_ENTROPY;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask pair for the given enum value of the
 *  BCMFP qualifier in presel TCAM.
 *
 * \param [in] unit    Logical device id.
 * \param [in] qual    BCMFP qualifier.
 * \param [in] value   Qalifier enum value.
 * \param [out] data   Qualifier KEY value.
 * \param [out] mask   Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum get function is not available.
 */
STATIC int
bcm56990_b0_fp_ing_presel_qual_enum_set(int unit,
                                        bcmfp_qualifier_t qual,
                                        int value,
                                        uint32_t *data,
                                        uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (qual) {
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ip_type_set(unit,
                                                        value,
                                                        data,
                                                        mask));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_fwd_type_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_stpstate_set(unit,
                                                      value,
                                                      data,
                                                      mask));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_LOOPBACK_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_loopback_type_set(unit,
                                                           value,
                                                           data,
                                                           mask));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_tunnel_type_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask pair for the given enum value of the
 *  BCMFP qualifier.
 *
 * \param [in] unit    Logical device id.
 * \param [in] qual    BCMFP qualifier.
 * \param [in] value   Qalifier enum value.
 * \param [out] data   Qualifier KEY value.
 * \param [out] mask   Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum get function is not available.
 */
STATIC int
bcm56990_b0_fp_ing_main_qual_enum_set(int unit,
                                      bcmfp_qualifier_t qual,
                                      int value,
                                      uint32_t *data,
                                      uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (qual) {
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ip_type_set(unit,
                                                        value,
                                                        data,
                                                        mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_COLOR_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_color_set(unit,
                                                      value,
                                                      data,
                                                      mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2_FORMAT_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_l2format_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_OUTER_TPID_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_tpid_set(unit,
                                                     value,
                                                     data,
                                                     mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FWD_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_fwd_type_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FRAG_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ipfrag_set(unit,
                                                       value,
                                                       data,
                                                       mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_ING_STP_STATE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_stpstate_set(unit,
                                                      value,
                                                      data,
                                                      mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_loopback_type_set(unit,
                                                           value,
                                                           data,
                                                           mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TNL_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_tunnel_type_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ACTION_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_mpls_fwd_lbl_action_set(unit,
                                                                 value,
                                                                 data,
                                                                 mask));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask pair for the given enum value of the
 *  BCMFP qualifier in presel or main TCAM.
 *
 * \param [in] unit    Logical device id.
 * \param [in] qual    BCMFP qualifier.
 * \param [in] value   Qalifier enum value.
 * \param [out] data   Qualifier KEY value.
 * \param [out] mask   Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success.
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum get function is not available.
 */
int
bcm56990_b0_fp_ing_qual_enum_set(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 int value,
                                 uint32_t *data,
                                 uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    if (is_presel == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_b0_fp_ing_presel_qual_enum_set(unit,
                                                     qual,
                                                     value,
                                                     data,
                                                     mask));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_b0_fp_ing_main_qual_enum_set(unit,
                                                   qual,
                                                   value,
                                                   data,
                                                   mask));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for the key and mask pair of the given
 *  BCMFP qualifier.
 *
 * \param [in] unit      Logical device id.
 * \param [in] qual      BCMFP qualifier.
 * \param [in] data      Qualifier KEY value.
 * \param [in] mask      Qualifier MASK value.
 * \param [out] value    Qalifier enum value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum get function is not available.
 */
STATIC int
bcm56990_b0_fp_ing_main_qual_enum_get(int unit,
                                      bcmfp_qualifier_t qual,
                                      uint32_t data,
                                      uint32_t mask,
                                      int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    switch (qual) {
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ip_type_get(unit,
                                                        data,
                                                        mask,
                                                        value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_COLOR_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_color_get(unit,
                                                      data,
                                                      mask,
                                                      value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2_FORMAT_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_l2format_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_OUTER_TPID_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_tpid_get(unit,
                                                     data,
                                                     mask,
                                                     value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FWD_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_fwd_type_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FRAG_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ipfrag_get(unit,
                                                       data,
                                                       mask,
                                                       value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_ING_STP_STATE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_stpstate_get(unit,
                                                      data,
                                                      mask,
                                                      value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_loopback_type_get(unit,
                                                           data,
                                                           mask,
                                                           value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TNL_TYPE_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_tunnel_type_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ACTION_BITMAPf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_mpls_fwd_lbl_action_get(unit,
                                                                 data,
                                                                 mask,
                                                                 value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for the key and mask pair of the given
 *  BCMFP qualifier in presel TCAM.
 *
 * \param [in] unit      Logical device id.
 * \param [in] qual      BCMFP qualifier.
 * \param [in] data      Qualifier KEY value.
 * \param [in] mask      Qualifier MASK value.
 * \param [out] value    Qalifier enum value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum get function is not available.
 */
STATIC int
bcm56990_b0_fp_ing_presel_qual_enum_get(int unit,
                                        bcmfp_qualifier_t qual,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    switch (qual) {
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ip_type_get(unit,
                                                        data,
                                                        mask,
                                                        value));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_fwd_type_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_LOOPBACK_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_loopback_type_get(unit,
                                                           data,
                                                           mask,
                                                           value));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_stpstate_get(unit,
                                                      data,
                                                      mask,
                                                      value));
            break;
        case FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_qual_tunnel_type_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for the key and mask pair of the given
 *  BCMFP qualifier in either presel or main TCAM.
 *
 * \param [in] unit      Logical device id.
 * \param [in] qual      BCMFP qualifier.
 * \param [in] data      Qualifier KEY value.
 * \param [in] mask      Qualifier MASK value.
 * \param [out] value    Qalifier enum value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum get function is not available.
 */
int
bcm56990_b0_fp_ing_qual_enum_get(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 uint32_t data,
                                 uint32_t mask,
                                 int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    if (is_presel == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_b0_fp_ing_presel_qual_enum_get(unit,
                                                     qual,
                                                     data,
                                                     mask,
                                                     value));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_b0_fp_ing_main_qual_enum_get(unit,
                                                   qual,
                                                   data,
                                                   mask,
                                                   value));
    }
exit:
    SHR_FUNC_EXIT();
}
