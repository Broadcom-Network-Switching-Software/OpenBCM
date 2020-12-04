/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File:        hurricane4_mmu_config_apis.c
 *  Purpose:     General MMU configs.
 *  Requires:
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#include <soc/hurricane4_tdm.h>
#include <soc/flexport/hurricane4/hurricane4_flexport.h>


/* ASF core config */
/*! @struct soc_hurricane4_mmu_asf_core_cfg_s
 *  @brief
 */
typedef struct soc_hurricane4_mmu_asf_core_cfg_s {
    int speed;
    uint32 min_sp;               /* Min. Src. Port Speed */
    uint32 max_sp;               /* Max. Src. Port Speed */
    uint32 mmu_prebuffer;        /* MMU prebuffer */
    uint32 ep_credit_low;        /*EP Credit Low Threshold: 2-isto-1 oversub*/
    uint32 ep_credit_hi;         /*EP Credit Hi Threshold: 2-isto-1 oversub*/
    uint32 ep_credit_low_1dot5;  /*EP Credit Low Threshold: 2-isto-1 oversub*/
    uint32 ep_credit_hi_1dot5;   /*EP Credit Hi Threshold: 2-isto-1 oversub*/
    uint32 ct_fifo_threshold_lr; /* CT Fifo Threshold*/
    uint32 ct_fifo_threshold_os; /* CT Fifo Threshold*/
    uint32 ct_fifo_depth_lr;     /* CT Fifo Depth*/
    uint32 ct_fifo_depth_os;     /* CT Fifo Depth*/
    uint32 fast_to_slow_accum;   /* Fast to Slow Accumulation*/
    /* NOTE: DON'T ALTER FIELD ORDER */
} soc_hurricane4_mmu_asf_core_cfg_t;

/* ASF Core Config Table  */
static const soc_hurricane4_mmu_asf_core_cfg_t
soc_hurricane4_mmu_asf_cfg_tbl[] = {
    {    -1,  0,  0,  0, 0,   0, 0, 0,  0,  0, 0,  0,  0}, /* SAF     */
    { 10000,  1, 10,  0, 1,   7, 1, 5,  3, 10, 8, 19, 38}, /* 10GE    */
    { 11000,  1, 10,  6, 1,   7, 1, 5,  3, 10, 8, 19, 38}, /* HG[11]  */
    { 20000,  3, 10,  0, 3,  12, 1, 7,  3, 10, 8, 19, 29}, /* 20GE    */
    { 21000,  3, 10,  7, 3,  12, 1, 7,  3, 10, 8, 19, 29}, /* HG[21]  */
    { 25000,  5, 10,  0, 3,  12, 2, 8,  3, 10, 8, 19, 25}, /* 25GE    */
    { 27000,  5, 10,  7, 3,  12, 2, 8,  3, 10, 8, 19, 25}, /* HG[27]  */
    { 40000,  7, 12,  0, 5,  21, 3, 12, 3, 10, 8, 19, 29}, /* 40GE    */
    { 42000,  7, 12,  7, 5,  21, 3, 12, 3, 10, 8, 19, 29}, /* HG[42]  */
    { 50000,  9, 12,  0, 6,  22, 3, 14, 3, 10, 8, 19, 25}, /* 50GE    */
    { 53000,  9, 12,  7, 6,  22, 3, 14, 3, 10, 8, 19, 25}, /* HG[53]  */
    {100000, 11, 12,  0, 11, 32, 7, 25, 3, 10, 8, 19,  3}, /* 100GE   */
    {106000, 11, 12,  8, 11, 32, 7, 25, 3, 10, 8, 19,  0}  /* HG[106] */
};

/*! @struct soc_hurricane4_mmu_thdi_pg_hdrm_cfg_s
 *  @brief
 */
typedef struct soc_hurricane4_mmu_thdi_pg_hdrm_cfg_s {
    int speed;
    uint32 line_rate_cells;
    uint32 oversub_cells;
} soc_hurricane4_mmu_thdi_pg_hdrm_cfg_t;

static const soc_hurricane4_mmu_thdi_pg_hdrm_cfg_t soc_hurricane4_mmu_thdi_pg_hdrm_cfg_tbl[]
= {
    {1000,  118, 0},
    {2500,  120, 0},
    {5000,  160, 194},
    {10000, 160, 194},
    {20000, 273, 402},
    {25000, 273, 402},
    {40000, 273, 402},
    {50000, 496, 690},
    {100000, 496, 690}
};


