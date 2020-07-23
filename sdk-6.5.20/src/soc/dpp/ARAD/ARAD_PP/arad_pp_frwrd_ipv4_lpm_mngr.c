/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
   'arad_pp_frwrd_ipv4_lpm_mngr.c' Obfuscated by COBF (Version 1.06 2006-01-07 by BB) at Tue Jun 09 11:54:42 2015
*/
#include<soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include<soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include<soc/dpp/SAND/Utils/sand_array_memory_allocator.h>
#include<soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include<soc/dpp/SAND/Utils/sand_os_interface.h>
#include<soc/dpp/SAND/Management/sand_error_code.h>
#include<soc/dpp/SAND/Management/sand_low_level.h>
#include<soc/dpp/drv.h>
#include<assert.h>
#define l200 typedef
#define l31 int32
#define l377 SOC_SAND_LONG
#define lf uint32
#define l12 SOC_SAND_ULONG
#define l4 static
#define ly uint8
#define l179 int
#define l365 enum
#define l290 ARAD_PP_ARR_MEM_ALLOCATOR_PTR
#define l287 struct
#define l16 addr
#define l22 prefix
#define l99 void
#define l7 SOC_SAND_OUT
#define lv ARAD_PP_IPV4_LPM_MNGR_INST
#define lh SOC_SAND_IN
#define l209 soc_sand_os_memcpy
#define li arr
#define l136 sizeof
#define lx ARAD_PP_IPV4_LPM_MNGR_INFO
#define ln return
#define l248 SOC_SAND_U64
#define l55 val
#define lg if
#define lr init_info
#define lp pxx_model
#define lk else
#define l23 SOC_SAND_INOUT
#define l18 ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
#define l54 ret
#define l10 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l333 SOC_SAND_SET_ERROR_CODE
#define l399 SOC_SAND_VALUE_BELOW_MIN_ERR
#define lj exit
#define l14 SOC_SAND_EXIT_AND_SEND_ERROR
#define ld res
#define l98 SOC_SAND_OK
#define l39 ARAD_PP_IPV4_LPM_PXX_MODEL_P6M
#define l50 ARAD_PP_IPV4_LPM_PXX_MODEL_P4X
#define l56 ARAD_PP_IPV4_LPM_PXX_MODEL_P4X_COMPLETE
#define l176 nof_rows_per_mem
#define l73 bank_to_mem
#define lo SOC_SAND_CHECK_FUNC_RESULT
#define l53 ARAD_PP_IPV4_LPM_PXX_MODEL_P6N
#define l67 ARAD_PP_IPV4_LPM_PXX_MODEL_P6X
#define l40 ARAD_PP_IPV4_LPM_PXX_MODEL_LP4
#define l71 payload
#define l380 arad_pp_ipv4_lpm_entry_decode
#define l334 ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY
#define l30 data
#define l8 for
#define l88 nof_mems
#define l175 nof_banks
#define l154 ARAD_PP_ARR_MEM_ALLOCATOR_INFO
#define l127 mem_alloc_get_fun
#define l27 prime_handle
#define l43 sec_handle
#define l384 arad_pp_arr_mem_allocator_read_chunk
#define l245 soc_sand_os_memset
#define l91 goto
#define l231 continue
#define l414 arad_pp_arr_mem_allocator_read
#define l301 arad_pp_arr_mem_allocator_write_chunk
#define l1 switch
#define ll case
#define l21 default
#define l59 FALSE
#define l90 arad_pp_ipv4_lpm_mngr_row_to_base_addr
#define lw ARAD_DO_NOTHING_AND_EXIT
#define l309 arad_pp_ipv4_lpm_mngr_update_base_addr
#define l11 break
#define lu data_info
#define lm bit_depth_per_bank
#define l79 ARAD_PP_IPV4_LPM_MNGR_T
#define l393 ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS
#define l389 ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear
#define l300 nof_reqs
#define l350 mem_to_bank
#define l246 block_size
#define l304 nof_bits_per_bank
#define l371 arad_pp_arr_mem_allocator_is_availabe_blocks
#define l286 arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get
#define l323 SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM
#define l341 arad_pp_arr_mem_allocator_block_size
#define l378 arad_pp_sw_db_free_list_add
#define l390 arad_pp_arr_mem_allocator_free
#define l221 SOC_SAND_CHECK_NULL_INPUT
#define l368 arad_pp_arr_mem_allocator_malloc
#define l372 soc_sand_os_printf
#define l118 SOC_SAND_PAT_TREE_NODE_KEY
#define l133 SOC_SAND_PAT_TREE_NODE_INFO
#define l82 SOC_SAND_PAT_TREE_INFO
#define l3 pat_tree
#define l366 SOC_SAND_PAT_TREE_FIND_IDENTICAL
#define l367 SOC_SAND_HASH_TABLE_DESTROY
#define l38 nof_vrf_bits
#define l70 pat_tree_get_fun
#define l165 soc_sand_pat_tree_lpm_get
#define l353 node_place
#define l137 TRUE
#define l407 arad_pp_sw_db_free_list_commit
#define l355 arad_pp_ipv4_lpm_mngr_lookup0
#define l369 arad_pp_ipv4_lpm_mngr_create
#define l249 nof_lpms
#define l63 lpms
#define l349 node_data_is_identical_fun
#define l412 arad_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun
#define l396 node_is_skip_in_lpm_identical_data_query_fun
#define l346 arad_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun
#define l143 pat_tree_data
#define l376 root
#define l340 SOC_SAND_PAT_TREE_NULL
#define l112 flags
#define l339 ARAD_PP_LPV4_LPM_SHARED_MEM
#define l251 tree_memory
#define l292 tree_memory_cache
#define l284 tree_size
#define l314 support_cache
#define l332 memory_use
#define l344 soc_sand_pat_tree_create
#define l167 arad_pp_arr_mem_allocator_create
#define l123 mem_allocators
#define l374 ARAD_PP_IPV4_LPM_MEMORY_1
#define l298 ARAD_PP_IPV4_LPM_MEMORY_2
#define l299 ARAD_PP_IPV4_LPM_MEMORY_3
#define l297 ARAD_PP_IPV4_LPM_MEMORY_4
#define l296 ARAD_PP_IPV4_LPM_MEMORY_5
#define l295 ARAD_PP_IPV4_LPM_MEMORY_6
#define l404 soc_sand_os_malloc
#define l235 NULL
#define l403 SOC_SAND_MALLOC_FAIL
#define l149 ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG
#define l191 soc_sand_group_mem_ll_create
#define l92 rev_ptrs
#define l347 arad_pp_ipv4_lpm_mngr_vrf_init
#define l174 SOC_SAND_PP_SYSTEM_FEC_ID
#define l24 key
#define l316 SOC_SAND_PAT_TREE_NODE_DATA
#define l240 SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED
#define l211 soc_sand_pat_tree_node_add
#define l405 SAL_BOOT_QUICKTURN
#define l402 arad_pp_ipv4_lpm_mngr_vrf_clear
#define l201 SOC_PPC_FRWRD_IP_ROUTE_STATUS
#define l103 SOC_SAND_PP_IPV4_SUBNET
#define l281 soc_sand_SAND_PP_IPV4_SUBNET_clear
#define l95 ip_address
#define l96 prefix_len
#define l269 soc_sand_pat_tree_clear
#define l276 nof_entries_for_hw_lpm_set_fun
#define l329 SOC_SAND_SOC_CHECK_FUNC_RESULT
#define l259 arad_pp_ipv4_lpm_mngr_prefix_add
#define l196 SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM
#define l345 arad_pp_ipv4_lpm_mngr_vrf_routes_clear
#define l379 soc_sand_pat_tree_clear_nodes
#define l413 arad_pp_ipv4_lpm_mngr_all_vrfs_clear
#define l303 arad_pp_arr_mem_allocator_clear
#define l270 soc_sand_group_mem_ll_clear
#define l397 soc_sand_pat_tree_root_reset
#define l352 arad_pp_ipv4_lpm_mngr_destroy
#define l418 soc_sand_pat_tree_destroy
#define l409 arad_pp_arr_mem_allocator_destroy
#define l401 soc_sand_group_mem_ll_destroy
#define l256 soc_sand_os_free_any_size
#define l364 arad_pp_ipv4_lpm_mngr_hw_sync
#define l382 arad_pp_ipv4_lpm_mngr_mark_non_lpm_route_pending_type_set
#define l227 sys_fec_id
#define l255 SOC_PPC_FRWRD_IP_ROUTE_LOCATION
#define l357 SOC_SAND_PAT_TREE_FIND_EXACT
#define l419 SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_ADD
#define l331 soc_sand_pat_tree_node_remove
#define l416 arad_pp_ipv4_lpm_mngr_sync
#define l387 SOC_SAND_SUCCESS_FAILURE
#define l335 soc_sand_bitstream_get_any_field
#define l381 soc_sand_pat_tree_head_key_of_changes
#define l370 arad_pp_arr_mem_allocator_rollback
#define l361 soc_sand_group_mem_ll_rollback
#define l360 SOC_SAND_FAILURE_OUT_OF_RESOURCES
#define l359 arad_pp_arr_mem_allocator_commit
#define l410 soc_sand_group_mem_ll_commit
#define l394 SOC_SAND_SUCCESS
#define l383 soc_sand_pat_tree_cache_commit
#define l358 arad_pp_ipv4_lpm_mngr_cache_set
#define l206 enable
#define l373 arad_pp_arr_mem_allocator_cache_set
#define l398 soc_sand_group_mem_ll_cache_set
#define l411 SOC_PPC_FRWRD_IP_ALL_VRFS_ID
#define l325 soc_sand_pat_tree_cache_set
#define l348 arad_pp_ipv4_lpm_mngr_prefix_remove
#define l315 arad_pp_ipv4_lpm_mngr_prefix_lookup
#define l362 arad_pp_ipv4_lpm_mngr_prefix_is_exist
#define l342 arad_pp_ipv4_lpm_mngr_sys_fec_get
#define l400 arad_pp_ipv4_lpm_mngr_get_stat
#define l250 ARAD_PP_IPV4_LPM_MNGR_STATUS
#define l395 soc_sand_pat_tree_get_size
#define l282 total_nodes
#define l324 prefix_nodes
#define l311 free_nodes
#define l408 arad_pp_ipv4_lpm_mngr_clear
#define l343 arad_pp_ipv4_lpm_mngr_get_block
#define l406 SOC_SAND_PAT_TREE_ITER_TYPE
#define l388 ARAD_ALLOC
#define l385 while
#define l417 SOC_SAND_PAT_TREE_ITER_IS_END
#define l392 soc_sand_pat_tree_get_block
#define l363 ARAD_FREE
#define l354 arad_pp_ARAD_PP_IPV4_LPM_MNGR_INFO_clear
#define l108 info
#define l271 SOC_SAND_MAGIC_NUM_SET
#define l327 SOC_SAND_VOID_EXIT_AND_SEND_ERROR
#define l351 arad_pp_ARAD_PP_IPV4_LPM_MNGR_STATUS_clear
l200 l31 l377;l200 lf l12;l4 ly l307[]={0,0,1,1,2,2,2,2,3,3,3,3,3,3,3
,3};l4 l179 l162[]={0,0,0,0,0,0,0,0,0,0,0};l4 lf l130=0;l200 l365{
l163,l68,l121,l35,l17,l75,l89,l15,l144,l117}l140;l200 l290 l97;l4 l31
l199[]={0,35,21,-1,35,-1,35};l4 lf l230[]={0,36,22,22,36,32,36};l200
l287{l287{lf l16;ly l22;ly l356;} *l415;lf l386;}l391;l4 l99 l48(l7 lv
 *l26,lh lv*l28){l209(l26->li,l28->li,l136( *l26));}l4 l179 l116(lh lx
 *la,lh lv*l26,lh lv*l28){ln(l26->li[0]!=l28->li[0]||l26->li[1]!=l28
->li[1]);}l4 ly l305(lh lx*la,lh l248*l55){lf l46;lg(l230[la->lr.lp]
<=32){l46=l55->li[0]&(0xffffffff>>(32-l230[la->lr.lp]));}lk{l46=l55->
li[0];l46^=l55->li[1]&(0xffffffff>>(64-l230[la->lr.lp]));}l46=l46^l46
>>16;l46=l46^l46>>8;l46=l46^l46>>4;l46=l46^l46>>2;l46=l46^l46>>1;ln((
(ly)l46)&1);}l4 l99 l207(lh lx*la,l23 l248*l55){lf l46;lh l18*lc;lc=&
(la->lr);lg(l199[lc->lp]<0){ln;}l46=l305(la,l55);lg(l199[lc->lp]<32){
l55->li[0]^=l46<<l199[lc->lp];}lk{l55->li[1]^=l46<<(l199[lc->lp]-32);
}}l4 lf l126(lf l122,l31*l6){l31 l54=0;l10(0);lg(l122==0){l333(l399,
75,lj);}lg(l122&0xFFFF0000){l54+=16;l122>>=16;}lg(l122&0xFF00){l54+=8
;l122>>=8;}lg(l122&0xf0){l54+=4;l122>>=4;}l54+=l307[l122]; *l6=l54;lj
:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x66"
"\x66\x73\x28\x29",0,0);}l4 lf l313(lh lx*la,l7 lv*le,lh lf lb,lh lf
ls,lh lf l33,lh lf l0){l31 l42;lh l18*lc;lf ld=l98;l10(0);lc=&(la->lr
);lg(lc->lp==l39){;;le->li[0]=(l0>>2)&0x1fff;le->li[0]|=(l33&0xffff)<<
13;le->li[0]|=0x40000000;}lk lg(lc->lp==l50||lc->lp==l56){lg(ls==4){
ld=l126(lc->l176[lc->l73[lb]]-1,&l42);lo(ld,10,lj);l42-=16;lg(l42>0){
;le->li[0]=l0>>l42;}lk lg(l42<0){le->li[0]=l0<<-l42;}}lk{le->li[0]=l0
&0x1fffff;}lg(ls==4){le->li[0]|=l33<<17;le->li[1]=l33>>15;}lk{le->li[
0]|=l33<<21;}lg(ls==4){le->li[1]|=0x4;}lk lg(ls==3){le->li[0]|=1<<31;
le->li[1]=0x6;}lk lg(ls==2){le->li[1]=0x7;}l207(la,le);}lk lg(lc->lp
==l53){;;le->li[0]=(l0>>1)&0x1fff;le->li[0]|=(l33&0xff)<<13;le->li[0]
|=0x400000;le->li[1]=0;}lk lg(lc->lp==l67){;;le->li[0]=(l0>>1)&0x3fff
;le->li[0]|=(l33&0xf)<<14;le->li[0]|=0x180000;le->li[1]=0;}lk lg(lc->
lp==l40){;;le->li[0]=(l0>>1)&0x3ffff;le->li[0]|=(l33&0xffff)<<18;le->
li[1]=0;le->li[1]|=(l33&0xffff)>>14;}lk{;}lj:l14("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x6b\x65\x5f\x65\x6c"
"\x69\x64\x65\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 lf l294(lh lx*la,l7 lv
 *le,lh lf lb,lh lf ls,lh lf l13,lh lf l0){l31 l42;lh l18*lc;lf ld=
l98;l10(0);lc=&(la->lr);lg(lc->lp==l39){;;le->li[0]=(l0>>1)&0x3fff;le
->li[0]|=l13<<14;le->li[1]=0;}lk lg(lc->lp==l50||lc->lp==l56){;ld=
l126(lc->l176[lc->l73[lb]]-1,&l42);lo(ld,10,lj);l42-=17;lg(l42>0){;le
->li[0]=l0>>l42;}lk lg(l42<0){le->li[0]=l0<<-l42;}le->li[0]|=l13<<18;
le->li[1]=l13>>14;l207(la,le);}lk lg(lc->lp==l53){;le->li[0]=l0&
0x3fff;le->li[0]|=l13<<14;le->li[1]=0;}lk{;}lj:l14("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x6b\x65\x5f\x63\x6f"
"\x64\x65\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 l99 l317(lh lx*la,l7 lv*
le,lh lf lb,lh lf l47,lh lf l9,lh lf l0){lh l18*lc;lc=&(la->lr);lg(lc
->lp==l39){;;;;le->li[0]=(l0>>1)&0x3fff;le->li[0]|=(l9<<(21-l47));le
->li[0]|=((1<<(l47-1))-1)<<(28-l47);le->li[0]|=0x60000000;le->li[1]=0
;}lk lg(lc->lp==l50||lc->lp==l56){le->li[0]=l0&0x1fffff;le->li[0]|=(
l9<<(30-l47))&0x3fffffff;le->li[0]|=1<<(29-l47);le->li[1]=0x6;l207(la
,le);}lk lg(lc->lp==l53){;;;;le->li[0]=(l0>>1)&0x1fff;le->li[0]|=(l9
<<(20-l47))&0xfffff;le->li[0]|=1<<(19-l47);le->li[0]|=0x600000;le->li
[1]=0;}lk lg(lc->lp==l67){;;;;lg(l47==5){le->li[0]=(l0>>1)&0x3fff;le
->li[0]|=(l9<<(19-l47))&0x7ffff;le->li[0]|=0x100000;le->li[1]=0;}lk{
le->li[0]=(l0>>1)&0x3fff;le->li[0]|=(l9<<(18-l47))&0x3ffff;le->li[0]
|=(l47-1)<<18;le->li[1]=0;}}lk lg(lc->lp==l40){lf l182;;;l182=l9<<1;
l182|=1;l182=l182<<(7-l47);le->li[0]=(l0)&0x1fffff;le->li[0]|=((l182&
0xff)<<21);le->li[0]|=(((l47-1)&0x7)<<29);le->li[1]=0x6;}lk{;}}l4 l99
l328(l23 lv*le,lh lf l216){;le->li[1]&=0x00000fff;le->li[1]|=l216<<12
;}l99 l375(l23 lv*le){le->li[1]&=0x00000fff;}l4 l99 l233(lh lx*la,l7
lv*le,lh lf lb,lh lf l71,lh lf l216){lh l18*lc;lc=&(la->lr);lg(lc->lp
==l39){;le->li[0]=l71&0x7fff;le->li[0]|=0x70000000;le->li[1]=0x0;}lk
lg(lc->lp==l50||lc->lp==l56){;le->li[0]=l71&0xfffff;le->li[0]|=1<<31;
le->li[1]=0x7;l207(la,le);}lk lg(lc->lp==l53){;lg(lb>=5){le->li[0]=
l71&0x3fff;}lk{le->li[0]=l71&0x3fff;le->li[0]|=0x700000;}le->li[1]=
0x0;}lk lg(lc->lp==l67){;le->li[0]=l71&0x3fff;le->li[0]|=0x1c0000;le
->li[1]=0x0;}lk lg(lc->lp==l40){;le->li[0]=(l71&0xffffffff);le->li[1]
=7;}lk{;}l328(le,l216);}l4 l140 l20(lh lx*la,lh lv*le,lh lf lb){lg(la
->lr.lp==l39){lg(lb>=5)ln(l15);lk lg((le->li[0]&0x40000000)==0)ln(l68
);lk lg((le->li[0]&0x60000000)==0x40000000)ln(l35);lk lg((le->li[0]&
0x70000000)==0x60000000)ln(l17);lk lg((le->li[0]&0x70000000)==
0x70000000)ln(l15);}lk lg(la->lr.lp==l50||la->lr.lp==l56){lg(lb==3)ln
(l144);lk lg((le->li[1]&0x4)==0)ln(l68);lk lg((le->li[1]&0x6)==0x4)ln
(l35);lk lg((le->li[1]&0x7)==0x6&&(le->li[0]&0x80000000)==0)ln(l17);
lk lg((le->li[1]&0x7)==0x6&&(le->li[0]&0x80000000)==0x80000000)ln(l75
);lk lg((le->li[1]&0x7)==0x7&&(le->li[0]&0x80000000)==0)ln(l89);lk lg
((le->li[1]&0x7)==0x7&&(le->li[0]&0x80000000)==0x80000000)ln(l15);}lk
lg(la->lr.lp==l53){lg(lb>=5){ln(l15);}lk lg((le->li[0]&0x400000)==0)ln
(l121);lk lg((le->li[0]&0x600000)==0x400000)ln(l75);lk lg((le->li[0]&
0x700000)==0x600000)ln(l17);lk lg((le->li[0]&0x700000)==0x700000)ln(
l15);}lk lg(la->lr.lp==l67){lg((le->li[0]&0x100000)==0)ln(l17);lk lg(
(le->li[0]&0x180000)==0x100000)ln(l17);lk lg((le->li[0]&0x1c0000)==
0x180000)ln(l89);lk lg((le->li[0]&0x1c0000)==0x1c0000)ln(l15);}lk lg(
la->lr.lp==l40){lg((le->li[1]&0x4)==0x0)ln(l35);lk lg((le->li[1]&0x6)==
0x4)ln(l117);lk lg((le->li[1]&0x7)==0x6)ln(l17);lk lg((le->li[1]&0x7)==
0x7)ln(l15);};ln(l163);}l4 ly l135(lh lx*la,lh lv*le,lh lf lb){lg(lb
>=5)ln 1;lg((le->li[0]&0x70000000)==0x70000000)ln 1;ln 0;}l99 l380(lh
lx*la,lh lf lb,lh l334*l30,lh lf l338,l7 lf*l107){lf l145;lv le;l8(
l145=0;l145<l338;++l145){le.li[l145]=l30[l145];l107[l145]=l30[l145];}
lg(l135(la,&le,lb)){lg(la->lr.lp==l39){lg(lb>=5){l107[0]= *l30&0x7fff
;}lk{l107[0]= *l30&0x7fff;l107[0]|=0x70000000;}}lk lg(la->lr.lp==l67){
lg(lb>=5){l107[0]= *l30&0x3fff;}lk{l107[0]= *l30&0x3fff;l107[0]|=
0x700000;}}lk lg(la->lr.lp==l40){l107[0]=l30[0];l107[1]=7;}}}l4 lf
l274(lh lx*la,lh lf lb,l7 lf*l16){lh l18*lc;lf l62=0;lf l115;lf l57;
l57= *l16;lc=&(la->lr);lg(lc->lp==l40&&lb>0){l115=(lc->l88-1)/(lc->
l175-1);lg(l115==1){l62=lc->l73[lb];}lk lg(l115==2){l62=lc->l73[lb]+(
(l57>>19)&0x1);}lk lg(l115==4){l62=lc->l73[lb]+((l57>>19)&0x3);}lk{;}
 *l16=( *l16&0x7ffff);}lk{l62=lc->l73[lb];}ln l62;}l4 lf l104(lh lx*
la,lh lf lb,lh lf l16,l7 lv*le,lh lf l72){l31 l66;lf l57;lv*l214;l154
 *l83;lf ld;lh l18*lc;lf l85[2* (1<<(8))];l10(0);lc=&(la->lr);l66=l72
;l214=le;l57=l16;ld=lc->l127(lc->l27,lc->l43,lc->l73[lb],&l83);lo(ld,
10,lj);ld=l384(l83,l57,l72,l85);lo(ld,20,lj);l8(l66=0;l66<l72;++l66){
l214[l66].li[0]=l85[2*l66];l214[l66].li[1]=l85[2*l66+1];}lj:l14("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x65\x61\x64"
"\x5f\x72\x6f\x77\x73\x28\x29",0,0);}l4 lf l190(lh lx*la,lh lf lb,lh
lf l16,lh lv*le,lh lf l72){lv l302;lf l66;lf l57;lf l262=0;l154*l83;
l334 l222[2],l225[2],l113=0,l105[2]={0};lf ld;lh l18*lc;lf l85[2* (1
<<(8))];l10(0);l245(&l302,0x0,l136(lv));lc=&(la->lr);lg(l130){l91 lj;
}l57=l16;ld=lc->l127(lc->l27,lc->l43,lc->l73[lb],&l83);lo(ld,10,lj);
l8(l66=0;l66<l72;l66++){lg(lb>=5&&lc->lp==l39){l85[2*l66]=l105[0]=le
->li[0]&0x7fff;l85[2*l66+1]=l105[1]=le->li[1];}lk lg(lb>=5&&lc->lp==
l53){l105[0]=le->li[0]&0x3fff;l85[2*l66+1]=l105[1];}lk{l85[2*l66]=
l105[0]=le->li[0];l85[2*l66+1]=l105[1]=le->li[1];}lg(l113>1){++l57;++
le;l231;}ld=l414(l83,l57,l222);lo(ld,20,lj);lg(l222[0]!=l105[0]||l222
[1]!=l105[1]){lg(l113==0){l225[0]=l105[0];l225[1]=l105[1];l262=l57;}
++l113;}++l57;++le;}lg(l113==1){ld=l301(l83,l262,1,l225);lo(ld,20,lj);
}lk lg(l113>1){ld=l301(l83,l16,l72,l85);lo(ld,20,lj);}lj:l14("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x77\x72\x69\x74"
"\x65\x5f\x72\x6f\x77\x73\x28\x29",0,0);}ly l288(lh lx*la,l7 lv*l26,
lh lv*l28,lh lf lb){lh l18*lc;lc=&(la->lr);lg(lc->lp==l39){l1(l20(la,
l28,lb)){ll l68:ll l17:ln((ly)((l28->li[0]&~0x3fff)==(l26->li[0]&~
0x3fff)));ll l35:ln((ly)((l28->li[0]&~0x1fff)==(l26->li[0]&~0x1fff)));
ll l15:ln((ly)(l135(la,l26,lb)));l21:;ln(l59);}}lk lg(lc->lp==l50||lc
->lp==l56){l1(l20(la,l28,lb)){ll l68:ln(ly)(((l28->li[1]&0x7)==(l26->
li[1]&0x7))&&((l28->li[0]&~0x3ffff)==(l26->li[0]&~0x3ffff)));ll l35:
ln(ly)(((l28->li[1]&0x7)==(l26->li[1]&0x7))&&((l28->li[0]&~0x1ffff)==
(l26->li[0]&~0x1ffff)));ll l17:ln(ly)(((l28->li[1]&0x7)==(l26->li[1]&
0x7))&&((l28->li[0]&~0x1ffff)==(l26->li[0]&~0x1ffff)));ll l75:ln(ly)(
((l28->li[1]&0x7)==(l26->li[1]&0x7))&&((l28->li[0]&~0x1ffff)==(l26->
li[0]&~0x1ffff)));ll l89:ln(ly)(((l28->li[1]&0x7)==(l26->li[1]&0x7))&&
((l28->li[0]&~0x1ffff)==(l26->li[0]&~0x1ffff)));ll l15:ln(ly)(l20(la,
l26,lb)==l15);ll l144:ln(ly)(l20(la,l26,lb)==l144);l21:;ln(l59);}}lk
lg(lc->lp==l53){l1(l20(la,l28,lb)){ll l121:ln((ly)((l28->li[0]&~
0x3fff)==(l26->li[0]&~0x3fff)));ll l75:ln((ly)((l28->li[0]&~0x1fff)==
(l26->li[0]&~0x1fff)));ll l17:ln((ly)((l28->li[0]&~0x1fff)==(l26->li[
0]&~0x1fff)));ll l15:ln((ly)(l20(la,l26,lb)==l15));l21:;ln(l59);}}lk
lg(lc->lp==l67){l1(l20(la,l28,lb)){ll l89:ll l17:ll l15:ln((ly)((l28
->li[0]&0x1fc000)==(l26->li[0]&0x1fc000)));l21:;ln(l59);}}lk lg(lc->
lp==l40){l1(l20(la,l28,lb)){ll l117:ll l35:ln(ly)(((l28->li[0]&
0xfffa0000)==(l26->li[0]&0xfffa0000))&&((l28->li[1]&0x7)==(l26->li[1]
&0x7)));ll l17:ln(ly)(((l28->li[0]&0xffe00000)==(l26->li[0]&
0xffe00000))&&((l28->li[1]&0x7)==(l26->li[1]&0x7)));ll l15:ln((ly)(
l20(la,l26,lb)==l15));l21:;ln(l59);}}lk{;ln(l59);}}lf l90(lh lx*la,lh
lv*le,lh lf lb,l7 l31*l6){l31 l42;lh l18*lc;lf l237=0;lf ld=l98;l10(0
);lc=&(la->lr);lg(lc->lp==l39){l1(l20(la,le,lb)){ll l68:ll l17: *l6=(
(le->li[0]&0x3fff) *2);lw;ll l35: *l6=((le->li[0]&0x1fff) *4);lw;ll
l15: *l6=(-1);lw;l21:; *l6=(-1);lw;}}lk lg(lc->lp==l50||lc->lp==l56){
l1(l20(la,le,lb)){ll l68:ld=l126(lc->l176[lc->l73[lb]]-1,&l42);lo(ld,
10,lj);l42-=17; *l6=(l42>0?(le->li[0]&0x3ffff)<<l42:(le->li[0]&
0x3ffff)>>l42);lw;ll l35:ld=l126(lc->l176[lc->l73[lb]]-1,&l42);lo(ld,
10,lj);l42-=16; *l6=(l42>0?(le->li[0]&0x1ffff)<<l42:(le->li[0]&
0x1ffff)>>l42);lw;ll l17: *l6=(le->li[0]&0x1fffff);lw;ll l75: *l6=(le
->li[0]&0x1fffff);lw;ll l89: *l6=(le->li[0]&0x1fffff);lw;ll l15: *l6=
(-1);lw;ll l144: *l6=(-1);lw;l21:; *l6=(-1);lw;}}lk lg(lc->lp==l53){
l1(l20(la,le,lb)){ll l121: *l6=(le->li[0]&0x3fff);lw;ll l75:ll l17: *
l6=((le->li[0]&0x1fff) *2);lw;ll l15: *l6=(-1);lw;l21:; *l6=(-1);lw;}
}lk lg(lc->lp==l67){l1(l20(la,le,lb)){ll l89:ll l17: *l6=((le->li[0]&
0x3fff) *2);lw;ll l15: *l6=(-1);lw;l21:; *l6=(-1);lw;}}lk lg(lc->lp==
l40){lf l186=0;lf l229=(lc->l88-1)/(lc->l175-1);lg(l229==1){l186=l237
&0x0;}lk lg(l229==2){l186=l237&0x1;}lk lg(l229==4){l186=l237&0x3;}lk{
;}l1(l20(la,le,lb)){ll l117:ll l35: *l6=((l186<<19)|((le->li[0]&
0x3ffff) *2));lw;ll l17: *l6=(le->li[0]&0x1fffff);lw;ll l15: *l6=(-1);
lw;l21:; *l6=(l59);lw;}}lk{; *l6=(-1);lw;}lj:l14("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f"
"\x62\x61\x73\x65\x5f\x61\x64\x64\x72\x28\x29",0,0);}l31 l309(lh lx*
la,l23 lv*le,lh lf lb,lh lf l159){lg(la->lr.lp==l39){l1(l20(la,le,lb)){
ll l68:ll l17:le->li[0]=((le->li[0]&~0x3fff)|l159/2);ln le->li[0];ll
l35:le->li[0]=((le->li[0]&~0x1fff)|l159/4);ln le->li[0];ll l15:ln(-1);
l21:ln(-1);}}lk lg(la->lr.lp==l53){l1(l20(la,le,lb)){ll l75:ll l17:le
->li[0]=((le->li[0]&~0x1fff)|l159/2);ln le->li[0];ll l121:ln le->li[0
]=((le->li[0]&~0x3fff)|l159);ll l15:ln(-1);l21:ln(-1);}}lk lg(la->lr.
lp==l40){l1(l20(la,le,lb)){ll l35:ll l117:le->li[0]=((le->li[0]&~
0x3ffff)|l159/2);ln le->li[0];ll l17:le->li[0]=((le->li[0]&~0x1fffff)|
l159);ln le->li[0];ll l15:ln(-1);l21:ln(-1);}}lk{ln(-1);}}l4 lf l238(
lh lx*la,lh lf lb,lh lf l13,lh lf ls){lf lz;lf l54;lf l147=l13;l147
<<=1;l147|=1;l8(l54=0,lz=0;lz<(lf)(1<<ls);lz++){l1((l147>>lz)&0x3){ll
0x0:ll 0x2:l11;ll 0x1:l54++;l11;ll 0x3:;l54+=1<<(la->lu.lm[lb+2]-la->
lu.lm[lb+1]-ls);l11;}}ln(l54);}l4 lf l291(lh lx*la,lh lf lb,lh lf l13
,lh lf ls){lf lz;lf l54;lf l147=l13;lf l236;lg((la->lu.lm[lb+2]-la->
lu.lm[lb+1])<=ls)l236=1;lk l236=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-
ls);l147<<=1;l147|=1;l8(l54=0,lz=0;lz<((lf)(1<<ls)-1);lz++){l1((l147
>>lz)&0x1){ll 0x0:l11;ll 0x1:l54+=l236;l11;}}ln(l54);}l4 lf l141(lh lx
 *la,lh lf lb,lh lf l33,lh lf ls){lf lz;lf l54;l8(l54=0,lz=0;lz<(lf)(
1<<ls);){lg((l33>>lz)&1){lg(la->lu.lm[lb+2]-la->lu.lm[lb+1]<ls)l54++;
lk l54+=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}lk{l54++;}lg(ls>la->
lu.lm[lb+2]-la->lu.lm[lb+1])lz+=1<<(ls-(la->lu.lm[lb+2]-la->lu.lm[lb+
1]));lk lz++;}ln(l54);}lf l278(lh lx*la,lh lv*le,lh lf lb){lh l18*lc;
lc=&(la->lr);lg(lc->lp==l39){l1(l20(la,le,lb)){ll l68:ln(l238(la,lb,
le->li[0]>>14,4));ll l35:ln(l141(la,lb,le->li[0]>>13,4));ll l17:ln(2);
ll l15:ln(0);l21:;ln(0);}}lk lg(lc->lp==l50||lc->lp==l56){l1(l20(la,
le,lb)){ll l68:ln(l238(la,lb,(le->li[0]>>18)|(le->li[1]<<14),4));ll
l35:ln(l141(la,lb,(le->li[0]>>17)|(le->li[1]<<15),4));ll l17:ln(2);ll
l75:ln(l141(la,lb,le->li[0]>>21,3));ll l89:ln(l141(la,lb,le->li[0]>>
21,2));ll l15:ln(0);ll l144:ln(0);l21:ln(0);}}lk lg(lc->lp==l53){l1(
l20(la,le,lb)){ll l121:ln(l238(la,lb,le->li[0]>>14,3));ll l75:ln(l141
(la,lb,le->li[0]>>13,3));ll l17:ln(2);ll l15:ln(0);l21:;ln(0);}}lk lg
(lc->lp==l67){l1(l20(la,le,lb)){ll l89:ln(l141(la,lb,le->li[0]>>14,2));
ll l17:ln(2);ll l15:ln(0);l21:;ln(0);}}lk lg(lc->lp==l40){lf l243=((
le->li[1]&0x3)<<14)+(le->li[0]>>18);l1(l20(la,le,lb)){ll l35:ln(l141(
la,lb,l243,4));ll l17:ln(2);ll l117:ln(l291(la,lb,l243,4));ll l15:ln(
0);l21:;ln(0);}}lk{;ln(0);}}l4 lf l138(lh lx*la,lh lv*le,lh lf lb,lh
lf ls,l7 lv*lt){lf lz,l49;l31 l0;lf l2;lf l33=0;lf ld;lf l25;lf l153;
lh l18*lc;lh l79*lq;l10(0);lc=&(la->lr);lq=&(la->lu);lg((la->lr.lp==
l39||la->lr.lp==l40)&&lq->lm[lb+2]-lq->lm[lb+1]<5){;lw;}l153=1<<ls;ld
=l90(la,le,lb,&l0);lo(ld,10,lj);lg(lq->lm[lb+2]-lq->lm[lb+1]<ls){l2=1
;}lk{l2=1<<(lq->lm[lb+2]-lq->lm[lb+1]-ls);}lg(lc->lp==l50||lc->lp==
l56){lg(ls==4)l33=le->li[0]>>17;lk l33=le->li[0]>>21;}lk lg(lc->lp==
l53){l33=le->li[0]>>13;}lk lg(lc->lp==l39){l33=le->li[0]>>13;}lk lg(
lc->lp==l67){l33=le->li[0]>>14;}lk lg(lc->lp==l40){l33=(le->li[0]>>18
)+((le->li[1]&0x3)<<14);}lk{;}l33&=(1<<l153)-1;l25=0;l8(l49=0;l49<
l153;){lg((l33>>l49)&1){ld=l104(la,lb+1,l0,&lt[l25],l2);lo(ld,10,lj);
l0+=l2;}lk{ld=l104(la,lb+1,l0,&lt[l25],1);lo(ld,20,lj);l8(lz=1;lz<l2;
lz++)l48(&lt[l25+lz],&lt[l25]);l0+=1;}l25+=l2;lg(ls>lq->lm[lb+2]-lq->
lm[lb+1])l49+=1<<(ls-(lq->lm[lb+2]-lq->lm[lb+1]));lk l49++;}lj:l14(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77"
"\x5f\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x65\x6c"
"\x69\x64\x65\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 lf l226(lh lx*la,lh lv
 *le,lh lf lb,lh lf ls,l7 lv*lt){lf lz,l49;l31 l0;lf l2;lf l13=0;lf ld
;lf l25;lf l84;lh l18*lc;lh l79*lq;l10(0);lc=&(la->lr);lq=&(la->lu);
ld=l90(la,le,lb,&l0);lo(ld,10,lj);l84=1<<ls;lg(lq->lm[lb+2]-lq->lm[lb
+1]<ls){l2=1;}lk{l2=1<<(lq->lm[lb+2]-lq->lm[lb+1]-ls);}lg(lc->lp==l39
){l13=le->li[0]>>14;}lk lg(lc->lp==l50||lc->lp==l56)l13=(le->li[0]>>
18)|(le->li[1]<<14);lk lg(lc->lp==l53){l13=le->li[0]>>14;}lk{;}l13&=(
1<<l84)-1;l25=0;l13<<=1;l13|=1;l8(l49=0;l49<l84;){l1((l13>>l49)&0x3){
ll 0x0:ll 0x2:l8(lz=0;lz<l2;lz++){l48(&lt[l25+lz],&lt[l25-1]);}l11;ll
0x1:ld=l104(la,lb+1,l0,&lt[l25],1);lo(ld,10,lj);l8(lz=1;lz<l2;lz++){
l48(&lt[l25+lz],&lt[l25]);}l0+=1;l11;ll 0x3:ld=l104(la,lb+1,l0,&lt[
l25],l2);lo(ld,20,lj);l0+=l2;l11;}l25+=l2;lg(ls>lq->lm[lb+2]-lq->lm[
lb+1])l49+=1<<(ls-(lq->lm[lb+2]-lq->lm[lb+1]));lk l49++;}lj:l14("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f"
"\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f\x64"
"\x65\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 lf l322(lh lx*la,lh lv*le,lh
lf lb,lh lf ls,l7 lv*lt){lf lz,l49;l31 l0;lf l2;lf l13=0;lf ld;lf l25
;lf l84;lh l18*lc;lh l79*lq;l10(0);lc=&(la->lr);lq=&(la->lu);ld=l90(
la,le,lb,&l0);lo(ld,10,lj);l84=1<<ls;lg(lq->lm[lb+2]-lq->lm[lb+1]<ls){
l2=1;}lk{l2=1<<(lq->lm[lb+2]-lq->lm[lb+1]-ls);}lg(lc->lp==l40){l13=(
le->li[0]>>18)+((le->li[1]&0x1)<<14);}lk{;}l13&=(1<<l84)-1;l25=0;l13
<<=1;l13|=1;l8(l49=0;l49<l84;){l1((l13>>l49)&0x1){ll 0x0:l8(lz=0;lz<
l2;lz++){l48(&lt[l25+lz],&lt[l25-1]);}l11;ll 0x1:ld=l104(la,lb+1,l0,&
lt[l25],l2);lo(ld,10,lj);l0+=l2;l11;}l25+=l2;lg(ls>lq->lm[lb+2]-lq->
lm[lb+1])l49+=1<<(ls-(lq->lm[lb+2]-lq->lm[lb+1]));lk l49++;}lj:l14(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77"
"\x5f\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f"
"\x64\x65\x5f\x31\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 l12 l258(lh lx*la
,lh lv*le,lh lf lb){lg(la->lr.lp==l39){lf l9;l9=(le->li[0]>>14)&0x7f;
;l9&=0x40|((le->li[0]>>21)&0x3f);l9<<=1;l9|=(((~le->li[0]>>21)&0x3f)+
1);ln(l9);}lk lg(la->lr.lp==l50||la->lr.lp==l56){ln((le->li[0]>>21)&
0x1ff);}lk lg(la->lr.lp==l53){ln((le->li[0]>>13)&0x7f);}lk lg(la->lr.
lp==l67){lf l9;lg((le->li[0]&0x100000)==0){l9=(le->li[0]>>14)&0xf;l9
>>=3-((le->li[0]>>18)&0x3);l9<<=3-((le->li[0]>>18)&0x3);l9<<=1;l9|=1
<<(3-((le->li[0]>>18)&0x3));l9<<=1;}lk{l9=(le->li[0]>>14)&0x1f;l9<<=1
;l9|=1;}ln(l9);}lk lg(la->lr.lp==l40){lf l9;lf l213;lf l217;l9=(le->
li[0]>>21)&0xff;l217=(le->li[0]>>29)&0x7;l213=l9|(1<<(6-l217));l213=
l213&(0xff<<(6-l217));;ln l9;};ln(l12)(-1);}l4 lf l181(lh lx*la,lh lv
 *le,lh lf lb,l7 lv*lt){l31 l0;lf l51;lf ld;lf l25;lf l9=0;lf l47=0;
lf l151;lf l220;lf l204;lh l18*lc;lh l79*lq;l10(0);lc=&(la->lr);lq=&(
la->lu);ld=l90(la,le,lb,&l0);lo(ld,10,lj);l51=1<<(lq->lm[lb+2]-lq->lm
[lb+1]);l9=l258(la,le,lb);lg(lc->lp==l39){l47=7;}lk lg(lc->lp==l50||
lc->lp==l56){l47=8;}lk lg(lc->lp==l53){l47=6;}lk lg(lc->lp==l67){l47=
5;}lk lg(lc->lp==l40){l47=7;}lk{;}lg((lq->lm[lb+2]-lq->lm[lb+1])<l47){
l9>>=l47-(lq->lm[lb+2]-lq->lm[lb+1]);}lk lg((lq->lm[lb+2]-lq->lm[lb+1
])>l47){l9<<=(lq->lm[lb+2]-lq->lm[lb+1])-l47;}l151=((l9-1)^l9);l151
>>=1;l9>>=1;l9&=~l151;l220=l9;lg(l9==0){l204=l9+l151+1;}lk{l204=0;}l8
(l25=0;l25<l51;l25++){lg((l25&~l151)==(l9&~l151)){lg(l25==l220){ld=
l104(la,lb+1,l0+1,&lt[l25],1);lo(ld,10,lj);}lk{l48(&lt[l25],&lt[l220]
);}}lk{lg(l25==l204){ld=l104(la,lb+1,l0,&lt[l25],1);lo(ld,20,lj);}lk{
l48(&lt[l25],&lt[l204]);}}}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75\x6c\x6c"
"\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f\x6d\x70\x61\x72\x65\x28\x29",0,
0);}l4 l99 l168(lh lx*la,lh lv*le,lh lf lb,l23 lv*lt){lf l51;lf l49;
lh l79*lq;lq=&(la->lu);l51=1<<(lq->lm[lb+2]-lq->lm[lb+1]);l8(l49=0;
l49<l51;l49++){l48(&lt[l49],le);}}l4 lf l337(lh lx*la,lh lv*le,lh lf
lb,l23 lv*lt,l23 l140*l106){lf ld;lh l18*lc;l10(0);lc=&(la->lr);lg(lc
->lp==l39){l1(l20(la,le,lb)){ll l68:ld=l226(la,le,lb,4,lt);lo(ld,10,
lj); *l106=l68;lw;ll l35:ld=l138(la,le,lb,4,lt);lo(ld,20,lj); *l106=
l35;lw;ll l17:ld=l181(la,le,lb,lt);lo(ld,30,lj); *l106=l17;lw;ll l15:
l168(la,le,lb,lt); *l106=l15;lw;l21:;lw;}}lk lg(lc->lp==l50||lc->lp==
l56){l1(l20(la,le,lb)){ll l68:ld=l226(la,le,lb,4,lt);lo(ld,40,lj);lw;
ll l35:ld=l138(la,le,lb,4,lt);lo(ld,50,lj);lw;ll l17:ld=l181(la,le,lb
,lt);lo(ld,60,lj);lw;ll l75:ld=l138(la,le,lb,3,lt);lo(ld,70,lj);lw;ll
l89:ld=l138(la,le,lb,2,lt);lo(ld,80,lj);lw;ll l15:ll l144:l168(la,le,
lb,lt);lw;l21:;lw;}}lk lg(lc->lp==l53){l1(l20(la,le,lb)){ll l121:ld=
l226(la,le,lb,3,lt);lo(ld,90,lj);lw;ll l75:ld=l138(la,le,lb,3,lt);lo(
ld,100,lj);lw;ll l17:ld=l181(la,le,lb,lt);lo(ld,110,lj);lw;ll l15:
l168(la,le,lb,lt);lw;l21:;lw;}}lk lg(lc->lp==l67){l1(l20(la,le,lb)){
ll l89:ld=l138(la,le,lb,2,lt);lo(ld,120,lj);lw;ll l17:ld=l181(la,le,
lb,lt);lo(ld,130,lj);lw;ll l15:l168(la,le,lb,lt);lw;l21:;lw;}}lk lg(
lc->lp==l40){l1(l20(la,le,lb)){ll l35:ld=l138(la,le,lb,4,lt);lo(ld,
140,lj);lw;ll l17:ld=l181(la,le,lb,lt);lo(ld,150,lj);lw;ll l117:ld=
l322(la,le,lb,4,lt);lo(ld,160,lj);lw;ll l15:l168(la,le,lb,lt);lw;l21:
;lw;}}lk{;};lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61"
"\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x65\x6c\x69\x64\x65\x28\x29",0,
0);}l4 lf l293(lh lx*la,l7 lv*le,lh lf lb,lh lf ls,l23 lv*lt,l23 lv*
l134,lh lf l188,lh l97 l0,l7 l31*l6){lf l51;lf l2;lf l34;lf l60;lv
l177;ly l132=1;lf l33;lh l79*lq;lf ld=l98;l10(0);lq=&(la->lu);l51=1<<
(lq->lm[lb+2]-lq->lm[lb+1]);lg(lq->lm[lb+2]-lq->lm[lb+1]<ls){ *l6=(-1
);lw;}lk{l2=1<<(lq->lm[lb+2]-lq->lm[lb+1]-ls);}l33=0;l8(l34=0,l60=0;
l34<l51;){lg(l34%l2==0){l132=1;l48(&l177,&lt[l34]);}lk lg(!l132||l116
(la,&l177,&lt[l34])!=0)l132=0;l48(&l134[l60],&lt[l34]);l34++;l60++;lg
(!l132&&l60>l188){ *l6=10000;lw;}lg(l34%l2==0){lg(lq->lm[lb+2]-lq->lm
[lb+1]>=ls){l33|=(~l132&1)<<(((l34-l2)>>(lq->lm[lb+2]-lq->lm[lb+1]-ls
)));}lk{l33|=(~l132&1)<<(((l34-l2)<<(ls-(lq->lm[lb+2]-lq->lm[lb+1]))));
}lg(l132){l60-=l2-1;}lg(l60>l188){ *l6=10000;lw;}}}ld=l313(la,le,lb,
ls,l33,l0);lo(ld,10,lj); *l6=(l60);lj:l14("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64"
"\x65\x5f\x74\x6f\x5f\x65\x6c\x69\x64\x65\x5f\x6c\x67\x6e\x5f\x32\x28"
"\x29",0,0);}l4 lf l308(lh lx*la,l7 lv*le,lh lf lb,lh lf ls,l23 lv*lt
,l23 lv*l134,lh lf l188,lh l97 l0,l7 l31*l6){lf l51;lf l2;lf l34;lf
l60;lf l13;lf l101=0;lf l242=0;lf l197=0;lh l79*lq;lf ld=l98;l10(0);
lq=&(la->lu);l51=1<<(lq->lm[lb+2]-lq->lm[lb+1]);lg(lq->lm[lb+2]-lq->
lm[lb+1]<ls){l2=1;}lk{l2=1<<(lq->lm[lb+2]-lq->lm[lb+1]-ls);}l13=1;l8(
l34=0;l34<l51;l34++){lg(l34%l2==0){lg(lq->lm[lb+2]-lq->lm[lb+1]>ls)l101
=l34>>(lq->lm[lb+2]-lq->lm[lb+1]-ls);lk l101=l34<<(ls-(lq->lm[lb+2]-
lq->lm[lb+1]));l197=l34;l242=l197-1;}lk{lg(l116(la,&lt[l197],&lt[l34]
)!=0){l13|=0x2<<l101;}}lg(l197==0||l116(la,&lt[l242],&lt[l34])!=0)l13
|=0x1<<l101;}l8(l60=0,l101=0,l34=0;l101<(lf)(1<<ls);){lg(l13&(0x1<<
l101)){lg(l13&(0x2<<l101)){lf lz;l8(lz=0;lz<l2;lz++){l48(&l134[l60],&
lt[l34]);l60++;l34++;}}lk{l48(&l134[l60],&lt[l34]);l60++;lg(l60>l188){
 *l6=l60;lw;}l34+=l2;}}lk{l34+=l2;}lg(ls>lq->lm[lb+2]-lq->lm[lb+1])l101
+=1<<(ls-(lq->lm[lb+2]-lq->lm[lb+1]));lk l101++;}l13>>=1;ld=l294(la,
le,lb,ls,l13,l0);lo(ld,10,lj); *l6=(l60);lj:l14("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69"
"\x64\x65\x5f\x74\x6f\x5f\x63\x6f\x64\x65\x5f\x6c\x67\x6e\x5f\x32\x28"
"\x29",0,0);}l4 lf l310(lh lx*la,l7 lv*le,lh lf lb,l23 lv*lt,l23 lv*
l134,lh l97 l0,l7 l31*l6){lf l51;lf l77;lv l171[2];lf l86[2];lf l155[
2];lf l94;lh l79*lq;lh l18*lc;lf l252;l31 l81,l257;lf ld=l98;l10(0);
lc=&(la->lr);lq=&(la->lu);l252=lq->lm[lb+2]-lq->lm[lb+1];l51=1<<l252;
l48(&l171[0],&lt[0]);l86[0]=1;l155[0]=0;l86[1]=0;l8(l77=1;l77<l51&&
l116(la,&lt[l77],&l171[0])==0;l77++){l86[0]++;}lg(l77>=l51){ *l6=(-1);
lw;}l48(&l171[1],&lt[l77]);l155[1]=l77;l86[1]++;l8(l77++;l77<l51&&
l116(la,&lt[l77],&l171[1])==0;l77++){l86[1]++;}l8(;l77<l51&&l116(la,&
lt[l77],&l171[0])==0;l77++){l86[0]++;}lg(l77<l51){ *l6=(-1);lw;}lg(
l86[0]<l86[1])l94=0;lk l94=1;lg(((l86[l94]-1)&l86[l94])!=0||((l86[l94
]-1)&l155[l94])!=0){ *l6=(-1);lw;}ld=l126(l51/l86[l94],&l81);lo(ld,10
,lj);l1(lc->lp){ll l39:lg(l81>7){ *l6=(-1);lw;}l11;ll l50:ll l56:lg(
l81>8){ *l6=(-1);lw;}l11;ll l53:lg(l81>6){ *l6=(-1);lw;}l11;ll l67:lg
(l81>5){ *l6=(-1);lw;}l11;ll l40:lg(l81>7){ *l6=(-1);lw;}l11;l21:;l11
;}ld=l126(l86[l94],&l257);lo(ld,10,lj);l317(la,le,lb,l81,l155[l94]>>
l257,l0);l48(&l134[0],&lt[l155[1-l94]]);l48(&l134[1],&lt[l155[l94]]);
 *l6=(2);lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x74\x6f\x5f\x63\x6f"
"\x6d\x70\x61\x72\x65\x5f\x32\x28\x29",0,0);}l4 l31 l321(lh lx*la,l7
lv*le,lh lf lb,l23 lv*lt){lf l51;lf l205;lh l79*lq;lq=&(la->lu);l51=1
<<(lq->lm[lb+2]-lq->lm[lb+1]);l8(l205=l51-1;l205>0;--l205){lg(l116(la
,&lt[0],&lt[l205])!=0){ln-1;}}l48(le,&lt[0]);ln(0);}l4 lf l330(lh lx*
la,l7 lv*le,lh lf lb,l23 lv*lt,l7 l140*l129,lh l140 l124,l7 lf*l6){
l31 l64;l31 l87=-1;l31 l189=-1;lv l215;lh l18*lc;lv l194[(1<<(8))];lv
l241[(1<<(8))];lf ld=l98;l10(0);lc=&(la->lr);l64=(1<<(8))+1;lg(l124==
l163){l87=l321(la,le,lb,lt);lg(l87!=-1&&l87<l64){l64=l87; *l129=l15; *
l6=(l64);lw;}}lg(l124==l15||l124==l17||l124==l163){ld=l310(la,le,lb,
lt,l194,((l97)(-1)),&l87);lo(ld,10,lj);lg(l87!=-1&&l87<l64){l64=l87; *
l129=l17;l209(lt,l194,l136(lv) *l64); *l6=(l64);lw;}}lg(lc->lp==l56||
lc->lp==l39){ld=l308(la,le,lb,4,lt,l194,10000,((l97)(-1)),&l87);lo(ld
,10,lj);lg(l87!=-1&&l87<l64){l64=l87; *l129=l68;l48(&l215,le);}ld=
l293(la,le,lb,4,lt,l241,l87,((l97)(-1)),&l189);lo(ld,10,lj);lg(l189!=
-1&&l189<l64){l64=l189; *l129=l35;l48(&l215,le);l209(lt,l241,l136(lv) *
l64);}lk{lg(l64>=0&&l64<(1<<(8))){l209(lt,l194,l136(lv) *l64);}}}l48(
le,&l215);; *l6=(l64);lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f"
"\x6d\x6e\x67\x72\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x74"
"\x6f\x5f\x6f\x70\x63\x6f\x64\x65\x5f\x32\x28\x29",0,0);}l4 l99 l254(
lh lx*la,l7 lv*le,lh lf lb,l23 lv*lt,lh l97 l0,lh l97 l60){l309(la,le
,lb,l0);l190(la,lb+1,l0,lt,l60);}l4 lf l187(lh lx*la,lh lf l268,l7 ly
 *l224){l154*l102;l393 l148;lf l32;lh l18*lc;lf ld=l98;lf l239;l10(0);
lc=&(la->lr);l389(&l148);lg(l268==0){l148.l300=2;}lk{l148.l300=1;}l8(
l32=1;l32<lc->l88;++l32){l239=la->lr.l350[l32];ld=lc->l127(lc->l27,lc
->l43,l32,&l102);lo(ld,10,lj);l148.l246[0]=((1<<la->lr.l304[l239])+2);
l148.l246[1]=l148.l246[0];ld=l371(l102,&l148,l224);lo(ld,30,lj);lg(! *
l224){lg(l268==2&&l162[l32]<=0){l162[l32]=0; *l224=1;l231;}l162[l32]=
0;l91 lj;}}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61"
"\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x6d\x65\x6d\x6f\x72\x79\x5f\x73\x75\x66\x66\x69\x63\x69\x65"
"\x6e\x74\x28\x29",0,0);}l4 lf l283(lh lx*la,lh lf lb,lh l97 l16){lf
l57,l261;l154*l83;lh l18*lc;ly l169;lf ld;lf l62;l10(0);l57=l16;lc=&(
la->lr);l62=l274(la,lb,&l57);ld=lc->l127(lc->l27,lc->l43,l62,&l83);lo
(ld,10,lj);lg(lc->lp==l50&&lb>=3){l57=l57/2;}ld=l286(lc->l27,l323,&
l169);lo(ld,5,lj);lg(l130){l341(l83,l57,&l261);l162[l62]-=l261;l91 lj
;}lg(!l169){ld=l378(lc->l27,l62,l57);lo(ld,30,lj);}lk{ld=l390(l83,l57
);lo(ld,40,lj);}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72"
"\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x66\x72\x65\x65\x28\x29",0,0);}l4 l12 l279(lh lx*la,lh
lv*le,lh lf lb,lh lf ls){lg(la->lr.lp==l39){l1(ls){ll 4:ln((le->li[0]
>>14)&0xffff);}}lk lg(la->lr.lp==l50||la->lr.lp==l56){l1(ls){ll 4:ln(
((le->li[0]>>18)|(le->li[1]<<14))&0xffff);}}lk lg(la->lr.lp==l53){l1(
ls){ll 3:ln((le->li[0]>>14)&0xff);}};ln(l12)(-1);}l4 l12 l272(lh lx*
la,lh lv*le,lh lf lb,lh lf ls){lg(la->lr.lp==l40){l1(ls){ll 4:ln(((le
->li[1]<<14)|(le->li[0]>>18))&0x7fff);}};ln(l12)(-1);}l4 l12 l285(lh
lx*la,lh lv*le,lh lf lb,lh lf ls){lg(la->lr.lp==l39){l1(ls){ll 4:ln((
le->li[0]>>13)&0xffff);}}lk lg(la->lr.lp==l50||la->lr.lp==l56){l1(ls){
ll 4:ln(((le->li[0]>>17)|(le->li[1]<<15))&0xffff);ll 3:ln((le->li[0]
>>21)&0xff);ll 2:ln((le->li[0]>>21)&0xf);}}lk lg(la->lr.lp==l53){l1(
ls){ll 3:ln((le->li[0]>>13)&0xff);}}lk lg(la->lr.lp==l67){l1(ls){ll 2
:ln((le->li[0]>>14)&0xf);}}lk lg(la->lr.lp==l40){l1(ls){ll 4:ln(((le
->li[1]<<14)|(le->li[0]>>18))&0xffff);}};ln(l12)(-1);}l4 lf l234(lh lx
 *la,lh lf lb,lh lv*le){l31 l0;lf ld=l98;l10(0);ld=l90(la,le,lb,&l0);
lo(ld,10,lj);l283(la,lb+1,l0);lj:l14("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x66\x72\x65\x65\x5f\x72\x6f\x77\x28\x29"
,0,0);}l4 lf l223(lh lx*la,lh lf lb,lh lv*le){lf lz,l264;lv l177;lh
l18*lc;l31 l0;lf ld=l98;l10(0);lc=&(la->lr);ld=l90(la,le,lb,&l0);lo(
ld,10,lj);lg(l0==-1){lw;}l264=l278(la,le,lb);lg(lb<lc->l175-2)l8(lz=0
;lz<l264;lz++){l104(la,lb+1,l0+lz,&l177,1);ld=l223(la,lb+1,&l177);lo(
ld,10,lj);}l234(la,lb,le);lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x66\x72\x65\x65\x5f\x72\x6f\x77\x5f\x72\x65"
"\x63\x75\x72\x73\x69\x76\x65\x28\x29",0,0);}l4 lf l289(lh lx*la,lh lf
lb,lh l140 l106,lh lf l85,l7 l97*l212){l290 l156;l154*l83;lf l185;lf
l74=1;lf ld;lf l62=0;lf l115;lh l18*lc;lf l263=0;l31 l81;l10(0);l221(
l212);lc=&(la->lr);lg(lc->lp==l39){l1(l106){ll l35:l74=4;l11;l21:l74=
2;l11;}}lk lg(lc->lp==l50||lc->lp==l56){l1(l106){ll l75:ll l89:ll l17
:l74=1;l11;ll l68:ld=l126(lc->l176[lc->l73[lb]]-1,&l81);lo(ld,10,lj);
lg(l81-17>0){l74=1<<(l81-17);}l11;ll l35:ld=l126(lc->l176[lc->l73[lb]
]-1,&l81);lo(ld,10,lj);lg(l81-16>0){l74=1<<(l81-16);}l11;l21:l74=1;
l11;}}lk lg(lc->lp==l53){l1(l106){ll l75:l74=2;l11;ll l121:l74=1;l11;
ll l17:l74=2;l11;l21:l74=1;l11;}}lk lg(lc->lp==l67){l74=2;}lk lg(lc->
lp==l40){l1(l106){ll l17:l74=1;l11;ll l117:l74=2;l11;ll l35:l74=2;l11
;l21:l74=1;l11;}}lk{}lg(lc->lp==l50&&lb>=3)l185=(l85+1)/2;lk l185=l85
;lg(lc->lp==l40){l115=(lc->l88-1)/(lc->l175-1);lg(l115==1){l62=lc->
l73[lb];}lk lg(l115==2){l62=lc->l73[lb]+(l263&0x1);}lk lg(l115==4){
l62=lc->l73[lb]+(l263&0x3);}lk{;}}lk{l62=lc->l73[lb];}lg(l130){l162[
l62]+=l185;ln 1;}ld=lc->l127(lc->l27,lc->l43,l62,&l83);lo(ld,10,lj);
l368(l83,l185,l74,&l156);lg(l156==-1){l372("\x45\x52\x52\x4f\x52\x3a"
"\x20\x4c\x50\x4d\x20\x6d\x61\x6c\x6c\x6f\x63\x20\x72\x61\x6e\x20\x6f"
"\x75\x74\x20\x6f\x66\x20\x6d\x65\x6d\x6f\x72\x79\n"); *l212=(
0xFFFFFFFF);}lg(lc->lp==l50&&lb>=3)l156*=2;lk lg(lc->lp==l40)l156=
l156+((l62-lc->l73[lb])<<19); *l212=l156;lj:l14("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x6c\x6c\x6f\x63\x28\x29"
,0,0);}l4 lf l247(lh lx*la,lh lf l5,lh lf l16,lh ly l22,lh lf lb,lh lv
 *l208,l7 lv*l228,lh ly l203,l7 ly*l19){ly l113;lv l76,l69,lt[(1<<(8))]
;l140 l129=0;lf l80,l72,l109,l244,l100;l118 l29;l133 l41;ly l61;ly l52
;lf ld;lf l183=0;l97 l164=(0xFFFFFFFF);l82*l3;ly l170,l320=l366;lh l18
 *lc;lh l79*lq;l140 l124=l163;l31 l0,l178,l180;l10(l367);l100=l16;l52
=l22;lc=&(la->lr);lq=&(la->lu);l29.l55=l100;l29.l22=(ly)(lq->lm[lb]-
lc->l38);ld=lc->l70(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);ld=l165(l3,&
l29,l320,&l41,&l61);lo(ld,20,lj);lg(l61){l244=(((l41.l30.l71)>>0)&
0xFFFFFFF);{l233(la,l228,lb-1,l244,l41.l353);lg(l19){ *l19=l137;}lw;}
}ld=l337(la,l208,lb-1,lt,&l124);lo(ld,30,lj);lg(l203&((0x1)|(0x4))){
l124=l163;}lg(l52<32){l100&=~(((lf)-1)>>l52);}lg(32+lc->l38-lq->lm[lb
+1]<32){l80=l100>>(32+lc->l38-lq->lm[lb+1]);}lk{l80=0;}l80&=((lf)(-1))>>
(32-(lq->lm[lb+1]-lq->lm[lb]));l72=1;lg(l52+lc->l38<lq->lm[lb+1]){l72
<<=lq->lm[lb+1]-l52-lc->l38;}lg(l52==0){l109=0;}lk{l109=l100&(((lf)(-
1))<<(32-l52));}lg(l52<lq->lm[lb+1]-lc->l38){l52=(ly)(lq->lm[lb+1]-lc
->l38);}l113=0;l8(;l72;l72--,l80++,l109+=(1<<(32+lc->l38-lq->lm[lb+1]
))){l48(&l76,&lt[l80]);ld=l247(la,l5,l109,l52,lb+1,&l76,&l69,l203,&
l170);lo(ld,30,lj);lg(!l170){lg(l19){ *l19=l59;}lw;}lg(l116(la,&l69,&
l76)!=0){l113=1;lg(!l135(la,&l76,lb)){lg(l135(la,&l69,lb)){ld=l223(la
,lb,&l76);lo(ld,10,lj);}lk{ld=l90(la,&l76,lb,&l178);lo(ld,10,lj);ld=
l90(la,&l69,lb,&l180);lo(ld,10,lj);lg(l178!=l180){ld=l234(la,lb,&l76);
lo(ld,10,lj);}}}l48(&lt[l80],&l69);}}lg(!l113){l48(l228,l208);}lk{ld=
l330(la,&l69,lb-1,lt,&l129,l124,&l183);lo(ld,10,lj);lg(!l288(la,&l69,
l208,lb-1)){lg(l183!=0){ld=l289(la,lb,l129,l183,&l164);lo(ld,10,lj);
lg(l164==(0xFFFFFFFF)){lg(l19){ *l19=l59;}lw;}lg(!l130){l254(la,&l69,
lb-1,lt,l164,l183);}}}lk{ld=l90(la,l208,lb-1,&l0);lo(ld,10,lj);l164=
l0;l254(la,&l69,lb-1,lt,l164,l183);}l48(l228,&l69);}lg(l19){ *l19=
l137;}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x61\x64\x64\x5f\x70\x72\x65\x66\x69\x78\x5f\x68\x65\x6c\x70\x65"
"\x72\x28\x29",0,0);}l4 lf l193(lh lx*la,lh lf l5,lh lf l16,lh ly l22
,lh ly l203,l7 ly*l19){lf lb;lv l69,l76;lf l80,l109,l72,l100;ly l169;
ly l52;lf ld;ly l170;lh l18*lc;lh l79*lq;l31 l178,l180;l10(0);lc=&(la
->lr);lq=&(la->lu);l52=l22;l100=l16;lb=0;lg(l52<32){l100&=~(((lf)-1)>>
l52);}lg(32+lc->l38-lq->lm[lb+1]<32){l80=l100>>(32+lc->l38-lq->lm[lb+
1]);l80&=((lf)(-1))>>(32+lc->l38-(lq->lm[lb+1]-lq->lm[lb]));}lk{l80=0
;}l80|=l5<<(lq->lm[1]-lc->l38);l72=1;lg(l52+lc->l38<lq->lm[lb+1]){l72
<<=lq->lm[lb+1]-l52-lc->l38;}lg(l52==0){l109=0;}lk{l109=l100&(((lf)(-
1))<<(32-l52));}lg(l52<lq->lm[lb+1]-lc->l38){l52=(ly)(lq->lm[lb+1]-lc
->l38);}l8(;l72;l72--,l80++,l109+=(1<<(32+lc->l38-lq->lm[lb+1]))){ld=
l104(la,lb,l80,&l76,1);lo(ld,10,lj);ld=l247(la,l5,l109,l52,lb+1,&l76,
&l69,l203,&l170);lo(ld,20,lj);lg(!l170){lg(l19){ *l19=l59;}lw;}lg(
l116(la,&l69,&l76)!=0){lg(!l135(la,&l76,lb)){lg(l135(la,&l69,lb)){ld=
l223(la,lb,&l76);lo(ld,10,lj);}lk{ld=l90(la,&l76,lb,&l178);lo(ld,10,
lj);ld=l90(la,&l69,lb,&l180);lo(ld,10,lj);lg(l178!=l180){ld=l234(la,
lb,&l76);lo(ld,10,lj);}}}ld=l190(la,lb,l80,&l69,1);lo(ld,20,lj);}}
l286(lc->l27,l323,&l169);lg(!l169){l407(lc->l27);}lg(l19){ *l19=l137;
}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x61"
"\x64\x64\x5f\x70\x72\x65\x66\x69\x78\x5f\x68\x65\x6c\x70\x65\x72\x30"
"\x28\x29",0,0);}l12 l336(lh lx*la,lh lv*le,lh lf lb){lg(la->lr.lp==
l39){l1(l20(la,le,lb)){ll l15:ln(le->li[0]&0x7fff);l21:ln(l12)(-1);}}
lk lg(la->lr.lp==l50||la->lr.lp==l56){l1(l20(la,le,lb)){ll l15:ln(le
->li[0]&0xfffff);ll l144:l21:ln(l12)(-1);}}lk lg(la->lr.lp==l53){l1(
l20(la,le,lb)){ll l15:ln(le->li[0]&0x3fff);l21:ln(l12)(-1);}}lk lg(la
->lr.lp==l67){l1(l20(la,le,lb)){ll l15:ln(le->li[0]&0x3fff);l21:ln(
l12)(-1);}}lk lg(la->lr.lp==l40){l1(l20(la,le,lb)){ll l15:ln(le->li[0
]&0xffffffff);l21:ln(l12)(-1);}}lk{;ln(l12)(-1);}}l4 l12 l218(lh lx*
la,lh lv*le,lh lf lb,lh l12 l45,lh lf ls){l12 l13;l12 l65;lf lz;lf l84
;lf l2;l12 l37=l45;l13=l279(la,le,lb,ls);l84=1<<ls;lg(la->lu.lm[lb+2]
-la->lu.lm[lb+1]<ls){l2=1;l37<<=ls-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);
}lk{l2=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}l13=(l13<<1)|1;l65=0;
l8(lz=0;lz<l84;lz++){lg(l37<l2)l1((l13>>lz)&0x3){ll 0x0:ll 0x2:ln(l65
-1);ll 0x1:ln(l65);ll 0x3:ln(l65+l37);}l37-=l2;l1((l13>>lz)&0x3){ll
0x0:ll 0x2:l11;ll 0x1:l65+=1;l11;ll 0x3:l65+=l2;l11;}};ln(l12)(-1);}
l4 l12 l273(lh lx*la,lh lv*le,lh lf lb,lh l12 l45,lh lf ls){l12 l13;
l12 l65;lf lz;lf l84;lf l2;l12 l37=l45;l13=l272(la,le,lb,ls);l84=1<<
ls;lg(la->lu.lm[lb+2]-la->lu.lm[lb+1]<ls){l2=1;l37<<=ls-(la->lu.lm[lb
+2]-la->lu.lm[lb+1]);}lk{l2=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}
l13=(l13<<1)|1;l65=0;l8(lz=0;lz<l84;lz++){lg(l37<l2)l1((l13>>lz)&0x1){
ll 0x0:ln(l65-1);ll 0x1:ln(l65+l37);}l37-=l2;l1((l13>>lz)&0x1){ll 0x0
:l11;ll 0x1:l65+=l2;l11;}};ln(l12)(-1);}l4 l12 l142(lh lx*la,lh lv*le
,lh lf lb,lh l12 l45,lh lf ls){l12 l33;lf l2;l12 l65;lf lz;lf l153;lf
l37=l45;l33=l285(la,le,lb,ls);l153=1<<ls;lg(la->lu.lm[lb+2]-la->lu.lm
[lb+1]<ls){l2=1;}lk{l2=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}l65=0
;l8(lz=0;lz<l153;){lg(l37<l2)l1((l33>>lz)&0x1){ll 0x0:ln(l65);ll 0x1:
ln(l65+l37);}l37-=l2;l1((l33>>lz)&0x1){ll 0x0:l65+=1;l11;ll 0x1:l65+=
l2;l11;}lg(ls>la->lu.lm[lb+2]-la->lu.lm[lb+1])lz+=1<<(ls-(la->lu.lm[
lb+2]-la->lu.lm[lb+1]));lk lz++;};ln(l12)(-1);}l4 l12 l184(lh lx*la,
lh lv*le,lh lf lb,lh l12 l45){l12 l9;l12 l37=l45;l9=l258(la,le,lb);lg
(la->lr.lp==l39){lg(la->lu.lm[lb+2]-la->lu.lm[lb+1]<=8)l37<<=8-(la->
lu.lm[lb+2]-la->lu.lm[lb+1]);lk l37>>=(la->lu.lm[lb+2]-la->lu.lm[lb+1
])-8;}lk lg(la->lr.lp==l50||la->lr.lp==l56){lg(la->lu.lm[lb+2]-la->lu
.lm[lb+1]<=9)l37<<=9-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);lk l37>>=(la->
lu.lm[lb+2]-la->lu.lm[lb+1])-9;}lk lg(la->lr.lp==l53){lg(la->lu.lm[lb
+2]-la->lu.lm[lb+1]<=7)l37<<=7-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);lk
l37>>=(la->lu.lm[lb+2]-la->lu.lm[lb+1])-7;}lk lg(la->lr.lp==l67){lg(
la->lu.lm[lb+2]-la->lu.lm[lb+1]<=6)l37<<=6-(la->lu.lm[lb+2]-la->lu.lm
[lb+1]);lk l37>>=(la->lu.lm[lb+2]-la->lu.lm[lb+1])-6;}lk lg(la->lr.lp
==l40){l37<<=8-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);}lk{;ln(l12)(-1);}
l37&=~(l9^(l9-1));l9&=~(l9^(l9-1));lg(l37==l9){ln(1);}lk{ln(0);}}l4
l12 l312(lh lx*la,lh lv*le,lh lf lb,lh l12 l45){lg(la->lr.lp==l39){l1
(l20(la,le,lb)){ll l68:ln(l218(la,le,lb,l45,4));ll l35:ln(l142(la,le,
lb,l45,4));ll l17:ln(l184(la,le,lb,l45));l21:l11;}}lk lg(la->lr.lp==
l50||la->lr.lp==l56){l1(l20(la,le,lb)){ll l68:ln(l218(la,le,lb,l45,4));
ll l35:ln(l142(la,le,lb,l45,4));ll l17:ln(l184(la,le,lb,l45));ll l75:
ln(l142(la,le,lb,l45,3));ll l89:ln(l142(la,le,lb,l45,2));l21:l11;}}lk
lg(la->lr.lp==l53){l1(l20(la,le,lb)){ll l121:ln(l218(la,le,lb,l45,3));
ll l75:ln(l142(la,le,lb,l45,3));ll l17:ln(l184(la,le,lb,l45));l21:l11
;}}lk lg(la->lr.lp==l67){l1(l20(la,le,lb)){ll l89:ln(l142(la,le,lb,
l45,2));ll l17:ln(l184(la,le,lb,l45));l21:l11;}}lk lg(la->lr.lp==l40){
l1(l20(la,le,lb)){ll l117:ln(l273(la,le,lb,l45,4));ll l35:ln(l142(la,
le,lb,l45,4));ll l17:ln(l184(la,le,lb,l45));l21:l11;}};ln(l12)(-1);}
l4 lf l267(lh lx*la,lh l179 l172,lh lf l16,lh lf lb,lh l12 l158,l7 lf
 *l195){lv le;l12 l125=l158;l31 l0;lf ld=l98;l10(0);l104(la,lb,l125,&
le,1);lg(l135(la,&le,lb)){ *l195=(l336(la,&le,lb));lw;}lg(32+la->lr.
l38-la->lu.lm[lb+2]<32){l125=l16>>(32+la->lr.l38-la->lu.lm[lb+2]);
l125&=(1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]))-1;}lk{l125=0;}l125=l312(
la,&le,lb,l125);ld=l90(la,&le,lb,&l0);lo(ld,10,lj);l125+=l0;ld=l267(
la,l172,l16,lb+1,l125,l195);lo(ld,20,lj);lj:l14("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6c\x6f\x6f\x6b\x75\x70\x28\x29"
,0,0);}lf l355(lh lx*la,lh l179 l172,lh lf l5,lh lf l16,l7 lf*l195){
l12 l158;l31 lb;lf ld=l98;l10(0);lb=-1;lg(32+la->lr.l38-la->lu.lm[lb+
2]<32){l158=l16>>(32+la->lr.l38-la->lu.lm[lb+2]);}lk{l158=0;}l158|=l5
<<(la->lu.lm[lb+2]-la->lr.l38);ld=l267(la,l172,l16,lb+1,l158,l195);lo
(ld,10,lj);lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61"
"\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x6c\x6f\x6f\x6b\x75\x70\x30\x28\x29",0,0);}lf l369(l23 lx*
la,lh l179 l172){l18*lc;l79*lq;lf l44,l161;lf ld;l10(0);lc=&(la->lr);
lq=&(la->lu);l8(l44=0;l44<lc->l249;++l44){lc->l63[l44].l349=l412;lc->
l63[l44].l396=l346;lc->l63[l44].l143.l376=l340;lg(l44>0&&lc->l112&
l339){lc->l63[l44].l143.l251=lc->l63[0].l143.l251;lc->l63[l44].l143.
l292=lc->l63[0].l143.l292;lc->l63[l44].l284=lc->l63[0].l284;lc->l63[
l44].l314=lc->l63[0].l314;lc->l63[l44].l143.l332=lc->l63[0].l143.l332
;}lk{lc->l63[l44].l143.l251=0;}ld=l344(&(lc->l63[l44]));lo(ld,10,lj);
}ld=l167(&(lc->l123[l374]));lo(ld,20,lj);ld=l167(&(lc->l123[l298]));
lo(ld,30,lj);ld=l167(&(lc->l123[l299]));lo(ld,40,lj);ld=l167(&(lc->
l123[l297]));lo(ld,50,lj);ld=l167(&(lc->l123[l296]));lo(ld,60,lj);ld=
l167(&(lc->l123[l295]));lo(ld,70,lj);lq->lm=(lf* )l404(l136(lf) * (lc
->l175+1),"\x64\x61\x74\x61\x5f\x69\x6e\x66\x6f\x5f\x70\x74\x72\x2d"
"\x3e\x62\x69\x74\x5f\x64\x65\x70\x74\x68\x5f\x70\x65\x72\x5f\x62\x61"
"\x6e\x6b");lg(lq->lm==l235){l333(l403,75,lj);}lg(lc->l112&l149){ld=
l191(lc->l92[l298]);lo(ld,80,lj);ld=l191(lc->l92[l299]);lo(ld,90,lj);
ld=l191(lc->l92[l297]);lo(ld,100,lj);ld=l191(lc->l92[l296]);lo(ld,110
,lj);ld=l191(lc->l92[l295]);lo(ld,120,lj);}lq->lm[0]=0;l8(l161=0;l161
<lc->l175;l161++){lq->lm[l161+1]=lq->lm[l161]+lc->l304[l161];}lj:l14(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x72\x65"
"\x61\x74\x65\x28\x29",0,0);}lf l347(lh l179 l172,l23 lx*la,lh lf l5,
lh l174 l111){l82*l3;lv l166;l118 l24;l316 l30;lf l36;lf l114;ly l19;
lh l18*lc;lh l79*lq;lf ld;l10(0);lc=&(la->lr);lq=&(la->lu);ld=lc->l70
(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);l24.l22=0;l24.l55=0;l30.l71=(((
(l240)&0x3)<<28)|(((l111)&0xFFFFFFF)<<0));ld=l211(l3,&l24,&l30,&l19);
lo(ld,20,lj);lg(!l405){l233(la,&l166,0,l111,0);l114=l5;l114<<=lq->lm[
1]-lc->l38;l8(l36=l114;l36<l114+(lf)(1<<(lq->lm[1]-lc->l38));++l36){
ld=l190(la,0,l36,&l166,1);lo(ld,100,lj);}}lj:l14("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f\x69\x6e\x69"
"\x74\x28\x29",0,0);}lf l402(l23 lx*la,lh lf l5,lh l174 l111,lh l201
l93){l82*l3;ly l19;lh l18*lc;l103 l110;lf ld;l10(0);lc=&(la->lr);ld=
lc->l70(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);l281(&l110);l110.l95=0;
l110.l96=0;ld=l269(l3);lo(ld,15,lj);ld=lc->l276(lc->l27,0);l329(ld,15
,lj);l259(la,l5,&l110,l111,l93,l196,&l19);lj:l14("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f\x63\x6c\x65"
"\x61\x72\x28\x29",0,0);}lf l345(l23 lx*la,lh lf l5,lh l174 l111,lh
l201 l93){l82*l3;ly l19;lh l18*lc;l103 l110;lf ld;l10(0);lc=&(la->lr);
ld=lc->l70(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);l281(&l110);l110.l95=
0;l110.l96=0;ld=l379(l3);lo(ld,15,lj);ld=lc->l276(lc->l27,0);l329(ld,
20,lj);l259(la,l5,&l110,l111,l93,l196,&l19);lj:l14("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f\x72\x6f\x75"
"\x74\x65\x73\x5f\x63\x6c\x65\x61\x72\x28\x29",0,0);}lf l413(l23 lx*
la,lh lf l120,lh l174 l111){l82*l3;lv l166;l118 l24;l316 l30;lf l36;
lf l114,l5;ly l19;lh l18*lc;lh l79*lq;lf ld;l10(0);lc=&(la->lr);lq=&(
la->lu);l8(l36=0;l36<la->lr.l88;++l36){ld=l303(&(lc->l123[l36]));lo(
ld,40,lj);lg(l36>=1&&la->lr.l112&l149){ld=l270((lc->l92[l36]));lo(ld,
30,lj);}}l8(l5=0;l5<l120;++l5){ld=lc->l70(lc->l27,lc->l43,l5,&l3);lo(
ld,10,lj);lg(l5==0){ld=l269(l3);lo(ld,15,lj);}lk{ld=l397(l3);lo(ld,16
,lj);}l24.l22=0;l24.l55=0;l30.l71=((((l240)&0x3)<<28)|(((l111)&
0xFFFFFFF)<<0));ld=l211(l3,&l24,&l30,&l19);lo(ld,20,lj);l233(la,&l166
,0,l111,0);l114=l5;l114<<=lq->lm[1]-lc->l38;l8(l36=l114;l36<l114+(lf)(
1<<(lq->lm[1]-lc->l38));++l36){ld=l190(la,0,l36,&l166,1);lo(ld,100,lj
);}}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f"
"\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f"
"\x61\x6c\x6c\x5f\x76\x72\x66\x73\x5f\x63\x6c\x65\x61\x72\x28\x29",0,
0);}lf l352(l23 lx*la){l18*lc;l79*lq;lf l58,l44;lf ld;l10(0);lc=&(la
->lr);lq=&(la->lu);l8(l44=0;l44<lc->l249;++l44){ld=l418(&(lc->l63[l44
]));lo(ld,10,lj);lg(lc->l112&l339){l11;}}l8(l58=0;l58<lc->l88;++l58){
ld=l409(&(lc->l123[l58]));lo(ld,20,lj);}lg(lc->l112&l149){l8(l58=1;
l58<lc->l88;++l58){ld=l401(lc->l92[l58]);lo(ld,260,lj);}}lg(lc->l112&
l149){l8(l58=1;l58<lc->l88;++l58){l256(lc->l92[l58]);}l256(lc->l92);}
l256(lq->lm);lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61"
"\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x64\x65\x73\x74\x72\x6f\x79\x28\x29",0,0);}lf l364(l23 lx*
la,lh lf l5){lf l16;ly l22;lf ld;l82*l3;l18*lc;l10(0);lc=&(la->lr);ld
=lc->l70(lc->l27,lc->l43,l5,&l3);lo(ld,20,lj);l16=0;l22=0;ld=l193(la,
l5,l16,l22,(0x1),l235);lo(ld,30,lj);lj:l14("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x68\x77\x5f\x73\x79\x6e\x63\x28\x29"
,0,0);}lf l382(l23 lx*la,lh lf l5,lh l103*l24,l201 l93){l82*l3;l18*lc
;l118 l29;l133 l41;lf l16,l30;ly l22;lf ld;ly l61,l19;l10(0);lc=&(la
->lr);l16=l24->l95;l22=l24->l96;l29.l22=l22;l29.l55=l16;ld=lc->l70(lc
->l27,lc->l43,l5,&l3);lo(ld,10,lj);ld=l165(l3,&l29,l59,&l41,&l61);lo(
ld,20,lj);lg(!l61){l91 lj;}l30=((((l93)&0x3)<<28)|((((((l41.l30.l71)>>
0)&0xFFFFFFF))&0xFFFFFFF)<<0));l41.l30.l71=l30;ld=l211(l3,&l29,&l41.
l30,&l19);lo(ld,10,lj);lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f"
"\x6d\x6e\x67\x72\x5f\x6d\x61\x72\x6b\x5f\x6e\x6f\x6e\x5f\x6c\x70\x6d"
"\x5f\x72\x6f\x75\x74\x65\x5f\x70\x65\x6e\x64\x69\x6e\x67\x5f\x74\x79"
"\x70\x65\x5f\x73\x65\x74\x28\x29",0,0);}lf l259(l23 lx*la,lh lf l5,
lh l103*l24,lh l174 l227,lh ly l152,lh l255 l131,l7 ly*l19){l82*l3;
l18*lc;l118 l29;l133 l41;lf l16,l30;ly l22,l61;lf ld;l10(0);lc=&(la->
lr);l16=l24->l95;l22=l24->l96;lg(!l152){ld=l187(la,0,l19);lo(ld,10,lj
);lg(! *l19){ld=lc->l70(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);l29.l22=
l22;l29.l55=l16;ld=l165(l3,&l29,l357,&l41,&l61);lo(ld,20,lj);lg(!l61){
l91 lj;}lk{ *l19=l137;}}}l202:l29.l22=l22;l29.l55=l16;ld=lc->l70(lc->
l27,lc->l43,l5,&l3);lo(ld,10,lj);l30=(((((l152==l137)?l419:l240)&0x3)<<
28)|(((l227)&0xFFFFFFF)<<0));l41.l30.l71=l30;ld=l211(l3,&l29,&l41.l30
,l19);lo(ld,10,lj);lg(! *l19){l130=0;l91 lj;}lg((!l152)&&((l131==l196
))){ld=l193(la,l5,l16,l22,(0x2),l19);lo(ld,20,lj);}lg(l130==1){ld=
l187(la,2,l19);lo(ld,101,lj);l130=0;lg( *l19){l91 l202;}lk{ld=l331(l3
,&l29);lo(ld,102,lj);}}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f"
"\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x61\x64\x64\x28\x29"
,0,0);}lf l416(l23 lx*la,lh lf l5,lh lf*l266,lh lf l120,lh ly l318,l7
l387*l19){lf l78,l146;ly l139=l59;lf l16,l150;ly l202,l22;lf l32;l154
 *l102;lf ld;l82*l3;lh l18*lc;l118 l219;l10(0);lc=&(la->lr);l16=0;l22
=0;lg(l318){l8(l78=0;l78<l120;++l78){lg(l120==1){l146=l5;}lk{l150=0;
ld=l335(l266,l78,1,&l150);lg(!l150){l231;}l146=l78;}ld=lc->l70(lc->
l27,lc->l43,l146,&l3);lo(ld,20,lj);l381(l3,&l219);l22=l219.l22;l16=
l219.l55;ld=l193(la,l146,l16,l22,(0x1),&l139);lo(ld,30,lj);lg(!l139){
l11;}}lg(l139){l187(la,1,&l202);l139=l202;}lg(!l139){l8(l32=0;l32<lc
->l88;++l32){ld=lc->l127(lc->l27,lc->l43,lc->l88-l32-1,&l102);lo(ld,
30,lj);ld=l370(l102,0);lo(ld,30,lj);lg(l32>=1&&la->lr.l112&l149){ld=
l361((lc->l92[l32]),0);lo(ld,40,lj);}} *l19=l360;l91 lj;}}lk{l139=
l137;}l8(l32=0;l32<lc->l88;++l32){ld=lc->l127(lc->l27,lc->l43,lc->l88
-l32-1,&l102);lo(ld,30,lj);lg(l139){ld=l359(l102,0);lo(ld,30,lj);lg(
l32>=1&&la->lr.l112&l149){ld=l410((lc->l92[l32]),0);lo(ld,40,lj);} *
l19=l394;}}l8(l78=0;l78<l120;++l78){lg(l120==1){l146=l5;}lk{l150=0;ld
=l335(l266,l78,1,&l150);lg(!l150){l231;}l146=l78;}ld=l383(&(la->lr.
l63[l146]),0);lo(ld,30,lj);}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x61\x64\x64\x28"
"\x29",0,0);}lf l358(l23 lx*la,lh lf l5,lh lf l120,lh ly l206){lf l32
,l78;l154*l102;l82*l3;lf ld;lh l18*lc;l10(0);lc=&(la->lr);l8(l32=0;
l32<lc->l88;++l32){ld=lc->l127(lc->l27,lc->l43,l32,&l102);lo(ld,10,lj
);ld=l373(l102,l206);lo(ld,10,lj);lg(la->lr.l112&l149&&l32>=1){ld=
l398((lc->l92[l32]),l206);lo(ld,20,lj);}}lg(l5==l411){l8(l78=0;l78<
l120;++l78){ld=lc->l70(lc->l27,lc->l43,l78,&l3);lo(ld,20,lj);ld=l325(
l3,l206);lo(ld,30,lj);}}lk{ld=lc->l70(lc->l27,lc->l43,l5,&l3);lo(ld,
40,lj);ld=l325(l3,l206);lo(ld,50,lj);}lj:l14("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x61\x63\x68\x65\x5f\x73\x65"
"\x74\x28\x29",0,0);}lf l348(l23 lx*la,lh lf l5,lh l103*l24,lh ly l152
,l7 ly*l19){l82*l3;l18*lc;l118 l29;l133 l41;lf l16;ly l22;lf ld;ly l61
;l10(0);l16=l24->l95;l22=l24->l96;lg(!l152){ld=l187(la,1,l19);lo(ld,
10,lj);lg(! *l19){lg(! *l19){l91 lj;}}}lc=&(la->lr);ld=lc->l70(lc->
l27,lc->l43,l5,&l3);lo(ld,10,lj);l29.l22=l22;l29.l55=l16;ld=l165(l3,&
l29,l59,&l41,&l61);lo(ld,20,lj);lg(!l61){l91 lj;}{ld=l331(l3,&l29);lo
(ld,10,lj);lg(!l152){ld=l193(la,l5,l16,l22,(0x4),l19);lo(ld,20,lj);}}
lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70"
"\x72\x65\x66\x69\x78\x5f\x72\x65\x6d\x6f\x76\x65\x28\x29",0,0);}lf
l315(l23 lx*la,lh lf l5,lh l103*l24,l7 l103*l160,l7 ly*l119,l7 ly*l61
){l82*l3;l18*lc;l118 l29;l133 l41;lf ld;l10(0);lc=&(la->lr);ld=lc->
l70(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);l29.l55=l24->l95;l29.l22=l24
->l96;ld=l165(l3,&l29,l59,&l41,l61);lo(ld,20,lj);lg(!( *l61)){ *l119=
l59;l91 lj;}l160->l95=l41.l24.l55;l160->l96=l41.l24.l22;lg((l160->l95
==l24->l95)&&(l160->l96==l24->l96)){ *l119=l137;}lk{ *l119=l59;}lj:
l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72"
"\x65\x66\x69\x78\x5f\x6c\x6f\x6f\x6b\x75\x70\x28\x29",0,0);}lf l362(
l23 lx*la,lh lf l5,lh l103*l24,l7 ly*l319){l103 l160;ly l306;ly l119;
lf ld;l10(0);ld=l315(la,l5,l24,&l160,&l119,&l306);lo(ld,10,lj); *l319
=l119;lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x70\x72\x65\x66\x69\x78\x5f\x69\x73\x5f\x65\x78\x69\x73\x74\x28"
"\x29",0,0);}lf l342(l23 lx*la,lh lf l5,lh l103*l24,lh ly l119,l7 l174
 *l227,l7 l201*l93,l7 l255*l131,l7 ly*l61){l82*l3;l18*lc;l118 l29;
l133 l41;ly l260;lf ld;l10(0);lc=&(la->lr);ld=lc->l70(lc->l27,lc->l43
,l5,&l3);lo(ld,10,lj);l29.l55=l24->l95;l29.l22=l24->l96;ld=l165(l3,&
l29,l59,&l41,&l260);lo(ld,20,lj);lg(!l260){ *l61=l59;l91 lj;}lg(l119
&&((l41.l24.l55!=l24->l95)||(l41.l24.l22!=l24->l96))){ *l61=l59;l91 lj
;} *l61=l137; *l227=(((l41.l30.l71)>>0)&0xFFFFFFF);lg(l93){ *l93=(((
l41.l30.l71)>>28)&0x3);}lg(l131){ *l131=(l196);}lj:l14("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x73\x79\x73\x5f\x66\x65"
"\x63\x5f\x67\x65\x74\x28\x29",0,0);}lf l400(l23 lx*la,lh lf l5,l7
l250*l198){l82*l3;l18*lc;lf ld;l10(0);l221(l198);lc=&(la->lr);ld=lc->
l70(lc->l27,lc->l43,l5,&l3);lo(ld,10,lj);ld=l395(l3,&(l198->l282),&(
l198->l324),&(l198->l311));lo(ld,20,lj);lj:l14("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x67\x65\x74\x5f\x73\x74\x61\x74"
"\x28\x29",0,0);}lf l408(l23 lx*la){l18*lc;lf l58,l44;lf ld;l10(0);lc
=&(la->lr);l8(l44=0;l44<lc->l249;++l44){ld=l269(&(lc->l63[l44]));lo(
ld,10,lj);}l8(l58=0;l58<lc->l88;++l58){ld=l303(&(lc->l123[l58]));lo(
ld,20,lj);lg(l58>=1&&la->lr.l112&l149){ld=l270((lc->l92[l58]));lo(ld,
30,lj);}}lj:l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x63\x6c\x65\x61\x72\x28\x29",0,0);}lf l343(l23 lx*la,lh lf l5,
lh l406 l277,l23 l248*l253,lh lf l280,lh lf l275,l7 l103*l265,l7 lf*
l326,l7 l201*l93,l7 l255*l131,l7 lf*l210){l133*l128=l235;lf l192,l173
,l232,l157,l36;l18*lc;l82*l3;lf ld;l10(0);lc=&(la->lr);ld=lc->l70(lc
->l27,lc->l43,l5,&l3);lo(ld,10,lj);l173=l275;l36=0;l232=0;l388(l128,
l133,1000,"\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x67\x65\x74\x5f\x62\x6c\x6f\x63\x6b"
"\x2e\x6e\x6f\x64\x65\x73");l385(!l417((l253))&&l173){lg(l173>1000){
l192=1000;}lk{l192=l173;}l173-=l192;ld=l392(l3,l277,l253,l280,l192,
l128,l210);lo(ld,20,lj);l232+= *l210;l8(l157=0;l36<l232;++l36,++l157){
l265[l36].l95=l128[l157].l24.l55;l265[l36].l96=l128[l157].l24.l22;
l326[l36]=(((l128[l157].l30.l71)>>0)&0xFFFFFFF);lg(l93){l93[l36]=(((
l128[l157].l30.l71)>>28)&0x3);}lg(l131){l131[l36]=(l196);}}} *l210=
l36;lj:l363(l128);l14("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72"
"\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x67\x65\x74\x5f\x62\x6c\x6f\x63\x6b\x28\x29",0,0);}l99
l354(l23 lx*l108){l10(0);l221(l108);l245(l108,0x0,l136(lx));l108->lu.
lm=l235;l271;lj:l327(0,0,0);}l99 l351(l23 l250*l108){l10(0);l221(l108
);l245(l108,0x0,l136(l250));l108->l311=0;l108->l324=0;l108->l282=0;
l271;lj:l327(0,0,0);}
#undef l200
#undef l31
#undef l377
#undef lf
#undef l12
#undef l4
#undef ly
#undef l179
#undef l365
#undef l290
#undef l287
#undef l16
#undef l22
#undef l99
#undef l7
#undef lv
#undef lh
#undef l209
#undef li
#undef l136
#undef lx
#undef ln
#undef l248
#undef l55
#undef lg
#undef lr
#undef lp
#undef lk
#undef l23
#undef l18
#undef l54
#undef l10
#undef l333
#undef l399
#undef lj
#undef l14
#undef ld
#undef l98
#undef l39
#undef l50
#undef l56
#undef l176
#undef l73
#undef lo
#undef l53
#undef l67
#undef l40
#undef l71
#undef l380
#undef l334
#undef l30
#undef l8
#undef l88
#undef l175
#undef l154
#undef l127
#undef l27
#undef l43
#undef l384
#undef l245
#undef l91
#undef l231
#undef l414
#undef l301
#undef l1
#undef ll
#undef l21
#undef l59
#undef l90
#undef lw
#undef l309
#undef l11
#undef lu
#undef lm
#undef l79
#undef l393
#undef l389
#undef l300
#undef l350
#undef l246
#undef l304
#undef l371
#undef l286
#undef l323
#undef l341
#undef l378
#undef l390
#undef l221
#undef l368
#undef l372
#undef l118
#undef l133
#undef l82
#undef l3
#undef l366
#undef l367
#undef l38
#undef l70
#undef l165
#undef l353
#undef l137
#undef l407
#undef l355
#undef l369
#undef l249
#undef l63
#undef l349
#undef l412
#undef l396
#undef l346
#undef l143
#undef l376
#undef l340
#undef l112
#undef l339
#undef l251
#undef l292
#undef l284
#undef l314
#undef l332
#undef l344
#undef l167
#undef l123
#undef l374
#undef l298
#undef l299
#undef l297
#undef l296
#undef l295
#undef l404
#undef l235
#undef l403
#undef l149
#undef l191
#undef l92
#undef l347
#undef l174
#undef l24
#undef l316
#undef l240
#undef l211
#undef l405
#undef l402
#undef l201
#undef l103
#undef l281
#undef l95
#undef l96
#undef l269
#undef l276
#undef l329
#undef l259
#undef l196
#undef l345
#undef l379
#undef l413
#undef l303
#undef l270
#undef l397
#undef l352
#undef l418
#undef l409
#undef l401
#undef l256
#undef l364
#undef l382
#undef l227
#undef l255
#undef l357
#undef l419
#undef l331
#undef l416
#undef l387
#undef l335
#undef l381
#undef l370
#undef l361
#undef l360
#undef l359
#undef l410
#undef l394
#undef l383
#undef l358
#undef l206
#undef l373
#undef l398
#undef l411
#undef l325
#undef l348
#undef l315
#undef l362
#undef l342
#undef l400
#undef l250
#undef l395
#undef l282
#undef l324
#undef l311
#undef l408
#undef l343
#undef l406
#undef l388
#undef l385
#undef l417
#undef l392
#undef l363
#undef l354
#undef l108
#undef l271
#undef l327
#include<soc/dpp/SAND/Utils/sand_footer.h>

#endif 

