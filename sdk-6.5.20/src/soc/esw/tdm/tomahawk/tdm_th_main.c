/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip main functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif

/* #ifdef _TDM_DB_STACK
    size_t stack_size = 0;
#endif */

/**
@name: tdm_th_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_th_corereq( tdm_mod_t *_tdm )
{
    _tdm->_core_data.vars_pkg.cal_id=_tdm->_core_data.vars_pkg.pipe;

    return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_th_scheduler_wrap_pipe
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_th_scheduler_wrap_pipe( tdm_mod_t *_tdm )
{
    int iter, idx1=0, idx2=0, ethernet_encap=BOOL_TRUE;
    int param_cal_len=0, param_lr_limit=0, param_ancl_num=0, 
        param_clk_freq;
    
	_tdm->_core_data.vars_pkg.pipe=(_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start/32);
	if (_tdm->_core_data.vars_pkg.pipe>3) {
		TDM_ERROR1("Invalid pipe ID - %0d\n",_tdm->_core_data.vars_pkg.pipe);
		return (TDM_EXEC_CHIP_SIZE+1);
	}
	tdm_th_parse_pipe(_tdm);
	for (iter=0; iter<TDM_AUX_SIZE; iter++) {
		_tdm->_core_data.vars_pkg.lr_buffer[iter]=TH_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.os_buffer[iter]=TH_NUM_EXT_PORTS;
	}
	for (iter=(_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start-1); iter<(_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end); iter++) {
		if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE_HG) ) {
			if (idx1<TDM_AUX_SIZE){
				_tdm->_core_data.vars_pkg.lr_buffer[idx1++]=(iter+1);
			}
			if (idx1>32){
				TDM_PRINT2("WARNING: Pipe %d, line rate queue overflow, port %d may have been skipped.\n", _tdm->_core_data.vars_pkg.pipe, (iter+1));
			}
		}
		else if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB_HG) ) {
			if (idx2<TDM_AUX_SIZE){
				_tdm->_core_data.vars_pkg.os_buffer[idx2++]=(iter+1);
			}
			if (idx2>32){
				TDM_PRINT2("WARNING: Pipe %d, oversub queue overflow, port %d may have been skipped.\n", _tdm->_core_data.vars_pkg.pipe, (iter+1));
			}
		}
	}
	if ( (_tdm->_chip_data.soc_pkg.soc_vars.th.mgmt_pm_hg==BOOL_TRUE) && 
		 (_tdm->_core_data.vars_pkg.pipe==1||_tdm->_core_data.vars_pkg.pipe==2) &&
		 ( (_tdm->_core_data.vars_pkg.lr_buffer[0]!=TH_NUM_EXT_PORTS&&_tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ)||
		   (_tdm->_core_data.vars_pkg.lr_buffer[0]==TH_NUM_EXT_PORTS) ) ) {
		_tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt=BOOL_TRUE;
	}
    else {
        _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt=BOOL_FALSE;
    }
	for (iter=0; iter<TDM_AUX_SIZE; iter++) {
		if (_tdm->_core_data.vars_pkg.lr_buffer[iter]!=TH_NUM_EXT_PORTS) {
			TDM_PUSH(_tdm->_core_data.vars_pkg.lr_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
			if (!ethernet_encap) {
				break;
			}
		}
		if (_tdm->_core_data.vars_pkg.os_buffer[iter]!=TH_NUM_EXT_PORTS) {
			TDM_PUSH(_tdm->_core_data.vars_pkg.os_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
			if (!ethernet_encap) {
				break;
			}
		}
	}
    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
    if ((_tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE)&&(ethernet_encap) &&
        (_tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) ) {
        switch (param_clk_freq) {
            case TH_CLK_950MHZ: param_cal_len = TH_LEN_950MHZ_EN; break;
            case TH_CLK_850MHZ: param_cal_len = TH_LEN_850MHZ_EN; break;
            case TH_CLK_765MHZ: param_cal_len = TH_LEN_765MHZ_EN; break;
            case TH_CLK_672MHZ: param_cal_len = TH_LEN_672MHZ_EN; break;
            case TH_CLK_645MHZ: param_cal_len = TH_LEN_645MHZ_EN; break;
            case TH_CLK_545MHZ: param_cal_len = TH_LEN_545MHZ_EN; break;
            default:
                TDM_PRINT1("Invalid frequency - %0dMHz not supported\n",param_clk_freq);
                return 0;
                break;
        }
    }
    else {
        switch (param_clk_freq) {
            case TH_CLK_950MHZ: param_cal_len = TH_LEN_950MHZ_HG; break;
            case TH_CLK_850MHZ: param_cal_len = TH_LEN_850MHZ_HG; break;
            case TH_CLK_765MHZ: param_cal_len = TH_LEN_765MHZ_HG; break;
            case TH_CLK_672MHZ: param_cal_len = TH_LEN_672MHZ_HG; break;
            case TH_CLK_645MHZ: param_cal_len = TH_LEN_645MHZ_HG; break;
            case TH_CLK_545MHZ: param_cal_len = TH_LEN_545MHZ_HG; break;
            default:
                TDM_PRINT1("Invalid frequency - %0dMHz not supported\n",param_clk_freq);
                return 0;
                break;
        }
    }
    param_ancl_num = TH_ACC_PORT_NUM;
    param_lr_limit = param_cal_len - param_ancl_num;

    _tdm->_chip_data.soc_pkg.tvec_size    = param_ancl_num;
    _tdm->_chip_data.soc_pkg.lr_idx_limit = param_lr_limit;
    
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_th_scheduler_wrap
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_th_scheduler_wrap( tdm_mod_t *_tdm )
{
    int pipe_id, result=PASS;
    
    /* TDM_PRINT_STACK_SIZE("tdm_th_scheduler_wrap"); */
    
    for (pipe_id=0; pipe_id<4; pipe_id++){
        result = tdm_th_scheduler_wrap_pipe(_tdm);
        if (result!=PASS){
            break;
        }
    }
    
    return result;
}


