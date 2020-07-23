/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Memory address and field manipulations.
 */

#include <shared/bsl.h>

#include <assert.h>

#include <sal/core/libc.h>

#include <soc/dnxc/multithread_analyzer.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#ifdef BCM_ESW_SUPPORT
#include <soc/er_tcam.h>
#endif
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_ESW_SUPPORT
#include <soc/format.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/esw/flow_db.h>
#endif

/*
 * save the last memory used in direct access (usually used for cache)
 * No init required since no harm is done if invalid memory saved here.
 * Thread safe - since the last memory used first copied to a local variable and then used.
 */
soc_mem_t drvmem_last_used_mem_direct_acc[SOC_MAX_NUM_DEVICES] = {0};


/*
 * Macro used by memory accessor functions to fix order
 */
#define FIX_MEM_ORDER_E(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? \
                                BYTES2WORDS((m)->bytes)-1-(v) : \
                                (v))

#ifdef BCM_TRIDENT3_SUPPORT
/*
 * Function:      soc_meminfo_field_get_helper
 * Purpose:       Get a memory field without reference to chip.
 * Parameters:    meminfo   --  direct reference to memory description
 */
STATIC uint32 *
soc_meminfo_field_get_helper(soc_mem_info_t *meminfo,
                      const uint32 *entbuf, 
                      soc_field_info_t *fieldinfo,
                      uint32 *fldbuf, uint32 fldbuf_size)
{
    int len;

    assert(fieldinfo);

    len = fieldinfo->len;
    assert(len / 32 <= fldbuf_size); /* assert we do not write beyond the end of the output buffer */

    return soc_meminfo_fieldinfo_field_get(entbuf,
                                    meminfo,
                                    fieldinfo,
                                    fldbuf);
}
#endif

/*
 * Function:      _soc_field_value_fit
 * Purpose:       Check if value will fit into a memory field. 
 * Parameters:    fieldinfo --  (IN)Direct reference to field description.
 *                value     --  (IN)Value to be checked.   
 * Returns: 
 *      TRUE  - buffer fits into the field. 
 *      FALSE - Otherwise. 
 */
STATIC int
_soc_field_value_fit(soc_field_info_t *fieldinfo, uint32 *value)
{
    uint32      mask;    /* Value mask                      */
    uint16      len;     /* Bits in field                   */
    int         idx;     /* Iteration index.                */

    idx = (fieldinfo->len - 1) >> 5;
    len = fieldinfo->len % 32;

    if (!len) {
       return TRUE; 
    }

    mask = (1 << len) - 1;
    if((value[idx] & ~mask) != 0) {
        return (FALSE);
    }
    return (TRUE);
}


#ifdef BCM_TRIDENT3_SUPPORT

/*
 * Function:      _soc_mem_view_sub_field_set
 * Purpose:       Write to a sub field specified by the start and end positions
 *                to the given user field.
 *
 * Parameters:    usr_field -- the given user field that sub field written to
 *                pos_min --  the start bit position in the given field.
 *                pos_max --  The end bit position in the given field.
 *                frag_field -- point to the sub_field value 
 * Returns:
 *      SOC_E_XXX.
 */

STATIC int
_soc_mem_view_sub_field_set(uint32 *usr_field,
                     int pos_min, int pos_max,
                     uint32 *frag_field)
{
    int i;
    int word_start;
    int bit_start;
    int bit_end;
    int word_num;
    int r_shifter;
    int l_shifter;
    int last_idx;

    word_start = pos_min/32;
    bit_start  = pos_min%32;
    bit_end    = pos_max%32;
    word_num = pos_max/32 - pos_min/32;
    l_shifter = bit_start;
    r_shifter  = 32 - l_shifter;

    /* safety check */
    if (pos_min > pos_max) {
        return SOC_E_INTERNAL;
    }
    /* frag_field must have exact number of bits value. extra bits
     * in the 32-bit word must be cleared
     */
    if ((pos_max - pos_min + 1)%32) {
        frag_field[(pos_max - pos_min + 1)/32] &= 
            ((1 << ((pos_max - pos_min + 1)%32)) - 1);
    }
    for (last_idx = 0, i = 0; i < word_num; i++) {
        usr_field[word_start + i] |= (frag_field[i] << l_shifter);
        if (l_shifter) {
            usr_field[word_start + i + 1] |= (frag_field[i] >> r_shifter);
        }
        last_idx = i + 1;
    }
    if (bit_end >= l_shifter) {
        usr_field[word_start + last_idx] |= (frag_field[last_idx] <<
                              l_shifter);
    }
    return SOC_E_NONE;
}

/*
 * Function:      _soc_mem_view_sub_field_get
 * Purpose:       Retrieve a sub field specified by the start and end positions of
 *                the given user field.
 *
 * Parameters:    usr_field -- the given user field that sub field resides in 
 *                pos_min --   the start bit position in the given field.
 *                pos_max --   the end bit position in the given field.
 *                frag_field -- point to the retrieved sub_field value
 * Returns:
 *      SOC_E_XXX.
 */

STATIC int
_soc_mem_view_sub_field_get(uint32 *usr_field, int pos_min,
                       int pos_max, uint32 *frag_field)
{
    int i;
    uint32 mask;
    int last_idx;
    int word_start;
    int bit_start;
    int bit_end;
    int word_num;
    int r_shifter;
    int l_shfter;

    word_start = pos_min/32;
    bit_start  = pos_min%32;
    bit_end    = pos_max%32;
    word_num = pos_max/32 - pos_min/32;
    r_shifter = bit_start;
    l_shfter  = 32 - r_shifter;

    /* safety check */
    if (pos_min > pos_max) {
        return SOC_E_INTERNAL;
    }
    for (i = 0; i < word_num; i++) {
        frag_field[i] = usr_field[word_start + i] >> r_shifter;
        if (r_shifter) {
            frag_field[i] |= usr_field[word_start + i + 1] << l_shfter;
        }
    }
    last_idx = i - 1;
    if (r_shifter > bit_end) {
        /* last_idx < 0 should never happen if r_shifter > bit_end */
        if (last_idx >= 0) { /* safety check */
            mask = ((uint32)0xFFFFFFFF) >> (r_shifter - (bit_end + 1));
            frag_field[last_idx] &= mask;
        }
    } else {
        mask = ((uint32)0xFFFFFFFF) >> (31 - (bit_end - r_shifter));
        frag_field[last_idx+1] = (usr_field[word_start + last_idx + 1] >>
                              r_shifter) & mask;
    }
    return SOC_E_NONE;
}

/*
 * Function:    _soc_mem_view_field_set
 * Purpose:     Set a memory field (of a memory view)
 * Parameters:  unit - device
 *              mem_view - mem view
 *              entbuf - table entry buffer
 *              field - which field to set
 *              fldbuf - field buffer
 */
STATIC void
_soc_mem_view_field_set(int unit, soc_mem_t mem_view, uint32 *entbuf, soc_field_t field,
                  uint32 *fldbuf)
{
    int rv = SOC_E_NONE;
    soc_mem_t phy_mem;
    soc_field_info_t fieldinfo;
    soc_mem_view_split_field_info_t *sf_info;  /* split field info */
    soc_mem_info_t *meminfo;
    uint32 *val_field;
    int i;
    char *buf_ptr;
    int alloc_size;
 
    if (!soc_feature(unit, soc_feature_flex_flow)) {
        return;
    }

    if (!SOC_MEM_IS_VIEW(unit, mem_view)) {
        return;
    }

    rv = soc_mem_view_phy_mem_get(unit, mem_view, &phy_mem);
    if (rv != SOC_E_NONE) {
        return;
    }
    meminfo = &SOC_MEM_INFO(unit, phy_mem);

    /* check if it is virtual field */
    rv = soc_flow_db_mem_view_field_is_virtual(unit, mem_view,field);

    if (rv) {

        /* use malloc to avoid coverity stack overflow issue */
        alloc_size = sizeof(soc_mem_view_split_field_info_t);
        alloc_size += SOC_MAX_MEM_WORDS * sizeof(uint32);

        buf_ptr = sal_alloc(alloc_size, "Temp buffer for _soc_mem_view_field_set");
        sal_memset((void *)buf_ptr,0,alloc_size);
        sf_info = (soc_mem_view_split_field_info_t *)buf_ptr;
        val_field = (uint32 *)(sf_info + 1);

        rv = soc_mem_view_split_field_info_get(unit,mem_view,field,sf_info);
        if (rv != SOC_E_NONE) {
            sal_free(buf_ptr);
            return;
        }

        for (i = 0; i < sf_info->num_fld; i++) {
            rv = _soc_mem_view_sub_field_get(fldbuf,
                  sf_info->fld[i].v_offset,
                  sf_info->fld[i].field.len + sf_info->fld[i].v_offset - 1, 
                  val_field);
            if (rv != SOC_E_NONE) {
                sal_free(buf_ptr);
                return;
            }
            soc_meminfo_fieldinfo_field_set(entbuf, meminfo, 
                    &sf_info->fld[i].field, val_field);
        }
        sal_free(buf_ptr);

    } else {
        sal_memset(&fieldinfo, 0x0, sizeof(fieldinfo));
        rv = soc_mem_view_fieldinfo_get(unit, mem_view, field, &fieldinfo);
        if (rv != SOC_E_NONE) {
            return;
        }
        soc_meminfo_fieldinfo_field_set(entbuf, meminfo, &fieldinfo, fldbuf);
    }

    return;
}

/*
 * Function:    _soc_mem_view_field_get
 * Purpose:     Get a memory field
 * Parameters:  unit - device
 *              mem_view - table
 *              entbuf - table entry buffer
 *              field - which field to get
 *              fldbuf - field buffer
 */
STATIC uint32*
_soc_mem_view_field_get(int unit, soc_mem_t mem_view, const uint32 *entbuf,
                  soc_field_t field, uint32 *fldbuf, uint32 words)
{
    int rv = SOC_E_NONE;
    soc_mem_t phy_mem;
    soc_field_info_t fieldinfo;
    soc_mem_info_t *meminfo;
    uint32 *val_field;
    soc_mem_view_split_field_info_t *sf_info;  /* split field info */
    int i;
    int fld_word_len;
    char *buf_ptr;
    int alloc_size;

    if (!soc_feature(unit, soc_feature_flex_flow)) {
        return fldbuf;
    }

    if (!SOC_MEM_IS_VIEW(unit, mem_view)) {
        return fldbuf;
    }

    rv = soc_mem_view_phy_mem_get(unit, mem_view, &phy_mem);
    if (rv != SOC_E_NONE) {
        return fldbuf;
    }

    rv = soc_flow_db_mem_view_field_is_virtual(unit, mem_view,field);

    if (rv) {

        /* use malloc to avoid coverity stack overflow issue */
        alloc_size = sizeof(soc_mem_view_split_field_info_t);
        alloc_size += SOC_MAX_MEM_WORDS * sizeof(uint32);
 
        buf_ptr = sal_alloc(alloc_size, "Temp buffer for _soc_mem_view_field_get");
        sal_memset((void *)buf_ptr,0,alloc_size);
        sf_info = (soc_mem_view_split_field_info_t *)buf_ptr;
        val_field = (uint32 *)(sf_info + 1);

        rv = soc_mem_view_split_field_info_get(unit,mem_view,field,sf_info);
        if (rv != SOC_E_NONE) {
            sal_free(buf_ptr);
            return fldbuf;
        }

        /* initialize the fldbuf */
        fld_word_len = (sf_info->width + 31)/32;
        assert (fld_word_len <= words);
        for (i = 0; i < fld_word_len; i++) {
            fldbuf[i] = 0;
        }
        meminfo = &SOC_MEM_INFO(unit, phy_mem);

        for (i = 0; i < sf_info->num_fld; i++) { 
            (void)soc_meminfo_field_get_helper(meminfo, entbuf, &sf_info->fld[i].field, 
                                               val_field, words);
        
            rv = _soc_mem_view_sub_field_set(fldbuf, sf_info->fld[i].v_offset, 
                     sf_info->fld[i].v_offset + sf_info->fld[i].field.len - 1,   
                     val_field);
            if (rv != SOC_E_NONE) {
                sal_free(buf_ptr);
                return fldbuf;
            }
        }
        sal_free(buf_ptr);

    } else {

        sal_memset(&fieldinfo, 0x0, sizeof(fieldinfo));
        rv = soc_mem_view_fieldinfo_get(unit, mem_view, field, &fieldinfo);
        if (rv != SOC_E_NONE) {
            return fldbuf;
        }

        meminfo = &SOC_MEM_INFO(unit, phy_mem);
        return soc_meminfo_field_get_helper(meminfo, entbuf, &fieldinfo, fldbuf, words);
    }
    return fldbuf;
}

/*
 * Function:
 *      _soc_mem_view_field_valid
 * Purpose:
 *      Verify if field is valid & present in memory view
 * Parameters:
 *      mem_view     - (IN)Memory view id
 *      field   - (IN)Field id.
 * Return:
 *      TRUE  -If field is present & valid.
 *      FALSE -Otherwise.
 */
STATIC int
_soc_mem_view_field_valid(int unit, soc_mem_t mem_view, soc_field_t field)
{
    return soc_mem_view_field_valid(unit, mem_view, field);
}

/*
 * Function:
 *      _soc_mem_view_field32_fit
 * Purpose:
 *      Check if uint32 value fits into a memory view field.
 * Parameters: 
 *      unit    - (IN)SOC unit number.
 *      mem     - (IN)Memory view id.
 *      field   - (IN)Field id.
 *      value   - (IN)Value to be checked. 
 * Return:     
 *      SOC_E_PARAM -If value is too big for field, or some other error.
 *      SOC_E_NONE  -Otherwise.
 */
STATIC int 
_soc_mem_view_field32_fit(int unit, soc_mem_t mem_view,
                       soc_field_t field, uint32 value)
{
    int rv = SOC_E_NONE;
    soc_mem_t phy_mem;
    soc_field_info_t fieldinfo;

    rv = soc_mem_view_phy_mem_get(unit, mem_view, &phy_mem);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    sal_memset(&fieldinfo, 0x0, sizeof(fieldinfo));
    rv = soc_mem_view_fieldinfo_get(unit, mem_view, field, &fieldinfo);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    return (_soc_field_value_fit(&fieldinfo, &value)) ?
        SOC_E_NONE : SOC_E_PARAM;
}

