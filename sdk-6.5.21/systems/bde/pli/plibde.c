/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef LINUX_PLI_COMBO_BDE
/* Avoid name clash when using two BDEs */
#define pci_config_getw pli_pci_config_getw
#endif

#ifdef LINUX_SAL_DMA_ALLOC_OVERRIDE
/* Must match name change in src/sal/core/unix/alloc.c */
#define sal_dma_alloc sal_sim_dma_alloc
#define sal_dma_free sal_sim_dma_free
#endif

#include <shared/bsl.h>

#include "plibde.h"
#include "plidev.h"
#include "verinet.h"

#include <sal/types.h>
#include <sal/appl/pci.h>
#include <sal/appl/sal.h>
#include <soc/devids.h>
#include <soc/defs.h>
#include <sal/appl/io.h>
#include <sal/appl/config.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/core/alloc.h>
#include <shared/util.h>
#include <stdio.h>

#ifdef SINGLE_MODE
#include <sim/pcid/pcid.h>

extern pcid_info_t pcid_info;
extern int _local_setup(pcid_info_t* pcid_info, char *chip_name, uint16 *dev_id, uint8 *rev_id);
#endif

/* 
 * PLI BDE
 *
 * Currently, the implementation is 
 * based directly on the old SAL architecture. 
 * As soon as the BDE migration is complete and tested, 
 * the old SAL architecture will be remove, 
 * and it will be implemented properly. 
 */
#define PLI_PCI_MBAR0        0x00000000
#define PLI_PCI_ILINE(dev)    (0 + (dev))

typedef struct vxbde_dev_s {
    ibde_dev_t bde_dev;
    pci_dev_t pci_dev;
    uint32 cmic_base;
} plibde_dev_t;

#if !defined(PLI_MAX_DEVICES)
#define PLI_MAX_DEVICES 16
#endif

static plibde_dev_t _devices[PLI_MAX_DEVICES];
static int _n_devices = 0;

STATIC const char*
_cmodel_bde_name(void)
{
    return "dpp-cmodel-bde";
}

STATIC int
_cmodel_bde_num_devices(int type)
{
    return _n_devices;
}

STATIC const ibde_dev_t*
_cmodel_bde_get_dev(int d)
{
    if(d < 0 || d > _n_devices) {
        return NULL;
    }

    return &_devices[d].bde_dev;
}

STATIC uint32
_cmodel_bde_get_dev_type(int d)
{
    if(d < 0 || d > _n_devices) {
        return 0;
    }
    return BDE_PCI_DEV_TYPE;
}

STATIC uint32
_cmodel_bde_pci_config_read(int d, uint32 addr)
{
    return (0xffffffff); /* not supported */
}

STATIC int
_cmodel_bde_pci_config_write(int d, uint32 addr, uint32 data)
{
    return (-1); /* not supported */
}

STATIC void
_cmodel_bde_pci_bus_features(int unit, int* be_pio, int* be_packet, int* be_other)
{
    *be_pio = 0;
    *be_packet = 0;
    *be_other = 0;
}


/*  function for mock access to device BARs (iproc and CMIC) */
STATIC uint32
_cmodel_bde_memory_read(int d, uint32 addr)
{
    return 0;
}

/*  function for mock access to device BARs (iproc and CMIC) */
STATIC int
_cmodel_bde_memory_write(int d, uint32 addr, uint32 data)
{
    return 0;
}

STATIC uint32*
_cmodel_bde_salloc(int d, int size, const char *name)
{
    return sal_dma_alloc(size, (char *)name);
}

STATIC void
_cmodel_bde_sfree(int d, void* ptr)
{
    sal_dma_free(ptr);
}

STATIC int
_cmodel_bde_sflush(int d, void* addr, int length)
{
    sal_dma_flush(addr, length);
    return 0;
}

STATIC int
_cmodel_bde_sinval(int d, void* addr, int length)
{
    sal_dma_inval(addr, length);
    return 0;
}

STATIC int
_cmodel_bde_interrupt_connect(int d, void (*isr)(void*), void* data)
{
    return 0;
}

STATIC int
_cmodel_bde_interrupt_disconnect(int d)
{
    return 0;
}

STATIC sal_paddr_t
_cmodel_bde_l2p(int d, void* laddr)
{
    return PTR_TO_INT(laddr);
}

STATIC void*
_cmodel_bde_p2l(int d, sal_paddr_t paddr)
{
    return INT_TO_PTR(paddr);
}

static ibde_t _cmodel_bde = {
    _cmodel_bde_name,
    _cmodel_bde_num_devices,
    _cmodel_bde_get_dev,
    _cmodel_bde_get_dev_type,
    _cmodel_bde_pci_config_read,
    _cmodel_bde_pci_config_write,
    _cmodel_bde_pci_bus_features,
    _cmodel_bde_memory_read,
    _cmodel_bde_memory_write,
    _cmodel_bde_salloc,
    _cmodel_bde_sfree,
    _cmodel_bde_sflush,
    _cmodel_bde_sinval,
    _cmodel_bde_interrupt_connect,
    _cmodel_bde_interrupt_disconnect,
    _cmodel_bde_l2p,
    _cmodel_bde_p2l,
    NULL,
    NULL,
    _cmodel_bde_memory_read,
    _cmodel_bde_memory_write
};

static const char*
_name(void)
{
    return "pli-pci-bde";
}

static int 
_num_devices(int type)
{
    switch (type) {
    case BDE_ALL_DEVICES:
    case BDE_SWITCH_DEVICES:
        return _n_devices; 
    case BDE_ETHER_DEVICES:
        return 0; 
    }

    return 0;
}

static const ibde_dev_t*
_get_dev(int d)
{
    if(d < 0 || d > _n_devices) {
        return NULL;
    }
    return &_devices[d].bde_dev;
}

static uint32
_get_dev_type(int d)
{
    return BDE_PCI_DEV_TYPE;
}


static uint32 
_pci_read(int d, uint32 addr)
{
    return  pci_config_getw(&_devices[d].pci_dev, addr);
}

static int
_pci_write(int d, uint32 addr, uint32 data)
{
    return pci_config_putw(&_devices[d].pci_dev, addr, data);
}

static void
_pci_bus_features(int unit, int* be_pio, int* be_packet, int* be_other)
{
#ifdef SYS_BE_PIO
    *be_pio = SYS_BE_PIO;
#else
    *be_pio = 0;
#endif
    
#ifdef SYS_BE_PACKET
    *be_packet = SYS_BE_PACKET;
#else
    *be_packet = (sal_boot_flags_get() & BOOT_F_RTLSIM) ? 1 : 0;
#endif

#ifdef SYS_BE_OTHER
    *be_other = SYS_BE_OTHER;
#else
    *be_other = 0;
#endif
}
  

static uint32  
_read(int d, uint32 addr)
{
    return pci_memory_getw(&_devices[d].pci_dev, 
               PLI_PCI_MBAR0 + _devices[d].cmic_base + addr); 
}

static int
_write(int d, uint32 addr, uint32 data)
{
    pci_memory_putw(&_devices[d].pci_dev, 
            PLI_PCI_MBAR0 + _devices[d].cmic_base + addr, data);
    return 0;
}

static uint32* 
_salloc(int d, int size, const char *name)
{
    return sal_dma_alloc(size, (char *)name);
}

static void
_sfree(int d, void* ptr)
{
    sal_dma_free(ptr);
}

static int 
_sflush(int d, void* addr, int length)
{
    sal_dma_flush(addr, length);
    return 0;
}

static int
_sinval(int d, void* addr, int length)
{
    sal_dma_inval(addr, length);
    return 0;
}

static int 
_interrupt_connect(int d, void (*isr)(void*), void* data)
{
    return pci_int_connect(PLI_PCI_ILINE(d), isr, data);
}
             
static int
_interrupt_disconnect(int d)
{
    return 0;
}
   
static sal_paddr_t
_l2p(int d, void* laddr)
{
    return (sal_paddr_t)laddr;
}

static void *
_p2l(int d, sal_paddr_t paddr)
{
    return (void *)paddr;
}

static uint32  
_iproc_read(int d, uint32 addr)
{
    return pci_memory_getw(&_devices[d].pci_dev, 
               PLI_PCI_MBAR0 + addr); 
}

static int
_iproc_write(int d, uint32 addr, uint32 data)
{
    pci_memory_putw(&_devices[d].pci_dev, 
            PLI_PCI_MBAR0 + addr, data);
    return 0;
}


