/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B Layer 2 Management
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRILL

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/sw_db.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/switch.h>

#include <bcm_int/dpp/l3.h>
/*
#include <bcm_int/dpp/mpls.h> 
*/ 
#include <bcm_int/common/field.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>

#include <soc/dpp/mbcm_pp.h>

#define TRILL_ACCESS            sw_state_access[unit].dpp.bcm.trill


/*#define BCM_TRILL_MULTIPATH 0*/


       
static int _bcm_dpp_trill_multicast_adjacency_add( int unit,
                                            bcm_trill_multicast_adjacency_t *trill_multicast_adjacency);

static int _bcm_dpp_trill_multicast_adjacency_delete( int unit,
                                               bcm_trill_multicast_adjacency_t *trill_multicast_adjacency);


/* helper function: indicate for a nickname index, the correspdoning my nickname */
static int _bcm_dpp_trill_port_my_nickname_get(int unit, 
   uint8 name_index, bcm_trill_name_t* name); 
   
/* helper function: indicate for a nickname, the corresponding trill nickname index
   Arad support up to 4 my_nickname (my nickname + 3 virtual my nickname), returned value is in range [0..3] */
static int _bcm_dpp_trill_port_my_nickname_index_get(int unit,
    bcm_trill_name_t name, uint8 *name_index); 


/*
 * FIX fec type for Trill
 */
static int _bcm_dpp_trill_gport_fix_fec_type(
    int unit,
    int fec_id,
    int  new_fec_type)
{
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
        fec_entry[2];
    SOC_PPC_FRWRD_FEC_PROTECT_INFO
        protect_info;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE              
        protect_type;
    uint8
      success;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* In case this is ECMP fec, no need to fix it */
    if (fec_id < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved) {
        BCM_EXIT;
    }

    soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_id,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* In case of LSR special converting, set eep pointer to be out_rif */
    /* In case of host special converting, set eep pointer to be out_rif */
    if(fec_entry[0].type == SOC_PPC_FEC_TYPE_ROUTING && (new_fec_type == SOC_PPC_FEC_TYPE_IP_UC)) {
      /* Replace eep pointer to be as arp pointer */
      fec_entry[0].eep = fec_entry[0].app_info.out_rif;
      fec_entry[0].app_info.out_rif = 0;
      new_fec_type = SOC_PPC_FEC_TYPE_TUNNELING;
    }
    
    fec_entry[0].type = new_fec_type;
    if(protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE) {
        fec_entry[1].type = new_fec_type;
        
    }
    soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id,fec_id,protect_type,&fec_entry[0],&fec_entry[1],&protect_info, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_port_is_ecmp(
    int unit,
    bcm_trill_port_t    *trill_port,
    int                 *is_ecmp,
    SOC_PPC_FEC_TYPE    *ecmp_fec_type)
{
    SOC_PPC_FEC_ID                      fec_index = -1;
    int                                 fec_ecmp_reserverd;
    int                                 ecmp_max_size, ecmp_curr_size;
    int                                 rv = BCM_E_NONE;    
    unsigned int                        soc_sand_dev_id;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    bcm_if_t                            intf_array[10];
    int                                 intf_count;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE      protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO        working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO        protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO      protect_info;

    BCMDNX_INIT_FUNC_DEFS; 
    *is_ecmp = 0;


    if (trill_port->egress_if == 0) 
        BCM_EXIT;
    soc_sand_dev_id = (unit);

    rv = _bcm_l3_intf_to_fec(unit, trill_port->egress_if, &fec_index );
    BCMDNX_IF_ERR_EXIT(rv);

    fec_ecmp_reserverd = SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved - 1;

    if (fec_index < fec_ecmp_reserverd ) {
        rv = bcm_sw_db_l3_get_ecmp_sizes(unit, fec_index, &ecmp_max_size, &ecmp_curr_size);;
        BCMDNX_IF_ERR_EXIT(rv);

        *is_ecmp = (ecmp_curr_size != 0);
    }

    if (*is_ecmp) {
        
        if (soc_property_get(unit, "custom_feature_ecmp_api_multipath", 0) == 1) {

          rv = bcm_l3_egress_multipath_get(unit, fec_index, 10,
                                 intf_array, &intf_count);
          BCMDNX_IF_ERR_EXIT(rv);

        } else {
          bcm_l3_egress_ecmp_t ecmp;

          bcm_l3_egress_ecmp_t_init(&ecmp);
          ecmp.ecmp_intf = fec_index;

          rv = bcm_l3_egress_ecmp_get(unit, &ecmp, 10,
                                 intf_array, &intf_count);
          BCMDNX_IF_ERR_EXIT(rv);

          intf_array[0] = BCM_L3_ITF_VAL_GET(intf_array[0]);
        }

        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, intf_array[0],
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *ecmp_fec_type = working_fec.type;

    }
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
   BCMDNX_FUNC_RETURN;
}

/* extract nickname from gport database
   trill port id must be multicast */
int
_bcm_dpp_dt_nickname_trill_get(
    int unit,
    bcm_gport_t trill_port_id, 
    int         *dt_nickname)
{
    int  rv = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO    phy_port;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (sw_state_htb_key_t) &trill_port_id,
                                       (sw_state_htb_data_t) &phy_port,
                                       FALSE);
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(rv,  (_BSL_BCM_MSG("Find trill port ")));
    }

    if (phy_port.type != _bcmDppGportResolveTypeTrillMC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill port not multicast")));
    }

    *dt_nickname = phy_port.encap_id;

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}

int
_bcm_dpp_mc_to_trill_get(
    int unit,
    bcm_multicast_t group, 
    bcm_gport_t     *port)
{
    int  rv = BCM_E_NONE;
    int  mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    mc_id = _BCM_MULTICAST_ID_GET(group);
    rv = _bcm_dpp_trill_sw_db_hash_mc_trill_find(unit,
                                        (sw_state_htb_key_t) &(mc_id),
                                        (sw_state_htb_data_t) port,
                                        FALSE);        
    if (rv != BCM_E_NOT_FOUND) {
         BCMDNX_IF_ERR_EXIT(rv); 
    }
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_add_to_trill_port_list(
     int unit,
     bcm_gport_t gport) 
{
    bcm_gport_t *tmp_ports = NULL;
    int new_cnt ;
    int port_cnt;
    int allocated_cnt;
    uint32 port_idx;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.port_cnt.get(unit, &port_cnt));
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.allocated_cnt.get(unit, &allocated_cnt));

    if (port_cnt == allocated_cnt ) {
        new_cnt = allocated_cnt + _BCM_TRILL_PORTS_ALLOC_SIZE;
        BCMDNX_ALLOC(tmp_ports, sizeof(bcm_gport_t) * port_cnt, "trill_port_list");
        if (tmp_ports == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        for(port_idx = 0; port_idx < port_cnt; ++port_idx) {
            BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.get(unit, port_idx, &tmp_ports[port_idx]));
        }
        BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.free(unit));
        BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.alloc(unit, new_cnt));
        for(port_idx = 0; port_idx < port_cnt; ++port_idx) {
            BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.set(unit, port_idx, tmp_ports[port_idx]));
        }
        BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.allocated_cnt.set(unit, new_cnt));
    }
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.set(unit, port_cnt, gport));
    port_cnt++;
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.port_cnt.set(unit, port_cnt));

