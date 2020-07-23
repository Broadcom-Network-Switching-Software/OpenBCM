/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in init related tests
 */



#include <appl/diag/system.h> /* for parse_small_integers */
#include <ibde.h>

#include <shared/bsl.h>

#ifdef BCM_PETRA_SUPPORT

#include <soc/debug.h>
#include <soc/cmext.h>
#include <soc/dpp/dpp_config_defs.h>
#include <shared/bsl.h>
#include <bcm/stack.h>
#include <bcm/l2.h>
#include <bcm/trunk.h>
#include <bcm/vswitch.h>
#include <bcm/mpls.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <bcm/policer.h>
#include <soc/drv.h>
#if defined(INCLUDE_KBP)
#include <soc/dpp/drv.h>
#endif /* defined(INCLUDE_KBP) */
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/link.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/bslenable.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/diag/dcmn/init_deinit.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if (defined(BCM_WARM_BOOT_SUPPORT) || defined(BCM_ESW_SUPPORT))
#include <shared/swstate/sw_state.h>
#endif /* (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)) */

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP)
#include <appl/diag/dpp/kbp.h>
#endif /* defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) */


char exhaustive_test_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"exhaustive Test Usage: TODO:add\n"
#else
"exhaustive Test Usage: TODO:add\n"
#endif
;

#define EXHAUSTIVE_ADD_SOC_PROPERTY(prop)\
    sh_process_command(unit, prop);\
    cli_out(prop);\
    cli_out("\n");\
    prop_added++;

char exhaustive_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"Exhaustive Usage:\n"
"  Kbp=<value>, Meter=<value>, Kaps=<value>.\n"
#else
"application Init/Deinit Usage:\n"
"  Kbp=<value>         1: Kbp sequence will be performed on the chip. (default=0)\n"
"  Meter=<value>       1: Metering sequence will be performed on the chip. (default=0)\n"
"  Kaps=<value>        1: Kaps sequence will be performed on the chip. (default=0)\n"
#endif
;

typedef struct exhaustive_init_param_s {
    uint32 unit;
    int32  Kbp;
    int32  Kaps;
    int32  Oam;
    int32  Lem;
    int32  Lif;
    int32  Eedb;
    int32  Fec;
    int32  Ecmp;
    int32  Meter;
    int32  Counter;
    int32  Ser;
    int32  ExactMatchShadow;
    int32  Acl;
    int32  Mpls;
} exhaustive_init_param_t;

STATIC exhaustive_init_param_t exhaustive[SOC_MAX_NUM_DEVICES];
#define EX_TEST_FIELD_TCAM_NUM_OF_ENTRIES 24000

