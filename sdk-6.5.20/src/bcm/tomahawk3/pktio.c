/*! \file pktio.c
 *
 * Emulator over SDKLT PacketIO interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_PKTIO)

#include <bcm_int/control.h>
#include <bcm_int/esw/pktio.h>
#include <bcm/pktio.h>

int
bcm_tomahawk3x_pktio_alloc(
    int unit,
    uint32 max_size,
    uint32 flags,
    bcm_pktio_pkt_t **packet)
{
    return bcmi_esw_pktio_alloc(unit, max_size, flags, packet);
}

int
bcm_tomahawk3x_pktio_claim(
    int unit,
    bcm_pktio_pkt_t *pkt,
    bcm_pktio_pkt_t **new_pkt)
{
    return bcmi_esw_pktio_claim(unit, pkt, new_pkt);
}

int
bcm_tomahawk3x_pktio_cleanup(
    int unit)
{
    return bcmi_esw_pktio_cleanup(unit);
}

int
bcm_tomahawk3x_pktio_free(
    int unit,
    bcm_pktio_pkt_t *packet)
{
    return bcmi_esw_pktio_free(unit, packet);
}

int
bcm_tomahawk3x_pktio_init(
    int unit)
{
    return bcmi_esw_pktio_init(unit);
}

int
bcm_tomahawk3x_pktio_pkt_data_get(
    int unit,
    bcm_pktio_pkt_t *packet,
    void **data,
    uint32 *length)
{
    return bcmi_esw_pktio_pkt_data_get(unit, packet, data, length);
}

int
bcm_tomahawk3x_pktio_pmd_fid_support_get(
    int unit,
    bcm_pktio_fid_support_t *support)
{
    return bcmi_esw_pktio_pmd_fid_support_get(unit, support);
}

int
bcm_tomahawk3x_pktio_pmd_field_get(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_pmd_type_t type,
    int fid,
    uint32 *val)
{
    return bcmi_esw_pktio_pmd_field_get(unit, packet, type, fid, val);
}

int
bcm_tomahawk3x_pktio_pmd_field_set(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_pmd_type_t type,
    int fid,
    uint32 val)
{
    return bcmi_esw_pktio_pmd_field_set(unit, packet, type, fid, val);
}

int
bcm_tomahawk3x_pktio_pmd_reasons_get(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_reasons_t *reasons)
{
    return bcmi_esw_pktio_pmd_reasons_get(unit, packet, reasons);
}

int
bcm_tomahawk3x_pktio_pmd_set(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_pmd_t *txpmd)
{
    return bcmi_esw_pktio_pmd_set(unit, packet, txpmd);
}

int
bcm_tomahawk3x_pktio_pull(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data)
{
    return bcmi_esw_pktio_pull(unit, packet, len, data);
}

int
bcm_tomahawk3x_pktio_push(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data)
{
    return bcmi_esw_pktio_push(unit, packet, len, data);
}

int
bcm_tomahawk3x_pktio_put(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data)
{
    return bcmi_esw_pktio_put(unit, packet, len, data);
}

int
bcm_tomahawk3x_pktio_reserve(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data)
{
    return bcmi_esw_pktio_reserve(unit, packet, len, data);
}

int
bcm_tomahawk3x_pktio_rx_register(
    int unit,
    const char *name,
    bcm_pktio_rx_cb_f callback,
    uint8 priority,
    void *cookie,
    uint32 flags)
{
    return bcmi_esw_pktio_rx_register(unit, name, callback, priority, cookie, flags);
}

int
bcm_tomahawk3x_pktio_rx_unregister(
    int unit,
    bcm_pktio_rx_cb_f callback,
    uint8 priority)
{
    return bcmi_esw_pktio_rx_unregister(unit, callback, priority);
}

int
bcm_tomahawk3x_pktio_trim(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len)
{
    return bcmi_esw_pktio_trim(unit, packet, len);
}

int
bcm_tomahawk3x_pktio_tx(
    int unit,
    bcm_pktio_pkt_t *packet)
{
    return bcmi_esw_pktio_tx(unit, packet);
}
#else
typedef int bcm_tomahawk3_pktio_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PKTIO */
