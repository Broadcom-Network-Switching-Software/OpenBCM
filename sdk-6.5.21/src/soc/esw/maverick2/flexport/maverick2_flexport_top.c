/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  maverick2_flexport_top.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_MAVERICK2_SUPPORT)
#include <soc/maverick2.h>
#include <soc/maverick2.h>
#ifndef SOC_MAVERICK2_FLEXPORT_C
#define SOC_MAVERICK2_FLEXPORT_C


#include <soc/flexport/maverick2/maverick2_flexport.h>

int (*mv2_flexport_phases[MAX_FLEX_PHASES]) (int unit, 
soc_port_schedule_state_t *port_schedule_state) = {
  &soc_maverick2_flex_start,
  &soc_maverick2_flex_top_port_down,
  &soc_maverick2_tdm_calculation_flexport,
  &soc_maverick2_flex_ep_reconfigure_remove,
  &soc_maverick2_flex_mmu_reconfigure_phase1,
  &soc_maverick2_flex_mmu_reconfigure_phase2,
  &soc_maverick2_flex_idb_reconfigure,
  &soc_maverick2_flex_ep_reconfigure_add,
  &soc_maverick2_flex_top_port_up,
  &soc_maverick2_flex_end,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

void *soc_maverick2_port_lane_info_alloc (
  void 
  ) {
  soc_port_lane_info_t *lane_info_mem;

  lane_info_mem = 
	(soc_port_lane_info_t *) sal_alloc(sizeof(soc_port_lane_info_t), "");

  return lane_info_mem;
}


int soc_maverick2_set_port_lane_info (
  int unit,
  const void *lane_info_ptr,
  int port_idx,
  int pgw,
  int xlp
  ) {
  soc_port_lane_info_t *lane_ptr = (soc_port_lane_info_t *) lane_info_ptr;

  lane_ptr->port_index = port_idx;
  lane_ptr->pgw = pgw;
  lane_ptr->xlp = xlp;

  return SOC_E_NONE;
}


int soc_maverick2_get_port_lane_info (
  int unit,
  const void *lane_info_ptr,
  int *port_idx,
  int *pgw,
  int *xlp
  ) {
  soc_port_lane_info_t *lane_ptr = (soc_port_lane_info_t *) lane_info_ptr;

  *port_idx = lane_ptr->port_index;
  *pgw = lane_ptr->pgw;
  *xlp = lane_ptr->xlp;

  return SOC_E_NONE;
}


int soc_maverick2_reconfigure_ports (
  int unit,
  soc_port_schedule_state_t *port_schedule_state
  ) {
  int i, j, rv;

  if (LOG_CHECK(BSL_LS_SOC_PORT)) {
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "FlexPort top level function entered.\n")));

    /* print: kept here for testing/debugging purposes only. */
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "nport = %d \n"), port_schedule_state->nport));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "resource[0].num_lanes = %d \n"), 
            port_schedule_state->resource[0].num_lanes));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Frequency = %d MHz\n"), 
				port_schedule_state->frequency));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "is_flexport = %d \n"), 
				port_schedule_state->is_flexport));
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
      LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "port_p2l_mapping[%d] = %d \n"), 
              i, port_schedule_state->in_port_map.port_p2l_mapping[i]));
    }
    for (i = 0; i < SOC_MAX_NUM_MMU_PORTS; i++) {
      LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "port_m2p_mapping[%d] = %d \n"), 
              i, port_schedule_state->in_port_map.port_m2p_mapping[i]));
    }
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Number of OVS groups = %d \n"), 
            port_schedule_state->tdm_egress_schedule_pipe[0].tdm_schedule_slice[
										0].num_ovs_groups));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "OVS group length = %d \n"), 
            port_schedule_state->tdm_egress_schedule_pipe[0].tdm_schedule_slice[
										0].ovs_group_len));
    for (i = 0; i < port_schedule_state->tdm_egress_schedule_pipe[
								0].tdm_schedule_slice[0].num_ovs_groups; i++) {
      LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Pipe 0 egress oversyb group %d entries = \n"),
																		 i));
      for (j = 0; j < port_schedule_state->tdm_egress_schedule_pipe[
							0].tdm_schedule_slice[0].ovs_group_len; j++) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "%d \n"), 
                port_schedule_state->tdm_egress_schedule_pipe[
					0].tdm_schedule_slice[0].oversub_schedule[i][j]));
      }
    }
  }

  /* FlexPort phase calls. */
  for (i = 0; i < MAX_FLEX_PHASES; i++) {
      if (mv2_flexport_phases[i] != NULL) {
          rv = mv2_flexport_phases[i](unit, port_schedule_state);
          if (rv != SOC_E_NONE) {
              sal_free(port_schedule_state->cookie);
              LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "Error encountered. Cookie space deallocated.\n")));
              return rv;
          }
      }
  }

  return SOC_E_NONE;
}


