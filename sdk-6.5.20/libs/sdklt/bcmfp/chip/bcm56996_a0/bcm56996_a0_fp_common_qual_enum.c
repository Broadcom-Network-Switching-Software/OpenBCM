/*! \file bcm56996_a0_fp_common_qual_enum.c
 *
 * Some qualifiers in FP are exposed as enumerations to end user.
 * User provides one of the many enumerated values supported for
 * those qualifers and internally the SDK converts that enumerated
 * value  to data and mask to be configured in h/w. This file contains
 * data and mask set/get functions for all such qualifiers for common to
 * all the three stages in Tomahawk-4G(56996_A0).
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

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*
 * \brief Get the key and mask values for the given enum values for
 *  IP type.
 *
 * \param [in] unit      Logical device id.
 * \param [in] type      IP type.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
int
bcm56996_a0_fp_common_qual_ip_type_set(int unit,
                                       int type,
                                       uint32_t *data,
                                       uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (type) {
        case BCMFP_IPTYPE_ANY:
            *data = 0x0;
            *mask = 0x0;
            break;
        case BCMFP_IPTYPE_NONIP:
            *data = 0xf;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_IP:
            *data = 0x0;
            *mask = 0x8;
            break;
        case BCMFP_IPTYPE_IPV4NOOPTS:
            *data = 0x0;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_IPV4WITHOPTS:
            *data = 0x1;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_IPV4ANY:
            *data = 0x0;
            *mask = 0xe;
            break;
        case BCMFP_IPTYPE_IPV6NOEXTHDR:
            *data = 0x4;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_IPV6ONEEXTHDR:
            *data = 0x5;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_IPV6TWOEXTHDR:
            *data = 0x6;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_IPV6_ANY:
            *data = 0x4;
            *mask = 0xc;
            break;
        case BCMFP_IPTYPE_ARP:
            *data = 0x8;
            *mask = 0xe;
            break;
        case BCMFP_IPTYPE_ARPREQUEST:
            *data = 0x8;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_ARPREPLY:
            *data = 0x9;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_MPLSUNICAST:
            *data = 0xc;
            *mask = 0xf;
            break;
        case BCMFP_IPTYPE_MPLSMULTICAST:
            *data = 0xd;
            *mask = 0xf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for IP type from the given
 *  data and mask.
 *
 * \param [in] unit    Logical device id.
 * \param [in] data    Qualifier KEY value.
 * \param [in] mask    Qualifier MASK value.
 * \param [out] type   IP type.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
int
bcm56996_a0_fp_common_qual_ip_type_get(int unit,
                                       uint32_t data,
                                       uint32_t mask,
                                       int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (0 == mask)) {
        *type = BCMFP_IPTYPE_ANY;
    } else if ((0xf == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_NONIP;
    } else if ((0x0 == data) && (0x8 == mask)) {
        *type = BCMFP_IPTYPE_IP;
    } else if ((0x0 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_IPV4NOOPTS;
    } else if ((0x1 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_IPV4WITHOPTS;
    } else if ((0x0 == data) && (0xe == mask)) {
        *type = BCMFP_IPTYPE_IPV4ANY;
    } else if ((0x4 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_IPV6NOEXTHDR;
    } else if ((0x5 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_IPV6ONEEXTHDR;
    } else if ((0x6 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_IPV6TWOEXTHDR;
    } else if ((0x4 == data) && (0xc == mask)) {
        *type = BCMFP_IPTYPE_IPV6_ANY;
    } else if ((0x8 == data) && (0xe == mask)) {
        *type = BCMFP_IPTYPE_ARP;
    } else if ((0x8 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_ARPREQUEST;
    } else if ((0x9 == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_ARPREPLY;
    } else if ((0xc == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_MPLSUNICAST;
    } else if ((0xd == data) && (0xf == mask)) {
        *type = BCMFP_IPTYPE_MPLSMULTICAST;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values for
 *  L2 format.
 *
 * \param [in] unit      Logical device id.
 * \param [in] type      L2 format.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
int
bcm56996_a0_fp_common_qual_l2format_set(int unit,
                                        int type,
                                        uint32_t *data,
                                        uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    if (type >= BCMFP_L2_FORMAT_COUNT) {
        return (SHR_E_PARAM);
    }

    switch (type) {
        case BCMFP_L2_FORMAT_ANY:
            *data = 0x0;
            *mask = 0x0;
            break;
        case BCMFP_L2_FORMAT_ETHII:
            *data = 0x0;
            *mask = 0x3;
            break;
        case BCMFP_L2_FORMAT_SNAP:
            *data = 0x1;
            *mask = 0x3;
            break;
        case BCMFP_L2_FORMAT_LLC:
        case BCMFP_L2_FORMAT_802DOT3:
            *data = 0x2;
            *mask = 0x7;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for L2 format from the given
 *  data and mask.
 *
 * \param [in] unit    Logical device id.
 * \param [in] data    Qualifier KEY value.
 * \param [in] mask    Qualifier MASK value.
 * \param [out] type   IP type.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
int
bcm56996_a0_fp_common_qual_l2format_get(int unit,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (mask == 0)) {
        *type = BCMFP_L2_FORMAT_ANY;
    } else if ((0 == data) && (mask == 3)) {
        *type = BCMFP_L2_FORMAT_ETHII;
    } else if ((1 == data) && (mask == 3)) {
        *type = BCMFP_L2_FORMAT_SNAP;
    } else if ((2 == data) && (mask == 7)) {
        *type = BCMFP_L2_FORMAT_LLC;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values for
 *  TPID.
 *
 * \param [in] unit      Logical device id.
 * \param [in] tpid      TPID.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
int
bcm56996_a0_fp_common_qual_tpid_set(int unit,
                                    int tpid,
                                    uint32_t *data,
                                    uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    *mask = 0x03;
    switch (tpid) {
        case BCMFP_TPID_8100:
            *data = 0x00;
            break;
        case BCMFP_TPID_9100:
            *data = 0x01;
            break;
        case BCMFP_TPID_88A8:
            *data = 0x02;
            break;
        default:
            *data = 0x03;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for TPID from the given
 *  data and mask.
 *
 * \param [in] unit        Logical device id.
 * \param [in] data        Qualifier KEY value.
 * \param [in] mask        Qualifier MASK value.
 * \param [out] tpid       TPID.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
int
bcm56996_a0_fp_common_qual_tpid_get(int unit,
                                    uint32_t data,
                                    uint32_t mask,
                                    int *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    switch (data) {
      case 0x0:
          *tpid = BCMFP_TPID_8100;
          break;
      case 0x1:
          *tpid = BCMFP_TPID_9100;
          break;
      case 0x2:
          *tpid = BCMFP_TPID_88A8;
          break;
      default:
          SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values for
 *  IP fragment info.
 *
 * \param [in] unit      Logical device id.
 * \param [in] frag_info IP fragment info.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
int
bcm56996_a0_fp_common_qual_ipfrag_set(int unit,
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
            *mask = 0x01;
            break;
        case BCMFP_IPFRAG_NON:
            /* Non-fragmented packet */
            *data = 0x01;
            *mask = 0x01;
            break;
        case BCMFP_IPFRAG_FIRST:
            /* First fragment of fragmented packet*/
            *data = 0x02;
            *mask = 0x03;
            break;
        case BCMFP_IPFRAG_NONORFIRST:
            /* Non-fragmented or first fragment */
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_IPFRAG_NOTFIRST:
            /* Not the first fragment */
            *data = 0x00;
            *mask = 0x03;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for IP fragment info from the
 *  given data and mask.
 *
 * \param [in] unit        Logical device id.
 * \param [in] data        Qualifier KEY value.
 * \param [in] mask        Qualifier MASK value.
 * \param [out] frag_info  IP fragment info.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
