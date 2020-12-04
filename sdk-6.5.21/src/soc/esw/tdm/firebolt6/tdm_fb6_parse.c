/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip based printing and parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_fb6_print_pipe_config
@param:

Parses pipe config from class structs
 */
void
tdm_fb6_print_pipe_config(tdm_mod_t *_tdm)
{
    int port_phy;
    int param_port_start, param_port_end, param_phy_lo, param_phy_hi,
        param_pipe_id;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_port_start= _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_start;
    param_port_end  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_end;
    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pipe_id   = _tdm->_core_data.vars_pkg.pipe;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;
    param_states    = _tdm->_chip_data.soc_pkg.state;

    if (param_port_start >= param_phy_lo && param_port_end <= param_phy_hi){
        TDM_BIG_BAR
        TDM_PRINT3("TDM: Pipe Config: Pipe %d, Port [%d - %d] \n\n",
                   param_pipe_id, param_port_start, param_port_end);
        TDM_PRINT5("%8s%8s%8s%8s%8s\n","port", "pm", "speed", "LR/OS", "EN/HG");
        for (port_phy=param_port_start; port_phy<=param_port_end; port_phy++) {
            if (param_speeds[port_phy]>0) {
                TDM_PRINT1("%8d", port_phy);
                TDM_PRINT1("%8d", tdm_fb6_cmn_get_port_pm(port_phy, _tdm));
                TDM_PRINT1("%8d", (param_speeds[port_phy]/1000));
                if (param_states[port_phy-1] == PORT_STATE__LINERATE ||
                    param_states[port_phy-1] == PORT_STATE__LINERATE_HG) {
                    TDM_PRINT1("%8s", "LR");
                } else {
                    TDM_PRINT1("%8s", "OS");
                }
                if (param_states[port_phy-1] == PORT_STATE__LINERATE_HG ||
                    param_states[port_phy-1] == PORT_STATE__OVERSUB_HG) {
                    TDM_PRINT1("%8s", "HG");
                } else {
                    TDM_PRINT1("%8s", "EN");
                }
                TDM_PRINT0("\n");
            }
        }
        TDM_BIG_BAR
    }
}

/**
@name: tdm_fb6_print_lr_cal
@param:

Print TDM calendar content of current pipe
 */
int
tdm_fb6_print_lr_cal(tdm_mod_t *_tdm, int cal_id)
{
    int j, token;
    int param_phy_lo, param_phy_hi,
        param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_fb6_cmn_get_pipe_cal_len(param_cal_id, _tdm);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            token = param_cal_main[j];
            TDM_PRINT2("TDM: PIPE %d, element #%0d, ", param_cal_id, j);
            if (((token != FB6_OVSB_TOKEN) &&
                 !(token >= param_phy_lo && token <= param_phy_hi))
                ) {
                TDM_PRINT0("contains ACCESSORY TOKEN - ");
                switch (token) {
                    case FB6_CMIC_TOKEN: TDM_PRINT0("CMIC/CPU\n"); break;
                    case FB6_MGM1_TOKEN: TDM_PRINT0("MANAGEMENT-1\n"); break;
                    case FB6_MGM2_TOKEN: TDM_PRINT0("MANAGEMENT-2\n"); break;
                    case FB6_MGM3_TOKEN: TDM_PRINT0("MANAGEMENT-3\n"); break;
                    case FB6_MGM4_TOKEN: TDM_PRINT0("MANAGEMENT-4\n"); break;
                    case FB6_FAE_TOKEN: TDM_PRINT0("FAE-1\n"); break;
                    case FB6_LPB0_TOKEN: TDM_PRINT0("LOOPBACK-0\n"); break;
                    case FB6_IDL1_TOKEN: TDM_PRINT0("OPPORTUNISTIC-1\n"); break;
                    case FB6_NULL_TOKEN: TDM_PRINT0("NULL\n"); break;
                    case FB6_IDL2_TOKEN: TDM_PRINT0("IDLE\n"); break;
                    default: TDM_PRINT1("UNKNOWN,%d\n",token); break;
                }
            } else {
                TDM_PRINT1("contains physical port #%0d\n", token);
            }
        }
    }

    return PASS;
}

/**
@name: tdm_fb6_print_pipe_cal
@param:

Print TDM calendar content of current pipe
 */
