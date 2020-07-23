/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        fec_allocation.h
 * Purpose:     fec allocation managament
 */


#include <appl/dpp/FecAllocation/fec_allocation.h>




/* ******structs*******  */

/* linked list strucs*/


/* linked list struct */
typedef struct linked_list_member_s {
   uint32 fec_id_table;
   uint32 ecmp_id;
   struct linked_list_member_s *next;
} linked_list_member_t;

typedef struct linked_list_s {
   linked_list_member_t *first_member;
   uint32 size;
} linked_list_t;




/*struct of the fec_id + ecmp number */
struct fec_id_ecmp_ {
   bcm_if_t fec_id;
   bcm_if_t ecmp_id;
   linked_list_t *fecs_list; /*in a case the struct is represents a real FEC*/
};

typedef struct fec_id_ecmp_ *fec_id_ecmp;


/*struct of alloc manager and fec's id*/

struct array_struct_ {
   int **array_;
   fec_id_ecmp ** fec_id_list;
   int *size; /*the size of the FECS table*/
   bcm_failover_t *fail_id_list;
};


typedef struct array_struct_ *array_struct;




/* ***global variables*** */


  /*alloc manager +fecs id's struct*/
static array_struct table = NULL;



/* ***implementation of alloc manager***
   This database will allow us know which index is occupied and in case the index is occupied by a FEC,
   will store the "real" FEC of the copy fec
   */



/*
 * Function:
 *      fec_allocation_create_list
 * Purpose:
 *      creates an array of size filled with -1
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      size       - (IN) The desired list size
 *      list_result- (OUT) The list result
 */
static int fec_allocation_create_list( int size, int **list_result) 
{
  BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

  BCMDNX_ALLOC(*list_result, size*sizeof(int), "Array for alloc manager.");

   /*initializing table with -1*/
   sal_memset(*list_result, -1, size*sizeof(int)); /*size of int*/


exit:
   BCMDNX_FUNC_RETURN;

   
}



/*
 * Function:
 *      fec_allocation_get_fec_father_table_id_number
 * Purpose:
 *      In case of a copy fec returns the real fec id
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      fec_table_copy_id   - (IN) - fec_table_copy_id: the copy id (table)
 */

static int fec_allocation_get_fec_father_table_id_number(int unit,int fec_table_copy_id) 
{
   return table->array_[unit][fec_table_copy_id];
}

/*
 * Function:
 *      fec_allocation_set_value
 * Purpose:
 *      Set the value in the array table at index
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      index   - (IN) - The desired index
 */


static void fec_allocation_set_value(int unit,int index, int value)
{
   table->array_[unit][index] = value;
}


static int fec_allocation_get_value(int unit,int index)
{
   return table->array_[unit][index];
}


static int fec_allocation_is_empty(int unit,int index)
{
   return table->array_[unit][index] == -1;
}




/*
 * "delete" the value in the table from index start to end
 */


/*
 * Function:
 *      fec_allocation_delete_value
 * Purpose:
 *      "deletes" the value in the table from index start to end
 */

static void fec_allocation_delete_value(int unit,int start, int end) {
   int i = start;
   for (; i <= end; i++) 
   {
      table->array_[unit][i] = -1;
   }

}



/*
 * Function:
 *      fec_allocation_alloc_chunk
 * Purpose:
 *    finds a chunk sized as size from top  down. index result will be even if found.
	returns -1 if no such place has been found 
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      size       - (IN) the size of the desired chunk
 */

static int fec_allocation_alloc_chunk(int unit,int size) { 
   int i;
   int size_found;

   size_found = 0;
   i = table->size[unit] - 1;
   for (; i > SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved ; i--)
   {
      if (table->array_[unit][i] == -1) 
      {
         size_found++;
         if (size_found == size) 
		 {
             if ( i%2==0 ) /*even index..*/
			 {
				 return i;
             }
			 else
			 {
				 size_found--; /*maybe the next cell will be free...*/
			 }
         }
      } 
      else 
      { /*the cut is not good*/
         size_found = 0;
      }

   }
   return -1;

}





/*    ****linked list implementation**** */


/* linked linked list*/
static void fec_allocation_linked_list_init(linked_list_t *linked_list) {
   linked_list->first_member = NULL;
   linked_list->size = 0;
}

static int fec_allocation_linked_list_member_add(int unit, linked_list_t *linked_list, uint32 fec_id_table, uint32 ecmp_id) {
   linked_list_member_t *linked_p = NULL;

   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_ALLOC(linked_p, sizeof(linked_list_member_t), "linked list member");
   linked_p->fec_id_table = fec_id_table;
   linked_p->ecmp_id = ecmp_id;
   linked_p->next = linked_list->first_member;
   linked_list->first_member = linked_p;
   linked_list->size += 1;
exit:
   BCMDNX_FUNC_RETURN;
}


 static int fec_allocation_linked_list_member_find(/*in*/int unit, linked_list_t *linked_list, uint32 fec_id_table,   uint8 delete_linked,/*out*/ uint32 *ecmp_id, uint8 *found) {
   linked_list_member_t *linked_p;
   linked_list_member_t *linked_prev_p;

   BCMDNX_INIT_FUNC_DEFS;
   linked_prev_p = linked_list->first_member;
   if (linked_prev_p == NULL) {
      *found = FALSE;
      BCMDNX_FUNC_RETURN;
   }
   if (linked_prev_p->fec_id_table == fec_id_table) {
      *found = TRUE;
      *ecmp_id = linked_prev_p->ecmp_id;
      if (delete_linked) {
         linked_list->first_member = linked_prev_p->next;
         BCM_FREE(linked_prev_p);
         linked_list->size -= 1;
      }
      BCMDNX_FUNC_RETURN;
   }
   linked_p = linked_prev_p->next;
   *found = FALSE;
   while (linked_p != NULL) {
      if (linked_p->fec_id_table == fec_id_table) {
         *ecmp_id = linked_prev_p->ecmp_id;
         *found = TRUE;
         break;
      }
      linked_prev_p = linked_p;
      linked_p = linked_p->next;
   }
   if (*found == TRUE) {
      if (delete_linked) {
         linked_prev_p->next = linked_p->next;
         BCM_FREE(linked_p);
         linked_list->size -= 1;
      }
   }

   BCMDNX_FUNC_RETURN;
}



static int fec_allocation_linked_list_destroy(int unit, linked_list_t *linked_list) {
   linked_list_member_t *linked_p;
   linked_list_member_t *linked_temp_p;

   BCMDNX_INIT_FUNC_DEFS;
   linked_p = linked_list->first_member;
   while (linked_p != NULL) {
      linked_temp_p = linked_p;
      linked_p = linked_p->next;
      BCM_FREE(linked_temp_p);
   }
   linked_list->first_member = 0;
   linked_list->size = 0;

   BCMDNX_FUNC_RETURN;
}






static int fec_allocation_linked_list_member_find_fec_from_ecmp(/*in*/ int unit, linked_list_t *linked_list,  uint32 ecmp_id,  uint8 delete_linked,/*out*/ uint32 *fec_id_table, uint8 *found) {
   linked_list_member_t *linked_p;
   linked_list_member_t *linked_prev_p;

   BCMDNX_INIT_FUNC_DEFS;
   linked_prev_p = linked_list->first_member;
   if (linked_prev_p == NULL) {
      *found = FALSE;
      BCMDNX_FUNC_RETURN;
   }
   if (linked_prev_p->ecmp_id == ecmp_id) {
      *found = TRUE;
      *fec_id_table = linked_prev_p->fec_id_table;
      if (delete_linked) {
         linked_list->first_member = linked_prev_p->next;
         BCM_FREE(linked_prev_p);
         linked_list->size -= 1;
      }
      BCMDNX_FUNC_RETURN;
   }
   linked_p = linked_prev_p->next;
   *found = FALSE;
   while (linked_p != NULL) {
      if (linked_prev_p->ecmp_id == ecmp_id) {
          *fec_id_table = linked_prev_p->fec_id_table;
         *found = TRUE;
         break;
      }
      linked_prev_p = linked_p;
      linked_p = linked_p->next;
   }
   if (*found == TRUE) {
      if (delete_linked) {
         linked_prev_p->next = linked_p->next;
         BCM_FREE(linked_p);
         linked_list->size -= 1;
      }
   }

   BCMDNX_FUNC_RETURN;
}






/*
 * Function:
 *      fec_allocation_create_fec_id_ecmp_struct
 * Purpose:
 *    creates a fec_id_ecmp struct and fill it with fec_id and ecmp_id
 */


static int fec_allocation_create_fec_id_ecmp_struct(fec_id_ecmp *fec_id_ecmp_struct, bcm_if_t fec_id, bcm_if_t ecmp_id) 
{ 
   BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
   (*fec_id_ecmp_struct) = NULL;
   BCMDNX_ALLOC(*fec_id_ecmp_struct, 1 * sizeof(struct fec_id_ecmp_), "Struct fec_id_ecmp");
   (*fec_id_ecmp_struct)->fec_id = fec_id;
   (*fec_id_ecmp_struct)->ecmp_id = ecmp_id;
   

exit:
   BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      fec_allocation_convert_fec_id_to_fec_table_id
 * Purpose:
 *      returns the fec id as shown in fec table id
 *
 * Parameters:
 *      fec_id     - (IN) The fec id to convert
 */
static int fec_allocation_convert_fec_id_to_fec_table_id(int fec_id) 
{
   return BCM_L3_ITF_VAL_GET(fec_id);
}


/*
 * Function:
 *      fec_allocation_convert_fec_table_id_to_fec_id
 * Purpose:
 *      returns the the real fec_id as of an id as shown in the alloc manager table
 *
 * Parameters:
 *      fec_id     - (IN) The fec table id to convert
 */
static bcm_if_t fec_allocation_convert_fec_table_id_to_fec_id(int fec_table_id) 
{
   bcm_if_t fec_id;
   BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, fec_table_id);
   return fec_id;

}


/*
 * Function:
 *      fec_allocation_ecmp_is_real
 * Purpose:
 *      returns True if ecmp consists of real fecs. otherwise - returns False
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      ecmp       - (IN) The ecmp egr object
 *      is_real    - (OUT) The function result
 */
static int fec_allocation_ecmp_is_real(/*in*/int unit, bcm_l3_egress_ecmp_t *ecmp,/*out*/ uint8 *is_real) 
{
   int rc;
   bcm_if_t *intf_array;
   int intf_size;
   int intf_count;

   BCMDNX_INIT_FUNC_DEFS;

    /* getting the ecmp data*/
   intf_size = ecmp->max_paths;
   intf_array = NULL;
   BCMDNX_ALLOC( intf_array,intf_size*sizeof( bcm_if_t), "An array for FECS.");
   rc = bcm_l3_egress_ecmp_get(unit, ecmp, intf_size, intf_array, &intf_count);

   if (rc != BCM_E_NONE) 
   {    
      BCM_FREE(intf_array);
	  BCMDNX_IF_ERR_EXIT(rc); 
   }

   /* since we maintain our ecmp in a way that all the FECS are real or al the FECS are copies we can check it in that way:*/
   *is_real = fec_allocation_convert_fec_id_to_fec_table_id(intf_array[0]) ==
	   fec_allocation_get_fec_father_table_id_number(unit,fec_allocation_convert_fec_id_to_fec_table_id(intf_array[0]));
   BCM_FREE(intf_array);

   exit:
   BCMDNX_FUNC_RETURN;

}


/**/



/*
 * Function:
 *      fec_allocation_fec_is_superfec
 * Purpose:
 *      returns TRUE if the fec is a superfec. Otherwise returns FALSE. *is_superfec will store the result
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      fec_id     - (IN) The fec id
 *      is_superfec- (OUT) The function result
 */
static int fec_allocation_fec_is_superfec(/*in*/int unit, bcm_if_t fec_id,/*out*/ uint8 *is_superfec)
{
   int rc;
   int fec_table_id;
   bcm_l3_egress_t copy_egr;

    BCMDNX_INIT_FUNC_DEFS;


   /*getting the FEC information*/
   rc = bcm_l3_egress_get(unit, fec_id, &copy_egr);
   if (rc != BCM_E_NONE) 
   {
	   fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(fec_id);
	   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Couldn't get the fec fec id %d   fec_table_id = %d      details. \n"),fec_id, fec_table_id));
       BCMDNX_IF_ERR_EXIT(rc);   
   }

   /* if the egr objects is not pointing to a failoverid - he is not a super fec. can a failover id points to 0??*/
   if (copy_egr.failover_id != 0)
   {
	  *is_superfec = TRUE;
      return rc;
   } 

   *is_superfec= FALSE;

   exit:
   BCMDNX_FUNC_RETURN;


}


