/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip linked list methods
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_ap_ll_print
@param:

Print elements from TDM linked list
 */
void
tdm_ap_ll_print(struct ap_ll_node *llist)
{
	int i=0;
	struct ap_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		TDM_PRINT0("\n");
		TDM_PRINT0("\tTDM linked list content: \n\t\t");
		while(list!=NULL) {
			TDM_PRINT1("[%03d]",list->port);
			list=list->next;
			if ((++i)%10==0) {
				TDM_PRINT0("->\n\t\t");
			}
			else {
				TDM_PRINT0("->");
			}
		}
		TDM_PRINT0("[*]\n\n\n");
	}
	else {
		TDM_PRINT0("\n\t\tUnable to parse TDM linked list for print\n\n");
	}
}


/**
@name: tdm_ap_ll_deref
@param:

Dereference entire TDM linked list into unlinked array
 */
void
tdm_ap_ll_deref(struct ap_ll_node *llist, int *tdm[AP_LR_LLS_LEN], int lim)
{
	int i=0;
	struct ap_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		while ( (list!=NULL) && (i<lim) ) {
			(*tdm)[i++]=list->port;
			list=list->next;
		}
	}
	else {
		TDM_PRINT0("\t\tUnable to parse TDM linked list for deref\n\n");
	}
}


/**
@name: tdm_ap_ll_append
@param:

Append an element to TDM linked list
 */
void
tdm_ap_ll_append(struct ap_ll_node *llist, unsigned short port_append, int *pointer)
{
	struct ap_ll_node *mbox,
					*tail = ( (struct ap_ll_node *) llist );
	
	if (llist) {
		while (tail->next!=NULL) {
			tail=tail->next;
		}
	}
	
	if (tail) {
		mbox = ( (struct ap_ll_node *) TDM_ALLOC(sizeof(struct ap_ll_node),"ap_ll_node") );
		mbox->port=port_append;
		mbox->next=NULL;
		tail->next=mbox;
		tail=mbox;
		tail->next=NULL;
	}
	else {
		/* llist=mbox; */
	}
	
	(*pointer)++;
}


/**
@name: tdm_ap_ll_insert
@param:

Insert an element at index to TDM linked list
 */
void
tdm_ap_ll_insert(struct ap_ll_node *llist, unsigned short port_insert, int idx)
{
	int i;
	struct ap_ll_node *mbox,
					*right=llist,
					*left=NULL;
	
	/* Conditions: 1) list header cannot be NULL; 2) valid element index; */
	if ( (llist != NULL)                        &&
		 (idx>=0 && idx<=tdm_ap_ll_len(llist)) ){
		mbox = (struct ap_ll_node *) TDM_ALLOC(sizeof(struct ap_ll_node),"ap_ll_node");
		mbox->port=port_insert;
		mbox->next=NULL;
		
		/* List header is always empty */
		left = right;
		right= right->next;		
		
		/* Insert the element at designated index */
		for (i=0; i<idx; i++){
			left = right;
			right= right->next;
		}
		left->next=mbox;
		left=mbox;
		left->next=right;
	}
}


/**
@name: tdm_ap_ll_delete
@param:

Delete an element at index from TDM linked list
 */
int
tdm_ap_ll_delete(struct ap_ll_node *llist, int idx)
{
	int i;
	struct ap_ll_node *mbox=llist,
					*last=NULL;
	
	/* Conditions: 1) non-empty list; 2) index range within [0,1,2,...,(len-1)] ; */
	if ((tdm_ap_ll_len(llist)>0)             && 
	    (idx>=0 && idx<tdm_ap_ll_len(llist)) ){
		/* List header is always empty */
		last=mbox;
		mbox=mbox->next;
		/* Delete the element at designated index */
		for (i=0; i<idx; i++) {
			last=mbox;
			mbox=mbox->next;
		}
		last->next=mbox->next;
		TDM_FREE(mbox);
		
		return PASS;
	}
	
	return FAIL;
}


