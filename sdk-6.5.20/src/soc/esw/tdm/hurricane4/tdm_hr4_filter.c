/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip based calendar post-processing filters
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_hr4_filter_chk_slot_shift_cond
@param:
 */
int
tdm_hr4_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm)
{
    int k, src, dst, port, port_pm, idx, port_k, idx_k, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_space_same, param_space_sister;
    
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_same  = _tdm->_core_data.rule__same_port_min;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    
    if (slot < cal_len) {
        /* determine src and dst index */
        src = slot;
        if (dir == TDM_DIR_UP) {
            dst = (src + cal_len - 1) % cal_len;
        } else {
            dst = (src + 1) % cal_len;
        }

        /* check src port spacing */
        idx     = src;
        port    = cal_main[idx];
        port_pm = tdm_hr4_cmn_get_port_pm(port, _tdm);
        if (port >= param_phy_lo && port <= param_phy_hi) {
            /* same port spacing */
            for (k=1; k<=param_space_same; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + cal_len - k) % cal_len;
                } else {
                    idx_k = (idx + k) % cal_len;
                }
                if (cal_main[idx_k] == port) {
                    result = FAIL;
                    break;
                }
            }
            /* sister port spacing */
            for (k=1; k<=param_space_sister; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + cal_len - k) % cal_len;
                } else {
                    idx_k = (idx + k) % cal_len;
                }
                port_k = cal_main[idx_k];
                if (port_k >= param_phy_lo && port_k <= param_phy_hi) {
                    if (tdm_hr4_cmn_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }

        /* check dst port spacing */
        idx     = dst;
        port    = cal_main[idx];
        port_pm = tdm_hr4_cmn_get_port_pm(port, _tdm);
        if (port >= param_phy_lo && port <= param_phy_hi) {
            /* same port spacing */
            for (k=1; k<=param_space_same; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + k) % cal_len;
                } else {
                    idx_k = (idx + cal_len - k) % cal_len;
                }
                if (cal_main[idx_k] == port) {
                    result = FAIL;
                    break;
                }
            }
            /* sister port spacing */
            for (k=1; k<=param_space_sister; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + k) % cal_len;
                } else {
                    idx_k = (idx + cal_len - k) % cal_len;
                }
                port_k = cal_main[idx_k];
                if (port_k >= param_phy_lo && port_k <= param_phy_hi) {
                    if (tdm_hr4_cmn_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }
    }

    return (result);
}


/**
@name: tdm_hr4_filter_lr_jitter
@param:
 */
