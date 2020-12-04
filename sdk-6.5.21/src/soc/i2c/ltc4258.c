/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Linear Technology LTC4258 Powered
 * Ethernet Controller Chip.
 * The LTC4258 is used to control DC power supplied over an ethernet
 * link.
 *
 * See the LTC4258 datasheet for more details.
 */

#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>

#define LTC4258 4258
#define LTC4259 4259

STATIC int PoeDeviceType = 0;
STATIC char *PoeDeviceName = NULL;

STATIC int
ltc4258_auto(int unit, uint8 saddr)
{
  int rv;
  /* Enable all ports' power on the POE controller (fully automatic) */
  /* Disable interrupts */
  if ((rv = soc_i2c_write_byte_data(unit, saddr, 0x01, 0x00)) != SOC_E_NONE)
    return rv;
  /* Full auto mode */
  if ((rv = soc_i2c_write_byte_data(unit, saddr, 0x12, 0xFF)) != SOC_E_NONE)
    return rv;
  /* Enable DC disconnect */
  if ((rv = soc_i2c_write_byte_data(unit, saddr, 0x13, 0x0F)) != 
      SOC_E_NONE)
    return rv;
  /* Enable classification and detection */
  if ((rv = soc_i2c_write_byte_data(unit, saddr, 0x14, 0xFF)) != SOC_E_NONE)
    return rv;
  return SOC_E_NONE;
}

STATIC int
ltc4258_enable_port(int unit, uint8 saddr, void *portmap)
{
  /* Enable (fully automatic) power on the indicated port(s) */
  /* return soc_i2c_write_byte_data(unit, saddr, 0x12, 0xFF); */
  return SOC_E_NONE;
}

STATIC int
ltc4258_disable_port(int unit, uint8 saddr, void *portmap)
{
  /* Shutdown power on the indicated port(s) */
  /* return soc_i2c_write_byte_data(unit, saddr, 0x12, 0x00); */
  return SOC_E_NONE;
}

STATIC int
ltc4258_shutdown(int unit, uint8 saddr)
{
  /* Shutdown power on all ports */
  return soc_i2c_write_byte_data(unit, saddr, 0x12, 0x00);
}

STATIC int
ltc4258_clear_ints(int unit, uint8 saddr)
{
  int rv;
  uint8 reg_data;
  /* Clear all event bits */
  if ((rv=soc_i2c_read_byte_data(unit, saddr, 0x03, &reg_data)) != SOC_E_NONE)
    return rv;
  if ((rv=soc_i2c_read_byte_data(unit, saddr, 0x05, &reg_data)) != SOC_E_NONE)
    return rv;
  if ((rv=soc_i2c_read_byte_data(unit, saddr, 0x07, &reg_data)) != SOC_E_NONE)
    return rv;
  if ((rv=soc_i2c_read_byte_data(unit, saddr, 0x09, &reg_data)) != SOC_E_NONE)
    return rv;
  if ((rv=soc_i2c_read_byte_data(unit, saddr, 0x0B, &reg_data)) != SOC_E_NONE)
    return rv;

  /* Clear all interrupt condition bits */
  if ((rv=soc_i2c_write_byte_data(unit, saddr, 0x1A, 0xC0)) != SOC_E_NONE)
    return rv;

  return SOC_E_NONE;
}

STATIC int
ltc4258_rescan(int unit, uint8 saddr)
{
  /* Re-scan the ports for class and detection. */
  return soc_i2c_write_byte_data(unit, saddr, 0x18, 0xFF);
}


/* This routine writes a textual listing of the register values
 * to the character buffer of length len pointed to by data.
 */

STATIC int
ltc4258_reg_dump_text(int unit, uint8 saddr, char *data, int len)
{
  int buflen, regnum;
  uint8 x;
  int rv = SOC_E_NONE;

  do {

    buflen = sal_snprintf(data, len, "Read all %s registers...\n",
			  PoeDeviceName);
    if (buflen >= len) 
      break;
    data += buflen;
    len -= buflen;
    
    for (regnum = 0; regnum <= 0x17; regnum++) {
      if ((regnum>=3) && (regnum<=0x0B) && (regnum&0x01))
	continue; 
      rv = soc_i2c_read_byte_data(unit, saddr, (uint8)regnum, &x);
      if (rv != SOC_E_NONE)
	break;
      buflen = sal_snprintf(data, len, "register[0x%02x] = 0x%02X\n", regnum, x);
      if (buflen >= len) 
	break;
      data += buflen;
      len -= buflen;
    }

  } while(0);

  return rv;
}

