/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_idb_flexport.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/init/tomahawk3_idb_init.h>
#include <soc/flexport/tomahawk3_idb_flexport.h>
#include <soc/flexport/flexport_common.h>



/*** START SDK API COMMON CODE ***/


/*! @file tomahawk3_idb_flexport.c
 *  @brief IDB flex for Tomahawk3.
 *  Details are shown below.
 */

/*! @fn int soc_tomahawk3_flex_set_epc_link_bmap(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *                             variable.
 *  @brief Helper function to Write EPC_LINK_BMAP table to disable forwarding traffic
 *         while port goes down during the flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_set_epc_link_bmap(
       int unit,
       soc_port_schedule_state_t *port_schedule_state,
       int is_port_down)
{
    uint32 epc_link_bmap_tbl[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    uint32 mask;
    int lport;
    int i;

    /* EPC_LINK_BMAP read, field modify and write. */
    sal_memset(epc_link_bmap_tbl, 0, sizeof(epc_link_bmap_tbl));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                     epc_link_bmap_tbl));
    soc_mem_field_get(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "EPC_LINK_BITMAP READ:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    mask = 0xffffffff;

    /* Make EPC_LINK_BMAP=0 for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if(1 == is_port_down) { /* PORT DOWN: disable port in EPC_LINK_BMAP */
            if (port_schedule_state->resource[i].physical_port == -1) {
                lport = port_schedule_state->resource[i].logical_port;
                memfld[(lport >> 5)] &= (mask ^ (0x1 << (lport & 0x1f)));
            }
        } else { /* PORT DOWN: enable port in EPC_LINK_BMAP */
            if (port_schedule_state->resource[i].physical_port != -1) {
                lport = port_schedule_state->resource[i].logical_port;
                memfld[(lport >> 5)] |= (0x1 << (lport & 0x1f));
            }
        }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "%s EPC_LINK_BITMAP WRITE:: %x %x %x %x %x \n"),
                          (is_port_down == 1) ? "Disable" : "Enable",
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    soc_mem_field_set(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                      epc_link_bmap_tbl));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_set_dest_port_bmap(int unit,
                soc_port_schedule_state_t *port_schedule_state,
 *              int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to Write ING_DEST_PORT_ENABLE table to disable
           forwarding traffic for the ports going down during the flexport
           operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_set_dest_port_bmap(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int is_port_down)
{
    uint32 ing_dst_p_en_tbl[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    uint32 mask;
    int lport;
    int i;

    /* ING_DEST_PORT_ENABLE read, field modify and write. */
    sal_memset(ing_dst_p_en_tbl, 0, sizeof(ing_dst_p_en_tbl));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL,
                                     0, ing_dst_p_en_tbl));
    soc_mem_field_get(unit, ING_DEST_PORT_ENABLEm, ing_dst_p_en_tbl,
                      PORT_BITMAPf,
                      memfld);
    mask = 0xffffffff;
    /* Make ING_DEST_PORT_ENABLEm=0 for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if(1 == is_port_down) { /* PORT DOWN: disable port in ING_DEST_PORT_ENABLE */
            if (port_schedule_state->resource[i].physical_port == -1) {
                lport = port_schedule_state->resource[i].logical_port;
                memfld[(lport>>5)] &= (mask^(1<<(lport&0x1f)));
            }
        } else { /* PORT DOWN: enable port in ING_DEST_PORT_ENABLE */
            if (port_schedule_state->resource[i].physical_port != -1) {
                lport = port_schedule_state->resource[i].logical_port;
                memfld[(lport >> 5)] |= (0x1 << (lport & 0x1f));
            }
        }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "%s ING_DEST_PORT_ENABLE WRITE:: %x %x %x %x %x \n"),
                          (is_port_down == 1) ? "Disable" : "Enable",
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));

    soc_mem_field_set(unit, ING_DEST_PORT_ENABLEm, ing_dst_p_en_tbl,
                      PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_DEST_PORT_ENABLEm,
                                      MEM_BLOCK_ALL, 0, ing_dst_p_en_tbl));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_dis_forwarding_traffic(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to diable forwarding traffic.
           Calls soc_tomahawk3_dis_epc_link_bmap &
           soc_tomahawk3_dis_dest_port_bmap.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_dis_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int wait_us, is_port_down;

    is_port_down = 1;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_set_epc_link_bmap(
        unit, port_schedule_state, is_port_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_set_dest_port_bmap(
        unit, port_schedule_state, is_port_down));

    /* Wait is 8 msec if either Management port is configured as 10/100 Kbps,
     * else 80 us. */
    if ((port_schedule_state->in_port_map.log_port_speed[_TH3_LOG_PORT_MNG0] <=
         100) ||
        (port_schedule_state->in_port_map.log_port_speed[_TH3_LOG_PORT_MNG1] <=
         100)) {
        wait_us = 8000;
    } else {
        wait_us = 80;
    }
    #ifdef DV_ONLY
    wait_us = 1; /* ONly for DV*/
    #endif
    sal_usleep(wait_us + (SAL_BOOT_QUICKTURN ? 1: 0) * EMULATION_FACTOR);
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_flex_en_forwarding_traffic(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to enable forwarding traffic.
           Calls soc_tomahawk3_dis_epc_link_bmap &
           soc_tomahawk3_dis_dest_port_bmap.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_en_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int is_port_down;

    is_port_down = 0;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_set_epc_link_bmap(
        unit, port_schedule_state, is_port_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_set_dest_port_bmap(
        unit, port_schedule_state, is_port_down));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_flex_idb_invalidate_pnum_mapping_tbl(int unit,
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
           variable
 *  @brief Helper function to invalidate phy to idb port mapping table
           (ING_PHY_TO_IDB_PORT_MAP) entries for the ports that are going down.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_idb_invalidate_pnum_mapping_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int pipe, phy_port, lport, i;
    const soc_mem_t th3_pnum_map_tbl[_TH3_PIPES_PER_DEV] = {
        ING_PHY_TO_IDB_PORT_MAP_PIPE0m, ING_PHY_TO_IDB_PORT_MAP_PIPE1m,
        ING_PHY_TO_IDB_PORT_MAP_PIPE2m, ING_PHY_TO_IDB_PORT_MAP_PIPE3m,
        ING_PHY_TO_IDB_PORT_MAP_PIPE4m, ING_PHY_TO_IDB_PORT_MAP_PIPE5m,
        ING_PHY_TO_IDB_PORT_MAP_PIPE6m, ING_PHY_TO_IDB_PORT_MAP_PIPE7m
    };

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            lport = port_schedule_state->resource[i].logical_port;
            sal_memset(entry, 0, sizeof(entry));
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[lport];
            pipe = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            mem = th3_pnum_map_tbl[pipe];
            memfld = 0;
            soc_mem_field_set(unit, mem, entry, IDB_PORTf, &memfld);
            memfld = 0;
            soc_mem_field_set(unit, mem, entry, VALIDf, &memfld);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                (phy_port - 1 - (pipe * _TH3_GPHY_PORTS_PER_PIPE)), entry));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_idb_init_pnum_mapping_tbl(int unit, 
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
           variable
 *  @brief Helper function to initialize phy to idb port mapping table for the
 *         ports comming up during flexport operations.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_idb_init_pnum_mapping_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int lport;
    int i;

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            lport = port_schedule_state->resource[i].logical_port;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_init_pnum_mapping_tbl(
                unit, &port_schedule_state->out_port_map,lport));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_idb_invalidate_idb_to_dev_pmap_tbl(int unit,
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
           variable
 *  @brief Helper function to invalidate phy to idb port mapping table
           (ING_IDB_TO_DEVICE_PORT_MAP) entries for the ports that are
           going down.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_idb_invalidate_idb_to_dev_pmap_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int pipe, phy_port, lport, i;
    int idb_port;
    const soc_mem_t th3_idb_to_dev_pmap_tbl[_TH3_PIPES_PER_DEV] = {
        ING_IDB_TO_DEVICE_PORT_MAP_PIPE0m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE1m,
        ING_IDB_TO_DEVICE_PORT_MAP_PIPE2m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE3m,
        ING_IDB_TO_DEVICE_PORT_MAP_PIPE4m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE5m,
        ING_IDB_TO_DEVICE_PORT_MAP_PIPE6m, ING_IDB_TO_DEVICE_PORT_MAP_PIPE7m
    };

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            lport = port_schedule_state->resource[i].logical_port;
            sal_memset(entry, 0, sizeof(entry));
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[lport];
            idb_port =
                port_schedule_state->in_port_map.port_l2i_mapping[lport];
            pipe = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            mem = th3_idb_to_dev_pmap_tbl[pipe];
            memfld = 0;
            soc_mem_field_set(unit, mem, entry, DEVICE_PORTf, &memfld);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                                       idb_port, entry));
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_flex_idb_init_idb_to_dev_pmap_tbl(int unit, 
                soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
           variable
 *  @brief Helper function to initialize IDB to DEVICE port mapping table
           for the ports comming up during flexport operations.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_idb_init_idb_to_dev_pmap_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int lport;
    int i;

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            lport = port_schedule_state->resource[i].logical_port;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_init_idb_to_dev_pmap_tbl(
                unit, &port_schedule_state->out_port_map,lport));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_idb_clear_stats_new_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to clear statistics of the new ports that are comming
 *         up during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_idb_clear_stats_new_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int pnum;
    int pipe_num;
    int pm_num;
    int subp;
    int usage_reg_offset;
    const soc_reg_t obm_usage_regs[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE][3]
        = {{{IDB_OBM0_MAX_USAGE_PIPE0r, IDB_OBM0_MAX_USAGE_1_PIPE0r,
             IDB_OBM0_MAX_USAGE_2_PIPE0r},
            {IDB_OBM1_MAX_USAGE_PIPE0r, IDB_OBM1_MAX_USAGE_1_PIPE0r,
             IDB_OBM1_MAX_USAGE_2_PIPE0r},
            {IDB_OBM2_MAX_USAGE_PIPE0r, IDB_OBM2_MAX_USAGE_1_PIPE0r,
             IDB_OBM2_MAX_USAGE_2_PIPE0r},
            {IDB_OBM3_MAX_USAGE_PIPE0r, IDB_OBM3_MAX_USAGE_1_PIPE0r,
             IDB_OBM3_MAX_USAGE_2_PIPE0r}},
           {{IDB_OBM0_MAX_USAGE_PIPE1r, IDB_OBM0_MAX_USAGE_1_PIPE1r,
             IDB_OBM0_MAX_USAGE_2_PIPE1r},
            {IDB_OBM1_MAX_USAGE_PIPE1r, IDB_OBM1_MAX_USAGE_1_PIPE1r,
             IDB_OBM1_MAX_USAGE_2_PIPE1r},
            {IDB_OBM2_MAX_USAGE_PIPE1r, IDB_OBM2_MAX_USAGE_1_PIPE1r,
             IDB_OBM2_MAX_USAGE_2_PIPE1r},
            {IDB_OBM3_MAX_USAGE_PIPE1r, IDB_OBM3_MAX_USAGE_1_PIPE1r,
             IDB_OBM3_MAX_USAGE_2_PIPE1r}},
           {{IDB_OBM0_MAX_USAGE_PIPE2r, IDB_OBM0_MAX_USAGE_1_PIPE2r,
             IDB_OBM0_MAX_USAGE_2_PIPE2r},
            {IDB_OBM1_MAX_USAGE_PIPE2r, IDB_OBM1_MAX_USAGE_1_PIPE2r,
             IDB_OBM1_MAX_USAGE_2_PIPE2r},
            {IDB_OBM2_MAX_USAGE_PIPE2r, IDB_OBM2_MAX_USAGE_1_PIPE2r,
             IDB_OBM2_MAX_USAGE_2_PIPE2r},
            {IDB_OBM3_MAX_USAGE_PIPE2r, IDB_OBM3_MAX_USAGE_1_PIPE2r,
             IDB_OBM3_MAX_USAGE_2_PIPE2r}},
           {{IDB_OBM0_MAX_USAGE_PIPE3r, IDB_OBM0_MAX_USAGE_1_PIPE3r,
             IDB_OBM0_MAX_USAGE_2_PIPE3r},
            {IDB_OBM1_MAX_USAGE_PIPE3r, IDB_OBM1_MAX_USAGE_1_PIPE3r,
             IDB_OBM1_MAX_USAGE_2_PIPE3r},
            {IDB_OBM2_MAX_USAGE_PIPE3r, IDB_OBM2_MAX_USAGE_1_PIPE3r,
             IDB_OBM2_MAX_USAGE_2_PIPE3r},
            {IDB_OBM3_MAX_USAGE_PIPE3r, IDB_OBM3_MAX_USAGE_1_PIPE3r,
             IDB_OBM3_MAX_USAGE_2_PIPE3r}},
           {{IDB_OBM0_MAX_USAGE_PIPE4r, IDB_OBM0_MAX_USAGE_1_PIPE4r,
             IDB_OBM0_MAX_USAGE_2_PIPE4r},
            {IDB_OBM1_MAX_USAGE_PIPE4r, IDB_OBM1_MAX_USAGE_1_PIPE4r,
             IDB_OBM1_MAX_USAGE_2_PIPE4r},
            {IDB_OBM2_MAX_USAGE_PIPE4r, IDB_OBM2_MAX_USAGE_1_PIPE4r,
             IDB_OBM2_MAX_USAGE_2_PIPE4r},
            {IDB_OBM3_MAX_USAGE_PIPE4r, IDB_OBM3_MAX_USAGE_1_PIPE4r,
             IDB_OBM3_MAX_USAGE_2_PIPE4r}},
           {{IDB_OBM0_MAX_USAGE_PIPE5r, IDB_OBM0_MAX_USAGE_1_PIPE5r,
             IDB_OBM0_MAX_USAGE_2_PIPE5r},
            {IDB_OBM1_MAX_USAGE_PIPE5r, IDB_OBM1_MAX_USAGE_1_PIPE5r,
             IDB_OBM1_MAX_USAGE_2_PIPE5r},
            {IDB_OBM2_MAX_USAGE_PIPE5r, IDB_OBM2_MAX_USAGE_1_PIPE5r,
             IDB_OBM2_MAX_USAGE_2_PIPE5r},
            {IDB_OBM3_MAX_USAGE_PIPE5r, IDB_OBM3_MAX_USAGE_1_PIPE5r,
             IDB_OBM3_MAX_USAGE_2_PIPE5r}},
           {{IDB_OBM0_MAX_USAGE_PIPE6r, IDB_OBM0_MAX_USAGE_1_PIPE6r,
             IDB_OBM0_MAX_USAGE_2_PIPE6r},
            {IDB_OBM1_MAX_USAGE_PIPE6r, IDB_OBM1_MAX_USAGE_1_PIPE6r,
             IDB_OBM1_MAX_USAGE_2_PIPE6r},
            {IDB_OBM2_MAX_USAGE_PIPE6r, IDB_OBM2_MAX_USAGE_1_PIPE6r,
             IDB_OBM2_MAX_USAGE_2_PIPE6r},
            {IDB_OBM3_MAX_USAGE_PIPE6r, IDB_OBM3_MAX_USAGE_1_PIPE6r,
             IDB_OBM3_MAX_USAGE_2_PIPE6r}},
           {{IDB_OBM0_MAX_USAGE_PIPE7r, IDB_OBM0_MAX_USAGE_1_PIPE7r,
             IDB_OBM0_MAX_USAGE_2_PIPE7r},
            {IDB_OBM1_MAX_USAGE_PIPE7r, IDB_OBM1_MAX_USAGE_1_PIPE7r,
             IDB_OBM1_MAX_USAGE_2_PIPE7r},
            {IDB_OBM2_MAX_USAGE_PIPE7r, IDB_OBM2_MAX_USAGE_1_PIPE7r,
             IDB_OBM2_MAX_USAGE_2_PIPE7r},
            {IDB_OBM3_MAX_USAGE_PIPE7r, IDB_OBM3_MAX_USAGE_1_PIPE7r,
             IDB_OBM3_MAX_USAGE_2_PIPE7r}}};
    const soc_field_t idb_obm_max_usage_fields[] = {
        PORT0_MAX_USAGEf, PORT1_MAX_USAGEf, PORT2_MAX_USAGEf, PORT3_MAX_USAGEf,
        PORT4_MAX_USAGEf, PORT5_MAX_USAGEf, PORT6_MAX_USAGEf, PORT7_MAX_USAGEf
    };

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            pnum = port_schedule_state->resource[i].physical_port;
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(pnum);
            pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(pnum);
            subp = soc_tomahawk3_get_subp_from_phy_port(pnum);
            if (subp<=2) {
               usage_reg_offset=0;
            }else if (subp<=5) {
               usage_reg_offset=1;
            }else{
               usage_reg_offset=2;
            }
            reg = obm_usage_regs[pipe_num][pm_num][usage_reg_offset];
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                REG_PORT_ANY, 0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, reg, &rval64,
                idb_obm_max_usage_fields[subp], fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                REG_PORT_ANY, 0, rval64));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_idb_reconfigure(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to reconfigure IDB during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_flex_idb_reconfigure(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int pmnum;
    int pm_going_up;
    int i;
    int lport;
    int lport_save;
    int phy_port;
    int reset;
    int pipe_num;
    int pipe_going_up;

    /* Re-configure IDB scheduler */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_idb_flexport(unit,
                                                       port_schedule_state));
    /* Invalidate ING_IDB_TO_DEVICE_PORT_MAP entries*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_idb_invalidate_pnum_mapping_tbl(unit,
                                                       port_schedule_state));
    /* Re-configure ING_IDB_TO_DEVICE_PORT_MAP entries*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_idb_init_pnum_mapping_tbl(unit,
                                                       port_schedule_state));
    /* Invalidate ING_PHY_TO_IDB_PORT_MAP entries*/
    /*SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_idb_invalidate_idb_to_dev_pmap_tbl(unit,
                                                       port_schedule_state));*/
    /* Re-configure ING_PHY_TO_IDB_PORT_MAP entries*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_idb_init_idb_to_dev_pmap_tbl(unit,
                                                       port_schedule_state));

    /* If multiple ports going up in same PM, write only once for the
       general registers that are common to all sub-ports. */
    for (pmnum = 0; pmnum < _TH3_NUM_OF_TSCBH ; pmnum++) {
        pm_going_up=0;
        lport_save=0;
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (port_schedule_state->resource[i].physical_port != -1) {
                lport = port_schedule_state->resource[i].logical_port;
                phy_port = port_schedule_state->resource[i].physical_port;
                if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)&&
                    (((phy_port - 1) >> 3) == pmnum)) {
                    pm_going_up|= 1;
                    lport_save = lport;
                }
            }
        }
        if (pm_going_up){
            lport = lport_save;
            reset=0;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_pa_reset(
                unit, &port_schedule_state->out_port_map,lport,reset));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_obm_tdm_cfg(
                unit, &port_schedule_state->out_port_map,lport));
        }
    }

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            lport = port_schedule_state->resource[i].logical_port;
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
        }
    }

    /* Reset status registers for the ports comming up */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_idb_clear_stats_new_ports(unit,
                                                       port_schedule_state));

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

    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */
