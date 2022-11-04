/*
 * out_lif_headers_profile.c
 *
 * General Procedures for Egress additional headers profile management for DNX.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/compiler.h>
#include <soc/types.h>
#include <soc/memory.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/lif/out_lif_headers_profile.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/** Generates MPLS header_specific_information value:
 *   {Label-Type(2),Label(20),EXP-Type(1),EXP(3),TTL-Type(1),TTL(8)}*/
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_TYPE_OFFSET     33
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_OFFSET          13
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_TYPE_OFFSET       12
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_OFFSET             9
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_TYPE_OFFSET        8
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_OFFSET             0
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(_protocol_specific_info_, _field_val_, _field_offset_)  \
    do {                                                                                                            \
      COMPILER_UINT64 field_val_64_bits;                                                                            \
      COMPILER_64_SET(field_val_64_bits, 0, _field_val_);                                                           \
      COMPILER_64_SHL(field_val_64_bits, _field_offset_);                                                           \
      COMPILER_64_OR(_protocol_specific_info_, field_val_64_bits);                                                  \
    } while (0);                                                                                                    \

    /** Control word encode*/
#define MPLS_CW_FLAG_BITS_WIDTH                        4
#define MPLS_CW_SPECIFIC_INFORMATION(val, seq_src) ((val<<1) | seq_src)
    /** VXLAN Protocol Specific Information*/
#define VXLAN_PROTOCOL_SPECIFIC_INFORMATION     8
    /** VXLAN GPE Protocol Specific Information*/
#define VXLAN_GPE_PROTOCOL_SPECIFIC_INFORMATION 0xC
    /** GRE Protocol Specific Information*/
#define GRE_PROTOCOL_SPECIFIC_INFORMATION       0
    /** UDP Protocol Specific Information*/
#define UDP_PROTOCOL_SPECIFIC_INFORMATION       0
    /** VXLAN UDP Protocol Specific Information:
     *  destination_port_info: 0x1
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set)
     *  destination_port: 4789
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set) source_port_info: 0x0
     *  (indicate the source port use load balancing key)
     *  source_port:      0x0000
     *  reserved: 0x0
     *   */
#define VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION (0x256B)
    /** VXLAN UDP Protocol Specific Information:
     *  destination_port_info: 0x1
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set)
     *  destination_port: 4789
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set)
     *  source_port_info: 0x1
     *  (indicate the source port use load balancing key)
     *  source_port:      0x0000
     *  reserved: 0x0
     *   */
#define VXLAN_DOUBLE_UDP_PROTOCOL_SPECIFIC_INFORMATION (0x2256B)
    /** ESP UDP Protocol Specific Information:
     *  These values won't be updated by an API and will remain fixed.
     *  destination_port_info: 0x1
     *  destination_port:      4500
     *  source_port_info:      0x1
     *  source_port:           4500
     *  reserved: 0x0
     */
#define UDP_ESP_PROTOCOL_SPECIFIC_INFORMATION (0x46522329)
    /** VXLAN GPE UDP Protocol Specific Information:
     *  destination_port_info: 0x1
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set)
     *  destination_port: 4790
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set) source_port_info: 0x0 (indicate the
     *  source port use load balancing key) source_port:      0x0000
     *  reserved: 0x0
     *   */
#define VXLAN_GPE_UDP_PROTOCOL_SPECIFIC_INFORMATION (0x256D)
/** GENEVE UDP Protocol Specific Information*/
#define GENEVE_UDP_PROTOCOL_SPECIFIC_INFORMATION      0

    /** GENEVE Protocol Specific Information*/
#define GENEVE_PROTOCOL_SPECIFIC_INFORMATION    0
    /** Next Protocol is GRE*/
#define CURRENT_NEXT_PROTOCOL_GRE               47
    /** Next Protocol is GRE*/
#define CURRENT_NEXT_PROTOCOL_UDP               17
    /** Next Protocol is ESP*/
#define CURRENT_NEXT_PROTOCOL_ESP               50
    /** Next Protocol is GENEVE*/
#define CURRENT_NEXT_PROTOCOL_GENEVE            6081
    /** Next Protocol is VXLAN*/
#define CURRENT_NEXT_PROTOCOL_VXLAN             4087

/** MPLS Header length */
#define MPLS_BYTES_TO_ADD                       4
/** VXLAN Header length */
#define VXLAN_BYTES_TO_ADD                      8
/** GENEVE Header length */
#define GENEVE_BYTES_TO_ADD                     8
/** GRE4 Header length */
#define GRE4_BYTES_TO_ADD                       4
/** GRE8 Header length */
#define GRE8_BYTES_TO_ADD                       8
/** GRE12 Header length */
#define GRE12_BYTES_TO_ADD                      12
/** UDP Header length */
#define UDP_BYTES_TO_ADD                        8

/*
 * }
 */

/*
 * Initialization table for additional_headers_profile_config_map, which configures DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP Table.
 * Each line configures the Additional Headers Profile, based on the Context Additional Header Profile and LIF Additional Header Profile
 */
static const dnx_additional_headers_profile_map_t additional_headers_profile_config_map[] = {
   /** LIF Additional Headers Profile,                                                                                     Context Additional Headers Profile,                                         Additional Headers Profile*/
    {DBAL_ENUM_FVAL_LIF_AHP_GENERAL_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE,
     ADDITIONAL_HEADERS_NONE, AHS_NAMESPACE_GENERAL},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_NONE, AHS_NAMESPACE_MPLS},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_ESI_EVPN, AHS_NAMESPACE_MPLS_IML},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_CW_ESI_EVPN, AHS_NAMESPACE_MPLS_IML},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_FL_ESI_EVPN, AHS_NAMESPACE_MPLS_IML},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_CW_FL_ESI_EVPN, AHS_NAMESPACE_MPLS_IML},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_CW_EL_ELI_PWE, AHS_NAMESPACE_MPLS},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_CW_FL_PWE, AHS_NAMESPACE_MPLS},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_ELOELI, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_EL_ELI, AHS_NAMESPACE_MPLS},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_CW_PWE, AHS_NAMESPACE_MPLS},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_FL_PWE, AHS_NAMESPACE_MPLS},
    {DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN_GPE,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE, ADDITIONAL_HEADERS_VXLAN_GPE_UDP,
     AHS_NAMESPACE_VXLAN},
    {DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE, ADDITIONAL_HEADERS_VXLAN_UDP,
     AHS_NAMESPACE_VXLAN},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GENEVE,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_GENEVE_UDP,
     AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,
     ADDITIONAL_HEADERS_GRE4, AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_WO_TNI_GRE8,
     AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_TNI_GRE8,
     AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE12,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_GRE12,
     AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_IPV6_TNI_GRE8,
     AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN,
     AHS_NAMESPACE_GRE_OR_GENEVE},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP, AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_1, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(1), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_2, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(2), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_3, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(3), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(4), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_5, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(5), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_6, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(6), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_7, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(7), AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_ESP, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_ESP, AHS_NAMESPACE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP,
     ADDITIONAL_HEADERS_NONE, AHS_NAMESPACE_IP_TUNNEL},
    {DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_IP_ESP, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP,
     DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IP_ESP, AHS_NAMESPACE_IP_TUNNEL},
    {DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_DOUBLE_UDP_ESP_VXLAN,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE, ADDITIONAL_HEADERS_VXLAN_DOUBLE_UDP_ESP,
     AHS_NAMESPACE_VXLAN},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_IOAM,
     ADDITIONAL_HEADERS_IOAM_FL_FLI, AHS_NAMESPACE_MPLS_IOAM},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_IOAM,
     ADDITIONAL_HEADERS_IOAM_CW_FL_FLI, AHS_NAMESPACE_MPLS_IOAM}
};

