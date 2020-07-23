/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bcm_xmod_api.h
 */

#ifndef _BCM_XMOD_API_H_
#define _BCM_XMOD_API_H_

#include "types.h"

/* XMOD API parameter types */
#define IN          /* SDK to FW */
#define OUT         /* FW to SDK */
#define INOUT       /* SDK to FW to SDK */

#define XMOD_BUFFER_MAX_LEN                         256

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a)    __attribute__ (_a)
#endif /* COMPILER_ATTRIBUTE */

typedef enum {
    XMOD_E_NONE_PROCESSING  = 1,
    XMOD_E_NONE             = 0,
    XMOD_E_INTERNAL         = -1,
    XMOD_E_MEMORY           = -2,
    XMOD_E_IO               = -3,
    XMOD_E_PARAM            = -4,
    XMOD_E_CORE             = -5,
    XMOD_E_PHY              = -6,
    XMOD_E_BUSY             = -7,
    XMOD_E_FAIL             = -8,
    XMOD_E_TIMEOUT          = -9,
    XMOD_E_RESOURCE         = -10,
    XMOD_E_CONFIG           = -11,
    XMOD_E_UNAVAIL          = -12,
    XMOD_E_INIT             = -13,
    XMOD_E_LIMIT            = -14           /* Must come last */
} xmod_error_t;

#define XMOD_ETHERNET_CMDS                  1
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_GET           0x00
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_SET           0x01
#define XMOD_PHY_INIT                               0x03
#define XMOD_PHY_RX_RESTART                         0x05
#define XMOD_PHY_POLARITY_GET                       0x06
#define XMOD_PHY_POLARITY_SET                       0x07
#define XMOD_PHY_TX_GET                             0x08
#define XMOD_PHY_TX_SET                             0x09
#define XMOD_PHY_MEDIA_TYPE_TX_GET                  0x0A
#define XMOD_PHY_TX_OVERRIDE_GET                    0x0C
#define XMOD_PHY_TX_OVERRIDE_SET                    0x0D
#define XMOD_PHY_RX_GET                             0x10
#define XMOD_PHY_RX_SET                             0x11
#define XMOD_PHY_RESET_GET                          0x12
#define XMOD_PHY_RESET_SET                          0x13
#define XMOD_PHY_POWER_GET                          0x14
#define XMOD_PHY_POWER_SET                          0x15
#define XMOD_PHY_TX_LANE_CONTROL_GET                0x16
#define XMOD_PHY_TX_LANE_CONTROL_SET                0x17
#define XMOD_PHY_RX_LANE_CONTROL_GET                0x18
#define XMOD_PHY_RX_LANE_CONTROL_SET                0x19
#define XMOD_PHY_FEC_ENABLE_GET                     0x1A
#define XMOD_PHY_FEC_ENABLE_SET                     0x1B
#define XMOD_PHY_INTERFACE_CONFIG_GET               0x1C
#define XMOD_PHY_INTERFACE_CONFIG_SET               0x1D
#define XMOD_PHY_CL72_GET                           0x1E
#define XMOD_PHY_CL72_SET                           0x1F
#define XMOD_PHY_CL72_STATUS_GET                    0x20
#define XMOD_PHY_AUTONEG_ABILITY_GET                0x22
#define XMOD_PHY_AUTONEG_ABILITY_SET                0x23
#define XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET         0x24
#define XMOD_PHY_AUTONEG_GET                        0x26
#define XMOD_PHY_AUTONEG_SET                        0x27
#define XMOD_PHY_AUTONEG_STATUS_GET                 0x28
#define XMOD_PHY_LOOPBACK_GET                       0x2A
#define XMOD_PHY_LOOPBACK_SET                       0x2B
#define XMOD_PHY_RX_PMD_LOCKED_GET                  0x2C
#define XMOD_PHY_LINK_STATUS_GET                    0x2E
#define XMOD_PHY_STATUS_DUMP                        0x30
#define XMOD_PHY_PCS_USERSPEED_GET                  0x32
#define XMOD_PHY_PCS_USERSPEED_SET                  0x33
#define XMOD_PORT_INIT                              0x35
#define XMOD_PHY_AN_TRY_ENABLE                      0x37
#define XMOD_PE_INIT                                0x39
#define XMOD_PE_STATUS_GET                          0x3a
#define XMOD_PE_PORT_STATS_GET                      0x3c
#define XMOD_PE_TAGID_SET                           0x3f
#define XMOD_PE_FW_VER_GET                          0x40
#define XMOD_ETHERNET_CMDS_COUNT                    0x42    /* LAST ENTRY */


#define WRITE_XMOD_ARG_BUFF(_xmodbuf, _argbuff, _arglen) \
    { \
	bcm_memcpy((void*)_xmodbuf, (void*)_argbuff, _arglen); \
	_xmodbuf+=_arglen; \
    }
#define READ_XMOD_ARG_BUFF(_xmodbuf, _argbuff, _arglen) \
    { \
	bcm_memcpy((void*)_argbuff, (void*)_xmodbuf, _arglen); \
	_xmodbuf+=_arglen; \
    }
