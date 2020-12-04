/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hercules.c
 * Purpose:     Driver functions for 5670/5675
 */

#include <shared/bsl.h>

#include <soc/hercules.h>
#include <soc/higig.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>

#ifdef BCM_HERCULES_SUPPORT

STATIC int
soc_hercules_uc_port_set(int unit, soc_port_t ingress_port, int modid, pbmp_t pbmp);
STATIC int
soc_hercules_vlan_port_init(int unit, soc_port_t ingress_port);
STATIC int
soc_hercules_vlan_port_init_all(int unit);
STATIC int
soc_hercules_vlan_port_set(int unit,
                      soc_port_t ingress_port, vlan_id_t vid, pbmp_t pbmp);
STATIC int
soc_hercules_mc_port_init(int unit, soc_port_t ingress_port);
STATIC int
soc_hercules_mc_port_init_all(int unit);
STATIC int
soc_hercules_mc_port_set(int unit, soc_port_t ingress_port, int mcidx, pbmp_t pbmp);
STATIC int
soc_hercules_mc_port_set_all(int unit, int mcidx, pbmp_t pbmp);
STATIC int
soc_hercules_port_mmu_init(int unit, int port);
STATIC int
soc_hercules_pie_init(int unit, int port, pbmp_t pbmp, int testmode);

/*
 * ICM/Module Driver/API locals
 */

#define VLAN_RANGE_CHECK(unit)\
    if (vid < soc_mem_index_min(unit, MEM_VIDm) ||\
        vid > soc_mem_index_max(unit, MEM_VIDm)) {\
        return SOC_E_BADID;\
    }
#define MODID_RANGE_CHECK(unit)\
    if (modid < soc_mem_index_min(unit, MEM_UCm) ||\
        modid > soc_mem_index_max(unit, MEM_UCm)) {\
        return SOC_E_BADID;\
    }
#define MCIDX_RANGE_CHECK(unit)\
    if (mcidx < soc_mem_index_min(unit, MEM_MCm) ||\
        mcidx > soc_mem_index_max(unit, MEM_MCm)) {\
        return SOC_E_BADID;\
    }

/*
 * Hercules 1.5 chip driver functions.
 */
soc_functions_t soc_hercules15_drv_funs = {
    soc_hercules_misc_init,
    soc_hercules_mmu_init,
    soc_hercules_age_timer_get,
    soc_hercules_age_timer_max_get,
    soc_hercules_age_timer_set,
};

int
soc_hercules_misc_init(int unit)
{
    uint32 reg, oreg;
    int use12G, eJitter, port;

    use12G = soc_property_get(unit, spn_CORE_CLOCK_12G, 0);
    READ_CMIC_RATE_ADJUSTr(unit, &reg);
    oreg = reg;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &reg, DIVISORf,
                      (use12G ? 189 : 157));
    if (reg != oreg) {
        WRITE_CMIC_RATE_ADJUSTr(unit, reg);
    }
    eJitter = soc_property_get(unit, spn_MMU_HOL_JITTER, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_MMU_CFGr(unit, port, &reg));
        oreg = reg;
        soc_reg_field_set(unit, MMU_CFGr, &reg, JITTER_ENf,
                          eJitter ? 1 : 0);
        /* Enable parity generation and checking */
        soc_reg_field_set(unit, MMU_CFGr, &reg,
                          PARITY_DIAGf, (SAL_BOOT_SIMULATION) ? 0 : 1);
        if (reg != oreg) {
            SOC_IF_ERROR_RETURN(WRITE_MMU_CFGr(unit, port, reg));
        }
    }
    reg = 0;
    soc_reg_field_set(unit, ING_CTRL2r, &reg,
                      DISABLE_MIRROR_SRCMODBLKf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_ING_CTRL2r(unit, port, reg));
    }

    /*
     * The following tables should be initialized with Valid parity
     * before enabling parity error interrupts.
     * MEM_UCm
     * MEM_VIDm
     * MEM_INGBUFm - initialized by packet store operations.
     * MEM_MCm
     * MEM_IPMCm
     * MEM_ING_SRCMODBLKm
     * MEM_ING_MODMAPm
     * MEM_EGR_MODMAPm
     */

    return SOC_E_NONE;
}


