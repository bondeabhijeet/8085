#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>
#include<ctype.h>
#include<time.h>

#define N 247                                    // No of entries in MOT are 246. we are using 1 to 246. We are not using 0th location of the array.
#define SOURCE_PROG_LINE_LENGTH 255                   // Max. length of source program line length
#define LABEL_LENGTH 11
#define MNEMONIC_LENGTH 9
#define OPERAND1_LENGTH 11
#define OPERAND2_LENGTH 11
#define OPCODE_LENGTH 2
#define LABEL_VALUE_LENGTH 4                  //  Maximum label value will be FFFF
#define COMMENT_LENGTH 50
#define MAX_NO_OF_ST_ENTRIES 200              // ST stands for Symbol Table

struct MOT                                     // For MachineOp Table
{
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
    char OpCode[OPCODE_LENGTH + 1];
    int Length;
    int Type;
};

struct SPROG                                   // SPROG stands for Source PROGram
{
    char Label[LABEL_LENGTH + 1];
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
    char Comment[COMMENT_LENGTH +1] ;
};

struct SYMBOLTABLEDEF                         // Symbol Table Definition
{
    char Symbol[LABEL_LENGTH + 1];
    char Value[LABEL_VALUE_LENGTH + 1];
};

struct P1OUTPUTDEF                             // Pass 1 Output
{
    unsigned int LN;
    unsigned int LC;
    char Label[LABEL_LENGTH + 1];
    char Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1];
    char Operand2[OPERAND2_LENGTH + 1];
    int IType;
    char Comment[COMMENT_LENGTH + 1];
    char LN_Output_Flag;
};

int Assembler_Pass1( struct MOT MOTInst[], char POT[][6], char SourceFileName[] );
char *DeleteExcessWhiteSpaces_Pass1( char str[] );
struct SPROG GetFields_Pass1( char Instruction[] );
int ReadMachineOpTable( struct MOT MOTInst[] );
int MOTGet_LS_Pass1( struct MOT MOTInst[], char Mnemonic[] );
int MOTGet_BS_Pass1( struct MOT MOTInst[] , char Mnemonic[] );
void STSTO_Pass1( FILE *fp2, struct SPROG SourceInst, unsigned int LC );
int IsPseudoOp_Pass1( char Mnemonic[], char POT[][6] );
int ProcessPseudoOps_Pass1( int p, struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessORGorSTART_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2 , unsigned int LN );
int ProcessEQU_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDS_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDB_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDW_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessEND_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int CheckSymbolTableForDuplicates_Pass1( void );
int Write_Pass1_OutputToFile( struct SPROG SourceInst, unsigned int LC, FILE *fp3, int Type, unsigned int LN );
int Are_All_Hexadecimal_Digits( char str[] );
int Are_All_Decimal_Digits( char str[] );
int Is_Register( char str[] );

///***************************************************************************************************************************************************

int Assembler_Pass2( struct MOT MOTInst[], char POT[][6], char LN_Output_Flag );
struct P1OUTPUTDEF GetP1OutputInst_Pass2( FILE *P1OutputFP );
int ReadSymbolTable_Pass2( struct SYMBOLTABLEDEF ST[] );
int STGet_Pass2( char Operand1[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
int ProcessPseudoOps_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP, unsigned int p );
int ProcessORGorSTART_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP );
int ProcessEND_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP );
int ProcessEQU_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP );
int ProcessDS_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP );
int ProcessDB_Pass2( struct P1OUTPUTDEF Temp, FILE *P2OutputFP );
int ProcessDW_Pass2( struct P1OUTPUTDEF Temp, FILE *P2OutputFP );
int MOTGet_LS_Pass2( struct MOT MOTInst[], char ConcatenatedString[], struct P1OUTPUTDEF P1OutputInst );
int MOTGet_BS_Pass2( struct MOT MOTInst[], char ConcatenatedString[], struct P1OUTPUTDEF P1OutputInst );
int ProcessMachineOps_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType1_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], FILE *P2OutputFP );
int ProcessMOType26_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType3_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType4_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType5_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType7_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType8_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int ProcessMOType9_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP );
int Error_Pass2( struct P1OUTPUTDEF P1OutputInst );
int ProcessLiteral_Pass2( char *Operand, int LiteralLen, unsigned int LN ) ;

void SetColor( int ForgC );

///******************************************************* MAIN STARTS HERE **************************************************************************************

int main()
{
    struct MOT MOTInst[N];
    char SourceFileName[80];
    char LN_Output_Flag;
    int Assembled_Lines, No_of_Symbols_Found ;


    char POT[][6] = {"ZZZZ", "DB", "DS", "DW", "END", "EQU", "ORG", "START", "\0"};  // remember that the zeroth element is dummy ( creating POT )

    ReadMachineOpTable( MOTInst );                                             // Reading MachineOp Table from the text file and storing it into a structure

    //printf("\nENTER THE FILE NAME OF SOURCE PROGRAM:: ");
    //scanf(" %[^\n]", SourceFileName );

    //printf("\nDo you need Source Program Line Numbers in assembled File (Y/N):: ");
    //scanf("%c", &LN_Output_Flag );
    //LN_Output_Flag = toupper(LN_Output_Flag);

    strcpy( SourceFileName, "SourceProgram.txt" );           // Source Program File Name
    LN_Output_Flag = 'Y';                                    // When 'Y' Source Progg. LNs will displayed in PASS 2 Output (Assembled ) File after Comments.
                                                             // When 'N' LNs will not displayed in assembled file

    No_of_Symbols_Found = Assembler_Pass1( MOTInst, POT, SourceFileName );         // Call Pass1 : Purpose : Prepare Symbol Table

    SetColor( 10 );
    printf( "\n Pass 1 Completed....           No. of Symbols Found : %d \n" ,  No_of_Symbols_Found ) ;
    SetColor( 13 );


    Assembled_Lines = Assembler_Pass2( MOTInst, POT, LN_Output_Flag );                       // Call Pass2 : Purpose : Prepare final Assembly


    SetColor( 10 );
    printf( "\n Pass 2 Completed....\n" ) ;
    SetColor( 27 );
    printf( "\n Assembly is successful...      No. of Lines Assembled : %d  \n\n" , Assembled_Lines ) ;
    SetColor( 15 );

    // getch();

}

/// *****************  MAIN ENDS HERE   ************************************************************************************


int Assembler_Pass1( struct MOT MOTInst[], char POT[][6], char SourceFileName[] )
{
    FILE *fp1, *fp2, *fp3;
    unsigned int counter, p;
    int MOTIndex, Length, No_of_Symbols_Found;
    unsigned int LC, LN;
    struct SPROG SourceInst;
    char Instruction[SOURCE_PROG_LINE_LENGTH+1], InstructionWithComment[SOURCE_PROG_LINE_LENGTH+1];
    char *str;  // For recieving purpose

    if( ( fp1 = fopen( SourceFileName, "r" ) ) == NULL )          // Opening a file ( read only ) to read the Source program
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

    /// Pass 1 : Preparing Symbol Table

    LC = 0;                                                 // Assigning location counter to 2000
    LN = 1;                                                 // Assigning the line counter to 1

    while(!feof(fp1))
    {
        //fscanf( fp1, " %[^\n]\n", Instruction );              // Read the full sentence from the given file with comment

        fgets( Instruction, SOURCE_PROG_LINE_LENGTH, fp1);                          // Read the full sentence from the given file with comment

        if( ( Instruction[0] == '/' && Instruction[1] == '/' ) || ( Instruction[0] == ';' ) )   //  Is the complete comment line, bypass it from further processing
        {
            LN = LN + 1;
            continue;
        }

        if( Instruction[0] == '\n'  )             //  Is it blank line, bypass it from further processing
        {
            printf( " \n Warning : Line No.: %d  : BLANK LINE Detected \n", LN) ;
            LN = LN + 1;
            continue;
        }

        str = DeleteExcessWhiteSpaces_Pass1( Instruction );      // Deletes all the extra spaces and tabs ( no tabs will be present after execution of this function )
        strcpy( InstructionWithComment, str );                   // Store the instruction in the InstructionWithComment variable

        SourceInst = GetFields_Pass1( InstructionWithComment );    // Separate the fields of the instruction into -
                                                                 // - Label, Mnemonic, Operand1, Operand2, Comment
        /// Processing PseudoOps and MachineOps

        if( ( p = IsPseudoOp_Pass1( SourceInst.Mnemonic, POT ) ) )             // checking if the Mnemonic is pseudoOp if yes then ProcessPseudoOp_Pass1() will be executed
        {
            Length = ProcessPseudoOps_Pass1( p, SourceInst, &LC, fp2 , LN );   // Process Pseudo op

            Write_Pass1_OutputToFile( SourceInst, LC, fp3, -99, LN );          // Storing the result of PASS1 in a file ( Pass1Output.txt ). Sending Dummy Instruction Type, as type is required only for MachineOp

            LC = LC + Length;
        }
        else                 /// If the Mnemonic is not a pseudoOp then it might be machine Op if yes then execute the following functions
        {
            MOTIndex = MOTGet_BS_Pass1( MOTInst, SourceInst.Mnemonic );         // Identify the index of the Mnemonic in MOT and store it in MOTIndex

            if( MOTIndex == N )                                                 // If syntax error in Mnemonic
            {
                SetColor( 12 );
                printf( "\n\n LINE NO:%d :: HAS SYNTAX ERROR IN MNEMONIC.... \n\n\a ", LN );     // error#1 in mnemonic
                SetColor( 0 );
                exit( 0 );
            }

            if( ( SourceInst.Label[0] )!= '\0' )                             // If label is present then call STSTO_Pass1
            {
                STSTO_Pass1(fp2, SourceInst, LC);                            // Store the Label in symbol table file
            }

            Write_Pass1_OutputToFile( SourceInst, LC, fp3, MOTInst[MOTIndex].Type, LN );  // Storing the result of PASS1 in a file ( Pass1Output.txt )

            LC = LC + MOTInst[MOTIndex].Length;                             // Increment the location counter by the length of the MachineOp ( length is fetched from the MOT table
        }

        if ( LC > 65535 )
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  Memory Address (%04XH) exceeds FFFFH :   \n", LN, LC ) ;
            SetColor( 0 );
            exit( 0 );
        }

        LN = LN + 1;                                                        // Increment the Line counter by 1
    }

    fclose( fp1 );                                                    // Closing Source  file
    fclose( fp2 );                                                    // Closing Symbol Table file
    fclose( fp3 );                                                    // Closing Pass Output file

    No_of_Symbols_Found = CheckSymbolTableForDuplicates_Pass1( );     // Checking symbol table for any duplicate ( ie. repeated ) symbols

    return( No_of_Symbols_Found ) ;                  // Return with no. of symbols found in pass 1
}



