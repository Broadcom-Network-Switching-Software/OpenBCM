/** \file in_lif_profile.h
 *
 * in_lif profile management for DNX.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifndef IN_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define IN_LIF_PROFILE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <include/bcm_int/dnx/rx/rx.h>
#include <bcm/l3.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
/***************************************************************************************/
/*
 * Defines
 */
/***************************************************************************************/
/*
 * number of bits that are used for in_lif_profile and eth_rif_profile
 */
#define NOF_ETH_RIF_PROFILE_BITS dnx_data_lif.in_lif.nof_used_eth_rif_profile_bits_get(unit)
/*
 * in_lif_profile in HW is 8 bits but in order to have a simpler implementation given all the restrictions we decided
 * to deal with only 7 bits out of the 8
 */
#define NOF_IN_LIF_PROFILE_BITS dnx_data_lif.in_lif.nof_used_in_lif_profile_bits_get(unit)

/*
 * define number of bits and start bits for PMF reserved bits in in_lif_profile and eth_rif_profile
 */
#define ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit)
#define ETH_RIF_PROFILE_PMF_RESERVED_START_BIT dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit)
#define IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS dnx_data_field.profile_bits.nof_ing_in_lif_get(unit)
#define IN_LIF_PROFILE_PMF_RESERVED_START_BIT dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit)

/*
 * define number of bits for egress in_lif_profile bits according to soc properties
 */
#define IN_LIF_ORIENTATION_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get(unit)
#define IN_LIF_ORIENTATION_NOF_BITS dnx_data_lif.in_lif.nof_in_lif_profile_allocate_orientation_bits_get(unit)
#define IVE_INNER_QOS_DP_PROFILE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_policer_inner_dp_get(unit)
#define IVE_INNER_QOS_DP_PROFILE_NOF_BITS dnx_data_lif.in_lif.nof_in_lif_profile_allocate_policer_inner_dp_bits_get(unit)
#define IVE_OUTER_QOS_DP_PROFILE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_policer_outer_dp_get(unit)
#define IVE_OUTER_QOS_DP_PROFILE_NOF_BITS dnx_data_lif.in_lif.nof_in_lif_profile_allocate_policer_outer_dp_bits_get(unit)
#define SAME_INTERFACE_MODE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit)
#define SAME_INTERFACE_MODE_NOF_BITS dnx_data_lif.in_lif.nof_in_lif_profile_allocate_same_interface_mode_bits_get(unit)

/*
 * define number of bits for native indexed mode in_lif_profile bits according to soc properties
 */
#define NATIVE_INDEXED_MODE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_indexed_mode_get(unit)
#define NATIVE_INDEXED_MODE_NOF_BITS dnx_data_lif.in_lif.nof_in_lif_profile_allocate_indexed_mode_bits_get(unit)
/*
 * defines the number of values, that should be allocated to support PWE indexed mode indication
 */
#define IN_LIF_PROFILE_PWE_INDEXED_MODE_NOF_VALUES 2
/*
 * define number of bits and start bits for EGRESS reserved bits in in_lif_profile and eth_rif_profile
 * meanwhile we support only IN_LIF_PROFILE egress reserved bits
 */
#define ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS dnx_data_lif.in_lif.eth_rif_profile_egress_reserved_nof_bits_get(unit)
#define ETH_RIF_PROFILE_EGRESS_RESERVED_START_BIT dnx_data_lif.in_lif.eth_rif_profile_egress_reserved_start_bit_get(unit)
#define IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS dnx_data_lif.in_lif.nof_in_lif_egress_reserved_bits_get(unit)
#define IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT dnx_data_lif.in_lif.in_lif_profile_egress_reserved_start_bit_get(unit)
#define IN_LIF_PROFILE_NATIVE_INDEXED_MODE_START_BIT dnx_data_lif.in_lif.in_lif_profile_native_indexed_mode_start_bit_get(unit)
/*
 * defines the max number of supported bits for in_lif_profile in JR sys header mode
 */
#define IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE 2
#define IN_LIF_PROFILE_EGRESS_NOF_BITS_DSCP_MODE 1
#define JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES 1
#define JR_COMP_MODE_ONLY_DEV_SCOPE_ENABLED_BY_SOC_PROPERTIES 2
#define JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES 3
#define PRESERVE_DSCP_PER_OUTLIF_AND_INLIF 0x1
#define JR_COMP_MODE_SAME_IF_MODE_1B_ALLOCATION(_property) ((_property == JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES) || (_property == JR_COMP_MODE_ONLY_DEV_SCOPE_ENABLED_BY_SOC_PROPERTIES))
#define JR_COMP_MODE_SAME_IF_MODE_2B_ALLOCATION(_property) (_property == JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
#define JR_COMP_MODE_SAME_PRESERVE_DSCP_PER_INLIF(_property) (_property == PRESERVE_DSCP_PER_OUTLIF_AND_INLIF)
/*
 * number of supported in_lif_profiles after reserved bits
 */