int
bcm56996_a0_fp_common_qual_ipfrag_get(int unit,
                                      uint32_t data,
                                      uint32_t mask,
                                      int *frag_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(frag_info, SHR_E_PARAM);

    if ((data == 0x1) && (mask == 0x1)) {
        *frag_info = BCMFP_IPFRAG_NON;
    } else  if ((data == 0x2) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_FIRST;
    } else  if ((data == 0x2) && (mask == 0x2)) {
        *frag_info = BCMFP_IPFRAG_NONORFIRST;
    } else  if ((data == 0x0) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_NOTFIRST;
    } else  if ((data == 0x0) && (mask == 0x1)) {
        *frag_info = BCMFP_IPFRAG_ANY;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values for
 *  packet color.
 *
 * \param [in] unit      Logical device id.
 * \param [in] color     Packet color.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
int
bcm56996_a0_fp_common_qual_color_set(int unit,
                                     int color,
                                     uint32_t *data,
                                     uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    *mask = 0x3;
    switch (color) {
        case BCMFP_COLOR_GREEN:
            *data = 0;
            break;
        case BCMFP_COLOR_RED:
            *data = 1;
            break;
        case BCMFP_COLOR_YELLOW:
            *data = 3;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for packet color from the given
 *  data and mask.
 *
 * \param [in] unit        Logical device id.
 * \param [in] data        Qualifier KEY value.
 * \param [in] mask        Qualifier MASK value.
 * \param [out] color      Color of the packet.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
int
bcm56996_a0_fp_common_qual_color_get(int unit,
                                     uint32_t data,
                                     uint32_t mask,
                                     int *color)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(color, SHR_E_PARAM);

    switch (data) {
      case 0:
          *color = BCMFP_COLOR_GREEN;
          break;
      case 1:
          *color = BCMFP_COLOR_RED;
          break;
      case 3:
          *color = BCMFP_COLOR_YELLOW;
          break;
      default:
          SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the key and mask values for the given enum values for
 *  packet forwarding type.
 *
 * \param [in] unit      Logical device id.
 * \param [in] type      Packet FWD type.
 * \param [out] data     Qualifier KEY value.
 * \param [out] mask     Qualifier MASK value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  Qualifier enum value is not supported.
 */
int
bcm56996_a0_fp_common_qual_fwd_type_set(int unit,
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
        case BCMFP_FWD_TYPE_L2:
            *data = 0;
            *mask = 0xE;
            break;
        case BCMFP_FWD_TYPE_L2INDEPENDENT:
        case BCMFP_FWD_TYPE_L2VPN:
            *data = 0;
            *mask = 0xF;
            break;
        case BCMFP_FWD_TYPE_L2SHARED:
            *data = 1;
            *mask = 0xF;
            break;
        case BCMFP_FWD_TYPE_L3DIRECT:
            *data = 4;
            *mask = 0xF;
            break;
        case BCMFP_FWD_TYPE_L3:
            *data = 5;
            *mask = 0xF;
            break;
        case BCMFP_FWD_TYPE_MPLS:
            *data = 7;
            *mask = 0xF;
            break;
        case BCMFP_FWD_TYPE_UNKNOWN:
            *data = 0xF;
            *mask = 0xF;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the enum value for packet forwarding type from the
 *  given data and mask.
 *
 * \param [in] unit        Logical device id.
 * \param [in] data        Qualifier KEY value.
 * \param [in] mask        Qualifier MASK value.
 * \param [out] type       Packet FWD type.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_UNAVAIL  data and mask pair is not supported.
 */
int
bcm56996_a0_fp_common_qual_fwd_type_get(int unit,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (mask == 0)) {
        *type = BCMFP_FWD_TYPE_ANY;
    } else if ((0x0 == data) && (mask == 0xE)) {
        *type = BCMFP_FWD_TYPE_L2;
    } else if ((0x0 == data) && (mask == 0xF)) {
        *type = BCMFP_FWD_TYPE_L2INDEPENDENT;
    } else if ((0x1 == data) && (mask == 0xF)) {
        *type = BCMFP_FWD_TYPE_L2SHARED;
    } else if ((0x4 == data) && (mask == 0xF)) {
        *type = BCMFP_FWD_TYPE_L3DIRECT;
    } else if ((0x5 == data) && (mask == 0xF)) {
        *type = BCMFP_FWD_TYPE_L3;
    } else if ((0x7 == data) && (mask == 0xF)) {
        *type = BCMFP_FWD_TYPE_MPLS;
    } else if ((0xF == data) && (mask == 0xF)) {
        *type = BCMFP_FWD_TYPE_UNKNOWN;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
