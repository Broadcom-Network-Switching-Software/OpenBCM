/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __KERNEL__
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#endif 

#include <shared/bsl.h>
#include <soc/cm.h>
#include <soc/dcmn/utils_fpga.h>

#include <sal/core/alloc.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_88750_SUPPORT)) && (defined(LINUX) || defined(UNIX)) && (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__))
extern int _cpu_write(int d, uint32 addr, uint32 *buf);
extern int _cpu_read(int d, uint32 addr, uint32 *buf);


#define WRITE_0_Dout                                   \
do                                                     \
{                                                   \
  gpout_reg_value &= ~SOC_DPP_GPOUTDR_DATA_BIT;        \
  _cpu_write(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);    \
}                                                    \
while(0)

#define WRITE_1_Dout                                   \
do                                                     \
{                                                      \
  gpout_reg_value |= SOC_DPP_GPOUTDR_DATA_BIT;        \
  _cpu_write(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);    \
}                                                     \
while(0)

#define WRITE_0_Dclk                                   \
do                                                     \
{                                                     \
  gpout_reg_value &= ~SOC_DPP_GPOUTDR_DCLK_BIT;        \
  _cpu_write(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);    \
}                                                    \
while(0)

#define WRITE_1_Dclk                                \
do                                                   \
{                                                      \
  gpout_reg_value |= SOC_DPP_GPOUTDR_DCLK_BIT;        \
  _cpu_write(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);    \
}                                                      \
while(0)

#define WRITE_0_NConfig                                \
do                                                     \
{                                                      \
  gpout_reg_value &= ~SOC_DPP_GPOUTDR_NCONFIG_BIT;    \
  _cpu_write(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);    \
}                                                      \
while(0)

#define WRITE_1_NConfig                                \
do                                                     \
{                                                      \
  gpout_reg_value |= SOC_DPP_GPOUTDR_NCONFIG_BIT;    \
  _cpu_write(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);    \
}                                                    \
while(0)


STATIC void soc_dpp_fpga_altera_download_error_report(int error_code)
{
  
  if (error_code == 0)
  {
    goto exit;
  }
  

  if (error_code == 5)
  {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("     Error: no base address for download buffer\r\n")));
  } else if (error_code == 10)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("     Download FPGA                       .....     fail - during init\r\n"
"   CPU failed to init FPGA for download\r\n")));
  }
  else if (error_code == 20)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("     Download FPGA                       .....     fail - during serial download\r\n"
"   FATAL Error: CPU failed to serial program FPGA\r\n")));
  }
  else if (error_code == 30)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("     Download FPGA                       .....     fail - the whole process\r\n"
"   FATAL Error: CPU failed to serial program FPGA\r\n")));
  }
  else
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("    Download FPGA                        .....    fail - error code is %d\r\n"),
error_code));
  }
  
exit: 
  return;
}

static int load_indication_total_work_load = 1;
static int load_indication_current_percent = 0;
 
STATIC void soc_dpp_fpga_load_indication_print(void)
{
  LOG_CLI((BSL_META("\r%3d%%"), load_indication_current_percent));
#ifndef __KERNEL__
  fflush(stdout);
#endif 
}

int soc_dpp_fpga_task_delay(int ticks)
{
  int status = TRUE;

#ifndef __KERNEL__
  int start_tick;

  start_tick = clock();
  while (((int)clock() - start_tick) < ((int)ticks * 60))
  {
  }
  status = FALSE;
#endif 
  return status;
}

void soc_dpp_fpga_delay_minor(void)
{
  int i = 0, delay = 0;

  for(i = 0; i < delay; i++)
  {
  }
}


int soc_dpp_fpga_load_indication_run(int current_work_load)
{
  int err = 0;
  int percent = 0;

  if (current_work_load > 30000)
  {
    current_work_load = current_work_load;
  }
  
  if (load_indication_total_work_load == 0)
  {
    percent = 100;
  }
  else
  {
    
    if (current_work_load < (0xFFFFFFFF/100))
    {
      percent = (current_work_load * 100) / load_indication_total_work_load;
    }
    else
    {
      percent = current_work_load / (load_indication_total_work_load/100);
    }
  }
  if (percent > load_indication_current_percent)
  {
    load_indication_current_percent = percent;
    soc_dpp_fpga_load_indication_print();
    soc_dpp_fpga_task_delay(1);
  }
  soc_dpp_fpga_task_delay(0);
  
  return err;
}