static const dnx_additional_headers_profile_properties_t additional_headers_map[] = {
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_NONE, 0, 0, 0, 0,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_ESI_EVPN, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_ESI_EVPN, 1, 0, 1, MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /** Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_FL_ESI_EVPN, 1, 0, 1, 3 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /** Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_FL_ESI_EVPN, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_EL_ELI_PWE, 1, 0, 1, 3 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_FL_PWE, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_EL_ELI, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_PWE, 0, 0, 1, MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_FL_PWE, 1, 0, 1, MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_VXLAN_GPE_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, VXLAN_GPE_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, CURRENT_NEXT_PROTOCOL_VXLAN, 1, VXLAN_BYTES_TO_ADD, COMPILER_64_INIT(0,
                                                                             VXLAN_GPE_UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additional Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_GENEVE_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + GENEVE_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, GENEVE_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_GENEVE_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, CURRENT_NEXT_PROTOCOL_GENEVE, 1, GENEVE_BYTES_TO_ADD, COMPILER_64_INIT(0,
                                                                               GENEVE_UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_VXLAN_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     1, 0, 0, 0, COMPILER_64_INIT(0, VXLAN_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, CURRENT_NEXT_PROTOCOL_VXLAN, 1, VXLAN_BYTES_TO_ADD, COMPILER_64_INIT(0,
                                                                             VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_GRE4, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE4_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE4_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_WO_TNI_GRE8, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE8K_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_TNI_GRE8, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_GRE12, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE12_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE12_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_IPV6_TNI_GRE8, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 8), DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(1), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(2), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(3), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(4), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(5), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(6), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(7), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_ESP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1,
     UDP_BYTES_TO_ADD,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, UDP_ESP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IP_ESP, 1, CURRENT_NEXT_PROTOCOL_ESP, 0, 0,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    {ADDITIONAL_HEADERS_VXLAN_DOUBLE_UDP_ESP,
     1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
      /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     1, 0, 0, 0, COMPILER_64_INIT(0, VXLAN_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start, Current_Next_Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD), COMPILER_64_INIT(0,
                                                                                                VXLAN_DOUBLE_UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
      /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, CURRENT_NEXT_PROTOCOL_VXLAN, 1, VXLAN_BYTES_TO_ADD, COMPILER_64_INIT(0, UDP_ESP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    {ADDITIONAL_HEADERS_IOAM_FL_FLI, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_IOAM_CW_FL_FLI, 1, 0, 1, 3 * MPLS_BYTES_TO_ADD,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION}
};

/**
 * \brief - The function calculates MPLS protocol specific information
 *          for EGRESS_ADDITIONAL_HEADERS_MAP_TABLE
 * \param [in] label_type
 * \param [in] label_val
 * \param [in] exp_type
 * \param [in] exp_val
 * \param [in] ttl_type
 * \param [in] ttl_val
 * \param [out] protocol_specific_info - MPLS protocol header
                specific information
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
egress_mpls_protocol_header_specific_information_get(
    uint32 label_type,
    uint32 label_val,
    uint32 exp_type,
    uint32 exp_val,
    uint32 ttl_type,
    uint32 ttl_val,
    COMPILER_UINT64 * protocol_specific_info)
{
    COMPILER_64_ZERO(*protocol_specific_info);
    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, label_type,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_TYPE_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, label_val,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, exp_type,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_TYPE_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, exp_val,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, ttl_type,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_TYPE_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, ttl_val,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_OFFSET);

    return;
}

/**
 * \brief - Build Control Word in the specific_info field of additional_header_profile.
 * \param [in] unit - unit ID
 * \param [in] value - value for build CW
 * \param [out] protocol_specific_info - MPLS protocol header specific information
 *
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
egress_mpls_protocol_specific_data_cw_build(
    int unit,
    int value,
    COMPILER_UINT64 * protocol_specific_info)
{
    int flags_bits_lsb = 0, flags_bits_val = 0;
    int cw_value;

    flags_bits_lsb = dnx_data_mpls.general.mpls_control_word_flag_bits_lsb_get(unit);
    flags_bits_val = (value >> flags_bits_lsb) & UTILEX_BITS_MASK(MPLS_CW_FLAG_BITS_WIDTH - 1, 0);
    cw_value = MPLS_CW_SPECIFIC_INFORMATION(flags_bits_val, 0);

    COMPILER_64_SET(*protocol_specific_info, 0, cw_value);

    return;
}

/**
 * \brief - Get the identifier index in profile_map or profile_properties array.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] additional_headers_identifier - Identifier for the ETPP additional headers profile.
 * \param [out] profile_properties_idx - Index of the profile identifier in profile properties array.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_properties_idx_get(
    int unit,
    int additional_headers_identifier,
    int *profile_properties_idx)
{
    int nof_entries, idx;

    SHR_FUNC_INIT_VARS(unit);

    nof_entries = sizeof(additional_headers_map) / sizeof(dnx_additional_headers_profile_properties_t);
    for (idx = 0; idx < nof_entries; idx++)
    {
        /** The identifier should be unique in the property array. Compare the identifier is enough.*/
        if (additional_headers_map[idx].additional_headers_profile_identifier == additional_headers_identifier)
        {
            break;
        }
    }

    if (idx >= nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The identifier(%d) is not defined!\n", additional_headers_identifier);
    }

    *profile_properties_idx = idx;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the profile map info and the related ETPP additional headers profile
 *          with the identifier.
 * \param [in] unit - Relevant unit.
 * \param [in] additional_headers_identifier - Identifier for the ETPP additional headers profile.
 * \param [out] additional_headers_profile - ETPP additional headers profile.
 *              Valid only when the is_allocated indication is set.
 * \param [out] additional_headers_profile_is_allocated - Allocated indication.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_identifier_info_get(
    int unit,
    int additional_headers_identifier,
    int *additional_headers_profile,
    uint8 *additional_headers_profile_is_allocated)
{
    int rv = 0;
    dnx_additional_headers_info_t additional_headers_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&additional_headers_info, 0, sizeof(additional_headers_info));
    additional_headers_info.additional_headers_profile_identifier = additional_headers_identifier;

    /** Check if the identifier is used in profile allocation.*/
    *additional_headers_profile_is_allocated = FALSE;
    *additional_headers_profile = 0;

    rv = algo_tunnel_db.additional_headers_profile.profile_get
        (unit, &additional_headers_info, additional_headers_profile);
    if (rv == _SHR_E_NONE)
    {
        *additional_headers_profile_is_allocated = TRUE;
    }
    else if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures the additional_headers_profile map table
 * \param [in] unit - Relevant unit.
 * \param [in] lif_additional_headers_profile - LIF Additional Headers Profile.
 * \param [in] ctx_additional_headers_profile - context additional_headers_profile
 * \param [in] additional_headers_profile - Additional Headers Profile
 *             If the value is 0, it's actually a clear operation.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_map_table_set(
    int unit,
    int lif_additional_headers_profile,
    int ctx_additional_headers_profile,
    int additional_headers_profile)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e ahp_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP table
     */
    switch (ctx_additional_headers_profile)
    {
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_UDP_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_UDP;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_GRE_GENEVE_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_GRE_GENEVE;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_VXLAN_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_VXLAN_VXLAN_GPE;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH:
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI:
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_IOAM:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MPLS_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_MPLS;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_IP_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_IP;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE:
            /** Select an table to configure the zero value */
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MPLS_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_MPLS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "CTX additional header profile is unknown\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, ahp_field_id, lif_additional_headers_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CTX_ADDITIONAL_HEADER_PROFILE,
                               ctx_additional_headers_profile);
    if (additional_headers_profile == 0)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PROFILE,
                                     INST_SINGLE, additional_headers_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configures the additional_headers_profile properties in HW table
 * \param [in] unit - Relevant unit.
 * \param [in] additional_headers_profile - Additional Headers Profile.
 * \param [in] profile_properties - Pointer for additional headers profile.
 *             NULL means clear the table entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_properties_hw_set(
    int unit,
    int additional_headers_profile,
    dnx_additional_headers_profile_properties_t * profile_properties)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to EGRESS_ADDITIONAL_HEADERS_MAP_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PROFILE,
                               additional_headers_profile);
    if (profile_properties == NULL)
    {
        /** It's clear operation */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_EXIT();
    }

    /** It's add operation */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 0,
                                 profile_properties->main_header_start_current_next_protocol_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 1,
                                 profile_properties->additional_header_0_start_current_next_protocol_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 2,
                                 profile_properties->additional_header_1_start_current_next_protocol_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 3,
                                 profile_properties->additional_header_2_start_current_next_protocol_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 0,
                                 profile_properties->main_header_start_current_next_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 1,
                                 profile_properties->additional_header_0_start_current_next_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 2,
                                 profile_properties->additional_header_1_start_current_next_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 3,
                                 profile_properties->additional_header_2_start_current_next_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 0,
                                 profile_properties->main_header_additional_headers_packet_size_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 1,
                                 profile_properties->additional_header_0_packet_size_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 2,
                                 profile_properties->additional_header_1_packet_size_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 3,
                                 profile_properties->additional_header_2_packet_size_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 0,
                                 profile_properties->main_header_additional_headers_packet_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 1,
                                 profile_properties->additional_header_0_packet_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 2,
                                 profile_properties->additional_header_1_packet_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 3,
                                 profile_properties->additional_header_2_packet_size);
    dbal_entry_value_field64_set(unit, entry_handle_id,
                                 profile_properties->additional_header_0_type, 0,
                                 profile_properties->additional_header_0_protocol_specific_data);
    dbal_entry_value_field64_set(unit, entry_handle_id,
                                 profile_properties->additional_header_1_type, 1,
                                 profile_properties->additional_header_1_protocol_specific_data);
    dbal_entry_value_field64_set(unit, entry_handle_id,
                                 profile_properties->additional_header_2_type, 2,
                                 profile_properties->additional_header_2_protocol_specific_data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the profile properties in the given index.
 *          It updates the swstate only.
 * \param [in] unit - Relevant unit.
 * \param [in] idx - index.
 * \param [in] profile_properties - profile properties.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_properties_sw_set(
    int unit,
    int idx,
    dnx_additional_headers_profile_properties_t profile_properties)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Additional_heder_identifier*/
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_headers_profile_identifier.set
                    (unit, idx, profile_properties.additional_headers_profile_identifier));

    /** Main header properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_start_current_next_protocol_enable.set
                    (unit, idx, profile_properties.main_header_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_start_current_next_protocol.set
                    (unit, idx, profile_properties.main_header_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_additional_headers_packet_size_enable.set
                    (unit, idx, profile_properties.main_header_additional_headers_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_additional_headers_packet_size.set
                    (unit, idx, profile_properties.main_header_additional_headers_packet_size));

    /** Additional headers 0 properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.
                    additional_header_0_start_current_next_protocol_enable.set(unit, idx,
                                                                               profile_properties.
                                                                               additional_header_0_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_start_current_next_protocol.set
                    (unit, idx, profile_properties.additional_header_0_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_packet_size_enable.set
                    (unit, idx, profile_properties.additional_header_0_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_packet_size.set
                    (unit, idx, profile_properties.additional_header_0_packet_size));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_protocol_specific_data.set
                    (unit, idx, profile_properties.additional_header_0_protocol_specific_data));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_type.set
                    (unit, idx, profile_properties.additional_header_0_type));

    /** Additional headers 1 properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.
                    additional_header_1_start_current_next_protocol_enable.set(unit, idx,
                                                                               profile_properties.
                                                                               additional_header_1_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_start_current_next_protocol.set
                    (unit, idx, profile_properties.additional_header_1_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_packet_size_enable.set
                    (unit, idx, profile_properties.additional_header_1_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_packet_size.set
                    (unit, idx, profile_properties.additional_header_1_packet_size));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_protocol_specific_data.set
                    (unit, idx, profile_properties.additional_header_1_protocol_specific_data));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_type.set
                    (unit, idx, profile_properties.additional_header_1_type));

    /** Additional headers 2 properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.
                    additional_header_2_start_current_next_protocol_enable.set(unit, idx,
                                                                               profile_properties.
                                                                               additional_header_2_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_start_current_next_protocol.set
                    (unit, idx, profile_properties.additional_header_2_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_packet_size_enable.set
                    (unit, idx, profile_properties.additional_header_2_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_packet_size.set
                    (unit, idx, profile_properties.additional_header_2_packet_size));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_protocol_specific_data.set
                    (unit, idx, profile_properties.additional_header_2_protocol_specific_data));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_type.set
                    (unit, idx, profile_properties.additional_header_2_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the profile properties in the given index.
 *          It reads the swstate only.
 * \param [in] unit - Relevant unit.
 * \param [in] idx - index.
 * \param [out] profile_properties - profile properties.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_properties_sw_get(
    int unit,
    int idx,
    dnx_additional_headers_profile_properties_t * profile_properties)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Additional_heder_identifier*/
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_headers_profile_identifier.get
                    (unit, idx, &profile_properties->additional_headers_profile_identifier));

    /** Main header properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_start_current_next_protocol_enable.get
                    (unit, idx, &profile_properties->main_header_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_start_current_next_protocol.get
                    (unit, idx, &profile_properties->main_header_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_additional_headers_packet_size_enable.get
                    (unit, idx, &profile_properties->main_header_additional_headers_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.main_header_additional_headers_packet_size.get
                    (unit, idx, &profile_properties->main_header_additional_headers_packet_size));

    /** Additional headers 0 properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.
                    additional_header_0_start_current_next_protocol_enable.get(unit, idx,
                                                                               &profile_properties->
                                                                               additional_header_0_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_start_current_next_protocol.get
                    (unit, idx, &profile_properties->additional_header_0_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_packet_size_enable.get
                    (unit, idx, &profile_properties->additional_header_0_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_packet_size.get
                    (unit, idx, &profile_properties->additional_header_0_packet_size));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_protocol_specific_data.get
                    (unit, idx, &profile_properties->additional_header_0_protocol_specific_data));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_0_type.get
                    (unit, idx, &profile_properties->additional_header_0_type));

    /** Additional headers 1 properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.
                    additional_header_1_start_current_next_protocol_enable.get(unit, idx,
                                                                               &profile_properties->
                                                                               additional_header_1_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_start_current_next_protocol.get
                    (unit, idx, &profile_properties->additional_header_1_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_packet_size_enable.get
                    (unit, idx, &profile_properties->additional_header_1_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_packet_size.get
                    (unit, idx, &profile_properties->additional_header_1_packet_size));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_protocol_specific_data.get
                    (unit, idx, &profile_properties->additional_header_1_protocol_specific_data));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_1_type.get
                    (unit, idx, &profile_properties->additional_header_1_type));

    /** Additional headers 2 properties. */
    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.
                    additional_header_2_start_current_next_protocol_enable.get(unit, idx,
                                                                               &profile_properties->
                                                                               additional_header_2_start_current_next_protocol_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_start_current_next_protocol.get
                    (unit, idx, &profile_properties->additional_header_2_start_current_next_protocol));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_packet_size_enable.get
                    (unit, idx, &profile_properties->additional_header_2_packet_size_enable));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_packet_size.get
                    (unit, idx, &profile_properties->additional_header_2_packet_size));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_protocol_specific_data.get
                    (unit, idx, &profile_properties->additional_header_2_protocol_specific_data));

    SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_properties.additional_header_2_type.get
                    (unit, idx, &profile_properties->additional_header_2_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configures the additional_headers_profile properties in HW & SW table
 * \param [in] unit - Relevant unit.
 * \param [in] additional_headers_identifier - Additional Headers Profile identifier.
 * \param [in] profile_properties - Pointer for additional headers profile.
 *             NULL means clear the table entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_properties_set(
    int unit,
    int additional_headers_identifier,
    dnx_additional_headers_profile_properties_t * profile_properties)
{
    int idx;
    int additional_headers_profile;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_hw_set
                        (unit, additional_headers_identifier, profile_properties));
        SHR_EXIT();
    }

    /** Look for the index of the identifier. */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_idx_get(unit, additional_headers_identifier, &idx));

    /** Update the swstate*/
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_sw_set(unit, idx, *profile_properties));

    /*
     * Check if HW should be updated too --
     *  If the header profile is allocated already, HW should be updated.
     */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_identifier_info_get
                    (unit, additional_headers_identifier, &additional_headers_profile, &is_allocated));
    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_hw_set
                        (unit, additional_headers_profile, profile_properties));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize the profile properties with default value for each profile identifier.
 * \param [in] unit - Relevant unit.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_properties_init(
    int unit)
{
    int nof_tables_entries, idx;
    int profile_identifier;
    COMPILER_UINT64 esi_protocol_specific_info;
    COMPILER_UINT64 el_protocol_specific_info;
    COMPILER_UINT64 eli_protocol_specific_info;
    COMPILER_UINT64 ioam_fl_protocol_specific_info;
    COMPILER_UINT64 ioam_fli_protocol_specific_info;
    COMPILER_UINT64 protocol_specific_data_header_1;
    COMPILER_UINT64 protocol_specific_data_header_2;

    dnx_additional_headers_profile_properties_t profile_properties;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(esi_protocol_specific_info);
    COMPILER_64_ZERO(el_protocol_specific_info);
    COMPILER_64_ZERO(eli_protocol_specific_info);
    COMPILER_64_ZERO(ioam_fl_protocol_specific_info);
    COMPILER_64_ZERO(ioam_fli_protocol_specific_info);

    /** Label Type=2, EXP Type=1, EXP=0, TTL Type=1, TTL=1 */
    egress_mpls_protocol_header_specific_information_get(2, 0, 1, 0, 0, 1, &esi_protocol_specific_info);
    /** Label Type=0, EXP Type=1, EXP=0, TTL Type=0, TTL=0 */
    egress_mpls_protocol_header_specific_information_get(0, 0, 1, 0, 0, 0, &el_protocol_specific_info);
    /** Label Type=1, Label=7, EXP Type=1, EXP=0, TTL Type=1, TTL=0 */
    egress_mpls_protocol_header_specific_information_get(1, 7, 1, 0, 1, 0, &eli_protocol_specific_info);
    /** Label Type=2, Label from dual_hom, EXP Type=0 (finally changed in ucode), TTL Type=0, TTL=1 */
    egress_mpls_protocol_header_specific_information_get(2, 0, 0, 0, 0, 1, &ioam_fl_protocol_specific_info);
    /** Label Type=0, Label can be changed by API, EXP Type=1, TTL Type=0 TTL=1*/
    egress_mpls_protocol_header_specific_information_get(1, 0, 1, 0, 0, 1, &ioam_fli_protocol_specific_info);

    /*
     * Initialize the swstate according to the predefined table
     */
    nof_tables_entries = sizeof(additional_headers_map) / sizeof(dnx_additional_headers_profile_properties_t);
    for (idx = 0; idx < nof_tables_entries; idx++)
    {
        /** Copy the initial values. */
        sal_memcpy(&profile_properties, &(additional_headers_map[idx]), sizeof(profile_properties));

        profile_identifier = profile_properties.additional_headers_profile_identifier;

        if (!dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit) &&
            (profile_identifier >= dnx_data_pp.ETPP.nof_additional_headers_profiles_get(unit)))
        {
            continue;
        }

        /** Update the specific data per applications. */
        protocol_specific_data_header_1 = additional_headers_map[idx].additional_header_1_protocol_specific_data;
        protocol_specific_data_header_2 = additional_headers_map[idx].additional_header_2_protocol_specific_data;

        if (profile_identifier == ADDITIONAL_HEADERS_CW_ESI_EVPN || profile_identifier == ADDITIONAL_HEADERS_ESI_EVPN)
        {
            protocol_specific_data_header_2 = esi_protocol_specific_info;
        }
        else if (profile_identifier == ADDITIONAL_HEADERS_CW_FL_ESI_EVPN ||
                 profile_identifier == ADDITIONAL_HEADERS_FL_ESI_EVPN)
        {
            protocol_specific_data_header_1 = el_protocol_specific_info;
            protocol_specific_data_header_2 = esi_protocol_specific_info;
        }
        else if (profile_identifier == ADDITIONAL_HEADERS_CW_EL_ELI_PWE ||
                 profile_identifier == ADDITIONAL_HEADERS_EL_ELI)
        {
            protocol_specific_data_header_1 = el_protocol_specific_info;
            protocol_specific_data_header_2 = eli_protocol_specific_info;
        }
        else if (profile_identifier == ADDITIONAL_HEADERS_FL_PWE)
        {
            protocol_specific_data_header_2 = el_protocol_specific_info;
        }
        else if (profile_identifier == ADDITIONAL_HEADERS_IOAM_FL_FLI ||
                 profile_identifier == ADDITIONAL_HEADERS_IOAM_CW_FL_FLI)
        {
            protocol_specific_data_header_1 = ioam_fl_protocol_specific_info;
            protocol_specific_data_header_2 = ioam_fli_protocol_specific_info;
        }

        profile_properties.additional_header_1_protocol_specific_data = protocol_specific_data_header_1;
        profile_properties.additional_header_2_protocol_specific_data = protocol_specific_data_header_2;

        /*
         * Write to the swstate table
         */
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_set
                        (unit, profile_identifier, &profile_properties));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the additional header specific info with the given value.
 * 
 * \param [in] unit - Relevant unit.
 * \param [in] additional_headers_identifier - Identifier for the ETPP additional headers profile.
 * \param [in] value - value used for updating the specific information.
 * \param [in] header_to_update - which header to be udpated.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_specific_data_set(
    int unit,
    int additional_headers_identifier,
    uint32 value[2],
    int header_to_update)
{
    int idx;
    dnx_additional_headers_profile_properties_t profile_properties;
    COMPILER_UINT64 *protocol_specific_data_header_0 = &(profile_properties.additional_header_0_protocol_specific_data);
    COMPILER_UINT64 *protocol_specific_data_header_1 = &(profile_properties.additional_header_1_protocol_specific_data);
    COMPILER_UINT64 *protocol_specific_data_header_2 = &(profile_properties.additional_header_2_protocol_specific_data);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Look for the identifier index in the static array
     */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_idx_get(unit, additional_headers_identifier, &idx));

    /** Get the initial values. */
    if (dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_sw_get(unit, idx, &profile_properties));
    }
    else
    {
        sal_memcpy(&profile_properties, &(additional_headers_map[idx]), sizeof(profile_properties));
    }

    /** Build the specific data for the application*/
    if (additional_headers_identifier == ADDITIONAL_HEADERS_IOAM_FL_FLI)
    {
        COMPILER_UINT64 ioam_fli_protocol_specific_info;
        COMPILER_64_ZERO(ioam_fli_protocol_specific_info);

        /** Label Type=0, Label can be changed by API, EXP Type=1, TTL Type=0, TTL=1 */
        egress_mpls_protocol_header_specific_information_get(1, value[0], 1, 0, 0, 1, &ioam_fli_protocol_specific_info);

        *protocol_specific_data_header_2 = ioam_fli_protocol_specific_info;
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_IOAM_CW_FL_FLI)
    {
        COMPILER_UINT64 ioam_fli_protocol_specific_info;
        COMPILER_64_ZERO(ioam_fli_protocol_specific_info);

        /** Label Type=0, Label can be changed by API, EXP Type=1, TTL Type=0, TTL=1 */
        egress_mpls_protocol_header_specific_information_get(1, value[0], 1, 0, 0, 1, &ioam_fli_protocol_specific_info);

        if (header_to_update == 0)
        {
            egress_mpls_protocol_specific_data_cw_build(unit, value[0], protocol_specific_data_header_0);
        }
        else if (header_to_update == 2)
        {
            *protocol_specific_data_header_2 = ioam_fli_protocol_specific_info;
        }
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_CW_FL_ESI_EVPN)
    {
        COMPILER_UINT64 esi_protocol_specific_info;
        COMPILER_UINT64 el_protocol_specific_info;

        COMPILER_64_ZERO(esi_protocol_specific_info);
        COMPILER_64_ZERO(el_protocol_specific_info);

        /** ESI: Label Type=2, EXP Type=1, EXP=0, TTL Type=1, TTL=1 */
        egress_mpls_protocol_header_specific_information_get(2, 0, 1, 0, 0, 1, &esi_protocol_specific_info);
        /** EL: Label Type=0, EXP Type=1, EXP=0, TTL Type=0, TTL=0 */
        egress_mpls_protocol_header_specific_information_get(0, 0, 1, 0, 0, 0, &el_protocol_specific_info);

        egress_mpls_protocol_specific_data_cw_build(unit, value[0], protocol_specific_data_header_0);
        *protocol_specific_data_header_1 = el_protocol_specific_info;
        *protocol_specific_data_header_2 = esi_protocol_specific_info;
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_CW_EL_ELI_PWE)
    {
        COMPILER_UINT64 el_protocol_specific_info;
        COMPILER_UINT64 eli_protocol_specific_info;

        COMPILER_64_ZERO(el_protocol_specific_info);
        COMPILER_64_ZERO(eli_protocol_specific_info);

        /** EL: Label Type=0, EXP Type=1, EXP=0, TTL Type=0, TTL=0 */
        egress_mpls_protocol_header_specific_information_get(0, 0, 1, 0, 0, 0, &el_protocol_specific_info);
        /** ELI: Label Type=1, Label=7, EXP Type=1, EXP=0, TTL Type=1, TTL=0 */
        egress_mpls_protocol_header_specific_information_get(1, 7, 1, 0, 1, 0, &eli_protocol_specific_info);

        egress_mpls_protocol_specific_data_cw_build(unit, value[0], protocol_specific_data_header_0);
        *protocol_specific_data_header_1 = el_protocol_specific_info;
        *protocol_specific_data_header_2 = eli_protocol_specific_info;
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_CW_FL_PWE)
    {
        egress_mpls_protocol_specific_data_cw_build(unit, value[0], protocol_specific_data_header_1);
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_CW_ESI_EVPN)
    {
        COMPILER_UINT64 esi_protocol_specific_info;

        COMPILER_64_ZERO(esi_protocol_specific_info);

        /** ESI: Label Type=2, EXP Type=1, EXP=0, TTL Type=1, TTL=1 */
        egress_mpls_protocol_header_specific_information_get(2, 0, 1, 0, 0, 1, &esi_protocol_specific_info);

        egress_mpls_protocol_specific_data_cw_build(unit, value[0], protocol_specific_data_header_1);
        *protocol_specific_data_header_2 = esi_protocol_specific_info;
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_CW_PWE)
    {
        egress_mpls_protocol_specific_data_cw_build(unit, value[0], protocol_specific_data_header_2);
    }
    else if (additional_headers_identifier == ADDITIONAL_HEADERS_VXLAN_UDP ||
             additional_headers_identifier == ADDITIONAL_HEADERS_VXLAN_GPE_UDP ||
             (additional_headers_identifier >= ADDITIONAL_HEADERS_UDP_USER_DEFINED(1) &&
              additional_headers_identifier <= ADDITIONAL_HEADERS_UDP_USER_DEFINED(7)))
    {
        /** It's expected the data is constructed already.*/
        COMPILER_64_SET(*protocol_specific_data_header_0, value[1], value[0]);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Construct data for the identifier(%d) is not support now\n",
                     additional_headers_identifier);
    }

    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_set
                    (unit, additional_headers_identifier, &profile_properties));

