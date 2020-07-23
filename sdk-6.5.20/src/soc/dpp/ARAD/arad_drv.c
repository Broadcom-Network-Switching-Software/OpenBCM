/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <soc/ipoll.h>
#include <soc/linkctrl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/fabric.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/error.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/port_map.h>
#include <soc/dpp/dpp_defs.h>
#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_stat.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/arad_link.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_stat_if.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mgmt.h>
#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/PPC/ppc_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/PORT/arad_ps_db.h>
#include <soc/shmoo_ddr40.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_sbusdma_desc.h>
#endif 

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/mbcm.h>
#include <soc/dcmn/dcmn_cmic.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif 

#include <soc/sand/sand_mem.h>
#include <soc/dcmn/dcmn_mem.h>

#ifdef INCLUDE_KNET
#include <soc/knet.h>
#endif

#include <shared/shr_resmgr.h>
#include <shared/shr_occupation.h>
#include <soc/dpp/QAX/qax_stat_if.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif


extern int
bcm_dpp_am_cosq_scheduler_allocate(int unit,
                                       int core,
                                       uint32 nof_remote_cores,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       uint8* src_modid,
                                       int *flow_id);

extern int
bcm_dpp_am_cosq_scheduler_deallocate(int unit,
                                       int core,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       int flow_id);




soc_driver_t soc_driver_acp = {
        SOC_CHIP_ACP,
        "acp",
        "",
        ACP_PCI_VENDOR_ID,
        ACP_PCI_DEVICE_ID,
        ACP_PCI_REV_ID,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
      0,
        NULL,
        NULL,
     NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        1,
        0x00000000
};  


#define SOC_DPP_ARAD_DEFAULT_TDM_SOURCE_FAP_ID_OFFSET       (256)
#define SOC_DPP_FIRST_SFI_PHY_PORT(unit)                    SOC_DPP_DEFS_GET(unit, first_sfi_phy_port)
#define SOC_DPP_ARAD_NUM_CPU_COSQ                           (64)
#define SOC_DPP_ARAD_OAM_DEFAULT_MIRROR_PROFILE             (5) 
#define SOC_DPP_IN_RIF_PROFILE_CONST_DEDICATED_BITS         (2) 



#define SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK 0x1e
#define SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_3_MASK 0xffffffff
#define SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_4_MASK 0xffffffff

#define JER_DEFAULT_NOF_MC_GROUPS (80 * 1024) 
#define JER_MAX_NOF_MC_GROUPS (128 * 1024) 
#define JER_MAX_NOF_EGRESS_MESH_MC_GROUPS (64 * 1024) 
#define QAX_DEFAULT_NOF_MC_GROUPS (16 * 1024) 
#define QAX_MAX_NOF_MC_GROUPS (SOC_IS_QUX(unit) ? (32 * 1024) : (64 * 1024)) 
#define QAX_MAX_NOF_EGRESS_MESH_MC_GROUPS (SOC_IS_QUX(unit) ? (16 * 1024): (32 * 1024)) 
#define QAX_DEFAULT_NOF_BITMAP_GROUPS (4 * 1024) 
#define QAX_MC_MAX_CUD  0x3ffff

#define QAX_COSQ_TOTAL_FLOW_REGIONS (64)
#define QUX_COSQ_TOTAL_FLOW_REGIONS (32)



#define SOC_DPP_DEFAULT_NOF_EGR_DSCP_EXP_MARKING(unit)   (SOC_IS_JERICHO_PLUS(unit) ? 16 : 4)  


enum
{
  SOC_DPP_ARAD_PCI_CMC  = 0,
  SOC_DPP_ARAD_ARM1_CMC = 1,
  SOC_DPP_ARAD_ARM2_CMC = 2,
  SOC_DPP_ARAD_NUM_CMCS = 3
};


#ifdef BCM_88660_A0
#define EXT_MODE_4LANES_PHY_PORTS ((1<<11) | (1<<12) | (1<<27) | (1<<28))
#define EXT_MODE_8LANES_PHY_PORTS (EXT_MODE_4LANES_PHY_PORTS | (1<<13) | (1<<14) | (1<<15) | (1<<16))
#endif 


#define SOC_DPP_ARAD_FABRIC_PORT_TO_PHY_PORT(unit, fabric_port) \
    (fabric_port-FABRIC_LOGICAL_PORT_BASE(unit)-SOC_DPP_DEFS_GET(unit, first_fabric_link_id)+SOC_DPP_FIRST_SFI_PHY_PORT(unit))

#define SOC_DPP_ARAD_DRAM_MODE_REG_SET(field, prop) \
      val = soc_property_port_get(unit, 0, prop, prop_invalid); \
      if (val != prop_invalid) { \
        field = val; \
        dpp_arad->init.dram.dram_conf.params_mode.params.auto_mode = FALSE; \
      }

#define SOC_MTR_BLK(unit)	\
	( SOC_IS_QAX(unit) ? SOC_BLK_IMP : SOC_BLK_MRPS )

#define SOC_DPP_VERIFIY_SSM_NOT_EXCEEDS_MAX_VALUE(ssm_value,max_val) ( (ssm_value <= max_val) ? SOC_E_NONE : SOC_E_FAIL)


int
soc_dpp_arad_str_prop_mc_tbl_mode(int unit)
{
    uint32 max_nof_egr_groups, max_nof_ingr_groups, default_nof_egr_groups, default_nof_ingr_groups;
#if defined(BCM_88660_A0)
    char *propkey, *propval;
#endif
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CONFIG(unit)->tm.mc_mode = 0;
    max_nof_egr_groups = max_nof_ingr_groups = SOC_IS_ARADPLUS_AND_BELOW(unit) ? ARAD_MULTICAST_TABLE_MODE :
      (SOC_IS_QAX(unit) ? QAX_MAX_NOF_MC_GROUPS : JER_MAX_NOF_MC_GROUPS);
    default_nof_egr_groups = default_nof_ingr_groups = SOC_IS_ARADPLUS_AND_BELOW(unit) ? ARAD_MULTICAST_TABLE_MODE :
      SOC_IS_QAX(unit) ? QAX_DEFAULT_NOF_MC_GROUPS :
      (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1 ? JER_DEFAULT_NOF_MC_GROUPS : JER_MAX_NOF_MC_GROUPS);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOC_DPP_CONFIG(unit)->tm.nof_mc_ids = default_nof_egr_groups;
        SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids = default_nof_ingr_groups;
    } else {
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP ||
            SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH ||
            SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_BACK2BACK) {
            if (!SOC_IS_QAX(unit)) {
                max_nof_egr_groups = default_nof_egr_groups = JER_MAX_NOF_EGRESS_MESH_MC_GROUPS;
            } else {
                max_nof_egr_groups = QAX_MAX_NOF_EGRESS_MESH_MC_GROUPS;
            }
        }
        SOC_DPP_CONFIG(unit)->tm.nof_mc_ids = soc_property_get(unit,spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX, default_nof_egr_groups - 1) + 1;
        if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids > max_nof_egr_groups) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max MC group specified in %s is too big, max %u ingress groups in this mode."),
              spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX, (unsigned)max_nof_egr_groups));
        }
        SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids = soc_property_get(unit, spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX, default_nof_ingr_groups - 1) + 1;
        if (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids > max_nof_ingr_groups || SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids > max_nof_ingr_groups) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max MC group specified in %s is too big"), spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX));
        }
        if (SOC_IS_QAX(unit)) {
            soc_dpp_config_qax_t *qax = SOC_DPP_CONFIG(unit)->qax;
            SOC_DPP_CONFIG(unit)->tm.nof_mc_bitmap_ids = QAX_DEFAULT_NOF_BITMAP_GROUPS;
            if (soc_property_get(unit, spn_MULTICAST_EGRESS_BITMAP_RESERVE, 1)) {
                qax->nof_egress_bitmaps = 4096;
            } else {
                qax->nof_egress_bitmaps = 0;
            }
            qax->nof_ingress_bitmaps = 0;
            qax->nof_egress_bitmap_bytes = qax->nof_ingress_bitmap_bytes = 4;
            if (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + SOC_DPP_CONFIG(unit)->tm.nof_mc_ids +
              qax->nof_ingress_bitmaps * qax->nof_ingress_bitmap_bytes +
              qax->nof_egress_bitmaps * qax->nof_egress_bitmap_bytes > QAX_NOF_MCDB_ENTRIES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("number of MC groups and bitmaps is too big to git in the device")));
            }
        } else {
            if(
                SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores *
                (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high - 1) > JER_NOF_MCDB_ENTRIES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("(number of ingress MCIDs) + (number of active cores) * ((number of egress MCIDs) - (number of egress direct bitmap MCIDs)) > 256K")));
            }
        }
    }

    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_0;
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = SOC_DPP_DEFS_GET(unit, nof_queues_per_pipe);
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_0_NOF_SYSPORT;
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD;
    SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud = ARAD_MC_16B_MAX_EGR_CUD;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && !SOC_IS_JERICHO(unit) && !SOC_DPP_PP_ENABLE(unit)) {
        propkey = spn_MULTICAST_DESTINATION_ENCODING;
        if ((propval = soc_property_get_str(unit, propkey))) {
            if (sal_strcmp(propval, "16B_CUD_96K_FLOWS_32K_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
            } else if (sal_strcmp(propval, "17B_CUD_64K_FLOWS_32K_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_1;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_1_MAX_QUEUE;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_1_NOF_SYSPORT;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_1_MAX_ING_CUD;
            } else if (sal_strcmp(propval, "18B_CUD_32K_FLOWS_16K_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_2;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_2_MAX_QUEUE;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_2_NOF_SYSPORT;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD;
            } else if (sal_strcmp(propval, "17B_CUD_96K_FLOWS_0_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_3;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_3_MAX_QUEUE;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_3_NOF_SYSPORT;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_3_MAX_ING_CUD;

            } else {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_EGR_17B_CUDS_127_PORTS_MODE | DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud = ARAD_MC_17B_MAX_EGR_CUD;
                if (sal_strcmp(propval, "16B_CUD_96K_FLOWS_32K_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                } else if (sal_strcmp(propval, "17B_CUD_64K_FLOWS_32K_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_1;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_1_MAX_QUEUE;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_1_NOF_SYSPORT;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_1_MAX_ING_CUD;
                } else if (sal_strcmp(propval, "18B_CUD_32K_FLOWS_16K_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_2;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_2_MAX_QUEUE;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_2_NOF_SYSPORT;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD;
                } else if (sal_strcmp(propval, "17B_CUD_96K_FLOWS_0_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_3;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_3_MAX_QUEUE;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_3_NOF_SYSPORT;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_3_MAX_ING_CUD;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s\n\r"), propval, propkey));
                }
            }
        }
    } else
#endif 
    if (SOC_IS_JERICHO(unit)) {
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP) {
            SOC_DPP_CONFIG(unit)->tm.mc_mode |= DPP_MC_EGR_CORE_MESH_MODE; 
        } else if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH ||
                   SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_BACK2BACK) {
            SOC_DPP_CONFIG(unit)->tm.mc_mode |= DPP_MC_EGR_CORE_MESH_MODE | DPP_MC_EGR_CORE_FDA_MODE; 
        } else if (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_links_to_core_mapping_mode == SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED &&
                   SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
            SOC_DPP_CONFIG(unit)->tm.mc_mode |= DPP_MC_EGR_CORE_FDA_MODE; 
        }
        if (SOC_IS_QAX(unit)) {
            SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud = QAX_MC_MAX_CUD;
        } else {
            SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = JER_MC_MAX_ING_CUD;
            SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud = JER_MC_MAX_EGR_CUD;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_q_pair_channel_mapping_get(int unit)
{
  int interface_i;
  int queue_i, core;
  int is_uc;
  char *s, *prefix, *tmp_s;
  int prefix_len, id;
  int nof_ilkn_ports;

  SOCDNX_INIT_FUNC_DEFS;

  nof_ilkn_ports = SOC_DPP_DEFS_GET(unit, nof_interlaken_ports);
  
  for(interface_i = 0; interface_i < nof_ilkn_ports; interface_i++)
  {
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][0]=0;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][1]=1;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][2]=4;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][3]=0;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][4]=5;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][5]=0;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][6]=0;
    SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][7]=0;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][0]=0;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][1]=0;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][2]=2;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][3]=3;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][4]=0;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][5]=0;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][6]=0;
    SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][7]=0;
  }


  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      prefix = "ILKN";
      prefix_len = sal_strlen(prefix);

      for(queue_i = 0; queue_i < nof_ilkn_ports * ARAD_Q_PAIRS_ILKN * 2; queue_i++)
      {
        is_uc = (queue_i < 16 ? 1 : 0);

        s = soc_property_port_suffix_num_get_str(unit, queue_i, core , spn_EGRESS_QUEUE, "core");

        if(s != NULL)
        {
          if (!sal_strncasecmp(s, prefix, prefix_len)) 
          {
            
            s += prefix_len;
            interface_i = sal_ctoi(s, &tmp_s);
              s += 2;
              id = sal_ctoi(s, &tmp_s);
              if(is_uc)
              {
                SOC_DPP_CONFIG(unit)->tm.uc_q_pair2channel_id[interface_i][queue_i % ARAD_Q_PAIRS_ILKN] = id;
              }
              else
              {
                SOC_DPP_CONFIG(unit)->tm.mc_q_pair2channel_id[interface_i][queue_i % ARAD_Q_PAIRS_ILKN] = id;
              }
          }
          else
          {
              
              SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: queue pair to channel mapping: illegal interface type, only ILKN is allowed")));
          }
        }
      }
  } 

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_default_config_tm_get(int unit)
{
    int i, core;
    int def_cal_len = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports = SOC_DPP_DEFS_GET(unit, nof_interlaken_ports);
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; i++) {
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_type[i] = SOC_TMC_FC_CAL_INB_TYPE_ILKN;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[i] = SOC_DPP_FC_CAL_MODE_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max);
        def_cal_len = (SOC_IS_JERICHO(unit) ? SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max) : (SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)-1));
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_TX] = def_cal_len;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[i] = SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[i] = SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_channel_mub_enable_mask[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_DPP_FC_INBAND_INTLKN_CHANNEL_MUB_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_channel_mub_enable_mask[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_DPP_FC_INBAND_INTLKN_CHANNEL_MUB_DISABLE;
    }

    SOC_DPP_CONFIG(unit)->tm.max_oob_ports = (SOC_IS_QAX(unit) ? 1 : 2);
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.max_oob_ports; i++) {
        SOC_DPP_CONFIG(unit)->tm.fc_oob_type[i] = SOC_TMC_FC_CAL_TYPE_NONE;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[i] = SOC_DPP_FC_CAL_MODE_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_LEN_MAX;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_LEN_MAX;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_ilkn_indication_invert[i][SOC_TMC_CONNECTION_DIRECTION_RX] = 0;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_ilkn_indication_invert[i][SOC_TMC_CONNECTION_DIRECTION_TX] = 0;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_spi_indication_invert[i] = 0;
    }

    SOC_DPP_CONFIG(unit)->tm.max_ses = SOC_TMC_SCH_MAX_SE_ID_ARAD + 1;
    SOC_DPP_CONFIG(unit)->tm.cl_se_min = SOC_TMC_CL_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.cl_se_max = SOC_TMC_CL_SE_ID_MAX_ARAD;
    SOC_DPP_CONFIG(unit)->tm.fq_se_min = SOC_TMC_FQ_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.fq_se_max = SOC_TMC_FQ_SE_ID_MAX_ARAD;
    SOC_DPP_CONFIG(unit)->tm.hr_se_min = SOC_TMC_HR_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.hr_se_max = SOC_TMC_HR_SE_ID_MAX_ARAD;
    SOC_DPP_CONFIG(unit)->tm.port_hr_se_min = SOC_TMC_HR_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.port_hr_se_max = SOC_TMC_HR_SE_ID_MIN_ARAD + SOC_TMC_SCH_MAX_PORT_ID_ARAD;

    SOC_DPP_CONFIG(unit)->tm.max_connectors = SOC_TMC_SCH_MAX_FLOW_ID_ARAD + 1;
    SOC_DPP_CONFIG(unit)->tm.max_egr_q_prio = SOC_TMC_EGR_NOF_Q_PRIO_ARAD;
    SOC_DPP_CONFIG(unit)->tm.invalid_port_id_num = SOC_TMC_SCH_PORT_ID_INVALID_ARAD;
    SOC_DPP_CONFIG(unit)->tm.invalid_se_id_num = SOC_TMC_SCH_SE_ID_INVALID_ARAD;
    SOC_DPP_CONFIG(unit)->tm.invalid_voq_connector_id_num = SOC_TMC_SCH_FLOW_ID_INVALID_ARAD;
    SOC_DPP_CONFIG(unit)->tm.nof_vsq_category = SOC_TMC_ITM_VSQ_GROUP_LAST_ARAD;
    SOC_DPP_CONFIG(unit)->tm.is_port_tc_enable = TRUE;

    
    
    for(core=0 ; core<SOC_DPP_DEFS_MAX(NOF_CORES) ; core++) {
        SOC_DPP_CONFIG(unit)->tm.hr_isq[core] = SOC_TMC_SCH_PORT_ID_INVALID_ARAD; 
        for (i = 0; i < SOC_TMC_MULT_FABRIC_NOF_CLS; i++) {
            SOC_DPP_CONFIG(unit)->tm.hr_fmqs[core][i] = SOC_TMC_SCH_PORT_ID_INVALID_ARAD; 
        }
    }

    SOC_DPP_CONFIG(unit)->tm.fc_calendar_coe_mode = SOC_TMC_FC_COE_MODE_PAUSE;
    SOC_DPP_CONFIG(unit)->tm.fc_calendar_pause_resolution = 1;
    SOC_DPP_CONFIG(unit)->tm.fc_calendar_e2e_status_of_entries = SOC_TMC_FC_E2E_STATUS_SIZE_8B;
    SOC_DPP_CONFIG(unit)->tm.fc_calendar_indication_invert = 0;

    SOC_DPP_CONFIG(unit)->tm.fc_coe_mac_address[0] = 0x01;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_mac_address[1] = 0x80;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_mac_address[2] = 0xC2;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_mac_address[3] = 0x00;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_mac_address[4] = 0x00;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_mac_address[5] = 0x01;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_ethertype = 0x8808;
    SOC_DPP_CONFIG(unit)->tm.fc_coe_data_offset = 2;

    SOCDNX_IF_ERR_EXIT(soc_arad_q_pair_channel_mapping_get(unit));

    SOC_DPP_CONFIG(unit)->tm.queue_level_interface_enable = soc_property_get(unit, spn_QUEUE_LEVEL_INTERFACE, 0);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_tdm_get(int unit)
{
    int bypass,packet;
    char *propkey, *propval;
    SOCDNX_INIT_FUNC_DEFS;


    propkey = spn_FAP_TDM_BYPASS;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "TDM_OPTIMIZED") == 0) {
            bypass = 1;
        } else if (sal_strcmp(propval, "TDM_STANDARD") == 0) {
            bypass = 1;
        } else if (sal_strcmp(propval, "0") == 0) {
            bypass = 0;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        bypass = 0;
    }

    propkey = spn_FAP_TDM_PACKET;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "1") == 0) {
            packet = 1;
        } else if (sal_strcmp(propval, "0") == 0) {
            packet = 0;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        packet = 0;
    }

    SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits = 48;
    SOC_DPP_CONFIG(unit)->tdm.min_cell_size = 65;
    SOC_DPP_CONFIG(unit)->tdm.max_cell_size = 256;
    SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size = 0;
    SOC_DPP_CONFIG(unit)->tdm.is_bypass = bypass;
    SOC_DPP_CONFIG(unit)->tdm.is_packet = packet;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_qos_get(int unit)
{
    soc_dpp_config_qos_t *dpp_qos;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_qos = &(SOC_DPP_CONFIG(unit))->qos;

    dpp_qos->nof_ing_elsp = 1;
    dpp_qos->nof_ing_lif_cos = 64;
    dpp_qos->nof_ing_pcp_vlan = 16;
	dpp_qos->nof_ing_dp =4;
    dpp_qos->nof_egr_remark_id = 16;
    dpp_qos->nof_egr_pcp_vlan = 16;
    dpp_qos->nof_egr_l2_i_tag = 1;
    dpp_qos->nof_ing_cos_opcode = 7;
    dpp_qos->nof_egr_mpls_php = 4;
    dpp_qos->mpls_elsp_label_range_min = soc_property_get(unit, spn_MPLS_ELSP_LABEL_RANGE_MIN, -1);
    dpp_qos->mpls_elsp_label_range_max = soc_property_get(unit, spn_MPLS_ELSP_LABEL_RANGE_MAX, -1);
    dpp_qos->egr_pcp_vlan_dscp_exp_profile_id = -1;
    dpp_qos->egr_pcp_vlan_dscp_exp_enable = 1;
    dpp_qos->egr_remark_encap_enable = 1;
    dpp_qos->dp_max = 4;
    dpp_qos->ecn_mpls_one_bit_mode = (soc_property_get(unit, spn_MPLS_ECN_MODE, 2) == 1);
    dpp_qos->ecn_mpls_enabled = (soc_property_get(unit, spn_MPLS_ECN_MODE, 2) != 0);
    dpp_qos->ecn_ip_enabled = soc_property_get(unit, spn_IP_ECN_MODE, 1);

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_qos->nof_egr_dscp_exp_marking = SOC_DPP_DEFAULT_NOF_EGR_DSCP_EXP_MARKING(unit);
    } else {
        dpp_qos->nof_egr_dscp_exp_marking = 0;
    }
#endif 

    if ((dpp_qos->mpls_elsp_label_range_min != -1) && (dpp_qos->mpls_elsp_label_range_max != -1)) {
        if (dpp_qos->mpls_elsp_label_range_min > 0xfffff) {
            dpp_qos->mpls_elsp_label_range_min = -1;
            dpp_qos->mpls_elsp_label_range_max = -1;
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: mpls_elsp_label_range_min range 0 - 0xfffff")));
        }

        if (dpp_qos->mpls_elsp_label_range_max > 0xfffff) {
            dpp_qos->mpls_elsp_label_range_min = -1;
            dpp_qos->mpls_elsp_label_range_max = -1;
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: mpls_elsp_label_range_min range 0 - 0xfffff")));
        }

        if (dpp_qos->mpls_elsp_label_range_min > dpp_qos->mpls_elsp_label_range_max) {
            dpp_qos->mpls_elsp_label_range_min = -1;
            dpp_qos->mpls_elsp_label_range_max = -1;
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: mpls_elsp_label_range_min greater than mpls_elsp_label_range_max")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_meter_get(int unit)
{
    soc_dpp_config_meter_t *dpp_meter;
    int meter_blk, range_mode, default_range_mode, color_resolution_mode, sharing_mode;
    int meter_cores = 1;
    soc_block_types_t meter_blk_type;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_meter = &(SOC_DPP_CONFIG(unit))->meter;

    
    if (SOC_IS_JERICHO(unit) && !SOC_IS_FLAIR(unit)) {
        meter_blk = SOC_MTR_BLK(unit);
        meter_blk_type = &meter_blk;
        SOCDNX_IF_ERR_EXIT(soc_dpp_nof_block_instances(unit, meter_blk_type, &meter_cores));
        if (0 == meter_cores) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ERROR: unable to indentify number of metering cores")));
        }
    }

    dpp_meter->nof_meter_cores = meter_cores;

    
    if (2 == meter_cores && SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
        default_range_mode = 128;
    } else {
        default_range_mode = 64;
    }
    range_mode = soc_property_get(unit, spn_POLICER_INGRESS_COUNT, default_range_mode);
    if (2 == meter_cores && SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)){
        if ((range_mode != 64) && (range_mode != 128)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for policer_ingress_count\n\r"), range_mode));
        }
    } else { 
        if ((range_mode != 32) && (range_mode != 64)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for policer_ingress_count\n\r"), range_mode));
        }
    }
    dpp_meter->meter_range_mode = range_mode;

    
    sharing_mode = soc_property_get(unit, spn_POLICER_INGRESS_SHARING_MODE, 0);
    if (sharing_mode >= SOC_PPC_NOF_MTR_SHARING_MODES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for policer_ingress_sharing_mode\n\r"), sharing_mode));
    }
    dpp_meter->sharing_mode = sharing_mode;

    
    color_resolution_mode = soc_property_get(unit, spn_POLICER_COLOR_RESOLUTION_MODE, 0);
    if (color_resolution_mode >= SOC_PPC_NOF_MTR_COLOR_RESOLUTION_MODES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for policer_color_resolution_mode\n\r"), color_resolution_mode));
    }
    dpp_meter->color_resolution_mode = color_resolution_mode;

    dpp_meter->nof_meter_a = SOC_IS_QUX(unit)? 8192 : 32768;
    dpp_meter->nof_meter_b = SOC_IS_QUX(unit)? 8192 : 32768;
    dpp_meter->nof_meter_profiles = 1024;
    dpp_meter->nof_high_rate_profiles = 0;
    if (SOC_IS_JERICHO(unit)) {
        dpp_meter->policer_min_rate = 22; 
    }else{
        dpp_meter->policer_min_rate = 146; 
    }

    
    dpp_meter->meter_min_rate = SOC_SAND_DIV_ROUND(SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 1000,
            ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR * (1 << (ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP + 2)));
    dpp_meter->max_rate  = ARAD_IF_MAX_RATE_KBPS(unit); 
    dpp_meter->min_burst = 64; 
    dpp_meter->max_burst = 4161536; 
    
    dpp_meter->lr_max_burst = 4161536; 
    dpp_meter->lr_max_rate = ARAD_IF_MAX_RATE_KBPS(unit); 

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_l3_get(int unit)
{
    soc_dpp_config_l3_t *dpp_l3;
    int    i;
    uint32 ipmc_mode;
    uint32 ipmc_l3mcastl2_mode, ipv4_double_capacity_used;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_l3 = &(SOC_DPP_CONFIG(unit))->l3;

    dpp_l3->ip_enable = soc_property_get(unit,spn_L3_ENABLE,1);
    
    dpp_l3->max_nof_vrfs = soc_property_get(unit,spn_IPV4_NUM_VRFS, (SOC_IS_JERICHO(unit)? SOC_DPP_DEFS_GET(unit, nof_vrfs): 8));
    dpp_l3->nof_rifs = SOC_DPP_DEFS_GET((unit), nof_rifs);
    dpp_l3->nof_rif_profiles = SOC_DPP_DEFS_GET((unit), nof_rif_profiles);
    dpp_l3->actual_nof_routing_enablers_bm = soc_property_get(unit, spn_NUMBER_OF_INRIF_MAC_TERMINATION_COMBINATIONS, SOC_PPC_RIF_MAX_NOF_ROUTING_ENABLERS_BITMAPS);

    if(SOC_IS_JERICHO(unit)){
        ipmc_l3mcastl2_mode  = soc_property_get(unit, spn_IPMC_L3MCASTL2_MODE, 0);
        ipv4_double_capacity_used = (soc_property_get(unit, spn_EXT_IP4_DOUBLE_CAPACITY_FWD_TABLE_SIZE, 0) == 0) ? 0 : 1;

        if((ipmc_l3mcastl2_mode == 1) && (ipv4_double_capacity_used == 1)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("SOC properties ext_ip4_double_capacity_fwd_table_size and ipmc_l3mcastl2_mode are mutually exclusive.")));
        } else {
            
            dpp_l3->nof_dedicated_in_rif_profile_bits = SOC_DPP_IN_RIF_PROFILE_CONST_DEDICATED_BITS + ipmc_l3mcastl2_mode + ipv4_double_capacity_used;
        }

        if((dpp_l3->actual_nof_routing_enablers_bm > 16) || (dpp_l3->actual_nof_routing_enablers_bm < 1)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Maximum INRIF_MAC_TERMINATION_COMBINATIONS is 16 Minimum is 1.")));
        } else if(dpp_l3->actual_nof_routing_enablers_bm > 8 && dpp_l3->nof_dedicated_in_rif_profile_bits == 3) {
            
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Maximum INRIF_MAC_TERMINATION_COMBINATIONS is 8 when the  ipmc_l3mcastl2_mode/ext_ip4_double_capacity_fwd_table_size SOC is enabled .")));
        }

        dpp_l3->bit_offset_of_the_routing_enablers_profile = 0;

        for (i = SOC_DPP_NOF_IN_RIF_PROFILE_BITS - dpp_l3->nof_dedicated_in_rif_profile_bits - 1; i >= 0; i--) {
            if (dpp_l3->actual_nof_routing_enablers_bm > (1 << i) ) {
                
                dpp_l3->bit_offset_of_the_routing_enablers_profile = SOC_DPP_NOF_IN_RIF_PROFILE_BITS - dpp_l3->nof_dedicated_in_rif_profile_bits - i -1;
                break;
            }
        }

        dpp_l3->intf_class_max_value = (1 << dpp_l3->bit_offset_of_the_routing_enablers_profile) - 1;

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_ivl", 0)) {
            if (!dpp_l3->intf_class_max_value) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("in this mode, at least one bit should be allocated for intf_class in Inrif profile!")));
            }

            if (!ipmc_l3mcastl2_mode) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("assumed l3mcastl2_mode is ON")));
            }
        }

        if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
            if (!dpp_l3->intf_class_max_value) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("at least one bit should be allocated for intf_class in Inrif profile for IPv6 MC SSM!")));
            }
        }
    }

    dpp_l3->ecmp_max_paths = 512;
    dpp_l3->eep_db_size = 65536;
    dpp_l3->fec_db_size = SOC_DPP_DEFS_GET(unit, nof_fecs);
    if(!soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_without_rif_support",0)){
      dpp_l3->fec_ecmp_reserved = SOC_DPP_DEFS_GET(unit, nof_ecmps); 
    }
    else {
      dpp_l3->fec_ecmp_reserved = 0;
    }

    dpp_l3->vrrp_max_vid = soc_property_get(unit, spn_L3_VRRP_MAX_VID, 4096);
    dpp_l3->vrrp_ipv6_distinct = soc_property_get(unit, spn_L3_VRRP_IPV6_DISTINCT, 0);
    ipmc_mode = soc_property_get(unit, spn_IPMC_PIM_MODE, 0x1);
    
    if (ipmc_mode & 2) {
        dpp_l3->nof_rps = 256;
        dpp_l3->nof_bidir_vrfs = 512;
    }
    else{
        dpp_l3->nof_rps = 0;
        dpp_l3->nof_bidir_vrfs = 0;
    }
    dpp_l3->ipmc_vpn_lookup_enable = soc_property_get(unit, spn_IPMC_VPN_LOOKUP_ENABLE, 0x1);
    if ((dpp_l3->ipmc_vpn_lookup_enable != 0x1) && (SOC_IS_JERICHO(unit)) && 
        (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ipmc_vpn_lookup_enable SOC property must be set to 1 on this device.")));
    }

#ifdef BCM_88660_A0
    
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_l3->multiple_mymac_enabled = soc_property_get(unit, spn_L3_MULTIPLE_MYMAC_TERMINATION_ENABLE, 0);
        dpp_l3->multiple_mymac_mode    = soc_property_get(unit, spn_L3_MULTIPLE_MYMAC_TERMINATION_MODE, 0); ;

        if (dpp_l3->multiple_mymac_mode > 1 && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Maximum multiple mymac mode for aradplus is 1.")));
        }

        
        if (SOC_IS_JERICHO(unit) || dpp_l3->vrrp_max_vid != 256) {

            if (dpp_l3->vrrp_max_vid == 0 && dpp_l3->multiple_mymac_enabled) {
                
                dpp_l3->multiple_mymac_enabled = 2;
            }
            dpp_l3->vrrp_max_vid = 4096;

            
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                dpp_l3->multiple_mymac_mode = (dpp_l3->multiple_mymac_mode || dpp_l3->vrrp_ipv6_distinct) ? TRUE : FALSE;
                dpp_l3->vrrp_ipv6_distinct = dpp_l3->multiple_mymac_mode;
            }
        }

        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && dpp_l3->vrrp_max_vid == 256 && dpp_l3->multiple_mymac_enabled) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Can't use VRRP with max vid 256 and mutliple mymac.")));
        }
    }

