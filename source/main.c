#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "defs.h"

int main(int argc,char* argv[])
{
	/**PARSE ARGUMENTS*/
	FILE* docfile=NULL;		///docfile:	holds the input file
	int k=0;				///k:		holds the maximum number of results
	if(parse_arguments(argc,argv,&docfile,&k)<0) return -1;
	
	/**CHECK DOCFILE*/
	printf("Checking the file...\n");
	int doc_num=0;			///doc_num:	holds the number of the documents in the docfile
	if((doc_num=check_file(docfile))<0) return -1;
	printf("Check Done! The file consists of %d docs.\n\n",doc_num);
	
	/**CREATE DATABASE*/
	printf("Database is being created...\n");
	char** map=NULL;		///map:		holds the map of the documents (i.e. map[0] is the document with id 0)
	if((map=malloc(doc_num * sizeof(char *)))==NULL)return-1;
	if(create_map(docfile,map)<0) return -1;//copy from file to map, close docfile
	
	struct TrieRoot trie;	///trie:	root of the trie
	if(create_index(map,doc_num,&trie)<0) return -1;//pass words from map into trie
	printf("Database has been created!\n\n");
	
	/**USER INPUT*/
	if(user_input(trie,map,doc_num,k)<0) return -1;
	
	/**CLEAN MEMORY*/
	for(int i=0;i<doc_num;i++)free(map[i]);
	free(map);TrieDelete(trie);
	return 0;
}