/**
@name: tdm_th_pmap_transcription
@param:

For Tomahawk BCM56960
Transcription algorithm for generating port module mapping
 */
int
tdm_th_pmap_transcription( tdm_mod_t *_tdm )
{
	int i, j, port_lanenum, last_port=TH_NUM_EXT_PORTS;
	int phy_lo, phy_hi, pm_lane_num, bcm_config_check=PASS;
	
	/* initialize pmap */
	for (i=0; i<TH_NUM_PHY_PM; i++) {
		for (j=0; j<TH_NUM_PM_LNS; j++) {
			_tdm->_chip_data.soc_pkg.pmap[i][j]=TH_NUM_EXT_PORTS;
		}
	}
	/* Set combination state */
	for (i=1; i<=TH_NUM_PHY_PORTS; i++){
		if(_tdm->_chip_data.soc_pkg.speed[i]>=SPEED_40G){
			switch(_tdm->_chip_data.soc_pkg.speed[i]/1000){
				case 106:
				case 100: 
						port_lanenum = 4; break;
				case 53 :
				case 50 : 
						port_lanenum =  2; break;
				case 42 :
				case 40 : 
						port_lanenum =  2; break;
				default: 
						port_lanenum =  0; break;
			}
			for (j=1; j<port_lanenum; j++){
				if ((i+j)<=TH_NUM_PHY_PORTS){
					_tdm->_chip_data.soc_pkg.state[i+j-1] = PORT_STATE__COMBINE;
				}
				else{
					break;
				}
			}
		}
		else if ( (_tdm->_chip_data.soc_pkg.speed[i]/1000)==20 || (_tdm->_chip_data.soc_pkg.speed[i]/1000)==21 ){
			if (_tdm->_chip_data.soc_pkg.speed[i+1]==SPEED_0 && (i+1)<=TH_NUM_PHY_PORTS ){
				_tdm->_chip_data.soc_pkg.state[i] = PORT_STATE__DISABLED;
			}
		}
	}
	/* Port transcription */
	for (i=1; i<=TH_NUM_PHY_PORTS; i+=TH_NUM_PM_LNS) {
		if (_tdm->_chip_data.soc_pkg.speed[i]>SPEED_0) {
			for (j=0; j<TH_NUM_PM_LNS; j++) {
				switch (_tdm->_chip_data.soc_pkg.state[i+j-1]) {
					case PORT_STATE__LINERATE: 
					case PORT_STATE__OVERSUB:
					case PORT_STATE__LINERATE_HG: 
					case PORT_STATE__OVERSUB_HG:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][j]=(i+j);
						last_port=(i+j);
						break;
					case PORT_STATE__COMBINE:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][j]=last_port;
						break;
					default:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][j]=TH_NUM_EXT_PORTS;
						break;
				}
			}
			/* tri mode x_xx */
			if (_tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3]&&_tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0&&_tdm->_chip_data.soc_pkg.speed[i]>=SPEED_40G) {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][0];
			}
			/* tri mode xxx_ */
			else if (_tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1]&&_tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0&&_tdm->_chip_data.soc_pkg.speed[i+2]>=SPEED_40G)  {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][3];
			}
			/* dual mode x_x_ */
			else if (_tdm->_chip_data.soc_pkg.speed[i]!=_tdm->_chip_data.soc_pkg.speed[i+1]&&_tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i]>=SPEED_40G) {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][3];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][0];
			}
		}
	}
	/* print port config after transcription */
	tdm_print_config( _tdm );
	
    /* Check if invalid port config exists */
    phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    pm_lane_num= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    for (i=phy_lo; i<=(phy_hi-pm_lane_num); i+=pm_lane_num) {
        if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_0){
            for (j=1; j<pm_lane_num; j++) {
                if (_tdm->_chip_data.soc_pkg.speed[i+j]>SPEED_0){
                    bcm_config_check = FAIL;
                    break;
                }
            }
        }
        if (bcm_config_check==FAIL){
            TDM_ERROR8("Invalid port configuration, port [%3d, %3d, %3d, %3d], speed [%3dG, %3dG, %3dG, %3dG]\n",
                i, i+1, i+2, i+3,
                _tdm->_chip_data.soc_pkg.speed[i]/1000,
                _tdm->_chip_data.soc_pkg.speed[i+1]/1000,
                _tdm->_chip_data.soc_pkg.speed[i+2]/1000,
                _tdm->_chip_data.soc_pkg.speed[i+3]/1000);
            return FAIL;
        }
    }
    
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_th_init
@param:
 */
