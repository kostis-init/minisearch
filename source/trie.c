#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "defs.h"

int avgdl=0;///average document length


int create_index(char** map,int sz,struct TrieRoot* trie)
{
	/**INIT TRIE*/
	if(TrieInit(trie)<0) return -1;
	
	/**INSERT TO TRIE*/
	for(int i=0; i<sz; i++)		//for every doc
	{
		char* temp=malloc((strlen(map[i])+1)*sizeof(char));
		strcpy(temp,map[i]);//make a copy of doc for strtok
		for(char* word=strtok(temp," \t"); word!=NULL; word=strtok(NULL, " \t"))//for every word
		{
			avgdl++;
			if(TrieInsert(trie,word,i)<0) return -1;
		}
		printf("\r%d%%",(i+1)*100/sz);fflush(stdout);//show progress dynamically
		free(temp);//clear temp for next doc
	}
	printf("\n");
	avgdl=avgdl/sz + 1;
	return 0;
}

void TRecDel(struct TrieNode* node)
{
	if(node->pos!=NULL)PosDelete(node->pos);
	if(node->child!=NULL || node->next!=NULL)
	{
		TRecDel(node->child);
		TRecDel(node->next);
	}
	free(node);
}

void TrieDelete(struct TrieRoot trie)
{
	TRecDel(trie.first);
}

int TriePrintTF(struct TrieNode* node,char* word,int id)
{
	struct PosList* pos=node->pos;
	int i=0;
	while(word[i]!='\0')///search trie character-character
	{
		
		while(node!=NULL)
		{
			if(word[i] == node->c)
				break;
			node=node->next;
		}
		if(node==NULL)///if not found
		{
			printf("%d %s 0\n",id,word);
			return 0;
		}
		
		i++;
		pos=node->pos;
		node=node->child;
	}
	///pos holds the postings list of the word
	if(pos==NULL)
	{
		printf("%d %s 0\n",id,word);
		return 0;
	}
	struct PosNode* cur = pos->first;
	while(cur!=NULL)///search pos for doc id
	{
		if(cur->docid==id)//found
		{
			printf("%d %s %d\n",id,word,cur->tf);
			break;
		}
		cur=cur->next;
	}
	if(cur==NULL)///if not found
		printf("%d %s 0\n",id,word);
	return 0;
}

///Recursive Search
void TriePrintAllDF(struct TrieNode* node,char* word,int depth)
{
	char* newword=malloc((depth+2)*sizeof(char));
	strcpy(newword,word);//keep current word
	
	while(node!=NULL)
	{
		newword[depth]=node->c;newword[depth+1]='\0';
		if(node->pos!=NULL)//we have a word, print it
			printf("%s %d\n",newword,node->pos->df);
		
		TriePrintAllDF(node->child,newword,depth+1);//go to next level
		
		newword[depth]='\0';//prepare the word for next char
		node=node->next;
	}
	free(newword);
}

int TriePrintWordDF(struct TrieNode* node,char* word)
{
	struct PosList* pos=node->pos;
	int i=0;
	while(word[i]!='\0')///search trie character-character
	{
		
		while(node!=NULL)
		{
			if(word[i] == node->c)
				break;
			node=node->next;
		}
		if(node==NULL)///if not found
		{
			printf("%s 0\n",word);
			return 0;
		}
		
		i++;
		pos=node->pos;
		node=node->child;
	}
	///pos holds the postings list of the word
	if(pos==NULL)
	{
		printf("TriePrintWordDF error: pos==NULL");
		return -1;
	}
	printf("%s %d\n",word,pos->df);
	
	return 0;
}

int TrieInsert(struct TrieRoot* trie,char* word,int docid)
{
	struct TrieNode* cur = trie->first;//cur:	current node
	char c;
	int i=0;
	while(word[i]!='\0' && cur!=NULL)//for every letter
	{
		while(cur->next!=NULL)//search nodes of current level for word[i]
		{
			if(cur->c == word[i])//found letter
				break;
			cur=cur->next;
		}
		if(cur->next==NULL)//if not found
		{
			cur->c=word[i];
			TrieNodeInit(&(cur->next));
			TrieNodeInit(&(cur->child));
		}
		//printf("    loop %d , cur->c=%c\n",i,cur->c);
		if(word[i+1]=='\0')//finished
			break;
		cur=cur->child;
		i++;
	}
	
	///cur points to the leaf of word
	///edit the postings list
	if(cur->pos==NULL)///initialize pos list if needed
	{
		if((cur->pos=malloc(sizeof(struct PosList)))==NULL)
		{
			printf("TrieInsert: malloc failure, exiting...\n");
			return -1;
		}
		cur->pos->df=1;
		if((cur->pos->first=malloc(sizeof(struct PosNode)))==NULL)
		{
			printf("TrieInsert: malloc failure, exiting...\n");
			return -1;
		}
		cur->pos->first->docid=docid;
		cur->pos->first->tf=1;
		cur->pos->first->next=NULL;
		//printf("docid=%d,pos->df=%d\n",docid,cur->pos->df);
	}
	else PosInsert(cur->pos,docid);
	
	
	return 0;
}

int TrieNodeInit(struct TrieNode** node)
{
	if(((*node)=malloc(sizeof(struct TrieNode)))==NULL)
	{
		printf("TrieNodeInit: malloc failure, exiting...\n");
		return -1;
	}
	(*node)->c     = 0;
	(*node)->child = NULL;
	(*node)->next  = NULL;
	(*node)->pos   = NULL;
	
	return 0;
}

int TrieInit(struct TrieRoot* trie)
{
	if((trie->first=malloc(sizeof(struct TrieNode)))==NULL)
	{
		printf("TrieInit: malloc failure, exiting...\n");
		return -1;
	}
	trie->first->c     = 0;
	trie->first->child = NULL;
	trie->first->next  = NULL;
	trie->first->pos   = NULL;
	
	return 0;
}
