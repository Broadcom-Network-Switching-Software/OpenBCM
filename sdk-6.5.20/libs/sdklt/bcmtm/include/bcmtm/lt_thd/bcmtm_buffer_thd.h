/*! \file bcmtm_buffer_thd.h
 *
 * TM buffer threshold calculation related apis and definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_BUFFER_THD_H
#define BCMTM_BUFFER_THD_H

#include <shr/shr_types.h>

struct bcmtm_dev_buf_info{
    uint32_t mmu_phy_cells_xpe;
    uint32_t mmu_rsvd_cfap_cells_xpe;
    uint32_t mmu_cfap_bank_full;
    uint32_t pg_headroom;
    uint32_t pg_guarantee;
    uint32_t mcq_rsvd;
    uint32_t egr_rsvd;
    uint32_t asf_rsvd;
};

extern int
bcmtm_port_pg_delta(int unit,
                uint32_t fval,
                int cur_val,
                int granularity,
                int pipe,
                int *delta);

extern int
bcmtm_shared_size_update(int unit,
                    uint32_t *shared_size,
                    int *delta,
                    int *update);

extern int
bcmtm_ing_res_limits_update(int unit,
                    uint32_t *shared_size,
                    int granularity);

extern int
bcmtm_egr_res_limits_update(int unit,
                    uint32_t *egr_db_shd_size,
                    int *egr_qe_shd_size);

#endif /* BCMTM_BUFFER_THD_H */
