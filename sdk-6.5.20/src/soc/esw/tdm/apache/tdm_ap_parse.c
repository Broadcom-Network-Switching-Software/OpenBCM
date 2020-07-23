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
@name: tdm_ap_print_tbl
@param:

Prints PGW TDM main calendar
 */
void
tdm_ap_print_tbl(int *cal, int len, const char* name, int id)
{
	int idx;
	
	for (idx=0; idx<len; idx++) {
		TDM_PRINT4("TDM: Pipe %d, %s, Slot -- #%03d, Port -- #%0d\n", id, name, idx, cal[idx]);
	}
}


/**
@name: tdm_ap_print_tbl_ovs
@param:

Prints PGW TDM calendar oversub scheduling groups
 */
void
tdm_ap_print_tbl_ovs(int *cal, int *spc, int len, const char* name, int id)
{
	int idx;
	
	for (idx=0; idx<len; idx++) {
		TDM_PRINT5("TDM: Pipe %d, %s, Slot -- #%03d, Port -- #%0d, Spacing -- #%0d\n", id, name, idx, cal[idx], spc[idx]);
	}
}


/**
@name: tdm_ap_print_quad
@param:
 */
void
tdm_ap_print_quad(enum port_speed_e *speed, enum port_state_e *state, int len, int idx_start, int idx_end)
{
	int iter, iter2;
	
	if (idx_start>0 && idx_end<=len){
		TDM_BIG_BAR
		TDM_PRINT0("TDM: --- Pipe Config ---: ");
		for (iter=idx_start; iter<=idx_end; iter++) {
			if ((((iter-1)%18)==0)) {
				TDM_PRINT0("\nTDM: ");
			}		
			TDM_PRINT1("{%03d}\t",(iter));
			if (iter%18==0) {
				TDM_PRINT0("\nTDM: ");
				for (iter2=(iter-18); iter2<iter; iter2++) {
					TDM_PRINT1(" %3d \t", speed[iter2+1]/1000);
				}
				TDM_PRINT0("\nTDM: ");
				for (iter2=(iter-18); iter2<iter; iter2++) {
					if (iter2%4==0){
						if(speed[iter2+1]/1000>0){
							switch ((PSBMP_LP_MASK & state[iter2])) {
								case 1:
									TDM_PRINT0("LINE \t --- \t --- \t --- \t");
									break;
								case 2:
									TDM_PRINT0("OVSB \t --- \t --- \t --- \t");
									break;
								default:
									TDM_PRINT0("     \t     \t     \t     \t");
									break;
							}
						}
						else{
							TDM_PRINT0("     \t     \t     \t     \t");
						}
					}
				}
				TDM_PRINT0("\nTDM: ");
				for (iter2=(iter-18); iter2<iter; iter2++) {
					if(iter2%4==0){
						if(speed[iter2+1]/1000>0){
							switch(( PSBMP_ENCAP_HIGIG & state[iter2])){
								case 0:
									TDM_PRINT0("  EN \t --- \t --- \t --- \t");
									break;
								case 4:
									TDM_PRINT0("  HG \t --- \t --- \t --- \t");
									break;
								default:
									TDM_PRINT0("     \t     \t     \t     \t");
									break;
							}
						}
						else{
							TDM_PRINT0("     \t     \t     \t     \t");
						}
					}
				}
			}
		}
		TDM_PRINT0("\n");
		TDM_BIG_BAR
	}

}




/**
@name: tdm_ap_parse_mmu_tdm_tbl
@param:
 */
int
tdm_ap_parse_mmu_tdm_tbl( tdm_mod_t *_tdm )
{
	const char *name;
	int j, k=512, m=0,
		*mmu_tdm_tbl, *mmu_tdm_ovs_1, *mmu_tdm_ovs_2, *mmu_tdm_ovs_3, *mmu_tdm_ovs_4, *mmu_tdm_ovs_5, *mmu_tdm_ovs_6,
		bw=_tdm->_chip_data.soc_pkg.clk_freq;
	  tdm_calendar_t *cal;
	
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
		case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
		case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
		case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
		case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
		case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
		case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
		case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
		default:													
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);		
			return (TDM_EXEC_CORE_SIZE+1);							
	}
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 2: name = "X-PIPE"; break;
		default: name = "UNKNOWN"; break;
	}	
	mmu_tdm_tbl = cal->cal_main;
	mmu_tdm_ovs_1 = cal->cal_grp[0];
	mmu_tdm_ovs_2 = cal->cal_grp[1];
	mmu_tdm_ovs_3 = cal->cal_grp[2];
	mmu_tdm_ovs_4 = cal->cal_grp[3];
	mmu_tdm_ovs_5 = cal->cal_grp[4];
	mmu_tdm_ovs_6 = cal->cal_grp[5];