/*
 * Initialize Hercules MMU Hardware
 * and setup module database definitions for API Layer.
 */
int
soc_hercules_mmu_init(int unit)
{
    uint32      tmp;
    int         done, mod, port, rv;
    pbmp_t      pbmp_tmp;
    char        pfmt[SOC_PBMP_FMT_LEN];
    int         idx_min, idx_max;
    mem_ing_srcmodblk_entry_t   mod_blk;

    /* Only valid driver for interconnect devices */
    if (!SOC_IS_XGS_FABRIC(unit)) {
        return SOC_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules_mmu_init: unit %d\n"), unit));

    /* Initialize MMU in CMIC */
    rv = soc_hercules_port_mmu_init(unit, CMIC_PORT(unit));
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: CMIC MMU init failed.\n")));
    }

    /* Setup Hercules 10G Port Interface Controller Registers */
    PBMP_PORT_ITER(unit, port) {
        /* Initialize MMU per-port, set test mode on PLI */
        rv = soc_hercules_port_mmu_init(unit, port);

        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit,
                                "soc_hercules_mmu_init: "
                                "MMU init failed (port %s).\n"),
                     SOC_PORT_NAME(unit, port)));
        }

        /* Initialize PIE per-port */
        rv = soc_hercules_pie_init(unit, port,
                                   PBMP_ALL(unit), SAL_BOOT_PLISIM);
        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit,
                                "soc_hercules_mmu_init: "
                                "Ingress/Egress init failed (port %s).\n"),
                     SOC_PORT_NAME(unit, port)));
        }
    }

    /* Wait for H/W to finish initializing itself
     * NOTE: this is several microseconds, so you would need
     * a really fast processor to ever see this error, but we
     * do this to synchronize with the H/W to be safe.
     */
    if (!SAL_BOOT_PLISIM) {
        PBMP_PORT_ITER(unit, port) {
            do {
                SOC_IF_ERROR_RETURN(
                    READ_MMU_ERRSTATr(unit, port, &tmp));
                done = soc_reg_field_get(unit, MMU_ERRSTATr, tmp, HW_INITf);
                if (!done)
                    LOG_CLI((BSL_META_U(unit,
                                        "Notice: HW initializing (port %s)\n"),
                             SOC_PORT_NAME(unit, port)));
            } while (!done);
        }
    }

    /*
     * Should ideally move to misc_init. Init below also genarates the correct
     * parity bits for the following tables
     *
     * The following tables should be initialized with Valid parity
     * before enabling parity error interrupts.
     * MEM_UCm
     * MEM_VIDm
     * MEM_INGBUFm - initialized by packet store operations.
     * MEM_MCm
     * MEM_ING_MODMAPm
     * MEM_EGR_MODMAPm
     * MEM_IPMCm
     * MEM_ING_SRCMODBLKm
     *
     *
     * Zero Module ID table.
 */
    SOC_PBMP_CLEAR(pbmp_tmp);
    idx_min = soc_mem_index_min(unit, MEM_UCm);
    idx_max = soc_mem_index_max(unit, MEM_UCm);
    mod = 0;

    if (!SAL_BOOT_SIMULATION) {
        for (mod = idx_min; mod <= idx_max; mod++) {
            rv = soc_hercules_uc_port_set_all(unit, mod, pbmp_tmp);
            if (rv < 0) break;
        }
    } else {
            rv = soc_hercules_uc_port_set_all(unit, mod, PBMP_CMIC(unit));
    }

    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: "
                            "set UC[MH.MODID=%d] to %s failed.\n"),
                 mod, SOC_PBMP_FMT(PBMP_ALL(unit), pfmt)));
    }

    /* Flush VLAN table */
    if (!SAL_BOOT_SIMULATION) { /* Way too slow for sim */
        rv = soc_hercules_vlan_port_init_all(unit);
        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit,
                                "ERROR: clearing VLAN tables failed.\n")));
        }
    }

    /* VLAN / DLF Packets (CPU) VID=0 */
    rv = soc_hercules_vlan_port_set_all(unit, 0, PBMP_CMIC(unit));
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: "
                            "set VID[MH.MODID=%d] to %s failed.\n"),
                 VLAN_ID_DEFAULT, SOC_PBMP_FMT(PBMP_ALL(unit), pfmt)));
    }

    /* VLAN / DLF Packets (VID=1) */
    rv = soc_hercules_vlan_port_set_all(unit, VLAN_ID_DEFAULT, PBMP_ALL(unit));
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: "
                            "set VID[MH.MODID=%d] to %s failed.\n"),
                 VLAN_ID_DEFAULT, SOC_PBMP_FMT(PBMP_ALL(unit), pfmt)));
    }

    /* MC / IPMC DLF Packets */
    if (!SAL_BOOT_SIMULATION) { /* Way too slow for sim */
        rv = soc_hercules_mc_port_init_all(unit);
        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit,
                                "ERROR: clearing MC tables failed.\n")));
        }
    }

    /* MC / IPMC DLF Packets */
    rv = soc_hercules_mc_port_set_all(unit, SOC_HIGIG_DST_MOD_CPU,
                                 PBMP_ALL(unit));
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: "
                            "set MC[MH.MODID=%d] to %s failed.\n"),
                 SOC_HIGIG_DST_MOD_CPU,
                 SOC_PBMP_FMT(PBMP_ALL(unit), pfmt)));
    }

    rv = soc_mem_clear(unit, MEM_ING_MODMAPm, MEM_BLOCK_ALL, TRUE);
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: ING_MODMAP clear failed\n")));
    }
    rv = soc_mem_clear(unit, MEM_EGR_MODMAPm, MEM_BLOCK_ALL, TRUE);
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: EGR_MODMAP clear failed\n")));
    }
    rv = soc_mem_clear(unit, MEM_TRUNK_PORT_POOLm, MEM_BLOCK_ALL, TRUE);
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: TRUNK_PORT_POOL clear failed\n")));
    }
    rv = soc_mem_clear(unit, MEM_IPMCm, MEM_BLOCK_ALL, TRUE);
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mmu_init: IPMC clear failed\n")));
    }

    sal_memset(&mod_blk, 0, sizeof(mod_blk));
    soc_mem_field32_set(unit, MEM_ING_SRCMODBLKm, &mod_blk,
                        BITMAPf, SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));

    idx_min = soc_mem_index_min(unit, MEM_ING_SRCMODBLKm);
    idx_max = soc_mem_index_max(unit, MEM_ING_SRCMODBLKm);
    for (mod = idx_min; mod <= idx_max; mod++) {
        rv = soc_mem_write(unit, MEM_ING_SRCMODBLKm, MEM_BLOCK_ALL,
                           mod, &mod_blk);
        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit,
                                "soc_hercules_mmu_init: ING_SRCMODBLK init failed\n")));
            break;
        }
    }
    PBMP_ALL_ITER(unit, port) {
        /* Clear any outstanding parity error notification */
        SOC_IF_ERROR_RETURN(
            WRITE_MMU_INTCLRr(unit, port, SOC_ERR_HAND_MMU_ALL_H15));
    }

    /* Prep MMU error handling */
    SOC_IF_ERROR_RETURN(soc_mmu_error_init(unit));

    return rv;
}