/*! @fn int soc_hurricane4_mmu_thdi_get_pg_hdrm_setting(int speed, int line_rate,
 *              int* setting)
 *  @param speed port speed
 *  @param line_rate whether the port is line rate or oversub
 *  @param setting pointer to the limit setting, contains returned value
 *  @brief API to provide PG headroom limit setting based on port speed and
 *         lr/os (for lossless mode)
 */
int
soc_hurricane4_mmu_thdi_get_pg_hdrm_setting(int unit, int speed, int line_rate,
        uint32* setting)
{
    int index;
    int found = 0;
    for(index=0; index<HR4_MMU_NUM_SPEEDS; index++) {
        if (soc_hurricane4_mmu_thdi_pg_hdrm_cfg_tbl[index].speed == speed) {
            *setting =
                line_rate ? soc_hurricane4_mmu_thdi_pg_hdrm_cfg_tbl[index].
                line_rate_cells
                : soc_hurricane4_mmu_thdi_pg_hdrm_cfg_tbl[index].oversub_cells;
            found = 1;
            break;
        }
    }
    if (!found) {
        LOG_CLI((BSL_META_U(unit,
                        "In soc_hurricane4_mmu_thdi_get_pg_hdrm_setting"
                        "function->Unsupported speed %0d for HDRM settings"),
                    speed));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}
/*! @fn int soc_hurricane4_mmu_get_num_l0_nodes_per_port(int unit, int lcl_port,
 *              int pipe, int *number_l0_nodes)
 *  @param unit Device number
 *  @param lcl_port Pipe's local mmu port number
 *  @param pipe Pipe Number
 *  @param *number_l0_nodes  Returned Number of L0 Nodes for that port
 *  @brief API to provide number of L0 Nodes per a Pipe's Local MMU Port.
 */
int
soc_hurricane4_mmu_get_num_l0_nodes_per_port(int unit, int lcl_port, int pipe,
        int *number_l0_nodes)
{
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *number_l0_nodes = HR4_MMU_NUM_L0_NODES_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *number_l0_nodes = HR4_MMU_NUM_L0_NODES_PER_HSP_DL_PORT;
    }
    else if ((HR4_MMU_IS_CPU_PORT(pipe, lcl_port))) {
        *number_l0_nodes = HR4_MMU_NUM_L0_NODES_CPU_PORT;
    }
    else if ((HR4_MMU_IS_LPBK_PORT(lcl_port))) {
        *number_l0_nodes = HR4_MMU_NUM_L0_NODES_LPBK_PORT;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified for"
                            "Getting L0 Offset. "
                            "Specified value is greater than 71. "
                            "Input is lcl_port %0d, pipe %0d"), lcl_port, pipe));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_num_l1_nodes_per_port(int unit, int lcl_port,
 *              int pipe, int *number_l1_nodes)
 *  @param unit Device number
 *  @param lcl_port Pipe's local mmu port number
 *  @param pipe Pipe Number
 *  @param *number_l1_nodes  Returned Number of L1 Nodes for that port
 *  @brief API to provide number of L1 Nodes per a Pipe's Local MMU Port.
 */
int
soc_hurricane4_mmu_get_num_l1_nodes_per_port(int unit, int lcl_port, int pipe,
        int *number_l1_nodes)
{
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *number_l1_nodes = HR4_MMU_NUM_L1_NODES_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *number_l1_nodes = HR4_MMU_NUM_L1_NODES_PER_HSP_DL_PORT;
    }
    else if ((HR4_MMU_IS_CPU_PORT(pipe, lcl_port))) {
        *number_l1_nodes = HR4_MMU_NUM_L1_NODES_CPU_PORT;
    }
    else if ((HR4_MMU_IS_LPBK_PORT(lcl_port))) {
        *number_l1_nodes = HR4_MMU_NUM_L1_NODES_LPBK_PORT;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified for"
                            "Getting L1 Offset. "
                            "Specified value is greater than 71. "
                            "Input is lcl_port %0d, pipe %0d"), lcl_port, pipe));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_mmu_get_num_l2_nodes_per_port(int unit, int lcl_port,
 *              int pipe, int *number_l2_nodes)
 *  @param unit Device number
 *  @param lcl_port Pipe's local mmu port number
 *  @param pipe Pipe Number
 *  @param *number_l2_nodes  Returned Number of L2 Nodes for that port
 *  @brief API to provide number of L2 Nodes per a Pipe's Local MMU Port.
 */