void soc_dpp_fpga_load_indication_start(int total_work_load)
{
  load_indication_total_work_load = total_work_load;
  if (load_indication_total_work_load == 0)
  {
    load_indication_total_work_load = 1;
  }
  
  load_indication_current_percent = 0;
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\r\n")));
  soc_dpp_fpga_load_indication_print();
}

int
soc_dpp_fpga_altera_download(int unit, char *fpga_buffer_file, int file_size)
{
    
    int ex = 0;
    int stream_size, i, j;
    const char* p_dataArray;
    char datum;
    uint32 gpout_reg_value, gpin_reg_value;
    
    
    _cpu_read(unit, SOC_DPP_GPOUTDR_PIN_REG_ADDR, &gpout_reg_value);
    _cpu_read(unit, SOC_DPP_GPINDR_PIN_REG_ADDR, &gpin_reg_value);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "[%s %d],%s: start: gpout_reg_value=0x%08x, gpin_reg_value=0x%08x\n"),__FILE__ , __LINE__, FUNCTION_NAME(),gpout_reg_value,gpin_reg_value));

     
    if (fpga_buffer_file)
    {
        stream_size = file_size;
        p_dataArray = fpga_buffer_file;
    }
    else
    {
        ex = 10;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "error [%s %d]:%s\n"),__FILE__ , __LINE__, FUNCTION_NAME()));
        goto exit;
    }
    
    soc_dpp_fpga_load_indication_start(stream_size);
    
    WRITE_1_NConfig;                
    soc_dpp_fpga_task_delay(10);
    
    for(i = 0; i < 5; i++)          
    {                                
        WRITE_1_Dclk;                 
        soc_dpp_fpga_delay_minor();    
        WRITE_0_Dclk;
        soc_dpp_fpga_delay_minor();
    }
    
    WRITE_0_NConfig;                    
    for(i = 0; i < 5; i++)            
    {
        soc_dpp_fpga_delay_minor(); 
        WRITE_1_Dclk;                 
        soc_dpp_fpga_delay_minor(); 
        WRITE_0_Dclk;
    }

    soc_dpp_fpga_task_delay(10);
    
    WRITE_1_NConfig;
    soc_dpp_fpga_task_delay(10);
    
    
    
    
    _cpu_read(unit, SOC_DPP_GPINDR_PIN_REG_ADDR, &gpin_reg_value);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "[%s %d],%s: validate: gpout_reg_value=0x%08x, gpin_reg_value=0x%08x\n"),__FILE__ , __LINE__, FUNCTION_NAME(),gpout_reg_value,gpin_reg_value));

    if ( ((gpin_reg_value & SOC_DPP_GPINDR_CONF_DONE_BIT) != 0) || ((gpin_reg_value & SOC_DPP_GPINDR_NSTATUS_BIT ) == 0x0))
    {
        ex = 10;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "error [%s %d]:%s\n"),__FILE__ , __LINE__, FUNCTION_NAME()));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "[%s %d],%s: gpout_reg_value=0x%08x, gpin_reg_value=0x%08x\n"),__FILE__ , __LINE__, FUNCTION_NAME(),gpout_reg_value,gpin_reg_value));
        goto exit;
    }
    
    
    
    
    
    for(i = 0; i < stream_size; i++)
    {
        datum = (char)(*p_dataArray);
        
        p_dataArray++;
        
        for(j = 0; j < 8; j++)
        {
            if (datum & 0x01)             
            {
                WRITE_1_Dout;
            }
            else
            {
                WRITE_0_Dout;
            }
            soc_dpp_fpga_delay_minor();
            WRITE_1_Dclk;                 
            soc_dpp_fpga_delay_minor();
            WRITE_0_Dclk;
            soc_dpp_fpga_delay_minor();
        
            datum = datum >> 1;         
        }
        
        
        
        _cpu_read(unit, SOC_DPP_GPINDR_PIN_REG_ADDR, &gpin_reg_value);
     
        if ((gpin_reg_value & SOC_DPP_GPINDR_NSTATUS_BIT ) == 0)
        {
            ex = 20;
               LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "[%s %d],%s: i=%d, monitor NStatus bit (0x%x): gpout_reg_value=0x%08x, gpin_reg_value=0x%08x\n"),
                  __FILE__ , __LINE__, FUNCTION_NAME(), i, SOC_DPP_GPINDR_NSTATUS_BIT, gpout_reg_value,gpin_reg_value));
            goto exit;
        }
    
        if ( !(i % 5000) )
        {
            soc_dpp_fpga_load_indication_run(i);
        }
    
    }

    soc_dpp_fpga_load_indication_run(i+1);
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n")));
    
    
    
    
    WRITE_1_Dclk; 
    soc_dpp_fpga_delay_minor();
    WRITE_0_Dclk;
    soc_dpp_fpga_delay_minor();
    
    
    
    
    _cpu_read(unit, SOC_DPP_GPINDR_PIN_REG_ADDR, &gpin_reg_value);
    if ((gpin_reg_value & SOC_DPP_GPINDR_CONF_DONE_BIT) == 0)
    {
        ex = 30;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "error [%s %d]:%s\n"),__FILE__ , __LINE__, FUNCTION_NAME()));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "[%s %d],%s: examine download suc: gpout_reg_value=0x%08x, gpin_reg_value=0x%08x\n"),__FILE__ , __LINE__, FUNCTION_NAME(),gpout_reg_value,gpin_reg_value));
        goto exit;
    }
    