int
soc_hercules_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(age_seconds);
    COMPILER_REFERENCE(enabled);

    return SOC_E_NONE;
}

int
soc_hercules_age_timer_set(int unit, int age_seconds, int enabled)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(age_seconds);
    COMPILER_REFERENCE(enabled);

    return SOC_E_NONE;
}

int
soc_hercules_age_timer_max_get(int unit, int *max_seconds)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(max_seconds);

    *max_seconds = 0x7fffffff; /* Use max int for BCMX purposes */

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_hercules_mem_read
 * Purpose:
 *      Read a Hercules word addressed memory internal to the SOC.
 * Notes:
 *      This should only be used for hercules.
 */

int
soc_hercules_mem_read(int unit,
             soc_mem_t mem,
             int copyno,
             int index,
             void *entry_data)
{
    uint32 entry_addr;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);
    int entry_words, word;
    uint32 *data = (uint32 *)entry_data;

    entry_words = soc_mem_entry_words(unit, mem);

    entry_addr = soc_mem_addr(unit, mem, 0, copyno, 0);
    entry_addr += index * meminfo->gran;

    if (mem == MEM_PPm && !soc_feature(unit, soc_feature_fabric_debug)) {
        /* Workaround for A0 problem */
        SOC_IF_ERROR_RETURN(soc_hercules_mem_read_word(unit,
                                entry_addr, &(data[0])));
    }

    for (word = 0; word < entry_words; word++) {
        SOC_IF_ERROR_RETURN(soc_hercules_mem_read_word(unit,
                                entry_addr + word, &(data[word])));
    }

    if (NULL != meminfo->snoop_cb && 
        (SOC_MEM_SNOOP_READ & meminfo->snoop_flags)) {
        meminfo->snoop_cb(unit, mem, SOC_MEM_SNOOP_READ, copyno, index, index, 
                           entry_data, meminfo->snoop_user_data);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hercules_mem_read_word
 * Purpose:
 *      Read a memory word internal to the SOC.
 * Notes:
 *      This should only be used for hercules.
 */

int
soc_hercules_mem_read_word(int unit, uint32 addr, void *word_data)
{
    schan_msg_t schan_msg;
    int src_blk;
    int opcode;

    /* Setup S-Channel command packet */
    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    soc_schan_header_cmd_set(unit, &schan_msg.header, READ_MEMORY_CMD_MSG,
                             0, src_blk, 0, 4, 0, 0);  

    schan_msg.readcmd.address = addr;

    /*
     * Write onto S-Channel "memory read" command packet consisting of header
     * word + address word, and read back header word + entry_dw data words.
     */
    SOC_IF_ERROR_RETURN(soc_schan_op(unit, &schan_msg, 2, 2, 0));

    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, NULL);
    if (opcode != READ_MEMORY_ACK_MSG) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_hercules_mem_read_word: "
                              "invalid S-Channel reply, expected READ_MEMORY_ACK:\n")));
        soc_schan_dump(unit, &schan_msg, 2);
        return SOC_E_INTERNAL;
    }

    sal_memcpy(word_data,
               schan_msg.readresp.data,
               sizeof (uint32));

    if (bsl_check(bslLayerSoc, bslSourceMem, bslSeverityNormal, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mem_read_word: u=%d: "
                            "addr=0x%08x: value=0x%08x\n"),
                 unit, addr, *(uint32 *)word_data));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hercules_mem_write
 * Purpose:
 *      Write a Hercules word addressed memory internal to the SOC.
 * Notes:
 *      This should only be used for hercules.
 */

