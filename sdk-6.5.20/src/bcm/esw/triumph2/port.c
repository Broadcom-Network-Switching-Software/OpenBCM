/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH2_SUPPORT)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/trident2.h>

STATIC SHR_BITDCL  *vpd_bitmap[BCM_MAX_NUM_UNITS];
#define _VLAN_PROTOCOL_DATA_PROF_USED_GET(_u_, _vpd_) \
        SHR_BITGET(vpd_bitmap[_u_], (_vpd_))
#define _VLAN_PROTOCOL_DATA_PROF_USED_SET(_u_, _vpd_) \
        SHR_BITSET(vpd_bitmap[_u_], (_vpd_))
#define _VLAN_PROTOCOL_DATA_PROF_USED_CLR(_u_, _vpd_) \
        SHR_BITCLR(vpd_bitmap[_u_], (_vpd_))

int 
_bcm_tr2_port_vpd_bitmap_alloc(int unit) 
{
    int num_profiles;
    if (vpd_bitmap[unit]) {
        sal_free(vpd_bitmap[unit]);
        vpd_bitmap[unit] = NULL;
    }
    num_profiles = soc_mem_index_count(unit, VLAN_PROTOCOL_DATAm) / 
                   soc_mem_index_count(unit, VLAN_PROTOCOLm);
    vpd_bitmap[unit] = sal_alloc(SHR_BITALLOCSIZE(num_profiles), 
                                 "vpd_bitmap");
    if (vpd_bitmap[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(vpd_bitmap[unit], 0, SHR_BITALLOCSIZE(num_profiles));
    return BCM_E_NONE;
}

void
_bcm_tr2_port_vpd_bitmap_free(int unit)
{
    if (vpd_bitmap[unit]) {
        sal_free(vpd_bitmap[unit]);
    }
    vpd_bitmap[unit] = NULL;
    return;
}

int
_bcm_port_vlan_prot_index_alloc(int unit, int *prof_ptr)
{
    int i, num_profiles;
    num_profiles = soc_mem_index_count(unit, VLAN_PROTOCOL_DATAm) / 
                   soc_mem_index_count(unit, VLAN_PROTOCOLm);
    for (i = 0; i < num_profiles; i++) {
        if (!_VLAN_PROTOCOL_DATA_PROF_USED_GET(unit, i)) {
            _VLAN_PROTOCOL_DATA_PROF_USED_SET(unit, i);
            *prof_ptr = i * soc_mem_index_count(unit, VLAN_PROTOCOLm);
            break;
        }
    }
    if (i == num_profiles) {
#ifdef BCM_KATANA2_SUPPORT
        
        if (SOC_IS_KATANA2(unit)) {
            *prof_ptr = i * soc_mem_index_count(unit, VLAN_PROTOCOLm);
            return BCM_E_NONE;
        }
#endif
        return BCM_E_RESOURCE;
    } else {
        return BCM_E_NONE;
    }
} 

/*
 * Function:
 *      _bcm_port_vlan_prot_index_free
 * Purpose:
 *      Free given VLAN protocol profile index.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      prof_ptr   - (IN) VLAN Protocol Profile index.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_port_vlan_prot_index_free(int unit, int prof_ptr)
{
    int index;
    int num_profiles;

    num_profiles = soc_mem_index_count(unit, VLAN_PROTOCOL_DATAm) /
                   soc_mem_index_count(unit, VLAN_PROTOCOLm);

    index = prof_ptr / soc_mem_index_count(unit, VLAN_PROTOCOLm);

    if ((index < 0) || (index >= num_profiles)) {
            return BCM_E_INTERNAL;
    }

    _VLAN_PROTOCOL_DATA_PROF_USED_CLR(unit, index);

    return BCM_E_NONE;
}

int
_bcm_tr2_port_tab_get(int unit, bcm_port_t port,
                      soc_field_t field, int *value)

{
    port_tab_entry_t pent;
    soc_mem_t mem;
    int rv = BCM_E_NONE;

    mem = SOC_PORT_MEM_TAB(unit, port);
    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }
    sal_memset(&pent, 0, sizeof(port_tab_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
    SOC_PORT_MOD_OFFSET(unit, port), &pent);

    if (BCM_SUCCESS(rv)) {
        *value = soc_PORT_TABm_field32_get(unit, &pent, field);
    }
    return rv;
}


/* Port table field programming - assumes PORT_LOCK is taken */
int
_bcm_tr2_port_tab_set(int unit, bcm_port_t port, 
                      soc_field_t field, int value)

{
    port_tab_entry_t pent;
    soc_mem_t mem;
    int rv, cur_val;

    mem = SOC_PORT_MEM_TAB(unit, port);
    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }
    sal_memset(&pent, 0, sizeof(port_tab_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);

    if (BCM_SUCCESS(rv)) {
        cur_val = soc_PORT_TABm_field32_get(unit, &pent, field);
        if (value != cur_val) {
            soc_PORT_TABm_field32_set(unit, &pent, field, value);
            rv = soc_mem_write(unit, mem,
                               MEM_BLOCK_ALL, port, &pent);
        }
    }
    return rv;
}

/* Helper functions for bcmPortControlLanes 
 * Used for flex-port support
 */
int 
_bcm_tr2_port_lanes_get(int unit, bcm_port_t port, int *value)
{
    int rv = BCM_E_NONE;
    uint32 rval, bitpos = 0;
    /*    coverity[new_values]    */

    if (!_bcm_esw_valid_flex_port_controlling_port(unit, port)) {
        return (BCM_E_PORT);
    }

    switch (port) {
    case 30:
        bitpos = 6;
        break;
    case 34:
        bitpos = 7;
        break;
    case 38:
        bitpos = 8;
        break;
    case 42:
        bitpos = 9;
        break;
    case 46:
        bitpos = 10;
        break;
    case 50:
        bitpos = 11;
        break;
    
    /* Defensive Default */
    /* coverity[dead_error_begin] */
    default:
        return (BCM_E_PORT);
    }

    BCM_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &rval));

    if (rval & (1 << bitpos)) {
        *value = 4;
    } else {
        *value = 1;
    }
    return rv;
}

