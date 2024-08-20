#ifndef _BINTREE_H
#define _BINTREE_H

#include <stdio.h>
#define LEFT 0
#define RIGHT 1

struct fsId{
	int key;
	char name[128];
	char type;
};
struct node {
	struct node* parent;
        struct node* cptr;
        struct node* sptr;
        struct fsId Id;
};
struct llistnode {
  char type;  // node = 1; qelements = 2;
  void** val;  //
  struct llistnode* next;
};

typedef struct qe{
	struct qe* next;
        struct node* node;
}qelements;

void getnodename(FILE *stream, struct node* node);
void printnode(FILE *stream, struct node* rnode);
void attachnode(struct node* rnode, struct node* tnode, short where);
void printfs(FILE *stream, struct node* rnode);
int qlenght(qelements *queue);
int enqueue(qelements **queue, struct node* tnode);
struct node* create(char* name, char type);
struct node* search(struct node* t, int id);
struct node* removenode(int id);
struct node* dequeue(qelements **queue);
#endif // !_BINTREE_H
