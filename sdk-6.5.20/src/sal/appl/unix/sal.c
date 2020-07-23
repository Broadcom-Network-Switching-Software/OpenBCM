/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	sal.c
 * Purpose:	Defines sal routines for user-mode Unix targets.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>

#include <sal/core/time.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/boot.h>
#include <sal/core/spl.h>
#include <sal/core/alloc.h>

#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/appl/config.h>
#include <shared/error.h>

#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
/* { */
#include <sal/core/alloc.h>
/* } */
#endif
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

#if defined (__STRICT_ANSI__)
#define RANDOM  rand
#define SRANDOM srand
#else
#define RANDOM  random
#define SRANDOM srandom
#endif

#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && !defined(INCLUDE_CPU_I2C)
#define INCLUDE_CPU_I2C
#endif

#ifdef INCLUDE_CPU_I2C
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#ifndef I2C_SMBUS_QUICK
#include <linux/i2c.h>
#endif
#include <sys/ioctl.h>      /* ioctl */

#define SAL_I2C_DEVICE_FILE_NAME_PREFIX "/dev/i2c-" /* i2c device is SAL_I2C_DEVICE_FILE_NAME_PREFIX + device number */
#define SAL_I2C_MAX_NAME_LENGTH 64
#define SAL_I2C_MAX_NOF_SLAVE_DEVS 128 /* Maximum number of slave devices (Addresses) on an I2C bus, numbered from 0 */
#define SAL_I2C_MAX_ADDRESS_SIZE 4 /* Maximum number of bytes of an address in an I2C device (register address) */

/* flags for sal_i2c_operation() */
#define SAL_I2C_FLAGS_WRITE 1
#define SAL_I2C_FLAGS_WRITE_SPLIT 2 /* Split the address and data writes */
#define SAL_I2C_FLAGS_IGNORE_UNKNOWN_DEV_ERR_ON_WRITE 4
#define SAL_I2C_MAX_NOF_BUSES 8 /* Maximum number of I2C buses supported which are consecutively numbered from 0 */
#endif /* INCLUDE_CPU_I2C */


/*
 * Function:
 *	sal_flash_sync
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failed
 * Notes:
 *	Not supported for Unix.
 */

int
sal_flash_sync(void)
{
    return(-1);
}

/*
 * Function:
 *	sal_flash_init
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failed
 * Notes:
 *	Not supported for Unix.
 */

int
sal_flash_init(int format)
{
    return(-1);
}

/*
 * Function:
 *	sal_flash_boot
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failed
 * Notes:
 *	Not supported for Unix.
 */

int
sal_flash_boot(char *f)
{
    return(-1);
}

/*
 * Function:
 *	sal_appl_init
 * Purpose:
 *	Initialize the SAL abstraction layer for Unix
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failed
 */

int
sal_appl_init(void)
{
#ifndef NO_FILEIO
    char		start_cwd[256];

    sal_getcwd(start_cwd, sizeof (start_cwd));
    sal_homedir_set(start_cwd);
#endif

    sal_console_init();
    sal_config_refresh();

    DISPLAY_MEM ;
    return(0);
}

/*
 * Function:
 *	sal_reboot
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	None
 * Notes:
 *	Causes the Unix application to exit.
 */

void
sal_reboot(void)
{
    exit(0);
}

/*
 * Function:
 *	sal_shell
 * Purpose:
 *	Fork a standard O/S shell.
 * Parameters:
 *	None
 */

void
sal_shell(void)
{
    char *s = getenv("SHELL");
    /*    coverity[tainted_string]    */
    system(s ? s : "/bin/sh");
}

/*
 * Function:
 *   sal_shell_cmd
 * Purpose:
 *   Execute a standard O/S shell command.
 * Parameters:
 *   cmd - commands to be executed
 */

void
sal_shell_cmd(const char *cmd)
{
    system(cmd);
}

/*
 * Function:
 *	sal_led
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	v - pattern to show on LED display
 * Returns:
 *	Previous state of LEDs.
 * Notes:
 *	Not supported for Unix.
 */

uint32
sal_led(uint32 v)
{
    static uint32	led = 0;
    uint32		led_prev;

    led_prev = led;
    led = v;

    return led_prev;
}