void
tdm_fb6_print_pipe_cal(tdm_mod_t *_tdm)
{
    int i, j, n;
    int param_pipe_id, param_cal_id, param_num_phy_pm,
        param_grp_num=0, param_grp_len=0;
    int **param_cal_grp=NULL;
    tdm_calendar_t *param_cal=NULL;

    param_pipe_id    = _tdm->_core_data.vars_pkg.pipe;
    param_cal_id     = _tdm->_core_data.vars_pkg.cal_id;
    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;

    switch(param_cal_id) {
        case 0: param_cal = (&(_tdm->_chip_data.cal_0)); break;
        case 1: param_cal = (&(_tdm->_chip_data.cal_1)); break;
/*      case 2: param_cal = (&(_tdm->_chip_data.cal_2)); break;
        case 3: param_cal = (&(_tdm->_chip_data.cal_3)); break; */
        default:
            break;;
    }

    if (param_cal != NULL) {
        param_grp_num = param_cal->grp_num;
        param_grp_len = param_cal->grp_len;
        param_cal_grp = param_cal->cal_grp;
        if (param_grp_num > 0 && param_grp_len > 0 && param_cal_grp != NULL) {
            /* OVSB groups: 1-dimension */
            for (i=0; i<param_grp_num; i++) {
                for (j=0; j<param_grp_len; j++) {
                    TDM_PRINT4("TDM: PIPE %d, OVSB GROUP %2d, element #%03d, port #%0d\n",
                               param_cal_id, i, j, param_cal_grp[i][j]);
                }
            }
            TDM_SML_BAR
            /* OVSB groups: 2-dimension */
            TDM_PRINT1("\nTDM: PIPE %d, OVSB GROUPs 0:11\n", param_pipe_id);
            for (i=0; i< param_grp_num; i++) {
                n = param_grp_num/2;
                if (n>0) {
                    if ((i%n)==0) {
                        TDM_PRINT2("\n%12s%2d\t", "HALF PIPE", (i/n));
                        for (j=0; j<param_grp_len; j++) {
                            TDM_PRINT1("%3d\t", j+1);
                        }
                        TDM_PRINT0("\n");
                    }
                }
                TDM_PRINT1("OVSB_GROUP[%2d]:\t", i);
                for (j=0; j< param_grp_len; j++) {
                    TDM_PRINT1("%3d\t", param_cal_grp[i][j]);
                }
                TDM_PRINT0("\n");
            }
            TDM_SML_BAR
            /* Packet Scheduler Calendar */
            TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 0\n",param_pipe_id);
            /*for (j=0; j<FB6_SHAPING_GRP_LEN; j++) {*/
            for (j=0; j<320; j++) {
                if (j%16==0) {
                    TDM_PRINT2("\n %3d : %3d\t", j, j+15);
                }
                TDM_PRINT1("\t%d", param_cal_grp[FB6_SHAPING_GRP_IDX_0][j]);
            }
            TDM_PRINT0("\n\n");
            TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 1\n",param_pipe_id);
            /*for (j=0; j<FB6_SHAPING_GRP_LEN; j++) {*/
            for (j=0; j<320; j++) {
                if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
                TDM_PRINT1("\t%d",param_cal_grp[FB6_SHAPING_GRP_IDX_1][j]);
            }
            TDM_PRINT0("\n");
            TDM_SML_BAR
        }
        /* PM_number to Port_Block_number mapping table */
        TDM_PRINT1("\nTDM: PIPE %d, PM_NUM to PBLK_ID MAP \nPM:PBLK_ID",param_pipe_id);
        for (j=(16*param_cal_id); j<(16*(param_cal_id+1)) && j<param_num_phy_pm; j++) {
            TDM_PRINT2("\t %2d:%2d",j, _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[j]);
        }
        TDM_PRINT0("\n");
        TDM_SML_BAR
    }
}

/**
@name: tdm_fb6_parse_mgmt
@param:

Parse MGMT/CMIC slots in IDB TDM calendar
 */