int Assembler_Pass2( struct MOT MOTInst[], char POT[][6], char LN_Output_Flag )
{
    int NoOfSTEntries, i, STIndex;
    FILE *P1OutputFP, *P2OutputFP;
    struct SYMBOLTABLEDEF ST[MAX_NO_OF_ST_ENTRIES];                        // ST for Symbol Table
    struct P1OUTPUTDEF P1OutputInst;
    unsigned int p, LN;

    // Reading Symbol Table from file and store it in a structural array

    NoOfSTEntries = ReadSymbolTable_Pass2( ST );

    if( ( P1OutputFP = fopen( "Pass1Output.txt", "r" ) ) == NULL )        //  Open File Pass1 O/P as an Input to Pass2
    {
        SetColor( 12 );
        printf("\nERROR OPENING Pass1Output.txt FILE\n\n\a ");
        SetColor( 0 );
        exit( 0 );
    }

    if( ( P2OutputFP = fopen( "Pass2Output.txt", "w" ) ) == NULL )        // Open File for writing O/P Pass2
    {
        SetColor( 12 );
        printf("\nERROR OPENING Pass2Output.txt FILE\n\n\a ");
        SetColor( 0 );
        exit( 0 );
    }

    LN = 1;                                                               // Initializing Line Number Counter

    while( !feof( P1OutputFP ) )                                          // Perform till end of I/P File
    {
        P1OutputInst = GetP1OutputInst_Pass2( P1OutputFP );               // Read subsequent instruction

        P1OutputInst.LN_Output_Flag = LN_Output_Flag;                     // Initializing "LN_Output_Flag"

        // printf( "\n %c", P1OutputInst.LN_Output_Flag) ;

        if( ( p = IsPseudoOp_Pass1( P1OutputInst.Mnemonic, POT ) ) )       // Is Pseudo Op in Mnemonic Field ??
        {
            ProcessPseudoOps_Pass2( P1OutputInst, P2OutputFP, p );         // if yes then call ProcessPseudoOp_Pass1()
        }
        else                                                               // If not Pseudo Op, then it must be M/C OP
        {
            ProcessMachineOps_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );   // Process M/C OP
        }

        LN = LN + 1;                                                                           // Increment Line Number Counter
    }

    fclose( P1OutputFP );                                                        // Close I/P File
    fclose( P2OutputFP );                                                        // Close O/P File

    return ( P1OutputInst.LN );                            // Returning with Line number of last line processed / Assembled
}


int ProcessMachineOps_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    char Spaces11[] = "           ";
    //int MOTIndex;                           // MOTIndex variable is Local and different than the one used in Pass 1
    /*
     * If the fields of P1OutputInst ( ie. Label, Mnemonic, Operand1, Operand2, and Comment) are empty ( ie. indicated
     * by ? ) then place spaces in those fields in temp variable.
     */

    if( P1OutputInst.Label[0] == '?')                   // If Label is not present
    {
        strcpy( P1OutputInst.Label, Spaces11 );         // Fill it with 11 blank spaces in the temp variable
        P1OutputInst.Label[11] = '\0';                  // And terminate the Label string
    }
    if( P1OutputInst.Operand1[0] == '?')                // If Operand 1 is not present
    {
        strcpy( P1OutputInst.Operand1, Spaces11 );      // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Operand1[11] = '\0';               // And terminate the Operand 1 string
    }
    if( P1OutputInst.Operand2[0] == '?')                // If Operand 2 is not present
    {
        strcpy( P1OutputInst.Operand2, Spaces11 );      // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Operand2[11] = '\0';               // And terminate the Operand 2 string
    }
    if( P1OutputInst.Comment[0] == '?')                 // If Operand 2 is not present
    {
        strcpy( P1OutputInst.Comment, Spaces11 );       // Fill it with 14 blank spaces in the temp variable
        P1OutputInst.Comment[1] = '\0';                // And terminate the Operand 2 string
    }

    if( P1OutputInst.IType == 1 )                                                       // Process Type 1 M/C Instructions
    {
         ProcessMOType1_Pass2( P1OutputInst, MOTInst, P2OutputFP );
    }
    else if( P1OutputInst.IType == 2 || P1OutputInst.IType == 6 )                       // Process Type 2 & 6 M/C Instructions
    {
        ProcessMOType26_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
    else if( P1OutputInst.IType == 3 )                                                  // Process Type 3 M/C Instructions
    {
        ProcessMOType3_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
    else if( P1OutputInst.IType == 4 )                                                  // Process Type 4 M/C Instructions
    {
        ProcessMOType4_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
    else if( P1OutputInst.IType == 5 )                                                  // Process Type 5 M/C Instructions
    {
        ProcessMOType5_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
    else if( P1OutputInst.IType == 7 )                                                  // Process Type 7 M/C Instructions
    {
        ProcessMOType7_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
    else if( P1OutputInst.IType == 8 )                                                  // Process Type 8 M/C Instructions
    {
        ProcessMOType8_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
    else if( P1OutputInst.IType == 9 )                                                  // Process Type 9 M/C Instructions
    {
        ProcessMOType9_Pass2( P1OutputInst, MOTInst, ST, NoOfSTEntries, P2OutputFP );
    }
}



int ProcessMOType9_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                                                        // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1], SourceOperand2[OPERAND2_LENGTH + 1];
    char HigherByte[3], LowerByte[3];

    ///  Constructing Type 9 Instruction as Follows     (Length of Instruction: 3 Bytes)
    ///  Mnemonic Rp, DATA(16)              Only All LXI Instructions
    ///  LXI B, 0F0FH     OR              LXI LABEL1, LABEL2      OR              LXI LABELl, =0F0FH
    ///                   OR     LABEL1 : EQU B                   OR     LABEL1 : EQU B
    ///                   OR     LABEL2 : EQU 0F0FH

    strcpy( SourceOperand1, P1OutputInst.Operand1 );
    strcpy( SourceOperand2, P1OutputInst.Operand2 );

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                             // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand2, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand2, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )

        if (strlen(P1OutputInst.Operand2) > 4)
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  16 bit Immediate Data  (%s) exceeds FFFFH :   \n", P1OutputInst.LN, P1OutputInst.Operand2 ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.Operand2[0] == '=' )                                    // Else if it is a Literal in Operand2 ??
    {
        ProcessLiteral_Pass2( &P1OutputInst.Operand2[0], 4, P1OutputInst.LN );    // Process Literal in Operand2 with LiteralLen = 4
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );        // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );        // Copying the Operans1 in ConcatenatedString for the searching process

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );   // Call MOTGet with Bin Search

    /// Constructing Type 9 Instruction as follows
    /// Mnemonic Rp, Data(16)              Only LXI Instruction
    /// LXI B, F0FH

    P1OutputInst.Operand2[4] = '\0';                           // Deleting H from the Operand 2
    HigherByte[0] = P1OutputInst.Operand2[0];                  // Separating Higher Byte from 16 bit address
    HigherByte[1] = P1OutputInst.Operand2[1];
    HigherByte[2] = '\0';

    LowerByte[0] = P1OutputInst.Operand2[2];                   // Separating Lower Byte from 16 bit address
    LowerByte[1] = P1OutputInst.Operand2[3];
    LowerByte[2] = '\0';

    /// Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                        // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                   // if true print Source Program LNs in Assembled file
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );
    }
    else                                                      // else
    {
        fprintf( P2OutputFP, "\n" );                          //  Don't print Source Program LNs in Assembled file
    }

    /// Printing LC+1 & Lower Byte of 16 bit immediate data on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );      // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", LowerByte );                    // Put Operand 1 in P2Output File

    /// Printing LC+2 & Higher Byte of 16 bit immediate data on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 2 ) );      // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", HigherByte );                   // Put Operand 1 in P2Output File
}



int ProcessMOType8_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                                                        // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1], SourceOperand2[OPERAND2_LENGTH + 1];

    /// Constructing Type 8 Instruction as follows :      (Length of Instruction: 2 Bytes)
    /// Mnemonic Rg, Data(8)    Only All MVI Rg, ImmediateData(8) instructions with 2 operands [Op1 is Rg,  Op2 is ImmediateData(8)]
    /// MVI B, 0F        OR              MVI Label1, LABEL2          OR              MVI Label1, =0FH
    ///                  OR     LABEL1 : EQU B                       OR     LABEL1 : EQU B
    ///                  OR     LABEL2 : EQU 0F                      OR

    strcpy( SourceOperand1, P1OutputInst.Operand1 );
    strcpy( SourceOperand2, P1OutputInst.Operand2 );

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand2, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand2, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )

        if( strlen(P1OutputInst.Operand2) >2 )
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d : 8-Bit Immediate Data exceeds FFH : (%s)  \n", P1OutputInst.LN, P1OutputInst.Operand2 ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.Operand2[0] == '=' )                                    // Else if it is a Literal in Operand2 ??
    {
        ProcessLiteral_Pass2( &P1OutputInst.Operand2[0], 2, P1OutputInst.LN );       // Then Process Literal in Operand1 with LiteralLen = 2
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );

    P1OutputInst.Operand2[2] = '\0';                                            // Deleting H from the Operand 2

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                        // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                   // if true print Source Program LNs in Assembled file
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );
    }
    else                                                       // else
    {
        fprintf( P2OutputFP, "\n" );                           //  Don't print Source Program LNs in Assembled file
    }

    /// Printing LC+1 & Operand2 ( 8 bit Immediate Data) on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", P1OutputInst.Operand2 );      // Put Operand 1 in P2Output File
}



int ProcessMOType7_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1], SourceOperand2[OPERAND2_LENGTH + 1];

    /// Constructing Type 7 Instruction as Follows :    (Length of Instruction: 1 Byte)
    /// Mnemonic Rg, Rg              Only All MOV Instructions
    /// MOV A, B       OR              MOV LABEL1, LABEL2
    ///                OR     LABEL1 : EQU A
    ///                OR     LABEL2 : EQU B

    strcpy( SourceOperand1, P1OutputInst.Operand1 );
    strcpy( SourceOperand2, P1OutputInst.Operand2 );

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand2, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand2, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );
    strcat( ConcatenatedString, P1OutputInst.Operand2 );

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );       // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                 // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );   // Print Source Program LNs in Assembled file
    }
    else                                                          // else
    {
        fprintf( P2OutputFP, "\n" );                              //  Don't print Source Program LNs in Assembled file
    }
}



