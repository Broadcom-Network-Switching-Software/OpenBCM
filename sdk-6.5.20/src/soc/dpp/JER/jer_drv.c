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
#define MAX_TCAM_MODE_JER            4
#define INVALID_TCAM_MODE_JER        2
#define TCAM_BIST_CHECK_BANK_JER     8
#define TCAM_BIST_CHECK_BANK_QUX     6


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <sal/appl/sal.h>
#include <sal/core/dpc.h>

#include <shared/utilex/utilex_str.h>
#include <shared/dbx/dbx_file.h>


#include <soc/uc.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/ipoll.h>
#include <soc/linkctrl.h>


#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dcmn/dcmn_iproc.h>
#include <soc/dcmn/dcmn_intr.h>
#include <soc/sand/sand_mem.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>


#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_defs.h>


#include <soc/dpp/JER/jer_drv.h>
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_regs.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_reg_access.h>
#include <soc/dpp/JER/jer_link.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_sbusdma_desc.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#define DB_INIT_PEMLA_UCODE "u_code_db2pem.txt"





typedef struct soc_jer_ucode_port_config_s {
    soc_port_if_t   interface;
    soc_pbmp_t      phy_pbmp;
    uint32          channel;
    int             core;
    uint32          tm_port;
    uint32          is_hg;
    uint32          is_nif;
    uint32          is_sif;
    uint32          is_kbp;
    uint32          is_reserved;
    uint32          is_lb_modem;
    uint32          protocol_offset;
} soc_jer_ucode_port_config_t;

STATIC int 
soc_jer_general_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int first_phy, i;
    soc_pbmp_t pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp);
    first_phy = (id * default_nof_lanes) + 1;

    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy+i);
    }
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &pbmp, &config->phy_pbmp));
   
exit:  
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_xaui_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int i, first_phy;
    soc_pbmp_t pbmp;
    uint32 xaui_if_base_lane    = SOC_DPP_DEFS_GET(unit, xaui_if_base_lane);
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp);
    first_phy = (id * default_nof_lanes) + xaui_if_base_lane;

    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy + i);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_rxaui_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int i, first_phy;
    soc_pbmp_t pbmp;
    uint32 rxaui_if_base_lane    = SOC_DPP_DEFS_GET(unit, rxaui_if_base_lane);
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp);
    first_phy = (id * default_nof_lanes) + rxaui_if_base_lane;

    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy + i);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_qsgmii_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    soc_pbmp_t phy_pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_nif_qsgmii_pbmp_get, (unit, port, id, &phy_pbmp)));

    SOC_PBMP_ASSIGN(config->phy_pbmp, phy_pbmp);
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_cpu_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (config->core == 0) {
        SOC_PBMP_PORT_ADD(config->phy_pbmp, 0);
    } else {
        SOC_PBMP_PORT_ADD(config->phy_pbmp, SOC_JERICHO_CPU_PHY_CORE_1);
    }


    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_caui_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int i, first_phy;
    int nof_pms_per_nbi = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);
    soc_pbmp_t pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp); 
    if (SOC_IS_JERICHO_PLUS_ONLY(unit) && id >= nof_pms_per_nbi) {
        id += nof_pms_per_nbi; 
    }
    first_phy = (id * default_nof_lanes) + 1;
    
    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy + i);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_xlaui2_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int i, first_phy;
    soc_pbmp_t pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp); 
    if (SOC_IS_JERICHO_PLUS_ONLY(unit) && id >= 12) {
        id += 12; 
    }
    first_phy = (id * default_nof_lanes) + 1;
    
    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy + i);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int 
soc_jer_ilkn_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{ 
    soc_pbmp_t phy_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_pbmp);

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_nif_ilkn_pbmp_get, (unit, port, id, &phy_pbmp, NULL)));

    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &phy_pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}


typedef int (*soc_jer_phy_pbmp_get_f)(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config);

typedef struct soc_jer_ucode_port_s {
    char* prefix;
    soc_port_if_t interface;
    uint32 default_nof_lanes;
    soc_jer_phy_pbmp_get_f phy_pbmp_get;
    uint32 flags;
} soc_jer_ucode_port_t;

#define SOC_JER_UCODE_FLAG_REQUIRED_INDEX   0x1
#define SOC_JER_UCODE_FLAG_NIF              0x2
#define SOC_JER_UCODE_FLAG_RESERVED         0x4

static soc_jer_ucode_port_t ucode_ports[] = {
    {"XE",          SOC_PORT_IF_XFI,        1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"10GBase-R",   SOC_PORT_IF_XFI,        1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF}, 
    {"XLGE2_",      SOC_PORT_IF_XLAUI2,     2,  &soc_jer_xlaui2_phy_pbmp_get,   SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"XLGE",        SOC_PORT_IF_XLAUI,      4,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"CGE",         SOC_PORT_IF_CAUI,       4,  &soc_jer_caui_phy_pbmp_get,     SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"ILKN",        SOC_PORT_IF_ILKN,       12, &soc_jer_ilkn_phy_pbmp_get,     SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"GE",          SOC_PORT_IF_SGMII,      1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"SGMII",       SOC_PORT_IF_SGMII,      1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF}, 
    {"XAUI",        SOC_PORT_IF_DNX_XAUI,   4,  &soc_jer_xaui_phy_pbmp_get,     SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"RXAUI",       SOC_PORT_IF_RXAUI,      2,  &soc_jer_rxaui_phy_pbmp_get,    SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"QSGMII",      SOC_PORT_IF_QSGMII,     1,  &soc_jer_qsgmii_phy_pbmp_get,   SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"CPU",         SOC_PORT_IF_CPU,        1,  &soc_jer_cpu_phy_pbmp_get,      0},
    {"RCY",         SOC_PORT_IF_RCY,        0,  &soc_jer_general_phy_pbmp_get,  0},
    {"SAT",         SOC_PORT_IF_SAT,        0,  &soc_jer_general_phy_pbmp_get,  0},
    {"IPSEC",       SOC_PORT_IF_IPSEC,      0,  &soc_jer_general_phy_pbmp_get,  0},
    {"IGNORE",      SOC_PORT_IF_NULL,       0,  NULL,                           0},
    {"RESERVED",    SOC_PORT_IF_NULL,       0,  NULL,                           SOC_JER_UCODE_FLAG_RESERVED},
    {NULL,          SOC_PORT_IF_NULL,       0,  NULL,                           0}, 

};


STATIC int
soc_jer_str_prop_parse_ucode_port(int unit, soc_port_t port, soc_jer_ucode_port_config_t* port_config)
{
    uint32 local_chan;
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len, id = 0, idx;

    SOCDNX_INIT_FUNC_DEFS;

    port_config->is_hg = 0;
    port_config->is_nif = 0;
    port_config->is_sif = 0;
    port_config->is_kbp = 0;
    port_config->channel = 0;
    port_config->interface = SOC_PORT_IF_NULL;
    port_config->core = -1;
    port_config->tm_port = 0xFFFFFFFF;
    port_config->protocol_offset = 0;
    port_config->is_reserved = 0;
    port_config->is_lb_modem = 0;
    SOC_PBMP_CLEAR(port_config->phy_pbmp);

    propkey = spn_UCODE_PORT;
    propval = soc_property_port_get_str(unit, port, propkey);    
    s = propval;

    
    if (propval){

        idx = 0;

        while(ucode_ports[idx].prefix) {
            prefix = ucode_ports[idx].prefix;
            prefix_len = sal_strlen(prefix);

            
            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += prefix_len;
                port_config->interface = ucode_ports[idx].interface;
                
                if(ucode_ports[idx].flags & SOC_JER_UCODE_FLAG_REQUIRED_INDEX) {
                    id = sal_ctoi(s, &ss);
                    if (s == ss) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                    }  
                    s = ss;
                }

                break;
            }

            idx++;
        }

        if (!ucode_ports[idx].prefix ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey)); 
        }

        
        if (ucode_ports[idx].flags & SOC_JER_UCODE_FLAG_RESERVED) {
            port_config->is_reserved = TRUE;
        }

        if (s && (*s == '.')) {
            
            ++s;
            local_chan = sal_ctoi(s, &ss);
            if (s != ss) {
                port_config->channel = local_chan;
            }
            s = ss;
        }

        
        while (s && (*s == ':')) {
            ++s;

            
            prefix = "hg";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += prefix_len;
                port_config->is_hg = 1;
                continue;
            } 

            prefix = "core_"; 
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {

                s += prefix_len;
                port_config->core = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Core not specify for (\"%s\") for %s"), propval, propkey));
                }
                s = ss;

                if (s && (*s == '.')) {
                    
                    ++s;
                    port_config->tm_port = sal_ctoi(s, &ss);
                    if (s == ss) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("TM port not specify for (\"%s\") for %s"), propval, propkey));
                    }
                    s = ss; 
                }

                if (s && (*s == ':'))
                {
                    prefix = "stat";
                    prefix_len = sal_strlen(prefix);
                    ++s;
                    if (!sal_strncasecmp(s, prefix, prefix_len))
                    {
                       port_config->is_sif = 1;
                    } else 
                    {
                        prefix = "kbp";
                        prefix_len = sal_strlen(prefix);
                        if (!sal_strncasecmp(s, prefix, prefix_len))
                        {
                           port_config->is_kbp = 1;
                        } else 
                        {
                            prefix = "modem";
                            prefix_len = sal_strlen(prefix);
                            if (!sal_strncasecmp(s, prefix, prefix_len))
                            {
                                port_config->is_lb_modem = 1;
                            } 
                        }
                    }
                }

                continue;
            } 
        }

        if(port_config->core == -1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Core not specify for (\"%s\") for %s"), propval, propkey));
        }

        if(port_config->tm_port == 0xFFFFFFFF && (!(port_config->is_sif)) && (!(port_config->is_kbp))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("TM port not specify for (\"%s\") for %s"), propval, propkey));
        }

        if(ucode_ports[idx].flags & SOC_JER_UCODE_FLAG_NIF) {
            port_config->is_nif = 1;
        }

        if(ucode_ports[idx].phy_pbmp_get) {
            SOCDNX_IF_ERR_EXIT(ucode_ports[idx].phy_pbmp_get(unit, port, id, ucode_ports[idx].default_nof_lanes, port_config));
        }

        port_config->protocol_offset = id;
    } 


exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_ports_config(int unit) 
{
    uint32 flags = 0,is_fiber_perf, is_pon, init_vid_enabled,value;
    uint32 nof_channels = 0;
    int port_i, port_j;
    int primary_port = 0, val, port_offset = 0;
    soc_jer_ucode_port_config_t port_config;
    int rv, core, is_channelized;
    uint8 is_tdm_queuing;
    soc_info_t          *si;
    uint32 erp_tm_port_id, erp_base_q_pair;
    soc_pbmp_t phy_ports, ports_bm, ports_same_if, pon_port_bm, is_tdm_queuing_on_bm;
    soc_port_if_t interface;
    ARAD_PORT_HEADER_TYPE header_type_in, header_type_out;
    uint32 is_hg_header;

    SOCDNX_INIT_FUNC_DEFS;
    
    si  = &SOC_INFO(unit);
    if (SOC_WARM_BOOT(unit)) {
              
       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_take(unit));
    }
    
    rv = soc_arad_str_prop_voq_mapping_mode_get(unit, &SOC_DPP_CONFIG(unit)->arad->voq_mapping_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_arad_str_prop_hqos_mapping_get(unit, &SOC_DPP_CONFIG(unit)->arad->hqos_mapping_enable);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_DPP_CONFIG(unit)->jer->tm.single_reassembly_for_rcy = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                                          "single_reassembly_for_rcy", 0);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_init(unit));

    
    sal_memset(SOC_DPP_CONFIG(unit)->arad->reserved_ports, 0x0, sizeof(SOC_DPP_CONFIG(unit)->arad->reserved_ports));

    
    soc_arad_ps_reserved_mapping_init(unit);


    if (SOC_WARM_BOOT(unit)) {

       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_restore(unit));

    }
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_interface_init(unit));

    for (port_i = 0; port_i <  SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++port_i) 
    {
        SOC_INFO(unit).port_l2pp_mapping[port_i] = -1;
        SOC_INFO(unit).port_l2po_mapping[port_i] = -1;
        if(SOC_E_NONE == soc_phy_primary_and_offset_get(unit, port_i, &primary_port, &port_offset)){
            SOC_INFO(unit).port_l2pp_mapping[port_i] = primary_port;
            SOC_INFO(unit).port_l2po_mapping[port_i] = port_offset;
        }
        SOC_INFO(unit).port_l2pa_mapping[port_i] = soc_property_port_get(unit, port_i, spn_PORT_PHY_ADDR, 0xFF);
        
        rv = soc_jer_str_prop_parse_ucode_port(unit, port_i, &port_config);
        SOCDNX_IF_ERR_EXIT(rv);

        
        if ((port_config.interface != _SHR_PORT_IF_NULL) && (port_i >= FABRIC_LOGICAL_PORT_BASE(unit))) {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Port %d is out-of-range. Max legal port is %d!\n"), port_i, (FABRIC_LOGICAL_PORT_BASE(unit)-1)));
            SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
        }

        if ((SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores == 1) && (port_config.core > 0)) {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error, in single core mode all ports must be on core 0\n")) );
            SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
        }

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_nif_sku_restrictions, (unit, port_config.phy_pbmp, port_config.interface, port_config.protocol_offset, port_config.is_kbp)));

        
        if (port_config.is_reserved) {
            SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].is_reserved = TRUE;
            SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].core = port_config.core;
            SOC_DPP_CONFIG(unit)->arad->reserved_ports[port_i].tm_port = port_config.tm_port;
            continue;
        }

        if(port_config.interface == SOC_PORT_IF_NULL) {
            continue;
        }

        flags = 0;

        
        if(port_config.is_nif) {
            flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
        }

        
        if(port_config.is_sif) {
            flags |= SOC_PORT_FLAGS_STAT_INTERFACE;
        }

        
        if(port_config.is_kbp) {
            flags |= SOC_PORT_FLAGS_ELK;
        }

        
        if(port_config.is_lb_modem) {
            flags |= SOC_PORT_FLAGS_LB_MODEM;
        }

        
        rv = soc_arad_str_prop_parse_pon_port(unit, port_i, &is_pon);
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_pon) {
            SOC_DPP_CONFIG(unit)->pp.pon_application_enable = 1;
            flags |= SOC_PORT_FLAGS_PON_INTERFACE;
        }

        
        if (is_pon) {
            PORT_SW_DB_PORT_ADD(pon, port_i);
        }

        
        rv = soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port_i, &init_vid_enabled);
        SOCDNX_IF_ERR_EXIT(rv);

        
        if (!init_vid_enabled) {
            SOC_DPP_CONFIG(unit)->pp.port_use_initial_vlan_only_enabled = 1;
            flags |= SOC_PORT_FLAGS_INIT_VID_ONLY;
        } else {
            
            
            SOC_DPP_CONFIG(unit)->pp.port_use_outer_vlan_and_initial_vlan_enabled = 1;
        }

        if (!SOC_WARM_BOOT(unit)) 
        {
            
            rv = soc_port_sw_db_port_validate_and_add(unit, port_config.core, port_i, port_config.channel, flags, port_config.interface, port_config.phy_pbmp);
            SOCDNX_IF_ERR_EXIT(rv);

            if ( port_config.interface == SOC_PORT_IF_ILKN || port_config.interface == SOC_PORT_IF_CAUI || 
                 port_config.interface == SOC_PORT_IF_QSGMII) {
                rv = soc_port_sw_db_protocol_offset_set(unit, port_i, 0, port_config.protocol_offset);
                SOCDNX_IF_ERR_EXIT(rv);
            }

            rv = soc_port_sw_db_is_hg_set(unit, port_i, port_config.is_hg);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_port_sw_db_local_to_tm_port_set(unit, port_i, port_config.tm_port);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_port_sw_db_local_to_pp_port_set(unit, port_i, port_config.tm_port); 
            SOCDNX_IF_ERR_EXIT(rv);

            
            is_fiber_perf = soc_property_port_get(unit, port_i, spn_SERDES_FIBER_PREF, 0);
            if(is_fiber_perf) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_FIBER));
            }
        }
    }

    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_nif_quad_to_core_validate(unit));
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
          SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable = 1;
        }

        
        if (nof_channels > SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num) {
            SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num = nof_channels;
        }
    }


    
    SOC_PBMP_CLEAR(phy_ports);

    erp_base_q_pair = (ARAD_EGR_NOF_PS-1)*ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) 
    {
        si->olp_port[core] = -1;
        val = soc_property_suffix_num_get(unit, core, spn_NUM_OLP_TM_PORTS, "core", 0);
        if (val > 0) {
            si->olp_port[core] = ARAD_OLP_PORT_ID + core;
            if (!SOC_WARM_BOOT(unit)) 
            {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, si->olp_port[core], 0, 0, SOC_PORT_IF_OLP, phy_ports));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->olp_port[core], ARAD_OLP_PORT_ID));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->olp_port[core], ARAD_OLP_PORT_ID));  
            }
        }

        si->oamp_port[core] = -1;
        SOCDNX_IF_ERR_EXIT(dcmn_property_suffix_num_get(unit, core, spn_NUM_OAMP_PORTS, "core", 0,&value));
        val = value;
        if (val > 0 || soc_property_get(unit, spn_SAT_ENABLE, 0)) {
            si->oamp_port[core] = ARAD_OAMP_PORT_ID + core;
            if (!SOC_WARM_BOOT(unit)) 
            {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, si->oamp_port[core], 0, 0, SOC_PORT_IF_OAMP, phy_ports));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->oamp_port[core], ARAD_OAMP_PORT_ID));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->oamp_port[core], ARAD_OAMP_PORT_ID));
            }
        }

        si->erp_port[core] = -1;
        val = soc_property_suffix_num_get(unit, core, spn_NUM_ERP_TM_PORTS, "core", 0);
        if (val > 0) {
            si->erp_port[core] = SOC_DPP_PORT_INTERNAL_ERP(core);
            if (!SOC_WARM_BOOT(unit)) 
            {
                erp_tm_port_id = soc_property_suffix_num_get(unit, si->erp_port[core], spn_LOCAL_TO_TM_PORT, "erp", ARAD_ERP_PORT_ID);
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, core, si->erp_port[core], 0, 0, SOC_PORT_IF_ERP, phy_ports));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->erp_port[core], erp_tm_port_id)); 
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->erp_port[core], erp_tm_port_id));   
                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.set(unit, si->erp_port[core], erp_base_q_pair);
                SOCDNX_IF_ERR_EXIT(rv);
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_set(unit, si->erp_port[core], 1));
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

    
    if (!SOC_WARM_BOOT(unit)) 
    {

        SOC_PBMP_ITER(ports_bm, port_i) {
            val = soc_property_port_get(unit, port_i, spn_PORT_INIT_SPEED, -1);
            if(-1 == val) {
                SOCDNX_IF_ERR_EXIT(soc_pm_default_speed_get(unit, port_i, &val));
            }
            if (dcmn_device_block_for_feature(unit,DCMN_NO_DC_FEATURE)) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
                if ((val == 50000 && interface == SOC_PORT_IF_XLAUI2) ||
                    (val == 25000 && interface == SOC_PORT_IF_XFI)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("device %s blocked for interface XLAUI2 with speed 50000  or interface XFI with speed 25000"), 
                                                      soc_dev_name(unit)));
                }
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port_i, val));
        }
    }

    
    val = soc_property_get(unit, spn_ILKN_TDM_DEDICATED_QUEUING, 0);
    SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing = (val == 0) ? ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_OFF : ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON;

    
    if (!SOC_WARM_BOOT(unit)) {
        SOC_PBMP_CLEAR(is_tdm_queuing_on_bm);
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.set(unit, is_tdm_queuing_on_bm));
        SOC_PBMP_ITER(ports_bm, port_i) {

            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
            if (interface != SOC_PORT_IF_ERP) {
                if (soc_dpp_str_prop_parse_tm_port_header_type(unit, port_i, &(header_type_in), &(header_type_out), &is_hg_header)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_header_type error")));       
                }
            } else {
                header_type_out=SOC_TMC_PORT_HEADER_TYPE_NONE;
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port_i,header_type_out));

            val = soc_property_port_get(unit, port_i, spn_TDM_QUEUING_FORCE, 0);
            if (val)
            {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_port_add(unit,port_i));
            }
            SOCDNX_IF_ERR_EXIT(soc_jer_egr_interface_alloc(unit, port_i));
        }

        if (SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
        
            SOC_PBMP_ITER(ports_bm, port_i) {

                
                soc_port_sw_db_interface_type_get(unit, port_i, &interface);
                if(interface != SOC_PORT_IF_ILKN) {
                    continue;
                }

                is_tdm_queuing = 0;
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port_i, &ports_same_if));
                SOC_PBMP_ITER(ports_same_if, port_j) {
                    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port_j, &is_tdm_queuing));
                    if (is_tdm_queuing || IS_TDM_PORT(unit, port_j)) {
                        is_tdm_queuing = 1;
                        break;
                    }
                }

                if (!is_tdm_queuing) {
                    LOG_ERROR(BSL_LS_SOC_INIT,(BSL_META_U(unit, "Error: When Adding port to ILKN interface while TDM_DEDICATED_QUEUING_MODE is ON, "
                                                                "there must be at least one TDM port or port with TDM_QUEUING_ON\n")));
                    SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
                }
            }
        }
    }

    
    if (!SOC_WARM_BOOT(unit)) 
    {
        SOC_PBMP_ITER(ports_bm, port_i) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port_i, &is_channelized));
            if (is_channelized) {
                
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port_i, FALSE));
            } else {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port_i, TRUE));
            }
        }
    }

    SOC_DPP_CONFIG(unit)->jer->tm.is_ilkn_big_cal = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ilkn_big_cal", 0);

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_get_default_config_jer(
    int unit)
{

    soc_dpp_config_jer_t *jer;

    SOCDNX_INIT_FUNC_DEFS;

    jer = SOC_DPP_JER_CONFIG(unit);

    
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_config_ovrd,(unit)));



    
    sal_memset(jer, 0, sizeof(soc_dpp_config_jer_t));

