/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc_common.c
 * Purpose:     Common functions for soc drivers
 * Requires:
 */

#include <soc/defs.h>
#include <assert.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif

#if defined(BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnx/drv.h>
#endif
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif

#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
#include <soc/greyhound.h>
#endif

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif

#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif

#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
#define CDMAC_OFFSET_CNT    0x10000
#define CDMAC0_STAGE_ID     1
#define CDMAC1_STAGE_ID     2
#endif

/* Initialize composite block types */
uint32 _soc_block_ports_t[] = {
    SOC_BLK_PORT_INITIALIZER
};

uint32 _soc_dnx_block_ports_t[] = {
    SOC_DNX_BLK_PORT_INITIALIZER
};

uint32 _soc_block_cpu_t[] = {
    SOC_BLK_CPU_INITIALIZER
};

uint32 _soc_block_ether_t[] = {
    SOC_BLK_ETHER_INITIALIZER
};

uint32 _soc_block_higig_t[] = {
    SOC_BLK_HIGIG_INITIALIZER
};

uint32 _soc_block_fabric_t[] = {
    SOC_BLK_FABRIC_INITIALIZER
};

uint32 _soc_block_net_t[] = {
    SOC_BLK_NET_INITIALIZER
};

uint32 _soc_block_hgport_t[] = {
    SOC_BLK_HGPORT_INITIALIZER
};


/* The granularity of register addresses, indexed by soc_regtype_t */
int soc_regtype_gran[] = {
    1, /* soc_schan_reg,    */
    1, /* soc_genreg,       */
    1, /* soc_portreg,      */
    1, /* soc_ppportreg,    */
    1, /* soc_cosreg,       */
    1, /* soc_pipereg,      */
    1, /* soc_xpereg,       */
    1, /* soc_slicereg      */
    1, /* soc_layerreg      */
    1, /* soc_itmreg      */
    1, /* soc_ebreg      */
    4, /* soc_cpureg,       */
    4, /* soc_pci_cfg_reg,  */
    1, /* soc_phy_reg,      */
    1, /* soc_spi_reg,      */
    4, /* soc_mcsreg,       */
    4, /* soc_iprocreg,     */
    4, /* soc_hostmem_w,    */
    2, /* soc_hostmem_h,    */
    1, /* soc_hostmem_b,    */
    1, /* soc_customreg,    */
    0, /* soc_invalidreg    */
};

/* Names of register types, indexed by soc_regtype_t */
char *soc_regtypenames[] = {
    "schan",          /* soc_schan_reg,    */
    "general",        /* soc_genreg,       */
    "port",           /* soc_portreg,      */
    "ppport",         /* soc_ppportreg,    */
    "cos",            /* soc_cosreg,       */
    "pipe",           /* soc_pipereg,      */
    "xpe",            /* soc_xpereg,       */
    "slice",          /* soc_slicereg,     */
    "layer",          /* soc_layerreg,     */
    "itm",            /* soc_itmreg        */
    "eb",             /* soc_ebreg         */
    "cpu",            /* soc_cpureg,       */
    "pci_cfg",        /* soc_pci_cfg_reg,  */
    "phy",            /* soc_phy_reg,      */
    "spi",            /* soc_spi_reg,      */
    "mcs",            /* soc_mcsreg,       */
    "iproc",          /* soc_iprocreg,     */
    "mem_word",       /* soc_hostmem_w,    */
    "mem_hword",      /* soc_hostmem_h,    */
    "mem_byte",       /* soc_hostmem_b,    */
    "custom",          /* soc_customreg,       */
    "invalid"         /* soc_invalidreg    */
};

/****************************************************************
 *
 * This array maps from specific chip enum value to the group
 * to which the chip belongs.
 *
 * NB: Order of this array must match soc_chip_types_e from soc/defs.h
 */
soc_chip_groups_t soc_chip_type_map[SOC_CHIP_TYPES_COUNT] = {
    SOC_CHIP_TYPE_MAP_INIT
#ifdef BCM_LTSW_SUPPORT
    SOC_CHIP_TYPE_MAP_LTSW_INIT
#endif
};


/****************************************************************
 *
 * This array maps type indices to names.
 *
 * Indexed by soc_chip_types
 */
char *soc_chip_type_names[SOC_CHIP_TYPES_COUNT] = {
    SOC_CHIP_TYPE_NAMES_INIT
#ifdef BCM_LTSW_SUPPORT
    SOC_CHIP_TYPE_NAMES_LTSW_INIT
#endif
};

/****************************************************************
 *
 * This array maps group indices to names.
 *
 * Indexed by soc_chip_groups_t
 */
char* soc_chip_group_names[SOC_CHIP_GROUPS_COUNT] = {
    SOC_CHIP_GROUP_NAMES_INIT
#ifdef BCM_LTSW_SUPPORT
    SOC_CHIP_GROUP_NAMES_LTSW_INIT
#endif
};

/*
 * The following routines can be called for CMIC (PCI memory space)
 * registers or for SOC registers.  The register type is indicated
 * in rt.
 */

/* Match block against a list */
/* Internally retreives block type from index */
int
SOC_BLOCK_IS_TYPE(int unit, int blk_idx, int *list)
{
    int _bidx = 0;
    while(list[_bidx] != SOC_BLOCK_TYPE_INVALID) {
        if (SOC_BLOCK_TYPE(unit, blk_idx) == list[_bidx]) {
            return TRUE;
        }
        _bidx++;
    }
    return FALSE;
}

/* Checks if a blk type is part of a composite type */
/* Internally retreives block type from index */
int
SOC_BLOCK_IS_COMPOSITE(int unit, int blk_idx, int type)
{
    uint32 *blks = NULL;
    uint8  _tidx = 0;

    switch(type) {
    case SOC_BLK_PORT:
        if (SOC_IS_DNX(unit))
        {
            blks = _soc_dnx_block_ports_t;
        }
        else
        {
            blks = _soc_block_ports_t;
        }
        break;
    case SOC_BLK_CPU:
         blks = _soc_block_cpu_t;
         break;
    case SOC_BLK_ETHER:
         blks = _soc_block_ether_t;
         break;
    case SOC_BLK_HIGIG:
         blks = _soc_block_higig_t;
         break;
    case SOC_BLK_FABRIC:
         blks = _soc_block_fabric_t;
         break;
    case SOC_BLK_NET:
         blks = _soc_block_net_t;
         break;
    case SOC_BLK_HGPORT:
         blks = _soc_block_hgport_t;
         break;
    default: return FALSE;
    }
    while(blks[_tidx] != SOC_BLOCK_TYPE_INVALID) {
        if (SOC_BLOCK_TYPE(unit, blk_idx) == blks[_tidx]) {
            return TRUE;
        }
        _tidx++;
    }
    return FALSE;
}

/* Match a block type against a block list.
 * Type could be specific or composite like PORT.
 * As an optimization one could use SOC_BLOCK_IS if type is
 * known to not be composite.
 */
int
SOC_BLOCK_IN_LIST(int unit, int *blk, int type)
{
    int _bidx = 0;
    uint32 *blks = NULL;
    while(blk[_bidx] != SOC_BLOCK_TYPE_INVALID) {
        int _tidx = 0;
        uint8 composite = TRUE;
        switch(type) {
        case SOC_BLK_PORT:
            if (SOC_IS_DNX(unit))
            {
                blks = _soc_dnx_block_ports_t;
            }
            else
            {
                blks = _soc_block_ports_t;
            }
            break;
        case SOC_BLK_CPU:
             blks = _soc_block_cpu_t;
             break;
        case SOC_BLK_ETHER:
             blks = _soc_block_ether_t;
             break;
        case SOC_BLK_HIGIG:
             blks = _soc_block_higig_t;
             break;
        case SOC_BLK_FABRIC:
             blks = _soc_block_fabric_t;
             break;
        case SOC_BLK_NET:
             blks = _soc_block_net_t;
             break;
        case SOC_BLK_HGPORT:
             blks = _soc_block_hgport_t;
             break;
        default: composite = FALSE;
        }
       /* when composite is non-null, you are coming from non-default
          case above and no need to check blks for NULL */
        if (composite) {
           /* coverity[dead_error_condition] */
            if (blks == NULL) {
               /*
                * Coverity
                * Defencive check.
		* coverity[dead_error_line]
		*/
                return FALSE;
            }
            while(blks[_tidx] != SOC_BLOCK_TYPE_INVALID) {
                if (blk[_bidx] == blks[_tidx]) {
                    return TRUE;
                }
                _tidx++;
            }
            return FALSE;
        } else if (blk[_bidx] == type) {
            return TRUE;
        }
        _bidx++;
    }
    return FALSE;
}
#ifdef BCM_CMICM_SUPPORT
/*
 * There is some PCI addresses that are not mapped to any register.
 * Accessing some of them lead to BUS error. So it is unreasonable to access
 * the invalid addresses.
 */
