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
@name: tdm_hr4_print_pipe_config
@param:


Parses pipe config from class structs
 */
void
tdm_hr4_print_pipe_config(tdm_mod_t *_tdm)
{
    int port_phy;
    int param_port_start, param_port_end, param_phy_lo, param_phy_hi,
        param_pipe_id;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_port_start= _tdm->_chip_data.soc_pkg.soc_vars.hr4.pipe_start;
    param_port_end  = _tdm->_chip_data.soc_pkg.soc_vars.hr4.pipe_end;
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
                TDM_PRINT1("%8d", tdm_hr4_cmn_get_port_pm(port_phy, _tdm));
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
@name: tdm_hr4_print_lr_cal
@param:

Print TDM calendar content of current pipe
 */
int
tdm_hr4_print_lr_cal(tdm_mod_t *_tdm, int cal_id)
{
    int j, token;
    int param_phy_lo, param_phy_hi, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_hr4_cmn_get_pipe_cal_len(param_cal_id, _tdm);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            token = param_cal_main[j];
            TDM_PRINT2("TDM: PIPE %d, element #%0d, ", param_cal_id, j);
            if (((token != HR4_OVSB_TOKEN) && 
                 !(token >= param_phy_lo && token <= param_phy_hi)) 
                ) {
                TDM_PRINT0("contains ACCESSORY TOKEN - ");
                switch (token) {
                    case HR4_MACSEC_TOKEN_0: TDM_PRINT0("HR4_MACSEC_TOKEN_0\n"); break;
                    case HR4_MACSEC_TOKEN_1: TDM_PRINT0("HR4_MACSEC_TOKEN_1\n"); break;
                    case HR4_MACSEC_TOKEN_2: TDM_PRINT0("HR4_MACSEC_TOKEN_2\n"); break;
                    case HR4_MACSEC_TOKEN_3: TDM_PRINT0("HR4_MACSEC_TOKEN_3\n"); break;
                    case HR4_IPFIX_TOKEN:    TDM_PRINT0("IPFIX/BSE-1\n"); break;
                    case HR4_LPB0_TOKEN:     TDM_PRINT0("LOOPBACK-0\n"); break;
                    case HR4_IDL1_TOKEN:     TDM_PRINT0("OPT-0\n"); break;
                    case HR4_IDL2_TOKEN:     TDM_PRINT0("OPT-1\n"); break;
                    case HR4_NULL_TOKEN:     TDM_PRINT0("NULL\n"); break;
                    case HR4_CMIC_TOKEN:     TDM_PRINT0("CMIC\n"); break;
                    default:                   TDM_PRINT1("UNKNOWN,%d\n",token); break;
                }
            } else {
                TDM_PRINT1("contains physical port #%0d\n", token);
            }
        }
    }

    return PASS;
}

/**
@name: tdm_hr4_print_pipe_cal
@param:

Print TDM calendar content of current pipe
 */
void
tdm_hr4_print_pipe_cal(tdm_mod_t *_tdm)
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
            for (j=0; j<HR4_SHAPING_GRP_LEN; j++) {
                if (j%16==0) {
                    TDM_PRINT2("\n %3d : %3d\t", j, j+15);
                }
                TDM_PRINT1("\t%d", param_cal_grp[HR4_SHAPING_GRP_IDX_0][j]);
            }
            TDM_PRINT0("\n\n");
            TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 1\n",param_pipe_id);
            for (j=0; j<HR4_SHAPING_GRP_LEN; j++) {
                if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
                TDM_PRINT1("\t%d",param_cal_grp[HR4_SHAPING_GRP_IDX_1][j]);
            }
            TDM_PRINT0("\n");
            TDM_SML_BAR
        }
        /* PM_number to Port_Block_number mapping table */
        TDM_PRINT1("\nTDM: PIPE %d, PM_NUM to PBLK_ID MAP \nPM:PBLK_ID",param_pipe_id);
        for (j=(16*param_cal_id); j<(16*(param_cal_id+1)) && j<param_num_phy_pm; j++) {
            TDM_PRINT2("\t %2d:%2d",j, _tdm->_chip_data.soc_pkg.soc_vars.hr4.pm_num_to_pblk[j]);
        }
        TDM_PRINT0("\n");
        TDM_SML_BAR
    }
}

/**
@name: tdm_hr4_parse_mgmt
@param:

Parse MGMT/CMIC slots in IDB TDM calendar
 */
