/*! \file bcmpmac_types.h
 *
 * BCMPMAC public data types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_TYPES_H
#define BCMPMAC_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>

/*! The maximum number of physical ports per PM. */
#define BCMPMAC_NUM_PORTS_PER_PM_MAX 16

/*!
 * \brief PMAC physical port number type.
 *
 * The PMAC physical port number is the physical port macro port. It means the
 * physical port number within a port macro which is equivalent to the
 * "block-port number" or the "port index".
 */
typedef shr_port_t bcmpmac_pport_t;

/*! PMAC MAC address type. */
typedef shr_mac_t bcmpmac_mac_t;

/*! PMAC encap modes. */
typedef enum bcmpmac_encap_mode_e {

    /*! IEEE. */
    BCMPMAC_ENCAP_IEEE = 0,

    /*! HIGIG. */
    BCMPMAC_ENCAP_HIGIG = 1,

    /*! HIGIG2. */
    BCMPMAC_ENCAP_HIGIG2 = 3,

    /*! HIGIG2 LITE. */
    BCMPMAC_ENCAP_HIGIG2_LITE = 4,

    /*! HiGig3 encapsulation. */
    BCMPMAC_ENCAP_HG3 = 6,

    /*! 1B preamble. */
    BCMPMAC_ENCAP_PREAMBLE_SOP_ONLY = 8,

    /*! Must be the last. */
    BCMPMAC_ENCAP_COUNT

} bcmpmac_encap_mode_t;

/*! PMAC core port modes. */
typedef struct bcmpmac_core_port_mode_e {

    /*! Port lane map. */
    uint32_t lane_map[BCMPMAC_NUM_PORTS_PER_PM_MAX];

} bcmpmac_core_port_mode_t;

/*! Pause flow control configuration. */
typedef struct bcmpmac_pause_ctrl_s {

    /*! Enable PAUSE frame transmit. */
    int tx_enable;

    /*! Enable PAUSE frame receive. */
    int rx_enable;

    /*!
     * use -1 for disable this feature;
     * Threshold for pause timer to cause XOFF to be resent.
     */
    int refresh_timer;

    /*! Time value sent in the Timer Field for classes in XOFF state. */
    int xoff_timer;

} bcmpmac_pause_ctrl_t;

/*! Fault status. */
typedef struct bcmpmac_fault_status_s {

    /*! Local Fault. */
    int local_fault;

    /*! Remote Fault. */
    int remote_fault;

} bcmpmac_fault_status_t;

/*! Priority-based flow control configuration. */
typedef struct bcmpmac_pfc_s {

    /*
     * PFC behavior control.
     */

    /*! Enable PFC transmit. */
    bool tx_enable;

    /*! Enable PFC receive. */
    bool rx_enable;

    /*! Enable PFC counters. */
    bool stats_enable;

    /*! Enable PFC force_xon. */
    bool xon_enable;

    /*!
     * Threshold for pause timer to cause XOFF to be resent. Set to 0 to
     * disable this feature.
     */
    uint32_t refresh_timer;

    /*
     * PFC frame configure.
     */

    /*! Destination MAC address to use for the PFC frame. */
    bcmpmac_mac_t da;

    /*! EtherType to use for PFC frame. */
    uint32_t eth_type;

    /*! Opcode to use for PFC frame. */
    uint32_t opcode;

    /*! Time value sent in the Timer Field for classes in XOFF state. */
    uint32_t xoff_timer;

    /*! Enable to allow PFC packets into switch device. */
    uint32_t pfc_pass;

} bcmpmac_pfc_t;

/*! CDPORT_MODE_REG fields. */
typedef struct bcmpmac_port_mode_s {

    /*! Indicate 400G single port mode. */
    int is_400g;

    /*! CDMAC_1 port mode. */
    int mac1_port_mode;

    /*! CDMAC_0 port mode. */
    int mac0_port_mode;

} bcmpmac_port_mode_t;

/*! Fault disable set status. */
typedef struct bcmpmac_fault_disable_s {

    /*! Local Fault disable. */
    int local_fault_disable;

    /*! Remote Fault disable. */
    int remote_fault_disable;

} bcmpmac_fault_disable_t;

/*! Port macro basis to control TSC . */
typedef struct bcmpmac_tsc_ctrl_s {

    /*! TSC clock select between PLL0 and PLL1. */
    uint32_t tsc_clk_sel;

    /*! TSC power down. */
    uint32_t tsc_pwrdwn;

    /*! TSC hard reset (active low). */
    uint32_t tsc_rstb;

    /*! TSC IDDQ. */
    uint32_t tsc_iddq;

} bcmpmac_tsc_ctrl_t;

/*! Port Macro TX timestamp information. */
typedef struct bcmpmac_tx_timestamp_info_s {

    /*! Timestamp low 32-bits. */
    uint32_t timestamp_lo;

    /*! Timestamp high 32-bits. */
    uint32_t timestamp_hi;

    /*! Sequence ID. */
    uint32_t sequence_id;

} bcmpmac_tx_timestamp_info_t;

/*! Flag for multi-mpp port mode used in port mode setting. */
#define BCMPMAC_PM_PORT_MODE_MUTI_MPP 0x1
/*! Set multi-mpp port mode flag in flags. */
#define BCMPMAC_PM_PORT_MODE_MUTI_MPP_SET(flags) \
                            (flags |= BCMPMAC_PM_PORT_MODE_MUTI_MPP)
/*! Clear multi-mpp port mode flag in flags. */
#define BCMPMAC_PM_PORT_MODE_MUTI_MPP_CLR(flags) \
                            (flags &= ~BCMPMAC_PM_PORT_MODE_MUTI_MPP)
/*! Get multi-mpp port mode flag from flags. */
#define BCMPMAC_PM_PORT_MODE_MUTI_MPP_GET(flags) \
                            (flags & BCMPMAC_PM_PORT_MODE_MUTI_MPP ? 1 : 0)

/*! MAC 1588 egress timestamping modes. */
typedef enum bcmpmac_egr_timestamp_mode_t {
    /*! 32-bit egress 1588 timestamp. */
    BCMPMAC_32_BIT_EGRESS_TIMESTAMP = 0,

    /*! 48-bit egress 1588 timestamp. */
    BCMPMAC_48_BIT_EGRESS_TIMESTAMP = 1
} bcmpmac_egr_timestamp_mode_t;

#endif /* BCMPMAC_TYPES_H */
