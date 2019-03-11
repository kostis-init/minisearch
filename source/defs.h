#ifndef DEFS_H
#define DEFS_H

//ui.c
int parse_arguments(int argc,char** argv,FILE** docfile,int* k);//parse starting arguments (docfile, k)
int user_input(struct TrieRoot trie,char** map,int doc_num,int k);

//map.c
int check_file(FILE* docfile);//check docfile for right format, return number of docs
int create_map(FILE* docfile,char** map);//save docs from docfile in the map

//trie.c
int create_index(char** map,int sz,struct TrieRoot* trie);//save words from map in the trie
int TrieInit(struct TrieRoot* trie);
int TrieNodeInit(struct TrieNode** node);
int TrieInsert(struct TrieRoot* trie,char* word,int docid);//insert word into trie
void TriePrintAllDF(struct TrieNode* node,char* word,int depth);//print DF for every word
int TriePrintWordDF(struct TrieNode* node,char* word);//print DF for this word
int TriePrintTF(struct TrieNode* node,char* word,int id);//print TF for this word in doc id
void TrieDelete(struct TrieRoot);

//scores.c
void BSTdel(struct BST* node);//free BST memory
struct BST* SetUpScores(struct TrieRoot trie,char** query,int sz,char** map,int doc_num);//create BST that holds doc ids with scores
int AddScore(struct BST* root,int id,double score);//add this score to doc with this id in the binary tree

//pos.c
void PosDelete(struct PosList* pos);
int PosInsert(struct PosList* pos,int docid);

//maxheap.c
struct MHnode* BSTtoMaxHeap(struct BST* tree,int doc_num);//convert BST to MaxHeap
struct MHnode MH_pop(struct MHnode* heap);//pop from MaxHeap
void MHdel(struct MHnode* heap);//free heap memory

#endif
