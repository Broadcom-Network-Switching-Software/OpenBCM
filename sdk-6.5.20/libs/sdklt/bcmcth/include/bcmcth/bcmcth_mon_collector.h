/*! \file bcmcth_mon_collector.h
 *
 * Collector custom handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_COLLECTOR_H
#define BCMCTH_MON_COLLECTOR_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util_pack.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmltd/chip/bcmltd_mon_constants.h>

/*! Maximum Field count of collector tables.*/
#define MON_COLLECTOR_FIELD_ID_MAX 32

/*! The data structure for MON_EXPORT_PROFILEt entry. */
typedef struct bcmcth_mon_export_profile_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_COLLECTOR_FIELD_ID_MAX);

    /*! Index into the MON_EXPORT_PROFILE table.  */
    uint32_t export_profile_id;

    /*! Wire format to be used for data export. */
    bcmltd_common_wire_format_t_t wire_format;

    /*! Export interval in micro seconds. */
    uint32_t export_interval;

    /*! Speicifes the export packet length indicator. */
    bcmltd_common_export_packet_len_indicator_t_t packet_len_indicator;

    /*! Maximum length of export packet sent to collector,
     *  valid when PACKET_LEN_INDICATOR=MAX_PKT_LENGTH.
     */
    uint32_t max_packet_length;

    /*! Number of records to be sent in one packet,
     *  valid when PACKET_LEN_INDICATOR=NUM_RECORDS.
     */
    int num_records;

} bcmcth_mon_export_profile_t;

/*! The data structure for MON_COLLECTOR_IPV4t entry. */
typedef struct bcmcth_mon_collector_ipv4_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_COLLECTOR_FIELD_ID_MAX);

    /*! Index into the MON_COLLECTOR_IPV4 table. */
    uint32_t collector_ipv4_id;

    /*! VLAN tag type. */
    uint8_t tag_type;

    /*! Destination MAC address. */
    shr_mac_t dst_mac;

    /*!Source MAC address. */
    shr_mac_t src_mac;

    /*! Outer VLAN tag. Applicable only when single or double tagged. */
    shr_util_pack_vlan_tag_t outer_vlan_tag;

    /*! Inner VLAN ID. Applicable only when double tagged. */
    shr_util_pack_vlan_tag_t inner_vlan_tag;

    /*! Source IPv4 address. */
    shr_ip_t src_ip;

    /*! Destination IPv4 address. */
    shr_ip_t dst_ip;

    /*! IPv4 type of service. */
    uint8_t tos;

    /*! IPv4 time to live. */
    uint8_t ttl;

    /*! Layer 4 UDP source port. */
    uint16_t src_port;

    /*! Layer 4 UDP dst port. */
    uint16_t dst_port;

    /*! Enable UDP checksum calculation for the packets.*/
    bool udp_chksum;

    /*! System ID length. */
    uint8_t system_id_len;

    /*! Identification of the system being monitored. */
    uint8_t system_id[PROTOBUF_SYSTEM_ID_MAX_LENGTH];

    /*! Component ID of the protobuf. */
    uint32_t component_id;

    /*! IPFIX observation domain. */
    uint32_t observation_domain;

    /*! IPFIX enterprise number. */
    uint32_t enterprise_number;
} bcmcth_mon_collector_ipv4_t;

/*! The data structure for MON_COLLECTOR_IPV6t entry. */
typedef struct bcmcth_mon_collector_ipv6_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_COLLECTOR_FIELD_ID_MAX);

    /*! Index into the MON_COLLECTOR_IPV6 table. */
    uint32_t collector_ipv6_id;

    /*! VLAN tag type. */
    uint8_t tag_type;

    /*! Destination MAC address. */
    shr_mac_t dst_mac;

    /*!Source MAC address. */
    shr_mac_t src_mac;

    /*! Outer VLAN tag. Applicable only when single or double tagged. */
    shr_util_pack_vlan_tag_t outer_vlan_tag;

    /*! Inner VLAN ID. Applicable only when double tagged. */
    shr_util_pack_vlan_tag_t inner_vlan_tag;

    /*! Outer source IPv6 address. */
    shr_ip6_t src_ip6_addr;

    /*! Outer destination IPv6 address. */
    shr_ip6_t dst_ip6_addr;

    /*! IPv6 type of service. */
    uint8_t tos;

    /*! IPv6 time to live. */
    uint8_t ttl;

    /*! Layer 4 UDP source port. */
    uint16_t src_port;

    /*! Layer 4 UDP dst port. */
    uint16_t dst_port;

    /*! Enable UDP checksum calculation for the packets.*/
    bool udp_chksum;

    /*! Ipv6 flow label. */
    uint32_t flow_label;

    /*! System ID length */
    uint8_t system_id_len;

    /*! Identification of the system being monitored. */
    uint8_t system_id[PROTOBUF_SYSTEM_ID_MAX_LENGTH];

    /*! Component ID of the protobuf. */
    uint32_t component_id;

    /*! IPFIX observation domain. */
    uint32_t observation_domain;

    /*! IPFIX enterprise number. */
    uint32_t enterprise_number;
} bcmcth_mon_collector_ipv6_t;


/*!
 * \brief Register IMM callbacks for collector.
 *
 * The MON collector custom table handling is done via the IMM component.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mon_collector_imm_register(int unit);

/*!
 * \brief Get IPv4 collector entry.
 *
 * Get the IPv4 collector details pointed by the collector id.
 *
 * \param [in] unit Unit number.
 * \param [in] collector_id IPv4 collector id.
 * \param [out] entry Structure containing the IPv4 collector details.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error
 */
int
bcmcth_mon_collector_ipv4_entry_get(int unit,
                                    uint32_t collector_id,
                                    bcmcth_mon_collector_ipv4_t *entry);

/*!
 * \brief Get IPv6 collector entry.
 *
 * Get the IPv6 collector details pointed by the collector id.
 *
 * \param [in] unit Unit number.
 * \param [in] collector_id IPv6 collector id.
 * \param [out] entry Structure containing the Ipv6 collector details.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error
 */
int
bcmcth_mon_collector_ipv6_entry_get(int unit,
                                    uint32_t collector_id,
                                    bcmcth_mon_collector_ipv6_t *entry);

/*!
 * \brief Get export profile entry.
 *
 * Get the export profile details pointed by the export profile id.
 *
 * \param [in] unit Unit number.
 * \param [in] export_profile_id Export profile Id.
 * \param [out] entry Structure containing the export profile details.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error
 */
int
bcmcth_mon_export_profile_entry_get(int unit,
                                    uint32_t export_profile_id,
                                    bcmcth_mon_export_profile_t *entry);
#endif /* BCMCTH_MON_COLLECTOR_H */
