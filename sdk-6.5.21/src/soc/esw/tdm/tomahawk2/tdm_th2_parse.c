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
@name: tdm_th2_parse_pipe
@param:

Parses pipe config from class structs
 */
void
tdm_th2_parse_pipe( tdm_mod_t *_tdm )
{
	int iter, iter2;

	/* Summarize the port config in this quadrant */
	TDM_PRINT0("TDM: --- Pipe Config ---: ");
	for (iter=_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_start; iter<=_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_end; iter++) {
		if ((((iter-1)%16)==0)) {
			TDM_PRINT0("\nTDM: ");
		}
		TDM_PRINT1("{%03d}\t",iter);
		if (iter%16==0) {
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				TDM_PRINT1("%d\t",_tdm->_chip_data.soc_pkg.speed[iter2+1]);
			}
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%TH2_NUM_PM_LNS==0) {
					switch (_tdm->_chip_data.soc_pkg.state[iter2-1]) {
						case PORT_STATE__LINERATE:
						case PORT_STATE__LINERATE_HG:
							TDM_PRINT0("LINE\t---\t---\t---\t");
							break;
						case PORT_STATE__OVERSUB:
						case PORT_STATE__OVERSUB_HG:
							TDM_PRINT0("OVSB\t---\t---\t---\t");
							break;
						default:
							break;
					}
				}
			}
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%TH2_NUM_PM_LNS==0) {
					switch (_tdm->_chip_data.soc_pkg.state[iter2-1]) {
						case PORT_STATE__LINERATE_HG: case PORT_STATE__OVERSUB_HG:
							TDM_PRINT0("HIGIG2\t---\t---\t---\t");
							break;
						case PORT_STATE__LINERATE: case PORT_STATE__OVERSUB:
							TDM_PRINT0("ETHRNT\t---\t---\t---\t");
							break;
						default:
							break;
					}
				}
			}
		}
	}
	TDM_PRINT0("\n");
	TDM_BIG_BAR
}


/**
@name: tdm_th2_parse_tdm_tbl
@param:

Postprocesses TDM calendar tokenized entries
 */