/*
 * Function:
 *     _soc_mem_view_field_length
 * Purpose:
 *     the length of a memory view field in bits.
 * Parameters:
 *     unit    - (IN)SOC unit number.
 *     mem     - (IN)Memory view id.
 *     field   - (IN)Field id.
 * Return:
 *     bits in field.
 *     0 if the field is not found.
 */ 
STATIC int
_soc_mem_view_field_length(int unit, soc_mem_t mem_view, soc_field_t field)
{
    int rv = SOC_E_NONE;
    soc_field_info_t fieldinfo;

    sal_memset(&fieldinfo, 0x0, sizeof(fieldinfo));
    rv = soc_mem_view_fieldinfo_get(unit, mem_view, field, &fieldinfo);
    if (rv != SOC_E_NONE) {
        return 0;
    }
    return fieldinfo.len;
}

/*
 * Function:   _soc_mem_view_fields32_modify
 * Purpose:    Modify the value of a fields in a memory view
 * Parameters:
 *       unit         - (IN) SOC unit number.
 *       mem          - (IN) Memory view
 *       index        - (IN) Memory entry index.
 *       field_count  - (IN) Number of fields to modify.
 *       fields       - (IN) Modified fields array.
 *       values       - (IN) New value for each member of fields array.
 * Returns:
 *       SOC_E_XXX
 */
int
_soc_mem_view_fields32_modify(int unit, soc_mem_t mem_view, int index,
                        int field_count, soc_field_t *fields, uint32 *values)
{
    uint32 buf[SOC_MAX_MEM_WORDS];       /* Buffer to read memory entry. */
    int idx;                             /* Iteration index.             */
    int rv;                              /* Operation return status.     */
    int value_changed = 0;               /* Flag of value changed comparing
                                            between input and current reading*/
    soc_mem_t phy_mem;

    /* Field count check before continue. */
    if (field_count == 0) {
        return (SOC_E_NONE);
    }

    rv = soc_mem_view_phy_mem_get(unit, mem_view, &phy_mem);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    /* Check entry index range. */
    if ((index > soc_mem_index_max(unit, phy_mem)) ||
        (index < soc_mem_index_min(unit, phy_mem))) {
        return (SOC_E_PARAM);
    }

    /*  Fields & values sanity check. */
    for (idx = 0; idx < field_count; idx++) {
        if ((NULL == fields + idx) || (NULL == values + idx)) {
           /*
            * COVERITY
            * This is kept intentional for future use
            * or as defensive statement.
            */
            /* coverity[dead_error_line] */
            return (SOC_E_PARAM);
        }

        /* Make sure value fits into memory field. */
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_fit(unit, mem_view, fields[idx], values[idx]));
    }

    /* Lock the memory. */
    soc_mem_lock(unit, phy_mem);
    rv = soc_mem_read(unit, mem_view, MEM_BLOCK_ANY, index, buf);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, phy_mem);
        return (rv);
    }

    /* Set updated values in the buffer. */
    for (idx = 0; idx < field_count; idx ++) {
        if(values[idx] != soc_mem_field32_get(unit, mem_view, buf, fields[idx])) {
            value_changed = 1;
            soc_mem_field32_set(unit, mem_view, buf, fields[idx], values[idx]);
        }
    }

    /* Write buffer back to memory. */
    if(value_changed) {
        rv = soc_mem_write(unit, mem_view, MEM_BLOCK_ALL, index, buf);
    }

    soc_mem_unlock(unit, phy_mem);
    return (rv);
}
#endif

/*
 * Function:     soc_mem_field_length
 * Purpose:      Return the length of a memory field in bits.
 *               Value is 0 if field is not found.
 * Returns:      bits in field
 */
int
soc_mem_field_length(int unit, soc_mem_t mem, soc_field_t field)
{
    soc_field_info_t    *fld;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            return _soc_mem_view_field_length(unit, mem, field);
        }
    }
#endif

    SOC_FIND_FIELD(field,
                   SOC_MEM_INFO(unit, mem).fields,
                   SOC_MEM_INFO(unit, mem).nFields,
                   fld);
    if (fld == NULL) {
        return 0;
    }
    return fld->len;
}

/****************************************************************
 *
 * MEMORY ENTRY VALUE MANIPULATION FUNCTIONS
 */

/*
 * Function:      soc_memacc_get
 * Purpose:       Get a (memory, field) access information structure
 * Parameters:    unit - device
 *                mem - table
 *                field - which field to get
 *                memacc - (OUT) Memory access structure
 */
int
soc_memacc_init(int unit, soc_mem_t mem, soc_field_t fld,
                soc_memacc_t *memacc)
{
    soc_mem_info_t *meminfo;
    soc_field_info_t *finfop;

    if (!SOC_MEM_IS_VALID(unit, mem))  {
        return SOC_E_PARAM;
    }

    meminfo = &SOC_MEM_INFO(unit, mem);
        
    SOC_FIND_FIELD(fld,
                   meminfo->fields,
                   meminfo->nFields,
                   finfop);

    if (finfop == NULL) {
        return SOC_E_PARAM;
    }

    memacc->minfo = meminfo;
    memacc->finfo = finfop;

    return SOC_E_NONE;
}

/*
 * Function:      soc_memacc_field_get
 * Purpose:       Get a memory field without reference to chip.
 * Parameters:    memacc - Memory access structure
 */
uint32 *
soc_memacc_field_get(soc_memacc_t *memacc, const uint32 *entbuf,
                     uint32 *fldbuf)
{
    soc_mem_info_t *meminfo = memacc->minfo;
    soc_field_info_t *fieldinfo = memacc->finfo;
    int                 i, wp, bp, len;

    bp = fieldinfo->bp;
    len = fieldinfo->len;

    if (len == 1) {     /* special case single bits */
        wp = bp / 32;
        bp = bp & (32 - 1);
        if (entbuf[FIX_MEM_ORDER_E(wp, meminfo)] & (1<<bp)) {
            fldbuf[0] = 1;
        } else {
            fldbuf[0] = 0;
        }
        return fldbuf;
    }

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (; len > 0; len -= 32) {
            if (bp) {
                fldbuf[i] =
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] >> bp &
                    ((1 << (32 - bp)) - 1);
                if ( len > (32 - bp) ) {
                    fldbuf[i] |= entbuf[FIX_MEM_ORDER_E(wp, meminfo)] <<
                        (32 - bp);
                }
            } else {
                fldbuf[i] = entbuf[FIX_MEM_ORDER_E(wp++, meminfo)];
            }

            if (len < 32) {
                fldbuf[i] &= ((1 << len) - 1);
            }

            i++;
        }
    } else {
        i = (len - 1) / 32;

        while (len > 0) {
            assert(i >= 0);

            fldbuf[i] = 0;

            do {
                fldbuf[i] =
                    (fldbuf[i] << 1) |
                    ((entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] >>
                      (bp & (32 - 1))) & 1);
                len--;
                bp++;
            } while (len & (32 - 1));

            i--;
        }
    }

    return fldbuf;
}

uint32
soc_memacc_field32_get(soc_memacc_t *memacc, void *entry)
{
    uint32 value;

    /*
     * COVERITY
     *
     * We do this intentionally to use the generic function
     * soc_memacc_field_get() which does the necessary handling.
 */
    /* coverity[address_of] */
    /* coverity[callee_ptr_arith] */
    soc_memacc_field_get(memacc, entry, &value);

    return value;
}

void
soc_memacc_field64_get(soc_memacc_t *memacc, void *entry, uint64 *val64)
{
    uint32 val64_field[2] = {0, 0};

    soc_memacc_field_get(memacc, entry, val64_field);

    COMPILER_64_SET(*val64, val64_field[1], val64_field[0]);
}

/*
 * Function:     soc_memacc_mac_addr_get
 * Purpose:      Get a mac address field in a memory from a mac addr type
 * Returns:      SOC_E_xxx
 */
void
soc_memacc_mac_addr_get(soc_memacc_t *memacc, void *entry, sal_mac_addr_t mac)
{
    uint32              mac_field[2];

    soc_memacc_field_get(memacc, entry, mac_field);

    SAL_MAC_ADDR_FROM_UINT32(mac, mac_field);
}

/*
 * Function:      soc_mem_fieldinfo_get
 * Purpose:       Get a memory field's fieldinfo reference.
 * Parameters:    unit - device
 *                mem - table
 *                field - which field to get
 * NB:
 *    This function is designed for cases where many entries of the
 *    same table and fields are processed in a batch, such as
 *    for traverse functions and tables of counter values.  It provides
 *    the necessary info structure for using
 *    soc_mem_fieldinfo_field_get below.
 */
soc_field_info_t *
soc_mem_fieldinfo_get(int unit, soc_mem_t mem, soc_field_t field)
{
    soc_mem_info_t  *meminfo;
    soc_field_info_t       *finfop; /* Field information structure. */

    /* Verify that memory is present on the device. */
    if (!SOC_MEM_IS_VALID(unit, mem))  {
        return NULL;
    }

    meminfo = &SOC_MEM_INFO(unit, mem);
    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   finfop);

    return finfop;
}

/*
 * Function:      soc_meminfo_fieldinfo_field_get
 * Purpose:       Get a memory field without reference to chip.
 * Parameters:    meminfo   --  direct reference to memory description
 *                fieldinfo   --  direct reference to field description
 * NB:
 *    Callee must verify that the requested memory and
 *    field specification are valid on the device from which the
 *    entry was read.
 *    This function is designed for cases where many entries of the
 *    same table and fields are processed in a batch, such as
 *    for traverse functions and tables of counter values.
 */
uint32 *
soc_meminfo_fieldinfo_field_get(const uint32 *entbuf,
                                soc_mem_info_t *meminfo,
                                soc_field_info_t *fieldinfo,
                                uint32 *fldbuf)
{
    int                 i, wp, bp, len;

    bp = fieldinfo->bp;
    len = fieldinfo->len;

    if (len == 1) {     /* special case single bits */
        wp = bp / 32;
        bp = bp & (32 - 1);
        if (entbuf[FIX_MEM_ORDER_E(wp, meminfo)] & (1<<bp)) {
            fldbuf[0] = 1;
        } else {
            fldbuf[0] = 0;
        }
        return fldbuf;
    }

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (; len > 0; len -= 32) {
            if (bp) {
                fldbuf[i] =
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] >> bp &
                    ((1 << (32 - bp)) - 1);
                if ( len > (32 - bp) ) {
                    fldbuf[i] |= entbuf[FIX_MEM_ORDER_E(wp, meminfo)] <<
                        (32 - bp);
                }
            } else {
                fldbuf[i] = entbuf[FIX_MEM_ORDER_E(wp++, meminfo)];
            }

            if (len < 32) {
                fldbuf[i] &= ((1 << len) - 1);
            }

            i++;
        }
    } else {
        i = (len - 1) / 32;

        while (len > 0) {
            assert(i >= 0);

            fldbuf[i] = 0;

            do {
                fldbuf[i] =
                    (fldbuf[i] << 1) |
                    ((entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] >>
                      (bp & (32 - 1))) & 1);
                len--;
                bp++;
            } while (len & (32 - 1));

            i--;
        }
    }

    return fldbuf;
}

/*
 * Function:    soc_meminfo_fieldinfo_field32_get
 * Purpose:      Get a <=32 bit field out of a memory entry
 * Returns:      The value of the field
 */
uint32
soc_meminfo_fieldinfo_field32_get(soc_mem_info_t *meminfo,
                                  void *entry,
                                  soc_field_info_t *fieldinfo) 
{
    uint32              value;

    /*
     * COVERITY
     *
     * We do this intentionally to use the generic function
     * soc_meminfo_fieldinfo_field_get() which does the necessary handling.
 */
    /* coverity[address_of] */
    /* coverity[callee_ptr_arith] */
    soc_meminfo_fieldinfo_field_get(entry, meminfo, fieldinfo, &value);

    return value;
}

/*
 * Function:    
 *     soc_meminfo_fieldinfo_field64_get
 * Purpose:  
 *     Get a field in a memory for a uint64 type
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     meminfo - (IN) direct reference to memory description
 *     entry - (IN) HW entry buffer.
 *     fieldinfo - direct reference to field description - (IN)
 *     val64 - (OUT) SW uint64 field buffer.
 * Returns:      void
 */
void
soc_meminfo_fieldinfo_field64_get(soc_mem_info_t *meminfo,
                                  void *entry,
                                  soc_field_info_t *fieldinfo,
                                  uint64 *val64)
{
    uint32              val64_field[2] = {0, 0};

    soc_meminfo_fieldinfo_field_get(entry, meminfo,
                                    fieldinfo, val64_field);

    COMPILER_64_SET(*val64, val64_field[1], val64_field[0]);
}

/*
 * Function:      soc_meminfo_field_get
 * Purpose:       Get a memory field without reference to chip.
 * Parameters:    meminfo   --  direct reference to memory description
 */
uint32 *
soc_meminfo_field_get(soc_mem_t mem, soc_mem_info_t *meminfo,
                      const uint32 *entbuf, soc_field_t field, uint32 *fldbuf,
                      uint32 fldbuf_size) /* The number of uint32s in fldbuf */
{
    soc_field_info_t    *fieldinfo;
    int                 i, wp, bp, len;

    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("mem %s field %s is invalid\n"),
                 soc_mem_name[mem], soc_fieldnames[field]));