exit:
    SOCDNX_FUNC_RETURN;
}



void soc_jer_pp_config_protection_get(
    int unit)
{
    soc_dpp_config_jer_pp_t *jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);
    char *prop_key;
    uint32 prop_value;

    
    prop_key = spn_BCM886XX_INGRESS_PROTECTION_COUPLED_MODE;
    prop_value = soc_property_get(unit, prop_key, 1);
    jer_pp_config->protection_ingress_coupled_mode = (prop_value) ? 1 : 0;

    
    prop_key = spn_BCM886XX_EGRESS_PROTECTION_COUPLED_MODE;
    prop_value = soc_property_get(unit, prop_key, 1);
    jer_pp_config->protection_egress_coupled_mode = (prop_value) ? 1 : 0;

    
    prop_key = spn_BCM886XX_FEC_ACCELERATED_REROUTE_MODE;
    prop_value = soc_property_get(unit, prop_key, 0);
    jer_pp_config->protection_fec_accelerated_reroute_mode = (prop_value) ? 1 : 0;
}


void soc_jer_pp_config_kaps_get(
    int unit)
{
    soc_dpp_config_jer_pp_t *jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);

    char *prop_key;
    uint32 prop_value;

    
    prop_key = spn_PUBLIC_IP_FRWRD_TABLE_SIZE;
    prop_value = soc_property_get(unit, prop_key, 0);
    jer_pp_config->kaps_public_ip_frwrd_table_size = prop_value;

    
    prop_key = spn_PRIVATE_IP_FRWRD_TABLE_SIZE;
    prop_value = soc_property_get(unit, prop_key, 0);
    jer_pp_config->kaps_private_ip_frwrd_table_size = prop_value;

    
    prop_key = spn_PMF_KAPS_LARGE_DB_SIZE;
    prop_value = soc_property_get(unit, prop_key, 0);
    jer_pp_config->kaps_large_db_size = prop_value;

    
    prop_key = spn_PMF_KAPS_MGMT_ADVANCED_MODE;
    prop_value = soc_property_get(unit, prop_key, 0);
    jer_pp_config->kaps_mgmt_advanced_mode = prop_value;

}


void soc_jer_pp_config_lif_get(
    int unit){
    soc_dpp_config_pp_t *dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
    uint32 prop_val;

    
    prop_val = soc_property_get(unit, spn_SYSTEM_IS_ARAD_IN_SYSTEM, 0);

    if (prop_val) {
        dpp_pp->nof_global_out_lifs = SOC_DPP_NOF_GLOBAL_LIFS_ARAD;
    }
}



int soc_jer_pp_config_rif_get(
    int unit)
{
    soc_dpp_config_l3_t *dpp_l3 = &(SOC_DPP_CONFIG(unit))->l3;
    char *prop_key;
    uint32 prop_value;
    int rif_multiplier = SOC_DPP_DEFS_GET((unit), nof_rifs); 

    SOCDNX_INIT_FUNC_DEFS;

    
    prop_key = spn_RIF_ID_MAX;
    prop_value = soc_property_get(unit, prop_key, rif_multiplier);
    
    if (prop_value % rif_multiplier == (rif_multiplier - 1)) {
        prop_value++;
    }
    if (prop_value % rif_multiplier != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Soc property RIF_ID_MAX must be a multiple of %d."), rif_multiplier));
    }
    if (prop_value > SOC_DPP_MAX_RIF_ID + 1)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Parameter for %s out of range. Valid range 0 - 32K-1."), prop_key));
    }


    dpp_l3->nof_rifs = prop_value;

exit: 
    SOCDNX_FUNC_RETURN;

}


static int soc_jer_shared_mem_reject_config_get(int unit)
{
    int dp;
    int shared_mem_reject_percentage[SOC_TMC_NOF_DROP_PRECEDENCE] = { 100, 
                                                                      85,  
                                                                      75,  
                                                                      0    
    };

    int factor_idx, is_divisiable = 0;
    int factors[] = {5, 5, 2, 2}; 

    SOCDNX_INIT_FUNC_DEFS;

    
    for (dp = 0 ; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++)
    {
        SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_numerator[dp] =
                soc_property_suffix_num_get(unit, dp, spn_CUSTOM_FEATURE, "shared_mem_reject_percentage_dp", shared_mem_reject_percentage[dp]);
    }

    SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_denominator = 100;

    
    for (factor_idx = 0 ; factor_idx < 4 ; factor_idx++)
    {
        is_divisiable = 1;

        for (dp = 0 ; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++)
        {
            if (SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_numerator[dp] % factors[factor_idx] != 0)
            {
                is_divisiable = 0;
                break;
            }
        }

        if (is_divisiable)
        {
            for (dp = 0 ; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++)
            {
                SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_numerator[dp] /= factors[factor_idx];
            }

            SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_denominator /= factors[factor_idx];
        }
    }

    SOCDNX_FUNC_RETURN;
}


int soc_jer_specific_info_config_direct(int unit)
{
    int rv, fmc_dbuff_mode,offset;
    uint32 dram_total_size,value;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(dcmn_property_suffix_num_get(unit, 4, spn_USE_FABRIC_LINKS_FOR_ILKN_NIF, "ilkn", 0,&value));
    SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[0] = value;
    SOCDNX_IF_ERR_EXIT(dcmn_property_suffix_num_get(unit, 5, spn_USE_FABRIC_LINKS_FOR_ILKN_NIF, "ilkn", 0,&value));
    SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[1] = value;

    for (offset = 0; offset < MAX_NUM_OF_PMS_IN_ILKN; offset++) {
            SOC_DPP_JER_CONFIG(unit)->nif.ilkn_burst_short[offset] = soc_property_port_get(unit, offset, spn_ILKN_BURST_SHORT, 32);
            SOC_DPP_JER_CONFIG(unit)->nif.ilkn_burst_min[offset] = soc_property_port_get(unit, offset, spn_ILKN_BURST_MIN, SOC_DPP_JER_CONFIG(unit)->nif.ilkn_burst_short[offset]);
            SOC_DPP_JER_CONFIG(unit)->nif.ilkn_burst_max[offset] = soc_property_port_get(unit, offset, spn_ILKN_BURST_MAX, 256);
    }

    SOC_INFO(unit).fabric_logical_port_base = soc_property_get(unit, spn_FABRIC_LOGICAL_PORT_BASE, SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT);
    if (SOC_IS_QAX(unit)) {
        SOC_DPP_CONFIG(unit)->qax->link_bonding_enable = soc_property_get(unit, spn_LINK_BONDING_ENABLE, 0);

        if (SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
            SOC_DPP_CONFIG(unit)->qax->lbi_en = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lbi_en", FALSE);
        }
        else {
            SOC_DPP_CONFIG(unit)->qax->lbi_en = 0;
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_ports_config(unit));

    rv = soc_jer_str_prop_mc_nbr_full_dbuff_get(unit, &fmc_dbuff_mode);
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode = fmc_dbuff_mode;

    
    if (SOC_IS_QAX(unit)) {
        
        SOC_DPP_CONFIG(unit)->jer->dbuffs.max_nof_dram_buffers = (((SOC_DPP_CONFIG(unit)->arad->init.dram.dram_size_total_mbyte) * 1024) / (SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size)) * 1024;
    } else {
        SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_init_pdm_nof_entries_calc(unit, SOC_DPP_CONFIG(unit)->arad->init.dram.pdm_mode, &dram_total_size)));
        
        SOC_DPP_CONFIG(unit)->jer->dbuffs.max_nof_dram_buffers = SOC_SAND_MIN(dram_total_size, ((SOC_DPP_CONFIG(unit)->arad->init.dram.dram_size_total_mbyte * 1024) / SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size) * 1024); 
    }

    
    arad_tbl_mark_cachable(unit);
    
    SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_pp_counter[0] = soc_property_suffix_num_get(unit, 0, spn_TRUNCATE_DELTA_IN_PP_COUNTER, "0", 0);
    SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_pp_counter[1] = soc_property_suffix_num_get(unit, 0, spn_TRUNCATE_DELTA_IN_PP_COUNTER, "1", 0);
    
    SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_tm_counter[0] = soc_property_suffix_num_get(unit, 0, spn_TRUNCATE_DELTA_IN_TM_COUNTER, "0", 0);
    SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_tm_counter[1] = soc_property_suffix_num_get(unit, 0, spn_TRUNCATE_DELTA_IN_TM_COUNTER, "1", 0);

    SOCDNX_IF_ERR_EXIT(soc_jer_shared_mem_reject_config_get(unit));