#define NOF_IN_LIF_PROFILES (1 << (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS))
#define NOF_IN_LIF_PROFILE_INGRESS_BITS (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS)
/*
 * number of supported eth_rif_profiles after reserved bits
 */
#define NOF_ETH_RIF_PROFILES (1 << (NOF_ETH_RIF_PROFILE_BITS - ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS - ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS))
#define NOF_ETH_RIF_PROFILE_INGRESS_BITS (NOF_ETH_RIF_PROFILE_BITS - ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS - ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS)

/*
 * Routing Enablers Defines
 */
/**
 * Specify all routing enablers are disabled
 */
#define DEFAULT_ROUTING_ENABLERS_PROFILE (0x0)
/*
 * The size of the routing enable vector in units of uint32
 */
#define ROUTING_ENABLERS_VECTOR_SIZE_IN_UINT32(unit) (BITS2WORDS(dnx_data_l3.routing_enablers.routing_enable_vector_length_get(unit)))

/**
 * number of enablers vector profiles
 */
#define NOF_ENTRIES_ENABLERS_VECTOR        dnx_data_lif.in_lif.nof_routing_enable_profiles_get(unit)
/*
 * supported enablers flags
 */
#define ENABLERS_DISABLE_SUPPORTED_FLAGS (BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST| \
        BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST|BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST| \
            BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST|BCM_L3_INGRESS_ROUTE_DISABLE_MPLS)
/***************************************************************************************/
/*
 * flags for setting cs_in_lif_profile value
 */
/**
 * For PWE
 */
#define DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED (1 << 0)

/**
 * For EVPN_IML
 */
#define DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_LIF_SCOPED  (1 << 0)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_NO_FL    (1 << 1)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_FL_ONLY  (1 << 2)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_FL_CW    (1 << 3)

/**
 * For LIF
 */
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_VIRTUAL_IP       (1 << 0)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_MPLS_NAMESPACE   (1 << 1)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_NAT              (1 << 2)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_AACL             (1 << 3)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_SRV6_GSID        (1 << 4)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_VXLAN_OVER_ESP        (1 << 5)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_VXLAN_VNI2LIF    (1 << 6)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_L2VPN            (1 << 7)
#define DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_L2VPN_LIF_SCOPED (1 << 8)

/***************************************************************************************/
/*
 * Default profiles/values defines
 */
/**
 * Specify default RPF mode is RPF disabled
 */
#define DEFAULT_RPF_MODE (DBAL_ENUM_FVAL_RPF_MODE_DISABLE)
/**
 * Specify default Context-Selection value when no specific action is required for the in_lif_profile (or on the case of init/clear) in VTT stage
 */
#define DNX_IN_LIF_PROFILE_CS_DEFAULT (0)
/**
 * Specify default load balancing flags disablers vector value.
 */
#define DNX_IN_LIF_LB_PROFILE_DISABLERS_FLAGS_DEFAULT (0)
/**
 * Specify default Context-Selection value when no specific action is required for the in_lif_profile (or on the case of init/clear) in FWD stage
 */
#define DNX_IN_LIF_FWD_PROFILE_CS_DEFAULT (0)
/**
 * Specify default in_lif_profile
 */
#define DNX_IN_LIF_PROFILE_DEFAULT (0)
/**
 * Specify default data for trap profiles
 */
#define DEFAULT_TRAP_PROFILES_DATA (0)
/**
 * Specify default in_lif_orientation
 */
#define DEFAULT_IN_LIF_ORIENTATION (0)
/**
 * Specify default VSI Profile IVL/SVL mode is SVL
 */
#define DEFAULT_VSI_PROFILE_IVL_SVL_MODE (DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_SVL)
/**
 * Specify default OAM Profile
 */
#define DEFAULT_OAM_PROFILE (0)
/**
 * Specify default native indexed mode
 */
#define DEFAULT_NATIVE_INDEXED_MODE (0)
/**
 * Specify default LIF LB Profile
 */
#define DEFAULT_IN_LIF_LB_PROFILE (0)
/**
 * Specify default DA not found destination profile
 */
#define DNX_DA_NOT_FOUND_DST_PROFILE_DEFAULT (0)
/**
 * Specify default in_lif_orientation
 */