static const uint16 _soc_triumph2_int_phy_addr[] = {
    0x00, /* Port  0 (cmic)         N/A */
    0x99, /* Port  1 (9SERDES)      IntBus=0 Addr=0x19 */
    0x81, /* Port  2 (8SERDES_0)    IntBus=0 Addr=0x01 */
    0x82, /* Port  3 (8SERDES_0)    IntBus=0 Addr=0x02 */
    0x83, /* Port  4 (8SERDES_0)    IntBus=0 Addr=0x03 */
    0x84, /* Port  5 (8SERDES_0)    IntBus=0 Addr=0x04 */
    0x85, /* Port  6 (8SERDES_0)    IntBus=0 Addr=0x05 */
    0x86, /* Port  7 (8SERDES_0)    IntBus=0 Addr=0x06 */
    0x87, /* Port  8 (8SERDES_0)    IntBus=0 Addr=0x07 */
    0x88, /* Port  9 (8SERDES_0)    IntBus=0 Addr=0x08 */
    0x89, /* Port 10 (8SERDES_1)    IntBus=0 Addr=0x09 */
    0x8a, /* Port 11 (8SERDES_1)    IntBus=0 Addr=0x0a */
    0x8b, /* Port 12 (8SERDES_1)    IntBus=0 Addr=0x0b */
    0x8c, /* Port 13 (8SERDES_1)    IntBus=0 Addr=0x0c */
    0x8d, /* Port 14 (8SERDES_1)    IntBus=0 Addr=0x0d */
    0x8e, /* Port 15 (8SERDES_1)    IntBus=0 Addr=0x0e */
    0x8f, /* Port 16 (8SERDES_1)    IntBus=0 Addr=0x0f */
    0x90, /* Port 17 (8SERDES_1)    IntBus=0 Addr=0x10 */
    0x91, /* Port 18 (9SERDES)      IntBus=0 Addr=0x11 */
    0x92, /* Port 19 (9SERDES)      IntBus=0 Addr=0x12 */
    0x93, /* Port 20 (9SERDES)      IntBus=0 Addr=0x13 */
    0x94, /* Port 21 (9SERDES)      IntBus=0 Addr=0x14 */
    0x95, /* Port 22 (9SERDES)      IntBus=0 Addr=0x15 */
    0x96, /* Port 23 (9SERDES)      IntBus=0 Addr=0x16 */
    0x97, /* Port 24 (9SERDES)      IntBus=0 Addr=0x17 */
    0x98, /* Port 25 (9SERDES)      IntBus=0 Addr=0x18 */
    0xd9, /* Port 26 (HC0)          IntBus=2 Addr=0x19 */
    0xda, /* Port 27 (HC1)          IntBus=2 Addr=0x1a */
    0xdb, /* Port 28 (HC2)          IntBus=2 Addr=0x1b */
    0xdc, /* Port 29 (HC3)          IntBus=2 Addr=0x1c */
    0xc1, /* Port 30 (HL0)          IntBus=2 Addr=0x01 */
    0xc2, /* Port 31 (HL0)          IntBus=2 Addr=0x02 */
    0xc3, /* Port 32 (HL0)          IntBus=2 Addr=0x03 */
    0xc4, /* Port 33 (HL0)          IntBus=2 Addr=0x04 */
    0xc5, /* Port 34 (HL1)          IntBus=2 Addr=0x05 */
    0xc6, /* Port 35 (HL1)          IntBus=2 Addr=0x06 */
    0xc7, /* Port 36 (HL1)          IntBus=2 Addr=0x07 */
    0xc8, /* Port 37 (HL1)          IntBus=2 Addr=0x08 */
    0xc9, /* Port 38 (HL2)          IntBus=2 Addr=0x09 */
    0xca, /* Port 39 (HL2)          IntBus=2 Addr=0x0a */
    0xcb, /* Port 40 (HL2)          IntBus=2 Addr=0x0b */
    0xcc, /* Port 41 (HL2)          IntBus=2 Addr=0x0c */
    0xcd, /* Port 42 (HL3)          IntBus=2 Addr=0x0d */
    0xce, /* Port 43 (HL3)          IntBus=2 Addr=0x0e */
    0xcf, /* Port 44 (HL3)          IntBus=2 Addr=0x0f */
    0xd0, /* Port 45 (HL3)          IntBus=2 Addr=0x10 */
    0xd1, /* Port 46 (HL4)          IntBus=2 Addr=0x11 */
    0xd2, /* Port 47 (HL4)          IntBus=2 Addr=0x12 */
    0xd3, /* Port 48 (HL4)          IntBus=2 Addr=0x13 */
    0xd4, /* Port 49 (HL4)          IntBus=2 Addr=0x14 */
    0xd5, /* Port 50 (HL5)          IntBus=2 Addr=0x15 */
    0xd6, /* Port 51 (HL5)          IntBus=2 Addr=0x16 */
    0xd7, /* Port 52 (HL5)          IntBus=2 Addr=0x17 */
    0xd8, /* Port 53 (HL5)          IntBus=2 Addr=0x18 */
};