int exaustive_test_field_group_tcam(int unit, int group_priority)
{
  bcm_error_t result;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
/*  bcm_field_data_qualifier_t data_qual;*/
  bcm_mac_t macData;
  bcm_mac_t macMask;
  int num_of_groups = 6;
  int num_of_enries_per_group = EX_TEST_FIELD_TCAM_NUM_OF_ENTRIES / num_of_groups /*4000*/;
  int index;
  bcm_field_entry_t *ent_arr = NULL;
  int i, j;
  bcm_field_group_t grp_tcam;


  /*
   * Define the QSET.
   * No need to use IsEqual as qualifier for this field group.
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);

  for (j = 0 ; j < num_of_groups ; group_priority++/*Number of groups*/, j++)
  {
      /*  Create the Field group with type TCAM */
      result = bcm_field_group_create(unit, qset, group_priority, &grp_tcam);
      if (BCM_E_NONE != result) {
          cli_out("Error in bcm_field_group_create\n");
          goto exit;
      }

      /*
       *  Define the ASET - use counter 0.
       */
      BCM_FIELD_ASET_INIT(aset);
/*      BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);*/
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel1);
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);

      /*  Attach the action set */
      result = bcm_field_group_action_set(unit, grp_tcam, aset);
      if (BCM_E_NONE != result) {
          cli_out("Error in bcm_field_group_action_set for group %d\n", grp_tcam);
          goto exit;
      }

      cli_out("BCM Field Group ID: %d\n", grp_tcam);

      ent_arr = sal_alloc(sizeof(bcm_field_entry_t) * EX_TEST_FIELD_TCAM_NUM_OF_ENTRIES, "exaustive_test_field_group_tcam.ent_arr");
      for (i = 0; i < num_of_enries_per_group; i++)
      {
    	  index = j/*num_of_groups*/ * num_of_enries_per_group + i;
          result = bcm_field_entry_create(unit, grp_tcam, &(ent_arr[index]));
          if (BCM_E_NONE != result) {
              cli_out("Error in bcm_field_entry_create\n");
              goto exit;
          }

          macData[0] = 0x0;
          macData[1] = 0x0;
          macData[2] = 0x0;
          macData[3] = 0x0;
          macData[4] = 0x0;
          macData[5] = 0x1;
          macMask[0] = 0xff;
          macMask[1] = 0xff;
          macMask[2] = 0xff;
          macMask[3] = 0xff;
          macMask[4] = 0xff;
          macMask[5] = 0xff;
          result = bcm_field_qualify_SrcMac(unit, ent_arr[index], macData, macMask);
          if (BCM_E_NONE != result) {
              cli_out("Error in bcm_field_qualify_SrcMac\n");
              goto exit;
          }

          result = bcm_field_qualify_EtherType(unit, ent_arr[index], 0x821, 0xffff);
          if (BCM_E_NONE != result) {
              cli_out("Error in bcm_field_qualify_EtherType\n");
              goto exit;
          }


          result = bcm_field_entry_prio_set(unit, ent_arr[index], EX_TEST_FIELD_TCAM_NUM_OF_ENTRIES - i);
          if (BCM_E_NONE != result) {
              cli_out("Error in bcm_field_entry_prio_set, index %d prio %d, i %d\n", index, EX_TEST_FIELD_TCAM_NUM_OF_ENTRIES - i, i);
              goto exit;
          }

          result = bcm_field_action_add(unit, ent_arr[index], bcmFieldActionPolicerLevel0,1000 + i, 0);
            if (BCM_E_NONE != result) {
                cli_out("Error in bcm_field_action_add\n");
                goto exit;
            }

          result = bcm_field_entry_install(unit, ent_arr[index]);
          if (BCM_E_NONE != result) {
              cli_out("Error in bcm_field_entry_install\n");
              goto exit;
          }
          if ((index % 100) == 0)
          {
              cli_out("BCM Field Group Prio: %d, group %d, index %d Entry ID: %d\n", group_priority, j, index, ent_arr[index]);
          }
      }
  }

exit:
    if(ent_arr) {
        sal_free(ent_arr);
    }
    return result;
} /* generic_field_group_tcam */

int
silent_vlan_port_create(int unit, bcm_vlan_port_t *vlan_port){
    int rv;
    bsl_severity_t severity_vlan, severity_common;

    severity_vlan = bslenable_get(bslLayerBcm, bslSourceVlan);
    severity_common = bslenable_get(bslLayerBcm, bslSourceCommon);
    bslenable_set(bslLayerBcm, bslSourceVlan, 0);
    bslenable_set(bslLayerBcm, bslSourceCommon, 0);
    rv = bcm_vlan_port_create(unit, vlan_port);
    bslenable_set(bslLayerBcm, bslSourceVlan, severity_vlan);
    bslenable_set(bslLayerBcm, bslSourceVlan, severity_common);

    return rv;
}


/*
 * Function:     exhaustive_test
 * Purpose:      memory measurement of exhaustive configurations
 * Parameters:    u - unit #.
 *        a - pointer to arguments.
 *        p - ignored cookie.
 * Returns:    0
 */