int ProcessMOType5_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                                                       // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], HigherByte[3], LowerByte[3], SourceOperand1[OPERAND1_LENGTH + 1];

    ///  Constructing Type 5 Instruction as Follows      (Length of Instruction: 3 Bytes)
    ///  Mnemonic Address(16)              Only All: JUMP, CALL, LDA, LHLD, SHLD, STA  Instructions
    ///  JMP 0F0FH        OR               JMP LABEL1              OR              JMP LABELl, =0F0FH
    ///                   OR      LABEL1 : EQU 0F0FH               OR
    ///                   OR      LABEL1 : May be attached to any M/C Instruction

    strcpy( SourceOperand1, P1OutputInst.Operand1 );


    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )

        if (strlen(P1OutputInst.Operand1) > 4)
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  Label Value : Memory Address  (%s) exceeds FFFFH :   \n", P1OutputInst.LN, P1OutputInst.Operand1 ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.Operand1[0] == '=' )                                    // Else if it is a Literal in Operand1 ??
    {
        ProcessLiteral_Pass2( &P1OutputInst.Operand1[0], 4, P1OutputInst.LN );   // Then Process Literal in Operand1 with LiteralLen = 4

        if (strlen(P1OutputInst.Operand1) > 4)
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  Literal Constant  (%s) exceeds FFFFH :   \n", P1OutputInst.LN, P1OutputInst.Operand1 ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }


    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );

    HigherByte[0] = P1OutputInst.Operand1[0];                // Separating Higher byte from 16 bit address
    HigherByte[1] = P1OutputInst.Operand1[1];
    HigherByte[2] = '\0';

    LowerByte[0] = P1OutputInst.Operand1[2];                 // Separating Lower byte from 16 bit address
    LowerByte[1] = P1OutputInst.Operand1[3];
    LowerByte[2] = '\0';


    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );       // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );    // Print Source Program LNs in Assembled file
    }
    else                                                           // else
    {
        fprintf( P2OutputFP, "\n" );                               //  Don't print Source Program LNs in Assembled file
    }

    /// Printing LC+1 & Lower Byte of 16 bit address on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", LowerByte );      // Put Operand 1 in P2Output File

    /// Printing LC+2 & Higher Byte of 16 bit address on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 2 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", HigherByte );      // Put Operand 1 in P2Output File

}



int ProcessMOType3_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1];

    ///  Constructing Type 3 Instruction as Follows   (Length of Instruction: 2 Bytes)
    ///  Mnemonic DATA(8)        All Immediate Data(8) with only 1 operand Instructions: ACI,ADI,ANI,CPI,ORI,SUI,XRI
    ///  ADI B, 0F  H     OR              ADI LABEL1, LABEL2      OR              ADI LABELl, =0FH
    ///                   OR     LABEL1 : EQU B                   OR     LABEL1 : EQU B
    ///                   OR     LABEL2 : EQU 0FH

    strcpy( SourceOperand1, P1OutputInst.Operand1 );

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )

        if( strlen(P1OutputInst.Operand1) >2 )
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d 8-Bit Immediate Data exceeds FFH : (%s)  \n", P1OutputInst.LN, P1OutputInst.Operand1 ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.Operand1[0] == '=' )                                    // Else if it is a Literal in Operand1 ??
    {
        ProcessLiteral_Pass2( &P1OutputInst.Operand1[0], 2, P1OutputInst.LN );       // Then Process Literal in Operand 1 with LiteralLen = 2
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );       // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );    // Print Source Program LNs in Assembled file
    }
    else                                                           // else
    {
        fprintf( P2OutputFP, "\n" );                              //  Don't print Source Program LNs in Assembled file
    }

    /// Printing LC+1 & 8 bit immediate data on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );  // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", P1OutputInst.Operand1 );      // Put Operand 1 in P2Output File

}



int ProcessMOType4_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1];

    /// Constructing Type 4 Instruction as follows :   (Length of Instruction: 2 Bytes)
    /// Mnemonic Data(8)              Only IN & OUT Instructions with 8 bit address as an operand1
    /// IN 0F        OR              IN PORT1             OR              IN =0FH
    ///              OR      PORT1 : EQU 0F               OR

    strcpy( SourceOperand1, P1OutputInst.Operand1 );

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )

        if( strlen(P1OutputInst.Operand1) > 2 )
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d : 8-Bit PORT ADDRESS is Expected : (%s)  \n", P1OutputInst.LN, P1OutputInst.Operand1 ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }
    else if( P1OutputInst.Operand1[0] == '=' )                                    // Else if it is a Literal in Operand1 ??
    {
        ProcessLiteral_Pass2( &P1OutputInst.Operand1[0], 2, P1OutputInst.LN );    // Then Process Literal in Operand1 with LiteralLen = 2
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                        // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                        // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );     // Print Source Program LNs in Assembled file
    }
    else                                                            // else
    {
        fprintf( P2OutputFP, "\n" );                                //  Don't print Source Program LNs in Assembled file
    }

    /// Printing LC+1 & 8 bit PORT address on new line (in Final Assembled File).

    fprintf( P2OutputFP, "%04X  ", ( P1OutputInst.LC + 1 ) );     // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%-7s\n", P1OutputInst.Operand1 );      // Put Operand 1 in P2Output File

}



int ProcessMOType26_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries, FILE *P2OutputFP )
{
    int STIndex, MOTIndex;                          // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20], SourceOperand1[OPERAND1_LENGTH + 1];

    /// Constructing Type 2 Instruction as follows :   (Length of Instruction: 1 Byte)
    /// Mnemonic Rg                All Arithmetic & Logical instructions, PSSH, POP,  (contains only 1 operand)
    /// ADD B            OR              ADD LABEL1
    ///                  OR     LABEL1 : EQU B

    /// Constructing Type 6 Instruction as follows :    (Length of Instruction: 1 Byte)
    /// Mnemonic n                Only All RST instructions (contains only 1 operand)
    /// RST 5            OR                RST KEYBOARD
    ///                  OR     KEYBOARD : EQU 5

    strcpy( SourceOperand1, P1OutputInst.Operand1 );

    if( ( STIndex = STGet_Pass2( P1OutputInst.Operand1, ST, NoOfSTEntries ) ) != -88 )  // To check if there is symbol in OP1 we go to STGet_Pass2, if yes
    {
        strcpy( P1OutputInst.Operand1, ST[STIndex].Value );                       // Copy the Value of symbol in that Operand field ( ie. in P1OutputInst.Operand1 )
        // Include error here
    }

    // Making SearchString for the instruction

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                          // Copying the Mnemonic in ConcatenatedString for the searching process
    strcat( ConcatenatedString, P1OutputInst.Operand1 );                          // Concatenating P1OutputInst.Operand1 and ConcatenatedString

    // Sending the SearchString for searching MOT and getting MOTIndex

    MOTIndex =  MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", SourceOperand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );       // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );    // Print Source Program LNs in Assembled file
    }
    else                                                          // else
    {
        fprintf( P2OutputFP, "\n" );                              //  Don't print Source Program LNs in Assembled file
    }

}



int ProcessMOType1_Pass2( struct P1OUTPUTDEF P1OutputInst, struct MOT MOTInst[], FILE *P2OutputFP )
{
    int MOTIndex;               // MOTIndex variable is Local and different than the one used in Pass 1
    char ConcatenatedString[20];

    /// Constructing Type 1 Instruction as follows :    (Length of Instruction: 1 Byte)
    /// Mnemonic        Only CMA,CMC,DAA,DI,EI,HLT,NOP,PCHL,all Rotate,all Return,RIM,SIM,SPHL,STC,XCHG,XTHL Instructions
    /// CMA

    // Sending the Mnemonic for searching MOT and getting MOTIndex

    strcpy( ConcatenatedString, P1OutputInst.Mnemonic );                  // Making the SearchString ( Searching only on Mnemonic )

    MOTIndex = MOTGet_BS_Pass2( MOTInst, ConcatenatedString, P1OutputInst );  // Sending the Mnemonic for searching process ( Searching only on Mnemonic )

    // Writing the processed output to the P2OutputFP File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put Location Counter in P2Output File
    fprintf( P2OutputFP, "%s", MOTInst[MOTIndex].OpCode );     // Put OpCode in P2Output File

    /// Printing Source Instruction in Final Assembled File

    fprintf( P2OutputFP, "%-4s", " " );                       // Put Spaces for formatting in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );     // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );       // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );    // Print Source Program LNs in Assembled file
    }
    else                                                           // else
    {
        fprintf( P2OutputFP, "\n" );                               //  Don't print Source Program LNs in Assembled file
    }
}



int ProcessLiteral_Pass2( char *Operand, int LiteralLen, unsigned int LN )
{
    unsigned int Hex_Flag, Dec_Flag, n;
    unsigned long i ;
    char str[OPERAND1_LENGTH + 1];

    strcpy( str, Operand );

    if( Operand[0] == '=' )                                          // Is Literal
    {
        strcpy( Operand, &str[1] );                                 // Remove '=' sign
    }

    Hex_Flag = 0 ;    Dec_Flag = 0 ;                               // Initializing Decimal and Hex Flags

    n = strlen(Operand);

    if ( Operand[n-1] == 'h' || Operand[n-1] == 'H')
    {
        Operand[n-1] = '\0';                                       // Terminate the string there
        Hex_Flag = 1 ;
    }
    else if ( Operand[n-1] == 'i' || Operand[n-1] == 'I')
    {
        Operand[n-1] = '\0';                                       // Terminate the string there
        Dec_Flag = 1 ;
    }


    if ( Hex_Flag == 1 )                                                           // If hex digits indicated by 'h' or 'H'
    {
        if (  !Are_All_Hexadecimal_Digits(Operand)  )                  // Are All Hex
        {
            SetColor( 12 );
            printf("\n ERROR IN LINE NO: %d :: LITERAL DOES NOT CONTAIN HEXADECIMAL DIGITS  ( ERROR IN \"%s\" ) \n\a", LN, str );     // error
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen( Operand) > LiteralLen )
        {
            SetColor( 12 );
            printf("\n ERROR IN LINE NO: %d :: LITERAL SHOULD HAVE %d HEX DIGITS  ( ERROR IN \"%s\" ) \n\a", LN, LiteralLen, str );     // error
            SetColor( 0 );
            exit( 0 );
        }

        if( LiteralLen == 2 )                           // If length of Operand is 2
        {
            if ( strlen(Operand) == 1 )
            {
                printf( "\n Warning : Line No.: %d :        0 Appended as higher nibble to make the literal 8 bit data constant \n",  LN );
                i = strtol( Operand, NULL, 16 );            // Convert Operand from string to long integer
                sprintf(Operand, "%02X", i);                // Convert Long integer to Hex ( 0 Appending )
            }
        }
        else if( LiteralLen == 4 )                      // If length of Operand is 2                                           // If length of Operand is not equal to 2 ( ie. is 4 )
        {
            if ( strlen(Operand) <= 3 )
            {
                printf( "\n Warning : Line No.: %d :        0 /0s Appended as higher nibble/s to make the literal 16 bit data constant \n",  LN );
                i = strtol( Operand, NULL, 16 );            // Convert Operand from string to Long integer
                sprintf(Operand, "%04X", i);                // Convert Long integer to Hex
            }
        }
    }

    /// Processing Decimal Literal

    if ( Dec_Flag == 1 )                                           // If Decimal
    {
        if (  !Are_All_Decimal_Digits(Operand)  )                  // Any Digit is Non-Decimal
        {
            SetColor( 12 );
            printf("\n ERROR IN LINE NO: %d :: LITERAL DOES NOT CONTAIN DECIMAL DIGITS  ( ERROR IN \"%s\" ) \n\a", LN, str );     // error
            SetColor( 0 );
            exit( 0 );
        }

        i = strtol( Operand, NULL, 10 );            // Convert Operand from string to long integer

        if ( LiteralLen == 2 )
        {
            if ( i > 255)
            {
                SetColor( 12 );
                printf("\n ERROR IN LINE NO: %d :: LITERAL SHOULD HAVE DECIMAL VALUE <= 255  ( ERROR IN \"%s\" ) \n\a", LN, str );     // error
                SetColor( 0 );
                exit( 0 );
            }

            sprintf(Operand, "%02X", i);             // Convert Long integer to Hex with 2 HEX Digits
        }
        else if ( LiteralLen == 4 )
        {
            if ( i > 65535 )
            {
                SetColor( 12 );
                printf("\n ERROR IN LINE NO: %d :: LITERAL SHOULD HAVE DECIMAL VALUE <= 65535   ( ERROR IN \"%s\" ) \n\a", LN, str );     // error
                SetColor( 0 );
                exit( 0 );
            }

            sprintf(Operand, "%04X", i);             // Convert Long integer to Hex with 4 HEX Digits
        }
    }

    /// For Default

    if ( (Hex_Flag == 0)  && (Dec_Flag == 0) )                         // Neither H nor I has been specified in literal
    {
        if (  !Are_All_Hexadecimal_Digits(Operand)  )                  // Any digit Non-Hex
        {
            SetColor( 12 );
            printf("\n ERROR IN LINE NO: %d :: LITERAL DOES NOT CONTAIN HEXADECIMAL DIGITS  ( ERROR IN \"%s\" ) \n\a", LN, str );     // error
            printf( " When the data Type Modifiers H or I are not mentioned in Literals, by default it assumes Hexadecimal Constants" ) ;
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen( Operand) > LiteralLen )                          // Is literal is bigger than LiteralLen
        {
            SetColor( 12 );
            printf("\n ERROR IN LINE NO: %d :: LITERAL SHOULD HAVE %d HEX DIGITS  ( ERROR IN \"%s\" ) \n\a", LN, LiteralLen, str );     // error
            SetColor( 0 );
            exit( 0 );
        }

        if( LiteralLen == 2 )                           // If LiteralLen == 2 ( Byte Literal : 2 Hex Digits)
        {
            i = strtol( Operand, NULL, 16 );            // Convert Operand from string to long integer
            sprintf(Operand, "%02X", i);                // Convert Long integer to 2 Hex Digits
        }
        else if( LiteralLen == 4 )                    // If LiteralLen == 4 (2 Byte Literal : 4 Hex Digits)
        {
            i = strtol( Operand, NULL, 16 );            // Convert Operand from string to Long integer
            sprintf(Operand, "%04X", i);                // Convert Long integer to 4 Hex Digits
        }
    }
}



