/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pon.h
 * Purpose:     PON internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_PON_H_
#define   _BCM_INT_DPP_PON_H_

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/drv.h>


/*
 * MACROs for PON
 */
/* given gport ID is it working port*/
#define _BCM_PPD_GPORT_IS_WORKING_PORT_ID(gport) (((gport)%2) == 0)

/* number of PON ports 
 * For Jericho device & custom_feature_pon_pp_port_mapping_bypass == 1: support 64 PON ports
 * For Jericho device & custom_feature_pon_pp_port_mapping_bypass == 0: support 16 PON ports
 * For Jericho below device: support 8 PON ports
 */
#define _BCM_PPD_NOF_PON_PHY_PORT(unit) (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS_IN_JER(unit) ? 64 : (SOC_IS_JERICHO(unit) ? 16 : 8))


/* default number of PON channel profiles */
#define _BCM_PPD_DEFAULT_NOF_PON_CHANNEL_PROFILE(unit) (_BCM_PPD_NOF_PON_PP_PORT(unit)/_BCM_PPD_NOF_PON_PHY_PORT(unit))

/* number of PON PP ports
 * In case PP PP port mapping is bypass, PON port is equel to PON PP port.
 */
#define _BCM_PPD_NOF_PON_PP_PORT(unit)  (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS_IN_JER(unit) ? _BCM_PPD_NOF_PON_PHY_PORT(unit) : 128)

#define _BCM_PPD_IS_PON_PP_PORT(pon_pp_port, unit) (((pon_pp_port) >= 0) && ((pon_pp_port) < _BCM_PPD_NOF_PON_PP_PORT(unit)))

/* PON channel default profile */
#define _BCM_PPD_PON_CHANNEL_DEFAULT_PROFILE (0)

/* number of PON channel profiles when PON port is channelized */
#define _BCM_PPD_NOF_PON_CHANNEL_PROFILE(unit, nof_channels) (_BCM_PPD_DEFAULT_NOF_PON_CHANNEL_PROFILE(unit)/(1<<((nof_channels)-1)))

/* Default offset of PON channel profiles */
#define _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET(unit) (SOC_IS_JERICHO(unit) ? 4 : 3)

/*  offset of PON channel profiles */
#define _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_OFFSET(unit, nof_channels) (_BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET(unit)+((nof_channels)-1))

/* given PON PP port returns physical port */
#define _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, pon_pp_port) ((pon_pp_port)&(_BCM_PPD_NOF_PON_PHY_PORT(unit)-1))

/* given PON PP port returns local port */
#define _BCM_PPD_GPORT_PON_TO_LOCAL_PORT(pon_pp_port, offset) ((pon_pp_port)&((1<<(offset))-1))

/* given PON PP port returns PON channel profile */
#define _BCM_PPD_GPORT_PON_PP_PORT_TO_CHANNEL_PROFILE(unit, pon_pp_port, offset) (((pon_pp_port)>>(offset))&(_BCM_PPD_DEFAULT_NOF_PON_CHANNEL_PROFILE(unit)-1))

/* given PON port and PON channel profile returns PON PP port */
#define _BCM_PPD_GPORT_PON_CHANNEL_PROFILE_TO_PON_PP_PORT(pon_port, pon_channel_profile, offset) (((pon_channel_profile)<<(offset))|(pon_port)) 


/*
 * Function:
 *      _bcm_dpp_pon_port_is_pon_port
 * Purpose:
 *      Check if the port is PON port
 * Parameters:
 *      unit        - (IN)  Device Number
 *      port        - (IN)  Device PP port Number
 *      is_pon_port - (OUT) TRUE/FALSE
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_port_is_pon_port(int unit, bcm_port_t port, int *is_pon_port);

/*
 * Function:
 *      _bcm_dpp_pon_lif_is_3_tags_data
 * Purpose:
 *      Check if the given lif index is 3 TAGs manipulation.
 * Parameters:
 *      unit           - (IN)  Device Number
 *      out_lif_id     - (IN)  out lif index
 *      is_3_tags_data - (OUT) TRUE/FALSE
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_lif_is_3_tags_data(int unit, int out_lif_id, int *is_3_tags_data);

/*
 * Function:
 *      _bcm_dpp_pon_encap_data_enty_add
 * Purpose:
 *       Add an EEDB entry with PON Tunnel info
 * Parameters:
 *      unit        - (IN)  Device Number
 *      entry_index - (IN)  Data entry index
 *      tpid        - (IN)  Tpid for out Tunnel Tag
 *      tunnel_id   - (IN)  Tunnel_id for out Tunnel Tag
 *      out_ac_id   - (IN)  Out AC index
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_encap_data_enty_add(int unit, uint32 entry_index, uint16 tpid,
    bcm_tunnel_id_t tunnel_id, uint32 out_ac_id);

/*
 * Function:
 *      _bcm_dpp_pon_encap_data_enty_get
 * Purpose:
 *       Get PON Tunnel info of an EEDB entry 
 * Parameters:
 *      unit        - (IN)  Device Number
 *      entry_index - (IN)  Data entry index
 *      tpid        - (OUT) Tpid for out Tunnel Tag
 *      pcp         - (OUT) Pcp for out Tunnel Tag
 *      tunnel_id   - (OUT) Tunnel_id for out Tunnel Tag
 *      out_ac_id   - (OUT) Out AC index
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_encap_data_enty_get(int unit, uint32 entry_index, uint16 *tpid,
    int *pcp, bcm_tunnel_id_t *tunnel_id, int *out_ac_id);

/*
 * Function:
 *      _bcm_dpp_gport_delete_global_and_local_lif
 * Purpose:
 *       Given global and local lif, removes them. If remove_glem_entry is set, removes the glem entry according to global lif as well.
 * Parameters:
 *      unit                - (IN) Device Number
 *      global_lif          - (IN) Global lif to be deallocated.
 *      local_inlif_id      - (IN) Local inlif to be deallocated.
 *      local_outlif_id     - (IN) Local outlif to be deallocated.
 *      remove_glem_entry   - (IN) True will remove Glem entry from HW, false will not.
 *  
 * Returns:
 *      BCM_E_XXX
 */

#endif /* _BCM_INT_DPP_PON_H_ */
