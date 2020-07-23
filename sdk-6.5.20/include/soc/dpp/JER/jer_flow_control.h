
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_FLOW_CONTROL_INCLUDED__

#define __JER_FLOW_CONTROL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>






















soc_error_t
  jer_flow_control_init(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
  jer_flow_control_init_oob_tx(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
  jer_flow_control_init_oob_rx(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
   jer_fc_enables_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *ena_info
    );

soc_error_t
   jer_fc_enables_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_OUT SOC_TMC_FC_ENABLE_BITMAP     *ena_info
    );

soc_error_t
   jer_fc_init_pfc_mapping(
      SOC_SAND_IN int  unit
    );


soc_error_t
  jer_fc_gen_inbnd_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_port_t                         port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO          *info
  );


soc_error_t
  jer_fc_gen_inbnd_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO  *info
  );

soc_error_t
  jer_fc_clear_calendar_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE            cal_mode_ndx,
    SOC_SAND_IN  ARAD_FC_CAL_TYPE               cal_type,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID              if_ndx
  );


soc_error_t
  jer_fc_rec_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info
  );


soc_error_t
  jer_fc_rec_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO           *info
  );

soc_error_t
  jer_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  );


soc_error_t
  jer_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  );


soc_error_t
  jer_fc_rec_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  );


soc_error_t
  jer_fc_rec_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR             *cal_buff
  );

soc_error_t
  jer_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID          ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  );

soc_error_t
  jer_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  );

soc_error_t
  jer_fc_pfc_generic_bitmap_set(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );
soc_error_t
  jer_fc_pfc_generic_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );

soc_error_t
jer_fc_pfc_generic_bitmap_valid_update(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    SOC_SAND_IN   int                               priority,
    SOC_SAND_OUT  uint32                           *is_valid
    );

soc_error_t
jer_fc_pfc_generic_bitmap_used_update(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    SOC_SAND_IN   int                               priority,
    SOC_SAND_IN   int                               pfc_bitmap_index,
    SOC_SAND_OUT  uint32                            is_set
    );

soc_error_t
  jer_fc_cat_2_tc_hcfc_bitmap_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  );

soc_error_t
  jer_fc_cat_2_tc_hcfc_bitmap_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  );

soc_error_t
  jer_fc_glb_hcfc_bitmap_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  );

soc_error_t
  jer_fc_glb_hcfc_bitmap_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  );

soc_error_t
  jer_fc_ilkn_mub_channel_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          bitmap
  );

soc_error_t
  jer_fc_ilkn_mub_channel_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          *bitmap
  );

soc_error_t
  jer_fc_ilkn_mub_gen_cal_set(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  );

soc_error_t
  jer_fc_ilkn_mub_gen_cal_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  );

uint32
  jer_fc_rec_cal_dest_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_REC_CAL_DEST dest
  );

uint32
  jer_fc_gen_cal_src_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_GEN_CAL_SRC src_type
  );

int
  jer_fc_pfc_mapping_set(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32                        nif_id,
    SOC_SAND_IN uint32                        src_pfc_ndx,
    SOC_SAND_IN SOC_TMC_FC_PFC_MAP            *pfc_map
  );

int
  jer_fc_pfc_mapping_get(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32                        nif_id,
    SOC_SAND_IN uint32                        src_pfc_ndx,
    SOC_SAND_OUT SOC_TMC_FC_PFC_MAP           *pfc_map
  );

soc_error_t
  jer_fc_inbnd_mode_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN int                           is_generate,
      SOC_SAND_IN SOC_TMC_FC_INBND_MODE         mode
  );

soc_error_t
  jer_fc_inbnd_mode_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN int                           is_generate,
      SOC_SAND_OUT SOC_TMC_FC_INBND_MODE        *mode
  );

soc_error_t
  jer_fc_glb_rcs_mask_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_IN uint32                        glb_res_src_bitmap
    );

soc_error_t
  jer_fc_glb_rcs_mask_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_OUT uint32                       *glb_res_src_bitmap
    );

int
  jer_fc_port_fifo_threshold_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  SOC_TMC_FC_PORT_FIFO_TH    *info
  );

int
  jer_fc_port_fifo_threshold_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_OUT SOC_TMC_FC_PORT_FIFO_TH  *info
  );

soc_error_t
  jer_fc_vsq_index_group2global(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP vsq_group,
    SOC_SAND_IN  uint32                vsq_in_group_ndx,
    SOC_SAND_IN  int                   cosq,
    SOC_SAND_IN  uint8                 is_ocb_only,
    SOC_SAND_IN  uint32                src_port,
    SOC_SAND_OUT uint32                *vsq_fc_ndx
  );

soc_error_t
  jer_fc_cmic_rx_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN uint32                        priority_bmp,
      SOC_SAND_IN int                           is_ena
  );

soc_error_t
  jer_fc_cmic_rx_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_OUT uint32                       *priority_bmp,
      SOC_SAND_OUT int                          *is_ena
  );

soc_error_t
   jer_fc_find_oob_inf_for_ilkn_inf (
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    ilkn_if,
      SOC_SAND_OUT SOC_TMC_FC_CAL_IF_ID         *oob_if
  );

soc_error_t
   jer_fc_connect_ilkn_inf_to_oob_inf (
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    ilkn_if
  );

soc_error_t
  jer_fc_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  );

soc_error_t
  jer_fc_pfc_index_get(
    SOC_SAND_IN   int     unit,
    SOC_SAND_IN   uint32  phy_port,
    SOC_SAND_OUT  uint32  *entry_offset
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


