/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        testlist.c
 * Purpose:     Defines list of tests and their associated
 *              functions.
 *
 * Notes:       Fields defined as follows:
 *
 *      Test # - Indicates the test number, all commands hat operate on tests
 *              can use either the test name or #. The # is intended to be the
 *              DV Test number, but is arbitrary. If the test number is
 *              negative, then the test is not selected by default; otherwise
 *              it is.
 *      Flags - TSEL - Test is selected by default on supported chips.
 *              T56xx- Test is supported on the specified chip.
 *              TALL - Simple form to include all chips.
 *              TRC - rc.soc is to be loaded before running test.
 *      Test Name - The name of the test.
 *      Init Func - A routine called before the test is run to perform any
 *              setup required, may be NULL if no initialization required.
 *      Cleanup Func - Function called after test has run to reset any state
 *              etc, may be NULL if not required.
 *      Test Function - actual test function, called "Loop Count" times.
 *      Loop Count - Number of times to call "Test Function".
 *      Default Args - Default argument string passed into test.
 *
 * Order of a normal test execution is:
 *
 *      [1] Load TEST_RC file (if required)
 *      [2] Call "Init Func"
 *      [3] Call "Test Func" Loop times
 *      [4] Call "Cleanup Func"
 *
 * All external declarations for functions are found in testlist.h.
 */

#include <appl/diag/system.h>
#include <appl/diag/test.h>
#include <shared/bsl.h>
#include "testlist.h"