int
soc_hercules_mem_write(int unit,
             soc_mem_t mem,
             int copyno,
             int index,
             void *entry_data)
{
    uint32 entry_addr;
    int entry_words, word;
    uint32 *data = (uint32 *)entry_data;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);
    int blk;

    entry_words = soc_mem_entry_words(unit, mem);

    if (copyno != COPYNO_ALL) {
        if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "soc_hercules_mem_write: "
                                 "invalid copy %d for table %s\n"),
                      copyno, SOC_MEM_NAME(unit, mem)));
            return SOC_E_PARAM;
        }
    }

    /* Write to one or all copies of the memory */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }

        entry_addr = soc_mem_addr(unit, mem, 0, blk, 0);
        entry_addr += index * meminfo->gran;

        for (word = 0; word < entry_words; word++) {
            SOC_IF_ERROR_RETURN(soc_hercules_mem_write_word(unit,
                                    entry_addr + word, &(data[word])));
        }
    }

    if (NULL != meminfo->snoop_cb && 
        (SOC_MEM_SNOOP_WRITE & meminfo->snoop_flags)) {
        meminfo->snoop_cb(unit, mem, SOC_MEM_SNOOP_WRITE, copyno, index, index,
                          entry_data, meminfo->snoop_user_data);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hercules_mem_write_word
 * Purpose:
 *      Write a memory word internal to the SOC.
 * Notes:
 */

int
soc_hercules_mem_write_word(int unit, uint32 addr, void *word_data)
{
    schan_msg_t schan_msg;
    int src_blk;

    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_MEMORY_CMD_MSG,
                             0, src_blk, 0, 4, 0, 0);  
    schan_msg.writecmd.address = addr;

    sal_memcpy(schan_msg.writecmd.data,
               word_data,
               sizeof (uint32));

    if (bsl_check(bslLayerSoc, bslSourceMem, bslSeverityNormal, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_hercules_mem_write_word: u=%d: "
                            "addr=0x%08x: value=0x%08x\n"),
                 unit, addr, *(uint32 *)word_data));
    }

    /* Write header + address + entry_dw data DWORDs */
    /* Note: The hardware does not send WRITE_MEMORY_ACK_MSG. */
    SOC_IF_ERROR_RETURN(soc_schan_op(unit, &schan_msg,
                                     3, 0, 0));

    return SOC_E_NONE;
}


