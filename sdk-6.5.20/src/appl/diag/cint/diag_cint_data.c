/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_LIB_CINT)
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>

#include <shared/bsl.h>
#include <sdk_config.h>

#include <soc/i2c.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/intr.h>

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT)
#include <bcm/port.h>
#include <soc/mcm/memregs.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_mem.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/stat.h>
#endif

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/dcmn/init_deinit.h>
#endif

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/multicast.h>
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/multicast.h>
#endif

#ifdef BCM_88750_A0
#include <soc/dfe/fe1600/fe1600_drv.h>
#endif
#ifdef BCM_88950_A0
#include <soc/dfe/fe3200/fe3200_drv.h>
#endif
#ifdef BCM_88790_A0
#include <soc/dnxf/ramon/ramon_drv.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#endif
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#if defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_INTR)
#include <appl/dcmn/rx_los/rx_los.h>
#endif
#endif
#if defined(BCM_DNX_SUPPORT)
#include <appl/diag/dnx/diag_dnx_diag.h>
#endif
#endif /* defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT) */

#if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT)

/* fill the table with the same given entry, using the device specific wrapper for the SLAM DMA function */
static int diag_mem_fill(int unit, char *name, void *buffer) /* buffer needs to be allocated by soc_cm_salloc() / diag_pointer(unit, "dmabuf_alloc", void **pointer_var, char *size) */
{
    int         copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",name);
        return SOC_E_PARAM;
    }
    if (buffer && ((rv = soc_mem_fill(unit, mem, copyno, buffer)))) {
        cli_out("soc_mem_fill() failed in memory %s.%d returned %d\n",
                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ? 0 : copyno, rv);
    }
    return rv;
}

/* fill the table with the same given entry, using the device specific wrapper for the SLAM DMA function */
static int diag_mem_array_fill_range(
    int unit,              /* unit of the memory */
    char *name,            /* Memory, block and array index to be written to */
    unsigned min_ar_index, /* min array index to be written to, not used in memories which are not arrays */
    unsigned max_ar_index, /* max array index to be written to, not used in memories which are not arrays */
    int index_min,         /* first memory index to write to */
    int index_max,         /* last memory index to write to */
    void *buffer)          /* buffer of they entry to write. If the memory can be written to by SLAM DMA,
                              then the buffer must be allocated using soc_cm_salloc() / diag_pointer(unit, "dmabuf_alloc", void **pointer_var, char *size). */
{
    int         copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",name);
        return SOC_E_PARAM;
    }
    if (buffer && ((rv = soc_mem_array_fill_range(unit, mem, min_ar_index, max_ar_index, copyno, index_min, index_max, buffer)))) {
        cli_out("soc_mem_fill() failed in memory %s.%d[%u] indices[%d:%d] returned %d\n",
                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ? 0 : copyno, array_index, min_ar_index, max_ar_index, rv);
    }
    return rv;
}


/* fill the table with the same given entry, using the device specific wrapper for the SLAM DMA function */
static int diag_fill_table(int unit, char *name, mem_val value)
{
    int         copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",name);
        return SOC_E_PARAM;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        return SOC_E_PARAM;
    }

    if (soc_mem_is_readonly(unit, mem)) {
        cli_out("ERROR: Table %s is read-only\n",
                SOC_MEM_UFNAME(unit, mem));
        return SOC_E_PARAM;
    }

#if defined(BCM_PETRA_SUPPORT)
    if(SOC_IS_ARAD(unit)) {
        if ((value) && ((rv = arad_fill_partial_table_with_entry(unit, mem, array_index, array_index, copyno, 0, soc_mem_index_max(unit, mem), value)))) {
            cli_out("Fill ERROR: table %s.%d: 0x%x\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, rv);
        }
    } else
#endif
    {
        cli_out("fast table filling not supported on this device\n");
        rv = SOC_E_UNIT;
    }

    return rv;
}

