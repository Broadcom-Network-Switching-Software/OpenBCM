/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * demo_opennsa_init: A simple diagnostic utility to verify OpenNSA library.
 *
 * The only thing that is does is that it scans the bus, attaches
 * all recognized Broadcom devices and attempts to perform soc_init(),
 * soc_misc_init(), soc_mmu_init() and bcm_init() on each attached
 * unit.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NO_SAL_APPL
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <appl/diag/bslmgmt.h>
#include <appl/diag/opennsa_diag.h>
#endif

#include <bcm/init.h>
#include <bcm/error.h>
#include <soc/cmext.h>
#include <soc/opensoc.h>
#include <sal/core/boot.h>
#include <linux-bde.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/switch.h>
#endif

/*
 * These includes are needed for do_per_switch_setup() part of the demo.
 */
#include <bcm/port.h>
#include <bcm/stg.h>

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

/* SOC Configuration Manager Device Vectors */

static char *
_sysconf_get_property( const char *property )
{
#ifndef NO_SAL_APPL
    return sal_config_get( property );
#else
    return NULL;
#endif
}

static char *
_config_var_get( soc_cm_dev_t *dev, const char *property )
{
    COMPILER_REFERENCE( dev );

    return _sysconf_get_property( property );
}

static void
_write( soc_cm_dev_t *dev, uint32 addr, uint32 data )
{
    bde->write( dev->dev, addr, data );
}

static uint32
_read( soc_cm_dev_t *dev, uint32 addr )
{
    return bde->read( dev->dev, addr );
}

static void *
_salloc( soc_cm_dev_t *dev, int size, const char *name )
{
    COMPILER_REFERENCE( name );
    return bde->salloc( dev->dev, size, name );
}

static void
_sfree( soc_cm_dev_t *dev, void *ptr )
{
    bde->sfree( dev->dev, ptr );
}

static int
_sflush( soc_cm_dev_t *dev, void *addr, int length )
{
    return( (bde->sflush) ? bde->sflush( dev->dev, addr, length ) : 0 );
}

static int
_sinval( soc_cm_dev_t *dev, void *addr, int length )
{
    return( (bde->sinval) ? bde->sinval( dev->dev, addr, length ) : 0 );
}

static sal_paddr_t
_l2p( soc_cm_dev_t *dev, void *addr )
{
    return( (bde->l2p) ? bde->l2p( dev->dev, addr ) : 0 );
}

static void*
_p2l( soc_cm_dev_t *dev, sal_paddr_t addr )
{
    return( (bde->p2l) ? bde->p2l( dev->dev, addr ) : 0 );
}

static void
_iproc_write( soc_cm_dev_t *dev, uint32 addr, uint32 data )
{
    bde->iproc_write( dev->dev, addr, data );
}

static uint32
_iproc_read( soc_cm_dev_t *dev, uint32 addr )
{
    return( bde->iproc_read( dev->dev, addr ) );
}

static int
_interrupt_connect( soc_cm_dev_t *dev, soc_cm_isr_func_t handler, void *data )
{
    return( bde->interrupt_connect( dev->dev, handler, data ) );
}

static int
_interrupt_disconnect( soc_cm_dev_t *dev )
{
    return( bde->interrupt_disconnect( dev->dev ) );
}

static void
_pci_conf_write( soc_cm_dev_t *dev, uint32 addr, uint32 data )
{
    bde->pci_conf_write( dev->dev, addr, data );
}

static uint32
_pci_conf_read( soc_cm_dev_t *dev, uint32 addr )
{
    return( bde->pci_conf_read( dev->dev, addr) );
}

static int
_spi_read( soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len )
{
    return( (bde->spi_read) ? bde->spi_read( dev->dev, addr, buf, len ) : -1 );
}

static int
_spi_write( soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len )
{
    return( (bde->spi_write) ? bde->spi_write(dev->dev, addr, buf, len) : -1 );
}

static int
_i2c_device_read( soc_cm_dev_t *dev, uint32 addr, uint32 *value )
{
    return( (bde->i2c_device_read) ?
             bde->i2c_device_read(dev->dev, addr, value) : -1 );
}

static int
_i2c_device_write( soc_cm_dev_t *dev, uint32 addr, uint32 value )
{
    return( (bde->i2c_device_write) ?
            bde->i2c_device_write( dev->dev, addr, value ) : -1 );
}

/*
 * _setup_bus
 *
 *    Utility routine used by _sysconf_probe
 */