exit:
    SHR_FUNC_EXIT;
}

/** see h file */
shr_error_e
egress_additional_headers_profile_free(
    int unit,
    int lif_additional_headers_profile,
    dnx_ahs_namespace_e ahs_namespace,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info)
{
    int nof_ahp_map_entries, idx;
    int ah_profile;
    uint8 is_last;
    dnx_additional_headers_profile_map_t profile_map_entry;
    dnx_additional_headers_info_t additional_headers_info;
    const dnx_additional_headers_profile_map_t *tmp_map_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_EXIT();
    }

    sal_memset(headers_profile_mngr_info, 0, sizeof(dnx_egress_headers_profile_mngr_info_t));

    /** Look for the map data with the given identifier.*/
    sal_memset(&additional_headers_info, 0, sizeof(additional_headers_info));
    nof_ahp_map_entries = sizeof(additional_headers_profile_config_map) / sizeof(dnx_additional_headers_profile_map_t);
    for (idx = 0; idx < nof_ahp_map_entries; idx++)
    {
        tmp_map_entry = &(additional_headers_profile_config_map[idx]);
        if (tmp_map_entry->lif_additional_headers_profile == lif_additional_headers_profile &&
            tmp_map_entry->additional_headers_stack_namespace == ahs_namespace)
        {
            sal_memcpy(&profile_map_entry, tmp_map_entry, sizeof(profile_map_entry));

            additional_headers_info.additional_headers_profile_identifier =
                tmp_map_entry->additional_headers_profile_identifier;

            /** Get the profile with the data*/
            SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_profile.profile_get
                            (unit, &additional_headers_info, &ah_profile));

            /** Free the additional_headers_profile. */
            SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_profile.free_single(unit, ah_profile, &is_last));

            headers_profile_mngr_info->is_old_profile_last_referred = is_last;
            headers_profile_mngr_info->old_additional_headers_profile = ah_profile;

            /** Check if the map table need to clear too. */
            SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_profile_map.profile_get
                            (unit, &profile_map_entry, &ah_profile));

            SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_profile_map.free_single(unit, ah_profile, &is_last));

            headers_profile_mngr_info->is_old_profile_map_last_referred = is_last;
            if (is_last)
            {
                sal_memcpy(&(headers_profile_mngr_info->old_profile_map), &profile_map_entry,
                           sizeof(profile_map_entry));
            }

            /** Suppose there is only one entry currently*/
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate and configure all the ETPP additional headers profiles used by the
 *          lif additional headers profile
 * 
 * \param [in] unit - Relevant unit.
 * \param [in] lif_additional_headers_profile - lif additional_headers_profile.
 * \param [in] ahs_namespace - namespace of the lif additional_headers_profile.
 * \param [out] headers_profile_mngr_info - additional headers profile management info.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_alloc_internal(
    int unit,
    int lif_additional_headers_profile,
    dnx_ahs_namespace_e ahs_namespace,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info)
{
    int nof_ahp_map_entries, idx;
    uint8 is_first;
    int ah_prfoile, dummy_profile;
    dnx_additional_headers_profile_map_t profile_map_entry;
    dnx_additional_headers_info_t additional_headers_info;
    const dnx_additional_headers_profile_map_t *tmp_map_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&additional_headers_info, 0, sizeof(additional_headers_info));

    /** Look for all the map entries with the given identifier.*/
    nof_ahp_map_entries = sizeof(additional_headers_profile_config_map) / sizeof(dnx_additional_headers_profile_map_t);
    for (idx = 0; idx < nof_ahp_map_entries; idx++)
    {
        tmp_map_entry = &(additional_headers_profile_config_map[idx]);
        if (tmp_map_entry->lif_additional_headers_profile == lif_additional_headers_profile &&
            tmp_map_entry->additional_headers_stack_namespace == ahs_namespace)
        {
            sal_memcpy(&profile_map_entry, tmp_map_entry, sizeof(profile_map_entry));

            additional_headers_info.additional_headers_profile_identifier =
                tmp_map_entry->additional_headers_profile_identifier;

            /** Alloc an additional_headers_profile with the identifier. */
            SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_profile.allocate_single
                            (unit, 0, &additional_headers_info, NULL, &ah_prfoile, &is_first));

            headers_profile_mngr_info->is_new_profile_first_referred = is_first;
            headers_profile_mngr_info->new_additional_headers_profile = ah_prfoile;

            /** Check if the map table need to configure. */
            SHR_IF_ERR_EXIT(algo_tunnel_db.additional_headers_profile_map.allocate_single
                            (unit, 0, &profile_map_entry, NULL, &dummy_profile, &is_first));

            headers_profile_mngr_info->is_new_profile_map_first_referred = is_first;
            if (is_first)
            {
                sal_memcpy(&(headers_profile_mngr_info->new_profile_map), &profile_map_entry,
                           sizeof(profile_map_entry));
            }

            /** Suppose there is only one entry currently*/
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see h file */
shr_error_e
egress_additional_headers_profile_alloc(
    int unit,
    uint8 is_replace,
    int lif_additional_headers_profile_old,
    int lif_additional_headers_profile,
    dnx_ahs_namespace_e ahs_namespace,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_EXIT();
    }

    if (is_replace && lif_additional_headers_profile_old != lif_additional_headers_profile)
    {
        /** Free the old profile first. */
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_free
                        (unit, lif_additional_headers_profile_old, ahs_namespace, headers_profile_mngr_info));
    }
    else
    {
        sal_memset(headers_profile_mngr_info, 0, sizeof(dnx_egress_headers_profile_mngr_info_t));
    }

    if (!is_replace || (lif_additional_headers_profile_old != lif_additional_headers_profile))
    {
        /** Alloc a new profile. */
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_alloc_internal
                        (unit, lif_additional_headers_profile, ahs_namespace, headers_profile_mngr_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see h file */
shr_error_e
egress_additional_headers_profile_first_set(
    int unit,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info)
{
    dnx_additional_headers_profile_map_t *profile_map_entry = &(headers_profile_mngr_info->new_profile_map);

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_EXIT();
    }

    /** Check if the profile properties need to set first.*/
    if (headers_profile_mngr_info->is_new_profile_first_referred)
    {
        int properties_idx;
        dnx_additional_headers_profile_properties_t profile_properties;

        /** Look for the index of the identifier. */
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_idx_get
                        (unit, profile_map_entry->additional_headers_profile_identifier, &properties_idx));

        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_sw_get(unit, properties_idx, &profile_properties));

        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_hw_set
                        (unit, headers_profile_mngr_info->new_additional_headers_profile, &profile_properties));
    }

    /** Check if the profile map need to set then.*/
    if (headers_profile_mngr_info->is_new_profile_map_first_referred)
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_map_table_set
                        (unit, profile_map_entry->lif_additional_headers_profile,
                         profile_map_entry->ctx_additional_headers_profile,
                         headers_profile_mngr_info->new_additional_headers_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see h file */
shr_error_e
egress_additional_headers_profile_last_clear(
    int unit,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info)
{
    dnx_additional_headers_profile_map_t *profile_map_old = &(headers_profile_mngr_info->old_profile_map);
    dnx_additional_headers_profile_map_t *profile_map_new = &(headers_profile_mngr_info->new_profile_map);

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_EXIT();
    }

    /*
     * Check if the profile map need to clear first.
     * Note: if the old profile value is the same as the new profile, the old profile properties 
     * should be overridden in configuring the new profile.
     */
    if (headers_profile_mngr_info->is_old_profile_map_last_referred &&
        headers_profile_mngr_info->old_additional_headers_profile !=
        headers_profile_mngr_info->new_additional_headers_profile)
    {
        /** Clear the map table and properties table.*/
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_hw_set
                        (unit, headers_profile_mngr_info->old_additional_headers_profile, NULL));
    }
    /** Check if the profile properties need to clear then similar to the profile properties table.*/
    if (headers_profile_mngr_info->is_old_profile_last_referred &&
        (profile_map_old->lif_additional_headers_profile != profile_map_new->lif_additional_headers_profile ||
         profile_map_old->ctx_additional_headers_profile != profile_map_new->ctx_additional_headers_profile))
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_map_table_set
                        (unit, profile_map_old->lif_additional_headers_profile,
                         profile_map_old->ctx_additional_headers_profile, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See h file*/
shr_error_e
egress_additional_headers_profile_init(
    int unit)
{
    dnx_egress_headers_profile_mngr_info_t headers_profile_mngr_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize the properties with default values
     */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_init(unit));

    /*
     * Alloc the default profiles in each namespace in advance,
     * This helps to avoid allocating the default profile in each Etps related API.
     * Currently, we alloc the general default profile only.
     */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_alloc
                    (unit, 0, 0, 0, AHS_NAMESPACE_GENERAL, &headers_profile_mngr_info));

    /** Set the hw table. */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_first_set(unit, &headers_profile_mngr_info));
