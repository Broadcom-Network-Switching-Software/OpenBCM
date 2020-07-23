import sys
import os
from os.path import getsize
from arm_ds.debugger_v1 import Debugger
#from arm_ds.debugger_v1 import DebugException    

print "--Start of script--"
MAX_U_BOOT_SIZE = 0xA0000

FLASH_PROGRAM_OFFSET                    = 0
FLASH_PAGE_SIZE                         = 256
FLASH_PAGES_PER_SECTOR                  = 256
FLASH_SECTOR_SIZE                       = FLASH_PAGE_SIZE*FLASH_PAGES_PER_SECTOR
FLASH_SECTOR_NUM                        = 512

#Flash command definition
MAX_CMD_NUM         = 16
CMD_M25PXX_PP       = 0x02
CMD_M25PXX_WREN     = 0x06
CMD_M25PXX_WRDI     = 0x04
CMD_M25PXX_EN4B     = 0xb7    #/* Enter 4-byte address mode */
CMD_M25PXX_EX4B     = 0xe9    #/* Exit 4-byte address mode */
CMD_M25PXX_SE       = 0xd8
CMD_M25PXX_RDSR     = 0x05
CMD_M25PXX_RDFSR    = 0x70

#We used internal RAM to store tiny program and its parameter
BREAK_POINT_OFFSET  = 0x13C
TMP_FILE_PATH       = sys.currentWorkingDir+"\\__TEMP.bin"

TX_RAM = [0]*16
CMD_BUF = [0]*MAX_CMD_NUM

if len(sys.argv) == 1:
    print " !!ErrorPlease provide the path of u-boot.bin"
    sys.exit(0)

image_path = sys.argv[1]                            #Get u-boot image path
img_len = getsize(image_path)                       #Get size of u-boot.bin
if img_len > MAX_U_BOOT_SIZE:                       #Image size can't exceed the maximum size of u-boot partition
    print " !!Error: Wrong image size(%d), the maximum size should be less than %d" %(img_len, MAX_U_BOOT_SIZE)
    sys.exit(0)
    
print "Size of %s is %d bytes." %(image_path, img_len)

IMAGE = [0]*img_len
uboot_image = open(image_path, "rb")

debugger = Debugger()
cur_ec = debugger.getCurrentExecutionContext()
memory = cur_ec.getMemoryService()
register = cur_ec.getRegisterService()

def REG_AND(addr, value):
    v = memory.readMemory32(addr)
    v &= value
    memory.writeMemory32(addr, v)
    return v

def REG_OR(addr, value):
    v = memory.readMemory32(addr)
    v |= value
    memory.writeMemory32(addr, v)
    return v

def num2hexstr(num):
    hexstr = "0x%X" %num
    return hexstr

def py_bspi_enable():
    memory.writeMemory32(QSPI_mspi_WRITE_LOCK, 0)
    
    memory.writeMemory32(QSPI_bspi_registers_B0_CTRL, 0)
    memory.writeMemory32(QSPI_bspi_registers_B1_CTRL, 0)
    memory.writeMemory32(QSPI_bspi_registers_B0_CTRL, 1)
    memory.writeMemory32(QSPI_bspi_registers_B1_CTRL, 1)
    
    memory.writeMemory32(QSPI_bspi_registers_MAST_N_BOOT_CTRL, 0)

#################################################################
#  Function: 
#      py_spi_init
#  Desc:
#      Init spi controller
#################################################################
def py_spi_init():
    print "Initialize spi controller..."
    memory.writeMemory32(QSPI_mspi_SPCR1_LSB, 0)
    memory.writeMemory32(QSPI_mspi_SPCR1_MSB, 0)
    memory.writeMemory32(QSPI_mspi_NEWQP, 0)
    memory.writeMemory32(QSPI_mspi_ENDQP, 0)
    memory.writeMemory32(QSPI_mspi_SPCR2, 0)
    #MSPI: SCK configuration
    memory.writeMemory32(QSPI_mspi_SPCR0_LSB, 0x0000008)
    #Master | 8 bits per word | mode: CPOL=1 / CPHA=1
    memory.writeMemory32(QSPI_mspi_SPCR0_MSB, 0x000000A3)
    
    memory.writeMemory32(QSPI_bspi_registers_MAST_N_BOOT_CTRL, 1)   #Enable MSPI
    #py_bspi_enable(0)   #Disable BSPI
    print "Initialize spi controller done!"