/*
 * Function:
 *	sal_led_string
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	s - string to show on LED display
 * Notes:
 *	Not supported for Unix.
 */

void
sal_led_string(const char *s)
{
    COMPILER_REFERENCE(s);
}

/*
 * Function:
 *	sal_date_set
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	val - new system time
 * Returns:
 *	0 - success
 *	-1 - failure
 * Notes:
 *	Not supported for Unix.
 */

int
sal_date_set(sal_time_t *val)
{
    sal_printf("Don't know how to set date on this platform (%lu)\n", *val);
    return -1;
}

/*
 * Function:
 *	sal_date_get
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failure
 */

int
sal_date_get(sal_time_t *val)
{
    time((time_t *) val);
    return 0;
}

#if !(defined(UNIX) && !defined(__STRICT_ANSI__) && !defined(USE_CUSTOM_STR_UTILS))
/*
 * Function:
 *	sal_strcasecmp
 * Purpose:
 *	Compare two strings ignoring the case of the characters.
 * Parameters:
 *	s1 - first string to compare
 *	s2 - second string to compare
 * Returns:
 *	0 if s1 and s2 are identical.
 *	negative integer if s1 < s2.
 *	positive integer if s1 > s2.
 * Notes
 *	See man page of strcasecmp for more info.
 */

int
sal_strcasecmp(const char *s1, const char *s2)
{
    unsigned char c1, c2;

    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while (c1 == c2 && c1 != 0);

    return c1 - c2;
}

/*
 * Function:
 *	sal_strncasecmp
 * Purpose:
 *	Compare two strings ignoring the case of the characters.
 * Parameters:
 *	s1 - first string to compare
 *	s2 - second string to compare
 *	n - maximum number of characters to compare
 * Returns:
 *	0 if s1 and s2 are identical up to n characters.
 *	negative integer if s1 < s2 up to n characters.
 *	positive integer if s1 > s2 up to n characters.
 * Notes
 *	See man page of strncasecmp for more info.
 */

int
sal_strncasecmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1, c2;

    if (n == 0) {
        return 0;
    }
    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while (--n && c1 == c2 && c1 != 0);

    return c1 - c2;
}

#endif /* !(defined(UNIX) && !defined(__STRICT_ANSI__)) */
/*
 * Function:
 *  sal_strcasestr
 * Purpose:
 *  Finds the first occurrence of the substring needle in the string haystack
 * Parameters:
 *  haystack - string to be searched in
 *  needle   - substring to be searched for in haystack
 * Returns:
 *  These functions return a pointer to the beginning of the substring
 *  or NULL if the substring is not found.
 * Notes
 *  See man page of strncasestr for more info.
 */

char *
sal_strcasestr(const char *haystack, const char *needle)
{
    char c, sc;
    size_t len;

    if ((c = *needle++) != 0) {
        c = tolower((unsigned char)c);
        len = strlen(needle);
        do {
            do {
                if ((sc = *haystack++) == 0)
                    return (NULL);
            } while ((char)tolower((unsigned char)sc) != c);
        } while (sal_strncasecmp(haystack, needle, len) != 0);
        haystack--;
    }
    return ((char *)haystack);
}


/* Code for supporting I2C operations on I2C devices whose master is the CPU */

#ifdef INCLUDE_CPU_I2C
/* File descriptor for each I2C bus the CPU is connected to, a positive value means it is open */
static int i2c_bus_filedesc[SAL_I2C_MAX_NOF_BUSES] = {0};

/* open a Linux device file for an I2C bus */
static int sal_i2c_init_fd(int* p_fd, uint8 bus_num)
{
  char dev_file_name[SAL_I2C_MAX_NAME_LENGTH];

  sal_snprintf(dev_file_name, SAL_I2C_MAX_NAME_LENGTH, SAL_I2C_DEVICE_FILE_NAME_PREFIX"%d", (int)bus_num);
  
  /* Try to open the given i2c dev num */
  if ( ((*p_fd) = open(dev_file_name, O_RDWR)) <= 0)
  {
      sal_printf("ERROR in %s: Failed to open() I2C device file for bus number %u\n",
                 FUNCTION_NAME(), (unsigned)bus_num);
      return _SHR_E_FAIL;
  }
  return _SHR_E_NONE;
}