int
soc_hurricane4_mmu_get_num_l2_nodes_per_port(int unit, int lcl_port, int pipe,
        int *number_l2_nodes)
{
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *number_l2_nodes = HR4_MMU_NUM_L2_NODES_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *number_l2_nodes = HR4_MMU_NUM_L2_NODES_PER_HSP_DL_PORT;
    }
    else if ((HR4_MMU_IS_CPU_PORT(pipe, lcl_port))) {
        *number_l2_nodes = HR4_MMU_NUM_L2_NODES_CPU_PORT;
    }
    else if ((HR4_MMU_IS_LPBK_PORT(lcl_port))) {
        *number_l2_nodes = HR4_MMU_NUM_L2_NODES_LPBK_PORT;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified for"
                            "Getting L2 Offset. "
                            "Specified value is greater than 71. "
                            "Input is lcl_port %0d, pipe %0d"), lcl_port, pipe));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_num_l3_uc_nodes_per_port(int unit,
 *               int lcl_port, int pipe, int *number_l3_uc_nodes)
 *  @param unit Device number
 *  @param lcl_port Pipe's local mmu port number
 *  @param pipe Pipe Number
 *  @param *number_l3_uc_nodes  Returned Number of UC L3 Nodes for that port
 *  @brief Provides number of unicast (UC) L3 Nodes per a Pipe's Local MMU Port.
 */
int
soc_hurricane4_mmu_get_num_l3_uc_nodes_per_port(int unit, int lcl_port, int pipe,
        int *number_l3_uc_nodes)
{
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *number_l3_uc_nodes = HR4_MMU_NUM_UCQ_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *number_l3_uc_nodes = HR4_MMU_NUM_UCQ_PER_HSP_DL_PORT;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified for"
                            "Getting L3 UC Offset. "
                            "Specified value is greater than 69")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_num_l3_mc_nodes_per_port(int unit,
  int lcl_port, int pipe, int *number_l3_mc_nodes)
 *  @param unit Device number
 *  @param lcl_port Pipe's local mmu port number
 *  @param pipe Pipe Number
 *  @param *number_l3_mc_nodes  Returned Number of MC L3 Nodes for that port
 *  @brief Provides # of multicast (MC) L3 Nodes per a Pipe's Local MMU Port.
 */
int
soc_hurricane4_mmu_get_num_l3_mc_nodes_per_port(int unit, int lcl_port, int pipe,
        int *number_l3_mc_nodes)
{
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *number_l3_mc_nodes = HR4_MMU_NUM_MCQ_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *number_l3_mc_nodes = HR4_MMU_NUM_MCQ_PER_HSP_DL_PORT;
    }
    else if (lcl_port == HR4_MMU_CPU_PORT_NUM) {
        *number_l3_mc_nodes = HR4_MMU_NUM_L3_NODES_CPU_PORT;
    }
    else if (lcl_port == HR4_MMU_LPBK_PORT_NUM) {
        *number_l3_mc_nodes = HR4_MMU_NUM_L3_NODES_LPBK_PORT;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified for"
                            " Getting L3MC Offset. "
                            "Specified value is greater than 71")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_mmu_get_valid_ipipes_for_xpe(int unit, int xpe,
 *              int *ipipes[SOC_MAX_NUM_PIPES])
 *  @param unit Device number
 *  @param XPE XPE number
 *  @param *epipes Returns Valid Ipipes for XPE
 *  @brief API to provide a XPE's valid ipipe numbers
 *         Returns an error if xpe num > 0
 */
