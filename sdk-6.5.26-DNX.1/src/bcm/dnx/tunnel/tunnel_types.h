/** \file tunnel_types.h
 * Tunnel types functionality for DNX for tunnel files
*/

#ifndef _TUNNEL_TYPES_INCLUDED__
/*
 * {
 */
#define _TUNNEL_TYPES_INCLUDED__

/** IP ANY */
#define DNX_TUNNEL_TYPE_IS_IP_ANY(_tunnel_type)                         ((_tunnel_type == bcmTunnelTypeIpAnyIn4) || (_tunnel_type == bcmTunnelTypeIpAnyIn6))

/** GRE */
#define DNX_TUNNEL_TYPE_IS_GRE(_tunnel_type)                            ((_tunnel_type == bcmTunnelTypeGreAnyIn4) || (_tunnel_type == bcmTunnelTypeGreAnyIn6))
/** UDP */
#define DNX_TUNNEL_TYPE_IS_UDP(_tunnel_type)                            ((_tunnel_type == bcmTunnelTypeUdp) || (_tunnel_type == bcmTunnelTypeUdp6))

/** VXLAN */
#define DNX_TUNNEL_TYPE_IS_VXLAN(_tunnel_type)                          ((_tunnel_type == bcmTunnelTypeVxlan)    || (_tunnel_type == bcmTunnelTypeVxlan6) || (_tunnel_type == bcmTunnelTypeVxlanEsp))
#define DNX_TUNNEL_TYPE_IS_VXLAN_GPE(_tunnel_type)                      ((_tunnel_type == bcmTunnelTypeVxlanGpe) || (_tunnel_type == bcmTunnelTypeVxlan6Gpe))
#define DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE(_tunnel_type)             (DNX_TUNNEL_TYPE_IS_VXLAN(_tunnel_type) || DNX_TUNNEL_TYPE_IS_VXLAN_GPE(_tunnel_type))

/** GENEVE */
#define DNX_TUNNEL_TYPE_IS_GENEVE(_tunnel_type)                          ((_tunnel_type == bcmTunnelTypeGeneve) || (_tunnel_type == bcmTunnelTypeGeneve6))

/** VXLAN or GENEVE */
#define DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE_OR_GENEVE(_tunnel_type)    (DNX_TUNNEL_TYPE_IS_GENEVE(_tunnel_type) || DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE(_tunnel_type))

/** SR6 */
#define DNX_TUNNEL_TYPE_IS_SR6(_tunnel_type)                             ((_tunnel_type == bcmTunnelTypeSR6) || (_tunnel_type == bcmTunnelTypeEthSR6))

/** is ipv6 */
#define DNX_TUNNEL_TYPE_IS_IPV6(_tunnel_type)           \
        ((_tunnel_type == bcmTunnelTypeIpAnyIn6)        \
            || (_tunnel_type == bcmTunnelTypeGreAnyIn6) \
            || (_tunnel_type == bcmTunnelTypeVxlan6)    \
            || (_tunnel_type == bcmTunnelTypeVxlan6Gpe) \
            || (_tunnel_type == bcmTunnelTypeGeneve6)   \
            || (_tunnel_type == bcmTunnelTypeUdp6)      \
            || (_tunnel_type == bcmTunnelTypeSR6)       \
            || (_tunnel_type == bcmTunnelTypeEthSR6)    \
            || (_tunnel_type == bcmTunnelTypeEthIn6))

#endif  /*_TUNNEL_TYPES_INCLUDED__ */
