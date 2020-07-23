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


/**
@name: tdm_th2_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_th2_corereq( tdm_mod_t *_tdm )
{
	int idx1;
	
	_tdm->_core_data.vars_pkg.cal_id=_tdm->_core_data.vars_pkg.pipe;
	if (_tdm->_core_data.vars_pkg.pipe==0) {
		_tdm->_core_data.vmap=(unsigned short **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned short *), "vector_map_l1");
		for (idx1=0; idx1<(_tdm->_core_data.vmap_max_wid); idx1++) {
			_tdm->_core_data.vmap[idx1]=(unsigned short *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned short), "vector_map_l2");
		}
	}
	return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_th2_scheduler_wrap
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_th2_scheduler_wrap( tdm_mod_t *_tdm )
{
	int iter, idx1=0, idx2=0, ethernet_encap=BOOL_TRUE;
	int tdm_cal_length=0;
	
	_tdm->_core_data.vars_pkg.pipe=(_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_start/64);
	if (_tdm->_core_data.vars_pkg.pipe>3) {
		TDM_ERROR1("Invalid pipe ID - %0d\n",_tdm->_core_data.vars_pkg.pipe);
		return (TDM_EXEC_CHIP_SIZE+1);
	}
	tdm_th2_parse_pipe(_tdm);
	for (iter=0; iter<TDM_AUX_SIZE; iter++) {
		_tdm->_core_data.vars_pkg.lr_buffer[iter]=TH2_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.os_buffer[iter]=TH2_NUM_EXT_PORTS;
	}
	for (iter=(_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_start-1); iter<(_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_end); iter++) {
		if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE_HG) ) {
			if (idx1<64) {
				_tdm->_core_data.vars_pkg.lr_buffer[idx1]=(iter+1);
				idx1++;
			}
			else {
				TDM_PRINT0("Line rate queue overflow, ports may have been skipped.\n");
			}
		}
		else if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB_HG) ) {
			if (idx2<64) {
				_tdm->_core_data.vars_pkg.os_buffer[idx2]=(iter+1);
				idx2++;
			}
			else {
				TDM_PRINT0("Oversub queue overflow, ports may have been skipped.\n");
			}
		}
	}
	if ( (_tdm->_chip_data.soc_pkg.soc_vars.th2.mgmt_pm_hg==BOOL_TRUE) && 
		 (_tdm->_core_data.vars_pkg.pipe==1||_tdm->_core_data.vars_pkg.pipe==2) &&
		 ( ((_tdm->_core_data.vars_pkg.lr_buffer[0]!=TH2_NUM_EXT_PORTS) && (_tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ))||
		   (_tdm->_core_data.vars_pkg.lr_buffer[0]==TH2_NUM_EXT_PORTS) ) ) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.higig_mgmt=BOOL_TRUE;
	}
	for (iter=0; iter<TDM_AUX_SIZE; iter++) {
		if (_tdm->_core_data.vars_pkg.lr_buffer[iter]!=TH2_NUM_EXT_PORTS) {
			TDM_PUSH(_tdm->_core_data.vars_pkg.lr_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
			if (!ethernet_encap) {
				break;
			}
		}
	}
	if ((ethernet_encap)&&(!_tdm->_chip_data.soc_pkg.soc_vars.th2.higig_mgmt)&&
	    (_tdm->_chip_data.soc_pkg.soc_vars.th2.cal_universal_en == 0)) {
		switch (_tdm->_chip_data.soc_pkg.clk_freq) {
			case 1700 : tdm_cal_length = LEN_1700MHZ_EN; break;
			case 1625 : tdm_cal_length = LEN_1625MHZ_EN; break;
			case 1525 : tdm_cal_length = LEN_1525MHZ_EN; break;
			case 1425 : tdm_cal_length = LEN_1425MHZ_EN; break;
			case 1325 : tdm_cal_length = LEN_1325MHZ_EN; break;
			case 1275 : tdm_cal_length = LEN_1275MHZ_EN; break;
			case 1225 : tdm_cal_length = LEN_1225MHZ_EN; break;
			case 1133 : tdm_cal_length = LEN_1133MHZ_EN; break;
			case 1125 : tdm_cal_length = LEN_1125MHZ_EN; break;
			case 1050 : tdm_cal_length = LEN_1050MHZ_EN; break;
			case  950 : tdm_cal_length =  LEN_950MHZ_EN; break;
			case  850 : tdm_cal_length =  LEN_850MHZ_EN; break;
			default:
				TDM_PRINT1("Invalid frequency - %0dMHz not supported\n",_tdm->_chip_data.soc_pkg.clk_freq);
				return 0;
				break;
		}		
	}
	else {
		switch (_tdm->_chip_data.soc_pkg.clk_freq) {
			case 1700 : tdm_cal_length = LEN_1700MHZ_HG; break;
			case 1625 : tdm_cal_length = LEN_1625MHZ_HG; break;
			case 1525 : tdm_cal_length = LEN_1525MHZ_HG; break;
			case 1425 : tdm_cal_length = LEN_1425MHZ_HG; break;
			case 1325 : tdm_cal_length = LEN_1325MHZ_HG; break;
			case 1275 : tdm_cal_length = LEN_1275MHZ_HG; break;
			case 1225 : tdm_cal_length = LEN_1225MHZ_HG; break;
			case 1133 : tdm_cal_length = LEN_1133MHZ_HG; break;
			case 1125 : tdm_cal_length = LEN_1125MHZ_HG; break;
			case 1050 : tdm_cal_length = LEN_1050MHZ_HG; break;
			case  950 : tdm_cal_length =  LEN_950MHZ_HG; break;
			case  850 : tdm_cal_length =  LEN_850MHZ_HG; break;
			default:
				TDM_PRINT1("Invalid frequency - %0dMHz not supported\n",_tdm->_chip_data.soc_pkg.clk_freq);
				return 0;
				break;
		}	
	}
	_tdm->_chip_data.soc_pkg.lr_idx_limit = tdm_cal_length - _tdm->_chip_data.soc_pkg.tvec_size;
	_tdm->_core_data.vmap_max_wid=TH2_VMAP_MAX_WID;
	_tdm->_core_data.vmap_max_len=TH2_VMAP_MAX_LEN;

	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_th2_pmap_transcription
@param:


Transcription algorithm for generating port module mapping
 */