int
soc_hurricane4_mmu_get_valid_ipipes_for_xpe(int unit, int xpe,
        int *ipipes)
{
    int i;
    for (i=0; i<SOC_MAX_NUM_PIPES; i++)
    {
        ipipes[i]= 0;
    }
    if (xpe==0)
    {
        ipipes[0] = 1;
        ipipes[1] = 1;
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid XPE number specified. "
                            "Specified XPE greater than 0")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_valid_epipes_for_xpe(int unit, int xpe,
 *              int *epipes[SOC_MAX_NUM_PIPES])
 *  @param unit Device number
 *  @param XPE XPE number
 *  @param *epipes Returns Valid Epipes for XPE
 *  @brief API to provide a XPE's valid epipe numbers
 *         Returns an error if xpe num > 0
 */
int
soc_hurricane4_mmu_get_valid_epipes_for_xpe(int unit, int xpe,
        int *epipes)
{
    int i;
    for (i=0; i<SOC_MAX_NUM_PIPES; i++)
    {
        epipes[i]= 0;
    }
    if (xpe==0)
    {
        epipes[0] = 1;
        epipes[1] = 1;
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid XPE number specified. "
                            "Specified XPE greater than 0")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_l0_base_offset_for_port(int unit,
 *               int lcl_port, int *l0_offset)
 *  @param unit Device number
 *  @param lclport Pipe's local MMU Port Number
 *  @param *l0_offset Returned L0 offset for the port
 *  @brief API to provide a pipe's local mmu port's L0 base offset which will be
 *         used to access the L0 Memories in VBS & MTRO.
 *         Returns invalid number is port number > 71 (Last valid port number in
 *         a pipe)
 */
int
soc_hurricane4_mmu_get_l0_base_offset_for_port(int unit, int lcl_port,
        int *l0_offset)
{
    *l0_offset = 0;
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *l0_offset = lcl_port * HR4_MMU_NUM_L0_NODES_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *l0_offset = HR4_MMU_NUM_UP_PORTS*HR4_MMU_NUM_L0_NODES_PER_HSP_UP_PORT +
            (lcl_port-HR4_MMU_NUM_UP_PORTS)*HR4_MMU_NUM_L0_NODES_PER_HSP_DL_PORT;
    }
    else if (lcl_port == HR4_MMU_CPU_PORT_NUM) {
        *l0_offset = HR4_MMU_CPU_PORT_L0_START;
    }
    else if (lcl_port == HR4_MMU_LPBK_PORT_NUM) {
        *l0_offset = HR4_MMU_LPBK_PORT_L0_START;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified"
                            "for Getting L0 Offset. "
                            "Specified value is greater than 71")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_mmu_get_l1_base_offset_for_port(int unit,
 *               int lcl_port, int *l1_offset)
 *  @param unit Device number
 *  @param lclport Pipe's local MMU Port Number
 *  @param *l1_offset Returned L1 offset for the port
 *  @brief API to provide a pipe's local mmu port's L1 base offset which will be
 *         used to access the l1 Memories in VBS & MTRO.
 *         Returns invalid number is port number > 71 (Last valid port number in
 *         a pipe)
 */
int
soc_hurricane4_mmu_get_l1_base_offset_for_port(int unit, int lcl_port,
        int *l1_offset)
{
    *l1_offset = 0;
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *l1_offset = lcl_port * HR4_MMU_NUM_L1_NODES_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *l1_offset = HR4_MMU_NUM_UP_PORTS*HR4_MMU_NUM_L1_NODES_PER_HSP_UP_PORT +
            (lcl_port-HR4_MMU_NUM_UP_PORTS)*HR4_MMU_NUM_L1_NODES_PER_HSP_DL_PORT;
    }
    else if (lcl_port == HR4_MMU_CPU_PORT_NUM) {
        *l1_offset = HR4_MMU_CPU_PORT_L1_START;
    }
    else if (lcl_port == HR4_MMU_LPBK_PORT_NUM) {
        *l1_offset = HR4_MMU_LPBK_PORT_L1_START;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified"
                            "for Getting L1 Offset. "
                            "Specified value is greater than 71")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_mmu_get_l2_base_offset_for_port(int unit,
 *               int lcl_port, int *l2_offset)
 *  @param unit Device number
 *  @param lclport Pipe's local MMU Port Number
 *  @param *l2_offset Returned L2 offset for the port
 *  @brief API to provide a pipe's local mmu port's L2 base offset which will be
 *         used to access the L2 Memories in VBS & MTRO.
 *         Returns invalid number is port number > 71 (Last valid port number in
 *         a pipe)
 */
int
soc_hurricane4_mmu_get_l2_base_offset_for_port(int unit, int lcl_port,
        int *l2_offset)
{
    *l2_offset = 0;
    if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
        *l2_offset = lcl_port * HR4_MMU_NUM_L2_NODES_PER_HSP_UP_PORT;
    }
    else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
        *l2_offset = HR4_MMU_NUM_UP_PORTS*HR4_MMU_NUM_L2_NODES_PER_HSP_UP_PORT +
            (lcl_port-HR4_MMU_NUM_UP_PORTS)*HR4_MMU_NUM_L2_NODES_PER_HSP_DL_PORT;
    }
    else if (lcl_port == HR4_MMU_CPU_PORT_NUM) {
        *l2_offset = HR4_MMU_CPU_PORT_L2_START;
    }
    else if (lcl_port == HR4_MMU_LPBK_PORT_NUM) {
        *l2_offset = HR4_MMU_LPBK_PORT_L2_START;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Invalid MMU Port Specified"
                            "for Getting L2 Offset. "
                            "Specified value is greater than 71")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_l3_base_offset_for_port(int unit,
 *               int lcl_port, int uc_bit, int *l3_offset)
 *  @param unit Device number
 *  @param lcl_port Pipe's local MMU Port Number
 *  @param uc_bit Unicast bit or Not to help calculate L3 offset.
 *  @param *l3_offset Returned L3 offset for the port
 *  @brief API to provide a pipe's local mmu port's L3 base offset which will be
 *         used to access the L3 Memories in VBS & MTRO depending on the unicast
 *         bit.
 *         Returns invalid number is port number > 33 (Last valid port number in
 *         a pipe)
 */
int
soc_hurricane4_mmu_get_l3_base_offset_for_port(int unit, int lcl_port,
        int uc_bit,
        int *l3_offset)
{
    *l3_offset = 0;
    if (uc_bit == 1) {
        if (lcl_port < HR4_MMU_NUM_UP_PORTS)
        {
            *l3_offset = lcl_port * HR4_MMU_NUM_UCQ_PER_HSP_UP_PORT;
        }
        else if (lcl_port < HR4_MMU_CPU_PORT_NUM)
        {
            *l3_offset =HR4_MMU_NUM_UP_PORTS*HR4_MMU_NUM_UCQ_PER_HSP_UP_PORT +
                (lcl_port-HR4_MMU_NUM_UP_PORTS)*HR4_MMU_NUM_UCQ_PER_HSP_DL_PORT;
        }
        else {
            return SOC_E_FAIL;
        }
    }
    else {
        if (lcl_port < HR4_MMU_NUM_UP_PORTS) {
            *l3_offset = HR4_MMU_MCQ_NUMBER_START +
                (lcl_port * HR4_MMU_NUM_MCQ_PER_HSP_UP_PORT);
        }
        else if (lcl_port < HR4_MMU_CPU_PORT_NUM) {
            *l3_offset = HR4_MMU_MCQ_NUMBER_START + (HR4_MMU_NUM_UP_PORTS*HR4_MMU_NUM_MCQ_PER_HSP_UP_PORT + (lcl_port-HR4_MMU_NUM_UP_PORTS)*HR4_MMU_NUM_MCQ_PER_HSP_DL_PORT);
        }
        else if (lcl_port == HR4_MMU_LPBK_PORT_NUM) {
            *l3_offset = HR4_MMU_LPBK_PORT_L3_START;
        }
        else if (lcl_port == HR4_MMU_CPU_PORT_NUM) {
            *l3_offset = HR4_MMU_CPU_PORT_L3_START;
        }
        else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Invalid MMU Port Specified for Getting L3 Offset. "
                                "Specified value is greater than 71")));
            return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_return_dip_delay_amount(int unit, int port_speed,
 *              int *dip_amount)
 *  @param unit Device number
 *  @param port_speed Speed in Mbps
 *  @param *dip_amount Dip Delay Amount returned
 *  @brief API to return the CT decoded setting for a port speed based on the
 *         values mentioned in the register ASF_IPORT_CFG
 */
