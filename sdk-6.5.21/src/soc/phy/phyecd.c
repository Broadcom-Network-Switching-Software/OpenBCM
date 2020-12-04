/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyecd.c
 * Purpose:     PHY ECD routines
 */

#include <sal/core/thread.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/phy/phyctrl.h>
#include <shared/bsl.h>
#include "phyreg.h"

int phy_ecd_cable_diag_init(int l1,soc_port_t l2){phy_ctrl_t*l3;uint16 l4;l3 = 
EXT_PHY_SW_STATE(l1,l2);SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xc0)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xc7)&0xff),0x15,(0xa01a)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc8)&0xff),0x15,(0x0100)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc9)&0xff),0x15
,(0x00ef)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcc
)&0xff),0x15,(0x0180)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xce)&0xff),0x15,(0x4000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xcf)&0xff),0x15,(0x4000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0)&0xff),0x15,(0x0a00)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe1)&0xff),0x15
,(0x0c0e)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe2
)&0xff),0x15,(0x0004)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe3)&0xff),0x15,(0x5000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe4)&0xff),0x15,(0x70cc)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe7)&0xff),0x15,(0x00a0)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xef)&0xff),0x15
,(0x409f)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd
)&0xff),0x15,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xcd)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe4)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe7)&0xff),0x15,(0x0000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xef)&0xff),0x15
,(0x8007)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd
)&0xff),0x15,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xcd)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0)&0xff),0x15,(0x00e0)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe1)&0xff),0x15
,(0x0501)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe2
)&0xff),0x15,(0x0c06)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe3)&0xff),0x15,(0x0c06)));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1)
,(l3),0x00,0x0f00|((0xc0)&0xff),0x15,(&l4)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15,(0x4000)));return
SOC_E_NONE;}static int l5(int l1,phy_ctrl_t*l3,uint16 l6[]){
SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15,
(&l6[0])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l3),0x00,0x0f00|((0xc1)&
0xff),0x15,(&l6[1])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l3),0x00,
0x0f00|((0xc2)&0xff),0x15,(&l6[2])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1)
,(l3),0x00,0x0f00|((0xc3)&0xff),0x15,(&l6[3])));SOC_IF_ERROR_RETURN(
phy_reg_ge_read((l1),(l3),0x00,0x0f00|((0xc4)&0xff),0x15,(&l6[4])));
SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l3),0x00,0x0f00|((0xc5)&0xff),0x15,
(&l6[5])));return SOC_E_NONE;}int phy_ecd_cable_diag(int l1,soc_port_t l2,
soc_port_cable_diag_t*l7){phy_ctrl_t*l3;uint16 l8,l9;uint16 l10,l11;uint16 l12
[6];uint16 l13;l3 = EXT_PHY_SW_STATE(l1,l2);l9 = 0;l13 = 0;
SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l3,0x1,&l10));SOC_IF_ERROR_RETURN(
READ_PHY_REG(l1,l3,0x1,&l10));sal_usleep(10000);SOC_IF_ERROR_RETURN(
READ_PHY_REG(l1,l3,0x1,&l11));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),
0x00,0x0f00|((0xc0)&0xff),0x15,(0x0000)));for(l8 = 0;l8<300;l8++){
SOC_IF_ERROR_RETURN(l5(l1,l3,l12));if(l12[0]&0x4){l13 = 1;}if(!(l12[0]&0x0800
)){break;}}if(l8>300){l9|= 0x0002;}if(!(((l10&l11&0x0004)!= 0)&&(l13 == 1))){
l13 = 0;SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc0)&
0xff),0x15,(0x9000)));for(l8 = 0;l8<300;l8++){SOC_IF_ERROR_RETURN(l5(l1,l3,
l12));if(!(l12[0]&0x0800)){break;}}if(l8>300){l9|= 0x0001;}if(((l12[1]&0xf000
) == 0x9000)||((l12[1]&0x0f00) == 0x0900)||((l12[1]&0x00f0) == 0x0090)||((l12
[1]&0x000f) == 0x0009)){SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe7)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x4080)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15,(0x1000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15
,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc0
)&0xff),0x15,(0x9000)));l13 = 0;for(l8 = 0;l8<300;l8++){SOC_IF_ERROR_RETURN(
l5(l1,l3,l12));if(l12[0]&0x4){l13 = 1;}if(!(l12[0]&0x0800)){break;}}if(l8>300
){l9|= 0x0001;}if(!l13){l9|= 0x0008;}SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe7)&0xff),0x15,(0x00a0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x4080)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15
,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd
)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe7)&0xff),0x15,(0x0000)));}}if(l9){return SOC_E_UNAVAIL;}
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15
,(0x4000)));l7->npairs = 4;l7->fuzz_len = 10;switch((l12[1]>>12)&0x000f){case
1:l7->pair_state[0] = SOC_PORT_CABLE_STATE_OK;l7->pair_len[0] = l12[2]/100;
break;case 2:l7->pair_state[0] = SOC_PORT_CABLE_STATE_OPEN;l7->pair_len[0] = 
l12[2]/100;break;case 3:l7->pair_state[0] = SOC_PORT_CABLE_STATE_SHORT;l7->
pair_len[0] = l12[2]/100;break;case 4:l7->pair_state[0] = 
SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[0] = l12[2]/100;break;default:l7
->pair_state[0] = SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[0] = 0;break;}
switch((l12[1]>>8)&0x000f){case 1:l7->pair_state[1] = SOC_PORT_CABLE_STATE_OK
;l7->pair_len[1] = l12[3]/100;break;case 2:l7->pair_state[1] = 
SOC_PORT_CABLE_STATE_OPEN;l7->pair_len[1] = l12[3]/100;break;case 3:l7->
pair_state[1] = SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[1] = l12[3]/100;break
;case 4:l7->pair_state[1] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[1] = 
l12[3]/100;break;default:l7->pair_state[1] = SOC_PORT_CABLE_STATE_UNKNOWN;l7
->pair_len[1] = 0;break;}switch((l12[1]>>4)&0x000f){case 1:l7->pair_state[2] = 
SOC_PORT_CABLE_STATE_OK;l7->pair_len[2] = l12[4]/100;break;case 2:l7->
pair_state[2] = SOC_PORT_CABLE_STATE_OPEN;l7->pair_len[2] = l12[4]/100;break;
case 3:l7->pair_state[2] = SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[2] = l12[4
]/100;break;case 4:l7->pair_state[2] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->
pair_len[2] = l12[4]/100;break;default:l7->pair_state[2] = 
SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[2] = 0;break;}switch((l12[1]>>0)&
0x000f){case 1:l7->pair_state[3] = SOC_PORT_CABLE_STATE_OK;l7->pair_len[3] = 
l12[5]/100;break;case 2:l7->pair_state[3] = SOC_PORT_CABLE_STATE_OPEN;l7->
pair_len[3] = l12[5]/100;break;case 3:l7->pair_state[3] = 
SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[3] = l12[5]/100;break;case 4:l7->
pair_state[3] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[3] = l12[5]/100;
break;default:l7->pair_state[3] = SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[3
] = 0;break;}return SOC_E_NONE;}int phy_ecd_cable_diag_run_40nm(int l1,
soc_port_t l2,soc_port_cable_diag_t*l7,uint16 l14){phy_ctrl_t*l3;uint16 l8,l9
;uint16 l10,l11;uint16 l12[6];uint16 l13,l15 = 0;l3 = EXT_PHY_SW_STATE(l1,l2)
;l9 = 0;l13 = 0;SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((
0xc0)&0xff),0x15,(l14&0x3fff)));SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l3,0x1,&
l10));SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l3,0x1,&l10));sal_usleep(10000);
SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l3,0x1,&l11));SOC_IF_ERROR_RETURN(l5(l1,
l3,l12));if(l12[0]&0x0800){l9|= 0x0002;SOC_IF_ERROR_RETURN(phy_reg_ge_write((
l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15,(l14&0x7fff)));LOG_VERBOSE(
BSL_LS_SOC_PHY,(BSL_META(
"\t->ECD engine still busy during intial flush! (u=%d p=%d) \n"),l1,l2));l13 = 
0;return SOC_E_FAIL;}if((((l10&l11&0x0004)!= 0)&&((l14&0x1000) == 0))){if(l12
[0]&0x4){l13 = 1;}else{l13 = 0;}}else{SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l3
,0x18,0x0C00));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l3,0x17,0x4030));
SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l3,0x15,0x1072));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xFB)&0xff),0x15,(0x7C00)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xFF)&0xff),0x15
,(0x5F0F)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xFF
)&0xff),0x15,(0x5000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xFB)&0xff),0x15,(0x4000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xc0)&0xff),0x15,(l14|0x8000)));l13 = 0;for(l8 = 0;l8<
3000;l8++){SOC_IF_ERROR_RETURN(l5(l1,l3,l12));if(l12[0]&0x4){l13 = 1;}if(!(
l12[0]&0x0800)){l15 = 1;break;}sal_usleep(1000);}if((l8>= 3000)||(l15 == 0)){
l9|= 0x0001;}SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((
0xFB)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),
0x00,0x0f00|((0xFF)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(
l1,l3,0x17,0x4030));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l3,0x15,0x1872));
SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l3,0x18,0x0400));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15,(l14&0x7FFF)));}if(
l13 == 0){l9|= 0x0008;}if(l12[0]&0x8){l9|= 0x0004;}if(l9){if(l9&0x0001){
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->ECD on demand not done - busy time out! (u:%d p:%d) \n"),l1,l2));}if(l9&
0x0008){LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->ECD no fresh result! (u:%d p:%d)\n"),l1,l2));}if(l9&0x0004){LOG_VERBOSE(
BSL_LS_SOC_PHY,(BSL_META("\t->ECD result invalid! (u:%d p:%d)\n"),l1,l2));}}
if((l12[2] == 0x47AE)||(l12[3] == 0x47AE)||(l12[4] == 0x47AE)||(l12[5] == 
0x47AE)){LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->ECD failure type 183! (u:%d p:%d)\n"),l1,l2));}if((l9&0xff)||((l12[2] == 
0x47AE)||(l12[3] == 0x47AE)||(l12[4] == 0x47AE)||(l12[5] == 0x47AE))){
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META("\t->EXP_C[0]: %x\n"),l12[0]));
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META("\t->EXP_C[1]: %x\n"),l12[1]));
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META("\t->EXP_C[2]: %x\n"),l12[2]));
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META("\t->EXP_C[3]: %x\n"),l12[3]));
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META("\t->EXP_C[4]: %x\n"),l12[4]));
LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META("\t->EXP_C[5]: %x\n"),l12[5]));l7->
pair_state[0] = SOC_PORT_CABLE_STATE_UNKNOWN;return SOC_E_FAIL;}l7->npairs = 
4;l7->fuzz_len = 10;l7->state = SOC_PORT_CABLE_STATE_OK;switch((l12[1]>>12)&
0x000f){case 1:l7->pair_state[0] = SOC_PORT_CABLE_STATE_OK;l7->pair_len[0] = 
l12[2]/100;break;case 2:l7->pair_state[0] = SOC_PORT_CABLE_STATE_OPEN;l7->
pair_len[0] = l12[2]/100;break;case 3:l7->pair_state[0] = 
SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[0] = l12[2]/100;break;case 4:l7->
pair_state[0] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[0] = l12[2]/100;
break;case 9:LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->Cabel Diag (40nm) pair A: u=%d p=%d\n, "
"Persistent noise present on MDI\n"),l1,l2));l7->pair_state[0] = 
SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[0] = 0;break;default:l7->pair_state
[0] = SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[0] = 0;break;}switch((l12[1]
>>8)&0x000f){case 1:l7->pair_state[1] = SOC_PORT_CABLE_STATE_OK;l7->pair_len[
1] = l12[3]/100;break;case 2:l7->pair_state[1] = SOC_PORT_CABLE_STATE_OPEN;l7
->pair_len[1] = l12[3]/100;break;case 3:l7->pair_state[1] = 
SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[1] = l12[3]/100;break;case 4:l7->
pair_state[1] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[1] = l12[3]/100;
break;case 9:LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->Cabel Diag (40nm) pair B: u=%d p=%d\n, "
"Persistent noise present on MDI\n"),l1,l2));l7->pair_state[0] = 
SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[0] = 0;break;default:l7->pair_state
[1] = SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[1] = 0;break;}switch((l12[1]
>>4)&0x000f){case 1:l7->pair_state[2] = SOC_PORT_CABLE_STATE_OK;l7->pair_len[
2] = l12[4]/100;break;case 2:l7->pair_state[2] = SOC_PORT_CABLE_STATE_OPEN;l7
->pair_len[2] = l12[4]/100;break;case 3:l7->pair_state[2] = 
SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[2] = l12[4]/100;break;case 4:l7->
pair_state[2] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[2] = l12[4]/100;
break;case 9:LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->Cabel Diag (40nm) pair C: u=%d p=%d\n, "
"Persistent noise present on MDI\n"),l1,l2));l7->pair_state[0] = 
SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[0] = 0;break;default:l7->pair_state
[2] = SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[2] = 0;break;}switch((l12[1]
>>0)&0x000f){case 1:l7->pair_state[3] = SOC_PORT_CABLE_STATE_OK;l7->pair_len[
3] = l12[5]/100;break;case 2:l7->pair_state[3] = SOC_PORT_CABLE_STATE_OPEN;l7
->pair_len[3] = l12[5]/100;break;case 3:l7->pair_state[3] = 
SOC_PORT_CABLE_STATE_SHORT;l7->pair_len[3] = l12[5]/100;break;case 4:l7->
pair_state[3] = SOC_PORT_CABLE_STATE_CROSSTALK;l7->pair_len[3] = l12[5]/100;
break;case 9:LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"\t->Cabel Diag (40nm) pair D: u=%d p=%d\n, "
"Persistent noise present on MDI\n"),l1,l2));l7->pair_state[0] = 
SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[0] = 0;break;default:l7->pair_state
[3] = SOC_PORT_CABLE_STATE_UNKNOWN;l7->pair_len[3] = 0;break;}return
SOC_E_NONE;}int phy_ecd_cable_diag_40nm(int l1,soc_port_t l2,
soc_port_cable_diag_t*l7,uint16 l14){unsigned int l16 = 1;int l17 = 
SOC_E_FAIL;do{LOG_VERBOSE(BSL_LS_SOC_PHY,(BSL_META(
"Cable diag 40nm run (attempt %d)\n"),l16));l17 = phy_ecd_cable_diag_run_40nm
(l1,l2,l7,l14);if(l17 == SOC_E_NONE){break;}}while(l16++<30);return l17;}int
phy_ecd_cable_diag_init_40nm(int l1,soc_port_t l2){phy_ctrl_t*l3;uint16 l4;l3
= EXT_PHY_SW_STATE(l1,l2);SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00
,0x0f00|((0xc0)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((
l1),(l3),0x00,0x0f00|((0xc7)&0xff),0x15,(0xa01a)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc8)&0xff),0x15,(0x0300)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc9)&0xff),0x15
,(0x00ef)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcb
)&0xff),0x15,(0x1304)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xcc)&0xff),0x15,(0x0180)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xce)&0xff),0x15,(0x4000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcf)&0xff),0x15,(0x3000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0)&0xff),0x15
,(0x0119)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe1
)&0xff),0x15,(0x0202)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe2)&0xff),0x15,(0x000F)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe3)&0xff),0x15,(0x5000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe4)&0xff),0x15,(0x738E)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe5)&0xff),0x15
,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe6
)&0xff),0x15,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe7)&0xff),0x15,(0xAA00)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x40ff)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15,(0x1000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15
,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0
)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe1)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe2)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe3)&0xff),0x15,(0x0000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe4)&0xff),0x15
,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe5
)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe6)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe7)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x0000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0)&0xff),0x15
,(0x0b00)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe1
)&0xff),0x15,(0x313d)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe2)&0xff),0x15,(0x007c)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x8007)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15,(0x1000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xcd)&0xff),0x15
,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0
)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe1)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe2)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xef)&0xff),0x15,(0x0000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe0)&0xff),0x15
,(0x0001)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe1
)&0xff),0x15,(0x0540)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xe2)&0xff),0x15,(0x0e40)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xe3)&0xff),0x15,(0x0841)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe4)&0xff),0x15,(0x1344)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe8)&0xff),0x15
,(0x8760)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xe9
)&0xff),0x15,(0x4b33)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,
0x0f00|((0xea)&0xff),0x15,(0x0400)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l3),0x00,0x0f00|((0xCD)&0xff),0x15,(0x00D0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_read((l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15,(&l4)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l3),0x00,0x0f00|((0xc0)&0xff),0x15
,(0x4000)));return SOC_E_NONE;}