int
tdm_th2_pmap_transcription( tdm_mod_t *_tdm )
{
	int i, j, last_port=TH2_NUM_EXT_PORTS;
	/*char tmp_str[16];*/
	/*sprintf (cmd_str, "soc setmem -index %x %s {", index->d, addr);*/
	
	for (i=0; i<TH2_NUM_PHY_PM; i++) {
		for (j=0; j<TH2_NUM_PM_LNS; j++) {
			_tdm->_chip_data.soc_pkg.pmap[i][j]=TH2_NUM_EXT_PORTS;
		}
	}
	for (i=1; i<=TH2_NUM_PHY_PORTS; i+=TH2_NUM_PM_LNS) {
			last_port = TH2_NUM_EXT_PORTS;
			for (j=0; j<TH2_NUM_PM_LNS; j++) {
				switch (_tdm->_chip_data.soc_pkg.state[i+j]) {
					case PORT_STATE__LINERATE: case PORT_STATE__OVERSUB:
					case PORT_STATE__LINERATE_HG: case PORT_STATE__OVERSUB_HG:
						if (_tdm->_chip_data.soc_pkg.speed[i+j]>SPEED_0) {
							last_port=(i+j);
						}
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH2_NUM_PM_LNS][j]=last_port;
						break;
					case PORT_STATE__COMBINE: case PORT_STATE__COMBINE_HG:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH2_NUM_PM_LNS][j]=last_port;
						break;
					default:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH2_NUM_PM_LNS][j]=TH2_NUM_EXT_PORTS;
						break;
				}
			}
	}
	tdm_print_stat( _tdm );
	for (i=0; i<(TH2_NUM_EXT_PORTS-1); i++) {
		_tdm->_chip_data.soc_pkg.state[i]=_tdm->_chip_data.soc_pkg.state[i+1];
	}
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_th2_dealloc
@param:
 */
