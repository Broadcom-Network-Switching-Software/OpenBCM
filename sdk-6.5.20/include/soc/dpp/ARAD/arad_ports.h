/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_PORTS_INCLUDED__

#define __ARAD_PORTS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>

#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dcmn/dcmn_port.h>






#define ARAD_PORTS_IF_UNMAPPED_INDICATION 0xff


#define ARAD_PORTS_NOF_REASSEMBLY_CONTEXT SOC_DPP_DEFS_GET(unit, num_of_reassembly_context)
#define ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED (SOC_IS_QUX(unit) ? ARAD_PORTS_NOF_REASSEMBLY_CONTEXT-1 : ARAD_PORTS_IF_UNMAPPED_INDICATION)   


#define ARAD_MAP_OFP_TO_MIRR_CH_FLD_SIZE 6


#define ARAD_MAX_PORT_TCS_PER_PS 8

#define ARAD_PP_PORT_NDX_MAX                             (ARAD_PORT_NOF_PP_PORTS - 1)

#define ARAD_PORT_EG_MIRROR_NOF_VID_MIRROR_INDICES                 (7)

#define ARAD_PORT_INVALID_RCY_PORT                                (-1)

#define ARAD_PORT_INVALID_EGQ_INTERFACE                            (-1)

#define ARAD_PORT_PCS_8_10_EFFECTIVE_RATE_PERCENT                              (80)


#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_LSB              (0)
#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_MSB              (1)
#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_LENGTH           (ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_MSB - ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_LSB + 1)
#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_VALUE            (0x1)
#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_LSB              (2)
#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_MSB              (2)
#define ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_LENGTH           (ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_MSB - ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_LSB + 1)


#define ARAD_PORT_INPORT_KEY_DIFFSERV_LSB               (0)
#define ARAD_PORT_INPORT_KEY_DIFFSERV_LENGTH            (1)
#define ARAD_PORT_INPORT_KEY_NOT_DIFFSERV_LSB           (4)
#define ARAD_PORT_INPORT_KEY_NOT_DIFFSERV_LENGTH        (1)
#define ARAD_PORT_INPORT_KEY_RECYCLE_PORT_LSB           (8)
#define ARAD_PORT_INPORT_KEY_RECYCLE_PORT_LENGTH        (20)


#define ARAD_PORT_PTC_KEY_BYTES_TO_RMV_LSB              (8)
#define ARAD_PORT_PTC_KEY_BYTES_TO_RMV_MSB              (15)
#define ARAD_PORT_PTC_KEY_BYTES_TO_RMV_LENGTH           (ARAD_PORT_PTC_KEY_BYTES_TO_RMV_MSB - ARAD_PORT_PTC_KEY_BYTES_TO_RMV_LSB + 1)


#define ARAD_MAPPED_PP_PORT_LENGTH                      (9)


#define ARAD_PORT_MAX_PON_PORT(unit) (SOC_IS_JERICHO(unit) ? (15) : (7))
#define ARAD_PORT_VIRTUAL_PORT_PON_RESERVE_ENTRY_NOF (256)
#define ARAD_PORT_MAX_TUNNEL_ID_IN_MAX_PON_PORT (2047 - ARAD_PORT_VIRTUAL_PORT_PON_RESERVE_ENTRY_NOF)
#define ARAD_PORT_MAX_TUNNEL_ID_IN_AVERAGE_MODE(unit) (SOC_IS_JERICHO(unit) ? 2032 : 2016)







#define ARAD_PORT_IS_INCOMING(dir) \
  SOC_SAND_NUM2BOOL(((dir) == ARAD_PORT_DIRECTION_INCOMING  ) || ((dir) == ARAD_PORT_DIRECTION_BOTH))

#define ARAD_PORT_IS_OUTGOING(dir) \
  SOC_SAND_NUM2BOOL(((dir) == ARAD_PORT_DIRECTION_OUTGOING  ) || ((dir) == ARAD_PORT_DIRECTION_BOTH))

