/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
	#include <tdm_ap_vec.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/tdm/apache/tdm_ap_vec.h>
#endif

int tdm_core_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports);

/**
@name: tdm_ap_ovs_spacer
@param:

Reconfigures oversub TDM calendar into Flexport compatibility mode
 */
void
tdm_ap_ovs_spacer(int *ovs_tdm_tbl, int *ovs_spacing)
{
	int i, j, w=0, k=0, port=0;
  int idx=0, size=AP_OS_LLS_GRP_LEN;
  unsigned short mirror_tdm_tbl[2*AP_OS_LLS_GRP_LEN];

 for(j=0; j<(2*AP_OS_LLS_GRP_LEN); j++) { mirror_tdm_tbl[j] = AP_NUM_EXT_PORTS;}
  
   do {
     idx++;
   } while(ovs_tdm_tbl[idx] != AP_NUM_EXT_PORTS);
   size =idx;

	for (i=0; i<size; i++) {
		mirror_tdm_tbl[i]        = ovs_tdm_tbl[i]; 
		mirror_tdm_tbl[i+size]   = ovs_tdm_tbl[i];
	}
	/* Populate PGW spacing table by calculating min spacing for each non-empty slots */
	for (j=0; j<size; j++) {
		w=size+1; 
	 	if(ovs_tdm_tbl[j]!=AP_NUM_EXT_PORTS){ 
      port = ovs_tdm_tbl[j]; 
			for (i=j+1; i<2*size; i++){
				if (mirror_tdm_tbl[i]== port){ 
					w=i-j;
					break;
				}
      }
		ovs_spacing[j] = w;
	}
}
	/* Regulate spacing values with AP_OS_LLS_GRP_LEN */
	for (k=0; k<AP_OS_LLS_GRP_LEN; k++) {
		if (ovs_spacing[k] == (size+1) || ovs_tdm_tbl[k] == AP_OVSB_TOKEN){
			ovs_spacing[k] = size;
		}
	}

}

/**
@name: tdm_ap_lls_scheduler
@param:

Populate a linked list for one quad by traversing ports lane by lane, and populate the PGW oversub calendar.
 */
