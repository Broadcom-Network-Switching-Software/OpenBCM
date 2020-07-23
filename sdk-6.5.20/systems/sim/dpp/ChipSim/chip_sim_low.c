/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include "chip_sim.h"
#include "chip_sim_low.h"
#include "chip_sim_cell.h"
#include "chip_sim_mem.h"
#include "chip_sim_indirect.h"
#include "chip_sim_pkt.h"
#include "chip_sim_log.h"
#include "chip_sim_task.h"

#include <soc/dpp/SAND/Management/sand_error_code.h>


/*****************************************************
*NAME
*  chip_sim_check_address
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 16:20:11
*FUNCTION:
*  1. Check for four byte align.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 address
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_check_address(
    SOC_SAND_IN UINT32 address
    )
{
  STATUS
    status = OK ;

  /* 1. */
  if ((address&0x3) != 0x0)
  {
    status = ERROR ;
    goto chip_sim_check_address_exit ;
  }


chip_sim_check_address_exit:
  return status ;
}


/*****************************************************
*NAME
*  chip_sim_triger
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 16:30:03
*FUNCTION:
*  (1) Check indirect triggering
*  (2) Check cell triggering
*  (3) Check packet triggering
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 chip_offset
*    (3) SOC_SAND_IN UINT32 value
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
    chip_sim_triger(
      SOC_SAND_IN int    chip_ndx,
      SOC_SAND_IN UINT32 chip_offset,
      SOC_SAND_IN UINT32 value
      )
{
  STATUS
    status;
  status = OK;
  /*
   */
  /* (1) */
  if (chip_sim_indirect_is_access_trig(chip_offset) &&
      ((value&0x1) == 1)
     )
  {
    if (chip_sim_indirect_get_delay()==0)
    {
      /* No wait */
      status = chip_sim_indirect_op(chip_ndx) ;
    }
    else
    {
      /*
       * Insert into the some future task the opperation of reset the trigger.
       */
      status = chip_sim_indirect_set_delay_op(chip_ndx);
    }
  }

  /* (2) */
  if (chip_sim_cell_is_trig_offset(chip_offset) &&
      ((value&0x1) == 1)
     )
  {
    if (chip_sim_cell_tx_get_delay()==0)
    {
      /*
       * No wait
       */
      status = chip_sim_cell_op(chip_ndx);
    }
    else
    {
      /*
       * Insert into the some future task the opperation of reset the trigger.
       */
      status = chip_sim_cell_set_delay_op(chip_ndx) ;
    }
  }

  /* (3) */
  if (chip_sim_mem_is_pkt_trig(chip_offset) &&
      ((value&0x1) == 1)
     )
  {
    if (chip_sim_mem_get_pkt_delay()==0)
    {
      /*
       * No wait
       */
      status = chip_sim_send_pkt_op(chip_ndx) ;
    }
    else
    {
      /*
       * Insert into the some future task the opperation of reset the trigger.
       */
      status = chip_sim_mem_set_pkt_delay_op(chip_ndx) ;
    }
  }
  return status ;
}
/*****************************************************
*NAME
*   chip_sim_write
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 16:22:58
*FUNCTION:
*  1. Convert to offset within the chip.
*  2. Check the address if valid.
*  3. Check if legitimate to write there.
*  4. Write the needed value.
*  5. Check if its need to trigger other operations,
*       e.g, cell send to driver or indirect.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 address
*    (2) SOC_SAND_IN UINT32 value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_write(
    SOC_SAND_IN UINT32 address,
    SOC_SAND_IN UINT32 value
    )
{
  STATUS
    status = OK ;
  int
    chip_ndx ;
  UINT32
    chip_offset ;

  /* 1 */
  if (chip_sim_mem_address_to_offset(address, &chip_offset, &chip_ndx))
  {
    status = ERROR ;
    goto chip_sim_write_exit ;
  }
  /* 2 */
  if (chip_sim_check_address(chip_offset))
  {
    status = ERROR ;
    goto chip_sim_write_exit ;
  }
  /* 3 */
  if (chip_sim_mem_check_is_write_access(chip_offset))
  {
    status = ERROR ;
    goto chip_sim_write_exit ;
  }
  /* 4 */
  if (chip_sim_mem_write(chip_ndx, chip_offset, value))
  {
    status = ERROR ;
    goto chip_sim_write_exit ;
  }

  /* 5 */
  if (chip_sim_triger(chip_ndx, chip_offset, CHIP_SIM_BYTE_SWAP(value)))
  {
    status = ERROR ;
    goto chip_sim_write_exit ;
  }