exit:
    SHR_FUNC_EXIT;
}

/** Update Special label value for RFC8321. Details at .h file */
shr_error_e
egress_additional_headers_rfc8321_special_label_set(
    int unit,
    int special_label_value)
{
    uint32 value[2] = { 0 };
    int header_to_update = 2;
    SHR_FUNC_INIT_VARS(unit);

    value[0] = special_label_value;
    SHR_IF_ERR_EXIT(egress_additional_headers_specific_data_set
                    (unit, ADDITIONAL_HEADERS_IOAM_FL_FLI, value, header_to_update));

    if (dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_specific_data_set
                        (unit, ADDITIONAL_HEADERS_IOAM_CW_FL_FLI, value, header_to_update));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
egress_additional_headers_mpls_control_word_set(
    int unit,
    int cw)
{
    int idx, nof_identifiers;
    uint32 additional_headers_cw_identifiers[] = { ADDITIONAL_HEADERS_CW_FL_ESI_EVPN, ADDITIONAL_HEADERS_CW_EL_ELI_PWE,
        ADDITIONAL_HEADERS_CW_FL_PWE, ADDITIONAL_HEADERS_CW_ESI_EVPN, ADDITIONAL_HEADERS_CW_PWE
    };
    int header_to_update = -1;
    uint32 value[2] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    value[0] = cw;
    nof_identifiers = sizeof(additional_headers_cw_identifiers) / sizeof(uint32);
    for (idx = 0; idx < nof_identifiers; idx++)
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_specific_data_set
                        (unit, additional_headers_cw_identifiers[idx], value, header_to_update));
    }

    if (dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        header_to_update = 0;
        SHR_IF_ERR_EXIT(egress_additional_headers_specific_data_set
                        (unit, ADDITIONAL_HEADERS_IOAM_CW_FL_FLI, value, header_to_update));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_header_map_tables_configuration_udp_ports_clear(
    int unit,
    uint32 udp_ports_profile)
{
    int idx;
    int additional_headers_identifier;
    dnx_additional_headers_profile_properties_t profile_properties;

    SHR_FUNC_INIT_VARS(unit);

    additional_headers_identifier = ADDITIONAL_HEADERS_UDP_USER_DEFINED(udp_ports_profile);

    /** Look for the index of the identifier. */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_idx_get(unit, additional_headers_identifier, &idx));

    sal_memcpy(&profile_properties, &(additional_headers_map[idx]), sizeof(profile_properties));

    /** Reset the profile propety table with the initial value. */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_set
                    (unit, additional_headers_identifier, &profile_properties));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_header_map_tables_configuration_udp_ports_update(
    int unit,
    uint32 udp_src_port,
    uint32 udp_dest_port,
    uint32 udp_ports_profile)
{
    uint32 tmp;
    uint32 protocol_specific_info[2] = { 0 };
    uint32 header_profile_identifier;
    int header_to_update = -1;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * By setting TRUE, UDP destination port is set by protocol specific information.
     */
    tmp = TRUE;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT_TYPE, &tmp, protocol_specific_info));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, DBAL_FIELD_SOURCE_PORT_TYPE, &tmp,
                     protocol_specific_info));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT, &udp_dest_port,
                                                    protocol_specific_info));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_SOURCE_PORT, &udp_src_port, protocol_specific_info));

    header_profile_identifier = ADDITIONAL_HEADERS_UDP_USER_DEFINED(udp_ports_profile);
    SHR_IF_ERR_EXIT(egress_additional_headers_specific_data_set
                    (unit, header_profile_identifier, protocol_specific_info, header_to_update));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_headers_map_tables_configuration_vxlan_udp_dest_update(
    int unit,
    uint32 udp_dest_port,
    int additional_header_profile)
{
    int idx;
    uint32 tmp;
    uint32 protocol_specific_info[2] = { 0 };
    int additional_headers_identifier = additional_header_profile;
    dnx_additional_headers_profile_properties_t profile_properties;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Validate it's vxlan or vxlan gpe
     */
    if ((additional_header_profile != DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN)
        && (additional_header_profile != DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE))
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "Only vxlan or vxlan gpe are expected to have their udp dest port updated. ");
    }

    /*
     * additional header profile identifier: vxlan or vxlan gpe
     */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_idx_get(unit, additional_headers_identifier, &idx));

    /** Get the initial values. */
    if (dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_sw_get(unit, idx, &profile_properties));
    }
    else
    {
        uint32 entry_handle_id;
        sal_memcpy(&profile_properties, &(additional_headers_map[idx]), sizeof(profile_properties));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PROFILE,
                                   additional_header_profile);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, 1,
                                       protocol_specific_info);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * update specific information with: destination port from next protocol: destination port type = 0
     */
    protocol_specific_info[0] = COMPILER_64_LO(profile_properties.additional_header_1_protocol_specific_data);
    protocol_specific_info[1] = COMPILER_64_HI(profile_properties.additional_header_1_protocol_specific_data);

    tmp = FALSE;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT_TYPE, &tmp, protocol_specific_info));

    COMPILER_64_SET(profile_properties.additional_header_1_protocol_specific_data,
                    protocol_specific_info[1], protocol_specific_info[0]);
    profile_properties.additional_header_1_start_current_next_protocol = udp_dest_port;

    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_set
                    (unit, additional_headers_identifier, &profile_properties));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_headers_map_tables_configuration_geneve_udp_dest_update(
    int unit,
    uint32 udp_dest_port,
    int additional_header_profile)
{
    int idx;
    uint32 tmp;
    uint32 protocol_specific_info[2] = { 0 };
    int additional_headers_identifier = additional_header_profile;
    dnx_additional_headers_profile_properties_t profile_properties;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Validate it's vxlan or vxlan gpe
     */
    if (additional_header_profile != DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GENEVE)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "Only geneve is expected to have their udp dest port updated.");
    }

    /*
     * additional header profile identifier: vxlan or vxlan gpe
     */
    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_idx_get(unit, additional_headers_identifier, &idx));

    /** Get the initial values. */
    if (dnx_data_pp.ETPP.dynamic_additional_headers_profile_get(unit))
    {
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_sw_get(unit, idx, &profile_properties));
    }
    else
    {
        uint32 entry_handle_id;
        sal_memcpy(&profile_properties, &(additional_headers_map[idx]), sizeof(profile_properties));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PROFILE,
                                   additional_header_profile);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, 1,
                                       protocol_specific_info);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * update specific information with: destination port from next protocol: destination port type = 0
     */

    protocol_specific_info[0] = COMPILER_64_LO(profile_properties.additional_header_1_protocol_specific_data);
    protocol_specific_info[1] = COMPILER_64_HI(profile_properties.additional_header_1_protocol_specific_data);

    tmp = FALSE;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT_TYPE, &tmp, protocol_specific_info));

    COMPILER_64_SET(profile_properties.additional_header_1_protocol_specific_data,
                    protocol_specific_info[1], protocol_specific_info[0]);
    profile_properties.additional_header_1_start_current_next_protocol = udp_dest_port;

    SHR_IF_ERR_EXIT(egress_additional_headers_profile_properties_set
                    (unit, additional_headers_identifier, &profile_properties));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
