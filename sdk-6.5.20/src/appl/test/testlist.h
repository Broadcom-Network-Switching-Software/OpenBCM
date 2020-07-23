/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        testlist.h
 * Purpose:     External declarations for test functions.
 */


#if !defined(_TEST_TESTLIST_H)
#define _TEST_TESTLIST_H

#include <appl/diag/test.h>
#include "cache_mem_test.h"
#include <soc/defs.h>
/****************************************************************
 *                                                              *
 *                      Test Functions                          *
 *                                                              *
 * Each test function may have an optional corresponding        *
 * "init" function and "done" function.                         *
 */

/* arltest.c */

extern  int     arl_test_init(int, args_t *, void **);
extern  int     arl_test_bc(int, args_t *, void *);
extern  int     arl_test_wc(int, args_t *, void *);
extern  int     arl_test_hd(int, args_t *, void *);
extern  int     arl_test_dm(int, args_t *, void *);
extern  int     arl_test_ov(int, args_t *, void *);
extern  int     arl_test_lu(int, args_t *, void *);
extern  int     arl_test_done(int, void *);

/* armcore.c */

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
extern  int     arm_core_test_init(int, args_t *, void **);
extern  int     arm_core_test(int, args_t *, void *);
extern  int     arm_core_test_done(int, void *);
#endif

/* cnttest.c */

extern  int     ctr_test_width(int, args_t *, void *);
extern  int     ctr_test_rw(int, args_t *, void *);

/* linkscan.c */

extern  int     ls_test_init(int, args_t *, void **);
extern  int     ls_test(int, args_t *, void *);
extern  int     ls_test_done(int, void *);

/* loopback.c */

extern  int     lb_mac_init(int, args_t *, void **);
extern  int     lb_mac_test(int, args_t *, void *);
extern  int     lb_mac_done(int, void *);

extern  int     lb_mii_init(int, args_t *, void **);
extern  int     lb_mii_test(int, args_t *, void *);
extern  int     lb_mii_done(int, void *);

extern  int     lb_dma_init(int, args_t *, void **);
extern  int     lb_dma_test(int, args_t *, void *);
extern  int     lb_dma_done(int, void *);

extern  int     lb_ext_init(int, args_t *, void **);
extern  int     lb_ext_test(int, args_t *, void *);
extern  int     lb_ext_done(int, void *);

extern  int     lb_snake_init(int, args_t *, void **);
extern  int     lb_snake_test(int, args_t *, void *);
extern  int     lb_snake_done(int, void *);

extern  int     lb_sg_dma_test(int, args_t *, void *);
extern  int     lb_sg_dma_init(int, args_t *, void **);
extern  int     lb_sg_dma_done(int, void *);

extern  int     lb_reload_test(int, args_t *, void *);
extern  int     lb_reload_init(int, args_t *, void **);
extern  int     lb_reload_done(int, void *);

extern  int     lb_random_test(int, args_t *, void *);
extern  int     lb_random_init(int, args_t *, void **);
extern  int     lb_random_done(int, void *);

/* loopback2.c */

extern  int     lb2_mac_init(int, args_t *, void **);
extern  int     lb2_phy_init(int, args_t *, void **);
extern  int     lb2_port_test(int, args_t *, void *);
extern  int     lb2_done(int, void *);
extern  int     lb2_snake_init(int, args_t *, void **);
extern  int     lb2_snake_test(int, args_t *, void *);

/* notify.c */

extern  int     not_link_test(int, args_t *, void *);
extern  int     not_link_init(int, args_t *, void **);
extern  int     not_link_done(int, void *);

extern  int     cos_stat_test(int, args_t *, void *);
extern  int     hol_stat_test(int u, args_t *, void *);
extern  int     bkp_stat_test(int u, args_t *, void *);