test_t common_test_list[] = { /*

Test #      "Test Name"                 Flags/Chips             Init Func               Cleanup Func            Test Func          Loop Cnt  Default Args
----------------------------------------------------------------------------------------------------------------------------------------------------------*/
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(1,     "Register reset defaults",  (TALL)|DPP_DFE,            0,                      0,                      rval_test,              1,      0)
#endif /* BCM_ESW_SUPPORT || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) */
#if defined(BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)
TEST(2,     "PCI Compliance",           (TSEL+TALL)|DPP_DFE,              pci_test_init,          pci_test_done,          pci_test,               100,    0)
#endif /* BCM_ESW_SUPPORT || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) */
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(3,     "Register read/write",      (TALL)|DPP_DFE,            0,                      0,                      reg_test,               1,      0)
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(4,     "PCI S-Channel Buf",        (TSEL+TALL)|DPP_DFE,       0,                      0,                      pci_sch_test,           100,    0)
#endif /* BCM_ESW_SUPPORT || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)*/
#if defined (BCM_ESW_SUPPORT) ||  defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)
TEST(5,     "BIST",                     (TSEL)|DPP_DFE,             bist_test_init,         bist_test_done,         bist_test,              1,      "ALL")
#if (defined (BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT))
TEST(6,     "Memories Test",                         DPP_DFE,             0,                                    0,  memories_rw_test,             1,      0)
TEST(7,     "Memories WR First Last Test",           DPP_DFE,             0,                                    0,  memories_rw_first_last_test,             1,      0)
#ifdef BCM_PETRA_SUPPORT
TEST(8,     "Memories Flip/Flop Test",               DPP_ALL,             0,                                    0,  mem_flipflop_test,             1,      0)
#endif
#endif
#endif
#if defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)
TEST(9,     "Brodcast Blocks",             DPP_ALL | DFE_ALL,             0,                                    0,  brdc_blk_test,             1,      0)
#endif /*defined(BCM_DFE_SUPPORT)*/
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT) ||  defined (BCM_PETRA_SUPPORT)
TEST(17,    "CPU Loopback",             (TRC+TSEL+TXGSSW-T56700-DFE_ALL)/*|DPP_DFE*/, lb_dma_init,            lb_dma_done,            lb_dma_test,            1,      0)
TEST(18,    "MAC Loopback",             (TRC+TALL+TSEL-DFE_ALL)/*|DPP_DFE*/,               lb_mac_init,            lb_mac_done,            lb_mac_test,            1,      0)
TEST(19,    "PHY Loopback",             (TRC+TALL+TSEL-DFE_ALL)/*|DPP_DFE*/,               lb_mii_init,            lb_mii_done,            lb_mii_test,            1,      0)
TEST(20,    "EXT Loopback",             (TRC+TALL+TSEL-DFE_ALL)/*|DPP_DFE*/,               lb_ext_init,            lb_ext_done,            lb_ext_test,            1,      0)
#endif /* BCM_ESW_SUPPORT || defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT) */
TEST(21,    "CPU Benchmarks",           (TALL),                   benchmark_init,         benchmark_done,         benchmark_test,         1,      0)
#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(22,    "CPU S/G, Reload",          (TRC+TSEL+TALL-DFE_ALL), lb_reload_init,   lb_reload_done,         lb_reload_test,         1,      0)
TEST(23,    "CPU S/G, Simple",          (TRC+TSEL+TALL-DFE_ALL),  lb_sg_dma_init,  lb_sg_dma_done,         lb_sg_dma_test,         1,      0)
TEST(24,    "CPU S/G, Random",          (TRC+TSEL+TALL-DFE_ALL),  lb_random_init,  lb_random_done,         lb_random_test,         1,      0)
#endif /* BCM_ESW_SUPPORT */
TEST(30,    "Counter widths",           (TRC+TSEL+TALL-DFE_ALL)| DPP_ALL,   0,                      0,                      ctr_test_width,         1,      0)
TEST(31,    "Counter read/write",       (TRC+TSEL+TALL-DFE_ALL)| DPP_ALL,   0,                      0,                      ctr_test_rw,            1,      0)
#ifdef BCM_ESW_SUPPORT
#ifdef BCM_XGS_SWITCH_SUPPORT
TEST(32,    "XGS L2 Ins/Lookup/Del",    TSEL+TXGSSW-T56640-T56850,     draco_l2_lu_test_init,  draco_l2_test_done,     draco_l2_test_lu,       1,      0)
TEST(33,    "XGS L2 Overflow Ins",      TSEL+TXGSSW-T56640-T56850,     draco_l2_ov_test_init,  draco_l2_test_done,     draco_l2_test_ov,       1,      0)
TEST(34,    "XGS L2 Hashing",           TSEL+TXGSSW-T56640-T56850,     draco_l2_hash_test_init,draco_l2_test_done,     draco_l2_test_hash,     1,      0)
TEST(35,    "XGS L2 Delete by Port",    (TSEL+TXGSSW-T56640)|T56850,     draco_l2_dp_test_init,  draco_l2_test_done,     draco_l2_test_dp,       1,      0)
TEST(36,    "XGS L2 Delete by VLAN",    (TSEL+TXGSSW-T56640)|T56850,     draco_l2_dv_test_init,  draco_l2_test_done,     draco_l2_test_dv,       1,      0)
#endif /* BCM_XGS_SWITCH_SUPPORT */
#endif /* BCM_ESW_SUPPORT */
TEST(39,    "New Snake Test",   (TRC+TALL)/* | DPP_DFE*/,                lb2_snake_init,         lb2_done,               lb2_snake_test,         1,      0)
TEST(40,    "BCM Packet Send",          (TALL) | DPP_ALL,                    tpacket_tx_init,        tpacket_tx_done,        tpacket_tx_test,        1,      0)
#if defined(BCM_FIELD_SUPPORT) || defined(BCM_FE2000_SUPPORT) ||  defined (BCM_PETRA_SUPPORT)
TEST(41,    "BCM Packet Receive",       (TRC+TXGSSW), rpacket_init,  rpacket_done,           rpacket_test,           1,      0)
#endif /* BCM_FIELD_SUPPORT || BCM_FE2000_SUPPORT || defined (BCM_PETRA_SUPPORT)*/
#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(48,    "MAC Loopback - Mark 2",    (TRC+TALL),               lb2_mac_init,           lb2_done,               lb2_port_test,          1,      0)
#endif /* BCM_ESW_SUPPORT || defined (BCM_PETRA_SUPPORT) */
TEST(49,    "PHY Loopback - Mark 2",    (TRC+TALL),              lb2_phy_init,           lb2_done,               lb2_port_test,          1,      0)
#if defined BCM_ESW_SUPPORT || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)
TEST(50,    "Memory Fill/Verify",       (TALL)|DPP_DFE,            mem_test_init,          mem_test_done,          mem_test,               1,      0)
TEST(51,    "Memory Random Addr/Data",  (TALL)|DPP_DFE,            mem_rand_init,          mem_rand_done,          mem_rand,               1,      0)
TEST(52,    "Rand Mem Addr, write all", (TALL)|DPP_DFE,            addr_rand_init,         addr_rand_done,         addr_rand,              1,      0)
#if defined BCM_TRIDENT3_SUPPORT || defined BCM_MAVERICK2_SUPPORT
TEST(54,    "TD3/MV2 Special Mem Test",       (T56870 | T56770),        special_mem_test_init,        special_mem_test_done,          special_mem_test,               1,      0)
#endif
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
#if defined (BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
TEST(60,    "Linkscan MDIO",            (TRC+TSEL+TXGSSW)|DPP_DFE,     ls_test_init,           ls_test_done,           ls_test,                1,      0)
#endif
#if defined (BCM_DDR3_SUPPORT) && !defined(BCM_PETRA_SUPPORT) && !defined(BCM_DFE_SUPPORT)
TEST(53,    "DDR Memory Fill/Verify",   TRC+(TALL & ~T56450)-DPP_DFE,               ddr_test_init,          ddr_test_done,          ddr_test,               1,      0)
#endif
#ifdef BCM_XGS_SWITCH_SUPPORT
#ifdef BCM_ISM_SUPPORT
TEST(55,    "Memory hashing",           TSEL+T56640+T56850+T56960,     test_generic_hash_init, test_generic_hash_done, test_generic_hash,      1,      0)
TEST(56,    "Memory hash overflow Ins", TSEL+T56640+T56850+T56960,     test_generic_hash_ov_init, test_generic_hash_done, test_generic_hash_ov,
                                                                                                                                        1,      0)