#define DEFAULT_IN_LIF_ORIENTATION (0)
/**
 * Specify default ive_outer_qos_dp_profile
 */
#define DEFAULT_IN_LIF_OUTER_DP_PROFILE (0)
/**
 * Specify default ive_inner_qos_dp_profile
 */
#define DEFAULT_IN_LIF_INNER_DP_PROFILE (0)
/**
 * Specify default lif_same_interface_mode
 */
#define DEFAULT_IN_LIF_SAME_INTERFACE_MODE (0)
/**
 * Specify default field_class_id
 */
#define DEFAULT_IN_LIF_FIELD_CLASS_ID (0)
/**
 * Specify default field_class_cs_id
 */
#define DEFAULT_IN_LIF_FIELD_CLASS_CS_ID (0)
/***************************************************************************************/
/*
 * supported lb profile enablers flags
 */
#define LB_ENABLERS_DISABLE_SUPPORTED_FLAGS (BCM_HASH_LAYER_ETHERNET_DISABLE | \
            BCM_HASH_LAYER_IPV4_DISABLE | BCM_HASH_LAYER_IPV6_DISABLE | \
            BCM_HASH_LAYER_MPLS_DISABLE | BCM_HASH_LAYER_UDP_DISABLE | \
            BCM_HASH_LAYER_TCP_DISABLE)
/***************************************************************************************/
/*
 * Masks,Prefixes and Offsets for relation between ETH_RIF_PROFILE and VSI_PROFILE/IN_LIF_PROFILE
 */
/*
 * the static prefix of VSI_PROFILE 3 MSBs when we access routing_profiles table with ETH_RIF profile
 */
#define VSI_MSB_PREFIX 0x7
/*
 * the static prefix of IN_LIF_PROFILE MSB bit when we access IN_LIF_PROFILE_TABLE table with ETH_RIF profile
 */
#define IN_LIF_PROFILE_TABLE_MSB_PREFIX 0x1
/*
 * used for VSI_TABLE dbal operations
 */
#define VSI_MSB_BITS_MASK_FOR_ETH_RIF_PROFILE (0x380)
#define VSI_LSB_BITS_MASK_FOR_ETH_RIF_PROFILE (0x7F)
/*
 * used for IN_LIF_PROFILE_TABLE dbal operations
 */
#define IN_LIF_PROFILE_MSB_BITS_MASK_FOR_ETH_RIF_PROFILE (0x80)
#define IN_LIF_PROFILE_LSB_BITS_MASK_FOR_ETH_RIF_PROFILE (0x7F)
/***************************************************************************************/
/*
 * Macros
 */
/*
 * calculates VSI_PROFILE from ETH_RIF profile by accumulating MSB prefix and ETH_RIF profile
 */
#define CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(vsi_msb_part,vsi_lsb_part)   (((vsi_msb_part << NOF_ETH_RIF_PROFILE_BITS) & VSI_MSB_BITS_MASK_FOR_ETH_RIF_PROFILE) | (vsi_lsb_part & VSI_LSB_BITS_MASK_FOR_ETH_RIF_PROFILE))
/*
 * calculates IN_LIF_PROFILE from ETH_RIF profile by accumulating MSB prefix and ETH_RIF profile
 */
#define CALCULATE_IN_LIF_PROFILE_FROM_ETH_RIF_PROFILE(vsi_msb_part,vsi_lsb_part)   (((vsi_msb_part << NOF_ETH_RIF_PROFILE_BITS)& IN_LIF_PROFILE_MSB_BITS_MASK_FOR_ETH_RIF_PROFILE) | (vsi_lsb_part & IN_LIF_PROFILE_LSB_BITS_MASK_FOR_ETH_RIF_PROFILE))
/***************************************************************************************/
/*
 * Enums
 */

typedef enum
{
    /**
     * routing enable type is UC
     */
    ROUTING_ENABLE_UC = 0,
    /**
     * routing enable type is MC
     */
    ROUTING_ENABLE_MC = 1,
   /**
     * number of routing enable type
     */
    NOF_ROUTING_ENABLE_TYPES
} routing_enable_type_e;

typedef enum
{
    /**
     * interface type is ETH-RIF
     */
    ETH_RIF,
    /**
     * interface type is LIF
     */
    LIF,
    /**
     * interface type is PWE
     */
    PWE,
    /**
     * interface type is EVPN
     */
    EVPN_IML
} in_lif_intf_type_e;

typedef enum
{
    /**
     * public routing is disabled
     */
    PUBLIC_ROUTING_DISABLED,
    /**
     * public routing is enabled
     */
    PUBLIC_ROUTING_ENABLED
} in_lif_public_routing_mode_e;

