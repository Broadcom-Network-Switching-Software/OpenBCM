/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix5_idb_flexport.c
 * Purpose:
 * Requires:
 */
 
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#include <soc/helix5_tdm.h>
#include <soc/flexport/helix5/helix5_flexport.h>
/*** START SDK API COMMON CODE ***/


/*! @file helix5_idb_flexport.c
 *  @brief IDB flex for Helix5.
 *  Details are shown below.
 */


/* Bubble MOP */
typedef struct soc_helix5_bmop_s {
    uint8 line_rate[SOC_SWITCH_BYPASS_MODE_LOW + 1];   /* Line-rate profile :: 
                                                        * 0->FULL, 1->L3, 2->L2 */
    uint8 oversub[SOC_SWITCH_BYPASS_MODE_LOW + 1];     /* Oversub profile :: 
                                                        * 0->FULL, 1->L3, 2->L2 */
} soc_helix5_bmop_t;

static const struct soc_helix5_bmop_cfg_s {
    int speed;
    soc_helix5_bmop_t bmop_enable;                  /* Bubble MOP enable */
} soc_helix5_bmop_cfg_tbl[] = {
    {    -1, {{0, 0, 0}, {0, 0, 0}}},       /* SAF     */
    { 10000, {{0, 0, 0}, {0, 0, 0}}},       /* 10GE    */
    { 11000, {{0, 0, 0}, {0, 0, 0}}},       /* HG[11]  */
    { 20000, {{1, 0, 0}, {1, 0, 0}}},       /* 20GE    */
    { 21000, {{1, 0, 0}, {1, 0, 0}}},       /* HG[21]  */
    { 25000, {{1, 0, 0}, {1, 0, 0}}},       /* 25GE    */
    { 27000, {{1, 0, 0}, {1, 0, 0}}},       /* HG[27]  */
    { 40000, {{1, 0, 0}, {1, 0, 0}}},       /* 40GE    */
    { 42000, {{1, 0, 0}, {1, 0, 0}}},       /* HG[42]  */
    { 50000, {{1, 0, 0}, {1, 0, 0}}},       /* 50GE    */
    { 53000, {{1, 0, 0}, {1, 0, 0}}},       /* HG[53]  */
    {100000, {{1, 0, 0}, {1, 0, 0}}},       /* 100GE   */
    {106000, {{1, 0, 0}, {1, 0, 0}}}        /* HG[106] */
};

static const struct soc_helix5_obm_setting_s {
    int discard_limit;
    int lossless_discard;
    int port_xoff;
    int port_xon;
    int lossless_xoff;
    int lossless_xon;
    int lossy_low_pri;
    int lossy_discard;
    int sop_discard_mode;
} soc_helix5_obm_settings[4][5] = {
    /* LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 - invalid */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff : Default value */
            576,             /* port_xon : Default value*/
            384,             /* lossless_xoff : Default value*/
            288,             /* lossless_xon : Default value*/
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 1 lane */
	    1532,            /* discard_limit */
            1532,            /* lossless_discard */
            1202,             /* port_xoff */
            1172,             /* port_xon */
            258,             /* lossless_xoff */
            248,              /* lossless_xon */
            68,             /* lossy_low_pri */
            133,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            764,            /* discard_limit */
            764,            /* lossless_discard */
            570,            /* port_xoff */
            540,            /* port_xon */
            108,             /* lossless_xoff */
            98,             /* lossless_xon */
            68,             /* lossy_low_pri */
            133,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff */
            576,             /* port_xon */
            384,             /* lossless_xoff */
            288,             /* lossless_xon */
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 4 lanes */
	    380,            /* discard_limit */
            380,            /* lossless_discard */
            256,            /* port_xoff */
            226,            /* port_xon */
            46,             /* lossless_xoff */
            36,             /* lossless_xon */
            68,             /* lossy_low_pri */
            133,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    { /* indexed by number of lanes used in the port */
        { /* 0 - invalid */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff : Default value */
            576,             /* port_xon : Default value*/
            384,             /* lossless_xoff : Default value*/
            288,             /* lossless_xon : Default value*/
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 1 lane */
	    1148,            /* discard_limit */
            4604,            /* lossless_discard */
            908,            /* port_xoff */
            878,            /* port_xon */
            136,             /* lossless_xoff */
            106,             /* lossless_xon */
            136,             /* lossy_low_pri */
            331,             /* lossy_discard */
            
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            2300,            /* discard_limit */
            4604,            /* lossless_discard */
            2020,            /* port_xoff */
            1990,            /* port_xon */
            324,             /* lossless_xoff */
            294,             /* lossless_xon */
            136,             /* lossy_low_pri */
            331,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff */
            576,             /* port_xon */
            384,             /* lossless_xoff */
            288,             /* lossless_xon */
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 4 lanes */
            4604,            /* discard_limit */
            4604,            /* lossless_discard */
            4195,             /* port_xoff */
            4165,             /* port_xon */
            774,             /* lossless_xoff */
            744,              /* lossless_xon */
            136,             /* lossy_low_pri */
            331,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },

    /* LOSSY Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 - invalid */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff : Default value */
            576,             /* port_xon : Default value*/
            384,             /* lossless_xoff : Default value*/
            288,             /* lossless_xon : Default value*/
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 1 lane */
            380,            /* discard_limit */
            1532,            /* lossless_discard */
            1532,            /* port_xoff */
            1532,            /* port_xon */
            1532,            /* lossless_xoff */
            1532,            /* lossless_xon */
            311,             /* lossy_low_pri */
            376,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            764,            /* discard_limit */
            1532,            /* lossless_discard */
            1532,            /* port_xoff */
            1532,            /* port_xon */
            1532,            /* lossless_xoff */
            1532,            /* lossless_xon */
            695,            /* lossy_low_pri */
            760,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff */
            576,             /* port_xon */
            384,             /* lossless_xoff */
            288,             /* lossless_xon */
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 4 lanes */
            1532,            /* discard_limit */
            1532,            /* lossless_discard */
            1532,            /* port_xoff */
            1532,            /* port_xon */
            1532,            /* lossless_xoff */
            1532,            /* lossless_xon */
            1463,            /* lossy_low_pri */
            1528,            /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSY Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 - invalid */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff : Default value */
            576,             /* port_xon : Default value*/
            384,             /* lossless_xoff : Default value*/
            288,             /* lossless_xon : Default value*/
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 1 lane */
            1148,            /* discard_limit */
            4604,            /* lossless_discard */
            4604,            /* port_xoff */
            4604,            /* port_xon */
            4604,            /* lossless_xoff */
            4604,            /* lossless_xon */
            949,             /* lossy_low_pri */
            1144,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            2300,            /* discard_limit */
            4604,            /* lossless_discard */
            4604,            /* port_xoff */
            4604,            /* port_xon */
            4604,            /* lossless_xoff */
            4604,            /* lossless_xon */
            2101,            /* lossy_low_pri */
            2296,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */
            0,               /* discard_limit */
            0,               /* lossless_discard */
            720,             /* port_xoff */
            576,             /* port_xon */
            384,             /* lossless_xoff */
            288,             /* lossless_xon */
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 4 lanes */
            4604,            /* discard_limit */
            4604,            /* lossless_discard */
            4604,            /* port_xoff */
            4604,            /* port_xon */
            4604,            /* lossless_xoff */
            4604,            /* lossless_xon */
            4405,            /* lossy_low_pri */
            4600,            /* lossy_discard */
            1                /* sop_discard_mode */
        }
    }
};

static const soc_reg_t soc_helix5_obm_ca_ctrl_regs[HELIX5_PIPES_PER_DEV][
    HELIX5_PBLKS_PER_PIPE] = {
    {IDB_OBM0_Q_CA_CONTROLr, IDB_OBM1_Q_CA_CONTROLr,
     IDB_OBM2_Q_CA_CONTROLr, IDB_OBM0_CA_CONTROLr,
     IDB_OBM1_CA_CONTROLr, IDB_OBM2_CA_CONTROLr,
     IDB_OBM3_CA_CONTROLr, IDB_OBM0_48_CA_CONTROLr,
     IDB_OBM1_48_CA_CONTROLr, IDB_OBM2_48_CA_CONTROLr
     }
    
};

