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
@name: tdm_th_print_tbl
@param:

Prints PGW TDM main calendar
 */
void
tdm_th_print_tbl(tdm_mod_t *_tdm)
{
	int idx, *cal_idb, *cal_mmu, cal_idb_len=0, cal_mmu_len=0, check=PASS;
	
	switch(_tdm->_core_data.vars_pkg.cal_id) {
		case 0: 
			cal_idb     = _tdm->_chip_data.cal_0.cal_main; 
			cal_mmu     = _tdm->_chip_data.cal_4.cal_main;
			cal_idb_len = _tdm->_chip_data.cal_0.cal_len;
			cal_mmu_len = _tdm->_chip_data.cal_4.cal_len;
			break;
		case 1: 
			cal_idb     = _tdm->_chip_data.cal_1.cal_main; 
			cal_mmu     = _tdm->_chip_data.cal_5.cal_main; 
			cal_idb_len = _tdm->_chip_data.cal_1.cal_len;
			cal_mmu_len = _tdm->_chip_data.cal_5.cal_len;
			break;
		case 2: 
			cal_idb     = _tdm->_chip_data.cal_2.cal_main; 
			cal_mmu     = _tdm->_chip_data.cal_6.cal_main; 
			cal_idb_len = _tdm->_chip_data.cal_2.cal_len;
			cal_mmu_len = _tdm->_chip_data.cal_6.cal_len;
			break;
		case 3: 
			cal_idb     = _tdm->_chip_data.cal_3.cal_main; 
			cal_mmu     = _tdm->_chip_data.cal_7.cal_main; 
			cal_idb_len = _tdm->_chip_data.cal_3.cal_len;
			cal_mmu_len = _tdm->_chip_data.cal_7.cal_len;
			break;
		default:
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			check = FAIL;
			break;
	}
	
	if (check==PASS){
		if (cal_idb_len==cal_mmu_len){
			TDM_SML_BAR
			TDM_PRINT4("TDM: Pipe %d, IDB/MMU table, table length %d (%d+%d)\n",  _tdm->_core_data.vars_pkg.cal_id, _tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size);
			TDM_PRINT4("TDM: Pipe %d \t %4s \t %3s \t %3s\n",  _tdm->_core_data.vars_pkg.cal_id, "idx", "IDB", "MMU");
			for (idx=0; idx<cal_mmu_len; idx++) {
				TDM_PRINT4("TDM: Pipe %d \t #%03d \t %3d \t %3d\n", _tdm->_core_data.vars_pkg.cal_id, idx, cal_idb[idx], cal_mmu[idx]);
			}
		}
		
	}
}


/**
@name: tdm_th_print_config
@param:

Parses pipe config from class structs
 */
void
tdm_th_print_config( tdm_mod_t *_tdm )
{
	int i, lgc_idx=0, tsc_idx, config_pass=BOOL_TRUE;
	TDM_SML_BAR
	TDM_SML_BAR
	TDM_PRINT0(" Port Info based on config.bcm\n");
	TDM_SML_BAR
	TDM_PRINT6("%4s%4s%8s%8s%8s%8s\n", "lgc", "phy", "spd","state", "encap", "traffic");
	
	for (i=0;i<TH_NUM_EXT_PORTS;i++){
		if (_tdm->_chip_data.soc_pkg.speed[i]>0) {
            lgc_idx++;
            TDM_PRINT1("%4d", lgc_idx);
        }
        else{
            TDM_PRINT1("%4s", "");
        }
		TDM_PRINT1("%4d", i);
		TDM_PRINT1("%8d", _tdm->_chip_data.soc_pkg.speed[i]/1000);
		if (i>0){
			switch (_tdm->_chip_data.soc_pkg.state[i-1]){
				case PORT_STATE__LINERATE:
					TDM_PRINT2("%8s%8s","[LNRT]","[ETHN]");
					break;
				case PORT_STATE__OVERSUB:
					TDM_PRINT2("%8s%8s","[OVSB]", "[ETHN]");
					break;
				case PORT_STATE__LINERATE_HG:
					TDM_PRINT2("%8s%8s","[LNRT]", "[HIG2]");
					break;
				case PORT_STATE__OVERSUB_HG:
					TDM_PRINT2("%8s%8s","[OVSB]", "[HIG2]");
					break;
				case PORT_STATE__COMBINE:
					TDM_PRINT2("%8s%8s","[COM]", "[----]");
					break;
				case PORT_STATE__COMBINE_HG:
					TDM_PRINT2("%8s%8s","[COM]", "[----]");
					break;
				default:
					TDM_PRINT2("%8s%8s","[----]","[----]");
					break;
			}
			
			tsc_idx = (i-1)/4;
			if (tsc_idx < TH_NUM_PHY_PM){
				switch(_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[tsc_idx]){
					case PM_ENCAP__ETHRNT:
						TDM_PRINT1("%8s", "[ETHN]");
						break;
					case PM_ENCAP__HIGIG2:
						TDM_PRINT1("%8s", "[HIG2]");
						break;
					default:
						TDM_PRINT1("%8s", "[----]");
						break;
				}
				if ( ( (_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[tsc_idx]== PM_ENCAP__HIGIG2) && (_tdm->_chip_data.soc_pkg.state[i-1]==PORT_STATE__LINERATE     || _tdm->_chip_data.soc_pkg.state[i-1]==PORT_STATE__OVERSUB) ) ||
					 ( (_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[tsc_idx]== PM_ENCAP__ETHRNT) && (_tdm->_chip_data.soc_pkg.state[i-1]==PORT_STATE__LINERATE_HG  || _tdm->_chip_data.soc_pkg.state[i-1]==PORT_STATE__OVERSUB_HG) ) ){
					TDM_PRINT1("\t%s", "[XXXX]");
					config_pass = BOOL_FALSE;
				}
			}
		}
		TDM_PRINT0("\n");
			
		if(i>0 && i%4==0){TDM_PRINT0("\n");}
	}
	TDM_PRINT0("\n");
	if (config_pass==BOOL_FALSE){TDM_PRINT0("TDM ERROR: config.bcm failed, encap type violation\n");}
	TDM_PRINT0("\n");
	TDM_BIG_BAR
}


/**
@name: tdm_th_parse_pipe
@param:

Parses pipe config from class structs
 */
void
tdm_th_parse_pipe( tdm_mod_t *_tdm )
{
	int iter, iter2, len, idx_start, idx_end;
	enum port_speed_e *speed = _tdm->_chip_data.soc_pkg.speed;
	enum port_state_e *state = _tdm->_chip_data.soc_pkg.state;
	
	idx_start= _tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start;
	idx_end  = _tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end;
	len      = TH_NUM_PHY_PORTS;
	
	if (idx_start>0 && idx_end<=len){
		TDM_BIG_BAR
		TDM_PRINT0("TDM: --- Pipe Config ---: ");
		for (iter=idx_start; iter<=idx_end; iter++) {
			if ((((iter-1)%16)==0)) {
				TDM_PRINT0("\nTDM: ");
			}		
			TDM_PRINT1("{%03d}\t",(iter));
			if (iter%16==0) {
				TDM_PRINT0("\nTDM: ");
				for (iter2=(iter-16); iter2<iter; iter2++) {
					TDM_PRINT1(" %3d \t", speed[iter2+1]/1000);
				}
				TDM_PRINT0("\nTDM: ");
				for (iter2=(iter-16); iter2<iter; iter2++) {
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
				for (iter2=(iter-16); iter2<iter; iter2++) {
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
@name: tdm_th_parse_tdm_tbl
@param:

Postprocesses TDM calendar tokenized entries
 */
int
tdm_th_parse_tdm_tbl( tdm_mod_t *_tdm )
{
	int i, j, m;
	tdm_calendar_t cal_idb;
	tdm_calendar_t cal_mmu;
	
	switch(_tdm->_core_data.vars_pkg.cal_id) {
		case 0: cal_idb=_tdm->_chip_data.cal_0; cal_mmu=_tdm->_chip_data.cal_4; break;
		case 1: cal_idb=_tdm->_chip_data.cal_1; cal_mmu=_tdm->_chip_data.cal_5; break;
		case 2: cal_idb=_tdm->_chip_data.cal_2; cal_mmu=_tdm->_chip_data.cal_6; break;
		case 3: cal_idb=_tdm->_chip_data.cal_3; cal_mmu=_tdm->_chip_data.cal_7; break;
		default:
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return (TDM_EXEC_CORE_SIZE+1);
	}
	for (j=0; j<256; j++) {
		cal_mmu.cal_main[j]=cal_idb.cal_main[j];
	}
	for (i=0; i<TH_OS_VBS_GRP_NUM; i++) {
		for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
			cal_mmu.cal_grp[i][j]=cal_idb.cal_grp[i][j];
		}
	}
	m=0;
	for (j=0; j<256; j++) {
		if (cal_idb.cal_main[j]!=TH_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, IDB TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, IDB TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			m++;
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_CMIC(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB0(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
				case 1:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM1(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB1(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
				case 2:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM2(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB2(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
				case 3:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_DOT3(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB3(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
			}
		}
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 0, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[0][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 1, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[1][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 2, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[2][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 3, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[3][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 4, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[4][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 5, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[5][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 6, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[6][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 7, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[7][j]);
	}
	m=0;
	for (j=0; j<256; j++) {
		if (cal_mmu.cal_main[j]!=TH_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, MMU TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, MMU TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			m++;
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_CMIC(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB0(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
				case 1:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM1(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB1(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
				case 2:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM2(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB2(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
				case 3:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_DOT3(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB3(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
			}
		}
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 0, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[0][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 1, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[1][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 2, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[2][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 3, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[3][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 4, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[4][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 5, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[5][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 6, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[6][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 7, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[7][j]);
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}
