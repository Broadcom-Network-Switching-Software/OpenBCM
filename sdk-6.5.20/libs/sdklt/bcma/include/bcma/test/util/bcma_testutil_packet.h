/*! \file bcma_testutil_packet.h
 *
 * packet utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_PACKET_H
#define BCMA_TESTUTIL_PACKET_H

#include <sal/sal_types.h>
#include <bcmpkt/bcmpkt_buf.h>

/*! Number of bytes for mac address. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR 6
/*! Number of bytes for IPV4 address. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR 4
/*! Number of bytes for IPV4 address. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_ADDR 16
/*! Number of bytes for L2 header. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR 18
/*! Number of bytes for IPV4 header. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR 20
/*! Number of bytes for IPV6 header. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR 40
/*! Number of bytes for sequence id. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_SEQ_ID 6
/*! Number of bytes for crc. */
#define BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC 4
/*! Number of bytes for crc table. */
#define BCMA_TESTUTIL_PACKET_NUM_ENTRIES_CRC_TABLE 256

/*! Attributes used for pkt generation */
typedef struct bcma_testutil_pkt_attr_s {
    /*! port to which is assigned to */
    int      port;
    /*! pkt id */
    uint32_t id;
    /*! pkt size */
    uint32_t size;
    /*! is L3 pkt */
    uint32_t l3_en;
    /*! is IPv6 pkt */
    uint32_t ipv6_en;
    /*! is MPLS pkt */
    uint32_t l3_mpls_en;
    /*! MACDA */
    uint8_t  mac_da[BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR];
    /*! MACSA */
    uint8_t  mac_sa[BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR];
    /*! VLAN_ID */
    uint32_t vlan_id;
    /*! pkt's 3 MPLS labels */
    uint32_t mpls_label[3];
    /*! IPv4/6 DA; ip_da[0] is used for IPv4 */
    uint32_t ip_da[4];
    /*! IPv4/6 SA; ip_sa[0] is used for IPv4 */
    uint32_t ip_sa[4];
    /*! pkt's TTL */
    uint32_t ttl;
    /*! pkt's TOS */
    uint32_t tos;
    /*! seed to generate random fields: e.g., payload */
    uint32_t seed;
    /*! is VXLAN pkt */
    uint32_t vxlan_en;
    /*! underlay(ul) MACDA */
    uint8_t  ul_mac_da[BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR];
    /*! ul MACSA */
    uint8_t  ul_mac_sa[BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR];
    /*! ul VLAN_ID */
    uint32_t ul_vlan_id;
    /*!  ul IPv4/6 DA; ip_da[0] is used for IPv4 */
    uint32_t ul_ip_da[4];
    /*! ul IPv4/6 SA; ip_sa[0] is used for IPv4 */
    uint32_t ul_ip_sa[4];
    /*!  VXLAN id */
    uint32_t vxlan_vnid;
} bcma_testutil_pkt_attr_t;

/*!
 * \brief crc table.
 */
uint32_t bcma_testutil_packet_crc_table[BCMA_TESTUTIL_PACKET_NUM_ENTRIES_CRC_TABLE];

/*!
 * \brief Init packet such as crc table.
 */
extern void
bcma_testutil_packet_init(void);

