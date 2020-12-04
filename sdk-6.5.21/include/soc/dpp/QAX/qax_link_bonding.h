/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_link_bonding.h
 */

#ifndef __QAX_LINK_BONDING_INCLUDED__

#define __QAX_LINK_BONDING_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/TMC/tmc_api_link_bonding.h>
#include <soc/types.h>



#define SOC_QAX_LB_LBG_BASE_Q_PAIR(_lbg_id) (_lbg_id*8)

#define SOC_QAX_EGR_IF_LBG_RESERVE          27
#define SOC_QAX_EGR_IF_LBG_RESERVE2         127

#define SOC_QAX_EGR_IF_LBG_MIN              64
#define SOC_QAX_EGR_IF_LBG_MAX              75
#define SOC_QAX_NOF_EGR_IF_LBG              (SOC_QAX_EGR_IF_LBG_MAX - SOC_QAX_EGR_IF_LBG_MIN + 1)
#define SOC_QAX_EGR_IF_GET_BY_LBG(_lbg_id)  (SOC_QAX_EGR_IF_LBG_MIN+_lbg_id*2)

soc_error_t
  qax_lb_init(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
  qax_lb_ing_init(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
  qax_lb_egr_init(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
  qax_lb_ing_total_shared_buffer_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 nof_buffers
  );

soc_error_t
  qax_lb_ing_total_shared_buffer_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT int                                *nof_buffers
  );

soc_error_t
  qax_lb_ing_guaranteed_buffer_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_IN  int                                 nof_buffers
  );

soc_error_t
  qax_lb_ing_guaranteed_buffer_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_OUT int                                *nof_buffers
  );

soc_error_t
  qax_lb_ing_modem_to_lbg_map_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

soc_error_t
  qax_lb_ing_modem_to_lbg_unmap_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

soc_error_t
  qax_lb_ing_modem_to_lbg_map_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lbg_t                   *lbg_id
  );

soc_error_t
  qax_lb_ing_expected_seqeunce_num_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          expected_seq_num
  );

soc_error_t
  qax_lb_ing_expected_seqeunce_num_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *expected_seq_num
  );

soc_error_t
  qax_lb_ing_glb_seqeunce_num_width_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          num_width
  );

soc_error_t
  qax_lb_ing_glb_seqeunce_num_width_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT int                         *num_width
  );

soc_error_t
  qax_lb_ing_packet_to_modem_map_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lb_rx_modem_map_index_t      *map_index,
    SOC_SAND_IN  soc_lb_rx_modem_map_config_t     *map_config
  );

soc_error_t
  qax_lb_ing_packet_to_modem_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lb_rx_modem_map_index_t         *map_index,
    SOC_SAND_INOUT  soc_lb_rx_modem_map_config_t     *map_config
  );

soc_error_t
  qax_lb_ing_reorder_config_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_IN  soc_lb_ing_reorder_config_t     *reorder_config
  );

soc_error_t
  qax_lb_ing_reorder_config_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_OUT soc_lb_ing_reorder_config_t     *reorder_config
  );

soc_error_t
  qax_lb_ing_flush_config_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_flush_type_t          type,
    SOC_SAND_OUT int                         *enable
  );

soc_error_t
  qax_lb_ing_flush_config_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_flush_type_t          type,
    SOC_SAND_IN  int                          enable
  );

soc_error_t
  qax_lb_ing_status_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_status_type_t         status_type,
    SOC_SAND_OUT int                         *status
  );

soc_error_t
  qax_lb_ing_modem_status_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_status_type_t   status_type,
    SOC_SAND_OUT int                         *status
  );

soc_error_t
  qax_lb_ing_glb_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  );

soc_error_t
  qax_lb_ing_lbg_stat_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  );

soc_error_t
  qax_lb_ing_modem_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_stat_val_t      type, 
    SOC_SAND_OUT uint64                      *value
  );

soc_error_t
  qax_lb_egr_modem_to_port_map_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_port_t                   port
  );

soc_error_t
  qax_lb_egr_modem_to_port_unmap_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_port_t                   port
  );

soc_error_t
  qax_lb_egr_modem_to_port_map_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT  soc_port_t                 *port
  );

soc_error_t
  qax_lb_egr_lbg_enable_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN soc_lbg_t                    lbg_id,
    SOC_SAND_IN  uint32                       is_enable
  );

soc_error_t
  qax_lb_egr_lbg_enable_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT uint32                       *is_enable
  );

soc_error_t
  qax_lb_egr_modem_to_lbg_map_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

soc_error_t
  qax_lb_egr_modem_to_lbg_unmap_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  );

soc_error_t
  qax_lb_egr_modem_to_lbg_map_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lbg_t                   *lbg_id
  );

soc_error_t
  qax_lb_egr_expected_seqeunce_num_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          expected_seq_num
  );