int
tdm_th_init( tdm_mod_t *_tdm )
{
	int index;
    
	/* TDM_PRINT_STACK_SIZE("tdm_th_init"); */
	
	/* initialize chip/core parameters */
	_tdm->_chip_data.soc_pkg.pmap_num_modules = TH_NUM_PM_MOD;
	_tdm->_chip_data.soc_pkg.pmap_num_lanes = TH_NUM_PM_LNS;
	_tdm->_chip_data.soc_pkg.pm_num_phy_modules = TH_NUM_PHY_PM;
	
	_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = TH_OVSB_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl1_token = TH_IDL1_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl2_token = TH_IDL2_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.ancl_token = TH_ANCL_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = TH_NUM_PHY_PORTS;
	
	_tdm->_chip_data.cal_0.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_0.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_0.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_1.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_1.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_1.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_2.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_2.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_2.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_3.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_3.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_3.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_4.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_4.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_4.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_5.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_5.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_5.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_6.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_6.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_6.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_7.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_7.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_7.grp_len = TH_OS_VBS_GRP_LEN;	
	
	_tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt=BOOL_FALSE;
	_tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en=BOOL_FALSE;
	_tdm->_chip_data.soc_pkg.lr_idx_limit=LEN_850MHZ_EN;
	_tdm->_chip_data.soc_pkg.tvec_size = TH_ACC_PORT_NUM;
	_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start=1;
	_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end=32;
	
	_tdm->_core_data.vars_pkg.pipe       = 0;
	_tdm->_core_data.rule__same_port_min = TH_MIN_SPACING_SAME_PORT;
	_tdm->_core_data.rule__prox_port_min = TH_MIN_SPACING_SISTER_PORT;
	_tdm->_core_data.vmap_max_wid        = TH_VMAP_MAX_WID;
	_tdm->_core_data.vmap_max_len        = TH_VMAP_MAX_LEN;
	
	/* Chip: cal_hg_en */
	if (_tdm->_chip_data.soc_pkg.soc_vars.th.cal_universal_en==BOOL_TRUE){
		_tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en=BOOL_TRUE;
	}
	/* Chip: encap */
	for (index=0; index<TH_NUM_PM_MOD; index++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*TH_NUM_PM_LNS]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index*TH_NUM_PM_LNS]==PORT_STATE__OVERSUB_HG)?(PM_ENCAP__HIGIG2):(PM_ENCAP__ETHRNT);
	}
	/* Chip: pmap */
	_tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		_tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	}
	/* Chip: IDB Pipe 0 calendar group */
	_tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
	_tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		_tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
	}
	/* Chip: IDB Pipe 1 calendar group */
	_tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
	_tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		_tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
	}
	/* Chip: IDB Pipe 2 calendar group */
	_tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
	_tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		_tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
	}
	/* Chip: IDB Pipe 3 calendar group */
	_tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
	_tdm->_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_num)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		_tdm->_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.grp_len);
	}
	/* Chip: MMU Pipe 0 calendar group */
	_tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 0 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);
	_tdm->_chip_data.cal_4.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
		_tdm->_chip_data.cal_4.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.grp_len);
	}
	/* Chip: MMU Pipe 1 calendar group */
	_tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
	_tdm->_chip_data.cal_5.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
		_tdm->_chip_data.cal_5.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_5.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.grp_len);
	}
	/* Chip: MMU Pipe 2 calendar group */
	_tdm->_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_6.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.cal_len);
	_tdm->_chip_data.cal_6.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_6.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_6.grp_num); index++) {
		_tdm->_chip_data.cal_6.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_6.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.grp_len);
	}
	/* Chip: MMU Pipe 3 calendar group */
	_tdm->_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_7.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.cal_len);
	_tdm->_chip_data.cal_7.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_7.grp_num)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_7.grp_num); index++) {
		_tdm->_chip_data.cal_7.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_7.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.grp_len);
	}
	/* Core: vmap */
	_tdm->_core_data.vmap=(unsigned short **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned short *), "vector_map_l1");
	for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
		_tdm->_core_data.vmap[index]=(unsigned short *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned short), "vector_map_l2");
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/**
@name: tdm_th_post
@param:
 */
