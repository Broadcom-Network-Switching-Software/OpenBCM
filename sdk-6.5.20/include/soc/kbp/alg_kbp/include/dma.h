/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 

#ifndef __DMA_H
#define __DMA_H

 

#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "device.h"
#include "db.h"

#ifdef __cplusplus
 extern "C" {
#endif

 

 

 struct kbp_dma_db;

 

 kbp_status kbp_dma_db_init(struct kbp_device *device, uint32_t id, uint32_t capacity,
                           uint32_t width_1, struct kbp_dma_db **db);

 

 kbp_status kbp_dma_db_destroy(struct kbp_dma_db *db);

 

 kbp_status kbp_dma_db_set_resource(struct kbp_dma_db *db, enum kbp_hw_resource resource, ...);

 

 kbp_status kbp_dma_db_get_resource(struct kbp_dma_db *db, enum kbp_hw_resource resource, ...);

 

 kbp_status kbp_dma_db_set_property(struct kbp_dma_db *db, enum kbp_db_properties property, ...);

 

 kbp_status kbp_dma_db_get_property(struct kbp_dma_db *db, enum kbp_db_properties property, ...);

 

 kbp_status kbp_dma_db_add_entry(struct kbp_dma_db *db, uint8_t * value, uint32_t offset);

 

 kbp_status kbp_dma_db_delete_entry(struct kbp_dma_db *db, uint32_t offset);

 

 kbp_status kbp_dma_db_read_entry(struct kbp_dma_db *db, uint32_t offset, uint8_t * value);

 

 

 

 kbp_status kbp_dma_db_refresh_handle(struct kbp_device *device, struct kbp_dma_db *stale_ptr, struct kbp_dma_db **dbp);

 

#ifdef __cplusplus
 }
#endif
#endif 