soc_error_t
  qax_lb_egr_expected_seqeunce_num_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *expected_seq_num
  );

soc_error_t
  qax_lb_egr_seqeunce_num_width_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          num_width
  );

soc_error_t
  qax_lb_egr_seqeunce_num_width_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *num_width
  );

soc_error_t
  qax_lb_egr_modem_seqeunce_num_width_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  int                          num_width
  );

soc_error_t
  qax_lb_egr_modem_seqeunce_num_width_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT int                         *num_width
  );


soc_error_t
  qax_lb_global_packet_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lb_global_packet_config_t      *config
  );

soc_error_t
  qax_lb_global_packet_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT soc_lb_global_packet_config_t      *config
  );

soc_error_t
  qax_lb_lbg_packet_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_IN  soc_lb_lbg_packet_config_t         *config
  );

soc_error_t
  qax_lb_lbg_packet_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_OUT soc_lb_lbg_packet_config_t         *config
  );

soc_error_t
  qax_lb_modem_packet_config_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_modem_t                        modem_id,
    SOC_SAND_IN  soc_lb_modem_packet_config_t      *config
  );

soc_error_t
  qax_lb_modem_packet_config_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_modem_t                        modem_id,
    SOC_SAND_OUT soc_lb_modem_packet_config_t      *config
  );

soc_error_t
  qax_lb_tc_dp_to_packet_priority_config_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lb_tc_dp_t              *tc_dp,
    SOC_SAND_IN  soc_lb_pkt_pri_t            *pkt_pri
  );

soc_error_t
  qax_lb_tc_dp_to_packet_priority_config_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lb_tc_dp_t              *tc_dp,
    SOC_SAND_OUT soc_lb_pkt_pri_t            *pkt_pri
  );

soc_error_t
  qax_lb_egr_sched_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       lbg_count,
    SOC_SAND_IN  soc_lb_lbg_weight_t         *lbg_weights
  );

soc_error_t
  qax_lb_egr_sched_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       max_lbg_count,
    SOC_SAND_OUT soc_lb_lbg_weight_t         *lbg_weights,
    SOC_SAND_OUT uint32                      *lbg_count    
  );

soc_error_t
  qax_lb_egr_segment_config_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_IN  soc_lb_segment_mode_t            segment_mode
  );

soc_error_t
  qax_lb_egr_segment_config_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_OUT soc_lb_segment_mode_t            *segment_mode
  );


soc_error_t
  qax_lb_egr_modem_shaper_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_modem_t                      modem_id,
    SOC_SAND_IN  soc_lb_modem_shaper_config_t    *shaper_config
  );

soc_error_t
  qax_lb_egr_modem_shaper_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_modem_t                      modem_id,
    SOC_SAND_OUT soc_lb_modem_shaper_config_t     *shaper_config
  );

soc_error_t
  qax_lb_egr_modem_segment_shaper_set(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  soc_modem_t                               modem_id,
    SOC_SAND_IN  soc_lb_modem_segment_shaper_config_t     *shaper_config
  );

soc_error_t
  qax_lb_egr_modem_segment_shaper_get(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  soc_modem_t                               modem_id,
    SOC_SAND_OUT soc_lb_modem_segment_shaper_config_t     *shaper_config
  );

soc_error_t
  qax_lb_egr_status_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_status_type_t         status_type,
    SOC_SAND_OUT int                         *status
  );

soc_error_t
  qax_lb_egr_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  );

soc_error_t
  qax_lb_egr_modem_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_stat_val_t      type, 
    SOC_SAND_OUT uint64                      *value
  );

int
  qax_lb_lbg_to_ingress_receive_editor(
    SOC_SAND_IN  int              unit, 
    SOC_SAND_IN  soc_port_t       port
  );

soc_error_t
  qax_lb_lbg_egr_interface_get_by_port(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT uint32     *egr_if
  );

soc_error_t
  qax_lb_lbg_egr_interface_alloc(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT uint32     *egr_if
  );

soc_error_t
  qax_lb_lbg_egr_interface_free(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  uint32     egr_if
  );


soc_error_t
  qax_lb_modem_ports_on_same_interface_get(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_OUT soc_pbmp_t *ports
  );


soc_error_t
  qax_lb_ports_on_reserve_intf_get(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_OUT soc_pbmp_t *ports,
    SOC_SAND_OUT uint32     *port_num
  );

soc_error_t
  qax_lb_port_speed_validate (
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  int         is_check_by_port, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_IN  soc_lbg_t   lbg_id,
    SOC_SAND_IN  int         speed
  );

soc_error_t
  qax_lb_port_speed_set(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_IN  int         speed
  );

soc_error_t
  qax_lb_port_speed_get(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_OUT int         *speed
  );

soc_error_t
  qax_lb_config_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_lb_info_key_t      *lb_key,
    SOC_SAND_OUT soc_lb_cfg_info_t      *lb_cfg_info 
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

