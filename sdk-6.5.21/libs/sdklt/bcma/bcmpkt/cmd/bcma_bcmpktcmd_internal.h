/*! \file bcma_bcmpktcmd_internal.h
 *
 * Definitions intended for bcmpktcmd internal use only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_INTERNAL_H
#define BCMA_BCMPKTCMD_INTERNAL_H

#include <bcma/cli/bcma_cli.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <shr/shr_bitop.h>
#include <shr/shr_pb.h>
#include <sal/sal_time.h>
#include <sal/sal_mutex.h>
#include <bcmpkt/bcmpkt_net.h>

/*! Maximum port number. */
#ifndef BCMDRD_CONFIG_MAX_PORTS
#define BCMA_PORT_MAP_PORT_MAX  255
#else
#define BCMA_PORT_MAP_PORT_MAX  BCMDRD_CONFIG_MAX_PORTS
#endif

/*! Maximum default packet size. */
#define MAX_FRAME_SIZE_DEF      (1536)

/*! Maximum jumbo frame size supported. */
#define MAX_JUMBO_FRAME_SIZE    (9216)

/*! Default netif MAC address. */
#define BCMA_BCMPKT_DEF_NETIF_MAC {0x00, 0xbc, 0x00, 0x00, 0x00, 0x00}

#define ENET_TYPE_NONE          0
#define ENET_TYPE_ETHERNET_II   1
#define ENET_TYPE_8023_RAW      2
#define ENET_TYPE_SNAP          3

#define ENET_MAC_SIZE           6
#define ENET_FCS_SIZE           4
#define ENET_MIN_PKT_SIZE       60 /* Does not include CRC */
#define ENET_TPID               0x8100
#define ENET_ETHERTYPE          0xFFFF
#define ENET_TAG_SIZE           4

#define BCMPKT_TXPMD_START_IHEADER        2
#define BCMPKT_TXPMD_HEADER_TYPE_FROM_CPU 1

#define BCMPKT_TXPMD_START_HIGIG  3

#define BCMPKT_LBHDR_START_CODE 0xFB

/* Show Packet Data in Packet Watcher */
#define WATCHER_DEBUG_SHOW_PACKET_DATA  (0x1 << 0)
/* Show Packet Meta Data in Packet Watcher */
#define WATCHER_DEBUG_SHOW_META_DATA    (0x1 << 1)
/* Show RX Reason in Packet Watcher */
#define WATCHER_DEBUG_SHOW_RX_REASON    (0x1 << 2)
/* Loopback Packet Data to TX in Packet Watcher */
#define WATCHER_DEBUG_LPBK_PACKET       (0x1 << 3)
/* Show RX packet data Rate in Packet Watcher */
#define WATCHER_DEBUG_SHOW_RX_RATE      (0x1 << 4)
/* Loopback Packet with time */
#define WATCHER_DEBUG_LDT               (0x1 << 5)

/* CLI parameter not configured flag */
#define PX_NOT_CONFIGURED       -1

#define NETIF_DEFID             1
#define RX_DMA_CHAN_DEFID       1