#endif 
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_general_get(int unit)
{
    soc_dpp_config_pp_general_t *dpp_general;
    soc_dpp_config_pp_t *dpp_pp;
    uint8 vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_DEFAULT;
    uint8 pon_tcam_enable = 0;
    char *propkey, *propval;
    uint8 prop_value;
    uint32 soc_sand_rv;
    char   mac_string[SOC_SAND_PP_MAC_ADDRESS_STRING_LEN];
    uint32 value;
    uint32 user_header_ndx;

    SOCDNX_INIT_FUNC_DEFS

    dpp_general = &(SOC_DPP_CONFIG(unit))->dpp_general;
    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    propkey = spn_VLAN_MATCH_DB_MODE;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "DEFAULT") == 0) {
            
        } else if (sal_strcmp(propval, "FULL_DB") == 0) {
            vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_FULL_DB;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_DEFAULT;
    }

    propkey = spn_VLAN_MATCH_CRITERIA_MODE;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "DEFAULT") == 0) {
            
        } else if (sal_strcmp(propval, "PCP_LOOKUP") == 0) {
            if (vlan_match_db_mode != SOC_DPP_VLAN_DB_MODE_DEFAULT) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "Unexpected property value (\"%s\") for %s while VLAN match also set\n\r"), propval, propkey));
            }
            vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_PCP;
        } else if (sal_strcmp(propval, "PON_PCP_ETHERTYPE") == 0) {
            vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_PCP;
            pon_tcam_enable = 1;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

    
    propkey = spn_L3_SOURCE_BIND_MODE;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "DISABLE") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE;
        } else if (sal_strcmp(propval, "IPV4") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_IPV4;
        } else if (sal_strcmp(propval, "IPV6") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_IPV6;
        } else if (sal_strcmp(propval, "IP") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_IP;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

    
    propkey = spn_L3_SOURCE_BIND_SUBNET_MODE;
    propval = soc_property_get_str(unit, propkey);
    if ((propval)) {
        if (sal_strcmp(propval, "DISABLE") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_DISABLE;
        } else if (sal_strcmp(propval, "IPV4") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV4;
        } else if (sal_strcmp(propval, "IPV6") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV6;
        } else if (sal_strcmp(propval, "IP") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IP;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_ERR_MSG_MODULE_NAME, unit, _SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

    
    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_source_bind_arp_relay", 0);
    if (prop_value > SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_BOTH) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_ERR_MSG_MODULE_NAME, unit, _SOCDNX_MSG("Unexpected property value (\"%hhu\") for %s"), prop_value, propkey));
    } else {
        dpp_pp->l3_src_bind_arp_relay = prop_value;
    }

    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_source_bind", 0);
    dpp_pp->compression_spoof_ip6_enable = prop_value;
    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ip6_compression_enable", 0);
    dpp_pp->compression_ip6_enable = prop_value;

    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_ivl", 0);
    dpp_pp->ipmc_ivl = prop_value;

    if (!SOC_IS_ARADPLUS(unit) && SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_OR_ARP_ENABLE(unit)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_ERR_MSG_MODULE_NAME, unit, _SOCDNX_MSG("The property value (\"%s\") for %s only working in arad_plus device"), propval, propkey));
    }

    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_ipmc", 0);
    dpp_pp->custom_pon_ipmc = prop_value;
    if (dpp_pp->custom_pon_ipmc && dpp_pp->compression_ip6_enable) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Customer PON IPMC can't working with customer IPv6 compression.")));
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
        (SOC_DPP_CONFIG(unit))->qos.nof_ing_lif_cos = 32;
    }

    
    propkey = spn_VMAC_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value == 0) {
        dpp_pp->vmac_enable = 0;
    } else {
        if(dpp_pp->l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value vmac_enable for %s"), propkey));
        }
        dpp_pp->vmac_enable = 1;
    }
    
    if (dpp_pp->vmac_enable) {
        
        soc_sand_os_memset(mac_string, 0, sizeof(mac_string));
        propkey = spn_VMAC_ENCODING_VALUE;
        propval = soc_property_get_str(unit, propkey);

        if ((propval)) {
            
            if (sal_strlen(propval) < 2 || *propval != '0' || (*(propval + 1) != 'x' && *(propval + 1) != 'X')) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }

            sal_memcpy(mac_string, (propval + 2), sal_strlen(propval));
            soc_sand_rv = soc_sand_pp_mac_address_string_parse(mac_string, &(dpp_pp->vmac_encoding_val));
            if (handle_sand_result(soc_sand_rv)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        }
        
        soc_sand_os_memset(mac_string, 0, sizeof(mac_string));
        propkey = spn_VMAC_ENCODING_MASK;
        propval = soc_property_get_str(unit, propkey);
        if ((propval)) {
            
            if (sal_strlen(propval) < 2 || *propval != '0' || (*(propval + 1) != 'x' && *(propval + 1) != 'X')) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }

            sal_memcpy(mac_string, (propval + 2), sal_strlen(propval));
            soc_sand_rv = soc_sand_pp_mac_address_string_parse(mac_string, &(dpp_pp->vmac_encoding_mask));
            if (handle_sand_result(soc_sand_rv)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        }
    }



    propkey = spn_LOCAL_SWITCHING_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value == 0) {
        dpp_pp->local_switching_enable = 0;
    } else {
        dpp_pp->local_switching_enable = 1;
    }

    if (dpp_pp->compression_ip6_enable && dpp_pp->local_switching_enable) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP compressed can't working with local switch")));
    }

    dpp_pp->ipv4_tunnel_term_bitmap_enable = 0; 
    dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_DISABLE;

    
    propkey = spn_BCM886XX_IP4_TUNNEL_TERMINATION_MODE;
    prop_value = soc_property_get(unit, propkey, 3);

    
    if (prop_value == 2) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT;
    } else if (prop_value == 1) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP;
    } else if (prop_value == 3) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT | SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP);
    } else if (prop_value == 4) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL);
    } else if (prop_value == 5) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT | SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL);
    } else if (prop_value == 6) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF);
    }

    
    if (SOC_DPP_L2TP_IS_ENABLE(unit) && prop_value != 1 && prop_value != 2) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("bcm886xx_ip4_tunnel_termination_mode 1 or 2 are supported for L2TP and L2TP is enabled.")));
    }

    
    if (!SOC_IS_JERICHO(unit) && prop_value == 6) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("bcm886xx_ip4_tunnel_termination_mode=6 available only for Jericho A0 and above.")));
    }

    
    dpp_pp->ipv4_tunnel_encap_mode = soc_property_get(unit, spn_BCM886XX_IP4_TUNNEL_ENCAPSULATION_MODE, 1);

    
    dpp_pp->ipv6_tunnel_enable = 0;
    propkey = spn_BCM886XX_IPV6_TUNNEL_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value) {
       dpp_pp->ipv6_tunnel_enable = 1;
    }

    if (dpp_pp->ipv6_tunnel_enable) {
        dpp_pp->ipv6_tunnel_encap_mode = 1;
        dpp_pp->ipv6_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT;
        if (SOC_IS_JERICHO(unit)) {
            dpp_pp->ipv6_tunnel_encap_mode = soc_property_get(unit, spn_BCM886XX_IP6_TUNNEL_ENCAPSULATION_MODE, 1);
            propkey = spn_BCM886XX_IP6_TUNNEL_TERMINATION_MODE;
            prop_value = soc_property_get(unit, propkey, 1);
            
            if (prop_value == SOC_DPP_IP6_TUNNEL_TERM_MODE_NORMAL) {
                dpp_pp->ipv6_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF);
                propkey = spn_IP6_TUNNEL_TERM_IN_TCAM_VRF_NOF_BITS;
                dpp_pp->ip6_tunnel_term_in_tcam_vrf_nof_bits = soc_property_get(unit, propkey, 10);
            }
      }
    }

    
    propkey = spn_BCM886XX_L2GRE_ENABLE;
    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,propkey, 0,&value));
    prop_value = value;

    if (prop_value) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE;
        if (dpp_pp->ipv6_tunnel_enable) {
            dpp_pp->ipv6_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE;
        }
    }

    if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) ||
        (dpp_pp->ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE)) {
      
      propkey = spn_BCM886XX_L2GRE_VPN_LOOKUP_MODE;
      prop_value = soc_property_get(unit, propkey, 0);
      dpp_pp->l2gre_vpn_lookup_mode = prop_value;
    }

    
    propkey = spn_BCM886XX_VXLAN_ENABLE;
    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,propkey, 0,&value));
    prop_value = value;

    
    dpp_pp->lawful_interception_enable = SOC_IS_ARADPLUS(unit)?soc_property_get(unit, spn_LAWFUL_INTERCEPTION_ENABLE, 0):0;

    if (prop_value && dpp_pp->lawful_interception_enable) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("VxLAN and Lawful Interception both enabled")));
    }

    if (prop_value) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN;
        if (dpp_pp->ipv6_tunnel_enable) {
            dpp_pp->ipv6_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN;
        }
    }

    if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) ||
        (dpp_pp->ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)) {
      
      propkey = spn_BCM886XX_VXLAN_VPN_LOOKUP_MODE;
      prop_value = soc_property_get(unit, propkey, 0);
      dpp_pp->vxlan_vpn_lookup_mode = prop_value;
    }

    dpp_pp->vxlan_udp_dest_port= soc_property_get(unit, spn_VXLAN_UDP_DEST_PORT, ARAD_PP_VXLAN_DST_PORT);

    
    propkey = spn_8865X_IPV4_TUNNEL_DF_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->ip_tunnel_defrag_set = (prop_value != 0);
    if (SOC_IS_ARAD_A0(unit) && prop_value != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("feature not supported on this device version(\"%s\") for %s"), propval, propkey));
    }

    
    propkey = spn_BCM886XX_FCOE_SWITCH_MODE;
    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,propkey, 0,&value));
    prop_value = value;

    dpp_pp->fcoe_enable = (prop_value != 0);
    
    propkey = spn_BCM886XX_FCOE_NUM_VRF;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->fcoe_reserved_vrf = prop_value;
    if (dpp_pp->fcoe_enable && (dpp_pp->fcoe_reserved_vrf > SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("VRF used for FCF not in range of valid VRFs(\"%s\") for %s"), propval, propkey));
    }

    
    dpp_pp->l2_src_encap_enable = 0;
    if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l2_src_encap", 0)) {
        dpp_pp->l2_src_encap_enable = 1;
    }

    
    dpp_pp->limited_gtp_parsing = 0;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "limited_gtp_parsing", 0)) {
        dpp_pp->limited_gtp_parsing = 1;
    }

    
    dpp_pp->gtp_teid_exists_parsing = 0;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gtp_teid_exists_parsing", 0)) {
        dpp_pp->gtp_teid_exists_parsing = 1;
    }

    
    dpp_pp->dm_down_trap_enable_udh_2cpu = 0;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dm_down_trap_enable_udh_2cpu", 0)) {
        dpp_pp->dm_down_trap_enable_udh_2cpu = 1;
    }

    
    dpp_pp->egress_vlan_filtering = 0;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_vlan_filtering", 0)) {
        dpp_pp->egress_vlan_filtering = 1;
    }

    
    dpp_pp->pon_dscp_remarking = 0;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_dscp_remarking", 0)) {
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            _bsl_warn(_BSL_SOCDNX_MSG("Warning: pon_dscp_remarking does not support for ARAD and below, ignored"));
        }
        else {
            dpp_pp->pon_dscp_remarking = 1;
        }
    }

    
    dpp_pp->vtt_pwe_vid_search = 0;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vtt_pwe_vid_search", 0)) {
        dpp_pp->vtt_pwe_vid_search = 1;
    }

    
    dpp_pp->egress_membership_mode = soc_property_get(unit, spn_EGRESS_MEMBERSHIP_MODE,  1);

    
    propkey = spn_BCM886XX_ETHER_IP_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);

    if (prop_value) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_ETHER;
        if (dpp_pp->ipv6_tunnel_enable) {
            dpp_pp->ipv6_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_ETHER;
        }
    }

    
    if (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) {
        propkey = spn_BCM886XX_L2GRE_TUNNEL_LOOKUP_MODE;
        prop_value = soc_property_get(unit, propkey, 2);
    }

    
    if (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) {
        propkey = spn_BCM886XX_VXLAN_TUNNEL_LOOKUP_MODE;
        prop_value = soc_property_get(unit, propkey, 2);
    }

    if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN | SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE))) {
        
        if (prop_value == 1) {
            dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_SEPERATED;
        } else if (prop_value == 3) {
            dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_DIP_SIP_VRF_SEM_ONLY_AND_DEFAULT_TCAM;
        } else {
            dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_JOIN;
        }
    }

    
    if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) && (prop_value == 3)) {
        dpp_pp->vxlan_tunnel_term_in_sem_vrf_nof_bits =
            soc_property_get(unit, spn_VXLAN_TUNNEL_TERM_IN_SEM_VRF_NOF_BITS, 12);
        dpp_pp->vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits =
            soc_property_get(unit, spn_VXLAN_TUNNEL_TERM_IN_SEM_MY_VTEP_INDEX_NOF_BITS, 3);

        if (dpp_pp->vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits > SOC_DPP_DEFS_GET(unit, vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits)) {
            SOCDNX_EXIT_WITH_ERR(BCM_E_PARAM, (_BSL_SOCDNX_MSG(" number of bits for my-vtep-inde must be <= %d"),
                                               SOC_DPP_DEFS_GET(unit, vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits)));
        }

        if (dpp_pp->vxlan_tunnel_term_in_sem_vrf_nof_bits > SOC_DPP_DEFS_GET(unit, vrf_nof_bits)) {
            SOCDNX_EXIT_WITH_ERR(BCM_E_PARAM,
                                 (_BSL_SOCDNX_MSG(" number of bits for my-vtep-inde must be <= %d"),
                                    SOC_DPP_DEFS_GET(unit, vrf_nof_bits)));
        }

        if (dpp_pp->vxlan_tunnel_term_in_sem_vrf_nof_bits + dpp_pp->vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits > 15) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                 (_BSL_SOCDNX_MSG("number of bits for VRFs: %d + number of bits for my-vtep-index: %d must be <= 15"),
                                    dpp_pp->vxlan_tunnel_term_in_sem_vrf_nof_bits + dpp_pp->vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits));
        }
    }



    
    dpp_pp->ip_lif_dummy_id = soc_property_get(unit, spn_DEFAULT_LOGICAL_INTERFACE_IP_TUNNEL_OVERLAY_MC, 0x0);
    dpp_pp->ip_lif_dummy_id_vxlan = soc_property_suffix_num_get_only_suffix(unit, -1, spn_DEFAULT_LOGICAL_INTERFACE_IP_TUNNEL_OVERLAY_MC, "vxlan", 0);
    dpp_pp->ip_lif_dummy_id_gre = soc_property_suffix_num_get_only_suffix(unit, -1, spn_DEFAULT_LOGICAL_INTERFACE_IP_TUNNEL_OVERLAY_MC, "gre", 0);
    if (dpp_pp->ip_lif_dummy_id && (dpp_pp->ip_lif_dummy_id_vxlan || dpp_pp->ip_lif_dummy_id_gre)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                         (_BSL_SOCDNX_MSG("soc property \"ip_lif_dummy_id\" and \"ip_lif_dummy_id_xxx\" can not be enabled at the same time")));
    }

         
#if defined(INCLUDE_KBP) && defined(BCM_88660)
    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "unbound_uninstalled_external_tcam_entries_number", 0);
    dpp_pp->unbound_uninstalled_elk_entries_number = prop_value;
#endif

    
    propkey = spn_EVB_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->evb_enable = prop_value;

    
    propkey = spn_BCM886XX_VLAN_TRANSLATE_MODE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->vlan_translate_mode = prop_value;

    
    propkey = spn_BCM88XXX_SYSTEM_RESOURCE_MANAGEMENT;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->sys_rsrc_mgmt = prop_value;

    
    propkey = spn_EXACT_MATCH_TABLES_SHADOW_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->em_shadow_enable = prop_value;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        
        propkey = spn_BCM88660_1588_48B_STAMPING_ENABLE;
        prop_value = soc_property_get(unit, propkey, 0);
        dpp_pp->ptp_48b_stamping_enable = prop_value;
    }
#endif

    dpp_general->nof_user_define_traps = SOC_PPC_TRAP_CODE_USER_DEFINED_LAST - SOC_PPC_TRAP_CODE_USER_DEFINED_0 + 1;
    dpp_general->nof_ingress_trap_codes = 256;
    dpp_general->use_hw_trap_id_flag = soc_property_get(unit, spn_BCM886XX_RX_USE_HW_TRAP_ID, 0 );
    dpp_pp->ipmc_enable = 1;
    dpp_pp->initial_vid_enable = 1;
    dpp_pp->drop_dest = (SOC_IS_JERICHO_PLUS(unit)) ? 1: 0x7FFFF;
    dpp_pp->mpls_label_index_enable = soc_property_get(unit, spn_MPLS_TERMINATION_LABEL_INDEX_ENABLE, 0);
    dpp_pp->vlan_match_db_mode = vlan_match_db_mode;
    dpp_pp->global_lif_index_simple = soc_property_get(unit, spn_LOGICAL_PORT_L2_BRIDGE, 1);
    dpp_pp->global_lif_index_drop = soc_property_get(unit, spn_LOGICAL_PORT_DROP, SOC_PPC_AC_ID_INVALID);
    dpp_pp->global_lif_rch = dpp_pp->global_lif_index_simple + 1;
    if (dpp_pp->global_lif_index_drop == dpp_pp->global_lif_rch) {
        dpp_pp->global_lif_rch++;
    }
    dpp_pp->pon_application_enable = 0;
    dpp_pp->pon_tcam_lkup_enable = pon_tcam_enable;
    dpp_pp->pon_tls_in_tcam = soc_property_get(unit, spn_PON_TLS_DATABASE, 0);
    dpp_pp->pon_custom_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_special_double_tag_lookup", 0);
    dpp_pp->mim_vsi_mode = (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) ? 1 : 0;
    dpp_pp->next_hop_mac_extension_enable = soc_property_get(unit, spn_BCM886XX_NEXT_HOP_MAC_EXTENSION_ENABLE, 1); 
    dpp_pp->mpls_eli_enable = soc_property_get(unit, spn_MPLS_ENTROPY_LABEL_INDICATOR_ENABLE, 1); 
    dpp_pp->dsa_tag_custom_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dsa_tag_support", 0);
    dpp_pp->mpls_termination_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2);

    if (SOC_IS_ARADPLUS_A0(unit)) {
        dpp_pp->oam_classifier_advanced_mode = soc_property_get(unit, spn_OAM_CLASSIFIER_ADVANCED_MODE, 1);
    } else {
        if (soc_property_get(unit, spn_OAM_CLASSIFIER_ADVANCED_MODE, 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("soc_dpp_info_config: oam_classifier_advanced_mode can be set only on ARAD+ device.")));
        } else {
            dpp_pp->oam_classifier_advanced_mode = 0;
        }
    }

    dpp_pp->oam_mip_level_filtering = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mip_level_filtering", 0);
    if (dpp_pp->oam_classifier_advanced_mode!=2 && dpp_pp->oam_mip_level_filtering ) {
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("soc property \"custom_feature_oam_mip_level_filtering\" May only be set when \"oam_classifier_advanced_mode\" is set to 2.")));
    }

    
    dpp_pp->oam_use_event_fifo_dma = soc_property_get(unit, spn_OAMP_FIFO_DMA_ENABLE, 0);
    if (dpp_pp->oam_use_event_fifo_dma) {
        if (SOC_IS_ARADPLUS_A0(unit)) {
            
            dpp_pp->oam_dma_event_threshold =  soc_property_get(unit, spn_OAMP_FIFO_DMA_THRESHOLD, 0x20);
            dpp_pp->oam_dma_event_threshold = (dpp_pp->oam_dma_event_threshold > ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)?
                                              ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: dpp_pp->oam_dma_event_threshold;
            dpp_pp->oam_dma_event_timeout = soc_property_get(unit, spn_OAMP_FIFO_DMA_TIMEOUT, 0);
            dpp_pp->oam_dma_event_buffer_size = soc_property_get(unit, spn_OAMP_FIFO_DMA_BUFFER_SIZE, 0);
        }
        
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("soc property \"OAMP_FIFO_DMA_ENABLE\" may only be set on Arad+")));
        }
    }
    else {
        
        dpp_pp->oam_use_event_fifo_dma = soc_property_get(unit, spn_OAMP_FIFO_DMA_EVENT_INTERFACE_ENABLE, dpp_pp->oam_use_event_fifo_dma);
        dpp_pp->oam_dma_event_threshold =  soc_property_get(unit, spn_OAMP_FIFO_DMA_EVENT_INTERFACE_THRESHOLD, 0x20);
        dpp_pp->oam_dma_event_threshold = (dpp_pp->oam_dma_event_threshold > ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)?
                                        ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: dpp_pp->oam_dma_event_threshold;
        dpp_pp->oam_dma_event_timeout = soc_property_get(unit, spn_OAMP_FIFO_DMA_EVENT_INTERFACE_TIMEOUT, 0);
        dpp_pp->oam_dma_event_buffer_size = soc_property_get(unit, spn_OAMP_FIFO_DMA_EVENT_INTERFACE_BUFFER_SIZE, 0);
    }
    dpp_pp->oam_use_report_fifo_dma = soc_property_get(unit, spn_OAMP_FIFO_DMA_REPORT_INTERFACE_ENABLE, 0);
    if (!SOC_IS_JERICHO(unit) && dpp_pp->oam_use_report_fifo_dma) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                         (_BSL_SOCDNX_MSG("soc property \"spn_OAMP_FIFO_DMA_REPORT_INTERFACE_ENABLE\" may only be set on Jericho")));
    }
    dpp_pp->oam_dma_report_threshold =  soc_property_get(unit, spn_OAMP_FIFO_DMA_REPORT_INTERFACE_THRESHOLD, dpp_pp->oam_dma_event_threshold);
    dpp_pp->oam_dma_report_threshold = (dpp_pp->oam_dma_report_threshold > ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)?
                                    ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: dpp_pp->oam_dma_report_threshold;
    dpp_pp->oam_dma_report_timeout = soc_property_get(unit, spn_OAMP_FIFO_DMA_REPORT_INTERFACE_TIMEOUT, dpp_pp->oam_dma_event_timeout);
    dpp_pp->oam_dma_report_buffer_size = soc_property_get(unit, spn_OAMP_FIFO_DMA_REPORT_INTERFACE_BUFFER_SIZE, dpp_pp->oam_dma_event_buffer_size);
    

    dpp_pp->oam_ccm_rx_wo_tx = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_ccm_rx_wo_tx", 0);
    if ((dpp_pp->oam_ccm_rx_wo_tx != 0) && (SOC_IS_QAX(unit))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                         (_BSL_SOCDNX_MSG("soc property \"custom_feature_oam_ccm_rx_wo_tx\" is not supported on this device")));
    }

    if (SOC_IS_QAX(unit)) {
        dpp_pp->oam_hierarchical_lm = soc_property_get(unit, spn_OAM_HIERARCHICAL_LOSS_MEASUREMENT_BY_MDL_ENABLE, 0);
    }

    dpp_pp->oam_use_double_outlif_injection = soc_property_get(unit, spn_OAM_USE_DOUBLE_OUTLIF_INJECTION, 0);
    if (!SOC_IS_QAX(unit) && dpp_pp->oam_use_double_outlif_injection) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                         (_BSL_SOCDNX_MSG("soc property \"spn_OAM_USE_DOUBLE_OUTLIF_INJECTION\" may only be set on QAX")));
    }

    propkey = spn_MICRO_BFD_SUPPORT_MODE;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "NONE") == 0) {
             dpp_pp->micro_bfd_support = SOC_DPP_ARAD_MICRO_BFD_SUPPORT_NONE;
        } else if (sal_strcmp(propval, "IPv4") == 0) {
            dpp_pp->micro_bfd_support = SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4;
        } else if (sal_strcmp(propval, "IPv6") == 0) {
            dpp_pp->micro_bfd_support = SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv6;
        } else if (sal_strcmp(propval, "IPv4_AND_IPv6") == 0) {
            dpp_pp->micro_bfd_support = SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s. Only NONE, IPv4, IPv6, IPv4_AND_IPv6 supported"), propval, propkey));
        }
    } else {
        dpp_pp->micro_bfd_support = SOC_DPP_ARAD_MICRO_BFD_SUPPORT_NONE;
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && dpp_pp->micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_NONE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") unsupported for this device."), propval));
    }

    dpp_pp->bfd_supported_flags_bitfield  = soc_property_get(unit, spn_BFD_SUPPORTED_FLAGS_BITFIELD, 0x32);
    dpp_pp->bfd_masked_flags_bitfield     = soc_property_get(unit, spn_BFD_MASK_FLAGS_BITFIELD, 0x0);
    if ((dpp_pp->bfd_supported_flags_bitfield != 0x32)&&
        (dpp_pp->bfd_supported_flags_bitfield != 0x38)){
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported property value. 0x%x is not supported for BFD_SUPPORTED_FLAGS_BITFIELD."),dpp_pp->bfd_supported_flags_bitfield));
    }
    if ((dpp_pp->bfd_masked_flags_bitfield != 0x8)&&
        (dpp_pp->bfd_masked_flags_bitfield != 0x2)&&
        (dpp_pp->bfd_masked_flags_bitfield != 0x0)){
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported property value. 0x%x is not supported for BFD_MASK_FLAGS_BITFIELD."),dpp_pp->bfd_masked_flags_bitfield));
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_clear_enable", 0)!=0) {
       SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported property. soc property BFD_MASK_FLAGS_BITFIELD should be used and should be set to 0x8 (C)")));
    }
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_server_enable", 0) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,
                             (_BSL_SOCDNX_MSG("OAMP server is Unsupported with maid48")));
    }
    if (soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE_EGRESS, 0)) {

	    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("oam_default_profile_egress may not be set.")));
	}

    dpp_pp->oam_default_egress_prof_num_bits = soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE_EGRESS, 0);
    dpp_pp->oam_pcp_egress_prof_num_bits = soc_property_get(unit, spn_OAM_PCP_MODE, 1);

    
    if (SOC_IS_QUX(unit)) {
        
        dpp_pp->qux_slm_lm_mode = soc_property_get(unit, spn_OAM_SLM_LM_MODE, 0);

        
        dpp_pp->qux_dm_1588_counting_enable = 0;
        dpp_pp->qux_dm_ntp_counting_enable = 0;
        dpp_pp->qux_lb_counting_enable = 0;

        
        dpp_pp->qux_ccm_counting_enable = soc_property_get(unit, spn_OAM_CCM_COUNTING_ENABLE, 1);
    }

    if (!SOC_IS_JERICHO_B0_AND_ABOVE(unit) && dpp_pp->oam_default_egress_prof_num_bits ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported property value. oam_default_profile_egress available only for Jericho B0 and above.")));
    }
    if (dpp_pp->oam_default_egress_prof_num_bits>2) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("oam_default_profile_egress may be 2 at most.")));
    }
    if (dpp_pp->oam_pcp_egress_prof_num_bits>2) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("oam_pcp_egress_prof_num_bits may be 2 at most.")));
    }

    dpp_pp->upmep_lbm_is_configured = 0;
    if (SOC_DPP_DEFS_GET(unit, nof_cores) == 2) {
        dpp_pp->oam_rcy_port_up_core_0 = soc_property_port_get(unit, 0, spn_OAM_RCY_PORT_UP, -1);
        dpp_pp->oam_rcy_port_up_core_1 = soc_property_port_get(unit, 1, spn_OAM_RCY_PORT_UP, -1);
        if ((dpp_pp->oam_rcy_port_up_core_0 != -1) && (dpp_pp->oam_rcy_port_up_core_1 != -1)) {
            dpp_pp->upmep_lbm_is_configured = 1;
        }
    }else{
        dpp_pp->oam_rcy_port_up = soc_property_get(unit, spn_OAM_RCY_PORT_UP, -1);
        if (dpp_pp->oam_rcy_port_up != -1){
            dpp_pp->upmep_lbm_is_configured = 1;
        }
    }

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)){
        dpp_pp->bfd_ipv4_single_hop = soc_property_get(unit, spn_BFD_IPV4_SINGLE_HOP_EXTENDED, 0 ) ;
    }
    else if (SOC_IS_ARADPLUS_A0(unit)) {
        dpp_pp->bfd_ipv4_single_hop = soc_property_get(unit, spn_BFD_IPV4_SINGLE_HOP_EXTENDED, 1 ); 
    } else {
        
        
        dpp_pp->bfd_ipv4_single_hop = 0;
        if(soc_property_get(unit, spn_BFD_IPV4_SINGLE_HOP_EXTENDED, 0)) {
            dpp_pp->bfd_ipv4_single_hop = soc_property_get(unit, spn_BFD_IPV4_SINGLE_HOP_EXTENDED, 0);
        } else {
            dpp_pp->bfd_ipv4_single_hop = (dpp_pp->micro_bfd_support == SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4) ||   (dpp_pp->micro_bfd_support == SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6);
        }
    }

    dpp_pp->bfd_echo_with_lem = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_echo_with_lem", 0) && SOC_IS_ARADPLUS_A0(unit));
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_echo_min_interval", 0)) {
        if (SOC_IS_ARADPLUS_A0(unit)){
            if(dpp_pp->bfd_ipv4_single_hop){
                dpp_pp->bfd_echo_min_interval = 1;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                        (_BSL_SOCDNX_MSG(" Set bfd_ipv4_single_hop soc property also")));
            }
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                    (_BSL_SOCDNX_MSG(" supported only in ARAD+.")));
        }
    }
    dpp_pp->bfd_mpls_lsp_support = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_lsp_support", SOC_IS_ARADPLUS_A0(unit)?1:0 ));

    dpp_pp->bfd_mpls_special_ttl_support = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_mpls_special_ttl_support", 0));
    if (dpp_pp->bfd_mpls_special_ttl_support && !SOC_IS_JERICHO_PLUS(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOCDNX_MSG("BFD over MPls with special TTL is not supported for this device.")));
    }

    
    dpp_pp->bfd_ipv6_enable = (soc_property_get(unit, spn_BFD_IPV6_ENABLE, 0) && (SOC_IS_ARADPLUS_A0(unit) || (SOC_IS_JERICHO(unit))))?SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM:
        SOC_DPP_ARAD_BFD_IPV6_SUPPORT_NONE ;
    if (dpp_pp->bfd_ipv6_enable==SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM &&   (SOC_IS_JERICHO(unit) &&
                                      (dpp_pp->micro_bfd_support ==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4 || dpp_pp->micro_bfd_support ==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_NONE) ) 
        && !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0))
        && !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0))) {
        
        dpp_pp->bfd_ipv6_enable = SOC_DPP_ARAD_BFD_IPV6_SUPPORT_CLASSIFIER;
    }

    if (soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0) && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                     (_BSL_SOCDNX_MSG(spn_BFD_ECHO_ENABLED" supported only in ARAD+.")));
    }
    if (SOC_IS_JERICHO(unit)) {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_lsp_support",0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                 (_BSL_SOCDNX_MSG("BFD LSP is not supported for this device.")));
        }
        else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mim",0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                 (_BSL_SOCDNX_MSG("OAM MIM currently is not supported for this device.")));
        }

        else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_echo_with_lem",0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                 (_BSL_SOCDNX_MSG("BFD echo with LEM currently is not supported for this device.")));
        }

        else if (soc_property_get(unit, spn_MPLS_TERMINATION_PWE_VCCV_TYPE4_MODE, 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                 (_BSL_SOCDNX_MSG(spn_MPLS_TERMINATION_PWE_VCCV_TYPE4_MODE" is not supported for this device.")));
        }
        else if (soc_property_get(unit, spn_MPLS_ENTROPY_LABEL_INDICATOR_ENABLE, 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                 (_BSL_SOCDNX_MSG(spn_MPLS_ENTROPY_LABEL_INDICATOR_ENABLE" cannot be set  on JERICHO devices.")));
        }
        else if (soc_property_get(unit, spn_FAST_REROUTE_LABELS_ENABLE, 0) && !((dpp_pp->mpls_termination_database_mode >= 20) && (dpp_pp->mpls_termination_database_mode <= 23))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                                 (_BSL_SOCDNX_MSG(spn_FAST_REROUTE_LABELS_ENABLE" cannot be set  on JERICHO devices.")));
        }
    }

    dpp_pp->add_ptch2_header = soc_property_get(unit, spn_ADD_PTCH2_HEADER, 0);
    dpp_pp->force_bridge_forwarding = soc_property_get(unit, spn_FORCE_BRIDGE_FORWARDING, 0);
    dpp_pp->bfd_extended_ipv4_src_ip = soc_property_get(unit, spn_BFD_EXTENDED_IPV4_SRC_IP, 0);
    dpp_pp->oam_statistics = (soc_property_get(unit, spn_OAM_STATISTICS_PER_MEP_ENABLED, 0));
    value = (soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0));
    if (dpp_pp->oam_statistics>0 && SOC_IS_ARAD_B1_AND_BELOW(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("OAM statistics per mep is not supported in ARAD")));
    }
    if (dpp_pp->oam_statistics>0 && value>0 && SOC_IS_JERICHO(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("OAM statistics per mep is not supported in ARAD mode")));
    }
    dpp_pp->custom_ip_route = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "spb_ip_shortcuts", 0));
    dpp_pp->flexible_qinq_enable = soc_property_get(unit, spn_VLAN_TRANSLATION_MATCH_IPV4, 0); 
    dpp_pp->ipv6_ext_header = soc_property_get(unit, spn_BCM886XX_IPV6_EXT_HDR_ENABLE, 0); 
    dpp_pp->custom_feature_vt_tst1 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vt_tst1", 0);
    dpp_pp->custom_feature_vt_tst2 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vt_tst2", 0);
    if(dpp_pp->custom_feature_vt_tst2 && !(dpp_pp->vlan_translate_mode))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("TST2 supported only in advance VLAN translate mode.")));
    }
    dpp_pp->gal_support = soc_property_get(unit, spn_MPLS_TERMINATION_PWE_VCCV_TYPE4_MODE, 0);
    dpp_pp->explicit_null_support = soc_property_get(unit, spn_MPLS_TERMINATION_EXPLICIT_NULL_LABEL_LOOKUP_MODE, 0);
    dpp_pp->explicit_null_support_lif_id = soc_property_get(unit, spn_DEFAULT_LOGICAL_INTERFACE_MPLS_TERMINATION_EXPLICIT_NULL, -1);
    dpp_pp->custom_feature_mpls_port_termination_lif_id = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "defult_logical_interface_mpls_1_port_termination", 0);
    dpp_pp->custom_feature_ip_port_termination_lif_id = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "defult_logical_interface_vxlan_or_gre_port_termination", 0);
    dpp_pp->custom_feature_ip_l3_gre_port_termination_lif_id = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "defult_logical_interface_l3_gre_port_termination", 0);
    dpp_pp->custom_feature_l2_gre_port_termination = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l2_gre_port_termination_enable", 0);
    dpp_pp->custom_feature_vxlan_port_termination = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vxlan_port_termination_enable", 0);

    dpp_pp->custom_feature_vrrp_scaling_tcam = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vrrp_scaling_tcam", 0);
    if ((dpp_pp->custom_feature_vrrp_scaling_tcam || (dpp_pp->explicit_null_support && SOC_IS_ARADPLUS_AND_BELOW(unit)) || dpp_pp->gal_support) && (!dpp_pp->custom_feature_vt_tst1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("gal and explicit null support can be enabled only when vt_tst1 is set.")));
    }
    dpp_pp->vrrp_scaling_tcam = soc_property_get(unit, spn_VRRP_SCALING_TCAM, 0);
    dpp_pp->use_small_banks_mode_vrrp = soc_property_get(unit, spn_USE_SMALL_BANKS_MODE_VRRP, 0);

    if (dpp_pp->explicit_null_support && (dpp_pp->explicit_null_support_lif_id == -1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("when explicit null is set, default_logical_interface_mpls_termination_explicit_null must be set to valid LIF-ID.")));
    }

    dpp_pp->test1 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pbb_test1_enable", 0);
    dpp_pp->test2 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "EoE_test2_enable", 0);
    dpp_pp->ingress_full_mymac_1 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ingress_full_mymac_1", 0);
    dpp_pp->egress_full_mymac_1 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_full_mymac_1", 0);
    dpp_pp->global_mldp_dummy_lif_id[0] = soc_property_get(unit,spn_DEFAULT_LOGICAL_INTERFACE_MPLS_1_LABEL_BUD_MULTICAST, -1);
    dpp_pp->global_mldp_dummy_lif_id[1] = soc_property_get(unit,spn_DEFAULT_LOGICAL_INTERFACE_MPLS_2_LABELS_BUD_MULTICAST, -1);
    if((dpp_pp->global_mldp_dummy_lif_id[0] != -1) && (dpp_pp->global_mldp_dummy_lif_id[1] != -1))
        dpp_pp->mldp_support = 1;
    else
        dpp_pp->mldp_support = 0;

    
    propkey = spn_UDP_TUNNEL_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->udp_tunnel_enable = (prop_value != 0);
    if (!SOC_IS_JERICHO_B0_AND_ABOVE(unit) && prop_value != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("feature not supported on this device version(\"%s\") for %s"), propval, propkey));
    }

    
    prop_value = (soc_property_get(unit, spn_EXTENDER_CONTROL_BRIDGE_ENABLE, 0))
                    ? soc_dpp_extender_init_status_enabled : soc_dpp_extender_init_status_off;
    if (SOC_IS_ARAD_B1_AND_BELOW(unit) && prop_value) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port extender supported only in ARAD+ and above.")));
    }

    SOC_DPP_CONFIG(unit)->extender.port_extender_init_status = prop_value;

    SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vn_tag_port_termination_enable", 0);
    if (SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination ==  1) {
        SOC_DPP_CONFIG(unit)->extender.port_extender_init_status = soc_dpp_extender_init_status_enabled;
        if (prop_value) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port extender can't coexist with VN-TAG port termination.")));
        }

    }


    if (SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing ==  1) {
        if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing ==  1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG(" gtp_teid_exists_parsing can't coexist with limited_gtp_parsing.")));
        }
        if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG(" gtp_teid_exists_parsing can't coexist with FCoE.")));
        }
        if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG(" gtp_teid_exists_parsing can't coexist with udp_tunnel_enable.")));
        }
    }
    

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_pp->router_over_ac = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "router_over_ac", 0);
    }
