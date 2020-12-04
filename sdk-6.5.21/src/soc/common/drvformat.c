/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * format field manipulations.
 */

#include <shared/bsl.h>

#include <assert.h>

#include <sal/core/libc.h>

#include <soc/drv.h>

#include <soc/mem.h>
#include <soc/memory.h>
#include <soc/format.h>

#include <soc/error.h>

#if defined(BCM_ESW_SUPPORT)

/*
 * Macro used by memory accessor functions to fix order
 */
#define FIX_FORMAT_ORDER_E(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? \
                                BITS2WORDS((m)->bits)-1-(v) : \
                                (v))

/*************************************************************
 * Function:     soc_format_field_length
 * Purpose:      Return the length of a format field in bits.
 *               Value is 0 if field is not found.
 * Returns:      bits in field
 */
int
soc_format_field_length(int unit, soc_format_t format, soc_field_t field)
{
    soc_field_info_t    *fld;

    SOC_FIND_FIELD(field,
                   SOC_FORMAT_INFO(unit, format).fields,
                   SOC_FORMAT_INFO(unit, format).nFields,
                   fld);
    if (fld == NULL) {
        return 0;
    }
    return fld->len;
}

/********************************************************************
 * Function:      _soc_formatinfo_field_get
 * Purpose:       Get a format field without reference to chip.
 * Parameters:    formatinfo   --  direct reference to format
 */
STATIC uint32 *
_soc_formatinfo_field_get(soc_format_t format, soc_format_info_t *formatinfo,
                      const uint32 *entbuf, soc_field_t field, uint32 *fldbuf,
                      uint32 fldbuf_size) /* The number of uint32s in fldbuf */
{
    soc_field_info_t    *fieldinfo;
    int                 i, wp, bp, len;

    SOC_FIND_FIELD(field,
                   formatinfo->fields,
                   formatinfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("format %s field %s is invalid\n"),
                 soc_format_name[format], soc_fieldnames[field]));
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
    assert(len / 32 <= fldbuf_size); /* assert we do not write beyond
                                      * the end of the output buffer */

    if (len == 1) {     /* special case single bits */
        wp = bp / 32;
        bp = bp & (32 - 1);
        if (entbuf[FIX_FORMAT_ORDER_E(wp, formatinfo)] & (1<<bp)) {
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
                    entbuf[FIX_FORMAT_ORDER_E(wp++, formatinfo)] >> bp &
                    ((1 << (32 - bp)) - 1);
                if ( len > (32 - bp) ) {
                    fldbuf[i] |= entbuf[FIX_FORMAT_ORDER_E(wp, formatinfo)] <<
                        (32 - bp);
                }
            } else {
                fldbuf[i] = entbuf[FIX_FORMAT_ORDER_E(wp++, formatinfo)];
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
                    ((entbuf[FIX_FORMAT_ORDER_E(bp / 32, formatinfo)] >>
                      (bp & (32 - 1))) & 1);
                len--;
                bp++;
            } while (len & (32 - 1));

            i--;
        }
    }

    return fldbuf;
}

/********************************************************************
 * Function:      _soc_format_field_value_fit
 * Purpose:       Check if value will fit into a format field.
 * Parameters:    fieldinfo --  (IN)Direct reference to field description.
 *                value     --  (IN)Value to be checked.
 * Returns:
 *      TRUE  - buffer fits into the field.
 *      FALSE - Otherwise.
 */
STATIC int
_soc_format_field_value_fit(soc_field_info_t *fieldinfo, uint32 *value)
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

/************************************************************************
 * Function:      _soc_formatinfo_field_set
 * Purpose:       Set a format field without reference to chip.
 * Parameters:    formatinfo   --  direct reference format
 */
STATIC void
_soc_formatinfo_field_set(soc_format_t format, soc_format_info_t *formatinfo,
                    uint32 *entbuf, soc_field_t field, uint32 *fldbuf)
{
    soc_field_info_t    *fieldinfo;
    uint32              mask;
    int                 i, wp, bp, len;

    SOC_FIND_FIELD(field,
                   formatinfo->fields,
                   formatinfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("format %s field %s is invalid\n"),
                 soc_format_name[format], soc_fieldnames[field]));
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
    if (!_soc_format_field_value_fit(fieldinfo, fldbuf)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("format %s field %s value does not fit\n"),
                 soc_format_name[format], soc_fieldnames[field]));