/*
 * Setup all ingress ports for a UNICAST entry to flood to the given port
 * bitmap
 */
int
soc_hercules_uc_port_set_all(int unit, int modid, pbmp_t pbmp)
{
    int port;

    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_hercules_uc_port_set(unit, port, modid, pbmp));
    }
    return SOC_E_NONE;
}

/*
 * Update all DLF/Broadcast (VID) table entries specifying the flood group
 * bitmaps for the given VLAN ID (VID).
 */
int
soc_hercules_vlan_port_set_all(int unit, vlan_id_t vid, pbmp_t pbmp)
{
    int port;

    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_hercules_vlan_port_set(unit, port, vid, pbmp));
    }
    return SOC_E_NONE;
}

/*
 * Configure a Hercules port's packet and cell limits.
 */

int
soc_hercules15_mmu_limits_config(int unit, int port, int num_ports,
				 int num_cos, int lossless)
{
    int cos, xq_entries, hysteresis;
    soc_port_t other_port;
    uint32 lla_cells;
    uint32 lla_cells_good = SOC_CONTROL(unit)->lla_cells_good[port];

    assert(num_cos > 0 && num_cos <= NUM_COS(unit));

    /*
     * Allowing for future decrease in the max XQ used.
     * NB:  Use one less than the total number of entries, as
     * pktlmting is one bit too small to handle the full size of XQ.
     */
    xq_entries = soc_mem_index_max(unit, MEM_XQm) -
                 soc_mem_index_min(unit, MEM_XQm);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules15_mmu_limits_config: "
                            "unit=%d port=%s nports=%d ncos=%d loss%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 num_ports, num_cos, lossless ? "y" : "less"));

    /* Set up COS specific registers per port */

   hysteresis = soc_property_get(unit, spn_MMU_HOL_HYSTERESIS, 0);

    for (cos = 0; cos < NUM_COS(unit); cos++) {
        if (cos < num_cos) {
            /* Packet Limit / COS */
            SOC_IF_ERROR_RETURN(WRITE_MMU_PKTLMTCOS_UPPERr(unit, port, cos,
                                                     xq_entries / num_cos));
            SOC_IF_ERROR_RETURN(WRITE_MMU_PKTLMTCOS_LOWERr(unit, port, cos,
                   hysteresis ? ((xq_entries / num_cos)- (xq_entries / num_cos / 50))
                              : (xq_entries / num_cos)));
            /* Cell Limit / COS */
            lla_cells = (hysteresis) ?
		((lla_cells_good / num_cos) - (lla_cells_good / num_cos / 50))
		: (lla_cells_good / num_cos);
            SOC_IF_ERROR_RETURN(WRITE_MMU_CELLLMTCOS_UPPERr(unit, port, cos,
                   lossless ? lla_cells_good : (lla_cells_good / num_cos)));
            SOC_IF_ERROR_RETURN(WRITE_MMU_CELLLMTCOS_LOWERr(unit, port, cos, 
                   lossless ? lla_cells_good : lla_cells));
        } else {
            /* Packet Limit / COS */
            SOC_IF_ERROR_RETURN(WRITE_MMU_PKTLMTCOS_UPPERr(unit, port, cos, 0));
            SOC_IF_ERROR_RETURN(WRITE_MMU_PKTLMTCOS_LOWERr(unit, port, cos, 0));
            /* Cell Limit / COS */
            SOC_IF_ERROR_RETURN(WRITE_MMU_CELLLMTCOS_UPPERr(unit, port, cos, 0));
            SOC_IF_ERROR_RETURN(WRITE_MMU_CELLLMTCOS_LOWERr(unit, port, cos, 0));
        }
    }

    if (lossless) { /* Lossless mode */
        assert(num_ports > 0);

        PBMP_ALL_ITER(unit, other_port) {
            /* Diagonals don't exist for these registers. */
            if (other_port != port) {
                /* Per Ingress Packet Limit / Port / COS (less 2 packets margin) */
                SOC_IF_ERROR_RETURN(WRITE_MMU_PKTLMTINGr(unit, port,
                                          other_port,
                                          xq_entries / num_ports / num_cos - 2));

                /* Per Ingress Cell Limit / Port (less 3 cells margin) */
                SOC_IF_ERROR_RETURN(
                       WRITE_MMU_CELLLMTINGr(unit, port, other_port,
                                             lla_cells_good / num_ports - 3));
                /* Note:  We've pulled a fast one here.  NUM_PORT does not
                 * include the CMIC, so it is one short of all of the ports
                 * that we are studying here.  However, since we want to skip
                 * the port itself, it works out.  For now. -jwd
                 */
            }
        }
   } else { /* Throughput or "Lossy" mode */
        PBMP_ALL_ITER(unit, other_port) {
            /* Diagonals don't exist for these registers. */
            if (other_port != port) {
                /* All XQ entries */
                SOC_IF_ERROR_RETURN(WRITE_MMU_PKTLMTINGr(unit, port,
                                          other_port, xq_entries));

                /* All good LLA/PP */
                SOC_IF_ERROR_RETURN(
                       WRITE_MMU_CELLLMTINGr(unit, port, other_port,
                                             lla_cells_good));
            }
        }
    }

    if (IS_HG_PORT(unit, port)) {  /* CMIC has no ING_CTRLr */
        uint32 reg, oreg;
        SOC_IF_ERROR_RETURN(READ_ING_CTRLr(unit, port, &reg));
        oreg = reg;
        /* Mark this lossless */
        soc_reg_field_set(unit, ING_CTRLr, &reg, LOSSLESSENf,
                          (lossless ? 1 : 0));
        if (reg != oreg) {
            SOC_IF_ERROR_RETURN(WRITE_ING_CTRLr(unit, port, reg));
        }
    }

    return SOC_E_NONE;
}

