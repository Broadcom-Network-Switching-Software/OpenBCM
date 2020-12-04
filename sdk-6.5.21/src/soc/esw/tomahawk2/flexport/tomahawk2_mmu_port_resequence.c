/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_mmu_port_resequence.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>
/*! @file tomahawk2_mmu_port_resequence.c
 *  @brief
 */

#include <soc/flexport/tomahawk2_flexport.h>


/*! @fn int soc_tomahawk2_mmu_set_mmu_to_phy_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the mmu port to physical port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk2_mmu_set_mmu_to_phy_port_mapping(
    int unit,soc_port_resource_t *port_resource_t)
{

    int mmu_port;
    uint64 phy_port_new, dev_port_new;
    soc_reg_t reg;
    uint64 rval;
    int inst;

    COMPILER_64_SET(phy_port_new, 0, 0);
    COMPILER_64_SET(dev_port_new, 0, 0);

    mmu_port = port_resource_t->mmu_port;
    inst = mmu_port;


    /* When a physical port goes down, the port resource struct phy port number
     * becomes -1. That time, we need to invalidate the phy port to 'hff  When
     * a port is coming back up, the struct is populated correctly.
     */
    if (port_resource_t->physical_port == -1) {
        COMPILER_64_SET(phy_port_new, 0, TH2_INVALID_PHY_PORT_NUMBER); /* All 1's */
    }
    else {
        COMPILER_64_SET(phy_port_new, 0, port_resource_t->physical_port);
        COMPILER_64_SET(dev_port_new, 0, port_resource_t->logical_port);
    }


    reg= MMU_PORT_TO_PHY_PORT_MAPPINGr;
    COMPILER_64_ZERO(rval);
    soc_reg64_field_set(unit, reg, &rval, PHY_PORTf, phy_port_new);
    /* BT:IPORT, AT: SINGLE, Block ID: MMU_GLOBAL */
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    if (port_resource_t->physical_port != -1) {
        reg= MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
        COMPILER_64_ZERO(rval);
        soc_reg64_field_set(unit, reg, &rval, DEVICE_PORTf, dev_port_new);
        /* BT:IPORT, AT: SINGLE, Block ID: MMU_GLOBAL */
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;

}


/*! @fn int soc_tomahawk2_mmu_vbs_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the port flush register
 *  @brief API to Set the Port Flush register
 */
int
soc_tomahawk2_mmu_vbs_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                 uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    uint64 enable_val;
    int mmu_port, lcl_mmu_port;
    int pipe_number, inst;

    reg = Q_SCHED_PORT_FLUSHr;
    pipe_number = port_resource_t->pipe;
    mmu_port = port_resource_t->mmu_port;
    lcl_mmu_port = mmu_port % TH2_MMU_PORT_PIPE_OFFSET;

    /* READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    COMPILER_64_ZERO(rval);

    inst = pipe_number;
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                          inst, 0, &rval));
    enable_val = soc_reg64_field_get(unit, reg,
                                  rval, ENABLEf);
    if (COMPILER_64_IS_ZERO(set_val) == 1) {
        COMPILER_64_BITCLR(enable_val, lcl_mmu_port);
    }
    else {
        COMPILER_64_BITSET(enable_val, lcl_mmu_port);
    }
    soc_reg64_field_set(unit, reg, &rval, ENABLEf, enable_val);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));


    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_mmu_rqe_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int phy_port, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the snapshot register
 *  @param phy_port Only valid for port disabling case. Physical port number of the port to be down
 *  @brief API to Set the RQE Snapshot register to Flush out packets in the
 *         RQE Replication FIFO
 */
int
soc_tomahawk2_mmu_rqe_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                            int phy_port, uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    uint64 enable_val;
    int count=0, count1=0;
    uint64 rd_val;
    int pipe_number, inst;
    uint64 rval64, fldval64;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 cell_requests;

    reg = Q_SCHED_RQE_SNAPSHOTr;
    pipe_number = port_resource_t->pipe;


    inst = pipe_number;

    enable_val = set_val;
    COMPILER_64_ZERO(rval);
    soc_reg64_field_set(unit, reg, &rval, INITIATEf, enable_val);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    while (1) {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                              inst, 0, &rval));
        rd_val = soc_reg64_field_get(unit, reg,
                                     rval, INITIATEf);
        if (COMPILER_64_IS_ZERO(rd_val)) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 10000) {
            count1++;

            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, EGR_PORT_REQUESTSm,
                              MEM_BLOCK_ANY, phy_port, entry));

            cell_requests = soc_mem_field32_get(unit, EGR_PORT_REQUESTSm, entry,
                                                OUTSTANDING_PORT_REQUESTSf);

            if ((count1 == 1) && (cell_requests == 0)) {
                /* Hold MAC in reset state */
                SOC_IF_ERROR_RETURN
                    (soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,0,
                                         &rval64));
                COMPILER_64_SET(fldval64, 0, 1);
                soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf,
                                    fldval64);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr,
                                                        phy_port, 0, rval64));

                /* Remove MAC reset */
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr,
                                                        phy_port, 0, &rval64));
                COMPILER_64_ZERO(fldval64);
                soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf,
                                    fldval64);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr,
                                                        phy_port,0, rval64));
                count=0;
                continue;
            }

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                           "Initiate isn't reset even after 1000 "
                           "iterations")));
            return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_mmu_mtro_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the snapshot register
 *  @brief This function is used to set the MTRO Port Flush register to
 *         ignore shaper information on a port during flexport operation
 */
int
soc_tomahawk2_mmu_mtro_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                  uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    int mmu_port, lcl_mmu_port;
    int pipe_number, inst;
    uint64 enable_val;

    reg = MTRO_PORT_ENTITY_DISABLEr;
    pipe_number = port_resource_t->pipe;
    mmu_port = port_resource_t->mmu_port;
    lcl_mmu_port = mmu_port % TH2_MMU_PORT_PIPE_OFFSET;

    /* READ MODIFY WRITE IN SW ... Hence get
       Register Value and Then Write ..  */
    inst = pipe_number;
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                          inst, 0, &rval));
    enable_val = soc_reg64_field_get(unit, reg,
                                  rval, METERING_DISABLEf);
    if (COMPILER_64_IS_ZERO(set_val) == 1) {
        COMPILER_64_BITCLR(enable_val, lcl_mmu_port);
    }
    else {
        COMPILER_64_BITSET(enable_val, lcl_mmu_port);
    }
    COMPILER_64_ZERO(rval);
    soc_reg64_field_set(unit, reg, &rval, METERING_DISABLEf, enable_val);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK2_SUPPORT */