int
tdm_th2_parse_tdm_tbl( tdm_mod_t *_tdm )
{
	int j, m;
	tdm_calendar_t cal_idb, cal_mmu;
	
	switch(_tdm->_core_data.vars_pkg.cal_id) {
		case 0: cal_idb=_tdm->_chip_data.cal_0; cal_mmu=_tdm->_chip_data.cal_4; break;
		case 1: cal_idb=_tdm->_chip_data.cal_1; cal_mmu=_tdm->_chip_data.cal_5; break;
		case 2: cal_idb=_tdm->_chip_data.cal_2; cal_mmu=_tdm->_chip_data.cal_6; break;
		case 3: cal_idb=_tdm->_chip_data.cal_3; cal_mmu=_tdm->_chip_data.cal_7; break;
		default:
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return (TDM_EXEC_CORE_SIZE+1);
	}
	TDM_COPY(cal_mmu.cal_main, cal_idb.cal_main, sizeof(int)*512);
	m=0;
	for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
		if (cal_idb.cal_main[j]!=TH2_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, IDB TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, IDB TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			m++;
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_CMIC(j,cal_idb.cal_main)
						case 2: case 7:
							TH2_LPB0(j,cal_idb.cal_main)
						case 4: case 9:
							TH2_IDLE(j,cal_idb.cal_main)
						case 5:
							TH2_OPT1(j,cal_idb.cal_main)
						case 10:
							TH2_NULL(j,cal_idb.cal_main)
					}
					break;
				case 1:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_MGM1(j,cal_idb.cal_main)
						case 2: case 7:
							TH2_LPB1(j,cal_idb.cal_main)
						case 4: case 9:
							TH2_IDLE(j,cal_idb.cal_main)
						case 5:
							TH2_OPT1(j,cal_idb.cal_main)
						case 10:
							TH2_NULL(j,cal_idb.cal_main)
					}
					break;
				case 2:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_MGM2(j,cal_idb.cal_main)
						case 2: case 7:
							TH2_LPB2(j,cal_idb.cal_main)
						case 4: case 9:
							TH2_IDLE(j,cal_idb.cal_main)
						case 5:
							TH2_OPT1(j,cal_idb.cal_main)
						case 10:
							TH2_NULL(j,cal_idb.cal_main)
					}
					break;
				case 3:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_OPT1(j,cal_idb.cal_main)
						case 2: case 7:
							TH2_LPB3(j,cal_idb.cal_main)
						case 4: case 9:
							TH2_IDLE(j,cal_idb.cal_main)
						case 5:
							TH2_OPT1(j,cal_idb.cal_main)
						case 10:
							TH2_NULL(j,cal_idb.cal_main)
					}
					break;
			}
		}
	}
	m=0;
	for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
		if (cal_mmu.cal_main[j]!=TH2_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, MMU TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, MMU TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			m++;
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_CMIC(j,cal_mmu.cal_main)
						case 4: case 10:
							TH2_LPB0(j,cal_mmu.cal_main)
						case 2: case 5: case 9:
							TH2_NULL(j,cal_mmu.cal_main)
						case 7:
							TH2_IDLE(j,cal_mmu.cal_main)
					}
					break;
				case 1:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_MGM1(j,cal_mmu.cal_main)
						case 4: case 10:
							TH2_LPB1(j,cal_mmu.cal_main)
						case 2: case 5: case 9:
							TH2_NULL(j,cal_mmu.cal_main)
						case 7:
							TH2_IDLE(j,cal_mmu.cal_main)
					}
					break;
				case 2:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_MGM2(j,cal_mmu.cal_main)
						case 4: case 10:
							TH2_LPB2(j,cal_mmu.cal_main)
						case 2: case 5: case 9:
							TH2_NULL(j,cal_mmu.cal_main)
						case 7:
							TH2_IDLE(j,cal_mmu.cal_main)
					}
					break;
				case 3:
					switch (m) {
						case 1: case 3: case 6: case 8:
							TH2_OPT1(j,cal_mmu.cal_main)
						case 4: case 10:
							TH2_LPB3(j,cal_mmu.cal_main)
						case 2: case 5: case 9:
							TH2_NULL(j,cal_mmu.cal_main)
						case 7:
							TH2_IDLE(j,cal_mmu.cal_main)
					}
					break;
			}
		}
	}	
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 0, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[0][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 1, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[1][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 2, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[2][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 3, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[3][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 4, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[4][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 5, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[5][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 6, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[6][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 7, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[7][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 8, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[8][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 9, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[9][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 10, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[10][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 11, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[11][j]);
	}	
	
	
	TDM_PRINT1("\nTDM: PIPE %d, OVS GROUPs 0:11\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j< TH2_OS_VBS_GRP_NUM; j++) {
		if ((j %(TH2_OS_VBS_GRP_NUM/2))==0) { TDM_PRINT1("\n   HALF PIPE %d", j/(TH2_OS_VBS_GRP_NUM/2) );}
		TDM_PRINT1("\nOVS_CAL[%d]:\t", j);
		for (m=0; m< TH2_OS_VBS_GRP_LEN; m++) {
			TDM_PRINT1("%d\t", cal_idb.cal_grp[j][m]);
		}
	}
	TDM_PRINT0("\n\n");
	
	
	TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 0\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j<SHAPING_GRP_LEN; j++) {
		if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
		TDM_PRINT1("\t%d",cal_idb.cal_grp[SHAPING_GRP_IDX_0][j]);
	}
	TDM_PRINT0("\n\n");
	TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 1\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j<SHAPING_GRP_LEN; j++) {
		if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
		TDM_PRINT1("\t%d",cal_idb.cal_grp[SHAPING_GRP_IDX_1][j]);
	}
	TDM_PRINT0("\n\n");
	
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}




/**
@name: tdm_th2_parse_tdm_tbl_new
@param:

Postprocesses TDM calendar tokenized entries
 */
int
tdm_th2_parse_tdm_tbl_new( tdm_mod_t *_tdm )
{
	int j, m;
	tdm_calendar_t cal_idb, cal_mmu;
	
	switch(_tdm->_core_data.vars_pkg.cal_id) {
		case 0: cal_idb=_tdm->_chip_data.cal_0; cal_mmu=_tdm->_chip_data.cal_4; break;
		case 1: cal_idb=_tdm->_chip_data.cal_1; cal_mmu=_tdm->_chip_data.cal_5; break;
		case 2: cal_idb=_tdm->_chip_data.cal_2; cal_mmu=_tdm->_chip_data.cal_6; break;
		case 3: cal_idb=_tdm->_chip_data.cal_3; cal_mmu=_tdm->_chip_data.cal_7; break;
		default:
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return (TDM_EXEC_CORE_SIZE+1);
	}
	TDM_COPY(cal_mmu.cal_main, cal_idb.cal_main, sizeof(int)*512);
	m=0;
	for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
		if (cal_idb.cal_main[j] < TH2_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, IDB TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, IDB TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					/* CPU token */
					if (cal_idb.cal_main[j]==TH2_ANCL_TOKEN) {
						TH2_CMIC(j,cal_idb.cal_main)
					}
					/* LBK 0 token */
					if (cal_idb.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB0(j,cal_idb.cal_main)
					}
					break;
				case 1:
					/* MGM 1 token */
					if (cal_idb.cal_main[j]==TH2_ANCL_TOKEN) {
						TH2_MGM1(j,cal_idb.cal_main)
					}
					/* LBK 1 token */
					if (cal_idb.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB1(j,cal_idb.cal_main)
					}
					break;
				case 2:
					/* MGM 2 token */
					if (cal_idb.cal_main[j]==TH2_ANCL_TOKEN) {
						TH2_MGM2(j,cal_idb.cal_main)
					}
					/* LBK 2 token */
					if (cal_idb.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB2(j,cal_idb.cal_main)
					}
					break;
				case 3:
					/* No MGM in PIPE 3; SPARE tokens */
					if (cal_idb.cal_main[j]==TH2_ANCL_TOKEN) {
						if ((!_tdm->_core_data.vars_pkg.os_enable) && _tdm->_core_data.vars_pkg.lr_enable) {
							TH2_IDLE(j,cal_idb.cal_main)
						} else {
							cal_idb.cal_main[j] = TH2_OVSB_TOKEN;
							TDM_PRINT0("OVS TOKEN\n");
							break;
						}
					}
					/* LBK 2 token */
					if (cal_idb.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB3(j,cal_idb.cal_main)
					}
					break;
				default : 
					TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
					return (TDM_EXEC_CORE_SIZE+1);
			}
			/* OPPT, IDLE, NULL tokens */
			if (cal_idb.cal_main[j]==(TH2_ANCL_TOKEN+2)) {
				switch (m) {
					case 0: case 2:
						TH2_IDLE(j,cal_idb.cal_main)
					case 1:
						TH2_OPT1(j,cal_idb.cal_main)
					case 3:
						TH2_NULL(j,cal_idb.cal_main)
				}
				m++;
			}
		}
	}


	m=0;
	for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
		if (cal_mmu.cal_main[j] < TH2_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, MMU TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, MMU TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					/* CPU token */
					if (cal_mmu.cal_main[j]==TH2_ANCL_TOKEN) {
						TH2_CMIC(j,cal_mmu.cal_main)
					}
					/* LBK 0 token */
					if (cal_mmu.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB0(j,cal_mmu.cal_main)
					}
					break;
				case 1:
					/* MGM 1 token */
					if (cal_mmu.cal_main[j]==TH2_ANCL_TOKEN) {
						TH2_MGM1(j,cal_mmu.cal_main)
					}
					/* LBK 1 token */
					if (cal_mmu.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB1(j,cal_mmu.cal_main)
					}
					break;
				case 2:
					/* MGM 2 token */
					if (cal_mmu.cal_main[j]==TH2_ANCL_TOKEN) {
						TH2_MGM2(j,cal_mmu.cal_main)
					}
					/* LBK 2 token */
					if (cal_mmu.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB2(j,cal_mmu.cal_main)
					}
					break;
				case 3:
					/* No MGM in PIPE 3; SPARE tokens */
					if (cal_mmu.cal_main[j]==TH2_ANCL_TOKEN) {
						if ((!_tdm->_core_data.vars_pkg.os_enable) && _tdm->_core_data.vars_pkg.lr_enable) {
							TH2_IDLE(j,cal_mmu.cal_main)
						} else {
							cal_mmu.cal_main[j] = TH2_OVSB_TOKEN;
							TDM_PRINT0("OVS TOKEN\n");
							break;
						}
					}
					/* LBK 2 token */
					if (cal_mmu.cal_main[j]==(TH2_ANCL_TOKEN+1)) {
						TH2_LPB3(j,cal_mmu.cal_main)
					}
					break;
				default : 
					TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
					return (TDM_EXEC_CORE_SIZE+1);
			}
			/* OPPT, IDLE, NULL tokens */
			if (cal_mmu.cal_main[j]==(TH2_ANCL_TOKEN+2)) {
				switch (m) {
					case 0: case 2: case 3:
						TH2_NULL(j,cal_mmu.cal_main)
					case 1:
						TH2_IDLE(j,cal_mmu.cal_main)
					default : 
						TDM_PRINT1("Invalid ANCL_TOKEN index m=%d\n",m);
				}
				m++;
			}
		}
	}

	
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 0, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[0][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 1, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[1][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 2, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[2][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 3, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[3][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 4, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[4][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 5, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[5][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 6, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[6][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 7, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[7][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 8, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[8][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 9, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[9][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 10, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[10][j]);
	}
	for (j=0; j<TH2_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, OVS GROUP 11, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[11][j]);
	}	
	
	
	TDM_PRINT1("\nTDM: PIPE %d, IDB TDM main calendar\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
		if (j%32==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+31);}
		TDM_PRINT1("%5d",cal_idb.cal_main[j]);
	}
	TDM_PRINT0("\n\n");
	
	TDM_PRINT1("\nTDM: PIPE %d, MMU TDM main calendar\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
		if (j%32==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+31);}
		TDM_PRINT1("%5d",cal_mmu.cal_main[j]);
	}
	TDM_PRINT0("\n\n");
	
	TDM_PRINT1("\nTDM: PIPE %d, OVS GROUPs 0:11\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j< TH2_OS_VBS_GRP_NUM; j++) {
		if ((j %(TH2_OS_VBS_GRP_NUM/2))==0) { TDM_PRINT1("\n   HALF PIPE %d", j/(TH2_OS_VBS_GRP_NUM/2) );}
		TDM_PRINT1("\nOVS_CAL[%d]:\t", j);
		for (m=0; m< TH2_OS_VBS_GRP_LEN; m++) {
			TDM_PRINT1("%d\t", cal_idb.cal_grp[j][m]);
		}
	}
	TDM_PRINT0("\n\n");
	
	
	TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 0\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j<SHAPING_GRP_LEN; j++) {
		if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
		TDM_PRINT1("\t%d",cal_idb.cal_grp[SHAPING_GRP_IDX_0][j]);
	}
	TDM_PRINT0("\n\n");
	TDM_PRINT1("\nTDM: PIPE %d, PKT SCHEDULER HALF PIPE 1\n",_tdm->_core_data.vars_pkg.pipe);
	for (j=0; j<SHAPING_GRP_LEN; j++) {
		if (j%16==0) { TDM_PRINT2("\n %3d : %3d\t", j, j+15);}
		TDM_PRINT1("\t%d",cal_idb.cal_grp[SHAPING_GRP_IDX_1][j]);
	}
	TDM_PRINT0("\n\n");

	TDM_PRINT1("\nTDM: PIPE %d, PM_NUM to PBLK_ID MAP \nPM:PBLK_ID",_tdm->_core_data.vars_pkg.pipe);
	for (j=(16*_tdm->_core_data.vars_pkg.cal_id); j<(16*(_tdm->_core_data.vars_pkg.cal_id+1)); j++) {
		TDM_PRINT2("\t %2d:%2d",j, _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[j]);
	}
	TDM_PRINT0("\n\n");	
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}