int
tdm_th2_dealloc( tdm_mod_t *_tdm )
{
	int index;


	TDM_PRINT0("Free pmap\n");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		TDM_FREE(_tdm->_chip_data.soc_pkg.pmap[index]);
	}
	TDM_FREE(_tdm->_chip_data.soc_pkg.pmap);

	/* Pipe 0 calendar group */

	TDM_FREE(_tdm->_chip_data.cal_0.cal_main);

	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0]);
	TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_1]);
	TDM_FREE(_tdm->_chip_data.cal_0.cal_grp);

	TDM_PRINT0("Free PIPE 1 \n");
	/* Pipe 1 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_1.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_1.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0]);
	TDM_FREE(_tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_1]);
	TDM_FREE(_tdm->_chip_data.cal_1.cal_grp);

	TDM_PRINT0("Free PIPE 2 \n");
	/* Pipe 2 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_2.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0]);
	TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_1]);
	TDM_FREE(_tdm->_chip_data.cal_2.cal_grp);

	TDM_PRINT0("Free PIPE 3 \n");
	/* Pipe 3 calendar group */
	TDM_FREE(_tdm->_chip_data.cal_3.cal_main);
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		TDM_FREE(_tdm->_chip_data.cal_3.cal_grp[index]);
	}
	TDM_FREE(_tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0]);
	TDM_FREE(_tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_1]);
	TDM_FREE(_tdm->_chip_data.cal_3.cal_grp);

	TDM_PRINT0("Free MMU cal 0-3 \n");
	/* Pipe 0 calendar group MMU mirror */
	TDM_FREE(_tdm->_chip_data.cal_4.cal_main);
	/* Pipe 1 calendar group MMU mirror */
	TDM_FREE(_tdm->_chip_data.cal_5.cal_main);
	/* Pipe 2 calendar group MMU mirror */
	TDM_FREE(_tdm->_chip_data.cal_6.cal_main);
	/* Pipe 3 calendar group MMU mirror */
	TDM_FREE(_tdm->_chip_data.cal_7.cal_main);


	TDM_PRINT0("Free vmap \n");	
	for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
		TDM_FREE(_tdm->_core_data.vmap[index]);
	}
	TDM_FREE(_tdm->_core_data.vmap);

	TDM_PRINT0("Free FINISH \n");
	
	TDM_PRINT0("Start free for FlexPort \n");

        /* If FlexPort, deallocate previous TDM tables */
        if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {

            /* Pipe 0 calendar group */
            TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_main);
            for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
                TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[index]);
            }
            TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0]);
            TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_1]);
            TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp);

            /* Pipe 1 calendar group */
            TDM_FREE(_tdm->_prev_chip_data.cal_1.cal_main);
            for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
                TDM_FREE(_tdm->_prev_chip_data.cal_1.cal_grp[index]);
            }
            TDM_FREE(_tdm->_prev_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0]);
            TDM_FREE(_tdm->_prev_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_1]);
            TDM_FREE(_tdm->_prev_chip_data.cal_1.cal_grp);

            /* Pipe 2 calendar group */
            TDM_FREE(_tdm->_prev_chip_data.cal_2.cal_main);
            for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
                TDM_FREE(_tdm->_prev_chip_data.cal_2.cal_grp[index]);
            }
            TDM_FREE(_tdm->_prev_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0]);
            TDM_FREE(_tdm->_prev_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_1]);
            TDM_FREE(_tdm->_prev_chip_data.cal_2.cal_grp);

            /* Pipe 3 calendar group */
            TDM_FREE(_tdm->_prev_chip_data.cal_3.cal_main);
            for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
                TDM_FREE(_tdm->_prev_chip_data.cal_3.cal_grp[index]);
            }
            TDM_FREE(_tdm->_prev_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0]);
            TDM_FREE(_tdm->_prev_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_1]);
            TDM_FREE(_tdm->_prev_chip_data.cal_3.cal_grp);

            /* Pipe 0 calendar group MMU mirror */
            TDM_FREE(_tdm->_prev_chip_data.cal_4.cal_main);
            /* Pipe 1 calendar group MMU mirror */
            TDM_FREE(_tdm->_prev_chip_data.cal_5.cal_main);
            /* Pipe 2 calendar group MMU mirror */
            TDM_FREE(_tdm->_prev_chip_data.cal_6.cal_main);
            /* Pipe 3 calendar group MMU mirror */
            TDM_FREE(_tdm->_prev_chip_data.cal_7.cal_main);

            TDM_PRINT0("Finish free for FlexPort \n");
        }

	return 1;
}