static int
soc_cpureg_address_is_invalid(int unit, soc_regaddrinfo_t *ainfo) {
    int off;

    off = SOC_REG_BASE(unit, ainfo->reg);
    if (soc_feature(unit, soc_feature_time_v3_no_bs)){
        if (off >= CMIC_BS0_CONFIG_OFFSET &&
            off <= CMIC_BS1_INITIAL_CRC_OFFSET){
            return TRUE;
        }
    }
    if (off >= CMIC_MMU_COSLC_COUNT_ADDR_OFFSET &&
            off < CMIC_TIM0_TIMER1LOAD_OFFSET) {
            return TRUE;
    }

    /*
     * These registers do not ack the CPU until the FSCHAN read cycle is completed.
     * Meaning you must activate a proper FSCHAN read command in order to read these registers.
     */
    switch (ainfo->reg) {
        case CMIC_CMC0_FSCHAN_DATA32r            :
        case CMIC_CMC0_FSCHAN_DATA64_LOr         :
        case CMIC_FSCHAN_DATA32r                 :
        case CMIC_FSCHAN_DATA64_LOr              :
        case CMIC_CMC1_FSCHAN_DATA32r            :
        case CMIC_CMC1_FSCHAN_DATA64_LOr         :
        case CMIC_CMC2_FSCHAN_DATA32r            :
        case CMIC_CMC2_FSCHAN_DATA64_LOr         :
            return TRUE;
        default:
            /* Do Nothing */
            break;
    }
    return FALSE;
}
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)
int
soc_anyreg_read(int unit, soc_regaddrinfo_t *ainfo, uint64 *data)
{
    uint32          tmpdata;
    int             rv = SOC_E_NONE;
    soc_regtype_t   type;
    int             port = ainfo->port;

    if (ainfo->reg < 0) {
        type = soc_genreg;
    } else {
        type = SOC_REG_INFO(unit, ainfo->reg).regtype;
    }

#if defined(BCM_SAND_SUPPORT)
    if (SOC_IS_SAND(unit)) {
        if ((ainfo->port >=0) || (ainfo->port & SOC_REG_ADDR_INSTANCE_MASK)) {
            port = ainfo->port;
        }  else {
            port = REG_PORT_ANY;
        }
    }
#endif

    switch (type) {
    case soc_cpureg:
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            if (soc_feature(unit, soc_feature_cpureg_dump)){
                if (soc_cpureg_address_is_invalid(unit, ainfo)) {
                    rv = SOC_E_INTERNAL;
                    break;
                }
            }
        }
#endif
        tmpdata = soc_pci_read(unit, ainfo->addr);
        COMPILER_64_SET(*data, 0, tmpdata);
        break;
#ifdef BCM_CMICM_SUPPORT
    case soc_mcsreg:
        tmpdata = soc_pci_mcs_read(unit, ainfo->addr);
        COMPILER_64_SET(*data, 0, tmpdata);
        break;
#endif
#ifdef BCM_IPROC_SUPPORT
    case soc_iprocreg:
        tmpdata = soc_cm_iproc_read(unit, ainfo->addr);
        COMPILER_64_SET(*data, 0, tmpdata);
        break;
#endif
    case soc_customreg:
    case soc_genreg:
    case soc_portreg:
    case soc_ppportreg:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_pgw_mac_rsv_mask_remap) &&
            ainfo->reg == PGW_MAC_RSV_MASKr) {
            int block;
            uint8 acc_type;

            ainfo->addr = soc_reg_addr_get(unit, PGW_MAC_RSV_MASKr,
                                           ainfo->port, 0,
                                           SOC_REG_ADDR_OPTION_NONE,
                                           &block, &acc_type);
            ainfo->addr &= 0xffffff00;
            ainfo->addr |=
                (SOC_INFO(unit).port_l2p_mapping[ainfo->port] - 1) & 0xf;
            rv = _soc_reg32_get(unit, block, acc_type, ainfo->addr, &tmpdata);
            if (SOC_SUCCESS(rv)) {
                COMPILER_64_SET(*data, 0, tmpdata);
            }
            break;
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        if (ainfo->valid && (int)ainfo->reg >= 0 &&
            SOC_REG_IS_VALID(unit, ainfo->reg) &&
            SOC_REG_IS_64(unit, ainfo->reg)) {
            if (soc_feature(unit, soc_feature_new_sbus_format)) {
                rv = soc_reg_get(unit, ainfo->reg, port, ainfo->idx, data);
            } else {
                rv = soc_reg_read(unit, ainfo->reg, ainfo->addr, data);
            }
        } else {
            if (soc_feature(unit, soc_feature_new_sbus_format)) {
                rv = soc_reg32_get(unit, ainfo->reg, port, ainfo->idx,
                                   &tmpdata);
            } else {
                rv = soc_reg32_read(unit, ainfo->addr, &tmpdata);
            }
            COMPILER_64_SET(*data, 0, tmpdata);
        }
        break;

    case soc_pipereg:
    case soc_xpereg:
    case soc_itmreg:
    case soc_ebreg:
    case soc_slicereg:
    case soc_layerreg:
        if (soc_feature(unit, soc_feature_new_sbus_format)) {
            if (ainfo->valid && (int)ainfo->reg >= 0 &&
                SOC_REG_IS_VALID(unit, ainfo->reg) &&
                SOC_REG_IS_64(unit, ainfo->reg)) {
                rv = soc_reg_get(unit, ainfo->reg, ainfo->port, ainfo->idx, data);
            } else {
                rv = soc_reg32_get(unit, ainfo->reg, ainfo->port, ainfo->idx, &tmpdata);
                 COMPILER_64_SET(*data, 0, tmpdata);
            }
            break;
        }
    case soc_cosreg:
        if (ainfo->valid && (int)ainfo->reg >= 0 &&
            SOC_REG_IS_VALID(unit, ainfo->reg) &&
            SOC_REG_IS_64(unit, ainfo->reg)) {
            if (soc_feature(unit, soc_feature_new_sbus_format)) {
                rv = soc_reg_get(unit, ainfo->reg, ainfo->cos, ainfo->idx, data);
            } else {
                rv = soc_reg_read(unit, ainfo->reg, ainfo->addr, data);
            }
        } else {
            if (soc_feature(unit, soc_feature_new_sbus_format)) {
                rv = soc_reg32_get(unit, ainfo->reg, ainfo->cos, ainfo->idx, &tmpdata);
            } else {
                rv = soc_reg32_read(unit, ainfo->addr, &tmpdata);
            }
            COMPILER_64_SET(*data, 0, tmpdata);
        }
        break;
    default:
        assert(0);
        rv = SOC_E_INTERNAL;
        break;
    }

    return rv;
}


/*
 * Function:
 *      soc_anyreg_write
 * Purpose:
 *      Write a soc register according to type
 * Parameters:
 *      unit - soc unit number
 *      ainfo - address information structure
 *      data - 64 bit data.  Will type correctly for register.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This routine requires ainfo->addr to be
 *      properly set.
 *      If it is a 64-bit register, then additionally, ainfo->valid
 *      must be set and ainfo->reg must be set properly.
 */

int
soc_anyreg_write(int unit, soc_regaddrinfo_t *ainfo, uint64 data)
{
    uint32          tmpdata = 0;
    int             rv = SOC_E_NONE;
    soc_regtype_t   type;
    int             port = ainfo->port;

    if (ainfo->reg < 0) {
        type = soc_genreg;
    } else {
        if (!SOC_REG_IS_VALID(unit, ainfo->reg)) {
            return SOC_E_PARAM;
        }
        type = SOC_REG_INFO(unit, ainfo->reg).regtype;
    }

#if defined(BCM_SAND_SUPPORT)
    if (SOC_IS_SAND(unit)) {
        if ((ainfo->port >=0) || (ainfo->port & SOC_REG_ADDR_INSTANCE_MASK)) {
            port = ainfo->port;
        }  else {
            port = REG_PORT_ANY;
        }
    }
#endif

    switch (type) {
    case soc_cpureg:
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            if (soc_feature(unit, soc_feature_cpureg_dump)){
                if (soc_cpureg_address_is_invalid(unit, ainfo)) {
                    rv = SOC_E_INTERNAL;
                    break;
                }
            }
        }
#endif
        COMPILER_64_TO_32_LO(tmpdata, data);
        soc_pci_write(unit, ainfo->addr, tmpdata);
        break;
#ifdef BCM_CMICM_SUPPORT
    case soc_mcsreg:
        COMPILER_64_TO_32_LO(tmpdata, data);
        soc_pci_mcs_write(unit, ainfo->addr, tmpdata);
        break;
#endif
#ifdef BCM_IPROC_SUPPORT
    case soc_iprocreg:
        COMPILER_64_TO_32_LO(tmpdata, data);
        soc_cm_iproc_write(unit, ainfo->addr, tmpdata);
        break;
#endif
    case soc_customreg:
    case soc_genreg:
    case soc_portreg:
    case soc_ppportreg:
    case soc_pipereg:
    case soc_xpereg:
    case soc_itmreg:
    case soc_ebreg:
    case soc_slicereg:
    case soc_layerreg:
    case soc_cosreg:
        if (type == soc_cosreg) {
            port = ainfo->cos;
        }
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_pgw_mac_rsv_mask_remap) &&
            ainfo->reg == PGW_MAC_RSV_MASKr) {
            int block;
            uint8 acc_type;

            ainfo->addr = soc_reg_addr_get(unit, PGW_MAC_RSV_MASKr,
                                           ainfo->port, 0,
                                           SOC_REG_ADDR_OPTION_WRITE,
                                           &block, &acc_type);
            ainfo->addr &= 0xffffff00;
            ainfo->addr |=
                (SOC_INFO(unit).port_l2p_mapping[ainfo->port] - 1) & 0xf;
            tmpdata = COMPILER_64_LO(data);
            rv = _soc_reg32_set(unit, block, acc_type, ainfo->addr, tmpdata);
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        if (ainfo->valid && (int)ainfo->reg >= 0) {
            if (SOC_REG_IS_64(unit, ainfo->reg)) {
                if (soc_feature(unit, soc_feature_new_sbus_format)) {
                    rv = soc_reg64_set(unit, ainfo->reg, port, ainfo->idx, data);
                } else {
                    rv = soc_reg64_write(unit, ainfo->addr, data);
                }
            } else {
                COMPILER_64_TO_32_LO(tmpdata, data);
                if (soc_feature(unit, soc_feature_new_sbus_format)) {
                    rv = soc_reg32_set(unit, ainfo->reg, port, ainfo->idx, tmpdata);
                } else {
                    rv = soc_reg32_write(unit, ainfo->addr, tmpdata);
                }
            }
#if defined(BCM_XGS_SUPPORT)
            if (soc_feature(unit, soc_feature_regs_as_mem)) {
                if (SOC_REG_IS_64(unit, ainfo->reg)) {
                    (void)soc_ser_reg_cache_set(unit, ainfo->reg, port < 0 ? 0 : port,
                                                ainfo->idx, data);
                } else {
                    (void)soc_ser_reg32_cache_set(unit, ainfo->reg, port < 0 ? 0 : port,
                                                  ainfo->idx < 0 ? 0 : ainfo->idx, tmpdata);
                }
            }
#endif /* BCM_XGS_SUPPORT */
        } else { /* Left around for some legacy support */
            rv = soc_reg32_write(unit, ainfo->addr, tmpdata);
        }
        break;
    default:
        assert(0);
        rv = SOC_E_INTERNAL;
        break;
    }
    return rv;
}