int
tdm_ap_lls_scheduler(struct ap_ll_node *pgw_tdm, tdm_ap_chip_legacy_t *ap_chip, ap_pgw_pntrs_t *pntrs_pkg, ap_pgw_scheduler_vars_t *vars_pkg, int *pgw_tdm_tbl[AP_LR_LLS_LEN], int *ovs_tdm_tbl[AP_OS_LLS_GRP_LEN], int op_flags[3], int *ap_fb_num_ovs, int *ap_fb_num_lr)
{
	int ll_nodes_appended=0, p, q, r, l1_tdm_len, max_tdm_len, pgw_tdm_idx_sub, xcount, sm_iter=0, wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS],
		num_lr=0, num_lr_1=0, num_lr_10=0, num_lr_12=0, num_lr_20=0, num_lr_25=0, num_lr_40=0, num_lr_50=0, num_lr_100=0, num_ovs=0, num_off=0;
		
	for (p=0; p<AP_NUM_PHY_PM; p++) {
		for (q=0; q<4; q++) {
			wc_array[p][q]=AP_NUM_EXT_PORTS;
		}
	}
	for (p=0; p<AP_NUM_PHY_PM; p++) {
		memcpy(&(wc_array[p][0]), &((*ap_chip).pmap[p][0]), (4*sizeof(int)));
	}
	/* Evenly distribute slots within one TSC */
	for (p=0; p<AP_NUM_PHY_PM; p++) {
		AP_TOKEN_CHECK(wc_array[p][0]){
			if ((*ap_chip).tdm_globals.speed[(wc_array[p][0])]<SPEED_100G && wc_array[p][0]!=AP_NUM_EXT_PORTS && wc_array[p][0]==wc_array[p][1] && wc_array[p][1]!=wc_array[p][2]) {
				r=wc_array[p][2];
				wc_array[p][2]=wc_array[p][1];
				wc_array[p][1]=r;
			}
			else if ((*ap_chip).tdm_globals.speed[(wc_array[p][0])]<SPEED_100G && wc_array[p][2]!=AP_NUM_EXT_PORTS && wc_array[p][2]==wc_array[p][3] && wc_array[p][2]!=wc_array[p][1]) {
				r=wc_array[p][1];
				wc_array[p][1]=wc_array[p][2];
				wc_array[p][2]=r;
			}
		}
	}
	/* Set max number of TSCs in current quad */
	switch((*ap_chip).tdm_globals.clk_freq) {
		case 435:
			pgw_tdm_idx_sub = 3;
      if (((*vars_pkg).first_port == 0) && ((*vars_pkg).last_port == 36)) {max_tdm_len = 26;}
      else {max_tdm_len = 24;}
			break;
    case 510:
      if (((*vars_pkg).first_port == 0) && ((*vars_pkg).last_port == 36)) {pgw_tdm_idx_sub = 9;} 
			 else {pgw_tdm_idx_sub = 8;} 
      max_tdm_len = 30;
			break;
		case 575:
			pgw_tdm_idx_sub = 4;
      max_tdm_len = 34;
			break;
        /* Added 795 to 798 clk for FB config*/
		case 793:
		case 794:
		case 795:
		case 796:
		case 797:
		case 798:
			pgw_tdm_idx_sub = 6;
      max_tdm_len = 48;
			break;
		case 840:
			pgw_tdm_idx_sub = 6;
      max_tdm_len = 48;
			break;
		default:
			pgw_tdm_idx_sub = 7;
      max_tdm_len = 56;
			break;
	}
	
	/* Calculate number of linerate/oversub/disabled ports */
	for (p = (*vars_pkg).first_port; p < (*vars_pkg).last_port; p++) {
		/* linerate ports */
		if ( (*ap_chip).tdm_globals.port_rates_array[p] == PORT_STATE__LINERATE   ||
		     (*ap_chip).tdm_globals.port_rates_array[p] == PORT_STATE__LINERATE_HG){
			num_lr++;
			switch ((*ap_chip).tdm_globals.speed[p+1]) {
				case SPEED_1G:
				case SPEED_2G:
				case SPEED_2p5G:
					num_lr_1++;
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
						num_lr_10++;
					break;
				case SPEED_20G:
					num_lr_20++;
					break;
				case SPEED_25G:
					num_lr_25++;
					break;
				case SPEED_40G:
					num_lr_40++;
					break;
				case SPEED_50G:
					num_lr_50++;
					break;
				case SPEED_100G:
					num_lr_100++;
					break;
				default:
					break;
			}
		}
		/* oversub ports */
		else if ((*ap_chip).tdm_globals.port_rates_array[p] == PORT_STATE__OVERSUB   ||
		         (*ap_chip).tdm_globals.port_rates_array[p] == PORT_STATE__OVERSUB_HG){
			num_ovs++;
		}
		/* disabled ports */
		else if ((*ap_chip).tdm_globals.port_rates_array[p] == PORT_STATE__DISABLED || ( (p%4==0) && ((*ap_chip).tdm_globals.port_rates_array[p]==PORT_STATE__COMBINE) ) ) {
			num_off++;
		}
	}
	/* Check length of PGW linked list for current quad */
  l1_tdm_len = (num_lr_1+(num_lr_10*2)+(num_lr_12*3)+(num_lr_20*4)+(num_lr_25*5)+(num_lr_40*8)+(num_lr_50*10)+(num_lr_100*20));
	TDM_SML_BAR
	if ( (l1_tdm_len>max_tdm_len) && (l1_tdm_len>64) ) {
		TDM_ERROR0("Bandwidth overloaded\n");
		return 0;
	}
	TDM_PRINT1("Base oversubscription token pool is %0d\n", pgw_tdm_idx_sub);
	TDM_SML_BAR
	TDM_PRINT1("1G line rate ports in this quadrant: %0d\n", num_lr_1);
	TDM_PRINT1("10G line rate ports in this quadrant: %0d\n", num_lr_10);
	TDM_PRINT1("12G line rate ports in this quadrant: %0d\n", num_lr_12);
	TDM_PRINT1("20G line rate ports in this quadrant: %0d\n", num_lr_20);
	TDM_PRINT1("25G line rate ports in this quadrant: %0d\n", num_lr_25);
	TDM_PRINT1("40G line rate ports in this quadrant: %0d\n", num_lr_40);
	TDM_PRINT1("50G line rate ports in this quadrant: %0d\n", num_lr_50);
	TDM_PRINT1("100G gestalt line rate ports in this quadrant: %0d\n", num_lr_100);
	TDM_PRINT1("Oversubscribed ports in this quadrant: %0d\n", num_ovs);
	TDM_PRINT1("Disabled subports in this quadrant: %0d\n", num_off);
	TDM_PRINT2("COVERAGE FROM PHYSICAL PORTS %0d to %0d\n", (*vars_pkg).first_port, (*vars_pkg).last_port);
        (*ap_fb_num_ovs) = num_ovs;
        (*ap_fb_num_lr) = num_lr_40;
	
	/* CASE 1: line-rate bandwidth overflow */
	if ((l1_tdm_len>max_tdm_len) && (num_ovs == 0)) {
		TDM_PRINT0("Applying obligate oversubscription correction\n");
		for (xcount=0; xcount<max_tdm_len; xcount++) {
			while (((*pntrs_pkg).cur_idx < (*vars_pkg).cur_idx_max) && 
				   ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport] == AP_NUM_EXT_PORTS) ||
					(wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport] == AP_MGMT_TOKEN)     ) ){	
				((*pntrs_pkg).cur_idx)++;
			}
			if ((*pntrs_pkg).cur_idx >= (*vars_pkg).cur_idx_max) {
				break;
			}
			if ( tdm_ap_ll_len(pgw_tdm) < max_tdm_len ) {
				tdm_ap_ll_append(pgw_tdm,AP_OVSB_TOKEN,&ll_nodes_appended);
				TDM_PRINT2("PGW %0d: PGW TDM calendar element #0%0d, content is OVSB token\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended));
			}
			(*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
			TDM_PRINT3("PGW %0d: Oversub subcalendar index %0d is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(*pntrs_pkg).ovs_tdm_idx,(*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx]);
			((*pntrs_pkg).ovs_tdm_idx)++;
			((*pntrs_pkg).cur_idx)++;
		}
		TDM_WARN3("PGW %0d: PGW TDM calendar, Line-rate bandwidth overflow (%d,%d). \n", (((*vars_pkg).cur_idx_max/9)-1), l1_tdm_len, max_tdm_len);
	}
	/* CASE 2: line-rate bandwidth is within limit */
	else {
		while ( (sm_iter++) < AP_NUM_EXT_PORTS) {
			/* Find next active port (either linerate or oversub) */
			while ( ((*pntrs_pkg).cur_idx < (*vars_pkg).cur_idx_max) && 
			        ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport] == AP_MGMT_TOKEN)    || 
					 (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport] == AP_NUM_EXT_PORTS) ||
					 ((*ap_chip).tdm_globals.port_rates_array[(wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport])-1]==PORT_STATE__MANAGEMENT) ||
           (((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] < SPEED_10G ) && op_flags[1]==1)) ){
				((*pntrs_pkg).cur_idx)++;
			}
			if ( ( ((*pntrs_pkg).cur_idx)>=((*vars_pkg).cur_idx_max)) ) {
				break;
			}
			/* Print selected active port info */
			TDM_SML_BAR
			TDM_PRINT2("Polling current index %0d subport %0d: [",(*pntrs_pkg).cur_idx,(*vars_pkg).subport);
			for (p=0; p<4; p++) {
				if (p==(*vars_pkg).subport) {
					TDM_PRINT1(" >%0d< ",wc_array[(*pntrs_pkg).cur_idx][p]);
				}
				else {
					TDM_PRINT1(" %0d ",wc_array[(*pntrs_pkg).cur_idx][p]);
				}
			}
			TDM_PRINT0("]\n");
			TDM_PRINT3("[ Port=%0d, Speed=%0dG, State=%0d ]\n", wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],(*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]]/1000,(*ap_chip).tdm_globals.port_rates_array[(wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport])-1]);
			/* Allocate one slot for selected port, either in linked-list or in stack */
			switch((*ap_chip).tdm_globals.port_rates_array[(wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport])-1]) {
				case PORT_STATE__DISABLED:
					TDM_PRINT0("The current port is DISABLED\n");
					if (num_lr > 0 && num_ovs > 0) {
							tdm_ap_ll_append(pgw_tdm,AP_OVSB_TOKEN,&ll_nodes_appended);
							TDM_PRINT3("PGW %0d: PGW TDM calendar element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1)));
					}
					((*pntrs_pkg).cur_idx)++;
					break;
				case PORT_STATE__LINERATE:
				case PORT_STATE__LINERATE_HG:
					TDM_PRINT0("The current port is LINE RATE\n");
					if (num_ovs==0) {
						while ( (((*pntrs_pkg).tdm_stk_idx)>0) && (tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1))!=(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]) ) {
							tdm_ap_ll_append(pgw_tdm,(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx],&ll_nodes_appended);
							TDM_PRINT4("PGW %0d: Dequeue from stack, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(*pntrs_pkg).tdm_stk_idx,(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]);
							((*pntrs_pkg).tdm_stk_idx)--;
							TDM_PRINT1("Swap stack pointer at %0d\n",(*pntrs_pkg).tdm_stk_idx);
						}
						if ( (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]!=tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1)) ) || (ll_nodes_appended==0) ) {
							/*tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);*/
              if( (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G ) {
                if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=1  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=28) ||
                    (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=37  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=64)) &&
                   ( (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G ) ) {
                  if(op_flags[1] ==0){
                    tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);  TDM_PRINT0("DPRT1\n");
                    if((*vars_pkg).subport==3)  { tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended); TDM_PRINT0("DPRT\n");}
                  }
                }
                if( ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                     (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) ) {
                  tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
                }
              } else {
                tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
              }
              if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                  (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) &&
                 ((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_40G ||
                  (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_20G) ) {
							    tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
              }
   /*Falcon core*/
              if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                  (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) &&
                 ((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_100G ||
                  (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_50G ||
                  (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G) ) {
                    tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
                    if(op_flags[1]==0) tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
              }

							TDM_PRINT3("PGW %0d: PGW TDM calendar element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1)));
							((*pntrs_pkg).cur_idx)++;
						}
						else {
							((*pntrs_pkg).tdm_stk_idx)++;
							(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
							TDM_PRINT3("PGW %0d: pushed port %0d at stack index %0d\n",(((*vars_pkg).cur_idx_max/9)-1),wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],(*pntrs_pkg).tdm_stk_idx);
							((*pntrs_pkg).cur_idx)++;
						}
					}
					else {
						if ( (((*pntrs_pkg).tdm_stk_idx)>0) &&
							 (tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-2))!=(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]) &&
							 (tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1))==AP_OVSB_TOKEN) ) {
							tdm_ap_ll_append(pgw_tdm,(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx],&ll_nodes_appended);
							TDM_PRINT4("PGW %0d: Dequeue from stack, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(*pntrs_pkg).tdm_stk_idx,(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]);
							((*pntrs_pkg).tdm_stk_idx)--;
							TDM_PRINT1("Swap stack pointer at %0d\n",(*pntrs_pkg).tdm_stk_idx);
						}
						while ( (((*pntrs_pkg).tdm_stk_idx)>0) &&
								(tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1))!=(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]) &&
								(tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1))!=AP_OVSB_TOKEN) ) {
							tdm_ap_ll_append(pgw_tdm,(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx],&ll_nodes_appended);
							TDM_PRINT4("PGW %0d: Dequeue from stack, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(*pntrs_pkg).tdm_stk_idx,(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]);
							((*pntrs_pkg).tdm_stk_idx)--;
							TDM_PRINT1("Swap stack pointer at %0d\n",(*pntrs_pkg).tdm_stk_idx);
						}
						if ( ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]!=tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1))) || (ll_nodes_appended==0)) ) {
                                                  if( (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G ) {
                                                    if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=1  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=28) ||
                                                        (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=37  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=64)) &&
                                                       ( (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G ) ) {
                                                      if(op_flags[1] ==0){
                                                        tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);  TDM_PRINT0("DPRT1\n");
                                                        if((*vars_pkg).subport==3)  { tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended); TDM_PRINT0("DPRT\n");}
                                                      }
                                                    }
                                                    if( ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                                                         (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) ) {
                                                      tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
                                                    }
                                                  } else {
                                                    tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
                                                  }

              if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=35) ||
                  (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=71)) &&
                 ((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_40G ||
                  (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_20G) ) {
							    tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
              }
   /*Falcon core*/
              if( ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                  (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) &&
                  ((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_100G ||
                 (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_50G   ||
                 (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G) ) {
							    tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
							    if(op_flags[1]==0) tdm_ap_ll_append(pgw_tdm,wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],&ll_nodes_appended);
              }

							TDM_PRINT3("PGW %0d: PGW TDM calendar element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1)));
							((*pntrs_pkg).cur_idx)++;
						}
						else {
							((*pntrs_pkg).tdm_stk_idx)++;
							(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
							TDM_PRINT3("PGW %0d: pushed port %0d at stack index %0d\n",(((*vars_pkg).cur_idx_max/9)-1),wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport],(*pntrs_pkg).tdm_stk_idx);
							((*pntrs_pkg).cur_idx)++;
						}
					}
					break;
				case PORT_STATE__OVERSUB:
				case PORT_STATE__OVERSUB_HG:
					TDM_PRINT0("The current port is OVERSUBSCRIBED\n");
					TDM_PRINT1("The oversub subcalendar index is %0d\n", (*pntrs_pkg).ovs_tdm_idx);
					if(op_flags[1]==0) {
              /*Apache 25.455G Eagle support (4x6.5625)*/
              if( (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G ) {
                if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=1  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=28) ||
                    (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=37  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=64)) &&
                   ( (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G ) ) {
                   if( (*vars_pkg).subport <3) {
                     (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
	             ((*pntrs_pkg).ovs_tdm_idx)++;
                   }
                }
                if( ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                     (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) ) {
                    (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
                    ((*pntrs_pkg).ovs_tdm_idx)++;
                }
              } else {
                 (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
					  ((*pntrs_pkg).ovs_tdm_idx)++;
              }

              if(((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=35) ||
                  (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=71)) &&
                 ((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_40G ||
                  (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_20G) ) {
                 (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
					       ((*pntrs_pkg).ovs_tdm_idx)++;
              }

            if( ((wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=29  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=36) ||
                 (wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]>=65  && wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]<=72)) &&
                ((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_100G ||
                 (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_50G   ||
                 (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G) ) {
                 (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
					       ((*pntrs_pkg).ovs_tdm_idx)++;
             
              if((*ap_chip).tdm_globals.clk_freq!=840) {
                 if((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_100G || 
                    (*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_50G) {  
                   if((*vars_pkg).subport <2) {
                       (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
					             ((*pntrs_pkg).ovs_tdm_idx)++;
                   }
                 }
                 else if((*ap_chip).tdm_globals.speed[wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport]] == SPEED_25G) {  
                       (*ovs_tdm_tbl)[(*pntrs_pkg).ovs_tdm_idx] = wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport];
					             ((*pntrs_pkg).ovs_tdm_idx)++;
                }
              }
            }
          }
					if (((*vars_pkg).subport!=0) && 
					    ((*ap_chip).tdm_globals.port_rates_array[(wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport-1])-1]==PORT_STATE__LINERATE ||
					     (*ap_chip).tdm_globals.port_rates_array[(wc_array[(*pntrs_pkg).cur_idx][(*vars_pkg).subport-1])-1]==PORT_STATE__LINERATE_HG) ){
							tdm_ap_ll_append(pgw_tdm,AP_OVSB_TOKEN,&ll_nodes_appended);
							TDM_PRINT3("PGW %0d: PGW TDM calendar element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1)));
					}
					((*pntrs_pkg).cur_idx)++;
					break;
				default:
					break;
			}
		}
		/* Dequeue linerate slot from stack */
		while ((*pntrs_pkg).tdm_stk_idx>0) {
			tdm_ap_ll_append(pgw_tdm,(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx], &ll_nodes_appended);
			TDM_PRINT4("PGW %0d: Dequeue from stack, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(*pntrs_pkg).tdm_stk_idx,(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),(*vars_pkg).swap_array[(*pntrs_pkg).tdm_stk_idx]);
			((*pntrs_pkg).tdm_stk_idx)--;
			TDM_PRINT1("Swap stack pointer at %0d\n",(*pntrs_pkg).tdm_stk_idx);
		}
		/* Extend linked-list with oversub token */
		if ( (ll_nodes_appended>0) || 
		     (ll_nodes_appended==0 && ((*pntrs_pkg).ovs_tdm_idx)>0) ){
			while ( ll_nodes_appended<pgw_tdm_idx_sub ) {
				TDM_SML_BAR
				TDM_PRINT1("Extending current cadence at index %0d\n",tdm_ap_ll_len(pgw_tdm));
				tdm_ap_ll_append(pgw_tdm,AP_OVSB_TOKEN,&ll_nodes_appended);
				TDM_PRINT3("PGW %0d: PGW TDM calendar element #0%0d, content is %0d\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended),tdm_ap_ll_get(pgw_tdm,(tdm_ap_ll_len(pgw_tdm)-1)));
			}
		}
  	if(op_flags[2] ==1) {
     while ( tdm_ap_ll_len(pgw_tdm) < max_tdm_len ) {
				tdm_ap_ll_append(pgw_tdm,AP_OVSB_TOKEN,&ll_nodes_appended);
				TDM_PRINT2("PGW %0d: PGW TDM calendar element #0%0d, content is OVSB token\n",(((*vars_pkg).cur_idx_max/9)-1),(((*vars_pkg).subport*pgw_tdm_idx_sub)+ll_nodes_appended));
			}
	  }
	}
	
	TDM_PRINT0("\n\tCalendar dump:\n");
	tdm_ap_ll_print(pgw_tdm);
	TDM_SML_BAR
	TDM_SML_BAR
	TDM_PRINT1("Current cycle: The PGW TDM calendar index is %0d\n",tdm_ap_ll_len(pgw_tdm));
	TDM_PRINT1("Current cycle: %0d ll_nodes appended to PGW TDM linked list\n",ll_nodes_appended);
	TDM_PRINT2("Current cycle: The current index is %0d at subport %0d\n",(*pntrs_pkg).cur_idx,(*vars_pkg).subport);
	TDM_SML_BAR
	TDM_SML_BAR
	
	return ll_nodes_appended;
}


/**
OVSB TBL RECONFIG WHEN CLPORTS in 100G 
 */
void tdm_ap_reconfig_ovs_tbl(int *ovs_tdm_tbl)
{

int i,j,k,idx,size=AP_OS_LLS_GRP_LEN;
int elmnt_repeating,elmnt_repeating_cnt=0, num_elements_repeating=0, rpt_slot0, rpt_slot1, rpt_slot2;
int uniq_elements_cnt=0, sum_of_spacing=0, current_spacing=0;
int temp, spacing_idx=0, timeout=16, temp_port=130;

int spacing_tbl[AP_OS_LLS_GRP_LEN],elmnt_repeating_tbl[AP_OS_LLS_GRP_LEN],uniq_elements_tbl[AP_OS_LLS_GRP_LEN];


  for(j=0; j<AP_OS_LLS_GRP_LEN; j++) { spacing_tbl[j] = 0;}
  
  for(i =0; i<AP_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == AP_NUM_EXT_PORTS) break;
   }
  size = i;
  
  for(i=0; i<size; i++) {
    AP_TOKEN_CHECK(ovs_tdm_tbl[i]) {
  	 if(ovs_tdm_tbl[i]==ovs_tdm_tbl[i+1]) {
  		 elmnt_repeating_tbl[num_elements_repeating++] = ovs_tdm_tbl[i];
  		 elmnt_repeating = ovs_tdm_tbl[i];
  		 for(j=0; j<size; j++) {
  			 if(ovs_tdm_tbl[j] == elmnt_repeating_tbl[num_elements_repeating-1]) {
  				 for(k=j-1; k>=0; k--){
  					 ovs_tdm_tbl[k+1] = ovs_tdm_tbl[k];
  				 }
  				 elmnt_repeating_cnt++;
  			   ovs_tdm_tbl[0] = elmnt_repeating;
  			 }
  		 }
  		 i = elmnt_repeating_cnt;
  	 }
   }
 }
   
  /*  move all ovsb tokens to end */
	while((--timeout)>0) {
		for (i=0; i<(size-1); i++) {
			if (ovs_tdm_tbl[i]==AP_OVSB_TOKEN && ovs_tdm_tbl[i+1]!=AP_OVSB_TOKEN) {
        temp_port=ovs_tdm_tbl[i+1];
        ovs_tdm_tbl[i+1]=ovs_tdm_tbl[i];
        ovs_tdm_tbl[i]=temp_port;
			}
		}
	}

  for(i=elmnt_repeating_cnt; i<size; i++) {
    AP_OVS_TOKEN_CHECK(ovs_tdm_tbl[i]) {
  	uniq_elements_tbl[uniq_elements_cnt++] = ovs_tdm_tbl[i];
    }
  }

  /*if no unique elements*/
   if(uniq_elements_cnt ==0 || uniq_elements_tbl[0] ==AP_OVSB_TOKEN)  {
     if(num_elements_repeating == 3) {
       rpt_slot0=ovs_tdm_tbl[0]; rpt_slot1=ovs_tdm_tbl[10]; rpt_slot2=ovs_tdm_tbl[20];
       for(i=0; i<(size-4); i=i+3) {
          ovs_tdm_tbl[i]   = rpt_slot0;
          ovs_tdm_tbl[i+1] = rpt_slot1;
          ovs_tdm_tbl[i+2] = rpt_slot2;
       }
     }
     else if (num_elements_repeating ==2) {
       rpt_slot0=ovs_tdm_tbl[0]; rpt_slot1=ovs_tdm_tbl[10];
       for(i=0; i<(size-4); i++) {
         if(i%2==0) ovs_tdm_tbl[i] = rpt_slot0;
         else       ovs_tdm_tbl[i] = rpt_slot1;
       }
     }
   } 
   else if (elmnt_repeating_cnt!=0) {   
     do	{
    	  for(i=0; i<elmnt_repeating_cnt/num_elements_repeating; i++) {
    		spacing_tbl[i]++;
    		sum_of_spacing++;
    		   if(sum_of_spacing == uniq_elements_cnt) break;
      	   }
     } while (sum_of_spacing < uniq_elements_cnt);
   /* make the spacing table symmetric*/
    do {
     spacing_idx++;
    } while(spacing_tbl[spacing_idx] != 0);

   for(i=0; i<spacing_idx; i++) {
      temp= spacing_tbl[spacing_idx-1];   
      for(k=spacing_idx-1; k>i; k--) { spacing_tbl[k] = spacing_tbl[k-1];}
      spacing_tbl[i] = temp;
      i=i+1;
   }
     
   /* put the repeating elements at start of tbl*/ 
     for(k=0, j=0; k<num_elements_repeating; k++) {
    		ovs_tdm_tbl[j++] = elmnt_repeating_tbl[k];
     }
    
     for(i=0, current_spacing=0, idx=0 ; i<uniq_elements_cnt; ) {
    	  if(current_spacing==spacing_tbl[idx]) {
    	 	 current_spacing = 0;
    		 idx++;
    		  for(k=0; k<num_elements_repeating; k++) {
    			ovs_tdm_tbl[j++] = elmnt_repeating_tbl[k];
    	 	  }
    	  }
    	  else { ovs_tdm_tbl[j++]=uniq_elements_tbl[i]; current_spacing++; i++; }
     }
   }


}

/**
 */
void tdm_ap_reconfig_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2)
{

int i, j, idx=0, size=AP_OS_LLS_GRP_LEN;

  do {
     idx++;
   } while(ovs_tdm_tbl[idx] != AP_NUM_EXT_PORTS);
  
    ovs_tdm_tbl[idx] = port1;
    ovs_tdm_tbl[idx+1] = port1;
    ovs_tdm_tbl[idx+2] = port2;
    ovs_tdm_tbl[idx+3] = port2;

  for(i=0; i<AP_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == AP_NUM_EXT_PORTS) break;
   }
  size = i;

   for(i=0;i<size;i++) {
     for(j=0;j<4;j++) {
  	   if(ovs_tdm_tbl[i]==(port1+j)) {
         ovs_tdm_tbl[i] = port1;
       }
  	   if(ovs_tdm_tbl[i]==(port2+j)) {
         ovs_tdm_tbl[i] = port2;
       }
     }
   }
 }

/**
 */
void tdm_ap_append_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2, int cxx_port, enum port_speed_e cxx_speed )
{

int i, cnt=0, cnt1=0, cnt2=0;
int token1=port1, token2=port2;

   if(cxx_speed == SPEED_100G) {
     for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
       if( ovs_tdm_tbl[i]>=cxx_port && ovs_tdm_tbl[i] <(cxx_port+10) ) {
         ovs_tdm_tbl[i] = cxx_port;
         if((cnt2++)==9) break;
       }
     }
     for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
       if( ovs_tdm_tbl[i]>cxx_port && ovs_tdm_tbl[i] <(cxx_port+10) ) {
         ovs_tdm_tbl[i] = (ovs_tdm_tbl[i] % 2)? port2 : port1;
       }
     }
   }
  for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
     if(ovs_tdm_tbl[i]== port1) {
       ovs_tdm_tbl[i] = token1++;
       if((cnt++) ==3) { 
         cnt =0;
         token1=port1;
       }
     }
     if(ovs_tdm_tbl[i]== port2) {
       ovs_tdm_tbl[i] = token2++;
       if((cnt1++) ==3) { 
         cnt1 =0;
         token2=port2;
       }
     }
   }

 }
/*
 

 */

void tdm_ap_clport_ovs_scheduler(int *ovs_tdm_tbl, int port[2], enum port_speed_e speed[2], int cxx_port, enum port_speed_e cxx_speed, int clk )
{

int i, j, idx=0, cnt=0, size=AP_OS_LLS_GRP_LEN;
int token = cxx_port;

  do {
     idx++;
  } while(ovs_tdm_tbl[idx] != AP_NUM_EXT_PORTS);
  
/*append additional slots for ovs */
if(clk!=840) {
  for(i=0; i<2; i++) {
    if(speed[i] == SPEED_100G || speed[i] == SPEED_50G) {
      ovs_tdm_tbl[idx++] = AP_OVSB_TOKEN;
      ovs_tdm_tbl[idx++] = AP_OVSB_TOKEN;
    }
  }
}
   for(i=0; i<AP_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == AP_NUM_EXT_PORTS) break;
   }
  size = i;

  if ((speed[0] == SPEED_25G) && (speed[1]== SPEED_25G)) {
    for(i=0;i<size;i++) {
      for(j=0;j<4;j++) {
  	    if(ovs_tdm_tbl[i]==(port[0]+j)) {
          ovs_tdm_tbl[i] = port[0];
        }
  	    if(ovs_tdm_tbl[i]==(port[1]+j)) {
          ovs_tdm_tbl[i] = port[1];
        }
      }
    }
   /* when all 3 cl ports are not in same speed */
    if(cxx_speed == SPEED_100G) {
      for(i=0;i<size;i++) {
  	    if(ovs_tdm_tbl[i]==cxx_port) {
          ovs_tdm_tbl[i] = token++;
        }
      }
    }
  }
  else if(speed[0] == SPEED_25G) { 
    for(i=0;i<size;i++) {
      for(j=0;j<4;j++) {
  	    if(ovs_tdm_tbl[i]==(port[0]+j)) {
          ovs_tdm_tbl[i] = port[0];
        }
      }
    }
   tdm_ap_scheduler_ovs_4x25(ovs_tdm_tbl,port[0]); /* first schedule 4x25 alone*/
  }
  else if(speed[1] == SPEED_25G) { 
    for(i=0;i<size;i++) {
      for(j=0;j<4;j++) {
  	    if(ovs_tdm_tbl[i]==(port[1]+j)) {
          ovs_tdm_tbl[i] = port[1];
        }
      }
    }
   tdm_ap_scheduler_ovs_4x25(ovs_tdm_tbl,port[1]);
  }
/*make 2x50GE as 1x100GE and the replace after scheduling*/
  if (speed[0] == SPEED_50G) {  
    for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
      if(ovs_tdm_tbl[i]== port[0]+2) {
			  ovs_tdm_tbl[i] = port[0];
      }
    }
  }
  if (speed[1] == SPEED_50G) {  
    for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
      if(ovs_tdm_tbl[i]== port[1]+2) {
			  ovs_tdm_tbl[i] = port[1];
      }
    }
  }

  /* main spacing logic */
  tdm_ap_reconfig_ovs_tbl(ovs_tdm_tbl); 
  
  if (speed[0] == SPEED_50G) {  
    for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
      if(ovs_tdm_tbl[i]== port[0]) {
        if ((cnt++)%2==0) { ovs_tdm_tbl[i] = port[0] ; }
				else { ovs_tdm_tbl[i] = (port[0]+2); }
      }
    }
  }
  if (speed[1] == SPEED_50G) {
    for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
      if(ovs_tdm_tbl[i]== port[1]) {
        if ((cnt++)%2==0) { ovs_tdm_tbl[i] = port[1] ; }
				else { ovs_tdm_tbl[i] = (port[1]+2); }
      }
    }
  }
  if ((speed[0] == SPEED_25G) && (speed[1]== SPEED_25G)) { tdm_ap_append_ovs_8x25(ovs_tdm_tbl,port[0],port[1],cxx_port,cxx_speed);}

  
}

   
void tdm_ap_scheduler_ovs_4x25(int *ovs_tdm_tbl, int port)
{

int i, j, k,cnt=0,div=0, cnt1=0, size=AP_OS_LLS_GRP_LEN;
int uniq_elements_tbl[AP_OS_LLS_GRP_LEN];
int token=port;

 for(j=0; j<AP_OS_LLS_GRP_LEN; j++) { uniq_elements_tbl[j] = AP_NUM_EXT_PORTS;}

   for(i=0; i<AP_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == AP_NUM_EXT_PORTS) break;
   }
   size = i;

    for(i=0,j=0; i<size; i++) {
  	   if(ovs_tdm_tbl[i]!=port) { uniq_elements_tbl[j++] = ovs_tdm_tbl[i]; }
       else cnt++;
    }
    div=cnt;
   if (cnt != 0) {
      for(i= ((size/div)-1); i<size; i+= (size/div) ) {
        for(k=size-1; k>=i; k--){
         uniq_elements_tbl[k] = uniq_elements_tbl[k-1];
    		}
        uniq_elements_tbl[i]=port;
  			if ((--cnt)<=0) {
  					break;
  			}
      }
  
     for (i=0; i<size; i++) {
       if(uniq_elements_tbl[i]== port) {
         uniq_elements_tbl[i] = token++;
         if((cnt1++) ==3) { 
           cnt1 =0;
           token=port;
         }
       }
     }
   }
 TDM_COPY(ovs_tdm_tbl,uniq_elements_tbl,sizeof(int)*AP_OS_LLS_GRP_LEN); 
}

 
/**
@name: tdm_postalloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to load time vectors into TDM vector map
 */
