#include "include/simulator.h"
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include "include/bintree.h"

#define OPMK 1
#define OPRM -1

extern struct node* cwd;
extern struct node* root;

func funcptr[12] = {(func)m_mkdir, m_rmdir, m_ls,     m_cd,   m_pwd, m_creat,
                    m_rm,          m_save,  m_reload, m_menu, m_quit};
char* cmd[] = {"mkdir", "rmdir", "ls",     "cd",   "pwd",  "creat",
               "rm",    "save",  "reload", "menu", "quit", 0};
char* tokenResult[12];
char** tokenize(char* res[], char* pathname) {
  // clear this global array as it may already have some content
  for (int i=0; i < 12; i++) {
    tokenResult[i] = 0;
  }
  char* s;
  char *pathcpy = (char *)malloc(sizeof(pathname));
  strcpy(pathcpy, pathname);
  s = strtok(pathcpy, "/");
  int index = 0;
  while (s) {
    res[index] = (char*)malloc(strlen(s));
    strcpy(res[index], s);
    index++;
    if (index > 11) return res;
    s = strtok(NULL, "/");
  }
  return res;
}

int findCmd(char* command) {
  int i = 0;
  while (cmd[i]) {
    if (!strcmp(command, cmd[i])) {
      return i;
    }
    i++;
  }
  return -1;
}

struct node* bfsImpl(struct node* rnode, char* path) { return NULL; }

struct node* searchbfs(struct node* rnode, char* pathname) {
  int index = 0;
  char** resArr = tokenize(tokenResult, pathname);
  struct node* fnode = bfsImpl(
      rnode,
      tokenResult[index++]);  // the ++ here says increement after you are done
                              // so it's resArr[1] then index becomes 1
  while (resArr[index]) {
    fnode = bfsImpl(fnode, resArr[index]);
    index++;
  }
  return fnode;
}

struct node* dfsImpl(struct node* rnode, char* path) {
  if (rnode == NULL || path == NULL) return NULL;
  if (strcmp(rnode->Id.name, path) == 0) {
    return rnode;
  } else {
    struct node* tnode;

    tnode = dfsImpl(rnode->cptr, path);
    if (tnode)
      return tnode;
    else
      return dfsImpl(rnode->sptr, path);
  }
}

struct node* searchdfs(struct node* rnode, char* pathname) {
  int index = 0;
  char** resArr = tokenize(
      tokenResult, pathname);  // the return value of tokenize should be changed
  struct node* fnode = dfsImpl(rnode, tokenResult[index++]);
  while (resArr[index]) {
    fnode = dfsImpl(fnode, resArr[index]);
    if (fnode == NULL) return NULL;
    index++;
  }
  return fnode;
}

// This function finds the direct senior of a node in the tree
struct node* findSenior(struct node* node){
      struct node* tnode = node->parent->cptr;
      while (tnode && tnode->sptr != node) {
        tnode= tnode->sptr;
      }
      return tnode;
}

int delete(struct node* node){
  //case 1: no child  no sibling
  if (node->cptr == NULL && node->sptr == NULL){
    struct node* tnode = findSenior(node);
    tnode->sptr = NULL;
    free(node);
    node = NULL;
    return 1;
  }
  //case 2: no child  yes sibling
  else if (node->cptr == NULL) {
    struct node* tnode = findSenior(node);
    if (tnode) {
      tnode->sptr = node->sptr;
      free(node);
      node = NULL;
      return 1;
      }
  }
  return -1;
} 

int makeNode(char *pathname, char type){
  char* pathcpy = (char*)malloc(strlen(
    pathname));  // we copy the pathname because dirname pathname modifies
  // the original copy that we want to pass to the basename
  strcpy(pathcpy, pathname);
  char *dname = dirname(pathcpy), *bname = basename(pathname);
  struct node* fnode;
  if (strcmp(dname, ".") != 0 && strcmp(dname, "/") != 0) {
    fnode = searchdfs(root, dname);
  } else {
    fnode = cwd;
  }
  if (pathname[0] == '/') {  // absolute search
    if (fnode && fnode->Id.type == 'D') {
      struct node* tnode = create(bname, type);
      attachnode(fnode, tnode, LEFT);
      return 0;
    }
  } else {
    if (strcmp(dname, ".")) fnode = searchdfs(cwd, dname);
    if (fnode && fnode->Id.type == 'D') {
      struct node* tnode = create(bname, type);
      attachnode(fnode, tnode, LEFT);
      return 0;
    }
  }
  free(pathcpy);
  return -1;

}
int removeNode(char *pathname, char type){
  struct node* fnode = pathname[0] == '/' ? searchdfs(root, pathname) : searchdfs(cwd, pathname);
  if (fnode && fnode->Id.type == type) {
    if (fnode->cptr == NULL) {  // why are we checking for this?
      delete (fnode);
      return 0;
    } else{
      fprintf(stderr, "directory is not empty\n");
      return -1;
    }
  }
  else {
    return -1;
  }
} 
int mknode(char* pathname, char type, int optype) { //needs refactoring
  switch (optype) {
    case OPMK: {
     return makeNode(pathname, type);
    }
    case OPRM: {
      return removeNode(pathname, type);
    }
    default:
      fprintf(stderr, "an error occured\n");
      return -1;
  }
}