/**
@name: tdm_ap_ll_get
@param:

Return content of ap_ll_node at index
 */
int
tdm_ap_ll_get(struct ap_ll_node *llist, int idx)
{
	int i;
	struct ap_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
	}
	
	if (list!=NULL) {
		return (list->port);
	}
	else {
		return AP_NUM_EXT_PORTS;
	}
}


/**
@name: tdm_ap_ll_len
@param:

Return current length of linked list
 */
int
tdm_ap_ll_len(struct ap_ll_node *llist)
{
	int len=0;
	struct ap_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		while ( (list!=NULL) ) {
			len++;
			list=list->next;
		}
	}
	
	return len;
}


/**
@name: tdm_ap_ll_strip
@param:

Strip all of one tokenized value from TDM linked list and updates pool value for that token
 */
void
tdm_ap_ll_strip(struct ap_ll_node *llist, int *pool, int token)
{
	int i;
	struct ap_ll_node *list = llist;

	if ( list!=NULL ) {
		for (i=0; i<tdm_ap_ll_len(list); i++) {
			if ( tdm_ap_ll_get(list,i)==token ) {
				tdm_ap_ll_delete(list,i);
				i--; 
				(*pool)++;
			}
		}
	}
}


/**
@name: tdm_ap_ll_count
@param:

Return count of tokenized port identity within TDM linked list
 */
int
tdm_ap_ll_count(struct ap_ll_node *llist, int token)
{
	int i, count=0;
	struct ap_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		for (i=0; i<tdm_ap_ll_len(list); i++) {
			AP_TOKEN_CHECK(token) {
				if ( (tdm_ap_ll_get(list,i)>0) && (tdm_ap_ll_get(list,i)<73) ) {
					count++;
				}				
			}
			else {
				if (tdm_ap_ll_get(list,i)==token) {
					count++;
				}
			}
		}
	}
	
	return count;
}


/**
@name: tdm_ap_ll_weave
@param:

Based on pool, interspace pooled tokenized ll_nodes to space PGW TDM ports
 */
void
tdm_ap_ll_weave(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int token)
{
	int i, pool=0, skew=0, slices=0, split, count, set=BOOL_FALSE, div, timeout, original_len;
	struct ap_ll_node *list = llist;

	if (list!=NULL && tdm_ap_ll_len(list)>0) {
		original_len=tdm_ap_ll_len(list);
		tdm_ap_ll_strip(list,&pool,token);
		div=pool;
		if ( (pool>0) && ( tdm_ap_ll_get(list,(tdm_ap_ll_len(list)-1))==tdm_ap_ll_get(list,0) ) ) {
			tdm_ap_ll_insert(list,token,(tdm_ap_ll_len(list))); set=BOOL_TRUE;
			pool--; slices++;
		}
		for (i=(tdm_ap_ll_len(list)-1); i>0; i--) {			
			if ( (pool>0) && ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,i),wc_array)==tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,(i-1)),wc_array) ) ) {
				tdm_ap_ll_insert(list,token,i); set=BOOL_TRUE;
				slices++;
				if ((--pool)<=0) {
					break;
				}
			}
		}
		split=(pool>0)?(pool):(1);
		timeout=32000;
		while (pool>0 && (--timeout)>0) {
			count=0;
			if (set) {
				for (i=(1+skew); i<tdm_ap_ll_len(list); i+=(((slices/split)>0)?(slices/split):(1))) {
					if (tdm_ap_ll_get(list,i)==token) {
						if (++count==(slices/split)) {
							tdm_ap_ll_insert(list,token,i);
							i++; count=0;
							if ((--pool)<=0) {
								break;
							}
						}
					}
				}
				skew++;
			}
			else {
				for (i=((original_len/div)-1); i<original_len; i+=(original_len/div)) {
					tdm_ap_ll_insert(list,token,i);
					if ((--pool)<=0) {
						break;
					}
				}
			}
		}
		if ( tdm_ap_ll_len(list) < original_len ) {
			tdm_ap_ll_insert(list,token,tdm_ap_ll_len(list));
		}
		timeout=32000;
		while ( (tdm_ap_ll_len(list) < original_len) && ((--timeout)>0) ) {
			for (i=(tdm_ap_ll_len(list)-2); i>0; i--) {
				if ( (tdm_ap_ll_get(list,(i-1))!=token) && (tdm_ap_ll_get(list,(i+1))!=token) && (tdm_ap_ll_get(list,i)!=token) ) {
					tdm_ap_ll_insert(list,token,i);
					break;
				}
			}
		}
		timeout=32000;
		while ( (tdm_ap_ll_len(list) < original_len) && ((--timeout)>0) ) {
			for (i=1; i<(tdm_ap_ll_len(list)-1); i++) {
				if ( (tdm_ap_ll_get(list,(i-1))!=token) && (tdm_ap_ll_get(list,i)!=token) ) {
					tdm_ap_ll_insert(list,token,i);
					break;
				}
			}
		}
		while ( tdm_ap_ll_len(list) < original_len ) {
			tdm_ap_ll_insert(list,token,tdm_ap_ll_len(list));
		}
	}
}