int 
_bcm_tr2_port_lanes_set(int unit, bcm_port_t port, int value)
{
    uint16      dev_id;
    uint8       rev_id;
    int i, okay, old_value, count, rv = BCM_E_NONE;
    uint32 rval, val2, to_usec, mode, flags, bitpos = 0;
    uint64 rval64;
    soc_info_t *si;
    soc_field_t xq_rst2 = INVALIDf;
    uint16 phy0, phy1, phy2, phy3;
    bcm_port_t it_port;
    iarb_tdm_table_entry_t iarb_tdm;
    arb_tdm_table_entry_t arb_tdm;
    mac_driver_t *macd;

    si = &SOC_INFO(unit);

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    if (!_bcm_esw_valid_flex_port_controlling_port(unit, port)) {
        return BCM_E_PORT;
    }

    if ((value != 1) && (value != 4)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_tr2_port_lanes_get(unit, port, &old_value));

#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

    /* Step 1: Change the SOC bitmaps */
    if ((value == 1) && (old_value == 4)) {
        /* The block was originally a single high-speed port */
        SOC_CONTROL_LOCK(unit);
        if (IS_HG_PORT(unit, port)) {
            SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
            SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
            SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
        } else {
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        }
        SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
        si->port_speed_max[port] = 1000;
        soc_port_cmap_set(unit, port, SOC_CTR_TYPE_GE);
        for (i = port + 1; i < port + 4; i++) {
            SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, i);
            SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, i);
            SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, i);
            SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, i);
            si->port_speed_max[i] = 1000;
            soc_port_cmap_set(unit, i, SOC_CTR_TYPE_GE);
        }
        RECONFIGURE_PORT_TYPE_INFO(ether);
        RECONFIGURE_PORT_TYPE_INFO(st);
        RECONFIGURE_PORT_TYPE_INFO(hg);
        RECONFIGURE_PORT_TYPE_INFO(xe);
        RECONFIGURE_PORT_TYPE_INFO(ge);
        SOC_CONTROL_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(_bcm_tr2_port_tab_set(unit, port, PORT_TYPEf, 0));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_64r, port, PORT_TYPEf, 0));
    } else if ((value == 4) && (old_value == 1)) {
        /* The block originally had 4 GE ports */
        SOC_CONTROL_LOCK(unit);
        SOC_PBMP_PORT_ADD(si->st.bitmap, port);
        SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
	switch (dev_id) {
	case BCM56526_DEVICE_ID:
	  si->port_speed_max[port] = 13000;
	  break;
	case BCM56636_DEVICE_ID:
	case BCM56638_DEVICE_ID:
	  si->port_speed_max[port] = 12000;
	  break;
	default:
	  si->port_speed_max[port] = 10000;
	}
        soc_port_cmap_set(unit, port, SOC_CTR_TYPE_XE);
        for (i = port + 1; i < port + 4; i++) {
            SOC_PBMP_PORT_ADD(si->ge.disabled_bitmap, i);
            SOC_PBMP_PORT_ADD(si->ether.disabled_bitmap, i);
            SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, i);
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, i);
        }
        RECONFIGURE_PORT_TYPE_INFO(ether);
        RECONFIGURE_PORT_TYPE_INFO(st);
        RECONFIGURE_PORT_TYPE_INFO(hg);
        RECONFIGURE_PORT_TYPE_INFO(ge);
        SOC_CONTROL_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(_bcm_tr2_port_tab_set(unit, port, PORT_TYPEf, 1));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_64r, port, PORT_TYPEf, 1));
    }
