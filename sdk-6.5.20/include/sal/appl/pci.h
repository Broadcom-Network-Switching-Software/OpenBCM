/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PCI Local Bus: constants, structures, and SAL procedure calls
 */

#ifndef	_SAL_PCI_H
#define	_SAL_PCI_H

#include <sal/types.h>

/*
 * PCI config space registers
 */
#define PCI_CONF_VENDOR_ID              0x00     /* Word @ Offset 0x00 */
#define PCI_CONF_DEVICE_ID              0x02     /* Word @ Offset 0x02 */

#define PCI_CONF_COMMAND                0x04     /* Word @ Offset 0x04 */

#define PCI_CONF_COMMAND_IO             (1 << 0)
#define PCI_CONF_COMMAND_MS             (1 << 1)
#define PCI_CONF_COMMAND_BM             (1 << 2)
#define PCI_CONF_COMMAND_SC             (1 << 3)
#define PCI_CONF_COMMAND_MWIE           (1 << 4)
#define PCI_CONF_COMMAND_VGA_PS         (1 << 5)
#define PCI_CONF_COMMAND_PERR           (1 << 6)
#define PCI_CONF_COMMAND_WCC            (1 << 7)
#define PCI_CONF_COMMAND_SERR           (1 << 8)
#define PCI_CONF_COMMAND_FBBE           (1 << 9)

#define PCI_CONF_STATUS                 0x06     /* Word @ Offset 0x06 */

#define PCI_CONF_STATUS_66MHZ          (1 << 5)
#define PCI_CONF_STATUS_UDF            (1 << 6)
#define PCI_CONF_STATUS_FBBC           (1 << 7)
#define PCI_CONF_STATUS_DPED           (1 << 8)
#define PCI_CONF_STATUS_DEVSEL         (3 << 9)
#define PCI_CONF_STATUS_STA            (1 << 11)
#define PCI_CONF_STATUS_RTA            (1 << 12)
#define PCI_CONF_STATUS_RMA            (1 << 13)
#define PCI_CONF_STATUS_SMA            (1 << 14)
#define PCI_CONF_STATUS_DPE            (1 << 15)

#define PCI_CONF_REVISION_ID            0x08     /* Byte @ Offset 0x08 */
#define PCI_CONF_CC_RPI                 0x09     /* Byte @ Offset 0x09 */
#define PCI_CONF_CC_SUB                 0x0A     /* Byte @ Offset 0x0A */
#define PCI_CONF_CC_BASE                0x0B     /* Byte @ Offset 0x0B */

#define PCI_CONF_CACHE_LINE_SIZE        0x0C     /* Byte @ Offset 0x0C */
#define PCI_CONF_LATENCY_TIMER          0x0D     /* Byte @ Offset 0x0D */
#define PCI_CONF_HDR_TYPE               0x0E     /* Byte @ Offset 0x0E */
#define PCI_CONF_BIST                   0x0F     /* Byte @ Offset 0x0F */

#define PCI_CONF_BAR0                   0x10     /* DWORD @ Offset 0x10 */
#define PCI_CONF_BAR1                   0x14     /* DWORD @ Offset 0x14 */
#define PCI_CONF_BAR2                   0x18     /* DWORD @ Offset 0x18 */
#define PCI_CONF_BAR3                   0x1C     /* DWORD @ Offset 0x1C */
#define PCI_CONF_BAR4                   0x20     /* DWORD @ Offset 0x20 */
#define PCI_CONF_BAR5                   0x24     /* DWORD @ Offset 0x24 */

#define PCI_CONF_BAR_IO_SPACE           (1 << 0)

#define PCI_CONF_BAR_MASK               0xFFFFFFF0
#define PCI_CONF_BAR_TYPE_MASK          (3 << 1)
#define PCI_CONF_BAR_TYPE_32B           (0 << 1)
#define PCI_CONF_BAR_TYPE_1MB           (1 << 1)
#define PCI_CONF_BAR_TYPE_64B           (2 << 1)
#define PCI_CONF_BAR_TYPE_RSV           (3 << 1)

#define PCI_CONF_BAR_PREFETCHABLE       (1 << 3)

#define PCI_CONF_CB_CIS_PTR             0x28     /* DWORD @ Offset 0x28 */

#define PCI_CONF_SUBSYS_VENDOR_ID       0x2C     /* Word @ Offset 0x2C */
#define PCI_CONF_SUBSYS_ID              0x2E     /* Word @ Offset 0x2E */

#define PCI_CONF_EXP_ROM                0x30     /* DWORD @ Offset 0x30 */

#define PCI_CONF_CAPABILITY_PTR         0x34     /* DWORD @ Offset 0x34 */

