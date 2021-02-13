
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX
#include <shared/shrextend/shrextend_debug.h>
#include <sal/compiler.h>
#include <soc/types.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/register.h>
#include <soc/dnx/mdb.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include "bcm_int/dnx/init/init_pp.h"
#include <src/bcm/dnx/init/init_pp_common.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ire_packet_generator_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <src/bcm/dnx/multicast/multicast_pp.h>
#include <sal/core/boot.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>
#include <bcm_int/dnx/switch/switch_em_false_hit.h>
#include <bcm_int/dnx/field/field_init.h>

typedef struct
{
    int layer_type_enum_val;
    int is_ethernet;
    int is_arp;
    int is_ipv4;
    int is_ipv6;
    int is_mpls_term;
    int is_mpls_fwd;
    int is_oam;
    int is_udp;
    int is_bfd_single_hop;
    int is_bfd_multi_hop;
    int is_tcp;
    int is_icmp;
    int is_icmpv6;
    int is_8021_x_2;
    int is_igmp;
    int is_bier_mpls;
} per_layer_protocol_t;

typedef struct ecol_ffc_s
{
    uint8 opcode;
    uint8 index;
    uint16 field_offset;
    uint8 field_size;
    uint8 key_offset;
} ecol_ffc_t;

typedef struct
{
    dbal_enum_value_field_current_protocol_type_e current_protocol_type;
    dbal_enum_value_field_etpp_next_protocol_namespace_e current_protocol_namespace;
    int current_next_protocol;
} current_next_protocol_config_table_t;

#define PPH_BASE_SIZE 12
#define PPH_BASE_SIZE_LEGACY 7
#define BUILD_ECOL_FFC_INSTRUCTION_32(ecol_ffc) ((ecol_ffc.opcode << 24) | (ecol_ffc.index << 21) \
                                  | (ecol_ffc.field_offset << 10) | (ecol_ffc.field_size << 5) \
                                  | (ecol_ffc.key_offset << 0))
#define BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc) ((ecol_ffc.opcode << 22) | (ecol_ffc.index << 19) \
                                  | (ecol_ffc.field_offset << 8) | (ecol_ffc.field_size << 4) \
                                  | (ecol_ffc.key_offset << 0))
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET (0)
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET (1)
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET (2)

#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(qualifier_p, segment_left_is_zero) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET, segment_left_is_zero))
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(qualifier_p, segment_left_is_one) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET, segment_left_is_one))
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(qualifier_p, single_pass_termination) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET, single_pass_termination))

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_OFFSET (1)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SIZE (1)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_OFFSET (2)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SIZE (5)
#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_OFFSET (7)
#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SIZE (1)

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(qualifier_p, _first_add_header_exists_) \
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_OFFSET, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SIZE, \
                                _first_add_header_exists_))

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(qualifier_p, _first_add_header_) \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_OFFSET, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SIZE, \
                                _first_add_header_))

#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(qualifier_p, _srv6_sl0_single_pass_) \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_OFFSET, \
                                LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SIZE, \
                                _srv6_sl0_single_pass_))

static const current_next_protocol_config_table_t current_next_protocol_config_map[] = {
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FCOE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_FCOE_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PPPOE_SESSION, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_PPPOE_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV2, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ERSPAN_OVER_GRE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV3, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ERSPAN_OVER_GRE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UDP, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GRE_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_BOS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT,
     DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_VXLAN_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN_GPE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_VXLAN_GPE_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GENEVE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GENEVE_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GRE_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_SRH_ONLY, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_SRV6_OVER_IPV6},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT,
     DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_VXLAN},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GTP, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GTPU_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PTP, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_PTP_OVER_ETHERNET}
};

static const per_layer_protocol_t per_layer_protocol[] = {
    {DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ARP, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_FCOE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_TCP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_UDP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PTP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ICMP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
     TRUE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_NON_MPLS, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE,
     FALSE, FALSE, FALSE, TRUE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IN_BIER_BASE_EG_BIER_MPLS, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, TRUE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_RCH, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IGMP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, TRUE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}
};

