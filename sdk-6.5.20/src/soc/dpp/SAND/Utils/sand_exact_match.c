/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/SAND/Utils/sand_exact_match.h>
#include<soc/dpp/SAND/Utils/sand_os_interface.h>
#include<soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include<soc/dpp/SAND/Utils/sand_bitstream.h>
#include<soc/dpp/SAND/Management/sand_general_macros.h>
#include<soc/dpp/SAND/Management/sand_error_code.h>
#define l64 static
#define lf uint32
#define l11 SOC_SAND_INOUT
#define l3 SOC_SAND_EXACT_MATCH_INFO
#define l6 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l103 SOC_SAND_EXACT_MATCH_MALLOC_STACK
#define lk SOC_SAND_CHECK_NULL_INPUT
#define lh init_info
#define l47 max_insert_steps
#define l2 for
#define l12 SOC_SAND_EXACT_MATCH_NOF_TABLES
#define ll mgmt_info
#define lo stack
#define l83 SOC_SAND_EXACT_MATCH_STACK_ENTRY
#define l24 soc_sand_os_malloc_any_size
#define l16 sizeof
#define lj key
#define l18 SOC_SAND_EXACT_MATCH_KEY
#define lx uint8
#define lp key_size
#define l61 soc_sand_os_memset
#define lu hash_indx
#define lg exit
#define l8 SOC_SAND_EXIT_AND_SEND_ERROR
#define l100 SOC_SAND_EXACT_MATCH_FREE_STACK
#define l20 soc_sand_os_free_any_size
#define l0 SOC_SAND_IN
#define lt SOC_SAND_OUT
#define l85 soc_sand_exact_match_create
#define l5 SOC_SAND_EXACT_MATCH_INIT_INFO
#define l17 SOC_SAND_EXACT_MATCH_T
#define le res
#define l111 SOC_SAND_EXACT_MATCH_CREATE
#define l19 get_entry_fun
#define l31 set_entry_fun
#define lv if
#define l50 key_bits
#define l23 hash_bits
#define l45 verifier_bits
#define l27 SOC_SAND_SET_ERROR_CODE
#define l78 SOC_SAND_EXACT_MATCH_SIZE_OUT_OF_RANGE_ERR
#define lr table_size
#define l46 SOC_SAND_NOF_BITS_IN_UINT32
#define l38 SOC_SAND_DIV_ROUND_UP
#define l39 SOC_SAND_NOF_BITS_IN_BYTE
#define l51 verifier_size
#define lz keys
#define l71 SOC_SAND_MALLOC_FAIL
#define l15 use_bitmap
#define l73 soc_sand_bitstream_clear
#define lm SOC_SAND_CHECK_FUNC_RESULT
#define ls tmp_entry
#define l72 SOC_SAND_EXACT_MATCH_ENTRY
#define l14 verifier
#define l87 SOC_SAND_EXACT_MATCH_VERIFIER
#define l13 payload
#define l68 SOC_SAND_EXACT_MATCH_PAYLOAD
#define l74 payload_size
#define l119 soc_sand_exact_match_destroy
#define l90 SOC_SAND_EXACT_MATCH_DESTROY
#define l99 soc_sand_exact_match_entry_add
#define l77 SOC_SAND_EXACT_MATCH_HASH_VAL
#define l112 SOC_SAND_EXACT_MATCH_ENTRY_ADD
#define l26 hw_set_fun
#define l53 key_to_verifier
#define ly prime_handle
#define l55 TRUE
#define l44 goto
#define l43 soc_sand_os_memcpy
#define l33 key_to_hash
#define l28 is_valid_entry_get
#define l7 sec_handle
#define l79 break
#define l59 FALSE
#define l117 soc_sand_os_printf
#define l34 is_valid_entry_set
#define l40 hw_get_fun
#define l107 soc_sand_exact_match_entry_remove
#define l94 SOC_SAND_EXACT_MATCH_ENTRY_REMOVE
#define l81 SOC_SAND_EXACT_MATCH_ENTRY_LOOKUP
#define l118 soc_sand_os_memcmp
#define l123 soc_sand_exact_match_entry_lookup
#define l120 soc_sand_exact_match_clear
#define l101 SOC_SAND_EXACT_MATCH_CLEAR
#define l60 SOC_SAND_NULL_POINTER_ERR
#define l91 soc_sand_exact_match_get_block
#define l96 SOC_SAND_TABLE_BLOCK_RANGE
#define l108 SOC_SAND_EXACT_MATCH_GET_BLOCK
#define l89 SOC_SAND_GET_BIT
#define l48 iter
#define l88 SOC_SAND_GET_BITS_RANGE
#define l93 entries_to_act
#define l104 entries_to_scan
#define l92 SOC_SAND_EXACT_MATCH_NULL
#define l105 soc_sand_exact_match_get_size_for_save
#define l57 const
#define l35 total_size
#define l95 soc_sand_exact_match_save
#define l122 buffer_size_bytes
#define l22 cur_size
#define l70 SOC_SAND_COPY_TO_BUFF_AND_INC
#define l106 soc_sand_exact_match_restore
#define l121 SOC_SAND_EXACT_MATCH_KEY_TO_HASH
#define l109 SOC_SAND_EXACT_MATCH_KEY_TO_VERIFY
#define l113 SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET
#define l114 SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET
#define l115 SOC_SAND_EXACT_MATCH_HW_ENTRY_SET
#define l116 SOC_SAND_EXACT_MATCH_HW_ENTRY_GET
#define l102 SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET
#define l110 SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET
#define l80 SOC_SAND_EXACT_MATCH_INFO_clear
#define l86 void
#define ln info
#define l56 NULL
#define l97 SOC_SAND_MAGIC_NUM_SET
#define l98 SOC_SAND_VOID_EXIT_AND_SEND_ERROR
l64 lf l76(l11 l3*lc){lf ld,lw,l29;l6(l103);lk(lc);l29=lc->lh.l47;l2(
ld=0;ld<l12;++ld){lc->ll.lo[ld]=(l83* )l24(l16(l83) *l29,"\x6d\x61"
"\x74\x63\x68\x31");l2(lw=0;lw<l29;++lw){lc->ll.lo[ld][lw].lj=(l18)l24
(l16(lx) *lc->ll.lp,"\x6d\x61\x74\x63\x68\x32");l61(lc->ll.lo[ld][lw]
.lj,0x0,lc->ll.lp);lc->ll.lo[ld][lw].lu=0;}}lg:l8("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78\x61"
"\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x6d\x61\x6c\x6c\x6f\x63\x5f\x73"
"\x74\x61\x63\x6b\x28\x29",0,0);}l64 lf l75(l11 l3*lc){lf ld,lw,l29;
l6(l100);lk(lc);l29=lc->lh.l47;l2(ld=0;ld<l12;++ld){l2(lw=0;lw<l29;++
lw){l20(lc->ll.lo[ld][lw].lj);}l20(lc->ll.lo[ld]);}lg:l8("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65"
"\x78\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x66\x72\x65\x65\x5f\x73"
"\x74\x61\x63\x6b\x28\x29",0,0);}l64 lf l52(l11 l3*lc,l0 l18 lj,lt lf
 *l10,lt lf*lu,lt lx*l1);lf l85(l11 l3*lc){l5*lb;l17*la;lf l4,ld;lf le
;l6(l111);lk(lc);lk(lc->lh.l19);lk(lc->lh.l31);lb=&(lc->lh);la=&(lc->
ll);lv(lb->l50==0||lb->l23==0||lb->l45==0){l27(l78,10,lg);}lv(lb->l23
>=32){l27(l78,20,lg);}la->lr=(1<<lb->l23);l4=la->lr/l46;la->lp=l38(lb
->l50,l39);la->l51=l38(lb->l45,l39);l2(ld=0;ld<l12;++ld){la->lz[ld]=(
l18)l24(la->lr*la->lp,"\x6d\x61\x74\x63\x68\x33");lv(!la->lz[ld]){l27
(l71,30,lg);}l61(la->lz[ld],0x0,la->lr*la->lp*l16(lx));la->l15[ld]=(
lf* )l24(l4*l16(lf),"\x6d\x61\x74\x63\x68\x34");lv(!la->l15[ld]){l27(
l71,40,lg);}le=l73(la->l15[ld],l4);lm(le,50,lg);}la->ls=(l72* )l24(
l16(l72),"\x6d\x61\x74\x63\x68\x35");la->ls->lj=(l18)l24(la->lp,"\x6d"
"\x61\x74\x63\x68\x36");la->ls->l14=(l87)l24(la->l51,"\x6d\x61\x74"
"\x63\x68\x37");la->ls->l13=(l68)l24(la->l74,"\x6d\x61\x74\x63\x68"
"\x38");le=l76(lc);lm(le,60,lg);lg:l8("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78\x61\x63\x74\x5f"
"\x6d\x61\x74\x63\x68\x5f\x63\x72\x65\x61\x74\x65\x28\x29",0,0);}lf
l119(l11 l3*lc){l5*lb;l17*la;lf ld;lf le;l6(l90);lk(lc);lb=&(lc->lh);
la=&(lc->ll);la->lp=l38(lb->l50,l39);la->l51=l38(lb->l45,l39);l2(ld=0
;ld<l12;++ld){l20(la->lz[ld]);l20(la->l15[ld]);}l20(la->ls->lj);l20(
la->ls->l14);l20(la->ls->l13);l20(la->ls);le=l75(lc);lm(le,10,lg);lg:
l8("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e"
"\x64\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x64\x65\x73"
"\x74\x72\x6f\x79\x28\x29",0,0);}lf l99(l11 l3*lc,l0 l18 lj,l0 l68 l13
,lt lf*l30,lt lf*lw,lt lf*l66,lt lx*l49){l5*lb;l17*la;l77 l42,l41,l58
=0;lx l63,l62,l1;lf lq=0,l10,lu;lf li;lf le;l6(l112);lk(lc);lk(lj);lk
(l13);lk(l49);lb=&(lc->lh);la=&(lc->ll);le=l52(lc,lj,&l10,&lu,&l1);lm
(le,10,lg);lv(l1){lv(lb->l26){lb->l53(lj,l10,la->ls->l14);le=lb->l26(
lb->ly,l10,lu,l13,la->ls->l14);lm(le,20,lg);} *l66=1; *l49=l55; *l30=
l10; *lw=lu;l44 lg;}l43(la->lo[0][0].lj,lj,la->lp);l43(la->lo[1][0].
lj,lj,la->lp);l2(li=0;li<lb->l47;++li){l42=lb->l33(la->lo[0][li].lj,0
);l41=lb->l33(la->lo[1][li].lj,1);la->lo[0][li].lu=l42;la->lo[1][li].
lu=l41;le=lb->l28(lb->ly,lb->l7,0,l42,&l63);lm(le,30,lg);lv(!l63){lq=
0;l58=l42;l79;}le=lb->l28(lb->ly,lb->l7,1,l41,&l62);lm(le,40,lg);lv(!
l62){lq=1;l58=l41;l79;}le=lb->l19(lb->ly,lb->l7,la->lz[0],l42,la->lp,
la->lo[1][li+1].lj);lm(le,50,lg);le=lb->l19(lb->ly,lb->l7,la->lz[1],
l41,la->lp,la->lo[0][li+1].lj);lm(le,60,lg);}lv(li==lb->l47){ *l49=
l59;l117("\x20\x46\x41\x49\x4c\x20\n");l44 lg;} *l49=l55; *l66=li+1;
le=lb->l34(lb->ly,lb->l7,lq,l58,l55);lm(le,70,lg);l2(lq^=1;li>0;--
li,lq^=1){le=lb->l40(lb->ly,lq,la->lo[lq][li-1].lu,la->ls->l13,la->ls
->l14);lm(le,80,lg);lb->l53(la->lo[lq^1][li].lj,lq^1,la->ls->l14);le=
lb->l26(lb->ly,lq^1,la->lo[lq^1][li].lu,la->ls->l13,la->ls->l14);lm(
le,90,lg);le=lb->l31(lb->ly,lb->l7,la->lz[lq^1],la->lo[lq^1][li].lu,
la->lp,la->lo[lq^1][li].lj);lm(le,100,lg);}lb->l53(lj,lq^1,la->ls->
l14);le=lb->l26(lb->ly,lq^1,la->lo[lq^1][0].lu,l13,la->ls->l14);lm(le
,110,lg);le=lb->l31(lb->ly,lb->l7,la->lz[lq^1],la->lo[lq^1][0].lu,la
->lp,lj);lm(le,120,lg); *l30=lq^1; *lw=la->lo[lq^1][0].lu;lg:l8("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f"
"\x65\x78\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x65\x6e\x74\x72\x79"
"\x5f\x61\x64\x64\x28\x29",0,0);}lf l107(l11 l3*lc,l0 l18 lj){l5*lb;
lx l1;lf l10,lu;lf le;l6(l94);lk(lc);lk(lj);lb=&(lc->lh);le=l52(lc,lj
,&l10,&lu,&l1);lm(le,10,lg);lv(!l1){l44 lg;}le=lb->l34(lb->ly,lb->l7,
l10,lu,l59);lm(le,20,lg);lg:l8("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61"
"\x74\x63\x68\x5f\x65\x6e\x74\x72\x79\x5f\x72\x65\x6d\x6f\x76\x65\x28"
"\x29",0,0);}lf l52(l11 l3*lc,l0 l18 lj,lt lf*l10,lt lf*lu,lt lx*l1){
lf li;lx l36;l77 l54;l5*lb;l17*la;lf le;l6(l81);lk(lc);lk(lj);lk(l10);
lk(lu);lk(l1);lb=&(lc->lh);la=&(lc->ll); *l1=l59;l2(li=0;li<l12;++li){
l54=lb->l33(lj,li);le=lb->l28(lb->ly,lb->l7,li,l54,&l36);lm(le,10,lg);
lv(l36){le=lb->l19(lb->ly,lb->l7,la->lz[li],l54,la->lp,la->ls->lj);lm
(le,20,lg);lv(l118(la->ls->lj,lj,la->lp)==0){ *l1=l55; *lu=l54; *l10=
li;l44 lg;}}} *l1=l59;lg:l8("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61\x74"
"\x63\x68\x5f\x66\x69\x6e\x64\x5f\x65\x6e\x74\x72\x79\x28\x29",0,0);}
lf l123(l11 l3*lc,l0 l18 lj,lt lf*l30,lt lf*lw,lt l68 l13,lt lx*l1){
l5*lb;l17*la;lf le;l6(l81);lk(lc);lk(lj);lk(l30);lk(lw);lk(l13);lk(l1
);lb=&(lc->lh);la=&(lc->ll);le=l52(lc,lj,l30,lw,l1);lm(le,10,lg);lv( *
l1){le=lb->l40(lb->ly, *l30, *lw,l13,la->ls->l14);lm(le,20,lg);}lg:l8
("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e"
"\x64\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x65\x6e\x74"
"\x72\x79\x5f\x6c\x6f\x6f\x6b\x75\x70\x28\x29",0,0);}lf l120(l11 l3*
lc){l5*lb;l17*la;lf l4,ld,lr;lf le;l6(l101);lk(lc);lb=&(lc->lh);la=&(
lc->ll);lr=(1<<lb->l23);l4=lr/l46;la->lp=l38(lb->l50,l39);la->l51=l38
(lb->l45,l39);l2(ld=0;ld<l12;++ld){lv(!la->lz[ld]){l27(l60,10,lg);}
l61(la->lz[ld],0x0,lr*la->lp*l16(lx));lv(!la->l15[ld]){l27(l60,20,lg);
}le=l73(la->l15[ld],l4);lm(le,30,lg);}lg:l8("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78\x61\x63\x74"
"\x5f\x6d\x61\x74\x63\x68\x5f\x63\x6c\x65\x61\x72\x28\x29",0,0);}lf
l91(l11 l3*lc,l11 l96*l21,lt l18 lz,lt lf*l67){l5*lb;l17*la;lf ld;lf
l25,l69=0,l37=0;lx l36;lf le;l6(l108);lk(lc);lb=&(lc->lh);la=&(lc->ll
);ld=l89(l21->l48,31);l25=l88(l21->l48,30,0);l2(;ld<l12;++ld){l2(;l25
<la->lr;++l25,++l69){le=lb->l28(lb->ly,lb->l7,ld,l25,&l36);lm(le,10,
lg);lv(l36){le=lb->l19(lb->ly,lb->l7,la->lz[ld],l25,la->lp,lz+(l37*la
->lp));lm(le,20,lg);++l37;lv(l37>=l21->l93||l69>=l21->l104){l21->l48=
ld<<31;l21->l48+=(l25+1); *l67=l37;l44 lg;}}}l25=0;} *l67=l37;l21->
l48=l92;lg:l8("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f"
"\x73\x61\x6e\x64\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f"
"\x67\x65\x74\x5f\x62\x6c\x6f\x63\x6b\x28\x29",0,0);}lf l105(l0 l3*lc
,lt lf*l65){l57 l5*lb;l57 l17*la;lf l4,ld,lr;lf l35=0;l6(0);lk(lc);lk
(l65);lb=&(lc->lh);la=&(lc->ll);l35+=l16(l5);lr=(1<<lb->l23);l4=lr/
l46;l2(ld=0;ld<l12;++ld){l35+=lr*la->lp;l35+=l4;} *l65=l35;lg:l8(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64"
"\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x67\x65\x74\x5f"
"\x73\x69\x7a\x65\x5f\x66\x6f\x72\x5f\x73\x61\x76\x65\x28\x29",0,0);}
lf l95(l0 l3*lc,lt lx*l32,l0 lf l122,lt lf*l82){l57 l5*lb;l57 l17*la;
lf l4,ld,lr;lx*l9=(lx* )l32;lf l22,l35=0;lf le;l6(0);lk(lc);lk(l32);
lk(l82);lb=&(lc->lh);la=&(lc->ll);l70(l9,lb,l5,1);lr=(1<<lb->l23);l4=
lr/l46;l2(ld=0;ld<l12;++ld){lv(!la->lz[ld]){l27(l60,10,lg);}l70(l9,la
->lz[ld],lx,lr*la->lp);lv(!la->l15[ld]){l27(l60,20,lg);}l70(l9,la->
l15[ld],lx,l4);}lg:l8("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f"
"\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78\x61\x63\x74\x5f\x6d\x61\x74\x63"
"\x68\x5f\x73\x61\x76\x65\x28\x29",0,0);}lf l106(l0 lx*l32,lt l3*lc,
l0 l121 l33,l0 l109 l84,l0 l113 l31,l0 l114 l19,l0 l115 l26,l0 l116
l40,l0 l102 l34,l0 l110 l28){l5*lb;l17*la;lf l4,ld,lr;lx*l9=(lx* )l32
;lf l22;lf le;l6(0);lk(lc);lk(l32);l80(lc);l43(&(lc->lh),l9,l16(l5));
l9+=l16(l5);lc->lh.l31=l31;lc->lh.l19=l19;lc->lh.l34=l34;lc->lh.l28=
l28;lc->lh.l26=l26;lc->lh.l40=l40;lc->lh.l33=l33;lc->lh.l53=l84;le=
l85(lc);lm(le,20,lg);lb=&(lc->lh);la=&(lc->ll);lr=(1<<lb->l23);l4=lr/
l46;l2(ld=0;ld<l12;++ld){l22=lr*la->lp;l43(la->lz[ld],l9,l22);l9+=l22
;l22=l4;l43(la->l15[ld],l9,l22);l9+=l22;}l32=l9;lg:l8("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x73\x61\x6e\x64\x5f\x65\x78"
"\x61\x63\x74\x5f\x6d\x61\x74\x63\x68\x5f\x72\x65\x73\x74\x6f\x72\x65"
"\x28\x29",0,0);}l86 l80(lt l3*ln){lf li;l6(0);lk(ln);l61(ln,0x0,l16(
l3));ln->lh.ly=0;ln->lh.l7=0;ln->lh.l47=0;ln->lh.l50=0;ln->lh.l23=0;
ln->lh.l45=0;ln->lh.l19=0;ln->lh.l31=0;ln->lh.l40=0;ln->lh.l26=0;ln->
lh.l28=0;ln->lh.l34=0;ln->lh.l33=0;ln->lh.l53=0;l2(li=0;li<l12;++li){
ln->ll.lz[li]=l56;ln->ll.l15[li]=l56;ln->ll.lo[li]=l56;}ln->ll.ls=l56
;ln->ll.lp=0;ln->ll.l74=0;ln->ll.l51=0;l97;lg:l98(0,0,0);}
#undef l64
#undef lf
#undef l11
#undef l3
#undef l6
#undef l103
#undef lk
#undef lh
#undef l47
#undef l2
#undef l12
#undef ll
#undef lo
#undef l83
#undef l24
#undef l16
#undef lj
#undef l18
#undef lx
#undef lp
#undef l61
#undef lu
#undef lg
#undef l8
#undef l100
#undef l20
#undef l0
#undef lt
#undef l85
#undef l5
#undef l17
#undef le
#undef l111
#undef l19
#undef l31
#undef lv
#undef l50
#undef l23
#undef l45
#undef l27
#undef l78
#undef lr
#undef l46
#undef l38
#undef l39
#undef l51
#undef lz
#undef l71
#undef l15
#undef l73
#undef lm
#undef ls
#undef l72
#undef l14
#undef l87
#undef l13
#undef l68
#undef l74
#undef l119
#undef l90
#undef l99
#undef l77
#undef l112
#undef l26
#undef l53
#undef ly
#undef l55
#undef l44
#undef l43
#undef l33
#undef l28
#undef l7
#undef l79
#undef l59
#undef l117
#undef l34
#undef l40
#undef l107
#undef l94
#undef l81
#undef l118
#undef l123
#undef l120
#undef l101
#undef l60
#undef l91
#undef l96
#undef l108
#undef l89
#undef l48
#undef l88
#undef l93
#undef l104
#undef l92
#undef l105
#undef l57
#undef l35
#undef l95
#undef l122
#undef l22
#undef l70
#undef l106
#undef l121
#undef l109
#undef l113
#undef l114
#undef l115
#undef l116
#undef l102
#undef l110
#undef l80
#undef l86
#undef ln
#undef l56
#undef l97
#undef l98
#include<soc/dpp/SAND/Utils/sand_footer.h>
