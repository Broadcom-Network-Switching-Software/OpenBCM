/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_CMTYPES_H
#define _SOC_CMTYPES_H

#include <sal/types.h>

#ifdef INCLUDE_RCPU
#include <soc/rcpu.h>
#endif /* INCLUDE_RCPU */

/* maximum number of sub-devices in a composite device */
#define SOC_MAX_NOF_COMPOSITE_SUB_DEVS 4
/* Value specifying this is not a composite device or a sub-device of one */
#define SOC_CM_NONE_COMPOSITE_DEV ((soc_cm_dev_id)(-1))

/* unsigned type that can hold any device id or unit */
typedef uint8 soc_cm_dev_id;

/* information about a device type */
typedef struct soc_cm_device_info_s {
    uint16          dev_id;	       /* Chip ID as found in table */
    uint16          rev_id;        /* revision ID and flags */
    const char     *dev_name;
    uint16          dev_id_driver;     /* Chip ID of driver to use */
    uint8           rev_id_driver;     /* (zero if same as real ID) */
    uint32          dev_type;
} soc_cm_device_info_t;

/* information about a composite device type */
typedef struct {
    soc_cm_device_info_t  info;         /* used fields of this structure: dev_name and dev_type */
    const uint32          comp_dev_id;  /* Device ID as specified in the composite_units soc property */
    const soc_cm_dev_id   nof_sub_devs; /* The number of sub devices: 1 < nof <= SOC_MAX_NOF_COMPOSITE_SUB_DEVS */
} soc_cm_comp_device_info_t;

typedef void (*soc_cm_isr_func_t)(void *data);

/* information about a specific CM device */
typedef struct soc_cm_dev_s {
    int             dev;  /* BDE handle or DRD handle */
    const soc_cm_device_info_t *info;
    void           *cookie;
    uint16          dev_id;	       /* True PCI chip ID */
    uint8           rev_id;
    /* For composite devices, this is the index in cm_composite_devices. */
    /* For sub devices, is the index  in soc_cm_device of the main device. */
    /* Otherwise SOC_CM_NONE_COMPOSITE_DEV */
    soc_cm_dev_id   composite_index;
} soc_cm_dev_t;

/* information for a specific composite device */
typedef struct soc_cm_composite_dev_s {
    const soc_cm_comp_device_info_t *dev_type;              /* The type of the composite device */
    soc_cm_dev_id sub_devs[SOC_MAX_NOF_COMPOSITE_SUB_DEVS]; /* sub device indices in soc_cm_device */
} soc_cm_composite_dev_t;

typedef struct soc_cm_device_vectors_s {
    /* Specify if the vectors are initialized */
    int init;

    /* PCI Specific Endian Configuration */
    int big_endian_pio;
    int big_endian_packet;
    int big_endian_other;

    /* Communication bus type */
    uint32 bus_type;

#ifdef INCLUDE_RCPU
    rcpu_trans_ptr_t    *rcpu_tp;
#endif /* INCLUDE_RCPU */

    /* Startup Register Initialization */
    /* This should be changed to return a uint32 */

    char *(*config_var_get)(soc_cm_dev_t *dev, const char *name);

    /*
     * POLLING/INTERRUPT MANAGEMENT
     * --------------------
     * These routines provide the ability to install and remove
     * an interrupt handler for this device. 
     */

    /*
     * Syntax:    int interrupt_connect(int dev, cm_isr_func_t handler, 
     *                                  void *data)
     *
     * Purpose:   Install an interrupt handler for this device
     *
     * Parameters: 
     *            dev     - The device handle (from cm_device_create())
     *            handler - The interrupt handler function. 
     *                      It must be called with the 'data' argument. 
     *            data    - Argument to be used when calling the handler
     *                      function at interrupt time.
     *
     * Returns:   0 if no errors, otherwise -1. 
 */		   
    int (*interrupt_connect)(soc_cm_dev_t *dev, 
			     soc_cm_isr_func_t handler, void *data);
  
    /*
     * Syntax:   int interrupt_disconnect(int dev)
     *
     * Purpose:  remove an interrupt handler for this device
     *
     * Parameters: 
     *            dev     - The device handle (from cm_device_create())
     *
     * Returns:   0 if no errors, otherwise -1. 
 */
    int (*interrupt_disconnect)(soc_cm_dev_t *dev);


    /*
     * REGISTER ACCESS
     * ---------------
     */

    /* 
     * Syntax:     sal_vaddr_t base_address;
     * Purpose:    provides the ability to add an offset to 
     *             device addresses before accessors are called. 
     * 
     * If adding a base address to register offsets makes your
     * accessor functions easier, set it here. The value of base_address
     * will be added to all address offsets.  
     */
    sal_vaddr_t       base_address;

    /*
     * Syntax:    uint32 read(int dev, uint32 addr);
     *
     * Purpose:   Reads a device register. 
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Register offset to read from. The value
     *                      of 'base_address' above will be added to 
     *                      the offset. 
     *
     * Returns:   the 32 bit register value. 
 */
    uint32 (*read)(soc_cm_dev_t *dev, uint32 addr);

    /*
     * Syntax:    void write(int dev, uint32 addr, uint32 data);
     *
     * Purpose:   Writes a device register
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Register offset of the write. The value
     *                      of 'base_address' above will be added to 
     *                      the offset. 
     *            data    - 32 bit register data to be written. 
     *
     * Returns:   Nothing
 */
    void (*write)(soc_cm_dev_t *dev, uint32 addr, uint32 data);
  
    /*
     * Syntax:    uint32 pci_conf_read(int dev, uint32 addr);
     *
     * Purpose:   Reads a PCI config register. 
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Register offset to read from.
     *
     * Returns:   the 32 bit register value. 
 */
    uint32 (*pci_conf_read)(soc_cm_dev_t *dev, uint32 addr);


    /*
     * Syntax:    void pci_conf_write(int dev, uint32 addr, uint32 data);
     *
     * Purpose:   Writes a device PCI configregister
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - register offset of the write.
     *            data    - 32 bit register data to be written. 
     *
     * Returns:   Nothing
 */
    void (*pci_conf_write)(soc_cm_dev_t *dev, uint32 addr, uint32 data);

    /*
     * SHARED MEMORY (DMA) ALLOCATION
     * ------------------------------
     * Data communication between the driver and the device
     * is often accomplished through shared memory buffers. 
     * These buffers must usually be allocated in a special 
     * way to make them shareable between the system and the device. 
     * When the SOC driver needs to allocate on of these buffers, 
     * It will call these functions. 
     */

    /*
     * Syntax:    void *salloc(int dev, int size, name);
     *
     * Purpose:   Allocates a region of shareable (DMA) memory.
     *
     * Parameters: 
     *            dev     - The device handle (from cm_device_create())
     *            size    - The amount of memory to allocate. 
     *
     * Returns:   A pointer to the memory if successful. 
     *            NULL if a failure occurs. 
 */
    void *(*salloc)(soc_cm_dev_t *dev, int size, const char *name);

    /*
     * Syntax:    void sfree(int dev, void *ptr); 
     * 
     * Purpose:   Deallocate a region of shareable (DMA) memory. 
     * 
     * Parameters: 
     *            dev     - The device handle (from cm_device_create())
     *            ptr     - A pointer to the memory region. 
     *
     * Returns:   Nothing
 */
    void  (*sfree)(soc_cm_dev_t *dev, void *ptr);


    /* Flush/Invalidate shared (DMA) memory */
    int (*sflush)(soc_cm_dev_t *dev, void *addr, int length);
    int (*sinval)(soc_cm_dev_t *dev, void *addr, int length);


    /* 
     * ADDRESS TRANSLATION
     *
     * Depending upon architecture, the address space the driver deals
     * with may be different from the address space the device deals
     * with. For this reason, and if this translation is required to
     * communicate with this device, you must provide the translation
     * functions to map between the two domains.
 */

    /*
     * Syntax:    uint32 l2p(soc_cm_dev_t *dev, void *addr);
     *
     * Purpose:   Translate the logical address the driver uses to a
     *		  physical address the device can access.
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Logical address to translate. 
     *
     * Returns:   32 bit physical address the device can handle. 
     *
     * NOTE:      The address to translate will always be within a region 
     *            of memory allocated by the salloc() function above.
     */
    sal_paddr_t (*l2p)(soc_cm_dev_t *dev, void *addr);
  
    /*
     * Syntax:    void *p2l(soc_cm_dev_t *dev, uint32 addr)
     *
     * Purpose:   Translate the physical address the driver uses to a
     *		  logical address the driver can access
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Physical device address
     *
     * Returns:   Logical driver address
 */
    void *(*p2l)(soc_cm_dev_t *dev, sal_paddr_t addr);

    /*
     * Syntax:    int spi_read(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len);
     *
     * Purpose:   Read a device register via spi interface
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Register offset of the write.
     *            data    - Pointer to the buffer to be read. 
     *            len     - Length of total data to be read.
     *
     * Returns:   -1 if error, otherwise 0 (or actual bytes read)
 */
    int  (*spi_read)(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len);

    /*
     * Syntax:    int spi_write(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len);
     *
     * Purpose:   Writes a device register via spi interface
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Register offset of the write. The value
     *                      of 'base_address' above will be added to 
     *                      the offset. 
     *            data    - Pointer to the buffer to be written. 
     *            len     - Length of total data to be written.
     *
     * Returns:   -1 if error, otherwise 0 (or actual bytes written)
 */
    int  (*spi_write)(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len);

    /* 
     * INTERNAL MICROCONTROLLER ACCESS
     *
     * Some switch devices contain an embedded microcontroller system,
     * and in certain situations the switch driver will need to access
     * microcontroller registers.  If the switch driver is running on
     * an external CPU then it may need to share memory with the
     * internal processor(s) as well.
 */

    /*
     * Purpose:   Reads an iProc device register. 
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Absolute register address in iProc address space.
     *
     * Returns:   The 32 bit register value
 */
    uint32 (*iproc_read)(soc_cm_dev_t *dev, uint32 addr);

    /*
     * Purpose:   Writes an iProc device register
     *
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Absolute register address in iProc address space.
     *            data    - 32 bit register data to be written. 
     *
     * Returns:   0 if no errors, otherwise -1. 
 */
    void (*iproc_write)(soc_cm_dev_t *dev, uint32 addr, uint32 data);

    /*
     * Purpose:   Reads data from shared memory region. 
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Address of shared memory region to read from.
     *            buf     - (OUT) Buffer to store data
     *            len     - Number of bytes to read
     *
     * Returns:   Number of bytes read, or -1 if error.
 */
    int (*shmem_read)(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, uint32 len);

    /*
     * Purpose:   Writes data to shared memory region. 
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Address of shared memory region to write to.
     *            buf     - Buffer of data to write
     *            len     - Number of bytes to write
     *
     * Returns:   Number of bytes written, or -1 if error.
 */
    int (*shmem_write)(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, uint32 len);

    /*
     * Purpose:   Maps a shared memory region. 
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - Address of shared memory region.
     *            size    - Size of region to map
     *
     * Returns:   Pointer to shared memory region, or NULL if error.
 */
    sal_vaddr_t (*shmem_map)(soc_cm_dev_t *dev, uint32 addr, uint32 size);

    /*
     * Purpose:   Read a 4 byte word from the device using an I2C connection to the device.
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - The address to access in the internal device address space.
     *            value   - A pointer for storing the value to be read.
     *
     * Returns:   Zero on success.
     */
    int (*i2c_device_read)(soc_cm_dev_t *dev, uint32 addr, uint32 *value);

    /*
     * Purpose:   Write a 4 byte word to the device using an I2C connection to the device.
     * 
     * Parameters:
     *            dev     - The device handle (from cm_device_create())
     *            addr    - The address to access in the internal device address space.
     *            value   - The value to be written.
     *
     * Returns:   Zero on success.
     */
    int (*i2c_device_write)(soc_cm_dev_t *dev, uint32 addr, uint32 value);

} soc_cm_device_vectors_t;

#define init_register_get config_var_get	/* compat. */

#endif	/* !_SOC_CMTYPES_H */