shr_error_e
egress_eedb_type_buster_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ESEM_GLOBAL, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, TRUE,
                                                    DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    if (max_value > 0)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, 1, DBAL_RANGE_ALL);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EM_BUSTER_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_eedb_forwarding_domain_vsd_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE_VSD_ENABLE, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, TRUE,
                                                    DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    if (max_value > 0)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, 1, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_eedb_svtag_indication_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE_SVTAG_ENABLE, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_SVTAG_ENABLE, TRUE,
                                                        DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
        if (max_value > 0)
        {
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_SVTAG_ENABLE, 1,
                                             DBAL_RANGE_ALL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_ENABLE, INST_SINGLE, TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_prt_profile_configuration(
    int unit,
    int prt_dnx_data_table_index)
{

    uint32 entry_handle_id;
    int ffc_instruction_5;
    int ffc_instruction_4;
    int ffc_instruction_3;
    int ffc_instruction_2;
    int ffc_instruction_1;
    int ffc_instruction_0;
    const dnx_data_port_general_prt_configuration_t *prt_configuration;
    uint32 prt_ffc_width_start_offset;
    uint32 prt_ffc_start_input_offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    prt_ffc_width_start_offset = dnx_data_port.general.prt_ffc_width_start_offset_get(unit);
    prt_ffc_start_input_offset = dnx_data_port.general.prt_ffc_start_input_offset_get(unit);
    prt_configuration = dnx_data_port.general.prt_configuration_get(unit, prt_dnx_data_table_index);
    ffc_instruction_5 =
        BUILD_PRT_FFC(prt_configuration->ffc_5_offset, prt_configuration->ffc_5_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_5_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_4 =
        BUILD_PRT_FFC(prt_configuration->ffc_4_offset, prt_configuration->ffc_4_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_4_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_3 =
        BUILD_PRT_FFC(prt_configuration->ffc_3_offset, prt_configuration->ffc_3_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_3_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_2 =
        BUILD_PRT_FFC(prt_configuration->ffc_2_offset, prt_configuration->ffc_2_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_2_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_1 =
        BUILD_PRT_FFC(prt_configuration->ffc_1_offset, prt_configuration->ffc_1_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_1_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_0 =
        BUILD_PRT_FFC(prt_configuration->ffc_0_offset, prt_configuration->ffc_0_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_0_input_offset, prt_ffc_start_input_offset);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_RECYCLE_PROFILE,
                               prt_configuration->prt_recycle_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE,
                               prt_configuration->port_termination_ptc_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_MODE, INST_SINGLE,
                                 prt_configuration->prt_qual_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE,
                                 prt_configuration->layer_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 4,
                                 prt_configuration->kbr_valid_bitmap_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 3,
                                 prt_configuration->kbr_valid_bitmap_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 2,
                                 prt_configuration->kbr_valid_bitmap_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 1,
                                 prt_configuration->kbr_valid_bitmap_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 0,
                                 prt_configuration->kbr_valid_bitmap_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 5, ffc_instruction_5);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 4, ffc_instruction_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 3, ffc_instruction_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 2, ffc_instruction_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 1, ffc_instruction_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 0, ffc_instruction_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_PRT_MODE, INST_SINGLE,
                                 prt_configuration->src_sys_port_prt_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT_ENABLE, INST_SINGLE,
                                 prt_configuration->pp_port_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_MODE_FULL_ENABLE, INST_SINGLE,
                                 prt_configuration->tcam_mode_full_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PRT_MODE, INST_SINGLE,
                                 prt_configuration->context_select);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_prt_recycle_command_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_RECYCLE_COMMAND, &entry_handle_id));


    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD, 2);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_ETH_APPEND64);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_ETH_APPEND64);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_prt_profile_configuration_init(
    int unit)
{
    const dnxc_data_table_info_t *prt_config_dnx_data_table_info;
    int index;
    SHR_FUNC_INIT_VARS(unit);

    prt_config_dnx_data_table_info = dnx_data_port.general.prt_configuration_info_get(unit);

    for (index = 0; index < prt_config_dnx_data_table_info->key_size[0]; index++)
    {
        SHR_IF_ERR_EXIT(ingress_prt_profile_configuration(unit, index));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_prt_configuration_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ingress_prt_recycle_command_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_prt_profile_configuration_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_lbp_vlan_editing_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LBP_VLAN_EDITING, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_IVE_SIZE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FHEI_SIZE_5B);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ippe_parser_context_mapping(
    int unit)
{
    uint32 entry_handle_id;
    uint32 hw_context_val = 0, enum_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    enum_value = INVALID_ENUM;

    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                    (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INVALID_ENUM, &enum_value));
    while (enum_value != INVALID_ENUM)
    {

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, enum_value, &hw_context_val));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_PARSING_CONTEXT_MAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_PARSING_CONTEXT, hw_context_val);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INST_SINGLE,
                                     enum_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, enum_value, &enum_value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ippb_l4_protocols_config(
    int unit)
{
    uint32 entry_handle_id;
    int ipv_tcp_protocol = 6;
    int ipv_udp_protocol = 17;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_PROTOCOL_L4_VALUES, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV_TCP_PROTOCL, INST_SINGLE, ipv_tcp_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV_UDP_PROTOCL, INST_SINGLE, ipv_udp_protocol);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_port_general_configuration_init(
    int unit)
{

    uint32 entry_handle_id, pp_port, mapped_pp_port;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        for (pp_port = 0; pp_port < dnx_data_port.general.nof_pp_ports_get(unit); pp_port++)
        {
            mapped_pp_port =
                ((core_id << utilex_log2_round_up(dnx_data_port.general.nof_pp_ports_get(unit))) | pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, INST_SINGLE, mapped_pp_port);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_da_type_map_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_DA_TYPE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));


    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_stat_fec_ecmp_mapping_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FEC_ECMP_STATISTICS_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FEC_STAT_OBJECT_PROFILE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FEC_STATISTIC_PROFILE_MAP_TWO_INDIRECT_COUNTER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECMP_STATISTIC_PROFILE_MAP_ECMP_GROUP);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ire_packet_generator_init(
    int unit)
{
    uint8 is_ire_packet_init = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ire_packet_generator_info.is_init(unit, &is_ire_packet_init));
    if (!is_ire_packet_init)
    {
        SHR_IF_ERR_EXIT(ire_packet_generator_info.init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_layer_protocol_allow_speculative_learning(
    int unit,
    int layer_entry,
    int enable)
{
    uint32 entry_handle_id;
    uint8 is_illegal = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_LAYER_TYPES, layer_entry, &is_illegal));
    if (!is_illegal)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_entry);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECULATIVE_PROTOCOL_ETH_ENABLE, INST_SINGLE,
                                     enable);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