/*
int
tdm_postalloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports)
{
	int v, load_status;
	
	TDM_BIG_BAR
	TDM_PRINT1("%sG scheduling pass\n",speed);
	TDM_SML_BAR
	TDM_PRINT0("Stack contains: [ ");
	for (v=*y; v>0; v--) {
		if (lr_stack[v]!=num_ext_ports) {
			TDM_PRINT1(" %0d ",lr_stack[v]);
		}
	}
	TDM_PRINT0(" ]\n");
	TDM_SML_BAR
	while (*y > 0) {
		load_status = tdm_ap_vec_load(vector_map, freq, spd, *yy, lr_idx_limit, num_ext_ports);
		if (load_status==FAIL) {
			TDM_ERROR2("Failed to load %sG vector for port %0d\n",speed,lr_stack[*y]);
			return FAIL;
		}
		else {
			for (v=0; v<VEC_MAP_LEN; v++) {
				if (vector_map[*yy][v]==token) {
					vector_map[*yy][v]=lr_stack[*y];
				}
			}
			TDM_PRINT4("Loaded %sG port %d vector from stack pointer %d into map at index %0d\n",speed, lr_stack[*y], *y, *yy);
		}
		(*y)--; (*yy)++;
		if ((*y)>0) {
			TDM_SML_BAR
			TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", *yy, *y);
			TDM_SML_BAR
		}
		else {
			TDM_SML_BAR
			TDM_PRINT2("Done loading %sG vectors, map pointer floating at: %0d\n",speed, *yy);
			TDM_SML_BAR
		}
	}
	TDM_BIG_BAR
	
	return PASS;
	
}

int
tdm_acc_alloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, int lr_idx_limit, int num_ext_ports)
{
	int load_status;
	
	TDM_BIG_BAR
	TDM_PRINT0("ACC BW scheduling pass\n");
	TDM_SML_BAR

		load_status = tdm_ap_vec_load(vector_map, freq, spd, *yy, lr_idx_limit, num_ext_ports);
		if (load_status==FAIL) {
			TDM_ERROR1("Failed to load acc_bw vectors for acc ports %0d\n",AP_ANCL_TOKEN);
			return FAIL;
		}
    (*yy)++;
			
      TDM_PRINT1("Done loading acc_bw vectors for acc ports, map pointer floating at: %0d\n",*yy);
	TDM_BIG_BAR
	
	return PASS;
	
}

*/
int 
tdm_ap_actual_pmap( tdm_mod_t *_tdm )
{
	int i; 
	
	if (_tdm==NULL) {return FAIL;}

    for (i=1; i<=AP_NUM_PHY_PORTS; i+=AP_NUM_PM_LNS) {
      /* CLPORTS PM4x25 in dual port mode */
      if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_50G && _tdm->_chip_data.soc_pkg.speed[i+2]==SPEED_50G) {
          _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = i+2;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][3] = i+2;
      }
      /* CLPORTS PM4x25 in tri-port mode x-xx or xx-x */
      if(i==AP_CL1_PORT || i==AP_CL2_PORT || i==AP_CL4_PORT || i==AP_CL5_PORT ){
				if ( _tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3] && _tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0 ) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2];
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][0];
				}
				else if ( _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] && _tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0 )  {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1];
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][3];
				}
      }
    }