#define UPDATE_BP(bp) while (*(bp)) (bp)++;

/* This is inefficient, because it does a lookup for each field. */
void
soc_reg_sprint_data(int unit, char *bp, char *infix, soc_reg_t reg,
                    uint32 value)
{
    int             i, n;
    soc_field_t     field;
    int             len;
    char            buf[24];

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return;
    }
    n = SOC_REG_INFO(unit, reg).nFields;
    for (i = 0; i < n; i++) {
        field = SOC_REG_INFO(unit, reg).fields[i].field;
        len = SOC_REG_INFO(unit, reg).fields[i].len;
        if (i == n - 1) {
            infix = "";
        }
#if defined(SOC_NO_NAMES)
        sal_sprintf(buf, "#%%d = 0x%%0%dx%%s", (len + 3) >> 2);
        sal_sprintf(bp, buf, field,
                    soc_reg_field_get(unit, reg, value, field), infix);
#else
        sal_sprintf(buf, "%%s = 0x%%0%dx%%s", (len + 3) >> 2);
        sal_sprintf(bp, buf, soc_fieldnames[field],
                    soc_reg_field_get(unit, reg, value, field), infix);
#endif
        UPDATE_BP(bp);
    }
}

void
soc_reg_sprint_addr(int unit, char *bp, soc_regaddrinfo_t *ainfo)
{
    soc_field_info_t *finfop = 0;
    int             msb, lsb;
    soc_reg_info_t *reginfo;
    int len = 0;

    if (!ainfo->valid) {
        len = sal_strlen("Invalid Address");
        sal_strncpy(bp, "Invalid Address", len);
        bp[len] = '\0';
        return;
    }
    if (ainfo->reg != INVALIDr && SOC_REG_IS_VALID(unit, ainfo->reg)) {
        reginfo = &SOC_REG_INFO(unit, ainfo->reg);
        if (ainfo->field != INVALIDf) {
            SOC_FIND_FIELD(ainfo->field,
                           reginfo->fields,
                           reginfo->nFields,
                           finfop);
            assert(finfop);
            if (finfop->flags & SOCF_LE) {
                msb = finfop->bp + finfop->len - 1;
                lsb = finfop->bp;
            } else {
                msb = finfop->bp;
                lsb = finfop->bp + finfop->len - 1;
            }
            sal_sprintf(bp, "[%d:%d] ", msb, lsb);
            UPDATE_BP(bp);
        }
    } else {
        len = sal_strlen("Reserved");
        sal_strncpy(bp, "Reserved", len);
        bp[len] = '\0';
        return;
    }

#ifdef SOC_NO_NAMES
    sal_sprintf(bp, "#%d", ainfo->reg);
#else
    sal_sprintf(bp, "%s", SOC_REG_NAME(unit, ainfo->reg));
#endif /* SOC_NO_NAMES */
    UPDATE_BP(bp);

    if (SOC_REG_ARRAY(unit, ainfo->reg) || SOC_REG_IS_ARRAY(unit, ainfo->reg)) {
        sal_sprintf(bp, "(%d)", ainfo->idx);
        UPDATE_BP(bp);
    }
    if (ainfo->cos != -1) {
        sal_sprintf(bp, ".%d", ainfo->cos);
        UPDATE_BP(bp);
    }
    if (((reginfo->regtype != soc_portreg) &&
        (reginfo->regtype != soc_ppportreg) &&
        (reginfo->regtype != soc_customreg))
        || (soc_is_reg_flexe_core(unit, ainfo->reg))) {
        *bp++ = '.';
        if ((reginfo->regtype == soc_xpereg) &&
            (ainfo->port & SOC_REG_ADDR_INSTANCE_MASK)) {
            /* port here represents XPE instance ID. */
            sal_sprintf(bp, "mmu_xpe%d", ainfo->port & 0xF);
        } else {
            len = sal_strlen(SOC_BLOCK_NAME(unit, ainfo->block));
            sal_strncpy(bp, SOC_BLOCK_NAME(unit, ainfo->block), len);
            if (len)
                bp[len] = '\0';
        }
        UPDATE_BP(bp);
    }
    else if (reginfo->regtype == soc_ppportreg) {
        *bp++ = '.';
        sal_sprintf(bp, "pp%d", ainfo->port);
        UPDATE_BP(bp);
    }
    else if (reginfo->regtype == soc_portreg || reginfo->regtype == soc_customreg) {
        *bp++ = '.';
        len = sal_strlen(SOC_PORT_NAME(unit, ainfo->port));
        sal_strncpy(bp, SOC_PORT_NAME(unit, ainfo->port), len);
        bp[len] = '\0';
        UPDATE_BP(bp);
    }
    if (finfop) {
#ifdef SOC_NO_NAMES
        sal_sprintf(bp, ".#%d", ainfo->field);
#else
        sal_sprintf(bp, ".%s", soc_fieldnames[ainfo->field]);
#endif /* SOC_NO_NAMES */
        UPDATE_BP(bp);
    }
}
#undef  UPDATE_BP

#define SOC_E_IGNORE -6000

/*
 * This handles the iteration for a particular register addr info
 * It does it across all array entries if the register is an array reg.
 */
static int
_do_reg_iter(int unit, soc_reg_iter_f do_it,
             soc_regaddrinfo_t *ainfo, void *data, int *done)
{
    int             first_array_idx, idx;
    int             num_els, gran;
    int             rv = SOC_E_NONE;
    uint32          base_addr;
    soc_reg_t       reg;

    /* The granularity of register addresses, indexed by soc_regtype_t */
    /* new bus format */
    int soc_regtype_gran_new_bus[] = {
        1,      /* soc_schan_reg,    */
        1 << 8, /* soc_genreg,       */
        1,      /* soc_portreg,      */
        1,      /* soc_ppportreg,    */
        1,      /* soc_cosreg,       */
        1 << 8, /* soc_pipereg,      */
        1 << 8, /* soc_xpereg,       */
        1 << 8, /* soc_slicereg      */
        1 << 8, /* soc_layerreg      */
        4,      /* soc_cpureg,       */
        4,      /* soc_pci_cfg_reg,  */
        1,      /* soc_phy_reg,      */
        1,      /* soc_spi_reg,      */
        4,      /* soc_mcsreg,       */
        4,      /* soc_iprocreg,     */
        4,      /* soc_hostmem_w,    */
        2,      /* soc_hostmem_h,    */
        1,      /* soc_hostmem_b,    */
        1 << 8, /* soc_customreg,    */
        0,      /* soc_invalidreg    */
    };
    int reg_type_no = 0;

    reg = ainfo->reg;
    reg_type_no = SOC_REG_INFO(unit, reg).regtype;
    if (ainfo->reg == INVALIDr || !SOC_REG_IS_VALID(unit, ainfo->reg) ||
        reg_type_no >= COUNTOF(soc_regtype_gran_new_bus)) {
        return SOC_E_PARAM;
    }

    base_addr = ainfo->addr;
    gran = (soc_feature(unit, soc_feature_new_sbus_format))?
           soc_regtype_gran_new_bus[reg_type_no]:
           SOC_REG_GRAN(unit, reg);

    if (SOC_IS_GREYHOUND2(unit) &&
        (SOC_REG_INFO(unit, reg).regtype == soc_genreg) &&
        (SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block) == SOC_BLK_TAF)) {
        gran = 1;
    }

    if (SOC_REG_ARRAY(unit, reg) || SOC_REG_IS_ARRAY(unit, reg)) {
        num_els = SOC_REG_NUMELS(unit, reg);
        first_array_idx = SOC_REG_INFO(unit, reg).first_array_index;
    } else {
        num_els = 1;
        first_array_idx = 0;
    }

    for (idx = first_array_idx; (idx < first_array_idx + num_els) && !*done; idx++) {
        if (SOC_REG_IS_ARRAY(unit, reg)) {
            ainfo->addr = base_addr + (idx - first_array_idx) * SOC_REG_ELEM_SKIP(unit, reg);
        } else if (SOC_REG_ARRAY(unit, reg)) {
            ainfo->addr = base_addr + (idx * gran);
            if (SOC_REG_ARRAY2(unit, reg)) {
                ainfo->addr = base_addr + (idx * gran * 2);
            } else if (SOC_REG_ARRAY4(unit, reg)) {
                ainfo->addr = base_addr + (idx * gran * 4);
            }
        } else {
            ainfo->addr = base_addr + (idx * gran); /* idx == 0 */
        }
        ainfo->idx = idx;
        if ((SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_NO_DGNL) &&
            (idx == ainfo->port)) {
            continue;
        }
        rv = (*do_it)(unit, ainfo, data);
        switch (rv) {
        case SOC_E_NONE:
            break;
        case SOC_E_IGNORE:
            rv = SOC_E_NONE;
            *done = 1;
            break;
        default:
            *done = 1;
            break;
        }
    }

    ainfo->addr = base_addr;
    return rv;
}