static ibde_t _ibde = {
    _name, 
    _num_devices, 
    _get_dev, 
    _get_dev_type, 
    _pci_read,
    _pci_write,
    _pci_bus_features,
    _read,
    _write,
    _salloc,
    _sfree,
    _sflush,
    _sinval,
    _interrupt_connect,
    _interrupt_disconnect,
    _l2p,
    _p2l,
    NULL, /* spi_read */
    NULL, /* spi_write */
    _iproc_read,
    _iproc_write,
};

static int
_setup(pci_dev_t* dev, 
       uint16 pciVenID, 
       uint16 pciDevID,
       uint8 pciRevID)
{
    plibde_dev_t*       vxd;
    uint32              tmp;
    int                 devno;

    if (!plidev_supported(pciVenID, pciDevID, pciRevID)) {
        return 0;
    }

    /* Pay dirt */
    devno = _n_devices++;
    vxd = _devices + devno;
  
    vxd->bde_dev.device = pciDevID;
    vxd->bde_dev.rev = pciRevID;
    vxd->bde_dev.base_address = 0; /* Device not memory accessible */
    vxd->pci_dev = *dev;

    /* Configure PCI
     * On PLI, to speed things up, we set these PCI config location to...
     * 
     * latency timer : 32'h0000_100c = 48<<8
     * enable fast back-to-back: 32'h0000_1004 = 32'h0000_0356    
 */

    
    /* Write control word (turns on parity detect) */
    pci_config_putw(dev, PCI_CONF_COMMAND, (PCI_CONF_COMMAND_BM |
                                            PCI_CONF_COMMAND_MS |
                                            PCI_CONF_COMMAND_PERR |
                                            PCI_CONF_COMMAND_FBBE));
    /* Write base address */
    pci_config_putw(dev, PCI_CONF_BAR0, PLI_PCI_MBAR0);
  
    /* read back */
    if((tmp = pci_config_getw(dev, PCI_CONF_BAR0) & PCI_CONF_BAR_MASK) !=
       PLI_PCI_MBAR0) {
        cli_out("pli-bde: warning: read back of MBAR0 failed.\n");
        cli_out("pli-bde: warning: expected 0x%.8x, read 0x%.8x\n", 
               PLI_PCI_MBAR0, tmp);
    }

    /* Write something to int line */
    tmp = pci_config_getw(dev, PCI_CONF_INTERRUPT_LINE);
    tmp = (tmp & ~0xff) | PLI_PCI_ILINE(devno);
    pci_config_putw(dev, PCI_CONF_INTERRUPT_LINE, tmp);

    /* Set latency timer to 48 */
    tmp = pci_config_getw(dev, PCI_CONF_CACHE_LINE_SIZE);
    tmp |= (48 << 8);
    pci_config_putw(dev, PCI_CONF_CACHE_LINE_SIZE, tmp);

    switch (pciDevID) {
    case BCM56340_DEVICE_ID:
    case BCM56150_DEVICE_ID:
    case BCM56450_DEVICE_ID:
    case BCM56560_DEVICE_ID:
    case BCM56670_DEVICE_ID:
    case BCM56671_DEVICE_ID:
    case BCM56672_DEVICE_ID:
    case BCM56675_DEVICE_ID:
        /* Ensure we work with BCMSIM */
        vxd->cmic_base = 0x48000000;
        break;
    default:
        vxd->cmic_base = pci_config_getw(dev, PCI_CONF_BAR1) & PCI_CONF_BAR_MASK;
        break;
    }
    cli_out("pli-bde: cmic_base = 0x%08x\n", vxd->cmic_base);

    /* Have at it */
    return 0;
}
   

int
plibde_create(ibde_t** bde)
{
    if(_n_devices == 0) {
        pci_device_iter(_setup);
    }
    *bde = &_ibde;
    return 0;  
}


/*
 * Perform a complete  SCHANNEL operation. 
 * This is a backdoor into the simulator to 
 * improve performance. 
 */

int 
plibde_schan_op(int unit,
                uint32* msg,
                int dwc_write, int dwc_read)
{
    /* in pli.c */
    extern int pli_schan(int devNo, uint32* words, int dw_write, int dw_read); 
    return pli_schan(unit, msg, dwc_write, dwc_read); 
}

#define CMODEL_DEFAULT_DEVID 0x8470
#define CMODEL_DEVID_MAX_LENGTH 8
#define CMODEL_DEFAULT_REVID 0x1
#define CMODEL_REVID_MAX_LENGTH 8
extern char *getenv(const char*);

