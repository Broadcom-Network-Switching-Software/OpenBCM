/** \file rx_trap_map.c
 * 
 *
 * RX procedures for DNX.
 *
 * Here add DESCRIPTION.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>

/*
 * }
 */

/*
 * Macros and defines for rx trap
 * {
 */

/*
 * }
 */

/*
 * Globals
 * {
 */

/* *INDENT-OFF* */

const dnx_rx_trap_map_type_t dnx_rx_trap_type_map[bcmRxTrapCount] = {
    [bcmRxTrapLinkLayerHeaderSizeErr]                   = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_HEADER_SIZE_ERR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "LinkLayerHeaderSizeErr"},
    [bcmRxTrapLinkLayerSaMulticast]                     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_SA_MULTICAST,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "LinkLayerSaMulticast"},
    [bcmRxTrapLinkLayerSaEqualsDa]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_SA_EQUALS_DA,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "LinkLayerSaEqualsDa"},
    [bcmRxTrapLinkLayerVlanTagDiscard]                  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_ACCEPTABLE_FRAME_TYPE1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "LinkLayerVlanTagDiscard"},
    [bcmRxTrapLinkLayerVlanTagAccept]                   = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_ACCEPTABLE_FRAME_TYPE0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "LinkLayerVlanTagAccept"},
    [bcmRxTrapDefault]                                  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "Default"},
    [bcmRxTrapUnknownDest]                              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_UNKNOWN_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "UnknownDest",0,TRUE},
    [bcmRxTrapPortNotVlanMember]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_PORT_NOT_VLAN_MEMBER,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "PortNotVlanMember"},
    [bcmRxTrapMyBmacUnknownISid]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MY_B_DA_UNKNOWN_I_SID,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MyBmacUnknownISid"},
    [bcmRxTrapMyMacAndIpDisabled]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MY_MAC_AND_IP_DISABLE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MyMacAndIpDisabled"},
    [bcmRxTrapMyMacAndMplsDisable]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MY_MAC_AND_MPLS_DISABLE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MyMacAndMplsDisable"},
    [bcmRxTrapArpReply]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MY_MAC_AND_ARP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ArpReply"},
    [bcmRxTrapMyMacAndUnknownL3]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MY_MAC_AND_UNKNOWN_L3,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MyMAcAndUnknownL3"},
    [bcmRxTrapIpCompMcInvalidIp]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IP_COMP_MC_INVALID_IP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "IpCompMcInvalidIp"},
    [bcmRxTrapMplsPreprocessingBosOrTtl]                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_PREPROCESSING_BOS_OR_TTL_TRAP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsPreprocessingBosOrTtl"},
    [bcmRxTrapMplsUnexpectedBos]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_UNEXPECTED_BOS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsUnexpectedBos"},
    [bcmRxTrapMplsUnexpectedNoBos]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_UNEXPECTED_NO_BOS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsUnexpectedNoBos"},
    [bcmRxTrapTerminatedMplsControlWordTrap]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_CONTROL_WORD_TRAP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedMplsControlWordTrap"},
    [bcmRxTrapTerminatedMplsControlWordDrop]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_CONTROL_WORD_DROP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedMplsControlWordDrop"},
    [bcmRxTrapStpStateBlock]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_STP_STATE_BLOCK,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "StpStateBlock"},
    [bcmRxTrapStpStateLearn]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_STP_STATE_LEARN,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "StpStateLearn"},
    [bcmRxTrapFailover1Plus1Fail]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_PROTECTION_PATH_INVALID,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "Failover1Plus1Fail"},
    [bcmRxTrapMplsTerminationFail]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_TERMINATION_ERR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsTerminationFail"},
    [bcmRxTrapTerminatedVlanTagDiscard]                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_ACCEPTABLE_FRAME_TYPE1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedVlanTagDiscard"},
    [bcmRxTrapTerminatedVlanTagAccept]                  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_ACCEPTABLE_FRAME_TYPE0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedVlanTagAccept"},
    [bcmRxTrapTerminatedGenericCoeFlowControl]          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_GENERIC_FC,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedGenericCoeFlowControl"},
    [bcmRxTrapTerminatedCoeFlowControl]                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_COE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedCoeFlowControl"},
    [bcmRxTrapTerminatedHeaderSizeErr]                  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_HEADER_SIZE_ERR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedHeaderSizeErr"},
    [bcmRxTrapTerminatedSaMulticast]                    = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_SA_MULTICAST,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedSaMulticast"},
    [bcmRxTrapTerminatedSaEqualsDa]                     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_SA_EQUALS_DA,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedSaEqualsDa"},
    [bcmRxTrapTerminatedSaEqualsZero]                   = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_SA_EQUALS_ZERO,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedSaEqualsZero"},
    [bcmRxTrapTerminatedVridMyMac]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_VRID_MY_MAC,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedVridMyMac"},
    [bcmRxTrapTerminatedIpv4VersionError]               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_VERSION_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4VersionError"},
    [bcmRxTrapTerminatedIpv4ChecksumError]              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_CHECKSUM_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4ChecksumError"},
    [bcmRxTrapTerminatedIpv4HeaderLengthError]          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_HEADER_LENGTH_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4HeaderLengthError"},
    [bcmRxTrapTerminatedIpv4TotalLengthError]           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_TOTAL_LENGTH_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4TotalLengthError"},
    [bcmRxTrapTerminatedIpv4Ttl0]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_TTL0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4Ttl0"},
    [bcmRxTrapTerminatedIpv4HasOptions]                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_HAS_OPTIONS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4HasOptions"},
    [bcmRxTrapTerminatedIpv4Ttl1]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_TTL1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4Ttl1"},
    [bcmRxTrapTerminatedIpv4SipEqualDip]                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_SIP_EQUAL_DIP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4SipEqualDip"},
    [bcmRxTrapTerminatedIpv4DipZero]                    = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_DIP_ZERO,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4DipZero"},
    [bcmRxTrapTerminatedIpv4SipIsMc]                    = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_SIP_IS_MC,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4SipIsMc"},
    [bcmRxTrapTerminatedIpv6VersionError]               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_VERSION_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6VersionError"},
    [bcmRxTrapIpv6HopCount0]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_HOP_COUNT0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "Ipv6HopCount0"},
    [bcmRxTrapIpv6HopCount1]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_HOP_COUNT1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "Ipv6HopCount1"},
    [bcmRxTrapTerminatedIpv6UnspecifiedDestination]     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_UNSPECIFIED_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6UnspecifiedDestination"},
    [bcmRxTrapTerminatedIpv6LoopbackAddress]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_LOOPBACK_ADDRESS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6LoopbackAddress"},
    [bcmRxTrapTerminatedIpv6MulticastSource]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_MULTICAST_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6MulticastSource"},
    [bcmRxTrapTerminatedIpv6NextHeaderNull]             = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_NEXT_HEADER_NULL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6NextHeaderNull"},
    [bcmRxTrapTerminatedIpv6UnspecifiedSource]          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_UNSPECIFIED_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6UnspecifiedSource"},
    [bcmRxTrapTerminatedIpv6LocalLinkDestination]       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_LOCAL_LINK_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6LocalLinkDestination"},
    [bcmRxTrapTerminatedIpv6LocalSiteDestination]       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_LOCAL_SITE_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6LocalSiteDestination"},
    [bcmRxTrapTerminatedIpv6LocalLinkSource]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_LOCAL_LINK_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6LocalLinkSource"},
    [bcmRxTrapTerminatedIpv6LocalSiteSource]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_LOCAL_SITE_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6LocalSiteSource"},
    [bcmRxTrapTerminatedIpv6Ipv4CompatibleDestination]  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_IPV4_COMPATIBLE_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6Ipv4CompatibleDestination"},
    [bcmRxTrapTerminatedIpv6Ipv4MappedDestination]      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_IPV4_MAPPED_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6Ipv4MappedDestination"},
    [bcmRxTrapTerminatedIpv6MulticastDestination]       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV6_MULTICAST_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv6MulticastDestination"},
    [bcmRxTrapTerminatedIpv4Fragmented]                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_IPV4_FRAGMENTED,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TerminatedIpv4Fragmented"},
    [bcmRxTrapForwardingSaEqualsZero]                   = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_EQUALS_ZERO,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingSaEqualsZero"},
    [bcmRxTrapFwdDomainAssignmentModeTrap]              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_FWD_DOMAIN_ASSIGN_MODE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "FwdDomainAssignmentModeTrap"},
    [bcmRxTrapForwardingIpv4VersionError]               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_VERSION_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4VersionError"},
    [bcmRxTrapForwardingIpv4ChecksumError]              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_CHECKSUM_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4ChecksumError"},
    [bcmRxTrapForwardingIpv4HeaderLengthError]          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_HEADER_LENGTH_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4HeaderLengthError"},
    [bcmRxTrapForwardingIpv4TotalLengthError]           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_TOTAL_LENGTH_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4TotalLengthError"},
    [bcmRxTrapForwardingIpv4Ttl0]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_TTL0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4Ttl0"},
    [bcmRxTrapForwardingIpv4HasOptions]                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_HAS_OPTIONS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4HasOptions"},
    [bcmRxTrapForwardingIpv4Ttl1]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_TTL1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4Ttl1"},
    [bcmRxTrapMplsForwardingTtl0]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_MPLS_TTL0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsForwardingTtl0"},
    [bcmRxTrapMplsForwardingTtl1]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_MPLS_TTL1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsForwardingTtl1"},
    [bcmRxTrapForwardingIpv6Ttl0]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_HOP_COUNT0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6Ttl0"},
    [bcmRxTrapForwardingIpv6Ttl1]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_HOP_COUNT1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6Ttl1"},
    [bcmRxTrapForwardingIpv4SipEqualDip]                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_SIP_EQUAL_DIP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4SipEqualDip"},
    [bcmRxTrapForwardingIpv4DipZero]                    = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_DIP_ZERO,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4DipZero"},
    [bcmRxTrapForwardingIpv4SipIsMc]                    = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV4_SIP_IS_MC,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv4SipIsMc"},
    [bcmRxTrapForwardingIpv6VersionError]               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_VERSION_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6VersionError"},
    [bcmRxTrapForwardingIpv6UnspecifiedDestination]     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_UNSPECIFIED_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6UnspecifiedDestination"},
    [bcmRxTrapForwardingIpv6LoopbackAddress]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_LOOPBACK_ADDRESS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6LoopbackAddress"},
    [bcmRxTrapForwardingIpv6MulticastSource]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_MULTICAST_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6MulticastSource"},
    [bcmRxTrapForwardingIpv6NextHeaderNull]             = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_NEXT_HEADER_NULL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6NextHeaderNull"},
    [bcmRxTrapForwardingIpv6UnspecifiedSource]          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_UNSPECIFIED_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6UnspecifiedSource"},
    [bcmRxTrapForwardingIpv6LocalLinkDestination]       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_LOCAL_LINK_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6LocalLinkDestination"},
    [bcmRxTrapForwardingIpv6LocalSiteDestination]       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_LOCAL_SITE_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6LocalSiteDestination"},
    [bcmRxTrapForwardingIpv6LocalLinkSource]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_LOCAL_LINK_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6LocalLinkSource"},
    [bcmRxTrapForwardingIpv6LocalSiteSource]            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_LOCAL_SITE_SOURCE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6LocalSiteSource"},
    [bcmRxTrapForwardingIpv6Ipv4CompatibleDestination]  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6Ipv4CompatibleDestination"},
    [bcmRxTrapForwardingIpv6Ipv4MappedDestination]      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_IPV4_MAPPED_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6Ipv4MappedDestination"},
    [bcmRxTrapForwardingIpv6MulticastDestination]       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IPV6_MULTICAST_DESTINATION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ForwardingIpv6MulticastDestination"},
    [bcmRxTrapMplsTunnelTerminationTtl0]                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_TTL_0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsTunnelTerminationTtl0"},
    [bcmRxTrapMplsTunnelTerminationTtl1]                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_VTT_MPLS_TTL_1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "MplsTunnelTerminationTtl1"},
    [bcmRxTrapTcpSnFlagsZero]                           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_TCP_SN_FLAGS_ZERO,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TcpSnFlagsZero"},
    [bcmRxTrapTcpSnZeroFlagsSet]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_TCP_SN_ZERO_FLAGS_SET,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TcpSnZeroFlagsSet"},
    [bcmRxTrapTcpSynFin]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_TCP_SYN_FIN,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TcpSynFin"},
    [bcmRxTrapTcpEqualPorts]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_TCP_EQUAL_PORTS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TcpEqualPorts"},
    [bcmRxTrapTcpFragmentIncompleteHeader]              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TcpFragmentIncompleteHeader"},
    [bcmRxTrapTcpFragmentOffsetLt8]                     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_TCP_FRAGMENT_OFFSET_LT8,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "TcpFragmentOffsetLt8"},
    [bcmRxTrapUdpEqualPorts]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_UDP_EQUAL_PORTS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "UdpEqualPorts"},
    [bcmRxTrapIcmpDataGt576]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_ICMP_DATA_GT_576,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "IcmpDataGt576"},
    [bcmRxTrapIcmpFragmented]                           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_ICMP_FRAGMENTED,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "IcmpFragmented"},
    [bcmRxTrapL2DiscardMacsaFwd]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_DROP0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DiscardMacsaFwd"},
    [bcmRxTrapL2DiscardMacsaDrop]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_DROP1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DiscardMacsaDrop"},
    [bcmRxTrapL2DiscardMacsaTrap]                       = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_DROP2,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DiscardMacsaTrap"},
    [bcmRxTrapL2DiscardMacsaSnoop]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_DROP3,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DiscardMacsaSnoop"},
    [bcmRxTrapL2Learn0]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2Learn0"},
    [bcmRxTrapL2Learn1]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2Learn1"},
    [bcmRxTrapL2Learn2]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND2,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2Learn2"},
    [bcmRxTrapL2Learn3]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND3,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2Learn3"},
    [bcmRxTrapL2DlfFwd]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DlfFwd",0,TRUE},
    [bcmRxTrapL2DlfDrop]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DlfDrop",0,TRUE},
    [bcmRxTrapL2DlfTrap]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND2,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DlfTrap",0,TRUE},
    [bcmRxTrapL2DlfSnoop]                               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND3,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "L2DlfSnoop", 0,TRUE},
    [bcmRxTrap1588]                                     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588"},
    [bcmRxTrap1588Discard]                              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588Discard"},
    [bcmRxTrap1588Accepted]                             = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_2,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588Accepted"},
    [bcmRxTrap1588User1]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_3,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588User1"},
    [bcmRxTrap1588User2]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_4,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588User2"},
    [bcmRxTrap1588User3]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_5,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588User3"},
    [bcmRxTrap1588User4]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_6,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588User4"},
    [bcmRxTrap1588User5]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_7,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "1588User5"},
    [bcmRxTrapExternalLookupError]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_ELK_ERROR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "ExternalLookupError"},
    [bcmRxTrapSameInterface]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_SAME_INTERFACE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "SameInterface"},
    [bcmRxTrapUcLooseRpfFail]                           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_UC_LOOSE_RPF_FAIL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "UcLooseRpfFail"},
    [bcmRxTrapFcoeSrcIdMismatchSa]                      = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_FABRIC_PROVIDED_SECURITY,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "FcoeSrcIdMismatchSa"},
    [bcmRxTrapFailoverFacilityInvalid]                  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_FACILITY_INVALID,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "FailoverFacilityInvalid"},
    [bcmRxTrapDfltDroppedPacket]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_DEFAULT_DROP_TRAP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "DfltDroppedPacket",0,TRUE},
    [bcmRxTrapDfltRedirectToCpuPacket]                  = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_DEFAULT_REDIRECT_TO_CPU_TRAP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "DfltRedirectToCpuPacket",0,TRUE},
    [bcmRxTrapUcStrictRpfFail]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_UC_STRICT_RPF_FAIL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "UcStrictRpfFail"},
    [bcmRxTrapMcExplicitRpfFail]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_MC_EXPLICIT_RPF_FAIL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "McExplicitRpfFail"},
    [bcmRxTrapMcUseSipRpfFail]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_MC_USE_SIP_RPF_FAIL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "McUseSipRpfFail"},
    [bcmRxTrapMcUseSipMultipath]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_SIP_TRANSPLANT_DETECTION,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "McUseSipMultipath"},
    [bcmRxTrapIcmpRedirect]                             = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_ICMP_REDIRECT,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "IcmpRedirect"},
    [bcmRxTrapEgressTrapped2ndPass]                     = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_EGRESS_TRAPPED_2ND_PASS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgressTrapped2ndPass"},
    [bcmRxTrapEgTxFieldSnoop0]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop0"},
    [bcmRxTrapEgTxFieldSnoop1]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_1,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop1"},
    [bcmRxTrapEgTxFieldSnoop2]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_2,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop2"},
    [bcmRxTrapEgTxFieldSnoop3]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_3,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop3"},
    [bcmRxTrapEgTxFieldSnoop4]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_4,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop4"},
    [bcmRxTrapEgTxFieldSnoop5]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_5,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop5"},
    [bcmRxTrapEgTxFieldSnoop6]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_6,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop6"},
    [bcmRxTrapEgTxFieldSnoop7]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_7,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop7"},
    [bcmRxTrapEgTxFieldSnoop8]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_8,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop8"},
    [bcmRxTrapEgTxFieldSnoop9]                          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_9,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop9"},
    [bcmRxTrapEgTxFieldSnoop10]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_10,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop10"},
    [bcmRxTrapEgTxFieldSnoop11]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_11,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop11"},
    [bcmRxTrapEgTxFieldSnoop12]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_12,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop12"},
    [bcmRxTrapEgTxFieldSnoop13]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_13,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop13"},
    [bcmRxTrapEgTxFieldSnoop14]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_14,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop14"},
    [bcmRxTrapEgTxFieldSnoop15]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_SNOOP_CODE_15,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "EgTxFieldSnoop15"},
    [bcmRxTrapUserDefine]                               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_0,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_USER_DEFINED,
                                                           "UserDefine",0,TRUE},
    [bcmRxTrapOamEthAccelerated]                        = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_ETH_OAM,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamEthAccelerated",0,TRUE},
    [bcmRxTrapOamY1731MplsTp]                           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_MPLSTP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamY1731MplsTp",0,TRUE},
    [bcmRxTrapOamY1731Pwe]                              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_PWE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamY1731Pwe",0,TRUE},
    [bcmRxTrapOamBfdIpv4]                               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamBfdIpv4",0,TRUE},
    [bcmRxTrapEgBfdIpv6InvalidUdpChecksum]              = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6_UDP_CHECKSUM,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamBfdIpv6InvalidUdpChecksum",0,FALSE},
    [bcmRxTrapOamBfdIpv6]                               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamBfdIpv6",0,TRUE},
    [bcmRxTrapOamBfdMpls]                               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_MPLS,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamBfdMpls",0,TRUE},
    [bcmRxTrapOamBfdPwe]                                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_PWE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamBfdPwe",0,TRUE},
    [bcmRxTrapOamReflector]                             = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_OAM_REFLECTOR,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamReflector",0,TRUE},
    [bcmRxTrapBfdOamDownMEP]                            = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NON_ACC_OAM_BFD,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "BfdOamDownMEP",0,TRUE},
    [bcmRxTrapSnoopOamPacket]                           = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_OAM_CPU_SNOOP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "SnoopOamPacket",0,TRUE},
    [bcmRxTrapOamLevel]                                 = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_OAM_LEVEL,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamLevel",0,TRUE},
    [bcmRxTrapOamPassive]                               = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_OAM_PASSIVE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamPassive",0,TRUE},
    [bcmRxTrapMplsUnknownLabel]                         = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_MPLS_UNKNOWN_LABEL,
                                                          DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                          "MplsUnknownLabel",0,TRUE},
    [bcmRxTrapOamPerformanceEthAccelerated]          = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_ETH_OAM,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamPerformanceEthAccelerated",0,TRUE},
    [bcmRxTrapOamPerformanceY1731MplsTp]             = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_MPLSTP,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamPerformanceY1731MplsTp",0,TRUE},
    [bcmRxTrapOamPerformanceY1731Pwe]                = {1,DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_PWE,
                                                           DNX_RX_TRAP_BLOCK_INGRESS,DNX_RX_TRAP_CLASS_PREDEFINED,
                                                           "OamPerformanceY1731Pwe",0,TRUE},
    /**Erpp User Define Trap */
    [bcmRxTrapEgUserDefine]                             = {1,DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES,
                                                           DNX_RX_TRAP_BLOCK_ERPP, DNX_RX_TRAP_CLASS_USER_DEFINED,
                                                           "EgUserDefine",0,TRUE},
    /**ETPP User Define Trap */
    [bcmRxTrapEgTxUserDefine]                           = {1,DBAL_NOF_ENUM_ETPP_TRAP_ID_VALUES,
                                                           DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED,
                                                           "EgTxUserDefine",0,TRUE},
    /**ETPP User Define Trap */
    [bcmRxTrapEgTxVisibility]                           = {1,DBAL_NOF_ENUM_ETPP_TRAP_ID_VALUES,
                                                           DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED,
                                                           "EgTxVisibility",0,TRUE},
    /**ETPP predefined OAM traps */
    [bcmRxTrapEgTxOamUpMEPOamp]   = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_2,DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP),
                                     DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                     "EgTxOamUpMEPOamp"},
    [bcmRxTrapEgTxOamUpMEPDest1]  = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3,DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1),
                                     DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                     "EgTxOamUpMEPDest1"},
    [bcmRxTrapEgTxOamUpMEPDest2]  = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4,DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2),
                                     DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                     "EgTxOamUpMEPDest2"},
    [bcmRxTrapEgTxOamLevel]       = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3,DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR),
                                     DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                     "EgTxOamLevel"},
    [bcmRxTrapEgTxOamPassive]     = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3,DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR),
                                     DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                     "EgTxOamPassive"},
    [bcmRxTrapEgTxOamReflector]   = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1,DNX_RX_TRAP_ETPP_OAM_REFLECTOR),
                                     DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                     "EgTxOamReflector"},
    /**ETPP IFA Recycle Trap */
   [bcmRxTrapEgTxIfaEgressMetadata]  = {1,DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_5,DNX_RX_TRAP_ETPP_IFA2),
           DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
           "EgTxIfaEgressMetadata"},
};