int Error_Pass2( struct P1OUTPUTDEF P1OutputInst )
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
            printf("\n ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN LABEL OF OPERAND 1\n\n\a", P1OutputInst.LN );
            printf("\n POSSIBLY LABEL IN OPERAND 1 MAYBE MISSPELLED OR LITERAL SHOULD BE WRITTEN AS ' =XXH ' OR ' =XXXXH ' \n");
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1 ( SOUULD BE IN BETWEEN RST 0 TO RST 7 )\n\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 2\n\n\a", P1OutputInst.LN );
            printf("\n POSSIBLE REGISTERS SHOULD BE: A, B, C, D, E, H, L, M \n");
            SetColor( 0 );
            exit( 0 );
        }

        if( i == 7 && j == 7 )
        {
            SetColor( 12 );
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN BOTH OPERANDS ( BOTH SHOULD NOT BE M )\n\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN LABEL OF OPERAND 2\n\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN OPERAND 1\n\n\a", P1OutputInst.LN );
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
            printf("\n\n  ERROR :: LINE NO: %d HAS ERROR IN LABEL OF OPERAND 2\n\n\a", P1OutputInst.LN );
            printf("\n POSSIBLY LABEL IN OPERAND 2 MAYBE MISSPELLED OR LITERAL SHOULD BE WRITTEN AS ' =XXXXH ' \n");
            SetColor( 0 );
            exit(0);
        }
    }

}



int MOTGet_LS_Pass2( struct MOT MOTInst[], char ConcatenatedString[], struct P1OUTPUTDEF P1OutputInst )
{
    int i;
    char ConCatinatedMOTString[20];

    Error_Pass2( P1OutputInst );

    //Searching MOT

    for( i=1; i<N; ++i )
    {
        if( P1OutputInst.IType == 1 || P1OutputInst.IType == 3 || P1OutputInst.IType == 4 || P1OutputInst.IType == 5 )
        {
            // Making ConCatinatedMOTString by appending only [ Mnemonic ] as Op1 & Op2 both are absent

            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
        }
        else if( P1OutputInst.IType == 2 || P1OutputInst.IType == 6 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 ]  as Op2 is absent

            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
        }
        else if( P1OutputInst.IType == 7 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 || Operand2 ]

            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand2 );
        }
        else if( P1OutputInst.IType == 8 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 ]  as Op2 is absent

            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
        }
        else if( P1OutputInst.IType == 9 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 ]  as Op2 is absent

            strcpy( ConCatinatedMOTString, MOTInst[i].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[i].Operand1 );
        }

        if( !strcmp( ConCatinatedMOTString, ConcatenatedString ) )
        {
            break;
        }
    }

    return ( i );             // If search fails, we reached at the end of the MOT, --
}                             // so return with i (i.e. 247, as we have only 246 instructions in MOT)



///  MOTGET Binary Search

int MOTGet_BS_Pass2( struct MOT MOTInst[], char ConcatenatedString[], struct P1OUTPUTDEF P1OutputInst )
{
    int i;
    char ConCatinatedMOTString[20];
    unsigned int First, Last, Mid ;
    int p ;


    Error_Pass2( P1OutputInst );

    //Searching MOT

    First = 1;
    Last = N-1 ;

    while( First <= Last )                            // First > Last the search fails
    {
        Mid = (unsigned int)( (First+Last)/2 ) ;

        if( P1OutputInst.IType == 1 || P1OutputInst.IType == 3 || P1OutputInst.IType == 4 || P1OutputInst.IType == 5 )
        {
            // Making ConCatinatedMOTString by appending only [ Mnemonic ] as Op1 & Op2 both are absent

            strcpy( ConCatinatedMOTString, MOTInst[Mid].Mnemonic );
        }
        else if( P1OutputInst.IType == 2 || P1OutputInst.IType == 6 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 ]  as Op2 is absent

            strcpy( ConCatinatedMOTString, MOTInst[Mid].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[Mid].Operand1 );
        }
        else if( P1OutputInst.IType == 7 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 || Operand2 ]

            strcpy( ConCatinatedMOTString, MOTInst[Mid].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[Mid].Operand1 );
            strcat( ConCatinatedMOTString, MOTInst[Mid].Operand2 );
        }
        else if( P1OutputInst.IType == 8 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 ]  as Op2 is absent

            strcpy( ConCatinatedMOTString, MOTInst[Mid].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[Mid].Operand1 );
        }
        else if( P1OutputInst.IType == 9 )
        {
            // Making ConCatinatedMOTString by appending [ Mnemonic || Operand1 ]  as Op2 is absent

            strcpy( ConCatinatedMOTString, MOTInst[Mid].Mnemonic );
            strcat( ConCatinatedMOTString, MOTInst[Mid].Operand1 );
        }

        p = strcmp( ConcatenatedString, ConCatinatedMOTString ) ;   // Compare with Middle string in MOT

        if( p == 0 )                         // If Found
        {
            return( Mid ) ;                  // Return with Mid as an index to required "MOT entry"
        }
        else if( p < 0 )                     // If the string to be searched is < Mid entry in MOT
        {
            Last = Mid-1;                    // then it must be in upper half of the table, so Last = Mid-1;
        }
        else                                 // else the string to be searched must be > Mid entry in MOT
        {
            First = Mid+1;                   // so, it must be in lower half of the table, so First = Mid+1;
        }
    }

    return ( N );                        // If search fails, return with N (i.e. 247, as we have only 246 instructions in MOT)
}



int ProcessPseudoOps_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP, unsigned int p )
{
    struct P1OUTPUTDEF Temp;
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
        P1OutputInst.Comment[1] = '\0';           // And terminate the Operand 2 string
    }

    if( p == 1)                                               //  DB : Define Byte  ( THREE : DB 03  // Define single Byte)
    {
        ProcessDB_Pass2( P1OutputInst, P2OutputFP );          // Process DB pseudo op
    }
    else if( p == 2 )                                         //  DS : Define Storage ( DATA : DS 4  // Reserve 4 Bytes )
    {
        ProcessDS_Pass2( P1OutputInst, P2OutputFP );          // Process DS pseudo op
        // Code of this function is same as that of ProcessEND_Pass2   #####
    }
    else if( p == 3)                                          //  DW : Define Word  ( DATA16 : DW F0F0  // Define 2 Bytes )
    {
        ProcessDW_Pass2( P1OutputInst, P2OutputFP );          // Process DW pseudo op
    }
    else if( p == 4 )                                         //  END of code block but may not be the end of program
    {
        ProcessEND_Pass2( P1OutputInst, P2OutputFP );         // Process END pseudo op
    }
    else if( p == 5 )                                         //  EQU : Define Symbol  ( COUNTER : EQU C  // Reg C may be reserved as counter )
    {
        ProcessEQU_Pass2( P1OutputInst, P2OutputFP );         // Process EQU pseudo op
    }
    else if( ( p == 6 ) || ( p == 7 ) )                        // START (ORG) : Start of Code Block ( PROG : START 20F0H ) OR ( PROG : ORG 20F0H )
    {
        ProcessORGorSTART_Pass2( P1OutputInst, P2OutputFP );   // Process START (ORG) pseudo op
    }
}



