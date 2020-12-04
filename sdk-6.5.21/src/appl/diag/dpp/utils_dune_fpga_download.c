/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>


#if !DUNE_BCM

#include "pub/include/ref_sys.h"
#include "Utilities/include/utils_defi.h"
#include "UserInterface/include/ui_pure_defi.h"

#include "./../include/utils_dune_fpga_download.h"
#include <usrApp.h>

#include "UserInterface/include/ui_cli_files.h"

#endif /* !DUNE_BCM */

/*
 * removed in 1602
 */
unsigned char FPGA_LineCard[] =
{
0
};

unsigned char FPGA_LineCard_b[] =
{
0
};

#define PORT_B_DIRECTION_REGISTER  0xFF000AB8
#define PORT_B_PIN_ASSIGN_REGISTER 0xFF000ABC
#define PORT_B_OPEN_DRAIN_REGISTER 0xFF000AC0
#define PORT_B_DATA_REGISTER       0xFF000AC4
#define PORT_C_DATA_REGISTER       0xFF000966
/**/
static
 unsigned int initialize_port_b_spi(
   void
 )
{
  volatile unsigned long *port_b_addr;
  /**/
  port_b_addr = (unsigned long *)PORT_B_DIRECTION_REGISTER;
  /* read, modify, write */
  *port_b_addr = (*port_b_addr |= 0x0000000F);
  /**/
  port_b_addr = (unsigned long *)PORT_B_PIN_ASSIGN_REGISTER;
  /* read, modify, write */
  *port_b_addr = (*port_b_addr &= 0xFFFFFFF0);
  /**/
  port_b_addr = (unsigned long *)PORT_B_OPEN_DRAIN_REGISTER;
  /* read, modify, write */
  *port_b_addr = (*port_b_addr &= 0xFFFFFFF0);
  /**/
  sal_usleep(1);
  return 0;
}

/* Output bits from port B to emulate programming of the ALTERA FPGAs   */
#define ALTERA_NConfig      0x00000001

unsigned long Altera_dclk = 0x00000002;
unsigned long Altera_dout = 0x00000004;
/*****************************************************************************/
/* Input bits from ALTERA programming emulation port to port C.              */
#define ALTERA_Nstatus      0x0001
#define ALTERA_Confdone     0x0002
/*****************************************************************************/
/* Macros to deliver '0' / '1' to the output bits of the port:               */
#define WRITE_0_Dout    (*DataPortPtr = PortValue = (PortValue & (~Altera_dout)  ))
#define WRITE_1_Dout    (*DataPortPtr = PortValue = (PortValue |  Altera_dout)    )
#define WRITE_0_Dclk    (*DataPortPtr = PortValue = (PortValue & (~Altera_dclk)  ))
#define WRITE_1_Dclk    (*DataPortPtr = PortValue = (PortValue |  Altera_dclk)    )
#define WRITE_0_NConfig (*DataPortPtr = PortValue = (PortValue & (~ALTERA_NConfig)))
#define WRITE_1_NConfig (*DataPortPtr = PortValue = (PortValue |  ALTERA_NConfig) )
/*****************************************************************************/
/* Macro for N*100nSec delay. Uses hardware execution time of port access    */
#define DELAY(x)        for (dly=0; dly<x; dly++) *DataPortPtr = PortValue;
#define WAIT_100ns      1

STATIC
  void
    utils_dune_AlteraDownload_error_reoprt(
      unsigned int error_code,
      unsigned int silent
    );