int
tdm_th_post( tdm_mod_t *_tdm )
{
    /* TDM_PRINT_STACK_SIZE("tdm_th_post"); */
    
    _tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start+=32;
    _tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end+=32;

    if (_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end>128){
        TDM_SML_BAR
        TDM_PRINT0("\nTDM: TDM algorithm is completed.\n\n");
        TDM_SML_BAR
        
        /* TDM self-check */
        if (_tdm->_chip_data.soc_pkg.soc_vars.th.tdm_chk_en==BOOL_TRUE){
            _tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm);
        }
    }

    return PASS;
}


/**
@name: tdm_th_free
@param:
 */
int
tdm_th_free( tdm_mod_t *_tdm )
{
	int index;
	/* Chip: pmap */
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		TDM_FREE(_tdm->_chip_data.soc_pkg.pmap[index]);
	}
	TDM_FREE(_tdm->_chip_data.soc_pkg.pmap);
	/* Chip: IDB Pipe 0 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_0.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_0.cal_grp);
	/* Chip: IDB Pipe 1 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_1.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_1.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_1.cal_grp);
	/* Chip: IDB Pipe 2 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_2.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_2.cal_grp);
	/* Chip: IDB Pipe 3 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_3.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_3.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_3.cal_grp);
	/* Chip: MMU Pipe 0 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_4.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_4.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_4.cal_grp);
	/* Chip: MMU Pipe 1 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_5.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_5.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_5.cal_grp);
	/* Chip: MMU Pipe 2 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_6.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_6.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_6.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_6.cal_grp);
	/* Chip: MMU Pipe 3 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_7.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_7.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_7.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_7.cal_grp);
	/* Core: vmap */
	for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
		TDM_FREE(_tdm->_core_data.vmap[index]);
	}
	TDM_FREE(_tdm->_core_data.vmap);
	
	return PASS;
}