/**
@name: tdm_th2_alloc_prev_chip_data
@param:

 * Description:
 *      Helper function to allocate previous TDM tables in in _tdm_pkg->_prev_chip_data at FlexPort
 */
int
tdm_th2_alloc_prev_chip_data( tdm_mod_t *_tdm_pkg )
{
    int index;

    /* Construct OLD tdm cal_0/1/2/3/4/5/6/7 */
    /* Pipe 0 calendar group */
    _tdm_pkg->_prev_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 0 main calendar");	
    _tdm_pkg->_prev_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((TH2_OS_VBS_GRP_NUM+2)*sizeof(int *), "OLD TDM inst 0 groups");
    for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
        _tdm_pkg->_prev_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((TH2_OS_VBS_GRP_LEN)*sizeof(int), "OLD TDM inst 0 group calendars");
    }
    _tdm_pkg->_prev_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 0 shaping calendars");
    _tdm_pkg->_prev_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 0 shaping calendars");
    /* Pipe 1 calendar group */
    _tdm_pkg->_prev_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 1 main calendar");	
    _tdm_pkg->_prev_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((TH2_OS_VBS_GRP_NUM+2)*sizeof(int *), "OLD TDM inst 1 groups");
    for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
        _tdm_pkg->_prev_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((TH2_OS_VBS_GRP_LEN)*sizeof(int), "OLD TDM inst 1 group calendars");
    }
    _tdm_pkg->_prev_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 1 shaping calendars");
    _tdm_pkg->_prev_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 1 shaping calendars");
    /* Pipe 2 calendar group */
    _tdm_pkg->_prev_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 2 main calendar");	
    _tdm_pkg->_prev_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((TH2_OS_VBS_GRP_NUM+2)*sizeof(int *), "OLD TDM inst 2 groups");
    for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
        _tdm_pkg->_prev_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((TH2_OS_VBS_GRP_LEN)*sizeof(int), "OLD TDM inst 2 group calendars");
    }
    _tdm_pkg->_prev_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 2 shaping calendars");
    _tdm_pkg->_prev_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 2 shaping calendars");
    /* Pipe 3 calendar group */
    _tdm_pkg->_prev_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 3 main calendar");	
    _tdm_pkg->_prev_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((TH2_OS_VBS_GRP_NUM+2)*sizeof(int *), "OLD TDM inst 3 groups");
    for (index=0; index<(TH2_OS_VBS_GRP_NUM); index++) {
        _tdm_pkg->_prev_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((TH2_OS_VBS_GRP_LEN)*sizeof(int), "OLD TDM inst 3 group calendars");
    }
    _tdm_pkg->_prev_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 3 shaping calendars");
    _tdm_pkg->_prev_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "OLD TDM inst 3 shaping calendars");
    /* Pipe 0 calendar group MMU mirror */
    _tdm_pkg->_prev_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 0 mirror calendar");
    /* Pipe 1 calendar group MMU mirror */
    _tdm_pkg->_prev_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 1 mirror calendar");
    /* Pipe 2 calendar group MMU mirror */
    _tdm_pkg->_prev_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 2 mirror calendar");
    /* Pipe 3 calendar group MMU mirror */
    _tdm_pkg->_prev_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((TH2_LR_VBS_LEN)*sizeof(int), "OLD TDM inst 3 mirror calendar");

    return 1;
}