/* regtest.c */
extern  int     reg_test(int u, args_t *, void *);
extern  int     rval_test(int u, args_t *, void *);
#ifdef BCM_SAND_SUPPORT
extern  int     disable_intr_and_count_reg_test(int u, args_t *, void **);
#endif
#if (defined (BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT))
extern  int     memories_rw_test(int u, args_t *, void *);
extern  int     memories_rw_first_last_test(int u, args_t *, void *);
extern  int     mem_flipflop_test(int u, args_t *, void *);
#endif
#ifdef BCM_DNX_SUPPORT
extern  int     memories_clear_test(int u, args_t *, void *);
#endif
#if (defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT))
typedef struct
{
   uint32    write_value;
   uint32    period;
   uint32    run_count;
   uint32    incr_run_count;
   uint32    total_count;
   uint32    error_count;
   uint32    unclear_skipped_count;
   int       fault_inject;
   int       skip_reset_and_write;
   uint32     include_port_macros;  /* when set 1: test include port macros(nbih,nbil) other skipping port macros*/
   uint32    start_from;  /*skip meme when mem-id<start_from*/
   uint32    count;  /*how many memory to test*/
   int       max_size;    /* skip mems when mem size in byte > max_size*/
} tr8_dbase_t;
#endif

extern  int     brdc_blk_test_dfe(int u, args_t *, void *);
extern  int     brdc_blk_test(int u, args_t *, void *);
/* pcitest.c */

extern  int     pci_test_init(int, args_t *, void **);
extern  int     pci_test(int, args_t *, void *);
extern  int     pci_test_done(int, void *);

extern  int     pci_sch_test(int, args_t *, void *);

/* bist.c */

extern  int     bist_test_init(int, args_t *, void **);
extern  int     bist_test(int, args_t *, void *);
extern  int     bist_test_done(int, void *);

/* memory.c */
extern  int    mem_test_init(int, args_t *, void **);
extern  int    special_mem_test_init(int, args_t *, void **);
extern  int    mem_test(int, args_t *, void *);
extern  int    special_mem_test(int, args_t *, void *);
extern  int    mem_test_done(int, void *);
extern  int    special_mem_test_done(int, void *);
extern soc_error_t enable_dynamic_memories_access(int unit);
#if defined (SER_TR_TEST_SUPPORT)
extern  int    ser_test_init(int unit, args_t *arg, void **p);
extern  int    ser_test_done(int unit, void *p);
extern  int    ser_test(int unit, args_t *arg, void *p);
extern  int    ser_bus_buffer_test_init(int unit, args_t *arg, void **p);
extern  int    ser_bus_buffer_test_done(int unit, void *p);
extern  int    ser_bus_buffer_test(int unit, args_t *arg, void *p);

#endif

/* memrand.c */

extern  int     mem_rand_init(int, args_t *, void **);
extern  int     mem_rand(int, args_t *, void *);
extern  int     mem_rand_done(int, void *);

/* ddrtest.c */
#ifdef BCM_DDR3_SUPPORT
extern int      ddr_test_init(int, args_t *, void **);
extern int      ddr_test(int, args_t *, void *);
extern int      ddr_test_done(int, void *);
#endif /* BCM_DDR3_SUPPORT */
#if defined (BCM_DDR3_SUPPORT) || defined (BCM_PETRA_SUPPORT)
extern int      ddr_bist_test_init(int, args_t *, void **);
extern int      ddr_bist_test(int, args_t *, void *);
extern int      ddr_bist_test_done(int, void *);
#endif  /* BCM_DDR3_SUPPORT || BCM_PETRA_SUPPORT */

/* randaddr.c */

extern  int     addr_rand_init(int, args_t *, void **);
extern  int     addr_rand(int, args_t *, void *);
extern  int     addr_rand_done(int, void *);

/* bench.c */

extern  int     benchmark_init(int, args_t *, void **);
extern  int     benchmark_test(int, args_t *, void *);
extern  int     benchmark_done(int, void *);

/* ffptest.c */

extern  int     ffp_test_init(int, args_t *, void **);
extern  int     ffp_test(int, args_t *, void *);
extern  int     ffp_test_done(int, void *);

/* tabledma.c */

#if defined (BCM_XGS_SWITCH_SUPPORT) || defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined (BCM_DNXF_SUPPORT)
extern  int     td_test_init(int, args_t *, void **);
extern  int     td_test_test(int, args_t *, void *);
extern  int     td_test_done(int, void *);
#endif
#ifdef BCM_SAND_SUPPORT
extern  int     dnx_table_dma_test(int, args_t *, void *);
#endif

