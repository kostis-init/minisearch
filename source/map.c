#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "defs.h"

int check_file(FILE* docfile)
{
	///checks input file's format,
	///returns the number of documents (or -1 in case of failure)
	char c;
	int doc_num=0;
	fseek(docfile, 0, SEEK_SET);//rewind to the beginning of file
	
	while(1)//take 1st word (id) from every line, and compare it with current doc_num
	{
		int empty_line=1;//flag that indicates if the line is empty
		int n=0;//id from doc (string), will be copied in n (int)
		c=fgetc(docfile);
		while(c!=' ' && c!= '\t' && c!='\n' && c!=EOF)//keep id
		{
			empty_line=0;
			n*=10;n+=c-'0';//create n
			c=fgetc(docfile);
		}
		while(c!='\n' && c!=EOF)//skip the rest of the line
		{
			empty_line=0;
			c=fgetc(docfile);
		}
		
		if(c==EOF)break;
		if(empty_line)continue;
		
		if(doc_num!=n)//check that ids are in order
		{
			printf("Input file has wrong format (ids must be in order, starting from 0), exiting...\n");
			return -1;
		}
		//printf("n=%d, doc_num=%d\n",n,doc_num);
		doc_num++;
	}
	return doc_num;
}

int create_map(FILE* docfile,char** map)//copies from docfile into map
{
	///scans file 2 times. 1 to measure every doc, 2 to save every doc.(to save memory)
	///returns -1 in failure
	///closes docfile
	char c;
	int i=0;//iteration number (or doc_number)
	fseek(docfile, 0, SEEK_SET);//rewind to the beginning of file
	while(1)//first scanning to measure every doc
	{
		int empty_line=1;//flag that indicates if the line is empty
		c=fgetc(docfile);
		while(c!=' ' && c!= '\t' && c!='\n' && c!=EOF)//skip id
		{
			empty_line=0;
			c=fgetc(docfile);
		}
		while(c==' ' || c== '\t')//skip spaces/tabs after id
			c=fgetc(docfile);
		
		if(c==EOF)break;
		if(empty_line)continue;
		
		int len=0;
		while(c!='\n' && c!=EOF)//count length of doc
		{
			len++;
			c=fgetc(docfile);
		}
		
		if((map[i]=malloc((len+1) * sizeof(char)))==NULL)//allocate as much memory, as needed, for every doc
		{
			printf("create_map: malloc failure, exiting...\n");
			return -1;
		}
		i++;
	}
	i=0;
	fseek(docfile, 0, SEEK_SET);//rewind to the beginning of file
	while(1)//second scanning to save docs
	{
		int empty_line=1;//flag that indicates if the line is empty
		c=fgetc(docfile);
		while(c!=' ' && c!= '\t' && c!='\n' && c!=EOF)//skip id
		{
			empty_line=0;
			c=fgetc(docfile);
		}
		while(c==' ' || c== '\t')//skip spaces/tabs after id
			c=fgetc(docfile);
		
		if(c==EOF)break;
		if(empty_line)continue;
		
		int j=0;//counter of doc
		while(c!='\n' && c!=EOF)//save doc
		{
			map[i][j]=c;
			j++;
			c=fgetc(docfile);
		}
		map[i][j]='\0';
		i++;
	}
	fclose(docfile);
	return 0;
}