/* perform an MBIST test, destroying the contents of memories */
static int diag_mbist(int unit, int skip_errors)
{
    uint32 rv = SOC_E_NONE;
    uint32 rv2 = SOC_E_NONE;
    bcm_switch_event_t switch_event;
    bcm_switch_event_control_t type;
    bcm_switch_service_config_t config;
    uint32 value;
    switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    type.index = 0;
    type.action = bcmSwitchEventMask;
    value = 1;
    bcm_switch_event_control_set(unit, switch_event, type, value);
    bcm_switch_service_config_t_init(&config);
    config.enabled = bcmServiceStateDisabled;
    bcm_switch_service_set(unit, bcmServiceCounterCollection, &config);

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

#ifdef BCM_88650_A0
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (SOC_IS_ARDON(unit)) {
            rv = soc_bist_all_ardon(unit, skip_errors);
            if (rv) {
                cli_out("MBIST failed\n");
            }
        } else {
            rv = soc_bist_all_arad(unit, skip_errors);
            if (rv) {
                cli_out("MBIST failed\n");
            }
        }
    } else
#endif
#ifdef BCM_88750_A0
    if (SOC_IS_FE1600(unit)) {
        rv = soc_bist_all_fe1600(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#ifdef BCM_88950_A0
    if (SOC_IS_FE3200(unit)) {
        rv = soc_fe3200_bist_all(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#if defined(BCM_88470_A0) || defined(BCM_88270_A0)
    if (SOC_IS_QAX(unit)) {
        rv = soc_bist_all_qax(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#if  defined(BCM_88675_A0) || defined(BCM_88375_A0)
    if (SOC_IS_JERICHO_AND_BELOW(unit) || SOC_IS_JERICHO_PLUS(unit)) {
        rv = soc_bist_all_jer(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#if  defined(BCM_88790_A0) 
    if (SOC_IS_DNXF(unit)) {
        rv = soc_bist_all_ramon(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#ifdef BCM_88690_A0
    if (SOC_IS_JERICHO2_ONLY(unit)) {
        rv = soc_bist_all_jr2(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#ifdef BCM_88800_A0
    if (SOC_IS_J2C(unit)) {
        rv = soc_bist_all_j2c(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#ifdef BCM_88480_A0
    if (SOC_IS_Q2A(unit)) {
        rv = soc_bist_all_q2a(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
#ifdef BCM_88850_A0
    if (SOC_IS_J2P(unit)) {
        rv = soc_bist_all_j2p(unit, skip_errors);
        if (rv) {
            cli_out("MBIST failed\n");
        }
    } else
#endif
    {
        cli_out("MBIST not supported on this device\n");
        rv = SOC_E_UNIT;
    }
    rv2 = bcm_switch_event_control_set(unit, switch_event, type, value);
    if (rv2 != BCM_E_NONE)
    {
        printf("Error, bcm_switch_event_control_set\n");
        if (rv == SOC_E_NONE)
        {
            rv = rv2;
        }
    }
    return rv;
}

/* perform an MBIST bases SER test, destroying the contents of memories */
static int diag_mbist_ser_test(const int unit, const int skip_errors, const uint32 nof_repeats, const uint32 time_to_wait, const uint32 ser_test_num)
{
    uint32 rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        rv = soc_bist_arad_ser_test(unit, skip_errors, nof_repeats, time_to_wait, ser_test_num);

        if (rv) {
            cli_out("MBIST SER test failed\n");
        }
    } else
#endif
#ifdef BCM_88750_A0
    if (SOC_IS_FE1600(unit)) {
        rv = soc_bist_fe1600_ser_test(unit, skip_errors, nof_repeats, time_to_wait);

        if (rv) {
            cli_out("MBIST SER test failed\n");
        }
    } else
#endif
    {
        cli_out("MBIST SER test not supported on this device\n");
        rv = SOC_E_UNIT;
    }

    return rv;
}

/* read and return the temperature of the given HBM, return a negative value on error */
static int diag_get_hbm_temp(const int unit, const unsigned hbm_number)
{
#ifdef BCM_DNX_SUPPORT
    uint32 temp = -1;
#endif
    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    } else if (!(SOC_IS_J2C(unit) || SOC_IS_JERICHO2_ONLY(unit))) {
        cli_out("Device not supported.\n");
        return SOC_E_PARAM;
    }
#ifdef BCM_DNX_SUPPORT
    if (dnx_drv_soc_dnx_read_hbm_temp(unit, hbm_number, &temp)) {
        cli_out("Failed to read the HBM temperature.\n");
        return SOC_E_FAIL;
    }
    return temp;
#endif
}

/* clear, print or re-populate the Arad direct mapping modport2sysport table */
static int diag_modport2sysport(int unit)
{
    uint32 rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

#if defined(BCM_PETRA_SUPPORT)
    if(SOC_IS_ARAD(unit)) {
        uint32 mod, port;
        ARAD_SYSPORT sysport;
        for (mod = 0; mod < ARAD_NOF_FAPS_IN_SYSTEM; ++mod) {
            for (port = 0; port < ARAD_NOF_FAP_PORTS; ++port) {
                if (arad_sw_db_modport2sysport_get(unit, mod, port, &sysport)) {
                     cli_out("failed to get the mapping for "
                             "module %u port %u\n", mod, port);
                     return SOC_E_FAIL;
                } else if (sysport != ARAD_NOF_SYS_PHYS_PORTS_GET(unit)) {
                     cli_out("%u , %u -> %u\n", mod, port, sysport);
                }
            }
        }
    } else
#endif
    {
        cli_out("not supported on this device\n");
        rv = SOC_E_UNIT;
    }

    return rv;
}

int
fill_memory_with_incremental_field(const int unit, const soc_mem_t mem, const soc_field_t field,
                                unsigned array_index_min, unsigned array_index_max,
                                const int copyno,
                                int index_min, int index_max,
                                const void *initial_entry)
{
    int    rv = 0, mem_words, mem_size, entry_words, indices_num;
    int    index, blk, tmp;
    unsigned array_index;
    uint32 *buf;
    uint32 *buf2;
    const uint32 *input_entry = initial_entry;
    uint32 field_buf[4] = {0}; /* To hold the field, max size 128 bits */

    if (initial_entry == NULL) {
        return SOC_E_PARAM;
    }

    /* get legal values for indices, if too small/big use the memory's boundaries */
    tmp = soc_mem_index_min(unit, mem);
    if (index_min < soc_mem_index_min(unit, mem)) {
      index_min = tmp;
    }
    if (index_max < index_min) {
        index_max = index_min;
    } else {
         tmp = soc_mem_index_max(unit, mem);
         if (index_max > tmp) {
             index_max = tmp;
         }
    }

    entry_words = soc_mem_entry_words(unit, mem);
    indices_num = index_max - index_min + 1;
    mem_words = indices_num * entry_words;
    mem_size = mem_words * 4;

    /* get the initial field from the input */
    soc_mem_field_get(unit, mem, initial_entry, field, field_buf);

    buf = soc_cm_salloc(unit, mem_size, "mem_clear_buf"); /* allocate DMA memory buffer */
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    /* get legal values for memory array indices */
    if (SOC_MEM_IS_ARRAY(unit, mem)) {
        soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        if (maip) {
            if (array_index_max >= maip->numels + maip->first_array_index) {
                array_index_max = (maip->numels - 1) + maip->first_array_index;
            } else if (array_index_min < array_index_max) {
                array_index_min =  maip->first_array_index;
            }
        }
    } else {
        array_index_min = array_index_max = 0;
    }

    /* fill the allocated memory with the input entry */
    for (index = 0; index < mem_words; ++index) {
        buf[index] = input_entry[index % entry_words];
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }
        for (array_index = array_index_min; array_index <= array_index_min; ++array_index) {
            /* update the field of all the entries in the buffer */
            for (index = 0, buf2 = buf; index < indices_num; ++index, buf2+=entry_words) {
                soc_mem_field_set(unit, mem, buf2, field, field_buf); /* set the index */
                /* increment the field, to be used in next entry */
                if (!++field_buf[0]) {
                    if (!++field_buf[1]) {
                        if (!++field_buf[2]) {
                            ++field_buf[3];
                        }
                    }
                }
            }

#ifdef PLISIM
            if (SAL_BOOT_PLISIM) {
                for (index = index_min, buf2 = buf; index <= index_max; ++index, buf2+=entry_words) {
                    if ((rv = soc_mem_array_write(unit, mem, array_index, blk, index, buf2)) < 0) {
                        cli_out("Write ERROR: table %s.%d[%d]: %s\n",
                                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                                0 : copyno, index,
                                soc_errmsg(rv));
                    }
                }
            } else
#endif
            {
                if ((rv = soc_mem_array_write_range(unit, 0, mem, array_index, blk, index_min, index_max, buf)) < 0) {
                    cli_out("Write ERROR: table %s.%d[%d-%d]: %s\n",
                            SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                            0 : copyno, index_min, index_max,
                            soc_errmsg(rv));
                }
            }
        }
    }
    soc_cm_sfree(unit, buf);
    return rv;
}

/* pointer management functions */
static int diag_pointer(int unit, char *func_name, void **p, char *arg)
{
    int rv;
    unsigned long val;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    } else if (!func_name || !p || !arg) {
        return SOC_E_PARAM;
    }
    if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) {
        /* coverity[secure_coding] */
        rv = sscanf(arg + 2, "%lx", &val);
    } else {
        /* coverity[secure_coding] */
        rv = sscanf(arg, "%lu", &val);
    }
    if (rv != 1) {
        cli_out("could not interpret value\n");
        return SOC_E_PARAM;
    }
    if (!sal_strcmp(func_name, "set")) {
        *p = (void*)(uint8*)val;
    } else if (!sal_strcmp(func_name, "print")) {
        cli_out("pointer value: %p\n", *p);
    } else if (!sal_strcmp(func_name, "dmabuf_alloc")) {
        if (!(*p = soc_cm_salloc(unit, val, "diag_pointer"))) {
            cli_out("Could not allocte a DMA buffer of %lx bytes\n", val);
            return SOC_E_MEMORY;
        }
        cli_out("Allocted a %lu bytes DMA buffer at %p\n", val, *p);
    } else if (!sal_strcmp(func_name, "dmabuf_free")) {
        soc_cm_sfree(unit, *p);
    } else {
        cli_out("ERROR: unknown pointer command, use one of: "
            "set, print, dmabuf_alloc dmabuf_free\n");
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

static int diag_soc_device_reset(int unit, int mode, int action)
{
    int rv = 0;

    rv = soc_device_reset(unit, mode, action);
    if (rv) {
        cli_out("Error: soc_device_reset Failed. rv=%d\n", rv);
        return rv;
    }

    return rv;
}

static int diag_cm_get_unit_by_id(uint16 dev_id, int occur, int *unit)
{
    int 
        rv = 0,
        num_devices = 0,
        tmp_occur = 0,
        tmp_unit,
        found = 0;
    uint16 
        tmp_dev_id;
    uint8 
        tmp_rev_id;


    num_devices = soc_cm_get_num_devices();

    for (tmp_unit = 0 ; tmp_unit < num_devices ; tmp_unit++ ) {
        soc_cm_get_id(tmp_unit, &tmp_dev_id, &tmp_rev_id);

        if (tmp_dev_id == dev_id) {
            if (tmp_occur == occur) {
                found = 1;
                break;
            }
            tmp_occur++;
        }
    }

    if (found == 1) {
        *unit = tmp_unit;
    } else {
        *unit = 0xffffffff;
    }

    return rv;
}




static int diag_get_interrupt_id_byname(int unit, char *regName, int reg_index, char* fldName, int bit_in_field, int* interrupt_id)
{
    int rc = SOC_E_NONE;
    soc_reg_t reg=INVALIDr;
    soc_field_t field=INVALIDf;
    soc_regaddrlist_t alist;
    soc_reg_info_t *reginfo;
    int f;
    int field_found = 0;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        cli_out("No interrupts for device\n");
        return BCM_E_INTERNAL;
    }

    if (soc_regaddrlist_alloc(&alist) < 0)
    {
        cli_out("Could not allocate address list.\n");
        return BCM_E_INTERNAL;
    }

    if (parse_symbolic_reference(unit, &alist, regName) < 0)
    {
        cli_out("Syntax error parsing %s\n", regName);
        soc_regaddrlist_free(&alist);
        return BCM_E_INTERNAL;
    }
    else if (alist.count < 1)
    {
        /** wrong register got */
        cli_out("get wrong register %s\n", regName);
        soc_regaddrlist_free(&alist);
        return BCM_E_INTERNAL;
    } else
    {
        /** get register */
        reg = alist.ainfo->reg;
        soc_regaddrlist_free(&alist);

        if (!SOC_REG_IS_VALID(unit,reg)) {
            cli_out("get invalid register %s\n", regName);
            return BCM_E_INTERNAL;
        }
        reginfo = &SOC_REG_INFO(unit, reg);

        for (f = reginfo->nFields - 1; f >= 0; f--)
        {
            soc_field_info_t *fld = &reginfo->fields[f];
            if (sal_strcasecmp(SOC_FIELD_NAME(unit, fld->field), fldName))
            {
                continue;
            }
            field_found = 1;
            field = fld->field;
        }
        if (!field_found) {
            cli_out("can not get field(%s) from %s\n", fldName, regName);
            return BCM_E_INTERNAL;
        }
    }

    rc = soc_get_interrupt_id(unit, reg, reg_index, field, bit_in_field, interrupt_id);
    return rc;
}

#endif /* defined(BCM_SANDSUPPORT) || defined(BCM_ESW_SUPPORT) */
#ifdef BCM_DNX_SUPPORT
static int diag_counter_value_get(
    int unit,
    const char *blkName,
    const char *counterName,
    uint64 *counter)
{
    int rc;
    rc = dnx_diag_get_counter_specific(unit, blkName, counterName, counter);
    return rc;
}

#endif
static void diag_printk(char *str)
{
    cli_out("%s", str);
}

#define DIAG_PRINTK_FLAGS_HEX 0x1

static void diag_printk_int(int num, int flags)
{
    if(flags & DIAG_PRINTK_FLAGS_HEX) {
        cli_out("%x",(uint32)num);
    } else {
        cli_out("%d",num);
    }
}

static void diag_pcie_read(int unit, uint32 addr, uint32 *val, int swap)
{

    *val = CMVEC(unit).read(&CMDEV(unit).dev, addr);

    if(swap) {
        *val = _shr_swap32(*val);
    }

    /* cli_out("%s(): unit=%d, addr=0x%x, swap=%d. after swap: *val=0x%x",
               FUNCTION_NAME(), unit, addr, swap, *val); */
}

static void diag_pcie_write(int unit, uint32 addr, uint32 val, int swap)
{
    /* cli_out("%s(): unit=%d, addr=0x%x, val=0x%x, swap=%d",
               FUNCTION_NAME(), unit, addr, val, swap); */

    if(swap) {
        val = _shr_swap32(val);
    }

    CMVEC(unit).write(&CMDEV(unit).dev, addr, val);

}

#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__) || defined(INCLUDE_CPU_I2C)
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cpu_i2c_write,
                         int, int, chip, 0, 0,
                         int, int, addr, 0, 0,
                         CPU_I2C_BUS_LEN, CPU_I2C_BUS_LEN, alen, 0, 0,
                         int, int, val, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cpu_i2c_read,
                         int, int, chip, 0, 0,
                         int, int, addr, 0, 0,
                         CPU_I2C_BUS_LEN, CPU_I2C_BUS_LEN, alen, 0, 0,
                         int*, int, p_val, 1, 0);
#endif

CINT_FWRAPPER_CREATE_VP1(diag_printk,
                         char*, char, str, 1, 0);

CINT_FWRAPPER_CREATE_VP2(diag_printk_int,
                         int, int, num, 0, 0,
                         int, int, flags, 0, 0);

CINT_FWRAPPER_CREATE_VP4(diag_pcie_read,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0,
                         uint32*,uint32,val,1,0,
                         int,int,swap,0,0);

CINT_FWRAPPER_CREATE_VP4(diag_pcie_write,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,val,0,0,
                         int,int,swap,0,0);

#if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT)
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         diag_reg_bits,
                         int,int,unit,0,0,
                         char*,char,reg_name,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_reg_field_bits,
                         int,int,unit,0,0,
                         char*,char,reg_name,1,0,
                         char*,char,field_name,1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         diag_reg_info_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int*,int,reg_id,1,0,
                         int*,int,block_instance_id,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_reg_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         diag_reg_field_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         char*,char,field_name,1,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_reg_field_only_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         uint32*,uint32,reg_value,1,0,
                         char*,char,field_name,1,0,
                         uint32*,uint32,field_value,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_reg_set,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         diag_reg_field_set,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         char*,char,field_name,1,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_reg_field_only_set,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         uint32*,uint32,reg_value,1,0,
                         char*,char,field_name,1,0,
                         uint32*,uint32,field_value,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_soc_device_reset,
                         int,int,unit,0,0,
                         int,int,mode,0,0,
                         int,int,action,0,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_cm_get_unit_by_id,
                         uint16,uint16,dev_id,0,0,
                         int,int,occur,0,0,
                         int*,int,unit,1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         diag_mem_info_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int* ,int ,mem,1,0,
                         int* ,int ,block_id,1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         diag_mem_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int,int,start,0,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_mem_field_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         char*,char,field_name,1,0,
                         int,int,start,0,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_mem_field_only_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         char*,char,field_name,1,0,
                         int,int,start,0,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_mem_set,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int,int,start,0,0,
                         int,int,count,0,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         diag_mem_field_set,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         char*,char,field_name,1,0,
                         int,int,start,0,0,
                         int,int,count,0,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         diag_mem_field_only_set,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         char*,char,field_name,1,0,
                         int,int,start,0,0,
                         int,int,count,0,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_soc_mem_read_range,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int,int,index1,0,0,
                         int,int,index2,0,0,
                         void*,void,buffer,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_soc_mem_write_range,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int,int,index1,0,0,
                         int,int,index2,0,0,
                         void*,void,buffer,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_mem_fill,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         void*,void,buffer,1,0);

CINT_FWRAPPER_CREATE_RP7(int, int, 0, 0,
                         diag_mem_array_fill_range,
                         int, int, unit, 0, 0,
                         char*, char, name, 1, 0,
                         unsigned int, unsigned int, min_ar_index, 0, 0,
                         unsigned int, unsigned int, max_ar_index, 0, 0,
                         int, int, index_min, 0, 0,
                         int, int, index_max, 0, 0,
                         void*, void, buffer, 1, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_fill_table,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         uint32*,uint32,value,1,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         diag_mbist,
                         int,int,unit,0,0,
                         int,int,skip_errors,0,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         diag_mbist_ser_test,
                         int, int, unit, 0, 0,
                         int, int, skip_errors, 0, 0,
                         uint32, uint32, nof_repeats, 0, 0,
                         uint32, uint32, time_to_wait, 0, 0,
                         uint32, uint32, ser_test_num, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         diag_get_hbm_temp,
                         int,int,unit,0,0,
                         unsigned int,unsigned int,hbm_number,0,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         diag_modport2sysport,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         diag_pointer,
                         int, int, unit, 0, 0,
                         char*, char, func_name, 1, 0,
                         void**, void, p, 2, 0,
                         char*, char, arg, 1, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_direct_reg_get,
                         int,int,unit,0,0,
                         int,int,cmic_block,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,dwc_read,0,0,
                         uint32*,uint32,data,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_direct_reg_set,
                         int,int,unit,0,0,
                         int,int,cmic_block,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,dwc_write,0,0,
                         uint32*,uint32,data,1,0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         soc_direct_memreg_get,
                         int,int,unit,0,0,
                         int,int,cmic_block,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,dwc_read,0,0,
                         int, int, is_mem, 0, 0,
                         uint32*,uint32,data,1,0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         soc_direct_memreg_set,
                         int,int,unit,0,0,
                         int,int,cmic_block,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,dwc_write,0,0,
                         int, int, is_mem, 0, 0,
                         uint32*,uint32,data,1,0);
CINT_FWRAPPER_CREATE_RP6(int,int, 0, 0,
                         diag_get_interrupt_id_byname,
                         int,int,unit,0,0,
                         char *,char,regName, 1,0,
                         int,int,reg_index, 0,0,
                         char *,char,fldName, 1,0,
                         int,int,bit_in_field, 0,0,
                         int *,int,interrupt_id, 1,0);
#ifdef BCM_DNX_SUPPORT
CINT_FWRAPPER_CREATE_RP4(int,int, 0, 0,
                         diag_counter_value_get,
                         int,int,unit,0,0,
                         char *,char,blkName, 1,0,
                         char *,char,counterName, 1,0,
                         uint64 *,uint64,counter, 1,0);
#endif
#endif /* defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT) */

static cint_function_t __cint_diag_functions[] =
{
    CINT_FWRAPPER_ENTRY(diag_printk),
    CINT_FWRAPPER_ENTRY(diag_printk_int),
    CINT_FWRAPPER_ENTRY(diag_pcie_read),
    CINT_FWRAPPER_ENTRY(diag_pcie_write),

#if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT)
    CINT_FWRAPPER_ENTRY(diag_reg_bits),
    CINT_FWRAPPER_ENTRY(diag_reg_field_bits),
    CINT_FWRAPPER_ENTRY(diag_reg_info_get),
    CINT_FWRAPPER_ENTRY(diag_reg_get),
    CINT_FWRAPPER_ENTRY(diag_reg_field_get),
    CINT_FWRAPPER_ENTRY(diag_reg_field_only_get),
    CINT_FWRAPPER_ENTRY(diag_reg_set),
    CINT_FWRAPPER_ENTRY(diag_reg_field_set),
    CINT_FWRAPPER_ENTRY(diag_reg_field_only_set),
    CINT_FWRAPPER_ENTRY(diag_mem_info_get),
    CINT_FWRAPPER_ENTRY(diag_mem_get),
    CINT_FWRAPPER_ENTRY(diag_mem_field_get),
    CINT_FWRAPPER_ENTRY(diag_mem_field_only_get),
    CINT_FWRAPPER_ENTRY(diag_mem_set),
    CINT_FWRAPPER_ENTRY(diag_mem_field_set),
    CINT_FWRAPPER_ENTRY(diag_mem_field_only_set),
    CINT_FWRAPPER_ENTRY(diag_soc_mem_read_range),
    CINT_FWRAPPER_ENTRY(diag_soc_mem_write_range),
    CINT_FWRAPPER_ENTRY(diag_mem_fill),
    CINT_FWRAPPER_ENTRY(diag_mem_array_fill_range),
    CINT_FWRAPPER_ENTRY(diag_fill_table),
    CINT_FWRAPPER_ENTRY(diag_mbist),
    CINT_FWRAPPER_ENTRY(diag_mbist_ser_test),
    CINT_FWRAPPER_ENTRY(diag_get_hbm_temp),
    CINT_FWRAPPER_ENTRY(diag_modport2sysport),
    CINT_FWRAPPER_ENTRY(diag_pointer),
    CINT_FWRAPPER_ENTRY(soc_direct_reg_get),
    CINT_FWRAPPER_ENTRY(soc_direct_reg_set),
    CINT_FWRAPPER_ENTRY(soc_direct_memreg_get),
    CINT_FWRAPPER_ENTRY(soc_direct_memreg_set),
    CINT_FWRAPPER_ENTRY(diag_soc_device_reset),
    CINT_FWRAPPER_ENTRY(diag_cm_get_unit_by_id),
    CINT_FWRAPPER_ENTRY(diag_get_interrupt_id_byname),
#ifdef BCM_DNX_SUPPORT
    CINT_FWRAPPER_ENTRY(diag_counter_value_get),
#endif

#endif
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__) || defined(INCLUDE_CPU_I2C)
    CINT_FWRAPPER_ENTRY(cpu_i2c_write),
    CINT_FWRAPPER_ENTRY(cpu_i2c_read),
#endif

    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_typedefs[] =
{
    {
        "uint32",
        "reg_val",
        0,
        SOC_REG_ABOVE_64_MAX_SIZE_U32
    },
    {
        "uint32",
        "mem_val",
        0,
        SOC_MAX_MEM_WORDS
    },
    {NULL}
};

static cint_constants_t __cint_diag_constants[] =
{
   { "DIAG_PRINTK_FLAGS_HEX", DIAG_PRINTK_FLAGS_HEX },
   { NULL }
};

#if defined(INCLUDE_I2C) && (defined (__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__))
static cint_enum_map_t __cint_diag_enum_map__CPU_I2C_BUS_LEN[] =
{
  { "CPU_I2C_ALEN_NONE_DLEN_BYTE", CPU_I2C_ALEN_NONE_DLEN_BYTE },
  { "CPU_I2C_ALEN_NONE_DLEN_WORD", CPU_I2C_ALEN_NONE_DLEN_WORD },
  { "CPU_I2C_ALEN_NONE_DLEN_LONG", CPU_I2C_ALEN_NONE_DLEN_LONG },
  { "CPU_I2C_ALEN_BYTE_DLEN_BYTE", CPU_I2C_ALEN_BYTE_DLEN_BYTE },
  { "CPU_I2C_ALEN_BYTE_DLEN_WORD", CPU_I2C_ALEN_BYTE_DLEN_WORD },
  { "CPU_I2C_ALEN_WORD_DLEN_WORD", CPU_I2C_ALEN_WORD_DLEN_WORD },
  { "CPU_I2C_ALEN_BYTE_DLEN_LONG", CPU_I2C_ALEN_BYTE_DLEN_LONG },
  { "CPU_I2C_ALEN_WORD_DLEN_LONG", CPU_I2C_ALEN_WORD_DLEN_LONG },
  { "CPU_I2C_ALEN_LONG_DLEN_LONG", CPU_I2C_ALEN_LONG_DLEN_LONG },
  { NULL }
};
#endif

static cint_enum_type_t __cint_diag_enums[] =
{
#if defined(INCLUDE_I2C) && (defined (__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__))
  { "CPU_I2C_BUS_LEN", __cint_diag_enum_map__CPU_I2C_BUS_LEN },
#endif
  { NULL }
};

cint_data_t diag_cint_data =
{
    NULL,
    __cint_diag_functions,
    NULL,
    __cint_diag_enums,
    __cint_typedefs,
    __cint_diag_constants,
    NULL
};

#endif /* INCLUDE_LIB_CINT*/

