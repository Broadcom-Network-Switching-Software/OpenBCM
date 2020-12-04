/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Matrix Orbital LCD Controller/Display
 * command set. The Matrix Orbital display is an LCD display with an
 * I2C interface and command set.
 *
 * See also: Matrix Orbital (http://www.matrix-orbital.com/)
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>

/* Matrix Orbital LCD 204x (20x4) */
#define NUM_LCD_COLUMNS     20
#define NUM_LCD_ROWS         4
#define LCD_COMMAND         0xfe


STATIC int
lcd_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(devno);
    COMPILER_REFERENCE(data);

    *len = 0;
    return SOC_E_NONE;
}

STATIC int
lcd_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint32 i;

    for(i = 0; i < len; i++) {
	rv |= soc_i2c_write_byte(unit, saddr, *data++);
	soc_i2c_device(unit, devno)->tbyte++;   
    }
    return rv;
}

STATIC void
lcd_config(int unit, int devid,
	 char columns, char rows)
{
    uint8 buffer[4];

    buffer[0] = LCD_COMMAND;
    buffer[1] = 'I';
    buffer[2] = columns;
    buffer[3] = rows;
    lcd_write(unit, devid, 0, buffer, 4);
}

STATIC void
lcd_ttymode(int unit, int devid){
    uint8 buffer[4];

    /* Turn on auto-scroll and auto-wrap */
    buffer[0] = LCD_COMMAND;
    buffer[1] = 'C'; /* Auto line-wrap mode */
    buffer[2] = LCD_COMMAND;
    buffer[3] = 'Q'; /* Auto scroll mode */
    lcd_write(unit, devid, 0, buffer, 4);
}

STATIC void
lcd_cls(int unit, int devid){
    uint8 buffer[4];

    buffer[0] = LCD_COMMAND;
    /* Goto upper left hand corner (0,0) */
    buffer[1] = 0x47;
    buffer[2] = 0;
    buffer[3] = 0;
    lcd_write(unit, devid, 0, buffer, 4);
    buffer[1] = 'X'; /* Clear screen */
    lcd_write(unit, devid, 0, buffer, 2);

}

STATIC void
lcd_contrast(int unit, int devid, uint8 val)
{
    uint8 buffer[3];

    /* Turn on auto-scroll and auto-wrap */
    buffer[0] = LCD_COMMAND;
    buffer[1] = 0x50; /* Clear screen */
    buffer[2] = val;
    lcd_write(unit, devid, 0, buffer, 3);    
}

STATIC int
lcd_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    switch (opcode) {

    case I2C_LCD_CLS:
	lcd_cls(unit, devno);
	break;

    case I2C_LCD_CONTRAST:
	if(!data)
	    break;
	else {
	    uint8 db = *((uint8*)data);
	    lcd_contrast(unit, devno, db);
	}
	break;
    case I2C_LCD_CONFIG:
	lcd_config(unit, devno, NUM_LCD_COLUMNS, NUM_LCD_ROWS);
	break;

    default:
	break;
    }
    return SOC_E_NONE;
}



STATIC int
lcd_init(int unit, int devno,
	  void* data, int len)
{
    int i;
    char *buf;

    soc_i2c_devdesc_set(unit, devno,
			"Matrix Orbital LCD (http://www.matrix-orbital.com)");

    buf = sal_alloc(128,"lcd");
    if (buf == NULL)
	return SOC_E_MEMORY;

    lcd_config(unit, devno, NUM_LCD_COLUMNS, NUM_LCD_ROWS);
    lcd_ttymode(unit, devno);
    lcd_cls(unit, devno);

    sal_memset(buf,0,128);
    /* On a 20x4, this should fit across the top */
    sal_sprintf(buf, "%s", "Broadcom Corporation\n");
    lcd_write(unit, devno, 0, (uint8 *)buf, sal_strlen(buf));
    for (i = 0; i < soc_cm_get_num_devices(); i++) {
	sal_memset(buf,0,128);
	sal_sprintf(buf, "%d:%s\n", i, soc_dev_name(i));
	lcd_write(unit, devno, 0, (uint8 *)buf, sal_strlen(buf));
    }

    sal_free(buf);
    return SOC_E_NONE;
}

/* LCD Driver callout */
i2c_driver_t _soc_i2c_lcd_driver = {
    0x0, 0x0, /* System assigned bytes */
    LCD_DEVICE_TYPE,
    lcd_read,
    lcd_write,
    lcd_ioctl,
    lcd_init,
    NULL,
};
