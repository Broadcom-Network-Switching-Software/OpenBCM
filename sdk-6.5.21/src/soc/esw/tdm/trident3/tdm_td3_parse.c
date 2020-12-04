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
@name: tdm_td3_print_pipe_config
@param:

Parses pipe config from class structs
 */
void
tdm_td3_print_pipe_config(tdm_mod_t *_tdm)
{
    int port_phy, port_speed;
    int param_port_start, param_port_end, param_phy_lo, param_phy_hi,
        param_pipe_id;

    param_port_start= _tdm->_chip_data.soc_pkg.soc_vars.td3.pipe_start;
    param_port_end  = _tdm->_chip_data.soc_pkg.soc_vars.td3.pipe_end;
    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pipe_id   = _tdm->_core_data.vars_pkg.pipe;

    if (param_port_start >= param_phy_lo && param_port_end <= param_phy_hi){
        TDM_BIG_BAR
        TDM_PRINT3("TDM: Pipe Config: Pipe %d, Port [%d - %d] \n\n",
                   param_pipe_id, param_port_start, param_port_end);
        TDM_PRINT5("%8s%8s%8s%8s%8s\n","port", "pm", "speed", "LR/OS", "EN/HG");
        for (port_phy=param_port_start; port_phy<=param_port_end; port_phy++) {
            port_speed = tdm_td3_cmn_get_port_speed(_tdm, port_phy);
            if (port_speed>0) {
                TDM_PRINT1("%8d", port_phy);
                TDM_PRINT1("%8d", tdm_td3_cmn_get_port_pm(_tdm, port_phy));
                TDM_PRINT1("%8d", (port_speed/1000));
                if (BOOL_TRUE == tdm_td3_cmn_chk_port_is_lr(_tdm, port_phy)) {
                    TDM_PRINT1("%8s", "LR");
                } else {
                    TDM_PRINT1("%8s", "OS");
                }
                if (BOOL_TRUE == tdm_td3_cmn_chk_port_is_hg(_tdm, port_phy)) {
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
@name: tdm_td3_print_lr_cal
@param:

Print TDM calendar content of current pipe
 */
int
tdm_td3_print_lr_cal(tdm_mod_t *_tdm, int cal_id)
{
    int j, token;
    int mgmt_2nd_en = 0;
    int param_phy_lo, param_phy_hi,
        param_mgmt_mode,
        param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_mgmt_mode = _tdm->_chip_data.soc_pkg.soc_vars.td3.mgmt_mode;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_td3_cmn_get_pipe_cal_len(_tdm, param_cal_id);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    if (param_mgmt_mode == TD3_MGMT_MODE_2_PORT_EN ||
        param_mgmt_mode == TD3_MGMT_MODE_2_PORT_HG) {
        mgmt_2nd_en = 1;
    }

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            token = param_cal_main[j];
            TDM_PRINT2("TDM: PIPE %d, element #%0d, ", param_cal_id, j);
            if (((token != TD3_OVSB_TOKEN) &&
                 !(token >= param_phy_lo && token <= param_phy_hi)) ||
                (token == TD3_MGM2_TOKEN && mgmt_2nd_en == 1)) {
                TDM_PRINT0("contains ACCESSORY TOKEN - ");
                switch (token) {
                    case TD3_CMIC_TOKEN: TDM_PRINT0("CMIC/CPU\n"); break;
                    case TD3_MGM1_TOKEN: TDM_PRINT0("MANAGEMENT-1\n"); break;
                    case TD3_MGM2_TOKEN: TDM_PRINT0("MANAGEMENT-2\n"); break;
                    case TD3_LPB0_TOKEN: TDM_PRINT0("LOOPBACK-0\n"); break;
                    case TD3_LPB1_TOKEN: TDM_PRINT0("LOOPBACK-1\n"); break;
                    case TD3_IDL1_TOKEN: TDM_PRINT0("OPPORTUNISTIC-1\n"); break;
                    case TD3_NULL_TOKEN: TDM_PRINT0("NULL\n"); break;
                    case TD3_IDL2_TOKEN: TDM_PRINT0("IDLE\n"); break;
                    default: TDM_PRINT0("UNKNOWN\n"); break;
                }
            } else {
                TDM_PRINT1("contains physical port #%0d\n", token);
            }
        }
        TDM_SML_BAR
    }

    return PASS;
}

/**
@name: tdm_td3_print_pipe_cal
@param:

Print TDM calendar content of current pipe
 */
void
tdm_td3_print_pipe_cal(tdm_mod_t *_tdm)
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
            for (j=0; j<TD3_SHAPING_GRP_LEN; j++) {
                if (j%16==0) {
                    TDM_PRINT2("\n %3d : %3d\t", j, j+15);
                }
                TDM_PRINT1("\t%d", param_cal_grp[TD3_SHAPING_GRP_IDX_0][j]);
            }
            TDM_PRINT0("\n\n");
            TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 1\n",param_pipe_id);
            for (j=0; j<TD3_SHAPING_GRP_LEN; j++) {
                if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
                TDM_PRINT1("\t%d",param_cal_grp[TD3_SHAPING_GRP_IDX_1][j]);
            }
            TDM_PRINT0("\n");
            TDM_SML_BAR
        }
        /* PM_number to Port_Block_number mapping table */
        TDM_PRINT1("\nTDM: PIPE %d, PM_NUM to PBLK_ID MAP \nPM:PBLK_ID",param_pipe_id);
        for (j=(16*param_cal_id); j<(16*(param_cal_id+1)) && j<param_num_phy_pm; j++) {
            TDM_PRINT2("\t %2d:%2d",j, _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_num_to_pblk[j]);
        }
        TDM_PRINT0("\n");
        TDM_SML_BAR
    }
}