#ifdef BCM_KATANA2_SUPPORT
/* EMC registers common to KT2 and SB2*/
static soc_reg_t emc_regs[] =
{
    CI0_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr,
    CI0_TO_EMC_WTAG_RETURN_COUNT_DEBUGr,
    CI1_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr,
    CI1_TO_EMC_WTAG_RETURN_COUNT_DEBUGr,
    EMC_BUFFER_EMPTY_FULL_STATUS_DEBUGr,
    EMC_CFGr,
    EMC_CI_FULL_STATUS_DEBUGr,
    EMC_CSDB_0_BUFFER_PAR_STATUS_DEBUGr,
    EMC_CSDB_1_BUFFER_PAR_STATUS_DEBUGr,
    EMC_CSDB_MEM_DEBUGr,
    EMC_ECC_DEBUG_0r,
    EMC_ECC_DEBUG_1r,
    EMC_ERRORr,
    EMC_ERROR_0r,
    EMC_ERROR_1r,
    EMC_ERROR_2r,
    EMC_ERROR_3r,
    EMC_ERROR_MASK_0r,
    EMC_ERROR_MASK_1r,
    EMC_ERROR_MASK_2r,
    EMC_ERROR_MASK_3r,
    EMC_FREE_POOL_SIZESr,
    EMC_GLOBAL_1B_ECC_ERROR_COUNT_DEBUGr,
    EMC_GLOBAL_2B_ECC_ERROR_COUNT_DEBUGr,
    EMC_IRRB_BUFFER_ECC_STATUS_DEBUGr,
    EMC_IRRB_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_IRRB_BUFFER_WATERMARK_DEBUGr,
    EMC_IRRB_THRESHOLDSr,
    EMC_RFCQ_BUFFER_ECC_STATUS_DEBUGr,
    EMC_RFCQ_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_RFCQ_BUFFER_WATERMARK_DEBUGr,
    EMC_RFCQ_MEM_DEBUGr,
    EMC_RSFP_BUFFER_ECC_STATUS_DEBUGr,
    EMC_RSFP_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_RSFP_MEM_DEBUGr,
    EMC_SWAT_BUFFER_ECC_STATUS_DEBUGr,
    EMC_SWAT_MEM_DEBUGr,
    EMC_TO_CI0_RD_REQ_COUNT_DEBUGr,
    EMC_TO_CI0_WR_REQ_COUNT_DEBUGr,
    EMC_TO_CI1_RD_REQ_COUNT_DEBUGr,
    EMC_TO_CI1_WR_REQ_COUNT_DEBUGr,
    EMC_WLCT_MEM_DEBUGr,
    EMC_WLCT_MERGED_RETURN_WTAG_FIFO_MEM_DEBUGr,
    EMC_WLCT_MERGED_RETURN_WTAG_FIFO_NFULL_THRESHOLDr,
    EMC_WTFP_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WTFP_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_WTFP_MEM_DEBUGr,
    EMC_WTOQ_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WTOQ_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_WTOQ_MEM_DEBUGr,
    ITE_TO_EMC_WR_REQ_SOP_COUNT_DEBUGr,
    MMU_ITE_EMC_BACKPRESSURE_DEBUG_CONFIGr,
    MMU_ITE_EMC_BACKPRESSURE_DEBUG_STATUSr
};

/* EMC registers of KT2 */
static soc_reg_t kt2_emc_regs[] =
{
    CI2_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr,
    CI2_TO_EMC_WTAG_RETURN_COUNT_DEBUGr,
    CI3_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr,
    CI3_TO_EMC_WTAG_RETURN_COUNT_DEBUGr,
    CI4_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr,
    CI4_TO_EMC_WTAG_RETURN_COUNT_DEBUGr,
    CI5_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr,
    CI5_TO_EMC_WTAG_RETURN_COUNT_DEBUGr,
    EMC_CSDB_2_BUFFER_PAR_STATUS_DEBUGr,
    EMC_CSDB_3_BUFFER_PAR_STATUS_DEBUGr,
    EMC_CSDB_4_BUFFER_PAR_STATUS_DEBUGr,
    EMC_CSDB_5_BUFFER_PAR_STATUS_DEBUGr,
    EMC_ERRB_0_BUFFER_ECC_STATUS_DEBUGr,
    EMC_ERRB_0_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_ERRB_0_BUFFER_WATERMARK_DEBUGr,
    EMC_ERRB_0_MEM_DEBUGr,
    EMC_ERRB_1_BUFFER_ECC_STATUS_DEBUGr,
    EMC_ERRB_1_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_ERRB_1_BUFFER_WATERMARK_DEBUGr,
    EMC_ERRB_1_MEM_DEBUGr,
    EMC_ERRB_FIFO_NFULL_THRESHOLDr,
    EMC_EWRB_0_BUFFER_0_ECC_STATUS_DEBUGr,
    EMC_EWRB_0_BUFFER_1_ECC_STATUS_DEBUGr,
    EMC_EWRB_0_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_EWRB_0_BUFFER_WATERMARK_DEBUGr,
    EMC_EWRB_0_MEM_DEBUGr,
    EMC_EWRB_1_BUFFER_0_ECC_STATUS_DEBUGr,
    EMC_EWRB_1_BUFFER_1_ECC_STATUS_DEBUGr,
    EMC_EWRB_1_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_EWRB_1_BUFFER_WATERMARK_DEBUGr,
    EMC_EWRB_1_MEM_DEBUGr,
    EMC_EWRB_FIFO_NFULL_THRESHOLDr,
    EMC_IWRB_BUFFER_ECC_STATUS_DEBUGr,
    EMC_IWRB_BUFFER_FILL_LEVEL_DEBUGr,
    EMC_IWRB_BUFFER_WATERMARK_DEBUGr,
    EMC_IWRB_MEM_DEBUGr,
    EMC_IWRB_SIZEr,
    EMC_TO_CI2_RD_REQ_COUNT_DEBUGr,
    EMC_TO_CI2_WR_REQ_COUNT_DEBUGr,
    EMC_TO_CI3_RD_REQ_COUNT_DEBUGr,
    EMC_TO_CI3_WR_REQ_COUNT_DEBUGr,
    EMC_TO_CI4_RD_REQ_COUNT_DEBUGr,
    EMC_TO_CI4_WR_REQ_COUNT_DEBUGr,
    EMC_TO_CI5_RD_REQ_COUNT_DEBUGr,
    EMC_TO_CI5_WR_REQ_COUNT_DEBUGr,
    EMC_WCMT_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WCMT_MEM_DEBUGr,
    EMC_WLCT0_LOWER_A_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT1_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr,
    EMC_WLCT0_LOWER_B_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT0_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr,
    EMC_WLCT0_UPPER_A_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT0_UPPER_B_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT1_LOWER_A_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT1_LOWER_B_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT1_UPPER_A_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT1_UPPER_B_BUFFER_ECC_STATUS_DEBUGr,
    EXT_BUFFER_POOL_CONG_STATEr,
    EMC_WLCT2_LOWER_A_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT2_LOWER_B_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT2_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr,
    EMC_WLCT2_UPPER_A_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT2_UPPER_B_BUFFER_ECC_STATUS_DEBUGr
};

static soc_reg_t sb2_emc_regs[] =
{
    EMC_FIXED_PATTERN_DEBUGr,
    EMC_IRRB_MEM_DEBUGr,
    EMC_TO_ITE_WR_REQ_PKT_COMPLETE_COUNT_DEBUGr,
    EMC_WLCT_BUFFER_ECC_STATUS_DEBUGr,
    EMC_WLCT_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr,
    EMC_WLCT_MERGED_RETURN_WTAG_FIFO_FILL_LEVEL_DEBUGr,
    EMC_WLCT_MERGED_RETURN_WTAG_FIFO_WATERMARK_DEBUGr,
    ITE_TO_EMC_WR_REQ_EOP_COUNT_DEBUGr
};

static int
_is_emc_reg(int unit, soc_reg_t reg)
{
    int i;

    /* EMC registers common to KT2 and SB2*/
    for (i = 0; i < (sizeof(emc_regs)/sizeof(soc_reg_t)); i++) {
        if (emc_regs[i] == reg) {
            return 1;
        }
    }

    /* EMC registers of KT2 */
    if (SOC_IS_KATANA2(unit) && !(SOC_IS_SABER2(unit))){
        for (i = 0; i < (sizeof(kt2_emc_regs)/sizeof(soc_reg_t)); i++) {
            if (kt2_emc_regs[i] == reg) {
                return 1;
            }
        }
    }

    /* EMC registers of SB2 */
    if (SOC_IS_SABER2(unit)) {
        for (i = 0; i < (sizeof(sb2_emc_regs)/sizeof(soc_reg_t)); i++) {
            if (sb2_emc_regs[i] == reg) {
                return 1;
            }
        }
    }

    return 0;
}
#endif

int
soc_reg_iterate(int unit, soc_reg_iter_f do_it, void *data)
{
    soc_reg_t       reg;
    soc_port_t      port;
    soc_cos_t       cos;
    int             blk;
    soc_block_types_t regblktype;
    soc_regaddrinfo_t ainfo;
    int             done;
#ifdef BCM_TRIUMPH2_SUPPORT
    int             skip_cpu;
#endif
    int             phy_port;
    int             port_num_blktype;
    int             i;
    int             idx_min, idx_max, idx;
    soc_regtype_t   regtype;

    ainfo.valid = 1;
    ainfo.field = INVALIDf;

#ifdef BCM_TRIUMPH2_SUPPORT
    skip_cpu = soc_property_get(unit, "cmic_regtest_skip", 1);
#endif

    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        if (!SOC_REG_IS_ENABLED(unit, reg)) {
            continue;
        }
        done = 0;
        ainfo.reg = reg;
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit)) {
            if (SOC_REG_ARRAY(unit, reg) || SOC_REG_IS_ARRAY(unit, reg)) {
                ainfo.idx = SOC_REG_INFO(unit, reg).first_array_index;
            } else {
                ainfo.idx = 0;
            }
        }