/**
@name: tdm_ap_ll_retrace
@param:

Repoint linked list ll_nodes if they violate PGW TDM min spacing
 */
void
tdm_ap_ll_retrace(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS])
{
	int i, j, port, done=BOOL_FALSE, timeout=32000;
	struct ap_ll_node *list = llist;
	
	while((--timeout)>0) {
		for (i=1; i<tdm_ap_ll_len(list); i++) {
			done=BOOL_FALSE;
			if (tdm_ap_ll_get(list,i)!=AP_OVSB_TOKEN && tdm_ap_ll_get(list,(i-1))!=AP_OVSB_TOKEN) {
				if ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,i),wc_array) ==tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,(i-1)),wc_array) ) {
					port=tdm_ap_ll_get(list,i);
					for (j=1; j<tdm_ap_ll_len(list); j++) {
						if ( ( tdm_ap_scan_which_tsc(port,wc_array) != tdm_ap_scan_which_tsc(tdm_ap_ll_get(list, j   ),wc_array) ) &&
							 ( tdm_ap_scan_which_tsc(port,wc_array) != tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,(j-1)),wc_array) ) ) {
							if(i<j){
								tdm_ap_ll_insert(list,port,j);
								tdm_ap_ll_delete(list,i);
							}
							else{
								tdm_ap_ll_delete(list,i);
								tdm_ap_ll_insert(list,port,j);
							}
							done=BOOL_TRUE;
							break;
						}
					}
				}
			}
			if (done) {
				break;
			}
		}
		if (!done) {
			break;
		}
	}
}


/**
@name: tdm_ap_ll_single_100
@param:

Return boolean on whether only 1 100G+ port exists within TDM linked list
 */
int
tdm_ap_ll_single_100(struct ap_ll_node *llist)
{
	int i, typing=BOOL_TRUE, port, count=1;
	struct ap_ll_node *list = llist;
	
	if (list!=NULL) {
		port=tdm_ap_ll_get(list,0);
		for (i=1; i<tdm_ap_ll_len(list); i++) {
			AP_TOKEN_CHECK(tdm_ap_ll_get(list,i)) {
				if (tdm_ap_ll_get(list,i)!=port) {
					typing=BOOL_FALSE;
					break;
				}
				else {
					count++;
				}
			}
		}
		if (count<20) {
			typing=BOOL_FALSE;
		}
	}

	return typing;
	
}


/**
@name: tdm_ap_ll_retrace_25
@param:
 */