return PASS;
}


/**
@name: tdm_ap_vmap_alloc
@param:

Populate original vmap containing all linerate ports with matched speed rates accordingly
 */
int
tdm_ap_vmap_alloc( tdm_mod_t *_tdm )
{
	int j, v, w, clk_freq, vmap_port_spd, cal_length;
	unsigned short lr1[TDM_AUX_SIZE], lr10[TDM_AUX_SIZE], lr20[TDM_AUX_SIZE], lr25[TDM_AUX_SIZE], lr40[TDM_AUX_SIZE], lr50[TDM_AUX_SIZE], lr100[TDM_AUX_SIZE];
	int speed_type_num,
        num_120g, num_100g, num_50g, num_40g, 
        num_25g, num_20g, num_10g, num_1g;
	int param_vmap_wid, param_vmap_len;
    
    param_vmap_wid = _tdm->_core_data.vmap_max_wid;
    param_vmap_len = _tdm->_core_data.vmap_max_len;
    
	/* Initialize speed group arrays for both linerate and oversub */
	for (j=0; j<(TDM_AUX_SIZE); j++) {
		lr1[j]  = AP_NUM_EXT_PORTS;
		lr10[j] = AP_NUM_EXT_PORTS;
		lr20[j] = AP_NUM_EXT_PORTS;  
		lr25[j] = AP_NUM_EXT_PORTS; 
		lr40[j] = AP_NUM_EXT_PORTS;
		lr50[j] = AP_NUM_EXT_PORTS; 
		lr100[j]= AP_NUM_EXT_PORTS;
		
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[j]  = AP_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[j] = AP_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[j] = AP_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[j] = AP_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[j] = AP_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[j] = AP_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[j]= AP_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120[j]= AP_NUM_EXT_PORTS;
	}	
	clk_freq  = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_length= _tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size;
	
	for (j=0; j<TDM_AUX_SIZE; j++) {
		/* Pre-allocate linerate ports with the same speed into the same linerate speed group */
		if (_tdm->_core_data.vars_pkg.lr_buffer[j] != AP_NUM_EXT_PORTS){
			if (_tdm->_core_data.vars_pkg.lr_buffer[j] <= AP_NUM_PHY_PORTS){
				if (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE   || 
					_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE_HG){
					switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]]) {
						case SPEED_1G:
						case SPEED_2p5G:
							tdm_core_prealloc(lr1, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), &(_tdm->_core_data.vars_pkg.lr_1), j);
							break;
						case SPEED_10G:
						case SPEED_11G:
						case SPEED_10G_DUAL:
						case SPEED_10G_XAUI:
							tdm_core_prealloc(lr10, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), &(_tdm->_core_data.vars_pkg.lr_10), j);
							break;
						case SPEED_20G:
						case SPEED_21G:
						case SPEED_21G_DUAL:
							tdm_core_prealloc(lr20, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), &(_tdm->_core_data.vars_pkg.lr_20), j);
							break;
						case SPEED_25G:
						case SPEED_27G:
							tdm_core_prealloc(lr25, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), &(_tdm->_core_data.vars_pkg.lr_25), j);
							break;
						case SPEED_40G:
						case SPEED_42G:
						case SPEED_42G_HG2:
							tdm_core_prealloc(lr40, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), &(_tdm->_core_data.vars_pkg.lr_40), j);
							break;
						case SPEED_50G:
						case SPEED_53G:
							tdm_core_prealloc(lr50, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), &(_tdm->_core_data.vars_pkg.lr_50), j);
							break;
						case SPEED_100G:
						case SPEED_106G:
							tdm_core_prealloc(lr100, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), &(_tdm->_core_data.vars_pkg.lr_100), j);
							break;				
						default:
							TDM_ERROR2("Speed %0d port %0d was skipped in linerate preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]],_tdm->_core_data.vars_pkg.lr_buffer[j]);
							break;
					}
				}
				else{
					TDM_ERROR2("Failed to presort linerate port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.lr_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1]);
				}
			}
			else{
				TDM_ERROR1("Unrecognized linerate port %0d in linerate preallocation\n", _tdm->_core_data.vars_pkg.lr_buffer[j]);
			}
		}
		/* Pre-allocate oversub ports with the same speed into the same ovesub speed group */
		if (_tdm->_core_data.vars_pkg.os_buffer[j] != AP_NUM_EXT_PORTS){
			if(_tdm->_core_data.vars_pkg.os_buffer[j] <= AP_NUM_PHY_PORTS){
				if (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB   || 
					_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB_HG){
					switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]]) {
						case SPEED_1G:
						case SPEED_2p5G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8), &(_tdm->_core_data.vars_pkg.os_1), j);
							break;
						case SPEED_10G:
						case SPEED_11G:
						case SPEED_10G_DUAL:
						case SPEED_10G_XAUI:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1), &(_tdm->_core_data.vars_pkg.os_10), j);
							break;
						case SPEED_20G:
						case SPEED_21G:
						case SPEED_21G_DUAL:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2), &(_tdm->_core_data.vars_pkg.os_20), j);
							break;
						case SPEED_25G:
						case SPEED_27G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6), &(_tdm->_core_data.vars_pkg.os_25), j);
							break;	
						case SPEED_40G:
						case SPEED_42G:
						case SPEED_42G_HG2:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3), &(_tdm->_core_data.vars_pkg.os_40), j);
							break;
						case SPEED_50G:
						case SPEED_53G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5), &(_tdm->_core_data.vars_pkg.os_50), j);
							break;
						case SPEED_100G:
						case SPEED_106G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4), &(_tdm->_core_data.vars_pkg.os_100), j);
							break;
						default:
							TDM_ERROR2("Speed %0d port %0d was skipped in oversub preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]],_tdm->_core_data.vars_pkg.os_buffer[j]);
					}
				}
				else{
					TDM_ERROR2("Failed to presort oversub port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.os_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1]);
				}
			}
			else{
				TDM_ERROR1("Unrecognized oversub port %0d in oversub preallocation\n", _tdm->_core_data.vars_pkg.lr_buffer[j]);
			}
		}
	}
	/* Set part of parameters used by core execution for vmap population */
	_tdm->_core_data.vars_pkg.os_enable = (_tdm->_core_data.vars_pkg.os_1||_tdm->_core_data.vars_pkg.os_10||_tdm->_core_data.vars_pkg.os_20||_tdm->_core_data.vars_pkg.os_25||_tdm->_core_data.vars_pkg.os_40||_tdm->_core_data.vars_pkg.os_50||_tdm->_core_data.vars_pkg.os_100||_tdm->_core_data.vars_pkg.os_120);
	_tdm->_core_data.vars_pkg.lr_enable = (_tdm->_core_data.vars_pkg.lr_1||_tdm->_core_data.vars_pkg.lr_10||_tdm->_core_data.vars_pkg.lr_20||_tdm->_core_data.vars_pkg.lr_25||_tdm->_core_data.vars_pkg.lr_40||_tdm->_core_data.vars_pkg.lr_50||_tdm->_core_data.vars_pkg.lr_100||_tdm->_core_data.vars_pkg.lr_120);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4;
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3;
    {
        num_120g= _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7;
        num_100g= _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4;
        num_50g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5;
        num_40g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3;
        num_25g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6;
        num_20g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2;
        num_10g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1;
        num_1g  = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8;
        speed_type_num = _tdm->_core_data.vars_pkg.lr_1+
                         _tdm->_core_data.vars_pkg.lr_10+
                         _tdm->_core_data.vars_pkg.lr_20+
                         _tdm->_core_data.vars_pkg.lr_25+
                         _tdm->_core_data.vars_pkg.lr_40+
                         _tdm->_core_data.vars_pkg.lr_50+
                         _tdm->_core_data.vars_pkg.lr_100+
                         _tdm->_core_data.vars_pkg.lr_120;
        TDM_PRINT2("%8s%d\n", "120G - ", num_120g);
        TDM_PRINT2("%8s%d\n", "100G - ", num_100g);
        TDM_PRINT2("%8s%d\n", " 50G - ", num_50g);
        TDM_PRINT2("%8s%d\n", " 40G - ", num_40g);
        TDM_PRINT2("%8s%d\n", " 25G - ", num_25g);
        TDM_PRINT2("%8s%d\n", " 20G - ", num_20g);
        TDM_PRINT2("%8s%d\n", " 10G - ", num_10g);
        TDM_PRINT2("%8s%d\n", "  1G - ", num_1g);
        TDM_PRINT1("\nNumber of Linerate Speed Types: %d\n\n", speed_type_num);
        TDM_BIG_BAR
    }
	/* Populate original vmap with pre-allocated linerate speed groups */
	/* 100G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 > 0){
		vmap_port_spd = 100;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), cal_length, lr100, TOKEN_100G, "100", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 100G port vector preallocation\n");
		}
	}
	/* 50G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 > 0){
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			if (tdm_ap_legacy_which_tsc(lr50[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_ap_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_ap_legacy_which_tsc(lr50[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_ap_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr50[v];
				for (w=v; w>0; w--) {
					lr50[w]=lr50[w-1];
				}
				lr50[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_ap_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_50G) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50=BOOL_TRUE;
					TDM_PRINT0("50G triport detected\n");
					break;
				}
			}
		}
		vmap_port_spd = 50;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), cal_length, lr50, TOKEN_50G, "50", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 50G port vector preallocation\n");
		}
	}
	/* 40G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			if (tdm_ap_legacy_which_tsc(lr40[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_ap_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_ap_legacy_which_tsc(lr40[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_ap_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr40[v];
				for (w=v; w>0; w--) {
					lr40[w]=lr40[w-1];
				}
				lr40[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_ap_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_40G && _tdm->_chip_data.soc_pkg.state[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]-1]!=PORT_STATE__DISABLED) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_TRUE;
					TDM_PRINT0("40G triport detected\n");
					break;
				}
			}
		}
		vmap_port_spd = 40;
		if(num_100g>0) {
			vmap_port_spd=41;
		}
		else if (num_100g==0 && num_50g>0){
			vmap_port_spd = 44;
		}
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), cal_length, lr40, TOKEN_40G, "40", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 40G port vector vmap allocation\n");
		}
	}
	/* 25G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_ap_legacy_which_tsc(lr20[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_20G) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20=BOOL_TRUE;
					TDM_PRINT0("20G triport detected\n");
					break;
				}
			}
		}
		vmap_port_spd = 25;
        if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4==0 &&
            _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5==0 &&
            _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3>0  ){
            vmap_port_spd = 26;
        }
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), cal_length, lr25, TOKEN_25G, "25", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 25G port vector preallocation\n");
		}
	}
	/* 20G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 > 0) {
		vmap_port_spd = 20;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), cal_length, lr20, TOKEN_20G, "20", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 20G port vector preallocation\n");
		}
	}
	/* 10G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 > 0) {
		vmap_port_spd = 10;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), cal_length, lr10, TOKEN_10G, "10", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 10G port vector preallocation\n");
		}
	}
	/* 1G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8 > 0) {
		vmap_port_spd = 1;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), cal_length, lr1, TOKEN_1G, "1", AP_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 1G port vector preallocation\n");
		}
	}
	
	/* 
	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) ); */
    if(tdm_core_vmap_alloc_mix(_tdm)!=PASS){
       TDM_ERROR0("VMAP Mix Allocation failed\n");
    } else {
       TDM_PRINT0("VMAP Mix Allocation passed\n");
    }
	return (tdm_ap_vbs_scheduler(_tdm));
}