/* close a Linux device file for an I2C bus */
static int sal_i2c_deinit_fd(int *fd)
{
  /* close the I2C device */
  if (*fd > 0) {
      if (close(*fd)) {
          return _SHR_E_NOT_FOUND;
      }
      *fd = -1;
  }
  
  return _SHR_E_NONE;
}

/* Init CPU I2C */
int sal_i2c_init()
{
    return _SHR_E_NONE;
}

/* De-init CPU I2C */
int sal_i2c_deinit()
{
    int i, ret = _SHR_E_NONE;
    for (i = 0; i < SAL_I2C_MAX_NOF_BUSES; ++i) {
        ret |= sal_i2c_deinit_fd(i2c_bus_filedesc + i);
    }
    return ret ? _SHR_E_NOT_FOUND : _SHR_E_NONE;
}


/*
 * Perform an I2C access operation on an I2C device connected to the CPU.
 * The device file for the I2C bus needs to already be open.
 */
static int sal_i2c_operation(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev, /* the device on the I2C bus (slave device ID/address) to be accessed */
  uint8 alen,    /* the length of the address in bytes, should be between 0 and 4. */
  uint8 dlen,    /* the length of the data to read. */
  uint32 addr,   /* The address in the device to be accessed (register address). Used if alen>0 */
  uint8 *data,   /* the buffer for the data to be read. */
  uint32 flags)  /* flags affecting function operation specifically SAL_I2C_FLAGS_WRITE indicated this is a write operation */
{
    int i;
    unsigned bus = i2c_bus;
    uint32 value;
    struct i2c_rdwr_ioctl_data rw_data;
    struct i2c_msg i2c_rw_msg[2];
    uint8 addr_n_data[SAL_I2C_MAX_ADDRESS_SIZE + 256]; /* buffer to hold the address and for writes also the write data */
    /*
     * Initialize variables which are transferred to ioctl() below
     * because Valgrind requires all input to ioctl() to be initialized.
     * What we may have here is 'alignment holes' (e.g., in 64 bits CPUs)
     */
    sal_memset(i2c_rw_msg,0, sizeof(i2c_rw_msg));
    sal_memset(&rw_data,0, sizeof(rw_data));
    if (bus >= SAL_I2C_MAX_NOF_BUSES || i2c_dev >= SAL_I2C_MAX_NOF_SLAVE_DEVS || alen > SAL_I2C_MAX_ADDRESS_SIZE) {
        sal_printf("I2c bus %u or I2C device 0x%x or address length %d out of range.\n",
                   bus, (unsigned)i2c_dev, (unsigned)alen);
        return _SHR_E_PARAM;
    }

    /* open the given i2c bus device file if it is not open */
    if (i2c_bus_filedesc[bus] <= 0) {
        if (sal_i2c_init_fd(i2c_bus_filedesc + bus, bus) != _SHR_E_NONE || i2c_bus_filedesc[bus] <= 0) {
            sal_printf("Failed to auto-open the device file.\n");
            return _SHR_E_FAIL;
        }
    }

    /* copy the address to addr_n_data as big endian */
    value = addr;
    for (i = alen ; i > 0 ; ) {
        addr_n_data[--i] = value & 0xff;
        value >>= 8;
    }

    /* init the i2c access struct */
    rw_data.msgs = &i2c_rw_msg[0];
    i2c_rw_msg[0].addr  = i2c_dev;
    i2c_rw_msg[0].flags = 0; /* I2C_M_WR */
    i2c_rw_msg[0].buf   = &addr_n_data[0];
    if (flags & SAL_I2C_FLAGS_WRITE) {
        if (flags & SAL_I2C_FLAGS_WRITE_SPLIT) { /* block write */
            rw_data.nmsgs = 2; /* two WRITE operations */
            i2c_rw_msg[0].len     = alen;
            i2c_rw_msg[1].addr    = i2c_dev;
            i2c_rw_msg[1].flags   = I2C_M_NOSTART;
            i2c_rw_msg[1].buf     = data;     /* write from input buffer */
            i2c_rw_msg[1].len     = dlen;
        } else {
            rw_data.nmsgs = 1; /* one WRITE operation */
            i2c_rw_msg[0].len = alen + dlen;
            sal_memcpy(addr_n_data + alen, data, dlen); /* copy write data after the address */
        }
    } else { /* read */
        if (alen) { /* We have an address so we need to first write it and then read */
            rw_data.nmsgs = 2;                /* WRITE + READ operations */
            i2c_rw_msg[0].len     = alen;
            i2c_rw_msg[1].addr    = i2c_dev;
            i2c_rw_msg[1].flags   = I2C_M_RD;
            i2c_rw_msg[1].buf     = data;     /* read to input buffer */
            i2c_rw_msg[1].len     = dlen;
        } else { /* No address, only read */
            rw_data.nmsgs = 1;                /* one READ operation */
            i2c_rw_msg[0].flags   = I2C_M_RD;
            i2c_rw_msg[0].buf     = data;     /* read to input buffer */
            i2c_rw_msg[0].len     = dlen;
        }
    }

    /* Perform the I2C messages/operations */
    while (ioctl(i2c_bus_filedesc[bus], I2C_RDWR, &rw_data) < 0) {
        if (errno == ENXIO &&
          (flags & (SAL_I2C_FLAGS_WRITE | SAL_I2C_FLAGS_IGNORE_UNKNOWN_DEV_ERR_ON_WRITE)) ==
          (SAL_I2C_FLAGS_WRITE | SAL_I2C_FLAGS_IGNORE_UNKNOWN_DEV_ERR_ON_WRITE)) {
            break;
        }
        /* sal_printf("ioctl I2C_RDWR failed in %s, %d, %s "
           "- error reading from bus %u slave device 0x%x, address 0x%x, error = %d\n",
           __FILE__, __LINE__, FUNCTION_NAME(), bus, (unsigned)i2c_dev, addr, errno); */
        if (errno != EINTR) {
            return _SHR_E_FAIL;
        }
    }
#ifdef CPU_I2C_DUMP_MESSAGES
    {
        unsigned j, k;
        struct i2c_msg *p = rw_data.msgs;
        sal_printf("I2C %s ioctl(%d,", flags & SAL_I2C_FLAGS_WRITE ? "Write":"Read", i2c_bus_filedesc[bus]);
        for (j = rw_data.nmsgs; j > 0; --j, ++p) {
            sal_printf(" [addr=0x%x flags=0x%x len=%u, buf=0x", (unsigned)p->addr, (unsigned)p->flags, (unsigned)p->len);
            for (k = 0; k < p->len; ++k ) {
                sal_printf("%.2x", (unsigned)p->buf[k]);
            }
            sal_printf("]");
        }
        sal_printf("\n");
    }
#endif /* CPU_I2C_DUMP_MESSAGES */

    /* In read operations the output is now in the data buffer */
    return _SHR_E_NONE;
}