void
tdm_ap_ll_retrace_25(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int port[2], enum port_speed_e speed[2] )
{
   int i, j, k, div, pool=0, set=BOOL_FALSE, original_len, timeout=32000;
   struct ap_ll_node *list = llist;
   int cl_token=0, temp_port=0;

if (speed[0] == SPEED_25G || speed[1]== SPEED_25G) {
  if ((speed[0] == SPEED_25G) && (speed[1]== SPEED_25G)) {
      for (i=0; i<tdm_ap_ll_len(list); i++) {
        for (k=0; k<4; k++) {
          if(tdm_ap_ll_get(list,i)== (port[1]+k)  ) {
           tdm_ap_ll_delete(list,i);
           tdm_ap_ll_insert(list,port[1],i);
          }
        }
      }
   cl_token=port[0];
   temp_port=cl_token;
  }
  else if(speed[0] == SPEED_25G) { 
   cl_token=port[0];
   temp_port=cl_token;
  }
  else if(speed[1] == SPEED_25G) { 
   cl_token=port[1];
   temp_port=cl_token;
  }

   
     original_len=tdm_ap_ll_len(list);
     for(j=0;j<4;j++) {
     tdm_ap_ll_strip(list,&pool,temp_port++);
      }
     div=pool;
     tdm_ap_ll_print(list);

		if ( (pool>0) && ( tdm_ap_ll_get(list,(tdm_ap_ll_len(list)-1))==tdm_ap_ll_get(list,0) ) ) {
			tdm_ap_ll_insert(list,cl_token,(tdm_ap_ll_len(list))); set=BOOL_TRUE;
		}
		for (i=(tdm_ap_ll_len(list)-1); i>0; i--) {			
			if ( (pool>0) && ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,i),wc_array)==tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,(i-1)),wc_array) ) ) {
				tdm_ap_ll_insert(list,cl_token,i); set=BOOL_TRUE;
				if ((--pool)<=0) {
					break;
				}
			}
		}
     tdm_ap_ll_print(list);

		 if ( (pool>0) && (tdm_ap_ll_get(list,(tdm_ap_ll_len(list)-1))!=cl_token) && ((tdm_ap_ll_get(list,0))!=cl_token) && (tdm_ap_ll_get(list,1)!=cl_token) ) {
			tdm_ap_ll_insert(list,cl_token,(tdm_ap_ll_len(list)));
					pool--;
		 }

    while (pool>0 && (--timeout)>0) {
      if (set) {
   		  for (i=(tdm_ap_ll_len(list)-2); i>0; i--) {
   				if ( (tdm_ap_ll_get(list,(i-1))!=cl_token) && (tdm_ap_ll_get(list,(i+1))!=cl_token) && (tdm_ap_ll_get(list,i)!=cl_token) ) {
   					tdm_ap_ll_insert(list,cl_token,i);
   			    if ((--pool)<=0) {
   					  break;
   				  } 
   			  }
   		  }
      }
      else {
      	for (i=((original_len/div)-1); i<original_len;) {
   			  tdm_ap_ll_insert(list,cl_token,i);
   		    if ((--pool)<=0) {
   			     break;
   		    }
           if(i<= original_len/2) { i+=((original_len/div)+1);}
           else { i+=(original_len/div);} 
   	    }
      }
    }

		 while ( tdm_ap_ll_len(list) < original_len ) {
			tdm_ap_ll_insert(list,cl_token,tdm_ap_ll_len(list));
		 }

     if ((speed[0] == SPEED_25G) && (speed[1]== SPEED_25G)) {
      tdm_ap_ll_append_25(list,port[1], wc_array[port[1]/4]);
     }
      tdm_ap_ll_append_25(list,cl_token, wc_array[cl_token/4]);
   }

 }


/**
@name: tdm_ap_ll_remove_25
@param:
 */

void
tdm_ap_ll_remove_25(struct ap_ll_node *llist, int port)
{
	int i, cnt=0;
	struct ap_ll_node *list = llist;
   for (i=0; i<tdm_ap_ll_len(list); i++) {
	   if(tdm_ap_ll_get(list,i)== port ) {
        tdm_ap_ll_delete(list,i);
        cnt++;
      }
     if (cnt==4) break;
   }
 }

/**
@name: tdm_ap_ll_append_25
@param:
 */

