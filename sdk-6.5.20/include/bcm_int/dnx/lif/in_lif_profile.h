/** \file in_lif_profile.h
 *
 * in_lif profile management for DNX.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IN_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define IN_LIF_PROFILE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <include/bcm_int/dnx/rx/rx.h>
#include <bcm/l3.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/aacl/aacl_in_intf.h>

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
#define ETH_RIF_PROFILE_PMF_RESERVED_START_BIT NOF_ETH_RIF_PROFILE_BITS - ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS
#define IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS dnx_data_field.profile_bits.nof_ing_in_lif_get(unit)
#define IN_LIF_PROFILE_PMF_RESERVED_START_BIT NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS

/*
 * define number of bits for egress in_lif_profile bits according to soc properties
 */
#define IN_LIF_ORIENTATION_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get(unit)
#define IN_LIF_ORIENTATION_NOF_BITS utilex_log2_round_up(IN_LIF_ORIENTATION_NOF_VALUES)
#define IVE_INNER_QOS_DP_PROFILE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_policer_inner_dp_get(unit)
#define IVE_INNER_QOS_DP_PROFILE_NOF_BITS utilex_log2_round_up(IVE_INNER_QOS_DP_PROFILE_NOF_VALUES)
#define IVE_OUTER_QOS_DP_PROFILE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_policer_outer_dp_get(unit)
#define IVE_OUTER_QOS_DP_PROFILE_NOF_BITS utilex_log2_round_up(IVE_OUTER_QOS_DP_PROFILE_NOF_VALUES)
#define SAME_INTERFACE_MODE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit)
#define SAME_INTERFACE_MODE_NOF_BITS utilex_log2_round_up(SAME_INTERFACE_MODE_NOF_VALUES)

/*
 * define number of bits for native indexed mode in_lif_profile bits according to soc properties
 */
#define NATIVE_INDEXED_MODE_NOF_VALUES dnx_data_lif.in_lif.in_lif_profile_allocate_indexed_mode_get(unit)
#define NATIVE_INDEXED_MODE_NOF_BITS utilex_log2_round_up(NATIVE_INDEXED_MODE_NOF_VALUES)
/*
 * defines the number of values, that should be allocated to support PWE indexed mode indication
 */
#define IN_LIF_PROFILE_PWE_INDEXED_MODE_NOF_VALUES 2
/*
 * define number of bits and start bits for EGRESS reserved bits in in_lif_profile and eth_rif_profile
 * meanwhile we support only IN_LIF_PROFILE egress reserved bits
 */
#define ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS 0
#define ETH_RIF_PROFILE_EGRESS_RESERVED_START_BIT (ETH_RIF_PROFILE_PMF_RESERVED_START_BIT - ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS)
#define IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS (IN_LIF_ORIENTATION_NOF_BITS + IVE_INNER_QOS_DP_PROFILE_NOF_BITS + IVE_OUTER_QOS_DP_PROFILE_NOF_BITS + SAME_INTERFACE_MODE_NOF_BITS)
#define IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT (IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS)
#define IN_LIF_PROFILE_NATIVE_INDEXED_MODE_START_BIT (IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS)
/*
 * defines the max number of supported bits for in_lif_profile in JR sys header mode
 */
#define IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE 2
#define IN_LIF_PROFILE_EGRESS_NOF_BITS_DSCP_MODE 1
#define JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES 1
#define JR_COMP_MODE_ONLY_DEV_SCOPE_ENABLED_BY_SOC_PROPERTIES 2
#define JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES 3
/*
 * number of supported in_lif_profiles after reserved bits
 */
#define NOF_IN_LIF_PROFILES (1 << (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS))
#define NOF_IN_LIF_PROFILE_INGRESS_BITS (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS)
/*
 * number of supported eth_rif_profiles after reserved bits
 */