int
soc_hurricane4_mmu_return_dip_delay_amount(int unit, int port_speed,
        uint64 *dip_amount)
{
    int rand_num;
    unsigned int seed=0;

    if (seed == 0) {
        seed= sal_time();
        sal_srand(seed);
    }

    rand_num = (unsigned int) sal_rand() % 20; /* Skew value */
    switch(port_speed)
    {
        case 10000:
            COMPILER_64_SET(*dip_amount,0, (15+ rand_num));
            break;
        case 20000:
            COMPILER_64_SET(*dip_amount, 0, (30+ rand_num));
            break;
        case 25000:
            COMPILER_64_SET(*dip_amount, 0, (40+ rand_num));
            break;
        case 40000:
            COMPILER_64_SET(*dip_amount, 0, (60+ rand_num));
            break;
        case 50000:
            COMPILER_64_SET(*dip_amount, 0, (80+ rand_num));
            break;
        case 100000:
            COMPILER_64_SET(*dip_amount, 0, (140+ rand_num));
            break;
        default:
            COMPILER_64_SET(*dip_amount, 0, 0);
            return SOC_E_FAIL;
            break;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_get_min_max_src_ct_speed(int unit, int speed_decode,
 *              int *min_speed, int *max_speed)
 *  @param unit Device number
 *  @param speed_decode Decoded speed value based on the function
 *         soc_hurricane4_mmu_decode_speed_setting
 *  @brief API to return the Minimum and Maximum src port speeds allowed to
 *         cut-through to a destination port. The population of these values
 *         is done in the soc_hurricane4_mmu_asf_cfg_tbl struct
 */
int
soc_hurricane4_get_min_max_src_ct_speed(int unit, int speed_decode,
        int ct_mode, int ct_profile,
        uint64 *min_speed,
        uint64 *max_speed)
{
    if ((ct_profile == _SOC_ASF_MEM_PROFILE_SIMILAR) ||
            (ct_profile == _SOC_ASF_MEM_PROFILE_EXTREME)) {
        if (ct_mode == _SOC_ASF_MODE_SAME_SPEED) {
            COMPILER_64_SET(*min_speed, 0,  speed_decode);
            COMPILER_64_SET(*max_speed, 0,  speed_decode);
        }
        else if (ct_mode == _SOC_ASF_MODE_SLOW_TO_FAST) {
            COMPILER_64_SET(*min_speed, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[speed_decode].min_sp);
            COMPILER_64_SET(*max_speed, 0,  speed_decode);
        }
        else if (ct_mode == _SOC_ASF_MODE_FAST_TO_SLOW) {
            COMPILER_64_SET(*min_speed, 0,  speed_decode);
            if (ct_profile == _SOC_ASF_MEM_PROFILE_SIMILAR) {
                if ((speed_decode % 2 == 0)
                        && (speed_decode > 0)) {
                    COMPILER_64_SET(*max_speed, 0,  speed_decode);
                }
                else {
                    COMPILER_64_SET(*max_speed, 0,  (speed_decode+1));
                }
            }
            else {
                COMPILER_64_SET(*max_speed, 0,
                        (soc_hurricane4_mmu_asf_cfg_tbl[speed_decode].max_sp));
            }
        }
    }

    else {
        COMPILER_64_SET(*min_speed, 0,  0);
        COMPILER_64_SET(*max_speed, 0,  0);
    }

    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_get_asf_xmit_start_count(int unit, int dst_decode,
 *              int ct_mode, int *asf_count)
 *  @param unit Device number
 *  @param dst_decode Decoded speed value based on the function
 *         soc_hurricane4_mmu_decode_speed_setting
 *  @param CT Mode Cut_through Mode
 *  @brief API to return the transmit start count for a port depending on
 *         its speed.
 */
int
soc_hurricane4_get_asf_xmit_start_count(int unit, int dst_decode, int ct_mode,
        uint64 *asf_count)
{
    if (dst_decode > HR4_MMU_NUM_CT_SPEED_GROUPS) {
        return SOC_E_FAIL;
    }

    if (ct_mode == _SOC_ASF_MODE_SAME_SPEED || ct_mode ==
            _SOC_ASF_MODE_FAST_TO_SLOW) {
        COMPILER_64_SET(*asf_count, 0, 0);
    }
    else if (ct_mode == _SOC_ASF_MODE_SLOW_TO_FAST) {
        COMPILER_64_SET(*asf_count, 0,
                soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].mmu_prebuffer);
    }
    else {
        COMPILER_64_SET(*asf_count, 0, 0);
    }
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_get_ct_fifo_threshold_depth(int unit, int dst_decode,
 *      int ct_mode, int oversub, int *ct_fifo_thresh, int *ct_fifo_depth)
 *  @param unit Device number
 *  @param dst_decode Decoded speed value based on the function
 *         soc_hurricane4_mmu_decode_speed_setting
 *  @param ct_Mode Cut_through Mode
 *  @param oversub Oversub TDM or LR tdm
 *  @param *fifo_depth CT Fifo Threshold Depth Value
 *  @brief API to return the transmit start count for a port depending on its
 *         speed.
 */
int
soc_hurricane4_get_ct_fifo_threshold_depth(int unit, int dst_decode, int ct_mode,
        int oversub,
        uint64 *ct_fifo_thresh,
        uint64 *ct_fifo_depth)
{
    uint64 temp64;

    if (dst_decode > HR4_MMU_NUM_CT_SPEED_GROUPS) {
        return SOC_E_FAIL;
    }
    if (oversub == 0) {
        if (ct_mode == _SOC_ASF_MODE_SAME_SPEED) {
            COMPILER_64_SET(*ct_fifo_thresh, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_threshold_lr);
            COMPILER_64_SET(*ct_fifo_depth, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_lr);
        }
        else if (ct_mode == _SOC_ASF_MODE_FAST_TO_SLOW) {
            COMPILER_64_SET(*ct_fifo_thresh, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_threshold_lr);
            COMPILER_64_SET(*ct_fifo_depth, 0,
                    (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_lr+
                     soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].fast_to_slow_accum));
            COMPILER_64_ZERO(temp64);
            COMPILER_64_ADD_32(temp64,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_lr);
            COMPILER_64_ADD_32(temp64, HR4_MAX_NUM_CELLS);
            if (COMPILER_64_GT(*ct_fifo_depth, temp64)) {
                COMPILER_64_SET(*ct_fifo_depth, 0,
                        (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_lr+
                         HR4_MAX_NUM_CELLS));
            }

        }
        else if (ct_mode == _SOC_ASF_MODE_SLOW_TO_FAST) {
            COMPILER_64_SET(*ct_fifo_thresh, 0,
                    (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_threshold_lr+
                     soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].mmu_prebuffer));
            COMPILER_64_SET(*ct_fifo_depth, 0,
                    (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_lr+
                     soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].mmu_prebuffer));

        }
        else {
            COMPILER_64_SET(*ct_fifo_thresh, 0, 0);
            COMPILER_64_SET(*ct_fifo_depth, 0, 0);

        }
    }
    else {
        if (ct_mode == _SOC_ASF_MODE_SAME_SPEED) {
            COMPILER_64_SET(*ct_fifo_thresh, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_threshold_os);
            COMPILER_64_SET(*ct_fifo_depth, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_os);

        }
        else if (ct_mode == _SOC_ASF_MODE_FAST_TO_SLOW) {
            COMPILER_64_SET(*ct_fifo_thresh, 0,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_threshold_os);
            COMPILER_64_SET(*ct_fifo_depth, 0,
                    (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_os+
                     soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].fast_to_slow_accum));
            COMPILER_64_ZERO(temp64);
            COMPILER_64_ADD_32(temp64,
                    soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_os);
            COMPILER_64_ADD_32(temp64, HR4_MAX_NUM_CELLS);
            if (COMPILER_64_GT(*ct_fifo_depth, temp64)) {
                COMPILER_64_SET(*ct_fifo_depth, 0,
                        (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_os+
                         HR4_MAX_NUM_CELLS));
            }

        }
        else if (ct_mode == _SOC_ASF_MODE_SLOW_TO_FAST) {
            COMPILER_64_SET(*ct_fifo_thresh, 0,
                    (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_threshold_os+
                     soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].mmu_prebuffer));
            COMPILER_64_SET(*ct_fifo_depth, 0,
                    (soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].ct_fifo_depth_os+
                     soc_hurricane4_mmu_asf_cfg_tbl[dst_decode].mmu_prebuffer));

        }
        else {
            COMPILER_64_SET(*ct_fifo_thresh, 0, 0);
            COMPILER_64_SET(*ct_fifo_depth, 0, 0);

        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_mmu_get_bst_mode(int unit, int *bst_mode)
 *  @param unit Device number
 *  @param *bst_mode Return value bst mode
 *  @brief API to return the bst mode programmed
 */
int
soc_hurricane4_mmu_get_bst_mode(int unit, int *bst_mode)
{
    uint64 mmu_rval64;
    uint32 mmu_rval32;

    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, MMU_GCFG_MISCCONFIGr,
                REG_PORT_ANY, 0, &mmu_rval64));
    mmu_rval32 = COMPILER_64_LO(mmu_rval64);
    *bst_mode = soc_reg_field_get(unit, MMU_GCFG_MISCCONFIGr, mmu_rval32,
            BST_TRACKING_MODEf);
    return SOC_E_NONE;
}