static const soc_reg_t obm_ctrl_regs[HELIX5_PIPES_PER_DEV][
	HELIX5_PBLKS_PER_PIPE] = {
    {IDB_OBM0_Q_CONTROLr, IDB_OBM1_Q_CONTROLr, IDB_OBM2_Q_CONTROLr,
     IDB_OBM0_CONTROLr,
     IDB_OBM1_CONTROLr, IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
     IDB_OBM0_48_CONTROLr,
     IDB_OBM1_48_CONTROLr, IDB_OBM2_48_CONTROLr}
     
     };

/*! @fn int soc_helix5_get_pipe_from_phy_pnum(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate pipe number from physical port number.
 *  @returns pipe_number
 */
int
soc_helix5_get_pipe_from_phy_pnum(int pnum)
{
    return(0);
}

/*! @fn int soc_helix5_get_q_reg(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate reg version for the OBM Q regs.
 *  @returns reg_ver
 */
int
soc_helix5_get_q_reg(int pnum)
{
 int reg_ver;
   if(pnum < 49)
   {
   reg_ver = (pnum-1)%16;	
   reg_ver = (reg_ver)/4;	
    return(reg_ver);
   }
   else
    return(0);
}



/*! @fn int soc_helix5_get_pm_from_phy_pnum(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate PM number from physical port number.
 *  @returns PM number
 */
int
soc_helix5_get_pm_from_phy_pnum(int pnum)
{
	  /* Return 0 - 15 for straight order for both pipes. */
   /* return (((pnum-1)&0x3f)>>2);*/
  pnum = pnum -1 ;
    if (pnum <= 48)
        return((pnum/16));
    else if(pnum <= HELIX5_TDM_PHY_PORTS_PER_PIPE) { 
        return((pnum/4)-9);
        }
    else	
        return(HX5_NUM_EXT_PORTS);
   
}

/*! @fn int soc_helix5_get_subp_from_phy_pnum(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate subport number from physical port number.
 *  @returns subport number
 */
int
soc_helix5_get_subp_from_phy_pnum(int pnum)
{
    return ((pnum-1)&0x3);
}

/*! @fn int soc_helix5_idb_clear_stats_new_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to clear statistics of the new ports that are comming
 *         up during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_clear_stats_new_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int prt;
    soc_reg_t reg;
    uint64 rval64;
    int pnum;
    int pipe_num;
    int pm_num;
    int subp;

    const soc_reg_t obm_usage_regs[HELIX5_PIPES_PER_DEV][
	HELIX5_PBLKS_PER_PIPE] = {
        {IDB_OBM0_Q_MAX_USAGEr,
         IDB_OBM1_Q_MAX_USAGEr,
         IDB_OBM2_Q_MAX_USAGEr,
         IDB_OBM0_MAX_USAGEr,
         IDB_OBM1_MAX_USAGEr,
         IDB_OBM2_MAX_USAGEr,
         IDB_OBM3_MAX_USAGEr,
         IDB_OBM0_48_MAX_USAGEr,
         IDB_OBM1_48_MAX_USAGEr,
         IDB_OBM2_48_MAX_USAGEr
         }
        };

    for (prt = 0; prt < port_schedule_state->nport; prt++) {
        if (port_schedule_state->resource[prt].physical_port != -1) {
            pnum =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[
                        prt].logical_port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(pnum);
            pm_num = soc_helix5_get_pm_from_phy_pnum(pnum);
            subp = soc_helix5_get_subp_from_phy_pnum(pnum);

            reg = obm_usage_regs[pipe_num][pm_num];
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                                    REG_PORT_ANY, 0,
                                                    &rval64));
			switch(subp) {
			case 0 : 
				soc_reg64_field32_set(unit, reg, &rval64, PORT0_MAX_USAGEf, 0);
				break;
			case 1 : 
				soc_reg64_field32_set(unit, reg, &rval64, PORT1_MAX_USAGEf, 0);
				break;
			case 2 : 
				soc_reg64_field32_set(unit, reg, &rval64, PORT2_MAX_USAGEf, 0);
				break;
			case 3 : 
				soc_reg64_field32_set(unit, reg, &rval64, PORT3_MAX_USAGEf, 0);
				break;
			default : 
				soc_reg64_field32_set(unit, reg, &rval64, PORT0_MAX_USAGEf, 0);
				break;
			}
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                                                    REG_PORT_ANY, 0,
                                                    rval64));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_reset_buffer(int unit, int pipe_num, int pm_num,
 *              int subp, int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number within the PM: Ranges from 0 to 3.
 *  @param pnum physical port number.
 *  @param reset_buffer IDB OBM reset buffer: 1 - Apply reset. 0 - Release reset.
 *  @brief Helper function to apply / or release IDB OBM buffer reset.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_obm_reset_buffer(int unit, int pipe_num, int pm_num, int subp,int pnum,
                                   int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    uint32 offset;

    reg = obm_ctrl_regs[pipe_num][pm_num];
    offset = soc_helix5_get_q_reg(pnum);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, offset, &rval));
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                             "IDB port Up rval %1d pipe_num %1d pm_num %1d sbup=%1d "
                             "reset_buffer=%1d offset=%1d  \n"),
                     rval,
                      pipe_num, pm_num, subp,reset_buffer,offset));
    if (subp == 0) {
        soc_reg_field_set(unit, reg, &rval, PORT0_RESETf, reset_buffer);
    } else if (subp == 1) {
        soc_reg_field_set(unit, reg, &rval, PORT1_RESETf, reset_buffer);
    } else if (subp == 2) {
        soc_reg_field_set(unit, reg, &rval, PORT2_RESETf, reset_buffer);
    } else {
        soc_reg_field_set(unit, reg, &rval, PORT3_RESETf, reset_buffer);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, offset, rval));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, offset, &rval));
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                             "IDB port Up rval_update %1d pipe_num %1d pm_num %1d sbup=%1d "
                             "reset_buffer=%1d offset=%1d \n"),
                     rval,
                     pipe_num, pm_num, subp,reset_buffer,offset));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_ca_reset_buffer(int unit, int pipe_num, int pm_num,
 *              int subp, int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number within the PM: Ranges from 0 to 3.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_ca_reset_buffer(int unit, int pipe_num, int pm_num, int subp,
                                  int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;

    reg = soc_helix5_obm_ca_ctrl_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 
									0, &rval));
    if (subp == 0) {
        soc_reg_field_set(unit, reg, &rval, PORT0_RESETf, reset_buffer);
    } else if (subp == 1) {
        soc_reg_field_set(unit, reg, &rval, PORT1_RESETf, reset_buffer);
    } else if (subp == 2) {
        soc_reg_field_set(unit, reg, &rval, PORT2_RESETf, reset_buffer);
    } else if (subp == 3) {
        soc_reg_field_set(unit, reg, &rval, PORT3_RESETf, reset_buffer);
    } else if (subp == 4) {
        soc_reg_field_set(unit, reg, &rval, PORT4_RESETf, reset_buffer);
    } else if (subp == 5) {
        soc_reg_field_set(unit, reg, &rval, PORT5_RESETf, reset_buffer);
    } else if (subp == 6) {
        soc_reg_field_set(unit, reg, &rval, PORT6_RESETf, reset_buffer);
    } else if (subp == 7) {
        soc_reg_field_set(unit, reg, &rval, PORT7_RESETf, reset_buffer);
    } else if (subp == 8) {
        soc_reg_field_set(unit, reg, &rval, PORT8_RESETf, reset_buffer);
    } else if (subp == 9) {
        soc_reg_field_set(unit, reg, &rval, PORT9_RESETf, reset_buffer);
    } else if (subp == 10) {
        soc_reg_field_set(unit, reg, &rval, PORT10_RESETf, reset_buffer);
    } else if (subp == 11) {
        soc_reg_field_set(unit, reg, &rval, PORT11_RESETf, reset_buffer);
    } else if (subp == 12) {
        soc_reg_field_set(unit, reg, &rval, PORT12_RESETf, reset_buffer);
    } else if (subp == 13) {
        soc_reg_field_set(unit, reg, &rval, PORT13_RESETf, reset_buffer);
    } else if (subp == 14) {
        soc_reg_field_set(unit, reg, &rval, PORT14_RESETf, reset_buffer);
    } else if (subp == 15) {
        soc_reg_field_set(unit, reg, &rval, PORT15_RESETf, reset_buffer);
    } else {
        soc_reg_field_set(unit, reg, &rval, PORT0_RESETf, reset_buffer);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 
									0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_lpbk_ca_reset_buffer(int unit, int pipe_num, 
                      int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset
 *                  for loopback port.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_lpbk_ca_reset_buffer(int unit, int pipe_num, int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    const soc_reg_t idb_ca_lpbk_ctrl[HELIX5_PIPES_PER_DEV]= {
         IDB_CA_LPBK_CONTROL_PIPE0r
         };


    reg = idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 
														0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_RESETf, reset_buffer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 
														0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_cpu_ca_reset_buffer(int unit, int pipe_num, 
                      int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num helix5 pipe number.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset
 *                  for CPU port.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_cpu_ca_reset_buffer(int unit, int pipe_num, int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    const soc_reg_t idb_ca_lpbk_ctrl[HELIX5_PIPES_PER_DEV]= {
         IDB_CA_CPU_CONTROL_PIPE0r
         };


    reg = idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 
															0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_RESETf, reset_buffer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 
															0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_helix5_idb_lpbk_ca_in_reset(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @brief Helper function to check if the IDB loopback CA is in reset.
 *  @returns Reset status.
 */