exit: 
    SOCDNX_FUNC_RETURN;
}



int soc_jer_specific_info_config_derived(int unit) 
{
    ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);
    int pml = 0, pll_binding = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (init->pp_enable == TRUE) {
        
        soc_jer_pp_config_protection_get(unit);
        
        soc_jer_pp_config_kaps_get(unit);

        SOCDNX_IF_ERR_EXIT(soc_jer_pp_config_rif_get(unit));
        
        soc_jer_pp_config_lif_get(unit);
    }

    
    SOC_DPP_CONFIG(unit)->jer->pp.roo_host_arp_msbs = (soc_property_get(unit, spn_BCM886XX_ROO_HOST_ARP_MSBS, 0x0)); 

    
    init->ports.smooth_division_resolve_using_msb = (soc_property_get(unit, spn_TRUNK_RESOLVE_USE_LB_KEY_MSB_SMOOTH_DIVISION, 0x0));
    init->ports.stack_resolve_using_msb = (soc_property_get(unit, spn_TRUNK_RESOLVE_USE_LB_KEY_MSB_STACK, 0x0));

    SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_in[0] = soc_property_suffix_num_get(unit, 0, spn_SERDES_NIF_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
    SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[0] = soc_property_suffix_num_get(unit, 0, spn_SERDES_NIF_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
    SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_synce_out[0] = soc_property_suffix_num_get(unit, 0, spn_SYNC_ETH_CLK_DIVIDER, "clk", soc_dcmn_init_serdes_ref_clock_156_25);
    SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_synce_out[1] = soc_property_suffix_num_get(unit, 1, spn_SYNC_ETH_CLK_DIVIDER, "clk", soc_dcmn_init_serdes_ref_clock_156_25);
    if (SOC_IS_QUX(unit)) {
        
        SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmx_in = soc_property_suffix_num_get(unit, 3, spn_SERDES_NIF_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
        SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmx_out = soc_dcmn_init_serdes_ref_clock_156_25;
    } else {
          SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_in[1] = soc_property_suffix_num_get(unit, 1, spn_SERDES_NIF_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
          SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[1] = soc_property_suffix_num_get(unit, 1, spn_SERDES_NIF_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
          SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_in = soc_property_suffix_num_get(unit, 2, spn_SERDES_NIF_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
          SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_out = soc_property_suffix_num_get(unit, 2, spn_SERDES_NIF_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
          if (SOC_IS_QAX(unit)) {
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_in[0] = soc_property_suffix_num_get(unit, 2, spn_SERDES_NIF_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_out[0] = soc_property_suffix_num_get(unit, 2, spn_SERDES_NIF_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_in[1] = soc_property_suffix_num_get(unit, 2, spn_SERDES_NIF_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_out[1] = soc_property_suffix_num_get(unit, 2, spn_SERDES_NIF_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
          } else {
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_in[0] = soc_property_suffix_num_get(unit, 0, spn_SERDES_FABRIC_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_out[0] = soc_property_suffix_num_get(unit, 0, spn_SERDES_FABRIC_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_in[1] = soc_property_suffix_num_get(unit, 1, spn_SERDES_FABRIC_CLK_FREQ, "in", soc_dcmn_init_serdes_ref_clock_156_25);
              SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_out[1] = soc_property_suffix_num_get(unit, 1, spn_SERDES_FABRIC_CLK_FREQ, "out", soc_dcmn_init_serdes_ref_clock_156_25);
          }
      }

    if (SOC_IS_QAX(unit)) {
        
        for (pml = 0; pml < SOC_DPP_DEFS_GET(unit, nof_instances_nbil); ++pml) {
            pll_binding = soc_property_suffix_num_get(unit, pml, spn_SERDES_NIF_CLK_BINDING, "in", 0); 
            if (pll_binding) {
                if (SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_in[pml] != SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_out) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Soc properties mismatch. Both SERDES_NIF_CLK_BINDING is enabled and "
                                                                        "SERDES_NIF_CLK_FREQ_IN is set to value different than SERDES_NIF_CLK_FREQ_OUT_2. "
                                                                        "Please match these configurations")));
                }
                
                SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_in[pml] = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_out;
                SOC_DPP_CONFIG(unit)->jer->pll.is_pll_binding_pml[pml] = 1;
            } else {
                SOC_DPP_CONFIG(unit)->jer->pll.is_pll_binding_pml[pml] = 0;
            }
        } 
    }

exit: 
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_dma_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    soc_dcb_unit_init(unit);

    
    SOCDNX_IF_ERR_EXIT(soc_dma_attach(unit, 1 ));

    
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_pcie_userif_purge_ctrl_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_arad_dma_mutex_init(unit));

#ifdef BCM_SBUSDMA_SUPPORT
    
    if (soc_mem_dmaable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm)) ||
      soc_mem_slamable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm))) { 
        SOCDNX_IF_ERR_EXIT(soc_sbusdma_init(unit, 0, 0));

        
        if (soc_property_get(unit, spn_DMA_DESC_AGGREGATOR_CHAIN_LENGTH_MAX, 0)) {
            uint32 desc_num_max, desc_mem_buff_size, desc_timeout_usec;
            if (((soc_property_get(unit, spn_DMA_DESC_AGGREGATOR_BUFF_SIZE_KB, 0)) == 0) ||
                ((soc_property_get(unit, spn_DMA_DESC_AGGREGATOR_TIMEOUT_USEC, 0)) == 0)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("desc_num_max, desc_mem_buff_size and desc_timeout_usec must be all initialized together.")));
            }

            desc_num_max = soc_property_get(unit, spn_DMA_DESC_AGGREGATOR_CHAIN_LENGTH_MAX, 0);
            
            desc_mem_buff_size = soc_property_get(unit, spn_DMA_DESC_AGGREGATOR_BUFF_SIZE_KB, 0) * 128;
            desc_timeout_usec = soc_property_get(unit, spn_DMA_DESC_AGGREGATOR_TIMEOUT_USEC, 0);

            SOCDNX_IF_ERR_EXIT(jer_sbusdma_desc_init(unit, desc_num_max, desc_mem_buff_size, desc_timeout_usec));
        }
    }
#endif

    SOCDNX_IF_ERR_EXIT(sand_init_fill_table(unit));   
    SOCDNX_IF_ERR_EXIT(jer_mgmt_dma_fifo_source_channels_db_init(unit));   

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_deinit_dma(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_SBUSDMA_SUPPORT
    
    if (soc_mem_dmaable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm)) ||
      soc_mem_slamable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm))) { 
        SOCDNX_IF_ERR_EXIT(soc_sbusdma_desc_detach(unit));
    }