void
tdm_ap_ll_append_25(struct ap_ll_node *llist, int token, int wc_array[AP_NUM_PM_LNS])
{
	int i;
	struct ap_ll_node *list = llist;
  int port=token;
 
  for (i=0; i<tdm_ap_ll_len(list); i++) {
     if(tdm_ap_ll_get(list,i)== token) {
       tdm_ap_ll_delete(list,i);
       while (wc_array[(port-1)%4]==AP_NUM_EXT_PORTS) port = ((port+1) > (token+3)) ? token : port+1; 
       tdm_ap_ll_insert(list,port++,i); 
       if (port>(token+3)) port = token;       
     }
   }
 }

/**
@name: tdm_ap_ll_retrace_8x25
@param:
 */

void
tdm_ap_ll_reconfig_8x25(struct ap_ll_node *llist, int token)
{
	int i, k;
	struct ap_ll_node *list = llist;
  if (list!=NULL && tdm_ap_ll_len(list)>0) {
    /*tdm_ap_ll_remove_25(list,token);*/
     for (i=0; i<tdm_ap_ll_len(list); i++) {
       for (k=0; k<4; k++) {
         if(tdm_ap_ll_get(list,i)== (token+k)  ) {
          tdm_ap_ll_delete(list,i);
          tdm_ap_ll_insert(list,token,i);
        }
      }
    }
  }
}

void
tdm_ap_ll_weave_100(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int token)
{
	int i, pool=0, div, set=BOOL_FALSE, timeout, original_len;
	struct ap_ll_node *list = llist;

	if (list!=NULL && tdm_ap_ll_len(list)>0) {
		original_len=tdm_ap_ll_len(list);
		tdm_ap_ll_strip(list,&pool,token);
		div=pool;
		timeout=32000;

			if ( (pool>0) && ( tdm_ap_ll_get(list,(tdm_ap_ll_len(list)-1))==tdm_ap_ll_get(list,0) ) ) {
			tdm_ap_ll_insert(list,token,(tdm_ap_ll_len(list))); set=BOOL_TRUE;
		}
		for (i=(tdm_ap_ll_len(list)-1); i>0; i--) {			
			if ( (pool>0) && ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,i),wc_array)==tdm_ap_scan_which_tsc(tdm_ap_ll_get(list,(i-1)),wc_array) ) ) {
				tdm_ap_ll_insert(list,token,i); set=BOOL_TRUE;
				if ((--pool)<=0) {
					break;
				}
			}
		}
     tdm_ap_ll_print(list);

		 if ( (pool>0) && (tdm_ap_ll_get(list,(tdm_ap_ll_len(list)-1))!=token) && ((tdm_ap_ll_get(list,0))!=token) && (tdm_ap_ll_get(list,1)!=token) ) {
			tdm_ap_ll_insert(list,token,(tdm_ap_ll_len(list)));
					pool--;
		 }

    while (pool>0 && (--timeout)>0) {
      if (set) {
   		  for (i=(tdm_ap_ll_len(list)-2); i>0; i--) {
   				if ( (tdm_ap_ll_get(list,(i-1))!=token) && (tdm_ap_ll_get(list,(i+1))!=token) && (tdm_ap_ll_get(list,i)!=token) ) {
   					tdm_ap_ll_insert(list,token,i);
   			    if ((--pool)<=0) {
   					  break;
   				  } 
   			  }
   		  }
      }
      else {
      	for (i=((original_len/div)-1); i<original_len;) {
   			  tdm_ap_ll_insert(list,token,i);
   		    if ((--pool)<=0) {
   			     break;
   		    }
           if(i<= original_len/2) { i+=((original_len/div)+1);}
           else { i+=(original_len/div);} 
   	    }
      }
    }

		 while ( tdm_ap_ll_len(list) < original_len ) {
			tdm_ap_ll_insert(list,token,tdm_ap_ll_len(list));
		 }
  }
}

/**
@name: tdm_ap_ll_free
@param:

Free memory space allocated to linked-list
 */
