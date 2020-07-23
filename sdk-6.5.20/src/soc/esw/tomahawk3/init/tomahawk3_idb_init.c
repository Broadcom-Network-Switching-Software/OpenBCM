/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_idb_init.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/init/tomahawk3_idb_init.h>
#include <soc/flexport/flexport_common.h>

/*** START SDK API COMMON CODE ***/


/*! @file tomahawk3_idb_init.c
 *  @brief IDB init for Tomahawk3.
 *  Details are shown below.
 */

static const struct soc_tomahawk3_obm_buffer_config_s {
    int buffer_start;
    int buffer_end;
} soc_tomahawk3_obm_buffer_config_settings[9][8] = {
    /* indexed by number of lanes used in the port */
    { /* 0 lanes - invalid */ {0},{0},{0},{0},{0},{0},{0},{0}},
    { /* 1 lane */
        {0,459},         /* buffer_config port 0 */
        {460,919},       /* buffer_config port 1 */
        {920,1379},      /* buffer_config port 2 */
        {1380,1839},     /* buffer_config port 3 */
        {1840,2299},     /* buffer_config port 4 */
        {2300,2759},     /* buffer_config port 5 */
        {2760,3219},     /* buffer_config port 6 */
        {3220,3679}      /* buffer_config port 7 */
    },
    { /* 2 lanes */
        {0,919},         /* buffer_config port 0 */
        {0,919},         /* buffer_config port 1 */
        {920,1839},      /* buffer_config port 2 */
        {920,1839},      /* buffer_config port 3 */
        {1840,2759},     /* buffer_config port 4 */
        {1840,2759},     /* buffer_config port 5 */
        {2760,3679},     /* buffer_config port 6 */
        {2760,3679}      /* buffer_config port 7 */
    },
    { /* 3 lanes - invalid */ {0},{0},{0},{0},{0},{0},{0},{0}},
    { /* 4 lanes */
        {0,1839},        /* buffer_config port 0 */
        {0,1839},        /* buffer_config port 1 */
        {0,1839},        /* buffer_config port 2 */
        {0,1839},        /* buffer_config port 3 */
        {1840,3679},     /* buffer_config port 4 */
        {1840,3679},     /* buffer_config port 5 */
        {1840,3679},     /* buffer_config port 6 */
        {1840,3679}      /* buffer_config port 7 */
    },
    { /* 5 lanes - invalid */ {0},{0},{0},{0},{0},{0},{0},{0}},
    { /* 6 lanes - invalid */ {0},{0},{0},{0},{0},{0},{0},{0}},
    { /* 7 lanes - invalid */ {0},{0},{0},{0},{0},{0},{0},{0}},
    { /* 8 lanes */
        {0,3679},        /* buffer_config port 0 */
        {0,3679},        /* buffer_config port 1 */
        {0,3679},        /* buffer_config port 2 */
        {0,3679},        /* buffer_config port 3 */
        {0,3679},        /* buffer_config port 4 */
        {0,3679},        /* buffer_config port 5 */
        {0,3679},        /* buffer_config port 6 */
        {0,3679}         /* buffer_config port 7 */
    }
};

static const struct soc_tomahawk3_obm_setting_s {
    int discard_limit;
    int lossless_discard;
    int port_xoff;
    int port_xon;
    int lossless_xoff;
    int lossless_xon;
    int lossy_low_pri;
    int lossy_discard;
    int sop_discard_mode;
} soc_tomahawk3_obm_settings[3][9] = {
    /* LOSSY Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            457,             /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            370,             /* lossy_low_pri */
            455,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            917,             /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            830,             /* lossy_low_pri */
            915,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            1750,            /* lossy_low_pri */
            1835,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid*/ 0 },
        { /* 6 lanes - Invalid*/ 0 },
        { /* 7 lanes - Invalid*/ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            3590,            /* lossy_low_pri */
            3675,            /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            457,             /* discard_limit */
            3677,            /* lossless_discard */
            209,             /* port_xoff */
            201,             /* port_xon */
            81,              /* lossless_xoff */
            73,              /* lossless_xon */
            60,              /* lossy_low_pri */
            145,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            917,             /* discard_limit */
            3677,            /* lossless_discard */
            524,             /* port_xoff */
            516,             /* port_xon */
            194,             /* lossless_xoff */
            186,             /* lossless_xon */
            60,              /* lossy_low_pri */
            145,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 lanes - invalid  */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            3677,            /* lossless_discard */
            1129,            /* port_xoff */
            1121,            /* port_xon */
            384,             /* lossless_xoff */
            376,             /* lossless_xon */
            60,              /* lossy_low_pri */
            145,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid*/ 0 },
        { /* 6 lanes - Invalid*/ 0 },
        { /* 7 lanes - Invalid*/ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            3677,            /* lossless_discard */
            2372,            /* port_xoff */
            2364,            /* port_xon */
            768,             /* lossless_xoff */
            760,             /* lossless_xon */
            205,              /* lossy_low_pri */
            290,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSY + LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            457,             /* discard_limit */
            3677,            /* lossless_discard */
            209,             /* port_xoff */
            201,             /* port_xon */
            81,              /* lossless_xoff */
            73,              /* lossless_xon */
            60,              /* lossy_low_pri */
            145,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            917,             /* discard_limit */
            3677,            /* lossless_discard */
            524,             /* port_xoff */
            516,             /* port_xon */
            194,             /* lossless_xoff */
            186,             /* lossless_xon */
            60,              /* lossy_low_pri */
            145,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            3677,            /* lossless_discard */
            1129,            /* port_xoff */
            1121,            /* port_xon */
            384,             /* lossless_xoff */
            376,             /* lossless_xon */
            60,              /* lossy_low_pri */
            145,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid*/ 0 },
        { /* 6 lanes - Invalid*/ 0 },
        { /* 7 lanes - Invalid*/ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            3677,            /* lossless_discard */
            2372,            /* port_xoff */
            2364,            /* port_xon */
            768,             /* lossless_xoff */
            760,             /* lossless_xon */
            205,             /* lossy_low_pri */
            290,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    }
};