/**
@name: tdm_td3_parse_filter_2nd_mgmt
@param:

This filter is used to fix a RTL defect on LineRate config when
the 2nd management port (physical port 128) is enabled.
Slot distribution pattern [..., 128, x, x, 125/126/127, ...]
 */
int
tdm_td3_parse_filter_2nd_mgmt(tdm_mod_t *_tdm, int cal_id)
{
    int port_2nd_mgmt, pm_2nd_mgmt, nb_sister_pass;
    int i, j, k, n, i_l, i_r, p_l, p_r;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_lr_en, param_mgmt_mode, param_sister_min,
        param_phy_lo, param_phy_hi;
    int *param_cal_main;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_mgmt_mode   = _tdm->_chip_data.soc_pkg.soc_vars.td3.mgmt_mode;
    param_sister_min = _tdm->_core_data.rule__prox_port_min;

    TDM_SEL_CAL(cal_id, param_cal_main);

    /* check if filter is applicable: linerate + 2nd_mgmt_enable */
    if (!((param_mgmt_mode == TD3_MGMT_MODE_2_PORT_EN ||
           param_mgmt_mode == TD3_MGMT_MODE_2_PORT_HG) &&
          (param_lr_en == BOOL_TRUE))) {
        return 0;
    }

    /* filter 2nd management port slots */
    TDM_PRINT1("\nFiltering 2nd management port on pattern %s\n",
               "..., 128, <x, x,> 125/126/127, ...");
    port_2nd_mgmt = TD3_MGM2_TOKEN;
    pm_2nd_mgmt = (port_2nd_mgmt - 1) / 4;
    for (i = 0; i < param_cal_len; i++) {
        if (param_cal_main[i] == port_2nd_mgmt) {
            for (j = 1; j < param_sister_min; j++) {
                n = (i + j) % param_cal_len;
                if (((param_cal_main[n] - 1) / 4) == pm_2nd_mgmt) {
                    nb_sister_pass = BOOL_TRUE;
                    for (k = 0; k < param_sister_min - j; k++) {
                        i_l = (i + param_cal_len - 1 - k) % param_cal_len;
                        i_r = (i_l + param_sister_min) % param_cal_len;
                        p_l = param_cal_main[i_l];
                        p_r = param_cal_main[i_r];
                        if ((p_l >= param_phy_lo && p_l <= param_phy_hi) &&
                            ((p_l - 1) / 4 == (p_r - 1) / 4)) {
                            nb_sister_pass = BOOL_FALSE;
                            break;
                        }
                    }
                    if (nb_sister_pass == BOOL_TRUE) {
                        for (k = 0; k < param_sister_min - j; k++) {
                            i_l = (i + param_cal_len - 1 - k) % param_cal_len;
                            i_r = (i_l + 1) % param_cal_len;
                            param_cal_main[i_r] = param_cal_main[i_l];
                        }
                        i_l = (i + param_cal_len - k) % param_cal_len;
                        param_cal_main[i_l] = port_2nd_mgmt;
                        TDM_PRINT3("Migrate port %0d from #%03d to #%03d\n",
                                    port_2nd_mgmt, i, i_l);
                    } else {
                        TDM_WARN2("%s, slot index %0d\n",
                                  "Failed to shift 2nd mgmt port slot", i);
                    }
                }
            }
        }
    }

    TDM_SML_BAR

    return 0;
}

