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
 *      Flags - See appl/diag/test.h for explanation
 *              TS88xxx- Test is supported on the specified chip.
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

test_t sand_test_list[] = { /*

Test #      "Test Name"                 Flags/Chips             Init Func               Cleanup Func            Test Func          Loop Cnt  Default Args
----------------------------------------------------------------------------------------------------------------------------------------------------------*/
#if defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
TEST(1,   "Register reset defaults",  TS_DPP | TS_DFE | TS_DNXF | TS_DNX,        0,                       0,              rval_test,       1,    0)
TEST(2,   "PCI Compliance",           TSEL | TS_DPP | TS_DFE | TS_DNXF | TS_DNX, pci_test_init,           pci_test_done,  pci_test,        100,  0)
#endif
#if defined (BCM_SAND_SUPPORT)
TEST(3,   "Register read/write",      TS_SAND, disable_intr_and_count_reg_test,             0,                       reg_test,        1,    0)
#endif
#if defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(4,   "PCI S-Channel Buf",        TSEL | TS_DPP | TS_DFE,                    0,                       0,              pci_sch_test,     100, 0)
#endif
#if defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
TEST(5,   "BIST",                     TSEL | TS_DPP | TS_DFE | TS_DNXF | TS_DNX, bist_test_init,          bist_test_done, bist_test,        1,   "ALL")
TEST(6,   "Memories Test",            TS_DPP | TS_DFE | TS_DNXF | TS_DNX,        0,                       0,              memories_rw_test, 1,   0)
#endif
#if (defined (BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT))
TEST(7,   "Memories WR First Last Test", TS_SAND,             0,                       0,   memories_rw_first_last_test, 1,    0)
#endif
#if (defined (BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT))
TEST(8,   "Memories Flip/Flop Test",  TS_DPP | TS_DNXF | TS_DNX,                 0,                       0,             mem_flipflop_test, 1,    0)
#endif
#if defined(BCM_SAND_SUPPORT)
TEST(9,   "Broadcast Blocks",         TS_SAND,                 0,                       0,                 brdc_blk_test, 1,    0)
#endif
#if defined(BCM_DNX_SUPPORT)
TEST(10,   "Memory clear on init test", TS_DNX,                 0,                       0,          memories_clear_test, 1,    0)
#endif
#if defined (BCM_PETRA_SUPPORT)
TEST(17,  "CPU Loopback",             TRC | TSEL | TS_DPP,    lb_dma_init,             lb_dma_done,         lb_dma_test, 1,    0)
TEST(18,  "MAC Loopback",             TRC | TSEL | TS_DPP,    lb_mac_init,             lb_mac_done,         lb_mac_test, 1,    0)
TEST(19,  "PHY Loopback",             TRC | TSEL | TS_DPP,    lb_mii_init,             lb_mii_done,         lb_mii_test, 1,    0)
TEST(20,  "EXT Loopback",             TRC | TSEL | TS_DPP,    lb_ext_init,             lb_ext_done,         lb_ext_test, 1,    0)
#endif
TEST(21,  "CPU Benchmarks",           TS_DPP | TS_DNX,        benchmark_init,          benchmark_done,   benchmark_test, 1,    0)
#if defined (BCM_PETRA_SUPPORT)
TEST(22,  "CPU S/G, Reload",          TRC | TSEL | TS_DPP,    lb_reload_init,          lb_reload_done,   lb_reload_test, 1,    0)
TEST(23,  "CPU S/G, Simple",          TRC | TSEL | TS_DPP,    lb_sg_dma_init,          lb_sg_dma_done,   lb_sg_dma_test, 1,    0)
TEST(24,  "CPU S/G, Random",          TRC | TSEL | TS_DPP,    lb_random_init,          lb_random_done,   lb_random_test, 1,    0)
#endif
TEST(30,  "Counter widths",           TRC | TSEL | TS_DPP | TS_DNX,    0,                       0,                 ctr_test_width,         1,      0)
TEST(31,  "Counter read/write",       TRC | TSEL | TS_DPP | TS_DNX,    0,                       0,                      ctr_test_rw,            1,      0)
TEST(39,  "New Snake Test",           TRC | TS_DPP,           lb2_snake_init,          lb2_done,               lb2_snake_test,         1,      0)
TEST(40,  "BCM Packet Send",          TS_DPP | TS_DNX,        tpacket_tx_init,         tpacket_tx_done,        tpacket_tx_test,        1,      0)
#if defined(BCM_FIELD_SUPPORT) || defined (BCM_PETRA_SUPPORT)  || defined (BCM_DNX_SUPPORT)
TEST(41,  "BCM Packet Receive",       TRC | TS_DPP | TS_DNX,  rpacket_init,            rpacket_done,           rpacket_test,           1,      0)
#endif
#if defined (BCM_PETRA_SUPPORT)
TEST(48,  "MAC Loopback - Mark 2",    TRC | TS_DPP,           lb2_mac_init,            lb2_done,               lb2_port_test,          1,      0)
#endif
TEST(49,  "PHY Loopback - Mark 2",    TRC | TS_DPP,           lb2_phy_init,            lb2_done,               lb2_port_test,          1,      0)
#if defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
TEST(50,  "Memory Fill/Verify",       TS_DPP | TS_DFE | TS_DNXF | TS_DNX,        mem_test_init,           mem_test_done,          mem_test,               1,      0)
TEST(51,  "Memory Random Addr/Data",  TS_DPP | TS_DFE | TS_DNXF | TS_DNX,        mem_rand_init,           mem_rand_done,          mem_rand,               1,      0)
TEST(52,  "Rand Mem Addr, write all", TS_DPP | TS_DFE | TS_DNXF | TS_DNX,        addr_rand_init,          addr_rand_done,         addr_rand,              1,      0)
#endif
#if defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
TEST(60,  "Linkscan MDIO",            TRC | TSEL | TS_DPP | TS_DFE, ls_test_init,      ls_test_done,           ls_test,                1,      0)
#endif
#if defined(BCM_SAND_SUPPORT)
TEST(71,  "Table DMA",                TS_SAND,    0,            0,           dnx_table_dma_test,           1,      0)
#endif
TEST(72,  "Traffic Test",             TRC | TS_DPP,           traffic_test_init,       traffic_test_done,      traffic_test,           1,      0)
#if defined (BCM_PETRA_SUPPORT)
TEST(73,  "SNMP MIB Object Test",     TS_DPP,                 snmp_test_init,          snmp_test_done,         snmp_test_test,         1,      0)
TEST(90,  "TX Reload Test",           TRC | TS_DPP,           pktspeed_test_init,      pktspeed_test_done,        pktspeed_test_tx,        1,      0)
TEST(91,  "RX Reload Test",           TRC | TS_DPP,           pktspeed_test_init,      pktspeed_test_done,        pktspeed_test_rx,        1,      0)
#endif
#ifdef BCM_88750_A0
#endif
#ifdef BCM_DNXF_SUPPORT
TEST(131, "DNXF fabric snake test",   TS_DNXF,                test_dnxf_fabric_snake_test_init,   test_dnxf_fabric_snake_test_done,    test_dnxf_fabric_snake_test,      1, 0)
#endif
#if defined (BCM_DDR3_SUPPORT) || defined (BCM_PETRA_SUPPORT)
TEST(140, "DDR bist test",            TS_DPP,                 ddr_bist_test_init,      ddr_bist_test_done,              ddr_bist_test,           1, 0)
#endif
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
TEST(141, "DEINT INIT (SOC BCM)",     TS_DNX | TS_DNXF,       test_dnxc_init_test_init,   test_dnxc_init_test_done,    test_dnxc_init_test, 1, 0)
#endif
#if defined (SER_TR_TEST_SUPPORT)
TEST(144, "Software Error Recovery",  TS_DPP,                 ser_test_init,           ser_test_done, ser_test, 1, 0)
#endif
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
TEST(146, "ARM core",                 TS_DPP,                 arm_core_test_init,      arm_core_test_done, arm_core_test, 1, 0)
#endif
#if defined (BCM_SAND_SUPPORT)
TEST(153,     "Memory Ser Test",   DPP_DFE | TS_DNX | TS_DNXF,       memory_ser_test_init,          memory_ser_test_done,      memory_ser_test_run,   1, 0)
#endif
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
TEST(152,     "Cache Memory Test",  DPP_ALL | TS_DNXF | TS_DNX,    do_cache_mem_test_init,        do_cache_mem_test_done,   do_cache_mem_test,      1, 0)
#endif
#if (defined(BCM_88675_A0) || defined(BCM_DNX_SUPPORT)) && defined(INCLUDE_KBP) && !defined(BCM_88030)
TEST(158,     "KBP VER",          TS_DNX,       0,          0,      kbp_sdk_ver_test,   1, 0)
#endif
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
TEST(200, "DNX DATA",                 TS_DNX | TS_DNXF,     0,                        0,                          test_dnxc_data_test, 1, 0)
#endif
#if defined(BCM_DNX_SUPPORT)
#if defined(INCLUDE_CTEST)
#endif /* INCLUDE_CTEST */
#endif
};

int sand_test_cnt = COUNTOF(sand_test_list);      /* # entries. */