/*for (j=0; j<16; j++) {
		if (mmu_tdm_ovs_1[j]!=TD2P_NUM_EXT_PORTS || mmu_tdm_ovs_2[j]!=TD2P_NUM_EXT_PORTS || mmu_tdm_ovs_3[j]!=TD2P_NUM_EXT_PORTS || mmu_tdm_ovs_4[j]!=TD2P_NUM_EXT_PORTS) {
			oversub=BOOL_TRUE;
		}
	}*/
	/* Allocate slots for MGMT, CMIC/LPBK, ANCL and RERESH among available AP_ANCL_TOKENs */
	for (j=0; j<k; j++) {
		if (mmu_tdm_tbl[j]!=AP_ANCL_TOKEN) {
			TDM_PRINT3("PIPE: %s, MMU TDM TABLE, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_tbl[j]);
		}
	 	else {
			TDM_PRINT2("PIPE: %s, MMU TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ", name, j);
			m++;
			if (bw==933) {
				if (_tdm->_core_data.vars_pkg.cal_id==AP_XPIPE_CAL_ID) {
					switch (m) {
						case 1: case 3: case 5: case 7:
							AP_ANCL(j)
						case 2: case 6: 
							AP_CMIC(j)
            case 4: case 9:
							AP_LPBK(j)
            case 8:
							AP_SBUS(j)
					}
				}
			}
			else if (bw==840) {
				if (_tdm->_core_data.vars_pkg.cal_id==AP_XPIPE_CAL_ID) {
					switch (m) {
						case 1: case 3: case 5: case 7:
							AP_ANCL(j)
						case 2: case 6: 
							AP_CMIC(j)
            case 4: case 9:
							AP_LPBK(j)
            case 8:
							AP_SBUS(j)
					}
				}
			}
			else if (bw==793 || bw==794 || bw==795 || bw==796 || bw==797) {
				if (_tdm->_core_data.vars_pkg.cal_id==AP_XPIPE_CAL_ID) {
					switch (m) {
						case 1: case 3: case 5: case 7: case 9: case 11: case 13: case 15: case 17:
							AP_ANCL(j)
						case 2: case 6: case 10: case 14: 
							AP_LPBK(j)
            case 4: case 8: case 12: case 16:
							AP_CMIC(j)
            case 18:
							AP_SBUS(j)
					}
				}
			}
			else if (bw==575) {
				if (_tdm->_core_data.vars_pkg.cal_id==AP_XPIPE_CAL_ID) {
					switch (m) {
						case 1: case 5: 
							AP_CMIC(j)
						case 2: case 4: case 6: case 8:
							AP_ANCL(j)
            case 3: case 7:
              AP_LPBK(j)
            case 9:
							AP_SBUS(j)
					}
				}
			}
			else if (bw==510) {
				if (_tdm->_core_data.vars_pkg.cal_id==AP_XPIPE_CAL_ID) {
					switch (m) {
						case 1: case 3: case 5: case 7: case 9:
							AP_CMIC(j)
						case 2: case 4: case 6: case 8:
							AP_ANCL(j)
            case 10:
							AP_SBUS(j)
					}
				}
			}
		  else if (bw==435) { 
			  if (_tdm->_core_data.vars_pkg.cal_id==AP_XPIPE_CAL_ID) {
					switch (m) {
						case 1: case 3: case 6: case 8:
							AP_ANCL(j)
            case 2: case 5: case 7:
              AP_LPBK(j)
						case 4: case 9: 
							AP_CMIC(j)
            case 10:
							AP_SBUS(j)
					}
        }
      }
			else {
				TDM_PRINT0("INVALID PARAMETERS\n");
			}
		}
	}

	for (j=0; j<AP_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("PIPE: %s, MMU OVS BUCKET 0, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_1[j]);
	}
	for (j=0; j<AP_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("PIPE: %s, MMU OVS BUCKET 1, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_2[j]);
	}
	for (j=0; j<AP_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("PIPE: %s, MMU OVS BUCKET 2, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_3[j]);
	}
	for (j=0; j<AP_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("PIPE: %s, MMU OVS BUCKET 3, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_4[j]);
	}
	for (j=0; j<AP_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("PIPE: %s, MMU OVS BUCKET 4, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_5[j]);
	}
	for (j=0; j<AP_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("PIPE: %s, MMU OVS BUCKET 5, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_6[j]);
	}
	
	return ( ( _tdm->_core_exec[TDM_CORE_EXEC__POST]( _tdm ) ) );
}