#endif
        assert(_soc_format_field_value_fit(fieldinfo, fldbuf));
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

                entbuf[FIX_FORMAT_ORDER_E(wp, formatinfo)] &= ~(mask << bp);
                entbuf[FIX_FORMAT_ORDER_E(wp++, formatinfo)] |= fldbuf[i] << bp;
                if (len > (32 - bp)) {
                    entbuf[FIX_FORMAT_ORDER_E(wp, formatinfo)] &=
                                                  ~(mask >> (32 - bp));
                    entbuf[FIX_FORMAT_ORDER_E(wp, formatinfo)] |=
                        fldbuf[i] >> (32 - bp) & ((1 << bp) - 1);
                }
            } else {
                if (len < 32) {
                    mask = (1 << len) - 1;
                    entbuf[FIX_FORMAT_ORDER_E(wp, formatinfo)] &= ~mask;
                    entbuf[FIX_FORMAT_ORDER_E(wp++, formatinfo)] |=
                                                         fldbuf[i] << bp;
                } else {
                    entbuf[FIX_FORMAT_ORDER_E(wp++, formatinfo)] = fldbuf[i];
                }
            }

            i++;
        }
    } else {                           /* Big endian: swap bits */
        len = fieldinfo->len;

        while (len > 0) {
            len--;
            entbuf[FIX_FORMAT_ORDER_E(bp / 32, formatinfo)] &=
                                                   ~(1 << (bp & (32-1)));
            entbuf[FIX_FORMAT_ORDER_E(bp / 32, formatinfo)] |=
                (fldbuf[len / 32] >> (len & (32-1)) & 1) << (bp & (32-1));
            bp++;
        }
    }
}
/***************************************************************
 * Function:
 *      soc_format_field_valid
 * Purpose:
 *      Verify if field is valid & present in format
 * Parameters:
 *      format     - (IN)Format id.
 *      field   - (IN)Field id.
 * Return:
 *      TRUE  -If field is present & valid.
 *      FALSE -Otherwise.
 */
int
soc_format_field_valid(int unit, soc_format_t format, soc_field_t field)
{
    soc_format_info_t  *formatinfo;
    soc_field_info_t    *finfop; /* Field information structure. */

    /* Verify that memory is present on the device. */
    if (!SOC_FORMAT_IS_VALID(unit, format))  {
        return FALSE;
    }

    formatinfo = &SOC_FORMAT_INFO(unit, format);
    SOC_FIND_FIELD(field,
                   formatinfo->fields,
                   formatinfo->nFields,
                   finfop);
    return (finfop != NULL) ? TRUE : FALSE;
}

/****************************************************
 * Function:    soc_format_field_get
 * Purpose:     Get a format field
 * Parameters:  unit - device
 *              format
 *              entbuf - format entry buffer
 *              field - which field to get
 *              fldbuf - field buffer
 */
uint32 *
soc_format_field_get(int unit, soc_format_t format, const uint32 *entbuf,
                  soc_field_t field, uint32 *fldbuf)
{
    soc_format_info_t *formatinfo;

    if (!SOC_FORMAT_IS_VALID(unit, format)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "format %s is invalid\n"), soc_format_name[format]));
#endif
        assert(SOC_FORMAT_IS_VALID(unit, format));
    }

    formatinfo = &SOC_FORMAT_INFO(unit, format);

    return _soc_formatinfo_field_get(format, formatinfo, entbuf,
                                   field, fldbuf, SOC_MAX_FORMAT_WORDS);
}

/********************************************************
 * Function:    soc_format_field_set
 * Purpose:     Set a format field
 * Parameters:  unit - device
 *              format
 *              entbuf - format entry buffer
 *              field - which field to set
 *              fldbuf - field buffer
 */
void
soc_format_field_set(int unit, soc_format_t format, uint32 *entbuf,
                      soc_field_t field, uint32 *fldbuf)
{
    soc_format_info_t *formatinfo;

    if (!SOC_FORMAT_IS_VALID(unit, format)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "format %s is invalid\n"),
                                    soc_format_name[format]));
#endif
        assert(SOC_FORMAT_IS_VALID(unit, format));
    }

    formatinfo = &SOC_FORMAT_INFO(unit, format);

    _soc_formatinfo_field_set(format, formatinfo, entbuf, field, fldbuf);
}

/***********************************************************
 * Function:     soc_format_field32_get
 * Purpose:      Get a <=32 bit field out of a format entry
 * Returns:      The value of the field
 */
uint32
soc_format_field32_get(int unit, soc_format_t format, const void *entbuf,
                    soc_field_t field)
{
    uint32              value;

    if (!SOC_FORMAT_IS_VALID(unit, format)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit, "format %s is invalid\n"),
                                  soc_format_name[format]));
#endif
        assert(SOC_FORMAT_IS_VALID(unit, format));
    }
    _soc_formatinfo_field_get(format, &SOC_FORMAT_INFO(unit, format),
                             entbuf, field, &value, 1);

    return value;
}

