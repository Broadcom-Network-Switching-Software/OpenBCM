/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __QAX_FABRIC_INCLUDED__

#define __QAX_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>







#define SOC_QAX_FABRIC_PRIORITY_NDX_NOF             (128)
#define SOC_QAX_FABRIC_PRIORITY_NDX_TC_MASK         (0x7)
#define SOC_QAX_FABRIC_PRIORITY_NDX_TC_OFFSET       (0)
#define SOC_QAX_FABRIC_PRIORITY_NDX_DP_MASK         (0x18)
#define SOC_QAX_FABRIC_PRIORITY_NDX_DP_OFFSET       (3)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_MASK      (0x20)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_OFFSET    (5)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_MASK      (0x40)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_OFFSET    (6)

























soc_error_t
soc_qax_fabric_multicast_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count,
    SOC_SAND_IN  soc_module_t                   *destid_array
  );


soc_error_t
  soc_qax_fabric_force_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN soc_dpp_fabric_force_t        force
  );



soc_error_t
soc_qax_fabric_link_config_ovrd(
  int                unit
);





soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_set(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_IN int                                   enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );

soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_get(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_OUT int                                  *enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );




soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type);

soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type
  );


soc_error_t
  soc_qax_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  );

soc_error_t
  soc_qax_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT  int*                               weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  );


soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );




soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT  int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );




soc_error_t
soc_qax_fabric_priority_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_IN  int                fabric_priority
  );

soc_error_t
soc_qax_fabric_priority_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_OUT  int                *fabric_priority
  );


uint32
  soc_qax_fabric_queues_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_OUT soc_dpp_fabric_queues_info_t    *queues_info
  );



soc_error_t
  qax_fabric_pcp_dest_mode_config_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_IN uint32           pcp_mode
  );


int
soc_qax_brdc_fsrd_blk_id_set(
    SOC_SAND_IN  int  unit
);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
