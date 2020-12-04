/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SAL PCI abstraction common to all platforms
 * See also: unix/pci.c
 */
#if !defined(BCM_ICS) && !defined(__KERNEL__) && !defined(GHS)

#include <sal/types.h>
#include <sal/appl/io.h>
#include <sal/appl/pci.h>

#ifndef PCI_MAX_BUS
# ifdef PLISIM
#  define PCI_MAX_BUS		1
# else
#  define PCI_MAX_BUS		4
# endif
#endif

#ifndef PCI_MAX_DEV
#define PCI_MAX_DEV		32
#endif

#if defined(OVERRIDE_PCI_MAX_DEV)
#undef PCI_MAX_DEV
#define PCI_MAX_DEV             OVERRIDE_PCI_MAX_DEV
#endif

#if defined(OVERRIDE_PCI_MAX_BUS)
#undef PCI_MAX_BUS
#define PCI_MAX_BUS             OVERRIDE_PCI_MAX_BUS
#endif

#if defined(OVERRIDE_PCI_BUS_NO_START)
#define PCI_BUS_NO_START        OVERRIDE_PCI_BUS_NO_START
#else
#define PCI_BUS_NO_START        0
#endif

/*
 * For historical reasons the PCI probe function skips device 12
 * by default to prevent a system hang on certain platforms.
 * Override this value with zero to probe all PCI devices.
 */
#if defined(OVERRIDE_PCI_SKIP_DEV_MASK)
#define PCI_SKIP_DEV_MASK       OVERRIDE_PCI_SKIP_DEV_MASK
#else
#define PCI_SKIP_DEV_MASK       (1L << 12)
#endif

/*
 * pci_device_iter
 *
 *   Scans all PCI busses and slots.
 *   Calls a user-supplied routine once per PCI device found.
 *   Passes the device location (dev) and device IDs to the routine.
 *   Continues as long as the user routine returns 0.
 *   Otherwise, stops and returns the value from the user routine.
 */

int
pci_device_iter(int (*rtn)(pci_dev_t *dev,
			   uint16 pciVenID,
			   uint16 pciDevID,
			   uint8 pciRevID))
{
    uint32		venID, devID, revID;
    int			rv = 0;
    pci_dev_t		dev;

    sal_memset(&dev, 0, sizeof(pci_dev_t));

    for (dev.busNo = PCI_BUS_NO_START;
	 dev.busNo < PCI_MAX_BUS && rv == 0;
	 dev.busNo++) {

	for (dev.devNo = 0;
	     dev.devNo < PCI_MAX_DEV && rv == 0;
	     dev.devNo++) {

	    if ((1L << dev.devNo) & PCI_SKIP_DEV_MASK) {
		continue;
	    }

	    venID = pci_config_getw(&dev, PCI_CONF_VENDOR_ID) & 0xffff;

	    if (venID == 0xffff) {
		continue;
	    }

	    devID = pci_config_getw(&dev, PCI_CONF_VENDOR_ID) >> 16;
	    revID = pci_config_getw(&dev, PCI_CONF_REVISION_ID) & 0xff;

	    rv = (*rtn)(&dev, venID, devID, revID);
	}
    }

    return rv;
}

/*
 * Scan function 0 of all busses and devices, printing a line for each
 * device found.
 */

void
pci_print_all(void)
{
    int		dev_count = PCI_MAX_DEV;
    int		bus_count = PCI_MAX_BUS;
    pci_dev_t	dev;

    /* coverity[stack_use_callee_max : FALSE] */
    sal_memset(&dev, 0, sizeof(pci_dev_t));

    sal_printf("Scanning function 0 of PCI busses 0-%d, devices 0-%d\n",
		 bus_count - 1, dev_count - 1);
    sal_printf("bus dev fn venID devID class  rev MBAR0    MBAR1    MBAR2    MBAR3    MBAR4    MBAR5    IPIN ILINE\n");

    for (dev.busNo = PCI_BUS_NO_START; dev.busNo < bus_count; dev.busNo++)
	for (dev.devNo = 0; dev.devNo < dev_count; dev.devNo++) {
	    uint32		vendorID, deviceID, class, revID;
	    uint32		MBAR0, MBAR1, MBAR2, MBAR3, MBAR4, MBAR5, ipin, iline;

	    vendorID = (pci_config_getw(&dev, PCI_CONF_VENDOR_ID) & 0x0000ffff);

	    if (vendorID == 0xffff)
		continue;


#define CONFIG(offset)	pci_config_getw(&dev, (offset))

	    deviceID = (CONFIG(PCI_CONF_VENDOR_ID) & 0xffff0000) >> 16;
	    class    = (CONFIG(PCI_CONF_REVISION_ID) & 0xffffff00) >>  8;
	    revID    = (CONFIG(PCI_CONF_REVISION_ID) & 0x000000ff) >>  0;
	    MBAR0    = (CONFIG(PCI_CONF_BAR0) & 0xffffffff) >>  0;
	    MBAR1    = (CONFIG(PCI_CONF_BAR1) & 0xffffffff) >>  0;
	    MBAR2    = (CONFIG(PCI_CONF_BAR2) & 0xffffffff) >>  0;
	    MBAR3    = (CONFIG(PCI_CONF_BAR3) & 0xffffffff) >>  0;
	    MBAR4    = (CONFIG(PCI_CONF_BAR4) & 0xffffffff) >>  0;
	    MBAR5    = (CONFIG(PCI_CONF_BAR5) & 0xffffffff) >>  0;
	    iline    = (CONFIG(PCI_CONF_INTERRUPT_LINE) & 0x000000ff) >>  0;
	    ipin     = (CONFIG(PCI_CONF_INTERRUPT_LINE) & 0x0000ff00) >>  8;

#undef CONFIG

	    sal_printf("%02x  %02x  %02x %04x  %04x  "
		       "%06x %02x  %08x %08x %08x %08x %08x %08x %02x   %02x\n",
		       dev.busNo, dev.devNo, dev.funcNo,
		       vendorID, deviceID, class, revID,
		       MBAR0, MBAR1, MBAR2, MBAR3, MBAR4, MBAR5, ipin, iline);
	}
}

