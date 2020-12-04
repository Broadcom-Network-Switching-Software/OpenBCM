/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifndef __SOC_SAND_LOW_LEVEL_H_INCLUDED__

#define __SOC_SAND_LOW_LEVEL_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>


#define VOLATILE  volatile
#define SOC_SAND_PHYSICAL_PRINT_WHEN_WRITING SOC_SAND_DEBUG



#undef SOC_SAND_LL_ACCESS_STATISTICS

#ifdef SOC_SAND_LL_ACCESS_STATISTICS

#define SOC_SAND_LL_NOF_TAGGED_STAT_MAX   20
#define SOC_SAND_LL_NOF_STAT_IDS          (SOC_SAND_LL_NOF_TAGGED_STAT_MAX+1)


#define SOC_SAND_LL_STAT_ID_TOTAL           SOC_SAND_LL_NOF_TAGGED_STAT_MAX

typedef enum
{
  SOC_SAND_LL_ACCESS_DIRECTION_READ   = 0,
  SOC_SAND_LL_ACCESS_DIRECTION_WRITE  = 1,
  SOC_SAND_LL_NOF_ACCESS_DIRECTIONS   = 2
} SOC_SAND_LL_ACCESS_DIRECTION;




void
  soc_sand_ll_stat_clear(void);


void 
  soc_sand_ll_stat_is_active_set(
    uint32 stat_ndx,
    uint8 is_active

  );

void 
  soc_sand_ll_stat_print(void);


void
  soc_sand_ll_stat_increment_if_active(
    SOC_SAND_LL_ACCESS_DIRECTION direction
  );

#endif 


#define SOC_SAND_LL_TIMER

#ifdef SOC_SAND_LL_TIMER


#define SOC_SAND_LL_TIMER_MAX_NOF_TIMERS               (50)
#define SOC_SAND_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME  (40)

typedef struct
{
  
  char name[SOC_SAND_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME];

  
  uint32 nof_hits;

  uint32 active;

  
  uint32 start_timer;

  
  uint32 total_time;

}SOC_SAND_LL_TIMER_FUNCTION;




#define ARAD_KBP_TIMERS_NONE 						0
#define ARAD_KBP_TIMERS_APPL 						1
#define ARAD_KBP_TIMERS_BCM							2
#define ARAD_KBP_TIMERS_SOC							3
#define ARAD_KBP_TIMERS_ROUTE_ADD_COMMIT 			4
#define ARAD_KBP_TIMERS_ADD_RECORDS_INTO_TABLES		5

typedef enum
{
	ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_REMOVE 	= ARAD_KBP_TIMERS_ADD_RECORDS_INTO_TABLES+1,
	ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD,
	ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD_ROP,
	ARAD_KBP_IPV4_TIMERS_NOF_TIMERS
} ARAD_KBP_IPV4_TIMERS;

typedef enum
{
	ARAD_KBP_ACL_TIMERS_ROUTE_REMOVE 		= ARAD_KBP_TIMERS_ADD_RECORDS_INTO_TABLES+1,
	ARAD_KBP_ACL_TIMERS_ROUTE_ADD,
	ARAD_KBP_ACL_TIMERS_NOF_TIMERS
} ARAD_KBP_ACL_TIMERS;

extern SOC_SAND_LL_TIMER_FUNCTION Soc_sand_ll_timer_cnt[SOC_SAND_LL_TIMER_MAX_NOF_TIMERS];
extern uint8 Soc_sand_ll_is_any_active;
extern uint32  Soc_sand_ll_timer_total;


uint32
  soc_sand_ll_get_time_in_ms(void);




void
  soc_sand_ll_timer_clear(void);

void
  soc_sand_ll_timer_stop_all(void);

void
  soc_sand_ll_timer_set(
    SOC_SAND_IN char name[SOC_SAND_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME],
    SOC_SAND_IN uint32 fn_ndx
  );


void
  soc_sand_ll_timer_stop(
    SOC_SAND_IN uint32 fn_ndx
  );



void 
  soc_sand_ll_timer_print_all(void);



#endif 


extern uint32
  Soc_sand_big_endian;
extern uint32
  Soc_sand_big_endian_was_checked;

SOC_SAND_RET
  soc_sand_check_chip_type_decide_big_endian(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 version_reg,
    SOC_SAND_IN uint32 chip_type_shift,
    SOC_SAND_IN uint32 chip_type_mask
  );

uint32
  soc_sand_system_is_big_endian(
    void
  );

