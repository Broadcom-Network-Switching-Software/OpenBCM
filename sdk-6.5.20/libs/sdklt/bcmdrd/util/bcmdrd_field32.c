/*! \file bcmdrd_field32.c
 *
 * Utility functions used for extracting field values from
 * registers and memories.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <bcmdrd/bcmdrd_field.h>

/*
 * Function:
 *	bcmdrd_field32_get
 * Purpose:
 *	Extract field value from multi-word register/memory.
 * Parameters:
 *	entbuf - current contents of register/memory (word array)
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 * Returns:
 *      Extracted field value.
 */
uint32_t
bcmdrd_field32_get(const uint32_t *entbuf, int sbit, int ebit)
{
    uint32_t fval;

    /* coverity[address_of] */
    bcmdrd_field_get(entbuf, sbit, ebit, &fval);

    return fval;
}

/*
 * Function:
 *	bcmdrd_field32_set
 * Purpose:
 *	Assign field value in multi-word register/memory.
 * Parameters:
 *	entbuf - current contents of register/memory (word array)
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 * Returns:
 *      Nothing.
 */
void
bcmdrd_field32_set(uint32_t *entbuf, int sbit, int ebit, uint32_t fval)
{
    /* coverity[address_of] */
    bcmdrd_field_set(entbuf, sbit, ebit, &fval);
}