int ProcessDW_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP )
{
    int p, n, Hex_Flag, Dec_Flag ;
    char LowerByte[3], HigherByte[3];                             // For storing higher and lower byte
    char str[ OPERAND1_LENGTH + 1 ] ;

    strcpy( str, P1OutputInst.Operand1 );                         // Setting aside Operand1 for writing Source File instruction as it is

    /// Identify and Remove 'H' or 'h' if present in Op1

    Hex_Flag = 0 ;    Dec_Flag = 0 ;                                                                       // Initializing the counter

    n = strlen(P1OutputInst.Operand1);

    if ( P1OutputInst.Operand1[n-1] == 'h' || P1OutputInst.Operand1[n-1] == 'H')
    {
        P1OutputInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Hex_Flag = 1 ;
    }
    else if ( P1OutputInst.Operand1[n-1] == 'i' || P1OutputInst.Operand1[n-1] == 'I')
    {
        P1OutputInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Dec_Flag = 1 ;
    }

    if ( Hex_Flag == 1 )                                                           // If hex digits indicated by 'h' or 'H'
    {
        if (  !Are_All_Hexadecimal_Digits(P1OutputInst.Operand1)  )                  // Are All Hex
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DW %s )\n ", P1OutputInst.LN , str ) ;
            SetColor( 0 );
            exit( 0 );
        }
        if ( strlen(P1OutputInst.Operand1) > 4 )                                   // if Length > 4
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Hex Digits value Should be <= FFFF (DW %s)  \n ", P1OutputInst.LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }
        if ( strlen(P1OutputInst.Operand1) < 4 )                            // If length is < 4, make it 4 by pre-pending 0s
        {
            p = (int) strtol( P1OutputInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
            sprintf( P1OutputInst.Operand1, "%04X", p);                     // Convert integer to Hex with 4 digits
        }
    }

    if ( Dec_Flag == 1 )                                                    // If Decimal Digits indicated by 'd' or 'D'
    {
        if (  Are_All_Decimal_Digits( P1OutputInst.Operand1 )  )            // Check for Decimal Digits
        {
            p = (int) strtol( P1OutputInst.Operand1, NULL, 10 );            // Convert Operand from string to integer

            if ( p <= 65535 )
            {
                sprintf( P1OutputInst.Operand1, "%04X", p);                 // Convert integer to Hex with 4 digits
            }
            else                                                            // else error of not <= 65535 (i.e. If > 65535)
            {
                SetColor( 12 );
                printf( "\n Error : Line No. # %d  :  Decimal Value should be <= 65535 (DW %s) \n ", P1OutputInst.LN, str ) ;
                SetColor( 0 );
                exit( 0 );
            }
        }
        else                                                               // Else Error of not Decimal digits
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Decimal (DW %s) \n", P1OutputInst.LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }

    if ( (Hex_Flag == 0)  &&   (Dec_Flag == 0) )       //  Neither ['h' or 'H'] nor ['i' or 'I'] is indicated, then treat as Hex & check
    {
        if (  !Are_All_Hexadecimal_Digits(P1OutputInst.Operand1)  )                  // Is any digit not Hex, then ERROR
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DW %s )\n ", P1OutputInst.LN , str ) ;
            printf( "\n When neither H(or h) nor I(or i) is indicated in DW pseudo op, data bytes are treated as HEX ");
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen(P1OutputInst.Operand1) > 4 )                                   // if Length > 4
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Hex Digits value Should be <= FFFF (DW %s)  \n ", P1OutputInst.LN, str ) ;
            printf( "\n When neither H(or h) nor I(or i) is indicated in DW pseudo op, data bytes are treated as HEX ");
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen(P1OutputInst.Operand1) < 4 )                            // If length < 4
        {
            p = (int) strtol( P1OutputInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
            sprintf( P1OutputInst.Operand1, "%04X", p);                     // Convert integer to Hex with 4 digits
        }
    }



    // Include here error check and provision for allowing Decimal word

    HigherByte[0] = P1OutputInst.Operand1[0];       // Copy the lower byte's first character into LowerByte
    HigherByte[1] = P1OutputInst.Operand1[1];       // Copy the lower byte's Second character into LowerByte
    HigherByte[2] = '\0';                           // End the LowerByte string

    LowerByte[0] = P1OutputInst.Operand1[2];        // Copy the higher byte's first character into HigherByte
    LowerByte[1] = P1OutputInst.Operand1[3];        // Copy the higher byte's second character into HigherByte
    LowerByte[2] = '\0';                            // End the HigherByte string

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Print the location counter of the LowerByte in the file
    fprintf( P2OutputFP, "%-4s  ", LowerByte );                // Print the LowerByte in the file

    //fprintf( P2OutputFP, "%-4s", " " );                      // Blank spaces for Operand 2

    /// Writing Original Instruction in Final Assembly File

    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", str );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );   // Print Source Program LNs in Assembled file
    }
    else                                                          // else
    {
        fprintf( P2OutputFP, "\n" );                              //  Don't print Source Program LNs in Assembled file
    }

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC + 1 );      // Print the location counter of the HigherByte in the file
    fprintf( P2OutputFP, "%-4s  \n", HigherByte );             // Print the HigherByte in the file


}



int ProcessDB_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP )
{
    int p, n;
    int Hex_Flag, Dec_Flag ;
    char str[ OPERAND1_LENGTH + 1 ] ;

    strcpy( str, P1OutputInst.Operand1 );                    // Setting aside Operand1 for writing Source File instruction as it is

    /// Identify and Remove 'H' or 'h' if present in Op1

    Hex_Flag = 0 ;    Dec_Flag = 0 ;                                                                       // Initializing the counter

    n = strlen(P1OutputInst.Operand1);

    if ( P1OutputInst.Operand1[n-1] == 'h' || P1OutputInst.Operand1[n-1] == 'H')
    {
        P1OutputInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Hex_Flag = 1 ;
    }
    else if ( P1OutputInst.Operand1[n-1] == 'i' || P1OutputInst.Operand1[n-1] == 'I')
    {
        P1OutputInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Dec_Flag = 1 ;
    }

    if ( Hex_Flag == 1 )                                                           // If hex digits indicated by 'h' or 'H'
    {
        if (  !Are_All_Hexadecimal_Digits(P1OutputInst.Operand1)  )                  // Are All Hex
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DB %s )\n ", P1OutputInst.LN , str ) ;
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen(P1OutputInst.Operand1) > 2 )                                   // if Length > 2
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Hex Digits value Should be <= FF (DB %s)  \n ", P1OutputInst.LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen(P1OutputInst.Operand1) < 2 )                            // If length is < 2, make it 2 by pre-pending 0s
        {
            p = (int) strtol( P1OutputInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
            sprintf( P1OutputInst.Operand1, "%02X", p);                     // Convert integer to Hex with 4 digits
        }
    }

    if ( Dec_Flag == 1 )                                                  // If Decimal Digits indicated by 'd' or 'D'
    {
        if (  Are_All_Decimal_Digits( P1OutputInst.Operand1 )  )            // Check for Decimal Digits
        {
            p = (int) strtol( P1OutputInst.Operand1, NULL, 10 );            // Convert Operand from string to integer

            if ( p <= 255 )
            {
                sprintf( P1OutputInst.Operand1, "%02X", p);                 // Convert integer to Hex with 2 digits
            }
            else                                                            // else error of not <= 255 (i.e. If > 255)
            {
                SetColor( 12 );
                printf( "\n Error : Line No. # %d  :  Decimal Value should be <= 255 (DB %s) \n ", P1OutputInst.LN, str ) ;
                SetColor( 0 );
                exit( 0 );
            }
        }
        else                                                               // Else Error of not Decimal digits
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Decimal (DB %s) \n", P1OutputInst.LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }

    /// Default case (Neither ['h' or 'H'] nor ['i' or 'I'] is indicated), HEX is treated

    if ( (Hex_Flag == 0)  &&   (Dec_Flag == 0) )       //  Neither ['h' or 'H'] nor ['i' or 'I'] is indicated, then treat as Hex & check
    {
        if (  !Are_All_Hexadecimal_Digits(P1OutputInst.Operand1)  )                  // Is any digit not Hex, then ERROR
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DB %s )\n ", P1OutputInst.LN , str ) ;
            printf( "\n When neither H(or h) nor I(or i) is indicated in DB pseudo op, data byte is treated as HEX ");
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen(P1OutputInst.Operand1) > 2 )                                   // if Length > 2
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Hex Digits value Should be <= FF (DB %s)  \n ", P1OutputInst.LN, str ) ;
            printf( "\n When neither H(or h) nor I(or i) is indicated in DW pseudo op, data byte is treated as HEX ");
            SetColor( 0 );
            exit( 0 );
        }

        if ( strlen(P1OutputInst.Operand1) < 2 )                            // If length is < 2, make it 2 by pre-pending 0s
        {
            p = (int) strtol( P1OutputInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
            sprintf( P1OutputInst.Operand1, "%02X", p);                     // Convert integer to Hex with 4 digits
        }
    }

    /// Writing Final Assembly File

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );           // Put LC in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Operand1 );         // Put Op1 in P2Output File

    /// Writing Original Instruction in Final Assembly File

    fprintf( P2OutputFP, "%-4s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );         // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );       // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", str );                        // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );      // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );          // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );    // Print Source Program LNs in Assembled file
    }
    else                                                           // else
    {
        fprintf( P2OutputFP, "\n" );                               //  Don't print Source Program LNs in Assembled file
    }
}



int ProcessDS_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP )
{
    int Opernad1, i, n, p, Hex_Flag, Dec_Flag;
    char str[ OPERAND1_LENGTH + 1 ] ;

    strcpy( str, P1OutputInst.Operand1 );                    // Setting aside Operand1 for writing Source File instruction as it is

    Hex_Flag = 0 ;    Dec_Flag = 0 ;                                            // Initializing

    n = strlen(P1OutputInst.Operand1);

    if ( P1OutputInst.Operand1[n-1] == 'h' || P1OutputInst.Operand1[n-1] == 'H')
    {
        P1OutputInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Hex_Flag = 1 ;
    }
    else if ( P1OutputInst.Operand1[n-1] == 'i' || P1OutputInst.Operand1[n-1] == 'I')
    {
        P1OutputInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Dec_Flag = 1 ;
    }

    if ( Hex_Flag == 1 )                                                           // If hex digits indicated by 'h' or 'H'
    {
        if (  !Are_All_Hexadecimal_Digits(P1OutputInst.Operand1)  )                  // Are All Hex
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DS %s )\n ", P1OutputInst.LN , str ) ;
            SetColor( 0 );
            exit( 0 );
        }
        p = (unsigned int) strtol( P1OutputInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
        sprintf( P1OutputInst.Operand1, "%d", p);
        //printf( "\n %d \n ", p);
    }

    if ( Dec_Flag == 1 )                                                    // If Decimal Digits indicated by 'd' or 'D'
    {
        if (  Are_All_Decimal_Digits( P1OutputInst.Operand1 )  )            // Check for Decimal Digits
        {
            //  Nothing to be done here                                     // If all Digits are decimal, nothing to be done here
        }
        else                                                                // Else Error of not Decimal digits
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Decimal (DS %s) \n", P1OutputInst.LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }

    if ( (Hex_Flag == 0)  &&   (Dec_Flag == 0) )       //  Neither ['h' or 'H'] nor ['i' or 'I'] is indicated, then treat as Decimal & check
    {
        if (  !Are_All_Hexadecimal_Digits(P1OutputInst.Operand1)  )                  // Is any digit not Hex, then ERROR
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DS %s )\n ", P1OutputInst.LN , str ) ;
            printf( " When the data Type Modifiers H or I are not mentioned in DS by default it assumes Hexadecimal Constant " ) ;
            SetColor( 0 );
            exit( 0 );
        }
        p = (unsigned int) strtol( P1OutputInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
        sprintf( P1OutputInst.Operand1, "%d", p);
    }

    fprintf( P2OutputFP, "%04X  ", P1OutputInst.LC );          // Put LC in P2Output File
    fprintf( P2OutputFP, "%-6s", " " );                        // Put Blank spaces in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", str );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                        // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );     // Print Source Program LNs in Assembled file
    }
    else                                                            // else
    {
        fprintf( P2OutputFP, "\n" );                                //  Don't print Source Program LNs in Assembled file
    }

    /// Print further Lines

    Opernad1 = atoi( P1OutputInst.Operand1 );                     // Determine the integer value of Operand1 as we need to print value of LC that many Lines in P2Output File
                                                                  // We want to reserve no. of bytes mentioned in Operane1
    for( i=1; i<(Opernad1 ); ++i)
    {
        fprintf( P2OutputFP, "%04X  \n", (P1OutputInst.LC+i) ); // Put the LC in P2Output File with a increment of i
    }
}