/*
 * Set a Unicast(UC) table entry specifying the forwarding egress port
 * bitmap for the ingress port and module ID.
 *
 * Note: Port zero is not the first port, but rather the CMIC port.
 */
STATIC int
soc_hercules_uc_port_set(int unit, soc_port_t ingress_port, int modid,
			 pbmp_t pbmp)
{
    mem_uc_entry_t uc;
    char pfmt[SOC_PBMP_FMT_LEN];

    MODID_RANGE_CHECK(unit);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules_uc_port_set: unit=%d port=%s modid=%d pbmp=%s\n"),
                 unit, SOC_PORT_NAME(unit, ingress_port), modid,
                 SOC_PBMP_FMT(pbmp, pfmt)));
    sal_memset(&uc, 0, sizeof(uc));
    soc_MEM_UCm_field32_set(unit, &uc, UCBITMAPf, SOC_PBMP_WORD_GET(pbmp, 0));
    return WRITE_MEM_UCm(unit, SOC_PORT_BLOCK(unit, ingress_port),
                       modid, &uc);
}

/*
 * Flush out the VLAN table at inital startup
 */
STATIC int
soc_hercules_vlan_port_init(int unit, soc_port_t ingress_port)
{
    mem_vid_entry_t vdata;
    int vid, vid_min, vid_max, blk;

    vid_min = soc_mem_index_min(unit, MEM_VIDm);
    vid_max = soc_mem_index_max(unit, MEM_VIDm);
    blk = SOC_PORT_BLOCK(unit, ingress_port);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules_vlan_port_init: unit=%d port=%s\n"),
                 unit, SOC_PORT_NAME(unit, ingress_port)));
    sal_memset(&vdata, 0, sizeof(mem_vid_entry_t));

    for (vid = vid_min; vid <= vid_max; vid++) {
        SOC_IF_ERROR_RETURN(WRITE_MEM_VIDm(unit, blk, vid, &vdata));
    }
    return SOC_E_NONE;
}