#endif
    
    dpp_pp->nof_global_out_lifs = SOC_DPP_DEFS_GET(unit, nof_global_lifs);
    dpp_pp->nof_global_in_lifs = SOC_DPP_DEFS_GET(unit, nof_global_lifs);

    
    dpp_pp->trap_packet_parse_test = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "trap_packet_parse_test", 0);
    dpp_pp->parse_oamp_punt_packet = soc_property_get(unit, "custom_feature_parse_oamp_punt_packet", 0);
    dpp_pp->oam_additional_FTMH_on_error_packets = soc_property_get(unit, "custom_feature_oam_additional_FTMH_on_error_packets", 0);
    
    for (user_header_ndx = 0; user_header_ndx < 4; user_header_ndx++) {
        value = soc_property_port_get(unit, user_header_ndx, spn_FIELD_CLASS_ID_SIZE, 0);
        dpp_pp->user_header_size += value;
    }
    dpp_pp->user_header_size /= 8;

    dpp_pp->parser_mode = (soc_property_get(unit, spn_PARSER_MODE, 0)) ? 1 : 0;
    dpp_pp->eth_default_procedure_disable = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "eth_default_procedure_disable", 0));

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (dpp_pp->parser_mode == 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("advance parser (parser_mode=1) supported only in Jericho and above.")));
    }

    dpp_pp->ipmc_lookup_model = soc_property_get(unit, spn_IPMC_LOOKUP_MODEL, 0) ? 1 : 0;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (dpp_pp->ipmc_lookup_model == 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("new ipmc lookup model (ipmc_lookup_model=1) supported only in Jericho and above.")));
    }

    dpp_pp->oamp_flexible_da = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oamp_flexible_da", 0);

    if (!(SOC_IS_QAX(unit)) && (dpp_pp->oamp_flexible_da != 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("custom_feature_oamp_flexible_da supported only in QAX and above.\n")));
    }

    if ((SOC_IS_QAX(unit)) && (dpp_pp->oamp_flexible_da != 0)) {
        
        if ((soc_property_port_get(unit, 0, spn_FIELD_CLASS_ID_SIZE, 0))
         || (soc_property_port_get(unit, 1, spn_FIELD_CLASS_ID_SIZE, 0))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error: UDH should not be enabled when custom feature for flexible DA is enabled.\n")));
        }
        
        if (dpp_pp->oam_statistics) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error: OAM statistics per MEP should not be enabled when custom feature for flexible DA is enabled.\n")));
        }
    }

    dpp_pp->bfd_ipv4_flex_iptos = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_ipv4_flex_iptos", 0);
    if ((!SOC_IS_JERICHO(unit)) && (dpp_pp->bfd_ipv4_flex_iptos != 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("custom_feature_bfd_ipv4_flex_iptos supported only in JERICHO and above.\n")));
    }

    dpp_pp->bfd_sbfd_enable = soc_property_get(unit, spn_SEAMLESS_BFD_ENABLE, 0);
    if ((!SOC_IS_JERICHO(unit)) && (dpp_pp->bfd_sbfd_enable != 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("seamless_bfd_enable property supported only in JERICHO and above.\n")));
    }

    if ((dpp_pp->bfd_sbfd_enable !=0) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0) == 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("seamless_bfd_enable property enabled. custom_feature_multi_hop_bfd_extra_dip_support needs to be set for Classical BFD Multi hop to work.\n")));
    }

    if (SOC_IS_JERICHO(unit) && (dpp_pp->bfd_sbfd_enable != 0)) {
        dpp_pp->sbfd_rcy_port = soc_property_get(unit, spn_SBFD_RCY_PORT, -1);
        if (dpp_pp->sbfd_rcy_port != -1) {
            dpp_pp->sbfd_reflector_is_configured = 1;
        }
    }

    dpp_pp->out_ac_ttl_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "out_ac_ttl_enable", 0);
    dpp_pp->mim_bsa_lsb_from_ssp = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_bsa_lsb_from_ssp", 0);

    dpp_pp->ipv6_compressed_sip_enable = SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit);
    if (!SOC_IS_JERICHO(unit) && (dpp_pp->ipv6_compressed_sip_enable == 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("IPv6 SSM in KAPS supported only in Jericho and above.")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_l2_get(int unit)
{
    soc_dpp_config_l2_t *dpp_l2;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_l2 = &(SOC_DPP_CONFIG(unit))->l2;

    dpp_l2->nof_vsis = SOC_PPC_VSI_MAX_NOF_ARAD;
    if (SOC_IS_QUX(unit)) {
        dpp_l2->nof_vsis = SOC_DPP_DEFS_GET(unit, nof_vsi_lowers);
    }
    dpp_l2->nof_lifs = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
    dpp_l2->mac_size = SOC_DPP_DEFS_GET(unit, nof_lem_lines);
    dpp_l2->mac_nof_event_handlers = 4;


    dpp_l2->learn_limit_mode = 0;
    dpp_l2->learn_limit_lif_range_base[0] = 0;
    dpp_l2->learn_limit_lif_range_base[1] = 0;

    if (SOC_IS_ARADPLUS(unit)) {
        dpp_l2->learn_limit_mode = SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI;
        dpp_l2->learn_limit_lif_range_base[0] = 0;
        dpp_l2->learn_limit_lif_range_base[1] = 0x4000;
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_trill_get(int unit)
{
    soc_dpp_config_trill_t *dpp_trill;
    uint8                   trill_mode;
    uint8                   trill_transparent_service;
    soc_port_t              port_i;
    int                     lif_id;
    uint32 value;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_trill = &(SOC_DPP_CONFIG(unit))->trill;
    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_TRILL_MODE, 0,&value));
    trill_mode = (uint8)value;
    switch (trill_mode) {
        case 0:
            dpp_trill->mode =  SOC_PPD_TRILL_MODE_DISABLED;
            break;
        case 1:
            dpp_trill->mode =  SOC_PPD_TRILL_MODE_VL;
            break;
        case 2:
            dpp_trill->mode = SOC_PPD_TRILL_MODE_FGL;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "soc_dpp_get_default_config_trill: Invalid trill mode \n")));
            break;
    }

    
    if (SOC_IS_JERICHO(unit) && (dpp_trill->mode ==  SOC_PPD_TRILL_MODE_VL)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                              (BSL_META_U(unit,
                                          "VL trill mode is not supported in Jerich\n")));
        return SOC_E_UNAVAIL;
    }


    dpp_trill->mc_prune_mode = soc_property_get(unit,spn_TRILL_MC_PRUNE_MODE, 0);
   
    dpp_trill->mc_id=0;
    
    dpp_trill->create_ecmp_port=0;

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i)  {
        lif_id = soc_property_port_suffix_num_get(unit, port_i, -1, spn_CUSTOM_FEATURE, "trill_designated_vlan_inlif", -1);
        if (lif_id != -1) {
            dpp_trill->designated_vlan_inlif_enable = 1;
            break;
        }
    }

    trill_transparent_service = soc_property_get(unit,spn_TRILL_TRANSPARENT_SERVICE, 0);
    if ((dpp_trill->mode != SOC_PPD_TRILL_MODE_FGL) && (trill_transparent_service != 0)){
        LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "soc_dpp_get_default_config_trill: Invalid trill transparent service \n")));
    } else {
        dpp_trill->transparent_service= trill_transparent_service;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_oam_bfd_get(int unit)
{
    soc_dpp_config_pp_t     *pp_config;
    uint8                   oam_enable;
    uint8                   bfd_enable;
    uint8                   oamp_ports;
    uint32 value;

    SOCDNX_INIT_FUNC_DEFS

    pp_config = &(SOC_DPP_CONFIG(unit))->pp;

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_BFD_ENABLE,0,&value));
    bfd_enable = value;
    pp_config->bfd_enable = (bfd_enable != 0);

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_OAM_ENABLE,0,&value));
    oam_enable = value;
    pp_config->oam_enable = (oam_enable != 0);

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_NUM_OAMP_PORTS,0,&value));
    oamp_ports = value;
    pp_config->oamp_enable = (oamp_ports != 0);

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_OAM_TRAP_STRENGTH_PASSIVE,0,&value));
    pp_config->oam_trap_strength_passive = value;

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_OAM_TRAP_STRENGTH_LEVEL,0,&value));
    pp_config->oam_trap_strength_level = value;

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_OAM_TRAP_STRENGTH_INJECTED,0,&value));
    pp_config->oam_trap_strength_injected = value;

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_OAM_DEFAULT_TRAP_STRENGTH,0,&value));
    pp_config->oam_default_trap_strength = value;

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_OAM_FORWARD_TRAP_STRENGTH,0,&value));
    pp_config->oam_forward_trap_strength = value;

    if((pp_config->user_header_size > 0) && (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0))
        && (SOC_IS_QUX(unit) && pp_config->qux_slm_lm_mode)) {
        
        LOG_ERROR(BSL_LS_SOC_INIT,
                              (BSL_META_U(unit,
                                          "SLM can't work with 11B MAID if UDH enabled\n")));
        return SOC_E_UNAVAIL;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_get(int unit, soc_dpp_config_arad_t *cfg)
{
  int rv = SOC_E_NONE;

  SOCDNX_INIT_FUNC_DEFS

  sal_memset(cfg, 0, sizeof (soc_dpp_config_arad_t));

  
  arad_ARAD_MGMT_INIT_clear(&(cfg->init));
  cfg->voq_mapping_mode = VOQ_MAPPING_INDIRECT;
  cfg->action_type_source_mode = ACTION_TYPE_FROM_QUEUE_SIGNATURE;
  cfg->hqos_mapping_enable = FALSE;

  ARAD_PP_MGMT_OPERATION_MODE_clear(&cfg->pp_op_mode);

  
  rv = soc_arad_default_config_tm_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_tdm_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_qos_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_meter_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_l3_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_general_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_l2_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_trill_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_default_config_oam_bfd_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_reset_cmicm_regs(int unit)
{
    uint32 rval, divisor, dividend, mdio_delay;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_ARDON(unit)) {

        
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit,   0x00000005));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit,  0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x11000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x00001111));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x00000002));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x00000020));
    } else {

        

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x02222227));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x11111122));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x33111111));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00011211));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x22222000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x11111111));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x32333332));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x10110012));
    }

    
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 24);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, dividend);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, 24);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_EXT_MDIOr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_EXT_MDIOr, &rval, DIVIDENDf, dividend);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RATE_ADJUST_EXT_MDIOr(unit, rval));

    
    rval = 0;
    mdio_delay = 0xf;
    soc_reg_field_set(unit, CMIC_MIIM_CONFIGr, &rval, MDIO_OUT_DELAYf, mdio_delay);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_CONFIGr(unit, rval));

    

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, 0x75E7E0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, 0x65A6DC));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, 0x5565D8));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit, 0x4524D4));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit, 0x24A2CC));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit, 0x1461C8));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, 0x0420C4));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, 0x34E3D0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, 0x000000));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, 0x000000));

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CTRLr(unit, 0xba));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_info_config_device_ports(int unit)
{
    soc_info_t          *si;

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    si->num_time_interface = 1;

    SOCDNX_FUNC_RETURN;

}

int
soc_arad_core_frequency_config_get(int unit, int dflt_freq_khz, uint32 *freq_khz)
{
    uint32 value;

    SOCDNX_INIT_FUNC_DEFS;

    if (soc_property_get_str(unit, spn_CORE_CLOCK_SPEED) != NULL) {
        
        SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit, spn_CORE_CLOCK_SPEED, dflt_freq_khz / 1000, &value));
        *freq_khz = value * 1000;

    } else {
        
        SOCDNX_IF_ERR_EXIT(dcmn_property_suffix_num_get(unit, 0,  spn_CORE_CLOCK_SPEED, "khz", dflt_freq_khz, &value));
        *freq_khz = value;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_schan_timeout_config_get(int unit, int *schan_timeout)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SAL_BOOT_QUICKTURN) {
        *schan_timeout = SCHAN_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
        *schan_timeout = SCHAN_TIMEOUT_PLI;
    } else {
        *schan_timeout = SCHAN_TIMEOUT;
    }

    *schan_timeout = soc_property_get(unit, spn_SCHAN_TIMEOUT_USEC, *schan_timeout);

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_cmic_sbus_timeout_set(int unit)
{
    soc_control_t        *soc;
    int rv = SOC_E_NONE;
    uint32 frequency, ticks,
           max_uint = 0xFFFFFFFF,
           max_ticks= 0x3FFFFF;

    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

     
    
    rv = soc_arad_core_frequency_config_get(unit, 600000, &(SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_arad_schan_timeout_config_get(unit, &(SOC_CONTROL(unit)->schanTimeout));
    SOCDNX_IF_ERR_EXIT(rv);


    frequency = (SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency) / 1000;

    if ((max_uint / frequency) > soc->schanTimeout) { 
        ticks = frequency * soc->schanTimeout;
        ticks = ((ticks / 100) * 75); 
    } else {
        ticks = max_ticks;
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, ticks));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_hard_reset(int unit, int reset_action)
{
    uint32
        rv,
        reg32_val = 0;
    soc_timeout_t
        to;

    SOCDNX_INIT_FUNC_DEFS;


    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CPS_RESETr(unit, 0x1));

        soc_timeout_init(&to, 100000, 100);
        for(;;) {
            SOCDNX_IF_ERR_EXIT(READ_CMIC_CPS_RESETr(unit, &reg32_val));
#ifdef PLISIM
            if (SAL_BOOT_PLISIM) {
                reg32_val = 0x0;
            }
#endif 
            if (reg32_val == 0x0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                SOCDNX_EXIT_WITH_ERR(_SHR_E_INIT, (_BSL_SOCDNX_MSG("Error: CPS reset field not asserted correctly.")));
                break;
            }
        }
    }

    
    soc_endian_config(unit);

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        rv = soc_arad_reset_cmicm_regs(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_arad_cmic_sbus_timeout_set(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }


exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_system_ftmh_load_balancing_ext_mode_get(int unit, ARAD_MGMT_FTMH_LB_EXT_MODE *system_ftmh_load_balancing_ext_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYSTEM_FTMH_LOAD_BALANCING_EXT_MODE;
    propval = soc_property_get_str(unit, propkey);

    if ((!propval) || (sal_strcmp(propval, "DISABLED") == 0)) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED;
    } else if (sal_strcmp(propval, "8B_LB_KEY_8B_STACKING_ROUTE_HISTORY") == 0) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY;
    } else if (sal_strcmp(propval, "16B_STACKING_ROUTE_HISTORY") == 0) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY;
    } else if (sal_strcmp(propval, "ENABLED") == 0) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_ENABLED;
    }
#ifdef BCM_88660_A0
    else if (sal_strcmp(propval, "STANDBY_MC_LB") == 0) {
        if(SOC_IS_ARADPLUS(unit)) {
            *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device doesn't support property  (\"%s\") for %s"), propval, propkey));
        }
    } else if (sal_strcmp(propval, "FULL_HASH") == 0) {
        if(SOC_IS_ARADPLUS(unit)) {
            *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device doesn't support property  (\"%s\") for %s"), propval, propkey));
        }
    }
#endif
     else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_system_trunk_hash_format_get(int unit, ARAD_MGMT_TRUNK_HASH_FORMAT *system_trunk_hash_format)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_TRUNK_HASH_FORMAT;
    propval = soc_property_get_str(unit, propkey);

    if ((!propval) || (sal_strcmp(propval, "NORMAL") == 0)) {
        *system_trunk_hash_format = ARAD_MGMT_TRUNK_HASH_FORMAT_NORMAL;
    }
#ifdef BCM_88660_A0
    else if (SOC_IS_ARADPLUS(unit )){
        if(sal_strcmp(propval, "INVERTED")== 0) {
            *system_trunk_hash_format = ARAD_MGMT_TRUNK_HASH_FORMAT_INVERTED;
        } else if(sal_strcmp(propval, "DUPLICATED")== 0) {
            *system_trunk_hash_format = ARAD_MGMT_TRUNK_HASH_FORMAT_DUPLICATED;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }
#endif
     else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device doesn't support property  (\"%s\") for %s"), propval, propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_validate_fap_device_mode(int unit, uint8 pp_enable, ARAD_MGMT_TDM_MODE tdm_mode)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {
        case BCM88654_DEVICE_ID:
        case BCM88664_DEVICE_ID:
            if(pp_enable != 0 ||
               (tdm_mode != ARAD_MGMT_TDM_MODE_TDM_OPT && tdm_mode != ARAD_MGMT_TDM_MODE_TDM_STA)) {
                SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
            }
             break;
        default:
             break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_prop_fap_device_mode_get(int unit, uint8 *pp_enable, SOC_TMC_MGMT_TDM_MODE *tdm_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FAP_DEVICE_MODE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "TM") == 0) {
            *pp_enable = FALSE;
        } else if (sal_strcmp(propval, "PP") == 0) {
            *pp_enable = TRUE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

    propkey = spn_FAP_TDM_BYPASS;
    propval = soc_property_get_str(unit, propkey);

    if ((propval)) {
        if (sal_strcmp(propval, "TDM_OPTIMIZED") == 0) {
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_TDM_OPT;
        } else if (sal_strcmp(propval, "TDM_STANDARD") == 0) {
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_TDM_STA;
        } else if (sal_strcmp(propval, "0") == 0) {
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_PACKET;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *tdm_mode = SOC_TMC_MGMT_TDM_MODE_PACKET;
    }

    if(soc_arad_validate_fap_device_mode(unit, *pp_enable, *tdm_mode)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("fap_device_mode configuration doesn't supported")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_mc_nbr_full_dbuff_get(int unit, ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_MODES *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_MULTICAST_NBR_FULL_DBUFF;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE") == 0) {
            *p_val = ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE;
        } else if (sal_strcmp(propval, "ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE") == 0) {
            *p_val = ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *p_val = ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_report_mode_get(int unit, int core_id, SOC_TMC_SIF_CORE_MODE core_mode, SOC_TMC_STAT_IF_REPORT_MODE *stat_if_report_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_MODE;
    propval = soc_property_suffix_num_str_get(unit, core_id, propkey, "core");
    *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_NOT_CONFIGURE;

    if (propval) {
        if (sal_strcmp(propval, "BILLING") == 0) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING;
        }
        else if (sal_strcmp(propval, "QSIZE") == 0) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_QSIZE;
        }
#ifdef BCM_88650_B0
        else if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && (sal_strcmp(propval, "BILLING_QUEUE_NUMBER") == 0)) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER;
        }
#endif
        else if (SOC_IS_JERICHO(unit) && (sal_strcmp(propval, "BILLING_INGRESS")==0) ) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_billing_pkt_size_get(int unit, SOC_TMC_STAT_IF_PKT_SIZE *stat_if_pkt_size)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_PKT_SIZE;
    propval = soc_property_get_str(unit, propkey);
    *stat_if_pkt_size = SOC_TMC_NOF_STAT_IF_PKT_SIZES;

    if (propval) {
        if (sal_strcmp(propval, "64B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_65B;
        } else if (sal_strcmp(propval, "128B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_126B;
        } else if (sal_strcmp(propval, "256B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_248B;
        } else if (sal_strcmp(propval, "512B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_492B;
        } else if (sal_strcmp(propval, "1024B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_1024B;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_qsize_pkt_size_get(int unit, SOC_TMC_STAT_IF_PKT_SIZE *stat_if_pkt_size)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_PKT_SIZE;
    propval = soc_property_get_str(unit, propkey);
    *stat_if_pkt_size = SOC_TMC_NOF_STAT_IF_PKT_SIZES;

    if (propval) {
        if (sal_strcmp(propval, "65B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_65B;
        } else if (sal_strcmp(propval, "126B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_126B;
        } else if (sal_strcmp(propval, "248B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_248B;
        } else if (sal_strcmp(propval, "492B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_492B;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_core_mode_get(int unit, SOC_TMC_SIF_CORE_MODE *stat_if_core_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_CORE_MODE;
    propval = soc_property_get_str(unit, propkey);
    *stat_if_core_mode = SOC_SIF_CORE_NOF_MODES;

    if (propval) {
        if (sal_strcmp(propval, "DEDICATED") == 0) {
            *stat_if_core_mode = SOC_TMC_SIF_CORE_MODE_DEDICATED;
        }
        else if (sal_strcmp(propval, "COMBINED") == 0) {
            *stat_if_core_mode = SOC_TMC_SIF_CORE_MODE_COMBINED;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("When working with statistics interface, core mode must be configured \n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_prop_stat_if_reports_per_packet_get(int unit, int core_id, SOC_TMC_STAT_IF_NOF_STAT_REPORTS_PER_PKT *nof_reports_per_pkt)
{
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;
    propval = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_REPORTS_PER_PACKET, "core", -1);

    switch (propval)
    {
        case 8:
            *nof_reports_per_pkt = SOC_TMC_STAT_IF_NOF_REPORTS_8;
            break;
        case 16:
            *nof_reports_per_pkt = SOC_TMC_STAT_IF_NOF_REPORTS_16;
            break;
        case 32:
            *nof_reports_per_pkt = SOC_TMC_STAT_IF_NOF_REPORTS_32;
            break;
        case 64:
            *nof_reports_per_pkt = SOC_TMC_STAT_IF_NOF_REPORTS_64;
            break;
        case 128:
            *nof_reports_per_pkt = SOC_TMC_STAT_IF_NOF_REPORTS_128;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Invalid configuration %d for number of reports per packet"), propval));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_prop_stat_if_report_size_get(int unit, int core_id, uint8 *report_size)
{
    char *propval, *propkey;
    SOCDNX_INIT_FUNC_DEFS;
    propkey = spn_STAT_IF_REPORT_SIZE;
    propval = soc_property_suffix_num_str_get(unit, core_id, spn_STAT_IF_REPORT_SIZE, "core");
    if (propval)
    {
        if (sal_strcmp(propval, "61b") == 0)
        {
            *report_size = SOC_TMC_STAT_IF_REPORT_SIZE_61b;
        }
        else if (sal_strcmp(propval, "64b") == 0)
        {
            *report_size = SOC_TMC_STAT_IF_REPORT_SIZE_64b;
        }
        else
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("When working in statistics interface queue size mode, report size must be configured\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_ucode_port(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID *p_val, uint32 *channel, uint32* is_hg)
{
    int found;
    uint32 local_chan;
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len, id;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_UCODE_PORT;
    propval = soc_property_port_get_str(unit, port, propkey);

    if(is_hg) {
        (*is_hg) = 0;
    }

    s = propval;
    found = 0;
    *channel=0;

    
    if (propval) {

        
        if (!found) {
            prefix = "IGNORE";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = SOC_TMC_NIF_ID_NONE;
                found = 1;
            }
        }

        if (!found) {
            prefix = "XAUI";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(XAUI,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "RXAUI";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(RXAUI,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "SGMII";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(SGMII,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "ILKN";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(ILKN,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "10GBase-R";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(10GBASE_R,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "XLGE";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(XLGE,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "CGE";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }
                *p_val = ARAD_NIF_ID(CGE,id);
                found = 1;
                s = ss;
            }
        }


        if (!found) {
            prefix = "CPU";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_CPU;
                found = 1;
            }
        }

        if (!found) {
            prefix = "RCY";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_RCY;
                found = 1;
            }
        }

        if (!found) {
            prefix = "TM_INTERNAL_PKT";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {

                if (!SOC_IS_ARDON(unit)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("TM_INTERNAL_PKT Interface is Enabled only for Ardon device. property value (\"%s\") for %s"), propval, propkey));
                }

                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_TM_INTERNAL_PKT;
                found = 1;
            }
        }

        if (!found) {
            prefix = "RESERVED";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {

                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_RESERVED;
                found = 1;
            }
        }
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *p_val = SOC_TMC_NIF_ID_NONE;
    }


    if (s && (*s == '.')) {
        
        ++s;
        local_chan = sal_ctoi(s, &ss);
        if (s != ss) {
            *channel = local_chan;
        }
        s = ss;
    }

    while (s && (*s == ':')) {
        ++s;

        
        prefix = "hg";
        prefix_len = sal_strlen(prefix);

        if (!sal_strncasecmp(s, prefix, prefix_len)) {
            s += sal_strlen(prefix);
            if(is_hg) {
                (*is_hg) = 1;
            }
        } else {
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_ucode_port_erp_lane(int unit, int *lane)
{
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_UCODE_PORT;
    propval = soc_property_suffix_num_only_suffix_str_get(unit, 0, propkey, "erp");

    s = propval;

    
    if (propval) {
        prefix = "ERP";
        prefix_len = sal_strlen(prefix);

        if (!sal_strncasecmp(s, prefix, prefix_len)) {
            s += sal_strlen(prefix);
            *lane = sal_ctoi(s, &ss);
            if (s == ss) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
            }
            s = ss;
        }
    } else {
        
        *lane = -1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_pon_port(int unit, soc_port_t port, uint32* is_pon)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_PON_APPLICATION_SUPPORT_ENABLED;
    propval = soc_property_port_num_get_str(unit, port, propkey);

    SOCDNX_NULL_CHECK(is_pon);
    (*is_pon) = 0;

    if (propval) {
        if (sal_strcmp(propval, "TRUE") == 0) {
            (*is_pon) = 1;
        }
        else {
            (*is_pon) = 0;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_init_vid_enabled_port_get(int unit, soc_port_t port, uint32 *init_vid_enabled)
{
    char  *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(init_vid_enabled);

    propkey = spn_VLAN_TRANSLATION_INITIAL_VLAN_ENABLE;
    propval = soc_property_port_get(unit, port, propkey, 1);

    if (propval) {
        (*init_vid_enabled) = 1;
    }
    else {
        (*init_vid_enabled) = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_mact_learn_limit_mode(int unit, uint8 *l2_learn_limit_mode)
{
    char *prop_key, *prop_str;

    SOCDNX_INIT_FUNC_DEFS;

    
    *l2_learn_limit_mode = SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI;

    
    prop_key = spn_L2_LEARN_LIMIT_MODE;
    prop_str = soc_property_get_str(unit, prop_key);
    if (prop_str != NULL) {
        if ((sal_strcmp(prop_str, "VLAN") == 0) || (sal_strcmp(prop_str, "0") == 0)) {
            *l2_learn_limit_mode = SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI;
        } else if ((sal_strcmp(prop_str, "VLAN_PORT") == 0) || (sal_strcmp(prop_str, "1") == 0)) {
            *l2_learn_limit_mode = SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF;
        } else if ((sal_strcmp(prop_str, "TUNNEL") == 0) || (sal_strcmp(prop_str, "2") == 0)) {
            *l2_learn_limit_mode = SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_TUNNEL;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), prop_str, prop_key));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_str_prop_parse_mact_learn_limit_range(int unit, int32 *used_lif_range)
{
    char *prop_key, *prop_str;
    uint32  lif_range_ix;

    SOCDNX_INIT_FUNC_DEFS;

    
    used_lif_range[0] = 0;
    used_lif_range[1] = 0x4000;

    
    prop_key = spn_L2_LEARN_LIF_RANGE_BASE;
    for (lif_range_ix = 0; lif_range_ix < SOC_DPP_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES; lif_range_ix++) {
        prop_str = soc_property_port_get_str(unit, lif_range_ix, prop_key);
        if (prop_str != NULL) {
            if (sal_strcmp(prop_str, "0") == 0) {
                used_lif_range[lif_range_ix] = 0;
            } else if (sal_strcmp(prop_str, "16K") == 0) {
                used_lif_range[lif_range_ix] = 0x4000;
            } else if (sal_strcmp(prop_str, "32K") == 0) {
                used_lif_range[lif_range_ix] = 0x8000;
            } else if (sal_strcmp(prop_str, "48K") == 0) {
                used_lif_range[lif_range_ix] = 0xC000;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), prop_str, prop_key));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_validate_fabric_mode(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {
        case BCM88350_DEVICE_ID:
        case BCM88351_DEVICE_ID:
        case BCM88360_DEVICE_ID:
        case BCM88361_DEVICE_ID:
        case BCM88363_DEVICE_ID:
            if(*fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP) { 
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "device support only fabric_connect_mode - SINGLE_FAP mode. fabric connect mode SINGLE_FAP enforced.\n")));
                *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP;
            }
            break;

        case BCM88450_DEVICE_ID:
        case BCM88451_DEVICE_ID:
        case BCM88460_DEVICE_ID:
        case BCM88461_DEVICE_ID:
            if(*fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_MESH &&
               *fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP &&
               *fabric_connect_mode !=SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK) {

                *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_MESH;
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "device support only fabric_connect_mode - MESH mode (or BACK2BACK). fabric connect mode MESH enforced.\n")));

             }
             break;

        default:
            break;
    }


    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_ilkn_counters_mode_get(int unit, uint32 *ilkn_counters_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_ILKN_COUNTERS_MODE;
    propval = soc_property_get_str(unit, propkey);
    if (propval) {
        if (sal_strcmp(propval, "STAT_PER_PHYSICAL_PORT") == 0) {
            *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_physical;
        } else if (sal_strcmp(propval, "PACKET_PER_CHANNEL") == 0) {
            *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_packets_per_channel;
        }
    } else {
        
        *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_physical;
    }

    if (SOC_IS_JERICHO(unit)) {
        *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_packets_per_channel;
    }
#if defined(INCLUDE_KBP) && defined(BCM_88660)
    if(SOC_IS_ARADPLUS(unit) && (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode) &&
       (*ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn counter mode packet per channel is not supported while ext_interface_mode_enabled")));
    }
#endif

#if defined(INCLUDE_KBP) && defined(BCM_88660)
exit:
#endif
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_lag_mode_get(int unit, SOC_TMC_PORT_LAG_MODE *lag_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_NUMBER_OF_TRUNKS;
    propval = soc_property_get_str(unit, propkey);
    if (SOC_IS_QUX(unit)) {
        if (propval == NULL) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_32_64;
        } else if (sal_strcmp(propval, "32") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_32_64;
        } else if (sal_strcmp(propval, "16") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_16_128;
        } else if (sal_strcmp(propval, "8") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_8_256;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else if (propval) {
        if (sal_strcmp(propval, "1024") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_1K_16;
        } else if (sal_strcmp(propval, "512") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_512_32;
        } else if (sal_strcmp(propval, "256") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_256_64;
        } else if (sal_strcmp(propval, "128") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_128_128;
        } else if (sal_strcmp(propval, "64") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_64_256;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *lag_mode = SOC_TMC_PORT_LAG_MODE_1K_16;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_swap_info_get(int unit, ARAD_SWAP_INFO *swap_info)
{
    char *propkey;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(swap_info, 0, sizeof (*swap_info));

    
    propkey = spn_PREPEND_TAG_OFFSET;
    swap_info->offset = soc_property_get(unit, propkey, 0x0);

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        char *propval;

        
        propkey = spn_PREPEND_TAG_BYTES;
        propval = soc_property_get_str(unit, propkey);

        
        if (propval) {
            if (sal_strcmp(propval, "4B") == 0) {
                swap_info->mode = SOC_TMC_SWAP_MODE_4_BYTES;
            } else if (sal_strcmp(propval, "8B") == 0) {
                swap_info->mode = SOC_TMC_SWAP_MODE_8_BYTES;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        } else {
            swap_info->mode = SOC_TMC_SWAP_MODE_4_BYTES;
        }

        
        if (SOC_DPP_CONFIG(unit)->extender.port_extender_init_status && !(SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination)) {
            if (swap_info->mode != SOC_TMC_SWAP_MODE_8_BYTES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("If using port extender, prepend_tag_bytes must be 8B.")));
            }
            if (swap_info->offset) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("If using port extender, prepend_tag_offset must be 0.")));
            }
        }
    }
#endif

#ifdef BCM_88660_A0
exit:
#endif
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_synce_mode_get(int unit, ARAD_NIF_SYNCE_MODE *mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYNC_ETH_MODE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "TWO_DIFF_CLK") == 0) {
            *mode = ARAD_NIF_SYNCE_MODE_TWO_DIFF_CLK;
        } else if (sal_strcmp(propval, "TWO_CLK_AND_VALID") == 0) {
            *mode = ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *mode = ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_synce_clk_div_get(int unit, int ind, ARAD_NIF_SYNCE_CLK_DIV *clk_div)
{
    int propval;

    SOCDNX_INIT_FUNC_DEFS;

    propval = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_DIVIDER, "clk_", 0x0);

    if (propval) {
        if (propval == 1) {
            *clk_div = ARAD_NIF_SYNCE_CLK_DIV_1;
        } else if (propval == 2) {
            *clk_div = ARAD_NIF_SYNCE_CLK_DIV_2;
        } else if (propval == 4) {
            *clk_div = ARAD_NIF_SYNCE_CLK_DIV_4;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for %s"), propval, spn_SYNC_ETH_CLK_DIVIDER));
        }
    } else {
        *clk_div = ARAD_NIF_SYNCE_CLK_DIV_1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
STATIC int
soc_arad_str_prop_nif_elk_tcam_dev_type_get(int unit, ARAD_NIF_ELK_TCAM_DEV_TYPE *tcam_dev_type)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_TCAM_DEV_TYPE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "NONE") == 0) {
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE;
        } else if ((sal_strcmp(propval, "NL88650") == 0) || (sal_strcmp(propval, "NL88650A") == 0) ){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88650A;
        } else if ((sal_strcmp(propval, "NL88650B") == 0)){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88650B;
        } else if ((sal_strcmp(propval, "NL88675") == 0)){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88675;
        } else if ((sal_strcmp(propval, "BCM52311") == 0)){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52311;
        } else if ((sal_strcmp(propval, "BCM52321") == 0)){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52321;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_nif_elk_tcam_connect_mode_get(int unit, ARAD_NIF_ELK_TCAM_CONNECT_MODE *kbp_connect_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_TCAM_CONNECT_MODE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "SINGLE") == 0) {
            *kbp_connect_mode = ARAD_NIF_ELK_TCAM_CONNECT_MODE_SINGLE;
        } else if ((sal_strcmp(propval, "DUAL_SHARED") == 0)){
            *kbp_connect_mode = ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED;
        } else if ((sal_strcmp(propval, "DUAL_SMT") == 0)){
            *kbp_connect_mode = ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *kbp_connect_mode = ARAD_NIF_ELK_TCAM_CONNECT_MODE_SINGLE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_nif_elk_core_port_mapping_get(int unit)
{
    char *propkey, *propval, *sub_str, *sub_str_end;
    int kbp_port;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_TCAM_START_LANE;
    for (kbp_port = 0; kbp_port < ARAD_KBP_MAX_KBP_PORT; kbp_port ++) {
        propval = soc_property_suffix_num_str_get(unit, kbp_port, propkey, "port");
        sub_str = propval;
        if (propval == NULL) {
            elk->kbp_port_core_mapping[kbp_port] = kbp_port % 2;
            switch (kbp_port) {
                case 0:
                    elk->kbp_start_lane[kbp_port] = 0;
                    break;
                case 1:
                    elk->kbp_start_lane[kbp_port] = ARAD_KBP_OP_IS_DUAL_MODE ? 8 : 20;
                    break;
                case 2:
                    elk->kbp_start_lane[kbp_port] = 16;
                    break;
                default:
                    elk->kbp_start_lane[kbp_port] = 24;
                    break;
            }
        } else {
            
            elk->kbp_start_lane[kbp_port] = sal_ctoi(sub_str, &sub_str_end);
            sub_str = sub_str_end;
            if (*sub_str != '\0') {
                if (*sub_str != ':') {
                    SOCDNX_EXIT_WITH_ERR
                        (SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP Port %d: Bad config string \"%s\"\n"), kbp_port, propval));
                }
                
                sub_str++;
                if (sal_strcmp(sub_str, "core0") == 0) {
                    elk->kbp_port_core_mapping[kbp_port] = 0;
                } else if (sal_strcmp(sub_str, "core1") == 0) {
                    elk->kbp_port_core_mapping[kbp_port] = 1;
                } else {
                    SOCDNX_EXIT_WITH_ERR
                        (SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP Port %d: Bad core ID \"%s\"\n"), kbp_port, propval));
                }
            } else {
                
                elk->kbp_port_core_mapping[kbp_port] = kbp_port % 2;
            }
        }
        if ((kbp_port % 2 == 1) && (elk->kbp_port_core_mapping[kbp_port] == elk->kbp_port_core_mapping[kbp_port - 1])) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Cannot attach two KBP ports in the same core\n")));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_nif_elk_lut_write_mode_get(int unit, ARAD_NIF_ELK_TCAM_LUT_WRITE_MODE *lut_write_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_TCAM_LUT_WRITE_MODE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "ROP") == 0) {
            *lut_write_mode = ARAD_KBP_LUT_ROP_WRITE;
        } else if ((sal_strcmp(propval, "MDIO") == 0)){
            *lut_write_mode = ARAD_KBP_LUT_MDIO_WRITE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *lut_write_mode = ARAD_KBP_LUT_MDIO_WRITE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_str_prop_nif_elk_fwd_table_size_get(int unit, ARAD_INIT_ELK *elk)
{
    int shared_ip_mpls_tbl_size = 0, extended_ipv6_tbl_size = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0] =
        SOC_SAND_MAX(soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0), soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0));

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1] = soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0);

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0] =
        SOC_SAND_MAX(soc_property_get(unit, spn_EXT_IP4_UC_RPF_PUBLIC_FWD_TABLE_SIZE, 0x0), soc_property_get(unit, spn_EXT_IP4_PUBLIC_FWD_TABLE_SIZE, 0x0));

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1] = soc_property_get(unit, spn_EXT_IP4_UC_RPF_PUBLIC_FWD_TABLE_SIZE, 0x0);

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0] =
        SOC_SAND_MAX(soc_property_get(unit, spn_EXT_IP6_UC_RPF_FWD_TABLE_SIZE, 0x0), soc_property_get(unit, spn_EXT_IP6_FWD_TABLE_SIZE, 0x0));

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] = soc_property_get(unit, spn_EXT_IP6_UC_RPF_FWD_TABLE_SIZE, 0x0);

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0] =
        SOC_SAND_MAX(soc_property_get(unit, spn_EXT_IP6_UC_RPF_PUBLIC_FWD_TABLE_SIZE, 0x0), soc_property_get(unit, spn_EXT_IP6_PUBLIC_FWD_TABLE_SIZE, 0x0));

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1] = soc_property_get(unit, spn_EXT_IP6_UC_RPF_PUBLIC_FWD_TABLE_SIZE, 0x0);

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE] = soc_property_get(unit, spn_EXT_IP4_MC_BRIDGE_FWD_TABLE_SIZE, 0x0);

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC] = soc_property_get(unit, spn_EXT_IP4_MC_FWD_TABLE_SIZE, 0x0);

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC] = soc_property_get(unit, spn_EXT_IP6_MC_FWD_TABLE_SIZE, 0x0);

    
    if ((!SOC_IS_ARADPLUS(unit))&&(elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC] == 0)){
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] = 0;
    }

    
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_UC] = soc_property_get(unit, spn_EXT_TRILL_UC_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_MC] = soc_property_get(unit, spn_EXT_TRILL_MC_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR] = soc_property_get(unit, spn_EXT_MPLS_FWD_TABLE_SIZE, 0x0);

    
    if (SOC_IS_JERICHO(unit) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0] && !elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1]) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: When using IPv6 UC on this device it is required to use it with RPF")));
    }

    
    if (!ARAD_KBP_IS_OP_OR_OP2 && (
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0] ||
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1] ||
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0] ||
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1])
    ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: Public tables are not supported on this device")));
    }

    
    if((elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0] == 0) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0] != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: IPv4 Public Unicast table requires the initialization of IPv4 Unicast (Private) table")));
    }
    if((elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1] == 0) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1] != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: IPv4 Public Unicast RPF table requires the initialization of IPv4 Unicast (Private) RPF table")));
    }
    if((elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0] == 0) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0] != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: IPv6 Public Unicast table requires the initialization of IPv6 Unicast (Private) table")));
    }
    if((elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] == 0) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1] != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: IPv6 Public Unicast RPF table requires the initialization of IPv6 Unicast (Private) RPF table")));
    }

    if((elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1] == 0) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC]) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: IPv4 Multicast table requires the initialization of IPv4 Unicast RPF table")));
    }
    if((elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] == 0) && elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC]) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("KBP: IPv6 Multicast table requires the initialization of IPv6 Unicast RPF table")));
    }

    if (SOC_IS_ARADPLUS(unit)){
        shared_ip_mpls_tbl_size = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mpls_extended_fwd_table_size", 0);
        if(shared_ip_mpls_tbl_size != 0){
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED] = shared_ip_mpls_tbl_size;
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP] = shared_ip_mpls_tbl_size;
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR] = shared_ip_mpls_tbl_size;
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1] = shared_ip_mpls_tbl_size;
        }

        extended_ipv6_tbl_size = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv6_extended_fwd_table_size", 0);
        if(extended_ipv6_tbl_size != 0){
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6] = extended_ipv6_tbl_size;
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] = extended_ipv6_tbl_size;
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0] = 8;
        }

        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_p2p_extended_fwd_table_size", 0);

        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_inrif_mapping_table_size", 0);

        if (SOC_IS_JERICHO(unit)) {
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_DC] = soc_property_get(unit, spn_EXT_IP4_DOUBLE_CAPACITY_FWD_TABLE_SIZE, 0x0);
        }else{
            elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_DC] = 0;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_is_device_elk_disabled(int unit)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    if(driver_dev_id == BCM88654_DEVICE_ID ||
       driver_dev_id == BCM88664_DEVICE_ID) {

        return TRUE;
    }

    return FALSE;
}