/*
int
tdm_fb6_parse_mgmt(tdm_mod_t *_tdm, int cal_id)
{
    int cnt = 0, result = PASS;
    int ave_space, idx = 0, lo, hi, idx_lo, idx_hi, found = BOOL_TRUE;
    int mgmt_2nd_en, mgmt_slot_num;
    int param_mgmt_mode, param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_mgmt_mode = _tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode;
    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_fb6_cmn_get_pipe_cal_len(param_cal_id, _tdm);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    if (param_mgmt_mode == FB6_MGMT_MODE_2_PORT_EN ||
        param_mgmt_mode == FB6_MGMT_MODE_2_PORT_HG) {
        mgmt_2nd_en = 1;
        mgmt_slot_num = 8;
    } else {
        mgmt_2nd_en = 0;
        mgmt_slot_num = 4;
    }

    if (mgmt_slot_num > 0) {
        ave_space = param_cal_len / mgmt_slot_num;
    }

    if (param_cal_main != NULL && param_cal_len > 0) {
        while (idx < param_cal_len &&
               param_cal_main[idx] != param_token_ancl) {
            idx++;
        }

        while (cnt < mgmt_slot_num && found == BOOL_TRUE) {
            found = BOOL_FALSE;
            if (param_cal_main[idx] == param_token_ancl) {
                found = BOOL_TRUE;
            } else {
                lo = 0;
                hi = 0;
                idx_lo = idx;
                idx_hi = idx;
                do {
                    lo++;
                    hi++;
                    idx_lo = (param_cal_len + idx_lo - 1) % param_cal_len;
                    idx_hi = (idx_hi + 1) % param_cal_len;
                } while ((lo + hi) < param_cal_len &&
                         param_cal_main[idx_lo] != param_token_ancl &&
                         param_cal_main[idx_hi] != param_token_ancl);
                if (param_cal_main[idx_lo] == param_token_ancl) {
                    idx = idx_lo;
                    found = BOOL_TRUE;
                } else if (param_cal_main[idx_hi] == param_token_ancl) {
                    idx = idx_hi;
                    found = BOOL_TRUE;
                }
            }
            if (found == BOOL_TRUE) {
                switch (param_cal_id) {
                    case 0:
                    case 4:
                        param_cal_main[idx] = FB6_CMIC_TOKEN;
                        break;
                    case 1:
                    case 5:
                        if (mgmt_2nd_en) {
                            param_cal_main[idx] = (cnt % 2 == 0) ?
                                                  FB6_MGM1_TOKEN :
                                                  FB6_MGM2_TOKEN;
                        } else {
                            param_cal_main[idx] = FB6_MGM1_TOKEN;
                        }
                        break;
                    default :
                        break;
                }
                TDM_PRINT2("Allocate %0d-th MGMT/CMIC slot at index #%03d\n",
                          cnt+1, idx);
                cnt++;
                idx = (idx + ave_space) % param_cal_len;
                if (cnt >= mgmt_slot_num) {
                    break;
                }
            }
        }


        if (cnt < mgmt_slot_num) {
            result = FAIL;
            TDM_ERROR4("%s, cal_id %0d, req_slot %0d, alloc_slot %0d\n",
                       "Failed in MGMT/CMIC slot allocation",
                       param_cal_id, mgmt_slot_num, cnt);
        }
    }

    return (result);
}
*/
/**
@name: tdm_fb6_parse_acc_idb
@param:

Parse ACCESSORY slots (LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_fb6_parse_acc_idb(tdm_mod_t *_tdm, int no_of_mgmt_ports, int cal_id)
{
    int param_token_ancl, param_cal_id, param_cal_len;

    int param_anc_size;
    int param_clk_freq;
    int param_mgmt_size;
    int param_lpbk;
    int param_cpu = 2;
    int param_fae;
   int  param_idle1 = 1;
   int param_null = 8;
int param_dummy = 0;
    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_fb6_cmn_get_pipe_cal_len(param_cal_id, _tdm);

    param_clk_freq =  _tdm->_chip_data.soc_pkg.clk_freq;

    TDM_PRINT1("Before param_clk_freq %0d\n", param_clk_freq);

    switch (param_clk_freq) {
       case FB6_CLK_4730_1012: param_mgmt_size = 8;  param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 9;  break;
       case FB6_CLK_4731_1012: param_mgmt_size = 0; param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 0; break;
       case FB6_CLK_4732_1012: param_mgmt_size = 0;  param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 0;  break;
       case FB6_CLK_4720_1250: param_mgmt_size = 8;  param_lpbk=2; param_fae = 2; param_null = 8; param_dummy = 2;  break;
       case FB6_CLK_4721_1250: param_mgmt_size = 16;  param_lpbk=2; param_fae = 2; param_null = 8;param_dummy = 2;  break;
       case FB6_CLK_4700_1250: param_mgmt_size = 16; param_lpbk=2; param_fae = 2; param_null = 8; param_dummy = 2;  break;
       case FB6_CLK_4701_1250: param_mgmt_size = 8;  param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 4; break;
       case FB6_CLK_4702_1250: param_mgmt_size = 8;  param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 1;  break;
       case FB6_CLK_4703_1250: param_mgmt_size = 4;  param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 0;  break;
       case FB6_CLK_4704_1087: param_mgmt_size = 16; param_lpbk=2; param_fae = 2; param_null = 8; param_dummy = 2;  break;
       case FB6_CLK_4705_1250: param_mgmt_size = 16; param_lpbk=2; param_fae = 2; param_null = 8; param_dummy = 2;  break;
       case FB6_CLK_1000_1012: param_mgmt_size = 16;  param_lpbk=4; param_fae = 4; param_null = 8;param_dummy = 5;  break;
       case FB6_CLK_1001_1250: param_mgmt_size = 16; param_lpbk=3; param_fae = 3; param_null = 8; param_dummy = 1;  break;
       default: param_mgmt_size = 20; param_fae = 0; param_lpbk=0; break;
    }

    param_anc_size = param_mgmt_size+param_lpbk+param_fae+param_cpu+1+param_null;
    TDM_PRINT4("After param_ancl_size %0d param_mgmt_size %0d param_clk_freq %0d cal size %d\n", param_anc_size, param_mgmt_size, param_clk_freq, param_cal_len);


    tdm_fb6_parse_tdm_cal_idb_mmu(_tdm, cal_id, param_mgmt_size, param_lpbk, param_fae, param_idle1, param_null, param_clk_freq, param_cal_len, param_token_ancl, param_dummy, no_of_mgmt_ports);
    return PASS;
}

/**
@name: tdm_fb6_parse_acc_mmu
@param:

Parse ACCESSORY slots (LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_fb6_parse_acc_mmu(tdm_mod_t *_tdm, int no_of_mgmt_ports, int cal_id)
{
    int param_token_ancl, param_cal_id, param_cal_len;

    int param_anc_size;
    int param_clk_freq;
    int param_mgmt_size;
    int param_lpbk;
    int param_cpu = 2;
    int param_fae;
   int  param_idle1 = 0;
   int param_null = 9;
int param_dummy = 0;
    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_fb6_cmn_get_pipe_cal_len(param_cal_id, _tdm);

    param_clk_freq =  _tdm->_chip_data.soc_pkg.clk_freq;

    TDM_PRINT1("MMU PARSE: Before  param_clk_freq %0d\n", param_clk_freq);

   switch (param_clk_freq) {
       case FB6_CLK_4730_1012: param_mgmt_size = 12; param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 8; break;
       case FB6_CLK_4731_1012: param_mgmt_size = 0; param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 3;  break;
       case FB6_CLK_4732_1012: param_mgmt_size = 0; param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 3; break;
       case FB6_CLK_4720_1250: param_mgmt_size = 12; param_lpbk=2; param_fae = 0; param_null = 9; param_dummy = 0;   break;
       case FB6_CLK_4721_1250: param_mgmt_size = 20;  param_lpbk=2; param_fae = 0; param_null = 9;param_dummy = 0;  break;
       case FB6_CLK_4700_1250: param_mgmt_size = 20; param_lpbk=2; param_fae = 0; param_null = 9; param_dummy = 0; break;
       case FB6_CLK_4701_1250: param_mgmt_size = 8;  param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 7; break;
       case FB6_CLK_4702_1250: param_mgmt_size = 12; param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 0; break;
       case FB6_CLK_4703_1250: param_mgmt_size = 6; param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 1; break;
       case FB6_CLK_4704_1087: param_mgmt_size = 20; param_lpbk=2; param_fae = 0; param_null = 9; param_dummy = 0; break;
       case FB6_CLK_4705_1250: param_mgmt_size = 20; param_lpbk=2; param_fae = 0; param_null = 9; param_dummy = 0; break;
       case FB6_CLK_1000_1012: param_mgmt_size = 16; param_lpbk=4; param_fae = 0; param_null = 9; param_dummy = 9; break;
       case FB6_CLK_1001_1250: param_mgmt_size = 20; param_lpbk=3; param_fae = 0; param_null = 9; param_dummy = 0; break;
       default: param_mgmt_size=0; param_fae = 0; param_lpbk=0; param_dummy=0; break;
    }
    param_anc_size = param_mgmt_size+param_lpbk+param_fae+param_cpu+param_null;
    TDM_PRINT3("MMU PARSE: After param_ancl_size %0d param_mgmt_size %0d param_clk_freq %0d\n", param_anc_size, param_mgmt_size, param_clk_freq);

    tdm_fb6_parse_tdm_cal_idb_mmu(_tdm, cal_id, param_mgmt_size, param_lpbk, param_fae, param_idle1, param_null, param_clk_freq, param_cal_len, param_token_ancl, param_dummy, no_of_mgmt_ports);
    return PASS;
}

/**
@name: tdm_fb6_parse_ancl
@param:

Parse ANCILLARY slots (MGMT/CMIC, LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_fb6_parse_ancl(tdm_mod_t *_tdm, int cal_id)
{
    int idb_en = 0, mmu_en = 0, result = PASS;
   static int no_of_mgmt_ports = 0;
   static int param_mgmt_mode[FB6_NUM_EXT_PORTS];
   static int mgmt_init = 0;
   int param_clk_freq;
   int exp_no_of_mgmt_ports=0;
   int j;

    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;

    switch (cal_id) {
        case 0: case 1: idb_en = 1; break;
        case 4: case 5: mmu_en = 1; break;
        default:
            TDM_ERROR1("Invalid calendar ID - %0d\n", cal_id);
            return FAIL;
            break;
    }

    /* MGMT/CMIC */
 /*   if (tdm_fb6_parse_mgmt(_tdm, cal_id) != PASS) { */
 /*       TDM_ERROR1("Failed in MGMT/CMIC allocation, cal_id %0d\n", cal_id); */
 /*       result = FAIL; */
 /*   } */



    if(mgmt_init == 0) {
    for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      param_mgmt_mode[j] = _tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode[j];
    }

    for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      if(param_mgmt_mode[j] == 1) {
       no_of_mgmt_ports++;
      }
    }
    mgmt_init = 1;
    }

     switch(param_clk_freq){
        case     FB6_CLK_4700_1250:
        case     FB6_CLK_4704_1087:
        case     FB6_CLK_4705_1250:
        case     FB6_CLK_4720_1250:
        case     FB6_CLK_4721_1250:
        case     FB6_CLK_4730_1012: exp_no_of_mgmt_ports = 4;
        break;
        case     FB6_CLK_4701_1250:
        case     FB6_CLK_4702_1250:
        case     FB6_CLK_4703_1250: exp_no_of_mgmt_ports = 2;
        break;
        case     FB6_CLK_4731_1012:
        case     FB6_CLK_4732_1012: exp_no_of_mgmt_ports = 0;
        break;
        case     FB6_CLK_1000_1012:
        case     FB6_CLK_1001_1250: exp_no_of_mgmt_ports = 4;
        break;
        default : exp_no_of_mgmt_ports = 0;
        break;
     }

     if(exp_no_of_mgmt_ports == no_of_mgmt_ports) {
       TDM_PRINT2(" No of Mgmt Ports Matching Expected %d Actual %d \n", exp_no_of_mgmt_ports, no_of_mgmt_ports);
     }
     else {
       TDM_ERROR2(" No of Mgmt Ports Mismatch Expected %d Actual %d \n", exp_no_of_mgmt_ports, no_of_mgmt_ports);
       return FAIL;
     }


    /* LOOPBACK, OPP1, OPP2, NULL, IDLE */
    if (idb_en == 1) {
        if (tdm_fb6_parse_acc_idb(_tdm, no_of_mgmt_ports, cal_id) != PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = FAIL;
        }
    } else if (mmu_en == 1) {
        if (tdm_fb6_parse_acc_mmu(_tdm, no_of_mgmt_ports, cal_id) != PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = FAIL;
        }
    }

    return result;
}

