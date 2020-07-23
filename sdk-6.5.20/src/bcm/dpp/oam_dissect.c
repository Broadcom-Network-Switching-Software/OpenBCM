/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam_disect.c
 * Purpose: OAM input analysis and validation
 */

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/oam_dissect.h>
#include <bcm_int/dpp/bfd.h>

#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/oam.h>

#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>


/* perform endpoint create validity check for default endpoint. */
int _bcm_oam_default_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info) {

	BCMDNX_INIT_FUNC_DEFS;

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d)"
                                          " but BCM_OAM_ENDPOINT_WITH_ID flag not set.\n"),
                             (int)(endpoint_info->id)));
    }


    if (endpoint_info->lm_counter_base_id != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d)"
                                          " but lm_counter_base_id is different then 0(invalid).\n"),
                             (int)(endpoint_info->id)));
    }
    if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
        if (endpoint_info->flags & ~(BCM_OAM_ENDPOINT_WITH_ID
                                     | BCM_OAM_ENDPOINT_REPLACE
                                     | BCM_OAM_ENDPOINT_UP_FACING)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d)"
                                              " but forbidden flags were used.\n"),
                                 (int)(endpoint_info->id)));
        }
        if (_BCM_OAM_DISSECT_IS_DEFAULT_ENDPOINT_UPMEP(endpoint_info)) {
            if (!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: default egress endpoint must face up\n")));
            }
        } else if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: default ingress endpoint must face down\n")));
        }
    }
    if ((SOC_IS_ARAD_B1_AND_BELOW(unit))
        && (endpoint_info->flags != BCM_OAM_ENDPOINT_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d)"
                                          " but forbidden flags were used.\n"),
                             (int)(endpoint_info->id)));
    }

    /* check field limitations */
    if (endpoint_info->ccm_period
        || endpoint_info->int_pri
        || endpoint_info->vlan
        || endpoint_info->inner_pkt_pri
        || endpoint_info->inner_tpid
        || endpoint_info->inner_vlan
        || endpoint_info->outer_tpid
        || !BCM_MAC_IS_ZERO(endpoint_info->src_mac_address)
        || !BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address)
        || endpoint_info->name
        || endpoint_info->type
        || endpoint_info->group
        || endpoint_info->opcode_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d)"
                                          " but other fields are set.\n"),(int)(endpoint_info->id)));
    }
    /* Extra limitations for Arad */
    else if (SOC_IS_ARAD_B1_AND_BELOW(unit) && (endpoint_info->level)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == -1)"
                                          " but other fields are set.\n")));
    }

    if (_BCM_OAM_IS_MEP_ID_EGRESS_UNSUPPORTED_DEFAULT(unit, endpoint_info->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                            (_BSL_BCM_MSG("Error: Egress default endpoints are not supported.\n")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks for all types of non-default endpoints */
int _bcm_oam_generic_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info) 
{
    int rv = 0;
    int correct_flags = 0, correct_flags2 = 0;
    uint8 is_accelerated = 0;
    uint8 is_upmep;
    uint8 found;
    _bcm_oam_ma_name_t ma_name_struct;

    BCMDNX_INIT_FUNC_DEFS;

    correct_flags =  BCM_OAM_ENDPOINT_REPLACE | BCM_OAM_ENDPOINT_REMOTE;
    is_accelerated = _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info);
    is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);

    if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info)) {
        /** In case of RFC6374 the accelerated meps must have tx_gport different then invalid */
        is_accelerated = endpoint_info->tx_gport != BCM_GPORT_INVALID ? 1 : 0;
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) {
        correct_flags |= BCM_OAM_ENDPOINT_WITH_ID;
    }


    if (((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) || is_accelerated)
        && SOC_IS_ARADPLUS(unit)
        && endpoint_info->type == bcmOAMEndpointTypeEthernet) {
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
            if ((endpoint_info->port_state > (BCM_OAM_PORT_TLV_UP
                                             ))
               || (endpoint_info->port_state < BCM_OAM_PORT_TLV_BLOCKED)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: flag status BCM_OAM_ENDPOINT_PORT_STATE_UPDATE"
                                                  " does not match port_state field.")));
            }
        } else if (endpoint_info->port_state) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: flag status BCM_OAM_ENDPOINT_PORT_STATE_UPDATE"
                                              " does not match port_state field.")));
        }
        if ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) ?
            (endpoint_info->interface_state < BCM_OAM_INTERFACE_TLV_UP
             || endpoint_info->interface_state > BCM_OAM_INTERFACE_TLV_LLDOWN) :
            endpoint_info->interface_state) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: flag status BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE"
                                              " does not match interface_state field.")));
        }
        correct_flags |=  BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE | BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
    } else {
        if (endpoint_info->port_state || endpoint_info->interface_state) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: port_state, interface_state"
                                              " only available in Jericho and for"
                                              " accelerated/remote endpoints.")));

        }
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
        /*remote endpoint. additional flags allowed.*/
        correct_flags |=   BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE
            | BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE | BCM_OAM_ENDPOINT_WITH_ID;
        if (SOC_IS_ARADPLUS(unit)) {
            correct_flags2 |= (BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE
                               | BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE
                               | BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE
                               | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI
                               | BCM_OAM_ENDPOINT_FLAGS2_RDI_CLEAR_ON_LOC_CLEAR
                               | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC);
            if  (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE &&
                 ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE)
                  || (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE"
                                                  " flag is set BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE,"
                                                  " BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE flags may not be set")));
            }

            if  (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE &&
                 ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE)
                  || (endpoint_info->flags2 & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE"
                                                  " flag is set BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE,"
                                                  " BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE flags may not be set")));
            }

            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE)
                && (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE"
                                                  " is set, BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE flag may not be set.\n")));

            }

            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE)
                && (endpoint_info->flags & BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE"
                                                  " is set, BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE"
                                                  " flag may not be set.\n")));

            }

            if ((endpoint_info->sampling_ratio < 0)
                || (endpoint_info->sampling_ratio > 8)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Sampling ratio can only be 0-8.\n")));
            }
        }
        /* Disabling creation of remote MEP with id 0 for Jericho and above devices */
        if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) && SOC_IS_JERICHO(unit)) { 

            uint32 internal_id = 0;

            if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info->id)) {
                _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info->id, internal_id);
            }

            if(0 == (internal_id - (1 << _BCM_OAM_REMOTE_MEP_INDEX_BIT))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                        (_BSL_BCM_MSG("Error: remote endpoint with id %d is not allowed.\n"), endpoint_info->id));
            }
        }
    } else {
        /*local endpoint*/
        correct_flags |= BCM_OAM_ENDPOINT_INTERMEDIATE | BCM_OAM_ENDPOINT_UP_FACING;

        if (is_upmep && endpoint_info->type != bcmOAMEndpointTypeEthernet) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Up MEP supported only for type"
                                              " Ethernet OAM.\n")));
        }

        if (is_accelerated) {
            /*Accelerated endpoint. TX fields must be correct as well */
            if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
                if ((endpoint_info->inner_tpid == 0)
                    && (endpoint_info->inner_pkt_pri)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: inner_vlan is not set but"
                                                      " inner_pkt_pri or inner_tpid are set\n")));
                }
                if ((endpoint_info->gport != BCM_GPORT_INVALID)
                    && BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: Can't create accelerated"
                                                      " OAMoETHoPWE endpoint\n")));
                }
                correct_flags2 |= BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN;
                if (endpoint_info->pkt_pri > 0xf || endpoint_info->inner_pkt_pri > 0xf) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("pkt_pri, inner_pkt_pri may not be greater than 0xf (PCP, DEI are 4 bits total) \n")));
                }
            } else if (endpoint_info->type == bcmOAMEndpointTypeBhhSection) {
                if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) {
                    if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: mpls_out_gport should be invalid\n")));
                    }
                }
                if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY) {
                    if (endpoint_info->gport != BCM_GPORT_INVALID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: gport should be invalid\n")));
                    }
                }
                if (endpoint_info->intf_id <= 0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: LL interface should be greater than zero.\n")));
                 }
                correct_flags2 |= (BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY|BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY);
            } else {
                if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) {
                    if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: mpls_out_gport should be invalid\n")));
                    }
                }
                if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY) {
                    if (endpoint_info->gport != BCM_GPORT_INVALID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: gport should be invalid\n")));
                    }
                }
                /* presumably MPLS or PWE. */
                if (((endpoint_info->intf_id <= 0) ||
                     (endpoint_info->egress_label.label == 0) ||
                     (endpoint_info->egress_label.ttl == 0)) && !(endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: MPLS label fields must be filled.\n")));
                }
                correct_flags2 |= (BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY|BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY);
                if(SOC_DPP_CONFIG(unit)->pp.oam_use_double_outlif_injection){
                    correct_flags2 |= BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ;
                } else {
                    if (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ){
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ may not be used without soc property - oam_use_double_outlif_injection .\n")));
                    }
                }
            }

            correct_flags |= BCM_OAM_ENDPOINT_WITH_ID;
            correct_flags |= BCM_OAM_ENDPOINT_RDI_TX;
            correct_flags2 |= (BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE | BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE)
                                * SOC_IS_ARADPLUS(unit);

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX
                && endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("When the flag BCM_OAM_ENDPOINT_RDI_TX is set,"
                                                  " the flag2 BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE"
                                                  " must not be set. ")));
            }
            /* handling the gport, tx-gport.*/
            if (is_upmep) {
                /*For up MEPs the tx-gport is not used. Should be invalid.*/
                if (endpoint_info->tx_gport != BCM_GPORT_INVALID && !_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("For accelerated up-MEPs tx_gport must be invalid")));
                }
                if (endpoint_info->gport == BCM_GPORT_INVALID && !_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("For accelerated up-MEPs gport may not be invalid.")));
                }
                if (SOC_IS_ARAD_B1_AND_BELOW(unit) && (endpoint_info->gport != BCM_GPORT_INVALID)) {
                    /* In Arad the MEP_DB.local_port goes on the PTCH and acts as the LSB of the src mac address. Verify that these values are in fact identical */
                    /* For server the value on the tx_gport represents the SSP on the inner PTCH, which should be SSP in remote device*/
                    _bcm_dpp_gport_info_t gport_info;
                    rv = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport,
                                                    _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (gport_info.local_port != endpoint_info->src_mac_address[5]) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: local port does not match LSB of src MAC address."
                                                          " Local port is: 0x%x, LSB of MAC address is 0x%x\n"),
                                             gport_info.local_port,  endpoint_info->src_mac_address[5]));
                    }

                }

            } else if (SOC_IS_ARAD_A0(unit)
                       && endpoint_info->tx_gport != BCM_GPORT_INVALID
                       && endpoint_info->type == bcmOAMEndpointTypeEthernet) {
                /* In Arad A0 the system port goes on the MEP_DB.local_port,
                   from which the LSB the the src mac address is taken.*/
                _bcm_dpp_gport_info_t gport_info;
                rv = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->tx_gport,
                                                _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                BCMDNX_IF_ERR_EXIT(rv);
                if ((0xff & gport_info.sys_port) != endpoint_info->src_mac_address[5]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: system port does not match LSB of src MAC address."
                                                      " System port is: 0x%x, LSB of MAC address is 0x%x\n"),
                                         0xff & gport_info.sys_port,  endpoint_info->src_mac_address[5]));
                }
            }

            if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: OAMP server - client side. Endpoint may not be accelerated.")));
            }

            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {

                if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server, Only supported for Arad+ and above.")));
                }
                if (!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) && !BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server, down MEP - remote_gport must"
                                                      " be set to the trap configured on client device"
                                                      " (trap destination should be server OAMP).")));
                }
                if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)
                    && !BCM_GPORT_IS_MODPORT(endpoint_info->remote_gport)
                    && !BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->remote_gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server, up MEP - remote_gport must"
                                                      " be a MODPORT (remote device's recycle port).")));
                }
            }

            /* Disabling creation of local MEP with id 0 for Jericho and above devices */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) && SOC_IS_JERICHO(unit)) { 
                if(0 == endpoint_info->id) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Local endpoint with id %d is not allowed.\n"), endpoint_info->id));
                }
            }

        } else {
            /*Non accelerated endpoint. Only RX fields should be filled.*/
            if (endpoint_info->ccm_period
                || endpoint_info->int_pri
                || endpoint_info->vlan
                || endpoint_info->inner_pkt_pri
                || endpoint_info->pkt_pri
                || endpoint_info->name
                || endpoint_info->intf_id
                || endpoint_info->egress_label.label!=BCM_MPLS_LABEL_INVALID
                || endpoint_info->egress_label.ttl
                || endpoint_info->egress_label.exp
                || endpoint_info->inner_tpid
                || endpoint_info->inner_vlan
                || endpoint_info->outer_tpid
                || !BCM_MAC_IS_ZERO(endpoint_info->src_mac_address)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Non acceleration in HW"
                                                  " marked but TX fields non zero.\n")));

            }
            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: OAMP server. Endpoint must be accelerated.")));
            }
            if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                if (endpoint_info->tx_gport != BCM_GPORT_INVALID) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server - client side."
                                                      " tx_port must be set to BCM_GPORT_INVALID.")));
                }
                if (!BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP server - Client's endpoint remote_gport field must be a trap.\n")));
                }
                if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) == 0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server - client side must"
                                                      " be set with the _WITH_ID flag, ID being"
                                                      " that of the server.")));
                }
                if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
                    correct_flags |= BCM_OAM_ENDPOINT_WITH_ID;
                }
            }
        }

        if (endpoint_info->opcode_flags & ~BCM_OAM_OPCODE_CCM_IN_HW) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: invalid opcode flag is set.\n")));
        }
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)
            && _BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: OAMP server supported only for Arad+ and above.")));
        }
    }

    if ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS) ||
        (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
        (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
        (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL) ||
        (endpoint_info->type == bcmOAMEndpointTypeBhhSection)) {
        correct_flags2 |= (BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY|BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY);
    }

    if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info)) {
        correct_flags2 |= BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM;
    }

    /*
     * Check the correct flags
     */
    if (endpoint_info->flags & ~correct_flags) {

            /* Endpoint replace */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) && !(endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: To replace Endpoint use flag BCM_OAM_ENDPOINT_WITH_ID \n")));
            }

            /* RDI_Auto_update for Local MEPs */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE) && !(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Only remote endpoints may use BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE \n")));
            }

            /* Remote_event_disable for Local MEPs */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) && !(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Only remote endpoints may use BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE \n")));
            }

            /* Endpoint intermediate for Remote MEPs */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) && (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Remote endpoints can not use BCM_OAM_ENDPOINT_INTERMEDIATE \n")));
            }

            /* Endpoint intermediate for Remote MEPs */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) && (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Remote endpoints can not use BCM_OAM_ENDPOINT_UP_FACING \n")));
            }

            /* RDI_TX for non-acc MEPs */
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) && !(is_accelerated)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Only accelerated local Endpoints may use BCM_OAM_ENDPOINT_RDI_TX \n")));
            }

            /* Arad+ Type Eth specific flags*/
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) && !(SOC_IS_ARADPLUS(unit) && endpoint_info->type == bcmOAMEndpointTypeEthernet)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Only MEPs of type bcmOAMEndpointTypeEthernet may use BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE \n")));
            }

            /* Arad+ Type Eth specific flags*/
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) && !(SOC_IS_ARADPLUS(unit) && endpoint_info->type == bcmOAMEndpointTypeEthernet)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Only MEPs of type bcmOAMEndpointTypeEthernet may use BCM_OAM_ENDPOINT_PORT_STATE_UPDATE \n")));
            }

            /* Other scenarios */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Requested flags are 0x%x while the correct flags are 0x%x . Please use only the correct flags.\n"), endpoint_info->flags, ~correct_flags));
    }

    if (endpoint_info->flags2 & ~correct_flags2) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: invalid flag2 combination is set.\n")));
    }

    if (endpoint_info->int_pri > 0xFF) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Supporting range for int_pri is 0-255.\n")));
    }
    
    if (endpoint_info->name > 0x1FFF) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Supporting range for name is 0-8191.\n")));
    }

    if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && !is_upmep) {
        if (!BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->tx_gport)
            && !BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: server, down MEP: tx-gport must"
                                              " be either trunk or a system port.\n")));
        }
    } else if ((endpoint_info->tx_gport != BCM_GPORT_INVALID
               && !BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->tx_gport)
               && !BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport)) &&
               !(BCM_GPORT_IS_MCAST(endpoint_info->tx_gport) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_itmh_mc", 0)
               && (endpoint_info->type == bcmOAMEndpointTypeEthernet))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: tx-gport must be either invalid,"
                                          " LAG or a system port.\n")));
    }

    if (((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) == 0) && !_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info)) {
        rv = _bcm_dpp_oam_ma_db_get(unit, endpoint_info->group, &ma_name_struct, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                          (_BSL_BCM_MSG("Error: Group %d not found.\n"), endpoint_info->group));
        }
        /*If Endpoint ID is higher than the size of oamp_umc_table return error*/
        if (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES) {
            if (endpoint_info->id > (SOC_DPP_DEFS_GET(unit, oamp_umc_table_size)-1)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                       (_BSL_BCM_MSG("Error: Endpoint cannot be with ID higher than umc_table_size, current id %d.\n"), endpoint_info->id));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks for OAMoETH endpoints */
int _bcm_oam_ethernet_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) == 0) {
        if (BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address)
            && endpoint_info->gport != BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: destination mac address may"
                                              " not be zero.\n")));
        }

        /* Arad+ Advanced node does not support level 0. Arad+ new classifier mode and Arad simple does support level 0.*/
        if ((endpoint_info->level > 7)
            || ( (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)
                  && !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)
                  && (endpoint_info->level <= 0))
                || (endpoint_info->level < 0))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: invalid level (must be in range %d to 7.\n"),
                                 (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)
                                  && !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit))));
        }
    }

    if ((endpoint_info->outer_tpid == 0 && endpoint_info->vlan != 0)
        || (endpoint_info->inner_tpid == 0 && endpoint_info->inner_vlan != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: VLAN fields inconsistent.\n")));
    }


    if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: mpls_out_gport only applies"
                                          " to BHH endpoints.")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks for OAMoMPLS/PWE endpoints */