/* draco_arltest.c */

#ifdef BCM_XGS_SWITCH_SUPPORT
extern  int     draco_l2_lu_test_init(int, args_t *, void **);
extern  int     draco_l2_ov_test_init(int, args_t *, void **);
extern  int     draco_l2_hash_test_init(int, args_t *, void **);
extern  int     draco_l2_dp_test_init(int, args_t *, void **);
extern  int     draco_l2_dv_test_init(int, args_t *, void **);
extern  int     draco_l2_test_lu(int, args_t *, void *);
extern  int     draco_l2_test_ov(int, args_t *, void *);
extern  int     draco_l2_test_hash(int, args_t *, void *);
extern  int     draco_l2_test_dp(int, args_t *, void *);
extern  int     draco_l2_test_dv(int, args_t *, void *);
extern  int     draco_l2_test_done(int, void *);

#ifdef INCLUDE_L3
extern  int     draco_l3_hash_test_init(int, args_t *, void **);
extern  int     draco_l3_ov_test_init(int, args_t *, void **);
extern  int     draco_l3_test_ov(int, args_t *, void *);
extern  int     draco_l3_test_hash(int, args_t *, void *);
extern  int     draco_l3_test_done(int, void *);
#ifdef BCM_FIREBOLT_SUPPORT
extern  int     fb_l3_hash_test_init(int, args_t *, void **);
extern  int     fb_l3_ov_test_init(int, args_t *, void **);
extern  int     fb_l3ip6_hash_test_init(int, args_t *, void **);
extern  int     fb_l3ip6_ov_test_init(int, args_t *, void **);
extern  int     fb_l3_test_ov(int, args_t *, void *);
extern  int     fb_l3_test_hash(int, args_t *, void *);
extern  int     fb_l3_test_done(int, void *);
#endif
#endif /* INCLUDE_L3 */

/* tr_hash.c */
#ifdef BCM_TRX_SUPPORT
extern  int     tr_vlan_xlate_hash_test_init(int, args_t *, void **);
extern  int     tr_vlan_xlate_ov_test_init(int, args_t *, void **);
extern  int     tr_vlan_xlate_test_ov(int, args_t *, void *);
extern  int     tr_vlan_xlate_test_hash(int, args_t *, void *);
extern  int     tr_vlan_xlate_test_done(int, void *);
extern  int     tr_egr_vlan_xlate_hash_test_init(int, args_t *, void **);
extern  int     tr_egr_vlan_xlate_ov_test_init(int, args_t *, void **);
extern  int     tr_egr_vlan_xlate_test_ov(int, args_t *, void *);
extern  int     tr_egr_vlan_xlate_test_hash(int, args_t *, void *);
extern  int     tr_egr_vlan_xlate_test_done(int, void *);
#ifdef BCM_TRIUMPH_SUPPORT
extern  int     tr_mpls_hash_test_init(int, args_t *, void **);
extern  int     tr_mpls_ov_test_init(int, args_t *, void **);
extern  int     tr_mpls_test_ov(int, args_t *, void *);
extern  int     tr_mpls_test_hash(int, args_t *, void *);
extern  int     tr_mpls_test_done(int, void *);
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_ISM_SUPPORT
extern  int     test_generic_hash_init(int unit, args_t *a, void **p);
extern  int     test_generic_hash(int unit, args_t *a, void *p);
extern  int     test_generic_hash_ov_init(int unit, args_t *a, void **p);
extern  int     test_generic_hash_ov(int unit, args_t *a, void *p);
extern  int     test_generic_hash_done(int unit, void *p);
#endif /* BCM_ISM_SUPPORT */
#endif /* BCM_TRX_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */

/* packet.c */

extern  int     tpacket_tx_init(int, args_t *, void **);
extern  int     tpacket_tx_done(int, void *);
extern  int     tpacket_tx_test(int, args_t *, void *);

extern  int     rpacket_init(int, args_t *, void **);
extern  int     rpacket_done(int, void *);
extern  int     rpacket_test(int, args_t *, void *);

/* traffic.c */