chip_sim_write_exit:
  return status ;
}

/*****************************************************
*NAME
*   chip_sim_read
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 16:32:11
*FUNCTION:
*  1. Convert to offset within the chip.
*  2. Check the address if valid.
*  3. Read the needed value.
*  4. Check if its need to clear,
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32  address
*    (2) SOC_SAND_OUT UINT32 *value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_read(
    SOC_SAND_IN  UINT32 address,
    SOC_SAND_OUT UINT32 *value
    )
{
  STATUS
    status = OK ;
  int
    chip_ndx ;
  UINT32
    chip_offset ;
  /* 1 */
  if (chip_sim_mem_address_to_offset(address, &chip_offset, &chip_ndx))
  {
    char msg[160];
    sal_sprintf(msg, "chip_sim_read(0x%X): failed to get valid chip offset\r\n", address);
    chip_sim_log_run(msg);

    status = ERROR ;
    goto chip_sim_read_exit ;
  }
  /* 2 */
  if (chip_sim_mem_check_is_read_access(chip_offset))
  {
    char msg[160];
    sal_sprintf(msg, "chip_sim_read(0x%X): chip_offset(0x%X) have no read access\r\n", address, chip_offset);
    chip_sim_log_run(msg);
    status = ERROR ;
    goto chip_sim_read_exit ;
  }
  /* 3 */
  if (chip_sim_mem_read(chip_ndx, chip_offset, value))
  {
    status = ERROR ;
    goto chip_sim_read_exit ;
  }
  /* 4 */
  if (OK == chip_sim_mem_check_is_clear_access(chip_offset))
  {
    if (chip_sim_mem_write(chip_ndx, chip_offset, 0x0))
    {
      status = ERROR ;
      goto chip_sim_read_exit ;
    }
  }

chip_sim_read_exit:
  return status ;
}

SOC_SAND_RET 
  soc_sand_sim_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  volatile uint32 
    *ul_ptr;
  const volatile uint32 
    *local_buff_ptr ;
  uint32 
    ui, 
    local_offset, 
    local_size;
  SOC_SAND_RET 
    ex;
  
  ex = SOC_SAND_OK ;

  /*
   * Notice that due to the endian issue this method
   * handles only memory that is comprized of longs
   */
  local_offset  = offset >> 2 ;
  local_size    = size   >> 2 ;
  ul_ptr        = base_address + local_offset ;

  /*
   * Use local pointer
   */
  local_buff_ptr = array;

  for (ui=0 ; ui<local_size ; ++ui)
  {
    if ( chip_sim_task_is_alive() )
    {
      if (chip_sim_write(PTR_TO_INT(ul_ptr), *local_buff_ptr) )
      {
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
  }
  goto exit ;
  /*
   */
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_PHYSICAL_WRITE_TO_CHIP,
        "error in soc_sand_physical_write_to_chip(): Cannot access chip",
         PTR_TO_INT(array), PTR_TO_INT(base_address),
         offset, size, 0,0
  ) ;
  return ex ;
}

SOC_SAND_RET 
  soc_sand_sim_read(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  const volatile uint32 
      *ul_ptr ;
  uint32 
    ui, 
    local_offset, 
    local_size;
  SOC_SAND_RET 
    ex;
  
  ex = SOC_SAND_OK ;

  /*
   * Notice that due to the endian issue this method
   * handles only memory that is comprized of longs
   */
  local_offset  = offset  >> 2 ;
  local_size    = size    >> 2 ;
  ul_ptr        = base_address + local_offset ;
  for (ui = 0 ; ui < local_size ; ui++)
  {
    if ( chip_sim_task_is_alive())
    {
      if (chip_sim_read(PTR_TO_INT(ul_ptr), (UINT32*)array) )
      {
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
      array++ ;
      ul_ptr++ ;
    }
    else
    {
      ex = SOC_SAND_ERR ;
      goto exit ;
    }
  }

  goto exit ;
  /*
   */
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_PHYSICAL_READ_FROM_CHIP,
        "error in soc_sand_physical_read_from_chip(): Cannot access chip", 
         PTR_TO_INT(array),
         PTR_TO_INT(base_address),
         offset,
         size,0,0) ;
  return ex ;
}

