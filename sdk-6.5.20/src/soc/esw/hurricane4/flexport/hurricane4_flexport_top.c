/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       hurricane4_flexport_top.c
 * Purpose:
 * Requires:
 */

#ifndef SOC_HURRICANE4_FLEXPORT_C
#define SOC_HURRICANE4_FLEXPORT_C
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#include <soc/hurricane4_tdm.h>
#include <soc/flexport/hurricane4/hurricane4_flexport.h>


int (*hr4_flexport_phases[MAX_FLEX_PHASES]) (int unit,
soc_port_schedule_state_t *port_schedule_state) = {
    &soc_hurricane4_flex_start,
    &soc_hurricane4_flex_top_port_down,
    &soc_hurricane4_tdm_calculation_flexport,
    &soc_hurricane4_flex_ep_reconfigure_remove,
    &soc_hurricane4_flex_mmu_reconfigure_phase1,
    &soc_hurricane4_flex_mmu_reconfigure_phase2,
    &soc_hurricane4_flex_idb_reconfigure,
    &soc_hurricane4_flex_ep_reconfigure_add,
    &soc_hurricane4_flex_top_port_up,
    &soc_hurricane4_flex_end,
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

int
soc_hurricane4_flex_is_gphy_pmq_port(unsigned int phy_port)
{
    if (phy_port >= (HR4_GPHY_START) && (phy_port <= (HR4_PMQ_END))) {
        return 1;
    } else {
        return 0;
    }
}

int
soc_hurricane4_flex_is_qgphy_port(unsigned int phy_port)
{
    if (phy_port >=(HR4_GPHY_START) && (phy_port <= (HR4_GPHY_END))) {
        return 1;
    } else {
        return 0;
    }
}

int
soc_hurricane4_flex_is_pmq_port(unsigned int phy_port)
{
    if (phy_port >=(HR4_PMQ_START) && (phy_port <= (HR4_PMQ_END))) {
        return 1;
    } else {
        return 0;
    }
}

int
soc_hurricane4_flex_is_eagle_port(unsigned int phy_port)
{
    if (phy_port >=(HR4_XLP_START) && (phy_port <= (HR4_XLP_END))) {
        return 1;
    } else {
        return 0;
    }
}

int
soc_hurricane4_flex_is_falcon_port(unsigned int phy_port)
{
    if (phy_port >=(HR4_CLP_START) && (phy_port <= (HR4_CLP_END))) {
        return 1;
    } else {
        return 0;
    }
}

int
soc_hurricane4_flex_is_macsec_port(unsigned int phy_port)
{
    if (phy_port >=(HR4_MACSEC_START) && (phy_port <= (HR4_MACSEC_END))) {
        return 1;
    } else {
        return 0;
    }
}

void
*soc_hurricane4_port_lane_info_alloc (
        void
)
{
    soc_port_lane_info_t *lane_info_mem;

    lane_info_mem =
        (soc_port_lane_info_t *) sal_alloc(sizeof(soc_port_lane_info_t), "");

    return lane_info_mem;
}


int
soc_hurricane4_set_port_lane_info (
        int unit,
        const void *lane_info_ptr,
        int port_idx,
        int pgw,
        int xlp
        )
{
    soc_port_lane_info_t *lane_ptr = (soc_port_lane_info_t *) lane_info_ptr;

    lane_ptr->port_index = port_idx;
    lane_ptr->pgw = pgw;
    lane_ptr->xlp = xlp;

    return SOC_E_NONE;
}


int
soc_hurricane4_get_port_lane_info (
        int unit,
        const void *lane_info_ptr,
        int *port_idx,
        int *pgw,
        int *xlp
        )
{
    soc_port_lane_info_t *lane_ptr = (soc_port_lane_info_t *) lane_info_ptr;

    *port_idx = lane_ptr->port_index;
    *pgw = lane_ptr->pgw;
    *xlp = lane_ptr->xlp;

    return SOC_E_NONE;
}


int
soc_hurricane4_reconfigure_ports (
        int unit,
        soc_port_schedule_state_t *port_schedule_state
        )
{
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
        if (hr4_flexport_phases[i] != NULL) {
            succ = hr4_flexport_phases[i](unit, port_schedule_state);
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


int
soc_hurricane4_flex_start (
        int unit,
        soc_port_schedule_state_t *port_schedule_state
        )
{
    soc_hurricane4_flex_scratch_t *cookie;

    cookie = (soc_hurricane4_flex_scratch_t *) sal_alloc(
            sizeof(soc_hurricane4_flex_scratch_t), "");
    port_schedule_state->cookie = cookie;
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Cookie space allocated.\n")));

    return SOC_E_NONE;
}


int
soc_hurricane4_flex_end (
        int unit,
        soc_port_schedule_state_t *port_schedule_state
        )
{
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

/*! @fn int soc_hurricane4_flex_top_port_down(int unit, soc_port_schedule_state_t
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
soc_hurricane4_flex_top_port_down (
        int                        unit,
        soc_port_schedule_state_t *port_schedule_state
        )
{
    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit,
                        "Flex top port down starting...\n")));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_dis_forwarding_traffic(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_mac_rx_port_down(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_idb_port_down(unit,
                  port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_mmu_port_down(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_ep_port_down(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_mac_tx_port_down(unit,
                port_schedule_state));

    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_flex_top_port_up(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief The part of FlexPort code shown as Port-Up in Section XXXX of Trident3
 *          uArch document. For legacy reasons, software wants to separate this part
 *          under a different function call from the rest of the FlexPort code.
 */
int
soc_hurricane4_flex_top_port_up (
        int                        unit,
        soc_port_schedule_state_t *port_schedule_state
        )
{
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "START soc_hurricane4_flex_top_port_up\n")));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_mmu_port_up_top(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_ep_port_up(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_idb_port_up(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_mac_port_up(unit,
                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hurricane4_flex_en_forwarding_traffic(unit,
                port_schedule_state));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "END soc_hurricane4_flex_top_port_up\n")));
    return SOC_E_NONE;
}


int
soc_hurricane4_flex_mmu_port_up_top(int                        unit,
        soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    int qmode;
    int inst;
    int pipe;
    int pipe_flexed;
    uint64 temp64;
    uint64 rval64;

    COMPILER_64_SET(temp64, 0, HR4_MMU_FLUSH_OFF);

    /* Per-Port configuration */
    for (pipe = 0; pipe < HURRICANE4_TDM_PIPES_PER_DEV; pipe++) {
        soc_hurricane4_mmu_get_pipe_flexed_status(
                unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {
            soc_hurricane4_mmu_vbs_port_flush(unit, port_schedule_state_t, pipe, &temp64);
            soc_hurricane4_mmu_rqe_port_flush(unit, pipe, temp64);
        }
    }

    /* Per-Pipe configuration */

    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            /* Clear Previous EP Credits on the port. */
            if (soc_hurricane4_flex_is_pmq_port(port_schedule_state_t->resource[port].physical_port)) {
                inst = ((port_schedule_state_t->resource[port].physical_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst , 0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;
            }
            if (qmode == 0 &&
                !soc_hurricane4_flex_is_qgphy_port(port_schedule_state_t->resource[port].physical_port)) {
            soc_hurricane4_mmu_clear_prev_ep_credits(
                    unit, &port_schedule_state_t->resource[port]);
            }
            soc_hurricane4_mmu_mtro_port_flush(
                    unit, &port_schedule_state_t->resource[port], temp64);

        }
    }
    return SOC_E_NONE;
}



/*! @fn int soc_hurricane4_get_ct_class(int speed)
 *  @param speed Speed
 *  @brief Helper function to get CT class
 */
int
soc_hurricane4_get_ct_class(int speed)
{
    int ct_class = 0;
    switch (speed) {
        case 1000: ct_class = 12; break;
        case 2500: ct_class = 14; break;
        case 5000: ct_class = 15; break;
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

#endif /* BCM_HURRICANE4_SUPPORT  */
#endif /* SOC_HURRICANE4_FLEXPORT_C */