/**
@name: tdm_ap_acc_alloc_new
@param:
 */
int
tdm_ap_acc_alloc_new( tdm_mod_t *_tdm )
{
	int i,j, idx, idx_opt, idx_up, idx_dn, dist_opt=0,
	    empty_slot_cnt=0, empty_slot_token, acc_slot_cnt=0, acc_slot_token, 
	    acc_slot_num, result=PASS;
	    
	int *tdm_pipe_main, tdm_pipe_main_len;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	empty_slot_token  = _tdm->_chip_data.soc_pkg.num_ext_ports;
	acc_slot_num      = _tdm->_chip_data.soc_pkg.tvec_size;
	acc_slot_token    = AP_ANCL_TOKEN;
	
	/* check available slots */
	for (i=0; i<tdm_pipe_main_len; i++){
		if (tdm_pipe_main[i] == empty_slot_token){
			empty_slot_cnt++;
		}
	}
	if (empty_slot_cnt<acc_slot_num){
		TDM_ERROR2("FAILED in ACC bandwidth allocation, Not enough available slots for ACC, Slot_Required %d, Slot_Available %d\n",acc_slot_cnt, empty_slot_cnt);
		result = FAIL;
	}
	/* allocate acc slots */
	if (acc_slot_num>0 && result==PASS){
		dist_opt = tdm_pipe_main_len/acc_slot_num;
		for (i=0; i<acc_slot_num; i++){
			idx_opt = i*dist_opt;
			if(idx_opt<tdm_pipe_main_len){
				idx    = tdm_pipe_main_len;
				idx_up = tdm_pipe_main_len;
				idx_dn = tdm_pipe_main_len;
				for (j=idx_opt; j>0; j--){
					if(tdm_pipe_main[j]==empty_slot_token){
						idx_up = j;
						break;
					}
				}
				for (j=idx_opt; j<tdm_pipe_main_len; j++){
					if(tdm_pipe_main[j]==empty_slot_token){
						idx_dn = j;
						break;
					}
				}
				if (idx_up<tdm_pipe_main_len || idx_dn<tdm_pipe_main_len){
					if (idx_up==tdm_pipe_main_len){
						idx = idx_dn;
					}
					else if (idx_dn==tdm_pipe_main_len){
						idx = idx_up;
					}
					else{
						idx = ((idx_opt-idx_up) < (idx_dn-idx_opt))? (idx_up): (idx_dn);
					}
				}
				
				if (idx < tdm_pipe_main_len){
					tdm_pipe_main[idx] = acc_slot_token;
				}
			}
		}
	}
	/* check acc slots */
	if (acc_slot_num>0 && result==PASS){
		for (i=0; i<tdm_pipe_main_len; i++){
			if (tdm_pipe_main[i] == acc_slot_token){
				acc_slot_cnt++;
			}
		}
		if(acc_slot_cnt != acc_slot_num){
			result = FAIL;
			TDM_ERROR2("FAILED in ACC bandwidth allocation, Slot_Required %d, Slot_Allocated %d\n",acc_slot_num, acc_slot_cnt);
		}
	}
	
	return result;
}

