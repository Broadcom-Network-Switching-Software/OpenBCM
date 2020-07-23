/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        chipsim_bde.c
 * Purpose:     Implements BDE interface on top of DPP Chipsim model code
 */

#include <stdlib.h>
#include <stdio.h>

#include <soc/devids.h>
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <ibde.h>
#include <appl/diag/parse.h>
#include <sal/core/alloc.h>
#include <bde/pli/plibde.h>

#include "chipsim_bde.h"
#include "ChipSim/chip_sim_low.h"

#define MAX_DEVICES 16
    
typedef struct bde_dev_s {
    uint32 dev_type;
    ibde_dev_t  bde_dev;
} chipsim_bde_dev_t;

static chipsim_bde_dev_t    _devices[MAX_DEVICES];
static int                  _n_devices = 0;

STATIC const char*
_chipsim_bde_name(void)
{
    return "dpp-chipsim-bde";
}

STATIC int
_chipsim_bde_num_devices(int type)
{
    switch (type) {
    case BDE_ALL_DEVICES:
        return _n_devices;

    case BDE_SWITCH_DEVICES:
        return _n_devices - 1;

    case BDE_ETHER_DEVICES:
    default:
        return 0;
    }
                                    
    return 0;
}

STATIC const ibde_dev_t*
_chipsim_bde_get_dev(int d)
{
    if(d < 0 || d > _n_devices) {
        return NULL;
    }
    return &_devices[d].bde_dev;
}

STATIC uint32
_chipsim_bde_get_dev_type(int d)
{
    if(d < 0 || d > _n_devices) {
        return 0;
    }
    return _devices[d].dev_type;
}

STATIC uint32
_chipsim_bde_pci_config_read(int d, uint32 addr)
{
    return (0xffffffff); /* not supported */
}

STATIC int
_chipsim_bde_pci_config_write(int d, uint32 addr, uint32 data)
{
    return (-1); /* not supported */
}

STATIC void
_chipsim_bde_pci_bus_features(int unit, int* be_pio, int* be_packet, int* be_other)
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


#ifndef CHIP_SIM_NOF_CHIPS
#define CHIP_SIM_NOF_CHIPS MAX_DEVICES
#endif

STATIC uint32  
_chipsim_bde_memory_read(int d, uint32 addr)
{
    int     status = -1;
    UINT32  data;

    if (status) {
        printf("Unit:%d Chipsim BDE read from addr:%x failed \n", d, addr);
        data = 0xffffffff; /* some error */
    }

    return data;
}

STATIC int
_chipsim_bde_memory_write(int d, uint32 addr, uint32 data)
{
    int     rv = -1;

    if (rv) {
        printf("Unit:%d Chipsim BDE write data:%x to addr:%x failed \n", 
               d, data, addr);
    }

    return rv;
}

STATIC uint32*
_chipsim_bde_salloc(int d, int size, const char *name)
{
    return sal_dma_alloc(size, (char *)name);
}

STATIC void
_chipsim_bde_sfree(int d, void* ptr)
{
    sal_dma_free(ptr);
}

STATIC int
_chipsim_bde_sflush(int d, void* addr, int length)
{
    sal_dma_flush(addr, length);
    return 0;
}

STATIC int
_chipsim_bde_sinval(int d, void* addr, int length)
{
    sal_dma_inval(addr, length);
    return 0;
}

STATIC int
_chipsim_bde_interrupt_connect(int d, void (*isr)(void*), void* data)
{
    return 0;
}

STATIC int
_chipsim_bde_interrupt_disconnect(int d)
{
    return 0;
}

STATIC sal_paddr_t
_chipsim_bde_l2p(int d, void* laddr)
{
    return (sal_paddr_t)(PTR_TO_INT(laddr));
}   

STATIC void*
_chipsim_bde_p2l(int d, sal_paddr_t paddr)
{
    return INT_TO_PTR(paddr);
}