#endif
        assert(fieldinfo);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    bp = fieldinfo->bp;
    len = fieldinfo->len; 
    assert(len / 32 <= fldbuf_size); /* assert we do not write beyond the end of the output buffer */

    if (len == 1) {     /* special case single bits */
        wp = bp / 32;
        bp = bp & (32 - 1);
        if (entbuf[FIX_MEM_ORDER_E(wp, meminfo)] & (1<<bp)) {
            fldbuf[0] = 1;
        } else {
            fldbuf[0] = 0;
        }
        return fldbuf;
    }

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (; len > 0; len -= 32) {
            if (bp) {
                fldbuf[i] =
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] >> bp &
                    ((1 << (32 - bp)) - 1);
                if ( len > (32 - bp) ) {
                    fldbuf[i] |= entbuf[FIX_MEM_ORDER_E(wp, meminfo)] <<
                        (32 - bp);
                }
            } else {
                fldbuf[i] = entbuf[FIX_MEM_ORDER_E(wp++, meminfo)];
            }

            if (len < 32) {
                fldbuf[i] &= ((1 << len) - 1);
            }

            i++;
        }
    } else {
        i = (len - 1) / 32;

        while (len > 0) {
            assert(i >= 0);

            fldbuf[i] = 0;

            do {
                fldbuf[i] =
                    (fldbuf[i] << 1) |
                    ((entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] >>
                      (bp & (32 - 1))) & 1);
                len--;
                bp++;
            } while (len & (32 - 1));

            i--;
        }
    }

    return fldbuf;
}

/*
 * Function:      _soc_field_value_chop
 * Purpose:       Chop field value so it will fit into a memory field. 
 * Parameters:    fieldinfo --  (IN)Direct reference to field description.
 *                value     --  (IN)Value to be checked.   
 * Returns: 
 *    SOC_E_XXX
 */
STATIC int
_soc_field_value_chop(soc_field_info_t *fieldinfo, uint32 *value)
{
    uint32      mask;    /* Value mask                      */
    uint16      len;     /* Bits in field                   */
    int         idx;     /* Iteration index.                */

    if ((NULL == fieldinfo) || (NULL == value)) {
        return (SOC_E_PARAM);
    }

    idx = (fieldinfo->len - 1) >> 5;
    len = fieldinfo->len % 32;

    if (len) {
        mask = (1 << len) - 1;
        value[idx] &=  mask;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_mem_field_pbmp_fit
 * Purpose:
 *      Check if pbmp fits into a memory field.
 * Parameters: 
 *      unit    - (IN)SOC unit number.
 *      mem     - (IN)Memory id.
 *      field   - (IN)Field id.
 *      value   - (IN)Value to be checked. 
 * Return:     
 *      SOC_E_PARAM -If value is too big for field, or some other error.
 *      SOC_E_NONE  -Otherwise.
 */
int 
soc_mem_field_pbmp_fit(int unit, soc_mem_t mem, 
                       soc_field_t field, uint32 *value)
{
    soc_mem_info_t      *meminfo;
    soc_field_info_t    *fieldinfo;

    /* Get memory info. */
    if (!SOC_MEM_IS_VALID(unit, mem)){
       return SOC_E_PARAM;
    }
    meminfo = &SOC_MEM_INFO(unit, mem);

    /* Get field properties. */
    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   fieldinfo);
    if (!fieldinfo) {
        return SOC_E_PARAM;
    }

    return (_soc_field_value_fit(fieldinfo, value)) ? SOC_E_NONE : SOC_E_PARAM;
}

/*
 * Function:
 *      soc_mem_field32_fit
 * Purpose:
 *      Check if uint32 value fits into a memory field.
 * Parameters: 
 *      unit    - (IN)SOC unit number.
 *      mem     - (IN)Memory id.
 *      field   - (IN)Field id.
 *      value   - (IN)Value to be checked. 
 * Return:     
 *      SOC_E_PARAM -If value is too big for field, or some other error.
 *      SOC_E_NONE  -Otherwise.
 */
int 
soc_mem_field32_fit(int unit, soc_mem_t mem, 
                       soc_field_t field, uint32 value)
{
    soc_mem_info_t      *meminfo;
    soc_field_info_t    *fieldinfo;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            return _soc_mem_view_field32_fit(unit, mem, field, value);
        }
    }
#endif

    /* Get memory info. */
    if (!SOC_MEM_IS_VALID(unit, mem)){
       return SOC_E_PARAM;
    }
    meminfo = &SOC_MEM_INFO(unit, mem);

    /* Get field properties. */
    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   fieldinfo);
    if (!fieldinfo) {
        return (SOC_E_PARAM);
    }

    /* coverity[callee_ptr_arith : FALSE] */
    return (_soc_field_value_fit(fieldinfo, &value)) ?
        SOC_E_NONE : SOC_E_PARAM;
}

/*
 * Function:     soc_memacc_field_set
 * Purpose:      Set a <=32 bit field out of a memory entry
 */
void
soc_memacc_field_set(soc_memacc_t *memacc, uint32 *entbuf, uint32 *fldbuf)
{
    soc_mem_info_t *meminfo = memacc->minfo;
    soc_field_info_t *fieldinfo = memacc->finfo;
    uint32              mask;
    int                 i, wp, bp, len;

    if (NULL == fieldinfo) {
        assert(fieldinfo);
    }

    /* Make sure value fits into the field. */
    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_model : FALSE] */
    if (!_soc_field_value_fit(fieldinfo, fldbuf)) {
        assert(_soc_field_value_fit(fieldinfo, fldbuf)); 
    }

    bp = fieldinfo->bp;

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (len = fieldinfo->len; len > 0; len -= 32) {
            if (bp) {
                if (len < 32) {
                    mask = (1 << len) - 1;
                } else {
                    mask = -1;
                }

                entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask << bp);
                entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                if (len > (32 - bp)) {
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &=
                        ~(mask >> (32 - bp));
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] |=
                        fldbuf[i] >> (32 - bp) & ((1 << bp) - 1);
                }
            } else {
                if (len < 32) {
                    mask = (1 << len) - 1;
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~mask;
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |=
                        fldbuf[i] << bp;
                } else {
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] = fldbuf[i];
                }
            }

            i++;
        }
    } else {                           /* Big endian: swap bits */
        len = fieldinfo->len;

        while (len > 0) {
            len--;
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] &=
                ~(1 << (bp & (32-1)));
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] |=
                (fldbuf[len / 32] >> (len & (32-1)) & 1) << (bp & (32-1));
            bp++;
        }
    }
}
/*
 * Function:     soc_memacc_field32_set
 * Purpose:      Set a <=32 bit field out of a memory entry
 */
void
soc_memacc_field32_set(soc_memacc_t *memacc, void *entry, uint32 value)
{
    /*
     * COVERITY
     *
     * We do this intentionally to use the generic function
     * soc_memacc_field_set() which does the necessary handling.
 */
    /* coverity[address_of] */
    /* coverity[callee_ptr_arith] */
    soc_memacc_field_set(memacc, entry, &value);
}

/*
 * Function:     soc_memacc_field64_set
 * Purpose:      Set a <=64 bit field out of a memory entry
 */
void
soc_memacc_field64_set(soc_memacc_t *memacc, void *entry, uint64 val64)
{
    uint32              val64_field[2];

    val64_field[0] = COMPILER_64_LO(val64);
    val64_field[1] = COMPILER_64_HI(val64);

    soc_memacc_field_set(memacc, entry, val64_field);
}

/*
 * Function:     soc_memacc_mac_addr_set
 * Purpose:      Set a mac address field in a memory from a mac addr type
 */
void
soc_memacc_mac_addr_set(soc_memacc_t *memacc, void *entry,
                        const sal_mac_addr_t mac)
{
    uint32              mac_field[2];

    SAL_MAC_ADDR_TO_UINT32(mac, mac_field);

    soc_memacc_field_set(memacc, entry, mac_field);
}

/*
 * Function:    soc_meminfo_fieldinfo_field_set
 * Purpose:      Set a <=32 bit field out of a memory entry
 */
void
soc_meminfo_fieldinfo_field_set(uint32 *entbuf,
                                  soc_mem_info_t *meminfo,
                                  soc_field_info_t *fieldinfo, 
                                  uint32 *fldbuf)
{
    uint32              mask;
    int                 i, wp, bp, len;

    if (NULL == fieldinfo) {
        assert(fieldinfo);
    }

    /* Make sure value fits into the field. */
    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_model : FALSE] */
    if (!_soc_field_value_fit(fieldinfo, fldbuf)) {
        assert(_soc_field_value_fit(fieldinfo, fldbuf)); 
    }

    bp = fieldinfo->bp;

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (len = fieldinfo->len; len > 0; len -= 32) {
            if (bp) {
                if (len < 32) {
                    mask = (1 << len) - 1;
                } else {
                    mask = -1;
                }

                entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask << bp);
                entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                if (len > (32 - bp)) {
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask >> (32 - bp));
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] |=
                        fldbuf[i] >> (32 - bp) & ((1 << bp) - 1);
                }
            } else {
                if (len < 32) {
                    mask = (1 << len) - 1;
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~mask;
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                } else {
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] = fldbuf[i];
                }
            }

            i++;
        }
    } else {                           /* Big endian: swap bits */
        len = fieldinfo->len;

        while (len > 0) {
            len--;
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] &= ~(1 << (bp & (32-1)));
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] |=
                (fldbuf[len / 32] >> (len & (32-1)) & 1) << (bp & (32-1));
            bp++;
        }
    }
}
/*
 * Function:    soc_meminfo_fieldinfo_field32_set
 * Purpose:      Set a <=32 bit field out of a memory entry
 */
void
soc_meminfo_fieldinfo_field32_set(soc_mem_info_t *meminfo,
                                  void *entry,
                                  soc_field_info_t *fieldinfo, 
                                  uint32 value)
{
    /*
     * COVERITY
     *
     * We do this intentionally to use the generic function
     * soc_meminfo_fieldinfo_field_set() which does the necessary handling.
 */
    /* coverity[address_of] */
    /* coverity[callee_ptr_arith] */
    soc_meminfo_fieldinfo_field_set(entry, meminfo, fieldinfo, &value);
}

/*
 * Function:    soc_meminfo_fieldinfo_field64_get
 * Purpose:      Get a <=64 bit field out of a memory entry
 */
void
soc_meminfo_fieldinfo_field64_set(soc_mem_info_t *meminfo,
                                  void *entry,
                                  soc_field_info_t *fieldinfo,
                                  uint64 val64)
{
    uint32              val64_field[2];

    val64_field[0] = COMPILER_64_LO(val64);
    val64_field[1] = COMPILER_64_HI(val64);

    soc_meminfo_fieldinfo_field_set(entry, meminfo,
                                    fieldinfo, val64_field);
}

/*
 * Function:      soc_meminfo_field_set
 * Purpose:       Set a memory field without reference to chip.
 * Parameters:    meminfo   --  direct reference to memory description
 */
void
soc_meminfo_field_set(soc_mem_t mem, soc_mem_info_t *meminfo, uint32 *entbuf,
                      soc_field_t field, uint32 *fldbuf)
{
    soc_field_info_t    *fieldinfo;
    uint32              mask;
    int                 i, wp, bp, len;

    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("mem %s field %s is invalid\n"),
                 soc_mem_name[mem], soc_fieldnames[field]));
#endif
        assert(fieldinfo);
    }

    /* Make sure value fits into the field. */
    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_model : FALSE] */
    if (!_soc_field_value_fit(fieldinfo, fldbuf)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("mem %s field %s value does not fit\n"),
                 soc_mem_name[mem], soc_fieldnames[field]));
#endif
        assert(_soc_field_value_fit(fieldinfo, fldbuf)); 
    }

    bp = fieldinfo->bp;

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (len = fieldinfo->len; len > 0; len -= 32) {
            if (bp) {
                if (len < 32) {
                    mask = (1 << len) - 1;
                } else {
                    mask = -1;
                }

                entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask << bp);
                entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                if (len > (32 - bp)) {
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask >> (32 - bp));
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] |=
                        fldbuf[i] >> (32 - bp) & ((1 << bp) - 1);
                }
            } else {
                if (len < 32) {
                    mask = (1 << len) - 1;
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~mask;
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                } else {
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] = fldbuf[i];
                }
            }

            i++;
        }
    } else {                           /* Big endian: swap bits */
        len = fieldinfo->len;

        while (len > 0) {
            len--;
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] &= ~(1 << (bp & (32-1)));
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] |=
                (fldbuf[len / 32] >> (len & (32-1)) & 1) << (bp & (32-1));
            bp++;
        }
    }
}
/*
 * Function:
 *      soc_mem_field_valid
 * Purpose:
 *      Verify if field is valid & present in memory
 * Parameters:
 *      mem     - (IN)Memory id.
 *      field   - (IN)Field id.
 * Return:
 *      TRUE  -If field is present & valid.
 *      FALSE -Otherwise.
 */
int
soc_mem_field_valid(int unit, soc_mem_t mem, soc_field_t field)
{
    soc_mem_info_t  *meminfo;
    soc_field_info_t    *finfop; /* Field information structure. */

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            return _soc_mem_view_field_valid(unit, mem, field);
        }
    }
#endif

    /* Verify that memory is present on the device. */
    if (!SOC_MEM_IS_VALID(unit, mem))  {
        return FALSE;
    }

    meminfo = &SOC_MEM_INFO(unit, mem);
    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   finfop);
    return (finfop != NULL) ? TRUE : FALSE;
}

/*
 * Function:    soc_mem_field_get
 * Purpose:     Get a memory field
 * Parameters:  unit - device
 *              mem - table
 *              entbuf - table entry buffer
 *              field - which field to get
 *              fldbuf - field buffer
 */
uint32 *
soc_mem_field_get(int unit, soc_mem_t mem, const uint32 *entbuf,
                  soc_field_t field, uint32 *fldbuf)
{
    soc_mem_info_t *meminfo;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            return _soc_mem_view_field_get(unit, mem, entbuf, field, fldbuf, SOC_MAX_MEM_WORDS);
        }
    }
#endif

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    meminfo = &SOC_MEM_INFO(unit, mem);

    return soc_meminfo_field_get(mem, meminfo, entbuf, field, fldbuf, SOC_MAX_MEM_WORDS);
}

/*
 * Function:    soc_mem_field_set
 * Purpose:     Set a memory field
 * Parameters:  unit - device
 *              mem - table
 *              entbuf - table entry buffer
 *              field - which field to set
 *              fldbuf - field buffer
 */
