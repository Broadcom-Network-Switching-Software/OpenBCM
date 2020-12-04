/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pli.h
 * Purpose:     
 */

#ifndef   _PLI_H_
#define   _PLI_H_

#include <sys/types.h>
#include <soc/types.h>

#include "cmicsim.h"
#include "dma.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <bde/pli/verinet.h>


/* Return values for process request */
#define PR_REQUEST_HANDLED 1
#define PR_NO_REQUEST 2
#define PR_ALL_DONE 0
#define PR_ERROR -1

extern int pli_reset_service(pcid_info_t *pcid_info);
extern uint32 pli_getreg_service(pcid_info_t * pcid_info, int unit,
                                 uint32 type, uint32 regnum);
extern uint32 pli_setreg_service(pcid_info_t * pcid_info, int unit,
                                 uint32 type, uint32 regnum, uint32 value);
extern uint32 pli_cmice_getreg_service(pcid_info_t * pcid_info, int unit,
                                 uint32 type, uint32 regnum, uint32 *value);
extern uint32 pli_cmice_setreg_service(pcid_info_t * pcid_info, int unit,
                                 uint32 type, uint32 regnum, uint32 value);

#endif /* _PLI_H_ */