exit:  
    BCM_FREE(tmp_ports);
    BCMDNX_FUNC_RETURN;

}
int
_bcm_petra_remove_from_trill_port_list(
     int unit,
     bcm_gport_t gport) 
{
    
    int i,j ;
    int port_cnt;
    bcm_gport_t cur_gport;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.port_cnt.get(unit, &port_cnt));
    for (i = 0; i < port_cnt; i++ ) {
        BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.get(unit, i, &cur_gport));
        if (cur_gport == gport) {
            for (j=i+1 ; j  < port_cnt  ; ++j) { /*used since memmove is unavailable and sal_memcpy is not to be used when the memory overlaps*/
                BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.get(unit, j, &cur_gport));
                BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.ports.set(unit, j - 1, cur_gport));
            }
            /*sal_memcpy(&(trill_ports->ports[i]), &(trill_ports->ports[i+1]), This was the original part. commented out and replace by above loop by sinai.
                       (trill_ports->port_cnt - i -1) * sizeof(bcm_gport_t) );*/
            port_cnt--;
            BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_ports.port_cnt.set(unit, port_cnt));
            break;
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}
int bcm_petra_trill_init_data(
                              int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_DPP_CONFIG(unit)->trill.mode) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill is not enabled (soc property)\n"), bcm_errmsg(rv)));
    }

    if(SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    rv = TRILL_ACCESS.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
    if(!is_allocated) {
        rv = TRILL_ACCESS.alloc(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = TRILL_ACCESS.mask_set.set(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = TRILL_ACCESS.trill_out_ac.set(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = TRILL_ACCESS.last_used_id.set(unit, -1);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = TRILL_ACCESS.trill_ports.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
    if(!is_allocated) {
        rv = TRILL_ACCESS.trill_ports.alloc(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = TRILL_ACCESS.trill_ports.port_cnt.set(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = TRILL_ACCESS.trill_ports.allocated_cnt.set(unit, _BCM_TRILL_PORTS_ALLOC_SIZE);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = TRILL_ACCESS.trill_ports.ports.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
    if(!is_allocated) {
        rv = TRILL_ACCESS.trill_ports.ports.alloc(unit, _BCM_TRILL_PORTS_ALLOC_SIZE);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /* Initialization has not been completed by default */
    rv = TRILL_ACCESS.init.set(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_trill_deinit_data(
                                int unit)
{
    int                                     rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_DPP_CONFIG(unit)->trill.mode) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill is not enabled (soc property)\n"), bcm_errmsg(rv)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_init(
    int unit)
{
    unsigned int                            soc_sand_dev_id;
    uint32                                  soc_sand_rv  = SOC_SAND_OK;
    int                                     rv = BCM_E_NONE;
    SOC_PPC_AC_ID                           trill_out_ac = 0;
    SOC_SAND_SUCCESS_FAILURE                success;
    uint8                                   trill_is_init;
    int                                     global_trill_in_lif_id;
    SOC_PPC_LIF_ID                          local_trill_in_lif_id = 0;
    bcm_port_t                              port;
    uint32                                  trill_disable_designated_vlan_check;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.trill_enable.get(soc_sand_dev_id, &trill_is_init);
    BCMDNX_IF_ERR_EXIT(rv);

    if (trill_is_init) {
        if (!SOC_WARM_BOOT(unit))
        {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Trill already initialized.\n")));
        }
     }
     else {
        if (SOC_WARM_BOOT(unit)) {
                BCM_EXIT;
        }
     }

    if (!SOC_DPP_CONFIG(unit)->trill.mode) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill is not enabled (soc property)\n"), bcm_errmsg(rv)));
    }

    if (!SOC_WARM_BOOT(unit)) {
        soc_sand_rv = soc_ppd_frwrd_trill_native_inner_tpid_add(soc_sand_dev_id, _BCM_PETRA_TRILL_NATIVE_TPID, &success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(success);
    }

    if (!SOC_WARM_BOOT(unit)) {
        rv = TRILL_ACCESS.trill_out_ac.set(unit, trill_out_ac);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    trill_disable_designated_vlan_check = soc_property_get(unit, spn_TRILL_DESIGNATED_VLAN_CHECK_DISABLE, 0);
    if (!trill_disable_designated_vlan_check) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            global_trill_in_lif_id = soc_property_port_suffix_num_get(unit, port, -1, spn_CUSTOM_FEATURE, "trill_designated_vlan_inlif", -1);
            local_trill_in_lif_id = -1; /* Set to -1 in case it won't be allocated */
            if (global_trill_in_lif_id != -1) {
                if (SOC_IS_JERICHO(unit)) {
                    bcm_dpp_am_local_inlif_info_t     local_in_lif_info;
                    sal_memset(&local_in_lif_info, 0, sizeof(local_in_lif_info));

                    local_in_lif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
                    local_in_lif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                    local_in_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

                    rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &global_trill_in_lif_id, 
                                                                   &local_in_lif_info, NULL);
                    BCMDNX_IF_ERR_EXIT(rv);

                    local_trill_in_lif_id = local_in_lif_info.base_lif_id;

                } else {
                    rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_VSI, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, (SOC_PPC_LIF_ID*)&global_trill_in_lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    local_trill_in_lif_id = global_trill_in_lif_id;
                }

                if (!SOC_WARM_BOOT(unit)) {
                    rv = TRILL_ACCESS.trill_local_in_lif.set(unit, local_trill_in_lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = TRILL_ACCESS.trill_global_in_lif.set(unit, global_trill_in_lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

    if (!SOC_WARM_BOOT(unit)) {
        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.trill_enable.set(unit, 1);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_port_add(
    int unit,
    bcm_trill_port_t * trill_port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_port_add(unit, trill_port, FALSE));

exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_trill_port_delete(int unit, bcm_gport_t trill_port_id) {
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_port_delete(unit, trill_port_id, TRUE));
exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_trill_port_delete_all(int unit){
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_port_get(int unit,bcm_trill_port_t *trill_port){
    int                             rv = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO        phy_port;
    _bcm_petra_trill_info_t         trill_info;
    unsigned int                    soc_sand_dev_id;
    uint32                          soc_sand_rv  = SOC_SAND_OK;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO tril_global_info;
    SOC_PPC_MYMAC_TRILL_INFO        mymac_trill_info;
    int                             virtual_nick_name_index;
    bcm_gport_t                     port_id=trill_port->trill_port_id ;  /* GPORT identifier. */
    bcm_trill_name_t                trill_nickname = trill_port->name;
    int                             key_val;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    bcm_trill_port_t_init(trill_port);

    /* egress trill port */
    if (BCM_GPORT_TRILL_PORT_ID_IS_EGRESS(port_id)) {
        int encap_id;
        int local_out_lif_id, global_out_lif_id;
        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;


        

        /* set egress flag */
        trill_port->flags |= BCM_TRILL_PORT_EGRESS; 

        /* extract the trill eep (aka outlif) from the trill gport */
        /* get the "encap id" part of the gport */
        encap_id = BCM_GPORT_TRILL_PORT_ID_GET(port_id); 
        /* get the outlif from the encap */
        global_out_lif_id = BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(encap_id); 


        /* check it's allocated */

        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_out_lif_id);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_trill_eep_is_alloc(unit, global_out_lif_id); 
        }
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("egress Trill port isn't allocated. gport: %d\n"), port_id));
        }

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_out_lif_id, &local_out_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
            "bcm_petra_trill_port_get.encap_entry_info");
        if(encap_entry_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }

        /* get the EEDB entry from HW */
        rv = soc_ppd_eg_encap_entry_get(unit, 
                                        SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL, 
                                        local_out_lif_id,
                                        1, /* depth */
                                        encap_entry_info, 
                                        next_eep, 
                                        &nof_entries);
        SOC_SAND_IF_ERR_EXIT(rv);

        /* nickname */          
        trill_port->name = encap_entry_info[0].entry_val.trill_encap_info.nick; 
        /* my nickname */
        rv = _bcm_dpp_trill_port_my_nickname_get(unit, encap_entry_info[0].entry_val.trill_encap_info.my_nickname_index, &(trill_port->virtual_name)); 
        BCMDNX_IF_ERR_EXIT(rv);
        /* multicast flag  */
        if (encap_entry_info[0].entry_val.trill_encap_info.m) {
            trill_port->flags |= BCM_TRILL_PORT_MULTICAST; 
        }
        /* egress_if, l3_if object of type encap, contains the ll_eep_ndx */
        if (next_eep[0] != SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID) {
            if (SOC_IS_JERICHO(unit)) {
                int global_lif_id;
                
                rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[0], &global_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
                BCM_L3_ITF_SET(trill_port->egress_if,BCM_L3_ITF_TYPE_LIF, global_lif_id); 
            } else {
                BCM_L3_ITF_SET(trill_port->egress_if,BCM_L3_ITF_TYPE_LIF, next_eep[0]); 
            }
        }

        /* restore trill_port_id (cleared by trill_port_t_init) */
        trill_port->trill_port_id = port_id; 

        BCM_EXIT; 
    }

    /* return virtual nickname and flag if gport is a trill virtual nickname gport */
    if (BCM_GPORT_TRILL_PORT_ID_IS_VIRTUAL(port_id)) {
        /* check if the index is allocated in sw */
        virtual_nick_name_index = _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(port_id);
        rv =  bcm_dpp_am_trill_virtual_nick_name_is_alloc(unit,0,virtual_nick_name_index);
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("virtual nickname isn't allocated. gport: %d\n"), port_id));
        }
        trill_port->flags |= BCM_TRILL_PORT_VIRTUAL;
        trill_port->name = _BCM_DPP_GPORT_TRILL_NICKNAME_GET(port_id);
        BCM_EXIT;
    }

    key_val = port_id ? port_id : trill_nickname;
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (sw_state_htb_key_t) &key_val,
                                       (sw_state_htb_data_t) &phy_port,
                                       FALSE);        
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                      bcm_errmsg(rv), key_val));
    }

    rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                       (sw_state_htb_key_t) &key_val,
                                       (sw_state_htb_data_t) &trill_info,
                                       FALSE);        
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                      bcm_errmsg(rv), key_val));
    }

    if (phy_port.type == _bcmDppGportResolveTypeTrillMC) {
        trill_port->flags = BCM_TRILL_PORT_MULTICAST;
    }
    /* 
    else if (trill_info.type == _BCM_PETRA_TRILL_PORT_MULTIPATH) {
        trill_port->flags = BCM_TRILL_MULTIPATH;
    }*/
    
    if (trill_info.fec_id != -1 ) {
        _bcm_l3_fec_to_intf(unit, trill_info.fec_id, &trill_port->egress_if); 
    }
    trill_port->trill_port_id = port_id; 
    trill_port->name = phy_port.encap_id; 

    soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &tril_global_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    trill_port->hopcount = tril_global_info.cfg_ttl; 

    soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &mymac_trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    if (phy_port.encap_id ==  mymac_trill_info.my_nick_name) {
        trill_port->flags |= BCM_TRILL_PORT_LOCAL;
    }

    if (phy_port.type != _bcmDppGportResolveTypeTrillMC) {
        uint8                           is_found;
        SOC_PPC_FRWRD_DECISION_INFO     fwd_decision; 

        soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_get(soc_sand_dev_id, trill_port->name, &fwd_decision, &is_found);  
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!is_found) {
            trill_port->flags |= BCM_TRILL_PORT_TUNNEL;
        } 
    }

    if (SOC_IS_JERICHO(unit)) {
        /* orientation for ingress unicast trill configuration */
        SOC_PPC_LIF_ID lif_index; 
        SOC_PPC_L2_LIF_TRILL_INFO trill_info; 
        uint8 found, is_wide_entry; 

        SOC_PPC_L2_LIF_TRILL_INFO_clear(&trill_info); 
        /* get lif */
        soc_ppd_frwrd_trill_ingress_lif_get(unit, 
                                            trill_port->name, 
                                            &lif_index, 
                                            &trill_info, 
                                            &found
                                            ); 
        if (found) {
            if (trill_info.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB) {
                trill_port->flags |= BCM_TRILL_PORT_NETWORK; 
            }

            /* Get wide entry flag */
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, lif_index, TRUE, &is_wide_entry, NULL));
            BCMDNX_IF_ERR_EXIT(rv);

            trill_port->flags |= (is_wide_entry) ? BCM_TRILL_PORT_INGRESS_WIDE : 0;
 
        }
    }
    BCM_EXIT;

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_port_get_all(
    int unit, 
    int port_max, 
    bcm_trill_port_t *port_array, 
    int *port_count)
{
    int i;
    int     rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = TRILL_ACCESS.trill_ports.port_cnt.get(unit, port_count);
    BCMDNX_IF_ERR_EXIT(rv);
    if(*port_count > port_max) {
        *port_count = port_max;
    }

    for (i = 0; i < *port_count; i++) {
        bcm_trill_port_t_init(&(port_array[i]));
        rv = TRILL_ACCESS.trill_ports.ports.get(unit, i, &(port_array[i].trill_port_id));
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_trill_port_get(unit, &(port_array[i]));
        BCMDNX_IF_ERR_EXIT(rv);

    }    

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_cleanup(int unit) {
    int status;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id)
    {
        status = _bcm_dpp_trill_sw_db_hash_mc_trill_destroy(unit);
        if( BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "error(%s) freeing MC trill DB failed\n"),
                       bcm_errmsg(status)));
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trill_rbridge_entry_add
 * Purpose:
 *      Create unicast Rbridge with transit functionality
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN)  Trill port configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_rbridge_entry_add(
    int unit,
    bcm_trill_rbridge_t * trill_port)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv = SOC_SAND_OK;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FEC_ID fec_index = -1;
    SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

    
    BCMDNX_INIT_FUNC_DEFS;   
    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

    if (trill_port->egress_if != 0) {
        rv = _bcm_l3_intf_to_fec(unit, trill_port->egress_if, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    /* learn data: built from forward decision. (dest_id=fec_id) */
    SOC_PPD_FRWRD_DECISION_FEC_SET(
       unit, &fwd_decision, fec_index, soc_sand_rv); 

    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_add(soc_sand_dev_id, trill_port->name, &fwd_decision, &success);  
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);
    BCM_EXIT;
    
exit:    
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trill_rbridge_entry_delete
 * Purpose:
 *      Delete unicast Rbridge with transit functionality
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN)  Trill port configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_rbridge_entry_delete(
    int unit,
    bcm_trill_rbridge_t * trill_port)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv = SOC_SAND_OK;
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_remove(soc_sand_dev_id, trill_port->name);  
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_EXIT;
    
exit:    
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trill_rbridge_entry_get
 * Purpose:
 *      Get unicast Rbridge with transit functionality
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT)  Trill port configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_rbridge_entry_get(
    int unit,
    bcm_trill_rbridge_t * trill_port)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv = SOC_SAND_OK;
    SOC_PPC_FEC_ID fec_index = -1;
    uint8 is_found;
    SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_get(soc_sand_dev_id, trill_port->name, &fwd_decision, &is_found);  
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (is_found) {
        fec_index = fwd_decision.dest_id;
        _bcm_l3_fec_to_intf(unit, fec_index, &(trill_port->egress_if));
    }        
    BCM_EXIT;
    
exit:    
    BCMDNX_FUNC_RETURN;
}
int 
_ppd_trill_multicast_route_add(
    int unit, 
    uint32              flags,
    bcm_trill_name_t    root_name, 
    bcm_vlan_t          vlan, 
    bcm_vlan_t          inner_vlan, 
    bcm_multicast_t     group,
    bcm_trill_name_t    src_name, 
    bcm_gport_t         src_port) {

     unsigned int                           soc_sand_dev_id;
     SOC_PPC_TRILL_MC_ROUTE_KEY                 trill_mc_key;

     SOC_SAND_SUCCESS_FAILURE                success;
     
     int                                    rv = BCM_E_NONE;
     uint32                                 mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    if ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) &&
        (SOC_DPP_CONFIG(unit)->trill.transparent_service == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill transparent service is invalid when soc property is not enabled")));
    }
   
    SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
    trill_mc_key.tree_nick = root_name;
    trill_mc_key.fid = vlan;
    trill_mc_key.ing_nick = src_name;
    trill_mc_key.adjacent_eep = 0;
    trill_mc_key.esadi = ((flags & BCM_TRILL_MULTICAST_ESADI) ? 0x1 : 0x0);
    trill_mc_key.tts = ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) ? 0x1 : 0x0);
    if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL)  {
        trill_mc_key.outer_vid = vlan; 
        if (inner_vlan == BCM_VLAN_INVALID) {
            trill_mc_key.inner_vid = 0;
        } else {
            trill_mc_key.inner_vid = inner_vlan;
        }
    }
    mc_id = _BCM_MULTICAST_ID_GET(group);

    rv = soc_ppd_frwrd_trill_multicast_route_add(soc_sand_dev_id, &trill_mc_key, mc_id, &success);
    BCM_SAND_IF_ERR_EXIT(rv);
    SOC_SAND_IF_FAIL_RETURN(success);
   
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
int 
_ppd_trill_multicast_route_remove(
    int unit, 
    uint32              flags,
    bcm_trill_name_t    root_name,
    bcm_vlan_t          vlan, 
    bcm_vlan_t          inner_vlan,
    bcm_multicast_t     group,
    bcm_trill_name_t    src_name) {

     unsigned int                           soc_sand_dev_id;
     SOC_PPC_TRILL_MC_ROUTE_KEY                 trill_mc_key;
     
     int                                    rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    if ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) &&
        (SOC_DPP_CONFIG(unit)->trill.transparent_service == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill transparent service is invalid when soc property is not enabled")));
    }  
	 
    SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
    trill_mc_key.tree_nick = root_name;
    trill_mc_key.fid = vlan;
    trill_mc_key.ing_nick = src_name;
    trill_mc_key.adjacent_eep = 0;
    trill_mc_key.esadi = ((flags & BCM_TRILL_MULTICAST_ESADI) ? 0x1 : 0x0);
    trill_mc_key.tts = ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) ? 0x1 : 0x0);
    if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL)  {
        trill_mc_key.outer_vid = vlan; 
        if (inner_vlan == BCM_VLAN_INVALID) {
            trill_mc_key.inner_vid = 0;
        } else {
            trill_mc_key.inner_vid = inner_vlan;
        }
    }

    rv = soc_ppd_frwrd_trill_multicast_route_remove(soc_sand_dev_id, &trill_mc_key);
    BCM_SAND_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_ppd_trill_multicast_route_get(
    int unit, 
    uint32              flags,
    bcm_trill_name_t    root_name, 
    bcm_vlan_t          vlan, 
    bcm_vlan_t          inner_vlan,     
    bcm_trill_name_t    src_name, 
    bcm_gport_t         src_port,
    bcm_multicast_t     *group,
    uint8               *found) {

    unsigned int                           soc_sand_dev_id;
    SOC_PPC_TRILL_MC_ROUTE_KEY             trill_mc_key;     
    int                                    rv = BCM_E_NONE;
    uint32                                 mc_id;
     

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = unit;

    if ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) &&
        (SOC_DPP_CONFIG(unit)->trill.transparent_service == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill transparent service is invalid when soc property is not enabled")));
    } 
   
    SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
    trill_mc_key.tree_nick = root_name;
    trill_mc_key.fid = vlan;
    trill_mc_key.ing_nick = src_name;
    trill_mc_key.adjacent_eep = 0;
    trill_mc_key.esadi = ((flags & BCM_TRILL_MULTICAST_ESADI) ? 0x1 : 0x0);
    trill_mc_key.tts = ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) ? 0x1 : 0x0);
    if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL)  {
        trill_mc_key.outer_vid = vlan; 
        if (inner_vlan == BCM_VLAN_INVALID) {
            trill_mc_key.inner_vid = 0;
        } else {
            trill_mc_key.inner_vid = inner_vlan;
        }
    }
    
    rv = soc_ppd_frwrd_trill_multicast_route_get(soc_sand_dev_id, &trill_mc_key, &mc_id, found);
    BCM_SAND_IF_ERR_EXIT(rv);
    *group = mc_id;   
   
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Currently supports only change of BCM_TRILL_PORT_LOCAL:                  */
/* accepts:     local->MC+local                                             */
/*              MC+local->MC                                                */
/*              MC+local->local                                             */
/*              MC->local+MC                                                 */