#endif /* BCM_ISM_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
#ifdef BCM_ESW_SUPPORT
TEST(60,    "Linkscan MDIO",            TRC+TSEL+TXGSSW,        ls_test_init,           ls_test_done,           ls_test,                1,      0)
#endif /* BCM_ESW_SUPPORT */
#if defined (BCM_XGS_SWITCH_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(71,    "Table DMA",                (TSEL+TXGSSW)|DPP_DFE,            td_test_init,           td_test_done,           td_test_test,           1,      0)
#endif /* BCM_XGS_SWITCH_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
TEST(72,    "Traffic Test",             (TRC+TALL),                traffic_test_init,      traffic_test_done,      traffic_test,           1,      0)

#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(73,    "SNMP MIB Object Test",     TALL,                   snmp_test_init,         snmp_test_done,         snmp_test_test,         1,      0)
#if defined(BCM_FIREBOLT_SUPPORT) && defined(INCLUDE_L3)
TEST(85,    "L3 IPV6 Overflow Ins",     TSEL+TXGS3SW-T56218-T56640-T56850, fb_l3ip6_ov_test_init, fb_l3_test_done,     fb_l3_test_ov,          1,      0)
TEST(86,    "L3 IPV6 Hashing",          TSEL+TXGS3SW-T56218-T56640-T56850, fb_l3ip6_hash_test_init, fb_l3_test_done,   fb_l3_test_hash,        1,      0)
TEST(87,    "L3 IPV4 Overflow Ins",     TSEL+TXGS3SW-T56218-T56640-T56850, fb_l3_ov_test_init, fb_l3_test_done,        fb_l3_test_ov,          1,      0)
TEST(88,    "L3 IPV4 Hashing",          TSEL+TXGS3SW-T56218-T56640-T56850, fb_l3_hash_test_init, fb_l3_test_done,      fb_l3_test_hash,        1,      0)
#endif
TEST(90,    "TX Reload Test",        (TRC+TALL)|DPP_ALL,                   pktspeed_test_init,   pktspeed_test_done,        pktspeed_test_tx,        1,      0)
TEST(91,    "RX Reload Test",        (TRC+TALL)|DPP_ALL,                   pktspeed_test_init,   pktspeed_test_done,        pktspeed_test_rx,        1,      0)

#if defined(BCM_TRX_SUPPORT) /* || defined (BCM_PETRA_SUPPORT)  */
TEST(100,   "VLAN xlate Overflow Ins",  (T56624|T56820|T56450|T56560|T56960|T56970)&(~T56640), tr_vlan_xlate_ov_test_init, tr_vlan_xlate_test_done, tr_vlan_xlate_test_ov,
                                                                                                                                        1,      0)
TEST(101,   "VLAN xlate Hashing",       (T56624|T56820|T56450|T56850|T56960|T56970)&(~T56640), tr_vlan_xlate_hash_test_init, tr_vlan_xlate_test_done, tr_vlan_xlate_test_hash,
                                                                                                                                        1,      0)
TEST(102,   "Egr VLAN xlate Ovrflw Ins", (T56624|T56820|T56450|T56560|T56960|T56970)&(~T56640), tr_egr_vlan_xlate_ov_test_init, tr_egr_vlan_xlate_test_done, tr_egr_vlan_xlate_test_ov,
                                                                                                                                        1,      0)