/*
 * Same operation as above, applied to all ports.
 */
STATIC int
soc_hercules_vlan_port_init_all(int unit)
{
    int port;

    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_hercules_vlan_port_init(unit, port));
    }
    return SOC_E_NONE;
}

/*
 * Update a DLF/Broadcast (VID) table entry specifying the flood group
 * bitmap for the given VLAN ID (VID).
 */
STATIC int
soc_hercules_vlan_port_set(int unit,
			   soc_port_t ingress_port, vlan_id_t vid, pbmp_t pbmp)
{
    int blk;
    mem_vid_entry_t vdata;
    char pfmt[SOC_PBMP_FMT_LEN];

    VLAN_RANGE_CHECK(unit);
    blk = SOC_PORT_BLOCK(unit, ingress_port);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules_vlan_port_set: unit=%d port=%s vid=%d pbmp=%s\n"),
                 unit, SOC_PORT_NAME(unit, ingress_port),
                 vid, SOC_PBMP_FMT(pbmp, pfmt)));
    sal_memset(&vdata, 0, sizeof(vdata));
    soc_MEM_VIDm_field32_set(unit, &vdata, VIDBITMAPf,
                             SOC_PBMP_WORD_GET(pbmp, 0));
    return WRITE_MEM_VIDm(unit, blk, vid, &vdata);
}

/*
 * Init a Multicast/IP Multicast (MC) table entry specifying the flood group
 * bitmap for the given Multicast index and port number.
 *
 * The MCBITMAP and IPMCBITMAP are both updated with the specified
 * bitmap at the given index into the MC table.
 */
STATIC int
soc_hercules_mc_port_init(int unit, soc_port_t ingress_port)
{
    mem_mc_entry_t mcdata;
    int mcidx, mcidx_min, mcidx_max, blk;

    mcidx_min = soc_mem_index_min(unit, MEM_MCm);
    mcidx_max = soc_mem_index_max(unit, MEM_MCm);
    blk = SOC_PORT_BLOCK(unit, ingress_port);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules_mc_port_init: unit=%d port=%s\n"),
                 unit, SOC_PORT_NAME(unit, ingress_port)));
    sal_memset(&mcdata, 0, sizeof(mem_mc_entry_t));
    for (mcidx = mcidx_min; mcidx <= mcidx_max; mcidx++) {
        SOC_IF_ERROR_RETURN(WRITE_MEM_MCm(unit, blk, mcidx, &mcdata));
    }
    return SOC_E_NONE;
}

STATIC int
soc_hercules_mc_port_init_all(int unit)
{
    int port;

    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_hercules_mc_port_init(unit, port));
    }
    return SOC_E_NONE;
}

/*
 * Update a L2 Multicast (MC) table entry specifying the flood group
 * bitmap for the given Multicast index and port number.
 */
STATIC int
soc_hercules_mc_port_set(int unit, soc_port_t ingress_port, int mcidx,
			 pbmp_t pbmp)
{
    mem_mc_entry_t mcdata;
    char pfmt[SOC_PBMP_FMT_LEN];
    int blk;

    MCIDX_RANGE_CHECK(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_hercules_mc_port_set: unit=%d port=%s mcidx=%d pbmp=%s\n"),
                 unit, SOC_PORT_NAME(unit, ingress_port),
                 mcidx, SOC_PBMP_FMT(pbmp, pfmt)));

    blk = SOC_PORT_BLOCK(unit, ingress_port);
    sal_memset(&mcdata, 0, sizeof(mcdata));
    SOC_IF_ERROR_RETURN(READ_MEM_MCm(unit, blk, mcidx, &mcdata));
    soc_MEM_MCm_field32_set(unit, &mcdata,
                            MCBITMAPf, SOC_PBMP_WORD_GET(pbmp, 0));
    SOC_IF_ERROR_RETURN(WRITE_MEM_MCm(unit, blk, mcidx, &mcdata));

    return SOC_E_NONE;
}