static ibde_t _chipsim_bde = {
    _chipsim_bde_name,
    _chipsim_bde_num_devices,
    _chipsim_bde_get_dev,
    _chipsim_bde_get_dev_type,
    _chipsim_bde_pci_config_read,
    _chipsim_bde_pci_config_write,
    _chipsim_bde_pci_bus_features,
    _chipsim_bde_memory_read,
    _chipsim_bde_memory_write,
    _chipsim_bde_salloc,
    _chipsim_bde_sfree,
    _chipsim_bde_sflush,
    _chipsim_bde_sinval,
    _chipsim_bde_interrupt_connect,
    _chipsim_bde_interrupt_disconnect,
    _chipsim_bde_l2p,
    _chipsim_bde_p2l,
};

int
chipsim_eb_bus_probe(chipsim_bde_bus_t *bus)
{
    chipsim_bde_dev_t   *cbd;
    char                *chipsim_ndev_str;
    int                 chipsim_ndev;
    
    chipsim_ndev_str = var_get("chipsim_ndev");
    
    if (chipsim_ndev_str != NULL) {
        chipsim_ndev = atoi(chipsim_ndev_str);
        if (chipsim_ndev <= 0 || chipsim_ndev > CHIP_SIM_NOF_CHIPS) {
            printf("chipsim_eb_bus_probe: Invalid number of chipsim instances"
                   " specified. Valid SOC_TARGET_COUNT range is 1..%d\n", 
                   CHIP_SIM_NOF_CHIPS);
            return -1;
        }
    } else {
        chipsim_ndev = 1;
    }

    if(chipsim_ndev != 0){
        char                str_buffer[32];
        char                *chip_type_str;

        while (_n_devices < chipsim_ndev) {
            cbd = _devices + _n_devices;
            sal_sprintf(str_buffer, "chipsim_dev.%d", _n_devices);
            chip_type_str = var_get(str_buffer);
            if (chip_type_str == NULL || sal_strcmp(chip_type_str, "arad") == 0) {
                cbd->dev_type = BDE_EB_DEV_TYPE;
                cbd->bde_dev.device = ARAD_DEVICE_ID;
                cbd->bde_dev.rev    = ARAD_A0_REV_ID;
		    } else if (sal_strcmp(chip_type_str, "fe1600") == 0) {
		        cbd->dev_type = BDE_EB_DEV_TYPE;
                cbd->bde_dev.device = BCM88750_DEVICE_ID;
                cbd->bde_dev.rev    = BCM88750_A0_REV_ID;
            } else {
                printf("chipsim_eb_bus_probe: Unknown chip type \"%s\"", chip_type_str);
                return -1;
            }
            cbd->bde_dev.base_address = 0;
            _n_devices++;
        }
    }

    return 0;
}

int
chipsim_bde_create(chipsim_bde_bus_t *bus, ibde_t **bde)
{
    int     use_sand_sim = 0;
    char    *use_sand_sim_str;
#ifdef SINGLE_MODE
    int     use_local_sim = 0;
    char    *use_local_sim_str;

    use_local_sim_str = getenv("USE_LOCAL_SIM");
    if (use_local_sim_str != NULL) {
        use_local_sim = atoi(use_local_sim_str);
        if (use_local_sim) {
            printf("Using local sim \n");
        }
    }

    if (use_local_sim) {
        plibde_local_create(bde);
        return 0;
    }
#endif
    
    use_sand_sim_str = getenv("USE_SAND_SIM");
    if (use_sand_sim_str != NULL) {
        use_sand_sim = atoi(use_sand_sim_str);
        if (use_sand_sim) {
            printf("Using SOC_SAND sim \n");
        }
    }  
    
    if(!use_sand_sim) {

#ifdef ADAPTER_SERVER_MODE
        plibde_cmodel_create(bde);
#else
        plibde_create(bde); 
#endif
        return 0;
    }
    
    if ((bus == NULL) || (bde == NULL)) {
        printf("chipsim_bde_create: invalid arguments \n");
        return (-1);
    }

    if(_n_devices == 0) {
        if (chipsim_eb_bus_probe(bus) < 0) {
            printf("chipsim_bde_create: eb bus probe failed. \n");
            return (-1);
        }
    }

    *bde = &_chipsim_bde;
    return 0;
}
