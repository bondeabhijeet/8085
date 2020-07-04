#include<stdio.h>
#include<string.h>

struct Fields
{
    char Mnemonic[5];
    char Operand1[10];
    char Operand2[10];
    char OpCode[3];
    int Length;
    int Type;
}Inst[247];

int ReadMachineOpTable();

int main()
{

    ReadMachineOpTable();
}

int ReadMachineOpTable()
{
    FILE *fp;

    int i;

    if((fp=fopen("Test.txt", "r")) == NULL)
    {
        printf("\n Error opening the text file named Test...\n Exiting....\n");
    }


    for(i=1; i<= 247; ++i)
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




/*    fscanf(fp, "%s", str);
    printf("%s", str);
    fscanf(fp, "%s", str1);
    printf("%s", str1);
    fscanf(fp, "%s", str2);
    printf("%s", str2);
    fscanf(fp, "%s", str3);
    printf("%s", str3);
    fscanf(fp, "%d", &in);
    printf("%d", in);
    fscanf(fp, "%d", &in1);
    printf("%d\n", in1);*/



    fclose(fp);
}
