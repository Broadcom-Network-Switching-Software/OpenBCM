/*! \file pktio_stub.c
 *
 * Stub functions required to include SDKLT PacketIO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <pktio_dep.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/drv.h>

typedef int bcmdrd_feature_t;
typedef int (*bcmpkt_netif_traverse_cb_f) (int unit,
                                           void *netif,
                                           void *cb_data);

bool
bcmdrd_dev_exists(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return FALSE;
    }

    if (SOC_IS_TOMAHAWK3(unit)) {
        return TRUE;
    }
    return FALSE;
}

void *
bcmdrd_hal_dma_alloc(int unit, size_t size, const char *s, uint64_t *dma_addr)
{
    void *dma_buf;

    dma_buf = soc_cm_salloc(unit, size, s);
    if (dma_buf) {
        *dma_addr = soc_cm_l2p(unit, dma_buf);
    }
    return dma_buf;
}

extern void
bcmdrd_hal_dma_free(int unit, size_t size, void *dma_mem, uint64_t dma_addr)
{
    return soc_cm_sfree(unit, dma_mem);
}

uint64_t
bcmdrd_hal_dma_map(int unit, void *dma_mem, size_t size, bool to_dev)
{
    return soc_cm_l2p(unit, dma_mem);
}

void
bcmdrd_hal_dma_unmap(int unit, uint64_t dma_addr, size_t size, bool to_dev)
{

}

const char *
bcmdrd_dev_type_str(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return NULL;
    }

    if (SOC_IS_TOMAHAWK3(unit)) {
        return "bcm56980_a0";
    }
    return NULL;
}

bool
bcmdrd_feature_enabled(int unit, bcmdrd_feature_t feature)
{
    /*
     * Checking feature is enabled in SDKLT.
     * None of them is required for pktio APIs for now.
     */
    return 0;
}

int
bcmdrd_hal_reg32_read(int unit, uint32_t addr, uint32_t *val)
{
    *val = soc_pci_read(unit, addr);
    return 0;
}

int
bcmdrd_hal_reg32_write(int unit, uint32_t addr, uint32_t val)
{
    return soc_pci_write(unit, addr, val);
}

bool
bcmdrd_dev_byte_swap_packet_dma_get(int unit)
{
    int  big_pio, big_packet, big_other;

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    return big_packet;
}

bool
bcmdrd_dev_byte_swap_non_packet_dma_get(int unit)
{
    int  big_pio, big_packet, big_other;

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    return big_other;
}

bcmlrd_variant_t
bcmlrd_variant_get(int unit)
{
    bcmlrd_variant_t variant = BCMLRD_VARIANT_T_NONE;

    if (SOC_IS_TOMAHAWK3(unit)) {
        variant = BCMLRD_VARIANT_T_bcm56980_a0_BASE;
    }
    return variant;
}

int
bcmbd_knet_enable_set(int unit, bool enable)
{
    /*
     * Referenced in bcmpkt_dev_drv_attach when deploying KNET driver.
     * For pktio APIs, we deploy CNET driver.
     * We can just skip this function for now.
     */
    return 0;
}

int
bcmbd_knet_enable_get(int unit, bool *enable)
{
    /*
     * Referenced in bcmpkt_dev_drv_attach when deploying KNET driver.
     * For pktio APIs, we deploy CNET driver.
     * We can just skip this function for now.
     */
    return 0;
}

int
bcmpkt_netif_traverse(int unit, bcmpkt_netif_traverse_cb_f cb_func,
                      void *cb_data)
{
    /*
     * Referenced in bcmpkt_unet_drv_detach when deploying KNET driver.
     * For pktio APIs, we deploy CNET driver.
     * We can just skip this function for now.
     */
    return 0;
}