egress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ipv4,
    int is_ipv6,
    int is_mpls,
    int is_oam,
    int is_tcp,
    int is_udp)
{

    uint32 entry_handle_id;
    uint8 is_illegal = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val, &is_illegal));
    if (is_illegal)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_ENABLE, INST_SINGLE, is_ipv4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_ENABLE, INST_SINGLE, is_ipv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_ENABLE, INST_SINGLE, is_mpls);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_OAM_ENABLE, INST_SINGLE, is_oam);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_TCP_ENABLE, INST_SINGLE, is_tcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_UDP_ENABLE, INST_SINGLE, is_udp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ethernet,
    int is_arp,
    int is_ipv4,
    int is_ipv6,
    int is_mpls_term,
    int is_mpls_fwd,
    int is_udp,
    int is_bfd_single_hop,
    int is_bfd_multi_hop,
    int is_icmpv6,
    int is_igmp,
    int is_8021_x_2,
    int is_icmp,
    int is_bier_mpls)
{

    uint32 entry_handle_id;
    int system_headers_mode;
    int allow_speculative_learning;
    uint8 is_illegal = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val, &is_illegal));
    if (is_illegal)
    {
        SHR_EXIT();
    }

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_ENABLE, INST_SINGLE, is_ethernet);

    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
        (!dnx_data_headers.
         general.feature_get(unit, dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_MINUS_ONE_ENABLE, INST_SINGLE, 0);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_MINUS_ONE_ENABLE, INST_SINGLE,
                                     is_ethernet);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ARP_ENABLE, INST_SINGLE, is_arp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IP_ENABLE, INST_SINGLE,
                                 (is_ipv4 | is_ipv6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_ENABLE, INST_SINGLE, is_ipv4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_ENABLE, INST_SINGLE, is_ipv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_TERM_ENABLE, INST_SINGLE,
                                 is_mpls_term);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_FWD_ENABLE, INST_SINGLE, is_mpls_fwd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_UDP_ENABLE, INST_SINGLE, is_udp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BFD_SINGLE_HOP_ENABLE, INST_SINGLE,
                                 is_bfd_single_hop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BFD_MULTI_HOP_ENABLE, INST_SINGLE,
                                 is_bfd_multi_hop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ICMPV6_ENABLE, INST_SINGLE, is_icmpv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IGMP_ENABLE, INST_SINGLE, is_igmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_8021_X_2_ENABLE, INST_SINGLE, is_8021_x_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ICMP_ENABLE, INST_SINGLE, is_icmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BIER_MPLS_ENABLE, INST_SINGLE,
                                 is_bier_mpls);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (!dnx_data_mpls.general.mpls_speculative_learning_handling_get(unit))
    {
        allow_speculative_learning = 1;
    }
    else
    {
        allow_speculative_learning = is_ethernet;
    }
    SHR_IF_ERR_EXIT(ingress_layer_protocol_allow_speculative_learning
                    (unit, layer_type_enum_val, allow_speculative_learning));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_egress_per_layer_protocol_configuration_init(
    int unit)
{
    int entry;
    int nof_layres_to_config;
    uint32 ingress_layer_check[DBAL_NOF_ENUM_LAYER_TYPES_VALUES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    nof_layres_to_config = sizeof(per_layer_protocol) / sizeof(per_layer_protocol_t);
    for (entry = 0; entry < nof_layres_to_config; entry++)
    {
        SHR_IF_ERR_EXIT(ingress_per_layer_protocol_configuration(unit, per_layer_protocol[entry].layer_type_enum_val,
                                                                 per_layer_protocol[entry].is_ethernet,
                                                                 per_layer_protocol[entry].is_arp,
                                                                 per_layer_protocol[entry].is_ipv4,
                                                                 per_layer_protocol[entry].is_ipv6,
                                                                 per_layer_protocol[entry].is_mpls_term,
                                                                 per_layer_protocol[entry].is_mpls_fwd,
                                                                 per_layer_protocol[entry].is_udp,
                                                                 per_layer_protocol[entry].is_bfd_single_hop,
                                                                 per_layer_protocol[entry].is_bfd_multi_hop,
                                                                 per_layer_protocol[entry].is_icmpv6,
                                                                 per_layer_protocol[entry].is_igmp,
                                                                 per_layer_protocol[entry].is_8021_x_2,
                                                                 per_layer_protocol[entry].is_icmp,
                                                                 per_layer_protocol[entry].is_bier_mpls));
        ingress_layer_check[per_layer_protocol[entry].layer_type_enum_val] = 1;
    }

    if (!dnx_data_mpls.general.mpls_speculative_learning_handling_get(unit))
    {
        for (entry = 0; entry < DBAL_NOF_ENUM_LAYER_TYPES_VALUES; entry++)
        {
            if (ingress_layer_check[entry] == 0)
            {
                SHR_IF_ERR_EXIT(ingress_layer_protocol_allow_speculative_learning(unit, entry, 1));
            }
        }
    }

    for (entry = 0; entry < nof_layres_to_config; entry++)
    {
        SHR_IF_ERR_EXIT(egress_per_layer_protocol_configuration(unit, per_layer_protocol[entry].layer_type_enum_val,
                                                                per_layer_protocol[entry].is_ipv4,
                                                                per_layer_protocol[entry].is_ipv6,
                                                                per_layer_protocol[entry].is_mpls_term,
                                                                per_layer_protocol[entry].is_oam,
                                                                per_layer_protocol[entry].is_tcp,
                                                                per_layer_protocol[entry].is_udp));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_udp_next_protocol_enable_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERUDP, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4OUDP_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6OUDP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pph_base_size_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 pph_base_size;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        pph_base_size = PPH_BASE_SIZE_LEGACY;
    }
    else
    {
        pph_base_size = PPH_BASE_SIZE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, pph_base_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_init_pp_layer_types_value_get(
    int unit,
    uint32 prototype)
{
    uint32 hw_enum_value;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_LAYER_TYPES, prototype, &hw_enum_value));

    return hw_enum_value;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ecologic_srv6_segment_endpoint_init(
    int unit)
{
    uint32 access_id;
    uint32 entry_handle_id;
    uint32 layer_qualifier;
    uint32 layer_qualifier_mask;
    ecol_ffc_t ecol_ffc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&ecol_ffc, 0, sizeof(ecol_ffc_t));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_CONTEXT_ID, DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_SEGMENT_ENDPOINT,
                     &access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECOL_IDENTIFICATION_CAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND);
    layer_qualifier_mask = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier_mask, TRUE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier_mask, TRUE);

    layer_qualifier = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier, FALSE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier, FALSE);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_PREVIOUSLAYER, layer_qualifier,
                                      layer_qualifier_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                                      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER, 0, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_FWD_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_SNP_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_MRR_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PP_PORT_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PTC_PROFILE, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_CONTEXT_ATTRIBUTES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_CONTEXT_ID,
                               DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_SEGMENT_ENDPOINT);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE, 0);
    ecol_ffc.opcode = 0;
    ecol_ffc.index = 7;
    ecol_ffc.field_offset = (144 * 8 - 3 * 8 - 8);
    ecol_ffc.field_size = 7;
    ecol_ffc.key_offset = 0;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_2, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    sal_memset(&ecol_ffc, 0, sizeof(ecol_ffc_t));
    ecol_ffc.opcode = 0;
    ecol_ffc.index = 7;
    ecol_ffc.field_offset = (144 * 8 - 3 * 8 - 8 + 7);
    ecol_ffc.field_size = 0;
    ecol_ffc.key_offset = 0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x08);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_MODE_USE_CAM_TABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_SRC, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_OP_B_SRC_USE_CFG);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_CFG, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_CALCULATION_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_CALCULATION_MODE_OP_A_MINUS_OP_B);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_ADD_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_BTC_OFFSET_ADD_MODE_SHIFT);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x8);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE_CAM, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                     DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT, &access_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VARIABLE_0, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE, INST_SINGLE, 4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ecologic_srv6_egress_tunnel_1_pass_init(
    int unit)
{
    uint32 access_id;
    uint32 entry_handle_id;
    uint32 layer_qualifier;
    uint32 layer_qualifier_mask;
    uint32 srv6_sl_0_additionl_header_hw_val;
    ecol_ffc_t ecol_ffc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_CONTEXT_ID, DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_1_PASS,
                     &access_id));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                     DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0, &srv6_sl_0_additionl_header_hw_val));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECOL_IDENTIFICATION_CAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    layer_qualifier_mask = 0;
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(&layer_qualifier_mask, 1);
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(&layer_qualifier_mask, 0x1F);
    LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(&layer_qualifier_mask, 1);

    layer_qualifier = 0;
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(&layer_qualifier, 1);
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(&layer_qualifier, srv6_sl_0_additionl_header_hw_val);
    LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(&layer_qualifier, 1);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_PREVIOUSLAYER, layer_qualifier,
                                      layer_qualifier_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                                      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER, 0, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_FWD_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_SNP_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_MRR_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PP_PORT_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PTC_PROFILE, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_CONTEXT_ATTRIBUTES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_CONTEXT_ID,
                               DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_1_PASS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    ecol_ffc.opcode = 0;
    ecol_ffc.index = 0;
    ecol_ffc.field_offset = (128 + 64 - 40);
    ecol_ffc.field_size = 7;
    ecol_ffc.key_offset = 4;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_2, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));
    sal_memset(&ecol_ffc, 0, sizeof(ecol_ffc_t));
    ecol_ffc.opcode = 0;
    ecol_ffc.index = 0;
    ecol_ffc.field_offset = (128 + 64 - 32);
    ecol_ffc.field_size = 0;
    ecol_ffc.key_offset = 0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_OFFSET);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_MODE_USE_CAM_TABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_SRC, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_OP_B_SRC_USE_CFG);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_CFG, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_CALCULATION_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_CALCULATION_MODE_OP_A_MINUS_OP_B);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_ADD_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_BTC_OFFSET_ADD_MODE_SHIFT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE_CAM, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX, DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE,
                     &access_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VARIABLE_0, 0, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ecologic_srv6_egress_tunnel_2_pass_init(
    int unit)
{
    uint32 access_id;
    uint32 entry_handle_id;
    uint32 layer_qualifier;
    uint32 layer_qualifier_mask;
    uint32 srv6_sl_0_additionl_header_hw_val;
    uint32 btc_offset;
    ecol_ffc_t ecol_ffc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_CONTEXT_ID, DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_2_PASS,
                     &access_id));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                     DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0, &srv6_sl_0_additionl_header_hw_val));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECOL_IDENTIFICATION_CAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    layer_qualifier_mask = 0;
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(&layer_qualifier_mask, 1);
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(&layer_qualifier_mask, 0x1F);
    LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(&layer_qualifier_mask, 1);

    layer_qualifier = 0;
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(&layer_qualifier, 1);
    LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(&layer_qualifier, srv6_sl_0_additionl_header_hw_val);
    LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(&layer_qualifier, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_PREVIOUSLAYER, layer_qualifier,
                                      layer_qualifier_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                                      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER, 0, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_FWD_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_SNP_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_MRR_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PP_PORT_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PTC_PROFILE, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_CONTEXT_ATTRIBUTES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_CONTEXT_ID,
                               DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_2_PASS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    sal_memset(&ecol_ffc, 0, sizeof(ecol_ffc_t));
    ecol_ffc.opcode = 0;
    ecol_ffc.index = 0;
    ecol_ffc.field_offset = (128 + 64 - 32);
    ecol_ffc.field_size = 0;
    ecol_ffc.key_offset = 0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_2, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0);

    btc_offset = (16 * (dnx_data_srv6.termination.max_nof_terminated_sids_usp_1pass_get(unit)));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_SHIFT_IN_BYTES, INST_SINGLE,
                                 btc_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ecologic_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ingress_ecologic_srv6_segment_endpoint_init(unit));
    SHR_IF_ERR_EXIT(ingress_ecologic_srv6_egress_tunnel_1_pass_init(unit));
    SHR_IF_ERR_EXIT(ingress_ecologic_srv6_egress_tunnel_2_pass_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_itpp_general_network_header_termination_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_network_header_termination))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ITPP_TERMINATE_NETWORK_HEADERS, &entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_STRENGTH, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TM_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_TERMINATE_NETWORK_HEADERS, INST_SINGLE,
                                     TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_STRENGTH, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PROFILE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_TERMINATE_NETWORK_HEADERS, INST_SINGLE,
                                     FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_cfg_in_lif_null_value_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 egress_in_lif_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    egress_in_lif_val = dnx_data_lif.global_lif.egress_in_lif_null_value_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ETPP_GLOBAL, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_IN_LIF_NULL_VALUE, INST_SINGLE,
                                 egress_in_lif_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_pp_egress_current_next_protocol_config_table_configure(
    int unit,
    int current_protocol_type,
    int current_protocol_namespace,
    int current_next_protocol)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_PROTOCOL_TYPE, current_protocol_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PROTOCOL_NAMESPACE, current_protocol_namespace);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_NEXT_PROTOCOL, INST_SINGLE,
                                 current_next_protocol);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_current_next_protocol_config_table_init(
    int unit)
{

    int nof_current_next_protocol_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    nof_current_next_protocol_entries =
        sizeof(current_next_protocol_config_map) / sizeof(current_next_protocol_config_table_t);

    for (entry = 0; entry < nof_current_next_protocol_entries; entry++)
    {
        SHR_IF_ERR_EXIT(dnx_init_pp_egress_current_next_protocol_config_table_configure
                        (unit, current_next_protocol_config_map[entry].current_protocol_type,
                         current_next_protocol_config_map[entry].current_protocol_namespace,
                         current_next_protocol_config_map[entry].current_next_protocol));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
egress_ingress_trapped_by_fhei_configure_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_INGRESS_TRAPPED_BY_FHEI, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, DBAL_ENUM_FVAL_FHEI_TYPE_TRAP_SNOOP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
