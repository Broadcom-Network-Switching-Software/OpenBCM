/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Petra-B PORTS
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/util.h>
#include <bcm_int/common/lock.h>

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_wb.h>

#include <sal/core/time.h>
#include <shared/bitop.h>
#include <soc/defs.h>
#include <soc/types.h>
#include <soc/portmode.h>
#include <soc/linkctrl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/phy/phyctrl.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/PPC/ppc_api_lif.h>

#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_fabric.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/TMC/tmc_api_link_bonding.h>

#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_flow_control.h>
#include <soc/dpp/ARAD/arad_serdes.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_api_cnm.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_esem_access.h>


#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/PORT/arad_ps_db.h>
/* phy includes */
#include <soc/phyctrl.h>
#include <soc/phyreg.h>

#include <soc/dpp/JER/jer_tdm.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_stat_if.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#ifdef BCM_88660
  #include <soc/dpp/PPD/ppd_api_eg_qos.h>
#endif /* BCM_88660 */

#ifdef BCM_QAX_SUPPORT
#include <soc/dpp/QAX/qax_nif.h>
#include <soc/dpp/QAX/qax_link_bonding.h>
#endif /* BCM_QAX_SUPPORT */

#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_llp_cos.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_llp_filter.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_ptp.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_extender.h>
#include <soc/dpp/PPC/ppc_api_port.h>

#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm/lb.h>
#include <bcm_int/dpp/lb.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/link.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/link.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/extender.h>
#include <bcm_int/dpp/pon.h>
#include <bcm_int/dpp/l2.h>
#include <bcm_int/dpp/mpls.h>

#include <shared/shr_template.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/shared/llm_msg.h>
#include <soc/cmicm.h>
#endif

#define _DPP_PORT_INIT_CHECK(unit)    \
    {                                                                                           \
        int _is_init;                                                                           \
        rv = PORT_ACCESS.dpp_port_config.bcm_petra_port_init_arrays_done.get(unit, &_is_init);  \
        BCMDNX_IF_ERR_EXIT(rv);                                                                 \
        if (_is_init == FALSE) {                                                                \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Device isn't initialized")));        \
        }                                                                                       \
    }

#define BCM_DPP_PORT_ADD_PHY_PORT_ID_USE_VIRTUAL    (0xFFFFFFFF)    /* Special Phy Port ID value that indicates 
                                                                       free vritual phy port will be allocated in SDK */

#define BCM_DPP_MAX_FCOE_VFT_VALUE                  4095

#define BCM_DPP_CLASS_ID_NO_CLASS                   (0xFFFFFFFF)    /* Special Class ID value that is used to de associate a LIF
                                                                       from a Ring Protection group */

#define BCM_DPP_PORT_MATCH_LIF_LEARNT               (0)

#define BCM_DPP_PORT_RX_DISABLE_TDM_MIN_SIZE        (192)

#define BCM_DPP_NIF_IPG_MIN                         8

#define _BCM_DPP_PORT_VLAN_CLASSIFICATION_TAG_FORMAT_CEP_MASK       (0x3780)

/* Definitions for 20B prefix header */

#define BCM_DPP_20B_PREFIX_PROPERTY_LENGTH                  (24)
#define BCM_DPP_20B_PREFIX_OUTLIF_MIN                       (1)
#define BCM_DPP_20B_PREFIX_OUTLIF_MAX                       (8)


#ifdef BCM_88660_A0
# define BCM_PORT_INLIF_PROFILE_MAX 15
/* Only 2 lsbs are passed to egress. */
# define BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK 0x3
#endif

  /* Advanced mode for vlan editing
     this is according to soc property 886xx_vlan_translate_mode
   */
#define _BCM_DPP_TPID_PARSE_ADVANCED_MODE(__unit)  (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED((__unit)))
#define _BCM_PETRA_PORT_NOF_TPID_VALS  (4)


  /* API not supported for parsing ADVANCED mode, other API should be used in stead */
#define _BCM_DPP_TPID_PARSE_ADVANCED_MODE_API_UNAVAIL(__unit)   \
  if (_BCM_DPP_TPID_PARSE_ADVANCED_MODE(__unit)) {                        \
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not supported in advanced mode")));   \
  }

  /* API supported only when working in advance vlan editing mode */
#define _BCM_DPP_TPID_PARSE_ADVANCED_MODE_CHECK(__unit)  \
  if (!(_BCM_DPP_TPID_PARSE_ADVANCED_MODE(__unit))) {                        \
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API supported only in advanced mode ")));   \
  }

/* Port default TPID profile */
#define _BCM_DPP_PORT_DFLT_TPID_PROFILE (0)

#define _BCM_DPP_PORT_ADD_IS_MODEM_PORT(__flags)                  (__flags & BCM_PORT_ADD_MODEM_PORT)
#define _BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(__flags)              (!(__flags & BCM_PORT_ADD_MODEM_PORT))
#define _BCM_DPP_PORT_REMOVE_IS_MODEM_PORT(__flags)               (SOC_PORT_IS_LB_MODEM(__flags))
#define _BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(__flags)           (!SOC_PORT_IS_LB_MODEM(__flags))

#define CTR_REFLECTOR_L2_ENCODE(__is_enable, __profile)           (((__profile)<<4) | ((__is_enable)&0x1))
#define CTR_REFLECTOR_L2_DECODE_ENABLE(__encode)                  ((__encode)&0x1)
#define CTR_REFLECTOR_L2_DECODE_PROFILE(__encode)                 (((__encode)&0xf0)>>4)

#ifdef BCM_PORT_DEFAULT_DISABLE
    int _default_port_enable = FALSE;
#else
    int _default_port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */
/* Value for Invalid TPID-Profile */
#define _BCM_DPP_TPID_PROFILE_INVALID (-1)

/* TPID reference counter modification enumeration */
  typedef enum _bcm_dpp_tpid_counter_modify_type_e {
    bcmDppTpidCounterModifyTypeDecrement = -1,  /* Decrement value */
    bcmDppTpidCounterModifyTypeIncrement = 1    /* Increment value */
} _bcm_dpp_tpid_counter_modify_type_t;

#define PORT_ACCESS                 sw_state_access[unit].dpp.bcm.port

int bcm_petra_port_pp_init(int unit);
int _bcm_petra_llvp_table_native_llvp_entries_init(int unit); 

int
soc_jer_port_ingress_enable_get(
    int                   unit,
    soc_port_t            port,
    int                   *enable);

int
soc_jer_port_ingress_enable_set(
    int                   unit,
    soc_port_t            port,
    int                   enable);


/***** SW resources access *******/

/*
 * Function:
 *      _bcm_dpp_in_lif_match_tpid_type_set
 * Purpose:
 *      Obtain tpid type information of an MPLS port/I-SID InLif
 * Parameters:
 *      unit    - (IN) Device Number
 *      tpid type - (OUT) tpid type information
 *      in_lif  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_match_tpid_type_set(int unit, _bcm_petra_tpid_profile_t tpid_type, int in_lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.in_lif_sw_resources.tpid_profile_type = tpid_type;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);
    
        
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_in_lif_match_tpid_type_get
 * Purpose:
 *      Obtain tpid type information of an MPLS port/I-SID InLif
 * Parameters:
 *      unit    - (IN) Device Number
 *      tpid type - (OUT) tpid type information
 *      in_lif  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_match_tpid_type_get(int unit, _bcm_petra_tpid_profile_t *tpid_type, int in_lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

    *tpid_type = gport_sw_resources.in_lif_sw_resources.tpid_profile_type;
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_port_outlif_profile_preset
 * Purpose:
 *      Assigns outlif_profile the value taken from the eedb entry. Should be used before
 *      assigning reserved bits to outlif_profile with the occupation manager.
 * Parameters:
 *      unit                  - (IN) unit number.
 *      encap_entry_info      - (IN) EEDB entry with information.
 *      outlif_profile        - (OUT) variable that will be used (by the occupation manager) to assign bits in the outlif profile field in the eedb entry.
 * Returns: None
 * Notes:
 */
void
_bcm_dpp_port_outlif_profile_preset(int unit, SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info, uint32 *outlif_profile){

    if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
        *outlif_profile = encap_entry_info->entry_val.mpls_encap_info.outlif_profile;
    } else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {
        *outlif_profile = encap_entry_info->entry_val.pwe_info.outlif_profile;
    } else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
        *outlif_profile = encap_entry_info->entry_val.ll_info.outlif_profile;
    } else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
        *outlif_profile = encap_entry_info->entry_val.ipv4_encap_info.outlif_profile;
    } else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
        *outlif_profile = encap_entry_info->entry_val.data_info.outlif_profile;
    }
}

/*
 * Exported Functions
 */

int
bcm_petra_port_init(int unit)
{
    pbmp_t okay_ports;
    bcm_error_t rv;
    int init_speed, port_enable;
    bcm_port_t port;
    int port_pp_initialized;
    pbmp_t prbs_mac_mode;
    bcm_pbmp_t ports_bm;
    uint32     is_master;
    soc_port_if_t interface;
    uint8 is_allocated;
    uint32  flags;
    uint32  is_skip_nrdy_alloc = 0;
    int        core = 0;
    uint32     tm_port = 0;
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
    uint32     egress_offset = 0;
    BCMDNX_INIT_FUNC_DEFS;


    if(!SOC_WARM_BOOT(unit)){
        rv = PORT_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
        if(!is_allocated) {
            rv = PORT_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = PORT_ACCESS.dpp_port_config.bcm_petra_port_init_arrays_done.set(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);

        /********************************************/
        /*          PP Related init                 */
        /********************************************/

        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
        if(!is_allocated) {
            rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /*Clear PRBS mode bitmap*/
        SOC_PBMP_CLEAR(prbs_mac_mode);
        rv = PORT_ACCESS.prbs_mac_mode.set(unit, prbs_mac_mode);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_DPP_PP_ENABLE(unit))
    {
        rv = PORT_ACCESS.dpp_port_config.port_pp_initialized.get(unit, &port_pp_initialized);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!port_pp_initialized) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_pp_init(unit));
            rv = PORT_ACCESS.dpp_port_config.port_pp_initialized.set(unit, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /********************************************/
    /*       NIF\Fabric Related init            */
    /********************************************/
    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_fabric_clk_freq_init, (unit, PBMP_SFI_ALL(unit)));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get ref clock for disabled SFI ports. Required for dynamic links */
    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_fabric_clk_freq_init, (unit, SOC_PORT_DISABLED_BITMAP(unit, sfi)));
    BCMDNX_IF_ERR_EXIT(rv);

    /* initialize */
    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_init,(unit));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Probe */
    BCMDNX_IF_ERR_EXIT(bcm_petra_port_probe(unit, PBMP_PORT_ALL(unit), &okay_ports));

    /* Post-probe operations */
    if (!SOC_WARM_BOOT(unit)) {
        PBMP_ITER(okay_ports, port) {
            port_enable = _default_port_enable;
            init_speed = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, 0);
            if(-1 == init_speed) {
                port_enable = FALSE;
            }
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_enable_set(unit, port, port_enable)); 
        }
    }

    /* Post init sequence */
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_post_init,(unit, &okay_ports));
    BCMDNX_IF_ERR_EXIT(rv);

    /*Configure ALDWP*/
    if (!SOC_WARM_BOOT(unit)) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_aldwp_config,(unit));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    

    /* Set all fabric links in loopback if single fap system */
    if (!SOC_WARM_BOOT(unit)) {
        if (SOC_DPP_SINGLE_FAP(unit)) {
            PBMP_SFI_ITER(unit, port) {
                if (!SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
                    BCMDNX_IF_ERR_EXIT(bcm_petra_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY)); 
                }
            }
        }
    }

    /* set NRDY thresholds */
    if (!SOC_WARM_BOOT(unit)) {
        if (SOC_IS_JERICHO(unit)) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));
            SOC_PBMP_ITER(ports_bm, port) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
                if (is_master) {
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
                    if (SOC_IS_QAX(unit)) {
                        /* 1. skip NRDY template allocate for LB modem port
                           2. skip allocate NRDY template twice for RESERVE EGQ interface. Only 1 template is needed. */
                        lb_port_num = 0;
                        if (SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
                            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
                            if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                                SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));                               
                            }
                        }
                        if (SOC_PORT_IS_LB_MODEM(flags) || (lb_port_num > 1)) {
                            is_skip_nrdy_alloc = 1;
                        }
                    }
                    if ((interface != SOC_PORT_IF_ERP) && (is_skip_nrdy_alloc == 0)) {
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_allocate(unit, port));
                    }
                }
            }
        }
    }

  /* configures the port - trap mapping for static forwarding in TM mode. Static forwarding in Ardon is unsupported. */
  if (!SOC_WARM_BOOT(unit) && !SOC_DPP_PP_ENABLE(unit) && !SOC_IS_ARDON(unit) && !SOC_IS_FLAIR(unit)) 
  {
       bcm_dpp_user_defined_traps_t ud_trap_info;
       sal_memset(&ud_trap_info, 0x0, sizeof(ud_trap_info));
       rv = _bcm_petra_port_traps_init(unit);
       BCMDNX_IF_ERR_EXIT(rv);
       rv = _bcm_dpp_am_template_user_defined_traps_init(unit,_BCM_PETRA_UD_DFLT_TRAP - SOC_PPC_TRAP_CODE_USER_DEFINED_0,&ud_trap_info);
       BCMDNX_IF_ERR_EXIT(rv);
  }

  /* init LLVP table for LLVP profile = 7.
     LLVP profile 7 is dedicated to egress native Ethernet Editing */
  if (!SOC_WARM_BOOT(unit) && SOC_IS_JERICHO_PLUS(unit)) {
      BCMDNX_IF_ERR_EXIT(_bcm_petra_llvp_table_native_llvp_entries_init(unit));
  }

  /* init egress table entries for 20B prefix header */
  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "20byte_prefix_header_enable", 0)) {
      int prefix_index, prefix_word;
      char prefix_property[BCM_DPP_20B_PREFIX_PROPERTY_LENGTH + 5];
      uint32 prefix_data_entry[4];
      SOC_PPC_EG_ENCAP_DATA_INFO data_info;

      SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
      sal_memset(prefix_property, 0, sizeof(prefix_property));
      sal_strncpy(prefix_property, "20byte_prefix_header_0_0", sizeof(prefix_property) - 1);
      for (prefix_index = BCM_DPP_20B_PREFIX_OUTLIF_MIN; prefix_index <= BCM_DPP_20B_PREFIX_OUTLIF_MAX; ++prefix_index) {
          /* Add data memory entry for 20B prefix */
          prefix_property[BCM_DPP_20B_PREFIX_PROPERTY_LENGTH - 3] = '0' + prefix_index; /* Set first index as prefix index (1-8)*/
          for (prefix_word = 0; prefix_word <= 3; ++prefix_word) {
              prefix_property[BCM_DPP_20B_PREFIX_PROPERTY_LENGTH - 1] = '0' + prefix_word; /* Set second index as word index (0-3)*/
              prefix_data_entry[prefix_word] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, prefix_property, 0);
          }
          arad_pp_epni_prge_data_tbl_set_unsafe(unit, prefix_index * 2, (void*)prefix_data_entry); /* Add 16 LSB as 1st PRGE data memory entry */

          sal_memset(prefix_data_entry, 0, 4*4); /* Clear data */
          prefix_property[BCM_DPP_20B_PREFIX_PROPERTY_LENGTH - 1] = '4'; /* Set second index as word index 4*/
          prefix_data_entry[0] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, prefix_property, 0);
          arad_pp_epni_prge_data_tbl_set_unsafe(unit, prefix_index * 2 + 1, (void*)prefix_data_entry); /* Add 4 MSB as 2nd PRGE data memory entry */

          /* Add data entry in EEDB */
          soc_ppd_eg_encap_data_lif_entry_add(unit, prefix_index, &data_info, FALSE, 0);
      }
  }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_deinit(int unit)
{
    bcm_error_t rv;
    int is_init;
    BCMDNX_INIT_FUNC_DEFS;

    /* this unit not init, no nothing to detach; done. */
    rv = PORT_ACCESS.dpp_port_config.bcm_petra_port_init_arrays_done.get(unit, &is_init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!is_init) {
        BCM_EXIT;
    }

     /* initialize */
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_deinit,(unit));
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_IS_DETACHING(unit)) {
        rv = PORT_ACCESS.dpp_port_config.port_pp_initialized.set(unit, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_petra_port_init_arrays_done.set(unit, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_clear
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device
 *      without resetting stacking ports.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 *      BCM_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      By default ports come up enabled. They can be made to come up disabled
 *      at startup by a compile-time application policy flag in your Make.local
 *      A call to bcm_petra_port_clear should exhibit similar behavior for 
 *      non-stacking ethernet ports
 *      PTABLE initialized.
 */

int
bcm_petra_port_clear(int unit)
{
    bcm_port_config_t *port_config = NULL;
    bcm_pbmp_t reset_ports;
    bcm_port_t port;
    int rv, port_enable;
    int port_pp_initialized;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_ALLOC(port_config, sizeof(bcm_port_config_t), "bcm_petra_port_clear.port_config");
    if (!port_config) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    BCMDNX_IF_ERR_EXIT(bcm_petra_port_config_get(unit, port_config));

    /* Clear all non-stacking ethernet ports */
    BCM_PBMP_ASSIGN(reset_ports, port_config->xe);
    BCM_PBMP_REMOVE(reset_ports, SOC_PBMP_STACK_CURRENT(unit));
    if (SOC_DPP_PP_ENABLE(unit))
    {
        rv = PORT_ACCESS.dpp_port_config.port_pp_initialized.get(unit, &port_pp_initialized);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (!port_pp_initialized)
        {
            rv = bcm_petra_port_pp_init(unit);
            if (rv != BCM_E_NONE) {
              LOG_VERBOSE(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "bcm_petra_port_pp_init failed: %s"),
                           bcm_errmsg(rv)));
              BCMDNX_IF_ERR_EXIT(rv);
            }

            rv = PORT_ACCESS.dpp_port_config.port_pp_initialized.set(unit, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
        }        
    }
    PBMP_ITER(reset_ports, port) {
        LOG_DEBUG(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "bcm_petra_port_clear: unit %d port %s\n"),
                   unit, SOC_PORT_NAME(unit, port)));
  
        /*
         * A compile-time application policy may prefer to disable 
         * ports at startup. The same behavior should be observed 
         * when bcm_petra_port_clear gets called.
         */
  
#ifdef BCM_PORT_DEFAULT_DISABLE
  port_enable = FALSE;
#else
  port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */
  

  if ((rv = bcm_petra_port_enable_set(unit, port, port_enable)) < 0) {
            LOG_WARN(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Warning: Unit %d Port %s: "
                                 "Failed to %s port: %s\n"),
                      unit, SOC_PORT_NAME(unit, port),(port_enable) ? "enable" : "disable" ,bcm_errmsg(rv)));
        }
  
    }

exit:
    BCM_FREE(port_config);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_config_get
 * Purpose:g_
 *      Get port configuration of a device
 * Parameters:
 *      unit   - Device unit number
 *      config - (OUT) Structure returning port configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_port_config_get(int unit, bcm_port_config_t *config)
{
    soc_pbmp_t nif_ports;
    soc_port_t port;
    uint32     flags;
    soc_port_if_t interface_type;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(config);
    
    bcm_port_config_t_init(config);

    config->il   = PBMP_IL_ALL(unit);
    config->xl   = PBMP_XL_ALL(unit);
    config->ge   = PBMP_GE_ALL(unit);
    config->ce   = PBMP_CE_ALL(unit);
    config->xe   = PBMP_XE_ALL(unit);
    config->hg   = PBMP_HG_ALL(unit);
    config->e    = PBMP_E_ALL(unit);
    config->sfi  = PBMP_SFI_ALL(unit);
    config->port = PBMP_PORT_ALL(unit);
    config->stack_ext  = PBMP_ST_ALL(unit);
    config->cpu  = PBMP_CMIC(unit);
    config->rcy  = PBMP_RCY_ALL(unit);
    config->all  = PBMP_ALL(unit);
    config->pon  = PBMP_PON_ALL(unit);

    /* get nif ports */
    BCM_PBMP_CLEAR(config->nif);
    BCM_PBMP_CLEAR(config->sat);
    BCM_PBMP_CLEAR(config->ipsec);
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &nif_ports));
    BCM_PBMP_ITER(nif_ports, port) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (SOC_PORT_IS_NETWORK_INTERFACE(flags))
        {
            if (!(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))) {
                BCM_PBMP_PORT_ADD(config->nif,port);
            }
        }
        if (interface_type == SOC_PORT_IF_SAT)
        {
            BCM_PBMP_PORT_ADD(config->sat,port);
        }
        if (interface_type == SOC_PORT_IF_IPSEC)
        {
            BCM_PBMP_PORT_ADD(config->ipsec,port);
        }
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function    : bcm_petra_port_local_get
 * Description : Get the local port from the given GPORT ID.
 *
 * Parameters  : (IN)  unit         - BCM device number
 *               (IN)  gport        - global port identifier
 *               (OUT) local_port   - local port encoded in gport
 * Returns     : BCM_E_XXX
 */
int
bcm_petra_port_local_get(int unit, bcm_gport_t gport, bcm_port_t *local_port)
{
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(local_port);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    *local_port = gport_info.local_port;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int                     rv = BCM_E_NONE;
    soc_port_t              port_i;
    _bcm_dpp_gport_info_t   gport_info;
    int                     has_fabric_port = 0;
    uint32                  soc_sand_rv;
    int                     loopback = BCM_PORT_LOOPBACK_NONE;
    soc_port_if_t           interface = SOC_PORT_IF_NULL;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));


    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        if(!IS_SFI_PORT(unit, port_i) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port_i)) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
        }
        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_enable_set,(unit, port_i, 0, enable)));
        BCMDNX_IF_ERR_EXIT(rv);

        if (IS_SFI_PORT(unit, port_i)) {
            has_fabric_port = 1;
        }

        if(SOC_PBMP_MEMBER(PBMP_CMIC(unit), port_i)) {
            continue;
        }

        BCMDNX_IF_ERR_EXIT(bcm_petra_port_loopback_get(unit, port_i, &loopback));
        rv = _bcm_linkscan_available(unit);
        if ((loopback != BCM_PORT_LOOPBACK_NONE) && (rv == BCM_E_NONE)) {
            /*Do not force link override for ILKN ports, as this will disable the ILKN first packet SW bypass*/
            if (interface != SOC_PORT_IF_ILKN){
                rv = _bcm_link_force(unit, port_i, TRUE, enable);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }


    /*ALDWP should be configured after init time for every speed change*/
    if (has_fabric_port) {
        rv = bcm_petra_init_check(unit);
        if (rv == BCM_E_UNIT /*init*/)
        {
            /*Avoiding ALDWP config at init time*/
        } else if (rv == BCM_E_NONE)
        {
            soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_aldwp_config ,(unit));      
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    int                     rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_enable_get,(unit, gport_info.local_port, 0, enable)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_port_add_lb_speed_set(int unit, bcm_port_t port)
{
    int                     rv = BCM_E_NONE;
    int                     speed, speed_init, pcnt = 0;
    bcm_port_if_t           interface;
#ifdef BCM_LB_SUPPORT
    soc_pbmp_t modem_pbmp;
#endif

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
#ifdef BCM_LB_SUPPORT
    SOCDNX_IF_ERR_EXIT(qax_lb_modem_ports_on_same_interface_get(unit, port, &modem_pbmp));
    SOC_PBMP_COUNT(modem_pbmp, pcnt);
#endif
    if ((pcnt != 0) || (SOC_PORT_IF_LBG == interface)) {
        BCMDNX_IF_ERR_EXIT(soc_pm_default_speed_get(unit, port, &speed));
        speed_init = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, speed);
        if ((speed_init != -1) && (speed_init <= SOC_TMC_LB_LBG_SPEED_MAX)) {
            speed = speed_init;
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_validate,(unit, TRUE, port, 0, speed)));

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, speed));

        /* update LBG FIFO, Credit Size */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_set,(unit, port, speed)));
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_port_speed_lb_set(int unit, bcm_port_t port, int speed)
{
    int                     rv = BCM_E_NONE;
    int                     speed_get;
    uint32                  out_port_priority;
    int                     core = 0, mc_reserved_pds;
    bcm_gport_t             gport;
    bcm_cosq_threshold_t    threshold;

    BCMDNX_INIT_FUNC_DEFS;

    if (speed > SOC_TMC_LB_LBG_SPEED_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("speed can't be greater than %d\n"), SOC_TMC_LB_LBG_SPEED_MAX));
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_get,(unit, port, &speed_get)));
    if (speed_get != speed) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_validate,(unit, TRUE, port, 0, speed)));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, speed));

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &out_port_priority));

        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_fc_port_threshold_init(unit, port, &mc_reserved_pds, 1));

        /* increment sp0 reserved pds */
        BCM_COSQ_GPORT_CORE_SET(gport, core); /* global threshold*/
        threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_EGRESS;
        threshold.type = bcmCosqThresholdAvailablePacketDescriptors;
        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_threshold_get(unit, gport, -1, &threshold));
        threshold.value += (mc_reserved_pds*out_port_priority);
        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_threshold_set(unit, gport, -1, &threshold));

        /* update hr handle */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_hr_handle(unit, port, 0));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_hr_handle(unit, port, 1));

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_to_interface_egress_map_set, (unit, port)));

        /* update PQP/FQP entries */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_q_nif_cal_set,(unit)));

        /* update NRDY TH */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_free(unit, port));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_allocate(unit, port));

        /* update LBG FIFO, Credit Size */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_set,(unit, port, speed)));
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_speed_set(int unit, bcm_port_t port, int speed)
{
    int                     rv = BCM_E_NONE;
    soc_port_t              port_i;
    soc_port_t              fap_port;
    int                     soc_sand_rv;
    int                     has_fabric_port = 0;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_cosq_gport_info_t   cosq_gport_info;
    uint32                  port_flags = 0, egress_offset = 0;
    uint32                  tm_port = 0;
    uint32                  sch_rate;
    uint32                  egq_rate;
    uint32                  kbits_sec_max;
    uint32                  dummy1;
    uint32                  dummy2;
    int                     core = 0;
    int                     is_channelized=0;
    int                     old_speed=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    if (speed == 0) {
        /* if speed is 0, set the port speed to max */
        BCMDNX_IF_ERR_EXIT(bcm_petra_port_speed_max(unit, port, &speed));   
    }
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        if (!IS_SFI_PORT(unit, port_i)) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &port_flags));
        }

        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
            if (!SOC_PORT_IS_LB_MODEM(port_flags)) {
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port_i, &tm_port, &core)));
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
            }
        }

        if (SOC_IS_QAX(unit) && 
            ((SOC_DPP_CONFIG(unit)->qax->link_bonding_enable && IS_LBG_PORT(unit,port_i)) || 
             (SOC_DPP_CONFIG(unit)->qax->lbi_en && (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE)))) {
             BCMDNX_IF_ERR_EXIT(_bcm_petra_port_speed_lb_set(unit, port_i, speed));
        }
        else {
            /* Get the old speed of the port */
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_speed_get(unit, port_i, &old_speed));

            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_speed_set,(unit, port_i, speed));
             BCMDNX_IF_ERR_EXIT(rv);

             if (!IS_SFI_PORT(unit, port_i) && !SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(port_flags)) {
                /* update PQP/FQP entries */
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_q_nif_cal_set,(unit)));

                /* update NRDY TH */
                if (SOC_IS_JERICHO(unit)) {
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_free(unit, port_i));
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_allocate(unit, port_i));
                }

                /* Check if port is channalized */
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port_i, &is_channelized));
                if ((!is_channelized) && (speed!=old_speed)) {

                    sch_rate = speed + (speed / 100) * 5; /* 5% speedup */
                    egq_rate = speed + (speed / 100) * 1; /* 1% speedup */

                    /* Get hanlde for Local Gport */
                    cosq_gport_info.in_gport = port_i;
                    BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,&cosq_gport_info));
                    BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_port_get(unit,cosq_gport_info.out_gport,0,&fap_port));

                    /* Update EGQ interface bandwidth */
                    BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_get(unit,fap_port,0,&dummy1,&kbits_sec_max,&dummy2));
                    /* Add the difference between the old speed and the new speed to the interface bandwidth */
                    kbits_sec_max+= (speed - old_speed)*1000;
                    if ((kbits_sec_max + egq_rate*1000) <= SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit)) {
                        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max + egq_rate*1000),0));
                    } else {
                        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit),0));
                    }

                    /* Get hanlde for E2E Gport */
                    cosq_gport_info.in_gport = port_i;
                    BCMDNX_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPort,&cosq_gport_info));
                    BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_port_get(unit,cosq_gport_info.out_gport,0,&fap_port));

                    /* Update Scheduler bandwidth */
                    BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_get(unit,fap_port,0,&dummy1,&kbits_sec_max,&dummy2));
                    /* Add the difference between the old speed and the new speed to the Scheduler bandwidth */
                    kbits_sec_max+= (speed - old_speed)*1000;
                    if ((kbits_sec_max + sch_rate*1000) <= SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit)) {
                        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max + sch_rate*1000),0));
                    } else {
                        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit),0));
                    }
                }
            }
        }

        if (IS_SFI_PORT(unit, port)) {
            has_fabric_port = 1;
        }
    }

    /*ALDWP should be configured after init time for every speed change*/
    if (has_fabric_port) {
        rv = bcm_petra_init_check(unit);
        if (rv == BCM_E_UNIT /*init*/)
        {
            /*Avoiding ALDWP config at init time*/
        } else if (rv == BCM_E_NONE)
        {
            soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_aldwp_config ,(unit));      
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_speed_get(int unit, bcm_port_t port, int *speed)
{
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    uint32                  port_flags = 0, egress_offset = 0;
    uint32                  tm_port = 0;
    int                     core = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(speed);

    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &port_flags));

        if (!SOC_PORT_IS_LB_MODEM(port_flags)) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
        }
    }

    if (SOC_IS_QAX(unit) && 
        ((SOC_DPP_CONFIG(unit)->qax->link_bonding_enable && IS_LBG_PORT(unit,port)) || 
         (SOC_DPP_CONFIG(unit)->qax->lbi_en && (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE)))) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_get,(unit, port, speed)));
    }
    else
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_speed_get,(unit, gport_info.local_port, speed)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_speed_max(int unit,
                       bcm_port_t port,
                       int *speed)
{
    soc_info_t              *si;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(speed);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if(gport_info.local_port >= SOC_MAX_NUM_PORTS)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Couldn't extract valid local port from param port")));
    }
    si  = &SOC_INFO(unit);
    *speed = si->port_speed_max[gport_info.local_port];

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_interface_set(int unit,
                 bcm_port_t port,
                 bcm_port_if_t intf)
{
    soc_port_t port_i;
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_interface_set,(unit, port_i, intf)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_interface_get(int unit,
                 bcm_port_t port,
                 bcm_port_if_t *intf)
{
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(intf);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
    
    rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_interface_get,(unit, gport_info.local_port, intf)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_link_state_get(int unit,
                              bcm_port_t port,
                              uint32 flags,
                              bcm_port_link_state_t *state)
{
    int status, latch_down, rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(state);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
    
    rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_link_state_get,(unit, gport_info.local_port, 1, &status, &latch_down)));
    BCMDNX_IF_ERR_EXIT(rv);

    state->status = status;
    state->latch_down = latch_down;

exit:
    BCMDNX_FUNC_RETURN;

}

int 
bcm_petra_port_encap_set(int unit, bcm_port_t port, int mode)
{
    int is_supported_mode = FALSE;
    uint32 is_initialized = FALSE;
    bcm_port_t port_i;
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    /* This API is invalid for port which already initialized */
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
         BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port_i, &is_initialized));
         if(is_initialized){
             BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Can't set encap mode when the port is initailized")));
         }
     }

    rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_is_supported_encap_get,(unit, mode, &is_supported_mode)));
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_supported_mode){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Mode %d not supported"), mode));
    } 

    /* Only store in DB, actul encap will be updated when port is initialized */
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_set(unit, port_i, (soc_encap_mode_t)mode));
    }
    
exit:
    BCMDNX_FUNC_RETURN;

}

int 
bcm_petra_port_encap_get(int unit, bcm_port_t port, int *mode)
{
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(mode);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
    
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_get(unit, gport_info.local_port, (soc_encap_mode_t *)mode));

exit:
    BCMDNX_FUNC_RETURN; 
}


int
bcm_petra_port_interface_config_get(int unit,
                                    bcm_port_t port,
                                    bcm_port_interface_config_t *config)
{
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;
        
    BCMDNX_NULL_CHECK(config);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &(config->interface)));
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, gport_info.local_port, &(config->channel)));
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, gport_info.local_port, &(config->phy_port)));
    
exit:    
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_lane_index_get(int unit, soc_port_t port, uint32 *lane_index)
{
    uint32  phy_port, lane;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit) && (IS_SFI_PORT(unit,port) || SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port))) {
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        lane = phy_port;
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &lane)));
    }

    *lane_index = lane;

exit:
    SOCDNX_FUNC_RETURN;
}

int
bcm_petra_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    bcm_error_t rv = BCM_E_NONE;
    int lb_start;
    uint32 cl72_start;
    soc_dcmn_loopback_mode_t dcmn_loopback;
    _bcm_dpp_gport_info_t   gport_info;
    soc_port_t port_i;
    uint32 lane_index=0;
    uint8 is_config_cl72 = 1;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
#if defined(BCM_ARAD_SUPPORT)
    int link;
    int is_linkscan_available;
#endif

    BCMDNX_INIT_FUNC_DEFS;

    /* for QUX, Viper doesn't support CL72 */
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(_bcm_petra_port_lane_index_get(unit, port, &lane_index));
        if ((lane_index > 8) && (lane_index < 37)) {
            is_config_cl72= 0;
        }
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    }

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
#if defined(BCM_ARAD_SUPPORT)
        rv = _bcm_linkscan_available(unit);
        if (rv == BCM_E_NONE) {
            is_linkscan_available = TRUE;
        } else {
            is_linkscan_available = FALSE;
        }
        if (is_linkscan_available == TRUE) {
            rv = bcm_petra_port_enable_get(unit, port_i, &link);
            BCMDNX_IF_ERR_EXIT(rv);
            /*
             * Always force link before changing hardware to avoid
             * race with the linkscan thread.
             * Exception: In case of ILKN we are not changing linkscan, as this will disable the ILKN first packet SW bypass
             */
            if (interface != SOC_PORT_IF_ILKN){
                if (loopback != BCM_PORT_LOOPBACK_NONE) {
                    rv = _bcm_link_force(unit, port_i, TRUE, FALSE);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
#endif

        switch(loopback) {
            case BCM_PORT_LOOPBACK_MAC:
                if(IS_SFI_PORT(unit, port_i)) {
                    dcmn_loopback = soc_dcmn_loopback_mode_mac_async_fifo;
                } else {
                    dcmn_loopback = soc_dcmn_loopback_mode_mac_outer;
                }
                break;
            case BCM_PORT_LOOPBACK_PHY:
                dcmn_loopback = soc_dcmn_loopback_mode_phy_gloop;
                break;
            case BCM_PORT_LOOPBACK_PHY_REMOTE:
                dcmn_loopback = soc_dcmn_loopback_mode_phy_rloop;
                break;
            case BCM_PORT_LOOPBACK_NONE:
                dcmn_loopback = soc_dcmn_loopback_mode_none;
                break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported loopback %d"), loopback)); 
                break;
        }

        /* Disable CL72 when closing loopback */
        BCMDNX_IF_ERR_EXIT(bcm_petra_port_loopback_get(unit, port_i, &lb_start));
        if ((is_config_cl72 == 1) && (loopback != 0) && (lb_start == 0) && (ext_phy_ctrl[unit] == NULL || EXT_PHY_SW_STATE(unit, port_i) == NULL)) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_phy_control_get(unit, port_i, BCM_PORT_PHY_CONTROL_CL72, &cl72_start));
            SOC_DPP_PORT_PARAMS(unit).cl72_configured[port_i] = cl72_start;
            if (cl72_start == 1) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_phy_control_set(unit, port_i, BCM_PORT_PHY_CONTROL_CL72, 0)); 
            }
        }

        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_loopback_set,(unit, port_i, dcmn_loopback));

        BCMDNX_IF_ERR_EXIT(rv);
        BCM_DPP_UNIT_CHECK(unit);

#if defined(BCM_ARAD_SUPPORT)
        if (is_linkscan_available == TRUE) {
            /*Do not force link override for ILKN ports, as this will disable the ILKN first packet SW bypass*/
            if (interface != SOC_PORT_IF_ILKN){
                if ((loopback == BCM_PORT_LOOPBACK_NONE) || BCM_FAILURE(rv)) {
                    rv = _bcm_link_force(unit, port_i, FALSE, FALSE);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    rv = _bcm_link_force(unit, port_i, TRUE, link);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
#endif

        if ((is_config_cl72 == 1) && (loopback == 0) && (lb_start != 0) && (ext_phy_ctrl[unit] == NULL || EXT_PHY_SW_STATE(unit, port_i) == NULL)) {
            cl72_start = SOC_DPP_PORT_PARAMS(unit).cl72_configured[port_i];
            if (cl72_start == 1) {
                rv = bcm_petra_port_phy_control_set(unit, port_i, BCM_PORT_PHY_CONTROL_CL72, 1);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    if (unit < BCM_LOCAL_UNITS_MAX) { 
    }
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_loopback_get(int unit, bcm_port_t port, int *loopback)
{
    bcm_error_t rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    soc_dcmn_loopback_mode_t dcmn_loopback;
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_loopback_get,(unit, gport_info.local_port, &dcmn_loopback));
    BCMDNX_IF_ERR_EXIT(rv);

    switch(dcmn_loopback) {
        case soc_dcmn_loopback_mode_none:
            *loopback = BCM_PORT_LOOPBACK_NONE;
            break;
        case soc_dcmn_loopback_mode_mac_outer:
        case soc_dcmn_loopback_mode_mac_pcs:
        case soc_dcmn_loopback_mode_mac_async_fifo:
            *loopback = BCM_PORT_LOOPBACK_MAC;
            break;
        case soc_dcmn_loopback_mode_phy_gloop:
            *loopback = BCM_PORT_LOOPBACK_PHY;
            break;
        case soc_dcmn_loopback_mode_phy_rloop:
            *loopback = BCM_PORT_LOOPBACK_PHY_REMOTE;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unrecognized loopback type %d"), dcmn_loopback)); 
            break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * returns gport of type "local" or "mod:port"
 * This reurns handle to port. Not to Interface. From a port the interface handle is retreived
 * via bcm_petra_fabric_port_get() API.
 * Following internal functionality required by other modules (internal functions)
 *    - convert the port to "fap port"
 *    - get the port type (e.g. to indicate that some operations cannot occur on some port types)
 */
int
bcm_petra_port_internal_get(int unit, uint32 flags, int internal_ports_max, bcm_gport_t *internal_gport, int *internal_ports_count)
{
    int count = 0;
    bcm_gport_t gport;
    soc_info_t  *si;
    int modid;
    int cores, core_i;
    BCMDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    if (flags & BCM_PORT_INTERNAL_RECYCLE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("recycle ports are no longer internal - they are available in pbmp")));
    }

    if((flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE0) &&  
        (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE1)){
        cores = SOC_CORE_ALL;
    } else if(flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE0) {
        cores = 0;
    } else if (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE1) {
        if(SOC_DPP_DEFS_GET(unit, nof_cores) < 2) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only single core supported for the device")));
        }
        cores = 1;
    } else {
        cores = SOC_CORE_ALL;
    }

    BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &modid));

    SOC_DPP_CORES_ITER(cores, core_i) {
        if ((flags & BCM_PORT_INTERNAL_EGRESS_REPLICATION) && (NUM_ERP_PORT(unit) > 0)) {
            if (count >= internal_ports_max) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("interl_port_max < number of internal ports (now adding ERP)")));
            }
            BCM_GPORT_LOCAL_SET(gport, si->erp_port[core_i]);
            internal_gport[count++] = gport;
        }

        if ((flags & BCM_PORT_INTERNAL_OLP)  && (si->olp_port[core_i] > 0)) {
            if (count >= internal_ports_max) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("interl_port_max < number of internal ports (now adding OLP)")));
            }
            BCM_GPORT_LOCAL_SET(gport, si->olp_port[core_i]);
            internal_gport[count++] = gport;
        }

        if ((flags & BCM_PORT_INTERNAL_OAMP)  && (NUM_OAMP_PORT(unit) > 0)) {
            if (count >= internal_ports_max) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("interl_port_max < number of internal ports (now adding OAMP)")));
            }
            BCM_GPORT_LOCAL_SET(gport, si->oamp_port[core_i]);
            internal_gport[count++] = gport;
        }
    }
    
    *internal_ports_count = count;

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_global_control_set(int unit, bcm_port_control_t type, int value);

STATIC int
_bcm_petra_port_fabric_control_set(int unit, bcm_port_t port,
                                   bcm_port_control_t type, int value)
{
    bcm_error_t rv = BCM_E_NONE;
    soc_dcmn_port_prbs_mode_t prbs_mode;
    pbmp_t prbs_mac_mode;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    rv = PORT_ACCESS.prbs_mac_mode.get(unit, &prbs_mac_mode);
    BCMDNX_IF_ERR_EXIT(rv);
    prbs_mode = SOC_PBMP_MEMBER(prbs_mac_mode, port) ? soc_dcmn_port_prbs_mode_mac : soc_dcmn_port_prbs_mode_phy;
       
    switch (type) {
        case bcmPortControlPCS:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_control_pcs_set, (unit, port, value)); 
            BCMDNX_IF_ERR_EXIT(rv);

            /*ALDWP should be configured at init time for every pcs change*/
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_fabric_aldwp_config ,(unit));      
            BCMDNX_IF_ERR_EXIT(rv);

            break;

        case bcmPortControlLinkDownPowerOn:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_power_set,(unit, port, 0, value ? soc_dcmn_port_power_on :  soc_dcmn_port_power_off));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlStripCRC:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_strip_crc_set,(unit, port, value ? 1 :  0));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlRxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_rx_enable_set,(unit, port, 0, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlTxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_tx_enable_set,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsPolynomial:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_polynomial_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;;

        case bcmPortControlPrbsTxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_tx_enable_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;;

        case bcmPortControlPrbsRxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_rx_enable_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;;

        case bcmPortControlPrbsTxInvertData:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_tx_invert_data_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;;

        case bcmPortControlPrbsMode:
            if(0 == value) { /*PRBS PHY MODE*/
                SOC_PBMP_PORT_REMOVE(prbs_mac_mode, port); 
            } else if (1 == value){ /*PRBS MAC MODE*/
                SOC_PBMP_PORT_ADD(prbs_mac_mode, port); 
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid value")));
            }
            rv = PORT_ACCESS.prbs_mac_mode.set(unit, prbs_mac_mode);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlLowLatencyLLFCEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_low_latency_set,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlFecErrorDetectEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_fec_error_detect_set,(unit,port,value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;
       case bcmPortControlLlfcCellsCongestionIndEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_extract_cig_from_llfc_enable_set,(unit,port,value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));
            break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_eee_control_set(int unit, bcm_port_t port,
                                bcm_port_control_t type, int value)
{
    bcm_error_t rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    switch(type) {
        case bcmPortControlEEEEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_enable_set,(unit, gport_info.local_port, value ? 1 : 0));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEEStatisticsClear:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_statistics_clear,(unit, gport_info.local_port));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEETransmitIdleTime:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_tx_idle_time_set,(unit, gport_info.local_port, (uint32)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEETransmitWakeTime:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_tx_wake_time_set,(unit, gport_info.local_port, (uint32)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEEEventCountSymmetric:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_event_count_symmetric_set,(unit, gport_info.local_port, (uint32)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEELinkActiveDuration:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_link_active_duration_set,(unit, gport_info.local_port, (uint32)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;


        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The api is not available for the required type\n")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_nif_control_set(int unit, bcm_port_t port,
                                bcm_port_control_t type, int value)
{
    bcm_error_t rv = BCM_E_NONE;
    soc_error_t soc_rv = SOC_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    soc_port_t port_ndx;
    int is_pcs_loopback=0;
    SOC_TMC_FC_ENABLE_BITMAP cfc_enables;
    int is_interface = 0;
    SOC_TMC_FC_INBND_MODE fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_DISABLED;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    switch(type) {
        case bcmPortControlLinkDownPowerOn:
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_enable_set,(unit, port_ndx, 1, value));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        case bcmPortControlPrbsPolynomial:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_set */
            rv = bcm_petra_port_phy_control_set(unit, port, SOC_PHY_CONTROL_PRBS_POLYNOMIAL, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsTxInvertData:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_set */
            rv = bcm_petra_port_phy_control_set(unit, port, SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsTxEnable:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_set */
            rv = bcm_petra_port_phy_control_set(unit, port, SOC_PHY_CONTROL_PRBS_TX_ENABLE, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsRxEnable:

            if (SOC_IS_JERICHO(unit)) {
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_is_pcs_loopback,(unit, port_ndx, &is_pcs_loopback));
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (is_pcs_loopback) {
                        _bsl_warn(_BSL_BCM_MSG("Warning: PCS loopback enabled"));
                    }
                }
            }

            /* call with original port - gport will be handled by bcm_petra_port_phy_control_set */
            rv = bcm_petra_port_phy_control_set(unit, port, SOC_PHY_CONTROL_PRBS_RX_ENABLE, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsRxStatus:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_set */
            rv = bcm_petra_port_phy_control_set(unit, port, SOC_PHY_CONTROL_PRBS_RX_STATUS, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
 
       /* Flow Control */
        case bcmPortControlPFCRefreshTime:
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_pfc_refresh_set,(unit, port_ndx, value));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        case bcmPortControlPFCTransmit:
        case bcmPortControlLLFCTransmit:
        case bcmPortControlSAFCTransmit:

            SHR_BITCLR_RANGE(cfc_enables.bmp, 0, SOC_TMC_FC_NOF_ENABLEs);
            if(value == 0) {
                fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_DISABLED;
            }
            else {
                if(type == bcmPortControlPFCTransmit) {
                    fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_PFC;
                    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_PFC_VSQ_TO_NIF_EN);
                    if (SOC_IS_JERICHO(unit)) {  
                        SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN);
                    }
                }
                if(type == bcmPortControlSAFCTransmit) {
                    fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_SAFC;
                    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_PFC_VSQ_TO_NIF_EN);
                    if (SOC_IS_JERICHO(unit)) {  
                        SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN);
                    }
                }
                if(type == bcmPortControlLLFCTransmit) {
                    fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_LL;
                    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN);
                    if (SOC_IS_JERICHO(unit)) {  
                        SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN);
                    }
                }
            }
        
            soc_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fc_inbnd_mode_set,(unit, port, TRUE, fc_inbnd_mode));
            if (soc_rv != SOC_E_NONE) {
                rv = soc_rv;
                BCMDNX_IF_ERR_EXIT(rv);
            }

            soc_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fc_enables_set,(unit, &cfc_enables, &cfc_enables));
            if (soc_rv != SOC_E_NONE) {
                rv = soc_rv;
                BCMDNX_IF_ERR_EXIT(rv);
            }

            break;

        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        case bcmPortControlSAFCReceive:

            if(value == 0) {
                fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_DISABLED;
            }
            else {
                if(type == bcmPortControlPFCReceive) {
                    fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_PFC;
                }
                if(type == bcmPortControlSAFCReceive) {
                    fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_SAFC;
                }
                if(type == bcmPortControlLLFCReceive) {
                    fc_inbnd_mode = SOC_TMC_FC_INBND_MODE_LL;
                }
            }

            soc_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fc_inbnd_mode_set,(unit, port, FALSE, fc_inbnd_mode));
            if (soc_rv != SOC_E_NONE) {
                rv = soc_rv;
                BCMDNX_IF_ERR_EXIT(rv);
            }

            break;

        case bcmPortControlLinkFaultLocal:
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_local_fault_clear,(unit, port_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        case bcmPortControlLinkFaultRemote:
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_remote_fault_clear,(unit, port_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        case bcmPortControlPadToSize:
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_pad_size_set,(unit, port_ndx, value));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        case bcmPortControlTxEnable:
            is_interface = 0;
            if (BCM_GPORT_IS_SET(port)) {
                if (BCM_GPORT_IS_LOCAL(port)) {
                    port = BCM_GPORT_LOCAL_GET(port);
                    if (BCM_DPP_PORT_IS_INTERFACE(port)) {
                        port = port - BCM_DPP_PORT_INTERFACE_START; 
                        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_tx_enable_set,(unit, port, value));
                        BCMDNX_IF_ERR_EXIT(rv);
                        is_interface = 1;
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid gport for bcmPortControlTxEnable")));
                }  
            } 
            if (!is_interface) {
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_action_cmd_cosq_flush,(unit, port_ndx, value));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
            break;
       case bcmPortControlStatOversize:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_stat_oversize_set ,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;


        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The api is not available for the required type\n")));
    }
    

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_pp_port_control_set(int unit, bcm_port_t port,
                           bcm_port_control_t type, int value);

STATIC int _bcm_petra_port_inlif_profile_set(
   const int unit, 
   const bcm_port_t port, 
   const uint32 arg, 
   const SOC_OCC_MGMT_INLIF_APP kind,

   /* Allocated lif_info */
   SOC_PPC_LIF_ENTRY_INFO *lif_info);

STATIC int
_bcm_petra_pp_port_control_get(int unit, bcm_port_t port,
                            bcm_port_control_t type, int *value);

STATIC int _bcm_petra_port_inlif_profile_get(
   const int unit,
   const bcm_port_t port,
   uint32 *arg,
   const SOC_OCC_MGMT_INLIF_APP kind,
   const uint8 all_kinds,

   /* Allocated lif_info */
   SOC_PPC_LIF_ENTRY_INFO *lif_info);

STATIC int
_bcm_petra_port_control_set(int unit, bcm_port_t port,
                            bcm_port_control_t type, int value)
{
    bcm_error_t rv =  BCM_E_NONE;
    uint32 soc_sand_rv;
    int is_interface = 0;
    int is_local_port = 0;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    /* Global type check */
    switch (type)
    {
          /* PP */
          case bcmPortControlMpls:
          case bcmPortControlIP4:
          case bcmPortControlIP6:
          case bcmPortControlDoNotCheckVlan:
          case bcmPortControlMacInMac:
          case bcmPortControlL2Move:
          case bcmPortControlBridge:
          case bcmPortControlPrivateVlanIsolate:
          case bcmPortControlFloodUnknownUcastGroup:
          case bcmPortControlFloodUnknownMcastGroup:
          case bcmPortControlFloodBroadcastGroup:
          case bcmPortControlDiscardMacSaAction:
          case bcmPortControlUnknownMacDaAction:
          case bcmPortControlTrillDesignatedVlan:
          case bcmPortControlL2SaAuthenticaion:
          case bcmPortControlLocalSwitching:
          case bcmPortControlMplsFRREnable:
          case bcmPortControlMplsContextSpecificLabelEnable:
          case bcmPortControlTrill:
          case bcmPortControlEgressFilterDisable:
          case bcmPortControlOverlayRecycle:
          case bcmPortControlEvbType:
          case bcmPortControlVMac:
          case bcmPortControlErspanEnable:
          case bcmPortControlFcoeNetworkPort:
          case bcmPortControlFcoeFabricId:
          case bcmPortControlMplsExplicitNullEnable:
          case bcmPortControlOamDefaultProfile:
          case bcmPortControlOamDefaultProfileEgress:
          case bcmPortControlMplsEncapsulateExtendedLabel:
          case bcmPortControlExtenderType:
          case bcmPortControlNonEtagDrop:
          case bcmPortControl1588P2PDelay:
          case bcmPortControlMplsSpeculativeParse:
          case bcmPortControlTCPriority:
          case bcmPortControlUntaggedVlanMember:
          case bcmPortControlMPLSEncapsulateAdditionalLabel:
          case bcmPortControlForceBridgeForwarding:
          case bcmPortControlLogicalInterfaceSameFilter:
          case bcmPortControlIP4Mcast:
          case bcmPortControlIP6Mcast:
              /* PP Configuration. Port check is done in pp port API */
              rv = _bcm_petra_pp_port_control_set(unit, port, type, value);    
              BCMDNX_IF_ERR_EXIT(rv);   
              break;

          /* TM */
          case bcmPortControlPrbsPolynomial:
          case bcmPortControlPrbsTxEnable:
          case bcmPortControlPrbsRxEnable:
          case bcmPortControlLinkDownPowerOn:
          case bcmPortControlTxEnable:
          case bcmPortControlPrbsTxInvertData:
          case bcmPortControlPrbsRxStatus:
          case bcmPortControlPFCTransmit:
          case bcmPortControlPFCReceive:
          case bcmPortControlLLFCReceive:
          case bcmPortControlLLFCTransmit:
          case bcmPortControlSAFCReceive:
          case bcmPortControlSAFCTransmit:
          case bcmPortControlPFCRefreshTime:
          case bcmPortControlLinkFaultLocal:
          case bcmPortControlLinkFaultRemote:
          case bcmPortControlPadToSize:
          case bcmPortControlStatOversize:
              rv = _bcm_petra_port_nif_control_set(unit, port, type, value);
              BCMDNX_IF_ERR_EXIT(rv);   
              break;

        case bcmPortControlEEEEnable:
        case bcmPortControlEEEStatisticsClear:
        case bcmPortControlEEETransmitIdleTime:
        case bcmPortControlEEETransmitWakeTime:
        case bcmPortControlEEEEventCountSymmetric:
        case bcmPortControlEEELinkActiveDuration:
            rv = _bcm_petra_port_eee_control_set(unit, port, type, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlRxEnable :
            if (BCM_GPORT_IS_SET(port)) {
                if (BCM_GPORT_IS_LOCAL(port)) {
                    port = BCM_GPORT_LOCAL_GET(port);
                    if (BCM_DPP_PORT_IS_INTERFACE(port)) {
                        port = port - BCM_DPP_PORT_INTERFACE_START;
                        is_interface = 1;
                    } else {
                        is_local_port = 1; 
                    }
                }
            } else {
                is_local_port = 1;
            }
            if ((is_interface || is_local_port) && (port < FABRIC_LOGICAL_PORT_BASE(unit)) ) {
                
                {
                    uint8
                        traffic_enable = 0;
                    SOC_SAND_U32_RANGE 
                        size_range;
                    if (value == 0)
                    {
                        if ((IS_TDM_PORT(unit,port) == TRUE))
                        {
                            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_enable_traffic_get,(unit, &traffic_enable)));
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                                
                            soc_sand_SAND_U32_RANGE_clear(&size_range);
                            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_stand_size_range_get,(unit, &size_range)));
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                            /* 
                             * When disabling the port rx, 
                             * If this is a tdm port and the traffic is enabled
                             * Then the min size tdm filter (register IRE_TDM_SIZEr, field TDM_MIN_SIZEf)
                             * must be set to 192B or above.
                             */
                            
                            if ((size_range.start < BCM_DPP_PORT_RX_DISABLE_TDM_MIN_SIZE) &&
                                (traffic_enable) &&
                                (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "allow_modifications_during_traffic", 0) == 0))
                            {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, 
                                                    (_BSL_BCM_MSG("unit %d, When disabling traffic for tdm ports, tdm min size filter must be set to over 192B, currently it is set to %d"), unit ,size_range.start));
                            }   
                        }   
                    }
                }
                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_rx_enable_set,(unit, port, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid gport for bcmPortControlRxEnable")));
            }
            break; 
        case bcmPortControlIlknRxPathEnable:
            {
                _bcm_dpp_gport_info_t gport_info;

                soc_port_if_t interface;
                uint32 sw_db_flags = 0;
                uint32 value_get = 0;

                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &interface));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, gport_info.local_port, &sw_db_flags));

                if (interface == SOC_PORT_IF_ILKN && (!SOC_PORT_IS_ELK_INTERFACE(sw_db_flags))){
                    /*
                     * Check if need to enable/disable ILKN RX bypass interface. If needed, do the operation.
                     */
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_bypass_interface_rx_check_and_enable, (unit, gport_info.local_port)));

                    /* 
                     * If the value_get different from value request, it indicates enable/disable operation was failed.
                     */
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_bypass_interface_rx_enable_get, (unit, gport_info.local_port, &value_get)));

                    if (value_get != value){
                        if (value) { /* user was asking to enable */
                            BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("ILKN port = %d can't be enabled since not all conditions to set Rx was met (either phy signal lock is not up and/or Tx ILKN is not set, yet"), gport_info.local_port));
                        }
                        else { /* user was asking to disable */
                            BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("ILKN port = %d can't be disabled since not all conditions to reset Rx was met (i.e. currently both phy signal lock is up and Tx ILKN is set, yet"), gport_info.local_port));
                        }
                    }
                }
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port (should be IlKN port that is not ELK)")));
                }
            }
            break;
        case bcmPortControlEgressModifyDscp:
            if(!(BCM_GPORT_IS_LOCAL(port))) {
                if (SOC_IS_ARADPLUS(unit)) {
                    int simple_mode = soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0);

                    if (value < 0 || value > 1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                    }

                    if (simple_mode) {
                        /* In simple mode the port is gport. */
                    
                        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_class_set.lif_info");
                        if (lif_info == NULL) {        
                            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                        }

                        rv = _bcm_petra_port_inlif_profile_set(unit, port, value, SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING, lif_info);
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));

                    } else {
                        /* In advanced mode the port is the inlif profile. */
                        uint32 mask;
                        SOC_PPC_EG_QOS_GLOBAL_INFO info;

                        if ((port < 0) || (port > BCM_PORT_INLIF_PROFILE_MAX)) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid inlif profile")));
                        }
                        /* We can only use the 2 lsbs for DSCP/EXP marking (2 lsbs are passed to egress only). */
                        port &= BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;
                        mask = (1 << port);
                        SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);
                        soc_sand_rv = soc_ppd_eg_qos_global_info_get(unit, &info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        if (value == 0) {
                            info.in_lif_profile_bitmap &= ~mask;
                        } else {
                            info.in_lif_profile_bitmap |= mask;
                        }

                        soc_sand_rv = soc_ppd_eg_qos_global_info_set(unit, &info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                } else {
                    BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
                }
            }else {
                if (SOC_IS_QUX(unit)) {
                    uint32 pp_port = 0;
                    int core = 0;

                    port = BCM_GPORT_LOCAL_GET(port);
                    if(port < 0  || port > 255) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port (should less than 256)")));
                    }
                    if (value < 0 || value > 1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                    }
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
                    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_eg_qos_marking_set,(unit, pp_port, value)));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
                }
            }
            break;

        case bcmPortControlExtenderEnable:
            {
                _bcm_dpp_gport_info_t   gport_info;
                bcm_port_t port_ndx = 0;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
                BCMDNX_IF_ERR_EXIT(rv);

                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                    break;
                }

                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_extender_mapping_enable_set,(unit, port_ndx, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            break;
        case bcmPortControlPONEnable:
            {
                _bcm_dpp_gport_info_t   gport_info;
                bcm_port_t port_ndx = 0;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;
        
                rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
                BCMDNX_IF_ERR_EXIT(rv);
        
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                    break;
                }
        
                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_pon_ports_enable_set,(unit, port_ndx, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            break;

        case bcmPortControlReserved280:
            if (SOC_DPP_CONFIG(unit)->pp.egress_full_mymac_1) {
                _bcm_dpp_gport_info_t   gport_info;
                bcm_port_t port_ndx = 0;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
                BCMDNX_IF_ERR_EXIT(rv);

                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));
                    break;
                }

                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_tm_port_var_set, (unit, port_ndx, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0)) {
                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_tm_port_var_set, (unit, port, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ch_enable", 0)) {
                soc_sand_rv = arad_egr_prog_editor_pp_profile_set(unit, port, (value ? ARAD_PRGE_PP_SELECT_CH_CONN_TO_FPGA : ARAD_PRGE_PP_SELECT_PP_COPY_HEADER));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
        break;
        case bcmPortControlReserved281:
            if (SOC_DPP_CONFIG(unit)->pp.custom_feature_vrrp_scaling_tcam) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                vtt_in_pp_port_conf_tbl.vlan_translation_profile = value ? (SOC_IS_JERICHO(unit) ? 11 : 5) : SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else if (SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id || SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id || SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                
                /* For  port termination - Port is configured with profile in order for program selection to happen only for this specific port   */ 
                vtt_in_pp_port_conf_tbl.vlan_translation_profile = value ? SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION : SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                vtt_in_pp_port_conf_tbl.termination_profile = value ? SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_TERMINATION : SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0)) {
                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_prog_editor_profile_set, (unit, 0, port, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tdm_coe_conn", 0)) {
                BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_pp_profile_set(unit, port, (value ? ARAD_PRGE_PP_SELECT_TDM_COE_INTER_CONN : ARAD_PRGE_PP_SELECT_PP_COPY_HEADER)));    
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
        break;
        case bcmPortControlTrustCustomHeaderPri:
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mixed_qos_model_enable", 0)) {
                BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_pp_profile_set(unit, port, (value ? ARAD_PRGE_PP_SELECT_MIXED_QOS_MODEL : ARAD_PRGE_PP_SELECT_PP_COPY_HEADER)));    
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }        
        break;
        case bcmPortControlPreserveDscpIngress:
            if ((SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_INLIF_CTRL_EN) {
                if (value < 0 || value > 1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                }

                /* lif_info released at exit jump point */
                BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_class_set.lif_info");
                if (lif_info == NULL) {        
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }                
                
                rv = _bcm_petra_port_inlif_profile_set(unit, port, value, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP, lif_info);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));
             } else {
                BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
            }
 
            break;            
        case bcmPortControlPreserveDscpEgress:
            if ((SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_OUTLIF_CTRL_EN) {
                if (value < 0 || value > 1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                }
                
                rv = _bcm_petra_port_outlif_profile_set(unit, port, value, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the outlif profile")));
             } else {
                BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
            }
        
            break;
        case bcmPortControlPWETerminationPortModeEnable:
            if (soc_property_get(unit, spn_PWE_TERMINATION_PORT_MODE_ENABLE, 0)) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                vtt_in_pp_port_conf_tbl.vlan_translation_profile= value ? SOC_PPC_PORT_DEFINED_VT_PROFILE_VLAN_DOMAIN_MPLS : 0;
                if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "terminate_up_to_two_label_with_port_context", 1)) {
                    vtt_in_pp_port_conf_tbl.termination_profile= value ? SOC_PPC_PORT_DEFINED_TT_PROFILE_VLAN_DOMAIN_MPLS : 0;
                }
                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            break;
        case bcmPortControlInnerPolicerRemark:
            if (SOC_IS_JERICHO_PLUS(unit)) {
                int simple_mode = soc_property_get(unit, spn_QOS_POLICER_COLOR_MAPPING_PCP, 0);
                int dp_profile = BCM_QOS_MAP_PROFILE_GET(value);

                BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_control_set.lif_info");
                if (lif_info == NULL) {        
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }
                if (simple_mode) {
                    /* In simple mode, set in-lif profile to dp profile. */
                    rv = _bcm_petra_port_inlif_profile_set(unit, port, dp_profile, SOC_OCC_MGMT_INLIF_APP_POLICER_COLOR_MAPPING_PCP, lif_info);
                               BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));
                } else {
                    uint32 inlif_profile;
                    /* In advanced mode , map dp profile to in-lif profile. */
                    rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, -1/*Not used*/, 1/*all_kinds*/, lif_info);
                    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                    BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);
                   /* We can only use the 2 lsbs for DP mapping PCP  (2 lsbs are passed to egress only). */
                    inlif_profile &=  BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;
                    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_ing_vlan_edit_inner_global_info_set, (unit, inlif_profile, dp_profile)));
                }
            } else {
                BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
            }
            break;
        case bcmPortControlOuterPolicerRemark:
                if (SOC_IS_JERICHO_PLUS(unit)) {
                int simple_mode = soc_property_get(unit, spn_QOS_POLICER_COLOR_MAPPING_PCP, 0);
                int dp_profile = BCM_QOS_MAP_PROFILE_GET(value);
                BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_control_set.lif_info");
                if (lif_info == NULL) {        
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }
                if (simple_mode) {
                    /* In simple mode */
                    rv = _bcm_petra_port_inlif_profile_set(unit, port, dp_profile, SOC_OCC_MGMT_INLIF_APP_POLICER_COLOR_MAPPING_PCP, lif_info);
                    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));
                } else {
                    /* In advanced mode  */
                    uint32 inlif_profile;
                    rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, -1/*Not used*/, 1/*all_kinds*/, lif_info);
                    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                    BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);
                    /* We can only use the 2 lsbs for DP mapping PCP  (2 lsbs are passed to egress only). */
                    inlif_profile &=  BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;
                    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_ing_vlan_edit_outer_global_info_set, (unit, inlif_profile, dp_profile)));
                }
            } else {
                BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
            }
            break;
        case bcmPortControlAllowTtlInheritance:
            if (SOC_IS_QUX(unit)) {
                uint32 outlif_profile;
                uint64 fld_val64;
                uint32 mask;
                int mode = soc_property_get(unit, spn_LOGICAL_INTERFACE_OUT_TTL_INHERITANCE, 0);

                if(mode) {
                    if ((value < 0) || (value > 1)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                    }
 
                    soc_sand_rv = _bcm_petra_port_outlif_profile_set(unit,port, value, SOC_OCC_MGMT_OUTLIF_APP_TTL_INHERITANCE);
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                    rv = arad_pp_occ_mgmt_get_app_mask(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_TTL_INHERITANCE, &mask);
                    BCMDNX_IF_ERR_EXIT(rv);

                    COMPILER_64_SET(fld_val64, 0xFFFFFFFF, 0xFFFFFFFE); /*outlif profile 0 must disable inheritance*/
                    for (outlif_profile = 0; outlif_profile < 64; outlif_profile++) {
                        if((outlif_profile & mask) && (value==0)) {
                            COMPILER_64_BITCLR(fld_val64, outlif_profile);
                        }
                    }

                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_eg_ttl_inheritance_set,(unit, fld_val64));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            } else {
                BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
            }
            break;
        case bcmPortControlAllowQosInheritance:
            if (SOC_IS_QUX(unit)) {
                uint32 outlif_profile;
                uint64 fld_val64;
                uint32 mask;
                int mode = soc_property_get(unit, spn_LOGICAL_INTERFACE_OUT_QOS_INHERITANCE, 0);
                if(mode) {
                    if ((value < 0) || (value > 1)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                    }

                    soc_sand_rv = _bcm_petra_port_outlif_profile_set(unit,port, value, SOC_OCC_MGMT_OUTLIF_APP_TOS_INHERITANCE);
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                    rv = arad_pp_occ_mgmt_get_app_mask(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_TOS_INHERITANCE, &mask);
                    BCMDNX_IF_ERR_EXIT(rv);

                    COMPILER_64_SET(fld_val64, 0xFFFFFFFF, 0xFFFFFFFE); /*outlif profile 0 must disable inheritance*/
                    for (outlif_profile = 0; outlif_profile < 64; outlif_profile++) {
                        if((outlif_profile & mask) && (value==0)) {
                            COMPILER_64_BITCLR(fld_val64, outlif_profile);
                        }
                    }

                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_eg_qos_inheritance_set,(unit, fld_val64));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            } else {
                BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
            }
            break;
            case bcmPortControlTunnelIdIngressSource:
             /*
              * In some application, customer need to configure the sys_in_lif using pre-defined value,
              * rather than global-lif.
             */
            if (SOC_IS_JERICHO(unit)){
                _bcm_dpp_gport_parse_info_t gport_parse_info;
                _bcm_dpp_gport_hw_resources gport_hw_resources;
                SOC_PPC_LIF_ENTRY_INFO lif_entry_info ;

                rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
                BCMDNX_IF_ERR_EXIT(rv);

                if (gport_parse_info.type != _bcmDppGportParseTypeSimple){

                     rv = _bcm_dpp_gport_to_hw_resources(unit, port,
                                _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                                _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                &gport_hw_resources);
                     BCMDNX_IF_ERR_EXIT(rv);

                     soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, &lif_entry_info);
                     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                     if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE) {
                         lif_entry_info.value.pwe.use_lif = 1;
                         lif_entry_info.value.pwe.global_lif_id = value;
                     } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC){
                         lif_entry_info.value.ac.use_lif = 1;
                         lif_entry_info.value.ac.global_lif = value;
                     } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
                         lif_entry_info.value.mpls_term_info.global_lif = value;
                     } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
                         lif_entry_info.value.ip_term_info.global_lif = value;
                     } else{
                         /* LIF is not PWE, AC, MPLS tunnel or IP tunnel */
                         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (
                             _BSL_BCM_MSG("Unexpected lif id. Must be of type PWE, AC, MPLS tunnel or IP tunnel")));
                     }

                     soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, &lif_entry_info);
                     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }else{
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control under current scenario")));
            }
            break;
        case bcmPortControlEgressAclTwoPassEditing:
            /* update  */    
            if (soc_property_get(unit, spn_EGRESS_ACL_TWO_PASS_ENABLE, 0)){
                uint32 tm_port;
                uint32 base_q_pair;
                uint32 prge_prog_select;
                uint32 nof_priorities;
                uint32 curr_q_pair;
                int core=SOC_CORE_INVALID;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
                soc_sand_rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_priorities));

                /* If disabled set TM profile to 0 (assuming ETH port) */
                prge_prog_select = 0;
                if (value) {
                    /* If enabled egress ACL, update TM profile */
                    BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_EGRESS_ACL_TWO_PASS_EDITING, &prge_prog_select));
                }

                for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_priorities; curr_q_pair++) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_prog_editor_profile_set, (unit,  core, curr_q_pair, prge_prog_select)));
                }
            }                
            else {
                /* Unavailable when egress_acl_two_pass_enable is not set */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            break;
        case bcmPortControlIPv6TerminationCascadedModeEnable:
            if (SOC_IS_JERICHO(unit)
                && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                vtt_in_pp_port_conf_tbl.vlan_translation_profile= value ? SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION_CASCADE : 0;
                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            break;
        case bcmPortConrolEgressVlanPortArpPointed:
            if (SOC_IS_JERICHO(unit)) {
                int local_outlif_id, local_outac_id;
                int lif_base, lif_offset;

                /*Get the link layer local-out-lif-id*/
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_global_and_local_lif(unit, port, NULL, NULL,
                                                                          &local_outlif_id, NULL,NULL));
                /*Get the ac local-out-lif-id*/
                if (BCM_GPORT_IS_VLAN_PORT(value) && BCM_GPORT_SUB_TYPE_IS_LIF(value)) {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_global_and_local_lif(unit, value, NULL, NULL,
                                                                          &local_outac_id, NULL,NULL));

                    /*Check if the ac local-out-lif-id matches with the sharing base*/
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.outlif_info.arp_next_lif.lif_base.get(unit, &lif_base));
                    if ((lif_base != -1) &&
                        ((lif_base ^ local_outac_id) >> _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_NOF_BITS)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MSBs of the ac index (0x%08X) doesn't match with the base ((0x%08X))"),local_outac_id ,lif_base));
                    }

                    lif_offset = local_outac_id & _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK;
                    lif_base = local_outac_id >> _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_NOF_BITS;

                } else if (value == BCM_GPORT_INVALID) {
                    lif_offset = 0xFFFFFFFF;
                    lif_base = 0;
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Value is not a valid vlan port id")));
                }

                /*Set the offset of local_outac_id[5:0] to next-out-lif-lsb of the arp entry*/
                BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_encap_lif_field_set(unit, local_outlif_id,
                                            SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF, lif_offset));

                SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EPNI_EEDB_OUTLIF_BASEr,
                                            REG_PORT_ANY, EEDB_OUTLIF_BASEf, lif_base));
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            break;
        case bcmPortControlVrrpScalingPort:
            if (SOC_DPP_CONFIG(unit)->pp.vrrp_scaling_tcam == 1) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                vtt_in_pp_port_conf_tbl.vlan_translation_profile = value ? (SOC_IS_JERICHO(unit) ? 11 : 5) : SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("VRRP scaling mode disabled")));
            }
            break;
        case bcmPortControlIngressEnable:
            if (!SOC_IS_JERICHO(unit))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            if ((IS_TDM_PORT(unit, port) == TRUE))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                    (_BSL_BCM_MSG("This API does not support TDM port. bcm_port_ingress_tdm_failover_set should be used for TDM ports\n")));

            }
            BCMDNX_IF_ERR_EXIT(soc_jer_port_ingress_enable_set(unit, port, value));

            break;
        case bcmPortControlReflectorL2:
            {
                uint32 tm_port;
                uint32 base_q_pair;
                uint32 prge_prog_select;
                uint32 nof_priorities;
                uint32 curr_q_pair;
                int core=SOC_CORE_INVALID;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
                soc_sand_rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_priorities));
                
                /* If disabled set TM profile to 0 (assuming ETH port) */
                prge_prog_select = 0;
                if (CTR_REFLECTOR_L2_DECODE_ENABLE(value)) {
                    BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REFLECTOR_ETH, &prge_prog_select));
                } else {
                    prge_prog_select = CTR_REFLECTOR_L2_DECODE_PROFILE(value);
                }
                
                for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_priorities; curr_q_pair++) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_prog_editor_profile_set, (unit,  core, curr_q_pair, prge_prog_select)));
                }
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            break;
    }

exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_control_set(int unit, bcm_port_t port,
                           bcm_port_control_t type, int value)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_dpp_gport_info_t   gport_info;
    /* In some cases port is actually inlif profile (e.g. bcmPortControlEgressModifyDscp). */
    /* In those cases we shouldn't do any checking on the port. */
    uint8 port_skip_port_validation = FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    switch(type) {
        case bcmPortControlEgressModifyDscp:
        case bcmPortControlOamDefaultProfile:
        case bcmPortControlOamDefaultProfileEgress:
        case bcmPortControlMplsEncapsulateExtendedLabel:
        case bcmPortControlMPLSEncapsulateAdditionalLabel:
        case bcmPortControlPreserveDscpIngress:
        case bcmPortControlPreserveDscpEgress:
        case bcmPortControlInnerPolicerRemark:
        case bcmPortControlOuterPolicerRemark:
        case bcmPortControlAllowQosInheritance:
        case bcmPortControlAllowTtlInheritance:
        case bcmPortControlTunnelIdIngressSource:
        case bcmPortConrolEgressVlanPortArpPointed:
        case bcmPortControlFloodUnknownUcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodBroadcastGroup:
            port_skip_port_validation = TRUE;
            break;
        default:
            break;
    }

    if (port == -1) { 
        /* port is invalid - used for gloabl configurations */
        rv = _bcm_petra_global_control_set(unit, type, value);  
        BCMDNX_IF_ERR_EXIT(rv);    
    } else if (port_skip_port_validation) {
        rv = _bcm_petra_port_control_set(unit, port, type, value);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        if(gport_info.lane != -1) {
            if(SOC_INFO(unit).port_num_lanes[gport_info.local_port]  <= gport_info.lane) {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid lane")));
            }
        }

        if(IS_SFI_PORT(unit, gport_info.local_port)) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_fabric_control_set(unit, gport_info.local_port, type, value));
        } else {
            /* TM / PP - Pass original gpoty */
            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_control_set(unit, port, type, value));     
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_nif_control_get(int unit, bcm_port_t port,
                                bcm_port_control_t type, int *value)
{
    bcm_error_t rv = BCM_E_NONE;
    soc_error_t soc_rv = SOC_E_NONE;
    uint32 flags;
    _bcm_dpp_gport_info_t gport_info;
    int is_interface = 0;
    SOC_TMC_FC_INBND_MODE fc_inbnd_mode;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    switch(type) {
        case bcmPortControlLinkDownPowerOn:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_enable_get,(unit, gport_info.local_port, 1, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsPolynomial:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_get */
            rv = bcm_petra_port_phy_control_get(unit, port, SOC_PHY_CONTROL_PRBS_POLYNOMIAL, (uint32*)value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsTxEnable:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_get */
            rv = bcm_petra_port_phy_control_get(unit, port, SOC_PHY_CONTROL_PRBS_TX_ENABLE, (uint32*)value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;


        case bcmPortControlPrbsRxEnable:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_get */
            rv = bcm_petra_port_phy_control_get(unit, port, SOC_PHY_CONTROL_PRBS_RX_ENABLE, (uint32*)value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsRxStatus:
            /* call with original port - gport will be handled by bcm_petra_port_phy_control_get */
            rv = bcm_petra_port_phy_control_get(unit, port, SOC_PHY_CONTROL_PRBS_RX_STATUS, (uint32*)value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        /* Flow Control */
        case bcmPortControlPFCRefreshTime:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_pfc_refresh_get,(unit, gport_info.local_port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPFCTransmit:
        case bcmPortControlLLFCTransmit:
        case bcmPortControlSAFCTransmit:
        
            soc_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fc_inbnd_mode_get,(unit, port, TRUE, &fc_inbnd_mode));
            if (soc_rv != SOC_E_NONE) {
                rv = soc_rv;
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if(fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_DISABLED)
            {
                *value = 0;
            }
            else
            {
                if(type == bcmPortControlPFCTransmit)
                {
                    *value = (fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_PFC) ? 1 : 0;
                }
                if(type == bcmPortControlSAFCTransmit)
                {
                    *value = (fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_SAFC) ? 1 : 0;
                }
                if(type == bcmPortControlLLFCTransmit)
                {
                    *value = (fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_LL) ? 1 : 0;
                }
            }
            break;

        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        case bcmPortControlSAFCReceive:

            soc_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fc_inbnd_mode_get,(unit, port, FALSE, &fc_inbnd_mode));
            if (soc_rv != SOC_E_NONE) {
                rv = soc_rv;
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if(fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_DISABLED)
            {
                *value = 0;
            }
            else
            {
                if(type == bcmPortControlPFCReceive)
                {
                    *value = (fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_PFC) ? 1 : 0;
                }
                if(type == bcmPortControlSAFCReceive)
                {
                    *value = (fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_SAFC) ? 1 : 0;
                }
                if(type == bcmPortControlLLFCReceive)
                {
                    *value = (fc_inbnd_mode == SOC_TMC_FC_INBND_MODE_LL) ? 1 : 0;
                }         
            }
            break;

        case bcmPortControlLinkFaultLocal:
            /* call with original port - gport will be handled by bcm_petra_port_fault_get */
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_fault_get(unit, port, &flags));
            (*value) = (flags & BCM_PORT_FAULT_LOCAL ? 1 : 0);
            break;

        case bcmPortControlLinkFaultRemote:
            /* call with original port - gport will be handled by bcm_petra_port_fault_get */
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_fault_get(unit, port, &flags));
            (*value) = (flags & BCM_PORT_FAULT_REMOTE ? 1 : 0);
            break;

        case bcmPortControlPadToSize:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_pad_size_get,(unit, gport_info.local_port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlTxEnable:
            is_interface = 0; 
            if (BCM_GPORT_IS_SET(port)) {
                if (BCM_GPORT_IS_LOCAL(port)) {
                    port = BCM_GPORT_LOCAL_GET(port);
                    if (BCM_DPP_PORT_IS_INTERFACE(port)) {
                        port = port - BCM_DPP_PORT_INTERFACE_START;
                        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_tx_enable_get,(unit, port, value));
                        BCMDNX_IF_ERR_EXIT(rv);
                        is_interface = 1;
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid gport for bcmPortControlTxEnable")));
                } 
            }
            if (!is_interface) {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_control_tx_nif_enable_get, (unit, gport_info.local_port, value)); 
                    BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

       case bcmPortControlStatOversize:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_stat_oversize_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The api is not available for the required type")));
        }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_eee_control_get(int unit, bcm_port_t port,
                                bcm_port_control_t type, int *value)
{
    bcm_error_t rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    switch(type) {
        case bcmPortControlEEEEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_enable_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEETransmitIdleTime:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_tx_idle_time_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEETransmitEventCount:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_tx_event_count_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEETransmitDuration:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_tx_duration_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEEReceiveEventCount:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_rx_event_count_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEEReceiveDuration:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_rx_duration_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEETransmitWakeTime:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_tx_wake_time_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEEEventCountSymmetric:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_event_count_symmetric_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEELinkActiveDuration:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_eee_link_active_duration_get,(unit, gport_info.local_port, (uint32 *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;


        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The api is not available for the required type")));
        }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_port_control_lanes_get(int unit, bcm_port_t port, int* value);

STATIC int
_bcm_petra_port_control_get(int unit, bcm_port_t port,
                            bcm_port_control_t type, int *value) 
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    uint32 soc_sand_rv;
    int is_interface = 0, is_local_port = 0;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    switch (type)
    {
        case bcmPortControlMpls:
        case bcmPortControlIP4:
        case bcmPortControlIP6:
        case bcmPortControlIP4Mcast:
        case bcmPortControlIP6Mcast:
        case bcmPortControlDoNotCheckVlan:
        case bcmPortControlMacInMac:
        case bcmPortControlL2Move:
        case bcmPortControlPrivateVlanIsolate:
        case bcmPortControlBridge:
        case bcmPortControlFloodBroadcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodUnknownUcastGroup:
        case bcmPortControlUnknownMacDaAction:
        case bcmPortControlDiscardMacSaAction:
        case bcmPortControlMplsFRREnable:
        case bcmPortControlMplsContextSpecificLabelEnable:
        case bcmPortControlPreservePacketPriority:
        case bcmPortControlTrillDesignatedVlan:
        case bcmPortControlL2SaAuthenticaion:
        case bcmPortControlLocalSwitching:
        case bcmPortControlTrill:
        case bcmPortControlEgressFilterDisable:
        case bcmPortControlOverlayRecycle:
        case bcmPortControlEvbType:
        case bcmPortControlVMac:           
        case bcmPortControlErspanEnable:
        case bcmPortControlFcoeNetworkPort:
        case bcmPortControlFcoeFabricId:
        case bcmPortControlMplsExplicitNullEnable:
        case bcmPortControlOamDefaultProfile:
        case bcmPortControlOamDefaultProfileEgress:
        case bcmPortControlMplsEncapsulateExtendedLabel:
        case bcmPortControlExtenderType:
        case bcmPortControlNonEtagDrop:
        case bcmPortControl1588P2PDelay:
        case bcmPortControlMplsSpeculativeParse:
        case bcmPortControlUntaggedVlanMember:
        case bcmPortControlMPLSEncapsulateAdditionalLabel:
        case bcmPortControlForceBridgeForwarding:
        case bcmPortControlLogicalInterfaceSameFilter:
            rv = _bcm_petra_pp_port_control_get(unit, port, type, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        case bcmPortControlPrbsPolynomial:
        case bcmPortControlPrbsTxEnable:
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlLinkDownPowerOn:
        case bcmPortControlTxEnable:
        case bcmPortControlPrbsTxInvertData:
        case bcmPortControlPrbsRxStatus:
        case bcmPortControlPFCTransmit:
        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        case bcmPortControlLLFCTransmit:
        case bcmPortControlSAFCTransmit:
        case bcmPortControlSAFCReceive:
        case bcmPortControlPFCRefreshTime:
        case bcmPortControlLinkFaultLocal:
        case bcmPortControlLinkFaultRemote:
        case bcmPortControlPadToSize:
        case bcmPortControlStatOversize:    
            /* NIF configuration. must get physical port */
            rv = _bcm_petra_port_nif_control_get(unit, port, type, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlEEEEnable:
        case bcmPortControlEEETransmitIdleTime:
        case bcmPortControlEEETransmitEventCount:
        case bcmPortControlEEETransmitDuration:
        case bcmPortControlEEEReceiveEventCount:
        case bcmPortControlEEEReceiveDuration:
        case bcmPortControlEEETransmitWakeTime:
        case bcmPortControlEEEEventCountSymmetric:
        case bcmPortControlEEELinkActiveDuration:
            rv = _bcm_petra_port_eee_control_get(unit, port, type, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
                 
        case bcmPortControlLanes:
           /* get nof_lanes */
            rv = _bcm_petra_port_control_lanes_get(unit, port, value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        case bcmPortControlRxEnable :
            if (BCM_GPORT_IS_SET(port)) {
                if (BCM_GPORT_IS_LOCAL(port)) {
                    port = BCM_GPORT_LOCAL_GET(port);
                    if (BCM_DPP_PORT_IS_INTERFACE(port)) {
                        port = port - BCM_DPP_PORT_INTERFACE_START;
                        is_interface = 1;
                    } else {
                        is_local_port = 1;
                    }
                }
            } else {
                is_local_port = 1; 
            }
            if ((is_interface || is_local_port) && (port < FABRIC_LOGICAL_PORT_BASE(unit)) ) {
                rv =  BCM_E_NONE;
                soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_rx_enable_get,(unit,  port, value)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {  
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid gport for bcmPortControlRxEnable")));  
            }
            break; 
        case bcmPortControlIlknRxPathEnable:
            {
                _bcm_dpp_gport_info_t gport_info;

                soc_port_if_t interface;
                uint32 sw_db_flags = 0;
                uint32 value_get = 0;

                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &interface));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, gport_info.local_port, &sw_db_flags));

                if (interface == SOC_PORT_IF_ILKN && (!SOC_PORT_IS_ELK_INTERFACE(sw_db_flags))){
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_bypass_interface_rx_enable_get, (unit, gport_info.local_port, &value_get)));

                    *value = value_get;
                }
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port (should be IlKN port that is not ELK)")));
                }
            }
            break;

#ifdef BCM_88660
        case bcmPortControlEgressModifyDscp:
            if(!(BCM_GPORT_IS_LOCAL(port))) {
                if (SOC_IS_ARADPLUS(unit)) {
                    int simple_mode = soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0);

                    rv = BCM_E_NONE;

                    if (simple_mode) {
                        uint32 inlif_profile = 0;
                    
                        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_control_get.lif_info");
                        if (lif_info == NULL) {        
                            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                        }

                        rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING, 0/*all_kinds*/, lif_info);
                        *value = (int)inlif_profile;

                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                    } else {
                        uint32 mask;
                        SOC_PPC_EG_QOS_GLOBAL_INFO info;
                        rv = BCM_E_NONE;

                        if ((port < 0) || (port > BCM_PORT_INLIF_PROFILE_MAX)) {
                           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid inlif profile")));
                        }

                        SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);
                        soc_sand_rv = soc_ppd_eg_qos_global_info_get(unit, &info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        mask = (1 << port);
                        *value = ((info.in_lif_profile_bitmap & mask) != 0) ? 1 : 0;
                    }

                } else {
                    rv = BCM_E_UNAVAIL;
                }
            }else {
                if (SOC_IS_QUX(unit)) {
                    uint32 pp_port = 0;
                    int core = 0;

                    port = BCM_GPORT_LOCAL_GET(port);
                    if(port < 0  || port > 255) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port (should less than 256)")));
                    }
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
                    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_eg_qos_marking_get,(unit, pp_port, value)));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else {
                     BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
                }
            }
            BCMDNX_IF_ERR_EXIT(rv);
        break;
#endif
    case bcmPortControlExtenderEnable:
        {
            _bcm_dpp_gport_info_t   gport_info;
            bcm_port_t port_ndx = 0;
            SOC_PPC_PORT soc_ppd_port = 0;
            int core;
        
            rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
            BCMDNX_IF_ERR_EXIT(rv);
        
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                break;
            }
        
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_extender_mapping_enable_get,(unit, port_ndx, value)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
        }
        break;
    case bcmPortControlPONEnable:
        {
            _bcm_dpp_gport_info_t   gport_info;
            bcm_port_t port_ndx = 0;
            SOC_PPC_PORT soc_ppd_port = 0;
            int core;
        
            rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
            BCMDNX_IF_ERR_EXIT(rv);
        
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                break;
            }
        
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_pon_ports_enable_get,(unit, port_ndx, value)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
        }
        break;
    case bcmPortControlReserved280:
        if (SOC_DPP_CONFIG(unit)->pp.egress_full_mymac_1) {
            _bcm_dpp_gport_info_t   gport_info;
            bcm_port_t port_ndx = 0;
            SOC_PPC_PORT soc_ppd_port = 0;
            int core;

            rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));
                break;
            }

            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_tm_port_var_get, (unit, port_ndx, value)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
        }
        break;
    case bcmPortControlReserved281:
        if (SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id || SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id || SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id) {
            ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
            SOC_PPC_PORT soc_ppd_port = 0;
            int core;

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port,&soc_ppd_port,&core)));

            soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *value = (vtt_in_pp_port_conf_tbl.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION) ? 1 : 0;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
        }
        break;
        case bcmPortControlPWETerminationPortModeEnable:
            if (soc_property_get(unit, spn_PWE_TERMINATION_PORT_MODE_ENABLE, 0)) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                *value = (vtt_in_pp_port_conf_tbl.vlan_translation_profile==  SOC_PPC_PORT_DEFINED_VT_PROFILE_VLAN_DOMAIN_MPLS)?1:0;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
        break;
    case bcmPortControlInnerPolicerRemark:
        if (SOC_IS_JERICHO_PLUS(unit)) {
            int simple_mode = soc_property_get(unit, spn_QOS_POLICER_COLOR_MAPPING_PCP, 0);
            uint32 inlif_profile = 0;
            
            BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_control_get.lif_info");
            if (lif_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }
            rv = BCM_E_NONE;
            

            if (simple_mode) {
                rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, SOC_OCC_MGMT_INLIF_APP_POLICER_COLOR_MAPPING_PCP, 0/*all_kinds*/, lif_info);
                *value = (int)inlif_profile;

                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

            } else {
                rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, -1/*Not used*/, 1/*all_kinds*/, lif_info);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);
                /* We can only use the 2 lsbs for DP mapping PCP  (2 lsbs are passed to egress only). */
                inlif_profile &= BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;
                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_ing_vlan_edit_inner_global_info_get, (unit, inlif_profile, value)));
            }

        } else {
            rv = BCM_E_UNAVAIL;
        }
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmPortControlOuterPolicerRemark:
        if (SOC_IS_JERICHO_PLUS(unit)) {
            int simple_mode = soc_property_get(unit, spn_QOS_POLICER_COLOR_MAPPING_PCP, 0);
            uint32 inlif_profile = 0;
            rv = BCM_E_NONE;
            
            BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_control_get.lif_info");
            if (lif_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }

            if (simple_mode) {
                uint32 inlif_profile = 0;
                
                rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, SOC_OCC_MGMT_INLIF_APP_POLICER_COLOR_MAPPING_PCP, 0/*all_kinds*/, lif_info);
                *value = (int)inlif_profile;

                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

            } else {
                rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, -1/*Not used*/, 1/*all_kinds*/, lif_info);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);
                /* We can only use the 2 lsbs for DP mapping PCP  (2 lsbs are passed to egress only). */
                inlif_profile &= BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;
                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_ing_vlan_edit_outer_global_info_get, (unit, inlif_profile, value)));
            }

        } else {
            rv = BCM_E_UNAVAIL;
        }
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmPortControlAllowTtlInheritance:
        if (SOC_IS_QUX(unit)) {
            uint32 outlif_profile;
            int local_outlif_id;
            uint64 reg_val64;
            rv = BCM_E_NONE;

             /*Convert gport to local LIF*/
             BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_global_and_local_lif(unit, port, NULL, NULL,
                                               &local_outlif_id, NULL,NULL));
            /*Read the Out-LIF-profile from HW*/
            BCM_SAND_IF_ERR_EXIT(
            soc_ppd_eg_encap_lif_field_get(unit, local_outlif_id,
                            SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE,
                            &outlif_profile));

            COMPILER_64_SET(reg_val64, 0, 0);
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_eg_ttl_inheritance_get,(unit, &reg_val64));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

            *value = COMPILER_64_BITTEST(reg_val64, outlif_profile) ? 1 : 0;
        } else {
            rv = BCM_E_UNAVAIL;
        }
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmPortControlAllowQosInheritance:
        if (SOC_IS_QUX(unit)) {
            uint32 outlif_profile;
            int local_outlif_id;
            uint64 reg_val64;
            rv = BCM_E_NONE;

            /*Convert gport to local LIF*/
             BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_global_and_local_lif(unit, port, NULL, NULL,
                                               &local_outlif_id, NULL,NULL));
            /*Read the Out-LIF-profile from HW*/
            BCM_SAND_IF_ERR_EXIT(
            soc_ppd_eg_encap_lif_field_get(unit, local_outlif_id,
                            SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE,
                            &outlif_profile));

            COMPILER_64_SET(reg_val64, 0, 0);
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_eg_qos_inheritance_get,(unit, &reg_val64));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

            *value = COMPILER_64_BITTEST(reg_val64, outlif_profile) ? 1 : 0;

        } else {
            rv = BCM_E_UNAVAIL;
        }
        BCMDNX_IF_ERR_EXIT(rv);
        /* Coverity: better to keep break statement anyway */
        /* coverity[dead_error_line] */
        break;
        case bcmPortControlTunnelIdIngressSource:
         /*
          * In some application, customer need to configure the sys_in_lif using pre-defined value,
          * rather than global-lif.
         */
        if (SOC_IS_JERICHO(unit)){
            _bcm_dpp_gport_parse_info_t gport_parse_info;
            _bcm_dpp_gport_hw_resources gport_hw_resources;
            SOC_PPC_LIF_ENTRY_INFO lif_entry_info ;
        
            rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
            BCMDNX_IF_ERR_EXIT(rv);
        
            if (gport_parse_info.type != _bcmDppGportParseTypeSimple){
        
                 rv = _bcm_dpp_gport_to_hw_resources(unit, port,
                            _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                            _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                            &gport_hw_resources);
                 BCMDNX_IF_ERR_EXIT(rv);
        
                 soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, &lif_entry_info);
                 BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
                 if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE) {
                     *value = lif_entry_info.value.pwe.use_lif?lif_entry_info.value.pwe.global_lif_id:0;
                 } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC){
                     *value = lif_entry_info.value.ac.use_lif?lif_entry_info.value.ac.global_lif:0;
                 } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
                     *value = lif_entry_info.value.mpls_term_info.global_lif;
                 } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
                     *value = lif_entry_info.value.ip_term_info.global_lif;
                 } else{
                     /* LIF is not PWE, AC, MPLS tunnel or IP tunnel */
                     BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (
                         _BSL_BCM_MSG("Unexpected lif id. Must be of type PWE, AC, MPLS tunnel or IP tunnel")));
                 }
            }
        }else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control under current scenario")));
        }
        break;
        case bcmPortControlEgressAclTwoPassEditing:
            *value = 0;
            /* update  */    
            if (soc_property_get(unit, spn_EGRESS_ACL_TWO_PASS_ENABLE, 0)){
                uint32 tm_port;
                uint32 base_q_pair;
                uint32 prge_prog_select;
                int core=SOC_CORE_INVALID;
                ARAD_EGQ_PCT_TBL_DATA pct_tbl_data;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
                soc_sand_rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_EGRESS_ACL_TWO_PASS_EDITING, &prge_prog_select));
                SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(unit, core, base_q_pair, &pct_tbl_data));
                *value = (pct_tbl_data.prog_editor_profile == prge_prog_select);
            }                
            break;
        case bcmPortControlIPv6TerminationCascadedModeEnable:
            if (SOC_IS_JERICHO(unit)
                && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) {
                ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_in_pp_port_conf_tbl;
                SOC_PPC_PORT soc_ppd_port = 0;
                int core;

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

                soc_sand_rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core, soc_ppd_port, &vtt_in_pp_port_conf_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                *value = (vtt_in_pp_port_conf_tbl.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION_CASCADE)?1:0;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            break;
        case bcmPortConrolEgressVlanPortArpPointed:
            if (SOC_IS_JERICHO(unit)) {
                int local_outlif_id, local_outac_id;
                int lif_base, global_lif_id;
                uint32 rval;

                /*Check if we have set arp pointed AC already. */
                BCMDNX_IF_ERR_EXIT(READ_EPNI_EEDB_OUTLIF_BASEr(unit,REG_PORT_ANY, &rval));
                lif_base = soc_reg_field_get(unit, EPNI_EEDB_OUTLIF_BASEr, rval, EEDB_OUTLIF_BASEf);

                /*Get the link layer local-out-lif-id*/
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_global_and_local_lif(unit, port, NULL, NULL,
                                                                          &local_outlif_id, NULL,NULL));

                /*Get the offset of local_outac_id[5:0] from next-out-lif-lsb of the arp entry*/
                BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_encap_lif_field_get(unit, local_outlif_id,
                                            SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF, (uint32 *)&local_outac_id));

                /*Construct the ac local-out-lif by setting its MSBs, translate it to global.*/
                if (local_outac_id == 0xFFFFFFFF) {
                    *value = BCM_GPORT_INVALID;
                } else {
                    local_outac_id |= (lif_base << _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_NOF_BITS);
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_global_lif_mapping_local_to_global_get(unit,
                                        _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, local_outac_id, &global_lif_id));

                    /*Encoding the global lif to vlan port*/
                    BCM_GPORT_VLAN_PORT_ID_SET(*value, global_lif_id);
                    BCM_GPORT_SUB_TYPE_LIF_SET(*value, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, *value);
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            break;
        case bcmPortControlIngressEnable:
            if (!SOC_IS_JERICHO(unit))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            }
            if ((IS_TDM_PORT(unit, port) == TRUE))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                    (_BSL_BCM_MSG("This API does not support TDM port. bcm_port_ingress_tdm_failover_get should be used for TDM ports\n")));

            }
            BCMDNX_IF_ERR_EXIT(soc_jer_port_ingress_enable_get(unit, port, value));

            break;
        case bcmPortControlReflectorL2:
            {
                uint32 tm_port;
                uint32 base_q_pair;
                uint32 prge_prog_select;
                int core=SOC_CORE_INVALID;
                ARAD_EGQ_PCT_TBL_DATA pct_tbl_data;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
                soc_sand_rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                prge_prog_select = 0;
                BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REFLECTOR_ETH, &prge_prog_select));
                SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(unit, core, base_q_pair, &pct_tbl_data));
                *value = CTR_REFLECTOR_L2_ENCODE(((prge_prog_select == pct_tbl_data.prog_editor_profile) ? 1 : 0), pct_tbl_data.prog_editor_profile);
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid control")));
            break;
    }

exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_fabric_control_get(int unit, bcm_port_t port,
                                bcm_port_control_t type, int *value)
{
    bcm_error_t rv = BCM_E_NONE;
    int locked = 0, prbs_mode;
    pbmp_t prbs_mac_mode;

    BCMDNX_INIT_FUNC_DEFS;

    rv = PORT_ACCESS.prbs_mac_mode.get(unit, &prbs_mac_mode);
    BCMDNX_IF_ERR_EXIT(rv);
    prbs_mode = SOC_PBMP_MEMBER(prbs_mac_mode, port) ? 1: 0;

    switch (type) {
        case bcmPortControlPCS:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_control_pcs_get, (unit, port, (soc_dcmn_port_pcs_t*)value)); 
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlLinkDownPowerOn:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_power_get,(unit, port, (soc_dcmn_port_power_t*)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlStripCRC:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_strip_crc_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlRxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_rx_enable_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlTxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_control_tx_enable_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsPolynomial:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_polynomial_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsTxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_tx_enable_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsRxEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_rx_enable_get,(unit, port, prbs_mode,  value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsRxStatus:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_rx_status_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        
        case bcmPortControlPrbsTxInvertData:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_prbs_tx_invert_data_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlPrbsMode:
            if(SOC_PBMP_MEMBER(prbs_mac_mode, port)) {
                *value = 1; /*PRBS MAC MODE*/
            } else {
                *value = 0; /*PRBS PHY MODE*/
            }
            break;

        case bcmPortControlLowLatencyLLFCEnable:
            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_control_low_latency_get,(unit, port, (int *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmPortControlFecErrorDetectEnable:
            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_control_fec_error_detect_get,(unit, port, (int *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        case bcmPortControlLlfcCellsCongestionIndEnable:
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_extract_cig_from_llfc_enable_get,(unit,port,(int *)value));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));
            break;
    }

exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_global_control_get(int unit, bcm_port_control_t type, int *value);

int
bcm_petra_port_control_get(int unit, bcm_port_t port,
                            bcm_port_control_t type, int *value)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_dpp_gport_info_t   gport_info;
    /* In some cases port is actually inlif profile (e.g. bcmPortControlEgressModifyDscp). */
    /* In those cases we shouldn't do any checking on the port. */
    uint8 port_skip_port_validation = FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(value);

    switch(type) {
        case bcmPortControlEgressModifyDscp:
        case bcmPortControlOamDefaultProfile:
        case bcmPortControlOamDefaultProfileEgress:
        case bcmPortControlMplsEncapsulateExtendedLabel:
        case bcmPortControlMPLSEncapsulateAdditionalLabel:
        case bcmPortControlInnerPolicerRemark:
        case bcmPortControlOuterPolicerRemark:
        case bcmPortControlTunnelIdIngressSource:
        case bcmPortConrolEgressVlanPortArpPointed:
        case bcmPortControlFloodBroadcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodUnknownUcastGroup:
            port_skip_port_validation = TRUE;
            break;
        default:
            break;
    }

    if (port == -1) { 
        /* port is invalid - used for gloabl configurations */
        rv = _bcm_petra_global_control_get(unit, type, value);      
    } else if (port_skip_port_validation) {
        rv = _bcm_petra_port_control_get(unit, port, type, value);
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        if(gport_info.lane != -1) {
            if(SOC_INFO(unit).port_num_lanes[gport_info.local_port]  <= gport_info.lane) {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid lane")));
            }
        }

        if(IS_SFI_PORT(unit, gport_info.local_port)) {
            rv = _bcm_petra_port_fabric_control_get(unit, gport_info.local_port, type, value);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            /* TM / PP */
            rv = _bcm_petra_port_control_get(unit, port, type, value);
            BCMDNX_IF_ERR_EXIT(rv);
        } 
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_linkscan_get(int unit,
                          bcm_port_t port,
                          int *linkscan)
{
    _bcm_dpp_gport_info_t   gport_info;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(linkscan);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = bcm_common_linkscan_mode_get(unit, gport_info.local_port, linkscan);
    if(rv == BCM_E_INIT) {
        *linkscan = BCM_LINKSCAN_MODE_NONE;
        rv = BCM_E_NONE;
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_linkscan_set(int unit,
                          bcm_port_t port,
                          int linkscan)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* _bcm_dpp_gport_to_phy_port is handled inside bcm_petra_linkscan_mode_set*/
    BCMDNX_IF_ERR_EXIT(bcm_petra_linkscan_mode_set(unit, port, linkscan));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_phy_reset(int unit, bcm_port_t port)
{
    uint32 flags;
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    soc_port_t port_ndx;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        if (!IS_SFI_PORT(unit, port_ndx)) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_ndx, &flags));
            if(!SOC_PORT_IS_NETWORK_INTERFACE(flags)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port %d isn't NIF"), port_ndx));
            }
        }


        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_reset,(unit, port_ndx)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp) 
{
    int rv, is_init_sequence;
    soc_port_t port;
    soc_pbmp_t virtual_pbmp;
    soc_port_if_t interface;
    int sfi_in_pbmp=0;
    BCMDNX_INIT_FUNC_DEFS;

   PBMP_ITER(pbmp, port) {
       if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
           rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_reference_clock_set, (unit, port));
           BCMDNX_IF_ERR_EXIT(rv);
        } else {
            sfi_in_pbmp=1;
        }
    }

    rv = bcm_petra_init_check(unit);
    if (rv == BCM_E_UNIT) {/* init */ 
        is_init_sequence = 1;
    }
    else if (rv == BCM_E_NONE) {
        is_init_sequence = 0;
    }
    else {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_probe, (unit, pbmp, okay_pbmp, is_init_sequence));
    BCMDNX_IF_ERR_EXIT(rv);

    /* set virtual ports to initialized */
    if (is_init_sequence && !SOC_WARM_BOOT(unit)) {

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &virtual_pbmp));

        PBMP_ITER(virtual_pbmp, port) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
            switch (interface) {
                case SOC_PORT_IF_CPU:
                case SOC_PORT_IF_TM_INTERNAL_PKT:
                case SOC_PORT_IF_RCY:
                case SOC_PORT_IF_OLP:
                case SOC_PORT_IF_ERP:
                case SOC_PORT_IF_OAMP:
                case SOC_PORT_IF_SAT:
                case SOC_PORT_IF_IPSEC:
                     /*mark port as initialized*/
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 1));
                    break;
                default:
                    break;
            }
        }
    }

   /*
    *This needs to be after the mbcm_dpp_port_probe because it updates the disabled ports pbmp. 
    */
    if (sfi_in_pbmp && !SOC_WARM_BOOT(unit) && !SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_brdc_fsrd_blk_id_set,(unit)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_port_fabric_detach(int unit,
          pbmp_t pbmp,
          pbmp_t *detached)
{
    bcm_port_t  port;
    bcm_error_t  rv;
    int counter_interval;
    uint32 counter_flags;
    pbmp_t counter_pbmp; 
    int counter_paused = 0, linkscan_paused = 0;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*detached);

    /* stop counter thread */
    BCMDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

    /*_bcm_petra_port_sfi_detach(unit,pbmp,*detached);*/
    /*Pause counter thread*/
    rv = soc_counter_stop(unit);
    BCMDNX_IF_ERR_EXIT(rv);
    counter_paused = 1;

    PBMP_ITER(pbmp, port) {
        /*Pause linkscan thread*/
        BCMDNX_IF_ERR_EXIT(bcm_petra_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_linkscan_pause(unit));
    linkscan_paused = 1;

    BCM_LOCK(unit);
    
    PBMP_ITER(pbmp, port) {

        rv = bcm_petra_port_enable_set(unit, port,0);
        BCMDNX_IF_ERR_EXIT(rv);

       
        rv = MBCM_DPP_DRIVER_CALL(unit ,mbcm_dpp_port_fabric_detach ,(unit, port));
        BCMDNX_IF_ERR_EXIT(rv);


        SOC_PBMP_PORT_ADD(*detached, port);
    }
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_brdc_fsrd_blk_id_set,(unit)));

    BCM_UNLOCK(unit);

    /*Continue linkscan thread*/
    rv = _bcm_linkscan_continue(unit);
    linkscan_paused = 0;
    SOCDNX_IF_ERR_EXIT(rv);

    /*Continuue counter thread*/
    rv = soc_counter_start(unit, counter_flags, counter_interval, PBMP_SFI_ALL(unit));
    counter_paused = 0;
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    if (linkscan_paused)
    {
        _bcm_linkscan_continue(unit);
    }

    if (counter_paused)
    {
        rv = soc_counter_start(unit, counter_flags, counter_interval, PBMP_SFI_ALL(unit));
        if(BCM_FAILURE(rv)) {
            cli_out("soc_counter_start failed\n");
        }
    }
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_detach(int unit,
          pbmp_t pbmp,
          pbmp_t *detached)
{
    bcm_port_t  port;
    bcm_error_t rv;
    uint32  is_valid, is_initialized, ports_count;
    int counter_interval, curr_mode;
    uint32 counter_flags;
    pbmp_t counter_pbmp, ports_pbmp, pbmp_sfi; 
    bcm_port_config_t config;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_port_config_get(unit,&config);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ASSIGN(pbmp_sfi, config.sfi);

    SOC_PBMP_AND(pbmp_sfi,pbmp);

    SOC_PBMP_CLEAR(*detached);

    /* Detach SFI ports */
    if (SOC_PBMP_NOT_NULL(pbmp_sfi)) {
       BCMDNX_IF_ERR_EXIT(_bcm_petra_port_fabric_detach(unit,pbmp_sfi,detached));
       SOC_PBMP_REMOVE(pbmp,pbmp_sfi);
    }

    /* Detach NIF ports */
    PBMP_ITER(pbmp, port) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_pbmp));
        SOC_PBMP_REMOVE(ports_pbmp, pbmp);
        SOC_PBMP_COUNT(ports_pbmp, ports_count);

        if (SOC_IS_ARADPLUS_AND_BELOW(unit) || (ports_count == 0 /* check if all channels are subset of the given ports */)) {
            rv = bcm_petra_port_enable_set(unit, port, 0);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (SOC_PBMP_MEMBER(SOC_INFO(unit).cmic_bitmap, port)) {
            /*mark port as not initialized*/
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
        } else if (SOC_PBMP_MEMBER(SOC_PORT_BITMAP(unit, lbgport), port)) {
            /*mark port as not initialized*/
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
        } else {
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_linkscan_get(unit, port, &curr_mode));
            if (curr_mode != BCM_LINKSCAN_MODE_NONE) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE));
            }
            
            /* stop counter thread */
            BCMDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
            soc_counter_stop(unit);
            
            /*validate port is valid*/
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
            if(!is_valid) {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PORT, (_BSL_BCM_MSG("Can't deinit invalid port %d"), port));
            }

            /*make sure port is initialized*/
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
            if(!is_initialized) {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PORT, (_BSL_BCM_MSG("Port isn't initialized %d"), port));
            }

            if (SOC_IS_JERICHO(unit) && (ports_count == 0)) {
                /*Priority Drop - return to HW defaults before removing the port*/
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_prd_restore_hw_defaults, (unit, port));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
            /*mark port as not initialized*/
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) || (ports_count == 0 /* check if all channels are subset of the given ports */)) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
            }

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_close_path, (unit, port)));

            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_detach, (unit, port));
            BCMDNX_IF_ERR_EXIT(rv);

            /* remove port and start counter thread */
            SOC_PBMP_PORT_REMOVE(counter_pbmp, port);
            BCMDNX_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
        }

        SOC_PBMP_PORT_ADD(*detached, port);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_phy_control_set(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32 value)
{
    bcm_error_t rv = BCM_E_NONE;
    int enable;
    bcm_port_t port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;
        

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if(BCM_PORT_PHY_CONTROL_CL72 == type) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_port_enable_get(unit, port, &enable));
        if(enable) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_enable_set(unit, port, 0));
        }
    }

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_control_set, (unit, 
                                                                            port_ndx, 
                                                                            gport_info.phyn, 
                                                                            gport_info.lane, 
                                                                            _BCM_DPP_GPORT_INFO_IS_SYS_SIDE(gport_info),
                                                                            (soc_phy_control_t)type,
                                                                            value));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(BCM_PORT_PHY_CONTROL_CL72 == type) {
        if(enable) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_enable_set(unit, port, 1));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_phy_control_get(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32 *value)
{
    _bcm_dpp_gport_info_t   gport_info;
    bcm_error_t rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(value);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_control_get, (unit, 
                                                                            gport_info.local_port, 
                                                                            gport_info.phyn, 
                                                                            gport_info.lane, 
                                                                            _BCM_DPP_GPORT_INFO_IS_SYS_SIDE(gport_info),
                                                                            (soc_phy_control_t)type,
                                                                            value));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_phy_get
 * Description:
 *      General PHY register read
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - (OUT) Data that was read
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(phy_data);

    if(flags & BCM_PORT_PHY_NOMAP) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_get, (unit, port, flags, phy_reg_addr, phy_data));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_get, (unit, gport_info.local_port, flags, phy_reg_addr, phy_data));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_phy_set
 * Description:
 *      General PHY register write
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    int rv;
    bcm_port_t port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if(flags & BCM_PORT_PHY_NOMAP) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_set, (unit, port, flags, phy_reg_addr, phy_data));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_set, (unit, port_ndx, flags, phy_reg_addr, phy_data));
            BCMDNX_IF_ERR_EXIT(rv);
        }

    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_phy_modify
 * Description:
 *      General PHY register modify
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 *      phy_mask - Bits to modify using phy_data
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_port_phy_modify(int unit, bcm_port_t port, uint32 flags,
                        uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask)
{
    int rv;
    bcm_port_t port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if(flags & BCM_PORT_PHY_NOMAP) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_modify, (unit, port, flags, phy_reg_addr, phy_data, phy_mask));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
   
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_modify, (unit, port_ndx, flags, phy_reg_addr, phy_data, phy_mask));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_pause_addr_set(int unit, bcm_port_t port, bcm_mac_t mac)
{
    int rv;
    bcm_port_t port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_mac_sa_set, (unit, port_ndx, mac));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_pause_addr_get(int unit, bcm_port_t port, bcm_mac_t mac)
{
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_mac_sa_get, (unit, gport_info.local_port, mac));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_pause_set(int unit, bcm_port_t port, int pause_tx, int pause_rx)
{
    int                         rv = BCM_E_NONE;
    bcm_port_t                  port_ndx;
    _bcm_dpp_gport_info_t       gport_info;
    SOC_TMC_FC_INBND_MODE       rec_mode;
    SOC_TMC_FC_GEN_INBND_INFO   gen_info;
    soc_port_if_t               interface_type;

    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        rv = soc_port_sw_db_interface_type_get(unit, port_ndx, &interface_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if (interface_type == SOC_PORT_IF_ILKN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported for Interlaken configuration")));
        }
    }

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        rec_mode = pause_rx ? SOC_TMC_FC_INBND_MODE_LL : SOC_TMC_FC_INBND_MODE_DISABLED;
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_fc_inbnd_mode_set,(unit, port_ndx, 0, rec_mode)));
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_TMC_FC_GEN_INBND_INFO_clear(&gen_info);
        gen_info.mode = pause_tx ? SOC_TMC_FC_INBND_MODE_LL : SOC_TMC_FC_INBND_MODE_DISABLED;
        gen_info.ll.cnm_enable = TRUE;
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_fc_gen_inbnd_set,(unit, port_ndx, &gen_info)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_pause_get(int unit, bcm_port_t port, int *pause_tx, int *pause_rx)
{
    int                         rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t       gport_info;
    SOC_TMC_FC_INBND_MODE       mode;
    SOC_TMC_FC_GEN_INBND_INFO   gen_info;
    soc_port_if_t               interface_type;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&gen_info, 0, sizeof gen_info);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &interface_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if (interface_type == SOC_PORT_IF_ILKN) {
        *pause_rx = *pause_tx = 0;
    } else {
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_fc_inbnd_mode_get,(unit, gport_info.local_port, 0, &mode)));
        BCMDNX_IF_ERR_EXIT(rv);
        *pause_rx = (mode == SOC_TMC_FC_INBND_MODE_LL ? 1 : 0);
        
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_fc_gen_inbnd_get,(unit, gport_info.local_port, &gen_info)));
        BCMDNX_IF_ERR_EXIT(rv);
        *pause_tx = (gen_info.mode == SOC_TMC_FC_INBND_MODE_LL ? 1 : 0);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_pause_sym_get(int unit, bcm_port_t port, int *pause)
{
    int rv = BCM_E_NONE;
    int pause_tx;
    int pause_rx;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_port_pause_get(unit, port, &pause_tx, &pause_rx);
    BCMDNX_IF_ERR_EXIT(rv);

    switch((pause_tx << 1) | pause_rx) {
    case 0:
        *pause = BCM_PORT_PAUSE_NONE;
        break;

    case 1:
        *pause = BCM_PORT_PAUSE_ASYM_RX;
        break;

    case 2:
        *pause = BCM_PORT_PAUSE_ASYM_TX;
        break;

    case 3:
        *pause = BCM_PORT_PAUSE_SYM;
        break;

    default:
        BCMDNX_IF_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid Pause configuration")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_pause_sym_set(int unit, bcm_port_t port, int pause)
{
    int pause_tx = 0;
    int pause_rx = 0;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (pause == BCM_PORT_PAUSE_ASYM_RX) {
        pause_rx = 1;
    } else if (pause == BCM_PORT_PAUSE_ASYM_TX) {
        pause_tx = 1;
    } else if (pause == BCM_PORT_PAUSE_SYM) {
        pause_tx = 1;
        pause_rx = 1;
    }

    BCMDNX_IF_ERR_EXIT(bcm_petra_port_pause_set(unit, port, pause_tx, pause_rx));

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88650_A0
int
_bcm_petra_port_link_up_mac_update(int unit, bcm_port_t port, int link){
    int rv; 

    BCMDNX_INIT_FUNC_DEFS;

    if(IS_IL_PORT(unit, port)){
        BCM_EXIT;
    }

    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_link_up_mac_update, (unit, port, link));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
#endif


/*
 * Function:
 *      bcm_petra_port_update
 * Purpose:
 *      Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      link - TRUE - process as link up.
 *             FALSE - process as link down.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_petra_port_update(int unit, bcm_port_t port, int link)
{
    int rv = BCM_E_NONE;
    int  soc_sand_dev_id;
    int soc_sand_rv = 0;
#ifdef BCM_88660_A0
    bcm_port_if_t interface;
#endif
    SOC_SAND_PP_SYS_PORT_ID
        pp_sys_port;
    SOC_TMC_DEST_INFO
        tm_dest_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    LOG_DEBUG(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "bcm_petra_port_update: u=%d p=%d link=%d rv=%d\n"),
               unit, port, link, rv));

    soc_sand_dev_id = (unit);

    soc_sand_SAND_PP_SYS_PORT_ID_clear(&pp_sys_port);
    SOC_TMC_DEST_INFO_clear(&tm_dest_info);

    /* 
     *  API is releavant only if port given is system port.
     *  API should be called global over all devices.
     */
    if (!BCM_GPORT_IS_SYSTEM_PORT(port)) {
        if (BCM_GPORT_IS_LOCAL(port) || SOC_PORT_VALID(unit, port)) {
#ifdef BCM_88650_A0
            if((link)){
                rv = _bcm_petra_port_link_up_mac_update(unit, port, link);
                BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_88660_A0
                if(SOC_IS_ARADPLUS(unit) && (!IS_SFI_PORT(unit, port)) && (!SOC_IS_JERICHO(unit))){
                    rv = soc_port_sw_db_interface_type_get(unit, port, &interface);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if(interface == BCM_PORT_IF_CAUI){
                        rv = soc_pm_tx_remote_fault_enable_set(unit, port, 0);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
#endif /*BCM_88660_A0*/
            }
            else{ /*link down*/
#ifdef BCM_88660_A0
                if(SOC_IS_ARADPLUS(unit) && (!IS_SFI_PORT(unit, port)) && (!SOC_IS_JERICHO(unit))){
                    rv = soc_port_sw_db_interface_type_get(unit, port, &interface);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if(interface == BCM_PORT_IF_CAUI){
                        rv = soc_pm_tx_remote_fault_enable_set(unit, port, 1);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
#endif /*BCM_88660_A0*/
            }
#endif /*BCM_88650_A0*/
            BCM_EXIT;
        }

        /* Invliad port */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid port expacted")));
        
    }

    rv = _bcm_dpp_gport_to_tm_dest_info(unit,port,&tm_dest_info);
    BCMDNX_IF_ERR_EXIT(rv);
    if(tm_dest_info.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
        pp_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
        pp_sys_port.sys_id = tm_dest_info.id;
    }
    else if(tm_dest_info.type == SOC_TMC_DEST_TYPE_LAG) {
        pp_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
        pp_sys_port.sys_id = tm_dest_info.id;
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid type %d"), tm_dest_info.type));
    }
    soc_sand_rv = soc_ppd_frwrd_fec_protection_sys_port_status_set(soc_sand_dev_id, &pp_sys_port, link);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ifg_set(int unit,
                     bcm_port_t port,
                     int speed,
                     bcm_port_duplex_t duplex,
                     int ifg)
{
    int rv = 0;
    bcm_port_t port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if (duplex != BCM_PORT_DUPLEX_FULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only Full Duplex Mode supported")));
    }

    if(ifg < BCM_DPP_NIF_IPG_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IFG size is out of range")));
    }

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ipg_set, (unit, port_ndx, ifg));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ifg_get(int unit,
                     bcm_port_t port,
                     int speed,
                     bcm_port_duplex_t duplex,
                     int *ifg)
{
    int ifg_val = 0;
    int rv = 0;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if (!SOC_PORT_VALID(unit, gport_info.local_port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid Port")));
    }

    if (duplex != BCM_PORT_DUPLEX_FULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only Full Duplex Mode supported")));
    }

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ipg_get, (unit, gport_info.local_port, &ifg_val));
    BCMDNX_IF_ERR_EXIT(rv);

    *ifg = ifg_val;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_selective_get
 * Purpose:
 *      Get requested port parameters
 * Parameters:
 *      unit - switch Unit
 *      port - switch port
 *      info - (OUT) port information structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The action_mask field of the info argument is used as an input
 */

int
bcm_petra_port_selective_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int    rv = BCM_E_NONE;
    uint32 mask = 0;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_port_t              port_ndx;
    bcm_port_if_t           interface;
    uint32                  is_valid, flags;
    
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    port_ndx = gport_info.local_port;

    if (!IS_SFI_PORT(unit, port_ndx)) {
        /*clear masks not relevant for nif links*/
        mask = info->action_mask & ~(BCM_PORT_ATTR_ENCAP_MASK        |
             BCM_PORT_ATTR_PFM_MASK          |
             BCM_PORT_ATTR_LOCAL_ADVERT_MASK |
             BCM_PORT_ATTR_REMOTE_ADVERT_MASK|
             BCM_PORT_ATTR_PHY_MASTER_MASK   |
             BCM_PORT_ATTR_RATE_MCAST_MASK   |
             BCM_PORT_ATTR_RATE_BCAST_MASK   |
             BCM_PORT_ATTR_RATE_DLFBC_MASK   |
             BCM_PORT_ATTR_ABILITY_MASK      |
             BCM_PORT_ATTR_MDIX_STATUS_MASK  |
             BCM_PORT_ATTR_MEDIUM_MASK       |
             BCM_PORT_ATTR_FAULT_MASK);

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_ndx, &is_valid));
        if(!is_valid) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("invalid local port %d"), port_ndx));
        }
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_ndx, &interface));
        if(BCM_PORT_IF_ILKN == interface) {
            mask &= ~(BCM_PORT_ATTR_FRAME_MAX_MASK |
                      BCM_PORT_ATTR_PAUSE_MAC_MASK);
        }
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_ndx, &flags));
        if(!SOC_PORT_IS_NETWORK_INTERFACE(flags)) {
            mask = info->action_mask & ~(BCM_PORT_ATTR_PAUSE_TX_MASK | BCM_PORT_ATTR_PAUSE_RX_MASK);
        }

        if(SOC_PORT_IS_ELK_INTERFACE(flags)) {
            mask = info->action_mask &= ~(
                 BCM_PORT_ATTR_LEARN_MASK        |
                 BCM_PORT_ATTR_DISCARD_MASK      |
                 BCM_PORT_ATTR_VLANFILTER_MASK   |
                 BCM_PORT_ATTR_UNTAG_PRI_MASK    |
                 BCM_PORT_ATTR_UNTAG_VLAN_MASK   |
                 BCM_PORT_ATTR_PFM_MASK          |
                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK |
                 BCM_PORT_ATTR_ENCAP_MASK        |
                 BCM_PORT_ATTR_MDIX_MASK         |
                 BCM_PORT_ATTR_MDIX_STATUS_MASK  |
                 BCM_PORT_ATTR_FRAME_MAX_MASK    |
                 BCM_PORT_ATTR_PAUSE_MAC_MASK    |
                 BCM_PORT_ATTR_PHY_MASTER_MASK   |
                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK |
                 BCM_PORT_ATTR_RATE_MCAST_MASK   |
                 BCM_PORT_ATTR_RATE_BCAST_MASK   |
                 BCM_PORT_ATTR_RATE_DLFBC_MASK   |
                 BCM_PORT_ATTR_ABILITY_MASK      |
                 BCM_PORT_ATTR_MEDIUM_MASK       |
                 BCM_PORT_ATTR_FAULT_MASK);
        }

    } else {
        /*clear masks not relevant for fabric links*/
        mask = info->action_mask & ~(BCM_PORT_ATTR_PAUSE_TX_MASK            | 
                  BCM_PORT_ATTR_PAUSE_RX_MASK                               |
                  BCM_PORT_ATTR_LEARN_MASK                                  |
                  BCM_PORT_ATTR_DISCARD_MASK                                |
                  BCM_PORT_ATTR_VLANFILTER_MASK                             |
                  BCM_PORT_ATTR_UNTAG_PRI_MASK                              |
                  BCM_PORT_ATTR_UNTAG_VLAN_MASK                             |
                  BCM_PORT_ATTR_STP_STATE_MASK                              |
                  /*BCM_PORT_ATTR_INTERFACE_MASK                              |*/
                  BCM_PORT_ATTR_FRAME_MAX_MASK                              |
                  BCM_PORT_ATTR_AUTONEG_MASK                                |
                  BCM_PORT_ATTR_LOCAL_ADVERT_MASK                           |
                  BCM_PORT_ATTR_REMOTE_ADVERT_MASK                          |
                  BCM_PORT_ATTR_ENCAP_MASK                                  |
                  BCM_PORT_ATTR_PFM_MASK                                    |
                  BCM_PORT_ATTR_PHY_MASTER_MASK                             |
                  BCM_PORT_ATTR_RATE_MCAST_MASK                             |
                  BCM_PORT_ATTR_RATE_BCAST_MASK                             |
                  BCM_PORT_ATTR_ABILITY_MASK                                |
                  BCM_PORT_ATTR_MDIX_STATUS_MASK                            |
                  BCM_PORT_ATTR_PAUSE_MAC_MASK                              |
                  BCM_PORT_ATTR_RATE_DLFBC_MASK                             |
                  BCM_PORT_ATTR_MEDIUM_MASK);
    } 


    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        rv = bcm_petra_port_enable_get(unit, port_ndx, &info->enable);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_enable_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK) {
        rv = bcm_petra_port_link_status_get(unit, port_ndx, &info->linkstatus);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_link_status_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        if ((rv = bcm_petra_port_speed_get(unit, port_ndx, &info->speed)) < 0) {
            if (rv != BCM_E_BUSY) {
                if (rv != BCM_E_NONE) {
                    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_speed_get failed: %s"), bcm_errmsg(rv)));
                }
            } else {
                info->speed = 0;
            }
        }
    }

    /* get both if either mask bit set */
    if (mask & (BCM_PORT_ATTR_PAUSE_TX_MASK |
                BCM_PORT_ATTR_PAUSE_RX_MASK)) {
        rv = bcm_petra_port_pause_get(unit, port_ndx,
                      &info->pause_tx, &info->pause_rx);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_pause_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        rv = bcm_petra_port_linkscan_get(unit, port_ndx, &info->linkscan);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_linkscan_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (IS_E_PORT(unit, port_ndx) && (mask & BCM_PORT_ATTR_LEARN_MASK)) {
        rv = bcm_petra_port_learn_get(unit, port_ndx, &info->learn);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_learn_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (IS_E_PORT(unit, port_ndx) && (mask & BCM_PORT_ATTR_DISCARD_MASK)) {
        rv = bcm_petra_port_discard_get(unit, port_ndx, &info->discard);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_discard_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (IS_E_PORT(unit, port_ndx) && (mask & BCM_PORT_ATTR_VLANFILTER_MASK)) {
        rv = bcm_petra_port_vlan_member_get(unit, port_ndx, &info->vlanfilter);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "bcm_petra_port_vlan_member_get failed: %s"),
                       bcm_errmsg(rv)));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (IS_E_PORT(unit, port_ndx) && (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK)) {
        rv = bcm_petra_port_untagged_priority_get(unit, port_ndx,
                                            &info->untagged_priority);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_untagged_priority_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (IS_E_PORT(unit, port_ndx) && (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK)) {
        rv = bcm_petra_port_untagged_vlan_get(unit, port_ndx, &info->untagged_vlan);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_untagged_vlan_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (IS_E_PORT(unit, port_ndx) && (mask & BCM_PORT_ATTR_STP_STATE_MASK)) {
        rv = bcm_petra_port_stp_get(unit, port_ndx, &info->stp_state);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_stp_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        rv = bcm_petra_port_loopback_get(unit, port_ndx, &info->loopback);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_loopback_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
        rv = bcm_petra_port_interface_get(unit, port_ndx, &info->interface);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_interface_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_SPEED_MAX_MASK) {
        rv = bcm_petra_port_speed_max(unit, port_ndx, &info->speed_max);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_speed_max failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        rv = bcm_petra_port_frame_max_get(unit, port_ndx, &info->frame_max);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_frame_max_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    /* Currently unsupported */
    if (mask & BCM_PORT_ATTR_ENCAP_MASK) {
    rv = bcm_petra_port_encap_get(unit, port_ndx, &info->encap_mode);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_encap_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK) {
        rv = bcm_petra_port_autoneg_get(unit, port_ndx, &info->autoneg);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_autoneg_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) {
        rv = bcm_petra_port_ability_local_get(unit, port,
                                            &info->local_ability);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_ability_advert_get failed: %s"), bcm_errmsg(rv)));
        }
        rv = soc_port_ability_to_mode(&info->local_ability,
                                     &info->local_advert);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("soc_port_ability_to_mode failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_REMOTE_ADVERT_MASK) {
        if ((rv = bcm_petra_port_ability_remote_get(unit, port_ndx,
                                             &info->remote_ability)) < 0) {
            info->remote_advert = 0;
            info->remote_advert_valid = FALSE;
            rv = BCM_E_NONE;
        } else {
            rv = soc_port_ability_to_mode(&info->remote_ability,
                                         &info->remote_advert);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("soc_port_ability_to_mode failed: %s"), bcm_errmsg(rv)));
            }
            info->remote_advert_valid = TRUE;
        } 
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK) {
        rv = bcm_petra_port_duplex_get(unit, port_ndx, &info->duplex);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_duplex_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        rv = bcm_petra_port_pause_addr_get(unit, port_ndx, info->pause_mac);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_pause_addr_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_PFM_MASK) {
        rv = bcm_port_pfm_get(unit, port_ndx, &info->pfm);
        if (BCM_FAILURE(rv)) {
            if (rv != BCM_E_UNAVAIL) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_pfm_get failed: %s"), bcm_errmsg(rv)));
            }
        }
    }

    if (mask & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        rv = bcm_port_master_get(unit, port_ndx, &info->phy_master);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_master_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_RATE_MCAST_MASK) {
        rv = bcm_rate_mcast_get(unit, &info->mcast_limit,
                                &info->mcast_limit_enable, port_ndx);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_rate_mcast_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_RATE_BCAST_MASK) {
        rv = bcm_rate_bcast_get(unit, &info->bcast_limit,
                                &info->bcast_limit_enable, port_ndx);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_rate_bcast_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_RATE_DLFBC_MASK) {
        rv = bcm_rate_dlfbc_get(unit, &info->dlfbc_limit,
                                &info->dlfbc_limit_enable, port_ndx);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_rate_dlfbc_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_ABILITY_MASK) {
        rv = bcm_petra_port_ability_get(unit, port_ndx, &info->ability);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_ability_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_MDIX_MASK) {
        rv = bcm_petra_port_mdix_get(unit, port_ndx, &info->mdix);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_mdix_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_MDIX_STATUS_MASK) {
        rv = bcm_petra_port_mdix_status_get(unit, port_ndx, &info->mdix_status);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_mdix_status_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_MEDIUM_MASK) {
        rv = bcm_petra_port_medium_get(unit, port_ndx, &info->medium);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_medium_get failed: %s"), bcm_errmsg(rv)));
        }
    }

    if (mask & BCM_PORT_ATTR_FAULT_MASK) {
        rv = bcm_petra_port_fault_get(unit, port_ndx, &info->fault);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_fault_get failed: %s"), bcm_errmsg(rv)));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_selective_set
 * Purpose:
 *      Set requested port parameters
 * Parameters:
 *      unit - switch unit
 *      port - switch port
 *      info - port information structure
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Does not set spanning tree state.
 */

int
bcm_petra_port_selective_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int    rv;
    uint32 mask;
    int flags = 0;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_port_t              port_ndx;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        if (IS_SFI_PORT(unit, port_ndx)) {
            mask = info->action_mask & ~(BCM_PORT_ATTR_PAUSE_TX_MASK        |
                 BCM_PORT_ATTR_PAUSE_RX_MASK     |
                 BCM_PORT_ATTR_LEARN_MASK        |
                 BCM_PORT_ATTR_DISCARD_MASK      |
                 BCM_PORT_ATTR_VLANFILTER_MASK   |
                 BCM_PORT_ATTR_UNTAG_PRI_MASK    |
                 BCM_PORT_ATTR_UNTAG_VLAN_MASK   |
                 BCM_PORT_ATTR_STP_STATE_MASK    |
                 BCM_PORT_ATTR_FRAME_MAX_MASK    |
                 BCM_PORT_ATTR_ENCAP_MASK        |
                 BCM_PORT_ATTR_PAUSE_MAC_MASK    |
                 BCM_PORT_ATTR_PHY_MASTER_MASK   |
                 BCM_PORT_ATTR_PFM_MASK          |
                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK |
                 BCM_PORT_ATTR_RATE_MCAST_MASK   |
                 BCM_PORT_ATTR_RATE_BCAST_MASK   |
                 BCM_PORT_ATTR_RATE_DLFBC_MASK   |
                 BCM_PORT_ATTR_MDIX_MASK         |
                 BCM_PORT_ATTR_AUTONEG_MASK      |
                 BCM_PORT_ATTR_DUPLEX_MASK);
        } else {
            bcm_port_if_t interface;

            mask = info->action_mask & ~(BCM_PORT_ATTR_ENCAP_MASK        |
                 BCM_PORT_ATTR_PFM_MASK          |
                 BCM_PORT_ATTR_PHY_MASTER_MASK   |
                 BCM_PORT_ATTR_RATE_MCAST_MASK   |
                 BCM_PORT_ATTR_RATE_BCAST_MASK   |
                 BCM_PORT_ATTR_RATE_DLFBC_MASK);

            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_ndx, &interface));
            if(BCM_PORT_IF_ILKN == interface) {
                mask &= ~(BCM_PORT_ATTR_FRAME_MAX_MASK | 
                          BCM_PORT_ATTR_PAUSE_MAC_MASK);
            }
        }
    
        if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
            rv = bcm_petra_port_linkscan_set(unit, port_ndx, info->linkscan);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_linkscan_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
            rv = bcm_petra_port_enable_set(unit, port_ndx, info->enable);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_enable_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
            rv = bcm_petra_port_interface_set(unit, port_ndx, info->interface);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_interface_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_SPEED_MASK) {
            /*When AN=on, speed will be auto-detected. Hence don't set*/
            if ((mask & BCM_PORT_ATTR_AUTONEG_MASK) && info->autoneg) {
                rv = BCM_E_PARAM;
                if (BCM_FAILURE(rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_PORT,
							 (BSL_META_U(unit,
                               "Autoneg:On. SPEED will be "
                               "auto-detected :%s\n"), bcm_errmsg(rv)));
                }
                BCM_IF_ERROR_RETURN(rv);
			}

            rv = bcm_petra_port_speed_set(unit, port_ndx, info->speed);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_speed_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & (BCM_PORT_ATTR_PAUSE_TX_MASK |
                    BCM_PORT_ATTR_PAUSE_RX_MASK)) {
            int     tpause, rpause;

            tpause = rpause = -1;
            if (mask & BCM_PORT_ATTR_PAUSE_TX_MASK) {
                tpause = info->pause_tx;
            }
            if (mask & BCM_PORT_ATTR_PAUSE_RX_MASK) {
                rpause = info->pause_rx;
            }
            rv = bcm_petra_port_pause_set(unit, port_ndx, tpause, rpause);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_pause_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_LEARN_MASK) {
            rv = bcm_petra_port_learn_set(unit, port_ndx, info->learn);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_learn_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_DISCARD_MASK) {
            rv = bcm_petra_port_discard_set(unit, port_ndx, info->discard);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_discard_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_VLANFILTER_MASK) {
            rv = bcm_petra_port_vlan_member_set(unit, port_ndx, info->vlanfilter);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_vlan_member_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
            rv = bcm_petra_port_untagged_priority_set(unit, port_ndx, info->untagged_priority);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_untagged_priority_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK) {
            rv = bcm_petra_port_untagged_vlan_set(unit, port_ndx, info->untagged_vlan);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_untagged_vlan_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        /*
         * Set loopback mode before setting the speed/duplex, since it may
         * affect the allowable values for speed/duplex.
         */

        if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
            rv = bcm_petra_port_loopback_set(unit, port_ndx, info->loopback);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_loopback_set failed: %s"), bcm_errmsg(rv)));
            }
        }
        if (mask & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) {
            if (info->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
                rv = bcm_petra_port_ability_advert_set(unit, port,&(info->local_ability));
                if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_ability_advert_set failed: %s"), bcm_errmsg(rv)));
                }
            }
        }
        if (mask & BCM_PORT_ATTR_AUTONEG_MASK) {
            rv = bcm_petra_port_autoneg_set(unit, port_ndx, info->autoneg);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_autoneg_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_STP_STATE_MASK) {
            rv = bcm_petra_port_stp_set(unit, port_ndx, info->stp_state);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_stp_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
            rv = bcm_petra_port_frame_max_set(unit, port_ndx, info->frame_max);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_frame_max_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        /* Currently unsupported */

        if (mask & BCM_PORT_ATTR_ENCAP_MASK) {
            rv = bcm_petra_port_encap_set(unit, port_ndx, info->encap_mode);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_encap_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
            rv = bcm_petra_port_pause_addr_set(unit, port_ndx, info->pause_mac);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_pause_addr_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_PHY_MASTER_MASK) {
            rv = bcm_port_master_set(unit, port_ndx, info->phy_master);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_master_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_PFM_MASK) {
            rv = bcm_port_pfm_set(unit, port_ndx, info->pfm);
            if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_port_pfm_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_DUPLEX_MASK) {
            /*When AN=on, then duplex method will be auto-detected, hence don't set*/
            if ((mask & BCM_PORT_ATTR_AUTONEG_MASK) && info->autoneg) {
                rv = BCM_E_PARAM;
                if (BCM_FAILURE(rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_PORT,
                                (BSL_META_U(unit,
                                 "Autoneg:On. DUPLEX should be "
                                 "auto-detected :%s\n"), bcm_errmsg(rv)));
                }
                BCM_IF_ERROR_RETURN(rv);
            }
            rv = bcm_petra_port_duplex_set(unit, port_ndx, info->duplex);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_duplex_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_RATE_MCAST_MASK) {
            flags = (info->mcast_limit_enable) ? BCM_RATE_MCAST : 0;
            rv = bcm_rate_mcast_set(unit, info->mcast_limit, flags, port_ndx);
            if (rv == BCM_E_UNAVAIL) {
                rv = BCM_E_NONE;     /* Ignore if not supported on chip */
            }
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_rate_mcast_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_RATE_BCAST_MASK) {
            flags = (info->bcast_limit_enable) ? BCM_RATE_BCAST : 0;
            rv = bcm_rate_bcast_set(unit, info->bcast_limit, flags, port_ndx);
            if (rv == BCM_E_UNAVAIL) {
                rv = BCM_E_NONE;     /* Ignore if not supported on chip */
            }
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_rate_bcast_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_RATE_DLFBC_MASK) {
            flags = (info->dlfbc_limit_enable) ? BCM_RATE_DLF : 0;
            rv = bcm_rate_dlfbc_set(unit, info->dlfbc_limit, flags, port_ndx);
            if (rv == BCM_E_UNAVAIL) {
                rv = BCM_E_NONE;     /* Ignore if not supported on chip */
            }
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_rate_dlfbc_set failed: %s"), bcm_errmsg(rv)));
            }
        }

        if (mask & BCM_PORT_ATTR_MDIX_MASK) {
            rv = bcm_petra_port_mdix_set(unit, port_ndx, info->mdix);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_mdix_set failed: %s"), bcm_errmsg(rv)));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_info_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    info->action_mask2 = BCM_PORT_ATTR_ALL_MASK;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        info->action_mask &= ~(BCM_PORT_ATTR_DISCARD_MASK);
    }

    BCMDNX_IF_ERR_EXIT(bcm_petra_port_selective_set(unit, port, info));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_info_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    bcm_port_info_t_init(info);
    info->action_mask = BCM_PORT_ATTR_ALL_MASK;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        info->action_mask &= ~(BCM_PORT_ATTR_DISCARD_MASK);
    }
    /* coverity[Out-of-bounds:FALSE]  */
    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), gport_info.local_port)) {
        uint32 flags;
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, gport_info.local_port, &flags));
        if(SOC_PORT_IS_STAT_INTERFACE(flags)) {
            info->action_mask &= ~(BCM_PORT_ATTR_LINKSCAN_MASK | BCM_PORT_ATTR_LINKSTAT_MASK);
        }
    }

    if(SOC_IS_ARDON(unit) && !IS_SFI_PORT(unit, gport_info.local_port) ) {
        soc_port_if_t interface_type;
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &interface_type));
        if (interface_type == SOC_PORT_IF_TM_INTERNAL_PKT) {
            info->action_mask = BCM_PORT_ATTR_ENABLE_MASK | BCM_PORT_ATTR_SPEED_MASK;
        }
    }

    BCMDNX_IF_ERR_EXIT(bcm_petra_port_selective_get(unit, port, info));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_info_save(int unit,
             bcm_port_t port,
             bcm_port_info_t *info)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    rv = bcm_petra_port_info_get(unit, port, info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_info_restore(int unit,
                bcm_port_t port,
                bcm_port_info_t *info)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    rv = bcm_petra_port_selective_set(unit, port, info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_link_get
 * Purpose:
 *      Return current up/down status
 * Parameters:
 *      unit - Device unit number.
 *      port - Port number.
 *      hw   - If TRUE, assume hardware linkscan is active and use it.
 *             If FALSE, do not use information from hardware linkscan.
 *      up   - (OUT) TRUE for link up, FALSE for link down.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      HW linkscan is not supported by device.
 *      This routine will be called by the SW linkscan thread at each
 *      linkscan interval.
 */
int
_bcm_petra_port_link_get(int unit, bcm_port_t port, int hw, int *up)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_link_get,(unit, port, up));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_link_get_and_hook_place_holder
 * Purpose:
 *      Wrapper to _bcm_petra_port_link_get, to allow also activate ILKN first packet 
 *      without harming the BCM API.
 *      
 *      The ILKN first packet is activated only if:
 *      1. ilkn_first_packet_sw_bypass SOC property was set.
 *      2. Port is ILKN.
 *      3. Port is not ELK.
 *
 * Parameters:
 *      unit - Device unit number.
 *      port - Port number.
 *      hw   - If TRUE, assume hardware linkscan is active and use it.
 *             If FALSE, do not use information from hardware linkscan.
 *      up   - (OUT) TRUE for link up, FALSE for link down.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This routine will be called by the SW linkscan thread at each
 *      linkscan interval.
 */
int
_bcm_petra_port_link_get_and_hook_placeholder (int unit, bcm_port_t port, int hw, int *up)
{
    int rv = BCM_E_NONE;
    int first_packet_sw_bypass;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_port_link_get(unit, port, hw, up);
    BCMDNX_IF_ERR_EXIT(rv);

    first_packet_sw_bypass = SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_first_packet_sw_bypass;
    if (first_packet_sw_bypass){
        if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
            soc_port_if_t interface;
            uint32 sw_db_flags;
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &sw_db_flags));
            if (interface == SOC_PORT_IF_ILKN && (!SOC_PORT_IS_ELK_INTERFACE(sw_db_flags))){
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_bypass_interface_rx_check_and_enable, (unit, port)));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_link_status_get
 * Purpose:
 *      Return current Link up/down status.
 * Parameters:
 *      unit - PetraSwitch Unit #.
 *      port - PetraSwitch port #.
 *      up - (OUT) Boolean value, FALSE for link down and TRUE for link up
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_petra_port_link_status_get(int unit, bcm_port_t port, int *up)
{
    int  rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(up);

    rv = _bcm_link_get(unit, port, up);

    if (rv == BCM_E_DISABLED) {
        rv = _bcm_petra_port_link_get(unit, port, 0, up);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set the Auto MDIX mode for a port. */
int
bcm_petra_port_mdix_set(int unit, 
    bcm_port_t port, 
    bcm_port_mdix_t mode)
{
    bcm_port_t port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_error_t rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* Just make sure 'port' value is legal */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_mdix_set,(unit, gport_info.local_port, mode));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit: 
    BCMDNX_FUNC_RETURN;
}

/* Get the Auto MDIX mode for a port. */
int
bcm_petra_port_mdix_get(int unit, 
    bcm_port_t port, 
    bcm_port_mdix_t *mode)
{
    _bcm_dpp_gport_info_t   gport_info;
   bcm_error_t rv = BCM_E_NONE; 
   BCMDNX_INIT_FUNC_DEFS ; 

   /* Just make sure 'port' value is legal */
   BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

   rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_mdix_get,(unit, gport_info.local_port, mode));
   BCMDNX_IF_ERR_EXIT(rv);

exit: 
   BCMDNX_FUNC_RETURN ;
}

/* Get the Auto MDIX status  for a port. */
int
bcm_petra_port_mdix_status_get(int unit, 
    bcm_port_t port, 
    bcm_port_mdix_status_t *status)
{
    _bcm_dpp_gport_info_t   gport_info;
    bcm_error_t rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS ;

    /* Just make sure 'port' value is legal */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_mdix_status_get,(unit, gport_info.local_port, status));
    BCMDNX_IF_ERR_EXIT(rv);

    exit:
    BCMDNX_FUNC_RETURN ;
}

int
bcm_petra_port_frame_max_set(int unit, bcm_port_t port, int size)
{
    int rv = BCM_E_NONE;
    bcm_port_t              port_ndx;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,  mbcm_dpp_mgmt_max_pckt_size_set, (unit, port_ndx, size));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_frame_max_get(int unit, bcm_port_t port, int *size)
{
    int  rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit,  mbcm_dpp_mgmt_max_pckt_size_get, (unit, gport_info.local_port, size));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Configure or retrieve the current autonegotiation settings for a port,
 * or restart autonegotiation if already enabled.
 */
int
bcm_petra_port_autoneg_set(int unit, bcm_port_t port, int autoneg)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    /* Make sure the port module is initialized */
    _DPP_PORT_INIT_CHECK(unit);
    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,  mbcm_dpp_port_autoneg_set, (unit, gport_info.local_port, autoneg));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Configure or retrieve the current autonegotiation settings for a port,
 * or restart autonegotiation if already enabled.
 */
int
bcm_petra_port_autoneg_get(int unit, bcm_port_t port, int *autoneg)
{
    int  rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,  mbcm_dpp_port_autoneg_get, (unit, gport_info.local_port, autoneg));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function     : bcm_petra_port_gport_get
 * Description  : Get the GPORT ID for the specified physical port.
 * Parameters   : (IN)  unit      - BCM device number
 *                (IN)  port      - Port number
 *                (OUT) gport     - GPORT ID
 * Returns      : BCM_E_XXX
 * Notes        : Always returns a MODPORT gport or an error.
 */
int
bcm_petra_port_gport_get(int unit, bcm_port_t port, bcm_gport_t *gport)
{
    bcm_error_t             rv;
    _bcm_dpp_gport_info_t   gport_info;
    soc_port_if_t           interface;
    bcm_module_t            my_modid;
    bcm_gport_t             l_gport;
    uint32                  tm_port;
    int                     core;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(gport);

    _DPP_PORT_INIT_CHECK(unit);
    if (IS_SFI_PORT(unit, port))
    {
        *gport = port;
    }
    else
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &my_modid));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, gport_info.local_port, &tm_port, &core));

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &interface));

        BCM_GPORT_MODPORT_SET(l_gport, SOC_DPP_CORE_TO_MODID(my_modid, core), tm_port);

        *gport = l_gport;
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ability_get(int unit,
         bcm_port_t port,
         bcm_port_abil_t *ability_mask)
{
    int rv = BCM_E_PORT;
    bcm_port_ability_t  port_ability;                                      
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ability_mask);

    rv = bcm_petra_port_ability_local_get(unit, port, &port_ability);
    if (BCM_SUCCESS(rv)) {
      rv = soc_port_ability_to_mode(&port_ability, ability_mask);
    }

    LOG_DEBUG(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "bcm_port_advert_get: u=%d p=%d abil=0x%x rv=%d\n"),
               unit, port, *ability_mask, rv));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ability_advert_get(int unit,
          bcm_port_t port,
          bcm_port_ability_t *ability_mask)
{
    int rv = BCM_E_PARAM;
    int port_i;
    _bcm_dpp_gport_info_t gport_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ability_mask);

    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {              
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ability_advert_get,(unit, port_i, ability_mask));
            break;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "bcm_petra_port_ability_advert_get: u=%d p=%d rv=%d\n"),
               unit, port, rv));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ability_advert_set(int unit,
          bcm_port_t port,
          bcm_port_ability_t *ability_mask)
{
    int             rv = BCM_E_NONE;
    bcm_port_ability_t port_ability;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(ability_mask);

    sal_memset(&port_ability, 0, sizeof(bcm_port_ability_t));    
    BCMDNX_IF_ERR_EXIT
        (bcm_petra_port_ability_local_get(unit, port, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.speed_half_duplex   = SOC_PA_ABILITY_NONE;
    port_ability.speed_full_duplex   &= ability_mask->speed_full_duplex;
    port_ability.pause      &= ability_mask->pause;
    port_ability.interface  &= ability_mask->interface;
    port_ability.medium     &= ability_mask->medium;
    port_ability.eee        &= ability_mask->eee;
    port_ability.loopback   &= ability_mask->loopback;
    port_ability.flags      &= ability_mask->flags;
    port_ability.channel    &= ability_mask->channel;
    port_ability.fec        &= ability_mask->fec;

    LOG_DEBUG(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "bcm_petra_port_ability_advert_set: u=%d p=%d rv=%d\n"),
               unit, port, rv));
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x\n"
                            "Interface=0x%08x Medium=0x%08x EEE=0x%08x Loopback=0x%08x Flags=0x%08x\n"),
                 port_ability.speed_half_duplex,
                 port_ability.speed_full_duplex,
                 port_ability.pause, port_ability.interface,
                 port_ability.medium, port_ability.eee,
                 port_ability.loopback, port_ability.flags));

    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ability_advert_set,(unit, port, &port_ability));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ability_remote_get(int unit,
          bcm_port_t port,
          bcm_port_ability_t *ability_mask)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ability_mask);

    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit,  mbcm_dpp_port_ability_remote_get, (unit, gport_info.local_port, ability_mask));
        BCM_ERR_EXIT_NO_MSG(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_ability_local_get(int unit,
         bcm_port_t port,
         bcm_port_ability_t *ability_mask)
{
  bcm_port_info_t info;
  bcm_port_t local_port;
  _bcm_dpp_gport_info_t gport_info;
  int rv;
  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_NULL_CHECK(ability_mask);
  bcm_port_info_t_init(&info);
  rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
  BCMDNX_IF_ERR_EXIT(rv);
  
    if(!IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        if (!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("Invalid port %d"), port));
        }
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {  
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ability_local_get, (unit, local_port, ability_mask));
            BCMDNX_IF_ERR_EXIT(rv);            
            break;
        }
    }

exit:
  BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_duplex_get
 * Purpose:
 *      Get the port duplex settings
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_petra_port_duplex_get(int unit, bcm_port_t port, int *duplex)
{
    _bcm_dpp_gport_info_t   gport_info;
    int                     phy_duplex;
    int                     rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_duplex_get, (unit, gport_info.local_port, &phy_duplex));
    BCMDNX_IF_ERR_EXIT(rv);

    *duplex = phy_duplex ? SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_duplex_set
 * Purpose:
 *      Set the port duplex settings.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      duplex - Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Turns off autonegotiation.  Caller must make sure other forced
 *      parameters (such as speed) are set.
 */

int
bcm_petra_port_duplex_set(int unit, bcm_port_t port, int duplex)
{
    int rv;
    int port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_duplex_set, (unit, port_i, duplex));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_modid_egress_set(int unit,
        bcm_port_t port,
        bcm_module_t modid,
        bcm_pbmp_t pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_modid_egress_get(int unit,
        bcm_port_t port,
        bcm_module_t modid,
        bcm_pbmp_t *pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_modid_enable_set(int unit,
        bcm_port_t port,
        int modid,
        int enable)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_modid_enable_get(int unit,
        bcm_port_t port,
        int modid,
        int *enable)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_stat_enable_set(int unit, bcm_gport_t port, int enable)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_get(int unit, bcm_gport_t port, bcm_port_stat_t stat, 
                      uint64 *val)
{
    _bcm_dpp_gport_info_t gport_info;
    int index;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
    /* coverity[Out-of-bounds:FALSE]  */
    if (!SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, gport_info.local_port)) {
        /*case of physical port counters mode*/
        if(stat == bcmPortStatEgressBytes) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_stat_get(unit, gport_info.local_port, snmpIfOutOctets,val));
        }
        else if (stat == bcmPortStatIngressBytes) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_stat_get(unit, gport_info.local_port, snmpIfInOctets,val));
        }
        else if (stat == bcmPortStatIngressPackets) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_stat_get(unit, gport_info.local_port, snmpIfInUcastPkts,val));
        }
        else if (stat == bcmPortStatEgressPackets) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_stat_get(unit, gport_info.local_port, snmpIfOutUcastPkts,val));
        }

    } else {
        int rv;
        uint32 channel;
        int max_channel_support;

        if(SOC_IS_JERICHO(unit)){
            max_channel_support = SOC_JER_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX;
        } else {
            max_channel_support = SOC_ARAD_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX;
        }

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, gport_info.local_port, &channel));
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), gport_info.local_port) && channel <= max_channel_support) {
            if (stat == bcmPortStatIngressPackets) {
                index = (SOC_IS_JERICHO(unit)) ? soc_jer_counters_ilkn_rx_pkt_counter : ARAD_NIF_RX_ILKN_PER_CHANNEL;
                rv = SOC_CONTROL(unit)->controlled_counters[index].controlled_counter_f(unit, index, gport_info.local_port, val, NULL);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (stat == bcmPortStatEgressPackets) {
                index = (SOC_IS_JERICHO(unit)) ? soc_jer_counters_ilkn_tx_pkt_counter : ARAD_NIF_TX_ILKN_PER_CHANNEL;
                rv = SOC_CONTROL(unit)->controlled_counters[index].controlled_counter_f(unit, index, gport_info.local_port, val, NULL);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (stat == bcmPortStatIngressBytes && SOC_IS_JERICHO(unit)) {
                index = soc_jer_counters_ilkn_rx_byte_counter;
                rv = SOC_CONTROL(unit)->controlled_counters[index].controlled_counter_f(unit, index, gport_info.local_port, val, NULL);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (stat == bcmPortStatEgressBytes && SOC_IS_JERICHO(unit)) {
                index = soc_jer_counters_ilkn_tx_byte_counter;
                rv = SOC_CONTROL(unit)->controlled_counters[index].controlled_counter_f(unit, index, gport_info.local_port, val, NULL);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable per channel counter")));
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable per channel counter")));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_get32(int unit, bcm_gport_t port, 
                        bcm_port_stat_t stat, uint32 *val)
{
    int     rv = BCM_E_NONE;
    uint64  val64;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_port_stat_get(unit, port, stat, &val64);

    COMPILER_64_TO_32_LO(*val, val64);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_multi_get(int unit, bcm_gport_t port, int nstat, 
                            bcm_port_stat_t *stat_arr, 
                            uint64 *value_arr)
{
    int stix;

    BCMDNX_INIT_FUNC_DEFS;

    if (nstat <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid nstat %d"), nstat));
    }

    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);
    
    for (stix = 0; stix < nstat; stix++) {
        BCMDNX_IF_ERR_EXIT
            (bcm_petra_port_stat_get(unit, port, stat_arr[stix],
             &(value_arr[stix])));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_multi_get32(int unit, bcm_gport_t port, int nstat, 
                              bcm_port_stat_t *stat_arr, 
                              uint32 *value_arr)
{
    int stix;

    BCMDNX_INIT_FUNC_DEFS;

    if (nstat <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid nstat %d"), nstat));
    }

    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);
    
    for (stix = 0; stix < nstat; stix++) {
        BCMDNX_IF_ERR_EXIT
      (bcm_petra_port_stat_get32(unit, port, stat_arr[stix],
               &(value_arr[stix])));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_multi_set(int unit, bcm_gport_t port, int nstat, 
                            bcm_port_stat_t *stat_arr, 
                            uint64 *value_arr)
{
    int stix;

    BCMDNX_INIT_FUNC_DEFS;

    if (nstat <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid nstat %d"), nstat));
    }

    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);
    
    for (stix = 0; stix < nstat; stix++) {
        BCMDNX_IF_ERR_EXIT
            (bcm_petra_port_stat_set(unit, port, stat_arr[stix],
             value_arr[stix]));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_multi_set32(int unit, bcm_gport_t port, int nstat, 
                              bcm_port_stat_t *stat_arr, 
                              uint32 *value_arr)
{
    int stix;

    BCMDNX_INIT_FUNC_DEFS;

    if (nstat <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid nstat %d"), nstat));
    }

    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);
    
    for (stix = 0; stix < nstat; stix++) {
        BCMDNX_IF_ERR_EXIT
            (bcm_petra_port_stat_set32(unit, port, stat_arr[stix],
               value_arr[stix]));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_set(int unit, bcm_gport_t port, bcm_port_stat_t stat, 
                      uint64 val)
{
    int     rv = BCM_E_NONE;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload = {0};
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    soc_dpp_config_pp_t     *pp_config;

    BCMDNX_INIT_FUNC_DEFS;

    /* counts tunnel trafic based on the ingress/egress OAM counter access*/
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tunnel_counters_enabled", 0)){

        
        if (stat == bcmPortStatIngressBytes || stat == bcmPortStatEgressBytes) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only packets can be counted")));
        }

        /*check if ingress or egress*/
        if (stat == bcmPortStatIngressPackets) {
            /* In Jericho the LIF in the ingress is the global LIF and at the egress it is the local LIF.*/
            rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS,
                                             &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);
            oem1_key.ingress = 1;
            oem1_key.oam_lif = gport_hw_resources.global_in_lif;

        } else {
            /* In Jericho the LIF in the ingress is the global LIF and at the egress it is the local LIF.*/
            rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                             &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);
            oem1_key.oam_lif = gport_hw_resources.local_out_lif;
        }

        oem1_payload.counter_ndx = COMPILER_64_LO(val);
        pp_config=&(SOC_DPP_CONFIG(unit))->pp;

        if (pp_config->tunnel_counter_init == 0) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_oem1_init, (unit)); 
            BCM_SAND_IF_ERR_EXIT(rv);
            pp_config->tunnel_counter_init = 1;
        }
        
     
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_oem1_entry_set_unsafe, (unit,&oem1_key,&oem1_payload));
        BCM_SAND_IF_ERR_EXIT(rv);


    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_port_stat_set32(int unit, bcm_gport_t port, 
                        bcm_port_stat_t stat, uint32 val)
{
    int     rv = BCM_E_NONE;
    uint64  val64;

    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(val64, 0, val);

    rv = bcm_petra_port_stat_set(unit, port, stat, val64);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_fault_get
 * Purpose:
 *      Get link fault type. 
 * Parameters:
 *      unit -  (IN)  BCM device number 
 *      port -  (IN)  Device or logical port number .
 *      flags - (OUT) Flags to indicate fault type 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_petra_port_fault_get(int unit, bcm_port_t port, uint32 *flags)
{
    int rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(flags);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /* coverity[Out-of-bounds:FALSE]  */
    if (BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), gport_info.local_port)) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_link_fault_get, (unit, gport_info.local_port, flags));
        BCMDNX_IF_ERR_EXIT(rv);
    /* coverity[Out-of-bounds:FALSE]  */
    } else if (BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), gport_info.local_port)) {
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_fault_get, (unit, gport_info.local_port, flags));
        BCMDNX_IF_ERR_EXIT(rv);
    } else  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("bcm_petra_port_fault_get port %d is invalid"),port));
    }

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Set the encapsulation to port mapping from encap_id to port.
 * Currently used only for egress multicast mapping of outlif to local port.
 */
int bcm_petra_port_encap_map_set(
    int unit,           /* input device */
    uint32 flags,       /* currently unused */
    bcm_if_t encap_id,  /* input cud/outlif */
    bcm_gport_t port)   /* input (local egress) port */
{
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (SOC_IS_QAX(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
            (_BSL_BCM_MSG("Unsupported API on current device.")));
    }

    if (flags && flags != BCM_PORT_ENCAP_MAP_ALLOW_CORE_CHANGE && SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported flags")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if (encap_id < 0 || encap_id > ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress encapsulation ID for mapping: 0x%x, should be between 0 and 0x%x."),
                encap_id, ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD));
            }
    } else {
            if (encap_id < 0 || encap_id > ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress encapsulation ID for mapping: 0x%x, should be between 0 and 0x%x."),
               encap_id, ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD));
            }
    }

    /* Derive local port */
    if (BCM_GPORT_IS_TRILL_PORT(port))  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Trill port cannot be mapped to from outlif; only local ports can be mapped to.")));
    }
    if (port == BCM_GPORT_INVALID) {
        gport_info.local_port = _SHR_GPORT_INVALID;
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_cud_to_port_map_set,(unit, flags, encap_id, gport_info.local_port)));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get the encapsulation to port mapping from encap_id.
 * Currently used only for egress multicast mapping of outlif to local port.
 */
int bcm_petra_port_encap_map_get(
    int unit,           /* input device */
    uint32 flags,       /* currently unused */
    bcm_if_t encap_id,  /* input cud/outlif */
    bcm_gport_t *port)  /* output (local egress) port */
{
    SOC_TMC_FAP_PORT_ID local_port = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(port);

    if (SOC_IS_QAX(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
            (_BSL_BCM_MSG("Unsupported API on current device.")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if (encap_id < 0 || encap_id > ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress encapsulation ID for mapping: 0x%x, should be between 0 and 0x%x."),
                encap_id, ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD));
            }
    } else {
            if (encap_id < 0 || encap_id > ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress encapsulation ID for mapping: 0x%x, should be between 0 and 0x%x."),
               encap_id, ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD));
            }
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_cud_to_port_map_get,(unit, encap_id, &local_port)));
    BCM_GPORT_LOCAL_SET(*port, local_port);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Find a free local port and allocate on specified core and interface */
int
_bcm_petra_port_find_free_port_and_allocate(int unit, bcm_core_t core, bcm_port_if_t interface, int with_id, bcm_port_t *port)
{
    uint32 tchannel, is_valid, tm_port = 0, tm_port_i = 0;
    soc_port_t port_i;
    soc_port_if_t interface_i;
    soc_pbmp_t phy_ports, ports_bm;
    uint8 channels[SOC_DPP_MAX_NOF_CHANNELS] = {0};
    uint8 tm_ports[SOC_MAX_NUM_PORTS] = {0};
    bcm_core_t core_i;
    int found = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

    /* first loop over reserved ports */  
    for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); port_i++)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) {
            if (SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].is_reserved && 
                (core == SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].core))
            {
                tm_port = SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].tm_port;
                (*port) = port_i;
                found = 1;
                break;
            }
        }
    }

    /* find free port */
    if (!found) {
        if(!with_id) {
            for(port_i = 0;  port_i < SOC_MAX_NUM_PORTS ; port_i++) {
                if(!SOC_PBMP_MEMBER(ports_bm, port_i)) {
                    (*port) = port_i;
                    break;
                }
            }
        }
    }

    /* Map used channels and tm_ports */
    SOC_PBMP_ITER(ports_bm, port_i){
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface_i));
        if(interface == interface_i) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port_i, &tchannel));
            channels[tchannel] = 1;
        }
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));
        if(core == core_i) {
            tm_ports[tm_port_i] = 1;
        }
    }

    /* find the first free channel */
    for (tchannel = 0; tchannel < SOC_DPP_MAX_NOF_CHANNELS && channels[tchannel]; ++tchannel); 
    if (tchannel >= SOC_DPP_MAX_NOF_CHANNELS) { /* if all channels are taken */
        BCMDNX_ERR_EXIT_MSG(SOC_E_RESOURCE, (_BSL_BCM_MSG("No free channel for operation")));
    }

    if(!found) {
        if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            tm_port = *port;
        } else {
            for (tm_port = 0; tm_port < SOC_MAX_NUM_PORTS && tm_ports[tm_port]; ++tm_port); 
            if (tm_port >= SOC_MAX_NUM_PORTS) { /* if all tm ports are taken */
                BCMDNX_ERR_EXIT_MSG(SOC_E_RESOURCE, (_BSL_BCM_MSG("No free tm port for operation")));
            }
        }
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
            tm_port = *port;
        }
    }
   
    /* Add to DB */
    SOC_PBMP_CLEAR(phy_ports);
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_port_add(unit, core, *port, tchannel, 0, interface, phy_ports));
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, *port, tm_port));
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, *port, tm_port));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_port_rate_egress_pps_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      pps - Rate in packet per second. A value of zero disables rate limiting (full speed).
 *      burst - Maximum burst size .
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_petra_port_rate_egress_pps_set(
    int unit, 
    bcm_port_t port, 
    uint32 pps, 
    uint32 burst)
{
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    /*Supports only by ARAD+*/
    if (SOC_IS_ARAD_B1_AND_BELOW(unit) || SOC_IS_QUX(unit))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature Unavailable")));
    }

    /*Supported only by fabric ports*/
    if (!SOC_PORT_VALID(unit, port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("invalid port")));
    }
    if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("API do not supprted by port %d"), port));
    }
    

    /*config shaper*/
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_rate_egress_pps_set,(unit, port, pps, burst));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_port_rate_egress_pps_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      pps (out)- Rate in packet per second. A value of zero disables rate limiting (full speed).
 *      burst(out) - Maximum burst size.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_port_rate_egress_pps_get(
    int unit, 
    bcm_port_t port, 
    uint32 *pps, 
    uint32 *burst)
{
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    /*Supports only by ARAD+*/
    if (SOC_IS_ARAD_B1_AND_BELOW(unit) || SOC_IS_QUX(unit))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature Unavailable")));
    }

    /*Supported only by fabric ports*/
    if (!SOC_PORT_VALID(unit, port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("invalid port")));
    }
    if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("API do not supprted by port %d"), port));
    }

    
    BCMDNX_NULL_CHECK(pps);
    BCMDNX_NULL_CHECK(burst);

    /*get shaper info*/
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_rate_egress_pps_get,(unit, port, pps, burst));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (*burst == 0 || *pps == 0) {
        *pps = 0;
        *burst = 0;
    }

exit:
    BCMDNX_FUNC_RETURN; 
}

int bcm_petra_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp) 
{
    uint32 base_q_pair, flags, tchannel;
    soc_port_t egr_port, port_i;
    soc_port_if_t interface, interface_i;
    soc_pbmp_t phy_ports, ports_bm;
    uint32 num_of_lanes, num_of_lanes_i, nof_priorities;
    uint32 first_phy_port, first_phy_port_i;
    uint8 channels[SOC_DPP_MAX_NOF_CHANNELS] = {0};
    bcm_gport_t uc_gport, uc_gport_i;
    bcm_gport_t mc_gport, mc_gport_i;
    bcm_gport_t local_gport, local_gport_i;
    bcm_cos_queue_t offset;
    bcm_cosq_egress_multicast_config_t config;
    int ipriority, idp;
    bcm_error_t rv;
    int core=0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get the port information */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

    /* Map used channels */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));
    SOC_PBMP_ITER(ports_bm, port_i){
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface_i));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, &num_of_lanes_i));
        if(interface == interface_i && num_of_lanes_i == num_of_lanes) {
            if(num_of_lanes_i != 0) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &first_phy_port_i));
            } else {
                first_phy_port = 0xFFFFFFFF;
                first_phy_port_i = first_phy_port;
            }
            if(first_phy_port == first_phy_port_i) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port_i, &tchannel));
                channels[tchannel] = 1;
            }
        }
    }
    for (tchannel = 0; tchannel < SOC_DPP_MAX_NOF_CHANNELS && channels[tchannel]; ++tchannel); /* find the first free channel */
    if (tchannel >= SOC_DPP_MAX_NOF_CHANNELS) { /* if all channels are taken */
        BCMDNX_ERR_EXIT_MSG(SOC_E_RESOURCE, (_BSL_BCM_MSG("No free channel for operation")));
    }

    SOC_PBMP_ITER(pbmp, egr_port) {

        /* Add to DB */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_port_add(unit, core, egr_port, tchannel, flags, interface, phy_ports));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, egr_port, egr_port));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, egr_port, egr_port));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_SAME_QPAIR));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.set(unit, egr_port, base_q_pair);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set TM port */
        BCMDNX_IF_ERR_EXIT(arad_egr_dsp_pp_to_base_q_pair_set(unit, core, egr_port, base_q_pair));

        /* Set PP port */
        BCMDNX_IF_ERR_EXIT(arad_port_to_pp_port_map_set_unsafe(unit, egr_port, ARAD_PORT_DIRECTION_BOTH));
    }

    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(uc_gport, port);
    BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(mc_gport, port);
    BCM_GPORT_LOCAL_SET(local_gport, port);

    SOC_PBMP_ITER(pbmp, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &nof_priorities));
        for(ipriority = 0 ; ipriority<nof_priorities ; ipriority++) {
            for(idp = 0; idp<DPP_DEVICE_COSQ_ING_NOF_DP ; idp++) {
                
                /* UC Queues */
                BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(uc_gport_i, port_i);
                rv = bcm_petra_cosq_gport_egress_map_get(unit, uc_gport, ipriority, idp, &offset);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = bcm_petra_cosq_gport_egress_map_set(unit, uc_gport_i, ipriority, idp, offset);
                BCMDNX_IF_ERR_EXIT(rv);

                /* MC Queues */
                BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(mc_gport_i, port_i);
                rv = bcm_petra_cosq_gport_egress_map_get(unit, mc_gport, ipriority, idp, &offset);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = bcm_petra_cosq_gport_egress_map_set(unit, mc_gport_i, ipriority, idp, offset);
                BCMDNX_IF_ERR_EXIT(rv);

                /* MC SP */
                BCM_GPORT_LOCAL_SET(local_gport_i, port_i);
                rv = bcm_petra_cosq_gport_egress_multicast_config_get(unit, local_gport, ipriority, idp, BCM_COSQ_MULTICAST_SCHEDULED, &config);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = bcm_petra_cosq_gport_egress_multicast_config_set(unit, local_gport_i, ipriority, idp, BCM_COSQ_MULTICAST_SCHEDULED, &config);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp) 
{
    return BCM_E_UNAVAIL;
}

int
bcm_petra_port_cable_diag(
    int unit,
    bcm_port_t port,
    bcm_port_cable_diag_t *status)
{
    int rv;
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(status);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = (MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_port_cable_diag,(unit, gport_info.local_port, status)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_port_control_lanes_get(int unit, bcm_port_t port, int* value)
{
    _bcm_dpp_gport_info_t   gport_info;
    uint32 is_valid, lane_count;
    bcm_port_if_t intf;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /*check port is valid*/
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, gport_info.local_port, &is_valid));
    if(!is_valid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid port %d"), gport_info.local_port));
    }
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &intf));

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, gport_info.local_port, &lane_count));
    *value = lane_count;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_port_medium_config_set
 * Description:
 *      Set the medium-specific configuration 
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 to apply the configuration to
 *      config   - must be null
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_petra_port_medium_config_set(int unit, bcm_port_t port,
                           bcm_port_medium_t medium,
                           bcm_phy_config_t  *config)
{
    _bcm_dpp_gport_info_t   gport_info;
    soc_port_t port_i;
    soc_phy_config_t config_tmp;
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not supported for this device %d")));
    }

    if (config != NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_phy_config_t must be NULL")));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_phyctrl_medium_config_set(unit, port_i, medium, &config_tmp));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_port_medium_get
 * Description:
 *      Get the current mediums
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 which is currently selected
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_petra_port_medium_get(int unit, bcm_port_t port,
                    bcm_port_medium_t *medium)
{
    _bcm_dpp_gport_info_t   gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not supported for this device %d")));
    }

    BCMDNX_NULL_CHECK(medium);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    BCMDNX_IF_ERR_EXIT(soc_phyctrl_medium_get(unit, gport_info.local_port, medium));

exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/
/************        PORT DYNAMIC APIs         *****************/
/***************************************************************/
/***************************************************************/

int bcm_petra_port_add(int unit, bcm_port_t port, uint32 flags, bcm_port_interface_info_t *interface_info, bcm_port_mapping_info_t *mapping_info)  
{
    uint32      is_valid, out_port_priority, tm_port = 0, pp_port = 0, first_header_size, first_phy_port, first_phy_port_elk, interface_max_rate;
    uint32      temp_flags = 0;
    uint32      mirror_profile, nof_lanes, nof_channels, kbits_sec_max_get, dummy1, dummy2;
    int         i, core, base_q_pair, curr_q_pair, mc_reserved_pds, header_type, is_channelized, speed, sch_rate, egq_rate;
    int         is_tm_src_syst_port_ext_present = 0, is_stag_enabled = 0, is_snoop_enabled = 0, is_tm_ing_shaping_enabled = 0;
    bcm_pbmp_t  phy_ports;
    bcm_gport_t gport, fap_port;
    bcm_cosq_threshold_t         threshold;
    soc_port_t  master_port, port_i;
    bcm_pbmp_t pbmp, okay, elk_pbmp;
    bcm_cosq_gport_info_t gport_info; 
    SOC_TMC_EGR_PORT_SHAPER_MODE shaper_mode;
    uint32      prog_editor_profile, max_allowed_port_id;
    SOC_TMC_PORT_PP_PORT_INFO    conf;
    SOC_PPC_PORT_INFO            port_info;
    SOC_SAND_SUCCESS_FAILURE     success;
    int         is_lbg_port = 0;
    bcm_lbg_t   lbg_id = 0;
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
    uint32     egress_offset = 0;
    uint32     is_skip_e2e_for_lb = 0, is_skip_nrdy_for_lb = 0;
    soc_pbmp_t ports_same_if;
    uint32     nof_ports_same_if = 0;
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* can't add new port in case port is valid*/
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));    
    if (is_valid) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port is is already added and set")));
    }

    /*check if port is not out-of-range*/
    max_allowed_port_id = FABRIC_LOGICAL_PORT_BASE(unit) - 1;
    if (port > max_allowed_port_id){
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "ERROR: Port %d is exceeding allowed max id (%d)!\n"), port, max_allowed_port_id));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    if (interface_info->interface == SOC_PORT_IF_LBG) {
        lbg_id = interface_info->interface_id;

        if (!SOC_IS_QAX(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Link Bonding isn't support on this chip\n")));            
        }

        if (SOC_DPP_CONFIG(unit)->qax->link_bonding_enable == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Link Bonding is disabled\n")));
        }

        if ((flags & BCM_PORT_ADD_CONFIG_CHANNELIZED) != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("channelize is not supported when adding LB Port\n")));
        }

        if ((flags & BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("base q pair should be set explicitly when adding LB Port\n")));
        }

        if (interface_info->interface_id > SOC_TMC_LB_LBG_MAX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("lbg id should be less than %d\n"), SOC_TMC_LB_LBG_MAX));
        }

        if (mapping_info->base_q_pair != SOC_QAX_LB_LBG_BASE_Q_PAIR(lbg_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("base q pair should be in range of [%d, %d]\n"), 
                SOC_TMC_LB_BASE_Q_PAIR_MIN, SOC_TMC_LB_BASE_Q_PAIR_MAX));
        }

        is_lbg_port = 1;
    }

    if (_BCM_DPP_PORT_ADD_IS_MODEM_PORT(flags)) {
        if (!SOC_IS_QAX(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Link Bonding isn't support on this chip\n")));
        }

        if (SOC_DPP_CONFIG(unit)->qax->link_bonding_enable == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Link Bonding is disabled\n")));
        }

        temp_flags |= SOC_PORT_FLAGS_LB_MODEM;
    }

    if (is_lbg_port == 0) {
        if (SOC_IS_JERICHO(unit)) { /* shift first phy QSGMII lane */
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add, (unit, interface_info->phy_port, &first_phy_port)));
        } else {
            first_phy_port = interface_info->phy_port;
        }

        /* get nof_lanes according to interface type */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nif_nof_lanes_get, (unit,  interface_info->interface,
                                                         first_phy_port, interface_info->num_lanes, &nof_lanes)));

        /* configure counter mapping and interface flags */
        switch(interface_info->interface) {
            case SOC_PORT_IF_XFI:
            case SOC_PORT_IF_SGMII:
            case SOC_PORT_IF_DNX_XAUI:
            case SOC_PORT_IF_XLAUI2:
            case SOC_PORT_IF_XLAUI:
            case SOC_PORT_IF_RXAUI:
                temp_flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                if (SOC_IS_QUX(unit) && first_phy_port > SOC_DPP_DEFS_GET(unit, pmx_base_lane)) {
                    SOC_CONTROL(unit)->counter_map[port] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_GE);
                } else {
                    SOC_CONTROL(unit)->counter_map[port] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_XE);
                }
                break;

            case SOC_PORT_IF_CPU:
                SOC_CONTROL(unit)->counter_map[port] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_CPU);
                break;

            case SOC_PORT_IF_QSGMII:
                temp_flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                SOC_CONTROL(unit)->counter_map[port] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_GE);
                break;

            case SOC_PORT_IF_ILKN:
                temp_flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                SOC_CONTROL(unit)->counter_map[port] = NULL;
                break;

            case SOC_PORT_IF_CAUI:
                temp_flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                /*counter map*/
                SOC_CONTROL(unit)->counter_map[port] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_CE);
                break;

            case SOC_PORT_IF_RCY:
            case SOC_PORT_IF_ERP:
            case SOC_PORT_IF_OLP:
            case SOC_PORT_IF_OAMP:
            case SOC_PORT_IF_SAT:
            case SOC_PORT_IF_IPSEC:
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Interface %d isn't supported"), interface_info->interface));
                break;
        }

        /* set is_stat_if */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if((interface_info->phy_port >= 29) && (interface_info->phy_port <= 32)) {
                temp_flags |= SOC_PORT_FLAGS_STAT_INTERFACE;
            }
        }
        
        BCM_PBMP_CLEAR(phy_ports);
        if (flags & BCM_PORT_ADD_USE_PHY_PBMP) { /* phy ports are given in phy_pbmp */
            if (SOC_IS_JERICHO(unit) && interface_info->interface == SOC_PORT_IF_ILKN) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nif_ilkn_pbmp_get, (unit,  port,
                                                          interface_info->interface_id, &phy_ports, &(interface_info->phy_pbmp))));
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag BCM_PORT_ADD_USE_PHY_PBMP is not supported on device or interface type %"), interface_info->interface));
            }
        } else {
#ifdef BCM_QAX_SUPPORT
            if(SOC_IS_QAX(unit) && interface_info->interface == SOC_PORT_IF_ILKN) {

                BCMDNX_IF_ERR_EXIT(soc_qax_nif_ilkn_pbmp_from_num_lanes_get(unit, nof_lanes, interface_info->phy_port, interface_info->interface_id, &phy_ports));
            } else 
#endif
            {
                for (i = 0; i < nof_lanes; i++) { /* set phy_ports according to phy_port and num_lanes */
                    BCM_PBMP_PORT_ADD(phy_ports, interface_info->phy_port+i); 
                }
            }
        }

        if (SOC_IS_JERICHO(unit)) { /* shift QSGMII lanes */
            bcm_pbmp_t  shifted_phy_ports;

            BCM_PBMP_CLEAR(shifted_phy_ports);
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add_pbmp, (unit, &phy_ports, &shifted_phy_ports)));
            BCM_PBMP_ASSIGN(phy_ports, shifted_phy_ports);

            /* in case of QSGMII interface - set phy_port according to QSGMII sub phy offset */
            if (interface_info->interface == SOC_PORT_IF_QSGMII || 
                (is_lbg_port && (interface_info->interface_id != 0xFFFFFFFF))) {
                BCM_PBMP_CLEAR(phy_ports);
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_nif_qsgmii_pbmp_get, (unit, port, interface_info->interface_id, &phy_ports)));
            }
        }
    } /* end if (is_lbg_port == 0) */
    else {
        BCM_PBMP_CLEAR(phy_ports);
        if (BCM_DPP_PORT_ADD_PHY_PORT_ID_USE_VIRTUAL == interface_info->phy_port) {
            /* nothing to do, free vritual phy port will be allocated in SDK */
        }
        else if (flags & BCM_PORT_ADD_USE_PHY_PBMP) { /* phy ports are given in phy_pbmp */
            if (SOC_IS_JERICHO(unit) && interface_info->interface == SOC_PORT_IF_ILKN) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nif_ilkn_pbmp_get, (unit,  port,
                                                          interface_info->interface_id, &phy_ports, &(interface_info->phy_pbmp))));
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag BCM_PORT_ADD_USE_PHY_PBMP is not supported on device or interface type %"), interface_info->interface));
            }
        } else {
            BCM_PBMP_PORT_ADD(phy_ports, interface_info->phy_port); 
        }
    }

    if (!(is_lbg_port && (BCM_DPP_PORT_ADD_PHY_PORT_ID_USE_VIRTUAL == interface_info->phy_port))) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_nif_sku_restrictions, (unit, phy_ports, interface_info->interface, interface_info->interface_id, 0)));
    }

    if (is_lbg_port) {
        rv = LB_INFO_ACCESS.port_to_lbg.set(unit, port, interface_info->interface_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /*set new port configuration*/
    core = mapping_info->core;
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, port, mapping_info->channel, temp_flags, 
                                    interface_info->interface, phy_ports));

    /* validate quad to core mapping */
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nif_quad_to_core_validate, (unit)));
    }

    if (_BCM_DPP_PORT_ADD_IS_MODEM_PORT(flags)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_same_if));
        SOC_PBMP_COUNT(ports_same_if, nof_ports_same_if);
        if (nof_ports_same_if > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Modem port should be the first port on a physical interface\n")));
        }
    }

    /* set TDM header in sw db */
    if (flags & BCM_PORT_ADD_TDM_PORT) {
       BCMDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port,SOC_TMC_PORT_HEADER_TYPE_TDM));
    }

    /* set lane speed. */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &nof_channels));
    if ((nof_channels == 1) || (_BCM_DPP_PORT_ADD_IS_MODEM_PORT(flags))) {
        BCMDNX_IF_ERR_EXIT(soc_pm_default_speed_get(unit, port, &speed));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, speed));
#ifdef BCM_LB_SUPPORT
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable && _BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags)) 
        {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_add_lb_speed_set(unit, port));
        }
#endif
    }

    /* set is_channelized port sw db indication */
    if (flags & BCM_PORT_ADD_CONFIG_CHANNELIZED || interface_info->interface == SOC_PORT_IF_ILKN) {  
        is_channelized = TRUE;
    }  else {
        is_channelized = FALSE;
    }
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_set(unit, port, is_channelized));
    /* check if it's tdm interleave port before allocating egr interface*/
    if (flags & BCM_PORT_ADD_TDM_QUEUING_ON) {
       SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_port_add(unit,port));
    }
    /* allocate egress interface */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_queuing_egr_interface_alloc, (unit,  port)));

    if ((nof_channels == 1) && (SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && \
        (interface_info->interface == SOC_PORT_IF_ILKN) && !(flags & BCM_PORT_ADD_TDM_QUEUING_ON) && !(flags & BCM_PORT_ADD_TDM_PORT)) {
            LOG_WARN(BSL_LS_BCM_PORT,(BSL_META_U(unit, "Warning: when adding port to ILKN interface while TDM_DEDICATED_QUEUING_MODE is ON, "
                                                       "the first added port must be TDM port or port with TDM_QUEUING_ON\n")));
    }

    /* set protocol offset */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
    if (interface_info->interface == SOC_PORT_IF_ILKN || interface_info->interface == SOC_PORT_IF_CAUI ||
        interface_info->interface == SOC_PORT_IF_QSGMII) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if (first_phy_port == 1) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port, 0, 0));
            } else {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port, 0, 1));
            }
        } else { /* SOC_IS_JERICHO */
            if (interface_info->interface == SOC_PORT_IF_ILKN) {
                if (first_phy_port >= SOC_DPP_FIRST_FABRIC_PHY_PORT(unit)) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_over_fabric_set,(unit, port, interface_info->interface_id)));
                }
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_protocol_offset_verify, (unit,  port, interface_info->interface_id))); 
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port, 0, interface_info->interface_id));
            }
            if (interface_info->interface == SOC_PORT_IF_CAUI) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port, 0, ((first_phy_port-1)/4)));
            }
            if (interface_info->interface == SOC_PORT_IF_QSGMII) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port, 0, interface_info->interface_id));
            }
        }
    } 

    if (_BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags)) {
        /* set number of priorities */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_set(unit, port, mapping_info->num_priorities));

        /* set tm port */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, port, mapping_info->tm_port));
        pp_port = tm_port = mapping_info->tm_port;

        /* set pp port */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, port, mapping_info->tm_port));
    }

    /* set padding size */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port, 0));

    /* set shaper mode */
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.shaper_mode.set(unit,port,SOC_TMC_EGR_PORT_SHAPER_DATA_MODE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* first Header size*/
    first_header_size = soc_property_port_get(unit, port, spn_FIRST_HEADER_SIZE, 0);
    /* start: this section is added for CPU port, should be removed after validation */
    if (interface_info->interface == BCM_PORT_IF_CPU) 
    {
        first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH_2;
    }
    /* end: this section is added for CPU port, should be removed after validation */
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.first_header_size.set(unit, port, first_header_size);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_parser_nof_bytes_to_remove_set, (unit, core, mapping_info->tm_port, first_header_size)));

    if (_BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags)) {
        /* PP fc types*/
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.fc_type.set(unit, port, SOC_TMC_PORTS_FC_TYPE_NONE);
        BCMDNX_IF_ERR_EXIT(rv);

        /* PP flags */
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.pp_flags.set(unit, port, 0);
        BCMDNX_IF_ERR_EXIT(rv);

        /* PP mirror profile */
        mirror_profile = soc_property_port_get(unit, port, spn_MIRROR_PROFILE, 0);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.mirror_profile.set(unit,port,mirror_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        /* PP src port extention */
        is_tm_src_syst_port_ext_present = 0;
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_tm_src_syst_port_ext_present.set(unit, port, is_tm_src_syst_port_ext_present);
        BCMDNX_IF_ERR_EXIT(rv);

        /* PP stag enable */
        is_stag_enabled = soc_property_port_get(unit, port, spn_STAG_ENABLE, 0);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_stag_enabled.set(unit,port,is_stag_enabled);
        BCMDNX_IF_ERR_EXIT(rv);

        /* PP snoop enable */
        is_snoop_enabled = soc_property_port_get(unit, port, spn_SNOOP_ENABLE, 0);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_snoop_enabled.set(unit,port,is_snoop_enabled);
        BCMDNX_IF_ERR_EXIT(rv);

        /* PP tm ing shaping enabled */
        is_tm_ing_shaping_enabled = 0;
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_tm_ing_shaping_enabled.set(unit, port, is_tm_ing_shaping_enabled);
        BCMDNX_IF_ERR_EXIT(rv);


        /* Search for suitable PS and corresponded base_q_pair. */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &out_port_priority));
        if (flags & BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID) {
            base_q_pair = mapping_info->base_q_pair;
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ps_db_alloc_binding_ps_with_id, (unit, port, out_port_priority, base_q_pair)));         
        } else {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ps_db_find_free_binding_ps, (unit, port, out_port_priority, 0 /*is_init*/, &base_q_pair)));
        }
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.set(unit,port,base_q_pair);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set port priorities */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_dsp_pp_priorities_mode_set, (unit, core, tm_port, out_port_priority)));

        /* Set egress port shaper mode */
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.shaper_mode.get(unit,port,&shaper_mode);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_dsp_pp_shaper_mode_set, (unit, core, tm_port, shaper_mode)));

        /* set program editor profile */
        /* go over all q-pairs */
        prog_editor_profile  = 5; /* REMOVE_SYSTEM_HEADER_WITH_USER_HEADER */
        for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + out_port_priority; curr_q_pair++)
        {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_prog_editor_profile_set, (unit,  core, curr_q_pair,prog_editor_profile)));
        }

        /* init tc/dp map */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_gport_egress_map_init(unit, port));

        /* init cgm thresholds */
        if (!(flags & BCM_PORT_ADD_CGM_AUTO_ADJUST_DISABLE)) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_fc_port_threshold_init(unit, port, &mc_reserved_pds, 1));

            /* increment sp0 reserved pds */
            BCM_COSQ_GPORT_CORE_SET(gport, core); /* global threshold*/
            threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_EGRESS;
            threshold.type = bcmCosqThresholdAvailablePacketDescriptors;
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_threshold_get(unit, gport, -1, &threshold));
            threshold.value += (mc_reserved_pds*out_port_priority);
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_threshold_set(unit, gport, -1, &threshold)); 
        }
    }
    /*init stat if billing filter egress */
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        BCMDNX_IF_ERR_EXIT(soc_jer_stat_if_billing_filter_egress_port_set(unit, port, 1));
    }
#ifdef BCM_QAX_SUPPORT
    /*check led bypass option*/
    if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit) && (interface_info->interface == SOC_PORT_IF_QSGMII)) {
        BCMDNX_IF_ERR_EXIT(soc_qax_led_pmlq_bypass_config_adjust(unit, port, 1));
    }
#endif

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        int full_reset = TRUE;
        if (interface_info->interface == BCM_PORT_IF_CAUI && nof_channels == 1) {
            /* only partially reset clp if ELK interface interlaps with the CAUI */
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_ELK, &elk_pbmp));
            BCM_PBMP_ITER(elk_pbmp, port_i) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &first_phy_port_elk));
                if ((nof_lanes == 10) && ((first_phy_port + nof_lanes) == first_phy_port_elk)) {
                    full_reset = FALSE;
                }
            }
            BCMDNX_IF_ERR_EXIT(soc_pm_port_clp_reset(unit, port, full_reset)); 
        } else if (interface_info->interface == BCM_PORT_IF_ILKN) {
            if(soc_arad_is_device_ilkn_disabled(unit, 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Device is ILKN disabled.\n")));
            }
        }
    }

    /* set reference clock for ports in channelized NIFs */
    if (!is_lbg_port) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
        if (nof_channels > 1) 
        {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                INT_PHY_SW_STATE(unit, port) = INT_PHY_SW_STATE(unit, master_port);
                EXT_PHY_SW_STATE(unit, port) = EXT_PHY_SW_STATE(unit, master_port);
                sal_memcpy(&SOC_PHY_INFO(unit, port),&SOC_PHY_INFO(unit, master_port),sizeof(soc_phy_info_t));
            }

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_reference_clock_set, (unit, port)));
        }
    }

    if (_BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags)) {
        /* add HR */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_hr_handle(unit, port, 1));
    }

    /*port to IF mapping*/
    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_to_interface_map_set, (unit, port, FALSE /* unmap */, TRUE /* is_dynamic_port */)));

    if (_BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags)) 
    {
        if (nof_channels == 1) {
            /* init FQP/PQP calendars for new interface */
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_q_nif_cal_set,(unit)));

            /* EGQ IF to E2E IF mapping */

            /* 
             * Only 1 e2e interface is needed for RESERVE EGQ interface.
             * Below codes is used to avoid to alloc more than 1 e2e interface for ports which are connect to RESERVE EGQ interface 
             */
            if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
                if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_lb_ports_on_reserve_intf_get, (unit, &lb_ports, &lb_port_num)));
                    if (lb_port_num > 1) {
                        is_skip_e2e_for_lb = 1;
                    }
                }
            }

            if (!is_skip_e2e_for_lb) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_e2e_interface_allocate, (unit,  port)));
            }
        }


        /* set default header type */
        header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        if (flags & BCM_PORT_ADD_TDM_PORT) {
           header_type = BCM_SWITCH_PORT_HEADER_TYPE_TDM;
        }
        /*update header in SW DB to TYPE_NONE to fully configurate newly added TDM if needed*/
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port,SOC_TMC_PORT_HEADER_TYPE_NONE));
        BCMDNX_IF_ERR_EXIT(bcm_petra_switch_control_port_set(unit, port,  bcmSwitchPortHeaderType, header_type));

        /* egress queueing configuration update */
        /*1. Disable Higher bandwidth requests for ports above 100g or higher*/
        /*2. Enable for any other port*/
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_q_fqp_scheduler_config, (unit)));

        
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
        BCM_SAND_IF_ERR_EXIT(soc_ppd_port_info_get(unit,core,pp_port,&port_info));

        port_info.enable_same_interfac_filter = TRUE;
        port_info.same_interface_filter_skip = FALSE;
        port_info.initial_action_profile.trap_code = _BCM_PETRA_UD_DFLT_TRAP;
        port_info.flags = (SOC_PPC_PORT_EGQ_PP_PPCT_TBL |
                           SOC_PPC_PORT_IHB_PINFO_FLP_TBL |
                           SOC_PPC_PORT_IHP_PINFO_LLR_TBL);
        BCM_SAND_IF_ERR_EXIT(soc_ppd_port_info_set(unit,core,pp_port,&port_info));
        /* end default port info set*/ 

        port_info.enable_same_interfac_filter = TRUE;
        port_info.same_interface_filter_skip = FALSE;
        port_info.initial_action_profile.trap_code = _BCM_PETRA_UD_DFLT_TRAP;
        port_info.flags = (SOC_PPC_PORT_EGQ_PP_PPCT_TBL |
                           SOC_PPC_PORT_IHB_PINFO_FLP_TBL |
                           SOC_PPC_PORT_IHP_PINFO_LLR_TBL);
        /* Port vlan translation profile Initial-VID settings */
        if ((SOC_DPP_CONFIG(unit))->pp.port_use_initial_vlan_only_enabled) {
            uint32 init_vid_enabled = 1;
            BCMDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port, &init_vid_enabled));

            /* In this port, support use Initial-VID with no difference between
             * untagged packets and tagged packets. so set port vlan translation file as
             * SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID.
             */
            if (!init_vid_enabled) {
                port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID; 
                port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
            }
        }
        BCM_SAND_IF_ERR_EXIT(soc_ppd_port_info_set(unit,core,pp_port,&port_info));
        /* end default port info set*/

        /*PP port configuration {*/
        SOC_TMC_PORT_PP_PORT_INFO_clear(&conf);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.fc_type.get(unit, port, &conf.fc_type);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.pp_flags.get(unit, port, &conf.flags);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.mirror_profile.get(unit, port, &conf.mirror_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        conf.is_tm_src_syst_port_ext_present = is_tm_src_syst_port_ext_present;
        conf.is_stag_enabled = is_stag_enabled;
        conf.is_snoop_enabled = is_snoop_enabled;
        conf.is_tm_ing_shaping_enabled = is_tm_ing_shaping_enabled;
        conf.first_header_size = first_header_size;
        conf.header_type = header_type;
        conf.header_type_out = header_type;

        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_set, (unit, core, pp_port, &conf,&success)));

        /*PP port configuration }*/
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_to_pp_port_map_set, (unit, port, SOC_TMC_PORT_DIRECTION_INCOMING)));

        /* congiure interface attributes */
        if (nof_channels == 1) {
            if (flags & BCM_PORT_ADD_HIGH_GIG) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_encap_set(unit, port, BCM_PORT_ENCAP_HIGIG2));
            }

            if (is_channelized) {
                /* by default dual mode is used */
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port, FALSE));
            } else {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port, TRUE));
            }

            /*Fast port enable*/
            if (interface_info->interface == BCM_PORT_IF_ILKN) { 
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_q_fast_port_set, (unit, port, 1, 1)));
            } else if (interface_info->interface == BCM_PORT_IF_CAUI) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_q_fast_port_set, (unit, port, 0, 1)));
            } else {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_q_fast_port_set, (unit, port, 0, 0)));
            }
        }
    }

    /* probe new port (In arad only the first channel is probed) 
     * For LB, no need to probe port 
     */
    if (!(flags & BCM_PORT_ADD_DONT_PROBE) && (!is_lbg_port)) {
        if (nof_channels == 1 || SOC_IS_JERICHO(unit)) {
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_CLEAR(okay);
            BCM_PBMP_PORT_ADD(pbmp, port);
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_probe(unit, pbmp, &okay));
        }
    }

    if (is_lbg_port) {
        /* mark port as initialized */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 1));
    }

    if (_BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags)) {
        /*set NRDY threshold */
        if (SOC_IS_JERICHO(unit)) {
            if (nof_channels == 1) {
                /* 1. skip allocate NRDY template twice for RESERVE EGQ interface. */
                if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
                    if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                        SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
                        if (lb_port_num > 1) {
                            is_skip_nrdy_for_lb = 1;
                        }
                    }
                }

                if (!is_skip_nrdy_for_lb) {
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_allocate(unit, port));
                }
            }
        }

        /* set all TDM interleave ports to high priority for egress scheduler (calendars) */
        if (flags & BCM_PORT_ADD_TDM_QUEUING_ON) {
            BCM_GPORT_LOCAL_SET(gport, port);
            BCMDNX_IF_ERR_EXIT(bcm_cosq_gport_sched_set(unit, gport, 0, BCM_COSQ_SP0, 0));
        }

        /* set interface shaper for non channelized interfaces */
        if (!is_channelized) {

            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &interface_max_rate));
            sch_rate = interface_max_rate + (interface_max_rate / 100) * 5; /* 5% speedup */
            egq_rate = interface_max_rate + (interface_max_rate / 100) * 1; /* 1% speedup */

            /* set EGQ interface bandwidth */
            gport_info.in_gport = port;
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,&gport_info));
            BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_port_get(unit,gport_info.out_gport,0,&fap_port));

            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_get(unit,fap_port,0,&dummy1,&kbits_sec_max_get,&dummy2));
            if ((kbits_sec_max_get + egq_rate*1000) <= SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit)) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max_get + egq_rate*1000),0));
            } else {
                BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit),0));
            }

            /*set sch bandwidth*/
            gport_info.in_gport = port;
            BCMDNX_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPort,&gport_info));
            BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_port_get(unit,gport_info.out_gport,0,&fap_port));

            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_get(unit,fap_port,0,&dummy1,&kbits_sec_max_get,&dummy2));
            if ((kbits_sec_max_get + sch_rate*1000) <= SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit)) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max_get + sch_rate*1000),0));
            } else {
                BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit,fap_port,0,0,SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit),0));
            }

        }

        if (is_lbg_port) {
            rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_set(unit, lbg_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Map local port to base_q_pair */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_dsp_pp_to_base_q_pair_set, (unit, core, tm_port, base_q_pair)));
    }

    /* Port linkscan initial configure */
    if (!is_lbg_port)
    {
        BCMDNX_IF_ERR_EXIT(_bcm_linkscan_port_init(unit, port));
    }

exit:
    /* set back to invalid */
    if (unit >= 0 && unit < BCM_LOCAL_UNITS_MAX && BCM_FUNC_ERROR)
    {
        /*
         * Don't roll back port SW db in case the error is due to port already existing,
         * Otherwise it will disable an existing working port
 */
        if (!is_valid)
        {

            soc_port_sw_db_is_valid_port_set(unit, port, 0);
        }
    }
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_port_get(int unit, bcm_port_t port, uint32 *flags, bcm_port_interface_info_t *interface_info, bcm_port_mapping_info_t *mapping_info) 
{
    bcm_pbmp_t phy_ports;
    int is_channelized;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    port = gport_info.local_port;

    BCM_PBMP_CLEAR(phy_ports);
    *flags = 0;

    /* get tm port and core */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &mapping_info->tm_port, &mapping_info->core));
            
    /* get number of priorities */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &mapping_info->num_priorities));

    /* get interface type */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &(interface_info->interface)));

    /* get num of lanes */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &(interface_info->num_lanes)));

    /* get channel id */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &(mapping_info->channel)));

    /* get bsae queue pair */
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &(mapping_info->base_q_pair));
    BCMDNX_IF_ERR_EXIT(rv);

    /* get first physical port and phy_pbmp */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
    if (SOC_IS_JERICHO(unit)) {
        soc_pbmp_t shifted_pbmp;

        BCM_PBMP_CLEAR(shifted_pbmp);
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove_pbmp, (unit,&phy_ports,&shifted_pbmp)));
        BCM_PBMP_ITER(shifted_pbmp, interface_info->phy_port) { break; }
        if (interface_info->interface == BCM_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nif_ilkn_phys_aligned_pbmp_get, (unit, port, &shifted_pbmp, 0)));
            *flags |= BCM_PORT_ADD_USE_PHY_PBMP;
        }
        BCM_PBMP_ASSIGN(phy_ports, shifted_pbmp);
    } else {
        BCM_PBMP_ITER(phy_ports, interface_info->phy_port) { break; }
    }
    BCM_PBMP_ASSIGN(interface_info->phy_pbmp ,phy_ports);

    /* get is channelized port indication */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
    if (is_channelized) {
        *flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
    }

    /* get interface id */
    if (interface_info->interface == BCM_PORT_IF_ILKN || interface_info->interface == BCM_PORT_IF_CAUI || 
        interface_info->interface == BCM_PORT_IF_QSGMII) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &interface_info->interface_id)); 
    } else if (interface_info->interface == BCM_PORT_IF_LBG) {
        BCMDNX_IF_ERR_EXIT(LB_INFO_ACCESS.port_to_lbg.get(unit, port, &(interface_info->interface_id)));
    } else {
        interface_info->interface_id = 0;
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN; 
}
/*
 * Function:
 *      _bcm_port_total_rate_for_non_channelized_ports_get
 * Description:
 *      Get the total interface rate for non-channelized ports
 * Parameters:
 *      unit     - Device number
 *      core     - core id
 *      total_rate   - Total rate
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_port_total_rate_for_non_channelized_ports_get(
    int unit,
    bcm_core_t core,
    int *total_rate)
{
    int is_channelized;
    uint32 interface_rate;
    bcm_pbmp_t pbmp;
    bcm_port_t logical_port;

    BCMDNX_INIT_FUNC_DEFS;

    *total_rate = 0;
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));

    BCM_PBMP_ITER(pbmp, logical_port)
    {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, logical_port, &is_channelized));
        if (!is_channelized)
        {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, logical_port, &interface_rate));
            *total_rate += interface_rate;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_port_remove(int unit, bcm_port_t port, uint32 flags) 
{
    uint32           is_valid, is_master, counter_flags, dummy1, dummy2, cal_id, kbits_sec_max_get;
    bcm_port_if_t    interface = BCM_PORT_IF_NULL;
    int              core, old_profile, new_profile, is_last, is_allocated, mc_reserved_pds;
    int              base_q_pair, profile, counter_interval, is_channelized, i, port_j, total_rate;
    uint32           tm_port, nof_channels, out_port_priority, first_phy_port, first_phy_port_elk, nof_lanes;
    bcm_gport_t      gport;
    bcm_pbmp_t       pbmp, detached, counter_pbmp;
    soc_pbmp_t       ports_same_if;
    soc_port_t       next_master, port_i;
    bcm_dpp_cosq_egress_queue_mapping_info_t egr_queue_mapping_info;
    bcm_dpp_cosq_egress_thresh_key_info_t    thresh_info;
    bcm_cosq_threshold_t                     threshold;
    bcm_cosq_gport_info_t                    gport_info;
    bcm_error_t                              rv;
    SOC_SAND_OCC_BM_PTR                      cals_occ;
    uint8                                    is_tdm_queuing;
    uint32           port_flags = 0, lb_port_num = 0, egress_offset = 0;
    soc_pbmp_t       lb_ports;
    int speed;
    uint32 egq_rate;
    soc_port_t fap_port;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all), port)) {
        BCMDNX_ERR_EXIT_MSG(SOC_E_PORT, (_BSL_BCM_MSG("Port %d is invalid"),port));
    }

    /*check port not already defined*/
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        BCMDNX_ERR_EXIT_MSG(SOC_E_PORT, (_BSL_BCM_MSG("Port %d is invalid"),port));
    }

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &port_flags));

    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        if (_BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(port_flags)) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
            if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_lb_ports_on_reserve_intf_get, (unit, &lb_ports, &lb_port_num)));
            }
        }
    }

    if (_BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(port_flags)) {
        /* validate port bw is 0 before it's removed */
        gport_info.in_gport = port;
        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,&gport_info));
        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_get(unit,gport_info.out_gport,0,&dummy1,&kbits_sec_max_get,&dummy2));
        if (kbits_sec_max_get != 0) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "unit(%d): Can't remove port if its bandwidth isn't 0\n"), unit)); 
            BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
        }

        /* flush ports queue to make sure no packets are left in the queue */
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_cosq_flush,(unit,port,FALSE)));

        /* free lbg fifo */
        if (IS_LBG_PORT(unit, port)) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_port_speed_set,(unit, port, 0)));
        }
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_same_if));
        SOC_PBMP_ITER(ports_same_if, port_j) {
            if (port_j == port) {
                continue;
            }
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "unit(%d): Can't remove port if other ports exists on same interface\n"), unit)); 
            BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
        }
    }

    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
    if (_BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(port_flags)) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core)); 
    }
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &nof_channels));

    /* free NRDY threshold */
    if (SOC_IS_JERICHO(unit)) {
        if ((nof_channels == 1) && (lb_port_num <= 1)) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nrdy_thresh_free(unit, port));
        }
    }

    if (!is_channelized) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_port_speed_get(unit, port, &speed));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        BCMDNX_IF_ERR_EXIT(_bcm_port_total_rate_for_non_channelized_ports_get(unit, core, &total_rate));
        /* Exclude the speed of the removed port from total rate */
        total_rate -= speed;
        egq_rate = total_rate + (total_rate / 100) * 1;

        gport_info.in_gport = port;
        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,&gport_info));
        BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_port_get(unit,gport_info.out_gport,0,&fap_port));

        /* Update EGQ interface bandwidth */
        if (egq_rate * 1000 >= SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit)) {
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit, fap_port, 0, 0, SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit), 0));
        } else {
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_bandwidth_set(unit, fap_port, 0, 0, egq_rate * 1000, 0));
        }
    }

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    /* check there is TDM port or TDM queuing-on ports in ILKN interface */
    if ((SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && \
          (interface == SOC_PORT_IF_ILKN) && (nof_channels != 1)) {

        is_tdm_queuing = 0;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_same_if));
        SOC_PBMP_ITER(ports_same_if, port_j) {
            if (port_j == port) {
                continue;
            }
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port_j, &is_tdm_queuing));
            if (is_tdm_queuing || IS_TDM_PORT(unit, port_j)) {
                is_tdm_queuing = 1;
                break;
            }
        }
        if (!is_tdm_queuing) {
            LOG_WARN(BSL_LS_BCM_PORT,(BSL_META_U(unit, "Warning: when removing port from ILKN interface while TDM_DEDICATED_QUEUING_MODE is ON, "
                                                       "at least one remaining port must be TDM port or port with TDM_QUEUING_ON\n")));
        }
    }

    if (!IS_LBG_PORT(unit, port)) {
        /* in case this is the last port added on the interface(for Arad), then detach the physical interface */
        if ((nof_channels == 1 || SOC_IS_JERICHO(unit))) {
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_CLEAR(detached);
            BCM_PBMP_PORT_ADD(pbmp, port);
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_detach(unit, pbmp, &detached));
        }
        else { /* disasble linkscan*/
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE));
        }
    }

    if (_BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(port_flags)) {
        /* release channelized calendars in case this is the last channelized port removed */
        if (nof_channels == 1 && is_channelized && (lb_port_num <= 1)) { 
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.channelized_cals_occ.get(unit, core, &cals_occ);
            BCMDNX_IF_ERR_EXIT(rv);

            /* high priority cal */
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port, &cal_id));
            if (cal_id != INVALID_CALENDAR) {
                BCM_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, cals_occ, cal_id, FALSE));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, port, INVALID_CALENDAR));
            }

            /* low priority cal */
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port, &cal_id));
            if (cal_id != INVALID_CALENDAR) {
                BCM_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, cals_occ, cal_id, FALSE));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, port, INVALID_CALENDAR));
            }
        }
    }
#ifdef BCM_QAX_SUPPORT
    /*check led bypass option*/
    if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit) && (interface == SOC_PORT_IF_QSGMII)) {
        BCMDNX_IF_ERR_EXIT(soc_qax_led_pmlq_bypass_config_adjust(unit, port, 0));
    }
#endif

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        int full_reset = TRUE;
        if (interface == SOC_PORT_IF_CAUI && nof_channels == 1) {
            /* only partially reset clp if ELK interface interlaps with the CAUI */
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_ELK, &pbmp));
            BCM_PBMP_ITER(pbmp, port_i) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i,& first_phy_port_elk));

                if ((nof_lanes == 10) && ((first_phy_port + nof_lanes) == first_phy_port_elk)) {
                    full_reset = FALSE;
                }
            }
            BCMDNX_IF_ERR_EXIT(soc_pm_port_clp_reset(unit, port, full_reset)); 
        }
    }

    if (_BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(port_flags)) {

        /* release egress port cls */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_egress_port_discount_cls_free(unit, core, tm_port, &is_last));

        /* release HR */
        _bcm_petra_cosq_hr_handle(unit, port, 0);

        /* release tc/dp mapping resources */ 
        sal_memset(&egr_queue_mapping_info,0x0,sizeof(bcm_dpp_cosq_egress_queue_mapping_info_t));  
        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_gport_egress_queue_mapping_hw_get(unit,DPP_COSQ_PB_EGR_QUEUE_DEFAULT_PROFILE_MAP,core,&egr_queue_mapping_info));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_egress_queue_mapping_exchange(unit,tm_port,core,&egr_queue_mapping_info,&old_profile,&is_last,&new_profile,&is_allocated));

        /* start - init port cgm thresholds */
        sal_memset(&thresh_info,0x0,sizeof(bcm_dpp_cosq_egress_thresh_key_info_t));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_cosq_egr_thresh_data_get(unit,core,port,&thresh_info));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &out_port_priority));

        mc_reserved_pds = 0;
        for(i = 0; i < out_port_priority; i++) {
            mc_reserved_pds += thresh_info.unsch_drop_packet_descriptors_available_tc[i];
        }

        sal_memset(&thresh_info,0x0,sizeof(bcm_dpp_cosq_egress_thresh_key_info_t));
        profile = SOC_DPP_DEFS_GET(unit, cosq_egr_default_thresh_type);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_egress_thresh_entry_get(unit,core,profile,&thresh_info));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_cosq_egr_thresh_exchange(unit,core,port,&thresh_info,&old_profile,&is_last,&new_profile,&is_allocated)); 

        /* end - init port cgm thresholds */

        /* reduce reserved pds for sp0 (only sp0 is used in dynamic port) */
        if (!(flags & BCM_PORT_ADD_CGM_AUTO_ADJUST_DISABLE)) {
            BCM_COSQ_GPORT_CORE_SET(gport, core); /* global threshold*/
            threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_EGRESS;
            threshold.type = bcmCosqThresholdAvailablePacketDescriptors;
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_threshold_get(unit, gport, -1, &threshold));
            threshold.value -= mc_reserved_pds;
            if (threshold.value < 0) {
                threshold.value = 0;
            }
            BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_gport_threshold_set(unit, gport, -1, &threshold));
        }

        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit,port,(uint32 *)(&base_q_pair));
        BCMDNX_IF_ERR_EXIT(rv);

        /* int cgm interface to default */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit ,mbcm_dpp_egr_q_cgm_interface_set, (unit, core, tm_port, ARAD_EGQ_IFC_DEF_VAL)));

        /* set cos_map_profile to 0 */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_q_profile_map_set, (unit, core, tm_port, 0)));

        /* PCT and PPCT cgm_port_profile init to default */
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_ofp_thresh_type_set,(unit, core, tm_port, SOC_TMC_EGR_PORT_THRESH_TYPE_15)));

        /* set TM port to invalid base_q_pair */
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_dsp_pp_to_base_q_pair_set,(unit, core, tm_port, ARAD_EGR_INVALID_BASE_Q_PAIR)));
    }

    /* unmap port */
    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_to_interface_map_set, (unit, port, TRUE /* unmap */, TRUE /*is_dynamic_port */)));

    if (_BCM_DPP_PORT_ADD_IS_NOT_MODEM_PORT(flags))
    {
        if ((nof_channels == 1) && (lb_port_num <= 1)) {
            /* init FQP/PQP when interface is removed */
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_q_nif_cal_set,(unit)));

            /* free EGQ IF to E2E IF mapping */
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_e2e_interface_deallocate, (unit,  port)));
        }
    }
    
    /* free egress interface */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_queuing_egr_interface_free, (unit, port)));

    if (_BCM_DPP_PORT_REMOVE_IS_NOT_MODEM_PORT(port_flags)) {
        /* release base q pair (PS) */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ps_db_release_binding_ps, (unit, port, base_q_pair)));
    }

    /* in case port is channelized and the removed port is master port, stop counter thread and update with new counter pbmp with new master port*/
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
    if ((nof_channels > 1) && 
        (is_master || 
        (interface == SOC_PORT_IF_ILKN && SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel))) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_next_master_get(unit, port, &next_master));
        BCMDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
        soc_counter_stop(unit);
        SOC_PBMP_PORT_REMOVE(counter_pbmp, port);
        if (SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode != soc_arad_stat_ilkn_counters_mode_packets_per_channel) {
            SOC_PBMP_PORT_ADD(counter_pbmp, next_master);
        }
        /* map next master port to counter */
        switch(interface) {
            case SOC_PORT_IF_XFI:
            case SOC_PORT_IF_SGMII:
                SOC_CONTROL(unit)->counter_map[next_master] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_XE);
                break;

            case SOC_PORT_IF_CPU:
                SOC_CONTROL(unit)->counter_map[next_master] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_CPU);
                break;

            case SOC_PORT_IF_RXAUI:
                SOC_CONTROL(unit)->counter_map[next_master] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_XE);
                break;

            case SOC_PORT_IF_DNX_XAUI:
            case SOC_PORT_IF_XLAUI:
            case SOC_PORT_IF_XLAUI2:
                SOC_CONTROL(unit)->counter_map[next_master] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_XE);
                break;

            case SOC_PORT_IF_QSGMII:
                SOC_CONTROL(unit)->counter_map[next_master] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_GE);
                break;

            case SOC_PORT_IF_ILKN:
                if (SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode != soc_arad_stat_ilkn_counters_mode_packets_per_channel) {
                    SOC_CONTROL(unit)->counter_map[next_master] = NULL;
                }
                break;

            case SOC_PORT_IF_CAUI:
                SOC_CONTROL(unit)->counter_map[next_master] = &SOC_CTR_DMA_MAP(unit, SOC_CTR_TYPE_CE);
                break;

            case SOC_PORT_IF_RCY:
            case SOC_PORT_IF_ERP:
            case SOC_PORT_IF_OLP:
            case SOC_PORT_IF_OAMP:
            case SOC_PORT_IF_SAT:
            case SOC_PORT_IF_IPSEC:
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Interface %d isn't supported"), interface));
                break;
        }

        BCMDNX_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (nof_channels > 1) {
            if (is_master) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_next_master_get(unit, port, &next_master));
                if(INT_PHY_SW_STATE(unit, port) != NULL) {
                    INT_PHY_SW_STATE(unit, port)->port = next_master;
                }

                if(EXT_PHY_SW_STATE(unit, port) != NULL) {
                    EXT_PHY_SW_STATE(unit, port)->port = next_master;
                }
            }

            INT_PHY_SW_STATE(unit, port) = NULL;
            EXT_PHY_SW_STATE(unit, port) = NULL;

            sal_memset(&SOC_PHY_INFO(unit, port), 0, sizeof(soc_phy_info_t));
        }
    }

    /* remove port from db, pause Linkscan thread before port removal to avoid Linkscan port validity check failure */
    BCMDNX_IF_ERR_EXIT(_bcm_linkscan_pause(unit)); 
    rv = soc_port_sw_db_port_remove(unit, port);
    BCMDNX_IF_ERR_EXIT(_bcm_linkscan_continue(unit));
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* remove from tdm interleave port */
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_port_remove(unit,port));


    /* reset counter map*/
    SOC_CONTROL(unit)->counter_map[port] = NULL;

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_port_nif_priority_set(  int                       unit,
                                       bcm_gport_t               local_port,
                                       uint32                    flags,
                                       uint32                    allow_tdm,
                                       bcm_port_nif_prio_t*      priority,
                                       bcm_pbmp_t*               affected_ports )
{
    int core;
    uint32 is_ilkn;
    uint32 quad_ilkn = 0;
    uint32 current;
    soc_port_if_t interface_type;
    soc_port_t local_port_internal;
    soc_pbmp_t quad_bmp;
    _bcm_dpp_gport_info_t   gport_info;
    int is_ilkn_over_fabric;

    BCMDNX_INIT_FUNC_DEFS;

    /* check input */
    BCMDNX_NULL_CHECK(priority);
    BCMDNX_NULL_CHECK(affected_ports);

    BCMDNX_IF_ERR_EXIT( _bcm_dpp_gport_to_phy_port(unit, local_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port_internal = gport_info.local_port;

    /* clean affected ports */
    BCM_PBMP_CLEAR(*affected_ports);

    /* get core */
    BCMDNX_IF_ERR_EXIT( soc_port_sw_db_core_get(unit, local_port_internal, &core));

    /* check if ILKN */
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, local_port_internal, &interface_type));
    is_ilkn = interface_type == SOC_PORT_IF_ILKN ? 1 : 0;
    if (is_ilkn) {
        /* get ILKN interface number (0-5) */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, local_port_internal, 0, &quad_ilkn));
    } else {
        /* get Quad number (0-17) */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_quad_get,(unit,local_port_internal,&quad_bmp, &is_ilkn_over_fabric)));
        BCM_PBMP_ITER(quad_bmp, current){
            quad_ilkn = current;
            break;
        }
    }

    /* mark in affected_ports all other logical ports in Quad or all other channels in ILKN which are affected by this priority configuration*/
    if (is_ilkn) {
        BCMDNX_IF_ERR_EXIT( soc_port_sw_db_ports_to_same_interface_get(unit, local_port_internal, affected_ports));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_ports_to_same_quad_get,(unit, local_port_internal, affected_ports)));
    }

    /* call mbcm function to do the work - set the priority according to prio level - should also get the quad/ILKN number, and should also get if ILKN and allow_tdm*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_nif_priority_set,(unit,core,quad_ilkn,is_ilkn,flags,allow_tdm,priority->priority_level)));

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_port_nif_priority_set(  int                       unit,
                                      bcm_gport_t               local_port,
                                      uint32                    flags,
                                      bcm_port_nif_prio_t*      priority,
                                      bcm_pbmp_t*               affected_ports )
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_nif_priority_set(unit, local_port, flags, 0, priority, affected_ports));

exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_port_nif_priority_get(  int                       unit,
                                      bcm_gport_t               local_port,
                                      uint32                    flags,
                                      bcm_port_nif_prio_t*      priority,
                                      bcm_pbmp_t*               affected_ports )
{
    int core;
    uint32 is_ilkn;
    uint32 allow_tdm = 0;
    uint32 quad_ilkn = 0;
    uint32 current;
    soc_port_if_t interface_type;
    soc_port_t local_port_internal;
    soc_pbmp_t quad_bmp;
    _bcm_dpp_gport_info_t   gport_info;
    int is_ilkn_over_fabric;

    BCMDNX_INIT_FUNC_DEFS;

    /* check input */
    BCMDNX_NULL_CHECK(priority);
    BCMDNX_NULL_CHECK(affected_ports);

    BCMDNX_IF_ERR_EXIT( _bcm_dpp_gport_to_phy_port(unit, local_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port_internal = gport_info.local_port;

    /* clean affected ports */
    BCM_PBMP_CLEAR(*affected_ports);

    /* get core */
    BCMDNX_IF_ERR_EXIT( soc_port_sw_db_core_get(unit, local_port_internal, &core));

    /* check if port is TDM and set allow_tdm accordingly */
    allow_tdm = (IS_TDM_PORT(unit,local_port_internal)) ? 1 : 0 ;

    /* check if ILKN */
    BCMDNX_IF_ERR_EXIT( soc_port_sw_db_interface_type_get(unit, local_port_internal, &interface_type));
    is_ilkn = interface_type == SOC_PORT_IF_ILKN ? 1 : 0;
    if (is_ilkn) {
        /* get ILKN interface number */
        BCMDNX_IF_ERR_EXIT( soc_port_sw_db_protocol_offset_get(unit, local_port_internal, 0, &quad_ilkn));
        
    } else {
        /* get Quad number */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_quad_get,(unit,local_port_internal,&quad_bmp, &is_ilkn_over_fabric)));
        BCM_PBMP_ITER(quad_bmp, current){
            quad_ilkn = current;
            break;
        }    }

    /* mark in affected_ports all other logical ports in Quad or all other channels in ILKN which are affected by this priority configuration*/
    if (is_ilkn) {
        BCMDNX_IF_ERR_EXIT( soc_port_sw_db_ports_to_same_interface_get(unit, local_port_internal, affected_ports));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_ports_to_same_quad_get,(unit, local_port_internal, affected_ports)));
    }

    /* call mbcm function to do the work - get the priority - should also get the quad/ILKN number, and should also get if ILKN and allow_tdm*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_nif_priority_get,(unit,core,quad_ilkn,is_ilkn,flags,allow_tdm,&(priority->priority_level))));

exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/
/************           PORT PP APIs           *****************/
/***************************************************************/
/***************************************************************/

/* fix tag format when the outer tag is priority */
STATIC int 
bcm_petra_tpid_profile_priority_fix(
      int unit, 
      SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT tag_format, 
      SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT *fixed_tag_format,
      SOC_SAND_PP_VLAN_TAG_TYPE              *priority_tag_type
    )
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(fixed_tag_format);
    BCMDNX_NULL_CHECK(priority_tag_type);

    switch(tag_format) {
        case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG:
        case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG:
            *fixed_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG;
        break;
        case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG:
        case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG:
            *fixed_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG;
        break;
        case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG:
        case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG:
            *fixed_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG;
        break;
        default:
            *fixed_tag_format = tag_format;
    }

    switch(tag_format) {
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG:
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG:
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG:
        *priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_CTAG;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG:
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG:
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG:
        *priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_STAG;
        break;
    default:
        *priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_STAG;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_tpid_profile_to_tag_format(int unit, 
                                          _bcm_petra_tpid_profile_t profile_type, 
                                          SOC_PPC_LLP_PARSE_TPID_INDEX outer_tpid_index, 
                                          SOC_PPC_LLP_PARSE_TPID_INDEX inner_tpid_index, 
                                          SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT *tag_format, int *is_double_tagged) {

    BCMDNX_INIT_FUNC_DEFS;

    *is_double_tagged = 0; 
    switch (profile_type){
    case _bcm_petra_tpid_profile_none: /* no tags */
        *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    break;
    case _bcm_petra_tpid_profile_outer: /* one outer */
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) {
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else{
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
    break;
    case _bcm_petra_tpid_profile_outer_inner:
        /* outer/inner */
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1 &&
            inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {
            /* double tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            /* set lookup for this porofile to be PVV*/
            *is_double_tagged = 1;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) {
            /* one-s-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else{ /* rest untagged, or find tpid2 as outer */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
    break;
    case _bcm_petra_tpid_profile_outer_inner_c_tag: /* outer/inner - different is that in case TPID2 is alone act as C_TAG */
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1 && inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) { /* double tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            /* set lookup for this porofile to be PVV*/
            *is_double_tagged = 1;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) {  /* one-s-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {  /* one-c-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
        }
        else{ /* rest untagged, or find tpid2 as outer */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
    break;
    case _bcm_petra_tpid_profile_outer_inner_same: /* outer/inner equal*/
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1 && inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) { /* double tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            /* set lookup for this porofile to be PVV*/
            *is_double_tagged = 1;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) {  /* one-s-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else{ /* rest untagged, or find tpid2 as outer */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
    break;
    case _bcm_petra_tpid_profile_outer_inner_same2: /* outer/inner equal*/
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2 && inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) { /* double tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            /* set lookup for this porofile to be PVV*/
            *is_double_tagged = 1;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {  /* one-s-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else{ /* rest untagged, or find tpid2 as outer */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
    break;
        case _bcm_petra_tpid_profile_outer_inner2: /* outer/inner2 (inner can show alone (as outer)) */
            if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1 && inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) { /* double tag */
                *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
                /* set lookup for this porofile to be PVV*/
                *is_double_tagged = 1;
            }
            else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) {  /* one-s-tag */
                *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
            }
            else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {  /* one-c-tag */
                *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
            }
            else{ /* rest untagged, or find tpid2 as outer */
                *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
            }
        break;
    case _bcm_petra_tpid_profile_inner_outer2: /* inner/outer2 (inner can show alone (as outer)) */
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2 && inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) { /* double tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            /* set lookup for this porofile to be PVV*/
            *is_double_tagged = 1;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {  /* one-s-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) {  /* one-c-tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        }
        else{ /* rest untagged, or find tpid2 as outer */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
    break;

    case _bcm_petra_tpid_profile_outer_outer: /* outer/outer  */
            if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1 || outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) { /* any of the tags show */
                *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
            }
            else{ /* rest untagged*/
                *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
            }
        break;
    case _bcm_petra_tpid_profile_outer_c_tag:
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_NONE) {   /* untagged */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }
        else{   /* c-tagged */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
        }

        break;
    case _bcm_petra_tpid_profile_outer_trill_fgl: /* In case TPID1 appear then S-Tag , in case TPID2 appear , must be outer and inner to be TPID2 */
        if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) { /* Single tag */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;                        
        }
        else if(outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2 && inner_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {  /* both TPIDs are FGL */
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            *is_double_tagged = 1;
        }
        else if (outer_tpid_index == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) {
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG; /* Single tag */
        }
        else{ /* rest untagged*/
            *tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        }

        break;

    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("profile_type %d not supported"), profile_type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* From QAX:
   Init the LLVP table for native egress vlan editing:
   LLVP profile=7 is dedicated to native egress vlan editing.
   */
int _bcm_petra_llvp_table_native_llvp_entries_init(int unit) {

    uint32 soc_sand_rv;

    int llvp_profile = 7; /* LLVP profile is dedicated to native egress vlan editing */
    SOC_PPC_LLP_PARSE_INFO prs_nfo;
    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO pkt_frmt_info_eg;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_LLP_PARSE_INFO_clear(&prs_nfo);

    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(&pkt_frmt_info_eg); 


    for (prs_nfo.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE; prs_nfo.outer_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS; ++prs_nfo.outer_tpid) {
        for(prs_nfo.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE; prs_nfo.inner_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS; ++prs_nfo.inner_tpid) {
            for (prs_nfo.is_outer_prio = FALSE; prs_nfo.is_outer_prio <= TRUE; ++prs_nfo.is_outer_prio) {

                if((prs_nfo.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) &&
                   ((prs_nfo.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) ||
                    (prs_nfo.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1))) {
                    /* May be S_C or S_S*/
                    pkt_frmt_info_eg.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
                } else if((prs_nfo.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) &&
                   ((prs_nfo.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2) ||
                    (prs_nfo.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1))) {
                    /* May be C_C or C_S*/
                    pkt_frmt_info_eg.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
                } else if((prs_nfo.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1) ||
                          (prs_nfo.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2)) {
                    /* May be C or S*/
                    pkt_frmt_info_eg.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
                } else if((prs_nfo.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_NONE) &&
                          ((prs_nfo.inner_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_NONE) ||
                           prs_nfo.is_outer_prio)) {
                    /* Invalid combination (outer is none, and inner exists), skip it*/
                    continue;
                } else {
                    pkt_frmt_info_eg.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
                }

                /* update egress LLVP table */
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                        mbcm_pp_llp_parse_packet_format_eg_info_set,
                                        (unit, llvp_profile, &prs_nfo, &pkt_frmt_info_eg));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_tpid_profile_info_set(int unit,
                int port_profile, int port_profile_eg,
                _bcm_petra_tpid_profile_t profile_type, int accept_mode,
                _bcm_petra_dtag_mode_t dtag_mode,
                _bcm_petra_ac_key_map_type_t ac_key_map_type,
                uint8 cep_port, uint8 evb_port)
{

    int is_mim_mode = 0, is_tagged, is_double_tagged=0, drop_prio = 0;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LLP_PARSE_INFO prs_nfo;
    SOC_PPC_L2_LIF_AC_KEY_QUALIFIER ac_key_qual;
    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO pkt_frmt_info;
    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO pkt_frmt_info_eg;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_clear(&ac_key_qual);
    SOC_PPC_LLP_PARSE_INFO_clear(&prs_nfo);

    /* check if this is MiM mode */
    if (_BCM_DPP_PORT_DISCARD_MODE_IS_MIM(accept_mode)) {
        is_mim_mode = 1;
        _BCM_DPP_PORT_DISCARD_MODE_MIM_REMOVE(accept_mode);
    }

    /* check if this is outer prio drop mode */
    if (_BCM_DPP_PORT_DISCARD_MODE_IS_OUTER_PRIO(accept_mode)) {
        drop_prio = 1;
        _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_REMOVE(accept_mode);
    }

    ac_key_qual.port_profile = port_profile;
    for(prs_nfo.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
        prs_nfo.outer_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
        ++prs_nfo.outer_tpid) {
        for(prs_nfo.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
            prs_nfo.inner_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
            ++prs_nfo.inner_tpid) {
            for (prs_nfo.is_outer_prio = FALSE; prs_nfo.is_outer_prio <= TRUE;
                 ++prs_nfo.is_outer_prio) {

                if(prs_nfo.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_NONE &&
                   (prs_nfo.inner_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_NONE||
                    prs_nfo.is_outer_prio)) {
                    /* invalid combination(outer is none, inner exists), skip it*/
                    continue;
                }
                is_double_tagged = 0;
                ac_key_qual.pkt_parse_info.is_outer_prio = prs_nfo.is_outer_prio;
                ac_key_qual.pkt_parse_info.outer_tpid = prs_nfo.outer_tpid;
                ac_key_qual.pkt_parse_info.inner_tpid = prs_nfo.inner_tpid;

                soc_sand_rv = soc_ppd_llp_parse_packet_format_info_get(
                                                  soc_sand_dev_id, port_profile,
                                                  &prs_nfo, &pkt_frmt_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_llp_parse_packet_format_eg_info_get,
                                    (soc_sand_dev_id, port_profile_eg,
                                     &prs_nfo, &pkt_frmt_info_eg));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* set tag format according to profile type and found TPIDs */
                /* packet with I-tag is not considered tagged */ 
                BCMDNX_IF_ERR_EXIT(_bcm_petra_tpid_profile_to_tag_format(unit,
                                        profile_type,
                                        prs_nfo.outer_tpid,
                                        prs_nfo.inner_tpid,
                                        &(pkt_frmt_info.tag_format),
                                        &is_double_tagged));

                /* accept mode */
                is_tagged = !(pkt_frmt_info.tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE);

                /* if this is CEP port (customer port, identify only C-tag) */
                if( cep_port &&
                    profile_type == _bcm_petra_tpid_profile_outer_c_tag &&
                    (is_tagged || is_double_tagged)) {
                    /* considered as single tagged*/
                    is_double_tagged = 0;
                    /* tag format is C-tag */
                    pkt_frmt_info.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
                }

                if (evb_port &&
                    profile_type == _bcm_petra_tpid_profile_outer_inner_c_tag &&
                    (pkt_frmt_info.tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG)) {
                    pkt_frmt_info.initial_c_tag = TRUE;
                }

                switch (accept_mode) {
                    case BCM_PORT_DISCARD_ALL:
                        pkt_frmt_info.action_trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
                    break;
                    case BCM_PORT_DISCARD_NONE:
                        pkt_frmt_info.action_trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
                    break;
                    case BCM_PORT_DISCARD_UNTAG:
                        pkt_frmt_info.action_trap_code = (is_tagged) ?\
                                SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT:\
                                SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
                    break;
                    case BCM_PORT_DISCARD_TAG:
                        pkt_frmt_info.action_trap_code = (!is_tagged) ?\
                                SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT:\
                                SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
                    break;
                    default:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                (_BSL_BCM_MSG("accept_mode %d not supported"),
                                accept_mode));
                }

                if(drop_prio && prs_nfo.is_outer_prio) {
                    pkt_frmt_info.action_trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
                }

                if (is_mim_mode &&
                    prs_nfo.outer_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_ISID_TPID &&
                    prs_nfo.inner_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_ISID_TPID) {
                    /* drop all packets without I-Tag */
                    pkt_frmt_info.action_trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
                }

                /* fix tag format for priority */
                if(prs_nfo.is_outer_prio) {
                    rv = bcm_petra_tpid_profile_priority_fix(unit,
                                             pkt_frmt_info.tag_format,
                                             &pkt_frmt_info.tag_format,
                                             &pkt_frmt_info.priority_tag_type);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                soc_sand_rv = soc_ppd_llp_parse_packet_format_info_set(
                                                soc_sand_dev_id, port_profile,
                                                &prs_nfo, &pkt_frmt_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                pkt_frmt_info_eg.tag_format = pkt_frmt_info.tag_format;
                pkt_frmt_info_eg.priority_tag_type = pkt_frmt_info.priority_tag_type;

                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_llp_parse_packet_format_eg_info_set,
                                    (soc_sand_dev_id, port_profile_eg, &prs_nfo,
                                     &pkt_frmt_info_eg));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                BCM_DPP_UNIT_CHECK(unit);
           }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_port_is_evb_port(
       int unit, bcm_port_t port, uint8 *evb_port)
{
    bcm_error_t rv;
    int evb_value;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_CONFIG(unit)->pp.evb_enable) {
        *evb_port = FALSE;
        BCM_EXIT;
    }

    /* 
     * Is EVB port
     */ 
    rv = bcm_petra_port_control_get(unit, port, bcmPortControlEvbType, &evb_value);
    BCMDNX_IF_ERR_EXIT(rv);

    *evb_port = (evb_value == bcmPortEvbTypeUplinkAccess) ? TRUE:FALSE;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_port_discard_extend_dtag_mode_get(
    int unit,
    bcm_port_t port,
    _bcm_petra_dtag_mode_t *dtag_mode)
{
   int rv = BCM_E_NONE;
   int accept_mode, core;
   _bcm_petra_tpid_profile_t profile_type;
   _bcm_petra_dtag_mode_t temp_dtag_mode;
   _bcm_petra_ac_key_map_type_t ac_key_map_type;
   SOC_PPC_PORT soc_ppd_port_i;
   _bcm_dpp_gport_info_t   gport_info;

   BCMDNX_INIT_FUNC_DEFS;

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);
   
   rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port_i, &core));
   BCMDNX_IF_ERR_EXIT(rv);

   /* get old data (profile_type) */
   rv =  _bcm_dpp_am_template_tpid_profile_data_get(unit, soc_ppd_port_i, core, &profile_type, &accept_mode, &temp_dtag_mode, &ac_key_map_type);
   BCMDNX_IF_ERR_EXIT(rv);
  
   *dtag_mode = temp_dtag_mode;

exit:
   BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_port_discard_extend_set(
    int unit, 
    SOC_PPC_PORT soc_ppd_port_i, 
    _bcm_petra_tpid_profile_t profile_type,
    int accept_mode,
    _bcm_petra_dtag_mode_t dtag_mode,
    _bcm_petra_ac_key_map_type_t ac_key_map_type)
{
    int soc_sand_dev_id = unit, soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT_INFO port_info;
    int last_appear,old_tpid_profile,new_tpid_profile, first_appear;
    uint8 evb_port;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* API not supported for parsing ADVANCED mode, other API should be used in stead */
    _BCM_DPP_TPID_PARSE_ADVANCED_MODE_API_UNAVAIL(unit);

    SOC_PPC_PORT_INFO_clear(&port_info);

    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, soc_ppd_port_i, &pp_port, &core));
    BCMDNX_IF_ERR_EXIT(rv);

    if (accept_mode > _BCM_DPP_PORT_DISCARD_MODE_MAX) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("accept_mode %d is above max value %d"),accept_mode,_BCM_DPP_PORT_DISCARD_MODE_MAX-1));
    }

    rv = _bcm_dpp_port_is_evb_port(unit, soc_ppd_port_i, &evb_port);
    BCMDNX_IF_ERR_EXIT(rv);
   
    /* Remove old, and add new mode */
    rv = _bcm_dpp_am_template_tpid_profile_exchange(unit, pp_port, core, profile_type, accept_mode, dtag_mode, ac_key_map_type, &old_tpid_profile, &last_appear, &new_tpid_profile, &first_appear);
    BCMDNX_IF_ERR_EXIT(rv);


    if (first_appear) {
      /* check if this cep port */
      soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id,core,pp_port,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      rv = bcm_petra_tpid_profile_info_set(unit, new_tpid_profile, new_tpid_profile,profile_type, accept_mode, dtag_mode, ac_key_map_type, port_info.port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP, evb_port);
      BCMDNX_IF_ERR_EXIT(rv);
    }

    if (old_tpid_profile != new_tpid_profile) {
      /* update port profile */
      soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id,core, pp_port,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      port_info.port_profile = new_tpid_profile;
      port_info.port_profile_eg = new_tpid_profile;
      port_info.flags = (SOC_PPC_PORT_IHP_PINFO_LLR_TBL | SOC_PPC_PORT_EPNI_PP_PCT_TBL);
      soc_sand_rv = soc_ppd_port_info_set(soc_sand_dev_id,core,pp_port,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
   BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_port_discard_extend_dtag_mode_set(
    int unit,
    bcm_port_t port,
    _bcm_petra_dtag_mode_t dtag_mode)
{
   int port_i;
   int rv = BCM_E_NONE;
   int accept_mode, core;
   _bcm_petra_tpid_profile_t profile_type;
   _bcm_petra_dtag_mode_t temp_dtag_mode;
   _bcm_petra_ac_key_map_type_t ac_key_map_type;
   SOC_PPC_PORT soc_ppd_port_i;
   _bcm_dpp_gport_info_t   gport_info;
   BCMDNX_INIT_FUNC_DEFS;

   if (dtag_mode >= _bcm_petra_dtag_mode_count) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("dtag_mode %d is above max value %d"),dtag_mode,_bcm_petra_dtag_mode_count));
   }

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core));
        BCMDNX_IF_ERR_EXIT(rv);

        /* get old data (profile_type) */
        rv =  _bcm_dpp_am_template_tpid_profile_data_get(unit, soc_ppd_port_i, core, &profile_type, &accept_mode, &temp_dtag_mode, &ac_key_map_type);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* Exchange in case of update and commit */
        rv = _bcm_petra_port_discard_extend_set(unit, port_i, profile_type, accept_mode, dtag_mode, ac_key_map_type);
        BCMDNX_IF_ERR_EXIT(rv);
   }

exit:
   BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_port_discard_extend_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode)
{
   int port_i;
   int rv = BCM_E_NONE;
   int accept_mode, core;
   _bcm_petra_tpid_profile_t profile_type;
   _bcm_petra_dtag_mode_t dtag_mode;   
   _bcm_petra_ac_key_map_type_t ac_key_map_type;
   SOC_PPC_PORT soc_ppd_port_i;
   int is_mim_port = 0;
   _bcm_dpp_gport_info_t   gport_info;
   uint32 flags;
   BCMDNX_INIT_FUNC_DEFS;

   if (mode > _BCM_DPP_PORT_DISCARD_MODE_MAX) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mode %d is above max value %d"),mode,_BCM_DPP_PORT_DISCARD_MODE_MAX-1));
   }

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
        if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))
        {
            continue;
        }
        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core));
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_port_control_get(unit, port_i, bcmPortControlMacInMac, &is_mim_port);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * Special case for MIM: 
         * Set MIM mode.
         * For MIM ports, discard tag mode means discard all packets that do not contain I-TPID on the packet.
         */
        if (is_mim_port && mode == BCM_PORT_DISCARD_TAG) {
            mode = BCM_PORT_DISCARD_NONE;
            /* set the MiM bit, to change the mode into MiM mode. */
            _BCM_DPP_PORT_DISCARD_MODE_MIM_SET(mode);
        }

        /* get old data (profile_type) */
        rv =  _bcm_dpp_am_template_tpid_profile_data_get(unit, soc_ppd_port_i, core, &profile_type, &accept_mode, &dtag_mode, &ac_key_map_type);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* Exchange in case of update and commit */
        rv = _bcm_petra_port_discard_extend_set(unit, port_i, profile_type, mode, dtag_mode, ac_key_map_type);
        BCMDNX_IF_ERR_EXIT(rv);
   }

exit:
   BCMDNX_FUNC_RETURN;
}

/* Set the inlif profile according to kind. */
/* When ARAD_PP_L2_LIF_PROFILE_TYPE_USER is given the basic/advanced mode is auto detected. */
STATIC int _bcm_petra_port_inlif_profile_set(
   const int unit, 
   const bcm_port_t port, 
   const uint32 arg, 
   const SOC_OCC_MGMT_INLIF_APP kind,

   /* Allocated lif_info */
   SOC_PPC_LIF_ENTRY_INFO *lif_info)
{
  int rv = BCM_E_NONE;  
  uint32 * mapped_val_p = NULL;  
  uint32 soc_sand_rv;
  _bcm_dpp_gport_hw_resources gport_hw_resources;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);


  /* See if LIF */
  rv = _bcm_dpp_gport_to_hw_resources(unit,
                             port,
                             _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                             &gport_hw_resources);
  if ((BCM_E_NONE == rv) && (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
    /* LIF found - update the entry */

    /* Get the info */
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /*Combine the unmasked part with the new masked part */
    switch (lif_info->type) {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
        mapped_val_p = &lif_info->value.ac.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
        mapped_val_p = &lif_info->value.pwe.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
        mapped_val_p = &lif_info->value.ip_term_info.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
        mapped_val_p = &lif_info->value.mpls_term_info.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
        mapped_val_p = &lif_info->value.extender.lif_profile;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Erro: No InLif profile for given lif type")));
    }

    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, (unit, SOC_OCC_MGMT_TYPE_INLIF, kind, arg, mapped_val_p));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    /* Update the entry */
    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  }

exit:
  BCMDNX_FUNC_RETURN;
}

/* 
*  
* set outlif profile for specific local outlif.
 */
int _bcm_petra_outlif_profile_set(
   const int unit, 
   const uint32 local_outlif_id, 
   const uint32 arg, 
   const SOC_OCC_MGMT_OUTLIF_APP kind)
{
    uint32 out_lif_profile = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /*Read the Out-LIF-profile from HW*/
    BCM_SAND_IF_ERR_EXIT(
        soc_ppd_eg_encap_lif_field_get(unit, local_outlif_id,
                            SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE,
                            &out_lif_profile));

    /*Create new Out-LIF-profile with mtu profile (set the needed bits)*/
    BCMDNX_IF_ERR_EXIT(
        MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, (unit,
                            SOC_OCC_MGMT_TYPE_OUTLIF, kind, arg,
                            &out_lif_profile)));

    /*Write the Out-LIF-profile back to HW*/
    BCM_SAND_IF_ERR_EXIT(
        soc_ppd_eg_encap_lif_field_set(unit,local_outlif_id,
                            SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE,
                            out_lif_profile));

exit:
  BCMDNX_FUNC_RETURN; 
}

/*
*
* set outlif profile for for PWE port.
 */ 
int _bcm_petra_port_outlif_profile_set(
   const int unit, 
   const bcm_port_t port, 
   const uint32 arg, 
   const SOC_OCC_MGMT_OUTLIF_APP kind)
{
    int local_outlif_id;

    BCMDNX_INIT_FUNC_DEFS;

    /*Convert gport to local LIF*/
    BCMDNX_IF_ERR_EXIT(
        _bcm_dpp_gport_to_global_and_local_lif(unit, port, NULL, NULL,
                                               &local_outlif_id, NULL,NULL));

    BCMDNX_IF_ERR_EXIT(
        _bcm_petra_outlif_profile_set(unit, local_outlif_id, arg, kind));

exit:
  BCMDNX_FUNC_RETURN; 
}

/**
 * Eitehr gets or sets the outlif profile, depending on 
 * get_1_set_0. 
 * Additionally may optionally set the oam-lif-valid bit (set to
 * -1 to disable this). Should only be called by 
 * _bcm_port_outlif_profile_by_lif_get/set() and related 
 * functions. 
 *  
 * 
 * @author sinai (02/06/2016)
 * 
 * @param unit 
 * @param out_gport 
 * @param outlif_profile 
 * @param get_1_set_0 
 * @param set_oam_lif_valid 
 * 
 * @return int 
 */
static int _bcm_port_outlif_profile_by_lif_internal(int unit, bcm_gport_t out_gport, uint32 * outlif_profile, int get_1_set_0, int set_oam_lif_valid) {
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries = 0; 
    int rv;
    BCMDNX_INIT_FUNC_DEFS;


    rv = _bcm_dpp_gport_to_hw_resources(unit, out_gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "_bcm_port_outlif_profile_by_lif_internal.encap_entry_info");
    if (encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP, gport_hw_resources.local_out_lif, 1 /* depth */ , 
                                             encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (encap_entry_info->entry_type) {
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP:
        if (get_1_set_0) {
            *outlif_profile = encap_entry_info->entry_val.mpls_encap_info.outlif_profile;
        } else {
            /* add encap entry, set outlif profile additional encapsulation bit */
            if (outlif_profile) {
                encap_entry_info->entry_val.mpls_encap_info.outlif_profile = *outlif_profile; 
            }
            if (set_oam_lif_valid!=-1) {
                encap_entry_info->entry_val.mpls_encap_info.oam_lif_set = set_oam_lif_valid;
            }
            soc_sand_rv = soc_ppd_eg_encap_mpls_entry_add(unit, gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.mpls_encap_info), next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }
        break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC:
        if (get_1_set_0) {
            *outlif_profile = encap_entry_info->entry_val.out_ac.edit_info.lif_profile;
        } else {
            if (outlif_profile) {
                encap_entry_info->entry_val.out_ac.edit_info.lif_profile = *outlif_profile; 
            }
            if (set_oam_lif_valid!=-1) {
                encap_entry_info->entry_val.out_ac.edit_info.oam_lif_set=set_oam_lif_valid;
            }
            soc_sand_rv = soc_ppd_eg_ac_info_set(unit,gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.out_ac));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE:
        if (get_1_set_0) {
            *outlif_profile = encap_entry_info->entry_val.pwe_info.outlif_profile;
        } else {
            if (set_oam_lif_valid !=-1) {
                encap_entry_info->entry_val.pwe_info.oam_lif_set = 1;
            }
            if (outlif_profile) {
                encap_entry_info->entry_val.pwe_info.outlif_profile = *outlif_profile; 
            }
            soc_sand_rv = soc_ppd_eg_encap_pwe_entry_add(unit, gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.pwe_info), next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: function only for PWE, MPLS and AC.")));
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

/**
 * Given the outlif (gport) returns the outlif-profile.
 * 
 * @author sinai (02/06/2016)
 * 
 * @param unit 
 * @param out_gport 
 * @param outlif_profile 
 * 
 * @return int 
 */
static int _bcm_port_outlif_profile_by_lif_get(int unit, bcm_gport_t out_gport, uint32 * outlif_profile) {
    return _bcm_port_outlif_profile_by_lif_internal(unit,out_gport,outlif_profile,1 /* get */, -1);
}

/**
 * Given an outlif (gport) and an outlif-profile sets the outlif 
 * profile on the outlif. 
 * May optionally set the OAM-LIF-Valid bit as well. 
 *  
 * @author sinai (02/06/2016)
 * 
 * @param unit 
 * @param out_gport 
 * @param outlif_profile 
 * @param set_oam_lif_valid 
 * 
 * @return int 
 */
static int _bcm_port_outlif_profile_by_lif_set(int unit, bcm_gport_t out_gport, uint32  outlif_profile, int set_oam_lif_valid) {
    uint32 temp_outlif_profile = outlif_profile;
    return _bcm_port_outlif_profile_by_lif_internal(unit,out_gport,&temp_outlif_profile,0 /* set */, set_oam_lif_valid /* set-oam-lif*/);
}


/**
 * Set OAM-LIF-valid
 * 
 * @author sinai (06/06/2016)
 * 
 * @param unit 
 * @param out_gport 
 * 
 * @return int 
 */
int bcm_port_outlif_set_oam_lif_valid(int unit, bcm_gport_t out_gport, int oam_lif_valid) {
    return _bcm_port_outlif_profile_by_lif_internal(unit,out_gport,NULL,1 /* set */, oam_lif_valid /* set-oam-lif*/);
}


/* Get the (part of) inlif profile according to kind. */
/* When ARAD_PP_L2_LIF_PROFILE_TYPE_USER is given the basic/advanced mode is auto detected. */
/* The value of the type is returned in arg. */
/* port must be a lif. */
/* Use all_kinds=TRUE to get the 'raw' inlif profile (all bits value regardless of application)*/
STATIC int _bcm_petra_port_inlif_profile_get(
   const int unit, 
   const bcm_port_t port, 
   uint32 *arg, 
   const SOC_OCC_MGMT_INLIF_APP kind,
   const uint8 all_kinds,

   /* Allocated lif_info */
   SOC_PPC_LIF_ENTRY_INFO *lif_info)
{
  int rv = BCM_E_NONE;   
  uint32 mapped_val;  
  uint32 soc_sand_rv;
  _bcm_dpp_gport_hw_resources gport_hw_resources;
  
  BCMDNX_INIT_FUNC_DEFS;

  /* See if LIF */
  rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                      &gport_hw_resources);  
  if ((BCM_E_NONE == rv) && (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
    /* LIF found - update the entry */

    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    mapped_val = 0;
    switch (lif_info->type) {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
        mapped_val = lif_info->value.ac.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
        mapped_val = lif_info->value.pwe.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
        mapped_val = lif_info->value.ip_term_info.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
        mapped_val = lif_info->value.mpls_term_info.lif_profile;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
        mapped_val = lif_info->value.extender.lif_profile;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Erro: No InLif profile for given lif type")));
    }

    if (!all_kinds) {
        soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get, (unit, SOC_OCC_MGMT_TYPE_INLIF, kind,&mapped_val, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else {
        *arg = mapped_val;
    }

  }

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_pp_port_control_set(int unit, bcm_port_t port,
                           bcm_port_control_t type, int value)
{
    int rv = BCM_E_NONE, core = 0;
    uint32    soc_sand_rv;    
    bcm_port_t port_ndx = 0;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_PORT_INFO  port_info;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    SOC_SAND_SUCCESS_FAILURE success = SOC_SAND_FAILURE_UNKNOWN_ERR;
    SOC_PPC_EG_FILTER_PORT_INFO eg_port_info;
    SOC_PPC_LLP_SA_AUTH_PORT_INFO sa_auth_port_info;
    _bcm_petra_port_mact_mgmt_action_profile_t action_profile;
    int old_template = 0, new_template = 0;
    int is_last = 0, is_new = 0 ;
    SOC_PPC_FRWRD_MACT_PORT_INFO frwrd_mact_port_info;
    SOC_PPC_LIF_ID lif_id;
    uint8 found;
    SOC_PPC_L2_LIF_AC_INFO ac_info;
    SOC_TMC_PORT_PP_PORT_INFO pp_port_info;
    uint32 soc_tm_port = 0;
    SOC_TMC_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;    
    SOC_PPC_FP_CONTROL_INFO
       ctrl_info;
    SOC_PPC_FP_CONTROL_INDEX
       ctrl_indx;
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    _bcm_dpp_gport_info_t   gport_info;
    uint32 operations;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;

    BCMDNX_INIT_FUNC_DEFS;
 
    /* In case of vlan isolation. ask for system port */
    operations = (type == bcmPortControlPrivateVlanIsolate) ? _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT : 0;
        
    sal_memset(&gport_info, 0, sizeof(gport_info));

    if (!BCM_GPORT_IS_TUNNEL(port)) {
        /* _bcm_dpp_gport_to_phy_port does not support tunnels */
        rv = _bcm_dpp_gport_to_phy_port(unit, port, operations, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_DPP_PP_ENABLE(unit)) {

        switch (type)
        {
          /* controls can be only on one*/
          case bcmPortControlMpls:
          case bcmPortControlIP4:
          case bcmPortControlIP6:
              if(value != 1) {
                  rv = BCM_E_UNAVAIL;
              }
          break;
          /* controls can be only on zero */
          case bcmPortControlDoNotCheckVlan:
              if(value != 0) {
                  rv = BCM_E_UNAVAIL;
              }
          break;
          case bcmPortControlFcoeFabricId:
              if(!SOC_DPP_CONFIG(unit)->pp.fcoe_enable ){
                  rv = BCM_E_UNAVAIL;
              } else {
                  if (value > BCM_DPP_MAX_FCOE_VFT_VALUE) {
                      BCM_SAND_IF_ERR_EXIT(BCM_E_PARAM);
                  }
                  soc_ppd_port = 0;
                  BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                      break;
                  }
                  SOC_PPC_FP_CONTROL_INDEX_clear(&ctrl_indx);
                  SOC_PPC_FP_CONTROL_INFO_clear(&ctrl_info);
                  ctrl_indx.val_ndx = soc_ppd_port;
                  ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA;
                  value = value << 1; /* one bit is reserved to zero in the vft header */
                  ctrl_info.val[0] = value;
                  soc_sand_rv = soc_ppd_fp_control_set(unit,core,&ctrl_indx,&ctrl_info,&failure_indication);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                  SOC_SAND_IF_FAIL_RETURN(failure_indication);
              }
          break;
         case bcmPortControlFcoeNetworkPort:
            soc_ppd_port = 0;
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                break;
            }            
            soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (port_info.is_n_port == (!value)){
                break;
            }else{
                port_info.is_n_port = (!value);

                if (soc_property_get(unit, spn_FCOE_NPV_BRIDGE_MODE, 0)){
                    uint32 tm_port, base_q_pair, prge_prog_select, out_port_priority, curr_q_pair;
                    int    core=SOC_CORE_INVALID;
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port_ndx, &tm_port, &core)));
                    soc_sand_rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_ndx, &out_port_priority));

                    if (port_info.is_n_port) {
                        BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_FCOE_N_PORT, &prge_prog_select));
                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
                    }else {
                        BCMDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, SOC_TMC_PORT_HEADER_TYPE_NONE, &prge_prog_select));
                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                    }
                    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + out_port_priority; curr_q_pair++) {
                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_prog_editor_profile_set, (unit,  core, curr_q_pair, prge_prog_select)));
                    }
                }                
                port_info.flags = (SOC_PPC_PORT_IHB_PINFO_FLP_TBL | 
                                   SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL);
                soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
           break;
        case bcmPortControlMacInMac:

              /* make sure MiM is enabled on the device */
              MIM_INIT(unit);

              if (value == 0 || value == 1) { /* disable/enable MiM */ 
                  SOC_PPC_L2_LIF_AC_KEY ac_key;
                  SOC_PPC_LIF_ID mim_local_lif_ndx; 
                  MIM_ACCESS.mim_local_lif_ndx.get(unit, &mim_local_lif_ndx); 

                  BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                      soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                      port_info.is_pbp = SOC_SAND_NUM2BOOL(value);
                      port_info.orientation = (SOC_SAND_NUM2BOOL(value)) ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
 
                      /* set default egress AC */
                      if (value == 1) { /* Enable MiM */
                          /* set out lif is default, set it to MIM default out lif */
                          if (port_info.dflt_egress_ac ==  ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID)) {
                              /* point by default to the mim_out_ac */
                              port_info.dflt_egress_ac = __dpp_mim_global_out_ac_get(unit);                          
                          }
                      }
                      else { /* Disable MiM */
                          /* set out lif is default, set it to L2 default out lif */
                          if (port_info.dflt_egress_ac == __dpp_mim_global_out_ac_get(unit)) {                              
                              port_info.dflt_egress_ac =  ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID);   
                          }
                      }
                      port_info.flags = (SOC_PPC_PORT_IHB_PINFO_FLP_TBL |
                                         SOC_PPC_PORT_EPNI_PP_PCT_TBL |
                                         SOC_PPC_PORT_EGQ_PP_PPCT_TBL);
                      soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                      /* prepare ac_key for lif entry */
                      SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);
                      ac_key.key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT;
                      ac_key.vlan_domain = soc_ppd_port; /* key value is the port_id */
                      ac_key.core_id = core;
                      /* Get LIF ID */
                      SOC_PPC_L2_LIF_AC_INFO_clear(&ac_info);
                      soc_sand_rv = soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                      if (value == 1) { /* enable MiM */
                          SOC_PPC_LIF_ID local_lif_index_simple;
                          sw_state_access[unit].dpp.bcm.vlan.vlan_info.local_lif_index_simple.get(unit, &local_lif_index_simple); 

                          /* Assumption is that if the user has set a default LIF different from the default one in SOC properties,
                             they know what they're doing. In that case, don't change the default LIF for the port. */
                          if (lif_id == local_lif_index_simple)
                          {                              
                              { /* ARAD VSI assignment mode */
                                  /* B-VSI = B-VID + 4 upper bits set (base) */
                                  ac_info.vsid                        = _BCM_DPP_VLAN_TO_BVID(unit, 0);
                                  ac_info.vsi_assignment_mode         = SOC_PPC_VSI_EQ_IN_VID_PLUS_VSI_BASE;
                              }

                              ac_info.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP;
                              ac_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
                              ac_info.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
                              ac_info.default_frwrd.default_forwarding.dest_id = soc_ppd_port;
                              ac_info.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
                              ac_info.learn_record.learn_info.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
                              ac_info.learn_record.learn_info.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_AC;
                              ac_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;

                              soc_sand_rv = soc_ppd_l2_lif_ac_add(unit, &ac_key, mim_local_lif_ndx, &ac_info, &success);
                              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                              SOC_SAND_IF_FAIL_RETURN(success);
                          }
                          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
                              SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
                              BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_get, (unit, core, soc_ppd_port,&pp_port_info)));
                              pp_port_info.flags |= SOC_MIM_SPB_PORT;
                              BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_set, (unit, core, soc_ppd_port,&pp_port_info,&success)));
                              SOC_SAND_IF_FAIL_EXIT(success);
                          }
                      }
                      else { /* disable MiM */

                          /* if the user has set a default LIF different than the default LIF in SOC properties, don't change the
                             default LIF for the port. */
                          if (lif_id == mim_local_lif_ndx) {
                              SOC_PPC_LIF_ID local_lif_index_simple;
                              sw_state_access[unit].dpp.bcm.vlan.vlan_info.local_lif_index_simple.get(unit, &local_lif_index_simple);

                              BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info"); 
                              if (lif_info == NULL) {
                                  BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
                              }
                              /* get default LIF info */
                              soc_sand_rv = soc_ppd_lif_table_entry_get(unit, local_lif_index_simple, lif_info);
                              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                              /* update existing lif entry */
                              soc_sand_rv = soc_ppd_l2_lif_ac_add(unit, &ac_key, local_lif_index_simple, &(lif_info->value.ac), &success);
                              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                              SOC_SAND_IF_FAIL_EXIT(success);
                          }
                          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
                              SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
                              BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_get, (unit, core, soc_ppd_port,&pp_port_info)));
                              pp_port_info.flags &= ~(SOC_MIM_SPB_PORT);
                              BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_set, (unit, core, soc_ppd_port,&pp_port_info,&success)));
                              SOC_SAND_IF_FAIL_EXIT(success);
                          }
                    }
                      
                    if (SOC_DPP_CONFIG(unit)->pp.test1 == 1 || SOC_DPP_CONFIG(unit)->pp.test2 || SOC_DPP_CONFIG(unit)->pp.mim_bsa_lsb_from_ssp) {

                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));
                        SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
                        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_get, (unit, core, soc_ppd_port,&pp_port_info)));
                        pp_port_info.flags |= SOC_TEST1_PORT_PP_PORT;


                        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_set, (unit, core, soc_ppd_port,&pp_port_info,&success)));
                        BCM_SAND_IF_ERR_EXIT(success);
                    }

                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("invalid value %d"), value));
            }
            break;

            /* Preserving DSCP on a per out-port bases */
        case bcmPortControlTCPriority:
            if (value == 0 || value == 1) { /* disable/enable */
                if ((SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN) {


                    soc_ppd_port = 0;
                    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));
                        break;
                    }
                    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    port_info.flags = SOC_PPC_PORT_ALL_TBL;
                    soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));
                    SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
                    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_get, (unit, core, soc_ppd_port,&pp_port_info)));
                  
                    if (value) { /*enable*/
                        pp_port_info.flags |= SOC_PRESERVING_DSCP_PORT_PP_PORT;
                    } else { /*disable */
                        pp_port_info.flags &= (~SOC_PRESERVING_DSCP_PORT_PP_PORT);
                    }

                    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_set, (unit, core, soc_ppd_port,&pp_port_info,&success)));
                    BCM_SAND_IF_ERR_EXIT(success);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmPortControlTCPriority not supported if custom_feature_preserving_dscp_enabled not set")));
                }

            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("invalid value %d"), value));
            }
            break;

#ifdef BCM_CMICM_SUPPORT
        case bcmPortControlL2Move:
            {
                if ((SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) && SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
                    if (value == 0 || value == 1) {
                        if (_BCM_DPP_GPORT_INFO_IS_LAG(gport_info)) {
                            rv = shr_llm_msg_mac_move_set(unit, gport_info.sys_port, value);
                            BCMDNX_IF_ERR_EXIT(rv);
                        } else {
                            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                                rv = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &soc_ppd_port, &core);
                                BCMDNX_IF_ERR_EXIT(rv);
                                rv = shr_llm_msg_mac_move_set(unit, soc_ppd_port, value);
                                BCMDNX_IF_ERR_EXIT(rv);
                            }
                        }
                    } else {
                        BCMDNX_ERR_EXIT_MSG(_SHR_E_PARAM, (_BSL_BCM_MSG("invalid value %d"), value));
                    }
                }
            }
            break;
#endif
        case bcmPortControlDiscardMacSaAction:
            {

                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));

                    SOC_PPC_FRWRD_MACT_PORT_INFO_clear(&frwrd_mact_port_info); 

              /* Convert flags to action profile */
              if (value == BCM_PORT_CONTROL_DISCARD_MACSA_NONE) {
                action_profile = _bcm_petra_port_mact_mgmt_action_profile_none;
              } else if (value == BCM_PORT_CONTROL_DISCARD_MACSA_TRAP) {
                action_profile = _bcm_petra_port_mact_mgmt_action_profile_trap;
              } else if (value == BCM_PORT_CONTROL_DISCARD_MACSA_DROP) {
                action_profile = _bcm_petra_port_mact_mgmt_action_profile_drop;
              } else if (value == (BCM_PORT_CONTROL_DISCARD_MACSA_DROP | BCM_PORT_CONTROL_DISCARD_MACSA_TRAP)) {
                action_profile = _bcm_petra_port_mact_mgmt_action_profile_trap_and_drop;
              } else {           
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: bcmPortControlDiscardMacSaAction failed, given incorrect value %d unit %d"), FUNCTION_NAME(), value, unit));
              }
              rv = _bcm_dpp_am_template_port_mact_sa_drop_exchange(unit,core,soc_ppd_port,&action_profile,&old_template,&is_last,&new_template,&is_new);
              BCMDNX_IF_ERR_EXIT(rv);

              if (old_template != new_template)
              {
                /* Set mapping in HW */
                soc_sand_rv = soc_ppd_frwrd_mact_port_info_get(unit,core,soc_ppd_port,&frwrd_mact_port_info);
                rv = handle_sand_result(soc_sand_rv); 
                if (rv == BCM_E_NONE) { 
                  /* All OK go on */
                  frwrd_mact_port_info.sa_drop_action_profile = new_template;
                  soc_sand_rv = soc_ppd_frwrd_mact_port_info_set(unit,core,soc_ppd_port,&frwrd_mact_port_info);
                  rv = handle_sand_result(soc_sand_rv);
                } 

                if (rv != BCM_E_NONE) {
                  /* Roll back there is a fail in the process of commit. */
                  rv = _bcm_dpp_am_template_port_mact_sa_drop_exchange(unit,core,soc_ppd_port,&action_profile,&old_template,&is_last,&new_template,&is_new);
                  BCMDNX_IF_ERR_EXIT(rv);

                  BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: sa drop failed on SW set rv %d, unit %d"),FUNCTION_NAME(), rv, unit)); /* returns BCM_E_NONE */
                }            
              }
            }
          }
          break;
        case bcmPortControlUnknownMacDaAction:
          {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported device"))); 
          }
          break;
        case bcmPortControlPrivateVlanIsolate:
          {
            SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE pvlan_type;
            SOC_SAND_PP_SYS_PORT_ID pp_sys_port;
            SOC_TMC_DEST_INFO tm_dest_info;

            if (!SOC_DPP_CONFIG(unit)->pp.pvlan_enable) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("pvlan_enable not enabled")));
            }              

            /* Convert value to pvlan type */
            if (value == 0) {           
                pvlan_type = SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY;
            } else if (value == 1){
                pvlan_type = SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED;
            } else if (value == 2){
                pvlan_type = SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("pvlan value is invalid")));
            }

            soc_sand_SAND_PP_SYS_PORT_ID_clear(&pp_sys_port);
            SOC_TMC_DEST_INFO_clear(&tm_dest_info);

            rv = _bcm_dpp_gport_to_tm_dest_info(unit,port,&tm_dest_info);
            BCMDNX_IF_ERR_EXIT(rv);

            if(tm_dest_info.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
                pp_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
                pp_sys_port.sys_id = tm_dest_info.id;
            }
            else if(tm_dest_info.type == SOC_TMC_DEST_TYPE_LAG) {
                pp_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
                pp_sys_port.sys_id = tm_dest_info.id;
            }
            else{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tm_dest_info.type %d is invalid"), tm_dest_info.type));
            }
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));
            
            soc_sand_rv = soc_ppd_eg_filter_pvlan_port_type_set(unit,&pp_sys_port,pvlan_type);
            SOC_SAND_IF_ERROR_RETURN(soc_sand_rv);
          }
          break;
        
        case bcmPortControlBridge: 
          {
            SOC_PPC_EG_FILTER_PORT_INFO eg_port_info;

            if (BCM_GPORT_IS_VLAN_PORT(port)) {
              int simple_mode = soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0);

              BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info");
              if (lif_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
              }

              if (value < 0 || value > 1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
              }

              /* User value == 0 means disable, however the bit itself signifies disable if set for soc layer.*/
              /* Flip the values. */
              value = (value == 0) ? 1 : 0;

              if (simple_mode) {

                /* Simple mode -- Enable proper bit in inlif profile. */
                /* 1 - disable, 0 - enable*/
                rv = _bcm_petra_port_inlif_profile_set(unit, port, value, SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE, lif_info);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));

              } else {

#ifdef BCM_88660_A0
                if (SOC_IS_ARADPLUS(unit)) {
                  /* Advanced mode */
                  /* Get the inlif profile and disable same if for it. */
                  uint32 mask;
                  SOC_PPC_EG_FILTER_GLOBAL_INFO info;
                  uint32 inlif_profile, profile_bmp;
                  SHR_BITDCL inlif_profile_mask = 0;

                  /* Get mask for User Defined application */
                  soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_get_app_mask, (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_APP_USER, &inlif_profile_mask)));
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  /* Get relevant bits from the mask */
                  inlif_profile_mask &= BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;

                  /* We take the user inlif profile (whichever part of the whole it is) and disable same i/f filter for it. */
                  /* This is done only for the 2 lsbs (since only those are passed to egress) */
                  rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, SOC_OCC_MGMT_APP_USER, 0/*all_kinds*/, lif_info);
                  BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));
                  inlif_profile = lif_info->value.ac.lif_profile;

                  /* Get relevant bits from the profile */
                  inlif_profile &= BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK;

                  SOC_PPC_EG_FILTER_GLOBAL_INFO_clear(&info);
                  soc_sand_rv = soc_ppd_eg_filter_global_info_get(unit, &info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  /* Loop on all the profiles and set the relevant ones according to the mask */
                  for (profile_bmp=0; profile_bmp<=BCM_PORT_INLIF_PROFILE_PASSED_TO_EGRESS_MASK; profile_bmp++) {
                      if ((profile_bmp & inlif_profile_mask) == (inlif_profile & inlif_profile_mask)) {
                          mask = (1 << profile_bmp);

                          /* 0 - Enable same i/f. */
                          if (value == 0) {
                            info.in_lif_profile_disable_same_interface_filter_bitmap &= ~mask;
                          } else {
                            info.in_lif_profile_disable_same_interface_filter_bitmap |= mask;
                          }
                      }
                  }

                  soc_sand_rv = soc_ppd_eg_filter_global_info_set(unit, &info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
#endif /* BCM_88660_A0 */

              }

            } else {
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  SOC_PPC_EG_FILTER_PORT_INFO_clear(&eg_port_info);                
                  soc_sand_rv = soc_ppd_eg_filter_port_info_get(unit, core, soc_ppd_port, &eg_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                  if (value == 1) {
                      eg_port_info.filter_mask |= SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE;
                  } else if (value == 0) {
                      eg_port_info.filter_mask &= ~SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE;
                  } else {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid value %d"),value));
                  }
                  soc_sand_rv = soc_ppd_eg_filter_port_info_set(unit, core, soc_ppd_port, &eg_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
          }
          break;
        case bcmPortControlFloodBroadcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodUnknownUcastGroup:
          {
            int is_lif_flooding = FALSE;
            int is_port_flooding = FALSE;   
            int global_lif_index = 0 ;
            bcm_multicast_t bc, mc, uc;
            _bcm_dpp_gport_parse_info_t gport_parse_info;
            bcm_gport_t logical_gport;
            
            rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
            BCMDNX_IF_ERR_EXIT(rv);

            gport_hw_resources.local_in_lif = -1;

            if (gport_parse_info.type == _bcmDppGportParseTypeSimple) {
                /* Flooding is per port */
                is_port_flooding = TRUE;      
            } else {
                /* VLAN, MPLS or Extender Port GPORT - retreive LIF index directly */
                if (BCM_GPORT_IS_VLAN_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port) ||
                    BCM_GPORT_IS_EXTENDER_PORT(port) || BCM_GPORT_IS_VXLAN_PORT(port) ||
                    BCM_GPORT_IS_TUNNEL(port)) {
                    is_lif_flooding = TRUE;

                    if (BCM_GPORT_IS_TUNNEL(port) &&
                        SOC_IS_JERICHO(unit) &&
                        soc_property_get(unit, spn_EVPN_ENABLE, 0) &&
                        (port & _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION)) {
                        /* Reset the iml_bos indicator in tunnel gport*/
                        logical_gport = port & (~_BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION);
                    } else {
                        logical_gport = port;
                    }

                    rv = _bcm_dpp_gport_to_hw_resources(unit, logical_gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                             &gport_hw_resources);
                    BCMDNX_IF_ERR_EXIT(rv);  
                }
            }

            rv = _bcm_petra_vlan_flooding_per_lif_get(unit, port, gport_hw_resources.local_in_lif, is_port_flooding,
                                 is_lif_flooding, &uc, &mc, &bc);                
            BCMDNX_IF_ERR_EXIT(rv);

            /* Offset only */
            switch (type) {
            case bcmPortControlFloodBroadcastGroup:
              bc = value;
              break;
            case bcmPortControlFloodUnknownMcastGroup:
              mc = value;
              break;
            case bcmPortControlFloodUnknownUcastGroup:
              uc = value;
              break;
            /* We mast the default - without the default - compilation error */
            /* coverity[dead_error_begin] */
            default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unexpected type")));
              break;
            }
            
             if (SOC_IS_JERICHO(unit)) {
                rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS,gport_hw_resources.local_in_lif, &global_lif_index);
                BCMDNX_IF_ERR_EXIT(rv);
            }
             else{
                 global_lif_index = gport_hw_resources.local_in_lif;
            }
            /* Set flooding */
            rv = _bcm_petra_vlan_flooding_per_lif_set(unit, port, global_lif_index, is_port_flooding,
                                 is_lif_flooding, uc, mc, bc);
            BCMDNX_IF_ERR_EXIT(rv);
          }
          break;
        case bcmPortControlTrillDesignatedVlan: 
          {
              uint8                                     accept;
              SOC_SAND_PP_VLAN_ID                       vid;
              int                                       update = 0;
              uint32 trill_disable_designated_vlan_check;
              int local_lif_index = 0;
              SOC_PPC_LIF_ID lif_index_o = 0;
              SOC_PPC_L2_LIF_AC_KEY ac_key;
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));


                  trill_disable_designated_vlan_check = soc_property_get(unit, spn_TRILL_DESIGNATED_VLAN_CHECK_DISABLE, 0);
                  if ((!trill_disable_designated_vlan_check) && ((SOC_DPP_CONFIG(unit))->trill.designated_vlan_inlif_enable)) {
                      soc_sand_rv = soc_ppd_llp_filter_designated_vlan_get(unit, core, soc_ppd_port, &vid, 
                                                                    &accept);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                      if ((vid != 0) && (vid != value)) {
                          update = 1;
                      }

                      rv = sw_state_access[unit].dpp.bcm.trill.trill_global_in_lif.get(unit, &local_lif_index);
                      BCMDNX_IF_ERR_EXIT(rv);
                      if ((local_lif_index != -1) && (vid != value)) {
                          soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                          if (update) {
                                  SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);
                              ac_key.vlan_domain = port_info.vlan_domain;
                                  ac_key.key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN;
                              ac_key.outer_vid = vid;
                              ac_key.core_id = core;
                              lif_index_o = local_lif_index;
                              soc_sand_rv = soc_ppd_l2_lif_ac_remove(unit, &ac_key, &lif_index_o);
                              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                          }

                              SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);
                          ac_key.vlan_domain = port_info.vlan_domain;
                              ac_key.key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN;
                          ac_key.outer_vid = value;
                          ac_key.core_id = core;

                              SOC_PPC_L2_LIF_AC_INFO_clear(&ac_info);

                          ac_info.use_lif = 1;
                              ac_info.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP;
                          ac_info.orientation  = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
                          ac_info.vsid         = value;
                              ac_info.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
                              ac_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
                          rv = sw_state_access[unit].dpp.bcm.trill.trill_global_in_lif.get(unit, &ac_info.global_lif);
                          BCMDNX_IF_ERR_EXIT(rv);

                          soc_sand_rv = soc_ppd_l2_lif_ac_add(unit, &ac_key, local_lif_index, &ac_info, &success);
                          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                          SOC_SAND_IF_FAIL_RETURN(success);
                      }
                  }
                  soc_sand_rv = soc_ppd_llp_filter_designated_vlan_set(unit, core, soc_ppd_port, value, 
                                                                TRUE, &success);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                  SOC_SAND_IF_FAIL_RETURN(success);
              }        
          }
          break;
        case bcmPortControlL2SaAuthenticaion:
        {
            
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                SOC_PPC_LLP_SA_AUTH_PORT_INFO_clear(&sa_auth_port_info);
                sa_auth_port_info.sa_auth_enable = (value!=0)?TRUE:FALSE;
                soc_sand_rv = soc_ppd_llp_sa_auth_port_info_set(unit, core, soc_ppd_port, &sa_auth_port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        break;
        case bcmPortControlLocalSwitching:
          {            
            BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info");
            if (lif_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }

            if (SOC_DPP_CONFIG(unit)->pp.local_switching_enable == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("soc property local_switch_enable is disable")));
            }

            rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);  
         
            soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (lif_info->type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
            }

            if (value == 1){
                /* Enable local switching in AC */
                lif_info->value.ac.cos_profile |= SOC_PPC_LIF_AC_LOCAL_SWITCHING_COS_PROFILE;
            }else {
                /* Disable local switching in AC */
                lif_info->value.ac.cos_profile &= ~SOC_PPC_LIF_AC_LOCAL_SWITCHING_COS_PROFILE;
            }
                
            /* update existing lif entry */
            soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
          break;
        case bcmPortControlTrill: 
          {
              SOC_PPC_LLP_VID_ASSIGN_PORT_INFO              vid_assign_port_info;
              int                                           lif_id;
              
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
              
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                if (value && ((SOC_DPP_CONFIG(unit))->trill.designated_vlan_inlif_enable)) {
                    rv = sw_state_access[unit].dpp.bcm.trill.trill_global_in_lif.get(unit, &lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (lif_id == -1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("In case of trill designated VLAN inlif enabled, trill ports all should set the specific lif IDs")));
                    }
                }
       
                soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(unit, core, soc_ppd_port, &vid_assign_port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
                soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                vid_assign_port_info.enable_sa_based = value;

                if (value) {

                    soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(unit, core, soc_ppd_port, &vid_assign_port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    
                    port_info.vlan_translation_profile = (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID) ? \
                        SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID:SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL;
                } else {
                    soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(unit, core, soc_ppd_port, &vid_assign_port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


                    port_info.vlan_translation_profile = (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID) ? \
                        SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID: SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                }
                port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
          }
          break;
        case bcmPortControlMplsExplicitNullEnable: 
          {
          
            if (!SOC_DPP_CONFIG(unit)->pp.explicit_null_support || SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Explicit null is not supported")));
            }

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                if (value) {
                  port_info.vlan_translation_profile = (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG) ? \
                        SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG_EXPLICIT_NULL: SOC_PPC_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL;
                  port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_EXPLICIT_NULL;
                } else {
                  port_info.vlan_translation_profile = (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG_EXPLICIT_NULL) ? \
                        SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG: SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                  port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                }
                

                port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
          }
          break;
        case bcmPortControlPreservePacketPriority: 
          {
              uint8                                     transmit_outer_tag;

              transmit_outer_tag = (value != 0);
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(unit, core, soc_ppd_port, 0, transmit_outer_tag);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
          }
          break;
        case bcmPortControlMplsFRREnable:
        case bcmPortControlMplsContextSpecificLabelEnable:
          {
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);

                  if (value) { /* In case of enable */
                    switch (port_info.vlan_translation_profile) {
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID:
                        if (SOC_DPP_CONFIG(unit)->pp.mpls_databases[0] != 0) { /* MPLS (and FRR) on Double lookup avaiable in case MPLS is in ISEM-A only */
                           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("FRR and Double lookup is not enabled in case MPLS Database is in ISEM-B"))); 
                        }                        
                        port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID;
                        break;
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("FRR and Double lookup is not enabled in case Port does not support initial-vid only"))); 
                        break;
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL:
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port can not support Trill and FRR-Coupling functionality")));
                        break;
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID: /* Support both ignore initial vid and FRR-Coupling */
                        port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID;
                        break;   
                    default:
                        port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING;
                        break;
                    }

                    switch (port_info.tunnel_termination_profile) {
                    default:
                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_FRR_COUPLING;
                        break;
                    } 
                                     
                  } else { /* In case of disable */
                    switch (port_info.vlan_translation_profile) {
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID: /* Disable FRR-Coupling for initial VID */
                        port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID;
                        break;                    
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING: 
                        port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                        break;   
                    case SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID:
                        port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID;
                        break;
                    default:
                        /* Don't touch */
                        break;
                    }  

                    switch (port_info.tunnel_termination_profile) {                    
                    case SOC_PPC_PORT_DEFINED_TT_PROFILE_FRR_COUPLING:
                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                        break;
                    default:
                        /* Don't touch */
                        break;
                    }  
                  }                  

                  port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                  soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
          }
          break;
        case bcmPortControlEgressFilterDisable:
          {

              /* verify value */
              if (value & ~(BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC | BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC) &&
                  (value != BCM_PORT_CONTROL_FILTER_DISABLE_ALL)) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid value parameter 0x%x"), unit, value));
              }

              /* API supports multiple ports, but in this case only the first port will be handled */
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                
                  soc_sand_rv = soc_ppd_eg_filter_port_info_get(unit, core, soc_ppd_port, &eg_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  /* Set uc/mc/bc filters, disable all filters, enable all filter controls */
                  if (value == BCM_PORT_CONTROL_FILTER_DISABLE_ALL) {
                      /* Disable all filters */
                      eg_port_info.filter_mask = SOC_PPC_EG_FILTER_PORT_ENABLE_NONE;
                  } else {
                      /* Enable uc/mc/bc filters or all filters include uc/mc/bc*/
                      eg_port_info.filter_mask |= (SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA |
                                                   SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA |
                                                   SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA |
                                                   SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON |
                                                   SOC_PPC_EG_FILTER_PORT_ENABLE_MTU);

                      /* Remove per indication */
                      if (value & BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC) {
                          eg_port_info.filter_mask &= ~SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA;
                      }

                      if (value & BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC) {
                          eg_port_info.filter_mask &= ~SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA;
                      }

                      if (value & BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC) {
                          eg_port_info.filter_mask &= ~SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA;
                      }
                  }

                  soc_sand_rv = soc_ppd_eg_filter_port_info_set(unit, core, soc_ppd_port, &eg_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
          }
          break;
          case bcmPortControlOverlayRecycle:
          {
              SOC_TMC_PORTS_PROGRAMS_INFO programs_info;

              SOC_TMC_PORTS_PROGRAMS_INFO_clear(&programs_info);

              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                  if (IS_RCY_PORT(unit,port_ndx)) {
                      
                      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                                         
                      SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
                      BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_get,(unit, core, soc_ppd_port, &pp_port_info)));

                      /* If header_type is INGECTED_2(PP)*/
                      soc_sand_rv = arad_port_header_type_get(unit, core, soc_ppd_port, &header_type_incoming, &header_type_outgoing); 
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                      if ((header_type_incoming == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2) || (header_type_incoming == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP)) {
                          pp_port_info.flags |= SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH;

                          BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_set,(unit, core, soc_ppd_port, &pp_port_info, &success)));
                          SOC_SAND_IF_FAIL_RETURN(success);
                      }

                      BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &soc_tm_port, &core));
                      if (soc_property_port_suffix_num_get(unit, port_ndx, -1, spn_CUSTOM_FEATURE, "recycle_with_ptch2", 0) == 0) {
                          /* Set other programs profiles */                      
                          BCM_SAND_IF_ERR_EXIT(arad_ports_programs_info_get(unit, soc_tm_port, &programs_info));
                          programs_info.ptc_vt_profile  = ARAD_PORTS_VT_PROFILE_OVERLAY_RCY;
                          programs_info.ptc_tt_profile  = ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
                          programs_info.ptc_flp_profile = ARAD_PORTS_FLP_PROFILE_OVERLAY_RCY;
                        
                          if ( soc_property_port_suffix_num_get(unit, port_ndx, -1,spn_CUSTOM_FEATURE,"rpf_failing_recycle_enable", FALSE)==1 ) {
                              programs_info.ptc_vt_profile  = ARAD_PORTS_VT_PROFILE_NONE;
                              programs_info.ptc_tt_profile  = ARAD_PORTS_TT_PROFILE_FORCE_BRIDGE_FWD;
                              programs_info.ptc_flp_profile = ARAD_PORTS_FLP_PROFILE_NONE;
                          }
                                                  
                          if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->trill.mode)) {
                            programs_info.en_trill_mc_in_two_path = value;
                          }
                          BCM_SAND_IF_ERR_EXIT(arad_ports_programs_info_set(unit, soc_tm_port, &programs_info));
                      }
                  } else {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port must be Recycle port")));
                  }
              }
          }
          break;
          case bcmPortControlEvbType:
          {
              SOC_PPC_LLP_TRAP_PORT_INFO  llp_trap_port_info;
              SOC_PPC_PORT soc_ppd_port;
              bcm_port_t port_ndx;

              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                
                  soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, soc_ppd_port, &llp_trap_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                
                  if (value == (int)bcmPortEvbTypeUplinkAccess) {
                      llp_trap_port_info.trap_enable_mask &= ~SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE; /* In case of EVB ingress same interface filter must be disabled so we can send packets from VEPA to VEPA */
                  } else {
                      /* other ports or when port EVB is disabled enable again same interface */
                      llp_trap_port_info.trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE;
                  }

                  soc_sand_rv = soc_ppd_llp_trap_port_info_set(unit, core, soc_ppd_port, &llp_trap_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  port_info.vlan_translation_profile = (value == (int)bcmPortEvbTypeUplinkAccess) ? \
                    SOC_PPC_PORT_DEFINED_VT_PROFILE_EVB:SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
                  port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                  soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }

              
                             
          }
          break;
        case bcmPortControlVMac:
            {
                int tunnel_id = 0;
                uint32 vmac_enable_in_ac = 0;
                bcm_vlan_port_t vlan_port;
                bcm_module_t mod_id;
                shr_llm_msg_pon_att_t msg;
                /* Disable/enable VMAC in gport */
                if (!(SOC_DPP_CONFIG(unit)->pp.vmac_enable)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't configure vmac enable/disable base gport when vmac_enable is 0")));
                }

                if (value != SOC_PPC_LIF_AC_VAMC_COS_PROFILE_DISABLE &&
                    value != SOC_PPC_LIF_AC_VAMC_COS_PROFILE_ENABLE) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid value.")));
                }
                BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info");
                if (lif_info == NULL) {        
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }
                SOC_PPC_LIF_ENTRY_INFO_clear(lif_info);
                rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                             &gport_hw_resources);
                BCMDNX_IF_ERR_EXIT(rv);  

                soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (lif_info->type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
                }

                soc_sand_rv = soc_sand_bitstream_get_field(&(lif_info->value.ac.cos_profile),
                                                               SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB,
                                                               SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB_NOF_BITS,
                                                           &vmac_enable_in_ac);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* Set vmac from disable to enable or
                 * Set vmac from enable to disable
                 */
                if ((value && (!vmac_enable_in_ac)) ||
                    (!value && vmac_enable_in_ac)){
                    soc_sand_rv = soc_sand_bitstream_set_field(&(lif_info->value.ac.cos_profile),
                                                                   SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB,
                                                                   SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB_NOF_BITS,
                                                               value);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    /* update existing lif entry */
                    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

                    if (_BCM_PPD_IS_PON_PP_PORT(gport_info.local_port, unit) && IS_PON_PORT(unit, gport_info.local_port)) {
                        /* Update mac limit in sw db */
                        bcm_vlan_port_t_init(&vlan_port);
                        rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port, gport_hw_resources.local_in_lif);
                        BCMDNX_IF_ERR_EXIT(rv);
                        tunnel_id = vlan_port.match_tunnel_value;

                        /* Remove all only if value old is different than value new */
                        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &mod_id));

                        rv = bcm_petra_l2_addr_delete_by_port(unit, mod_id, port, BCM_L2_DELETE_STATIC);
                        BCMDNX_IF_ERR_EXIT(rv);
                        
                        /* Send LLM GET message to uC */
                        sal_memset(&msg, 0x0, sizeof(msg));
                        msg.port = gport_info.local_port;
                        msg.tunnel = tunnel_id;
                        msg.tunnel_count = 1;
                        if (value) {
                            msg.type_of_service = LLM_SERVICE_VMAC;
                        } else {
                            msg.type_of_service = LLM_SERVICE_NOT_DEFINED;
                        }
                        rv = shr_llm_msg_mac_limit_set(unit, SHR_LLM_MSG_PON_ATT_MAC_BITMAP_SET, &msg);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
            }
            break;
            case bcmPortControlErspanEnable:
            {
                if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                    if (value < 0 || value > 1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                    }
                
                    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                        /* port can do ERSPAN or RSPAN/SPAN but not both,
                           setting appropriate ERSPAN/SPAN prge_profile program selection in port profile */
                        soc_sand_rv = soc_ppd_eg_encap_port_erspan_set(unit, core, soc_ppd_port, value);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Enable/Disable ERSPAN is  supported only for ARAD plus and below")));
                }
            }
            break;

        case bcmPortControlOamDefaultProfile:
            {
                uint32 oam_lif_profile_mode;
                uint32 inlif_profile;
                uint32 limit;
                if (SOC_IS_ARADPLUS(unit)) {

                    oam_lif_profile_mode = soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0);

                    BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info");
                    if (lif_info == NULL) {        
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                    }
                    switch (oam_lif_profile_mode) {
                    case 0:  /* Advanced mode */
                        rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, -1/*Not used*/, 1/*all_kinds*/, lif_info);
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                        BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);

                        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_inlif_profile_map_set,(unit, inlif_profile, value));
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Failed to set a map from inlif profile to oam profile")));
                        break;
                    case 1:  /* Simple mode - 1 bit */
                    case 2:  /* Simple mode - 2 bits */
                        limit = (1<<oam_lif_profile_mode);
                        if ((value < 0) || (value >= limit)) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Lif profile value out of bounds for oam profile")));
                        }
                        rv = _bcm_petra_port_inlif_profile_set(unit, port, value, SOC_OCC_MGMT_INLIF_APP_OAM, lif_info);
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Failed to set a map from inlif profile to oam profile")));
                        break;
                    default:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("OAM default profile mode unsupported (profile = %d)"), oam_lif_profile_mode));
                    }
                }
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("OAM default profile is not supported for this device")));
                }
            }
            break;
        case bcmPortControlOamDefaultProfileEgress:
            {
                uint32 oam_lif_profile_num_bits;
                uint32 outlif_profile;
                uint32 limit;
                oam_lif_profile_num_bits = SOC_DPP_CONFIG(unit)->pp.oam_default_egress_prof_num_bits;
                if (oam_lif_profile_num_bits) {
                    limit = (1 << oam_lif_profile_num_bits);
                    if ((value < 0) || (value >= limit)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Lif profile value out of bounds for oam profile")));
                    }
                    
                    rv = _bcm_port_outlif_profile_by_lif_get(unit,port, &outlif_profile);
                    BCMDNX_IF_ERR_EXIT(rv); 

                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_OAM_DEFAULT_MEP, value,
                                                           &outlif_profile));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                    
                    rv = _bcm_port_outlif_profile_by_lif_set(unit,port, outlif_profile, 1 /* set oam-lif*/);
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv); 
                }
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("OAM default profile egress soc property (\"bcm886xx_oam_default_profile_egress\") must be set")));
                }
            }
            break;
        case bcmPortControlMplsEncapsulateExtendedLabel:
            {
                if (SOC_IS_JERICHO(unit))  {
                    if (soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0)) {
                        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries = 0;
                        int out_lif_id = 0;
                        _bcm_lif_type_e lif_usage;

                        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                            &gport_hw_resources);

                        out_lif_id = gport_hw_resources.local_out_lif;

                        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, out_lif_id, NULL, &lif_usage);
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (lif_usage != _bcmDppLifTypeMplsTunnel && lif_usage != _bcmDppLifTypeMplsPort) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS tunnel outlif supported")));
                        }

                        encap_entry_info =
                            sal_alloc(sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, 
                                "_bcm_petra_pp_port_control_set.encap_entry_info");
                        if(encap_entry_info == NULL) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                        }

                        soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, out_lif_id, 1, \
                                                                 encap_entry_info, next_eep, &nof_entries);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, \
                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL, value,
                                              &(encap_entry_info->entry_val.mpls_encap_info.outlif_profile)));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            /* add encap entry, set outlif profile additional encapsulation bit */
                            soc_sand_rv = soc_ppd_eg_encap_mpls_entry_add(unit, out_lif_id, &(encap_entry_info->entry_val.mpls_encap_info), next_eep[0]);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, \
                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL, value,
                                              &(encap_entry_info->entry_val.swap_info.outlif_profile)));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            /* add encap entry, set outlif profile additional encapsulation bit */
                            soc_sand_rv = soc_ppd_eg_encap_swap_command_entry_add(unit, out_lif_id, &(encap_entry_info->entry_val.swap_info), next_eep[0]);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS tunnel/SWAP outlif supported")));
                        }

                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                        (_BSL_BCM_MSG("Currently bcmPortControlMplsEncapsulateExtendedLabel is supported only for ROO_EXTENSION_LABEL_ENCAPSULATION")));
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcmPortControlMplsEncapsulateExtendedLabel is supported for jericho and above")));
                }
            }
            break;
        case bcmPortControlMPLSEncapsulateAdditionalLabel:
            {
                if (SOC_IS_JERICHO(unit))  {
                    if (soc_property_get(unit, spn_MPLS_EGRESS_LABEL_EXTENDED_ENCAPSULATION_MODE, 0)) {
                        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries = 0;
                        int out_lif_id = 0;
                        _bcm_lif_type_e lif_usage;

                        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                            &gport_hw_resources);

                        out_lif_id = gport_hw_resources.local_out_lif;

                        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, out_lif_id, NULL, &lif_usage);
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (lif_usage != _bcmDppLifTypeLinkLayer && lif_usage != _bcmDppLifTypeMplsTunnel) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only MPLS tunnel and LinkLayer outlif supported")));
                        }

                        encap_entry_info =
                            sal_alloc(sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, 
                                "_bcm_petra_pp_port_control_set.encap_entry_info");
                        if(encap_entry_info == NULL) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                        }

                        soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,
                                                                 (lif_usage == _bcmDppLifTypeMplsTunnel) ? SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP : SOC_PPC_EG_ENCAP_EEP_TYPE_LL, 
                                                                 out_lif_id, 1, encap_entry_info, next_eep, &nof_entries);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, \
                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL, value,
                                              &(encap_entry_info->entry_val.mpls_encap_info.outlif_profile)));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            /* add encap entry, set outlif profile additional encapsulation bit */
                            soc_sand_rv = soc_ppd_eg_encap_mpls_entry_add(unit, out_lif_id, &(encap_entry_info->entry_val.mpls_encap_info), next_eep[0]);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, \
                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL, value,
                                              &(encap_entry_info->entry_val.ll_info.outlif_profile)));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            /* add encap entry, set outlif profile additional encapsulation bit */
                            soc_sand_rv = soc_ppd_eg_encap_ll_entry_add(unit, out_lif_id, &(encap_entry_info->entry_val.ll_info));
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS push tunnel and LL supported")));
                        }

                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                        (_BSL_BCM_MSG("Currently bcmPortControlMPLSEncapsulateAdditionalLabel is supported when mpls_egress_label_extended_encapsulation_mode is set")));
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcmPortControlMPLSEncapsulateAdditionalLabel is supported for jericho and above")));
                }
            }
            break;
        case bcmPortControlExtenderType:
            /* Set a port to a specific Port Extender port type */
            if (SOC_IS_ARADPLUS(unit)) {
                SOC_PPC_EXTENDER_PORT_INFO extender_info;
                int val;

                _BCM_DPP_EXTENDER_INIT_CHECK(unit);

                sal_memset(&extender_info, 0, sizeof(SOC_PPC_EXTENDER_PORT_INFO));

                /* Get the pp port to be used later */
                soc_ppd_port = 0;
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                    break;
                }            


                /* Configure according to the Port Extender port type*/  
                switch (value) {
                case BCM_PORT_EXTENDER_TYPE_SWITCH:

                    /* Perform cascaded port HW configuration */
                    extender_info.ing_ecid_mode = SOC_PPC_EXTENDER_PORT_ING_ECID_NOP;
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_port_info_set, (unit, port, &extender_info));
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Configure port VT profile to be of type extender control bridge */
                    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    soc_sand_rv = _bcm_petra_pp_port_control_get(unit, soc_ppd_port, bcmPortControlNonEtagDrop, &val);
                    BCMDNX_IF_ERR_EXIT(rv);

                    port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_EXTENDER_PE;
                    port_info.tunnel_termination_profile = (val) ? SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_UNTAG_CB
                                                                 : SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_CB;

                    port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                    soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    LOG_INFO(BSL_LS_BCM_PORT, (BSL_META_U(unit, "Port %d set to be a Port Extender Cascaded port\n"), gport_info.local_port));
                    break;
                case BCM_PORT_EXTENDER_TYPE_NONE:

                    /* Perform cascaded port HW configuration */
                    extender_info.ing_ecid_mode = SOC_PPC_EXTENDER_PORT_ING_ECID_NOP;
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_port_info_set, (unit, port, &extender_info));
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Configure port TT profile to be of type extender */
                    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                    port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                    soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    LOG_INFO(BSL_LS_BCM_PORT, (BSL_META_U(unit, "Port %d is no longer a Port Extender port\n"), gport_info.local_port));
                    break;
                default:
                  BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unsupported Port Extender type - %d for port %d\n"), value, gport_info.local_port));
                  break;
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Port Extender isn't supported on the device")));
            }
            break;

        case bcmPortControlNonEtagDrop:
            {
                /* Set Port drop according to an ETAG presence on the packet. */
                if (SOC_IS_ARADPLUS(unit)) {

                    int val;

                    _BCM_DPP_EXTENDER_INIT_CHECK(unit);

                    soc_ppd_port = 0;
                    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                        break;
                    }

                    /* Get the old port info, and change the TT profile to use the etag check program.*/
                    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    soc_sand_rv = _bcm_petra_pp_port_control_get(unit, soc_ppd_port, bcmPortControlExtenderType, &val);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (val == BCM_PORT_EXTENDER_TYPE_SWITCH) {
                        port_info.tunnel_termination_profile = (value) ? SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_UNTAG_CB
                                                                       : SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_CB;
                    } else {
                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                    }
                    port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;

                    soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    LOG_INFO(BSL_LS_BCM_PORT, (BSL_META_U(unit, "Port %d non ETAG drop state set to %d\n"), gport_info.local_port, value));
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Port Extender isn't supported on the device")));
                }
                break;
            }
        case bcmPortControl1588P2PDelay:
            {
                soc_ppd_port = 0;
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    break;
                }
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ptp_p2p_delay_set, \
                                                        (unit, port_ndx, value));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            }
            break;
        case bcmPortControlMplsSpeculativeParse:
            soc_ppd_port = 0;
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                break;
            }
            soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            port_info.enable_mpls_speculative_parsing = (value?1:0);
            port_info.flags = SOC_PPC_PORT_IHP_PP_PORT_INFO_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            break;
        case bcmPortControlUntaggedVlanMember:
            if (SOC_IS_JERICHO_PLUS(unit)) {
                soc_ppd_port = 0;
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                    break;
                }
                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_filter_default_port_membership_set, (unit, soc_ppd_port, value)));
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("bcmPortControlUntaggedVlanMember isn't supported on the device")));
            }
            break;
        case bcmPortControlForceBridgeForwarding:
            {
                SOC_TMC_PORTS_PROGRAMS_INFO programs_info;

                SOC_TMC_PORTS_PROGRAMS_INFO_clear(&programs_info);

                if (BCM_GPORT_IS_VLAN_PORT(port)) {
                        uint32 lif_disable_mode = soc_property_get(unit, spn_L3_DISABLED_ON_LIF_MODE, 0);
                        uint32 v4_state,v6_state;

                        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info");
                        if (lif_info == NULL) {        
                            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                        }
                        SOC_PPC_LIF_ENTRY_INFO_clear(lif_info);
                        /*bit 1: IPv6
                          bit 0: IPv4*/
                        v4_state = value&1;
                        v6_state =  (value/2)&1;


                        if (lif_disable_mode == 2) {
                              rv = _bcm_petra_port_inlif_profile_set(unit, port, v4_state, SOC_OCC_MGMT_INLIF_APP_IPV4_DISABLE, lif_info);
                              BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));                            
                              rv = _bcm_petra_port_inlif_profile_set(unit, port, v6_state, SOC_OCC_MGMT_INLIF_APP_IPV6_DISABLE, lif_info);
                              BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));                            
                        } else if (lif_disable_mode == 1){
                            rv = _bcm_petra_port_inlif_profile_set(unit, port, v4_state, SOC_OCC_MGMT_INLIF_APP_IP_DISABLE, lif_info);
                            BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));                            
                        }
                       BCM_EXIT;

                }

                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    if (SOC_DPP_CONFIG(unit)->pp.force_bridge_forwarding) {

                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx,&soc_ppd_port,&core)));

                        SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
                        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_get, (unit, core, soc_ppd_port,&pp_port_info)));

                        /* If header_type is INJECTED_2(PP)*/
                        soc_sand_rv = arad_port_header_type_get(unit, core, soc_ppd_port, &header_type_incoming, &header_type_outgoing);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        if ((SOC_DPP_CONFIG(unit)->pp.add_ptch2_header) && ((header_type_incoming == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2) || (header_type_incoming == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP))) {
                            if (value) {
                                pp_port_info.flags |= SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH;
                            } else {
                                pp_port_info.flags |= ~SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH;
                            }

                            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_pp_port_set, (unit, core, soc_ppd_port,&pp_port_info,&success)));
                            SOC_SAND_IF_FAIL_RETURN(success);
                        }
                        /* set force bridge forwarding with TT program that program selection will be ptc profile */
                        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &soc_tm_port, &core));     
                        programs_info.ptc_vt_profile  = ARAD_PORTS_VT_PROFILE_NONE;
                        programs_info.ptc_tt_profile  = value ? ARAD_PORTS_TT_PROFILE_FORCE_BRIDGE_FWD : ARAD_PORTS_TT_PROFILE_NONE;
                        programs_info.ptc_flp_profile = ARAD_PORTS_FLP_PROFILE_NONE;

                        BCM_SAND_IF_ERR_EXIT(arad_ports_programs_info_set(unit, soc_tm_port, &programs_info));

                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("force_bridge_forwarding soc property must be set")));
                    }
                }
            }
            break;
            case bcmPortControlLogicalInterfaceSameFilter:
              {
                int enable_logical_intf_same_filter = soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_SAME_FILTER_ENABLE, 0);

                if (!BCM_GPORT_IS_SET(port)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port, it should be gport")));
                }

                if (value < 0 || value > 1) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid arg (may only be 0 or 1)")));
                }
                if (enable_logical_intf_same_filter == 0) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("please set bcm886xx_logical_interface_same_filter_enable=1 at first")));
                }

                BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_pp_port_control_set.lif_info");
                if (lif_info == NULL) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }
#ifdef BCM_88660_A0
                if (SOC_IS_ARADPLUS(unit)) {
                    rv = _bcm_petra_port_inlif_profile_set(unit, port, value, SOC_OCC_MGMT_INLIF_APP_LOGICAL_INTF_SAME_FILTER, lif_info);
                    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));
                }
#endif /* BCM_88660_A0 */
              }
              break;

        case bcmPortControlIP4Mcast:
        case bcmPortControlIP6Mcast:
          {
           SOC_PPC_LIF_ENTRY_INFO lif_info;

           if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0) == 0) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("soc property ipmc_l2_ssm_fwd_enable is disable")));
           }

           rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                 &gport_hw_resources);
           BCMDNX_IF_ERR_EXIT(rv);

           BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
             if (_BCM_PPD_IS_PON_PP_PORT(port_ndx, unit)) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmPortControlIP4/6 Mcast is NOT supported on PON ports")));
             }
           }

           soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, &lif_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

           if (lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
           }

           if (value == 1){
               /* Enable l2 ssm ipmc in AC */
               lif_info.value.ac.cos_profile |= SOC_PPC_LIF_AC_L2_SSM_IPMC_COS_PROFILE;
           }else {
               /* Disable l2 ssm ipmc in AC */
               lif_info.value.ac.cos_profile &= ~SOC_PPC_LIF_AC_L2_SSM_IPMC_COS_PROFILE;
           }

           /* update existing lif entry */
           soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, &lif_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         }

           break;
        default:
          rv = BCM_E_UNAVAIL;       
          break;
        }
    } else {
        rv = BCM_E_UNAVAIL;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lif_info);
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_global_control_set(int unit, bcm_port_control_t type, int value) {
    int soc_sand_rv, soc_sand_dev_id;
    SOC_PPC_LLP_COS_GLBL_INFO glbl_info;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Global type check */
    switch (type) {  
      case bcmPortControlDropPrecedence:
          if ((value < 0) || (value > 3)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Drop Precedence value is out of range. Range is 0-3")));
          }
          SOC_PPC_LLP_COS_GLBL_INFO_clear(&glbl_info);
          glbl_info.default_dp = value;

          soc_sand_rv = soc_ppd_llp_cos_glbl_info_set(soc_sand_dev_id, &glbl_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          break;

      case bcmPortControlFcoeFabricSel:
        if((value != bcmPortFcoeVsanSelectVft) && (value != bcmPortFcoeVsanSelectOuterVlan)){
            BCM_SAND_IF_ERR_EXIT(BCM_E_PARAM);
        }
        else {
            if (value == bcmPortFcoeVsanSelectOuterVlan) {
                value = TRUE;
            }
            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_fcf_vsan_mode_set,(unit, value)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
      default:  
          BCM_ERR_EXIT_NO_MSG(BCM_E_UNAVAIL);       
          break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_pp_port_control_get(int unit, bcm_port_t port,
                            bcm_port_control_t type, int *value)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    int core = 0;
    SOC_PPC_PORT soc_ppd_port = 0;
    bcm_port_t port_ndx;
    _bcm_petra_port_mact_mgmt_action_profile_t action_profile;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_EG_FILTER_PORT_INFO eg_port_info;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    SOC_PPC_LIF_ENTRY_INFO lif_info;
    SOC_PPC_FP_CONTROL_INFO
       ctrl_info;
    SOC_PPC_FP_CONTROL_INDEX
       ctrl_indx;
    _bcm_dpp_gport_info_t   gport_info;
    uint32 sys_port=0, operations;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    BCMDNX_INIT_FUNC_DEFS;

    /* In case of vlan isolation. ask for system port */
    operations = (type == bcmPortControlPrivateVlanIsolate) ? _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT : 0;
    
    if (!BCM_GPORT_IS_TUNNEL(port)) {
        rv = _bcm_dpp_gport_to_phy_port(unit, port, operations, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        sys_port = gport_info.sys_port;
    }
    else {
         sal_memset(&gport_info, 0, sizeof(_bcm_dpp_gport_info_t));       
    }

    /* Check for PP controls */
    if (SOC_DPP_PP_ENABLE(unit)) {
      switch (type)
      {
      /* controls can be only on one*/
      case bcmPortControlMpls:
      case bcmPortControlIP4:
      case bcmPortControlIP6:
      case bcmPortControlIP4Mcast:
      case bcmPortControlIP6Mcast:
            *value = 1;
            break;
      /* controls can be only on zero */
      case bcmPortControlDoNotCheckVlan:
            *value = 0;
            break;
      case bcmPortControlFcoeFabricId:

          if(!SOC_DPP_CONFIG(unit)->pp.fcoe_enable ){
              rv = BCM_E_UNAVAIL;
          } else{
              soc_ppd_port = 0;
                  BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                      break;
                  }
                  SOC_PPC_FP_CONTROL_INDEX_clear(&ctrl_indx);
                  SOC_PPC_FP_CONTROL_INFO_clear(&ctrl_info);
                  ctrl_indx.val_ndx = soc_ppd_port;
                  ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA;
                  soc_sand_rv = soc_ppd_fp_control_get(unit,SOC_CORE_INVALID,&ctrl_indx,&ctrl_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                  (*value) = ctrl_info.val[0];
                  (*value) = (*value)>>1;
          }
          break;
      case bcmPortControlFcoeNetworkPort:
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                (*value) = (!port_info.is_n_port);
                break;
            }            
           break;
      case bcmPortControlMacInMac:
      {

          /* API supports multiple ports, but in this case only the first port will be handled */
          BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
              BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

              /* return whether port is PBP */
              soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              *value = SOC_SAND_BOOL2NUM(port_info.is_pbp);
              break;
          }
          break;
      }   
#ifdef BCM_CMICM_SUPPORT  
      case bcmPortControlL2Move: 
        {
          uint32 val;
          
            if ((SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) && SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
              if (_BCM_DPP_GPORT_INFO_IS_LAG(gport_info)) {
                rv = shr_llm_msg_mac_move_get(unit, sys_port, &val);
                BCMDNX_IF_ERR_EXIT(rv);
                *value = val;
              } else {
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                  rv = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &soc_ppd_port, &core);
                  BCMDNX_IF_ERR_EXIT(rv);
                  rv = shr_llm_msg_mac_move_get(unit, soc_ppd_port, &val);
                  BCMDNX_IF_ERR_EXIT(rv);
                  *value = val;
                }
              }
            }
        }        
        break;  
#endif
      case bcmPortControlDiscardMacSaAction:
      {

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
              BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
              
              rv = _bcm_dpp_am_template_port_mact_sa_drop_data_get(unit,core,soc_ppd_port,&action_profile);
              BCMDNX_IF_ERR_EXIT(rv);
              
              /* Convert action profile to value */
              switch (action_profile)
              {
                    case _bcm_petra_port_mact_mgmt_action_profile_none:
                        *value = BCM_PORT_CONTROL_DISCARD_MACSA_NONE;
                        break;
                  case _bcm_petra_port_mact_mgmt_action_profile_trap:
                    *value = BCM_PORT_CONTROL_DISCARD_MACSA_TRAP;
                    break;
                  case _bcm_petra_port_mact_mgmt_action_profile_drop:
                    *value = BCM_PORT_CONTROL_DISCARD_MACSA_DROP;
                    break;
                  case _bcm_petra_port_mact_mgmt_action_profile_trap_and_drop:
                    *value = BCM_PORT_CONTROL_DISCARD_MACSA_DROP | BCM_PORT_CONTROL_DISCARD_MACSA_TRAP;
                    break;
                  default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: bcmPortControlDiscardMacSaAction failed, given incorrect action profile %d for port %d unit %d"), FUNCTION_NAME(), action_profile, port, unit));
                    break;
              }              
              break;
            }
         }
        break;
      case bcmPortControlUnknownMacDaAction:
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Petra-B only"))); 
        }
        break;
      case bcmPortControlPrivateVlanIsolate:
        {
            SOC_SAND_PP_SYS_PORT_ID pp_sys_port;
            SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE pvlan_type;
            SOC_TMC_DEST_INFO tm_dest_info;

            if (!SOC_DPP_CONFIG(unit)->pp.pvlan_enable) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("pvlan_enable isn't enabled")));
            }

            soc_sand_SAND_PP_SYS_PORT_ID_clear(&pp_sys_port);
            SOC_TMC_DEST_INFO_clear(&tm_dest_info);

            rv = _bcm_dpp_gport_to_tm_dest_info(unit,port,&tm_dest_info);
            BCMDNX_IF_ERR_EXIT(rv);

            if(tm_dest_info.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
                pp_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
                pp_sys_port.sys_id = tm_dest_info.id;
            }
            else if(tm_dest_info.type == SOC_TMC_DEST_TYPE_LAG) {
                pp_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
                pp_sys_port.sys_id = tm_dest_info.id;
            }
            else{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid type %d"),tm_dest_info.type));
            }
              
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));
            soc_sand_rv = soc_ppd_eg_filter_pvlan_port_type_get(unit,&pp_sys_port,&pvlan_type);
            SOC_SAND_IF_ERROR_RETURN(soc_sand_rv);

            /* Convert pvlan type to value */
            if (pvlan_type == SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED) {            
              *value = 1;
            } else if (pvlan_type == SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS){
              *value = 2;
            } else {
              *value = 0;
            }
          }
        break;
      case bcmPortControlBridge:
        {
          if (BCM_GPORT_IS_VLAN_PORT(port)) {
            /* Same interface according to incoming-LIF */
            int simple_mode = soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0);

            if (simple_mode) {
              /* Simple mode -- Get proper bit in inlif profile. */
              rv = _bcm_petra_port_inlif_profile_get(unit, port, (uint32*)value, SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE, 0/*all_kinds*/, &lif_info);
              BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

              *value = (*value == 0) ? 1 : 0;
            } else {

              *value = 0;
#ifdef BCM_88660_A0
              if (SOC_IS_ARADPLUS(unit)) {
                /* Advanced mode */
                uint32 mask;
                SOC_PPC_EG_FILTER_GLOBAL_INFO info;
                uint32 inlif_profile;

                /* We take the user inlif profile (whichever part of the whole it is) and check its mask. */
                rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, SOC_OCC_MGMT_APP_USER, 0/*all_kinds*/, &lif_info);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);

                mask = (1 << inlif_profile);

                SOC_PPC_EG_FILTER_GLOBAL_INFO_clear(&info);
                soc_sand_rv = soc_ppd_eg_filter_global_info_get(unit, &info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                *value  = (info.in_lif_profile_disable_same_interface_filter_bitmap & mask) ? 1 : 0;

              }
#endif /* BCM_88660_A0 */
            }

          } else {
            /* Same interface pruning filter settings */
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

              BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
              SOC_PPC_EG_FILTER_PORT_INFO_clear(&eg_port_info);                
              soc_sand_rv = soc_ppd_eg_filter_port_info_get(unit, core, soc_ppd_port, &eg_port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              if (eg_port_info.filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE) {
                  *value = 1;
              } else {
                  *value = 0;
              }
            }
          }
        }
        break;
        case bcmPortControlFloodBroadcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodUnknownUcastGroup:
          {
            int is_lif_flooding = FALSE;
            int is_port_flooding = FALSE;                
            _bcm_dpp_gport_parse_info_t gport_parse_info;                
            bcm_gport_t uc, mc, bc;
            bcm_gport_t logical_gport;

            gport_hw_resources.local_in_lif = -1;
            
            rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
            BCMDNX_IF_ERR_EXIT(rv);

            if (gport_parse_info.type == _bcmDppGportParseTypeSimple) {
                /* Flooding is per port */
                is_port_flooding = TRUE;      
            } else {
                /* VLAN, MPLS or Extender Port GPORT - retreive LIF index directly */
                if (BCM_GPORT_IS_VLAN_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port) ||
                    BCM_GPORT_IS_EXTENDER_PORT(port) || BCM_GPORT_IS_VXLAN_PORT(port) ||
                    BCM_GPORT_IS_TUNNEL(port)) {
                    is_lif_flooding = TRUE;

                    if (BCM_GPORT_IS_TUNNEL(port) &&
                        SOC_IS_JERICHO(unit) &&
                        soc_property_get(unit, spn_EVPN_ENABLE, 0) &&
                        (port & _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION)) {
                        /* Reset the iml_bos indicator in tunnel gport*/
                        logical_gport = port & (~_BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION);
                    } else {
                        logical_gport = port;
                    }
                    rv = _bcm_dpp_gport_to_hw_resources(unit, logical_gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                             &gport_hw_resources);
                    BCMDNX_IF_ERR_EXIT(rv);  
                }
            }

            rv = _bcm_petra_vlan_flooding_per_lif_get(unit, port, gport_hw_resources.local_in_lif, is_port_flooding,
                                 is_lif_flooding, &uc, &mc, &bc);                
            BCMDNX_IF_ERR_EXIT(rv);

            /* Retreive information according to type */
            switch (type) {
            case bcmPortControlFloodBroadcastGroup:
              *value = bc;
              break;
            case bcmPortControlFloodUnknownMcastGroup:
              *value = mc;
              break;
            case bcmPortControlFloodUnknownUcastGroup:
              *value = uc;
              break;
            /* must have default. Otherwise, compilation error */
            /* coverity[dead_error_begin : FALSE] */
            default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unexpected type control")));
            }
            
            BCMDNX_IF_ERR_EXIT(rv);
            _BCM_MULTICAST_GROUP_SET(*value, _BCM_MULTICAST_TYPE_L2, *value);
          }
          break;
          case bcmPortControlMplsFRREnable:
          case bcmPortControlMplsContextSpecificLabelEnable:
          {
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);

                  *value = (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID 
                           || port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING) ? 1:0;                  
                  break;
              }
          }
          break;
        case bcmPortControlMplsExplicitNullEnable: 
        {
            if (!SOC_DPP_CONFIG(unit)->pp.explicit_null_support || SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Explicit null is not supported")));
            }

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);

                  *value = port_info.tunnel_termination_profile == SOC_PPC_PORT_DEFINED_TT_PROFILE_EXPLICIT_NULL;                           
                  break;
              }
        }
        break;

      case bcmPortControlPreservePacketPriority: 
        {
            uint8                                     transmit_outer_tag;

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                soc_sand_rv = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(unit, core, soc_ppd_port, 0, &transmit_outer_tag);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                *value = transmit_outer_tag;
                break;
            }
        }
        break;
        case bcmPortControlTrillDesignatedVlan: 
          {
              uint8                                     accept;
              SOC_SAND_PP_VLAN_ID                       vid;
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_llp_filter_designated_vlan_get(unit, core, soc_ppd_port, &vid, 
                                                                &accept);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  *value = (int)vid;
              }
        
          }
          break;
         case bcmPortControlL2SaAuthenticaion: 
          {
              SOC_PPC_LLP_SA_AUTH_PORT_INFO                 port_auth_info;
              
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_llp_sa_auth_port_info_get(unit, core, soc_ppd_port, &port_auth_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);                 

                  *value = (port_auth_info.sa_auth_enable) ? 1:0;
              }
        
          }
          break;
        case bcmPortControlLocalSwitching:
          {            
            if (SOC_DPP_CONFIG(unit)->pp.local_switching_enable == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("soc property local_switch_enable is disable")));
            }

            rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);  
      
            soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, &lif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
            }

            *value = ((lif_info.value.ac.cos_profile & SOC_PPC_LIF_AC_LOCAL_SWITCHING_COS_PROFILE) ? 1: 0);
          }
          break;
        case bcmPortControlTrill: 
          {
              
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
              
                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  *value = (int)(port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL || port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID);
                  break;
              }
          }
          break;
        case bcmPortControlEgressFilterDisable:
        {

              /* only the first port will be handled */
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
  
                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                
                  soc_sand_rv = soc_ppd_eg_filter_port_info_get(unit, core, soc_ppd_port, &eg_port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  *value = 0; /* All enable */
                  if (eg_port_info.filter_mask == SOC_PPC_EG_FILTER_PORT_ENABLE_NONE) {
                      *value = BCM_PORT_CONTROL_FILTER_DISABLE_ALL;
                  } else {
                      /* Remove per indication */
                      if (!(eg_port_info.filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA)) {
                          *value |= BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC;
                      }

                      if (!(eg_port_info.filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA)) {
                          *value |= BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC;
                      }

                      if (!(eg_port_info.filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA)) {
                          *value |= BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
                      }
                  }
                  break;
              }
        }
        break;
        case bcmPortControlOverlayRecycle:
        {
          uint32 soc_tm_port;
          SOC_TMC_PORTS_PROGRAMS_INFO programs_info;

          SOC_TMC_PORTS_PROGRAMS_INFO_clear(&programs_info);

          /* only the first port will be handled */
          BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
             BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &soc_tm_port, &core));

              BCM_SAND_IF_ERR_EXIT(arad_ports_programs_info_get(unit, soc_tm_port, &programs_info));
              *value = programs_info.en_trill_mc_in_two_path;
              break;        
          }
        }
        break;
        case bcmPortControlEvbType:
        {
            SOC_PPC_LLP_TRAP_PORT_INFO llp_trap_port_info;

            /* only the first port will be handled */
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, soc_ppd_port, &llp_trap_port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                if (llp_trap_port_info.trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE){
                    *value = bcmPortEvbTypeNone;
                } else {
                    *value = bcmPortEvbTypeUplinkAccess;
                }
                break;
            }
        }
        break;
        case bcmPortControlVMac:
          {              
              uint32 vmac_enable_in_ac = 0;
              
              /* Disable/enable VMAC in gport */
              if (!(SOC_DPP_CONFIG(unit)->pp.vmac_enable)) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't configure vmac enable/disable base gport when vmac_enable is 0")));
              }

              SOC_PPC_LIF_ENTRY_INFO_clear(&lif_info);
              rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                             &gport_hw_resources);
              BCMDNX_IF_ERR_EXIT(rv);  

              soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, &lif_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              if (lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
              }

              soc_sand_rv = soc_sand_bitstream_get_field(&(lif_info.value.ac.cos_profile),
                                                             SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB,
                                                             SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB_NOF_BITS,
                                                         &vmac_enable_in_ac);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              *value = vmac_enable_in_ac;
          }

        break;
          case bcmPortControlErspanEnable:
          {
              if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                  uint8   is_erspan;

                  BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {

                      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                      /* port can do ERSPAN or RSPAN/SPAN but not both,
                         gettting whether ERSPAN prrogram enabled acording to prge_profile program selection in port profile */
                      soc_sand_rv = soc_ppd_eg_encap_port_erspan_get(unit, core, soc_ppd_port, &is_erspan);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                      *value = is_erspan;
                      break;
                  }
              } else {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Enable/Disable ERSPAN is  supported only for ARAD plus and below")));
              }
          }
          break;
        case bcmPortControlOamDefaultProfile:
            {
                uint32 oam_lif_profile_mode;
                SOC_PPC_LIF_ENTRY_INFO lif_entry;
                uint32 inlif_profile;
                uint32 oam_trap_profile;
                if (SOC_IS_ARADPLUS(unit)) {

                    oam_lif_profile_mode = soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0);
                    switch (oam_lif_profile_mode) {
                    case 0:  /* Advanced mode */
                        rv = _bcm_petra_port_inlif_profile_get(unit, port, &inlif_profile, -1/*Not used*/, 1/*all_kinds*/, &lif_entry);
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));

                        BCMDNX_VERIFY(inlif_profile <= BCM_PORT_INLIF_PROFILE_MAX);

                        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_inlif_profile_map_get,(unit, inlif_profile, &oam_trap_profile));
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Failed to set a map from inlif profile to oam profile")));
                        break;
                    case 1:  /* Simple mode - 1 bit */
                    case 2:  /* Simple mode - 2 bits */
                        _bcm_petra_port_inlif_profile_get(unit, port, &oam_trap_profile, SOC_OCC_MGMT_INLIF_APP_OAM, 0/*all_kinds*/, &lif_entry);
                        break;
                    default:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("OAM default profile mode unsupported (profile = %d)"), oam_lif_profile_mode));
                    }
                    *value = (int)oam_trap_profile;
                }
            }
            break;
        case bcmPortControlOamDefaultProfileEgress:
            {
                uint32 outlif_profile;
                uint32 oam_outlif_prof;
                uint32 oam_lif_profile_num_bits;
                oam_lif_profile_num_bits = SOC_DPP_CONFIG(unit)->pp.oam_default_egress_prof_num_bits;
                if (oam_lif_profile_num_bits) {
                    rv = _bcm_port_outlif_profile_by_lif_get(unit,port, &outlif_profile);
                    BCMDNX_IF_ERR_EXIT(rv); 

                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_OAM_DEFAULT_MEP, &outlif_profile,
                                                           &oam_outlif_prof));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                    *value = oam_outlif_prof;
                }
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("OAM default profile egress soc property (\"bcm886xx_oam_default_profile_egress\") must be set")));
                }
            }
            break; 
        case bcmPortControlMplsEncapsulateExtendedLabel:
            {
                if (SOC_IS_JERICHO(unit))  {
                    if (soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0)) {
                        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries = 0;
                        int out_lif_id = 0;
                        _bcm_lif_type_e lif_usage;

                        encap_entry_info =
                            sal_alloc(sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "encap_entry_info");
                        if(encap_entry_info == NULL) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                        }

                        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                            &gport_hw_resources);

                        out_lif_id = gport_hw_resources.local_out_lif;

                        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, out_lif_id, NULL, &lif_usage);
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (lif_usage != _bcmDppLifTypeMplsTunnel && lif_usage != _bcmDppLifTypeMplsPort) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS tunnel outlif supported")));
                        }

                        soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, out_lif_id, 1, \
                                                                 encap_entry_info, next_eep, &nof_entries);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get, \
                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL,
                                              &(encap_entry_info->entry_val.mpls_encap_info.outlif_profile), (uint32*)value));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get, \
                                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL,
                                              &(encap_entry_info->entry_val.swap_info.outlif_profile), (uint32*)value));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS tunnel/SWAP outlif supported")));
                        }

                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                        (_BSL_BCM_MSG("Currently bcmPortControlMplsEncapsulateExtendedLabel is supported only for ROO_EXTENSION_LABEL_ENCAPSULATION")));
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcmPortControlMplsEncapsulateExtendedLabel is supported for jericho and above")));
                }
            }
            break;
      case bcmPortControlMPLSEncapsulateAdditionalLabel:
          {
              if (SOC_IS_JERICHO(unit))  {
                  if (soc_property_get(unit, spn_MPLS_EGRESS_LABEL_EXTENDED_ENCAPSULATION_MODE, 0)) {
                      uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries = 0;
                      int out_lif_id = 0;
                      _bcm_lif_type_e lif_usage;

                      encap_entry_info =
                          sal_alloc(sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "encap_entry_info");
                      if(encap_entry_info == NULL) {
                          BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                      }

                      rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                          &gport_hw_resources);

                      out_lif_id = gport_hw_resources.local_out_lif;

                      rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, out_lif_id, NULL, &lif_usage);
                      BCMDNX_IF_ERR_EXIT(rv);

                      if (lif_usage != _bcmDppLifTypeMplsTunnel && lif_usage != _bcmDppLifTypeLinkLayer) {
                          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS push tunnel and Link Layer outlif supported")));
                      }

                      soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,
                                                               (lif_usage == _bcmDppLifTypeMplsTunnel) ? SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP : SOC_PPC_EG_ENCAP_EEP_TYPE_LL, 
                                                               out_lif_id, 1, encap_entry_info, next_eep, &nof_entries);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                      if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
                          soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get, \
                                        (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                            &(encap_entry_info->entry_val.mpls_encap_info.outlif_profile), (uint32*)value));
                          BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                      }
                      else if (encap_entry_info->entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
                          soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get, \
                                        (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                            &(encap_entry_info->entry_val.ll_info.outlif_profile), (uint32*)value));
                          BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                      }
                      else {
                          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS push tunnel and Link Layer outlif supported")));
                      }

                  } else {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                      (_BSL_BCM_MSG("Currently bcmPortControlMPLSEncapsulateAdditionalLabel is supported only when mpls_egress_label_extended_encapsulation_mode is set")));
                  }
              } else {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcmPortControlMPLSEncapsulateAdditionalLabel is supported for jericho and above")));
              }
          }
          break;

        case bcmPortControl1588P2PDelay:
            {
                soc_ppd_port = 0;
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    break;
                }
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ptp_p2p_delay_get, \
                                                        (unit, port_ndx, value));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            }
            break;          
      case bcmPortControlExtenderType:
          /* Get a port's Port Extender port type */
          if (SOC_IS_ARADPLUS(unit)) {
              SOC_PPC_EXTENDER_PORT_INFO extender_info;

              _BCM_DPP_EXTENDER_INIT_CHECK(unit);

              sal_memset(&extender_info, 0, sizeof(SOC_PPC_EXTENDER_PORT_INFO));

              /* Get the pp port to be used later */
              soc_ppd_port = 0;
              BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                  break;
              }            


              /* Get the Port Extender HW configuration. */  
              rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_port_info_get, (unit, port, &extender_info));
              BCMDNX_IF_ERR_EXIT(rv);

              soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              
              if ((extender_info.ing_ecid_mode == SOC_PPC_EXTENDER_PORT_ING_ECID_NOP)
                  && (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_EXTENDER_PE)
                  && (port_info.tunnel_termination_profile != SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT)) {
                  *value = BCM_PORT_EXTENDER_TYPE_SWITCH; 
              } else {
                  *value = BCM_PORT_EXTENDER_TYPE_NONE;
              }
          } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Port Extender isn't supported on the device")));
          }
          break;
      case bcmPortControlNonEtagDrop:
          {
              /* Get Port drop according to an ETAG presence on the packet. */
              if (SOC_IS_ARADPLUS(unit)) {

                  _BCM_DPP_EXTENDER_INIT_CHECK(unit);

                  BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                      break;
                  }

                  /* Get the old port info, and change the TT profile to use the etag check program.*/
                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  *value = (port_info.tunnel_termination_profile == SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_UNTAG_CB) ? TRUE : FALSE;
              } else {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Port Extender isn't supported on the device")));
              }
              break;
          }
      case bcmPortControlMplsSpeculativeParse:
            soc_ppd_port = 0;
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                break;
            }
            soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            *value = port_info.enable_mpls_speculative_parsing;
            break;
          case bcmPortControlUntaggedVlanMember:
              if (SOC_IS_JERICHO_PLUS(unit)) {
                  uint8 is_member;
                  soc_ppd_port = 0;
                  BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));
                    break;
                  }
                  BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_filter_default_port_membership_get, (unit, soc_ppd_port, &is_member)));
                  *value = is_member;
              } else {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("bcmPortControlUntaggedVlanMember isn't supported on the device")));
              }
              break;
        case bcmPortControlForceBridgeForwarding:
            {
                uint32 soc_tm_port;
                SOC_TMC_PORTS_PROGRAMS_INFO programs_info;

                if (BCM_GPORT_IS_VLAN_PORT(port)) {
                        uint32 lif_disable_mode = soc_property_get(unit, spn_L3_DISABLED_ON_LIF_MODE, 0);
                        uint32 v4_state,v6_state;
                        SOC_PPC_LIF_ENTRY_INFO lif_info;

                        sal_memset(&lif_info,0,sizeof(SOC_PPC_LIF_ENTRY_INFO));

                        if (lif_disable_mode == 2) {
                              rv = _bcm_petra_port_inlif_profile_get(unit, port, &v4_state, SOC_OCC_MGMT_INLIF_APP_IPV4_DISABLE,0, &lif_info);
                              BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));                            
                              rv = _bcm_petra_port_inlif_profile_get(unit, port, &v6_state, SOC_OCC_MGMT_INLIF_APP_IPV6_DISABLE,0, &lif_info);
                              BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));                     
                              *value = v4_state | (v6_state<<1);       
                        } else if (lif_disable_mode == 1){
                            rv = _bcm_petra_port_inlif_profile_get(unit, port, &v4_state, SOC_OCC_MGMT_INLIF_APP_IP_DISABLE, 0,&lif_info);
                            BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed setting the inlif profile")));                            
                            *value = v4_state ;

                        }
                       BCM_EXIT;

                }

                SOC_TMC_PORTS_PROGRAMS_INFO_clear(&programs_info);

                /* only the first port will be handled */
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &soc_tm_port, &core));

                    BCM_SAND_IF_ERR_EXIT(arad_ports_programs_info_get(unit, soc_tm_port, &programs_info));
                    *value = (programs_info.ptc_tt_profile == ARAD_PORTS_TT_PROFILE_FORCE_BRIDGE_FWD) ? 1 : 0;
                    break;
                }
            }
            break;
        case bcmPortControlLogicalInterfaceSameFilter:
          {
            int enable_logical_intf_same_filter = soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_SAME_FILTER_ENABLE, 0);

            if (!BCM_GPORT_IS_SET(port)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port, it should be gport")));
            }

            if (enable_logical_intf_same_filter == 0) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("please set bcm886xx_logical_interface_same_filter_enable=1 at first")));
            }

#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)) {
                SOC_PPC_LIF_ENTRY_INFO_clear(&lif_info);
                rv = _bcm_petra_port_inlif_profile_get(unit, port, (uint32*)value, SOC_OCC_MGMT_INLIF_APP_LOGICAL_INTF_SAME_FILTER, 0/*all_kinds*/, &lif_info);
                BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed getting the inlif profile")));
            }
#endif /* BCM_88660_A0 */
          }
          break;

      default:
          rv = BCM_E_UNAVAIL;
          break;
      }
    } else {
      rv = BCM_E_UNAVAIL;
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if(encap_entry_info != NULL)
        sal_free(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_global_control_get(int unit, bcm_port_control_t type, int *value)
{
    int soc_sand_rv;
    SOC_PPC_LLP_COS_GLBL_INFO glbl_info;
    int curr_val = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Global type check */
    switch (type) {  
    case bcmPortControlDropPrecedence:
        SOC_PPC_LLP_COS_GLBL_INFO_clear(&glbl_info);
        soc_sand_rv = soc_ppd_llp_cos_glbl_info_get(unit, &glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *value = glbl_info.default_dp;
        break;

    case bcmPortControlFcoeFabricSel:
        soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_fcf_vsan_mode_get,(unit, &curr_val)));
        BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        if(curr_val == TRUE){
            (*value) = bcmPortFcoeVsanSelectOuterVlan;
        } else {
            (*value) = curr_val;
        }
        break;

      default:  
          BCM_ERR_EXIT_NO_MSG(BCM_E_UNAVAIL);       
          break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}
    

int
bcm_arad_port_qcn_config_set(int unit, bcm_gport_t port, bcm_port_congestion_config_t *config)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_port_t otm_port;
    uint32 soc_sand_rc = 0, soc_sand_dev_id; 
    ARAD_CNM_CP_PACKET_INFO info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    info.ftmh_tc = config->traffic_class;
    info.ftmh_dp = config->color;
    info.ether_type = config->ethertype;
    info.pdu_version = config->version;
    info.pdu_reserved_v = config->reserved_v;

    if(config->flags & BCM_PORT_CONGESTION_CONFIG_DEST_PORT_VALID &&
       config->dst_port != BCM_GPORT_TYPE_NONE &&
       config->dst_port != BCM_GPORT_INVALID)
    {
      /* OTM port is specified */
      rv = bcm_petra_port_local_get(unit, config->dst_port, &otm_port);
      BCMDNX_IF_ERR_EXIT(rv);

      info.ftmh_otm_port = (uint32)otm_port;
    }
    else
    {
      /* OTM will be taken from IFP mapping */
      info.ftmh_otm_port = -1;
    }

    /* Get source port from given GPort */
    if(config->src_port == BCM_GPORT_TYPE_NONE || config->src_port == BCM_GPORT_INVALID)
    {
      /* SRC port taken from original packet */
      info.ftmh_src_port = -1;
    }
    else
    {
      /* SRC port is specified */
      rv = bcm_petra_port_local_get(unit, config->src_port, &otm_port);
      BCMDNX_IF_ERR_EXIT(rv);

      info.ftmh_src_port = otm_port;
    }

    /* The SOC_SAND MAC address is little-endian */
    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
        info.mac_sa.address[i] = config->src_mac[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1];
    }

    /* VLAN */
    if(BCM_VLAN_VALID(config->vlan))
    {
      info.has_vlan_config = 1;
      info.vlan_priority = config->pri;
      info.vlan_cfi = config->cfi;
      info.vlan_id = config->vlan;
    } else {
      info.has_vlan_config = 0;
    }

    info.vlan_edit_command_with_cn_tag = 0;
    info.vlan_edit_command_without_cn_tag = 0;

    soc_sand_rc = arad_cnm_cp_packet_set(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_arad_port_qcn_config_get(int unit, bcm_gport_t port, bcm_port_congestion_config_t *config)
{
    int i;
    uint32 soc_sand_rc = 0, soc_sand_dev_id; 
    bcm_module_t    my_modid;
    ARAD_CNM_CP_PACKET_INFO info;
    uint32                  tm_port;
    int                     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &my_modid));

    soc_sand_rc = arad_cnm_cp_packet_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    config->traffic_class = info.ftmh_tc;
    config->ethertype = info.ether_type;
    config->version = info.pdu_version;
    config->reserved_v = info.pdu_reserved_v;

    /* Handle OTM Port */
    if(info.ftmh_otm_port == -1)
    {
      /* OTM will be taken from IFP mapping */
      config->dst_port = BCM_GPORT_TYPE_NONE;
    }
    else
    {
      /* OTM port is specified */
      config->flags |= BCM_PORT_CONGESTION_CONFIG_DEST_PORT_VALID;
      BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &my_modid));
      BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, info.ftmh_otm_port, &tm_port, &core));

      SOC_GPORT_MODPORT_SET(config->dst_port, SOC_DPP_CORE_TO_MODID(my_modid, core), tm_port);
    }
    

    /* Set Gport (must be MODPORT) from source port */
    if(info.ftmh_src_port == -1)
    {
      /* SRC port taken from original packet */
      config->src_port = BCM_GPORT_TYPE_NONE;
    }
    else
    {
        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &my_modid));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, info.ftmh_src_port, &tm_port, &core));

        /* SRC port is specified */
        SOC_GPORT_MODPORT_SET(config->src_port, SOC_DPP_CORE_TO_MODID(my_modid, core), tm_port);
    }
    
    /* The SOC_SAND MAC address is little-endian */
    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
        config->src_mac[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = info.mac_sa.address[i];
    }

    /* VLAN */
    if(info.has_vlan_config)
    {
      config->vlan = info.vlan_id;
      config->cfi = info.vlan_cfi;
      config->pri = info.vlan_priority;
    }
    else
    {
      config->vlan = BCM_VLAN_INVALID;
      config->cfi = 0;
      config->pri = 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_congestion_config_set(int unit, bcm_gport_t port,
                                           bcm_port_congestion_config_t *config)
{
    int         rv = BCM_E_NONE;
    bcm_port_t  local_port;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Handle CNM / QCN Configuration */
    if((config->flags & BCM_PORT_CONGESTION_CONFIG_QCN) && (config->flags & BCM_PORT_CONGESTION_CONFIG_TX))
    {
        rv = bcm_arad_port_qcn_config_set(unit, port, config);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
      rv = bcm_petra_port_local_get(unit, port, &local_port);
      BCMDNX_IF_ERR_EXIT(rv);

      rv = bcm_petra_port_pause_addr_set(unit, local_port, config->src_mac);
      BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_congestion_config_get(int unit, bcm_gport_t port,
                                           bcm_port_congestion_config_t *config)
{
    int         rv = BCM_E_NONE;
    int         restore_flags;

    BCMDNX_INIT_FUNC_DEFS;

    restore_flags = config->flags;
    sal_memset(config, 0, sizeof(bcm_port_congestion_config_t));
    config->flags = restore_flags;

    /* Handle CNM / QCN Configuration */
    if((config->flags & BCM_PORT_CONGESTION_CONFIG_QCN) && (config->flags & BCM_PORT_CONGESTION_CONFIG_TX))
    {
        rv = bcm_arad_port_qcn_config_get(unit, port, config);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
      config->flags = 0; /* clear it for now */
      rv = bcm_petra_port_pause_addr_get(unit, port, config->src_mac);
      BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}



#define _BCM_DPP_MAX_DSCP_VAL   (255)

/* globals */

/* default learn flags per port*/
#define BCM_PETRA_PORT_LEARN_DFLT_FLGS (BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD)

/*  mark as invalid flags in given trap */
#define BCM_PETRA_PORT_LEARN_INVALID_FLGS (0Xf)

/* when COS mapping has several tables, this table is used*/
#define BCM_PETRA_PORT_COS_TBL_INDX (0)

/* Default entry for PORT CFI COLOR */
#define BCM_PETRA_PORT_CFI_COLOR_GLOBAL_ENTRY (0)

int
bcm_petra_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    SOC_PPC_LLP_VID_ASSIGN_PORT_INFO port_info;
    uint32                           rv, soc_sand_rv;
    SOC_PPC_PORT                     soc_ppd_port;
    bcm_port_t                       port_i;
    int                              core;
    _bcm_dpp_gport_info_t            gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* SDK-SDK-101984 */
    /* per Customer demand, allow pvid 0 */
    if (vid >= BCM_VLAN_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given invalid VLAN")));
    }
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
    
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(&port_info);
        soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        port_info.pvid = vid; 
        soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_untagged_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vid)
{
    SOC_PPC_LLP_VID_ASSIGN_PORT_INFO port_info;
    uint32                           rv ,soc_sand_rv;
    SOC_PPC_PORT                     soc_ppd_port;
    bcm_port_t                       port_i;
    int                              core;
    _bcm_dpp_gport_info_t            gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(&port_info);
        soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        *vid = port_info.pvid;
        break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
   SOC_PPC_LLP_COS_PORT_INFO port_info;
   int port_i, core;
   int soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_PORT soc_ppd_port;
   _bcm_dpp_gport_info_t gport_info;
   BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);
   /* verify parameters */
   if(priority > SOC_SAND_PP_TC_MAX) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Port %d priority %d out of range, unit %d"),FUNCTION_NAME(), port, priority, unit));
   }

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

       soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, core, soc_ppd_port, &port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       port_info.default_tc = priority;

       soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit, core, soc_ppd_port, &port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   }

exit:
   BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
   SOC_PPC_LLP_COS_PORT_INFO port_info;
   int port_i;
   int soc_sand_rv = 0;
   int rv = BCM_E_NONE, core;
   SOC_PPC_PORT soc_ppd_port;
   _bcm_dpp_gport_info_t gport_info;
   BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

       soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, core, soc_ppd_port, &port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       *priority = port_info.default_tc;

       break;
   }

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_cfi_color_set
 * Purpose:
 *      Specify the color selection for the given CFI.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      cfi -   Canonical format indicator (TRUE/FALSE) 
 *      color - color assigned to packets with indicated CFI.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_petra_port_cfi_color_set(int unit, bcm_port_t port,
                           int cfi, bcm_color_t color)
{
    bcm_error_t rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv, tbl_index, entry_index;
    int dp;    
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO entry;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&entry);

    /* In dune devices, mapping is device wide. Port must be of -1 */
    if(port != -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("%s: global configuration, port(%d) has to be -1, unit %d"),FUNCTION_NAME(), port, unit));
    }

    /* Verify cfi & color */
    if ((cfi != 0) && (cfi != 1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid cfi %d"), cfi));
    }

    rv = _bcm_petra_port_color_encode(unit,color,&dp);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_dev_id = (unit);

    /* set entry. table index is always 0 (Global table) */
    tbl_index = BCM_PETRA_PORT_CFI_COLOR_GLOBAL_ENTRY;

    /* Entry index = CFI. entry.value1 = DP */
    entry_index = cfi;    
    entry.value1 = dp;

    soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(soc_sand_dev_id,SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP,tbl_index,entry_index,&entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_cfi_color_get
 * Purpose:
 *      Get the color selection for the given CFI.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      cfi -   Canonical format indicator (TRUE/FALSE) 
 *      color - (OUT) color assigned to packets with indicated CFI.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_petra_port_cfi_color_get(int unit, bcm_port_t port,
                           int cfi, bcm_color_t *color)
{
    bcm_error_t rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv, tbl_index, entry_index;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO entry;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&entry);

    BCMDNX_NULL_CHECK(color);

    /* In dune devices, mapping is device wide. Port must be of -1 */
    if(port != -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("%s: global configuration, port(%d) has to be -1, unit %d"),FUNCTION_NAME(), port, unit));
    }

    /* Verify cfi */
    if ((cfi != 0) && (cfi != 1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid cfi %d"), cfi));
    }

    soc_sand_dev_id = (unit);

    /* get entry. table index is always 0 (Global table) */
    tbl_index = BCM_PETRA_PORT_CFI_COLOR_GLOBAL_ENTRY;

    /* Entry index = CFI. entry.value1 = DP */
    entry_index = cfi;
    
    soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_get(soc_sand_dev_id,SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP,tbl_index,entry_index,&entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_petra_port_color_decode(unit,entry.value1,color);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* map color to hw value DP*/
int 
_bcm_petra_port_color_encode(
    int unit,
    bcm_color_t color,
    int *dp)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (color < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid color %d"), color));
    }

    switch(color) {
    case bcmColorGreen:
        *dp = _BCM_DPP_COLOR_GREEN;
        break;
    case bcmColorYellow:
        *dp = _BCM_DPP_COLOR_YELLOW;
        break;
    case bcmColorRed:
        *dp = _BCM_DPP_COLOR_RED;
        break;
    case bcmColorBlack:
        *dp = _BCM_DPP_COLOR_BLACK;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid color %d"),color));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* map hw value DP to color*/
int 
_bcm_petra_port_color_decode(
    int unit,
    int dp,
    bcm_color_t *color)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (dp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid dp %d"), dp));
    }

    switch(dp) {
    case _BCM_DPP_COLOR_GREEN:
        *color = bcmColorGreen;
        break;
    case _BCM_DPP_COLOR_YELLOW:
        *color = bcmColorYellow;
        break;
    case _BCM_DPP_COLOR_RED:
        *color = bcmColorRed;
        break;
    case _BCM_DPP_COLOR_BLACK:
        *color = bcmColorBlack;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid dp %d"),dp));
    }

exit:
    BCMDNX_FUNC_RETURN;
}




static int _Bcm_dpp_port_map_nof_tbls[_BCM_DPP_PORT_MAP_MAX_NOF_TBLS] = {4,4,2,2,2,0,0,0};
static int _Bcm_dpp_port_map_nof_entries[_BCM_DPP_PORT_MAP_MAX_NOF_TBLS] = {8,8,256,2,2,0,0,0};

/* Common vlan domain (6b value). All ports will be on this vlan domain, so when doing
 * simply bridging, in-AC will be common_vd*vid */


/* 0,1,2,3: PV*/
/* 3,4,5,6: PVV*/
/* Forward indication for defines */
#define _BCM_DPP_PP_FWD_NONE (0x1)
#define _BCM_DPP_PP_FWD_DROP (0x2)
#define _BCM_DPP_PP_FWD_CPU  (0x4)

STATIC int
  _bcm_petra_port_pp_trap_code_get_and_clear(int unit,
                                             SOC_PPC_TRAP_CODE trap_code, 
                                             SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *trap_info,
                                             SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_action) 
{
  uint32 soc_sand_rv, soc_sand_dev_id;
  bcm_error_t rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(trap_info);
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(snoop_action);
  soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id,trap_code,trap_info);
  rv = handle_sand_result(soc_sand_rv);
  if (rv != BCM_E_NONE) {
    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: trap code %d failed obtain trap info, unit %d"),FUNCTION_NAME(), trap_code, unit));
  }
  soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(soc_sand_dev_id,trap_code,snoop_action);
  rv = handle_sand_result(soc_sand_rv);
  if (rv != BCM_E_NONE) {
    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: trap code %d failed obtain snoop info, unit %d"),FUNCTION_NAME(), trap_code, unit));
  }
exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_port_pp_trap_code_fwd_set(int unit,
                                       SOC_PPC_TRAP_CODE trap_code, 
                                       uint32 fwd_indication,
                                       SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *trap_info,
                                       SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_action) 
{

  uint32 soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;
  unit = (unit);

  if (fwd_indication == _BCM_DPP_PP_FWD_DROP) {
    trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info->dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  } else if (fwd_indication == _BCM_DPP_PP_FWD_CPU) {
    trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    SOC_PPD_FRWRD_DECISION_LOCAL_CPU_SET(unit, &(trap_info->dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info->dest_info.frwrd_dest.dest_id = _BCM_DPP_CPU_DEST_PORT;
  } else {
    /* _BCM_PORT_PP_FWD_NONE */
    trap_info->bitmap_mask &= ~SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_port_pp_trap_code_trap_set(int unit,
                                       SOC_PPC_TRAP_CODE trap_code, 
                                       int is_trap,
                                       SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *trap_info,
                                       SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_action) 
{
  uint32 soc_sand_rv, soc_sand_dev_id;
  bcm_error_t rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  if (is_trap) {
    snoop_action->strength = (SOC_DPP_CONFIG(unit)->pp.default_snoop_strength);
    snoop_action->snoop_cmnd = _BCM_DPP_SNOOP_CMD_TO_CPU;
  } else {
    snoop_action->strength = 0;
  }
  soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id,trap_code,snoop_action);
  rv = handle_sand_result(soc_sand_rv);
  if (rv != BCM_E_NONE) {
    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: set snoop profile trap code %d failed, unit %d"),FUNCTION_NAME(), trap_code, unit));
  }
exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_port_pp_trap_code_mact_trap_info_set(int unit,
                                       SOC_PPC_FRWRD_MACT_TRAP_TYPE trap_type,
                                       SOC_PPC_TRAP_CODE trap_code,    
                                       _bcm_petra_port_mact_mgmt_action_profile_t action_profile_ndx,
                                       SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *trap_info,
                                       SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_action) 
{
  uint32 soc_sand_rv, soc_sand_dev_id;
  bcm_error_t rv = BCM_E_NONE;
  SOC_PPC_ACTION_PROFILE action_profile;

  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  SOC_PPC_ACTION_PROFILE_clear(&action_profile);

  action_profile.frwrd_action_strength = trap_info->strength;
  action_profile.snoop_action_strength = snoop_action->strength;
  action_profile.trap_code = trap_code;
  soc_sand_rv = soc_ppd_frwrd_mact_trap_info_set(soc_sand_dev_id, trap_type, action_profile_ndx, &action_profile);
  rv = handle_sand_result(soc_sand_rv);
  if (rv != BCM_E_NONE) {
    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: set frwrd mact trap info for trap code %d failed, unit %d"),FUNCTION_NAME(), trap_code, unit));
  }
exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_port_trap_default_set(int unit,
                                   bcm_port_t port)
{
    uint32 soc_sand_rv, soc_sand_dev_id;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_PORT_INFO port_info;
    int core;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id,core,soc_ppd_port,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    port_info.initial_action_profile.trap_code = _BCM_PETRA_UD_DFLT_TRAP;
    port_info.flags = SOC_PPC_PORT_IHP_PINFO_LLR_TBL;
    soc_sand_rv = soc_ppd_port_info_set(soc_sand_dev_id,core,soc_ppd_port,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_port_traps_init(int unit)
{
    /* traps */
    SOC_PPC_TRAP_CODE
    trap_code;
    int hw_trap_code = 0;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO
    trap_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO
    snoop_action;
    
    uint32 soc_sand_rv, flags;
    int rv = BCM_E_NONE, core;
    SOC_PPC_EG_FILTER_PORT_INFO eg_filter_port_info;
    bcm_port_t port;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_PORT soc_ppd_port;
    pbmp_t ports_map;

    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);

    /* init drop trap to drop packets*/
    trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    rv = handle_sand_result(soc_sand_rv);
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* drop packet that came from the same interface (ingress filter) */
    trap_code = SOC_PPC_TRAP_CODE_SAME_INTERFACE;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    rv = handle_sand_result(soc_sand_rv);
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

    /* SA-drop to drop packet */
    trap_code = SOC_PPC_TRAP_CODE_SA_DROP_1;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* set last user define trap code to drop packets */
    trap_code = _BCM_PETRA_UD_DROP_TRAP;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit, trap_code, &hw_trap_code));
    rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &hw_trap_code);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* 1+1 protection failover drop */
    trap_code = SOC_PPC_TRAP_CODE_LIF_PROTECT_PATH_INVALID;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* init accept trap to NOP */
    trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    trap_info.strength = 0; /* as this just to accept, no drop here, VTT default strength*/
    trap_info.bitmap_mask &= ~SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* drop packet that came from the same interface (eg filter) */
   BCM_PBMP_ASSIGN(ports_map, PBMP_E_ALL(unit));
   BCM_PBMP_ITER(ports_map, port) {

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))
        {
            continue;
        }
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

        soc_sand_rv =soc_ppd_eg_filter_port_info_get(unit,core,soc_ppd_port,&eg_filter_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        eg_filter_port_info.filter_mask |= (SOC_PPC_EG_FILTER_PORT_ENABLE_MTU|SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE|SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON);
        soc_sand_rv =soc_ppd_eg_filter_port_info_set(unit,core,soc_ppd_port,&eg_filter_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        port_info.enable_same_interfac_filter = TRUE;
        port_info.same_interface_filter_skip = FALSE;
        port_info.initial_action_profile.trap_code = _BCM_PETRA_UD_DFLT_TRAP;
        port_info.flags = (SOC_PPC_PORT_IHP_PINFO_LLR_TBL |
                           SOC_PPC_PORT_IHB_PINFO_FLP_TBL |
                           SOC_PPC_PORT_EGQ_PP_PPCT_TBL);
        soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

    }
   /* 
    * set TM ports to default trap profile
    * all TM ports 
    * RCY ports 
    * CPU TM ports 
    */
    BCM_PBMP_ASSIGN(ports_map, PBMP_PORT_ALL(unit));
    BCM_PBMP_OR(ports_map, SOC_INFO(unit).cmic_bitmap);
    BCM_PBMP_OR(ports_map, PBMP_RCY_ALL(unit));
    BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
    BCM_PBMP_REMOVE(ports_map, PBMP_E_ALL(unit));
    BCM_PBMP_ITER(ports_map, port) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))
        {
            continue;
        }
        soc_sand_rv = _bcm_petra_port_trap_default_set(unit, port);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* Init Traps for Mact-Mgmt that is PBP-SA-DROP & DA-Not-Found  { */
    /* Used in Port Control bcmPortControlDiscardMacsaAction, bcmPortControlUnknownMacDaAction */

    /* Init Traps to NONE i.e. No trap effect, switch packet (fwd) */
    trap_code = SOC_PPC_TRAP_CODE_SA_DROP_0 + _bcm_petra_port_mact_mgmt_action_profile_none;
    rv = _bcm_petra_port_pp_trap_code_get_and_clear(unit,trap_code,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_fwd_set(unit,trap_code,_BCM_DPP_PP_FWD_NONE,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_trap_set(unit,trap_code,FALSE,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_mact_trap_info_set(unit,SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP,trap_code,_bcm_petra_port_mact_mgmt_action_profile_none,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Init Traps to DROP i.e. Only Drop packets */
    trap_code = SOC_PPC_TRAP_CODE_SA_DROP_0 + _bcm_petra_port_mact_mgmt_action_profile_drop;
    rv = _bcm_petra_port_pp_trap_code_get_and_clear(unit,trap_code,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_fwd_set(unit,trap_code,_BCM_DPP_PP_FWD_DROP,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_trap_set(unit,trap_code,FALSE,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_mact_trap_info_set(unit,SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP,trap_code,_bcm_petra_port_mact_mgmt_action_profile_drop,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);

    

    /* Init Traps to TRAP & DROP i.e. Forward packet to CPU */
    trap_code = SOC_PPC_TRAP_CODE_SA_DROP_0 + _bcm_petra_port_mact_mgmt_action_profile_trap;
    rv = _bcm_petra_port_pp_trap_code_get_and_clear(unit,trap_code,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_fwd_set(unit,trap_code,_BCM_DPP_PP_FWD_DROP,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_trap_set(unit,trap_code,TRUE,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_mact_trap_info_set(unit,SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP,trap_code,_bcm_petra_port_mact_mgmt_action_profile_trap,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);

    

    /* Init Traps to TRAP i.e. Drop packet and Copy packet to CPU (snooping) */
    trap_code = SOC_PPC_TRAP_CODE_SA_DROP_0 + _bcm_petra_port_mact_mgmt_action_profile_trap_and_drop;
    rv = _bcm_petra_port_pp_trap_code_get_and_clear(unit,trap_code,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_fwd_set(unit,trap_code,_BCM_DPP_PP_FWD_NONE,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_trap_set(unit,trap_code,TRUE,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_port_pp_trap_code_mact_trap_info_set(unit,SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP,trap_code,_bcm_petra_port_mact_mgmt_action_profile_trap_and_drop,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Init Traps for Mact-Mgmt that is PBP-SA-DROP & DA-Not-Found  } */


    /* Init Traps for Mact-Mgmt when SA-not-found { */

    /* 1 LEARN and drop */
    trap_code = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2;
    rv = _bcm_petra_port_pp_trap_code_fwd_set(unit,trap_code,_BCM_DPP_PP_FWD_DROP,&trap_info,&snoop_action);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* 2 don't LEARN and Forward */
    trap_code = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    rv = handle_sand_result(soc_sand_rv);
    trap_info.processing_info.enable_learning = 0;
    trap_info.strength = SOC_DPP_CONFIG(unit)->pp.default_trap_strength;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* 3 don't LEARN and Drop */
    trap_code = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    rv = handle_sand_result(soc_sand_rv);
    trap_info.processing_info.enable_learning = 0;
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


exit:
    BCMDNX_FUNC_RETURN;
}

typedef enum _bcm_petra_tpid_op_e{
    _bcm_petra_tpid_op_outer_set=0,
    _bcm_petra_tpid_op_outer_add,
    _bcm_petra_tpid_op_outer_delete,
    _bcm_petra_tpid_op_outer_delete_all,
    _bcm_petra_tpid_op_inner_set,
    _bcm_petra_tpid_op_inner_remove,
    _bcm_petra_tpid_op_outer_add_order /* this mode is for advanced mode, when called in port then change 
                                        _bcm_petra_tpid_profile_none -> _bcm_petra_tpid_profile_outer -> _bcm_petra_tpid_profile_outer_inner -> ERROR
    */
} _bcm_petra_tpid_op_t;

/* given tpid profile, return TPID values assigned on this profile */
STATIC int
_bcm_petra_tpid_class_assigned_tpids_get(int unit, uint32 tpid_profile,
                uint16 tpids[_BCM_PETRA_PORT_NOF_TPID_VALS], uint32 *nof_tpids)
{
   uint32 tpid_indx=0;
   SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
   SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO tpid_profile_info;
   int soc_sand_dev_id, soc_sand_rv = 0;

   /* Retrive local PP ports */
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  *nof_tpids = 0;
  /* get TPID values */
  soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(soc_sand_dev_id, &tpid_vals);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  /* get profie TPIDS */
  soc_sand_rv = soc_ppd_llp_parse_tpid_profile_info_get(soc_sand_dev_id,
                                             tpid_profile, &tpid_profile_info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  tpids[tpid_indx++] = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
  tpids[tpid_indx++] = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];

  /* get MIM TPID */
  tpids[tpid_indx++] = tpid_vals.tpid_vals[4];
  *nof_tpids = tpid_indx;

  LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit, "tpid_class_assigned_tpids_get: "
              "tpid_profile=%d tpid1=0x%x tpid2=0x%x tpid3=0x%x\n"),
              tpid_profile, tpids[0], tpids[1],tpids[2]));


exit:
   BCMDNX_FUNC_RETURN;
}

/* given
    -  port and two TPIDs values (port_tpid_class) 
    -  TPIDs assigned on tpid_profile
   return
    - relevant TPID indexes to be used in tag structure
 
    2.  TPID outer/inner:
        a.  Given port,TPID1 and array of <TPID indexes >
            i.  0: if TPID = BCM_PORT_TPID_CLASS_TPID_INVALID
            ii. {0,1,2,3}: if TPID = BCM_PORT_TPID_CLASS_TPID_ANY
            iii.1: TPID = first TPID of port TPID profile
            iv. 2: TPID = second TPID of port TPID profile
        v.  3: TPID = MIM TPID
        vi. Error otherwise

 */


STATIC int
_bcm_petra_port_tpid_indexes_get(int unit, SOC_PPC_PORT ppd_port_id,
                    uint32 tpid_val, uint16 *tpid_vals, uint32 nof_tpid_vals,
                    uint16 tpid_indexes[_BCM_PETRA_PORT_NOF_TPID_VALS]/* out */,
                    uint32 *nof_tpid_indexes)
{
    uint32 tpid_indx=0;
    uint32 tpid_val_indx=0;

    /* Retrive local PP ports */
    BCMDNX_INIT_FUNC_DEFS;


    *nof_tpid_indexes = 0;
    /* invalid ==> no TPID found */
    if (tpid_val == BCM_PORT_TPID_CLASS_TPID_INVALID) {
        tpid_indexes[tpid_indx++] = 0;
        *nof_tpid_indexes = tpid_indx;
        BCM_EXIT;
    }

    /* any ==> ALL */
    if (tpid_val == BCM_PORT_TPID_CLASS_TPID_ANY) {
        tpid_indexes[tpid_indx++] = 0;
        tpid_indexes[tpid_indx++] = 1;
        tpid_indexes[tpid_indx++] = 2;
        tpid_indexes[tpid_indx++] = 3;
        *nof_tpid_indexes = tpid_indx;
        BCM_EXIT;
    }

    for (tpid_val_indx = 0; tpid_val_indx < nof_tpid_vals; ++tpid_val_indx) {
        if (tpid_val == tpid_vals[tpid_val_indx]) {
            /* +1: as 0 means no TPID, 1 means TPID 0, etc.. */
            tpid_indexes[tpid_indx++] = tpid_val_indx+1;
            *nof_tpid_indexes = tpid_indx;
            LOG_VERBOSE(BSL_LS_BCM_PORT,(BSL_META_U(unit,
                        "tpid_indexes_get: u=%d p=0x%x *Num:%d "
                        "indx1=0x%x indx2=0x%x indx3=0x%x indx4=0x%x\n"),
                        unit, ppd_port_id, *nof_tpid_indexes, tpid_indexes[0],
                        tpid_indexes[1],tpid_indexes[2],tpid_indexes[3]));
          BCM_EXIT;
      }
  }
  /* user specify explicit TPID value that is not assigned in port */
  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                      (_BSL_BCM_MSG("TPID:0x%08x is not assigned in port:0x%08x"),
                      tpid_val,ppd_port_id));

exit:
   BCMDNX_FUNC_RETURN;
}

/* map bcm tpid_class to PPD LLP_PARSE */
STATIC int
_bcm_petra_port_tpid_class_info_to_ppd(
       int unit,
       bcm_port_tpid_class_t *tpid_class,
       SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *pkt_frmt_info
    )
{
  /* Retrive local PP ports */
  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(pkt_frmt_info);

  pkt_frmt_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_NONE;
  pkt_frmt_info->dlft_edit_command_id = tpid_class->vlan_translation_action_id;
  pkt_frmt_info->dflt_edit_pcp_profile = tpid_class->vlan_translation_qos_map_id;

  if (tpid_class->flags & BCM_PORT_TPID_CLASS_DISCARD) {
      pkt_frmt_info->action_trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
  }
  else{
      pkt_frmt_info->action_trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
  }
  pkt_frmt_info->tag_format = tpid_class->tag_format_class_id;

  
  if (tpid_class->flags & BCM_PORT_TPID_CLASS_INNER_C) {
      pkt_frmt_info->flags = SOC_PPC_LLP_PARSE_FLAGS_INNER_C;
  }
  if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_C) {
      pkt_frmt_info->flags = SOC_PPC_LLP_PARSE_FLAGS_OUTER_C;
  }

   BCMDNX_FUNC_RETURN;
}


/* map bcm tpid_class from PPD LLP_PARSE */
STATIC int
_bcm_petra_port_tpid_class_info_from_ppd(
       int unit,
       bcm_port_tpid_class_t *tpid_class,
       SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *pkt_frmt_info
    )
{
  /* Retrive local PP ports */
  BCMDNX_INIT_FUNC_DEFS;

  /* clear old data (except "key" data) */
  tpid_class->flags &= ~(BCM_PORT_TPID_CLASS_DISCARD|BCM_PORT_TPID_CLASS_OUTER_C|BCM_PORT_TPID_CLASS_INNER_C);

  tpid_class->vlan_translation_action_id = pkt_frmt_info->dlft_edit_command_id;
  tpid_class->vlan_translation_qos_map_id = pkt_frmt_info->dflt_edit_pcp_profile;
  tpid_class->tag_format_class_id = pkt_frmt_info->tag_format;

  if (pkt_frmt_info->action_trap_code == SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP) {
      tpid_class->flags |= BCM_PORT_TPID_CLASS_DISCARD;
  }
  
  if (pkt_frmt_info->flags == SOC_PPC_LLP_PARSE_FLAGS_INNER_C) {
      tpid_class->flags |= BCM_PORT_TPID_CLASS_INNER_C;
  }
  if (pkt_frmt_info->flags == SOC_PPC_LLP_PARSE_FLAGS_OUTER_C) {
      tpid_class->flags |= BCM_PORT_TPID_CLASS_OUTER_C;
  }

   BCMDNX_FUNC_RETURN;
}

/* given bcm_port_tpid_class_t,
   compress data into bitmap to be used in template management
   assume compress buffer data is less than 32 bit
   For ingress:
    0:0  PcpDeiProfile  This is the port-based PCP-DEI mapping profile
    6:1  Ivec  This is the port-based ingress VLAN edit command. 19:16
    10:7 IncomingTagStructure  Used together with the VLAN-Edit-Profile
    11:11 discard
    12:12: outer-cep
    13:13 inner-cep

   For Egress:
    6:0 Reserved.
    10:7 IncomingTagFomat Used together with the VLAN-Edit-Profile.
    11:11 Reserved
    12:12 outer-cep
    13:13 inner-cep

   where:
    PcpDeiProfile  = qos_map_id
    Ivec = vlan_translation_action_id
    IncomingTagStructure = bcm_port_tag_format_class_t
    Discard = BCM_PORT_TPID_CLASS_DISCARD
    Outer cep: BCM_PORT_TPID_CLASS_OUTER_C
    Inner cep: BCM_PORT_TPID_CLASS_INNER_C

 */
STATIC int
_bcm_petra_port_tpid_class_info_compress_get(
       int unit,
       bcm_port_tpid_class_t *tpid_class,
       uint32 *buff,
       uint32 *buff_eg)
{
   uint32 tmp_val = 0;
   uint32 buff_val;

  /* Retrive local PP ports */
  BCMDNX_INIT_FUNC_DEFS;

  *buff = 0;
  *buff_eg = 0;
  buff_val = 0;

  /* 13:13 inner-cep */
  tmp_val = (tpid_class->flags & BCM_PORT_TPID_CLASS_INNER_C)?1:0;
  buff_val |= tmp_val<<13; 

  /* 12:12 outer-cep */
  tmp_val = (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_C)?1:0;
  buff_val |= tmp_val<<12;

  /* 11:11 discard */
  tmp_val = (tpid_class->flags & BCM_PORT_TPID_CLASS_DISCARD)?1:0;
  buff_val |= tmp_val<<11;

  /* 10:7 IncomingTagStructure */
  tmp_val = tpid_class->tag_format_class_id;
  buff_val |= tmp_val<<7;

  /* 6:1  Ivec */
  tmp_val = tpid_class->vlan_translation_action_id;
  buff_val |= tmp_val<<1;

  /* 0:0  PcpDeiProfile  */
  tmp_val = tpid_class->vlan_translation_qos_map_id;
  buff_val |= tmp_val;

  /*For ingress, all bits are needed. For egress, only TagFormat and cep bits are needed.*/
  *buff = buff_val;
  *buff_eg = buff_val & _BCM_DPP_PORT_VLAN_CLASSIFICATION_TAG_FORMAT_CEP_MASK;

  LOG_VERBOSE(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "tpid_class_info_compress_get: buff_val=0x%x\n"),
               buff_val));
   BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_port_tpid_tag_structure_to_index(
       int unit,
       SOC_PPC_LLP_PARSE_INFO *prs_nfo,
       uint32 *index
    )
{
    
  /* Retrive local PP ports */
  BCMDNX_INIT_FUNC_DEFS;

  *index = prs_nfo->outer_tpid << 3;
  *index |= prs_nfo->is_outer_prio << 2;
  *index |= prs_nfo->inner_tpid;

   BCMDNX_FUNC_RETURN;
}
 


/* update the tpid_class_buffer to include tag_strucutre_buff in the given tag_strucutre_index
   tpid_class_buffer will be used as profile data in template manager
 */
STATIC int
_bcm_petra_port_tpid_class_info_buffer_update(
       int unit,
       uint32 tag_strucutre_index,  /* actualy tag-structure (output of parser */
       uint32 tag_strucutre_buff,    /* change data */
       _bcm_dpp_port_tpid_class_info_t *tpid_class_buffer
    )
{
  /* Retrive local PP ports */
  BCMDNX_INIT_FUNC_DEFS;


  SHR_BITCOPY_RANGE(
                    tpid_class_buffer->buff,
                    _BCM_DPP_PORT_TPID_CLASS_BITS_PER_TAG_STRCT*tag_strucutre_index,
                    &tag_strucutre_buff,
                    0,
                    _BCM_DPP_PORT_TPID_CLASS_BITS_PER_TAG_STRCT);


   BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_pp_init(int unit)
{  
    uint32 trap_indx = 0;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO snp_to_cpu;

    /*SOC_TMC_ACTION_CMD_SNOOP_INFO snoop_tmp;*/

    bcm_dpp_snoop_t snoop_tmp;

    uint32 snoop_indx = 0;
    uint32 idx;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    _bcm_petra_tpid_profile_t profile_type;
    _bcm_petra_dtag_mode_t dtag_mode;
    _bcm_petra_ac_key_map_type_t ac_key_map_type;
    bcm_dpp_user_defined_traps_t ud_trap_info;
    int accept_mode;
    uint32 tag_buff, tag_buff_eg;
    int port_i, gport;
    bcm_port_tpid_class_t port_tpid_class;
    int template_init_id;
    SOC_PPC_LLP_PARSE_INFO prs_nfo;
    _bcm_petra_port_mact_mgmt_action_profile_t action_profile;
    _bcm_dpp_port_tpid_class_info_t tpid_class_init_data;
    _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t
        tpid_class_acceptable_frame_info;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (SOC_DPP_PP_ENABLE(unit)) {
        /* by default all PORTS points to tpid profile 0 and accept untagged only*/
        profile_type = _bcm_petra_tpid_profile_none;

        accept_mode = BCM_PORT_DISCARD_TAG;
        dtag_mode = _bcm_petra_dtag_mode_accept;
        ac_key_map_type = _bcm_petra_ac_key_map_type_normal;

        if (!SOC_WARM_BOOT(unit)) {
            rv = _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t_clear(unit, &tpid_class_acceptable_frame_info);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_template_tpid_profile_init(unit,0,profile_type,accept_mode,dtag_mode, ac_key_map_type);
            BCMDNX_IF_ERR_EXIT(rv);
            if(_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
                bcm_port_tpid_class_t_init(&port_tpid_class);
                /* 3. calculate new buffer data for this tag class */
                rv = _bcm_petra_port_tpid_class_info_compress_get(
                        unit,
                        &port_tpid_class,
                        &tag_buff,
                        &tag_buff_eg
                     );
                BCMDNX_IF_ERR_EXIT(rv);

                SOC_PPC_LLP_PARSE_INFO_clear(&prs_nfo);

                /* run over all relevant tag strucutres */
                for(prs_nfo.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE; prs_nfo.outer_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS; ++prs_nfo.outer_tpid) {
                    for(prs_nfo.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE; prs_nfo.inner_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS; ++prs_nfo.inner_tpid) {
                        for (prs_nfo.is_outer_prio = FALSE; prs_nfo.is_outer_prio <= TRUE; ++prs_nfo.is_outer_prio) {
                            /* map parser result to index in buffer */
                            rv = _bcm_petra_port_tpid_tag_structure_to_index(unit,&prs_nfo,&idx);
                            BCMDNX_IF_ERR_EXIT(rv);

                            /* update data in buffer */
                            rv = _bcm_petra_port_tpid_class_info_buffer_update(unit,idx,tag_buff,&tpid_class_init_data);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }
                }

                /* update data in buffer. tpid_class_init_data is applicable to both ingress and egress sides at init stage.*/
                rv = _bcm_dpp_am_template_port_tpid_class_init(unit, 0, &tpid_class_init_data);
                BCMDNX_IF_ERR_EXIT(rv);

                /* get data from tpid class info and fill egress tpid class info acceptable
                   frame type*/
                _bcm_dpp_port_tpid_class_info_t_to_bcm_dpp_port_tpid_class_acceptable_frame_type_info_t(unit, &tpid_class_init_data, &tpid_class_acceptable_frame_info);

                /* update date in buffer */
                rv = _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_init(unit, 0, &tpid_class_acceptable_frame_info);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* by default all points to entry 0 */
    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&(snoop_tmp.snoop_action_info));
    snoop_tmp.snoop_action_info.prob = (uint32)-1;
    snoop_tmp.snoop_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING;

    snoop_indx = 0;
    rv = _bcm_dpp_am_template_snoop_cmd_init(unit,snoop_indx,&snoop_tmp);
    BCMDNX_IF_ERR_EXIT(rv);

    ud_trap_info.snoop_cmd = 0;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(ud_trap_info.trap));
    ud_trap_info.ud_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING; 
    ud_trap_info.gport_trap_id = 0;

    rv = _bcm_dpp_am_template_user_defined_traps_init(unit,_BCM_PETRA_UD_DFLT_TRAP - SOC_PPC_TRAP_CODE_USER_DEFINED_0,&ud_trap_info);
    BCMDNX_IF_ERR_EXIT(rv);

    for(idx=0; idx < _BCM_PETRA_PORT_LEARN_NOF_TRAPS; ++idx) {

        rv = PORT_ACCESS.dpp_port_config.trap_to_flag.set(unit,
                idx,
                (idx == trap_indx) ? BCM_PETRA_PORT_LEARN_DFLT_FLGS : BCM_PETRA_PORT_LEARN_INVALID_FLGS);
        BCMDNX_IF_ERR_EXIT(rv);

    }

    /* set snoop command '_BCM_DPP_SNOOP_CMD_TO_CPU' to send packet to CPU*/
    
    if (!SOC_IS_JERICHO(unit)) 
    {
        if (!SOC_WARM_BOOT(unit)) {
         
            SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&snp_to_cpu);

            snp_to_cpu.cmd.dest_id.type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
            snp_to_cpu.cmd.dest_id.id = _BCM_DPP_CPU_DEST_PORT;
            snp_to_cpu.prob = 1023;
            snp_to_cpu.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_snoop_set,(unit,_BCM_DPP_SNOOP_CMD_TO_CPU,&snp_to_cpu)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }
    }

    if (SOC_DPP_PP_ENABLE(unit)) {
        if (!SOC_WARM_BOOT(unit)) {
            /*
             * init traps
             */
            rv = _bcm_petra_port_traps_init(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /*
         * init SW states, mapping tables
         */

        /* profile 0 is used*/
        for(idx = 0; idx < _bcm_dpp_port_map_type_count; ++idx ) {
            rv = PORT_ACCESS.dpp_port_config.map_tbl_use.set(unit, idx, 0, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* allocate SW databases*/
        if (!SOC_WARM_BOOT(unit)) {

            if (!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
                /* set all ports to accept untagged packets only */
                BCM_PBMP_ITER(PBMP_E_ALL(unit),port_i){
                    BCM_GPORT_LOCAL_SET(gport,port_i);
                    rv = bcm_petra_port_discard_set(unit,gport,BCM_PORT_DISCARD_TAG);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

        /* Init TRAPs Mact mgmt SA-Drop, Unknown-DA. Init to forward (NONE) */
        template_init_id = _bcm_petra_port_mact_mgmt_action_profile_none;
        action_profile = _bcm_petra_port_mact_mgmt_action_profile_none;
        rv = _bcm_dpp_am_template_port_mact_sa_drop_init(unit,template_init_id,&action_profile);
        BCMDNX_IF_ERR_EXIT(rv);

       
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * find map-profile for the given port/s
 */
int 
_bcm_petra_port_map_alloc_profile(
    int unit, 
    int type,
    bcm_pbmp_t pbmp_local_ports, 
    int flags, 
    int modified_entry_indx,
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *new_entry_val,
    int *profile,
    int *new_profile
    )
{
   SOC_PPC_LLP_COS_PORT_INFO    cos_port_info;
   int found_first=FALSE, port_idx, my_profile=0xffff;
   int tbl_indx,nof_tbls,nof_entries,entry_indx;
   int reuse_profile = TRUE, reuse_self = TRUE;
   int soc_sand_dev_id = unit, soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    entry , my_entry;
   SOC_PPC_LLP_COS_MAPPING_TABLE map_table_type = 0;
   SOC_PPC_PORT soc_ppd_port_idx;
   bcm_port_t port_from_ppd;
   int map_tbl_use, core;
   soc_port_t  ether_port;

   BCMDNX_INIT_FUNC_DEFS;
   soc_sand_dev_id = (unit);

   *new_profile = FALSE;
   BCM_PBMP_ITER(pbmp_local_ports, port_idx) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_idx, &soc_ppd_port_idx, &core)));

       soc_sand_rv = soc_ppd_llp_cos_port_info_get(soc_sand_dev_id,core,soc_ppd_port_idx,&cos_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       if(!found_first) {
       switch (type) {
           case _bcm_dpp_port_map_type_up_to_dp:
           my_profile = cos_port_info.l2_info.tbls_select.up_to_dp_index;
           map_table_type = SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP;
           break;
           case _bcm_dpp_port_map_type_in_up_to_tc_and_de:
           my_profile = cos_port_info.l2_info.tbls_select.in_up_to_tc_and_de_index;
           map_table_type = SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC;
           break;
           default:
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: unknown port map type %d, unit %d"),FUNCTION_NAME(), type, unit));
       }
       found_first = TRUE;
       }
       /* all ports in the group must has same profile*/
       else if (found_first &&
      (cos_port_info.l2_info.tbls_select.up_to_dp_index != my_profile) &&
      (cos_port_info.l2_info.tbls_select.in_up_to_tc_and_de_index != my_profile)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: all ports in the group must has same profile, unit %d"),FUNCTION_NAME(), unit));
       }
   }

   /*pbmp is empty*/
   if(!found_first) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("pbmp is empty")));
   }

   nof_tbls = _Bcm_dpp_port_map_nof_tbls[type];
   nof_entries = _Bcm_dpp_port_map_nof_entries[type];

   /* check if there is table with new mapping, if so reuse it*/
   for(tbl_indx = 0; tbl_indx < nof_tbls; ++tbl_indx) {
       /* skip self compare 
       if(my_profile == tbl_indx) {
       continue;
       }*/
       reuse_profile = TRUE;

       for(entry_indx = 0; entry_indx < nof_entries; ++entry_indx) {

           if(entry_indx != modified_entry_indx) {
               int table_id;
               if(my_profile != 0) {
                   table_id = my_profile;
               }
               else
               {
                   table_id = tbl_indx;
               }
               soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_get(soc_sand_dev_id,
                                     map_table_type,
                                     table_id,
                                     entry_indx,&my_entry);
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           }
           else
           {
               sal_memcpy(&my_entry,new_entry_val,sizeof(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO));
           }

           soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_get(soc_sand_dev_id,
                                 map_table_type,
                                 tbl_indx,
                                 entry_indx,&entry);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

           if(sal_memcmp(&my_entry,&entry,sizeof(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO)))
           {
               reuse_profile = FALSE;
               break;
           }
       }
       if(reuse_profile) {
           *profile = tbl_indx;
           break;
       }
   }

   /* check if my profile is used only be me, if so reuse it */
   /* if already move to another profile check i was last, so the profile is not longer in use*/
   PBMP_E_ITER(unit, ether_port) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, ether_port, &soc_ppd_port_idx, &core)));

       soc_sand_rv = soc_ppd_llp_cos_port_info_get(soc_sand_dev_id,core,soc_ppd_port_idx,&cos_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       /* for port uses this profile is it part of the group */
       if(found_first && 
        ((cos_port_info.l2_info.tbls_select.up_to_dp_index == my_profile) ||
         (cos_port_info.l2_info.tbls_select.in_up_to_tc_and_de_index == my_profile))) 
       {
           port_from_ppd = ether_port;
           if (!BCM_PBMP_MEMBER(pbmp_local_ports,port_from_ppd)) {
               reuse_self = FALSE;
               break;
           }
       }
   }

   /* if move to another profile (tbl_indx), and old profile (my_profile) not in use any more */
   if(reuse_profile && reuse_self && my_profile != tbl_indx) {
       rv = PORT_ACCESS.dpp_port_config.map_tbl_use.set(unit, type, my_profile, FALSE);
       BCMDNX_IF_ERR_EXIT(rv);
       BCM_EXIT;
   }

   /* the profile is used only by me, then can be resued*/
   if(reuse_self) {
       *profile = my_profile;
       BCMDNX_IF_ERR_EXIT(rv);
       BCM_EXIT;
   }

   /* If an existing profile is identical, can be reused*/
   if(reuse_profile) {
       *profile = tbl_indx;

       /* Mark the table as used if it's not already */
       rv = PORT_ACCESS.dpp_port_config.map_tbl_use.get(unit, type, tbl_indx, &map_tbl_use);
       BCMDNX_IF_ERR_EXIT(rv);       
       if(!map_tbl_use) {
           rv = PORT_ACCESS.dpp_port_config.map_tbl_use.set(unit, type, tbl_indx, TRUE);
           BCMDNX_IF_ERR_EXIT(rv);
       }

       BCMDNX_IF_ERR_EXIT(rv);
       BCM_EXIT;
   }


   /* if arrive here then allocate new-table */
   for(tbl_indx = 0; tbl_indx < nof_tbls; ++tbl_indx) {
       rv = PORT_ACCESS.dpp_port_config.map_tbl_use.get(unit, type, tbl_indx, &map_tbl_use);
       BCMDNX_IF_ERR_EXIT(rv);       
       if(!map_tbl_use) {
           rv = PORT_ACCESS.dpp_port_config.map_tbl_use.set(unit, type, tbl_indx, TRUE);
           BCMDNX_IF_ERR_EXIT(rv);
           *new_profile = TRUE;
           *profile = tbl_indx;
           break;
       }
   }
   if(*new_profile) {
       /* copy mapping from old to new table */
       for(entry_indx = 0; entry_indx < nof_entries; ++entry_indx) {
       /* set value from old table */
       if(entry_indx != modified_entry_indx) {
           soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_get(soc_sand_dev_id,
                                 map_table_type,
                                 my_profile,
                                 entry_indx,&my_entry);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       }
       else
       {
           sal_memcpy(&my_entry,new_entry_val,sizeof(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO));
       }
       /* set into new table */
       soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(soc_sand_dev_id,
                             map_table_type,
                             tbl_indx,
                             entry_indx,&my_entry);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       }

       BCMDNX_IF_ERR_EXIT(rv);
       BCM_EXIT;
   }

   /* if arrive here then not available */
   BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("not available")));

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_port_class_set_split_horizon
 * Purpose:
 *      Setting of auxilary table split horizon value for given lif
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      class_id - in Arad:
 *                 1 for HUB
 *                 0 for SPOKE (in Arad)
 *  
 *                 in Jericho:
 *                 0-3 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
_bcm_dpp_port_class_set_split_horizon(
   int unit,
   bcm_port_t port,
   uint32 class_id,
   int is_ingress) 
{
    uint32 soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 lif_eep_ndx = 0;
    int is_use_data_entry = 0;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;

    BCMDNX_INIT_FUNC_DEFS

    if ((soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 1) ||
        (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 2)) {
        if (class_id > 3) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("if SPLIT_HORIZON_FORWARDING_GROUPS_MODE is 1 or 2, class_id should be 0-3")));
        }
    } else {
        if (class_id > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("if SPLIT_HORIZON_FORWARDING_GROUPS_MODE is 0 or 3, class_id should be 0-1")));
        }
    }


    if (is_ingress) {
        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO),
            "_bcm_dpp_port_class_set_split_horizon.lif_info");
        if (lif_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
        }
        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);  

        soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            if (class_id == 0){
                lif_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
            }else {
                lif_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
            }
        } else if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF){
            if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                lif_info->value.mpls_term_info.orientation = (class_id & 1);
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                     (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION, ((class_id & 2) >> 1),
                                      &(lif_info->value.mpls_term_info.lif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            } else {
                if (class_id == 0){
                    lif_info->value.mpls_term_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
                }else {
                    lif_info->value.mpls_term_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
                }
            }

            /* update existing lif entry */
            soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_ISID){
            if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                lif_info->value.isid.orientation = (class_id & 1) == 1 ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                     (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION, ((class_id & 2) >> 1),
                                      &(lif_info->value.isid.lif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            } else {
                if (class_id == 0){
                    lif_info->value.isid.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
                }else {
                    lif_info->value.isid.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
                }
            }

            /* update existing lif entry */
            soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
        }

    } else { /* egress */

        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);          

        if (SOC_IS_JERICHO(unit)) {
            uint32 
                next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
                nof_entries;
            _bcm_lif_type_e lif_usage;
            int is_ipv6_tunnel = 0;
            uint32 outlif_profile = 0; 

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, gport_hw_resources.local_out_lif, NULL, &lif_usage));
            if ((lif_usage != _bcmDppLifTypeMplsTunnel)
                && (lif_usage != _bcmDppLifTypeLinkLayer)
                && (lif_usage != _bcmDppLifTypeIpTunnel)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS tunnel outlif LL outlif and IPv4 tunnel outLif are supported")));
            }

            BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "_bcm_dpp_port_class_set_split_horizon.encap_entry_info");
            if (encap_entry_info == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }

            /* get encap entry */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(
                unit,
                SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, gport_hw_resources.local_out_lif, 1,
                encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

            _bcm_dpp_port_outlif_profile_preset(unit,encap_entry_info,&outlif_profile);

            /* IPv6 tunnel */
            if ((encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) && (nof_entries > 1)) {
                is_ipv6_tunnel = 1;
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1))  {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, class_id,
                                          &outlif_profile));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }

                /* update is_l2_lif in outlif profile.
                                Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    /* set entry to be L2LIF */
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                                          &outlif_profile));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            } else {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                              (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, class_id,
                                  &outlif_profile));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            }            

            /* add mpls encap entry */
            if (lif_usage == _bcmDppLifTypeMplsTunnel) {
                encap_entry_info->entry_val.mpls_encap_info.outlif_profile = outlif_profile; 
                soc_sand_rv =
                    soc_ppd_eg_encap_mpls_entry_add(unit, gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.mpls_encap_info), next_eep[0]);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } 
            /* add LL encap entry */
            else if (lif_usage == _bcmDppLifTypeLinkLayer) {
                encap_entry_info->entry_val.ll_info.outlif_profile = outlif_profile; 
                soc_sand_rv =
                    soc_ppd_eg_encap_ll_entry_add(unit, gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.ll_info)); 
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            /* add Ip Tunnel encap entry */
            else if (lif_usage == _bcmDppLifTypeIpTunnel) {
                if (is_ipv6_tunnel) {
                    encap_entry_info->entry_val.data_info.outlif_profile = outlif_profile; 
                    soc_sand_rv = 
                        soc_ppd_eg_encap_data_lif_entry_add(unit, gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.data_info), TRUE, next_eep[0]);
                } else {
                    encap_entry_info->entry_val.ipv4_encap_info.outlif_profile = outlif_profile; 
                    soc_sand_rv = 
                        soc_ppd_eg_encap_ipv4_entry_add(unit, gport_hw_resources.local_out_lif, &(encap_entry_info->entry_val.ipv4_encap_info), next_eep[0]); 
                }
            }

        } else {
            lif_eep_ndx = gport_hw_resources.local_out_lif;
            if (SOC_IS_ARAD_B1_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->pp.pon_application_enable){
               /* EEDB format for PON out-lif in ARAD:
                *     1 data entry  --  store tunnel ID information, and the next-outlif is pointer to an out AC entry
                *     2 out ac entry --  store AC infromation such as vlan translated infos.
                *     Now gport_hw_resources.local_out_lif point out ac entry and gport_hw_resources.global_out_lif
                *     point data entry. and SPOKE/HUB configuration is only effective for 1st out_lif. 
                */
                rv = _bcm_dpp_pon_lif_is_3_tags_data(unit, gport_hw_resources.global_out_lif, &is_use_data_entry);
                BCMDNX_IF_ERR_EXIT(rv);
                if (is_use_data_entry) {
                    lif_eep_ndx = gport_hw_resources.global_out_lif;
                }
            }

            if (class_id == 0) {
                soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(unit, lif_eep_ndx, SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
            } else {
                soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(unit, lif_eep_ndx, SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
            }
            SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

exit:
    BCM_FREE(lif_info);
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_port_class_get_split_horizon
 * Purpose:
 *      Getting of auxilary table split horizon value for given lif
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      class_id - 1 for HUB
 *                 0 for SPOKE
 *                 (out)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int 
_bcm_dpp_port_class_get_split_horizon(
    int unit, 
    bcm_port_t port, 
    uint32 *class_id,
   int is_ingress)
{    
    SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
    uint32 soc_sand_rv = 0;
    int rv;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;

    BCMDNX_INIT_FUNC_DEFS

    if (is_ingress) {
        uint32 inlif_profile = 0;
        uint32 inlif_profile_orientation = 0;
        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_dpp_port_class_get_split_horizon.lif_info");
        if (lif_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_ISID) {
            inlif_profile = lif_info->value.isid.lif_profile;
            if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                        (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION, &inlif_profile,
                                         &inlif_profile_orientation));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
                *class_id = (inlif_profile_orientation << 1) | ((lif_info->value.isid.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE) ? 0 : 1);
            } else {
                *class_id = ((lif_info->value.isid.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE) ? 0 : 1);
            }
        } else if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
            inlif_profile = lif_info->value.mpls_term_info.lif_profile;
            if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                        (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION, &inlif_profile,
                                         &inlif_profile_orientation));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
                *class_id = (inlif_profile_orientation << 1) | ((lif_info->value.mpls_term_info.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE) ? 0 : 1);
            } else {
                *class_id = ((lif_info->value.mpls_term_info.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE) ? 0 : 1);
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif type.")));
        }

    } else { /*egress*/
        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_JERICHO(unit)) {
            if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                uint32 outlif_profile = 0;
                uint32
                    next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
                    nof_entries;
                _bcm_lif_type_e lif_usage;

                BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, gport_hw_resources.local_out_lif, NULL, &lif_usage));
                if ((lif_usage != _bcmDppLifTypeMplsTunnel)
                    && (lif_usage != _bcmDppLifTypeLinkLayer)
                    && (lif_usage != _bcmDppLifTypeIpTunnel)){
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only MPLS tunnel outlif LL outlif and IPv4 tunnel outLif are supported")));
                }

                BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "_bcm_dpp_port_class_get_split_horizon.encap_entry_info");
                if (encap_entry_info == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }

                soc_sand_rv = soc_ppd_eg_encap_entry_get(
                    unit,
                    SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, gport_hw_resources.local_out_lif, 1,
                    encap_entry_info, next_eep, &nof_entries);
                SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* get outlif profile according to lif type */
                if (lif_usage == _bcmDppLifTypeMplsTunnel) {
                    outlif_profile = encap_entry_info->entry_val.mpls_encap_info.outlif_profile;
                }
                else if (lif_usage == _bcmDppLifTypeLinkLayer) {
                    outlif_profile = encap_entry_info->entry_val.ll_info.outlif_profile;
                }
                else if (lif_usage == _bcmDppLifTypeIpTunnel) {
                    outlif_profile = encap_entry_info->entry_val.ipv4_encap_info.outlif_profile;
                }

                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                  (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
                                      &outlif_profile, class_id));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);

            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG(
                   "cannot get split horizon class id when soc property split_horizon_forwarding_groups_mode is unset")));
            }
        } else {
            soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_get(unit, gport_hw_resources.local_out_lif, &orientation);
            SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

            *class_id = ((orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE) ? 0 : 1);
        }
    }

exit:
    BCM_FREE(lif_info);
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_class_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_class_t pclass, 
    uint32 *class_id)
{
   int port_i;

   int soc_sand_dev_id = unit, soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_FP_CONTROL_INDEX
       ctrl_indx;
   SOC_PPC_FP_CONTROL_INFO
       ctrl_info;
   SOC_PPC_PORT_INFO port_info;
   SOC_PPC_PORT soc_ppd_port;
   SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
   _BCM_GPORT_PHY_PORT_INFO phy_port;
   int core;      
   _bcm_dpp_gport_info_t gport_info;
   _bcm_dpp_gport_hw_resources gport_hw_resources;
   uint32 flags;
   BCMDNX_INIT_FUNC_DEFS;

   soc_sand_dev_id = (unit);

   *class_id = 0;
   /* For Field (LIF-Profile), the gport can be LIF and not port */
   switch (pclass) {
       case bcmPortClassFieldIngressPacketProcessing:
       case bcmPortClassFieldIngress:
           /* See if LIF */
           rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                               &gport_hw_resources);           
           if ((BCM_E_NONE == rv) && (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
             /* LIF found - update the entry */
             

             BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_port_class_get.lif_info");
             if (lif_info == NULL) {        
               BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
             }
             rv = _bcm_petra_port_inlif_profile_get(unit, port, class_id, SOC_OCC_MGMT_APP_USER, 0/*all_kinds*/, lif_info);
             BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error getting the inlif profile")));
             BCM_EXIT;
           }
           break;
   case bcmPortClassL2Lookup:

            /* Verify that the port is of type port-vlan */
            if (!(BCM_GPORT_IS_VLAN_PORT(port))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong port parameter - Not port-vlan type")));
            }

            /* Get the SW DB for the LIF */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                               (sw_state_htb_key_t) &port,
                                               (sw_state_htb_data_t) &phy_port,
                                               FALSE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not existing port supplied - %d"), port));
            }

            /* Validate the type to be ring */
            if (phy_port.type != _bcmDppGportResolveTypeRing) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port %d is not part of a L2 Lookup group"), port));
            }

            /* Get the failover id (class id) based on the fec id */
            DPP_FAILOVER_L2_LOOKUP_SET(*class_id, phy_port.phy_gport);

            BCM_EXIT;
   case bcmPortClassForwardIngress:
            rv = _bcm_dpp_port_class_get_split_horizon(unit, port, class_id, 1/*is_ingress*/);
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_EXIT;
   case bcmPortClassForwardEgress:
            /* Only MPLS Tunnel is supported */
           rv = _bcm_dpp_port_class_get_split_horizon(unit, port, class_id, 0/*is_ingress*/);
           BCMDNX_IF_ERR_EXIT(rv);

           BCM_EXIT;
   default:
       /* continue */
       break;
   }

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
       rv=soc_port_sw_db_flags_get(unit, port_i, &flags);
       BCMDNX_IF_ERR_EXIT(rv);
       if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags)) {
           continue;
       }
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));
       switch (pclass) {
       case bcmPortClassFieldEgressPacketProcessing:
       case bcmPortClassFieldEgress:
       case bcmPortClassFieldIngressPacketProcessing:
       case bcmPortClassFieldIngressPacketProcessingHigh:
       case bcmPortClassFieldIngress:
       case bcmPortClassFieldLookup:
           /* class range test in ppd */
           SOC_PPC_FP_CONTROL_INDEX_clear(&ctrl_indx);
           ctrl_indx.val_ndx = soc_ppd_port;
           switch (pclass) {
               case bcmPortClassFieldEgressPacketProcessing:
                       ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_EGR_PP_PORT_DATA;
                   break;
               case bcmPortClassFieldEgress:
                       ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_EGR_TM_PORT_DATA;
                   break;
               case bcmPortClassFieldLookup:
                   if(SOC_DPP_CONFIG(unit)->pp.fcoe_enable){
                           BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
                       }
                       ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA;
                   break;
               case bcmPortClassFieldIngressPacketProcessing:
               case bcmPortClassFieldIngressPacketProcessingHigh:
                   ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_ING_PP_PORT_DATA;
                   break;
               case bcmPortClassFieldIngress:
                       ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_ING_TM_PORT_DATA;
                   break;
               /* must default. Otherwise, compilations error */
               /* coverity[dead_error_begin]*/
               default:
                   break;
           }
           soc_sand_rv = soc_ppd_fp_control_get(soc_sand_dev_id,core,&ctrl_indx,&ctrl_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           if (pclass == bcmPortClassFieldIngressPacketProcessingHigh) {
               *class_id = ctrl_info.val[1];
           } else {
               *class_id = ctrl_info.val[0];
           }
           break;

        case bcmPortClassId:
           /* Get the VLAN Domain = Port class */
           soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, core, soc_ppd_port, &port_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           *class_id = port_info.vlan_domain;
          break;

       case bcmPortClassFieldIngressVlanTranslation:
           {
               soc_dpp_config_pp_t *dpp_pp;
               dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

               /* This port class is releveant if tunnel termination key is {sip, dip, port property, next protocol} */
               if (!(dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL))) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("IPv4 tunnel mode must include dip_sip_port_next_protocol to change port class.")));
               }

               rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_property_get, (unit, core, soc_ppd_port, soc_ppc_port_property_vlan_translation, class_id));
               BCMDNX_IF_ERR_EXIT(rv);
           }
           break;

            case bcmPortClassFieldIngressTunnelTerminated:
           {
               soc_dpp_config_pp_t *dpp_pp;
               dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

               /* This port class is releveant for FCoE and for {dip,sip,vrf,port_property} tunnel termination lookup */
               if (!(SOC_DPP_CONFIG(unit)->pp.fcoe_enable) &&
                   !(dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) &&
                   !(dpp_pp->ipv6_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF))) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("applicable when FCoE or DIP_SIP_VRF or IPv6 DIP_SIP_VRF termination mode are enabled.")));
               }

               rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_property_get, (unit, core, soc_ppd_port, soc_ppc_port_property_tunnel_termination, class_id));
               BCMDNX_IF_ERR_EXIT(rv);
           }           
           break;
           
           case bcmPortClassProgramEditorEgressPacketProcessing:
           
           {
               rv = arad_egr_prog_editor_pp_pct_var_get(unit, port_i, class_id);
               BCMDNX_IF_ERR_EXIT(rv);
           }
           break;
           case bcmPortClassProgramEditorEgressTrafficManagement:
           {
               int tm_port_var = 0;
               soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_tm_port_var_get, (unit, port, &tm_port_var)));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

               *class_id = (uint32) tm_port_var;
           }
           break;

        default:
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: Not supported flag. Supported flags: bcmPortClassFieldEgress, bcmPortClassId, unit %d"),FUNCTION_NAME(), unit));
       }
       BCM_EXIT; /* return info of first*/
   }
exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_port_class_nof_vlan_domains_count(
    int unit, 
    uint32 class_id,
    int *nof_pp_ports)
{
   int local_port;
   int soc_sand_dev_id = unit, soc_sand_rv = 0;
   SOC_PPC_PORT_INFO port_info;
   int core = SOC_CORE_INVALID;
   SOC_PPC_PORT soc_ppd_port = 0;

   BCMDNX_INIT_FUNC_DEFS;

   soc_sand_dev_id = (unit);

   /* Loop on the PP-Ports */
   *nof_pp_ports = 0;
   for (local_port = 0; local_port < SOC_DPP_DEFS_GET(unit, nof_local_ports); local_port++) {
       /* Get the VLAN Domain = Port class */
       SOC_PPC_PORT_INFO_clear(&port_info);

       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

       soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, core, soc_ppd_port, &port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       if (class_id == port_info.vlan_domain) {
           *nof_pp_ports = *nof_pp_ports + 1;
       }
   }

exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_class_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_class_t pclass, 
    uint32 class_id)
{
   int port_i;
   int soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_FP_CONTROL_INDEX
       ctrl_indx;
   SOC_PPC_FP_CONTROL_INFO
       ctrl_info;
   SOC_SAND_SUCCESS_FAILURE       failure_indication;
   SOC_PPC_PORT_INFO port_info;
   uint32 class_id_previous;
   SOC_PPC_PORT soc_ppd_port_i;
   uint32 failover_fec_id;   
   int is_local;
   SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
   SOC_PPC_FRWRD_FEC_ENTRY_INFO     hw_fec_working_entry_info, hw_fec_protection_entry_info,
                                    hw_current_ring_fec_working_entry_info;
   SOC_PPC_FRWRD_FEC_PROTECT_INFO   protect_info;
   uint8                            fec_success;
   SOC_PPC_FRWRD_FEC_PROTECT_TYPE   protect_type;
   int is_de_associate = FALSE, core;
   _BCM_GPORT_PHY_PORT_INFO phy_port;
   _bcm_dpp_gport_info_t gport_info;
   _bcm_dpp_gport_hw_resources gport_hw_resources;

   BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_port_class_set.lif_info");
    if (lif_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

   /* For Field (LIF-Profile), the gport can be LIF and not port */
   switch (pclass) {
       case bcmPortClassFieldIngressPacketProcessing:
       case bcmPortClassFieldIngress:

           /* See if LIF */
           rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, 
                                                 &gport_hw_resources);           
           if ((BCM_E_NONE == rv) && (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
             /* LIF found - update the entry */
             

             rv = _bcm_petra_port_inlif_profile_set(unit, port, class_id, SOC_OCC_MGMT_APP_USER, lif_info);
             BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error setting the inlif profile")));
             BCM_EXIT;
           }
           break;

           /* Association and de association of a LIF to and from a failover group
              in case of Ring Protection. The associated LIF forwarding destination
              is set to the failover group FEC, which in turn, is being set to the
              physical port of the associated gport. The LIF learning info is
              updated to the group FEC.
              Common initial stages:
              1. LIF and failover id validation.
              2. Get the original forwarding data from the SW DB
              3. Get the lif_id from the gport. */
        case bcmPortClassL2Lookup:

            /* Verify that the port is of type port-vlan */
            if (!(BCM_GPORT_IS_VLAN_PORT(port))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong port parameter - Not port-vlan type")));
            }

            /* Get a fec id from the class id */
            DPP_FAILOVER_TYPE_RESET(failover_fec_id, class_id);

            /* Verify that failover id (class id) is either a de associate value or an L2 Lookup
               group */
            if (DPP_FAILOVER_IS_L2_LOOKUP(class_id)) {
                /* May add a FEC existance test */
            }
            else if (class_id == BCM_DPP_CLASS_ID_NO_CLASS) {
                is_de_associate = TRUE;
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong class id parameter - Not L2 Lookup type")));
            }

            /* Get the gport original SW DB data from the hash table */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                               (sw_state_htb_key_t) &port,
                                               (sw_state_htb_data_t) &phy_port,
                                               FALSE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not existing port supplied")));
            }

            /* Get the LIF from the gport in order to assign the FEC learning to the LIF */
            rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);  

            /* Perform the port to group association sequence:
               1. Set the forwarding destination for the FEC according to the
                  SW DB of the associated LIF.
               2. Update the LIF learning info with the failover group FEC. */
            if (is_de_associate == FALSE) {

                /* In case of a LIF, Get the FEC destination from the gport SW DB */
                SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&hw_fec_working_entry_info);
                if (phy_port.type == _bcmDppGportResolveTypeAC) {
                    rv = _bcm_dpp_gport_to_sand_pp_dest(unit, port, &(hw_fec_working_entry_info.dest));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /* In case of a Ring LIF, Get the FEC destination from current FEC destination */
                else if (phy_port.type == _bcmDppGportResolveTypeRing) {
                    /* Get the data for the FEC entry that is pointed by the gport */
                    soc_sand_rv = soc_ppd_frwrd_fec_entry_get(unit,
                                                              phy_port.phy_gport,
                                                              &protect_type,
                                                              &hw_current_ring_fec_working_entry_info,
                                                              &hw_fec_protection_entry_info,
                                                              &protect_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    sal_memcpy(&(hw_fec_working_entry_info.dest), &(hw_current_ring_fec_working_entry_info.dest),
                               sizeof(hw_fec_working_entry_info.dest));
                }
                /* Verify that the port is an AC port or an already associated port */
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port type is not supported for L2 Lookup grouping")));
                }

                /* Set FEC entry values, other than the sys-port destination */
                hw_fec_working_entry_info.type = SOC_PPC_FEC_TYPE_SIMPLE_DEST;
                SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&hw_fec_protection_entry_info);
                SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);
                protect_type = SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE;

                /* Set the HW FEC entry */
                soc_sand_rv = soc_ppd_frwrd_fec_entry_add(unit,
                                                          failover_fec_id,
                                                          protect_type,
                                                          &hw_fec_working_entry_info,
                                                          &hw_fec_protection_entry_info,
                                                          &protect_info,
                                                          &fec_success);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* Get the LIF info that includes the learning info in order to update the LIF HW with
                   the learning info. Applicable only for local LIFs. */
                if (is_local) {
                    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    /* Set the update values as long as the learning isn't disabled */
                    if (!(lif_info->value.ac.learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_DISABLE)) {
                        lif_info->value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
                        lif_info->value.ac.learn_record.learn_info.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
                        lif_info->value.ac.learn_record.learn_info.dest_id = failover_fec_id;
                    }

                    /* Update the LIF entry */
                    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                /* Update the gport SW DB to RING group values */
                rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                   (sw_state_htb_key_t) &port,
                                                   (sw_state_htb_data_t) &phy_port,
                                                   TRUE);        
                if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not existing port supplied")));
                }

                phy_port.type = _bcmDppGportResolveTypeRing;
                phy_port.phy_gport = failover_fec_id;

                rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                                     (sw_state_htb_key_t) &port,
                                                     (sw_state_htb_data_t) &phy_port);   
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /* Perform the port from group de association sequence:
               1. Get physical port from the FEC entry.
               2. Update the LIF learning info to the original physical port
                  that was retrieved from the FEC.
               3. Update the SW DB forwarding destination to the original
                  physical port that was retrieved from the FEC. */
            else {
                /* Verify that the port is an already associated port */
                if (phy_port.type != _bcmDppGportResolveTypeRing) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port type is not of type L2 Lookup")));
                }

                /* Get the data for the FEC entry that is pointed by the gport */
                soc_sand_rv = soc_ppd_frwrd_fec_entry_get(unit,
                                                          phy_port.phy_gport,
                                                          &protect_type,
                                                          &hw_fec_working_entry_info,
                                                          &hw_fec_protection_entry_info,
                                                          &protect_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* Get the LIF info that includes the learning info in order to reset the LIF HW
                   learning info. Applicable only for local LIFs. */
                if (is_local) {
                    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                /* Get the port dest id via the FEC Entry */
                soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit,
                                                                &(hw_fec_working_entry_info.dest),
                                                                &(lif_info->value.ac.learn_record.learn_info));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* Set the update values for a local LIF as long as the learning isn't disabled */
                if (is_local) {
                    if (!(lif_info->value.ac.learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_DISABLE)) {

                        /* Set the learn type according to the retrieved learn info. For UC_FLOW retain
                           the INFO learn type */
                        if ((lif_info->value.ac.learn_record.learn_info.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) ||
                            (lif_info->value.ac.learn_record.learn_info.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG)) {
                            lif_info->value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
                        }

                        /* Update the LIF entry */
                        soc_sand_rv = soc_ppd_lif_table_entry_update(unit, gport_hw_resources.local_in_lif, lif_info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }

                /* Update the gport SW DB to the original physical port. First, get the current entry. */
                rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                   (sw_state_htb_key_t) &port,
                                                   (sw_state_htb_data_t) &phy_port,
                                                   TRUE);        
                if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not existing port supplied")));
                }

                /* Format the SW DB entry. The physical gport is taken from the FEC entry. */
                phy_port.type = _bcmDppGportResolveTypeAC;
                rv = _bcm_dpp_gport_from_sand_pp_dest(unit, &(phy_port.phy_gport), &(hw_fec_working_entry_info.dest));
                BCMDNX_IF_ERR_EXIT(rv);

                /* Insert the updated destination to the SW DB */
                rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                                     (sw_state_htb_key_t) &port,
                                                     (sw_state_htb_data_t) &phy_port);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            BCM_EXIT;
       case bcmPortClassForwardIngress:
                if (BCM_GPORT_IS_TUNNEL(port)) {
                    rv = _bcm_dpp_port_class_set_split_horizon(unit, port, class_id,1);
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_EXIT;
                }
                break;

       case bcmPortClassForwardEgress:
             /* only MPLS Tunnel, LL and IP tunnel are supported */
             if (BCM_GPORT_IS_TUNNEL(port)) {
                 rv = _bcm_dpp_port_class_set_split_horizon(unit, port, class_id,0);
                 BCMDNX_IF_ERR_EXIT(rv);
                 BCM_EXIT;
             }
             break;
        default:
            /* continue */
            break;
    }

   /* Retrieve local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
       switch (pclass) {
           case bcmPortClassFieldEgressPacketProcessing:
           case bcmPortClassFieldEgress:
           case bcmPortClassFieldIngressPacketProcessing:
           case bcmPortClassFieldIngress:
           case bcmPortClassFieldLookup:
               /* class range test in ppd */
               SOC_PPC_FP_CONTROL_INDEX_clear(&ctrl_indx);
               SOC_PPC_FP_CONTROL_INFO_clear(&ctrl_info);

               ctrl_indx.val_ndx = soc_ppd_port_i;
               switch (pclass) {
                   case bcmPortClassFieldEgressPacketProcessing:
                           ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_EGR_PP_PORT_DATA;
                       break;
                   case bcmPortClassFieldEgress:
                           ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_EGR_TM_PORT_DATA;
                       break;
                   case bcmPortClassFieldLookup:
                       if(SOC_DPP_CONFIG(unit)->pp.fcoe_enable){
                           BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
                       }
                           ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA;
                       break;
                   case bcmPortClassFieldIngressPacketProcessing:
                       BCM_SAND_IF_ERR_EXIT(PORT_ACCESS.dpp_port_config.in_port_key_gen_var_32_msb.get(unit, core, soc_ppd_port_i, &(ctrl_info.val[1])));
                       ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_ING_PP_PORT_DATA;
                       break;
                   case bcmPortClassFieldIngress:
                           ctrl_indx.type = SOC_PPC_FP_CONTROL_TYPE_ING_TM_PORT_DATA;
                       break;
                   /* must default. Otherwise, compilations error */
                   /* coverity[dead_error_begin:FALSE]*/
                   default:
                       break;
               }

               ctrl_info.val[0] = class_id;
               soc_sand_rv = soc_ppd_fp_control_set(unit,core,&ctrl_indx,&ctrl_info,&failure_indication);
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
               SOC_SAND_IF_FAIL_EXIT(failure_indication);

              if (pclass == bcmPortClassFieldIngressPacketProcessing) {
                  BCM_SAND_IF_ERR_EXIT(PORT_ACCESS.dpp_port_config.in_port_key_gen_var_32_msb.set(unit, core, soc_ppd_port_i, 0));
              }

           break;

         case bcmPortClassFieldIngressPacketProcessingHigh:
             BCM_SAND_IF_ERR_EXIT(PORT_ACCESS.dpp_port_config.in_port_key_gen_var_32_msb.set(unit, core, soc_ppd_port_i, class_id));
             break;
         case bcmPortClassId:
           /* Get the previous VLAN Domain = Port class */
           rv = bcm_petra_port_class_get(unit, port_i, pclass, &class_id_previous);
           BCMDNX_IF_ERR_EXIT(rv);

           /* Do nothing for the same VLAN-Domain */
           if (class_id_previous == class_id) {
             BCM_EXIT;
           }

           if (class_id >= SOC_DPP_DEFS_GET(unit, nof_vlan_domains)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: class id %d is out of range, unit %d"),FUNCTION_NAME(), class_id, unit));
           }

           /* Set the new VLAN Domain */
           SOC_PPC_PORT_INFO_clear(&port_info);
           soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i, &port_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           port_info.vlan_domain = class_id;
           port_info.flags = (SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL |
                              SOC_PPC_PORT_EPNI_PP_PCT_TBL);
           soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port_i, &port_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         break;

       case bcmPortClassForwardEgress:
         {
               rv = _bcm_dpp_port_class_set_split_horizon(unit, port, class_id,0);
               BCMDNX_IF_ERR_EXIT(rv);
         }
         break;

       case bcmPortClassFieldIngressVlanTranslation:
           {
               soc_dpp_config_pp_t *dpp_pp;
               dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

               /* This port class is releveant if tunnel termination key is {sip, dip, port property, next protocol} */
               if (!(dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL))) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("IPv4 tunnel mode must include dip_sip_port_next_protocol to change port class.")));
               }

               if (class_id > SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Class id is too high. Value should be between 0 and %d"), SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX));
               }

               /* After all input was validated, write to HW. */
               rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_property_set, (unit, core, soc_ppd_port_i, soc_ppc_port_property_vlan_translation, class_id));
               BCMDNX_IF_ERR_EXIT(rv);
           }
           break;

         case bcmPortClassFieldIngressTunnelTerminated:
           {
               soc_dpp_config_pp_t *dpp_pp;
               dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

               /* for FCoE and {dip,sip,vrf,port_property} tunnel termination key */
               if (!(SOC_DPP_CONFIG(unit)->pp.fcoe_enable) &&
                   !(dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) &&
                   !(dpp_pp->ipv6_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF))) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("applicable when FCoE or DIP_SIP_VRF or IPv6 DIP_SIP_VRF termination mode are enabled.")));
               }

               if (class_id > SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Class id is too high. Value should be between 0 and %d"), SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX));
               }

               /* After all input was validated, write to HW. */
               rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_port_property_set, (unit, core, soc_ppd_port_i, soc_ppc_port_property_tunnel_termination, class_id));
               BCMDNX_IF_ERR_EXIT(rv);
           }
           break;

       case bcmPortClassProgramEditorEgressPacketProcessing:
           {
               rv = arad_egr_prog_editor_pp_pct_var_set(unit, port_i, class_id);
               BCMDNX_IF_ERR_EXIT(rv);
           }
           break;
       case bcmPortClassProgramEditorEgressTrafficManagement:
           {
               int tm_port_var;
               
               tm_port_var = (int)class_id;
               soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_tm_port_var_set, (unit, port, tm_port_var)));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           }
           break;

       default:
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: Not supported flag. Supported flags: bcmPortClassFieldEgress, bcmPortClassId, unit %d"),FUNCTION_NAME(), unit));
       }
   }
exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_sample_trap_set(
    int unit, 
    bcm_port_t port, 
    int ingress_rate, 
    int egress_rate,
    bcm_port_t egr_port, 
    int enable
   )
{
    int port_i;
    int trap_indx;
    bcm_dpp_user_defined_traps_t ud_trap_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO   snoop_action;
    int first_appear, last_appear;
    SOC_PPC_PORT_INFO port_info;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    int snoop_cmd = 0, core;
    bcm_dpp_snoop_t snoop_info, old_snoop_data;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(&ud_trap_info,0x0,sizeof(bcm_dpp_user_defined_traps_t));

    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(ud_trap_info.trap));

    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&snoop_info.snoop_action_info);
    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&old_snoop_data.snoop_action_info);
    /*Use for sync*/
    snoop_info.snoop_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING;
    old_snoop_data.snoop_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING;

    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_action);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if disable sampling and trapping */
    if(ingress_rate == 0 && !enable) {
         BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

            soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
            /* if already both are disabled skip */
            if(port_info.initial_action_profile.frwrd_action_strength != 0) {
                port_info.initial_action_profile.frwrd_action_strength = 0;
                if(port_info.initial_action_profile.snoop_action_strength == 0){
                    port_info.initial_action_profile.trap_code = _BCM_PETRA_UD_DFLT_TRAP;
                }
                if (!BCM_GPORT_IS_TRAP(egr_port)) {
                    /* remove trap code usage */
                    ud_trap_info.snoop_cmd = 0;
                    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(ud_trap_info.trap));
                    /*is useded for sync*/
                    ud_trap_info.ud_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING;
                    ud_trap_info.gport_trap_id = 0;
                    rv = _bcm_dpp_am_template_user_defined_trap_exchange(unit, port_i, &ud_trap_info, NULL, &last_appear, &trap_indx, &first_appear);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }

            if(port_info.initial_action_profile.snoop_action_strength != 0) {
                /* reset both strength */
                port_info.initial_action_profile.snoop_action_strength = 0;
                if( port_info.initial_action_profile.frwrd_action_strength == 0) {
                    /*If there is action str remove trap code*/
                    port_info.initial_action_profile.trap_code = _BCM_PETRA_UD_DFLT_TRAP;  
                }
                if (!BCM_GPORT_IS_TRAP(egr_port)) {


                    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&snoop_info.snoop_action_info);
                    /*Use for sync*/
                    snoop_info.snoop_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING;
                    rv = _bcm_dpp_am_template_snoop_cmd_exchange(unit,port_i,&snoop_info,NULL, NULL, &snoop_cmd, &first_appear);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }

            port_info.flags = SOC_PPC_PORT_IHP_PINFO_LLR_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        }
        /* done if disable both */
        BCM_EXIT;
    }



    if(enable) {
        if (BCM_GPORT_IS_TRAP(egr_port)){
            if(ingress_rate != 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit(%d): invalid state: ingress_rate is diffrent than 0, while working with gport trap"), unit));
            }
        }
        else{
            /* map egr_port to forwardig decision */
            rv = _bcm_dpp_gport_to_fwd_decision(unit,egr_port,&(ud_trap_info.trap.dest_info.frwrd_dest));
            BCMDNX_IF_ERR_EXIT(rv);

            /* enable destination changing */
            ud_trap_info.trap.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            /* control so not filtered anyway*/
            ud_trap_info.trap.processing_info.is_control = TRUE;
            /* trapped to maintain system headers */
            ud_trap_info.trap.processing_info.is_trap = TRUE;
        }
    }

    if(ingress_rate) {

        snoop_info.snoop_action_info.prob = ingress_rate;
        snoop_info.snoop_action_info.size = _BCM_DPP_SNOOP_SIZE;
        snoop_info.snoop_action_info.cmd.dest_id.type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
        snoop_info.snoop_action_info.cmd.dest_id.id = _BCM_DPP_SNOOP_DEST;

       /* Allocate for first port only */
       BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) { 
 
           rv =  _bcm_dpp_am_template_snoop_cmd_data_get(
                    unit,
                    port_i,
                    &old_snoop_data
                );
          BCMDNX_IF_ERR_EXIT(rv);

          
           rv = _bcm_dpp_am_template_snoop_cmd_exchange(unit,port_i,&snoop_info,NULL, NULL, &snoop_cmd, &first_appear);
           BCMDNX_IF_ERR_EXIT(rv);      

           snoop_cmd += 2; /* 0 and 1 are reserved */

           
           if (SOC_IS_JERICHO(unit)) {
               continue;
           }

           if(first_appear) {
               soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_snoop_set,(unit,snoop_cmd, &snoop_info.snoop_action_info)));
               /* if fail, due to fail in params range remove from template manage and exit */
               if( handle_sand_result(soc_sand_rv) != BCM_E_NONE)
               {
                   rv = _bcm_dpp_am_template_snoop_cmd_exchange(unit,port_i,&old_snoop_data,NULL, NULL, &snoop_cmd, &first_appear);
                   BCMDNX_IF_ERR_EXIT(rv);      
           
               }
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           }
            snoop_action.snoop_cmnd = snoop_cmd;
            /* Settings are for first port setting needed */
            /*break;*/
        }
    }


    /* convert trap-info into buffer */
    ud_trap_info.snoop_cmd = snoop_cmd;


    /*  update trap-code */
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (BCM_GPORT_IS_TRAP(egr_port)) {
            SOC_PPC_TRAP_CODE ppd_trap_code;

            port_info.initial_action_profile.frwrd_action_strength = BCM_GPORT_TRAP_GET_STRENGTH(egr_port);
            rv = _bcm_rx_ppd_trap_code_from_trap_id(unit, BCM_GPORT_TRAP_GET_ID(egr_port), &ppd_trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
            port_info.initial_action_profile.trap_code = ppd_trap_code;
            port_info.initial_action_profile.snoop_action_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(egr_port);
        }
        else{/* not trap */
            /*is useded for sync*/
            ud_trap_info.ud_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING; 
            rv = _bcm_dpp_am_template_user_defined_trap_exchange(unit, port_i, &ud_trap_info, NULL, &last_appear, &trap_indx, &first_appear);
            
            if (!SOC_IS_JERICHO(unit)) {
                if (rv != BCM_E_NONE && ingress_rate) {
                    /* revert snoop command since user define trap returned error. Revert is needed only when ingress_rate is != 0 */
                    rv = _bcm_dpp_am_template_snoop_cmd_exchange(unit,port_i,&old_snoop_data,NULL, NULL, &snoop_cmd, &first_appear);
                    BCMDNX_IF_ERR_EXIT(rv);
                    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_snoop_set,(unit,snoop_cmd, &old_snoop_data.snoop_action_info)));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }

            BCMDNX_IF_ERR_EXIT(rv);

            if(enable && first_appear) {
                soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_indx+SOC_PPC_TRAP_CODE_USER_DEFINED_0,&(ud_trap_info.trap),BCM_CORE_ALL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            if(ingress_rate && first_appear) {
                soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(unit,trap_indx+SOC_PPC_TRAP_CODE_USER_DEFINED_0,&snoop_action);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            if(ingress_rate) {
                port_info.initial_action_profile.snoop_action_strength = (SOC_DPP_CONFIG(unit)->pp.default_snoop_strength);
                port_info.initial_action_profile.trap_code = trap_indx + SOC_PPC_TRAP_CODE_USER_DEFINED_0;
            }
            if(enable) {
                port_info.initial_action_profile.frwrd_action_strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
                port_info.initial_action_profile.trap_code = trap_indx + SOC_PPC_TRAP_CODE_USER_DEFINED_0;
            }
        }

        port_info.flags = SOC_PPC_PORT_IHP_PINFO_LLR_TBL;
        soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_force_forward_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_t *egr_port, 
    int *enabled)
{
    int port_i;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO   trap;
    SOC_PPC_PORT_INFO port_info;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE, core;
    SOC_PPC_PORT soc_ppd_port_i;
    bcm_gport_t eg_gport;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        /* check if disable */
        if(port_info.initial_action_profile.frwrd_action_strength == 0) {
            *enabled = 0;
            *egr_port = BCM_GPORT_TYPE_NONE;
            BCM_EXIT;
        }
    
        *enabled = 1;
    
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,port_info.initial_action_profile.trap_code,&trap);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        /* map egr_port to forwardig decision */
        rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,egr_port,NULL,&(trap.dest_info.frwrd_dest), -1/*encap_usage*/,0/*force_destination*/);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* translate the gport to phy port */
        eg_gport = *egr_port;

        if (!BCM_GPORT_IS_MCAST(eg_gport)) {
            rv = bcm_petra_port_local_get(unit, eg_gport, egr_port);
            BCMDNX_IF_ERR_EXIT(rv); /* return first info*/
        }
        
        BCM_EXIT;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_port_sample_rate_get(
    int unit, 
    bcm_port_t port, 
    int *ingress_rate, 
    int *egress_rate)
{
    int port_i, core; 
    SOC_PPC_PORT_INFO
       port_info;    
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_action_info;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO          snoop_info;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    *egress_rate = 0;

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);   
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) { 
       
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = handle_sand_result(soc_sand_rv);
    
        /* check if disable */
        if(port_info.initial_action_profile.snoop_action_strength == 0) {
            *ingress_rate = 0;
             BCMDNX_IF_ERR_EXIT(rv);
             BCM_EXIT;
        }
   
        soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(unit,port_info.initial_action_profile.trap_code,&snoop_action_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if(snoop_action_info.snoop_cmnd == 0) {
            *ingress_rate = 0;
             BCMDNX_IF_ERR_EXIT(rv);
             BCM_EXIT;
        }
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_snoop_get,(unit,snoop_action_info.snoop_cmnd,&snoop_info)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = handle_sand_result(soc_sand_rv);

        /* map egr_port to forwardig decision */

        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *ingress_rate = snoop_info.prob;
        BCMDNX_IF_ERR_EXIT(rv); /* return first info*/
        BCM_EXIT;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_force_forward_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_t egr_port, 
    int enable)
{
    int rv = BCM_E_NONE;
    int ingress_rate=0, egress_rate;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_petra_port_sample_rate_get(unit,port,&ingress_rate,&egress_rate);
    BCMDNX_IF_ERR_EXIT(rv);


    if((ingress_rate!=0) && (BCM_GPORT_IS_TRAP(egr_port))) {
        ingress_rate = 0; /* using trap-gport then snooping is set as part of trap configuration */
    }

    rv = bcm_petra_port_sample_trap_set(unit,port,ingress_rate,egress_rate,egr_port, enable);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_petra_port_force_dest_set(int unit, 
            bcm_gport_t gport, 
            bcm_port_dest_info_t *dest_info)
{
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported for ARAD")));
exit:
  BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_force_dest_get(int unit, 
            bcm_gport_t gport, 
            bcm_port_dest_info_t *dest_info)
{
  int port_i = 0, core;
  int soc_sand_rv = 0;
  uint32 pp_port = 0;
  SOC_TMC_PORTS_FORWARDING_HEADER_INFO pfh_info;
 
  BCMDNX_INIT_FUNC_DEFS;
  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_clear(&pfh_info);

  if (BCM_GPORT_IS_LOCAL(gport)) {
    port_i = BCM_GPORT_LOCAL_GET(gport);
  } else if (BCM_GPORT_IS_MODPORT(gport)) {
    port_i = BCM_GPORT_MODPORT_PORT_GET(gport);
  } else if (IS_PORT(unit, gport)) {
    port_i = gport;
  } else {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit(%d): Invalid gport type input to bcm_petra_force_dest_get()"), unit));
  }
  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core)));

  soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_forwarding_header_get,(unit, pp_port, &pfh_info));
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  dest_info->dp = pfh_info.dp;
  dest_info->priority = pfh_info.tr_cls;

  if (pfh_info.destination.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
    BCM_GPORT_SYSTEM_PORT_ID_SET(dest_info->gport, pfh_info.destination.id);
  } else if (pfh_info.destination.type == SOC_TMC_DEST_TYPE_QUEUE) {
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(dest_info->gport, pfh_info.destination.id);
  } else if ( pfh_info.destination.type == SOC_TMC_DEST_TYPE_MULTICAST) {
    BCM_GPORT_MCAST_SET(dest_info->gport, pfh_info.destination.id);
  } else {
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: dest port dest type(%d) unsupported, unit %d"), FUNCTION_NAME(), pfh_info.destination.type, unit));
  } 
exit:
  BCMDNX_FUNC_RETURN;
}



/* global mapping and not per port */
/* for C-tag packets */
int 
bcm_petra_port_priority_color_get(
    int unit, 
    bcm_port_t port, 
    int prio, 
    bcm_color_t *color)
{
    int port_i;
    SOC_PPC_LLP_COS_PORT_INFO    cos_port_info;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    map_entry;
    int tbl_indx, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_VLAN_CHK_PRIO(unit, prio);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit,core,soc_ppd_port_i,&cos_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        tbl_indx = cos_port_info.l2_info.tbls_select.up_to_dp_index;
        soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_get(unit,SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP,tbl_indx,prio,&map_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        rv = _bcm_petra_port_color_decode(unit,map_entry.value1,color);
        BCMDNX_IF_ERR_EXIT(rv);
    
        break; /* return first */
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_port_priority_color_set(
    int unit, 
    bcm_port_t port, 
    int prio, 
    bcm_color_t color)
{
   int core;
   SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    new_entry;
   int profile, dp, new_profile;
   int soc_sand_rv = 0,port_idx;
   int rv = BCM_E_NONE;
   SOC_PPC_LLP_COS_PORT_INFO cos_port_info;
   SOC_PPC_PORT soc_ppd_port_idx;
   _bcm_dpp_gport_info_t gport_info;
   BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

   BCM_DPP_VLAN_CHK_PRIO(unit, prio);
   
   rv = _bcm_petra_port_color_encode(unit,color,&dp);
   BCMDNX_IF_ERR_EXIT(rv);
  
   SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&new_entry);
   new_entry.value1 = dp;

    /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);

   rv = _bcm_petra_port_map_alloc_profile(unit, _bcm_dpp_port_map_type_up_to_dp, gport_info.pbmp_local_ports, 0, prio, &new_entry, &profile, &new_profile);
   BCMDNX_IF_ERR_EXIT(rv);

   /* set entry in allocated profile */
   soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(unit,SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP,profile,prio,&new_entry);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   /* make ports point to this profile */
   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_idx) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_idx, &soc_ppd_port_idx, &core)));

       soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit,core,soc_ppd_port_idx,&cos_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       cos_port_info.l2_info.tbls_select.up_to_dp_index = profile;
       soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit,core, soc_ppd_port_idx,&cos_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   }

exit:
  BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_port_discard_convert_in_lif_to_state
 * Description:
 *      Convert an In-LIF Protection-Pointer value to an In-LIF discard state.
 * Parameters:
 *      unit - (IN) Device number
 *      protection_pointer - (IN) The In-LIF Protection-Pointer value
 * Return Value:
 *      The In-LIF discard state
 */
int _bcm_petra_port_discard_convert_in_lif_to_state(
   int unit, 
   int protection_pointer)
{
    /* Return the discard state according to the Protection-Pointer value */
    return ((protection_pointer == _BCM_INGRESS_PROTECTION_POINTER_INVALID) ? TRUE : FALSE); 
}


/*
 * Function:
 *      _bcm_petra_port_discard_out_lif_get
 * Description:
 *      Retrieves the discard state of an Out-LIF.
 * Parameters:
 *      unit - (IN) Device number
 *      out_lif_index - (IN) The Out-LIF for which to retrieve the discard state
 *      discard_state - (OUT) Returned Out-LIF discard state
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_petra_port_discard_out_lif_get(
    int unit, 
    int out_lif_index,
    uint32 *discard_state)
{
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* update LIF attribute */
    soc_sand_rv = soc_ppd_eg_encap_lif_field_get(unit, out_lif_index, SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP, discard_state);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_port_discard_mode_to_lif_state
 * Description:
 *      Determines the discard state for both In-LIF & Out-LIF
 *      according to the supplied discard mode value.
 * Parameters:
 *      unit - (IN) Device number
 *      mode - (IN) Discard mode value
 *      is_in_lif_discard -  (OUT) Returned In-LIF discard state
 *      is_out_lif_discard - (OUT) Returned Out-LIF discard state
 * Return Value:
 *      BCM_E_XXX
 */
STATIC
int _bcm_petra_port_discard_mode_to_lif_state(
   int unit,
   int mode,
   uint8 *is_in_lif_discard,
   uint8 *is_out_lif_discard)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* Determine the In-LIF & Out-LIF discard state */
    switch (mode) {
    case BCM_PORT_DISCARD_NONE:
        *is_in_lif_discard = FALSE;
        *is_out_lif_discard = FALSE;
        break;
    case BCM_PORT_DISCARD_ALL:
        *is_in_lif_discard = TRUE;
        *is_out_lif_discard = TRUE;
        break;
    case BCM_PORT_DISCARD_INGRESS:
        *is_in_lif_discard = TRUE;
        *is_out_lif_discard = FALSE;
        break;
    case BCM_PORT_DISCARD_EGRESS:
        *is_in_lif_discard = FALSE;
        *is_out_lif_discard = TRUE;
        break;
    default:
        /* Unsupported mode values */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Mode #%d is invalid for LIF discard"),FUNCTION_NAME(), mode));
    }

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_discard_mode_to_hw_resources_flags
  * Description:
 *      Determines the HW resources flags (In-LIF,Out-LIF or Both)
 *      according to the supplied discard mode value.
 * Parameters:
 *      unit - (IN) Device number
 *      mode - (IN) Discard mode value
 *      flags - (OUT) Returned HW resources flags
 * Return Value:
 *      BCM_E_XXX
 */
STATIC
int _bcm_petra_port_discard_mode_to_hw_resources_flags(
   int unit,
   int mode,
   uint32 *flags)
{
    BCMDNX_INIT_FUNC_DEFS;

    switch (DPP_PORT_DISCARD_MODE_GET(mode))
    {
    case _BCM_DPP_PORT_DISCARD_MODE_INGRESS_ONLY:
        *flags = _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
        break;
    case _BCM_DPP_PORT_DISCARD_MODE_EGRESS_ONLY:
        *flags = _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        break;
    case _BCM_DPP_PORT_DISCARD_MODE_BOTH:
        *flags = _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF;
        break;
    default:
        /* Unsupported mode values */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Mode #%d is invalid for LIF discard"),FUNCTION_NAME(), mode));
    }

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_discard_lif_state_to_mode
 * Description:
 *      Determines discard mode state according to both the In-LIF & Out-LIF discard states
 *      discard states.
 * Parameters:
 *      unit - (IN) Device number
 *      is_in_lif_discard -  (IN) In-LIF Discard mode value
 *      is_out_lif_discard - (IN) Out-LIF Discard mode value
 *      mode - (OUT) Returned discard mode
 * Return Value:
 *      BCM_E_XXX
 */
STATIC
int _bcm_petra_port_discard_lif_state_to_mode(
   int unit,
   uint8 is_in_lif_discard,
   uint8 is_out_lif_discard,
   int *mode)
{
    if (is_in_lif_discard) {
        *mode = (is_out_lif_discard) ? BCM_PORT_DISCARD_ALL : BCM_PORT_DISCARD_INGRESS;
    } else {
        *mode = (is_out_lif_discard) ? BCM_PORT_DISCARD_EGRESS : BCM_PORT_DISCARD_NONE;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_petra_port_discard_in_lif_get
 * Description:
 *      Retrieve In-LIF discard operation.
 * Parameters:
 *      unit -         (IN) Device number
 *      in_lif_index - (IN) The In-LIF that the discard operation will be performed on
 *      in_lif_entry_info -  (OUT) returned In-LIF information
 *      Protection-Pointer - (OUT) Returned In-LIF Protection-Pointer
 * Return Value:
 *      BCM_E_XXX
 */
STATIC 
int _bcm_petra_port_discard_in_lif_get(
   int unit,
   int in_lif_index,
   SOC_PPC_LIF_ENTRY_INFO *in_lif_entry_info,
   int32 *protection_pointer)
{
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get the In-LIF discard state from the HW */
    SOC_PPC_LIF_ENTRY_INFO_clear(in_lif_entry_info);
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, in_lif_index, in_lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* The In-LIF discard state is the In-LIF Protection-Pointer. Format it from the In-LIF struct */
    switch (in_lif_entry_info->type) {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
        *protection_pointer = in_lif_entry_info->value.ac.protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
        *protection_pointer = in_lif_entry_info->value.pwe.protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_ISID:
        *protection_pointer = in_lif_entry_info->value.isid.protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
        *protection_pointer = in_lif_entry_info->value.ip_term_info.protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
        *protection_pointer = in_lif_entry_info->value.mpls_term_info.protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
        *protection_pointer = in_lif_entry_info->value.trill.protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
        *protection_pointer = in_lif_entry_info->value.extender.protection_pointer;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported In-LIF type %d for discard operation on In-LIF %d"),
                                          in_lif_entry_info->type, in_lif_index));
    }

exit:
   BCMDNX_FUNC_RETURN;
}



int 
_bcm_petra_port_discard_extend_get(
    int unit, 
    bcm_port_t port, 
    int *mode)
{
   int port_i, core;
   int rv = BCM_E_NONE;
   _bcm_petra_tpid_profile_t profile_type;   
   SOC_PPC_PORT soc_ppd_port_i;
   _bcm_petra_dtag_mode_t dtag_mode;
   _bcm_petra_ac_key_map_type_t ac_key_map_type;
   _bcm_dpp_gport_info_t gport_info;
   BCMDNX_INIT_FUNC_DEFS;

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

       /* get old data*/
       rv =  _bcm_dpp_am_template_tpid_profile_data_get(unit, soc_ppd_port_i, core, &profile_type, mode, &dtag_mode, &ac_key_map_type);
       BCMDNX_IF_ERR_EXIT(rv);
       
       break; /* return size for first*/
   }

exit:
   BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_discard_get
 * Description:
 *      Retrieves the port discard state.
 *      Supports LIFs and physical ports.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) gport of a LIF or a physical port
 *      mode - (OUT) Returned Discard mode value
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_petra_port_discard_get(
    int unit, 
    bcm_port_t port, 
    int *mode)
{
    bcm_error_t rv = BCM_E_NONE;
    int is_local,
        protection_pointer;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    uint32 in_lif_discard_state = FALSE, out_lif_discard_state = FALSE;
    SOC_PPC_LIF_ENTRY_INFO in_lif_entry_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Determine if it's physical port or LIF configuration according to the gport type */
   rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
   BCMDNX_IF_ERR_EXIT(rv);

   /* It's a LIF gport, if it's not a physical port. */
   if (gport_parse_info.type != _bcmDppGportParseTypeSimple) {

        /* Convert the gport to LIF IDs and verify that it's a local object
           that represent an In-LIF/Out-LIF */        
        rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_local) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No local LIF for port discard get from gport %d"), port));
        }

        rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF, 
                                                 &gport_hw_resources);
        if (rv == BCM_E_NOT_FOUND) {
            /* If one of LIFs doesnt exist, try Out-LIF only */
            rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                 &gport_hw_resources);
            if (rv == BCM_E_NOT_FOUND) {
                /* If Out-LIF doesnt exist, try In-LIF only */
                rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                 &gport_hw_resources);
            }
        }
        BCMDNX_IF_ERR_EXIT(rv);  

        /* If there's no valid In-LIF/Out-LIF */
        if ((gport_hw_resources.local_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) && (gport_hw_resources.local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No valid LIF for gport %d"), port));
        }
   
        /* Retrieve the In-LIF discard state */
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {

            /* Get the In-LIF HW discard info */
            rv = _bcm_petra_port_discard_in_lif_get(unit, gport_hw_resources.local_in_lif, &in_lif_entry_info, &protection_pointer);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Convert the In-LIF Protection-Pointer value to a discard state */
            in_lif_discard_state = _bcm_petra_port_discard_convert_in_lif_to_state(unit, protection_pointer);
        }

        /* Retrieve the Out-LIF discard state */
        if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {

            /* Get the Out-LIF HW discard info */
            rv = _bcm_petra_port_discard_out_lif_get(unit, gport_hw_resources.local_out_lif, &out_lif_discard_state);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Build a discard state from the In-LIF and Out-LIF discard states */
        rv = _bcm_petra_port_discard_lif_state_to_mode(unit, in_lif_discard_state, out_lif_discard_state, mode);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
   }

   /* Get the discard state for physical ports when not in Advanced VLAN edit mode. */
    _BCM_DPP_TPID_PARSE_ADVANCED_MODE_API_UNAVAIL(unit);
    rv = _bcm_petra_port_discard_extend_get(unit,port,mode);
    BCMDNX_IF_ERR_EXIT(rv);

    if (_BCM_DPP_PORT_DISCARD_MODE_IS_MIM(*mode)) {
       /* if MiM bit is set - remove it */
       _BCM_DPP_PORT_DISCARD_MODE_MIM_REMOVE(*mode);
    }

    if (_BCM_DPP_PORT_DISCARD_MODE_IS_OUTER_PRIO(*mode)) {
       /* if OuterPrio bit is set - remove it */
       _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_REMOVE(*mode);
    }

exit:
   BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_port_discard_in_lif_validate
 * Description:
 *      Validate the In-LIF discard operation.
 *      Disallows an In-LIF discard operation for an In-LIF that uses the
 *      Ingress Protection scheme.
 * Parameters:
 *      unit -         (IN) Device number
 *      in_lif_index - (IN) The In-LIF that the discard operation will be performed on
 *      is_in_lif_discard - (IN) The discard operation for the In-LIF
 *      cur_protection_pointer - (IN) The In-LIF Protection-Pointer value
 *      validation_code -   (OUT) Returned validation code:
 *              BCM_E_NONE - Operation allowed
 *              BCM_E_DISABLED - The In-LIF uses the Ingress Protection scheme
 *              BCM_E_EXISTS - No discard value change
 * Return Value:
 *      BCM_E_XXX
 */
STATIC 
int _bcm_petra_port_discard_in_lif_validate(
   int unit,
   int in_lif_index,
   uint8 is_in_lif_discard,
   int cur_protection_pointer,
   bcm_error_t *validation_code)
{
    int cur_discard_state;

    BCMDNX_INIT_FUNC_DEFS;

    *validation_code = BCM_E_NONE;

    /* Convert the current In-LIF Protection-Pointer value to a discard state */
    cur_discard_state = _bcm_petra_port_discard_convert_in_lif_to_state(unit, cur_protection_pointer);

    /* If the current Protection-Pointer value is as required - No need to actualy perform an operation */
    if (cur_discard_state == is_in_lif_discard) {
        *validation_code = BCM_E_EXISTS;
        LOG_DEBUG(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit, "No discard operation set to HW for In-LIF %d as there's no discard value change (%d)\n"),
                 in_lif_index, cur_discard_state));
        BCM_EXIT;
    }

    /* Disallow if the In-LIF is used for Ingress Protection. i.e. the Protection-Pointer was set from the default value */
    if (is_in_lif_discard && (cur_protection_pointer != _BCM_INGRESS_PROTECTION_POINTER_DEFAULT)) {
        *validation_code = BCM_E_DISABLED;
        LOG_INFO(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit, "In-LIF discard operation not performed for In-LIF %d - The Protection-Pointer (%d) is used for Ingress Protection\n"),
                             in_lif_index, cur_protection_pointer));
         BCM_EXIT;
    }

exit:
   BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_port_discard_in_lif_set
 * Description:
 *      Set the In-LIF discard indication to the required state.
 *      The In-LIF Protection-Pointer is set according to the discard state.
 *  
 * Parameters:
 *      unit -         (IN) Device number
 *      in_lif_index - (IN) The In-LIF that the discard operation will be performed on
 *      is_in_lif_discard - (IN) The discard operation for the In-LIF Protection
 *      in_lif_entry_info - (IN) The In-LIF current information
 * Return Value:
 *      BCM_E_XXX
 */
STATIC 
int _bcm_petra_port_discard_in_lif_set(
   int unit,
   int in_lif_index,
   uint8 is_in_lif_discard,
   SOC_PPC_LIF_ENTRY_INFO *in_lif_entry_info)
{
    int32 new_protection_pointer;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Determine the required value for the In-LIF Protectio-Pointer */
    new_protection_pointer = (is_in_lif_discard) ? _BCM_INGRESS_PROTECTION_POINTER_INVALID : _BCM_INGRESS_PROTECTION_POINTER_DEFAULT;

    /* Format the Protection-Pointer value accrding to the retrieved In-LIF type */
    switch (in_lif_entry_info->type) {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
        in_lif_entry_info->value.ac.protection_pointer = new_protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
        in_lif_entry_info->value.pwe.protection_pointer = new_protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_ISID:
        in_lif_entry_info->value.isid.protection_pointer = new_protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
        in_lif_entry_info->value.ip_term_info.protection_pointer = new_protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
        in_lif_entry_info->value.mpls_term_info.protection_pointer = new_protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
        in_lif_entry_info->value.trill.protection_pointer = new_protection_pointer;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
        in_lif_entry_info->value.extender.protection_pointer = new_protection_pointer;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported In-LIF type %d for discard operation on In-LIF %d"),
                                          in_lif_entry_info->type, in_lif_index));
    }

    /* Set the Protecion-Pointer to the HW */
    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, in_lif_index, in_lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   
exit:
   BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_port_discard_out_lif_set
 * Description:
 *      Set the Out-LIF discard indication to the required state.
 *      The Out-LIF entry drop bit is set according to the discard state.
 *  
 * Parameters:
 *      unit -          (IN) Device number
 *      out_lif_index - (IN) The Out-LIF that the drop operation will be performed on
 *      is_out_lif_drop -  (IN) The drop operation for the Out-LIF Protection
 * Return Value:
 *      BCM_E_XXX
 */
STATIC 
int _bcm_petra_port_discard_out_lif_set(
   int unit,
   int out_lif_index,
   uint8 is_out_lif_drop)
{
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

   /* Update the Out-LIF attribute */
   soc_sand_rv = soc_ppd_eg_encap_lif_field_set(unit, out_lif_index, SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP, is_out_lif_drop);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
   BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_discard_set
 * Description:
 *      Set a gport discard state to discard or pass.
 *      For In-LIF, the Protection-Pointer is used for the discard.
 *      For Out-LIF, the entry drop bit is set according to the discard state.
 *  
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) gport of a LIF or a physical port
 *      mode - (IN) Discard mode value
 * Return Value:
 *      BCM_E_XXX
 */
int 
bcm_petra_port_discard_set(
    int unit, 
    bcm_port_t port, 
    int mode)
{    
    int is_local, cur_protection_pointer;
    int rv;
    uint32 flags;
    bcm_error_t in_lif_validation_code = BCM_E_NONE;
    SOC_PPC_LIF_ENTRY_INFO in_lif_entry_info;
    uint8 is_in_lif_discard, is_out_lif_discard;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    /* Determine if it's physical port or LIF configuration according to the gport type */
    rv = _bcm_dpp_gport_parse(unit, port, &gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* It's a LIF gport, if it's not a physical port.
       Get the LIF IDs from the gport, validate and call dedicated functions to perform the operation */
    if (gport_parse_info.type != _bcmDppGportParseTypeSimple) {

        /* Convert the gport to LIF IDs and verify that it's a local object
           that represent an In-LIF/Out-LIF */        
        rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_local) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No local LIF for port discard set to gport %d"), port));
        }

        rv = _bcm_petra_port_discard_mode_to_hw_resources_flags(unit, mode, &flags);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_to_hw_resources(unit, port, flags, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);    

        /* Validate and determine the required discard states according to the specified mode parameter value */
        rv = _bcm_petra_port_discard_mode_to_lif_state(unit, DPP_PORT_DISCARD_VAL_GET(mode), &is_in_lif_discard, &is_out_lif_discard);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Perform object specific retrieval and validations */
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {

            /* Get the In-LIF HW discard info and perform validations */
            rv = _bcm_petra_port_discard_in_lif_get(unit, gport_hw_resources.local_in_lif, &in_lif_entry_info, &cur_protection_pointer);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Validate the In-LIF discard info operation */
            rv = _bcm_petra_port_discard_in_lif_validate(unit, gport_hw_resources.local_in_lif, is_in_lif_discard, cur_protection_pointer, &in_lif_validation_code);
            BCMDNX_IF_ERR_EXIT(rv);
        } 

        /* Perform the In-LIF discard operation in case the validation was approved */
        if ((in_lif_validation_code == BCM_E_NONE) && (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
            rv = _bcm_petra_port_discard_in_lif_set(unit, gport_hw_resources.local_in_lif, is_in_lif_discard, &in_lif_entry_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Perform the Out-LIF discard operation */
        if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_petra_port_discard_out_lif_set(unit, gport_hw_resources.local_out_lif, is_out_lif_discard);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* If no operation was valid for both In-LIF & Out-LIF */
        if (((in_lif_validation_code == BCM_E_DISABLED) || (gport_hw_resources.local_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID)) &&
            (gport_hw_resources.local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Discard operation invalid for gport %d"), port));
        }

        BCM_EXIT;
    }

    /* Set the discard state for physical ports when not in Advanced VLAN edit mode. */
    _BCM_DPP_TPID_PARSE_ADVANCED_MODE_API_UNAVAIL(unit);
    if (DPP_PORT_DISCARD_MODE_GET(mode) != _BCM_DPP_PORT_DISCARD_MODE_BOTH)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Mode #%d is invalid for port discard"), mode));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_discard_extend_mode_set(unit, port, mode));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_dscp_map_mode_get(
    int unit, 
    bcm_port_t port, 
    int *mode)
{
    SOC_PPC_LLP_COS_PORT_INFO    cos_port_info;
    int port_i, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(mode);

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
        soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit,core,soc_ppd_port_i,&cos_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        /* Translate port information to mode */
        if (cos_port_info.l3_info.use_ip_qos && cos_port_info.l2_info.ignore_pkt_pcp_for_tc) {
            /* Map TC from Packet TOS always either the packet is untagged or not */
            *mode = BCM_PORT_DSCP_MAP_ALL;
        } else if (!cos_port_info.l3_info.use_ip_qos && !cos_port_info.l2_info.ignore_pkt_pcp_for_tc) {
            /* Map TC from Packet PCP and for untagged packets have a default TC. */
            *mode = BCM_PORT_DSCP_MAP_NONE;
        } else if (!cos_port_info.l3_info.use_ip_qos && cos_port_info.l2_info.ignore_pkt_pcp_for_tc) {
            /* Map TC from default TC (no looking at PCP or TOS) */
            *mode = BCM_PORT_DSCP_MAP_DEFAULT;
        } else if (cos_port_info.l3_info.use_ip_qos && !cos_port_info.l2_info.ignore_pkt_pcp_for_tc) {
            /* Map TC from Packet TOS when packet is untagged. */
            *mode = BCM_PORT_DSCP_MAP_UNTAGGED_ONLY;
        } else {
            /* Not suppose to come here */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: not supported retreive from port information to mode. use_ip_qos: %d, ignore_pkt_pcp_for_tc: %d, unit %d"),FUNCTION_NAME(), 
                      cos_port_info.l3_info.use_ip_qos, cos_port_info.l2_info.ignore_pkt_pcp_for_tc, unit));
        }
        
        break; /* return size for first*/
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_dscp_map_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode)
{
    SOC_PPC_LLP_COS_PORT_INFO    cos_port_info;
    int port_i, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    bcm_pbmp_t pbmp_local_ports;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if(mode == BCM_PORT_DSCP_MAP_ZERO) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: not supported mode %d, unit %d"),FUNCTION_NAME(), mode, unit));
    }

    BCM_PBMP_CLEAR(pbmp_local_ports);

    /* Retrive local PP ports */
    if (port == -1) {  
      BCM_PBMP_ASSIGN(pbmp_local_ports, PBMP_E_ALL(unit));
    } else {
      rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
      BCMDNX_IF_ERR_EXIT(rv);

      BCM_PBMP_ASSIGN(pbmp_local_ports, gport_info.pbmp_local_ports);
    }

    switch(mode) {
        case BCM_PORT_DSCP_MAP_NONE:
        case BCM_PORT_DSCP_MAP_ALL:
        case BCM_PORT_DSCP_MAP_DEFAULT:
        case BCM_PORT_DSCP_MAP_UNTAGGED_ONLY:
      break;        
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid mode %d"), mode));
    }

    BCM_PBMP_ITER(pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit,core,soc_ppd_port_i,&cos_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        
        if (mode == BCM_PORT_DSCP_MAP_ALL) {
            /* Map TC from Packet TOS always either the packet is untagged or not */

            /* Enable TOS */            
            cos_port_info.l3_info.use_ip_qos = TRUE;
            /* Disable PCP */
            cos_port_info.l2_info.ignore_pkt_pcp_for_tc = TRUE;

        } else if (mode == BCM_PORT_DSCP_MAP_NONE) {
            /* Map TC from Packet PCP and for untagged packets have a default TC. */

            /* Disable TOS */
            cos_port_info.l3_info.use_ip_qos = FALSE;
            /* Enable PCP */
            cos_port_info.l2_info.ignore_pkt_pcp_for_tc = FALSE;
        } else if (mode == BCM_PORT_DSCP_MAP_DEFAULT) {
            /* Map TC from default TC (no looking at PCP or TOS) */

            /* Disable TOS */
            cos_port_info.l3_info.use_ip_qos = FALSE;
            /* Disable PCP */
            cos_port_info.l2_info.ignore_pkt_pcp_for_tc = TRUE;
        } else {
            /* BCM_PORT_DSCP_MAP_UNTAGGED_ONLY */
            /* Map TC from Packet TOS when packet is untagged. */

            /* Enable TOS */
            cos_port_info.l3_info.use_ip_qos = TRUE;
            /* Enable PCP */
            cos_port_info.l2_info.ignore_pkt_pcp_for_tc = FALSE;
        }
        
        soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit,core, soc_ppd_port_i,&cos_port_info);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);           
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_dscp_map_set(
    int unit, 
    bcm_port_t port, 
    int srccp, 
    int mapcp, 
    int prio)
{
    int rv = BCM_E_NONE;    
    int port_i, core;
    SOC_PPC_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    SOC_PPC_LLP_COS_PORT_INFO port_info;
    int soc_sand_dev_id = unit, soc_sand_rv = 0;
    int dp = 0;
    int srccp_ndx;
    int tbl_ndx = 0;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    entry;

    BCMDNX_INIT_FUNC_DEFS;

    if(mapcp != srccp && srccp !=-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: srccp !=  mapcp not supported, unit %d"),FUNCTION_NAME(), unit));
    }

    if(srccp > _BCM_DPP_MAX_DSCP_VAL ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: srccp is out of range %d, unit %d"),FUNCTION_NAME(), srccp, unit));
    }

    dp = 0x01; /* preserve and default */
    if (prio < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: prio out of range  %d, unit %d"),FUNCTION_NAME(), prio, unit));
    }
    if (prio & BCM_PRIO_BLACK) {
        dp = 0x03;  /* Black */
        prio &= ~BCM_PRIO_BLACK;
    } else if (prio & BCM_PRIO_RED) {
        dp = 0x02;  /* Red */
        prio &= ~BCM_PRIO_RED;
    } else if (prio & BCM_PRIO_YELLOW) {
        dp = 0x01;  /* Yellow  */
        prio &= ~BCM_PRIO_YELLOW;
    } else if (prio & BCM_PRIO_GREEN) {
        dp = 0x00;  /* Green  */
        prio &= ~BCM_PRIO_GREEN;
    }    
    
    if (prio & BCM_PRIO_SECONDARY) {
        tbl_ndx = 1;
        prio &= ~BCM_PRIO_SECONDARY;
    }   
   
    if ((prio & BCM_PRIO_MASK) > BCM_PRIO_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: out of range prio value %d, unit %d"),FUNCTION_NAME(), prio & BCM_PRIO_MASK, unit));
    }

    if ((prio & ~BCM_PRIO_MAX)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: not supported bits in prio value %d, unit %d"),FUNCTION_NAME(), prio & ~BCM_PRIO_MASK, unit));
    }
    
    /* The logic is as below:
        *     if (port == -1) 
        *         do the global mapping from TOS to TC,DP
        *     else 
        *        do the port profile mapping
        */
    if (port == -1) {
        /* Map IPV4 and IPV6 */
        SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&entry);
        entry.value1 = dp;
        entry.value2 = prio & BCM_PRIO_MASK;
        entry.valid = TRUE;

    if (srccp != -1) {
      soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(soc_sand_dev_id,
        SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID,
        tbl_ndx,
        srccp,&entry);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(soc_sand_dev_id,
        SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID,
        tbl_ndx,
        srccp,&entry);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else{
      for (srccp_ndx = 0; srccp_ndx < 256; srccp_ndx++)
      {
        soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(soc_sand_dev_id,
          SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID,
          tbl_ndx,
          srccp_ndx,&entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(soc_sand_dev_id,
              SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID,
              tbl_ndx,
              srccp_ndx,&entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
        }
    } else {    
        /* Retrive local PP ports */
        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);  

        /* Set the In-PP-Port. TC-DP-TOS-Index */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

           BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

           soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, core, soc_ppd_port, &port_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

           port_info.l3_info.ip_qos_to_tc_index = tbl_ndx;

           soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit, core, soc_ppd_port, &port_info);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       }  
    }     

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_dscp_map_get(
    int unit, 
    bcm_port_t port, 
    int srccp, 
    int *mapcp, 
    int *prio)
{
    int rv = BCM_E_NONE;    
    int port_i, core;
    SOC_PPC_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    SOC_PPC_LLP_COS_PORT_INFO port_info;
    int soc_sand_dev_id = unit, soc_sand_rv = 0;
    int dp = 0;
    int act_srccp = srccp;
    int tbl_ndx = 0;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    entry;

    BCMDNX_INIT_FUNC_DEFS;
    /* check input parameters*/

    BCMDNX_NULL_CHECK(mapcp);
    BCMDNX_NULL_CHECK(prio);

    if (srccp < -1 || srccp > _BCM_DPP_MAX_DSCP_VAL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid srccp %d"), srccp));
    }

    if(srccp == -1){
      act_srccp = 0;
    }

    *mapcp = srccp;
    
    /* The logic is as below:
        *     if (port == -1) 
        *         get the global mapping from TOS to TC,DP
        *     else 
        *        get the port profile mapping
        */
    if (port == -1) {        
        if (*prio & BCM_PRIO_SECONDARY) {
            tbl_ndx = 1;
            *prio &= ~BCM_PRIO_SECONDARY;
        }   
        /* global get of port 1. Enough to get only from IPV6 */
        soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_get(soc_sand_dev_id,
                              SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID,
                              tbl_ndx,
                              act_srccp,&entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


        if (!entry.valid) {
            *prio = -1;
            BCM_EXIT;
        }

        *prio = entry.value2;
        dp = entry.value1;
        
        if (dp == 0x03) {
            *prio |= BCM_PRIO_BLACK;  /* Black */
        } else if (dp == 0x02) {
            *prio |= BCM_PRIO_RED;  /* Red  */
        } else if (dp == 0x01) {
            *prio |= BCM_PRIO_YELLOW;  /* Yellow  */
        } else if (dp == 0x0) {
            *prio |= BCM_PRIO_GREEN;  /* Green */
        }
        
        if (tbl_ndx == 1) {
            *prio |= BCM_PRIO_SECONDARY; /*Secondary */
        }        
    } else {    
        /* Retrive local PP ports */
        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);  

        /* Get the In-PP-Port. TC-DP-TOS-Index */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

            soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            tbl_ndx = port_info.l3_info.ip_qos_to_tc_index;

            if (tbl_ndx == 1) {
                *prio = BCM_PRIO_SECONDARY; /*Secondary */
            } else if (tbl_ndx > 1) {
                *prio = -1;
            }        
        }  
    }     

exit:
    BCMDNX_FUNC_RETURN;
}

/* This API works only in PP mode */
int 
bcm_petra_port_l3_mtu_get(
    int unit, 
    bcm_port_t port, 
    int *size)
{
    SOC_PPC_PORT_INFO port_info;
    int port_i, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
      soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      *size = port_info.mtu;
      break; /* return size for first*/
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* This API works only in PP mode */
int 
bcm_petra_port_l3_mtu_set(
    int unit, 
    bcm_port_t port, 
    int size)
{
    SOC_PPC_PORT_INFO
    port_info;
    int port_i, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
      soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      port_info.mtu = size;
      port_info.flags = SOC_PPC_PORT_EGQ_PP_PPCT_TBL;
      soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}

/* This API works only in PP mode */
int 
bcm_petra_port_l3_encapsulated_mtu_get(
    int unit, 
    bcm_port_t port, 
    int *size)
{
    SOC_PPC_PORT_INFO port_info;
    int port_i, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
      soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      *size = port_info.mtu_encapsulated;
      break; /* return size for first*/
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* This API works only in PP mode */
int 
bcm_petra_port_l3_encapsulated_mtu_set(
    int unit, 
    bcm_port_t port, 
    int size)
{
    SOC_PPC_PORT_INFO
    port_info;
    int port_i, core;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
      soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      port_info.mtu_encapsulated = size;
      port_info.flags = SOC_PPC_PORT_EPNI_PP_PCT_TBL;
      soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * given flags, map to the trap code to be used to acheive these flags.
 */
STATIC 
 int bcm_petra_port_learn_flags_to_trap_code(
     int unit,
     uint32 flags
 )
{
    /* learn and forward */
    if((flags & BCM_PORT_LEARN_FWD) && (flags & BCM_PORT_LEARN_ARL)) {
        return SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0;
    }

    /* neither: drop and don't learn*/
    if((flags & (BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD)) == 0) {
        return SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3;
    }
    /* only learn */
    if((flags & BCM_PORT_LEARN_FWD) == 0) {
        return SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2;
    }
    /* only forward */
    if((flags & BCM_PORT_LEARN_ARL) == 0) {
        return SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1;
    }

    return SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0;
}

/*
 * map from trap code to learn_flags.
 */
STATIC 
 int bcm_petra_port_learn_flags_from_trap_code(
     int unit,
     uint32 trap_code,
     uint32 *flags
 )
{
    *flags = 0;
    switch(trap_code) {
    case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0:
        *flags = BCM_PORT_LEARN_FWD|BCM_PORT_LEARN_ARL;
        break;
    case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1:
        *flags = BCM_PORT_LEARN_FWD;
        break;
    case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2:
        *flags = BCM_PORT_LEARN_ARL;
        break;
    case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3:
        *flags = 0;
        break;
    default:
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/*
* given ppd-port return which flags this port is mapping to
*/
STATIC int 
_bcm_petra_pp_port_flags_get(
    int unit,
    int port_i,
    uint32 *flags)
{
    SOC_PPC_FRWRD_MACT_PORT_INFO
    mact_port_info;
    uint32
      soc_sand_rv;
    int soc_sand_dev_id = unit;
    SOC_PPC_PORT soc_ppd_port_i;
    int rv = BCM_E_NONE, core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

    soc_sand_rv = soc_ppd_frwrd_mact_port_info_get(soc_sand_dev_id,core,soc_ppd_port_i,&mact_port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    rv = bcm_petra_port_learn_flags_from_trap_code(
        unit,
        mact_port_info.sa_unknown_action_profile + SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0,
        flags
    );
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int 
_bcm_petra_gport_lif_learn_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags)
{    
    int rv = BCM_E_NONE;
    uint8 learn_enable;
    int is_local;
    SOC_PPC_LIF_ENTRY_INFO lif_info;
    uint32
      soc_sand_rv;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

   BCMDNX_INIT_FUNC_DEFS;

   *flags = 0;

   /* map gport to LIF */
   rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
   BCMDNX_IF_ERR_EXIT(rv);

   if (!is_local) {
     /* API is releavant only for local configuration */
     BCM_EXIT;
   }

   rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
   BCMDNX_IF_ERR_EXIT(rv); 

   /* update LIF attribute */
   soc_sand_rv = soc_ppd_lif_table_entry_get(unit,gport_hw_resources.local_in_lif,&lif_info);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   if(lif_info.type & (SOC_PPC_LIF_ENTRY_TYPE_AC|SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)){
       learn_enable = lif_info.value.ac.learn_record.learn_type != SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
   }
   else if(lif_info.type & SOC_PPC_LIF_ENTRY_TYPE_PWE){
       learn_enable = lif_info.value.pwe.learn_record.enable_learning != SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
   }
   else if(lif_info.type & SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF){
       learn_enable = lif_info.value.ip_term_info.learn_enable;
   }
   else if(lif_info.type & SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF){
       learn_enable = lif_info.value.mpls_term_info.learn_enable;
   }
   else if(lif_info.type & SOC_PPC_LIF_ENTRY_TYPE_ISID){
       learn_enable = lif_info.value.isid.learn_enable;
   }
   else if(lif_info.type & SOC_PPC_LIF_ENTRY_TYPE_EXTENDER){
       learn_enable = lif_info.value.extender.learn_record.learn_type != SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
   }
   else{
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: cannot enable learning for this LIF type"), FUNCTION_NAME()));
   }

   /* for gport lif, destination is forwarding */
   *flags |= BCM_PORT_LEARN_FWD;
   if(learn_enable) {
       *flags |= BCM_PORT_LEARN_ARL;
   }

exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_learn_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags)
{
    int port_i;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if the supplied gport is not simple (i.e. it;s a LIF) handle it in special function*/
    if (gport_parse_info.type != _bcmDppGportParseTypeSimple) {
        rv = _bcm_petra_gport_lif_learn_get(unit,port,flags);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
       *flags = 0;
       rv = _bcm_petra_pp_port_flags_get(unit,port_i,flags);
       BCMDNX_IF_ERR_EXIT(rv);
       /* if not using trap code, then flags as in default*/
       if(*flags == 0xf ) {
           *flags = BCM_PETRA_PORT_LEARN_DFLT_FLGS;
       }
       break; /* get flags for first */
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_learn_modify(
    int unit, 
    bcm_port_t port, 
    uint32 add, 
    uint32 remove)
{
    uint32 flags;
    SOC_PPC_PORT port_i;
    SOC_PPC_FRWRD_MACT_PORT_INFO mact_port_info;
    uint32 sa_unknown_action_profile;
    int soc_sand_rv = 0;
    SOC_PPC_PORT_INFO port_info;
    int learn_enable;
    int rv = BCM_E_NONE;
    int core = SOC_CORE_INVALID;
    SOC_PPC_PORT soc_ppd_port = 0;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if((add & (~(BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD))) != 0 ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: unsupported learn flags %x"),FUNCTION_NAME(), add));
    } 

    /* nothing to do */
    if(add == 0  && remove == 0) {
        BCM_EXIT;
    }

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        /* get */
        flags = 0;

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        rv = _bcm_petra_pp_port_flags_get(unit,port_i,&flags);
        BCMDNX_IF_ERR_EXIT(rv);

        /* update */
        flags |= add;
        flags &= ~remove;

        /* set */
        sa_unknown_action_profile = bcm_petra_port_learn_flags_to_trap_code(unit,flags) - SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0;

        /* Set new port-trap mapping */
        soc_sand_rv = soc_ppd_frwrd_mact_port_info_get(unit,core,soc_ppd_port,&mact_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        mact_port_info.sa_unknown_action_profile = sa_unknown_action_profile;
        soc_sand_rv = soc_ppd_frwrd_mact_port_info_set(unit,core,soc_ppd_port,&mact_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* port info setting */

        /* new learn status */
        learn_enable = (flags & BCM_PORT_LEARN_ARL)?1:0;

        /* update learn info if required */
        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (port_info.enable_learning != learn_enable) {
            port_info.enable_learning = learn_enable;
            port_info.flags = SOC_PPC_PORT_IHB_PINFO_FLP_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * control the handling of packet with unknown SA. 
 *   action can be to:
 *      - drop/forward packet.
 *      - learn/not learn SA.
 *  
 * port can be: 
 *    - local pp port.
 *    - local gport (AC, PWE,...)
 */
STATIC int 
_bcm_petra_gport_lif_learn_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags)
{
   int rv = BCM_E_NONE;
   uint8 learn_enable;
   int is_local;
   SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
   int learn_gport = 0;
   SOC_PPC_FRWRD_DECISION_INFO* learn_info_ptr = NULL;
   uint32 soc_sand_rv;
   _bcm_dpp_gport_hw_resources gport_hw_resources;

   BCMDNX_INIT_FUNC_DEFS;   

   /* only learn disable/enable suport packet has to be forward */
   if((flags & (~(BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD))) != 0 ) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: unsupported learn flags %x"),FUNCTION_NAME(), flags));
   } 

   /* packet forwarding cannot be change according to learn event */
   if(!(flags & BCM_PORT_LEARN_FWD)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: unsupported learn flags %x, FWD has to be present for LIF learn control "),FUNCTION_NAME(), flags));
   } 

   learn_enable = (flags & BCM_PORT_LEARN_ARL) != 0;

   /* map gport to LIF */
   rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
   BCMDNX_IF_ERR_EXIT(rv);
  
   if (!is_local) {
     /* API is releavant only for local configuration */
     BCM_EXIT;
   }

   rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
   BCMDNX_IF_ERR_EXIT(rv);    

    BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_gport_lif_learn_set.lif_info");
    if (lif_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

   /* update LIF attribute */
   soc_sand_rv = soc_ppd_lif_table_entry_get(unit,gport_hw_resources.local_in_lif, lif_info);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   if(lif_info->type & (SOC_PPC_LIF_ENTRY_TYPE_AC|SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)){

       if(lif_info->value.ac.service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP && learn_enable) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: cannot enable learning for P2P service "), FUNCTION_NAME()));
       }
       lif_info->value.ac.learn_record.learn_type = (learn_enable)?SOC_PPC_L2_LIF_AC_LEARN_INFO:SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
       
       if (learn_enable) {
           learn_info_ptr = &lif_info->value.ac.learn_record.learn_info;
       }
   }
   else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) {

       if (lif_info->value.extender.service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP && learn_enable) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: cannot enable learning for P2P service "), FUNCTION_NAME()));
       }
       lif_info->value.extender.learn_record.learn_type = (learn_enable)? SOC_PPC_L2_LIF_AC_LEARN_INFO : SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
       
       if (learn_enable) {
           learn_info_ptr = &lif_info->value.extender.learn_record.learn_info;
       }
   }
   else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_PWE){
       if(lif_info->value.pwe.service_type != SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP && learn_enable) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: cannot enable learning for P2P service "), FUNCTION_NAME()));
       }
       lif_info->value.pwe.learn_record.enable_learning = (learn_enable)?SOC_PPC_L2_LIF_AC_LEARN_INFO:SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
       if (learn_enable) {
           learn_info_ptr = &lif_info->value.pwe.learn_record.learn_info;
       }
   }
   else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF){
       lif_info->value.ip_term_info.learn_enable = learn_enable;
        /* Update froward decision information if learn is enabled  */
       if (learn_enable) {
           learn_info_ptr = &lif_info->value.ip_term_info.learn_record;
       }
   }
   else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF){
       lif_info->value.mpls_term_info.learn_enable = learn_enable;
   }
   else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_ISID){
       lif_info->value.isid.learn_enable = learn_enable;
   }
   else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK){
       lif_info->value.trill.learn_enable = learn_enable;
   }
   else{
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: cannot enable learning for this LIF type"), FUNCTION_NAME()));
   }

   /* 
     *  HW save this information from the learning information. Depends on LIF type we can try to retreive learn information if exist
     *  Type          | Learn information
     *  AC, EXTENDER  | Take from learn record in case learning is done per AC
     *  PWE           | Take once PWE learn is enabled
     *  IP Tunnel RIF | Take once IP tunnel learn is enabled
     */
   if (learn_enable && (learn_info_ptr != NULL)) {
       if ((lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_AC)
            && (lif_info->value.ac.learn_record.learn_info.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC)) {
            /* for AC 1+1 protection, doesn't update learn info */
       } else {
           rv = _bcm_dpp_local_lif_sw_resources_learn_gport_get(unit, gport_hw_resources.local_in_lif, &learn_gport);
           BCMDNX_IF_ERR_EXIT(rv);

           SOC_PPC_FRWRD_DECISION_INFO_clear(learn_info_ptr);
           rv = _bcm_dpp_gport_to_fwd_decision(unit,learn_gport, learn_info_ptr);
           BCMDNX_IF_ERR_EXIT(rv);
       }
   }

   /* update LIF attribute */
   soc_sand_rv = soc_ppd_lif_table_entry_update(unit,gport_hw_resources.local_in_lif, lif_info);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCM_FREE(lif_info);
   BCMDNX_FUNC_RETURN;
}


/*
 * control the handling of packet with unknown SA. 
 *   action can be to:
 *      - drop/forward packet.
 *      - learn/not learn SA.
 *  
 * port can be: 
 *    - local pp port.
 *    - local gport (AC, PWE,...)
 */
int 
bcm_petra_port_learn_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags)
{
    int rv = BCM_E_NONE;
    SOC_PPC_FRWRD_MACT_PORT_INFO mact_port_info;
    uint32 soc_sand_rv;
    SOC_PPC_PORT port_i;
    uint32 sa_unknown_action_profile;
    SOC_PPC_PORT_INFO port_info;
    int learn_enable;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    int core = SOC_CORE_INVALID;
    SOC_PPC_PORT soc_ppd_port = 0;


   BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* only ARL and FWD is supported */

   /*       Flags
    *  ARL      FWD     |     ACTION            | TRAP CODE
    *   V        V      |  learn & Forward      | bcmRxTrapL2Learn0
    *   X        V      |  don't learn & Forward| bcmRxTrapL2Learn1
    *   V        X      |  learn & Drop         | bcmRxTrapL2Learn2
    *   X        X      |  don't learn & Drop   | bcmRxTrapL2Learn3
    */

   rv = _bcm_dpp_gport_parse(unit,port, &gport_parse_info);
   BCMDNX_IF_ERR_EXIT(rv);

   /* if the supplied gport is not simple (i.e. it;s a LIF) handle it in special function*/
   if (gport_parse_info.type != _bcmDppGportParseTypeSimple) {
       rv = _bcm_petra_gport_lif_learn_set(unit,port,flags);
       BCMDNX_IF_ERR_EXIT(rv);
       BCM_EXIT;
   }
    /* to get trap/snoop instead drop use  bcm_petra_rx_trap_set with relevant trap code */
    if((flags & (~(BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD))) != 0 ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: unsupported learn flags %x"),FUNCTION_NAME(), flags));
    } 

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    sa_unknown_action_profile = bcm_petra_port_learn_flags_to_trap_code(unit,flags) - SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0;
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        /* Set new port-trap mapping */
        soc_sand_rv = soc_ppd_frwrd_mact_port_info_get(unit,core,soc_ppd_port,&mact_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        mact_port_info.sa_unknown_action_profile = sa_unknown_action_profile;
        soc_sand_rv = soc_ppd_frwrd_mact_port_info_set(unit,core,soc_ppd_port,&mact_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* port info setting */
        /* new learn status */
        learn_enable = (flags & BCM_PORT_LEARN_ARL)?1:0;
        /* update learn info if required */
        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (port_info.enable_learning != learn_enable) {
            port_info.enable_learning = learn_enable;
            port_info.flags = SOC_PPC_PORT_IHB_PINFO_FLP_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

exit:
   BCMDNX_FUNC_RETURN;
}

int bcm_petra_port_policer_get(
    int unit, 
    bcm_port_t port, 
    bcm_policer_t *policer_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(bcm_petra_policer_port_get(unit, port, policer_id));

exit:
    BCMDNX_FUNC_RETURN;
}

/* Set the Policer ID accociated for the specified physical port. */
int bcm_petra_port_policer_set(
    int unit, 
    bcm_port_t port, 
    bcm_policer_t policer_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(bcm_petra_policer_port_set(unit, port, policer_id));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_stp_set
 * Purpose:
 *      Set the spanning tree state for a port.
 *      All STGs containing all VLANs containing the port are updated.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      stp_state - State to place port in, one of BCM_PORT_STP_xxx.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 * Notes:
 *      BCM_LOCK is taken so that the current list of VLANs
 *      can't change during the operation.
 */

int
bcm_petra_port_stp_set(int unit, bcm_port_t port, int stp_state)
{
    bcm_stg_t           *list = NULL;
    int                 count = 0, i;
    int                 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* BCM_LOCK(unit); */

    rv = bcm_petra_stg_list(unit, &list, &count);

    if (rv == BCM_E_UNAVAIL) {
        if (stp_state == BCM_STG_STP_FORWARD) {
            rv = BCM_E_NONE;
        } else {
            rv = BCM_E_PARAM;
        }
    } else if (BCM_SUCCESS(rv)) {
        for (i = 0; i < count; i++) {
            if ((rv = bcm_petra_stg_stp_set(unit, list[i], 
                                          port, stp_state)) < 0) {
                break;
            }
        }
    }

    bcm_petra_stg_list_destroy(unit, list, count);
    /* BCM_UNLOCK(unit); */

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "port %d, state %d rv %d"), port, stp_state, rv));

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_stp_get
 * Purpose:
 *      Get the spanning tree state for a port in the default STG.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      stp_state - Pointer where state stored.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_petra_port_stp_get(int unit, bcm_port_t port, int *stp_state)
{
    int stg_defl, rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_stg_default_get(unit, &stg_defl);
    if (rv >= 0) {
        rv = bcm_petra_stg_stp_get(unit, stg_defl, port, stp_state);
    } else if (rv == BCM_E_UNAVAIL) {   /* FABRIC switches, etc */
        *stp_state = BCM_STG_STP_FORWARD;
        rv = BCM_E_NONE;
    }

    LOG_DEBUG(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "port %d, state %d rv %d"), port, *stp_state, rv));

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * allocate tpid value
 */
int
bcm_petra_port_tpid_val_alloc(
       int unit, uint16 tpid[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int tpid_indx[_BCM_PETRA_NOF_TPIDS_PER_PORT]
    )
{
    int indx, nof_values=0, vals_indx, tpid_i = 0;
    int soc_sand_rv = 0;
    uint16 used_tpid_vals[_BCM_PORT_NOF_TPID_VALS + 1] = {0}; /* '0' Can be included as a TPID value in unallocatd TPID Profiles */
    uint16 tpid_val;
    SOC_PPC_LLP_PARSE_TPID_VALUES
    tpid_vals;
    int new_val;
    int tpid_found[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    int nof_filled = 0;
    int rv, old_profile, new_profile, is_last, is_allocated;
    bcm_dpp_vlan_egress_edit_profile_info_t mapping_profile;
    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY command_key;
    SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO command_info;
    SOC_PPC_EG_AC_INFO ac_info;
    int tpid_count;
    int vlan_format;
    
    BCMDNX_INIT_FUNC_DEFS;    

    /* check if such TPID already exist */
    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit,&tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "allocate tpid values: for tpid[0]=%d tpid[1]=%d nof_tpids=%d \n"),
                 tpid[0], tpid[1], nof_tpids));

    
    for(indx = 0; indx < SOC_PPC_LLP_PARSE_NOF_TPID_VALS - 1; ++indx) {

        for(tpid_i = 0 ; tpid_i < nof_tpids && nof_filled < nof_tpids;  ++tpid_i) {
            if(tpid_vals.tpid_vals[indx] == tpid[tpid_i] )
            {
                tpid_indx[tpid_i] = indx;
                tpid_found[tpid_i] = 1;
                ++nof_filled;
                /* mark it as in use */
                used_tpid_vals[nof_values++] = tpid[tpid_i];
            }
        }
    }

    /* done?*/
    if(nof_filled == nof_tpids) {
        LOG_DEBUG(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "TPIDs already exist\n")));
        BCM_EXIT;
    }

    /* go over existing tpid-profiles and check which is in use */
    for(indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES*2; ++indx) {
         rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, indx/_BCM_PORT_NOF_TPID_PROFILES, indx % _BCM_PORT_NOF_TPID_PROFILES, &tpid_count);
         BCMDNX_IF_ERR_EXIT(rv);
         if(tpid_count != 0) {
            new_val = 1;
            rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, indx/_BCM_PORT_NOF_TPID_PROFILES, indx % _BCM_PORT_NOF_TPID_PROFILES, &tpid_val);
            BCMDNX_IF_ERR_EXIT(rv);
    
            /* check if new different value */
            for(vals_indx= 0; vals_indx < nof_values; ++vals_indx) {
                if(tpid_val == used_tpid_vals[vals_indx]){
                    new_val = 0;
                    break;
                }
            }
            if(new_val) {
                used_tpid_vals[nof_values++] = tpid_val;
            }
        }
    }

    /* now check if there is tpid-entry not in use */
    for(indx = 0; (indx < SOC_PPC_LLP_PARSE_NOF_TPID_VALS - 1) && (nof_filled < nof_tpids); ++indx) {
        for(vals_indx= 0; vals_indx < nof_values && nof_filled < nof_tpids; ++vals_indx) {
            if(tpid_vals.tpid_vals[indx] == used_tpid_vals[vals_indx]){
                break;
            }
        }
        /* found unused index */
        if(vals_indx == nof_values) {
            /* fill it */
            for(tpid_i = 0 ; tpid_i < nof_tpids;  ++tpid_i) {
                if(tpid_found[tpid_i] != 1)
                {
                    tpid_vals.tpid_vals[indx] = tpid[tpid_i];
                    tpid_found[tpid_i] = 1;
                    tpid_indx[tpid_i] = indx;
                    ++nof_filled;
                    break;
                }
            }
        }   
    }

    if(nof_filled == nof_tpids) {
        soc_sand_rv = soc_ppd_llp_parse_tpid_values_set(unit, &tpid_vals);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (MIM_IS_INIT(unit) && SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
            SOC_PPC_AC_ID mim_local_out_ac;
            MIM_ACCESS.mim_local_out_ac.get(unit, &mim_local_out_ac);

            /* go over tpids to find if one of them is the B-tag tpid */
            for (tpid_i = 0; tpid_i < _BCM_PORT_NOF_TPID_VALS; ++tpid_i) {
                if (tpid_vals.tpid_vals[tpid_i] == BCM_PETRA_MIM_BTAG_TPID) {
                    break;
                }
            }
            if (tpid_i != _BCM_PORT_NOF_TPID_VALS) { /* B-tag tpid was found */

                _bcm_dpp_vlan_egress_edit_profile_info_t_init(&mapping_profile);
                rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_data_get(unit, mim_local_out_ac, &mapping_profile);
                BCMDNX_IF_ERR_EXIT(rv);

                vlan_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;

                /* get a profile to config vlan_edit_command */
                mapping_profile.evec[vlan_format].tags_to_remove = 0;
                mapping_profile.evec[vlan_format].outer_tag.tpid_index = tpid_i;
                mapping_profile.evec[vlan_format].outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_VSI;
                   
                rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_exchange(unit, mim_local_out_ac, &mapping_profile, &old_profile, &is_last, 
                                                                                    &new_profile, &is_allocated);
                BCMDNX_IF_ERR_EXIT(rv);

                /* set vlan_edit_command for encapsulation of B-tag */
                SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(&command_key);

                command_key.edit_profile = new_profile;
                command_key.tag_format = vlan_format;

                soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_get(unit, &command_key, &command_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                command_info.tags_to_remove = 0;
                command_info.outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_VSI;
                command_info.outer_tag.tpid_index = tpid_i; /* B-tag tpid */
                command_info.outer_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
                command_info.inner_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY;
                command_info.inner_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;

                soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_set(unit, &command_key, &command_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* if not default, set AC to point at new profile */
                if (new_profile != 0) {
                    soc_sand_rv = soc_ppd_eg_ac_info_get(unit, mim_local_out_ac, &ac_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    ac_info.edit_info.edit_profile = new_profile;

                    soc_sand_rv = soc_ppd_eg_ac_info_set(unit, mim_local_out_ac, &ac_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }

        BCM_EXIT;
    }

    /* didn't manage to allocate */
    BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("didn't manage to allocate")));

exit:
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "allocate tpid values: at indexes %d %d\n"),
                 tpid_indx[0],tpid_indx[1]));

    BCMDNX_FUNC_RETURN;
}


/* 
 * Search for exact match TPID profile
 */
int
bcm_petra_port_tpid_profile_exact_match_search(int unit, int indx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match)
{
  bcm_error_t rv;
  int tpid_count1;
  int tpid_count0;
  uint16 tpid1;
  uint16 tpid0;    
  
  BCMDNX_INIT_FUNC_DEFS;

  *is_exact_match = 0;
  /* full double match */
  if(nof_tpids == 2) {
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, indx, &tpid_count0);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 1, indx, &tpid_count1);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0, indx, &tpid0);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1, indx, &tpid1);
      BCMDNX_IF_ERR_EXIT(rv);

      if( (tpid0 == tpids[0] && tpid_count0 !=0 ) && (tpid1 == tpids[1] && tpid_count1 !=0 )) {
          *is_exact_match = 1;          
      }
  }
  /* full one match */
  if(nof_tpids == 1) {
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, inner_only, indx, &tpid_count0);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, inner_only, indx, &tpid0);
      BCMDNX_IF_ERR_EXIT(rv);

      /* Whether inner_only is set or not, tpids[0] always represent the first TPID */
      if( (tpid0 == tpids[0] && tpid_count0 !=0 ) ) {
          *is_exact_match = 1;          
      }
  }

exit:
  BCMDNX_FUNC_RETURN;       
}

/* 
 * Search for first match TPID profile
 */
int
bcm_petra_port_tpid_profile_first_match_search(int unit, int indx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_first_match)
{
  bcm_error_t rv;
  int tpid_count0;
  uint16 tpid0; 
  
  BCMDNX_INIT_FUNC_DEFS;
   

  *is_first_match = 0;

  if(nof_tpids == 1) {
     rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, indx, &tpid_count0);
     BCMDNX_IF_ERR_EXIT(rv);
     rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0, indx, &tpid0);
     BCMDNX_IF_ERR_EXIT(rv);
     if (tpid0 == tpids[0] && tpid_count0 != 0 ) {     
         *is_first_match = 1;
     }
  }

exit:
  BCMDNX_FUNC_RETURN;       
}

/* 
 * Search for exact match TPID profile in opposite direction.
 */
int
bcm_petra_port_tpid_profile_opposite_match_search(int unit, int indx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match)
{
  bcm_error_t rv;
  int tpid_count1;
  int tpid_count0;
  uint16 tpid1;
  uint16 tpid0;    
  int index_opp;
  
  BCMDNX_INIT_FUNC_DEFS;

  *is_exact_match = 0;
  /* Full double match in opposite direction */
  if(nof_tpids == 2) {
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, indx, &tpid_count0);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 1, indx, &tpid_count1);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0, indx, &tpid0);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1, indx, &tpid1);
      BCMDNX_IF_ERR_EXIT(rv);

      if( (tpid0 == tpids[1] && tpid_count0 !=0 ) && (tpid1 == tpids[0] && tpid_count1 !=0 )) {
          *is_exact_match = 1;          
      }
  }

  /* full one match in opposite direction */
  if(nof_tpids == 1) {
      index_opp = (inner_only == 0) ? 1 : 0;
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, index_opp, indx, &tpid_count0);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, index_opp, indx, &tpid0);
      BCMDNX_IF_ERR_EXIT(rv);

      if( (tpid0 == tpids[0] && tpid_count0 !=0 ) ) {
          *is_exact_match = 1;        
      }
  }

exit:
  BCMDNX_FUNC_RETURN;       
}


/* 
 * Given a pair of similar TPIDs, look for the TPID at any of the Profile TPIDs.
 */
int
bcm_petra_port_tpid_profile_similar_match_search(int unit, int tpid_profile_idx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_match)
{
    bcm_error_t rv;
    int outer_tpid_count, inner_tpid_count;
    uint16 outer_tpid, inner_tpid;
  
    BCMDNX_INIT_FUNC_DEFS;

    *is_match = 0;

    /* The check is applicable only if the suuplied outer and inner TPIDs are similar */
    if ((nof_tpids == 2) && (tpids[0] == tpids[1])) {

        /* Get the TPID data for the given TPID profile */
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, tpid_profile_idx, &outer_tpid_count);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 1, tpid_profile_idx, &inner_tpid_count);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0, tpid_profile_idx, &outer_tpid);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1, tpid_profile_idx, &inner_tpid);
        BCMDNX_IF_ERR_EXIT(rv);

        if (((tpids[0] == outer_tpid) && outer_tpid_count) || 
            ((tpids[0] == inner_tpid) && inner_tpid_count)) {
          *is_match = 1;
        }
    }

exit:
  BCMDNX_FUNC_RETURN;
}


/*
 * assign tpid profile, for the given tpid values
 */
int
bcm_petra_port_tpid_profile_alloc(int unit,
                    uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT],
                    int nof_tpids, int inner_only, int *tpid_profile)
{
   int d_match_indx=0, part_match_indx=0, first_empty_indx=0, indx;
   int d_match_valid=0, part_match_valid=0, first_empty_valid=0;
   int tpid_indx[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
   uint16 alloc_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
   int soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO
       tpid_profile_info;
   int tpid_count;
   int tpid_count0;
   int tpid_count1;
   uint16 tpid;

   BCMDNX_INIT_FUNC_DEFS;
   LOG_VERBOSE(BSL_LS_BCM_PORT,
               (BSL_META_U(unit, "allocate tpid profile for tpid[0]=0x%08x "
                                 "tpid[1]=0x%08x nof_tpids=%d inner_only=%d\n"),
                tpids[0], tpids[1], nof_tpids, inner_only));

   for(indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES; ++indx) {

       rv = bcm_petra_port_tpid_profile_exact_match_search(unit, indx, tpids,
                                        nof_tpids, inner_only, &d_match_valid);
       BCMDNX_IF_ERR_EXIT(rv);

       if (d_match_valid) {
          /* full double match or one match */
          d_match_indx = indx;
          break;
       }

       /* if already found partial match, skip below checks*/
       if(part_match_valid) {
           continue;
       }

       /* partial match and second part is empty use it*/
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 1, indx, &tpid_count1);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, indx, &tpid_count0);
        BCMDNX_IF_ERR_EXIT(rv);

       if(nof_tpids == 2 && tpid_count1 == 0)
       {
          rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0, indx, &tpid);
          BCMDNX_IF_ERR_EXIT(rv);
          rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, indx, &tpid_count);
          BCMDNX_IF_ERR_EXIT(rv);
           if( (tpid == tpids[0] && tpid_count != 0 ) ) {
               part_match_indx = indx;
               part_match_valid = 1;
           }
       }
       

       /* if already found empty, skip below check*/
       if(first_empty_valid) {
           continue;
       }

       if( tpid_count0 == 0 && tpid_count1 == 0) {
           first_empty_indx = indx;
           first_empty_valid = 1;
       }
   }

   /* use best match from the above*/

   if(d_match_valid) {
       *tpid_profile = d_match_indx;
       LOG_VERBOSE(BSL_LS_BCM_PORT,
                   (BSL_META_U(unit,
                               "exact match TPID-profile =%d\n"),
                    *tpid_profile));
       BCM_EXIT;
   }
   else if(part_match_valid) {
       *tpid_profile = part_match_indx;
       /* alloc missing TPID */
       alloc_tpids[0] = tpids[1];
       rv = bcm_petra_port_tpid_val_alloc(unit, alloc_tpids, 1,tpid_indx);
       BCMDNX_IF_ERR_EXIT(rv);
       rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.set(unit, 1, *tpid_profile, tpids[1]);
       BCMDNX_IF_ERR_EXIT(rv);
       LOG_VERBOSE(BSL_LS_BCM_PORT,
                   (BSL_META_U(unit,
                               "reuse profile =%d\n, set tpid=0x%08x as tpid2"),
                    *tpid_profile,tpids[1]));

       /* set TPID profile */
       soc_sand_rv = soc_ppd_llp_parse_tpid_profile_info_get(unit,*tpid_profile,&tpid_profile_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       tpid_profile_info.tpid2.index = tpid_indx[0];
       soc_sand_rv = soc_ppd_llp_parse_tpid_profile_info_set(unit,*tpid_profile,&tpid_profile_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   }
   else if(first_empty_valid){

       *tpid_profile = first_empty_indx;
       LOG_VERBOSE(BSL_LS_BCM_PORT,
                   (BSL_META_U(unit,
                               "use empty profile =%d\n"),
                    *tpid_profile));

       /* found empty place allocate it */
       SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(&tpid_profile_info);
       tpid_profile_info.tpid1.type = SOC_SAND_PP_VLAN_TAG_TYPE_ANY;
       tpid_profile_info.tpid2.type = SOC_SAND_PP_VLAN_TAG_TYPE_ANY;
       /* alloc missing TPID/s */
       alloc_tpids[0] = (inner_only)?tpids[1]:tpids[0];
       alloc_tpids[1] = tpids[1];
       rv = bcm_petra_port_tpid_val_alloc(unit, alloc_tpids, nof_tpids, tpid_indx);
       BCMDNX_IF_ERR_EXIT(rv);
       rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.set(unit, inner_only, *tpid_profile, tpids[0]);
       BCMDNX_IF_ERR_EXIT(rv);

       if (!inner_only) {
           tpid_profile_info.tpid1.index = tpid_indx[0];
       }
       else
       {
           tpid_profile_info.tpid2.index = tpid_indx[0];
       }

       if(nof_tpids == 2) {
           /* mark as allocated above is in use*/
           tpid_profile_info.tpid2.index = tpid_indx[1];
           rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.set(unit, 1, *tpid_profile, tpids[1]);
           BCMDNX_IF_ERR_EXIT(rv);

       }

       LOG_VERBOSE(BSL_LS_BCM_PORT,
                   (BSL_META_U(unit,
                               "set tpid-profile =%d, with TPID1 = %d, TPID2= %d "),
                    *tpid_profile,tpid_profile_info.tpid1.index, tpid_profile_info.tpid2.index));

       /* set TPID profile */
       soc_sand_rv = soc_ppd_llp_parse_tpid_profile_info_set(unit,*tpid_profile,&tpid_profile_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   }
   else
   {
       BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Can't allocate tpid profile")));
   }
   

exit:
   BCMDNX_FUNC_RETURN;
}

int
bcm_petra_port_tpid_new_tpids_calc(int unit, int entity,
    uint16 old_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_old_tpids,
    _bcm_petra_tpid_profile_t old_profile_type,
    _bcm_petra_tpid_op_t oper, uint16 new_tpid,
    uint16 new_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int *nof_new_tpids,
    _bcm_petra_tpid_profile_t *new_profile_type,
    uint8 cep_port,
    uint8 evb_port /* Port that support both S and C-tags */
    )
{
    int two_outers;
    BCMDNX_INIT_FUNC_DEFS;

    /* assume no changes */
    new_tpids[0] = old_tpids[0];
    new_tpids[1] = old_tpids[1];
    *new_profile_type = old_profile_type;
    *nof_new_tpids = nof_old_tpids;


    LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit,
                "Port %d old values: \n nof_tpids:%d "
                "old_tpids[0]:0x%08x old_tpids[1]:0x%08x profile:%d\n"),
                entity, nof_old_tpids, old_tpids[0], old_tpids[1],
                old_profile_type));

    LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit,
                "Operation: %d tpid:0x%08x\n"),
                oper, new_tpid));

    /* set inner */
    if(oper == _bcm_petra_tpid_op_inner_set) {
        if(nof_old_tpids == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG( "%s: set inner "
                "TPID for entity %d before setting outer-TPID, unit %d"),
                FUNCTION_NAME(), entity, unit));
        }
        if (_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
            /* outer/inner*/
            *new_profile_type = _bcm_petra_tpid_profile_outer_inner;
            *nof_new_tpids = 2;
            new_tpids[0] = old_tpids[0];
            new_tpids[1] = new_tpid;
        } else if (old_profile_type == _bcm_petra_tpid_profile_outer_outer) {
            if(old_tpids[0] == new_tpid) {
                *new_profile_type = _bcm_petra_tpid_profile_inner_outer2;
                new_tpids[0] = old_tpids[0];
                new_tpids[1] = old_tpids[1];
                *nof_new_tpids = 2;
                BCM_EXIT;
            } else if(old_tpids[1] == new_tpid) {
                if (evb_port) {
                    *new_profile_type = _bcm_petra_tpid_profile_outer_inner_c_tag;
                } else {
                    *new_profile_type = _bcm_petra_tpid_profile_outer_inner2;
                }
                new_tpids[0] = old_tpids[0];
                new_tpids[1] = old_tpids[1];
                *nof_new_tpids = 2;
                BCM_EXIT;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
                    "%s: set 2nd outer TPID for entity %d that already includes"
                    " outer/inner, unit %d"), FUNCTION_NAME(), entity, unit));
            }
        } else {
            /* one outer / one inner */
            if(old_tpids[0] == new_tpid) {
                *new_profile_type = _bcm_petra_tpid_profile_outer_inner_same;
            } else if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL &&
                     new_tpid == _BCM_PETRA_TRILL_NATIVE_TPID){
                *new_profile_type = _bcm_petra_tpid_profile_outer_trill_fgl;
            } else {
                *new_profile_type = _bcm_petra_tpid_profile_outer_inner;
            }
            /* outer/inner*/
            *nof_new_tpids = 2;
            new_tpids[0] = old_tpids[0];
            new_tpids[1] = new_tpid;
        }
    } else if(oper == _bcm_petra_tpid_op_outer_set) {
        new_tpids[0] = new_tpid;
        if(nof_old_tpids == 0) {
            if (new_tpid) {
                *nof_new_tpids = 1;
                *new_profile_type =
                    (cep_port && !_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit))?\
                                        _bcm_petra_tpid_profile_outer_c_tag:\
                                        _bcm_petra_tpid_profile_outer;
            } else {
                *nof_new_tpids = nof_old_tpids;
                *new_profile_type = old_profile_type;
            }

        } else {
            *nof_new_tpids = nof_old_tpids;
            *new_profile_type = old_profile_type;
            if(old_tpids[1] == new_tpid &&
               !_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
                *new_profile_type = _bcm_petra_tpid_profile_outer_inner_same;
            }

        }
    } else if(oper == _bcm_petra_tpid_op_outer_add) {
        if(nof_old_tpids == 0) {
             new_tpids[0] = new_tpid;
             *new_profile_type = (cep_port)?_bcm_petra_tpid_profile_outer_c_tag:\
                                            _bcm_petra_tpid_profile_outer;
        } else if(nof_old_tpids == 1) {
            new_tpids[0] = old_tpids[0];
            new_tpids[1] = new_tpid;
            if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL &&
                new_tpid == _BCM_PETRA_TRILL_NATIVE_TPID) {
                *new_profile_type = _bcm_petra_tpid_profile_outer_trill_fgl;
            } else {
                *new_profile_type = _bcm_petra_tpid_profile_outer_outer;
            }
        } else {   /* if(nof_old_tpids >= _BCM_PETRA_NOF_TPIDS_PER_PORT)*/
            /* if new_tpid equal to inner, it's possible */
            if (old_profile_type == _bcm_petra_tpid_profile_outer_inner &&
                new_tpid == old_tpids[1]) {
                if (evb_port) {
                    *new_profile_type = _bcm_petra_tpid_profile_outer_inner_c_tag;
                } else {
                    *new_profile_type = _bcm_petra_tpid_profile_outer_inner2;
                }
                BCM_EXIT;
            }
            if (old_profile_type == _bcm_petra_tpid_profile_inner_outer &&
                new_tpid == old_tpids[0]) {
                *new_profile_type = _bcm_petra_tpid_profile_inner_outer2;
                BCM_EXIT;
            }
            if (old_profile_type == _bcm_petra_tpid_profile_outer_trill_fgl &&
                (new_tpid == old_tpids[0] || new_tpid == old_tpids[1])) {
                *new_profile_type = _bcm_petra_tpid_profile_outer_trill_fgl;
                BCM_EXIT;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("%s: cannot add more"
                " than two TPID values on same entity %d, unit %d"),
                FUNCTION_NAME(), entity, unit));
        }
        *nof_new_tpids = nof_old_tpids + 1;
    } else if(oper == _bcm_petra_tpid_op_outer_add_order) {
        if(nof_old_tpids == 0) {
             new_tpids[0] = new_tpid;
             *new_profile_type = _bcm_petra_tpid_profile_outer;
        } else if(nof_old_tpids == 1) {
            new_tpids[0] = old_tpids[0];
            new_tpids[1] = new_tpid;
            if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL &&
                     new_tpid == _BCM_PETRA_TRILL_NATIVE_TPID) {
                *new_profile_type = _bcm_petra_tpid_profile_outer_trill_fgl;
            } else {
                *new_profile_type = _bcm_petra_tpid_profile_outer_inner;
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("%s: cannot add more"
                " than two TPID values on same port %d, unit %d"),
                FUNCTION_NAME(), entity, unit));
        }
        *nof_new_tpids = nof_old_tpids + 1;
    } else if(oper == _bcm_petra_tpid_op_outer_delete) {
        /* no tpids on the port */
        if(nof_old_tpids == 0) {
            *nof_new_tpids = 0;
            *new_profile_type = _bcm_petra_tpid_profile_none;
            BCM_EXIT;
        }

        two_outers = (old_profile_type == _bcm_petra_tpid_profile_outer_outer);

        /* remove first */
        if(old_tpids[0] == new_tpid) {
            if(!two_outers) {
                /* if the remain is inner only, then also remove it*/
                *nof_new_tpids = 0;
                *new_profile_type = _bcm_petra_tpid_profile_none;
            } else {
                new_tpids[0] = old_tpids[1];
                *nof_new_tpids = nof_old_tpids - 1;
            }
        } else if (two_outers && old_tpids[1] == new_tpid) {
            /* the second is outer, if equal remove it */
            new_tpids[0] = old_tpids[0];
            *nof_new_tpids = nof_old_tpids - 1;
        } else {
            /* nothing was removed */
            new_tpids[0] = old_tpids[0];
            new_tpids[1] = old_tpids[1];
            *nof_new_tpids = nof_old_tpids;
        }
    } else if(oper == _bcm_petra_tpid_op_outer_delete_all) {
        /* also if there inner, it's not valid to remain with inner only*/
        *nof_new_tpids = 0;
        *new_profile_type = _bcm_petra_tpid_profile_none;
    }

exit:
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Port %d new values: \n nof_tpids:%d "
                 "new_tpids[0]:0x%08x new_tpids[1]:0x%08x profile:%d\n"),
                 entity, *nof_new_tpids, new_tpids[0], new_tpids[1],
                 *new_profile_type));

    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_port_update_tpid_counters
 * Purpose:
 *      Update the usage counter for TPIDs that are stored per TPID-Profile
 *      (Outer,Inner - 0,1).
 *      The update retrieves the current counter value for each TPID in the
 *      TPID-Profile and modifies it with the supplied change value.
 * Parameters:
 *      unit    - (IN) Device Number
 *      tpid_profile - (IN) TPID-Profile number
 *      nof_tpids  - (IN) Number of relevant TPIDs in the TPID-Profile
 *      tpid_counter_modify_type - (IN) The type of the required TPID counter
 *          modification. This type is also the value by which the counters will
 *          be modified.
 *              
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_port_update_tpid_counters(
        int unit,
        int tpid_profile,
        int nof_tpids,
        _bcm_dpp_tpid_counter_modify_type_t tpid_counter_modify_type)
{
    bcm_error_t rv;
    int tpid_count;
    BCMDNX_INIT_FUNC_DEFS

    if (nof_tpids >= 1) {
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 0, tpid_profile, &tpid_count);
        BCMDNX_IF_ERR_EXIT(rv);
        tpid_count += tpid_counter_modify_type;
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.set(unit, 0, tpid_profile, tpid_count);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (nof_tpids >= 2) {
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, 1, tpid_profile, &tpid_count);
        BCMDNX_IF_ERR_EXIT(rv);
        tpid_count += tpid_counter_modify_type;
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.set(unit, 1, tpid_profile, tpid_count);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_tpid_profile_modify
 * Purpose: 
 *      Perform a TPID-Profile modification for a Port/LIF.
 *      The function accepts the TPID-Profile and NOF relevant TPIDs for the
 *      current Port/LIF TPID-Profile and the requested TPIDs.
 *      It returns the newly selected TPID-Profile (May not change).
 *      The participating TPID-Profiles are updated accordingly in the HW and
 *      in the SW DBs, as well as the Global TPIDs.
 *      NOTE: The TPID-Profile of the Port/LIF isn't updated in this function.
 * Parameters:
 *      unit    - (IN) Device Number
 *      new_tpids - (IN) Pointer to the selected TPIDs for the Port/LIF
 *      nof_new_tpids - (IN) Number of selected TPIDs for the Port/LIF
 *      cur_tpid_profile - (IN) Current TPID-Profile for the Port/LIF
 *      nof_cur_tpids - (IN) NOF relevant TPIDs in the current TPID-Profile 
 *      new_tpid_profile - (OUT) The returned new TPID-Profile 
 *
 * Returns:
 *      BCM_E_XXX - The expected errors are only from the TPID-Profile
 *      allocation process that is performed by bcm_petra_port_tpid_profile_alloc().
 */
int _bcm_dpp_tpid_profile_modify(
        int unit,
        uint16 *new_tpids,
        int nof_new_tpids,
        uint32 cur_tpid_profile,
        int nof_cur_tpids,
        int *new_tpid_profile)
{
    int rv, alloc_rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Initiate the TPID Profile so we can distinguish a failed profile allocation */
    *new_tpid_profile = _BCM_DPP_PORT_DFLT_TPID_PROFILE;

    /* Decrement the SW DB reference counters for the TPIDs of the current
     * TPID-Profile. This is currently required prior to calling
     * bcm_petra_port_tpid_profile_alloc as the later assumes the current TPID
     * profile was already removed.
     */
    rv = _bcm_dpp_port_update_tpid_counters(unit, cur_tpid_profile,
                          nof_cur_tpids, bcmDppTpidCounterModifyTypeDecrement);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If there are TPIDs to associate, look for an appropriate TPID-Profile in the HW */
    if(nof_new_tpids > 0) {
        /* Use a complicated logic to find a TPID-Profile that matches the
         * supplied TPIDs or allocate a new TPID-Profile. Update the HW with
         * new TPID-Profiles and device TPIDs.
         */
        alloc_rv = bcm_petra_port_tpid_profile_alloc(unit, new_tpids,
                                                     nof_new_tpids, FALSE,
                                                     (int *)new_tpid_profile);

        

        /* On failure, perform TPID SW counters rollback */
        if (alloc_rv != BCM_E_NONE)
        {
            rv = _bcm_dpp_port_update_tpid_counters(unit, cur_tpid_profile,
                          nof_cur_tpids, bcmDppTpidCounterModifyTypeIncrement);
            *new_tpid_profile = _BCM_DPP_TPID_PROFILE_INVALID;
            BCMDNX_ERR_EXIT_MSG(alloc_rv,
                (_BSL_BCM_MSG("TPID-Profile allocation failed")));
        }

        /* Increment the SW DB reference counters for the TPIDs of the selected TPID-Profile. */
        rv = _bcm_dpp_port_update_tpid_counters(unit, *new_tpid_profile,
                          nof_new_tpids, bcmDppTpidCounterModifyTypeIncrement);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_petra_port_tpid_profile_update(int unit, bcm_port_t port, uint16 tpid,
                                    _bcm_petra_tpid_op_t oper)
{
    int port_i;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO tpid_profile_info;
    uint16 old_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    uint16 new_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    int nof_new_tpids,nof_old_tpids;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
    int tpid_profile, accept_mode;
    _bcm_petra_tpid_profile_t old_profile_type, new_profile_type, profile_type;
    _bcm_petra_dtag_mode_t dtag_mode;
    _bcm_petra_ac_key_map_type_t ac_key_map_type;
    int last_appear,first_appear;
    int old_tpid_profile, new_tpid_profile;
    SOC_PPC_PORT soc_ppd_port_i;
    uint8 evb_port,tmp;
    int core;
    uint32 flags;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
        if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags)) {
            continue;
        }
        BCMDNX_IF_ERR_EXIT(
            MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get,
                                 (unit, port_i, &soc_ppd_port_i, &core)));

        /* check wether this port used TPIDs already */
        soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* clean all old usages*/
        soc_sand_rv = soc_ppd_llp_parse_tpid_profile_info_get(unit,
                                    port_info.tpid_profile, &tpid_profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


        if (!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
            rv =  _bcm_dpp_am_template_tpid_profile_data_get(unit, soc_ppd_port_i,
                                            core, &profile_type, &accept_mode,
                                            &dtag_mode, &ac_key_map_type);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            /* get number of tpids */
            tmp = 0;
            rv = PORT_ACCESS.tpids_count.get(unit, core, soc_ppd_port_i, &tmp);
            BCMDNX_IF_ERR_EXIT(rv);

            profile_type = tmp;
        }
        old_profile_type = profile_type;

        /* according to profile type, get the number of used TPIDs and its usage*/
        if(old_profile_type == _bcm_petra_tpid_profile_none) {
            nof_old_tpids = 0;
        } else if ((old_profile_type == _bcm_petra_tpid_profile_outer) ||
                   (old_profile_type == _bcm_petra_tpid_profile_outer_c_tag)) {
            nof_old_tpids = 1;
        } else {
            nof_old_tpids = 2;
        }

        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0,
                                        port_info.tpid_profile, &old_tpids[0]);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1,
                                        port_info.tpid_profile, &old_tpids[1]);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_port_is_evb_port(unit, port_i, &evb_port);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_port_tpid_new_tpids_calc(unit, port_i, old_tpids,
                        nof_old_tpids, old_profile_type, oper, tpid, new_tpids,
                        &nof_new_tpids, &new_profile_type,
                        port_info.port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP,
                        evb_port);
        BCMDNX_IF_ERR_EXIT(rv);

        /* in advance mode, TPID setting doesn't affect port-LLVP profile */
        if(!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
            /* Allocate new TPID profile, Remove old, and add new */
            rv = _bcm_dpp_am_template_tpid_profile_exchange(unit,soc_ppd_port_i,
                            core, new_profile_type, accept_mode, dtag_mode,
                            ac_key_map_type, &old_tpid_profile, &last_appear,
                            &new_tpid_profile, &first_appear);
            BCMDNX_IF_ERR_EXIT(rv);

            port_info.port_profile = new_tpid_profile;
            LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit,
                                          "new Port profile %d\n"),
                        new_tpid_profile));
        }

        /* Match the selected TPIDs with a TPID Profile and update the
         * HW and SW DB accordingly. Retrieve the allocated TPID-Profile.
         */
        rv =_bcm_dpp_tpid_profile_modify(unit, new_tpids, nof_new_tpids,
                        port_info.tpid_profile, nof_old_tpids, &tpid_profile);

        
        /* On failure, perform LLVP Port-Profile rollback in standard VLAN-Edit mode */
        if ((rv != BCM_E_NONE) &&
            (tpid_profile == _BCM_DPP_TPID_PROFILE_INVALID)) {
            /* In Standard VLAN-Edit mode, TPID setting affects the port-LLVP profile */
            if(!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {

                /* remove new, add old */
                BCMDNX_IF_ERR_EXIT(
                    _bcm_dpp_am_template_tpid_profile_exchange(unit,
                                soc_ppd_port_i, core, profile_type, accept_mode,
                                dtag_mode, ac_key_map_type, &old_tpid_profile,
                                &last_appear, &new_tpid_profile,&first_appear));
            }
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("TPID-Profile modify failed")));
        }
        /* If the TPID Profile matching was successful, Update the LLVP Port-Profile
         * in Standard VLAN-Edit mode and update the Port HW with new TPID Profile. */
        if(nof_new_tpids > 0) {

            /* In advanced VLAN-Edit mode, TPID setting doesn't affect port-LLVP profile */
            if(!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
                port_info.port_profile = new_tpid_profile;
                port_info.port_profile_eg = new_tpid_profile;
            }

            /* update the port TPID-Profile */
            if (port_info.tpid_profile != tpid_profile) {
                port_info.tpid_profile = tpid_profile;
            }

            /* update the port tpid profile*/
            port_info.tpid_profile = tpid_profile;
        }

        /* Refer to default tpid-profile when delete all tpid from port */
        if (!nof_new_tpids &&
            (new_profile_type == _bcm_petra_tpid_profile_none)) {
            port_info.tpid_profile = _BCM_DPP_PORT_DFLT_TPID_PROFILE;
        }

        port_info.flags = (SOC_PPC_PORT_IHP_PINFO_LLR_TBL |
                           SOC_PPC_PORT_EPNI_PP_PCT_TBL |
                           SOC_PPC_PORT_IHP_PP_PORT_INFO_TBL);
        soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /*
         * get accept mode for this port

           rv = bcm_petra_port_discard_get(unit,port,&accept_mode);
           BCM_IF_ERROR_RETURN(rv);*/


        if (!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
            rv = bcm_petra_tpid_profile_info_set(unit, port_info.port_profile,
                            port_info.port_profile_eg, new_profile_type,
                            accept_mode, dtag_mode, ac_key_map_type,
                            port_info.port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP,
                            evb_port);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            /* store how many TPID are set on port */
            tmp = (uint8)new_profile_type;
            rv = PORT_ACCESS.tpids_count.set(unit, core, soc_ppd_port_i, tmp);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}




/*
 * Update TPID profile for PWE/I-SID LIF
 */
STATIC int
_bcm_petra_tpid_profile_update(
                int unit, int lif_id, uint16 tpid, _bcm_petra_tpid_op_t oper)
{
    bcm_error_t rv = BCM_E_NONE;
    int soc_sand_rv = 0;
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = 0;
    int tpid_profile = 0, new_tpid_profile;
    _bcm_petra_tpid_profile_t old_profile_type, lif_tpid_type, new_profile_type;
    uint16 old_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    uint16 new_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    int nof_new_tpids,nof_old_tpids;
    int is_pwe = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Retreive old tpid profile tpids and nof_tpids */
    BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO),
                 "_bcm_petra_tpid_profile_update.lif_entry_info");
    if (lif_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                            (_BSL_BCM_MSG("Memory allocation failure")));
    }

    /* Retreive lif profile */
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, lif_id, lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if ((lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_PWE) &&
        (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_ISID)) {
        /* LIF is not PWE nor I-SID */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (
            _BSL_BCM_MSG("Unexpected lif id. Must be of type PWE or I-SID")));
    }

    /* Retreive tpid type */
    rv = _bcm_dpp_in_lif_match_tpid_type_get(unit, &lif_tpid_type, lif_id);
    BCMDNX_IF_ERR_EXIT(rv);
    is_pwe = (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_PWE) ? 1 : 0;


    if (lif_tpid_type != _bcm_petra_tpid_profile_none) {
        /* PWE/I-SID use an assigned tpid profile */
        if (is_pwe) {
            tpid_profile = lif_entry_info->value.pwe.tpid_profile_index;
        }
        else { /* I-SID */
            tpid_profile = lif_entry_info->value.isid.tpid_profile_index;
        }

        nof_old_tpids = (lif_tpid_type == _bcm_petra_tpid_profile_outer) ? 1 : 2;
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0,
                                                tpid_profile, &old_tpids[0]);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1,
                                                tpid_profile, &old_tpids[1]);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        tpid_profile = 0; /* Doesn't matter */
        nof_old_tpids = 0;
    }

    old_profile_type = lif_tpid_type;
    
    /* Calculate the required TPIDs of the TPID-Profile based on the TPIDs
     * in the TPID-Profile and the requested operation.
     */
    rv = bcm_petra_port_tpid_new_tpids_calc(unit, lif_id, old_tpids,
                        nof_old_tpids, old_profile_type, oper, tpid, new_tpids,
                        &nof_new_tpids, &new_profile_type, FALSE, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Match the selected TPIDs with a TPID Profile and update the HW and SW
     * DB accordingly. Retrieve the allocated TPID-Profile.
     */
    rv =_bcm_dpp_tpid_profile_modify(unit, new_tpids, nof_new_tpids,
                                tpid_profile, nof_old_tpids, &new_tpid_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set PWE/I-SID assocation */
    rv = _bcm_dpp_in_lif_match_tpid_type_set(unit, new_profile_type, lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update lif profile */
    if (is_pwe) {
        lif_entry_info->value.pwe.tpid_profile_index = new_tpid_profile;
    } else {
        lif_entry_info->value.isid.tpid_profile_index = new_tpid_profile;
    }

    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, lif_id, lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_port_rx_vlan_tag_outer_tpid_set(int unit, bcm_port_t port,uint16 tpid)
{
    int port_i;
    #if defined PORTMOD_SUPPORT
    portmod_vlan_tag_t vlan_tag;
    #endif
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    uint32 flags;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport_parse_info.type == _bcmDppGportParseTypeSimple){
        /* Retrive local PP ports */
        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (tpid == 0) {
            tpid = 0x8100;
        }
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
            if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags)
                || !SOC_PORT_IS_NETWORK_INTERFACE(flags) || IS_IL_PORT(unit, port_i)
                || IS_QSGMII_PORT(unit, port_i)) {
                continue;
            }
            #if defined PORTMOD_SUPPORT
            rv = portmod_port_vlan_tag_get(unit, port_i, &vlan_tag);
            if(rv != BCM_E_NONE) {
                BCM_ERR_EXIT_NO_MSG(BCM_E_NONE);
            }
            vlan_tag.outer_vlan_tag = tpid;
            rv = portmod_port_vlan_tag_set(unit, port_i, &vlan_tag);
            if(rv != BCM_E_NONE) {
                BCM_ERR_EXIT_NO_MSG(BCM_E_NONE);
            }
            #endif
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/*
* given port, and new TPID and operation to perform on port,
* adding/removing/setting outer/inner TPID
* allocate TPID value, TPID-profile, port-profile, and set Port with these profiles
*/
int
bcm_petra_port_tpid_profile_update(
       int unit, bcm_port_t port, uint16 tpid, _bcm_petra_tpid_op_t oper)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    uint16 new_tpid = 0;

    BCMDNX_INIT_FUNC_DEFS;

   /*
    *  if port is gport (PWE/I-SID) then
    *  map gport --> lif
    *  update lif with tpid-profile
    *
    *  API can handle:
    *  1. Incoming port (physical PP port)
    *  2. PWE (P2P or MP) - user must set port to be a logical gport
    *  3. I-SID
    */
    rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport_parse_info.type == _bcmDppGportParseTypeSimple) { /* Phy port */
        /* port update */
        rv = _bcm_petra_port_tpid_profile_update(unit, port, tpid, oper);
        BCMDNX_IF_ERR_EXIT(rv);
        if(SOC_IS_JERICHO(unit)) {
            rv = bcm_petra_port_tpid_get(unit, port, &new_tpid);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_petra_port_rx_vlan_tag_outer_tpid_set(unit, port, new_tpid);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else { /* PWE or I-SID*/
        _bcm_dpp_gport_hw_resources gport_hw_resources;
        int is_local;

        rv = _bcm_dpp_gport_is_local(unit, port, &is_local);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_local) {
            /* API is releavant only for local configuration */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
            (_BSL_BCM_MSG("Unexpected lif index. should be local only")));
        }

        rv = _bcm_dpp_gport_to_hw_resources(unit, port,
                    _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                    _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                    &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        /* pwe/I-SID update */
        rv = _bcm_petra_tpid_profile_update(unit,
                                gport_hw_resources.local_in_lif, tpid, oper);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get tpid for a gport(PWE  or I-SID)
 */
STATIC int
_bcm_petra_tunnel_lif_tpid_get(
                int unit, int lif_id, uint16 tpid[_BCM_PETRA_NOF_TPIDS_PER_PORT], _bcm_petra_tpid_profile_t *tpid_type  )
{
    bcm_error_t rv = BCM_E_NONE;
    int soc_sand_rv = 0;
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = 0;
    int tpid_profile = 0;
    int is_pwe = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Retreive old tpid profile tpids and nof_tpids */
    BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO),
                 "_bcm_petra_tpid_profile_update.lif_entry_info");
    if (lif_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                            (_BSL_BCM_MSG("Memory allocation failure")));
    }

    /* Retreive lif profile */
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, lif_id, lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if ((lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_PWE) &&
        (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_ISID)) {
        /* LIF is not PWE nor I-SID */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (
            _BSL_BCM_MSG("Unexpected lif id. Must be of type PWE or I-SID")));
    }

    /* Retreive tpid type */
    rv = _bcm_dpp_in_lif_match_tpid_type_get(unit, tpid_type, lif_id);
    BCMDNX_IF_ERR_EXIT(rv);
    is_pwe = (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_PWE) ? 1 : 0;


    if (tpid_type != _bcm_petra_tpid_profile_none) {
        /* PWE/I-SID use an assigned tpid profile */
        if (is_pwe) {
            tpid_profile = lif_entry_info->value.pwe.tpid_profile_index;
        }
        else { /* I-SID */
            tpid_profile = lif_entry_info->value.isid.tpid_profile_index;
        }

        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0,
                                                tpid_profile, &tpid[0]);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1,
                                                tpid_profile, &tpid[1]);
        BCMDNX_IF_ERR_EXIT(rv);

    } 
    
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}

/*
* given a lif, search a tpid profile for the given tpid 
* and update the lif tpid profile index 
* if no tpid profile is found, return error 
*/
int
_bcm_petra_mim_tpid_profile_update(
       int unit, int lif, uint16 tpid
    )
{
    return _bcm_petra_tpid_profile_update(unit, lif, tpid, _bcm_petra_tpid_op_outer_set);
}
/*
 * Function:
 *      bcm_petra_port_tpid_vals_get
 * Description:
 *      Get the the tpids and tpids type associated with the given port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Return Value:_
 *      BCM_E_XXX
 */
int
bcm_petra_port_tpid_vals_get(int unit, bcm_port_t port, uint16 tpid[_BCM_PETRA_NOF_TPIDS_PER_PORT], _bcm_petra_tpid_profile_t *tpid_type )
{
   int port_i, core;
   SOC_PPC_PORT_INFO port_info;
   int rv = BCM_E_NONE;
   _bcm_petra_tpid_profile_t profile_type;
   _bcm_petra_dtag_mode_t dtag_mode;
   int accept_mode;
   uint8 tmp;
   _bcm_petra_ac_key_map_type_t ac_key_map_type;
   SOC_PPC_PORT soc_ppd_port_i;
   int soc_sand_rv;
   _bcm_dpp_gport_info_t gport_info;
   _bcm_dpp_gport_parse_info_t gport_parse_info;

   BCMDNX_INIT_FUNC_DEFS;

   SOC_PPC_PORT_INFO_clear(&port_info);

   rv = _bcm_dpp_gport_parse(unit,port,&gport_parse_info);
   BCMDNX_IF_ERR_EXIT(rv);

   if (gport_parse_info.type != _bcmDppGportParseTypeSimple){
        _bcm_dpp_gport_hw_resources gport_hw_resources;

        rv = _bcm_dpp_gport_to_hw_resources(unit, port,
                   _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                   _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                   &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_petra_tunnel_lif_tpid_get(unit,
                                gport_hw_resources.local_in_lif, tpid, tpid_type);
        BCMDNX_IF_ERR_EXIT(rv);  

        BCM_EXIT;
   }

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

      if (!_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
          rv = _bcm_dpp_am_template_tpid_profile_data_get(unit, soc_ppd_port_i, core, &profile_type, &accept_mode, &dtag_mode, &ac_key_map_type);
          BCMDNX_IF_ERR_EXIT(rv);
      }
      else{
          /* get number of tpids */
          tmp = 0;
          rv = PORT_ACCESS.tpids_count.get(unit, core, soc_ppd_port_i, &tmp);
          BCMDNX_IF_ERR_EXIT(rv);
          profile_type = tmp;
      }

      soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      *tpid_type = profile_type;

      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 0, port_info.tpid_profile, &tpid[0]);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, 1, port_info.tpid_profile, &tpid[1]);
      BCMDNX_IF_ERR_EXIT(rv);

      break;
   }

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_fb_port_tpid_get
 * Description:
 *      Get the default Tag Protocol ID for a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_petra_port_tpid_get(int unit, bcm_port_t port, uint16 *tpid)
{
    uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    _bcm_petra_tpid_profile_t tpid_type;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_port_tpid_vals_get(unit,port,tpids,&tpid_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if(tpid_type == _bcm_petra_tpid_profile_none) {
        *tpid = 0;
    }
    else
    {
        *tpid = tpids[0];
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_tpid_set
 * Description:
 *      Set the default Tag Protocol ID for a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_petra_port_tpid_set(int unit, bcm_port_t port, uint16 tpid)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    
    BCMDNX_IF_ERR_EXIT(bcm_petra_port_tpid_profile_update(unit,port,tpid,_bcm_petra_tpid_op_outer_set));  
      
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_tpid_add
 * Description:
 *      Add allowed TPID for a port.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      tpid         - (IN) Tag Protocol ID
 *      color_select - (IN) Color mode for TPID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_petra_port_tpid_add(int unit, bcm_port_t port,
            uint16 tpid, int color_select)
{
  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);

  LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit,
               "\n\nCall bcm_petra_port_tpid_add: Port %d tpid:0x%08x\n"),
               port, tpid));


  /* check if color_select is valid */
  if (color_select != 0) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid color_select %d"),
                          color_select));
  }
  if (_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
      BCMDNX_IF_ERR_EXIT(bcm_petra_port_tpid_profile_update(unit, port, tpid,
                                        _bcm_petra_tpid_op_outer_add_order));
  } else {
      BCMDNX_IF_ERR_EXIT(bcm_petra_port_tpid_profile_update(unit, port, tpid,
                                        _bcm_petra_tpid_op_outer_add));
  }

exit:
  BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_port_is_tpid_attached_to_port(int unit, bcm_port_t port, uint16 tpid,int *is_exist)
{
  int rv;
  uint16 attached_tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
  _bcm_petra_tpid_profile_t tpid_type;

  BCMDNX_INIT_FUNC_DEFS;
  rv = bcm_petra_port_tpid_vals_get(unit, port, attached_tpids, &tpid_type);
  BCMDNX_IF_ERR_EXIT(rv);

    switch (tpid_type) {
        case _bcm_petra_tpid_profile_outer:
            *is_exist = (tpid == attached_tpids[0]);
            break;

        case _bcm_petra_tpid_profile_outer_inner:
            *is_exist = (tpid == attached_tpids[0]);
            break;

        case _bcm_petra_tpid_profile_outer_inner2:
        case _bcm_petra_tpid_profile_inner_outer2:
        case _bcm_petra_tpid_profile_outer_inner_c_tag:
        case _bcm_petra_tpid_profile_outer_trill_fgl:
            *is_exist = (tpid == attached_tpids[0] || tpid == attached_tpids[1]);
            break;
  
        case _bcm_petra_tpid_profile_outer_outer:
            *is_exist = (tpid == attached_tpids[0] || tpid == attached_tpids[1]);
            break;

        case _bcm_petra_tpid_profile_none:
            *is_exist = 0;
            break;

        default:
            *is_exist = 0;
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("invalid tpid_type %d"),tpid_type));
            break;
  }

exit:
  BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_tpid_delete
 * Description:
 *      Delete allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      tpid - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_petra_port_tpid_delete(int unit, bcm_port_t port, uint16 tpid)
{
  int rv;
  int is_exist = 0;  

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  /* check if the tpid is attached to the port */ 
  _BCM_DPP_TPID_PARSE_ADVANCED_MODE_API_UNAVAIL(unit);

  rv = _bcm_petra_port_is_tpid_attached_to_port(unit, port, tpid, &is_exist);
  BCMDNX_IF_ERR_EXIT(rv);
  if (!is_exist)
  {
      BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tpid isn't attached to the port")));
  } 
    
  BCMDNX_IF_ERR_EXIT(bcm_petra_port_tpid_profile_update(unit,port,tpid,_bcm_petra_tpid_op_outer_delete));

exit:
  BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_tpid_delete_all
 * Description:
 *      Delete all allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 * Return Value:
 *      BCM_E_XXX
 */
int 
bcm_petra_port_tpid_delete_all(int unit, bcm_port_t port)
{
    uint32
        rv;
    bcm_port_tpid_class_t
        port_tpid_class;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    LOG_DEBUG(BSL_LS_BCM_PORT,
              (BSL_META_U(unit,
                          "\n\nCall bcm_petra_port_tpid_delete_all: Port:%d\n"),
               port));

    rv = bcm_petra_port_tpid_profile_update(unit,port,0,_bcm_petra_tpid_op_outer_delete_all);
    BCMDNX_IF_ERR_EXIT(rv);

    /* in advance mode, reset port to default LLVP profile, nothing is tagged */
    if (_BCM_DPP_TPID_PARSE_ADVANCED_MODE(unit)) {
        /* Parse the gport */
        rv = _bcm_dpp_gport_parse(unit, port, &gport_parse_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (gport_parse_info.type == _bcmDppGportParseTypeSimple) { /* defined only for Phy port */
            bcm_port_tpid_class_t_init(&port_tpid_class);
            port_tpid_class.port = port;
            port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
            port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
            rv = bcm_petra_port_tpid_class_set(unit, &port_tpid_class);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_inner_tpid_set
 * Purpose:
 *      Set the expected TPID for the inner tag in double-tagging mode.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_petra_port_inner_tpid_set(int unit, bcm_port_t port, uint16 tpid)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

   BCMDNX_IF_ERR_EXIT(bcm_petra_port_tpid_profile_update(unit,port,tpid,_bcm_petra_tpid_op_inner_set));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_inner_tpid_get
 * Purpose:
 *      Get the expected TPID for the inner tag in double-tagging mode.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - (OUT) Tag Protocol ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_petra_port_inner_tpid_get(int unit, bcm_port_t port, uint16 *tpid)
{
    uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    _bcm_petra_tpid_profile_t tpid_type;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_port_tpid_vals_get(unit,port,tpids,&tpid_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* the inner tpid of VLAN format S_C_tag came from TPID2 */
    if(tpid_type == _bcm_petra_tpid_profile_outer_inner || 
       tpid_type == _bcm_petra_tpid_profile_outer_inner2 || 
       tpid_type == _bcm_petra_tpid_profile_outer_inner_same ||
       tpid_type == _bcm_petra_tpid_profile_outer_inner_c_tag || 
       tpid_type == _bcm_petra_tpid_profile_outer_trill_fgl) {
        *tpid = tpids[1];
    }
    else if(tpid_type == _bcm_petra_tpid_profile_inner_outer2) {
        *tpid = tpids[0];
    }
    else
    {
        *tpid = 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_tpid_class_set
 * Purpose:
 *      Set VLAN tag classification for a port. Given identified TPIDs on
 *      packet set tag-classifier
 * Parameters:
 *      unit - Device number
 *      tpid_class - Pointer to a TPID class structure that includes the port
 *          and the data to set.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_petra_port_tpid_class_set(int unit, bcm_port_tpid_class_t *tpid_class)
{
    uint16 port_tpids[_BCM_DPP_MAX_NOF_USABLE_TPIDS_PER_PORT]; 
    uint16 port_tpid_indexes[2][_BCM_PETRA_PORT_NOF_TPID_VALS]; /* 0 for TPID1 , 1 for TPID2*/
    uint32 port_tpid_nof_indexes[2];
    uint32 priority_types[2], nof_prirority_types=0;
    uint32 indx1,indx2,indx3;
    uint32 tag_stcture_indx;
    uint32 tag_buff, tag_buff_eg;
    SOC_PPC_LLP_PARSE_INFO
        prs_nfo;
    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO
        pkt_frmt_info;
    _bcm_dpp_port_tpid_class_info_t
        tpid_class_buffer;
    _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t
        tpid_class_acceptable_frame_info;
    int old_template,is_last, new_template,is_first;
    int old_tpid_class_tmpl_eg,is_last_tpid_eg,
        new_tpid_class_tmpl_eg,is_first_tpid_eg;
    int old_acceptable_tmpl,new_acceptable_tmpl,is_first_acceptable;
    SOC_PPC_PORT_INFO port_info;
    int local_port;
    SOC_PPC_PORT soc_ppd_port_i;
    int rv = BCM_E_NONE;
    uint32 nof_tpids;
    int soc_sand_rv, core;
    SOC_PPC_EG_FILTER_PORT_INFO eg_filter_port_info;
    uint8 eg_filter_port_accept;
    uint8 is_ingress, is_egress, is_egr_native;
    _bcm_dpp_gport_info_t gport_info;
    uint32 buffer_lcl[1];
    BCMDNX_INIT_FUNC_DEFS;
    
    _BCM_DPP_TPID_PARSE_ADVANCED_MODE_CHECK(unit);

    LOG_DEBUG(BSL_LS_BCM_PORT, (BSL_META_U(unit,
                                "\n\nCall bcm_petra_port_tpid_class_set: Port "
                                "%d tpid1:0x%08x  tpid2:0x%08x tag_format:%d\n"),
              tpid_class->port, tpid_class->tpid1, tpid_class->tpid2,
              tpid_class->tag_format_class_id));

    _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t_clear(unit,
                                            &tpid_class_acceptable_frame_info);

    /* Verify that untagged packets are classified only to tag format 0 */
    if ((tpid_class->tpid1 == BCM_PORT_TPID_CLASS_TPID_INVALID) &&
        (tpid_class->tpid2 == BCM_PORT_TPID_CLASS_TPID_INVALID) &&
        (tpid_class->tag_format_class_id != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Untagged packets can "
                            "be classified only to tag format 0")));
    }

    if ((tpid_class->flags & BCM_PORT_TPID_CLASS_NATIVE_EVE) &&
        (tpid_class->tpid_class_id >= _BCM_PORT_NOF_TPID_PROFILES)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("A valid tpid_class_id is needed!")));
    }

    /* Ingress and Egress only seperated */
    if ((tpid_class->flags & BCM_PORT_TPID_CLASS_INGRESS_ONLY) && (tpid_class->flags & BCM_PORT_TPID_CLASS_EGRESS_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                            (_BSL_BCM_MSG("Given invalid flags, can not be set both Ingress ONLY and egress ONLY")));
    }

    /* map gport to ppd-port */
    rv = _bcm_dpp_gport_to_phy_port(unit, tpid_class->port, 0, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get first port */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get,
                                            (unit, gport_info.local_port,
                                             &soc_ppd_port_i, &core)));

    /* get port info */
    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    /* 1. get port associated TPIDs */
    rv = _bcm_petra_tpid_class_assigned_tpids_get(unit, port_info.tpid_profile,
                                                  port_tpids, &nof_tpids);
    BCMDNX_IF_ERR_EXIT(rv);


    /* 2. calc TPID indexes for given two TPIDs (to be found on packet) */
    rv = _bcm_petra_port_tpid_indexes_get(unit, soc_ppd_port_i,
                            tpid_class->tpid1, port_tpids, nof_tpids,
                            port_tpid_indexes[0], &port_tpid_nof_indexes[0]);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_port_tpid_indexes_get(unit, soc_ppd_port_i,
                            tpid_class->tpid2, port_tpids, nof_tpids,
                            port_tpid_indexes[1], &port_tpid_nof_indexes[1]);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get priority indexes */
    if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_IS_PRIO) {
        priority_types[0] = 1;
        nof_prirority_types = 1;
    } else if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO) {
        priority_types[0] = 0; 
        nof_prirority_types = 1;
    } else {
        priority_types[0] = 0;
        priority_types[1] = 1;
        nof_prirority_types = 2;
    }

    /* 3. calculate new buffer data for this tag class */
    rv = _bcm_petra_port_tpid_class_info_compress_get(unit, tpid_class,
                                                      &tag_buff, &tag_buff_eg);
    BCMDNX_IF_ERR_EXIT(rv);

    is_ingress = (tpid_class->flags & BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = (tpid_class->flags & BCM_PORT_TPID_CLASS_INGRESS_ONLY)? FALSE : TRUE;
    is_egr_native = (tpid_class->flags & BCM_PORT_TPID_CLASS_NATIVE_EVE)? TRUE : FALSE;

    /* Egress and native vlan classification can't be configured at the same call.*/
    if ((tpid_class->flags & BCM_PORT_TPID_CLASS_EGRESS_ONLY) &&
        (tpid_class->flags & BCM_PORT_TPID_CLASS_NATIVE_EVE)) {
        is_ingress = FALSE;
        is_egress = FALSE;
    }


    SOC_PPC_LLP_PARSE_INFO_clear(&prs_nfo);

    if (is_ingress) {
        /* read old buffer this port used */
        rv = _bcm_dpp_am_template_port_tpid_class_data_get(unit,
                                    gport_info.local_port, &tpid_class_buffer);
        BCMDNX_IF_ERR_EXIT(rv);

        /* run over all relevant tag strucutres */
        for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
            for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                    prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                    prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                    prs_nfo.is_outer_prio = priority_types[indx3];
                    /* map parser result to index in buffer */
                    rv = _bcm_petra_port_tpid_tag_structure_to_index(unit,
                                                  &prs_nfo, &tag_stcture_indx);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* update data in buffer */
                    rv = _bcm_petra_port_tpid_class_info_buffer_update(unit,
                                                  tag_stcture_indx, tag_buff,
                                                  &tpid_class_buffer);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

        /* now buffer ready with all information, allocate port profile match this buffer */
        rv = _bcm_dpp_am_template_port_tpid_class_exchange(unit, 0,
                                                  gport_info.local_port,
                                                  &tpid_class_buffer,
                                                  &old_template, &is_last,
                                                  &new_template, &is_first);
        BCMDNX_IF_ERR_EXIT(rv);

        LOG_DEBUG(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit, "new port.llvp-profile is %d\n"),
                  new_template));

        /* if new profile write to hardware */
        if (is_first) {

            /* this is new prt-LLVP profile need to configure it
               1. first part is copy from old profile, then overwrite with new changes
             */
            if (new_template != old_template) {
                for(prs_nfo.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
                    prs_nfo.outer_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
                    ++prs_nfo.outer_tpid) {
                    for(prs_nfo.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
                        prs_nfo.inner_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
                        ++prs_nfo.inner_tpid) {
                        for (prs_nfo.is_outer_prio = FALSE;
                             prs_nfo.is_outer_prio <= TRUE;
                             ++prs_nfo.is_outer_prio) {
                            soc_sand_rv = soc_ppd_llp_parse_packet_format_info_get(unit,
                                                        old_template, &prs_nfo,
                                                        &pkt_frmt_info);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                            /* ppd call*/
                            soc_sand_rv = soc_ppd_llp_parse_packet_format_info_set(unit,
                                                        new_template, &prs_nfo,
                                                        &pkt_frmt_info);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }
                }
            }

            /* map bcm info to PPD LLP_PARSE info */
            rv = _bcm_petra_port_tpid_class_info_to_ppd(unit, tpid_class,
                                                        &pkt_frmt_info
            );
            BCMDNX_IF_ERR_EXIT(rv);

           /* now change port-profile x tag structure in HW */
            /* run over all relevant tag strucutres */
            for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
                for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                    for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                        prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                        prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                        prs_nfo.is_outer_prio = priority_types[indx3];
                        /* ppd call*/
                        soc_sand_rv = soc_ppd_llp_parse_packet_format_info_set(unit,
                                                        new_template, &prs_nfo,
                                                        &pkt_frmt_info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
        }

        /* make port use this profile */
       port_info.port_profile = new_template;
       port_info.flags = SOC_PPC_PORT_IHP_PINFO_LLR_TBL;
       soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* update egress acceptable frame port profile */
    if (is_egress) {    
        /* read old buffer this port used */
        rv = _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_data_get(unit,
                                       gport_info.local_port,
                                       &tpid_class_acceptable_frame_info);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_template_port_tpid_class_eg_data_get(unit,
                                       gport_info.local_port,
                                       &tpid_class_buffer);
        BCMDNX_IF_ERR_EXIT(rv);

        /* run over all relevant tag strucutres */
        for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
            for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                    prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                    prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                    prs_nfo.is_outer_prio = priority_types[indx3];
                    /* map parser result to index in buffer */
                    rv = _bcm_petra_port_tpid_tag_structure_to_index(unit,
                                       &prs_nfo, &tag_stcture_indx);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* update data in buffer */
                    rv = _bcm_petra_port_tpid_class_info_buffer_update(unit,
                                       tag_stcture_indx, tag_buff_eg,
                                       &tpid_class_buffer);
                    BCMDNX_IF_ERR_EXIT(rv);

                    *buffer_lcl = tpid_class_acceptable_frame_info.buffer;
                    if (tpid_class->flags & BCM_PORT_TPID_CLASS_DISCARD) {
                        SHR_BITSET(buffer_lcl, tag_stcture_indx);
                    } else {
                        SHR_BITCLR(buffer_lcl, tag_stcture_indx);
                    }
                    tpid_class_acceptable_frame_info.buffer = *buffer_lcl;
                }
            }
        }

        /* now buffer ready with all information allocate port profile match this buffer */
        rv = _bcm_dpp_am_template_port_tpid_class_eg_exchange(unit,
                                       0,
                                       gport_info.local_port,
                                       &tpid_class_buffer,
                                       &old_tpid_class_tmpl_eg,
                                       &is_last_tpid_eg,
                                       &new_tpid_class_tmpl_eg,
                                       &is_first_tpid_eg);
        BCMDNX_IF_ERR_EXIT(rv);

        /* decide on egress acceptable frame port profile */
        rv = _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_exchange(unit,
                                       0,
                                       gport_info.local_port,
                                       &tpid_class_acceptable_frame_info,
                                       &old_acceptable_tmpl,
                                       &is_last,
                                       &new_acceptable_tmpl,
                                       &is_first_acceptable);
        BCMDNX_IF_ERR_EXIT(rv);
        /* map parser result to index in buffer */
        /* if new profile write to hardware */
        if ((is_first_acceptable)||(is_first_tpid_eg)) {
            /* 1. first part is copy from old profile, then overwrite with new changes */
            /* update for all frame types */
            if ((new_acceptable_tmpl != old_acceptable_tmpl)||
                (new_tpid_class_tmpl_eg != old_tpid_class_tmpl_eg)) {
                for(prs_nfo.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
                    prs_nfo.outer_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
                    ++prs_nfo.outer_tpid) {
                    for(prs_nfo.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
                        prs_nfo.inner_tpid < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
                        ++prs_nfo.inner_tpid) {
                        for (prs_nfo.is_outer_prio = FALSE;
                             prs_nfo.is_outer_prio <= TRUE;
                             ++prs_nfo.is_outer_prio) {
                             if((is_first_tpid_eg)&&
                                (new_tpid_class_tmpl_eg != old_tpid_class_tmpl_eg)){
                                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_llp_parse_packet_format_eg_info_get,
                                    (unit, old_tpid_class_tmpl_eg, &prs_nfo,
                                     &pkt_frmt_info));
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                                /* ppd call*/
                                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_llp_parse_packet_format_eg_info_set,
                                    (unit, new_tpid_class_tmpl_eg, &prs_nfo,
                                    &pkt_frmt_info));
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                             }
                             if((is_first_acceptable)&&
                                (new_acceptable_tmpl != old_acceptable_tmpl)){
                               /* get "egress acceptable frame filter" from old profile */
                               soc_sand_rv = soc_ppd_eg_filter_port_acceptable_frames_get(unit,
                                                      old_acceptable_tmpl,
                                                      &prs_nfo,
                                                      &eg_filter_port_accept);
                               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                               
                               /* update with "egress acceptable frame filter" for new profile */
                               soc_sand_rv =  soc_ppd_eg_filter_port_acceptable_frames_set(unit,
                                                      new_acceptable_tmpl,
                                                      port_info.port_profile_eg,
                                                      &prs_nfo,
                                                      eg_filter_port_accept);
                               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                             }
                        }
                    }
                }
            }

            /* map bcm info to PPD LLP_PARSE info */
            rv = _bcm_petra_port_tpid_class_info_to_ppd(unit,
                                                        tpid_class,
                                                        &pkt_frmt_info);

            BCMDNX_IF_ERR_EXIT(rv);

            /* now change port-profile x tag structure in HW */
            /* run over all relevant tag structures */
            for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
                for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                    for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                        prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                        prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                        prs_nfo.is_outer_prio = priority_types[indx3];
                        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_llp_parse_packet_format_eg_info_set,
                                    (unit, new_tpid_class_tmpl_eg, &prs_nfo,
                                     &pkt_frmt_info));
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        /* get "egress acceptable frame filter" from buffer */
                        rv = _bcm_petra_port_tpid_tag_structure_to_index(unit,
                                   &prs_nfo, &tag_stcture_indx);
                        BCMDNX_IF_ERR_EXIT(rv);
                        /* for the tag structure, if the bit is 1 it means we need to drop.  */
                        *buffer_lcl = tpid_class_acceptable_frame_info.buffer;
                        eg_filter_port_accept = !(SHR_BITGET(buffer_lcl, tag_stcture_indx));

                        /* ppd call*/
                        soc_sand_rv = soc_ppd_eg_filter_port_acceptable_frames_set(unit,
                                                (uint32)new_acceptable_tmpl,
                                                port_info.port_profile,
                                                &prs_nfo,
                                                eg_filter_port_accept);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
        }


        /* make port use this profile */
        port_info.port_profile_eg = new_tpid_class_tmpl_eg;
        port_info.flags = SOC_PPC_PORT_EPNI_PP_PCT_TBL;
        soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* make port use this profile */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
            BCMDNX_IF_ERR_EXIT(
                MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_pp_port_get, (unit,
                                     local_port, &soc_ppd_port_i, &core)));

            soc_sand_rv = soc_ppd_eg_filter_port_info_get(unit, core,
                                     soc_ppd_port_i, &eg_filter_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            eg_filter_port_info.acceptable_frames_profile  = new_acceptable_tmpl;
            soc_sand_rv  = soc_ppd_eg_filter_port_info_set(unit, core,
                                     soc_ppd_port_i, &eg_filter_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /* update egress native vlan classification entries*/
    if (is_egr_native) {
        /* Calculate port_tpid_nof_indexes with TPIDs from tpid_class_id.
         * No Need to update port_tpid_nof_indexes, it is not related to
         * These TPIDs.
         */
        /* 1. get tpid profile associated TPIDs */
        rv = _bcm_petra_tpid_class_assigned_tpids_get(unit,
                                                      tpid_class->tpid_class_id,
                                                      port_tpids, &nof_tpids);
        BCMDNX_IF_ERR_EXIT(rv);


        /* 2. calc TPID indexes for given two TPIDs (to be found on packet) */
        rv = _bcm_petra_port_tpid_indexes_get(unit, soc_ppd_port_i,
                            tpid_class->tpid1, port_tpids, nof_tpids,
                            port_tpid_indexes[0], &port_tpid_nof_indexes[0]);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_petra_port_tpid_indexes_get(unit, soc_ppd_port_i,
                            tpid_class->tpid2, port_tpids, nof_tpids,
                            port_tpid_indexes[1], &port_tpid_nof_indexes[1]);
        BCMDNX_IF_ERR_EXIT(rv);

        /* The dedicated port profile 7 is reserved for native vlan classification.
         * No need the allocation routine.
         */
        new_tpid_class_tmpl_eg = 7;

        /* map bcm info to PPD LLP_PARSE info */
        rv = _bcm_petra_port_tpid_class_info_to_ppd(unit,
                                                    tpid_class,
                                                    &pkt_frmt_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* now change port-profile x tag structure in HW */
        /* run over all relevant tag structures */
        SOC_PPC_LLP_PARSE_INFO_clear(&prs_nfo);
        for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
            for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                    prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                    prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                    prs_nfo.is_outer_prio = priority_types[indx3];
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_llp_parse_packet_format_eg_info_set,
                                    (unit, new_tpid_class_tmpl_eg, &prs_nfo,
                                     &pkt_frmt_info));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_tpid_class_get
 * Purpose:
 *      Get VLAN tag classification from a port
 * Parameters:
 *      unit - Device number
 *      tpid_class - Pointer to a TPID class structure that includes the port
 *          and the retrieved data.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_petra_port_tpid_class_get(int unit, bcm_port_tpid_class_t *tpid_class)
{
    uint16 port_tpids[_BCM_DPP_MAX_NOF_USABLE_TPIDS_PER_PORT]; 
    uint16 port_tpid_indexes[2][_BCM_PETRA_PORT_NOF_TPID_VALS]; /* 0 for TPID1 , 1 for TPID2*/
    uint32 port_tpid_nof_indexes[2];
    uint32 priority_types[2], nof_prirority_types=0;
    uint32 indx1,indx2,indx3;
    SOC_PPC_LLP_PARSE_INFO
        prs_nfo;
    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO
        pkt_frmt_info;
    SOC_PPC_PORT_INFO port_info;
    int local_port;
    SOC_PPC_PORT soc_ppd_port_i;
    int rv = BCM_E_NONE;
    uint32 nof_tpids, tpid_class_id;
    int soc_sand_rv, core;
    uint8 is_ingress, is_egress, is_egr_native;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_TPID_PARSE_ADVANCED_MODE_CHECK(unit);

    /* map gport to ppd-port */
    rv = _bcm_dpp_gport_to_phy_port(unit, tpid_class->port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);


    /* get first port */
    DPP_PBMP_SINGLE_PORT_GET(gport_info.pbmp_local_ports, local_port);
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get,
                              (unit, local_port, &soc_ppd_port_i, &core));
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* 1. get associated TPIDs from port or given tpid class id*/
    if (tpid_class->flags & BCM_PORT_TPID_CLASS_NATIVE_EVE) {
        tpid_class_id = tpid_class->tpid_class_id;
    } else {
        tpid_class_id = port_info.tpid_profile;
    }
    rv = _bcm_petra_tpid_class_assigned_tpids_get(unit, tpid_class_id,
                                                  port_tpids, &nof_tpids);
    BCMDNX_IF_ERR_EXIT(rv);


    /* 2. calc TPID indexes for given two TPIDs (to be found on packet) */
    rv = _bcm_petra_port_tpid_indexes_get(
       unit, soc_ppd_port_i, tpid_class->tpid1, port_tpids, nof_tpids,
       /* out */ port_tpid_indexes[0], &port_tpid_nof_indexes[0]);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_port_tpid_indexes_get(
       unit, soc_ppd_port_i, tpid_class->tpid2, port_tpids, nof_tpids,
       /* out */ port_tpid_indexes[1], &port_tpid_nof_indexes[1]);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get priority indexes */
    if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_IS_PRIO) {
        priority_types[0] = 1;
        nof_prirority_types = 1;
    }
    else if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO) {
        priority_types[0] = 0; 
        nof_prirority_types = 1;
    }
    else {
        priority_types[0] = 0;
        priority_types[1] = 1;
        nof_prirority_types = 2;
    }


    SOC_PPC_LLP_PARSE_INFO_clear(&prs_nfo);
    SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(&pkt_frmt_info);

    is_ingress = (tpid_class->flags & BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = (tpid_class->flags & BCM_PORT_TPID_CLASS_INGRESS_ONLY)? FALSE : TRUE;
    is_egr_native = (tpid_class->flags & BCM_PORT_TPID_CLASS_NATIVE_EVE)? TRUE : FALSE;

    if (is_egr_native) {
        /* run over all relevant tag strucutres */
        for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
            for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                    prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                    prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                    prs_nfo.is_outer_prio = priority_types[indx3];

                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_llp_parse_packet_format_eg_info_get, (unit, port_info.port_profile_eg, &prs_nfo, &pkt_frmt_info));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
    } else if (is_ingress) {
        /* run over all relevant tag strucutres */
        for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
            for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                    prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                    prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                    prs_nfo.is_outer_prio = priority_types[indx3];
                    /* ppd call*/
                    soc_sand_rv = soc_ppd_llp_parse_packet_format_info_get(unit, port_info.port_profile, &prs_nfo,&pkt_frmt_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
    } else if (is_egress) {
        /* run over all relevant tag strucutres */
        for(indx1 = 0; indx1 < port_tpid_nof_indexes[0]; ++indx1) {
            for(indx2 = 0; indx2 < port_tpid_nof_indexes[1]; ++indx2) {
                for(indx3 = 0; indx3 < nof_prirority_types; ++indx3) {
                    prs_nfo.outer_tpid = port_tpid_indexes[0][indx1];
                    prs_nfo.inner_tpid = port_tpid_indexes[1][indx2];
                    prs_nfo.is_outer_prio = priority_types[indx3];

                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_llp_parse_packet_format_eg_info_get, (unit, port_info.port_profile_eg, &prs_nfo,&pkt_frmt_info));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                }
            }
        }
    }

    /* now map PPD to BCM */
    rv = _bcm_petra_port_tpid_class_info_from_ppd(unit,tpid_class,&pkt_frmt_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    /* Not really needed. Just to aviod coverity defect */
    if(unit < BCM_LOCAL_UNITS_MAX) {
    }
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_dtag_mode_set
 * Description:
 *      Set the double-tagging mode of a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - Double-tagging mode, one of:
 *              BCM_PORT_DTAG_MODE_INTERNAL        Service Provider port
 *              BCM_PORT_DTAG_MODE_EXTERNAL        Customer port
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_petra_port_dtag_mode_set(int unit, bcm_port_t port, int mode)
{
    int port_i;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE, core;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Verify mode */
    if (mode != BCM_PORT_DTAG_MODE_INTERNAL &&
        mode != BCM_PORT_DTAG_MODE_EXTERNAL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                    (_BSL_BCM_MSG("Device support customer or service port")));
    }

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(
            MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get,
                                 (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        port_info.port_type = (mode == BCM_PORT_DTAG_MODE_EXTERNAL) ?\
                              SOC_SAND_PP_PORT_L2_TYPE_CEP :\
                              SOC_SAND_PP_PORT_L2_TYPE_VBP;

        /* for CEP port learn only TM destination, without Logical interface */
        port_info.is_learn_ac = (port_info.port_type != SOC_SAND_PP_PORT_L2_TYPE_CEP);
        port_info.flags = (SOC_PPC_PORT_EPNI_PP_PCT_TBL | 
                           SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL);

        soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port_i, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_dtag_mode_get
 * Description:
 *      Return the current double-tagging mode of a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (OUT) Double-tagging mode
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_petra_port_dtag_mode_get(int unit, bcm_port_t port, int *mode)
{
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE, core;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;


    /* Verify mode */
    if (mode == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mode pointer is NULL")));
    }

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port_i, &core)));

    soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *mode = (port_info.port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP) ? BCM_PORT_DTAG_MODE_EXTERNAL: BCM_PORT_DTAG_MODE_INTERNAL;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_vlan_member_get
 * Description:
 *      Return filter mode for a port.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number to operate on
 *      flags - (OUT) Filter mode, one of BCM_PORT_VLAN_MEMBER_xxx. 
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 */
int 
bcm_petra_port_vlan_member_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags)
{
   int port_i, core;
   int soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_EG_FILTER_PORT_INFO
       eg_filter_port_info;
   SOC_PPC_PORT soc_ppd_port_i;
   SOC_PPC_LLP_TRAP_PORT_INFO llp_trap_port_info;
   _bcm_dpp_gport_info_t gport_info;
   BCMDNX_INIT_FUNC_DEFS;

   if (flags == NULL) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Null param: flags")));
   }

   *flags = 0;

   /* ingress filtering always enabled */
   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);


   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

       soc_sand_rv  = soc_ppd_eg_filter_port_info_get(unit, core, soc_ppd_port_i, &eg_filter_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       /* egress filtering is enabled */
       if (eg_filter_port_info.filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP) {
           *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
       }

       /* Ingress membership check is configurable */
       soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, soc_ppd_port_i, &llp_trap_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       if (llp_trap_port_info.trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP){
           *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
       }

       break;
   }

exit:
   BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_vlan_member_set
 * Description:
 *      Set ingress and egress filter mode for a port.
 * Parameters:
 *      unit - unit number 
 *      port - Port number to operate on
 *      flags - BCM_PORT_VLAN_MEMBER_xxx.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 */

int 
bcm_petra_port_vlan_member_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags)
{
   int port_i, core;
   int soc_sand_rv = 0;
   int rv = BCM_E_NONE;
   SOC_PPC_EG_FILTER_PORT_INFO eg_filter_port_info;
   SOC_PPC_PORT soc_ppd_port_i;
   SOC_PPC_LLP_TRAP_PORT_INFO llp_trap_port_info;
   _bcm_dpp_gport_info_t gport_info;
   BCMDNX_INIT_FUNC_DEFS;
   BCM_DPP_UNIT_CHECK(unit);

   /* verify flags */
   if((flags & (~(BCM_PORT_VLAN_MEMBER_INGRESS|BCM_PORT_VLAN_MEMBER_EGRESS))) != 0 ) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%d: unsupported flags %x"), unit, flags));
   }

   /* verify paramters Petra-B does not support vlan member ingress */
   SOC_PPC_EG_FILTER_PORT_INFO_clear(&eg_filter_port_info);
   SOC_PPC_LLP_TRAP_PORT_INFO_clear(&llp_trap_port_info);

   /* Retrive local PP ports */
   rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
   BCMDNX_IF_ERR_EXIT(rv);


   BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

       /* Ingress */
       soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, soc_ppd_port_i, &llp_trap_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
              llp_trap_port_info.trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP;
       } else {
              llp_trap_port_info.trap_enable_mask &= ~SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP;
       }
       soc_sand_rv = soc_ppd_llp_trap_port_info_set(unit, core, soc_ppd_port_i, &llp_trap_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       
       /* Egress */
       soc_sand_rv  = soc_ppd_eg_filter_port_info_get(unit, core, soc_ppd_port_i, &eg_filter_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

       /* egress filtering is enabled */
       if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
           eg_filter_port_info.filter_mask |= SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP;
       }
       else
       {
           eg_filter_port_info.filter_mask &= ~SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP;
       }
       soc_sand_rv  = soc_ppd_eg_filter_port_info_set(unit, core, soc_ppd_port_i, &eg_filter_port_info);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   }

exit:
   BCMDNX_FUNC_RETURN;
}

typedef enum _outer_tpid_type_e {
   _outer_tpid_type_none=0,
   _outer_tpid_type_port_outer,
   _outer_tpid_type_port_inner,
   _outer_tpid_type_ISID,
   _outer_tpid_type_count      
} _outer_tpid_type_t;

/*
 * Function:
 *      bcm_petra_port_vlan_priority_map_set
 * Description:
 *      Define the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_petra_port_vlan_priority_map_set(int unit, 
                     bcm_port_t port, 
                     int pkt_pri,
                     int cfi, 
                     int internal_pri, bcm_color_t color)
{
    int soc_sand_rv = 0, rv = BCM_E_NONE;
    int port_i, dp, core;
    SOC_PPC_LLP_COS_PORT_INFO    cos_port_info;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO new_entry;
    int profile, new_profile;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (port == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("invalid port")));
    }

    if ((cfi != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("CFI must be set to 0. Not used in API")));
    }

    if (color > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
           (_BSL_BCM_MSG("color parameter referes to DEI which is between 0-1. Mapping refers when packet is s-tagged.\n In case user wants to set dp it needs to map also dei to dp by api bcm_petra_port_cfi_color_set")));
    }

    BCM_DPP_VLAN_CHK_PRIO(unit, pkt_pri);
    BCM_DPP_VLAN_CHK_PRIO(unit, internal_pri);

    rv = _bcm_petra_port_color_encode(unit,color,&dp);
    BCMDNX_IF_ERR_EXIT(rv);
    
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&new_entry);
    new_entry.value1 = dp;
    new_entry.value2 = internal_pri;

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_port_map_alloc_profile(unit, 
                       _bcm_dpp_port_map_type_in_up_to_tc_and_de, 
                       gport_info.pbmp_local_ports, 0, pkt_pri, 
                       &new_entry, &profile, &new_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    /* set entry in allocated profile */
    soc_sand_rv = soc_ppd_llp_cos_mapping_table_entry_set(unit,
                          SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC,
                          profile,pkt_pri,&new_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {        
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit,core,soc_ppd_port_i,&cos_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        cos_port_info.l2_info.tbls_select.in_up_to_tc_and_de_index = profile;
        
        soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit,core,soc_ppd_port_i,&cos_port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_vlan_priority_map_get
 * Description:
 *      Get the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (OUT) Internal priority
 *      color        - (OUT) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_petra_port_vlan_priority_map_get(int unit, 
                     bcm_port_t port, 
                     int pkt_pri,
                     int cfi, 
                     int *internal_pri, 
                     bcm_color_t *color)
{
    int port_i, core;
    SOC_PPC_LLP_COS_PORT_INFO    cos_port_info;
    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    map_entry;
    int tbl_indx = 0;
    SOC_PPC_PORT soc_ppd_port_i;
    _bcm_dpp_gport_info_t gport_info;
    uint32 map_entry_found = 0;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(internal_pri);
    BCMDNX_NULL_CHECK(color);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));

    if ((cfi != 0) && (cfi != 1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid cfi %d"), cfi));
    }

    BCM_DPP_VLAN_CHK_PRIO(unit, pkt_pri);
    
    *internal_pri = -1;
    *color = -1;

    SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&map_entry);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_cos_port_info_get(unit,core,soc_ppd_port_i,&cos_port_info));
  
        tbl_indx = cos_port_info.l2_info.tbls_select.in_up_to_tc_and_de_index;

        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_cos_mapping_table_entry_get(unit,
                     SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC,
                     tbl_indx,
                     pkt_pri,
                     &map_entry));
        map_entry_found = 1;
        break;
    }

    if (map_entry_found) {
        *internal_pri = map_entry.value2;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit,map_entry.value1,color));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid port %d"), port));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* VLAN compression will be implemented in the future */
int 
bcm_petra_port_vlan_vector_set(
    int unit, 
    bcm_gport_t port_id, 
    bcm_vlan_vector_t vlan_vec)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_vlan_vector_get(
    int unit, 
    bcm_gport_t port_id, 
    bcm_vlan_vector_t vlan_vec)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavaliable")));

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
int
_bcm_dpp_port_sw_dump(int unit)
{
    bcm_error_t rv;
    uint32 i, j, rc;
    int map_tbl_use;
    int flags;
    uint16 tpid;
    int tpid_count;
    int port_pp_initialized;
    int tpid_profile_count; 

    BCMDNX_INIT_FUNC_DEFS;

    /* Make sure the port module is initialized */
    _DPP_PORT_INIT_CHECK(unit);

    LOG_CLI((BSL_META_U(unit,
                        "\nPORT:")));
    LOG_CLI((BSL_META_U(unit,
                        "\n-----")));

    LOG_CLI((BSL_META_U(unit,
                        "\n\nmap_tbl_in_use:\n")));

    for(i = 0; i < _bcm_dpp_port_map_type_count; i++) {

        for(j = 0; j < _BCM_PORT_NOF_TPID_PROFILES; j++) {
            rv = PORT_ACCESS.dpp_port_config.map_tbl_use.get(unit, i, j, &map_tbl_use);
            BCMDNX_IF_ERR_EXIT(rv);

            if(FALSE != map_tbl_use) {
                LOG_CLI((BSL_META_U(unit,
                                    "  (%d, %d) %s\n"), 
                         i,
                         j,
                         map_tbl_use ? "TRUE" : "FALSE"));
            }
        }
    }


    LOG_CLI((BSL_META_U(unit,
                        "\n\ntrap_to_flag:\n")));

    for(i = 0; i < _BCM_PETRA_PORT_LEARN_NOF_TRAPS; i++) {
        rv = PORT_ACCESS.dpp_port_config.trap_to_flag.get(unit, i, &flags);
        BCMDNX_IF_ERR_EXIT(rv);
        if(BCM_PETRA_PORT_LEARN_INVALID_FLGS != flags) {
            LOG_CLI((BSL_META_U(unit,
                                "  (%d) 0x%02x\n"),
                     i, 
                     flags));
        }
        
    }


    LOG_CLI((BSL_META_U(unit,
                        "\n\ntpid_profile:\n")));

    if (SOC_IS_JERICHO_PLUS(unit)) {
        /* For QAX, there is one additional LLVP profile (Dedicated to egress native ethernet editor). */
        tpid_profile_count = _DPP_AM_TEMPLATE_TPID_PROFILE_COUNT(unit) + 1; 
    } else {
        tpid_profile_count = _DPP_AM_TEMPLATE_TPID_PROFILE_COUNT(unit); 
    }

    for(i = 0; i < tpid_profile_count; i++) {

        uint32 ref_count, data;

        rc = _bcm_dpp_am_template_tpid_profile_ref_get(unit, i, &ref_count);
        if (rc != BCM_E_NONE) {
            ref_count = 0;
            rc = BCM_E_NONE;
        }                                

        if(ref_count != 0) {
            _bcm_petra_tpid_profile_t profile_type;
            _bcm_petra_dtag_mode_t dtag_mode;
            _bcm_petra_ac_key_map_type_t ac_key_map_type;
            int accept_mode;

            rc = _bcm_dpp_am_template_tpid_profile_tdata_get(unit, i, &(data));
            BCMDNX_IF_ERR_EXIT(rc);

            profile_type = (data >> _DPP_AM_TEMPLATE_PORT_TAG_TYPE_SHIFT)  & _DPP_AM_TEMPLATE_PORT_TAG_TYPE_MASK;
            dtag_mode    = (data >> _DPP_AM_TEMPLATE_PORT_DTAG_MODE_SHIFT) & _DPP_AM_TEMPLATE_PORT_DTAG_MODE_MASK;
            accept_mode  = (data >> _DPP_AM_TEMPLATE_PORT_ACC_MODE_SHIFT)  & _DPP_AM_TEMPLATE_PORT_ACC_MODE_MASK;
            ac_key_map_type  = (data >> _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_SHIFT)  & _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_MASK;

            LOG_CLI((BSL_META_U(unit,
                                "  (%d) ref %6d profile_type %1d dtag_mode %1d accept_mode %10d ac_key_map_type %10d\n"),
                     i,
                     ref_count,
                     profile_type,
                     dtag_mode,
                     accept_mode,
                     ac_key_map_type));
        }        
    }

    if (SOC_DPP_PP_ENABLE(unit))
    {
        rv = PORT_ACCESS.dpp_port_config.port_pp_initialized.get(unit, &port_pp_initialized);
        BCMDNX_IF_ERR_EXIT(rv);
        if(port_pp_initialized) {
            LOG_CLI((BSL_META_U(unit,
                                "\n\ntpids:\n")));

            for(i = 0; i < _BCM_PETRA_NOF_TPIDS_PER_PORT; i++) {
        
                for(j = 0; j < _BCM_PORT_NOF_TPID_PROFILES; j++) {
                    rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid.get(unit, i, j, &tpid);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = PORT_ACCESS.dpp_port_config.bcm_port_tpid_info.tpid_count.get(unit, i, j, &tpid_count);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if(0 != tpid_count ||
                       0 != tpid) {
                        LOG_CLI((BSL_META_U(unit,
                                            "  (%d, %d) id %6d cnt %13d\n"), 
                                 i,
                                 j,
                                 tpid,
                                 tpid_count));
                    }
                }
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


/* Add service classifications(tunnel-port-ids) per pon port. */
int bcm_petra_port_pon_tunnel_add(
    int unit, 
    bcm_gport_t pon_port, 
    uint32 flags, 
    bcm_gport_t *tunnel_port_id)
{
    int rv = BCM_E_NONE;    
    uint32 soc_sand_rv;
    uint32 soc_ppd_port;
    int core;
    uint32 profile_alloc_flag=0;
    uint16 pon_channel_profile, offset;
    bcm_gport_t pon_pp_port;
    SOC_PPC_PORT_INFO port_info;
    SOC_TMC_PORT_PP_PORT_INFO pp_port_info;
    SOC_SAND_SUCCESS_FAILURE failure_indication = SOC_SAND_SUCCESS;
    _bcm_dpp_gport_info_t gport_info;
    uint32 nof_channels = 1;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel_port_id);

    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_pon_tunnel_add is not supported in bypass mode")));
    }

    pon_channel_profile = *tunnel_port_id;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, pon_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /* It's not necessary to add default profile. */
    if (pon_channel_profile == _BCM_PPD_PON_CHANNEL_DEFAULT_PROFILE) {
        *tunnel_port_id = gport_info.local_port;
        BCM_EXIT;
    }

    if (!_BCM_PPD_IS_PON_PP_PORT(gport_info.local_port, unit) || !IS_PON_PORT(unit,_BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Can not add tunnel to a non PON port")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable) {
        nof_channels = SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num;
        offset = _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_OFFSET(unit, nof_channels);
    } else {
        offset = _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET(unit);
    }

    /* if local_port is not a channelized port, alloc PON channel(LLID) profile */
    if (gport_info.local_port == _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port)) {
        if (flags & BCM_PORT_PON_TUNNEL_WITH_ID)
        {
            if (pon_channel_profile >= _BCM_PPD_NOF_PON_CHANNEL_PROFILE(unit, nof_channels)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PON channel profile range is 0~%d"), _BCM_PPD_NOF_PON_CHANNEL_PROFILE(unit, nof_channels)));
            }
            
            rv = bcm_dpp_am_pon_channel_profile_is_alloced(unit, pon_channel_profile);
            if (rv == BCM_E_NOT_FOUND) {
                profile_alloc_flag |=  BCM_DPP_AM_FLAG_ALLOC_WITH_ID;            
                BCMDNX_IF_ERR_EXIT(bcm_dpp_am_pon_channel_profile_alloc(unit, profile_alloc_flag, &pon_channel_profile));
            }
        }
        else
        {
            BCMDNX_IF_ERR_EXIT(bcm_dpp_am_pon_channel_profile_alloc(unit, profile_alloc_flag, &pon_channel_profile));
        }
    }
    /* Get PON PP port from PON physical port and PON channel profile */
    pon_pp_port = _BCM_PPD_GPORT_PON_CHANNEL_PROFILE_TO_PON_PP_PORT(gport_info.local_port, pon_channel_profile, offset);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_find_free_port_and_allocate(unit, core, SOC_PORT_IF_NOCXN, 1, &pon_pp_port));

    /* copy port configuration from local port to PON PP port */
    SOC_PPC_PORT_INFO_clear(&port_info);
    soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  
    port_info.flags = SOC_PPC_PORT_ALL_TBL;
    soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    /* copy pp port configuration from local port to PON pp port */
    SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_get,(unit, core, soc_ppd_port, &pp_port_info)));

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_set,(unit, core, pon_pp_port, &pp_port_info, &failure_indication)));
    SOC_SAND_IF_FAIL_RETURN(failure_indication);

    *tunnel_port_id = pon_pp_port;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Remove service classifications(tunnel-port-ids) per pon port. */
int bcm_petra_port_pon_tunnel_remove(
    int unit, 
    bcm_gport_t tunnel_port_id)
{
    int rv = BCM_E_NONE;
    uint16 pon_channel_profile, offset;
    uint32 nof_channels = 0;
    uint32 local_port;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_pon_tunnel_remove is not supported in bypass mode")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable) {
        nof_channels = SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num;
        offset = _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_OFFSET(unit, nof_channels);
    } else {
        offset = _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET(unit);
    }

    pon_channel_profile = _BCM_PPD_GPORT_PON_PP_PORT_TO_CHANNEL_PROFILE(unit, tunnel_port_id, offset);
    if (pon_channel_profile == _BCM_PPD_PON_CHANNEL_DEFAULT_PROFILE) {
        BCM_EXIT; /* It's not necessary to remove default profile. */
    }

    local_port = _BCM_PPD_GPORT_PON_TO_LOCAL_PORT(tunnel_port_id, offset);

    /* if local_port is not a channelized port, dealloc PON channel(LLID) profile */
    if (local_port == _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, tunnel_port_id)) {
        rv = bcm_dpp_am_pon_channel_profile_is_alloced(unit, pon_channel_profile);
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PON channel profile is not allocated")));
        }
    }
    BCMDNX_IF_ERR_EXIT(bcm_dpp_am_pon_channel_profile_dealloc(unit, pon_channel_profile));

    SOC_IF_ERROR_RETURN(soc_port_sw_db_port_remove(unit, tunnel_port_id));

exit:
    BCMDNX_FUNC_RETURN;
}

/* Set the tunnel id to the service class mapping. */
int bcm_petra_port_pon_tunnel_map_set(
    int unit, 
    bcm_gport_t pon_port, 
    bcm_tunnel_id_t tunnel_id, 
    bcm_gport_t tunnel_port_id)
{
    int core;
    uint32 pon_tunnel_id_alloc_mode;
    SOC_PPC_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    SOC_TMC_PORTS_PON_TUNNEL_INFO info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_pon_tunnel_map_set is not supported in bypass mode")));
    }
   
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, pon_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    if (!_BCM_PPD_IS_PON_PP_PORT(gport_info.local_port, unit) || !IS_PON_PORT(unit,_BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Can not add tunnel to a non PON port")));
    }

    pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 0);
    if (pon_tunnel_id_alloc_mode == 0) {
        if (((_BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port)) == ARAD_PORT_MAX_PON_PORT(unit)) && ((tunnel_id >= (ARAD_PORT_MAX_TUNNEL_ID_IN_MAX_PON_PORT + 1)) && (tunnel_id <= 2047))) { 
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PON port %d Tunnel-IDs %d-2047 can not use"), _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port), (ARAD_PORT_MAX_TUNNEL_ID_IN_MAX_PON_PORT + 1))); 
        }
    } else if (pon_tunnel_id_alloc_mode == 1) {
        if (tunnel_id >= ARAD_PORT_MAX_TUNNEL_ID_IN_AVERAGE_MODE(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel-IDs %d-2047 can not use"), ARAD_PORT_MAX_TUNNEL_ID_IN_AVERAGE_MODE(unit)));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel-IDs alloc mode doesn't support")));
    }

    SOC_TMC_PORTS_PON_TUNNEL_INFO_clear(&info);
    info.pp_port = tunnel_port_id;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port), &soc_ppd_port, &core)));

    /* Set the Mapping between tunnel_ID and PON PP port  */
    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_pon_tunnel_info_set,(unit, soc_ppd_port, tunnel_id, &info)));

exit:
    BCMDNX_FUNC_RETURN;
}


/* Get the tunnel id to the service class mapping. */
int bcm_petra_port_pon_tunnel_map_get(
    int unit, 
    bcm_gport_t pon_port, 
    bcm_tunnel_id_t tunnel_id, 
    bcm_gport_t *tunnel_port_id)
{
    int core;
    uint32 pon_tunnel_id_alloc_mode;
    SOC_PPC_PORT soc_ppd_port;
    SOC_TMC_PORTS_PON_TUNNEL_INFO info;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;    

    BCMDNX_NULL_CHECK(tunnel_port_id);

    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_pon_tunnel_map_get is not supported in bypass mode")));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, pon_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

    if (!_BCM_PPD_IS_PON_PP_PORT(gport_info.local_port, unit) || !IS_PON_PORT(unit,_BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Can not add tunnel to a non PON port")));
    }

    pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 0);
    if (pon_tunnel_id_alloc_mode == 0) {
        if (((_BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port)) == ARAD_PORT_MAX_PON_PORT(unit)) && ((tunnel_id >= (ARAD_PORT_MAX_TUNNEL_ID_IN_MAX_PON_PORT + 1)) && (tunnel_id <= 2047))) { 
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PON port %d Tunnel-IDs %d-2047 can not use"), ARAD_PORT_MAX_PON_PORT(unit), (ARAD_PORT_MAX_TUNNEL_ID_IN_MAX_PON_PORT + 1))); 
        }
    } else if (pon_tunnel_id_alloc_mode == 1) {
        if (tunnel_id >= ARAD_PORT_MAX_TUNNEL_ID_IN_AVERAGE_MODE(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel-IDs %d-2047 can not use"), ARAD_PORT_MAX_TUNNEL_ID_IN_AVERAGE_MODE(unit)));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel-IDs alloc mode doesn't support")));
    }


    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, gport_info.local_port), &soc_ppd_port, &core)));

    SOC_TMC_PORTS_PON_TUNNEL_INFO_clear(&info);
    /* Get PON PP port from PON tunnel_ID */
    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_pon_tunnel_info_get,(unit, soc_ppd_port, tunnel_id, &info)));

    *tunnel_port_id = info.pp_port;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_encap_config_set
 * Purpose:
 *      Configre 88650l2C3 feature
 * Parameters:
 *      unit - unit number.
 *      port - port number
 *      encap_config - encapsulation information
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write internal tables entries
 *      BCM_E_UNAVAIL - feature unavaliable
 * Notes:
 */ 
int
bcm_petra_port_encap_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_encap_config_t *encap_config)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);    
    BCMDNX_NULL_CHECK(encap_config);  
             
    switch (encap_config->encap)
    {
    case BCM_PORT_ENCAP_IEEE:
        /*L2 encapsulation*/
        if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "88650L23C", 0) && !BCM_GPORT_IS_TUNNEL(gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_encap_config_set is not enabled")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_encap_config_set unsupported encapsuation type")));
    }

    {
        SOC_TMC_FAP_PORT_ID         port_ndx;
        SOC_TMC_L2_ENCAP_INFO       info;
        SOC_PPC_EG_ENCAP_DATA_INFO data_info;
        int rv = BCM_E_NONE;
        int local_port;
        int i, core;  
        uint16 vlan_pcp_dei; 
        uint32 pp_port_var, mac_lsb,mac_msb;
        uint64 prge_var;
        _bcm_dpp_gport_hw_resources gport_hw_resources;

        if(BCM_GPORT_IS_TUNNEL(gport)){

            SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);    
         
            rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                    &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);

            vlan_pcp_dei=encap_config->vlan;

            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l2_encap_pcp_dei_config_enable", 0) == 1)
            {
                vlan_pcp_dei|=(encap_config->tos & 0xF)<<12;
            }

            SOC_PPD_EG_ENCAP_DATA_L2_ENCAP_EXTERNAL_CPU_FORMAT_SET(unit, vlan_pcp_dei, encap_config->oui_ethertype, encap_config->dst_mac, &data_info);

            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                /* check if this tunnel is allocated */
                rv = bcm_dpp_am_l2_encap_eep_is_alloced(unit, gport_hw_resources.local_out_lif);
                if (rv == BCM_E_NOT_FOUND) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found")));
                }
            }
            rv = soc_ppd_eg_encap_data_lif_entry_add(unit, gport_hw_resources.local_out_lif, &data_info, FALSE, 0);
            BCM_SAND_IF_ERR_EXIT(rv);

           /*SA MAC # TPID*/
            mac_msb = ((uint32)(encap_config->src_mac[0])<<24)|
                      ((uint32)(encap_config->src_mac[1])<<16)|
                      ((uint32)(encap_config->src_mac[2])<<8)|
                      ((uint32)(encap_config->src_mac[3]));
            mac_lsb = ((uint32)(encap_config->src_mac[4])<<24)|((uint32)(encap_config->src_mac[5])<<16);

            COMPILER_64_SET(prge_var, mac_msb,mac_lsb | (encap_config->tpid & 0xFFFF) );

            pp_port_var = (uint32)encap_config->dst_mac[0]<<16 | (uint32)encap_config->dst_mac[1]<<8 |(uint32)encap_config->dst_mac[2];

            rv = arad_egr_prog_l2_encap_external_cpu_program_info_set(unit,pp_port_var,prge_var);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    else{

            SOC_TMC_L2_ENCAP_INFO_clear(&info);
            info.eth_type=encap_config->oui_ethertype;
            info.tpid = encap_config->tpid;
            info.vlan = encap_config->vlan;
            for (i=0;i<6;i++)
            {
                info.da[i]=encap_config->dst_mac[i];
                info.sa[i]=encap_config->src_mac[i];
            }
            rv = bcm_petra_port_local_get(unit, gport, &local_port);
            BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_encap_config_set unable to extract local port infomraiton")));
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, local_port, &port_ndx, &core));
            rv = arad_port_encap_config_set(unit,core,port_ndx,&info);
            if (rv != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_petra_port_encap_config_set Internal error")));
            }

        }

    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

 /*
  * Function:
  *      bcm_petra_port_timesync_timesync_tx_info_get
  * Purpose:
  *    Retrives two-step PTP timestamp and seqid from MAC FIFO after packet TX
  * Parameters:
  *      unit - (IN) Unit number.
  *      port - (IN) Port
  *      conf - (IN) Configuration
  * Returns:
  *      BCM_E_xxx
  * Notes:
  */

int
bcm_petra_port_timesync_tx_info_get(int unit, bcm_port_t port,
                                  bcm_port_timesync_tx_info_t *tx_info)
{
    int soc_sand_rv;
    int port_i;
    SOC_PPC_PTP_PORT_INFO profile_data;
    SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx;
    int rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_timesync_support)) {

        SOC_PPC_PTP_PORT_INFO_clear(&profile_data);

        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

        /* get profile index from HW */
        soc_sand_rv = soc_ppd_ptp_port_get(unit, port_i, &profile_ndx);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* and profile data from SW state */
        rv = _bcm_dpp_am_template_ptp_port_profile_tdata_get(unit, profile_ndx, &profile_data);
        BCMDNX_IF_ERR_EXIT(rv);

        if(0 == profile_data.ptp_enabled) {
            return BCM_E_PARAM;
        } else {
#ifdef PORTMOD_SUPPORT
       portmod_fifo_status_t info;
       _rv = portmod_port_timesync_tx_info_get(unit, port, &info);
       COMPILER_64_SET(tx_info->timestamp, info.timestamps_in_fifo_hi, info.timestamps_in_fifo);
       tx_info->sequence_id = info.sequence_id;
#endif

        }
    }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("1588 is not supported for this device")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_petra_port_timestamp_adjust_set
 * Purpose:
 *     set OSTS,TSTS timestamp adjust based on speed
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port
 *      ts_adjust - (IN) configuration for tsts,osts timestamp adjustment.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_port_timestamp_adjust_set(int unit,
          bcm_port_t port,
          bcm_port_timestamp_adjust_t *ts_adjust)
{
    soc_port_t port_i;
    _bcm_dpp_gport_info_t   gport_info;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(ts_adjust);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_timestamp_adjust_set,(unit, port_i, ts_adjust)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_petra_port_timestamp_adjust_get
 * Purpose:
 *     get OSTS,TSTS timestamp adjust on current speed
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port
 *      ts_adjust - (OUT) configuration for tsts,osts timestamp adjust with current speed.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
bcm_petra_port_timestamp_adjust_get(int unit,
          bcm_port_t port,
          bcm_port_timestamp_adjust_t *ts_adjust)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(ts_adjust);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    rv = (MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_timestamp_adjust_get,(unit, gport_info.local_port, ts_adjust)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_encap_config_get
 * Purpose:
 *      Configre 88650l2C3 feature
 * Parameters:
 *      unit - unit number.
 *      port - port number
 *      encap_config - encapsulation information
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write internal tables entries
 *      BCM_E_UNAVAIL - feature unavaliable
 * Notes:
 */ 
int
bcm_petra_port_encap_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_encap_config_t *encap_config)
{

   SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;

   BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);    
   BCMDNX_NULL_CHECK(encap_config);  
  
   switch (encap_config->encap)
   {
   case BCM_PORT_ENCAP_IEEE:
       /*L2 encapsulation*/
       if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "88650L23C", 0) && !BCM_GPORT_IS_TUNNEL(gport)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_encap_config_get is not enabled")));
       }
       break;
   default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_encap_config_get unsupported encapsuation type")));
   }

   {
       uint32                      port_ndx;
       SOC_TMC_L2_ENCAP_INFO       info;
       int rv;
       int local_port;
       int i, core;
       SOC_PPC_EG_ENCAP_DATA_INFO *data_info;
       uint32
        pp_port_var,    
        next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
        nof_entries;
       uint64 prge_var;
       uint16 vlan_pcp_dei;
       _bcm_dpp_gport_hw_resources gport_hw_resources;

       if(BCM_GPORT_IS_TUNNEL(gport)){

           rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK|_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                               &gport_hw_resources);           
           BCMDNX_IF_ERR_EXIT(rv);

            BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
                "bcm_petra_port_encap_config_get.encap_entry_info");
            if(encap_entry_info == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
            }

           rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, gport_hw_resources.local_out_lif, 1, encap_entry_info, next_eep, &nof_entries);
           SOC_SAND_IF_ERR_EXIT(rv);

           data_info=&(encap_entry_info[0].entry_val.data_info);

           rv = arad_egr_prog_l2_encap_external_cpu_program_info_get(unit,&pp_port_var,&prge_var);
           BCMDNX_IF_ERR_EXIT(rv);

           encap_config->tpid = (COMPILER_64_LO(prge_var)) & 0xFFFF;
           encap_config->dst_mac[0]=(pp_port_var>>16) &0xFF;
           encap_config->dst_mac[1]=(pp_port_var>>8) &0xFF;
           encap_config->dst_mac[2]=(pp_port_var) &0xFF;

           encap_config->src_mac[0]=(COMPILER_64_HI(prge_var)>>24) & 0xFF;
           encap_config->src_mac[1]=(COMPILER_64_HI(prge_var)>>16) & 0xFF;
           encap_config->src_mac[2]=(COMPILER_64_HI(prge_var)>>8) & 0xFF;
           encap_config->src_mac[3]=(COMPILER_64_HI(prge_var)) & 0xFF;
           encap_config->src_mac[4]=(COMPILER_64_LO(prge_var)>>24) & 0xFF;
           encap_config->src_mac[5]=(COMPILER_64_LO(prge_var)>>16) & 0xFF;

           SOC_PPD_EG_ENCAP_DATA_L2_ENCAP_EXTERNAL_CPU_FORMAT_GET(unit, vlan_pcp_dei, encap_config->oui_ethertype, encap_config->dst_mac, data_info);

           if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l2_encap_pcp_dei_config_enable", 0) == 1)
           {
               encap_config->tos=(vlan_pcp_dei>>12)&0xF;
           }
           encap_config->vlan=vlan_pcp_dei&0xFFF;

       }else
       {
       
           rv = bcm_petra_port_local_get(unit, gport, &local_port);
           BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_port_encap_config_get unable to extract local port infomraiton")));
           BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, local_port, &port_ndx, &core));

           SOC_TMC_L2_ENCAP_INFO_clear(&info);
           rv = arad_port_encap_config_get(unit,core,port_ndx,&info);
           if (rv != 0)
           {
               BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_petra_port_encap_config_get Internal error")));
           }
           encap_config->oui_ethertype = info.eth_type;
           encap_config->tpid = info.tpid;
           encap_config->vlan = info.vlan;
           for (i=0;i<6;i++)
           {
               encap_config->dst_mac[i] = info.da[i];
               encap_config->src_mac[i] = info.sa[i];
           }

       } 

   }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_timesync_config_set
 * Purpose:
 *      set 1588 protocol configuaion for port 
 * Parameters:
 *      unit - (IN) unit number.
 *      port - (IN) port number
 *      config_count - (IN) config_array size 
 *      config_array - (IN) 1588 port configuration
 * Returns:
 *      BCM_E_NONE - success 
 *      BCM_E_PARAM - parameter not supported
 *      BCM_E_UNAVAIL - feature unavaliable
 * Notes:
 *
 * Disable port 1588 configuration  - config_count = 0
 * Enable  port 1588 configuration  - config_count = 1 
 *                       config_array->flags =
 *                       BCM_PORT_TIMESYNC_DEFAULT            mast be on 
 *                       BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP when one step Tranparent clock (TC) is enabled system updates the 
 *                                                            correction field in 1588 Event 1588 messages.    
 *                       BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP when step step Tranparent clock (TC) is enabled system records the 
 *                                                            1588 Event 1588 messages TX time.    
 *                         1588 event messages:
 *                           1. SYNC
 *                           2. DELAY_REQ
 *                           3. PDELAY_REQ
 *                           4. PDELAY_RESP
 *                       config_array->pkt_drop, config_array->pkt_tocpu - bit masks indicating wehter to forward (drop-0,tocpu-0), 
 *                       drop(drop-1) or trap(tocpu-1) the packet
 */ 
int
bcm_petra_port_timesync_config_set(int unit, bcm_port_t port, int config_count,
                                  bcm_port_timesync_config_t *config_array)
{  
    int soc_sand_rv;
    int port_i;
    int rv; 
    SOC_PPC_PTP_PORT_INFO new_profile_data;
    SOC_PPC_PTP_IN_PP_PORT_PROFILE new_profile_ndx;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    

    if (soc_feature(unit, soc_feature_timesync_support)) {

        SOC_PPC_PTP_PORT_INFO_clear(&new_profile_data);

        /* Sanity */
        if(0 != config_count && 1 != config_count) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config_count != 0 && config_count != 1")));
        }
        else if(1 == config_count && NULL == config_array) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config_array == NULL")));
            
        }
        
        if(config_array) {      
            /* Verify valid values */
            if (!(config_array->flags & BCM_PORT_TIMESYNC_DEFAULT)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("parameters not supported, BCM_PORT_TIMESYNC_DEFAULT must be on")));
            }

            if (config_array->flags & BCM_PORT_TIMESYNC_MPLS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_PORT_TIMESYNC_MPLS flag is not supported")));
            }

            if (config_array->pkt_drop & config_array->pkt_tocpu) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("same bit should not turned on in pkt_drop & pkt_tocpu")));
            }

            if (config_array->flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP &&
                config_array->flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one step & two step flags should not be enabled simultaniesly")));
            }

            if (1 == config_count) {
                new_profile_data.ptp_enabled = 1; 
            }else {
                new_profile_data.ptp_enabled = 0; 
            }

            if (config_array->flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP) {
                new_profile_data.flags |= SOC_PPC_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
            }
            if (config_array->flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP) {
                new_profile_data.flags |= SOC_PPC_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP;
            }
            new_profile_data.pkt_drop    = config_array->pkt_drop;
            new_profile_data.pkt_tocpu   = config_array->pkt_tocpu;
            new_profile_data.user_trap_id = config_array->user_trap_id;
        }else {
            new_profile_data.ptp_enabled = 0;
        }

        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

            /* set SW state - allocation manager */
            rv = _bcm_dpp_am_template_ptp_port_profile_exchange(unit, port_i, &new_profile_data, &new_profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);

            /* and update HW */
            soc_sand_rv = soc_ppd_ptp_port_set(unit, port_i, &new_profile_data, new_profile_ndx);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("1588 is not supported for this device")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_port_timesync_config_get
 * Purpose:
 *      get 1588 protocol port configuaion
 * Parameters:
 *      unit - (IN) unit number.
 *      port - (IN) port number
 *      array_size   - (IN) config_array size 
 *      config_array - (OUT) 1588 port configuration
 *      config_count - (OUT) config_array size 
 * Returns:
 *      BCM_E_NONE - success 
 *      BCM_E_PARAM - parameter not supported
 *      BCM_E_UNAVAIL - feature unavaliable
 * Notes:
 *
 * output will be the follwing:
 * when port 1588 configuration Disabled - *array_count = 0
 * when port 1588 configuration Enabled  - *array_count = 1, config_array is set with the port configuration
 */ 
int
bcm_petra_port_timesync_config_get(int unit, bcm_port_t port, int array_size,
                                  bcm_port_timesync_config_t *config_array, 
                                  int *array_count)
{                                                                                
    int soc_sand_rv;
    int port_i;
    SOC_PPC_PTP_PORT_INFO profile_data;
    SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx;
    int rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    
    /* Check for array_count */
    if ((NULL == array_count) ||
        (array_size > 0 && NULL == config_array)) {
        return BCM_E_PARAM;
    }


    if (soc_feature(unit, soc_feature_timesync_support)) {

        SOC_PPC_PTP_PORT_INFO_clear(&profile_data);

        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

            /* get profile index from HW */
            soc_sand_rv = soc_ppd_ptp_port_get(unit, port_i, &profile_ndx);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


            /* and profile data from SW state */
            rv = _bcm_dpp_am_template_ptp_port_profile_tdata_get(unit, profile_ndx, &profile_data);
            BCMDNX_IF_ERR_EXIT(rv);


            if(0 == profile_data.ptp_enabled) {

                *array_count = 0;

            } else {

                if (array_size < 1) {
                    return BCM_E_PARAM;
                }

                bcm_port_timesync_config_t_init(config_array);

                *array_count = 1;

                config_array->flags |= BCM_PORT_TIMESYNC_DEFAULT;

                /* values mapping: SOC -> BCM */
                if (profile_data.flags & SOC_PPC_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP) {
                    config_array->flags |= BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
                }
                if (profile_data.flags & SOC_PPC_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP) {
                    config_array->flags |= BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP;
                }

                config_array->pkt_drop  = profile_data.pkt_drop;
                config_array->pkt_tocpu = profile_data.pkt_tocpu;
                config_array->user_trap_id = profile_data.user_trap_id;
            
            }

            break;

        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("1588 is not supported for this device")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_convert_match_criteria_to_vlan_port_match_criteria
 * Purpose:
 *      Convert match criteria to VLAN port match criteria
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Match information
 *      vlan_port - (OUT) VLAN port structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int _bcm_petra_port_convert_match_criteria_to_vlan_port_match_criteria(
    int unit,
    bcm_port_match_t match,
    bcm_vlan_port_match_t *vlan_port_match)
{
    BCMDNX_INIT_FUNC_DEFS;

    switch (match) {
    case BCM_PORT_MATCH_PORT:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT;
        break;
    case BCM_PORT_MATCH_PORT_INITIAL_VLAN:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
        break;
    case BCM_PORT_MATCH_PORT_PCP_VLAN:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
        break;
    case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN_ETHERTYPE:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN_STACKED:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_ETHERTYPE:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
        break;
    case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED_ETHERTYPE:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED_ETHERTYPE;
        break;
    case BCM_PORT_MATCH_PORT_VLAN:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        break;
    case BCM_PORT_MATCH_PORT_VLAN_STACKED:    
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        break;
    case BCM_PORT_MATCH_PORT_UNTAGGED:
        *vlan_port_match = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
        break;
    case BCM_PORT_MATCH_COUNT:
    case BCM_PORT_MATCH_INVALID:
    case BCM_PORT_MATCH_LABEL:
    case BCM_PORT_MATCH_LABEL_PORT:
    case BCM_PORT_MATCH_LABEL_VLAN:
    case BCM_PORT_MATCH_PORT_VLAN_RANGE:
    case BCM_PORT_MATCH_TUNNEL_VLAN_SRCMAC:
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid match parameter 0x%x"),match));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_convert_match_to_vlan_port
 * Purpose:
 *      Convert match to VLAN port structure
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) vlan_port_id
 *      match - (IN) Match information
 *      vlan_port - (OUT) VLAN port structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int _bcm_petra_port_convert_match_to_vlan_port(
    int unit,
    bcm_gport_t vlan_port_id,
    bcm_port_match_info_t *match,
    bcm_vlan_port_t *vlan_port)
{
    bcm_error_t rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    vlan_port->vlan_port_id = vlan_port_id;
    vlan_port->port = match->port;
    vlan_port->match_vlan = (match->flags & BCM_PORT_MATCH_EGRESS_ONLY) ? (match->match_inner_vlan):(match->match_vlan);
    vlan_port->match_inner_vlan = match->match_inner_vlan;
    vlan_port->match_ethertype = match->match_ethertype;
    vlan_port->match_pcp = match->match_pcp;
    vlan_port->match_tunnel_value = match->match_pon_tunnel;    
    vlan_port->vsi = match->match_vlan;
    vlan_port->flags = 0;

    rv = _bcm_petra_port_convert_match_criteria_to_vlan_port_match_criteria(unit, match->match, &vlan_port->criteria);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_extender_dbal_qual_vals_get
 * Purpose:
 *      Get dbal qualification value (key) for extender match criteria
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      match       - (IN)  Match criterias
 *      qual_vals   - (OUT) dbal qualification key
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Removal of the original match (original i.e. from bcm_petra_extender_port_add)
 *      is not currently supported.
 */
int
_bcm_extender_port_dbal_qual_vals_get(
    int unit,
    bcm_port_match_info_t *match,
    SOC_PPC_FP_QUAL_VAL *qual_vals)
{
    uint32 pp_port;
    int core;
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPC_PORT_INFO port_info;

    SOCDNX_INIT_FUNC_DEFS;

    /* Get the Port information in order to retrieve the VLAN-Domain */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, match->port, &pp_port, &core)));
    rv = soc_ppd_port_info_get(unit, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(rv);

    /* Format the Extender qualifiers */
    DBAL_QUAL_VALS_CLEAR(qual_vals);

    qual_vals[0].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    qual_vals[0].val.arr[0] = port_info.vlan_domain;

    qual_vals[1].type = SOC_PPC_FP_QUAL_PORT_EXTENDER_ECID;
    qual_vals[1].val.arr[0] = match->extended_port_vid;

    if (match->match == BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_INITIAL_VLAN) {
        qual_vals[2].type = SOC_PPC_FP_QUAL_INITIAL_VID;
        qual_vals[3].type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_vals[3].val.arr[0] = 0;

    } else {
        qual_vals[2].type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_vals[3].type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
        qual_vals[3].val.arr[0] = 1;
    }

    qual_vals[2].val.arr[0] = match->match_vlan;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_vlan_port_match_ingress_add
 * Purpose:
 *      Add an Ingress match to an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      lif - (IN) LIF ID 
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_vlan_port_match_ingress_add(
    int unit, 
    bcm_gport_t port,
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_PPC_L2_LIF_AC_KEY  in_ac_key;
    SOC_PPC_L2_LIF_AC_INFO ac_info;
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = NULL;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ID lif_id_tmp;
    uint32 soc_sand_rv;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    uint8 found;
    bcm_error_t rv;
    bcm_vlan_port_t vlan_port;
    int is_local;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* There's no point in adding an ingress match to a remote port. */
    rv = _bcm_dpp_gport_is_local(unit, match->port, &is_local);       
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_local) {
        BCM_EXIT;
    }

    rv = _bcm_petra_port_convert_match_to_vlan_port(unit, port, match, &vlan_port);
    BCMDNX_IF_ERR_EXIT(rv);


    rv = _bcm_dpp_gport_fill_ac_key(unit, &vlan_port, BCM_GPORT_VLAN_PORT, &in_ac_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* look up existing entry. No needs to lookup for port match scenario. */
    if (match->match != BCM_PORT_MATCH_PORT) {
        soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &in_ac_key,
                                            &lif_id_tmp, &ac_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        found = TRUE;
    }

    /* In case of port match an entry is always found as it's a static table */
    if (found && (match->match != BCM_PORT_MATCH_PORT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("match already exist")));
    }    

    BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_port_match_ingress_add.lif_entry_info");
    if (lif_entry_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    /* Add this match entry */
    /* Retreive LIF information */
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, lif_id, lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#if BCM_DPP_PORT_MATCH_LIF_LEARNT /* match add should not be called in case LIF is learnt */
    if (lif_entry_info->value.ac.learn_record.learn_type != SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT &&
        lif_entry_info->value.ac.learn_record.learn_type != SOC_PPC_L2_LIF_AC_LEARN_DISABLE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("match failed since LIF should not be enabled learn")));
    }
#endif
    
    /* Add match */
    soc_sand_rv = soc_ppd_l2_lif_ac_add(soc_sand_dev_id, &in_ac_key, lif_id, &(lif_entry_info->value.ac) ,&failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_EXIT(failure_indication);

exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}


/* Internal Function.
   ESEM lookup configuration: 
   Convert bcm_port_match_info_t to arad_pp_esem_key according to type of ESEM lookup
*/
int 
_bcm_vlan_port_port_match_info_to_esem_key(
   int unit, 
   bcm_port_match_info_t *match, 
     ARAD_PP_ESEM_KEY *esem_key
   ) {
    BCMDNX_INIT_FUNC_DEFS;

    /* For new SEM lookup, like oulif + VSI -> outAC */
    if (match->match == BCM_PORT_MATCH_PORT_VPN) {
        _bcm_dpp_gport_hw_resources gport_hw_resources;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, match->port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS
                                                          | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                     &gport_hw_resources));

        /* Convert bcm_port_match_info_t to arad_pp_esem_key */
        esem_key->key_type = ARAD_PP_ESEM_KEY_TYPE_LIF_VSI; 
        esem_key->key_info.lif_vsi.vsi = match->vpn; 
        esem_key->key_info.lif_vsi.lif = gport_hw_resources.local_out_lif; 
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;

}


int 
_bcm_vlan_port_match_egress_verify(    
    int unit, 
    bcm_gport_t port, 
    int lif_id,
    bcm_port_match_info_t *match)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* For new SEM lookup, like oulif + VSI -> outAC */
    if (match->match == BCM_PORT_MATCH_PORT_VPN) {

        /* check tunnel gport */
        _bcm_dpp_gport_hw_resources tunnel_gport_hw_resources, ac_gport_hw_resources;
        _bcm_lif_type_e tunnel_lif_usage, ac_lif_usage;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, match->port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS
                                                          | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                     &tunnel_gport_hw_resources));

        /* check tunnel gport has a local outlif */
        if (tunnel_gport_hw_resources.local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("get local outlif from gport match->port failed")));
        }

        /* check it's a tunnel gport */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, tunnel_gport_hw_resources.local_out_lif, NULL, &tunnel_lif_usage));
        if ((tunnel_lif_usage != _bcmDppLifTypeMplsPort)
            && (tunnel_lif_usage != _bcmDppLifTypeMim)
            && (tunnel_lif_usage != _bcmDppLifTypeTrill)
            && (tunnel_lif_usage != _bcmDppLifTypeIpTunnel)
            && (tunnel_lif_usage != _bcmDppLifTypeL2Gre)
            && (tunnel_lif_usage != _bcmDppLifTypeVxlan)
            ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("match->port is not a tunnel gport")));
        }

        /* check VSI is in range */
        if (match->vpn >= _BCM_GPORT_NOF_VSIS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI out of range")));
        }

        /* check native AC gport */

        /* check native ac gport has a local outlif */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS
                                                          | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                     &ac_gport_hw_resources));

        /* check native AC gport has a local outlif */
        if (ac_gport_hw_resources.local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("get local outlif from port failed")));
        }

        /* check it's a native ac gport */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, ac_gport_hw_resources.local_out_lif, NULL, &ac_lif_usage));
        if (ac_lif_usage != _bcmDppLifTypeNativeVlan) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("AC gport is not a native AC gport")));
        }
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_vlan_port_match_egress_add
 * Purpose:
 *      Add an Egress match to an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      lif - (IN) LIF ID 
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_vlan_port_match_egress_add(
    int unit, 
    bcm_gport_t port, 
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_PPC_EG_AC_VBP_KEY vbp_key;
    SOC_PPC_EG_AC_CEP_PORT_KEY cep_key;
    SOC_PPC_PORT_INFO port_info;
    uint8 is_cep = 0;
    bcm_vlan_port_t vlan_port;
    SOC_PPC_LIF_ID out_lif_id_tmp;
    SOC_PPC_EG_AC_INFO eg_ac_info;
    int core;
    SOC_PPC_PORT soc_ppd_port_i;
    bcm_port_t port_i;
    uint32 soc_sand_rv;
    bcm_error_t rv;
    uint8 is_cep_lookup = 1;
    uint8 found;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_EG_AC_CEP_PORT_KEY_clear(&cep_key);
    SOC_PPC_EG_AC_VBP_KEY_clear(&vbp_key);    

    /* For new SEM lookup, like oulif + VSI -> outAC, we enable them in API bcm_port_match only. Not in vlan port create.
       Therefore, the logic is a bit different: we don't convert the match to vlan.
       - verify parameters 
       - convert match to sem key
       - add esem entry
    */
    BCMDNX_IF_ERR_EXIT(_bcm_vlan_port_match_egress_verify(unit, port, lif_id, match)); 

    if ((SOC_IS_JERICHO_PLUS(unit)) && (match->match == BCM_PORT_MATCH_PORT_VPN)) {
        ARAD_PP_ESEM_KEY esem_key;
        ARAD_PP_ESEM_ENTRY esem_val; 
        uint8 found; 
        SOC_SAND_SUCCESS_FAILURE success; 

        sal_memset(&esem_key,   0x0, sizeof(ARAD_PP_ESEM_KEY));
        sal_memset(&esem_val, 0x0, sizeof(ARAD_PP_ESEM_ENTRY));

        /* configure SEM key */
        BCMDNX_IF_ERR_EXIT(_bcm_vlan_port_port_match_info_to_esem_key(unit, match, &esem_key)); 

        /* check that the ESEM entry doesn't already exist */
        rv = arad_pp_esem_entry_get_unsafe(unit, &esem_key, &esem_val, &found);
        BCM_SAND_IF_ERR_EXIT(rv);
        if (found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("match already exist")));
        }

        /* configure SEM result: AC lif id */
        esem_val.out_ac = lif_id; 

        /* add entry to ESEM */
        rv = arad_pp_esem_entry_add_unsafe(
                unit,
                &esem_key,
                &esem_val,
                &success
              );
        BCM_SAND_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }  

    rv = _bcm_petra_port_convert_match_to_vlan_port(unit, port, match, &vlan_port);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, vlan_port.port, 0, &gport_info)); 

    if (!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) { /* Nothing to do here */
        BCM_EXIT;
    }

    /* 
     * In case match is only PORT (and VSI is invalid) then Set Default-Port-Out-AC
     */
    if (vlan_port.criteria == BCM_VLAN_PORT_MATCH_PORT && vlan_port.vsi == BCM_VLAN_ALL) {

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

            soc_sand_rv = soc_ppd_port_info_get(unit,core,soc_ppd_port_i,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    
            /* check if default egress ac is a null entry */
            if (port_info.dflt_egress_ac  !=  ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("match already exist")));
            }
        
            port_info.dflt_egress_ac = lif_id;
            port_info.flags = SOC_PPC_PORT_EPNI_PP_PCT_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit,core,soc_ppd_port_i,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {        

        rv = _bcm_dpp_gport_fill_out_ac_key(unit, &vlan_port, &is_cep, &vbp_key, &cep_key);
        BCMDNX_IF_ERR_EXIT(rv);

        /* CEP lookup is done in case port is CEP and match is untagged (PORT or PORT_INITIAL_VID) */
        is_cep_lookup = (is_cep) && (match->match != BCM_PORT_MATCH_PORT) && (match->match != BCM_PORT_MATCH_PORT_INITIAL_VLAN)
        ;

        /* Get out ac id */
        if (!is_cep_lookup) {
            soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_get(unit, &vbp_key, &out_lif_id_tmp, &eg_ac_info, &found);               
        } else {
            soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_get(unit, &cep_key, &out_lif_id_tmp, &eg_ac_info, &found);                
        }
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("match already exist")));
        }

        soc_sand_rv = soc_ppd_eg_ac_info_get(unit, lif_id, &eg_ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (!is_cep_lookup) {
            soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_add(unit, lif_id, &vbp_key, &eg_ac_info, &failure_indication);               
        } else {
            soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_add(unit, lif_id, &cep_key, &eg_ac_info, &failure_indication);                
        }
       
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_extender_port_match_add
 * Purpose:
 *      Add an Extender match to an existing port
 * Parameters:
 *      unit    - (IN) Unit number
 *      lif_id  - (IN) Lif ID
 *      match   - (IN) Match criterias
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_extender_port_match_add(
    int unit,
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    uint32 priority;
    uint8 hit_bit, found;
    int payload, is_local;
    bcm_error_t rv = BCM_E_NONE;
    char *msg;

    SOCDNX_INIT_FUNC_DEFS;

    if (!_BCM_DPP_EXTENDER_IS_INIT(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Extender not initialized")));
    }

    /* There's no point in adding an ingress match to a remote port. */
    rv = _bcm_dpp_gport_is_local(unit, match->port, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_local) {
        BCM_EXIT;
    }

    if (match->match == BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_INITIAL_VLAN) {
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_UT_SEM_A;
        msg = "Initial";
    } else {
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A;
        msg = "Outer";
    }

    rv = _bcm_extender_port_dbal_qual_vals_get(unit, match, qual_vals);
    BCM_SAND_IF_ERR_EXIT(rv);

    /* Check for existing match */
    rv = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, &priority, &hit_bit, &found);
    BCM_SAND_IF_ERR_EXIT(rv);

    if (found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Trying to add existing match. Port: %d, ECID: %d, %s VID: %d "),
                                           match->port, match->extended_port_vid, msg, match->match_vlan));
    }

    /* Add match to SEM */
    rv = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0, &lif_id, &success);
    BCM_SAND_IF_ERR_EXIT(rv);
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_mim_port_match_add
 * Purpose:
 *      Add an MiM match to an existing port
 * Parameters:
 *      unit    - (IN) Unit number
 *      lif_id  - (IN) Lif ID
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_mim_port_match_add(
    int unit, 
    int lif_id, 
    bcm_port_match_info_t *match)
{
    bcm_error_t                           rv = BCM_E_NONE;
    SOC_PPC_L2_LIF_ISID_KEY               isid_key;
    SOC_PPC_L2_LIF_ISID_INFO              isid_info;
    SOC_PPC_LIF_ID                        lif_index;
    SOC_PPC_SYS_VSI_ID                    vsi_index;
    uint8  found;
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE success;
    uint32 soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 bsa_nickname;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    bsa_nickname = match->match_tunnel_srcmac[2] | (match->match_tunnel_srcmac[1] << 8) | ((match->match_tunnel_srcmac[0] & 0xf0) << 12); /* bsa_nickname = bsa[47:44],bsa[39:24] */

    /* Check that this ISID is not already in the ISEM tables */
    SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
    isid_key.isid_domain = 0; /* isid_domain is always 0 */
    isid_key.isid_id = match->isid;
    if (match->flags & BCM_PORT_MATCH_MIM_SERVICE_SMAC) {
        isid_key.bsa_nickname = bsa_nickname;
        isid_key.bsa_nickname_valid = 1;
    }
    SOC_PPC_L2_LIF_ISID_INFO_clear(&isid_info);

    soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &lif_index, &vsi_index, &isid_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Trying to add an ISID that already exists - 0x%x"), match->isid));
    }

    SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
    isid_key.isid_domain = 0; /* isid_domain is always 0 */
    isid_key.isid_id = match->isid;
    if (match->flags & BCM_PORT_MATCH_MIM_SERVICE_SMAC) {
        isid_key.bsa_nickname = bsa_nickname;
        isid_key.bsa_nickname_valid = 1;
    }
    SOC_PPC_L2_LIF_ISID_INFO_clear(&isid_info);
    /* The below fields in the isid_info struct are set just so the verify function will pass. We don't need this struct and it has no effect */
    isid_info.service_type = SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP;
    isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
    soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, 0, &isid_key, lif_id, &isid_info, TRUE, FALSE, TRUE, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_SAND_IF_ERR_EXIT(rv);
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;

}

/*
 * Function:
 *      _bcm_petra_port_match_verify
 * Purpose:
 *      Verify parameters for bcm_port_match_* APIs
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_petra_port_match_verify(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *match)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (match == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("match must be a valid pointer")));
    }

    if (BCM_GPORT_IS_VLAN_PORT(port)) {
        if ((match->flags & BCM_PORT_MATCH_INGRESS_ONLY) && (match->flags & BCM_PORT_MATCH_EGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_port_match do not support both INGRESS and EGRESS only flags set ")));
        }
        if (!(match->flags & (BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_EGRESS_ONLY))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("INGRESS_ONLY or EGRESS_ONLY needs to be set")));
        }

        /* unsuported match flags*/
        switch (match->match) {
        case BCM_PORT_MATCH_PORT_PCP_VLAN:
        case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            if (SOC_DPP_CONFIG(unit)->pp.vlan_match_db_mode != SOC_DPP_VLAN_DB_MODE_PCP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid match criteria")));
            }
            break;
        case BCM_PORT_MATCH_PORT_UNTAGGED:
            if (!SOC_DPP_CONFIG(unit)->pp.custom_feature_vt_tst1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("UNTAGGED database is not valid criteria")));
            }
            break;
        default:
            break;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        if (!(match->flags & BCM_PORT_MATCH_INGRESS_ONLY) || (match->flags & BCM_PORT_MATCH_EGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_port_match supports only INGRESS flag set for Extender port")));
        }
    } else if (BCM_GPORT_IS_TUNNEL(port)) {
        if (!(match->flags & BCM_PORT_MATCH_INGRESS_ONLY) || (match->flags & BCM_PORT_MATCH_EGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_port_match supports only INGRESS flag set for MiM port")));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("API bcm_petra_port_match receives only VLAN, MiM or Extender ports")));
    }

    if (!(match->flags & BCM_PORT_MATCH_MIM) && (match->flags & BCM_PORT_MATCH_MIM_SERVICE_SMAC)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MATCH_MIM must be set for MIM_SERVICE_SMAC")));
    }
    if (match->flags & BCM_PORT_MATCH_INGRESS_ONLY) {
        /* match_pcp is only used in ingress device */
        BCM_DPP_VLAN_CHK_PRIO(unit, match->match_pcp);
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_match_add
 * Purpose:
 *      Add a match to an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_port_match_add(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *match)
{
    _bcm_lif_type_e usage, out_lif_usage, in_lif_usage;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int is_local;
    bcm_error_t rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_port_match_verify(unit, port, match);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the LIF - InLIF or OutLIF */
    rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_local) {
        BCM_EXIT;
    }

    /* Get local LIF from gport */
    rv = _bcm_dpp_gport_to_hw_resources(unit, port, ((match->flags & BCM_PORT_MATCH_INGRESS_ONLY) ? _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS:_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
                                                      | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify the type (VLAN or Extender)for the relevant LIF */
    rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, gport_hw_resources.local_in_lif, gport_hw_resources.local_out_lif, &in_lif_usage, &out_lif_usage);
    BCMDNX_IF_ERR_EXIT(rv);
    usage = (gport_hw_resources.local_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? out_lif_usage : in_lif_usage;

    if (usage == _bcmDppLifTypeVlan) {
        /* Call the appropriate Ingress or Egress match add function */
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_vlan_port_match_ingress_add(unit, port, gport_hw_resources.local_in_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_vlan_port_match_egress_add(unit, port, gport_hw_resources.local_out_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (usage == _bcmDppLifTypeNativeVlan) {
        /* Native Vlan is only for Egress. */
        if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_vlan_port_match_egress_add(unit, port, gport_hw_resources.local_out_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (usage == _bcmDppLifTypeExtender) {
        if (match->match != BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_VLAN && match->match != BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_INITIAL_VLAN){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API bcm_petra_port_match_add received invalid extender match criterion")));
        }
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_extender_port_match_add(unit, gport_hw_resources.local_in_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API bcm_petra_port_match_add received invalid port")));
        }
    } else if (usage == _bcmDppLifTypeMim) {
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_mim_port_match_add(unit, gport_hw_resources.local_in_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API bcm_petra_port_match_add received invalid port")));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API bcm_petra_port_match_add received invalid port type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_port_is_original_match(
    int unit,
    int lif_id,
    bcm_vlan_port_t *vlan_port,
    bcm_port_match_info_t *match,
    uint8 *is_original_match)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port_tmp;

    BCMDNX_INIT_FUNC_DEFS;

    *is_original_match = 1;

    bcm_vlan_port_t_init(&vlan_port_tmp);

    /* There's no original match for Egress only LIFs, since Egress only LIF creation is allowed only without
       a valid key */
    if ((BCM_GPORT_SUB_TYPE_IS_LIF(vlan_port->vlan_port_id)) &&
         (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)) {
        *is_original_match = 0;
        BCM_EXIT;
    }

    rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port_tmp, lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if LIF was created with BCM_VLAN_PORT_MATCH_NONE criteria,
       it cannot have original match */
    if (vlan_port_tmp.criteria == BCM_VLAN_PORT_MATCH_NONE) {
        *is_original_match = 0;
        BCM_EXIT;
    }

    if (match->flags & BCM_PORT_MATCH_INGRESS_ONLY) {
        /* Verify match key between vlan port tmp and vlan port */
        if (vlan_port_tmp.criteria != vlan_port->criteria ||
            vlan_port_tmp.match_vlan != vlan_port->match_vlan ||
            vlan_port_tmp.match_inner_vlan != vlan_port->match_inner_vlan ||
            vlan_port_tmp.match_tunnel_value != vlan_port->match_tunnel_value ||
            vlan_port_tmp.match_ethertype != vlan_port->match_ethertype ||
            vlan_port_tmp.match_pcp != vlan_port->match_pcp ||
            vlan_port_tmp.port != vlan_port->port) {
            *is_original_match = 0;
        }
    }

    if (match->flags & BCM_PORT_MATCH_EGRESS_ONLY) {
        /* Verify match key between vlan port tmp and vlan port */
        if (vlan_port_tmp.vsi != vlan_port->vsi ||
            vlan_port_tmp.match_inner_vlan != vlan_port->match_inner_vlan ||
            vlan_port_tmp.port != vlan_port->port) {
            *is_original_match = 0;
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_port_match_ingress_delete
 * Purpose:
 *      Remove an Ingress match from an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      lif_id - (IN) Logical Interface ID 
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes: 
 *      Removal of the original match (original i.e. from bcm_petra_vlan_port_create)
 *      is not currently supported.
 */

int 
_bcm_petra_port_match_ingress_delete(
    int unit, 
    bcm_gport_t port,
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_PPC_L2_LIF_AC_KEY  in_ac_key;
    SOC_PPC_L2_LIF_AC_INFO ac_info;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ID lif_id_tmp;
    uint32 soc_sand_rv;
    uint8 found;
    bcm_error_t rv;
    bcm_vlan_port_t vlan_port;
    uint8 is_original_match;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    rv = _bcm_petra_port_convert_match_to_vlan_port(unit, port, match, &vlan_port);
    BCMDNX_IF_ERR_EXIT(rv);



    rv = _bcm_dpp_gport_fill_ac_key(unit, &vlan_port, BCM_GPORT_VLAN_PORT, &in_ac_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* look up existing entry */
    soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &in_ac_key, &lif_id_tmp, &ac_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("match does not exist")));
    }    


    /* 
     * Verify if this match is similiar to original SW DB match for this VLAN port
     */ 
    rv = _bcm_petra_port_is_original_match(unit, lif_id, &vlan_port, match, &is_original_match);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_original_match) {
        /* Clean original match SW DB */
        /* Will be supported in a later stage */
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("currently API does not support original match removal")));  
    }


    /* Remove this match entry without removing LIF */    
    /* NULL indicates do not remove also LIF */
    soc_sand_rv = soc_ppd_l2_lif_ac_remove(soc_sand_dev_id, &in_ac_key, NULL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_port_match_egress_delete
 * Purpose:
 *      Remove an Egress match from an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      lif_id - (IN) Logical Interface ID 
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes: 
 *      Removal of the original match (original i.e. from bcm_petra_vlan_port_create)
 *      is not currently supported.
 */

int 
_bcm_petra_port_match_egress_delete(
    int unit, 
    bcm_gport_t port,
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_PPC_EG_AC_VBP_KEY vbp_key;
    SOC_PPC_EG_AC_CEP_PORT_KEY cep_key;
    uint8 is_cep_lookup = 1, is_cep = 0;
    bcm_vlan_port_t vlan_port;
    SOC_PPC_LIF_ID out_lif_id_tmp;
    SOC_PPC_EG_AC_INFO eg_ac_info;
    int core;
    SOC_PPC_PORT soc_ppd_port_i;
    SOC_PPC_PORT_INFO port_info;
    bcm_port_t port_i;
    uint32 soc_sand_rv = SOC_SAND_OK;
    bcm_error_t rv;
    uint8 found;
    uint8 is_original_match;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_EG_AC_CEP_PORT_KEY_clear(&cep_key);
    SOC_PPC_EG_AC_VBP_KEY_clear(&vbp_key);

    if ((SOC_IS_JERICHO_PLUS(unit)) && (match->match == BCM_PORT_MATCH_PORT_VPN)) {
        ARAD_PP_ESEM_KEY esem_key;
        uint8  success; 

        sal_memset(&esem_key,   0x0, sizeof(ARAD_PP_ESEM_KEY));

        /* configure SEM key */
        BCMDNX_IF_ERR_EXIT(_bcm_vlan_port_port_match_info_to_esem_key(unit, match, &esem_key)); 

        /* add entry to ESEM */
        rv = arad_pp_esem_entry_remove_unsafe(
                unit,
                &esem_key,
                &success
              );
        BCM_SAND_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }  

    rv = _bcm_petra_port_convert_match_to_vlan_port(unit, port, match, &vlan_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * In case match is only PORT (and VSI is invalid) then Set Default-Port-Out-AC
     */
    if (vlan_port.criteria == BCM_VLAN_PORT_MATCH_PORT && vlan_port.vsi == BCM_VLAN_ALL) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, vlan_port.port, 0, &gport_info)); 

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
                        
            soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
            out_lif_id_tmp = port_info.dflt_egress_ac;                 

            /* check out_lif_id_tmp isn't the null entry, allocated at init */
            if (out_lif_id_tmp ==  ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("match does not exist")));
            }
        }
    } else {
      rv = _bcm_dpp_gport_fill_out_ac_key(unit, &vlan_port, &is_cep, &vbp_key, &cep_key);
      BCMDNX_IF_ERR_EXIT(rv);

      /* CEP lookup is done in case port is CEP and match is untagged (PORT) */
      is_cep_lookup = (is_cep) && (match->match != BCM_PORT_MATCH_PORT) && (match->match != BCM_PORT_MATCH_PORT_INITIAL_VLAN);

      /* Get out ac id */
      if (!is_cep_lookup) {
          soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_get(unit, &vbp_key, &out_lif_id_tmp, &eg_ac_info, &found);               
      } else {
          soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_get(unit, &cep_key, &out_lif_id_tmp, &eg_ac_info, &found);                
      }
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      if (!found) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("match does not exist")));
      }
    }

    /* 
     * Verify if this match is similiar to original SW DB match for this VLAN port
     */ 
    rv = _bcm_petra_port_is_original_match(unit, lif_id, &vlan_port, match, &is_original_match);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_original_match) {
        /* Clean original match SW DB */
        /* Will be supported in a later stage */
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("currently API does not support original match removal")));  
    }

    if (vlan_port.criteria == BCM_VLAN_PORT_MATCH_PORT && vlan_port.vsi == BCM_VLAN_ALL) {
        /* Return to default SDK egress AC */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

             /* this outlif points to a null entry, allocated at init */
            port_info.dflt_egress_ac = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID); 
            port_info.flags = SOC_PPC_PORT_EPNI_PP_PCT_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
        if (!is_cep_lookup) {
            soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_remove(unit, &vbp_key, &out_lif_id_tmp);               
        } else {
            soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_remove(unit, &cep_key, &out_lif_id_tmp);                
        }
    }
   
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_extender_port_match_delete
 * Purpose:
 *      Remove an Extender match from an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      lif_id - (IN) Logical Interface ID
 *      match - (IN) Match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Removal of the original match (original i.e. from bcm_petra_extender_port_add)
 *      is not currently supported.
 */
int
_bcm_extender_port_match_delete(
    int unit,
    bcm_gport_t port,
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    int lif;
    bcm_extender_port_t extender_port;
    bcm_error_t rv = BCM_E_NONE;
    uint8 found, hit_bit;
    uint32 priority;
    char *msg;

    SOCDNX_INIT_FUNC_DEFS;

    if (!_BCM_DPP_EXTENDER_IS_INIT(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Extender not initialized")));
    }

    bcm_extender_port_t_init(&extender_port);
    extender_port.extender_port_id = port;
    rv = bcm_petra_extender_port_get(unit, &extender_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Not allowed to delete original extender port match*/
    if (extender_port.port == match->port &&
        extender_port.extended_port_vid == match->extended_port_vid &&
        extender_port.match_vlan == match->match_vlan) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API does not support original match removal")));
    }

    rv = _bcm_extender_port_dbal_qual_vals_get(unit, match, qual_vals);
    BCM_SAND_IF_ERR_EXIT(rv);

    if (match->match == BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_INITIAL_VLAN) {
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_UT_SEM_A;
        msg = "Initial";
    } else {
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A;
        msg = "Outer";
    }
    /* Check that the match exists */
    rv = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &lif, &priority, &hit_bit, &found);
    BCM_SAND_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trying to remove non existing match. port: %d, ECID: %d, %s VID: %d "),
                                               match->port, match->extended_port_vid, msg, match->match_vlan));
    }

    /* Check that match.gport is identical to gport */
    if (lif != lif_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Trying to remove match of different Gport")));
    }

    /* remove match from SEM */
    rv = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success);
    BCM_SAND_IF_ERR_EXIT(rv);
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_mim_port_match_delete (
    int unit,
    bcm_gport_t port,
    int lif_id,
    bcm_port_match_info_t *match)
{
    SOC_PPC_L2_LIF_ISID_KEY               isid_key;
    SOC_PPC_L2_LIF_ISID_INFO              isid_info;
    SOC_PPC_LIF_ID                        lif_index;
    SOC_PPC_SYS_VSI_ID                    vsi_index;
    uint8  found;
    uint32 soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 bsa_nickname;
    uint32 sw_state_isid, sw_state_bsa_nickname;
    uint8 sw_state_bsa_nickname_valid;
    uint32 mim_info_flags, mim_info_bsa_nickname;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    
    BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_flags.get(unit, match->vpn, &mim_info_flags));
    BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_bsa_nickname.get(unit, match->vpn, &mim_info_bsa_nickname));
    sw_state_isid = _BCM_DPP_MIM_VPN_ISID_GET(mim_info_flags);
    sw_state_bsa_nickname = _BCM_DPP_MIM_VPN_BSA_NICKNAME_GET(mim_info_bsa_nickname);
    sw_state_bsa_nickname_valid = _BCM_DPP_MIM_VPN_IS_BSA_NICKNAME_VALID(mim_info_bsa_nickname);

    bsa_nickname = match->match_tunnel_srcmac[2] | (match->match_tunnel_srcmac[1] << 8) | ((match->match_tunnel_srcmac[0] & 0xf0) << 12); /* bsa_nickname = bsa[47:44],bsa[39:24] */

    /* Not allowed to delete the ISEM entry created when the MiM VPN was created */
    if ((sw_state_isid == match->isid) && (!sw_state_bsa_nickname_valid || (sw_state_bsa_nickname == bsa_nickname))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API does not support original match removal")));
    }

    SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
    isid_key.isid_domain = 0; /* isid_domain is always 0 */
    isid_key.isid_id = match->isid;
    if (match->flags & BCM_PORT_MATCH_MIM_SERVICE_SMAC) {
        isid_key.bsa_nickname = bsa_nickname;
        isid_key.bsa_nickname_valid = 1;
    }
    SOC_PPC_L2_LIF_ISID_INFO_clear(&isid_info);

    soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &lif_index, &vsi_index, &isid_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trying to remove a non existing ISID - 0x%x"), match->isid));
    }
    if (lif_index != lif_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Trying to remove match of different Gport")));
    }
    SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
    isid_key.isid_domain = 0; /* isid_domain is always 0 */
    isid_key.isid_id = match->isid;
    if (match->flags & BCM_PORT_MATCH_MIM_SERVICE_SMAC) {
        isid_key.bsa_nickname = bsa_nickname;
        isid_key.bsa_nickname_valid = 1;
    }
    soc_sand_rv = soc_ppd_l2_lif_isid_remove(soc_sand_dev_id, &isid_key, TRUE, FALSE, 0, TRUE, &lif_index);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

    exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_match_delete
 * Purpose:
 *      Remove a match from an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      match - (IN) Match criterias
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_port_match_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *match)
{
    bcm_error_t rv = BCM_E_NONE;
    _bcm_lif_type_e usage, out_lif_usage, in_lif_usage;
    int is_local;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_port_match_verify(unit, port, match);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the LIF - InLIF or OutLIF */
    rv = _bcm_dpp_gport_is_local(unit, port, &is_local);       
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_local) {
        BCM_EXIT;
    }

    /* Get local LIF from gport */
    rv = _bcm_dpp_gport_to_hw_resources(unit, port, ((match->flags & BCM_PORT_MATCH_INGRESS_ONLY) ? _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS:_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
                                                      | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify the type (VLAN or Extender) for the relevant LIF */
    rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, gport_hw_resources.local_in_lif, gport_hw_resources.local_out_lif, &in_lif_usage, &out_lif_usage);
    BCMDNX_IF_ERR_EXIT(rv);
    usage = (gport_hw_resources.local_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? out_lif_usage : in_lif_usage;

    if (usage == _bcmDppLifTypeVlan) {
        /* Call the appropriate Ingress or Egress match delete function */
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_petra_port_match_ingress_delete(unit, port, gport_hw_resources.local_in_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_petra_port_match_egress_delete(unit, port, gport_hw_resources.local_out_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (usage == _bcmDppLifTypeNativeVlan) {
        /* Native Vlan is only for Egress. */
        if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_petra_port_match_egress_delete(unit, port, gport_hw_resources.local_out_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }else if (usage == _bcmDppLifTypeExtender) {

        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_extender_port_match_delete(unit, port, gport_hw_resources.local_in_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API bcm_petra_port_match_delete received invalid port")));
        }
    } else if (usage == _bcmDppLifTypeMim) {
        if (gport_hw_resources.local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            rv = _bcm_mim_port_match_delete(unit, port, gport_hw_resources.local_in_lif, match);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API bcm_petra_port_match_delete received invalid port")));
        }

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API bcm_petra_port_match_delete received invalid port type")));
    }    

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_port_match_replace
 * Purpose:
 *      Replace an old match with a new one for an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      old_match - (IN) Old match criteria
 *      new_match - (IN) New match criteria
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_port_match_replace(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *old_match, 
    bcm_port_match_info_t *new_match)
{
    bcm_error_t rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_port_match_add(unit, port, new_match);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_petra_port_match_delete(unit, port, old_match);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_port_match_multi_get
 * Purpose:
 *      Get all the matches for an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      size - (IN) Number of elements in match array
 *      match_array - (OUT) Match array
 *      count - (OUT) Match count
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_port_match_multi_get(
    int unit, 
    bcm_gport_t port, 
    int size, 
    bcm_port_match_info_t *match_array, 
    int *count)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_match_multi_get not supported ")));

exit:
    BCMDNX_FUNC_RETURN;    
}

/*
 * Function:
 *      bcm_petra_port_match_set
 * Purpose:
 *      Assign a set of matches to an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 *      size - (IN) Number of elements in match array
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
/* Generic port match attribute structure */
int 
bcm_petra_port_match_set(
    int unit, 
    bcm_gport_t port, 
    int size, 
    bcm_port_match_info_t *match_array)
{
    int i;
    bcm_error_t rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (size <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("size must be higher than 0 ")));
    }

    if (match_array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("match_array is invalid")));
    }

    /* Add every single match */
    for (i = 0; i < size; i++) {
        rv = bcm_petra_port_match_add(unit, port, match_array + i);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    BCMDNX_FUNC_RETURN;      
}

/*
 * Function:
 *      bcm_petra_port_match_delete_all
 * Purpose:
 *      Delete all matches for an existing port
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port or gport
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_port_match_delete_all(
    int unit, 
    bcm_gport_t port)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_match_delete_all not supported ")));

exit:
    BCMDNX_FUNC_RETURN;  
}

/*
 * init acceptable frame type buffer 
 */
 int _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t_clear(int unit, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *tpid_class_acceptable_frame_info) {
  BCMDNX_INIT_FUNC_DEFS;
  BCMDNX_NULL_CHECK(tpid_class_acceptable_frame_info);
  sal_memset(tpid_class_acceptable_frame_info, 0x0, sizeof(_bcm_dpp_port_tpid_class_acceptable_frame_type_info_t));

exit:
    BCMDNX_FUNC_RETURN; 
 }


/* 
 * convert _bcm_dpp_port_tpid_class_info_t to _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t
 */
void _bcm_dpp_port_tpid_class_info_t_to_bcm_dpp_port_tpid_class_acceptable_frame_type_info_t(
   int unit, 
   _bcm_dpp_port_tpid_class_info_t *tpid_class_info, 
   _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *tpid_class_acceptable_frame_info) 
{
    int tag_structure_id;
    uint32 acceptable_frame_bit;
    uint32 buffer_lcl[1];

    /* get "acceptable frame" from "tpid class info" for each tag structure (32) and set it in "tpid class acceptable frame info" */

    /* get SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS*SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS*2 = 32 */
    for(tag_structure_id = 0; tag_structure_id < SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS*SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS*2; tag_structure_id++) {
        /* get the acceptable frame info bit for tag structure id*/
        acceptable_frame_bit = SHR_BITGET(tpid_class_info->buff, tag_structure_id * _BCM_DPP_PORT_TPID_CLASS_INFO_BUF_NOF_U32 + 11);  
        if (acceptable_frame_bit) {
            /* update accpetable frame for tag structure */
            *buffer_lcl = tpid_class_acceptable_frame_info->buffer;
            SHR_BITSET(buffer_lcl, tag_structure_id);
            tpid_class_acceptable_frame_info->buffer = *buffer_lcl;
        }
    }
}

/*
 * Function:
 *      bcm_petra_extender_port_mapping_info_set
 * Purpose:
 *      Configure port extender mapping from different inputs to Local pp port
 * Parameters:
 *      unit - (IN) unit number.
 *      flags - (IN) BCM_EXTENDER_PORT_MAPPING_XXXX
 *      type - (IN) type of mapping info
 *      mapping_info - (IN) mapping info (PON port+tunnel id/VLAN/user define value)
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write internal tables entries
 *      BCM_E_UNAVAIL - feature unavaliable
 * Notes:
 */
int bcm_petra_port_extender_mapping_info_set(
    int unit, 
    uint32 flags, 
    bcm_port_extender_mapping_type_t type, 
    bcm_port_extender_mapping_info_t *mapping_info)
{
    int rv = 0;
    _bcm_dpp_gport_info_t gport_info;
    SOC_TMC_PORTS_APPLICATION_MAPPING_INFO info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mapping_info);

    if (flags & BCM_PORT_EXTENDER_MAPPING_INGRESS) {
        switch(type) {
            case bcmPortExtenderMappingTypePonTunnel:
                rv = bcm_petra_port_pon_tunnel_map_set(unit, mapping_info->phy_port, mapping_info->tunnel_id, mapping_info->pp_port);
                break;
        
            case bcmPortExtenderMappingTypePortVlan:
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, mapping_info->phy_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

                info.type = SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER;
                info.value.pp_port_ext.index = mapping_info->vlan;

                rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_get, (unit, gport_info.local_port, &info)));
                BCMDNX_IF_ERR_EXIT(rv);
        
                info.value.pp_port_ext.pp_port = mapping_info->pp_port;
                rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_set, (unit, gport_info.local_port, &info)));
                BCMDNX_IF_ERR_EXIT(rv);
                break;
        
            case bcmPortExtenderMappingTypeUserDefineValue:                
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, mapping_info->phy_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

                info.type = SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER;
                info.value.pp_port_ext.index = mapping_info->user_define_value;
                rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_get, (unit, gport_info.local_port, &info)));
                BCMDNX_IF_ERR_EXIT(rv);

                info.value.pp_port_ext.pp_port = mapping_info->pp_port;
                rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_set, (unit, gport_info.local_port, &info)));
                BCMDNX_IF_ERR_EXIT(rv);
                break;
                
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type of mapping_info is not supported.\n")));
                break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_extender_port_mapping_info_get
 * Purpose:
 *      Get port extender mapping info (Local pp port) from inputs
 * Parameters:
 *      unit - (IN) unit number.
 *      flags - (IN) BCM_EXTENDER_PORT_MAPPING_XXXX
 *      type - (IN) type of mapping info
 *      mapping_info - (INOUT) mapping info (PON port+tunnel id/VLAN/user define value)
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write internal tables entries
 *      BCM_E_UNAVAIL - feature unavaliable
 * Notes:
 */
int bcm_petra_port_extender_mapping_info_get(
    int unit, 
    uint32 flags, 
    bcm_port_extender_mapping_type_t type, 
    bcm_port_extender_mapping_info_t *mapping_info)
{
    int rv = 0;
    _bcm_dpp_gport_info_t gport_info;
    SOC_TMC_PORTS_APPLICATION_MAPPING_INFO info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mapping_info);

    if (flags & BCM_PORT_EXTENDER_MAPPING_INGRESS) {
        switch(type) {
            case bcmPortExtenderMappingTypePonTunnel:
                rv = bcm_petra_port_pon_tunnel_map_get(unit, mapping_info->phy_port, mapping_info->tunnel_id, &mapping_info->pp_port);
                break;

            case bcmPortExtenderMappingTypePortVlan:
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, mapping_info->phy_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

                info.type = SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER;
                info.value.pp_port_ext.index = mapping_info->vlan;
                rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_get, (unit, gport_info.local_port, &info)));
                BCMDNX_IF_ERR_EXIT(rv);

                mapping_info->pp_port = info.value.pp_port_ext.pp_port;
                break;

            case bcmPortExtenderMappingTypeUserDefineValue:
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, mapping_info->phy_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

                info.type = SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER;
                info.value.pp_port_ext.index = mapping_info->user_define_value;
                rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_get, (unit, gport_info.local_port, &info)));
                BCMDNX_IF_ERR_EXIT(rv);

                mapping_info->pp_port = info.value.pp_port_ext.pp_port;
                break;
                
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type of mapping_info is not supported.\n")));
                break;
        }
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_port_wide_data_get_lif
 * Purpose:
 *      Helper function for the _bcm_dpp_port_wide_data_* functions. Given a gport and direction flags, returns
 *          the local lif and ingress indication.
 * Parameters:
 *      unit        - (IN) unit number.
 *      flags       - (IN) BCM_PORT_WIDE_DATA_* flags.
 *      gport       - (IN) Gport using the lif.
 *      local_lif   - (OUT)Local lif id mapped to this gport.
 *      is_ingress  - (OUT)Indication whether lif is ingress or egress.
 * Returns:
 *      BCM_E_NONE      - success
 *      BCM_E_PARAM     - Flags were incorrect
 *      BCM_E_NOT_FOUND - Gport is not mapped to a lif.
 *      BCM_E_UNAVAIL   - Soc doesn't support wide data entries.
 *      BCM_E_*         - Otherwise.
 * Notes:
 */
int
_bcm_dpp_port_wide_data_get_lif(int unit, uint32 flags, bcm_gport_t gport, int *local_lif, uint8 *is_ingress){
    int rv;
    uint8 is_egress = FALSE, tmp_is_ingress = FALSE;
    int lif_id;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_port_wide_data_set is not supported in this device")));
    }

    tmp_is_ingress = DPP_IS_FLAG_SET(flags, BCM_PORT_WIDE_DATA_INGRESS); 
    is_egress = DPP_IS_FLAG_SET(flags, BCM_PORT_WIDE_DATA_EGRESS);

    if (!(tmp_is_ingress ^ is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Exactly one flag must be set: BCM_PORT_WIDE_DATA_EGRESS/INGRESS")));
    }

    /* Get local LIF from gport */
    rv = _bcm_dpp_gport_to_hw_resources(unit, gport, ((tmp_is_ingress) ? _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS:_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
                                                      | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    


    lif_id = (tmp_is_ingress) ? gport_hw_resources.local_in_lif : gport_hw_resources.local_out_lif;
    /* Finally, fill the user's pointers. */
    *local_lif = lif_id;
    *is_ingress = tmp_is_ingress;

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_port_wide_data_set(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    uint64 data)
{
    int rv;
    int lif_id;
    int is_wide_entry = 0;
    uint8 is_ingress;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get the local lif, and ingress/egress indication */
    rv = _bcm_dpp_port_wide_data_get_lif(unit, flags, gport, &lif_id, &is_ingress);
    BCMDNX_IF_ERR_EXIT(rv); 
    
    rv = _bcm_dpp_local_lif_is_wide_entry(unit, lif_id, is_ingress, &is_wide_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_wide_entry) {
        return BCM_E_UNAVAIL; 
    }

    /*Call the SOC API for writing the additional data*/
    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_set,(unit,lif_id, is_ingress, data, FALSE)));
    BCMDNX_IF_ERR_EXIT(rv); 
        
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_port_encap_local_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    int* encap_local_id)
{
    int rv;
    uint8 is_egress = FALSE, is_ingress = FALSE;
    int lif_id;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_encap_local_get is not supported in this device")));
    }

    is_ingress = DPP_IS_FLAG_SET(flags, BCM_PORT_ENCAP_LOCAL_INGRESS); 
    is_egress = DPP_IS_FLAG_SET(flags, BCM_PORT_ENCAP_LOCAL_EGRESS);

    if (!(is_ingress ^ is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Exactly one flag must be set: BCM_PORT_ENCAP_LOCAL_INGRESS/EGRESS")));
    }

    /* Get local LIF from gport */
    rv = _bcm_dpp_gport_to_hw_resources(unit, gport, ((is_ingress) ? _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS:_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
                                                      | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    

    lif_id = (is_ingress) ? gport_hw_resources.local_in_lif : gport_hw_resources.local_out_lif;

    *encap_local_id = lif_id;

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_ingress_tdm_failover_get(
    int unit, 
    int flag, 
    bcm_pbmp_t *tdm_enable_pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit) || SOC_IS_QUX(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_ingress_tdm_failover_get is not supported in this device")));
    }

    BCMDNX_IF_ERR_EXIT(jer_tdm_ingress_failover_get(unit, tdm_enable_pbmp));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_port_ingress_tdm_failover_set(
    int unit, 
    int flag, 
    bcm_pbmp_t tdm_enable_pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit) || SOC_IS_QUX(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_port_ingress_tdm_failover_set is not supported in this device")));
    }

    BCMDNX_IF_ERR_EXIT(jer_tdm_ingress_failover_set(unit, tdm_enable_pbmp));

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get additional data for a wide LIF. */
int bcm_petra_port_wide_data_get(
    int unit, 
    bcm_gport_t gport,
    uint32 flags, 
    uint64 *data)
{
    int rv;
    int lif_id;
    int is_wide_entry;
    uint8 is_ingress;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get the local lif, and ingress/egress indication */
    rv = _bcm_dpp_port_wide_data_get_lif(unit, flags, gport, &lif_id, &is_ingress);
    BCMDNX_IF_ERR_EXIT(rv); 

    rv = _bcm_dpp_local_lif_is_wide_entry(unit, lif_id, is_ingress, &is_wide_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_wide_entry) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The LIF is not an entry with extended data!"))); 
    }

    /*Call the SOC API for reading the additional data*/
    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_get,(unit,lif_id, is_ingress, data)));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_port_nrdy_thresh_allocate(
   int unit, 
   soc_port_t port)
{
    int core, is_allocated, profile_id;
    uint32 tm_port, optimal_val;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core)); 

    /* get optimal nrdy threshold value for the port */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nrdy_th_optimial_value_get ,(unit, port, &optimal_val)));

    /* allocate profile */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_nrdy_threshold_alloc(unit, core, 0, &optimal_val, &is_allocated, &profile_id));

    if (!SOC_IS_FLAIR(unit) && is_allocated) {         /* set profile data */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_queuing_nrdy_th_profile_data_set ,(unit, core, profile_id, optimal_val)));
    }

    /* attach the profile to the port */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nrdy_th_profile_set ,(unit, port, profile_id)));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_port_nrdy_thresh_free(
   int unit, 
   soc_port_t port)
{
    int core, is_last;
    uint32 tm_port, profile_id;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core)); 

    /* get current profile */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nrdy_th_profile_get ,(unit, port, &profile_id)));

    /* free profile */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_nrdy_threshold_free(unit, core, profile_id, &is_last));


exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_port_reassembly_context_get(int unit,
                                     bcm_port_t port, 
                                     uint32 *port_termination_context,
                                     uint32 *reassembly_context)
{
    int rc;

    BCMDNX_INIT_FUNC_DEFS;

    rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_ingr_reassembly_context_get, (unit, port, port_termination_context, reassembly_context));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *    _bcm_dpp_port_mod_port_to_sys_port
 * Description:
 *    map module-ID + mod-port to system port
 *    This function translates local port to tm port if the module ID is the (base) module ID of the local device.
 *    This is wrong behaviour since the port of modports should be a TM port.
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *  modid -   [IN] module id
 *  mode_port -     [IN] module user port
 *  sys_port -     [OUT] unique System port, or SOC_TMC_SYS_PHYS_PORT_INVALID if
 *                       given module or port on module not exist
 * Returns:
 *    BCM_E_XXX
 */
int 
 _bcm_dpp_port_mod_port_to_sys_port(int unit, bcm_module_t modid, bcm_port_t mode_port, unsigned int *sys_port)
{

    uint32 soc_sand_rv;
    uint32 system_port_invalid = 0;
    int rv = BCM_E_NONE;
    bcm_gport_t mod_port_gport = 0;
    bcm_gport_t sys_port_gport = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(sys_port);

    system_port_invalid = ((SOC_DPP_CONFIG(unit)->arad->voq_mapping_mode == VOQ_MAPPING_INDIRECT) ? \
                           SOC_TMC_SYS_PHYS_PORT_INVALID_INDIRECT : SOC_TMC_SYS_PHYS_PORT_INVALID_DIRECT);
    *sys_port = system_port_invalid;/* not really needed - just to avoid coverity defect */

    /* ERP special case */
    if (mode_port == BCM_DPP_PORT_INTERNAL_ERP(0))
    {      
      BCM_GPORT_MODPORT_SET(mod_port_gport,modid,mode_port);
      rv = bcm_petra_stk_gport_sysport_get(unit, mod_port_gport,&sys_port_gport);
      BCMDNX_IF_ERR_EXIT(rv);
      *sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sys_port_gport);
    }
    else
    {
      uint32 port;
      /* The port argument to mbcm_dpp_local_to_sys_phys_port_map_get needs to be a local port
       * if the fap is the local one, and the module port in case it is a remote fap.
       */
      port = mode_port;
      /* get the system port from the modport or local port */
      soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit, modid, port, sys_port)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }    

    if(*sys_port == system_port_invalid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Falied to get system port")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *  _bcm_dpp_port_mod_port_from_sys_port
 * Description:
 *    map module-ID + mod-port to system port
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *  modid -   [IN] module id
 *  mode_port -     [IN] module user port
 *  sys_port -     [OUT] unique System port, or SOC_TMC_SYS_PHYS_PORT_INVALID if
 *                       given module or port on module not exist
 * Returns:
 *    BCM_E_XXX
 */
int 
 _bcm_dpp_port_mod_port_from_sys_port(int unit, bcm_module_t *modid, bcm_port_t *mode_port, uint32 sys_port)
{

    uint32 fapid = 0;
    uint32 local_port = 0;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mode_port);
    BCMDNX_NULL_CHECK(modid);

    /* map sys port to fap and tm-port*/
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit,sys_port,&fapid,&local_port)));
    *modid = (bcm_module_t)fapid;
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* map tm port to mod-port*/
    *mode_port = local_port;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_port_pbmp_arm_get
 * Purpose:
 *      Get port pbmp for ARM core
 * Parameters:
 *      unit    - (IN) Device Number
 *      bcm_pbmp_t - (OUT) arm pbmp
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_port_pbmp_arm_get(int unit, bcm_pbmp_t *arm_pbmp)
{
    bcm_port_t port_i;
    int core;
    bcm_pbmp_t cmic_pbmp;
    uint32 tm_port;
    uint32 base_queue_pair;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_PBMP_CLEAR(*arm_pbmp);
    cmic_pbmp = PBMP_CMIC(unit);
    PBMP_ITER(cmic_pbmp, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_queue_pair));
        
        if (base_queue_pair >= 192) {
            if (!SHR_BITGET(CPU_ARM_QUEUE_BITMAP(unit, CMC0), base_queue_pair - 192)) {
                BCM_PBMP_PORT_ADD(*arm_pbmp, port_i);
            }
        }
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_petra_port_master_set
 * Purpose:
 *     Set the Master mode on the PHY attached to the specified port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port
 *      ms - (IN) Master mode selection.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_port_master_set(int unit,
          bcm_port_t port,
          int ms)
{
    soc_port_t port_i;
    _bcm_dpp_gport_info_t   gport_info;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_master_set,(unit, port_i, ms)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_petra_port_master_get
 * Purpose:
 *     Get the Master mode on the PHY attached to the specified port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port
 *      ms - (OUT) Master mode selection.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_port_master_get(int unit,
          bcm_port_t port,
          int *ms)
{
    soc_port_t port_i;
    _bcm_dpp_gport_info_t   gport_info;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(ms);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        rv = (MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_master_get,(unit, port_i, ms)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

