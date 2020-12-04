/*! \file bcm56990_b0_fp_vlan_qual_enum.c
 *
 * Some qualifiers in VFP are exposed as enumerations to end user.
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

STATIC int
bcm56990_b0_fp_vlan_qual_ipprotocolcommon_set(int unit,
                                              int ipprotocol,
                                              uint32_t *data,
                                              uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (ipprotocol) {
      case BCMFP_IP_PROTO_ANY:
          *data = 0x00;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_TCP:
          *data = 0x00;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_UDP:
          *data = 0x01;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_IGMP:
          *data = 0x02;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_ICMP:
          *data = 0x03;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_IP6ICMP:
          *data = 0x04;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_IP6HOPBYHOP:
          *data = 0x05;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_IPINIP:
          *data = 0x06;
          *mask = 0x07;
          break;
      case BCMFP_IP_PROTO_COMMON_TCPUDP:
          *data = 0x00;
          *mask = 0x06;
          break;
      case BCMFP_IP_PROTO_UNKNOWN:
          *data = 0x7;
          *mask = 0x7;
          break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_b0_fp_vlan_qual_ipprotocolcommon_get(int unit,
                                              uint32_t data,
                                              uint32_t mask,
                                              int *ipprotocol)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ipprotocol, SHR_E_PARAM);

    if ((0x0 == data)  && (0x0 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_ANY;
    } else if ((0x0 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_TCP;
    } else  if ((0x1 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_UDP;
    } else  if ((0x2 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_IGMP;
    } else  if ((0x3 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_ICMP;
    } else  if ((0x4 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_IP6ICMP;
    } else  if ((0x5 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_IP6HOPBYHOP;
    } else  if ((0x6 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_IPINIP;
    } else  if ((0x0 == data)  && (0x6 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_COMMON_TCPUDP;
    } else  if ((0x7 == data)  && (0x7 == mask)) {
       *ipprotocol = BCMFP_IP_PROTO_UNKNOWN;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_b0_fp_vlan_qual_ipfrag_set(int unit,
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
            *data = 0x00;
            *mask = 0x02;
            break;
        case BCMFP_IPFRAG_NON:
            /* Non-fragmented */
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_IPFRAG_FIRST:
            /* First fragment of fragmented packet*/
            *data = 0x01;
            *mask = 0x03;
            break;
        case BCMFP_IPFRAG_NOTFIRST:
            /* Not the first fragment */
            *data = 0x00;
            *mask = 0x03;
            break;
        case BCMFP_IPFRAG_NONORFIRST:
            /* Non-fragmented or first fragment. */
            *data = 0x01;
            *mask = 0x01;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_b0_fp_vlan_qual_ipfrag_get(int unit,
                                    uint32_t data,
                                    uint32_t mask,
                                    int *frag_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(frag_info, SHR_E_PARAM);

    if ((data == 0x0) && (mask == 0x2)) {
        *frag_info = BCMFP_IPFRAG_ANY;
    } else  if ((data == 0x2) && (mask == 0x2)) {
        *frag_info = BCMFP_IPFRAG_NON;
    } else  if ((data == 0x1) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_FIRST;
    } else  if ((data == 0x0) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_NOTFIRST;
    } else  if ((data == 0x1) && (mask == 0x1)) {
        *frag_info = BCMFP_IPFRAG_NONORFIRST;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_b0_fp_vlan_main_qual_enum_set(int unit,
                                       bcmfp_qualifier_t qual,
                                       int value,
                                       uint32_t *data,
                                       uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (qual) {
        case FP_VLAN_GRP_TEMPLATEt_QUAL_IP_TYPEf:
        case FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ip_type_set(unit,
                                                        value,
                                                        data,
                                                        mask));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_L2_FORMATf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_l2format_set(unit,
                                                         value,
                                                         data,
                                                         mask));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_OUTER_TPIDf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_tpid_set(unit,
                                                     value,
                                                     data,
                                                     mask));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FRAGf:
        case FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_FRAGf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_vlan_qual_ipfrag_set(unit,
                                                     value,
                                                     data,
                                                     mask));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTO_COMMONf:
        case FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTO_COMMONf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_vlan_qual_ipprotocolcommon_set(unit,
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
bcm56990_b0_fp_vlan_qual_enum_set(int unit,
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
            (bcm56990_b0_fp_vlan_main_qual_enum_set(unit,
                                                    qual,
                                                    value,
                                                    data,
                                                    mask));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_b0_fp_vlan_main_qual_enum_get(int unit,
                                       bcmfp_qualifier_t qual,
                                       uint32_t data,
                                       uint32_t mask,
                                       int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    switch (qual) {
        case FP_VLAN_GRP_TEMPLATEt_QUAL_IP_TYPEf:
        case FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_ip_type_get(unit,
                                                        data,
                                                        mask,
                                                        value));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_L2_FORMATf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_l2format_get(unit,
                                                         data,
                                                         mask,
                                                         value));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_OUTER_TPIDf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_common_qual_tpid_get(unit,
                                                     data,
                                                     mask,
                                                     value));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FRAGf:
        case FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_FRAGf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_vlan_qual_ipfrag_get(unit,
                                                     data,
                                                     mask,
                                                     value));
            break;
        case FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTO_COMMONf:
        case FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTO_COMMONf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_vlan_qual_ipprotocolcommon_get(unit,
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
bcm56990_b0_fp_vlan_qual_enum_get(int unit,
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
            (bcm56990_b0_fp_vlan_main_qual_enum_get(unit,
                                                    qual,
                                                    data,
                                                    mask,
                                                    value));
    }
exit:
    SHR_FUNC_EXIT();
}