/*
 * Function:
 *      fec_allocation_fec_list_contains_superfec
 * Purpose:
 *      returns TRUE if found a super fec in the intf_array, otherwise, returns FALSE
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      intf_array - (IN) The fec id list
 *      size       - (IN) The size of the intf_array
 */
static int fec_allocation_fec_list_contains_superfec(int unit, bcm_if_t *intf_array, int size)
{
   int i;
   uint8 superfec_flag;
 

   for (i = 0; i < size; i++) 
   {
	  fec_allocation_fec_is_superfec(unit, intf_array[i],&superfec_flag);
      if (superfec_flag) 
      {
         return TRUE;
      }
   }
   /* no superfec found*/
   return FALSE;
}



/*
 * Function:
 *      fec_allocation_ecmp_is_superfec
 * Purpose:
 *      returns 1 iff the fec is consists of only super FECs. Otherwise - returns 0
 *
 * Parameters:
 *      unit             - (IN) Device Number
 *      ecmp             - (IN) The ecmp egr object
 *      ecmp_is_superfec - (OUT) The function result
 */
static int fec_allocation_ecmp_is_superfec(int unit,  bcm_l3_egress_ecmp_t *ecmp,uint8 *ecmp_is_superfec){
   int rc;
   bcm_if_t *intf_array;
   int intf_size;
   int intf_count;

   BCMDNX_INIT_FUNC_DEFS;

   BCMDNX_NULL_CHECK(ecmp);

   intf_size = ecmp->max_paths;
   intf_array = NULL;
   BCMDNX_ALLOC(intf_array, intf_size * sizeof( bcm_if_t), "Array for FECS.");


   /* getting the ecmp data*/
   rc = bcm_l3_egress_ecmp_get(unit, ecmp, intf_size,intf_array, &intf_count);
   if (rc != BCM_E_NONE)
   {
	   BCMDNX_IF_ERR_EXIT(rc); 
       BCM_FREE(intf_array);
  
   }

   /* all FECS are either superfecs or regular*/
   fec_allocation_fec_is_superfec( unit, intf_array[0],ecmp_is_superfec);
   BCM_FREE(intf_array);

exit:
   BCMDNX_FUNC_RETURN;
}

static uint8 fec_allocation_fec_is_real(int unit, bcm_if_t fec_id)
{
   return fec_allocation_get_value(unit,fec_allocation_convert_fec_id_to_fec_table_id(fec_id)) == fec_allocation_convert_fec_id_to_fec_table_id(fec_id);

}

/*
 * Function:
 *      fec_allocation_associate_fec
 * Purpose:
 *      Update the database in a case that the user created a FEC with the "regular" bcm API.
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      fec_id     - (IN) The fec id of the created fec.
 */
int fec_allocation_associate_fec(int unit, bcm_if_t fec_id)
{
	int fec_id_in_table;
	linked_list_t *fecs_list;
   BCMDNX_INIT_FUNC_DEFS;

	fec_id_in_table= fec_allocation_convert_fec_id_to_fec_table_id(fec_id);


	  /* data structure update */
	  /*updating the alloc_manager*/
	  fec_allocation_set_value(unit,fec_id_in_table, fec_id_in_table); /* A real fec id will be pointed by himself.*/

	  /*updating the fec_id's list*/
	  fec_allocation_create_fec_id_ecmp_struct(&(table->fec_id_list[unit][fec_id_in_table]),fec_id_in_table, DOES_NOT_BELONG_TO_ECMP);
	  /*initializing fecs_list*/

	  fecs_list = NULL;
	  BCMDNX_ALLOC(fecs_list, sizeof(  linked_list_t ), "Linked list");
	  sal_memset(fecs_list,0, sizeof(  linked_list_t ));

	  fec_allocation_linked_list_init(fecs_list);
	  table->fec_id_list[unit][fec_id_in_table]->fecs_list = fecs_list;
exit:
   BCMDNX_FUNC_RETURN;


}



static int fec_allocation_get_fec_father_id_number(int unit,int fec_copy_id)
{
   return fec_allocation_convert_fec_table_id_to_fec_id( fec_allocation_get_fec_father_table_id_number(unit, fec_allocation_convert_fec_id_to_fec_table_id(fec_copy_id) ));
}



 
/* 
This function copies a FEC 
Params: 
-  desired_fec_copy_id - the new FEC id 
-  original_fec_id - the original FEC id
- failover_if_id - the failover interface id of the new FEC. If equals to -1 : will not change the value of the real fec
-  failover_id - the failover id of the new FEC. If equals to -1 : will not change the value of the real fec 
 
Updates the alloc manager table. 
*/

static int fec_allocation_copy_fec(int unit, bcm_if_t desired_fec_copy_id, bcm_if_t original_fec_id, bcm_if_t failover_if_id, bcm_if_t failover_id) {
   int rc;
   int fec_copy_table_id;
   int original_fec_table_id;
   bcm_l3_egress_t copy_egr;
   bcm_l3_egress_t bug;
   uint32 flags;
   uint8 is_superfec;

   BCMDNX_INIT_FUNC_DEFS;


   /*getting the FEC information*/
   rc = bcm_l3_egress_get(unit, original_fec_id, &copy_egr);
   BCMDNX_IF_ERR_EXIT(rc); 


   fec_copy_table_id = fec_allocation_convert_fec_id_to_fec_table_id(desired_fec_copy_id);
   original_fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(original_fec_id);


   /*creating the new FEC*/
   flags = copy_egr.flags;
   flags = BCM_L3_WITH_ID | flags ; /*we want to determine the id*/

   /*reseting the encap_id so we can craete another FEC*/
   copy_egr.encap_id = 0;


   if (failover_if_id != -1) 
   {
       copy_egr.failover_if_id = failover_if_id;
   }

   if (failover_id != -1) 
   {
      copy_egr.failover_id = failover_id;
   }

   fec_allocation_fec_is_superfec(unit,original_fec_id, &is_superfec);
   if ( !is_superfec && !(failover_id == -1 && failover_if_id == -1)) /* We are in a case of copying a fec of superfec. Thus, we cann't get the information from the father + the failover id is fake..and the father FEC doesn't have it..*/
   {
	   copy_egr.failover_id = table->fail_id_list[unit];

   }
   else if(failover_id == -2)     /* until bug is fixed*/
   {
	   rc = bcm_l3_egress_get(unit,original_fec_id-1,&bug);
	   BCMDNX_IF_ERR_EXIT(rc);
	   copy_egr.failover_id = bug.failover_id;
   }

   

   rc = bcm_l3_egress_create(unit, flags, &copy_egr, &desired_fec_copy_id);
   if (rc != BCM_E_NONE) {
     LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Error. Couldn't create the FEC\n")));
      return rc;
   }

   /*data structers update */
   /* fec-id's list will be updated by the function calling this function*/

   /*updating the alloc manager*/
   /*we always copy a real fec*/
   fec_allocation_set_value(unit,fec_copy_table_id, original_fec_table_id); /*the "father" FEC is the old one.*/

exit:
   BCMDNX_FUNC_RETURN;

}



/*
 * Function:
 *      fec_allocation_delete_old_copy
 * Purpose:
 * This function deletes the fec pointed by fec_id.
 * if fec_id is a super fec - delete both fecs.
 * It assumes the fec id points to a copy fec and not a real one 
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      fec_id     - (IN) The fec_id
 */
static int fec_allocation_delete_old_copy(int unit, bcm_if_t fec_id) 
{
   int rc;
   int fec_table_id;
   int fec_father_table_id;
   uint8 fec_is_superfec_flag;
   uint32 ecmp_id;  
   uint8 found;
  

   BCMDNX_INIT_FUNC_DEFS;

   fec_allocation_fec_is_superfec(unit,fec_id,&fec_is_superfec_flag);
   fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(fec_id);

   rc = bcm_l3_egress_destroy(unit, fec_id);
   BCMDNX_IF_ERR_EXIT(rc); 
   if (fec_is_superfec_flag) 
   {
      rc = bcm_l3_egress_destroy(unit, fec_id + 1);
      BCMDNX_IF_ERR_EXIT(rc); 
   }

   /* data structure update */

   /*updating the alloc manager table*/
   fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, fec_table_id);
   fec_allocation_delete_value(unit, fec_table_id, fec_table_id);

   /* updating the fec's id list */
   /* deleting the value */

   rc =  fec_allocation_linked_list_member_find(unit,table->fec_id_list[unit][fec_father_table_id]->fecs_list, fec_table_id,   1, &ecmp_id, &found);


   BCMDNX_IF_ERR_EXIT(rc); 

   if (fec_is_superfec_flag)
   { 

      /* updating the fec's id list */
      /* deleting the value */
      fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, fec_table_id + 1);
	  fec_allocation_delete_value(unit,fec_table_id + 1, fec_table_id + 1);
      rc =   fec_allocation_linked_list_member_find(unit,  table->fec_id_list[unit][fec_father_table_id]->fecs_list, fec_table_id + 1,   1,&ecmp_id, &found);
      BCMDNX_IF_ERR_EXIT(rc); 


   }

exit:
   BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      fec_allocation_check_fec_list_validity
 * Purpose:
 * returns TRUE iff the fec list consists of only regualr fecs or only superfecs, and the fecs re consecutive. otherwise ' returns FALSE
 *
 * Parameters:
 *      unit       - (IN) Device Number
 *      intf       - (IN) The fec_id list
 *      size       - (IN) The size of intf
 */
int static fec_allocation_check_fec_list_validity(int unit, bcm_if_t *intf, int size)
{
	int i;
	uint8 fec_is_superfec_flag;
	fec_allocation_fec_is_superfec(unit,intf[0],&fec_is_superfec_flag);

    if (size ==0)
	{
		return TRUE;
    }
    if (fec_is_superfec_flag)
	{
		/* checking that all other fecs are super fecs*/
        for (i = 1 ; i<size; i++)
		{
			fec_allocation_fec_is_superfec(unit, intf[i],&fec_is_superfec_flag );
            if (!fec_is_superfec_flag )
			{
				return FALSE;
            }
        }

		/*checking that all FECS are consecutive*/
       for (i = 0 ; i<size - 1; i++)
		{
            if (intf[i] != intf[i+1]-2)
			{
				return FALSE;
            }
        }

	   return TRUE;

    }
	else /* FECS suppose to be regular*/
	{		/* checking that all other fecs are regular fecs*/
        for (i = 1 ; i<size; i++)
		{
           	fec_allocation_fec_is_superfec(unit, intf[i],&fec_is_superfec_flag );
            if (fec_is_superfec_flag )
			{
				return FALSE;
			}
        }

		/*checking that all FECS are consecutive*/
       for (i = 0 ; i<size - 1; i++)
		{
            if (intf[i] != intf[i+1]-1)
			{
				return FALSE;
            }
        }

	   return TRUE;

	}

	return FALSE;


}


/*
 * Function:
 *      fec_allocation_get_real_fec_of_ecmp
 * Purpose:
 * Returns the real FEC id of the given fec_id that belongs to the ECMP 
  function_result will store the result
 *
 * Parameters:
 *      unit             - (IN) Device Number
 *      ecmp             - (IN) The ecmp egr object
 *      fec_id           - (IN) The given fec_id
 *      real_fec_of_ecmp - (OUT) The real fec id that belongs to the ECMP
 */
static int fec_allocation_get_real_fec_of_ecmp(/*in*/int unit,bcm_l3_egress_ecmp_t *ecmp, bcm_if_t fec_id, /*out*/int *real_fec_of_ecmp)
{
    int rc;
    bcm_if_t *intf_array;
    int intf_size;
    int intf_count;
    int ecmp_real_table_id;
    int fec_table_id;
    uint32 result;        
    uint8 found;

	BCMDNX_INIT_FUNC_DEFS;



    /* getting the ecmp data*/
    intf_size = ecmp->max_paths;
	intf_array = NULL;
	BCMDNX_ALLOC(intf_array, intf_size*sizeof( bcm_if_t), "Array for FECS");
    rc = bcm_l3_egress_ecmp_get(unit, ecmp, intf_size, intf_array, &intf_count);
    if (rc != BCM_E_NONE)
	{
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "  Couldn't get the ECMP details. \n")));
       BCM_FREE(intf_array);
       return rc;
    }

    fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(fec_id);
    ecmp_real_table_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);

    rc = fec_allocation_linked_list_member_find_fec_from_ecmp(unit, table->fec_id_list[unit][ fec_table_id]->fecs_list, ecmp_real_table_id ,0, &result, &found);
	if (found == FALSE) /*the FEC is a real FEC*/
	{
	   result = fec_id;
	}

   if (rc != BCM_E_NONE) 
   {
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, " Error. Couldn't delete the FEC_item from the list\n")));
	  BCM_FREE(intf_array);
	  return rc;
   }

   *real_fec_of_ecmp = fec_allocation_convert_fec_table_id_to_fec_id(result);

   