int
soc_helix5_idb_lpbk_ca_in_reset(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int buffer_in_reset;
    const soc_reg_t idb_ca_lpbk_ctrl[HELIX5_PIPES_PER_DEV]= {
         IDB_CA_LPBK_CONTROL_PIPE0r
         };

    reg = idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    buffer_in_reset = soc_reg_field_get(unit, reg, rval, PORT_RESETf);
    return buffer_in_reset;
}


/*! @fn int soc_helix5_idb_cpu_ca_in_reset(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @brief Helper function to check if the IDB CPU CA is in reset.
 *  @returns Reset status;
 */
int
soc_helix5_idb_cpu_ca_in_reset(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int buffer_in_reset;
    static const soc_reg_t idb_ca_cpu_ctrl[HELIX5_PIPES_PER_DEV]= {
         IDB_CA_CPU_CONTROL_PIPE0r
         };
    reg = idb_ca_cpu_ctrl[0];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    buffer_in_reset = soc_reg_field_get(unit, reg, rval, PORT_RESETf);
    return buffer_in_reset;
}


/*! @fn int soc_helix5_idb_port_mode_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to program IDB port mode for the new ports that are
 *         comming up during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_port_mode_set(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    int i;
    uint32 rval;
    int pnum;
    int pipe_num;
    int pm_num;
    int mode;
    int qmode;
    int inst;
    uint64 rval64;
    static const int soc_helix5_hw_mode_values[SOC_HX5_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };


    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            pnum =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            pm_num = soc_helix5_get_pm_from_phy_pnum(pnum);
            soc_hx5_tdm_get_port_ratio(unit, port_schedule_state, pm_num,
                                         &mode,
                                         0);
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(pnum);
	   
	    if(pnum < 49){
	    inst = ((pnum - 1) / 16 | SOC_REG_ADDR_INSTANCE_MASK);
	    SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                                                    0, &rval64));
            qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                                                QMODEf);
	   }else
	   {
	    qmode = 0;
	   
	   }
    	    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_helix5_idb_port_mode_set: mode: %1d, qmode: %1d,"
 			"pnum: %1d, pm_num: %1d \n"),
              mode, qmode, pnum, pm_num));

            reg = soc_helix5_obm_ca_ctrl_regs[pipe_num][pm_num];
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      0, &rval));
            if((pnum < 49) && (qmode))
            soc_reg_field_set(unit, reg, &rval, PORT_MODEf,
                              8);
            else
            soc_reg_field_set(unit, reg, &rval, PORT_MODEf,
                              soc_helix5_hw_mode_values[mode]);

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                      0, rval));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_wr_obm_shared_config(int unit, int pipe_num,
 *              int pm_num, int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port number.
 *  @param num_lanes Number of lanes in the port
 *  @param lossless Chip mode - 1: Lossless; 0: Lossy.
 *  @brief Helper function to config OBM shared buffer during flexing
 *  portmodes(its always quad) are going to be same for pmq and there 
 *  is no buffer allocation required in pmq mode
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_wr_obm_shared_config(int unit, int pipe_num, int pm_num,
                                       int subp,
                                       int num_lanes, int lossless)
{
    soc_reg_t reg;
    int sop_discard_mode;
    uint32 rval;

    const soc_reg_t obm_shared_cfg[HELIX5_PIPES_PER_DEV][
		HELIX5_PBLKS_PER_PIPE] = {
        {IDB_OBM0_Q_SHARED_CONFIGr , IDB_OBM1_Q_SHARED_CONFIGr ,
         IDB_OBM2_Q_SHARED_CONFIGr , IDB_OBM0_SHARED_CONFIGr,
         IDB_OBM1_SHARED_CONFIGr, IDB_OBM2_SHARED_CONFIGr,
	 IDB_OBM3_SHARED_CONFIGr,
         IDB_OBM0_48_SHARED_CONFIGr, IDB_OBM1_48_SHARED_CONFIGr,
         IDB_OBM2_48_SHARED_CONFIGr}
        
    };

    reg = obm_shared_cfg[pipe_num][pm_num];
    if (lossless) {
      if(pm_num < 7){
        sop_discard_mode =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_EG_LOSSLESS_INDEX]
            [num_lanes].sop_discard_mode;
	}
      else{
        sop_discard_mode =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_FL_LOSSLESS_INDEX]
            [num_lanes].sop_discard_mode;
	}
    } else {
      if(pm_num < 7){
        sop_discard_mode =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_EG_LOSSY_INDEX]
            [num_lanes].sop_discard_mode;
	}
      else{
        sop_discard_mode =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_FL_LOSSY_INDEX]
            [num_lanes].sop_discard_mode;
	}
    }

	rval = 0;
    soc_reg_field_set(unit, reg, &rval, SOP_DISCARD_MODEf, sop_discard_mode);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 
										0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_wr_obm_flow_ctrl_cfg(int unit, int pipe_num,
 *              int pm_num, int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port number.
 *  @brief Helper function to config OBM shared buffer during flexing
 *  @returns SOC_E_NONE
 *  There is no flow control to be supported for Q mode,
 *  hence not extending Q_FC regs
 */
