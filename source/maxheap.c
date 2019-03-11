#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "defs.h"

int heapSize=0;

//local functions
void RecursiveInsert(struct MHnode* heap,struct BST* node);
void MH_insert(struct MHnode* heap,int docid,double score);
void switch_nodes(struct MHnode* heap,int i,int j);

void MHdel(struct MHnode* heap)
{
	heapSize=0;
	free(heap);
}

struct MHnode* BSTtoMaxHeap(struct BST* tree,int doc_num)
{
	struct MHnode* heap=malloc(doc_num*sizeof(struct MHnode));//allocate heap
	
	RecursiveInsert(heap,tree);
	
	return heap;
}

void RecursiveInsert(struct MHnode* heap,struct BST* node)
{
	if(node->left!=NULL || node->right!=NULL)
	{
		if(node->left!=NULL)RecursiveInsert(heap,node->left);
		if(node->right!=NULL)RecursiveInsert(heap,node->right);
	}
	MH_insert(heap,node->docid,node->score);
}

void switch_nodes(struct MHnode* heap,int i,int j)//only for local use of maxheap!!!
{
	int tempp = heap[i].docid;
	double temp = heap[i].score;
	
	heap[i].docid = heap[j].docid;
	heap[i].score = heap[j].score;
	
	heap[j].docid = tempp;
	heap[j].score = temp;
}

void MH_insert(struct MHnode* heap,int docid,double score)
{
	//insert at last
	heap[heapSize].docid=docid;
	heap[heapSize++].score=score;
	
	//put it in the right position
	int i = heapSize - 1;
	while ((i-1)/2 >= 0) //while not in the root
		if (heap[i].score > heap[(i-1)/2].score)	//if child > parent then switch them and continue
		{
			switch_nodes(heap,i,(i-1)/2);
			i = (i-1)/2;
		}
		else //no need to continue
			break;
	
}

struct MHnode MH_pop(struct MHnode* heap)
{
	//keep the first
	struct MHnode item;
	item.docid = heap[0].docid;
	item.score = heap[0].score;
	heapSize--;
	
	//last -> first
	switch_nodes(heap,heapSize,0);
	
	//fix the heap
	int i = 0;
	while (heapSize > 2*i + 1)//while there is at least one child
	{
		int maxpos;
		if (2*i + 1 == heapSize - 1)//if it has one child only
			maxpos = 2*i+1;
		else if (heap[2*i+1].score < heap[2*i+2].score)//keep the minimum child
			maxpos = 2*i+2;
		else
			maxpos = 2*i+1;
		
		if (heap[i].score < heap[maxpos].score)//if parent < child then switch them and continue
		{
			switch_nodes(heap,i,maxpos);
			i=maxpos;
		}
		else //no need to continue
			break;
	}
	
	return item;
}
