/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core oversub algorithms
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_fill_ovs
@param:

Fills and sorts oversub speed groups, supports up to 8 groups
 */
int
tdm_fill_ovs( tdm_mod_t *_tdm )
{
	int i, j, k, tsc_id, tsc_fetch_cnt=0, slice_factor;
	unsigned short tsc_stack[TDM_AUX_SIZE], *ovs_buf;
	short *z;
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
	switch (_tdm->_core_data.vars_pkg.grp_id) {	
		case 1:	
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10;
			break;
		case 2:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20;
			break;
		case 3:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40;
			break;
		case 4:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100;
			break;
		case 5:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50;
			break;
		case 6:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25;
			break;
		case 7:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120;
			break;
		case 8:
			z=(&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8));
			ovs_buf=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1;
			break;
		default:
			TDM_PRINT1("Invalid group ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return (TDM_EXEC_CORE_SIZE+1);
	}
	TDM_SML_BAR
	TDM_PRINT2("Pointer location: %0d | Get port: %0d\n",(*z),ovs_buf[*z]);
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		tsc_stack[i]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	}
	tsc_stack[0]=ovs_buf[*z];
	ovs_buf[*z]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	_tdm->_core_data.vars_pkg.port=tsc_stack[0];
	tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=tsc_stack[0] && _tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]==_tdm->_chip_data.soc_pkg.speed[tsc_stack[0]]) {
			for (j=1; j<=*z; j++) {
				if (ovs_buf[j]==_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]) {
					tsc_stack[++tsc_fetch_cnt]=_tdm->_chip_data.soc_pkg.pmap[tsc_id][i];
					for (k=j; k<*z; k++) {
						ovs_buf[k]=ovs_buf[k+1];
					}
				}
			}
		}
	}
	(*z)-=(1+tsc_fetch_cnt);	
	TDM_PRINT1("TSC fetch {%0d}: ",(tsc_fetch_cnt+1));
	for (i=0; i<4; i++) {
		TDM_PRINT1("[%0d] ",tsc_stack[i]);
	}
	TDM_PRINT0("\n");
	
	switch (tsc_fetch_cnt) {
		case 1:
			slice_factor=(cal->grp_len/2);
			break;
		case 2:
		case 3:
			slice_factor=(cal->grp_len/4);
			break;
		default:
			slice_factor=cal->grp_len;
			break;
	}
	TDM_PRINT5("Slice size: %0d | (tsc_fetch_cnt=%0d, Z11=%0d, grp_len=%d) Group speed: %0dG\n",slice_factor,tsc_fetch_cnt, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11, cal->grp_len,  (_tdm->_chip_data.soc_pkg.speed[tsc_stack[0]]/1000));
	while (cal->cal_grp[0][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)++;
	while (cal->cal_grp[1][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)++;
	while (cal->cal_grp[2][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)++;
	while (cal->cal_grp[3][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)++;
	while (cal->cal_grp[4][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)++;
	while (cal->cal_grp[5][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)++;
	while (cal->cal_grp[6][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)++;
	while (cal->cal_grp[7][_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88]!=_tdm->_chip_data.soc_pkg.num_ext_ports && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)<cal->grp_len) (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)++;
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes ; i++) {
		if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			if (( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )  )
				if (cal->cal_grp[0][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)+=(tsc_fetch_cnt*slice_factor);
				}
			if (( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )   )
				if (cal->cal_grp[1][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)+=(tsc_fetch_cnt*slice_factor);
				}
			if (( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )   )
				if (cal->cal_grp[2][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)+=(tsc_fetch_cnt*slice_factor);
				}
			if (( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )    )
				if (cal->cal_grp[3][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)+=(tsc_fetch_cnt*slice_factor);
				}
			if (( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )    )
				if (cal->cal_grp[4][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)+=(tsc_fetch_cnt*slice_factor);
				}
			if ((((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )    )
				if (cal->cal_grp[5][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)+=(tsc_fetch_cnt*slice_factor);
				}
			if ((((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )    )
				if (cal->cal_grp[6][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)+=(tsc_fetch_cnt*slice_factor);
				}
			if ((((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len )    )
				if (cal->cal_grp[7][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)+(i*slice_factor)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)+=(tsc_fetch_cnt*slice_factor);
				}
		}
	}
	if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[0][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 0 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[1][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 1 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[2][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 2 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[3][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 3 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[4][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 4 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[5][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 5 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[6][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 6 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)++;
	}
	else if ( ((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)+(tsc_fetch_cnt*slice_factor)) < cal->grp_len ) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
			if (tsc_stack[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				cal->cal_grp[7][(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)+(i*slice_factor)] = tsc_stack[i];
				TDM_PRINT2("Group: 7 | Index: %0d | Port: %0d\n",((_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)++;
	}
	else {
		TDM_ERROR1("No speed group available, pointer at %0d\n",(*z));
		return FAIL;
	}
	
	TDM_PRINT1("Pointer location: %0d\n",(*z));
	return PASS;
}


/**
@name: tdm_fill_ovs_simple
@param:

Fills oversub speed groups without sort, supports up to 8 groups
 */
int
tdm_fill_ovs_simple(short *z, unsigned short ovs_buf[TDM_AUX_SIZE], int *bucket1, unsigned short *z11, int *bucket2, unsigned short *z22, int *bucket3, unsigned short *z33, int *bucket4, unsigned short *z44, int *bucket5, unsigned short *z55, int *bucket6, unsigned short *z66, int *bucket7, unsigned short *z77, int *bucket8, unsigned short *z88, int grp_len)
{
	TDM_SML_BAR
	
	if (*z11 < grp_len) {
		TDM_PRINT2("Group: 0 | Index: %0d | Port: %0d\n",(*z11),ovs_buf[(*z)]);
		bucket1[*z11] = ovs_buf[*z];
		(*z11)++;
		(*z)--;
	}
	else if (*z22 < grp_len) {
		TDM_PRINT2("Group: 1 | Index: %0d | Port: %0d\n",(*z22),ovs_buf[(*z)]);
		bucket2[*z22] = ovs_buf[*z];
		(*z22)++;
		(*z)--;
	}
	else if (*z33 < grp_len) {
		TDM_PRINT2("Group: 2 | Index: %0d | Port: %0d\n",(*z33),ovs_buf[(*z)]);
		bucket3[*z33] = ovs_buf[*z];
		(*z33)++;
		(*z)--;
	}
	else if (*z44 < grp_len) {
		TDM_PRINT2("Group: 3 | Index: %0d | Port: %0d\n",(*z44),ovs_buf[(*z)]);
		bucket4[*z44] = ovs_buf[*z];
		(*z44)++;
		(*z)--;
	}
	else if (*z55 < grp_len) {
		TDM_PRINT2("Group: 4 | Index: %0d | Port: %0d\n",(*z55),ovs_buf[(*z)]);
		bucket5[*z55] = ovs_buf[*z];
		(*z55)++;
		(*z)--;
	}
	else if (*z66 < grp_len) {
		TDM_PRINT2("Group: 5 | Index: %0d | Port: %0d\n",(*z66),ovs_buf[(*z)]);
		bucket6[*z66] = ovs_buf[*z];
		(*z66)++;
		(*z)--;
	}
	else if (*z77 < grp_len) {
		TDM_PRINT2("Group: 6 | Index: %0d | Port: %0d\n",(*z77),ovs_buf[(*z)]);
		bucket7[*z77] = ovs_buf[*z];
		(*z77)++;
		(*z)--;
	}
	else if (*z88 < grp_len) {
		TDM_PRINT2("Group: 7 | Index: %0d | Port: %0d\n",(*z88),ovs_buf[(*z)]);
		bucket8[*z88] = ovs_buf[*z];
		(*z88)++;
		(*z)--;
	}
	else {
		return FAIL;
	}
	
	return PASS;
}


/**
@name: tdm_core_vbs_scheduler_ovs
@param:

Generate sortable weightable groups for oversub round robin arbitration
 */
int
tdm_core_vbs_scheduler_ovs( tdm_mod_t *_tdm )
{	
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
	TDM_BIG_BAR
	TDM_PRINT9("(1G - %0d) (10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) (120G - %0d) (Number of Oversub Types - %0d)\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7, (_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100 + _tdm->_core_data.vars_pkg.os_120));
	if ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100 + _tdm->_core_data.vars_pkg.os_120) > cal->grp_num) {
		TDM_ERROR0("Oversub speed type limit exceeded\n");
		return FAIL;
	}
	if (((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)==cal->grp_num && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>cal->grp_len || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>cal->grp_len || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2>cal->grp_len)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-1) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>32 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>32 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2>32)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-2) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>48 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>48)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-3) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>TDM_AUX_SIZE || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>TDM_AUX_SIZE))) {
		TDM_ERROR0("Oversub bucket overflow\n");
		return FAIL;
	}
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8 > 0) {
		_tdm->_core_data.vars_pkg.grp_id=8;
		if (tdm_fill_ovs(_tdm)==FAIL) {
			TDM_ERROR0("Could not sort 1G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;	
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1 > 0) {
		_tdm->_core_data.vars_pkg.grp_id=1;
		if (tdm_fill_ovs(_tdm)==FAIL) {
			TDM_ERROR0("Could not sort 10G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2 > 0) {
		_tdm->_core_data.vars_pkg.grp_id=2;
		if (tdm_fill_ovs(_tdm)==FAIL) {
			TDM_ERROR0("Could not sort 20G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6 > 0) {
		_tdm->_core_data.vars_pkg.grp_id=6;
		if (tdm_fill_ovs(_tdm)==FAIL) {
			TDM_ERROR0("Could not sort 25G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3 > 0) {
		if (tdm_fill_ovs_simple((&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3)), _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, cal->cal_grp[0], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)), cal->cal_grp[1], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)), cal->cal_grp[2], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)), cal->cal_grp[3], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)), cal->cal_grp[4], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)), cal->cal_grp[5], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)), cal->cal_grp[6], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)), cal->cal_grp[7], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)), cal->grp_len)==FAIL) {
			TDM_ERROR0("Could not sort 40G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5 > 0) {
		if (tdm_fill_ovs_simple((&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5)), _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, cal->cal_grp[0], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)), cal->cal_grp[1], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)), cal->cal_grp[2], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)), cal->cal_grp[3], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)), cal->cal_grp[4], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)), cal->cal_grp[5], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)), cal->cal_grp[6], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)), cal->cal_grp[7], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)), cal->grp_len)==FAIL) {
			TDM_ERROR0("Could not sort 50G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;	
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4 > 0) {
		if (tdm_fill_ovs_simple((&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4)), _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, cal->cal_grp[0], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)), cal->cal_grp[1], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)), cal->cal_grp[2], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)), cal->cal_grp[3], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)), cal->cal_grp[4], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)), cal->cal_grp[5], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)), cal->cal_grp[6], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)), cal->cal_grp[7], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)), cal->grp_len)==FAIL) {
			TDM_ERROR0("Could not sort 100G oversub speed groups\n");
			break;
		}
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77>0) ? cal->grp_len:0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88 = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88>0) ? cal->grp_len:0;
	while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7 > 0) {
		if (tdm_fill_ovs_simple((&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7)), _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120, cal->cal_grp[0], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11)), cal->cal_grp[1], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22)), cal->cal_grp[2], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33)), cal->cal_grp[3], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44)), cal->cal_grp[4], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55)), cal->cal_grp[5], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66)), cal->cal_grp[6], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77)), cal->cal_grp[7], (&(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88)), cal->grp_len)==FAIL) {
			TDM_ERROR0("Could not sort 120G oversub speed groups\n");
			break;
		}
	}
	
	return PASS;
}