int
_bcm_dpp_trill_port_replace(
    int unit,
    bcm_trill_port_t *  trill_port)
{
    bcm_trill_port_t            old_port;
    int                         rv = BCM_E_NONE;
    uint32                      flag_change, orig_flags, is_local;
   _bcm_petra_trill_mc_route_info_t            *route_list;
    int                                        route_cnt;
    BCMDNX_INIT_FUNC_DEFS;

    old_port.name = trill_port->name;
    old_port.trill_port_id = 0;

    orig_flags = trill_port->flags;
    rv = bcm_petra_trill_port_get(unit, &old_port);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port to replace(0x%x)\n"),
                  bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->name)));
    }

    flag_change = old_port.flags ^ trill_port->flags;
    
    if ( (flag_change & ~BCM_TRILL_PORT_LOCAL & ~BCM_TRILL_PORT_MULTICAST & ~BCM_TRILL_PORT_REPLACE) != 0) {
        rv = BCM_E_UNAVAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("error(%s) This flag change is not allowed.\n"),
              bcm_errmsg(rv)));
    }


    /* reset replace bit*/
    trill_port->flags &= 0xffffffdf;

    rv = _bcm_dpp_trill_port_is_local(unit, old_port.name, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);


   if (is_local && !(trill_port->flags & BCM_TRILL_PORT_LOCAL)) {
       rv = _bcm_dpp_trill_port_reset_local_info(unit, old_port.name );
       BCMDNX_IF_ERR_EXIT(rv);

   }
 

   /* egress_if replace for ingress configuration:
    *  we should check: if (flags & BCM_TRILL_PORT_INGRESS), but it's never used so, we'll check !BCM_TRILL_PORT_EGRESS instead
    *  (egress_if replace at egress configuration is allowed)
    */
    if ((trill_port->flags & BCM_TRILL_PORT_INGRESS) || ((trill_port->flags & BCM_TRILL_PORT_EGRESS) == 0)) {
        /* Change of FEC is not supported, but allowed to add a FEC. 
           A FEC may be added when a fecless MC port is also used as UC.
        */
        if ((old_port.egress_if !=  trill_port->egress_if) && (old_port.egress_if !=0 )) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("error(%s) Trill port 0x%x: Change of egress_if is not supported.) .\n"),
                   bcm_errmsg(rv),trill_port->name));
        }


    }
 
   if ((old_port.flags & BCM_TRILL_PORT_MULTICAST ) && !(trill_port->flags & BCM_TRILL_PORT_MULTICAST)) {
       rv = _bcm_dpp_trill_mc_route_get(unit, old_port.name, &route_list, &route_cnt);
       BCMDNX_IF_ERR_EXIT(rv);

       BCM_FREE(route_list);       

       if (route_cnt > 0) {
           BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Trill port 0x%x has multicast routes. The Multicast flag cannot be reset.\n"),
                old_port.name));
       }
  
   }
   rv = _bcm_dpp_trill_port_add(unit, trill_port, TRUE);
   BCMDNX_IF_ERR_EXIT(rv);

  

  BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    /* restore orig flags*/
   trill_port->flags = orig_flags;

   BCMDNX_FUNC_RETURN;
}


/* helper function: indicate for a nickname index, the correspdoning my nickname */
static int 
_bcm_dpp_trill_port_my_nickname_get(
   int unit, 
   uint8 name_index, 
   bcm_trill_name_t* name) {

    SOC_PPC_MYMAC_TRILL_INFO trill_info;

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit); 
    BCMDNX_VERIFY((name_index < SOC_PPC_MYMAC_NOF_MY_NICKNAMES)); 

    SOC_PPC_MYMAC_TRILL_INFO_clear(&trill_info); 

    /* get trill nicknames */
    rv = soc_ppd_mymac_trill_info_get(unit, &trill_info);
    BCM_SAND_IF_ERR_EXIT(rv);

    /* return my nickname */
    if (name_index == 0) {
        *name = trill_info.my_nick_name; 
        BCM_EXIT; 
    } 
    /* return virtual nickname */
    else {
        /* virtual nickname is in range [1 ..3], we got index [0..2] from array */
        *name = trill_info.virtual_nick_names[name_index -1]; 
        BCM_EXIT;
    }
     
exit:
    BCMDNX_FUNC_RETURN; 
}
        
/* helper function: indicate for a nickname, the corresponding trill nickname index
   Arad support up to 4 my_nickname (my nickname + 3 virtual my nickname), returned value is in range [0..3] */
static int
_bcm_dpp_trill_port_my_nickname_index_get(
    int unit,
    bcm_trill_name_t name, 
    uint8            *name_index
   )        
{
    int rv = BCM_E_NONE; 

    SOC_PPC_MYMAC_TRILL_INFO            trill_info;

    int virtual_name_index; 
    uint32 virtual_nickname; 

    BCMDNX_INIT_FUNC_DEFS; 

    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit); 
    BCMDNX_NULL_CHECK(name_index); 

    SOC_PPC_MYMAC_TRILL_INFO_clear(&trill_info); 

    /* get trill nicknames */
    rv = soc_ppd_mymac_trill_info_get(unit, &trill_info);
    BCM_SAND_IF_ERR_EXIT(rv);

    /* name found at my nickname */
    if (name == trill_info.my_nick_name) {
        *name_index = 0; 
        BCM_EXIT; 
    }

    /* search for name in virtual trill nicknames */
    for (virtual_name_index=0; virtual_name_index<SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE; virtual_name_index++) {
        virtual_nickname = trill_info.virtual_nick_names[virtual_name_index]; 
        if (virtual_nickname == name) {
            /* name found, virtual nickname is in range [1 ..3], we got index [0..2] from array */
            *name_index = virtual_name_index + 1; 
            BCM_EXIT; 
        }
    }

    /* didn't find name in trill nickname */
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trill nickname not found. "))); 

exit:
    BCMDNX_FUNC_RETURN; 
}

/* 
   This function may be called from bcm_dpp_trill_port_add or from _bcm_dpp_trill_port_replace.
 
   When called from bcm_dpp_trill_port_add: the flag is_replace is false,In this case the replace
   opertaion is indicated by the trill port flag BCM_TRILL_PORT_REPLACE.
 
   When called from _bcm_dpp_trill_port_replace he flag is_replace is true.
 
   When a port is added a new ID is allocated for it.
   Multiple ports may have the same nickname but for each nickname a port with id=nickname is also created.
   this port will be fecless (others may have a fec).
   This port is used for searches done by nickname such as trill_multicast_entry_add.
 
   For ports with MC fec a mapping MC->port is added. Calling  trill_multicast_entry_add for
   such port will check that the MC is the same as the FEC.
   For fecless ports trill_multicast_entry_add will create the MC->trill mapping.
 
   
   Local RBridge configuration are indicated by BCM_TRILL_PORT_LOCAL and BCM_TRILL_PORT_VIRTUAL
   
   For remote RBridge configuration (not local rbridge configuration):
      - egress objects configuration is indicated by BCM_TRILL_PORT_EGRESS. 
      - ingress objects configuration is not indicated, so we'll use: ! BCM_TRILL_PORT_EGRESS
   
   
*/    
                                                             
