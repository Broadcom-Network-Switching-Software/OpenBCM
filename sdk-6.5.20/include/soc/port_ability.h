/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines common network port modes.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port modes.
 */

#ifndef _SOC_PORTABILITY_H
#define _SOC_PORTABILITY_H

#include <shared/port_ability.h>

typedef _shr_port_ability_t soc_port_ability_t;

#define SOC_PA_ABILITY_ALL     _SHR_PA_ABILITY_ALL
#define SOC_PA_ABILITY_NONE    _SHR_PA_ABILITY_NONE


typedef _shr_pa_encap_t soc_pa_encap_t;

#define SOC_PA_ENCAP_IEEE             _SHR_PA_ENCAP_IEEE 
#define SOC_PA_ENCAP_HIGIG            _SHR_PA_ENCAP_HIGIG 
#define SOC_PA_ENCAP_B5632            _SHR_PA_ENCAP_B5632 
#define SOC_PA_ENCAP_HIGIG2           _SHR_PA_ENCAP_HIGIG2 
#define SOC_PA_ENCAP_HIGIG2_LITE      _SHR_PA_ENCAP_HIGIG2_LITE 
#define SOC_PA_ENCAP_HIGIG2_L2        _SHR_PA_ENCAP_HIGIG2_L2 
#define SOC_PA_ENCAP_HIGIG2_IP_GRE    _SHR_PA_ENCAP_HIGIG2_IP_GRE 
#define SOC_PA_ENCAP_OBSOLETE            _SHR_PA_ENCAP_OBSOLETE
#define SOC_PA_ENCAP_HIGIG_OVER_ETHERNET _SHR_PA_ENCAP_HIGIG_OVER_ETHERNET

#define SOC_PA_ENCAP_STRING          _SHR_PA_ENCAP_STRING


/*
 * Defines:
 *      SOC_PA_SPEED_*
 * Purpose:
 *      Defines for port speeds.
 */
#define SOC_PA_SPEED_10MB         _SHR_PA_SPEED_10MB
#define SOC_PA_SPEED_100MB        _SHR_PA_SPEED_100MB
#define SOC_PA_SPEED_1000MB       _SHR_PA_SPEED_1000MB
#define SOC_PA_SPEED_2500MB       _SHR_PA_SPEED_2500MB
#define SOC_PA_SPEED_3000MB       _SHR_PA_SPEED_3000MB
#define SOC_PA_SPEED_5000MB       _SHR_PA_SPEED_5000MB
#define SOC_PA_SPEED_6000MB       _SHR_PA_SPEED_6000MB
#define SOC_PA_SPEED_10GB         _SHR_PA_SPEED_10GB
#define SOC_PA_SPEED_11GB         _SHR_PA_SPEED_11GB
#define SOC_PA_SPEED_12GB         _SHR_PA_SPEED_12GB
#define SOC_PA_SPEED_12P5GB       _SHR_PA_SPEED_12P5GB
#define SOC_PA_SPEED_13GB         _SHR_PA_SPEED_13GB
#define SOC_PA_SPEED_15GB         _SHR_PA_SPEED_15GB
#define SOC_PA_SPEED_16GB         _SHR_PA_SPEED_16GB
#define SOC_PA_SPEED_20GB         _SHR_PA_SPEED_20GB
#define SOC_PA_SPEED_21GB         _SHR_PA_SPEED_21GB
#define SOC_PA_SPEED_23GB         _SHR_PA_SPEED_23GB
#define SOC_PA_SPEED_24GB         _SHR_PA_SPEED_24GB
#define SOC_PA_SPEED_25GB         _SHR_PA_SPEED_25GB
#define SOC_PA_SPEED_27GB         _SHR_PA_SPEED_27GB
#define SOC_PA_SPEED_30GB         _SHR_PA_SPEED_30GB
#define SOC_PA_SPEED_32GB         _SHR_PA_SPEED_32GB
#define SOC_PA_SPEED_40GB         _SHR_PA_SPEED_40GB
#define SOC_PA_SPEED_42GB         _SHR_PA_SPEED_42GB
#define SOC_PA_SPEED_48GB         _SHR_PA_SPEED_48GB
#define SOC_PA_SPEED_50GB         _SHR_PA_SPEED_50GB
#define SOC_PA_SPEED_53GB         _SHR_PA_SPEED_53GB
#define SOC_PA_SPEED_100GB        _SHR_PA_SPEED_100GB
#define SOC_PA_SPEED_106GB        _SHR_PA_SPEED_106GB
#define SOC_PA_SPEED_120GB        _SHR_PA_SPEED_120GB
#define SOC_PA_SPEED_127GB        _SHR_PA_SPEED_127GB
#define SOC_PA_SPEED_EXTENDED     _SHR_PA_SPEED_EXTENDED

/* NOTE: these speeds are overlayed with 10MB and EXTENDED */
#define SOC_PA_SPEED_200GB        _SHR_PA_SPEED_200GB
#define SOC_PA_SPEED_400GB        _SHR_PA_SPEED_400GB

#define SOC_PA_SPEED_STRING       _SHR_PA_SPEED_STRING

/*
 * Defines:
 *      SOC_PA_PAUSE_*
 * Purpose:
 *      Defines for flow control abilities.
 */
#define SOC_PA_PAUSE_TX        _SHR_PA_PAUSE_TX
#define SOC_PA_PAUSE_RX        _SHR_PA_PAUSE_RX
#define SOC_PA_PAUSE_ASYMM     _SHR_PA_PAUSE_ASYMM

#define SOC_PA_PAUSE_STRING    _SHR_PA_PAUSE_STRING


/*
 * Defines:
 *      SOC_PA_FEC_*
 * Purpose:
 *      Defines for FEC abilities.
 */
#define SOC_PA_FEC_NONE         _SHR_PA_FEC_NONE
#define SOC_PA_FEC_CL74         _SHR_PA_FEC_CL74
#define SOC_PA_FEC_CL91         _SHR_PA_FEC_CL91
#define SOC_PA_FEC_STRING       _SHR_PA_FEC_STRING

/*
 * Defines:
 *      SOC_PA_CHANNEL_*
 * Purpose:
 *      Defines for CHANNEL abilities.
 */
#define SOC_PA_CHANNEL_LONG     _SHR_PA_CHANNEL_LONG
#define SOC_PA_CHANNEL_SHORT    _SHR_PA_CHANNEL_SHORT

/*
 * Defines:
 *      SOC_PA_INTF_*
 * Purpose:
 *      Defines for port interfaces supported.
 */
#define SOC_PA_INTF_TBI        _SHR_PA_INTF_TBI  
#define SOC_PA_INTF_MII        _SHR_PA_INTF_MII  
#define SOC_PA_INTF_GMII       _SHR_PA_INTF_GMII 
#define SOC_PA_INTF_RGMII      _SHR_PA_INTF_RGMII
#define SOC_PA_INTF_SGMII      _SHR_PA_INTF_SGMII
#define SOC_PA_INTF_XGMII      _SHR_PA_INTF_XGMII
#define SOC_PA_INTF_QSGMII     _SHR_PA_INTF_QSGMII
#define SOC_PA_INTF_CGMII      _SHR_PA_INTF_CGMII

#define SOC_PA_INTF_STRING     _SHR_PA_INTF_STRING

/*
 * Defines:
 *      SOC_PA_MEDIUM_*
 * Purpose:
 *      Defines for port medium modes.
 */
#define SOC_PA_MEDIUM_COPPER    _SHR_PA_MEDIUM_COPPER
#define SOC_PA_MEDIUM_FIBER     _SHR_PA_MEDIUM_FIBER
#define SOC_PA_MEDIUM_BACKPLANE _SHR_PA_MEDIUM_BACKPLANE

#define SOC_PA_MEDIUM_STRING   _SHR_PA_MEDIUM_STRING 

