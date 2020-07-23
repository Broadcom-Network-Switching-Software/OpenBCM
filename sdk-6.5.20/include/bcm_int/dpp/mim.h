/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B Layer 2 Management
 */

#ifndef   _BCM_INT_DPP_MIM_H_
#define   _BCM_INT_DPP_MIM_H_

#include <bcm_int/dpp/error.h>

#include <bcm/mim.h>
#include <bcm/types.h>

#include <soc/dpp/drv.h>


/* 
 * Defines
 */

#define BCM_PETRA_MIM_VPN_INVALID(_unit_) ((SOC_IS_JERICHO_A0(_unit_) | SOC_IS_JERICHO_B0(_unit_) | SOC_IS_QMX(_unit_)) ? 0x7fff: 0xffff)

#define BCM_PETRA_MIM_BTAG_TPID    (0x81a8)
#define BCM_PETRA_MIM_ITAG_TPID    (0x88e7)

#define BCM_PETRA_MIM_BVID_MC_GROUP_BASE    (12*1024)

#define MIM_ACCESS sw_state_access[unit].dpp.bcm.mim

/* Defines for support of SW State to hold INGRESS/EGRESS only status for each VPN */
#define _BCM_DPP_MIM_VPN_FLAGS_SET(_is_ingress, _is_egress, _is_valid, _isid) ((_isid << 8) | (_is_valid << 2) | (_is_egress << 1) | _is_ingress)
#define _BCM_DPP_MIM_VPN_BSA_NICKNAME_SET(_is_valid, _bsa_nickname) ((_is_valid << 20) | (_bsa_nickname & 0xFFFFF))
#define _BCM_DPP_MIM_VPN_ISID_GET(_val) ((_val >> 8) & 0xFFFFFF)
#define _BCM_DPP_MIM_VPN_IS_EGRESS(_val) ((_val >> 1) & 0x1)
#define _BCM_DPP_MIM_VPN_IS_INGRESS(_val) (_val & 0x1)
#define _BCM_DPP_MIM_VPN_IS_VALID(_val) ((_val >> 0x2) & 0x1)
#define _BCM_DPP_MIM_VPN_BSA_NICKNAME_GET(_val) (_val & 0xFFFFF)
#define _BCM_DPP_MIM_VPN_IS_BSA_NICKNAME_VALID(_val) ((_val >> 20) & 0x1)
#define _BCM_DPP_MIM_NUM_OF_VSI (32*1024)
/*
 * MiM Module Helper functions
 */

uint8
  __dpp_mim_initialized_get(int unit);

SOC_PPC_LIF_ID
  __dpp_mim_lif_ndx_get(int unit);

SOC_PPC_AC_ID
  __dpp_mim_global_out_ac_get(int unit);


int dpp_mim_set_global_mim_tpid(int unit, uint16 tpid);
int dpp_mim_get_global_mim_tpid(int unit, uint16 *tpid);



int _bcm_dpp_in_lif_mim_match_add(int unit, bcm_mim_port_t *mim_port, int lif);
int _bcm_dpp_in_lif_mim_match_get(int unit, bcm_mim_port_t *mim_port, int lif);

/* 
 * Macros
 */

#define MIM_INIT(unit)                                    \
    do {                                                  \
        if (!__dpp_mim_initialized_get(unit)) {           \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

#define MIM_IS_INIT(unit)   (__dpp_mim_initialized_get(unit))


typedef struct bcm_dpp_mim_info_s {
    SOC_PPC_LIF_ID    mim_local_lif_ndx; /* default local lif, used only for MiM */
    SOC_PPC_AC_ID     mim_local_out_ac; /* default out-ac, used only for MiM */
    uint32            mim_vpn_flags[_BCM_DPP_MIM_NUM_OF_VSI]; /* INGRESS/EGRESS only flags per VPN and ISID for INGRESS_ONLY VPNs */
    uint32            mim_vpn_bsa_nickname[_BCM_DPP_MIM_NUM_OF_VSI]; /* BSA nicknames for INGRESS_ONLY VPNs */
} bcm_dpp_mim_info_t;


#endif /* _BCM_INT_DPP_MIM_H_ */
     