#define XMOD_ARG_BUFF_ADJUST(_xmodbuf, _arglen) \
    { \
	_xmodbuf+=_arglen; \
    }

#define XMOD_CMD_MODE_CORE(cmd)  (cmd)
#define XMOD_CMD_MODE_ETH(cmd)   (0x100 | cmd)


/*!
 * @enum xmod_pe_mode_e
 * @brief port extender mode types
 */
typedef enum xmod_pe_mode_e {
    XMOD_PE_MODE_4_PORTS = 1,
    XMOD_PE_MODE_2_PORTS = 2,
} xmod_pe_mode_td;
typedef uint8 xmod_pe_mode_t;

/*!
 * @enum xmod_pe_tag_e
 * @brief port extender tag types
 */
typedef enum xmod_pe_tag_e {
    XMOD_PE_TAG_BR      = 1,
    XMOD_PE_TAG_VLAN    = 2,
} xmod_pe_tag_td;
typedef uint8 xmod_pe_tag_t;

/*!
 * @enum xmod_port_if_e
 * @brief port media type
 */
typedef enum xmod_port_if_e {
    XMOD_PORT_IF_NOCXN, /* No physical connection */
    XMOD_PORT_IF_NULL,  /* Pass-through connection without PHY */
    XMOD_PORT_IF_MII,
    XMOD_PORT_IF_GMII,
    XMOD_PORT_IF_SGMII,
    XMOD_PORT_IF_TBI,
    XMOD_PORT_IF_XGMII,
    XMOD_PORT_IF_RGMII,
    XMOD_PORT_IF_RvMII,
    XMOD_PORT_IF_SFI,
    XMOD_PORT_IF_XFI,
    XMOD_PORT_IF_KR,
    XMOD_PORT_IF_KR4,
    XMOD_PORT_IF_CR,
    XMOD_PORT_IF_CR4,
    XMOD_PORT_IF_XLAUI,
    XMOD_PORT_IF_SR,
    XMOD_PORT_IF_RXAUI,
    XMOD_PORT_IF_XAUI,
    XMOD_PORT_IF_SPAUI,
    XMOD_PORT_IF_QSGMII,
    XMOD_PORT_IF_ILKN,
    XMOD_PORT_IF_RCY,
    XMOD_PORT_IF_FAT_PIPE,
    XMOD_PORT_IF_CGMII,
    XMOD_PORT_IF_CAUI,
    XMOD_PORT_IF_LR,
    XMOD_PORT_IF_LR4,
    XMOD_PORT_IF_SR4,
    XMOD_PORT_IF_KX,
    XMOD_PORT_IF_ZR,
    XMOD_PORT_IF_SR10,
    XMOD_PORT_IF_OTL,
    XMOD_PORT_IF_CPU,
    XMOD_PORT_IF_OLP,
    XMOD_PORT_IF_OAMP,
    XMOD_PORT_IF_ERP,
    XMOD_PORT_IF_TM_INTERNAL_PKT,   
    XMOD_PORT_IF_SR2,
    XMOD_PORT_IF_KR2,
    XMOD_PORT_IF_CR2,
    XMOD_PORT_IF_XFI2,
    XMOD_PORT_IF_XLAUI2,
    XMOD_PORT_IF_COUNT /* last, please */
} xmod_port_if_td;
typedef uint8 xmod_port_if_t;

/*!
 * @struct xmod_pe_config_s
 * @brief PE initial configuration information
 */