#define PCI_CONF_INTERRUPT_LINE         0x3C     /* Byte @ Offset 0x3C */
#define PCI_CONF_INTERRUPT_PIN          0x3D     /* Byte @ Offset 0x3D */
#define PCI_CONF_MIN_GRANT              0x3E     /* Byte @ Offset 0x3E */
#define PCI_CONF_MAX_LATENCY            0x3F     /* Byte @ Offset 0x3F */

#define	PCI_CONF_TRDY_TO	        0x40	 /* Byte @ Offset 0x40 */
#define	PCI_CONF_RETRY_CNT	        0x41	 /* Byte @ Offset 0x41 */
#define PCI_CONF_PLL_BYPASS             0x44     /* Byte @ Offset 0x42 */

#define PCI_CONF_SOCCFG0                0x44     /* DWORD @ Offset 0x44 */
#define PCI_CONF_SOCCFG1                0x48     /* DWORD @ Offset 0x48 */

#define PCI_CONF_EXT_CAPABILITY_PTR         0x100     /* DWORD @ Offset 0x100 */

/*
 * PCI_CONF_INTERRUPT_PIN
 */
#define PCI_INTERRUPT_A                 1
#define PCI_INTERRUPT_B                 2
#define PCI_INTERRUPT_C                 3
#define PCI_INTERRUPT_D                 4

/*
 * StrataSwitch SAL Definitions
 */

#define PCI_NETWORK_CLASS_ID	0x28000

#define PCI_SOC_MEM_WINSZ	0x10000

#ifdef MOUSSE
/* NOTE: Keep these in sync with config.h and sysLib.c in BSPs */
# define PCI_SOC_MBAR0		0x81000000
# define PCI_SOC_INTLINE(unit)	2		/* MOUSSE_IRQ_CPCI */
extern STATUS pciIntConnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine,        /* routine to be called              */
    int parameter               /* parameter to be passed to routine */
    );
#endif

#ifdef IDT438
# define PCI_SOC_MBAR0		0xe1000000
# define PCI_SOC_INTLINE(unit)	1
#endif

#ifdef IDTRP334
# define PCI_SOC_MBAR0		0x41000000
# define PCI_SOC_INTLINE(unit)	1
#endif

#ifdef MBZ
# define PCI_SOC_MBAR0		0xa8100000
# define PCI_SOC_INTLINE(unit)	4
#endif

#if defined(NSX)
# define PCI_SOC_MBAR0                0x41000000
# define PCI_SOC_INTLINE(unit)        0x38  /* 0x3a */
#endif

#if defined(METROCORE)
# define PCI_SOC_MBAR0                0x41000000
# define PCI_SOC_INTLINE(unit)        0x38  /* 0x3a */
#endif

#ifdef PLISIM
# define PCI_SOC_MBAR0		0xcd600000	/* Arbitrary value */
# define PCI_SOC_INTLINE(unit)	(unit)
#endif /* PLISIM */

#ifdef MPC8548
#define PCI_SOC_MBAR0           0x50000000
#endif

/* Default PCI_SOC_MBAR() */
#if !defined(PCI_SOC_MBAR) && defined(PCI_SOC_MBAR0)
#define PCI_SOC_MBAR(unit)	(PCI_SOC_MBAR0 + (unit) * PCI_SOC_MEM_WINSZ)
#endif


typedef void (*pci_isr_t)(void *isr_data);

typedef struct pci_dev_s {
    int		busNo;
    int		devNo;
    int		funcNo;
} pci_dev_t;

extern int pci_config_putw(pci_dev_t *, uint32 addr, uint32 data);
extern uint32 pci_config_getw(pci_dev_t *, uint32 addr);

extern int pci_memory_putw(pci_dev_t *, uint32 addr, uint32 data);
extern uint32 pci_memory_getw(pci_dev_t *, uint32 addr);

extern int pci_device_iter(int (*rtn)(pci_dev_t *dev,
				   uint16 pciVenID,
				   uint16 pciDevID,
				   uint8 pciRevID));

extern void pci_print_all(void);

extern void pci_print_config(pci_dev_t *);

extern int pci_int_connect(int intLine, pci_isr_t isr, void *isr_data);

#ifdef SINGLE_MODE
extern int local_config_putw(pci_dev_t *dev, uint32 addr, uint32 data);
extern uint32 local_config_getw(pci_dev_t *dev, uint32 addr);

extern int local_memory_putw(pci_dev_t *dev, uint32 addr, uint32 data);
extern uint32 local_memory_getw(pci_dev_t *dev, uint32 addr);

extern int local_int_connect(int intLine, pci_isr_t isr, void *isr_data);
#endif /* !SINGLE_MODE*/

#endif	/* !_SAL_PCI_H */