exit:
   BCMDNX_FUNC_RETURN;

}




/*utilitis*/


/*
 * Function:
 *      fec_allocation_create_database_file
 * Purpose:
 *      Creating a file which describes the database - to be used when having errors.
 *
 * Parameters:
 *      unit       - (IN) Device Number
 */

int fec_allocation_create_database_file(int unit)
{	
    #ifndef NO_FILEIO
	FILE *fp;
	int rc;
	int i;
	char buffer[100];
	linked_list_member_t *node;
	int fec_id;
   BCMDNX_INIT_FUNC_DEFS;

   	/* writing the alloc manager table*/
	fp = sal_fopen(DATABASE_FILE_PATH_ALLOC_MANAGER ,"w");
    if (fp == NULL)
	{
		rc = BCM_E_FAIL;
		BCMDNX_IF_ERR_EXIT(rc); 
    }
    for (i =0; i< table->size[unit];i++)
	{
        if (table->array_[unit][i] == -1) 
		{
			continue;
        }
		 snprintf(buffer,50,"row number: %d value %d\r\n",i , table->array_[unit][i]);
 
         sal_fprintf(fp, buffer);
	    sal_fflush(fp);

    }
	sal_fclose(fp);
	/* writing the fec_id_list*/
	fp = sal_fopen(DATABASE_FILE_PATH_FECS_LIST ,"w");
    if (fp == NULL)
	{
		rc = BCM_E_FAIL;
		BCMDNX_IF_ERR_EXIT(rc); 
    }
    for (i = SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved +1 ; i< table->size[unit];i++)
	{
		fec_id = fec_allocation_convert_fec_table_id_to_fec_id(i);
        if (fec_allocation_get_value(unit, i)!= -1 && fec_allocation_fec_is_real( unit,	fec_id  ))  /* In case the FEC exists*/
		{		
			 snprintf(buffer,100,"REAL fec id = %d, REAL ecmp id =  %d   SONS = : ",table->fec_id_list[unit][i]->fec_id,table->fec_id_list[unit][i]->ecmp_id);
                     sal_fprintf(fp, buffer);
                    sal_fflush(fp);

			node = table->fec_id_list[unit][i]->fecs_list->first_member;
			while (node != NULL ) 
			{
                snprintf(buffer,100,"SON ->>> fec id = %d, REAL ecmp id =  %d  ",node->fec_id_table,node->ecmp_id);
                  sal_fprintf(fp, buffer);
                  sal_fflush(fp);
				node = node->next;

			}
                     sal_fprintf(fp, "\r\n");
                    sal_fflush(fp);
		}
    }

	sal_fclose(fp);

exit:
   BCMDNX_FUNC_RETURN;



#else
    return BCM_E_NONE;


#endif /* NO_FILEIO */

}


/* ****FEC allocation**** */



/*
 * Function:
 *      fec_allocation_initialize
 * Purpose:
 *     Allocates the DATABASES
 *     should be ran first (per unit)
 *
 * Parameters:
 *      unit       - (IN) Device Number
 */
int fec_allocation_initialize(int unit) 
{

   int fecs_table_size;
   bcm_failover_t fail_id;
   int rc;


   BCMDNX_INIT_FUNC_DEFS;


   if (table == NULL) /*first use of this function - we initialize all data structures and we don't fill them*/
   {
	  BCMDNX_ALLOC(table, sizeof(struct array_struct_ ) , "array_struct ");
	  sal_memset(table,0, sizeof(struct array_struct_ ));

	     /*creating the alloc manager array*/
	  BCMDNX_ALLOC(table->array_,( SOC_SAND_MAX_DEVICE+1)*sizeof(int*), "Table array.");
	  sal_memset(table->array_,0, (SOC_SAND_MAX_DEVICE+1)*sizeof(int*));
	  /*creating the Fec's id strucs*/
	  BCMDNX_ALLOC(table->fec_id_list, (SOC_SAND_MAX_DEVICE+1)*sizeof(  fec_id_ecmp *), "Fec's id list");
	  sal_memset(table->fec_id_list,0, (SOC_SAND_MAX_DEVICE+1)*sizeof(  fec_id_ecmp *));
	  BCMDNX_ALLOC(table->size, (SOC_SAND_MAX_DEVICE+1)*sizeof(int), "Sizes list");
	  sal_memset(table->fec_id_list,0, (SOC_SAND_MAX_DEVICE+1)*sizeof(  fec_id_ecmp *));
	  /*initializing the fail_id array*/
	  BCMDNX_ALLOC(table->fail_id_list, (SOC_SAND_MAX_DEVICE+1)*sizeof(  bcm_failover_t ), "Fail_id_list");
	  sal_memset(table->fec_id_list,0, (SOC_SAND_MAX_DEVICE+1)*sizeof( bcm_failover_t ));

   }


   /* initializing data structure of the specific unit*/
      fecs_table_size = SOC_DPP_CONFIG(unit)->l3.fec_db_size;
	  fec_allocation_create_list(fecs_table_size, &table->array_[unit]);
	  BCMDNX_ALLOC(table->fec_id_list[unit],fecs_table_size * sizeof(struct array_struct_), "Fec's list");
	  sal_memset(table->fec_id_list[unit],0,fecs_table_size * sizeof(struct array_struct_));
	  table->size[unit] = fecs_table_size;

	  rc = bcm_failover_create(unit, BCM_FAILOVER_FEC, &fail_id);
	  if (rc != BCM_E_NONE)
	  {
		   BCMDNX_IF_ERR_EXIT(rc); 
	  }
	  table->fail_id_list[unit] = fail_id;
  
   
exit:
   BCMDNX_FUNC_RETURN;

}





/*
 * Function:
 *      fec_allocation_deinitialize_last
 * Purpose:
 *      Deinitilize the global "table" variable.
 *      This functioon should be ran last
 * 
 *      Parameters:
 *      unit       - (IN) Device Number
 */
void fec_allocation_deinitialize_last(void) 
{
	  BCM_FREE(table->size);
	  BCM_FREE(table->fail_id_list);
	  BCM_FREE(table->array_);
	  BCM_FREE(table->fec_id_list);
	  BCM_FREE(table);
}



/*This functioon should be ran last (per unit)*/


/*
 * Function:
 *      fec_allocation_deinitialize_last
 * Purpose:
 *      Deinitilize the database of the unit
 *      This functioon should be ran last (per unit)
 */
void fec_allocation_deinitialize_unit(int unit) 
{
   int i;
   int fec_id;

/* deinitializing data structure of the specific unit*/

   /* travarsing over all fec_id's*/
   for (i = SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved +1 ; i< table->size[unit];i++)
   {
	   fec_id = fec_allocation_convert_fec_table_id_to_fec_id(i);
       if (fec_allocation_get_value(unit, i)!= -1 && fec_allocation_fec_is_real( unit,	fec_id  )) /* In case the FEC exists*/
	   {
		     fec_allocation_linked_list_destroy(unit, table->fec_id_list[unit][i]->fecs_list);
			 BCM_FREE( table->fec_id_list[unit][i]);
	   }
	

   }

   

   bcm_failover_destroy(unit,table->fail_id_list[unit]);
   BCM_FREE(table->array_[unit]);
   BCM_FREE(table->fec_id_list[unit]);
   BCM_FREE(table->fec_id_list);  

}

/*returns TRUE if the fec_id exists in the intf array, otherwise, returns ZERO*/
int fec_allocation_fec_exists_array(bcm_if_t *intf,int size, bcm_if_t fec_id)
{
	int i;
    for (i =0; i< size; i++)
	{
        if (intf[i] == fec_id)
		{	
			return TRUE;
        }
	}
	return FALSE;
    
}







/*user API*/



