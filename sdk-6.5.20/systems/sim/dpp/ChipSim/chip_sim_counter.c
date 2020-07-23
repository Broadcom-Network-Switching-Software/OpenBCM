/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include  "chip_sim_counter.h"
#include  "chip_sim_mem.h"
#include  "chip_sim_log.h"

#include <stdlib.h>

/*
 * Counters object.
 */
CHIP_SIM_COUNTER* Cnt[CHIP_SIM_NOF_CHIPS] ={0} ;
CHIP_SIM_COUNTER* Cnt_mirror = NULL;


/*****************************************************
*NAME
*   chip_sim_cntr_init
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:57
*FUNCTION:
*  Initialize the counter module
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_cntr_init(
    void
    )
{
  int
    i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    Cnt[i] = NULL ;
  }
  Cnt_mirror = NULL;

  return OK;
}

/*****************************************************
*NAME
*  chip_sim_cntr_init_chip_specifics
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:01
*FUNCTION:
*  Initialize with specifics the counter module
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) CHIP_SIM_COUNTER * const m_counters
*    (2) SOC_SAND_IN int i_nof_mirror_counters

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
SOC_SAND_RET
  chip_sim_cntr_init_chip_specifics(
    CHIP_SIM_COUNTER * const m_counters
    )
{
  Cnt_mirror = m_counters ;
  return OK;
}

/*****************************************************
*NAME
*  chip_sim_cntr_malloc
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:05
*FUNCTION:
*  Malloc the counter module
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_cntr_malloc(
    void
    )
{
  STATUS
    status = OK;
  int i;
  int nof_mirror_counters = 0 ;

  for (;Cnt_mirror[nof_mirror_counters].chip_offset!=INVALID_ADDRESS; nof_mirror_counters++)
  {
  }
  nof_mirror_counters++;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    int counter_i;
    Cnt[i] = (CHIP_SIM_COUNTER*)CHIP_SIM_MALLOC(nof_mirror_counters*sizeof(CHIP_SIM_COUNTER), "Cnt[i]");
    if (NULL == Cnt[i])
    {
      status = ERROR ;
      chip_sim_log_run("chip_sim_interrupt_malloc(): malloc failed\r\n");
      chip_sim_cntr_free();
      GOTO_FUNC_EXIT_POINT;
    }

    for (counter_i=0; counter_i<nof_mirror_counters; counter_i++)
    {
      Cnt[i][counter_i] = Cnt_mirror[counter_i] ;
    }
  }
FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_cntr_free
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:09
*FUNCTION:
*  Free the counter module
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  chip_sim_cntr_free(
    void
    )
{
  int i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    if (NULL != Cnt[i])
    {
      CHIP_SIM_FREE(Cnt[i]);
    }
    Cnt[i] = NULL;
  }
}

/*****************************************************
*NAME
*   chip_sim_counter
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 18-Sep-02 16:41:21
*FUNCTION:
*  Initialize the counter acoording to the asked profil.
*   So we have several counters profile to choose from.
*   For quick initilizations.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN    int              profile
*    (2) SOC_SAND_INOUT CHIP_SIM_COUNTER *counter
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_counter(
    SOC_SAND_IN    int              profile,
    SOC_SAND_INOUT CHIP_SIM_COUNTER *counter
    )
{
  STATUS
    status = OK ;

  return status ;
}

/*****************************************************
*NAME
*   chip_sim_counter_run
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:13
*FUNCTION:
*  1. for every counter
*  2. get the filed.
*  3. increment and boundary checking
*  4. write
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 time_diff
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_counter_run(
    SOC_SAND_IN UINT32 time_diff
    )
{
  const CHIP_SIM_COUNTER
    *counter_p;
  STATUS
    status = OK;
  UINT32
    reg_val,
    counter_val,
    counter_diff;
  int
    chip_i;

  /* 1 */
  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    for (counter_p=Cnt[chip_i]; counter_p->chip_offset!=INVALID_ADDRESS; counter_p++ )
    {
      /* 2 */
      if( chip_sim_mem_read(chip_i, counter_p->chip_offset, &reg_val) )
      {/* counter_p->chip_offset ERROR*/
        char msg[160];
        sal_sprintf(msg,"chip_sim_counter_run, error -- chip_sim_mem_read(chip_i(%i), counter_p->chip_offset(0x%X)) failed\r\n",
                    chip_i, counter_p->chip_offset);
        chip_sim_log_run(msg);
        status = ERROR;
        GOTO_FUNC_EXIT_POINT;
      }
      counter_val = SOC_SAND_GET_FLD_FROM_PLACE(reg_val, counter_p->shift, counter_p->mask);
      reg_val = (reg_val & ~(counter_p->mask)) ;

      /* 3 */
      if (counter_p->is_random)
      {
        counter_val = sal_rand();
        if (0 != counter_p->max)
        {
          counter_val %= (counter_p->max);
        }
      }
      else
      {
        counter_diff = (time_diff*(counter_p->count_per_sec))/1000;
        counter_val += counter_diff;
      }

      if (counter_val > counter_p->max)
      {
        counter_val = counter_p->max ;
      }

      if (counter_val < counter_p->min)
      {
        counter_val = counter_p->min ;
      }

      /* 4 */
      reg_val |= SOC_SAND_SET_FLD_IN_PLACE(counter_val, (counter_p->shift), (counter_p->mask)) ;
      if( chip_sim_mem_write(chip_i, counter_p->chip_offset, reg_val) )
      {/* counter_p->chip_offset ERROR*/
        char msg[160];
        sal_sprintf(msg,"chip_sim_counter_run, error -- chip_sim_mem_write(chip_i(%i), counter_p->chip_offset(0x%X)) failed\r\n",
                    chip_i, counter_p->chip_offset);
        chip_sim_log_run(msg);
        status = ERROR;
        GOTO_FUNC_EXIT_POINT;
      }
    }
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_counter_change
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:28
*FUNCTION:
* 1. search
* 2. write
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 chip_offset
*    (3) SOC_SAND_IN uint8   is_random
*    (4) SOC_SAND_IN UINT32 count_per_sec
*    (5) SOC_SAND_IN UINT32 min
*    (6) SOC_SAND_IN UINT32 max

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_counter_change(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 chip_offset,
    SOC_SAND_IN uint8   is_random,
    SOC_SAND_IN UINT32 count_per_sec,
    SOC_SAND_IN UINT32 min,
    SOC_SAND_IN UINT32 max
    )
{
  CHIP_SIM_COUNTER
    *counter_p;
  STATUS
    status = ERROR;

  /* 1 */
  for (counter_p=Cnt[chip_ndx]; counter_p->chip_offset!=INVALID_ADDRESS; counter_p++ )
  {
    if (chip_offset == counter_p->chip_offset)
    {
      /* 2 */
      counter_p->is_random = is_random;
      counter_p->count_per_sec = count_per_sec;
      counter_p->min = min;
      counter_p->max = max;
      status = OK;
      GOTO_FUNC_EXIT_POINT;
    }
  }


FUNC_EXIT_POINT:
  return status;
}