/**
@name: tdm_ap_vbs_scheduler
@param:
 */
int
tdm_ap_vbs_scheduler( tdm_mod_t *_tdm )
{
	int i, *tdm_pipe_main, tdm_cal_length, cnt=0; 
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_cal_length = _tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size;
	
	/* Allocate slots for all linerate ports among entire TDM calendar */
	if (_tdm->_core_data.vars_pkg.lr_enable) {
		if(FAIL==tdm_core_vbs_scheduler_lr(_tdm)){
			TDM_ERROR0("FAILED in slot allocation for LINERATE ports\n");
		}
	}
	/* Allocate ACC slots */
	if (_tdm->_core_data.vars_pkg.os_enable || _tdm->_core_data.vars_pkg.lr_enable) {
		if(FAIL==tdm_ap_acc_alloc_new(_tdm)){
			TDM_ERROR0("FAILED in slot allocation for ANCILLARY bandwidth\n");
		}
	}
	/* Allocate OVSB and/or IDLE slots */
	if (_tdm->_core_data.vars_pkg.os_enable || _tdm->_core_data.vars_pkg.lr_enable) {
		/* OVSB: Place the OVS token for all unallocated slots */
		if (_tdm->_core_data.vars_pkg.os_enable && (!_tdm->_core_data.vars_pkg.lr_enable)) {
			for (i=0; i<tdm_cal_length; i++) {
				if (tdm_pipe_main[i]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
					tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
				}
			}
		}
		/* IDLE: Place IDLE tokens for all unallocated slots */
		else {
		  for (i=0; i<tdm_cal_length; i++) {
				if (tdm_pipe_main[i]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
 				  if (_tdm->_core_data.vars_pkg.os_enable) {
					  tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
				  }
				  else {
					  if ((cnt++)%2==0) {
						  tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;
					  }
					  else {
						  tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token;
					  }
				  }
			  }
		  }
    }
	}
	
	/* OVS grouping */
	if (_tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER_OVS](_tdm)==FAIL) {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__FILTER](_tdm) );
	
}


