/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _MSC_VER
#pragma warning(disable:4308)
#endif /* _MSC_VER */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include "chip_sim_indirect.h"
#include "chip_sim_mem.h"
#include "chip_sim_log.h"
#include "chip_sim_task.h"
#include "chip_sim.h"


#include <string.h>

/*
 * Every allocated 32-bit in the indirect is filled with
 * running number starting from CHIP_SIM_INDIRECT_LONG_FILLER
 */
/*#define CHIP_SIM_INDIRECT_LONG_FILLER  0xCC000000*/
#define CHIP_SIM_INDIRECT_LONG_FILLER  0x00000000

/*
 * Indierct knowledge-base.
 */
struct
{
  INT32 delay_milis ;

  CHIP_SIM_INDIRECT_BLOCK* blocks;

  uint8   has_delayed_task[CHIP_SIM_NOF_CHIPS];
  INT32 delayed_task_time_milis[CHIP_SIM_NOF_CHIPS];
} Indirect;

/*
 * Count how many un-initialized address were accessed.
 * Un-Initialized address is when the value CHIP_SIM_INDIRECT_LONG_FILLER
 * is writen there.
 */
unsigned long
  Indirect_read_un_initialized_counter = 0;

/*****************************************************
*NAME
*  chip_sim_indirect_init
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:35
*FUNCTION:
*  Clear from the code.
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
  chip_sim_indirect_init(
    void
    )
{
  int
    i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    Indirect.has_delayed_task[i] = FALSE ;
    Indirect.delayed_task_time_milis[i] = 0 ;
  }
  Indirect.delay_milis = 0 ;

  Indirect.blocks = NULL ;

 }


/*****************************************************
*NAME
*  chip_sim_indirect_init_chip_specifics
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:39
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) CHIP_SIM_INDIRECT_BLOCK* indirect_blocks
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
  chip_sim_indirect_init_chip_specifics(
    CHIP_SIM_INDIRECT_BLOCK* indirect_blocks
  )
{
  int
    i;
  CHIP_SIM_INDIRECT_BLOCK*
    block_p;

  Indirect.blocks  =  indirect_blocks;

  for (block_p=Indirect.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
    {
      block_p->base[i] = NULL ;
    }
  }
}

/*****************************************************
*NAME
*  chip_sim_indirect_malloc
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:44
*FUNCTION:
*  Clear from the code.
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
  chip_sim_indirect_malloc(
    void
  )
{
  STATUS
    status = OK;
  UINT32
    nof_addresses =0;
  int
    i;
  CHIP_SIM_INDIRECT_BLOCK*
    block_p;

  for (block_p=Indirect.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    nof_addresses = block_p->end_address - block_p->start_address + 1;
    for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
    {
      block_p->base[i] =
        (UINT32*)CHIP_SIM_MALLOC(nof_addresses*sizeof(UINT32)*block_p->nof_longs_to_move, "block_p->base[i]");
      if (NULL == block_p->base[i])
      {
        chip_sim_log_run("failed to alloc in chip_sim_indirect_malloc()\r\n");
        status = ERROR ;
        chip_sim_mem_free();
        GOTO_FUNC_EXIT_POINT;
      }
      {
        /*
         * Initialize the the indirect allocated memory with
         * CHIP_SIM_INDIRECT_LONG_FILLER+.... to be marked.
         * Debuging porpuses.
         */
        UINT32
          u32_i;
        UINT32
          filler_i;
        sal_memset(
          block_p->base[i],
          0x0,
          nof_addresses*sizeof(UINT32)*block_p->nof_longs_to_move
        );
        filler_i = CHIP_SIM_INDIRECT_LONG_FILLER;
        for (u32_i=0; u32_i<nof_addresses; u32_i++)
        {
          (block_p->base[i])[u32_i] = filler_i;
          if (CHIP_SIM_INDIRECT_LONG_FILLER != 0x0)
          {
            /* if filler not zero incremant it, so every place as different value*/
            filler_i++;
            if (filler_i==0x0)
            {
              filler_i = CHIP_SIM_INDIRECT_LONG_FILLER;
            }
          }
        }
      }
    }
  }

  Indirect_read_un_initialized_counter = 0;

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_indirect_free
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:50
*FUNCTION:
*  Clear from the code.
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
  chip_sim_indirect_free(
    void
  )
{
  int
    i;
  CHIP_SIM_INDIRECT_BLOCK*
    block_p;

  if (NULL == Indirect.blocks)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  for (block_p=Indirect.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
    {
      if (NULL != block_p->base[i])
      {
        CHIP_SIM_FREE(block_p->base[i]) ;
      }
      block_p->base[i] = NULL;
    }
  }
  Indirect.blocks = NULL;

FUNC_EXIT_POINT:
  return;
}