int
soc_helix5_idb_wr_obm_flow_ctrl_cfg(int unit, int pipe_num, int pm_num,
                                       int subp, int lossless)
{
    soc_reg_t reg;
    uint64 rval64;

    const soc_reg_t obm_flow_ctrl_cfg[HELIX5_PIPES_PER_DEV][
			HELIX5_PBLKS_PER_PIPE] = {
        {IDB_OBM0_Q_FLOW_CONTROL_CONFIGr,
         IDB_OBM1_Q_FLOW_CONTROL_CONFIGr,
         IDB_OBM2_Q_FLOW_CONTROL_CONFIGr,
         IDB_OBM0_FLOW_CONTROL_CONFIGr,
         IDB_OBM1_FLOW_CONTROL_CONFIGr,
         IDB_OBM2_FLOW_CONTROL_CONFIGr,
         IDB_OBM3_FLOW_CONTROL_CONFIGr,
         IDB_OBM0_48_FLOW_CONTROL_CONFIGr,
         IDB_OBM1_48_FLOW_CONTROL_CONFIGr,
         IDB_OBM2_48_FLOW_CONTROL_CONFIGr,
         }
        
        };

    reg = obm_flow_ctrl_cfg[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, subp,
                                            &rval64));
    if (lossless) {
        soc_reg64_field32_set(unit, reg, &rval64, XOFF_REFRESH_TIMERf, 256);
        soc_reg64_field32_set(unit, reg, &rval64, FC_TYPEf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, 1);
        soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, 1);
        soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, 
									LOSSLESS0_PRIORITY_PROFILEf, 0xffff);
        soc_reg64_field32_set(unit, reg, &rval64, 
									LOSSLESS1_PRIORITY_PROFILEf, 0);
    } else {
        soc_reg64_field32_set(unit, reg, &rval64, XOFF_REFRESH_TIMERf, 256);
        soc_reg64_field32_set(unit, reg, &rval64, FC_TYPEf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_PRIORITY_PROFILEf, 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY, subp,
                                            rval64));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_wr_obm_ovs_en(int unit, int pipe_num, int pm_num,
 *              int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port numbber.
 *  @param pnum physical port number.
 *  @brief Helper function to config OBM CONTROL register during flexing
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_wr_obm_ovs_en(int unit, int pipe_num, int pm_num, int subp , int pnum)
{
    soc_reg_t reg;
    uint32 rval;
    uint32 offset;

    reg = obm_ctrl_regs[pipe_num][pm_num];
    offset = soc_helix5_get_q_reg(pnum);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                                    reg, REG_PORT_ANY, offset, &rval));
    if (subp == 0) {
        soc_reg_field_set(unit, reg, &rval, PORT0_OVERSUB_ENABLEf, 1);
    } else if (subp == 1) {
        soc_reg_field_set(unit, reg, &rval, PORT1_OVERSUB_ENABLEf, 1);
    } else if (subp == 2) {
        soc_reg_field_set(unit, reg, &rval, PORT2_OVERSUB_ENABLEf, 1);
    } else {
        soc_reg_field_set(unit, reg, &rval, PORT3_OVERSUB_ENABLEf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 
								offset, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_wr_obm_buffer_limit(int unit, int pipe_num, int pm_num,
 *              int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port numbber.
 *  @param num_lanes Number of serdes lanes used in the port.
 *  @brief Helper function to config OBM buffer start & end during flexing
 *  portmodes(its always quad) are going to be same for pmq and there 
 *  is no buffer allocation required in pmq mode
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_wr_obm_buffer_limit(int unit, int pipe_num, int pm_num, int subp,
                                int num_lanes)
{
    soc_reg_t reg;
    uint64 rval64;
	uint32 buffer_start, buffer_end;

    const soc_reg_t obm_buffer_config[HELIX5_PIPES_PER_DEV][
		HELIX5_PBLKS_PER_PIPE] = {
        {IDB_OBM0_Q_BUFFER_CONFIGr,
		 IDB_OBM1_Q_BUFFER_CONFIGr,
		 IDB_OBM2_Q_BUFFER_CONFIGr,
		 IDB_OBM0_BUFFER_CONFIGr,
		 IDB_OBM1_BUFFER_CONFIGr,
		 IDB_OBM2_BUFFER_CONFIGr,
		 IDB_OBM3_BUFFER_CONFIGr,
		 IDB_OBM0_48_BUFFER_CONFIGr,
		 IDB_OBM1_48_BUFFER_CONFIGr,
		 IDB_OBM2_48_BUFFER_CONFIGr
         } 
	};

        if(pm_num < 7)
	buffer_start 	= subp * HELIX5_OBM_BUFFER_PER_LANE_EG;
	else
	buffer_start 	= subp * HELIX5_OBM_BUFFER_PER_LANE_FL;


	if(num_lanes == 0) 	buffer_end = buffer_start;
	else{
        if(pm_num < 7){
	buffer_end = buffer_start + (num_lanes * HELIX5_OBM_BUFFER_PER_LANE_EG) - 1;}
	else{
	buffer_end = buffer_start + (num_lanes * HELIX5_OBM_BUFFER_PER_LANE_FL) - 1;}
	}
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_helix5_idb_wr_obm_buffer_limit: pipe: %1d, pm_num: %1d,"
 			"subp: %1d, num_lanes: %1d, buffer_start: %1d, buffer_end: %1d \n"),
              pipe_num, pm_num, subp, num_lanes, buffer_start, buffer_end));

    reg = obm_buffer_config[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                            subp, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, BUFFER_STARTf,
                        					buffer_start);
    soc_reg64_field32_set(unit, reg, &rval64, BUFFER_ENDf,
                        					buffer_end);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                            subp, rval64));
	
    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_wr_obm_thresh(int unit, int pipe_num, int pm_num,
 *              int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port numbber.
 *  @param num_lanes Number of serdes lanes used in the port.
 *  @param lossless Chip mode - 1: Lossless; 0: Lossy.
 *  @brief Helper function to config OBM thresholds during flexing
 *  portmodes(its always quad) are going to be same for pmq and there 
 *  is no buffer allocation required in pmq mode
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_wr_obm_thresh(int unit, int pipe_num, int pm_num, int subp,
                                int num_lanes, int lossless)
{
    soc_reg_t reg;
    int i;
    uint32 lossless0_discard, lossy_discard, lossy_low_pri,
           discard_limit, lossless1_discard;
    uint64 rval64;
    uint32 HELIX5_OBM_PARAMS_LOSSLESS;
    uint32 HELIX5_OBM_PARAMS_LOSSY;

    const soc_reg_t obm_thresh_pp[HELIX5_PIPES_PER_DEV][
		HELIX5_PBLKS_PER_PIPE][2] = {
        {{IDB_OBM0_Q_THRESHOLDr,IDB_OBM0_Q_THRESHOLD_1r},
         {IDB_OBM1_Q_THRESHOLDr,IDB_OBM1_Q_THRESHOLD_1r},
         {IDB_OBM2_Q_THRESHOLDr,IDB_OBM2_Q_THRESHOLD_1r},
         {IDB_OBM0_THRESHOLDr,IDB_OBM0_THRESHOLD_1r},
         {IDB_OBM1_THRESHOLDr,IDB_OBM1_THRESHOLD_1r},
         {IDB_OBM2_THRESHOLDr,IDB_OBM2_THRESHOLD_1r},
         {IDB_OBM3_THRESHOLDr,IDB_OBM3_THRESHOLD_1r},
         {IDB_OBM0_48_THRESHOLDr,IDB_OBM0_48_THRESHOLD_1r},
         {IDB_OBM1_48_THRESHOLDr,IDB_OBM1_48_THRESHOLD_1r},
         {IDB_OBM2_48_THRESHOLDr,IDB_OBM2_48_THRESHOLD_1r},
         }
            };
    if(pm_num < 7)
    {
     HELIX5_OBM_PARAMS_LOSSLESS = HELIX5_OBM_PARAMS_EG_LOSSLESS_INDEX;
     HELIX5_OBM_PARAMS_LOSSY    = HELIX5_OBM_PARAMS_EG_LOSSY_INDEX;
    }
    else
    {
     HELIX5_OBM_PARAMS_LOSSLESS =  HELIX5_OBM_PARAMS_FL_LOSSLESS_INDEX;
     HELIX5_OBM_PARAMS_LOSSY    =  HELIX5_OBM_PARAMS_FL_LOSSY_INDEX;
    }


    if (lossless) {
        discard_limit =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].discard_limit;
        lossless1_discard =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossless_discard;
        lossless0_discard =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossless_discard;
        lossy_discard =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossy_discard;
        lossy_low_pri =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossy_low_pri;
    } else {
        discard_limit =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].discard_limit;
        lossless1_discard =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossless_discard;
        lossless0_discard =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossless_discard;
        lossy_discard =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossy_discard;
        lossy_low_pri =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossy_low_pri;
    }
    for (i = 0; i < 2; i++) {
        reg = obm_thresh_pp[pipe_num][pm_num][i];
        if (i == 0) {
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                                    subp, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                lossless0_discard);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf,
                                lossy_discard);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf,
                                lossy_low_pri);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
        else {
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                                    subp, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf,
                                discard_limit);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                lossless1_discard);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_wr_obm_fc_threshold(int unit, int pipe_num, int pm_num, *              int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port number.
 *  @param num_lanes Number of serdeslanes used in the port.
 *  @param lossless Chip mode - 1: Lossless; 0: Lossy.
 *  @brief Helper function to config OBM flow-control thresholds during flexing
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_wr_obm_fc_threshold(int unit, int pipe_num, int pm_num,
                                      int subp,
                                      int num_lanes, int lossless)
{
    soc_reg_t reg;
    int i;
    uint32 lossless0_xon, lossless0_xoff, lossless1_xon, lossless1_xoff,
           port_xon, port_xoff;
    uint64 rval64;
    uint32 HELIX5_OBM_PARAMS_LOSSLESS;
    uint32 HELIX5_OBM_PARAMS_LOSSY;
    const soc_reg_t obm_fc_thresh_pp[HELIX5_PIPES_PER_DEV][
		HELIX5_PBLKS_PER_PIPE][2] = {
        {{IDB_OBM0_Q_FC_THRESHOLDr,
          IDB_OBM0_Q_FC_THRESHOLD_1r},
         {IDB_OBM1_Q_FC_THRESHOLDr, IDB_OBM1_Q_FC_THRESHOLD_1r},
         {IDB_OBM2_Q_FC_THRESHOLDr,
          IDB_OBM2_Q_FC_THRESHOLD_1r},
         {IDB_OBM0_FC_THRESHOLDr, IDB_OBM0_FC_THRESHOLD_1r},
         {IDB_OBM1_FC_THRESHOLDr,
          IDB_OBM1_FC_THRESHOLD_1r},
         {IDB_OBM2_FC_THRESHOLDr, IDB_OBM2_FC_THRESHOLD_1r},
         {IDB_OBM3_FC_THRESHOLDr,
          IDB_OBM3_FC_THRESHOLD_1r},
         {IDB_OBM0_48_FC_THRESHOLDr,
	  IDB_OBM0_48_FC_THRESHOLD_1r},
         {IDB_OBM1_48_FC_THRESHOLDr,
          IDB_OBM1_48_FC_THRESHOLD_1r},
         {IDB_OBM2_48_FC_THRESHOLDr, 
	  IDB_OBM2_48_FC_THRESHOLD_1r}
         }

        
    };

    if(pm_num < 7)
    {
     HELIX5_OBM_PARAMS_LOSSLESS = HELIX5_OBM_PARAMS_EG_LOSSLESS_INDEX;
     HELIX5_OBM_PARAMS_LOSSY    = HELIX5_OBM_PARAMS_EG_LOSSY_INDEX;
    }
    else
    {
     HELIX5_OBM_PARAMS_LOSSLESS =  HELIX5_OBM_PARAMS_FL_LOSSLESS_INDEX;
     HELIX5_OBM_PARAMS_LOSSY    =  HELIX5_OBM_PARAMS_FL_LOSSY_INDEX;
    }

    if (lossless) {
        lossless0_xon =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossless_xon;
        lossless1_xon =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossless_xon;
        lossless0_xoff =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossless_xoff;
        lossless1_xoff =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].lossless_xoff;
        port_xon =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].port_xon;
        port_xoff =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSLESS]
            [num_lanes].port_xoff;
    } else {
        lossless0_xon =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossless_xon;
        lossless1_xon =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossless_xon;
        lossless0_xoff =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossless_xoff;
        lossless1_xoff =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].lossless_xoff;
        port_xon =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].port_xon;
        port_xoff =
            soc_helix5_obm_settings[HELIX5_OBM_PARAMS_LOSSY]
            [num_lanes].port_xoff;
    }

	COMPILER_64_ZERO(rval64);
    for (i = 0; i < 2; i++) {
        reg = obm_fc_thresh_pp[pipe_num][pm_num][i];
        if (i == 0) {
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                lossless0_xon);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                lossless0_xoff);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                lossless1_xon);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                lossless1_xoff);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
        else {
            soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, port_xon);
            soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, port_xoff);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_obm_poll_buffer_empty(int unit, int pipe_num,
 *              int pm_num, int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number.
 *  @param pnum physical port number.
 *  @brief Helper function to poll for IDB OBM buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_obm_poll_buffer_empty(int unit, int pipe_num, int pm_num,
                                        int subp)
{
    soc_reg_t reg;
    uint64 rval64;
    int	cnt = 0;

    const soc_reg_t obm_usage_regs[HELIX5_PIPES_PER_DEV][
		HELIX5_PBLKS_PER_PIPE] = { 
        {IDB_OBM0_Q_USAGEr, IDB_OBM1_Q_USAGEr, IDB_OBM2_Q_USAGEr,
         IDB_OBM0_USAGEr,
         IDB_OBM1_USAGEr, IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
         IDB_OBM0_48_USAGEr,
         IDB_OBM1_48_USAGEr,
         IDB_OBM2_48_USAGEr
         }
        
    };

    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }

    reg = obm_usage_regs[pipe_num][pm_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, subp,
                                                &rval64));

        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
		cnt++;
		if(cnt >= 20000) {
			LOG_CLI((BSL_META_U(unit, "OBM is not going to empty even after"
								"20000 attempts")));
			return SOC_E_FAIL;
		}
    } while (soc_reg64_field32_get(unit, reg, rval64, TOTAL_COUNTf)>0);

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_ca_poll_buffer_empty(int unit, int pipe_num,
 *              int pm_num, int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Helix5 pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number.
 *  @brief Helper function to poll for IDB Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_ca_poll_buffer_empty(int unit, int pipe_num, int pm_num,
                                       int subp,int pnum)
{
    soc_reg_t reg;
    uint64 rval64;
    uint32 ca_fifo_empty;
    uint32 offset;
	int	cnt = 0;

    static const soc_reg_t ca_status_regs[HELIX5_PIPES_PER_DEV][
        HELIX5_PBLKS_PER_PIPE] = {
        {IDB_OBM0_Q_CA_HW_STATUSr, IDB_OBM1_Q_CA_HW_STATUSr,
         IDB_OBM2_Q_CA_HW_STATUSr, IDB_OBM0_CA_HW_STATUSr,
         IDB_OBM1_CA_HW_STATUSr, IDB_OBM2_CA_HW_STATUSr,
         IDB_OBM3_CA_HW_STATUSr, IDB_OBM0_48_CA_HW_STATUSr,
         IDB_OBM1_48_CA_HW_STATUSr, IDB_OBM2_48_CA_HW_STATUSr
         }
        
    };

    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }

    reg = ca_status_regs[pipe_num][pm_num];
    offset = soc_helix5_get_q_reg(pnum);
    do {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, offset,
                                                &rval64));
        switch (subp) {
        case 0:
            ca_fifo_empty = soc_reg64_field32_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT0f);
            break;
        case 1:
            ca_fifo_empty = soc_reg64_field32_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT1f);
            break;
        case 2:
            ca_fifo_empty = soc_reg64_field32_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT2f);
            break;
        case 3:
            ca_fifo_empty = soc_reg64_field32_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT3f);
            break;
        default:
            ca_fifo_empty = 1;
            break;
        }

        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
		cnt++;
		if(cnt >= 20000) {
			LOG_CLI((BSL_META_U(unit, "CA is not going to empty even after"
								"20000 attempts")));
			return SOC_E_FAIL;
		}
    } while (ca_fifo_empty==0);

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num helix5 pipe number.
 *  @brief Helper function to poll for IDB CPU Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 ca_fifo_empty;
    const soc_reg_t idb_ca_cpu_hw_status[HELIX5_PIPES_PER_DEV]= {
         IDB_CA_CPU_HW_STATUS_PIPE0r
         };
    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }
    reg = idb_ca_cpu_hw_status[pipe_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                   reg, REG_PORT_ANY, 0, &rval32));
        ca_fifo_empty = soc_reg_field_get(unit, reg, rval32,
                                                FIFO_EMPTYf);
    } while (!ca_fifo_empty);
    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num helix5 pipe number.
 *  @brief Helper function to poll for IDB LOOPBACK port
           Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 ca_fifo_empty;
    const soc_reg_t idb_ca_lpbk_hw_status[HELIX5_PIPES_PER_DEV]= {
         IDB_CA_LPBK_HW_STATUS_PIPE0r
         };
    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }
    reg = idb_ca_lpbk_hw_status[pipe_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                   reg, REG_PORT_ANY, 0, &rval32));
        ca_fifo_empty = soc_reg_field_get(unit, reg, rval32,
                                                FIFO_EMPTYf);
    } while (!ca_fifo_empty);
    return SOC_E_NONE;
}