int
_bcm_dpp_trill_port_add(
    int unit,
    bcm_trill_port_t *  trill_port,
    int                 is_replace)
{
    _BCM_GPORT_PHY_PORT_INFO        phy_port;
    _bcm_petra_trill_info_t         hash_trill_info;
    _BCM_GPORT_PHY_PORT_INFO        phy_port_fecless;
    _bcm_petra_trill_info_t         hash_trill_info_fecless;

    int                             rv = BCM_E_NONE;
    SOC_PPC_LIF_ID                      local_lif_id = 0;
    int                                 global_lif_id = 0;
    
    unsigned int                    soc_sand_dev_id;
    uint32                          soc_sand_rv  = SOC_SAND_OK;
    SOC_SAND_SUCCESS_FAILURE        success;
    SOC_PPC_FEC_ID                  fec_index = -1;

    SOC_PPC_MYMAC_TRILL_INFO        trill_info;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO trill_global_info;
    
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE      protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO        working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO        protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO      protect_info;
    uint8                               fec_success;
    int                                 is_ecmp = 0;
    SOC_PPC_FEC_TYPE                    ecmp_fec_type;
    int                                 prev_fec;
    bcm_gport_t                         flood_trill_port_p;
    int                                 dt_nickname;
    int                                 port_id;
    int                              virtual_nick_name_index;
    uint8                               is_ingress; 
    uint8                               is_egress; 
    SOC_SAND_PP_HUB_SPOKE_ORIENTATION   orientation_val ;
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* check if ingress configuration:
       we should check: if (flags & BCM_TRILL_PORT_INGRESS), but it's never used so, we'll check !BCM_TRILL_PORT_EGRESS instead */
    is_ingress = 
        ((trill_port->flags & BCM_TRILL_PORT_INGRESS) || ((trill_port->flags & BCM_TRILL_PORT_EGRESS) == 0)) ? TRUE : FALSE; 
    /* check if egress configuration */
    is_egress = (trill_port->flags & BCM_TRILL_PORT_EGRESS) ? TRUE : FALSE; 

    /* check replace mode, except for egress configuration.*/
    if(trill_port->flags & BCM_TRILL_PORT_REPLACE && !is_egress) {
        return _bcm_dpp_trill_port_replace(unit, trill_port);
    }

    /* set orientation */
    if (trill_port->flags & BCM_TRILL_PORT_NETWORK) {
        orientation_val = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB; 
    } else {
        orientation_val = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE; 
    }

    /* add virtual nickname: using nickname from trill_port
                             return trill port id (gport) of type virtual nickname */
    if (trill_port->flags & BCM_TRILL_PORT_VIRTUAL) {

        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: virtual nicknames are supported starting with arad+ \n"), FUNCTION_NAME()));
        } else {
            /* get index for a new virtual nickname, using allocation mgr
             * return an index in range [1;3] */
            rv = bcm_dpp_am_trill_virtual_nick_name_alloc(unit, trill_port->flags, &virtual_nick_name_index);
            BCMDNX_IF_ERR_EXIT(rv);

            /* add virtual nickname in hw
             * trill_info.virtual_nick_names
               trill_info.skip_ethernet_virtual_nick_names */
            soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            trill_info.virtual_nick_names[virtual_nick_name_index-1] = trill_port->name;
            trill_info.skip_ethernet_virtual_nick_names[virtual_nick_name_index-1] = ((trill_port->flags & BCM_TRILL_PORT_TERM_ETHERNET)?TRUE:FALSE); 
            soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* virtual_nick_name_index from allocmgr has range: [0;2]
             * virtual_nick_name_index in gport: 0 means my_nickname.
             *                                   [1;3] are virtual nicknames */
            /* return trill port: trill port id:  encoding: 0:15: nickname, 16:17: virtual index */
            _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_PORT_ID_SET(trill_port->trill_port_id, 
                                                          virtual_nick_name_index, 
                                                          trill_port->name);

            BCM_EXIT;

        }
    }

    /* BCM_TRILL_PORT_EGRESS is set: create egress TRILL, supported by arad+:
     * 1. allocate an eedb entry (SW)
     * 2. convert bcm struct to soc struct
     * 3. add entry in HW
     * 
     * For replace functionality:
     * 1. don't allocate eep. It's already allocated.
 */
    if (is_egress) {
        SOC_PPC_EG_ENCAP_TRILL_INFO trill_encap_info; 
        uint8 my_nickname_index = 0; 
        int trill_global_out_lif_id = 0, trill_local_out_lif_id = 0, gport;
        uint32 global_ll_lif = 0, local_ll_lif = 0; 
        uint8  ll_eep_is_valid = 0; 
        uint32 trill_eep_alloc_type = _BCM_DPP_AM_TRILL_EEP_TYPE_DEFAULT; /* type of eedb trill tunnel allocation */

        soc_sand_os_memset(&trill_encap_info, 0x0, sizeof(trill_encap_info));

        /* verify */

        /* check flag WITH_ID */
        /* outlif is provided, we check the outlif is valid */
        if (trill_port->flags & BCM_TRILL_PORT_WITH_ID) {
            int encap_id; 
            /* get the "encap id" part of the gport */
            encap_id = BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id); 
            /* check sub-type is outlif for trill gport */
            if (!BCM_GPORT_SUB_TYPE_IS_TRILL_OUTLIF(encap_id)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trill gport id support only trill gport of type outlif when configuring egress object (BCM_TRILL_PORT_EGRESS) \n")));
            }

            /* check the entry wasn't allocated, otherwise, we were expecting REPLACE flag */ 

            /* get the trill eedb index */
            trill_global_out_lif_id = BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(encap_id); 

            if (SOC_IS_JERICHO(unit)) {
                rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, trill_global_out_lif_id);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_trill_eep_is_alloc(unit, trill_global_out_lif_id); 
            }

            /* check the entry wasn't allocated */
            if ((rv == BCM_E_EXISTS) && !(trill_port->flags & BCM_TRILL_PORT_REPLACE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("REPLACE and WITH_ID flags are mandatory for an existing id \n")));
            }

            /* check replace flags is provided */
            if ((rv != BCM_E_EXISTS) && (trill_port->flags & BCM_TRILL_PORT_REPLACE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG(" existing id is mandatory for REPLACE and WITH_ID flags \n")));
            }

        }

        /* check replace mode is valid */
        if (trill_port->flags & BCM_TRILL_PORT_REPLACE) {
            /* check with_id is provided */
            if (!(trill_port->flags & BCM_TRILL_PORT_WITH_ID)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TRILL_PORT_REPLACE is supported only with valid encap_id when replace egress object\n")));
            }
        }
        
        
        /* trill tunnel points to link layer. */
        /* extract ll_eep_ndx from egress_if. */
        global_ll_lif = BCM_L3_ITF_VAL_GET(trill_port->egress_if); 
        ll_eep_is_valid = (global_ll_lif != 0); 

        if (ll_eep_is_valid) {
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_ll_lif, (int*)&local_ll_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* 1. allocate eep (aka outlif) */
        if (!(trill_port->flags & BCM_TRILL_PORT_REPLACE)) {
            uint32 outlif_alloc_flags = ((trill_port->flags & BCM_TRILL_PORT_WITH_ID)? BCM_DPP_AM_FLAG_ALLOC_WITH_ID : 0);

            /* get application type: type of eedb trill tunnel allocation 
             * - for Trill tunnel eedb entry in ROO application for Jericho/QMX devices. (For QAX, default application). 
             * - for all other applications: default
             * application type change EEDB lookups order (EEDB phases)
                 */
            if (SOC_IS_ARADPLUS(unit)) {
                SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type; 

                /* identify type of eedb trill tunnel allocation according to LL eedb entry type: 
                   for ROO application: LL eedb entry type is "data entry" for arad+ 
                                        or linker layer from jericho
                   default otherwise */
                rv = soc_ppd_eg_encap_entry_type_get(unit, local_ll_lif, &entry_type);
                BCM_IF_ERROR_RETURN(rv);

                if (((entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA && SOC_IS_ARADPLUS_A0(unit)) 
                     || ((entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP)  && SOC_IS_JERICHO_AND_BELOW(unit)))    
                    && SOC_IS_ROO_ENABLE(unit)) {
                    trill_eep_alloc_type = _BCM_DPP_AM_TRILL_EEP_TYPE_ROO;
                } 
            }

            /* allocate eep */
            if (SOC_IS_JERICHO(unit)) {
                bcm_dpp_am_local_out_lif_info_t outlif_info;
                sal_memset(&outlif_info, 0, sizeof(outlif_info));

                outlif_info.app_alloc_info.pool_id = dpp_am_res_eep_trill;
                outlif_info.app_alloc_info.application_type = (trill_eep_alloc_type == _BCM_DPP_AM_TRILL_EEP_TYPE_ROO) 
                                                                ? bcm_dpp_am_egress_encap_app_trill_roo : 0;
                outlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, outlif_alloc_flags, &trill_global_out_lif_id, NULL, &outlif_info);
                BCMDNX_IF_ERR_EXIT(rv);

                trill_local_out_lif_id = outlif_info.base_lif_id;

            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_trill_eep_alloc(unit, 
                                                trill_eep_alloc_type, /* application type: change EEDB lookups (EEDB phases) */
                                                outlif_alloc_flags, /* flags */
                                                &trill_global_out_lif_id); 
                BCMDNX_IF_ERR_EXIT(rv);
                trill_local_out_lif_id = trill_global_out_lif_id;
            }
        } 


        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, trill_global_out_lif_id, &trill_local_out_lif_id);
        if (rv != BCM_E_NONE) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("eep not allocated")));
        }              

        /* 2. convert bcm struct to soc struct */
        trill_encap_info.nick = trill_port->name; 
        trill_encap_info.m = ((trill_port->flags & BCM_TRILL_PORT_MULTICAST)?TRUE:FALSE); 
        /* trill my_nickname_index */
        rv = _bcm_dpp_trill_port_my_nickname_index_get(unit, trill_port->virtual_name, &my_nickname_index); 
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("My nickname: 0x%x is not configured \n")));
        }
        trill_encap_info.my_nickname_index = my_nickname_index; 

        /* update is_l2_lif in outlif profile.
           Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            /* set entry to be L2LIF */
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                              (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                                  &(trill_encap_info.outlif_profile)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        }
        trill_encap_info.ll_eep_ndx = local_ll_lif;
        trill_encap_info.ll_eep_is_valid = ll_eep_is_valid;


        /* 3. add EEDB entry in HW */
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_trill_entry_set,(unit,trill_local_out_lif_id,&trill_encap_info)));
        /*rv = arad_pp_eg_trill_entry_set(unit,trill_local_out_lif_id,&trill_encap_info,local_ll_lif,ll_eep_is_valid);*/
        BCMDNX_IF_ERR_EXIT(rv);

        /* set egress trill gport */
        BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_SET(gport, trill_global_out_lif_id);

        BCM_GPORT_TRILL_PORT_ID_SET(trill_port->trill_port_id,   /* gport */
                                    gport /* gport's encap_id */
                                    ); 
        BCM_EXIT;
    }

    if((trill_port->flags & BCM_TRILL_PORT_DROP)
       || 
       (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)
       ||
       ((trill_port->flags & BCM_TRILL_PORT_NETWORK) && SOC_IS_ARADPLUS_AND_BELOW(unit))

       ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags")));
    }

    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(&trill_global_info);
    SOC_PPC_MYMAC_TRILL_INFO_clear(&trill_info);


    /* for ingress configuration */
    if (trill_port->egress_if != 0 && is_ingress) {
        rv = _bcm_l3_intf_to_fec(unit, trill_port->egress_if, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    

    /*Set mymac info for local rbridge*/

    if (trill_port->flags & BCM_TRILL_PORT_LOCAL ) {
        rv = _bcm_dpp_trill_port_set_local_info(unit, 
                                                trill_port->name, 
                                                ((trill_port->flags & BCM_TRILL_PORT_TERM_ETHERNET)?TRUE:FALSE) /* indicate if skip ethernet for my nickname */
                                                );
        BCMDNX_IF_ERR_EXIT(rv);
 
    }
    
    soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &trill_global_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (trill_global_info.cfg_ttl != trill_port->hopcount) {
        /*
        LOG_WARN(BSL_LS_BCM_TRILL,
                 (BSL_META_U(unit,
                             "Port hopcount must be equal to global trill hopcount.port value ignored.")));
        */
    }

    if (is_replace) {
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           (sw_state_htb_key_t) &trill_port->trill_port_id,
                                           (sw_state_htb_data_t) &phy_port,
                                           FALSE);        
        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

        rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                           (sw_state_htb_key_t) &trill_port->trill_port_id,
                                           (sw_state_htb_data_t) &hash_trill_info,
                                           FALSE);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

    } else {
        rv = _bcm_dpp_trill_alloc_port_id(unit,&port_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_GPORT_TRILL_PORT_ID_SET(trill_port->trill_port_id, port_id);

        sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
        sal_memset(&hash_trill_info, 0, sizeof(_bcm_petra_trill_info_t));
    }

    rv = _bcm_dpp_trill_port_is_ecmp(unit, trill_port, &is_ecmp, &ecmp_fec_type );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    if (is_ecmp) {
        phy_port.type = _bcmDppGportResolveTypeProtectedFec;
        hash_trill_info.type = _BCM_PETRA_TRILL_PORT_MULTIPATH;
    } else if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
        phy_port.type = _bcmDppGportResolveTypeTrillMC;
        hash_trill_info.type = _BCM_PETRA_TRILL_PORT_MC;
    } else {
        phy_port.type = _bcmDppGportResolveTypeProtectedFec;
        hash_trill_info.type = _BCM_PETRA_TRILL_PORT_UNI;

    }

    prev_fec = hash_trill_info.fec_id;

    if (!is_replace) {

        phy_port.encap_id = trill_port->name; /* encap_id = nickname */
        phy_port.phy_gport = fec_index; 
        hash_trill_info.fec_id = fec_index; 

        rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                         (sw_state_htb_key_t) &(trill_port->trill_port_id), 
                                         (sw_state_htb_data_t) &phy_port);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

        rv = _bcm_dpp_trill_sw_db_hash_trill_info_insert(unit,
                                         (sw_state_htb_key_t) &(trill_port->trill_port_id), 
                                         (sw_state_htb_data_t) &hash_trill_info);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

        rv = _bcm_petra_add_to_trill_port_list(unit, trill_port->trill_port_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Find the name=id copy
         * The below hash is used to count ports that share the same name.
         * The key for the hash should avoid port id and nick name has the same key.
         * So removed gport type from the key for nick name. */
        rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                           (sw_state_htb_key_t) &(trill_port->name),
                                           (sw_state_htb_data_t) &hash_trill_info_fecless,
                                           TRUE);        
        if (BCM_FAILURE(rv)) {
            sal_memcpy(&phy_port_fecless, &phy_port, sizeof(_BCM_GPORT_PHY_PORT_INFO));
            hash_trill_info_fecless = hash_trill_info;

            phy_port_fecless.phy_gport = -1; 
            hash_trill_info_fecless.fec_id = -1; 

            /*This hash is used for searches done by nickname such as trill_multicast_entry_add*/
            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                             (sw_state_htb_key_t) &(trill_port->name), 
                                             (sw_state_htb_data_t) &phy_port_fecless);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                          bcm_errmsg(rv), trill_port->name));
            }
        }

        hash_trill_info_fecless.use_cnt++;
        rv = _bcm_dpp_trill_sw_db_hash_trill_info_insert(unit,
                              (sw_state_htb_key_t) &(trill_port->name),
                              (sw_state_htb_data_t) &hash_trill_info_fecless);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                      bcm_errmsg(rv), trill_port->name));
        }
    } else if (fec_index != -1) {
        /* Add FEC on MC port - Needed for flooding*/
        if (prev_fec != -1) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x) to FEC %d already has Fec %d\n"),
                bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id), fec_index, hash_trill_info.fec_id));   
        } else {
            phy_port.phy_gport = fec_index; 
            hash_trill_info.fec_id = fec_index; 

        }

    }

    /* Trill port with FEC may be UC,MC or MC+UC. In the case of MC+UC the MC must be fecless and the FEC */
    /* is for the UC behavior.                                                                            */ 
    /* Replace operation for fec change is not supported.                                                                            */ 

    if ( ( (fec_index != -1) && !is_ecmp   ) 
         || (is_ecmp && (ecmp_fec_type == SOC_PPC_FEC_TYPE_TRILL_UC) )) {
        /* coverity explanation: coverity has found that we get here with fec_index = -1. 
           However, if trill_port->egress_if == 0 then fec_index == -1 and is_ecmp == FALSE. (see _bcm_dpp_trill_port_is_ecmp)  
                    and if trill_port->egress_if != 0 then fec_index > 0 (see _bcm_l3_intf_to_fec) 
           Conclusion: We won't get there with fec_index == -1 */
        /* coverity[overrun-call:FALSE] */        
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_index,
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        
        /* For MC with FEC : update tree in FEC */
        if (working_fec.type == SOC_PPC_FEC_TYPE_TRILL_MC) {
            if (!(trill_port->flags & BCM_TRILL_PORT_MULTICAST)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("error(%s) Non MC trill (0x%x) has a MC fec %d\n"),
                      bcm_errmsg(rv), trill_port->trill_port_id, fec_index));
        
            }
            if (is_replace) {
                /* For replace with MC FEC we assume it is for flooding.                         */
                /* In this case we check that the MC group is associated with a trill dist tree  */
                /* by bcm_trill_multicast_entry_add.                                             */
                /* It is true also for create (not only replace) but since we allow the flood MC */
                /* to be the same as port dist_tree we assume the association will be done later. */

                /* old implementation that use sw db for mapping mc-id to nickname*/
                if (SOC_DPP_CONFIG(unit)->trill.mc_id)
                {
                    rv = _bcm_dpp_mc_to_trill_get(unit,
                                                  working_fec.dest.dest_val, 
                                                  &flood_trill_port_p);
                    if ( rv != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,  (_BSL_BCM_MSG("error(%s) The MC flooding group %d is not associated to a trill rootname.\n"),
                          bcm_errmsg(rv), working_fec.dest.dest_val));
                    }
                }
                else{
                    rv = _bcm_dpp_dt_nickname_trill_get(unit,
                                                        trill_port->trill_port_id,
                                                        &dt_nickname);
                    if ( rv != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,  (_BSL_BCM_MSG("error(%s) The trill port %d is not associated to a distribution tree.\n"),
                          bcm_errmsg(rv), trill_port->trill_port_id));
                    }
                }
            }

            if (trill_port->name == 0) {
                rv = _bcm_dpp_trill_gport_fix_fec_type(unit, fec_index, SOC_PPC_FEC_TYPE_SIMPLE_DEST);
                if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Changing fec %d to simple failed (0x%x)\n"),
                          bcm_errmsg(rv), fec_index, BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
                }
            } else {
                working_fec.app_info.dist_tree_nick = trill_port->name;
        
                soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id, fec_index,
                                    protect_type, &working_fec,
                                    &protect_fec, &protect_info, &fec_success);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            /* old implementation that use sw db for mapping mc-id to nickname*/
            if (SOC_DPP_CONFIG(unit)->trill.mc_id){
                rv = _bcm_dpp_mc_to_trill_add(unit,  working_fec.dest.dest_val, trill_port->trill_port_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }

        
        } else {
            /* Add route for unicast entry only.For multicast the route is added explicitly by bcm_trill_multicast_add/source add */
            if ( !(trill_port->flags & BCM_TRILL_PORT_LOCAL ) ) {
                SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

                /* learn data: built from forward decision. (dest_id=fec_id) */
                SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

                SOC_PPD_FRWRD_DECISION_FEC_SET(
                   unit, &fwd_decision, fec_index, soc_sand_rv); 

                /* Create unicast Rbridge with transit functionality if BCM_TRILL_PORT_TUNNEL is not set */
                if (!(trill_port->flags & BCM_TRILL_PORT_TUNNEL)) {
                    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_add(soc_sand_dev_id, trill_port->name, &fwd_decision, &success);  
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    SOC_SAND_IF_FAIL_RETURN(success);
                }
                /* Create SVP if BCM_TRILL_PORT_EGRESS is not set */
                if (!(trill_port->flags & BCM_TRILL_PORT_EGRESS)) {        

                    SOC_PPC_L2_LIF_TRILL_INFO lif_trill_info;
                    SOC_PPC_L2_LIF_TRILL_INFO_clear(&lif_trill_info); 

                    lif_trill_info.fec_id = fec_index;
                    lif_trill_info.learn_enable  = TRUE;

                    /* learn info */
                    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                       SOC_PPD_FRWRD_DECISION_FEC_SET(
                          unit, &(lif_trill_info.learn_info), fec_index, soc_sand_rv); 
                    }
                    if (SOC_IS_JERICHO(unit)) {
                       SOC_PPD_FRWRD_DECISION_TRILL_SET(
                          unit, &(lif_trill_info.learn_info), trill_port->name, 0, fec_index, soc_sand_rv);

                       /* orientation */
                       lif_trill_info.orientation = orientation_val; 
                    }


                    if (SOC_IS_JERICHO(unit)) {
                        bcm_dpp_am_local_inlif_info_t     local_in_lif_info;
                        sal_memset(&local_in_lif_info, 0, sizeof(local_in_lif_info));

                        local_in_lif_info.app_type = bcm_dpp_am_ingress_lif_app_trill_nick;
                        local_in_lif_info.local_lif_flags = DPP_IS_FLAG_SET(trill_port->flags, BCM_TRILL_PORT_INGRESS_WIDE) ? 
                                                                BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                        local_in_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
                        
                        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, 0, &global_lif_id, &local_in_lif_info, NULL);
                        BCMDNX_IF_ERR_EXIT(rv);

                        local_lif_id = local_in_lif_info.base_lif_id;
                    } else {
                        rv = bcm_dpp_trill_lif_alloc(unit,0,(SOC_PPC_LIF_ID*)&global_lif_id);
                        BCMDNX_IF_ERR_EXIT(rv);

                        local_lif_id = global_lif_id;
                    }

                    lif_trill_info.global_lif_id = global_lif_id;
                    lif_trill_info.is_extended = (trill_port->flags & BCM_TRILL_PORT_INGRESS_WIDE) ? TRUE:FALSE;
                    
                    soc_sand_rv = soc_ppd_frwrd_trill_ingress_lif_add(soc_sand_dev_id, local_lif_id, trill_port->name, &lif_trill_info, &success);  
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    SOC_SAND_IF_FAIL_RETURN(success);
                }

                /* old implementation(multicast adjacency added as a part of trill_port_add function*/
                if (SOC_DPP_CONFIG(unit)->trill.mult_adjacency)
                {
                    if (!is_ecmp) {
                    
                        SOC_PPC_TRILL_ADJ_INFO          mac_auth_info;
                        bcm_l3_egress_t                 egress_if;
                        SOC_SAND_PP_MAC_ADDRESS         soc_sand_mac;
                        int                             i;
                        _bcm_dpp_gport_info_t           gport_info;

                        /* Current assumption: FEC and LL reside on the same device) */
                        bcm_l3_egress_t_init(&egress_if);/*Coverity fix UNINT*/
                        rv = bcm_petra_l3_egress_get(unit, trill_port->egress_if, &egress_if); /* Lookup FEC information */
                        BCMDNX_IF_ERR_EXIT(rv);                
                    
                        SOC_PPC_TRILL_ADJ_INFO_clear(&mac_auth_info);
                        
                        rv = _bcm_dpp_gport_to_phy_port(unit, egress_if.port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT , &gport_info); 
                        BCMDNX_IF_ERR_EXIT(rv);
                        mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;
                    
                        rv = bcm_petra_l3_egress_get(unit, egress_if.encap_id, &egress_if); /* Lookup LL information */
                        BCMDNX_IF_ERR_EXIT(rv);                
                    
                        /* get encap ID from FEC */
                        mac_auth_info.expect_adjacent_eep = working_fec.eep;
                        for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
                            soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egress_if.mac_addr[i];
                        }
                    
                        soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(soc_sand_dev_id, &soc_sand_mac, &mac_auth_info, TRUE, &success);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        SOC_SAND_IF_FAIL_RETURN(success);
                    }
                }
            }
        }
    }



    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
   BCMDNX_FUNC_RETURN;
}
/* The reset local is optional - It is used in replace operations.*/
int
_bcm_dpp_trill_port_delete(
    int unit,
    bcm_gport_t trill_port_id,
    int         reset_local_info)
{
    _bcm_petra_trill_info_t     hash_trill_info;
    _BCM_GPORT_PHY_PORT_INFO    phy_port;
    int                         rv = BCM_E_NONE;
    
    unsigned int                soc_sand_dev_id;
    uint32                      soc_sand_rv  = SOC_SAND_OK;
    SOC_PPC_LIF_ID                  local_in_lif_id=0;

    SOC_PPC_MYMAC_TRILL_INFO        trill_info;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO tril_global_info;
    SOC_PPC_FEC_ID                  fec_index = -1;
    
    bcm_trill_port_t            trill_port;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE      protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO        working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO        protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO      protect_info;
    
    _bcm_petra_trill_mc_route_info_t            *route_list;
     int                                        route_cnt;
     int                                        is_ecmp = 0;
     soc_dpp_config_arad_t                      *dpp_arad;
     SOC_PPC_FEC_TYPE                           ecmp_fec_type;
     int                                        virtual_nick_name_index;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* Check that we're not in TM mode, to prevent segmentation faults */
    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    if (!dpp_arad->init.pp_enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Trill in not available in TM mode\n")));
    }

    /* remove egress trill port */
    if (BCM_GPORT_TRILL_PORT_ID_IS_EGRESS(trill_port_id)) {
        int encap_id; 
        int trill_global_out_lif, trill_local_out_lif; 
        SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries; 


        /* extract the trill eep (aka outlif) from the trill gport */
        /* get the "encap id" part of the gport */
        encap_id = BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id); 
        /* get the outlif from the encap */
        trill_global_out_lif = BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(encap_id); 

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, trill_global_out_lif, &trill_local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* read entry from HW */
        rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL, trill_local_out_lif, 1, 
                                        encap_entry_info, next_eep, &nof_entries); 
        SOC_SAND_IF_ERR_RETURN(rv);

        /* check entry is Trill  */
        if (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_TRILL_ENCAP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Incorrect trill eedb entry \n")));
        }


        /* Free outlif from SW DB. */
        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, trill_global_out_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, trill_local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = bcm_dpp_am_trill_eep_dealloc(unit, trill_global_out_lif); 
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Free used EEP entry in HW */
        rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL, trill_local_out_lif); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_EXIT;
    }

    /* remove virtual nickname if gport is a trill virtual nickname gport */
    if (BCM_GPORT_TRILL_PORT_ID_IS_VIRTUAL(trill_port_id)) {
        /* return an index in range [1;3] */
        virtual_nick_name_index = _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(trill_port_id);

        /* remove virtual nickname in sw */
        rv = bcm_dpp_am_trill_virtual_nick_name_dealloc(unit, 0, virtual_nick_name_index);
        BCMDNX_IF_ERR_EXIT(rv);
        /* remove virtual nickname in hw */
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* virtual_nick_names in range [0;2] */
        trill_info.virtual_nick_names[virtual_nick_name_index-1] = 0;
        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_EXIT;
    }

    trill_port.trill_port_id = trill_port_id;
    trill_port.name = 0;
    rv = bcm_petra_trill_port_get(unit, &trill_port);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_trill_mc_route_get(unit, trill_port.name, &route_list, &route_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_FREE(route_list);       

    if (route_cnt > 0) {
           BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Trill port 0x%x has multicast routes. The Multicast flag cannot be reset.\n"),
                trill_port.name));
    }


    /* Remove entry in hash_vlan*/
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (sw_state_htb_key_t) &(trill_port_id),
                                       (sw_state_htb_data_t) &phy_port,
                                       TRUE);        
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Removing trill port (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
    }

    /* Remove entry in hash_trill_info*/
    rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                       (sw_state_htb_key_t) &(trill_port_id),
                                       (sw_state_htb_data_t)&hash_trill_info,
                                       FALSE);        
    if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Removing trill info (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
    }
    else {
        fec_index = hash_trill_info.fec_id;

        rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                           (sw_state_htb_key_t) &(trill_port_id),
                                           (sw_state_htb_data_t) &hash_trill_info,
                                           TRUE);        
        if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Removing trill info (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
        }
    }

    if (fec_index != -1) {

         soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_index,
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      
    }

    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(&tril_global_info);
    SOC_PPC_MYMAC_TRILL_INFO_clear(&trill_info);

    
   
    if (reset_local_info) {
        rv = _bcm_dpp_trill_port_reset_local_info(unit, phy_port.encap_id );
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_trill_port_is_ecmp(unit, &trill_port, &is_ecmp, &ecmp_fec_type );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if ( (phy_port.type != _bcmDppGportResolveTypeTrillMC) && !(trill_port.flags & BCM_TRILL_PORT_LOCAL ) && !is_ecmp) {        
        /* Remove unicast Rbridge with transit functionality if BCM_TRILL_PORT_TUNNEL is not set */
        if (!(trill_port.flags & BCM_TRILL_PORT_TUNNEL)) {   
            soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_remove(soc_sand_dev_id, phy_port.encap_id);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }          
        /* Remove SVP if BCM_TRILL_PORT_EGRESS is not set */
        if (!(trill_port.flags & BCM_TRILL_PORT_EGRESS)) {   
            soc_sand_rv = soc_ppd_frwrd_trill_ingress_lif_remove(soc_sand_dev_id, phy_port.encap_id , &local_in_lif_id);  
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (SOC_IS_JERICHO(unit)) {
                int global_lif_id;
                rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_in_lif_id, &global_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, local_in_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                rv = bcm_dpp_trill_lif_dealloc(unit, 0, local_in_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }        

        /* old implementation(multicast adjacency deleted as a part of trill_port_delete function*/
        if (SOC_DPP_CONFIG(unit)->trill.mult_adjacency){
            SOC_PPC_TRILL_ADJ_INFO      mac_auth_info;
            bcm_l3_egress_t             egr;
            SOC_SAND_SUCCESS_FAILURE    success;
            SOC_SAND_PP_MAC_ADDRESS     soc_sand_mac;         
            int i;

            SOC_PPC_TRILL_ADJ_INFO_clear(&mac_auth_info);
            soc_sand_SAND_PP_SYS_PORT_ID_clear(&mac_auth_info.expect_system_port);
            bcm_l3_egress_t_init(&egr);

            rv = bcm_petra_l3_egress_get(unit, trill_port.egress_if, &egr);
            BCMDNX_IF_ERR_EXIT(rv);
           
            if (fec_index != -1) {
                mac_auth_info.expect_adjacent_eep = working_fec.eep;
            } else {
                mac_auth_info.expect_adjacent_eep = 0;
            }

            for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
                soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egr.mac_addr[i];
            }
           
             soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(soc_sand_dev_id, &soc_sand_mac, &mac_auth_info, FALSE, &success);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
             SOC_SAND_IF_FAIL_RETURN(success); 
        }
           
    }
    if (fec_index != -1) {
        /* old implementation that use sw db for mapping mc-id to nickname*/
        if (SOC_DPP_CONFIG(unit)->trill.mc_id){
            if (working_fec.type==SOC_PPC_FEC_TYPE_TRILL_MC) {
                bcm_gport_t  existing_port;

                /* checking sw db at this stage as it could be removed previously in case of transit mc rb*/
                rv = _bcm_dpp_mc_to_trill_get(unit,
                                              working_fec.dest.dest_val, 
                                              &existing_port);
                if (rv == BCM_E_NONE)
                {                    
                    _bcm_dpp_mc_to_trill_remove( unit, working_fec.dest.dest_val, trill_port.name);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }
   _bcm_petra_remove_from_trill_port_list(unit, trill_port_id);
   phy_port.type = _bcmDppGportResolveTypeInvalid;

   /* Find the name=id copy and remove it if the usage is 0*/
   rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                      (sw_state_htb_key_t) &(trill_port.name),
                                      (sw_state_htb_data_t) &hash_trill_info,
                                      FALSE);        
   if (BCM_FAILURE(rv)) {
           BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) hash_trill_info not found for (0x%x)\n"),
                     bcm_errmsg(rv), trill_port.name));
   }
   hash_trill_info.use_cnt--;

   if (hash_trill_info.use_cnt == 0) {

       rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                          (sw_state_htb_key_t) &(trill_port.name),
                                          (sw_state_htb_data_t) &phy_port,
                                          TRUE);        
       if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
               BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) hash_vlan_find not fouund for trill port (0x%x)\n"),
                         bcm_errmsg(rv), trill_port.name));
       }
       rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                          (sw_state_htb_key_t) &(trill_port.name),
                                          (sw_state_htb_data_t) &hash_trill_info,
                                          TRUE);        
       if (BCM_FAILURE(rv)) {
               BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) hash_trill_info not found for (0x%x)\n"),
                         bcm_errmsg(rv), trill_port.name));

       }
       phy_port.type = _bcmDppGportResolveTypeInvalid;

   }



   BCMDNX_IF_ERR_EXIT(rv);
    
exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_trill_multicast_entry_add(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc) 
{
     SOC_PPC_TRILL_MC_ROUTE_KEY   trill_mc_key;
     int                          rv = BCM_E_NONE;
     bcm_trill_port_t             trill_port;
     bcm_gport_t                  existing_port;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   
    SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);

    /*   1. Add the route without source,   */
    /*   2. Add the mapping trill_port-mc group */
   bcm_trill_port_t_init(&trill_port);
   /* old implementation that use sw db for mapping mc-id to nickname*/
   if (SOC_DPP_CONFIG(unit)->trill.mc_id){

       rv = _bcm_dpp_mc_to_trill_get(unit, 
                                     trill_mc->group, 
                                     &existing_port);
       if (rv != BCM_E_NOT_FOUND) {

           trill_port.trill_port_id = existing_port;
           rv = bcm_petra_trill_port_get(unit, &trill_port);
           BCMDNX_IF_ERR_EXIT(rv);

           if (trill_port.name != trill_mc->root_name) {
               LOG_ERROR(BSL_LS_BCM_TRILL,
                         (BSL_META_U(unit,
                                     "Multicast group %x already defined for a different trill port (0x%x)\n"),
                          trill_mc->group, BCM_GPORT_TRILL_PORT_ID_GET(existing_port)));
               BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, ( _BSL_BCM_MSG("The MC group already defined for a different trill port")));
            }
       } else {

           trill_port.name = trill_mc->root_name;

           rv = bcm_petra_trill_port_get(unit, &trill_port);
           BCMDNX_IF_ERR_EXIT(rv);

           rv = _bcm_dpp_mc_to_trill_add(unit, trill_mc->group, trill_port.trill_port_id);
           BCMDNX_IF_ERR_EXIT(rv);
         }
    } 

    /* set transit forwarding HW lookups */
    if ((trill_mc->flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) == 0) {
       /* Add  without source*/
       rv =  _ppd_trill_multicast_route_add(unit, trill_mc->flags, 
                                            trill_mc->root_name, 
                                            trill_mc->c_vlan, 
                                            trill_mc->c_vlan_inner ,
                                            trill_mc->group, 0,0);
       BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
  
int 
bcm_petra_trill_multicast_entry_delete(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc)
{
     bcm_trill_port_t                       trill_port;
     int                                    rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   
    /* Remove  the route. */
    rv = bcm_petra_trill_multicast_entry_get(unit, trill_mc);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_trill_port_t_init(&trill_port);
    trill_port.name = trill_mc->root_name;

    rv = bcm_petra_trill_port_get(unit, &trill_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id){
        rv = _bcm_dpp_mc_to_trill_remove(unit, trill_mc->group, trill_mc->root_name);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* delete transit forwarding */
    if ((trill_mc->flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) == 0) {
        /* Remove  without source*/
        rv =  _ppd_trill_multicast_route_remove(unit, trill_mc->flags,
                                                trill_mc->root_name, 
                                                trill_mc->c_vlan, 
                                                trill_mc->c_vlan_inner, 
                                                trill_mc->group, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get TRILL multicast entry */
int bcm_petra_trill_multicast_entry_get(
    int unit,
    bcm_trill_multicast_entry_t *trill_mc)
{
    int                                    rv = BCM_E_NONE;
    uint8                                  found = FALSE;
    bcm_trill_port_t                       trill_port;
    bcm_gport_t                            existing_port;
    bcm_multicast_t                        group;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);   

    /*   1. Get the route without source,   */
    /*   2. Get the mapping trill_port-mc group  (for Soc_petra it is done when a MC trill port is added to MC group*/
    /*                                           For arad the trill port is not added to group so the mapping is done here.)*/
    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id) {

        rv = _bcm_dpp_mc_to_trill_get(unit, 
                                      trill_mc->group, 
                                      &existing_port);
         BCMDNX_IF_ERR_EXIT(rv);

         bcm_trill_port_t_init(&trill_port);

         trill_port.trill_port_id = existing_port;
         rv = bcm_petra_trill_port_get(unit, &trill_port);

        BCMDNX_IF_ERR_EXIT(rv);
        if (trill_port.name != trill_mc->root_name) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "Trill multicast entry name=%x group %x not found.\n"),
                       trill_mc->group, BCM_GPORT_TRILL_PORT_ID_GET(existing_port)));
             BCMDNX_ERR_EXIT_MSG(_SHR_E_NOT_FOUND, (_BSL_BCM_MSG("Trill multicast entry not found")));

       }
    }
    /* Get  without source*/
    rv =  _ppd_trill_multicast_route_get(unit, trill_mc->flags, 
                                         trill_mc->root_name, 
                                         trill_mc->c_vlan, 
                                         trill_mc->c_vlan_inner ,
                                         0,0,
                                         &group, 
                                         &found);
     BCMDNX_IF_ERR_EXIT(rv);

     if (found)
        trill_mc->group = group;
     else{
         if(!SOC_DPP_CONFIG(unit)->trill.mc_id){
             BCMDNX_ERR_EXIT_MSG(_SHR_E_NOT_FOUND, (_BSL_BCM_MSG("Trill multicast entry not found")));
         }
     }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_multicast_adjacency_add(
                     int unit,
                     bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{
     BCMDNX_INIT_FUNC_DEFS;
     BCM_DPP_UNIT_CHECK(unit);
     BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_multicast_adjacency_add(unit, trill_multicast_adjacency));
exit:
     BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_multicast_adjacency_delete(
                     int unit,
                     bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{
     BCMDNX_INIT_FUNC_DEFS;
     BCM_DPP_UNIT_CHECK(unit);
     BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_multicast_adjacency_delete(unit, trill_multicast_adjacency));
exit:
     BCMDNX_FUNC_RETURN;
}


int bcm_petra_trill_multicast_adjacency_delete_all(int unit)
{
    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    /* Return BCM_E_UNAVAIL until requirement from customers.*/
    BCMDNX_IF_ERR_EXIT(rv);
exit:
     BCMDNX_FUNC_RETURN;
}


int bcm_petra_trill_multicast_adjacency_traverse(
                     int unit,
                     bcm_trill_multicast_adjacency_traverse_cb a,
                     void *b)
{
    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    /* Return BCM_E_UNAVAIL until requirement from customers.*/
    BCMDNX_IF_ERR_EXIT(rv);
exit:
     BCMDNX_FUNC_RETURN;
}


int bcm_petra_trill_multicast_delete_all(int unit, bcm_trill_name_t root_name)
{

    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    /* Return BCM_E_UNAVAIL until requirement from customers.*/
    BCMDNX_IF_ERR_EXIT(rv);
exit:
     BCMDNX_FUNC_RETURN;
}

/* LCOV_EXCL_START */
int 
bcm_petra_trill_multicast_source_add(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t port,
    bcm_if_t intf) 
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
exit:
    BCMDNX_FUNC_RETURN;
}
/* LCOV_EXCL_STOP */
/* LCOV_EXCL_START */
int 
bcm_petra_trill_multicast_source_delete(
    int unit,
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_name, 
    bcm_gport_t gport,
    bcm_if_t    intf)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_multicast_adjacency_add
 * Purpose:
 *      Set multicast adjacency entry.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      trill_multicast_adjacency   - (IN)
 *                   (IN) port        Trill Network port
 *                   (IN) encap_intf  Neghbor Interface
 *                   (IN) flags       not in use
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_multicast_adjacency_add( int unit,
                                            bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{

    SOC_PPC_TRILL_ADJ_INFO    mac_auth_info;
    SOC_SAND_PP_MAC_ADDRESS   soc_sand_mac;
    SOC_SAND_SUCCESS_FAILURE  success;
    bcm_l3_egress_t           egress_if;
    int                       i;
    int                       rv = BCM_E_NONE;
    uint32                    soc_sand_rv  = SOC_SAND_OK;
    _bcm_dpp_gport_info_t     gport_info;
    int global_out_lif_id; 
    
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_TRILL_ADJ_INFO_clear(&mac_auth_info);
    soc_sand_SAND_PP_SYS_PORT_ID_clear(&mac_auth_info.expect_system_port);

    /* get system port id - used as a key in multiacst adjacency table*/
    rv = _bcm_dpp_gport_to_phy_port(unit, trill_multicast_adjacency->port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT , &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv); 
    mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;

    /* get eep adjacency - used as a result in multiacst adjacency table*/
    rv = _bcm_l3_encap_id_to_eep(unit, trill_multicast_adjacency->encap_intf, &global_out_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);
    /* get local out lif from global out lif */
    mac_auth_info.expect_adjacent_eep = (uint32) global_out_lif_id; 

    bcm_l3_egress_t_init(&egress_if);
    egress_if.flags    = BCM_L3_EGRESS_ONLY;

    /* get mac*/
    rv = bcm_petra_l3_egress_get(unit, trill_multicast_adjacency->encap_intf, &egress_if); /* Lookup LL information */
    BCMDNX_IF_ERR_EXIT(rv);

    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
        soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egress_if.mac_addr[i];
    }

    /* Set multicast adjacency entry Keys: port,mac Result:eep adjacency*/
    soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(unit, &soc_sand_mac, &mac_auth_info, TRUE, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_multicast_adjacency_delete
 * Purpose:
 *      Set multicast adjacency entry.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      trill_multicast_adjacency   - (IN)
 *                   (IN) port        Trill Network port
 *                   (IN) encap_intf  Neghbor Interface
 *                   (IN) flags       not in use
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_multicast_adjacency_delete( int unit,
                                               bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{
 
    SOC_PPC_TRILL_ADJ_INFO    mac_auth_info;
    bcm_l3_egress_t           egress_if;
    SOC_SAND_SUCCESS_FAILURE  success;
    uint32                    soc_sand_rv  = SOC_SAND_OK;
    SOC_SAND_PP_MAC_ADDRESS   soc_sand_mac;
    int                       i;
    _bcm_dpp_gport_info_t     gport_info;
    int                       rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_TRILL_ADJ_INFO_clear(&mac_auth_info);
    soc_sand_SAND_PP_SYS_PORT_ID_clear(&mac_auth_info.expect_system_port);

    /* get system port id - used as a key in multiacst adjacency table*/
    rv = _bcm_dpp_gport_to_phy_port(unit, trill_multicast_adjacency->port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
    mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;
        
    bcm_l3_egress_t_init(&egress_if);
    egress_if.flags    = BCM_L3_EGRESS_ONLY;

    /* get mac*/
    rv = bcm_petra_l3_egress_get(unit, trill_multicast_adjacency->encap_intf, &egress_if);/* Lookup LL information */
    BCMDNX_IF_ERR_EXIT(rv);

    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
        soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egress_if.mac_addr[i];
    }
    /* Remove multicast adjacency entry Keys: port,mac*/
    soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(unit, &soc_sand_mac, &mac_auth_info, FALSE, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success); 

    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;

}

 /* LCOV_EXCL_STOP */
int
_bcm_dpp_mc_to_trill_add(
    int unit,
    bcm_multicast_t group, 
    bcm_gport_t port)
{
    int  rv = BCM_E_NONE;
    bcm_gport_t  existing_port;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    int                     mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    mc_id = _BCM_MULTICAST_ID_GET(group);

    rv = _bcm_dpp_mc_to_trill_get(unit,
                                  group, 
                                  &existing_port);
    if (rv != BCM_E_NOT_FOUND) {
        if (existing_port != port) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "Multicast group %x already defined for a different trill port (0x%x)\n"),
                       group, BCM_GPORT_TRILL_PORT_ID_GET(existing_port)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The MC group already defined for a different trill port")));

        }
    }
    else { 
        rv = _bcm_dpp_trill_sw_db_hash_mc_trill_insert(unit,
                                          (sw_state_htb_key_t) &(mc_id),
                                          (sw_state_htb_data_t) &port);        
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "error(%s) Updating MC trill mapping (0x%x)\n"),
                       bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(port)));
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);

        }
    }
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (sw_state_htb_key_t) (&port), 
                                       (sw_state_htb_data_t) &phy_port,
                                       FALSE);        
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_dpp_sw_db_hash_vlan_find failed")));
    }
   
     phy_port.phy_gport = group;

           
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_dpp_mc_to_trill_remove(
    int unit,
    bcm_multicast_t group, 
    bcm_trill_name_t name)
{
    int  rv = BCM_E_NONE;
    bcm_gport_t  port_p;
    bcm_gport_t  existing_port;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    int                     mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    mc_id = _BCM_MULTICAST_ID_GET(group);

    rv = _bcm_dpp_mc_to_trill_get(unit,
                                  group, 
                                  &existing_port);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_TRILL,
                  (BSL_META_U(unit,
                              "MC to trill mapping was not found for MC %x and trill port (0x%x)\n"),
                   group, BCM_GPORT_TRILL_PORT_ID_GET(existing_port)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The MC group was not found for trill port")));
    }


    rv = _bcm_dpp_trill_sw_db_hash_mc_trill_find(unit,
                                          (sw_state_htb_key_t) &(mc_id),
                                          (sw_state_htb_data_t) &port_p,
                                           TRUE);        
    if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "error(%s) Remove MC to trill mapping %d (0x%x)\n"),
                       bcm_errmsg(rv), group, name));
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);

    }

    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (sw_state_htb_key_t) (&existing_port), 
                                       (sw_state_htb_data_t) &phy_port,
                                       FALSE);        
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_dpp_sw_db_hash_vlan_find failed")));
    }   
    phy_port.phy_gport = 0;
           
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* LCOV_EXCL_START */
int 
bcm_petra_trill_multicast_source_get(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t *port) 
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
exit:
    BCMDNX_FUNC_RETURN;
}
/* LCOV_EXCL_STOP */
int 
bcm_petra_trill_multicast_source_traverse(
    int unit,
    bcm_trill_multicast_source_traverse_cb a,
    void *b)
{
    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_multicast_entry_traverse(
    int unit,
    bcm_trill_multicast_entry_traverse_cb a,
    void *b)
{
    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_petra_trill_port_fec_id_get (
    int unit,
    bcm_gport_t trill_port_id,
    int *fec_id)
{
    int                         rv = BCM_E_NONE;

    _bcm_petra_trill_info_t     trill_info;
    BCMDNX_INIT_FUNC_DEFS;

    *fec_id = -1;
    rv = _bcm_dpp_trill_sw_db_hash_trill_info_find(unit,
                                       (sw_state_htb_key_t) &(trill_port_id),
                                       (sw_state_htb_data_t) &trill_info,
                                       FALSE);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
    }

    *fec_id = trill_info.fec_id;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_trill_get_trill_out_ac(int unit, SOC_PPC_AC_ID *out_ac) {
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_out_ac.get(unit, out_ac));
exit:
    BCMDNX_FUNC_RETURN;
}
int   _bcm_petra_trill_set_trill_out_ac(int unit, SOC_PPC_AC_ID out_ac) {

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.trill_out_ac.set(unit, out_ac));
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_trill_port_set_local_info(
    int unit,
    bcm_trill_name_t port_name, 
    uint32 is_skip_ethernet)
{
    unsigned int                        soc_sand_dev_id;
    SOC_PPC_MYMAC_TRILL_INFO            trill_info;
    uint32                              soc_sand_rv  = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /*Set mymac info for local rbridge: 
     *- my nickname 
      - skip ethernet per my nickname */

    SOC_PPC_MYMAC_TRILL_INFO_clear(&trill_info);
    soc_sand_rv=soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trill_info.my_nick_name = port_name;
    trill_info.skip_ethernet_my_nickname = is_skip_ethernet; 
       
    soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_port_is_local(
    int unit,
    bcm_trill_name_t port_name,
    uint32                 *is_local)
{
    unsigned int                        soc_sand_dev_id;
    SOC_PPC_MYMAC_TRILL_INFO            trill_info;
    uint32                              soc_sand_rv  = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;
    *is_local = FALSE;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *is_local = (port_name ==  trill_info.my_nick_name);
  
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
        

        
int
_bcm_dpp_trill_port_reset_local_info(
    int unit,
    bcm_trill_name_t port_name)
{
    
    unsigned int                        soc_sand_dev_id;
    SOC_PPC_MYMAC_TRILL_INFO            trill_info;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    uint32                              is_local;
    int                                 rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_trill_port_is_local(unit, port_name, &is_local );
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_local) {
        soc_sand_dev_id = (unit);

        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        trill_info.my_nick_name = 0;
        trill_info.skip_ethernet_my_nickname = 0;
        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}  
/* Returnes a list of MC routes for the given root_name.
   The allocated result array should be freed by the caller*/
      
int      
_bcm_dpp_trill_mc_route_get(
    int                                        unit,
    bcm_trill_name_t                           root_name,
    _bcm_petra_trill_mc_route_info_t           **route_list,
    int                                        *cnt)
{

    unsigned int                           soc_sand_dev_id;
    SOC_PPC_TRILL_MC_ROUTE_KEY             trill_mc_key;
    int                                    rv = BCM_E_NONE;
    uint32                                 mc_id;
    uint8                                  found;
    bcm_vlan_data_t                        *vlan_list; 
    int                                    vlan_cnt;
    int                                    i,j;
    _bcm_petra_trill_mc_route_info_t       *route = NULL;
    int                                    route_cnt = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);
    *cnt = 0;
    *route_list = NULL;

    rv = bcm_petra_vlan_list(unit, &vlan_list, &vlan_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

    if (vlan_cnt == 0) {
        BCM_EXIT;
    }
    BCMDNX_ALLOC(route, sizeof(_bcm_petra_trill_mc_route_info_t)*vlan_cnt, "_bcm_dpp_trill_mc_route_get");
    if (route == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }

    for (i = 0; i < vlan_cnt; i++) {
        SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
        trill_mc_key.tree_nick = root_name;
        trill_mc_key.fid = vlan_list[i].vlan_tag;

        for (j =0 ; j < 2; j++) {
            trill_mc_key.tts = j;
            rv = soc_ppd_frwrd_trill_multicast_route_get(soc_sand_dev_id, &trill_mc_key, &mc_id, &found);
            BCM_SAND_IF_ERR_EXIT(rv);

            if (found) {
                route[route_cnt].root_name = root_name;
                route[route_cnt].vlan = vlan_list[i].vlan_tag;
                route[route_cnt].group = mc_id;

                route_cnt++;
            }
        }
    }
    

    BCM_EXIT;
exit:
    if (route_cnt == 0) {
        BCM_FREE(route);
    } else {
        *route_list = route;
        *cnt = route_cnt;
    }

    BCMDNX_FUNC_RETURN;
}    
/*
 * Function:
 *      bcm_petra_trill_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_vpn_create(
    int unit, 
    bcm_trill_vpn_config_t *info) {

    int                     rv = BCM_E_NONE;
    int                     update;
    uint32                  vsi_id;
    bcm_vlan_control_vlan_t control;
    _bcm_petra_trill_vpn_info_t  vpn_hash_info/*, *vpn_hash_tmp=NULL*/;

    BCMDNX_INIT_FUNC_DEFS;

    if ( (SOC_DPP_CONFIG(unit)->trill.mode != SOC_PPD_TRILL_MODE_FGL) && ( info->low_vid != BCM_VLAN_INVALID )) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill FGL is not enabled (soc property): Double tagged VPN is not allowed.\n"), bcm_errmsg(rv)));
    }

    if ((SOC_DPP_CONFIG(unit)->trill.transparent_service != 0) && 
        ((info->flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) != 0) &&
        (info->low_vid != BCM_VLAN_INVALID)) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill transparent servcie is enabled: Double tagged VPN is not allowed.\n"), bcm_errmsg(rv)));
    }

    update = ((info->flags & BCM_BCM_TRILL_VPN_REPLACE) ? TRUE : FALSE);

    /* Validate VSI does not exist */
    rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, info->vpn);
    if (rv != BCM_E_EXISTS) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("error(%s) VSI not fownd (%d)\n"),
                bcm_errmsg(rv), info->vpn));
    }

    
    vsi_id = info->vpn;
    
    /* set flooding for VSI */
    bcm_vlan_control_vlan_t_init(&control);

    control.forwarding_vlan = info->vpn;

    control.broadcast_group = info->broadcast_group;
    control.unknown_multicast_group = info->unknown_multicast_group;
    control.unknown_unicast_group = info->unknown_unicast_group;

    rv = bcm_petra_vlan_control_vlan_set(unit, info->vpn, control);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!update) {
    } else {
        rv = _bcm_dpp_trill_sw_db_hash_trill_vpn_find(unit, 
                                            (sw_state_htb_key_t) &vsi_id, 
                                            (sw_state_htb_data_t) &vpn_hash_info, 
                                            TRUE);        
        if (BCM_FAILURE(rv)) {
             BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Remove TRILL vpn DB (0x%x)\n"),
                       bcm_errmsg(rv), info->vpn));
        }
    }
    
    vpn_hash_info.high_vid=info->high_vid;
    vpn_hash_info.low_vid=info->low_vid;
    vpn_hash_info.flags=info->flags;

    rv = _bcm_dpp_trill_sw_db_hash_trill_vpn_insert(unit,
                                      (sw_state_htb_key_t) &vsi_id, 
                                      (sw_state_htb_data_t) &vpn_hash_info);        
    if (BCM_FAILURE(rv)) {
         BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating TRILL vpn DB (0x%x)\n"),
                   bcm_errmsg(rv), info->vpn));
    }

     
