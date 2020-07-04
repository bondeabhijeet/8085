#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define N 247

char *DeleteComment(char str[]);
char *DeleteExcessWhiteSpaces(char str[]);
int GetFields(char Instruction[]);
int ReadMachineOpTable();

struct Fields
{
    char Mnemonic[5];
    char Operand1[10];
    char Operand2[10];
    char OpCode[3];
    int Length;
    int Type;
}Inst[247];

int main()
{
    FILE* fp, fp1;
    int counter, i;
    char str[256], InstructionWithComment[256], Instruction[256];
    char *str2;  // For recieving purpose

    if((fp=fopen("InstructionSet.txt", "r"))==NULL)  // Opening a file to read the program
    {
        printf("\nError opening Instruction file\n");
        exit(0);
    }

    ReadMachineOpTable();

    fscanf(fp, " %[^\n]", str);  // Read the full sentence from the given file with comment
    printf("\n%s\n", str);

    str2 = DeleteExcessWhiteSpaces(str);  // Deletes all the extra spaces and tabs (no tabs will be present after execution of this function)
    strcpy(InstructionWithComment, str2);

    str2 = DeleteComment(str);  // In this function the comment will get deleted
    strcpy(Instruction, str2);
    printf("\n%s\n", Instruction);

    GetFields(Instruction);  // In this function the fields are separated

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
    return(str);
}

int GetFields(char Instruction[])  // The instruction is broken down into smaller tokens for analysis and differentiating whether it is a label or mnemonic or a operand
{
    int j, k;
    char Label[10], *p, Temp[256], *p1, *p2, Mnemonic[5], Operand1[10], Operand2[10];
    strcpy(Temp, Instruction);

    // Separating Label

    if(p = strchr(Temp, ':'))
    {
        *(p) = '\0';

        if(p = strchr(Temp, ' '))
        {
            *(p) = '\0';
        }
        strcpy(Label, Temp);
        printf("\n\nLabel:%s?\n", Label);
    }
    else
    {
        Label[0] = '\0';
    }

    // Separating mnemonics

    strcpy(Temp, Instruction);

    if(p1 = strchr(Temp, ':'))  // When Label is present
    {
        if(*(p1+1) == ' ')
        {
            p1 = (p1+2);
        }
        if(p2 = strchr(p1, ' '))
        {
            *(p2) = '\0';
        }
        strcpy(Mnemonic, p1);
        //printf("\n\nWhen Label is present:%s?\n", Mnemonic);
    }
    else  // When Label is absent
    {
        p1 = Temp;

        if(Temp[0] == ' ')
        {
            p1++;
        }
        if(p2 = strchr(p1, ' '))
        {
            *(p2) = '\0';
        }

        strcpy(Mnemonic, p1);
        //printf("%\n\nWhen Label is absent:%s?\n", Mnemonic);
    }


    // Separating operand 1


    strcpy(Temp, Instruction);

    if(p1 = strchr(Temp, ':'))  // When Label is present
    {
        if(*(p1+1) == ' ')
        {
            p1 = (p1+2);
        }
        if(p2 = strchr(p1, ' '))
        {
            p1 = (p2+1);
        }
        p2 = strtok(p1, " ,");

        if(p2 == NULL)
        {
            Operand1[0] = '\0';
        }
        else
        {
            strcpy(Operand1, p1);
        }

        // printf("\n\nOperand1:%s?\n", Operand1);
    }
    else  // When Label is absent
    {
        p1 = Temp;

        if(Temp[0] == ' ')
        {
            p1++;
        }
        if(p2 = strchr(p1, ' '))
        {
            p1 = (p2+1);
        }
        p2 = strtok(p1, " ,");

        if(p2 == NULL)
        {
            Operand1[0] = '\0';
        }
        else
        {
            strcpy(Operand1, p1);
        }

        // printf("%\n\nOperand1q:%s?\n", Operand1);
    }

    // Separating operand 2

    strcpy(Temp, Instruction);

    if(p1 = strchr(Temp, ','))
    {
        if(*(p1+1) == ' ')
        {
            p1 = (p1 +2);
        }
        if(p2 = strchr(p1, ' '))
        {
            *(p2) = '\0';
        }
        strcpy(Operand2, p1);
       // printf("\n\nOperand2:%sh\n", Operand2);
    }
    else
    {
        Operand2[0] = '\0';
    }


    printf("\nFINAL:\n");
    printf("\n#Label:%s#\n", Label);
    printf("\n#Mnemonic:%s#\n", Mnemonic);
    printf("\n#Operand1:%s#\n", Operand1);
    printf("\n#Operand2:%s#\n", Operand2);
}

int ReadMachineOpTable()
{
    FILE *fp;

    int i;

    if((fp=fopen("MachineOpTable.txt", "r")) == NULL)
    {
        printf("\n Error opening the text file named Test...\n Exiting....\n");
    }


    for(i=1; i<= N; ++i)
    {
        fscanf(fp, "%s", Inst[i].Mnemonic);
        fscanf(fp, "%s", Inst[i].Operand1);
        fscanf(fp, "%s", Inst[i].Operand2);
        fscanf(fp, "%s", Inst[i].OpCode);
        fscanf(fp, "%d", &Inst[i].Length);
        fscanf(fp, "%d", &Inst[i].Type);

        printf("%s", Inst[i].Mnemonic);
        printf("%s", Inst[i].Operand1);
        printf("%s", Inst[i].Operand2);
        printf("%s", Inst[i].OpCode);
        printf("%d", Inst[i].Length);
        printf("%d\n", Inst[i].Type);
    }

}

/*ReadMachineOpCode(void)
{
    char str[256];
    struct
    if((fp1=fopen("MachineOpTable.txt","r"))==NULL)  // Opening a file to read the Machine-Op Code
    {
        printf("\nError opening Machine-op code file\n");
    }

    while(!feof(fp1))
    {

    }
    fscanf(fp1, " %[^\n]", str);

    if(str == "NULL")
    fclose(fp1);
}*/