/*! @fn int soc_helix5_speed_to_bmop_class_map(int speed)
 *  @param speed Port speed.
 *  @brief Helper function to get bubble MOP class as below::
 *         The function look at the port speed & returns the encoding as below::
 *         0 - SAF,
 *         1 - 10GE,
 *         2 - HG[11],
 *         3 - 20GE,
 *         4 - 21[HG},
 *         5 - 25GE,
 *         6 - HG[27],
 *         7 - 40GE,
 *         8 - HG[42],
 *         9 - 50GE,
 *         10 - HG[53],
 *         11 - 100GE,
 *         12 - HG[106],
 *  @returns int
 */
int
soc_helix5_speed_to_bmop_class_map(int speed)
{
    int bmop_class;
    int i;

    bmop_class=0;
    for (i = 0; i <= 12; i++) {
        if (soc_helix5_bmop_cfg_tbl[i].speed == speed) {
            bmop_class = i;
            break;
        }
    }

    return bmop_class;
}

/*! @fn int soc_helix5_idb_obm_bubble_mop_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to re-configure BUBBLE MOP settings during flexport
 *         operation. The function looks at the port speed & the latency modes
 *         to determine what the bubble MOP settings should be.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_obm_bubble_mop_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    int pipe_num;
    int pm_num;
    int subp;
    int bubble_mop_disable;
    int phy_port;
    int port;
    int oversub;
    int latency;
    int i;
    int speed_encoding;

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
            oversub = port_schedule_state->resource[i].oversub;
            latency = port_schedule_state->cutthru_prop.switch_bypass_mode;
            speed_encoding =
                (port_schedule_state->cutthru_prop.asf_modes[port]==
                 _SOC_ASF_MODE_SAF) ? 0 :
                soc_helix5_speed_to_bmop_class_map(
                    port_schedule_state->resource[i].speed);
            bubble_mop_disable =
                oversub ? !(soc_helix5_bmop_cfg_tbl[speed_encoding].
                            bmop_enable.
                            oversub[latency]) :
                !(soc_helix5_bmop_cfg_tbl[speed_encoding].
                  bmop_enable.line_rate[latency]);
            reg = obm_ctrl_regs[pipe_num][pm_num];
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      0, &rval32));
            if (subp==0) {
                soc_reg_field_set(unit, reg, &rval32, PORT0_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            } else if (subp ==1) {
                soc_reg_field_set(unit, reg, &rval32, PORT1_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            } else if (subp ==2) {
                soc_reg_field_set(unit, reg, &rval32, PORT2_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            } else {
                soc_reg_field_set(unit, reg, &rval32, PORT3_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                      0, rval32));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_obm_port_config_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to re-configure IDB OBM port config register default
 *         priority if lossless class.
 *         flow control is not supported
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_obm_port_config_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int port;
    int i;
    static const soc_reg_t obm_port_config_regs[HELIX5_PIPES_PER_DEV][
        HELIX5_PBLKS_PER_PIPE] = {
        {IDB_OBM0_Q_PORT_CONFIGr, IDB_OBM1_Q_PORT_CONFIGr,
         IDB_OBM2_Q_PORT_CONFIGr, IDB_OBM0_PORT_CONFIGr,
         IDB_OBM1_PORT_CONFIGr, IDB_OBM2_PORT_CONFIGr,
         IDB_OBM3_PORT_CONFIGr, IDB_OBM0_48_PORT_CONFIGr,
         IDB_OBM1_48_PORT_CONFIGr, IDB_OBM2_48_PORT_CONFIGr}
    };

    if (port_schedule_state->lossless) {
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (port_schedule_state->resource[i].physical_port != -1) {
                port = port_schedule_state->resource[i].logical_port;
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[port];
                pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
                pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
                subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
                rval32=0;
                reg = obm_port_config_regs[pipe_num][pm_num];
                soc_reg_field_set(unit, reg, &rval32, PORT_PRIf, 2);
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                                                          REG_PORT_ANY, subp,
                                                          rval32));
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_obm_dscp_map_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to re-configure IDB_OBM0_DSCP_MAP table as per the lossy/lossless configuration.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_idb_obm_dscp_map_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int port;
    int i;
    int j;
    int count;
    int lossless;
    uint32 priority;
    static const soc_mem_t obm_pri_map_mem[][HELIX5_PIPES_PER_DEV][
		HELIX5_PORTS_PER_PBLK] = {
        {{IDB_OBM0_Q_PRI_MAP_PORT0m, IDB_OBM0_Q_PRI_MAP_PORT1m,
          IDB_OBM0_Q_PRI_MAP_PORT2m, IDB_OBM0_Q_PRI_MAP_PORT3m}},
        {{IDB_OBM1_Q_PRI_MAP_PORT0m, IDB_OBM1_Q_PRI_MAP_PORT1m,
          IDB_OBM1_Q_PRI_MAP_PORT2m, IDB_OBM1_Q_PRI_MAP_PORT3m}},
        {{IDB_OBM2_Q_PRI_MAP_PORT0m, IDB_OBM2_Q_PRI_MAP_PORT1m,
          IDB_OBM2_Q_PRI_MAP_PORT2m, IDB_OBM2_Q_PRI_MAP_PORT3m}},
        {{IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
          IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m}},
        {{IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
          IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m}},
        {{IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
          IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m}},
        {{IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
          IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m}},
        {{IDB_OBM0_48_PRI_MAP_PORT0m, IDB_OBM0_48_PRI_MAP_PORT1m,
          IDB_OBM0_48_PRI_MAP_PORT2m, IDB_OBM0_48_PRI_MAP_PORT3m}},
        {{IDB_OBM1_48_PRI_MAP_PORT0m, IDB_OBM1_48_PRI_MAP_PORT1m,
          IDB_OBM1_48_PRI_MAP_PORT2m, IDB_OBM1_48_PRI_MAP_PORT3m}},
        {{IDB_OBM2_48_PRI_MAP_PORT0m, IDB_OBM2_48_PRI_MAP_PORT1m,
          IDB_OBM2_48_PRI_MAP_PORT2m, IDB_OBM2_48_PRI_MAP_PORT3m}}
       
    };
    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    lossless = port_schedule_state->lossless;
    priority = lossless ? HELIX5_OBM_PRIORITY_LOSSLESS0 : 
							HELIX5_OBM_PRIORITY_LOSSY_LO;
    count = COUNTOF(obm_ob_pri_fields);

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
            mem = obm_pri_map_mem[pm_num][pipe_num][subp];
			sal_memset(entry, 0, sizeof(entry));
            for (j = 0; j < count; j++) {
                soc_mem_field_set(unit, mem, entry, obm_ob_pri_fields[j], 
												&priority);
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_obm_force_saf_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to set the force_SAF setting for OBM.
 *  @returns SOC_E_NONE
 */
