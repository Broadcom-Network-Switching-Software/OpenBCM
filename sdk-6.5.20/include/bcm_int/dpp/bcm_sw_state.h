/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * INFO: this module is the entry point the branch of the sw state that compose the dpp bcm 
 * layer's sw state 
 */
#ifndef _BCM_SW_STATE_H
#define _BCM_SW_STATE_H

#if defined(BCM_PETRA_SUPPORT)
/* { */
#include <soc/types.h>
#include <shared/swstate/sw_state.h>
#include <soc/error.h>

typedef struct soc_dpp_bcm_sw_state_s{
    PARSER_HINT_PTR bcm_stg_info_t                       *stg;
    PARSER_HINT_ARR _dpp_policer_state_t                 *policer;
    PARSER_HINT_AUTOSYNC_EXCLUDE PARSER_HINT_PTR _bcm_dpp_counter_state_t             *counter;
    PARSER_HINT_PTR _bcm_petra_mirror_unit_data_t        *mirror;
    PARSER_HINT_PTR bcm_dpp_l3_info_t                    *l3;
    PARSER_HINT_PTR l2_data_t                            *l2;
    PARSER_HINT_PTR trunk_state_t                        *trunk;
    PARSER_HINT_PTR _bcm_dpp_vlan_unit_state_t           *vlan;
    PARSER_HINT_PTR _bcm_dpp_vswitch_bookkeeping_t       *vswitch;
    PARSER_HINT_PTR bcm_dpp_rx_info_t                    *rx;
    PARSER_HINT_PTR bcm_dpp_failover_info_t              *failover;
    PARSER_HINT_PTR bcm_dpp_gport_mgmt_info_t            *gport_mgmt;
    PARSER_HINT_PTR bcm_dpp_bfd_info_t                   *bfd;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_info_t            *alloc_mngr;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_utils_info_t      *alloc_mngr_utils;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_glif_info_t       *alloc_mngr_glif;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_local_lif_info_t  *alloc_mngr_local_lif;
    PARSER_HINT_PTR bcm_dpp_ipmc_info_t                  *ipmc;
    PARSER_HINT_PTR bcm_dpp_switch_info_t                *_switch;
    PARSER_HINT_PTR bcm_dpp_port_info_t                  *port;
    PARSER_HINT_PTR bcm_dpp_trill_state_t                *trill;
    PARSER_HINT_PTR bcm_dpp_stack_config_t               *stack;
    PARSER_HINT_PTR bcm_dpp_qos_state_t                  *qos;
    PARSER_HINT_PTR bcm_dpp_mim_info_t                   *mim;
    PARSER_HINT_PTR bcm_dpp_oam_info_t                   *oam;
    PARSER_HINT_PTR bcm_dpp_cosq_info_t                  *cosq;
    PARSER_HINT_PTR bcm_dpp_field_info_t                 *field;
    PARSER_HINT_PTR bcm_dpp_lif_linked_list_t            *lif_linked_list ;
    PARSER_HINT_PTR bcm_dpp_lb_info_t                    *linkbonding;
    PARSER_HINT_PTR bcm_dpp_bfd_ipv6_info_t              *bfd_ipv6;
    PARSER_HINT_PTR bcm_time_interface_t                 *time_intf;
} soc_dpp_bcm_sw_state_t;
/* } */
#endif
#endif /* _SHR_SW_STATE_H */