/* See the bcm_l3_egress_create documentation for more infromation. This function creates a FEC and updates the database accordingly.*/
int fec_allocation_bcm_l3_egress_create(int unit, uint32 flags, bcm_l3_egress_t *egr, bcm_if_t *intf)
{
   int rc;
   int fec_id_in_table;
   linked_list_t *fecs_list; /*will store the original FEC children*/
   int current_fec_table_id;
   int current_fec_id;
   linked_list_member_t *linked_prev_p;
   bcm_if_t *intf_array;
   int intf_size;
   uint8 ecmp_consists_of_superfecs;
   bcm_l3_egress_ecmp_t ecmp;
   int fec_father_id;
   uint8 found;
   uint32 current_ecmp_id;
   int i;
   int new_chunk_index_start;
   int *fecs_for_ecmp;
   int ecmp_real_id;
   int fec_father_table_id;
   int intf_count;
   bcm_if_t desired_fec_copy_id;
   bcm_if_t original_fec_id; 
   bcm_if_t failover_if_id;


   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(egr);
   BCMDNX_NULL_CHECK(intf);


  
   /* first edge case: The user is editing the fec - we need to edit all copies of the fec*/
   if ((( flags & (BCM_L3_WITH_ID |  BCM_L3_REPLACE)) == (BCM_L3_WITH_ID |  BCM_L3_REPLACE))  )
   {
        fec_id_in_table = fec_allocation_convert_fec_id_to_fec_table_id(*intf);
        if ( table->fec_id_list[unit][fec_id_in_table] !=NULL )
        {  
               /*checking that the fec is a real fec*/
              if (!fec_allocation_fec_is_real(unit,*intf))
        	  {
        		 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  The FEC id is not associated with a real FEC id."))); 
        		 rc =  BCM_E_PARAM;
        		 BCMDNX_IF_ERR_EXIT(rc); 
          
        	  }

              /* we iterate over all the copies of the fec and edit them*/
              linked_prev_p = table->fec_id_list[unit][fec_id_in_table]->fecs_list->first_member;
              while (linked_prev_p != NULL) 
              {
            
                 current_fec_table_id =linked_prev_p->fec_id_table;
                 current_fec_id = fec_allocation_convert_fec_table_id_to_fec_id(current_fec_table_id);
                 rc =  bcm_l3_egress_create(unit, flags, egr, &current_fec_id); 
                  BCMDNX_IF_ERR_EXIT(rc); 

             
                 linked_prev_p= linked_prev_p->next;
              }

              /* we edit the real fec*/
                rc =  bcm_l3_egress_create(unit, flags, egr, intf); 
                BCMDNX_IF_ERR_EXIT(rc); 

              return BCM_E_NONE;
        }
   }

/*Second edge case: Tthe user is trying to create a fec with id of an occupied fec*/
   if ( ((flags & (BCM_L3_WITH_ID | BCM_L3_REPLACE )) == (BCM_L3_WITH_ID)))
   {
	  fec_id_in_table = fec_allocation_convert_fec_id_to_fec_table_id(*intf);
	  if (  !fec_allocation_fec_is_real(unit, *intf) && !fec_allocation_is_empty(unit, fec_id_in_table ) ) /* the fec is a copy fec*/
	  {
		 /* we copy the ecmp that the fecs belongs to and delete the FEC so the user can create it*/
	  
		 /*first we try to get the ecmp array of fecs and the ECMP details*/
          fec_id_in_table = fec_allocation_convert_fec_id_to_fec_table_id(*intf);
	      fec_father_id = fec_allocation_get_fec_father_table_id_number(unit,fec_id_in_table);
		  fec_allocation_linked_list_member_find(unit,table->fec_id_list[unit][fec_father_id]->fecs_list,fec_id_in_table,0,&current_ecmp_id,&found);
		  bcm_l3_egress_ecmp_t_init(&ecmp);
		  ecmp.ecmp_intf = fec_allocation_convert_fec_id_to_fec_table_id(current_ecmp_id);
		  ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp.ecmp_intf);
		  ecmp.max_paths = fec_allocation_get_value(unit,ecmp_real_id );
		  ecmp.flags = BCM_L3_WITH_ID;
		  intf_size = ecmp.max_paths;
		  intf_array = NULL;
		   BCMDNX_ALLOC( intf_array, intf_size*sizeof(bcm_if_t), "Array for FECS");

		   rc = bcm_l3_egress_ecmp_get(unit, &ecmp, intf_size, intf_array, &intf_count);
		   if (rc != BCM_E_NONE)
		   {
			 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Couldn't get the ECMP details. \n")));
			 BCM_FREE(intf_array);
			 BCMDNX_IF_ERR_EXIT(rc);
		   }
	  
	 
		   fec_allocation_ecmp_is_superfec(unit, &ecmp, &ecmp_consists_of_superfecs);
	  
	  
		   if (ecmp_consists_of_superfecs)
		   {
				 new_chunk_index_start =  fec_allocation_alloc_chunk(unit, (intf_count)*2 );
				 if (new_chunk_index_start == -1)  /* no place found for copying the fecs */
				 {
					LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  FEC table is FULL\n")));
					BCM_FREE(intf_array);
					rc = BCM_E_FULL;
					BCMDNX_IF_ERR_EXIT(rc); 
				 }
		 
		 
				 fecs_for_ecmp = NULL;
				 BCMDNX_ALLOC( fecs_for_ecmp, (intf_count)*sizeof(int), "Array for Fec's.");
		 
				 /* copying all fecs*/
				 for (i=0; i < intf_count; i++)
				 {
					   fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + 2 * i);
					   desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 1 + 2 * i);
					   original_fec_id = fec_allocation_get_fec_father_id_number(unit, intf_array[i]+1);
					   failover_if_id = 0;
					   fec_allocation_copy_fec(unit,desired_fec_copy_id,original_fec_id , failover_if_id, -2); /* we copy the failover regulary.*/
					   /* now we copy the base FEC so he will point the the created FEC from above*/
					   desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 2 * i);
					   original_fec_id = fec_allocation_get_fec_father_id_number( unit, intf_array[i]);
					     failover_if_id= fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 1 + 2 * i);
					   fec_allocation_copy_fec(unit,desired_fec_copy_id,original_fec_id , failover_if_id, -1);
				 }
		 
				 /*creating the ECMP*/
				 rc = bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count , fecs_for_ecmp);
				 BCM_FREE(fecs_for_ecmp);
				 if (rc != BCM_E_NONE)
				 {
					LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:   Couldn't create the ECMP \n")));
					 BCM_FREE(intf_array);
					return rc;
				 }
				 ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp.ecmp_intf);
		 
				 /*updating data structure*/
				 /*updating the fec's id list */
				 for (i = 0; i < intf_count * 2 ; i++)
				 {
					fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + i));
					rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list,new_chunk_index_start + i,  ecmp_real_id);
					if (rc != BCM_E_NONE)
					{
					   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
					   BCM_FREE(intf_array);
					   return rc;
					}
				 }
		 
				 /* deleting old copies fec */
				 for (i = 0; i < intf_count; i++ )
				 {
					fec_allocation_delete_old_copy(unit, *(intf_array + i)); /* the deletion function deletes the superfec as well*/
				 }
				
		   }


		  else /*ecmp consists of regular copies*/
		  {
				 new_chunk_index_start =  fec_allocation_alloc_chunk(unit, intf_count );
				  if (new_chunk_index_start == -1) {   /* no place found for copying the fecs */
					 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  FEC table is FULL\n")));
					 BCM_FREE(intf_array);
					 return BCM_E_FULL; 
				  }	   
				 fecs_for_ecmp = NULL;
				 BCMDNX_ALLOC( fecs_for_ecmp, (intf_count)*sizeof(int), "Array for Fec's.");
				  /* copying all fecs*/
				  for (i = 0; i < intf_count; i++)
				  {
					 fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + i);
					 rc =  fec_allocation_copy_fec(unit, fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + i), 
												   fec_allocation_get_fec_father_id_number(unit,intf_array[i]), -1, -1);
					 if (rc != BCM_E_NONE)
					 {
						LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't create the shadow FEC\n")));
						BCM_FREE(intf_array);
						BCM_FREE(fecs_for_ecmp);
						return rc;
					 }
				  }

		
				  /*creating the ECMP*/
				  rc = bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count , fecs_for_ecmp);
				  BCM_FREE(fecs_for_ecmp);
				  if (rc != BCM_E_NONE) {
					 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Couldn't create the ECMP \n")));
					 BCM_FREE(intf_array);
					 
					 return rc;
				  }

				  ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp.ecmp_intf);

				  /*updating data structure*/



				  /* updating the fec's id list */
				  for (i = 0; i < intf_count ; i++) 
				  {
					 fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, fec_allocation_convert_fec_id_to_fec_table_id(new_chunk_index_start+i));
					 rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list,new_chunk_index_start + i,  ecmp_real_id);
					 if (rc != BCM_E_NONE)
					 {
						LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
						BCM_FREE(intf_array);
						return rc;
					 }
				  }

				  /* deleting old copies fec */
				  for (i = 0; i < intf_count; i++) 
				  {
					 fec_allocation_delete_old_copy(unit, *(intf_array+i));
				  }
				  BCM_FREE(intf_array);
				 
		  }
      }
   }
	  
   /*Creatingthe FEC*/
     
   rc =  bcm_l3_egress_create(unit, flags, egr, intf);
   fec_id_in_table = fec_allocation_convert_fec_id_to_fec_table_id(*intf);
   BCMDNX_IF_ERR_EXIT(rc); 
    /*we need to update the data structres*/
   
		
	  /* data structure update */
	  /*updating the alloc_manager*/
	  fec_allocation_set_value(unit,fec_id_in_table, fec_id_in_table); /* A real fec id will be pointed by himself.*/

	  /*updating the fec_id's list*/
	  fec_allocation_create_fec_id_ecmp_struct(&(table->fec_id_list[unit][fec_id_in_table]),fec_id_in_table, DOES_NOT_BELONG_TO_ECMP);
	  /*initializing fecs_list*/

	  fecs_list = NULL;
	  BCMDNX_ALLOC(fecs_list, sizeof(  linked_list_t ), "Linked list");
	  sal_memset(fecs_list,0, sizeof(  linked_list_t ));

	  fec_allocation_linked_list_init(fecs_list);
	  table->fec_id_list[unit][fec_id_in_table]->fecs_list = fecs_list;

   
exit:
   BCMDNX_FUNC_RETURN;


}



/* See the bcm_l3_egress_ecmp_create documentation for more infromation. This function allows creating a ECMP with superfecs and regular FECs at the same time, and uses "copies" fecs in order to
   allow creating a ECMP with nonconsecutive FECs*/   
int fec_allocation_bcm_l3_egress_ecmp_create(int unit, bcm_l3_egress_ecmp_t *ecmp, int intf_count, bcm_if_t *intf_array)
{
   int *fecs_for_ecmp;
   int rc;
   int i;
   int current_fec_position;
   int fec_father_table_id;
   int ecmp_real_id;
   bcm_if_t *intf_array_old;
   int intf_size;
   int intf_count_old;
   uint8 ecmp_is_real_flag;
   bcm_failover_t fail_id;
   uint8 fec_is_superfec_flag;
   bcm_if_t desired_fec_copy_id;
   bcm_if_t failover_if_id;
   bcm_l3_egress_ecmp_t ecmp_copy_old;


   BCMDNX_INIT_FUNC_DEFS;

   BCMDNX_NULL_CHECK(ecmp);
   BCMDNX_NULL_CHECK(intf_array);
   rc = BCM_E_NONE;
   intf_array_old = NULL;
  
   /* Checking that all FEC's are real fecs. */


   for (i =0 ; i< intf_count; i++)
   {
	  if (!fec_allocation_fec_is_real(unit,intf_array[i]))
	  {
		 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  One of the FEC id is not associated with a real FEC id."))); 
		 rc =  BCM_E_PARAM;
		 BCMDNX_IF_ERR_EXIT(rc); 
  
	  }
   }


   /*checking that ecmp max path < intf count*/
   if (ecmp->max_paths < intf_count)
   {
	   	   BCM_FREE(intf_array);
		   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  ecmp.max_paths < intf_count"))); 
		   BCMDNX_IF_ERR_EXIT(rc); 
   }


    /* getting the ecmp data in case the user wants to edit the ecmp*/

    if (( ecmp->flags & (BCM_L3_WITH_ID |  BCM_L3_REPLACE)) == (BCM_L3_WITH_ID |  BCM_L3_REPLACE))
    {
           ecmp_copy_old = *ecmp; 
           intf_size = SOC_DPP_CONFIG(unit)->l3.ecmp_max_paths;
           intf_array_old = NULL;
           BCMDNX_ALLOC(intf_array_old, intf_size * sizeof( bcm_if_t), "Array for FECS.");
           rc = bcm_l3_egress_ecmp_get(unit, &ecmp_copy_old, intf_size,intf_array_old, &intf_count_old);
           if (rc != BCM_E_NONE)
           {
        	BCM_FREE(intf_array_old);
        	BCMDNX_IF_ERR_EXIT(rc);
           }
           fec_allocation_ecmp_is_real(unit,ecmp,&ecmp_is_real_flag);
    }


   /*first if FECS are valid ( consecutive and consists of of only superfecs or only regular fecs )- we use the regular function*/

   if (fec_allocation_check_fec_list_validity( unit, intf_array, intf_count)) 
   {
	  rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count, intf_array);
	 if (rc == BCM_E_NONE)  /* creation succeeded*/
	  {
		  ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);
		  /*data structure update*/
		  for (i=0; i<intf_count; i++) 
		  {      
			 table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i])]->ecmp_id = ecmp_real_id;
			 fec_allocation_fec_is_superfec(unit,intf_array[i],&fec_is_superfec_flag);
			 if (fec_is_superfec_flag)/* we need to update the failover as well*/
			 {
				table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i]  +  1 )]->ecmp_id = ecmp_real_id;
			 }
		  }
		  fec_allocation_set_value(unit,ecmp_real_id,intf_count);
		  return rc;

	 }
	 else /*failure*/
	 {
		 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Could not create the ECMP.")));
		 BCMDNX_IF_ERR_EXIT(rc); 
	 }

   }
   else
   {

		   fail_id = table->fail_id_list[unit];

		   /* first case: no superfecs*/
		   if(!fec_allocation_fec_list_contains_superfec(unit, intf_array,intf_count))
		   {
			  current_fec_position =  fec_allocation_alloc_chunk(unit,intf_count);
			  if (current_fec_position == -1) /* no place found for copying the fecs */
			  {   
				  return BCM_E_PARAM;
			  }
			  
			  fecs_for_ecmp = NULL;
			  BCMDNX_ALLOC( fecs_for_ecmp, (intf_count+1)*sizeof(int), "Array for Fec's.");

			  /* copying all fecs*/
			  for (i = 0; i < intf_count; i++)
			  {
				 desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id(current_fec_position + i);
				 fecs_for_ecmp[i] = desired_fec_copy_id;
				 rc =  fec_allocation_copy_fec(unit,desired_fec_copy_id , intf_array[i], -1, -1);
			
				 if (rc != BCM_E_NONE) 
				 {
						 BCM_FREE(fecs_for_ecmp);
						 BCMDNX_IF_ERR_EXIT(rc); 
				 }
			  }

			  /*creating the ECMP*/

			  rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count, fecs_for_ecmp);
			  BCM_FREE(fecs_for_ecmp);
			  BCMDNX_IF_ERR_EXIT(rc); 
			  ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);
			  /* updating the fec's id list */
			  for (i = 0; i < intf_count; i++)
			  {
				 fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, current_fec_position + i);
				 rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list,current_fec_position + i,ecmp_real_id);
                 BCMDNX_IF_ERR_EXIT(rc); 


			  }
				fec_allocation_set_value(unit,ecmp_real_id,intf_count);

		   }

		  /* second case : one or more of the FECs is a super fec.*/
		  else 
		  {
					 /*we need a double size - one for the FEC and one for the failover FEC*/
					  current_fec_position =  fec_allocation_alloc_chunk(unit, 2 * intf_count);
					  if (current_fec_position == -1)  /* no place found for copying the fecs */
					  {  
						 rc = BCM_E_PARAM;
						 BCMDNX_IF_ERR_EXIT(rc); 
					  }
					  fecs_for_ecmp = NULL;
					  BCMDNX_ALLOC( fecs_for_ecmp,(intf_count+1)*sizeof(int), "Array for Fec's.");
					  /* we copy each FEC*/
					  for (i=0; i < intf_count; i++)
					  {

							 fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id( current_fec_position + 2*i);
							fec_allocation_fec_is_superfec(unit, intf_array[i],&fec_is_superfec_flag );
							 if (fec_is_superfec_flag) 
							 {
								 
							   desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id( current_fec_position + 1 + 2*i);

								fec_allocation_copy_fec(unit,desired_fec_copy_id, intf_array[i]+1 , 0, -2); /* we copy the failover regulary.*/

								/* now we copy the base FEC so he will point the the created FEC from above*/
								desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id( current_fec_position + 2*i);

								 failover_if_id =fec_allocation_convert_fec_table_id_to_fec_id(  current_fec_position + 1 + 2*i);
								fec_allocation_copy_fec(unit,  desired_fec_copy_id, intf_array[i],failover_if_id, -1);

							 } 
							 else /*fec is a regular fec - we copy in a special way so it seems like a protected FEC*/
							 {           
								/* the failover is always zero*/
								/* we allocate a fake FEC*/
								  desired_fec_copy_id  =   fec_allocation_convert_fec_table_id_to_fec_id(current_fec_position + 1 + 2 * i);
								fec_allocation_copy_fec(unit, desired_fec_copy_id  , intf_array[i], 0, fail_id); /* we copy the failover regulary.*/

								/* we allocate the protected FEC */
								 desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id(current_fec_position + 2 * i);
								 failover_if_id = fec_allocation_convert_fec_table_id_to_fec_id(current_fec_position + 1 + 2 * i);
								fec_allocation_copy_fec(unit, desired_fec_copy_id , intf_array[i], failover_if_id , fail_id);
							 }

					  }
				  /*creating the ECMP*/
				  rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count, fecs_for_ecmp); /* size is intf_count becuase we make an ecmp consists of superfecs*/
				  BCM_FREE(fecs_for_ecmp);
                  BCMDNX_IF_ERR_EXIT(rc); 


				  ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);

				  /* database update */

				  /* updating the fec's id list */
				  for (i = 0; i < intf_count * 2; i++)
				  { 
					 fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit,(current_fec_position + i));
					 rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list, current_fec_position + i,  ecmp_real_id);
					BCMDNX_IF_ERR_EXIT(rc); 

				  }
					fec_allocation_set_value(unit,ecmp_real_id,intf_count);
		   }
   }
   /* If we arrived to that point - the creation succeeded. */

   /* Edge case - the user is calling the function with "replace" and "with id" flags - we need to update our database accordingly */
  
   if (( ecmp->flags & (BCM_L3_WITH_ID |  BCM_L3_REPLACE)) == (BCM_L3_WITH_ID |  BCM_L3_REPLACE)) 
   {
	  
	   /*if previous ecmp pointed on copies - we can delete them*/
       if (!ecmp_is_real_flag)
	   {
           for (i =0; i<intf_count_old; i++)
		   {
			   fec_allocation_delete_old_copy(unit,intf_array_old[i]);
           }
       }
	   else /* the fecs were real - we need to update the data structe*/
	   {
         for (i =0; i<intf_count_old; i++)
		   {
			  table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array_old[i])]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
              if (fec_is_superfec_flag)/* we need to update the failover as well*/
			  {
				table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i]  +  1 )]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
              }
           }
	   }

   }



