#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>
#include<ctype.h>

#define N 247  // No of entries in MOT are 246. we are using 1 to 246. We are not using zeroth location of the array.
#define LABEL_LENGTH 11
#define MNEMONIC_LENGTH 9
#define OPERAND1_LENGTH 11
#define OPERAND2_LENGTH 11
#define OPCODE_LENGTH 2
#define LABEL_VALUE_LENGTH 4     //  Maximum label value will be FFFF
#define COMMENT_LENGTH 50
#define MAX_NO_OF_ST_ENTRIES 200  // ST stands for Symbol Table

/*ERROR CODES:
1000: SYNTAX ERROR IN MNEMONIC....
1001: SYNTAX ERROR IN OPERAND1....
1002: SYNTAX ERROR IN OPERAND1 OR OPERAND2*/

struct MOT  // For MachineOp Table
{
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
    char OpCode[OPCODE_LENGTH + 1];
    int Length;
    int Type;
};

struct PROG // PROG stands for program
{
    char Label[LABEL_LENGTH + 1];
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
};

struct SYMBOLTABLEDEF
{
    char Symbol[LABEL_LENGTH + 1];
    char Value[LABEL_VALUE_LENGTH + 1];
};

struct DEFP1OUTPUT
{
    unsigned int LC;
    char Label[LABEL_LENGTH + 1];
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
    char OpCode[OPCODE_LENGTH + 1];
    int IType;
    char Comment[COMMENT_LENGTH + 1];
};