int
plibde_cmodel_create(ibde_t** bde)
{
    char *dev_id_string, *rev_id_string;
    /* Use +2 to leave space for the '0x' addition */
    char dev_id_hex_string[CMODEL_DEVID_MAX_LENGTH+2];
    char rev_id_hex_string[CMODEL_REVID_MAX_LENGTH+2];
    unsigned int dev_id = CMODEL_DEFAULT_DEVID;
    unsigned int rev_id = CMODEL_DEFAULT_REVID;
    char tmp[50];
    int i;
    _n_devices = pli_client_count();

    for (i = 0; i < _n_devices; i++)
    {
        sal_snprintf(tmp, sizeof(tmp), "CMODEL_DEVID_%d", i);
        dev_id_string = getenv(tmp);
        sal_snprintf(tmp, sizeof(tmp), "CMODEL_REVID_%d", i);
        rev_id_string = getenv(tmp);

        if (dev_id_string)
        {
            dev_id_hex_string[0]='0';
            dev_id_hex_string[1]='x';
            sal_memcpy(&(dev_id_hex_string[2]), dev_id_string, CMODEL_DEVID_MAX_LENGTH);
            sscanf(dev_id_hex_string, "%x", &dev_id);
        }

        if (rev_id_string)
        {
            sal_memcpy(rev_id_hex_string, rev_id_string, CMODEL_REVID_MAX_LENGTH);
            sscanf(rev_id_hex_string, "%x", &rev_id);
        }


        _devices[i].bde_dev.device = dev_id;
        _devices[i].bde_dev.rev = rev_id;
    }


    *bde = &_cmodel_bde;
    return 0;
}

#ifdef SINGLE_MODE
/**********************************************************
**********   SINGLE MODE FUNCTIONS
 */

static uint32 
_local_pci_read(int d, uint32 addr)
{
    return local_config_getw(&_devices[d].pci_dev, PLI_PCI_MBAR0 + _devices[d].cmic_base + addr);
}

static int
_local_pci_write(int d, uint32 addr, uint32 data)
{
    return local_config_putw(&_devices[d].pci_dev, PLI_PCI_MBAR0 + _devices[d].cmic_base + addr, data);
}

static uint32  
_local_read(int d, uint32 addr)
{
    return local_memory_getw(&_devices[d].pci_dev, PLI_PCI_MBAR0 + _devices[d].cmic_base + addr);
}

static int
_local_write(int d, uint32 addr, uint32 data)
{
    return local_memory_putw(&_devices[d].pci_dev, PLI_PCI_MBAR0 + _devices[d].cmic_base + addr, data);
}

static uint32  
_local_iproc_read(int d, uint32 addr)
{
    return local_memory_getw(&_devices[d].pci_dev, PLI_PCI_MBAR0 + addr);
}

static int
_local_iproc_write(int d, uint32 addr, uint32 data)
{
    return local_memory_putw(&_devices[d].pci_dev, PLI_PCI_MBAR0 + addr, data);
}

static int 
_local_interrupt_connect(int d, void (*isr)(void*), void* data)
{
    return local_int_connect(PLI_PCI_ILINE(d), isr, data);
}

static ibde_t _local_ibde = {
    _name, 
    _num_devices, 
    _get_dev, 
    _get_dev_type, 
    _local_pci_read,
    _local_pci_write,
    _pci_bus_features,
    _local_read,
    _local_write,
    _salloc,
    _sfree,
    _sflush,
    _sinval,
    _local_interrupt_connect,
    _interrupt_disconnect,
    _l2p,
    _p2l,
    NULL, /* spi_read */
    NULL, /* spi_write */
    _local_iproc_read,
    _local_iproc_write,
};
 
int
plibde_local_create(ibde_t** bde)
{
    char    *chip_name;
    uint16  dev_id;
    uint8   rev_id;

    _n_devices = 1;

    chip_name = getenv("USE_LOCAL_SIM");
    _local_setup(&pcid_info, chip_name, &dev_id, &rev_id);

    _devices[0].bde_dev.device = dev_id; 
    _devices[0].bde_dev.rev = rev_id;
        
    *bde = &_local_ibde;
    return 0;  
}
#else
/* This is decleration of a dummy function that needed in case that SINGLE_MODE is not defined*/
extern void single_mode_dummy_function(void);
#endif /* !SINGLE_MODE */