/*****************************************************
*NAME
*   chip_sim_indirect_triger
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:55
*FUNCTION:
*  Clear from the code.
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
  chip_sim_indirect_triger(
    SOC_SAND_IN UINT32 time_diff
    )
{
  STATUS
    status = OK;
  int
    i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    if (Indirect.has_delayed_task[i])
    {
      Indirect.delayed_task_time_milis[i] -= time_diff ;
      if (Indirect.delayed_task_time_milis[i]<0)
      {
        Indirect.delayed_task_time_milis[i] = 0;
        Indirect.has_delayed_task[i]=FALSE;
        if (chip_sim_indirect_op(i))
        {
          chip_sim_log_run("chip_sim_indirect_triger\r\n");
          status = ERROR;
          GOTO_FUNC_EXIT_POINT;
        }
      }
    }
  }

FUNC_EXIT_POINT:
  return status;
}



/*****************************************************
*NAME
*   chip_sim_indirect_set_delay_time
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:59
*FUNCTION:
*  Sets the Delay time afterwards the indirect trigger comes done.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 delay in ,mili-seconds

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
  chip_sim_indirect_set_delay_time(
    SOC_SAND_IN UINT32 delay
    )
{
  Indirect.delay_milis = delay ;
  return OK;
}

/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_indirect_is_access_trig
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:03
*FUNCTION:
*  Check if the 'chip_offset' is one of the indirect triggers offset.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN UINT32 chip_offset
*      Offset in the device.
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
uint8
  chip_sim_indirect_is_access_trig(
    SOC_SAND_IN UINT32 chip_offset
  )
{
  uint8
    answer = FALSE;
  CHIP_SIM_INDIRECT_BLOCK
    *block_p = NULL;


  if (NULL == Indirect.blocks)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  for (block_p=Indirect.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    if (chip_offset == block_p->access_trig_offset)
    {
      answer = TRUE;
      GOTO_FUNC_EXIT_POINT;
    }
  }


FUNC_EXIT_POINT:
  return answer ;
}

/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_indirect_get_access_trig
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:07
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx

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
UINT32
  chip_sim_indirect_get_access_trig(
    SOC_SAND_IN int                      chip_ndx,
    SOC_SAND_IN CHIP_SIM_INDIRECT_BLOCK* block_p
  )
{
  UINT32
    trig = 0;
  char
    msg[160];

  if (chip_sim_mem_read(chip_ndx, block_p->access_trig_offset, &trig))
  {
    sal_sprintf(msg,"chip_ndx(%d) block_p->access_trig_offset(0x%08X) ERROR\n\r",
                chip_ndx, block_p->access_trig_offset);
    chip_sim_log_run(msg);
  }
  return trig;
}

/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_indirect_set_access_trig
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:12
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx
*    (2) SOC_SAND_IN UINT32 val

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
  chip_sim_indirect_set_access_trig(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 val
    )
{
  STATUS
    status = OK;
  char
    msg[160];
  CHIP_SIM_INDIRECT_BLOCK
    *block_p = NULL;
  uint32
    iter_cnt = 0;

  for (block_p=Indirect.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    if (++iter_cnt > 1000)
    {
      /*
       *	Exceeded maximal expected iterations - probably infinite loop
       */
      return ERROR;
    }
    if (chip_sim_mem_write(chip_ndx, block_p->access_trig_offset, val))
    {
      sal_sprintf(msg,"chip_ndx(%d) block_p->access_trig_offset(0x%08X) ERROR\n\r",
                chip_ndx, block_p->access_trig_offset);
      chip_sim_log_run(msg);
      status = ERROR;
    }

    
  }

  return status ;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_indirect_get_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:16