STATIC int
ltc4258_chip_status_text(int unit, uint8 saddr, char *data, int len)
{
  int buflen, port;
  uint8 x, y;
  char *class_text[8] = {
    "Class_Unknown", "Class_1      ", "Class_2      ", "Class_3      ",
    "Class_4      ", "Undef_Class_0", "Class_0      ", "Overcurrent  "
  };
  char *detect_text[8] = {
    "Detect_Unknown", "Short_Circuit ", "?????         ", "RLOW          ",
    "Detect_Good   ", "RHIGH         ", "Open_Circuit  ", "?????         "
  };
  int rv = SOC_E_NONE;

  rv = soc_i2c_read_byte_data(unit, saddr, 0x00, &x);
  if (rv != SOC_E_NONE)
    return rv;
  buflen = sal_snprintf(data, len, "LTC4258 status:\n %s%s%s%s%s%s%s%s\n",
			x&0x80?"Supply_Event ":""   , x&0x40?"TSTART_Fault ":"",
			x&0x20?"TICUT_Fault ":""    , x&0x10?"Class_Complete ":"",
			x&0x08?"Detect_Complete ":"", x&0x04?"Disconnect ":"",
			x&0x02?"PwrGood_Event ":""  , x&0x01?"PwrEnable_Event ":"");
  if (buflen >= len) 
    return rv;

  do {

    for (port=0; port<4; port++) {
      rv = soc_i2c_read_byte_data(unit, saddr, (uint8)port + 0x0C, &x);
      if (rv != SOC_E_NONE)
	break;
      rv = soc_i2c_read_byte_data(unit, saddr, 0x10, &y);
      if (rv != SOC_E_NONE)
	break;

      buflen += sal_snprintf(&data[buflen], len-buflen, "Port #%d:\n %s %s %s%s\n", 
			     port+1,class_text[(x>>4) & 0x7],
			     detect_text[x & 0x7],
			     (y & (0x10<<port))?"Power_Good   ":"",
			     (y & (0x01<<port))?"Power_Enable ":"");
      if (buflen >= len)
	break;
    }

  } while(0);


  return rv;
}

STATIC int
ltc4258_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    *len = 1; /* Byte mode */
    return soc_i2c_read_byte_data(unit,
				  soc_i2c_addr(unit, devno),
				  (uint8)addr, data);
}

STATIC int
ltc4258_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    /* len is ignored; write 1 byte only */
    return soc_i2c_write_byte_data(unit,
				   soc_i2c_addr(unit, devno),
				   (uint8)addr, *data);
}



/*
 * Function: ltc4285_ioctl
 *
 * Purpose: perform various operations on the LTC4258 Powered Ethernet
 *          Controller chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    opcode - CPU request (I2C_POE_IOC_XXX)
 *    data - address of optional data (opcode dependent)
 *    len - size of data
 *
 * Returns:
 *     SOC_E_NONE - no error
 *     SOC_E_TIMEOUT - I/O error.
 *     SOC_E_PARAM - chip operation unavailable.
 */
STATIC int
ltc4258_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
  int rv;
  uint8 saddr = soc_i2c_addr(unit, devno);
  switch (opcode) {
  case I2C_POE_IOC_SHUTDOWN:
    rv = ltc4258_shutdown(unit, saddr);
    break;
  case I2C_POE_IOC_AUTO:
    rv = ltc4258_auto(unit, saddr);
    break;
  case I2C_POE_IOC_ENABLE_PORT:
    rv = ltc4258_enable_port(unit, saddr, data);
    break;
  case I2C_POE_IOC_DISABLE_PORT:
    rv = ltc4258_disable_port(unit, saddr, data);
    break;
  case I2C_POE_IOC_REG_DUMP:
    if ((data == NULL) || (len <= 0))
      rv = SOC_E_PARAM;
    else
      rv = ltc4258_reg_dump_text(unit, saddr, (char *)data, len);
    break;
  case I2C_POE_IOC_STATUS:
    if ((data == NULL) || (len <= 0))
      rv = SOC_E_PARAM;
    else
      rv = ltc4258_chip_status_text(unit, saddr, (char *)data, len);
    break;
  case I2C_POE_IOC_CLEAR:
    rv = ltc4258_clear_ints(unit, saddr);
    break;
  case I2C_POE_IOC_RESCAN:
    rv = ltc4258_rescan(unit, saddr);
    break;
  default:
    rv = SOC_E_PARAM;
  }
  return rv;
}

STATIC int
ltc4258_init(int unit, int devno,
	  void* data, int len)
{
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 orig_data, regdata = 0; 
    int rv;

    /* Save register's original value, for later restoration.      */
    if ((rv = soc_i2c_read_byte_data(unit, saddr, 0x13, &orig_data)) != 
	SOC_E_NONE)
        return rv;

    /* This register is tested to identify the installed POE chip. */
    if ((rv = soc_i2c_write_byte_data(unit, saddr, 0x13, 0xFF)) != SOC_E_NONE)
        return rv;
    if ((rv = soc_i2c_read_byte_data(unit, saddr, 0x13, &regdata)) != 
	SOC_E_NONE)
        return rv;
    if (regdata == 0x0F) {
        /* Upper bits are reserved, read as zeros */
        PoeDeviceType = LTC4258;
        PoeDeviceName = "LTC4258";
        soc_i2c_devdesc_set(unit, devno, "LTC4258 Power over Ethernet");
    }
    else {
        /* Upper bits are implemented, retain values */
        PoeDeviceType = LTC4259;
        PoeDeviceName = "LTC4259";
        soc_i2c_devdesc_set(unit, devno, "LTC4259 Power over Ethernet");
    }

    /* Restore register's original data */
    if ((rv = soc_i2c_write_byte_data(unit, saddr, 0x13, orig_data)) != 
	SOC_E_NONE)
        return rv;

    /* ltc4258_shutdown(unit, saddr); */
    ltc4258_auto(unit, saddr); 
    return SOC_E_NONE;
}

/* LTC4258 POE Controller Chip Driver callout */
i2c_driver_t _soc_i2c_ltc4258_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC4258_DEVICE_TYPE,
    ltc4258_read,
    ltc4258_write,
    ltc4258_ioctl,
    ltc4258_init,
    NULL,
};