#undef RECONFIGURE_PORT_TYPE_INFO
    soc_dport_map_update(unit);

    /* Step 2: Perform necessary block-level reset and initialization */
    /* Egress disable */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port + 1, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port + 2, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port + 3, rval));

    /* Hold the XQPORT hotswap reset */
    switch (port) {
    case 30:
        bitpos = 6;
        xq_rst2 = XQ0_HOTSWAP_RST_Lf;
        break;
    case 34:
        bitpos = 7;
        xq_rst2 = XQ1_HOTSWAP_RST_Lf;
        break;
    case 38:
        bitpos = 8;
        xq_rst2 = XQ2_HOTSWAP_RST_Lf;
        break;
    case 42:
        bitpos = 9;
        xq_rst2 = XQ3_HOTSWAP_RST_Lf;
        break;
    case 46:
        bitpos = 10;
        xq_rst2 = XQ4_HOTSWAP_RST_Lf;
        break;
    case 50:
        bitpos = 11;
        xq_rst2 = XQ5_HOTSWAP_RST_Lf;
        break;
    default:
        break;
    }
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REG_2r(unit, &val2));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2, 
                      xq_rst2, 0);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REG_2r(unit, val2));
    sal_usleep(to_usec);

    /* Program the ingress and MMU TDMs */
    count = -1;
    for (i = 0; i < soc_mem_index_count(unit, ARB_TDM_TABLEm); i++) {
        BCM_IF_ERROR_RETURN
            (READ_ARB_TDM_TABLEm(unit, MEM_BLOCK_ANY, i, &arb_tdm));
        BCM_IF_ERROR_RETURN
            (READ_IARB_TDM_TABLEm(unit, MEM_BLOCK_ANY, i, &iarb_tdm));
        it_port = soc_ARB_TDM_TABLEm_field32_get(unit, &arb_tdm, PORT_NUMf);
        /* First set to disabled - 63 */
        if (old_value == value) {
            continue;
        } else if (old_value == 4 && value == 1) {
            if (port != it_port) {
                continue;
            }
        } else {
            if ((it_port != port) && (it_port != port + 1) && 
                (it_port != port + 2) && (it_port != port + 3)) {
                continue;
            }
        }
        soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, 63);
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 63);
        BCM_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, MEM_BLOCK_ALL, i, 
                                                  &iarb_tdm));
        BCM_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, MEM_BLOCK_ALL, i, 
                                                 &arb_tdm));
        /* Then program the desired value */
        if (old_value == 4 && value == 1) {
            count++;
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, 
                                           port + (count % 4));
            soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 
                                            port + (count % 4));
        } else {
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, port);
            soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, port);
        }
        BCM_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, MEM_BLOCK_ALL, i, 
                                                  &iarb_tdm));
        BCM_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, MEM_BLOCK_ALL, i, 
                                                 &arb_tdm));
        if (soc_ARB_TDM_TABLEm_field32_get(unit, &arb_tdm, WRAP_ENf)) {
            break;
        }
    }

    /* Clear the ECRC register */
    for (i = port; i < port + 4; i++) {
        BCM_IF_ERROR_RETURN(WRITE_TOQ_EP_CREDITr(unit, i, 0));
    }

    /* Reset the EP */
    BCM_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &rval));
    rval |= (1 << (bitpos - 6));
    BCM_IF_ERROR_RETURN(WRITE_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, rval));
    sal_usleep(to_usec);
    BCM_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &rval));
    if (value == 4) {
        rval |= (1 << bitpos);
    } else {
        rval &= ~(1 << bitpos);
    }  
    /* coverity[BAD_SHIFT: FALSE] */
    rval &= ~(1 << (bitpos - 6));
    BCM_IF_ERROR_RETURN(WRITE_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, rval));
    sal_usleep(to_usec);

    /* Bring the XQPORT block out of reset */
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REG_2r(unit, &val2));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REG_2r, &val2, 
                      xq_rst2, 1);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REG_2r(unit, val2));
    sal_usleep(to_usec);

    /* Bring the hyperlite out of reset */
    soc_xgxs_reset(unit, port);
    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XQPORT_XGXS_NEWCTL_REGr, 
                                               port, TXD1G_FIFO_RSTBf, 0xf));

    /* Change the XQPORT block mode */
    mode = (value == 4) ? 2 : 1; /* 2 = high-speed, 1 = GE */
    SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
    soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, 
                      XQPORT_MODE_BITSf, mode);
    SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, port, rval));
    if (value == 4) {
        rval = 0;
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf, 1);
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
    }

    /* Egress Enable */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));

    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 
                      (value == 1) ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port + 1, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port + 2, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port + 3, rval));

    /* Step 3: Port probe and initialization */
    flags = 0;
    if (value == 1) {
        phy0 = soc_property_port_get(unit, port, spn_FLEX_PORT_PHY_ADDR, 0);
        if (!phy0) {
            phy0 = _soc_triumph2_int_phy_addr[port];
            flags = _SHR_PORT_PHY_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port));
        SOC_IF_ERROR_RETURN(soc_phy_cfg_addr_set(unit, port, flags, phy0));
        BCM_IF_ERROR_RETURN(_bcm_port_probe(unit, port, &okay));
        if (!okay) {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port, TRUE));
        phy1 = soc_property_port_get(unit, port + 1, spn_FLEX_PORT_PHY_ADDR, 0);
        if (!phy1) {
            phy1 = _soc_triumph2_int_phy_addr[port + 1];
            flags = _SHR_PORT_PHY_INTERNAL;
        }  
        SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port + 1));
        SOC_IF_ERROR_RETURN(soc_phy_cfg_addr_set(unit, port + 1, flags, phy1));
        BCM_IF_ERROR_RETURN(_bcm_port_probe(unit, port + 1, &okay));
        if (!okay) {
            return BCM_E_INTERNAL;
        }     
        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port + 1, TRUE));
        phy2 = soc_property_port_get(unit, port + 2, spn_FLEX_PORT_PHY_ADDR, 0);
        if (!phy2) {
            phy2 = _soc_triumph2_int_phy_addr[port + 2];
            flags = _SHR_PORT_PHY_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port + 2));
        SOC_IF_ERROR_RETURN(soc_phy_cfg_addr_set(unit, port + 2, flags, phy2));
        BCM_IF_ERROR_RETURN(_bcm_port_probe(unit, port + 2, &okay));
        if (!okay) {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port + 2, TRUE));
        phy3 = soc_property_port_get(unit, port + 3, spn_FLEX_PORT_PHY_ADDR, 0);
        if (!phy3) {
            phy3 = _soc_triumph2_int_phy_addr[port + 3];
            flags = _SHR_PORT_PHY_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port + 3));
        SOC_IF_ERROR_RETURN(soc_phy_cfg_addr_set(unit, port + 3, flags, phy3));
        BCM_IF_ERROR_RETURN(_bcm_port_probe(unit, port + 3, &okay));
        if (!okay) {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port + 3, TRUE));
    } else {
        phy0 = soc_property_port_get(unit, port, spn_FLEX_PORT_PHY_ADDR, 0);
        if (!phy0) {
            phy0 = _soc_triumph2_int_phy_addr[port];
            flags = _SHR_PORT_PHY_INTERNAL;
        } 
        SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port));
        if (old_value == 1) {
            SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port + 1));
            SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port + 2));
            SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port + 3));
        }
        SOC_IF_ERROR_RETURN(soc_phy_cfg_addr_set(unit, port, flags, phy0));
        BCM_IF_ERROR_RETURN(_bcm_port_probe(unit, port, &okay));
        if (!okay) {
            return BCM_E_INTERNAL;
        }

        /* Need to reset the Bigmac to make registers accessible */
        SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));

        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &rval64));
        soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, TXRESETf, 1);
        soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, RXRESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, rval64));

        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &rval64));
        soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, TXRESETf, 0);
        soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, RXRESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, rval64));

        SOC_IF_ERROR_RETURN(MAC_INIT(macd, unit, port));

        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port, TRUE));
    }
    return rv;
}