/*
int
tdm_hr4_parse_mgmt(tdm_mod_t *_tdm, int cal_id)
{
    int cnt = 0, result = PASS;
    int ave_space, idx = 0, lo, hi, idx_lo, idx_hi, found = BOOL_TRUE;
    int mgmt_2nd_en, mgmt_slot_num;
    int param_mgmt_mode, param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_mgmt_mode = _tdm->_chip_data.soc_pkg.soc_vars.hr4.mgmt_mode;
    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_hr4_cmn_get_pipe_cal_len(param_cal_id, _tdm);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    if (param_mgmt_mode == HR4_MGMT_MODE_2_PORT_EN ||
        param_mgmt_mode == HR4_MGMT_MODE_2_PORT_HG) {
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
                        param_cal_main[idx] = HR4_CMIC_TOKEN;
                        break;
                    case 1: 
                    case 5: 
                        if (mgmt_2nd_en) {
                            param_cal_main[idx] = (cnt % 2 == 0) ?
                                                  HR4_MGM1_TOKEN :
                                                  HR4_MGM2_TOKEN;
                        } else {
                            param_cal_main[idx] = HR4_MGM1_TOKEN;
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
@name: tdm_hr4_parse_acc_idb
@param:

Parse ACCESSORY slots (LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_hr4_parse_acc_idb(tdm_mod_t *_tdm, int cal_id)
{
    int j, cnt = 0, freq = 0/*, total_macsec_token=64*/;
    int param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_hr4_cmn_get_pipe_cal_len(param_cal_id, _tdm);
    freq            = _tdm->_chip_data.soc_pkg.clk_freq;
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    cnt=0;
    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            if (param_cal_main[j] == param_token_ancl) {
                cnt++;
                switch (cnt) {
                    case 1:
                        param_cal_main[j] = HR4_IDL1_TOKEN;
                        break;
                    case 5: case 7: case 9:
                        param_cal_main[j] = HR4_IDL2_TOKEN;
                        break;
                    case 2:
                        param_cal_main[j] = HR4_LPB0_TOKEN;
                        break;
                    case 4:
                        param_cal_main[j] = HR4_CMIC_TOKEN;
                        break;
                    case 6: case 3:
                        param_cal_main[j] = HR4_NULL_TOKEN;
                        break;
                    case 8:
                        if(freq != 255)
                          param_cal_main[j] = HR4_IPFIX_TOKEN;
                        if(freq == 255)
                          param_cal_main[j] = HR4_NULL_TOKEN;
                        break;
                    default:
                        TDM_PRINT1("ANCL token match hit default - %0d\n", param_token_ancl);
                        break;
                }
               TDM_PRINT3("tdm_hr4_parse_acc_idb - j=%0d cnt=%0d value of ANC=%d \n", j, cnt, param_cal_main[j]);
            }
        }
    }

    return PASS;
}

/**
@name: tdm_hr4_parse_acc_mmu
@param:

Parse ACCESSORY slots (LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_hr4_parse_acc_mmu(tdm_mod_t *_tdm, int cal_id)
{
    int cnt = 0, j;
    int  param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_hr4_cmn_get_pipe_cal_len(param_cal_id, _tdm);
    TDM_SEL_CAL(param_cal_id, param_cal_main);


    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            if (param_cal_main[j] == param_token_ancl) {
                cnt++;
                switch (cnt) {
                    case 1: case 7:  case 8:
                        param_cal_main[j] = HR4_NULL_TOKEN;
                        break;
                    case 2:
                        param_cal_main[j] = HR4_LPB0_TOKEN;
                        break;
                    case 4:
                        param_cal_main[j] = HR4_CMIC_TOKEN;
                        break;
                    case 3: case 5: case 6:
                        param_cal_main[j] = HR4_IDL1_TOKEN;
                        break;
                    default:
                        TDM_PRINT1("ANCL token match hit default - %0d\n", param_token_ancl);
                        break;
                }
            }
        }
    }

    return PASS;
}

/**
@name: tdm_hr4_parse_ancl
@param:

Parse ANCILLARY slots (MGMT/CMIC, LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_hr4_parse_ancl(tdm_mod_t *_tdm, int cal_id)
{
    int idb_en = 0, mmu_en = 0, result = PASS;

    switch (cal_id) {
        case 0: case 1: idb_en = 1; break;
        case 4: case 5: mmu_en = 1; break;
        default:
            TDM_ERROR1("Invalid calendar ID - %0d\n", cal_id);
            return FAIL;
            break;
    }

    /* MGMT/CMIC */
 /*   if (tdm_hr4_parse_mgmt(_tdm, cal_id) != PASS) { */
 /*       TDM_ERROR1("Failed in MGMT/CMIC allocation, cal_id %0d\n", cal_id); */
 /*       result = FAIL; */
 /*   } */

    /* LOOPBACK, OPP1, OPP2, NULL, IDLE */
    if (idb_en == 1) {
        if (tdm_hr4_parse_acc_idb(_tdm, cal_id) != PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = FAIL;
        }
    } else if (mmu_en == 1) {
        if (tdm_hr4_parse_acc_mmu(_tdm, cal_id) != PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = FAIL;
        }
    }

    return result;
}

/**
@name: tdm_hr4_parse_tdm_tbl
@param:

Post-process TDM calendar tokenized entries
 */
int
tdm_hr4_parse_tdm_tbl(tdm_mod_t *_tdm)
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
    if (tdm_hr4_parse_ancl(_tdm, param_cal_id_idb) != PASS) {
        result = FAIL;
    }
                TDM_PRINT1("done ancy %d \n",result);
    tdm_hr4_print_lr_cal(_tdm, param_cal_id_idb);

    /* parse ancillary slots in MMU TDM calendar */
    if (tdm_hr4_parse_ancl(_tdm, param_cal_id_mmu) != PASS) {
        result = FAIL;
    }
                TDM_PRINT1("done MMU ancy %d \n",result);
    tdm_hr4_print_lr_cal(_tdm, param_cal_id_mmu);
                TDM_PRINT0("done print MMU lr \n");

    /* construct Oversub Half Pipes */
    if (tdm_hr4_vbs_scheduler_ovs(_tdm) != PASS) {
        result = FAIL;
    }

                TDM_PRINT1("done half pipe %d \n",result);
    /* print: OVSB_HALF_PIPE, Packet_Scheduler, PORT_BLOCK_Map */
    /*tdm_hr4_print_pipe_cal(_tdm);*/

    if (result != PASS) {
        return (TDM_EXEC_CORE_SIZE+1);
    }
    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}