/**
@name: tdm_td3_parse_mgmt
@param:

Parse MGMT/CMIC slots in IDB TDM calendar
 */
int
tdm_td3_parse_mgmt(tdm_mod_t *_tdm, int cal_id)
{
    int cnt = 0, result = PASS;
    int ave_space, idx = 0, lo, hi, idx_lo, idx_hi, found = BOOL_TRUE;
    int mgmt_2nd_en, mgmt_slot_num;
    int param_mgmt_mode, param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_mgmt_mode = _tdm->_chip_data.soc_pkg.soc_vars.td3.mgmt_mode;
    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_td3_cmn_get_pipe_cal_len(_tdm, param_cal_id);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    if (param_mgmt_mode == TD3_MGMT_MODE_2_PORT_EN ||
        param_mgmt_mode == TD3_MGMT_MODE_2_PORT_HG) {
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
        /* find first pos of ANCL slot */
        while (idx < param_cal_len &&
               param_cal_main[idx] != param_token_ancl) {
            idx++;
        }

        /* allocate MGMT/CMIC slots */
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
                    case 0: /* IDB pipe 0*/
                    case 4: /* MMU pipe 0*/
                        param_cal_main[idx] = TD3_CMIC_TOKEN;
                        break;
                    case 1: /* IDB pipe 1*/
                    case 5: /* MMU pipe 1*/
                        if (mgmt_2nd_en) {
                            param_cal_main[idx] = (cnt % 2 == 0) ?
                                                  TD3_MGM1_TOKEN :
                                                  TD3_MGM2_TOKEN;
                        } else {
                            param_cal_main[idx] = TD3_MGM1_TOKEN;
                        }
                        break;
                    default :
                        break;
                }
                TDM_PRINT2("TDM: Allocate MGMT/CMIC slot %0d at index #%03d\n",
                           cnt+1, idx);
                cnt++;
                idx = (idx + ave_space) % param_cal_len;
                if (cnt >= mgmt_slot_num) {
                    break;
                }
            }
        }
        TDM_PRINT0("\n");

        /* check allocated slot amount */
        if (cnt < mgmt_slot_num) {
            result = FAIL;
            TDM_ERROR4("%s, cal_id %0d, req_slot %0d, alloc_slot %0d\n",
                       "Failed in MGMT/CMIC slot allocation",
                       param_cal_id, mgmt_slot_num, cnt);
        }
    }

    /* Filter slots of the 2nd mamagement port (temporal fix for RTL) */
    tdm_td3_parse_filter_2nd_mgmt(_tdm, cal_id);

    return (result);
}

