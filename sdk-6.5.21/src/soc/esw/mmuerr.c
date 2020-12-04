/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC Error Handlers
 *
 * NOTE: These handlers are triggered by interrupts, but happen in their own
 *      contexts.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/mmuerr.h>

#ifdef BCM_HERCULES_SUPPORT

/*
 * Function:
 *      soc_mmu_error_init
 * Purpose:
 *      Initialize the statistics of the MMU error handler
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXX on other failure
 * Notes:
 */

int
soc_mmu_error_init(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    soc_mmu_error_t     *mmu_errors;
    soc_port_t          port;
    uint32		regval;
    int                 rv;

    /* Don't want any interrupts from MMU until initialized */
    /* This will be turned on by soc_mmu_init in drv.c */
    soc_pci_write(unit, CMIC_MMUIRQ_MASK, 0);    

    /* Free if allocated ... */
    if (soc->mmu_errors) {
        sal_free(soc->mmu_errors);
    }

    mmu_errors = sal_alloc(sizeof(soc_mmu_error_t) * SOC_MAX_NUM_PORTS,
                           "MMU error counters");

    if (mmu_errors == NULL) {
	return SOC_E_MEMORY;
    }
    
    sal_memset(mmu_errors, 0, sizeof(soc_mmu_error_t) * SOC_MAX_NUM_PORTS);

    PBMP_ALL_ITER(unit, port) {
        mmu_errors[port].xq_parity = 0;
        mmu_errors[port].lla_parity = 0;
        mmu_errors[port].upk_parity = 0;
        mmu_errors[port].ing_parity = 0;
        mmu_errors[port].egr_parity = 0;

        if ((rv = READ_MMU_PP_SBE_CNTr(unit, port, &regval)) < 0) {
            sal_free(mmu_errors);
            return rv;
        }

        mmu_errors[port].pp_sbe_blocks = 
        mmu_errors[port].pp_sbe_blocks_init =
            soc_reg_field_get(unit, MMU_PP_SBE_CNTr, regval, BLOCKCOUNTf);
        
        mmu_errors[port].pp_sbe_cells = 
        mmu_errors[port].pp_sbe_cells_init =
            soc_reg_field_get(unit, MMU_PP_SBE_CNTr, regval, CELLCOUNTf);
        
        if ((rv = READ_MMU_PP_DBE_CNTr(unit, port, &regval)) < 0) {
            sal_free(mmu_errors);
            return rv;
        }

        mmu_errors[port].pp_dbe_blocks = 
        mmu_errors[port].pp_dbe_blocks_init =
            soc_reg_field_get(unit, MMU_PP_DBE_CNTr, regval, BLOCKCOUNTf);
        
        mmu_errors[port].pp_dbe_cells = 
        mmu_errors[port].pp_dbe_cells_init =
            soc_reg_field_get(unit, MMU_PP_DBE_CNTr, regval, CELLCOUNTf);
    }

    soc->mmu_errors = mmu_errors;

    /* Start up the interrupt handling now that we are initialized */
    soc_pci_write(unit, CMIC_MMUIRQ_MASK, CMIC_MMUIRQ_ENABLE_MASK);
    soc_intr_enable(unit, IRQ_MMU_IRQ_STAT);

    return SOC_E_NONE;    
}

/*
 * Function:
 *      soc_mmu_error_done
 * Purpose:
 *      Clean up the statistics of the MMU error handler
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXX on other failure
 * Notes:
 */

int
soc_mmu_error_done(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    soc_mmu_error_t     *mmu_errors = soc->mmu_errors;
    int                 port;

    if (mmu_errors == NULL) {
        return SOC_E_NONE;
    }

    PBMP_ALL_ITER(unit, port) {
        if  (mmu_errors[port].xq_parity) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU:  %d XQ Parity Errors\n"),
                       unit, port, mmu_errors[port].xq_parity));
        }

        if  (mmu_errors[port].lla_parity) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU:  %d LLA Parity Errors\n"),
                       unit, port, mmu_errors[port].lla_parity));
        }

         if  (mmu_errors[port].upk_parity) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU:  %d UPK Parity Errors\n"),
                       unit, port, mmu_errors[port].upk_parity));
        }

        if  (mmu_errors[port].ing_parity) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU:  %d ING Parity Errors\n"),
                       unit, port, mmu_errors[port].ing_parity));
        }

#ifdef BCM_HERCULES15_SUPPORT
        if  (SOC_IS_HERCULES15(unit) && (mmu_errors[port].egr_parity)) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU:  %d EGR Parity Errors\n"),
                       unit, port, mmu_errors[port].egr_parity));
        }
