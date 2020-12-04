/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    pon.c
 * Purpose: Manages pon interface
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT

#include <shared/bsl.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/pon.h>


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
int _bcm_dpp_pon_port_is_pon_port(int unit, bcm_port_t port, int *is_pon_port)
{
    if (is_pon_port == NULL) {
        return BCM_E_PARAM;
    }

    *is_pon_port = FALSE;

    if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
        if ((_BCM_PPD_IS_PON_PP_PORT(port, unit)) && (IS_PON_PORT(unit, _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, port)))) {
            *is_pon_port = TRUE;
        }
    }

    return BCM_E_NONE;
}

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
int _bcm_dpp_pon_lif_is_3_tags_data(int unit, int lif_id, int *is_3_tags_data)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type;
    
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(is_3_tags_data);

    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_eg_encap_entry_type_get(soc_sand_dev_id,
                                         lif_id,
                                         &entry_type);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* DATA entry for PON 3 TAGs manipulation */
    if (entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA) {
        *is_3_tags_data = TRUE;
    } else {
        *is_3_tags_data = FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

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
    bcm_tunnel_id_t tunnel_id, uint32 out_ac_id)
{

    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;

    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);

    /* build data entry and fill with PON Tunnel info */
    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
    SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_SET(unit, tpid, tunnel_id, &data_info);
    
    /* add enry to allocated place */
    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, entry_index, &data_info, TRUE, out_ac_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

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
    int *pcp, bcm_tunnel_id_t *tunnel_id, int *out_ac_id)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(tpid);
    BCMDNX_NULL_CHECK(pcp);
    BCMDNX_NULL_CHECK(tunnel_id);
    BCMDNX_NULL_CHECK(out_ac_id);

    soc_sand_dev_id = (unit);

    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "_bcm_dpp_pon_encap_data_enty_get.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0,
                               encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

    *tpid = SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TPID_GET(soc_sand_dev_id, &encap_entry_info[0].entry_val.data_info);

    *tunnel_id = SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(soc_sand_dev_id, &encap_entry_info[0].entry_val.data_info);
    
    *out_ac_id = next_eep[0];

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