void
soc_mem_field_set(int unit, soc_mem_t mem, uint32 *entbuf, soc_field_t field,
                  uint32 *fldbuf)
{
    soc_mem_info_t *meminfo;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            _soc_mem_view_field_set(unit, mem, entbuf, field, fldbuf);
            return;
        }
    }
#endif

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    meminfo = &SOC_MEM_INFO(unit, mem);

    soc_meminfo_field_set(mem, meminfo, entbuf, field, fldbuf);
}


/*
 * Function:    soc_mem_field_width_fit_set
 * Purpose:     Set a memory field
 *              Chop the value if fld_buf doesn't fit into the entry.
 * Parameters:  unit - device
 *              mem - table
 *              entbuf - table entry buffer
 *              field - which field to set
 *              fldbuf - field buffer
 */
void
soc_mem_field_width_fit_set(int unit, soc_mem_t mem, uint32 *entbuf, 
                            soc_field_t field, uint32 *fldbuf)
{
    soc_mem_info_t *meminfo;
    soc_field_info_t    *fieldinfo;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    meminfo = &SOC_MEM_INFO(unit, mem);

    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s field %s is invalid\n"),
                 soc_mem_name[mem], soc_fieldnames[field]));
#endif
        assert(fieldinfo);
    }

    /* Chop the value so it fits into the field. */
    _soc_field_value_chop(fieldinfo, fldbuf);

    soc_meminfo_field_set(mem, meminfo, entbuf, field, fldbuf);
}

/*
 * Function:     soc_mem_field32_get
 * Purpose:      Get a <=32 bit field out of a memory entry
 * Returns:      The value of the field
 */
uint32
soc_mem_field32_get(int unit, soc_mem_t mem, const void *entbuf,
                    soc_field_t field)
{
    uint32              value;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            _soc_mem_view_field_get(unit, mem, entbuf, field, &value, 1);
            return value;
        }
    }
#endif

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit, "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }
    soc_meminfo_field_get(mem, &SOC_MEM_INFO(unit, mem), entbuf, field, &value, 1);

    return value;
}

/*
 * Function:     soc_mem_field32_get_def
 * Purpose:      Get a <=32 bit field out of a memory entry,
 *               or a default value if the memory or field does not exist
 * Returns:      The value of the field, or the given default value
 */
uint32
soc_mem_field32_get_def(int         unit,
            soc_mem_t   mem,
            const void  *entbuf,
            soc_field_t field,
            uint32      def
            )
{
    if (soc_mem_field_valid(unit, mem, field)) {
    uint32 value;

    soc_mem_field_get(unit, mem, entbuf, field, &value);
    
    return value;
    }

    return (def);
}

/*
 * Function:     soc_mem_field32_set
 * Purpose:      Set a <=32 bit field out of a memory entry
 * Returns:      void
 */
void
soc_mem_field32_set(int unit, soc_mem_t mem, void *entbuf,
                    soc_field_t field, uint32 value)
{
    soc_mem_field_set(unit, mem, entbuf, field, &value);
}

/*
 * Function:     soc_mem_field32_force
 * Purpose:      Set a <=32 bit field out of a memory entry
 *               without checking for field width.  Lower
 *               bits of value are taken.
 * Returns:      void
 */
void
soc_mem_field32_force(int unit, soc_mem_t mem, void *entry,
                      soc_field_t field, uint32 value)
{
    soc_mem_info_t      *meminfo;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    meminfo = &SOC_MEM_INFO(unit, mem);

    soc_meminfo_field32_force(mem, meminfo, entry, field, value);
}

/*
 * Function:     soc_meminfo_field32_force
 * Purpose:      Set a <=32 bit field out of a memory entry
 *               without checking for field width.  Lower
 *               bits of value are taken.
 * Returns:      Value put in field.
 */
void
soc_meminfo_field32_force(soc_mem_t mem, soc_mem_info_t *meminfo, void *entry,
                          soc_field_t field, uint32 value)
{
    soc_field_info_t    *fieldinfo;
    int                 len;

    SOC_FIND_FIELD(field,
                   meminfo->fields,
                   meminfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("mem %s field %s is invalid\n"),
                 soc_mem_name[mem], soc_fieldnames[field]));
#endif
        assert(fieldinfo);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_opl : FALSE] */
    len = fieldinfo->len;

    assert(len <= 32);

    if (len < 32) {                    /* Force value to fit in field width */
        value &= (1 << len) - 1;
    }

    /*
     * COVERITY
     *
     * We do this intentionally to use the generic function
     * soc_meminfo_field_set() which does the necessary handling.
 */
    /* coverity[address_of] */
    /* coverity[callee_ptr_arith] */
    soc_meminfo_field_set(mem, meminfo, entry, field, &value);
}

/*
 * Function:    soc_mem_mask_field_get
 * Purpose:     Get a memory mask field
 * Parameters:  unit - device
 *              mem - table
 *              entbuf - table entry buffer
 *              field - which field to get
 *              fldbuf - field buffer
 */
uint32 *
soc_mem_mask_field_get(int unit, soc_mem_t mem, const uint32 *entbuf,
                       soc_field_t field, uint32 *fldbuf)
{
    soc_mem_info_t *meminfo;
    uint32 *rfldbuf;
#if defined(BCM_ESW_SUPPORT)
    int dc_val, len, i;
#endif

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    meminfo = &SOC_MEM_INFO(unit, mem);

    rfldbuf = soc_meminfo_field_get(mem, meminfo, entbuf, field, fldbuf, SOC_MAX_MEM_WORDS);

#if defined(BCM_ESW_SUPPORT)
    soc_tcam_get_info(unit, NULL, NULL, &dc_val, NULL);
    if (dc_val) {
        /* TCAM mask polarity is different than software representation */
        i = 0;
        len = soc_mem_field_length(unit, mem, field);
        for (; len > 0; len -= 32) {
            rfldbuf[i++] ^= 0xffffffff;
        }
        if (len & 0x1f) {
            rfldbuf[i - 1] &= (1 << (len & 0x1f)) - 1;
        }
    }
#endif

    return rfldbuf;
}

/*
 * Function:    soc_mem_mask_field_set
 * Purpose:     Set a memory mask field
 * Parameters:  unit - device
 *              mem - table
 *              entbuf - table entry buffer
 *              field - which field to set
 *              fldbuf - field buffer
 */
void
soc_mem_mask_field_set(int unit, soc_mem_t mem, uint32 *entbuf,
                       soc_field_t field, uint32 *fldbuf)
{
    soc_mem_info_t *meminfo;
#if defined(BCM_ESW_SUPPORT)
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];
    int dc_val, len, i, rv;
#endif

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    meminfo = &SOC_MEM_INFO(unit, mem);

#if defined(BCM_ESW_SUPPORT)
    rv = soc_tcam_get_info(unit, NULL, NULL, &dc_val, NULL);
    if (SOC_SUCCESS(rv) && dc_val) {
        /* TCAM mask polarity is different than software representation */
        i = 0;
        len = soc_mem_field_length(unit, mem, field);
        for (; len > 0; len -= 32) {
            buf[i] = fldbuf[i] ^ 0xffffffff;
            i++;
        }
        if (len & 0x1f) {
            buf[i - 1] &= (1 << (len & 0x1f)) - 1;
        }
        soc_meminfo_field_set(mem, meminfo, entbuf, field, buf);
    }
    else 
#endif
    {
        soc_meminfo_field_set(mem, meminfo, entbuf, field, fldbuf);
    }

}

/*
 * Function:    soc_mem_mask_field32_get
 * Purpose:     Get a <= 32 bit mask field out of a memory entry
 * Returns:     The value of the mask field
 */
uint32
soc_mem_mask_field32_get(int unit, soc_mem_t mem, const void *entbuf,
                         soc_field_t field)
{
    uint32 value;
    
    /* coverity[callee_ptr_arith : FALSE] */
    soc_mem_mask_field_get(unit, mem, entbuf, field, &value);

    return value;
}

/*
 * Function:    soc_mem_mask_field32_set
 * Purpose:     Set a <= 32 bit mask field of a memory entry
 * Returns:     void
 */
void
soc_mem_mask_field32_set(int unit, soc_mem_t mem, void *entbuf,
                         soc_field_t field, uint32 value)
{   /* coverity[callee_ptr_arith : FALSE] */ 
    soc_mem_mask_field_set(unit, mem, entbuf, field, &value);
}


#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)
/*
 * Function:   soc_mem_fields32_modify
 * Purpose:    Modify the value of a fields in a memory.
 * Parameters:
 *       unit         - (IN) SOC unit number.
 *       mem          - (IN) Memory.
 *       index        - (IN) Memory entry index.
 *       field_count  - (IN) Number of fields to modify.
 *       fields       - (IN) Modified fields array.
 *       values       - (IN) New value for each member of fields array.
 * Returns:
 *       SOC_E_XXX
 */
int
soc_mem_fields32_modify(int unit, soc_mem_t mem, int index,
                        int field_count, soc_field_t *fields, uint32 *values)
{
    uint32 buf[SOC_MAX_MEM_WORDS];       /* Buffer to read memory entry. */
    int idx;                             /* Iteration index.             */
    int rv;                              /* Operation return status.     */
    int value_changed = 0;               /* Flag of value changed comparing
                                            between input and current reading*/

    /* Field count check before continue. */
    if (field_count == 0) {
        return (SOC_E_NONE);
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_MEM, mem, TRUE));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (SOC_MEM_IS_VIEW(unit, mem)) {
            return _soc_mem_view_fields32_modify(unit, mem, index, field_count, fields, values);
        }
    }
#endif

    /* Check that memory is a valid one for this unit. */
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return (SOC_E_UNAVAIL);
    }

    /* Check entry index range. */
    if ((index > soc_mem_index_max(unit, mem)) ||
        (index < soc_mem_index_min(unit, mem))) {
        return (SOC_E_PARAM);
    }

    /*  Fields & values sanity check. */
    for (idx = 0; idx < field_count; idx++) {
        if ((NULL == fields + idx) || (NULL == values + idx)) {
           /*
            * COVERITY
            * This is kept intentional for future use
            * or as defensive statement.
            */
            /* coverity[dead_error_line] */
            return (SOC_E_PARAM);
        }

        /* Make sure value fits into memory field. */
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_fit(unit, mem, fields[idx], values[idx]));
    }

    /* Lock the memory. */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, buf);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return (rv);
    }

    /* Set updated values in the buffer. */
    for (idx = 0; idx < field_count; idx ++) {
        if(values[idx] != soc_mem_field32_get(unit, mem, buf, fields[idx])) {
            value_changed = 1;
            soc_mem_field32_set(unit, mem, buf, fields[idx], values[idx]);
        }
    }

    /* Write buffer back to memory. */
    if(value_changed) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, buf);
    }

    soc_mem_unlock(unit, mem);
    return (rv);
}

/*
 * Function:   soc_mem_field32_modify
 * Purpose:    Modify the value of a field in a memory entry.
 * Parameters:
 *       unit      - (IN) SOC unit number.
 *       mem       - (IN) Memory.
 *       index     - (IN) Memory entry index number.
 *       field     - (IN) Modified field.
 *       value     - (IN) New field value.
 * Returns:
 *       SOC_E_XXX
 */
int
soc_mem_field32_modify(int unit, soc_mem_t mem, int index,
                       soc_field_t field, uint32 value)
{

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_MEM, mem, TRUE));
    return soc_mem_fields32_modify(unit, mem, index, 1, &field, &value);
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT) */

/*
 * Function:    
 *     soc_mem_field64_set
 * Purpose:  
 *     Set a field in a memory from a uint64 type
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     field - (IN) Memory field id. 
 *     val64 - (IN) SW uint64 field buffer.
 * Returns:      void
 */
void
soc_mem_field64_set(int unit, soc_mem_t mem, void *entry,
                    soc_field_t field, const uint64 val64)
{
    uint32              val64_field[2];

    val64_field[0] = COMPILER_64_LO(val64);
    val64_field[1] = COMPILER_64_HI(val64);

    soc_mem_field_set(unit, mem, entry, field, val64_field);
}

/*
 * Function:    
 *     soc_mem_field64_get
 * Purpose:  
 *     Get a field in a memory for a uint64 type
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     field - (IN) Memory field id. 
 *     val64 - (OUT) SW uint64 field buffer.
 * Returns:      void
 */
void
soc_mem_field64_get(int unit, soc_mem_t mem, void *entry,
                    soc_field_t field, uint64 *val64)
{
    uint32              val64_field[2] = {0, 0};

    soc_mem_field_get(unit, mem, entry, field, val64_field);

    COMPILER_64_SET(*val64, val64_field[1], val64_field[0]);
}

/*
 * Function:     soc_mem_mac_addr_set
 * Purpose:      Set a mac address field in a memory from a mac addr type
 * Returns:      void
 */
void
soc_mem_mac_addr_set(int unit, soc_mem_t mem, void *entry,
                     soc_field_t field, const sal_mac_addr_t mac)
{
    uint32              mac_field[2];

    SAL_MAC_ADDR_TO_UINT32(mac, mac_field);

    soc_mem_field_set(unit, mem, entry, field, mac_field);
}

/*
 * Function:     soc_mem_mac_addr_get
 * Purpose:      Get a mac address field in a memory from a mac addr type
 * Returns:      SOC_E_xxx
 */
void
soc_mem_mac_addr_get(int unit, soc_mem_t mem, const void *entry,
                     soc_field_t field, sal_mac_addr_t mac)
{
    uint32              mac_field[2];

    soc_mem_field_get(unit, mem, entry, field, mac_field);

    SAL_MAC_ADDR_FROM_UINT32(mac, mac_field);
}

/*
 * Function:     soc_mem_mac_address_set
 * Purpose:      Set a MAC address field in a memory from a macaddr type
 *               Use SOC_MEM_MAC_UPPER_ONLY to set upper 24 bits
 *               or SOC_MEM_MAC_LOWER_ONLY to set lower 24 bits
 *               Default to set 48 bits.
 * Returns:      void
 */
