/*! \file types.h
 *
 * Types header file.
 * This file contains LTSW types definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_TYPES_H
#define BCMI_LTSW_TYPES_H

#include <bcm/types.h>

#define BCMI_LTSW_TUNNEL_TYPE_OFFSET    20
#define BCMI_LTSW_TUNNEL_TYPE_MASK      0x3F
#define BCMI_LTSW_TUNNEL_ID_OFFSET      0
#define BCMI_LTSW_TUNNEL_ID_MASK        0xFFFFF

typedef enum bcmi_ltsw_tunnel_type_e {
    /* Reserved type ID. */
    bcmiTunnelTypeInvalid = 0,

    /* Flex flow tunnel type. */
    bcmiTunnelTypeFlexFlow = 1,

    /* L2 MPLS tunnel type. */
    bcmiTunnelTypeMplsL2 = 2,

    /* L3 MPLS tunnel type. */
    bcmiTunnelTypeMplsL3 = 3,

    /* L3 IPv4 tunnel type. */
    bcmiTunnelTypeIpL3 = 4,

    /* L3 IP tunnel IPv6 type. */
    bcmiTunnelTypeIp6L3 = 5,

    /* MPLS tunnel type. */
    bcmiTunnelTypeMpls = 6,

    /* Should be at last. */
    bcmiTunnelTypeCount
} bcmi_ltsw_tunnel_type_t;

#define BCMI_LTSW_TUNNEL_TYPE_VALID(_tunnel_type) \
            (((_tunnel_type <= bcmiTunnelTypeInvalid) || \
             (_tunnel_type >= bcmiTunnelTypeCount)) ? 0 : 1 )

#define BCMI_LTSW_GPORT_TUNNEL_ID_SET(_gport, _tunnel_type, _tunnel_id) \
            do { \
                if (BCMI_LTSW_TUNNEL_TYPE_VALID(_tunnel_type)) { \
                    BCM_GPORT_TUNNEL_ID_SET(_gport, _tunnel_id); \
                    _gport |= \
                        (_tunnel_type << BCMI_LTSW_TUNNEL_TYPE_OFFSET);\
                } else { \
                    _gport = -1; \
                } \
            } while (0)

#define BCMI_LTSW_GPORT_TUNNEL_ID_GET(_gport, _tunnel_type, _tunnel_id) \
            do { \
                _tunnel_type = ((_gport >> BCMI_LTSW_TUNNEL_TYPE_OFFSET) & \
                                        BCMI_LTSW_TUNNEL_TYPE_MASK); \
                _tunnel_id = (_gport >> BCMI_LTSW_TUNNEL_ID_OFFSET) & \
                                     BCMI_LTSW_TUNNEL_ID_MASK; \
            } while (0)

/*!
 * \brief Get scalar value from symbol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] val scalar value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*bcmi_ltsw_symbol_to_scalar_f)(int unit, const char *symbol,
                                            uint64_t *val);

/*!
 * \brief Get symbol from scalar value.
 *
 * \param [in] unit Unit number.
 * \param [in] val scalar value.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*bcmi_ltsw_scalar_to_symbol_f)(int unit, uint64_t val,
                                            const char **symbol);

#endif /* BCMI_LTSW_TYPES_H */