/**
@name: tdm_fb6_parse_tdm_cal_idb_mmu
@param:

process TDM calendar for ancillary tokenized entries
 */
int tdm_fb6_parse_tdm_cal_idb_mmu(tdm_mod_t *_tdm, int cal_id, int param_mgmt_size, int param_lpbk, int param_fae, int param_idle1, int param_null, int param_clk_freq, int param_cal_len, int param_token_ancl, int param_dummy, int no_of_mgmt_ports)
{
    int param_cal_id;
    int token_cnt;
    int j, first_flag, prev_val_mgmt, prev_val_mgmt1, prev_val_mgmt2, prev_val_mgmt3, prev_val_mgmt4, prev_val_cpu, prev_val_lpbk, mgmt1_en, mgmt2_en, mgmt3_en, mgmt4_en, skip_lpbk;

    /*Credits*/
    int mgmt1_credits, mgmt2_credits, mgmt3_credits, mgmt4_credits;
    int lpbk_credits, fae_credits, cpu_credits, idle1_credits, null_credits, dummy_credits;
    int prog_done;
    int mgmt_same_spacing;
    int mgm1_token, mgm2_token, mgm3_token, mgm4_token;
    int shuffle_cnt;

    int *param_cal_main=NULL;

    static int param_mgmt_mode[FB6_NUM_EXT_PORTS];
    static int init_mgmt=0;
    int param_mgmt_pm[4];

    if(init_mgmt==0) {
    for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      param_mgmt_mode[j] = _tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode[j];
    }
     init_mgmt = 1;
    }
    for(j=0; j<4; j++) {
       param_mgmt_pm[j] = 0;
    }
    for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      if(param_mgmt_mode[j] == 1) {
        param_mgmt_pm[j%4] = 1;
      }
    }

    /*TDM_PRINT3("IDB_MMU::j %d mgmt_mode %d mgmt_pm %d\n",  j, param_mgmt_mode[j], param_mgmt_pm[j%4]);*/
    TDM_PRINT1("IDB_MMU::no_of_mgmt_ports %d\n",  no_of_mgmt_ports);
    param_cal_id = cal_id;
    TDM_PRINT1("IDB_MMU::param_mgmt_size %d \n",  param_mgmt_size);
    if(param_mgmt_size==0) token_cnt=4;
    else token_cnt = 0;

    prog_done = 0;
    prev_val_cpu   = 0;
    shuffle_cnt    = 0;
    first_flag     = 1;
    prev_val_mgmt       = 0;
    prev_val_mgmt1       = 0;
    prev_val_mgmt2       = 0;
    prev_val_mgmt3       = 0;
    prev_val_mgmt4       = 0;
    prev_val_lpbk        = 0;
    mgmt1_en       = 0;
    mgmt2_en       = 0;
    mgmt3_en       = 0;
    mgmt4_en       = 0;
    skip_lpbk      = 0;
    lpbk_credits  = param_lpbk;
    fae_credits   = param_fae;
    idle1_credits = param_idle1;
    cpu_credits   = 2;
    null_credits  = param_null;
    dummy_credits  = param_dummy;
    mgmt1_credits = param_mgmt_size/4;
    mgmt2_credits = param_mgmt_size/4;
    mgmt3_credits = param_mgmt_size/4;
    mgmt4_credits = param_mgmt_size/4;

    if(no_of_mgmt_ports == 0 && !((param_clk_freq == FB6_CLK_4731_1012) || (param_clk_freq == FB6_CLK_4732_1012))) {
      TDM_ERROR1("Invalid No of Mgmt Ports - %0d\n", no_of_mgmt_ports);
      return FAIL;
    }
    else if((param_clk_freq == FB6_CLK_4731_1012) || (param_clk_freq == FB6_CLK_4732_1012)) {
      mgmt1_credits = 0;
      mgmt2_credits = 0;
      mgmt3_credits = 0;
      mgmt4_credits = 0;
    }
    else {
      mgmt1_credits = (param_mgmt_size * param_mgmt_pm[0])/no_of_mgmt_ports;
      mgmt2_credits = (param_mgmt_size * param_mgmt_pm[1])/no_of_mgmt_ports;
      mgmt3_credits = (param_mgmt_size * param_mgmt_pm[2])/no_of_mgmt_ports;
      mgmt4_credits = (param_mgmt_size * param_mgmt_pm[3])/no_of_mgmt_ports;
    }

    TDM_PRINT4("IDB_MMU::mgmt1_credits %d mgmt2_credits %d mgmt3_credits %d mgmt4_credits %d\n",  mgmt1_credits, mgmt2_credits, mgmt3_credits, mgmt4_credits);