char *SplitInstructionAndComment( char str[], char Comment[] );
char *DeleteExcessWhiteSpaces( char str[] );
struct PROG GetFields( char Instruction[] );
int ReadMachineOpTable( struct MOT MOTInst[] );
int MOTGet_Pass1( struct MOT MOTInst[], char Mnemonic[], unsigned int LN );
void STSTO( FILE *fp2, struct PROG SourceInst, unsigned int LC );
int IsPseudoOp_Pass1( char Mnemonic[], char POT[][6] );
int ProcessPseudoOp_Pass1( int p, struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessORGorSTART_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2 , unsigned int LN );
int ProcessEQU_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDS_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDB_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDW_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessEND_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int CheckSymbolTableForDuplicates( void );
int GetComment( char InstructionWithComment[], char Comment[] );
int WritePass1OutputToFile( struct PROG SourceInst, unsigned int LC, char Comment[], FILE *fp3, int Type );
int Pass2( struct MOT MOTInst[], char POT[][6] );
int ReadSymbolTable( struct SYMBOLTABLEDEF ST[] );
struct DEFP1OUTPUT GetP1OutputInst( FILE *P1OutputFP );
int ProcessMachineInstType1( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessMachineInstType2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessMachineInstType3( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
int ProcessMachineInstType4( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
int STGet( char Operand1[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
int ProcessPseudoOp_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, unsigned int p, unsigned int LN );
int ProcessORGorSTART_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessEND_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessEQU_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessDS_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessDB_Pass2( struct DEFP1OUTPUT Temp, FILE *P2OutputFP );
int ProcessDW_Pass2( struct DEFP1OUTPUT Temp, FILE *P2OutputFP );
int ProcessMOType1( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], FILE *P2OutputFP, unsigned int LN );
int ProcessMachineOpPass2( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int ProcessMOType26( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int MOTGet_Pass2( struct MOT MOTInst[], char ConcatenatedString[], struct DEFP1OUTPUT P1OutputInst, unsigned int LN );
int ProcessMOType1( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], FILE *P2OutputFP, unsigned int LN );
int ProcessMOType34( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int ProcessMOType5( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int ProcessMOType7( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int ProcessMOType8( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int ProcessMOType9( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN );
int Pass1( struct MOT MOTInst[], char POT[][6] );
int Error( struct DEFP1OUTPUT P1OutputInst, unsigned int LN );
int ProcessLiteralPass2( char *Operand );
void SetColor( int ForgC );

int main()
{
    struct MOT MOTInst[N];
    char POT[][6] = {"ZZZZ", "DB", "DS", "DW", "END", "EQU", "ORG", "START", "\0"};  // remember that the zeroth element is dummy ( creating POT )

    ReadMachineOpTable( MOTInst );                                                   // Reading MachineOp Table from the text file and store it into a structure

    Pass1( MOTInst, POT );
    Pass2( MOTInst, POT );
}

int Pass1( struct MOT MOTInst[], char POT[][6] )
{
    FILE *fp, *fp2, *fp3;
    unsigned int counter, p;
    int MOTIndex, Length;
    unsigned int LC, LN;
    struct PROG SourceInst;
    char str[256], InstructionWithComment[256], Instruction[256], Comment[256];
    char *str2;  // For recieving purpose

    if( ( fp = fopen( "SourceProgram.txt", "r" ) ) == NULL )          // Opening a file ( read only ) to read the Source program
    {
        SetColor(12);
        printf( "\nError opening Source Instruction file \n\a" );     //error
        SetColor(0);
        exit(0);
    }

    if( ( fp2 = fopen( "SymbolTable.txt","w" ) ) == NULL )        // Opening a file ( write only ) to store the Symbol table
    {
        SetColor(12);
        printf( "Error opening symbol Table for writing purpose \n\a" );  //error
        SetColor(0);
        exit(0);
    }

    if( ( fp3 = fopen( "Pass1Output.txt","w" ) ) == NULL )        // Opening a file ( write only ) to store the result of Pass1 in file
    {
        SetColor(12);
        printf( "Error opening Pass1output for writing purpose \n\a" );  //error
        SetColor(0);
        exit(0);
    }


    /// Starting pass 1 : preparing symbol table

    LC = 0;                                                 // Assigning location counter to 2000
    LN = 1;                                                 // Assigning the line counter to 1

    while(!feof(fp))
    {
        fscanf( fp, " %[^\n]\n", str );                         // Read the full sentence from the given file with comment

        str2 = DeleteExcessWhiteSpaces( str );                // Deletes all the extra spaces and tabs ( no tabs will be present after execution of this function )
        strcpy( InstructionWithComment, str2 );               // Store the instruction in the InstructionWithComment variable

        str2 = SplitInstructionAndComment( str, Comment );    // Delete comment from the instruction
        strcpy( Instruction, str2 );                          // Store the instruction ( without comment and extra spaces ) ie. A proper instruction

        SourceInst = GetFields(Instruction);                // Separating the fields from the Instruction ie. Label, Mnemonic, Operand1, Operand2

        /// PASS 1 STARTS HERE

        /// Processing PseudoOps and MachineOps

        if( ( p = IsPseudoOp_Pass1( SourceInst.Mnemonic, POT ) ) )               // checking if the Mnemonic is pseudoOp if yes then ProcessPseudoOp_Pass1() will be executed
        {
            Length = ProcessPseudoOp_Pass1( p, SourceInst, &LC, fp2 , LN );
            WritePass1OutputToFile( SourceInst, LC, Comment, fp3, -99 );     // Storing the result of PASS1 in a file ( Pass1Output.txt ). Sending Dummy Instruction Type, as type is required only for MachineOp
            LC = LC + Length;
        }
        else                                                                 // If the Mnemonic is not a pseudoOp then it might be machine Op if yes then execute the following functions
        {
            MOTIndex = MOTGet_Pass1( MOTInst, SourceInst.Mnemonic, LN );              // Identify the index of the Mnemonic in MOT and store it in MOTIndex

            if( MOTIndex == N )                                                  // If syntax error in Mnemonic
            {
                SetColor( 12 );
                printf( "\n\n LINE NO:%d :: HAS SYNTAX ERROR IN MNEMONIC.... \n\n\a ", LN );     // error#1 in mnemonic
                SetColor( 0 );
                exit( 0 );
            }

            if( ( SourceInst.Label[0] )!= '\0' )                             // If label is present then call STSTO
            {
                STSTO(fp2, SourceInst, LC);                                  // Store the Label in symbol table file
            }

            WritePass1OutputToFile( SourceInst, LC, Comment, fp3, MOTInst[MOTIndex].Type );  // Storing the result of PASS1 in a file ( Pass1Output.txt )

            LC = LC + MOTInst[MOTIndex].Length;                             // Increment the location counter by the length of the MachineOp ( length is fetched from the MOT table
        }

        LN = LN + 1;                                                        // Increment the Line counter by 1

    }

    fclose( fp );                                                           // Closing all the files
    fclose( fp2 );
    fclose( fp3 );

    CheckSymbolTableForDuplicates( );                                        // Checking symbol table for any duplicate ( ie. repeated ) symbols

}

   /// MAIN ENDS HERE

int Pass2( struct MOT MOTInst[], char POT[][6] )
{
    int NoOfSTEntries, i, STIndex;
    FILE *P1OutputFP, *P2OutputFP;
    struct SYMBOLTABLEDEF ST[MAX_NO_OF_ST_ENTRIES];  // ST for Symbol Table
    struct DEFP1OUTPUT P1OutputInst;
    unsigned int p, LN;


    // Reading Symbol Table from file and store it in a structural array

    NoOfSTEntries = ReadSymbolTable( ST );

    if( ( P1OutputFP = fopen( "Pass1Output.txt", "r" ) ) == NULL )
    {
        SetColor( 12 );
        printf("\nERROR OPENING Pass1Output.txt FILE\n\n\a ");
        SetColor( 0 );
        exit( 0 );
    }

    if( ( P2OutputFP = fopen( "Pass2Output.txt", "w" ) ) == NULL )
    {
        SetColor( 12 );
        printf("\nERROR OPENING Pass2Output.txt FILE\n\n\a ");
        SetColor( 0 );
        exit( 0 );
    }

    LN = 1;
    while( !feof( P1OutputFP ) )
    {
        P1OutputInst = GetP1OutputInst( P1OutputFP );

        if( ( p = IsPseudoOp_Pass1( P1OutputInst.Mnemonic, POT ) ) )  // checking if the Mnemonic is pseudoOp if yes then ProcessPseudoOp_Pass1() will be executed
        {
            ProcessPseudoOp_Pass2( P1OutputInst, P2OutputFP, p, LN );
        }
        else
        {
            ProcessMachineOpPass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
        }

        LN = LN + 1;
    }

    fclose( P1OutputFP );
    fclose( P2OutputFP );
}

int ProcessMachineOpPass2( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    char Spaces11[] = "           ";
    //int MOTIndex;                           // MOTIndex variable is Local and different than the one used in Pass 1
    /*
     * If the fields of P1OutputInst ( ie. Label, Mnemonic, Operand1, Operand2, and Comment) are empty ( ie. indicated
     * by ? ) then place spaces in those fields in temp variable.
     */

    if( P1OutputInst.Label[0] == '?')              // If Label is not present
    {
        strcpy( P1OutputInst.Label, Spaces11 );    // Fill it with 11 blank spaces in the temp variable
        P1OutputInst.Label[11] = '\0';             // And terminate the Label string
    }
    if( P1OutputInst.Operand1[0] == '?')           // If Operand 1 is not present
    {
        strcpy( P1OutputInst.Operand1, Spaces11 ); // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Operand1[11] = '\0';          // And terminate the Operand 1 string
    }
    if( P1OutputInst.Operand2[0] == '?')           // If Operand 2 is not present
    {
        strcpy( P1OutputInst.Operand2, Spaces11 ); // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Operand2[11] = '\0';          // And terminate the Operand 2 string
    }
    if( P1OutputInst.Comment[0] == '?')            // If Operand 2 is not present
    {
        strcpy( P1OutputInst.Comment, Spaces11 );  // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Comment[11] = '\0';           // And terminate the Operand 2 string
    }

    if( P1OutputInst.IType == 1 )
    {
         ProcessMOType1( P1OutputInst, MOTInst, P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 2 || P1OutputInst.IType == 6 )
    {
        ProcessMOType26( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 3 || P1OutputInst.IType == 4 )
    {
        ProcessMOType34( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 5 )
    {
        ProcessMOType5( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 7 )
    {
        ProcessMOType7( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 8 )
    {
        ProcessMOType8( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 9 )
    {
        ProcessMOType9( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP, LN );
    }
}


int ProcessMOType9( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    int STIndex, MOTIndex;                                                        // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1], SourceOperand2[OPERAND2_LENGTH + 1];
    char HigherByte[3], LowerByte[3];

    strcpy( SourceOperand1, P1OutputInst.Operand1 );
    strcpy( SourceOperand2, P1OutputInst.Operand2 );

    if( ( STIndex = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    if( ( STIndex = STGet( P1OutputInst.Operand2, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand2, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }
    else                                                                          // Else it is a Literal then process Literal
    {
        ProcessLiteralPass2( &P1OutputInst.Operand2[0] );                         // Process Literal in Operand 1
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );

    P1OutputInst.Operand2[4] = '\0';                                            // Deleting H from the Operand 2
    HigherByte[0] = P1OutputInst.Operand2[0];
    HigherByte[1] = P1OutputInst.Operand2[1];
    HigherByte[2] = '\0';
    LowerByte[0] = P1OutputInst.Operand2[2];
    LowerByte[1] = P1OutputInst.Operand2[3];
    LowerByte[2] = '\0';

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", LowerByte );      // Put Operand 1 in P2Output File

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 2 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", HigherByte );      // Put Operand 1 in P2Output File
}


int ProcessMOType8( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    int STIndex, MOTIndex;                                                        // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1], SourceOperand2[OPERAND2_LENGTH + 1];

    strcpy( SourceOperand1, P1OutputInst.Operand1 );
    strcpy( SourceOperand2, P1OutputInst.Operand2 );

    if( ( STIndex = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    if( ( STIndex = STGet( P1OutputInst.Operand2, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand2, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }
    else                                                                          // Else it is a Literal then process Literal
    {
        ProcessLiteralPass2( &P1OutputInst.Operand2[0] );                         // Process Literal in Operand 1
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );

    P1OutputInst.Operand2[2] = '\0';                                            // Deleting H from the Operand 2

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", P1OutputInst.Operand2 );      // Put Operand 1 in P2Output File
}


int ProcessMOType7( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1], SourceOperand2[OPERAND2_LENGTH + 1];

    strcpy( SourceOperand1, P1OutputInst.Operand1 );
    strcpy( SourceOperand2, P1OutputInst.Operand2 );

    if( ( STIndex = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    if( ( STIndex = STGet( P1OutputInst.Operand2, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand2, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );
    strcat( ConcatenatedString, P1OutputInst.Operand2 );

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File
}


int ProcessMOType5( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    int STIndex, MOTIndex;                                                       // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], HigherByte[3], LowerByte[3], SourceOperand1[OPERAND1_LENGTH + 1];

    strcpy( SourceOperand1, P1OutputInst.Operand1 );

    if( ( STIndex = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }
    else                                                                          // Else it is a Literal then process Literal
    {
        ProcessLiteralPass2( &P1OutputInst.Operand1[0] );                         // Process Literal in Operand 1
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );
    HigherByte[0] = P1OutputInst.Operand1[0];
    HigherByte[1] = P1OutputInst.Operand1[1];
    HigherByte[2] = '\0';
    LowerByte[0] = P1OutputInst.Operand1[2];
    LowerByte[1] = P1OutputInst.Operand1[3];
    LowerByte[2] = '\0';


    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", LowerByte );      // Put Operand 1 in P2Output File

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 2 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", HigherByte );      // Put Operand 1 in P2Output File

}

int ProcessMOType34( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1];

    strcpy( SourceOperand1, P1OutputInst.Operand1 );

    if( ( STIndex = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }
    else                                                                          // Else it is a Literal then process Literal
    {
        ProcessLiteralPass2( &P1OutputInst.Operand1[0] );                         // Process Literal in Operand 1
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", P1OutputInst.Operand1 );      // Put Operand 1 in P2Output File

}

int ProcessLiteralPass2( char *Operand )
{
    int i;
    char Op[OPERAND1_LENGTH + 1];

    strcpy( Op, Operand );

    if( Op[0] == '=' )
    {
        i = 1;
        while( Op[i] != '\0' )
        {
            if( Op[i] != ' ' )
            {
                break;
            }
            else
            {
                i = i + 1;
            }
        }
        strcpy( Operand, &Op[i] );

        i = 0;

        while( Operand[i] != '\0' )
        {
            if( Operand[i] == 'h' || Operand[i] == 'H' )
            {
                Operand[i] = '\0';
            }
            else
            {
                i = i + 1;
            }
        }
    }
}


int ProcessMOType26( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP, unsigned int LN )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1];

    strcpy( SourceOperand1, P1OutputInst.Operand1 );

    if( ( STIndex = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );                          // Concatenating P1OutputInst.Operand1 and ConcatenatedString

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File
    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

}

///****************************************************************************************************************************************************************************************************************
int Error( struct DEFP1OUTPUT P1OutputInst, unsigned int LN )
{
    int i, j, Flag, n;

    if( P1OutputInst.IType == 2 )
    {
        char Possibility2[][4] = { "A", "B", "C", "D", "E", "H", "L", "M", "SP", "PSW" };

        for( i=0; i<=9; ++i)
        {
            if( !strcmp( P1OutputInst.Operand1, Possibility2[i] ) )
            {
                break;
            }
        }
        if( i == 10 )
        {
            SetColor( 12 );
            printf("\n ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\a", LN );
            printf("\n POSSIBLE REGISTERS SHOULD BE: A, B, C, D, E, H, L, M, SP, PSW \n");
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.IType == 3 || P1OutputInst.IType == 4 || P1OutputInst.IType == 5 )
    {
        Flag = 0;
        n = strlen( P1OutputInst.Operand1 );

        for(i=0; i<n; ++i)
        {
            if( !isxdigit(P1OutputInst.Operand1[i] ) )
            {
                Flag = 1;
            }
        }

        if( Flag == 1 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN LABEL OF OPERAND 1\n\n\a", LN );
            printf("\n POSSIBLY LABEL IN OPERAND 2 MAYBE MISSPELLED OR LITERAL SHOULD BE WRITTEN AS ' =XXH ' OR ' =XXXXH ' \n");
            SetColor( 0 );
            exit(0);
        }
    }
    else if( P1OutputInst.IType == 6 )
    {
        char Possibility6[][2] = { "0", "1", "2", "3", "4", "5", "6", "7" };

        for( i=0; i<=7; ++i)
        {
            if( !strcmp( P1OutputInst.Operand1, Possibility6[i] ) )
            {
                break;
            }
        }
        if( i == 8 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1 ( SOUULD BE IN BETWEEN RST 0 TO RST 7 )\n\n\a", LN );
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.IType == 7 )
    {
        char Possibility7[][2] = { "A", "B", "C", "D", "E", "H", "L", "M" };

        for( i=0; i<=7; ++i)
        {
            if( !strcmp( P1OutputInst.Operand1, Possibility7[i] ) )
            {
                break;
            }
        }
        if( i == 8 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\n\a", LN );
            printf("\n POSSIBLE REGISTERS SHOULD BE: A, B, C, D, E, H, L, M \n");
            SetColor( 0 );
            exit( 0 );
        }

        for( j=0; j<=7; ++j)
        {
            if( !strcmp( P1OutputInst.Operand2, Possibility7[j] ) )
            {
                break;
            }
        }
        if( j == 8 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 2\n\n\a", LN );
            printf("\n POSSIBLE REGISTERS SHOULD BE: A, B, C, D, E, H, L, M \n");
            SetColor( 0 );
            exit( 0 );
        }

        if( i == 7 && j == 7 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN BOTH OPERANDS ( BOTH SHOULD NOT BE M )\n\n\a", LN );
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.IType == 8 )
    {
        char Possibility8[][2] = { "A", "B", "C", "D", "E", "H", "L", "M" };

        for( i=0; i<=7; ++i)
        {
            if( !strcmp( P1OutputInst.Operand1, Possibility8[i] ) )
            {
                break;
            }
        }
        if( i == 8 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\n\a", LN );
            printf("\n POSSIBLE REGISTERS SHOULD BE: A, B, C, D, E, H, L, M \n");
            SetColor( 0 );
            exit( 0 );
        }

        Flag = 0;
        n = strlen( P1OutputInst.Operand2 );

        for(i=0; i<n; ++i)
        {
            if( !isxdigit(P1OutputInst.Operand2[i] ) )
            {
                Flag = 1;
            }
        }

        if( Flag == 1 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN LABEL OF OPERAND 2\n\n\a", LN );
            printf("\n POSSIBLY LABEL IN OPERAND 2 MAYBE MISSPELLED OR LITERAL SHOULD BE WRITTEN AS ' =XXH ' \n");
            SetColor( 0 );
            exit(0);
        }

    }
    else if( P1OutputInst.IType == 9 )
    {
        char Possibility9[][3] = { "B", "D", "H", "SP" };

        for( i=0; i<=3; ++i)
        {
            if( !strcmp( P1OutputInst.Operand1, Possibility9[i] ) )
            {
                break;
            }
        }
        if( i == 4 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\n\a", LN );
            printf("\n POSSIBLE REGISTERS SHOULD BE: B, D, H, SP \n");
            SetColor( 0 );
            exit( 0 );
        }

        Flag = 0;
        n = strlen( P1OutputInst.Operand2 );

        for(i=0; i<n; ++i)
        {
            if( !isxdigit(P1OutputInst.Operand2[i] ) )
            {
                Flag = 1;
            }
        }

        if( Flag == 1 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN LABEL OF OPERAND 2\n\n\a", LN );
            printf("\n POSSIBLY LABEL IN OPERAND 2 MAYBE MISSPELLED OR LITERAL SHOULD BE WRITTEN AS ' =XXXXH ' \n");
            SetColor( 0 );
            exit(0);
        }
    }

}

int MOTGet_Pass2( struct MOT MOTInst[], char ConcatenatedString[], struct DEFP1OUTPUT P1OutputInst, unsigned int LN )
{
    int i;
    char ConCatinatedMOTString[20];

    Error( P1OutputInst, LN );

    //Searching MOT

    for( i=1; i<N; ++i )
    {
        if( P1OutputInst.IType == 1 || P1OutputInst.IType == 3 || P1OutputInst.IType == 4 || P1OutputInst.IType == 5 )
        {
            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
        }
        else if( P1OutputInst.IType == 2 || P1OutputInst.IType == 6 )
        {
            // Making ConCatinatedMOTString by appending Operand1 to Mnemonic

            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
        }
        else if( P1OutputInst.IType == 7 )
        {
            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand2 );
        }
        else if( P1OutputInst.IType == 8 )
        {
            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
        }
        else if( P1OutputInst.IType == 9 )
        {
            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
        }


        if( !strcmp( ConCatinatedMOTString, ConcatenatedString ) )
        {
            break;
        }
    }

    return ( i );
}

int ProcessMOType1( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], FILE *P2OutputFP, unsigned int LN )
{
    int MOTIndex;               // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20];

    // Sending the Mnemonic for searching MOT and getting MOTIndex

    //MOTIndex  = MOTGet_Pass1( MOTInst, P1OutputInst.Mnemonic, LN );     // Sending the Mnemonic for searching process ( Searching only on Mnemonic )

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                  // Making the SearchString ( Searching only on Mnemonic )

    MOTIndex = MOTGet_Pass2( MOTInst, ConcatenatedString, P1OutputInst, LN );  // Sending the Mnemonic for searching process ( Searching only on Mnemonic )
    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File
    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File
}




int ProcessPseudoOp_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, unsigned int p, unsigned int LN )
{
    struct DEFP1OUTPUT Temp;
    char Spaces11[] = "           ";

    Temp = P1OutputInst;         // Copying the structure to temp variable for Pseudo Op processing

    /*
     * If the fields of P1OutputInst ( ie. Label, Mnemonic, Operand1, Operand2, and Comment) are empty ( ie. indicated
     * by ? ) then place spaces in those fields in temp variable.
     */

    if( P1OutputInst.Label[0] == '?')              // If Label is not present
    {
        strcpy( P1OutputInst.Label, Spaces11 );    // Fill it with 11 blank spaces in the temp variable
        P1OutputInst.Label[11] = '\0';             // And terminate the Label string
    }
    if( P1OutputInst.Operand1[0] == '?')           // If Operand 1 is not present
    {
        strcpy( P1OutputInst.Operand1, Spaces11 ); // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Operand1[11] = '\0';          // And terminate the Operand 1 string
    }
    if( P1OutputInst.Operand2[0] == '?')           // If Operand 2 is not present
    {
        strcpy( P1OutputInst.Operand2, Spaces11 ); // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Operand2[11] = '\0';          // And terminate the Operand 2 string
    }
    if( P1OutputInst.Comment[0] == '?')            // If Operand 2 is not present
    {
        strcpy( P1OutputInst.Comment, Spaces11 );  // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Comment[11] = '\0';           // And terminate the Operand 2 string
    }

    if( p == 1)
    {
        ProcessDB_Pass2( P1OutputInst, P2OutputFP );
    }
    else if( p == 2 )
    {
        ProcessDS_Pass2( P1OutputInst, P2OutputFP );
        // Code of this function is same as that of ProcessEND_Pass2
    }
    else if( p == 3)
    {
        ProcessDW_Pass2( P1OutputInst, P2OutputFP );
    }
    else if( p == 4 )
    {
        ProcessEND_Pass2( P1OutputInst, P2OutputFP );
    }
    else if( p == 5 )
    {
        ProcessEQU_Pass2( P1OutputInst, P2OutputFP );
    }
    else if( ( p == 6 ) || ( p == 7 ) )
    {
        ProcessORGorSTART_Pass2( P1OutputInst, P2OutputFP );
    }
}

int ProcessDW_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    int i;
    char LowerByte[3], HigherByte[3];       // For storing higher and lower byte

    i = 0;                                  // Initializing the counter

    while( P1OutputInst.Operand1[i] != '\0' )       // Till \0
    {
        if( P1OutputInst.Operand1[i] == 'h' || P1OutputInst.Operand1[i] == 'H' )   // If there is h or H
        {
            P1OutputInst.Operand1[i] = '\0';                               // Terminate the string there
        }

        i = i + 1;                          // Increment the counter
    }

    HigherByte[0] = P1OutputInst.Operand1[0];        // Copy the lower byte's first character into LowerByte
    HigherByte[1] = P1OutputInst.Operand1[1];        // Copy the lower byte's Second character into LowerByte
    HigherByte[2] = '\0';                    // End the LowerByte string
    LowerByte[0] = P1OutputInst.Operand1[2];       // Copy the higher byte's first character into HigherByte
    LowerByte[1] = P1OutputInst.Operand1[3];       // Copy the higher byte's second character into HigherByte
    LowerByte[2] = '\0';                   // End the HigherByte string

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Print the location counter of the LowerByte in the file
    fprintf( P2OutputFP, "%-4s  ", LowerByte );        // Print the LowerByte in the file

    //fprintf( P2OutputFP, "%-4s", " " );               // Blank spaces for Operand 2
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC + 1 );      // Print the location counter of the HigherByte in the file
    fprintf( P2OutputFP, "%-4s  \n", HigherByte );     // Print the HigherByte in the file


}

int ProcessDB_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    int i;

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );

    i = 0;

    while( P1OutputInst.Operand1[i] != '\0' )
    {
        if( P1OutputInst.Operand1[i] == 'h' || P1OutputInst.Operand1[i] == 'H' )
        {
            P1OutputInst.Operand1[i] = '\0';
        }

        i = i + 1;
    }

    fprintf( P2OutputFP, "%s", P1OutputInst.Operand1 );
    fprintf( P2OutputFP, "%-4s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File
}

int ProcessDS_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    int Opernad1, i;

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put LC in P2Output File
    fprintf( P2OutputFP, "%-6s", " " );                       // Put Blank spaces in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File

    Opernad1 = atoi( P1OutputInst.Operand1 );                  // Determine the integer value of Operand1 as we need to print value of LC that many Lines in P2Output File

    for( i=1; i<(Opernad1 ); ++i)
    {
        fprintf( P2OutputFP, "%04X  \n", (P1OutputInst.LC+i) ); // Put the LC in P2Output File with a increment of i
    }
}

int ProcessEQU_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%-12s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File
}

int ProcessEND_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%-12s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File
}

int ProcessORGorSTART_Pass2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%-12s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );       // Put Comment in P2Output File
}

int STGet( char Operand1[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries )
{
    int i;


    for (i=0; i<NoOfSTEntries; ++i)
    {
        if( !strcasecmp( Operand1, ST[i].Symbol ) )
        {
            //printf("%d ", i );
            return( i );
        }
    }

    return( -88 );
}


struct DEFP1OUTPUT GetP1OutputInst( FILE *P1OutputFP )
{
    struct DEFP1OUTPUT P1OutputInst;

    fscanf( P1OutputFP, "%X", &P1OutputInst.LC );
    fscanf( P1OutputFP, "%s", P1OutputInst.Label );
    fscanf( P1OutputFP, "%s", P1OutputInst.Mnemonic);
    fscanf( P1OutputFP, "%s", P1OutputInst.Operand1 );
    fscanf( P1OutputFP, "%s", P1OutputInst.Operand2 );
    fscanf( P1OutputFP, "%d", &P1OutputInst.IType );
    fscanf( P1OutputFP, " %[^\n]\n", P1OutputInst.Comment );

    return( P1OutputInst );
}


int ReadSymbolTable( struct SYMBOLTABLEDEF ST[] )
{
    FILE *STFP;  // Symbol Table File Pointer
    int NoOfSTEntries;
    char temp[16];

    // Reading Symbol Table from file and store it in a structural array
    if( ( STFP = fopen( "SymbolTable.txt", "r" ) ) == NULL )
    {
        SetColor( 12 );
        printf( "Error opening Pass1Output.txt file for read purpose\n\n\a" );
        SetColor( 0 );
        exit( 0 );
    }

    NoOfSTEntries = 0;

    while( !feof( STFP ) )
    {
        fscanf( STFP, "%s", ST[NoOfSTEntries].Symbol );
        fscanf( STFP, "%s\n", ST[NoOfSTEntries].Value );

        //fscanf( STFP, "%s\n", temp );
        //printf("%s\n", temp );

        NoOfSTEntries = NoOfSTEntries + 1;
    }

    fclose( STFP );

    return( NoOfSTEntries );
}


int WritePass1OutputToFile( struct PROG SourceInst, unsigned int LC, char Comment[], FILE *fp3, int Type )
{
        if( SourceInst.Label[0] == '\0')                    // If there is no label then
        {
            SourceInst.Label[0] = '?'; SourceInst.Label[1] = '\0';  // put "?" at that location and then put \0 as, while reading the file \0 is not identified
        }
        if( SourceInst.Operand1[0] == '\0')                 // If there is no Operand1 then
        {
            SourceInst.Operand1[0] = '?'; SourceInst.Operand1[1] = '\0';  // put "?" at that location and then put \0 as, while reading the file \0 is not identified
        }
        if( SourceInst.Operand2[0] == '\0')                 // If there is no Operand1 then
        {
            SourceInst.Operand2[0] = '?'; SourceInst.Operand2[1] = '\0';  // put "?" at that location and then put \0 as, while reading the file \0 is not identified
        }

        fprintf( fp3, "%04X    ", LC );
        fprintf( fp3, "%-15s", SourceInst.Label );
        fprintf( fp3, "%-12s", SourceInst.Mnemonic );
        fprintf( fp3, "%-15s", SourceInst.Operand1 );
        fprintf( fp3, "%-15s", SourceInst.Operand2 );
        fprintf( fp3, "%-7d", Type );
        fprintf( fp3, "%s\n", Comment );
}




int CheckSymbolTableForDuplicates( void )                   // Checking symbol table for any duplicate ( ie. repeated ) symbols
{
    FILE *fp;
    char Label[100][LABEL_LENGTH + 1], LabelValue[LABEL_VALUE_LENGTH + 1];  //**
    int i, j, n;

    if( ( fp = fopen( "SymbolTable.txt","r" ) ) == NULL )        // Opening a file for checking duplicates for Symbol
    {
        SetColor( 12 );
        printf( "Error opening symbol Table for reading purpose\n\a" );  //error
        SetColor( 0 );
        exit( 0 );
    }

    n = 0;                                                      // Acting as a counter to count the number of elements in the file

    while( !feof( fp ) )                                        // Reading the file till EOF occurs
    {
        fscanf( fp, "%s", Label[n] );                           // Reading the Label stored in the file and saving it in Label array
        fscanf( fp, "%s", LabelValue );                         // Reading the Value of the Label which is stored in-front of the label in the file ( not to be used )

        n = n + 1;                                              // Incrementing the counter
    }

    Label[n-1][0] = '\0';                                       // As there is a empty line at the end of the file to discard it we put \0 on that line's 0th position

    for( i =0; i<=(n-3); ++i)
    {

        for(j=i+1; j<= (n-2); ++j)
        {

            if( !strcmp( Label[i], Label[j] ) )
            {
                SetColor( 12 );
                printf( "\n\nDUPLICATE LABEL FOUND : %s\n\n\a ", Label[i] );           //error
                SetColor( 0 );
                exit( 0 );
            }
        }
    }

    fclose( fp );

}

int ProcessPseudoOp_Pass1( int p, struct PROG SourceInst, unsigned int *LC, FILE *fp2 , unsigned int LN)
{
    int Length;
    /// Processing the PseudoOp's

    //printf("PseudoOp Found=%d", p);
    if( p == 1 )
    {
        Length = ProcessDB_Pass1( SourceInst, LC, fp2, LN );         // Process DB pseudoOp
    }
    else if( p == 2)
    {
        Length = ProcessDS_Pass1( SourceInst, LC, fp2, LN );         // Process DS pseudoOp
    }
    else if( p == 3)
    {
        Length = ProcessDW_Pass1( SourceInst, LC, fp2, LN );         // Process DW pseudoOp
    }
    else if( p == 4)
    {
        Length = ProcessEND_Pass1( SourceInst, LC, fp2, LN );        // Process END pseudoOp
    }
    else if( p == 5)
    {
        //printf("\n\nEQU3:%04X\n\n", *LC );
        Length = ProcessEQU_Pass1( SourceInst, LC, fp2, LN );        // Process EQU pseudoOp
    }
    else if( p == 6 || p == 7)
    {
        Length = ProcessORGorSTART_Pass1( SourceInst, LC, fp2, LN);  // Process ORG or START pseudoOp
    }
    else                                                    // In case any unknown error occurs
    {
        printf("\nPseudoOp Error\n");                       //error
    }

    return( Length );
}

int ProcessDW_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )
{
    if( SourceInst.Label[0] != '\0' )
    {
        STSTO( fp2, SourceInst, *LC ) ;
    }

    //*LC = *LC + 2;
    return( 2 );       // Returning length of DW = 2 ( ie. the space required to store double byte constant )
}


int ProcessDB_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  // Processing the PseudoOp DB
{
    if( SourceInst.Label[0] != '\0' )    // if the Label is not empty
    {
        STSTO( fp2, SourceInst, *LC ) ;   // store it in symbol table
    }

    //WritePass1OutputToFile( SourceInst, LC, Comment, fp3 );  // Storing the result of PASS1 in a file ( Pass1Output.txt )

    //*LC = *LC + 1;                        // increment the value of LC by 1 as single byte is reserved for storing the constant
    return( 1 );       // Returning length of DB = 1 ( ie. the space required to store single byte constant )
}

int ProcessEND_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )   // Processing the PseudoOp END
{
    if( SourceInst.Label[0] != '\0' )                                                          // if the Label is not empty
    {
        STSTO( fp2, SourceInst, *LC ) ;                                                        // store it in symbol table
    }

    return(0);                                                                                 // Returning length of END = 0 ( ie. the space required to store 0 byte constant )
}

int ProcessDS_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )   // Processing the PseudoOp DS
{
    unsigned int OP1;

    OP1 = (unsigned int) atoi(SourceInst.Operand1);
    STSTO( fp2, SourceInst, *LC );

    //WritePass1OutputToFile( SourceInst, LC, Comment, fp3 );  // Storing the result of PASS1 in a file ( Pass1Output.txt )

    //*(LC) = *(LC) + OP1;
    return( OP1 );       // Returning length of DS = OP1 ( ie. the space required to store OP1 bytes  )
}

int ProcessEQU_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  //Processing the PseudoOp EQU
{
    unsigned int temp;
    int i;
    char *p;
    if( !strcmp( SourceInst.Operand1, "*" ) )               // If * then
    {
        STSTO( fp2, SourceInst, *LC );                      // store the value symbol = current value of LC ( which is integer )
    }
    else                                                    // else
    {
        i = 0;                                              // Remove h or H from the Operand 1

        while( SourceInst.Operand1[i] != '\0')              // till \0
        {
            if(SourceInst.Operand1[i] == 'h' || SourceInst.Operand1[i] == 'H')
            {
                SourceInst.Operand1[i] = '\0';
            }

            i =i + 1;                                       // Increment the value of i
        }
        temp = ( unsigned int )atoi( SourceInst.Operand1 ); // Convert the value of Operand 1 to integer for storing in symbol table
        STSTO( fp2, SourceInst, temp );                     // store the value symbol = as it is in operand 1

        // NOTE: we always store Hex values in symbol Table
    }

    return(0);  // Returning length of EQU = 0 ( ie. the space required to store 0 byte constant )
}

int ProcessORGorSTART_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  // Processing the PseudoOps ORG and START ( both are same )
{
    int Len;

    if( !strcmp( SourceInst.Operand1, "*" ) )            // If * is present then assign LC the current value of the location
    {
        *(LC) = *(LC);
    }
    else                                                 // If * is not present
    {
        Len = strlen( SourceInst.Operand1 );             // Calculating the length of the operand 1 for removing the character H from operand 1 ( if present )

        if( SourceInst.Operand1[Len - 1] == 'H' || SourceInst.Operand1[Len - 1] == 'h')  // If there is H or h at the end of operand ( which stands for hexadecimal )
        {
            SourceInst.Operand1[Len  - 1] = '\0';        // Replace it with \0
        }
        *(LC) = ( unsigned int ) strtol( SourceInst.Operand1, NULL, 16 );   // Convert the value of LC from Hex to decimal but
                                                                            // while storing in symbol table its stored in Hex
    }

    STSTO( fp2, SourceInst, *LC );                          // Symbol table storage

    return(0);  // Returning length of ORGOrSTART = 0 ( ie. the space required to store 0 byte constant )
}

int IsPseudoOp_Pass1( char Mnemonic[], char POT[][6] )       // Check for Pseudo Op if yes then return the index of that Pseudo Op in POT or else return 0
{
    int i;

    i = 1;                                                  // as zeroth element is dummy

    while( POT[i][0] != '\0' )                                // till the last element comes ie. \0
    {
        //printf("\PseudoOp:%s\n", Mnemonic);
        if( !strcmp( Mnemonic, POT[i] ) )                   // If PseudoOp is found then return its index ( ie. if the mnemonic matches PseudoOp )
        {
            return(i);
        }
        i = i + 1;
    }
    return( 0 );                                              // Else return 0
}

void STSTO( FILE *fp2, struct PROG SourceInst, unsigned int LC )  // Symbol table storage
{
    // NOTE: we always store Hex values in symbol Table with one exception

    /*
     * when [ LABEL:EQU * ] current value of the location counter will be the value of the label. so the label value will be
     * the value of LC in hex.
     *
     * when [ LABEL:EQU 04 ] we print the label value 04 ( ie. as it is ) in string format.
     *
     * When it is not EQU pseudoOp then we print the value of label in Hex format
     */

    if( !strcmp( SourceInst.Mnemonic, "EQU" ) )
    {
        if( !strcmp( SourceInst.Operand1, "*" ) )
        {
            fprintf( fp2, "%-15s  ", SourceInst.Label );
            fprintf( fp2, "%04X", LC );
            fprintf( fp2, "\n" );
        }
        else       // exception
        {
            fprintf( fp2, "%-15s  ", SourceInst.Label );
            fprintf( fp2, "%s", SourceInst.Operand1 );
            fprintf( fp2, "\n" );

        }

    }
    else
    {
        fprintf( fp2, "%-15s  ", SourceInst.Label );
        fprintf( fp2, "%04X", LC );
        fprintf( fp2, "\n" );
    }

}


char *SplitInstructionAndComment( char str[], char Comment[] )  // wherever there is ; or // they will be replaced with \0 to mark the end of string thus deleting the comment
{
    int count, IsComment;
    //char str1[50];
    IsComment = 0;           // Initializing the comment flag
    count = 0;               // Initializing counter
    while( str[count] != '\0' )         // till \0 is reached
    {

        if( str[count] == ';' || ( str[count] == '/' && str[count+1] == '/' ) )  // if there is "//" or ";" then
        {
            str[count] = '\0';                                                   // Put \0 at that position and end the string there
            IsComment = 1;                                                       // Setting a flag to identify if there is a comment to the instruction or not
            break;                                                               // Break the while loop
        }
        count++;                      // Incrementing counter
    }

    if ( IsComment )
    {
        count = count + 1; Comment[0] = '/';
        strcpy( &Comment[1], &str[count] );
    }
    else
    {
        Comment[0] = '?';
        Comment[1] = '\0';
    }

    Comment[COMMENT_LENGTH + 1] = '\0';   // Restricting comment length to output file to 50 characters

    return(str);                      // Return the string
}

char *DeleteExcessWhiteSpaces( char str[] )  // All the extra tabs and spaces will get deleted. wherever there are more than 1 space, it will be deleted and the tabs will be replaced by spaces.
{
    int count, i;
    char str1[256], c;
    count = 0;
    while( str[count] != '\0' )
    {
        if( str[count] == '\t' )
        {
            str[count] = ' ';
        }
        count++;
    }

    count = 0;
    i = 0;


    while( str[count] != '\0' )
    {

        if( str[count] == ' ' )
        {

            while( str[count + 1] == ' ' )
            {
                count++;
            }
        }

        str1[i] = str[count];
        count++;
        i++;

    }
    str1[ i ] = '\0';
    strcpy( str, str1 );
    return( str );
}

struct PROG GetFields( char Instruction[] )  // The instruction is broken down into smaller tokens for analysis and differentiating whether it is a label or mnemonic or a operand
{
    char Label[LABEL_LENGTH + 1], *p, Temp[256], *p1, *p2, Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1], Operand2[OPERAND2_LENGTH + 1];
    struct PROG TempInst;

    strcat( Instruction, " ");
    strcpy(Temp, Instruction);

    // Separating Label

    if(p = strchr( Temp, ':' ) )                   //
    {
        *(p) = '\0';

        if( p = strchr( Temp, ' ' ) )
        {
            *(p) = '\0';
        }
        if( strlen( Temp ) > LABEL_LENGTH )  // Restricting label size to 11 characters
        {
            printf( "\n WARNING: LABEL IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n \a" );
            Temp[LABEL_LENGTH] = '\0';
        }
        strcpy( TempInst.Label, Temp );
        //printf("\n\nLabel:%s?\n", TempInst.Label);
    }
    else  // If Label is not present so entering \0 in the label string
    {
        TempInst.Label[0] = '\0';
    }

    // Separating mnemonics

    strcpy( Temp, Instruction );

    if(p1 = strchr( Temp, ':' ) )  // When Label is present
    {
        if( *( p1 + 1 ) == ' ' )
        {
            p1 = ( p1 + 2 );
        }
        else
        {
            p1 = ( p1 + 1 );
        }
        if( p2 = strchr( p1, ' ' ) )
        {
            *(p2) = '\0';
        }
        strcpy( TempInst.Mnemonic, p1 );
        //printf("\n\nWhen Label is present:%s?\n", TempInst.Mnemonic);
    }
    else  // When Label is absent
    {
        p1 = Temp;

        if( Temp[0] == ' ' )
        {
            p1++;
        }
        if( p2 = strchr( p1, ' ' ) )
        {
            *(p2) = '\0';
        }

        strcpy( TempInst.Mnemonic, p1 );
        //printf("%\n\nWhen Label is absent:%s?\n", TempInst.Mnemonic);
    }


    /// Separating operand 1


    strcpy( Temp, Instruction );

    if(p1 = strchr( Temp, ':' ) )                              // When Label is present bypass Label
    {
        if( *( p1 + 1 ) == ' ' )    // If blank space is present after :
        {
            p1 = (p1+2);            // Bypass blank space after colon and point to starting char of Mnemonic
        }
        else                        // Else
        {
            p1 = (p1 + 1);          // Point to starting char of Mnemonic
        }
        if( p2 = strchr( p1, ' ' ) )
        {
            p1 = ( p2 + 1 );
        }

        p2 = strtok( p1, " ," );

        if( p2 == NULL )
        {

            TempInst.Operand1[0] = '\0';
        }
        else
        {
            if( strlen(p1) > OPERAND1_LENGTH )                           // Restricting Operand1 size to 11 characters
            {
                printf("\n WARNING: OPERAND1 IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n \a");
                *( p1 + OPERAND1_LENGTH ) = '\0';
            }
            strcpy( TempInst.Operand1, p1 );
        }

        // printf("\n\nOperand1:%s?\n", Operand1);
    }
    else                                                    // When Label is absent
    {
        p1 = Temp;

        if( Temp[0] == ' ' )
        {
            p1++;
        }
        if( p2 = strchr( p1, ' ' ) )
        {
            p1 = (p2+1);
        }
        p2 = strtok( p1, " ," );

        if( p2 == NULL )
        {
            TempInst.Operand1[0] = '\0';
        }
        else
        {
            if( strlen(p1) > OPERAND1_LENGTH )  // Restricting Operand1 size to 11 characters
            {
                printf( "\n WARNING: OPERAND1 IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n \a" );
                Temp[OPERAND1_LENGTH] = '\0';
            }
            strcpy( TempInst.Operand1, p1 );
        }

        // printf("%\n\nOperand1q:%s?\n", Operand1);
    }

    /// Separating operand 2

    strcpy( Temp, Instruction );

    if( p1 = strchr( Temp, ',' ) )
    {
        if( *(p1+1) == ' ' )
        {
            p1 = (p1 +2);
        }
        if( p2 = strchr( p1, ' ' ) )
        {
            *(p2) = '\0';
        }
        if( strlen(p1) > OPERAND2_LENGTH )  // Restricting Operand2 size to 11 characters
        {
            printf( "\n WARNING: OPERAND2 IS TOO BIG....ONLY FIRST 11 CHARACTERS ARE SIGNIFICANT \n \a" );
            *(p1 + OPERAND2_LENGTH ) = '\0';
        }
        strcpy( TempInst.Operand2, p1 );
        //printf("\n\nOperand2:%sh\n", TempInst.Operand2);
    }
    else
    {
        TempInst.Operand2[0] = '\0';
    }

    return(TempInst);
}

int ReadMachineOpTable( struct MOT MOTInst[] )           // Reading MachineOp Table from the text file and store it into a structure
{
    FILE *fp;

    int i;

    if( ( fp = fopen( "MachineOpTable 8085A.txt", "r" ) ) == NULL )   // Opening text file which has MachineOp table
    {
        //error
        printf( "\n Error opening the text file named MachineOpTable 8085A...\n Exiting....\n" );
        exit(0);
    }


    for( i=1; i<N; ++i )
    {
        fscanf(fp, "%s", MOTInst[i].Mnemonic);
        fscanf(fp, "%s", MOTInst[i].Operand1);
        fscanf(fp, "%s", MOTInst[i].Operand2);
        fscanf(fp, "%s", MOTInst[i].OpCode);
        fscanf(fp, "%d", &MOTInst[i].Length);
        fscanf(fp, "%d", &MOTInst[i].Type);
    }

    fclose(fp);

}

int MOTGet_Pass1( struct MOT MOTInst[], char Mnemonic[], unsigned int LN )  // Fetch the MOT if present and return its index in MOT table
{
    int Index;
    char MOTStr[100], SourceStr[100];

    //Searching MOT for the first time
    for( Index = 1; Index<N; ++Index )                                          // Checking if the Source Instruction's Mnemonic is present in the MOT table
    {                                                             // if present then break the loop with its index number stored in the variable i
        if( !( strcmp( MOTInst[Index].Mnemonic, Mnemonic ) ) )
        {
            break;
        }
    }

    /*if( Index == N )                                                  // If syntax error in Mnemonic
    {
        SetColor( 12 );
        printf( "\n\n LINE NO:%d :: HAS SYNTAX ERROR IN MNEMONIC.... \n\n\a ", LN );     // error#1 in mnemonic
        SetColor( 0 );
        return( -1 );
    }*/

    return( Index );
}


 /// Color Function

void SetColor(int ForgC)
{
     WORD wColor;
     ///We will need this handle to get the current background attribute
     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;

     ///We use csbi for the wAttributes word.
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
        ///Mask out all but the background attribute, and add in the foreground color
          // 0000 1111 && 0001 1110 = 0000 1110 = 14
          wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);
     }
     return;
}