int
exhaustive_test(int unit, args_t *a, void *p)
{
    int rv = 0x0;
    uint8 prop_added = 0;

    exhaustive_init_param_t *init_param = p;

    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(p);

    cli_out("Adding soc properties for Exhaustive Configurations\n");

    /**********************/
    /* add soc properties */
    /**********************/
    /* 
     * for every property added do: 
     * 1. sh_process_command(unit, "config add <property>"); 
     * 2. prop_added++ 
     */
    if (init_param->Meter) {
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add policer_ingress_count=64");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add policer_ingress_sharing_mode=0");
    }

    if (init_param->Ser) {
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add mem_cache_enable_all=1");
    }

    if (init_param->Kaps) {
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add private_ip_frwrd_table_size=1");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add public_ip_frwrd_table_size=100000");
    }

    if (init_param->Kbp) {        
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ucode_port_32.BCM88675=ILKN1:core_0.32:kbp");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ilkn_num_lanes_1.BCM88650=12");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add port_init_speed_il.BCM88650=10312");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_tcam_dev_type=NL88650");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_ilkn_reverse=0");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add rate_ext_mdio_divisor=0x36");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add rate_ext_mdio_dividend=1");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_ip4_uc_rpf_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_ip4_mc_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_ip6_uc_rpf_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_ip6_uc_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_ip6_mc_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_trill_uc_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_trill_mc_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_mpls_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_coup_mpls_fwd_table_size=1000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ext_acl160_table_size=2000");
    }

    if (init_param->Oam) {
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add num_oamp_ports.0=1");
        /*EXHAUSTIVE_ADD_SOC_PROPERTY("config add runtime_performance_optimize_enable_sched_allocation.0=0");*/
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add tm_port_header_type_out_232.0=CPU");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add tm_port_header_type_out_0.0=CPU");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add ucode_port_40.0=RCY.0:core_0.40");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add oam_rcy_port.0=40");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add tm_port_header_type_in_40.0=TM");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add tm_port_header_type_out_40.0=ETH");
        /*EXHAUSTIVE_ADD_SOC_PROPERTY("config add bcm886xx_next_hop_mac_extension_enable.0=0");*/
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add mplstp_g8113_channel_type.0=0x8902");
        /*EXHAUSTIVE_ADD_SOC_PROPERTY("config add bcm886xx_ipv6_tunnel_enable.0=0");*/
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add phy_1588_dpll_phase_initial_lo.0=0x40000000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add phy_1588_dpll_phase_initial_hi.0=0x10000000");
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add phy_1588_dpll_frequency_lock.0=1");
    }

    if (init_param->ExactMatchShadow) {
        EXHAUSTIVE_ADD_SOC_PROPERTY("config add exact_match_tables_shadow_enable=1");
    }

    if (prop_added > 0) {
        /* reinit to apply soc property changes */
        appl_dcmn_init_param_t deinit_init_param;
        sal_memset(&deinit_init_param, 0x0, sizeof(appl_dcmn_init_param_t));
        deinit_init_param.nof_devices = -1;
        deinit_init_param.base_modid = -1;
        deinit_init_param.elk_ilkn_rev = 1;
        deinit_init_param.elk_mdio_id = 257;
        deinit_init_param.appl_traffic_enable_stage = 1;

        cli_out("reset device to apply soc properties changes\n");

        appl_dcmn_deinit(unit, &deinit_init_param);
        appl_dcmn_init(unit, &deinit_init_param);
    }


#if (0)
/* { */
    /*
     * This code is temporarily taken out since jer_kaps_sync() calls kbp_device_save_state()
     * which blocks any further API calls to Kaps. (See sh_warmboot())
     */
    cli_out("\nDumping refference memory sizes before makeing any stress configuration.\n");
    sh_process_command(unit, "warmboot memoryuse");
/* } */
#endif

    /***********************************/
    /* do the exhaustive configuration */
    /***********************************/

    if (init_param->Counter) {
        cli_out("\nCounters: nothing to do - default counter configuration is exhaustive.\n");
    }

    if (init_param->Ser) {
        cli_out("\nSER: all HW cach tables are enabled.\n");
    }

    if (init_param->ExactMatchShadow) {
        cli_out("\nExact Match Shadow: enabled.\n");
    }


    /* OAM */
    /*-----*/
    if (init_param->Oam) {
#if defined(BCM_PETRA_SUPPORT)
        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("\nOAM: init OAM, default init is exhaustive.\n");
        rv = bcm_oam_init(unit);
        if (rv != BCM_E_NONE) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "bcm_oam_init() Failed\n"))); 
           goto done;
        }

        cli_out("\nBFD: init BFD, default init is exhaustive.\n");
        rv = bcm_bfd_init(unit);
        if (rv != BCM_E_NONE) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "bcm_bfd_init() Failed\n"))); 
           goto done;
        }
#else
        cli_out("\nOAM/BFD: supported only in Jericho/Arad.\n");