int m_mkdir(char* pathname) { return mknode(pathname, 'D', OPMK); }

int m_rmdir(char* pathname) { return mknode(pathname, 'D', OPRM); }

int m_cd(char* pathname) {
  if (strcmp(pathname, "") == 0) {
    cwd = root;
  } else if (pathname[0] == '/') {
    struct node* fnode = searchdfs(root, pathname);
    if (fnode && fnode->Id.type == 'D') {
      cwd = fnode;
      return 0;
    } else
      return -1;
  } else {
    struct node* fnode = searchdfs(cwd, pathname);
    if (fnode && fnode->Id.type == 'D') {
      cwd = fnode;
      return 0;
    } else {
      return -1;
    }
  }
  return 0;
}

int m_ls(char* pathname) {
  if (strcmp(pathname, "") == 0) {
    printfs(stdout, cwd);
    return 0;
  }
  if (strcmp(pathname, "/") == 0) {
    printfs(stdout, root);
    return 0;
  }
  struct node* fnode = searchdfs(pathname[0] == '/' ? root : cwd, pathname);
  if (fnode && fnode->Id.type == 'D') {
    printfs(stdout, fnode);
    return 0;
  } else {
    return -1;
  }
}

int m_pwd(char* pathname) {
  if (cwd == root)
    printf("/");
  else
    getnodename(stdout,cwd);
  printf("\n");
  return 0;
}
void getnodename(FILE *stream, struct node* node) {
  if (node == NULL || node->Id.key == 0) return;
  getnodename(stream, node->parent);
  fprintf(stream,"/%s", node->Id.name);
}

int m_creat(char* pathname) { return mknode(pathname, 'F', OPMK); }

int m_rm(char* pathname) { return mknode(pathname, 'F', OPRM); }

int m_save(char* pathname) {
  //open the file pathname or default if pathname is null
  int o_fsfd;
  if (strcmp(pathname, "") == 0){
    return -1;
  }
  if ((o_fsfd = open(pathname, O_WRONLY | O_CREAT, 0640)) < 0) {
    perror("open failed");
  }
  char *str = "type/pathname\n____________\n";
  if (write(o_fsfd,str , strlen(str)) != strlen(str)){
    return -1;
  }
  FILE *stream;
  if ((stream = fdopen(o_fsfd, "a")) < 0) {
    perror("open failed");
  }
  printfs(stream, root);
  fclose(stream);
  close(o_fsfd);
  return 0;
}

int m_reload(char* pathname) {
  if (strcmp(pathname, "") == 0) return -1;
  // open file
  FILE *stream;
  if ((stream = fopen(pathname, "r")) < 0) {
    perror("open failed");
  }
  // read line
  char line[128];
  char c;
  char type, path[20];
  int count = 0, newline_count = 0;
  while (!feof(stream)) {
    while (c != '\n'){
      c =fgetc(stream);
      if ( c > 127) break;
      line[count++] = c;
    }
    if ( c > 127) break;
    c = 0;
    newline_count++;
    if (newline_count < 3 ) count = 0;
    else{
        sscanf(line, "[%c]/%s", type , path);
        printf("%c %s\n", type, path);
    }
  }
  printf("%s\n", line);
  // create node with the type and name 
  // proceed to the next
  //
  fclose(stream);
  return 0;
}

int m_menu(char* pathname) {
  printf(
      "Welcome to the c file system simulator, here is a list of the "
      "commands:\nmkdir pathname, rmdir pathname, ls [pathname], cd "
      "[pathname], creat pathname, rm pathname, menu, save [pathname], reload "
      "[pathname], quit [pathname]\n");
  printf("Enter any one of the commands above to continue\n");
  return 0;
}

int m_quit(char* pathname) { 
  if (pathname){
    m_save(pathname); 
  }
  exit(0);
}