/* int
*soc_helix5_idb_obm_force_saf_set(
*    int unit, soc_port_schedule_state_t *port_schedule_state)
*{
*    soc_reg_t reg;
*    uint32 rval32;
*    int pipe_num;
*    int pm_num; 
*    int subp; 
*    int phy_port;
*    int port;
*    int i;
*    int unsatisfied_threshold; 
*    int send_enable; 
*    int receive_enable; 
*    int duration_timer; 
*    uint32 ovs_prt_up;
*  static const soc_reg_t obm_force_saf_config_regs[HELIX5_PIPES_PER_DEV][ 
*      HELIX5_PBLKS_PER_PIPE] = { 
*      {IDB_OBM0_Q_DBG_Ar, IDB_OBM1_Q_DBG_Ar, IDB_OBM2_Q_DBG_Ar, 
*       IDB_OBM0_DBG_Ar, 
*       IDB_OBM1_DBG_Ar, IDB_OBM2_DBG_Ar, IDB_OBM3_DBG_Ar, 
*       IDB_OBM0_48_DBG_Ar, 
*       IDB_OBM1_48_DBG_Ar, IDB_OBM2_48_DBG_Ar
*       } 
*  }; 
*   static const soc_reg_t idb_force_saf_config_regs[HELIX5_PIPES_PER_DEV] = 
*        {IDB_DBG_B_PIPE0r}; 
*
*    ovs_prt_up=0;
*    for (i = 0; i < port_schedule_state->nport; i++) {
*        if (port_schedule_state->resource[i].physical_port != -1) {
*            port = port_schedule_state->resource[i].logical_port;
*            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
*            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
*        pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port); 
*          subp = soc_helix5_get_subp_from_phy_pnum(phy_port);                 
*          if (port_schedule_state->resource[i].oversub) {                     
*              send_enable=1;                                                  
*              receive_enable=1;                                               
*              ovs_prt_up |= (1<<pipe_num);                                    
*          }else{                                                              
*              send_enable=0;                                                  
*              receive_enable=0;                                               
*          }                                                                   
*          unsatisfied_threshold =                                             
*              ((port_schedule_state->resource[i].speed == 1000)||             
*               (port_schedule_state->resource[i].speed == 10000)||            
*               (port_schedule_state->resource[i].speed == 11000)||            
*               (port_schedule_state->resource[i].speed == 20000)||            
*               (port_schedule_state->resource[i].speed == 21000)||            
*               (port_schedule_state->resource[i].speed == 40000)||            
*               (port_schedule_state->resource[i].speed == 42000)) ? 12 : 18;  
*          rval32=0;                                                         
*            reg = obm_force_saf_config_regs[pipe_num][pm_num]; 
*           soc_reg_field_set(unit, reg, &rval32, FIELD_Bf, send_enable);
*           soc_reg_field_set(unit, reg, &rval32, FIELD_Af, receive_enable);
*           soc_reg_field_set(unit, reg, &rval32, FIELD_Cf,
*                             unsatisfied_threshold);
*           SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
*                           subp, rval32)); 
*        }
*    }
*    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
*              "soc_helix5_idb_obm_force_saf_set ovs_prt_up:: %x \n"),
*              ovs_prt_up));
*    duration_timer=
*        soc_helix5_fsaf_dur_timer_tbl[HELIX5_FORCE_SAF_TIMER_ENTRY_CNT].
*        duration_timer;
*    for (i=0; i<HELIX5_FORCE_SAF_TIMER_ENTRY_CNT; i++) {
*        if (port_schedule_state->frequency ==
*            soc_helix5_fsaf_dur_timer_tbl[i].frequency) {
*            duration_timer=soc_helix5_fsaf_dur_timer_tbl[i].duration_timer;
*        }
*    }
*    for (i = 0; i < HELIX5_PIPES_PER_DEV; i++) {
*        pipe_num=i;
*        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
*                  "soc_helix5_idb_obm_force_saf_set ovs_prt_up:: %x "
*                  "pipe_num %d\n"), ovs_prt_up,pipe_num));
*     if (ovs_prt_up>>pipe_num) {                                           
*     reg = idb_force_saf_config_regs[pipe_num]; 
*     rval32=0;                                                          
*     LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,                       
*     "soc_helix5_idb_obm_force_saf_set Write to "             
*     "IDB_FORCE_SAF_CONFIG pipe_num %d\n"),                   
*     pipe_num));                                              
*     soc_reg_field_set(unit, reg, &rval32, FIELD_Af, duration_timer);   
*     SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 
*     0, rval32));             
*     }                                                                      
*    }
*
*    return SOC_E_NONE;
*}
*/

