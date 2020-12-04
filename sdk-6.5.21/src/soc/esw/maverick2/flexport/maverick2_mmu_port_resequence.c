/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  maverick2_mmu_port_resequence.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_MAVERICK2_SUPPORT)
#include <soc/maverick2.h>
#include <soc/maverick2.h>
/*! @file maverick2_mmu_port_resequence.c
 *  @brief
 */

#include <soc/flexport/maverick2/maverick2_flexport.h>


/*! @fn int soc_maverick2_mmu_set_mmu_to_phy_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the mmu port to physical port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_maverick2_mmu_set_mmu_to_phy_port_mapping(
    int unit,soc_port_resource_t *port_resource_t)
{

    int mmu_port;
    uint32 phy_port_new;
    soc_reg_t reg;
    uint64 rval;
    int inst;


    mmu_port = port_resource_t->mmu_port;
    inst = mmu_port;


    /* When a physical port goes down, the port resource struct phy port number
     * becomes -1. That time, we need to invalidate the phy port to 'hff  When
     * a port is coming back up, the struct is populated correctly.
     */
    if (port_resource_t->physical_port == -1) {
        phy_port_new = MV2_INVALID_PHY_PORT_NUMBER; /* All 1's */
    }
    else {
        phy_port_new= port_resource_t->physical_port;
    }


    reg= MMU_PORT_TO_PHY_PORT_MAPPINGr;
    COMPILER_64_ZERO(rval);
    soc_reg64_field32_set(unit, reg, &rval, PHY_PORTf, phy_port_new);
    /* BT:IPORT, AT: SINGLE, Block ID: MMU_GLOBAL */
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;

}


/*! @fn int soc_maverick2_mmu_vbs_port_flush(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t, int pipe, int set_val)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param set_val Value to be set to the port flush register
 *  @brief API to Set the Port Flush register
 */
int
soc_maverick2_mmu_vbs_port_flush(int unit,soc_port_schedule_state_t *port_schedule_state_t,
                                 int pipe, uint64 set_val)
{
    soc_reg_t reg1, reg2;
    uint64 rval_0, rval_1;
    uint64 enable_val_0, enable_val_1;
    int port;
    int mmu_port, lcl_mmu_port;
    int inst;
    int update0, update1;
    uint64 new_val_0, new_val_1;
    uint64 temp64;

    reg1 = Q_SCHED_PORT_FLUSH_SPLIT0r;
    reg2 = Q_SCHED_PORT_FLUSH_SPLIT1r;

    inst = pipe;

    /* READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    COMPILER_64_ZERO(rval_0);
    COMPILER_64_ZERO(rval_1);

    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg1,inst, 0, &rval_0));
    enable_val_0 = soc_reg64_field_get(unit, reg1, rval_0, ENABLEf);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg2,inst, 0, &rval_1));
    enable_val_1 = soc_reg64_field_get(unit, reg2, rval_1, ENABLEf);

    update0 = 0;
    update1 = 0;

    for (port=0; port<port_schedule_state_t->nport; port++) {
	    if (port_schedule_state_t->resource[port].pipe == pipe) {

	      COMPILER_64_SET(new_val_0, 0, 1);
	      COMPILER_64_SET(new_val_1, 0, 1);

	      mmu_port = port_schedule_state_t->resource[port].mmu_port;
	      lcl_mmu_port = mmu_port % MV2_MMU_PORT_PIPE_OFFSET;

          COMPILER_64_ZERO(temp64);
          COMPILER_64_SET(temp64, 0, MV2_MMU_FLUSH_ON);

	      if (lcl_mmu_port < 64) {
	        COMPILER_64_SHL(new_val_0, lcl_mmu_port);

	        if (COMPILER_64_EQ(set_val, temp64)) {
	          COMPILER_64_OR(enable_val_0, new_val_0);
	        } else {
			  COMPILER_64_NOT(new_val_0);
			  COMPILER_64_AND(enable_val_0, new_val_0);
	        }

	        COMPILER_64_ZERO(rval_0);
	        soc_reg64_field_set(unit, reg1, &rval_0, ENABLEf, enable_val_0);
	        update0 = 1;

	      } else {
	        COMPILER_64_SHL(new_val_1, (lcl_mmu_port - 64));

	        if (COMPILER_64_EQ(set_val, temp64)) {
	          COMPILER_64_OR(enable_val_1, new_val_1);
	        }
	        else {
			  COMPILER_64_NOT(new_val_1);
			  COMPILER_64_AND(enable_val_1, new_val_1);
	        }

	        COMPILER_64_ZERO(rval_1);
	        soc_reg64_field_set(unit, reg2, &rval_1, ENABLEf, enable_val_1);
	        update1 = 1;

	      }
	    }
    }

    if(update0 == 1)
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg1, inst, 0, rval_0));
    if(update1 == 1)
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg2, inst, 0, rval_1));

    return SOC_E_NONE;
}


/*! @fn int soc_maverick2_mmu_rqe_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param pipe Pipe number to flush
 *  @param set_val Value to be set to the snapshot register
 *  @brief API to Set the RQE Snapshot register to Flush out packets in the
 *         RQE Replication FIFO
 */
int
soc_maverick2_mmu_rqe_port_flush(int unit,int pipe,
                                 uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    uint64 enable_val;
    int count=0;
    uint64 rd_val;
    int pipe_number, inst;

    reg = Q_SCHED_RQE_SNAPSHOTr;
    pipe_number = pipe;


    inst = pipe_number;

    enable_val = set_val;
    COMPILER_64_ZERO(rval);
    if (!SAL_BOOT_BCMSIM) {
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
            if (count > 30000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Initiate isn't reset even after 1000 "
                                    "iterations")));
                return SOC_E_FAIL;
            }
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_maverick2_mmu_mtro_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the snapshot register
 *  @brief This function is used to set the MTRO Port Flush register to
 *         ignore shaper information on a port during flexport operation
 */