void
soc_mem_mac_address_set(int unit, soc_mem_t mem, void *entry,
                        soc_field_t field, const sal_mac_addr_t mac, int flags)
{
    uint32              mac_addr;

    if (flags == SOC_MEM_MAC_UPPER_ONLY) {
        mac_addr = ((mac[0] << 16) | (mac[1] << 8)  | (mac[2] << 0));
        soc_mem_field_set(unit, mem, entry, field, &mac_addr);
    } else if (flags == SOC_MEM_MAC_LOWER_ONLY) {
        mac_addr = ((mac[3] << 16) | (mac[4] << 8)  | (mac[5] << 0));
        soc_mem_field_set(unit, mem, entry, field, &mac_addr);
    } else {
        soc_mem_mac_addr_set(unit, mem, entry, field, mac);
    }
}

/*
 * Function:     soc_mem_mac_address_get
 * Purpose:      Get a MAC address field in a memory from a mac addr type
 *               Use SOC_MEM_MAC_UPPER_ONLY to get upper 64 bits
 *               or SOC_MEM_MAC_LOWER_ONLY to get lower 64 bits
 *               Default to get 48 bits.
 * Returns:      void
 */
void
soc_mem_mac_address_get(int unit, soc_mem_t mem, const void *entry,
                        soc_field_t field, sal_mac_addr_t mac, int flags)
{
    uint32              mac_field;

    if (flags == SOC_MEM_MAC_UPPER_ONLY) {
        soc_mem_field_get(unit, mem, entry, field, &mac_field);
        mac[0] = (uint8) (mac_field >> 16 & 0xff);
        mac[1] = (uint8) (mac_field >> 8 & 0xff);
        mac[2] = (uint8) (mac_field & 0xff);
    } else if (flags == SOC_MEM_MAC_LOWER_ONLY) {
        soc_mem_field_get(unit, mem, entry, field, &mac_field);
        mac[3] = (uint8) (mac_field >> 16 & 0xff);
        mac[4] = (uint8) (mac_field >> 8 & 0xff);
        mac[5] = (uint8) (mac_field & 0xff);
    } else {
        soc_mem_mac_addr_get(unit, mem, entry, field, mac);
    }
}

/*
 * Function:     soc_meminfo_mac_addr_set
 * Purpose:      Set a mac address field in a memory from a mac addr type
 * Returns:      void
 */
void
soc_meminfo_mac_addr_set(soc_mem_t mem, soc_mem_info_t *meminfo, void *entry,
                         soc_field_t field, const sal_mac_addr_t mac)
{
    uint32              mac_field[2];

    SAL_MAC_ADDR_TO_UINT32(mac, mac_field);

    soc_meminfo_field_set(mem, meminfo, entry, field, mac_field);
}

/*
 * Function:    
 *     soc_mem_ip6_addr_set
 * Purpose:  
 *     Set an IP6 address field in a memory from a ip6 addr type
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     field - (IN) Memory field id. 
 *     ip6   - (IN) SW ip6 address buffer.
 *     flags - (IN) SOC_MEM_IP6_UPPER_ONLY to set upper 64 bits
 *                  SOC_MEM_IP6_LOWER_ONLY to set lower 64 bits
 *                  SOC_MEM_IP6_UPPER_96BIT to set upper 96 bits
 *                  0  - set all 128 bits.
 * Returns:      void
 */