int _bcm_oam_mpls_pwe_endpoint_validity_checks(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0) && (BCM_L3_ITF_VAL_GET(endpoint_info->intf_id) & 0xfffe0000) > 0){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Arad Mode support only 2^17 interfaces.")));
     }

    if ((!BCM_MAC_IS_ZERO(endpoint_info->src_mac_address))
        || (!BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: src, dst MAC adresses must"
                                          " be zero in case of BHH type.\n")));
    }


    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && endpoint_info->level != 7
        && (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) == 0
        && endpoint_info->type == bcmOAMEndpointTypeBHHMPLS
        && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Only level 7 supported"
                                          " for Y.1731 endpoints over MPLS.\n")));
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)
        && !_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)
        && endpoint_info->level) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint Level masked,"
                                          " should be 0 for non accelerated endpoints.\n")));
    }


    if ((endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)) {
        if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->mpls_out_gport)
            && !BCM_GPORT_IS_TUNNEL(endpoint_info->mpls_out_gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: mpls_out_gport must"
                                              " be an MPLS port or a tunnel gport\n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks for RFC6374 endpoints */
int _bcm_oam_rfc6374_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    uint8 is_inferred_lm = 0;
    uint32 no_of_slm_endpoints = 0, no_of_dlm_endpoints = 0;
    BCMDNX_INIT_FUNC_DEFS;

    is_inferred_lm = ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) != 0);

    if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: OAMP server is not supported"
                                          " for RFC6374 endpoints.\n")));
    }

    if ((!BCM_MAC_IS_ZERO(endpoint_info->src_mac_address)) ||
        (!BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Source and destination MAC adresses must"
                                          " be zero for RFC6374 endpoints.\n")));
    }

    if ((endpoint_info->gport == BCM_GPORT_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: gport must"
                                          " be valid for RFC6374 endpoints.\n")));
    }

    if ((endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) &&
        (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->mpls_out_gport)) &&
        (!BCM_GPORT_IS_TUNNEL(endpoint_info->mpls_out_gport))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: mpls_out_gport must"
                                          " be an MPLS port or a tunnel gport.\n")));
    }

    if (is_inferred_lm) {
        rv = OAM_ACCESS.no_of_dlm_endpoints.get(unit, &no_of_dlm_endpoints);
        BCMDNX_IF_ERR_EXIT(rv);

        if (no_of_dlm_endpoints != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                (_BSL_BCM_MSG("RFC-6374 ILM endpoint cannot be created when RFC-6374 DLM endpoints exist.")));
        }
    } else {
        rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
        BCMDNX_IF_ERR_EXIT(rv);

        if (no_of_slm_endpoints != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                (_BSL_BCM_MSG("RFC-6374 DLM endpoint cannot be created when RFC-6374 ILM endpoints exist.")));
        }
    }

    /**
     * Return E_UNAVAIL if LM mode is enabled and endpoint is ILM or
     * if SLM mode is enabled and endpoint is DLM
     */
    if (SOC_IS_QUX(unit)) {
        if ((SOC_DPP_CONFIG(unit)->pp.qux_slm_lm_mode == 0) && is_inferred_lm) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                (_BSL_BCM_MSG("RFC-6374 ILM endpoint cannot be created when oam_slm_lm_mode is set to 0 (LM).")));
        }

        if ((SOC_DPP_CONFIG(unit)->pp.qux_slm_lm_mode == 1) && !is_inferred_lm) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                (_BSL_BCM_MSG("RFC-6374 DLM endpoint cannot be created when oam_slm_lm_mode is set to 1 (SLM).")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks used by default and non default endpoints alike. */
int _bcm_oam_endpoint_validity_checks_all(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    int max_counter_range = SOC_IS_JERICHO(unit) ? 
                            SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)*SOC_DPP_DEFS_GET(unit, nof_counter_processors)*2 /*double mode*/ :
                            0x4000;
    BCMDNX_INIT_FUNC_DEFS;

    if (((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) == 0)
		&& !_BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info)) {
        /* Local endpoint*/

        if (endpoint_info->lm_counter_base_id >= max_counter_range
            || (endpoint_info->lm_counter_base_id < 0)) {
            /* Jericho: 18 bit field + 1 for double mode. range is 0 to 0x80000
               ARAD and ARAD+ support only 0-16K counters ids (do not support pcp) */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: invalid lm_counter_base_id"
                                              " (must be in range 0 to %dK-1).\n"),
								  16 + ((endpoint_info->lm_flags & BCM_OAM_LM_PCP) != 0)));

        }

        if (endpoint_info->lm_flags) {
            if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("LM flags not supported for RFC-6374 endpoints.")));
            }

            if (endpoint_info->lm_flags & BCM_OAM_LM_PCP) {
                if (SOC_IS_JERICHO(unit)) {
                    if (endpoint_info->lm_counter_base_id & 0x7) {
                        /* Three LSBs in this case must be off.*/
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("When using LM PCP 3 LSBs of"
    													  " lm_counter_base_id must be off.")));
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("LM PCP unavalable.")));
                }
            }
        }

        if (soc_property_get(unit, spn_OAM_DM_NTP_ENABLE, 1) == 0
			&& (endpoint_info->timestamp_format != bcmOAMTimestampFormatIEEE1588v1)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: NTP disabled, timestamp format must"
											  " be bcmOAMTimestampFormatIEEE1588v1.")));

        }

        if (SOC_IS_ARAD_A0(unit)
            && endpoint_info->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: 1588 timestamp format"
                                              " supported only for Arad B0 and above.")));
        }
    } else {
        if (endpoint_info->lm_counter_base_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("lm_counter base id should be zero.")));
        }
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info)
			&& (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("MIP may not be replaced.")));

        }
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            if ((!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE))
                && (endpoint_info->faults & (~(BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT)))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Only BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT is allowed"
												  " to be set for remote endpoint.")));
            }
        } else if (endpoint_info->faults) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Fault flags are not allowed to be set for local endpoints.")));
        }
    }
    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)
		&&((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) == 0)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
							(_BSL_BCM_MSG("Error: BCM_OAM_ENDPOINT_REPLACE flag can be used only"
										  " with BCM_OAM_ENDPOINT_WITH_ID specification.\n")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks before replacing a local MEP */
int _bcm_oam_endpoint_local_replace_validity_check(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                                   bcm_oam_endpoint_info_t *existing_endpoint_info,
                                                   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    int rv = 0;
	uint8 is_accelerated = 0;
    uint8 port_is_equal;

    BCMDNX_INIT_FUNC_DEFS;

 	is_accelerated = _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info_lcl) || (classifier_mep_entry->is_rfc_6374_entry && existing_endpoint_info->tx_gport != BCM_GPORT_INVALID);

    if (_BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info_lcl) != _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(existing_endpoint_info)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Endpoint can not be updated"
                                          " from MEP to MIP or vise versa.\n")));
    }
    if ((endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_REMOTE) != (existing_endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint remote can not be updated.\n")));
    }
    if (endpoint_info_lcl->type != existing_endpoint_info->type) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: type can not be changed LCL %d EXISTING %d\n"),endpoint_info_lcl->type, existing_endpoint_info->type));
    }
    if (is_accelerated != _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Acceleration in HW can not be"
                                          " added / removed.\n")));
    }

    if (classifier_mep_entry->is_rfc_6374_entry && (existing_endpoint_info->tx_gport == BCM_GPORT_INVALID && endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID)) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: tx gport can not be updated to valid from invalid.\n")));
    }

    if (is_accelerated) {

        if (classifier_mep_entry->is_rfc_6374_entry && (existing_endpoint_info->tx_gport != BCM_GPORT_INVALID && endpoint_info_lcl->tx_gport == BCM_GPORT_INVALID))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: tx gport can not be updated to invalid in case of RFC6374 MEP.\n")));
        }

        if ((endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID || existing_endpoint_info->tx_gport != BCM_GPORT_INVALID) 
            && !_BCM_OAM_IS_SERVER_SERVER(existing_endpoint_info) && !BCM_GPORT_IS_MCAST(existing_endpoint_info->tx_gport)) {
            /* OAMP Server / Multicast destination requires the ability to replace tx_gport */
            rv = _bcm_dpp_gport_compare(unit, endpoint_info_lcl->tx_gport,
                                        existing_endpoint_info->tx_gport,
                                        _bcmDppGportCompareTypeSystemPort,
                                        &port_is_equal);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!port_is_equal) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: tx gport can not be updated.\n")));
            }
        }

        if (BCM_GPORT_IS_MCAST(existing_endpoint_info->tx_gport) != BCM_GPORT_IS_MCAST(endpoint_info_lcl->tx_gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: multicast tx_gport cannot be replaced with unicast tx_gport and vice versa\n")));
        }

        if (((endpoint_info_lcl->vlan != existing_endpoint_info->vlan) && !_BCM_OAM_IS_SERVER_SERVER(existing_endpoint_info))
            /* OAMP Server requires the ability to replace vlan */
             || (endpoint_info_lcl->outer_tpid != existing_endpoint_info->outer_tpid)
             || (endpoint_info_lcl->inner_tpid != existing_endpoint_info->inner_tpid)
             || (endpoint_info_lcl->inner_vlan != existing_endpoint_info->inner_vlan))   {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: encapsulation fields"
                                              " can not be updated.\n")));
        }
    }

    if (endpoint_info_lcl->gport != BCM_GPORT_INVALID
        || existing_endpoint_info->gport != BCM_GPORT_INVALID) {
        rv = _bcm_dpp_gport_compare(unit, endpoint_info_lcl->gport,
                                    existing_endpoint_info->gport,
                                    _bcmDppGportCompareTypeInLif,
                                    &port_is_equal);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!port_is_equal) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Gport can not be updated.\n")));
        }
    }

    if (endpoint_info_lcl->mpls_out_gport != BCM_GPORT_INVALID
        || existing_endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
        rv = _bcm_dpp_gport_compare(unit, endpoint_info_lcl->mpls_out_gport,
                existing_endpoint_info->mpls_out_gport,
                _bcmDppGportCompareTypeOutLif,
                &port_is_equal);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!port_is_equal) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Gport can not be updated.\n")));
        }
    }

    if (endpoint_info_lcl->name != existing_endpoint_info->name) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: name can not be changed\n")));
    }


    if (endpoint_info_lcl->group != classifier_mep_entry->ma_index) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: MA id can not be updated.\n")));
    }

    if ((endpoint_info_lcl->level != classifier_mep_entry->md_level)
        && (!_BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint_info_lcl->id))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: level can not be updated.\n")));
    }

    if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl) != _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: MEP direction can not be updated.\n")));
    }

    if ((endpoint_info_lcl->lm_counter_base_id == 0 && existing_endpoint_info->lm_counter_base_id != 0) || \
        (endpoint_info_lcl->lm_counter_base_id != 0 && existing_endpoint_info->lm_counter_base_id == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: lm_counter_base_id cannot be replaced to/from 0.\n")));
    }

    if (existing_endpoint_info->flags2 &  BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) {
        if (!(endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Cann't replace ingress only with egress only/bidirectional.\n")));
        }
    } else {
        if (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Cann't replace bidirectional with ingress only.\n")));
        }
    }
    if (existing_endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY) {
        if (!(endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Cann't replace egress only with ingress only/bidirectional.\n")));
        }
    } else {
        if (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Cann't replace bidirectional with egress only.\n")));
        }
    }

    if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info_lcl)) {
        if ((!(endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) &&
              (existing_endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM)) ||
            ((endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) &&
             !(existing_endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: LM type cannot be updated to/from ILM.\n")));
        }

        if (endpoint_info_lcl->timestamp_format != existing_endpoint_info->timestamp_format) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Timestamp format cannot be updated for RFC-6374 endpoints.\n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_oam_loopback_validity_check(int unit, bcm_oam_loopback_t *loopback_ptr,
                                     SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    int rv = 0;
    uint8 oam_is_init, found;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(classifier_mep_entry);

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

    if ((!SOC_IS_ARADPLUS_A0(unit))&&(!SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+ and Jericho.")));
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), loopback_ptr->id));
    }

    if (loopback_ptr->tx_count || loopback_ptr->rx_count || loopback_ptr->drop_count || loopback_ptr->invalid_tlv_offset || loopback_ptr->invalid_mep_tlv_subtype || loopback_ptr->invalid_target_mep_tlv
		|| loopback_ptr->remote_mipid_missmatch || loopback_ptr->out_of_sequence || loopback_ptr->unexpected_response) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid fields set")));
    }

    if (loopback_ptr->flags & ~(BCM_OAM_LOOPBACK_UPDATE|BCM_OAM_LOOPBACK_WITH_CTF_ID|BCM_OAM_LOOPBACK_WITH_GTF_ID|BCM_OAM_LOOPBACK_PERIOD_IN_PPS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flag set")));
    }

    if (((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) == 0)
        && (loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE)&&(!SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint has no loopback object to update")));
    }
	
    if (SOC_IS_JERICHO(unit)&&( loopback_ptr->period != 0)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Period should be zero in Jericho")));
    }

    if (SOC_IS_JERICHO(unit)&&(loopback_ptr->num_tlvs > 1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("lb can support one TLV at most")));
    }

	if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry) && loopback_ptr->int_pri!=-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("int_pri must be -1 on Up-MEPs")));
    }


    if (((loopback_ptr->pkt_pri != 0xff)&&(loopback_ptr->pkt_pri > 0xf)) || ((loopback_ptr->inner_pkt_pri != 0xff)&&(loopback_ptr->inner_pkt_pri > 0xf))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("pkt_pri and inner_pkt_pri are 4 bits fields")));
    }

    if (loopback_ptr->int_pri > 0xFF) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Supporting range for int_pri is -1,0-255.\n")));
    }
    if ((SOC_IS_ARADPLUS_A0(unit))&&(loopback_ptr->period >  1000 * arad_chip_kilo_ticks_per_sec_get(unit))&&(loopback_ptr->flags & BCM_OAM_LOOPBACK_PERIOD_IN_PPS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Period is higher than possible rate in HW. max rate is %d (also subject to OAMP maximal transmission rate)"),1000 * arad_chip_kilo_ticks_per_sec_get(unit)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_oam_delay_validity_checks(int unit, const bcm_oam_delay_t *delay_ptr, const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY* classifier_mep_entry) {
    int rv = 0;
    uint32 soc_sand_rv;
    int dont_care;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY old_eth1731_profile;
    uint32 correct_flags = 0;
    uint8 report_mode, demand_mode;
    uint32 user_header_0_size = 0, user_header_1_size = 0,udh_size = 0;
    uint32 user_header_egress_pmf_offset_0_dummy = 0, user_header_egress_pmf_offset_1_dummy = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(delay_ptr);

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/
    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    report_mode = ((delay_ptr->flags & BCM_OAM_DELAY_REPORT_MODE) == BCM_OAM_DELAY_REPORT_MODE);
    /*The demand mode can't be entered on existing entry*/
    demand_mode = (delay_ptr->period == 0 && !(delay_ptr->flags & BCM_OAM_DELAY_UPDATE));

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/

    correct_flags |= BCM_OAM_DELAY_UPDATE;

    if (!classifier_mep_entry->is_rfc_6374_entry) {
        correct_flags |= BCM_OAM_DELAY_ONE_WAY;
    }

    if (SOC_IS_JERICHO(unit)) {
        correct_flags |= BCM_OAM_DELAY_REPORT_MODE;
        if (SOC_IS_QAX(unit)) {
            correct_flags |= BCM_OAM_DELAY_WITH_ID;
        }
    }

    if (delay_ptr->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal flag combination set.")));
    }

    if (!classifier_mep_entry->is_rfc_6374_entry) {
        if (delay_ptr->timestamp_format !=bcmOAMTimestampFormatIEEE1588v1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only 1588 time stamp format supported for y1731.")));
        }
    }

    if (delay_ptr->int_pri || delay_ptr->pkt_pri || delay_ptr->rx_oam_packets  || delay_ptr->tx_oam_packets ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal field set.")));
    }

    if (delay_ptr->period <0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Period below 0 not supported.")));
    }

    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), delay_ptr->id));
    }

    if (classifier_mep_entry->is_rfc_6374_entry) {
        if (((delay_ptr->timestamp_format == bcmOAMTimestampFormatNTP) && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588)) ||
            ((delay_ptr->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) && !(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d timestamp format is different than delay timestamp format."), delay_ptr->id));
        }
    } else {
        if (classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF && !(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not defined as 1588"), delay_ptr->id));
        }
    }

    /* This limitation is relevant for Jericho, QMX and Jericho Plus, QAX */
    if ((SOC_IS_JERICHO(unit)) && demand_mode) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DMM on-demand is unavailable")));
    }

    /* Check that in case of on demand DM endpoint id 3 lsbs are not 0 */
    /* This limitation is relevant for Jericho, QMX and Jericho Plus */
    if ((SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) && demand_mode && ((delay_ptr->id & 0x7) == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Due to HW limitation in case of on demand DM endpoint id 3 lsbs cannot be 0, current id is %d"), delay_ptr->id));
    }

    if (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) {
        if (!soc_property_get(unit, spn_OAM_ONE_DM_ENABLE, 1)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("1DM porgram is required but not loaded.")));
        }

        /* First read the size of the UDH header to reject 1DM config on Jericho and below devices*/
        rv = arad_pmf_db_fes_user_header_sizes_get(
                unit,
                &user_header_0_size,
                &user_header_1_size,
                &user_header_egress_pmf_offset_0_dummy,
                &user_header_egress_pmf_offset_1_dummy
                );
        BCMDNX_IF_ERR_EXIT(rv);
        udh_size = user_header_1_size / 8 + user_header_0_size / 8;

        if(udh_size && SOC_IS_JERICHO_AND_BELOW(unit))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("1DM is not supported with UDH on Jericho and below devices")));
        }
        if (!SOC_IS_JERICHO(unit)) {
            /* 1 dm requires an OAMP_PE program
               in ARAD+, a MAC LSB restriction exist */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_pe_use_1dm_check, (unit,delay_ptr->id));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        /* Jericho validations - Check compatability between the report mode in the MEP profile and the
           requested report mode (In case this is not on-demand DMM).
           QAX validation - in QAX we can have separate reports for LM and DM */
        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth1731_profile);
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, delay_ptr->id, &dont_care, &old_eth1731_profile);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!demand_mode) {
            /* profile.report_mode is 2 bit bitmap that indicate which report is active:
                00 - None    01 - LM     10 - DM     11-Both
                In order to check if DM reports are active we need to check if the 2nd bit is set.
            */
            if ((!report_mode) && (old_eth1731_profile.report_mode & 2) && !SOC_IS_QAX(unit)) {
                /* This ep uses reports, but trying to add delay not in report mode */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses reports for LM/DM but delay added without reports flag"), delay_ptr->id));  /*i1*/
            }

            if (report_mode && !(old_eth1731_profile.report_mode & 2)) {
                uint32 found = 0;
                /* Requested report mode but the profile is either no-report-mode or don't-care.
                   Check if there's a shared entry for LM/DM or piggy-backed LM (which would mean No-Report mode) */
                if (old_eth1731_profile.piggy_back_lm) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses piggy-back LM but delay added with report flag"), delay_ptr->id));
                }

                /* Not Piggy-backed. Check for no-report LM/DM */
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, delay_ptr->id, &found));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (found != 0 && !SOC_IS_QAX(unit)) { /* LM/DM entry found. MEP uses no-report mode */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses shared entry for LM/DM but delay added with report flag"), delay_ptr->id));
                }
            }
        }
    }
 
    LOG_DEBUG(BSL_LS_BCM_OAM,
          (BSL_META_U(unit,
                      "passed initial tests")));

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_oam_endpoint_action_set_validity_check(int unit,
                                                bcm_oam_endpoint_t id,
                                                bcm_oam_endpoint_action_t *action)
{

    int rv = BCM_E_NONE;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 is_default;
    uint8 is_mip;
    uint8 oam_is_init;
    uint8 found;
    int action_option;
    bcm_oam_mpls_lm_dm_opcode_type_t opcode;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, id);
    if ((id < 0) && !is_default) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Endpoint id must be positive or default. Given id: %d\n"), id));
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify that the action is amongst the supported options */
    for (action_option = 0; action_option < bcmOAMActionCount; ++action_option) {
        if (classifier_mep_entry.is_rfc_6374_entry) {
            if ((action_option != bcmOAMActionCountEnable) && (action_option != bcmOAMActionFwd)) {
                /* Such an action is not allowed! Verify that it isn't set */
                if (BCM_OAM_ACTION_GET(*action, action_option)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported action: %d. Only bcmOAMActionCountEnable and bcmOAMActionFwd are supported.\n"), action_option));
                }
            }
        } else {
            if (action_option != bcmOAMActionMcFwd && action_option != bcmOAMActionUcFwd && action_option != bcmOAMActionMeterEnable
                && action_option != bcmOAMActionCountEnable && action_option != bcmOAMActionMcDrop && action_option != bcmOAMActionUcDrop
                && action_option != bcmOAMActionSLMEnable && action_option != bcmOAMActionUcFwdAsData && action_option != bcmOAMActionMcFwdAsData) {
                /* Such an action is not allowed! Verify that it isn't set */
                if (BCM_OAM_ACTION_GET(*action, action_option)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported action: %d. Only bcmOAMActionMcFwd, bcmOAMActionUcFwd, bcmOAMActionMeterEnable,"
                                                            "bcmOAMActionMcFwdAsData, bcmOAMActionUcFwdAsData, bcmOAMActionSLMEnable and bcmOAMActionCountEnable supported.\n"), action_option));
                }
            }
        }
    }

    if (classifier_mep_entry.is_rfc_6374_entry) {
        /* Verify that the opcode is supported */
        for (opcode = 0; opcode < bcmOamMplsLmDmOpcodeTypeMax; ++opcode) {
            if ((opcode != bcmOamMplsLmDmOpcodeTypeLm) && (opcode != bcmOamMplsLmDmOpcodeTypeDm) && (opcode != bcmOamMplsLmDmOpcodeTypeIlm)) {
                if (BCM_OAM_OPCODE_GET(*action, opcode)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported opcode: %d. Only bcmOamMplsLmDmOpcodeTypeLm, bcmOamMplsLmDmOpcodeTypeDm"
                                                                   " and bcmOamMplsLmDmOpcodeTypeIlm are supported.\n"), opcode));
                }
            }
        }
    }

    if ((BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable)) && (!SOC_IS_ARADPLUS(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("SLM is available only in Arad+.")));
    }

    /*
     * Return E_UNAVAIL if LM mode is enabled and SLM action is enabled.
     */
    if (SOC_IS_QUX(unit)) {
        if ((SOC_DPP_CONFIG(unit)->pp.qux_slm_lm_mode == 0) &&
            (BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, 
                (_BSL_BCM_MSG("SLM action enable cannot be done when oam_slm_lm_mode is set to 0 (LM).")));
        }
    }

    if (SOC_IS_QUX(unit)) {
        if (classifier_mep_entry.is_rfc_6374_entry) {
            if ((BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable) &&
                (BCM_OAM_OPCODE_GET(*action, bcmOamMplsLmDmOpcodeTypeDm)))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                    (_BSL_BCM_MSG("Count action cannot be done for RFC-6374 DM opcode.")));
            }
        } else {
            if ((BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable) &&
                (BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM) ||
                BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR) ||
                BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM) ||
                BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR)))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                    (_BSL_BCM_MSG("Count action cannot be done for opcodes DMM, DMR, LBM and LBR.")));
            }
        }
    }

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Mep id %d does not exist.\n"), id));
    }

    if (classifier_mep_entry.mep_type != SOC_PPC_OAM_MEP_TYPE_ETH_OAM && BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: for types other than Ethernet OAM select multicast.\n")));
    }

    is_mip = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&classifier_mep_entry);

    if (BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable) && is_mip) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: SLM action is not supported for MIPs.\n")));
    }

    if (is_mip) {
        /* MIP destination can be eiter valid or invalid, so is destination2.
           In case only one is set, only the corresponding direction will be updated. */
        if (action->destination == BCM_GPORT_INVALID && action->destination2 == BCM_GPORT_INVALID) {
            /* Both destinations are invalid*/
            if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwd)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Fwd action supports only valid gport in destination fields.\n")));
            }
        } else if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcDrop) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcDrop) ||
                   BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwdAsData) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwdAsData)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Drop Fwd and as data actions supports only invalid gport in destination fields.\n")));
        }
    } else {
        /* MEP destination must be set for some trap types while mustn't be set for other types.
           destination2 mustn't be set anyway. */
        if (action->destination2 != BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: destination2 should be set only for MIPs.\n")));
        }
        if (action->destination != BCM_GPORT_INVALID) {
            if (!BCM_GPORT_IS_TRAP(action->destination)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Destination gport must be a trap.\n")));
            }
            if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcDrop) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcDrop) ||
                BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwdAsData) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwdAsData)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Drop Fwd and as data actions supports only invalid gport in destination field.\n")));
            }
        } else if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwd)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Fwd action supports only valid gport in destination field.\n")));
        }
    }

    if ((classifier_mep_entry.lif == _BCM_OAM_INVALID_LIF) &&
        ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
         (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
         (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
         (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
         (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
         (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION))) {
        /* Egress only */
        if ((action->opcode_actions[0] != 0) &&
            ((BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable) == 0) ||
             ((BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM) == 0 &&
               BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR) == 0)))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Egress only set bcmOAMActionSLMEnable\n")));
         }
    } else if (BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable) && (BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM)==0 &&
                   BCM_OAM_OPCODE_GET(*action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR)==0 ) ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: bcmOAMActionSLMEnable may only be set with the SLM/R opcodes. ")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
   This function needs to make sure that:
   If this is the 1st LM/DM entry added to the MEP,
        then the entry is explicitly placed in the MEP DB.
   If there already is an entry,
        then the entry location is implicit.
 */
