#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "structs.h"
#include "defs.h"

extern int heapSize;//maxheap.c

//local use
int printdoc(int i,char* text,int digitsk,int digitsn,struct MHnode doc,char** query,int qsz);

int user_input(struct TrieRoot trie,char** map,int doc_num,int k)
{
	//align-printing variables
	int digitsn=0,temp=doc_num;
	while(temp){temp/=10;digitsn++;}
	int digitsk=0,temp2=k;
	while(temp2){temp2/=10;digitsk++;}
	
	while(1)
	{
		char buffer[128];char* token;
		do{
		printf("> ");
		fgets(buffer,sizeof(buffer),stdin);
		token=strtok(buffer," \t\n");
		}while(token==NULL);//while input is empty
		
		if(!strcmp(token,"/search"))		///SEARCH CASE
		{
			char* query[10];
			int sz=0;	///sz holds the number of words
			while(token!=NULL && sz<10)
			{
				token=strtok(NULL," \t\n");
				if(token!=NULL)
				{
					query[sz]=malloc((strlen(token)+1) * sizeof(char));//allocate memory for every word of query
					strcpy(query[sz++],token);//keep every word
				}
			}
			struct BST* tree=NULL;///tree holds the root of BST
			if (!sz || (tree = SetUpScores(trie,query,sz,map,doc_num))==NULL)
			{
				printf("Search did not found any doc, try a different query\n");
				continue;
			}
			
			struct MHnode* heap = BSTtoMaxHeap(tree,doc_num);///heap holds the Max Heap
			
			for (int i = 0; i < k && heapSize; i++)///OUTPUT
			{
				struct MHnode doc = MH_pop(heap);
				if(printdoc(i,map[doc.docid],digitsk,digitsn,doc,query,sz)<0)return -1;
			}
			
			BSTdel(tree);//free tree
			MHdel(heap);//free heap
			for (int i = 0; i < sz; i++)
				free(query[i]);
		}
		else if(!strcmp(token,"/df"))		///DF CASE
		{
			char all=1;//flag that indicates if user wants everything or a word 
			while(token!=NULL)
			{
				token=strtok(NULL," \t\n");
				if (token!=NULL)
				{
					all=0;//user does not want everything
					TriePrintWordDF(trie.first,token);
				}
			}
			if(all)//print everything
				TriePrintAllDF(trie.first,"",0);
		}
		else if(!strcmp(token,"/tf"))		///TF CASE
		{
			int error;
			while(token!=NULL)
			{
				int id=0,i=0;
				token=strtok(NULL," \t\n");//create the id
				error=0;
				if (token!=NULL) while(token[i]!='\0')
				{
					if(token[i]<'0'||token[i]>'9')error=1;
					id*=10;id+=token[i]-'0';
					i++;
				}
				else break;
				if(error)break;
				token=strtok(NULL," \t\n");//take the word
				if (token!=NULL) TriePrintTF(trie.first,token,id);
			}
			if(error){printf("Usage: > /tf id1 word1 id2 word2 id3 word3 ...\n");continue;}
		}
		else if(!strcmp(token,"/exit"))		///EXIT CASE
			break;
		else 								///DEFAULT CASE
			printf("command not found\n");
	}
	return 0;
}

int inQuery(char* word,char** query,int sz)//returns True if word is in query
{
	int in=0;
	for (int i = 0; i < sz; i++)
		if(!strcmp(word,query[i])){in=1;break;}
	return in;
}