typedef struct xmod_pe_config_s {
    xmod_pe_mode_t      mode;
    xmod_pe_tag_t       tag;
    uint8               pfc;
    uint8               line10g;
    uint16              tagid[4];
    uint8               lt_up_port;     /* link training */
    uint8               res0[3];
    uint8               lt_pe_port[4];  /* link training */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_config_t;

/*!
 * @struct xmod_pe_port_status_s
 * @brief PE port status information
 */
typedef struct xmod_pe_port_status_s {
    uint8             res;			  /* reserved */
    uint8             link_up;        /* link_up */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_port_status_t;

/*!
 * @struct xmod_pe_status_s
 * @brief PE port status information
 */
typedef struct xmod_pe_status_s {
    xmod_pe_port_status_t       up_port;        /* up stream port */
    xmod_pe_port_status_t       pe_port0;       /* port 0 extender */
    xmod_pe_port_status_t       pe_port1;       /* port 1 extender */
    xmod_pe_port_status_t       pe_port2;       /* port 2 extender */
    xmod_pe_port_status_t       pe_port3;       /* port 3 extender */
    xmod_port_if_t              up_port_mod;    /* up stream port module type */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_status_t;

/*!
 * @struct xmod_pe_port_stats_s
 * @brief PE port statistics information
 */
typedef struct xmod_pe_port_stats_s {
    uint32            port_dn_cnt;        /* port down counter */

    uint64            rpkt;               /* Receive frame/packet Counter */
    uint64            rbyt;               /* Receive Byte Counter */
    uint64            tpkt;               /* Transmit Packet/Frame Counter */
    uint64            tbyt;               /* Transmit Byte Counter */

    uint64            rfcs;               /* Receive FCS Error Frame Counter */
    uint64            rfrg;               /* Receive Fragment Counter */
    uint64            rjbr;               /* Receive Jabber Frame Counter */
    uint64            rovr;               /* Receive Oversized Frame Counter */
    uint64            rrpkt;              /* Receive RUNT Frame Counter */
    uint64            raln;               /* Receive Alignment Error Frame Counter */
    uint64            rerpkt;             /* Receive Code Error Frame Counter */
    uint64            rmtue;              /* Receive MTU Check Error Frame Counter */
    uint64            rprm;               /* Receive Promiscuous Frame Counter */
    uint64            rschcrc;            /* Receive SCH CRC Error */
    uint64            rtrfu;              /* Receive Truncated Frame Counter (due to RX FIFO full) */
    uint64            rfcr;               /* Receive False Carrier Counter */
    uint64            rxpf;               /* Receive PAUSE Frame Counter */
    uint64            rflr;               /* Receive Length Out of Range Frame Counter */

    uint64            tfcs;               /* Transmit FCS Error Counter */
    uint64            tfrg;               /* Transmit Fragment Counter */
    uint64            tjbr;               /* Transmit Jabber Counter */
    uint64            tncl;               /* Transmit Total Collision Counter */
    uint64            tovr;               /* Transmit Oversize Packet Counter */
    uint64            trpkt;              /* Transmit RUNT Frame Counter */
    uint64            txpf;               /* Transmit Pause Control Frame Counter */

} COMPILER_ATTRIBUTE((packed)) xmod_pe_port_stats_t;

/*!
 * @struct xmod_pe_tagid_s
 * @brief PE port E-channel ID information
 */
typedef struct xmod_pe_tagid_s {
    uint16            tagid0;    /* port 0 extender channel id */
    uint16            tagid1;    /* port 1 extender channel id */
    uint16            tagid2;    /* port 2 extender channel id */
    uint16            tagid3;    /* port 3 extender channel id */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_tagid_t;

/*!
 * @struct xmod_pe_fw_ver_s
 * @brief FW ver information
 */
typedef struct xmod_pe_fw_ver_s {
    uint16            major;    /* major version */
    uint16            minor;    /* minor version */
    uint16            build;    /* build version */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_fw_ver_t;

/* XMOD_ETHERNET_CMDS & LENGTHS */
/* XMOD_PHY_FIRMWARE_LANE_CONFIG_GET */
/* XMOD_PE_INIT */
extern int xmod_pe_init(IN xmod_pe_config_t config);
#define XMOD_PE_INIT_IN_LEN   (sizeof(xmod_pe_config_t))
#define XMOD_PE_INIT_OUT_LEN  (0)
/* XMOD_PE_STATUS_GET */
extern int xmod_pe_status_get(OUT xmod_pe_status_t status);
#define XMOD_PE_STATUS_GET_IN_LEN   (0)
#define XMOD_PE_STATUS_GET_OUT_LEN  (sizeof(xmod_pe_status_t))
/* XMOD_PE_PORT_STATS_GET */
extern int xmod_pe_port_stats_get(IN uint8 port, OUT xmod_pe_port_stats_t stats, IN uint8 reset);
#define XMOD_PE_PORT_STATS_GET_IN_LEN   (sizeof(uint8)+sizeof(uint8))
#define XMOD_PE_PORT_STATS_GET_OUT_LEN  (sizeof(xmod_pe_port_stats_t))
/* XMOD_PE_TAGID_SET */
extern int xmod_pe_tagid_set(IN xmod_pe_tagid_t tagid);
#define XMOD_PE_TAGID_SET_IN_LEN   (sizeof(xmod_pe_tagid_t))
#define XMOD_PE_TAGID_SET_OUT_LEN  (0)
/* XMOD_PE_FW_VER_GET */
extern int xmod_pe_fw_ver_get(OUT xmod_pe_fw_ver_t fw_ver);
#define XMOD_PE_FW_VER_GET_IN_LEN   (0)
#define XMOD_PE_FW_VER_GET_OUT_LEN  (sizeof(xmod_pe_fw_ver_t))

typedef struct xmod_api_length_s
{
  int in_len;
  int out_len;
} xmod_api_length_t;

extern const xmod_api_length_t bcm_xmod_ethernet_cmds_arg_len_tbl[];
#define GET_XMOD_ETHERNET_CMD_IN_LEN(idx)   (bcm_xmod_ethernet_cmds_arg_len_tbl[idx].in_len)
#define GET_XMOD_ETHERNET_CMD_OUT_LEN(idx)  (bcm_xmod_ethernet_cmds_arg_len_tbl[idx].out_len)

#define GET_XMOD_BUF_WORD_LEN(len)   ((len+3)>>2)

#endif /* _BCM_XMOD_API_H_ */