exit:
     BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_trill_get_vsi_info(
        int unit, 
        bcm_vlan_t vsi, 
        _bcm_petra_trill_vpn_info_t *vsi_info) {

    int                     rv = BCM_E_NONE;
    uint32                  vsi_id;
   
    BCMDNX_INIT_FUNC_DEFS;

    vsi_id = vsi;
    rv = _bcm_dpp_trill_sw_db_hash_trill_vpn_find(unit, (sw_state_htb_key_t)&vsi_id, (sw_state_htb_data_t)vsi_info, FALSE);
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);

  
exit:
     BCMDNX_FUNC_RETURN;


}
/*
 * Function:
 *      _bcm_dpp_trill_config_vpn
 * Purpose:
 *      Set the vpn configuration.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                 rv = BCM_E_NONE;
     _bcm_petra_trill_vpn_info_t         hash_vpn_info;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &hash_vpn_info);
     BCMDNX_IF_ERR_EXIT(rv);

     if ((hash_vpn_info.flags & BCM_BCM_TRILL_VPN_ACCESS) != 0)  {
         rv = _bcm_dpp_trill_config_vpn_access(unit, vsi, port);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     if ((hash_vpn_info.flags & BCM_BCM_TRILL_VPN_NETWORK) != 0)  {
         rv = _bcm_dpp_trill_config_vpn_network(unit, vsi, port);
         BCMDNX_IF_ERR_EXIT(rv);
     }
exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_trill_unconfig_vpn(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                rv = BCM_E_NONE;
     _bcm_petra_trill_vpn_info_t        hash_vpn_info;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &hash_vpn_info);
     BCMDNX_IF_ERR_EXIT(rv);

     if ((hash_vpn_info.flags & BCM_BCM_TRILL_VPN_ACCESS) != 0)  {
         rv = _bcm_dpp_trill_delete_vpn_access(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
     }

     if ((hash_vpn_info.flags & BCM_BCM_TRILL_VPN_NETWORK) != 0)  {
         rv = _bcm_dpp_trill_delete_vpn_network(unit, vsi, port);
         BCMDNX_IF_ERR_EXIT(rv);
     }

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      _bcm_dpp_trill_config_vpn_access
 * Purpose:
 *      Set the vpn information - Access side.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn_access(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

    int                                 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) { /* In advanced vlan translation mode, this change must be called in a different api. */
        rv = _bcm_dpp_trill_vlan_editing_create(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_trill_delete_vpn_access(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

    int                                 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) { /* In advanced vlan translation mode, this change must be called in a different api. */
        rv = _bcm_dpp_trill_vlan_editing_delete(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_config_vpn_access
 * Purpose:
 *      Set the vpn information - Access side.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn_network(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) 
{
    int                                 rv = BCM_E_NONE;
    _bcm_petra_trill_vpn_info_t         trill_vpn;
    SOC_SAND_SUCCESS_FAILURE            success;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    unsigned int                        soc_sand_dev_id;
    uint32                              flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);
    rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    if (trill_vpn.flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) {
        flags = SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE;
    }
    soc_sand_rv = soc_ppd_frwrd_trill_vsi_entry_add(soc_sand_dev_id, vsi, flags, trill_vpn.high_vid, trill_vpn.low_vid, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_trill_delete_vpn_network(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) 
{
    int                                 rv = BCM_E_NONE;
    _bcm_petra_trill_vpn_info_t         trill_vpn;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    unsigned int                        soc_sand_dev_id;
    uint32                              flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);
    rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    if (trill_vpn.flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) {
        flags = SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE;
    }
    soc_sand_rv = soc_ppd_frwrd_trill_vsi_entry_remove(soc_sand_dev_id, vsi, flags, trill_vpn.high_vid, trill_vpn.low_vid);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      _bcm_dpp_trill_vlan_editing_create
 * Purpose:
 *      map VSI to outer-vid and inner-vid
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_vlan_editing_create(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                 rv = BCM_E_NONE;
     bcm_vlan_action_set_t               action;
     _bcm_petra_trill_vpn_info_t         trill_vpn;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
     BCMDNX_IF_ERR_EXIT(rv);


    rv = bcm_petra_vlan_translate_action_get(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    BCMDNX_IF_ERR_EXIT(rv);

     action.new_outer_vlan = trill_vpn.high_vid;     
     if ((trill_vpn.flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) != 0) {
         action.outer_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
         action.inner_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
         action.ut_inner = bcmVlanActionNone;
         action.ut_inner_pkt_prio = bcmVlanActionNone;
         action.ot_outer = bcmVlanActionReplace;
         action.ot_outer_prio = bcmVlanActionReplace;
         action.ot_inner = bcmVlanActionCopy;
         action.ot_inner_pkt_prio = bcmVlanActionCopy;
         action.dt_outer = bcmVlanActionNone;
         action.dt_inner = bcmVlanActionNone;
         action.dt_outer_prio = bcmVlanActionNone;
         action.dt_inner_prio = bcmVlanActionNone;

         action.inner_tpid_action = bcmVlanTpidActionModify;
         action.outer_tpid_action = bcmVlanTpidActionModify;
     } else {
         action.ut_inner = bcmVlanActionAdd;
         action.ut_inner_pkt_prio = bcmVlanActionAdd;
         if (trill_vpn.low_vid == BCM_VLAN_INVALID) {  
             action.new_inner_vlan = 0;
             action.inner_tpid = 0;
             action.ot_outer = bcmVlanActionReplace;
             action.ot_outer_prio = bcmVlanActionReplace;
             action.ot_inner = bcmVlanActionNone;
             action.ot_inner_pkt_prio = bcmVlanActionNone;
             action.dt_outer = bcmVlanActionNone;
             action.dt_inner = bcmVlanActionNone;
             action.dt_outer_prio = bcmVlanActionNone;
             action.dt_inner_prio = bcmVlanActionNone;
             
             action.inner_tpid_action = bcmVlanTpidActionNone;
			 action.outer_tpid_action = bcmVlanTpidActionNone;             
         } else {
             action.outer_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
             action.new_inner_vlan = trill_vpn.low_vid;
             action.inner_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
             action.ot_outer = bcmVlanActionReplace;
             action.ot_outer_prio = bcmVlanActionReplace;
             action.ot_inner = bcmVlanActionAdd;
             action.ot_inner_pkt_prio = bcmVlanActionAdd;
             action.dt_outer = bcmVlanActionReplace;
             action.dt_inner = bcmVlanActionReplace;
             action.dt_outer_prio = bcmVlanActionReplace;
             action.dt_inner_prio = bcmVlanActionReplace;

             action.inner_tpid_action = bcmVlanTpidActionModify;
             action.outer_tpid_action = bcmVlanTpidActionModify;
        }
    }   
     action.it_outer = bcmVlanActionAdd;
     action.it_inner = bcmVlanActionReplace;
     action.ut_outer = bcmVlanActionAdd;     

     
     action.it_outer_pkt_prio = bcmVlanActionAdd;
     action.ut_outer_pkt_prio = bcmVlanActionAdd;

     rv = bcm_petra_vlan_translate_action_create(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action); 
     if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
         BCMDNX_IF_ERR_EXIT(rv);
     }
     rv = BCM_E_NONE;
exit:
    BCMDNX_FUNC_RETURN;

 }
int _bcm_dpp_trill_vlan_editing_delete(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                 rv = BCM_E_NONE;
     bcm_vlan_action_set_t               action;
     _bcm_petra_trill_vpn_info_t         trill_vpn;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
     BCMDNX_IF_ERR_EXIT(rv);


    rv = bcm_petra_vlan_translate_action_get(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_vlan_translate_action_delete(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;

 }

/*
 * Function:
 *    _bcm_dpp_trill_gport_to_fwd_decision
 * Description:
 *    convert trill gport to forwarding decision (destination + editing information)
 * Parameters:
 *  unit        -  [IN]    DPP device unit number (driver internal).
 *  gport       -  [IN]    general trill port
 *  mc_id       -  [IN]    MC_ID group
 *  fwd_decsion -  [OUT] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_dpp_trill_gport_to_fwd_decision(int unit, bcm_gport_t gport, bcm_multicast_t mc_id, SOC_PPC_FRWRD_DECISION_INFO  *fwd_decsion)
{
    uint32 soc_sand_rv;
    bcm_error_t rv;
    _BCM_GPORT_PHY_PORT_INFO        phy_port;
    SOC_TMC_MULT_ID     mc_group_id = 0;
 
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(fwd_decsion);

    SOC_PPC_FRWRD_DECISION_INFO_clear(fwd_decsion);

    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (sw_state_htb_key_t) &gport,
                                       (sw_state_htb_data_t) &phy_port,
                                       FALSE);
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trill port not  found\n")));
    }

    if (phy_port.type != _bcmDppGportResolveTypeTrillMC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Trill port must be multicast\n")));
    }

    _bcm_petra_mc_id_to_ppd_val(unit, mc_id ,&mc_group_id);
    if (mc_group_id > BCM_DPP_MAX_MC_ID(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l2mc_index is out of range")));
    }

    SOC_PPD_FRWRD_DECISION_TRILL_SET(unit, fwd_decsion, phy_port.encap_id, TRUE, mc_group_id, soc_sand_rv)
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_vpn_destroy(
    int unit, 
    bcm_vpn_t vpn) {


    int     rv = BCM_E_NONE;
    uint32  vsi_id;
    bcm_trill_vpn_config_t          info;
    _bcm_petra_trill_vpn_info_t     hash_info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_trill_vpn_get(unit, vpn, &info);
    BCMDNX_IF_ERR_EXIT(rv);

    vsi_id = vpn;
    rv = _bcm_dpp_trill_sw_db_hash_trill_vpn_find(unit, (sw_state_htb_key_t) &vsi_id, (sw_state_htb_data_t) &hash_info, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}


int bcm_petra_trill_vpn_destroy_all(int unit)
{
    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    /* Return BCM_E_UNAVAIL until requirement from customers.*/
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}
int bcm_petra_trill_vpn_get(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_trill_vpn_config_t *info) {
    int                     rv = BCM_E_NONE;
    uint32                  vsi_id;
    _bcm_petra_trill_vpn_info_t  vpn_hash_info;
    bcm_vlan_control_vlan_t      vlan_control;
    BCMDNX_INIT_FUNC_DEFS;

    vsi_id = vpn;
    bcm_trill_vpn_config_t_init(info);

    rv = _bcm_dpp_trill_get_vsi_info(unit, vpn, &vpn_hash_info);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_vlan_control_vlan_t_init(&vlan_control);
    rv = bcm_petra_vlan_control_vlan_get(unit, vpn, &vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);


    info->flags = vpn_hash_info.flags;
    info->vpn = vsi_id;
    info->high_vid = vpn_hash_info.high_vid;
    info->low_vid = vpn_hash_info.low_vid;
    info->broadcast_group = _BCM_MULTICAST_ID_GET(vlan_control.broadcast_group);
    info->unknown_unicast_group =   _BCM_MULTICAST_ID_GET(vlan_control.unknown_multicast_group);
    info->unknown_multicast_group = _BCM_MULTICAST_ID_GET(vlan_control.unknown_multicast_group);


exit:
     BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_vpn_traverse(int unit, bcm_trill_vpn_traverse_cb cb, 
                                 void *user_data)
{

    int   rv = BCM_E_UNAVAIL;
    BCMDNX_INIT_FUNC_DEFS;

    /* Return BCM_E_UNAVAIL until requirement from customers.*/
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Alocate a new trill port id .                                        */
/* Trill ports may have id=name which is not allocated by this routine, */
/* So the new ID is verified to be unsued                               */

int _bcm_dpp_trill_alloc_port_id(int unit, int *id) {
    int                         last_id;
    bcm_gport_t                 trill_port_id;
    _BCM_GPORT_PHY_PORT_INFO    phy_port;
    int                         rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = TRILL_ACCESS.last_used_id.get(unit, &last_id);
    BCMDNX_IF_ERR_EXIT(rv);
    ++last_id;
    for (;;last_id++) {
        BCM_GPORT_TRILL_PORT_ID_SET(trill_port_id, last_id);

        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           (sw_state_htb_key_t) &trill_port_id,
                                           (sw_state_htb_data_t) &phy_port,
                                           FALSE);        
        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
            break;
        }

    }


    *id = last_id;
    rv = TRILL_ACCESS.last_used_id.set(unit, last_id);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;

}


int
_bcm_dpp_trill_sw_db_hash_mc_trill_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int gport_mc_trill_db;
    bcm_error_t rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.mc_trill_db_htb_handle.get(unit, &gport_mc_trill_db));
    rv = sw_state_htb_find(unit, gport_mc_trill_db, key, data, remove);
    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
    }


    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_trill_sw_db_hash_mc_trill_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int gport_mc_trill_db;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.mc_trill_db_htb_handle.get(unit, &gport_mc_trill_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit, gport_mc_trill_db, key, data));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_sw_db_hash_trill_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int gport_trill_info_db;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.trill_info_db_htb_handle.get(unit, &gport_trill_info_db));
    rv = sw_state_htb_find(unit, gport_trill_info_db, key, data, remove);
    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_trill_sw_db_hash_trill_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int gport_trill_info_db;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.trill_info_db_htb_handle.get(unit, &gport_trill_info_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit, gport_trill_info_db, key, data));