extern int    traffic_test_init(int unit, args_t *a, void **pa);
extern int    traffic_test(int unit, args_t *a, void *pa);
extern int    traffic_test_done(int unit, void *pa);

/* snmp.c */

extern  int     snmp_test_init(int, args_t *, void **);
extern  int     snmp_test_test(int, args_t *, void *);
extern  int     snmp_test_done(int, void *);

/* pktspeed.c */

extern  int     pktspeed_test_init(int, args_t *, void **);
extern  int     pktspeed_test_rx(int, args_t *, void *);
extern  int     pktspeed_test_tx(int, args_t *, void *);
extern  int     pktspeed_test_done(int, void *);

#ifdef BCM_88750_A0
#endif /*BCM_88750_A0*/

#ifdef BCM_DFE_SUPPORT
/* fabric_snake_test.c */
extern  int     fabric_snake_test_init(int, args_t *, void **);
extern  int     fabric_snake_test(int, args_t *, void *);
extern  int     fabric_snake_test_done(int, void *);
#endif  /*BCM_DFE_SUPPORT*/

#ifdef BCM_PETRA_SUPPORT
/* arad_test.c */
extern int      arad_bist_test_init(int, args_t *, void **);
extern int      arad_bist_test(int, args_t *, void *);
extern int      arad_bist_test_done(int, void *);

extern int      arad_field_test_init(int, args_t *, void **);
extern int      arad_field_test(int, args_t *, void *);
extern int      arad_field_test_done(int, void *);
#endif /* BCM_PETRA_SUPPORT */

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/*init.c*/
extern int init_deinit_test(int u, args_t *a, void *p);
extern int init_deinit_test_init(int unit, args_t *a, void **p);
extern int init_deinit_test_done(int unit, void *p);
extern int init_deinit_test_internal(int u, args_t *a, void *p);
#endif

#if defined(BCM_PETRA_SUPPORT)
/*exhaustive.c*/
extern int exhaustive_test(int u, args_t *a, void *p);
extern int exhaustive_test_init(int unit, args_t *a, void **p);
extern int exhaustive_test_done(int unit, void *p);
#endif

#ifdef BCM_PETRA_SUPPORT
extern int diag_pp_test(int unit, args_t *a, void *p);
extern int diag_pp_test_init(int unit, args_t *a, void **p);
extern int diag_pp_test_done(int unit, void *p);
#endif

#ifdef BCM_TRIDENT2_SUPPORT
extern int trident2_mem_bist(int unit);
extern int trident2_regfile_bist(int unit);
extern int memory_logic_bist_test_init(int u, args_t *a, void **p);
extern int memory_logic_bist_test(int u, args_t *a, void *p);
extern int memory_logic_bist_test_done(int u, void *p);
extern int mbist_cpu_test_init(int u, args_t *a, void **p);
extern int mbist_cpu_test(int u, args_t *a, void *p);
extern int mbist_cpu_test_done(int u, void *p);
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
extern  int     cambist_test_init(int,args_t *, void **);
extern  int     cambist_test_done(int, void *);
extern  int     cambist_test(int, args_t *, void *);
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
extern soc_error_t tomahawk_mem_bist(int unit);
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
extern soc_error_t tomahawk2_mem_bist(int unit);
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
extern soc_error_t tomahawk3_mem_bist(int unit);
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
extern soc_error_t trident3_mem_bist(int unit);
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_MAVERICK2_SUPPORT
extern soc_error_t maverick2_mem_bist(int unit);
#endif /* BCM_MAVERICK2_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
extern soc_error_t firebolt6_mem_bist(int unit);
#endif /* BCM_FIREBOLT6_SUPPORT */
/* SER Field Test */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
extern  int     ser_ih_test_init(int,args_t *, void **);
extern  int     ser_ih_test_cleanup(int, void *);
extern  int     ser_ih_test(int, args_t *, void *);
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2_SUPPORT */