#define ARAD_BASE_PORT_TC2PS(base_port_tc) \
  ((base_port_tc)/ARAD_MAX_PORT_TCS_PER_PS)

#define ARAD_PS2BASE_PORT_TC(ps) \
  (ps*ARAD_MAX_PORT_TCS_PER_PS)

#define ARAD_IS_START_OF_PS(port_tc) \
  (port_tc % ARAD_MAX_PORT_TCS_PER_PS == 0)

#define ARAD_PS_CPU_FIRST_VALID_QPAIR 192

#define ARAD_PS_CPU_VALID_RANGE(ps) \
  (SOC_SAND_IS_VAL_IN_RANGE(ARAD_PS2BASE_PORT_TC(ps),ARAD_PS_CPU_FIRST_VALID_QPAIR,ARAD_EGR_BASE_Q_PAIRS_NDX_MAX))

#define ARAD_PS_RCPU_VALID_RANGE(ps) \
  (SOC_SAND_IS_VAL_IN_RANGE(ARAD_PS2BASE_PORT_TC(ps),0,96))







typedef struct 
{
  
  SOC_SAND_OCC_BM_PTR
    reassembly_ctxt_occ;

} ARAD_REASSBMEBLY_CTXT;











int
  arad_port_parse_header_type_unsafe(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN soc_port_t  port,
    SOC_SAND_IN uint32      port_parser_program_pointer,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE * header_type_in
  );


uint32
  arad_ports_init(
    SOC_SAND_IN  int                 unit
  );

uint32
  arad_ports_lag_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_LAG_MODE        lag_mode
  );

uint32
  arad_ports_lag_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PORT_LAG_MODE        *lag_mode,
    SOC_SAND_OUT uint32                     *sys_lag_port_id_nof_bits
  );

uint32
  arad_ports_lag_nof_lag_groups_get_unsafe(
    SOC_SAND_IN  int                 unit
  );

uint32
  arad_ports_lag_nof_lag_entries_get_unsafe(
    SOC_SAND_IN  int                 unit
  );

uint32
  arad_ports_logical_sys_id_build(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8 is_lag_not_phys,
    SOC_SAND_IN  uint32  lag_id,
    SOC_SAND_IN  uint32  lag_member_id,
    SOC_SAND_IN  uint32  sys_phys_port_id,
    SOC_SAND_OUT uint32  *sys_logic_port_id
  );

uint32
  arad_ports_logical_sys_id_parse(
      SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  sys_logic_port_id,
    SOC_SAND_OUT uint8 *is_lag_not_phys,
    SOC_SAND_OUT uint32 *lag_id,
    SOC_SAND_OUT uint32 *lag_member_id,
    SOC_SAND_OUT uint32 *sys_phys_port_id
  );

uint32
  arad_sys_phys_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  );


uint32
  arad_sys_virtual_port_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  );


int
  arad_sys_phys_to_local_port_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  );


uint32
  arad_sys_phys_to_local_port_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  );


uint32
  arad_local_to_sys_phys_port_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  );


uint32
  arad_modport_to_sys_phys_port_map_get_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  fap_id,            
    SOC_SAND_IN  uint32  tm_port,           
    SOC_SAND_OUT uint32  *sys_phys_port_id  
  );



int
  arad_port_control_pcs_set(
    SOC_SAND_IN    int              unit,
    SOC_SAND_IN    soc_port_t       port,
    SOC_SAND_IN    soc_dcmn_port_pcs_t    pcs
  );

uint32
  arad_port_control_pcs_set_verify(
    SOC_SAND_IN    int      unit,
    SOC_SAND_IN    uint32      link_ndx,
    SOC_SAND_IN    soc_dcmn_port_pcs_t pcs
  );


soc_error_t
  arad_port_control_low_latency_set(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN soc_port_t     port,
     SOC_SAND_IN int            value
     );