void
soc_mem_ip6_addr_set(int unit, soc_mem_t mem, void *entry,
                     soc_field_t field, const ip6_addr_t ip6, int flags)
{
    uint32              ip6_field[4];

    

    if (flags == SOC_MEM_IP6_UPPER_ONLY) {
        ip6_field[1] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        ip6_field[0] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    } else if (flags == SOC_MEM_IP6_LOWER_ONLY) {
        ip6_field[3] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        ip6_field[2] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        soc_mem_field_set(unit, mem, entry, field, (uint32 *)&ip6_field[2]);
    } else if (flags == SOC_MEM_IP6_UPPER_96BIT) {
        ip6_field[2] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        ip6_field[0] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    } else if (flags == SOC_MEM_IP6_LOWER_96BIT) {
        ip6_field[2] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        ip6_field[1] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        ip6_field[0] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    } else if (flags == SOC_MEM_IP6_BITS_119_96) {
        ip6_field[0] = ((ip6[1] << 16) | (ip6[2] << 8)  |
                        (ip6[3] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    } else if (flags == SOC_MEM_IP6_BITS_63_32) {
        ip6_field[0] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    } else if (flags == SOC_MEM_IP6_BITS_31_0) {
        ip6_field[0] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    } else {
        ip6_field[3] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        ip6_field[2] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        ip6_field[1] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        ip6_field[0] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        soc_mem_field_set(unit, mem, entry, field, ip6_field);
    }
}

/*
 * Function:    
 *     soc_mem_ip6_addr_get
 * Purpose:  
 *     Read IP6 address field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     field - (IN) Memory field id. 
 *     ip6   - (OUT) SW ip6 address buffer.
 *     flags - (IN) SOC_MEM_IP6_UPPER_ONLY to get upper 64 bits
 *                  SOC_MEM_IP6_LOWER_ONLY to get lower 64 bits
 *                  SOC_MEM_IP6_UPPER_96BIT to get upper 96 bits
 *                  0  - get all 128 bits.
 * Returns:      void
 */
void
soc_mem_ip6_addr_get(int unit, soc_mem_t mem, const void *entry,
                     soc_field_t field, ip6_addr_t ip6, int flags)
{
    uint32              ip6_field[4];

    

    if (flags == SOC_MEM_IP6_UPPER_ONLY) {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        ip6[0] = (uint8) (ip6_field[1] >> 24);
        ip6[1] = (uint8) (ip6_field[1] >> 16 & 0xff);
        ip6[2] = (uint8) (ip6_field[1] >> 8 & 0xff);
        ip6[3] = (uint8) (ip6_field[1] & 0xff);
        ip6[4] = (uint8) (ip6_field[0] >> 24);
        ip6[5] = (uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[6] = (uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[7] = (uint8) (ip6_field[0] & 0xff);
    } else if (flags == SOC_MEM_IP6_LOWER_ONLY) {
        soc_mem_field_get(unit, mem, entry, field, (uint32 *)&ip6_field[2]);
        ip6[8] = (uint8) (ip6_field[3] >> 24);
        ip6[9] = (uint8) (ip6_field[3] >> 16 & 0xff);
        ip6[10] =(uint8) (ip6_field[3] >> 8 & 0xff);
        ip6[11] =(uint8) (ip6_field[3] & 0xff);
        ip6[12] =(uint8) (ip6_field[2] >> 24);
        ip6[13] =(uint8) (ip6_field[2] >> 16 & 0xff);
        ip6[14] =(uint8) (ip6_field[2] >> 8 & 0xff);
        ip6[15] =(uint8) (ip6_field[2] & 0xff);
    } else if (flags == SOC_MEM_IP6_UPPER_96BIT) {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        ip6[0] = (uint8) (ip6_field[2] >> 24);
        ip6[1] = (uint8) (ip6_field[2] >> 16 & 0xff);
        ip6[2] = (uint8) (ip6_field[2] >> 8 & 0xff);
        ip6[3] = (uint8) (ip6_field[2] & 0xff);
        ip6[4] = (uint8) (ip6_field[1] >> 24);
        ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
        ip6[6] = (uint8) (ip6_field[1] >> 8 & 0xff);
        ip6[7] = (uint8) (ip6_field[1] & 0xff);
        ip6[8] = (uint8) (ip6_field[0] >> 24);
        ip6[9] = (uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[10] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[11] =(uint8) (ip6_field[0] & 0xff);
    } else if (flags == SOC_MEM_IP6_BITS_119_96) {
        soc_mem_field_get(unit, mem, entry, field, (uint32 *)&ip6_field[3]);
        ip6[1] = (uint8) (ip6_field[3] >> 16 & 0xff);
        ip6[2] = (uint8) (ip6_field[3] >> 8 & 0xff);
        ip6[3] = (uint8) (ip6_field[3] & 0xff);
    } else if (flags == SOC_MEM_IP6_BITS_63_32) {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        ip6[8] = (uint8) (ip6_field[0] >> 24);
        ip6[9] = (uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[10] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[11] =(uint8) (ip6_field[0] & 0xff);
    } else if (flags == SOC_MEM_IP6_BITS_31_0) {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        ip6[12] =(uint8) (ip6_field[0] >> 24);
        ip6[13] =(uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[14] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[15] =(uint8) (ip6_field[0] & 0xff);
    } else if (flags == SOC_MEM_IP6_LOWER_96BIT) {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        ip6[4] = (uint8) (ip6_field[2] >> 24);
        ip6[5] = (uint8) (ip6_field[2] >> 16 & 0xff);
        ip6[6] = (uint8) (ip6_field[2] >> 8 & 0xff);
        ip6[7] = (uint8) (ip6_field[2] & 0xff);
        ip6[8] = (uint8) (ip6_field[1] >> 24);
        ip6[9] = (uint8) (ip6_field[1] >> 16 & 0xff);
        ip6[10] =(uint8) (ip6_field[1] >> 8 & 0xff);
        ip6[11] =(uint8) (ip6_field[1] & 0xff);
        ip6[12] =(uint8) (ip6_field[0] >> 24);
        ip6[13] =(uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[14] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[15] =(uint8) (ip6_field[0] & 0xff);
    } else {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        ip6[0] = (uint8) (ip6_field[3] >> 24);
        ip6[1] = (uint8) (ip6_field[3] >> 16 & 0xff);
        ip6[2] = (uint8) (ip6_field[3] >> 8 & 0xff);
        ip6[3] = (uint8) (ip6_field[3] & 0xff);
        ip6[4] = (uint8) (ip6_field[2] >> 24);
        ip6[5] = (uint8) (ip6_field[2] >> 16 & 0xff);
        ip6[6] = (uint8) (ip6_field[2] >> 8 & 0xff);
        ip6[7] = (uint8) (ip6_field[2] & 0xff);
        ip6[8] = (uint8) (ip6_field[1] >> 24);
        ip6[9] = (uint8) (ip6_field[1] >> 16 & 0xff);
        ip6[10] =(uint8) (ip6_field[1] >> 8 & 0xff);
        ip6[11] =(uint8) (ip6_field[1] & 0xff);
        ip6[12] =(uint8) (ip6_field[0] >> 24);
        ip6[13] =(uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[14] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[15] =(uint8) (ip6_field[0] & 0xff);
    }
}

void
soc_mem_ip6_addr_mask_set(int unit, soc_mem_t mem, void *entry,
                          soc_field_t field, const ip6_addr_t ip6, int flags)
{
    uint32 ip6_field[4];

    if (flags == SOC_MEM_IP6_UPPER_ONLY) {
        SAL_IP6_ADDR_HALF_TO_UINT32(ip6, ip6_field);
        soc_mem_mask_field_set(unit, mem, entry, field, ip6_field);
    } else if (flags == SOC_MEM_IP6_LOWER_ONLY) {
        SAL_IP6_ADDR_HALF_TO_UINT32(&ip6[8], &ip6_field[2]);
        soc_mem_mask_field_set(unit, mem, entry, field, &ip6_field[2]);
    } else {
        SAL_IP6_ADDR_TO_UINT32(ip6, ip6_field);
        soc_mem_mask_field_set(unit, mem, entry, field, ip6_field);
    }
}

void
soc_mem_ip6_addr_mask_get(int unit, soc_mem_t mem, const void *entry,
                          soc_field_t field, ip6_addr_t ip6, int flags)
{
    uint32 ip6_field[4];

    if (flags == SOC_MEM_IP6_UPPER_ONLY) {
        soc_mem_mask_field_get(unit, mem, entry, field, ip6_field);
        SAL_IP6_ADDR_HALF_FROM_UINT32(ip6, ip6_field);
    } else if (flags == SOC_MEM_IP6_LOWER_ONLY) {
        soc_mem_mask_field_get(unit, mem, entry, field, &ip6_field[2]);
        SAL_IP6_ADDR_HALF_FROM_UINT32(&ip6[8], &ip6_field[2]);
    } else {
        soc_mem_field_get(unit, mem, entry, field, ip6_field);
        SAL_IP6_ADDR_FROM_UINT32(ip6, ip6_field);
    }
}

void
soc_mem_pbmp_field_set(int unit, soc_mem_t mem, void *entbuf,
                       soc_field_t field, soc_pbmp_t *pbmp)
{
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;

    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(*pbmp, i);
    }

    soc_mem_field_set(unit, mem, entbuf, field, fldbuf);
}

void
soc_mem_pbmp_field_get(int unit, soc_mem_t mem, const void *entbuf,
                       soc_field_t field, soc_pbmp_t *pbmp)
{
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, mem, entbuf, field, fldbuf);

    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        SOC_PBMP_WORD_SET(*pbmp, i, fldbuf[i]);
    }
}

/*
 * Function:     soc_mem_datamask_get
 * Purpose:      Get a bit mask for a particular field in a memory entry
 */
void
soc_mem_datamask_get(int unit, soc_mem_t mem, uint32 *buf)
{
    int                 f, b, start, end;
    soc_field_info_t    *fieldp;
    soc_mem_info_t      *memp;
    uint32              tmp;
    uint32              field_ignore = 0;        
    
    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    memp = &SOC_MEM_INFO(unit, mem);
    sal_memset(buf, 0, sizeof(*buf) * BYTES2WORDS(memp->bytes));

    for (f = 0; f < memp->nFields; f++) {
        fieldp = &(memp->fields[f]);
        field_ignore = 0;
#ifdef BCM_TRIUMPH_SUPPORT
        /*global field overlay with view reserved filed and cause mask error. */
        if (SOC_IS_TRIUMPH3(unit) &&
            (mem == MPLS_ENTRY_EXTDm) &&
            (fieldp->field == ENTRY_2_FROM_ENTRY_1_PART1f)) {
            field_ignore = 1;
        }
#endif

        if ((!(fieldp->flags & SOCF_RES)) && /* not reserved */
            (!field_ignore)) {
            start = fieldp->bp;
            end = fieldp->bp + fieldp->len - 1;
            for (b = start / 32; b <= end / 32; b++) {
                tmp = -1;

                if (b == start / 32) {
                    tmp &= -1 << (start % 32);
                }

                if (b == end / 32) {
                    tmp &= (1 << (end % 32) << 1) - 1;
                }

                buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
            }
        }
    }

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        if (mem == FP_GLOBAL_MASK_TCAM_Xm) {
            soc_mem_pbmp_field_set(unit, mem, buf, IPBMf, &PBMP_XPIPE(unit));
            soc_mem_pbmp_field_set(unit, mem, buf, IPBM_MASKf,
                                   &PBMP_XPIPE(unit));
        } else if (mem == FP_GLOBAL_MASK_TCAM_Ym) {
            soc_mem_pbmp_field_set(unit, mem, buf, IPBMf, &PBMP_YPIPE(unit));
            soc_mem_pbmp_field_set(unit, mem, buf, IPBM_MASKf,
                                   &PBMP_YPIPE(unit));
        } else if (mem == EGR_VLANm) {
            if (soc_mem_field_valid(unit, mem, PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, PORT_BITMAPf,
                                       &PBMP_ALL(unit));
            }
            if (soc_mem_field_valid(unit, mem, UT_PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, UT_PORT_BITMAPf,
                                        &PBMP_ALL(unit));
            }
        } else if (mem == EGR_VLAN_Xm) {
            if (soc_mem_field_valid(unit, mem, PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, PORT_BITMAPf,
                                       &PBMP_XPIPE(unit));
            }
            soc_mem_pbmp_field_set(unit, mem, buf, UT_PORT_BITMAPf,
                                   &PBMP_XPIPE(unit));
        } else if (mem == EGR_VLAN_Ym) {
            if (soc_mem_field_valid(unit, mem, PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, PORT_BITMAPf,
                                       &PBMP_YPIPE(unit));
            }
            soc_mem_pbmp_field_set(unit, mem, buf, UT_PORT_BITMAPf,
                                   &PBMP_YPIPE(unit));
        } else if (mem == EGR_VLAN_VFI_MEMBERSHIPm) {
            soc_mem_pbmp_field_set(unit, mem, buf, PORT_BITMAPf,
                                   &PBMP_ALL(unit));
        } else if (mem == SRC_MODID_INGRESS_BLOCKm) {
            soc_mem_pbmp_field_set(unit, mem, buf, PORT_BITMAPf,
                                   &PBMP_ALL(unit));
        } else if (mem == L3_TUNNELm) {
            if (soc_mem_field_valid (unit, mem, ALLOWED_PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, ALLOWED_PORT_BITMAPf,
                                        &PBMP_ALL(unit));
            }
        } else if (mem == L3_TUNNEL_DATA_ONLYm) {
            if (soc_mem_field_valid (unit, mem, ALLOWED_PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, ALLOWED_PORT_BITMAPf,
                                        &PBMP_ALL(unit));
            }
        } else if (mem == EGR_VLAN_VFI_UNTAGm) {
            if (soc_mem_field_valid(unit, mem, UT_PORT_BITMAPf)) {
                soc_mem_pbmp_field_set(unit, mem, buf, UT_PORT_BITMAPf,
                                        &PBMP_ALL(unit));
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */
}

/*
 * Function:     soc_mem_datamask_rw_get
 * Purpose:      Get a bit mask for a particular field in a memory entry
 */
void
soc_mem_datamask_rw_get(int unit, soc_mem_t mem, uint32 *buf)
{
    int                 f, b, start, end;
    soc_field_info_t    *fieldp;
    soc_mem_info_t      *memp;
    uint32              tmp;
    uint16              access_flag;
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    memp = &SOC_MEM_INFO(unit, mem);
    sal_memset(buf, 0, sizeof(*buf) * BYTES2WORDS(memp->bytes));

    for (f = 0; f < memp->nFields; f++) {
        fieldp = &(memp->fields[f]);
        access_flag =  fieldp->flags & (SOCF_RES | SOCF_RO | SOCF_SIG | SOCF_WO);
       /* if ((!(fieldp->flags & SOCF_RES)) && (!(fieldp->flags & SOCF_RO)) && (!(fieldp->flags & SOCF_SIG))) {    */  /* not reserved */
        if (access_flag == 0) {    /* Get field mask just in case if the field is not read only, is not write only, not a signal and not reserved*/
            start = fieldp->bp;
            end = fieldp->bp + fieldp->len - 1;
            for (b = start / 32; b <= end / 32; b++) {
                tmp = -1;

                if (b == start / 32) {
                    tmp &= -1 << (start % 32);
                }

                if (b == end / 32) {
                    tmp &= (1 << (end % 32) << 1) - 1;
                }

                buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
            }
        }
    }
}

/*
 * Function:     soc_mem_tcammask_get
 * Purpose:      Get a bit mask for TCAM mask field in a memory entry
 */
void
soc_mem_tcammask_get(int unit, soc_mem_t mem, uint32 *buf)
{
    soc_mem_info_t      *memp;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    memp = &SOC_MEM_INFO(unit, mem);
    sal_memset(buf, 0, sizeof(*buf) * BYTES2WORDS(memp->bytes));

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_support)) {
        int               f, b, start, end;
        soc_field_info_t  *fieldp;
        uint32            tmp;

        for (f = 0; f < memp->nFields; f++) {
            fieldp = &(memp->fields[f]);

            if (fieldp->flags & SOCF_RES) {
                continue;
            }

            switch (fieldp->field) {
            case TMW0f:
            case TMW1f:
            case TMW2f:
            case TMW3f:
            case TMW4f:
            case TMW5f:
                break;
            default:
                continue;
            }

            start = fieldp->bp;
            end = fieldp->bp + fieldp->len - 1;

            for (b = start / 32; b <= end / 32; b++) {
                tmp = -1;

                if (b == start / 32) {
                    tmp &= -1 << (start % 32);
                }

                if (b == end / 32) {
                    tmp &= (1 << (end % 32) << 1) - 1;
                }

                buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
            }
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_SHADOW_SUPPORT)
    if (SOC_IS_SHADOW(unit)) {
        int               f, b, start, end;
        soc_field_info_t  *fieldp;
        uint32            tmp;

        if (memp->flags & SOC_MEM_FLAG_CAM) {
            for (f = 0; f < memp->nFields; f++) {
                fieldp = &(memp->fields[f]);

                if (fieldp->flags & SOCF_RES) {
                    continue;
                }

                switch (fieldp->field) {
                case MASKf:
                case MASK0f:
                case MASK1f:
                case FULL_MASKf:
                    break;
                default:
                    continue;
                }

                start = fieldp->bp;
                end = fieldp->bp + fieldp->len - 1;

                for (b = start / 32; b <= end / 32; b++) {
                    tmp = -1;

                    if (b == start / 32) {
                        tmp &= -1 << (start % 32);
                    }

                    if (b == end / 32) {
                        tmp &= (1 << (end % 32) << 1) - 1;
                    }

                    buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
                }
            }
        }
    }
#endif /* BCM_SHADOW_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
    if (soc_feature(unit, soc_feature_xy_tcam)) {
        int               f, b, start, end;
        soc_field_info_t  *fieldp;
        uint32            tmp;

        if (memp->flags & SOC_MEM_FLAG_CAM) {
            for (f = 0; f < memp->nFields; f++) {
                fieldp = &(memp->fields[f]);

                if (fieldp->flags & SOCF_RES) {
                    continue;
                }

                switch (fieldp->field) {
                case MASKf:
                case MASK0f:
                case MASK1f:
                case MASK0_UPRf:
                case MASK1_UPRf:
                case MASK0_LWRf:
                case MASK1_LWRf:
                case DATA_MASKf:
                case FULL_MASKf:
                    break;
                default:
                    continue;
                }

                start = fieldp->bp;
                end = fieldp->bp + fieldp->len - 1;
                for (b = start / 32; b <= end / 32; b++) {
                    tmp = -1;

                    if (b == start / 32) {
                        tmp &= -1 << (start % 32);
                    }

                    if (b == end / 32) {
                        tmp &= (1 << (end % 32) << 1) - 1;
                    }

                    buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
                }
            }
            if (SOC_IS_TD_TT(unit)) {
                if (mem == FP_GLOBAL_MASK_TCAM_Xm) {
                    soc_mem_pbmp_field_set(unit, mem, buf, IPBM_MASKf,
                                           &PBMP_XPIPE(unit));
                } else if (mem == FP_GLOBAL_MASK_TCAM_Ym) {
                    soc_mem_pbmp_field_set(unit, mem, buf, IPBM_MASKf,
                                           &PBMP_YPIPE(unit));
                }
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */
}

/*
 * Function:     soc_mem_eccmask_get
 * Purpose:      Get a bit mask for ECC mask field in a memory entry
 */
void
soc_mem_eccmask_get(int unit, soc_mem_t mem, uint32 *buf)
{
    soc_mem_info_t      *memp;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    memp = &SOC_MEM_INFO(unit, mem);
    sal_memset(buf, 0, sizeof(*buf) * BYTES2WORDS(memp->bytes));

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_support)) {
        int               f, b, start, end;
        soc_field_info_t  *fieldp;
        uint32            tmp;

        for (f = 0; f < memp->nFields; f++) {
            fieldp = &(memp->fields[f]);

            if (fieldp->flags & SOCF_RES) {
                continue;
            }

            switch (fieldp->field) {
            case ECC_ALLf:
            case ECC0_ALLf:
            case ECC1_ALLf:
            case ECC2_ALLf:
            case ECC3_ALLf:
            case ECC4_ALLf:
            case ECC5_ALLf:
            case ECC6_ALLf:
            case ECC7_ALLf:
                break;
            default:
                continue;
            }

            start = fieldp->bp;
            end = fieldp->bp + fieldp->len - 1;

            for (b = start / 32; b <= end / 32; b++) {
                tmp = -1;

                if (b == start / 32) {
                    tmp &= -1 << (start % 32);
                }

                if (b == end / 32) {
                    tmp &= (1 << (end % 32) << 1) - 1;
                }

                buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
            }
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (soc_feature(unit, soc_feature_mem_parity_eccmask)) {
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_ESW_SUPPORT)

        int               f, b, start, end;
        soc_field_info_t  *fieldp;
        uint32            tmp;

        for (f = 0; f < memp->nFields; f++) {
            fieldp = &(memp->fields[f]);

            if (fieldp->flags & SOCF_RES) {
                continue;
            }

            switch (fieldp->field) {
            case ECCPf:
            case ECCf:
            case PARITYf:
            case ECCP0f:
            case ECCP1f:
            case ECCP2f:
            case ECCP3f:
            case ECC0f:
            case ECC1f:
            case ECC2f:
            case ECC3f:
            case PARITY0f:
            case PARITY1f:
            case PARITY2f:
            case PARITY3f:
            case EVEN_PARITYf:
            case ECCP_0f:
            case ECCP_1f:
            case ECCP_2f:
            case ECCP_3f:
            case ECC_0f:
            case ECC_1f:
            case ECC_2f:
            case ECC_3f:
            case PARITY_0f:
            case PARITY_1f:
            case PARITY_2f:
            case PARITY_3f:
            case ECCP_P0f:
            case ECCP_P1f:
            case ECCP_P2f:
            case ECCP_P3f:
            case ECC_P0f:
            case ECC_P1f:
            case ECC_P2f:
            case ECC_P3f:
            case PARITY_P0f:
            case PARITY_P1f:
            case PARITY_P2f:
            case PARITY_P3f:
            case EVEN_PARITY_P0f:
            case EVEN_PARITY_P1f:
            case EVEN_PARITY_P2f:
            case EVEN_PARITY_P3f:               
            case ECCP_PBM_0f:
            case ECCP_PBM_1f:
            case ECCP_PBM_2f:
            case ECCP_PBM_3f:
            case ECC_PBM_0f:
            case ECC_PBM_1f:
            case ECC_PBM_2f:
            case ECC_PBM_3f:
            case PARITY_PBM_0f:
            case PARITY_PBM_1f:
            case PARITY_PBM_2f:
            case PARITY_PBM_3f:
            case TCAM_PARITY_MASKf:
            case TCAM_PARITY_KEYf:
                break;
            default:
                continue;
            }

            start = fieldp->bp;
            end = fieldp->bp + fieldp->len - 1;

            for (b = start / 32; b <= end / 32; b++) {
                tmp = -1;

                if (b == start / 32) {
                    tmp &= -1 << (start % 32);
                }

                if (b == end / 32) {
                    tmp &= (1 << (end % 32) << 1) - 1;
                }

                buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
            }
        }
#endif /* BCM_KATANA2_SUPPORT || BCM_GREYHOUND_SUPPORT || BCM_ESW_SUPPORT */
    }
}

/*
 * Function:     soc_mem_forcedata_get
 * Purpose:      Get a bit mask and value for non-zero sticky fields in a
 *               memory entry
 */
void
soc_mem_forcedata_get(int unit, soc_mem_t mem, uint32 *maskbuf,
                      uint32 *databuf)
{
    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    sal_memset(maskbuf, 0, sizeof(*maskbuf) * soc_mem_entry_words(unit, mem));
    sal_memset(databuf, 0, sizeof(*databuf) * soc_mem_entry_words(unit, mem));

#if defined(BCM_TRIDENT_SUPPORT)
    if (mem == HG_TRUNK_GROUPm &&
        soc_feature(unit, soc_feature_hg_trunk_16_members)) {
        int field_len;
        field_len = soc_mem_field_length(unit, mem, TG_SIZEf);
        soc_mem_field32_set(unit, mem, maskbuf, TG_SIZEf,
                            (1 << field_len) - 1);
        soc_mem_field32_set(unit, mem, databuf, TG_SIZEf, 0xf);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit) && (
        mem == SR_SN_HISTORY_0m || mem == SR_SN_HISTORY_1m || mem == SR_SN_HISTORY_2m ||
        mem == SR_SN_HISTORY_3m || mem == SR_SN_HISTORY_4m || mem == SR_SN_HISTORY_5m ||
        mem == SR_SN_HISTORY_6m || mem == SR_SN_HISTORY_7m || mem == SR_SN_HISTORY_8m ||
        mem == SR_SN_HISTORY_9m || mem == SR_SN_HISTORY_10m || mem == SR_SN_HISTORY_11m ||
        mem == SR_SN_HISTORY_12m || mem == SR_SN_HISTORY_13m || mem == SR_SN_HISTORY_14m ||
        mem == SR_SN_HISTORY_15m )) {
        soc_mem_field32_set(unit, mem, databuf, BLOCK_SELECTf,
                            ((1 << soc_mem_field_length(unit, mem, BLOCK_SELECTf)) - 1));
        soc_mem_field32_set(unit, mem, maskbuf, BLOCK_SELECTf,
                            ((1 << soc_mem_field_length(unit, mem, BLOCK_SELECTf)) - 1));
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

}

#if defined(BCM_ESW_SUPPORT)
void
soc_mem_base_to_wide_entry_conv(int unit, soc_mem_t dest_mem, soc_mem_t src_mem, 
                                uint32 *dest, uint32 *src[4], uint8 conv_type)
{
    uint32 field[SOC_MAX_MEM_FIELD_WORDS];
    switch (conv_type) {
    case TYPE_1_TO_TYPE_2:        
        soc_mem_field_set(unit, dest_mem, dest, ENTRY_2_FROM_ENTRY_1_PART0f,
                          soc_mem_field_get(unit, src_mem, src[0], 
                                            WIDE_ENTRY_BITSf, field));
        soc_mem_field_set(unit, dest_mem, dest, ENTRY_2_FROM_ENTRY_1_PART1f,
                          soc_mem_field_get(unit, src_mem, src[1], 
                                            WIDE_ENTRY_BITSf, field));
        soc_mem_field32_set(unit, dest_mem, dest, HIT_BITSf,
                          soc_mem_field32_get(unit, src_mem, src[0], 
                                              HIT_BITSf));
        return;
    case TYPE_1_TO_TYPE_4:
        soc_mem_field_set(unit, dest_mem, dest, ENTRY_4_FROM_ENTRY_1_PART0f,
                          soc_mem_field_get(unit, src_mem, src[0], 
                                            WIDE_ENTRY_BITSf, field));
        soc_mem_field_set(unit, dest_mem, dest, ENTRY_4_FROM_ENTRY_1_PART1f,
                          soc_mem_field_get(unit, src_mem, src[1], 
                                            WIDE_ENTRY_BITSf, field));
        soc_mem_field_set(unit, dest_mem, dest, ENTRY_4_FROM_ENTRY_1_PART2f,
                          soc_mem_field_get(unit, src_mem, src[2], 
                                            WIDE_ENTRY_BITSf, field));
        soc_mem_field_set(unit, dest_mem, dest, ENTRY_4_FROM_ENTRY_1_PART3f,
                          soc_mem_field_get(unit, src_mem, src[3], 
                                            WIDE_ENTRY_BITSf, field));
        soc_mem_field32_set(unit, dest_mem, dest, HIT_BITSf,
                            soc_mem_field32_get(unit, src_mem, src[0], 
                                                HIT_BITSf));
        return;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "Unimplemented convertion type: %d\n"), conv_type));
        assert(0);
    }
}
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)

/*
 * Function:    soc_mem_addr
 * Purpose:     Turn a memory, block, and index into a memory address
 * Returns:     address to send off in an schannel message
 */
/*
 * Regular:      | Table(8) | Block(4) | Region(4) | Index(16) |
 * Monolithic:   | Table(8) | Index(24)                        |
 */
uint32
soc_mem_addr(int unit, soc_mem_t mem, unsigned array_index, int blk, int index)
{
    uint32              blkoff;
    soc_mem_info_t      *mip;
    soc_mem_array_info_t *maip;
    uint32              base;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    assert(blk >= 0 && blk < SOC_MAX_NUM_BLKS);
    assert(index >= 0);

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_support)) {
        if ((SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_ESM) || 
           (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_ETU)) {
            SOC_IF_ERROR_RETURN(soc_tcam_mem_index_to_raw_index(unit,
                                                                mem,
                                                                index,
                                                                &mem,
                                                                &index));
        }
    }
#endif

    mip = &SOC_MEM_INFO(unit, mem);

#if defined(BCM_TRIUMPH3_SUPPORT) 
    if (soc_feature(unit, soc_feature_etu_support)) {
        if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_ISM) {
            if (mip->flags & (SOC_MEM_FLAG_CAM | SOC_MEM_FLAG_EXT_CAM)) {
                SOC_IF_ERROR_RETURN(soc_tcam_mem_index_to_raw_index(unit,
                                                                mem,
                                                                index,
                                                                &mem,
                                                                &index));
            }
        } 
        /* do it again if returned mem is different */
        mip = &SOC_MEM_INFO(unit, mem);
    }
#endif
    if ((blk>=32)?(mip->blocks_hi & (1 << (blk&0x1F))):(mip->blocks & (1 << blk))) {
        blkoff = ((SOC_BLOCK2OFFSET(unit, blk) & 0xf) << SOC_BLOCK_BP) |
                 (((SOC_BLOCK2OFFSET(unit, blk) >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
    } else {
        blkoff = 0;
    }

    base = mip->base;

#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_FE1600(unit)) {
        base = (((mip->base >> 20) & 0x3F) << 24) | (mip->base & 0xFFFFF);
    }
#endif

    if (array_index) {
        /* non zero array index, implying this is a memory array */
        assert (mip->flags & SOC_MEM_FLAG_IS_ARRAY); /* verify this is really a memory array */
        maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        assert(maip);                       /* verify this is memory array information exists */
        assert(array_index >= maip->first_array_index && array_index < maip->numels + maip->first_array_index); /* verify that the array index is in range */
        LOG_VERBOSE(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "addr: %x, mip->base: %x, blkoff: %x, "
                             "index = %d, mip->gran: %d, * = %x, arr_in = %u, skip = %u\n"), 
                  base+blkoff+(index * mip->gran) + (array_index * maip->element_skip),
                  mip->base,blkoff, index, mip->gran,
                  (index * mip->gran), array_index, maip->element_skip));
        return base + blkoff + (index * mip->gran) + (array_index * maip->element_skip);
    }

    LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "addr: %x, mip->base: %x, blkoff: %x, "
                         "index = %d, mip->gran: %d, * = %x\n"), 
              base+blkoff+(index * mip->gran),
              mip->base,blkoff, index, mip->gran,
              (index * mip->gran)));
    return base + blkoff + (index * mip->gran);
}

