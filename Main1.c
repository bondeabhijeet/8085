#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define N 247

/*ERROR CODES:
1000: SYNTAX ERROR IN MNEMONIC....
1001: SYNTAX ERROR IN OPERAND1....
1002: SYNTAX ERROR IN OPERAND1 OR OPERAND2*/

struct MOT  // For MachineOp Table
{
    char Mnemonic[10];
    char Operand1[12];
    char Operand2[12];
    char OpCode[3];
    int Length;
    int Type;
};

struct POT // For Pseud Op Table
{
    char PseudoOp[10];
};

struct PROG // PROG stands for program
{
    char Label[12];
    char Mnemonic[10];
    char Operand1[12];
    char Operand2[12];
};

char *DeleteComment(char str[]);
char *DeleteExcessWhiteSpaces(char str[]);
struct PROG GetFields(char Instruction[]);
int ReadMachineOpTable();
void ReadPseudoOpTable(struct POT POTInst[]);
int MOTGetPass1(struct MOT MOTInst[], struct PROG SourceInst);
void DisplayError(unsigned int ErrorCode, unsigned int LN);
void STSTO(FILE *fp2, char Label[], unsigned int LC);
int IsPseudoOp(char Mnemonic[], struct POT POTInst[]);


int main()
{
    FILE *fp, *fp2;
    unsigned int counter, i, MOTIndex, e;
    unsigned int LC, LN;
    struct MOT MOTInst[N];
    struct POT POTInst[N];
    struct PROG SourceInst;
    char str[256], InstructionWithComment[256], Instruction[256];
    char *str2;  // For recieving purpose

    // Preparing MachineOp Table
    ReadMachineOpTable(MOTInst);

    ReadPseudoOpTable(POTInst);

    if((fp=fopen("SourceProgram.txt", "r"))==NULL)  // Opening a file to read the program
    {
        printf("\nError opening Instruction file\n");  //error
        exit(0);
    }

    if((fp2 = fopen("SymbolTable.txt","w")) == NULL)
    {
        printf("Error opening symbol Table\n");  //error
        exit(0);
    }


    // Starting pass 1 : preparing symbol table
    LC = 2000; // Assuming program will start at 2000 location
    LN = 1;

    while(!feof(fp))
    {
        fscanf(fp, " %[^\n]", str);  // Read the full sentence from the given file with comment
        //printf("\n%s\n", str);

        str2 = DeleteExcessWhiteSpaces(str);  // Deletes all the extra spaces and tabs (no tabs will be present after execution of this function)
        strcpy(InstructionWithComment, str2);

        str2 = DeleteComment(str);  // In this function the comment will get deleted
        strcpy(Instruction, str2);
        //printf("\n%s\n", Instruction);

        SourceInst = GetFields(Instruction);  // In this function the fields are separated*/

        //  process pseudo Op

        if(e=IsPseudoOp(SourceInst.Mnemonic, POTInst))
        {
            printf("\nPseudoOp Found...:%d\n", e);
            exit(0);
            // POTGET()
        }

        MOTIndex = MOTGetPass1(MOTInst, SourceInst);

        if(MOTIndex >= 1000)
        {
            DisplayError(MOTIndex, LN);
        }


        if((SourceInst.Label[0])!= '\0')  // If label call STSTO
        {
            STSTO(fp2, SourceInst.Label, LC);
        }

        LC = LC + MOTInst[MOTIndex].Length;
        LN = LN + 1;
    }

    fclose(fp2);
    fclose(fp);
}

int IsPseudoOp(char Mnemonic[], struct POT POTInst[])  // Check for Pseudo Op
{
    int i;
    i = 1;
    while(POTInst[i].PseudoOp[0] != '\0')
    {
        if(!strcmp(Mnemonic, POTInst[i].PseudoOp))
        {
            return(i);
        }
        i = i + 1;
    }
    return(0);
}

void STSTO(FILE *fp2, char Label[], unsigned int LC)  // Symbol table storage
{
    fprintf(fp2, "%15s", Label);
    fprintf(fp2, "%6d", LC);
    fprintf(fp2, "\n");
}