/*! @fn int soc_helix5_flex_dis_forwarding_traffic(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to Write EPC_LINK_BMAP table & wait. Follows the sequence
 *         specified in document "_____", Section _._._
 *  @returns SOC_E_NONE
 */
int
soc_helix5_flex_dis_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    uint32 mask;
    int port;
    int wait_us; 

    /* ING_DEST_PORT_ENABLEm */
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                                     entry));
    soc_mem_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                      memfld);
    mask = 0xffffffff;
    /* Make ING_DEST_PORT_ENABLE_BMAP=0 for all the ports involved in flex */
    for (i = 0; i < port_schedule_state->nport; i++) {
        port = port_schedule_state->resource[i].logical_port;
        memfld[(port>>5)] &= (mask^(1<<(port&0x1f)));
    }
    soc_mem_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                                      entry));

    /* EPC_LINK_BMAP read, field modify and write. */
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                     entry));
    soc_mem_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                      memfld);

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Disable EPC_LINK_BITMAP READ:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    mask = 0xffffffff;
    /* Make EPC_LINK_BMAP=0 for all the ports involved in flex */
    for (i = 0; i < port_schedule_state->nport; i++) {
        port = port_schedule_state->resource[i].logical_port;
        memfld[(port>>5)] &= (mask^(1<<(port&0x1f)));
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Disable EPC_LINK_BITMAP write:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));

    soc_mem_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                      entry));

    /* Wait is 8ms if 10/100 Kbps, else 80 us. Please refer Michael Lau's doc: 
     * "HX5_uA_flexport.pdf", Section 6.6.1
     */

/* We can use a common value of 8ms for port-down of any speed in HX5 or HR4.
 * Port down is not a frequent event and it may be simpler to just use single value
 * than have different sets based on speed combinations.
 * The value 8ms should work for HX5/HR4. 
 */ 
        wait_us = 8000;
          LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " epc delay being applied %0d \n"), wait_us));
    