int
tdm_hr4_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed)
{
    int i, j, k, filter_cnt=0, x, y, chk_x, chk_y;
    int m, port, port_speed, idx_curr,
        space_min, space_max,
        dist_up, dist_dn, dist_mv;
    int *port_bmp;
    int param_lr_limit, param_ancl_num, param_cal_len, param_ext_phy_num,
        param_lr_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_ext_phy_num = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id, param_cal_main);

    /* allocate memory */
    port_bmp = (int*) TDM_ALLOC(param_ext_phy_num*sizeof(int), 
                                "hr4_filter_lr_jitter, port_bmp");
    for (i=0; i<param_ext_phy_num; i++) {
        port_bmp[i] = 0;
    }
    
    TDM_PRINT0("Smooth LR port jitter\n\n");
    if (param_lr_en==BOOL_TRUE && min_speed>=SPEED_10G) {
        for (i=0; i<param_cal_len; i++) {
            port = param_cal_main[i];
            port_speed = tdm_hr4_cmn_get_port_speed(port, _tdm);
            if (port_speed == SPEED_0 || port_speed < min_speed) {
                continue;
            }
            if (port < param_ext_phy_num) {
                if (port_bmp[port] == 1) {
                    continue;
                }
                port_bmp[port] = 1;
                tdm_hr4_cmn_get_speed_jitter(port_speed, param_cal_len,
                                             &space_min, &space_max);
                for (j=0; j<param_cal_len; j++) {
                    k = (i+param_cal_len-j)%param_cal_len;
                    if (param_cal_main[k] == port) {
                        idx_curr = k;
                        dist_up = tdm_hr4_cmn_get_same_port_dist(idx_curr,
                                    TDM_DIR_UP, param_cal_main, param_cal_len);
                        dist_dn = tdm_hr4_cmn_get_same_port_dist(idx_curr,
                                    TDM_DIR_DN, param_cal_main, param_cal_len);
                        /* filter port if space violation is detected: up */
                        if (dist_up > space_max || dist_dn < space_min) {
                            TDM_PRINT7("%s, port %d, slot %d, dist_up %d > %d, dist_dn %d < %d\n",
                                      "TDM: [Linerate jitter 1]",
                                      port, idx_curr,
                                      dist_up, space_max, dist_dn, space_min);
                            dist_mv = (dist_up-dist_dn)/2;
                            for (m=1; m<=dist_mv; m++) {
                                /* check spacing for neighbor and target port */
                                x = (idx_curr + param_cal_len - m) % param_cal_len;
                                y = (x + 1) % param_cal_len;
                                chk_x = tdm_hr4_filter_chk_slot_shift_cond(x, TDM_DIR_DN,
                                            param_cal_len, param_cal_main, _tdm);
                                chk_y = tdm_hr4_filter_chk_slot_shift_cond(y, TDM_DIR_UP,
                                            param_cal_len, param_cal_main, _tdm);
                                /* shift port */
                                if (chk_x == PASS && chk_y == PASS) {
                                    param_cal_main[y] = param_cal_main[x];
                                    param_cal_main[x] = port;
                                    filter_cnt++;
                                    TDM_PRINT3("\t Shift port %d UP from slot %d to slot %d\n",
                                              port, y, x);
                                } else {
                                    break;
                                }
                            }
                        }
                        /* filter port if space violation is detected: down */
                        else if (dist_up < space_min || dist_dn > space_max) {
                            TDM_PRINT7("%s port %d, slot %d, dist_up %d < %d, dist_dn %d > %d\n",
                                      "TDM: [Linerate jitter 2]",
                                      port, idx_curr,
                                      dist_up, space_min, dist_dn, space_max);
                            dist_mv = (dist_dn-dist_up)/2;
                            for (m=1; m<=dist_mv; m++) {
                                /* check spacing for neighbor and target port */
                                x = (idx_curr + m) % param_cal_len;
                                y = (x + param_cal_len - 1) % param_cal_len;
                                chk_x = tdm_hr4_filter_chk_slot_shift_cond(x, TDM_DIR_UP,
                                            param_cal_len, param_cal_main, _tdm);
                                chk_y = tdm_hr4_filter_chk_slot_shift_cond(y, TDM_DIR_DN,
                                            param_cal_len, param_cal_main, _tdm);
                                /* shift port */
                                if (chk_x == PASS && chk_y == PASS) {
                                    param_cal_main[y] = param_cal_main[x];
                                    param_cal_main[x] = port;
                                    filter_cnt++;
                                    TDM_PRINT3("\t Shift port %d DOWN from slot %d to slot %d\n",
                                              port, y, x);
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    TDM_SML_BAR
    
    TDM_FREE(port_bmp);
    return (filter_cnt);
}

/**
@name: tdm_hr4_filter_lr
@param:
 */
int
tdm_hr4_filter_lr(tdm_mod_t *_tdm)
{
    if (tdm_hr4_filter_lr_jitter(_tdm, SPEED_10G) == 0){
        return PASS;
    }
    if (tdm_hr4_filter_lr_jitter(_tdm, SPEED_20G) == 0) {
        return PASS;
    }
    if (tdm_hr4_filter_lr_jitter(_tdm, SPEED_40G) == 0) {
        return PASS;
    }
    return PASS;
}

void
tdm_hr4_tdm_print(int *tdm_tbl, int length) 
{
        int i;
        for (i=0; i<length; i++) {      
                if (i%10==0) TDM_PRINT2("\nTDM-TBL %3d : %3d ", i, tdm_tbl[i]);
                else TDM_PRINT1(" %3d ", tdm_tbl[i]);
        }
        TDM_PRINT0("\n"); 
}  
 
/**
@name: tdm_hr4_osub_filter
@param:
 */
int
tdm_hr4_osub_filter( tdm_mod_t *_tdm )
{
   int os_ratio=0, lr_ratio=0, num_anc_token=0;
   int lr_idx=0, ovs_idx, cur_idx, swap_idx;
    
    int i, lr_slot_cnt=0, os_slot_cnt=0;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb, param_token_ancl;
    int *param_cal_main;
    enum port_speed_e *param_speeds;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    lr_idx = 0; 
    for (i=0; i<param_cal_len; i++){
/*TDM_PRINT2(" param_cal_main[%0d] speed is %d \n", i, tdm_hr4_cmn_get_port_speed(param_cal_main[i],_tdm));*/
        if (param_cal_main[i]==param_token_ovsb){
            os_slot_cnt++;
        }
        else {
            lr_slot_cnt++;
	    if (param_cal_main[i]==param_token_ancl) num_anc_token++;	
        }
    }

TDM_PRINT0("tdm_hr4_osub_filter::1\n"); 
tdm_hr4_tdm_print(param_cal_main, param_cal_len);

    if (os_slot_cnt>lr_slot_cnt && lr_slot_cnt>0) os_ratio=(os_slot_cnt + lr_slot_cnt)/lr_slot_cnt  ; /*ceil(os_slot_cnt/lr_slot_cnt)*/
    else if (os_slot_cnt>0) lr_ratio=(lr_slot_cnt + os_slot_cnt)/os_slot_cnt ; /*ceil(lr_slot_cnt/os_slot_cnt)*/

   if (os_ratio>0) {
       /*Step 1- find OS clump. If the number of successive OS ports > os_ratio, then
         we have identified a OS clump*/
       for (cur_idx=0; cur_idx<param_cal_len; cur_idx++) {
           ovs_idx=cur_idx;
           /*Step 2- Traverse till the end of the OS clump & check for the next available slot 
             that is eligible for a SWAP. The slot that is eligible for a swap will be either
             ANC token slot or port with speed==1G (applicable only for HR4/HR4)
            */
           while(param_cal_main[ovs_idx]==param_token_ovsb && ovs_idx<param_cal_len) ovs_idx++; /*FIXME: wraparound not checked! bk*/
           if (ovs_idx-cur_idx>os_ratio) { /*if true, then there is a eligibility for SWAP*/
               for(swap_idx=ovs_idx; swap_idx<param_cal_len; swap_idx++) {
                 /*Step 3-  Swap such a port (index) with start of the clump + os_ratio and iterate step-2*/
                  if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
			(((param_speeds[(param_cal_main[swap_idx])] == SPEED_10G) ||(param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G))  && os_ratio<3)) {
/*TDM_PRINT2("Inside os_ratio for  %0d with %0d\n",  swap_idx, param_cal_main[swap_idx]);*/

                     param_cal_main[cur_idx+os_ratio] = param_cal_main[swap_idx];
                     param_cal_main[swap_idx] = param_token_ovsb;
/*TDM_PRINT2("Swapped %0d with %0d\n", cur_idx, swap_idx);
tdm_hr4_tdm_print(param_cal_main, param_cal_len);*/
                     break;
                  }
               }
           }
       }
       for (cur_idx=param_cal_len; cur_idx>=0; cur_idx--) {
           ovs_idx=cur_idx;
           while(param_cal_main[ovs_idx]==param_token_ovsb && ovs_idx<param_cal_len) ovs_idx--;
           if (cur_idx-ovs_idx>os_ratio) { /*if true, then there is a eligibility for SWAP*/
               for(swap_idx=ovs_idx; swap_idx>=0; swap_idx--) {
                  if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
			(((param_speeds[(param_cal_main[swap_idx])] == SPEED_10G) ||(param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G))  && os_ratio<3)) {
                     param_cal_main[cur_idx-os_ratio] = param_cal_main[swap_idx];
                     param_cal_main[swap_idx] = param_token_ovsb;
                     break;
                  }
               }
           }
       }
   } else if (lr_ratio>0) {
       /*Step 1- find LR clump. If the number of successive LR ports > lr_ratio, then
         we have identified a LR clump*/
       for (cur_idx=0; cur_idx<param_cal_len; cur_idx++) {
           lr_idx=cur_idx;
           /*Step 2- Traverse till the end of the LR clump & check for the next available slot 
             that is eligible for a SWAP. The slot that is eligible for a swap will be either
             ANC token slot or port with speed==1G (applicable only for HR4/HR4)
            */
           while(param_cal_main[lr_idx]!=param_token_ovsb && lr_idx<param_cal_len) {
               if ((param_speeds[param_cal_main[lr_idx]]==SPEED_50G) || (param_speeds[param_cal_main[lr_idx]]==SPEED_100G)) { /* 50G & 100G port speeds are sensitive to spacing - dont touch them cfg:5708 */
                     break; 
               }
               lr_idx++; /*FIXME: wraparound not checked! bk*/
            }
           if (lr_idx-cur_idx>lr_ratio && (param_speeds[param_cal_main[lr_idx]]==SPEED_1G || param_speeds[param_cal_main[lr_idx]]==SPEED_2p5G || 
		((param_speeds[param_cal_main[lr_idx]]==SPEED_10G || param_speeds[param_cal_main[lr_idx]]==SPEED_25G) && lr_ratio<3)  )) { /*if true, then there is a eligibility for SWAP*/
               for(swap_idx=lr_idx; swap_idx<param_cal_len; swap_idx++) {
                 /*Step 3-  Swap such a port (index) with start of the clump + lr_ratio and iterate step-2*/
/*TDM_PRINT2("Inside lr_ratio for  %0d with %0d\n",  swap_idx, param_cal_main[swap_idx]);*/
                  if (param_cal_main[swap_idx]==param_token_ovsb) { 
                     for(i=swap_idx;i>cur_idx+lr_ratio;i--) param_cal_main[i] = param_cal_main[i-1];
                     param_cal_main[cur_idx+lr_ratio] = param_token_ovsb;
/*TDM_PRINT2("Swapped %0d with %0d\n", cur_idx+lr_ratio, swap_idx);
tdm_hr4_tdm_print(param_cal_main, param_cal_len);*/
                     break;
                  } 
               }
           }
       }
       /*Step 4 - Check if there are OS clumps. If yes, then they need to be decluttered*/
       for (cur_idx=0; cur_idx<param_cal_len; cur_idx++) {
           ovs_idx=cur_idx;
           /*Step 4.1- Traverse till the end of the OS clump & check for the next available slot 
             that is eligible for a SWAP. The slot that is eligible for a swap will be either
             ANC token slot or port with speed==1G (applicable only for HR4/HR4)
            */
           while(param_cal_main[ovs_idx]==param_token_ovsb && ovs_idx<param_cal_len) ovs_idx++; /*FIXME: wraparound not checked! bk*/
           if (ovs_idx-cur_idx>1) { /*if true, then there is a eligibility for SWAP*/
               for(swap_idx=ovs_idx; swap_idx<param_cal_len; swap_idx++) {
                 /*Step 4.2-  Swap such a port (index) with start of the clump + os_ratio and iterate step-2*/
                  if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
			((param_speeds[(param_cal_main[swap_idx])] == SPEED_10G ||param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G || param_speeds[param_cal_main[lr_idx]]==SPEED_25G)  /*&& lr_ratio<7*/)) {
                     param_cal_main[cur_idx+1] = param_cal_main[swap_idx];
                     param_cal_main[swap_idx] = param_token_ovsb;
/*TDM_PRINT2("Swapped %0d with %0d\n", cur_idx, swap_idx);
tdm_hr4_tdm_print(param_cal_main, param_cal_len);*/
                     break;
                  }
               }
           }
       }
       for (cur_idx=param_cal_len; cur_idx>=0; cur_idx--) {
           ovs_idx=cur_idx;
           while(param_cal_main[ovs_idx]==param_token_ovsb && ovs_idx<param_cal_len) ovs_idx--;
           if (cur_idx-ovs_idx>1) { /*if true, then there is a eligibility for SWAP*/
               for(swap_idx=ovs_idx; swap_idx>=0; swap_idx--) {
                  if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
			((param_speeds[(param_cal_main[swap_idx])] == SPEED_10G ||param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G || param_speeds[param_cal_main[lr_idx]]==SPEED_25G)  /*&& lr_ratio<7*/)) {
                     param_cal_main[cur_idx-1] = param_cal_main[swap_idx];
                     param_cal_main[swap_idx] = param_token_ovsb;
                     break;
                  }
               }
           }
       }
   }
tdm_hr4_same_port_1n6_filter(_tdm);
return(0);
}


/**
@name: tdm_hr4_same_port_1n6_filter
@param:
 */
int
tdm_hr4_same_port_1n6_filter( tdm_mod_t *_tdm )
{
   int lr_ratio=0;
   int swap_idx_start, ovs_idx=0, swap_idx; 
    
    int i;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb, param_token_ancl;
    int *param_cal_main,  *param_os_buff;
    enum port_speed_e *param_speeds;
    int hp0_idx=0, hp1_idx=0, *hp0, *hp1, flag=0;
    int hp0_num_os_port=0, hp1_num_os_port=0, param_token_empty;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_os_buff     = _tdm->_core_data.vars_pkg.os_buffer;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
 
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    hp0     = (int*) TDM_ALLOC(param_cal_len*sizeof(int), 
                                   "tdm_hr4_same_port_1n6_filter, hp0");
    hp1     = (int*) TDM_ALLOC(param_cal_len*sizeof(int), 
                                   "tdm_hr4_same_port_1n6_filter, hp1");
    
    for (i=0; i<param_cal_len; i++){
        if (param_cal_main[i]==param_token_ovsb){
            if (flag==1) { hp0[hp0_idx]=i; hp0_idx++;flag=0;}
	    else { hp1[hp1_idx]=i; hp1_idx++; flag=1;}
        }


    }
    while (param_os_buff[ovs_idx]!=param_token_empty  ) {
	if (ovs_idx%2) hp0_num_os_port++;
	else  hp1_num_os_port++;
TDM_PRINT2("param_os_buff[%0d] = %0d \n", ovs_idx, param_os_buff[ovs_idx]);
	ovs_idx++;
    }
TDM_PRINT2("Number of OS ports in HP0=%0d, HP1=%0d\n", hp0_num_os_port, hp1_num_os_port);
TDM_PRINT2("Indices Number HP0=%0d, HP1=%0d\n", hp0_idx, hp1_idx);
tdm_hr4_tdm_print(hp0, hp0_idx);

if (hp0_num_os_port<2) {
    for (i=1;i<=hp0_idx-hp0_num_os_port;i++) {
      if (hp0[i*hp0_num_os_port] - hp0[i-1]<6) { /* Same port spacing violation  - fix required*/
        TDM_PRINT2("Noticed violations between idx - %0d and %0d. need to space them.\n", hp0[i], hp0[i-1]) ;
	swap_idx_start = hp0[i*hp0_num_os_port] + 6 - (hp0[i*hp0_num_os_port]-hp0[i-1]);
               for(swap_idx=swap_idx_start; swap_idx<param_cal_len; swap_idx++) {
                  if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
			((/*param_speeds[(param_cal_main[swap_idx])] == SPEED_25G ||*/param_speeds[(param_cal_main[swap_idx])] == SPEED_10G ||param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G )  && lr_ratio<3)) {
                     param_cal_main[hp0[i*hp0_num_os_port]] = param_cal_main[swap_idx];
                     param_cal_main[swap_idx] = param_token_ovsb;
        TDM_PRINT2("Swapping idx %0d with %0d\n", hp0[i*hp0_num_os_port], swap_idx) ;
                     break;
                  }
               }
      }
    }
/* Check for same port spacing violation in wrap-around condition */
    TDM_PRINT3("param_cal_len=%0d, hp0[last_idx]=%0d, hp0[0]=%0d\n", param_cal_len, hp0[hp0_idx-1], hp0[0]);
    if ((param_cal_len-hp0[hp0_idx-1]+hp0[0])<6) {
      swap_idx_start = 6-(param_cal_len-hp0[hp0_idx-1]+hp0[0]);
      for (swap_idx=hp0[0]+swap_idx_start;swap_idx<hp1[0];swap_idx++) { /* Check between hp0's 1st index and hp1's 1st index */
        if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
              ((/*param_speeds[(param_cal_main[swap_idx])] == SPEED_25G ||*/param_speeds[(param_cal_main[swap_idx])] == SPEED_10G ||param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G )  && lr_ratio<3)) {
           param_cal_main[hp0[0]] = param_cal_main[swap_idx];
           param_cal_main[swap_idx] = param_token_ovsb;
           TDM_PRINT2("Swapping idx %0d with %0d\n", hp0[hp0_idx-1], swap_idx) ;
           break;
        }
      }
    }
}
tdm_hr4_tdm_print(hp1, hp1_idx);
if (hp1_num_os_port<2) {
    for (i=1;i<=hp1_idx-hp1_num_os_port;i++) {
      if (hp1[i*hp1_num_os_port] - hp1[i-1]<6) { /* Same port spacing violation  - fix required */
        TDM_PRINT2("Noticed violations between idx - %0d and %0d. need to space them.\n", hp1[i], hp1[i-1]) ;
	swap_idx_start = hp1[i*hp1_num_os_port] + 6 - (hp1[i*hp1_num_os_port]-hp1[i-1]);
               for(swap_idx=swap_idx_start; swap_idx<param_cal_len; swap_idx++) {
                  if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
			((/*param_speeds[(param_cal_main[swap_idx])] == SPEED_25G ||*/param_speeds[(param_cal_main[swap_idx])] == SPEED_10G ||param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G )  && lr_ratio<3)) {
                     param_cal_main[hp1[i*hp1_num_os_port]] = param_cal_main[swap_idx];
                     param_cal_main[swap_idx] = param_token_ovsb;
        TDM_PRINT2("Swapping idx %0d with %0d\n", hp1[i*hp1_num_os_port], swap_idx) ;
                     break;
                  }
               }
      }
    }
/* Check for same port spacing violation in wrap-around condition */
    if ((param_cal_len-hp1[hp1_idx-1]+hp1[0])<6) {
      swap_idx_start = 6-(param_cal_len-hp1[hp1_idx-1]+hp1[0]);
      for (swap_idx=hp1[0]+swap_idx_start;swap_idx<hp0[0];swap_idx++) { /* Check between hp0's 1st index and hp1's 1st index */
        if ((param_cal_main[swap_idx]==param_token_ancl) || (param_speeds[(param_cal_main[swap_idx])] == SPEED_1G) || 
              ((/*param_speeds[(param_cal_main[swap_idx])] == SPEED_25G ||*/param_speeds[(param_cal_main[swap_idx])] == SPEED_10G ||param_speeds[(param_cal_main[swap_idx])] == SPEED_2p5G )  && lr_ratio<3)) {
           param_cal_main[hp1[0]] = param_cal_main[swap_idx];
           param_cal_main[swap_idx] = param_token_ovsb;
           TDM_PRINT2("Swapping idx %0d with %0d\n", hp1[hp1_idx-1], swap_idx) ;
           break;
        }
      }
    }
}
TDM_FREE(hp0);
TDM_FREE(hp1);
return 1;

}


/**
@name: tdm_hr4_filter
@param:
 */
int
tdm_hr4_filter( tdm_mod_t *_tdm )
{
    int param_lr_en, param_os_en;
    
    param_lr_en  = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en  = _tdm->_core_data.vars_pkg.os_enable;
    /* TD3 doesn't support mixed LineRate and OverSub config - Helix5 supports!*/
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Filters Applied to smooth TDM calendar\n\n");
    TDM_SML_BAR
    
    /* Smooth Line-rate port jitter */
    tdm_hr4_filter_lr(_tdm);

    /* Smooth oversub port jitter */

    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        tdm_hr4_osub_filter(_tdm);
    }
    

    return (_tdm->_chip_exec[TDM_CHIP_EXEC__PARSE](_tdm));
}