int _bcm_tr2_ing_pri_cng_map_default_entry_add(int unit, 
                                               soc_profile_mem_t *prof)
{
    ing_pri_cng_map_entry_t pri_map[16];
    phb_mapping_tbl_1_entry_t phb_pri_map[16];
    ing_untagged_phb_entry_t phb;
    int index, pkt_pri, cfi, base = 0;
    void *entries[2];

    sal_memset(pri_map, 0, sizeof(ing_pri_cng_map_entry_t) * 16);
    sal_memset(&phb, 0, sizeof(phb));

    for (cfi = 0; cfi <= 1; cfi++) {
        for (pkt_pri = 0; pkt_pri <= 7; pkt_pri++) {
            index = (pkt_pri << 1) | cfi;
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &phb_pri_map[index], PRIf,
                                    pkt_pri);
                soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &phb_pri_map[index],
                                    CNGf, cfi);
            } else {
                soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map[index], PRIf,
                                    pkt_pri);
                soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map[index],
                                    CNGf, cfi);
            } 
        }
    }
    if (SOC_IS_TRIDENT3X(unit)) {
        entries[0] = &phb_pri_map;
    } else {
        entries[0] = &pri_map;
    }

    /** Revisit ---- No ING_UNTAGGED_PHB found in Trident3 */
    entries[1] = &phb;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, prof, entries, 16, (uint32 *)&base));

    if (SOC_IS_TRIDENT3X(unit)) {
        if (base != soc_mem_index_min(unit, PHB_MAPPING_TBL_1m)) {
            return BCM_E_INTERNAL;
        }
    } else {
        if (base != soc_mem_index_min(unit, ING_PRI_CNG_MAPm)) {
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_port_vlan_priority_map_get
 * Description:
 *      Get the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (OUT) Internal priority
 *      color        - (OUT) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
_bcm_tr2_port_vlan_priority_map_get(int unit, bcm_port_t port, int pkt_pri,
                                    int cfi, int *internal_pri, 
                                    bcm_color_t *color)
{
    port_tab_entry_t pent;
    ing_pri_cng_map_entry_t pri_cng_entry;
     phb_mapping_tbl_1_entry_t phb_pri_map;
    int rv; 
    int index;
    int hw_color;
    int ptr;

    sal_memset(&pent, 0, sizeof(port_tab_entry_t));
    sal_memset(&pri_cng_entry, 0, sizeof(ing_pri_cng_map_entry_t));

    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);
    BCM_IF_ERROR_RETURN(rv);

    ptr = soc_PORT_TABm_field32_get(unit, &pent, TRUST_DOT1P_PTRf);

    index = ((pkt_pri << 1) | cfi) + (ptr * _BCM_TR2_PRI_CNG_MAP_SIZE);

    if (SOC_IS_TRIDENT3X(unit)) {
        rv = soc_mem_read(unit, PHB_MAPPING_TBL_1m, MEM_BLOCK_ANY, 
                          index, &phb_pri_map);

        *internal_pri = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, 
                                            (uint32 *)&phb_pri_map, PRIf);
        hw_color = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, 
                                       (uint32 *)&phb_pri_map, CNGf);
    } else {
        rv = soc_mem_read(unit, ING_PRI_CNG_MAPm, MEM_BLOCK_ANY,
                          index, &pri_cng_entry);

        *internal_pri = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm,
                                            (uint32 *)&pri_cng_entry, PRIf);
        hw_color = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm,
                                       (uint32 *)&pri_cng_entry, CNGf);
    }

    *color = _BCM_COLOR_DECODING(unit, hw_color);
    return (rv);
}