/*
 * Function:    soc_mem_addr_get
 * Purpose:     Turn a memory, block, and index into a memory address
 * Returns:     address to send off in an schannel message
 */
/*
 * Regular:      | Table(8) | Block(4) | Region(4) | Index(16) |
 * Monolithic:   | Table(8) | Index(24)                        |
 */
uint32
soc_mem_addr_get(int unit, soc_mem_t mem, unsigned array_index, soc_block_t block, 
                 int index, uint8 *acc_type)
{
    soc_mem_info_t       *mip;
    soc_mem_array_info_t *maip;

    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        return soc_mem_addr(unit, mem, array_index, block, index);
    }
    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    assert(block >= 0 && block < SOC_MAX_NUM_BLKS);
    assert(index >= 0);
    
    *acc_type = SOC_MEM_ACC_TYPE(unit, mem);
#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_support)) {
        if ((SOC_BLOCK_TYPE(unit, block) == SOC_BLK_ESM) ||
           (SOC_BLOCK_TYPE(unit, block) == SOC_BLK_ETU)) {
            SOC_IF_ERROR_RETURN(soc_tcam_mem_index_to_raw_index(unit,
                                                                mem,
                                                                index,
                                                                &mem,
                                                                &index));
        }
    }
#endif

    mip = &SOC_MEM_INFO(unit, mem);

#if defined(BCM_TRIUMPH3_SUPPORT) 
    if (soc_feature(unit, soc_feature_etu_support)) {
        if (SOC_BLOCK_TYPE(unit, block) == SOC_BLK_ISM) {
            if (mip->flags & (SOC_MEM_FLAG_CAM | SOC_MEM_FLAG_EXT_CAM)) {
                SOC_IF_ERROR_RETURN(soc_tcam_mem_index_to_raw_index(unit,
                                                                mem,
                                                                index,
                                                                &mem,
                                                                &index));
            }
        } 
        /* do it again if returned mem is different */
        mip = &SOC_MEM_INFO(unit, mem);
    }
#endif
    if (array_index) {
        /* non zero array index, implying this is a memory array */
        assert (mip->flags & SOC_MEM_FLAG_IS_ARRAY); /* verify this is really a memory array */
        maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        assert(maip);                       /* verify this is memory array information exists */
        assert(array_index >= maip->first_array_index && array_index < maip->numels + maip->first_array_index); /* verify that the array index is in range */
        if (array_index > maip->first_array_index) {
            LOG_VERBOSE(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "addr: %x, mip->base: %x, block: %x, "
                             "index = %d, mip->gran: %d, * = %x, arr_in = %u, skip = %u\n"), 
                  mip->base + (index * mip->gran) + ((array_index - maip->first_array_index) * maip->element_skip), mip->base,
                  SOC_BLOCK2OFFSET(unit, block), index, 
                  mip->gran, (index * mip->gran), array_index, maip->element_skip));
            return mip->base + (index * mip->gran) + ((array_index - maip->first_array_index) * maip->element_skip);
        } else {
        LOG_VERBOSE(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "addr: %x, mip->base: %x, block: %x, "
                             "index = %d, mip->gran: %d, * = %x\n"), 
                  mip->base+(index * mip->gran), mip->base,
                  SOC_BLOCK2OFFSET(unit, block), index, 
                  mip->gran, (index * mip->gran)));
            return mip->base + (index * mip->gran);
        }
    } else {
        /* non memory array acces, works also with index 0 of a memory array */
        LOG_VERBOSE(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "addr: %x, mip->base: %x, block: %x, "
                             "index = %d, mip->gran: %d, * = %x\n"), 
                  mip->base+(index * mip->gran), mip->base,
                  SOC_BLOCK2OFFSET(unit, block), index, 
                  mip->gran, (index * mip->gran)));
        return mip->base + (index * mip->gran);
    }
}


#define ACC_TYPE_MASK 0xE0000
/* Translate a memory address to the memory using it
 * If array_index is not NULL and the found memory is a memory array,
 * then the array index is returned in array_index .
 */
soc_mem_t
soc_addr_to_mem(int unit, uint32 address, uint32 *mem_block)
{
    soc_mem_t       mem;
    uint32          offset, min_addr, max_addr, block = 0;

    offset = address & ~0xC0f00000; /* strip block id */
    if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
        offset &= ~ACC_TYPE_MASK; /* strip memAcc*/
    }
#ifdef BCM_DFE_SUPPORT
    if(SOC_DRIVER(unit)->type == SOC_CHIP_BCM88750_A0 || 
       SOC_DRIVER(unit)->type == SOC_CHIP_BCM88750_B0 ||
       SOC_DRIVER(unit)->type == SOC_CHIP_BCM88754_A0 ||
       SOC_DRIVER(unit)->type == SOC_CHIP_BCM88755_B0) {
        offset = (offset & 0x000fffff) | (((offset >> 24 ) & 0x3F) << 20);
    }
#endif
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (soc_mem_is_valid(unit, mem) &&
            ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0) ) {

            min_addr = max_addr = SOC_MEM_INFO(unit, mem).base;
            if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
                min_addr = max_addr = max_addr & ~ACC_TYPE_MASK; /* strip memAcc*/
            }
            min_addr += SOC_MEM_INFO(unit, mem).index_min;
            max_addr += SOC_MEM_INFO(unit, mem).index_max;
            if (SOC_MEM_IS_ARRAY(unit, mem)) {
                if (offset < min_addr || offset > max_addr +
                      (SOC_MEM_NUMELS(unit, mem) - 1 + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem)) * SOC_MEM_ELEM_SKIP(unit, mem)) {
                    continue;
                }
                /* now make sure that the address is legal for the memory array */
                offset = (offset - min_addr) % SOC_MEM_ELEM_SKIP(unit, mem); /* calculate the array index and check it is in range */
                if (offset >= SOC_MEM_NUMELS(unit, mem)) {
                    continue;
                }
                offset += SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            } else if (offset < min_addr || offset > max_addr) {
                continue;
            }

            if (SOC_IS_XGS3_SWITCH(unit)) {
                /* Match block */
                block = ((address >> SOC_BLOCK_BP) & 0xf) | 
                        (((address >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
                if (block != SOC_BLOCK2OFFSET(unit, SOC_MEM_BLOCK_ANY(unit, mem))) {
                    continue;
                }
            }
            if (mem_block) {
                *mem_block = block;
            }
            return mem;
        }
    }
    return INVALIDm;
}
int
soc_addr_is_mem(int unit, int acc_type, uint32 block, int blk, uint32 offset, soc_mem_t mem)
{
    uint32          min_addr, max_addr;
    int copyno;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint32 block_tmp = block;
    int rv;
#endif

    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0) ) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = soc_th3_mmu_mem_blk_remap(mem, &block);
            if (rv == SOC_E_NOT_FOUND) {
                block = block_tmp;
            }
        }