#endif /*defined(BCM_PETRA_SUPPORT)*/
    }

    /*  KAPS */
    /*-------*/
    if (init_param->Kaps) {

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("\nKAPS: adding routes.\n");
        /*kbp test_ipv4_random <EntryNum> <EntryNumPrintMod> <CachedEntNum> <WarmBoot> <Update> <Delete>*/
        sh_process_command(unit, "kbp test_ipv4_random EntryNum=230000 EntryNumPrintMod=230000 CachedEntNum=0 WarmBoot=0 Update=0 Delete=0");
        cli_out("\nKAPS: done adding routes.\n");
    }

     if (init_param->Kbp) {

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("\nKBP: adding routes.\n");        
        sh_process_command(unit, "kbp test_ipv4_random EntryNum=999 EntryNumPrintMod=999 CachedEntNum=0 WarmBoot=0 Update=0 Delete=0");
        cli_out("\nKBP: done adding routes.\n");
    }

    /* LIF + EEDB*/
    /*-----------*/
    /* only working for jericho */
    if (init_param->Lif) {
        bcm_vlan_port_t vlan_port;
        uint32 i;
        uint16 outer_vid = 1, inner_vid = 1, port_ndx = 0;
        int vsi = 5000;
        int port_list[20] = {0, 1, 13, 14, 15, 16, 17, 200, 201, 202, 203};
        int port_ndx_max = (SOC_DPP_DEFS_GET(unit, nof_isem_lines) / (BCM_VLAN_MAX + 1));

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("\nLIF: exhaustive configuration.\n");

        /* Create a vsi to map the ingress ports to all the vlan */
        rv = bcm_vswitch_create_with_id(unit, vsi);
        if (rv) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "bcm_vswitch_create_with_id() failed\n"))); 
            goto done;
        }
        
        /* Since we don't have enough ports configured, create trunks to use instead of ports... */
        for (i = 0 ; i < port_ndx_max - 11 ; i++) {
            rv = bcm_trunk_create(unit, 0, &port_list[11 + i]);
            if (rv != BCM_E_NONE) {
              cli_out("bcm_trunk_create failed: %d \n", rv);
              return rv;
            }

            rv = bcm_trunk_psc_set(unit, port_list[11 + i], BCM_TRUNK_PSC_ROUND_ROBIN);
            if (rv != BCM_E_NONE) {
              cli_out("Error, in bcm_trunk_psc_set\n");
              return rv;
            }
       }

        /* INGRESS */

        /* Fill ISEM-A: PORT-VLAN*/
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.vlan_port_id = 0;
        vlan_port.vsi = vsi;

        
        i = 0;
        for (port_ndx = 0 ; port_ndx < port_ndx_max ; port_ndx++) {
            for (outer_vid = 1 ; outer_vid < BCM_VLAN_MAX ; outer_vid++) {
                vlan_port.match_vlan = outer_vid;
                vlan_port.match_inner_vlan = inner_vid;
                vlan_port.port = port_list[0];


                rv = silent_vlan_port_create(unit, &vlan_port);

                if (rv != BCM_E_NONE) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,
                             (BSL_META_U(unit,
                                         "bcm_vlan_port_create() Failed i:%d outer_vid:%d port_ndx:%d port:%d:\n"), 
                                            i, outer_vid, port_ndx, port_list[port_ndx])); 
                   goto done;
                }

                vlan_port.vlan_port_id = 0;
                if (++i > SOC_DPP_DEFS_GET(unit, nof_isem_lines)){
                    outer_vid = BCM_VLAN_MAX;
                    port_ndx = port_ndx_max;
                }
            }
        }


        /* Fill ISEM-B: PORT-VLAN-VLAN */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.port = 13;
        vlan_port.vlan_port_id = 0;
        vlan_port.vsi = vsi;

        
        i = 0;
        for (outer_vid = 1 ; outer_vid < BCM_VLAN_MAX ; outer_vid++) {
            for (inner_vid = 1 ; inner_vid < BCM_VLAN_MAX ; inner_vid++) {
                vlan_port.match_vlan = outer_vid;
                vlan_port.match_inner_vlan = inner_vid;

                vlan_port.vlan_port_id = 0;

                rv = silent_vlan_port_create(unit, &vlan_port);

                if (rv != BCM_E_NONE) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,
                             (BSL_META_U(unit,
                                         "bcm_vlan_port_create() Failed i:%d outer_vid:%d inner_vid:%d:\n"), i, outer_vid, inner_vid)); 
                   goto done;
                }
                if (++i >= SOC_DPP_DEFS_GET(unit, nof_isem_lines) - 1000 /*ISEM B has some entries configured by deafult */){
                    inner_vid = BCM_VLAN_MAX;
                    outer_vid = BCM_VLAN_MAX;
                }
            }
        }

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_FULL) {
           LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "Lif resources are not exhausted.\n")));
           rv = BCM_E_NONE;
        }

        cli_out("Done exhausting inlif. Allocated %d in lifs.\n", i);

        /* EGRESS */
        cli_out("EEDB: exhaustive configuration.\n");
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        i = 0;

        cli_out(" lifs than the GLEM can hold.\n");

        for (outer_vid = 1 ; outer_vid < BCM_VLAN_MAX ; outer_vid++) {
            for (inner_vid = 1 ; inner_vid < BCM_VLAN_MAX ; inner_vid++) {
                vlan_port.egress_vlan = outer_vid;
                vlan_port.egress_inner_vlan = inner_vid;


                rv = silent_vlan_port_create(unit, &vlan_port);

                vlan_port.vlan_port_id = 0;

                if (rv != BCM_E_NONE) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,
                             (BSL_META_U(unit,
                                         "bcm_vlan_port_create() Failed i:%d egress_vid:%d egress_inner_vid:%d:\n"), i, outer_vid, inner_vid)); 
                   goto done;
                }
                if (++i > SOC_DPP_DEFS_GET(unit, nof_glem_lines) - 17000 /* Upper EEDB banks access is different, and not yet implemented. */){
                    inner_vid = BCM_VLAN_MAX;
                    outer_vid = BCM_VLAN_MAX;
                }
            }
        }

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_FULL) {
          LOG_WARN(BSL_LS_APPL_SHELL,
             (BSL_META_U(unit,
                         "EEDB resources are not exhausted.\n")));
           rv = BCM_E_NONE;
        }

        cli_out("Done exhausting EEDB. Allocated %d out lifs.\n", i);

        cli_out("LIF: exhaustive configuration done.\n");
    }

    /* Metering */
    /*----------*/
    if (init_param->Meter) {
        int i;
        int j;
        bcm_policer_config_t pol_cfg;
        int pol_id;

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("\nMetering: adding 65535 meters on 2 cores\n");

        for (j=0; j<2; j++) {
            for (i=0; i<65535; i++) {
                pol_id = i;
                bcm_policer_config_t_init(&pol_cfg);
                pol_cfg.flags |= BCM_POLICER_WITH_ID; 
                pol_cfg.mode = bcmPolicerModeTrTcmDs;
                pol_cfg.ckbits_sec = 3000000; 
                pol_cfg.ckbits_burst = 10000;
                pol_cfg.pkbits_burst = 10000;
                pol_cfg.pkbits_sec = 2000000; 
                pol_cfg.max_pkbits_sec = 2000000;
                if (j==1) {
                    pol_cfg.core_id=1;
                }
                rv = bcm_policer_create(0, &pol_cfg, &pol_id);
                if (BCM_FAILURE(rv)) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,
                             (BSL_META_U(unit,
                                         "bcm_policer_create() Failed:\n"))); 
                   goto done;
                }
                
            }
        }
        cli_out("Metering: added\n");
    }

    /* MPLS */
    /*------*/
    if (init_param->Mpls) {
#if defined(INCLUDE_L3) && defined(DPP)

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        bcm_mpls_egress_label_t eg_label;
        bcm_mpls_tunnel_switch_t entry;
        bcm_mpls_egress_label_t_init(&eg_label);
        eg_label.label=1;
        bcm_mpls_tunnel_switch_t_init(&entry);
        entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
        entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT|BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
        entry.label = 100;    
        entry.egress_label = eg_label;

        cli_out("\nMPLS: start adding mpls tunnels (may take few minutes)\n");

        while (rv == BCM_E_NONE) {
            rv = bcm_mpls_tunnel_switch_create(0,&entry);
            entry.label++;
            entry.egress_label.label++;
        }

        cli_out("MPLS: added %d mpls tunnels until create returned rv=%d\n", entry.egress_label.label, rv);
#else
        cli_out("MPLS: not supported with these compilation flags\n");
#endif
        rv = BCM_E_NONE;
    }


    /* LEM + ILM*/
    /*----------*/
    if (init_param->Lem) {
        uint8 mac[6] = {0, 0, 0, 0, 0, 1};
        uint32 counter = 0;
        bcm_l2_addr_t l2_addr;
        l2_addr.port = 1;
        l2_addr.vid = 1;
        l2_addr.flags = BCM_L2_STATIC;
        sal_memcpy(l2_addr.mac, mac, sizeof(bcm_mac_t));

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("LEM: start adding addresses\n");

        while (rv == BCM_E_NONE) {
           rv = bcm_l2_addr_add(0, &l2_addr);
           if (BCM_FAILURE(rv) && rv!= BCM_E_RESOURCE) {
               LOG_ERROR(BSL_LS_APPL_SHELL,
                         (BSL_META_U(unit,
                                     "bcm_l2_addr_add() Failed:\n"))); 
               break;
           }
           counter++;
           if (l2_addr.mac[1] == 255) {
               l2_addr.mac[1] = 0;
               l2_addr.mac[2]++;
           }
           if (l2_addr.mac[0] == 255) {
               l2_addr.mac[0] = 0;
               l2_addr.mac[1]++;
           }
           l2_addr.mac[0]++;
        }

        cli_out("LEM: added %d addresses until rv was (%d)\n", counter, rv);
        rv = BCM_E_NONE;

    }

    /*  ACL  */
    /*-------*/
    if (init_param->Acl) {

        DISPLAY_MEM ;
        DISPLAY_SW_STATE_MEM ;

        cli_out("\n ACL Exhaustive test.\n");
        rv = exaustive_test_field_group_tcam(unit, 10 /*priority*/);
		if (BCM_E_NONE != rv) {
			cli_out("Error in generic_field_group_tcam\n");
/*			rv = BCM_E_NONE;	*//*TBD: FIX this. in case of error - return error*/
			return rv;
	}

    }

    cli_out("Exhaustive Configurations done.\n\n");

    goto done;

