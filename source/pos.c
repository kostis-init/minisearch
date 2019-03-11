#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "defs.h"

void PRecDel(struct PosNode* node)
{
	if(node->next!=NULL)
	{
		PRecDel(node->next);
	}
	free(node);
	
}

void PosDelete(struct PosList* pos)
{
	PRecDel(pos->first);
	free(pos);
}

int PosInsert(struct PosList* pos,int docid)
{
	struct PosNode* cur=pos->first;
	struct PosNode* previous=NULL;
	while(cur!=NULL)
	{
		if(docid==cur->docid)//found
		{
			cur->tf++;
			//printf("ALREADY HERE tf=%d ",cur->tf);
			break;
		}
		previous=cur;//useful later, if docid not founds
		cur=cur->next;
	}
	if(cur==NULL)///not found, add the docid, df++
	{
		cur=previous;///go back to add new node
		if((cur->next=malloc(sizeof(struct PosNode)))==NULL)
		{
			printf("PosInsert: malloc failure, exiting...\n");
			return -1;
		}
		cur->next->docid=docid;
		cur->next->tf=1;
		cur->next->next=NULL;
		pos->df++;
	}
	return 0;
}