def wait_for_complete(reg_addr, mask, value):
    print "wait_for_complete reg_addr = 0x%X, mask=0x%X, value=0x%X" %(reg_addr, mask, value)
    while 1:
        reg = memory.readMemory32(reg_addr)
        if reg&mask == value:
            break;

#################################################################
#  Function: 
#      py_spi_cmd_transmit
#  Desc:
#      Fill txram buffer, and trigger spi controller to shift out date
#################################################################
def py_spi_cmd_transmit(tx_buf, tx_len, rx_len):
    total_len = tx_len + rx_len
    if total_len > MAX_CMD_NUM or total_len == 0:
        return
        
    for ii in range(total_len):
        memory.writeMemory32(QSPI_mspi_CDRAM00+ii*4, 0x82)
        if ii == total_len - 1:
            memory.writeMemory32(QSPI_mspi_CDRAM00+ii*4, 0x02)
        if ii >= tx_len:
            tx_buf[ii] = 0xFF
        memory.writeMemory32(QSPI_mspi_TXRAM00+ii*8, tx_buf[ii])
    
    memory.writeMemory32(QSPI_mspi_NEWQP, 0)
    memory.writeMemory32(QSPI_mspi_ENDQP, total_len - 1)
    
    memory.writeMemory32(QSPI_mspi_MSPI_STATUS, 0)  #clear status
    memory.writeMemory32(QSPI_mspi_SPCR2, 0xC0) #cont_after_cmd|spe
     
    wait_for_complete(QSPI_mspi_MSPI_STATUS, 0x00000001, 0x00000001)
    
    if rx_len == 0:
        return
    
    for ii in range(total_len):
        tx_buf[ii] = 0
        
    for ii in range(rx_len):
        tx_buf[ii] = 0
        tx_buf[ii] = memory.readMemory32(QSPI_mspi_RXRAM00+4+tx_len*8+ii*8)


#################################################################
#  Function: 
#      py_erase_flash
#  Desc:
#      Main entry for flash erasing
#################################################################
def py_erase_flash(offset, img_len):
    if offset%FLASH_SECTOR_SIZE != 0:
        print " !!Error: Erasing offset must be aligned by 0x%x" %FLASH_SECTOR_SIZE
        sys.exit(0)
    
    print "Sflash erasing......"
    erase_len = img_len
    mod_size = erase_len%FLASH_SECTOR_SIZE
    #print "mod_size =%d" %mod_size
    if mod_size != 0:
        erase_len = erase_len-mod_size+FLASH_SECTOR_SIZE    #Must align by FLASH_SECTOR_SIZE
        
    erasing_sector_num = erase_len/FLASH_SECTOR_SIZE
    print "%d sectors need to be erased" %erasing_sector_num
    for sector in range(erasing_sector_num):
        CMD_BUF[0] = CMD_M25PXX_WREN     #Write enable
        py_spi_cmd_transmit(CMD_BUF, 1, 0)
        CMD_BUF[0] = CMD_M25PXX_SE       #Sector erase
        CMD_BUF[1] = sector
        CMD_BUF[2] = 0x00;
        CMD_BUF[3] = 0x00;
        py_spi_cmd_transmit(CMD_BUF, 4, 0)
        print "polling status register"
        while 1: #Wait for erase complete
            CMD_BUF[0] = CMD_M25PXX_RDSR     #Read status register
            py_spi_cmd_transmit(CMD_BUF, 1, 1)
            if CMD_BUF[0]&1 == 0:
                break
        print "polling flag status register"
        while 1: #Wait for erase/program controller ready (for some Micron flashes)
            CMD_BUF[0] = CMD_M25PXX_RDFSR     #Read flag status register
            py_spi_cmd_transmit(CMD_BUF, 1, 1)
            if CMD_BUF[0]&0x80 == 0x80:
                break
        print " %dth secotr erased......done!" %(sector+1)
    print "Erase done!"
    #print "erase_len=%d %d" %(erase_len,erasing_sector_num)