/**
@name: tdm_td3_parse_acc_idb
@param:

Parse ACCESSORY slots (LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_td3_parse_acc_idb(tdm_mod_t *_tdm, int cal_id)
{
    int j, cnt = 0;
    int param_token_lpbk, param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_td3_cmn_get_pipe_cal_len(_tdm, param_cal_id);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    switch (param_cal_id) {
        case 0: param_token_lpbk = TD3_LPB0_TOKEN; break;
        case 1: param_token_lpbk = TD3_LPB1_TOKEN; break;
        default: return FAIL; break;
    }

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            if (param_cal_main[j] == param_token_ancl) {
                cnt++;
                switch (cnt) {
                    case 1: case 3: case 5:  case 7:
                        param_cal_main[j] = param_token_lpbk;
                        break;
                    case 2:
                        param_cal_main[j] = TD3_IDL1_TOKEN;
                        break;
                    case 6:
                        param_cal_main[j] = TD3_NULL_TOKEN;
                        break;
                    case 4: case 8:
                        param_cal_main[j] = TD3_IDL2_TOKEN;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return PASS;
}

/**
@name: tdm_td3_parse_acc_mmu
@param:

Parse ACCESSORY slots (LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_td3_parse_acc_mmu(tdm_mod_t *_tdm, int cal_id)
{
    int cnt = 0, j;
    int param_token_lpbk, param_token_ancl, param_cal_id, param_cal_len;
    int *param_cal_main=NULL;

    param_token_ancl= _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_cal_id    = cal_id;
    param_cal_len   = tdm_td3_cmn_get_pipe_cal_len(_tdm, param_cal_id);
    TDM_SEL_CAL(param_cal_id, param_cal_main);

    switch (param_cal_id) {
        case 4: param_token_lpbk = TD3_LPB0_TOKEN; break;
        case 5: param_token_lpbk = TD3_LPB1_TOKEN; break;
        default: return FAIL; break;
    }

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            if (param_cal_main[j] == param_token_ancl) {
                cnt++;
                switch (cnt) {
                    case 1: case 3: case 5:  case 7:
                        param_cal_main[j] = param_token_lpbk;
                        break;
                    case 2: case 4:  case 6:
                        param_cal_main[j] = TD3_NULL_TOKEN;
                        break;
                    case 8:
                        param_cal_main[j] = TD3_IDL2_TOKEN;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return PASS;
}

/**
@name: tdm_td3_parse_ancl
@param:

Parse ANCILLARY slots (MGMT/CMIC, LOOPBACK, OPP1, OPP2, NULL, IDLE)
 */
int
tdm_td3_parse_ancl(tdm_mod_t *_tdm, int cal_id)
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
    if (tdm_td3_parse_mgmt(_tdm, cal_id) != PASS) {
        TDM_ERROR1("Failed in MGMT/CMIC allocation, cal_id %0d\n", cal_id);
        result = FAIL;
    }

    /* LOOPBACK, OPP1, OPP2, NULL, IDLE */
    if (idb_en == 1) {
        if (tdm_td3_parse_acc_idb(_tdm, cal_id) != PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = FAIL;
        }
    } else if (mmu_en == 1) {
        if (tdm_td3_parse_acc_mmu(_tdm, cal_id) != PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = FAIL;
        }
    }

    return result;
}

/**
@name: tdm_td3_parse_tdm_tbl
@param:

Post-process TDM calendar tokenized entries
 */
int
tdm_td3_parse_tdm_tbl(tdm_mod_t *_tdm)
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

    /* parse ancillary slots in IDB TDM calendar */
    if (tdm_td3_parse_ancl(_tdm, param_cal_id_idb) != PASS) {
        result = FAIL;
    }
    tdm_td3_print_lr_cal(_tdm, param_cal_id_idb);

    /* parse ancillary slots in MMU TDM calendar */
    if (tdm_td3_parse_ancl(_tdm, param_cal_id_mmu) != PASS) {
        result = FAIL;
    }
    tdm_td3_print_lr_cal(_tdm, param_cal_id_mmu);

    /* construct Oversub Half Pipes */
    if (tdm_td3_vbs_scheduler_ovs(_tdm) != PASS) {
        result = FAIL;
    }

    /* print: OVSB_HALF_PIPE, Packet_Scheduler, PORT_BLOCK_Map */
    tdm_td3_print_pipe_cal(_tdm);

    if (result != PASS) {
        return (TDM_EXEC_CORE_SIZE+1);
    }
    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}
