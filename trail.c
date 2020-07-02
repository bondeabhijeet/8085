
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char *DeleteComment(char str[]);
char *DeleteExcessWhiteSpaces(char str[]);
//int GetFields(char Instruction[]);

struct fields
{
    char label[8];
    char mnemonic[4];
    char operand1[4];
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

    fscanf(fp, " %[^\n]", str);  // Read the full sentence from the given file with comment
    printf("\n%s\n", str);

    str2 = DeleteExcessWhiteSpaces(str);  // Deletes all the extra spaces and tabs (no tabs will be present after execution of this function)
    strcpy(InstructionWithComment, str2);

    str2 = DeleteComment(str);  // In this function the comment will get deleted
    strcpy(Instruction, str2);
    printf("\n%sh\n", Instruction);

    //GetFields(Instruction);  // In this function the fields are separated

    fclose(fp);
}

char *DeleteComment(char str[])  // wherever there is ; or // they will be replaced with \0 to mark the end of string thus deleting the comment
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

char *DeleteExcessWhiteSpaces(char str[])  // All the extra tabs and spaces will get deleted. wherever there are more than 1 space, it will be deleted and the tabs will be replaced by spaces.
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
    printf("\nTEST:%sh\n", str1);
    return(str);
}


