/*! \file bcmfp_ingress_internal.h
 *
 * Init/Cleanup APIs for Ingress Field Processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_INGRESS_INTERNAL_H
#define BCMFP_INGRESS_INTERNAL_H

/*! Initialize the s/w state for ingress field processor. */
extern int
bcmfp_ingress_init(int unit, bool warm);

/*! Register call back function for Ingress Field Processor. */
extern int
bcmfp_ingress_imm_register(int unit);

/*! Register call back function for Exact Match Field Processor. */
extern int
bcmfp_em_imm_register(int unit);

/*! Cleanup the s/w state for ingress field processor. */
extern int
bcmfp_ingress_cleanup(int unit);

#endif /* BCMFP_INGRESS_INTERNAL_H */