/*
 * Function:
 *      _bcm_tr2_port_vlan_priority_map_set
 * Description:
 *      Define the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
_bcm_tr2_port_vlan_priority_map_set(int unit, bcm_port_t port, int pkt_pri,
                                   int cfi, int internal_pri, bcm_color_t color)
{
    ing_pri_cng_map_entry_t pri_cng_map[_BCM_TR2_PRI_CNG_MAP_SIZE];
    phb_mapping_tbl_1_entry_t phb_pri_cng_map[_BCM_TR2_PRI_CNG_MAP_SIZE];
    void *entries[1];
    port_tab_entry_t pent;
    int index;
    int rv;
    int new_ptr;
    int old_ptr;
    char *pri_map;
    int alloc_size;

    /* Allocate buffer for dma read. */
    alloc_size = _BCM_TR2_PRI_CNG_MAP_SIZE * sizeof (ing_pri_cng_map_entry_t);
    pri_map = soc_cm_salloc(unit, alloc_size, "TR2 pri cng map");
    if (NULL == pri_map) {
        return (BCM_E_MEMORY);
    }
    sal_memset(pri_map, 0, alloc_size);
    sal_memset(pri_cng_map, 0, alloc_size);

    /* Get man id from port table. */
    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, pri_map);
        return (rv);
    }
    old_ptr = soc_PORT_TABm_field32_get(unit, &pent, TRUST_DOT1P_PTRf);

    if (SOC_IS_TRIDENT3X(unit)) {
        /* Get the existing profile chunk */
        index = old_ptr * _BCM_TR2_PRI_CNG_MAP_SIZE;
        rv = soc_mem_read_range(unit, PHB_MAPPING_TBL_1m, MEM_BLOCK_ANY,
                                index, index + _BCM_TR2_PRI_CNG_MAP_SIZE - 1,
                                pri_map);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, pri_map);
            return (rv);
         }
        for (index = 0; index < _BCM_TR2_PRI_CNG_MAP_SIZE; index++) {
            entries[0] = soc_mem_table_idx_to_pointer(unit, PHB_MAPPING_TBL_1m,
                                                      void *, pri_map, index);
            sal_memcpy(phb_pri_cng_map + index, entries[0],
                       sizeof(phb_mapping_tbl_1_entry_t));
        }
    } else {
        /* Get the existing profile chunk */
        index = old_ptr * _BCM_TR2_PRI_CNG_MAP_SIZE;
        rv = soc_mem_read_range(unit, ING_PRI_CNG_MAPm, MEM_BLOCK_ANY,
                                index, index + _BCM_TR2_PRI_CNG_MAP_SIZE - 1,
                                pri_map);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, pri_map);
            return (rv);
         }
        for (index = 0; index < _BCM_TR2_PRI_CNG_MAP_SIZE; index++) {
            entries[0] = soc_mem_table_idx_to_pointer(unit, ING_PRI_CNG_MAPm,
                                                      void *, pri_map, index);
            sal_memcpy(pri_cng_map + index, entries[0],
                       sizeof(ing_pri_cng_map_entry_t));
        }

    }

    /* When invoked from bcm_port_cfi_color_set, color for a particular 
     * cfi and port needs to be programmed for all priorities. 
     * To identify this special case, 
     * both pkt_pri and internal_pri are set to -1.
     */
    if (pkt_pri < 0 && internal_pri < 0) {
        for (pkt_pri = 0; pkt_pri <= 7; pkt_pri++) {
            index = (pkt_pri << 1) | cfi;
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, (uint32 *)&phb_pri_cng_map[index],
                        PRIf, pkt_pri);
                soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, (uint32 *)&phb_pri_cng_map[index],
                        CNGf, _BCM_COLOR_ENCODING(unit, color));
            } else {
                soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, (uint32 *)&pri_cng_map[index],
                        PRIf, pkt_pri);
                soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, (uint32 *)&pri_cng_map[index],
                        CNGf, _BCM_COLOR_ENCODING(unit, color));
            }
        }
    } else {
        /* Modify what's needed */
        index = (pkt_pri << 1) | cfi;
        
        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, (uint32 *)&pri_cng_map[index],
                PRIf, internal_pri);
        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, (uint32 *)&pri_cng_map[index],
                CNGf, _BCM_COLOR_ENCODING(unit, color));
    }

    /* Delete the old profile chunk */
    if (old_ptr != 0) {
        index = _BCM_TR2_PRI_CNG_MAP_SIZE * old_ptr;
        rv = _bcm_ing_pri_cng_map_entry_delete(unit, index);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, pri_map);
            return (rv);
        }
    }

    /* Add and point to the new profile chunk */
    entries[0] = pri_cng_map;
    rv = _bcm_ing_pri_cng_map_entry_add(unit, entries,
                                        _BCM_TR2_PRI_CNG_MAP_SIZE, 
                                        (uint32 *)&new_ptr);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, pri_map);
        return (rv);
    }
    /*
     * COVERITY
     *
     * The variable new_ptr is passed by reference and will have a 
     * deterministic value if _bcm_ing_pri_cng_map_entry_add does
     * not return an error
     */ 
    /* coverity[uninit_use : FALSE] */
    new_ptr /= _BCM_TR2_PRI_CNG_MAP_SIZE;
    soc_PORT_TABm_field32_set(unit, &pent, TRUST_DOT1P_PTRf, new_ptr);
    rv = soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                       SOC_PORT_MOD_OFFSET(unit, port), &pent);
    soc_cm_sfree(unit, pri_map);
    return rv;
}