exit:

   BCMDNX_FUNC_RETURN;
}




/* See bcm_l3_egress_ecmp_add documentation for more infromation. This function allows adding a nonconsecutive FEC and adding a superfec or a regular FEC to a regular ECMP or protected ECMP respectively*/   
int fec_allocation_bcm_l3_egress_ecmp_add(int unit, bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf)
{
   int rc;
   bcm_if_t *intf_array;
   int intf_size;
   int intf_count;
   uint8 is_real_ecmp;
   int i;
   int new_chunk_index_start;
   int ecmp_real_id;
   int fec_father_table_id;
   int fec_table_id;
   int *copy_new_itf_array;
   bcm_failover_t fail_id;
   int fec_doesnt_belong_to_ecmp;
   uint8 fec_is_superfec_flag;
   uint8 ecmp_consists_of_superfecs;
   int *fecs_for_ecmp; 
   bcm_if_t desired_fec_copy_id;
   bcm_if_t original_fec_id; 
   bcm_if_t failover_if_id;
   int old_ecmp_count;
   int first_option_free_place;
   int second_option_free_place;
   int free_place;
   int free_place_table_id;

   
   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(ecmp);



  /*we assume the user transfers a real fec*/
   if (!fec_allocation_fec_is_real(unit,intf)) 
   {
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  The FEC id is not associated with a real FEC id.")));
	  return BCM_E_PARAM;
   }


   /*the flags must be zero according to the regular ecmp add function*/
   if (ecmp->flags!=0) 
   {
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Flags must be zero")));
	  return BCM_E_PARAM;
   }

    fail_id = table->fail_id_list[unit];

 
   /* getting the ecmp data*/
   intf_size = ecmp->max_paths;
   intf_array = NULL;
   BCMDNX_ALLOC( intf_array, intf_size*sizeof(bcm_if_t), "Array for FECS");
   rc = bcm_l3_egress_ecmp_get(unit, ecmp, intf_size, intf_array, &intf_count);
   if (rc != BCM_E_NONE)
   {
	 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Couldn't get the ECMP details. \n")));
     BCM_FREE(intf_array);
     BCMDNX_IF_ERR_EXIT(rc);  
   }

   /*checking that the FEC doesn't already exist in the ecmp*/
   if (fec_allocation_fec_exists_array( intf_array,intf_count, intf))
   {
	    LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  FEC already exists in the ECMP")));
		BCM_FREE(intf_array);
		BCMDNX_IF_ERR_EXIT(rc);  
   }


   /*checking that ecmp max path < intf count*/
   if (ecmp->max_paths < intf_count)
   {
		   BCM_FREE(intf_array);
		   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  ecmp.max_paths < intf_count"))); 
		   BCMDNX_IF_ERR_EXIT(rc); 
   }


   fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(intf);
   fec_allocation_ecmp_is_real(unit, ecmp,&is_real_ecmp);
   fec_doesnt_belong_to_ecmp = table->fec_id_list[unit][fec_table_id ]->ecmp_id == DOES_NOT_BELONG_TO_ECMP;
   fec_allocation_fec_is_superfec(unit,  intf, &fec_is_superfec_flag );
   fec_allocation_ecmp_is_superfec(unit,  ecmp,  &ecmp_consists_of_superfecs );
   if (is_real_ecmp && fec_doesnt_belong_to_ecmp) 
   {
	   
      /* Maybe the FEC is already in place. so we try to add it. In addition we don't allow a fec to be in more than one ECMP*/
      /* we assume the user is giving a real fec - he doesn't know about the fake one's*/
       if ( (!fec_is_superfec_flag &&(intf == (intf_array[0]-1 )|| intf == (intf_array[intf_count-1]+1 ) )) || 
			(fec_is_superfec_flag &&(intf == (intf_array[0]-2 )|| intf == (intf_array[intf_count-1]+2 ) )))
	   {

		  rc =  bcm_l3_egress_ecmp_add(unit, ecmp,  intf);
		  if (rc == BCM_E_NONE)
		  { /*succeded */
			 /*updating data structers*/

			 ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);
			 if (!fec_is_superfec_flag) 
			 {
			   table->fec_id_list[unit][ fec_table_id]->ecmp_id = ecmp_real_id;
			 } 
			 else /* super fec situation */
			 {     
				table->fec_id_list[unit][fec_table_id]->ecmp_id = ecmp_real_id;
				table->fec_id_list[unit][fec_table_id+1]->ecmp_id = ecmp_real_id;
			 }
			 old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
			 fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count+1);

			 BCM_FREE(intf_array);
			 /*changing the flags to zero as the user transfered the ecmp object*/
			 ecmp->flags = 0;
			 return rc;
		  }
		  else
		  {
			ecmp->flags = 0;
			BCMDNX_IF_ERR_EXIT(rc);

		  }
       }
   }
   /*second termination case: there is a place before or after a copy ecmp - we can copy the fec to that place and finish*/
    if (!is_real_ecmp ) 
    {
		   first_option_free_place = fec_allocation_convert_fec_id_to_fec_table_id(intf_array[0]-1);
		   second_option_free_place =  fec_allocation_convert_fec_id_to_fec_table_id(intf_array[intf_count-1]+1);
		   /*case 1: regular fec and the upper cell or the down cell are free and the ecmp concsists of regular fecs*/
		   if (!ecmp_consists_of_superfecs && !fec_is_superfec_flag && (fec_allocation_is_empty(unit,  first_option_free_place ) ||fec_allocation_is_empty(unit,second_option_free_place)))
		   {
               if (fec_allocation_is_empty(unit,  first_option_free_place )) 
			   {
				   free_place = intf_array[0]-1;
               }
			   else
			   {
				   free_place = intf_array[intf_count-1]+1;
			   }
			  fec_allocation_copy_fec(unit,free_place,intf,-1,-1 );
			  rc =  bcm_l3_egress_ecmp_add(unit, ecmp, free_place);
			  if (rc == BCM_E_NONE)
			  { /*succeded */
				 /*updating data structers*/
			     ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);
				 old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
				 fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count+1);
				 free_place_table_id = fec_allocation_convert_fec_id_to_fec_table_id(free_place);
				 fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit,free_place_table_id);
				 rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list ,free_place_table_id,  ecmp_real_id);
				 if (rc != BCM_E_NONE)
				 {
					 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
					 BCM_FREE(intf_array);
					 BCMDNX_IF_ERR_EXIT(rc);
				 } 
				 return rc;
				   
			  } 
          }
		     /*case 2: super fec and the upper cell(*2) or the down cell(*2) are free*/
		   if (ecmp_consists_of_superfecs && fec_is_superfec_flag && ((fec_allocation_is_empty(unit, first_option_free_place ) &&(fec_allocation_is_empty(unit, first_option_free_place-1 )))
										  || (fec_allocation_is_empty(unit,second_option_free_place) && (fec_allocation_is_empty(unit,second_option_free_place+1)))))
		   {
               if (fec_allocation_is_empty(unit, first_option_free_place ) &&(fec_allocation_is_empty(unit, first_option_free_place-1 ))) 
			   {
				   free_place = intf_array[0]-2;

               }
			   else
			   {
				   free_place = intf_array[intf_count-1]+2;
			   }

				fec_allocation_copy_fec(unit,free_place+1,intf+1,0,-2 );
				fec_allocation_copy_fec(unit,free_place,intf,free_place+1,-1 );
			  
			  rc =  bcm_l3_egress_ecmp_add(unit, ecmp,  free_place);
			  if (rc == BCM_E_NONE)
			  { /*succeded */
				 /*updating data structers*/
			     ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);
				 old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
				 fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count+1);
                 /*updating the base FEC*/
	 			 free_place_table_id = fec_allocation_convert_fec_id_to_fec_table_id(free_place);
				 fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit,free_place_table_id);
				 rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list ,free_place_table_id,  ecmp_real_id);
				 if (rc != BCM_E_NONE)
				 {
					 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
					 BCM_FREE(intf_array);
					 BCMDNX_IF_ERR_EXIT(rc);
				 } 
                 /*updating the Failover FEC*/
				 free_place_table_id = fec_allocation_convert_fec_table_id_to_fec_id(free_place+1);
				 fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit,free_place_table_id);
				 rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list ,free_place_table_id,  ecmp_real_id);
				 if (rc != BCM_E_NONE)
				 {
					 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
					 BCM_FREE(intf_array);
					 BCMDNX_IF_ERR_EXIT(rc);
				 } 
		
		
				 return rc;
				   
			  } 
          }
		
   }
	
   /*If not, we copy all chunk and create a new ECMP. Then, we iterate over the old chunk and delete every old copied FEC - in case the ECMP consists of copies*/
	   ecmp->flags =  (BCM_L3_REPLACE | BCM_L3_WITH_ID); /* we want to determine the id of the ECMP. i.e stay as it was.*/

		 fec_allocation_fec_is_superfec(unit,  intf, &fec_is_superfec_flag );

		 /* the ecmp consists of real fecs - we can use the function of creating a new ecmp */
		 if (is_real_ecmp) 
		 {
			copy_new_itf_array = NULL;
			BCMDNX_ALLOC(copy_new_itf_array,(intf_count + 1)*sizeof(int), "Array for FECS");
			sal_memcpy(copy_new_itf_array, intf_array, intf_count*sizeof(int));
			copy_new_itf_array[intf_count] = intf;
			for ( i=0; i< intf_count; i++) 
			{
			  table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i])]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
              if (ecmp_consists_of_superfecs)
              {
                  /* we need to update the failover as well*/
                    table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i]+1)]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
              }
			}
			BCM_FREE(intf_array);
			rc = fec_allocation_bcm_l3_egress_ecmp_create(unit, ecmp, intf_count + 1,  copy_new_itf_array);
			/*changing the flags to zero as the user transfered the ecmp object*/
			ecmp->flags = 0;
			return rc;

		 }
		 /* Now we have 3 cases:
			case 1:
			adding a  regular fec to a regular ecmp consists of regular copies*/
		 if (!ecmp_consists_of_superfecs  && !fec_is_superfec_flag) 
		 {

			new_chunk_index_start =  fec_allocation_alloc_chunk(unit, intf_count + 1);
			if (new_chunk_index_start == -1) {   /* no place found for copying the fecs */
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "FEC table is FULL\n")));
			   BCM_FREE(intf_array);
			   /*changing the flags to zero as the user transfered the ecmp object*/
			   ecmp->flags = 0;
			   return BCM_E_FULL; 
			}
	 
		   fecs_for_ecmp = NULL;
		   BCMDNX_ALLOC( fecs_for_ecmp, (intf_count+1)*sizeof(int), "Array for Fec's.");
			/* copying all fecs*/
			for (i = 0; i < intf_count; i++)
			{
			   fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + i);
			   desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + i);
			   original_fec_id = fec_allocation_get_fec_father_id_number(unit,intf_array[i]);
			   rc =  fec_allocation_copy_fec(unit,desired_fec_copy_id, original_fec_id, -1, -1);
			   if (rc != BCM_E_NONE)
			   {
				  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't create the shadow FEC\n")));
				  BCM_FREE(intf_array);
				  BCM_FREE(fecs_for_ecmp);
				  /*changing the flags to zero as the user transfered the ecmp object*/
				  ecmp->flags = 0;
				  return rc;
			   }
			}

			/* copying the new fec to the end of the fec list. FEC is real*/
			fecs_for_ecmp[i] =  fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + intf_count);
			desired_fec_copy_id  = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + intf_count);
			rc =  fec_allocation_copy_fec(unit,desired_fec_copy_id , intf, -1, -1);
			if (rc != BCM_E_NONE) 
			{
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:  Error. Couldn't create the shadow FEC\n")));
			   BCM_FREE(intf_array);
               BCM_FREE(fecs_for_ecmp);
			   /*changing the flags to zero as the user transfered the ecmp object*/
			   ecmp->flags = 0;
			   return rc;
			}

			/*creating the ECMP*/
			rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count + 1, fecs_for_ecmp);
			BCM_FREE(fecs_for_ecmp);
			if (rc != BCM_E_NONE) {
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Couldn't create the ECMP \n")));
			   BCM_FREE(intf_array);
			   /*changing the flags to zero as the user transfered the ecmp object*/
			   ecmp->flags = 0;
			   return rc;
			}

			ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);

			/*updating data structure*/

			/* updating the fec's id list */
			for (i = 0; i < intf_count + 1; i++) 
			{
			   fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, new_chunk_index_start+i);
			   rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list,new_chunk_index_start + i,  ecmp_real_id);
			   if (rc != BCM_E_NONE)
			   {
				  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
				  BCM_FREE(intf_array);
				  /*changing the flags to zero as the user transfered the ecmp object*/
				  ecmp->flags = 0;
				  return rc;
			   }
			}
			old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
			fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count+1);

			/* deleting old copies fec */
			for (i = 0; i < intf_count; i++) 
			{
			   fec_allocation_delete_old_copy(unit, *(intf_array+i));
			}
			BCM_FREE(intf_array);
			/*changing the flags to zero as the user transfered the ecmp object*/
			 ecmp->flags = 0;
			return rc;
		 }

		 /* second case: adding a fec to a superfec ecmp consists of copies*/
		 if (ecmp_consists_of_superfecs) 
		 {
			new_chunk_index_start =  fec_allocation_alloc_chunk(unit, (intf_count)*2 + 2);
			if (new_chunk_index_start == -1)  /* no place found for copying the fecs */
			{  
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  FEC table is FULL\n")));
			   BCM_FREE(intf_array);	
			   rc = BCM_E_FULL;	 
			   	/*changing the flags to zero as the user transfered the ecmp object*/
			   ecmp->flags = 0;
			   BCMDNX_IF_ERR_EXIT(rc); 

			}

				 
			fecs_for_ecmp = NULL;
			BCMDNX_ALLOC( fecs_for_ecmp, (intf_count+1)*sizeof(int), "Array for Fec's.");

			/* copying all fecs*/
			for (i=0; i < intf_count; i++)
			{
				  fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + 2 * i);
				  desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 1 + 2 * i);
				  original_fec_id =  fec_allocation_get_fec_father_id_number(unit, intf_array[i]+1);
				  fec_allocation_copy_fec(unit, desired_fec_copy_id, original_fec_id , 0, -2); /* we copy the failover regulary.*/
				  /* now we copy the base FEC so he will point the the created FEC from above*/
				   desired_fec_copy_id = fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 2 * i);
				   original_fec_id = fec_allocation_get_fec_father_id_number( unit, intf_array[i]);
					failover_if_id  = fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 1 + 2 * i);
				   fec_allocation_copy_fec(unit, desired_fec_copy_id,  original_fec_id,   failover_if_id, -1);

			}

			/* copying the new fec to the end of the fec list*/
			/* in a case the fec is a super fec */
			 fec_allocation_fec_is_superfec(unit,intf,&fec_is_superfec_flag);
			if (fec_is_superfec_flag)
			{
				 desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id(  new_chunk_index_start + (intf_count)*2 + 1) ;
			   fec_allocation_copy_fec(unit, desired_fec_copy_id, intf + 1, 0, -2); /* we copy the failover regulary*/
			   /* now we copy the base FEC so he will point the the created FEC from above*/
			   fecs_for_ecmp[i] =fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + (intf_count)*2);
			   desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id(  new_chunk_index_start + (intf_count)*2 );
			   failover_if_id =  fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + (intf_count)*2 + 1);

			   fec_allocation_copy_fec(unit, desired_fec_copy_id , intf,  failover_if_id, -1);
			} 
			else /* regular fec - we copy it so it will like a superfec*/
			{        		
				desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + (intf_count)*2 + 1);					  
			   /* the failover is always zero*/
			   fec_allocation_copy_fec(unit,  desired_fec_copy_id  , intf, 0, fail_id); /* we copy the failover regulary. */
			   /* now we copy the base FEC so he will point the the created FEC from above*/
			   fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + (intf_count)*2);
			   desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + (intf_count)*2);
			   failover_if_id =  fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + (intf_count)*2 + 1);
			   fec_allocation_copy_fec(unit,desired_fec_copy_id , intf,failover_if_id , fail_id);

			}

			/*creating the ECMP*/
			rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count + 1, fecs_for_ecmp);
			BCM_FREE(fecs_for_ecmp);
			if (rc != BCM_E_NONE) 
			{
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit," Fec_Allocation:  Couldn't create the ECMP \n")));
			   BCM_FREE(intf_array);
				/*changing the flags to zero as the user transfered the ecmp object*/
				ecmp->flags = 0;
			   return rc;
			}
			ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);

			/*updating data structure*/
			/*updating the fec's id list */
			for (i = 0; i < intf_count * 2 + 2; i++)
			{
			   fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, new_chunk_index_start + i);
			   rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list,new_chunk_index_start + i,  ecmp_real_id);
			   if (rc != BCM_E_NONE) 
			   {
				  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
				  BCM_FREE(intf_array);
				  /*changing the flags to zero as the user transfered the ecmp object*/
				  ecmp->flags = 0;
				  return rc;
			   }
			}
			 old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
			 fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count+1);
			/* deleting old copies fec */
			for (i = 0; i < intf_count; i++ ) 
			{
			   fec_allocation_delete_old_copy(unit, intf_array[i]); /* the deletion function deletes the superfec as well*/
			}
			/*changing the flags to zero as the user transfered the ecmp object*/
			ecmp->flags = 0;
			return rc;
		 }

		 /*third case: adding a  super fec to a regular ecmp consists of regular copies*/
		  fec_allocation_fec_is_superfec(unit,intf, &fec_is_superfec_flag);
		 if (!ecmp_consists_of_superfecs &&fec_is_superfec_flag)
		 {
			new_chunk_index_start =  fec_allocation_alloc_chunk(unit,(intf_count)*2 + 2);
			if (new_chunk_index_start == -1)  /* no place found for copying the fecs */
			{  
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the Failover id\n")));
			   BCM_FREE(intf_array); 
			   	/*changing the flags to zero as the user transfered the ecmp object*/
			   ecmp->flags = 0;
			   BCMDNX_IF_ERR_EXIT(rc);   
			}

			fecs_for_ecmp = NULL;
			BCMDNX_ALLOC( fecs_for_ecmp, (intf_count+1)*sizeof(int), "Array for Fec's.");
			/* the failover is always zero*/
			for (i=0; i < (intf_count); i++)
			{
			   fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + 2 * i);

				desired_fec_copy_id =fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 1 + 2 * i);
				original_fec_id = fec_allocation_get_fec_father_id_number(unit, intf_array[i]+1);
			   fec_allocation_copy_fec(unit,     desired_fec_copy_id,  original_fec_id  , 0, fail_id); /* we copy the failover regulary with a fake fail_id */
			   /* now we copy the base FEC so he will point the the created FEC from above*/

			   desired_fec_copy_id =fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + 2 * i);
			   original_fec_id = fec_allocation_get_fec_father_id_number (unit,intf_array[i]);
			   failover_if_id =  fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + 1 + 2 * i);
			   fec_allocation_copy_fec(unit, desired_fec_copy_id,  original_fec_id ,   failover_if_id, fail_id);
			}
		   
			/* copying the new fec to the end of the fec list*/
				desired_fec_copy_id =  fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + intf_count * 2 + 1);
			   fec_allocation_copy_fec(unit,desired_fec_copy_id , intf+ 1, 0, -2); /* we copy the failover regulary. */
			   /* now we copy the base FEC so he will point the the created FEC from above*/
			   fecs_for_ecmp[i] = fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + intf_count * 2);
			   desired_fec_copy_id =fec_allocation_convert_fec_table_id_to_fec_id( new_chunk_index_start + intf_count * 2) ;
			   failover_if_id = fec_allocation_convert_fec_table_id_to_fec_id(new_chunk_index_start + intf_count * 2 + 1);

			   fec_allocation_copy_fec(unit, desired_fec_copy_id , intf,  failover_if_id , -1);

			   /*creating the ECMP*/
			   rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count+1,fecs_for_ecmp);
			   BCM_FREE(fecs_for_ecmp);
			   if (rc != BCM_E_NONE) 
			   {
				  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:   Couldn't create the ECMP \n")));
				  BCM_FREE(intf_array);
				  /*changing the flags to zero as the user transfered the ecmp object*/
				  ecmp->flags = 0;
				  BCMDNX_IF_ERR_EXIT(rc);   
			   }

			   /*updating data structure*/
			   ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);

			   /*updating the fec's id list*/
			   for (i = 0; i < (intf_count)*2+2; i++)
			   {
				  fec_father_table_id = fec_allocation_get_fec_father_table_id_number(unit, new_chunk_index_start + i);
				  rc = fec_allocation_linked_list_member_add(unit, table->fec_id_list[unit][fec_father_table_id]->fecs_list ,new_chunk_index_start + i,  ecmp_real_id);
				  if (rc != BCM_E_NONE)
				  {
					 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't add the FEC item to the list\n")));
					 BCM_FREE(intf_array);
					 /*changing the flags to zero as the user transfered the ecmp object*/
					 ecmp->flags = 0;
					 BCMDNX_IF_ERR_EXIT(rc);
				  }
			   }
			   old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
				fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count+1);

			   /* deleting old copies fec */
			   for (i = 0; i < intf_count; i ++) 
			   {
				  fec_allocation_delete_old_copy(unit, *(intf_array + i));
			   }
			   /*changing the flags to zero as the user transfered the ecmp object*/
			   ecmp->flags = 0;
			   BCMDNX_IF_ERR_EXIT(rc);
		 }
	


