#ifndef FIREBOLT6_MAC_FLEXPORT_H
#define FIREBOLT6_MAC_FLEXPORT_H

extern int soc_firebolt6_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_firebolt6_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_firebolt6_flex_mac_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);


#endif