int ProcessEQU_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP )
{

    fprintf( P2OutputFP, "%-12s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                          // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );       // Print Source Program LNs in Assembled file
    }
    else                                                              // else
    {
        fprintf( P2OutputFP, "\n" );                                  //  Don't print Source Program LNs in Assembled file
    }
}



int ProcessEND_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%-12s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );        // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );      // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );     // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );     // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );         // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                       // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );    // Print Source Program LNs in Assembled file
    }
    else                                                           // else
    {
        fprintf( P2OutputFP, "\n" );                               //  Don't print Source Program LNs in Assembled file
    }
}

int ProcessORGorSTART_Pass2( struct P1OUTPUTDEF P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%-12s", " " );
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Label );               // Put Label in P2Output File
    fprintf( P2OutputFP, "%-8s", P1OutputInst.Mnemonic );             // Put Mnemonic in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );            // Put Operand 1 in P2Output File
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand2 );            // Put Operand 2 in P2Output File
    fprintf( P2OutputFP, "%s", P1OutputInst.Comment );                // Put Comment in P2Output File

    if( P1OutputInst.LN_Output_Flag == 'Y' )                           // if true
    {
        fprintf( P2OutputFP, "  S_LN# %d\n", P1OutputInst.LN );        // Print Source Program LNs in Assembled file
    }
    else                                                               // else
    {
        fprintf( P2OutputFP, "\n" );                                   //  Don't print Source Program LNs in Assembled file
    }
}



int STGet_Pass2( char Operand1[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries )
{
    int i;

    /// Searching Symbol Table for the value of a given symbol

    for (i=0; i<NoOfSTEntries; ++i)                        // Up to the end of ST
    {
        if( !strcasecmp( Operand1, ST[i].Symbol ) )        // If Symbol Found
        {
            return( i );                                   // Return with the index of match
        }
    }

    return( -88 );                                         // Else Return -88
}


struct P1OUTPUTDEF GetP1OutputInst_Pass2( FILE *P1OutputFP )
{
    /// Reads the intermediate file created by Pass1 and puts all the fields of instruction  in P1OutputInst

    struct P1OUTPUTDEF P1OutputInst;

    fscanf( P1OutputFP, "%u", &P1OutputInst.LN );                // Reads Source Program Line Numbers
    fscanf( P1OutputFP, "%X", &P1OutputInst.LC );                // Reads LC value
    fscanf( P1OutputFP, "%s", P1OutputInst.Label );              // Reads Label
    fscanf( P1OutputFP, "%s", P1OutputInst.Mnemonic);            // Reads Mnemonic
    fscanf( P1OutputFP, "%s", P1OutputInst.Operand1 );           // Reads Operand1
    fscanf( P1OutputFP, "%s", P1OutputInst.Operand2 );           // Reads Operand2
    fscanf( P1OutputFP, "%d", &P1OutputInst.IType );             // Reads Instruction Type
    fscanf( P1OutputFP, " %[^\n]\n", P1OutputInst.Comment );     // Reads Comment

    return( P1OutputInst );
}



int ReadSymbolTable_Pass2( struct SYMBOLTABLEDEF ST[] )
{
    FILE *STFP;  // Symbol Table File Pointer
    int NoOfSTEntries;
    char temp[16];

    // Reading Symbol Table from file and store it in a structural array ST[]
    // ST[] is transferred to calling program by "call by reference method"  ( ST[] is an array of structures )
    // It returns integer value of no. of entries found in symbol Table

    if( ( STFP = fopen( "SymbolTable.txt", "r" ) ) == NULL )                     // Open SymbolTable.txt file
    {
        SetColor( 12 );
        printf( "Error opening Pass1Output.txt file for read purpose\n\n\a" );
        SetColor( 0 );
        exit( 0 );
    }

    NoOfSTEntries = 0;

    while( !feof( STFP ) )
    {
        fscanf( STFP, "%s", ST[NoOfSTEntries].Symbol );           // Read Symbol
        fscanf( STFP, "%s\n", ST[NoOfSTEntries].Value );          // Read value of symbol

        NoOfSTEntries = NoOfSTEntries + 1;                        // Increment counter (for counting no of entries in ST)
    }

    fclose( STFP );                                               // Close SymbolTable.txt file

    return( NoOfSTEntries );                                      // No of Entries found in ST
}



int Write_Pass1_OutputToFile( struct SPROG SourceInst, unsigned int LC, FILE *fp3, int Type, unsigned int LN )
{
        /// Writing the intermediate O/P Line of Pass1 into a file which is to be used as an I/P for Pass2
        /// The Instruction Fields which are absent (if 0th element is '\0') are written by ? in the file
        /// Mnemonic will always present for both M/C op or Pseudo op
        /// Label, Operand1, Operand2, Comment may be absent
        /// If comment is absent it is written as it is as it is the last element in a line to be printed

        if( SourceInst.Label[0] == '\0')                              // If there is no label then
        {
            SourceInst.Label[0] = '?'; SourceInst.Label[1] = '\0';    // put "?" at that location and then put \0 as, while reading the file \0 is not identified
        }
        if( SourceInst.Operand1[0] == '\0')                           // If there is no Operand1 then
        {
            SourceInst.Operand1[0] = '?'; SourceInst.Operand1[1] = '\0';   // put "?" at that location and then put \0 as, while reading the file \0 is not identified
        }
        if( SourceInst.Operand2[0] == '\0')                                // If there is no Operand1 then
        {
            SourceInst.Operand2[0] = '?'; SourceInst.Operand2[1] = '\0';   // put "?" at that location and then put \0 as, while reading the file \0 is not identified
        }

        /// Writing processed line by pass1 in intermediate (Pass1 O/P File) file

        fprintf( fp3, "%04u    ", LN );                      // Write Source File Line Number in Pass1 O/P File
        fprintf( fp3, "%04X    ", LC );                      // Write LC value in Pass1 O/P File
        fprintf( fp3, "%-15s", SourceInst.Label );           // Write Label in Pass1 O/P File
        fprintf( fp3, "%-12s", SourceInst.Mnemonic );        // Write Mnemonic in Pass1 O/P File
        fprintf( fp3, "%-15s", SourceInst.Operand1 );        // Write Opertand1 in Pass1 O/P File
        fprintf( fp3, "%-15s", SourceInst.Operand2 );        // Write Opertand2 in Pass1 O/P File
        fprintf( fp3, "%-7d", Type );                        // Write Instruction Type in Pass1 O/P File
        fprintf( fp3, "%s\n", SourceInst.Comment );          // Write Comment in Pass1 O/P File
}



int CheckSymbolTableForDuplicates_Pass1( void )
{
    /// Checking symbol table for any duplicate ( ie. repeated ) symbols
    /// If duplicate Symbol is Found it prints error msg with duplicate symbol and exit to OS
    /// In any case it does not return any value.

    FILE *fp;
    char Label[MAX_NO_OF_ST_ENTRIES][LABEL_LENGTH + 1], LabelValue[LABEL_VALUE_LENGTH + 1];  //**
    int i, j, n;

    if( ( fp = fopen( "SymbolTable.txt","r" ) ) == NULL )        // Opening a file for checking duplicates for Symbol
    {
        SetColor( 12 );
        printf( "Error opening symbol Table for reading purpose\n\a" );  //error
        SetColor( 0 );
        exit( 0 );
    }

    n = 0;                                         // Acting as a counter to count the number of elements in the file

    while( !feof( fp ) )                           // Reading the file till EOF occurs
    {
        fscanf( fp, "%s", Label[n] );              // Reading the Label stored in the file and saving it in Label array
        fscanf( fp, "%s", LabelValue );            // Reading the Value of the Label which is stored in-front of the label in the file ( not to be used )

        n = n + 1;                                 // Incrementing the counter
    }

    /// Remember, there is a blank line at the end of ST. So if there are 5 (n = 0 ..4) symbols in ST file 5th line will be blank line
    /// So n here will be 6. So we need to put '\0' on starting of 5th line, i.e at 0th location of (n-1)st line
    /// (n-2)nd line contain last symbol.
    /// Last comparison  : So (n-3)rd [i.e. Last but one] symbol is to be compared with (n-2)nd [i.e. Last]
    /// First comparison : symbol in 0th position is to be compared with symbol in 1st position.

    Label[n-1][0] = '\0';                          // As there is a empty line at the end of the file to discard it we put \0 on that line's 0th position

    for( i =0; i<=(n-3); ++i)                      // First Symbol at 0th Position & Last but one symbol is at (n-3)rd position
    {
        for(j=i+1; j<= (n-2); ++j)                 // Last Symbol is at (n-2)nd position
        {

            if( !strcmp( Label[i], Label[j] ) )
            {
                SetColor( 12 );
                printf( "\n\n DUPLICATE LABEL FOUND : %s\n\n\a ", Label[i] );      //error MSG indicating duplicate label
                SetColor( 0 );
                exit( 0 );                                                         // Exit to OS in case of Duplicate label
            }
        }
    }

    fclose( fp );                                                                     // Close ST file

    return (n-1) ;                          // Return with No. of Symbols found in the source program file

}



int ProcessPseudoOps_Pass1( int p, struct SPROG SourceInst, unsigned int *LC, FILE *fp2 , unsigned int LN)
{
    int Length;

    /// Processing the PseudoOp's

    if( p == 1 )
    {
        Length = ProcessDB_Pass1( SourceInst, LC, fp2, LN );           // Process DB pseudoOp
    }
    else if( p == 2)
    {
        Length = ProcessDS_Pass1( SourceInst, LC, fp2, LN );           // Process DS pseudoOp
    }
    else if( p == 3)
    {
        Length = ProcessDW_Pass1( SourceInst, LC, fp2, LN );           // Process DW pseudoOp
    }
    else if( p == 4)
    {
        Length = ProcessEND_Pass1( SourceInst, LC, fp2, LN );          // Process END pseudoOp
    }
    else if( p == 5)
    {
        Length = ProcessEQU_Pass1( SourceInst, LC, fp2, LN );          // Process EQU pseudoOp
    }
    else if( p == 6 || p == 7)
    {
        Length = ProcessORGorSTART_Pass1( SourceInst, LC, fp2, LN);    // Process ORG or START pseudoOp
    }

    return( Length );                                                  // Return length
}



int ProcessDW_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )   // Define Word Pseudo op Processing
{
    if( SourceInst.Label[0] != '\0' )
    {
        STSTO_Pass1( fp2, SourceInst, *LC ) ;
    }

    return( 2 );       // Returning Length of DW = 2 ( ie. the space required to store double byte constant )
}