TEST(103,   "Egr VLAN xlate Hashing",   (T56624|T56820|T56450|T56850|T56960|T56970)&(~T56640), tr_egr_vlan_xlate_hash_test_init, tr_egr_vlan_xlate_test_done, tr_egr_vlan_xlate_test_hash,
                                                                                                                                        1,      0)
#ifdef BCM_TRIUMPH_SUPPORT
TEST(104,   "MPLS Overflow Ins",        (T56624|T56450|T56560|T56960|T56970)&(~T56640), tr_mpls_ov_test_init, tr_mpls_test_done,     tr_mpls_test_ov,        1,      0)
TEST(105,   "MPLS Hashing",             (T56624|T56450|T56850|T56960|T56970)&(~T56640), tr_mpls_hash_test_init, tr_mpls_test_done,   tr_mpls_test_hash,      1,      0)
#endif /* BCM_TRIUMPH_SUPPORT */
#endif /* BCM_TRX_SUPPORT */
#endif  /* BCM_ESW_SUPPORT */
#ifdef BCM_88750_A0
#endif /* (BCM_88750_A0)*/

#ifdef BCM_DFE_SUPPORT
TEST(131,    "DFE fabric snake test",    DFE_ALL,            fabric_snake_test_init,       fabric_snake_test_done,          fabric_snake_test,      1, 0)
#endif

#if defined (BCM_DDR3_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(140,    "DDR bist test",            T56450+DPP_ALL,      ddr_bist_test_init,          ddr_bist_test_done,              ddr_bist_test,           1, 0)
#endif

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
TEST(141,    "DEINT INIT (SOC BCM)", DPP_DFE,      init_deinit_test_init,   init_deinit_test_done,       init_deinit_test, 1, 0)
#endif

#ifdef BCM_PETRA_SUPPORT
TEST(142,    "ARAD field tests",          DPP_ALL,      arad_field_test_init,         arad_field_test_done,             arad_field_test,       1, 0)
#endif

#if defined (SER_TR_TEST_SUPPORT)
TEST(143, "Software BUS BUFFER Error Recovery", T56860 | T56960 | T56870 | T56980, ser_bus_buffer_test_init, ser_bus_buffer_test_done, ser_bus_buffer_test, 1, 0)
TEST(144, "Software Error Recovery", T56634 | T56850 | T56450 | T56960 | T56640 | T56870, ser_test_init, ser_test_done, ser_test, 1, 0)
#endif
#ifdef BCM_PETRA_SUPPORT
TEST(145, "Diag PP test", DPP_ALL , diag_pp_test_init, diag_pp_test_done, diag_pp_test, 1, 0)
#endif
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
TEST(146, "ARM core", TALL, arm_core_test_init, arm_core_test_done, arm_core_test, 1, 0)
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
TEST(147, "Memory/Logic BIST", T56850|T56960, memory_logic_bist_test_init, memory_logic_bist_test_done, memory_logic_bist_test, 1, 0)
#endif
#if defined(BCM_HELIX5_SUPPORT)
TEST(149, "Simple A72 access test", (T56370), a72_access_test_init, a72_access_test_done, a72_access_test, 1, 0)
#endif
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
TEST(152,     "Cache Memory Test",        DPP_ALL,       do_cache_mem_test_init,        do_cache_mem_test_done,   do_cache_mem_test,      1, 0)
#endif
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
TEST(153,     "Memory Ser Test",          DPP_DFE,       memory_ser_test_init,          memory_ser_test_done,      memory_ser_test_run,   1, 0)
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
TEST(154,     "KAPS TCAM BIST",          DPP_ALL,       0,          0,      kaps_bist_test,   1, 0)
#endif
#ifdef BCM_PETRA_SUPPORT
TEST(155,     "AC-MP and IP tunnel inlif learn info encoding", DPP_ALL, 0 ,0, lif_learn_info_bist_test, 1, 0)
#endif
#if defined(BCM_PETRA_SUPPORT)
TEST(157,     "Exhaustive Configuration", DPP_ALL, exhaustive_test_init,   exhaustive_test_done,       exhaustive_test, 1, 0)
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
TEST(158,     "KBP VER",          DPP_ALL,       0,          0,      kbp_sdk_ver_test,   1, 0)
#endif
/* simple CMICx sim test */
#if defined(PLISIM) && defined(BCM_CMICX_SUPPORT)
TEST(201,     "Simple CMICx Sim Test",          DPP_ALL,      cmicx_test_init,  cmicx_test_done,  cmicx_test_run,   1, 0)
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
TEST(159,     "KAPS SEARCH",          DPP_ALL,       0,          0,      kaps_search_test,   1, 0)
#endif
#if defined (BCM_PETRA_SUPPORT) && !defined (__KERNEL__) && defined (_SHR_SW_STATE_EXM)
TEST(160,     "SW_STATE",          DPP_ALL,       sw_state_test_init,       sw_state_test_done,        sw_state_test,      1, 0)
#endif
#if defined (BCM_PETRA_SUPPORT)
TEST(161,     "OAMP",          DPP_ALL,       oamp_test_init,       oamp_test_done,        oamp_test,      1, 0)
#endif
#if defined (BCM_PETRA_SUPPORT) && !defined (__KERNEL__)
TEST(162,     "SW_STATE_ISSU",     DPP_ALL,       sw_state_issu_test_init,  sw_state_issu_test_done,   sw_state_issu_test, 1, 0)
#endif
#if defined (BCM_PETRA_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT)
TEST(163,     "CR_DMA_SUPPORT",    DPP_ALL,       cr_dma_support_test_init, cr_dma_support_test_done,  cr_dma_support_test, 1, 0)
#endif
#if defined(BCM_PETRA_SUPPORT)
TEST(165,     "TCAM BIST",          DPP_ALL,       0,          0,      tcam_bist_test,   1, 0)
#endif

