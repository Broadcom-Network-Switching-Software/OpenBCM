/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        traffic.h
 * Purpose:     Defines and types required for traffic test.
 */

#ifndef _TRAFFIC_TEST_H
#define _TRAFFIC_TEST_H

#include <soc/types.h>

#include <bcm/port.h>

/* Traffic Test Port (A) and Link Partner (B) */
typedef struct {
    bcm_l2_addr_t l2A;
    bcm_l2_addr_t l2B;    
    uint32  portA;
    uint32  portB;
    int     vidA;
    int     vidB;    
} traffic_port_config_t;

/*
 * Port stats we monitor on a per-port basis while the test is running.
 */
typedef struct {
    int      error;
    uint64   rbyt;
    uint64   rpkt;
    uint64   raln;
    uint64   rdisc;
    uint64   tbyt;
    uint64   tpkt;
    uint64   dropped;
} traffic_port_stat_t;

typedef enum {
    TRAFFIC_INTERNAL=0,      /* FD, MAC */
    TRAFFIC_INTERNAL_PHY,    /* FD, PHY */
    TRAFFIC_EXTERNAL_BI,     /* Full Duplex */
    TRAFFIC_EXTERNAL_UP,     /* A->B */
    TRAFFIC_EXTERNAL_DOWN    /* B->A */
} traffic_mode_t;

typedef enum {
    SPEED_MAX,
    SPEED_10,
    SPEED_100,
    SPEED_1GIG,
    SPEED_2_5GIG,
    SPEED_3GIG,
    SPEED_10GIG,
    SPEED_12GIG,
    SPEED_13GIG,
    SPEED_15GIG,
    SPEED_16GIG,
    SPEED_20GIG,
    SPEED_21GIG,
    SPEED_24GIG,
    SPEED_25GIG,
    SPEED_30GIG,
    SPEED_32GIG,
    SPEED_40GIG,
    SPEED_42GIG
} traffic_speed_t;

#if defined(INCLUDE_MACSEC)

typedef struct traffic_macsec_key_info_s {
    unsigned char   crypto_key[16];
    int             assocId;
    int             an;
} traffic_macsec_key_info_t;

typedef struct traffic_macsec_port_info_s {
    int             tx_chanId;
    int             num_alloc;
    int             rx_chanId;
    int             cur_an;
    traffic_macsec_key_info_t tx_keys[4];
    traffic_macsec_key_info_t rx_keys[4];
} traffic_macsec_port_info_t;

typedef struct traffic_macsec_info_s {
    pbmp_t          macsec_pbm;
    /* Specify the number of assocs to be cretaed for the test. If
     * one, then application will create another assoc just before
     * hard expiry and switch to the new one. If 2, application will
     * create 2 assoc in the beginning, one active and other as backup. 
     * On expiry, switch will happen to backup and application will
     * replace the expired assoc with a new backup. */
    int             num_assoc;
    traffic_macsec_port_info_t  ports[SOC_MAX_NUM_PORTS];
} traffic_macsec_info_t;

#endif /* INCLUDE_MACSEC */

/* Core test API structure */
typedef struct traffic_test_s {
    bcm_port_info_t         *port_info;
    bcm_port_info_t         *saved_port_info;
#ifdef BCM_ESW_SUPPORT
    modport_map_entry_t     *saved_modport_map;
    modport_map_sw_entry_t  *saved_modport_map_sw;
#else
    void     *saved_modport_map;
    void     *saved_modport_map_sw;
#endif
    traffic_port_stat_t     *port_stats;
    traffic_port_config_t   *port_pair;
    pbmp_t                   ports;
    int                      npkts;
    int                      unit;
    uint32                   pattern;       /* Data Pattern */
    uint32                   pattern_inc;   /* Data Pattern increment  */
    int                      pkt_size;
    int                      l2agetime;
    int                      req_speed;
    int                      stop_on_fail;
    int                      cleanup;
    int                      showstats;
    int                      runtime;
    int                      poll_interval;
    traffic_mode_t           mode;
    int                      run2end; /* run till the end */
#if defined(INCLUDE_MACSEC)
    int                      macsec_enabled;
    traffic_macsec_info_t   *macsec_info;
#endif /* INCLUDE_MACSEC */
} traffic_test_t;

#define TRAFFIC_VID_BASE                10
#define TRAFFIC_LINK_TIMEOUT            15000000

#endif /* _TRAFFIC_TEST_H */