static soc_field_t _bcm_xgs3_priority_fields[] = {
    PRIORITY0_CNGf,
    PRIORITY1_CNGf,
    PRIORITY2_CNGf,
    PRIORITY3_CNGf,
    PRIORITY4_CNGf,
    PRIORITY5_CNGf,
    PRIORITY6_CNGf,
    PRIORITY7_CNGf,
};

/*
 * Function:
 *      _bcm_tr2_port_priority_color_set
 * Purpose:
 *      Specify the color selection for the given priority.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      prio -  priority (aka 802.1p CoS)
 *      color - color assigned to packets with indicated priority.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
_bcm_tr2_port_priority_color_set(int unit, bcm_port_t port,
                                 int prio, bcm_color_t color)
{
    /* In order to set color based on port and priority, both the
     * ING_PRI_CNG_MAP and CNG_MAP tables have to be configured.
     * Hardware derives color from the ING_PRI_CNG_MAP table if
     * incoming packet is tagged and PORT_TAB.TRUST_OUTER_DOT1P = 1.
     * If PORT_TAB.TRUST_OUTER_DOT1P = 0, or if packet is untagged,
     * harware derives color from CNG_MAP.
     */

    ing_pri_cng_map_entry_t pri_cng_map[_BCM_TR2_PRI_CNG_MAP_SIZE];
    cng_map_entry_t cng_map_entry;
    void *entries[1];
    port_tab_entry_t pent;
    int index;
    int rv = BCM_E_NONE;
    int new_ptr = 0;
    int old_ptr;
    char *pri_map;
    int alloc_size;
    uint32 val;

    /* Allocate buffer for dma read. */
    alloc_size = _BCM_TR2_PRI_CNG_MAP_SIZE * sizeof (ing_pri_cng_map_entry_t);
    pri_map = soc_cm_salloc(unit, alloc_size, "TR2 pri cng map");
    if (NULL == pri_map) {
        return (BCM_E_MEMORY);
    }
    sal_memset(pri_map, 0, alloc_size);
    sal_memset(pri_cng_map, 0, alloc_size);

    /* Lock the port table */
    soc_mem_lock(unit, PORT_TABm); 

    /* Get man id from port table. */
    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_tr2_port_priority_color_set;
    }
    old_ptr = soc_PORT_TABm_field32_get(unit, &pent, TRUST_DOT1P_PTRf);

    /* Get the existing profile chunk, even if old_ptr == 0, in order 
     * to retain the PRI field of ING_PRI_CNG_MAP.
     */
    index = old_ptr * _BCM_TR2_PRI_CNG_MAP_SIZE;
    rv = soc_mem_read_range(unit, ING_PRI_CNG_MAPm, MEM_BLOCK_ANY, 
                            index, index + _BCM_TR2_PRI_CNG_MAP_SIZE - 1,
                            pri_map);
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_tr2_port_priority_color_set;
    }

    for (index = 0; index < _BCM_TR2_PRI_CNG_MAP_SIZE; index++) {
        entries[0] = soc_mem_table_idx_to_pointer(unit, ING_PRI_CNG_MAPm,
                                                   void *, pri_map, index);
        sal_memcpy(pri_cng_map + index, entries[0],
                   sizeof(ing_pri_cng_map_entry_t));
    }

    /* Set the CNG fields in 2 consecutive entries, whose indices are given by 
     * (priority << 1) | cfi, where cfi = 0 or 1.
     */
    index = prio << 1;
    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, (uint32 *)&pri_cng_map[index],
                        CNGf, _BCM_COLOR_ENCODING(unit, color));
    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, (uint32 *)&pri_cng_map[index+1],
                        CNGf, _BCM_COLOR_ENCODING(unit, color));

    /* Add and point to the new profile chunk */
    entries[0] = pri_cng_map;
    rv = _bcm_ing_pri_cng_map_entry_add(unit, entries,
                                                   _BCM_TR2_PRI_CNG_MAP_SIZE, 
                                                   (uint32 *)&new_ptr);
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_tr2_port_priority_color_set;
    }

    new_ptr /= _BCM_TR2_PRI_CNG_MAP_SIZE;
    soc_PORT_TABm_field32_set(unit, &pent, TRUST_DOT1P_PTRf, new_ptr);
    rv = soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                       SOC_PORT_MOD_OFFSET(unit, port), &pent);
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_tr2_port_priority_color_set;
    }

    soc_cm_sfree(unit, pri_map);
    /* Release port table lock */
    soc_mem_unlock(unit, PORT_TABm);

    if (old_ptr != 0) {
        /* Delete the old profile chunk */
        index = _BCM_TR2_PRI_CNG_MAP_SIZE * old_ptr;
        BCM_IF_ERROR_RETURN(_bcm_ing_pri_cng_map_entry_delete(unit, index));
    } 
    
    /* Configure CNG_MAP */
    if (SOC_REG_IS_VALID(unit, CNG_MAPr)) {
        SOC_IF_ERROR_RETURN(READ_CNG_MAPr(unit, old_ptr, &val));
        soc_reg_field_set(unit, CNG_MAPr, &val,
                          _bcm_xgs3_priority_fields[prio],
                          _BCM_COLOR_ENCODING(unit, color));
        SOC_IF_ERROR_RETURN(WRITE_CNG_MAPr(unit, new_ptr, val));
        if (old_ptr != new_ptr) {
            /* Clear Old CNG_MAP profile */
            SOC_IF_ERROR_RETURN(WRITE_CNG_MAPr(unit, old_ptr, 0));
        }
    } else if (SOC_MEM_IS_VALID(unit, CNG_MAPm)) {
        SOC_IF_ERROR_RETURN(READ_CNG_MAPm(unit, MEM_BLOCK_ANY,
                                         old_ptr, &cng_map_entry));
        soc_CNG_MAPm_field32_set(unit, &cng_map_entry,
                                 _bcm_xgs3_priority_fields[prio],
                                 _BCM_COLOR_ENCODING(unit, color));
        SOC_IF_ERROR_RETURN(WRITE_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                          new_ptr, &cng_map_entry));
        if (old_ptr != new_ptr) {
            /* Clear Old CNG_MAP profile */
            sal_memset(&cng_map_entry, 0, sizeof(cng_map_entry));
            SOC_IF_ERROR_RETURN(WRITE_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                               old_ptr, &cng_map_entry));
        }
    }

    return (rv);

