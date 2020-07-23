/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_NIF_INCLUDED__

#define __ARAD_NIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>





#define ARAD_EGQ_IFC_RCY              31
#define ARAD_EGQ_IFC_OAMP             30
#define ARAD_EGQ_IFC_OLP              29
#define ARAD_EGQ_IFC_CPU              28

#define ARAD_EGQ_IFC_DEF_VAL          0
#define ARAD_EGQ_NOF_IFCS             32


#define ARAD_EGQ_NOF_NIFS_IFCS        28


#define ARAD_NIF_ILKN_A_AND_B_NOF_LANES_MIN_CONFLICT                            (22)

#define ARAD_NIF_NOF_CLP_PORTS                           (SOC_MAX_NUM_CLP_BLKS)
#define ARAD_NIF_NOF_XLP_PORTS                           (SOC_MAX_NUM_XLP_BLKS)
#define ARAD_NIF_NOF_XMACS                                (8)

#define ARAD_NIF_NOF_CLP_WCS                             (6)
#define ARAD_NIF_NOF_XLP_WCS                             (2)

#define ARAD_NIF_NOF_STAT_BINS                           (14)

#define ARAD_NIF_CLP_PORT_0_WC_MIN                       (0)
#define ARAD_NIF_CLP_PORT_0_WC_MAX                       (2)
#define ARAD_NIF_CLP_PORT_1_WC_MIN                       (4)
#define ARAD_NIF_CLP_PORT_1_WC_MAX                       (6)
#define ARAD_NIF_CLP_NDX(wc_ndx) \
  ((wc_ndx <= ARAD_NIF_CLP_PORT_0_WC_MAX)?0:(((wc_ndx >= ARAD_NIF_CLP_PORT_1_WC_MIN) && (wc_ndx <= ARAD_NIF_CLP_PORT_1_WC_MAX))?1:ARAD_NIF_NOF_CLP_PORTS))
#define ARAD_NIF_XMAC_NDX_IN_CLP(wc_ndx) \
  (wc_ndx - ((ARAD_NIF_CLP_NDX(wc_ndx) == 0)? ARAD_NIF_CLP_PORT_0_WC_MIN: ARAD_NIF_CLP_PORT_1_WC_MIN))

#define ARAD_NIF_XLP_PORT_0_WC                             (3)
#define ARAD_NIF_XLP_PORT_1_WC                             (7)
#define ARAD_NIF_XLP_NDX(wc_ndx) \
  ((wc_ndx == ARAD_NIF_XLP_PORT_0_WC)?0:((wc_ndx == ARAD_NIF_XLP_PORT_1_WC)?1:ARAD_NIF_NOF_XLP_PORTS))

#define ARAD_NIF_NOF_MAC_TYPES                             (3) 

#define ARAD_NIF_IS_CMAC(nif_type) \
  ((nif_type == ARAD_NIF_TYPE_100G_CGE)? TRUE : FALSE)

#define ARAD_NIF_CLP_NOF_PORTS                             (12)
#define ARAD_NIF_XLP0_NOF_PORTS                             (4)

#define ARAD_NIF_CLP0_PORT_MIN                             (0)
#define ARAD_NIF_CLP1_PORT_MIN                             (ARAD_NIF_CLP0_PORT_MIN + ARAD_NIF_CLP_NOF_PORTS + ARAD_NIF_XLP0_NOF_PORTS)
#define ARAD_NIF_CLP_PORT_NDX(nif_internal_id)  \
  (nif_internal_id - ((ARAD_NIF_CLP_NDX(ARAD_NIF2WC_GLBL_ID(nif_internal_id)) == 0)?ARAD_NIF_CLP0_PORT_MIN:ARAD_NIF_CLP1_PORT_MIN))