#endif
        regblktype = SOC_REG_INFO(unit, reg).block;
        if (SAL_BOOT_QUICKTURN && (SOC_BLOCK_IS(regblktype, SOC_BLK_BSAFE))) {
            continue;
        }

        regtype = SOC_REG_INFO(unit, reg).regtype;
        if (soc_is_reg_flexe_core(unit, reg))
        {
            regtype = soc_genreg;
        }
        switch (regtype) {
        case soc_ppportreg:
            PBMP_PP_ALL_ITER(unit, port) {
                ainfo.port = port;
                ainfo.addr = soc_reg_addr_get(unit, reg, ainfo.port, 0,
                                              SOC_REG_ADDR_OPTION_NONE,
                                              &ainfo.block, &ainfo.acc_type);
                ainfo.cos = -1;
                _do_reg_iter(unit, do_it, &ainfo, data, &done);
                if (done) {
                    break;
                }
            }
            break;
        case soc_customreg:
        case soc_portreg:
            PBMP_ALL_ITER(unit, port) {
                if (SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_PORT)) {
                    if (SOC_BLOCK_IS(regblktype, SOC_BLK_PGW_CL)) {
                        port_num_blktype = SOC_DRIVER(unit)->port_num_blktype;
                        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                        blk = -1;
                        for (i = 0; i < port_num_blktype; i++) {
                            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                            if (blk < 0) {
                                break;
                            }
                            if (SOC_REG_BLOCK_IN_LIST(unit, reg,
                                             SOC_BLOCK_TYPE(unit, blk))) {
                                break;
                            }
                        }
                        if (blk < 0) {
                            continue;
                        }
                    } else if (!SOC_REG_BLOCK_IN_LIST(unit, reg,
                                             SOC_PORT_TYPE(unit, port))) {
                        continue;
                    }
                }
#ifdef BCM_FIREBOLT_SUPPORT
                if (SOC_IS_FBX(unit)) {
                    if (SOC_BLOCK_IS(regblktype, SOC_BLK_IPIPE_HI) &&
                        ((!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit,
                                        IPIPE_HI_BLOCK(unit)), port)))) {
                        continue;
                    }
                    if (SOC_BLOCK_IS(regblktype, SOC_BLK_EPIPE_HI) &&
                        ((!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit,
                                        EPIPE_HI_BLOCK(unit)), port)))) {
                        continue;
                    }
                }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
                if(SOC_IS_GREYHOUND(unit) && (SOC_BLOCK_IS(regblktype, SOC_BLK_XLPORT)||
                    SOC_BLOCK_IS(regblktype, SOC_BLK_GXPORT))){
                    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype;
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    blk = -1;
                    for (i = 0; i < port_num_blktype; i++) {
                        if (regblktype[0] == SOC_BLK_XLPORT){
                            if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port,
                                NULL, &blk, NULL, 0) > 0){
                                /* get the correct blk */
                                break;
                            }
                        }
                        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                        if (blk < 0) {
                            break;
                        }
                        if (SOC_BLOCK_IS(regblktype,
                                         SOC_BLOCK_TYPE(unit, blk))) {
                            break;
                        }
                    }
                    if (blk <= 0) {
                        continue;
                    }
                    if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port,
                                                NULL, NULL, NULL, 0) == SOC_E_NOT_FOUND){
                        /* port register not belong to pgw_gx and pgw_xl */
                        if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, blk), port)){
                            /* check the valid blk bitmap*/
                            continue;
                        }
                    } else if (regblktype[0] == SOC_BLK_XLPORT){
                        if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port,
                            NULL, &blk, NULL, 1) > 0){
                            /* check the valid blk */
                            continue;
                        }
                    }
                }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    if ((regblktype[0] == SOC_BLK_CDMAC) &&
                                    IS_CPU_PORT(unit,port)) {
                        continue;
                    }
                }
#endif
                ainfo.port = port;
                ainfo.addr = soc_reg_addr_get(unit, reg, ainfo.port, 0,
                                              SOC_REG_ADDR_OPTION_NONE,
                                              &ainfo.block, &ainfo.acc_type);
                ainfo.block = -1;
                ainfo.cos = -1;
                _do_reg_iter(unit, do_it, &ainfo, data, &done);
                if (done) {
                    break;
                }
            }
            break;
        case soc_cosreg:
            for (cos = 0; cos < NUM_COS(unit) && !done; cos++) {
                SOC_BLOCKS_ITER(unit, blk, regblktype) {
                    ainfo.port = SOC_BLOCK_PORT(unit, blk);
                    ainfo.addr = soc_reg_addr_get(unit, reg, ainfo.port, cos,
                                                  SOC_REG_ADDR_OPTION_NONE,
                                                  &ainfo.block, &ainfo.acc_type);
                    ainfo.block = blk;
                    ainfo.cos = cos;
                    _do_reg_iter(unit, do_it, &ainfo, data, &done);
                    if (done) {
                        break;
                    }
                }
            }
            break;
        case soc_genreg:
            if (SOC_BLOCK_IS(regblktype, SOC_BLK_ESM) &&
                (!soc_feature(unit, soc_feature_esm_support) || (SOC_CONTROL(unit)->tcam_info == NULL))) {
                break;
            }
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if (_is_emc_reg(unit, reg) || SOC_BLOCK_IS(regblktype, SOC_BLK_CI)) {
                    if (soc_feature(unit, soc_feature_ddr3)) {
                        /* Don't read external RAM related registers if same
                         * is not present or not configured */
                        if (!(soc_property_get(unit, spn_EXT_RAM_PRESENT, 0) &&
                               soc_property_get(unit, spn_PBMP_EXT_MEM, 0))) {
                            break;
                        }
                    } else {
                        /* Don't read external RAM related regs if
                         * feature not supported */
                        break;
                    }
                }
            }
#endif
            SOC_BLOCKS_ITER(unit, blk, regblktype) {
                switch (regblktype[0]) {
                case SOC_BLK_PMQPORT:
                case SOC_BLK_QTGPORT:
                case SOC_BLK_PMQ:
                case SOC_BLK_PGW_CL:
                    ainfo.port = SOC_REG_ADDR_INSTANCE_MASK |
                        SOC_BLOCK_NUMBER(unit, blk);
                    break;

#ifdef BCM_MONTEREY_SUPPORT
                case SOC_BLK_CPRI:
                        ainfo.port = SOC_BLOCK_PORT(unit, blk);
                        break;
#endif
                default:
                    ainfo.port = SOC_BLOCK_PORT(unit, blk);
                    break;
                }
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit) && ((regblktype[0] == SOC_BLK_CDPORT) || (regblktype[0] == SOC_BLK_CLPORT) ||
                    (regblktype[0] == SOC_BLK_CDMAC) || (regblktype[0] == SOC_BLK_CLMAC))) {
                    dnx_startup_test_function_f test_function = dnx_startup_test_functions[unit];
                    if(test_function == NULL) {
                        bcm_pbmp_t valid_ports;
                        bcm_port_t valid_port;
                        int phy_port = 0;
                        int cd_block = 0;
                        if (dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &valid_ports) < 0)   /* Get NIF logical port bitmap */
                        {
                            break;
                        }
                        BCM_PBMP_ITER(valid_ports, valid_port)
                        {
                            phy_port = SOC_INFO(unit).port_l2p_mapping[valid_port]; /* Get NIF port first phy */
                            cd_block = SOC_PORT_BLOCK(unit, phy_port);
                            if (blk == cd_block)
                            {
                                break;
                            }
                        }
                        if (blk != cd_block)
                        {
                            continue;
                        }
                    }
                }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
                if (SOC_IS_GREYHOUND(unit) && ((regblktype[0] == SOC_BLK_XLPORT) ||
                    (regblktype[0] == SOC_BLK_GXPORT))) {
                    if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, ainfo.port,
                                                NULL, NULL, NULL, 0) == SOC_E_NOT_FOUND){
                        /* port register not belong to pgw_gx and pgw_xl */
                        if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, blk), ainfo.port)){
                            /* check the valid blk bitmap*/
                            continue;
                        }
                    } else if (regblktype[0] == SOC_BLK_XLPORT){
                        if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, ainfo.port,
                            NULL, &blk, NULL, 1) > 0){
                            /* check the valid blk */
                            continue;
                        }
                    }
                }
#endif

#ifdef BCM_PETRA_SUPPORT
                if ((SOC_IS_ARAD(unit)) && (regblktype[0] == SOC_BLK_GPORT)) {
                    break;
                }
#endif /*BCM_PETRA_SUPPORT*/

#ifdef BCM_HURRICANE3_SUPPORT
                if ((SOC_IS_HURRICANE3(unit)) &&
                    (regblktype[0] == SOC_BLK_PGW_GE)) {

                    ainfo.port = SOC_REG_ADDR_INSTANCE_MASK |
                        SOC_BLOCK_NUMBER(unit, blk);
                }
#endif

#ifdef BCM_MONTEREY_SUPPORT
                if (regblktype[0] == SOC_BLK_CPRI && !IS_ROE_PORT(unit, ainfo.port )) {
		    continue;
                }
#endif
                ainfo.addr = soc_reg_addr_get(unit, reg, ainfo.port,
                                              (SOC_REG_ARRAY(unit, reg) || SOC_REG_IS_ARRAY(unit, reg)) ? SOC_REG_INFO(unit, reg).first_array_index : 0,
                                              SOC_REG_ADDR_OPTION_NONE,
                                              &ainfo.block, &ainfo.acc_type);

                ainfo.block = blk;
                ainfo.cos = -1;
                if (!soc_feature(unit, soc_feature_esm_support) &&
                   soc_feature(unit, soc_feature_new_sbus_format) &&
                   SOC_BLOCK_IS(regblktype, SOC_BLK_IPIPE)) { /* Needs update per chip */
                    if (SOC_IS_TRIUMPH3(unit) &&
                        SOC_REG_ADDR_STAGE(ainfo.addr) == 0x9) { /* IESMIF */
                        continue;
                    }
                }
                _do_reg_iter(unit, do_it, &ainfo, data, &done);
                if (done) {
                    break;
                }
            }
            break;
        case soc_cpureg:
            /* Skip CPU registers in soc iteration. */
#ifdef BCM_TRIUMPH2_SUPPORT
            if (SOC_IS_TRIUMPH2(unit) && !skip_cpu) {
                SOC_BLOCKS_ITER(unit, blk, regblktype) {
                    ainfo.port = SOC_BLOCK_PORT(unit, blk);
                    ainfo.addr = soc_reg_addr(unit, reg, ainfo.port, 0);
                    ainfo.block = blk;
                    ainfo.cos = -1;
                    _do_reg_iter(unit, do_it, &ainfo, data, &done);
                    if (done) {
                        break;
                    }
                }
            }
