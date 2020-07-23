/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        util.c
 *
 * Provides:
 *     pcid_x2i
 *     pcid_type_name
 *
 * Requires: types and memory type enumeration from verinet.h
 */

#include <sys/types.h>
#include <soc/types.h>
#include <bde/pli/verinet.h>
#include <string.h>

int
pcid_x2i(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}


/*
 * type_name
 */

void pcid_type_name(char *buffer, uint32 type)
{
    switch (type) {
    case PCI_CONFIG:
    /* coverity[secure_coding] */
	strcpy(buffer, "PCI CONFIG");
	break;
    case PCI_MEMORY:
    /* coverity[secure_coding] */
	strcpy(buffer, "PCI MEMORY");
	break;
    case I2C_CONFIG:
    /* coverity[secure_coding] */
	strcpy(buffer, "I2C CONFIG");
	break;
    case PLI_CONFIG:
    /* coverity[secure_coding] */
	strcpy(buffer, "PLI CONFIG");
	break;
    case JTAG_CONFIG:
    /* coverity[secure_coding] */
	strcpy(buffer, "JTAG");
	break;
    default:
    /* coverity[secure_coding] */
	strcpy(buffer, "UNKNOWN");
	break;
    }
}
