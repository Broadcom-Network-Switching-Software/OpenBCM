/*! \file bcm56996_a0_fp_egr_qual_enum.c
 *
 * Some qualifiers in EFP are exposed as enumerations to end user.
 * User provides one of the many enumerated values supported for
 * those qualifers and internally the SDK converts that enumerated
 * value  to data and mask to be configured in h/w. This file contains
 * data and mask set/get functions for all such qualifiers for
 * Tomahawk-4GG(56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcm56996_a0_fp_egr_qual_ifp_class_id_type_get(int unit,
                                              uint32_t data,
                                              uint32_t mask,
                                              int *class_id_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(class_id_type, SHR_E_PARAM);

    if ((data == 0) && (mask == 0)) {
        *class_id_type = BCMFP_ANY_CLASS_ID;
    } else  if ((data == 0) && (mask == 0xF)) {
        *class_id_type = BCMFP_INVALID_CLASS_ID;
    } else  if ((data == 0x1) && (mask == 0xF)) {
        *class_id_type = BCMFP_ING_INTF_PORT_CLASS_ID;
    } else  if ((data == 0x2) && (mask == 0xF)) {
        *class_id_type = BCMFP_ING_INTF_VPORT_CLASS_ID;
    } else  if ((data == 0x3) && (mask == 0xF)) {
        *class_id_type = BCMFP_L3_INTF_CLASS;
    } else  if ((data == 0x4) && (mask == 0xF)) {
        *class_id_type = BCMFP_FP_VLAN_CLASS1;
    } else  if ((data == 0x5) && (mask == 0xF)) {
        *class_id_type = BCMFP_FP_VLAN_CLASS0;
    } else  if ((data == 0x6) && (mask == 0xF)) {
        *class_id_type = BCMFP_L2_SRC_CLASS;
    } else  if ((data == 0x7) && (mask == 0xF)) {
        *class_id_type = BCMFP_L2DEST_CLASS;
    } else  if ((data == 0x8) && (mask == 0xF)) {
        *class_id_type = BCMFP_L3_SRC_CLASS;
    } else  if ((data == 0x9) && (mask == 0xF)) {
        *class_id_type = BCMFP_L3DEST_CLASS;
    } else  if ((data == 0xA) && (mask == 0xF)) {
        *class_id_type = BCMFP_VLAN_CLASS;
    } else  if ((data == 0xB) && (mask == 0xF)) {
        *class_id_type = BCMFP_VRF_CLASS;
    } else  if ((data == 0xF) && (mask == 0xF)) {
        *class_id_type = BCMFP_ING_CLASS_ID;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_qual_ifp_class_id_type_set(int unit,
                                              int class_id_type,
                                              uint32_t *data,
                                              uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (class_id_type) {
        case BCMFP_ANY_CLASS_ID:
            *data = 0;
            *mask = 0;
            break;
        case BCMFP_INVALID_CLASS_ID:
            *data = 0;
            *mask = 0xF;
            break;
        case BCMFP_ING_INTF_PORT_CLASS_ID:
            *data = 0x1;
            *mask = 0xF;
            break;
        case BCMFP_ING_INTF_VPORT_CLASS_ID:
            *data = 0x2;
            *mask = 0xF;
            break;
        case BCMFP_L3_INTF_CLASS:
            *data = 0x3;
            *mask = 0xF;
            break;
        case BCMFP_FP_VLAN_CLASS1:
            *data = 0x4;
            *mask = 0xF;
            break;
        case BCMFP_FP_VLAN_CLASS0:
            *data = 0x5;
            *mask = 0xF;
            break;
        case BCMFP_L2_SRC_CLASS:
            *data = 0x6;
            *mask = 0xF;
            break;
        case BCMFP_L2DEST_CLASS:
            *data = 0x7;
            *mask = 0xF;
            break;
        case BCMFP_L3_SRC_CLASS:
            *data = 0x08;
            *mask = 0xF;
            break;
        case BCMFP_L3DEST_CLASS:
            *data = 0x09;
            *mask = 0xF;
            break;
        case BCMFP_VLAN_CLASS:
            *data = 0xA;
            *mask = 0xF;
            break;
        case BCMFP_VRF_CLASS:
            *data = 0xB;
            *mask = 0xF;
            break;
        case BCMFP_ING_CLASS_ID:
            *data = 0xF;
            *mask = 0xF;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


STATIC int
bcm56996_a0_fp_egr_qual_loopback_set(int unit,
                                     int loopback,
                                     uint32_t *data,
                                     uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (loopback) {
        case BCMFP_LB_ANY:
            *data = 0x00;
            *mask = 0x00;
            break;
        case BCMFP_LB_TUNNEL:
            *data = 0x00;
            *mask = 0x1F;
            break;
        case BCMFP_LB_TRILL_NETWORK:
            *data = 0x01;
            *mask = 0x1F;
            break;
        case BCMFP_LB_TRILL_ACCESS:
            *data = 0x02;
            *mask = 0x1F;
            break;
        case BCMFP_LB_TRILL_MASQUERADE:
            *data = 0x03;
            *mask = 0x1F;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_qual_loopback_get(int unit,
                                     uint32_t data,
                                     uint32_t mask,
                                     int *loopback)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(loopback, SHR_E_PARAM);

    if ((data == 0x0) && (mask == 0x0)) {
        *loopback = BCMFP_LB_ANY;
    } else  if ((data == 0x0) && (mask == 0x1F)) {
        *loopback = BCMFP_LB_TUNNEL;
    } else  if ((data == 0x1) && (mask == 0x1F)) {
        *loopback = BCMFP_LB_TRILL_NETWORK;
    } else  if ((data == 0x2) && (mask == 0x1F)) {
        *loopback = BCMFP_LB_TRILL_ACCESS;
    } else  if ((data == 0x3) && (mask == 0x1F)) {
        *loopback = BCMFP_LB_TRILL_MASQUERADE;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_qual_ipfrag_set(int unit,
                                   int frag_info,
                                   uint32_t *data,
                                   uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (frag_info) {
        case BCMFP_IPFRAG_ANY:
            /* Any fragment of fragmented packet */
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_IPFRAG_NON:
            /* Not the first fragment */
            *data = 0x00;
            *mask = 0x03;
            break;
        case BCMFP_IPFRAG_FIRST:
            /* First fragment of fragmented packet*/
            *data = 0x02;
            *mask = 0x03;
            break;
        case BCMFP_IPFRAG_NONORFIRST:
            /* Not the first fragment */
            *data = 0x00;
            *mask = 0x01;
            break;
        case BCMFP_IPFRAG_NOTFIRST:
            /* Not the first fragment */
            *data = 0x03;
            *mask = 0x03;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_qual_ipfrag_get(int unit,
                                   uint32_t data,
                                   uint32_t mask,
                                   int *frag_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(frag_info, SHR_E_PARAM);

    if ((data == 0x2) && (mask == 0x2)) {
        *frag_info = BCMFP_IPFRAG_ANY;
    } else  if ((data == 0x0) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_NON;
    } else  if ((data == 0x2) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_FIRST;
    } else  if ((data == 0x0) && (mask == 0x1)) {
        *frag_info = BCMFP_IPFRAG_NONORFIRST;
    } else  if ((data == 0x3) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_NOTFIRST;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_qual_fwd_type_set(int unit,
                                     int type,
                                     uint32_t *data,
                                     uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    if (type >= BCMFP_FWD_TYPE_COUNT) {
        return (SHR_E_PARAM);
    }

    switch (type) {
        case BCMFP_FWD_TYPE_ANY:
            *data = 0;
            *mask = 0;
            break;
        case BCMFP_FWD_TYPE_INVALID:
            *data = 0;
            *mask = 0x3;
            break;
        case BCMFP_FWD_TYPE_VLAN:
            *data = 1;
            *mask = 0x3;
            break;
        case BCMFP_FWD_TYPE_VRF:
            *data = 3;
            *mask = 0x3;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_qual_fwd_type_get(int unit,
                                     uint32_t data,
                                     uint32_t mask,
                                     int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (mask == 0)) {
        *type = BCMFP_FWD_TYPE_ANY;
    } else if ((0x0 == data) && (mask == 0x3)) {
        *type = BCMFP_FWD_TYPE_INVALID;
    } else if ((0x1 == data) && (mask == 0x3)) {
        *type = BCMFP_FWD_TYPE_VLAN;
    } else if ((0x3 == data) && (mask == 0x3)) {
        *type = BCMFP_FWD_TYPE_VRF;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_main_qual_enum_set(int unit,
                                      bcmfp_qualifier_t qual,
                                      int value,
                                      uint32_t *data,
                                      uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (qual) {
        case FP_EGR_GRP_TEMPLATEt_QUAL_IP_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_common_qual_ip_type_set(unit,
                                                        value,
                                                        data,
                                                        mask));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_COLORf:
        case FP_EGR_GRP_TEMPLATEt_QUAL_COLORf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_common_qual_color_set(unit,
                                                      value,
                                                      data,
                                                      mask));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_L2_FORMATf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_common_qual_l2format_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_FWD_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_fwd_type_set(unit,
                                                      value,
                                                      data,
                                                      mask));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_ING_CLASS_ID_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_ifp_class_id_type_set(unit,
                                                               value,
                                                               data,
                                                               mask));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_loopback_set(unit,
                                                      value,
                                                      data,
                                                      mask));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_IP_FRAGf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_ipfrag_set(unit,
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
bcm56996_a0_fp_egr_qual_enum_set(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 int value,
                                 uint32_t *data,
                                 uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    if (is_presel == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_fp_egr_main_qual_enum_set(unit,
                                                   qual,
                                                   value,
                                                   data,
                                                   mask));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56996_a0_fp_egr_main_qual_enum_get(int unit,
                                      bcmfp_qualifier_t qual,
                                      uint32_t data,
                                      uint32_t mask,
                                      int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    switch (qual) {
        case FP_EGR_GRP_TEMPLATEt_QUAL_IP_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_common_qual_ip_type_get(unit,
                                                        data,
                                                        mask,
                                                        value));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_COLORf:
        case FP_EGR_GRP_TEMPLATEt_QUAL_COLORf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_common_qual_color_get(unit,
                                                      data,
                                                      mask,
                                                      value));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_L2_FORMATf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_common_qual_l2format_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_FWD_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_fwd_type_get(unit,
                                                      data,
                                                      mask,
                                                      value));
            break;
        case FP_EGR_GRP_TEMPLATEt_QUAL_ING_CLASS_ID_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_ifp_class_id_type_get(unit,
                                                               data,
                                                               mask,
                                                               value));
            break;
         case FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_loopback_get(unit,
                                                      data,
                                                      mask,
                                                      value));
            break;
         case FP_EGR_GRP_TEMPLATEt_QUAL_IP_FRAGf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_ipfrag_get(unit,
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
bcm56996_a0_fp_egr_qual_enum_get(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 uint32_t data,
                                 uint32_t mask,
                                 int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    if (is_presel == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_fp_egr_main_qual_enum_get(unit,
                                                   qual,
                                                   data,
                                                   mask,
                                                   value));
    }
exit:
    SHR_FUNC_EXIT();
}