int
tdm_ap_ll_free(struct ap_ll_node *llist)
{
        struct ap_ll_node *temp, *list = llist;
        if(list!=NULL){
                list = list->next;
                while(list!=NULL ){
                        temp = list;
                        list = list->next;
                        TDM_FREE(temp);
                }
        }
        llist->next = NULL;

        return PASS;
}


void
tdm_ap_ll_retrace_cl(struct ap_ll_node *llist,  tdm_ap_chip_legacy_t *ap_chip, int port[2], enum port_speed_e speed[2])
{
    int i, j, avg=0, token, pos=0, len;
    struct ap_ll_node *list = llist;

    if (list!=NULL && tdm_ap_ll_len(list)>0) {
    len=tdm_ap_ll_len(list);
/*average out the spacing of 10G ports for cut thru support*/
    for( i = 0 ; i<len; i++) {
      if(((*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] == SPEED_100G ||
         (*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] == SPEED_50G ||
         (*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] == SPEED_25G )
        ) {
          pos=0;
          token=0;
       if((tdm_ap_ll_get(list,i)<29  || (tdm_ap_ll_get(list,i)>36 && tdm_ap_ll_get(list,i)<65 )) &&
         ((*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] >= SPEED_10G && (*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] <= SPEED_40G)) {
         token=tdm_ap_ll_get(list,i);
         avg = ((*ap_chip).tdm_globals.speed[token]== SPEED_40G) ? (len/8) :
                 ((*ap_chip).tdm_globals.speed[token]== SPEED_25G) ? (len/5) :
                 ((*ap_chip).tdm_globals.speed[token]== SPEED_20G) ? (len/4) : (len/2);
       for (j=i+1; j<len; j++) {
         if (tdm_ap_ll_get(list,j)==token) {
           pos=j-i;
           break;
         }
       }
       if(pos > avg){
         if((i+avg) < len ){
         tdm_ap_ll_delete(list,j);
         tdm_ap_ll_insert(list,token,(i+avg));
         }
         else {
         tdm_ap_ll_delete(list,j); 
         tdm_ap_ll_insert(list,token,(i+avg)%len);
         }
       }
       else if(pos < avg && pos !=0){
         if((i+avg) < len ){
         tdm_ap_ll_delete(list,j);
         tdm_ap_ll_insert(list,token,(i+avg));
         }
         else {
         tdm_ap_ll_delete(list,j); 
         tdm_ap_ll_insert(list,token,(i+avg)%len);
         }
       }
       }
     }
    }

/**/

   if (speed[0] == SPEED_50G) {
      for (i=0; i<len; i++) {
          if(tdm_ap_ll_get(list,i)== (port[0]+2) ) {
           tdm_ap_ll_delete(list,i);
           tdm_ap_ll_insert(list,port[0],i);
          }
      }
   }

   if (speed[1]== SPEED_50G) {
      for (i=0; i<len; i++) {
          if(tdm_ap_ll_get(list,i)== (port[1]+2) ) {
           tdm_ap_ll_delete(list,i);
           tdm_ap_ll_insert(list,port[1],i);
          }
      }
   }

    for( i = 0 ; i<len; i++) {
      if(((tdm_ap_ll_get(list,i)>=29  && tdm_ap_ll_get(list,i)<=35) || (tdm_ap_ll_get(list,i)>=65  && tdm_ap_ll_get(list,i)<=71)) &&
      ((*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] == SPEED_40G || (*ap_chip).tdm_globals.speed[tdm_ap_ll_get(list,i)] == SPEED_20G)) {
        if(tdm_ap_ll_get(list,i)==tdm_ap_ll_get(list,(i+1))) {
          token=tdm_ap_ll_get(list,i);
          avg = ((*ap_chip).tdm_globals.speed[token]== SPEED_40G) ? (len/8) : (len/4);
          if( (i+avg) <len ){
          tdm_ap_ll_delete(list,i);
          tdm_ap_ll_insert(list,token,(i+avg));
          }
          else {
          tdm_ap_ll_delete(list,i); 
          tdm_ap_ll_insert(list,token,(i+avg)%len);
          }
        }
      }
    }
  }
}