_cleanup_bcm_tr2_port_priority_color_set:
    /* Release port table lock */
    soc_mem_unlock(unit, PORT_TABm);
    soc_cm_sfree(unit, pri_map);
    return (rv);
}

#ifdef INCLUDE_L3
int
_bcm_tr2_svp_field_set(int unit, bcm_gport_t port_id, 
                       soc_field_t field, int value)
{
    int vp, rv = BCM_E_NONE;

    /* Get the VP index from the gport */
    if (BCM_GPORT_IS_MIM_PORT(port_id)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port_id);
    } else if (BCM_GPORT_IS_MPLS_PORT(port_id)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port_id);
    } else if (BCM_GPORT_IS_VXLAN_PORT(port_id)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(port_id);
    } else { 
        return BCM_E_BADID;
    }

    /* Be sure the entry is used and is set for MIM/MPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim) && 
        !_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls) &&
        !_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_BADID;
    }

    if (BCM_GPORT_IS_MIM_PORT(port_id)) {
        rv = _bcm_tr2_mim_svp_field_set(unit, vp, field, value);
    } else if (BCM_GPORT_IS_MPLS_PORT(port_id)) {
#if defined(BCM_MPLS_SUPPORT)
        rv = _bcm_tr2_mpls_svp_field_set(unit, vp, field, value);
#endif
    } else if (BCM_GPORT_IS_VXLAN_PORT(port_id)) {
#if defined(BCM_TRIDENT2_SUPPORT)
        rv = _bcm_td2_vxlan_svp_field_set(unit, vp, field, value);
#endif
    }
    return rv;
}

int
_bcm_tr2_svp_field_get(int unit, bcm_gport_t port_id, 
                       soc_field_t field, int *value)
{
    int vp, rv = BCM_E_NONE;
    source_vp_entry_t svp;

    /* Get the VP index from the gport */
    if (BCM_GPORT_IS_MIM_PORT(port_id)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port_id);
    } else if (BCM_GPORT_IS_MPLS_PORT(port_id)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port_id);
    } else if (BCM_GPORT_IS_VXLAN_PORT(port_id)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(port_id);
    } else { 
        return BCM_E_BADID;
    }

    /* Be sure the entry is used and is set for MIM/MPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim) && 
        !_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls) &&
        !_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_BADID;
    }
    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    *value = soc_SOURCE_VPm_field32_get(unit, &svp, field);
    return rv;
}

#endif /* INCLUDE_L3 */
#endif