typedef enum
{
    /**
     * number of ethernet tags is 0
     */
    ZERO_TAGS,
    /**
     * number of ethernet tags is 1
     */
    ONE_TAG,
    /**
     * number of ethernet tags is 2
     */
    TWO_TAGS,
    /**
     * number of ethernet tags could be 0, 1, 2
     */
    X_TAGS
} in_lif_pwe_tagged_nof_eth_tags_e;

typedef enum
{
    /**
     * device scope lif
     */
    DEVICE_SCOPE = 0,
    /**
     * system scope lif
     */
    SYSTEM_SCOPE = 1,
    /**
     * disable same interface filter
     */
    DISABLE = 2,
    /**
     * disable same interface filter for device scope
     */
    DEVICE_SCOPE_DISABLE = 3,
    /**
     * disable same interface filter for system scope
     */
    SYSTEM_SCOPE_DISABLE = 4
} in_lif_same_interface_mode_e;

typedef enum
{
    /**
     * preserve DSCP mode is disabled
     */
    DISABLE_PRESERVE_DSCP_MODE = 0,
    /**
     * preserve DSCP mode is enabled
     */
    ENABLE_PRESERVE_DSCP_MODE = 1
} preserve_dscp_mode_e;
/***************************************************************************************/
/*
 * Structures
 */
typedef struct
{
    /*
     * Layer type
     */
    dbal_enum_value_field_layer_types_e layer_type;
    /*
     * MC enable value
     */
    uint8 mc_enable;
    /*
     * UC enable value
     */
    uint8 uc_enable;
} layer_types_enablers_t;
/**
 * /brief - in_lif template data structure.
 * The template manager's data of in_lif_profile, including all inner profiles and all fields.
 * this structure is for inner usage only.
 */
typedef struct
{
    /**
     * routing enablers profile
     */
    uint8 routing_enablers_profile;
    /**
     * RPF mode for IPv4 packets
     * /see dbal_enum_value_field_rpf_mode_e in dbal_defines_enum_field_types.h
     */
    dbal_enum_value_field_rpf_mode_e rpf_mode_ipv4;
    /**
     * RPF mode for IPv6 packets
     * /see dbal_enum_value_field_rpf_mode_e in dbal_defines_enum_field_types.h
     */
    dbal_enum_value_field_rpf_mode_e rpf_mode_ipv6;
    /**
     * CS VTT in_lif profile
     */
    uint8 cs_vtt_in_lif_profile;
    /**
     * CS VTT to FLP in_lif profile
     */
    uint8 cs_vtt_to_flp_in_lif_profile;
    /**
     * CS in_lif FWD profile
     */
    uint8 cs_flp_in_lif_profile;
    /**
     * Protocol trap profiles structure
     */
    bcm_rx_trap_protocol_profiles_t protocol_trap_profiles;
    /**
     * InLif Destination profile for unknown DA
     */
    uint8 da_not_found_dst_profile;
    /*
     * MACT mode, used only for RIF/VSI type
     */
    dbal_enum_value_field_vsi_profile_bridge_e mact_mode;
    /*
     * L2 IPv4 MC forward type, used only for RIF/VSI type
     */
    dbal_enum_value_field_l2_v4_mc_fwd_type_e l2_v4_mc_fwd_type;
    /*
     * L2 IPv6 MC forward type, used only for RIF/VSI type
     */
    dbal_enum_value_field_l2_v6_mc_fwd_type_e l2_v6_mc_fwd_type;
    /*
     * OAM Default Profile
     */
    int oam_default_profile;
    /**
     * LB inlif profile
     */
    uint8 in_lif_lb_profile;
    /**
     * In Lif orientation
     */
    uint8 in_lif_orientation;
    /**
     * in_lif outer dp profile for ive
     */
    uint8 ive_outer_qos_dp_profile;
    /**
     * in_lif inner dp profile for ive
     */
    uint8 ive_inner_qos_dp_profile;
    /**
     * same interface mode - supported are device and system scope
     */
    in_lif_same_interface_mode_e lif_same_interface_mode;
    /**
     * Field class ID
     */
    int field_class_id;
} in_lif_template_data_t;

/**
 * /brief - Default forwarding destination structure.
 *  It is used in in_lif_profile_info_t to exchange a in-lif-flood profile.
 *  The profile will take part in the exchanges of in-lif-profile.
 */
typedef struct
{
    /*
     * port/LIF default destination.
     */
    uint32 destination;

    /*
     * Add the offset to destination when add_vsi is TRUE.
     */
    uint32 offset;

    /*
     * If TRUE, Indicate that the forward destination is calculated as:
     * forward-destination = vsi.default_destination + offset.
     * If FALSE, the destination above is used as forward destination.
     * This is useful to define the LIF default forwarding destination.
     */
    uint8 add_vsi_dst;

} dnx_default_frwrd_dst_t;