int ProcessDB_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  // Processing Define Byte PseudoOp
{
    if( SourceInst.Label[0] != '\0' )                // if the Label is not empty
    {
        STSTO_Pass1( fp2, SourceInst, *LC ) ;        // store it in symbol table
    }

    return( 1 );       // Returning Length of DB = 1 ( ie. the space required to store single byte constant )
}



int ProcessEND_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )   // Processing the PseudoOp END
{
    if( SourceInst.Label[0] != '\0' )            // if the Label is not empty, ie. Is label present ?
    {
        STSTO_Pass1( fp2, SourceInst, *LC ) ;    // store it in symbol table
    }

    return(0);                                   // Returning length of END = 0 ( ie. the space required to store 0 byte constant )
}



int ProcessDS_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )   // Processing the PseudoOp DS
{
    unsigned int OP1, n, p, Hex_Flag, Dec_Flag;
    char str[ OPERAND1_LENGTH + 1 ];

    strcpy( str, SourceInst.Operand1 );                    // Setting aside Operand1 for writing Source File instruction as it is

    Hex_Flag = 0 ;    Dec_Flag = 0 ;                                            // Initializing

    n = strlen(SourceInst.Operand1);

    if ( SourceInst.Operand1[n-1] == 'h' || SourceInst.Operand1[n-1] == 'H')
    {
        SourceInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Hex_Flag = 1 ;
    }
    else if ( SourceInst.Operand1[n-1] == 'i' || SourceInst.Operand1[n-1] == 'I')
    {
        SourceInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Dec_Flag = 1 ;
    }

    if ( Hex_Flag == 1 )                                                           // If hex digits indicated by 'h' or 'H'
    {
        if (  !Are_All_Hexadecimal_Digits(SourceInst.Operand1)  )                  // Are All Hex
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DS %s )\n ", LN , str ) ;
            SetColor( 0 );
            exit( 0 );
        }

        p = (unsigned int) strtol( SourceInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
        sprintf( SourceInst.Operand1, "%d", p);
    }

    if ( Dec_Flag == 1 )                                                  // If Decimal Digits indicated by 'd' or 'D'
    {
        if (  Are_All_Decimal_Digits( SourceInst.Operand1 )  )            // Check for Decimal Digits
        {
            //p = (unsigned int) strtol( SourceInst.Operand1, NULL, 10 );            // Convert Operand from string to integer
            //sprintf( SourceInst.Operand1, "%d", p);
        }
        else                                                               // Else Error of not Decimal digits
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Decimal (DS %s) \n", LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }

    if ( (Hex_Flag == 0)  &&   (Dec_Flag == 0) )       //  Neither ['h' or 'H'] nor ['i' or 'I'] is indicated, then treat as Hex & check
    {
        if (  !Are_All_Hexadecimal_Digits(SourceInst.Operand1)  )                  // Is any digit not Hex, then ERROR
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( DS %s )\n ", LN , str ) ;
            printf( " When the data Type Modifiers H or I are not mentioned in DS by default it assumes Hexadecimal Constant " ) ;
            SetColor( 0 );
            exit( 0 );
        }
        p = (unsigned int) strtol( SourceInst.Operand1, NULL, 16 );            // Convert Operand from string to integer
        sprintf( SourceInst.Operand1, "%d", p);
    }

    OP1 = (unsigned int) atoi(SourceInst.Operand1);                             // converting back to int for computation of length required for DS
    STSTO_Pass1( fp2, SourceInst, *LC );

    return( OP1 );       // Returning length of DS = OP1 ( ie. the space required to store no of bytes mentioned in OP1  )
}



int ProcessEQU_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  //Processing the PseudoOp EQU
{
    unsigned int temp=0;
    int i, Hex_Flag, Dec_Flag, p, n ;
    char str[ OPERAND1_LENGTH + 1 ];

    strcpy( str, SourceInst.Operand1 );                    // Setting aside Operand1 for writing Source File instruction as it is

    Hex_Flag = 0 ;    Dec_Flag = 0 ;                                            // Initializing

    n = strlen(SourceInst.Operand1);

    if ( SourceInst.Operand1[n-1] == 'h' || SourceInst.Operand1[n-1] == 'H')
    {
        SourceInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Hex_Flag = 1 ;
    }
    else if ( SourceInst.Operand1[n-1] == 'i' || SourceInst.Operand1[n-1] == 'I')
    {
        SourceInst.Operand1[n-1] = '\0';                                       // Terminate the string there
        Dec_Flag = 1 ;
    }

    /// Test 3 Conditions : 1) LABEL1 : EQU *      2) LABEL1 : EQU 3FH       3) LABEL1 : EQU 2095D

    if( !strcmp( SourceInst.Operand1, "*" ) )               // If * then  ( '*' means current value of LC)
    {
        if ( *LC > 65535 )                // If current value of LC > 65535, Flag error and exit to OS
        {
            SetColor( 12 );
            printf("\n ERROR :: Line No. # %d : Location Counter Value should be < FFFF ( LC = %d) \n\a", LN, LC );
            SetColor( 0 );
            exit( 0 );
        }
        STSTO_Pass1( fp2, SourceInst, *LC );                // store the value symbol = current value of LC ( which is integer )
    }

    else if( Is_Register(SourceInst.Operand1) )             // Are resistors [ A, B, C, D, E, H, L, SP, PSW ] in Operand1
    {
        STSTO_Pass1( fp2, SourceInst, temp );               // temp is used as a dummy variable
    }

    else if ( Hex_Flag == 1 )                               // if source operand is Hex   2) LABEL1 : EQU 3FH
    {
        if (  Are_All_Hexadecimal_Digits( SourceInst.Operand1 )  )   // Check For Hexadecimal Digits
        {
            p = (int) strtol( SourceInst.Operand1, NULL, 16 );            // Convert Operand1 from string to long integer base 16

            if ( strlen(SourceInst.Operand1) <=2 )
            {
                if ( strlen(SourceInst.Operand1) == 1 )
                {
                    printf( "\n Warning : Line No.: %d :        0 Appended as higher nibble to make 8 bit data constant \n",  LN );
                }
                sprintf( SourceInst.Operand1, "%02X", p);                 // Convert Long integer to Hex with 2 digits (0 Appending)
            }
            else if (  strlen(SourceInst.Operand1) > 2 &&  strlen(SourceInst.Operand1) <= 4 )
            {
                if ( strlen(SourceInst.Operand1) == 3 )
                {
                    printf( "\n Warning : Line No.: %d :        0 Appended as higher nibble to make 16 bit data constant \n",  LN );
                }
                sprintf( SourceInst.Operand1, "%04X", p);                 // Convert Long integer to Hex with 4 digits (0 appending)
            }
            else
            {
                SetColor( 12 );
                printf("\n ERROR :: Line No. # %d : Operand1 in EQU pseudo op is > FFFF (or > 65535I) [ %s  %s  %s ]\n\a", LN, SourceInst.Label, SourceInst.Mnemonic, str );
                printf("\n It should be <= FFFF (or <= 65535I)" );
                SetColor( 0 );
                exit( 0 );
            }

            STSTO_Pass1( fp2, SourceInst, temp );                    // store the value symbol = as it is in operand 1. Here temp is DUMMY argument passed
        }
        else
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal (EQU %s)\n ", LN, str ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }

    else if ( Dec_Flag == 1 )                                           // Then Operand must be decimal  3) LABEL1 : EQU 2095D
    {
        if (  Are_All_Decimal_Digits( SourceInst.Operand1 )  )            // Check for Decimal Digits
        {
            p = (int) strtol( SourceInst.Operand1, NULL, 10 );            // Convert Operand from string to long integer

            if ( p > 65535)                                               // Decimal value of operand1 should be < 65535
            {
                SetColor( 12 );
                printf("\n ERROR :: Line No. # %d : Operand1 in EQU pseudo op is > FFFF (or > 65535I) [ %s  %s  %s ]\n\a", LN, SourceInst.Label, SourceInst.Mnemonic, str );
                printf("\n It should be <= FFFF (or <= 65535I)" );
                SetColor( 0 );
                exit( 0 );
            }

            if ( p <= 255 )
            {
                sprintf( SourceInst.Operand1, "%02X", p);                 // Convert Long integer to Hex with 2 digits
            }
            else
            {
                sprintf( SourceInst.Operand1, "%04X", p);                 // Convert Long integer to Hex with 4 digits
            }

            STSTO_Pass1( fp2, SourceInst, temp );                         // Write the result in ST
        }
        else
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Decimal [ %s  %s  %s ] \n", LN, SourceInst.Label, SourceInst.Mnemonic, str ) ;
            SetColor( 0 );
            exit( 0 );

        }
    }

    else if ( (Hex_Flag == 0)  &&   (Dec_Flag == 0) )       //  Neither ['h' or 'H'] nor ['i' or 'I'] is indicated, then treat as Hex & check
    {
        if (  !Are_All_Hexadecimal_Digits(SourceInst.Operand1)  )                  // Is any digit not Hex, then ERROR
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d  :  Digits are not Hexadecimal ( EQU %s )\n ", LN , str ) ;
            printf( " When the data Type Modifiers H or I are not mentioned in EQU pseudo op, by default it assumes Hexadecimal Constant" ) ;
            SetColor( 0 );
            exit( 0 );
        }

        p = (int) strtol( SourceInst.Operand1, NULL, 16 );            // Convert Operand1 from string to long integer base 16

        if ( strlen(SourceInst.Operand1) <=2 )
        {
            sprintf( SourceInst.Operand1, "%02X", p);                 // Convert Long integer to Hex with 2 digits
        }
        else
        {
            sprintf( SourceInst.Operand1, "%04X", p);                 // Convert Long integer to Hex with 4 digits
        }

        STSTO_Pass1( fp2, SourceInst, temp );                         // Write the result in ST
    }

    return(0);                        // Returning length of EQU = 0 ( ie. the space required to store 0 byte constant )
}