const dnx_rx_trap_map_type_t dnx_rx_app_trap_type_map[bcmRxTrapCount]= {
    [bcmRxTrapEgHairPinFilter]                      = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_SRC_EQUAL_DEST,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgHairPinFilter"},
    [bcmRxTrapEgSplitHorizonFilter]                 = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_SPLIT_HORIZON,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgSplitHorizonFilter"},
    [bcmRxTrapEgUnknownDa]                          = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_UNKNOWN_DA,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgUnknownDa"},
    [bcmRxTrapEgDiscardFrameTypeFilter]             = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_UNACCEPTABLE_FRAME_TYPE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgDiscardFrameTypeFilter"},
    [bcmRxTrapEgIpv4Ttl0]                           = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ZERO,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4Ttl0"},
    [bcmRxTrapEgIpv4Ttl1]                           = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4Ttl1"},
    [bcmRxTrapDssStacking]                          = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_DSS_STACKING,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "DssStacking"},
    [bcmRxTrapLagMulticast]                         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_LAG_MULTICAST,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "LagMulticast"},
    [bcmRxTrapEgIpmcTtlErr]                         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_SCOPE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpmcTtlErr"},
    [bcmRxTrapEgInvalidDestPort]                    = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_INVALID_OTM,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgInvalidDestPort"},
    [bcmRxTrapEgRqpDiscard]                         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_RQP_DISCARD,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgRqpDiscard"},
    [bcmRxTrapEgTdmDiscard]                         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TDM_DISCARD,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTdmDiscard"},
    [bcmRxTrapEgExcludeSrc]                         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_EXCLUDE_SRC,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgExcludeSrc"},
    [bcmRxTrapEgGlemPpTrap]                         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_GLEM_PP_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgGlemPpTrap"},
    [bcmRxTrapEgGlemNonePpTrap]                     = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_GLEM_NON_PP_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgGlemNonePpTrap"},
    [bcmRxTrapEgIpv4Error]                          = {0, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_FILTERS_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4Error"},
    [bcmRxTrapEgIpv6Error]                          = {0, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_FILTERS_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6Error"},
    [bcmRxTrapEgL4Error]                            = {0, DBAL_ENUM_FVAL_ERPP_TRAP_ID_LAYER_4_FILTERS_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgL4Error"},
    [bcmRxTrapEgIpv4VersionError]                   = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_VERSION_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4VersionError"},
    [bcmRxTrapEgIpv4ChecksumError]                  = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_CHECKSUM_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4ChecksumError"},
    [bcmRxTrapEgIpv4HeaderLengthError]              = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_HEADER_LENGTH_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4HeaderLengthError"},
    [bcmRxTrapEgIpv4TotalLengthError]               = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_TOTAL_LENGTH_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4TotalLengthError"},
    [bcmRxTrapEgIpv4HasOptions]                     = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_HAS_OPTIONS,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4HasOptions"},
    [bcmRxTrapEgIpv4SipEqualDip]                    = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_SIP_EQUAL_DIP,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4SipEqualDip"},
    [bcmRxTrapEgIpv4DipZero]                        = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_DIP_ZERO,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4DipZero"},
    [bcmRxTrapEgIpv4SipIsMc]                        = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_SIP_IS_MC,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv4SipIsMc"},
    [bcmRxTrapEgIpv6UnspecifiedDestination]         = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_UNSPECIFIED_DESTINATION,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6UnspecifiedDestination"},
    [bcmRxTrapEgIpv6LoopbackAddress]                = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_LOOPBACK_ADDRESS,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6LoopbackAddress"},
    [bcmRxTrapEgIpv6MulticastSource]                = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_MULTICAST_SOURCE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6MulticastSource"},
    [bcmRxTrapEgIpv6UnspecifiedSource]              = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_UNSPECIFIED_SOURCE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6UnspecifiedSource"},
    [bcmRxTrapEgIpv6LocalLinkDestination]           = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_LOCAL_LINK_DESTINATION,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6LocalLinkDestination"},
    [bcmRxTrapEgIpv6LocalSiteDestination]           = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_LOCAL_SITE_DESTINATION,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6LocalSiteDestination"},
    [bcmRxTrapEgIpv6LocalLinkSource]                = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_LOCAL_LINK_SOURCE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6LocalLinkSource"},
    [bcmRxTrapEgIpv6LocalSiteSource]                = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_LOCAL_SITE_SOURCE,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6LocalSiteSource"},
    [bcmRxTrapEgIpv6Ipv4CompatibleDestination]      = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_IPV4_COMPATIBLE_DESTINATION,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6Ipv4CompatibleDestination"},
    [bcmRxTrapEgIpv6Ipv4MappedDestination]          = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_IPV4_MAPPED_DESTINATION,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6Ipv4MappedDestination"},
    [bcmRxTrapEgIpv6MulticastDestination]           = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_MULTICAST_DESTINATION,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6MulticastDestination"},
    [bcmRxTrapEgIpv6NextHeaderNull]                 = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_NEXT_HEADER_NULL,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6NextHeaderNull"},
    [bcmRxTrapEgIpv6VersionError]                   = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_VERSION_ERROR,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgIpv6VersionError"},
    [bcmRxTrapEgTcpSnFlagsZero]                     = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TCP_SN_FLAGS_ZERO,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTcpSnFlagsZero"},
    [bcmRxTrapEgTcpSnZeroFlagsSet]                  = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TCP_SN_ZERO_FLAGS_SET,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTcpSnZeroFlagsSet"},
    [bcmRxTrapEgTcpSynFin]                          = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TCP_SYN_FIN,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTcpSynFin"},
    [bcmRxTrapEgTcpEqualPorts]                      = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TCP_EQUAL_PORTS,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTcpEqualPorts"},
    [bcmRxTrapEgTcpFragmentIncompleteHeader]        = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TCP_FRAGMENT_INCOMPLETE_HEADER,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTcpFragmentIncompleteHeader"},
    [bcmRxTrapEgTcpFragmentOffsetLt8]               = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_TCP_FRAGMENT_OFFSET_LT8,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgTcpFragmentOffsetLt8"},
    [bcmRxTrapEgUdpEqualPorts]                      = {1, DBAL_ENUM_FVAL_ERPP_TRAP_ID_UDP_EQUAL_PORTS,
                                                       DNX_RX_TRAP_BLOCK_ERPP, 0, "EgUdpEqualPorts"},
    [bcmRxTrapEgTxStpStateFail]                     = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_STP_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxStpStateFail"},
    [bcmRxTrapEgTxProtectionPathUnexpected]         = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_PROTECTION_DB_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxProtectionPathUnexpected"},
    [bcmRxTrapEgTxSplitHorizonFilter]               = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_SPLIT_HORIZON,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxSplitHorizonFilter"},
    [bcmRxTrapEgTxDiscardFrameTypeFilter]           = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_ACCEPTABLE_FRAME,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxDiscardFrameTypeFilter"},
    [bcmRxTrapEgTxPortNotVlanMember]                = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_VLAN_MEMBERSHIP,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxPortNotVlanMember"},
    [bcmRxTrapEgTxLatency]                          = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_LATENCY_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxLatency"},
    [bcmRxTrapEgTxMetering]                         = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_METERING_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxMetering"},
    [bcmRxTrapEgTxGlem]                             = {1, DBAL_ENUM_FVAL_ETPP_TRAP_ID_GLEM_TRAP,
                                                       DNX_RX_TRAP_BLOCK_ETPP, 0, "EgTxGlem"},
};

const dnx_rx_trap_map_ud_t dnx_rx_ud_trap_map[DNX_RX_TRAP_BLOCK_NUM_OF]= {
    [DNX_RX_TRAP_BLOCK_INGRESS]         = {DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_0,
                                           DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_223,
                                           "Ingress",
                                           {-1}},
    [DNX_RX_TRAP_BLOCK_ERPP]            = {DNX_RX_TRAP_ERPP_FWD_ACT_PROFILE_1ST_USER_TRAP,
                                           DNX_RX_TRAP_ERPP_FWD_ACT_PROFILE_LAST_USER_TRAP,
                                           "ERPP",
                                           {BCM_RX_TRAP_EG_TRAP_ID_DEFAULT, -1}},
    [DNX_RX_TRAP_BLOCK_ETPP]            = {DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1,
                                           DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4,
                                           "ETPP",
                                           {BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, -1}},
};

const dnx_rx_trap_map_ranges_t dnx_rx_range_trap_map[DNX_RX_TRAP_BLOCK_NUM_OF]= {
    [DNX_RX_TRAP_BLOCK_INGRESS]         = {DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_HEADER_SIZE_ERR,
                                           DBAL_ENUM_FVAL_INGRESS_TRAP_ID_SNIF_CODE_31},

    [DNX_RX_TRAP_BLOCK_OAMP]            = {DNX_RX_OAMP_TRAPS_LOWER_RANGE,
                                           (DNX_RX_OAMP_TRAPS_LOWER_RANGE | 0xFF)},

    [DNX_RX_TRAP_BLOCK_ERPP]            = {BCM_RX_TRAP_EG_TRAP_ID_DEFAULT,
                                           (BCM_RX_TRAP_EG_TRAP_ID_DEFAULT + DNX_RX_TRAP_ERPP_FWD_ACT_PROFILE_LAST_USER_TRAP)},

    [DNX_RX_TRAP_BLOCK_ETPP]            = {BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT,
                                           (BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT + DNX_RX_TRAP_ETPP_FWD_ACT_NOF_PROFILES)}
};

const dnx_rx_trap_map_etpp_profile_dbal_fields_t dnx_rx_trap_map_etpp_ud_profile[DNX_RX_TRAP_ETPP_FWD_ACT_NOF_PROFILES]= {
    [DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1]      = {DBAL_FIELD_CFG_RCY_PRIO_GT1,
                                                           DBAL_FIELD_CFG_RCY_CROP_GT1,
                                                           DBAL_FIELD_CFG_RCY_APPEND_ORIG_GT1,
                                                           DBAL_FIELD_CFG_RCY_CMD_GT1,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_PP_DROP_REASON_GT1,
                                                           DBAL_FIELD_CFG_DROP_FWD_COPY_GT1,
                                                           DBAL_FIELD_CFG_GEN_RCY_COPY_GT1},

    [DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_2]      = {DBAL_FIELD_CFG_RCY_PRIO_GT2,
                                                           DBAL_FIELD_CFG_RCY_CROP_GT2,
                                                           DBAL_FIELD_CFG_RCY_APPEND_ORIG_GT2,
                                                           DBAL_FIELD_CFG_RCY_CMD_GT2,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_PP_DROP_REASON_GT2,
                                                           DBAL_FIELD_CFG_DROP_FWD_COPY_GT2,
                                                           DBAL_FIELD_CFG_GEN_RCY_COPY_GT2},

    [DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3]      = {DBAL_FIELD_CFG_RCY_PRIO_GT3,
                                                           DBAL_FIELD_CFG_RCY_CROP_GT3,
                                                           DBAL_FIELD_CFG_RCY_APPEND_ORIG_GT3,
                                                           DBAL_FIELD_CFG_RCY_CMD_GT3,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_PP_DROP_REASON_GT3,
                                                           DBAL_FIELD_CFG_DROP_FWD_COPY_GT3,
                                                           DBAL_FIELD_CFG_GEN_RCY_COPY_GT3},

    [DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4]      = {DBAL_FIELD_CFG_RCY_PRIO_GT4,
                                                           DBAL_FIELD_CFG_RCY_CROP_GT4,
                                                           DBAL_FIELD_CFG_RCY_APPEND_ORIG_GT4,
                                                           DBAL_FIELD_CFG_RCY_CMD_GT4,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_PP_DROP_REASON_GT4,
                                                           DBAL_FIELD_CFG_DROP_FWD_COPY_GT4,
                                                           DBAL_FIELD_CFG_GEN_RCY_COPY_GT4},

    [DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_VISIBILITY]       = {DBAL_FIELD_CFG_RCY_PRIO_VISIBILITY,
                                                           DBAL_FIELD_CFG_RCY_CROP_VISIBILITY,
                                                           DBAL_FIELD_CFG_RCY_APPEND_ORIG_VISIBILITY,
                                                           DBAL_FIELD_CFG_RCY_CMD_VISIBILITY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_PP_DROP_REASON_VISIBILITY,
                                                           DBAL_FIELD_CFG_DROP_FWD_COPY_VISIBILITY,
                                                           DBAL_FIELD_CFG_GEN_RECYCLE_COPY_VISIBILITY},

    [DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_KEEP_FABRIC_PACKET] = {DBAL_FIELD_CFG_RCY_PRIO_KEEP_ORIGINAL,
                                                               DBAL_FIELD_CFG_RCY_CROP_KEEP_ORIGINAL,
                                                               DBAL_FIELD_CFG_RCY_APPEND_ORIG_KEEP_ORIGINAL,
                                                               DBAL_FIELD_CFG_RCY_CMD_KEEP_ORIGINAL,
                                                               DBAL_FIELD_EMPTY,
                                                               DBAL_FIELD_CFG_PP_DROP_REASON_KEEP_ORIGINAL,
                                                               DBAL_FIELD_CFG_DROP_FWD_COPY_KEEP_ORIGINAL,
                                                               DBAL_FIELD_CFG_GEN_RCY_COPY_KEEP_ORIGINAL},


};

const dnx_rx_trap_map_etpp_profile_dbal_fields_t dnx_rx_trap_map_etpp_oam_profile[DNX_RX_TRAP_ETPP_OAM_NOF_TRAPS]= {
    [DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP]                  = {DBAL_FIELD_CFG_RCY_PRIO_OAM_OAMP,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_OAM_OAMP,
                                                           DBAL_FIELD_CFG_CPU_TRAP_CODE_LM},

    [DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1]                 = {DBAL_FIELD_CFG_RCY_PRIO_OAM_CPU,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_OAM_CPU,
                                                           DBAL_FIELD_CFG_CPU_TRAP_CODE_LM},

    [DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2]                 = {DBAL_FIELD_CFG_RCY_PRIO_OAM_FPGA,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_OAM_FPGA,
                                                           DBAL_FIELD_CFG_CPU_TRAP_CODE_LM},

    [DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR]                    = {DBAL_FIELD_CFG_RCY_PRIO_OAM_LVLERR,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_OAM_LVLERR,
                                                           DBAL_FIELD_CFG_CPU_TRAP_CODE_OAM_LVLERR},

    [DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR]                  = {DBAL_FIELD_CFG_RCY_PRIO_OAM_PSVERR,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_OAM_PSVERR,
                                                           DBAL_FIELD_CFG_CPU_TRAP_CODE_OAM_PSVERR},

    [DNX_RX_TRAP_ETPP_OAM_REFLECTOR]                    = {DBAL_FIELD_CFG_RCY_PRIO_OAM_LOOPBACK,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_OAM_LOOPBACK,
                                                           DBAL_FIELD_EMPTY},

    [DNX_RX_TRAP_ETPP_IFA2]                             = {DBAL_FIELD_CFG_RCY_PRIO_OAM_CPU,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_EMPTY,
                                                           DBAL_FIELD_CFG_RCY_CMD_IFA,
                                                           DBAL_FIELD_CFG_CPU_TRAP_CODE_LM},
};

const dnx_rx_trap_map_stat_obj_dbal_fields_t dnx_rx_trap_map_stat_obj[DNX_RX_TRAP_NOF_STAT_OBJS_TO_OW]= {
    [DNX_RX_TRAP_STAT_OBJ_TO_OW_0]                      = {DBAL_FIELD_STAT_OBJ_OVERWRITE_0,
                                                           DBAL_FIELD_STAT_OBJ_INDEX_TO_OVERWRITE_0,
                                                           DBAL_FIELD_STAT_OBJ_VALUE_0,
                                                           DBAL_FIELD_STAT_OBJ_OFFSET_BY_QUALIFIER_0,
                                                           DBAL_FIELD_STAT_OBJ_TYPE_0,
                                                           DBAL_FIELD_STAT_OBJ_IS_METER_0},

    [DNX_RX_TRAP_STAT_OBJ_TO_OW_1]                      = {DBAL_FIELD_STAT_OBJ_OVERWRITE_1,
                                                           DBAL_FIELD_STAT_OBJ_INDEX_TO_OVERWRITE_1,
                                                           DBAL_FIELD_STAT_OBJ_VALUE_1,
                                                           DBAL_FIELD_STAT_OBJ_OFFSET_BY_QUALIFIER_1,
                                                           DBAL_FIELD_STAT_OBJ_TYPE_1,
                                                           DBAL_FIELD_STAT_OBJ_IS_METER_1},
};

const dnx_rx_trap_map_ingress_ud_t dnx_rx_trap_map_ingress_ud[DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES] = {
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_0]     =     {1,0},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_1]     =     {1,1},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_2]     =     {1,2},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_3]     =     {1,3},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_4]     =     {1,4},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_5]     =     {1,5},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_6]     =     {1,6},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_7]     =     {1,7},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_8]     =     {1,8},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_9]     =     {1,9},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_10]    =     {1,10},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_11]    =     {1,11},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_12]    =     {1,12},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_13]    =     {1,13},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_14]    =     {1,14},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_15]    =     {1,15},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_16]    =     {1,16},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_17]    =     {1,17},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_18]    =     {1,18},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_19]    =     {1,19},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_20]    =     {1,20},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_21]    =     {1,21},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_22]    =     {1,22},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_23]    =     {1,23},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_24]    =     {1,24},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_25]    =     {1,25},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_26]    =     {1,26},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_27]    =     {1,27},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_28]    =     {1,28},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_29]    =     {1,29},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_30]    =     {1,30},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_31]    =     {1,31},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_32]    =     {1,32},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_33]    =     {1,33},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_34]    =     {1,34},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_35]    =     {1,35},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_36]    =     {1,36},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_37]    =     {1,37},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_38]    =     {1,38},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_39]    =     {1,39},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_40]    =     {1,40},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_41]    =     {1,41},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_42]    =     {1,42},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_43]    =     {1,43},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_44]    =     {1,44},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_45]    =     {1,45},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_46]    =     {1,46},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_47]    =     {1,47},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_48]    =     {1,48},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_49]    =     {1,49},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_50]    =     {1,50},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_51]    =     {1,51},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_52]    =     {1,52},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_53]    =     {1,53},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_54]    =     {1,54},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_55]    =     {1,55},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_56]    =     {1,56},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_57]    =     {1,57},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_58]    =     {1,58},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_59]    =     {1,59},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_60]    =     {1,60},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_61]    =     {1,61},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_62]    =     {1,62},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_63]    =     {1,63},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_64]    =     {1,64},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_65]    =     {1,65},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_66]    =     {1,66},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_67]    =     {1,67},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_68]    =     {1,68},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_69]    =     {1,69},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_70]    =     {1,70},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_71]    =     {1,71},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_72]    =     {1,72},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_73]    =     {1,73},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_74]    =     {1,74},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_75]    =     {1,75},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_76]    =     {1,76},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_77]    =     {1,77},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_78]    =     {1,78},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_79]    =     {1,79},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_80]    =     {1,80},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_81]    =     {1,81},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_82]    =     {1,82},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_83]    =     {1,83},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_84]    =     {1,84},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_85]    =     {1,85},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_86]    =     {1,86},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_87]    =     {1,87},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_88]    =     {1,88},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_89]    =     {1,89},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_90]    =     {1,90},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_91]    =     {1,91},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_92]    =     {1,92},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_93]    =     {1,93},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_94]    =     {1,94},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_95]    =     {1,95},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_96]    =     {1,96},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_97]    =     {1,97},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_98]    =     {1,98},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_99]    =     {1,99},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_100]   =     {1,100},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_101]   =     {1,101},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_102]   =     {1,102},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_103]   =     {1,103},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_104]   =     {1,104},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_105]   =     {1,105},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_106]   =     {1,106},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_107]   =     {1,107},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_108]   =     {1,108},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_109]   =     {1,109},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_110]   =     {1,110},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_111]   =     {1,111},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_112]   =     {1,112},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_113]   =     {1,113},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_114]   =     {1,114},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_115]   =     {1,115},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_116]   =     {1,116},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_117]   =     {1,117},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_118]   =     {1,118},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_119]   =     {1,119},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_120]   =     {1,120},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_121]   =     {1,121},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_122]   =     {1,122},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_123]   =     {1,123},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_124]   =     {1,124},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_125]   =     {1,125},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_126]   =     {1,126},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_127]   =     {1,127},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_128]   =     {1,128},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_129]   =     {1,129},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_130]   =     {1,130},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_131]   =     {1,131},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_132]   =     {1,132},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_133]   =     {1,133},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_134]   =     {1,134},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_135]   =     {1,135},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_136]   =     {1,136},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_137]   =     {1,137},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_138]   =     {1,138},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_139]   =     {1,139},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_140]   =     {1,140},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_141]   =     {1,141},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_142]   =     {1,142},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_143]   =     {1,143},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_144]   =     {1,144},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_145]   =     {1,145},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_146]   =     {1,146},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_147]   =     {1,147},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_148]   =     {1,148},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_149]   =     {1,149},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_150]   =     {1,150},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_151]   =     {1,151},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_152]   =     {1,152},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_153]   =     {1,153},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_154]   =     {1,154},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_155]   =     {1,155},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_156]   =     {1,156},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_157]   =     {1,157},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_158]   =     {1,158},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_159]   =     {1,159},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_160]   =     {1,160},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_161]   =     {1,161},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_162]   =     {1,162},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_163]   =     {1,163},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_164]   =     {1,164},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_165]   =     {1,165},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_166]   =     {1,166},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_167]   =     {1,167},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_168]   =     {1,168},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_169]   =     {1,169},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_170]   =     {1,170},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_171]   =     {1,171},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_172]   =     {1,172},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_173]   =     {1,173},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_174]   =     {1,174},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_175]   =     {1,175},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_176]   =     {1,176},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_177]   =     {1,177},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_178]   =     {1,178},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_179]   =     {1,179},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_180]   =     {1,180},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_181]   =     {1,181},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_182]   =     {1,182},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_183]   =     {1,183},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_184]   =     {1,184},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_185]   =     {1,185},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_186]   =     {1,186},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_187]   =     {1,187},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_188]   =     {1,188},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_189]   =     {1,189},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_190]   =     {1,190},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_191]   =     {1,191},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_192]   =     {1,192},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_193]   =     {1,193},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_194]   =     {1,194},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_195]   =     {1,195},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_196]   =     {1,196},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_197]   =     {1,197},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_198]   =     {1,198},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_199]   =     {1,199},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_200]   =     {1,200},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_201]   =     {1,201},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_202]   =     {1,202},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_203]   =     {1,203},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_204]   =     {1,204},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_205]   =     {1,205},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_206]   =     {1,206},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_207]   =     {1,207},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_208]   =     {1,208},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_209]   =     {1,209},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_210]   =     {1,210},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_211]   =     {1,211},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_212]   =     {1,212},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_213]   =     {1,213},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_214]   =     {1,214},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_215]   =     {1,215},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_216]   =     {1,216},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_217]   =     {1,217},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_218]   =     {1,218},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_219]   =     {1,219},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_220]   =     {1,220},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_221]   =     {1,221},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_222]   =     {1,222},
    [DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_223]   =     {1,223},
};