def py_new_write_flash():
    #load_str = "loadfile " + sys.argv[2] + " " + num2hexstr(IMAGE_START_ADDRESS);
    load_str = "restore " + sys.argv[2] + " binary "+num2hexstr(IMAGE_START_ADDRESS)
    print load_str
    cur_ec.executeDSCommand(load_str)
    block_num = img_len/DATA_BLOCK_SIZE
    print "block_num = %d" %block_num
    uboot_image = open(image_path, "rb")
    print sys.argv[1]
    
    #Set input parameters for sfw.bin
    memory.writeMemory32(PARAM_START_ADDRESS+0x08, DATA_START_ADDRESS);
    memory.writeMemory32(PARAM_START_ADDRESS+0x0C, QSPI_mspi_SPCR0_MSB);
    memory.writeMemory32(PARAM_START_ADDRESS+0x10, QSPI_mspi_NEWQP);
    memory.writeMemory32(PARAM_START_ADDRESS+0x14, QSPI_mspi_ENDQP);
    memory.writeMemory32(PARAM_START_ADDRESS+0x18, QSPI_mspi_SPCR2);
    memory.writeMemory32(PARAM_START_ADDRESS+0x1C, QSPI_mspi_MSPI_STATUS);
    memory.writeMemory32(PARAM_START_ADDRESS+0x20, QSPI_mspi_TXRAM00);
    memory.writeMemory32(PARAM_START_ADDRESS+0x24, QSPI_mspi_RXRAM00);
    memory.writeMemory32(PARAM_START_ADDRESS+0x28, QSPI_mspi_CDRAM00);
    memory.writeMemory32(PARAM_START_ADDRESS+0x2C, QSPI_mspi_WRITE_LOCK);
    
    break_str = "advance *" + num2hexstr(IMAGE_START_ADDRESS+BREAK_POINT_OFFSET) 
    for block_idx in range(block_num):
        print "write block %d len %d" %(block_idx, DATA_BLOCK_SIZE)
        file_content = uboot_image.read(DATA_BLOCK_SIZE)
        tmp_file = open(TMP_FILE_PATH, "wb+")
        tmp_file.write(file_content)
        tmp_file.close()
        restore_str = "restore "+TMP_FILE_PATH+" binary " + num2hexstr(DATA_START_ADDRESS)
        cur_ec.executeDSCommand(restore_str)
        memory.writeMemory32(PARAM_START_ADDRESS, block_idx*DATA_BLOCK_SIZE)
        memory.writeMemory32(PARAM_START_ADDRESS+4, DATA_BLOCK_SIZE)
        register.setValue("R0", PARAM_START_ADDRESS)
        register.setValue("PC", IMAGE_START_ADDRESS)
        register.setValue("SP", IMAGE_END_ADDRESS-4)
        register.setValue("LR", IMAGE_END_ADDRESS-4)
        #cur_ec.executeDSCommand("continue")
        cur_ec.executeDSCommand(break_str)
        cur_ec.getExecutionService().waitForStop(5000)
        print " >>>>> Block %d, size %d, write done!" %(block_idx, DATA_BLOCK_SIZE)

    last_len = img_len - block_num*DATA_BLOCK_SIZE
    if last_len > 0:
        print "write block %d len %d" %(block_num, last_len)
        file_content = uboot_image.read(last_len)
        tmp_file = open(TMP_FILE_PATH, "wb+")
        tmp_file.write(file_content)
        tmp_file.close()
        restore_str = "restore "+TMP_FILE_PATH+" binary " + num2hexstr(DATA_START_ADDRESS)
        cur_ec.executeDSCommand(restore_str)
        memory.writeMemory32(PARAM_START_ADDRESS, block_num*DATA_BLOCK_SIZE)
        memory.writeMemory32(PARAM_START_ADDRESS+4, last_len)
        register.setValue("R0", PARAM_START_ADDRESS)
        register.setValue("PC", IMAGE_START_ADDRESS)
        register.setValue("SP", IMAGE_END_ADDRESS-4)
        register.setValue("LR", IMAGE_END_ADDRESS-4)
        #cur_ec.executeDSCommand("continue")
        
        cur_ec.executeDSCommand(break_str)
        cur_ec.getExecutionService().waitForStop(5000)
        print " >>>>> Block %d, size %d, write done!" %(block_num, last_len)

    uboot_image.close()
    os.remove(TMP_FILE_PATH)
    #cur_ec.executeDSCommand("delete breakpoints")
    
   