/*! @fn int soc_hurricane4_mmu_get_pktstat_mode(int unit, int *pktstat_mode)
 *  @param unit Device number
 *  @brief API to return the pktstat mode programmed
 */
int
soc_hurricane4_mmu_get_pktstat_mode(int unit, int *pktstat_mode)
{
    uint64 mmu_rval64;
    uint32 mmu_rval32;

    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, MMU_GCFG_PKTSTAT_OOBSTATr,
                REG_PORT_ANY, 0, &mmu_rval64));
    mmu_rval32 = COMPILER_64_LO(mmu_rval64);
    *pktstat_mode =
        soc_reg_field_get(unit, MMU_GCFG_PKTSTAT_OOBSTATr, mmu_rval32,
                HWM_MODEf);
    /*  *pktstat_mode &=
        soc_reg_field_get(unit, MMU_GCFG_PKTSTAT_OOBSTATr, mmu_rval32,
        SELECTf); */
    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_ct_ep_credit_low_high_threshold(int unit,
 *              int speed_decode, int frequency, int oversub_ratio,
 *              int *ep_credit_low, int *ep_credit_hi)
 *  @param unit Device number
 *  @param frequency Core clock frequency
 *  @param ratio oversub ratio in the TDM
 *  @param *ep_credit_low EP_Credit Low value
 *  @param *ep_credit_hi High EP credit Value
 *  @brief API to return the EP Credit Low and high thresholds for a particular
 *         CT Setting
 */
