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
@name: tdm_td2p_ll_print
@param:

Print elements from TDM linked list
 */
void
tdm_td2p_ll_print(struct ll_node *llist)
{
	int i=0;
	struct ll_node *list = llist;
	
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
@name: tdm_td2p_ll_deref
@param:

Dereference entire TDM linked list into unlinked array
 */
void
tdm_td2p_ll_deref(struct ll_node *llist, int *tdm[TD2P_LR_LLS_LEN], int lim)
{
	int i=0;
	struct ll_node *list = llist;
	
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
@name: tdm_td2p_ll_append
@param:

Append an element to TDM linked list
 */
void
tdm_td2p_ll_append(struct ll_node *llist, unsigned short port_append, int *pointer)
{
	struct ll_node *mbox,
					*tail = ( (struct ll_node *) llist );
	
	if (llist) {
		while (tail->next!=NULL) {
			tail=tail->next;
		}
	}
	
	if (tail) {
		mbox = ( (struct ll_node *) TDM_ALLOC(sizeof(struct ll_node),"ll_node") );
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
@name: tdm_td2p_ll_insert
@param:

Insert an element at index to TDM linked list
 */
void
tdm_td2p_ll_insert(struct ll_node *llist, unsigned short port_insert, int idx)
{
	int i;
	struct ll_node *mbox,
					*right=llist,
					*left=NULL;
	
	/* Conditions: 1) list header cannot be NULL; 2) valid element index; */
	if ( (llist != NULL)                        &&
		 (idx>=0 && idx<=tdm_td2p_ll_len(llist)) ){
		mbox = (struct ll_node *) TDM_ALLOC(sizeof(struct ll_node),"ll_node");
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
@name: tdm_td2p_ll_delete
@param:

Delete an element at index from TDM linked list
 */
int
tdm_td2p_ll_delete(struct ll_node *llist, int idx)
{
	int i;
	struct ll_node *mbox=llist,
					*last=NULL;
	
	/* Conditions: 1) non-empty list; 2) index range within [0,1,2,...,(len-1)] ; */
	if ((tdm_td2p_ll_len(llist)>0)             && 
	    (idx>=0 && idx<tdm_td2p_ll_len(llist)) ){
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
@name: tdm_td2p_ll_get
@param:

Return content of ll_node at index
 */
int
tdm_td2p_ll_get(struct ll_node *llist, int idx)
{
	int i;
	struct ll_node *list = llist;
	
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
		return TD2P_NUM_EXT_PORTS;
	}
}


/**
@name: tdm_td2p_ll_len
@param:

Return current length of linked list
 */
int
tdm_td2p_ll_len(struct ll_node *llist)
{
	int len=0;
	struct ll_node *list = llist;
	
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
@name: tdm_td2p_ll_strip
@param:

Strip all of one tokenized value from TDM linked list and updates pool value for that token
 */
void
tdm_td2p_ll_strip(struct ll_node *llist, int *pool, int token)
{
	int i;
	struct ll_node *list = llist;

	if ( list!=NULL ) {
		for (i=0; i<tdm_td2p_ll_len(list); i++) {
			if ( tdm_td2p_ll_get(list,i)==token ) {
				tdm_td2p_ll_delete(list,i);
				i--; 
				(*pool)++;
			}
		}
	}
}


/**
@name: tdm_td2p_ll_count
@param:

Return count of tokenized port identity within TDM linked list
 */
int
tdm_td2p_ll_count(struct ll_node *llist, int token)
{
	int i, count=0;
	struct ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		for (i=0; i<tdm_td2p_ll_len(list); i++) {
			TD2P_TOKEN_CHECK(token) {
				if ( (tdm_td2p_ll_get(list,i)>0) && (tdm_td2p_ll_get(list,i)<129) ) {
					count++;
				}				
			}
			else {
				if (tdm_td2p_ll_get(list,i)==token) {
					count++;
				}
			}
		}
	}
	
	return count;
}


/**
@name: tdm_td2p_ll_weave
@param:

Based on pool, interspace pooled tokenized ll_nodes to space PGW TDM ports
 */
void
tdm_td2p_ll_weave(struct ll_node *llist, int wc_array[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS], int token)
{
	int i, pool=0, skew=0, slices=0, split, count, set=BOOL_FALSE, div,
        timeout, original_len, clump;
	int x, y, tsc_x, tsc_y;
	struct ll_node *list = llist;

	if (list!=NULL && tdm_td2p_ll_len(list)>0) {
		original_len=tdm_td2p_ll_len(list);
		tdm_td2p_ll_strip(list,&pool,token);
		div=pool;
        for (i=tdm_td2p_ll_len(list); i>0; i--) {
            x = (i==tdm_td2p_ll_len(list)) ? (0) : (i);
            y = i-1;
            tsc_x = tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(list,x),wc_array);
            tsc_y = tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(list,y),wc_array);
            if ( (pool>0) && ( tsc_x == tsc_y) ) {
                tdm_td2p_ll_insert(list,token,i); set=BOOL_TRUE;
                slices++;
                if ((--pool)<=0) {
                    break;
                }
            }
        }

		split=(pool>0)?(pool):(1);
        clump = ((slices/split)>0)?(slices/split):(1);
		timeout=32000;
		while (pool>0 && (--timeout)>0) {
			count=0;
			if (set) {
                for (i=tdm_td2p_ll_len(list)-1; i>0; i-=clump) {
					if (tdm_td2p_ll_get(list,i)==token) {
						if (++count==(clump+skew)) {
							tdm_td2p_ll_insert(list,token,i);
							/* i--; */ count=0;
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
					tdm_td2p_ll_insert(list,token,i);
					if ((--pool)<=0) {
						break;
					}
				}
			}
		}
		if ( tdm_td2p_ll_len(list) < original_len ) {
			tdm_td2p_ll_insert(list,token,tdm_td2p_ll_len(list));
		}
		timeout=32000;
		while ( (tdm_td2p_ll_len(list) < original_len) && ((--timeout)>0) ) {
			for (i=(tdm_td2p_ll_len(list)-2); i>0; i--) {
				if ( (tdm_td2p_ll_get(list,(i-1))!=token) && (tdm_td2p_ll_get(list,(i+1))!=token) && (tdm_td2p_ll_get(list,i)!=token) ) {
					tdm_td2p_ll_insert(list,token,i);
					break;
				}
			}
		}
		timeout=32000;
		while ( (tdm_td2p_ll_len(list) < original_len) && ((--timeout)>0) ) {
			for (i=1; i<(tdm_td2p_ll_len(list)-1); i++) {
				if ( (tdm_td2p_ll_get(list,(i-1))!=token) && (tdm_td2p_ll_get(list,i)!=token) ) {
					tdm_td2p_ll_insert(list,token,i);
					break;
				}
			}
		}
		while ( tdm_td2p_ll_len(list) < original_len ) {
			tdm_td2p_ll_insert(list,token,tdm_td2p_ll_len(list));
		}
        for (i=0; (i+3)<tdm_td2p_ll_len(list); i++) {
            if (tdm_td2p_ll_get(list,i)  !=token &&
                tdm_td2p_ll_get(list,i+1)!=token &&
                tdm_td2p_ll_get(list,i+2)==token &&
                tdm_td2p_ll_get(list,i+3)==token ){
                tdm_td2p_ll_insert(list,token,i+1);
                tdm_td2p_ll_delete(list,i+3);
                i += 3;
            }
        }
        for (i=0; (i+3)<tdm_td2p_ll_len(list); i++) {
            if (tdm_td2p_ll_get(list,i)  ==token &&
                tdm_td2p_ll_get(list,i+1)==token &&
                tdm_td2p_ll_get(list,i+2)!=token &&
                tdm_td2p_ll_get(list,i+3)!=token ){
                tdm_td2p_ll_insert(list,token,i+3);
                tdm_td2p_ll_delete(list,i+1);
                i += 3;
            }
        }
	}
}


/**
@name: tdm_td2p_ll_retrace
@param:

Repoint linked list ll_nodes if they violate PGW TDM min spacing
 */
void
tdm_td2p_ll_retrace(struct ll_node *llist, int wc_array[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS])
{
	int i, j, port, done=BOOL_FALSE, timeout=32000;
    int ll_len, ovsb_i, ovsb_max, ovsb_min, ovsb_max_idx, ovsb_min_idx;
	struct ll_node *list = llist;
	
    ll_len = tdm_td2p_ll_len(list);
    
	while((--timeout)>0) {
		for (i=1; i<tdm_td2p_ll_len(list); i++) {
			done=BOOL_FALSE;
			if (tdm_td2p_ll_get(list,i)!=TD2P_OVSB_TOKEN && tdm_td2p_ll_get(list,(i-1))!=TD2P_OVSB_TOKEN) {
				if ( tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(list,i),wc_array) == tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(list,(i-1)),wc_array) ) {
					port=tdm_td2p_ll_get(list,i);
					for (j=1; j<tdm_td2p_ll_len(list); j++) {
						if ( ( tdm_td2p_scan_which_tsc(port,wc_array) != tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(list, j   ),wc_array) ) &&
							 ( tdm_td2p_scan_which_tsc(port,wc_array) != tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(list,(j-1)),wc_array) ) ){
							if(i<j){
								tdm_td2p_ll_insert(list,port,j);
								tdm_td2p_ll_delete(list,i);
							}
							else{
								tdm_td2p_ll_delete(list,i);
								tdm_td2p_ll_insert(list,port,j);
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
    timeout=ll_len;
    while((--timeout)>0) {
        done     = BOOL_FALSE;
        ovsb_i   = 0;
        ovsb_max = 0; 
        ovsb_min = ll_len;
        ovsb_max_idx = ll_len;
        ovsb_min_idx = ll_len;
        for (i=0; i<ll_len; i++){
            port = tdm_td2p_ll_get(list,i);
            if (port==TD2P_OVSB_TOKEN){
                ovsb_i++;
            }
            if ((port!=TD2P_OVSB_TOKEN) ||
                (port==TD2P_OVSB_TOKEN && i==(ll_len-1)) ){
                if (ovsb_i>0){
                    if (ovsb_i>ovsb_max){
                        ovsb_max = ovsb_i;
                        ovsb_max_idx = i-1;
                    }
                    if (ovsb_i<ovsb_min){
                        ovsb_min = ovsb_i;
                        ovsb_min_idx = i-1;
                    }
                }
                ovsb_i = 0;
            }
        }
        if (ovsb_max>(ovsb_min+1) &&
            (ovsb_max_idx<ll_len && ovsb_min_idx<ll_len) ){
            port = TD2P_OVSB_TOKEN;
            if(ovsb_max_idx<ovsb_min_idx){
                tdm_td2p_ll_insert(list,port,ovsb_min_idx);
                tdm_td2p_ll_delete(list,ovsb_max_idx);
            }
            else{
                tdm_td2p_ll_delete(list,ovsb_max_idx);
                tdm_td2p_ll_insert(list,port,ovsb_min_idx);
            }
            done = BOOL_TRUE;
        }
        if (done==BOOL_FALSE){
            break;
        }
    }
}


/**
@name: tdm_td2p_ll_single_100
@param:

Return boolean on whether only 1 100G+ port exists within TDM linked list
 */
int
tdm_td2p_ll_single_100(struct ll_node *llist)
{
	int i, typing=BOOL_TRUE, port, count=1;
	struct ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		port=tdm_td2p_ll_get(list,0);
		for (i=1; i<tdm_td2p_ll_len(list); i++) {
			TD2P_TOKEN_CHECK(tdm_td2p_ll_get(list,i)) {
				if (tdm_td2p_ll_get(list,i)!=port) {
					typing=BOOL_FALSE;
					break;
				}
				else {
					count++;
				}
			}
		}
		if (count<10) {
			typing=BOOL_FALSE;
		}
	}

	return typing;
	
}


/**
@name: tdm_td2p_ll_free
@param:

Free memory space allocated to linked-list
 */
int
tdm_td2p_ll_free(struct ll_node *llist)
{
	struct ll_node *temp, *list = llist;
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