#endif
    SOCDNX_IF_ERR_EXIT(sand_deinit_fill_table(unit));

    
    soc_arad_dma_mutex_destroy(unit);

    SOCDNX_IF_ERR_EXIT(soc_dma_detach(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_reset_cmic_regs(
    int unit)
{
    uint32 core_freq = 0x0;
    uint32 rval = 0;
    int schan_timeout = 0x0;
    int dividend, divisor;
    int mdio_int_freq, mdio_delay;
    int non_qsgmii_mode = 1;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x00222227));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x24442220));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x22222222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x53333222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x55555555));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x32333335));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x00000022));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_112_119r(unit, 0x00000040));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x00000000));

        
        
        
        
        
        
        
        
        
        
        
        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 36);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 35);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 34);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, 33);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 32);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 31);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 30);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 29);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  28);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  27);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 26);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, 25);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 24);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 23);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 22);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 21);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 20);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 19);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 18);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 17);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 16);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 15);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 14);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 13);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_24f, 12);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_25f, 11);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_26f, 10);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_27f, 9);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_28f, 8);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_29f, 7);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_30f, 6);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_31f, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_32f, 4);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_33f, 3);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_34f, 2);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_35f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit,   rval));

        rval = 0;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit,   0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit,   0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit,   0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit,   0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit,   0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit,   0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit,   0));


        
        
        

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CTRLr(unit, 0x9A));

        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 25);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_DIVr(unit, rval));


        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x7270E0);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_PARAMSr(unit, rval));

    } else if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x00022227));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x00333220));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x22022000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x22222022));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x00222222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x02000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x30330002));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x33033333));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x55555553));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x55555555));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x55555555));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_88_95r(unit, 0x55555555));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_96_103r(unit, 0x00005555));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_104_111r(unit, 0x22222000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_112_119r(unit, 0x02044442));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x00000002));

        
        
        
        
        
        
        
        
        
        
        
        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 32);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 31);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 30);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, 29);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 28);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 27);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 26);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 25);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  24);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  23);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 22);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, 21);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 20);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 19);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 18);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 17);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 16);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 15);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 14);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 13);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 12);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 11);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 10);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f,  9);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_24f, 8);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_25f, 7);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_26f, 6);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_27f, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_28f, 4);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_29f, 3);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_30f, 2);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_31f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_32f, 48);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_33f, 47);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_34f, 46);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_35f, 45);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_36f, 44);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_37f, 43);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_38f, 42);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_39f, 41);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_40f, 40);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_41f, 39);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_42f, 38);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_43f, 37);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_44f, 36);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_45f, 35);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_46f, 34);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_47f, 33);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit,   rval));

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, 0x0));



        
        
        

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CTRLr(unit, 0xEA));

        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 60);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_DIVr(unit, rval));


        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x112A880);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_PARAMSr(unit, rval));
        rval = 0;

    } else if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x04444447));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x22222334));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x33433222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x33333333));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x22222233));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x22222222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x55555522));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x44444665));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x44444444));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_88_95r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_96_103r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_104_111r(unit, 0x66226666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_112_119r(unit, 0x44555066));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x04404633));

        

        
        
        
        
        
        

        
        
        

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 4);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 3);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 2);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 8);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 7);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 6);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  12);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  11);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 10);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f,  9);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 20);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 19);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 18);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 17);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 16);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 15);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 14);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 13);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 24);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 23);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 22);
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 21);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit,   rval));

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, 0x0));

        
        
        
        
        

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 12);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 11);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 10);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f,  9);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 4);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 3);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 2);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  8);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  7);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 6);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 25);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 21);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 17);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 13);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 41);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 37);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 33);
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 29);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        if (non_qsgmii_mode) {
            
            
            
            
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 57);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 53);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 49);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 45);
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r(unit,   rval));

            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r(unit, 0x0));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r(unit, 0x0));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r(unit, 0x0));
        } else {
            
            
            
            
            
            
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 45);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 46);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 47);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 48);
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r(unit,   rval));

            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_24f, 49);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_25f, 50);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_26f, 51);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_27f, 52);
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r(unit,   rval));

            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_28f, 53);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_29f, 54);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_30f, 55);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_31f, 56);
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r(unit,   rval));

            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_32f, 57);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_33f, 58);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_34f, 59);
            soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_35f, 60);
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r(unit,   rval));
        }

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r(unit, 0x0));

        
        
        
        
        


        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 12);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 11);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 10);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f,  9);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 4);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 3);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 2);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  8);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  7);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 6);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 25);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 21);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 17);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 13);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 41);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 37);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 33);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 29);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 57);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 53);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 49);
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 45);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r(unit,   rval));

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r(unit, 0x0));

        
        
        
        
        

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CTRLr(unit, 0x20a));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_CTRLr(unit, 0x28a));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_CTRLr(unit, 0x28a));


        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 84);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_DIVr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 84);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_CLK_DIVr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 84);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_CLK_DIVr(unit, rval));


        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x17F6088);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_PARAMSr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x17F6088);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_CLK_PARAMSr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x17F6088);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_CLK_PARAMSr(unit, rval));

        } else {

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x04444447)); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x22222334));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x33433222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x33333333));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x22222233));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x55222222));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x46655555));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x42444444));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x66666664));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_88_95r(unit, 0x46666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_96_103r(unit, 0x66666666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_104_111r(unit, 0x55220666));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_112_119r(unit, 0x62633445));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x24000000));

          

          
        
        
        
        
        

          
                
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 4); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 3); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 2); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, 1); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 8); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 7); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 6); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 5); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  12); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  11); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 10); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f,  9); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 20); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 19); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 18); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 17); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 16); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 15); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 14); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 13); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 24); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 23); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 22); 
        soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 21); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit,   rval));

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, 0x0));

          
                
        
                
        
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 12); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 11); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 10); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f,  9); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 4); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 3); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 2); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 1); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  8); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  7); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 6); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, 5); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 25); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 21); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 17); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 13); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 41); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 37); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 33); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 29); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 57); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 53); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 49); 
        soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 45); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r(unit,   rval));

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r(unit, 0x0));

          
                
        
                
        

        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, 12); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, 11); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, 10); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f,  9); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, 4); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, 3); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, 2); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, 1); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  8); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  7); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, 6); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, 5); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, 25); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, 21); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, 17); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, 13); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, 41); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, 37); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, 33); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, 29); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r(unit,   rval));

        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, 57); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, 53); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, 49); 
        soc_reg_field_set(unit, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, 45); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r(unit,   rval));

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r(unit, 0x0));

          
            
        
        
        
   
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CTRLr(unit, 0x20a));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_CTRLr(unit, 0x28a));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_CTRLr(unit, 0x28a));  


            
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 72); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_DIVr(unit, rval)); 
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 72); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_CLK_DIVr(unit, rval)); 
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_CLK_DIVr, &rval, LEDCLK_HALF_PERIODf, 72); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_CLK_DIVr(unit, rval)); 


          
        
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP0_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x1499700); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CLK_PARAMSr(unit, rval)); 
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP1_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x1499700); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP1_CLK_PARAMSr(unit, rval)); 
        rval = 0;
        soc_reg_field_set(unit, CMIC_LEDUP2_CLK_PARAMSr, &rval, REFRESH_CYCLE_PERIODf, 0x1499700); 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP2_CLK_PARAMSr(unit, rval)); 
         
    }

    
    SOCDNX_IF_ERR_EXIT(soc_arad_core_frequency_config_get(unit, SOC_JER_CORE_FREQ_KHZ_DEFAULT, &core_freq));
    SOCDNX_IF_ERR_EXIT(soc_arad_schan_timeout_config_get(unit, &schan_timeout));
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_sbus_timeout_set(unit, core_freq, schan_timeout));

    

    

    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, -1); 
    if (dividend == -1) 
    {
        
        dividend =  SOC_DPP_IMP_DEFS_GET(unit, mdio_int_dividend_default);

    }

    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, -1); 
    if (divisor == -1) 
    {
        
        mdio_int_freq = SOC_DPP_IMP_DEFS_GET(unit, mdio_int_freq_default);
        divisor = core_freq * dividend / (2* mdio_int_freq);

    }

    mdio_delay = SOC_DPP_IMP_DEFS_GET(unit, mdio_int_out_delay_default);

    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_mdio_config(unit,dividend,divisor,mdio_delay));

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC0_SCHAN_ERRr(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC1_SCHAN_ERRr(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC2_SCHAN_ERRr(unit, 0));

    
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_mdio_set(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_reset(
    int unit,
    int reset_action)
{
    int disable_hard_reset = 0x0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_dcmn_iproc_config_paxb(unit));

    
    disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0);
    if (disable_hard_reset == 0) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_device_hard_reset(unit, reset_action));
    }

    SOCDNX_IF_ERR_EXIT(soc_dcmn_iproc_config_paxb(unit));

    
    soc_endian_config(unit);
    soc_pci_ep_config(unit, 0);

        
    if (soc_feature(unit, soc_feature_cmicm)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_reset_cmic_regs(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_rcpu_base_q_pair_init(int unit, int port_i)
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
int soc_jer_rcpu_reply_channel_set(int unit, int channel)
{
    uint32 rval;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IRE_CPU_CHANNEL_CONFIGURATIONr(unit, &rval));
    soc_reg_field_set(unit, IRE_CPU_CHANNEL_CONFIGURATIONr, &rval, CPU_CHANNELf, channel);
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_CPU_CHANNEL_CONFIGURATIONr(unit, rval));

    exit:
         SOCDNX_FUNC_RETURN;
}

int soc_jer_rcpu_init(int unit, soc_dpp_config_t *dpp)
{
    int port_i = 0;
    uint32 rcpu_reply_done = 0, channel;
    soc_error_t rv;
    ARAD_PORT_HEADER_TYPE header_type_in;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(dpp->arad->init.rcpu.slave_port_pbmp, port_i) 
    {
        if ( !rcpu_reply_done ) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_in.get(unit, port_i, &header_type_in);
            SOCDNX_IF_ERR_EXIT(rv);
            if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_ETH) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port_i, &channel));
                SOCDNX_IF_ERR_EXIT(soc_jer_rcpu_reply_channel_set(unit, channel));
                rcpu_reply_done = 1;
            }
        }
        SOCDNX_IF_ERR_EXIT(soc_jer_rcpu_base_q_pair_init(unit, port_i));
    }

    exit:
         SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_str_prop_ingress_congestion_management_guarantee_mode_get(int unit, SOC_TMC_ITM_CGM_MGMT_GUARANTEE_MODE *ingress_congestion_management_guarantee_mode)
{
    char *propkey, *propval;
    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_INGRESS_CONGESTION_MANAGEMENT;
    propval = soc_property_suffix_num_str_get(unit, -1, propkey, "guarantee_mode");

    if (propval) {
        if (sal_strcmp(propval, "STRICT") == 0) {
            *ingress_congestion_management_guarantee_mode = SOC_TMC_ITM_CGM_MGMT_GUARANTEE_STRICT;
        } else if (sal_strcmp(propval, "LOOSE") == 0) {
            *ingress_congestion_management_guarantee_mode = SOC_TMC_ITM_CGM_MGMT_GUARANTEE_LOOSE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *ingress_congestion_management_guarantee_mode = SOC_TMC_ITM_CGM_MGMT_GUARANTEE_STRICT;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC void 
soc_set_bcm88x7x(int unit, int core, int value) 
{
    uint32 cfg_ctrl;

    if (core == 0) {
        READ_MHOST_0_CR5_CFG_CTRLr(unit, &cfg_ctrl);
        soc_reg_field_set(unit, MHOST_0_CR5_CFG_CTRLr, &cfg_ctrl,
                          RSVDf, value);
        WRITE_MHOST_0_CR5_CFG_CTRLr(unit, cfg_ctrl);
    } else if (core == 1) { 
        READ_MHOST_1_CR5_CFG_CTRLr(unit, &cfg_ctrl);
        soc_reg_field_set(unit, MHOST_1_CR5_CFG_CTRLr, &cfg_ctrl,
                          RSVDf, value);
        WRITE_MHOST_1_CR5_CFG_CTRLr(unit, cfg_ctrl);
    } else {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: CORE is 0 - 1\n")) );
    }        
}


STATIC void
soc_get_bcm88x7x(int unit, int core, int *value)
{
    uint32 cfg_ctrl;

    if (core == 0) {
        READ_MHOST_0_CR5_CFG_CTRLr(unit, &cfg_ctrl);
        *value = soc_reg_field_get(unit, MHOST_0_CR5_CFG_CTRLr, cfg_ctrl, RSVDf);
    } else if (core == 1) {
        READ_MHOST_1_CR5_CFG_CTRLr(unit, &cfg_ctrl);
        *value = soc_reg_field_get(unit, MHOST_1_CR5_CFG_CTRLr, cfg_ctrl, RSVDf);
    } else {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: CORE is 0 - 1\n")) );
    }
}


void
soc_reset_bcm88x7x(int unit, int core) 
{
    soc_set_bcm88x7x(unit, core, 1);
}


void 
soc_restore_bcm88x7x(int unit, int core) 
{
    soc_set_bcm88x7x(unit, core, 0);
}


int
soc_jer_tcam_parity_machine_enable(int unit)
{
    uint64 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_PPDB_A_TCAM_PARITY_MACHINE_CONFIGURATIONr(unit, &reg_val));
    soc_reg64_field32_set(unit, PPDB_A_TCAM_PARITY_MACHINE_CONFIGURATIONr, &reg_val, TCAM_PARITY_BANK_BITMAPf, 0xFFFF);
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_TCAM_PARITY_MACHINE_CONFIGURATIONr(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_tcam_bist_check(int unit)
{
    int i,j,k;
    uint32 value;
    uint64 reg_val64;
    uint32 max_bank = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QUX(unit)) {
        max_bank = TCAM_BIST_CHECK_BANK_QUX;
    } else {
        max_bank = TCAM_BIST_CHECK_BANK_JER;
    }

    for (j=0; j<MAX_TCAM_MODE_JER; j++) {
        if (j != INVALID_TCAM_MODE_JER) {
            for (i=0; i< max_bank; i++) {
                SOCDNX_IF_ERR_EXIT(READ_PPDB_A_REG_0526r(unit, &reg_val64));
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, PPDB_A_REG_0526r, 0, 0, &reg_val64));
                if(SOC_IS_QUX(unit)) {
                    soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_56_61f, 0x3f);
                } else {
                    soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_56_63f, 0xff);
                }
                soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_0_3f, i);
                soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_4_5f, j);
                SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_REG_0526r(unit, reg_val64));

                sal_usleep(10000);

                SOCDNX_IF_ERR_EXIT(READ_PPDB_A_REG_0528r(unit, &value));
                if (value != 3 ) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Running TCAM BIST FAILED: got %u err in tcam bank %d bist mode %d test\n"),value,i,j));
                }
            }

            for (i=0; i< max_bank; i++) {
                k =  1 << i ;
                SOCDNX_IF_ERR_EXIT(READ_PPDB_A_REG_0526r(unit, &reg_val64));
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, PPDB_A_REG_0526r, 0, 0, &reg_val64));
                if(SOC_IS_QUX(unit)) {
                    soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_56_61f, k);
                } else {
                    soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_56_63f, k);
                }
                soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_0_3f, i);
                soc_reg64_field32_set(unit, PPDB_A_REG_0526r, &reg_val64, FIELD_4_5f, j);
                SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_REG_0526r(unit, reg_val64));

                sal_usleep(10000);

                SOCDNX_IF_ERR_EXIT(READ_PPDB_A_REG_0528r(unit, &value));
                if (value != 3 ) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Running TCAM BIST FAILED: got %u err in tcam bank %d bist mode %d test\n"),value,i,j));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_getstatus_bcm88x7x(int unit, int core)
{
    int value = 0;
    soc_get_bcm88x7x(unit, core, &value);

    return value;
}


