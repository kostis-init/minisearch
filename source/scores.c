#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "defs.h"

extern int avgdl;//in trie.c

//debug print
void printB(struct BST* node)
{
	if(node->left!=NULL || node->right!=NULL)
	{
		if(node->left!=NULL)printB(node->left);
		if(node->right!=NULL)printB(node->right);
	}
	printf("docid:%d score:%f\n",node->docid,node->score);
}

struct BST* SetUpScores(struct TrieRoot trie,char** query,int sz,char** map,int doc_num)
{
	/**	We have a binary search tree (based on the doc_id),
	*	so that every time we want to edit a score of a doc,
	*	searching will be done in O(logn).*/
	
	struct BST* root=malloc(sizeof(struct BST));
	root->right=NULL;root->left=NULL;root->score=0;
	root->docid= doc_num/2; //it will be changed later
	
	char query_valid=0;///flag that indicates if query has at least one word in any doc
	char first=1;///flag to change root->docid
	for (int i = 0; i < sz; i++)///for every word of query
	{
		char exists=1;///flag that indicates if word exists in trie
		int j=0;
		struct TrieNode* node=trie.first;
		struct PosList* pos=node->pos;///pos holds the postings list of the word
		while(query[i][j]!='\0')///search trie character-character
		{
			while(node!=NULL)
			{
				if(query[i][j] == node->c)
					break;
				node=node->next;
			}
			if(node==NULL)///if not found
			{
				exists=0;
				break;
			}
			j++;
			pos=node->pos;
			node=node->child;
		}
		if(!exists || pos==NULL)continue;
		
		double idf=log((double)(doc_num - pos->df + 0.5)/(double)(pos->df + 0.5));
		struct PosNode* cur = pos->first;
		while(cur!=NULL)///calculate (additional) score of every doc and insert to tree
		{
			query_valid=1;
			if(first){first=0;root->docid=cur->docid;}
			
			int wc=0;//word count of doc
			for(int index=0;map[cur->docid][index]!='\0';index++)
				if(map[cur->docid][index]!=' ' && map[cur->docid][index]!='\t')
				{
					wc++;
					while(map[cur->docid][index]!=' ' && map[cur->docid][index]!='\t' && map[cur->docid][index]!='\0')
						index++;
					if(map[cur->docid][index]=='\0')break;
				}
			double k1=1.2,b=0.75;
			double score = idf * ( (double)( cur->tf*(k1+1) ) / (double)( cur->tf + k1*(1-b+b*((double)(wc)/(double)(avgdl))) ) );
			
			if(AddScore(root,cur->docid,score)<0)return NULL;
			
			cur=cur->next;
		}
		
		
	}
	
	if(!query_valid)return NULL;
	return root;
}

int AddScore(struct BST* root,int id,double score)
{
	/**Search BST for id. If it exists add the score,
	 * else create a node and init score*/
	
	struct BST* cur=root;
	struct BST* prev=NULL;
	
	while(cur!=NULL)
	{
		prev=cur;
		if(id==cur->docid)
		{
			cur->score+=score;
			break;
		}
		else if(id > cur->docid)
			cur=cur->right;
		else
			cur=cur->left;
	}
	if(cur==NULL)//not found id, create a node
	{
		
		if(id > prev->docid)
		{
			prev->right=malloc(sizeof(struct BST));
			prev->right->docid=id;
			prev->right->score=score;
			prev->right->left=NULL;prev->right->right=NULL;
		}
		else
		{
			prev->left=malloc(sizeof(struct BST));
			prev->left->docid=id;
			prev->left->score=score;
			prev->left->left=NULL;prev->left->right=NULL;
		}
	}
	
	return 0;
}

void BSTdel(struct BST* node)//free BST
{
	if(node->left!=NULL || node->right!=NULL)
	{
		if(node->left!=NULL)BSTdel(node->left);
		if(node->right!=NULL)BSTdel(node->right);
	}
	free(node);
}