void
pci_print_config(pci_dev_t *dev)
{
    uint32		data;

    data = pci_config_getw(dev, PCI_CONF_VENDOR_ID);
    sal_printf("%04x: %08x  DeviceID=%04x  VendorID=%04x\n",
	       PCI_CONF_VENDOR_ID, data,
	       (data & 0xffff0000) >> 16,
	       (data & 0x0000ffff) >>  0);

    data = pci_config_getw(dev, PCI_CONF_COMMAND);
    sal_printf("%04x: %08x  Status=%04x  Command=%04x\n",
	       PCI_CONF_COMMAND, data,
	       (data & 0xffff0000) >> 16,
	       (data & 0x0000ffff) >>  0);

    data = pci_config_getw(dev, PCI_CONF_REVISION_ID);
    sal_printf("%04x: %08x  ClassCode=%06x  RevisionID=%02x\n", 
	       PCI_CONF_REVISION_ID, data,
	       (data & 0xffffff00) >> 8,
	       (data & 0x000000ff) >> 0);

    data = pci_config_getw(dev, PCI_CONF_CACHE_LINE_SIZE);
    sal_printf("%04x: %08x  BIST=%02x  HeaderType=%02x  "
	       "LatencyTimer=%02x  CacheLineSize=%02x\n",
	       PCI_CONF_CACHE_LINE_SIZE, data,
	       (data & 0xff000000) >> 24,
	       (data & 0x00ff0000) >> 16,
	       (data & 0x0000ff00) >>  8,
	       (data & 0x000000ff) >>  0);

    data = pci_config_getw(dev, PCI_CONF_BAR0);
    sal_printf("%04x: %08x  BaseAddress0=%08x\n",
	       PCI_CONF_BAR0, data, data);

    data = pci_config_getw(dev, PCI_CONF_BAR1);
    sal_printf("%04x: %08x  BaseAddress1=%08x\n",
	       PCI_CONF_BAR1, data, data);

    data = pci_config_getw(dev, PCI_CONF_BAR2);
    sal_printf("%04x: %08x  BaseAddress2=%08x\n",
	       PCI_CONF_BAR2, data, data);

    data = pci_config_getw(dev, PCI_CONF_BAR3);
    sal_printf("%04x: %08x  BaseAddress3=%08x\n",
	       PCI_CONF_BAR3, data, data);

    data = pci_config_getw(dev, PCI_CONF_BAR4);
    sal_printf("%04x: %08x  BaseAddress4=%08x\n",
	       PCI_CONF_BAR4, data, data);

    data = pci_config_getw(dev, PCI_CONF_BAR5);
    sal_printf("%04x: %08x  BaseAddress5=%08x\n",
	       PCI_CONF_BAR5, data, data);

    data = pci_config_getw(dev, PCI_CONF_CB_CIS_PTR);
    sal_printf("%04x: %08x  CardbusCISPointer=%08x\n",
	       PCI_CONF_CB_CIS_PTR, data, data);

    data = pci_config_getw(dev, PCI_CONF_SUBSYS_VENDOR_ID);
    sal_printf("%04x: %08x  SubsystemID=%02x  SubsystemVendorID=%02x\n",
	       PCI_CONF_SUBSYS_VENDOR_ID, data,
	       (data & 0xffff0000) >> 16,
	       (data & 0x0000ffff) >>  0);

    data = pci_config_getw(dev, PCI_CONF_EXP_ROM);
    sal_printf("%04x: %08x  ExpansionROMBaseAddress=%08x\n",
	       PCI_CONF_EXP_ROM, data, data);

    data = pci_config_getw(dev, 0x34);
    sal_printf("%04x: %08x  Reserved=%06x  CapabilitiesPointer=%02x\n",
	       0x34, data,
	       (data & 0xffffff00) >> 8,
	       (data & 0x000000ff) >> 0);

    data = pci_config_getw(dev, 0x38);
    sal_printf("%04x: %08x  Reserved=%08x\n",
	       0x38, data, data);

    data = pci_config_getw(dev, PCI_CONF_INTERRUPT_LINE);
    sal_printf("%04x: %08x  Max_Lat=%02x  Min_Gnt=%02x  "
	       "InterruptLine=%02x  InterruptPin=%02x\n",
	       PCI_CONF_INTERRUPT_LINE, data,
	       (data & 0xff000000) >> 24,
	       (data & 0x00ff0000) >> 16,
	       (data & 0x0000ff00) >>  8,
	       (data & 0x000000ff) >>  0);

    data = pci_config_getw(dev, 0x40);
    sal_printf("%04x: %08x  Reserved=%02x  "
	       "RetryTimeoutValue=%02x  TRDYTimeoutValue=%02x\n",
	       0x40, data,
	       (data & 0xffff0000) >> 16,
	       (data & 0x0000ff00) >>  8,
	       (data & 0x000000ff) >>  0);
}
#else
typedef int _sal_appl_pci_common_not_empty; /* Make ISO compilers happy. */
#endif /* !BCM_ICS && !LINUX && !GHS */