/**
@name: tdm_th2_init
@param:
 */
int
tdm_th2_init( tdm_mod_t *_tdm )
{
	int index;
	
	_tdm->_chip_data.soc_pkg.pmap_num_modules = TH2_NUM_PM_MOD;
	_tdm->_chip_data.soc_pkg.pmap_num_lanes = TH2_NUM_PM_LNS;
	_tdm->_chip_data.soc_pkg.pm_num_phy_modules = TH2_NUM_PHY_PM;
	
	_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = TH2_OVSB_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl1_token = TH2_IDL1_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl2_token = TH2_IDL2_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.ancl_token = TH2_ANCL_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = TH2_NUM_PHY_PORTS;
	
	_tdm->_chip_data.cal_0.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_0.grp_num = TH2_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_0.grp_len = TH2_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_1.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_1.grp_num = TH2_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_1.grp_len = TH2_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_2.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_2.grp_num = TH2_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_2.grp_len = TH2_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_3.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_3.grp_num = TH2_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_3.grp_len = TH2_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_4.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_5.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_6.cal_len = TH2_LR_VBS_LEN;
	_tdm->_chip_data.cal_7.cal_len = TH2_LR_VBS_LEN;
	
	_tdm->_core_data.vars_pkg.pipe=0;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.higig_mgmt=BOOL_FALSE;
	_tdm->_chip_data.soc_pkg.lr_idx_limit=LEN_850MHZ_EN;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_start=1;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_end=64;
	_tdm->_chip_data.soc_pkg.tvec_size = TH2_ACC_PORT_NUM;
	
	_tdm->_core_data.rule__same_port_min = TH2_LR_VBS_LEN;
	_tdm->_core_data.rule__prox_port_min = TH2_VBS_MIN_SPACING;
	
	for (index=0; index<TH2_NUM_PM_MOD; index++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*TH2_NUM_PM_LNS]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index]==PORT_STATE__OVERSUB_HG)?(999):(998);
	}
	_tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		_tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	}
	/* Pipe 0 calendar group */
	_tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
	_tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num+2)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		_tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
	}
	_tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 0 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	_tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 0 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_1],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	/* Pipe 1 calendar group */
	_tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
	_tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num+2)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		_tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
	}
	_tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 1 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	_tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 1 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_1],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	/* Pipe 2 calendar group */
	_tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
	_tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num+2)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		_tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
	}
	_tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 2 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	_tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 2 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_1],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	/* Pipe 3 calendar group */
	_tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
	_tdm->_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_num+2)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		_tdm->_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.grp_len);
	}
	_tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 3 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	_tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_1]=(int *) TDM_ALLOC((SHAPING_GRP_LEN)*sizeof(int), "TDM inst 3 shaping calendars");
	TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_1],(_tdm->_chip_data.soc_pkg.num_ext_ports),SHAPING_GRP_LEN);
	/* Pipe 0 calendar group MMU mirror */
	_tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 0 mirror calendar");	
	TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);	
	/* Pipe 1 calendar group MMU mirror */
	_tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 1 mirror calendar");	
	TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);	
	/* Pipe 2 calendar group MMU mirror */
	_tdm->_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.cal_len)*sizeof(int), "TDM inst 2 mirror calendar");	
	TDM_MSET(_tdm->_chip_data.cal_6.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.cal_len);	
	/* Pipe 3 calendar group MMU mirror */
	_tdm->_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.cal_len)*sizeof(int), "TDM inst 3 mirror calendar");	
	TDM_MSET(_tdm->_chip_data.cal_7.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.cal_len);	
	
	for (index=(TH2_NUM_EXT_PORTS-1); index>0; index--) {
		_tdm->_chip_data.soc_pkg.state[index]=_tdm->_chip_data.soc_pkg.state[index-1];
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/**
@name: tdm_th2_post
@param:
 */
int
tdm_th2_post( tdm_mod_t *_tdm )
{
	_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_start+=64;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_end+=64;
	
	return (_tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_end>TH2_NUM_EXT_PORTS)?(PASS):( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}
