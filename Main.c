#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define N 247

/*ERROR CODES:
300: SYNTAX ERROR IN MNEMONIC....
400: SYNTAX ERROR IN OPERAND1....
500: SYNTAX ERROR IN OPERAND1 OR OPERAND2*/

struct MOT  // For MachineOp Table
{
    char Mnemonic[5];
    char Operand1[10];
    char Operand2[10];
    char OpCode[3];
    int Length;
    int Type;
};

struct PROG // PROG stands for program
{
    char Label[10];
    char Mnemonic[5];
    char Operand1[10];
    char Operand2[10];
};


char *DeleteComment(char str[]);
char *DeleteExcessWhiteSpaces(char str[]);
struct PROG GetFields(char Instruction[]);
int ReadMachineOpTable();
int MOTGetPass1(struct MOT MOTInst[], struct PROG SourceInst);


int main()
{
    FILE* fp, fp1;
    int counter, i, MOTIndex;
    struct MOT MOTInst[N];
    struct PROG SourceInst;
    char str[256], InstructionWithComment[256], Instruction[256];
    char *str2;  // For recieving purpose

    if((fp=fopen("InstructionSet.txt", "r"))==NULL)  // Opening a file to read the program
    {
        printf("\nError opening Instruction file\n");
        exit(0);
    }

    ReadMachineOpTable(MOTInst);

    fscanf(fp, " %[^\n]", str);  // Read the full sentence from the given file with comment
    printf("\n%s\n", str);

    str2 = DeleteExcessWhiteSpaces(str);  // Deletes all the extra spaces and tabs (no tabs will be present after execution of this function)
    strcpy(InstructionWithComment, str2);

    str2 = DeleteComment(str);  // In this function the comment will get deleted
    strcpy(Instruction, str2);
    printf("\n%s\n", Instruction);

    SourceInst = GetFields(Instruction);  // In this function the fields are separated*/

    MOTIndex = MOTGetPass1(MOTInst, SourceInst);
    if(MOTIndex == 300)
    {
        printf("\nSYNTAX ERROR IN MNEMONIC....\n");
    }
    if(MOTIndex == 400)
    {
        printf("\nSYNTAX ERROR IN OPERAND1....\n");
    }
    if(MOTIndex == 500)
    {
        printf("\nSYNTAX ERROR IN OPERAND1 OR OPERAND2\n");
    }

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

struct PROG GetFields(char Instruction[])  // The instruction is broken down into smaller tokens for analysis and differentiating whether it is a label or mnemonic or a operand
{
    int j, k;
    char Label[10], *p, Temp[256], *p1, *p2, Mnemonic[5], Operand1[10], Operand2[10];
    struct PROG TempInst;
    strcpy(Temp, Instruction);

    // Separating Label

    if(p = strchr(Temp, ':'))
    {
        *(p) = '\0';

        if(p = strchr(Temp, ' '))
        {
            *(p) = '\0';
        }
        strcpy(TempInst.Label, Temp);
        printf("\n\nLabel:%s?\n", TempInst.Label);
    }
    else
    {
        TempInst.Label[0] = '\0';
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
        strcpy(TempInst.Mnemonic, p1);
        printf("\n\nWhen Label is present:%s?\n", TempInst.Mnemonic);
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

        strcpy(TempInst.Mnemonic, p1);
        printf("%\n\nWhen Label is absent:%s?\n", TempInst.Mnemonic);
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
            TempInst.Operand1[0] = '\0';
        }
        else
        {
            strcpy(TempInst.Operand1, p1);
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
            TempInst.Operand1[0] = '\0';
        }
        else
        {
            strcpy(TempInst.Operand1, p1);
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
        strcpy(TempInst.Operand2, p1);
       printf("\n\nOperand2:%sh\n", TempInst.Operand2);
    }
    else
    {
        TempInst.Operand2[0] = '\0';
    }


    printf("\nFINAL:\n");
    printf("\n#Label:%s#\n", TempInst.Label);
    printf("\n#Mnemonic:%s#\n", TempInst.Mnemonic);
    printf("\n#Operand1:%s#\n", TempInst.Operand1);
    printf("\n#Operand2:%s#\n", TempInst.Operand2);

    return(TempInst);
}

int ReadMachineOpTable(struct MOT MOTInst[])
{
    FILE *fp;

    int i;

    if((fp=fopen("MachineOpTable 8085A.txt", "r")) == NULL)
    {
        printf("\n Error opening the text file named MachineOpTable 8085A...\n Exiting....\n");
        exit(0);
    }


    for(i=1; i<N; ++i)
    {
        fscanf(fp, "%s", MOTInst[i].Mnemonic);
        fscanf(fp, "%s", MOTInst[i].Operand1);
        fscanf(fp, "%s", MOTInst[i].Operand2);
        fscanf(fp, "%s", MOTInst[i].OpCode);
        fscanf(fp, "%d", &MOTInst[i].Length);
        fscanf(fp, "%d", &MOTInst[i].Type);

        /*printf("%s", MOTInst[i].Mnemonic);
        printf("%s", MOTInst[i].Operand1);
        printf("%s", MOTInst[i].Operand2);
        printf("%s", MOTInst[i].OpCode);
        printf("%d", MOTInst[i].Length);
        printf("%d\n", MOTInst[i].Type);*/
    }

}

int MOTGetPass1(struct MOT MOTInst[], struct PROG SourceInst)
{
    int i, index;
    char MOTStr[25], SourceStr[25];

    for(i=1; i<N; ++i)
    {
        if(!(strcmp(MOTInst[i].Mnemonic, SourceInst.Mnemonic)))
        {
            break;
        }
    }

    if(i == N)
    {
        //printf("\nSYNTAX ERROR IN MNEMONIC....\n");
        return(300);
    }

    // If the instruction is of type 1, 3, 4, 5 ie. only Mnemonic
    if(MOTInst[i].Type == 1 || MOTInst[i].Type == 3 || MOTInst[i].Type == 4 || MOTInst[i].Type == 5)
    {
        printf("TYPE:%d", MOTInst[i].Type);
        return(i);
    }

    // If the Instruction if of type 2, 6, 8, 9 ie. Mnemonic with operand1
    if(MOTInst[i].Type == 2 || MOTInst[i].Type == 6 || MOTInst[i].Type == 8 || MOTInst[i].Type == 9)
    {
        // Making SourceStr by appending Operand1 to Mnemonic
        strcpy(SourceStr, SourceInst.Mnemonic);
        strcat(SourceStr, SourceInst.Operand1);
        printf("\n%s\n", SourceStr);

        //Searching MOT
        for(i=1; i<N; ++i)
        {
            // Making MOTStr by appending Operand1 to Mnemonic
            strcpy(MOTStr, MOTInst[i].Mnemonic);
            strcat(MOTStr, MOTInst[i].Operand1);

            if(!strcmp(SourceStr, MOTStr))
            {
                //printf("\nMATCH FOUND....EXACT INDEX:%d\n", i);
                break;
            }
        }
        if(i == N)
        {
            return(400);
        }

    }

    // If the Instruction is of type 7 ie.Mnemonic With Operand1 and Operand2
    if(MOTInst[i].Type == 7)
    {
        //Making SourceStr by appending Operand1 and Operand2 to Mnemonic
        strcpy(SourceStr, SourceInst.Mnemonic);
        strcat(SourceStr, SourceInst.Operand1);
        strcat(SourceStr, SourceInst.Operand2);
        printf("\n%s\n", SourceStr);

        //Searching MOT
        for(i=1; i<N; ++i)
        {
            //Making MOTStr by appending Operand1 and Operand2 to Mnemonic
            strcpy(MOTStr, MOTInst[i].Mnemonic);
            strcat(MOTStr, MOTInst[i].Operand1);
            strcat(MOTStr, MOTInst[i].Operand2);


            if(!strcmp(SourceStr, MOTStr))
            {
                //printf("MATCH FOUND 2.... EXACT INDEX:%d\n", i);
                break;
            }
        }
        if(i == N)
        {
            return(500);
        }
    }
    return(i);
}