/*****************************************************************************/
/*  FUNCTION NAME:  utils_dune_AlteraDownload
**  FUNCTION DESCRIPTION:
**  FORMAL PARAMENETS:
**   unsigned char *base_addr  - Beginning address of memory stream to download.
**                               if NULL, then the default ROM file is taken.
**   unsigned long  size       - Size in bytes of that stream. can be '0', in
**                               base_addr is NULL
**   BOOL           silent     - TRUE: no errors are printing to screen.
**  RETURN VALUE:
**    an unsigned integer typed returned:
**     -0  - download OK.
**     -10 - Device was not ready to download (Jumper is probably plugged)
**     -20 - Device had a problem while downloading.
**     -30 - Device had a problem after downloading.
**
**  DESIGN ISSUES:
**    The FPGA by ALTERA needs to be downloaded with a configuration stream
**    after each power-up (Power-Up, NOT Reset!).
**    (The configuration stream is held here above).
**    1). Download emulation facility:
**        The CPU is equipped with 2 I/O port we use to
**        emulate the download sequence to the ALTERA.
**        There are 3 output bits supplied by the Manager towards the
**        FPGA on port B. The FPGA provides 2 status bits that can be read
**        by the Manager in order to monitor the sequence flow on port C.
**    2). Download emulation flow:
**        We use the "passive serial mode" to download the FPGAs. The flow
**        looks as follows:
**        NConfig  _     _____________________________________\\________________
**                |___|                                      \\
**        NStatus  __      _____________________________________||______________
**                   |____|                                     ||
**        ConfDone __                                           ||            ___
**                   |__________________________________________||___________|
**                                __    __    __    __    __    ||_    __    __
**        Dclk     _|||__________|  |__|  |__|  |__|  |__|  |__|||.|__|  |__|  |__
**                              ____  ____  ____  ____  ____  __||__  ____
**        Dout     ------------<_D0_><_D1_><_D2_><_D3_><_D4_><___\\_><_Dn_>-------
**                                                                \\
**        The flow is devided into three stages:
**        -. INIT:
**                Write 'NConfig' = 0;
**                Wait 100mSec
**                Write 'NConfig' = 1;
**
**        -. PROGRAM:
**             -> Write 'Dout'.
**            |   Wait >30nSec
**            |   Write 'Dclk' = 1
**            |   Wait >50nSec
**            |   Write 'Dclk' = 0
**            |   Wait >50nSec
**            |   Read 'NStatus' : if '0' - Device had error.
**            |_________|
**
**        -. MONITOR SUCCESS:
**                Write 'Dclk' = 1
**                Wait >100nSec
**                Read ConfDone : If '0' - Device didn't configured well.
**                run 10 'Dclk' ticks.
**
**        For more details look at application note #59 from altera:
**        "Configuring Flex 10K devices" - Passive serial mode.
**        Notice I added a bunch of clocks around the NConfig falling edge. This
**        was done to fully emulate the behavior of the 'Byte-Blaster' cable.
 */
