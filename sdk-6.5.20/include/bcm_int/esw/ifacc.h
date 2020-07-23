/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ifacc.h
 */
#ifndef _BCM_INT_IFA_CC_H_
#define _BCM_INT_IFA_CC_H_

#if defined(INCLUDE_IFA)

/*  Maximum number of gports supported by IFA-CC Eapp */
#define     BCM_IFA_CC_MAX_NUM_GPORT              64

/*  Maximum number of PIPES */
#define     BCM_IFA_CC_MAX_NUM_PIPES              8

/*  EGR_VLAN_XLATE_1_SINGLE data size */
#define     BCM_IFA_CC_EVLXT_DATA_SIZE            4

/* 5 sec for FW response */
#define BCM_IFA_CC_UC_MSG_TIMEOUT                 5000000

/* 100 usec for FW response */
#define BCM_IFA_CC_UC_STATUS_TIMEOUT              100

#define BCM_IFA_CC_M0_NUM_UCS                     0x4
#define BCM_IFA_CC_M0_EAPP_STATUS_OFFSET          0xC0
#define BCM_IFA_CC_M0_EAPP_STATUS                 0xBADC0DE1
#define BCM_IFA_CC_APP_STATUS_1                   0xC001C0DE
#define BCM_IFA_CC_APP_STATUS_2                   0xC0DEC001
#define BCM_IFA_CC_CONFIG_STATUS_BIT              0x1

typedef struct _bcm_ifa_cc_gport_port_queue_map_s {
    /* gport value */
    bcm_gport_t gport;

    /* DGPP Port */
    bcm_gport_t dgpp;

    /* global queue based on physical port and local queue id */
    uint32 global_num;

    /* Pipe Port and queue number */
    uint16 pipe_num;
    uint16 port_num;
    uint16 queue_num;

    struct _bcm_ifa_cc_gport_port_queue_map_s *next;

} _bcm_ifa_cc_gport_port_queue_map_t;

/* IFA CC */
typedef struct _bcm_ifa_cc_data_s {
    uint32 app_status[2];
    uint32 host_status;
    uint32 uc_status;
    uint32 debug_info[16];
    uint32 uc_bit_map;
    uint32 flag;
    uint32 num_pipes;
    uint32 num_gports;
    uint32 num_sbus_dma;
    uint32 ports_per_pipe;
    uint32 queues_per_pipe;
    uint32 queues_per_port;
    uint32 dgpp[BCM_IFA_CC_MAX_NUM_GPORT];
    uint32 gport[BCM_IFA_CC_MAX_NUM_GPORT];
    uint32 pri_class_id[BCM_IFA_CC_MAX_NUM_GPORT];
    uint32 port_pipe_queue[BCM_IFA_CC_MAX_NUM_GPORT];
    uint32 egr_tx_cnt_tbl_addr[BCM_IFA_CC_MAX_NUM_PIPES];
    uint32 queue_size_tbl_addr[BCM_IFA_CC_MAX_NUM_PIPES];
    uint32 egr_vlan_xlate_tbl_addr[BCM_IFA_CC_MAX_NUM_PIPES];
    uint32 egr_vlan_xlate_tbl_offset[BCM_IFA_CC_MAX_NUM_GPORT];
    uint32 egr_vlan_xlate_tbl_data[(BCM_IFA_CC_MAX_NUM_GPORT * BCM_IFA_CC_EVLXT_DATA_SIZE)];
} _bcm_ifa_cc_data_t;

#define BASEADDR_OFFSET_COMMON_SRAM(unit) \
       (SOC_IS_TRIDENT3X(unit) ? 0x01350000 :  0x01350000)
#define BASEADDR_OFFSET(unit)      (SOC_IS_TRIDENT3X(unit) ? 0x100 :  0x100)
#define ifa_cc_offset(mem) \
        ((size_t)((char *)&((_bcm_ifa_cc_data_t *)0)->mem - \
                  (char *)(_bcm_ifa_cc_data_t *)0))

#endif /* INCLUDE_IFA */
#endif  /* !_BCM_INT_IFA_CC_H_ */