exit:
    soc_dpp_fpga_altera_download_error_report(ex);
    return ex;
}

int
soc_dpp_fpga_file2buff(const char* file_name, char* p_buff, int* p_size)
{

#ifdef __KERNEL__
    return -1;
#else
    int bytes_read = 0;
    int ret = 0;
    FILE* p_fp = NULL;
    int file_size = 0;
    
    if(p_buff == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META("utils_fpga.c, soc_dpp_fpga_file2buff(): Buffer pointer is NULL\n")));
        ret = -1;
        goto exit;
    }
    
    p_fp = fopen(file_name, "r");
    if(p_fp == NULL) 
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META("utils_fpga.c, soc_dpp_fpga_file2buff(): Could not open %s\n"),file_name));
        ret = -1;
        goto exit;
    }
    
    fseek(p_fp, 0L, SEEK_END);
    file_size = ftell(p_fp);
    
    rewind(p_fp);
    
    bytes_read = fread(p_buff, sizeof(char), file_size, p_fp);
        
    if(bytes_read < file_size)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
             (BSL_META("utils_fpga.c, soc_dpp_fpga_file2buff(): Could read only %d bytes from file: %s\n"),bytes_read, file_name));
        ret = -1;
        goto exit;        
    }

    *p_size = file_size;
exit:
    if(p_fp != NULL)
    {
        fclose(p_fp);
    }

    return ret;
#endif
}

int
soc_dpp_fpga_load(int unit, char *file_name)
{

#ifdef __KERNEL__
    return -1;
#else

    int ret = 0;
    int file_size;
    char *fpga_buffer_file = NULL;
    char the_path[256];      

    getcwd(the_path, 255);

    LOG_ERROR(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "%s(): download and burn file: %s/%s\n"), FUNCTION_NAME(), the_path, file_name));

    
    if (file_name == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "%s(): file_name is NULL\n"), FUNCTION_NAME()));
        ret =  -1;
        goto exit;
    }

    
    if ((fpga_buffer_file = (char *)sal_alloc(SOC_DPP_FPGA_BUFF_SIZE, "dpp")) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "%s(): failde to allocate fpga buffer\n"), FUNCTION_NAME()));
        ret =  -1;
        goto exit;
    }

    
    if ( (ret = soc_dpp_fpga_file2buff(file_name, fpga_buffer_file, &file_size)) != 0)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                 "%s(): soc_dpp_fpga_file2buff() has failed for file: %s\n"), FUNCTION_NAME(), file_name)) ;
        ret =  -1;
        goto exit;
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "%s(): file download succeeded, size=%d. start file burn:\n"), FUNCTION_NAME(), file_size));

     
    ret = soc_dpp_fpga_altera_download(unit, (char *)fpga_buffer_file, file_size);

exit:
    if (fpga_buffer_file)
    {
        sal_free(fpga_buffer_file);
    }

    return ret;
#endif 
}

#endif