exit:
   BCMDNX_FUNC_RETURN;

}





/* See bcm_l3_egress_ecmp_delete documentation for more infromation. This function allows deleting a FEC at any index in the ECMP*/   
int fec_allocation_bcm_l3_egress_ecmp_delete(int unit, bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf)
{
    int rc;
    bcm_if_t *intf_array;
    int intf_size;
    int intf_count;
    uint8 is_real_ecmp;
    int i,j;
    int fec_table_id;
    int real_fec_id_to_remove;
    uint8 fec_remove_is_real;
    int *new_intf_array;
    uint8 ecmp_consists_of_superfecs;
	uint8 fec_is_superfec_flag;
   bcm_if_t original_fec_id; 
   int   old_ecmp_count;
   int ecmp_real_id;
   uint8 fec_to_remove_superfec_flag;


    BCMDNX_INIT_FUNC_DEFS;
	BCMDNX_NULL_CHECK(ecmp);
     real_fec_id_to_remove = 0;

	/*we assume the user transfers a real fec*/
	if (!fec_allocation_fec_is_real(unit,intf)) 
	{
		LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  The FEC id is not associated with a real FEC id.")));
		return BCM_E_PARAM;
	}
   /*the flags must be zero according to the regular ecmp add function*/
   if (!ecmp->flags==0) 
   {
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Flags must be zero")));
	  return BCM_E_PARAM;
   }


    fec_allocation_ecmp_is_superfec(unit, ecmp, &ecmp_consists_of_superfecs);
    fec_allocation_ecmp_is_real(unit, ecmp,&fec_remove_is_real);

  
	 /* getting the ecmp data*/
    intf_size = ecmp->max_paths;
	intf_array = NULL;
	BCMDNX_ALLOC(intf_array , intf_size*sizeof(bcm_if_t), "Array for FECS");
    rc = bcm_l3_egress_ecmp_get(unit, ecmp, intf_size, intf_array, &intf_count);
    if (rc != BCM_E_NONE) 
	{
	   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Couldn't get the ECMP details. \n")));
       BCM_FREE(intf_array);
       return rc;
    }




    fec_allocation_get_real_fec_of_ecmp( unit,ecmp, intf, &real_fec_id_to_remove );
    fec_allocation_fec_is_superfec(unit, intf,&fec_to_remove_superfec_flag);



     if (!fec_to_remove_superfec_flag && ecmp_consists_of_superfecs &&!fec_remove_is_real)  /* we inserted to the database both failover and base as the copies of the "regular fec" so we need to extract the
       base fec first, so in case we get the fail id, we need to substruct 1 form the id number.*/
	 {
           if ((real_fec_id_to_remove % 2) == 1 ) 
           {
               		 real_fec_id_to_remove--;
           }
	 }

       /*checking that the FEC exists in the ecmp*/
   if (!fec_allocation_fec_exists_array(intf_array,intf_count,real_fec_id_to_remove))
   {
	    LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  FEC is not contained in the ECMP")));
		BCM_FREE(intf_array);
		BCMDNX_IF_ERR_EXIT(rc);  
   }

    fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(real_fec_id_to_remove);
    fec_allocation_ecmp_is_real(unit, ecmp,&is_real_ecmp);
    ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);


   /*in case the ecmp consists of copies FEC*/
   if (!is_real_ecmp)
   {
	   /* first case: the removed fec is at the begining or at the end of the FEC entries*/
	  if ( real_fec_id_to_remove == intf_array[0] || real_fec_id_to_remove == intf_array[intf_count-1])
	  {
		 rc =   bcm_l3_egress_ecmp_delete( unit, ecmp, real_fec_id_to_remove);
		 if (rc != BCM_E_NONE)
		 {
			LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't remove the FEC from the ECMP\n")));
			BCM_FREE(intf_array);
			return rc;
		 }
		 fec_allocation_delete_old_copy(unit,real_fec_id_to_remove);
		 old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
		 fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count-1);
		 return rc;
	  }
	  else
	  { 
		 /*we do the trick of removing the fec and putting the last fec instead of him*/
		 /* if ecmp consists of regulars fec*/
		 if ( !ecmp_consists_of_superfecs )
		 {          
		   fec_allocation_delete_old_copy(unit,real_fec_id_to_remove);
		   fec_allocation_copy_fec(unit, real_fec_id_to_remove, intf_array[intf_count-1],-1,-1);
		   rc = bcm_l3_egress_ecmp_delete( unit, ecmp,  intf_array[intf_count-1]);
           fec_allocation_delete_old_copy(unit,intf_array[intf_count-1]);
		   if (rc != BCM_E_NONE)
		   {
			  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't remove the FEC from the ECMP\n")));
			  BCM_FREE(intf_array);
			  return rc;
		   } 
		   	old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
			fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count-1);
			/* no need to update data structre- the ecmp stays as it was and delete_old_copy function took care of the removed FEC.*/
		 }
		 else /*ecmp consists of superfecs*/
		 {
		   fec_allocation_delete_old_copy(unit,real_fec_id_to_remove);

		   original_fec_id = fec_allocation_get_fec_father_id_number(unit, intf_array[intf_count-1] +1);
		   fec_allocation_copy_fec(unit, real_fec_id_to_remove +1,    original_fec_id ,0,-2); 
		   original_fec_id =  fec_allocation_get_fec_father_id_number(unit,  intf_array[intf_count-1]);
		   fec_allocation_copy_fec(unit, real_fec_id_to_remove, original_fec_id, real_fec_id_to_remove +1,-1);
           fec_allocation_delete_old_copy(unit,intf_array[intf_count-1]);
		   rc = bcm_l3_egress_ecmp_delete( unit, ecmp,  intf_array[intf_count-1]);
		   if (rc != BCM_E_NONE)
		   {
			   LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't remove the FEC from the ECMP\n")));
			   BCM_FREE(intf_array);
			   return rc;
			} 

		   old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
		  fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count-1);
			/* no need to update data structure- the ecmp stays as it was.*/
		 }
	  }
   }
   else /*ecmp consists of real fecs - we need to copy the fecs*/
   {
	  /* first case: the removed fec is at the begining or at the end of the FEC entries*/
	  if ( real_fec_id_to_remove == intf_array[0] || real_fec_id_to_remove == intf_array[intf_count-1])
	  {
		  rc = bcm_l3_egress_ecmp_delete( unit, ecmp, real_fec_id_to_remove);
		  if (rc != BCM_E_NONE)
		  {
			 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't remove the FEC from the ECMP\n")));
			 BCM_FREE(intf_array);
			 return rc;
		  } 

		 /*data structure update*/
		 table->fec_id_list[unit][fec_table_id]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
		  fec_allocation_fec_is_superfec(unit,real_fec_id_to_remove,&fec_is_superfec_flag);
		 if (fec_is_superfec_flag) /*we need to update the failover fec*/
		 {
			table->fec_id_list[unit][fec_table_id+1]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
		 }
		 ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp->ecmp_intf);
		 old_ecmp_count =  fec_allocation_get_value(unit, ecmp_real_id );
		 fec_allocation_set_value(unit,ecmp_real_id,old_ecmp_count-1);
	  }
	  else /* we copy the intf_array without the fec_id and call the regular ecmp create*/
	  {
		 new_intf_array = NULL;
		 BCMDNX_ALLOC( new_intf_array,((intf_count)-1)*( sizeof(int)), "Array for FECS");
		  j=0;
		  for (i =0; i<intf_count; i++) 
		  {
			 if (intf_array[i]!= real_fec_id_to_remove ) 
			 {
				new_intf_array[j] = intf_array[i];
				j++;
			 }
		  }

		  /*data structure update*/
		  /*updating data structure*/
		   for ( i=0; i< intf_count; i++) 
		   {
			 table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i])]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
              if ( ecmp_consists_of_superfecs ) /* we need to update the failover fec as well*/
              {
                   table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(intf_array[i]+1)]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
              }
             
		   }
			BCM_FREE(intf_array);
			ecmp->flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
			rc = fec_allocation_bcm_l3_egress_ecmp_create(unit, ecmp, intf_count-1,  new_intf_array);
			BCM_FREE(new_intf_array);
			ecmp->flags = 0 ;
			return rc;
	  }
  
    }