#endif
            break;
        case soc_mcsreg:
        case soc_iprocreg:
            /* Skip MCS registers in soc iteration. */
            break;
        case soc_pipereg:
        case soc_xpereg:
        case soc_itmreg:
        case soc_ebreg:
        case soc_slicereg:
        case soc_layerreg:
            idx_min = 0;
            switch (SOC_REG_INFO(unit, reg).regtype) {
            case soc_pipereg:
                idx_max = NUM_PIPE(unit) - 1;
                break;
            case soc_xpereg:
                idx_max = NUM_XPE(unit) - 1;
                break;
            case soc_itmreg:
                idx_max = NUM_ITM(unit) - 1;
                break;
            case soc_ebreg:
                idx_max = NUM_EB(unit) - 1;
                break;
            case soc_slicereg:
                idx_max = NUM_SLICE(unit) - 1;
                break;
            case soc_layerreg:
                idx_max = NUM_LAYER(unit) - 1;
                break;
            default:
                assert(0);
                return 0;
            }

            for (idx = idx_min; idx <= idx_max && !done; idx++) {
                SOC_BLOCKS_ITER(unit, blk, regblktype) {
                    ainfo.port = SOC_REG_ADDR_INSTANCE_MASK | idx;
                    ainfo.addr = soc_reg_addr_get(unit, reg, ainfo.port, 0,
                                                  SOC_REG_ADDR_OPTION_NONE,
                                                  &ainfo.block,
                                                  &ainfo.acc_type);
                    ainfo.block = blk;
                    ainfo.cos = -1;
                    _do_reg_iter(unit, do_it, &ainfo, data, &done);
                    if (done) {
                        break;
                    }
                }
            }
            break;
        default:
            assert(0);
            break;
        }
    }

    return 0;
}

/*
 * Given an address, fill in an regaddrinfo structure
 * This can only be called for SOC registers, not for CPU registers
 */
void
soc_regaddrinfo_get(int unit, soc_regaddrinfo_t *ainfo, uint32 addr)
{
    int                 i, idx;
    int                 blk = 0, bindex, port, phy_port;
    uint32              minadr, maxadr;
    soc_regtype_t       regtype;
    int                 reg;
    uint32              offset;         /* base address */
    int                 block = -1;     /* block number */
    int                 pindex;         /* port/cos index field */
    int                 aindex = 0;     /* array index field */
    soc_block_t         portblktype;
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_reg_t reg_excep_list[]={MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                                MMU_RQE_REPL_PORT_AGG_MAPr};
    int reg_index, num_regs, add_exception = 0;
#endif


    ainfo->addr = addr;
    ainfo->valid = 1;
    ainfo->reg = INVALIDr;

    {
        portblktype = SOC_BLK_PORT;
    }
    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        block = ((addr >> SOC_BLOCK_BP) & 0xf) |
                (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        /* extract fields from addr */
        if (SOC_IS_HERCULES(unit)) {
            offset = addr & 0xfffff;
            pindex = 0;
            aindex = 0;
#if defined(BCM_TRIDENT_SUPPORT)
        } else if (SOC_IS_TD_TT(unit)) {
            offset = addr & 0x3f080fff;
            pindex = (addr >> SOC_REGIDX_BP) & 0x7f;
            aindex = addr & 0xfff;
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
        } else if (SOC_IS_FBX(unit)) {
            offset = addr & 0x3f0c0fff;
            pindex = (addr >> SOC_REGIDX_BP) & 0x3f;
            if (addr & 0x80000000) {
                pindex += 64;
            }
            aindex = addr & 0xfff;
#endif
        } else {
            offset = addr & 0xc0fff;
            pindex = (addr >> SOC_REGIDX_BP) & 0x3f;
            aindex = addr & 0xfff;
        }
    } else {
        pindex = addr & 0xff;
        if (addr & 0x02000000) {
            offset = addr;
        } else {
            offset = addr & 0xffffff00;
        }
    }

    /* find the matching block */
    /* we find the last matching block to make things work with cmic/cpic */
    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        blk = -1;
        for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
            if (SOC_BLOCK_INFO(unit, i).cmic == block) {
                blk = i;
                /* break; */
            }
        }
    }
    assert(blk >= 0);
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        minadr = SOC_REG_INFO(unit, reg).offset;
        if (offset < minadr) {
            continue;
        }
        if (SOC_REG_IS_ARRAY(unit, reg)) {
            maxadr = minadr + (SOC_REG_NUMELS(unit, reg) - 1)*SOC_REG_ELEM_SKIP(unit, reg);
        } else if (SOC_REG_ARRAY(unit, reg)) {
            if (SOC_REG_ARRAY2(unit, reg)) {
                maxadr = minadr +
                    2 * (SOC_REG_GRAN(unit, reg) * (SOC_REG_NUMELS(unit, reg) - 1));
            } else if (SOC_REG_ARRAY4(unit, reg)) {
                maxadr = minadr +
                    4 * (SOC_REG_GRAN(unit, reg) * (SOC_REG_NUMELS(unit, reg) - 1));
            } else {
                maxadr = minadr +
                    (SOC_REG_GRAN(unit, reg) * (SOC_REG_NUMELS(unit, reg) - 1));
            }
        } else {
            maxadr = minadr;
        }
        if (offset > maxadr) {
            continue;
        }

        if (SOC_REG_ARRAY2(unit, reg)) {
            if ( (offset & 0x1) != (minadr & 0x1) ) {
                /* make sure last bit are same as the base addr when array stride is 2 */
                continue;
            }
        }
        if (SOC_REG_ARRAY4(unit, reg)) {
            if ( (offset & 0x11) != (minadr & 0x11) ) {
                /* make sure last two bit are same as the base addr when array stride is 4 */
                continue;
            }
        }
        regtype = SOC_REG_INFO(unit, reg).regtype;
        if (soc_is_reg_flexe_core(unit, reg))
        {
            regtype = soc_genreg;
        }
        if ((regtype == soc_cpureg) || (regtype == soc_mcsreg) || (regtype == soc_iprocreg))  {
            continue;
        }
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            if (!SOC_BLOCK_IS_TYPE(unit, blk, SOC_REG_INFO(unit, reg).block)) {
                continue;
            }
        }

        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            ainfo->block = blk;
        }

        if (SOC_REG_IS_ARRAY(unit, reg)) {
            for(idx=0 ; idx<SOC_REG_NUMELS(unit, reg) ; idx++) {
                if(minadr+idx*SOC_REG_ELEM_SKIP(unit, reg) == offset) {
                    ainfo->reg = reg;
                    ainfo->field = INVALIDf;
                    ainfo->cos = -1;
                    ainfo->port = -1;
                    ainfo->idx = idx;
                    return;
                }
            }
            continue;
        }

        if (SOC_REG_ARRAY(unit, reg)) {
            if (soc_feature(unit, soc_feature_new_sbus_format)) {
                aindex = (offset - SOC_REG_BASE(unit, reg)) >> 8;
            } else {
                aindex = (offset - SOC_REG_BASE(unit, reg));
            }
            if (SOC_REG_ARRAY2(unit, reg)) {
                ainfo->idx = aindex / (2 * SOC_REG_GRAN(unit, reg));
            } else if (SOC_REG_ARRAY4(unit, reg)) {
                ainfo->idx = aindex / (4 * SOC_REG_GRAN(unit, reg));
            } else {
                ainfo->idx = aindex / SOC_REG_GRAN(unit, reg);
            }
        } else {
            ainfo->idx = -1;
        }

        ainfo->reg = reg;
        ainfo->field = INVALIDf;
        ainfo->cos = -1;
        ainfo->port = -1;
        switch (regtype) {
        case soc_customreg:
        case soc_ppportreg:
        case soc_portreg:
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, portblktype)) {
                for (phy_port = 0; ; phy_port++) {
                    if (soc_feature(unit, soc_feature_logical_port_num)) {
                        port = SOC_INFO(unit).port_p2l_mapping[phy_port];
                        if (port < 0) {
                            continue;
                        }
                    } else {
                        port = phy_port;
                    }
                    blk = SOC_PORT_BLOCK(unit, phy_port);
                    bindex = SOC_PORT_BINDEX(unit, phy_port);
                    if (blk < 0 && bindex < 0) {        /* end of list */
                        break;
                    }
                    if (blk < 0) {                      /* empty slot */
                        continue;
                    }
                    
                    if ((uint32)blk == ainfo->block && bindex == pindex) {
                        ainfo->port = port;
                        break;
                    }
                }
                if (ainfo->port == -1) {
                    ainfo->reg = INVALIDr;
                }
            } else {
                if (soc_feature(unit, soc_feature_logical_port_num) &&
                    SOC_REG_INFO(unit, reg).block[0] == SOC_BLK_MMU) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    /*MMU register with device port indexing*/
                    num_regs = sizeof(reg_excep_list) / sizeof(soc_reg_t);
                    for (reg_index = 0;reg_index < num_regs; reg_index++) {
                        if (reg_excep_list[reg_index] == reg) {
                            add_exception = 1;
                            break;
                        }
                    }
                    if (add_exception) {
                        ainfo->port = pindex;
                    } else

#endif
                    {
                    phy_port = SOC_INFO(unit).port_m2p_mapping[pindex];
                    ainfo->port = SOC_INFO(unit).port_p2l_mapping[phy_port];
                    }
                } else {
                    ainfo->port = pindex;
                }
            }
            break;
        case soc_cosreg:
            if (pindex >= 0 && pindex <= NUM_COS(unit)) {
                ainfo->cos = pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_genreg:
            if (pindex != 0) {
                ainfo->reg = INVALIDr;
            }
            break;
        default:
            assert(0);
        }
        break;
    }
}

/*
 * Given an address, fill in an regaddrinfo structure
 * This can only be called for SOC registers, not for CPU registers
 */