/* ECC Parity Test */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
extern  int     ecc_par_test_init(int,args_t *, void **);
extern  int     ecc_par_test_cleanup(int, void *);
extern  int     ecc_par_test(int, args_t *, void *);
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2_SUPPORT */

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
extern int memory_ser_test_init(int unit, args_t *a, void **p);
extern int memory_ser_test_run(int unit, args_t *a, void *p);
extern int memory_ser_test_done(int unit, void *p);
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
extern int kaps_bist_test(int unit, args_t *a, void *p);
#endif

#ifdef BCM_PETRA_SUPPORT
extern int lif_learn_info_bist_test(int unit, args_t *a, void *p);
#endif

#if (defined(BCM_88675_A0) || defined(BCM_DNX_SUPPORT)) && defined(INCLUDE_KBP) && !defined(BCM_88030)
extern int kbp_sdk_ver_test(int unit, args_t *a, void *p);
#endif
/* cmicx sim test */
#if defined(PLISIM) && defined(BCM_CMICX_SUPPORT)
extern int cmicx_test_init(int, args_t *, void **);
extern int cmicx_test_run( int, args_t *, void *);
extern int cmicx_test_done(int, void *);
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
extern int kaps_search_test(int unit, args_t *a, void *p);
#endif

#if defined (BCM_PETRA_SUPPORT) && !defined (__KERNEL__) && defined (_SHR_SW_STATE_EXM )
/* sw_state_tests.c */
extern int sw_state_test(int u, args_t *a, void *p);
extern int sw_state_test_init(int unit, args_t *a, void **p);
extern int sw_state_test_done(int unit, void *p);
#endif

#if defined (BCM_PETRA_SUPPORT)
/* test_oam.c */
extern int oamp_test(int unit, args_t *a, void *p);
extern int oamp_test_init(int unit, args_t *a, void **p);
extern int oamp_test_done(int unit, void *p);
#endif

#if defined (BCM_PETRA_SUPPORT) && !defined (__KERNEL__)
/* sw_state_issu_tests.c */
extern int sw_state_issu_test(int u, args_t *a, void *p);
extern int sw_state_issu_test_init(int unit, args_t *a, void **p);
extern int sw_state_issu_test_done(int unit, void *p);
#endif

#if defined (BCM_PETRA_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT)
extern int cr_dma_support_test(int u, args_t *a, void *p);
extern int cr_dma_support_test_init(int unit, args_t *a, void **p);
extern int cr_dma_support_test_done(int unit, void *p);
#endif

#if defined (BCM_PETRA_SUPPORT)
extern int tcam_bist_test(int u, args_t *a, void *p);
#endif

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* test_dnx_data.c */
extern int test_dnxc_data_test(int unit, args_t *a, void *p);
#endif
#if defined(BCM_DNX_SUPPORT)
/* test_dnx_dbal.c */
extern int test_dnx_dbal_unit_test(int unit, args_t *a, void *p);
extern int test_dnx_dbal_logical_test(int unit, args_t *a, void *p);
#endif

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* test_dnxc_init_deinit.c */
extern int test_dnxc_init_test(int unit, args_t *a, void *p);
extern int test_dnxc_init_test_init(int unit, args_t *a, void **p);
extern int test_dnxc_init_test_done(int unit, void *p);
#endif

#if defined(BCM_DNXF_SUPPORT)
/* test_dnxf_fabric_snake.c */
extern int test_dnxf_fabric_snake_test(int unit, args_t *a, void *p);
extern int test_dnxf_fabric_snake_test_init(int unit, args_t *a, void **p);
extern int test_dnxf_fabric_snake_test_done(int unit, void *p);
#endif

/* pktdma_soc_test.c */

#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
extern  int     pktdma_soc_test_init(int,args_t *, void **);
extern  int     pktdma_soc_test_cleanup(int, void *);
extern  int     pktdma_soc_test(int, args_t *, void *);
#endif

/* streaming.c */

#if defined(BCM_ESW_SUPPORT)
extern  int     streaming_test_init(int,args_t *, void **);
extern  int     streaming_test_cleanup(int, void *);
extern  int     streaming_test(int, args_t *, void *);
#endif

/* streaming_l2uc.c */

#if defined(BCM_ESW_SUPPORT)
extern  int     l2uc_test_init(int,args_t *, void **);
extern  int     l2uc_test_cleanup(int, void *);
extern  int     l2uc_test(int, args_t *, void *);
#endif