const dbal_enum_value_field_ingress_trap_id_e dnx_rx_trap_map_ingress_ud_alloc[DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES] = {
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_0,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_1,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_2,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_3,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_4,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_5,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_6,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_7,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_8,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_9,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_10,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_11,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_12,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_13,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_14,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_15,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_16,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_17,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_18,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_19,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_20,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_21,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_22,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_23,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_24,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_25,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_26,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_27,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_28,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_29,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_30,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_31,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_32,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_33,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_34,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_35,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_36,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_37,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_38,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_39,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_40,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_41,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_42,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_43,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_44,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_45,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_46,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_47,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_48,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_49,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_50,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_51,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_52,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_53,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_54,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_55,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_56,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_57,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_58,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_59,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_60,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_61,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_62,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_63,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_64,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_65,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_66,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_67,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_68,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_69,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_70,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_71,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_72,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_73,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_74,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_75,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_76,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_77,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_78,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_79,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_80,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_81,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_82,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_83,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_84,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_85,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_86,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_87,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_88,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_89,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_90,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_91,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_92,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_93,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_94,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_95,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_96,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_97,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_98,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_99,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_100,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_101,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_102,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_103,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_104,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_105,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_106,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_107,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_108,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_109,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_110,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_111,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_112,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_113,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_114,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_115,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_116,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_117,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_118,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_119,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_120,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_121,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_122,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_123,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_124,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_125,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_126,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_127,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_128,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_129,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_130,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_131,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_132,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_133,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_134,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_135,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_136,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_137,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_138,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_139,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_140,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_141,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_142,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_143,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_144,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_145,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_146,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_147,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_148,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_149,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_150,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_151,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_152,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_153,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_154,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_155,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_156,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_157,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_158,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_159,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_160,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_161,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_162,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_163,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_164,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_165,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_166,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_167,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_168,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_169,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_170,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_171,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_172,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_173,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_174,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_175,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_176,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_177,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_178,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_179,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_180,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_181,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_182,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_183,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_184,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_185,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_186,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_187,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_188,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_189,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_190,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_191,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_192,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_193,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_194,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_195,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_196,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_197,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_198,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_199,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_200,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_201,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_202,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_203,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_204,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_205,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_206,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_207,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_208,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_209,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_210,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_211,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_212,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_213,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_214,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_215,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_216,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_217,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_218,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_219,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_220,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_221,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_222,
    DBAL_ENUM_FVAL_INGRESS_TRAP_ID_USER_DEFINED_223,
    DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES,
};

