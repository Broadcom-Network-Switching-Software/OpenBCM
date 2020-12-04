/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_RX_H_
#define _BCM_INT_DPP_RX_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

/* Defines BCM no action profile */
#define _BCM_DPP_TRAP_EG_NO_ACTION (0)
#define _BCM_DPP_TRAP_PREVENT_SWAPPING (-1)
#define _BCM_DPP_TRAP_INVALID_VIRTUAL_ID (-2)

/*
 *  Egress traps don't really work that way.  The 'trap' action takes a profile
 *  (not a trap) as an argument, and follows the profile action.  Still, need a
 *  way to manage these profiles, but until then, just take the top one.
 */
#define _BCM_PPD_TRAP_MGMT_EG_ACTION_PROFILE_MAX       (7) 
#define _DPP_SWITCH_TRAP_TO_CPU_PROFILE_INDEX          (0x2)
#define _DPP_SWITCH_EG_TRAP_DROP_PROFILE_INDEX         (0x1)
#define _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT                (10)
#define _BCM_DPP_RX_TRAP_ETPP_TYPE_SHIFT               (17)
#define _BCM_DPP_RX_TRAP_ETPP_TYPE_MASK                (1)
#define _BCM_DPP_RX_TRAP_TYPE_MASK                     (0x3)
#define _BCM_DPP_RX_TRAP_ID_MASK                       (0x3ff)
#define _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT              (16)

#define _BCM_NUMBER_OF_VIRTUAL_TRAPS_MC_RESERVED       (8) /*Number of FRWRD profiles in IHB_FWD_ACT_PROFILE for reserved MC traps*/
#define _BCM_NUMBER_OF_VIRTUAL_TRAPS_PROG              (4) /*Number of FRWRD profiles in IHB_FWD_ACT_PROFILE for Prog traps*/
#define _BCM_NUMBER_OF_VIRTUAL_TRAPS                   (_BCM_NUMBER_OF_VIRTUAL_TRAPS_MC_RESERVED + _BCM_NUMBER_OF_VIRTUAL_TRAPS_PROG)



typedef enum
{
  /*
   *  ingress trap.
   */
  _BCM_DPP_RX_TRAP_INGRESS = 0x0,
  /*
   *  TBD trap.
   */
  _BCM_DPP_RX_TRAP_OAMP_ERROR = 0x1,
  /*
   *  egress trap.
   */
  _BCM_DPP_RX_TRAP_EGRESS = 0x2,
  /*
   *  virtual trap.
   */
  _BCM_DPP_RX_TRAP_VIRTUAL = 0x3,
  /*
   *  Number of types in _BCM_DPP_RX_TRAP_TYPE_SIZES
   */
  _BCM_DPP_RX_TRAP_TYPE_SIZES = 0x4
}_BCM_DPP_RX_TRAP_TYPE;

typedef struct bcm_dpp_rx_virtual_traps_s {
    uint32 gport_trap_id;                       /* Virtual trap id. Range: 0-11 */
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO trap_info;   /* Trap info profile */
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_info;  /* Snoop info */
    SOC_PPC_TRAP_CODE soc_ppc_trap_code;    /*For Virtual traps, one of SOC_PPC_TRAP_CODE_RESERVED_MC_X or SOC_PPC_TRAP_CODE_PROG_TRAP_X*/
    uint32            flags;
} bcm_dpp_rx_virtual_traps_t;

typedef struct bcm_dpp_rx_info_s {
    bcm_dpp_rx_virtual_traps_t virtual_traps[_BCM_NUMBER_OF_VIRTUAL_TRAPS];
} bcm_dpp_rx_info_t;

typedef struct _bcm_rx_trap_to_ppd_map_s {
    uint8 valid;
    SOC_PPC_TRAP_CODE ppc_trap_code;
} _bcm_rx_trap_to_ppd_map_t ;

/* gport trap full encoding:
   bits 0:9 trap id
   bits 10:11 virtual/egress symbol
   bits 12:15 trap strength
   bits 16:19 snoop strength
   bits 17:17 is etpp trap
   bits 26:31 trap type
*/
#define _BCM_TRAP_ID_IS_VIRTUAL(_virtual_trap) (((_virtual_trap >> _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) & _BCM_DPP_RX_TRAP_TYPE_MASK) == _BCM_DPP_RX_TRAP_VIRTUAL) /* Bits 11:10 == 0x3 mark the trap is virtual */
#define _BCM_TRAP_ID_IS_EGRESS(_eg_trap)       (((_eg_trap >> _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) & _BCM_DPP_RX_TRAP_TYPE_MASK) == _BCM_DPP_RX_TRAP_EGRESS) /* Bits 11:10 == 0x2 mark the trap is virtual */
#define _BCM_TRAP_ID_IS_OAM(_oam_trap)         (((_oam_trap >> _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) & _BCM_DPP_RX_TRAP_TYPE_MASK) == _BCM_DPP_RX_TRAP_OAMP_ERROR) /* Bits 11:10 == 0x1 mark the trap is oam */
#define _BCM_TRAP_ID_IS_ETPP(_etpp_trap)       ((_etpp_trap >> _BCM_DPP_RX_TRAP_ETPP_TYPE_SHIFT ) & _BCM_DPP_RX_TRAP_ETPP_TYPE_MASK) /* Bit 17 == 0x1 mark the trap is etpp */