int printdoc(int i,char* text,int digitsk,int digitsn,struct MHnode doc,char** query,int qsz)
{
	///print the doc aligned
	
	int first=1;
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);//w.ws_col holds the number of columns of STDOUT
	int width=digitsk+digitsn+14;
	int limit=w.ws_col-width;
	if(width+2>w.ws_col){printf("stdout width error, too few columns\n");return-1;}
	
	printf("%*d.(%*d)[%08.4f] ",digitsk,i+1,digitsn,doc.docid,doc.score);fflush(stdout);
	
	//allocate space for 2 buffers, init with spaces
	char* buf=malloc((limit)*sizeof(char));
	memset(buf,' ',limit);buf[limit-1]='\0';
	char* bufunder=malloc((limit)*sizeof(char));
	memset(bufunder,' ',limit);bufunder[limit-1]='\0';
	
	int textc=0;
	while(text[textc]!='\0')
	{
		if(!first)for(int k=0;k<width;k++)putchar(' ');//print spaces if not in first time
		int linec=0;int flag_under=0;
		while(linec < limit-2 && text[textc+linec]!='\0')//until end of line
		{
			int linec1 = linec;	//keep position of linecounter
			
			int wordc=0;//word counter
			while(text[textc+linec1]!=' ' && text[textc+linec1]!='\t' && text[textc+linec1]!='\0')//keep word
			{
				if(linec1<limit)
					buf[wordc++]=text[textc+linec1];
				linec1++;
			}
			buf[wordc]='\0';
			
			if(linec1>=limit)break;//if word does not fit, break;
			
			if(inQuery(buf,query,qsz))//if word in query, modify bufunder
				{for(int j=linec;j<linec1;j++)
					bufunder[j]='^';
				flag_under=1;
				}
			
			printf("%s",buf);fflush(stdout);
			while(text[textc+linec1]==' ' || text[textc+linec1]=='\t')//whitespace
			{
				if(linec1<limit)
					putchar(text[textc+linec1]);
				else
					break;
				linec1++;
			}
			linec=linec1;
		}
		printf("\n");
		if(flag_under)
		{
			for(int k=0;k<width;k++)putchar(' ');
			printf("%s\n",bufunder);
		}
		memset(bufunder,' ',limit);bufunder[limit-1]='\0';//empty underline buffer for next line
		textc+=linec;
		first=0;
	}
	free(buf);free(bufunder);
}

int parse_arguments(int argc,char** argv,FILE** docfile,int* k)
{
	///returns -1 in failure
	///opens docfile
	char* error_msg = "Usage: ./minisearch -i docfile -k K\n";
	if(argc==5)
	{
		if(!strcmp(argv[1],"-i"))
		{
			if(strcmp(argv[3],"-k"))
			{
				printf("%s-Give the max number parameter (-k)\n",error_msg);
				return -1;
			}
			if((*docfile = fopen(argv[2],"r")) == NULL)	//opening docfile
			{
				printf("%s-Wrong input file\n",error_msg);
				return -1;
			}
			if((*k = atoi(argv[4])) <= 0)			//parsing k		
			{
				printf("%s-Wrong K number\n",error_msg);
				return -1;
			}
		}	
		else if(!strcmp(argv[3],"-i"))
		{
			if(strcmp(argv[1],"-k"))
			{
				printf("%s-Give the max number parameter (-k)\n",error_msg);
				return -1;
			}
			if((*docfile = fopen(argv[4],"r")) == NULL)	//opening docfile
			{
				printf("%s-Wrong input file\n",error_msg);
				return -1;
			}
			if((*k = atoi(argv[2])) <= 0)			//parsing k		
			{
				printf("%s-Wrong K number\n",error_msg);
				return -1;
			}
		}		
		else	
		{
			printf("%s-Give the docfile parameter (-i)\n",error_msg);
			return -1;
		}
	}
	else if(argc==3)//default k=10
	{
		if(!strcmp(argv[1],"-i"))
		{
			if((*docfile = fopen(argv[2],"r")) == NULL)	//opening docfile
			{
				printf("%s-Wrong input file\n",error_msg);
				return -1;
			}
			*k = 10;
		}
		else
		{
			printf("%s-Give the docfile parameter (-i)\n",error_msg);
			return -1;
		}
	}
	else
	{
		printf("%s-Wrong number of arguments\n",error_msg);
		return -1;
	}

	return 0;
}