#endif 

int
soc_arad_ilkn_phy_ports_btmp_set(int unit, uint32 ilkn_id, soc_pbmp_t *phy_ports)
{
    int i;
    uint32 lanes;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*phy_ports);
    lanes = soc_property_port_get(unit, ilkn_id, spn_ILKN_NUM_LANES, 0);

#if defined(INCLUDE_KBP) && defined(BCM_88660_A0)
    if(SOC_IS_ARADPLUS(unit) && (ilkn_id == 1) && (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)){
        uint32 phy_interfaces = 0;

        phy_interfaces = _SHR_PBMP_WORD_GET(*phy_ports , 0);
        switch(lanes){
            case 4:
                phy_interfaces |= EXT_MODE_4LANES_PHY_PORTS;
                break;
            case 8:
                phy_interfaces |= EXT_MODE_8LANES_PHY_PORTS;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Support just 4 or 8 lanes in ext_interface_mode")));
        }
        _SHR_PBMP_WORD_SET(*phy_ports, 0, phy_interfaces);
    }
    else
#endif 
    {
        for(i=0;i<lanes;i++) {
            if(i<12) {
                SOC_PBMP_PORT_ADD(*phy_ports, ilkn_id*16+i+1);
            } else{
                SOC_PBMP_PORT_ADD(*phy_ports, 40-i);
            }
        }
    }

#if defined(INCLUDE_KBP) && defined(BCM_88660_A0)
exit:
#endif
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_validate_device_core_frequency(int unit, int frequency)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    if( driver_dev_id == BCM88351_DEVICE_ID ||
        driver_dev_id == BCM88451_DEVICE_ID ||
        driver_dev_id == BCM88551_DEVICE_ID ||
        driver_dev_id == BCM88552_DEVICE_ID ||
        driver_dev_id == BCM88651_DEVICE_ID ||
        driver_dev_id == BCM88361_DEVICE_ID ||
        driver_dev_id == BCM88461_DEVICE_ID ||
        driver_dev_id == BCM88561_DEVICE_ID ||
        driver_dev_id == BCM88562_DEVICE_ID ||
        driver_dev_id == BCM88661_DEVICE_ID) {

        if(frequency > 500000) {
            return -1;
        }
    }

    return 0;
}

int
soc_arad_str_prop_voq_mapping_mode_get(int unit, int8 *voq_mapping_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_VOQ_MAPPING_MODE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "DIRECT") == 0) {
            *voq_mapping_mode = VOQ_MAPPING_DIRECT;
        } else if (sal_strcmp(propval, "INDIRECT") == 0) {
            *voq_mapping_mode = VOQ_MAPPING_INDIRECT;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *voq_mapping_mode = VOQ_MAPPING_INDIRECT;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_hqos_mapping_get(int unit, int8 *hqos_mapping_enable)
{

    *hqos_mapping_enable = soc_property_get(unit, spn_HQOS_MAPPING_ENABLE, 0);

    return BCM_E_NONE;
}

STATIC int
soc_arad_str_prop_action_type_source_mode_get(int unit, int8 *action_type_source_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_ACTION_TYPE_SIGNATURE_STAMPING;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "FORWARDING_ACTION") == 0) {
            *action_type_source_mode = ACTION_TYPE_FROM_FORWARDING_ACTION;
        } else if (sal_strcmp(propval, "QUEUE_SIGNATURE") == 0) {
            *action_type_source_mode = ACTION_TYPE_FROM_QUEUE_SIGNATURE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *action_type_source_mode = ACTION_TYPE_FROM_FORWARDING_ACTION;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_egress_shared_resources_mode_get(int unit, SOC_TMC_EGR_QUEUING_PARTITION_SCHEME *egress_shared_resources_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EGRESS_SHARED_RESOURCES_MODE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "Strict") == 0) {
            *egress_shared_resources_mode = SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_STRICT;
        } else if (sal_strcmp(propval, "Discrete") == 0) {
            *egress_shared_resources_mode = SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_DISCRETE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *egress_shared_resources_mode = SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_STRICT;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_credit_worth_resolution_get(int unit, ARAD_MGMT_CREDIT_WORTH_RESOLUTION *credit_worth_resolution)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_CREDIT_WORTH_RESOLUTION;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "high") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_HIGH;
        } else if (sal_strcmp(propval, "medium") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_MEDIUM;
        } else if (sal_strcmp(propval, "low") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_LOW;
        } else if (sal_strcmp(propval, "auto") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_AUTO;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_AUTO;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_external_voltage_mode_get(int unit, ARAD_MGMT_EXT_VOLT_MOD *external_voltage_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_VOLTAGE_MODE;
    propval = soc_property_suffix_num_str_get(unit, -1, spn_EXT_VOLTAGE_MODE, "oob");

    if (propval) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if (sal_strcmp(propval, "HSTL_1.5V") == 0) {
                *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V;
            } else if (sal_strcmp(propval, "3.3V") == 0) {
                *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_3p3V;
            } else if (sal_strcmp(propval, "HSTL_1.5V_VDDO_DIV_2") == 0) {
                *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V_VDDO;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        }
        else {
            if (sal_strcmp(propval, "HSTL_1.8V") == 0) {
                *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_HSTL_1p8V;
            } else if (sal_strcmp(propval, "3.3V") == 0) {
                *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_3p3V;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        }
    } else {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            *external_voltage_mode = ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V;
        }
        else {
            *external_voltage_mode = ARAD_MGMT_EXT_VOL_MOD_HSTL_1p8V;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_dpp_str_prop_xgs_compatible_system_port_mode_get(int unit, int8 *xgs_compatible_system_port_mode)
{
    uint32 propval;
    char *propkey;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_HIGIG_FRC_TM_SYSTEM_PORT_ENCODING;
    propval = soc_property_get(unit, propkey, SOC_DPP_XGS_TM_7_MODID_8_PORT);

    switch (propval) {
        case SOC_DPP_XGS_TM_7_MODID_8_PORT:
            *xgs_compatible_system_port_mode = SOC_DPP_XGS_TM_7_MODID_8_PORT;
            break;
        case SOC_DPP_XGS_TM_8_MODID_7_PORT:
            *xgs_compatible_system_port_mode = SOC_DPP_XGS_TM_8_MODID_7_PORT;
            break;
        default:
            *xgs_compatible_system_port_mode = SOC_DPP_XGS_TM_7_MODID_8_PORT;
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_is_olp(int unit, soc_port_t port, uint32* is_olp)
{
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    *is_olp = (SOC_PORT_IF_OLP == interface ? 1: 0);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_is_oamp(int unit, soc_port_t port, uint32* is_oamp)
{
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    *is_oamp = (SOC_PORT_IF_OAMP == interface ? 1: 0);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_mpls_termination_database_mode(int unit, uint8 mpls_termination_label_index_enable, uint8 mpls_termination_rif_enable, uint8 *mpls_database, SOC_PPC_MPLS_TERM_NAMESPACE_TYPE  *mpls_namespace)
{
    int default_value, mode;

    SOCDNX_INIT_FUNC_DEFS;

    if (mpls_termination_label_index_enable) {
        default_value = 2; 
    } else {
        default_value = 0; 
    }

    mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, default_value);

    
    mpls_database[0] = mpls_database[1] = mpls_database[2] = 0xff;

    
    if ((mode == 0 || mode == 1) &&
        mpls_termination_label_index_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE when mpls_termination_label_index_enable is set"), mode));
    }

    if (!mpls_termination_label_index_enable && mode > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE when mpls_termination_label_index_enable is unset"), mode));
    }

    if ((mode == 6 || mode == 7) &&
        mpls_termination_rif_enable) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE when mpls_termination_rif_enable is set"), mode));
    }

    
    if (SOC_IS_ARAD_B1_AND_BELOW(unit) && mode == 10)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE for ARAD-B and below"), mode));
    }

    
    SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only = (mode == 10)  ? 1:0;
    SOC_DPP_CONFIG(unit)->pp.tunnel_termination_ordered_by_index = (mode == 11 || mode==12)  ? 1:0;

    switch (mode) {
        case 0:
            
            mpls_database[0] =  1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            break;
        case 1:
            
            mpls_database[0] =  0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            break;
        case 2:
            
            mpls_database[0] = 1;
            mpls_database[1] = 0;
            mpls_database[2] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 3:
            
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_database[2] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 4:
            
            mpls_database[0] = 1;
            mpls_database[1] = 1;
            mpls_database[2] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 5:
            
            mpls_database[0] = 0;
            mpls_database[1] = 0;
            mpls_database[2] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 6:
            
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 7:
            
            mpls_database[0] = 1;
            mpls_database[1] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 8:
            
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            break;
        case 9:
            
            mpls_database[0] = 1;
            mpls_database[1] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            break;
        case 10:
            
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            break;
        case 11:
            
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            break;
        case 12:
            
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;

        
        case 20:
            mpls_database[0] = 1;
            mpls_database[1] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 21:
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 22:
            mpls_database[0] = 1;
            mpls_database[1] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            break;
        case 23:
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE"), mode));
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_prop_parse_dram_number_of_rows(int unit, SOC_TMC_DRAM_NUM_ROWS *p_val)
{
    char *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_ROWS;
    propval = soc_property_get(unit, propkey, SOC_TMC_NOF_DRAM_NUM_ROWS);
    switch (propval) {
        case 8192:
            *p_val = SOC_TMC_DRAM_NUM_ROWS_8192;
            break;
        case 16384:
            *p_val = SOC_TMC_DRAM_NUM_ROWS_16384;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_dram_nof_buffs_calc(int unit,
                             int dram_size_total_mbyte,
                             ARAD_ITM_DBUFF_SIZE_BYTES  dbuff_size,
                             ARAD_INIT_PDM_MODE pdm_mode,
                             int *nof_dram_buffs)
{
    int rv = SOC_E_NONE,
        max_buffs_by_dram;
    uint32 pdm_nof_entries;

    SOCDNX_INIT_FUNC_DEFS;

    rv = handle_sand_result(arad_init_pdm_nof_entries_calc(unit, pdm_mode, &pdm_nof_entries));
    SOCDNX_IF_ERR_EXIT(rv);

    max_buffs_by_dram = (dram_size_total_mbyte * 1024 * 1024)/dbuff_size;
    SOC_SAND_LIMIT_FROM_ABOVE(max_buffs_by_dram, ARAD_ITM_NOF_DRAM_BUFFS);

    *nof_dram_buffs = SOC_SAND_MIN(pdm_nof_entries, max_buffs_by_dram);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_dram_param_set(int unit, soc_dpp_config_arad_t *dpp_arad)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    if(SOC_IS_QAX(unit)) {
        dpp_arad->init.dram.dbuff_size                     = soc_property_get(unit, spn_EXT_RAM_DBUFF_SIZE, ARAD_ITM_DBUFF_SIZE_BYTES_4096);
        if (dpp_arad->init.dram.dbuff_size < ARAD_ITM_DBUFF_SIZE_BYTES_4096) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property dram buffer size is %d, the buff size of QAX shouldn't be less than 4096!"), dpp_arad->init.dram.dbuff_size));
        }
    } else {
        dpp_arad->init.dram.dbuff_size                     = soc_property_get(unit, spn_EXT_RAM_DBUFF_SIZE, ARAD_ITM_DBUFF_SIZE_BYTES_1024);
    }

    dpp_arad->init.dram.dram_size_total_mbyte          = soc_property_get(unit, spn_EXT_RAM_TOTAL_SIZE, 200);
    dpp_arad->init.dram.dram_user_buffer_size_mbytes   = soc_property_suffix_num_get(unit, -1, spn_USER_BUFFER_SIZE, "dram", 0x0);
    dpp_arad->init.dram.dram_device_buffer_size_mbytes = dpp_arad->init.dram.dram_size_total_mbyte - dpp_arad->init.dram.dram_user_buffer_size_mbytes;

    
    switch(soc_property_get(unit, spn_BCM886XX_PDM_MODE , 0))
    {
        case 0:
            dpp_arad->init.dram.pdm_mode = ARAD_INIT_PDM_MODE_SIMPLE;
            break;
        case 1:
            dpp_arad->init.dram.pdm_mode = ARAD_INIT_PDM_MODE_REDUCED;
            break;
        default:
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }

    
    rv = soc_arad_dram_nof_buffs_calc(unit,
                                           dpp_arad->init.dram.dram_device_buffer_size_mbytes,
                                           dpp_arad->init.dram.dbuff_size,
                                           dpp_arad->init.dram.pdm_mode,
                                           &dpp_arad->init.dram.nof_dram_buffers);
    SOCDNX_IF_ERR_EXIT(rv);

    
    dpp_arad->init.dram.dram_user_buffer_start_ptr = dpp_arad->init.dram.nof_dram_buffers + 1;

    rv = soc_dpp_prop_parse_ingress_congestion_management_pdm_extensions_get(unit, &SOC_DPP_CONFIG(unit)->pdm_extension);
    SOCDNX_IF_ERR_RETURN(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_otm_port_rate_type(int unit, soc_port_t port, SOC_TMC_EGR_PORT_SHAPER_MODE *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_OTM_PORT_PACKET_RATE;
    propval = soc_property_port_get_str(unit, port, propkey);

    if (propval) {
        if (sal_strcmp(propval, "DATA") == 0) {
            *p_val = SOC_TMC_EGR_PORT_SHAPER_DATA_MODE;
        } else if (sal_strcmp(propval, "PACKET") == 0) {
            *p_val = SOC_TMC_EGR_PORT_SHAPER_PACKET_MODE;
        } else  {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *p_val = SOC_TMC_EGR_PORT_SHAPER_DATA_MODE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

void
soc_arad_dma_mutex_destroy(int unit)
{
    (void)soc_sbusdma_lock_deinit(unit);
}

int
soc_arad_deinit(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    
    
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

    if (SOC_IS_JERICHO(unit)) {
        
        soc_jer_cmic_interrupts_disable(unit);
    } else {
        
        soc_cmicm_intr2_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK);
        soc_cmicm_intr3_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_3_MASK);
        soc_cmicm_intr4_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_4_MASK);

        arad_interrupts_array_deinit(unit);

        if ((!SAL_BOOT_NO_INTERRUPTS)) {
            if (soc_ipoll_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "error disconnecting polled interrupt mode\n")));
            }
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
            if (soc_ipoll_pktdma_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "error disconnecting polled Packet DMA interrupt mode\n")));
            }
#endif
            SOC_CONTROL(unit)->soc_flags &= ~SOC_F_POLLED;
              
            if (soc_cm_interrupt_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "could not disconnect interrupt line\n")));
            }
        }
    }

    rv = soc_linkctrl_deinit(unit);
    if (SOC_FAILURE(rv)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Error Running soc_linkctrl_deinit.")));
    }

    
    soc_arad_free_cache_memory(unit);

    _soc_dpp_port_map_deinit(unit); 

#if defined(BCM_WARM_BOOT_SUPPORT)
    soc_port_sw_db_destroy(unit);
#endif
    arad_tbl_access_deinit(unit);
    arad_pp_isem_access_deinit(unit);

    arad_pp_dbal_deinit(unit);
#ifdef BCM_JERICHO_SUPPORT
    jer_sbusdma_desc_deinit(unit);
#endif 

#ifdef BCM_SBUSDMA_SUPPORT
    if ((rv=soc_sbusdma_desc_detach(unit)) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_sbusdma_desc_detach returns error.")));
    }
#endif
    SOCDNX_IF_ERR_EXIT(sand_deinit_fill_table(unit));

#ifdef INCLUDE_KNET
    soc_knet_cleanup();
#endif

    
    soc_arad_dma_mutex_destroy(unit);

    rv = soc_dma_detach(unit);
    if (SOC_FAILURE(rv)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dma_detach returns error.")));
    }

    
    if (SOC_IS_JERICHO(unit)) {
        
        soc_jer_interrupts_deinit(unit);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

void soc_arad_free_cache_memory(int unit)
{
    soc_mem_t mem;
    soc_error_t rc = SOC_E_NONE;

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }
        
        rc = soc_mem_cache_set(unit, mem, COPYNO_ALL, FALSE);
        if (SOC_FAILURE(rc))
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error to deallocate cache for mem %d\n"), mem));
        }
    }

    
    if (!SOC_IS_ARDON(unit))
    {
        arad_set_tcam_cache(unit, 0);
    }
}

int
soc_arad_attach(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_DPP_CONFIG(unit)->arad = sal_alloc(sizeof (soc_dpp_config_arad_t), "arad_config");
    if (SOC_DPP_CONFIG(unit)->arad == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to alloc arad_config")));
    }
    sal_memset(SOC_DPP_CONFIG(unit)->arad, 0x0, sizeof(soc_dpp_config_arad_t));


#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        SOC_DPP_CONFIG(unit)->jer = sal_alloc(sizeof(soc_dpp_config_jer_t), "soc_dpp_config_jer_t");
        if (SOC_DPP_CONFIG(unit)->jer == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to alloc soc_dpp_config_jer_t")));
        }
        sal_memset(SOC_DPP_CONFIG(unit)->jer, 0x0, sizeof(soc_dpp_config_jer_t)); 
        if (SOC_IS_QAX(unit)) {
            SOC_DPP_CONFIG(unit)->qax = sal_alloc(sizeof(soc_dpp_config_qax_t), "soc_dpp_config_qax_t");
            if (SOC_DPP_CONFIG(unit)->qax == NULL) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to alloc soc_dpp_config_qax_t")));
            }
            sal_memset(SOC_DPP_CONFIG(unit)->qax, 0x0, sizeof(soc_dpp_config_qax_t)); 
        }
    }
#endif 

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_arad_allocate_rcy_port(int unit, int core, soc_port_t *port)
{
    soc_port_t port_first = 0;
    uint32 tchannel, is_valid;
    uint8 channels[SOC_DPP_MAX_NOF_CHANNELS] = {0};
    soc_pbmp_t phy_ports, ports_bm, tm_ports;
    soc_port_if_t interface_type;
    uint32 channel, tm_port = 0;
    int found = 0;
    int egr_if_i;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &ports_bm));
    SOC_PBMP_ITER(ports_bm, *port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, *port, &interface_type));
        if(SOC_PORT_IF_RCY == interface_type) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, *port, &tchannel));
             channels[tchannel] = 1;
        }
    }

    for (tchannel = 0; tchannel < SOC_DPP_MAX_NOF_CHANNELS; ++tchannel) { 
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_channels_to_egr_nif_mapping.get(unit, core, tchannel, &egr_if_i));
        if ((!channels[tchannel]) && (egr_if_i == ARAD_PORT_INVALID_EGQ_INTERFACE)) {
            break;
        }
    }

    if (tchannel >= SOC_DPP_MAX_NOF_CHANNELS) {
        return SOC_E_RESOURCE;
    }

    channel = tchannel;

    if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
        port_first = 128;
    }

    
    for (*port = 0; *port < SOC_DPP_DEFS_GET(unit, nof_logical_ports); (*port)++)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, *port, &is_valid));
        if (!is_valid) {
            if (SOC_DPP_CONFIG(unit)->arad->reserved_ports[*port].is_reserved &&
                (core == SOC_DPP_CONFIG(unit)->arad->reserved_ports[*port].core))
            {
                tm_port = SOC_DPP_CONFIG(unit)->arad->reserved_ports[*port].tm_port;
                found = 1;
                break;
            }
        }
    }
    
    if (!found) {
        for (*port = port_first; *port < SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++(*port)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, *port, &is_valid));
            if (!is_valid) {

                if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                    tm_port = *port;
                    
                    if ((SOC_INFO(unit).erp_port[core] >= 0) && (tm_port == ARAD_ERP_PORT_ID)) {
                        continue;
                    }
                    found = 1;

                } else { 
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_free_tm_ports_get(unit, core, &tm_ports));
                    SOC_PBMP_ITER(tm_ports, tm_port) {
                        
                        if ((SOC_INFO(unit).erp_port[core] >= 0) && (tm_port == ARAD_ERP_PORT_ID)) {
                            continue;
                        }
                        found = 1;
                        break;
                    }
                }

                if (found) {
                    break;
                }
            }
        }
    }

    if (found) {
        SOC_PBMP_CLEAR(phy_ports);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_add(unit, core, *port, channel, SOC_PORT_FLAGS_VIRTUAL_RCY_INTERFACE, SOC_PORT_IF_RCY, phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, *port, tm_port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, *port, tm_port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, *port, 1));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: No availabe ports")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_free_tm_port_and_recycle_channel(int unit, int port)
{
    uint32 is_valid;
    soc_port_if_t interface_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("invalid port %d"), port));
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (interface_type != SOC_PORT_IF_RCY) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("port(%d) not allocated by soc_arad_allocate_rcy_port()"), port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_remove(unit, port));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_fc_oob_mode_validate(int unit, int port)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_ps_static_mapping(int unit, soc_port_t port, int out_port_priority, int *is_static_mapping, int* base_q_pair)
{
    soc_dpp_config_arad_t *dpp_arad;
    int prop_invalid = 0xffffffff, val;
    soc_port_if_t interface_type;
    uint32 channel;
    SOCDNX_INIT_FUNC_DEFS;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    *is_static_mapping = 0;

    
    val = soc_property_port_get(unit, port, spn_OTM_BASE_Q_PAIR, prop_invalid);
    if (val != prop_invalid)
    {
        
        *base_q_pair = val;
        *is_static_mapping = 1;
    }
    else
    {
       
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (SOC_PORT_IF_CPU == interface_type && !SOC_PBMP_MEMBER(dpp_arad->init.rcpu.slave_port_pbmp, port)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
            *base_q_pair = ARAD_PS_CPU_FIRST_VALID_QPAIR + channel;
            *is_static_mapping = 1;
        } else {
            if (interface_type == SOC_PORT_IF_ERP) {
                *base_q_pair = ARAD_FAP_EGRESS_REPLICATION_BASE_Q_PAIR;
                *is_static_mapping = 1;
            }
        }
    }

    if(*is_static_mapping) {
        if((*base_q_pair+(out_port_priority-1)) >= ARAD_EGR_NOF_Q_PAIRS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Port %d, qpair is out of range"), port));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_arad_ps_reserved_mapping_init(int unit)
{
    int  val, i, core;
    char *propval;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOC_PBMP_CLEAR(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair[core]);
        SOC_PBMP_CLEAR(SOC_DPP_CONFIG(unit)->arad->reserved_fmq_base_q_pair[core]);
    }

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        propval = soc_property_suffix_num_only_suffix_str_get(unit, core, spn_OTM_BASE_Q_PAIR, "isq_core");
        if (propval != NULL) {
            val = _shr_ctoi(propval);
            if (val < 0 || val >= SOC_MAX_NUM_PORTS) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_arad_ps_reserved_mapping_init error in soc_arad_info_config")));
            }
            SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair[core], val);
        } else {
            propval = soc_property_suffix_num_only_suffix_str_get(unit, 0, spn_OTM_BASE_Q_PAIR, "isq");
            if (propval != NULL) {
                val = _shr_ctoi(propval);
                if (val < 0 || val >= SOC_MAX_NUM_PORTS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_arad_ps_reserved_mapping_init error in soc_arad_info_config")));
                }
                SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair[core], val);
            }
        }

        
        for (i=0; i<SOC_TMC_MULT_FABRIC_NOF_CLS; i++)
        {
            char buf[20];
            sal_snprintf(buf, 20,"fmq%d_core", i);
            propval = soc_property_suffix_num_only_suffix_str_get(unit, core, spn_OTM_BASE_Q_PAIR, buf);
            if (propval != NULL) {
                val = _shr_ctoi(propval);
                if (val < 0 || val >= SOC_MAX_NUM_PORTS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_arad_ps_reserved_mapping_init error in soc_arad_info_config")));
                }
                SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_fmq_base_q_pair[core], val);
            } else {
                propval = soc_property_suffix_num_only_suffix_str_get(unit, i, spn_OTM_BASE_Q_PAIR, "fmq");
                if (propval != NULL) {
                    val = _shr_ctoi(propval);
                    if (val < 0 || val >= SOC_MAX_NUM_PORTS) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_arad_ps_reserved_mapping_init error in soc_arad_info_config")));
                    }
                    SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_fmq_base_q_pair[core], val);
                }
            }
        }
     }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_fmq_isq_hr_init(int unit, bcm_core_t core, uint8 is_fmq)
{
    int index = 0, base_q_pair = 0, found = 0;
    uint8 is_hr_free = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!is_fmq && (SOC_DPP_CONFIG(unit)->tm.hr_isq[core] == SOC_TMC_SCH_PORT_ID_INVALID_ARAD) ) {
        for (base_q_pair=0; base_q_pair<SOC_MAX_NUM_PORTS; base_q_pair++)
        {
            if (SOC_PBMP_MEMBER(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair[core], base_q_pair)) {
                
                SOCDNX_IF_ERR_EXIT(soc_arad_validate_hr_is_free(unit, core, base_q_pair, &is_hr_free));
                if (is_hr_free) {
                    found = 1;
                    break;
                }
            }
        }
        if (!found)
        {
            SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_non_binding_ps(unit, core, 0 , &base_q_pair));
        }

        SOC_DPP_CONFIG(unit)->tm.hr_isq[core] = base_q_pair;
    }

    
    if (is_fmq && (SOC_DPP_CONFIG(unit)->tm.hr_fmqs[core][0] == SOC_TMC_SCH_PORT_ID_INVALID_ARAD) ) {
        for (base_q_pair=0; base_q_pair<SOC_MAX_NUM_PORTS; base_q_pair++)
        {
            if (SOC_PBMP_MEMBER(SOC_DPP_CONFIG(unit)->arad->reserved_fmq_base_q_pair[core], base_q_pair) && index<4) {
                SOC_DPP_CONFIG(unit)->tm.hr_fmqs[core][index] = base_q_pair;
                index++;
            }
        }
        while (index < SOC_TMC_MULT_FABRIC_NOF_CLS) {
            SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_non_binding_ps(unit, core, 0 , &base_q_pair));
            SOC_DPP_CONFIG(unit)->tm.hr_fmqs[core][index] = base_q_pair;
            index++;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_isq_hr_get(int unit, bcm_core_t core, int *hr_isq)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_arad_fmq_isq_hr_init(unit, core, FALSE);
    SOCDNX_IF_ERR_EXIT(rv);

    *hr_isq = SOC_DPP_CONFIG(unit)->tm.hr_isq[core];

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_fmq_base_hr_get(int unit, bcm_core_t core, int** base_hr_fmq)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_arad_fmq_isq_hr_init(unit, core, TRUE);
    SOCDNX_IF_ERR_EXIT(rv);

    *base_hr_fmq = &(SOC_DPP_CONFIG(unit)->tm.hr_fmqs[core][0]);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_collect_nif_config(int unit)
{
    uint32 nof_ilkn_ports, nof_caui_ports, profile_offset;

    SOCDNX_INIT_FUNC_DEFS;

    nof_ilkn_ports = SOC_DPP_DEFS_GET(unit, nof_interlaken_ports);
    nof_caui_ports = SOC_DPP_DEFS_GET(unit, nof_caui_ports);

    
    for (profile_offset = 0; profile_offset < nof_ilkn_ports; profile_offset++) {
        SOCDNX_IF_ERR_EXIT(soc_arad_ilkn_config_get(unit,profile_offset));
    }

    
    for (profile_offset = 0; profile_offset < nof_caui_ports; profile_offset++) {
        SOCDNX_IF_ERR_EXIT(soc_arad_caui_config_get(unit,profile_offset));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_is_device_ilkn_disabled(int unit, int inlk)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    if(driver_dev_id == BCM88350_DEVICE_ID ||
       driver_dev_id == BCM88351_DEVICE_ID ||
       driver_dev_id == BCM88450_DEVICE_ID ||
       driver_dev_id == BCM88451_DEVICE_ID ||
       driver_dev_id == BCM88550_DEVICE_ID ||
       driver_dev_id == BCM88551_DEVICE_ID ||
       driver_dev_id == BCM88552_DEVICE_ID ||
       driver_dev_id == BCM88360_DEVICE_ID ||
       driver_dev_id == BCM88361_DEVICE_ID ||
       driver_dev_id == BCM88461_DEVICE_ID ||
       driver_dev_id == BCM88560_DEVICE_ID ||
       driver_dev_id == BCM88561_DEVICE_ID
       ) {

#if defined(INCLUDE_KBP)
        
         if(SOC_DPP_CONFIG(unit)->arad->init.elk.enable == FALSE ||
           (SOC_DPP_CONFIG(unit)->arad->init.elk.enable == TRUE && inlk == 0))
#endif 
        return TRUE;
    }


    return FALSE;
}


STATIC int
soc_arad_validate_device_num_of_dram(int unit, uint32 val)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {

        case BCM88351_DEVICE_ID:
        case BCM88451_DEVICE_ID:
        case BCM88551_DEVICE_ID:
        case BCM88651_DEVICE_ID:
        case BCM88361_DEVICE_ID:
        case BCM88461_DEVICE_ID:
        case BCM88561_DEVICE_ID:
        case BCM88661_DEVICE_ID:
            if(val > 6) {
                SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
            }
            break;
        case BCM88552_DEVICE_ID:
        case BCM88562_DEVICE_ID:

            if(val > 4) {
                SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
            }
            break;

        case BCM88664_DEVICE_ID:
        case BCM88560_DEVICE_ID:
            if(val != 0) {
                SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
            }
            break;
        default:
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_validate_nif_in_use(int unit,  soc_pbmp_t phy_ports)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;
    int port_i;
    int logical_port;
    uint32 flags;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {

        case BCM88351_DEVICE_ID:
        case BCM88361_DEVICE_ID:

            for(port_i=17; port_i <= 28; ++port_i) {
                
                if(SOC_PBMP_MEMBER(phy_ports, port_i)) {
                    logical_port=SOC_INFO(unit).port_p2l_mapping[port_i];
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, logical_port, &flags));
                    if (!(SOC_PORT_IS_ELK_INTERFACE(flags))) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Port %d disable by the device, ( 0x%x pool B disabled)"), port_i, driver_dev_id));
                    }
                }
            }
            break;

            default:
                break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_ocb_enable_mode(int unit, ARAD_INIT_OCB *ocb)
{
    int val;
    char *val_string;
    SOCDNX_INIT_FUNC_DEFS;

    val_string = soc_property_get_str(unit, spn_BCM886XX_OCB_ENABLE);
    if (val_string == NULL) {
    
        ocb->ocb_enable = OCB_ENABLED;
    }
    else if ((sal_strcmp(val_string, "0") == 0) || (sal_strcmp(val_string, "FALSE") == 0)) {
        ocb->ocb_enable = OCB_DISABLED;
    }
    else if ((sal_strcmp(val_string, "1") == 0) || (sal_strcmp(val_string, "TRUE") == 0)) {
        ocb->ocb_enable = OCB_ENABLED;
    }
    else if (SOC_IS_ARADPLUS(unit) && !SOC_IS_JERICHO(unit)){
        if (sal_strcmp(val_string, "ONE_WAY_BYPASS") == 0) {
            val = soc_property_get(unit, spn_EXT_RAM_PRESENT,0);
            if (val==0) {
                ocb->ocb_enable = OCB_ONLY;
            }
            else if (val==1) {
                ocb->ocb_enable = OCB_ONLY_1_DRAM;
            }
            else if ((val>=2) && (val<=8)) {
                ocb->ocb_enable = OCB_DRAM_SEPARATE;
            }
            else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("invalid soc property: 'ext_ram_present'")));
            }
        }
    }
    else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("invalid soc property: 'bcm886xx_ocb_enable'")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_validate_ocb_enable(int unit,  ARAD_OCB_ENABLE_MODE ocb_enable)
{
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {

        case BCM88654_DEVICE_ID:
        case BCM88664_DEVICE_ID:
            if(ocb_enable != OCB_DISABLED) {
                SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
            }

            break;

        default:
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_arad_cmic_info_config(int unit)
{

    int cmc_i;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PCI_CMC(unit)   = soc_property_uc_get(unit, 0, spn_PCI_CMC, SOC_DPP_ARAD_PCI_CMC);
    SOC_ARM_CMC(unit, 0) = soc_property_uc_get(unit, 1, spn_CMC, SOC_DPP_ARAD_ARM1_CMC);
    SOC_ARM_CMC(unit, 1) = soc_property_uc_get(unit, 2, spn_CMC, SOC_DPP_ARAD_ARM2_CMC);

    
    SOC_CMCS_NUM(unit) = SOC_DPP_ARAD_NUM_CMCS;
    NUM_CPU_COSQ(unit) = SOC_DPP_ARAD_NUM_CPU_COSQ - 1;

    
    NUM_CPU_ARM_COSQ(unit, 0) = soc_property_uc_get(unit, 0, spn_NUM_QUEUES, NUM_CPU_COSQ(unit));
    NUM_CPU_ARM_COSQ(unit, 1) = soc_property_uc_get(unit, 1, spn_NUM_QUEUES, 0);
    NUM_CPU_ARM_COSQ(unit, 2) = soc_property_uc_get(unit, 2, spn_NUM_QUEUES, 0);

    
    for (cmc_i = 0; cmc_i < SOC_CMCS_NUM(unit); cmc_i++) {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc_i), 0, NUM_CPU_COSQ_MAX);
    }

    
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 0), 0, NUM_CPU_ARM_COSQ(unit, 0));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 1), NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 2), NUM_CPU_ARM_COSQ(unit, 0) + NUM_CPU_ARM_COSQ(unit, 1), NUM_CPU_ARM_COSQ(unit, 2));

    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_arad_ports_info_config(int unit)
{
    int rv = SOC_E_NONE;
    uint32 val;
    uint32 channel, is_hg, flags, is_pon, init_vid_enabled, nof_channels, offset;
    SOC_TMC_INTERFACE_ID nif_id;
    soc_port_t port_i;
    soc_info_t          *si;
    int i, intern_id, is_channelized;
    soc_pbmp_t phy_ports, pon_port_bm, ports_bm;
    soc_port_if_t interface;
    uint32 erp_tm_port_id;
    int core = SOC_ARAD_CPU_PHY_CORE ; 
    soc_dpp_config_pp_t *dpp_pp;

    SOCDNX_INIT_FUNC_DEFS;
    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    si  = &SOC_INFO(unit);

    
    arad_tbl_default_dynamic_set();

    
    arad_tbl_mark_cachable(unit);

    
    SOC_PBMP_CLEAR(phy_ports);
    for (port_i = 0; port_i < SOC_DPP_NOF_DIRECT_PORTS; ++port_i) {
        SOC_PBMP_PORT_ADD(phy_ports, port_i+1);
    }

    if (SOC_WARM_BOOT(unit)) {
       
       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_take(unit));
    }

    
    rv = soc_arad_str_prop_voq_mapping_mode_get(unit, &SOC_DPP_CONFIG(unit)->arad->voq_mapping_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_arad_str_prop_hqos_mapping_get(unit, &SOC_DPP_CONFIG(unit)->arad->hqos_mapping_enable);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_init(unit));

    
    sal_memset(SOC_DPP_CONFIG(unit)->arad->reserved_ports, 0x0, sizeof(SOC_DPP_CONFIG(unit)->arad->reserved_ports));

    
    soc_arad_ps_reserved_mapping_init(unit);


    if (SOC_WARM_BOOT(unit)) {
       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_restore(unit));

    }

    
    SOCDNX_IF_ERR_EXIT(soc_pm_init(unit, phy_ports, SOC_DPP_FIRST_DIRECT_PORT(unit)));

    
    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i)
    {
        nif_id = SOC_TMC_NIF_ID_NONE;
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_ucode_port(unit, port_i, &nif_id, &channel, &is_hg));
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_pon_port(unit, port_i, &is_pon));
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port_i, &init_vid_enabled));


        
        if (nif_id == SOC_TMC_IF_ID_RESERVED) {
            SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].is_reserved = TRUE;
            SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].core = 0;
            SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].tm_port = port_i;
            continue;
        }

        
        if (is_pon) {
          dpp_pp->pon_application_enable = 1;
          
          
          PORT_SW_DB_PORT_ADD(pon, port_i);
        }

        
        if (!init_vid_enabled) {
           dpp_pp->port_use_initial_vlan_only_enabled = 1;
        }

        if (!SOC_WARM_BOOT(unit)) {
            if (nif_id != SOC_TMC_NIF_ID_NONE && nif_id != SOC_TMC_IF_ID_RESERVED) {
                interface = SOC_PORT_IF_NULL;
                flags = 0;

                intern_id = arad_nif2intern_id(unit, nif_id);
                SOC_PBMP_CLEAR(phy_ports);

                if (ARAD_NIF_IS_TYPE_ID(XAUI, nif_id))
                {
                    interface = SOC_PORT_IF_DNX_XAUI;
                    for(i=0;i<4;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(RXAUI, nif_id))
                {
                    interface = SOC_PORT_IF_RXAUI;
                    for(i=0;i<2;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(SGMII, nif_id))
                {
                    interface = SOC_PORT_IF_SGMII;
                    SOC_PBMP_PORT_ADD(phy_ports, intern_id+1);
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_id))
                {
                    uint32 ilkn_id;
                    ilkn_id = ARAD_NIF_ID_OFFSET(ILKN, nif_id);
                    if(soc_arad_is_device_ilkn_disabled(unit, ilkn_id)){
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc property try to config ILKN, device is ILKN disabled.")));
                    }
                    interface = SOC_PORT_IF_ILKN;
                    SOCDNX_IF_ERR_EXIT(soc_arad_ilkn_phy_ports_btmp_set(unit, ilkn_id, &phy_ports));
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
#if defined(INCLUDE_KBP)
                    if(SOC_DPP_CONFIG(unit)->arad->init.elk.enable) {
                        if (ilkn_id == 1) {
                            flags |= SOC_PORT_FLAGS_ELK;
                        }
                    }
#endif
                }
                else if (ARAD_NIF_IS_TYPE_ID(10GBASE_R, nif_id))
                {
                    interface = SOC_PORT_IF_XFI;
                    SOC_PBMP_PORT_ADD(phy_ports, intern_id+1);
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(CGE, nif_id))
                {
                    interface = SOC_PORT_IF_CAUI;
                    val = soc_property_port_get(unit, intern_id/16, spn_CAUI_NUM_LANES, 0);
                    for(i=0;i<val;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(XLGE, nif_id))
                {
                    interface = SOC_PORT_IF_XLAUI;
                    for(i=0;i<4;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if(ARAD_IF_ID_CPU == nif_id) {
                    interface = SOC_PORT_IF_CPU;
                    
                    SOC_PBMP_PORT_ADD(phy_ports, 0);
                }
                else if(ARAD_IF_ID_RCY == nif_id) {
                    interface = SOC_PORT_IF_RCY;
                    SOC_PBMP_CLEAR(phy_ports);
                }
                else if(ARAD_IF_ID_TM_INTERNAL_PKT == nif_id) {
                    interface = SOC_PORT_IF_TM_INTERNAL_PKT;
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                    
                    SOC_PBMP_PORT_ADD(phy_ports, 1);
                }

                 
                if(SOC_PBMP_MEMBER(phy_ports, 29) ||
                   SOC_PBMP_MEMBER(phy_ports, 30) ||
                   SOC_PBMP_MEMBER(phy_ports, 31) ||
                   SOC_PBMP_MEMBER(phy_ports, 32))
                {
                    flags |= SOC_PORT_FLAGS_STAT_INTERFACE;
                }

                if (is_pon) {
                  flags |= SOC_PORT_FLAGS_PON_INTERFACE;
                }

                
                if (!init_vid_enabled) {
                    flags |= SOC_PORT_FLAGS_INIT_VID_ONLY;
                } else {
                      
                      
                      dpp_pp->port_use_outer_vlan_and_initial_vlan_enabled = 1;
                }

                rv = soc_port_sw_db_port_validate_and_add(unit, core, port_i, channel, flags, interface, phy_ports);
                SOCDNX_IF_ERR_EXIT(rv);

                SOCDNX_IF_ERR_EXIT(soc_arad_validate_nif_in_use(unit, phy_ports));

                
                if (interface == SOC_PORT_IF_ILKN || interface == SOC_PORT_IF_CAUI) {
                    offset = 0;
                    if (interface == SOC_PORT_IF_ILKN) {
                        offset = ARAD_NIF_ID_OFFSET(ILKN, nif_id);
                    } else {
                        if (interface == SOC_PORT_IF_CAUI) {
                            offset = ARAD_NIF_ID_OFFSET(CGE, nif_id);
                        }
                    }

                   SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port_i, 0, offset));
                }


                rv = soc_port_sw_db_is_hg_set(unit, port_i, is_hg);
                SOCDNX_IF_ERR_EXIT(rv);

                val = soc_property_port_get(unit, port_i, spn_LOCAL_TO_TM_PORT, port_i);
                if (soc_port_sw_db_local_to_tm_port_set(unit, port_i, val)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("_soc_dpp_local_to_tm_port_set error in soc_arad_info_config")));
                }
                val = soc_property_port_get(unit, port_i, spn_LOCAL_TO_PP_PORT, port_i);

                if (soc_port_sw_db_local_to_pp_port_set(unit, port_i, val)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("_soc_dpp_local_to_pp_port_set error in soc_arad_info_config")));
                }
            }
        }
    }

    SOC_PBMP_CLEAR(pon_port_bm);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_PON_INTERFACE, &pon_port_bm));

    SOC_PBMP_ITER(pon_port_bm, port_i) {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
        if (SOC_PORT_IF_CPU == interface) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port_i, &nof_channels));

        if (nof_channels > 1) {
          dpp_pp->pon_port_channelization_enable = 1;
        }

        
        if (nof_channels > dpp_pp->pon_port_channelization_num) {
            dpp_pp->pon_port_channelization_num = nof_channels;
        }
    }

    
    if (!SOC_WARM_BOOT(unit))
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));
        SOC_PBMP_ITER(ports_bm, port_i) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port_i, &is_channelized));
            if (is_channelized) {
                
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port_i, FALSE));
            } else {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port_i, TRUE));
            }
        }
    }

    if (!SOC_WARM_BOOT(unit)) {

        
        SOC_PBMP_CLEAR(phy_ports);
        si->olp_port[0] = -1;
        si->olp_port[1] = -1;
        val = soc_property_suffix_num_get(unit, 0, spn_NUM_OLP_TM_PORTS, "core", 0);
        if (val > 0) {
            si->olp_port[0] = ARAD_OLP_PORT_ID;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, si->olp_port[0], 0, 0, SOC_PORT_IF_OLP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->olp_port[0], ARAD_OLP_PORT_ID));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->olp_port[0], ARAD_OLP_PORT_ID));
        }

        si->oamp_port[0] = -1;
        si->oamp_port[1] = -1;
        SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_NUM_OAMP_PORTS, 0,&val));

        if (val > 0) {
            si->oamp_port[0] = ARAD_OAMP_PORT_ID;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, si->oamp_port[0], 0, 0, SOC_PORT_IF_OAMP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->oamp_port[0], ARAD_OAMP_PORT_ID));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->oamp_port[0], ARAD_OAMP_PORT_ID));
        }

        si->erp_port[0] = -1;
        si->erp_port[1] = -1;
        val = soc_property_get(unit, spn_NUM_ERP_TM_PORTS, 0);
        if (val > 0) {
            si->erp_port[0] = SOC_DPP_PORT_INTERNAL_ERP(0);
            erp_tm_port_id = soc_property_suffix_num_get(unit, si->erp_port[0], spn_LOCAL_TO_TM_PORT, "erp", ARAD_ERP_PORT_ID);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, si->erp_port[0], 0, 0, SOC_PORT_IF_ERP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->erp_port[0], erp_tm_port_id));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->erp_port[0], erp_tm_port_id)); 
        }
    } else {
        
        uint32 value;
        SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_NUM_OAMP_PORTS, 0,&value));

        if (value) {
            si->oamp_port[0] = ARAD_OAMP_PORT_ID;
        }
        if (soc_property_suffix_num_get(unit, 0, spn_NUM_OLP_TM_PORTS, "core", 0)) {
            si->olp_port[0] = ARAD_OLP_PORT_ID;
        }
        if (soc_property_get(unit, spn_NUM_ERP_TM_PORTS, 0)) {
            si->erp_port[0] = SOC_DPP_PORT_INTERNAL_ERP(0);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_info_elk_config(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
{
    soc_dpp_config_arad_t *dpp_arad;
    ARAD_INIT_ELK *elk;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;
    elk = &(SOC_DPP_CONFIG(unit)->arad->init.elk);

    
    SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_tcam_dev_type_get(unit, &dpp_arad->init.elk.tcam_dev_type));

    if (dpp_arad->init.elk.tcam_dev_type != ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE) {

        if(soc_arad_is_device_elk_disabled(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc property try to config ELK, device is ELK disabled.")));
        }
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
            dpp_arad->init.elk.ext_interface_mode = soc_property_get(unit, spn_EXT_INTERFACE_MODE, FALSE);
        }
#endif 
        dpp_arad->init.elk.enable = 0x1;
    }

    SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_fwd_table_size_get(unit, &dpp_arad->init.elk));

    elk->kbp_recover_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_recovery_enable", 0);
    elk->kbp_recover_iter = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_recovery_iter", 5);

#ifdef BCM_88675_A0

    elk->kbp_no_fwd_ipv6_dip_sip_sharing_disable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_no_fwd_ipv6_dip_sip_sharing_disable", 0);
    elk->kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header", 0);
    if(elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1]){
        elk->kbp_no_fwd_ipv6_dip_sip_sharing_disable = 1;
    }
    if((elk->kbp_no_fwd_ipv6_dip_sip_sharing_disable == 1) && (elk->kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header == 1)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Illegal soc property combination. Both kbp_no_fwd_ipv6_dip_sip_sharing_disable and kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header set to 1.\n")));
    }