/**
 * \brief - the input structure of dnx_in_lif_profile_exchange,
 * includes the in_lif profile properties to set.
 * \remark
 *  need to call in_lif_profile_info_t_init in order to initiate the fields to the default valid values
 *  and then to update the structure and send it to the API.
 */
typedef struct
{
    /**
     * Interface flags, supported flags are:
     * BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST
     * BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST
     * BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST
     * BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST
     * BCM_L3_INGRESS_ROUTE_DISABLE_MPLS
     * BCM_L3_INGRESS_GLOBAL_ROUTE
     * BCM_L3_INGRESS_MPLS_INTF_NAMESPACE
     * BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN
     */
    uint32 l3_ingress_flags;
    /**
     * If set, allow termination when layer above is UDP or TCP
     */
    uint8 allow_udp_tcp_above_termination;
    /**
     * URPF (Unicast Reverse Path Forwarding) mode setting for L3 Ingress
     */
    bcm_l3_ingress_urpf_mode_t urpf_mode;
    /**
     * number of inner ethernet tags
     */
    in_lif_pwe_tagged_nof_eth_tags_e pwe_nof_eth_tags;
    /**
     * Protocol trap profiles structure
     */
    bcm_rx_trap_protocol_profiles_t protocol_trap_profiles;
    /**
     * Additional flags that may change the cs_in_lif_profile value
     * Note: Other fields may change the cs profile (e.g. l3_ingress_flags)
     * supported flags are:
     * DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED
     */
    uint32 cs_in_lif_profile_flags;
    /**
     * Default Destinations for unknown DA
     */
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];
    /**
      * Vlan forwarding and learning mode
      * Indicate if it is SVL (shared vlan learning) or IVL (independent vlan learning)
      * '0' means SVL, '1' means IVL
      * Relevant only for VSI Profile
      */
    dbal_enum_value_field_vsi_profile_bridge_e mact_mode;
    /**
      * L2 multicast fwd type
      * '0' means Bridge, '1' means IPv4MC
      * Relevant only for VSI Profile
      */
    dbal_enum_value_field_l2_v4_mc_fwd_type_e l2_v4_mc_fwd_type;
    /*
     * L2 IPv6 MC forward type, used only for RIF/VSI type
     * '0' means Bridge, '1' means IPv6MC
     * Relevant only for VSI Profile
     */
    dbal_enum_value_field_l2_v6_mc_fwd_type_e l2_v6_mc_fwd_type;
    /*
     * OAM Default Profile
     */
    int oam_default_profile;
    /*
     *  load balancing Disable flags, valid values are:
     *  BCM_HASH_LAYER_ETHERNET_DISABLE
     *  BCM_HASH_LAYER_IPV4_DISABLE
     *  BCM_HASH_LAYER_IPV6_DISABLE
     *  BCM_HASH_LAYER_MPLS_DISABLE
     *  BCM_HASH_LAYER_TCP_DISABLE
     *  BCM_HASH_LAYER_UDP_DISABLE
     *
     */
    uint32 lb_per_lif_flags;
    /**
     * LB inlif profile
     */
    uint8 in_lif_lb_profile;
    /*
     * LIF GPORT
     */
    bcm_gport_t lif_gport;
    uint8 is_default_in_lif;
    /*
     * Indicate whether to show LIF created on LAG
     */
    uint8 lif_on_lag_is_shown;
    /*
     * Indicate whether enable oam primary vlan mode on lif
     */
    uint8 oam_primary_vlan_mode;

    dbal_enum_value_field_ip_address_spoofing_check_modes_e ip_anti_spoofing_check_mode;
} in_lif_profile_info_ingress_t;

typedef struct
{
    /**
     * in_lif orientation
     */
    uint8 in_lif_orientation;
    /**
     * in_lif outer dp profile for ive
     */
    uint8 ive_outer_qos_dp_profile;
    /**
     * in_lif inner dp profile for ive
     */
    uint8 ive_inner_qos_dp_profile;
    /**
     * same interface mode - supported are device and system scope
     */
    in_lif_same_interface_mode_e lif_same_interface_mode;
    /**
     * in_lif_profile in jr_mode
     */
    int in_lif_profile_jr_mode;
    /*
     * Preserve DSCP mode
     */
    preserve_dscp_mode_e preserve_dscp_mode;
} in_lif_profile_info_egress_t;