/*create egress bit*/         
#define _BCM_TRAP_EG_TYPE_TO_ID(_eg_trap)  ((_BCM_DPP_RX_TRAP_EGRESS << _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) | (((_eg_trap) <= SOC_PPC_TRAP_EG_TYPE_CFM_TRAP)? (_bcm_dpp_rx_trap_egrr_calc(_eg_trap)) : (_eg_trap>>_BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT) + _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT))
#define _BCM_TRAP_EG_TYPE_BITS_TO_ID(_eg_trap_bits)  ((_BCM_DPP_RX_TRAP_EGRESS << _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) | _eg_trap_bits)
#define _BCM_TRAP_VIRTUAL_TO_ID(_virtual_trap)  ((_BCM_DPP_RX_TRAP_VIRTUAL << _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) | _virtual_trap)

/* create etpp bit */
#define _BCM_TRAP_ETPP_TO_ID(_etpp_trap)  (_etpp_trap  | (_BCM_DPP_RX_TRAP_ETPP_TYPE_MASK << _BCM_DPP_RX_TRAP_ETPP_TYPE_SHIFT))

#define _BCM_RX_TRAP_IS_REGULAR(trap_id) ( !(_BCM_TRAP_ID_IS_VIRTUAL(trap_id))  && !(_BCM_TRAP_ID_IS_EGRESS(trap_id)) && !( _BCM_TRAP_ID_IS_OAM(trap_id)) && !( _BCM_TRAP_ID_IS_ETPP(trap_id))) /* not a l2 cache,oam and egress trap */
#define _BCM_RX_EXPOSE_HW_ID(unit)  (SOC_DPP_CONFIG(unit)->dpp_general.use_hw_trap_id_flag)

/* converts the SW trap id to HW trap in the GPORT */
#define _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, gport, rv) \
        do { \
                if (BCM_GPORT_IS_TRAP((gport)) &&  _BCM_RX_EXPOSE_HW_ID(unit) ) \
                { \
                        uint32 trap_id_hw; \
                        int trap_strength; \
                        int snoop_strength; \
                        int trap_id_sw = BCM_GPORT_TRAP_GET_ID((gport));  \
                        if (_BCM_RX_EXPOSE_HW_ID(unit) && _BCM_RX_TRAP_IS_REGULAR(trap_id_sw))   \
                        { \
                            rv = _bcm_dpp_field_trap_ppd_to_hw(unit,trap_id_sw,&trap_id_hw);    /* exposing the hw id*/ \
                            if( rv == BCM_E_NONE){\
                                trap_strength = BCM_GPORT_TRAP_GET_STRENGTH((gport)); \
                                snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH((gport)); \
                                BCM_GPORT_TRAP_SET((gport), trap_id_hw, trap_strength, snoop_strength);      \
                            }\
                        }       \
                } \
          } while (0) 

 /* converts the HW trap id to SW trap in the GPORT */
#define _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, gport) \
        do { \
                if (BCM_GPORT_IS_TRAP((gport)) &&  _BCM_RX_EXPOSE_HW_ID(unit)) \
                { \
                        int rv; \
                        int trap_id_sw; \
                        SOC_PPC_TRAP_CODE ppd_trap_code_temp; \
                        int trap_strength; \
                        int snoop_strength; \
                        int trap_id_hw = BCM_GPORT_TRAP_GET_ID((gport)); \
                        if (_BCM_RX_EXPOSE_HW_ID(unit) && _BCM_RX_TRAP_IS_REGULAR(trap_id_hw)) \
                        { \
                            rv = _bcm_dpp_field_trap_ppd_from_hw(unit,trap_id_hw,&ppd_trap_code_temp); /* converting the HW trap id back to ppd trap code */ \
                            trap_id_sw  = ppd_trap_code_temp; \
                            BCMDNX_IF_ERR_EXIT(rv); \
                            trap_strength = BCM_GPORT_TRAP_GET_STRENGTH((gport)); \
                            snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH((gport)); \
                            BCM_GPORT_TRAP_SET((gport), trap_id_sw, trap_strength, snoop_strength); \
                        } \
                } \
            } while (0)