#endif 
    elk->is_master = TRUE;
    if (dpp_arad->init.elk.tcam_dev_type == ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52311 || dpp_arad->init.elk.tcam_dev_type == ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52321) {
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_tcam_connect_mode_get(unit, &dpp_arad->init.elk.kbp_connect_mode));
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_core_port_mapping_get(unit));
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_lut_write_mode_get(unit, &dpp_arad->init.elk.kbp_lut_write_mode));
        if (ARAD_KBP_OP_IS_DUAL_MODE) {
            if (unit % 2 != 0) {
                elk->is_master = FALSE;
            }
        }
        elk->kbp_serdes_pcie_init = soc_property_get(unit, spn_EXT_TCAM_SERDES_PCIE_INIT, 0);
    }

    elk->kbp_24bit_fwd = ((soc_property_suffix_num_get(unit, 0, spn_EXT_TCAM_RESULT_SIZE_SEGMENT, "0", 48) == 24) ? TRUE : FALSE);
}
exit:
#endif 

    SOCDNX_FUNC_RETURN;
}


int
soc_arad_info_config(int unit)
{
    int rv = SOC_E_NONE;
    soc_dpp_config_arad_t *dpp_arad;
    int prop_invalid = 0xffffffff, val;
    uint32 is_hg_header, flags, is_sop_only, drams, stat_if;
    int fabric_link;
    uint8 include_port = 0;
    int ind;
    int ip_lpm_total = 0;
    soc_port_t port_i, phy_port;
    int blk;
    int blktype;
    int pp_port_configured[SOC_MAX_NUM_PORTS];
    uint32 pp_port, tm_port;
    uint32 out_port_priority;
    soc_info_t          *si;
    int base_q_pair;
    int is_static_mapping, core;
    int auxiliary_table_mode;
    int quad_active, quad_index;
    int i, quad, nof_quads;
    soc_pbmp_t pbmp_disabled;
    soc_port_if_t interface;
    uint32 is_valid, multicast_offset, channel;
    uint32 padding_size;
    uint8 xgs_port_exists = 0;
    uint8 udh_port_exists =0;
    soc_dpp_config_pp_t *dpp_pp;
    uint8 bank_phase;
    ARAD_PORT_HEADER_TYPE header_type_in, header_type_out;
    uint32 first_header_size;
    uint32 shaper_mode = 0;
    uint32 peer_tm_domain;
    int dflt_core_clck_frq_khz, dflt_glob_clck_frq_mhz;
    char *propkey, *propval;
    int tdm_priority;
    uint32 dram_bitmap = 0x0;
    uint32 is_vendor_pp_port;
    uint32 port_extender_map_lc = 0, gl_port_extender_map_lc = 0;
    int core_id;
    uint8 is_allocated;
    int use_trunk_as_ingress_mc_destination_default = 0;
    uint32 timestamp_and_ssp_type = 0;
    int stat_if_nof_report_mode_configure = 0;
    int total_flow_region = SOC_TMC_COSQ_TOTAL_FLOW_REGIONS;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(pp_port_configured, 0x0, sizeof(pp_port_configured));

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;
    si  = &SOC_INFO(unit);
    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    SOCDNX_IF_ERR_EXIT(soc_arad_cmic_info_config(unit));

    SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system = soc_property_get(unit, spn_SYSTEM_IS_PETRA_B_IN_SYSTEM, 0x0);
    SOC_DPP_CONFIG(unit)->emulation_system = soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0);
    si->num_ucs = 2;
    if(!SOC_WARM_BOOT(unit)) {
        rv = sw_state_access[unit].dpp.soc.arad.pp.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.pp.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        rv = sw_state_access[unit].dpp.soc.arad.pp.vt_profile_info.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.pp.vt_profile_info.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        rv = sw_state_access[unit].dpp.soc.arad.pp.oamp_pe.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

       if(!is_allocated) {
           rv = sw_state_access[unit].dpp.soc.arad.pp.oamp_pe.alloc(unit);
           SOCDNX_IF_ERR_EXIT(rv);
       }

        rv = sw_state_access[unit].dpp.soc.arad.pp.ptp.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.pp.ptp.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }

    }

    rv = _soc_dpp_port_map_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_WARM_BOOT(unit)) {
        _soc_dpp_wb_pp_port_restore(unit);
        _dflt_tm_pp_port_map[unit] = FALSE;
    }

    
    dpp_arad->init.is_petrab_in_system = SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system;

    
    SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits = (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) ? 32:48;

    SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size = (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) ? 1:0;

    if (SOC_IS_JERICHO(unit)) {
        SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe = 0x0;
    } else {
        SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe = soc_property_get(unit, spn_FABRIC_TDM_OVER_PRIMARY_PIPE, 0x0);
    }
    
    dflt_core_clck_frq_khz = SOC_DPP_DEFS_GET(unit, core_clock_freq_khz);
    rv = soc_arad_core_frequency_config_get(unit, dflt_core_clck_frq_khz , &(dpp_arad->init.core_freq.frequency));
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_INFO(unit).frequency = dpp_arad->init.core_freq.frequency/1000;

    SOCDNX_IF_ERR_EXIT(soc_arad_validate_device_core_frequency(unit, dpp_arad->init.core_freq.frequency));

    dflt_glob_clck_frq_mhz = SOC_DPP_DEFS_GET(unit, glob_clock_freq_mhz);
    if (soc_property_get_str(unit, spn_SYSTEM_REF_CORE_CLOCK) != NULL) {
        
        dpp_arad->init.core_freq.system_ref_clock = soc_property_get(unit, spn_SYSTEM_REF_CORE_CLOCK, dflt_glob_clck_frq_mhz) * 1000;
    } else {
        
        dpp_arad->init.core_freq.system_ref_clock = soc_property_suffix_num_get(unit, 0, spn_SYSTEM_REF_CORE_CLOCK, "khz", dflt_glob_clck_frq_mhz * 1000);
    }
    dpp_arad->init.core_freq.enable = TRUE;

    SOCDNX_IF_ERR_EXIT(soc_arad_prop_fap_device_mode_get(unit, &dpp_arad->init.pp_enable, &dpp_arad->init.tdm_mode));

    

    rv = soc_arad_schan_timeout_config_get(unit, &(SOC_CONTROL(unit)->schanTimeout));
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_CONTROL(unit)->schanIntrEnb = soc_property_get(unit, spn_SCHAN_INTR_ENABLE, 0);
    SOC_CONTROL(unit)->miimTimeout =  soc_property_get(unit, spn_MIIM_TIMEOUT_USEC, 2000);
    SOC_CONTROL(unit)->miimIntrEnb = soc_property_get(unit, spn_MIIM_INTR_ENABLE, 0);

    
    if (!SOC_IS_FLAIR(unit)) {
        dpp_arad->init.credit.credit_worth_enable = TRUE;
        dpp_arad->init.credit.credit_worth = soc_property_get(unit, spn_CREDIT_SIZE, 1024);
        rv = soc_arad_str_prop_credit_worth_resolution_get(unit, &dpp_arad->init.credit.credit_worth_resolution);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        dpp_arad->init.credit.credit_worth_enable = FALSE;
        dpp_arad->init.credit.credit_worth = 0;
    }

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_common_tm(unit));
    }

    
    if (SOC_IS_JERICHO(unit)) {
        propkey = spn_FABRIC_TDM_PRIORITY_MIN;
        propval = soc_property_get_str(unit, propkey);
        if (propval) {
            if (sal_strcmp(propval, "NONE") == 0) {
                dpp_arad->init.fabric.fabric_tdm_priority_min = SOC_DPP_FABRIC_TDM_PRIORITY_NONE;
            } else {
                tdm_priority = soc_property_get(unit, spn_FABRIC_TDM_PRIORITY_MIN, 3);
                if ((tdm_priority < 0) || (tdm_priority >= ARAD_FBC_PRIORITY_NOF)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s\n\r"), propval, propkey));
                }
                dpp_arad->init.fabric.fabric_tdm_priority_min = tdm_priority;
            }
        } else {
            dpp_arad->init.fabric.fabric_tdm_priority_min = 3;
        }
    } else {
        dpp_arad->init.fabric.fabric_tdm_priority_min = 3;
    }

    if (SOC_IS_JERICHO(unit)) {
        dpp_arad->init.fabric.dual_pipe_tdm_packet = FALSE;
        dpp_arad->init.fabric.is_dual_mode_in_system = FALSE;
        dpp_arad->init.fabric.system_contains_multiple_pipe_device = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE, FALSE));
        dpp_arad->init.fabric.fabric_pipe_map_config.nof_pipes = soc_property_get(unit, spn_FABRIC_NUM_PIPES, 1);
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_pipes_config(unit));
        dpp_arad->init.fabric.fabric_mesh_topology_fast = SOC_SAND_NUM2BOOL(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_topology_fast", TRUE));
    } else {
        dpp_arad->init.fabric.dual_pipe_tdm_packet = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_IS_DUAL_MODE, FALSE));
        dpp_arad->init.fabric.fabric_pipe_map_config.nof_pipes = dpp_arad->init.fabric.dual_pipe_tdm_packet ? 2 : 1;
        dpp_arad->init.fabric.is_dual_mode_in_system = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_IS_DUAL_MODE_IN_SYSTEM, dpp_arad->init.fabric.dual_pipe_tdm_packet));
        dpp_arad->init.fabric.system_contains_multiple_pipe_device = dpp_arad->init.fabric.is_dual_mode_in_system;
        if (dpp_arad->init.fabric.dual_pipe_tdm_packet && !(dpp_arad->init.fabric.is_dual_mode_in_system)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Unsupported properties: is_dual_mode=1 & system_is_dual_mode_in_system=0")));
        }
        dpp_arad->init.fabric.fabric_mesh_topology_fast = 0;
    }


    SOCDNX_IF_ERR_EXIT(soc_dpp_str_prop_fabric_connect_mode_get(unit, &dpp_arad->init.fabric.connect_mode));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_arad_str_prop_mc_tbl_mode(unit)); 

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        
        SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start = soc_property_get(unit,
          spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MIN, 0);
        SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end = soc_property_get(unit,
          spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX, ARAD_MULTICAST_TABLE_MODE - 1);
        SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start = soc_property_get(unit,
          spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MIN, SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high + 1);
        SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end = soc_property_get(unit,
          spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1);

        if (SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start > SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end ||
            SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start > SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end) {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit,
              "multicast group id ranges can not have a min value bigger than a max value:\n\r"
              "ingress range: min=%u to max=%d\n\r"
              "egress range: min=%u to max=%d\n\r"),
              SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start, SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end,
              SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start, SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end));
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
        } else if (SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end >= ARAD_MULTICAST_TABLE_MODE) {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit,
              "ingress multicast group id range ends in %u which is not a valid group id\n\r"),
              SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end));
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
        } else if (SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit,
              "egress multicast group id range ends in %u which is not a valid group id\n\r"),
              SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end));
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
        }
    } else { 
        if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
            SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start = 0;
            SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1;
        } else {
            SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start = -1;
            SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end = 0;
        }
        if (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
            SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start = 0;
            SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - 1;
        } else {
            SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start = -1;
            SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end = 0;
        }
    }

    
    val = soc_property_get(unit, spn_ILKN_TDM_DEDICATED_QUEUING, 0);
    dpp_arad->init.ilkn_tdm_dedicated_queuing = (val == 0) ? ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_OFF : ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON;

    
    val = soc_property_get(unit, spn_TDM_EGRESS_PRIORITY, 0);
    dpp_arad->init.tdm_egress_priority = val;
    
    val = soc_property_get(unit, spn_TDM_EGRESS_DP, 0);
    dpp_arad->init.tdm_egress_dp = val;

    
    dpp_arad->init.nif_recovery_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "nif_recovery_enable", 1);
    dpp_arad->init.nif_recovery_iter = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "nif_recovery_iter", 5);

    
    dpp_arad->init.rcy_channelized_shared_context_enable = soc_property_get(unit, spn_RCY_CHANNELIZED_SHARED_CONTEXT_ENABLE, 0);

    
    dpp_arad->init.rcpu.slave_port_pbmp = soc_property_get_pbmp(unit, spn_RCPU_RX_PBMP, 0);

    SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 0;

    dpp_pp->ipv6_with_rpf_2pass_exists = 0;

    dpp_pp->ivl_inlif_profile = -1;

    SOCDNX_IF_ERR_EXIT(arad_ps_db_init(unit));

    
    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) {
            continue;
        }

        
        if (SOC_WARM_BOOT(unit)) {
            rv = soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &out_port_priority);
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            out_port_priority = soc_property_port_get(unit, port_i, spn_PORT_PRIORITIES, prop_invalid);
            if (out_port_priority != prop_invalid) {
                rv = soc_port_sw_db_local_to_out_port_priority_set(unit, port_i, out_port_priority);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
        if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_SAME_QPAIR(flags) || SOC_PORT_IS_LB_MODEM(flags))) {
            if (SOC_WARM_BOOT(unit))
            {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
                if ( !((interface == SOC_PORT_IF_RCY) && SOC_PORT_IS_VIRTUAL_RCY_INTERFACE(flags)) ) {
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, (uint32 *) &base_q_pair);
                    SOCDNX_IF_ERR_EXIT(rv);

                    if (interface == SOC_PORT_IF_ERP || interface == SOC_PORT_IF_NOCXN) {
                        SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_non_binding_ps_with_id(unit, port_i, base_q_pair));
                    } else {
                        rv = arad_ps_db_alloc_binding_ps_with_id(unit, port_i, out_port_priority, base_q_pair);
                        SOCDNX_IF_ERR_EXIT(rv);
                    }
                }
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port_i, &channel));

                
                if (interface == SOC_PORT_IF_CPU) {
                    if (channel > (NUM_CPU_ARM_COSQ(unit, 0) + NUM_CPU_ARM_COSQ(unit, 1) + NUM_CPU_ARM_COSQ(unit, 2))) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG_STR( "invalid base queue id %d for CPU port %d\n"),
                                                          (ARAD_PS_CPU_FIRST_VALID_QPAIR + channel), port_i));
                    }
                }

                
                rv = soc_arad_ps_static_mapping(unit, port_i, out_port_priority, &is_static_mapping, &base_q_pair);
                SOCDNX_IF_ERR_EXIT(rv);

                if(is_static_mapping)
                {
                    if (interface == SOC_PORT_IF_ERP) {
                        SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_non_binding_ps_with_id(unit, port_i, base_q_pair));
                    } else {
                        rv = arad_ps_db_alloc_binding_ps_with_id(unit, port_i, out_port_priority, base_q_pair);
                        SOCDNX_IF_ERR_EXIT(rv);
                    }

                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.set(unit, port_i, base_q_pair);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) {
            continue;
        }

        is_vendor_pp_port = soc_property_port_suffix_num_get(unit, port_i, -1, spn_CUSTOM_FEATURE,"vendor_custom_pp_port", FALSE);
        if (is_vendor_pp_port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_VENDOR_PP_PORT));
            dpp_pp->custom_feature_pp_port = 1;
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,"vendor_customer65", FALSE)) {
                dpp_pp->custom_feature_pp_port = 2;
            }
        }
        timestamp_and_ssp_type = soc_property_port_suffix_num_get(unit, port_i, -1, spn_CUSTOM_FEATURE, "Timestamp_SSP", 0);
        if ((timestamp_and_ssp_type == 1) || (timestamp_and_ssp_type == 2) || (timestamp_and_ssp_type == 3)) {
	        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.timestamp_and_ssp.set(unit,port_i,timestamp_and_ssp_type);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.timestamp_and_ssp.set(unit,port_i,0);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        
        gl_port_extender_map_lc = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "map_port_extr_enabled", 0);
        if (!gl_port_extender_map_lc) {
            
            port_extender_map_lc = soc_property_port_suffix_num_get(unit, port_i, -1, spn_CUSTOM_FEATURE, "map_port_extr_enabled", 0);
            if ((port_extender_map_lc==1) || (port_extender_map_lc==2)) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_COE_PORT));
                gl_port_extender_map_lc = port_extender_map_lc;
            }
        }

        if (gl_port_extender_map_lc == 1) {
            dpp_pp->port_extender_map_lc_exists = 1;
        } else if (gl_port_extender_map_lc == 2) {
            dpp_pp->port_extender_map_lc_exists = 2;
        }

        if (!SOC_WARM_BOOT(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
            if ((!SOC_PORT_IS_STAT_INTERFACE(flags)) && (!SOC_PORT_IS_LB_MODEM(flags)) && (!SOC_PORT_IS_ELK_INTERFACE(flags)))
            {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port_i, &pp_port, &core));
            }
            else
            {
                pp_port = -1;
            }
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));

            if(SOC_PORT_IF_CPU == interface) {
               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port_i,SOC_TMC_PORT_HEADER_TYPE_CPU));
            }

            padding_size = soc_property_port_get(unit, port_i, spn_PACKET_PADDING_SIZE, 0);
            if(padding_size > SOC_DPP_MAX_PADDING_SIZE) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Padding size is out-of-range port %d, value %d, max size %d"), port_i, padding_size, SOC_DPP_MAX_PADDING_SIZE));
            }
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port_i, padding_size));

            
            multicast_offset = soc_property_port_get(unit, port_i , spn_MULTICAST_ID_OFFSET, 0);
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.multicast_offset.set(unit,port_i,multicast_offset);
            SOCDNX_IF_ERR_EXIT(rv);

            if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                
                val = soc_property_port_get(unit, port_i, spn_PORT_INIT_SPEED, -1);
                if(-1 == val) {
                    SOCDNX_IF_ERR_EXIT(soc_pm_default_speed_get(unit, port_i, &val));
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port_i, val));
            }

            if(SOC_PORT_IF_ERP != interface) {

                if (soc_dpp_str_prop_parse_tm_port_header_type(unit, port_i, &(header_type_in), &(header_type_out),&is_hg_header)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_header_type error")));
                }
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_set(unit,port_i,header_type_in));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port_i,header_type_out));
                if(is_hg_header) { 
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_set(unit, port_i, is_hg_header));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_XGS_MAC_EXT));
                }
                is_sop_only = soc_property_port_get(unit, port_i, spn_PREAMBLE_SOP_ONLY, FALSE);
                if(IS_HG_PORT(unit,port_i)){
#ifdef BCM_88660_A0
                    if (SOC_IS_ARADPLUS(unit)) {
                        if(is_sop_only){
                            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("can't set port %d to be hg and preamble sop only"), port_i));
                        }
                    }
#endif 
                }
                else{
                    if(SOC_DPP_ARAD_IS_HG_SPEED_ONLY(val)){
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("port_init_speed hg speed %d for not hg port %d"), val, port_i));
                    }
#ifdef BCM_88660_A0
                    if (SOC_IS_ARADPLUS(unit)) {
                        if(is_sop_only){
                            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_set(unit, port_i, SOC_ENCAP_SOP_ONLY));
                        }
                    }
