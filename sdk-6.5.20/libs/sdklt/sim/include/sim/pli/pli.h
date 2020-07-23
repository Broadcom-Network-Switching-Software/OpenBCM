/*! \file pli.h
 *
 * PLI API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PLI_H
#define PLI_H

#include <sal/sal_types.h>

typedef void (*pli_isr_t)(void *isr_data);

/*
 * Client calls.
 */
extern int pli_client_count(void);
extern int pli_client_attach(int dev_no);
extern int pli_register_isr(int dev_no, pli_isr_t isr, void *isr_data);
extern uint32_t pli_getreg(int dev_no, uint32_t regtype, uint32_t regnum);
extern uint32_t pli_setreg(int dev_no, uint32_t regtype, uint32_t regnum,
                         uint32_t regval);
extern int pli_client_detach(int dev_no);
extern int pli_schan(int dev_no, uint32_t *words, int dw_write, int dw_read);

#endif /* PLI_H */