/*    switch (param_clk_freq) {
       case FB6_CLK_4701_1250: mgmt1_credits = param_mgmt_size/2; mgmt3_credits=param_mgmt_size/2; mgmt2_credits= 0; mgmt4_credits=0; break;
       case FB6_CLK_4702_1250: mgmt1_credits = param_mgmt_size/2; mgmt3_credits=param_mgmt_size/2; mgmt2_credits= 0; mgmt4_credits=0; break;
       case FB6_CLK_4703_1250: mgmt1_credits = param_mgmt_size/2; mgmt3_credits=param_mgmt_size/2; mgmt2_credits= 0; mgmt4_credits=0; break;
       case FB6_CLK_4731_1012: mgmt1_credits = param_mgmt_size/2; mgmt3_credits=param_mgmt_size/2; mgmt2_credits= 0; mgmt4_credits=0;
       case FB6_CLK_4732_1012: mgmt1_credits = param_mgmt_size/2; mgmt3_credits=param_mgmt_size/2; mgmt2_credits= 0; mgmt4_credits=0;
                               break;
      default:  break;

    }*/
    TDM_PRINT1("IDB_MMU::After mgmt1_credits %d \n",  mgmt1_credits);

    tdm_fb6_cmn_get_mgmt_tokens(_tdm, &mgm1_token, &mgm2_token, &mgm3_token, &mgm4_token);
    TDM_PRINT4("IDB_MMU:: mgm1_token %d mgm2_token %d mgm3_token %d mgm4_token %d\n",  mgm1_token, mgm2_token, mgm3_token, mgm4_token);
    /*if(param_clk_freq == FB6_CLK_4702_1250) {
       if(param_idle1 == 1) {
         mgmt_same_spacing = 65;
       }
       else {
         mgmt_same_spacing = 45;
       }
    }
    else {
      mgmt_same_spacing = 12;
    }*/

    if((param_clk_freq == FB6_CLK_4731_1012) || (param_clk_freq == FB6_CLK_4732_1012)) {
      mgmt_same_spacing = 12;
    }
    else {
      mgmt_same_spacing = (param_cal_len/mgmt1_credits*80)/100;
    }
    TDM_PRINT1("IDB_MMU::mgmt_same_spacing %d \n",  mgmt_same_spacing);
    TDM_SEL_CAL(param_cal_id, param_cal_main);
    for (j=0; j<param_cal_len; j++) {
        if(j%20==0) TDM_PRINT1("[%d]\n", j);
        TDM_PRINT1("%d   ",  param_cal_main[j]);
    }
    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; ) {
            if (param_cal_main[j] == param_token_ancl) {
        TDM_PRINT1("CALENDER  %d ",  param_cal_id);
        TDM_PRINT4("Credits: j %d token_cnt %d mgmt1 %d lpbk %d ",  j, token_cnt, mgmt1_credits, lpbk_credits);
        TDM_PRINT4("fae %d cpu %d idle1 %d null %d \n", fae_credits, cpu_credits, idle1_credits, null_credits);
        switch(token_cnt) {
            case 0 :
                    TDM_PRINT4("Case M1 mgmt1_en %0d mgmt1_credits %d skip_lpbk %d prev_val_mgmt %d \n", mgmt1_en, mgmt1_credits, skip_lpbk, prev_val_mgmt);
                     if(mgmt1_credits>0) {
                       if(first_flag) {
                         param_cal_main[j] = mgm1_token;
                         TDM_PRINT3("cnt %d j %d MGM1 TOKEN %d \n", shuffle_cnt, j, mgm1_token);
                         shuffle_cnt++;
                         prev_val_mgmt1 = j;
                         mgmt1_credits --;
                         token_cnt++;
                         first_flag=0;
                         j++;
                         prog_done=1;
                         break;
                       }
                       else if((j-prev_val_mgmt1 < mgmt_same_spacing) || (j-prev_val_mgmt2 <4) || (j-prev_val_mgmt3<4) || (j-prev_val_mgmt4<4)) {
                          token_cnt=4;
                          mgmt1_en=1;
                       }
                       else {
                          if((j-prev_val_mgmt1 < mgmt_same_spacing) || (j-prev_val_mgmt2 <4) || (j-prev_val_mgmt3<4) || (j-prev_val_mgmt4<4)) {
                            token_cnt=4;
                            mgmt1_en=1;
                          }
                          else {
                          param_cal_main[j] = mgm1_token;
                         TDM_PRINT3("cnt %d j %d MGM1 TOKEN %d \n", shuffle_cnt, j, mgm1_token);
                         shuffle_cnt++;
                           mgmt1_credits --;
                           prev_val_mgmt1 = j;
                           token_cnt++;
                           j++;
                         prog_done=1;
                           break;
                        }
                      }
                      }
                      else {
                        token_cnt++;
                      }
            case 1 : TDM_PRINT4("Case M2 mgmt2_en %0d mgmt2_credits %d skip_lpbk %d prev_val_mgmt %d\n", mgmt2_en, mgmt2_credits, skip_lpbk, prev_val_mgmt);
                     if(!mgmt1_en) {
                     if(mgmt2_credits>0) {
                          if(((prev_val_mgmt2>0) && (j-prev_val_mgmt2 < mgmt_same_spacing)) || (j-prev_val_mgmt1 <4) || (j-prev_val_mgmt3<4) || (j-prev_val_mgmt4<4)) {
                          token_cnt=4;
                          if(mgmt1_en==0)
                          mgmt2_en=1;
                        }
                        else {
                       param_cal_main[j] = mgm2_token;
                         TDM_PRINT3("cnt %d j %d MGM2 TOKEN %d \n", shuffle_cnt, j, mgm2_token);
                         shuffle_cnt++;
                        mgmt2_credits --;
                        prev_val_mgmt2 = j;
                        token_cnt++;
                         j++;
                         prog_done=1;
                        break;
                        }
                      }
                      else {
                        token_cnt++;
                      }
                      }
            case 2 : TDM_PRINT4("Case M3 mgmt3_en %0d mgmt3_credits %d skip_lpbk %d prev_val_mgmt %d\n", mgmt3_en, mgmt3_credits, skip_lpbk, prev_val_mgmt);
                     if(!mgmt2_en) {
                     if(mgmt3_credits>0) {
                      if(((prev_val_mgmt3>0) && (j-prev_val_mgmt3 < mgmt_same_spacing)) || (j-prev_val_mgmt1 <4) || (j-prev_val_mgmt2<4) || (j-prev_val_mgmt4<4)) {
                        token_cnt=4;
                        if(mgmt1_en==0 &&  mgmt2_en ==0)
                        mgmt3_en=1;
                      }
                      else {
                     param_cal_main[j] = mgm3_token;
                         TDM_PRINT3("cnt %d j %d MGM3 TOKEN %d \n", shuffle_cnt, j, mgm3_token);
                         shuffle_cnt++;
                      mgmt3_credits --;
                      prev_val_mgmt3 = j;
                      token_cnt++;
                         j++;
                         prog_done=1;
                      break;
                      }
                      }
                      else {
                        token_cnt++;
                      }
                      }
            case 3 : TDM_PRINT4("Case M4 mgmt4_en %0d mgmt4_credits %d skip_lpbk %d prev_val_mgmt %d \n", mgmt4_en, mgmt4_credits, skip_lpbk, prev_val_mgmt);
                     if(!mgmt3_en) {
                     if(mgmt4_credits>0) {
                      if(((prev_val_mgmt4>0) && (j-prev_val_mgmt4 < mgmt_same_spacing)) || (j-prev_val_mgmt1 <4) || (j-prev_val_mgmt2<4) || (j-prev_val_mgmt3<4)) {
                        token_cnt=4;
                        if(mgmt1_en==0 &&  mgmt2_en ==0 && mgmt3_en == 0)
                        mgmt4_en=1;
                      }
                      else {
                     param_cal_main[j] = mgm4_token;
                         TDM_PRINT3("cnt %d j %d MGM4 TOKEN %d \n", shuffle_cnt, j, mgm4_token);
                         shuffle_cnt++;
                      mgmt4_credits --;
                      prev_val_mgmt4 = j;
                      token_cnt++;
                         j++;
                         prog_done=1;
                      break;
                      }
                      }
                      else {
                        token_cnt++;
                      }
                      }

            case 4: TDM_PRINT0("Case LPBK\n");
                    if(lpbk_credits > 0) {
                      if(skip_lpbk) {
                        token_cnt++;
                      }
                      else {
                        if(j-prev_val_lpbk>7) {
                          lpbk_credits --;
                          param_cal_main[j] = FB6_LPB0_TOKEN;
                               TDM_PRINT3("cnt %d j %d LPBK TOKEN %d \n", shuffle_cnt, j, FB6_LPB0_TOKEN);
                               shuffle_cnt++;
                          prev_val_lpbk = j;
                         j++;
                         prog_done=1;

                          if(mgmt1_en) {
                              token_cnt=0;
                              if(lpbk_credits > 0) {
                              skip_lpbk=1;
                              }
                              mgmt1_en =0;
                              break;
                          }
                          else if(mgmt2_en) {
                              token_cnt=1;
                              if(lpbk_credits > 0) {
                              skip_lpbk=1;
                              }
                              mgmt2_en =0;
                              break;
                          }
                          else if(mgmt3_en) {
                              token_cnt=2;
                              if(lpbk_credits > 0) {
                              skip_lpbk=1;
                              }
                              mgmt3_en =0;
                              break;
                          }
                          else if(mgmt4_en) {
                              token_cnt=3;
                              if(lpbk_credits > 0) {
                              skip_lpbk=1;
                              }
                              mgmt4_en =0;
                              break;
                          }
                          else {

                          if(cpu_credits > 0) token_cnt++;
                          else if(fae_credits>0) token_cnt = 6;
                          else   token_cnt=8;
                          break;
                          }
                        }
                        else{
                         token_cnt++;
                        }
                      }
                   }
                    else {
                      token_cnt++;
                    }
            case 5: TDM_PRINT0("Case CPU\n");
                    if(cpu_credits > 0) {
                    if(skip_lpbk) {
                      if(cpu_credits>0) {
                        if(j-prev_val_cpu>14) {
                          param_cal_main[j] = FB6_CMIC_TOKEN;
                           TDM_PRINT3("cnt %d j %d CPU TOKEN %d \n", shuffle_cnt, j, FB6_CMIC_TOKEN);
                           shuffle_cnt++;
                          skip_lpbk=0;
                          prev_val_cpu = j;
                          token_cnt++;
                          cpu_credits --;
                         j++;
                         prog_done=1;
                          break;
                        }
                      }
                      else {
                        token_cnt++;
                      }
                     }
                     else {
                      if(cpu_credits>0) {
                        if(j-prev_val_cpu>14) {
                           param_cal_main[j] = FB6_CMIC_TOKEN;
                           TDM_PRINT3("cnt %d j %d CPU TOKEN %d \n", shuffle_cnt, j, FB6_CMIC_TOKEN);
                           shuffle_cnt++;
                           prev_val_cpu = j;
                           token_cnt++;
                           cpu_credits --;
                         j++;
                         prog_done=1;
                           break;
                        }
                      }
                      else {
                        token_cnt++;
                      }
                     }
                     }
                     else {
                       token_cnt++;
                     }
            case 6: TDM_PRINT0("Case FAE\n");
                   if(fae_credits>0) {
                    if(skip_lpbk) {
                   if(fae_credits>0) {
                       param_cal_main[j] = FB6_FAE_TOKEN;
                         TDM_PRINT3("cnt %d j %d FAE TOKEN %d \n", shuffle_cnt, j, FB6_FAE_TOKEN);
                         shuffle_cnt++;
                        skip_lpbk=0;
                        token_cnt++;
                        fae_credits --;
                         j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        token_cnt++;
                      }
                     }
                     else {
                      if(fae_credits>0) {
                       param_cal_main[j] = FB6_FAE_TOKEN;
                         TDM_PRINT3("cnt %d j %d FAE TOKEN %d \n", shuffle_cnt, j, FB6_FAE_TOKEN);
                         shuffle_cnt++;
                        token_cnt++;
                        fae_credits --;
                         j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        token_cnt++;
                      }
                     }
                     }
                     else {
                      token_cnt++;
                     }
            case 7: TDM_PRINT0("Case Idle1\n");
                   if(idle1_credits > 0) {
                    if(skip_lpbk) {
                   if(idle1_credits>0) {
                       param_cal_main[j] = FB6_IDL1_TOKEN;
                         TDM_PRINT3("cnt %d j %d IDLE1 TOKEN %d \n", shuffle_cnt, j, FB6_IDL1_TOKEN);
                         shuffle_cnt++;
                        skip_lpbk=0;
                        token_cnt++;
                        idle1_credits --;
                         prog_done=1;
                         j++;
                        break;
                      }
                      else {
                        token_cnt++;
                      }
                     }
                     else {
                      if(idle1_credits>0) {
                       param_cal_main[j] = FB6_IDL1_TOKEN;
                         TDM_PRINT3("cnt %d j %d IDLE1 TOKEN %d \n", shuffle_cnt, j, FB6_IDL1_TOKEN);
                         shuffle_cnt++;
                        token_cnt++;
                        idle1_credits --;
                         j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        token_cnt++;
                      }
                     }
                     }
                     else {
                       token_cnt++;
                     }
            case 8: TDM_PRINT0("Case Null\n");

                    if(skip_lpbk) {
                   if(null_credits>0) {
                       param_cal_main[j] = FB6_NULL_TOKEN;
                         TDM_PRINT3("cnt %d j %d NULL TOKEN %d \n", shuffle_cnt, j, FB6_NULL_TOKEN);
                        shuffle_cnt++;
                        skip_lpbk=0;
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(idle1_credits>0)token_cnt=8;
                        null_credits --;
                         j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(idle1_credits>0)token_cnt=8;
                        break;
                      }
                     }
                     else {
                      if(null_credits>0) {
                       param_cal_main[j] = FB6_NULL_TOKEN;
                         TDM_PRINT3("cnt %d j %d NULL TOKEN %d \n", shuffle_cnt, j, FB6_NULL_TOKEN);
                        shuffle_cnt++;
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(idle1_credits>0)token_cnt=8;
                        null_credits --;
                        j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(idle1_credits>0)token_cnt=8;
                      }
                     }
            case 9: TDM_PRINT0("Case Dummy\n");

                    if(skip_lpbk) {
                   if(dummy_credits>0) {
                       param_cal_main[j] = FB6_DUMMY_TOKEN;
                         TDM_PRINT3("cnt %d j %d DUMMY TOKEN %d \n", shuffle_cnt, j, FB6_DUMMY_TOKEN);
                        shuffle_cnt++;
                        skip_lpbk=0;
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(null_credits>0)token_cnt=8;
                        dummy_credits --;
                         j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(null_credits>0)token_cnt=8;
                      }
                     }
                     else {
                      if(dummy_credits>0) {
                       param_cal_main[j] = FB6_DUMMY_TOKEN;
                         TDM_PRINT3("cnt %d j %d DUMMY TOKEN %d \n", shuffle_cnt, j, FB6_DUMMY_TOKEN);
                        shuffle_cnt++;
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(null_credits>0)token_cnt=8;
                        dummy_credits --;
                        j++;
                         prog_done=1;
                        break;
                      }
                      else {
                        if(mgmt1_en==1) mgmt1_en = 0;
                        if(mgmt2_en==1) mgmt2_en = 0;
                        if(mgmt3_en==1) mgmt3_en = 0;
                        if(mgmt4_en==1) mgmt4_en = 0;
                        if(mgmt1_credits>0)token_cnt=0;
                        else if(mgmt2_credits>0)token_cnt=1;
                        else if(mgmt3_credits>0)token_cnt=2;
                        else if(mgmt4_credits>0)token_cnt=3;
                        else if(lpbk_credits>0)token_cnt=4;
                        else if(cpu_credits>0)token_cnt=5;
                        else if(fae_credits>0)token_cnt=6;
                        else if(idle1_credits>0)token_cnt=7;
                        else if(null_credits>0)token_cnt=8;
                      }
                     }
            case 10: TDM_PRINT0("Case Last\n");
               if(prog_done==1) {
                  prog_done = 0;
                  break;
               }
               else {
                 token_cnt=0;
                 break;

               }
        }

            }
            else {
              j++;
            }
        }
    }

