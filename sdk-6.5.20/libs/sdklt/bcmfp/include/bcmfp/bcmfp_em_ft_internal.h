/*! \file bcmfp_em_ft_internal.h
 *
 * Init/Cleanup APIs for Exact Match Field Processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_EM_FT_INTERNAL_H
#define BCMFP_EM_FT_INTERNAL_H

/*! Initialize the s/w state for exact match field processor. */
extern int
bcmfp_em_ft_init(int unit, bool warm);

/*! Register call back function for exact match field processor. */
extern int
bcmfp_em_ft_imm_register(int unit);

/*! Cleanup the s/w state for exact match field processor. */
extern int
bcmfp_em_ft_cleanup(int unit);

extern void
bcmfp_em_ft_grp_change_event_cb(int unit, const char *event, uint64_t ev_data);

#endif /* BCMFP_EM_FT_INTERNAL_H */