int _bcm_oam_lm_dm_pointed_add_shared_verify(int unit, SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *lm_dm_mep_db_entry,
                                             SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_entry) {
    BCMDNX_INIT_FUNC_DEFS;

    if (lm_dm_mep_db_entry->is_update) { /* UPDATE */
        /* Make sure there's a chain with entries to update */
        if (mep_entry->lm_dm_ptr == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("No LM/DM entries for MEP 0x%04x."),
                                 lm_dm_mep_db_entry->mep_id));
        }
    }
    else { /* NEW */
        if (mep_entry->lm_dm_ptr) {
            /* MEP entry already has a LM/DM chain */
            if (lm_dm_mep_db_entry->lm_dm_id) {
                /* But the LM/DM entry is explicitly set */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Entry already has LM/DM, cannot add with explicit LM/DM ID (WITH_ID).")));
            }
        } else if (lm_dm_mep_db_entry->lm_dm_id == 0) {
            /* MEP entry has no LM/DM chain, but no explicit entry supplied */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("First LM/DM addition must supply the loss_is/delay_id and set WITH_ID flag.")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks for Y.1711 LM info */
int _bcm_oam_y1711_lm_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info) {
  BCMDNX_INIT_FUNC_DEFS;
  if((endpoint_info->mpls_network_info.function_type != SOC_PPC_OAM_Y1711_LM_MPLS)&&(endpoint_info->mpls_network_info.function_type != SOC_PPC_OAM_Y1711_LM_PWE)){
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Please set correct function type,  function type  0x1-lsp,0x2-pwe)")));
  }
exit:
  BCMDNX_FUNC_RETURN;
}