#endif /* INCLUDE_CPU_I2C */

/*
 * Perform an I2C operations on an I2C device connected to the CPU's I2C controller.
 * The CPU is the I2C master.
 */

/* 
 * Function     : sal_i2c_read
 * Purpose      : Provide i2c bus read functionality using CPU i2c controller
 * Returns      : 0 - success, otherwise - failure
 */
int
sal_i2c_read(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev, /* the device on the I2C bus (slave device ID/address) to be accessed */
  uint32 addr,    /* The address in the device to be accessed (register address). Used if alen>0 */
  uint8 addr_len, /* the length of the address in bytes, should be between 0 and 4. */
  uint8 *data,    /* the buffer for the data to be read. */
  uint8 data_len) /* the length of the data to read. */
{
#ifdef INCLUDE_CPU_I2C
    return sal_i2c_operation(i2c_bus, i2c_dev, addr_len, data_len, addr, data, 0);
#else  /* INCLUDE_CPU_I2C */
    return _SHR_E_UNAVAIL;
#endif /* INCLUDE_CPU_I2C */
}

/* 
 * Function     : sal_i2c_write
 * Purpose      : Provide i2c bus write functionality using CPU i2c controller    
 * Returns      : 0 - success, otherwise - failure
 */
int
sal_i2c_write(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev, /* the device on the I2C bus (slave device ID/address) to be accessed */
  uint32 addr,    /* The address in the device to be accessed (register address). Used if addr_len>0 */
  uint8 addr_len, /* the length of the address in bytes, should be between 0 and 4. */
  uint8 *data,    /* the buffer of the data to be written. */
  uint8 data_len) /* the length of the data to write. */
{
#ifdef INCLUDE_CPU_I2C
    return sal_i2c_operation(i2c_bus, i2c_dev, addr_len, data_len, addr, data, SAL_I2C_FLAGS_WRITE);
#else  /* INCLUDE_CPU_I2C */
    return _SHR_E_UNAVAIL;
#endif /* INCLUDE_CPU_I2C */
}