#endif 
                }
                if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM) {
                    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN otmh_ext_en;
                    if (soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(unit,port_i, &otmh_ext_en)) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_otmh_extensions_en error")));
                    }

                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.set(unit, port_i, otmh_ext_en.src_ext_en);
                    SOCDNX_IF_ERR_EXIT(rv);
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.dst_ext_en.set(unit, port_i, otmh_ext_en.dest_ext_en);
                    SOCDNX_IF_ERR_EXIT(rv);
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.set(unit, port_i, otmh_ext_en.outlif_ext_en);
                    SOCDNX_IF_ERR_EXIT(rv);
                    if(otmh_ext_en.outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE) {
                      SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 1;
                    }
                    if (otmh_ext_en.outlif_ext_en != SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER) {
                        dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
                        dpp_pp->otmh_cud_ext_used = 1;
                    }
                }
                if(SOC_PORT_IF_OLP != interface  &&
                   SOC_PORT_IF_OAMP != interface &&
                   SOC_PORT_IF_IPSEC != interface) {
                    
                    if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT) {
                       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_XGS_MAC_EXT));
                    }
                }

                if (!xgs_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
                    dpp_pp->xgs_port_exists = 1;
                    xgs_port_exists = 1;
                }

                
                if (!udh_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_UDH_ETH)) {
                    dpp_pp->udh_port_exists = 1;
                    udh_port_exists = 1;
                }

                if (SOC_IS_JERICHO(unit)) {
                    if (!dpp_pp->rch_port0_in_exists && (header_type_in == SOC_TMC_PORT_HEADER_TYPE_RCH_0) ) {
                        dpp_pp->rch_port0_in_exists = 1;
                    }
                    if (!dpp_pp->rch_port1_in_exists && (header_type_in == SOC_TMC_PORT_HEADER_TYPE_RCH_1) ) {
                        dpp_pp->rch_port1_in_exists = 1;
                    }
                    if (!dpp_pp->rch_port0_out_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_RCH_0) ) {
                        dpp_pp->rch_port0_out_exists = 1;
                    }
                    if (!dpp_pp->rch_port1_out_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_RCH_1) ) {
                        dpp_pp->rch_port1_out_exists = 1;
                    }
                }

          
          if (!dpp_pp->ipv6_with_rpf_2pass_exists)
          {
              dpp_pp->ipv6_fwd_pass2_rcy_local_port = soc_property_suffix_num_get(unit,  -1, spn_CUSTOM_FEATURE, CF_IPV6_UC_WITH_RPF_2PASS_ENABLED, -1);

              if (dpp_pp->ipv6_fwd_pass2_rcy_local_port >= 0)
              {
                  dpp_pp->ipv6_with_rpf_2pass_exists = 1;
              }
          }

                first_header_size = soc_property_port_get(unit, port_i, spn_FIRST_HEADER_SIZE, 0);

                if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED) || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP))
                {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH;
                }
                else if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2) || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP))
                {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH_2;

                    if(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_OAM_STATISTICS_PER_MEP_ENABLED, 0) &&
                        soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, 0))
                    {
						if(( port_i == ARAD_OAMP_PORT_ID) || (!SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) && ( port_i == ARAD_OAMP_PORT_ID + 1)))
                        first_header_size += 2;
                    }
                }
                else if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
                   first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_FRC_PPD;
                }
                else if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_RCH_0) || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_RCH_1)) {
                   first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_RCH;
                }
                else if (is_vendor_pp_port) {
                    first_header_size += SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()?0:SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_VENDOR_PP;
                }
                else if (port_extender_map_lc == 1) {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH_2;
                }
                else if (port_extender_map_lc == 2) {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_COE;
                }

                
                if (is_hg_header && (!((header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS)
                                       || header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ
                                       || header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)))
                {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_HIGIG_FB;
                }

                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.first_header_size.set(unit, port_i, first_header_size);
                SOCDNX_IF_ERR_EXIT(rv);

                if ((pp_port != -1) && !pp_port_configured[pp_port]) {
                    uint8 is_stag_enabled,
                          is_snoop_enabled;
                    uint32 mirror_profile;
                    SOC_TMC_PORTS_FC_TYPE fc_type;

                    is_stag_enabled = soc_property_port_get(unit, port_i, spn_STAG_ENABLE, 0);
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_stag_enabled.set(unit,port_i,is_stag_enabled);
                    SOCDNX_IF_ERR_EXIT(rv);

                    rv = soc_dpp_str_prop_parse_flow_control_type(unit, port_i, &(fc_type));
                    SOCDNX_IF_ERR_EXIT(rv);
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.fc_type.set(unit,port_i,fc_type);
                    SOCDNX_IF_ERR_EXIT(rv);

                    is_snoop_enabled = soc_property_port_get(unit, port_i, spn_SNOOP_ENABLE, 0);
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_snoop_enabled.set(unit,port_i,is_snoop_enabled);
                    SOCDNX_IF_ERR_EXIT(rv);
                    mirror_profile = soc_property_port_get(unit, port_i, spn_MIRROR_PROFILE, 0);
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.mirror_profile.set(unit,port_i,mirror_profile);
                    SOCDNX_IF_ERR_EXIT(rv);

                    pp_port_configured[pp_port] = TRUE;
                }

                if ((header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM)
                    || (header_type_out == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2)) {
                    SOC_DPP_CONFIG(unit)->tm.otmh_header_used = 1;
                }

                 if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU) {
                    SOC_DPP_CONFIG(unit)->tm.l2_encap_external_cpu_used = 1;
                }
            } 

            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &out_port_priority));

            switch (out_port_priority)
            {
                case 1:
                    rv = soc_port_sw_db_local_to_out_port_priority_set(unit,port_i,ARAD_EGR_PORT_ONE_PRIORITY);
                    if (SOC_FAILURE(rv)) {
                         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG_STR( "invalid port %d\n"), port_i));
                    }
                    break;
                case 2:
                    rv = soc_port_sw_db_local_to_out_port_priority_set(unit,port_i,ARAD_EGR_PORT_TWO_PRIORITIES);
                    if (SOC_FAILURE(rv)) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,(_BSL_SOCDNX_MSG_STR( "invalid port %d\n"), port_i));
                    }
                    break;
                case 8:
                    rv = soc_port_sw_db_local_to_out_port_priority_set(unit,port_i,ARAD_EGR_PORT_EIGHT_PRIORITIES);
                    if (SOC_FAILURE(rv)) {
                         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG_STR( "invalid port %d\n"), port_i));
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("out_port_priority error priority is out of valid values")));
            }

            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
            if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_LB_MODEM(flags))) {

                rv = soc_arad_ps_static_mapping(unit, port_i, out_port_priority, &is_static_mapping, &base_q_pair);
                SOCDNX_IF_ERR_EXIT(rv);

                if(!is_static_mapping) {
                    SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_binding_ps(unit, port_i, out_port_priority, 1 , &base_q_pair));
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.set(unit,port_i,base_q_pair);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }

            
            rv = soc_arad_str_prop_parse_otm_port_rate_type(unit, port_i, &shaper_mode);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.shaper_mode.set(unit,port_i,shaper_mode);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else { 
            if (interface != SOC_PORT_IF_ERP) {

                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit,port_i,&header_type_out);
                SOCDNX_IF_ERR_EXIT(rv);
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));

                if (!xgs_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
                    dpp_pp->xgs_port_exists = 1;
                    xgs_port_exists = 1;
                }

                
                if (!udh_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_UDH_ETH)) {
                    dpp_pp->udh_port_exists = 1;
                    udh_port_exists = 1;
                }
                if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM) {
                    SOC_TMC_PORTS_FTMH_EXT_OUTLIF outlif_ext_en;
                    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.get(unit, port_i, &outlif_ext_en);
                    SOCDNX_IF_ERR_EXIT(rv);
                    if(outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE) {
                      SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 1;
                    }
                }
            }
        }
    }

    
    dpp_pp->ivl_inlif_profile = soc_property_suffix_num_get(unit,  -1, spn_CUSTOM_FEATURE, "IVL_inlif_profile", 0);
    
    rv = soc_arad_str_prop_ilkn_counters_mode_get(unit, &(dpp_arad->init.ports.ilkn_counters_mode));
    SOCDNX_IF_ERR_EXIT(rv);


    
    dpp_arad->init.ports.ilkn_first_packet_sw_bypass = 0;
    if(SOC_IS_JERICHO(unit)){ 
        dpp_arad->init.ports.ilkn_first_packet_sw_bypass = soc_property_get(unit, spn_ILKN_FIRST_PACKET_SW_BYPASS, 0);
    }

    
    rv = soc_arad_str_prop_lag_mode_get(unit, &dpp_arad->init.ports.lag_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    
    use_trunk_as_ingress_mc_destination_default = SOC_IS_ARAD_B1_AND_BELOW(unit) ? 0 : 1;
    dpp_arad->init.ports.use_trunk_as_ingress_mc_dest = soc_property_get(unit, spn_USE_TRUNK_AS_INGRESS_MC_DESTINATION, use_trunk_as_ingress_mc_destination_default);

    
    rv = soc_arad_str_prop_synce_mode_get(unit, &dpp_arad->init.synce.mode);
    SOCDNX_IF_ERR_EXIT(rv);

    for (ind = 0; ind < ARAD_NIF_NOF_SYNCE_CLK_IDS ; ind++) {
        if (SOC_IS_JERICHO(unit)) {
            dpp_arad->init.synce.conf[ind].port_id = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_TO_PORT_ID_CLK, "", 0xffffffff);
        } else {
            dpp_arad->init.synce.conf[ind].port_id = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_TO_NIF_ID, "clk_", 0xffffffff);
        }
        if (dpp_arad->init.synce.conf[ind].port_id != 0xffffffff) {
            dpp_arad->init.synce.conf[ind].enable = 0x1;
            dpp_arad->init.synce.enable = 0x1;

            rv = soc_arad_str_prop_synce_clk_div_get(unit, ind, &(dpp_arad->init.synce.conf[ind].clk_divider));
            if (rv != SOC_E_NONE) {
              LOG_INFO(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_arad_str_prop_synce_clk_div_get error\n")));
            }
        }

        dpp_arad->init.synce.conf[ind].squelch_enable = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_SQUELCH_ENABLE, "clk_", 0x0);
    }

    
    dpp_arad->init.fabric.enable = TRUE;

    rv = soc_dpp_str_prop_fabric_ftmh_outlif_extension_get(unit, &dpp_arad->init.fabric.ftmh_extension);
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        if (dpp_arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH) {
            if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)){
                dpp_arad->init.fabric.fabric_mesh_multicast_enable = 1; 
            } else {
                
                dpp_arad->init.fabric.fabric_mesh_multicast_enable = soc_property_get(unit, spn_FABRIC_MESH_MULTICAST_ENABLE, 1);
            }
        } else {
            dpp_arad->init.fabric.fabric_mesh_multicast_enable = 0;
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        dpp_arad->init.fabric.segmentation_enable = TRUE;
        dpp_arad->init.fabric.scheduler_adapt_to_links = FALSE; 
    } else {
        dpp_arad->init.fabric.scheduler_adapt_to_links = SOC_SAND_NUM2BOOL(soc_property_get(unit,spn_SCHEDULER_FABRIC_LINKS_ADAPTATION_ENABLE, FALSE));
        dpp_arad->init.fabric.segmentation_enable = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_FABRIC_SEGMENTATION_ENABLE, TRUE));
    }

    propval = soc_property_get_str(unit, spn_FABRIC_LINKS_TO_CORE_MAPPING_MODE);
    if ((propval == NULL) || (sal_strcmp(propval, "SHARED") == 0) || SOC_IS_QAX(unit)){ 
        dpp_arad->init.fabric.fabric_links_to_core_mapping_mode = SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED;
    } else if (sal_strcmp(propval, "DEDICATED") == 0){
        dpp_arad->init.fabric.fabric_links_to_core_mapping_mode = SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_DEDICATED;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported properties: fabric_links_to_core_mapping_mode should be SHARED or DEDICATED")));
    }

    
    if (SOC_IS_JERICHO(unit)) {
        dpp_arad->init.fabric.is_fe600 = FALSE;
    } else {
        dpp_arad->init.fabric.is_fe600 = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_IS_FE600_IN_SYSTEM,  FALSE));
    }
    dpp_arad->init.fabric.is_128_in_system = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_IS_VCS_128_IN_SYSTEM,  FALSE));
    if (dpp_arad->init.fabric.is_fe600 && !(dpp_arad->init.fabric.is_128_in_system)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported properties: is_fe600=1 & dpp_arad->init.fabric.is_128_in_system=0")));
    }

    
    dpp_arad->caui_fast_recycle = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "caui_fast_recycle", 0);

    
    flags = soc_property_get(unit, spn_TDM_SOURCE_FAP_ID_OFFSET, SOC_DPP_ARAD_DEFAULT_TDM_SOURCE_FAP_ID_OFFSET);
    dpp_arad->tdm_source_fap_id_offset = flags >= ARAD_NOF_FAPS_IN_SYSTEM ? SOC_DPP_ARAD_DEFAULT_TDM_SOURCE_FAP_ID_OFFSET : flags;

    
    SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast_low_priority = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_MULTICAST_LOW,  SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_almost_full_mc_low_prio));
    SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_MULTICAST,  SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_almost_full_mc));
    SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_all = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_ALL,  SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_almost_full_all));

    if (SOC_IS_JERICHO(unit))
    {
        SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_except_tdm       = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_ALL_EXCEPT_TDM,       34);
    }

    
    if(SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast_low_priority > SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_threshold_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("egress_fabric_drop_threshold_multicast_low is out of range")));
    }
    if(SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast > SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_threshold_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("egress_fabric_drop_threshold_multicast is out of range")));
    }
    if(SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_all > SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_threshold_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("egress_fabric_drop_threshold_all is out of range")));
    }

    
    rv = soc_arad_str_prop_system_ftmh_load_balancing_ext_mode_get(unit, &dpp_arad->init.fabric.ftmh_lb_ext_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_arad_str_prop_system_trunk_hash_format_get(unit, &dpp_arad->init.fabric.trunk_hash_format);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (SOC_IS_JERICHO(unit) && !(SOC_DPP_IS_MESH(unit)) && !(SOC_DPP_SINGLE_FAP(unit))) {
        dpp_arad->init.fabric.fabric_pcp_enable = soc_property_get(unit, spn_FABRIC_PCP_ENABLE, 1);
    } else {
        dpp_arad->init.fabric.fabric_pcp_enable = 0;
    }

    
    dpp_arad->init.ports.is_stacking_system = soc_property_get(unit, spn_STACKING_ENABLE, 0x0);
    if ((dpp_arad->init.ports.is_stacking_system) && soc_property_get(unit, spn_OAM_STATISTICS_PER_MEP_ENABLED, 0)) {
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("OAM statistics and STACKING cannot be configured in the same time.")));
    }

    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_statistics_with_stacking", 0) ) {
        if ( !soc_property_suffix_num_get(unit, -1, spn_FIELD_INGRESS_DEFAULT_PGM_LOAD_DISABLE, "xgs", 0) ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("if using oam_statistics_with_stacking, must disable XGS "
                    "PMF program from loading in initialization stage with field_ingress_default_pgm_load_disable_xgs=1\n\r")));
        }
    }

    if ((dpp_arad->init.ports.is_stacking_system == 0x1) || (soc_property_get(unit, spn_STACKING_EXTENSION_ENABLE, 0x0))) {
        dpp_arad->init.fabric.ftmh_stacking_ext_mode = 0x1;
    }
    dpp_arad->init.ports.add_dsp_extension_enable = soc_property_get(unit, spn_FTMH_DSP_EXTENSION_ADD, 0x0);
    dpp_arad->init.ports.pph_learn_extension_disable = soc_property_get(unit, spn_BCM886XX_PPH_LEARN_EXTENSION_DISABLE, 0x0);
    dpp_arad->init.ports.tm_domain = soc_property_get(unit, spn_DEVICE_TM_DOMAIN, 0x0);

    for (port_i = 0; port_i < ARAD_NOF_FAP_PORTS; ++port_i) {
        peer_tm_domain = soc_property_port_get(unit, port_i, spn_PEER_TM_DOMAIN, prop_invalid);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.peer_tm_domain.set(unit,port_i,peer_tm_domain);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    dpp_arad->init.mirror_stamp_sys_dsp_ext = soc_property_get(unit, spn_MIRROR_STAMP_SYS_ON_DSP_EXT, 0x0);

    
    dpp_arad->init.ports.is_system_red = soc_property_get(unit, spn_SYSTEM_RED_ENABLE, 0x0);

    
    rv = soc_arad_str_prop_swap_info_get(unit, &dpp_arad->init.ports.swap_info);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_arad_str_prop_action_type_source_mode_get(unit, &dpp_arad->action_type_source_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    if (!SOC_WARM_BOOT(unit)) {
        
        rv = SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_UNINITIALIZED);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, 0);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    
    rv = soc_arad_str_prop_egress_shared_resources_mode_get(unit, &dpp_arad->init.eg_cgm_scheme);
    SOCDNX_IF_ERR_EXIT(rv);

    dpp_arad->port_egress_recycling_scheduler = soc_property_get(unit, spn_PORT_EGRESS_RECYCLING_SCHEDULER_CONFIGURATION,  0);

    
    rv = soc_dpp_str_prop_xgs_compatible_system_port_mode_get(unit, &dpp_arad->xgs_compatability_tm_system_port_encoding);
    SOCDNX_IF_ERR_EXIT(rv);

    
    dpp_arad->enable_lpm_custom_lookup = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lpm_custom_lookup", 0);

    if (dpp_arad->enable_lpm_custom_lookup) {
        
        if ((SOC_DPP_CONFIG(unit))->pp.fcoe_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("FCoE and LPM custom lookup could not be enabled at the same time")));
        }
    }

    
    SOC_PBMP_CLEAR(pbmp_disabled);
    nof_quads = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    for (quad = SOC_DPP_DEFS_GET(unit, first_fabric_link_id)/4; quad < nof_quads + SOC_DPP_DEFS_GET(unit, first_fabric_link_id)/4; quad++) {
        quad_active = soc_property_suffix_num_get(unit, quad, spn_SERDES_QRTT_ACTIVE, "", 1);
        if (!quad_active) {
            for (quad_index = 0; quad_index < 4; quad_index++) {
                SOC_PBMP_PORT_ADD(pbmp_disabled, FABRIC_LOGICAL_PORT_BASE(unit) + quad*4 + quad_index);
            }
         }
    }


    
    for (fabric_link = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id);
        fabric_link < FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + SOC_DPP_DEFS_GET(unit, nof_fabric_links);
        ++fabric_link) {
        if (PBMP_MEMBER(pbmp_disabled, fabric_link)) {
            DPP_ADD_DISABLED_PORT(sfi, fabric_link);
            DPP_ADD_DISABLED_PORT(port, fabric_link);
            DPP_ADD_DISABLED_PORT(all, fabric_link);
        }
        else {
            PORT_SW_DB_PORT_ADD(sfi, fabric_link);
            PORT_SW_DB_PORT_ADD(port,fabric_link);
            PORT_SW_DB_PORT_ADD(all,fabric_link);
        }
        
        phy_port = SOC_DPP_ARAD_FABRIC_PORT_TO_PHY_PORT(unit, fabric_link);

        blk = SOC_PORT_IDX_INFO(unit, phy_port, 0).blk;
        if (blk < 0) { 
            blktype = 0;
        }
        else
        {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
        }

        si->port_p2l_mapping[phy_port] = fabric_link;
        si->port_l2p_mapping[fabric_link] = phy_port;
        si->port_type[fabric_link] = blktype;
        sal_snprintf(si->port_name[fabric_link], sizeof(si->port_name[fabric_link]),
                         "sfi%d", fabric_link);
        sal_snprintf(si->port_name_alter[fabric_link], sizeof(si->port_name_alter[fabric_link]),
                         "fabric%d", fabric_link - SOC_INFO(unit).fabric_logical_port_base);
        si->port_name_alter_valid[fabric_link] = 1;

        SOC_PBMP_PORT_ADD(si->block_bitmap[blk], fabric_link);
        ++si->port_num;
    }

    SOCDNX_IF_ERR_EXIT(soc_arad_collect_nif_config(unit));

    

    SOCDNX_IF_ERR_EXIT(soc_arad_ocb_enable_mode(unit, &(dpp_arad->init.ocb)));
    SOCDNX_IF_ERR_EXIT(soc_arad_validate_ocb_enable(unit, dpp_arad->init.ocb.ocb_enable));

    dpp_arad->init.ocb.databuffer_size = soc_property_get(unit, spn_BCM886XX_OCB_DATABUFFER_SIZE, 256);
    val = soc_property_get(unit, spn_BCM886XX_OCB_REPARTITION, 0);
    switch(val)
    {
      case 0:
        dpp_arad->init.ocb.repartition_mode = ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST;
        break;
      case 1:
        dpp_arad->init.ocb.repartition_mode = ARAD_OCB_REPARTITION_MODE_ALL_UNICAST;
        break;
      default:
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }

      
      val = soc_property_get(unit, spn_SERDES_NIF_CLK_FREQ, 0);
      switch(val)
      {
          case 0:
              dpp_arad->init.pll.nif_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_125;
              break;
          case 1:
              dpp_arad->init.pll.nif_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_156_25;
              break;
          default:
              SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
      }

      val = soc_property_get(unit, spn_SERDES_FABRIC_CLK_FREQ, 0);
      switch(val)
      {
          case 0:
              dpp_arad->init.pll.fabric_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_125;
              break;
          case 1:
              dpp_arad->init.pll.fabric_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_156_25;
              break;
          default:
              SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
      }

      val = soc_property_get(unit, spn_SYNTH_DRAM_FREQ, 25);
      switch(val)
      {
        case 25:
            dpp_arad->init.pll.synthesizer_clock_freq = ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_25_MHZ;
            break;
        case 125:
            dpp_arad->init.pll.synthesizer_clock_freq = ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_125_MHZ;
            break;
        default:
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
      }

      
      SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit, spn_EXT_RAM_PRESENT, 0, &drams));
      val=drams;
      dpp_arad->init.dram.nof_drams = val;

      SOCDNX_IF_ERR_EXIT(soc_arad_validate_device_num_of_dram(unit, val));

      if (SOC_IS_DPP_DRC_COMBO28(unit) && !SOC_IS_FLAIR(unit)) {
            
            rv = soc_dpp_drc_combo28_info_config_default(unit, &(dpp_arad->init.drc_info));
            SOCDNX_IF_ERR_EXIT(rv);

            
            if (SOC_IS_ARDON(unit)) {
                
                switch (val) {
                case 0:
                    break;
                case 1:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                case 2:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                case 3:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                case 4:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Ardon: Unsupported number of external dram interfaces (%d)"), val));
                }

                
                SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 0);
                SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 2);
                SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 3);

                
                dpp_arad->init.drc_info.zq_calib_map[0] = 1;
                dpp_arad->init.drc_info.zq_calib_map[1] = 1;
                dpp_arad->init.drc_info.zq_calib_map[2] = 3;
                dpp_arad->init.drc_info.zq_calib_map[3] = 3;

            } else if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
                dram_bitmap = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dram_bitamp", 0x0);
                if (dram_bitmap != 0) {
                    SHR_BITCOPY_RANGE(dpp_arad->init.drc_info.dram_bitmap, 0, &dram_bitmap, 0, SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max));
                } else {

                    
                    switch (val) {
                    case 0:
                        break;
                    case 2:
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                        break;
                    case 3:
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                        break;
                    case 41:
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                        break;
                    case 42:
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 5);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 7);
                        break;
                    case 6:
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 4);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 5);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 7);
                        break;
                    case 8:
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 3);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 4);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 5);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 6);
                        SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 7);
                        break;
                    default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Jericho: Unsupported number of external dram interfaces (%d)"), val));
                    }

                    
                    SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 0);
                    SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 2);
                    SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 5);
                    SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 7);

                
                dpp_arad->init.drc_info.zq_calib_map[0] = 2;
                dpp_arad->init.drc_info.zq_calib_map[1] = 2;
                dpp_arad->init.drc_info.zq_calib_map[2] = 2;
                dpp_arad->init.drc_info.zq_calib_map[3] = 2;
                dpp_arad->init.drc_info.zq_calib_map[4] = 5;
                dpp_arad->init.drc_info.zq_calib_map[5] = 5;
                dpp_arad->init.drc_info.zq_calib_map[6] = 5;
                dpp_arad->init.drc_info.zq_calib_map[7] = 5;

                }
            } else if( SOC_IS_QAX(unit)) {
                
                switch (val) {
                case 0:
                    break;
                case 1:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                    break;
                case 21:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                    break;
                case 22:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                    break;
                case 3:
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 0);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 1);
                    SHR_BITSET(dpp_arad->init.drc_info.dram_bitmap, 2);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("QAX: Unsupported number of external dram interfaces (%d)"), val));
                }

                
                SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 0);
                SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 1);
                SHR_BITSET(dpp_arad->init.drc_info.ref_clk_bitmap, 2);

                
                dpp_arad->init.drc_info.zq_calib_map[0] = 0;
                dpp_arad->init.drc_info.zq_calib_map[1] = 0;
                dpp_arad->init.drc_info.zq_calib_map[2] = 2;
            } else {
                SHR_BITCOPY_RANGE(dpp_arad->init.drc_info.dram_bitmap, 0, (uint32*)&val, 0, SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max));
            }

           
           SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_info_config(unit, &(dpp_arad->init.drc_info)));

           
           dpp_arad->init.drc_info.device_core_freq = dpp_arad->init.core_freq.frequency / 1000;
           if ((SOC_DPP_CONFIG(unit)->emulation_system)
#if defined(PLISIM)
            || (SAL_BOOT_PLISIM)
#endif
               )
           {
               dpp_arad->init.drc_info.sim_system_mode = 1;
           }

           if (SOC_IS_ARDON(unit)) {
               
               dpp_arad->init.dram.enable = dpp_arad->init.drc_info.enable;
           }

      } else {

          if (val == 0) {
              
              dpp_arad->init.dram.enable = FALSE;
          } else {
              dpp_arad->init.dram.enable = TRUE;

            switch (val) {
              case 1:
                if ((dpp_arad->init.ocb.ocb_enable == OCB_DISABLED) || (dpp_arad->init.ocb.ocb_enable == OCB_ENABLED)) {
                    
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported number of external dram interfaces (%d). Not one-way-bypass mode"), val));
                }
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = FALSE;
                dpp_arad->init.dram.is_valid[2] = FALSE;
                dpp_arad->init.dram.is_valid[3] = FALSE;
                dpp_arad->init.dram.is_valid[4] = FALSE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 2:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = FALSE;
                dpp_arad->init.dram.is_valid[2] = FALSE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = FALSE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 3:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = TRUE;
                dpp_arad->init.dram.is_valid[3] = FALSE;
                dpp_arad->init.dram.is_valid[4] = FALSE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 4:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = FALSE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = TRUE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 6:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = TRUE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = TRUE;
                dpp_arad->init.dram.is_valid[5] = TRUE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 8:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = TRUE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = TRUE;
                dpp_arad->init.dram.is_valid[5] = TRUE;
                dpp_arad->init.dram.is_valid[6] = TRUE;
                dpp_arad->init.dram.is_valid[7] = TRUE;
                break;
              default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported number of external dram interfaces (%d)"), val));
            }

        
          {
              int drams[8] = {SOC_BLK_DRCA, SOC_BLK_DRCB, SOC_BLK_DRCC, SOC_BLK_DRCD, SOC_BLK_DRCE, SOC_BLK_DRCF, SOC_BLK_DRCG, SOC_BLK_DRCH};

              for(i=0; i<8; ++i) {
                  if(dpp_arad->init.dram.enable == FALSE || dpp_arad->init.dram.is_valid[i] == FALSE) {
                      SOC_BLOCK_ITER(unit, val, drams[i]){
                         SOC_INFO(unit).block_valid[val]= FALSE;
                      }
                  }
              }
          }

          rv = soc_dpp_str_prop_ext_ram_type(unit, &dpp_arad->init.dram.dram_type);
          SOCDNX_IF_ERR_EXIT(rv);

          dpp_arad->init.dram.pll_conf.m = soc_property_get(unit, spn_EXT_QDR_PLL_M, 0);
          dpp_arad->init.dram.pll_conf.n = soc_property_get(unit, spn_EXT_QDR_PLL_N, 0);
          dpp_arad->init.dram.pll_conf.p = soc_property_get(unit, spn_EXT_QDR_PLL_P, 0);

          dpp_arad->init.dram.conf_mode = ARAD_HW_DRAM_CONF_MODE_PARAMS;
          dpp_arad->init.dram.dram_conf.params_mode.dram_freq = soc_property_get(unit, spn_EXT_RAM_FREQ, 0);
          if (dpp_arad->init.dram.dram_conf.params_mode.dram_freq < 500){
              SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ERROR PLL setup does not support dram freq below 500Mhz")));
          }

          rv = soc_dpp_prop_parse_dram_number_of_columns(unit, &dpp_arad->init.dram.nof_columns);
          SOCDNX_IF_ERR_EXIT(rv);
          dpp_arad->init.dram.dram_conf.params_mode.params.nof_cols = dpp_arad->init.dram.nof_columns;

          rv = soc_arad_prop_parse_dram_number_of_rows(unit,&dpp_arad->init.dram.nof_rows);
          SOCDNX_IF_ERR_EXIT(rv);

          rv = soc_dpp_prop_parse_dram_number_of_banks(unit, &dpp_arad->init.dram.nof_banks);
          SOCDNX_IF_ERR_EXIT(rv);
          dpp_arad->init.dram.dram_conf.params_mode.params.nof_banks = dpp_arad->init.dram.nof_banks;

#ifdef BCM_DDR3_SUPPORT
          SOC_DDR3_NUM_MEMORIES(unit)          = dpp_arad->init.dram.nof_drams * 2;
          SOC_DDR3_CLOCK_MHZ(unit)             = dpp_arad->init.dram.dram_conf.params_mode.dram_freq;
          SOC_DDR3_NUM_COLUMNS(unit)           = soc_property_get(unit,spn_EXT_RAM_COLUMNS, 1024);
          SOC_DDR3_NUM_ROWS(unit)              = dpp_arad->init.dram.nof_rows;
          SOC_DDR3_NUM_BANKS(unit)             = dpp_arad->init.dram.dram_conf.params_mode.params.nof_banks;
          SOC_DDR3_MEM_GRADE(unit)             = soc_property_get(unit, spn_DDR3_MEM_GRADE, 0x111111);
          SOC_DDR3_OFFSET_WR_DQ_CI02_WL0(unit) = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                             "ddr3_offset_we_dq_ci02_wl0", SHMOO_CI02_WL0_OFFSET_WR_DQ);
          SOC_DDR3_OFFSET_WR_DQ_CI00_WL1(unit)  = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                             "ddr3_offset_we_dq_ci00_wl1", SHMOO_CI0_WL1_OFFSET_WR_DQ);
#endif

          rv = soc_dpp_prop_parse_dram_ap_bit_pos(unit, &dpp_arad->init.dram.dram_conf.params_mode.params.ap_bit_pos);
          SOCDNX_IF_ERR_EXIT(rv);

          rv = soc_dpp_prop_parse_dram_burst_size(unit, &dpp_arad->init.dram.dram_conf.params_mode.params.burst_size);
          SOCDNX_IF_ERR_EXIT(rv);

          dpp_arad->init.dram.dram_conf.params_mode.params.auto_mode = TRUE;
          dpp_arad->init.dram.dram_conf.params_mode.params.c_cas_latency = soc_property_get(unit, spn_EXT_RAM_C_CAS_LATENCY, prop_invalid);
          dpp_arad->init.dram.dram_conf.params_mode.params.c_wr_latency = soc_property_get(unit, spn_EXT_RAM_C_WR_LATENCY, prop_invalid);
          dpp_arad->init.dram.dram_conf.params_mode.params.t_rc =  soc_property_get(unit, spn_EXT_RAM_T_RC, prop_invalid);
          dpp_arad->init.dram.dram_conf.params_mode.params.jedec =  soc_property_get(unit, spn_EXT_RAM_JEDEC, prop_invalid);

          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RFC, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rfc);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RAS, &dpp_arad->init.dram.dram_conf.params_mode.params.t_ras);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_FAW, &dpp_arad->init.dram.dram_conf.params_mode.params.t_faw);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RCD_RD, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rcd_rd);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RCD_WR, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rcd_wr);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RRD, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rrd);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_REF, &dpp_arad->init.dram.dram_conf.params_mode.params.t_ref);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RP, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rp);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_WR, &dpp_arad->init.dram.dram_conf.params_mode.params.t_wr);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_WTR, &dpp_arad->init.dram.dram_conf.params_mode.params.t_wtr);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RTP, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rtp);
          SOCDNX_IF_ERR_EXIT(rv);

          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs0_wr1, spn_EXT_RAM_DDR3_MRS0_WR1);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs0_wr2, spn_EXT_RAM_DDR3_MRS0_WR2);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs1_wr1, spn_EXT_RAM_DDR3_MRS1_WR1);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs2_wr1, spn_EXT_RAM_DDR3_MRS2_WR1);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs3_wr1, spn_EXT_RAM_DDR3_MRS3_WR1);

        }

        
        dpp_arad->init.dram.bist_enable = soc_property_suffix_num_get(unit, -1, spn_BIST_ENABLE, "dram", 0x1);

        
        val = soc_property_get(unit, spn_DRAM_CRC_DEL_BUFFER_MAX_RECLAIMS, 0x0);


        if(SOC_SAND_FAILURE(arad_dram_crc_del_buffer_max_reclaims_set_unsafe((uint32)unit, (uint32)val))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("arad_dram_crc_del_buffer_max_reclaims_set_unsafe error")));
        }

        
        for(ind = 0; ind < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++ind) {
          if(soc_property_port_get(unit, ind, spn_DRAM0_CLAMSHELL_ENABLE, FALSE))
          {
            if(soc_property_port_get(unit, ind, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("only one of the dram should be enabled")));
            }
            dpp_arad->init.dram.dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DRAM_0;
          }
          else if(soc_property_port_get(unit, ind, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
            dpp_arad->init.dram.dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DRAM_1;
          }
          else
          {
            dpp_arad->init.dram.dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DISABLED;
          }
        }
      }

    
    dpp_arad->init.dynamic_port_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dynamic_port", FALSE);


    
     SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_external_voltage_mode_get(unit, &(dpp_arad->init.ex_vol_mod) ));


    
    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit, spn_STAT_IF_ENABLE, FALSE, &stat_if));
    dpp_arad->init.stat_if.stat_if_enable = stat_if;
    dpp_arad->init.stat_if.stat_if_etpp_mode = soc_property_get(unit, spn_STAT_IF_ETPP_MODE, -1);
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        soc_port_t port;

        dpp_arad->init.stat_if.stat_if_enable = FALSE;
        for(port=0; port<SOC_MAX_NUM_PORTS ; port++){
            dpp_arad->init.stat_if.stat_if_info[0].stat_if_port_enable[port] =
                                                soc_property_suffix_num_get(unit, port, spn_STAT_IF_ENABLE, "", 0x0);
            dpp_arad->init.stat_if.stat_if_enable |= dpp_arad->init.stat_if.stat_if_info[0].stat_if_port_enable[port];
        }
    }
    
    
    for(i=0; i < SOC_TMC_STAT_IF_COUNTERS_COMMANDS; i++)
    {
        propval = soc_property_port_get_str(unit, i, spn_STAT_IF_ETPP_COUNTER_MODE);
        if (propval != NULL)
        {
            if(sal_strcmp(propval, "EGRESS_VSI") == 0)
            {
                dpp_arad->init.stat_if.stat_if_info[0].if_etpp_counter_mode[i] = STAT_IF_ETPP_COUNTERS_MODE_VSI;
            }
            else if(sal_strcmp(propval, "EGRESS_LIF") == 0)
            {
                dpp_arad->init.stat_if.stat_if_info[0].if_etpp_counter_mode[i] = STAT_IF_ETPP_COUNTERS_MODE_LIF;
            }
            else if(sal_strcmp(propval, "EGRESS_PORT") == 0)
            {
                dpp_arad->init.stat_if.stat_if_info[0].if_etpp_counter_mode[i] = STAT_IF_ETPP_COUNTERS_MODE_PORT;
            }
            else
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("wrong 'stat_if_etpp_counter_mode' value")));
            }
        }
    }

    
    dpp_arad->init.stat_if.stat_if_info[0].rate = soc_property_suffix_num_get(unit, 0, spn_STAT_IF_RATE, "core", 0x0);
    dpp_arad->init.stat_if.stat_if_info[1].rate = soc_property_suffix_num_get(unit, 1, spn_STAT_IF_RATE, "core", 0x0);

    if (dpp_arad->init.stat_if.stat_if_enable) {
        if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))
        {
            rv = soc_arad_str_prop_stat_if_core_mode_get(unit, &(dpp_arad->init.stat_if.core_mode));
            SOCDNX_IF_ERR_EXIT(rv);
        }
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id)
        {
            
            dpp_arad->init.stat_if.stat_if_info[core_id].mode = SOC_TMC_STAT_IF_REPORT_MODE_NOT_CONFIGURE;
            rv = soc_arad_str_prop_stat_if_report_mode_get(unit, core_id, dpp_arad->init.stat_if.core_mode, &(dpp_arad->init.stat_if.stat_if_info[core_id].mode));
            SOCDNX_IF_ERR_EXIT(rv);
            if (dpp_arad->init.stat_if.stat_if_info[core_id].mode != SOC_TMC_STAT_IF_REPORT_MODE_NOT_CONFIGURE)
            {
                stat_if_nof_report_mode_configure++;
            }
            else
            {
                continue;
            }

            dpp_arad->init.stat_if.stat_if_info[core_id].is_idle_reports_present = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_IDLE_REPORTS_PRESENT, "core", TRUE);
            

            if (SOC_IS_JERICHO(unit))
            {
                rv = soc_arad_prop_stat_if_reports_per_packet_get(unit, core_id ,&(dpp_arad->init.stat_if.stat_if_info[core_id].if_nof_reports_per_packet));
                SOCDNX_IF_ERR_EXIT(rv);
            }

            if (dpp_arad->init.stat_if.stat_if_info[core_id].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING ||
                dpp_arad->init.stat_if.stat_if_info[core_id].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS ||
                (SOC_IS_ARAD_B0_AND_ABOVE(unit) && SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER == dpp_arad->init.stat_if.stat_if_info[core_id].mode))
            {

                
                if (SOC_IS_JERICHO(unit))
                {
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_billing_ingress_queue_stamp = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_BILLING_INGRESS_QUEUE_STAMP_ENABLE, "core", FALSE);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_billing_ingress_drop_reason = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_BILLING_INGRESS_DROP_REASON_ENABLE, "core", FALSE);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_billing_filter_reports_ingress = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_BILLING_FILTER_REPORTS, "ingress_core", FALSE);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_billing_filter_reports_egress = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_BILLING_FILTER_REPORTS, "egress_core", FALSE);
                }

                
                else
                {
                    rv = soc_arad_str_prop_stat_if_billing_pkt_size_get(unit, &(dpp_arad->init.stat_if.stat_if_info[core_id].if_pkt_size));
                    SOCDNX_IF_ERR_EXIT(rv);
                }

                dpp_arad->init.stat_if.stat_if_info[core_id].if_report_mc_once = soc_property_get(unit, spn_STAT_IF_REPORT_MULTICAST_SINGLE_COPY, TRUE);
            }
            else if (dpp_arad->init.stat_if.stat_if_info[core_id].mode == SOC_TMC_STAT_IF_REPORT_QSIZE)
            {
                
                if (SOC_IS_JERICHO(unit))
                {
                    rv = soc_arad_prop_stat_if_report_size_get(unit, core_id ,&(dpp_arad->init.stat_if.stat_if_info[core_id].report_size));
                    SOCDNX_IF_ERR_EXIT(rv);

                    dpp_arad->init.stat_if.stat_if_info[core_id].if_qsize_queue_min = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MIN, "core", ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MIN);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_qsize_queue_max = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MAX, "core",
                                                                                                                   (SOC_IS_QUX(unit) ? QUX_REPORT_QUEUE_SIZE_MAX :
                                                                                                                   (SOC_IS_QAX(unit) ? QAX_REPORT_QUEUE_SIZE_MAX : JER_STAT_IF_REPORT_QSIZE_QUEUE_MAX)));
                }
                
                else
                {
                    rv = soc_arad_str_prop_stat_if_qsize_pkt_size_get(unit, &(dpp_arad->init.stat_if.stat_if_info[0].if_pkt_size));
                    SOCDNX_IF_ERR_EXIT(rv);
                }

                dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_queue_min          = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_SCRUBBER_QUEUE_MIN, "core",
                                                                                                                           (SOC_IS_QUX(unit) ? QUX_REPORT_QUEUE_SIZE_MAX :
                                                                                                                           (SOC_IS_QAX(unit) ? QAX_REPORT_QUEUE_SIZE_MAX : ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN)));
                dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_queue_max          = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_SCRUBBER_QUEUE_MAX, "core",
                                                                                                                           (SOC_IS_QUX(unit) ? QUX_REPORT_QUEUE_SIZE_MAX :
                                                                                                                           (SOC_IS_QAX(unit) ? QAX_REPORT_QUEUE_SIZE_MAX : ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN)));
                dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_rate_min           = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_SCRUBBER_RATE_MIN, "core", ARAD_STAT_IF_REPORT_SCRUBBER_DISABLE);
                dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_rate_max           = soc_property_suffix_num_get(unit, core_id, spn_STAT_IF_SCRUBBER_RATE_MAX, "core", ARAD_STAT_IF_REPORT_SCRUBBER_DISABLE);

                for(ind = 0; ind < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; ++ind) {
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_buffer_descr_th[ind] = soc_property_port_suffix_num_get(unit, ind, core_id, spn_STAT_IF_SCRUBBER_BUFFER_DESCR_TH,"core", ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_bdb_th[ind] = soc_property_port_suffix_num_get(unit, ind, core_id, spn_STAT_IF_SCRUBBER_BDB_TH, "core", ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_uc_dram_buffer_th[ind]  = soc_property_port_suffix_num_get(unit, ind, core_id, spn_STAT_IF_SCRUBBER_UC_DRAM_BUFFER_TH, "core", ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_sram_buffers_th[ind] = soc_property_port_suffix_num_get(unit, ind, core_id, spn_STAT_IF_SCRUBBER_SRAM_BUFFERS_TH, "core", ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
                    dpp_arad->init.stat_if.stat_if_info[core_id].if_scrubber_sram_pdbs_th[ind]  = soc_property_port_suffix_num_get(unit, ind, core_id, spn_STAT_IF_SCRUBBER_SRAM_PDBS_TH, "core", ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
                }
            }
        }
        if (stat_if_nof_report_mode_configure == 0)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("When working with statistics interface, report mode must be configured at least to one of the cores \n")));
        }
        dpp_arad->init.stat_if.stat_if_info[0].if_report_original_pkt_size = soc_property_get(unit, spn_STAT_IF_REPORT_ORIGINAL_PKT_SIZE, FALSE);