exit:
   BCMDNX_FUNC_RETURN;
}



/* See bcm_l3_egress_destroy documentation for more infromation. This function deletes the FEC pointed by fec_id, update database accordignly. 
   It assumes that fec_id pointes to a FEC that the user has created*/   
int fec_allocation_bcm_l3_egress_destroy(int unit, bcm_if_t intf)
{
   int rc;
    int fec_table_id; 
    bcm_l3_egress_ecmp_t ecmp;
    int current_ecmp_id;
	int ecmp_id;
	int fec_delete_table_id;
	int fec_delete_id;
	int ecmp_real_id;


    linked_list_member_t *linked_prev_p;

	
   BCMDNX_INIT_FUNC_DEFS;
   rc = BCM_E_NONE;

	 /*we assume the user transfers a real fec*/
	  if (!fec_allocation_fec_is_real(unit,intf)) 
	  {
		 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  The FEC id is not associated with a real FEC id.")));
		 rc =  BCM_E_PARAM;
		 BCMDNX_IF_ERR_EXIT(rc);  
	  }

    fec_table_id = fec_allocation_convert_fec_id_to_fec_table_id(intf);
	ecmp_id = table->fec_id_list[unit][ fec_table_id]->ecmp_id;

    /*We iterate over all ecmp's groups that the FEC belongs to, and call our function of remove*/
    /*first case - the real FEC is in a ECMP*/
    current_ecmp_id = fec_allocation_convert_fec_table_id_to_fec_id(ecmp_id);
    if ( ecmp_id != DOES_NOT_BELONG_TO_ECMP)
    {
       /* we try to get the ecmp struct - no need of extra information besides the ecmp intf number*/
        bcm_l3_egress_ecmp_t_init(&ecmp);
		ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp.ecmp_intf);
		ecmp.max_paths = fec_allocation_get_value(unit,ecmp_real_id );
        ecmp.ecmp_intf = current_ecmp_id;
        rc = fec_allocation_bcm_l3_egress_ecmp_delete( unit, &ecmp, intf);
        if (rc != BCM_E_NONE)
        {
              LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:   In a case you are trying to destroy a failover FEC, please call remove the base fec from the ecmp first.")));
               return rc;
        }
    }
    /* we iterate over all the copies of the fec and remove them form the ecmp's*/

    linked_prev_p = table->fec_id_list[unit][fec_table_id]->fecs_list->first_member;
    while(linked_prev_p != NULL) 
    {
	
	      bcm_l3_egress_ecmp_t_init(&ecmp);
		  current_ecmp_id = fec_allocation_convert_fec_table_id_to_fec_id(linked_prev_p->ecmp_id);
		  ecmp.ecmp_intf = current_ecmp_id;
		  ecmp_real_id = fec_allocation_convert_fec_id_to_fec_table_id(ecmp.ecmp_intf);
		  ecmp.max_paths = 	fec_allocation_get_value(unit,ecmp_real_id);
		  fec_delete_table_id =  fec_allocation_get_fec_father_table_id_number(unit,linked_prev_p->fec_id_table);
		  fec_delete_id = fec_allocation_convert_fec_table_id_to_fec_id(fec_delete_table_id );
		  rc = fec_allocation_bcm_l3_egress_ecmp_delete( unit, &ecmp,fec_delete_id );
          if (rc != BCM_E_NONE)
          {
              LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  In a case you are trying to destroy a failover FEC, please call remove the base fec from the ecmp first.")));
               return rc;
           }
		  /* delete will remove the fec becuase he is a copy fec*/
		  linked_prev_p = table->fec_id_list[unit][fec_table_id]->fecs_list->first_member;
	}

    rc = bcm_l3_egress_destroy( unit, intf);
    if (rc != BCM_E_NONE)
    {
       LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Fec_Allocation:  Error. Couldn't destroy the FEC\n.")));
      return rc;
    } 
    table->array_[unit][fec_table_id] = -1;

    fec_allocation_linked_list_destroy(unit, table->fec_id_list[unit][fec_table_id]->fecs_list);
    BCM_FREE( table->fec_id_list[unit][fec_table_id]);
  




	
exit:
   BCMDNX_FUNC_RETURN;
 }





/* See bcm_l3_egress_ecmp_destroy documentation for more infromation.*/   

int fec_allocation_bcm_l3_egress_ecmp_destroy(int unit,bcm_l3_egress_ecmp_t *ecmp)
{
   int i,rc;
   uint8 ecmp_is_real_flag;
   uint8 ecmp_consists_of_superfecs;
   bcm_if_t *intf_array;
   int intf_size;
   int intf_count;

   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(ecmp);


      /* getting the ecmp data*/
   intf_size = ecmp->max_paths; 
   intf_array = NULL;
   BCMDNX_ALLOC(intf_array,intf_size*sizeof( bcm_if_t), "Array for FECS");
   rc = bcm_l3_egress_ecmp_get(unit, ecmp, intf_size, intf_array, &intf_count);
   if (rc != BCM_E_NONE)
   {
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:   Couldn't get the ECMP details. \n")));
      BCM_FREE(intf_array);
      return rc;
   }


   fec_allocation_ecmp_is_real(unit,ecmp, &ecmp_is_real_flag);
   fec_allocation_ecmp_is_superfec(unit,ecmp,&ecmp_consists_of_superfecs);
   /*in case the ecmp is real, we can call destroy fec function*/
   if (ecmp_is_real_flag)
   {
       	 rc = bcm_l3_egress_ecmp_destroy(unit,ecmp);
	 if (rc != BCM_E_NONE) 
	 {
		  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:  Couldn't destroy the ECMP.\n")));
		  BCM_FREE(intf_array);
		  return rc;
     }
      /*data structure update*/
      for (i=0;i<intf_count; i++) 
      {
         table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(fec_allocation_convert_fec_table_id_to_fec_id(intf_array[i]))]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
		 if (ecmp_consists_of_superfecs) 
		 {
			table->fec_id_list[unit][fec_allocation_convert_fec_id_to_fec_table_id(fec_allocation_convert_fec_table_id_to_fec_id(intf_array[i]+1))]->ecmp_id = DOES_NOT_BELONG_TO_ECMP;
		 }
	  }
   }
   else  /*the ecmp is an ecmp of copies - we can remove the fecs and the ecmp*/
   {
	  /*destorying the ecmp*/
	  rc =  bcm_l3_egress_ecmp_destroy( unit, ecmp);
	  if (rc != BCM_E_NONE) 
	  {
		 LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Fec_Allocation:  Error. Couldn't destroy the FEC\n")));
		 BCM_FREE(intf_array);
		 return rc;
	  }
      for (i=0;i<intf_count; i++) 
	  {
		 fec_allocation_delete_old_copy(unit,intf_array[i]);
	  }

   }
   BCM_FREE(intf_array);
   