/* 
 * This function contains all the sanity 
 * checks required when creating or 
 * renaming maintenance endpoint groups 
 * (MEGs) 
*/ 
int _bcm_oam_group_create_verify(int unit, bcm_oam_group_info_t *group_info) {
    int rv = BCM_E_NONE;
    int legal_flags=0;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
        if ((group_info->flags & BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) == 0) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Only 48 byte MAID groups are allowed when custom feature flexible da is set\n")));
        }
    }

    if (SOC_IS_QAX(unit) || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
       /*checking input parameters*/
        legal_flags |= (BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE | BCM_OAM_GROUP_REPLACE | BCM_OAM_GROUP_RX_NAME);
    }
    legal_flags |= BCM_OAM_GROUP_WITH_ID; 
    if (group_info->flags & ~legal_flags)  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: unsupported flag combination\n")));
    }
    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        if (!SOC_IS_QAX(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: group renaming supported for QAX only\n")));
        }
        if (((group_info->flags & BCM_OAM_GROUP_WITH_ID) == 0) ||
            ((group_info->flags & BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) == 0)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Only existing groups with 48 byte MAIDs can be renamed\n")));
        }
        rv = bcm_dpp_am_oam_ma_id_is_alloced(unit, group_info->id);
        if (rv == BCM_E_EXISTS) {
            rv = BCM_E_NONE;
        }
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                 (_BSL_BCM_MSG("Error: Group with id %d doesn't exist\n"), group_info->id));
        }
        else if (rv != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
            rv = bcm_dpp_am_oam_ma_id_is_alloced(unit, group_info->id);
            if (rv==BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                     (_BSL_BCM_MSG("Error: Group with id %d exists\n"), group_info->id));
            }
            else if (rv != BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    if (group_info->flags & BCM_OAM_GROUP_RX_NAME) {
        if (!SOC_IS_QAX(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: different names for rx and tx supported for QAX only\n")));
        }
        if ((group_info->flags & BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: different names for rx and tx allowed only with 48-byte MAID\n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * This function contains all the sanity 
 * checks required for the channel type values 
 * variable in channel_type_rx_set
 * function.
 */ 
int _bcm_oam_mpls_tp_rx_channel_type_values_validity_check (int unit, int num_channel_types, int *list_of_channel_types) {
    int max_num_channel_types = soc_mem_index_count(unit, IHP_OAM_CHANNEL_TYPEm);
    int channel_type_idx = 0 ;

    BCMDNX_INIT_FUNC_DEFS;


    if (list_of_channel_types == NULL) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: list_of_channel_types is NULL")));

    }

    if ((num_channel_types <= 0) || 
        (num_channel_types > max_num_channel_types)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
        (_BSL_BCM_MSG(" This number of channel types is not allowed %d. Max is %d"),
                              num_channel_types, max_num_channel_types));
    }

    for (channel_type_idx = 0; channel_type_idx < num_channel_types;
         channel_type_idx++) {
        if (!_BCM_OAM_DISSECT_MPLS_TP_CHANNEL_TYPE_IN_VALID_RANGE(list_of_channel_types[channel_type_idx])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
            (_BSL_BCM_MSG("channel_type %d is out of valid range"),list_of_channel_types[channel_type_idx]));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * This function contains all the sanity 
 * checks required for the channel type value
 * variable in channel_type_tx_set
 * function.
 */ 
int _bcm_oam_mpls_tp_tx_channel_type_value_validity_check (int unit, int channel_type) {

    BCMDNX_INIT_FUNC_DEFS;


    if (!_BCM_OAM_DISSECT_MPLS_TP_CHANNEL_TYPE_IN_VALID_RANGE(channel_type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
        (_BSL_BCM_MSG("channel_type %d is out of valid range"),channel_type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}