#ifdef BCM_88650_B0
        if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && (!SOC_IS_JERICHO(unit))) {
            dpp_arad->init.stat_if.stat_if_info[0].if_qsize_queue_min          = soc_property_get(unit, spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MIN, ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MIN);
            dpp_arad->init.stat_if.stat_if_info[0].if_qsize_queue_max          = soc_property_get(unit, spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MAX, ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MAX);
        }
#endif

        if (SOC_IS_ARDON(unit))
        {
            dpp_arad->init.stat_if.stat_if_info[0].if_report_enqueue_enable = soc_property_get(unit, spn_STAT_IF_REPORT_ENQUEUE_ENABLE, 1);
            dpp_arad->init.stat_if.stat_if_info[0].if_report_dequeue_enable = soc_property_get(unit, spn_STAT_IF_REPORT_DEQUEUE_ENABLE, 1);
        }

    }

    for(ind = 0; ind<SOC_TMC_FC_NOF_OOB_IDS; ind++) {
        val = soc_property_port_get(unit, ind, spn_FC_OOB_TX_FREQ_RATIO, 4);
        switch (val)
        {
        case 2:
          dpp_arad->init.fc.oob_tx_speed[ind] = ARAD_FC_OOB_TX_SPEED_CORE_2;
          break;
        case 8:
          dpp_arad->init.fc.oob_tx_speed[ind] = ARAD_FC_OOB_TX_SPEED_CORE_8;
          break;
        case 4:
        default:
          dpp_arad->init.fc.oob_tx_speed[ind] = ARAD_FC_OOB_TX_SPEED_CORE_4;
        }
      }

      
      for (ind = 0; ind < SOC_TMC_FC_NOF_OOB_IDS; ind++) {
        dpp_arad->init.fc.fc_oob_type[ind] = soc_property_port_get(unit, ind, spn_FC_OOB_TYPE, 0);

        dpp_arad->init.fc.fc_directions[ind] = soc_property_port_get(unit, ind, spn_FC_OOB_MODE, 0);
        dpp_arad->init.fc.fc_directions[ind] &= (SOC_DPP_FC_CAL_MODE_RX_ENABLE | SOC_DPP_FC_CAL_MODE_TX_ENABLE);

        dpp_arad->init.fc.fc_oob_calender_length[ind][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_LENGTH, "rx", 0);
        dpp_arad->init.fc.fc_oob_calender_length[ind][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_LENGTH, "tx", 0);

        dpp_arad->init.fc.fc_oob_calender_rep_count[ind][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_REP_COUNT, "rx", 0);
        dpp_arad->init.fc.fc_oob_calender_rep_count[ind][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_REP_COUNT, "tx", 0);

      }

      dpp_arad->init.fc.cl_sch_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cl_scheduler_fc", 0);


    

    if (dpp_arad->init.pp_enable) {

      dpp_arad->pp_op_mode.authentication_enable = FALSE;
      dpp_arad->pp_op_mode.hairpin_enable = TRUE;
      dpp_arad->pp_op_mode.system_vsi_enable = FALSE;
      dpp_arad->pp_op_mode.mpls_info.mpls_ether_types[0] = 0x8847;
      dpp_arad->pp_op_mode.mpls_info.mpls_ether_types[1] = 0x8848;
      dpp_arad->pp_op_mode.ipv4_info.nof_vrfs = SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs;
      dpp_arad->pp_op_mode.ipv4_info.flags = ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE|ARAD_PP_MGMT_IPV4_SHARED_ROUTES_MEMORY|ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG;
      dpp_arad->pp_op_mode.ipv4_info.pvlan_enable = FALSE;
      dpp_arad->pp_op_mode.mim_enable = FALSE;

      for (ind = 0; ind < dpp_arad->pp_op_mode.ipv4_info.nof_vrfs; ++ind)
      {
        if (ind == 0 )
        {
          dpp_arad->pp_op_mode.ipv4_info.max_routes_in_vrf[ind] = soc_property_get(unit,spn_IPV4_NUM_ROUTES,50000);
        }
        else
        {
          dpp_arad->pp_op_mode.ipv4_info.max_routes_in_vrf[ind] = 0;
        }
      }

      

      
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[0] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE1_SIZE,14);
      if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[0] != 14) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("LPM first slices size has to be 14")));
      }

      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[1] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE2_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[2] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE3_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[3] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE4_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[4] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE5_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[5] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE6_SIZE,0);

      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase_valid = 0;

      
      for (ind = 1; ind < ARAD_PP_MGMT_IPV4_LPM_BANKS; ++ind) {
          if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] != 0){
              dpp_arad->pp_op_mode.ipv4_info.bits_in_phase_valid = 1;
              break;
          }
      }

      ip_lpm_total = dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[0];
      
      if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase_valid == 1){
          for (ind = 1; ind < ARAD_PP_MGMT_IPV4_LPM_BANKS; ++ind) {
              if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] != 0){
                  if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] < 3 ||
                     dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] > 7
                    ) {
                      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Incorrect slice size for LPM config")));
                  }
                  ip_lpm_total += dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind];
              }
          }

          
          if(ip_lpm_total != 32 + soc_sand_log2_round_up(dpp_arad->pp_op_mode.ipv4_info.nof_vrfs)) {
              SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Incorrect slice size for LPM config")));
          }
      }

      dpp_arad->pp_op_mode.authentication_enable = (soc_property_get(unit, spn_SA_AUTH_ENABLED, 0) == 1 ? TRUE : FALSE);

      auxiliary_table_mode = soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0);

      if ((auxiliary_table_mode == 1) && SOC_IS_JERICHO(unit))
      {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Split Horizon mode is not supported by bcm886xx_auxiliary_table_mode on Jericho. Please use 'split_horizon_forwarding_groups_mode' instead.")));
      }
      dpp_pp->pvlan_enable = FALSE;

      if (auxiliary_table_mode == AUXI_TBL_MODE_PVLAN) {
          dpp_arad->pp_op_mode.split_horizon_filter_enable = FALSE;
          dpp_arad->pp_op_mode.ipv4_info.pvlan_enable = TRUE;
          dpp_pp->pvlan_enable = TRUE;
      }
      else if (auxiliary_table_mode == AUXI_TBL_MODE_S_H) {
          dpp_arad->pp_op_mode.split_horizon_filter_enable = TRUE;
      }
      else if (auxiliary_table_mode == AUXI_TBL_MODE_MIM) {
          
          if (dpp_arad->pp_op_mode.authentication_enable) {
            
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
          }
          dpp_arad->pp_op_mode.split_horizon_filter_enable = FALSE;
          dpp_arad->pp_op_mode.mim_enable = TRUE;
      }

      
      dpp_arad->pp_op_mode.mpls_info.fast_reroute_labels_enable = (soc_property_get(unit, spn_FAST_REROUTE_LABELS_ENABLE, 0) == 1 ? TRUE : FALSE);
      dpp_arad->pp_op_mode.mpls_info.mpls_termination_label_index_enable = (soc_property_get(unit, spn_MPLS_TERMINATION_LABEL_INDEX_ENABLE, 0) == 1 ? TRUE : FALSE);

      rv = soc_dpp_str_prop_parse_mpls_context(unit,&(dpp_arad->pp_op_mode.mpls_info.lookup_include_inrif),&include_port,&(dpp_arad->pp_op_mode.mpls_info.lookup_include_vrf));
      SOCDNX_IF_ERR_EXIT(rv);

      rv = soc_arad_str_prop_parse_mpls_termination_database_mode(unit,dpp_arad->pp_op_mode.mpls_info.mpls_termination_label_index_enable,
                                                                  dpp_arad->pp_op_mode.mpls_info.lookup_include_inrif,
                                                                  dpp_pp->mpls_databases,dpp_pp->mpls_namespaces);
      SOCDNX_IF_ERR_EXIT(rv);


      
      if (SOC_DPP_CONFIG(unit)->trill.mode && (dpp_pp->vlan_match_db_mode != SOC_DPP_VLAN_DB_MODE_DEFAULT)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Trill application and DB mode (%d) cant be both set"), dpp_pp->vlan_match_db_mode));
      }

      
      if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable &&
          SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("UDP tunnel and FCOE do not co exist on same device")));
      }

      
      if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable &&
          (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("UDP tunnel and VXLAN do not co exist on same device")));
      }

      
      if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable &&
          (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("UDP tunnel and BFD multi-hop do not co exist on same device")));
      }

      
       if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable &&
          !(SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("UDP tunnel is supported only with bcm886xx_ip4_tunnel_termination_mode=6")));
      }

      
      if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) &&
          dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN | SOC_DPP_IP_TUNNEL_TERM_DB_ETHER)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP applications L2oIP only one mode of VXLAN, NVGRE and EoIP can be enabled. Bimtap enabled 0x%x"), dpp_pp->ipv4_tunnel_term_bitmap_enable));
      }

      if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) &&
          dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE | SOC_DPP_IP_TUNNEL_TERM_DB_ETHER)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP applications L2oIP only one mode of VXLAN, NVGRE and EoIP can be enabled. Bimtap enabled 0x%x"), dpp_pp->ipv4_tunnel_term_bitmap_enable));
      }

      if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_ETHER) &&
          dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE | SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP applications L2oIP only one mode of VXLAN, NVGRE and EoIP can be enabled.")));
      }

      if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.mpls_info.fast_reroute_labels_enable && dpp_pp->evb_enable) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("EVB and FRR do not co exist on same device")));
      }

      if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->pp_op_mode.mpls_info.fast_reroute_labels_enable && dpp_pp->explicit_null_support) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Explicit NULL and FRR do not co exist on same device because of VT port profile resources")));
      }

      if (SOC_IS_ARADPLUS_AND_BELOW(unit) && dpp_pp->evb_enable && dpp_pp->explicit_null_support) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Explicit NULL and EVB do not co exist on same device because of VT port profile resources")));
      }

      if (dpp_pp->pon_application_enable && dpp_pp->evb_enable) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("EVB and PON do not co exist on same device")));
      }

      if (dpp_pp->pon_application_enable && SOC_DPP_CONFIG(unit)->trill.mode) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("PON and Trill do not co exist on same device")));
      }

      if (((dpp_pp->explicit_null_support && SOC_IS_ARADPLUS_AND_BELOW(unit)) || dpp_pp->gal_support) && (!dpp_pp->tunnel_termination_in_tt_only))  {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("gal and explicit NULL supported only in case tunnel termination is done only in TT stage")));
      }

      
      if ((dpp_pp->l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV4 || dpp_pp->l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IP)
          && SOC_DPP_CONFIG(unit)->trill.mode) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Trill and L3 PON Source bind do not co exist on same device")));
      }

      
      dpp_arad->pp_op_mode.p2p_info.mim_vsi = (SOC_IS_JERICHO_A0(unit) | SOC_IS_JERICHO_B0(unit) | SOC_IS_QMX(unit)) ? 0x7fff: 0xffff;

      
      dpp_pp->min_egress_encap_ip_tunnel_range = soc_property_get(unit, spn_EGRESS_ENCAP_IP_TUNNEL_RANGE_MIN, 0x1001);
      dpp_pp->max_egress_encap_ip_tunnel_range = soc_property_get(unit, spn_EGRESS_ENCAP_IP_TUNNEL_RANGE_MAX, 0x2000);

      SOCDNX_IF_ERR_EXIT(soc_dpp_str_prop_parse_logical_port_mim(unit, &dpp_pp->mim_global_lif_ndx, &dpp_pp->mim_global_out_ac));

      
      dpp_pp->nof_ive_action_ids = SOC_PPC_NOF_INGRESS_VLAN_EDIT_ACTION_IDS_ARAD;
      dpp_pp->nof_ive_reserved_action_ids = SOC_PPC_NOF_INGRESS_VLAN_EDIT_RESERVED_ACTION_IDS_ARAD;
      dpp_pp->nof_eve_action_mappings = SOC_PPC_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS(unit);
      dpp_pp->nof_eve_action_ids = SOC_PPC_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit);
      dpp_pp->nof_eve_reserved_action_ids = SOC_PPC_NOF_EGRESS_VLAN_EDIT_RESERVED_ACTION_IDS_ARAD;
    }
    


    
    dpp_pp->default_trap_strength = soc_property_get(unit, spn_DEFAULT_TRAP_STRENGTH, 4);
    dpp_pp->default_snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 1);

    
    dpp_arad->init.pll.bs_clk_mode = soc_property_get(unit, spn_BROADSYNC_ENABLE_CLK, 0x0);

    
    dpp_arad->init.pll.ts_clk_mode = soc_property_get(unit, spn_PHY_1588_DPLL_FREQUENCY_LOCK, 0x0);
    dpp_arad->init.pll.ts_pll_phase_initial_lo = soc_property_get(unit, spn_PHY_1588_DPLL_PHASE_INITIAL_LO, 0x40000000);
    dpp_arad->init.pll.ts_pll_phase_initial_hi = soc_property_get(unit, spn_PHY_1588_DPLL_PHASE_INITIAL_HI, 0x10000000);

    
    for (i = 0; i < SOC_PPC_EG_ENCAP_NOF_BANKS(unit); i++) {
        bank_phase = soc_property_port_get(unit, i, spn_EGRESS_ENCAP_BANK_PHASE, 0);
        if ((bank_phase == egress_encap_bank_phase_static_5 || bank_phase == egress_encap_bank_phase_static_6) && SOC_IS_JERICHO_AND_BELOW(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No support for EEDB phases 5 and 6 on this device.")));
        }
        dpp_pp->egress_encap_bank_phase[i] = bank_phase;
    }

    
    
    _dflt_tm_pp_port_map[unit] = FALSE;

    

#ifdef BCM_SBUSDMA_SUPPORT
        SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DCMN_MAX_SBUSDMA_CHANNELS;
        SOC_CONTROL(unit)->tdma_ch              = SOC_DCMN_TDMA_CHANNEL;
        SOC_CONTROL(unit)->tslam_ch             = SOC_DCMN_TSLAM_CHANNEL;
        SOC_CONTROL(unit)->desc_ch              = SOC_DCMN_DESC_CHANNEL;
        
        SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit,
            soc_property_get(unit, spn_MEM_CLEAR_CHUNK_SIZE, SOC_DCMN_MEM_CLEAR_CHUNK_SIZE));
#endif

#ifdef BCM_ARAD_SUPPORT
      dpp_pp->roo_enable = (soc_property_get(unit, spn_BCM886XX_ROO_ENABLE, 0));
#endif

      
      dpp_pp->routing_preserve_dscp = 0;
      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "preserving_dscp_enabled", 0)) {
          dpp_pp->routing_preserve_dscp |= SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN;
      }

      if ((val = soc_property_get(unit, spn_LOGICAL_PORT_ROUTING_PRESERVE_DSCP, 0)) != 0) {
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              _bsl_warn(_BSL_SOCDNX_MSG("Warning: logical_port_routing_preserve_dscp does not support on ARAD+, ignored"));
          }
          else {
              if (val == 1) { 
                  dpp_pp->routing_preserve_dscp |= SOC_DPP_DSCP_PRESERVE_INLIF_CTRL_EN;

                  if (!(dpp_pp->routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN)) {
                      dpp_pp->routing_preserve_dscp |= SOC_DPP_DSCP_PRESERVE_OUTLIF_CTRL_EN;
                  }
              }
              else { 
                  dpp_pp->routing_preserve_dscp = val & ((SOC_DPP_DSCP_PRESERVE_OUTLIF_SEL_MASK << SOC_DPP_DSCP_PRESERVE_OUTLIF_SEL_SHIFT)
                    | (SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN | SOC_DPP_DSCP_PRESERVE_OUTLIF_CTRL_EN | SOC_DPP_DSCP_PRESERVE_INLIF_CTRL_EN));
                  if ((dpp_pp->routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN) &&
                    (dpp_pp->routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_OUTLIF_CTRL_EN)) {
                      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("DSCP preserve port control and OutLIF control could not be enabled at the same time.")));
                  }
              }
          }
      }

      
      if (SOC_IS_QUX(unit)) {
          total_flow_region = QUX_COSQ_TOTAL_FLOW_REGIONS;
      }
      else if (SOC_IS_QAX(unit)) {
          total_flow_region = QAX_COSQ_TOTAL_FLOW_REGIONS;
      }
      SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
          for (i=0; i<total_flow_region; i++) {
              SOCDNX_IF_ERR_EXIT(soc_dpp_prop_parse_dtm_nof_remote_cores(unit, core, i));
          }
      }
      
      
      {
          char *propval ;

          propval = soc_property_suffix_num_str_get(unit, -1, spn_RUNTIME_PERFORMANCE_OPTIMIZE_ENABLE, "sched_allocation") ;
          if (propval)
          {
              SOC_CONTROL(unit)->
                runtime_performance_optimize_enable_sched_allocation = sal_ctoi((const char *)propval, (char **)0) ;
          }
          else
          {
              SOC_CONTROL(unit)->runtime_performance_optimize_enable_sched_allocation = 0 ;
          }
      }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_arad_dma_mutex_init(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS

    rv = soc_sbusdma_lock_init(unit);
    if (rv != SOC_E_NONE) {
        SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG("failed to allocate TSLAMDMA/ TABLEDMA Locks")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_reset(int unit, int reset_action)
{
    soc_reg_above_64_val_t reg_above_64_val = {0};
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;


    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SHR_BITSET_RANGE(reg_above_64_val, 0, 64);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

     if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
         SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
         SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

         soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, REG_PORT_ANY, 0, IQC_INITf, 0x0);
         SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

         soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, IPS_INIT_TRIGGERf, 0x0);
         SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

         soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, EGQ_BLOCK_INITf, 0x0);
         SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
     }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_soft_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_FULL);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_soft_ingress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_INGR);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_soft_egress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_EGR);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_and_fabric_soft_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_and_fabric_soft_ingress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_and_fabric_soft_egress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int
soc_arad_bring_up_interrupts(int unit)
{
    int cmc;

    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);

    
    soc_cmicm_intr2_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK);
    soc_cmicm_intr3_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_3_MASK);
    soc_cmicm_intr4_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_4_MASK);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        
        if (arad_interrupts_array_init(unit) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
        }

        
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            SOC_CONTROL(unit)->soc_flags |= SOC_F_POLLED;
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
            if (soc_ipoll_pktdma_connect(unit, soc_cmicm_pktdma_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing Packet DMA polled interrupt mode")));
            }
#endif
        } else {
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error separate pktdma thread cann't be created under interrupt mode")));
#endif
            
            if (soc_cm_interrupt_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }

        if (!SOC_WARM_BOOT(unit)) {
            uint32 rval;
            uint32 enable_msi = 1;

            if (soc_feature(unit, soc_feature_iproc) &&
                 (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_MSI) == 0) {
               
                enable_msi = 0;
            }
            rval = soc_pci_read(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc));
            soc_reg_field_set(unit, CMIC_CMC0_PCIE_MISCELr, &rval, ENABLE_MSIf, enable_msi);
            soc_pci_write(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc), rval);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_arad_specific_info_config_direct(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_arad_info_elk_config(unit));
    SOCDNX_IF_ERR_EXIT(soc_arad_ports_info_config(unit));

    exit:
        SOCDNX_FUNC_RETURN;
}


int soc_arad_specific_info_config_derived(int unit)
{
    int rv;
    soc_dpp_config_arad_t *dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    SOCDNX_INIT_FUNC_DEFS;

    

    
    if ((dpp_arad->init.ocb.ocb_enable == OCB_DRAM_SEPARATE) ||
        (dpp_arad->init.ocb.ocb_enable == OCB_ONLY_1_DRAM) ||
        ((dpp_arad->init.ocb.ocb_enable == OCB_ENABLED) && (soc_property_get(unit, spn_EXT_RAM_PRESENT, 0) == 0))) {
        if (soc_property_get(unit, spn_EXT_RAM_DBUFF_SIZE, ARAD_ITM_NOF_DBUFF_SIZES) != dpp_arad->init.ocb.databuffer_size) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Dram buffer size has to be equal to Ocb buffer size.")));
        }
    }
    rv = soc_arad_str_prop_mc_nbr_full_dbuff_get(unit, &(dpp_arad->init.dram.fmc_dbuff_mode));
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_pp_init(int unit)
{
    int
        rv = SOC_E_NONE,
        soc_sand_rv = 0, silent = 0,
        auxiliary_table_mode, port_i,
        vlan;
    uint32
        ipmc_enable,
        ipmc_l2_ssm_mode,
        ipmc_l2_ssm_mode_max = 0;
    soc_dpp_config_t *dpp = NULL;
    soc_dpp_config_pp_t *dpp_pp;
    ARAD_PP_INIT_PHASE1_CONF pp_phase1_conf;
    ARAD_PP_INIT_PHASE2_CONF pp_phase2_conf;
    soc_pbmp_t ports_pbmp;
    uint32 pp_port;
    int    core;

    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);
    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    

    ARAD_PP_INIT_PHASE1_CONF_clear(&pp_phase1_conf);
    ARAD_PP_INIT_PHASE2_CONF_clear(&pp_phase2_conf);

    
    if (!SOC_WARM_BOOT(unit)) {
        auxiliary_table_mode = soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0);
        if (auxiliary_table_mode == 2) {
            
            soc_sand_rv = arad_pp_frwrd_bmact_mac_in_mac_enable(unit);
            SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
        }
    }
    
    soc_sand_rv = arad_pp_mgmt_operation_mode_set(unit, &dpp->arad->pp_op_mode);
    rv = handle_sand_result(soc_sand_rv);
    SOCDNX_IF_ERR_EXIT(rv);

    
    ipmc_enable = soc_property_get(unit, spn_IPMC_ENABLE, 1);
    dpp->pp.ipmc_enable = (ipmc_enable) ? 1:0;

    if (SOC_IS_ARADPLUS_A0(unit) || SOC_IS_JERICHO(unit)) {
        
        ipmc_l2_ssm_mode = soc_property_get(unit, spn_IPMC_L2_SSM_MODE, 0);
	if ((ipmc_l2_ssm_mode > 0) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("When custom_feature_ipmc_l2_ssm_mode_lem is ENABLED, ipmc_l2_ssm_mode must be DISABLED.")));
	}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if(ARAD_KBP_ENABLE_IPV4_MC_BRIDGE) {
            ipmc_l2_ssm_mode_max = 3;
            rv = SOC_DPP_VERIFIY_SSM_NOT_EXCEEDS_MAX_VALUE(ipmc_l2_ssm_mode, ipmc_l2_ssm_mode_max);
        } else {
            ipmc_l2_ssm_mode_max = JER_KAPS_ENABLE(unit) ? 2 : 1;
            rv = SOC_DPP_VERIFIY_SSM_NOT_EXCEEDS_MAX_VALUE(ipmc_l2_ssm_mode, ipmc_l2_ssm_mode_max);
        }
#else
        ipmc_l2_ssm_mode_max = 1;
        rv = SOC_DPP_VERIFIY_SSM_NOT_EXCEEDS_MAX_VALUE(ipmc_l2_ssm_mode, ipmc_l2_ssm_mode_max);
#endif
        if (rv != SOC_E_NONE) {
            SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG(" ipmc_l2_ssm_mode soc property value %d exceed MAX value %d."), ipmc_l2_ssm_mode, ipmc_l2_ssm_mode_max));
        }
        dpp->pp.ipmc_l2_ssm_mode = soc_property_get(unit, spn_IPMC_L2_SSM_MODE, 0);
    }

    if (SOC_IS_JERICHO(unit)) {
        uint8 enhanced_fib_scale_prefix_length,
              enhanced_fib_scale_prefix_length_ipv6_long,
              enhanced_fib_scale_prefix_length_ipv6_short,
              l3_mc_use_tcam;
        dpp->pp.ipmc_l3mcastl2_mode = soc_property_get(unit, spn_IPMC_L3MCASTL2_MODE, 0);
#if defined(INCLUDE_KBP)
        l3_mc_use_tcam = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mc_use_tcam", 0));
#else
        
        l3_mc_use_tcam = 1;
#endif
        if (l3_mc_use_tcam > 2) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("l3_mc_use_tcam soc property value must be between 0-2.")));
        }
        dpp->pp.l3_mc_use_tcam = l3_mc_use_tcam;

        enhanced_fib_scale_prefix_length = soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH, 0);
        if ((enhanced_fib_scale_prefix_length > 28) || (enhanced_fib_scale_prefix_length % 4 > 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("enhanced_fib_scale_prefix_length soc property value must be 0(disabled)/4/8/12/16/20/24/28.")));
        }
        if ((enhanced_fib_scale_prefix_length > 0) && (soc_property_get(unit, spn_PUBLIC_IP_FRWRD_TABLE_SIZE, 0) > 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("public_ip_frwrd_table_size must be 0 (single KAPS DB) to enable enhanced_fib_scale_prefix_length.")));
        }
        if ((enhanced_fib_scale_prefix_length > 0) && (soc_property_get(unit, spn_PRIVATE_IP_FRWRD_TABLE_SIZE, 0) == 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("private_ip_frwrd_table_size must be enabled to enable enhanced_fib_scale_prefix_length.")));
        }
        if ((enhanced_fib_scale_prefix_length > 0) && (soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0) > 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("enhanced_fib_scale_prefix_length requires IPv4 UC routes to stored in the KAPS, disable ELK soc property.")));
        }
        dpp->pp.enhanced_fib_scale_prefix_length = enhanced_fib_scale_prefix_length;

        enhanced_fib_scale_prefix_length_ipv6_long = soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_LONG, 0);
        enhanced_fib_scale_prefix_length_ipv6_short = soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_SHORT, 0);
        if ((enhanced_fib_scale_prefix_length_ipv6_long > 64) || (enhanced_fib_scale_prefix_length_ipv6_long % 4 > 0) ||
            ((enhanced_fib_scale_prefix_length_ipv6_short >= enhanced_fib_scale_prefix_length_ipv6_long) && (enhanced_fib_scale_prefix_length_ipv6_long))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("enhanced_fib_scale_prefix_length_ipv6_long soc property value must be 0(disabled)/enhanced_fib_scale_prefix_length_ipv6_short + 4/../56/60/64.")));
        }
        if ((enhanced_fib_scale_prefix_length_ipv6_short > 60) || (enhanced_fib_scale_prefix_length_ipv6_short % 4 > 0) ||
            (enhanced_fib_scale_prefix_length_ipv6_long - enhanced_fib_scale_prefix_length_ipv6_short > 12)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("enhanced_fib_scale_prefix_length_ipv6_short soc property value must be "
                                                               "0(disabled)/enhanced_fib_scale_prefix_length_ipv6_long - 12/..- 8/enhanced_fib_scale_prefix_length_ipv6_long - 4.")));
        }
        if (((enhanced_fib_scale_prefix_length_ipv6_short > 0) && (enhanced_fib_scale_prefix_length_ipv6_long == 0)) ||
            ((enhanced_fib_scale_prefix_length_ipv6_short == 0) && (enhanced_fib_scale_prefix_length_ipv6_long > 0))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("enhanced_fib_scale_prefix_length_ipv6_long and enhanced_fib_scale_prefix_length_ipv6_short must both have non-zero values.")));
        }
        if ((enhanced_fib_scale_prefix_length_ipv6_long > 0) && (soc_property_get(unit, spn_PUBLIC_IP_FRWRD_TABLE_SIZE, 0) > 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("public_ip_frwrd_table_size must be 0 (single KAPS DB) to enable enhanced_fib_scale_prefix_length.")));
        }
        if ((enhanced_fib_scale_prefix_length_ipv6_long > 0) && (soc_property_get(unit, spn_PRIVATE_IP_FRWRD_TABLE_SIZE, 0) == 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("private_ip_frwrd_table_size must be enabled to enable enhanced_fib_scale_prefix_length.")));
        }
        if ((enhanced_fib_scale_prefix_length_ipv6_long > 0) && (soc_property_get(unit, spn_EXT_IP6_UC_RPF_FWD_TABLE_SIZE, 0) > 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("enhanced_fib_scale_prefix_length_ipv6_long requires IPv6 UC routes to stored in the KAPS, disable ELK soc property.")));
        }
        dpp->pp.enhanced_fib_scale_prefix_length_ipv6_long = enhanced_fib_scale_prefix_length_ipv6_long;
        dpp->pp.enhanced_fib_scale_prefix_length_ipv6_short = enhanced_fib_scale_prefix_length_ipv6_short;
    }

    
    rv = soc_arad_str_prop_parse_mact_learn_limit_mode(unit, &(SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE));
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_ARADPLUS(unit)) {
        
        rv = soc_arad_str_prop_parse_mact_learn_limit_range(unit, &(SOC_PPC_FRWRD_MACT_LEARN_LIF_RANGE_BASE(0)));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    if (SOC_WARM_BOOT(unit)) {
      SOC_EXIT;
    }

    
    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_occ_mgmt_init,(unit)));
    SOCDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = arad_pp_mgmt_init_sequence_phase1(unit, &pp_phase1_conf, silent);
    rv = handle_sand_result(soc_sand_rv);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = arad_pp_mgmt_init_sequence_phase2(unit, &pp_phase2_conf, silent);
    rv = handle_sand_result(soc_sand_rv);
    SOCDNX_IF_ERR_EXIT(rv);

    
    {
        uint8 include_inrif;
        uint8 include_port;
        uint8 include_vrf;

        SOC_PPC_MPLS_TERM_LKUP_INFO lkup_info;
        SOC_PPC_FRWRD_ILM_GLBL_INFO glbl_info;

        SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
        SOC_PPC_MPLS_TERM_LKUP_INFO_clear(&lkup_info);

        rv = soc_dpp_str_prop_parse_mpls_context(unit,&include_inrif,&include_port,&include_vrf);
        SOCDNX_IF_ERR_RETURN(rv);

        
        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_frwrd_ilm_glbl_info_get(unit, &glbl_info));

        if ((SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_min != -1) &&
             (SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_max != -1)) {

            glbl_info.elsp_info.labels_range.start = SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_min;
            glbl_info.elsp_info.labels_range.end = SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_max;
        }

        glbl_info.key_info.mask_inrif = (!include_inrif);
        glbl_info.key_info.mask_port = (!include_port);

        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_frwrd_ilm_glbl_info_set(unit, &glbl_info));
    }

    if (SOC_IS_JERICHO(unit)) {
        arad_pp_flp_lsr_stat_init(unit);
    }

    {
        SOC_PPC_PORT_INFO port_info;
        uint32 init_vid_enabled = 0;
      
        if (dpp_pp->pon_application_enable) {
            for (port_i = 0; port_i < SOC_MAX_NUM_PORTS ; ++port_i) {

                if (IS_PON_PORT(unit, port_i)) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port_i, &pp_port, &core));

                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit,core,pp_port,&port_info));
                    port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT;
					port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit,core,pp_port,&port_info));
                }

                if (IS_E_PORT(unit, port_i)) {
                    
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port_i, &pp_port, &core));
                    for (vlan = 0; vlan < 4096; vlan++) {
                        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(unit,core,pp_port,vlan,TRUE));
                    }
                }
            }
        }

        
        if (dpp_pp->port_use_initial_vlan_only_enabled) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE | SOC_PORT_FLAGS_INIT_VID_ONLY, &ports_pbmp));

            SOC_PBMP_ITER(ports_pbmp, port_i) {
                rv = soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port_i, &init_vid_enabled);
                if (rv != SOC_E_NONE) {
                   LOG_ERROR(BSL_LS_SOC_INIT,
                             (BSL_META_U(unit,
                                         "soc_arad_str_prop_parse_init_vid_enabled_port_get error in soc_arad_info_config\n")));
                }
                
                if (!init_vid_enabled) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port_i, &pp_port, &core));

                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit,core,pp_port,&port_info));
                    port_info.vlan_translation_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID;
					port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit,core,pp_port,&port_info));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_dpp_read_dummy_func(int unit, uint8 *buf, int offset, int nbytes)
{
    return SOC_E_RESOURCE;
}

int
soc_dpp_write_dummy_func(int unit, uint8 *buf, int offset, int nbytes)
{
    return SOC_E_RESOURCE;
}
#endif 

#ifdef BCM_WARM_BOOT_SUPPORT
void
soc_arad_init_empty_scache(int unit)
{
    int     stable_location = 0;
    uint32  stable_flags = 0;
    int     stable_size = 0;

    if (soc_scache_is_config(unit)!=SOC_E_NONE) {
       
       if (soc_switch_stable_register(unit,
                                      &soc_dpp_read_dummy_func,
                                      &soc_dpp_write_dummy_func,
                                      NULL, NULL) < 0) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "soc_switch_stable_register failure.\n")));
       }

       if (soc_stable_set(unit, stable_location, stable_flags) < 0) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "soc_stable_set failure\n")));
       } else if (soc_stable_size_set(unit, stable_size) < 0) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "soc_stable_size_set failure\n")));
       }
       
    }
}
#endif 