exit:
    BCMDNX_FUNC_RETURN;
}
int 
_bcm_dpp_trill_sw_db_hash_trill_vpn_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int trill_vpn_db;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.trill_vpn_db_htb_handle.get(unit, &trill_vpn_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit, trill_vpn_db, key, data));
exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_dpp_trill_sw_db_hash_trill_vpn_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int trill_vpn_db;
    bcm_error_t rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.trill_vpn_db_htb_handle.get(unit, &trill_vpn_db));
    rv = sw_state_htb_find(unit, trill_vpn_db, key, data, remove);
    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_trill_sw_db_hash_trill_route_info_destroy(int unit)
{
    int mc_trill_route_info_db;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.mc_trill_route_info_db_htb_handle.get(unit, &mc_trill_route_info_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit, mc_trill_route_info_db, (sw_state_htb_data_free_f)0));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_trill_sw_db_hash_mc_trill_destroy(int unit)
{
    int gport_mc_trill_db;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.mc_trill_db_htb_handle.get(unit, &gport_mc_trill_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit, gport_mc_trill_db, (sw_state_htb_data_free_f)0));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_trill_sw_db_hash_trill_info_destroy(int unit)
{
    int gport_trill_info_db;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.trill_info_db_htb_handle.get(unit, &gport_trill_info_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit, gport_trill_info_db, (sw_state_htb_data_free_f)0));
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_trill_sw_db_hash_trill_info_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int gport_trill_info_db;
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.bcm.gport_mgmt.trill_info_db_htb_handle.get(unit, &gport_trill_info_db);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = sw_state_htb_iterate(unit, gport_trill_info_db, restore_cb);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_sw_db_hash_mc_trill_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int gport_mc_trill_db;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.mc_trill_db_htb_handle.get(unit, &gport_mc_trill_db));

    BCMDNX_IF_ERR_EXIT(sw_state_htb_iterate(unit, gport_mc_trill_db, restore_cb));

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_sw_db_hash_trill_route_info_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int mc_trill_route_info_db;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.mc_trill_route_info_db_htb_handle.get(unit, &mc_trill_route_info_db));

    BCMDNX_IF_ERR_EXIT(sw_state_htb_iterate(unit, mc_trill_route_info_db, restore_cb));

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_sw_db_hash_trill_src_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int mc_trill_root_src_db;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(TRILL_ACCESS.mc_trill_root_src_db_htb_handle.get(unit, &mc_trill_root_src_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_iterate(unit, mc_trill_root_src_db, restore_cb));

exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_dpp_trill_sw_db_hash_trill_vpn_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int trill_vpn_db;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.trill_vpn_db_htb_handle.get(unit, &trill_vpn_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_iterate(unit, trill_vpn_db, restore_cb));

exit:
    BCMDNX_FUNC_RETURN;
}