#define NOF_ETH_RIF_PROFILES (1 << (NOF_ETH_RIF_PROFILE_BITS - ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS - ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS))
#define NOF_ETH_RIF_PROFILE_INGRESS_BITS (NOF_ETH_RIF_PROFILE_BITS - ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS - ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS - NATIVE_INDEXED_MODE_NOF_BITS)
/*
 * Routing Enablers Defines
 */
/**
 * initial enablers vector before calling bcm_l3_intf_create
 */
#define INITIAL_ENABLERS_VECTOR_MSB (0x0)
#define INITIAL_ENABLERS_VECTOR_LSB (0x00000004)
/**
 * Specify all routing enablers are disabled
 */
#define DEFAULT_ROUTING_ENABLERS_PROFILE (0x0)

/**
 * number of enablers vector profiles
 */
#define NOF_ENTRIES_ENABLERS_VECTOR        (32)
/**
 * number of routing enablers profile bits
 */
#define NOF_ENABLERS_PROFILE_BITS          (5)
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
/***************************************************************************************/
/**
 * Specify default LIF LB Profile vector - enabled for all layer types
 */
#define INITIAL_IN_LIF_LB_ENABLERS_VECTOR (0xFFFFFFFF)
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
    TWO_TAGS
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
     * RPF mode
     * /see dbal_enum_value_field_rpf_mode_e in dbal_defines_fields.h
     */
    dbal_enum_value_field_rpf_mode_e rpf_mode;
    /**
     * CS in_lif profile
     */
    uint8 cs_in_lif_profile;
    /**
     * CS in_lif FWD profile
     */
    uint8 cs_in_lif_fwd_profile;
    /**
     * Protocol trap profiles structure
     */
    bcm_rx_trap_protocol_profiles_t protocol_trap_profiles;
    /*
     * LIF type
     */
    in_lif_intf_type_e lif_type;
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
 */
    uint32 lb_per_lif_flags;
    /**
     * LB inlif profile
     */
    uint8 in_lif_lb_profile;
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
    in_lif_profile_info_ingress_t ingress_fields;
    in_lif_profile_info_egress_t egress_fields;
    uint8 native_indexed_mode;
} in_lif_profile_info_t;

static const layer_types_enablers_t layer_types_enablers[] = {
    {DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ARP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_FCOE, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_TCP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_UDP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_L2TP, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PTP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ICMP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_RCH, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IGMP, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPT, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_GTP, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PPP, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPVX, 1, 1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN, 0, 0},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL, 0, 0}
};

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
    in_lif_intf_type_e intf_type);

/**
 * \brief
 *   updates template manager for L2/L3 profiles, and exchanges the old in_lif_profile with
 *   new in_lif_profile according to LIF properties which are stored in in_lif_profile_info.
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
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   We don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the InLIF
 *   to the new place traffic will get some "undefine" state.
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
    dbal_tables_e inlif_dbal_table_id);

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
 * \param [in] intf_id - The ETH_RIF/LIF interface ID.
 * \param [out] intf_profile - the retrieved profile ID of the interface (LIF/VSI profile)
 * \param [out] result_type - the result type of the VSI profile entry
 * \param [out] intf_type - interface type ETH_RIF or LIF.
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   none
 */
shr_error_e dnx_in_lif_vsi_profile_get(
    int unit,
    uint32 intf_id,
    uint32 *intf_profile,
    uint32 *result_type,
    in_lif_intf_type_e * intf_type);

/**
 * \brief -
 *  Sets the profile data for the VSI/LIF profile
 * \param [in] unit - unit number
 * \param [in] intf_id - The ETH_RIF/LIF interface ID.
 * \param [in] intf_profile - the profile ID of the interface (LIF/VSI profile)
 * \param [in] result_type - the result type of the VSI profile entry
 * \param [out] intf_type - interface type ETH_RIF or LIF.
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   none
 */
shr_error_e dnx_in_lif_vsi_profile_set(
    int unit,
    uint32 intf_id,
    uint32 intf_profile,
    uint32 result_type,
    in_lif_intf_type_e * intf_type);

/*
 * }
 */
#endif /* IN_LIF_PROFILE_H_INCLUDED */