int soc_maverick2_flex_start (
  int unit,
  soc_port_schedule_state_t *port_schedule_state
  ) {
  soc_maverick2_flex_scratch_t *cookie;

  cookie = (soc_maverick2_flex_scratch_t *) sal_alloc(
					sizeof(soc_maverick2_flex_scratch_t), "");
  port_schedule_state->cookie = cookie;
  LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, 
							"Cookie space allocated.\n")));

  return SOC_E_NONE;
}


int soc_maverick2_flex_end (
  int unit,
  soc_port_schedule_state_t *port_schedule_state
  ) {
  int i, j;

  sal_free(port_schedule_state->cookie);
  LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Cookie space deallocated.\n")));

  /* Iterate through all port resource items and reclaim the allocated */
  /* lane information spaces.                                          */
  for (i = 0; i < port_schedule_state->nport; i++) {
    for (j = 0; j < port_schedule_state->resource[i].num_lanes; j++) {
      if (port_schedule_state->resource[i].lane_info[j] != NULL) {
        sal_free(port_schedule_state->resource[i].lane_info[j]);
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, 
					"Lane information space deallocated.\n")));
      }
    }
  }

  return SOC_E_NONE;
}

/*! @fn int soc_maverick2_flex_top_port_down(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 */
int
soc_maverick2_flex_top_port_down (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex top port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_dis_forwarding_traffic(unit,
                                                   port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_mac_rx_port_down(unit,
                                                   port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_idb_port_down(unit,
                                                   port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_mmu_port_down(unit,
                                                   port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_ep_port_down(unit,
                                                   port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_mac_tx_port_down(unit,
                                                   port_schedule_state));

    return SOC_E_NONE;
}

/*! @fn int soc_maverick2_flex_top_port_up(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 */
int
soc_maverick2_flex_top_port_up (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, 
				"START soc_maverick2_flex_top_port_up\n")));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_mmu_port_up(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_ep_port_up(unit, 
													   port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_idb_port_up(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_mac_port_up(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_maverick2_flex_en_forwarding_traffic(unit,
                                                       port_schedule_state));

	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, 
				"END soc_maverick2_flex_top_port_up\n")));
    return SOC_E_NONE;
}

/*! @fn int soc_maverick2_get_ct_class(int speed)
 *  @param speed Speed
 *  @brief Helper function to get CT class
 */
int
soc_maverick2_get_ct_class(int speed)
{
    int ct_class = 0;
    switch (speed) {
    case 1000: ct_class = 1; break;
    case 10000: ct_class = 1; break;
    case 11000: ct_class = 2; break;
    case 20000: ct_class = 3; break;
    case 21000: ct_class = 4; break;
    case 25000: ct_class = 5; break;
    case 27000: ct_class = 6; break;
    case 40000: ct_class = 7; break;
    case 42000: ct_class = 8; break;
    case 50000: ct_class = 9; break;
    case 53000: ct_class = 10; break;
    case 100000: ct_class = 11; break;
    case 106000: ct_class = 12; break;
    default: ct_class = 0; break;
    }
    return ct_class;
}

#endif /* SOC_MAVERICK2_FLEXPORT_C */



#endif /* BCM_MAVERICK2_SUPPORT */