/* 
 * Function     : sal_i2c_write_split
 * Purpose      : Provide i2c bus write functionality using CPU i2c controller    
 *                Uses two Linux messages for the write instead of one.
 * Returns      : 0 - success, otherwise - failure
 */
int
sal_i2c_write_split(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev, /* the device on the I2C bus (slave device ID/address) to be accessed */
  uint32 addr,    /* The address in the device to be accessed (register address). Used if addr_len>0 */
  uint8 addr_len, /* the length of the address in bytes, should be between 0 and 4. */
  uint8 *data,    /* the buffer of the data to be written. */
  uint8 data_len) /* the length of the data to write. */
{
#ifdef INCLUDE_CPU_I2C
    return sal_i2c_operation(i2c_bus, i2c_dev, addr_len, data_len, addr, data,
      SAL_I2C_FLAGS_WRITE | SAL_I2C_FLAGS_WRITE_SPLIT);
#else  /* INCLUDE_CPU_I2C */
    return _SHR_E_UNAVAIL;
#endif /* INCLUDE_CPU_I2C */
}

/*
 * Function: cpu_i2c_device_present
 * Purpose: Probe the I2C bus using i2c_dev, report if a device responds.
 *          The I2C bus is released upon return.
 *          No further action is taken.
 *
 * Parameters:
 *    i2c_dev - I2C slave address
 *
 * Return:
 *     _SHR_E_NONE - An I2C device responds at the indicated slave address i2c_dev.
 *     otherwise  - No I2C response at the indicated saddr, or I/O error.
 */
int
sal_i2c_device_present(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev) /* the device on the I2C bus (slave device ID/address) to be accessed */
{
#ifdef INCLUDE_CPU_I2C
    struct i2c_smbus_ioctl_data smbus_data;
    unsigned bus = i2c_bus;

    if (bus >= SAL_I2C_MAX_NOF_BUSES || i2c_dev >= SAL_I2C_MAX_NOF_SLAVE_DEVS) {
        sal_printf("I2c bus %u or I2C device 0x%x out of range.\n",
                   bus, (unsigned)i2c_dev);
        return _SHR_E_PARAM;
    }

    /* open the given i2c bus device file if it is not open */
    if (i2c_bus_filedesc[bus] <= 0) {
        if (sal_i2c_init_fd(i2c_bus_filedesc + bus, bus) != _SHR_E_NONE || i2c_bus_filedesc[bus] <= 0) {
            sal_printf("Failed to auto-open the device file.\n");
            return _SHR_E_FAIL;
        }
    }


    if (ioctl(i2c_bus_filedesc[bus], I2C_SLAVE, i2c_dev) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return _SHR_E_NOT_FOUND;
    }

    sal_memset(&smbus_data, 0, sizeof(smbus_data));
    smbus_data.read_write = I2C_SMBUS_QUICK;
    smbus_data.size = I2C_SMBUS_QUICK;

    if (ioctl(i2c_bus_filedesc[bus], I2C_SMBUS, &smbus_data) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
       return _SHR_E_NOT_FOUND;
    }
    return _SHR_E_NONE;
#else  /* INCLUDE_CPU_I2C */
    return _SHR_E_UNAVAIL;
#endif /* INCLUDE_CPU_I2C */
}

/* 
 * Function     : sal_i2c_config_get 
 * Purpose      : Retrieve configurable i2c bus properties 
 * unit         : I2C controller 
 * flags        : SAL_I2C_* flags 
 */
int
sal_i2c_config_get(int unit, uint32 *flags)
{
    return(-1); /* Not supported */
}

/* 
 * Function     : sal_i2c_config_set 
 * Purpose      : Configure i2c bus properties 
 * unit         : I2C controller 
 * flags        : SAL_I2C_* flags 
 */
int
sal_i2c_config_set(int unit, uint32 flags)
{
    return(-1); /* Not supported */
}
