	/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <bcm_int/dpp/gport_mgmt.h> 





STATIC int
_bcm_dpp_sw_db_gport_type_to_string(int unit, int type, char* gport_type_name) {
    if (type ==_bcmDppGportResolveTypeInvalid) sal_strncpy(gport_type_name, "INVALID", 50);
    else if (type ==_bcmDppGportResolveTypeProtectedFec) sal_strncpy(gport_type_name, "FEC", 50);
    else if (type ==_bcmDppGportResolveTypeForwardFec) sal_strncpy(gport_type_name, "FEC", 50);
    else if (type ==_bcmDppGportResolveTypeAC) sal_strncpy(gport_type_name, "AC", 50);
    else if (type ==_bcmDppGportResolveTypePhy) sal_strncpy(gport_type_name, "PHYSICAL PORT", 50);
    else if (type ==_bcmDppGportResolveTypeFecVC) sal_strncpy(gport_type_name, "FEC VC", 50);
    else if (type ==_bcmDppGportResolveTypePhyEEP) sal_strncpy(gport_type_name, "EEP", 50);
    else if (type ==_bcmDppGportResolveTypeFecEEP) sal_strncpy(gport_type_name, "FEC EEP", 50);
    else if (type ==_bcmDppGportResolveTypeMimMP) sal_strncpy(gport_type_name, "MIM MP", 50);
    else if (type ==_bcmDppGportResolveTypeTrillMC) sal_strncpy(gport_type_name, "TRILL MC", 50);
    else if (type ==_bcmDppGportResolveTypeMimP2P) sal_strncpy(gport_type_name, "MIM P2P", 50);
    else if (type ==_bcmDppGportResolveTypeIPTunnel) sal_strncpy(gport_type_name, "IP TUNNEL", 50);
    else if (type ==_bcmDppGportResolveTypeL2Gre) sal_strncpy(gport_type_name, "L2 GRE", 50);
    else if (type ==_bcmDppGportResolveTypeL2GreEgFec) sal_strncpy(gport_type_name, "GRE EG FEC", 50);
    else if (type ==_bcmDppGportResolveTypeVxlan) sal_strncpy(gport_type_name, "VXLAN", 50);
    else if (type ==_bcmDppGportResolveTypeVxlanEgFec) sal_strncpy(gport_type_name, "VXLAN EG FEC", 50);
    else if (type ==_bcmDppGportResolveTypeRing) sal_strncpy(gport_type_name, "RING", 50);
    else if (type ==_bcmDppGportResolveTypeFecEEI) sal_strncpy(gport_type_name, "FEC EEI", 50);
    else if (type ==_bcmDppGportResolveTypeMimMPNoLearn) sal_strncpy(gport_type_name, "MIM MP NO LEARN", 50);
    else sal_strncpy(gport_type_name, "INVALID", 50);

    return BCM_E_NONE;
}






int 
_bcm_dpp_sw_db_hash_vlan_print(int unit, _BCM_GPORT_PHY_PORT_INFO *data) {
    char type_name[50];
    int ret; 

    /* get the type */
    ret = _bcm_dpp_sw_db_gport_type_to_string(unit, data->type, type_name);
    if (ret != BCM_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
                            "_bcm_dpp_sw_db_gport_type_to_string failed")));
        return BCM_E_FAIL;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nforwarding database info: \n")));
    LOG_CLI((BSL_META_U(unit,
                        "   type:      %s\n"), type_name));
    LOG_CLI((BSL_META_U(unit,
                        "   phy_gport: 0x%x\n"), data->phy_gport));
    LOG_CLI((BSL_META_U(unit,
                        "   encap_id:  0x%x\n"),data->encap_id));

    return BCM_E_NONE;
}


int 
_bcm_dpp_lif_match_print(int unit, int lif, char* criteria_type, char* flag_type) {

    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int ret; 

    ret = _bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    if (ret != BCM_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
                            "_bcm_dpp_local_lif_to_sw_resources failed")));
        return BCM_E_FAIL;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nlif database info: \n")));
    LOG_CLI((BSL_META_U(unit,
                        "   criteria:           0x%x %s \n"), gport_sw_resources.in_lif_sw_resources.criteria, criteria_type));
    LOG_CLI((BSL_META_U(unit,
                        "   flags:              0x%x %s \n"), gport_sw_resources.in_lif_sw_resources.flags, flag_type));
    LOG_CLI((BSL_META_U(unit,
                        "   port:               0x%x\n"), gport_sw_resources.in_lif_sw_resources.port));
    LOG_CLI((BSL_META_U(unit,
                        "   match1:             %d\n"), gport_sw_resources.in_lif_sw_resources.match1));
    LOG_CLI((BSL_META_U(unit,
                        "   match2:             %d\n"), gport_sw_resources.in_lif_sw_resources.match2));
    LOG_CLI((BSL_META_U(unit,
                        "   match_tunnel_value: %d\n"), gport_sw_resources.in_lif_sw_resources.match_tunnel));
    LOG_CLI((BSL_META_U(unit,
                        "   match_ethertype:    %d\n"), gport_sw_resources.in_lif_sw_resources.match_ethertype));
    LOG_CLI((BSL_META_U(unit,
                        "   port_id:            0x%x\n"), gport_sw_resources.in_lif_sw_resources.gport_id));
    LOG_CLI((BSL_META_U(unit,
                        "   key1:               %d\n"), gport_sw_resources.in_lif_sw_resources.key1));
    LOG_CLI((BSL_META_U(unit,
                        "   peer_gport:         0x%x\n"), gport_sw_resources.in_lif_sw_resources.peer_gport));
    LOG_CLI((BSL_META_U(unit,
                        "   learn_gport_id:     0x%x\n"), gport_sw_resources.in_lif_sw_resources.learn_gport_id));
    LOG_CLI((BSL_META_U(unit,
                        "   tpid_profile_type:  %d\n"), gport_sw_resources.in_lif_sw_resources.tpid_profile_type));
    LOG_CLI((BSL_META_U(unit,
                        "   vsi:                %d\n"), gport_sw_resources.in_lif_sw_resources.vsi));

    return BCM_E_NONE;
}


int
_bcm_dpp_lif_ac_match_print(int unit, int lif) {
    return _bcm_dpp_lif_match_print(unit, lif, "(BCM_VLAN_PORT_MATCH_XXX)", "(BCM_VLAN_PORT_XXX)");
}

int
_bcm_dpp_lif_mpls_match_print(int unit, int lif) {
    return _bcm_dpp_lif_match_print(unit, lif, "(BCM_MPLS_PORT_MATCH_XXX)", "(BCM_MPLS_PORT_XXX)");
}