/*clears virtual bit*/
#define _BCM_TRAP_ID_TO_VIRTUAL(_virtual_trap)  ( _BCM_DPP_RX_TRAP_ID_MASK & _virtual_trap) /*set the bit 11:10 to zero, , total number of bits for traps is 10*/
#define _BCM_TRAP_ID_TO_OAM(_oam_trap)  ( _BCM_DPP_RX_TRAP_ID_MASK & _oam_trap) /*set the bit 11:10 to zero, , total number of bits for traps is 10*/

extern int _packet_counter[BCM_MAX_NUM_UNITS];

int _bcm_dpp_rx_virtual_trap_get(int unit, int virtual_trap_id, bcm_dpp_rx_virtual_traps_t *virtual_trap_str);
int _bcm_dpp_rx_virtual_trap_set(int unit, int virtual_trap_id, bcm_dpp_rx_virtual_traps_t *virtual_trap_str);
int _bcm_dpp_rx_egress_set(int unit,int trap_id, SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *prm_egr_profile_info);
int _bcm_dpp_rx_egress_set_to_drop_info(int unit, SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *eg_drop_info);
int _bcm_dpp_rx_egress_set_to_cpu_drop_info(int unit, SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *eg_cpu_drop_info);
int _bcm_dpp_rx_trap_set(int unit, int trap_id, bcm_rx_trap_config_t *config, uint8 trap_fwd_offset_index);
int _bcm_petra_rx_partial_init(int);
int _bcm_dpp_rx_trap_id_to_egress(int _eg_trap);
int _bcm_dpp_rx_trap_id_to_etpp(int _etpp_trap);
int _bcm_dpp_rx_egress_to_trap_id(int _eg_trap);
int _bcm_dpp_rx_ppd_to_forwarding_type(SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType, bcm_forwarding_type_t *bcmForwardingType);
int _bcm_dpp_rx_forwarding_type_to_ppd(bcm_forwarding_type_t bcmForwardingType, SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType);

int _bcm_dpp_rx_ppd_trap_get(int unit, bcm_rx_trap_t trap_type, int *soc_ppd_trap_id);
void _bcm_dpp_rx_trap_type_get(int unit, uint8 convert_flag, int soc_ppd_trap_id, uint8 *found, bcm_rx_trap_t *trap_type);
void _bcm_dpp_rx_trap_type_get_from_array(int soc_ppd_trap_id, bcm_rx_trap_t *trap_type, uint8 *found);

int _bcm_dpp_rx_packet_parse(int unit, bcm_pkt_t *pkt, uint8 device_access_allowed);
void _bcm_dpp_rx_packet_cos_parse(int unit, bcm_pkt_t *pkt);

int _bcm_rx_ppd_trap_code_from_trap_id(int unit, int trap_id, SOC_PPC_TRAP_CODE *ppd_trap_code);
int _bcm_dpp_rx_trap_sw_id_to_virtual_id(int unit, int trap_id_sw,int *trap_id_virtual);

int _bcm_dpp_rx_trap_sw_id_to_hw_id(int unit, int trap_id_sw, int *trap_id_converted);
int _bcm_dpp_rx_trap_hw_id_to_sw_id(int unit, int trap_id,  int *trap_id_converted);
int _bcm_dpp_rx_bcm_forwarding_header_to_ppd(bcm_rx_trap_forwarding_header_t fwd_header,uint32 *ppd_fwd_header);
int _bcm_dpp_rx_bcm_ppd_to_forwarding_header(uint32 ppd_fwd_header, bcm_rx_trap_forwarding_header_t* fwd_header);
int _bcm_dpp_rx_get_next_eg_soc_ppc_trap(SOC_PPC_TRAP_EG_TYPE current_eg_trap_id,SOC_PPC_TRAP_EG_TYPE *next_eg_trap_id);
void _bcm_dpp_rx_egress_trap_to_soc_ppc_trap(int trap_id_eg, SOC_PPC_TRAP_EG_TYPE *trap_id_ppc);
int _bcm_dpp_rx_trap_egrr_calc(int egrr_trap);

int bcm_petra_rx_detach(int unit);

void rx_adapter_process_packet(int unit, bcm_pkt_t *pkt);
int _bcm_adapter_rx_start(int unit, bcm_rx_cfg_t *cfg);

#endif /* _BCM_INT_DPP_RX_H_ */