void
soc_regaddrinfo_extended_get(int unit, soc_regaddrinfo_t *ainfo,
                             soc_block_t block, int acc_type, uint32 addr)
{
    int                 i;
    int                 blk, bindex, port, phy_port = 0, cmic;
    int                 actual_blk; /* For broadcast blocks this will be a broadcast member block, otherwise it will be the block itself */
    uint32              minadr, maxadr;
    soc_regtype_t       regtype;
    int                 reg;
    uint32              offset, offset_ori;     /* base address */
    int                 pindex;         /* port/cos index field */
    int                 aindex;         /* array index field */
    soc_block_t         portblktype;
    soc_block_types_t   regblktype;
    int                 port_num_blktype;
#ifdef BCM_SAND_SUPPORT
    int                 use_orig_address = 0;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_reg_t reg_excep_list[]={MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                                MMU_RQE_REPL_PORT_AGG_MAPr};
    int reg_index, num_regs, add_exception = 0;
    int               stage_id;
#endif


    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        soc_regaddrinfo_get(unit, ainfo, addr);
        return;
    }
    ainfo->addr = addr;
    ainfo->valid = 1;
    ainfo->reg = INVALIDr;
    portblktype = SOC_BLK_PORT;
    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
        SOC_DRIVER(unit)->port_num_blktype : 1;
    pindex = addr & 0xff;

    /* find the matching block */
    /* we find the last matching block to make things work with cmic/cpic */
    blk = -1;
    for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
        cmic = SOC_BLOCK_INFO(unit, i).cmic;
        if (cmic == block) {
            blk = i;
            /* break; */
        }
    }

    if (SOC_BLOCK_INFO(unit, blk).type != SOC_BLK_MMU_GLB &&
        SOC_BLOCK_INFO(unit, blk).type != SOC_BLK_MMU_SC &&
        SOC_BLOCK_INFO(unit, blk).type != SOC_BLK_MMU_SED &&
        SOC_BLOCK_INFO(unit, blk).type != SOC_BLK_MMU_XPE &&
        SOC_BLOCK_INFO(unit, blk).type != SOC_BLK_MMU_ITM &&
        SOC_BLOCK_INFO(unit, blk).type != SOC_BLK_MMU_EB &&
        addr & 0x02000000) {
        offset = addr;
    } else {
        if (SOC_IS_GREYHOUND2(unit) &&
            (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_TAF)) {
            offset = addr;
        } else {
            offset = addr & 0xffffff00;
        }
    }

#if defined(BCM_SAND_SUPPORT)
    if (SOC_IS_SAND(unit)) {
        use_orig_address = 1; /* The register address of a soc_genreg does not end with 0x00 in DPP */
    }
#endif

    actual_blk = blk;
#ifdef BCM_JERICHO_SUPPORT
    /* handle broadcast blocks */
    if (SOC_IS_JERICHO(unit) && SOC_BLOCK_IS_BROADCAST(unit, blk)) {
        actual_blk = SOC_BLOCK_BROADCAST_MEMBER(unit, blk, 0);
    }
#endif /* BCM_JERICHO_SUPPORT */

    assert(actual_blk >= 0);
    offset_ori = offset;
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        uint32 reg_offset; /* base address according to the checked register candidate */
        if (SOC_IS_GREYHOUND2(unit)) {
            offset = offset_ori;
        }
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        regtype = SOC_REG_INFO(unit, reg).regtype;
        if (soc_is_reg_flexe_core(unit, reg))
        {
            regtype = soc_genreg;
        }
        regblktype = SOC_REG_INFO(unit, reg).block;
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
           /* Since TH3 has the dummy blocks CDMAC defined in the regsfile, we need
              to assign them to the CDPORT block */
            if (regblktype[0] == SOC_BLK_CDMAC) {
                regblktype[0] = SOC_BLK_CDPORT;
            }
        }
#endif
        /* Base address decision of GH2 TAF block */
        if (SOC_IS_GREYHOUND2(unit) &&
            (SOC_REG_INFO(unit, reg).regtype == soc_genreg) &&
            (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_TAF)) {
            offset |= (1 << SOC_RT_BP);
            pindex = 0;
        }

#ifdef BCM_SAND_SUPPORT
        reg_offset = (use_orig_address && regtype == soc_genreg) ? addr : offset;
        if ((SOC_IS_DNX(unit) && !soc_feature(unit, soc_feature_single_instance_cdmac))
              && (SOC_BLOCK_TYPE(unit, actual_blk) == SOC_BLK_CDMAC || SOC_BLOCK_TYPE(unit, actual_blk) == SOC_BLK_CDPORT)) {
            reg_offset &= 0xf3ffff00;
        }
#else
        reg_offset = offset;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Since there are two CDMAC0 and CDMAC1, we need to determine which
           stage does this port belong to for the per port registers.
           For CDPORT registers, the stage is 0 and CDMAC_0 registers, it is 1
           while for CDMAC_1 it is 2 */
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (SOC_BLOCK_IS(regblktype, SOC_BLK_CDPORT)) {
                if ((reg_offset & 0x000F0000) == 0x10000) { /* CDMAC_OFFSET_CNT */
                    reg_offset &= ~(0x04000000 | 0x08000000);
                }
            }
        }
#endif

        if ((regtype == soc_cpureg) || (regtype == soc_mcsreg) || (regtype == soc_iprocreg))  {
            continue;
        }
        /* coverity[negative_returns:FALSE] */
        if (!SOC_BLOCK_IS_TYPE(unit, actual_blk, regblktype)) {
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) &&
              (((SOC_BLOCK_TYPE(unit, actual_blk) == SOC_BLK_CDPORT && *SOC_REG_INFO(unit, reg).block == SOC_BLK_CDMAC) ||
               (SOC_BLOCK_TYPE(unit, actual_blk) == SOC_BLK_CDMAC && *SOC_REG_INFO(unit, reg).block == SOC_BLK_CDPORT)) ||
               ((SOC_BLOCK_TYPE(unit, actual_blk) == SOC_BLK_CLPORT && *SOC_REG_INFO(unit, reg).block == SOC_BLK_CLMAC) ||
               (SOC_BLOCK_TYPE(unit, actual_blk) == SOC_BLK_CLMAC && *SOC_REG_INFO(unit, reg).block == SOC_BLK_CLPORT)))
               ) {
                /* In Jr2 CDPORT and CDMAC are the same sbus blocks */
            } else
#endif
            continue;
        }

        if ((acc_type != SOC_REG_ACC_TYPE(unit, reg)) && (acc_type >= 0)
                /* skip acc_type check when acc_type is -ve */
#ifdef BCM_SAND_SUPPORT
                && (!use_orig_address)
#endif
            ) {
            continue;
        }

        minadr = SOC_REG_INFO(unit, reg).offset;
        if (reg_offset < minadr) {
            continue;
        }

        if (SOC_REG_IS_ARRAY(unit, reg)) {
            maxadr = minadr + (SOC_REG_NUMELS(unit, reg) - 1) * SOC_REG_ELEM_SKIP(unit, reg);
            if ( reg_offset > maxadr || ((reg_offset-minadr) % SOC_REG_ELEM_SKIP(unit, reg))) {
                continue; /* exit if address out of array range, or is not an exact multiple of element skip */
            }
            ainfo->idx = (reg_offset-minadr) / SOC_REG_ELEM_SKIP(unit, reg);
        } else {
            /* handle other possible array types */
            if (SOC_REG_ARRAY(unit, reg)) {
                if (SOC_IS_GREYHOUND2(unit) &&
                    (SOC_REG_INFO(unit, reg).regtype == soc_genreg) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_TAF)) {
                    aindex = reg_offset - SOC_REG_BASE(unit, reg);
                } else {
                    aindex = (reg_offset - SOC_REG_BASE(unit, reg)) >> 8;
                }

                if (SOC_REG_ARRAY2(unit, reg)) {
                    ainfo->idx = aindex / (2 * SOC_REG_GRAN(unit, reg));
                    maxadr = minadr +
                        ((2 * (SOC_REG_GRAN(unit, reg) *
                         (SOC_REG_NUMELS(unit, reg) - 1))) << 8);
                } else if (SOC_REG_ARRAY4(unit, reg)) {
                    ainfo->idx = aindex / (4 * SOC_REG_GRAN(unit, reg));
                    maxadr = minadr +
                        ((4 * (SOC_REG_GRAN(unit, reg) *
                         (SOC_REG_NUMELS(unit, reg) - 1))) << 8);
                } else {
                    ainfo->idx = aindex / SOC_REG_GRAN(unit, reg);
                    if (SOC_IS_GREYHOUND2(unit) &&
                        (SOC_REG_INFO(unit, reg).regtype == soc_genreg) &&
                        (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_TAF)) {
                        maxadr = minadr +
                            ((SOC_REG_GRAN(unit, reg) *
                             (SOC_REG_NUMELS(unit, reg) - 1)));
                    } else {
                        maxadr = minadr +
                            (((SOC_REG_GRAN(unit, reg) *
                             (SOC_REG_NUMELS(unit, reg) - 1))) << 8);
                    }
                }
            } else {
                ainfo->idx = -1;
                maxadr = minadr;
            }
            if (reg_offset > maxadr) {
                continue;
            }
            if (SOC_REG_ARRAY2(unit, reg)) {
                if ( (reg_offset & 0x100) != (minadr & 0x100) ) {
                    /* make sure last bit are same as the base addr when array stride is 2 */
                    continue;
                }
            }
        }

        ainfo->block = blk;
        ainfo->acc_type = acc_type;
        ainfo->reg = reg;
        ainfo->field = INVALIDf;
        ainfo->cos = -1;
        ainfo->port = REG_PORT_ANY;
        switch (regtype) {
        case soc_customreg:
        case soc_portreg:
        case soc_ppportreg:
            if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype) &&
                    !(SOC_BLOCK_IS(regblktype, SOC_BLK_CDPORT))) {
                for (phy_port = 0; ; phy_port++) {
                    if (soc_feature(unit, soc_feature_logical_port_num)) {
                        port = SOC_INFO(unit).port_p2l_mapping[phy_port];
                        if (port < 0) {
                            continue;
                        }
                    } else {
                        port = phy_port;
                    }
#ifdef BCM_KATANA2_SUPPORT
                    /* Override port blocks with Linkphy Blocks.. */
                    if(SOC_IS_KATANA2(unit) &&
                        (regblktype[0] == SOC_BLK_TXLP) ) {
                        if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                            SOC_BLK_TXLP, &blk, &bindex) != SOC_E_NONE) {
                            continue;
                        }
                    } else if(SOC_IS_KATANA2(unit) &&
                    (regblktype[0] == SOC_BLK_RXLP) ) {
                        if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                            SOC_BLK_RXLP, &blk, &bindex) != SOC_E_NONE) {
                            continue;
                        }

                    } else