int soc_dpp_jericho_init(
    int unit,
    int reset_action)
{

    soc_dpp_config_t *dpp = NULL;
    int  schan_intr_enb;
    uint32 bist_enable,tcam_bist_enable;

#ifdef JERICHO_HW_IMPLEMENTATION
    int silent = 0;
    int port_i, tm_port;
    int header_type_in;
    uint32 base_q_pair, rval;
#endif                          

    SOCDNX_INIT_FUNC_DEFS;
    DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "entering soc_dpp_jericho_init");

    if (SOC_CONTROL(unit) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("SOC_CONTROL() is not allocated.")));
    }

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Jericho function. Invalid Device")));
    }

    dpp = SOC_DPP_CONFIG(unit);
    if ((dpp == NULL) || (dpp->arad == NULL) || (dpp->jer == NULL)) {         
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("Soc control structures are not allocated.")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#else
  #if defined(BCM_QAX_SUPPORT) || defined(BCM_JERICHO_SUPPORT) || defined(BCM_QUX_SUPPORT) 
    if (SOC_IS_QAX(unit) || SOC_IS_QMX(unit) || SOC_IS_QUX(unit)) {
        
        SOCDNX_IF_ERR_EXIT(WRITE_MHOST_0_CR5_RST_CTRLr(unit, 0));
        SOCDNX_IF_ERR_EXIT(WRITE_MHOST_1_CR5_RST_CTRLr(unit, 0));
        
        SOCDNX_IF_ERR_EXIT(WRITE_CHIPCOMMONG_UART0_UART_SRRr(unit,1));
        SOCDNX_IF_ERR_EXIT(WRITE_CHIPCOMMONG_UART1_UART_SRRr(unit,1));
        sal_usleep(10000);
        
        SOCDNX_IF_ERR_EXIT(WRITE_MHOST_0_CR5_RST_CTRLr(unit, 0xf));
        SOCDNX_IF_ERR_EXIT(WRITE_MHOST_1_CR5_RST_CTRLr(unit, 0xf));
    }
  #endif
#endif
    

    if (!SOC_IS_FLAIR(unit)) {
        soc_reset_bcm88x7x(unit, 0);
    }

    sal_usleep(50000); 
    soc_uc_reset(unit, 0);

    SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_ports(unit));

    
    DCMN_RUNTIME_DEBUG_PRINT(unit);
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Read SOC property Configuration\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_dpp_get_default_config_jer(unit));

    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_direct(unit));

    SOCDNX_IF_ERR_EXIT(soc_arad_info_config(unit));

    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_derived(unit));

    
    sal_dpc_enable(INT_TO_PTR(unit));
    
    if (!SOC_WARM_BOOT(unit)) {
        DCMN_RUNTIME_DEBUG_PRINT(unit);

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Device Reset and Access Enable\n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_init_reset(unit, reset_action));
        if (reset_action == SOC_DPP_RESET_ACTION_IN_RESET) {
            SOC_EXIT;
        }
    }
     
    soc_jer_cmic_interrupts_disable(unit);
    
#ifdef BCM_CMICM_SUPPORT
    SOCDNX_IF_ERR_EXIT(_soc_mem_address_remap_allocation_init(unit));
#endif    
    
    
    schan_intr_enb = SOC_CONTROL(unit)->schanIntrEnb;
    SOC_CONTROL(unit)->schanIntrEnb = 0;
    
    if (!SOC_WARM_BOOT(unit) && !SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_regs_eci_access_check(unit));
    }

    
    if (!SOC_WARM_BOOT(unit)) {
        DCMN_RUNTIME_DEBUG_PRINT(unit);

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Blocks OOR and PLL configuration\n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET));

        DISPLAY_SW_STATE_MEM ;
        DISPLAY_SW_STATE_MEM_PRINTF(("%s(): Just out of %s\r\n",__func__,"soc_jer_device_reset")) ;
    }

    bist_enable = soc_property_get(unit, spn_BIST_ENABLE, 0);
    if (!bist_enable && SOC_DPP_CONFIG(unit)->tm.various_bm & DPP_VARIOUS_BM_FORCE_MBIST_TEST) {
        bist_enable = 1;
    }
    tcam_bist_enable = soc_property_get(unit, spn_TCAM_BIST_ENABLE, 0);

    
    if (!SOC_WARM_BOOT(unit)) {
#ifdef PLISIM
       if (!SAL_BOOT_PLISIM) 
#endif
           {
               
               if (bist_enable || tcam_bist_enable) {
                   if (bist_enable) {
                       LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: running internal memories BIST\n"),unit));
                       SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_soc_bist_all,(unit, (bist_enable == 2 ? 1 : 0))));
                   }

                   if (tcam_bist_enable) {
                       LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: running TCAM BIST\n"),unit));
                       SOCDNX_IF_ERR_EXIT(soc_jer_tcam_bist_check(unit));
                   }

                   LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Device Reset and Access Enable\n"),unit));
                   SOCDNX_IF_ERR_EXIT(soc_jer_init_reset(unit, reset_action));

                   
                   SOCDNX_IF_ERR_EXIT(soc_jer_regs_eci_access_check(unit));
                   LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Blocks OOR and PLL configuration\n"),unit));
                   SOCDNX_IF_ERR_EXIT(soc_jer_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET));
                }
                
                if(SOC_IS_QAX_A0(unit)) {
                   SOCDNX_IF_ERR_EXIT(qax_mbist_fix_arm_core(unit, 0));
                }
           }
       }

       
       if (!SOC_IS_FLAIR(unit)) {
           SOCDNX_IF_ERR_EXIT(soc_jer_interrupts_init(unit));
       }
       
       SOC_CONTROL(unit)->schanIntrEnb = schan_intr_enb;

      
       SOCDNX_IF_ERR_EXIT(soc_dpp_cache_enable_init(unit));
 
       if (!SOC_WARM_BOOT(unit) && !SOC_IS_FLAIR(unit)) {

           SOCDNX_IF_ERR_EXIT(soc_jer_interrupts_disable(unit));
       }


    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_dma_init(unit));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_common_init(unit));


    if (!SOC_IS_FLAIR(unit) && !SOC_WARM_BOOT(unit)) {
        
        DCMN_RUNTIME_DEBUG_PRINT(unit);
        SOCDNX_SAND_IF_ERR_EXIT(jer_mgmt_enable_traffic_set(unit, FALSE));

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Traffic Disable\n"),unit));
        SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_all_ctrl_cells_enable_set(unit, FALSE));
    }


    
    SOCDNX_IF_ERR_RETURN(soc_linkctrl_init(unit, &soc_linkctrl_driver_jer));
         

    if (SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mcds_multicast_init2, (unit)));
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_str_prop_ingress_congestion_management_guarantee_mode_get(unit, &dpp->jer->tm.cgm_mgmt_guarantee_mode));

    if (SOC_IS_QAX(unit)) {
        dpp->qax->per_packet_compensation_legacy_mode = 
            (soc_property_get(unit, spn_INGRESS_CONGESTION_MANAGEMENT_PKT_HEADER_COMPENSATION_ENABLE, 0) == 0);
    }

    if (!SOC_WARM_BOOT(unit)) {
        arad_sw_db_is_petrab_in_system_set(unit, SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system);
    }


    
    SOCDNX_SAND_IF_ERR_EXIT(soc_jer_init_sequence_phase1(unit));

    
    if (SOC_IS_FLAIR(unit)) {
        goto exit;
    }

    
    arad_set_tcam_cache(unit, 1);

    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_dpp_common_tm_init(unit, &(dpp->tm.multicast_egress_bitmap_group_range)));
    }

    


    if (dpp->arad->init.pp_enable) {
        DCMN_RUNTIME_DEBUG_PRINT(unit);

        DISPLAY_SW_STATE_MEM ;

        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: PP Initialization\n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_arad_pp_init(unit));

        DISPLAY_SW_STATE_MEM ;
        DISPLAY_SW_STATE_MEM_PRINTF(("%s(): Just out of %s\r\n",__func__,"soc_arad_pp_init")) ;
    }

    
    if (!SOC_WARM_BOOT(unit)) 
    {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_rcpu_init(unit, dpp));

        SOCDNX_IF_ERR_EXIT(soc_dcmn_ser_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_tcam_parity_machine_enable(unit));
        if (SOC_IS_JERICHO_PLUS(unit)) {
            uint32 reg_val;
            int ii;
            soc_reg_above_64_val_t data;
            uint32 tcam_rows_num;

            
            SOC_REG_ABOVE_64_CLEAR(data);
            SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, KAPS_RPB_TCAM_CPU_COMMANDm, KAPS_BLOCK(unit, 0), data));

            if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
                tcam_rows_num = 16 * 1024;
            } else {
                tcam_rows_num = 4 * 1024;
            }

            
            for (ii = SOC_REG_INFO(unit, KAPS_RPB_TCAM_SCAN_ERROR_MASKr).first_array_index; ii < SOC_REG_INFO(unit, KAPS_RPB_TCAM_SCAN_ERROR_MASKr).first_array_index + SOC_REG_NUMELS(unit, KAPS_RPB_TCAM_SCAN_ERROR_MASKr); ii++) {
                reg_val = 0;
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, KAPS_RPB_TCAM_SCAN_ERROR_MASKr, REG_PORT_ANY, ii, reg_val));
                soc_reg_field_set(unit, KAPS_RPB_TCAM_SCAN_DEBUG_0r, &reg_val, RPB_TCAM_SCAN_SEC_ACTION_Nf, 0x2);
                soc_reg_field_set(unit, KAPS_RPB_TCAM_SCAN_DEBUG_0r, &reg_val, RPB_TCAM_SCAN_DEC_INVALID_Nf, 0x1);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, KAPS_RPB_TCAM_SCAN_DEBUG_0r, REG_PORT_ANY, ii, reg_val));

                
                reg_val = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 10 / tcam_rows_num;
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, KAPS_RPB_TCAM_SCAN_DEBUG_1r, REG_PORT_ANY, ii, reg_val));
            }

            
            reg_val = 0xF;
            WRITE_KAPS_KAPS_ERROR_REGISTERr(unit, reg_val);
        }
    }
    arad_fast_regs_and_fields_access_init(unit);

