#include <stdio.h>
#include <string.h>

#include "include/bintree.h"
#include "include/simulator.h"

//GLOBALS
/*
 * type			name
 * --------------------------|
 * node*		root
 * node*		cwd
 * char[]		line
 * char[]		command
 * char[]		dname
 * char[]		bname
 */

struct node* root, *cwd;
char line[128], command[16], pathname[64], dname[64], bname[64];

void initialize();

int main(void){
	int index;
	initialize();
	struct node* tnode1= create("dir1", 'D');
	struct node* tnode2= create("dir2", 'D');
	attachnode(root, tnode1, LEFT);		//child
	attachnode(tnode1, tnode2, RIGHT);	//sibling
        while (1) {
          printf("$: ");
          if (fgets(line, 128, stdin) == NULL) break;
          line[strlen(line) - 1] = 0;
          sscanf(line, "%s %s", command, pathname);
          index = findCmd(command);
          if (index < 0) continue;
          if ((funcptr[index](pathname)) != 0){
              fprintf(stderr, "an error occured!\n");
          }

          // flush variables for reuse
          command[0] = 0;
          pathname[0] = 0;
        }
        /*
        struct node* tnode1= create("dir1", 'D');
        struct node* tnode2= create("dir2", 'D');
        struct node* tnode3= create("dir3", 'D');
        struct node* tnode4= create("file1.txt", 'F');
        struct node* tnode5= create("file2.txt", 'F');
        struct node* tnode6= create("file3.txt", 'F');
        struct node* tnode7= create("dir4", 'D');
        struct node* tnode8= create("file4.txt", 'F');
        attachnode(root, tnode1, LEFT);		//child
        attachnode(tnode1, tnode2, RIGHT);	//sibling
        attachnode(tnode2, tnode3, RIGHT);	//sibling
        attachnode(tnode2, tnode4, LEFT);	//child
        attachnode(tnode3, tnode5, LEFT);	//child
        attachnode(tnode1, tnode6, LEFT);	//child
        attachnode(tnode3, tnode7, LEFT);	//child
        attachnode(tnode7, tnode8, LEFT);	//child

        printfs(root);
        //preorder_travseral(root);
        */
        printf("\n");
}

/*
 * Initialize the root node
 **/
void initialize() {
  root = create("", 'D');
  root->sptr = root->cptr = NULL;
  cwd = root;
}