*FUNCTION:
*  Clear from the code.
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
UINT32
  chip_sim_indirect_get_delay(
    void
    )
{
  return Indirect.delay_milis ;
}

/* Under Interrupt Halting No OS Calls */
/*set the delay task .*/
/*****************************************************
*NAME
*  chip_sim_indirect_set_delay_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:20
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN chip_ndx

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
  chip_sim_indirect_set_delay_op(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK ;

  if (Indirect.has_delayed_task[chip_ndx] == TRUE)
  {
    /* an error by the driver */
    chip_sim_log_run("chip_sim_indirect_set_delay_op: try to set Indirect -- \r\n"
                     "while Indirect is already working ..\r\n");
    status = ERROR ;
    goto chip_sim_mem_set_indirect_delay_op_exit ;
  }

  Indirect.has_delayed_task[chip_ndx] = TRUE ;
  Indirect.delayed_task_time_milis[chip_ndx] = Indirect.delay_milis ;

chip_sim_mem_set_indirect_delay_op_exit:
  return status ;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*   chip_sim_indirect_get_access_address
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:25
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx

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
UINT32
  chip_sim_indirect_get_access_address(
    SOC_SAND_IN int                      chip_ndx,
    SOC_SAND_IN CHIP_SIM_INDIRECT_BLOCK* block_p
  )
{
  UINT32
    address = 0x0;
  char
    msg[160];

  if (chip_sim_mem_read(chip_ndx, block_p->access_address_offset,&address))
  {
    sal_sprintf(msg,"chip_ndx(%d) block_p->access_address_offset(0x%08X) ERROR\n\r",
              chip_ndx, block_p->access_address_offset);
    chip_sim_log_run(msg);
  }

  return address ;
}

/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_indirect_get_write_value
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:29
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx

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
UINT32
  chip_sim_indirect_get_write_value(
    SOC_SAND_IN int                      chip_ndx,
    SOC_SAND_IN CHIP_SIM_INDIRECT_BLOCK* block_p,
    SOC_SAND_OUT UINT32                  value[10]
    )
{
  int
    i;
  char
    msg[160];

  for (i=0; i<block_p->nof_longs_to_move; i++)
  {
    if (chip_sim_mem_read(chip_ndx, block_p->write_val_offset+i*4,&(value[i])))
    {
      sal_sprintf(msg,"chip_ndx(%d) block_p->write_val_offset(0x%08X) ERROR\n\r",
                chip_ndx, block_p->write_val_offset);
      chip_sim_log_run(msg);
    }
  }

  return value[1] ;
}

/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*   chip_sim_indirect_address_to_offset
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:33
*FUNCTION:
*  1. for every block
*  2. check that this Indirect is between start and end of the block
*  3. return the offset from base and the block pointer
*
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 access_address
*    (2) SOC_SAND_OUT UINT32 *access_offset
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
  chip_sim_indirect_address_to_offset(
    SOC_SAND_IN  UINT32                   access_address,
    SOC_SAND_IN  CHIP_SIM_INDIRECT_BLOCK* block_p,
    SOC_SAND_OUT UINT32*                  access_offset
    )
{
  STATUS
    status = ERROR ;

  *access_offset = INVALID_ADDRESS;
  /*1*/

  /*2*/
  if (access_address >= block_p->start_address)
  {
    if (access_address <= block_p->end_address)
    {
      /*3*/
      status = OK;
      *access_offset = access_address - block_p->start_address ;
      GOTO_FUNC_EXIT_POINT;
    }
  }

FUNC_EXIT_POINT:
  return status ;
}