static const struct soc_tomahawk3_e2e_settings_s {
    int speed;
    /*Bubble MOP disable.*/
    uint8 bmop_disable[SOC_SWITCH_BYPASS_MODE_LOW + 1];
    int ca_ct_threshold;
    int obm_ct_threshold;
    int obm_unsatisfied_threshold;
    int ca_unsatisfied_threshold;
    int obm_enter_lagging_threshold;
    int obm_exit_lagging_threshold;
    int ca_enter_lagging_threshold;
    int ca_exit_lagging_threshold;
} soc_tomahawk3_e2e_settings_tbl[] = {
    {    -1, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* SAF     */
    { 10000, {1, 1, 1}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 10GE    */
    { 25000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 25GE    */
    { 40000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 40GE    */
    { 50000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 50GE    */
    {100000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* 100GE   */
    {200000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* 200GE   */
    {400000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20}        /* 400GE   */
};

static const struct soc_tomahawk3_peek_depth_settings_s {
    int speed;
    int peek_depth;
} soc_tomahawk3_peek_depth_settings_tbl[] = {
    {    -1, 6},       /* Default */
    { 10000, 2},       /* 10GE    */
    { 25000, 2},       /* 25GE    */
    { 40000, 2},       /* 40GE    */
    { 50000, 2},       /* 50GE    */
    {100000, 2},       /* 100GE   */
    {200000, 4},       /* 200GE   */
    {400000, 6}        /* 400GE   */
};

static const soc_reg_t obm_ctrl_regs[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] =
{
    {IDB_OBM0_CONTROL_PIPE0r, IDB_OBM1_CONTROL_PIPE0r,
     IDB_OBM2_CONTROL_PIPE0r, IDB_OBM3_CONTROL_PIPE0r},
    {IDB_OBM0_CONTROL_PIPE1r, IDB_OBM1_CONTROL_PIPE1r,
     IDB_OBM2_CONTROL_PIPE1r, IDB_OBM3_CONTROL_PIPE1r},
    {IDB_OBM0_CONTROL_PIPE2r, IDB_OBM1_CONTROL_PIPE2r,
     IDB_OBM2_CONTROL_PIPE2r, IDB_OBM3_CONTROL_PIPE2r},
    {IDB_OBM0_CONTROL_PIPE3r, IDB_OBM1_CONTROL_PIPE3r,
     IDB_OBM2_CONTROL_PIPE3r, IDB_OBM3_CONTROL_PIPE3r},
    {IDB_OBM0_CONTROL_PIPE4r, IDB_OBM1_CONTROL_PIPE4r,
     IDB_OBM2_CONTROL_PIPE4r, IDB_OBM3_CONTROL_PIPE4r},
    {IDB_OBM0_CONTROL_PIPE5r, IDB_OBM1_CONTROL_PIPE5r,
     IDB_OBM2_CONTROL_PIPE5r, IDB_OBM3_CONTROL_PIPE5r},
    {IDB_OBM0_CONTROL_PIPE6r, IDB_OBM1_CONTROL_PIPE6r,
     IDB_OBM2_CONTROL_PIPE6r, IDB_OBM3_CONTROL_PIPE6r},
    {IDB_OBM0_CONTROL_PIPE7r, IDB_OBM1_CONTROL_PIPE7r,
     IDB_OBM2_CONTROL_PIPE7r, IDB_OBM3_CONTROL_PIPE7r},
};

static const soc_reg_t obm_ram_ctrl_regs[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] =
{
    {IDB_OBM0_RAM_CONTROL_PIPE0r, IDB_OBM1_RAM_CONTROL_PIPE0r,
     IDB_OBM2_RAM_CONTROL_PIPE0r, IDB_OBM3_RAM_CONTROL_PIPE0r},
    {IDB_OBM0_RAM_CONTROL_PIPE1r, IDB_OBM1_RAM_CONTROL_PIPE1r,
     IDB_OBM2_RAM_CONTROL_PIPE1r, IDB_OBM3_RAM_CONTROL_PIPE1r},
    {IDB_OBM0_RAM_CONTROL_PIPE2r, IDB_OBM1_RAM_CONTROL_PIPE2r,
     IDB_OBM2_RAM_CONTROL_PIPE2r, IDB_OBM3_RAM_CONTROL_PIPE2r},
    {IDB_OBM0_RAM_CONTROL_PIPE3r, IDB_OBM1_RAM_CONTROL_PIPE3r,
     IDB_OBM2_RAM_CONTROL_PIPE3r, IDB_OBM3_RAM_CONTROL_PIPE3r},
    {IDB_OBM0_RAM_CONTROL_PIPE4r, IDB_OBM1_RAM_CONTROL_PIPE4r,
     IDB_OBM2_RAM_CONTROL_PIPE4r, IDB_OBM3_RAM_CONTROL_PIPE4r},
    {IDB_OBM0_RAM_CONTROL_PIPE5r, IDB_OBM1_RAM_CONTROL_PIPE5r,
     IDB_OBM2_RAM_CONTROL_PIPE5r, IDB_OBM3_RAM_CONTROL_PIPE5r},
    {IDB_OBM0_RAM_CONTROL_PIPE6r, IDB_OBM1_RAM_CONTROL_PIPE6r,
     IDB_OBM2_RAM_CONTROL_PIPE6r, IDB_OBM3_RAM_CONTROL_PIPE6r},
    {IDB_OBM0_RAM_CONTROL_PIPE7r, IDB_OBM1_RAM_CONTROL_PIPE7r,
     IDB_OBM2_RAM_CONTROL_PIPE7r, IDB_OBM3_RAM_CONTROL_PIPE7r},
};

static const soc_reg_t ca_control_regs[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] =
   {{IDB_CA0_CONTROL_PIPE0r, IDB_CA1_CONTROL_PIPE0r,
     IDB_CA2_CONTROL_PIPE0r, IDB_CA3_CONTROL_PIPE0r},
    {IDB_CA0_CONTROL_PIPE1r, IDB_CA1_CONTROL_PIPE1r,
     IDB_CA2_CONTROL_PIPE1r, IDB_CA3_CONTROL_PIPE1r},
    {IDB_CA0_CONTROL_PIPE2r, IDB_CA1_CONTROL_PIPE2r,
     IDB_CA2_CONTROL_PIPE2r, IDB_CA3_CONTROL_PIPE2r},
    {IDB_CA0_CONTROL_PIPE3r, IDB_CA1_CONTROL_PIPE3r,
     IDB_CA2_CONTROL_PIPE3r, IDB_CA3_CONTROL_PIPE3r},
    {IDB_CA0_CONTROL_PIPE4r, IDB_CA1_CONTROL_PIPE4r,
     IDB_CA2_CONTROL_PIPE4r, IDB_CA3_CONTROL_PIPE4r},
    {IDB_CA0_CONTROL_PIPE5r, IDB_CA1_CONTROL_PIPE5r,
     IDB_CA2_CONTROL_PIPE5r, IDB_CA3_CONTROL_PIPE5r},
    {IDB_CA0_CONTROL_PIPE6r, IDB_CA1_CONTROL_PIPE6r,
     IDB_CA2_CONTROL_PIPE6r, IDB_CA3_CONTROL_PIPE6r},
    {IDB_CA0_CONTROL_PIPE7r, IDB_CA1_CONTROL_PIPE7r,
     IDB_CA2_CONTROL_PIPE7r, IDB_CA3_CONTROL_PIPE7r},
};

static const soc_reg_t ca_control_1_regs[_TH3_PIPES_PER_DEV] = {
    IDB_CA_CONTROL_1_PIPE0r,
    IDB_CA_CONTROL_1_PIPE1r,
    IDB_CA_CONTROL_1_PIPE2r,
    IDB_CA_CONTROL_1_PIPE3r,
    IDB_CA_CONTROL_1_PIPE4r,
    IDB_CA_CONTROL_1_PIPE5r,
    IDB_CA_CONTROL_1_PIPE6r,
    IDB_CA_CONTROL_1_PIPE7r
};

static const soc_mem_t pnum_map_tbl[_TH3_PIPES_PER_DEV] = {
    ING_PHY_TO_IDB_PORT_MAP_PIPE0m, ING_PHY_TO_IDB_PORT_MAP_PIPE1m,
    ING_PHY_TO_IDB_PORT_MAP_PIPE2m, ING_PHY_TO_IDB_PORT_MAP_PIPE3m,
    ING_PHY_TO_IDB_PORT_MAP_PIPE4m, ING_PHY_TO_IDB_PORT_MAP_PIPE5m,
    ING_PHY_TO_IDB_PORT_MAP_PIPE6m, ING_PHY_TO_IDB_PORT_MAP_PIPE7m
};

static const soc_mem_t idb_to_dev_pmap_tbl[_TH3_PIPES_PER_DEV] = {
    ING_IDB_TO_DEVICE_PORT_MAP_PIPE0m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE1m,
    ING_IDB_TO_DEVICE_PORT_MAP_PIPE2m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE3m,
    ING_IDB_TO_DEVICE_PORT_MAP_PIPE4m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE5m,
    ING_IDB_TO_DEVICE_PORT_MAP_PIPE6m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE7m
};

static const soc_reg_t th3_idb_ca_lpbk_ctrl[_TH3_PIPES_PER_DEV]= {
         IDB_CA_LPBK_CONTROL_PIPE0r,
         IDB_CA_LPBK_CONTROL_PIPE1r,
         IDB_CA_LPBK_CONTROL_PIPE2r,
         IDB_CA_LPBK_CONTROL_PIPE3r,
         IDB_CA_LPBK_CONTROL_PIPE4r,
         IDB_CA_LPBK_CONTROL_PIPE5r,
         IDB_CA_LPBK_CONTROL_PIPE6r,
         IDB_CA_LPBK_CONTROL_PIPE7r};

static const soc_reg_t th3_idb_ca_cpu_ctrl[_TH3_PIPES_PER_DEV]= {
         IDB_CA_CPU_CONTROL_PIPE0r,
         IDB_CA_CPU_CONTROL_PIPE1r,
         IDB_CA_CPU_CONTROL_PIPE2r,
         IDB_CA_CPU_CONTROL_PIPE3r,
         IDB_CA_CPU_CONTROL_PIPE4r,
         IDB_CA_CPU_CONTROL_PIPE5r,
         IDB_CA_CPU_CONTROL_PIPE6r,
         IDB_CA_CPU_CONTROL_PIPE7r};

static const soc_reg_t th3_idb_ca_control_2[_TH3_PIPES_PER_DEV] = {
        IDB_CA_CONTROL_2_PIPE0r, IDB_CA_CONTROL_2_PIPE1r,
        IDB_CA_CONTROL_2_PIPE2r, IDB_CA_CONTROL_2_PIPE3r,
        IDB_CA_CONTROL_2_PIPE4r, IDB_CA_CONTROL_2_PIPE5r,
        IDB_CA_CONTROL_2_PIPE6r, IDB_CA_CONTROL_2_PIPE7r
    };

/*! @fn int soc_tomahawk3_get_pipe_from_phy_port(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate pipe number from physical port number.
           This function works for both front panel ports &
           non-front panel ports.
 *  @returns pipe_number
 */
int
soc_tomahawk3_get_pipe_from_phy_port(int pnum)
{
    return((pnum==_TH3_PHY_PORT_LPBK0)                        ? 0 :
           (pnum==_TH3_PHY_PORT_LPBK1)                        ? 1 :
           (pnum==_TH3_PHY_PORT_LPBK2)                        ? 2 :
           (pnum==_TH3_PHY_PORT_LPBK3)                        ? 3 :
           (pnum==_TH3_PHY_PORT_LPBK4)                        ? 4 :
           (pnum==_TH3_PHY_PORT_LPBK5)                        ? 5 :
           (pnum==_TH3_PHY_PORT_LPBK6)                        ? 6 :
           (pnum==_TH3_PHY_PORT_LPBK7)                        ? 7 :
           (pnum==_TH3_PHY_PORT_CPU)                          ? 0 :
           (pnum==_TH3_PHY_PORT_MNG0)                         ? 1 :
           (pnum==_TH3_PHY_PORT_MNG1)                         ? 5 :
           (((pnum - 1) >> 5) & 7));
}

/*! @fn int soc_tomahawk3_get_pipe_pm_from_phy_port(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate PM number from physical port number.
           This function should be called only for front panel ports.
 *  @returns PM number
 */
int
soc_tomahawk3_get_pipe_pm_from_phy_port(int pnum)
{
    return (((pnum - 1) & 0x1f) >> 3);
}

/*! @fn int soc_tomahawk3_get_subp_from_phy_port(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate subport number from physical port number.
           This function should be called only for front panel ports.
 *  @returns subport number
 */
int
soc_tomahawk3_get_subp_from_phy_port(int pnum)
{
    return ((pnum - 1) & 0x7);
}

/*! @fn int soc_tomahawk3_phy_is_lpbk_port(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to check if the physical port is loopback port.
 *  @returns 1 or 0
 */
int
soc_tomahawk3_phy_is_lpbk_port(int pnum)
{
    return ((pnum==_TH3_PHY_PORT_LPBK0)||
            (pnum==_TH3_PHY_PORT_LPBK1)||
            (pnum==_TH3_PHY_PORT_LPBK2)||
            (pnum==_TH3_PHY_PORT_LPBK3)||
            (pnum==_TH3_PHY_PORT_LPBK4)||
            (pnum==_TH3_PHY_PORT_LPBK5)||
            (pnum==_TH3_PHY_PORT_LPBK6)||
            (pnum==_TH3_PHY_PORT_LPBK7));
}

/*! @fn int soc_tomahawk3_phy_is_cpu_mgmt_port(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to check if the physical port is either CPU or
 *         management port.
 *  @returns 1 or 0
 */
int
soc_tomahawk3_phy_is_cpu_mgmt_port(int pnum)
{
    return ((pnum==_TH3_PHY_PORT_CPU)||
            (pnum==_TH3_PHY_PORT_MNG0)||
            (pnum==_TH3_PHY_PORT_MNG1));
}

/*! @fn int soc_tomahawk3_idb_pa_reset(int unit,
            soc_port_map_type_t *port_map, int lport, int reset)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @param reset IDB CA write clock domain reset. 1 - Apply reset,
                                                  0 - Release reset.
 *  @brief Helper function to apply/ or release IDB OBM buffer reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_pa_reset(int unit,
    soc_port_map_type_t *port_map, int lport, int reset)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pipe_num;
    int pm_num;
    int rst_l;
    static const soc_reg_t idb_pa_reset_ctrl_regs[_TH3_PIPES_PER_DEV] = {
        IDB_PA_RESET_CONTROL_PIPE0r,
        IDB_PA_RESET_CONTROL_PIPE1r,
        IDB_PA_RESET_CONTROL_PIPE2r,
        IDB_PA_RESET_CONTROL_PIPE3r,
        IDB_PA_RESET_CONTROL_PIPE4r,
        IDB_PA_RESET_CONTROL_PIPE5r,
        IDB_PA_RESET_CONTROL_PIPE6r,
        IDB_PA_RESET_CONTROL_PIPE7r
    };
    static const soc_field_t idb_pa_reset_fields[] = {
        PM0_PA_RST_Lf, PM1_PA_RST_Lf, PM2_PA_RST_Lf, PM3_PA_RST_Lf
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    reg = idb_pa_reset_ctrl_regs[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                           reg, REG_PORT_ANY, 0, &rval32));
    rst_l = reset ? 0 : 1;
    soc_reg_field_set(unit, reg, &rval32, idb_pa_reset_fields[pm_num], rst_l);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                        REG_PORT_ANY, 0, rval32));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_obm_reset(int unit,
            soc_port_map_type_t *port_map, int lport, int reset)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @param reset IDB CA write clock domain reset. 1 - Apply reset,
                                                  0 - Release reset.
 *  @brief Helper function to apply/ or release IDB OBM buffer reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_obm_reset(int unit,
    soc_port_map_type_t *port_map, int lport, int reset)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pipe_num;
    int pm_num;
    int subp;
    static const soc_field_t obm_port_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    reg = obm_ctrl_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                           reg, REG_PORT_ANY, 0, &rval64));
    COMPILER_64_ZERO(fldval64);
    soc_reg64_field_set(unit, reg, &rval64, OBM_RESETf, fldval64);
    COMPILER_64_SET(fldval64,0, reset);
    soc_reg64_field_set(unit, reg, &rval64,
             obm_port_reset_fields[subp], fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                        REG_PORT_ANY, 0, rval64));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_obm_rst_rel_all(int unit)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to release IDB OBM buffer reset
           for all the IDB front panel ports during init time bringup.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_obm_rst_rel_all(int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    uint32 pipe_map;
    int pipe_num;
    int pm_num;
    int subp;
    int pnum;
    int lport;
    int tsc_enabled;
    uint16 dev_id;
    uint8 rev_id;

    static const soc_field_t obm_port_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    soc_cm_get_id(unit, &dev_id, &rev_id);
  
    soc_tomahawk3_get_pipe_map(unit,port_schedule_state, &pipe_map);
    for (pipe_num = 0; pipe_num < _TH3_PIPES_PER_DEV; pipe_num++) {
        if ((pipe_map >> pipe_num) & 1){
            for (pm_num = 0; pm_num < _TH3_PBLKS_PER_PIPE; pm_num++) {
                tsc_enabled=0;
                for (subp = 0; subp < _TH3_PORTS_PER_PBLK; subp++) {
                   pnum = ((pipe_num<<5) | (pm_num<<3) | subp) +1;
                   lport = port_schedule_state->
                           out_port_map.port_p2l_mapping[pnum];
                   tsc_enabled |=
                       (port_schedule_state->
                       out_port_map.log_port_speed[lport] > 0 )?1:0;
                }
                if (tsc_enabled) {
                    reg = obm_ctrl_regs[pipe_num][pm_num];
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                        reg, REG_PORT_ANY, 0, &rval64));
                    COMPILER_64_ZERO(fldval64);
                    soc_reg64_field_set(unit, reg, &rval64,
                        OBM_RESETf, fldval64);

                    for (subp = 0; subp < _TH3_PORTS_PER_PBLK; subp++) {
                        COMPILER_64_ZERO(fldval64);
                        soc_reg64_field_set(unit, reg, &rval64,
                            obm_port_reset_fields[subp], fldval64);
                    }
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                                        REG_PORT_ANY, 0, rval64));
                  
                    if (rev_id == BCM56980_A0_REV_ID) {
                        reg = obm_ram_ctrl_regs[pipe_num][pm_num];
                        COMPILER_64_SET(rval64, 0, 0x180);
                        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                                        REG_PORT_ANY, 0, rval64));
                    }

                }
            }
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_buffer_config(int unit,
 *          soc_port_map_type_t *port_map, int lport)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @brief Helper function to Configure CA buffer start & buffer end address.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_buffer_config(
    int unit, soc_port_map_type_t *port_map, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 buffer_start_addr;
    uint32 buffer_end_addr;
    int phy_port;
    int pm_num;
    int subp;
    int num_of_lanes;
    int pipe_num;
    static const soc_reg_t ca_buffer_config_regs[_TH3_PIPES_PER_DEV]
       [_TH3_PBLKS_PER_PIPE] =
       {{IDB_CA0_BUFFER_CONFIG_PIPE0r, IDB_CA1_BUFFER_CONFIG_PIPE0r,
         IDB_CA2_BUFFER_CONFIG_PIPE0r, IDB_CA3_BUFFER_CONFIG_PIPE0r},
        {IDB_CA0_BUFFER_CONFIG_PIPE1r, IDB_CA1_BUFFER_CONFIG_PIPE1r,
         IDB_CA2_BUFFER_CONFIG_PIPE1r, IDB_CA3_BUFFER_CONFIG_PIPE1r},
        {IDB_CA0_BUFFER_CONFIG_PIPE2r, IDB_CA1_BUFFER_CONFIG_PIPE2r,
         IDB_CA2_BUFFER_CONFIG_PIPE2r, IDB_CA3_BUFFER_CONFIG_PIPE2r},
        {IDB_CA0_BUFFER_CONFIG_PIPE3r, IDB_CA1_BUFFER_CONFIG_PIPE3r,
         IDB_CA2_BUFFER_CONFIG_PIPE3r, IDB_CA3_BUFFER_CONFIG_PIPE3r},
        {IDB_CA0_BUFFER_CONFIG_PIPE4r, IDB_CA1_BUFFER_CONFIG_PIPE4r,
         IDB_CA2_BUFFER_CONFIG_PIPE4r, IDB_CA3_BUFFER_CONFIG_PIPE4r},
        {IDB_CA0_BUFFER_CONFIG_PIPE5r, IDB_CA1_BUFFER_CONFIG_PIPE5r,
         IDB_CA2_BUFFER_CONFIG_PIPE5r, IDB_CA3_BUFFER_CONFIG_PIPE5r},
        {IDB_CA0_BUFFER_CONFIG_PIPE6r, IDB_CA1_BUFFER_CONFIG_PIPE6r,
         IDB_CA2_BUFFER_CONFIG_PIPE6r, IDB_CA3_BUFFER_CONFIG_PIPE6r},
        {IDB_CA0_BUFFER_CONFIG_PIPE7r, IDB_CA1_BUFFER_CONFIG_PIPE7r,
         IDB_CA2_BUFFER_CONFIG_PIPE7r, IDB_CA3_BUFFER_CONFIG_PIPE7r},
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    num_of_lanes = port_map->port_num_lanes[lport];
    buffer_start_addr = subp * _TH3_CA_BUFFER_SIZE / _TH3_PORTS_PER_PBLK;
    buffer_end_addr = buffer_start_addr +
           num_of_lanes * _TH3_CA_BUFFER_SIZE/_TH3_PORTS_PER_PBLK -1;
    reg = ca_buffer_config_regs[pipe_num][pm_num];
    rval32 = 0;
    soc_reg_field_set(unit, reg, &rval32, BUFFER_STARTf,
                                           buffer_start_addr);
    soc_reg_field_set(unit, reg, &rval32, BUFFER_ENDf,
                                           buffer_end_addr);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
                                  reg, REG_PORT_ANY, subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_wr_domain_reset(int unit,
            soc_port_map_type_t *port_map, int lport, int reset)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @param reset IDB CA write clock domain reset. 1 - Apply reset,
                                                  0 - Release reset.
 *  @brief Helper function to apply/release IDB CA write clock domain reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_wr_domain_reset(
    int unit, soc_port_map_type_t *port_map, int lport, int reset)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 fldval32;
    int phy_port;
    int pipe_num;
    int pm_num;
    int subp;
    static const soc_field_t ca_ctrl_reg_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    reg = ca_control_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                           reg, REG_PORT_ANY, 0, &rval32));
    fldval32=reset;
    soc_reg_field_set(unit, reg, &rval32,
        ca_ctrl_reg_reset_fields[subp], fldval32);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                        REG_PORT_ANY, 0, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_wr_dom_rst_rel_all(int unit,
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to release IDB CA write clock domain reset
           for all the IDB front panel ports during init time bringup.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_wr_dom_rst_rel_all( int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 fldval32;
    uint32 pipe_map;
    int pipe_num;
    int pm_num;
    int subp;
    int pnum;
    int lport;
    int tsc_enabled;
    static const soc_field_t ca_ctrl_reg_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    soc_tomahawk3_get_pipe_map(unit,port_schedule_state, &pipe_map);
    for (pipe_num = 0; pipe_num < _TH3_PIPES_PER_DEV; pipe_num++) {
        if ((pipe_map >> pipe_num) & 1){
            for (pm_num = 0; pm_num < _TH3_PBLKS_PER_PIPE; pm_num++) {
                tsc_enabled=0;
                for (subp = 0; subp < _TH3_PORTS_PER_PBLK; subp++) {
                   pnum = ((pipe_num<<5) | (pm_num<<3) | subp) +1;
                   lport = port_schedule_state->
                           out_port_map.port_p2l_mapping[pnum];
                   tsc_enabled |=
                       (port_schedule_state->
                       out_port_map.log_port_speed[lport] > 0 )?1:0;
                }
                if (tsc_enabled) {
                    reg = ca_control_regs[pipe_num][pm_num];
                    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                                           reg, REG_PORT_ANY, 0, &rval32));
                    for (subp = 0; subp < _TH3_PORTS_PER_PBLK; subp++) {
                        fldval32=0;
                        soc_reg_field_set(unit, reg, &rval32,
                            ca_ctrl_reg_reset_fields[subp], fldval32);
                    }
                    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                                        REG_PORT_ANY, 0, rval32));
                }
            }
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_rd_domain_reset(int unit,
            soc_port_map_type_t *port_map, int lport, int reset)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @param reset IDB CA read clock domain reset. 1 - Apply reset,
                                                  0 - Release reset.
 *  @brief Helper function to release IDB CA read clock domain reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_rd_domain_reset(
    int unit, soc_port_map_type_t *port_map, int lport, int reset)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pipe_num;
    int pm_num;
    int subp;
    static const soc_field_t ca_ctrl_1_reg_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    reg = ca_control_1_regs[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                           reg, REG_PORT_ANY, pm_num, &rval32));
    soc_reg_field_set(unit, reg, &rval32,
        ca_ctrl_1_reg_reset_fields[subp], reset);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                        REG_PORT_ANY, pm_num, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_rd_dom_rst_rel_all(int unit,
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to release IDB CA read clock domain reset
           for all the IDB front panel ports during init time bringup.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_rd_dom_rst_rel_all(int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 pipe_map;
    int pipe_num;
    int pm_num;
    int subp;
    int pnum;
    int lport;
    int tsc_enabled;
    static const soc_field_t ca_ctrl_1_reg_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    soc_tomahawk3_get_pipe_map(unit,port_schedule_state, &pipe_map);
    for (pipe_num = 0; pipe_num < _TH3_PIPES_PER_DEV; pipe_num++) {
        if ((pipe_map >> pipe_num) & 1){
            for (pm_num = 0; pm_num < _TH3_PBLKS_PER_PIPE; pm_num++) {
                tsc_enabled=0;
                for (subp = 0; subp < _TH3_PORTS_PER_PBLK; subp++) {
                   pnum = ((pipe_num<<5) | (pm_num<<3) | subp) +1;
                   lport = port_schedule_state->
                           out_port_map.port_p2l_mapping[pnum];
                   tsc_enabled |=
                       (port_schedule_state->
                       out_port_map.log_port_speed[lport] > 0 )?1:0;
                }
                if (tsc_enabled) {
                    reg = ca_control_1_regs[pipe_num];
                    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                                   reg, REG_PORT_ANY, pm_num, &rval32));
                    for (subp = 0; subp < _TH3_PORTS_PER_PBLK; subp++) {
                        soc_reg_field_set(unit, reg, &rval32,
                            ca_ctrl_1_reg_reset_fields[subp], 0);
                    }
                    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                                        REG_PORT_ANY, pm_num, rval32));
                }
            }
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_init_pnum_mapping_tbl(int unit,
 *          soc_port_map_type_t *port_map, int lport)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @brief Helper function to initialize phy to idb port mapping table for the
           front panel ports comming up. This function should be called
           only for front panel ports
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_init_pnum_mapping_tbl(
    int unit, soc_port_map_type_t *port_map, int lport)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int pipe, phy_port, idb_port;

    phy_port = port_map->port_l2p_mapping[lport];
    if (!_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)){
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "ERROR soc_tomahawk3_idb_init_pnum_mapping_tbl function"
            "should be called only for front panel ports.\n "
            "lport %0d phy_port %0d"),lport,phy_port));
        return SOC_E_PARAM;
    }
    sal_memset(entry, 0, sizeof(entry));
    idb_port = port_map->port_l2i_mapping[lport];
    pipe = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    mem = pnum_map_tbl[pipe];
    memfld = idb_port;
    soc_mem_field_set(unit, mem, entry, IDB_PORTf, &memfld);
    memfld = 1;
    soc_mem_field_set(unit, mem, entry, VALIDf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
        (phy_port - 1 - (pipe * _TH3_GPHY_PORTS_PER_PIPE)), entry));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_init_idb_to_dev_pmap_tbl(int unit,
            soc_port_map_type_t *port_map, int lport)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @brief Helper function to initialize idb to local device port number
           for the ports comming up. This function works for both
           front panel ports & non-front panel ports.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_init_idb_to_dev_pmap_tbl(
    int unit, soc_port_map_type_t *port_map, int lport)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int pipe, phy_port, idb_port;
    int local_dev_port;

    sal_memset(entry, 0, sizeof(entry));
    phy_port = port_map->port_l2p_mapping[lport];
    idb_port = port_map->port_l2i_mapping[lport];
    pipe = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    local_dev_port =
        port_map->port_p2l_mapping[phy_port] % _TH3_DEV_PORTS_PER_PIPE;
    mem = idb_to_dev_pmap_tbl[pipe];
    memfld = local_dev_port;
    soc_mem_field_set(unit, mem, entry, DEVICE_PORTf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      idb_port, entry));
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_idb_init_idb_to_dev_all_pipe(int unit,
            int pipe)
 *  @param unit Chip unit number.
 *  @param pipe pipe num
 *  @brief Helper function to initialize idb to local device port number
           This function works for both
           front panel ports & non-front panel ports.
 *  @returns SOC_E_NONE
 */
static int
soc_tomahawk3_idb_init_idb_to_dev_all_pipe(
    int unit,
    int pipe)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int local_dev_port, idb_port;

   for (local_dev_port = 0; local_dev_port < _TH3_DEV_PORTS_PER_PIPE; local_dev_port++) {
        if (pipe == 0){
            idb_port = (local_dev_port == 0) ? 19 : (local_dev_port - 1);
        } else {
            idb_port = (local_dev_port == 18) ? 19 :
                       ((local_dev_port == 19) ? 18 : local_dev_port);
        }

        sal_memset(entry, 0, sizeof(entry));
        mem = idb_to_dev_pmap_tbl[pipe];
        memfld = local_dev_port;
        soc_mem_field_set(unit, mem, entry, DEVICE_PORTf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      idb_port, entry));
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_idb_wr_obm_shared_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM shared config register.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_shared_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    int sop_discard_mode;
    uint32 rval;
    int phy_port;
    int pipe_num;
    int pm_num;
    int num_lanes;
    int lossless;
    const soc_reg_t obm_shared_cfg[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_SHARED_CONFIG_PIPE0r, IDB_OBM1_SHARED_CONFIG_PIPE0r,
         IDB_OBM2_SHARED_CONFIG_PIPE0r, IDB_OBM3_SHARED_CONFIG_PIPE0r},
        {IDB_OBM0_SHARED_CONFIG_PIPE1r, IDB_OBM1_SHARED_CONFIG_PIPE1r,
         IDB_OBM2_SHARED_CONFIG_PIPE1r, IDB_OBM3_SHARED_CONFIG_PIPE1r},
        {IDB_OBM0_SHARED_CONFIG_PIPE2r, IDB_OBM1_SHARED_CONFIG_PIPE2r,
         IDB_OBM2_SHARED_CONFIG_PIPE2r, IDB_OBM3_SHARED_CONFIG_PIPE2r},
        {IDB_OBM0_SHARED_CONFIG_PIPE3r, IDB_OBM1_SHARED_CONFIG_PIPE3r,
         IDB_OBM2_SHARED_CONFIG_PIPE3r, IDB_OBM3_SHARED_CONFIG_PIPE3r},
        {IDB_OBM0_SHARED_CONFIG_PIPE4r, IDB_OBM1_SHARED_CONFIG_PIPE4r,
         IDB_OBM2_SHARED_CONFIG_PIPE4r, IDB_OBM3_SHARED_CONFIG_PIPE4r},
        {IDB_OBM0_SHARED_CONFIG_PIPE5r, IDB_OBM1_SHARED_CONFIG_PIPE5r,
         IDB_OBM2_SHARED_CONFIG_PIPE5r, IDB_OBM3_SHARED_CONFIG_PIPE5r},
        {IDB_OBM0_SHARED_CONFIG_PIPE6r, IDB_OBM1_SHARED_CONFIG_PIPE6r,
         IDB_OBM2_SHARED_CONFIG_PIPE6r, IDB_OBM3_SHARED_CONFIG_PIPE6r},
        {IDB_OBM0_SHARED_CONFIG_PIPE7r, IDB_OBM1_SHARED_CONFIG_PIPE7r,
         IDB_OBM2_SHARED_CONFIG_PIPE7r, IDB_OBM3_SHARED_CONFIG_PIPE7r}
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    num_lanes = port_schedule_state->out_port_map.port_num_lanes[lport];
    lossless = port_schedule_state->lossless;
    reg = obm_shared_cfg[pipe_num][pm_num];
    sop_discard_mode =
        soc_tomahawk3_obm_settings[lossless][num_lanes].sop_discard_mode;
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, SOP_DISCARD_MODEf, sop_discard_mode);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_obm_tdm(int unit,
 *          soc_port_map_type_t *port_map, int lport)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @brief Helper function to Configure OBM TDM slots for teh port going up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_obm_tdm_cfg(
    int unit, soc_port_map_type_t *port_map, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int phy_port_base;
    int pm_num;
    int num_lanes;
    int pipe_num;
    int curr_lport;
    int lane;
    int i;
    static const soc_reg_t obm_tdm_regs[_TH3_PIPES_PER_DEV]
       [_TH3_PBLKS_PER_PIPE] =
       {{IDB_OBM0_TDM_PIPE0r, IDB_OBM1_TDM_PIPE0r,
         IDB_OBM2_TDM_PIPE0r, IDB_OBM3_TDM_PIPE0r},
        {IDB_OBM0_TDM_PIPE1r, IDB_OBM1_TDM_PIPE1r,
         IDB_OBM2_TDM_PIPE1r, IDB_OBM3_TDM_PIPE1r},
        {IDB_OBM0_TDM_PIPE2r, IDB_OBM1_TDM_PIPE2r,
         IDB_OBM2_TDM_PIPE2r, IDB_OBM3_TDM_PIPE2r},
        {IDB_OBM0_TDM_PIPE3r, IDB_OBM1_TDM_PIPE3r,
         IDB_OBM2_TDM_PIPE3r, IDB_OBM3_TDM_PIPE3r},
        {IDB_OBM0_TDM_PIPE4r, IDB_OBM1_TDM_PIPE4r,
         IDB_OBM2_TDM_PIPE4r, IDB_OBM3_TDM_PIPE4r},
        {IDB_OBM0_TDM_PIPE5r, IDB_OBM1_TDM_PIPE5r,
         IDB_OBM2_TDM_PIPE5r, IDB_OBM3_TDM_PIPE5r},
        {IDB_OBM0_TDM_PIPE6r, IDB_OBM1_TDM_PIPE6r,
         IDB_OBM2_TDM_PIPE6r, IDB_OBM3_TDM_PIPE6r},
        {IDB_OBM0_TDM_PIPE7r, IDB_OBM1_TDM_PIPE7r,
         IDB_OBM2_TDM_PIPE7r, IDB_OBM3_TDM_PIPE7r},
    };
    static const int tdm_index[_TH3_PORTS_PER_PBLK] = {
        0, 4, 2, 6, 1, 5, 3, 7
    };
    int lane_to_port_map[_TH3_PORTS_PER_PBLK];

    /* Set the lane_to_port_map based on num_lanes */
    for (lane = 0; lane < _TH3_PORTS_PER_PBLK; lane++) {
        lane_to_port_map[lane] = lane;
    }
    phy_port = port_map->port_l2p_mapping[lport];
    phy_port_base = (((phy_port - 1) >> 3) << 3) + 1;
    for (lane = 0; lane < _TH3_PORTS_PER_PBLK; lane++) {
        curr_lport = port_map->port_p2l_mapping[phy_port_base + lane];
        if (!((curr_lport == -1) || (port_map->log_port_speed[curr_lport] == 0))) {
            num_lanes = port_map->port_num_lanes[curr_lport];
            for (i = lane; i < (lane + num_lanes);i++){
                if (i >= _TH3_PORTS_PER_PBLK) {
                    LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk3_tdm_set_max_spacing()"
                        "Unsupported sched_inst_name\n")));
                    return SOC_E_PARAM;
                } else {
                    lane_to_port_map[i] = lane;
                }
            }
        }
    }
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    reg = obm_tdm_regs[pipe_num][pm_num];
    rval32 = 0;
    soc_reg_field_set(unit, reg, &rval32, SLOT0_PORT_NUMf,lane_to_port_map[tdm_index[0]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT1_PORT_NUMf,lane_to_port_map[tdm_index[1]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT2_PORT_NUMf,lane_to_port_map[tdm_index[2]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT3_PORT_NUMf,lane_to_port_map[tdm_index[3]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT4_PORT_NUMf,lane_to_port_map[tdm_index[4]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT5_PORT_NUMf,lane_to_port_map[tdm_index[5]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT6_PORT_NUMf,lane_to_port_map[tdm_index[6]]);
    soc_reg_field_set(unit, reg, &rval32, SLOT7_PORT_NUMf,lane_to_port_map[tdm_index[7]]);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
                                  reg, REG_PORT_ANY, 0, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_obm_buffer_config(int unit,
 *          soc_port_map_type_t *port_map, int lport)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up
 *  @brief Helper function to Configure CA buffer start & buffer end address.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_obm_buffer_config(
    int unit, soc_port_map_type_t *port_map, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 buffer_start_addr;
    uint32 buffer_end_addr;
    int phy_port;
    int pm_num;
    int subp;
    int num_lanes;
    int pipe_num;
    static const soc_reg_t obm_buffer_config_regs[_TH3_PIPES_PER_DEV]
       [_TH3_PBLKS_PER_PIPE] =
       {{IDB_OBM0_BUFFER_CONFIG_PIPE0r, IDB_OBM1_BUFFER_CONFIG_PIPE0r,
         IDB_OBM2_BUFFER_CONFIG_PIPE0r, IDB_OBM3_BUFFER_CONFIG_PIPE0r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE1r, IDB_OBM1_BUFFER_CONFIG_PIPE1r,
         IDB_OBM2_BUFFER_CONFIG_PIPE1r, IDB_OBM3_BUFFER_CONFIG_PIPE1r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE2r, IDB_OBM1_BUFFER_CONFIG_PIPE2r,
         IDB_OBM2_BUFFER_CONFIG_PIPE2r, IDB_OBM3_BUFFER_CONFIG_PIPE2r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE3r, IDB_OBM1_BUFFER_CONFIG_PIPE3r,
         IDB_OBM2_BUFFER_CONFIG_PIPE3r, IDB_OBM3_BUFFER_CONFIG_PIPE3r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE4r, IDB_OBM1_BUFFER_CONFIG_PIPE4r,
         IDB_OBM2_BUFFER_CONFIG_PIPE4r, IDB_OBM3_BUFFER_CONFIG_PIPE4r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE5r, IDB_OBM1_BUFFER_CONFIG_PIPE5r,
         IDB_OBM2_BUFFER_CONFIG_PIPE5r, IDB_OBM3_BUFFER_CONFIG_PIPE5r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE6r, IDB_OBM1_BUFFER_CONFIG_PIPE6r,
         IDB_OBM2_BUFFER_CONFIG_PIPE6r, IDB_OBM3_BUFFER_CONFIG_PIPE6r},
        {IDB_OBM0_BUFFER_CONFIG_PIPE7r, IDB_OBM1_BUFFER_CONFIG_PIPE7r,
         IDB_OBM2_BUFFER_CONFIG_PIPE7r, IDB_OBM3_BUFFER_CONFIG_PIPE7r},
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    num_lanes = port_map->port_num_lanes[lport];
    buffer_start_addr =
            soc_tomahawk3_obm_buffer_config_settings[num_lanes]
            [subp].buffer_start;
    buffer_end_addr =
            soc_tomahawk3_obm_buffer_config_settings[num_lanes]
            [subp].buffer_end;
    reg = obm_buffer_config_regs[pipe_num][pm_num];
    rval32 = 0;
    soc_reg_field_set(unit, reg, &rval32, BUFFER_STARTf,
                                           buffer_start_addr);
    soc_reg_field_set(unit, reg, &rval32, BUFFER_ENDf,
                                           buffer_end_addr);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
                                  reg, REG_PORT_ANY, subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_thresh(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM thresholds when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_thresh(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    int i;
    uint32 lossless0_discard, lossy_discard, lossy_low_pri,
           discard_limit, lossless1_discard;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int num_lanes;
    int pipe_num;
    int lossless;
    static const soc_reg_t obm_thresh_pp[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE][2] =
    {
        {{IDB_OBM0_THRESHOLD_PIPE0r, IDB_OBM0_THRESHOLD_1_PIPE0r},
         {IDB_OBM1_THRESHOLD_PIPE0r, IDB_OBM1_THRESHOLD_1_PIPE0r},
         {IDB_OBM2_THRESHOLD_PIPE0r, IDB_OBM2_THRESHOLD_1_PIPE0r},
         {IDB_OBM3_THRESHOLD_PIPE0r, IDB_OBM3_THRESHOLD_1_PIPE0r}},
        {{IDB_OBM0_THRESHOLD_PIPE1r, IDB_OBM0_THRESHOLD_1_PIPE1r},
         {IDB_OBM1_THRESHOLD_PIPE1r, IDB_OBM1_THRESHOLD_1_PIPE1r},
         {IDB_OBM2_THRESHOLD_PIPE1r, IDB_OBM2_THRESHOLD_1_PIPE1r},
         {IDB_OBM3_THRESHOLD_PIPE1r, IDB_OBM3_THRESHOLD_1_PIPE1r}},
        {{IDB_OBM0_THRESHOLD_PIPE2r, IDB_OBM0_THRESHOLD_1_PIPE2r},
         {IDB_OBM1_THRESHOLD_PIPE2r, IDB_OBM1_THRESHOLD_1_PIPE2r},
         {IDB_OBM2_THRESHOLD_PIPE2r, IDB_OBM2_THRESHOLD_1_PIPE2r},
         {IDB_OBM3_THRESHOLD_PIPE2r, IDB_OBM3_THRESHOLD_1_PIPE2r}},
        {{IDB_OBM0_THRESHOLD_PIPE3r, IDB_OBM0_THRESHOLD_1_PIPE3r},
         {IDB_OBM1_THRESHOLD_PIPE3r, IDB_OBM1_THRESHOLD_1_PIPE3r},
         {IDB_OBM2_THRESHOLD_PIPE3r, IDB_OBM2_THRESHOLD_1_PIPE3r},
         {IDB_OBM3_THRESHOLD_PIPE3r, IDB_OBM3_THRESHOLD_1_PIPE3r}},
        {{IDB_OBM0_THRESHOLD_PIPE4r, IDB_OBM0_THRESHOLD_1_PIPE4r},
         {IDB_OBM1_THRESHOLD_PIPE4r, IDB_OBM1_THRESHOLD_1_PIPE4r},
         {IDB_OBM2_THRESHOLD_PIPE4r, IDB_OBM2_THRESHOLD_1_PIPE4r},
         {IDB_OBM3_THRESHOLD_PIPE4r, IDB_OBM3_THRESHOLD_1_PIPE4r}},
        {{IDB_OBM0_THRESHOLD_PIPE5r, IDB_OBM0_THRESHOLD_1_PIPE5r},
         {IDB_OBM1_THRESHOLD_PIPE5r, IDB_OBM1_THRESHOLD_1_PIPE5r},
         {IDB_OBM2_THRESHOLD_PIPE5r, IDB_OBM2_THRESHOLD_1_PIPE5r},
         {IDB_OBM3_THRESHOLD_PIPE5r, IDB_OBM3_THRESHOLD_1_PIPE5r}},
        {{IDB_OBM0_THRESHOLD_PIPE6r, IDB_OBM0_THRESHOLD_1_PIPE6r},
         {IDB_OBM1_THRESHOLD_PIPE6r, IDB_OBM1_THRESHOLD_1_PIPE6r},
         {IDB_OBM2_THRESHOLD_PIPE6r, IDB_OBM2_THRESHOLD_1_PIPE6r},
         {IDB_OBM3_THRESHOLD_PIPE6r, IDB_OBM3_THRESHOLD_1_PIPE6r}},
        {{IDB_OBM0_THRESHOLD_PIPE7r, IDB_OBM0_THRESHOLD_1_PIPE7r},
         {IDB_OBM1_THRESHOLD_PIPE7r, IDB_OBM1_THRESHOLD_1_PIPE7r},
         {IDB_OBM2_THRESHOLD_PIPE7r, IDB_OBM2_THRESHOLD_1_PIPE7r},
         {IDB_OBM3_THRESHOLD_PIPE7r, IDB_OBM3_THRESHOLD_1_PIPE7r}}
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    num_lanes = port_schedule_state->out_port_map.port_num_lanes[lport];
    lossless = port_schedule_state->lossless;
    discard_limit =
        soc_tomahawk3_obm_settings[lossless][num_lanes].discard_limit;
    lossless1_discard =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossless_discard;
    lossless0_discard =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossless_discard;
    lossy_discard =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossy_discard;
    lossy_low_pri =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossy_low_pri;
    for (i = 0; i < 2; i++) {
        reg = obm_thresh_pp[pipe_num][pm_num][i];
        if (i == 0) {
            COMPILER_64_SET(rval64, 0, 0);
            COMPILER_64_SET(fldval64, 0, lossless0_discard);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossy_discard);
            soc_reg64_field_set(unit, reg, &rval64, LOSSY_DISCARDf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossy_low_pri);
            soc_reg64_field_set(unit, reg, &rval64, LOSSY_LOW_PRIf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
        else {
            COMPILER_64_SET(rval64, 0, 0);
            COMPILER_64_SET(fldval64, 0, discard_limit);
            soc_reg64_field_set(unit, reg, &rval64, DISCARD_LIMITf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless1_discard);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_ct_thresh(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM CUT THRU thresholds
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_ct_thresh(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    uint32 ct_threshold;
    static const soc_reg_t obm_ct_thresh[_TH3_PIPES_PER_DEV]
                                        [_TH3_PBLKS_PER_PIPE] =
    {
        {IDB_OBM0_CT_THRESHOLD_PIPE0r, IDB_OBM1_CT_THRESHOLD_PIPE0r,
         IDB_OBM2_CT_THRESHOLD_PIPE0r, IDB_OBM3_CT_THRESHOLD_PIPE0r},
        {IDB_OBM0_CT_THRESHOLD_PIPE1r, IDB_OBM1_CT_THRESHOLD_PIPE1r,
         IDB_OBM2_CT_THRESHOLD_PIPE1r, IDB_OBM3_CT_THRESHOLD_PIPE1r},
        {IDB_OBM0_CT_THRESHOLD_PIPE2r, IDB_OBM1_CT_THRESHOLD_PIPE2r,
         IDB_OBM2_CT_THRESHOLD_PIPE2r, IDB_OBM3_CT_THRESHOLD_PIPE2r},
        {IDB_OBM0_CT_THRESHOLD_PIPE3r, IDB_OBM1_CT_THRESHOLD_PIPE3r,
         IDB_OBM2_CT_THRESHOLD_PIPE3r, IDB_OBM3_CT_THRESHOLD_PIPE3r},
        {IDB_OBM0_CT_THRESHOLD_PIPE4r, IDB_OBM1_CT_THRESHOLD_PIPE4r,
         IDB_OBM2_CT_THRESHOLD_PIPE4r, IDB_OBM3_CT_THRESHOLD_PIPE4r},
        {IDB_OBM0_CT_THRESHOLD_PIPE5r, IDB_OBM1_CT_THRESHOLD_PIPE5r,
         IDB_OBM2_CT_THRESHOLD_PIPE5r, IDB_OBM3_CT_THRESHOLD_PIPE5r},
        {IDB_OBM0_CT_THRESHOLD_PIPE6r, IDB_OBM1_CT_THRESHOLD_PIPE6r,
         IDB_OBM2_CT_THRESHOLD_PIPE6r, IDB_OBM3_CT_THRESHOLD_PIPE6r},
        {IDB_OBM0_CT_THRESHOLD_PIPE7r, IDB_OBM1_CT_THRESHOLD_PIPE7r,
         IDB_OBM2_CT_THRESHOLD_PIPE7r, IDB_OBM3_CT_THRESHOLD_PIPE7r}
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    ct_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].obm_ct_threshold;
    reg = obm_ct_thresh[pipe_num][pm_num];
    COMPILER_64_ZERO(rval64);
    COMPILER_64_SET(fldval64, 0, ct_threshold);
    soc_reg64_field_set(unit, reg, &rval64, CUT_THROUGH_OKf, fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_force_saf_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM force SAF unsatisfied threshold
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_force_saf_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int unsatisfied_threshold;
    static const soc_reg_t obm_force_saf_config_regs[_TH3_PIPES_PER_DEV]
                                                    [_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_DBG_A_PIPE0r, IDB_OBM1_DBG_A_PIPE0r,
         IDB_OBM2_DBG_A_PIPE0r, IDB_OBM3_DBG_A_PIPE0r},
        {IDB_OBM0_DBG_A_PIPE1r, IDB_OBM1_DBG_A_PIPE1r,
         IDB_OBM2_DBG_A_PIPE1r, IDB_OBM3_DBG_A_PIPE1r},
        {IDB_OBM0_DBG_A_PIPE2r, IDB_OBM1_DBG_A_PIPE2r,
         IDB_OBM2_DBG_A_PIPE2r, IDB_OBM3_DBG_A_PIPE2r},
        {IDB_OBM0_DBG_A_PIPE3r, IDB_OBM1_DBG_A_PIPE3r,
         IDB_OBM2_DBG_A_PIPE3r, IDB_OBM3_DBG_A_PIPE3r},
        {IDB_OBM0_DBG_A_PIPE4r, IDB_OBM1_DBG_A_PIPE4r,
         IDB_OBM2_DBG_A_PIPE4r, IDB_OBM3_DBG_A_PIPE4r},
        {IDB_OBM0_DBG_A_PIPE5r, IDB_OBM1_DBG_A_PIPE5r,
         IDB_OBM2_DBG_A_PIPE5r, IDB_OBM3_DBG_A_PIPE5r},
        {IDB_OBM0_DBG_A_PIPE6r, IDB_OBM1_DBG_A_PIPE6r,
         IDB_OBM2_DBG_A_PIPE6r, IDB_OBM3_DBG_A_PIPE6r},
        {IDB_OBM0_DBG_A_PIPE7r, IDB_OBM1_DBG_A_PIPE7r,
         IDB_OBM2_DBG_A_PIPE7r, IDB_OBM3_DBG_A_PIPE7r}};

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    unsatisfied_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        obm_unsatisfied_threshold;
    reg = obm_force_saf_config_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      subp, &rval32));
    soc_reg_field_set(unit, reg, &rval32, FIELD_Cf,
                                           unsatisfied_threshold);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_lag_detection_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM lag detection threshold
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_lag_detection_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int enter_threshold;
    int exit_threshold;
    static const soc_reg_t obm_lag_detection_config_regs
        [_TH3_PIPES_PER_DEV]
        [_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_DBG_B_PIPE0r, IDB_OBM1_DBG_B_PIPE0r,
         IDB_OBM2_DBG_B_PIPE0r, IDB_OBM3_DBG_B_PIPE0r},
        {IDB_OBM0_DBG_B_PIPE1r, IDB_OBM1_DBG_B_PIPE1r,
         IDB_OBM2_DBG_B_PIPE1r, IDB_OBM3_DBG_B_PIPE1r},
        {IDB_OBM0_DBG_B_PIPE2r, IDB_OBM1_DBG_B_PIPE2r,
         IDB_OBM2_DBG_B_PIPE2r, IDB_OBM3_DBG_B_PIPE2r},
        {IDB_OBM0_DBG_B_PIPE3r, IDB_OBM1_DBG_B_PIPE3r,
         IDB_OBM2_DBG_B_PIPE3r, IDB_OBM3_DBG_B_PIPE3r},
        {IDB_OBM0_DBG_B_PIPE4r, IDB_OBM1_DBG_B_PIPE4r,
         IDB_OBM2_DBG_B_PIPE4r, IDB_OBM3_DBG_B_PIPE4r},
        {IDB_OBM0_DBG_B_PIPE5r, IDB_OBM1_DBG_B_PIPE5r,
         IDB_OBM2_DBG_B_PIPE5r, IDB_OBM3_DBG_B_PIPE5r},
        {IDB_OBM0_DBG_B_PIPE6r, IDB_OBM1_DBG_B_PIPE6r,
         IDB_OBM2_DBG_B_PIPE6r, IDB_OBM3_DBG_B_PIPE6r},
        {IDB_OBM0_DBG_B_PIPE7r, IDB_OBM1_DBG_B_PIPE7r,
         IDB_OBM2_DBG_B_PIPE7r, IDB_OBM3_DBG_B_PIPE7r}};

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    enter_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        obm_enter_lagging_threshold;
    exit_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        obm_exit_lagging_threshold;
    reg = obm_lag_detection_config_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      subp, &rval32));
    soc_reg_field_set(unit, reg, &rval32, FIELD_Af, enter_threshold);
    soc_reg_field_set(unit, reg, &rval32, FIELD_Bf, exit_threshold);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_ca_force_saf_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config CA force SAF unsatisfied threshold
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_ca_force_saf_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int unsatisfied_threshold;
    static const soc_reg_t ca_force_saf_config_regs[_TH3_PIPES_PER_DEV]
                                                    [_TH3_PBLKS_PER_PIPE] = {
        {IDB_CA0_DBG_A_PIPE0r, IDB_CA1_DBG_A_PIPE0r,
         IDB_CA2_DBG_A_PIPE0r, IDB_CA3_DBG_A_PIPE0r},
        {IDB_CA0_DBG_A_PIPE1r, IDB_CA1_DBG_A_PIPE1r,
         IDB_CA2_DBG_A_PIPE1r, IDB_CA3_DBG_A_PIPE1r},
        {IDB_CA0_DBG_A_PIPE2r, IDB_CA1_DBG_A_PIPE2r,
         IDB_CA2_DBG_A_PIPE2r, IDB_CA3_DBG_A_PIPE2r},
        {IDB_CA0_DBG_A_PIPE3r, IDB_CA1_DBG_A_PIPE3r,
         IDB_CA2_DBG_A_PIPE3r, IDB_CA3_DBG_A_PIPE3r},
        {IDB_CA0_DBG_A_PIPE4r, IDB_CA1_DBG_A_PIPE4r,
         IDB_CA2_DBG_A_PIPE4r, IDB_CA3_DBG_A_PIPE4r},
        {IDB_CA0_DBG_A_PIPE5r, IDB_CA1_DBG_A_PIPE5r,
         IDB_CA2_DBG_A_PIPE5r, IDB_CA3_DBG_A_PIPE5r},
        {IDB_CA0_DBG_A_PIPE6r, IDB_CA1_DBG_A_PIPE6r,
         IDB_CA2_DBG_A_PIPE6r, IDB_CA3_DBG_A_PIPE6r},
        {IDB_CA0_DBG_A_PIPE7r, IDB_CA1_DBG_A_PIPE7r,
         IDB_CA2_DBG_A_PIPE7r, IDB_CA3_DBG_A_PIPE7r}};

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    unsatisfied_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        ca_unsatisfied_threshold;
    reg = ca_force_saf_config_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      subp, &rval32));
    soc_reg_field_set(unit, reg, &rval32, FIELD_Cf,
                                           unsatisfied_threshold);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_ca_lag_detection_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config CA lag detection threshold
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_ca_lag_detection_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int enter_threshold;
    int exit_threshold;
    static const soc_reg_t ca_lag_detection_config_regs
        [_TH3_PIPES_PER_DEV]
        [_TH3_PBLKS_PER_PIPE] = {
        {IDB_CA0_DBG_B_PIPE0r, IDB_CA1_DBG_B_PIPE0r,
         IDB_CA2_DBG_B_PIPE0r, IDB_CA3_DBG_B_PIPE0r},
        {IDB_CA0_DBG_B_PIPE1r, IDB_CA1_DBG_B_PIPE1r,
         IDB_CA2_DBG_B_PIPE1r, IDB_CA3_DBG_B_PIPE1r},
        {IDB_CA0_DBG_B_PIPE2r, IDB_CA1_DBG_B_PIPE2r,
         IDB_CA2_DBG_B_PIPE2r, IDB_CA3_DBG_B_PIPE2r},
        {IDB_CA0_DBG_B_PIPE3r, IDB_CA1_DBG_B_PIPE3r,
         IDB_CA2_DBG_B_PIPE3r, IDB_CA3_DBG_B_PIPE3r},
        {IDB_CA0_DBG_B_PIPE4r, IDB_CA1_DBG_B_PIPE4r,
         IDB_CA2_DBG_B_PIPE4r, IDB_CA3_DBG_B_PIPE4r},
        {IDB_CA0_DBG_B_PIPE5r, IDB_CA1_DBG_B_PIPE5r,
         IDB_CA2_DBG_B_PIPE5r, IDB_CA3_DBG_B_PIPE5r},
        {IDB_CA0_DBG_B_PIPE6r, IDB_CA1_DBG_B_PIPE6r,
         IDB_CA2_DBG_B_PIPE6r, IDB_CA3_DBG_B_PIPE6r},
        {IDB_CA0_DBG_B_PIPE7r, IDB_CA1_DBG_B_PIPE7r,
         IDB_CA2_DBG_B_PIPE7r, IDB_CA3_DBG_B_PIPE7r}};

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    enter_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        ca_enter_lagging_threshold;
    exit_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        ca_exit_lagging_threshold;
    reg = ca_lag_detection_config_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      subp, &rval32));
    soc_reg_field_set(unit, reg, &rval32, FIELD_Af, enter_threshold);
    soc_reg_field_set(unit, reg, &rval32, FIELD_Bf, exit_threshold);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_ca_ct_thresh(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config CA CUT THRU thresholds
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_ca_ct_thresh(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    uint32 ct_threshold;
    int speed_encoding;
    static const soc_reg_t ca_ct_thresh[_TH3_PIPES_PER_DEV]
                                       [_TH3_PBLKS_PER_PIPE] =
    {
       {IDB_CA0_CT_CONTROL_PIPE0r,IDB_CA1_CT_CONTROL_PIPE0r,
        IDB_CA2_CT_CONTROL_PIPE0r,IDB_CA3_CT_CONTROL_PIPE0r},
       {IDB_CA0_CT_CONTROL_PIPE1r,IDB_CA1_CT_CONTROL_PIPE1r,
        IDB_CA2_CT_CONTROL_PIPE1r,IDB_CA3_CT_CONTROL_PIPE1r},
       {IDB_CA0_CT_CONTROL_PIPE2r,IDB_CA1_CT_CONTROL_PIPE2r,
        IDB_CA2_CT_CONTROL_PIPE2r,IDB_CA3_CT_CONTROL_PIPE2r},
       {IDB_CA0_CT_CONTROL_PIPE3r,IDB_CA1_CT_CONTROL_PIPE3r,
        IDB_CA2_CT_CONTROL_PIPE3r,IDB_CA3_CT_CONTROL_PIPE3r},
       {IDB_CA0_CT_CONTROL_PIPE4r,IDB_CA1_CT_CONTROL_PIPE4r,
        IDB_CA2_CT_CONTROL_PIPE4r,IDB_CA3_CT_CONTROL_PIPE4r},
       {IDB_CA0_CT_CONTROL_PIPE5r,IDB_CA1_CT_CONTROL_PIPE5r,
        IDB_CA2_CT_CONTROL_PIPE5r,IDB_CA3_CT_CONTROL_PIPE5r},
       {IDB_CA0_CT_CONTROL_PIPE6r,IDB_CA1_CT_CONTROL_PIPE6r,
        IDB_CA2_CT_CONTROL_PIPE6r,IDB_CA3_CT_CONTROL_PIPE6r},
       {IDB_CA0_CT_CONTROL_PIPE7r,IDB_CA1_CT_CONTROL_PIPE7r,
        IDB_CA2_CT_CONTROL_PIPE7r,IDB_CA3_CT_CONTROL_PIPE7r}
    };
    static const soc_field_t ca_ct_threshold_fields[] = {
        PORT0_CT_THRESHOLDf, PORT1_CT_THRESHOLDf,
        PORT2_CT_THRESHOLDf, PORT3_CT_THRESHOLDf,
        PORT4_CT_THRESHOLDf, PORT5_CT_THRESHOLDf,
        PORT6_CT_THRESHOLDf, PORT7_CT_THRESHOLDf
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    ct_threshold =
        soc_tomahawk3_e2e_settings_tbl[speed_encoding].ca_ct_threshold;
    reg = ca_ct_thresh[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                            0, &rval64));
    COMPILER_64_SET(fldval64, 0, ct_threshold);
    soc_reg64_field_set(unit, reg, &rval64, ca_ct_threshold_fields[subp],
                                                          fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    0, rval64));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_port_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM Per port config register
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_port_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    static const soc_reg_t obm_port_config[_TH3_PIPES_PER_DEV]
                                          [_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_PORT_CONFIG_PIPE0r, IDB_OBM1_PORT_CONFIG_PIPE0r,
         IDB_OBM2_PORT_CONFIG_PIPE0r, IDB_OBM3_PORT_CONFIG_PIPE0r},
        {IDB_OBM0_PORT_CONFIG_PIPE1r, IDB_OBM1_PORT_CONFIG_PIPE1r,
         IDB_OBM2_PORT_CONFIG_PIPE1r, IDB_OBM3_PORT_CONFIG_PIPE1r},
        {IDB_OBM0_PORT_CONFIG_PIPE2r, IDB_OBM1_PORT_CONFIG_PIPE2r,
         IDB_OBM2_PORT_CONFIG_PIPE2r, IDB_OBM3_PORT_CONFIG_PIPE2r},
        {IDB_OBM0_PORT_CONFIG_PIPE3r, IDB_OBM1_PORT_CONFIG_PIPE3r,
         IDB_OBM2_PORT_CONFIG_PIPE3r, IDB_OBM3_PORT_CONFIG_PIPE3r},
        {IDB_OBM0_PORT_CONFIG_PIPE4r, IDB_OBM1_PORT_CONFIG_PIPE4r,
         IDB_OBM2_PORT_CONFIG_PIPE4r, IDB_OBM3_PORT_CONFIG_PIPE4r},
        {IDB_OBM0_PORT_CONFIG_PIPE5r, IDB_OBM1_PORT_CONFIG_PIPE5r,
         IDB_OBM2_PORT_CONFIG_PIPE5r, IDB_OBM3_PORT_CONFIG_PIPE5r},
        {IDB_OBM0_PORT_CONFIG_PIPE6r, IDB_OBM1_PORT_CONFIG_PIPE6r,
         IDB_OBM2_PORT_CONFIG_PIPE6r, IDB_OBM3_PORT_CONFIG_PIPE6r},
        {IDB_OBM0_PORT_CONFIG_PIPE7r, IDB_OBM1_PORT_CONFIG_PIPE7r,
         IDB_OBM2_PORT_CONFIG_PIPE7r, IDB_OBM3_PORT_CONFIG_PIPE7r}};

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    reg = obm_port_config[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                    subp, &rval32));
    soc_reg_field_set(unit, reg, &rval32, OUTER_TPID_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval32));
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_idb_wr_obm_monitor_stats_config(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM Per port Instrumentation Monitor
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_monitor_stats_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int lossless;
    int drop_count_select;
    static const soc_reg_t obm_monitor_stats_config[_TH3_PIPES_PER_DEV]
                                                   [_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE0r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE0r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE0r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE0r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE1r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE1r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE1r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE1r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE2r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE2r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE2r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE2r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE3r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE3r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE3r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE3r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE4r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE4r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE4r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE4r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE5r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE5r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE5r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE5r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE6r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE6r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE6r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE6r},
        {IDB_OBM0_MONITOR_STATS_CONFIG_PIPE7r,
         IDB_OBM1_MONITOR_STATS_CONFIG_PIPE7r,
         IDB_OBM2_MONITOR_STATS_CONFIG_PIPE7r,
         IDB_OBM3_MONITOR_STATS_CONFIG_PIPE7r}};

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    lossless = port_schedule_state->lossless;
    if (lossless == _TH3_OBM_PARAMS_LOSSLESS_INDEX){
        drop_count_select = _TH3_OBM_PARAMS_DROP_COUNT_SELECT_LOSSLESS0;
    } else {
        drop_count_select = _TH3_OBM_PARAMS_DROP_COUNT_SELECT_LOSSY_LO;
    }
    reg = obm_monitor_stats_config[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                    subp, &rval32));
    soc_reg_field_set(unit, reg, &rval32, DROP_COUNT_SELECTf, drop_count_select);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_ct_enable(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport,
            int ct_enable)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config CA CUT THRU disable
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_ct_enable(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport, int ct_enable)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int ct_disable;
    static const soc_field_t ca_ct_disable_fields[] = {
        PORT0_CT_DISABLEf, PORT1_CT_DISABLEf,
        PORT2_CT_DISABLEf, PORT3_CT_DISABLEf,
        PORT4_CT_DISABLEf, PORT5_CT_DISABLEf,
        PORT6_CT_DISABLEf, PORT7_CT_DISABLEf
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    reg = th3_idb_ca_control_2[pipe_num];
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                            pm_num, &rval64));
    ct_disable = ct_enable ? 0: 1;
    COMPILER_64_SET(fldval64, 0, ct_disable);
    soc_reg64_field_set(unit, reg, &rval64, ca_ct_disable_fields[subp],
                                                          fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    pm_num, rval64));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_peek_depth_cfg(int unit,
            soc_port_map_type_t *port_map, int lport,
            int ct_enable)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config CA peek_depth
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_peek_depth_cfg(int unit,
    soc_port_map_type_t *port_map, int lport)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int peek_depth;
    int speed_encoding;
    static const soc_field_t ca_peek_depth_fields[] = {
        PORT0_CA_PEEK_DEPTHf, PORT1_CA_PEEK_DEPTHf,
        PORT2_CA_PEEK_DEPTHf, PORT3_CA_PEEK_DEPTHf,
        PORT4_CA_PEEK_DEPTHf, PORT5_CA_PEEK_DEPTHf,
        PORT6_CA_PEEK_DEPTHf, PORT7_CA_PEEK_DEPTHf
    };

    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    reg = th3_idb_ca_control_2[pipe_num];
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                            pm_num, &rval64));
    speed_encoding = soc_tomahawk3_speed_to_e2e_settings_class_map(
                     port_map->log_port_speed[lport]);
    peek_depth = soc_tomahawk3_peek_depth_settings_tbl[speed_encoding].
                     peek_depth;
    COMPILER_64_SET(fldval64, 0, peek_depth);
    soc_reg64_field_set(unit, reg, &rval64, ca_peek_depth_fields[subp],
                                                          fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    pm_num, rval64));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_lpbk_ca_peek_depth_cfg(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to configure loopback port CA peek depth while 
           the ports are comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_lpbk_ca_peek_depth_cfg(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;

    reg = th3_idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PEEK_DEPTHf, _TH3_IDB_CA_PEEK_DEPTH_CPU_LPBK_PORT);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_cpu_ca_peek_depth_cfg(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to configure cpu port CA peek depth while 
           the ports are comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_cpu_ca_peek_depth_cfg(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;

    reg = th3_idb_ca_cpu_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PEEK_DEPTHf, _TH3_IDB_CA_PEEK_DEPTH_CPU_LPBK_PORT);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_bmop_set(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config CA Bubble MOP settings
           when port comming up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_bmop_set(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    uint32 bmop_disable;
    int latency_mode;
    int speed_encoding;
    static const soc_field_t ca_bmop_disable_fields[] = {
        PORT0_BUBBLE_MOP_DISABLEf, PORT1_BUBBLE_MOP_DISABLEf,
        PORT2_BUBBLE_MOP_DISABLEf, PORT3_BUBBLE_MOP_DISABLEf,
        PORT4_BUBBLE_MOP_DISABLEf, PORT5_BUBBLE_MOP_DISABLEf,
        PORT6_BUBBLE_MOP_DISABLEf, PORT7_BUBBLE_MOP_DISABLEf
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    latency_mode = port_schedule_state->cutthru_prop.switch_bypass_mode;
    speed_encoding =
        (port_schedule_state->cutthru_prop.asf_modes[lport]
        == _SOC_ASF_MODE_SAF) ? 0 :
        soc_tomahawk3_speed_to_e2e_settings_class_map(
        port_schedule_state->out_port_map.log_port_speed[lport]);
    bmop_disable = soc_tomahawk3_e2e_settings_tbl[speed_encoding].
        bmop_disable[latency_mode];
    reg = th3_idb_ca_control_2[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                            pm_num, &rval64));
    COMPILER_64_SET(fldval64, 0, bmop_disable);
    soc_reg64_field_set(unit, reg, &rval64, ca_bmop_disable_fields[subp],
                                                          fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    pm_num, rval64));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_flow_ctrl_cfg(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM flow control config regsiter
           during port going up.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_flow_ctrl_cfg(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int lossless;
    const soc_reg_t obm_flow_ctrl_cfg[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE]
        = {
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE4r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE4r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE4r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE4r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE5r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE5r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE5r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE5r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE6r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE6r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE6r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE6r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE7r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE7r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE7r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE7r}
        };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    lossless = port_schedule_state->lossless;
    reg = obm_flow_ctrl_cfg[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, subp,
                                            &rval64));
    if (lossless == _TH3_OBM_PARAMS_LOSSLESS_INDEX) {
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, FC_TYPEf, fldval64);
        COMPILER_64_SET(fldval64, 0, 1);
        soc_reg64_field_set(unit, reg, &rval64, PORT_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, fldval64);
        COMPILER_64_SET(fldval64, 0, 0xffff);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_PRIORITY_PROFILEf, fldval64);
    } else if (lossless == _TH3_OBM_PARAMS_LOSSY_INDEX) {
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, FC_TYPEf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, PORT_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_PRIORITY_PROFILEf, fldval64);
    } else { /* _TH3_OBM_PARAMS_LOSSY_PLUS_LOSSLESS_INDEX */
        COMPILER_64_SET(fldval64, 0, 1);
        soc_reg64_field_set(unit, reg, &rval64, FC_TYPEf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, PORT_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, fldval64);
        COMPILER_64_SET(fldval64, 0, 1);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_PRIORITY_PROFILEf, fldval64);
    }
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY, subp,
                                            rval64));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_wr_obm_fc_threshold(int unit,
            soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to config OBM flow-control thresholds.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_wr_obm_fc_threshold(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_reg_t reg;
    int i;
    uint32 lossless0_xon, lossless0_xoff, lossless1_xon, lossless1_xoff,
           port_xon, port_xoff;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int num_lanes;
    int lossless;
    int pipe_num;
    const soc_reg_t obm_fc_thresh_pp[_TH3_PIPES_PER_DEV]
        [_TH3_PBLKS_PER_PIPE][2] = {
        {{IDB_OBM0_FC_THRESHOLD_PIPE0r, IDB_OBM0_FC_THRESHOLD_1_PIPE0r},
         {IDB_OBM1_FC_THRESHOLD_PIPE0r, IDB_OBM1_FC_THRESHOLD_1_PIPE0r},
         {IDB_OBM2_FC_THRESHOLD_PIPE0r, IDB_OBM2_FC_THRESHOLD_1_PIPE0r},
         {IDB_OBM3_FC_THRESHOLD_PIPE0r, IDB_OBM3_FC_THRESHOLD_1_PIPE0r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE1r, IDB_OBM0_FC_THRESHOLD_1_PIPE1r},
         {IDB_OBM1_FC_THRESHOLD_PIPE1r, IDB_OBM1_FC_THRESHOLD_1_PIPE1r},
         {IDB_OBM2_FC_THRESHOLD_PIPE1r, IDB_OBM2_FC_THRESHOLD_1_PIPE1r},
         {IDB_OBM3_FC_THRESHOLD_PIPE1r, IDB_OBM3_FC_THRESHOLD_1_PIPE1r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE2r, IDB_OBM0_FC_THRESHOLD_1_PIPE2r},
         {IDB_OBM1_FC_THRESHOLD_PIPE2r, IDB_OBM1_FC_THRESHOLD_1_PIPE2r},
         {IDB_OBM2_FC_THRESHOLD_PIPE2r, IDB_OBM2_FC_THRESHOLD_1_PIPE2r},
         {IDB_OBM3_FC_THRESHOLD_PIPE2r, IDB_OBM3_FC_THRESHOLD_1_PIPE2r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE3r, IDB_OBM0_FC_THRESHOLD_1_PIPE3r},
         {IDB_OBM1_FC_THRESHOLD_PIPE3r, IDB_OBM1_FC_THRESHOLD_1_PIPE3r},
         {IDB_OBM2_FC_THRESHOLD_PIPE3r, IDB_OBM2_FC_THRESHOLD_1_PIPE3r},
         {IDB_OBM3_FC_THRESHOLD_PIPE3r, IDB_OBM3_FC_THRESHOLD_1_PIPE3r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE4r, IDB_OBM0_FC_THRESHOLD_1_PIPE4r},
         {IDB_OBM1_FC_THRESHOLD_PIPE4r, IDB_OBM1_FC_THRESHOLD_1_PIPE4r},
         {IDB_OBM2_FC_THRESHOLD_PIPE4r, IDB_OBM2_FC_THRESHOLD_1_PIPE4r},
         {IDB_OBM3_FC_THRESHOLD_PIPE4r, IDB_OBM3_FC_THRESHOLD_1_PIPE4r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE5r, IDB_OBM0_FC_THRESHOLD_1_PIPE5r},
         {IDB_OBM1_FC_THRESHOLD_PIPE5r, IDB_OBM1_FC_THRESHOLD_1_PIPE5r},
         {IDB_OBM2_FC_THRESHOLD_PIPE5r, IDB_OBM2_FC_THRESHOLD_1_PIPE5r},
         {IDB_OBM3_FC_THRESHOLD_PIPE5r, IDB_OBM3_FC_THRESHOLD_1_PIPE5r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE6r, IDB_OBM0_FC_THRESHOLD_1_PIPE6r},
         {IDB_OBM1_FC_THRESHOLD_PIPE6r, IDB_OBM1_FC_THRESHOLD_1_PIPE6r},
         {IDB_OBM2_FC_THRESHOLD_PIPE6r, IDB_OBM2_FC_THRESHOLD_1_PIPE6r},
         {IDB_OBM3_FC_THRESHOLD_PIPE6r, IDB_OBM3_FC_THRESHOLD_1_PIPE6r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE7r, IDB_OBM0_FC_THRESHOLD_1_PIPE7r},
         {IDB_OBM1_FC_THRESHOLD_PIPE7r, IDB_OBM1_FC_THRESHOLD_1_PIPE7r},
         {IDB_OBM2_FC_THRESHOLD_PIPE7r, IDB_OBM2_FC_THRESHOLD_1_PIPE7r},
         {IDB_OBM3_FC_THRESHOLD_PIPE7r, IDB_OBM3_FC_THRESHOLD_1_PIPE7r}}
    };

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    num_lanes = port_schedule_state->out_port_map.port_num_lanes[lport];
    lossless = port_schedule_state->lossless;
    lossless0_xon =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossless_xon;
    lossless1_xon =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossless_xon;
    lossless0_xoff =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossless_xoff;
    lossless1_xoff =
        soc_tomahawk3_obm_settings[lossless][num_lanes].lossless_xoff;
    port_xon =
        soc_tomahawk3_obm_settings[lossless][num_lanes].port_xon;
    port_xoff =
        soc_tomahawk3_obm_settings[lossless][num_lanes].port_xoff;
    COMPILER_64_ZERO(rval64);
    for (i = 0; i < 2; i++) {
        reg = obm_fc_thresh_pp[pipe_num][pm_num][i];
        if (i == 0) {
            COMPILER_64_SET(fldval64, 0, lossless0_xon);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless0_xoff);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless1_xon);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless1_xoff);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
        else {
            COMPILER_64_SET(fldval64, 0, port_xon);
            soc_reg64_field_set(unit, reg, &rval64, PORT_XONf, fldval64);
            COMPILER_64_SET(fldval64, 0, port_xoff);
            soc_reg64_field_set(unit, reg, &rval64, PORT_XOFFf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_obm_pri_map_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int lport)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @param lport Logical port number for the port going up.
 *  @brief Helper function to configure IDB_OBM0_DSCP_MAP table as per the
           lossy/lossless configuration.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_obm_pri_map_set(
    int unit, soc_port_schedule_state_t *port_schedule_state, int lport)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int j;
    int count;
    int lossless;
    uint32 priority;
    static const soc_mem_t obm_pri_map_mem[_TH3_PBLKS_PER_PIPE]
        [_TH3_PIPES_PER_DEV][_TH3_PORTS_PER_PBLK] = {
        {{IDB_OBM0_PRI_MAP_PORT0_PIPE0m, IDB_OBM0_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE0m, IDB_OBM0_PRI_MAP_PORT3_PIPE0m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE0m, IDB_OBM0_PRI_MAP_PORT5_PIPE0m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE0m, IDB_OBM0_PRI_MAP_PORT7_PIPE0m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE1m, IDB_OBM0_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE1m, IDB_OBM0_PRI_MAP_PORT3_PIPE1m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE1m, IDB_OBM0_PRI_MAP_PORT5_PIPE1m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE1m, IDB_OBM0_PRI_MAP_PORT7_PIPE1m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE2m, IDB_OBM0_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE2m, IDB_OBM0_PRI_MAP_PORT3_PIPE2m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE2m, IDB_OBM0_PRI_MAP_PORT5_PIPE2m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE2m, IDB_OBM0_PRI_MAP_PORT7_PIPE2m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE3m, IDB_OBM0_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE3m, IDB_OBM0_PRI_MAP_PORT3_PIPE3m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE3m, IDB_OBM0_PRI_MAP_PORT5_PIPE3m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE3m, IDB_OBM0_PRI_MAP_PORT7_PIPE3m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE4m, IDB_OBM0_PRI_MAP_PORT1_PIPE4m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE4m, IDB_OBM0_PRI_MAP_PORT3_PIPE4m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE4m, IDB_OBM0_PRI_MAP_PORT5_PIPE4m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE4m, IDB_OBM0_PRI_MAP_PORT7_PIPE4m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE5m, IDB_OBM0_PRI_MAP_PORT1_PIPE5m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE5m, IDB_OBM0_PRI_MAP_PORT3_PIPE5m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE5m, IDB_OBM0_PRI_MAP_PORT5_PIPE5m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE5m, IDB_OBM0_PRI_MAP_PORT7_PIPE5m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE6m, IDB_OBM0_PRI_MAP_PORT1_PIPE6m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE6m, IDB_OBM0_PRI_MAP_PORT3_PIPE6m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE6m, IDB_OBM0_PRI_MAP_PORT5_PIPE6m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE6m, IDB_OBM0_PRI_MAP_PORT7_PIPE6m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE7m, IDB_OBM0_PRI_MAP_PORT1_PIPE7m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE7m, IDB_OBM0_PRI_MAP_PORT3_PIPE7m,
          IDB_OBM0_PRI_MAP_PORT4_PIPE7m, IDB_OBM0_PRI_MAP_PORT5_PIPE7m,
          IDB_OBM0_PRI_MAP_PORT6_PIPE7m, IDB_OBM0_PRI_MAP_PORT7_PIPE7m}},
        {{IDB_OBM1_PRI_MAP_PORT0_PIPE0m, IDB_OBM1_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE0m, IDB_OBM1_PRI_MAP_PORT3_PIPE0m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE0m, IDB_OBM1_PRI_MAP_PORT5_PIPE0m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE0m, IDB_OBM1_PRI_MAP_PORT7_PIPE0m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE1m, IDB_OBM1_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE1m, IDB_OBM1_PRI_MAP_PORT3_PIPE1m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE1m, IDB_OBM1_PRI_MAP_PORT5_PIPE1m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE1m, IDB_OBM1_PRI_MAP_PORT7_PIPE1m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE2m, IDB_OBM1_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE2m, IDB_OBM1_PRI_MAP_PORT3_PIPE2m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE2m, IDB_OBM1_PRI_MAP_PORT5_PIPE2m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE2m, IDB_OBM1_PRI_MAP_PORT7_PIPE2m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE3m, IDB_OBM1_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE3m, IDB_OBM1_PRI_MAP_PORT3_PIPE3m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE3m, IDB_OBM1_PRI_MAP_PORT5_PIPE3m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE3m, IDB_OBM1_PRI_MAP_PORT7_PIPE3m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE4m, IDB_OBM1_PRI_MAP_PORT1_PIPE4m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE4m, IDB_OBM1_PRI_MAP_PORT3_PIPE4m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE4m, IDB_OBM1_PRI_MAP_PORT5_PIPE4m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE4m, IDB_OBM1_PRI_MAP_PORT7_PIPE4m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE5m, IDB_OBM1_PRI_MAP_PORT1_PIPE5m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE5m, IDB_OBM1_PRI_MAP_PORT3_PIPE5m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE5m, IDB_OBM1_PRI_MAP_PORT5_PIPE5m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE5m, IDB_OBM1_PRI_MAP_PORT7_PIPE5m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE6m, IDB_OBM1_PRI_MAP_PORT1_PIPE6m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE6m, IDB_OBM1_PRI_MAP_PORT3_PIPE6m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE6m, IDB_OBM1_PRI_MAP_PORT5_PIPE6m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE6m, IDB_OBM1_PRI_MAP_PORT7_PIPE6m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE7m, IDB_OBM1_PRI_MAP_PORT1_PIPE7m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE7m, IDB_OBM1_PRI_MAP_PORT3_PIPE7m,
          IDB_OBM1_PRI_MAP_PORT4_PIPE7m, IDB_OBM1_PRI_MAP_PORT5_PIPE7m,
          IDB_OBM1_PRI_MAP_PORT6_PIPE7m, IDB_OBM1_PRI_MAP_PORT7_PIPE7m}},
        {{IDB_OBM2_PRI_MAP_PORT0_PIPE0m, IDB_OBM2_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE0m, IDB_OBM2_PRI_MAP_PORT3_PIPE0m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE0m, IDB_OBM2_PRI_MAP_PORT5_PIPE0m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE0m, IDB_OBM2_PRI_MAP_PORT7_PIPE0m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE1m, IDB_OBM2_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE1m, IDB_OBM2_PRI_MAP_PORT3_PIPE1m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE1m, IDB_OBM2_PRI_MAP_PORT5_PIPE1m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE1m, IDB_OBM2_PRI_MAP_PORT7_PIPE1m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE2m, IDB_OBM2_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE2m, IDB_OBM2_PRI_MAP_PORT3_PIPE2m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE2m, IDB_OBM2_PRI_MAP_PORT5_PIPE2m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE2m, IDB_OBM2_PRI_MAP_PORT7_PIPE2m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE3m, IDB_OBM2_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE3m, IDB_OBM2_PRI_MAP_PORT3_PIPE3m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE3m, IDB_OBM2_PRI_MAP_PORT5_PIPE3m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE3m, IDB_OBM2_PRI_MAP_PORT7_PIPE3m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE4m, IDB_OBM2_PRI_MAP_PORT1_PIPE4m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE4m, IDB_OBM2_PRI_MAP_PORT3_PIPE4m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE4m, IDB_OBM2_PRI_MAP_PORT5_PIPE4m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE4m, IDB_OBM2_PRI_MAP_PORT7_PIPE4m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE5m, IDB_OBM2_PRI_MAP_PORT1_PIPE5m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE5m, IDB_OBM2_PRI_MAP_PORT3_PIPE5m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE5m, IDB_OBM2_PRI_MAP_PORT5_PIPE5m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE5m, IDB_OBM2_PRI_MAP_PORT7_PIPE5m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE6m, IDB_OBM2_PRI_MAP_PORT1_PIPE6m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE6m, IDB_OBM2_PRI_MAP_PORT3_PIPE6m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE6m, IDB_OBM2_PRI_MAP_PORT5_PIPE6m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE6m, IDB_OBM2_PRI_MAP_PORT7_PIPE6m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE7m, IDB_OBM2_PRI_MAP_PORT1_PIPE7m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE7m, IDB_OBM2_PRI_MAP_PORT3_PIPE7m,
          IDB_OBM2_PRI_MAP_PORT4_PIPE7m, IDB_OBM2_PRI_MAP_PORT5_PIPE7m,
          IDB_OBM2_PRI_MAP_PORT6_PIPE7m, IDB_OBM2_PRI_MAP_PORT7_PIPE7m}},
        {{IDB_OBM3_PRI_MAP_PORT0_PIPE0m, IDB_OBM3_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE0m, IDB_OBM3_PRI_MAP_PORT3_PIPE0m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE0m, IDB_OBM3_PRI_MAP_PORT5_PIPE0m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE0m, IDB_OBM3_PRI_MAP_PORT7_PIPE0m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE1m, IDB_OBM3_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE1m, IDB_OBM3_PRI_MAP_PORT3_PIPE1m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE1m, IDB_OBM3_PRI_MAP_PORT5_PIPE1m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE1m, IDB_OBM3_PRI_MAP_PORT7_PIPE1m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE2m, IDB_OBM3_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE2m, IDB_OBM3_PRI_MAP_PORT3_PIPE2m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE2m, IDB_OBM3_PRI_MAP_PORT5_PIPE2m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE2m, IDB_OBM3_PRI_MAP_PORT7_PIPE2m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE3m, IDB_OBM3_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE3m, IDB_OBM3_PRI_MAP_PORT3_PIPE3m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE3m, IDB_OBM3_PRI_MAP_PORT5_PIPE3m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE3m, IDB_OBM3_PRI_MAP_PORT7_PIPE3m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE4m, IDB_OBM3_PRI_MAP_PORT1_PIPE4m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE4m, IDB_OBM3_PRI_MAP_PORT3_PIPE4m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE4m, IDB_OBM3_PRI_MAP_PORT5_PIPE4m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE4m, IDB_OBM3_PRI_MAP_PORT7_PIPE4m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE5m, IDB_OBM3_PRI_MAP_PORT1_PIPE5m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE5m, IDB_OBM3_PRI_MAP_PORT3_PIPE5m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE5m, IDB_OBM3_PRI_MAP_PORT5_PIPE5m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE5m, IDB_OBM3_PRI_MAP_PORT7_PIPE5m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE6m, IDB_OBM3_PRI_MAP_PORT1_PIPE6m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE6m, IDB_OBM3_PRI_MAP_PORT3_PIPE6m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE6m, IDB_OBM3_PRI_MAP_PORT5_PIPE6m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE6m, IDB_OBM3_PRI_MAP_PORT7_PIPE6m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE7m, IDB_OBM3_PRI_MAP_PORT1_PIPE7m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE7m, IDB_OBM3_PRI_MAP_PORT3_PIPE7m,
          IDB_OBM3_PRI_MAP_PORT4_PIPE7m, IDB_OBM3_PRI_MAP_PORT5_PIPE7m,
          IDB_OBM3_PRI_MAP_PORT6_PIPE7m, IDB_OBM3_PRI_MAP_PORT7_PIPE7m}}
    };
    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    sal_memset(entry, 0, sizeof(entry));
    lossless = port_schedule_state->lossless;
    count = COUNTOF(obm_ob_pri_fields);
    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    mem = obm_pri_map_mem[pm_num][pipe_num][subp];
    for (j = 0; j < count; j++) {
       priority = (lossless == _TH3_OBM_PARAMS_LOSSY_INDEX) ?
                               _TH3_OBM_PRIORITY_LOSSY_LO :
                  (lossless == _TH3_OBM_PARAMS_LOSSLESS_INDEX) ?
                               _TH3_OBM_PRIORITY_LOSSLESS0 :
                  (j == 0) ?   _TH3_OBM_PRIORITY_LOSSLESS0 :
                               _TH3_OBM_PRIORITY_LOSSY_LO;
        soc_mem_field_set(unit, mem, entry, obm_ob_pri_fields[j], &priority);
    }
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_outer_tpid_config(int unit,
                soc_port_map_type_t *port_map, int lport, int enable,
                int tag_num, int tag_value)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param lport Logical port number for the port going up.
 *  @param enable 1 - Enable match this outer TPID,
                  0 - Disable match this outer TPID.
 *  @param tag_num Tag number (Can be 0, 1, 2, 3).
 *  @param tag_value TPID value to be programmed.
 *  @brief Helper function to configure IDB OBM outer TPID.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_outer_tpid_config(int unit,
    soc_port_map_type_t *port_map, int lport, int enable,
    int tag_num, int tag_value)
{
    soc_reg_t reg;
    uint32 rval32;
    int phy_port;
    int pm_num;
    int pipe_num;
    const soc_reg_t obm_fc_thresh_pp[_TH3_PIPES_PER_DEV]
        [_TH3_PBLKS_PER_PIPE][4] = {
        {{IDB_OBM0_OUTER_TPID_0_PIPE0r, IDB_OBM0_OUTER_TPID_1_PIPE0r,
          IDB_OBM0_OUTER_TPID_2_PIPE0r, IDB_OBM0_OUTER_TPID_3_PIPE0r},
         {IDB_OBM1_OUTER_TPID_0_PIPE0r, IDB_OBM1_OUTER_TPID_1_PIPE0r,
          IDB_OBM1_OUTER_TPID_2_PIPE0r, IDB_OBM1_OUTER_TPID_3_PIPE0r},
         {IDB_OBM2_OUTER_TPID_0_PIPE0r, IDB_OBM2_OUTER_TPID_1_PIPE0r,
          IDB_OBM2_OUTER_TPID_2_PIPE0r, IDB_OBM2_OUTER_TPID_3_PIPE0r},
         {IDB_OBM3_OUTER_TPID_0_PIPE0r, IDB_OBM3_OUTER_TPID_1_PIPE0r,
          IDB_OBM3_OUTER_TPID_2_PIPE0r, IDB_OBM3_OUTER_TPID_3_PIPE0r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE1r, IDB_OBM0_OUTER_TPID_1_PIPE1r,
          IDB_OBM0_OUTER_TPID_2_PIPE1r, IDB_OBM0_OUTER_TPID_3_PIPE1r},
         {IDB_OBM1_OUTER_TPID_0_PIPE1r, IDB_OBM1_OUTER_TPID_1_PIPE1r,
          IDB_OBM1_OUTER_TPID_2_PIPE1r, IDB_OBM1_OUTER_TPID_3_PIPE1r},
         {IDB_OBM2_OUTER_TPID_0_PIPE1r, IDB_OBM2_OUTER_TPID_1_PIPE1r,
          IDB_OBM2_OUTER_TPID_2_PIPE1r, IDB_OBM2_OUTER_TPID_3_PIPE1r},
         {IDB_OBM3_OUTER_TPID_0_PIPE1r, IDB_OBM3_OUTER_TPID_1_PIPE1r,
          IDB_OBM3_OUTER_TPID_2_PIPE1r, IDB_OBM3_OUTER_TPID_3_PIPE1r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE2r, IDB_OBM0_OUTER_TPID_1_PIPE2r,
          IDB_OBM0_OUTER_TPID_2_PIPE2r, IDB_OBM0_OUTER_TPID_3_PIPE2r},
         {IDB_OBM1_OUTER_TPID_0_PIPE2r, IDB_OBM1_OUTER_TPID_1_PIPE2r,
          IDB_OBM1_OUTER_TPID_2_PIPE2r, IDB_OBM1_OUTER_TPID_3_PIPE2r},
         {IDB_OBM2_OUTER_TPID_0_PIPE2r, IDB_OBM2_OUTER_TPID_1_PIPE2r,
          IDB_OBM2_OUTER_TPID_2_PIPE2r, IDB_OBM2_OUTER_TPID_3_PIPE2r},
         {IDB_OBM3_OUTER_TPID_0_PIPE2r, IDB_OBM3_OUTER_TPID_1_PIPE2r,
          IDB_OBM3_OUTER_TPID_2_PIPE2r, IDB_OBM3_OUTER_TPID_3_PIPE2r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE3r, IDB_OBM0_OUTER_TPID_1_PIPE3r,
          IDB_OBM0_OUTER_TPID_2_PIPE3r, IDB_OBM0_OUTER_TPID_3_PIPE3r},
         {IDB_OBM1_OUTER_TPID_0_PIPE3r, IDB_OBM1_OUTER_TPID_1_PIPE3r,
          IDB_OBM1_OUTER_TPID_2_PIPE3r, IDB_OBM1_OUTER_TPID_3_PIPE3r},
         {IDB_OBM2_OUTER_TPID_0_PIPE3r, IDB_OBM2_OUTER_TPID_1_PIPE3r,
          IDB_OBM2_OUTER_TPID_2_PIPE3r, IDB_OBM2_OUTER_TPID_3_PIPE3r},
         {IDB_OBM3_OUTER_TPID_0_PIPE3r, IDB_OBM3_OUTER_TPID_1_PIPE3r,
          IDB_OBM3_OUTER_TPID_2_PIPE3r, IDB_OBM3_OUTER_TPID_3_PIPE3r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE4r, IDB_OBM0_OUTER_TPID_1_PIPE4r,
          IDB_OBM0_OUTER_TPID_2_PIPE4r, IDB_OBM0_OUTER_TPID_3_PIPE4r},
         {IDB_OBM1_OUTER_TPID_0_PIPE4r, IDB_OBM1_OUTER_TPID_1_PIPE4r,
          IDB_OBM1_OUTER_TPID_2_PIPE4r, IDB_OBM1_OUTER_TPID_3_PIPE4r},
         {IDB_OBM2_OUTER_TPID_0_PIPE4r, IDB_OBM2_OUTER_TPID_1_PIPE4r,
          IDB_OBM2_OUTER_TPID_2_PIPE4r, IDB_OBM2_OUTER_TPID_3_PIPE4r},
         {IDB_OBM3_OUTER_TPID_0_PIPE4r, IDB_OBM3_OUTER_TPID_1_PIPE4r,
          IDB_OBM3_OUTER_TPID_2_PIPE4r, IDB_OBM3_OUTER_TPID_3_PIPE4r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE5r, IDB_OBM0_OUTER_TPID_1_PIPE5r,
          IDB_OBM0_OUTER_TPID_2_PIPE5r, IDB_OBM0_OUTER_TPID_3_PIPE5r},
         {IDB_OBM1_OUTER_TPID_0_PIPE5r, IDB_OBM1_OUTER_TPID_1_PIPE5r,
          IDB_OBM1_OUTER_TPID_2_PIPE5r, IDB_OBM1_OUTER_TPID_3_PIPE5r},
         {IDB_OBM2_OUTER_TPID_0_PIPE5r, IDB_OBM2_OUTER_TPID_1_PIPE5r,
          IDB_OBM2_OUTER_TPID_2_PIPE5r, IDB_OBM2_OUTER_TPID_3_PIPE5r},
         {IDB_OBM3_OUTER_TPID_0_PIPE5r, IDB_OBM3_OUTER_TPID_1_PIPE5r,
          IDB_OBM3_OUTER_TPID_2_PIPE5r, IDB_OBM3_OUTER_TPID_3_PIPE5r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE6r, IDB_OBM0_OUTER_TPID_1_PIPE6r,
          IDB_OBM0_OUTER_TPID_2_PIPE6r, IDB_OBM0_OUTER_TPID_3_PIPE6r},
         {IDB_OBM1_OUTER_TPID_0_PIPE6r, IDB_OBM1_OUTER_TPID_1_PIPE6r,
          IDB_OBM1_OUTER_TPID_2_PIPE6r, IDB_OBM1_OUTER_TPID_3_PIPE6r},
         {IDB_OBM2_OUTER_TPID_0_PIPE6r, IDB_OBM2_OUTER_TPID_1_PIPE6r,
          IDB_OBM2_OUTER_TPID_2_PIPE6r, IDB_OBM2_OUTER_TPID_3_PIPE6r},
         {IDB_OBM3_OUTER_TPID_0_PIPE6r, IDB_OBM3_OUTER_TPID_1_PIPE6r,
          IDB_OBM3_OUTER_TPID_2_PIPE6r, IDB_OBM3_OUTER_TPID_3_PIPE6r}},
        {{IDB_OBM0_OUTER_TPID_0_PIPE7r, IDB_OBM0_OUTER_TPID_1_PIPE7r,
          IDB_OBM0_OUTER_TPID_2_PIPE7r, IDB_OBM0_OUTER_TPID_3_PIPE7r},
         {IDB_OBM1_OUTER_TPID_0_PIPE7r, IDB_OBM1_OUTER_TPID_1_PIPE7r,
          IDB_OBM1_OUTER_TPID_2_PIPE7r, IDB_OBM1_OUTER_TPID_3_PIPE7r},
         {IDB_OBM2_OUTER_TPID_0_PIPE7r, IDB_OBM2_OUTER_TPID_1_PIPE7r,
          IDB_OBM2_OUTER_TPID_2_PIPE7r, IDB_OBM2_OUTER_TPID_3_PIPE7r},
         {IDB_OBM3_OUTER_TPID_0_PIPE7r, IDB_OBM3_OUTER_TPID_1_PIPE7r,
          IDB_OBM3_OUTER_TPID_2_PIPE7r, IDB_OBM3_OUTER_TPID_3_PIPE7r}}
    };

    if (tag_num>3) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
          "Function soc_tomahawk3_idb_outer_tpid_config called with incorrect \
           tag number %0d. Only 0 to 3 tag number are allowed.\n "),tag_num));
    }
    phy_port = port_map->port_l2p_mapping[lport];
    pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    reg = obm_fc_thresh_pp[pipe_num][pm_num][tag_num];
    rval32 = 0;
    soc_reg_field_set(unit, reg, &rval32, ENABLEf, enable);
    soc_reg_field_set(unit, reg, &rval32, TPIDf, tag_value);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                    0, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_enable_dpp_ctrl(int unit, int pipe)
 *  @param unit Chip unit number.
 *  @param pipe Pipe number going up.
 *  @brief Helper function to enable DPP Control.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_enable_dpp_ctrl(int unit, int pipe)
{
    soc_reg_t reg;
    uint32 rval32;
    static const soc_reg_t iarb_dpp_ctrl_regs[_TH3_PIPES_PER_DEV] = {
        IARB_DPP_CTRL_PIPE0r,
        IARB_DPP_CTRL_PIPE1r,
        IARB_DPP_CTRL_PIPE2r,
        IARB_DPP_CTRL_PIPE3r,
        IARB_DPP_CTRL_PIPE4r,
        IARB_DPP_CTRL_PIPE5r,
        IARB_DPP_CTRL_PIPE6r,
        IARB_DPP_CTRL_PIPE7r
    };
    reg = iarb_dpp_ctrl_regs[pipe];
    rval32 = 0;
    soc_reg_field_set(unit, reg, &rval32, RATE_ENf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                        REG_PORT_ANY, 0, rval32));
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_en_epc_link_bmap(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to Write EPC_LINK_BMAP table to enable forwarding traffic
 *         to port.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_en_epc_link_bmap(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    uint32 epc_link_bmap_tbl[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    int lport;
    sal_memset(epc_link_bmap_tbl, 0, sizeof(epc_link_bmap_tbl));
    sal_memset(memfld, 0, sizeof(memfld));
    /* Make EPC_LINK_BMAP=1 for all the ports going up */
    for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
        if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
            memfld[(lport>>5)] |= (0x1<<(lport&0x1f));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Enable EPC_LINK_BITMAP write:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    soc_mem_field_set(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                      epc_link_bmap_tbl));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_en_dest_port_bmap(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to Write ING_DEST_PORT_ENABLE table to enable
           forwarding traffic to port.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_en_dest_port_bmap(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    uint32 ing_dst_p_en_tbl[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    int lport;

    sal_memset(ing_dst_p_en_tbl, 0, sizeof(ing_dst_p_en_tbl));
    sal_memset(memfld, 0, sizeof(memfld));
    /* Make EPC_LINK_BMAP=1 for all the ports going up */
    for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
        if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
            memfld[(lport>>5)] |= (0x1<<(lport&0x1f));
        }
    }
    soc_mem_field_set(unit, ING_DEST_PORT_ENABLEm, ing_dst_p_en_tbl,
                      PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_DEST_PORT_ENABLEm,
                                      MEM_BLOCK_ALL, 0, ing_dst_p_en_tbl));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_ip_slot_pipeline_config(
      int unit,
      soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
               struct variable.
 *  @brief API to initialize SLOT_PIPELINE_CONFIG
 *  @returns SOC_E_NONE
 */
int soc_tomahawk3_ip_slot_pipeline_config(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval;
    int latency;

    /* Set the latency in Slot Pipeline config register */
    reg = SLOT_PIPELINE_CONFIGr;
    rval = 0;

    /*
     * Recommended Latency  = (DPPL + A + B + C) * R + D + E + F
     * DPP Latency (DPPL)                       : 134
     * Extra latency in DPP clk (A)             : 6
     * Domain crossing for output event fifo(B) : 3
     * Outstanding Credit (C)                   : 20
     * Early retrieval in core clk (E)          : -54
     * Default margin (D)                       : 5
     * Core clock to DPP clock ratio            : R
     * Max Accumulation Latency                 : F
     */

    soc_tomahawk3_get_set_slot_pipeline_latency(
        port_schedule_state->frequency,
        port_schedule_state->in_port_map.port_p2PBLK_inst_mapping[0],
        134, /* DPP Latency (DPPL) */
        6,   /* Extra latency in DPP clk (A) */
        3,   /* Domain crossing for output event fifo(B) */
        12,  /* N - Credit loop (12) */
        20,  /* C - Outstanding Credit (20) */
        5,   /*  Default margin (D) */
        -54, /* Early retrieval in core clk (E) */
        &latency);

    soc_reg_field_set(unit, reg, &rval, WR_ENf, 1);
    soc_reg_field_set(unit, reg, &rval, LATENCYf, latency);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_obm_poll_buffer_empty(int unit, int pipe_num,
 *              int pm_num, int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 3.
 *  @param subp Subport number.
 *  @brief Helper function to poll for IDB OBM buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_obm_poll_buffer_empty(int unit,
                 int pipe_num, int pm_num, int subp)
{
    soc_reg_t reg;
    uint64 rval64, temp64;
    uint64 total_count;
    int timeout_in_usec;
    int timeout_count;
    int total_count_lo, total_count_hi;

    const soc_reg_t obm_usage_1_regs[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_USAGE_1_PIPE0r, IDB_OBM1_USAGE_1_PIPE0r,
         IDB_OBM2_USAGE_1_PIPE0r, IDB_OBM3_USAGE_1_PIPE0r},
        {IDB_OBM0_USAGE_1_PIPE1r, IDB_OBM1_USAGE_1_PIPE1r,
         IDB_OBM2_USAGE_1_PIPE1r, IDB_OBM3_USAGE_1_PIPE1r},
        {IDB_OBM0_USAGE_1_PIPE2r, IDB_OBM1_USAGE_1_PIPE2r,
         IDB_OBM2_USAGE_1_PIPE2r, IDB_OBM3_USAGE_1_PIPE2r},
        {IDB_OBM0_USAGE_1_PIPE3r, IDB_OBM1_USAGE_1_PIPE3r,
         IDB_OBM2_USAGE_1_PIPE3r, IDB_OBM3_USAGE_1_PIPE3r},
        {IDB_OBM0_USAGE_1_PIPE4r, IDB_OBM1_USAGE_1_PIPE4r,
         IDB_OBM2_USAGE_1_PIPE4r, IDB_OBM3_USAGE_1_PIPE4r},
        {IDB_OBM0_USAGE_1_PIPE5r, IDB_OBM1_USAGE_1_PIPE5r,
         IDB_OBM2_USAGE_1_PIPE5r, IDB_OBM3_USAGE_1_PIPE5r},
        {IDB_OBM0_USAGE_1_PIPE6r, IDB_OBM1_USAGE_1_PIPE6r,
         IDB_OBM2_USAGE_1_PIPE6r, IDB_OBM3_USAGE_1_PIPE6r},
        {IDB_OBM0_USAGE_1_PIPE7r, IDB_OBM1_USAGE_1_PIPE7r,
         IDB_OBM2_USAGE_1_PIPE7r, IDB_OBM3_USAGE_1_PIPE7r}
    };

    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }

    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    COMPILER_64_ZERO(temp64);
    reg = obm_usage_1_regs[pipe_num][pm_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, subp,
                                                &rval64));
        total_count = soc_reg64_field_get(unit, reg, rval64, TOTAL_COUNTf);
        if (!COMPILER_64_GT(total_count, temp64)) {
            break;
        }

        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            total_count_lo = COMPILER_64_LO(total_count);
            total_count_hi = COMPILER_64_HI(total_count);
            LOG_ERROR(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
               "Timeout:: soc_tomahawk3_idb_obm_poll_buffer_empty "
               "%0d us timeout !! pipe %0d pm %0d total_count_lo %0d total_count_hi %0d subp %0d"),
               timeout_in_usec,pipe_num,pm_num,total_count_lo,total_count_hi,subp));
            return SOC_E_FAIL;
        }
    } while (COMPILER_64_GT(total_count, temp64));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_ca_poll_buffer_empty(int unit,
 *              int pipe_num, int pm_num, int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 3.
 *  @param subp Subport number.
 *  @brief Helper function to poll for IDB Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_poll_buffer_empty(int unit,
                    int pipe_num, int pm_num, int subp)
{
    soc_reg_t reg;
    uint64 rval64;
    uint64 ca_fifo_empty;
    int timeout_in_usec;
    int timeout_count;

    static const soc_reg_t ca_status_2_regs[_TH3_PIPES_PER_DEV]
        [_TH3_PBLKS_PER_PIPE] = {
        {IDB_CA0_HW_STATUS_2_PIPE0r, IDB_CA1_HW_STATUS_2_PIPE0r,
         IDB_CA2_HW_STATUS_2_PIPE0r, IDB_CA3_HW_STATUS_2_PIPE0r},
        {IDB_CA0_HW_STATUS_2_PIPE1r, IDB_CA1_HW_STATUS_2_PIPE1r,
         IDB_CA2_HW_STATUS_2_PIPE1r, IDB_CA3_HW_STATUS_2_PIPE1r},
        {IDB_CA0_HW_STATUS_2_PIPE2r, IDB_CA1_HW_STATUS_2_PIPE2r,
         IDB_CA2_HW_STATUS_2_PIPE2r, IDB_CA3_HW_STATUS_2_PIPE2r},
        {IDB_CA0_HW_STATUS_2_PIPE3r, IDB_CA1_HW_STATUS_2_PIPE3r,
         IDB_CA2_HW_STATUS_2_PIPE3r, IDB_CA3_HW_STATUS_2_PIPE3r},
        {IDB_CA0_HW_STATUS_2_PIPE4r, IDB_CA1_HW_STATUS_2_PIPE4r,
         IDB_CA2_HW_STATUS_2_PIPE4r, IDB_CA3_HW_STATUS_2_PIPE4r},
        {IDB_CA0_HW_STATUS_2_PIPE5r, IDB_CA1_HW_STATUS_2_PIPE5r,
         IDB_CA2_HW_STATUS_2_PIPE5r, IDB_CA3_HW_STATUS_2_PIPE5r},
        {IDB_CA0_HW_STATUS_2_PIPE6r, IDB_CA1_HW_STATUS_2_PIPE6r,
         IDB_CA2_HW_STATUS_2_PIPE6r, IDB_CA3_HW_STATUS_2_PIPE6r},
        {IDB_CA0_HW_STATUS_2_PIPE7r, IDB_CA1_HW_STATUS_2_PIPE7r,
         IDB_CA2_HW_STATUS_2_PIPE7r, IDB_CA3_HW_STATUS_2_PIPE7r}
    };
    static const soc_field_t ca_status_2_regs_fifo_empty_fields[] = {
        FIFO_EMPTY_PORT0f, FIFO_EMPTY_PORT1f,
        FIFO_EMPTY_PORT2f, FIFO_EMPTY_PORT3f,
        FIFO_EMPTY_PORT4f, FIFO_EMPTY_PORT5f,
        FIFO_EMPTY_PORT6f, FIFO_EMPTY_PORT7f
    };

    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }

    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    reg = ca_status_2_regs[pipe_num][pm_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                                 reg, REG_PORT_ANY, 0, &rval64));
        ca_fifo_empty = soc_reg64_field_get(unit, reg, rval64,
                        ca_status_2_regs_fifo_empty_fields[subp]);
        if (!COMPILER_64_IS_ZERO(ca_fifo_empty)) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
               "Timeout:: soc_tomahawk3_idb_ca_poll_buffer_empty "
                           "%0d us timeout reached!!"),timeout_in_usec));
            return SOC_E_FAIL;
        }
    } while (COMPILER_64_IS_ZERO(ca_fifo_empty));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_lpbk_ca_in_reset(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to check if the IDB loopback CA is in reset.
 *  @returns Reset status.
 */
int
soc_tomahawk3_idb_lpbk_ca_in_reset(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int buffer_in_reset;
    reg = th3_idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    buffer_in_reset = soc_reg_field_get(unit, reg, rval, PORT_RESETf);
    return buffer_in_reset;
}

/*! @fn int soc_tomahawk3_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to poll for IDB LOOPBACK port
           Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 ca_fifo_empty;
    int timeout_in_usec;
    int timeout_count;
    const soc_reg_t th3_idb_ca_lpbk_hw_status[_TH3_PIPES_PER_DEV]= {
        IDB_CA_LPBK_HW_STATUS_PIPE0r, IDB_CA_LPBK_HW_STATUS_PIPE1r,
        IDB_CA_LPBK_HW_STATUS_PIPE2r, IDB_CA_LPBK_HW_STATUS_PIPE3r,
        IDB_CA_LPBK_HW_STATUS_PIPE4r, IDB_CA_LPBK_HW_STATUS_PIPE5r,
        IDB_CA_LPBK_HW_STATUS_PIPE6r, IDB_CA_LPBK_HW_STATUS_PIPE7r};
    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }
    reg = th3_idb_ca_lpbk_hw_status[pipe_num];
    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                   reg, REG_PORT_ANY, 0, &rval32));
        ca_fifo_empty = soc_reg_field_get(unit, reg, rval32,
                                                FIFO_EMPTYf);
        if (ca_fifo_empty) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
               "Timeout:: soc_tomahawk3_idb_ca_lpbk_poll_buffer_empty "
                           "%0d us timeout reached!!"),timeout_in_usec));
            return SOC_E_FAIL;
        }
    } while (!ca_fifo_empty);
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_cpu_ca_in_reset(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to check if the IDB CPU CA is in reset.
 *  @returns Reset status;
 */
int
soc_tomahawk3_idb_cpu_ca_in_reset(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int buffer_in_reset;
    reg = th3_idb_ca_cpu_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    buffer_in_reset = soc_reg_field_get(unit, reg, rval, PORT_RESETf);
    return buffer_in_reset;
}

/*! @fn int soc_tomahawk3_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to poll for IDB CPU Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 ca_fifo_empty;
    int timeout_in_usec;
    int timeout_count;
    const soc_reg_t idb_ca_cpu_hw_status[_TH3_PIPES_PER_DEV]= {
        IDB_CA_CPU_HW_STATUS_PIPE0r, IDB_CA_CPU_HW_STATUS_PIPE1r,
        IDB_CA_CPU_HW_STATUS_PIPE2r, IDB_CA_CPU_HW_STATUS_PIPE3r,
        IDB_CA_CPU_HW_STATUS_PIPE4r, IDB_CA_CPU_HW_STATUS_PIPE5r,
        IDB_CA_CPU_HW_STATUS_PIPE6r, IDB_CA_CPU_HW_STATUS_PIPE7r};
    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }
    reg = idb_ca_cpu_hw_status[pipe_num];
    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                   reg, REG_PORT_ANY, 0, &rval32));
        ca_fifo_empty = soc_reg_field_get(unit, reg, rval32,
                                                FIFO_EMPTYf);
        if (ca_fifo_empty) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
               "Timeout:: soc_tomahawk3_idb_obm_poll_buffer_empty "
                           "%0d us timeout reached!!"),timeout_in_usec));
            return SOC_E_FAIL;
        }
    } while (!ca_fifo_empty);
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_lpbk_ca_reset_buffer(int unit, int pipe_num, 
                      int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset
 *                  for loopback port.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_lpbk_ca_reset_buffer(int unit, int pipe_num, int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    reg = th3_idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_RESETf, reset_buffer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_cpu_ca_reset_buffer(int unit, int pipe_num, 
                      int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset
 *                  for CPU port.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_cpu_ca_reset_buffer(int unit, int pipe_num, int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    reg = th3_idb_ca_cpu_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_RESETf, reset_buffer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_force_saf_duration_timer_cfg(int unit,
                soc_port_schedule_state_t *port_schedule_state, int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
               struct variable.
 *  @param pipe_num Tomahawk3 pipe number.
 *  @brief Helper function to configure 
           IDB_FORCE_SAF_CONFIG.DURATION_TIMER register field.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_force_saf_duration_timer_cfg(int unit, 
    soc_port_schedule_state_t *port_schedule_state, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int core_clock_freq;
    int duration_timer;
    const soc_reg_t idb_force_saf_config_regs[_TH3_PIPES_PER_DEV] =
    {IDB_DBG_B_PIPE0r, IDB_DBG_B_PIPE1r, IDB_DBG_B_PIPE2r, IDB_DBG_B_PIPE3r,
     IDB_DBG_B_PIPE4r, IDB_DBG_B_PIPE5r, IDB_DBG_B_PIPE6r, IDB_DBG_B_PIPE7r};

    core_clock_freq = port_schedule_state->frequency;
    duration_timer = 2*core_clock_freq;
    reg = idb_force_saf_config_regs[pipe_num];
    rval=0;
    soc_reg_field_set(unit, reg, &rval, FIELD_Af, duration_timer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_speed_to_e2e_settings_class_map(int speed)
 *  @param speed Port speed.
 *  @brief Helper function to get e2e settings class as below::
 *         The function look at the port speed & returns the encoding as below::
 *          0 - SAF,
 *          1 - 10GE,
 *          2 - 25GE,
 *          3 - 40GE,
 *          4 - 50GE,
 *          5 - 100GE,
 *          6 - 200GE,
 *          7 - 400GE,
 *  @returns int
 */
int
soc_tomahawk3_speed_to_e2e_settings_class_map(int speed)
{
    int e2e_settings_class;
    int i;

    e2e_settings_class=0;
    for (i = 0; i < COUNTOF(soc_tomahawk3_e2e_settings_tbl); i++) {
        if (soc_tomahawk3_e2e_settings_tbl[i].speed == speed) {
            e2e_settings_class = i;
            break;
        }
    }
    return e2e_settings_class;
}


/*! @fn int soc_tomahawk3_idb_init(int unit,
               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to soc_port_schedule_state_t variable.
 *  @brief Helper function to initialize TH3 IDB.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_init(int unit,
                             soc_port_schedule_state_t *port_schedule_state)
{
    int lport;
    int phy_port;
    int pmnum;
    int pm_going_up;
    int reset;
    int lport_save = 0;
    int pipe_num;
    int pipe_going_up;
    uint32 pipe_map;

    /* If multiple ports going up in same PM, write only once for the
       general registers that are common to all sub-ports. */
    for (pmnum = 0; pmnum < _TH3_NUM_OF_TSCBH ; pmnum++) {
        pm_going_up=0;
        lport_save=0;
        for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
            if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
                if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)&&
                    (((phy_port - 1) >> 3) == pmnum)) {
                    pm_going_up|= 1;
                    lport_save = lport;
                }
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_idb_port_init: pmnum %0d pm_going_up=%0d"),
                  pmnum, pm_going_up));

        if (pm_going_up){
            reset=0;
            lport = lport_save;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_pa_reset(
                unit, &port_schedule_state->out_port_map,lport,reset));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_tdm_cfg(
                unit, &port_schedule_state->out_port_map,lport));
        }
    }

    /*IDB to DEV mapping table configured for all entries */
    soc_tomahawk3_get_pipe_map(unit, port_schedule_state, &pipe_map);
    for (pipe_num = 0; pipe_num < _TH3_PIPES_PER_DEV; pipe_num++) {
        if (!(pipe_map & (1 << pipe_num))) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_init_idb_to_dev_all_pipe(
            unit, pipe_num));
    }

    /* Configure per port registers: Front Panel ports; */
    for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
        if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
            phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            /*IDB mapping tables*/
            /*SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_init_idb_to_dev_pmap_tbl(
                unit, &port_schedule_state->out_port_map,lport));*/

            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)){
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_idb_init: Start obm init phy_port %0d"),
                  phy_port));
                /*OBM init*/
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_shared_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_thresh(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_flow_ctrl_cfg(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_fc_threshold(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_ct_thresh(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_port_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_monitor_stats_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_force_saf_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_obm_lag_detection_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_pri_map_set(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_buffer_config(
                    unit, &port_schedule_state->out_port_map,lport));

                /* Front panel Physical to IDB port mapping */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_init_pnum_mapping_tbl(
                    unit, &port_schedule_state->out_port_map,lport));

                /*CA */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_ca_ct_thresh(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_bmop_set(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_ca_force_saf_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_wr_ca_lag_detection_config(
                    unit, port_schedule_state,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_buffer_config(
                    unit, &port_schedule_state->out_port_map,lport));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_peek_depth_cfg(
                    unit, &port_schedule_state->out_port_map,lport));
            }else if(soc_tomahawk3_phy_is_lpbk_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_lpbk_ca_peek_depth_cfg(
                    unit, pipe_num));
            }else if(soc_tomahawk3_phy_is_cpu_mgmt_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_peek_depth_cfg(
                    unit, pipe_num));
            }
        }
    }
    
    /* Force SAF Duration timer configuration per pipe */
    for (pipe_num = 0; pipe_num < _TH3_PIPES_PER_DEV ; pipe_num++) {
        pipe_going_up=0;
        for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
            if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
                if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)&&
                    (soc_tomahawk3_get_pipe_from_phy_port(phy_port) == pipe_num)) {
                    pipe_going_up|= 1;
                    break;
                }
            }
        }
        if (pipe_going_up){
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_force_saf_duration_timer_cfg(
                unit, port_schedule_state, pipe_num));
        }
    }

    /* Release reset for CPU & Loopback port CA */
    reset = 0;
    for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
        if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
            phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);

            if(soc_tomahawk3_phy_is_lpbk_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_lpbk_ca_reset_buffer(unit,
                    pipe_num, reset));
            }else if(soc_tomahawk3_phy_is_cpu_mgmt_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
                    pipe_num, reset));
            }
        }
    }

    /* Release reset for All Front panel port CA */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_wr_dom_rst_rel_all(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_rd_dom_rst_rel_all(
        unit, port_schedule_state));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_rst_rel_all(
        unit, port_schedule_state));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_ipipe_init(int unit,
               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to soc_port_schedule_state_t variable.
 *  @brief Helper function to initialize TH3 IPIPE.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_ipipe_init(int unit,
                             soc_port_schedule_state_t *port_schedule_state)
{
    SOC_IF_ERROR_RETURN(soc_tomahawk3_en_epc_link_bmap(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_en_dest_port_bmap(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ip_slot_pipeline_config(
        unit, port_schedule_state));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_port_down(int unit,
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
           variable
 *  @brief Helper function to follow IDB port down sequence based on
           port_schedule_state->resource[i] array.
           Follows the sequence specified in document 
           "TH3 Flexport Specification.pdf", Revision 0.1, Section 5.1.
           Two main parts:
              (1) Poll untill IDB buffers are empty.
              (2) Hold IDB buffers in reset state.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_port_down(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int reset_buffer;
    int phy_port;
    int subp;
    int lport;

    /* Poll until IDB buffers are empty for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
            subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_idb_port_down: call soc_tomahawk3_idb_obm_poll_buffer_empty %0d"),
                  phy_port));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_poll_buffer_empty(
                    unit, pipe_num, pm_num, subp));
            }
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_idb_port_down: call soc_tomahawk3_idb_ca_poll_buffer_empty %0d"),
                  phy_port));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_poll_buffer_empty(
                    unit, pipe_num, pm_num, subp));
            } else if(soc_tomahawk3_phy_is_lpbk_port(phy_port)){
                if (!soc_tomahawk3_idb_lpbk_ca_in_reset(unit,pipe_num)){
                    SOC_IF_ERROR_RETURN(
                        soc_tomahawk3_idb_ca_lpbk_poll_buffer_empty(
                        unit, pipe_num));
                }
            }else if(soc_tomahawk3_phy_is_cpu_mgmt_port(phy_port)){
                if (!soc_tomahawk3_idb_cpu_ca_in_reset(unit,pipe_num)){
                    SOC_IF_ERROR_RETURN(
                        soc_tomahawk3_idb_ca_cpu_poll_buffer_empty(
                        unit, pipe_num));
                }
            }
        }
    }

    /* Hold IDB buffers in reset state for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            lport = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->in_port_map.port_l2p_mapping[lport];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
            subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
            reset_buffer = 1;
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "port_down: call soc_tomahawk3_idb_obm_reset %0d"),
                  phy_port));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_reset(
                    unit, &port_schedule_state->in_port_map,
                    lport, reset_buffer));
            }
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "port_down: call soc_tomahawk3_idb_ca_rd_domain_reset %0d"),
                  phy_port));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_rd_domain_reset(
                    unit, &port_schedule_state->in_port_map,
                    lport, reset_buffer));
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "port_down: call soc_tomahawk3_idb_ca_wr_domain_reset %0d"),
                  phy_port));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_wr_domain_reset(
                    unit, &port_schedule_state->in_port_map,
                    lport, reset_buffer));
            }else if(soc_tomahawk3_phy_is_lpbk_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_lpbk_ca_reset_buffer(unit,
                    pipe_num, reset_buffer));
            }else if(soc_tomahawk3_phy_is_cpu_mgmt_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
                    pipe_num, reset_buffer));
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_idb_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to bringup IDB based on
           port_schedule_state->resource[i] array.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_idb_port_up(int unit,
                          soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int reset_buffer;
    int phy_port;
    int lport;
    int subp;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_idb_port_up: start.")
                  ));

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            lport = port_schedule_state->resource[i].logical_port;
            phy_port = 
                port_schedule_state->out_port_map.port_l2p_mapping[lport];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            reset_buffer = 1;
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_reset(
                    unit, &port_schedule_state->out_port_map,
                    lport, reset_buffer));
            }
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_rd_domain_reset(
                    unit, &port_schedule_state->out_port_map,
                    lport, reset_buffer));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_wr_domain_reset(
                    unit, &port_schedule_state->out_port_map,
                    lport, reset_buffer));
            }else if(soc_tomahawk3_phy_is_lpbk_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_lpbk_ca_reset_buffer(unit,
                    pipe_num, reset_buffer));
            }else if(soc_tomahawk3_phy_is_cpu_mgmt_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
                    pipe_num, reset_buffer));
            }
        }
    }

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            lport = port_schedule_state->resource[i].logical_port;
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[lport];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
            subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "IDB port Up i %1d phy_port %1d pipe=%1d pm=%1d "
                "subp=%1d \n"),
                i, phy_port, pipe_num, pm_num, subp));
            reset_buffer = 0;
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_wr_domain_reset(
                    unit, &port_schedule_state->out_port_map, lport, reset_buffer));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_ca_rd_domain_reset(
                    unit, &port_schedule_state->out_port_map, lport, reset_buffer));
            }else if(soc_tomahawk3_phy_is_lpbk_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_lpbk_ca_reset_buffer(unit,
                    pipe_num, reset_buffer));
            }else if(soc_tomahawk3_phy_is_cpu_mgmt_port(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
                    pipe_num, reset_buffer));
            }
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_reset(
                    unit, &port_schedule_state->out_port_map, lport, reset_buffer));
            }
        }
    }
    return SOC_E_NONE;
}



/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */
