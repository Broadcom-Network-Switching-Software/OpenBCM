/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C access from the CPU
 */

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/i2c.h>

#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__) || defined(INCLUDE_CPU_I2C)

#include <sal/appl/i2c.h>


#define CPU_I2C_MAX_ADDRESS_SIZE 4 /* Maximum number of bytes of an address in an I2C device (register address) */

uint8 cpu_i2c_bus_num_default = CPU_I2C_BUS_NUM_DEFAULT;

int cpu_i2c_write(int chip, int addr, CPU_I2C_BUS_LEN alen, int val)
{
    int ret, bus_num = CPU_I2C_BUS_NUM_DEFAULT;
    uint32 value;
    uint8 data_len = alen & 0xf, addr_len = (alen >> 4) & 0xf, i, data[CPU_I2C_MAX_ADDRESS_SIZE] = {0};

    LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META(
      "cpu_i2c_write: bus:%d dev:@%02x reg:%0x data:%x\n"), bus_num, chip, addr, val));

    switch (alen)
    {
        /* No internal/register address */
        case CPU_I2C_ALEN_NONE_DLEN_BYTE:
        case CPU_I2C_ALEN_NONE_DLEN_WORD:
        case CPU_I2C_ALEN_NONE_DLEN_LONG:
        case CPU_I2C_ALEN_BYTE_DLEN_BYTE: /* address is 1 byte long */
        case CPU_I2C_ALEN_WORD_DLEN_WORD: /* address is word (2 bytes) long */
        case CPU_I2C_ALEN_BYTE_DLEN_LONG: /* address is 1 bytes long, data is 4 byte long*/
        case CPU_I2C_ALEN_WORD_DLEN_LONG: /* address is 1 bytes long, data is 4 byte long*/
        case CPU_I2C_ALEN_LONG_DLEN_LONG: /* address is 4 bytes long */
        case CPU_I2C_ALEN_BYTE_DLEN_WORD: /* address is 1 bytes long, data is 2 bytes long */
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_I2C, (BSL_META(
              "error - unsupported address and data length given = 0x%x"), (unsigned)alen));
            return SOC_E_PARAM;
    }

    /* build data as big endian */
    value = val;
    for (i = data_len ; i > 0 ;) {
        data[--i] = value & 0xff;
        value >>= 8;
    }

    /* Perform an I2C write operation */
    ret = sal_i2c_write(CPU_I2C_BUS_NUM_DEFAULT, chip, (uint32)addr, addr_len, data, data_len);

    if (ret != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SOC_I2C, (BSL_META(
          "Error in %s: cpu_i2c_write() returned with error.\n"), FUNCTION_NAME()));
    }

    return ret;
}

/* perform an I2C read operation by the CPU card */
int cpu_i2c_read_extended(int chip, int addr, CPU_I2C_BUS_LEN alen, int* p_val, int donot_print_errors)
{
    int ret = 0;
    int bus_num = CPU_I2C_BUS_NUM_DEFAULT;
    uint32 value;
    uint8 data_len = alen & 0xf, addr_len = (alen >> 4) & 0xf, i, result[CPU_I2C_MAX_ADDRESS_SIZE] = {0};

    LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META(
      "cpu_i2c_read: bus:%d dev:@%02x reg:%0x\n"), bus_num, chip, addr));

    switch (alen)
    {
        /* No internal/register address */
        case CPU_I2C_ALEN_NONE_DLEN_BYTE:
        case CPU_I2C_ALEN_NONE_DLEN_WORD:
        case CPU_I2C_ALEN_NONE_DLEN_LONG:

        case CPU_I2C_ALEN_BYTE_DLEN_BYTE: /* address is 1 byte long */
        case CPU_I2C_ALEN_WORD_DLEN_WORD: /* address is word (2 bytes) long */
        case CPU_I2C_ALEN_BYTE_DLEN_LONG: /* address is 1 bytes long, data is 4 byte long*/
        case CPU_I2C_ALEN_WORD_DLEN_LONG: /* address is 1 bytes long, data is 4 byte long*/
        case CPU_I2C_ALEN_LONG_DLEN_LONG: /* address is 4 bytes long */
        case CPU_I2C_ALEN_BYTE_DLEN_WORD: /* address is 1 bytes long, data is 2 bytes long */
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_I2C, (BSL_META(
              "error - unsupported address and data length given = 0x%x"), (unsigned)alen));
            return SOC_E_PARAM;
    } /* switch*/

    /* Perform an I2C read operation */
    ret = sal_i2c_read(CPU_I2C_BUS_NUM_DEFAULT, chip, (uint32)addr, addr_len, result, data_len);

    /* build results value according received as big endian */
    value = 0;
    if (ret == SOC_E_NONE) {
        for (i = 0 ; i < data_len; ++i) {
            value = (value << 8) | result[i];
        }
        LOG_INFO(BSL_LS_SOC_I2C, (BSL_META(
          "cpu_i2c_read: bus:%d dev:@%02x reg:%0x data:%x\n"), bus_num, chip, addr, value));
    } else if (!donot_print_errors) {
        LOG_ERROR(BSL_LS_SOC_I2C, (BSL_META("cpu_i2c_read() returned with error.\n")));
    }
    *p_val = value;
    return ret;
}

int cpu_i2c_read(int chip, int addr, CPU_I2C_BUS_LEN alen, int* p_val)
{
  return cpu_i2c_read_extended(chip, addr, alen, p_val, 0);
}

/* perform an I2C read operation on an I2C device connected to the CPU */
soc_error_t cpu_i2c_read_generic(
  uint8 i2c_bus, /* The number of the I2C bus to access on */
  uint16 i2c_dev,/* the device on the I2C bus (slave address) to be accessed */
  uint8 alen,    /* the length of the address in bytes, should be between 0 and 4. */
  uint8 dlen,    /* the length of the data to read. */
  uint32 addr,   /* The address in the device to be accessed (register address). Used if alen>0 */
  uint8 *data)   /* the buffer for the data to be read. */
{
    return sal_i2c_read(i2c_bus, i2c_dev, addr, alen, data, dlen);
}

/* perform an I2C write operation on an I2C device connected to the CPU */
soc_error_t cpu_i2c_write_generic(
  uint8 i2c_bus, /* The number of the I2C bus to access on */
  uint16 i2c_dev,/* the device on the I2C bus (slave address) to be accessed */
  uint8 alen,    /* the length of the address in bytes, should be between 0 and 4. */
  uint8 dlen,    /* the length of the data to read. */
  uint32 addr,   /* The address in the device to be accessed (register address). Used if alen>0 */
  uint8 *data)   /* the buffer for the data to be read. */
{
    return sal_i2c_write(i2c_bus, i2c_dev, addr, alen, data, dlen);
}



/*
 * Function: cpu_i2c_device_present
 * Purpose: Probe the I2C bus using saddr, report if a device responds.
 *          The I2C bus is released upon return.
 *          No further action is taken.
 *
 * Parameters:
 *    saddr - I2C slave address
 *
 * Return:
 *     SOC_E_NONE - An I2C device responds at the indicated saddr.
 *     otherwise  - No I2C response at the indicated saddr, or I/O error.
 */
int
cpu_i2c_device_present(int bus, i2c_saddr_t saddr) {
    return sal_i2c_device_present(bus, saddr);
}

/*
 * Function: cpu_i2c_block_read
 * Purpose: Read a block of data from I2c device
 *
 * Parameters:
 *    bus  - cpu controller to use
 *    saddr  - slave address
 *    addr -  interal starting address, only 8bit addressing as of now
 *    data  - pointer to buffer
 *    len   - len of data to be read
 * Return:
 *     SOC_E_NONE - An I2C device responds at the indicated saddr.
 *     otherwise  - No I2C response at the indicated saddr, or I/O error.
 */
int
cpu_i2c_block_read(int bus, i2c_saddr_t saddr, 
                   uint8 reg, uint8 *data, uint8 *len)
{

    int rv;

    if (*len == 0) {
        LOG_ERROR(BSL_LS_SOC_I2C,
                 (BSL_META("ERROR in cpu_i2c_block_read: invalid len specified (%d)"),
                  *len));
        return SOC_E_PARAM;
    }
    rv = sal_i2c_read(bus, saddr, reg, 1, data, *len);
  
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_I2C,
                 (BSL_META("\nFailed reading %d bytes data from %x at device saddr %x"),
                  *len, reg, saddr));
        return rv;
    }
    return SOC_E_NONE;
}

/*
 * Function: cpu_i2c_block_write
 * Purpose: Write a block of data from I2c device
 *
 * Parameters:
 *    bus  - cpu controller to use
 *    saddr  - slave address
 *    addr -  interal starting address, only 8bit addressing as of now
 *    data  - pointer to buffer
 *    len   - len of data to be read
 * Return:
 *     SOC_E_NONE - An I2C device responds at the indicated saddr.
 *     otherwise  - No I2C response at the indicated saddr, or I/O error.
 */
int
cpu_i2c_block_write(int bus, i2c_saddr_t saddr, 
                   uint8 reg, uint8 *data, uint8 len)
{
    int rv = sal_i2c_write_split(bus, saddr, reg, 1, data, len);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_I2C,
                 (BSL_META("\nFailed reading %d bytes data from %x at device saddr %x"),
                  len, reg, saddr));
        return rv;
    }
    return SOC_E_NONE;
}


/* Write to the internal device Address space using I2C */
soc_error_t cpu_i2c_device_read(
  uint8 i2c_bus, /* The number of the I2C bus to access on */
  uint8 i2c_dev, /* the device on the I2C bus (slave address) to be accessed */
  uint32 addr,   /* The address to access in the internal device address space */
  uint32 *value) /* the value to be read. */
{
    unsigned i;
    uint8 data[sizeof(*value)];

    soc_error_t ret = sal_i2c_read(i2c_bus, i2c_dev, addr, sizeof(addr), data, sizeof(*value));
    /* build value according received as big endian */
    *value = 0;
    for (i = 0 ; i < sizeof(*value); ++i) {
        *value = (*value << 8) | data[i];
    }
    return ret;
}

/* Write to the internal device Address space using I2C */
soc_error_t cpu_i2c_device_write(
  uint8 i2c_bus, /* The number of the I2C bus to access on */
  uint8 i2c_dev, /* the device on the I2C bus (slave address) to be accessed */
  uint32 addr,   /* The address to access in the internal device address space */
  uint32 value)  /* the value to be written. */
{
    unsigned i;
    uint8 data[sizeof(value)];

    /* copy the address to data as big endian */
    for (i = sizeof(value) ; i > 0 ; ) {
        data[--i] = value & 0xff;
        value >>= 8;
    }
    /* The device returns an error on writes, even though they are performed */
    sal_i2c_write(i2c_bus, i2c_dev, addr, sizeof(addr), data, sizeof(value));
    return SOC_E_NONE; /* ignore errors */
}


#else
typedef int _src_soc_i2c_cpu_c_not_empty; /* Make ISO compilers happy. */
#endif  /* defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__) || defined(INCLUDE_CPU_I2C) */