int
soc_maverick2_mmu_mtro_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                  uint64 set_val)
{


    soc_reg_t reg1, reg2;
    uint64 rval_0;
    uint64 rval_1;
    int mmu_port, lcl_mmu_port;
    int pipe_number, inst;
    uint64 enable_val_0;
    uint64 enable_val_1;

    reg1 = MTRO_PORT_ENTITY_DISABLE_SPLIT0r;
    reg2 = MTRO_PORT_ENTITY_DISABLE_SPLIT1r;
    pipe_number = port_resource_t->pipe;
    mmu_port = port_resource_t->mmu_port;
    lcl_mmu_port = mmu_port % MV2_MMU_PORT_PIPE_OFFSET;

    /* READ MODIFY WRITE IN SW ... Hence get
       Register Value and Then Write ..  */
    inst = pipe_number;

    if (lcl_mmu_port < 64) {

        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg1,
                                                inst, 0, &rval_0));
        enable_val_0 = soc_reg64_field_get(unit, reg1,
                                            rval_0, METERING_DISABLEf);
        if (COMPILER_64_IS_ZERO(set_val) == 1) {
            COMPILER_64_BITCLR(enable_val_0, lcl_mmu_port);
        }
        else {
            COMPILER_64_BITSET(enable_val_0, lcl_mmu_port);
        }
        COMPILER_64_ZERO(rval_0);
        soc_reg64_field_set(unit, reg1, &rval_0, METERING_DISABLEf, enable_val_0);
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg1, inst, 0, rval_0));
    } else {

        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg2,
                                                inst, 0, &rval_1));
        enable_val_1 = soc_reg64_field_get(unit, reg2,
                                            rval_1, METERING_DISABLEf);
        if (COMPILER_64_IS_ZERO(set_val) == 1) {
            COMPILER_64_BITCLR(enable_val_1, lcl_mmu_port - 64);
        }
        else {
            COMPILER_64_BITSET(enable_val_1, lcl_mmu_port - 64);
        }
        COMPILER_64_ZERO(rval_1);
        soc_reg64_field_set(unit, reg2, &rval_1, METERING_DISABLEf, enable_val_1);
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg2, inst, 0, rval_1));
    }

    return SOC_E_NONE;
}

/*! @fn int soc_maverick2_mmu_disable_ct(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to disable CT for a port when it goes down
 */
int
soc_maverick2_mmu_disable_ct(int                  unit,
                            soc_port_resource_t  *port_resource_t)
{
    uint64 rval, fldval64;
    int mmu_port;
    int inst;
    soc_reg_t reg;

    mmu_port = port_resource_t->mmu_port;
    reg = ASF_EPORT_CFGr;
    inst = mmu_port;

    /* Set CT enable low */

    COMPILER_64_ZERO(rval);
    COMPILER_64_ZERO(fldval64);
    soc_reg64_field_set(unit, reg, &rval, ENABLEf, fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}



#endif /* BCM_MAVERICK2_SUPPORT */