const char * dnx_rx_trap_type_description[bcmRxTrapCount] = {
    [bcmRxTrapLinkLayerHeaderSizeErr]                   = "Parser indicates header size error",
    [bcmRxTrapLinkLayerSaMulticast]                     = "Source address is multicast",
    [bcmRxTrapLinkLayerSaEqualsDa]                      = "Source address equals destination address",
    [bcmRxTrapLinkLayerVlanTagDiscard]                  = "Discarded packets according to their VLAN Tag configuration",
    [bcmRxTrapLinkLayerVlanTagAccept]                   = "Accepted packets according to their VLAN Tag configuration",
    [bcmRxTrapDefault]                                  = "Default trap, no change to packet's forward action",
    [bcmRxTrapUnknownDest]                              = "Unknown destination",
    [bcmRxTrapPortNotVlanMember]                        = "Initial VID membership error",
    [bcmRxTrapMyBmacUnknownISid]                        = "Unknown I-SID",
    [bcmRxTrapMyMacAndIpDisabled]                       = "Terminated My-MAC over IP, but routing is disabled for InRIF",
    [bcmRxTrapMyMacAndMplsDisable]                      = "Terminated My-MAC over MPLS, but MPLS is disabled for InRIF",
    [bcmRxTrapArpReply]                                 = "Terminated My-MAC over ARP",
    [bcmRxTrapMyMacAndUnknownL3]                        = "Terminated My-MAC over Unknown L3",
    [bcmRxTrapIpCompMcInvalidIp]                        = "L2 compatible MC, but IP does not match",
    [bcmRxTrapMplsPreprocessingBosOrTtl]                = "Error of BOS or TTL was idendefined in MPLS preprocessing",
    [bcmRxTrapMplsUnexpectedBos]                        = "Terminated MPLS label indicates that another MPLS header follows, but following label is BOS",
    [bcmRxTrapMplsUnexpectedNoBos]                      = "Terminated MPLS label doesn't indicate there's a following header, but following label is not BOS",
    [bcmRxTrapTerminatedMplsControlWordTrap]            = "Lookup key is found in ELK/LEM, P2P-Service If lookup result is set, lookup result determines that there's a control word, nibble after terminated MPLS stack equals one",
    [bcmRxTrapTerminatedMplsControlWordDrop]            = "Lookup key is found in ELK/LEM, P2P-Service If lookup result is set, lookup result determines that there's a control word, nibble after terminated MPLS stack doesn't equal one or zero",
    [bcmRxTrapStpStateBlock]                            = "Packet ingresses a blocked port",
    [bcmRxTrapStpStateLearn]                            = "Packet ingresses a port in a LEARN state",
    [bcmRxTrapFailover1Plus1Fail]                       = "One plus one protected LIF and failover status is down",
    [bcmRxTrapMplsTerminationFail]                      = "MPLS already terminated twice by label range match, additional labels may not be terminated by Label range match",
    [bcmRxTrapTerminatedVlanTagDiscard]                 = "Discarded terminated packets according to their VLAN Tag configuration",
    [bcmRxTrapTerminatedVlanTagAccept]                  = "Accepted terminated packets according to their VLAN Tag configuration",
    [bcmRxTrapTerminatedGenericCoeFlowControl]          = "Channelized over Ethernet Generic Flow Control terminated Packet",
    [bcmRxTrapTerminatedCoeFlowControl]                 = "Channelized over Ethernet Flow Control terminated Packet",
    [bcmRxTrapTerminatedHeaderSizeErr]                  = "Parser indicates terminated header size error",
    [bcmRxTrapTerminatedSaMulticast]                    = "Source address is multicast on terminated packet",
    [bcmRxTrapTerminatedSaEqualsDa]                     = "Source address equals destination address on terminated packet",
    [bcmRxTrapTerminatedVridMyMac]                      = "VRID-My-Mac is invalid",
    [bcmRxTrapTerminatedIpv4VersionError]               = "Terminated IPv4 header Version is different than 4",
    [bcmRxTrapTerminatedIpv4ChecksumError]              = "Terminated IPv4 Internet Header Length is 5 and the checksum over the first 20 bytes doesn't verify",
    [bcmRxTrapTerminatedIpv4HeaderLengthError]          = "Terminated IPv4 Internet Header Length is less than 5",
    [bcmRxTrapTerminatedIpv4TotalLengthError]           = "Terminated IPv4 header Total length is less than 20",
    [bcmRxTrapTerminatedIpv4Ttl0]                       = "Terminated IPv4 header TTL equals to zero",
    [bcmRxTrapTerminatedIpv4HasOptions]                 = "Terminated IPv4 Internet Header Length is greater than 5",
    [bcmRxTrapTerminatedIpv4Ttl1]                       = "Terminated IPv4 header TTL equals to one",
    [bcmRxTrapTerminatedIpv4SipEqualDip]                = "Terminated IPv4 source IP equals to destination IP",
    [bcmRxTrapTerminatedIpv4DipZero]                    = "Terminated IPv4 destination IP equals to zero",
    [bcmRxTrapTerminatedIpv4SipIsMc]                    = "Terminated IPv4 source ip is multicast",
    [bcmRxTrapTerminatedIpv6VersionError]               = "Terminated IPv6 header Version is different than 6",
    [bcmRxTrapIpv6HopCount0]                            = "Terminated IPv6 header Hop count (TTL) equals to zero",
    [bcmRxTrapIpv6HopCount1]                            = "Terminated IPv6 header Hop count (TTL) equals to one",
    [bcmRxTrapTerminatedIpv6UnspecifiedDestination]     = "Terminated IPv6 header DIP is unspecified",
    [bcmRxTrapTerminatedIpv6LoopbackAddress]            = "Terminated IPv6 header SIP or DIP are loopback addresses",
    [bcmRxTrapTerminatedIpv6MulticastSource]            = "Terminated IPv6 header SIP is Multicast",
    [bcmRxTrapTerminatedIpv6NextHeaderNull]             = "Terminated IPv6 header next protocol is zero",
    [bcmRxTrapTerminatedIpv6UnspecifiedSource]          = "Terminated IPv6 header SIP is unspecified",
    [bcmRxTrapTerminatedIpv6LocalLinkDestination]       = "Terminated IPv6 header DIP bits 127:118 equal to 0x3FA",
    [bcmRxTrapTerminatedIpv6LocalSiteDestination]       = "Terminated IPv6 header DIP bits 127:118 equal to 0x3FB",
    [bcmRxTrapTerminatedIpv6LocalLinkSource]            = "Terminated IPv6 header SIP bits 127:118 equal to 0x3FA",
    [bcmRxTrapTerminatedIpv6LocalSiteSource]            = "Terminated IPv6 header SIP bits 127:118 equal to 0x3FB",
    [bcmRxTrapTerminatedIpv6Ipv4CompatibleDestination]  = "Terminated IPv6 header DIP bits 127:118 equal to zero",
    [bcmRxTrapTerminatedIpv6Ipv4MappedDestination]      = "Terminated IPv6 header DIP bits 127:118 equal to 0000_FFF_0000_0000_0000_0000",
    [bcmRxTrapTerminatedIpv6MulticastDestination]       = "Terminated IPv6 header DIP is Multicast",
    [bcmRxTrapTerminatedIpv4Fragmented]                 = "Terminated IPv4 header is fragmented",
    [bcmRxTrapForwardingIpv4VersionError]               = "Forwarding IPv4 header Version is different than 4",
    [bcmRxTrapForwardingIpv4ChecksumError]              = "Forwarding IPv4 Internet Header Length is 5 and the checksum over the first 20 bytes doesn't verify",
    [bcmRxTrapForwardingIpv4HeaderLengthError]          = "Forwarding IPv4 Internet Header Length is less than 5",
    [bcmRxTrapForwardingIpv4TotalLengthError]           = "Forwarding IPv4 header Total length is less than 20",
    [bcmRxTrapForwardingIpv4Ttl0]                       = "Forwarding IPv4 header TTL equals to zero",
    [bcmRxTrapForwardingIpv4HasOptions]                 = "Forwarding IPv4 Internet Header Length is greater than 5",
    [bcmRxTrapForwardingIpv4Ttl1]                       = "Forwarding IPv4 header TTL equals to one",
    [bcmRxTrapMplsForwardingTtl0]                       = "Mpls forwarding trap with TTL equal to 0.",
    [bcmRxTrapMplsForwardingTtl1]                       = "Mpls forwarding trap with TTL equal to 1.",
    [bcmRxTrapForwardingIpv6Ttl0]                       = "Fwd IPv6 TTL is equal to 0.",
    [bcmRxTrapForwardingIpv6Ttl1]                       = "Fwd IPv6 TTL is equal to 1.",
    [bcmRxTrapForwardingIpv4SipEqualDip]                = "Forwarding IPv4 source IP equals to destination IP",
    [bcmRxTrapForwardingIpv4DipZero]                    = "Forwarding IPv4 destination IP equals to zero",
    [bcmRxTrapForwardingIpv4SipIsMc]                    = "Forwarding IPv4 source ip is multicast",
    [bcmRxTrapForwardingIpv6VersionError]               = "Forwarding IPv6 header Version is different than 6",
    [bcmRxTrapForwardingIpv6UnspecifiedDestination]     = "Forwarding IPv6 header DIP is unspecified",
    [bcmRxTrapForwardingIpv6LoopbackAddress]            = "Forwarding IPv6 header SIP or DIP are loopback addresses",
    [bcmRxTrapForwardingIpv6MulticastSource]            = "Forwarding IPv6 header SIP is Multicast",
    [bcmRxTrapForwardingIpv6NextHeaderNull]             = "Forwarding IPv6 header next protocol is zero",
    [bcmRxTrapForwardingIpv6UnspecifiedSource]          = "Forwarding IPv6 header SIP is unspecified",
    [bcmRxTrapForwardingIpv6LocalLinkDestination]       = "Forwarding IPv6 header DIP bits 127:118 equal to 0x3FA",
    [bcmRxTrapForwardingIpv6LocalSiteDestination]       = "Forwarding IPv6 header DIP bits 127:118 equal to 0x3FB",
    [bcmRxTrapForwardingIpv6LocalLinkSource]            = "Forwarding IPv6 header SIP bits 127:118 equal to 0x3FA",
    [bcmRxTrapForwardingIpv6LocalSiteSource]            =  "Forwarding IPv6 header SIP bits 127:118 equal to 0x3FB",
    [bcmRxTrapForwardingIpv6Ipv4CompatibleDestination]  = "Forwarding IPv6 header DIP bits 127:118 equal to zero",
    [bcmRxTrapForwardingIpv6Ipv4MappedDestination]      = "Forwarding IPv6 header DIP bits 127:118 equal to 0000_FFF_0000_0000_0000_0000",
    [bcmRxTrapForwardingIpv6MulticastDestination]       = "Forwarding IPv6 header DIP is Multicast",
    [bcmRxTrapMplsTunnelTerminationTtl0]                = "Terminated IPv4 header TTL equals to zero",
    [bcmRxTrapMplsTunnelTerminationTtl1]                = "Terminated IPv4 header TTL equals to one",
    [bcmRxTrapTcpSnFlagsZero]                           = "TCP packet, L4 Sequence-Number and Flags are both zero",
    [bcmRxTrapTcpSnZeroFlagsSet]                        = "TCP packet, L4 Sequence-Number is zero and either FIN/URG/PSH Flags are set",
    [bcmRxTrapTcpSynFin]                                = "TCP packet, SYN and FIN Flags are both set",
    [bcmRxTrapTcpEqualPorts]                            = "TCP packet, L4 source port equals destination port",
    [bcmRxTrapTcpFragmentIncompleteHeader]              = "TCP packet, L3 is IPv4 and IP-Header Fragmented with Fragment-Offset zero, and IP-Header (Total-Length - 4*IHL) is less than 20",
    [bcmRxTrapTcpFragmentOffsetLt8]                     = "TCP packet, L3 is IPv4 and IP-Header Fragmented with Fragment-Offset less than 8",
    [bcmRxTrapUdpEqualPorts]                            = "UDP packet, L4 source port equals destination port",
    [bcmRxTrapIcmpDataGt576]                            = "ICMP packet, L3 is IPv4 and IP-Header (Total-Length - 4*IHL) is greater than 576B or L3 is IPv6 and IP-header Payload-Length is greater than 576B",
    [bcmRxTrapIcmpFragmented]                           = "ICMP packet and IP-Header is Fragmented",
    [bcmRxTrapL2DiscardMacsaFwd]                        = "Triggered by SA procedure (sa_drop), configured by default to FWD the packet",
    [bcmRxTrapL2DiscardMacsaDrop]                       = "Triggered by SA procedure (sa_drop), configured by default to Drop the packet",
    [bcmRxTrapL2DiscardMacsaTrap]                       = "Triggered by SA procedure (sa_drop), configured by default to redirect the packet to CPU",
    [bcmRxTrapL2DiscardMacsaSnoop]                      = "Triggered by SA procedure (sa_drop), configured by default to Snoop the packet",
    [bcmRxTrapL2Learn0]                                 = "Triggered by SA procedure (sa_not_found), configured by default to FWD the packet",
    [bcmRxTrapL2Learn1]                                 = "Triggered by SA procedure (sa_not_found), configured by default to Drop the packet",
    [bcmRxTrapL2Learn2]                                 = "Triggered by SA procedure (sa_not_found), configured by default to redirect the packet to CPU",
    [bcmRxTrapL2Learn3]                                 = "Triggered by SA procedure (sa_not_found), configured by default to Snoop the packet",
    [bcmRxTrapL2DlfFwd]                                 = "Triggered by Ethernet default procedure (da_not_found)",
    [bcmRxTrapL2DlfDrop]                                = "Triggered by Ethernet default procedure (da_not_found)",
    [bcmRxTrapL2DlfTrap]                                = "Triggered by Ethernet default procedure (da_not_found)",
    [bcmRxTrapL2DlfSnoop]                               = "Triggered by Ethernet default procedure (da_not_found)",
    [bcmRxTrap1588]                                     = "1588 protocol packet",
    [bcmRxTrap1588Discard]                              = "1588 protocol packet, configured by default to Drop the packet",
    [bcmRxTrap1588Accepted]                             = "1588 protocol packet, configured by default to FWD the packet",
    [bcmRxTrap1588User1]                                = "1588 protocol user trap #1",
    [bcmRxTrap1588User2]                                = "1588 protocol user trap #2",
    [bcmRxTrap1588User3]                                = "1588 protocol user trap #3",
    [bcmRxTrap1588User4]                                = "1588 protocol user trap #4",
    [bcmRxTrap1588User5]                                = "1588 protocol user trap #5",
    [bcmRxTrapExternalLookupError]                      =  "ELK is accessed and returns an error",
    [bcmRxTrapSameInterface]                            = "Source interface is equal to destination interface (hair-pin)",
    [bcmRxTrapUcLooseRpfFail]                           = "Forwarding Code is IPv4 UC and RPF FEC Pointer Valid is not set",
    [bcmRxTrapFcoeSrcIdMismatchSa]                      =  "FCoE packet, where FC.SrC mismatchs Eth.SA",
    [bcmRxTrapFailoverFacilityInvalid]                  =  "Both Destination-0-Valid and Destination-1-Valid are not set",
    [bcmRxTrapDfltDroppedPacket]                        = "Deafult action to drop packet",
    [bcmRxTrapDfltRedirectToCpuPacket]                  = "Deafult action to redirect packet to CPU",
    [bcmRxTrapUcStrictRpfFail]                          = "UC-RPF-Mode is 'Strict' and OutRIF is not equal to packet InRIF",
    [bcmRxTrapMcExplicitRpfFail]                        = "MC-RPF-Mode is 'Explicit' and RPF-Entry.Expected-InRIF is not equal to packet InRIF",
    [bcmRxTrapMcUseSipRpfFail]                          = "MC-RPF-Mode is 'Use-SIP-WITH-ECMP' and Out-RIF is not equal to In-RIF",
    [bcmRxTrapMcUseSipMultipath]                        = "MC-RPF-Mode is 'Use-SIP' and RPF-ECMP-Group-Size > 1",
    [bcmRxTrapIcmpRedirect]                             = "ICMP-Redirect is enabled, Forwarding-Code is IPv4|6-UC, and packet InRIF is equal to FEC-Entry.OutRIF",
    [bcmRxTrapEgressTrapped2ndPass]                     =  "Egress trapped packet on 2nd pass",
    [bcmRxTrapEgTxFieldSnoop0]                          = "Trap code reserved for mapping to snoop command #0",
    [bcmRxTrapEgTxFieldSnoop1]                          = "Trap code reserved for mapping to snoop command #1",
    [bcmRxTrapEgTxFieldSnoop2]                          = "Trap code reserved for mapping to snoop command #2",
    [bcmRxTrapEgTxFieldSnoop3]                          = "Trap code reserved for mapping to snoop command #3",
    [bcmRxTrapEgTxFieldSnoop4]                          = "Trap code reserved for mapping to snoop command #4",
    [bcmRxTrapEgTxFieldSnoop5]                          = "Trap code reserved for mapping to snoop command #5",
    [bcmRxTrapEgTxFieldSnoop6]                          = "Trap code reserved for mapping to snoop command #6",
    [bcmRxTrapEgTxFieldSnoop7]                          = "Trap code reserved for mapping to snoop command #7",
    [bcmRxTrapEgTxFieldSnoop8]                          = "Trap code reserved for mapping to snoop command #8",
    [bcmRxTrapEgTxFieldSnoop9]                          = "Trap code reserved for mapping to snoop command #9",
    [bcmRxTrapEgTxFieldSnoop10]                         = "Trap code reserved for mapping to snoop command #10",
    [bcmRxTrapEgTxFieldSnoop11]                         = "Trap code reserved for mapping to snoop command #11",
    [bcmRxTrapEgTxFieldSnoop12]                         = "Trap code reserved for mapping to snoop command #12",
    [bcmRxTrapEgTxFieldSnoop13]                         = "Trap code reserved for mapping to snoop command #13",
    [bcmRxTrapEgTxFieldSnoop14]                         = "Trap code reserved for mapping to snoop command #14",
    [bcmRxTrapEgTxFieldSnoop15]                         = "Trap code reserved for mapping to snoop command #15",
    [bcmRxTrapUserDefine]                               = "Ingress user defined trap, used to set trap action only",
    [bcmRxTrapOamEthAccelerated]                        = "Accelerated Ethernet OAM",
    [bcmRxTrapOamY1731MplsTp]                           = "Accelerated MPLS-TP OAM",
    [bcmRxTrapOamY1731Pwe]                              = "Accelerated PWE OAM",
    [bcmRxTrapOamBfdIpv4]                               = "BFD over IPv4 including Single Hop, Multi-Hop and Micro-BFD",
    [bcmRxTrapEgBfdIpv6InvalidUdpChecksum]              = "BFD over IPv6 UDP invalid checksum",
    [bcmRxTrapOamBfdIpv6]                               = "BFD over IPv6 including Single Hop, Multi-Hop and Micro-BFD",
    [bcmRxTrapOamBfdMpls]                               = "BFD over LSP, including IPv4 and IPv6",
    [bcmRxTrapOamBfdPwe]                                = "BFD over PWE",
    [bcmRxTrapOamReflector]                             = "OAM Down MEP LBM reply through reflector",
    [bcmRxTrapBfdOamDownMEP]                            = "Non accelerated default ingress trap for OAM and BFD",
    [bcmRxTrapSnoopOamPacket]                           = "OAM snoop",
    [bcmRxTrapOamLevel]                                 = "OAM packet with mdlevel below highest MEP on active side",
    [bcmRxTrapOamPassive]                               = "OAM packet equal or below highest MEP level on passive side",
    [bcmRxTrapMplsUnknownLabel]                         = "In case the MPLS lookup didnt hit any result",
    /**Erpp User Define Trap */
    [bcmRxTrapEgUserDefine]                             = "ERPP user defined trap, used to set trap action only",
    /**ETPP User Define Trap */
    [bcmRxTrapEgTxUserDefine]                           = "ETPP user defined trap, used to set trap action only",
    /**ETPP User Define Trap */
    [bcmRxTrapEgTxVisibility]                           = "ETPP trap activated when visibility bit is set",
    /**ETPP predefined OAM traps */
    [bcmRxTrapEgTxOamUpMEPOamp]                     = "ETPP trap for OAM Up MEPs with destination OAMP",
    [bcmRxTrapEgTxOamUpMEPDest1]                    = "ETPP trap for OAM Up MEPs with configurable destination",
    [bcmRxTrapEgTxOamUpMEPDest2]                    = "ETPP trap for OAM Up MEPs with configurable destination",
    [bcmRxTrapEgTxOamLevel]                         = "ETPP trap for OAM MD-level error",
    [bcmRxTrapEgTxOamPassive]                       = "ETPP trap for OAM passive error",
    [bcmRxTrapEgTxOamReflector]                     = "ETPP trap for OAM Up MEP LBM reply through reflector",
    [bcmRxTrapEgHairPinFilter]                      = "Source interface is equal to destination interface (hair-pin)",
    [bcmRxTrapEgSplitHorizonFilter]                 = "Triggered by conditions on incoming and outgoing packet orientation",
    [bcmRxTrapEgUnknownDa]                          = "Unknown Destination address",
    [bcmRxTrapEgDiscardFrameTypeFilter]             = "Unacceptable frame type on port (tagged, untagged)",
    [bcmRxTrapEgIpv4Ttl0]                           = "IPv4 header TTL equals to zero",
    [bcmRxTrapEgIpv4Ttl1]                           = "IPv4 header TTL equals to one",
    [bcmRxTrapDssStacking]                          = "Trap according to Dss stacking denail (history Bitmap)",
    [bcmRxTrapLagMulticast]                         = "Trap Lag Multicast Packets",
    [bcmRxTrapEgIpmcTtlErr]                         = "IPv4 MC packet with invalid ttl",
    [bcmRxTrapEgInvalidDestPort]                    = "Invalid out port number",
    [bcmRxTrapEgRqpDiscard]                         = "Triggered by discard indication from RQP",
    [bcmRxTrapEgTdmDiscard]                         = "Triggered by discard indication from TDM",
    [bcmRxTrapEgExcludeSrc]                         = "Trap packet of specific source",
    [bcmRxTrapEgGlemPpTrap]                         = "Triggered by ERPP GLEM PP error",
    [bcmRxTrapEgGlemNonePpTrap]                     = "Triggered by ERPP GLEM None PP error",
    [bcmRxTrapEgIpv4Error]                          = "Triggered by IPv4 error",
    [bcmRxTrapEgIpv6Error]                          = "Triggered by IPv6 error",
    [bcmRxTrapEgL4Error]                            = "Triggered by L4 error",
    [bcmRxTrapEgTxStpStateFail]                     = "Triggered by STP state failure",
    [bcmRxTrapEgTxProtectionPathUnexpected]         = "Unexpected Traffic on a Protecting Path",
    [bcmRxTrapEgTxSplitHorizonFilter]               = "Triggered by conditions on incoming and outgoing packet orientation",
    [bcmRxTrapEgTxDiscardFrameTypeFilter]           = "Unacceptable frame type on port (tagged, untagged)",
    [bcmRxTrapEgTxPortNotVlanMember]                = "VID membership error",
    [bcmRxTrapEgTxLatency]                          = "Triggered by latency measurements",
    [bcmRxTrapEgTxMetering]                         = "Triggered by meter limitations",
    [bcmRxTrapEgTxGlem]                             = "Triggered by ETPP GLEM error",
    [bcmRxTrapEgBfdIpv6InvalidUdpChecksum]          = "BFD IPv6 trap for invalid UDP checksum.",
    [bcmRxTrapOampPuntNextGoodPacket]               = "OAMP punt good packet trap.",
    [bcmRxTrapEgTcpSnFlagsZero]                     = "ERPP L4 Sequence-Number and Flags (6) are both zero.",
    [bcmRxTrapEgTcpSnZeroFlagsSet]                  = "ERPP L4 Sequence-Number is zero and either Flags. FIN, Flags. URG or FLAGS. PSH are set.",
    [bcmRxTrapEgTcpSynFin]                          = "ERPP Both Flags. SYN and Flags. FIN are set.",
    [bcmRxTrapEgTcpEqualPorts]                      = "ERPP Source-Port equals Destination-Port.",
    [bcmRxTrapEgTcpFragmentIncompleteHeader]        = "ERPP L3 is IPv4 and IP-Header. Fragmented and IP-Header. Fragment-Offset zero and (IPv4-Header. Total-Length - 4 * IPv4-Header. IHL) is less than 20B.",
    [bcmRxTrapEgTcpFragmentOffsetLt8]               = "ERPP L3 is IPv4 and IP-Header. Fragmented and IP-Header.  Fragment-Offset is less than 8.",
    [bcmRxTrapEgUdpEqualPorts]                      = "ERPP Source-Port equals  Destination-Port.",
    [bcmRxTrapEgIpv6VersionError]                   = "ERPP Version is different than 6..",
    [bcmRxTrapOamPerformanceEthAccelerated]         = "Down MEP trapping of LM/DM to OAMP",
    [bcmRxTrapOamPerformanceY1731MplsTp]            = "MPLS-TP trapping of LM/DM to OAMP",
    [bcmRxTrapOamPerformanceY1731Pwe]               = "PWE trapping of LM/DM to OAMP",
    [bcmRxTrapTerminatedSaEqualsZero]               = "Termination SA equals Zero.",
    [bcmRxTrapForwardingSaEqualsZero]               = "Forwarding SA equals Zero.",
    [bcmRxTrapFwdDomainAssignmentModeTrap]          = "Forward Domain Assignment Mode trap.",
    [bcmRxTrapEgIpv4VersionError]                   = "Egress trap: Version is different than 4..",
    [bcmRxTrapEgIpv4ChecksumError]                  = "Egress trap: IHL is 5 and the checksum over the first 20 bytes doesn't verify.",
    [bcmRxTrapEgIpv4HeaderLengthError]              = "Egress trap: IHL (Internet Header Length) is less than 5.",
    [bcmRxTrapEgIpv4TotalLengthError]               = "Egress trap: Total length is less than 20.",
    [bcmRxTrapEgIpv4Ttl0]                           = "Egress trap: TTL is equal to 0.",
    [bcmRxTrapEgIpv4HasOptions]                     = "Egress trap: IHL (Internet Header Length) is greater than 5.",
    [bcmRxTrapEgIpv4Ttl1]                           = "Egress trap: In-TTL is 0.",
    [bcmRxTrapEgIpv4SipEqualDip]                    = "Egress trap: Source-IP is equal to destination IP.",
    [bcmRxTrapEgIpv4DipZero]                        = "Egress trap: Destination IP is 0.",
    [bcmRxTrapEgIpv4SipIsMc]                        = "Egress trap: Source-IP is multicast.",
    [bcmRxTrapEgIpv6UnspecifiedDestination]         = "Egress trap: Forwarding header DIP = ::.",
    [bcmRxTrapEgIpv6LoopbackAddress]                = "Egress trap: DIP = ::1 or SIP = ::1.",
    [bcmRxTrapEgIpv6MulticastSource]                = "Egress trap: The MSB of the SIP =  0xFF.",
    [bcmRxTrapEgIpv6UnspecifiedSource]              = "Egress trap: SIP = ::.",
    [bcmRxTrapEgIpv6LocalLinkDestination]           = "Egress trap: Bits 127:118 of the destination-IP are equal to 0x3FA.",
    [bcmRxTrapEgIpv6LocalSiteDestination]           = "Egress trap: Bits 127:118 of the DIP = 0x3FB (deprecated).",
    [bcmRxTrapEgIpv6LocalLinkSource]                = "Egress trap: Bits 127:118 of the SIP = 0x3FA.",
    [bcmRxTrapEgIpv6LocalSiteSource]                = "Egress trap: Bits 127:118 of the SIP = 0x3FB.",
    [bcmRxTrapEgIpv6Ipv4CompatibleDestination]      = "Egress trap: Bits 127:32 of the DIP = 0.",
    [bcmRxTrapEgIpv6Ipv4MappedDestination]          = "Egress trap: Bits 127:32 of the DIP are equal to 0000_0000_0000_0000_0000_FFFF.",
    [bcmRxTrapEgIpv6MulticastDestination]           = "Egress trap: MSB of the DIP=0xFF.",
    [bcmRxTrapEgIpv6NextHeaderNull]                 = "Egress trap: Next-protocol is zero.",
    [bcmRxTrapEgTxIfaEgressMetadata]                = "ETPP trap for IFA Egress Node with metadata mirroring."
};