static int
_setup_bus( int unit )
{
    uint16 driverDevID;
    uint16 driverRevID;
    const ibde_dev_t *dev = bde->get_dev( unit );
    char *bus_type;
    uint8 revID = dev->rev;

    soc_cm_get_id_driver( dev->device, revID,
                          &driverDevID, &driverRevID );

    switch( bde->get_dev_type( unit ) & BDE_DEV_BUS_TYPE_MASK )
    {
        case BDE_PCI_DEV_TYPE:
            bus_type = "PCI";
            break;
        case BDE_SPI_DEV_TYPE:
            bus_type = "SPI";
            break;
        case BDE_EB_DEV_TYPE:
            bus_type = "EB";
            break;
        case BDE_ICS_DEV_TYPE:
            bus_type = "ICS";
            break;
        case (BDE_SPI_DEV_TYPE|BDE_EB_DEV_TYPE) :
            bus_type = "SPI/EB";
            break;
        case BDE_ET_DEV_TYPE:
            bus_type = "ETH";
            break;
        case BDE_EMMI_DEV_TYPE:
            bus_type = "EMMI";
            break;
        case BDE_AXI_DEV_TYPE:
            bus_type = "AXI";
            break;
       case BDE_I2C_DEV_TYPE:
            bus_type = "I2C";
            break;
        default:
            printf( "Error : Unknown bus type=0x%x !!\n",
                    bde->get_dev_type( unit ) & BDE_DEV_BUS_TYPE_MASK );
            return -1;
    }

    printf( "BDE unit %d (%s), "
            "Dev 0x%04x, Rev 0x%02x, Chip %s, Driver %s\n",
            unit, bus_type,
            dev->device, revID,
            soc_cm_get_device_name( dev->device, revID ),
            soc_cm_get_device_name( driverDevID, driverRevID ) );

    return 0;
}


/*
 * _sysconf_probe
 *
 * Searches for known devices and creates Configuration Manager instances.
 */

static int _sysconf_probe_done;

static int
_sysconf_probe( void )
{
    int u;
    uint16 devID;
    uint8 revID;

    if( _sysconf_probe_done ){
        printf( "_sysconf_probe: cannot probe more than once\n" );
        return -1;
    }

    /*
     * Initialize system BDE
     */
    if( bde_create() ){
        return -1;
    }

    /* Iterate over device */
    for( u = 0; u < bde->num_devices( BDE_ALL_DEVICES ) &&
                u < SOC_MAX_NUM_SWITCH_DEVICES; u++ ){
        const ibde_dev_t *dev = bde->get_dev( u );
        devID = dev->device;
        revID = dev->rev;

        if ( soc_cm_device_supported( devID, revID ) < 0 ){
            /* Not a switch chip; continue probing other devices */
            printf( "ERROR: device 0x%x revision 0x%x is not supported\n",
                    (unsigned)devID, (unsigned)revID );
            return 0;
        }

        if( _setup_bus( u ) < 0 ){
            /*
             * Error message already printed; continue probing other
             * devices
             */
            return 0;
        }

        /*
         * Create a device handle, but don't initialize yet.  This
         * sneakily relies upon the fact that the cm handle = bde
         * handle.
         */
        (void) soc_cm_device_create( devID, revID, NULL );

        _sysconf_probe_done++;
    }

    return 0;
}

static int
_sysconf_attach( int unit )
{
    int bdx = 0; /* BDE handle */

    /* Ready to install into configuration manager */
    soc_cm_device_vectors_t vectors;

    if( soc_cm_dev_num_get( unit, &bdx ) != 0 ){
        printf( "Unable to get BDE handle for unit=%d\n", unit );
        return 0;
    }

    memset( &vectors, 0, sizeof( soc_cm_device_vectors_t ) );

    /* Proceed with BDE device initialization */

    bde->pci_bus_features( bdx, &vectors.big_endian_pio,
                           &vectors.big_endian_packet,
                           &vectors.big_endian_other );
    vectors.config_var_get = _config_var_get;
    vectors.interrupt_connect = _interrupt_connect;
    vectors.interrupt_disconnect= _interrupt_disconnect;
    vectors.base_address = bde->get_dev( bdx )->base_address;
    vectors.read = _read;
    vectors.write = _write;
    vectors.pci_conf_read = _pci_conf_read;
    vectors.pci_conf_write = _pci_conf_write;
    vectors.salloc = _salloc;
    vectors.sfree = _sfree;
    vectors.sinval = _sinval;
    vectors.sflush = _sflush;
    vectors.l2p = _l2p;
    vectors.p2l = _p2l;
    vectors.iproc_read = _iproc_read;
    vectors.iproc_write = _iproc_write;
    vectors.bus_type = bde->get_dev_type( bdx );
    vectors.spi_read = _spi_read;
    vectors.spi_write = _spi_write;
    vectors.i2c_device_read = _i2c_device_read;
    vectors.i2c_device_write = _i2c_device_write;
    if( soc_cm_device_init(unit, &vectors) < 0 ){
        printf( "_sysconf_attach(): CM device init failed for unit=%d\n",
                unit );
        return( -1 );
    }
    return 0;
}

int do_per_switch_setup(int unit)
{
    /* Just an example of things that can be done. */

    /*
     * Set STP state to "forwarding" for all ports that are
     * configured for Ethernet or HiGig encapsulation.
     */
    bcm_port_config_t port_config;
    bcm_port_t port;

    bcm_port_config_get(unit, &port_config);

    BCM_PBMP_ITER(port_config.e, port) {
        BCM_IF_ERROR_RETURN(bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD));
    }

    BCM_PBMP_ITER(port_config.hg, port) {
        BCM_IF_ERROR_RETURN(bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD));
    }

    return 0;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int scache_read_dummy_f( int unit, uint8 *buf, int offset, int nbytes )
{
    return SOC_E_RESOURCE;
}

int scache_write_dummy_f( int unit, uint8 *buf, int offset, int nbytes )
{
    return SOC_E_RESOURCE;
}
#endif

/*
 * Main loop.
 */
int main( int argc, char *argv[] )
{
    int i;
    uint32 flags;
    int rv = BCM_E_NONE;

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

    sal_thread_main_set( sal_thread_self() );

#ifndef NO_SAL_APPL
    bslmgmt_init();
#endif

    soc_cm_init();

    /*
     * At boot time, probe for devices and attach the first one.
     */
    flags = sal_boot_flags_get();

    if( !(flags & BOOT_F_NO_PROBE) ){
        if( _sysconf_probe() < 0 ){
            printf( "ERROR: PCI SOC device probe failed\n" );
        }

        if( !(flags & BOOT_F_NO_ATTACH) ){
            for( i = 0; i < soc_all_ndev; i++ ){
                rv = _sysconf_attach( i );
                printf( "%s: soc_attach( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );

#ifdef BCM_WARM_BOOT_SUPPORT
                SOC_WARM_BOOT_DONE( i );
                rv = soc_stable_set( i, BCM_SWITCH_STABLE_NONE, 0 );
                printf( "%s: soc_stable_set( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );

                rv = soc_stable_size_set( i, 0 );
                printf( "%s: soc_stable_size_set( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );

                rv = soc_switch_stable_register( i,
                                                 scache_read_dummy_f,
                                                 scache_write_dummy_f,
                                                 NULL, NULL );
                printf( "%s: soc_switch_stable_register( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#endif
#ifndef BCM_DNX_SUPPORT
                rv = soc_reset_init( i );
                printf( "%s: soc_reset_init( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#endif /* BCM_DNX_SUPPORT */
                rv = soc_misc_init( i );
                printf( "%s: soc_misc_init( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#ifndef BCM_DNX_SUPPORT
                rv = soc_mmu_init( i );
                printf( "%s: soc_mmu_init( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#endif /* BCM_DNX_SUPPORT */
                rv = bcm_attach( i, NULL, NULL, i );
                printf( "%s: bcm_attach( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#ifdef BCM_DNX_SUPPORT
#ifndef NO_SAL_APPL
                soc_mem_config_set = sal_config_set;
                rv = diag_rc_load(i);
                printf( "%s: diag_rc_load( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#endif /* NO_SAL_APPL */
#endif /* BCM_DNX_SUPPORT */
                rv = bcm_init( i );
                printf( "%s: bcm_init( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );

                printf( "Unit=%d completed initialization.\n", i );
#ifndef BCM_DNX_SUPPORT
                rv = do_per_switch_setup( i );
                printf( "%s: do_per_switch_setup( %d ), result=%d\n",
                        (rv) ? "FAIL" : "SUCCESS", i, rv );
#endif /* BCM_DNX_SUPPORT */
                printf( "Unit=%d completed configuration.\n", i );
            }
        } else {
            printf( "Boot flags: Attach NOT performed\n" );
        }
    } else {
        printf( "Boot flags: Probe NOT performed\n" );
    }

    /* Initialize diag shell so we can use it later on. */
    diag_init();
    cmdlist_init();

#ifndef BCM_DNX_SUPPORT
    sh_process_command( 0, "ps" );
#endif /* BCM_DNX_SUPPORT */

    linux_bde_destroy( bde );
    return 0;
}