/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*   chip_sim_indirect_read
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:38
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 access_address

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
  chip_sim_indirect_read(
    SOC_SAND_IN int                      chip_ndx,
    SOC_SAND_IN UINT32                   access_address,
    SOC_SAND_IN CHIP_SIM_INDIRECT_BLOCK* block_p
  )
{
  STATUS
    status = OK ;
  UINT32
    long_offset,
    word_size,
    value ;
  int
    i;


  if (chip_sim_indirect_address_to_offset(access_address, block_p, &long_offset))
  {
    chip_sim_log_run("chip_sim_indirect_read: address in not in Indirect ...\r\n");
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  word_size = block_p->nof_longs_to_move;
  for (i=0; i<block_p->nof_longs_to_move; i++)
  {
    value = *((block_p->base[chip_ndx])+long_offset * word_size +i);
    /*
     * Keep track of specific unitialized places.
     */
    /* CHIP_SIM_INDIRECT_LONG_FILLER may be changed and be changed to be greater than 0 */
    /* coverity[unsigned_compare : FALSE] */
    if (value >= CHIP_SIM_INDIRECT_LONG_FILLER)
    {
      Indirect_read_un_initialized_counter ++;
    }

    if (chip_sim_log_get_spy_low_mem())
    {
      char
        msg[100];
      sal_sprintf(msg, "r_ind chip_ndx(%d) time(%lu) long_offset+i(0x%X) val(0x%X)\r\n",
                    chip_ndx, (long unsigned)chip_sim_task_get_mili_time(), long_offset+i, value);
      chip_sim_log_run(msg);
    }

    if (chip_sim_mem_write(chip_ndx, (block_p->read_result_address)+i*4, value))
    {
      chip_sim_log_run("chip_sim_indirect_read: chip_sim_mem_write failed -- \r\n"
                       " probably configuration ERROR ..\r\n");
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }

FUNC_EXIT_POINT:
  return status ;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*   chip_sim_indirect_write
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:03:42
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 access_address
*    (3) SOC_SAND_IN UINT32 write_value

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
  chip_sim_indirect_write(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 access_address,
    SOC_SAND_IN UINT32 write_value[10],
    SOC_SAND_IN CHIP_SIM_INDIRECT_BLOCK* block_p
  )
{
  STATUS
    status = OK ;
  UINT32
    long_offset,
    word_size,
    i;

  if (chip_sim_indirect_address_to_offset(access_address, block_p, &long_offset))
  {
    chip_sim_log_run("chip_sim_indirect_write: address in not in Indirect ...\r\n");
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }
  word_size = block_p->nof_longs_to_move;
  for (i=0; i<word_size; i++)
  {
    *(block_p->base[chip_ndx]+(long_offset * word_size)+i) = write_value[i] ;

    if (chip_sim_log_get_spy_low_mem())
    {
      char
        msg[100];
      sal_sprintf(msg, "w_ind chip_ndx(%d) time(%lu) long_offset+i(0x%X) val(0x%X)\r\n",
              chip_ndx, (long unsigned)chip_sim_task_get_mili_time(), long_offset+i, write_value[i]);
      chip_sim_log_run(msg);
    }
  }

FUNC_EXIT_POINT:
  return status ;
}


/*****************************************************
*NAME
*  chip_sim_indirect_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 18-Sep-02 16:47:14
*FUNCTION:
*  (1) check the chip if have been triggered to Indirect.
*  (2) If NO simulator error.
*  (3) read the needed address and values.
*  (4) put the needed value if place.
*  (5) set off the trigger bit.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*      Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_indirect_op(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK;
  UINT32
    indirect_access_triger[1] = {0},
    indirect_access_address = 0;
  CHIP_SIM_INDIRECT_BLOCK
     *block_p = NULL;
 UINT32
   dummy_val,
   nof_indirect_accesses,
   command_count,
   i;

  nof_indirect_accesses = 0;

  for (block_p=Indirect.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    /* 1 */
    *indirect_access_triger  = chip_sim_indirect_get_access_trig(chip_ndx, block_p) ;
    *indirect_access_triger  = CHIP_SIM_BYTE_SWAP(*indirect_access_triger);

    /* 2 */
    if ((*indirect_access_triger & 0x1) != 0x1)
    {
      /*
       * This module is was not triggered
       */
      continue;
    }


    /* 2 */
    indirect_access_address = chip_sim_indirect_get_access_address(chip_ndx, block_p) ;
    /*
     * Swap in WIN systems (little endian)
     */
    indirect_access_address = CHIP_SIM_BYTE_SWAP(indirect_access_address);

    if (chip_sim_mem_check_is_indirect_access((~SOC_SAND_RD_NOT_WR_MASK)&indirect_access_address))
    {
      /* not an Indirect place */
      chip_sim_log_run("chip_sim_indirect_op: not to Indirect place -- \r\n"
                       " chip_sim_mem_check_is_indirect_access() failed ..\r\n");
      status = ERROR ;
      GOTO_FUNC_EXIT_POINT ;
    }

    if ((SOC_SAND_RD_NOT_WR_MASK&indirect_access_address) == SOC_SAND_RD_NOT_WR_MASK )
    {
      /* read */
      indirect_access_address &= (~SOC_SAND_RD_NOT_WR_MASK) ;
      if (chip_sim_mem_check_is_read_access(indirect_access_address))
      {
        /* not a read place */
        chip_sim_log_run("chip_sim_indirect_op: ERROR ..\r\n");
        status = ERROR ;
        GOTO_FUNC_EXIT_POINT ;
      }

      if (chip_sim_indirect_address_to_offset(indirect_access_address, block_p, &dummy_val) == ERROR)
      {
        /*
         * We are in the right access triger - but not in the righr memory section.
         */
        continue;
      }
      else
      {
        nof_indirect_accesses ++;
      }

      if (chip_sim_indirect_read(chip_ndx, indirect_access_address, block_p))
      {
        chip_sim_log_run("chip_sim_indirect_op: chip_sim_indirect_read() failed ..\r\n");
        status = ERROR ;
        GOTO_FUNC_EXIT_POINT ;
      }
    }
    else
    {
      UINT32
        indirect_write_value[10] = {0} ;

      /* write */
      if (chip_sim_mem_check_is_write_access(indirect_access_address))
      {
        /* not a write place */
        chip_sim_log_run("chip_sim_indirect_op: chip_sim_mem_check_is_write_access() failed ..\r\n");
        status = ERROR ;
        GOTO_FUNC_EXIT_POINT ;
      }

      if (chip_sim_indirect_address_to_offset(indirect_access_address, block_p, &dummy_val) == ERROR)
      {
        /*
         * We are in the right access trigger - but not in the right memory section.
         */
        continue;
      }
      else
      {
        chip_sim_indirect_get_write_value(chip_ndx, block_p,indirect_write_value) ;
        nof_indirect_accesses++;
      }

      /*
       *  Support multiple writes (DD).
       */
       command_count = 0 ;
       soc_sand_bitstream_get_field(indirect_access_triger, 2, 15, &command_count) ;
       if (command_count == 0) {
          /* Write value once in this case */
          command_count++;
       }
       if (command_count > 1)
       {
         command_count--;
       }
       for( i = 0; i < command_count; i++ )
       {
         if (block_p->start_address + i > block_p->end_address )
         {
           break;
         }
         if (chip_sim_indirect_write(chip_ndx, indirect_access_address, indirect_write_value, block_p))
         {
           chip_sim_log_run("chip_sim_indirect_op: chip_sim_indirect_write() failed ..\r\n");
           status = ERROR ;
           GOTO_FUNC_EXIT_POINT ;
         }
         indirect_access_address++;
       }
    }
  }

FUNC_EXIT_POINT:
/* set the inidrect trigger off.*/
  chip_sim_indirect_set_access_trig(chip_ndx, 0x0);

  return status ;
}