#define ARAD_NIF_XLP0_PORT_MIN                             (ARAD_NIF_CLP_NOF_PORTS)
#define ARAD_NIF_XLP1_PORT_MIN                             (ARAD_NIF_XLP0_PORT_MIN + ARAD_NIF_CLP_NOF_PORTS + ARAD_NIF_XLP0_NOF_PORTS)
#define ARAD_NIF_XLP_PORT_NDX(nif_internal_id)  \
  (nif_internal_id - ((ARAD_NIF_XLP_NDX(ARAD_NIF2WC_GLBL_ID(nif_internal_id)) == 0)?ARAD_NIF_XLP0_PORT_MIN:ARAD_NIF_XLP1_PORT_MIN))

#define ARAD_NIF_NUM_OF_LANES_IN_PROTOCOL_GROUP             (24)
#define ARAD_NIF_NUM_OF_OFFSETS_IN_PROTOCOL_GROUP           (2)


uint32
  arad_nif_init(
    SOC_SAND_IN  int                 unit
  );

uint32
  arad_nif_id_verify(
    SOC_SAND_IN ARAD_INTERFACE_ID nif_ndx
  );


uint32 arad_nif_synce_clk_sel_port_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port);

uint32 arad_nif_synce_clk_cfg_num_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num);

uint32 arad_nif_synce_clk_sel_port_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port);

uint32 arad_nif_synce_clk_sel_port_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT soc_port_t                  *port);

uint32 arad_nif_synce_clk_div_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_IN  ARAD_NIF_SYNCE_CLK_DIV   clk_div);

uint32 arad_nif_synce_clk_div_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_OUT  ARAD_NIF_SYNCE_CLK_DIV*   clk_div);

uint32 arad_nif_synce_clk_squelch_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  int             enable);

uint32 arad_nif_synce_clk_squelch_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT int                    *enable);


uint32 arad_nif_synce_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_SYNCE        *info);


uint32
  arad_nif_counter_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t                  port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE       counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT              *counter_val
  );

uint32
  arad_nif_counter_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t                  port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE       counter_type
  );


uint32
  arad_nif_all_counters_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT             counter_val[ARAD_NIF_NOF_COUNTERS]
  );

uint32
  arad_nif_all_counters_ndx_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port
  );


uint32
  arad_nif_all_nifs_all_counters_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counters_val[ARAD_NIF_NOF_NIFS][ARAD_NIF_NOF_COUNTERS]
  );


uint32
  arad_nif_is_channelized(
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_port_t port,
    SOC_SAND_OUT uint8* is_channelized
  );


int
  arad_port2egress_offset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *egr_if
  );


uint32
  arad_nif_channelize_arbiter_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t         port_ndx,
    SOC_SAND_IN  ARAD_INTERFACE_ID  nif_ndx,
    SOC_SAND_IN  uint32             base_q_pair
  );

uint32
  arad_nif_chan_arb_field_val_to_enum(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 chan_arb_field_val,
    SOC_SAND_OUT uint32                 *required_chan_arb_val
  );

uint32
  arad_nif_channelize_arbiter_delete_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  soc_port_t      port_ndx
  );

int
  arad_nif_ipg_get_unsafe(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN soc_port_t        port, 
    SOC_SAND_OUT int              *ipg
  );
int
  arad_nif_ipg_set_unsafe(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN soc_port_t        port, 
    SOC_SAND_IN int               ipg
  );

int
    arad_nif_free_wc_ucode_dma_buf(
        int unit);
uint32
  arad_nif_type_verify(
    SOC_SAND_IN ARAD_NIF_TYPE nif_type
  );

uint32
  arad_port_to_nif_type(
      SOC_SAND_IN int unit,
      SOC_SAND_IN soc_port_t port,
      SOC_SAND_OUT ARAD_NIF_TYPE* nif_type
  );



uint32
  arad_nif2intern_id(
    SOC_SAND_IN  int  unit,
    ARAD_INTERFACE_ID  arad_nif_id
  );

ARAD_INTERFACE_ID
  arad_nif_ilkn_tdm_intern2nif_id(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  ARAD_NIF_TYPE  nif_type,
    SOC_SAND_IN  uint32         internal_id,
    SOC_SAND_IN  uint32         is_tdm
  );