#endif /* BCM_HERCULES15_SUPPORT */

        if  (mmu_errors[port].pp_sbe_blocks ||
             mmu_errors[port].pp_sbe_cells ) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU SBE:  %d blocks, %d cells\n"),
                       unit, port, mmu_errors[port].pp_sbe_blocks,
                       mmu_errors[port].pp_sbe_cells));
        }

        if  (mmu_errors[port].pp_dbe_blocks ||
             mmu_errors[port].pp_dbe_cells ) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %d MMU DBE:  %d blocks, %d cells\n"),
                       unit, port, mmu_errors[port].pp_dbe_blocks,
                       mmu_errors[port].pp_dbe_cells));
        }
   }

    sal_free(mmu_errors);
    soc->mmu_errors = NULL;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mmu_error_port
 * Purpose:
 *      Analyze the source of a Hercules MMU interrupt from a port
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port of the MMU to analyze.
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXX on other failure
 * Notes:
 *      Some of these interrupt sources are fatal errors.  For now, assert.
 *      Eventually, these probably ought to have some more processing.
 */

int
soc_mmu_error_port(int unit, int port)
{
    uint32		regval;
    int                 mmu_status;
    soc_mmu_error_t     *mmu_errors = SOC_CONTROL(unit)->mmu_errors;

    SOC_IF_ERROR_RETURN(READ_MMU_INTSTATr(unit, port, &regval));

    LOG_ERROR(BSL_LS_SOC_MMU,
              (BSL_META_U(unit,
                          "soc_mmu_error_port: u=%d p =%d MMU_INTSTAT %08x\n"),
                          unit, port, regval));

    mmu_status = regval & (SOC_IS_HERCULES15(unit) ?
                            SOC_ERR_HAND_MMU_ALL_H15 : SOC_ERR_HAND_MMU_ALL);

    if (mmu_status & SOC_ERR_HAND_MMU_XQ_PARITY) {
        mmu_errors[port].xq_parity++;
  
        SOC_IF_ERROR_RETURN(READ_MMU_XQ_PARADr(unit, port, &regval));
        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "UNIT %d Port %s MMU XQ Parity Error at address 0x%08x\n"),
                   unit, SOC_PORT_NAME(unit, port), regval));
    }

    if (mmu_status & SOC_ERR_HAND_MMU_PP_SBE) {
        int blocks, cells, last_blocks, last_cells;
        int word, bit, bitpos, addr;
        
        last_blocks = mmu_errors[port].pp_sbe_blocks;
        last_cells = mmu_errors[port].pp_sbe_cells;

        SOC_IF_ERROR_RETURN(READ_MMU_PP_SBE_CNTr(unit, port, &regval));
        blocks = soc_reg_field_get(unit, MMU_PP_SBE_CNTr, regval,
                                   BLOCKCOUNTf);
        cells = soc_reg_field_get(unit, MMU_PP_SBE_CNTr, regval,
                                   CELLCOUNTf);

        if (((blocks - last_blocks) > 1) || ((cells - last_cells) > 1)) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %s MMU SBE Errors missed:\n\t"
                                  "%d blocks, %d cells\n"),
                       unit, SOC_PORT_NAME(unit, port),
                       blocks - last_blocks - 1, 
                       cells - last_cells - 1));
        }

        mmu_errors[port].pp_sbe_blocks = blocks;
        mmu_errors[port].pp_sbe_cells = cells;

        SOC_IF_ERROR_RETURN(READ_MMU_PP_SBE_LOGr(unit, port, &regval));
        bitpos = soc_reg_field_get(unit, MMU_PP_SBE_LOGr, regval, BITPOSf);
        addr = soc_reg_field_get(unit, MMU_PP_SBE_LOGr, regval, ADDRf);
        bit = bitpos & 0xff;
        word = bitpos >> 8;

        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "UNIT %d Port %s MMU Packet Pool SBE Error\n\t"
                              "Entry %d, Word %d,  Bit position %d\n"),
                   unit, SOC_PORT_NAME(unit, port), addr, word, bit));
    }

    if (mmu_status & SOC_ERR_HAND_MMU_PP_DBE) {
        int blocks, cells, last_blocks, last_cells;
        int addr;
        
        last_blocks = mmu_errors[port].pp_dbe_blocks;
        last_cells = mmu_errors[port].pp_dbe_cells;

        SOC_IF_ERROR_RETURN(READ_MMU_PP_DBE_CNTr(unit, port, &regval));
        blocks = soc_reg_field_get(unit, MMU_PP_DBE_CNTr, regval, 
                                   BLOCKCOUNTf);
        cells = soc_reg_field_get(unit, MMU_PP_DBE_CNTr, regval,
                                   CELLCOUNTf);

        if (((blocks - last_blocks) > 1) || ((cells - last_cells) > 1)) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %s MMU DBE Errors missed:\n\t"
                                  "%d blocks, %d cells\n"),
                       unit, SOC_PORT_NAME(unit, port),
                       blocks - last_blocks - 1, 
                       cells - last_cells - 1));
        }

        mmu_errors[port].pp_dbe_blocks = blocks;
        mmu_errors[port].pp_dbe_cells = cells;

        SOC_IF_ERROR_RETURN(READ_MMU_PP_DBE_LOGr(unit, port, &regval));

        addr = soc_reg_field_get(unit, MMU_PP_DBE_LOGr, regval, ADDRf);
        
        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "UNIT %d Port %s MMU Packet Pool DBE Error\n\t"
                              "Entry %d\n"),
                   unit, SOC_PORT_NAME(unit, port), addr));
    }

    if (mmu_status & SOC_ERR_HAND_MMU_LLA_PARITY) {
        mmu_errors[port].lla_parity++;
 
        SOC_IF_ERROR_RETURN(READ_MMU_LLA_PARADr(unit, port, &regval));
        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "UNIT %d Port %s MMU LLA Parity Error at address 0x%08x\n"),
                   unit, SOC_PORT_NAME(unit, port), regval));
    }

    if (mmu_status & SOC_ERR_HAND_MMU_UPK_PARITY) {
        uint32 addr;
        int count;

        mmu_errors[port].upk_parity++;
  
        SOC_IF_ERROR_RETURN(READ_MMU_UPK_ERRLOGr(unit, port, &regval));
        addr = soc_reg_field_get(unit, MMU_UPK_ERRLOGr, regval, ADDRf);
        count = soc_reg_field_get(unit, MMU_UPK_ERRLOGr, regval, COUNTf);

        if (count > mmu_errors[port].upk_parity) {
            LOG_ERROR(BSL_LS_SOC_MMU,
                      (BSL_META_U(unit,
                                  "UNIT %d Port %s MMU UPK Parity Errors missed: %d\n"),
                       unit, SOC_PORT_NAME(unit, port),
                       count - mmu_errors[port].upk_parity));
        }

        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "UNIT %d Port %s MMU UPK Parity Error at address 0x%08x\n"),
                   unit, SOC_PORT_NAME(unit, port), addr));
    }

    if (mmu_status & SOC_ERR_HAND_MMU_ING_PARITY) {
	int	ingmem;
        char	*memory_string;
        static char *mstr[] = {
            "unknown table",
            "Ingress Buffer table",
            "VLAN table",
            "Ingress Buffer and VLAN tables",
            "Multicast table",
            "Ingress Buffer and Multicast tables",
            "VLAN and Multicast tables",
            "Ingress Buffer, VLAN, and Multicast tables"
        };

        mmu_errors[port].ing_parity++;

        SOC_IF_ERROR_RETURN(READ_MMU_ING_PARADr(unit, port, &regval));
        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "soc_mmu_error_port: u=%d p =%d MMU_ING_PARAD %08x\n"),
                   unit, port, regval));
        ingmem = (regval >> SOC_MMU_ERR_ING_SHIFT) & SOC_MMU_ERR_ING_MASK;

        memory_string = mstr[ingmem];
        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "UNIT %d Port %s MMU ING Parity Error in %s\n"),
                   unit, SOC_PORT_NAME(unit, port), memory_string));
    }

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit) && (mmu_status & SOC_ERR_HAND_MMU_EGR_PARITY)){
        mmu_errors[port].egr_parity++;
 
        SOC_IF_ERROR_RETURN(READ_MMU_EGR_PARADr(unit, port, &regval));
        LOG_ERROR(BSL_LS_SOC_MMU,
                  (BSL_META_U(unit,
                              "soc_mmu_error_port: u=%d p =%d MMU_EGR_PARAD %08x\n"),
                   unit, port, regval));
    }
#endif /* BCM_HERCULES15_SUPPORT */
    /* Clear the analyzed bits */
    SOC_IF_ERROR_RETURN(WRITE_MMU_INTCLRr(unit, port, mmu_status));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mmu_error_all
 * Purpose:
 *      Analyze the source of a Hercules MMU interrupt
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXX on other failure
 */

int
soc_mmu_error_all(int unit)
{
    uint32 src, port, bit;
    int rv = SOC_E_NONE;

    src = soc_pci_read(unit, CMIC_MMUIRQ_STAT);

    /* Should be < 9 */
    for (port = 0, bit = 1; port < 32; port++, bit <<= 1) {
	if (!(src & bit)) {
	    continue;
	}
        if ((rv = soc_mmu_error_port(unit, port)) < 0) {
            /* If we missed some ports, the interrupt with trigger again */
            break;
        }
    }

    /* Re-enable the ports to interrupt now that they are serviced */
    soc_pci_write(unit, CMIC_MMUIRQ_MASK, CMIC_MMUIRQ_ENABLE_MASK);    

    return rv;
}

#endif /* BCM_HERCULES_SUPPORT */
