#include "include/bintree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int GlobalNodeKeyCount = 0;
static int GlobalNodeCreateCount = 0;
void birthnode(struct node* rnode, struct node* tnode);
void newsibling(struct node* onode, struct node* nnode);
struct node* create(char* name, char type) {
  if (!(type == 'D' || type == 'F')) {
     fprintf(stderr, "Error: invalid arguments\n");
  }
  struct node* t = (struct node*)malloc(sizeof(struct node));
  GlobalNodeCreateCount++;
  t->Id.type = type;
  strcpy(t->Id.name, name);
  t->Id.key = GlobalNodeKeyCount++;
  t->parent = t->cptr = t->sptr = NULL;
  return t;
};

void attachnode(struct node* rnode, struct node* tnode, short where) {
  if (where == LEFT) {
    tnode->parent = rnode;
    birthnode(rnode, tnode);
  } else if (where == RIGHT) {
    newsibling(rnode, tnode);
    tnode->parent = rnode->parent;
  }
}
// this search would work only if our tree was a binary search tree
struct node* search(struct node* t, int id) {
  if (t == NULL || t->Id.key == id) {
    return t;
  }
  if (id < t->Id.key) {
    return search(t->cptr, id);
  } else {
    return search(t->sptr, id);
  }
}

void update(struct node* rnode, int id, char* name) {
  struct node* fnode = search(rnode, id);
  if (fnode != NULL) {
    strcpy(fnode->Id.name, name);
  } else {
    return;
  }
}

void printnode(FILE *stream, struct node* rnode) {
  if (rnode->parent == NULL)
    fprintf(stream, "/");
  else {
    fprintf(stream, "[%c]", rnode->Id.type);
    getnodename(stream,rnode);
    fprintf(stream, "  ");
  }
}
int enqueue(qelements** queue, struct node* tnode) {
  qelements* q = *queue;
  qelements* r = (qelements*)malloc(sizeof(qelements));
  GlobalNodeCreateCount++;
  r->node = tnode;
  if (q == 0)
    *queue = r;
  else {
    while (q->next) {
      q = q->next;
    }
    q->next = r;
  }
  r->next = NULL;
  return 0;
}
struct node* dequeue(qelements** queue) {
  qelements* q = *queue;
  struct node* retnode;
  if (q){
    *queue = q->next;
    retnode = q->node;
    free(q);
    q = NULL;
    GlobalNodeCreateCount--;
  }
  return retnode;
}
int qlenght(qelements* queue) {
  int n = 0;
  while (queue) {
    n++;
    queue = queue->next;
  }
  return n;
}

// print a binary tree by levels, each level on a line
void printLevel(struct node* rnode) {
  int nodeCount;
  if (rnode == NULL) return;
  qelements* queue = 0;
  enqueue(&queue, rnode);
  while (1) {
    nodeCount = qlenght(queue);
    if (nodeCount == 0) break;
    while (nodeCount > 0) {
      struct node* node = dequeue(&queue);
      //printnode(node);
      if (node->cptr != NULL) enqueue(&queue, node->cptr);
      if (node->sptr != NULL) enqueue(&queue, node->sptr);
      nodeCount--;
    }
    printf("\n"); 
  }
}
void printfs(FILE *stream, struct node* rnode) { // needs refactori
  if (rnode == NULL) return;
  struct node* tchild = rnode->cptr;
  struct node* firstchild = tchild;
  int nodecount = 0;
  qelements* queue = 0;
  while (firstchild) {
    // for every child add all it's siblings to the queue
    while (tchild) {
      enqueue(&queue, tchild);
      tchild = tchild->sptr;
    }
    nodecount = qlenght(queue);
    // remove each child from the queue and add the oldest sibling child to the
    // queue
    while (nodecount > 0) {
      struct node* node = dequeue(&queue);
      if (node->cptr) enqueue(&queue, node->cptr);
      printnode(stream,node);
    fprintf(stream, "\n");
      nodecount--;
    }
    if (queue == NULL)
      break;
    else {
      firstchild = queue->node;
      tchild = firstchild->sptr ? firstchild->sptr : NULL;
    }
  }
}
void birthnode(struct node* rnode, struct node* tnode) {
  if (rnode->cptr != NULL) {
    struct node* ttnode = rnode->cptr->sptr;
    if (ttnode == NULL) {
      rnode->cptr->sptr = tnode;
      return;
    }
    while (ttnode->sptr) {
      ttnode = ttnode->sptr;
    }
    ttnode->sptr = tnode;
  } else
    rnode->cptr = tnode;
}
void newsibling(struct node* onode, struct node* nnode) {
  if (onode->sptr != NULL) {
    struct node* tnode = onode->sptr;
    while (tnode->sptr) {
      tnode = tnode->sptr;
    }
    tnode->sptr->sptr = nnode;
  } else {
    onode->sptr = nnode;
  }
}
