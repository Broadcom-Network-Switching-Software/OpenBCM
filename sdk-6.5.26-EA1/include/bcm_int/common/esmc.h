/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_INT_ESMC_H__
#define __BCM_INT_ESMC_H__

#if defined(INCLUDE_PTP)

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/time.h>
#include <bcm/error.h>

#define ESMC_PORT_TABLE_ENTRIES_MAX _SHR_PBMP_PORT_MAX

#define ESMC_FLAG_BIT_ESMC_TIMEOUT_INDICATION     (0u)
#define ESMC_FLAG_BIT_SYNCE_TS_MISS_INDICATION    (1u)
#define ESMC_FLAG_BIT_PORT_LINK_DOWN_INDICATION   (2u)

typedef struct esmc_rx_port_info_s {
    uint8 ssm_code;
				uint8 flags;
} esmc_rx_port_info_t;

typedef struct bcm_extdpll_esmc_data_s {
    int rx_enable;
    int tx_enable;

    bcm_pbmp_t rx_pbmp;
    bcm_pbmp_t tx_pbmp;

    bcm_esmc_pdu_data_t pdu_data;

				bcm_tdpll_event_cb_f esmc_cb_hdlr;

				bcm_port_t best_port;
				bcm_port_t port_l1mux_pri; /* ingress port number of the selected ESMC Rx port for L1MUX primary */
				bcm_port_t port_l1mux_bkup; /* ingress port number of the selected ESMC Rx port for L1MUX backup */

    esmc_rx_port_info_t esmc_info[ESMC_PORT_TABLE_ENTRIES_MAX+1];/* backup last ESMC PDU details required */
} bcm_extdpll_esmc_data_t;

extern int bcm_extdpll_esmc_init(int, int);
int bcm_extdpll_esmc_cleanup(int, int);
int bcm_extdpll_esmc_rx_state_machine(int, int, int, bcm_esmc_pdu_data_t *);
int bcm_extdpll_esmc_switch_event_send(int, int, bcm_esmc_quality_level_t);
int bcm_extdpll_esmc_ql_get(int, int, bcm_esmc_quality_level_t *);
int bcm_extdpll_esmc_ql_set(int, int, int, bcm_esmc_quality_level_t);
int bcm_extdpll_esmc_mac_get(int, int, bcm_mac_t *);
int bcm_extdpll_esmc_mac_set(int, int, bcm_mac_t *);
int bcm_extdpll_esmc_rx_enable_get(int, int, int *);
int bcm_extdpll_esmc_rx_enable_set(int, int, int);
int bcm_extdpll_esmc_tx_enable_get(int, int, int *);
int bcm_extdpll_esmc_tx_enable_set(int, int, int);
int bcm_extdpll_esmc_rx_portbitmap_get(int, int, bcm_pbmp_t *);
int bcm_extdpll_esmc_rx_portbitmap_set(int, int, bcm_pbmp_t);
int bcm_extdpll_esmc_tx_portbitmap_get(int, int, bcm_pbmp_t *);
int bcm_extdpll_esmc_tx_portbitmap_set(int, int, bcm_pbmp_t);
int bcm_extdpll_esmc_essm_code_get(int, int, bcm_esmc_essm_code_t *);
int bcm_extdpll_esmc_essm_code_set(int, int, bcm_esmc_essm_code_t);
int bcm_extdpll_event_callback_register(int, int, bcm_tdpll_event_cb_f);
int bcm_extdpll_event_callback_unregister(int, int);
int bcm_extdpll_esmc_select_port_set(int, bcm_port_t,	bcm_time_synce_clock_src_type_t);
void bcm_extdpll_linkscan_handler(int, bcm_port_t, bcm_port_info_t *);
int bcm_extdpll_esmc_synce_event_handler(int, int, int);

bcm_error_t bcm_extdpll_register_linkscan_callback(int);
bcm_error_t bcm_extdpll_unregister_linkscan_callback(int);

int bcm_extdpll_esmc_event_cb_register(int unit, int stack_id);
int bcm_extdpll_esmc_event_cb_unregister(int unit, int stack_id);

#endif /* defined(INCLUDE_PTP) */
#endif /* !__BCM_INT_ESMC_H__ */
