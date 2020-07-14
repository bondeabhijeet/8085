#include<stdio.h>
#include<string.h>

#define LABEL_LENGTH 11
#define MNEMONIC_LENGTH 9
#define OPERAND1_LENGTH 11
#define OPERAND2_LENGTH 11
#define OPCODE_LENGTH 2
#define LABELVALUE_LENGTH 4

struct PROG // PROG stands for program
{
    char Label[LABEL_LENGTH + 1];
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
};


int main(void)
{
    struct PROG SourceInst;
    FILE *fp3;
    unsigned int LC;
    int i, counter, InstType;
    char OpCode[3], Comment[51];

    fp3 = fopen( "Pass1Output.txt", "r" );

    while( !feof( fp3 ) )
    {

        fscanf( fp3, "%X", &LC );
        fscanf( fp3, "%s", SourceInst.Label );
        fscanf( fp3, "%s", SourceInst.Mnemonic);
        fscanf( fp3, "%s", SourceInst.Operand1 );
        fscanf( fp3, "%s", SourceInst.Operand2 );
        fscanf( fp3, "%s", OpCode );
        fscanf( fp3, "%d", &InstType );
        fscanf( fp3, " %[^\n]\n", Comment);



        printf( "%04X    ", LC );
        printf( "%-15s", SourceInst.Label );
        printf( "%-12s", SourceInst.Mnemonic  );
        printf( "%-15s", SourceInst.Operand1);
        printf( "%-15s", SourceInst.Operand2 );
        printf( "%-5s", OpCode );
        printf( "%-8d", InstType );
        printf( "%s", Comment);

        printf("\n");
    }
    fclose( fp3 );
    return(0);
}

/*
        printf( "%04X    ", P1OutputInst.LC );
        printf( "%-15s", P1OutputInst.Label );
        printf( "%-12s", P1OutputInst.Mnemonic  );
        printf( "%-15s", P1OutputInst.Operand1 );
        printf( "%-15s", P1OutputInst.Operand2 );
        printf( "%-5s", P1OutputInst.OpCode );
        printf( "%-8d", P1OutputInst.IType );
        printf( "%s\n", P1OutputInst.Comment );
*/


/*
int ProcessMachineInstType2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries )
{
    int SValue;

    SValue = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries );

    /// TO B MODIFIED

    fprintf( P2OutputFP, "%04X ", P1OutputInst.LC );         // Printing Location Counter or Address
    fprintf( P2OutputFP, "%-5s ", P1OutputInst.OpCode );     // Printing OpCode

    if( SValue == ( -1 ) )
    {
        fprintf( P2OutputFP, "%-8s", " " );  // For absence Operand 1
    }
    else
    {
        fprintf( P2OutputFP, "%04X", SValue );                      // For absence Operand 1
    }
    //fprintf( P2OutputFP, "%-8s", " " );                      // For absence Operand 1
    fprintf( P2OutputFP, "%-8s", " " );                      // For absence Operand 2

    /// Assembling and printing Source Instruction-------------------------------------------------------------

    if( P1OutputInst.Label[0] != '?')                        // If label is present
    {
        strcat( P1OutputInst.Label, ":");                    // Appending ":"
        fprintf( P2OutputFP, "%-15s", P1OutputInst.Label);   // Printing label
    }
    else                                                     // If label is absent
    {
        fprintf( P2OutputFP, "%-15s", " " );                 // Printing blank spaces
    }

    fprintf( P2OutputFP, "%-12s", P1OutputInst.Mnemonic );   // Printing Mnemonic
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );                     // Printing Operand 1
    fprintf( P2OutputFP, "%-15s", " " );                     // Printing Operand 2

    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );     // Printing Comment
}
*/
