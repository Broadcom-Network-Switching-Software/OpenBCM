/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * format field manipulations.
 */

#ifndef _SOC_FORMAT_H
#define _SOC_FORMAT_H

#include <soc/defs.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(PORTMOD_SUPPORT)
#include <soc/mcm/allenum.h>
#endif

#include <soc/field.h>


/*************************************************************
 * Function:     soc_format_field_length
 * Purpose:      Return the length of a format field in bits.
 *               Value is 0 if field is not found.
 * Returns:      bits in field
 */
int
soc_format_field_length(int unit, soc_format_t format, soc_field_t field);

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
soc_format_field_valid(int unit, soc_format_t format, soc_field_t field);

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
                  soc_field_t field, uint32 *fldbuf);

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
                      soc_field_t field, uint32 *fldbuf);
/***********************************************************
 * Function:     soc_format_field32_get
 * Purpose:      Get a <=32 bit field out of a format entry
 * Returns:      The value of the field
 */
uint32
soc_format_field32_get(int unit, soc_format_t format, const void *entbuf,
                    soc_field_t field);

/***********************************************************
 * Function:     soc_format_field32_set
 * Purpose:      Set a <=32 bit field out of a format entry
 * Returns:      void
 */
void
soc_format_field32_set(int unit, soc_format_t format, void *entbuf,
                    soc_field_t field, uint32 value);

#if defined(BCM_ESW_SUPPORT)
#if !defined(SOC_NO_NAMES)
extern char *soc_format_name[];
#define SOC_FORMAT_NAME(unit, format)		soc_format_name[format]
#else
#define SOC_FORMAT_NAME(unit, format)		""
#endif
#endif

/***********************************************************
 * DOP_ID => FORMAT Functionality
 */
int
soc_dop_format_get(int unit, uint16 dop_id, uint16 dop_block,
                    uint16 *n_data_phases, soc_format_t *format);

/********************************************************************
 * Function:      _soc_format_fieldinfo_get
 * Purpose:       Get a format field info without reference to chip.
 * Parameters:    unit - device
 *                fmt  - format
 *                field - Field in format
 * Returns        Reference to field info format               
 */
soc_field_info_t *
soc_format_fieldinfo_get(int unit, soc_format_t fmt, soc_field_t field);

/********************************************************************
 * Function:      soc_format_entry_dump
 * Purpose:       Dump the format entry.
 * Parameters:    unit - device
 *                fmt  - format
 *                entbuf - format entry buffer
 * Returns:       voiod
 */
void
soc_format_entry_dump(int unit, soc_format_t format, const uint32 *entbuf);
#endif
