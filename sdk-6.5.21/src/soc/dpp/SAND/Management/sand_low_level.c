  /* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/



#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>


#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#define PRINT_LOW_LEVEL_ACESS 0

#define TRACE_LOW_LEVEL 0

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#ifdef    SAND_LOW_LEVEL_SIMULATION
  #define   USING_CHIP_SIM 1
#endif

#ifdef USING_CHIP_SIM
  #include <sim/dpp/ChipSim/chip_sim_low.h>
  extern int chip_sim_task_is_alive(void) ;
#endif

#ifndef SOC_SAND_LOW_LEVEL_ERR_MSG
  #define SOC_SAND_LOW_LEVEL_ERR_MSG 0
#endif







uint32
  Soc_sand_big_endian_was_checked = FALSE;

#ifdef    SAND_LOW_LEVEL_SIMULATION
static uint8
  Soc_sand_low_level_is_sim_active = TRUE;
#endif



uint32
  Soc_sand_big_endian = TRUE;


#ifdef SOC_SAND_LL_TIMER


SOC_SAND_LL_TIMER_FUNCTION Soc_sand_ll_timer_cnt[SOC_SAND_LL_TIMER_MAX_NOF_TIMERS];
uint8 Soc_sand_ll_is_any_active;
uint32  Soc_sand_ll_timer_total;
#endif

#ifdef SOC_SAND_LL_ACCESS_STATISTICS


uint8 Soc_sand_ll_stat_is_active[SOC_SAND_LL_NOF_STAT_IDS];
uint32  Soc_sand_ll_access_count[SOC_SAND_LL_NOF_STAT_IDS][SOC_SAND_LL_NOF_ACCESS_DIRECTIONS];


uint8 Soc_sand_ll_overflow[SOC_SAND_LL_NOF_STAT_IDS];

#endif



#ifdef SOC_SAND_LL_ACCESS_STATISTICS

void
  soc_sand_ll_stat_clear(void)
{
  uint32
    stat_i,
    dir_i;

  for (stat_i = 0; stat_i < SOC_SAND_LL_NOF_STAT_IDS; stat_i++)
  {
    Soc_sand_ll_stat_is_active[stat_i] = FALSE;
    Soc_sand_ll_overflow[stat_i] = FALSE;

    for (dir_i = 0; dir_i < SOC_SAND_LL_NOF_ACCESS_DIRECTIONS; dir_i++)
    {
      Soc_sand_ll_access_count[stat_i][dir_i] = 0;
    }
  }
}



void 
  soc_sand_ll_stat_is_active_set(
    uint32 stat_ndx,
    uint8 is_active

  )
{

  if (stat_ndx >= SOC_SAND_LL_NOF_STAT_IDS)
  {
    
    goto exit;
  }

  Soc_sand_ll_stat_is_active[stat_ndx] = is_active;
 
  if (is_active == TRUE)
  {
    Soc_sand_ll_stat_is_active[SOC_SAND_LL_STAT_ID_TOTAL] = TRUE;
  }

exit:
   return;
}

STATIC uint8
  soc_sand_ll_is_stat_active(void)
{
  return Soc_sand_ll_stat_is_active[SOC_SAND_LL_STAT_ID_TOTAL];
}


void
  soc_sand_ll_stat_increment_if_active(
    SOC_SAND_LL_ACCESS_DIRECTION direction
  )
{
  uint32
    stat_i;
  
  if (soc_sand_ll_is_stat_active())
  {
    if (direction >= SOC_SAND_LL_NOF_ACCESS_DIRECTIONS)
    {
      
      goto exit;
    }

    for (stat_i = 0; stat_i < SOC_SAND_LL_NOF_STAT_IDS; stat_i++)
    {
      if (Soc_sand_ll_stat_is_active[stat_i] == TRUE)
      {
        Soc_sand_ll_access_count[stat_i][direction]++;
      }

      if (Soc_sand_ll_access_count[stat_i][direction] == SOC_SAND_U32_MAX)
      {
        Soc_sand_ll_access_count[stat_i][direction] = 0;
        Soc_sand_ll_overflow[stat_i] = TRUE;
      }
    }
  }
  
exit:
  return;
}

void 
  soc_sand_ll_stat_print(void)
{
  uint32
    stat_i,
    dir_i;

  for (stat_i = 0; stat_i < SOC_SAND_LL_NOF_STAT_IDS; stat_i++)
  {
    if (stat_i == SOC_SAND_LL_STAT_ID_TOTAL)
    {
      LOG_CLI((BSL_META("------------------ \n\r")));
      LOG_CLI((BSL_META("Total: \n\r")));
    }
    else
    {
      LOG_CLI((BSL_META("Tag %u: \n\r"), stat_i));
    }

    if (Soc_sand_ll_overflow[stat_i] == TRUE)
    {
      LOG_CLI((BSL_META("OVERFLOW occurred, counter not available! \n\r")));
      continue;
    }

    for (dir_i = 0; dir_i < SOC_SAND_LL_NOF_ACCESS_DIRECTIONS; dir_i++)
    {
      LOG_CLI((BSL_META("  -- %s: %lu\n\r"), (dir_i == SOC_SAND_LL_ACCESS_DIRECTION_READ)?"Read ":"Write",
               Soc_sand_ll_access_count[stat_i][dir_i]
               ));
      
    }
  }
}

#endif 

#ifdef SOC_SAND_LL_TIMER

uint32
  soc_sand_ll_get_time_in_ms(
    void
  )
{
  uint32
    seconds,
    nano_seconds;

  soc_sand_os_get_time(&seconds, &nano_seconds);
  return seconds*1000 + nano_seconds/1000000;
}



void
  soc_sand_ll_timer_clear(
    void
  )
{
  uint32
    cnt_ndx;
  for (cnt_ndx = 0; cnt_ndx < SOC_SAND_LL_TIMER_MAX_NOF_TIMERS; ++cnt_ndx)
  {
    soc_sand_os_strncpy(Soc_sand_ll_timer_cnt[cnt_ndx].name, "no_function", SOC_SAND_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME);
    Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits = 0;
    Soc_sand_ll_timer_cnt[cnt_ndx].total_time = 0;
    Soc_sand_ll_timer_cnt[cnt_ndx].start_timer = 0;
    Soc_sand_ll_timer_cnt[cnt_ndx].active = 0;
  }
  Soc_sand_ll_is_any_active = FALSE;
  Soc_sand_ll_timer_total = 0;
}

void
  soc_sand_ll_timer_stop_all(
    void
  )
{
  uint32
    cnt_ndx;
  for (cnt_ndx = 0; cnt_ndx < SOC_SAND_LL_TIMER_MAX_NOF_TIMERS; ++cnt_ndx)
  {
    if( Soc_sand_ll_timer_cnt[cnt_ndx].active){
      soc_sand_ll_timer_stop(cnt_ndx);
    }
  }
}


void
  soc_sand_ll_timer_set(
    SOC_SAND_IN char name[SOC_SAND_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME],
    SOC_SAND_IN uint32 timer_ndx
  )
{

  if (timer_ndx >= SOC_SAND_LL_TIMER_MAX_NOF_TIMERS)
  {
    
    goto exit;
  }

  if(Soc_sand_ll_timer_cnt[timer_ndx].active)
  {
    goto exit;
  }

  soc_sand_os_strncpy(Soc_sand_ll_timer_cnt[timer_ndx].name, name, SOC_SAND_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME);    
  Soc_sand_ll_timer_cnt[timer_ndx].start_timer = sal_time_usecs();

  if (!Soc_sand_ll_is_any_active)
  {
    Soc_sand_ll_is_any_active = TRUE;
  }

  Soc_sand_ll_timer_cnt[timer_ndx].active = 1;

exit:
  return;
}


void
  soc_sand_ll_timer_stop(
    SOC_SAND_IN uint32 timer_ndx
  )
{
  uint32
    new_delta;

  if (timer_ndx >= SOC_SAND_LL_TIMER_MAX_NOF_TIMERS)
  {
    
    goto exit;
  }

  if(!Soc_sand_ll_timer_cnt[timer_ndx].active)
  {
    goto exit;
  }

  new_delta = sal_time_usecs() - Soc_sand_ll_timer_cnt[timer_ndx].start_timer;
  Soc_sand_ll_timer_cnt[timer_ndx].nof_hits += 1;
  Soc_sand_ll_timer_cnt[timer_ndx].total_time += new_delta;
  Soc_sand_ll_timer_cnt[timer_ndx].start_timer = 0;

  Soc_sand_ll_timer_total += new_delta;
  Soc_sand_ll_timer_cnt[timer_ndx].active = 0;

exit:
  return;
}


void 
  soc_sand_ll_timer_print_all(
    void
  )
{
  uint32
    cnt_ndx;
  COMPILER_UINT64
    total_time_1000, total_time_100, timer_total ;

  LOG_CLI((BSL_META("\r\n")));
  if (Soc_sand_ll_timer_total == 0)
  {
    LOG_CLI((BSL_META("No timers were hit, total measured execution time: 0\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META(" Execution Time Measurements.\n\r")));
    LOG_CLI((BSL_META(" Note: Percents are calculated relative to to the total measured time,\n\r")));
    LOG_CLI((BSL_META(" not accounting for overlapping timers\n\r")));
    LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
    LOG_CLI((BSL_META(" | Timer Name                             |Hit Count |Total Time[us] |Per Hit[us] |Percent |\n\r")));
    LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
    COMPILER_64_SET(timer_total, 0, Soc_sand_ll_timer_total);
    for (cnt_ndx = 0; cnt_ndx < SOC_SAND_LL_TIMER_MAX_NOF_TIMERS; ++cnt_ndx)
    {
      if (Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits != 0)
      {
        COMPILER_64_SET(total_time_1000 ,0 ,Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
        COMPILER_64_SET(total_time_100, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
        COMPILER_64_UMUL_32(total_time_1000,1000);
        COMPILER_64_UMUL_32(total_time_100,100);
        COMPILER_64_UDIV_64(total_time_1000,timer_total);
        COMPILER_64_UDIV_64(total_time_100,timer_total);
        LOG_CLI((BSL_META(" |%-40s| %-9d|%-15d|%-12d|%3d.%1d%%  |\n\r"), 
            Soc_sand_ll_timer_cnt[cnt_ndx].name, 
            Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
            Soc_sand_ll_timer_cnt[cnt_ndx].total_time, 
            Soc_sand_ll_timer_cnt[cnt_ndx].total_time / Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
            COMPILER_64_LO(total_time_100),
            COMPILER_64_LO(total_time_1000) - COMPILER_64_LO(total_time_100)*10
        ));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
      }
    }
    LOG_CLI((BSL_META(" Total time: %u[us]\n\r"), Soc_sand_ll_timer_total));
  } 
}


#endif 

void
  soc_sand_ssr_set_big_endian(
    uint32 soc_sand_big_endian_was_checked,
    uint32 soc_sand_big_endian
  )
{
  Soc_sand_big_endian_was_checked = soc_sand_big_endian_was_checked;
  Soc_sand_big_endian             = soc_sand_big_endian;
}

void
  soc_sand_ssr_get_big_endian(
    uint32 *soc_sand_big_endian_was_checked,
    uint32 *soc_sand_big_endian
  )
{
  *soc_sand_big_endian_was_checked = Soc_sand_big_endian_was_checked;
  *soc_sand_big_endian             = Soc_sand_big_endian;
}



#ifdef  SAND_LOW_LEVEL_SIMULATION
uint8
  soc_sand_low_is_sim_active_get(void)
{
  return Soc_sand_low_level_is_sim_active;
}

void soc_sand_low_is_sim_active_set(
       SOC_SAND_IN uint8 is_sim_active
     )
{
  Soc_sand_low_level_is_sim_active = is_sim_active;
}
#endif


SOC_SAND_RET
  soc_sand_check_chip_type_decide_big_endian(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 version_reg,
    SOC_SAND_IN uint32 chip_type_shift,
    SOC_SAND_IN uint32 chip_type_mask
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    chip_type_on_device;

  soc_sand_ret = SOC_SAND_OK;

  soc_sand_os_task_lock();

  chip_type_on_device = SOC_SAND_GET_FLD_FROM_PLACE (version_reg, chip_type_shift, chip_type_mask);
  if(soc_sand_get_chip_descriptor_chip_type(unit) == chip_type_on_device)
  {
    
    soc_sand_ret = SOC_SAND_OK;

    
    Soc_sand_big_endian_was_checked = TRUE;
    goto exit;
  }

  
  chip_type_on_device = SOC_SAND_GET_FLD_FROM_PLACE (SOC_SAND_BYTE_SWAP(version_reg), chip_type_shift, chip_type_mask);
  if(soc_sand_get_chip_descriptor_chip_type(unit) == chip_type_on_device)
  {
    
    Soc_sand_big_endian = !Soc_sand_big_endian ;

    
    Soc_sand_big_endian_was_checked = TRUE;

    soc_sand_ret = SOC_SAND_OK;
    goto exit;
  }

  
  soc_sand_ret = SOC_SAND_ERR;


exit:
  soc_sand_os_task_unlock();
  return soc_sand_ret;
}


uint32
  soc_sand_system_is_big_endian(
    void
  )
{
  return Soc_sand_big_endian;
}





SOC_SAND_PHYSICAL_ACCESS
  Soc_sand_physical_access = { soc_sand_eci_write, soc_sand_eci_read} ;


SOC_SAND_RET
  soc_sand_set_physical_access_hook(
    SOC_SAND_IN SOC_SAND_PHYSICAL_ACCESS* physical_access
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK ;
  if (NULL == physical_access)
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  
  soc_sand_os_task_lock() ;
  if (physical_access->physical_write)
  {
    Soc_sand_physical_access.
      physical_write = physical_access->physical_write ;
  }
  else
  {
    Soc_sand_physical_access.physical_write = soc_sand_eci_write ;
  }
  if (physical_access->physical_read)
  {
    Soc_sand_physical_access.
      physical_read = physical_access->physical_read ;
  }
  else
  {
    Soc_sand_physical_access.physical_read = soc_sand_eci_read ;
  }
  soc_sand_os_task_unlock() ;
exit:
  return soc_sand_ret ;
}

SOC_SAND_RET
  soc_sand_get_physical_access_hook(
    SOC_SAND_OUT SOC_SAND_PHYSICAL_ACCESS* physical_access
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK ;

  if (NULL != physical_access)
  {
    physical_access->physical_write = Soc_sand_physical_access.physical_write ;
    physical_access->physical_read = Soc_sand_physical_access.physical_read ;
  }
  else
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }

exit:
  return soc_sand_ret ;
}






uint32 Soc_sand_physical_write_enable=TRUE;
void
  soc_sand_set_physical_write_enable(
    uint32 physical_write_enable
  )
{
  Soc_sand_physical_write_enable = physical_write_enable;
  return;
}

void
  soc_sand_get_physical_write_enable(
    uint32 *physical_write_enable
  )
{
  *physical_write_enable = Soc_sand_physical_write_enable;
  return;
}

uint32 Soc_sand_physical_print_when_writing=FALSE;
uint32 Soc_sand_physical_print_asic_style=FALSE;
uint32 Soc_sand_physical_print_indirect_write=FALSE;
uint32 Soc_sand_physical_print_part_of_indirect_read=FALSE;
uint32 Soc_sand_physical_print_part_of_indirect_write=FALSE;
uint32 Soc_sand_physical_print_part_of_read_modify_write=FALSE;
uint32 Soc_sand_physical_print_unit_or_base_address=TRUE;

uint32 Soc_sand_physical_print_first_reg = 0;
uint32 Soc_sand_physical_print_last_reg = 0xFFFF;

void
  soc_sand_set_print_when_writing_reg_range(
    uint32 first_reg,
    uint32 last_reg
  )
{
  Soc_sand_physical_print_first_reg = first_reg;
  Soc_sand_physical_print_last_reg  = last_reg;
}

void
  soc_sand_set_print_when_writing(
    uint32 physical_print_when_writing,
    uint32 asic_style,
    uint32 indirect_write
  )
{
  Soc_sand_physical_print_when_writing = physical_print_when_writing;
  Soc_sand_physical_print_asic_style = asic_style;
  Soc_sand_physical_print_indirect_write = indirect_write;
  return;
}

void
  soc_sand_get_print_when_writing(
    uint32 *physical_print_when_writing,
    uint32 *asic_style,
    uint32 *indirect_write
  )
{
  *physical_print_when_writing = Soc_sand_physical_print_when_writing;
  *asic_style                  = Soc_sand_physical_print_asic_style;
  *indirect_write              = Soc_sand_physical_print_indirect_write;
  return;
}


void
  soc_sand_get_print_when_writing_unit_or_base_address(
    uint32 *unit_or_base_address
    )
{
  *unit_or_base_address = Soc_sand_physical_print_unit_or_base_address;
  return;
}

void
  soc_sand_set_print_when_writing_unit_or_base_address(
    uint32 unit_or_base_address
    )
{
  Soc_sand_physical_print_unit_or_base_address = unit_or_base_address;
}

void
  soc_sand_set_print_when_writing_part_of_indirect_read(
    uint32 part_of_indirect
  )
{
  Soc_sand_physical_print_part_of_indirect_read = part_of_indirect;
}


void
  soc_sand_set_print_when_writing_part_of_indirect_write(
    uint32 part_of_indirect
  )
{
  Soc_sand_physical_print_part_of_indirect_write = part_of_indirect;
}

STATIC SOC_SAND_RET
  soc_sand_physical_print_when_write_to_chip(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  )
{
  SOC_SAND_RET
    soc_sand_ret ;
  soc_sand_ret = SOC_SAND_OK ;

  if(Soc_sand_physical_print_when_writing)
  {
    uint32
      print_num = size/4,
      print_i=0;

    while(print_i<print_num)
    {
      if(Soc_sand_physical_print_part_of_indirect_read ||
          (Soc_sand_physical_print_part_of_indirect_write &&
           !Soc_sand_physical_print_indirect_write
          )
        )
      {
        print_i++;
        continue;
      }

      if(Soc_sand_physical_print_part_of_indirect_write ||
         Soc_sand_physical_print_part_of_read_modify_write
        )
      {
        print_i++;
        continue;
      }

      if(Soc_sand_physical_print_first_reg > offset/4 ||
         Soc_sand_physical_print_last_reg  < offset/4
        )
      {
        print_i++;
        continue;
      }

      if(Soc_sand_physical_print_asic_style)
      {
        LOG_CLI((BSL_META("WRITE_REG 0x%x 0x%08x\n\r"),
                 offset/4 + print_i,
                 *(array+print_i)
                 ));
      }
      else if (Soc_sand_physical_print_unit_or_base_address)
      {
        LOG_CLI((BSL_META("0x%08x 0x%08x 0x%08X\n\r"),
                 PTR_TO_INT(base_address),
                 offset/4 + print_i,
                 *(array+print_i)
                 ));
      }
      else
      {
        LOG_CLI((BSL_META("0x%08x 0x%08X\n\r"),
                 offset/4 + print_i,
                 *(array+print_i)
                 ));
      }
      print_i++;
    }
  }

  if(!Soc_sand_physical_write_enable)
  {
    LOG_CLI((BSL_META("soc_sand_physical_write_to_chip FAILED. Writing is disabled.\n\r"
                      "(0x%08x 0x%08x).\n\r"),
             offset/4,
             *(array)
             ));
  }
  goto exit;
exit:
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_physical_write_to_chip(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  )
{
  SOC_SAND_RET
    soc_sand_ret ;
  soc_sand_ret = SOC_SAND_OK ;

#if SOC_SAND_PHYSICAL_PRINT_WHEN_WRITING
  if ((Soc_sand_physical_print_when_writing) || (!Soc_sand_physical_write_enable))
  {
    soc_sand_physical_print_when_write_to_chip(
      array,base_address,offset,size
    );
  }
#endif

#ifdef SOC_SAND_LL_ACCESS_STATISTICS
  soc_sand_ll_stat_increment_if_active(SOC_SAND_LL_ACCESS_DIRECTION_WRITE);
#endif

  if(!Soc_sand_physical_write_enable)
  {
    goto exit;
  }
  if (Soc_sand_physical_access.physical_write != NULL)
  {
    soc_sand_ret = Soc_sand_physical_access.physical_write(
                 array,
                 base_address,
                 offset,
                 size
               ) ;
  }
  else
  {
    soc_sand_ret = SOC_SAND_NULL_USER_CALLBACK_FUNC ;
    goto exit ;
  }

exit:
  return soc_sand_ret ;
}



SOC_SAND_RET
  soc_sand_physical_read_from_chip(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  )
{
  SOC_SAND_RET
    soc_sand_ret ;
  soc_sand_ret = SOC_SAND_OK ;

#ifdef SOC_SAND_LL_ACCESS_STATISTICS
  soc_sand_ll_stat_increment_if_active(SOC_SAND_LL_ACCESS_DIRECTION_READ);;
#endif

  if (Soc_sand_physical_access.physical_read != NULL)
  {
    soc_sand_ret = Soc_sand_physical_access.physical_read(
                 array,
                 base_address,
                 offset,
                 size
               ) ;
  }
  else
  {
    soc_sand_ret = SOC_SAND_NULL_USER_CALLBACK_FUNC ;
    goto exit ;
  }

exit:
  return soc_sand_ret ;
}

STATIC SOC_SAND_RET
  soc_sand_physical_print_when_read_modify_write_to_chip(
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 mask,
    SOC_SAND_IN     uint32 data_to_write
  )
{
  SOC_SAND_RET
    soc_sand_ret ;
  soc_sand_ret = SOC_SAND_OK ;

  if(Soc_sand_physical_print_when_writing)
  {
    if(Soc_sand_physical_print_part_of_indirect_read   ||
       (Soc_sand_physical_print_part_of_indirect_write &&
        !Soc_sand_physical_print_indirect_write)       ||
       (Soc_sand_physical_print_first_reg > offset/4   ||
        Soc_sand_physical_print_last_reg  < offset/4)
      )
    {
      goto exit;
    }

    if(Soc_sand_physical_print_asic_style)
    {
      LOG_CLI((BSL_META("`READ_MOD_WRITE (14'h%x,32'h%x,32'h%x);\n\r"),
               offset/4,
               mask,
               data_to_write
               ));
    }
    else if (Soc_sand_physical_print_unit_or_base_address)
    {
      LOG_CLI((BSL_META("0x%08x 0x%08x 0x%08x [ mask 0x%X]\n\r"),
               PTR_TO_INT(base_address),
               offset/4,
               data_to_write,
               mask
               ));
    }
    else
    {
      LOG_CLI((BSL_META("0x%08x 0x%08x [ mask 0x%X]\n\r"),
               offset/4,
               data_to_write,
               mask
               ));
    }
  }

  if(!Soc_sand_physical_write_enable)
  {
    LOG_CLI((BSL_META("soc_sand_read_modify_write FAILED. Writing is disabled.\n\r"
                      "(0x%08x 0x%08x [mask 0x%X]).\n\r"),
             offset/4,
             data_to_write,
             mask
             ));
  }
  goto exit;
exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_read_modify_write(
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 shift,
    SOC_SAND_IN     uint32 mask,
    SOC_SAND_IN     uint32 data_to_write
  )
{
  SOC_SAND_RET ex ;
  uint32 tmp[1];
  
  Soc_sand_physical_print_part_of_read_modify_write = TRUE;
  ex = soc_sand_physical_read_from_chip( tmp, base_address, offset, sizeof(uint32)) ;
  if (SOC_SAND_OK != ex)
  {
    goto exit ;
  }
  
  *tmp &= ~mask ;
  *tmp |= SOC_SAND_SET_FLD_IN_PLACE(data_to_write, shift, mask) ;

#if SOC_SAND_PHYSICAL_PRINT_WHEN_WRITING
  if(!Soc_sand_physical_print_part_of_indirect_write ||
     !Soc_sand_physical_print_indirect_write
    )
  {
    soc_sand_physical_print_when_read_modify_write_to_chip(
      base_address, offset,mask,data_to_write
    );
  }
#endif
  
  ex  = soc_sand_physical_write_to_chip( tmp, base_address, offset, sizeof(uint32)) ;
  if (SOC_SAND_OK != ex)
  {
    goto exit ;
  }
  
exit:
  Soc_sand_physical_print_part_of_read_modify_write = FALSE;
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_READ_MODIFY_WRITE,
        "error in soc_sand_read_modify_write(): Cannot access chip",0,0,0,0,0,0) ;
  return ex ;
}



SOC_SAND_RET
  soc_sand_eci_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  VOLATILE uint32
    *ul_ptr;
  const VOLATILE uint32
    *local_buff_ptr ;
  uint32
    ui,
    local_offset,
    local_size;
#ifndef USING_CHIP_SIM
  uint32
    big_endian;
#endif
  SOC_SAND_RET
    ex;

  ex = SOC_SAND_OK ;
#ifndef USING_CHIP_SIM
  big_endian = Soc_sand_big_endian;
#endif

#if PRINT_LOW_LEVEL_ACESS
  LOG_INFO(BSL_LS_SOC_MANAGEMENT,
           (BSL_META("\r\n eci_write() base: 0x%X ; offset: 0x%X ; size %d ; data: 0x%X"), base_address, offset, size, *array));
#endif
  
  local_offset  = offset >> 2 ;
  local_size    = size   >> 2 ;
  ul_ptr        = base_address + local_offset ;

  
  local_buff_ptr = array;

  for (ui=0 ; ui<local_size ; ++ui)
  {
#ifdef USING_CHIP_SIM

    if ( chip_sim_task_is_alive() )
    {
      if (chip_sim_write(PTR_TO_INT(ul_ptr), *local_buff_ptr) )
      {
#if SOC_SAND_LOW_LEVEL_ERR_MSG

        logMsg(
          "soc_sand_physical_write_to_chip()"
          " chip_sim_write(0x%X) returned with an error\r\n",
          ul_ptr,0,0,0,0,0
        ) ;

#endif  
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
      local_buff_ptr++ ;
      ul_ptr++ ;
    }
    else
    {
      ex = SOC_SAND_ERR ;
      goto exit ;
    }

#else

    if(big_endian) 
    {
      *ul_ptr++ = *local_buff_ptr++ ;
    }
    else
    {
      *ul_ptr = SOC_SAND_BYTE_SWAP(*local_buff_ptr) ;
      ul_ptr++;
      local_buff_ptr++;
    }

#endif
  }
  goto exit ;
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_PHYSICAL_WRITE_TO_CHIP,
        "error in soc_sand_physical_write_to_chip(): Cannot access chip",
         PTR_TO_INT(array), PTR_TO_INT(base_address),
         offset, size, 0,0
  ) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_eci_read(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  const VOLATILE uint32
      *ul_ptr ;
  uint32
    ui,
    local_offset,
    local_size,
    local_value;
#if PRINT_LOW_LEVEL_ACESS
  uint32
    *tmp_array,
#endif
#ifndef USING_CHIP_SIM
  uint32
    big_endian;
#endif
  SOC_SAND_RET
    ex;

#if TRACE_LOW_LEVEL
  uint32
    before, after ;
  uint32
    elapsed ;
#endif

  ex = SOC_SAND_OK ;
#ifndef USING_CHIP_SIM
  big_endian = soc_sand_system_is_big_endian();
#endif
#if PRINT_LOW_LEVEL_ACESS
  tmp_array = array ;
#endif
  
  local_offset  = offset  >> 2 ;
  local_size    = size    >> 2 ;
  ul_ptr        = base_address + local_offset ;
  for (ui = 0 ; ui < local_size ; ui++)
  {
#ifdef USING_CHIP_SIM

    if ( chip_sim_task_is_alive())
    {
#if TRACE_LOW_LEVEL

      before = soc_sand_os_get_time_micro() ;

#endif
      if (chip_sim_read(PTR_TO_INT(ul_ptr), (UINT32*)array) )
      {
#if SOC_SAND_LOW_LEVEL_ERR_MSG

        logMsg(
          "soc_sand_physical_read_from_chip() chip_sim_read(0x%X) returned with an error\r\n",
          ul_ptr,0,0,0,0,0) ;

#endif  
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
#if TRACE_LOW_LEVEL

      after = soc_sand_os_get_time_micro() ;
      elapsed = after - before;

#endif
      array++ ;
      ul_ptr++ ;
    }
    else
    {
      ex = SOC_SAND_ERR ;
      goto exit ;
    }

    SOC_SAND_IGNORE_UNUSED_VAR(local_value);

#else

    if(big_endian) 
    {
      *array++ = *ul_ptr++ ;
    }
    else
    {
      local_value = *ul_ptr ;
      *array = SOC_SAND_BYTE_SWAP(local_value) ;
      array++;
      ul_ptr++;
    }

#endif
  }

#if PRINT_LOW_LEVEL_ACESS
  LOG_INFO(BSL_LS_SOC_MANAGEMENT,
           (BSL_META("\r\n eci_read() base: 0x%X ; offset: 0x%X ; size %d ; data: 0x%X"), base_address, offset, size, *tmp_array));
#endif
  goto exit ;
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_PHYSICAL_READ_FROM_CHIP,
        "error in soc_sand_physical_read_from_chip(): Cannot access chip", 
         PTR_TO_INT(array),
         PTR_TO_INT(base_address),
         offset,
         size,0,0) ;
  return ex ;
}



#if SOC_SAND_DEBUG



void
  soc_sand_physical_print_when_writing(
    uint32 do_print
  )
{
  Soc_sand_physical_print_when_writing = do_print;
}


void
  soc_sand_physical_access_print(
    void
  )
{

  if (Soc_sand_big_endian_was_checked == FALSE)
  {
    LOG_CLI((BSL_META("SOC_SAND did NOT checked and NOT decide on CPU BIG/LITTLE endian\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META("SOC_SAND checked and decide on CPU BIG/LITTLE endian\n\r")));
  }

  
  if (soc_sand_system_is_big_endian() == FALSE)
  {
    LOG_CLI((BSL_META("SOC_SAND identifies the CPU as LITTLE endian\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META("SOC_SAND identifies the CPU as BIG endian\n\r")));
  }

  LOG_CLI((BSL_META("Physical access:\n\r")));

  
  LOG_CLI((BSL_META("  + Device write access function:")));

  if (Soc_sand_physical_access.physical_write == soc_sand_eci_write)
  {
    LOG_CLI((BSL_META(" ECI - supplied by the driver.")));
  }
  else if (Soc_sand_physical_access.physical_write == NULL)
  {
    LOG_CLI((BSL_META(" NULL (probably an error).")));
  }
  else
  {
    LOG_CLI((BSL_META(" user supplied.")));
  }
  LOG_CLI((BSL_META("\n\r")));

  
  LOG_CLI((BSL_META("  + Device read access function:")));

  if (Soc_sand_physical_access.physical_read == soc_sand_eci_read)
  {
    LOG_CLI((BSL_META(" ECI - supplied by the driver.")));
  }
  else if (Soc_sand_physical_access.physical_read == NULL)
  {
    LOG_CLI((BSL_META(" NULL (probably an error).")));
  }
  else
  {
    LOG_CLI((BSL_META(" user supplied.")));
  }
  LOG_CLI((BSL_META("\n\r")));


}




SOC_SAND_RET
  soc_sand_eci_write_and_print(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  SOC_SAND_RET
    ex;
  uint32
    access_i;

  
  for(access_i=0; access_i<(size/4); ++access_i)
  {

    LOG_CLI((BSL_META("mem write 0x%04X data 0x%08x offset silent type gp1_fap20v_1 address_in_longs\n"),
             (offset+access_i*4),
             array[access_i]
             ));

  }

  
  ex = soc_sand_eci_write(array, base_address, offset, size);
  return ex;
}

SOC_SAND_RET
  soc_sand_eci_read_and_print(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  SOC_SAND_RET
    ex;
  uint32
    access_i;

  
  ex = soc_sand_eci_read(array, base_address, offset, size);

  
  for(access_i=0; access_i<(size/4); ++access_i)
  {
    LOG_CLI((BSL_META("Read Offset:0x%04X Value:0x%08x\n\r"),
             (offset + 4*access_i),
             array[access_i]
             ));
  }
  return ex;
}



#ifdef BROADCOM_DEBUG

#ifdef FUNCTION_TIME_PROFILING
#include <stdlib.h>
#include <stdio.h>
#include <sal/core/sync.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
 
typedef struct {
  unsigned long long tot_usec;
  struct timespec last_ts;
} ht_value_t;

typedef struct {
  void *addr;
} ht_key_t;

typedef struct entry_s {
	ht_key_t key;
  ht_value_t value;

	struct entry_s *next;
} entry_t;

static inline unsigned int ht_default_hash_func(const ht_key_t key) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline int ht_default_compare_func(const ht_key_t left, const ht_key_t right) __attribute__((no_instrument_function)) __attribute__((always_inline));

static inline unsigned int ht_default_hash_func(const ht_key_t key) 
{
  return (unsigned int)key.addr;
}

static inline int ht_default_compare_func(const ht_key_t left, const ht_key_t right) 
{
  return left.addr == right.addr ? 0 : 1;
}
 
 
typedef unsigned int (*ht_hash_func_t)(const ht_key_t key);

typedef int (*ht_compare_func_t)(const ht_key_t left, const ht_key_t right);

typedef struct {
	int size;
	struct entry_s **table;	
  
  ht_hash_func_t hash_func;
  ht_compare_func_t compare_func;

} hashtable_t;
 
typedef int (*ht_iter_cb_t)(ht_key_t key, ht_value_t value, void *opaque);

static inline hashtable_t *ht_create(int size, ht_hash_func_t hash_func, ht_compare_func_t compare_func) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline entry_t *ht_newpair( ht_key_t key, ht_value_t value ) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline int ht_set( hashtable_t *hashtable, ht_key_t key, ht_value_t value ) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline ht_value_t ht_get( hashtable_t *hashtable, ht_key_t key, char *found ) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline int ht_iterate(hashtable_t *ht, ht_iter_cb_t cb, void *opaque) __attribute__((no_instrument_function)) __attribute__((always_inline));


static inline hashtable_t *ht_create(int size, const ht_hash_func_t hash_func, const ht_compare_func_t compare_func) 
{
	hashtable_t *hashtable = NULL;
  ht_hash_func_t hash_func_tmp = hash_func != (ht_hash_func_t)NULL ? hash_func : ht_default_hash_func;
  ht_compare_func_t compare_func_tmp = compare_func != (ht_compare_func_t)NULL ? compare_func : ht_default_compare_func;
	int i;
 
	if (size < 1) {
    return NULL;
  }

  hashtable = malloc(sizeof(hashtable_t));
	if (hashtable == NULL) {
		return NULL;
	}

  hashtable->table = malloc(sizeof(entry_t *) * size);
  if (hashtable->table == NULL ) {
		return NULL;
	}

	for( i = 0; i < size; i++ ) {
		hashtable->table[i] = NULL;
	}
 
	hashtable->size = size;
  hashtable->hash_func = hash_func_tmp;
  hashtable->compare_func = compare_func_tmp;
 
	return hashtable;	
}
 

static inline entry_t *ht_newpair(ht_key_t key, ht_value_t value)
{
	entry_t *newpair;
 
  newpair = malloc(sizeof(entry_t));
	if (newpair == NULL) {
		return NULL;
	}

  newpair->key = key;
  newpair->value = value;
	newpair->next = NULL;
 
	return newpair;
}
 


static inline int ht_set( hashtable_t *hashtable, ht_key_t key, ht_value_t value )
{
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;
 
	bin = hashtable->hash_func(key) % hashtable->size;
 
	next = hashtable->table[bin];
 
	while (next != NULL && hashtable->compare_func(key, next->key)) {
		last = next;
		next = next->next;
	}
 
	
	if (next != NULL && hashtable->compare_func(key, next->key) == 0) {
    next->value = value;

	
	} else {
		newpair = ht_newpair(key, value);
    if (newpair == NULL) {
      return -1;
    }
 
		
		if(next == hashtable->table[bin]) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
	
		
		} else if ( next == NULL ) {
			last->next = newpair;
	
		
		} else  {
			newpair->next = next;
			last->next = newpair;
		}
	}

  return 0;
}
 

static inline ht_value_t ht_get( hashtable_t *hashtable, ht_key_t key, char *found )
{
	int bin = 0;
	entry_t *pair;
  ht_value_t garbage;
 
	bin = hashtable->hash_func(key) % hashtable->size;
 
	
	pair = hashtable->table[bin];
	while (pair != NULL && hashtable->compare_func(key, pair->key)) {
		pair = pair->next;
	}
 
	
	if( pair == NULL || hashtable->compare_func(key, pair->key)) {
    *found = 0;
		return garbage;
	} 
    *found = 1;
    return pair->value;
  
}

static inline int ht_iterate(hashtable_t *ht, ht_iter_cb_t cb, void *opaque)
{
  int i;
  entry_t *e;
  for (i=0; i<ht->size; i++) {
    for (e = ht->table[i]; e != NULL; e = e->next) {
      int rv = cb(e->key, e->value, opaque);
      if (rv) {
        return rv;
      }
    }
  }

  return 0;
}
 
static hashtable_t *ht;
static FILE *htb_file;
static char *htb_file_name = "function-call.dump";
enum INSTRUMENT_STATUS{
  
  INSTRUMENT_STATUS_NEW,
  
  INSTRUMENT_STATUS_NO_INSTRUMENT,
  
  INSTRUMENT_STATUS_DO_INSTRUMENT,
  
  INSTRUMENT_STATUS_ERROR
};
static int status = INSTRUMENT_STATUS_NEW;

static inline int flush_htb_cb(ht_key_t key, ht_value_t value, void *opaque) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline int flush_htb_cb(ht_key_t key, ht_value_t value, void *opaque)
{
  int rv = fprintf(htb_file, "%p: %llu\n", key.addr, value.tot_usec);
  if (rv == 0){
    printf("Error writing to file!\n");
  }

  return 0;
}

static inline void flush_htb() __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline void flush_htb()
{
  
    status = INSTRUMENT_STATUS_NO_INSTRUMENT;
    htb_file = fopen(htb_file_name, "w");
    if (htb_file == NULL) {
      printf("Error - Could not open file %s for rw.\n", htb_file_name);
      return;
    }

    ht_iterate(ht, flush_htb_cb, NULL);
    fclose(htb_file);
    status = INSTRUMENT_STATUS_DO_INSTRUMENT;
  
  
}


   
inline static int init_instrumentation() __attribute__((no_instrument_function)) __attribute__((always_inline));
inline static int init_instrumentation()
{
  char *error_func;
  int rv;

  

  error_func = "atexit";
  rv = atexit(flush_htb);
  if (rv != 0) {
    goto exit;
  }

  error_func = "ht_create";
  ht = ht_create(1000000, NULL, NULL);
  if (ht == NULL) {
    goto exit;
  }
  
exit:
  if (rv != 0) {
    printf("Error - Failed doing %s (%d).\n", error_func, rv);
    return rv;
  } 
  return 0;
}


static inline void instrument_enter_callback(void *func) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline void instrument_enter_callback(void *func)
{
  int rv;
  ht_key_t k;
  ht_value_t v;
  char found;
  
  k.addr = func;

  v = ht_get(ht, k, &found);
  if (!found) {
    v.tot_usec = 0;
  }

  rv = clock_gettime(CLOCK_REALTIME, &v.last_ts);

  if (rv) {
    printf("Error in clock_gettime.\n");
    v.last_ts.tv_sec = 0;
    v.last_ts.tv_nsec = 0;
  }

  rv = ht_set(ht, k, v);
  if (rv != 0) {
    printf("Error - Failed doing ht_set (%d).\n", rv);
  }

}

static inline void instrument_exit_callback(void *func) __attribute__((no_instrument_function)) __attribute__((always_inline));
static inline void instrument_exit_callback(void *func) 
{
  int rv;
  ht_key_t k;
  ht_value_t v;
  struct timespec ts;
  long long call_time;
  char found;
  
  k.addr = func;

  v = ht_get(ht, k, &found);
  if (!found) {
    printf("Error - Could not find key at exit_callback.\n");
    return;
  }

  rv = clock_gettime(CLOCK_REALTIME, &ts);
  if (rv) {
    printf("Error in clock_gettime.\n");
    return;
  }

  call_time = (long long)(ts.tv_sec - v.last_ts.tv_sec) * 1000000000 + ts.tv_nsec - v.last_ts.tv_nsec;
  if (call_time < 0) {
    printf("Error - Negative call time!\n");
    return;
  }
  v.tot_usec += call_time;

  rv = ht_set(ht, k, v);
  if (rv != 0) {
    printf("Error - Failed doing ht_set (%d).\n", rv);
  }
}

static int init_status = 0;
static sal_thread_t main_thread;

extern inline void __wrap___cyg_profile_func_enter (void *this_fn, void *call_site) __attribute__((no_instrument_function)) __attribute__((always_inline));
inline void __wrap___cyg_profile_func_enter (void *this_fn, void *call_site)
{
  
  if (init_status == 0) {
    int rv;

    main_thread = sal_thread_self();
    rv = init_instrumentation();
    
    if (rv) {
      printf("Error - Could not initialize instrumentation!\n");
      init_status = -1;
    } else {
      init_status = 1;
    }
  } else if (main_thread != sal_thread_self()) {
    return;
  } else if (init_status == 1) {
    
    instrument_enter_callback(this_fn);
    
  }
  
}

extern inline void __wrap___cyg_profile_func_exit  (void *this_fn, void *call_site) __attribute__((no_instrument_function)) __attribute__((always_inline));
inline void __wrap___cyg_profile_func_exit  (void *this_fn, void *call_site)
{

  if (init_status == 1 && main_thread == sal_thread_self()) {
    
    instrument_exit_callback(this_fn);
    
  }
}
#endif 
#endif 


#endif