/*
 * Defines:
 *      SOC_PA_LOOPBACK_*
 * Purpose:
 *      Defines for port loopback modes.
 */
#define SOC_PA_LB_NONE         _SHR_PA_LB_NONE
#define SOC_PA_LB_MAC          _SHR_PA_LB_MAC
#define SOC_PA_LB_PHY          _SHR_PA_LB_PHY
#define SOC_PA_LB_LINE         _SHR_PA_LB_LINE

#define SOC_PA_LB_STRING       _SHR_PA_LB_STRING

/*
 * Defines:
 *      SOC_PA_FLAGS_*
 * Purpose:
 *      Defines for the reest of port ability flags.
 */
#define SOC_PA_AUTONEG         _SHR_PA_AUTONEG 
#define SOC_PA_COMBO           _SHR_PA_COMBO 


#define SOC_PA_FLAGS_STRING    _SHR_PA_FLAGS_STRING

/* Ability filters  */

#define SOC_PA_PAUSE           _SHR_PA_PAUSE

#define SOC_PA_SPEED_ALL    _SHR_PA_SPEED_ALL

#define SOC_PA_SPEED_MAX(m) _SHR_PA_SPEED_MAX(m)

#define SOC_PA_SPEED_MAX_EXTENDED(m) _SHR_PA_SPEED_MAX_EXTENDED(m)

#define SOC_PA_SPEED(s)     _SHR_PA_SPEED(s) 

#define SOC_PA_IS_HG_SPEED(m)   _SHR_PA_IS_HG_SPEED(m)

#define SOC_PORT_ABILITY_SFI      _SHR_PORT_ABILITY_SFI
#define SOC_PORT_ABILITY_DUAL_SFI _SHR_PORT_ABILITY_DUAL_SFI
#define SOC_PORT_ABILITY_SCI      _SHR_PORT_ABILITY_SCI
#define SOC_PORT_ABILITY_SFI_SCI  _SHR_PORT_ABILITY_SFI_SCI


/* EEE Abilities */
#define SOC_PA_EEE_100MB_BASETX         _SHR_PA_EEE_100MB_BASETX
#define SOC_PA_EEE_1GB_BASET            _SHR_PA_EEE_1GB_BASET
#define SOC_PA_EEE_10GB_BASET           _SHR_PA_EEE_10GB_BASET
#define SOC_PA_EEE_10GB_KX              _SHR_PA_EEE_10GB_KX
#define SOC_PA_EEE_10GB_KX4             _SHR_PA_EEE_10GB_KX4
#define SOC_PA_EEE_10GB_KR              _SHR_PA_EEE_10GB_KR

#define SOC_PA_EEE_STRING               _SHR_PA_EEE_STRING

/* FCMAP Abilities */

#define SOC_PA_FCMAP                  _SHR_PA_FCMAP
#define SOC_PA_FCMAP_FCMAC_LOOPBACK   _SHR_PA_FCMAP_FCMAC_LOOPBACK
#define SOC_PA_FCMAP_AUTONEG          _SHR_PA_FCMAP_AUTONEG
#define SOC_PA_FCMAP_2GB              _SHR_PA_FCMAP_2GB
#define SOC_PA_FCMAP_4GB              _SHR_PA_FCMAP_4GB
#define SOC_PA_FCMAP_8GB              _SHR_PA_FCMAP_8GB
#define SOC_PA_FCMAP_16GB             _SHR_PA_FCMAP_16GB

#define SOC_PA_FCMAP_STRING           _SHR_PA_FCMAP_STRING
/* Port ability functions */
extern int
soc_port_mode_to_ability(soc_port_mode_t mode, soc_port_ability_t *ability);

extern int
soc_port_ability_to_mode(soc_port_ability_t *ability, soc_port_mode_t *mode);

extern int
soc_port_ability_mask(soc_port_ability_t *ability, soc_port_ability_t *mask);

#endif  /* !_SOC_PORTABILITY_H */