ARAD_INTERFACE_ID
  arad_nif_intern2nif_id(
     SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         internal_id
  );

int
arad_port_protocol_offset_verify(
   SOC_SAND_IN   int                   unit,
   SOC_SAND_IN   soc_port_t            port,
   SOC_SAND_IN   uint32                protocol_offset
   );

int
arad_port_cable_diag(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN soc_port_t              port,
    SOC_SAND_OUT soc_port_cable_diag_t  *status
  );

int arad_port_init(int unit);
int arad_port_deinit(int unit);
int arad_port_post_init(int unit, soc_pbmp_t* ports);
int arad_port_enable_set(int unit, soc_port_t port, uint32 mac_only, int enable);
int arad_port_enable_get(int unit, soc_port_t port, uint32 mac_only, int* enable);
int arad_port_speed_set(int unit, soc_port_t port, int speed);
int arad_port_speed_get(int unit, soc_port_t port, int* speed);
int arad_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf) ;
int arad_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int arad_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down);
int arad_port_is_supported_encap_get(int unit, int mode, int* is_supported);
int arad_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
int soc_ardon_fabric_broadcast_firmware_loader(int unit,  uint32 length, const uint8* data);
int arad_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
int arad_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback);
int arad_port_phy_reset(int unit, soc_port_t port);
int arad_port_phy_control_set(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value);
int arad_port_phy_control_get(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32* value);
int arad_port_phy_reg_get(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 *phy_data);
int arad_port_phy_reg_set(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data);
int arad_port_phy_reg_modify(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask);
int arad_port_link_get(int unit, int port, int* link);
int arad_port_autoneg_set(int unit, soc_port_t port, int autoneg);
int arad_port_autoneg_get(int unit, soc_port_t port, int *autoneg);
int arad_port_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
int arad_port_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int arad_port_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int arad_port_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int arad_port_eee_enable_set(int unit, soc_port_t port, uint32 value);
int arad_port_eee_statistics_clear(int unit, soc_port_t port);
int arad_port_eee_event_count_symmetric_set(int unit, soc_port_t port, uint32 value);
int arad_port_eee_event_count_symmetric_get(int unit, soc_port_t port, uint32 *value);
int arad_port_eee_tx_event_count_get(int unit, soc_port_t port, uint32 *value);
int arad_port_eee_tx_duration_get(int unit, soc_port_t port, uint32 *value);
int arad_port_eee_rx_event_count_get(int unit, soc_port_t port, uint32 *value);
int arad_port_eee_rx_duration_get(int unit, soc_port_t port, uint32 *value);
int arad_port_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
int arad_port_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode);
int arad_port_cntmaxsize_set(int unit, soc_port_t port, int val);
int arad_port_cntmaxsize_get(int unit, soc_port_t port, int *val);
int arad_port_mdix_status_get(int unit, soc_port_t port, soc_port_mdix_status_t *status);
int arad_port_duplex_set(int unit, soc_port_t port, int duplex);
int arad_port_duplex_get(int unit, soc_port_t port, int *duplex);
int arad_port_fault_get(int unit, soc_port_t port, uint32 *flags);
int arad_port_nif_nof_lanes_get(int unit, soc_port_if_t interface, uint32 first_phy_port, uint32 nof_lanes_to_set, uint32 *nof_lanes);
uint32 arad_port_link_up_mac_update(int unit, soc_port_t port, int link);
int arad_port_close_path(int unit, soc_port_t port);
int soc_arad_port_speed_sku_restrictions(int unit, soc_port_t port, int speed);
int soc_arad_nif_sku_restrictions(int unit, soc_pbmp_t phy_pbmp, soc_port_if_t interface, uint32 protocol_offset, uint32 is_kbp);

soc_error_t arad_port_fabric_clk_freq_init(int unit, soc_pbmp_t pbmp);

#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