exit:
   BCMDNX_FUNC_RETURN;
}


int soc_dpp_fec_allocation_main(int unit)
{
    return fec_allocation_main(unit, 1);
}


/*
 * Function:
 *      fec_allocation_main
 * Purpose:
 *      A test - to be used with regression or as an example of use
 *
 * Parameters:
 *      unit       - (IN) Device Number
 */
int fec_allocation_main(int unit, int write_logs)
{
  int rc;
  bcm_l3_egress_ecmp_t *ecmp = NULL;
  bcm_l3_egress_t *regular_fecs = NULL;
  bcm_l3_egress_t *superfec_base = NULL;
  bcm_l3_egress_t *superfec_failover = NULL;
  bcm_if_t regular_fecs_id[100];
  bcm_if_t superfec_base_id[100];
  bcm_if_t superfec_failover_id[100];
  bcm_if_t fecs_buffer[100];
  int i;
  bcm_failover_t fail_id;
  rc =  BCM_E_NONE;

  LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC allocation test start\n")));

  /*creating the failover*/
  rc = bcm_failover_create(unit, BCM_FAILOVER_FEC, &fail_id);
  if (rc != BCM_E_NONE)
  {
	  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Failed creating failover\n")));
	  goto exit;
  }

  rc = fec_allocation_initialize(0);
  if (rc != BCM_E_NONE)
  {
       LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Couldn't initialize the data structure")));
       goto exit;
  }


  superfec_base = sal_alloc(sizeof(bcm_l3_egress_t)*100, "fec_allocation_main.superfec_base");
  if(superfec_base == NULL) {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Memory allocation failure")));
      rc = BCM_E_MEMORY;
      goto exit;
  }
  superfec_failover = sal_alloc(sizeof(bcm_l3_egress_t)*100, "fec_allocation_main.superfec_failover");
  if(superfec_failover == NULL) {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Memory allocation failure")));
      rc = BCM_E_MEMORY;
      goto exit;
  }
  /*initializing bcm_l3_egress_ecmp_t superfecs objdects*/
  for (i =0 ; i< 100; i++)
  {
     bcm_l3_egress_t_init(&superfec_failover[i]);
     superfec_failover[i].vlan   = 0;
     superfec_failover[i].module = 0;
     superfec_failover[i].port   = 0;
     superfec_failover[i].failover_id = fail_id;
     superfec_failover[i].failover_if_id = 0;
     rc = fec_allocation_bcm_l3_egress_create(unit,0, &superfec_failover[i],&superfec_failover_id[i]);
     if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC creation has failed")));
         goto exit;
     }
     bcm_l3_egress_t_init(&superfec_base[i]);
     superfec_base[i].vlan   = 0;
     superfec_base[i].module = 0;
     superfec_base[i].port   = 0;
     superfec_base[i].failover_id = fail_id;
     superfec_base[i].failover_if_id = superfec_failover_id[i];
     rc = fec_allocation_bcm_l3_egress_create(unit,0 , &superfec_base[i],&superfec_base_id[i]);
     if (rc != BCM_E_NONE)
     {
          LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC creation has failed")));
          goto exit;
     }
  }

  regular_fecs = sal_alloc(sizeof(bcm_l3_egress_t)*100, "fec_allocation_main.regular_fecs");
  if(regular_fecs == NULL) {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Memory allocation failure")));
      rc = BCM_E_MEMORY;
      goto exit;
  }
  /*creating regular fecs */
  for (i =0 ; i< 100; i++){
      bcm_l3_egress_t_init(&regular_fecs[i]);
      regular_fecs[i].vlan   = 0;
      regular_fecs[i].module = 0;
      regular_fecs[i].port   = 0;
      rc = fec_allocation_bcm_l3_egress_create(unit, 0, &regular_fecs[i],&regular_fecs_id[i]);
      if (rc != BCM_E_NONE)
      {
          LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC creation has failed")));
          goto exit;
      }
  }


  ecmp = sal_alloc(sizeof(*ecmp)*100, "fec_allocation_main.ecmp");
  if(ecmp == NULL) {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Memory allocation failure")));
      rc = BCM_E_MEMORY;
      goto exit;
  }

  /* Creating ECMP's */

 /*case 1: valid input - regular*/
  bcm_l3_egress_ecmp_t_init(&ecmp[0]);
  ecmp[0].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[0], 10,regular_fecs_id);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }

   /*case 2: valid input - superfecs*/
  bcm_l3_egress_ecmp_t_init(&ecmp[1]);
  ecmp[1].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[1], 10,superfec_base_id);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }

  /*case 3: invlaid input regular fecs unordered*/
  fecs_buffer[0] = regular_fecs_id[31];
  fecs_buffer[1] = regular_fecs_id[14];
  fecs_buffer[2] = regular_fecs_id[15];
  fecs_buffer[3] = regular_fecs_id[12];
  fecs_buffer[4] = regular_fecs_id[16];
  bcm_l3_egress_ecmp_t_init(&ecmp[2]);
  ecmp[2].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[2], 5,fecs_buffer);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }

    /*case 3: invlaid input superfecs + regular fecs unordered*/
  fecs_buffer[0] = regular_fecs_id[13];
  fecs_buffer[1] = superfec_base_id[11];
  fecs_buffer[2] = regular_fecs_id[17];
  fecs_buffer[3] = superfec_base_id[12];
  fecs_buffer[4] = regular_fecs_id[18];
  bcm_l3_egress_ecmp_t_init(&ecmp[3]);
  ecmp[3].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[3], 5,fecs_buffer);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }

    /*case 4: invlaid input superfecs + regular fecs unordered - with already used fecs!*/
   fecs_buffer[0] = regular_fecs_id[19];
   fecs_buffer[1] = superfec_base_id[13];
   fecs_buffer[2] = regular_fecs_id[2];
   fecs_buffer[3] = superfec_base_id[12];
   fecs_buffer[4] = regular_fecs_id[3];
   bcm_l3_egress_ecmp_t_init(&ecmp[8]);
   ecmp[8].max_paths = 100;
   rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[8], 5,fecs_buffer);
   if (rc != BCM_E_NONE)
   {
       LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
       goto exit;
   }

  /*case 5: invlaid input superfecs + regular fecs unordered - with already used twice fecs!*/
  fecs_buffer[0] = regular_fecs_id[19];
  fecs_buffer[1] = superfec_base_id[13];
  fecs_buffer[2] = regular_fecs_id[2];
  fecs_buffer[3] = superfec_base_id[12];
  fecs_buffer[4] = regular_fecs_id[3];
  bcm_l3_egress_ecmp_t_init(&ecmp[4]);
  ecmp[4].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[4], 5,fecs_buffer);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }



  /*case 7: valid input - regular*/
    bcm_l3_egress_ecmp_t_init(&ecmp[6]);
    ecmp[6].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[6], 10,regular_fecs_id+60);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }

   /*case 8: valid input - superfecs*/
  bcm_l3_egress_ecmp_t_init(&ecmp[7]);
  ecmp[7].max_paths = 100;
  rc = fec_allocation_bcm_l3_egress_ecmp_create(unit,&ecmp[7], 10,superfec_base_id+60);
  if (rc != BCM_E_NONE)
  {
        LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP creation has failed")));
        goto exit;
  }

 /* adding fec section*/


  /*adding a regular fec to a real ecmp consists of regular - in the right place!*/
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[0], regular_fecs_id[10]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
      goto exit;
  }

  /*adding a superfec fec to a real ecmp consists of regular*/
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[0], superfec_base_id[39]);
  if (rc != BCM_E_NONE)
  {
     LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
     goto exit;
  }


  /*adding a regular to real ecmp consists of superfecs*/
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[1], regular_fecs_id[40]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
      goto exit;
  }


  /*adding a regular to  fake ecmp consists of regulars*/
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[2], regular_fecs_id[50]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
      goto exit;
  }


  /*adding a regular that already has a copy to  fake ecmp consists of regulars*/
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[2], regular_fecs_id[2]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
      goto exit;
  }


  /*adding a superfec to copy ecmp consists of superfecs */
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[1], superfec_base_id[45]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
      goto exit;
  }


  /*adding a regular to copy of copy ecmp consists of superfecs*/
  rc = fec_allocation_bcm_l3_egress_ecmp_add(unit,&ecmp[4], regular_fecs_id[50]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Adding fec to the ECMP has failed")));
      goto exit;
  }

  if (write_logs) {
	  rc = fec_allocation_create_database_file(unit); 
	  if (rc != BCM_E_NONE)
	  {
		  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Writing Database to the file has failes")));
		  goto exit;
	  }
  }


  /*remove section*/

  /*removing a regular fec from a regular ecmp - from the beginning */
  rc = fec_allocation_bcm_l3_egress_ecmp_delete(unit, &ecmp[6],regular_fecs_id[60]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC remove from ECMP has failed")));
      goto exit;
  }

    /*removing a regular fec from a regular ecmp - in the middle */
  rc = fec_allocation_bcm_l3_egress_ecmp_delete(unit, &ecmp[6], regular_fecs_id[65]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC remove from ECMP has failed")));
      goto exit;
  }

     /*removing a super fec from a copy superfec ecmp - in the middle */
  rc = fec_allocation_bcm_l3_egress_ecmp_delete(unit, &ecmp[4], superfec_base_id[12]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC remove from ECMP has failed")));
      goto exit;
  }


     /*removing a "regular" fec from a copy superfec ecmp - in the middle */
  rc = fec_allocation_bcm_l3_egress_ecmp_delete(unit, &ecmp[4], regular_fecs_id[2]);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC remove from ECMP has failed")));
      goto exit;
  }




/*destrying ECMP's 1 - 8 without 5*/

  for (i =0 ; i<=8; i++)
  {
      if (i==5)
      {
          continue;
      }
      rc = fec_allocation_bcm_l3_egress_ecmp_destroy(unit, &ecmp[i]);
      if (rc != BCM_E_NONE)
      {
          LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"ECMP remove has failed")));
          goto exit;
      }

  }

  if (write_logs) {
	  rc = fec_allocation_create_database_file(unit); 
	  if (rc != BCM_E_NONE)
	  {
		  LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Writing Database to the file has failes")));
		  goto exit;
	  }
  }


/* deletion section*/
 for (i =0 ; i< 100; i++)
 {    
      rc = fec_allocation_bcm_l3_egress_destroy(unit,  regular_fecs_id[i]);
      if (rc != BCM_E_NONE)
      {
          LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC destroy has failed")));
          goto exit;
      }
      rc = fec_allocation_bcm_l3_egress_destroy(unit,  superfec_base_id[i]);
      if (rc != BCM_E_NONE)
      {
          LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC destroy has failed")));
          goto exit;
      }
      rc = fec_allocation_bcm_l3_egress_destroy(unit,  superfec_failover_id[i]);
      if (rc != BCM_E_NONE)
      {
          LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC destroy has failed")));
          goto exit;
      }


 }

  rc = bcm_failover_destroy(unit,fail_id);
  if (rc != BCM_E_NONE)
  {
      LOG_ERROR(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"Failover destroy has failed")));
      goto exit;
  }


  fec_allocation_deinitialize_unit(unit);
 
  fec_allocation_deinitialize_last();

   LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit,"FEC allocation test done\n")));

exit:

  if(regular_fecs){
      sal_free(regular_fecs);
  }
  if(superfec_base){
      sal_free(superfec_base);
  }
  if(superfec_failover){
      sal_free(superfec_failover);
  }
  if(ecmp) {
      sal_free(ecmp);
  }
  return rc;


}