return 0;

}
/**
@name: tdm_fb6_parse_tdm_tbl
@param:

Post-process TDM calendar tokenized entries
 */
int
tdm_fb6_parse_tdm_tbl(tdm_mod_t *_tdm)
{
    int j, result = PASS;
    int param_cal_id, param_cal_id_idb, param_cal_id_mmu;
    tdm_calendar_t *cal_idb=NULL;
    tdm_calendar_t *cal_mmu=NULL;

    param_cal_id     = _tdm->_core_data.vars_pkg.cal_id;

    switch(param_cal_id) {
        case 0:
            cal_idb = (&(_tdm->_chip_data.cal_0));
            cal_mmu = (&(_tdm->_chip_data.cal_4));
            param_cal_id_idb = 0;
            param_cal_id_mmu = 4;
            break;
        case 1:
            cal_idb = (&(_tdm->_chip_data.cal_1));
            cal_mmu = (&(_tdm->_chip_data.cal_5));
            param_cal_id_idb = 1;
            param_cal_id_mmu = 5;
            break;
        case 2:
            cal_idb = (&(_tdm->_chip_data.cal_2));
            cal_mmu = (&(_tdm->_chip_data.cal_6));
            param_cal_id_idb = 2;
            param_cal_id_mmu = 6;
            break;
        case 3:
            cal_idb = (&(_tdm->_chip_data.cal_3));
            cal_mmu = (&(_tdm->_chip_data.cal_7));
            param_cal_id_idb = 3;
            param_cal_id_mmu = 7;
            break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n", param_cal_id);
            return (TDM_EXEC_CORE_SIZE+1);
    }

    /* construct MMU TDM calendar by copying from IDB TDM calendar */
    for (j=0; j<(cal_idb->cal_len) && j<(cal_mmu->cal_len); j++) {
        cal_mmu->cal_main[j] = cal_idb->cal_main[j];
    }

                TDM_PRINT0("done copy \n");
    /* parse ancillary slots in IDB TDM calendar */
    if (tdm_fb6_parse_ancl(_tdm, param_cal_id_idb) != PASS) {
        result = FAIL;
    }
                TDM_PRINT1("done ancy %d \n",result);
    tdm_fb6_print_lr_cal(_tdm, param_cal_id_idb);

    /* parse ancillary slots in MMU TDM calendar */
    if (tdm_fb6_parse_ancl(_tdm, param_cal_id_mmu) != PASS) {
        result = FAIL;
    }
                TDM_PRINT1("done ancy %d \n",result);
    tdm_fb6_print_lr_cal(_tdm, param_cal_id_mmu);
                TDM_PRINT0("done print lr \n");

    /* construct Oversub Half Pipes */
    if (tdm_fb6_vbs_scheduler_ovs(_tdm) != PASS) {
        result = FAIL;
    }

                TDM_PRINT1("done half pipe %d \n",result);
    /* print: OVSB_HALF_PIPE, Packet_Scheduler, PORT_BLOCK_Map */
    tdm_fb6_print_pipe_cal(_tdm);

    if (result != PASS) {
        return (TDM_EXEC_CORE_SIZE+1);
    }
    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}
