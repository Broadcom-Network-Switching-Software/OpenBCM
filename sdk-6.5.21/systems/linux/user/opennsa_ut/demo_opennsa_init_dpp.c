/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * demo_opennsa_init_dpp.c: A simple diagnostic utility to verify opennsa library.
 *
 * This utility leverages Broadcom Diag Shell to initialize the device and to allow
 * full access to diagnostics.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NO_SAL_APPL
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <appl/diag/bslmgmt.h>
#endif

#include <appl/diag/opennsa_diag.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <soc/opensoc.h>
#include <soc/cmext.h>
#include <sal/core/boot.h>
#include <linux-bde.h>

ibde_t *bde;

/*
 * The bus properties are (currently) the only system specific
 * settings required. These must be defined beforehand.
 */

#ifndef SYS_BE_PIO
#error "SYS_BE_PIO must be defined for the target platform"
#endif
#ifndef SYS_BE_PACKET
#error "SYS_BE_PACKET must be defined for the target platform"
#endif
#ifndef SYS_BE_OTHER
#error "SYS_BE_OTHER must be defined for the target platform"
#endif

#if !defined(SYS_BE_PIO) || !defined(SYS_BE_PACKET) || !defined(SYS_BE_OTHER)
#error "platform bus properties not defined."
#endif

/* Function defined in linux-user-bde.c */
extern int
bde_icid_get( int d, uint8 *data, int len );

static soc_chip_info_vectors_t chip_info_vect = {
    bde_icid_get
};

#ifdef INCLUDE_KNET

#include <soc/knet.h>
#include <bcm-knet-kcom.h>

/* Function defined in linux-user-bde.c */
extern int
bde_irq_mask_set( int unit, uint32 addr, uint32 mask );
extern int
bde_hw_unit_get( int unit, int inverse );

static soc_knet_vectors_t knet_vect_bcm_knet = {
    {
        bcm_knet_kcom_open,
        bcm_knet_kcom_close,
        bcm_knet_kcom_msg_send,
        bcm_knet_kcom_msg_recv
    },
    bde_irq_mask_set,
    bde_hw_unit_get
};

static void
knet_kcom_config( void )
{
    /* Direct IOCTL by default */
    soc_knet_config( &knet_vect_bcm_knet );
}

#endif /* INCLUDE_KNET */

int
bde_create( void )
{
    linux_bde_bus_t bus;
    bus.be_pio = SYS_BE_PIO;
    bus.be_packet = SYS_BE_PACKET;
    bus.be_other = SYS_BE_OTHER;

    return linux_bde_create( &bus, &bde );

}

/*
 * Main loop.
 */
int main( int argc, char *argv[] )
{
    int i;

    if (sal_core_init() < 0
#ifndef NO_SAL_APPL
        || sal_appl_init() < 0
#endif
        ) {
#ifndef NO_SAL_APPL
        printf( "SAL Initialization failed\r\n" );
#endif
        exit( 1 );
    }

    for( i = 1; i < argc; i++ ){
        if( !strcmp(argv[i], "-r") || !strcmp(argv[i], "--reload") ){
            sal_boot_flags_set( sal_boot_flags_get() | BOOT_F_RELOAD );
        }
    }

#ifdef INCLUDE_KNET
    knet_kcom_config();
#endif
    soc_chip_info_vect_config( &chip_info_vect );

    diag_shell();

    linux_bde_destroy( bde );
    return 0;
}