unsigned int
 utils_dune_AlteraDownload (
   const unsigned char                *base_addr,
   const char                         *image_name,
   unsigned long                       size,
   UTILS_DUNE_FPGA_DOWNLOAD_TYPE_ENUM  board_version,
   unsigned int                        silent
 )
{
   volatile unsigned long  *DataPortPtr;
   volatile unsigned short *StatusPortPtr;
   unsigned long PortValue, value;
   unsigned int  retry, dly, ex;
   unsigned long stream_size, i,j;
   const unsigned char *dataArray;
   unsigned char datum;
   /* init */

   if(board_version == UTILS_DUNE_FPGA_DOWNLOAD_V_1)
   {
    /* At the old line card, there was a bug in the hardware.
     *  This is the software patch.
     */
     Altera_dclk = 0x00000004;
     Altera_dout = 0x00000002;
   }
   else
   {
     Altera_dclk = 0x00000002;
     Altera_dout = 0x00000004;
   }

   ex = PortValue = retry = 0;
   DataPortPtr     = (unsigned long  *)(PORT_B_DATA_REGISTER);
   StatusPortPtr   = (unsigned short *)(PORT_C_DATA_REGISTER);
   /**/
   initialize_port_b_spi();


   if(base_addr)
   {
     stream_size = size;
     dataArray = base_addr;
     d_printf(
       image_name
     );

   }
   else
   {
     if(board_version == UTILS_DUNE_FPGA_DOWNLOAD_V_2)
     {
       d_printf(
         "     Download FPGA                       .....     FPGA_LineCard_b\r\n"
       );
       stream_size = sizeof(FPGA_LineCard_b);
       dataArray = FPGA_LineCard_b;
       d_printf(
         "     ***************************************************** \r\n"
         "     ********       Download FAIL!    .............  ***** \r\n"
         "     FPGA_LineCard_b/FPGA_LineCard are deleted from code (so large) \r\n"
         "     See utils_dune_fpga_download                          \r\n"
         "     ***************************************************** \r\n"
         );

     }
     else
     {
       stream_size = sizeof(FPGA_LineCard);
       dataArray = FPGA_LineCard;
       d_printf(
         "     Download FPGA                       .....     FPGA_LineCard\r\n"
         );

       d_printf(
         "     ***************************************************** \r\n"
         "     ********       Download FAIL!    .............  ***** \r\n"
         "     FPGA_LineCard_b/FPGA_LineCard are deleted from code (so large) \r\n"
         "     See utils_dune_fpga_download                          \r\n"
         "     ***************************************************** \r\n"
         );

     }
   }

   load_indication_start(stream_size);
   /**/

   WRITE_1_NConfig;           /* Initial state. NConfig is set to '1'         */
   sal_usleep(10);

   for (i=0;i<5;i++)          /* Generate 5 Dclk ticks befor NConfig set '0' */
   {                          /* This was added in order to emulate the      */
     WRITE_1_Dclk;            /* 'Byte-Blaster' behavior, as monitored by    */
     DELAY (WAIT_100ns);      /* an osciloscope.                             */
     WRITE_0_Dclk;
     DELAY (WAIT_100ns);
   }

   WRITE_0_NConfig;           /* A low level pulse at NConfig for 100mSec     */

   for (i=0;i<5;i++)          /* Generate 5 Dclk ticks befor NConfig set '0' */
   {
     DELAY (WAIT_100ns);      /* This was added in order to emulate the      */
     WRITE_1_Dclk;            /* 'Byte-Blaster' behavior, as monitored by    */
     DELAY (WAIT_100ns);      /* an osciloscope.                             */
     WRITE_0_Dclk;

   }

   sal_usleep(10);

   WRITE_1_NConfig;
   sal_usleep(10);

   /* Read the Port to see if the fpga responded well                      */

   value = (*StatusPortPtr) & 0x000f;

   /* ConfDone should be '0' and Nstatus should be '1' if one of those     */
   /* bit fail to meet the above expectations, we mark failure.            */
   if ( (0 != (value & ALTERA_Confdone)) ||
        (0 == (value & ALTERA_Nstatus ))
      )
   {
     ex = 10;
     goto exit;
   }
   /************************************************************************/
   /*              P R O G R A M   -   S E Q U E N C E                     */
   /************************************************************************/
   /* Run a loop to read all the 'size' count of bytes to program          */
   for (i=0; i<stream_size; i++)
   {
     datum = (unsigned char)(*dataArray);
     dataArray++;
     /* Shift a bit at a time from the given current pointed Byte          */
     for (j=0; j<8; j++)
     {
        if (datum & 0x01)     /* Write a proper '0'/'1' Bit to the Dout    */
        {
          WRITE_1_Dout;
        }
        else
        {
          WRITE_0_Dout;
        }
        DELAY (WAIT_100ns);
        WRITE_1_Dclk;         /* Generate a 'tick' in the Dclk bit         */
        DELAY (WAIT_100ns);
        WRITE_0_Dclk;
        DELAY (WAIT_100ns);
        datum = datum >> 1;   /* Shift the next bit to program.            */
     }

     /* Check to monitor NStatus bit of the Altera is set. If not,        */
     /* the device had problems. We mark failure upon NStatus = 0.        */
     value = (*StatusPortPtr) & 0x000f;
     if (0 == (value & ALTERA_Nstatus))
     {
       ex = 20;
       goto exit;
     }
     if( !(i % 5000) )
     {
       load_indication_run(i);
     }
   }
   load_indication_run(i+1);
   /************************************************************************/
   /*        E X A M I N E    D O W N L O A D   S U C C E S S              */
   /************************************************************************/
   WRITE_1_Dclk;              /* Generate a 'tick' at the Dclk bit         */
   DELAY (WAIT_100ns);
   WRITE_0_Dclk;
   DELAY (WAIT_100ns);

 /* Read the Port to see if STICs ConfDone returned back high            */
   value = (*StatusPortPtr) & 0x000f;
   /* ConfDone should be '1' to indicate sequence completed OK.            */
   /* bit fail to meet the above expectations, we mark failure at stic     */
   if (0 == (value & ALTERA_Confdone))
   {
     ex = 30;
     goto exit;
   }
exit:
  utils_dune_AlteraDownload_error_reoprt(ex, silent);
  return ex;
}
/*****************************************************
*NAME: utils_dune_download_and_load_fpga_rbf_file
*TYPE: PROC
*DATE: 03/JUL/2003
*FUNCTION: Loads the rbf file from tftp to the
*          altera FPGA (using the above util)
*CALLING SEQUENCE:
*  utils_dune_download_and_load_fpga_rbf_file()
*INPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT: rbf file name is 'fpga_linecard'.
*OUTPUT:
*  SOC_SAND_DIRECT: non-zero in case of an error
*  SOC_SAND_INDIRECT: FPGA is downloaded
*REMARKS:
*SEE ALSO: utils_dune_AlteraDownload
 */