soc_error_t
  arad_port_control_fec_error_detect_set(
     SOC_SAND_IN int        unit,
     SOC_SAND_IN soc_port_t port,
     SOC_SAND_IN int        value
     );



soc_error_t
  arad_port_control_low_latency_get(
     SOC_SAND_IN  int           unit,
     SOC_SAND_IN  soc_port_t    port,
     SOC_SAND_OUT int*          value
     );



soc_error_t
  arad_port_control_fec_error_detect_get(
     SOC_SAND_IN  int           unit,
     SOC_SAND_IN  soc_port_t    port,
     SOC_SAND_OUT int*          value
     );



int
  arad_port_control_pcs_get(
    SOC_SAND_IN     int              unit,
    SOC_SAND_IN     soc_port_t       port,
    SOC_SAND_OUT soc_dcmn_port_pcs_t *pcs
  );


uint32
  arad_port_to_dynamic_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8                    is_init
  );
uint32
  arad_port_to_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8               is_init
  );


uint32
  arad_port_to_dynamic_interface_unmap_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  );

uint32
  arad_port_to_interface_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info
  );


int
  arad_port_to_interface_map_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32             tm_port,
    SOC_SAND_OUT ARAD_INTERFACE_ID  *if_id,
    SOC_SAND_OUT uint32             *channel_id
  );


 
   
uint32  
    arad_ports_is_port_lag_member_unsafe( 
              SOC_SAND_IN  int                                 unit, 
              SOC_SAND_IN  int                                 core_id,
              SOC_SAND_IN  uint32                                 port_id, 
              SOC_SAND_OUT uint8                                 *is_in_lag, 
              SOC_SAND_OUT uint32                                 *lag_id); 



int arad_ports_lag_fix_next_member_pionter(int unit, uint32 lag_ndx);



uint32
  arad_ports_lag_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO        *new_lag_info
  );



uint32
  arad_ports_lag_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info
  );


uint32
  arad_ports_lag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PPC_LAG_INFO      *info_outgoing
  );

void
  arad_ports_lag_mem_id_mark_invalid(
    SOC_SAND_INOUT SOC_PPC_LAG_INFO      *info
  );


int
  arad_ports_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER               *add_lag_member
  );




int
  arad_ports_lag_sys_port_remove_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER  *removed_lag_member
  );


uint32
  arad_ports_lag_order_preserve_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  );


uint32
  arad_ports_lag_order_preserve_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  );


uint32
  arad_ports_lag_order_preserve_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  );

uint32 arad_ports_lag_lb_key_range_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info);


int
  arad_port_header_type_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE  header_type
  );


uint32
  arad_port_header_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  );


int
  arad_port_header_type_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE  *header_type_incoming,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE  *header_type_outgoing
  );

uint32
  arad_ports_ftmh_extension_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  );


uint32
  arad_ports_ftmh_extension_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  );


uint32
  arad_ports_ftmh_extension_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT ARAD_PORTS_FTMH_EXT_OUTLIF *ext_option
  );


int
  arad_ports_otmh_extension_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     tm_port,
    SOC_SAND_IN  ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  );


uint32
  arad_ports_otmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  );



uint32
  arad_port_egr_hdr_credit_discount_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );



uint32
  arad_port_egr_hdr_credit_discount_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );



uint32
  arad_port_egr_hdr_credit_discount_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );



uint32
  arad_port_egr_hdr_credit_discount_select_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  );



uint32
  arad_port_egr_hdr_credit_discount_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  );



uint32
  arad_port_egr_hdr_credit_discount_select_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  );

uint32 arad_port_stacking_info_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd);

uint32 arad_port_stacking_info_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd);

uint32 arad_port_stacking_info_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_OUT  uint32                              *is_stacking,
    SOC_SAND_OUT  uint32                              *peer_tmd);

uint32 arad_port_stacking_route_history_bitmap_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                           tm_port,
    SOC_SAND_IN  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN  uint32                              bitmap);


int
  arad_port_pp_port_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     pp_port,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO     *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );

uint32
  arad_port_pp_port_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO     *info
  );


int
  arad_port_pp_port_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     pp_port,
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO     *info
  );

uint32
  arad_port_pp_port_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx
  );


int
  arad_port_to_pp_port_map_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION        direction_ndx
  );

int
  arad_port_to_pp_port_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  );

uint32
  arad_port_to_pp_port_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 port_ndx
  );


uint32
  arad_port_to_pp_port_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  );

uint32
  arad_ports_fap_and_nif_type_match_verify(
    SOC_SAND_IN ARAD_INTERFACE_ID if_id,
    SOC_SAND_IN ARAD_FAP_PORT_ID  fap_port_id
  );

int
  arad_ports_fap_port_id_cud_extension_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID   port_id
  );


uint32
  arad_ports_init_interfaces_context_map(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN ARAD_INIT_PORTS   *info
  );



uint32
  arad_ports_init_interfaces_dynamic_context_map(
    SOC_SAND_IN int         unit
  );



uint32
  arad_ports_init_interfaces_erp_setting(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN ARAD_INIT_PORTS   *info
  );

uint32
  ARAD_PORT_PP_PORT_INFO_verify(
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO *info
  );

soc_error_t arad_port_prbs_tx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t arad_port_prbs_tx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t arad_port_prbs_rx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t arad_port_prbs_rx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t arad_port_prbs_rx_status_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t arad_port_prbs_tx_invert_data_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t arad_port_prbs_tx_invert_data_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);


soc_error_t 
arad_port_speed_max(int unit, soc_port_t port, int *speed);
  

uint32
  alloc_reassembly_context_and_recycle_channel_unsafe(
    int unit,
    int core_id,
    uint32 out_pp_port,
    uint32 *channel,
    uint32 *reassembly_ctxt
  );


uint32
  release_reassembly_context_and_mirror_channel_unsafe(
    int unit,
    int core,
    uint32 out_pp_port,
    uint32 channel
  );


uint32
  arad_port_ingr_map_write_val_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_IN  uint8            is_mapped,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO   *map_info
  );


uint32
  arad_port_ingr_reassembly_context_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN soc_port_t  port,
    SOC_SAND_OUT uint32     *port_termination_context,
    SOC_SAND_OUT uint32     *reassembly_context
  );




uint32 
  arad_port_direct_lb_key_set( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 local_port,
    SOC_SAND_IN uint32 min_lb_key,
    SOC_SAND_IN uint32 set_min,
    SOC_SAND_IN uint32 max_lb_key,
    SOC_SAND_IN uint32 set_max
   );

uint32 
  arad_port_direct_lb_key_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint32 *min_lb_key,
    SOC_SAND_OUT uint32 *max_lb_key
  );



uint32 
  arad_port_direct_lb_key_min_set_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 min_lb_key
   );

uint32 
  arad_port_direct_lb_key_max_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 max_lb_key
   );

uint32 
  arad_port_direct_lb_key_min_get_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    uint32* min_lb_key
    );

uint32 
  arad_port_direct_lb_key_max_get_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    uint32* max_lb_key
    );



uint32 
  arad_port_direct_lb_key_set_verify(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 lb_key
   );

uint32 
  arad_port_direct_lb_key_get_verify(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port
    );

#ifdef BCM_88660_A0

uint32
   arad_port_synchronize_lb_key_tables_at_egress_unsafe(
   SOC_SAND_IN int unit
    );



uint32
  arad_port_switch_lb_key_tables_unsafe(
     SOC_SAND_IN int unit
    );

#endif 



uint32 arad_ports_swap_set_verify(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_IN ARAD_PORTS_SWAP_INFO     *ports_swap_info);


uint32 arad_ports_swap_set_unsafe(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_IN ARAD_PORTS_SWAP_INFO     *ports_swap_info);


uint32 arad_ports_swap_get_verify(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_IN ARAD_PORTS_SWAP_INFO     *ports_swap_info);