/***********************************************************
 * Function:     soc_format_field32_set
 * Purpose:      Set a <=32 bit field out of a format entry
 * Returns:      void
 */
void
soc_format_field32_set(int unit, soc_format_t format, void *entbuf,
                    soc_field_t field, uint32 value)
{
    /*
     * COVERITY
     *
     * We do this intentionally to use the generic function
     * soc_format_field_set() which does the necessary handling.
 */
    /* coverity[callee_ptr_arith : FALSE] */
    soc_format_field_set(unit, format, entbuf, field, &value);
}

/***********************************************************
 * Function:     soc_dop_format_get
 * Purpose:      get the format corresponding to a dop_id
 * Inputs:       unit - device
 *               dop_id - 16 bit dop_id
 *               dop_block - block
 * Outputs:      format
 *               n_data_phase entry from the corresponding Format
 * Returns:      SOC_E_NONE if found
 *               SOC_E_NOT_FOUND if given DOP is not found
 *               SOC_E_UNAVAIL if DOP functionality is not available
 */
int
soc_dop_format_get(int unit, uint16 dop_id, uint16 dop_block, uint16 *n_data_phases, soc_format_t *format)
{
    uint32 dop_id_blk = 0;
    int i=0;
    soc_dop_t *myDop = SOC_DOP_INFO(unit);

    if (NULL == myDop) {
        return SOC_E_UNAVAIL;
    }

    dop_id_blk = dop_id | (dop_block << 16);

    while (myDop[i].dop_id != 0)
    {
        if (myDop[i].dop_id == dop_id_blk)
        {
            *n_data_phases = myDop[i].n_data_phases;
            *format = myDop[i].dop_fmt;
            return SOC_E_NONE;
        }
        i++;
    }
    return SOC_E_NOT_FOUND;
}

/********************************************************************
 * Function:      _soc_format_fieldinfo_get
 * Purpose:       Get a format field info without reference to chip.
 * Parameters:    unit - device
 *                fmt  - format
 *                field - Field in format
 * Returns        Reference to field info format               
 */
soc_field_info_t *
soc_format_fieldinfo_get(int unit, soc_format_t fmt, soc_field_t field)
{
    soc_field_info_t    *fieldinfo;
    soc_format_info_t   *formatinfo;

    if (!SOC_FORMAT_IS_VALID(unit, fmt)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "format %s is invalid\n"), soc_format_name[fmt]));
#endif
        assert(SOC_FORMAT_IS_VALID(unit, fmt));
    }

    formatinfo = &SOC_FORMAT_INFO(unit, fmt);

    SOC_FIND_FIELD(field,
                   formatinfo->fields,
                   formatinfo->nFields,
                   fieldinfo);
    if (NULL == fieldinfo) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META("format %s field %s is invalid\n"),
                 soc_format_name[fmt], soc_fieldnames[field]));
#endif
        assert(fieldinfo);
    }

    return fieldinfo;
}

/********************************************************************
 * Function:      soc_format_entry_dump
 * Purpose:       Dump the format entry.
 * Parameters:    unit - device
 *                fmt  - format
 *                entbuf - format entry buffer
 * Returns:       voiod
 */
void
soc_format_entry_dump(int unit, soc_format_t format, const uint32 *entbuf)
{
    soc_format_info_t *formatinfo;
    soc_field_info_t *finfo;
    int f_idx;
    uint32 fbuf[SOC_MAX_MEM_FIELD_WORDS];
    char   tmp[512];

    if (!SOC_FORMAT_IS_VALID(unit, format)) {
        assert(SOC_FORMAT_IS_VALID(unit, format));
    }

    formatinfo = &SOC_FORMAT_INFO(unit, format);

    BSL_LOG(BSL_LSS_CLI, (BSL_META_U(unit,
                        "%s"), "<"));
    for (f_idx = 0;
         f_idx < formatinfo->nFields;
         f_idx++) {
        finfo = &formatinfo->fields[f_idx];
        soc_format_field_get(unit,
                   format,
                   entbuf,
                   finfo->field,
                   fbuf);
        sal_memset(tmp, 0, sizeof(tmp));
        _shr_format_long_integer(tmp, fbuf, BITS2BYTES(finfo->len));
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        BSL_LOG(BSL_LSS_CLI, (BSL_META_U(unit,
            "%s<%d:%d>=%s%s"), soc_fieldnames[finfo->field], finfo->bp, finfo->bp+finfo->len-1,
        tmp, ","));
#endif
    }
    BSL_LOG(BSL_LSS_CLI, (BSL_META_U(unit,
                  "%s\n"), ">"));
}
#endif
