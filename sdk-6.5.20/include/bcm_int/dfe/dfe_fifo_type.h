/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE FIFO TYPES H
 */

#ifndef _BCM_DFE_FIFO_TYPE_H_
#define _BCM_DFE_FIFO_TYPE_H_

#include <sal/types.h>

#include <soc/dfe/cmn/dfe_drv.h>

typedef soc_dfe_fifo_type_handle_t _bcm_dfe_fifo_type_handle_t;

int bcm_dfe_fifo_type_clear(int unit, _bcm_dfe_fifo_type_handle_t *h);

int bcm_dfe_fifo_type_is_rx(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_rx);
int bcm_dfe_fifo_type_is_tx(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_tx);
int bcm_dfe_fifo_type_is_fe1(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_fe1);
int bcm_dfe_fifo_type_is_fe3(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_fe3);
int bcm_dfe_fifo_type_is_primary(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_primary);
int bcm_dfe_fifo_type_is_secondary(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_secondary);

int bcm_dfe_fifo_type_set(int unit, _bcm_dfe_fifo_type_handle_t* h, int is_rx, int is_tx, int is_fe1, int is_fe3, int is_primary, int is_secondary);

int bcm_dfe_fifo_type_get_id(int unit, _bcm_dfe_fifo_type_handle_t h, int* id);
int bcm_dfe_fifo_type_set_id(int unit, _bcm_dfe_fifo_type_handle_t* h, int fifo_id);

int bcm_dfe_fifo_type_set_handle_flag(int unit, _bcm_dfe_fifo_type_handle_t* h);
int bcm_dfe_fifo_type_get_handle_flag(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_handle_flag);

int bcm_dfe_fifo_type_is_overlap(int unit, _bcm_dfe_fifo_type_handle_t h1, _bcm_dfe_fifo_type_handle_t  h2, int* is_overlap);
int bcm_dfe_fifo_type_add(int unit, soc_dfe_fabric_link_fifo_type_index_t fifo_type, _bcm_dfe_fifo_type_handle_t  h2);
int bcm_dfe_fifo_type_sub(int unit, soc_dfe_fabric_link_fifo_type_index_t fifo_type, _bcm_dfe_fifo_type_handle_t  h2);

#endif /*_BCM_DFE_FIFO_TYPE_H_*/