/*   if ((port_schedule_state->in_port_map.log_port_speed[ */
/*       		HELIX5_LOG_PORT_MNG0] <= 100)) { */
/*       wait_us = 8000; */
/*   } else { */
/*       wait_us = 80; */
/*   } */

    sal_usleep(wait_us + (SAL_BOOT_QUICKTURN ? 1: 0) * EMULATION_FACTOR); 

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_flex_en_forwarding_traffic(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to Write EPC_LINK_BMAP table to enable forwarding traffic
 *         to port. Follows the sequence specified in document "HX5_uA_flexport.pdf",
 *         Section 6.6.2, bullet number 5.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_flex_en_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    int port;

    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                                     entry));
    soc_mem_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                      memfld);

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            memfld[(port>>5)] |= (0x1<<(port&0x1f));
        }
    }
    soc_mem_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                                      entry));

    /* EPC_LINK_BMAP read, field modify and write. */
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                     entry));
    soc_mem_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                      memfld);

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Enable EPC_LINK_BITMAP READ:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    /* Make EPC_LINK_BMAP=1 for all the ports going up */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            memfld[(port>>5)] |= (0x1<<(port&0x1f));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Enable EPC_LINK_BITMAP write:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));

    soc_mem_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                      entry));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_idb_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to follow IDB port down sequence for flexport operation.
 *         Follows the sequence specified in document "HX5_uA_flexport.pdf", Section
 *         6.6.1.1.
 *         Two main parts:
 *            (1) Poll untill IDB buffers are empty.
 *            (2) Hold IDB buffers in reset state.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_flex_idb_port_down(int                        unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int reset_buffer;
    int phy_port;
    int subp;

    /* Poll until IDB buffers are empty for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
	    if(phy_port == HELIX5_PHY_PORT_CPU) 
	    					{
                if (!soc_helix5_idb_cpu_ca_in_reset(unit,pipe_num)){
                    SOC_IF_ERROR_RETURN(soc_helix5_idb_ca_cpu_poll_buffer_empty(
                                                         unit, pipe_num));
                }
            } else if((phy_port == HELIX5_PHY_PORT_LPBK0)) {
                if (!soc_helix5_idb_lpbk_ca_in_reset(unit,pipe_num)){
                    SOC_IF_ERROR_RETURN(soc_helix5_idb_ca_lpbk_poll_buffer_empty(
                                                             unit, pipe_num));
                }
            } else {

            SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_poll_buffer_empty(unit,
                                                                        pipe_num,
                                                                        pm_num,
                                                                        subp));
            SOC_IF_ERROR_RETURN(soc_helix5_idb_ca_poll_buffer_empty(unit,
                                                                       pipe_num,
                                                                       pm_num,
                                                                       subp,
								       phy_port));
                }
        }
    }

    /* Hold IDB buffers in reset state for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
            reset_buffer = 1;
            SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_reset_buffer(unit,
                                                                  pipe_num,
                                                                  pm_num, subp,
								  phy_port,
                                                                  reset_buffer));
	    if(phy_port <49)
	    subp = (phy_port-1)%16;

            SOC_IF_ERROR_RETURN(soc_helix5_idb_ca_reset_buffer(unit,
                                                                 pipe_num,
                                                                 pm_num, subp,
                                                                 reset_buffer));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_flex_idb_reconfigure(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to reconfigure IDB during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_flex_idb_reconfigure(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int phy_port, idb_port;
    int physical_port, logical_port;
    int subp, valid;
    int num_lanes;
    int lossless;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int qmode;
    int inst;
    uint64 rval64;
    soc_mem_t mem;

    static soc_mem_t idb_to_dev_port_map_mem[1] =
          {ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLE_PIPE0m};
    /* Ingress physical to IDB port mapping */
    static soc_mem_t phy_to_idb_port_map_mem[1] =
          {ING_PHY_TO_IDB_PORT_MAPm};

    if (1 == port_schedule_state->is_flexport) {
        /* Initial TDM already configured */

        SOC_IF_ERROR_RETURN(soc_helix5_tdm_flexport_idb(unit,
                                                       port_schedule_state));
        SOC_IF_ERROR_RETURN(_soc_hx5_tdm_set_idb_hsp(unit,
                                                       port_schedule_state));
        SOC_IF_ERROR_RETURN(_soc_hx5_tdm_set_idb_calendar(unit,
                                                            port_schedule_state));
    }

    /* Update IDB_TO_DEVICE_PORT_MAPPING TABLE */
    for(i = 0; i < port_schedule_state->nport; i++) {
        physical_port = port_schedule_state->resource[i].physical_port;
        logical_port = port_schedule_state->resource[i].logical_port;
        
        if(physical_port == -1) {
        	idb_port = 
            port_schedule_state->in_port_map.port_l2i_mapping[logical_port];
        	memfld = 0x7f ; 
        	physical_port = 
            port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
	        valid = 0;
        } else {
        	idb_port = port_schedule_state->resource[i].idb_port;
        	memfld = logical_port;
	        valid = 1;
        }
        
        mem = idb_to_dev_port_map_mem[0];	
        
        sal_memset(&entry, 0, sizeof(entry));
        soc_mem_field_set(unit, mem, entry, DEVICE_PORT_NUMBERf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 
                                                  idb_port, &entry));
	
        memfld = valid;
        mem = phy_to_idb_port_map_mem[0];	
        sal_memset(&entry, 0, sizeof(entry));
        soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, valid == 0 ? 0x7f : idb_port);
        soc_mem_field_set(unit, mem, entry, VALIDf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 
                                                  (physical_port-1), &entry));

    }

    /* Reconfigure OBM thresholds for ports going up */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
			logical_port = port_schedule_state->resource[i].logical_port;
	
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[logical_port];
          /* checking for qsgmii ports */
	    if(phy_port < 49){
	    inst = ((phy_port - 1) / 16 | SOC_REG_ADDR_INSTANCE_MASK);
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                                                    0, &rval64));
            qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                                                QMODEf);
	   }else
	   {
	    qmode = 0;
	   }

            if (!SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, logical_port)) {
	    if(qmode == 1)
                continue;
            }

            num_lanes =
                port_schedule_state->out_port_map.port_num_lanes[logical_port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
            lossless = port_schedule_state->lossless;
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Reconfigure IDB: Write OBM thresh i %1d "
                      "phy_port %1d pipe=%1d pm=%1d subp=%1d "
                      "num_lanes=%1d\n"),
                      i,phy_port, pipe_num, pm_num, subp, num_lanes));

            SOC_IF_ERROR_RETURN(soc_helix5_idb_wr_obm_ovs_en(unit, pipe_num,
                                                                pm_num,subp,phy_port ));
            SOC_IF_ERROR_RETURN(soc_helix5_idb_wr_obm_buffer_limit(unit, pipe_num,
                                                                pm_num,subp,
                                                                num_lanes));

            SOC_IF_ERROR_RETURN(soc_helix5_idb_wr_obm_thresh(unit, pipe_num,
                                                                pm_num,subp,
                                                                num_lanes,
                                                                lossless));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Write Srared config i %1d pipe=%1d pm=%1d "
                      "subp=%1d num_lanes=%1d\n"),
                      i, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_helix5_idb_wr_obm_shared_config(unit,
                                                                      pipe_num,
                                                                      pm_num,
                                                                      subp,
                                                                      num_lanes,
                                                                      lossless));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Write Flow_ctrl_cfg i %1d pipe=%1d pm=%1d "
                      "subp=%1d num_lanes=%1d\n"),
                      i, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_helix5_idb_wr_obm_flow_ctrl_cfg(unit,
                                                                      pipe_num,
                                                                      pm_num,
                                                                      subp,
                                                                      lossless));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Write fc_thresh i %1d pipe=%1d pm=%1d subp=%1d "
                      "num_lanes=%1d\n"),
                      i, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_helix5_idb_wr_obm_fc_threshold(unit,
                                                                     pipe_num,
                                                                     pm_num,
                                                                     subp,
                                                                     num_lanes,
                                                                     lossless));
        }
    }

   SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_bubble_mop_set(unit, 
                                                   port_schedule_state));
   
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_helix5_idb_port_mode_set \n")));
    SOC_IF_ERROR_RETURN(soc_helix5_idb_port_mode_set(unit,
                                                     port_schedule_state));
  /* LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
   *           "Call soc_helix5_idb_obm_force_saf_set \n")));
   * SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_force_saf_set(unit,
   *                                                  port_schedule_state));
   */                                                  
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_helix5_idb_obm_port_config_set \n")));
    SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_port_config_set(unit,
                                                     port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_helix5_idb_obm_dscp_map_set \n")));
    SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_dscp_map_set(unit,
                                                     port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_helix5_idb_clear_stats_new_ports \n")));
    SOC_IF_ERROR_RETURN(soc_helix5_idb_clear_stats_new_ports(unit,
                                                     port_schedule_state));

    return SOC_E_NONE;
}

/*! @fn int soc_helix5_flex_idb_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to bringup IDB port during flex operation.
 *  @returns SOC_E_NONE
 */
int
soc_helix5_flex_idb_port_up(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int reset_buffer;
    int phy_port;
    int subp;

    /* Release IDB buffers from reset state for all the ports going up */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
            reset_buffer = 1;
            if (HELIX5_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_reset_buffer(unit,
                                                                pipe_num,
                                                                pm_num, subp,
								  phy_port,
                                                                reset_buffer));
            }
            if (HELIX5_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
	    if(phy_port <49)
	    subp = (phy_port-1)%16;

                SOC_IF_ERROR_RETURN(soc_helix5_idb_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               pm_num, subp,
                                                               reset_buffer));
            }else if((phy_port==HELIX5_PHY_PORT_LPBK0)
                     ){
                SOC_IF_ERROR_RETURN(soc_helix5_idb_lpbk_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }else if((phy_port==HELIX5_PHY_PORT_CPU))
	    {
                SOC_IF_ERROR_RETURN(soc_helix5_idb_cpu_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }
        }
    }
    sal_usleep(5 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            pipe_num = soc_helix5_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_helix5_get_pm_from_phy_pnum(phy_port);
            subp = soc_helix5_get_subp_from_phy_pnum(phy_port);
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                             "IDB port Up i %1d phy_port %1d pipe=%1d pm=%1d "
                             "subp=%1d \n"),
                     i,
                     phy_port, pipe_num, pm_num, subp));
            reset_buffer = 0;
            if (HELIX5_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_helix5_idb_obm_reset_buffer(unit,
                                                              pipe_num,
                                                              pm_num, subp,
								  phy_port,
                                                              reset_buffer));
            }
            if (HELIX5_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
	    if(phy_port <49)
	    subp = (phy_port-1)%16;
                SOC_IF_ERROR_RETURN(soc_helix5_idb_ca_reset_buffer(unit,
                                                              pipe_num,
                                                              pm_num, subp,
                                                              reset_buffer));
            }else if((phy_port==HELIX5_PHY_PORT_LPBK0))
	    {
                SOC_IF_ERROR_RETURN(soc_helix5_idb_lpbk_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }else if((phy_port==HELIX5_PHY_PORT_CPU)){
                SOC_IF_ERROR_RETURN(soc_helix5_idb_cpu_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }
        }
    }
    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

int
soc_helix5_idb_flexport(int                        unit,
                           soc_port_schedule_state_t *port_schedule_state)
{

    SOC_IF_ERROR_RETURN(soc_helix5_flex_start(unit, port_schedule_state));

    /* IDB Ports down. */
    SOC_IF_ERROR_RETURN(soc_helix5_flex_idb_port_down(unit,
                                                    port_schedule_state));

    /* startTDM flex related codes: Florin's routines.*/
    SOC_IF_ERROR_RETURN(soc_helix5_tdm_calculation_flexport(unit,
                                                    port_schedule_state));

    /* Reconfigure IDB. */
    SOC_IF_ERROR_RETURN(soc_helix5_flex_idb_reconfigure(unit,
                                                    port_schedule_state));

    /* IDB Ports up*/
    SOC_IF_ERROR_RETURN(soc_helix5_flex_idb_port_up(unit,
                                                    port_schedule_state));

    SOC_IF_ERROR_RETURN(soc_helix5_flex_end(unit, port_schedule_state));

    return SOC_E_NONE;
}
#endif /* BCM_HELIX5_SUPPORT */