uint32 arad_ports_swap_get_unsafe(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_OUT ARAD_PORTS_SWAP_INFO    *ports_swap_info);


uint32 arad_swap_info_set_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info);


uint32 arad_swap_info_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info);


uint32 arad_swap_info_get_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info);


uint32 arad_swap_info_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_OUT ARAD_SWAP_INFO     *swap_info);

uint32
  arad_ports_pon_tunnel_info_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  );

uint32
  arad_ports_pon_tunnel_info_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  );

uint32
  arad_ports_pon_tunnel_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  );

uint32
  arad_ports_pon_tunnel_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  );

uint32
arad_port_encap_config_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN ARAD_L2_ENCAP_INFO       *info
    );

uint32
arad_port_encap_config_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_L2_ENCAP_INFO       *info
    );

uint32
arad_port_encap_config_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN ARAD_L2_ENCAP_INFO       *info
    );

uint32
soc_arad_ports_stop_egq(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  soc_port_t     port,
    SOC_SAND_OUT uint32     enable);

int
soc_arad_port_control_tx_nif_enable_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int        enable);

int
soc_arad_port_control_tx_nif_enable_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int        *enable);

int
  arad_port_rx_enable_get(
   int                         unit,
    soc_port_t                  port_ndx,
    int                       *enable
  );

int
  arad_port_rx_enable_set(
   int                         unit,
    soc_port_t                 port_ndx,
    int                        enable
  );

uint32
  arad_ports_programs_info_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  );

uint32
  arad_ports_programs_info_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  );

uint32
  arad_ports_programs_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  );

uint32
  arad_ports_programs_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  );

uint32 
  arad_port_rate_egress_pps_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
    );

uint32 
  arad_port_rate_egress_pps_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
    );


uint32 
  arad_ports_application_mapping_info_set_unsafe (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    );

uint32
  arad_ports_application_mapping_info_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info   
  );

uint32 
  arad_ports_application_mapping_info_get_unsafe (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_INOUT ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    );

uint32
  arad_ports_application_mapping_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN ARAD_FAP_PORT_ID          port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    );

int
  arad_ports_header_type_update(
      int                   unit,
      soc_port_t            port
   );



int
  arad_ports_reference_clock_set(
      int                   unit,
      soc_port_t            port
   );

int
  arad_ports_port_to_nif_id_get(
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   int                   core,
      SOC_SAND_IN   uint32                tm_port,
      SOC_SAND_OUT  ARAD_INTERFACE_ID     *if_id
   );

int
  arad_ports_extender_mapping_enable_set(
      int                   unit,
      soc_port_t            port,
      int                   value
   );

int
  arad_ports_extender_mapping_enable_get(
      int                   unit,
      soc_port_t            port,
      int                   *value
   );

int
  arad_pon_port_enable_set(
      int                   unit,
      soc_port_t            port,
      int                   value
   );

int
  arad_pon_port_enable_get(
      int                   unit,
      soc_port_t            port,
      int                   *value
   );


int
  arad_ports_tm_port_var_set(
      int                   unit,
      soc_port_t            port,
      int                   value
   );

int
  arad_ports_tm_port_var_get(
      int                   unit,
      soc_port_t            port,
      int                   *value
   );

int
  arad_ports_mirrored_channel_and_context_map(int unit, 
                                            int core, 
                                            uint32 termination_context,
                                            uint32 reassembly_context, 
                                            uint32 channel
                                            );

int 
  get_recycling_port(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id,
     SOC_SAND_IN uint32  pp_port, 
     SOC_SAND_IN uint32 channel, 
     SOC_SAND_OUT soc_port_t *logical_port
     );

int
arad_ports_mirror_to_rcy_port_get(
      SOC_SAND_IN  int        unit,
      SOC_SAND_IN  int        core,
      SOC_SAND_IN  uint32     pp_port,
      SOC_SAND_OUT soc_port_t *rcy_mirror_port
   );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