state = cur_ec.getState()
if(cmp(state, "RUNNING") == 0):
    cur_ec.getExecutionService().stop()
    cur_ec.getExecutionService().waitForStop(5000)
    print "Force to stop debugger"
    cur_ec.executeDSCommand("memory 0x18000000 +0xFFFFF0 noverify")
    

QSPI_MSPI_BASE      = 0x18021000
RAM_SIZE            = 0x10000
RAM_START_ADDRESS   = 0x2000000
PARAM_AREA_SIZE     = 0x400
TINY_IMAGE_SIZE     = 0xC00
QSPI_mspi_SPCR0_LSB                     = QSPI_MSPI_BASE+0x200
QSPI_mspi_SPCR0_MSB                     = QSPI_MSPI_BASE+0x204
QSPI_mspi_SPCR1_LSB                     = QSPI_MSPI_BASE+0x208
QSPI_mspi_SPCR1_MSB                     = QSPI_MSPI_BASE+0x20C
QSPI_mspi_NEWQP                         = QSPI_MSPI_BASE+0x210
QSPI_mspi_ENDQP                         = QSPI_MSPI_BASE+0x214
QSPI_mspi_SPCR2                         = QSPI_MSPI_BASE+0x218
QSPI_mspi_MSPI_STATUS                   = QSPI_MSPI_BASE+0x220
QSPI_mspi_TXRAM00                       = QSPI_MSPI_BASE+0x240
QSPI_mspi_RXRAM00                       = QSPI_MSPI_BASE+0x2C0
QSPI_mspi_CDRAM00                       = QSPI_MSPI_BASE+0x340
QSPI_mspi_WRITE_LOCK                    = QSPI_MSPI_BASE+0x380

QSPI_bspi_registers_MAST_N_BOOT_CTRL    = QSPI_MSPI_BASE+0x008
QSPI_bspi_registers_BUSY_STATUS         = QSPI_MSPI_BASE+0x00C
QSPI_bspi_registers_B0_CTRL             = QSPI_MSPI_BASE+0x018
QSPI_bspi_registers_B1_CTRL             = QSPI_MSPI_BASE+0x020

PARAM_START_ADDRESS = RAM_START_ADDRESS    #Parameter area
IMAGE_START_ADDRESS = RAM_START_ADDRESS+PARAM_AREA_SIZE
IMAGE_END_ADDRESS   = IMAGE_START_ADDRESS+TINY_IMAGE_SIZE
DATA_START_ADDRESS  = IMAGE_END_ADDRESS    #Data area
DATA_BLOCK_SIZE     = RAM_START_ADDRESS + RAM_SIZE - DATA_START_ADDRESS        #The max length of data which could be saved in data area

py_spi_init()   #Init spi
py_erase_flash(FLASH_PROGRAM_OFFSET, img_len) #Erase sector according to offset and img_len
py_new_write_flash()
py_bspi_enable()

print "--End of script--"