done:

    if (rv < 0) {
        test_error(unit, "Exhaustive test Failed!!!\n");
        return rv;
    }
    else {
        sh_process_command(unit, "warmboot memoryuse");
    }

    return rv;
}


/*
 * Function:     exhaustive_usage_parse
 * Purpose:      exhaustive_usage_parse command
 * Parameters:   u - unit number to operate on
 *               a - args (none expected)
 *               init_param - return parsed parameter struct 
 * Returns:      CMD_OK/CMD_FAIL.
 */
cmd_result_t
exhaustive_usage_parse(int unit, args_t *a, exhaustive_init_param_t *init_param)
{
    int rv = CMD_OK;
    parse_table_t pt;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Kbp", PQ_INT, (void *) 0,
       &(init_param->Kbp), NULL);
    parse_table_add(&pt, "Kaps", PQ_INT, (void *) 0,
      &(init_param->Kaps), NULL);
    parse_table_add(&pt, "Oam", PQ_INT, (void *) 0,
      &(init_param->Oam), NULL);
    parse_table_add(&pt, "Lem", PQ_INT, (void *) 0,
      &(init_param->Lem), NULL);
    parse_table_add(&pt, "Lif", PQ_INT, (void *) 0,
      &(init_param->Lif), NULL);
    parse_table_add(&pt, "Eedb", PQ_INT, (void *) 0,
      &(init_param->Eedb), NULL);
    parse_table_add(&pt, "Fec", PQ_INT, (void *) 0,
      &(init_param->Fec), NULL);
    parse_table_add(&pt, "Ecmp", PQ_INT, (void *) 0,
      &(init_param->Ecmp), NULL);
    parse_table_add(&pt, "Meter", PQ_INT, (void *) 0,
      &(init_param->Meter), NULL);
    parse_table_add(&pt, "Counter", PQ_INT, (void *) 0,
      &(init_param->Counter), NULL);
    parse_table_add(&pt, "Ser", PQ_INT, (void *) 0,
      &(init_param->Ser), NULL);
    parse_table_add(&pt, "ExactMatchShadow", PQ_INT, (void *) 0,
      &(init_param->ExactMatchShadow), NULL);
    parse_table_add(&pt, "Acl", PQ_INT, (void *) 0,
      &(init_param->Acl), NULL);
    parse_table_add(&pt, "Mpls", PQ_INT, (void *) 0,
      &(init_param->Mpls), NULL);
    
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",exhaustive_usage);
        rv = CMD_FAIL;
    } else if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",exhaustive_usage);
        rv = CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    return rv;
}

int
exhaustive_test_init(int unit, args_t *a, void **p)
{
    exhaustive_init_param_t *init_param;
    init_param = &exhaustive[unit];
    sal_memset(init_param, 0x0, sizeof(exhaustive_init_param_t));

    *p = init_param;
    return exhaustive_usage_parse(unit, a, init_param);
}

int
exhaustive_test_done(int unit, void *p)
{
    cli_out("Exhaustive Test Done\n");
    return 0;
}

#endif /* BCM_PETRA_SUPPORT */