void
  soc_sand_ssr_set_big_endian(
    uint32 soc_sand_big_endian_was_checked,
    uint32 soc_sand_big_endian
  );


#ifdef  SAND_LOW_LEVEL_SIMULATION
uint8
  soc_sand_low_is_sim_active_get(void);

void soc_sand_low_is_sim_active_set(
       SOC_SAND_IN uint8 is_sim_active
     );
#endif



void
  soc_sand_ssr_get_big_endian(
    uint32 *soc_sand_big_endian_was_checked,
    uint32 *soc_sand_big_endian
  );

extern uint32 Soc_sand_physical_write_enable;

void
  soc_sand_set_physical_write_enable(
    uint32 physical_write_enable
  );

void
  soc_sand_get_physical_write_enable(
    uint32 *physical_write_enable
  );




typedef
  SOC_SAND_RET
    (*SOC_SAND_PHYSICAL_WRITE_ACCESS_PTR)(
      SOC_SAND_IN     uint32 *array,
      SOC_SAND_INOUT  uint32 *base_address,
      SOC_SAND_IN     uint32 offset,
      SOC_SAND_IN     uint32 size
   ) ;


typedef
  SOC_SAND_RET
    (*SOC_SAND_PHYSICAL_READ_ACCESS_PTR)(
      SOC_SAND_INOUT  uint32 *array,
      SOC_SAND_IN     uint32 *base_address,
      SOC_SAND_IN     uint32 offset,
      SOC_SAND_IN     uint32 size
    ) ;

typedef struct
{
  
  SOC_SAND_PHYSICAL_WRITE_ACCESS_PTR physical_write;

  
  SOC_SAND_PHYSICAL_READ_ACCESS_PTR  physical_read;
} SOC_SAND_PHYSICAL_ACCESS;


extern uint32 Soc_sand_physical_print_when_writing;
extern uint32 Soc_sand_physical_print_asic_style;
extern uint32 Soc_sand_physical_print_indirect_write;
extern uint32 Soc_sand_physical_print_part_of_indirect_read;
extern uint32 Soc_sand_physical_print_part_of_indirect_write;
extern uint32 Soc_sand_physical_print_part_of_read_modify_write;
extern uint32 Soc_sand_physical_print_unit_or_base_address;

extern uint32 Soc_sand_physical_print_first_reg ;
extern uint32 Soc_sand_physical_print_last_reg ;




SOC_SAND_RET
  soc_sand_set_physical_access_hook(
    SOC_SAND_IN SOC_SAND_PHYSICAL_ACCESS* physical_access
  );

SOC_SAND_RET
  soc_sand_get_physical_access_hook(
    SOC_SAND_OUT SOC_SAND_PHYSICAL_ACCESS* physical_access
  );





SOC_SAND_RET
  soc_sand_eci_read(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

SOC_SAND_RET
  soc_sand_eci_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

SOC_SAND_RET
  soc_sand_physical_write_to_chip(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  ) ;

SOC_SAND_RET
  soc_sand_physical_read_from_chip(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  ) ;


SOC_SAND_RET
  soc_sand_read_modify_write(
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 shift,
    SOC_SAND_IN     uint32 mask,
    SOC_SAND_IN     uint32 data_to_write
  ) ;


void
  soc_sand_set_print_when_writing(
    uint32 physical_print_when_writing,
    uint32 asic_style,
    uint32 indirect_write
  );

void
  soc_sand_get_print_when_writing(
    uint32 *physical_print_when_writing,
    uint32 *asic_style,
    uint32 *indirect_write
  );

#if SOC_SAND_DEBUG


void
  soc_sand_physical_print_when_writing(
    uint32 do_print
  );

void
  soc_sand_physical_access_print(
    void
  );

void
  soc_sand_set_print_when_writing_part_of_indirect_write(
    uint32 part_of_indirect
  );

void
  soc_sand_set_print_when_writing_part_of_indirect_read(
    uint32 part_of_indirect
  );



SOC_SAND_RET
  soc_sand_eci_write_and_print(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );


SOC_SAND_RET
  soc_sand_eci_read_and_print(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

void
  soc_sand_get_print_when_writing_unit_or_base_address(
    uint32 *unit_or_base_address
  );

void
  soc_sand_set_print_when_writing_unit_or_base_address(
    uint32 unit_or_base_address
  );

void
  soc_sand_set_print_when_writing_reg_range(
    uint32 first_reg,
    uint32 last_reg
  );


#endif

#ifdef  __cplusplus
}
#endif


#endif
