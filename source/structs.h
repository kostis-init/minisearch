#ifndef STRUCTS_H
#define STRUCTS_H

///MAX HEAP
struct MHnode
{
	double score;
	int docid;
};

///BST
struct BST
{
	double score;
	int docid;
	struct BST* left;
	struct BST* right;
};

///POSTINGS LIST
struct PosNode
{
	int docid;
	int tf;
	struct PosNode* next;
};

struct PosList
{
	int df;
	struct PosNode* first;
};

///TRIE
struct TrieNode
{
	char c;
	struct TrieNode* child;
	struct TrieNode* next;
	struct PosList*  pos;
};

struct TrieRoot
{
	struct TrieNode* first;
};


#endif