int
soc_hurricane4_mmu_get_ct_ep_credit_low_high_threshold(int  unit,
        int  speed_decode,
        int  frequency,
        int  oversub_ratio,
        int  oversub,
        uint64 *ep_credit_low,
        uint64 *ep_credit_hi)
{
    if (speed_decode > HR4_MMU_NUM_CT_SPEED_GROUPS) {
        return SOC_E_FAIL;
    }
    if (frequency< 1525 ||
            ((frequency == 1525) && (oversub ==1) && (oversub_ratio > 15))) {
        COMPILER_64_SET(*ep_credit_low, 0,
                soc_hurricane4_mmu_asf_cfg_tbl[speed_decode].ep_credit_low);
        COMPILER_64_SET(*ep_credit_hi, 0,
                soc_hurricane4_mmu_asf_cfg_tbl[speed_decode].ep_credit_hi);
    }
    else {
        COMPILER_64_SET(*ep_credit_low, 0,
                soc_hurricane4_mmu_asf_cfg_tbl[speed_decode].ep_credit_low_1dot5);
        COMPILER_64_SET(*ep_credit_hi, 0,
                soc_hurricane4_mmu_asf_cfg_tbl[speed_decode].ep_credit_hi_1dot5);
    }


    return SOC_E_NONE;
}


/*! @fn int soc_hurricane4_mmu_get_pipe_flexed_status(int unit,
 *              soc_port_resource_t *port_resource_t, int pipe, int *pipe_flexed)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param pipe Pipe to check
 *  @param *pipe_flexed Pipe Flexed status,
 *             0 = 'No change on ay Ports in this Pipe'
 *             1 = 'One of more Ports in this Pipe has changed configuration'
 *  @brief API to return the Flexed status of a particular pipe
 */
int
soc_hurricane4_mmu_get_pipe_flexed_status(int                         unit,
        soc_port_schedule_state_t  *port_schedule_state_t,
        int                         pipe,
        int                        *pipe_flexed)
{
    int port;

    *pipe_flexed = 0;

    for (port=0; port<port_schedule_state_t->nport; port++) {
      if (port_schedule_state_t->resource[port].pipe == pipe) {
          *pipe_flexed = 1;
          break;
      }
    }

    return SOC_E_NONE;
}
#endif
