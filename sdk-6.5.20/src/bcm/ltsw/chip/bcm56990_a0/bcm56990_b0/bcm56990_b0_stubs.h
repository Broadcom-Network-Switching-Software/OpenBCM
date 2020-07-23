/*! \file bcm56990_b0_stubs.h
 *
 * BCM56990_B0 subordinate driver stubs list.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BCM56990_A0_STUB_DRV_ENTRY
/*
 * BCM56990_A0_STUB_DRV_ENTRY macros.
 *
 * Before including this file, define BCM56990_A0_STUB_DRV_ENTRY
 * as a macro to operate on the following parameters:
 *
 *     #define BCM56990_A0_STUB_DRV_ENTRY(_dn,_mn,_bd)
 *
 *     _dn: Device Name
 *     _mn: Module Name
 *     _bd: Base Device
 *
 * Note that this macro will be undefined at the end of this definition.
 */

/*
 * Stub functions list for those sub-drivers which will not be attached
 * for this device.
 */
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, dev, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, flexctr, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, flow, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, int, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, l3, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, l3_egress, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, l3_fib, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, l3_intf, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, lb_hash, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, mirror, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, mpls, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, stat, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, tunnel, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, uft, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, virtual, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, vlan, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, l3_aacl, "BCM56990")
BCM56990_A0_STUB_DRV_ENTRY(bcm56990_b0, stg, "BCM56990")

/* End BCM56990_A0_STUB_DRV_ENTRY Macros */

#undef BCM56990_A0_STUB_DRV_ENTRY
#endif /* BCM56990_A0_STUB_DRV_ENTRY */
