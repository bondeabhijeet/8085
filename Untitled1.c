#include <stdio.h>
#include <string.h>
int main ()
{
  char string[50] ="Test,string1,Test,string2:Test:string3";
  char *p;
  printf ("String  \"%s\" is split into tokens:\n",string);
  p = strtok (string,",:");
  printf("\nNEW:%s\n", p);
  while (p!= NULL)
  {
    printf ("%s\n",p);
    p = strtok (string, ",:");
  }
  return 0;
}