/**
 */
void tdm_ap_ovs_20_40_clport(int *ovs_tdm_tbl, tdm_ap_chip_legacy_t *ap_chip)
{

int i, j, idx, size=AP_OS_LLS_GRP_LEN, avg=0;
int rpt_port;

   for(idx=0; idx<AP_OS_LLS_GRP_LEN; idx++) {
    if(ovs_tdm_tbl[idx] == AP_NUM_EXT_PORTS) break;
   }
   size = idx;
  
  for( i = 0 ; i<size; i++) {
    if(((ovs_tdm_tbl[i]>=29  && ovs_tdm_tbl[i]<=35) || (ovs_tdm_tbl[i]>=65  && ovs_tdm_tbl[i]<=71)) &&
      ((*ap_chip).tdm_globals.speed[ovs_tdm_tbl[i]] == SPEED_40G || (*ap_chip).tdm_globals.speed[ovs_tdm_tbl[i]] == SPEED_20G)) {
      if(ovs_tdm_tbl[i]==ovs_tdm_tbl[i+1]) {
        rpt_port=ovs_tdm_tbl[i];
        avg = ((*ap_chip).tdm_globals.speed[rpt_port]== SPEED_40G) ? (size/4) : (size/2);
        if( (i+avg) <size ){
          for(j=i; j<(i+avg); j++) { ovs_tdm_tbl[j] = ovs_tdm_tbl[j+1]; }
          ovs_tdm_tbl[i+avg] = rpt_port; 
        }
        else { 
          for(j=i; j<size; j++) { ovs_tdm_tbl[j] = ovs_tdm_tbl[j+1];}
          ovs_tdm_tbl[size-1]= ovs_tdm_tbl[0];
          for(j=0; j<(i+avg)%size; j++) {ovs_tdm_tbl[j] = ovs_tdm_tbl[j+1];}
          ovs_tdm_tbl[(i+avg)%size] = rpt_port;
        }
      }
    }
  }

}