#ifdef BCM_88270_A0
#ifndef __KERNEL__
#if !defined(NO_FILEIO)
   
    if (!SOC_WARM_BOOT(unit) && SOC_IS_QUX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "extra_hash_enable", 0)) {
        char file_path[RHFILE_MAX_SIZE];
        char file_name[RHFILE_MAX_SIZE];
        sal_strcpy(file_name, DB_INIT_PEMLA_UCODE);
        SOCDNX_IF_ERR_EXIT(dbx_file_get_file_path(unit, file_name, CONF_OPEN_PER_DEVICE, file_path));
        SOCDNX_IF_ERR_EXIT(pem_meminfo_init(file_path)); 
    }
#endif
#endif
#endif

exit:

    DISPLAY_MEM ;
    DISPLAY_SW_STATE_MEM ;
    DISPLAY_MEM_PRINTF(("%s(): unit %d: Exit\r\n",__func__,unit)) ;

    SOCDNX_FUNC_RETURN;
}


int soc_dpp_jericho_deinit(
    int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

    
    soc_jer_cmic_interrupts_disable(unit);

    
    SOCDNX_IF_ERR_EXIT(soc_jer_tbls_deinit(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_deinit_dma(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_interrupts_deinit(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_info_config_custom_reg_access(int unit)
{
    soc_custom_reg_access_t* reg_access;
    SOCDNX_INIT_FUNC_DEFS;

    reg_access = &(SOC_INFO(unit).custom_reg_access);

    reg_access->custom_reg32_get = soc_jer_ilkn_reg32_get;
    reg_access->custom_reg32_set = soc_jer_ilkn_reg32_set;

    reg_access->custom_reg64_get = soc_jer_ilkn_reg64_get;
    reg_access->custom_reg64_set = soc_jer_ilkn_reg64_set;

    reg_access->custom_reg_above64_get = soc_jer_ilkn_reg_above_64_get;
    reg_access->custom_reg_above64_set = soc_jer_ilkn_reg_above_64_set;

    BCM_PBMP_ASSIGN(reg_access->custom_port_pbmp, PBMP_IL_ALL(unit)); 
   
    SOCDNX_FUNC_RETURN;
}


int soc_jer_pll_info_get(int unit, soc_dpp_pll_info_t *pll_info)
{
    int rv;
    uint32 output_buffer=0;
    uint32 field;
    soc_reg_above_64_val_t reg_val_long;
    soc_dpp_pll_t *pll_val=NULL;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(pll_info, 0, sizeof(soc_dpp_pll_info_t));

    
    pll_val=&(pll_info->core_pll);
    pll_val->ref_clk = 25; 
    pll_val->p_div   = 1;  
    SOCDNX_IF_ERR_EXIT(READ_ECI_POWERUP_CONFIGr_REG32(unit, &reg_val_long[0]));

    
    soc_sand_bitstream_get_any_field(&reg_val_long[0], 0, 8, &output_buffer);
    pll_val->n_div=output_buffer;
    output_buffer=0;
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 8, 4, &output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    pll_val->m0_div=output_buffer;
    output_buffer=0;
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_OGER_1028r(unit, reg_val_long));
        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 8, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m1_div=output_buffer;
        output_buffer=0;

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 16, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m4_div=output_buffer;
        output_buffer=0;

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 24, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m5_div=output_buffer;
        output_buffer=0;
    }

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    if (pll_val->m4_div) {
        pll_val->ch4=pll_val->vco/pll_val->m4_div;
    } else {
        pll_val->ch4=0;
    }

    
    if (pll_val->m5_div) {
        pll_val->ch5=pll_val->vco/pll_val->m5_div;
    } else {
        pll_val->ch5=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_CORE_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_CORE_PLL_STATUSr, reg_val_long[0], CORE_PLL_LOCKEDf);

    
    pll_val=&(pll_info->uc_pll);
    pll_val->ref_clk = 25; 
    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_OGER_1027r(unit, reg_val_long));
        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[1], 26, 3, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->p_div=output_buffer;
        output_buffer=0;
        if (pll_val->p_div==0) {
            pll_val->p_div=1;
        }

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[2], 3, 10, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->n_div=output_buffer;
        output_buffer=0;
        if (pll_val->n_div==0) {
            pll_val->n_div=140;
        }

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 0, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m0_div=output_buffer;
        output_buffer=0;
        if (pll_val->m0_div==0) {
            pll_val->m0_div=28;
        }

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 8, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m1_div=output_buffer;
        output_buffer=0;

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 0, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m4_div=output_buffer;
        output_buffer=0;

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 24, 8, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->m5_div=output_buffer;
        output_buffer=0;


    } else {
        
        pll_val->p_div   = 1; 
        pll_val->n_div   = 120; 
        pll_val->m0_div  = 24; 
        pll_val->m4_div  = 12; 
        pll_val->m5_div  = 6; 
    }


    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m1_div) {
        pll_val->ch1=pll_val->vco/pll_val->m1_div;
    } else {
        pll_val->ch1=0;
    }

    
    if (pll_val->m4_div) {
        pll_val->ch4=pll_val->vco/pll_val->m4_div;
    } else {
        pll_val->ch4=0;
    }

    
    if (pll_val->m5_div) {
        pll_val->ch5=pll_val->vco/pll_val->m5_div;
    } else {
        pll_val->ch5=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_UC_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_UC_PLL_STATUSr, reg_val_long[0], UC_PLL_LOCKEDf);

     
    pll_val=&(pll_info->ts_pll);
    pll_val->ref_clk = 25; 
    SOCDNX_IF_ERR_EXIT(READ_ECI_TS_PLL_CONFIGr(unit, reg_val_long));
    
    pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_TS_PLL_CONFIGr, reg_val_long, TS_PLL_CFG_PDIVf);
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }

    
    pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_TS_PLL_CONFIGr, reg_val_long, TS_PLL_CFG_NDIVf);
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_TS_PLL_CONFIGr, reg_val_long, TS_PLL_CFG_CH_0_MDIVf);
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_TS_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_TS_PLL_STATUSr, reg_val_long[0], MISC_PLL_0_LOCKEDf);

    if (SOC_IS_QAX(unit)) {
        
        pll_val=&(pll_info->bs_pll[0]);
        pll_val->ref_clk = 25; 
        SOCDNX_IF_ERR_EXIT(READ_ECI_BS_0_PLL_CONFIGr(unit, reg_val_long));
        
        pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_BS_0_PLL_CONFIGr, reg_val_long, BS_0_PLL_PDIVf);
        if (pll_val->p_div==0) {
            pll_val->p_div=1;
        }   

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 0, 10, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->n_div=output_buffer;
        output_buffer=0;
        if (pll_val->n_div==0) {
            pll_val->n_div=120;
        }

        
        pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_BS_0_PLL_CONFIGr, reg_val_long, BS_0_PLL_CH_0_MDIVf);
        if (pll_val->m0_div==0) {
            pll_val->m0_div=120;
        }

        
        pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

        
        pll_val->ch0=pll_val->vco/pll_val->m0_div;

        
        reg_val_long[0]=0;
        SOCDNX_IF_ERR_EXIT(READ_ECI_BS_0_PLL_STATUSr(unit, &reg_val_long[0]));
        pll_val->locked=soc_reg_field_get(unit, ECI_BS_0_PLL_STATUSr, reg_val_long[0], MISC_PLL_1_LOCKEDf);

        
        pll_val=&(pll_info->bs_pll[1]);
        pll_val->ref_clk = 25; 
        SOCDNX_IF_ERR_EXIT(READ_ECI_BS_1_PLL_CONFIGr(unit, reg_val_long));
        
        pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_BS_1_PLL_CONFIGr, reg_val_long, BS_1_PLL_PDIVf);
        if (pll_val->p_div==0) {
            pll_val->p_div=1;
        }   

        
        rv=soc_sand_bitstream_get_any_field(&reg_val_long[0], 0, 10, &output_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        pll_val->n_div=output_buffer;
        output_buffer=0;
        if (pll_val->n_div==0) {
            pll_val->n_div=120;
        }

                
        pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_BS_1_PLL_CONFIGr, reg_val_long, BS_1_PLL_CH_0_MDIVf);
        if (pll_val->m0_div==0) {
            pll_val->m0_div=120;
        }

        
        pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

        
        pll_val->ch0=pll_val->vco/pll_val->m0_div;

        
        reg_val_long[0]=0;
        SOCDNX_IF_ERR_EXIT(READ_ECI_BS_1_PLL_STATUSr(unit, &reg_val_long[0]));
        pll_val->locked=soc_reg_field_get(unit, ECI_BS_1_PLL_STATUSr, reg_val_long[0], MISC_PLL_2_LOCKEDf);

    } else {
        
        pll_val=&(pll_info->bs_pll[0]);
        pll_val->ref_clk = 25; 
        SOCDNX_IF_ERR_EXIT(READ_ECI_BS_PLL_CONFIGr(unit, reg_val_long));
        
        pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_BS_PLL_CONFIGr, reg_val_long, BS_PLL_CFG_PDIVf);
        if (pll_val->p_div==0) {
            pll_val->p_div=8;
        }   

        
        pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_BS_PLL_CONFIGr, reg_val_long, BS_PLL_CFG_NDIVf);
        if (pll_val->n_div==0) {
            pll_val->n_div=1024;
        }

        
        pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_BS_PLL_CONFIGr, reg_val_long, BS_PLL_CFG_CH_0_MDIVf);
        if (pll_val->m0_div==0) {
            pll_val->m0_div=256;
        }

        
        pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

        
        pll_val->ch0=pll_val->vco/pll_val->m0_div;

        
        reg_val_long[0]=0;
        SOCDNX_IF_ERR_EXIT(READ_ECI_BS_PLL_STATUSr(unit, &reg_val_long[0]));
        pll_val->locked=soc_reg_field_get(unit, ECI_BS_PLL_STATUSr, reg_val_long[0], MISC_PLL_1_LOCKEDf);
    }       

    
    pll_val=&(pll_info->pml_pll[0]);
    pll_val->ref_clk = 156.25;
    SOCDNX_IF_ERR_EXIT(READ_ECI_NIF_PML_0_PLL_CONFIGr(unit, reg_val_long));
    if (SOC_IS_QAX(unit)) {
        field=PML_0_PLL_PDIVf;
    } else {
        field=NIF_PML_0_PLL_CFG_PDIVf;
    }
    
    pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }   

    
    if (SOC_IS_QAX(unit)) {
        field=PML_0_PLL_FBDIV_NDIV_INTf;
    } else {
        field=NIF_PML_0_PLL_CFG_NDIVf;
    }
    pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    if (SOC_IS_QAX(unit)) {
        field=PML_0_PLL_CH_0_MDIVf;
    } else {
        field=NIF_PML_0_PLL_CFG_CH_0_MDIVf;
    }

    pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    if (SOC_IS_QAX(unit)) {
        pll_val->m4_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val_long, PML_0_PLL_CH_4_MDIVf);
    }

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m4_div) {
        pll_val->ch4=pll_val->vco/pll_val->m4_div;
    } else {
        pll_val->ch4=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_PML_0_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_PML_0_PLL_STATUSr, reg_val_long[0], MISC_PLL_5_LOCKEDf);

    
    pll_val=&(pll_info->pml_pll[1]);
    pll_val->ref_clk = 156.25;
    SOCDNX_IF_ERR_EXIT(READ_ECI_NIF_PML_1_PLL_CONFIGr(unit, reg_val_long));
    if (SOC_IS_QAX(unit)) {
        field=PML_1_PLL_PDIVf;
    } else {
        field=NIF_PML_1_PLL_CFG_PDIVf;
    }
    
    pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }   

    
    if (SOC_IS_QAX(unit)) {
        field=PML_1_PLL_FBDIV_NDIV_INTf;
    } else {
        field=NIF_PML_1_PLL_CFG_NDIVf;
    }

    pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    if (SOC_IS_QAX(unit)) {
        field=PML_1_PLL_CH_0_MDIVf;
    } else {
        field=NIF_PML_1_PLL_CFG_CH_0_MDIVf;
    }

    pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    if (SOC_IS_QAX(unit)) {
        pll_val->m4_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val_long, PML_1_PLL_CH_4_MDIVf);
    }

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m4_div) {
        pll_val->ch4=pll_val->vco/pll_val->m4_div;
    } else {
        pll_val->ch4=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_PML_1_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_PML_1_PLL_STATUSr, reg_val_long[0], MISC_PLL_6_LOCKEDf);

    
    pll_val=&(pll_info->pmh_pll);
    pll_val->ref_clk = 156.25;
    SOCDNX_IF_ERR_EXIT(READ_ECI_NIF_PMH_PLL_CONFIGr(unit, reg_val_long));
    if (SOC_IS_QAX(unit)) {
        field=PMH_PLL_PDIVf;
    } else {
        field=NIF_PMH_PLL_CFG_PDIVf;
    }

    
    pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->p_div==0) {
        pll_val->p_div=8;
    }   

    
    if (SOC_IS_QAX(unit)) {
        field=PMH_PLL_FBDIV_NDIV_INTf;
    } else {
        field=NIF_PMH_PLL_CFG_NDIVf;
    }

    pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->n_div==0) {
        pll_val->n_div=1024;
    }

    
    if (SOC_IS_QAX(unit)) {
        field=PMH_PLL_CH_0_MDIVf;
    } else {
        field=NIF_PMH_PLL_CFG_CH_0_MDIVf;
    }

    pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val_long, field);
    if (pll_val->m0_div==0) {
        pll_val->m0_div=256;
    }

    if (SOC_IS_QAX(unit)) {
        pll_val->m4_div=soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val_long, PMH_PLL_CH_4_MDIVf);
    }

    
    pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

    
    pll_val->ch0=pll_val->vco/pll_val->m0_div;

    
    if (pll_val->m4_div) {
        pll_val->ch4=pll_val->vco/pll_val->m4_div;
    } else {
        pll_val->ch4=0;
    }

    
    reg_val_long[0]=0;
    SOCDNX_IF_ERR_EXIT(READ_ECI_PMH_PLL_STATUSr(unit, &reg_val_long[0]));
    pll_val->locked=soc_reg_field_get(unit, ECI_PMH_PLL_STATUSr, reg_val_long[0], MISC_PLL_4_LOCKEDf);

    if (!SOC_IS_QAX(unit)) {
        
        pll_val=&(pll_info->fab_pll[0]);
        pll_val->ref_clk = 156.25;
        SOCDNX_IF_ERR_EXIT(READ_ECI_FAB_0_PLL_CONFIGr(unit, reg_val_long));
        
        pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val_long, FAB_0_PLL_CFG_PDIVf);
        if (pll_val->p_div==0) {
            pll_val->p_div=8;
        }   

        
        pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val_long, FAB_0_PLL_CFG_NDIVf);
        if (pll_val->n_div==0) {
            pll_val->n_div=1024; 
        }

        
        pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val_long, FAB_0_PLL_CFG_CH_0_MDIVf);
        if (pll_val->m0_div==0) {
            pll_val->m0_div=256;
        }   

        
        pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

        
        pll_val->ch0=pll_val->vco/pll_val->m0_div;

        
        reg_val_long[0]=0;
        SOCDNX_IF_ERR_EXIT(READ_ECI_FAB_0_PLL_STATUSr(unit, &reg_val_long[0]));
        pll_val->locked=soc_reg_field_get(unit, ECI_FAB_0_PLL_STATUSr, reg_val_long[0], MISC_PLL_2_LOCKEDf);

        
        pll_val=&(pll_info->fab_pll[1]);
        pll_val->ref_clk = 156.25;
        SOCDNX_IF_ERR_EXIT(READ_ECI_FAB_0_PLL_CONFIGr(unit, reg_val_long));
        
        pll_val->p_div=soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val_long, FAB_1_PLL_CFG_PDIVf);
        if (pll_val->p_div==0) {
            pll_val->p_div=8;
        }   

        
        pll_val->n_div=soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val_long, FAB_1_PLL_CFG_NDIVf);
        if (pll_val->n_div==0) {
            pll_val->n_div=1024; 
        }

        
        pll_val->m0_div=soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val_long, FAB_1_PLL_CFG_CH_0_MDIVf);
        if (pll_val->m0_div==0) {
            pll_val->m0_div=256;
        }   

        
        pll_val->vco=pll_val->ref_clk/pll_val->p_div*pll_val->n_div;

        
        pll_val->ch0=pll_val->vco/pll_val->m0_div;

        
        reg_val_long[0]=0;
        SOCDNX_IF_ERR_EXIT(READ_ECI_FAB_1_PLL_STATUSr(unit, &reg_val_long[0]));
        pll_val->locked=soc_reg_field_get(unit, ECI_FAB_1_PLL_STATUSr, reg_val_long[0], MISC_PLL_3_LOCKEDf);
    }


exit:
  SOCDNX_FUNC_RETURN;
}