typedef struct
{
    int field_class_id;
    in_lif_profile_info_ingress_t ingress_fields;
    in_lif_profile_info_egress_t egress_fields;
    uint8 native_indexed_mode;
} in_lif_profile_info_t;

/**
 * A structure that is used to indicate whether 
 * working on the last LIF profile and linked LIF profiles.
 */
typedef struct
{
    /**
     * Last routing enablers profile
     */
    uint8 old_routing_enablers_profile;
    /**
     * Last CS in_lif profile
     */
    uint8 old_cs_vtt_in_lif_profile;
    /**
     * Last CS in_lif profile
     */
    uint8 old_cs_vtt_to_flp_in_lif_profile;
    /**
     * Last CS in_lif FWD profile
     */
    uint8 old_cs_flp_in_lif_profile;
    /**
     * Last InLif Destination profile for unknown DA
     */
    uint8 old_da_not_found_dst_profile;
    /**
     * Last LB inlif profile
     */
    uint8 old_in_lif_lb_profile;
    /**
     * Last In LIF profile
     */
    uint32 old_in_lif_profile;
} dnx_in_lif_profile_last_info_t;

/***************************************************************************************/
/*
 * API
 */
/**
 * \brief
 *   initializes the module in_lif_profile:
 *   writes default profiles/values to relevant tables, for example:
 *   IN_LIF_PROFILE_TABLE
 *   VSI_PROFILE_TABLE
 *   ENABLERS_VECTORS
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * updates dbal tables IN_LIF_PROFILE_TABLE,VSI_PROFILE_TABLE,ENABLERS_VECTORS
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_module_init(
    int unit);

/**
 * \brief -
 *     Initialise the dnx_in_lif_profile_last_info_t structure.
 */
void dnx_in_lif_profile_last_info_t_init(
    dnx_in_lif_profile_last_info_t * last_profiles);
/**
 * \brief
 *   deallocates in_lif_profile and returns the default in_lif_profile
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile -
 *   in_lif_profile id to deallocate
 * \param [out] new_in_lif_profile -
 *   Pointer to new_in_lif_profile \n
 *     This procedure loads pointed memory by the newly assigned in_lif_profile
 *     id in template manager
 * \param [in] in_lif_profile_info -
 *   Pointer to in_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties
 * \param [in] intf_type -
 *   can be one of the following:
 *       ETH_RIF - interface type is ETH_RIF
 *       LIF - interface type is LIF
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   We don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the InLIF
 *   to the new place traffic will get some "undefine" state.
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_dealloc(
    int unit,
    int in_lif_profile,
    int *new_in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info,
    in_lif_intf_type_e intf_type);

/**
 * \brief
 *   Destroy the in_lif_profile (lif type or ETH_RIF) and all of its properties.
 * \param [in] unit -
 *   The unit number.
 * \param [in] last_profile -
 *   structure holding all last profiles to cleared from DBAL
 * \param [in] intf_type -
 *   can be one of the following:
 *       ETH_RIF - interface type is ETH_RIF
 *       LIF - interface type is LIF
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 * 1. This function assumes exchange was called before and its is_last indication is set.
 * 2. This function should be called after there are no references to the old in LIF profile.
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_destroy(
    int unit,
    dnx_in_lif_profile_last_info_t last_profile,
    in_lif_intf_type_e intf_type);

/**
 * \brief
 *  Updates the template manager for LIF/RIF profiles, and exchanges the old in_lif_profile with
 *   the new in_lif_profile according to LIF properties which are stored in in_lif_profile_info.
 *   In case new_in_lif_profile is the first reference, HW configuration (DBAL tables) will be configured accordingly.
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile_info -
 *   Pointer to in_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties
 * \param [in] old_in_lif_profile -
 *   old in_lif_profile id to exchnage
 * \param [out] new_in_lif_profile -
 *   Pointer to new_in_lif_profile \n
 *     This procedure loads pointed memory by the newly assigned in_lif_profile
 *     id in template manager
 * \param [in] intf_type -
 *   can be one of the following:
 *       ETH_RIF - interface type is ETH_RIF
 *       LIF - interface type is LIF
 * \param [in] inlif_dbal_table_id -
 *   used to retrieve dbal table information about the LIF
 * \param [out] last_profile -
 *   used to return an indication if there are no references to the profiles
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   We don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the InLIF
 *   to the new place traffic will get some "undefine" state.
 *   Instead, it is expected to call later to dnx_in_lif_profile_destroy in case is_last is true.
 * \see
 *   * dnx_in_lif_profile_algo_handle_routing_enablers_profile
 *   * dnx_in_lif_profile_algo_convert_to_rpf_mode
 *   * dnx_algo_template_exchange
 *   * DBAL_TABLE_IN_LIF_PROFILE_TABLE
 *   * NOF_ROUTING_PROFILES_DUPLICATES
 */

