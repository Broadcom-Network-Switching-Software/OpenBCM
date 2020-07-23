/*! \file bcmfp_egress_internal.h
 *
 * Init/Cleanup APIs for Egress Field Processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_EGRESS_INTERNAL_H
#define BCMFP_EGRESS_INTERNAL_H

/*! Initialize the s/w state for egress field processor. */
extern int
bcmfp_egress_init(int unit);

/*! Register call back function for egress field processor. */
extern int
bcmfp_egress_imm_register(int unit);

/*! Cleanup the s/w state for egress field processor. */
extern int
bcmfp_egress_cleanup(int unit);

#endif /* BCMFP_EGRESS_INTERNAL_H */