#endif
#ifdef BCM_METROLITE_SUPPORT
                    /* Override port blocks with Linkphy Blocks.. */
                    if (SOC_IS_METROLITE(unit) &&
                        (regblktype[0] == SOC_BLK_IECELL)) {
                        if (soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                            SOC_BLK_IECELL, &blk, &bindex) != SOC_E_NONE) {
                            continue;
                        }
                    } else
#endif
#if defined (BCM_SABER2_SUPPORT)
                    /* Override port blocks with Linkphy Blocks.. */
                    if (SOC_IS_SABER2(unit) &&
                        (regblktype[0] == SOC_BLK_IECELL)) {
                        if (soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                            SOC_BLK_IECELL, &blk, &bindex) != SOC_E_NONE) {
                            continue;
                        }
                    } else
#endif
#if defined (BCM_FIRELIGHT_SUPPORT)
                    /* Override port blocks with Linkphy Blocks.. */
                    if (SOC_IS_FIRELIGHT(unit) &&
                        (regblktype[0] == SOC_BLK_MACSEC)) {
                        if (soc_fl_macsec_port_reg_blk_idx_get(unit, phy_port,
                            SOC_BLK_MACSEC, &blk, &bindex) != SOC_E_NONE) {
                            continue;
                        }
                    } else
#endif
                    {
                        blk = SOC_PORT_BLOCK(unit, phy_port);
                        bindex = SOC_PORT_BINDEX(unit, phy_port);
                    }
                    if (blk < 0 && bindex < 0) {        /* end of list */
                        break;
                    }
                    if (blk < 0) {                      /* empty slot */
                        continue;
                    }
                    
                    for (i = 0; i < port_num_blktype ; i++) {
#ifdef BCM_KATANA2_SUPPORT
                        /* Override port blocks with Linkphy Blocks.. */
                        if(SOC_IS_KATANA2(unit) &&
                            (regblktype[0] == SOC_BLK_TXLP)){
                            if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                                SOC_BLK_TXLP, &blk, &bindex) != SOC_E_NONE) {
                                continue;
                            }
                        } else if(SOC_IS_KATANA2(unit) &&
                        (regblktype[0] == SOC_BLK_RXLP) ) {
                            if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                                SOC_BLK_RXLP, &blk, &bindex) != SOC_E_NONE) {
                                continue;
                            }
                        } else
#endif
#ifdef BCM_METROLITE_SUPPORT
                        if ((SOC_IS_METROLITE(unit)) &&
                            (regblktype[0] == SOC_BLK_IECELL) ) {
                            if (soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                                SOC_BLK_IECELL, &blk, &bindex) != SOC_E_NONE) {
                                continue;
                            }
                        } else
#endif
#if defined (BCM_SABER2_SUPPORT)
                        if ((SOC_IS_SABER2(unit)) &&
                            (regblktype[0] == SOC_BLK_IECELL) ) {
                            if (soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                                SOC_BLK_IECELL, &blk, &bindex) != SOC_E_NONE) {
                                continue;
                            }
                        } else
#endif
#if defined (BCM_FIRELIGHT_SUPPORT)
                        if (SOC_IS_FIRELIGHT(unit) &&
                            (regblktype[0] == SOC_BLK_MACSEC)) {
                            if (soc_fl_macsec_port_reg_blk_idx_get(unit, phy_port,
                                SOC_BLK_MACSEC, &blk, &bindex) != SOC_E_NONE) {
                                continue;
                            }
                        } else
#endif
#if defined (BCM_TRIDENT3_SUPPORT)
                        if ((SOC_IS_TRIDENT3X(unit)) &&
                            ((SOC_REG_INFO(unit, reg).block[0] == SOC_BLK_XLPORT) &&
                            (pindex == 2) && (phy_port == 128))) {
                            blk = SOC_PORT_IDX_BLOCK(unit, 129, i);
                            bindex = 2;
                        } else
#endif
                        {
                            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                        }
                        if ((uint32)blk != ainfo->block) {
                            continue;
                        }
                        if (soc_feature(unit,
                                        soc_feature_pgw_mac_rsv_mask_remap) &&
                            reg == PGW_MAC_RSV_MASKr) {
                            bindex = (phy_port - 1) & 0xf;
                        }
                        if (bindex == pindex) {
                            ainfo->port = port;
                            break;
                        }
                    }
                    if (i == port_num_blktype) {
                        continue;
                    }
                    if (blk >= 0) {
                        break;
                    }
                }
                if (ainfo->port == REG_PORT_ANY) {
                    ainfo->reg = INVALIDr;
                }
            } else {
                if (soc_feature(unit, soc_feature_logical_port_num) &&
                    (regblktype[0] == SOC_BLK_MMU ||
                     regblktype[0] == SOC_BLK_MMU_GLB ||
                     regblktype[0] == SOC_BLK_MMU_XPE ||
                     regblktype[0] == SOC_BLK_MMU_SED ||
                     regblktype[0] == SOC_BLK_MMU_ITM ||
                     regblktype[0] == SOC_BLK_MMU_EB ||
                     regblktype[0] == SOC_BLK_MMU_SC)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    /*MMU register with device port indexing*/
                    num_regs = sizeof(reg_excep_list) / sizeof(soc_reg_t);
                    for (reg_index = 0;reg_index < num_regs; reg_index++) {
                        if (reg_excep_list[reg_index] == reg) {
                            add_exception = 1;
                            break;
                        }
                    }
                    if (add_exception) {
                        ainfo->port = pindex;
                    } else

#endif
                    {
                    phy_port = SOC_INFO(unit).port_m2p_mapping[pindex];
                    ainfo->port = SOC_INFO(unit).port_p2l_mapping[phy_port];
                    }
                } else {
                    ainfo->port = pindex;
                }
#ifdef BCM_TOMAHAWK3_SUPPORT
                /* Since there are two CDMAC0 and CDMAC1, we need to determine which
                * CDMAC/stage id does this register belong to.
                * For CDMAC_0 registers, it is 1, while for CDMAC_1 it is 2 */
                if (SOC_IS_TOMAHAWK3(unit)) {
                   phy_port = SOC_INFO(unit).port_m2p_mapping[pindex];
                   if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CDPORT)) {
                       if ((SOC_REG_INFO(unit, reg).offset & 0x000F0000) ==
                                       CDMAC_OFFSET_CNT) {
                           stage_id = ((((phy_port - 1) >> 2) & 1) == 0) ?
                                             CDMAC0_STAGE_ID : CDMAC1_STAGE_ID;
                           if (stage_id == CDMAC0_STAGE_ID) {
                               pindex = pindex | 0x04000000;
                               break;
                           } else {
                               pindex = pindex | 0x08000000;
                               break;
                           }
                       }
                   }
                   ainfo->port = pindex;
                }
#endif
            }
            break;
        case soc_cosreg:
            if (pindex >= 0 && pindex <= NUM_COS(unit)) {
                ainfo->cos = pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_pipereg:
            if (pindex >= 0 && pindex < NUM_PIPE(unit)) {
                ainfo->port = SOC_REG_ADDR_INSTANCE_MASK | pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_xpereg:
            if (pindex >= 0 && pindex < NUM_XPE(unit)) {
                ainfo->port = SOC_REG_ADDR_INSTANCE_MASK | pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_itmreg:
            if (pindex >= 0 && pindex < NUM_ITM(unit)) {
                ainfo->port = SOC_REG_ADDR_INSTANCE_MASK | pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_ebreg:
            if (pindex >= 0 && pindex < NUM_EB(unit)) {
                ainfo->port = SOC_REG_ADDR_INSTANCE_MASK | pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_slicereg:
            if (pindex >= 0 && pindex < NUM_SLICE(unit)) {
                ainfo->port = SOC_REG_ADDR_INSTANCE_MASK | pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_layerreg:
            if (pindex >= 0 && pindex < NUM_LAYER(unit)) {
                ainfo->port = SOC_REG_ADDR_INSTANCE_MASK | pindex;
            } else {
                ainfo->reg = INVALIDr;
            }
            break;
        case soc_genreg:
            if (pindex != 0) {
#ifdef BCM_SAND_SUPPORT
                if (!use_orig_address)
#endif
                    ainfo->reg = INVALIDr;
            }
            break;
        default:
            assert(0);
        }
        if (ainfo->idx != -1) {
            continue;
        }
        break;
    }
}

/* port_types supported: -1(all), ETH_PORT, STK_PORT, INTLB_PORT */
int
soc_pipe_port_get(int unit, uint32 port_type, int pipe, soc_pbmp_t *pbmp)
{
    if (!soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        return SOC_E_UNAVAIL;
    }
    if (pipe > NUM_PIPE(unit)) {
        return SOC_E_PARAM;
    }
    SOC_PBMP_ASSIGN(*pbmp, PBMP_PIPE(unit, pipe));
    SOC_PBMP_REMOVE(*pbmp, SOC_PORT_DISABLED_BITMAP(unit, all));
    if (0xffffffff == port_type) {
        return SOC_E_NONE;
    } else {
        if (!(port_type & ETH_PORT)) {
            SOC_PBMP_REMOVE(*pbmp,  PBMP_E_ALL(unit));
        }
        if (!(port_type & STK_PORT)) {
            SOC_PBMP_REMOVE(*pbmp,  PBMP_ST_ALL(unit));
        }
        if (!(port_type & INTLB_PORT)) {
            SOC_PBMP_REMOVE(*pbmp,  PBMP_LB_ALL(unit));
        }
    }
    return SOC_E_NONE;
}

int
soc_port_pipe_get(int unit, int port, int *pipe)
{
    if (!soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        return SOC_E_UNAVAIL;
    }
    if (port > MAX_PORT(unit)) {
        return SOC_E_PARAM;
    }
    *pipe = SOC_INFO(unit).port_pipe[port];
    return SOC_E_NONE;
}

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)*/