unsigned int
  utils_dune_download_and_load_fpga_rbf_file(
    UTILS_DUNE_FPGA_DOWNLOAD_TYPE_ENUM board_ver_id
  )
{
  unsigned int   ex;
  unsigned char  err_msg[320];
  char *rbf_file_name = "fpga_lineCard";
  unsigned char *rbf_file_base;
  unsigned long  rbf_file_size;
  int            rbf_file_descriptor;
  const char *image_name = "     Download FPGA                       .....     Unknown\r\n";
  /**/
#if !DUNE_BCM
  if(download_cli_file(rbf_file_name, err_msg, DEFAULT_DOWNLOAD_IP/*Local*/))
#else
  if(0)
#endif
  {
    ex = 100;
    goto exit;
  }
#if !DUNE_BCM
  if (get_cli_file_name_handle(rbf_file_name, err_msg, &rbf_file_descriptor) )
#else
  err_msg[0] = 0;
  rbf_file_descriptor = 0;
  rbf_file_base = 0;
  rbf_file_size = 0;
  rbf_file_name = "fpga_lineCard_bogus";
  if(0) 
#endif
  {
    ex = 200;
    goto exit;
  }
#if !DUNE_BCM
  ex  = get_cli_file_size(rbf_file_descriptor, (void *)&rbf_file_size);
  ex |= get_cli_file_mem_base(rbf_file_descriptor, (void *)&rbf_file_base);
#else
  ex = 0; 
#endif
  if(ex)
  {
    ex = 300;
    goto exit;
  }
  ex = utils_dune_AlteraDownload(rbf_file_base, image_name, rbf_file_size, board_ver_id, 1);
  if(ex)
  {
    ex += 400;
    goto exit;
  }
#if !DUNE_BCM
  ex = erase_cli_file(rbf_file_name, err_msg);
#else
  ex = 0;
#endif
  if(ex)
  {
    ex = 500;
    goto exit;
  }
/**/
exit:
  return ex;
}

/*****************************************************
*NAME: utils_dune_AlteraDownload_error_reoprt
*TYPE: PROC
*DATE: 07/JUL/2003
*FUNCTION: Error report function for
*     utils_dune_AlteraDownload()
*CALLING SEQUENCE:
*  utils_dune_AlteraDownload_error_reoprt(silent)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int error_code -
*      utils_dune_AlteraDownload() error code.
*    unsigned int silent -
*     TRUE/FALSE if FALSE no printing is done.
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT: Screen prints
*REMARKS:
*SEE ALSO: utils_dune_AlteraDownload
 */
STATIC
  void
    utils_dune_AlteraDownload_error_reoprt(
      unsigned int error_code,
      unsigned int silent
    )
{
  char
    *proc_name;
  proc_name = "utils_dune_AlteraDownload_error_reoprt";

  if (silent)
  {
    goto exit;
  }

  if (0 == error_code)
  {
    /*
     * No error was found.
     * Print nothing.
     */
    goto exit;
  }

  if(10 == error_code)
  {
    d_printf(
      "     Download FPGA                       .....     fail - during init\r\n"
    );
    gen_err(FALSE,FALSE,0,0,
            "CPU failed to init FPGA for download",
            proc_name,SVR_MSG,0,TRUE,0,-1,FALSE);
  }
  else if(20 == error_code)
  {
    d_printf(
      "     Download FPGA                       .....     fail - during serial download\r\n"
    );
    gen_err(FALSE,FALSE,0,0,
            "FATAL Error: CPU failed to serial program FPGA",
            proc_name,SVR_ERR,0,TRUE,0,-1,FALSE);
  }
  else /* 30 */
  {
    d_printf(
      "     Download FPGA                       .....     fail - the whole process\r\n"
    );
    gen_err(FALSE,FALSE,0,0,
            "FATAL Error: CPU failed to download FPGA",
            proc_name,SVR_ERR,0,TRUE,0,-1,FALSE);
  }

exit:
  return;
}
