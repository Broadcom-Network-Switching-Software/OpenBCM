/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef SOC_FIREBOLT6_FLEXPORT_C
#define SOC_FIREBOLT6_FLEXPORT_C
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#include <soc/firebolt6_tdm.h>
#include <soc/flexport/firebolt6/firebolt6_flexport.h>

int (*fb6_flexport_phases[MAX_FLEX_PHASES]) (int unit,
soc_port_schedule_state_t *port_schedule_state) = {
  &soc_firebolt6_flex_start,
  &soc_firebolt6_flex_top_port_down,
  &soc_firebolt6_tdm_calculation_flexport,
  &soc_firebolt6_flex_ep_reconfigure_remove,
  &soc_firebolt6_flex_mmu_reconfigure_phase1,
  &soc_firebolt6_flex_mmu_reconfigure_phase2,
  &soc_firebolt6_flex_idb_reconfigure,
  &soc_firebolt6_flex_ep_reconfigure_add,
  &soc_firebolt6_flex_top_port_up,
  &soc_firebolt6_flex_end,
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

void *soc_firebolt6_port_lane_info_alloc (
  void
  ) {
  soc_port_lane_info_t *lane_info_mem;

  lane_info_mem =
	(soc_port_lane_info_t *) sal_alloc(sizeof(soc_port_lane_info_t), "");

  return lane_info_mem;
}


int soc_firebolt6_set_port_lane_info (
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


int soc_firebolt6_get_port_lane_info (
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


int soc_firebolt6_reconfigure_ports (
  int unit,
  soc_port_schedule_state_t *port_schedule_state
  ) {
  int i, j, succ;

  if (LOG_CHECK(BSL_LS_SOC_PORT)) {
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "FlexPort top level function entered.\n")));

    
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
      if (fb6_flexport_phases[i] != NULL) {
        succ = fb6_flexport_phases[i](unit, port_schedule_state);
        if (succ != SOC_E_NONE) {
            sal_free(port_schedule_state->cookie);
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Error encountered. Cookie space deallocated.\n")));
            return succ;
        }
      }
  }

  return SOC_E_NONE;
}


int soc_firebolt6_flex_start (
  int unit,
  soc_port_schedule_state_t *port_schedule_state
  ) {
  soc_firebolt6_flex_scratch_t *cookie;

  int i, logical_port, mmu_port, idb_port;

  cookie = (soc_firebolt6_flex_scratch_t *) sal_alloc(
					sizeof(soc_firebolt6_flex_scratch_t), "");

  sal_memset(cookie, 0, sizeof(soc_firebolt6_flex_scratch_t));
  port_schedule_state->cookie = cookie;
  LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
							"Cookie space allocated.\n")));
  for (i = 0; i < port_schedule_state->nport; i++) {
    logical_port = port_schedule_state->resource[i].logical_port;
    mmu_port = port_schedule_state->resource[i].mmu_port;
    idb_port = port_schedule_state->resource[i].idb_port;
    if(logical_port == -1) {
      LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				       	"Logical Port is not allocated\n")));
      return SOC_E_PARAM;
    }
    if (port_schedule_state->resource[i].physical_port != -1) {
      if (mmu_port == -1) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
			  	       	"MMU Port is not allocated\n")));
        return SOC_E_PARAM;
      }
      if (idb_port == -1) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
			  	       	"IDB Port is not allocated\n")));
        return SOC_E_PARAM;
      }
    }
  }

 return SOC_E_NONE;
}


int soc_firebolt6_flex_end (
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

/*! @fn int soc_firebolt6_flex_top_port_down(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief The part of FlexPort code shown as Port-Down in Section 6.6.1 of
 *          Tomahawk2 uArch document. For legacy reasons, software wants to
 *          separate this part under a different function call from the rest of
 *          the FlexPort code.
 */
int
soc_firebolt6_flex_top_port_down (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex top port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_dis_forwarding_traffic(unit,
                                                   port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex mac rx down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_mac_rx_port_down(unit,
                                                   port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex idb port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_idb_port_down(unit,
                                                   port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex mmu port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_mmu_port_down(unit,
                                                   port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex ep port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_ep_port_down(unit,
                                                   port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Flex mac tx port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_mac_tx_port_down(unit,
                                                   port_schedule_state));

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_flex_top_port_up(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief The part of FlexPort code shown as Port-Up in Section XXXX of Trident3
 *          uArch document. For legacy reasons, software wants to separate this part
 *          under a different function call from the rest of the FlexPort code.
 */
int
soc_firebolt6_flex_top_port_up (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				"START soc_firebolt6_flex_top_port_up\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_mmu_port_up(unit,
                                                       port_schedule_state));
	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				"START soc_firebolt6_flex_ep_port_up\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_ep_port_up(unit,
													   port_schedule_state));
	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				"START soc_firebolt6_flex_idb_port_up\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_idb_port_up(unit,
                                                       port_schedule_state));
	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				"START soc_firebolt6_flex_mac_port_up\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_mac_port_up(unit,
                                                       port_schedule_state));
	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				"START soc_firebolt6_flex_en_forwarding_traffic\n")));
    SOC_IF_ERROR_RETURN(soc_firebolt6_flex_en_forwarding_traffic(unit,
                                                       port_schedule_state));

	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
				"END soc_firebolt6_flex_top_port_up\n")));
    return SOC_E_NONE;
}


int
soc_firebolt6_flex_mmu_port_up_top(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            /* Clear Previous EP Credits on the port. */
            soc_firebolt6_mmu_clear_prev_ep_credits(
                unit, &port_schedule_state_t->resource[port]);

    }
    }
    return SOC_E_NONE;
}



/*! @fn int soc_firebolt6_get_ct_class(int speed)
 *  @param speed Speed
 *  @brief Helper function to get CT class
 */
int
soc_firebolt6_get_ct_class(int speed)
{
    int ct_class = 0;
    switch (speed) {
    case 1000: ct_class = 12; break;
    case 2500: ct_class = 12; break;
    case 5000: ct_class = 1; break;
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
    case 106000: ct_class = 13; break;
    default: ct_class = 0; break;
    }
    return ct_class;
}

#endif /* BCM_FIREBOLT6_SUPPORT */
#endif /* SOC_FIREBOLT6_FLEXPORT_C */