void DisplayError(unsigned int ErrorCode, unsigned int LN)
{
    char ErrorMessages[][80] = {"SYNTAX ERROR IN MNEMONIC....",
                                "SYNTAX ERROR IN OPERAND1....",
                                "SYNTAX ERROR IN OPERAND2...."};

    printf("\nERROR IN LINE NO: %d : %s\n\n\a", LN, ErrorMessages[ErrorCode - 1000]);
    exit(0);

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
    char Label[12], *p, Temp[256], *p1, *p2, Mnemonic[10], Operand1[12], Operand2[12];
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
        if( strlen(Temp) > 11)  // Restricting label size to 11 characters
        {
            printf("\n WARNING: LABEL IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n");
            Temp[11] = '\0';
        }
        strcpy(TempInst.Label, Temp);
        //printf("\n\nLabel:%s?\n", TempInst.Label);
    }
    else  // If Label is not present so entering \0 in the label string
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
        else
        {
            p1 = (p1 + 1);
        }
        if(p2 = strchr(p1, ' '))
        {
            *(p2) = '\0';
        }
        strcpy(TempInst.Mnemonic, p1);
        //printf("\n\nWhen Label is present:%s?\n", TempInst.Mnemonic);
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
        //printf("%\n\nWhen Label is absent:%s?\n", TempInst.Mnemonic);
    }


    // Separating operand 1


    strcpy(Temp, Instruction);

    if(p1 = strchr(Temp, ':'))  // When Label is present
    {
        if(*(p1+1) == ' ')
        {
            p1 = (p1+2);
        }
        else
        {
            p1 = (p1 + 1);
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
            if( strlen(p1) > 11)  // Restricting Operand1 size to 11 characters
            {
                printf("\n WARNING: OPERAND1 IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n");
                Temp[11] = '\0';
            }
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
            if( strlen(p1) > 11)  // Restricting Operand1 size to 11 characters
            {
                printf("\n WARNING: OPERAND1 IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n");
                Temp[11] = '\0';
            }
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
        if( strlen(p1) > 11)  // Restricting Operand2 size to 11 characters
        {
            printf("\n WARNING: OPERAND2 IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n");
            Temp[11] = '\0';
        }
        strcpy(TempInst.Operand2, p1);
        //printf("\n\nOperand2:%sh\n", TempInst.Operand2);
    }
    else
    {
        TempInst.Operand2[0] = '\0';
    }


    /*printf("\nFINAL:\n");
    printf("\n#Label:%s#\n", TempInst.Label);
    printf("\n#Mnemonic:%s#\n", TempInst.Mnemonic);
    printf("\n#Operand1:%s#\n", TempInst.Operand1);
    printf("\n#Operand2:%s#\n", TempInst.Operand2);*/

    return(TempInst);
}

int ReadMachineOpTable(struct MOT MOTInst[])
{
    FILE *fp;

    int i;

    if((fp=fopen("MachineOpTable 8085A.txt", "r")) == NULL)
    {
        //error
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

    fclose(fp);

}

void ReadPseudoOpTable(struct POT POTInst[])
{
    FILE *fp;
    int i;

    if((fp = fopen("PseudoOpTable.txt", "r")) == NULL)
    {
        //error
        printf("\nERROR OPENING FILE....\n");
        exit(0);
    }

    i = 1;
    while(!feof(fp))
    {
        fscanf(fp, "%s", POTInst[i].PseudoOp);
        //printf("\nPSEUDOOP:%sh\n", POTInst[i].PseudoOp);
        i = i + 1;
    }
    POTInst[i].PseudoOp[0] = '\0';

    fclose(fp);
}

int MOTGetPass1(struct MOT MOTInst[], struct PROG SourceInst)
{
    int i, index;
    char MOTStr[100], SourceStr[100];

    for(i=1; i<N; ++i)
    {
        if(!(strcmp(MOTInst[i].Mnemonic, SourceInst.Mnemonic)))
        {
            //printf("%sh", SourceInst.Mnemonic);
            break;
        }
    }

    if(i == N)
    {
        //printf("\nSYNTAX ERROR IN MNEMONIC....\n");
        return(1000);
    }

    // If the instruction is of type 1, 3, 4, 5 ie. only Mnemonic
    if(MOTInst[i].Type == 1 || MOTInst[i].Type == 3 || MOTInst[i].Type == 4 || MOTInst[i].Type == 5)
    {
        //printf("TYPE:%d", MOTInst[i].Type);
        return(i);
    }

    // If the Instruction if of type 2, 6, 8, 9 ie. Mnemonic with operand1
    if(MOTInst[i].Type == 2 || MOTInst[i].Type == 6 || MOTInst[i].Type == 8 || MOTInst[i].Type == 9)
    {
        // Making SourceStr by appending Operand1 to Mnemonic
        strcpy(SourceStr, SourceInst.Mnemonic);
        strcat(SourceStr, SourceInst.Operand1);
        //printf("\n%s\n", SourceStr);

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
            return(1001);
        }

    }

    // If the Instruction is of type 7 ie.Mnemonic With Operand1 and Operand2
    if(MOTInst[i].Type == 7)
    {
        //Making SourceStr by appending Operand1 and Operand2 to Mnemonic
        strcpy(SourceStr, SourceInst.Mnemonic);
        strcat(SourceStr, SourceInst.Operand1);
        strcat(SourceStr, SourceInst.Operand2);
        //printf("\n%s\n", SourceStr);

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
            return(1002);
        }
    }
    return(i);
}


