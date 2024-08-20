#ifndef _SIMULATOR_H
#define _SIMULATOR_H

int m_mkdir(char* pathname);
int m_rmdir(char* pathname);
int m_cd(char* pathname);
int m_ls(char* pathname);
int m_pwd(char* pathname);
int m_creat(char* pathname);
int m_rm(char* pathname);
int m_save(char* pathname);
int m_reload(char* pathname);
int m_menu(char* pathname);
int m_quit(char* pathname);
int findCmd(char* command);

typedef int(*func)(char*) ; //func is a function pointer that takes a char* as an argument and returns an int
extern func funcptr[12]; // this is a vector of 12 func function pointers
#endif