int ProcessORGorSTART_Pass1( struct SPROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  // Processing the PseudoOps ORG and START ( both are same )
{
    int Len;
    char str[ OPERAND1_LENGTH + 1 ];

    strcpy( str, SourceInst.Operand1 );                    // Setting aside Operand1 for writing Source File instruction as it is

    if( !strcmp( SourceInst.Operand1, "*" ) )            // If * is present, then assign current value of the location to LC
    {
        *(LC) = *(LC);
    }
    else                                                 // If * is not present
    {
        Len = strlen( SourceInst.Operand1 );             // Calculating the length of the operand 1 for removing the character H from operand 1 ( if present )

        if( SourceInst.Operand1[Len - 1] == 'H' || SourceInst.Operand1[Len - 1] == 'h')  // If there is H or h at the end of operand ( which stands for hexadecimal )
        {
            SourceInst.Operand1[Len  - 1] = '\0';                           // Replace it with \0
        }

        if( Are_All_Hexadecimal_Digits(SourceInst.Operand1) )
        {
            *(LC) = ( unsigned int ) strtol( SourceInst.Operand1, NULL, 16 );   // Convert the value of LC from Hex to decimal but -
        }                                                                        // - while storing in symbol table it is stored in Hex
        else
        {
            SetColor( 12 );
            printf( "\n Error : Line No. # %d : Digits are not Hexadecimal ( %s %s )\n ", LN , SourceInst.Mnemonic, str ) ;
            printf( "\n Only Hexadecimal Address is permitted in START Pseudo op \n" ) ;
            SetColor( 0 );
            exit( 0 );
        }
    }

    if ( SourceInst.Label[0] != '\0')                        //
    {
        STSTO_Pass1( fp2, SourceInst, *LC );                 // Symbol table storage
    }

    return(0);                    // Returning length of ORGOrSTART = 0 ( ie. the space required to store 0 byte constant )
}



int IsPseudoOp_Pass1( char Mnemonic[], char POT[][6] )
{
    /// Check for Pseudo Op:
    /// if yes then return the index of that Pseudo Op in POT
    /// else return 0

    int i;

    i = 1;                                         // as zeroth element is dummy

    while( POT[i][0] != '\0' )                     // till the last element comes ie. \0
    {
        //printf("\PseudoOp:%s\n", Mnemonic);

        if( !strcmp( Mnemonic, POT[i] ) )          // If PseudoOp is found then return its index ( ie. if the mnemonic matches PseudoOp )
        {
            return(i);
        }
        i = i + 1;
    }
    return( 0 );                                            // Else return 0
}



void STSTO_Pass1( FILE *fp2, struct SPROG SourceInst, unsigned int LC )  // Symbol table storage
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

    if ( LC > 65535 )
    {
        SetColor( 12 );
        printf( "\n Error Writing ST STSTO : Memory Address (%04XH) exceeds FFFFH : \n", LC ) ;
        SetColor( 0 );
        exit( 0 );
    }

    if( !strcmp( SourceInst.Mnemonic, "EQU" ) )                  //  If EQU Pseudo Op ?
    {
        if( !strcmp( SourceInst.Operand1, "*" ) )                //  Is it EQU * ( Is * is operand Field ?)
        {
            fprintf( fp2, "%-15s  ", SourceInst.Label );         // Then, Store label and
            fprintf( fp2, "%04X", LC );                          // current value of LC ( * means current value of LC) in ST
            fprintf( fp2, "\n" );                                // Go to new line
        }
        else       // exception                                  //  If not * in operand1 field
        {
            fprintf( fp2, "%-15s  ", SourceInst.Label );         // Store label and
            fprintf( fp2, "%s", SourceInst.Operand1 );           // the value mentioned in operand1 field in ST
            fprintf( fp2, "\n" );                                // Go to new line
        }
    }
    else          // If it is not EQU, may be DB, DS, DW, START, ORG pseudo ops OR may be machine Op
    {
        fprintf( fp2, "%-15s  ", SourceInst.Label );            // Simply store label and
        fprintf( fp2, "%04X", LC );                             // LC value in ST
        fprintf( fp2, "\n" );                                   // Go to new line
    }

}



char *DeleteExcessWhiteSpaces_Pass1( char str[] )  // All the extra tabs and spaces will get deleted. wherever there are more than 1 space, it will be deleted and the tabs will be replaced by spaces.
{
    int count, i;
    char str1[256], c;

    /// First Delete replace '\n' with '\0'

    if( str[strlen(str)-1] == '\n' )          // If '\n' at the last location of str
    {
        str[strlen(str)-1] = '\0' ;           // Then terminate the string there
    }

    /// First replacing all Tabs [i.e. '\t'] by blank spaces [i.e. ' ']

    count = 0;
    while( str[count] != '\0' )           // Till the end of the string
    {
        if( str[count] == '\t' )          // If '\t'
        {
            str[count] = ' ';             // Replace it by ' '
        }
        count++;
    }

    /// Now replacing multiple blanks by a single blank

    count = 0;                                  // Used for pointing (Indexing) str
    i = 0;                                      // Used for pointing (Indexing) str1

    while( str[count] != '\0' )                 // Till the end of the string
    {

        if( str[count] == ' ' )                 // If ' '
        {

            while( str[count + 1] == ' ' )      // Till further non-blank
            {
                count++;                        // Increment count
            }
        }

        /// Here Count points first non-blank character

        str1[i] = str[count];                // Copy the non blank character from str  to str1
        count++;                             // Point next location of str
        i++;                                 // Point next location of str1

    }
    str1[ i ] = '\0';                        // Terminate str1
    strcpy( str, str1 );                     // Copy the str1 back to str as an result
    return( str );                           // Return with str containing string with multiple blanks replaced by a single blank
}



struct SPROG GetFields_Pass1( char Instruction[] )  // The instruction is broken down into smaller tokens for analysis and differentiating whether it is a label or mnemonic or a operand
{
    char Label[LABEL_LENGTH + 1], *p, Temp[256], *p1, *p2, Mnemonic[MNEMONIC_LENGTH + 1];
    char Operand1[OPERAND1_LENGTH + 1], Operand2[OPERAND2_LENGTH + 1];
    struct SPROG TempInst;
    int count, IsComment;
    char Comment[256];

    /// Separating Comment

    IsComment = 0;           // Initializing the comment flag
    count = 0;               // Initializing counter
    while( Instruction[count] != '\0' )         // till \0 is reached
    {

        if( Instruction[count] == ';' || ( Instruction[count] == '/' && Instruction[count+1] == '/' ) )  // if there is "//" or ";" then
        {
            Instruction[count] = '\0';                               // Put \0 at that position and end the string there
            IsComment = 1;                                           // Setting a flag to identify if there is a comment to the instruction or not
            break;                                                   // Break the while loop
        }
        count++;                      // Incrementing counter
    }

    if ( IsComment )                               //  If comment is present
    {
        count = count + 1;                           // Point to second / or position after ;
        Comment[0] = '/';                            // Put First slash in comment
        strcpy( &Comment[1], &Instruction[count] );  // Copy From second slash
    }
    else
    {
        Comment[0] = '?';                            // If comment not present, Put ? in comments 0th location
        Comment[1] = '\0';                           // Terminate comment
    }

    Comment[COMMENT_LENGTH] = '\0';   // Restricting comment length to output file to 50 characters
    strcpy( TempInst.Comment, Comment);


    /// Separating Label

    strcat( Instruction, " ");
    strcpy(Temp, Instruction);

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

    /// Separating mnemonics

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
    /// Reads the complete MOT from file and put it into array of structures ( MOTInst[] ) for further reference

    FILE *fp;

    int i;

    if( ( fp = fopen( "MachineOpTable 8085A.txt", "r" ) ) == NULL )             // Opening text file which has MachineOp table
    {
        //error
        printf( "\n Error opening the text file named MachineOpTable 8085A...\n Exiting....\n" );
        exit(0);
    }

    for( i=1; i<N; ++i )
    {
        fscanf(fp, "%s", MOTInst[i].Mnemonic);            // Read Mnemonic from and put it into Mnemonic field of MOTInst[i]
        fscanf(fp, "%s", MOTInst[i].Operand1);
        fscanf(fp, "%s", MOTInst[i].Operand2);
        fscanf(fp, "%s", MOTInst[i].OpCode);
        fscanf(fp, "%d", &MOTInst[i].Length);
        fscanf(fp, "%d", &MOTInst[i].Type);
    }

    fclose(fp);                                           // Close file containing MOT
}


/// MOTGET Using Linear Search

int MOTGet_LS_Pass1( struct MOT MOTInst[], char Mnemonic[] )
{
    /// Search the Mnemonic in MOT for Pass1 only. ( Linear Search Method is used )
    /// If present return with Index where match is found,
    /// else return N (247) if no match is found

    int Index;

    //Searching MOT for Pass1

    for( Index = 1; Index<N; ++Index )                            // Checking if the Source Instruction's Mnemonic is present in the MOT table
    {
        if( !( strcmp( MOTInst[Index].Mnemonic, Mnemonic ) ) )
        {
            break;                                                // if present then break the loop with its index number stored in the variable i
        }
    }

    return( Index );                                               // Return with matched index value
}


/// MOTGET Using Binary Search

int MOTGet_BS_Pass1( struct MOT MOTInst[] , char Mnemonic[] )
{
    /// Search the Mnemonic in MOT for Pass1 only. ( Binary Search Method is used )
    /// If present return with Index where match is found,
    /// else return N (247) if no match is found

    unsigned int First, Last, Mid ;
    int p ;

    First = 1;                                              //  Pointing First Element
    Last = N-1 ;                                            // Pointing Last Element

    while( First <= Last )                                  // Till the end condition ( First <= Last ) is reached
    {
        Mid = (unsigned int)( (First+Last)/2 ) ;            // Find Middle index value to compare with middle element

        p = strcmp( Mnemonic, MOTInst[Mid].Mnemonic ) ;     // Compare with middle element

        if( p == 0 )                                        // If match found with middle element
        {
            return( Mid ) ;                                 // Return Middle value as index
        }
        else if( p < 0 )                                    // If Mnemonic is < Middle entry
        {
            Last = Mid-1;                                   // So, the required Mnemonic may be present in Upper half of the Table
        }
        else                                                // else Mnemonic must be > Middle entry
        {
            First = Mid+1;                                  // So, the required Mnemonic may be present in Lower half of the Table
        }
    }

    return(N) ;                                             // If no match is Found, return with N ( i.e. 247 )
}



int Are_All_Hexadecimal_Digits( char str[] )
{
    int i ;

    for( i = 0; i<strlen(str); ++i )
    {
        if( !isxdigit( str[i] ) )
        {
            return( 0 );
        }
    }

    return(1) ;
}


int Are_All_Decimal_Digits( char str[] )
{
    int i ;

    for( i = 0; i<strlen(str); ++i )
    {
        if( !isdigit( str[i] ) )
        {
            return( 0 );
        }
    }

    return(1) ;
}


int Is_Register( char str[] )
{
    char Registers[][4] = { "A", "B", "C", "D", "E", "H", "L", "SP", "PSW" } ;
    int i ;

    for( i = 0; i<9; ++i )
    {
        if( !strcmp( str, &Registers[i][0] ) )
        {
            return( 1 );
        }
    }

    return(0) ;
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