STATIC int
soc_hercules_mc_port_set_all(int unit, int mcidx, pbmp_t pbmp)
{
    int port;

    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_hercules_mc_port_set(unit, port, mcidx, pbmp));
    }
    return SOC_E_NONE;
}

/*
 * Initialize an MMU (port0=CMIC, 1-8 = PIE)
 *       THIS IS FOR HERCULES ONLY
 */
STATIC int
soc_hercules_port_mmu_init(int unit, int port)
{
    uint32 lla_cells_good = SOC_CONTROL(unit)->lla_cells_good[port];

    /* Enable MMU interrupts */
    SOC_IF_ERROR_RETURN(WRITE_MMU_INTCLRr(unit, port, SOC_MMU_ERR_CLEAR_ALL));

    SOC_IF_ERROR_RETURN
        (WRITE_MMU_INTCLRr(unit, port, SOC_ERR_HAND_MMU_ALL_H15));
    if (SOC_CONTROL(unit)->sbe_disable[port]) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_INTCTRLr(unit, port,
                                               (port == 0) ? 0 :
                                               SOC_ERR_HAND_MMU_ALL_H15 ^
                                               SOC_ERR_HAND_MMU_PP_SBE));
    } else {
        SOC_IF_ERROR_RETURN(WRITE_MMU_INTCTRLr(unit, port,
                                               (port == 0) ? 0 :
                                               SOC_ERR_HAND_MMU_ALL_H15));
    }

    /* Total Cell Limit/Port */
    SOC_IF_ERROR_RETURN(
        WRITE_MMU_CELLLMTTOTAL_UPPERr(unit, port, lla_cells_good));
    SOC_IF_ERROR_RETURN(
        WRITE_MMU_CELLLMTTOTAL_LOWERr(unit, port, lla_cells_good));

    return SOC_E_NONE;
}

/*
 * Initialize a Port Ingress/Egress (PIE) Module
 */
STATIC int
soc_hercules_pie_init(int unit, int port, pbmp_t pbmp, int testmode)
{
    uint32 reg;

    /* DEBUG/TEST/BRINGUP stuff */

    if (testmode) {
        /*
         * Set Ingress test mode. In this mode, the first entry
         * in the UC table is always used as the final egress
         * bitmap for the packet - no matter what.
         */

	reg = 0;
        soc_reg_field_set(unit, ING_CTRLr, &reg, TSTMODEENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_CTRLr(unit, port, reg));
        LOG_CLI((BSL_META_U(unit,
                            "NOTICE: port %s: INGRESS test mode enabled.\n"),
                 SOC_PORT_NAME(unit, port)));
    }

    /*
     * Set EGRMSKBMAPr
     * Initially, allow egressing to any of the ports.
     */
    SOC_IF_ERROR_RETURN
	(WRITE_ING_EGRMSKBMAPr(unit, port, SOC_PBMP_WORD_GET(pbmp, 0)));

    /*
     * Set ING_CPUTOBMAP
     * This register defines the ports which CPU packets need to
     * be forwarded to (always CPU port bitmap).
     */
    SOC_IF_ERROR_RETURN
	(WRITE_ING_CPUTOBMAPr(unit, port,
			      SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0)));

    /*
     * Set ING_EPC_LNKBMAPr register to cmic-only forwarding.
     * Linkscan will add the rest.
     */
    
    SOC_IF_ERROR_RETURN
	(WRITE_ING_EPC_LNKBMAPr(unit, port,
				SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0)));

    return SOC_E_NONE;
}

#endif  /* BCM_HERCULES_SUPPORT */