/*!
 * \brief Fill packet buffer.
 *
 * \param [out] buf packet buffer to be writed
 * \param [in] pkt_len packet length
 * \param [in] src_mac source mac address
 * \param [in] dst_mac destination mac address
 * \param [in] need_vlan whether to insert vlan tag
 * \param [in] vid vlan id if need_vlan is true
 * \param [in] ethertype payload ether type
 * \param [in] pattern payload pattern
 * \param [in] pattern_inc payload pattern increment
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_packet_fill(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          bool need_vlan, uint32_t vid, uint16_t ethertype,
                          uint32_t pattern, uint32_t pattern_inc);



/*!
 * \brief Fill packet buffer.
 *
 * \param [out] buf packet buffer to be writed
 * \param [in] pkt_len packet length
 * \param [in] src_mac source mac address
 * \param [in] dst_mac destination mac address
 * \param [in] need_vlan whether to insert vlan tag
 * \param [in] need_svtag whether to insert svtag tag
 * \param [in] svtag macs_sec_svtag if need_svtag is true
 * \param [in] vid vlan id if need_vlan is true
 * \param [in] ethertype payload ether type
 * \param [in] pattern payload pattern
 * \param [in] pattern_inc payload pattern increment
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_packet_fill_svtag(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          bool need_vlan, bool need_svtag, uint32_t svtag,
                           uint32_t vid, uint16_t ethertype,
                          uint32_t pattern, uint32_t pattern_inc);
/*!
 * \brief Fill l3 packet buffer.
 *
 * \param [out] buf packet buffer to be writed
 * \param [in] pkt_len packet length
 * \param [in] src_mac source mac address
 * \param [in] dst_mac destination mac address
 * \param [in] src_ip_addr source ip address
 * \param [in] dst_ip_addr destination ip address
 * \param [in] vid vlan id if need_vlan is true
 * \param [in] ttl Time to live
 * \param [in] tos Type of service
 * \param [in] pattern payload pattern
 * \param [in] pattern_inc payload pattern increment
 * \param [in] random random payload
 * \param [in] seed seed
 * \param [in] ipv6 is ipv6
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l3_packet_fill(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          const shr_ip6_t src_ip_addr, const shr_ip6_t dst_ip_addr,
                          uint32_t vid, uint8_t ttl, uint8_t tos,
                          uint32_t pattern, uint32_t pattern_inc, bool random,
                          int seed, bool ipv6);
/*!
 * \brief Fill l3 packet buffer.
 * \brief random priority assigned to each packet - used for tr 517 power test
 *
 * \param [out] buf packet buffer to be writed
 * \param [in] pkt_len packet length
 * \param [in] src_mac source mac address
 * \param [in] dst_mac destination mac address
 * \param [in] src_ip_addr source ip address
 * \param [in] dst_ip_addr destination ip address
 * \param [in] vid vlan id if need_vlan is true
 * \param [in] ttl Time to live
 * \param [in] tos Type of service
 * \param [in] pattern payload pattern
 * \param [in] pattern_inc payload pattern increment
 * \param [in] random random payload
 * \param [in] seed seed
 * \param [in] ipv6 is ipv6
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l3_packet_with_pri_fill(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          const shr_ip6_t src_ip_addr, const shr_ip6_t dst_ip_addr,
                          uint32_t vid, uint8_t ttl, uint8_t tos,
                          uint32_t pattern, uint32_t pattern_inc, bool random,
                          int seed, bool ipv6);


/*!
 * \brief Init packet device for testing
 *
 * \param [in] unit Unit id
 * \param [in] tx_chan TX channel id
 * \param [in] rx_chan RX channel id
 * \param [in] pkt_max_size Packet max size
 * \param [in] pkt_count Packet count
 * \param [out] ret_netif_id Returned created netif id
 * \param [out] ret_pktdev_id Returned packet device id
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_packet_dev_init(int unit, int tx_chan, int rx_chan,
                              int pkt_max_size, int pkt_count,
                              int *ret_netif_id, int* ret_pktdev_id);

/*!
 * \brief Cleanup packet device for testing
 *
 * \param [in] unit Unit id
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_packet_dev_cleanup(int unit);

/*!
 * \brief Fill MPLS packet buffer
 *
 * \param [in]  unit Unit id
 * \param [out] buf packet buffer to be populated
 * \param [in]  pkt_attr Pkt attributes
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_mpls_packet_fill(
    int unit,
    bcmpkt_data_buf_t *buf,
    bcma_testutil_pkt_attr_t pkt_attr);

/*!
 * \brief Fill VXLAN packet buffer
 *
 * \param [in]  unit Unit id
 * \param [out] buf packet buffer to be populated
 * \param [in]  pkt_attr Pkt attributes
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vxlan_packet_fill(
    int unit,
    bcmpkt_data_buf_t *buf,
    bcma_testutil_pkt_attr_t pkt_attr);


#endif /* BCMA_TESTUTIL_PACKET_H */