STATIC int soc_arad_rcpu_base_q_pair_init(int unit, int port_i)
{
    uint32 base_q_pair = 0, rval = 0;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    if (base_q_pair < 32)
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_0r(unit, &rval));
        rval |= 0x1 << base_q_pair;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_0r(unit, rval));
    } else if (base_q_pair < 64)
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_1r(unit, &rval));
        rval |= 0x1 << (base_q_pair - 32);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_1r(unit, rval));
    } else if (base_q_pair < 96)
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_2r(unit, &rval));
        rval |= 0x1 << (base_q_pair - 64);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_2r(unit, rval));
    } else if (base_q_pair == 96)
    {
        rval = 0x1;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_3r(unit, rval));
    } else
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: RCPU base_q_pair range is 0 - 96\n")) );
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }
    exit:
         SOCDNX_FUNC_RETURN;
}

STATIC int soc_arad_rcpu_init(int unit, soc_dpp_config_t *dpp)
{
    int port_i = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(dpp->arad->init.rcpu.slave_port_pbmp, port_i)
    {
        SOCDNX_IF_ERR_EXIT(soc_arad_rcpu_base_q_pair_init(unit, port_i));
    }

    exit:
         SOCDNX_FUNC_RETURN;
}


int
soc_arad_init(int unit, int reset_action)
{
    int rv = SOC_E_NONE;
    soc_dpp_config_t *dpp = NULL;
    int soc_sand_rv = 0, silent = 0;
    int port_i, core;
    SOC_TMC_PORT_HEADER_TYPE header_type_in;
    soc_pbmp_t pbmp;
    uint32 disable_hard_reset, tm_port;

    uint32 perform_iqm_workaround;

    SOCDNX_INIT_FUNC_DEFS;
#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_set_immediate_hw_access(unit);
#endif 

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#endif

    dpp = SOC_DPP_CONFIG(unit);

    if (dpp == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("SOC_DPP_CONFIG() is not allocated.")));
    }

   if (dpp->arad == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("dpp->arad is not allocated.")));
    }

    
    SOC_INFO(unit).fabric_logical_port_base = SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT;

    SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_ports(unit));

    DISPLAY_MEM ;

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_direct(unit));

    SOCDNX_IF_ERR_EXIT(soc_arad_info_config(unit));

    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_derived(unit));

    
    sal_dpc_enable(INT_TO_PTR(unit));

    
    disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0);
    if ((!SOC_WARM_BOOT(unit)) && (disable_hard_reset ==0 ))
    {
        

        DISPLAY_MEM ;

        rv = soc_arad_device_hard_reset(unit, reset_action);
        SOCDNX_IF_ERR_EXIT(rv);

        if (reset_action == SOC_DPP_RESET_ACTION_IN_RESET) {
            SOC_EXIT;
        }
    }

    
    rv = soc_arad_bring_up_interrupts(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(soc_dpp_cache_enable_init(unit));

    if (!SOC_WARM_BOOT(unit))
    {
       if (reset_action != SOC_DPP_RESET_ACTION_OUT_RESET) {
            
            rv = soc_arad_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_IN_RESET);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    

    soc_dcb_unit_init(unit);

    DISPLAY_MEM ;
    DISPLAY_MEM_PRINTF(("%s(): unit %d: Going to call %s\r\n",__func__,unit,"soc_dma_attach")) ;

    if ((rv = soc_dma_attach(unit, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_attach: soc_dma_attach returns error.\n")));
        return SOC_E_INTERNAL;
    }

    
    rv = soc_dcmn_cmic_pcie_userif_purge_ctrl_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_arad_dma_mutex_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        rv = soc_sbusdma_init(unit, 0, 0);
        SOCDNX_IF_ERR_EXIT(rv);
    }
#endif
    SOCDNX_IF_ERR_EXIT(sand_init_fill_table(unit));

    
    rv = soc_dpp_common_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_RETURN(soc_linkctrl_init(unit, &soc_linkctrl_driver_arad));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
      SOCDNX_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));
    }
#endif
    if (!SOC_WARM_BOOT(unit)) {
        

        DISPLAY_MEM ;
        DISPLAY_MEM_PRINTF(("%s(): unit %d: Going to do %s\r\n",__func__,unit,"INIT PHASE 1")) ;


        soc_sand_rv = arad_mgmt_init_sequence_phase1(unit, &(dpp->arad->init), silent);
        rv = handle_sand_result(soc_sand_rv);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = soc_dpp_common_tm_init(unit, &(dpp->tm.multicast_egress_bitmap_group_range));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    if (!SOC_IS_ARDON(unit))
    {
        arad_set_tcam_cache(unit, 1);
    }

    

    DISPLAY_MEM ;
    DISPLAY_MEM_PRINTF(("%s(): unit %d: Start %s\r\n",__func__,unit,"PP INIT")) ;

    if (dpp->arad->init.pp_enable) {
        rv = soc_arad_pp_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    if (!SOC_WARM_BOOT(unit)) {
        
        if (dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT
            || dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA) {
            
            ARAD_TDM_FTMH_INFO ftmh_info;

            ARAD_TDM_FTMH_INFO_clear(&ftmh_info);
            ftmh_info.action_ing = ARAD_TDM_ING_ACTION_ADD;
            ftmh_info.action_eg = ARAD_TDM_EG_ACTION_REMOVE;

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

            SOC_PBMP_ITER(pbmp, port_i) {
              if (port_i != SOC_DPP_PORT_INTERNAL_ERP(0)) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_in.get(unit, port_i, &header_type_in);
                SOCDNX_IF_ERR_EXIT(rv);
                if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
                  soc_sand_rv = arad_tdm_ftmh_set(unit,
                                                  core,
                                                  tm_port,
                                                  &ftmh_info);
                  rv = handle_sand_result(soc_sand_rv);
                  SOCDNX_IF_ERR_EXIT(rv);
                }
              }
            }
        }

        
        if (dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT) {
            
            soc_sand_rv = arad_tdm_opt_size_set(
                unit,
                SOC_DPP_DRV_TDM_OPT_SIZE
            );
            rv = handle_sand_result(soc_sand_rv);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        
    {
            uint32 is_valid;
            int core;
            SOC_TMC_EGR_OFP_SCH_INFO ofp_sch_info;
            SOC_TMC_EGR_OFP_SCH_INFO_clear(&ofp_sch_info);

            for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
                
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
                if (!is_valid) {
                    continue;
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));

                if (IS_TDM_PORT(unit, port_i)) {
                    soc_sand_rv = arad_egr_ofp_scheduling_get(unit, core, tm_port, &ofp_sch_info);
                    rv = handle_sand_result(soc_sand_rv);
                    SOCDNX_IF_ERR_EXIT(rv);

                    ofp_sch_info.nif_priority = SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH;

                    soc_sand_rv = arad_egr_ofp_scheduling_set(unit, core, tm_port, &ofp_sch_info);
                    rv = handle_sand_result(soc_sand_rv);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }
        }
        

        
        SOCDNX_IF_ERR_EXIT(soc_arad_rcpu_init(unit, dpp));

        perform_iqm_workaround = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "8865001", 0);
        if (perform_iqm_workaround) {
            rv = arad_iqm_workaround(unit);
            SOCDNX_SAND_IF_ERR_RETURN(rv);
        }

    }

    
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    if (!SOC_IS_ARDON(unit)) {
        arad_fast_regs_and_fields_access_init(unit);
    }
#endif 

    
    rv = arad_ser_init(unit);
    SOCDNX_SAND_IF_ERR_RETURN(rv);

exit:
#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_restore_immediate_hw_access(unit);
#endif 

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_init_reg_access(int unit, int reset)
{

  int rv = SOC_E_NONE;
  uint32 reg32_val;

  SOCDNX_INIT_FUNC_DEFS;

#ifdef CRASH_RECOVERY_SUPPORT
  soc_hw_set_immediate_hw_access(unit);
#endif 

  rv = soc_arad_device_hard_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
  SOCDNX_IF_ERR_EXIT(rv);

  
  rv = soc_arad_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
  SOCDNX_IF_ERR_EXIT(rv);

  if (!SOC_IS_ARDON(unit)) {
      reg32_val = 0;
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, CMAC_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_BYPASS_OSTSf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_LINE_LPBK_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_BYPASS_OSTSf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_LINE_LPBK_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_BYPASS_OSTSf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_LINE_LPBK_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_RESETf, 0);
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) , reg32_val));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) , reg32_val));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) , reg32_val));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) , reg32_val));

      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) , 0xfff));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) , 0xfff));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) , 0xfff));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) , 0xfff));

      sal_usleep(1000);

      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) , 0));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) , 0));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) , 0));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) , 0));
  }

exit:
#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_restore_immediate_hw_access(unit);
#endif 
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_init_enable_traffic(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (reset_action == SOC_DPP_RESET_ACTION_IN_RESET) {
        soc_sand_rv = arad_mgmt_enable_traffic_set(unit, FALSE);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        soc_sand_rv = arad_mgmt_enable_traffic_set(unit, TRUE);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_device_reset(int unit, int mode, int action)
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_set_immediate_hw_access(unit);
#endif 
     switch (mode) {
    case SOC_DPP_RESET_MODE_HARD_RESET:
        rv = soc_arad_device_hard_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_RESET:
        rv = soc_arad_device_blocks_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET:
        rv = soc_arad_device_blocks_soft_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET:
        rv = soc_arad_device_blocks_soft_ingress_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET:
        rv = soc_arad_device_blocks_soft_egress_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        rv = soc_arad_device_blocks_and_fabric_soft_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET:
        rv = soc_arad_device_blocks_and_fabric_soft_ingress_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET:
        rv = soc_arad_device_blocks_and_fabric_soft_egress_reset(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_INIT_RESET:
        rv = soc_arad_init(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_REG_ACCESS:
        rv = soc_arad_init_reg_access(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DPP_RESET_MODE_ENABLE_TRAFFIC:
        rv = soc_arad_init_enable_traffic(unit, action);
        SOCDNX_IF_ERR_EXIT(rv);
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown/Unsupported Reset Mode")));
    }

exit:
#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_restore_immediate_hw_access(unit);
#endif 
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_reinit(int unit, int reset)
{
    int         rv = SOC_E_NONE;
    uint32      reg32_val;

    SOCDNX_INIT_FUNC_DEFS;

#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_set_immediate_hw_access(unit);
#endif 

    rv = soc_arad_device_hard_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rv);

     
     rv = soc_arad_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
     SOCDNX_IF_ERR_EXIT(rv);

     if (SOC_PORT_NUM(unit,port) > 0) {
         reg32_val = 0;
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, CMAC_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_BYPASS_OSTSf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_LINE_LPBK_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_BYPASS_OSTSf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_LINE_LPBK_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_BYPASS_OSTSf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_LINE_LPBK_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_RESETf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) , reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) , reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) , reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) , reg32_val));
     }

 exit:
#ifdef CRASH_RECOVERY_SUPPORT
     soc_hw_restore_immediate_hw_access(unit);
#endif 
     SOCDNX_FUNC_RETURN;
}

typedef struct {
    uint32 if_first_port;
    uint32 is_valid;
    uint32 sch_total_if_rate;
} arad_ports_rate_t;

typedef struct {
    uint32 chan_arb_first_port;
    uint32 is_valid;
    uint32 egq_total_if_rate;
} arad_chan_arbs_rate_t;

int
soc_arad_default_ofp_rates_set(int unit) {
    uint32 egress_offset, sch_offset;
    int soc_sand_rv = 0;
    uint32 tm_port;
    int rv = SOC_E_NONE;
    arad_ports_rate_t e2e_ifs_rate[SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)];
    arad_chan_arbs_rate_t chan_arbs_first_ports[SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS)];
    int first_port = -1, core = SOC_CORE_INVALID;
    int priority_i, tcg_i, is_channelized;
    uint32 rate, if_rate_mbps, nof_priorities;
    SOC_TMC_EGR_QUEUING_TCG_INFO egq_tcg_info;
    SOC_TMC_SCH_PORT_INFO sch_tcg_info;
    uint32 nof_channels;
    soc_pbmp_t ports_bm;
    soc_port_t port_i;
    soc_port_if_t interface;
    uint32 flags;
    ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id, chan_arb_id_alt;
    uint32 tcg_egq_rate_arr[ARAD_NOF_TCGS];
    uint32 tcg_sch_rate_arr[ARAD_NOF_TCGS];
    uint32 sch_rate;
    uint32 egq_rate;
    soc_reg_above_64_val_t
        tbl_data;
#ifdef BCM_LB_SUPPORT
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    if(unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG_STR( "ERROR: invalid unit")));
    }

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {

        SOC_TMC_EGR_QUEUING_TCG_INFO_clear(&egq_tcg_info);
        SOC_TMC_SCH_PORT_INFO_clear(&sch_tcg_info);

        
        sal_memset(e2e_ifs_rate,0,sizeof(arad_ports_rate_t)*(SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)));
        sal_memset(chan_arbs_first_ports,0,sizeof(arad_chan_arbs_rate_t)*SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS));
        sal_memset(tcg_egq_rate_arr,0,sizeof(uint32)*ARAD_NOF_TCGS);
        sal_memset(tcg_sch_rate_arr,0,sizeof(uint32)*ARAD_NOF_TCGS);


        rv = soc_port_sw_db_valid_ports_core_get(unit, core, 0, &ports_bm);
        if (rv != SOC_E_NONE)
        {
           SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm valid ports bitmap failed")));
        }

        SOC_PBMP_ITER(ports_bm, port_i) {
            rv =  soc_port_sw_db_flags_get(unit, port_i, &flags);
            if (rv != SOC_E_NONE)
            {
             SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive port flags failed")));
            }
            if(SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_VIRTUAL_RCY_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_LB_MODEM(flags)) {
              continue;
            }

            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &nof_priorities));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port_i, &is_channelized));

            rv = soc_port_sw_db_interface_rate_get(unit, port_i, &if_rate_mbps);
            if (rv != SOC_E_NONE)
            {
               SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port interface rate %d failed"), port_i));
            }

            
            rate = if_rate_mbps * ARAD_RATE_1K;

            rv = soc_port_sw_db_num_of_channels_get(unit, port_i, &nof_channels);
            if (rv != SOC_E_NONE)
            {
               SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port number of channels %d failed"), port_i));
            }

#ifdef BCM_LB_SUPPORT
            if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port_i, &egress_offset);
                SOCDNX_IF_ERR_EXIT(rv);
                if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                    SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
                    if (0 == lb_port_num) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: the number of port connect to reserve interface is 0.\n")));
                    }

                    nof_channels = lb_port_num;
                }
            }
#endif

            
            rate = rate / nof_channels;

            sch_rate = SOC_SAND_MIN(SOC_TMC_RATE_1K * ((SOC_TMC_IF_MAX_RATE_MBPS_ARAD(unit) * 9)/10),(rate + (rate / 100) * 5)); 
            egq_rate = SOC_SAND_MIN(SOC_TMC_RATE_1K * ((SOC_TMC_IF_MAX_RATE_MBPS_ARAD(unit) * 9)/10),(rate + (rate / 100) * 1)); 

            rv = soc_port_sw_db_interface_type_get(unit, port_i, &interface);
            if (rv != SOC_E_NONE)
            {
               SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port interface type %d failed"), port_i));
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
            
            if (interface == SOC_PORT_IF_ERP) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.erp_interface_id.get(unit, core, &egress_offset);
                SOCDNX_IF_ERR_EXIT(rv);
            } else {
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
            }

            
            SOC_REG_ABOVE_64_CLEAR(tbl_data);
            SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &tbl_data));
            sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&tbl_data,FC_MAP_FCMf);

            if (!e2e_ifs_rate[sch_offset].is_valid) {
                e2e_ifs_rate[sch_offset].is_valid = TRUE;
                e2e_ifs_rate[sch_offset].if_first_port = tm_port;
                e2e_ifs_rate[sch_offset].sch_total_if_rate = 0;
            }

            
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = arad_ofp_rates_port2chan_arb(unit,tm_port,&chan_arb_id);
                if (SOC_FAILURE(rv)) {
                    SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: can't get chan_arb_id in ofp_rates setting port %d"), port_i));
                }
            } else { 
                if (is_channelized) {
                    chan_arb_id = egress_offset;
                } else {
                    chan_arb_id = SOC_DPP_DEFS_GET(unit, non_channelized_cal_id);
                }
            }

            
            if (!chan_arbs_first_ports[chan_arb_id].is_valid) {
                chan_arbs_first_ports[chan_arb_id].is_valid = TRUE;
                chan_arbs_first_ports[chan_arb_id].egq_total_if_rate = 0;
                chan_arbs_first_ports[chan_arb_id].chan_arb_first_port = tm_port;
                first_port = tm_port;
            }

            e2e_ifs_rate[sch_offset].sch_total_if_rate += sch_rate;
            
            if (SOC_IS_JERICHO(unit) && (interface == SOC_PORT_IF_ILKN) &&
               (SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing) ) {
               
               chan_arb_id_alt = chan_arb_id;
               SOC_SAND_SET_BIT(chan_arb_id_alt, 0x0, 0);
               chan_arbs_first_ports[chan_arb_id_alt].egq_total_if_rate += egq_rate;
               chan_arbs_first_ports[chan_arb_id_alt+1].egq_total_if_rate += egq_rate;
            }
            else
            {
                chan_arbs_first_ports[chan_arb_id].egq_total_if_rate += egq_rate;
            }

            
            

            if (sch_rate > SOC_TMC_SCH_MAX_RATE_KBPS_ARAD(unit)) {
                sch_rate = SOC_TMC_SCH_MAX_RATE_KBPS_ARAD(unit); 
            }

            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_single_port_rate_sw_set,(unit, core, tm_port, sch_rate));
            if(SOC_FAILURE(rv)) {
               SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port sch rate for port %d"), tm_port));
            }

            
            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_single_port_rate_sw_set,(unit, core, tm_port, egq_rate));
            if(SOC_FAILURE(rv)) {
               SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port egq rate for port %d"), tm_port));
            }
            
            for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
            {
               
               rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_port_priority_rate_sw_set,(unit, core, tm_port ,priority_i, egq_rate));
               if(SOC_FAILURE(rv)) {
                   SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq port_priority rate for port %d priority %d"), tm_port, priority_i));
               }

               
               rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_sch_port_priority_rate_sw_set, (unit, core, tm_port, priority_i, sch_rate));
               if(SOC_FAILURE(rv)) {
                   SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch port_priority rate for port %d priority %d"), tm_port, priority_i));
               }
            }

            
            if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
            {
              soc_sand_rv = arad_egr_queuing_ofp_tcg_get(unit, core, tm_port, &egq_tcg_info);
              SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

              
              soc_sand_rv = arad_sch_port_sched_get(unit, core, tm_port, &sch_tcg_info);
              SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

              for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
              {
                tcg_egq_rate_arr[egq_tcg_info.tcg_ndx[priority_i]] = egq_rate;
                tcg_sch_rate_arr[sch_tcg_info.tcg_ndx[priority_i]] = sch_rate;
              }

              
              for (tcg_i = 0; tcg_i < SOC_TMC_NOF_TCGS; ++tcg_i)
              {
                  
                  rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_tcg_rate_sw_set,(unit, core, tm_port ,tcg_i, tcg_egq_rate_arr[tcg_i]));
                  if(SOC_FAILURE(rv)) {
                   SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq tcg rate for port %d tcg %d"), tm_port, tcg_i));
                  }
                  
                  rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_tcg_rate_set,(unit, core, tm_port ,tcg_i, tcg_sch_rate_arr[tcg_i]));
                  if(SOC_FAILURE(rv)) {
                   SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch tcg rate for port %d tcg %d"), tm_port, tcg_i));
                  }
              }
            }
        }

        
        rv =  MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_port_priority_rate_hw_set,(unit));
        if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq port_priority rate to hw")));
        }

        
        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_sch_port_priority_hw_set, (unit, core));
        if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch port_priority rate to hw")));
        }

        
        rv =  MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_tcg_rate_hw_set,(unit));
        if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq tcg rate to hw")));
        }


        
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_single_port_rate_hw_set,(unit, 0, 0, 0));
        if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch port rate to hw")));
        }

        
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_single_port_rate_hw_set,(unit, core, first_port, 0));

        if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port egq rate for port %d"), first_port));
        }

        
        for (sch_offset = 0; sch_offset < SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces); ++sch_offset) {
            if (e2e_ifs_rate[sch_offset].is_valid) {
                rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_if_shaper_rate_set,(unit, core, e2e_ifs_rate[sch_offset].if_first_port,
                                                                                e2e_ifs_rate[sch_offset].sch_total_if_rate));
                if(SOC_FAILURE(rv)) {
                   SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch interface shaper for port %d"), e2e_ifs_rate[sch_offset].if_first_port));
                }
            }
        }

        
        for (chan_arb_id = 0; chan_arb_id < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars); ++chan_arb_id) {
            if (chan_arbs_first_ports[chan_arb_id].is_valid) {
                rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_interface_shaper_set, (unit, core, chan_arbs_first_ports[chan_arb_id].chan_arb_first_port,
                                                                                   SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE,chan_arbs_first_ports[chan_arb_id].egq_total_if_rate));
                if(SOC_FAILURE(rv)) {
                   SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq interface shaper for port %d"), chan_arbs_first_ports[chan_arb_id].chan_arb_first_port));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_ilkn_config_get(int unit, uint32 protocol_offset)
{
  ARAD_PORTS_ILKN_CONFIG *ilkn_config;
  SOCDNX_INIT_FUNC_DEFS;

  ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

  sal_memset(ilkn_config, 0, sizeof(ARAD_PORTS_ILKN_CONFIG));



  ilkn_config->metaframe_sync_period = soc_property_port_get(unit, protocol_offset, spn_ILKN_METAFRAME_SYNC_PERIOD, 2048);
  ilkn_config->interfcae_status_ignore = soc_property_port_get(unit, protocol_offset, spn_ILKN_INTERFACE_STATUS_IGNORE, 0);
  ilkn_config->interfcae_status_oob_ignore = soc_property_port_get(unit, protocol_offset, spn_ILKN_INTERFACE_STATUS_OOB_IGNORE, 0);

  ilkn_config->mubits_tx_polarity = soc_property_suffix_num_get(unit, protocol_offset, spn_FC_INTLKN_INDICATION_INVERT, "mubits_tx", 0);
  ilkn_config->mubits_rx_polarity = soc_property_suffix_num_get(unit, protocol_offset, spn_FC_INTLKN_INDICATION_INVERT, "mubits_rx", 0);
  ilkn_config->fc_tx_polarity = soc_property_suffix_num_get(unit, protocol_offset, spn_FC_INTLKN_INDICATION_INVERT, "tx", 0);
  ilkn_config->fc_rx_polarity = soc_property_suffix_num_get(unit, protocol_offset, spn_FC_INTLKN_INDICATION_INVERT, "rx", 0);

  
  ilkn_config->retransmit.enable_rx = soc_property_port_get(unit,protocol_offset, spn_ILKN_RETRANSMIT_ENABLE_RX, FALSE);
  ilkn_config->retransmit.enable_tx = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_ENABLE_TX, FALSE);
  ilkn_config->retransmit.buffer_size_entries = soc_property_get(unit, spn_ILKN_RETRANSMIT_BUFFER_SIZE, 255);
  ilkn_config->retransmit.nof_requests_resent = soc_property_get(unit, spn_ILKN_RETRANSMIT_NUM_REQUESTS_RESENT, 15);
  ilkn_config->retransmit.nof_seq_number_repetitions_tx = soc_property_get(unit, spn_ILKN_RETRANSMIT_NUM_SN_REPETITIONS_TX, 1);
  ilkn_config->retransmit.nof_seq_number_repetitions_rx = soc_property_get(unit, spn_ILKN_RETRANSMIT_NUM_SN_REPETITIONS_RX, 1);
  ilkn_config->retransmit.tx_ignore_requests_when_fifo_almost_empty = soc_property_get(unit, spn_ILKN_RETRANSMIT_TX_IGNORE_REQUESTS_WHEN_FIFO_ALMOST_EMPTY, 1);
  ilkn_config->retransmit.tx_wait_for_seq_num_change = soc_property_get(unit, spn_ILKN_RETRANSMIT_TX_WAIT_FOR_SEQ_NUM_CHANGE_ENABLE, 1);

  if (SOC_IS_JERICHO(unit)) {
    ilkn_config->retransmit.reserved_channel_id_rx = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_RESERVED_CHANNEL_ID, 0);
    ilkn_config->retransmit.reserved_channel_id_tx = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_RESERVED_CHANNEL_ID, 0);

    ilkn_config->retransmit.reserved_channel_id_rx = soc_property_suffix_num_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_RESERVED_CHANNEL_ID, "rx", ilkn_config->retransmit.reserved_channel_id_rx);
    ilkn_config->retransmit.reserved_channel_id_tx = soc_property_suffix_num_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_RESERVED_CHANNEL_ID, "tx", ilkn_config->retransmit.reserved_channel_id_tx);

    ilkn_config->retransmit.seq_number_bits_rx = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_SN_BITS, 8);
    ilkn_config->retransmit.seq_number_bits_tx = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_SN_BITS, 8);

    ilkn_config->retransmit.seq_number_bits_rx = soc_property_suffix_num_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_SN_BITS, "rx", ilkn_config->retransmit.seq_number_bits_rx);
    ilkn_config->retransmit.seq_number_bits_tx = soc_property_suffix_num_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_SN_BITS, "tx", ilkn_config->retransmit.seq_number_bits_tx);

    ilkn_config->retransmit.rx_discontinuity_event_timeout = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_RX_DISCONTINUITY_EVENT_TIMEOUT, 0);

    ilkn_config->retransmit.peer_tx_buffer_size = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_PEER_TX_BUFFER_SIZE, 0);

    ilkn_config->interleaved = soc_property_port_get(unit, protocol_offset, spn_ILKN_IS_BURST_INTERLEAVING, 1);

    
    ilkn_config->dedicated_tdm_context = soc_property_port_suffix_num_get (unit, protocol_offset, -1, spn_CUSTOM_FEATURE, "ilkn_dedicated_tdm_context", 0);

    if (ilkn_config->retransmit.reserved_channel_id_rx > SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for reserved_channel_id_rx\n\r"),
        ilkn_config->retransmit.reserved_channel_id_rx));
    }
    if (ilkn_config->retransmit.reserved_channel_id_tx > SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for reserved_channel_id_tx\n\r"),
        ilkn_config->retransmit.reserved_channel_id_tx));
    }

    if ((ilkn_config->retransmit.seq_number_bits_rx < 5) || (ilkn_config->retransmit.seq_number_bits_rx > 8)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for seq_number_bits_rx\n\r"),
        ilkn_config->retransmit.seq_number_bits_rx));
    }
    if ((ilkn_config->retransmit.seq_number_bits_tx < 5) || (ilkn_config->retransmit.seq_number_bits_tx > 8)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for seq_number_bits_tx\n\r"),
        ilkn_config->retransmit.seq_number_bits_tx));
    }

    if (ilkn_config->retransmit.rx_discontinuity_event_timeout * SOC_INFO(unit).frequency > 0x3ff) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for rx_discontinuity_event_timeout\n\r"),
        ilkn_config->retransmit.rx_discontinuity_event_timeout));
    }

    if (ilkn_config->retransmit.peer_tx_buffer_size > 0xFFFFFF) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for peer_tx_buffer_size\n\r"),
        ilkn_config->retransmit.peer_tx_buffer_size));
    }
  }
  else {
    ilkn_config->retransmit.rx_timeout_words =soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_TIMEOUT_WORDS, 0);
    ilkn_config->retransmit.rx_timeout_sn =soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_TIMEOUT_SN, 0);
    ilkn_config->retransmit.rx_ignore = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_IGNORE, 0);
    ilkn_config->retransmit.rx_watchdog = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_WATCHDOG, 0);
    ilkn_config->retransmit.rx_reset_when_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_ERROR_ENABLE, 1);
    ilkn_config->retransmit.rx_reset_when_retry_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_RETRY_ERROR_ENABLE, 1);
    ilkn_config->retransmit.rx_reset_when_alligned_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_ALLIGNED_ERROR_ENABLE, 1);
    ilkn_config->retransmit.rx_reset_when_timout_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_TIMOUT_ERROR_ENABLE, 0);
    ilkn_config->retransmit.rx_reset_when_wrap_after_disc_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_WRAP_AFTER_DISC_ERROR_ENABLE, 1);
    ilkn_config->retransmit.rx_reset_when_wrap_before_disc_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_WRAP_BEFORE_DISC_ERROR_ENABLE, 0);
    ilkn_config->retransmit.rx_reset_when_watchdog_err = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_UPON_WATCHDOG_ERROR_ENABLE, 1);
  }

exit:
  SOCDNX_FUNC_RETURN;
}


int
soc_arad_caui_config_get(int unit, uint32 protocol_offset)
{
  ARAD_PORTS_CAUI_CONFIG *caui_config;

  SOCDNX_INIT_FUNC_DEFS;

  caui_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.caui[protocol_offset];

  sal_memset(caui_config, 0, sizeof(ARAD_PORTS_CAUI_CONFIG));

  caui_config->rx_recovery_lane = 0;
#ifdef BCM_88660_A0
  if(SOC_IS_ARADPLUS(unit)){
    caui_config->rx_recovery_lane = soc_property_port_get(unit, (protocol_offset == 0) ? 0 : 1, spn_CAUI_RX_CLOCK_RECOVERY_LANE, 0);
    if(caui_config->rx_recovery_lane > 3){
      SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid option for rx recovery lane %d"), caui_config->rx_recovery_lane));
    }
  }
#endif 

exit:
  SOCDNX_FUNC_RETURN;
}

int arad_info_config_custom_reg_access(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_validate_hr_is_free(int unit, int core, uint32 base_q_pair, uint8 *is_free)
{
    int se_id, flow_id, rc;

    SOCDNX_INIT_FUNC_DEFS;

    
    se_id = base_q_pair + ARAD_HR_SE_ID_MIN;

    if (se_id == (SOC_DPP_CONFIG(unit)->tm.invalid_se_id_num)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error in retreiving seId" )));
    }

    
    flow_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_se2flow_id,((se_id))));
    if (flow_id == SOC_DPP_CONFIG(unit)->tm.invalid_voq_connector_id_num) {
        SOCDNX_EXIT_WITH_ERR(BCM_E_INTERNAL, (_BSL_SOCDNX_MSG("error in retreiving FlowId for seId(0x%x)"), se_id));
    }

    
    rc = bcm_dpp_am_cosq_scheduler_allocate(unit, core, 1, SHR_RES_ALLOC_WITH_ID, FALSE, FALSE, FALSE, TRUE, 1, SOC_TMC_AM_SCH_FLOW_TYPE_HR, NULL, &flow_id);
    if (rc != SOC_E_NONE)
    {
        *is_free = FALSE;
    }
    else
    {
        *is_free = TRUE;
        
        SOCDNX_IF_ERR_EXIT(bcm_dpp_am_cosq_scheduler_deallocate(unit, core, SHR_RES_ALLOC_WITH_ID, FALSE, FALSE, FALSE, TRUE, 1,SOC_TMC_AM_SCH_FLOW_TYPE_HR, flow_id));
    }

exit:
  SOCDNX_FUNC_RETURN;
}



int soc_arad_pll_info_get(int unit, soc_dpp_pll_info_t *pll_info)
{
    int rv;
    uint32 output_buffer=0;
    soc_reg_above_64_val_t reg_val_long;
    soc_dpp_pll_t *pll_val=NULL;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(pll_info, 0, sizeof(soc_dpp_pll_info_t));

    
    pll_val=&(pll_info->core_pll);
    pll_val->ref_clk = 25; 
    pll_val->p_div   = 1;  
    SOCDNX_IF_ERR_EXIT(READ_ECI_POWERUP_CONFIGr_REG32(unit, &reg_val_long[0]));

    
    pll_val->n_div=soc_reg_field_get(unit, ECI_POWERUP_CONFIGr, reg_val_long[0], CORE_PLL_N_DIVIDERf);
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    pll_val->m0_div=soc_reg_field_get(unit, ECI_POWERUP_CONFIGr, reg_val_long[0], CORE_PLL_M_DIVIDERf);
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_CORE_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_CORE_PLL_STATUSr, reg_val_long[0], CORE_PLL_LOCKEDf);

    
    pll_val=&(pll_info->uc_pll);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_TS_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    pll_val->ref_clk = 25; 
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_UC_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_UC_PLL_STATUSr, reg_val_long[0], UC_PLL_LOCKEDf);


    
    pll_val=&(pll_info->ts_pll);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_TS_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_TS_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_TS_PLL_STATUSr, reg_val_long[0], TS_PLL_LOCKEDf);

    
    pll_val=&(pll_info->ddr_pll[0]);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_0_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_0_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_DDR_0_PLL_STATUSr, reg_val_long[0], DDR_0_PLL_LOCKEDf);

    
    pll_val=&(pll_info->ddr_pll[1]);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_1_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_1_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_DDR_1_PLL_STATUSr, reg_val_long[0], DDR_1_PLL_LOCKEDf);

    
    pll_val=&(pll_info->ddr_pll[2]);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_2_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_2_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_DDR_2_PLL_STATUSr, reg_val_long[0], DDR_2_PLL_LOCKEDf);

     
    pll_val=&(pll_info->ddr_pll[3]);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_3_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_DDR_3_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_DDR_3_PLL_STATUSr, reg_val_long[0], DDR_3_PLL_LOCKEDf);


     
    pll_val=&(pll_info->srd_pll[0]);
    pll_val->ref_clk=125;
    SOCDNX_IF_ERR_EXIT(READ_ECI_SRD_0_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_SRD_0_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_SRD_0_PLL_STATUSr, reg_val_long[0], SRD_0_PLL_LOCKEDf);

     
    pll_val=&(pll_info->srd_pll[1]);
    pll_val->ref_clk=125;
    SOCDNX_IF_ERR_EXIT(READ_ECI_SRD_1_PLL_CONFIGr(unit, reg_val_long));
    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 11, 3, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->p_div=output_buffer;
    output_buffer=0;
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 3, 10, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 13, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[4], 10, 8, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m1_div=output_buffer;
    output_buffer=0;

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_SRD_1_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_SRD_1_PLL_STATUSr, reg_val_long[0], SRD_1_PLL_LOCKEDf);

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_arad_rif_info_routing_enabler_bm_mask_get(int unit)
{
	uint32 ipmc_l3mcastl2_mode, ipv4_double_capacity_used;
	uint32 nof_dedicated_bits, bit_offset_of_routing_enablers_profile = 0;
	uint32 routing_enablers_bm;
	int Mask = 0;
	int i;

	ipmc_l3mcastl2_mode  = soc_property_get(unit, spn_IPMC_L3MCASTL2_MODE, 0);
    ipv4_double_capacity_used = (soc_property_get(unit, spn_EXT_IP4_DOUBLE_CAPACITY_FWD_TABLE_SIZE, 0) == 0) ? 0 : 1;
	nof_dedicated_bits = SOC_DPP_IN_RIF_PROFILE_CONST_DEDICATED_BITS + ipmc_l3mcastl2_mode + ipv4_double_capacity_used;

    routing_enablers_bm = soc_property_get(unit, spn_NUMBER_OF_INRIF_MAC_TERMINATION_COMBINATIONS, SOC_PPC_RIF_MAX_NOF_ROUTING_ENABLERS_BITMAPS);

	for (i = SOC_DPP_NOF_IN_RIF_PROFILE_BITS - nof_dedicated_bits - 1; i >= 0; i--) {
        if (routing_enablers_bm > (1 << i) ) {
            bit_offset_of_routing_enablers_profile = SOC_DPP_NOF_IN_RIF_PROFILE_BITS - nof_dedicated_bits - i -1;
            break;
        }
    }

	Mask = (1 << (SOC_DPP_NOF_IN_RIF_PROFILE_BITS - nof_dedicated_bits)) - (1 << bit_offset_of_routing_enablers_profile);

	return Mask;
}


#undef _ERR_MSG_MODULE_NAME

