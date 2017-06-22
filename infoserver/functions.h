#ifndef FUNCTIONS_H
#define FUNCTIONS_H

char *program_path();
int getch();
bool starts_with(char *str, const char *neddle);
bool ends_with(char *str, int size, const char *needle) ;
char *get_xml(char *name);

#endif