#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
TEST(500,       "Packet DMA SOC test",  TRC+T56960+T56860,      pktdma_soc_test_init,   pktdma_soc_test_cleanup,        pktdma_soc_test,        1,      0)
TEST(501,       "Streaming test",       TRC+T56960+T56860,      streaming_test_init,    streaming_test_cleanup,         streaming_test,         1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT) && \
    defined(BCM_SBUSDMA_SUPPORT)
TEST(502,       "SBUS DMA",             T56960+T56860,          sb_test_init,           sb_test_done,            sb_test_main,           1,      0)
TEST(506,       "SBUS DMA stress test", T56960,          0,                      0,                              sb_stress_test,         1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
#if defined(BCM_CCMDMA_SUPPORT)
TEST(503,       "CCM DMA test",         T56960+T56860,          ccmdma_test_init,       ccmdma_test_cleanup,            ccmdma_test,            1,      0)
#endif
#endif
#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT)
TEST(504,       "FIFO DMA test",        T56960+T56860,          fifodma_test_init,      fifodma_test_cleanup,           fifodma_test,           1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
TEST(507,       "Latency test",            T56960+T56860,   latency_test_init,      latency_test_cleanup,           latency_test,           1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
TEST(510,       "L2UC test",            TRC+T56960+T56860,      l2uc_test_init,         l2uc_test_cleanup,              l2uc_test,              1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
TEST(511,       "L2MC test",            TRC+T56960+T56860,      l2mc_test_init,         l2mc_test_cleanup,              l2mc_test,              1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
TEST(512,       "L3UC test",            TRC+T56960+T56860,      l3uc_test_init,         l3uc_test_cleanup,              l3uc_test,              1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
TEST(513,       "IPMC test",            TRC+T56960+T56860,      ipmc_test_init,         ipmc_test_cleanup,              ipmc_test,              1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)) && !defined(NO_SAL_APPL) && (!defined(__KERNEL__))
TEST(514,       "Flexport test",     (TRC)+(T56960|T56860|T56870|T56980|T56770),    flex_test_init,   flex_test_cleanup,      flex_test,   1,      0)
#endif
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
TEST(515,       "MBIST",                T56860,                 mbist_cpu_test_init,    mbist_cpu_test_done,            mbist_cpu_test,         1,      0)
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
TEST(516,       "CAMBIST",              T56960,                 cambist_test_init,      cambist_test_done,              cambist_test,           1,      0)
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
TEST(901,    "SER In-House Test",       T56960+T56860,          ser_ih_test_init,        ser_ih_test_cleanup,           ser_ih_test,            1,      0)
TEST(903,    "ECC Parity Test",         T56960+T56860,          ecc_par_test_init,       ecc_par_test_cleanup,          ecc_par_test,           1,      0)
#endif
#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICX_SUPPORT)
TEST(505,       "SCHAN FIFO test",    T56870 | T56980,                   schanfifo_test_init,    schanfifo_test_cleanup,         schanfifo_test,        1,      0)
TEST(518,       "SCHANFIFO Bulk Mem Test",      T56870 | T56980,         mem_bulk_test_init,     mem_bulk_test_cleanup,          mem_bulk_test,         1,      0)
#endif
};

int common_test_cnt = COUNTOF(common_test_list);      /* # entries. */
