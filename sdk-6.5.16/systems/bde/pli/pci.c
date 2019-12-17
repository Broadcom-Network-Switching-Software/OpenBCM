/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * PCI memory and configuration space routines.
 *
 * Under Solaris, these routines call the verinet model PLI simulation.
 * All PCI operations are performed talking over a socket to the model.
 *
 * NOTE: the base address used in this file is opaque. This means that
 * the PCI device must first have its base-address programmed by
 * writing the start offset to PCI configuration offset 0x10.
 * Writes to the memory space afterwards requires the address to be
 * based on the base-address plus the offset to the desire register
 * to be accessed.
 */

#ifdef LINUX_PLI_COMBO_BDE
/* Avoid name clash when using two BDEs */
#define pci_config_getw pli_pci_config_getw
#endif

#include <shared/bsl.h>

#include <assert.h>

#include <sal/appl/io.h>
#include <sal/appl/pci.h>

#include "verinet.h"

#ifdef SINGLE_MODE
#define _SOCKLEN_T
#define __socklen_t_defined
#include <sim/pcid/pli.h>
extern pcid_info_t pcid_info;
#endif

/*
 * NOTE:  Assume dev->devNo is the verinet unit (for now always zero).
 */

/* In the DV model for the RTL, bit 12 selects Orion */
#define IDSEL 0x1000

#define VALID_DEV(dev) \
	((dev)->busNo == 0 && \
         (dev)->devNo < pli_client_count() && \
         (dev)->funcNo == 0)

/*
 * Initialize the package if not already done
 */

int pci_init_check(void)
{
    static int	 	pli_connected;
    int			client;

    if (! pli_connected) {
	/*
	 * Initialize each PLI client.
	 */

	for (client = 0; client < pli_client_count(); client++) {
	    if (pli_client_attach(client) < 0) {
		cli_out("pci_init_check: error attaching PLI client %d\n",
                        client);
		return -1;
	    }
	}

	pli_connected = 1;
    }

    return 0;
}


/*
 * Write a DWORD (32 bits) of data to PCI configuration space
 * at the specified offset.
 */

int pci_config_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    if(pci_init_check() < 0)
        return -1;

    assert(! (addr & 3));

    if (VALID_DEV(dev))
	return pli_setreg(dev->devNo, PCI_CONFIG, addr | IDSEL, data);
    else
	return -1;
}

/*
 * Read a DWORD (32 bits) of data from PCI configuration space
 * at the specified offset.
 */

uint32 pci_config_getw(pci_dev_t *dev, uint32 addr)
{
    if (pci_init_check() < 0)
	return 0xffffffff;

    assert(! (addr & 3));

    if (VALID_DEV(dev))
	return pli_getreg(dev->devNo, PCI_CONFIG, addr | IDSEL);
    else
	return 0xffffffff;
}

/*
 * Write a DWORD (32 bits) of data to PCI memory space
 * at the specified offset. Return last value at address.
 */

int pci_memory_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    if (pci_init_check() < 0)
	return -1;

    return pli_setreg(dev->devNo, PCI_MEMORY, addr, data);
}

/*
 * Read a DWORD (32 bits) of data from PCI memory space.
 */

uint32 pci_memory_getw(pci_dev_t *dev, uint32 addr)
{
    if (pci_init_check() < 0)
	return 0xffffffff;

    return pli_getreg(dev->devNo, PCI_MEMORY, addr);
}

/*
 * pci_int_connect
 *
 *    For PLI simulation, the ISR has already been attached.
 */

int pci_int_connect(int intLine,
		    pci_isr_t isr,
		    void *isr_data)
{
    if (pci_init_check() < 0)
	return -1;

    return pli_register_isr(intLine, isr, isr_data);
}

#ifdef SINGLE_MODE
/********************************
***** SINGLE MODE FUNCTIONS
 */
/*
 * The following function are equivalent to the above function. 
 * They used when using single mode (not going through the net). 
 */

int local_config_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    assert(!(addr & 3)); 

    if (VALID_DEV(dev))
        return pli_setreg_service(&pcid_info, pcid_info.unit, PCI_CONFIG, addr | IDSEL, data);
    else
        return -1;
}

uint32 local_config_getw(pci_dev_t *dev, uint32 addr)
{
    assert(! (addr & 3));

    if (VALID_DEV(dev))
        return pli_getreg_service(&pcid_info, pcid_info.unit, PCI_CONFIG, addr | IDSEL);
    else
        return 0xffffffff;
}

int local_memory_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    return pli_setreg_service(&pcid_info, pcid_info.unit, PCI_MEMORY, addr, data);
}

uint32 local_memory_getw(pci_dev_t *dev, uint32 addr)
{
    return pli_getreg_service(&pcid_info, pcid_info.unit, PCI_MEMORY, addr);
}

int local_int_connect(int intLine,
		    pci_isr_t isr,
		    void *isr_data)
{
    return pli_register_isr(intLine, isr, isr_data);
}
#endif /* !SINGLE_MODE*/
