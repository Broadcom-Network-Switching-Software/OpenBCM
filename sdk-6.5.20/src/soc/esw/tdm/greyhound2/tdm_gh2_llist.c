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
@name: tdm_gh2_ll_print
@param:

Print elements from TDM linked list
 */
void
tdm_gh2_ll_print(struct gh2_ll_node *llist)
{
	int i=0;
	struct gh2_ll_node *list = llist;
	
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
@name: tdm_gh2_ll_deref
@param:

Dereference entire TDM linked list into unlinked array
 */
void
tdm_gh2_ll_deref(struct gh2_ll_node *llist, int *tdm[GH2_LR_VBS_LEN], int lim)
{
	int i=0;
	struct gh2_ll_node *list = llist;
	
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
@name: tdm_gh2_ll_append
@param:

Append an element to TDM linked list
 */
void
tdm_gh2_ll_append(struct gh2_ll_node *llist, unsigned short port_append, int *pointer)
{
	struct gh2_ll_node *mbox,
					*tail = ( (struct gh2_ll_node *) llist );
	
	if (llist) {
		while (tail->next!=NULL) {
			tail=tail->next;
		}
	}
	
	if (tail) {
		mbox = ( (struct gh2_ll_node *) TDM_ALLOC(sizeof(struct gh2_ll_node),"gh2_ll_node") );
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
@name: tdm_gh2_ll_insert
@param:

Insert an element at index to TDM linked list
 */
void
tdm_gh2_ll_insert(struct gh2_ll_node *llist, unsigned short port_insert, int idx)
{
	int i;
	struct gh2_ll_node *mbox,
					*right=llist,
					*left=NULL;
	
	/* Conditions: 1) list header cannot be NULL; 2) valid element index; */
	if ( (llist != NULL)                        &&
		 (idx>=0 && idx<=tdm_gh2_ll_len(llist)) ){
		mbox = (struct gh2_ll_node *) TDM_ALLOC(sizeof(struct gh2_ll_node),"gh2_ll_node");
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
@name: tdm_gh2_ll_delete
@param:

Delete an element at index from TDM linked list
 */
int
tdm_gh2_ll_delete(struct gh2_ll_node *llist, int idx)
{
	int i;
	struct gh2_ll_node *mbox=llist,
					*last=NULL;
	
	/* Conditions: 1) non-empty list; 2) index range within [0,1,2,...,(len-1)] ; */
	if ((tdm_gh2_ll_len(llist)>0)             && 
	    (idx>=0 && idx<tdm_gh2_ll_len(llist)) ){
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
@name: tdm_gh2_ll_get
@param:

Return content of gh2_ll_node at index
 */
int
tdm_gh2_ll_get(struct gh2_ll_node *llist, int idx)
{
	int i;
	struct gh2_ll_node *list = llist;
	
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
		return GH2_NUM_EXT_PORTS;
	}
}


/**
@name: tdm_gh2_ll_len
@param:

Return current length of linked list
 */
int
tdm_gh2_ll_len(struct gh2_ll_node *llist)
{
	int len=0;
	struct gh2_ll_node *list = llist;
	
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
@name: tdm_gh2_ll_strip
@param:

Strip all of one tokenized value from TDM linked list and updates pool value for that token
 */
void
tdm_gh2_ll_strip(struct gh2_ll_node *llist, int *pool, int token)
{
	int i;
	struct gh2_ll_node *list = llist;

	if ( list!=NULL ) {
		for (i=0; i<tdm_gh2_ll_len(list); i++) {
			if ( tdm_gh2_ll_get(list,i)==token ) {
				tdm_gh2_ll_delete(list,i);
				i--; 
				(*pool)++;
			}
		}
	}
}


/**
@name: tdm_gh2_ll_count
@param:

Return count of tokenized port identity within TDM linked list
 */
int
tdm_gh2_ll_count(struct gh2_ll_node *llist, int token)
{
	int i, count=0;
	struct gh2_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		for (i=0; i<tdm_gh2_ll_len(list); i++) {
			GH2_TOKEN_CHECK(token) {
				if ( (tdm_gh2_ll_get(list,i)>0) && (tdm_gh2_ll_get(list,i)<129) ) {
					count++;
				}				
			}
			else {
				if (tdm_gh2_ll_get(list,i)==token) {
					count++;
				}
			}
		}
	}
	
	return count;
}


/**
@name: tdm_gh2_ll_weave
@param:

Based on pool, interspace pooled tokenized ll_nodes to space PGW TDM ports
 */
void
tdm_gh2_ll_weave(struct gh2_ll_node *llist, int token, tdm_mod_t *_tdm)
{
	int i, pool=0, skew=0, slices=0, split, count, set=BOOL_FALSE, div, timeout, original_len;
	struct gh2_ll_node *list = llist;

	if (list!=NULL && tdm_gh2_ll_len(list)>0) {
		original_len=tdm_gh2_ll_len(list);
		tdm_gh2_ll_strip(list,&pool,token);
		div=pool;
		if ( (pool>0) && ( tdm_gh2_ll_get(list,(tdm_gh2_ll_len(list)-1))==tdm_gh2_ll_get(list,0) ) ) {
			tdm_gh2_ll_insert(list,token,(tdm_gh2_ll_len(list))); set=BOOL_TRUE;
			pool--; slices++;
		}
		for (i=(tdm_gh2_ll_len(list)-1); i>0; i--) {			
			if ( (pool>0) && ( tdm_gh2_scan_which_tsc(tdm_gh2_ll_get(list,i),_tdm) == tdm_gh2_scan_which_tsc(tdm_gh2_ll_get(list,(i-1)),_tdm) ) ) {
				tdm_gh2_ll_insert(list,token,i); set=BOOL_TRUE;
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
				for (i=(1+skew); i<tdm_gh2_ll_len(list); i+=(((slices/split)>0)?(slices/split):(1))) {
					if (tdm_gh2_ll_get(list,i)==token) {
						if (++count==(slices/split)) {
							tdm_gh2_ll_insert(list,token,i);
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
					tdm_gh2_ll_insert(list,token,i);
					if ((--pool)<=0) {
						break;
					}
				}
			}
		}
		if ( tdm_gh2_ll_len(list) < original_len ) {
			tdm_gh2_ll_insert(list,token,tdm_gh2_ll_len(list));
		}
		timeout=32000;
		while ( (tdm_gh2_ll_len(list) < original_len) && ((--timeout)>0) ) {
			for (i=(tdm_gh2_ll_len(list)-2); i>0; i--) {
				if ( (tdm_gh2_ll_get(list,(i-1))!=token) && (tdm_gh2_ll_get(list,(i+1))!=token) && (tdm_gh2_ll_get(list,i)!=token) ) {
					tdm_gh2_ll_insert(list,token,i);
					break;
				}
			}
		}
		timeout=32000;
		while ( (tdm_gh2_ll_len(list) < original_len) && ((--timeout)>0) ) {
			for (i=1; i<(tdm_gh2_ll_len(list)-1); i++) {
				if ( (tdm_gh2_ll_get(list,(i-1))!=token) && (tdm_gh2_ll_get(list,i)!=token) ) {
					tdm_gh2_ll_insert(list,token,i);
					break;
				}
			}
		}
		while ( tdm_gh2_ll_len(list) < original_len ) {
			tdm_gh2_ll_insert(list,token,tdm_gh2_ll_len(list));
		}
	}
}


/**
@name: tdm_gh2_ll_retrace
@param:

Repoint linked list ll_nodes if they violate PGW TDM min spacing
 */
void
tdm_gh2_ll_retrace(struct gh2_ll_node *llist, tdm_mod_t *_tdm)
{
	int i, j, port, done=BOOL_FALSE, timeout=32000;
	struct gh2_ll_node *list = llist;
	
	while((--timeout)>0) {
		for (i=1; i<tdm_gh2_ll_len(list); i++) {
			done=BOOL_FALSE;
			if (tdm_gh2_ll_get(list,i)!=GH2_OVSB_TOKEN && tdm_gh2_ll_get(list,(i-1))!=GH2_OVSB_TOKEN) {
				if ( tdm_gh2_scan_which_tsc(tdm_gh2_ll_get(list,i),_tdm) == tdm_gh2_scan_which_tsc(tdm_gh2_ll_get(list,(i-1)),_tdm) ) {
					port=tdm_gh2_ll_get(list,i);
					for (j=1; j<tdm_gh2_ll_len(list); j++) {
						if ( ( tdm_gh2_scan_which_tsc(port,_tdm) != tdm_gh2_scan_which_tsc(tdm_gh2_ll_get(list, j   ),_tdm) ) &&
							 ( tdm_gh2_scan_which_tsc(port,_tdm) != tdm_gh2_scan_which_tsc(tdm_gh2_ll_get(list,(j-1)),_tdm) ) ){
							if(i<j){
								tdm_gh2_ll_insert(list,port,j);
								tdm_gh2_ll_delete(list,i);
							}
							else{
								tdm_gh2_ll_delete(list,i);
								tdm_gh2_ll_insert(list,port,j);
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
@name: tdm_gh2_ll_single_100
@param:

Return boolean on whether only 1 100G+ port exists within TDM linked list
 */
int
tdm_gh2_ll_single_100(struct gh2_ll_node *llist)
{
	int i, typing=BOOL_TRUE, port, count=1;
	struct gh2_ll_node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		port=tdm_gh2_ll_get(list,0);
		for (i=1; i<tdm_gh2_ll_len(list); i++) {
			GH2_TOKEN_CHECK(tdm_gh2_ll_get(list,i)) {
				if (tdm_gh2_ll_get(list,i)!=port) {
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
@name: tdm_gh2_ll_free
@param:

Free memory space allocated to linked-list
 */
int
tdm_gh2_ll_free(struct gh2_ll_node *llist)
{
	struct gh2_ll_node *temp, *list = llist;
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