void tdm_ap_reconfig_pgw_tbl(int *pgw_tdm_tbl, int port[2], enum port_speed_e speed[2])
{

int i, j ,k, idx, num_ovs=0, avg,cnt0=0, cnt1=0, size=AP_LR_LLS_LEN;
int elmnt_repeating, elmnt_repeating_cnt=0, num_elements_repeating=0;
int uniq_elements_cnt=0, sum_of_spacing=0, current_spacing=0;
int temp,spacing_idx=0;
int temp_swap_storage = 0;

int spacing_tbl[AP_LR_LLS_LEN],elmnt_repeating_tbl[AP_LR_LLS_LEN],uniq_elements_tbl[AP_LR_LLS_LEN];

  for(j=0; j<AP_LR_LLS_LEN; j++) { spacing_tbl[j] = 0;}
  
  for(i=0; i<AP_LR_LLS_LEN; i++) {
    if(pgw_tdm_tbl[i] == AP_NUM_EXT_PORTS) break;
    if(pgw_tdm_tbl[i] == AP_OVSB_TOKEN) num_ovs++;
   }
  size = i;

if(speed[0]==SPEED_100G || speed[0]==SPEED_50G || speed[1]==SPEED_100G || speed[1]==SPEED_50G ) {
  
  for(i=0; i<2; i++) {
  	 if(speed[i]==SPEED_100G || speed[i]==SPEED_50G) {
  		 elmnt_repeating_tbl[num_elements_repeating++] = port[i];
  		 elmnt_repeating = port[i];
  		 for(j=0; j<size; j++) {
  			 if(pgw_tdm_tbl[j] == elmnt_repeating_tbl[num_elements_repeating-1]) {
  				 for(k=j-1; k>=0; k--){
  					 pgw_tdm_tbl[k+1] = pgw_tdm_tbl[k];
  				 }
  				 elmnt_repeating_cnt++;
  			   pgw_tdm_tbl[0] = elmnt_repeating;
  			 }
  		 }
  	 }
   }   

   for(i=elmnt_repeating_cnt; i<size; i++) {
  	 uniq_elements_tbl[uniq_elements_cnt++] = pgw_tdm_tbl[i];
   }
  /*if one falcon in 100G*/
  if(num_elements_repeating == 1) {
     do	{
    	  for(i=0; i<elmnt_repeating_cnt/num_elements_repeating; i++) {
    		spacing_tbl[i]++;
    		sum_of_spacing++;
    		   if(sum_of_spacing == uniq_elements_cnt) break;
      	   }
     } while (sum_of_spacing < uniq_elements_cnt);
   /* make the spacing table symmetric*/
    do {
     spacing_idx++;
    } while(spacing_tbl[spacing_idx] != 0);

   for(i=0; i<spacing_idx; i++) {
      temp= spacing_tbl[spacing_idx-1];   
      for(k=spacing_idx-1; k>i; k--) { spacing_tbl[k] = spacing_tbl[k-1];}
      spacing_tbl[i] = temp;
      i=i+1;
   }
   /* put the repeating elements at start of tbl*/ 
     for(k=0, j=0; k<num_elements_repeating; k++) {
    		pgw_tdm_tbl[j++] = elmnt_repeating_tbl[k];
     }
    
     for(i=0, current_spacing=0, idx=0 ; i<uniq_elements_cnt; ) {
    	  if(current_spacing==spacing_tbl[idx]) {
    	 	 current_spacing = 0;
    		 idx++;
    		  for(k=0; k<num_elements_repeating; k++) {
    			pgw_tdm_tbl[j++] = elmnt_repeating_tbl[k];
    	 	  }
    	  }
    	  else { pgw_tdm_tbl[j++]=uniq_elements_tbl[i]; current_spacing++; i++; }
     }
   }
/*if two falcons in 100G or in 40G*/
   if (num_elements_repeating == 2) {
     for(i=0, j=0; i< elmnt_repeating_cnt/num_elements_repeating; i++ ) {
    	 for(k=0; k<num_elements_repeating; k++) {
    			pgw_tdm_tbl[j++] = elmnt_repeating_tbl[k];
    	 }
     }
     if (uniq_elements_cnt > 0) {
        avg=size/uniq_elements_cnt; /*  + (size%uniq_elements_cnt>0 ? 1:0);  ceil(size/uniq_elements_cnt) */
     } else {
        TDM_ERROR0("Error: Divide by zero\n");
        return;
     }
    
   	 for(i=avg-1, idx=0; i<size; ) {
  	   for(k=size-1; k>=i; k--) { pgw_tdm_tbl[k] = pgw_tdm_tbl[k-1];}
       pgw_tdm_tbl[i] =uniq_elements_tbl[idx++]; 
       i+=avg;
       if(idx ==uniq_elements_cnt) break;
     }
   }
  /* reconfig PGW calendar for 2x50G */
   if (speed[0] == SPEED_50G) {  
     for (i=0; i<AP_LR_LLS_LEN; i++) {
       if(pgw_tdm_tbl[i]== port[0]) {
        if ((cnt0++)%2==0) pgw_tdm_tbl[i] = port[0] ;
        else pgw_tdm_tbl[i] = (port[0]+2);
       }
     }
   }
   if (speed[1]== SPEED_50G) {
     for (i=0; i<AP_LR_LLS_LEN; i++) {
       if(pgw_tdm_tbl[i]== port[1]) {
        if ((cnt1++)%2==0) pgw_tdm_tbl[i] = port[1];
        else pgw_tdm_tbl[i] = (port[1]+2);
       }
     }
   }


   /* Recheck if any of the ports arrive back-to-back in pgw table */
   for (j=1; j<size;j++) {
       for (i=j;i<size;i++) {
           if ((pgw_tdm_tbl[i-1] == pgw_tdm_tbl[i]) ||
              ( (i==size-1) && (pgw_tdm_tbl[i]==pgw_tdm_tbl[j]))) { /* in an array {a,b,c,...} if a==b, then swap b & c */
              if (pgw_tdm_tbl[i]==AP_OVSB_TOKEN) continue;
              temp_swap_storage = pgw_tdm_tbl[i];
              pgw_tdm_tbl[i] = pgw_tdm_tbl[i+1];
              pgw_tdm_tbl[i+1] = temp_swap_storage;
             /* i=0;  this make sure we reiterate the loop to make sure that the repetition doesnt occur */
           }
       }
   }

 }
}