shr_error_e dnx_in_lif_profile_exchange(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info,
    int old_in_lif_profile,
    int *new_in_lif_profile,
    in_lif_intf_type_e intf_type,
    dbal_tables_e inlif_dbal_table_id,
    dnx_in_lif_profile_last_info_t * last_profile);

/**
 * \brief
 *   gets in_lif_profile and interface type and returns references count
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile -
 *   in_lif_profile id to get ref_count for
 * \param [out] ref_count -
 *   Pointer to ref_count \n
 *     Pointed memory includes references count of the in_lif_profile
 * \param [in] intf_type -
 *   can be one of the following:
 *       ETH_RIF - interface type is RIF
 *       LIF - interface type is LIF
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_get_ref_count(
    int unit,
    int in_lif_profile,
    int *ref_count,
    in_lif_intf_type_e intf_type);

/**
 * \brief
 *   gets in_lif_profile and interface type and returns in_lif_profile data
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile -
 *   in_lif_profile id to get data for
 * \param [out] in_lif_profile_info -
 *   Pointer to in_lif_profile_info \n
 *     Pointed memory includes in_lif_profile properties
 * \param [in] inlif_dbal_table_id -
 *   used to retrieve dbal table information about the LIF
 * \param [in] intf_type -
 *   can be one of the following:
 *       ETH_RIF - interface type is ETH_RIF
 *       LIF - interface type is LIF
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_get_data(
    int unit,
    int in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info,
    dbal_tables_e inlif_dbal_table_id,
    in_lif_intf_type_e intf_type);

/*
 * \brief
 *      initialize fields of the structure in_lif_profile_info_t to the default values
 * \param [in] unit -
 *   The unit number.
 * \param [out] in_lif_profile_info -
 *   Pointer to in_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties initiated to the default values
 * \return
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e in_lif_profile_info_t_init(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info);

/**
 * \brief -
 *  extracts egress in_lif_profile values from in_lif_profile
 *
 * \param [in] unit - unit number
 * \param [in] in_lif_profile - the whole in_lif_profile value
 * \param [in] intf_type - intf type
 * \param [out] egress_in_lif_profile_info - the return struct that includes egress in_lif_profile fields values
 * \param [out] in_lif_profile_jr_mode - the inlif profile in jr sys headers mode
 * \param [out] in_lif_profile_exclude_orientation - the inlif profile in jr sys headers mode excluding orientation
 * \param [out] in_lif_orientation - the inlif orientation in jr sys headers mode
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
shr_error_e dnx_in_lif_profile_algo_extract_egress_profile(
    int unit,
    int in_lif_profile,
    in_lif_profile_info_egress_t * egress_in_lif_profile_info,
    int *in_lif_profile_jr_mode,
    int *in_lif_profile_exclude_orientation,
    int *in_lif_orientation,
    in_lif_intf_type_e intf_type);

/**
 * \brief -
 *  extracts egress attributes from lif profile
 *
 * \param [in] unit - unit number
 * \param [in] in_lif_profile - the whole in_lif_profile value
 * \param [out] egress_in_lif_profile_info - the return struct that includes egress in_lif_profile fields values
 * \param [out] in_lif_profile_jr_mode - the inlif profile in jr sys headers mode
 * \param [out] in_lif_profile_exclude_orientation - the inlif profile in jr sys headers mode excluding orientation
 * \param [out] in_lif_orientation - the inlif orientation in jr sys headers mode
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
shr_error_e dnx_in_lif_profile_algo_extract_egress_profile_and_orientation_iop_mode(
    int unit,
    int in_lif_profile,
    in_lif_profile_info_egress_t * egress_in_lif_profile_info,
    int *in_lif_profile_jr_mode,
    int *in_lif_profile_exclude_orientation,
    int *in_lif_orientation);

/**
 * \brief -
 *  extracts egress eth_rif_profile values from eth_rif_profile
 *
 * \param [in] unit - unit number
 * \param [in] eth_rif_profile - the whole eth_rif_profile value
 * \param [out] egress_in_lif_profile_info - the return struct that includes egress lif_profile fields values
 * \param [out] eth_rif_profile_jr_mode - the eth_rif profile in jr sys headers mode
 * \param [out] eth_rif_profile_exclude_orientation - the eth_rif profile in jr sys headers mode excluding orientation
 * \param [out] eth_rif_orientation - the eth_rif orientation in jr sys headers mode
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
shr_error_e dnx_eth_rif_profile_algo_extract_egress_profile(
    int unit,
    int eth_rif_profile,
    in_lif_profile_info_egress_t * egress_in_lif_profile_info,
    int *eth_rif_profile_jr_mode,
    int *eth_rif_profile_exclude_orientation,
    int *eth_rif_orientation);

/**
 * \brief -
 *  Retrieves data for the VSI/LIF profile
 * \param [in] unit - unit number
 * \param [in] gport - The ETH_RIF/LIF interface ID.
 * \param [out] intf_profile - the retrieved profile ID of the interface (LIF/VSI profile)
 * \param [out] result_type - the result type of the VSI profile entry
 * \param [out] is_ip_tunnel_l2vpn - It's a IP tunnel for L2VPN if it's set.
 * \param [in] intf_type - interface type ETH_RIF or LIF.
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   none
 */
