#include<trial.h>
#include<stdio.h>
#include<stdlib.h>
int sum(int a, int b);

int main()
{
    int a, b, c;

    a=2;
    b=3;
    c=sum(a,b);
    printf("%d", c);
}

int sum(int a, int b)
{
    return(a+b);
}
