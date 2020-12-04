
/*! \file pktio.h
 *
 * esw pktio module internal header file.
 * Include structure definitions and function declarations used by other modules.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMI_ESW_PKTIO_H
#define BCMI_ESW_PKTIO_H

#ifdef INCLUDE_PKTIO

extern int
bcmi_esw_pktio_alloc(
    int unit,
    uint32 max_size,
    uint32 flags,
    bcm_pktio_pkt_t **packet);

extern int
bcmi_esw_pktio_claim(
    int unit,
    bcm_pktio_pkt_t *pkt,
    bcm_pktio_pkt_t **new_pkt);

extern int
bcmi_esw_pktio_cleanup(
    int unit);

extern int
bcmi_esw_pktio_free(
    int unit,
    bcm_pktio_pkt_t *packet);

extern int
bcmi_esw_pktio_init(
    int unit);

extern int
bcmi_esw_pktio_pkt_data_get(
    int unit,
    bcm_pktio_pkt_t *packet,
    void **data,
    uint32 *length);

extern int
bcmi_esw_pktio_pmd_fid_support_get(
    int unit,
    bcm_pktio_fid_support_t *support);

extern int
bcmi_esw_pktio_pmd_field_get(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_pmd_type_t type,
    int fid,
    uint32 *val);

extern int
bcmi_esw_pktio_pmd_field_set(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_pmd_type_t type,
    int fid,
    uint32 val);

extern int
bcmi_esw_pktio_pmd_reasons_get(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_reasons_t *reasons);

extern int
bcmi_esw_pktio_pmd_set(
    int unit,
    bcm_pktio_pkt_t *packet,
    bcm_pktio_pmd_t *txpmd);

extern int
bcmi_esw_pktio_pull(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data);

extern int
bcmi_esw_pktio_push(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data);

extern int
bcmi_esw_pktio_put(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data);

extern int
bcmi_esw_pktio_reserve(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len,
    uint8 **data);

extern int
bcmi_esw_pktio_rx_register(
    int unit,
    const char *name,
    bcm_pktio_rx_cb_f callback,
    uint8 priority,
    void *cookie,
    uint32 flags);

extern int
bcmi_esw_pktio_rx_unregister(
    int unit,
    bcm_pktio_rx_cb_f callback,
    uint8 priority);

extern int
bcmi_esw_pktio_trim(
    int unit,
    bcm_pktio_pkt_t *packet,
    uint32 len);

extern int
bcmi_esw_pktio_tx(
    int unit,
    bcm_pktio_pkt_t *packet);

extern int
bcmi_esw_pktio_rx_dump(
    int unit,
    bcm_pktio_pkt_t *packet,
    int flags);

#endif /* INCLUDE_PKTIO */
#endif /* BCMI_ESW_PKTIO_H */