/* *INDENT-ON* */

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_predefined_map_info_get(
    int unit,
    bcm_rx_trap_t trap_type,
    dnx_rx_trap_map_type_t * predefined_trap_info_p)
{
    uint32 hw_value = 0;
    uint8 is_illegal;
    uint8 bfdv6_invalid_udp_checksum_block = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(predefined_trap_info_p, _SHR_E_PARAM, "predefined_trap_info_p");

    if (trap_type < bcmRxTrapCount)
    {

        predefined_trap_info_p->is_strengthless = dnx_rx_trap_type_map[trap_type].is_strengthless;
        predefined_trap_info_p->trap_block = dnx_rx_trap_type_map[trap_type].trap_block;
        predefined_trap_info_p->trap_class = dnx_rx_trap_type_map[trap_type].trap_class;
        predefined_trap_info_p->trap_id = dnx_rx_trap_type_map[trap_type].trap_id;
        predefined_trap_info_p->valid = dnx_rx_trap_type_map[trap_type].valid;

        
        bfdv6_invalid_udp_checksum_block =
            dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_checksum_feature_fixed);
        if ((trap_type == bcmRxTrapEgBfdIpv6InvalidUdpChecksum) && (bfdv6_invalid_udp_checksum_block == TRUE))
        {
            predefined_trap_info_p->valid = FALSE;
            SHR_EXIT();
        }

        if ((predefined_trap_info_p->trap_class != DNX_RX_TRAP_CLASS_OAM) &&
            (predefined_trap_info_p->trap_class != DNX_RX_TRAP_CLASS_USER_DEFINED) &&
            (trap_type != bcmRxTrapLinkLayerHeaderSizeErr))
        {
            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                            (unit, DBAL_FIELD_INGRESS_TRAP_ID, dnx_rx_trap_type_map[trap_type].trap_id, &is_illegal));
            if (is_illegal)
            {
                predefined_trap_info_p->valid = FALSE;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                                (unit, DBAL_FIELD_INGRESS_TRAP_ID, dnx_rx_trap_type_map[trap_type].trap_id, &hw_value));
                predefined_trap_info_p->valid = (dnx_rx_trap_type_map[trap_type].valid && hw_value);
            }
        }

        sal_strncpy(predefined_trap_info_p->trap_name, dnx_rx_trap_type_map[trap_type].trap_name,
                    (DBAL_MAX_STRING_LENGTH - 1));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap_type=%d\n", trap_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_appl_trap_map_info_get(
    int unit,
    bcm_rx_trap_t trap_type,
    dnx_rx_trap_map_type_t * appl_trap_info_p)
{
    uint32 dbal_enum_val = 0;
    uint8 is_illegal;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(appl_trap_info_p, _SHR_E_PARAM, "appl_trap_info_p");

    if (trap_type < bcmRxTrapCount)
    {
        appl_trap_info_p->trap_block = dnx_rx_app_trap_type_map[trap_type].trap_block;
        appl_trap_info_p->trap_class = dnx_rx_app_trap_type_map[trap_type].trap_class;
        appl_trap_info_p->trap_id = dnx_rx_app_trap_type_map[trap_type].trap_id;
        appl_trap_info_p->valid = dnx_rx_app_trap_type_map[trap_type].valid;
        sal_strncpy(appl_trap_info_p->trap_name, dnx_rx_app_trap_type_map[trap_type].trap_name, DBAL_MAX_STRING_LENGTH);

        /*
         * Get the DBAL Enum value of the Egress traps to valdiate if it is mapped and valid for the current device.
         */
        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                        (unit, DBAL_FIELD_ERPP_TRAP_ID, dnx_rx_app_trap_type_map[trap_type].trap_id, &is_illegal));
        if (is_illegal)
        {
            appl_trap_info_p->valid = FALSE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_ERPP_TRAP_ID, dnx_rx_app_trap_type_map[trap_type].trap_id,
                             &dbal_enum_val));
            /*
             * Doing an AND operation between the MAP valid bit and the dbal_enum_value
             * will tell us if the trap is actually valid for the device.
             * If Trap is INVALID for the device, the dbal_enum_val will be 0.
             */
            appl_trap_info_p->valid = (dnx_rx_app_trap_type_map[trap_type].valid && dbal_enum_val);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap_type=%d\n", trap_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_user_defined_block_map_info_get(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_ud_t * ud_block_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ud_block_info_p, _SHR_E_PARAM, "ud_block_info_p");

    if (DNX_RX_TRAP_BLOCK_IS_VALID(trap_block))
    {
        ud_block_info_p->lower_bound_trap_id = dnx_rx_ud_trap_map[trap_block].lower_bound_trap_id;
        ud_block_info_p->upper_bound_trap_id = dnx_rx_ud_trap_map[trap_block].upper_bound_trap_id;
        sal_memcpy(ud_block_info_p->default_trap_id_list, dnx_rx_ud_trap_map[trap_block].default_trap_id_list,
                   (sizeof(int) * (DNX_RX_TRAP_MAP_MAX_NOF_DEFAULT_TRAPS)));
        sal_strncpy(ud_block_info_p->block_name, dnx_rx_ud_trap_map[trap_block].block_name,
                    (DBAL_MAX_STRING_LENGTH - 1));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap_type = %d\n", trap_block);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_etpp_ud_profile_map_info_get(
    int unit,
    dnx_rx_trap_etpp_fwd_act_profiles_e fwd_action_profile,
    dnx_rx_trap_map_etpp_profile_dbal_fields_t * ud_profile_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ud_profile_info_p, _SHR_E_PARAM, "ud_profile_info_p");

    if (fwd_action_profile >= DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1
        && fwd_action_profile <= DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_KEEP_FABRIC_PACKET)
    {
        ud_profile_info_p->cfg_cpu_trap_code = dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_cpu_trap_code;
        ud_profile_info_p->cfg_drop_fwd_copy = dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_drop_fwd_copy;
        ud_profile_info_p->cfg_gen_recycle_copy =
            dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_gen_recycle_copy;
        ud_profile_info_p->cfg_pp_drop_reason = dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_pp_drop_reason;
        ud_profile_info_p->cfg_rcy_append_orig =
            dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_rcy_append_orig;
        ud_profile_info_p->cfg_rcy_cmd = dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_rcy_cmd;
        ud_profile_info_p->cfg_rcy_crop = dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_rcy_crop;
        ud_profile_info_p->cfg_rcy_prio = dnx_rx_trap_map_etpp_ud_profile[fwd_action_profile].cfg_rcy_prio;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ETPP Action Profile = %d\n", fwd_action_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_etpp_oam_profile_map_info_get(
    int unit,
    dnx_rx_trap_etpp_oam_types_e oam_action_profile,
    dnx_rx_trap_map_etpp_profile_dbal_fields_t * oam_profile_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(oam_profile_info_p, _SHR_E_PARAM, "oam_profile_info_p");

    if (oam_action_profile > DNX_RX_TRAP_ETPP_OAM_INVALID && oam_action_profile < DNX_RX_TRAP_ETPP_OAM_NOF_TRAPS)
    {
        oam_profile_info_p->cfg_cpu_trap_code = dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_cpu_trap_code;
        oam_profile_info_p->cfg_drop_fwd_copy = dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_drop_fwd_copy;
        oam_profile_info_p->cfg_gen_recycle_copy =
            dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_gen_recycle_copy;
        oam_profile_info_p->cfg_pp_drop_reason =
            dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_pp_drop_reason;
        oam_profile_info_p->cfg_rcy_append_orig =
            dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_rcy_append_orig;
        oam_profile_info_p->cfg_rcy_cmd = dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_rcy_cmd;
        oam_profile_info_p->cfg_rcy_crop = dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_rcy_crop;
        oam_profile_info_p->cfg_rcy_prio = dnx_rx_trap_map_etpp_oam_profile[oam_action_profile].cfg_rcy_prio;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ETPP OAM Action Profile = %d\n", oam_action_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_stat_obj_map_info_get(
    int unit,
    uint32 stat_obj_profile,
    dnx_rx_trap_map_stat_obj_dbal_fields_t * stat_obj_info_p)
{
    uint32 max_nof_stat_objs_to_ow = dnx_data_trap.ingress.max_nof_stat_objs_to_overwrite_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stat_obj_info_p, _SHR_E_PARAM, "stat_obj_info_p");

    if (stat_obj_profile < max_nof_stat_objs_to_ow)
    {
        stat_obj_info_p->index = dnx_rx_trap_map_stat_obj[stat_obj_profile].index;
        stat_obj_info_p->is_meter = dnx_rx_trap_map_stat_obj[stat_obj_profile].is_meter;
        stat_obj_info_p->offset_by_qual = dnx_rx_trap_map_stat_obj[stat_obj_profile].offset_by_qual;
        stat_obj_info_p->overwrite = dnx_rx_trap_map_stat_obj[stat_obj_profile].overwrite;
        stat_obj_info_p->type = dnx_rx_trap_map_stat_obj[stat_obj_profile].type;
        stat_obj_info_p->value = dnx_rx_trap_map_stat_obj[stat_obj_profile].value;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Stat Object Profile = %d\n", stat_obj_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_ingress_ud_map_info_get(
    int unit,
    int trap_id,
    dnx_rx_trap_map_ingress_ud_t * ingress_ud_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ingress_ud_info_p, _SHR_E_PARAM, "ingress_ud_info_p");

    if (trap_id < DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES)
    {
        ingress_ud_info_p->alloc_index = dnx_rx_trap_map_ingress_ud[trap_id].alloc_index;
        ingress_ud_info_p->valid = dnx_rx_trap_map_ingress_ud[trap_id].valid;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap_id=%d\n", trap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_map.h for more info
 */
shr_error_e
dnx_rx_trap_id_boundry_ranges_map_info_get(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_ranges_t * trap_id_range_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trap_id_range_info_p, _SHR_E_PARAM, "trap_id_range_info_p");

    if (DNX_RX_TRAP_BLOCK_IS_VALID(trap_block))
    {
        trap_id_range_info_p->lower_bound_trap_id = dnx_rx_range_trap_map[trap_block].lower_bound_trap_id;
        trap_id_range_info_p->upper_bound_trap_id = dnx_rx_range_trap_map[trap_block].upper_bound_trap_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap block = %d\n", trap_block);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_rx_trap_type_description_map_info_get(
    int unit,
    bcm_rx_trap_t trap_type,
    char **description)
{

    SHR_FUNC_INIT_VARS(unit);

    if (trap_type < bcmRxTrapCount)
    {
        if (dnx_rx_trap_type_description[trap_type] != NULL)
        {
            *description = (char *) dnx_rx_trap_type_description[trap_type];
        }
        else
        {
            *description = "N/A";
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap type = %d! Should be less than bcmRxTrapCount(%d)\n", trap_type,
                     bcmRxTrapCount);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_description_init_check(
    int unit)
{
    int trap_type;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    int valid = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    for (trap_type = 0; trap_type < bcmRxTrapCount; trap_type++)
    {
        valid = FALSE;
        if (dnx_rx_trap_type_map[trap_type].valid == TRUE)
        {
            valid = TRUE;
            sal_strncpy(trap_name, dnx_rx_trap_type_map[trap_type].trap_name, (sizeof(trap_name) - 1));
        }
        else if (dnx_rx_app_trap_type_map[trap_type].valid == TRUE)
        {
            valid = TRUE;
            sal_strncpy(trap_name, dnx_rx_trap_type_map[trap_type].trap_name, (sizeof(trap_name) - 1));
        }

        if (valid && ISEMPTY(dnx_rx_trap_type_description[trap_type]))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Trap :\"%s\" is mapped but has no valid description\n", trap_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
