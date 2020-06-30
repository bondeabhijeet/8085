
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char *DeleteComment(char str[]);
char *DeleteExcessWhiteSpaces(char str[]);
int GetFields(char Instruction[]);

struct fields
{
    char label[8];
    char mnemonic[4];
    char operand1[2];
    char operand2[8];
};

int main()
{
    FILE* fp;
    int counter, i;
    char str[256], InstructionWithComment[256], Instruction[256];
    char *str2;  // For recieving purpose

    if((fp=fopen("InstructionSet.txt", "r"))==NULL)
    {
        printf("\nError Opening File\n");
        exit(0);
    }

    fscanf(fp, " %[^\n]", str);
    printf("\n%s\n", str);

    str2 = DeleteExcessWhiteSpaces(str);
    strcpy(InstructionWithComment, str2);

    str2 = DeleteComment(str);
    strcpy(Instruction, str2);
    printf("\n%s\n", Instruction);

    GetFields(Instruction);

    fclose(fp);
}

char *DeleteComment(char str[])
{
    int count;
    count = 0;
    while(str[count] != '\0')
    {

        if(str[count] == ';' || (str[count] == '/' && str[count+1] == '/'))
        {
            str[count] = '\0';
        }
        count++;
    }
    return(str);
}

char *DeleteExcessWhiteSpaces(char str[])
{
    int count, i;
    char str1[256], c;
    count = 0;
    while(str[count] != '\0')
    {
        if(str[count] == '\t')
        {
            str[count] = ' ';
        }
        count++;
    }

    count = 0;
    i = 0;


    while(str[count] != '\0')
    {

        if(str[count] == ' ')
        {

            while(str[count+1] == ' ')
            {
                count++;
            }
        }

        str1[i] = str[count];
        count++;
        i++;

    }
    str1[i] = '\0';
    strcpy(str,str1);
    return(str);
}

int GetFields(char Instruction[])
{
    int j, k;
    char colon, Label[9], *p, Temp[256];
    strcpy(Temp, Instruction);

    if(p = strchr(Temp, ':'))
    {
        *(p) = '\0';

        if(p = strchr(Temp, ' '))
        {
            *(p) = '\0';
        }
        strcpy(Label, Temp);
    }
}