/**
@name: tdm_ovsb_fill_os_grps
@param:

Fill oversub ports into oversub speed groups
 */
int
tdm_ovsb_fill_os_grps(tdm_mod_t *_tdm)
{
    int n, prt, spd, spd_idx, grp_idx, prt_idx, grp_req, prt_cnt,
        result = PASS;
    m_tdm_pipe_info_t *pipe_info;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    pipe_info = &(_tdm->_core_data.vars_pkg.pipe_info);
    cal = tdm_cmn_get_pipe_cal(_tdm);
    if (pipe_info == NULL || cal == NULL) return result;

    if (pipe_info->os_en) {
        for (spd_idx = 0; spd_idx < MAX_SPEED_TYPES; spd_idx++) {
            if (pipe_info->os_spd_en[spd_idx] == 0) continue;

            /* calc number of groups required by ports with the same speed */
            grp_req = (cal->grp_len > 0) ?
                      ((pipe_info->os_prt_cnt[spd_idx]-1)/cal->grp_len+1) : 0;

            /* allocate ports with the same speed into ovsb groups */
            prt_cnt = 0;
            spd = tdm_cmn_get_port_speed(_tdm, pipe_info->os_prt[spd_idx][0]);
            for (n = 0; n < grp_req; n++) {
                /* find the first empty group */
                grp_idx = 0;
                while (cal->cal_grp[grp_idx][0] != param_token_empty &&
                       grp_idx < cal->grp_num) grp_idx++;
                /* allocate ports into group */
                if (grp_idx < cal->grp_num) {
                    for (prt_idx = 0; prt_idx < cal->grp_len &&
                         prt_idx < pipe_info->os_prt_cnt[spd_idx]; prt_idx++) {
                        prt = pipe_info->os_prt[spd_idx][prt_cnt++];
                        if (prt == param_token_empty) break;
                        cal->cal_grp[grp_idx][prt_idx] = prt;
                        TDM_PRINT4("TDM: Assign %3dG port %3d to grp[%0d][%0d]\n",
                                   spd/1000, prt, grp_idx, prt_idx);
                    }
                } else {
                    result = FAIL;
                    prt = pipe_info->os_prt[spd_idx][prt_cnt];
                    TDM_ERROR3("%s, port %3d, speed %0dG\n",
                               "OVSB speed groups overflow",
                               prt, tdm_cmn_get_port_speed(_tdm, prt)/1000);
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}


/**
@name: tdm_core_ovsb_alloc
@param:

Generate oversub speed groups for oversub round robin arbitration
 */
int
tdm_core_ovsb_alloc(tdm_mod_t *_tdm)
{
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Generating OVSB speed groups\n\n");
    /* check oversub speed types */
    if (tdm_cmn_chk_pipe_os_spd_types(_tdm) != PASS) return FAIL;

    /* populate speed groups */
    if (tdm_ovsb_fill_os_grps(_tdm) != PASS) return FAIL;

    /* balance speed groups with the same speed (optional) */

    return PASS;
}
