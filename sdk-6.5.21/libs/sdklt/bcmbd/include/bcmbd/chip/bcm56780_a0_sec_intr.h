/*! \file bcm56780_a0_mmu_intr.h
 *
 * MMU interrupt sources for BCM56780_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_INTR_H
#define BCM56780_A0_SEC_INTR_H

/*!
 * \name SEC interrupt sources.
 */

#define SEC_INTR_SA_EXPIRY_BASE                 0
/* SA expiry interrupts */
#define SEC_INTR_SA_EXPIRY_ENCRYPT              (SEC_INTR_SA_EXPIRY_BASE + 0)
#define SEC_INTR_SA_EXPIRY_DECRYPT              (SEC_INTR_SA_EXPIRY_BASE + 1)
#define NUM_SEC_INTR_SA_EXPIRY                  2

#define SEC_INTR_SER_EVENT_BASE                 (SEC_INTR_SA_EXPIRY_BASE + \
                                                 NUM_SEC_INTR_SA_EXPIRY)
#define SEC_INTR_ESEC_SC_TABLE_SER_EVENT        (SEC_INTR_SER_EVENT_BASE + 0)
#define SEC_INTR_ISEC_SC_TABLE_SER_EVENT        (SEC_INTR_SER_EVENT_BASE + 1)
#define SEC_INTR_ESEC_SA_TABLE_SER_EVENT        (SEC_INTR_SER_EVENT_BASE + 2)
#define SEC_INTR_ISEC_SA_TABLE_SER_EVENT        (SEC_INTR_SER_EVENT_BASE + 3)
#define SEC_INTR_ESEC_HASH_TABLE_SER_EVENT      (SEC_INTR_SER_EVENT_BASE + 4)
#define SEC_INTR_ISEC_HASH_TABLE_SER_EVENT      (SEC_INTR_SER_EVENT_BASE + 5)
#define SEC_INTR_ISEC_SP_POLICY_SER_EVENT       (SEC_INTR_SER_EVENT_BASE + 6)
#define SEC_INTR_ISEC_SP_MAP_SER_EVENT          (SEC_INTR_SER_EVENT_BASE + 7)
#define SEC_INTR_ISEC_SP_TCAM_SER_EVENT         (SEC_INTR_SER_EVENT_BASE + 8)
#define SEC_INTR_ISEC_SC_TCAM_SER_EVENT         (SEC_INTR_SER_EVENT_BASE + 9)
#define SEC_INTR_MIB_SER_EVENT                  (SEC_INTR_SER_EVENT_BASE + 10)
#define SEC_INTR_ISEC_SC_MAP_SER_EVENT          (SEC_INTR_SER_EVENT_BASE + 11)
#define NUM_SEC_INTR_SER_EVENT                  12
#define SEC_INTR_MAX                            (SEC_INTR_SER_EVENT_BASE + \
                                                 NUM_SEC_INTR_SER_EVENT)

#endif /* BCM56780_A0_SEC_INTR_H */