#define BCMPKT_API_HANDLER_CHECK(_h, _ls) \
    if (_h == NULL || _h->initialized != 1) {\
        LOG_ERROR(BSL_LS_APPL_PKTDEV,\
                  (BSL_META("%s handler was not found!\n"),\
                    #_h));\
        return -1;\
    }

typedef struct bcma_txpmd_s {
    bcmdrd_dev_type_t dev_type;
    uint32_t *data;
} bcma_txpmd_t;

typedef struct bcma_lbhdr_s {
    bcmdrd_dev_type_t dev_type;
    uint32_t *data;
} bcma_lbhdr_t;

typedef struct bcma_hg3_s {
    bcmlrd_variant_t variant;
    uint32_t *data;
} bcma_hg3_t;

typedef struct bcma_generic_loopback_s {
    bcmlrd_variant_t variant;
    uint32_t *data;
} bcma_generic_loopback_t;

typedef struct higig_symbl_info_s {
    struct higig_symbl_info_s *next;

   int unit;
   uint32_t sflags; /*! save symbol flags*/
   char *name;
   uint16_t minbit;
   uint16_t maxbit;
} higig_symbl_info_t;

typedef struct bcma_pbmp_s {
    SHR_BITDCLNAME(pbits, BCMA_PORT_MAP_PORT_MAX);
} bcma_pbmp_t;

/*!
 * Packet generation parameters.
 * len is used for clear file/data (len > 64).
 */
typedef struct bcma_bcmpkt_pktgen_cfg_s {
    uint32_t pkttype;    /*! NONE, ETH2, 802.3RAW, SNAP */
    uint32_t len;        /* packet length */
    uint16_t ethertype;  /* EtherType */

    uint16_t tpid;
    bool untagged;
    uint8_t dei;
    uint8_t pcp;
    uint16_t vlan;        /* VLAN ID */

    uint32_t pattern;    /* Specific 32-bit data pattern */
    uint32_t pat_inc;    /*Value by which each word of the data
                         pattern is incremented*/
    bool pat_random; /* use random pattern */
    bool per_port_smac; /*different source mac for each source port*/
    shr_mac_t smac;       /* source MAC */
    uint32_t smac_inc;   /* source MAC increment value */

    shr_mac_t dmac;       /* destination MAC */
    uint32_t dmac_inc;   /* destination MAC increment value */

} bcma_bcmpkt_pktgen_cfg_t;

typedef struct watcher_data_s {
    struct watcher_data_s *next;
    int netif_id;
    uint32_t debug_mode;
    /*! For RX rate debug. */
    uint32_t rx_packets;
    sal_usecs_t start_time;
    /*! Terminate the packets from the netif. */
    int term_netif;
    /*! Terminate the packets from the VLAN. */
    int term_vlan;
    /*! Terminate the packets with the RXPMD MATCH_ID. */
    int term_match_id;
    /*! Mutex lock for printing debug info. */
    sal_mutex_t pb_mutex;
} watcher_data_t;

/*!
 * \brief Packet watcher function.
 *
 * Packet watcher.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network device index.
 * \param [in] packet Packet structure.
 * \param [in] cookie Not used here.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_UNIT Invalid unit number
 * \retval SHR_E_PARAM A parameter was invalid
 */
extern int
bcma_bcmpkt_watcher(int unit, int netif_id, bcmpkt_packet_t *packet,
                    void *cookie);

extern int
bcma_bcmpkt_parse_data_add(void);

extern int
bcma_bcmpkt_load_data_from_istr(char *istr, uint8_t *buf, uint32_t buf_size,
                                uint32_t *data_size);

extern void
bcma_bcmpkt_macaddr_inc(shr_mac_t macaddr, int amount);

extern int
bcma_bcmpkt_packet_payload_fill(bcma_bcmpkt_pktgen_cfg_t *cfg,
                                bcmpkt_data_buf_t *dbuf);

extern int
bcma_bcmpkt_packet_generate(bcma_bcmpkt_pktgen_cfg_t *cfg,
                            bcmpkt_data_buf_t *dbuf);

extern int
bcma_bcmpkt_load_data_from_file(char *fname, bcmpkt_data_buf_t *dbuf);

extern int
bcma_bcmpkt_lmatch_check(const char *dst, const char *src, int size);

extern void
bcma_bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size);

extern void
bcma_bcmpktcmd_watcher_destroy(int unit, int netif_id);

extern int
bcma_bcmpkt_txpmd_init(uint32_t dev_type, uint32_t *txpmd);

extern int
bcma_bcmpkt_lbhdr_init(uint32_t dev_type, uint32_t *lbhdr);

extern int
bcma_bcmpkt_hg3hdr_init(uint32_t variant, uint32_t *hg3hdr);

extern int
bcma_bcmpkt_generic_loopback_init(uint32_t variant, uint32_t *generic_loopback);

extern void
bcma_bcmpkt_flag_set( uint32_t  *flags, uint32_t this_flag, int do_set);

extern bool
bcma_bcmpkt_flag_status_get(const uint32_t flags, uint32_t this_flag);

extern int
bcma_bcmpkt_netif_defid_get(int unit);

extern int
bcma_bcmpkt_netif_defid_set(int unit, int netif_id);

/*!
 * \brief DMA channel mapping function.
 *
 * Mapping CPU CoS queues into receive packet DMA channel.
 *
 * \param [in] unit Switch unit number.
 * \param [in] chan_id Receive DMA channel ID.
 * \param [in] queue_bmp CPU CoS queues bitmap.
 * \param [in] num_queues Number of CPU CoS queues.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_chan_qmap_set(int unit, int chan_id, SHR_BITDCL *queue_bmp,
                          uint32_t num_queues);
/*!
 * \brief Set output file name.
 *
 * Pass the output file name for logging RX watcher received packets.
 *
 * \param [in] fname Output file name.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_rx_watch_output_file_set(const char *fname);

/*!
 * \brief Selection of data logging.
 *
 * Selection of RX watcher received pacekts logging and display.
 *
 * \param [in] file_en Enable or disable logging to file.
 * \param [in] console_en Enable or disable console display.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_rx_watch_output_enable(int file_en, int console_en);

#endif /* BCMA_BCMPKTCMD_INTERNAL_H */