shr_error_e dnx_in_lif_vsi_profile_get(
    int unit,
    bcm_port_t gport,
    uint32 *intf_profile,
    uint32 *result_type,
    uint8 *is_ip_tunnel_l2vpn,
    in_lif_intf_type_e intf_type);

/**
 * \brief -
 *  Sets the profile data for the VSI/LIF profile
 * \param [in] unit - unit number
 * \param [in] gport - The ETH_RIF/LIF interface ID.
 * \param [in] intf_profile - the profile ID of the interface (LIF/VSI profile)
 * \param [in] result_type - the result type of the VSI profile entry
 * \param [in] intf_type - interface type ETH_RIF or LIF.
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   none
 */
shr_error_e dnx_in_lif_vsi_profile_set(
    int unit,
    bcm_port_t gport,
    uint32 intf_profile,
    uint32 result_type,
    in_lif_intf_type_e intf_type);

/**
 * \brief -
 *  Get the default routing enablers vector -
 *  only ETH UC is enabled, all other protocols are disabled. 
 * \param [in] unit - unit number
 * \param [out] routing_enable_vector - the default routing vector
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   none
 */
shr_error_e dnx_enablers_vectors_initial_set(
    int unit,
    uint32 *routing_enable_vector);

/**
 * \brief -
 *  Get in-lif-profile template data according to interface type.
 *
 * \param [in] unit - The unit ID
 * \param [in] in_lif_profile - in_lif_profile id to get template data for
 * \param [in] intf_type - can be one of the following:
 *                         ETH_RIF - interface type is ETH_RIF
 *                         LIF - interface type is LIF
 * \param [out] in_lif_profile_template_data - in-lif-profile template data.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   The template data may be got from DNX_ALGO_ETH_RIF_PROFILE,
 *   DNX_ALGO_ETH_RIF_PROFILE or DNX_ALGO_IN_LIF_PROFILE according
 *   to the in_lif_profile id and intf_type.
 *
 * \see
 *   dnx_in_lif_profile_exchange
 *   dnx_algo_in_lif_profile_template_create
 *   dnx_algo_in_lif_profile_and_eth_rif_shared_template_create
 */
shr_error_e dnx_in_lif_profile_template_data_get(
    int unit,
    int in_lif_profile,
    in_lif_intf_type_e intf_type,
    in_lif_template_data_t * in_lif_profile_template_data);

/**
 * \brief -
 *  returns the in_lif_profile_used_bits_mask
 *
 * \param [in] unit - unit number
 * \param [out] in_lif_profile_used_bits_mask - in_lif_profile_used_bits_mask according to soc properties
 * \param [out] egress_reserved_nof_bits - egress_reserved_nof_bits according to soc properties
 * \param [out] egress_reserved_start_bit - egress_reserved_start_bit according to soc properties
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
shr_error_e dnx_in_lif_profile_used_bits_mask_get(
    int unit,
    int *in_lif_profile_used_bits_mask,
    int *egress_reserved_nof_bits,
    int *egress_reserved_start_bit);
/**
 * \brief -
 *  returns the eth_rif_profile_used_bits_mask
 *
 * \param [in] unit - unit number
 * \param [out] eth_rif_profile_used_bits_mask - eth_rif_profile_used_bits_mask according to soc properties
 * \param [out] egress_reserved_nof_bits - egress_reserved_nof_bits according to soc properties
 * \param [out] eth_rif_profile_egress_reserved_start_bit - eth_rif_profile_egress_reserved_start_bit according to soc properties
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
shr_error_e dnx_eth_rif_profile_used_bits_mask_get(
    int unit,
    int *eth_rif_profile_used_bits_mask,
    int *egress_reserved_nof_bits,
    int *eth_rif_profile_egress_reserved_start_bit);
/*
 * }
 */
#endif /* IN_LIF_PROFILE_H_INCLUDED */