#endif

        if (SOC_IS_SAND(unit) && blk != SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))) {
            return FALSE; /* skip memory if not belonging the correct block */
        }

        min_addr = max_addr = SOC_MEM_INFO(unit, mem).base;
        min_addr += SOC_MEM_INFO(unit, mem).index_min;
        max_addr += SOC_MEM_INFO(unit, mem).index_max;
        if (SOC_MEM_IS_ARRAY(unit, mem)) {
            if (offset < min_addr || offset > max_addr +
                  (SOC_MEM_NUMELS(unit, mem) - 1) * SOC_MEM_ELEM_SKIP(unit, mem)) {
                return FALSE;
            }
            /* now make sure that the address is legal for the memory array */
            if ( (offset - min_addr) % SOC_MEM_ELEM_SKIP(unit, mem) /* in this line (index - index_min) is computed */
                 > max_addr - min_addr ) { /* Is the index too big */
                return FALSE;
            }
        } else if (offset < min_addr || offset > max_addr) {
            return FALSE;
        }
        if (SOC_IS_XGS3_SWITCH(unit)) {

            /* Match block */
            SOC_MEM_BLOCK_ITER2(unit, mem, copyno) {
                if (block == SOC_BLOCK2OFFSET(unit, copyno)) {
                    break;
                }
            }
            if (copyno > SOC_MEM_BLOCK_MAX(unit, mem)) {
                return FALSE;
            }

            if ((acc_type >= 0) &&
                (acc_type != SOC_MEM_ACC_TYPE(unit, mem))) {
                return FALSE;
            }
        }
        if (SOC_IS_SAND(unit) || SOC_IS_GREYHOUND(unit)
            || SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
            /* check that the memory belongs to the correct block */
            if (blk != SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))) {
                return FALSE;
            }
        }
        return TRUE;
    }

    return FALSE;
}
/* Note: acc_type = -1 is used as don't care */
soc_mem_t
soc_addr_to_mem_extended(int unit, uint32 block, int acc_type, uint32 address)
{
    soc_mem_t       mem;
    uint32          offset;
    int blk = -1;
    offset = address;
#ifdef BCM_DFE_SUPPORT
    if(SOC_DRIVER(unit)->type == SOC_CHIP_BCM88750_A0 || 
       SOC_DRIVER(unit)->type == SOC_CHIP_BCM88750_B0 ||
       SOC_DRIVER(unit)->type == SOC_CHIP_BCM88754_A0 ||
       SOC_DRIVER(unit)->type == SOC_CHIP_BCM88755_B0) {
        offset = (address & 0x000fffff) | (((address >>24 ) & 0x3F) << 20);
    }
#endif
    if (SOC_IS_SAND(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
        /* Find the block of the given block */
        for (blk = 0; ; ++blk) {
            if (SOC_BLOCK_TYPE(unit, blk) < 0) {
                return INVALIDm;
            } else if (SOC_BLOCK2OFFSET(unit, blk) == block) {
                break;
            }
        }
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit) && SOC_BLOCK_IS_BROADCAST(unit, blk)) {
            blk = SOC_BLOCK_BROADCAST_MEMBER(unit, blk, 0); /* For broadcast blocks we want the type of the broadcast members */
        }
#endif /* BCM_JERICHO_SUPPORT */
        blk = SOC_BLOCK_TYPE(unit, blk); /* Get block type for later comparisons from block */
    }

    /* first look in already used mems for optimization*/
    if (soc_feature(unit, soc_feature_mem_direct_acc_last_used_first))
    {

        soc_mem_t last_used_mem = drvmem_last_used_mem_direct_acc[unit];
        if (soc_addr_is_mem(unit, acc_type, block, blk, offset, last_used_mem))
        {
            return last_used_mem;
        }
    }

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (soc_addr_is_mem(unit, acc_type, block, blk, offset, mem))
        {
            if (soc_feature(unit, soc_feature_mem_direct_acc_last_used_first))
            {
                drvmem_last_used_mem_direct_acc[unit] = mem;
            }
            return mem;
        }
    }

    return INVALIDm;
}

/*
 * Function:     soc_mem_addr_to_array_element_and_index     
 * Purpose:      For the given memory and a given address of its entry, return
 *               the entry index in index, and the array index in array_index.
 *               If the memory is not an array the returned array index is 0.
 * Returns:      SOC_E_NONE if address is compatible to mem, otherwise - SOC_E_UNAVAIL       
 */
int
soc_mem_addr_to_array_element_and_index(int unit, soc_mem_t mem, uint32 address, 
                                        unsigned* array_index, int* index)
{
    int array_size_base;

    if((NULL == array_index) || (NULL == index)){
        return SOC_E_PARAM; 
    }

    /* address includes the offest of the memory | the entry index */
    if (soc_mem_is_valid(unit, mem) &&
            ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0) ) {
        if (!SOC_MEM_IS_ARRAY(unit, mem)) {
            if((address < (SOC_MEM_INFO(unit, mem).base + SOC_MEM_INFO(unit, mem).index_min)) ||
               (address > (SOC_MEM_INFO(unit, mem).base + SOC_MEM_INFO(unit, mem).index_max))) {
                return SOC_E_UNAVAIL;
            } else {
                *array_index = 0;
                *index = address - SOC_MEM_INFO(unit, mem).base;
                return SOC_E_NONE;
            }
        }  else {
            /* for arrays */        
            if ((address < SOC_MEM_INFO(unit, mem).base) || 
                           (address > (SOC_MEM_INFO(unit, mem).base + SOC_MEM_INFO(unit, mem).index_max +
                                       (SOC_MEM_NUMELS(unit, mem) - 1) * SOC_MEM_ELEM_SKIP(unit, mem)))) {
                return SOC_E_UNAVAIL;
            } else {
                array_size_base = SOC_MEM_ELEM_SKIP(unit, mem);
                *array_index = ((address - SOC_MEM_INFO(unit, mem).base) / array_size_base + SOC_MEM_ARRAY_INFO(unit, mem).first_array_index);
                *index = (address - SOC_MEM_INFO(unit, mem).base) % array_size_base;
                return SOC_E_NONE;
            }
        }
    }
    else {
        return SOC_E_UNAVAIL;
    }
}

/*
 * Function:     soc_mem_entry_bits
 * Purpose:      Get number of bits in entry
 * Returns:      Number of bits in the entry
 */
int
soc_mem_entry_bits(int unit, soc_mem_t mem)
{
    int                 f, end;
    soc_field_info_t    *fieldp;
    soc_mem_info_t      *memp;
    int                 numbits = 0;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    memp = &SOC_MEM_INFO(unit, mem);

    for (f = 0; f < memp->nFields; f++) {
        fieldp = &(memp->fields[f]);
        end = fieldp->bp + fieldp->len;
        if (end > numbits) {
            numbits = end;
        }
    }

    return numbits;
}

/*
 * Function:
 *     soc_mem_snoop_register
 * Purpose:
 *      Registers a snooping call back for specific memory.
 *      Call back will be called on Read or Write operations
 *      on the memory according to specified flags
 * Parameters:
 *      unit         -  (IN) BCM device number.
 *      mem          -  (IN) Memory to register a call back for.
 *      flags        -  (IN) SOC_MEM_SNOOP_XXX flags.
 *      snoop_cv     -  (IN) User provided call back, NULL for unregister
 *      user_data    -  (IN) user provided data to be passed to call back function
 * Returns:
 *      None
 */
void
soc_mem_snoop_register(int unit, soc_mem_t mem, uint32 flags,
                       soc_mem_snoop_cb_t snoop_cb, void *user_data)
{
    soc_mem_info_t      *mem_info_p;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    mem_info_p = &SOC_MEM_INFO(unit, mem);

    assert(NULL != snoop_cb);

    mem_info_p->snoop_cb = snoop_cb;
    mem_info_p->snoop_user_data = user_data;
    mem_info_p->snoop_flags |= flags;

    return;
}

/*
 * Function:
 *     soc_mem_snoop_unregister
 * Purpose:
 *      Unregisters a snooping call back with specific flag for specific memory.
 *      This function will not fail even if call back was not previously
 *      registered.
 * Parameters:
 *      unit         -  (IN) BCM device number.
 *      mem          -  (IN) Memory to register a call back for.
 *      flags        -  (IN) SOC_MEM_SNOOP_XXX flags.
 * Returns:
 *      None
 */
void
soc_mem_snoop_unregister(int unit, soc_mem_t mem, uint32 flags)
{
    soc_mem_info_t      *mem_info_p;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "mem %s is invalid\n"), soc_mem_name[mem]));
#endif
        assert(SOC_MEM_IS_VALID(unit, mem));
    }

    mem_info_p = &SOC_MEM_INFO(unit, mem);

    if (SOC_MEM_SNOOP_UNREGISTER & flags) {
        mem_info_p->snoop_cb = NULL;
        mem_info_p->snoop_user_data = NULL;
        mem_info_p->snoop_flags = 0;
    } else {
        mem_info_p->snoop_flags &= ~flags;
        if (mem_info_p->snoop_flags == 0) {
            mem_info_p->snoop_cb = NULL;
            mem_info_p->snoop_user_data = NULL;
        }
    }

    return;
}
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT || defined(PORTMOD_SUPPORT)*/

#ifdef BCM_ESW_SUPPORT
/* Resolve DESTINATIONf value to type and value
 * dest_type - OUT
 * value     - OUT
 */
void _soc_mem_dest_value_resolve(int unit, uint32 dest_value, uint32 *enum_type,
                                 uint32 *value)
{
    uint32 dest_type;

    /* Decode application enum type and destination type */
    *enum_type = SOC_MEM_FIF_DEST_INVALID;
    *value = 0;

    if ((dest_type = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DEST_TYPE0f)) != 0) {
        switch (dest_type) {
            case 3:
                *enum_type = SOC_MEM_FIF_DEST_NEXTHOP;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value,NEXT_HOP_INDEXf);
                break;
            case 2:
                *enum_type = SOC_MEM_FIF_DEST_DGPP;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DGPPf);
                break;
            case 1:
                *enum_type = SOC_MEM_FIF_DEST_DVP;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DVPf);
                break;
            default:
                break;
        }
    } else if ((dest_type = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DEST_TYPE1f)) != 0) {
        switch (dest_type) {
            case 3:
                *enum_type = SOC_MEM_FIF_DEST_IPMC;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, IPMC_GROUPf);
                break;
            case 2:
                *enum_type = SOC_MEM_FIF_DEST_L2MC;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, L2MC_GROUPf);
                break;
            case 1:
                *enum_type = SOC_MEM_FIF_DEST_ECMP;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, ECMP_GROUPf);
                break;
            default:
                break;
        }
    } else if ((dest_type = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DEST_TYPE2f)) != 0) {
        switch (dest_type) {
            case 1:
                *enum_type = SOC_MEM_FIF_DEST_LAG;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, LAGf);
                break;
            default:
                break;
       }
    } else if ((dest_type = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DEST_TYPE3f)) != 0) {
        switch (dest_type) {
            case 3:
                *enum_type = SOC_MEM_FIF_DEST_MYSTA;
                *value = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, MY_STATION_PROFILE_IDXf);
                break;
            default:
                break;
        }
    } else if ((dest_type = soc_format_field32_get(unit, DESTINATION_FORMATfmt, &dest_value, DEST_TYPE5f)) !=0 ) {
        switch (dest_type) {
            case 3:
                *enum_type = SOC_MEM_FIF_DEST_DISC_CP2CPU;
                *value = 0;
                break;
            case 2:
                *enum_type = SOC_MEM_FIF_DEST_DISC;
                *value = 0;
                break;
            default:
                break;
        }
    }

    return ;
}

/* Construct DESTINATIONf value with type and value
 * return - DESTINATIONf value
 */
uint32
_soc_mem_dest_value_construct(int unit, uint32 dest_type, uint32 value)
{
    soc_field_t  format_type_field = INVALIDf;
    uint32       format_type_field_value = 0;
    soc_field_t  format_value_field = INVALIDf;
    uint32       format_value_field_value = 0;
    uint32       dest_value = 0;

    switch (dest_type) {
        case SOC_MEM_FIF_DEST_NEXTHOP:
            format_type_field            = DEST_TYPE0f;
            format_type_field_value      = 3;
            format_value_field           = NEXT_HOP_INDEXf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_DGPP:
            format_type_field            = DEST_TYPE0f;
            format_type_field_value      = 2;
            format_value_field           = DGPPf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_DVP:
            format_type_field            = DEST_TYPE0f;
            format_type_field_value      = 1;
            format_value_field           = DVPf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_IPMC:
            format_type_field            = DEST_TYPE1f;
            format_type_field_value      = 3;
            format_value_field           = IPMC_GROUPf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_L2MC:
            format_type_field            = DEST_TYPE1f;
            format_type_field_value      = 2;
            format_value_field           = L2MC_GROUPf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_ECMP:
            format_type_field            = DEST_TYPE1f;
            format_type_field_value      = 1;
            format_value_field           = ECMP_GROUPf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_LAG:
            format_type_field            = DEST_TYPE2f;
            format_type_field_value      = 1;
            format_value_field           = LAGf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_MYSTA:
            format_type_field            = DEST_TYPE3f;
            format_type_field_value      = 3;
            format_value_field           = MY_STATION_PROFILE_IDXf;
            format_value_field_value     = value;
            break;
        case SOC_MEM_FIF_DEST_DISC_CP2CPU:
            format_type_field            = DEST_TYPE5f;
            format_type_field_value      = 3;
            break;
        case SOC_MEM_FIF_DEST_DISC:
            format_type_field            = DEST_TYPE5f;
            format_type_field_value      = 2;
            break;
        default:
            dest_value = 0;
            break;
    }

    /* Convert field data into format type */
    if (format_type_field != INVALIDf) {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt, &dest_value,  format_type_field,  format_type_field_value);
    }

    if (format_value_field != INVALIDf) {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt, &dest_value,  format_value_field,  format_value_field_value);
    }

    return (dest_value);
}

/*
 * soc_mem_field32_dest_set
 *
 * Based on application enum type, covert configuration value
 * to destination format and write it to hardware memory.
 */
void
soc_mem_field32_dest_set(int unit, soc_mem_t mem, void *entbuf,
                         soc_field_t fld, uint32 dest_type, uint32 value)
{
    uint32       dest_value = 0;

    /* Convert value into destination format */
    dest_value = _soc_mem_dest_value_construct(unit, dest_type, value);

    /* Write data (in format structure) to memory buffer */
    soc_mem_field32_set(unit, mem, entbuf,  fld, dest_value);

    return;
}

/*
 * soc_mem_field32_dest_get
 *
 * Derive application enum type and destination value from hardware readout
 * based on destination format.
 */
uint32
soc_mem_field32_dest_get(int unit, soc_mem_t mem, const void *entbuf,
                         soc_field_t fld, uint32 *enum_type)
{
    uint32 dest_value;
    uint32 value = 0;

    /* Hardware memory readout */
    dest_value = soc_mem_field32_get(unit, mem, entbuf, fld);

    /* Decode readout from destination format */
    _soc_mem_dest_value_resolve(unit, dest_value, enum_type, &value);

    return value;
}
#endif /* End of BCM_ESW_SUPPORT */