/* streaming_l2mc.c */

#if defined(BCM_ESW_SUPPORT)
extern  int     l2mc_test_init(int,args_t *, void **);
extern  int     l2mc_test_cleanup(int, void *);
extern  int     l2mc_test(int, args_t *, void *);
#endif

/* streaming_l3uc.c */

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3)
extern  int     l3uc_test_init(int,args_t *, void **);
extern  int     l3uc_test_cleanup(int, void *);
extern  int     l3uc_test(int, args_t *, void *);
#endif

/* streaming_ipmc.c */

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3)
extern  int     ipmc_test_init(int,args_t *, void **);
extern  int     ipmc_test_cleanup(int, void *);
extern  int     ipmc_test(int, args_t *, void *);
#endif

/* sbusdma_test.c */

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT) && \
    defined(BCM_SBUSDMA_SUPPORT)
extern  int     sb_test_init(int, args_t *, void **);
extern  int     sb_test_main(int, args_t *, void *);
extern  int     sb_test_done(int, void *);
extern  int     sb_stress_test(int, args_t *, void *);
#endif

/* ccmdma_test.c */

#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
extern  int     ccmdma_test_init(int,args_t *, void **);
extern 	int 	ccmdma_test_cleanup(int, void *);
extern 	int 	ccmdma_test(int, args_t *, void *);
#endif

/* fifodma_test.c */

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT)
extern  int 	fifodma_test_init(int,args_t *, void **);
extern 	int 	fifodma_test_cleanup(int, void *);
extern 	int 	fifodma_test(int, args_t *, void *);
#endif

/* flexport.c */

#if defined(BCM_ESW_SUPPORT) && !defined(NO_SAL_APPL) && (!defined(__KERNEL__))  && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
extern  int     flexport_test_init(int,args_t *, void **);
extern  int     flexport_test_cleanup(int, void *);
extern  int     flexport_test(int, args_t *, void *);
#endif

/* flexport_td3.c */

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && !defined(NO_SAL_APPL) && (!defined(__KERNEL__))  && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
extern  int     flexport_td3_test_init(int,args_t *, void **);
extern  int     flexport_td3_test_cleanup(int, void *);
extern  int     flexport_td3_test(int, args_t *, void *);
#endif

/* flex_test.c */

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && !defined(NO_SAL_APPL) && (!defined(__KERNEL__))  && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
extern  int     flex_test_init(int,args_t *, void **);
extern  int     flex_test_cleanup(int, void *);
extern  int     flex_test(int, args_t *, void *);
#endif

/* power_test.c  */



#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
extern  int     power_test_init(int,args_t *, void **);
extern  int     power_test_cleanup(int, void *);
extern  int     power_test(int, args_t *, void *);
#endif

/* schanfifo_test.c */

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICX_SUPPORT)
extern  int     schanfifo_test_init(int,args_t *, void **);
extern  int     schanfifo_test_cleanup(int, void *);
extern  int     schanfifo_test(int, args_t *, void *);
#endif

/* soc_mem_bulk_test.c */

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICX_SUPPORT)
extern  int     mem_bulk_test_init(int,args_t *, void **);
extern  int     mem_bulk_test_cleanup(int, void *);
extern  int     mem_bulk_test(int, args_t *, void *);
#endif
/* latency.c */

#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
extern  int     latency_test_init(int,args_t *, void **);
extern  int     latency_test_cleanup(int, void *);
extern  int     latency_test(int, args_t *, void *);
#endif

/* qspi_flash_test.c */

#if defined(BCM_CMICX_SUPPORT)
extern  int     qspi_flash_test_init(int, args_t *, void *);
extern  int     qspi_flash_test_done(int, void *);
extern  int     qspi_flash_test_run(int , args_t *, void *);
#endif

/* a72_test.c */

#if defined(BCM_HELIX5_SUPPORT)
extern  int     a72_access_test_init(int, args_t *, void **);
extern  int     a72_access_test_done(int, void *);
extern  int     a72_access_test(int , args_t *, void *);
#endif

#endif  /* _TEST_TESTLIST_H */

