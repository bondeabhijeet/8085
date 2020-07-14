#include<windows.h>
#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>

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
    int Value;
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
int MOTGet_Pass1( struct MOT MOTInst[], struct PROG SourceInst, unsigned int LN );
void DisplayError( unsigned int ErrorCode, unsigned int LN );
void STSTO( FILE *fp2, struct PROG SourceInst, unsigned int LC );
int IsPseudoOp_Pass1( char Mnemonic[], char POT[][6] );
int ProcessPseudoOp_Pass1( unsigned int p, struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessORGorSTART_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2 , unsigned int LN );
int ProcessEQU_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDS_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDB_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessDW_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN );
int ProcessEND_Pass1( void );
int CheckSymbolTableForDuplicates( void );
int GetComment( char InstructionWithComment[], char Comment[] );
int WritePass1OutputToFile( struct PROG SourceInst, struct MOT MOTInst[], int MOTIndex, unsigned int MOTFlag, unsigned int LC, char Comment[], FILE *fp3 );
int Pass2( void );
int ReadSymbolTable( struct SYMBOLTABLEDEF ST[] );
struct DEFP1OUTPUT GetP1OutputInst( FILE *P1OutputFP );
int ProcessMachineInstType1( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessMachineInstType2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP );
int ProcessMachineInstType3( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
int ProcessMachineInstType4( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
int STGet( char Operand1[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries );
void SetColor( int ForgC );


int main()
{
    FILE *fp, *fp2, *fp3;
    unsigned int counter, i, MOTIndex, p;
    unsigned int LC, LN, MOTFlag;
    struct MOT MOTInst[N];
    char POT[][6] = {"ZZZZ", "DB", "DS", "DW", "END", "EQU", "ORG", "START", "\0"};  // remember that the zeroth element is dummy ( creating POT )
    struct PROG SourceInst;
    char str[256], InstructionWithComment[256], Instruction[256], Comment[256];
    char *str2;  // For recieving purpose


    ReadMachineOpTable( MOTInst );                          // Reading MachineOp Table from the text file and store it into a structure

    if( ( fp=fopen("SourceProgram.txt", "r" ) ) == NULL )          // Opening a file ( read only ) to read the Source program
    {
        SetColor(12);
        printf("\nError opening Source Instruction file\n\a");     //error
        SetColor(0);
        exit(0);
    }

    if( ( fp2 = fopen( "SymbolTable.txt","w" ) ) == NULL )        // Opening a file ( write only ) to store the Symbol table
    {
        SetColor(12);
        printf("Error opening symbol Table for writing purpose \n\a");  //error
        SetColor(0);
        exit(0);
    }

    if( ( fp3 = fopen( "Pass1Output.txt","w" ) ) == NULL )        // Opening a file ( write only ) to store the result of Pass1 in file
    {
        SetColor(12);
        printf("Error opening Pass1output for writing purpose \n\a");  //error
        SetColor(0);
        exit(0);
    }


    /// Starting pass 1 : preparing symbol table

    LC = 00;                                                // Assigning location counter to 2000
    LN = 1;                                                 // Assigning the line counter to 1

    while(!feof(fp))
    {
        fscanf(fp, " %[^\n]", str);                         // Read the full sentence from the given file with comment

        str2 = DeleteExcessWhiteSpaces(str);                // Deletes all the extra spaces and tabs ( no tabs will be present after execution of this function )
        strcpy(InstructionWithComment, str2);               // Store the instruction in the InstructionWithComment variable

        str2 = SplitInstructionAndComment(str, Comment);    // Delete comment from the instruction
        strcpy(Instruction, str2);                          // Store the instruction ( without comment and extra spaces ) ie. A proper instruction

        SourceInst = GetFields(Instruction);                // Separating the fields from the Instruction ie. Label, Mnemonic, Operand1, Operand2

        /// PASS 1 STARTS HERE

        /// Processing PseudoOps and MachineOps

        if( p = IsPseudoOp_Pass1( SourceInst.Mnemonic, POT ) )  // checking if the Mnemonic is pseudoOp if yes then ProcessPseudoOp_Pass1() will be executed
        {
            ProcessPseudoOp_Pass1( p, SourceInst, &LC, fp2 , LN );
            MOTFlag = 0;
        }
        else                                                // If the Mnemonic is not a pseudoOp then it might be machine Op if yes then execute the following functions
        {
            MOTIndex = MOTGet_Pass1(MOTInst, SourceInst, LN );   // Identify the index of the Mnemonic in MOT and store it in MOTIndex


            if( ( SourceInst.Label[0] )!= '\0' )                // If label is present then call STSTO
            {
                STSTO(fp2, SourceInst, LC);                 // Store the Label in symbol table file
            }

            LC = LC + MOTInst[MOTIndex].Length;             // Increment the location counter by the length of the MachineOp ( length is fetched from the MOT table
            MOTFlag = 1;
        }

        /// Storing the result of PASS1 in a file ( Pass1Output.txt )

        WritePass1OutputToFile( SourceInst, MOTInst, MOTIndex, MOTFlag, LC, Comment, fp3 );

        LN = LN + 1;                                        // Increment the Line counter by 1

    }

    fclose( fp );                                           // Closing all the files
    fclose( fp2 );
    fclose( fp3 );

    CheckSymbolTableForDuplicates( );                        // Checking symbol table for any duplicate ( ie. repeated ) symbols

    Pass2();

}

   /// MAIN ENDS HERE

int Pass2( void )
{
    int NoOfSTEntries, i;
    FILE *P1OutputFP, *P2OutputFP;
    struct SYMBOLTABLEDEF ST[MAX_NO_OF_ST_ENTRIES];  // ST for Symbol Table
    struct DEFP1OUTPUT P1OutputInst;

    // Reading Symbol Table from file and store it in a structural array

    NoOfSTEntries = ReadSymbolTable( ST );

    /*
    for(i=0; i<NoOfSTEntries; ++i)
    {
        printf( "%-15s", ST[i].Symbol );
        printf( "%04X\n", ST[i].Value );
    }
    */

    if( ( P1OutputFP = fopen( "Pass1Output.txt", "r" ) ) == NULL )
    {
        SetColor( 12 );
        printf("\nERROR OPENING Pass1Output.txt FILE\n\n\a ");
        SetColor( 0 );
        exit( 0 );
    }

    if( ( P2OutputFP = fopen( "Pass2Output.txt", "w" ) ) == NULL)
    {
        SetColor( 12 );
        printf("\nERROR OPENING Pass2Output.txt FILE\n\n\a ");
        SetColor( 0 );
        exit( 0 );
    }

    while( !feof( P1OutputFP ) )
    {
        P1OutputInst = GetP1OutputInst( P1OutputFP );

        if( P1OutputInst.IType == 1 )
        {
            ProcessMachineInstType1( P1OutputInst, P2OutputFP );
        }
        else if( P1OutputInst.IType == 2 )
        {
            ProcessMachineInstType2( P1OutputInst, P2OutputFP );
        }
        else if( P1OutputInst.IType == 3 )
        {
            ProcessMachineInstType3( P1OutputInst, P2OutputFP, ST, NoOfSTEntries );
        }
        else if( P1OutputInst.IType == 4 )
        {
            ProcessMachineInstType4( P1OutputInst, P2OutputFP, ST, NoOfSTEntries );
        }
    }

    fclose( P1OutputFP );
    fclose( P2OutputFP );
}

int ProcessMachineInstType4( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries )
{
    int SValue;
    char temp[5];

    SValue = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries );

    /// TO B MODIFIED

    fprintf( P2OutputFP, "%04X ", P1OutputInst.LC );         // Printing Location Counter or Address
    fprintf( P2OutputFP, "%-3s ", P1OutputInst.OpCode );     // Printing OpCode

    if( SValue == ( -1 ) )
    {
        strcpy( temp, P1OutputInst.Operand1 );               // To remove H we copy the operand in temp and then cut out the H
        temp[2] = '\0';
        //printf("%s", temp);
        fprintf( P2OutputFP, "%-8s", temp );                 // For absence Operand 1
    }
    else
    {
        printf("%X\n", SValue);
        fprintf( P2OutputFP, "%02X%6s", SValue, " " );       // For absence Operand 1
    }
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

int ProcessMachineInstType3( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP, struct SYMBOLTABLEDEF ST[], int NoOfSTEntries )
{
    int SValue;
    char temp[5];

    SValue = STGet( P1OutputInst.Operand1, ST, NoOfSTEntries );

    /// TO B MODIFIED

    fprintf( P2OutputFP, "%04X ", P1OutputInst.LC );         // Printing Location Counter or Address
    fprintf( P2OutputFP, "%-3s ", P1OutputInst.OpCode );     // Printing OpCode

    if( SValue == ( -1 ) )
    {
        strcpy( temp, P1OutputInst.Operand1 );               // To remove H we copy the operand in temp and then cut out the H
        temp[2] = '\0';
        fprintf( P2OutputFP, "%-8s", temp );                 // For absence Operand 1
    }
    else
    {
        fprintf( P2OutputFP, "%02X%6s", SValue, " " );       // For absence Operand 1
    }
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

int STGet( char Operand1[], struct SYMBOLTABLEDEF ST[], int NoOfSTEntries )
{
    int i;

    for (i=0; i<NoOfSTEntries; ++i)
    {
        if( !strcasecmp( Operand1, ST[i].Symbol ) )
        {
            return( ST[i].Value );
        }
    }

    return( -1 );
}


int ProcessMachineInstType2( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%04X ", P1OutputInst.LC );         // Printing Location Counter or Address
    fprintf( P2OutputFP, "%-3s ", P1OutputInst.OpCode );     // Printing OpCode

    fprintf( P2OutputFP, "%-8s", " " );                      // For absence Operand 1
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
    fprintf( P2OutputFP, "%-15s", P1OutputInst.Operand1 );   // Printing Operand 1
    fprintf( P2OutputFP, "%-15s", " " );                     // Printing Operand 2

    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );     // Printing Comment
}


int ProcessMachineInstType1( struct DEFP1OUTPUT P1OutputInst, FILE *P2OutputFP )
{
    fprintf( P2OutputFP, "%04X ", P1OutputInst.LC );         // Printing Location Counter or Address
    fprintf( P2OutputFP, "%-3s ", P1OutputInst.OpCode );     // Printing OpCode
    fprintf( P2OutputFP, "%-8s", " " );                      // For absence Operand 1
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
    fprintf( P2OutputFP, "%-15s", " " );                     // Printing Operand 1
    fprintf( P2OutputFP, "%-15s", " " );                     // Printing Operand 2

    fprintf( P2OutputFP, "%s\n", P1OutputInst.Comment );     // Printing Comment
}

struct DEFP1OUTPUT GetP1OutputInst( FILE *P1OutputFP )
{
    struct DEFP1OUTPUT P1OutputInst;

    fscanf( P1OutputFP, "%X", &P1OutputInst.LC );
    fscanf( P1OutputFP, "%s", P1OutputInst.Label );
    fscanf( P1OutputFP, "%s", P1OutputInst.Mnemonic);
    fscanf( P1OutputFP, "%s", P1OutputInst.Operand1 );
    fscanf( P1OutputFP, "%s", P1OutputInst.Operand2 );
    fscanf( P1OutputFP, "%s", P1OutputInst.OpCode );
    fscanf( P1OutputFP, "%d", &P1OutputInst.IType );
    fscanf( P1OutputFP, " %[^\n]\n", P1OutputInst.Comment );

    return( P1OutputInst );
}


int ReadSymbolTable( struct SYMBOLTABLEDEF ST[] )
{
    FILE *STFP;  // Symbol Table File Pointer
    int NoOfSTEntries;

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
        fscanf( STFP, "%X\n", &ST[NoOfSTEntries].Value );
        printf("%X\n", ST[NoOfSTEntries].Value );

        NoOfSTEntries = NoOfSTEntries + 1;
    }

    fclose( STFP );

    return( NoOfSTEntries );
}


int WritePass1OutputToFile( struct PROG SourceInst, struct MOT MOTInst[], int MOTIndex, unsigned int MOTFlag, unsigned int LC, char Comment[], FILE *fp3 )
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


        if( MOTFlag == 0)                                   // If PseudoOp
        {
            fprintf( fp3, "%04X    ", LC );                 // Write the location counter in Hex ( its present in intger form but while printing we print it in Hex )
        }
        else                                                // // If not PseudoOp then it must be MachineOp
        {
            fprintf( fp3, "%04X    ", ( LC - MOTInst[MOTIndex].Length ) );  // write the location counter by decrementing the value that we incremented while exiting the MOT loop in the previous part of this function
        }
        fprintf( fp3, "%-15s", SourceInst.Label );
        fprintf( fp3, "%-12s", SourceInst.Mnemonic );
        fprintf( fp3, "%-15s", SourceInst.Operand1 );
        fprintf( fp3, "%-15s", SourceInst.Operand2 );

        if( MOTFlag == 0)                                   // If PseudoOp
        {
             fprintf( fp3, "%-5s", "?" );                   // there is no OpCode so write "?"
             fprintf( fp3, "%-8d", -99 );                   // When PseudoOp found, no Opcode would be there so entering a -ve value to ease off the further code ( ie. reading process )
        }
        else                                                // If not PseudoOp then it must be MachineOp
        {
            fprintf( fp3, "%-5s", MOTInst[MOTIndex].OpCode );  // Write OpCode from MOTInst
            fprintf( fp3, "%-8d", MOTInst[MOTIndex].Type );  // Write type of instruction from the MOTInst
        }

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

int ProcessPseudoOp_Pass1( unsigned int p, struct PROG SourceInst, unsigned int *LC, FILE *fp2 , unsigned int LN)
{
    /// Processing the PseudoOp's

    //printf("PseudoOp Found=%d", p);
    if( p == 1 )
    {
        ProcessDB_Pass1( SourceInst, LC, fp2, LN );         // Process DB pseudoOp
    }
    else if( p == 2)
    {
        ProcessDS_Pass1( SourceInst, LC, fp2, LN );         // Process DS pseudoOp
    }
    else if( p == 3)
    {
        ProcessDW_Pass1( SourceInst, LC, fp2, LN );         // Process DW pseudoOp
    }
    else if( p == 4)
    {
        ProcessEND_Pass1();                                 // Process END pseudoOp
    }
    else if( p == 5)
    {
        //printf("\n\nEQU3:%04X\n\n", *LC );
        ProcessEQU_Pass1( SourceInst, LC, fp2, LN );        // Process EQU pseudoOp
    }
    else if( p == 6 || p == 7)
    {
        ProcessORGorSTART_Pass1( SourceInst, LC, fp2, LN);  // Process ORG or START pseudoOp
    }
    else                                                    // In case any unknown error occurs
    {
        printf("\nPseudoOp Error\n");                          //error
    }
}

int ProcessDW_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )
{
    if( SourceInst.Label[0] != '\0' )
    {
        STSTO( fp2, SourceInst, *LC ) ;
    }

    *LC = *LC + 2;
}


int ProcessDB_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  // Processing the PseudoOp DB
{
    if( SourceInst.Label[0] != '\0' )    // if the Label is not empty
    {
        STSTO( fp2, SourceInst, *LC ) ;   // store it in symbol table
    }

    *LC = *LC + 1;                        // increment the value of LC by 1 as single byte is reserved for storing the constant
}

int ProcessEND_Pass1( void )                                                                  // Processing the PseudoOp END
{
    return(1);
}

int ProcessDS_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )   // Processing the PseudoOp DS
{
    unsigned int OP1;

    OP1 = (unsigned int) atoi(SourceInst.Operand1);
    STSTO( fp2, SourceInst, *LC );
    *(LC) = *(LC) + OP1;
}

int ProcessEQU_Pass1( struct PROG SourceInst, unsigned int *LC, FILE *fp2, unsigned int LN )  //Processing the PseudoOp EQU
{
    unsigned int temp;
    if( !strcmp( SourceInst.Operand1, "*" ) )  // If * then
    {
        STSTO( fp2, SourceInst, *LC );         // store the value symbol = current value of LC ( which is integer )
    }
    else                                      // else
    {
        temp = ( unsigned int )atoi( SourceInst.Operand1 ); // Convert the value of Operand 1 to integer for storing in symbol table
        STSTO( fp2, SourceInst, temp );      // store the value symbol = as it is in operand 1
        // NOTE: we always store Hex values in symbol Table
    }
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

    return(1);
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
    return(0);                                              // Else return 0
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

void DisplayError( unsigned int ErrorCode, unsigned int LN )  // Display the errors that occurred during the process of the program
{
    char ErrorMessages[][80] = { "SYNTAX ERROR IN MNEMONIC....",
                                 "SYNTAX ERROR IN OPERAND1....",
                                 "SYNTAX ERROR IN OPERAND2...." };

    SetColor( 12 );
    printf( "\nERROR IN LINE NO: %d : %s\n\n\a", LN, ErrorMessages[ErrorCode - 1000] );
    SetColor( 0 );
    exit( 0 );

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

    if(p1 = strchr( Temp, ':' ) )                              // When Label is present
    {
        if( *( p1 + 1 ) == ' ' )
        {
            p1 = (p1+2);
        }
        else
        {
            p1 = (p1 + 1);
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


    /*printf("\nFINAL:\n");
    printf("\n#Label:%s#\n", TempInst.Label);
    printf("\n#Mnemonic:%s#\n", TempInst.Mnemonic);
    printf("\n#Operand1:%s#\n", TempInst.Operand1);
    printf("\n#Operand2:%s#\n", TempInst.Operand2);*/

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

int MOTGet_Pass1( struct MOT MOTInst[], struct PROG SourceInst, unsigned int LN )  // Fetch the MOT if present and return its index in MOT table
{
    int i, index;
    char MOTStr[100], SourceStr[100];

    //Searching MOT for the first time
    for( i=1; i<N; ++i )                                          // Checking if the Source Instruction's Mnemonic is present in the MOT table
    {                                                             // if present then break the loop with its index number stored in the variable i
        if( !( strcmp( MOTInst[i].Mnemonic, SourceInst.Mnemonic ) ) )
        {
            //printf("%sh", SourceInst.Mnemonic);
            break;
        }
    }

    if( i == N )                                                  // If syntax error in Mnemonic
    {
        DisplayError( 1000, LN );                                 // Set error code to 1000
    }

    /*
     *Searching MOT for the 2nd time ( depending on the type of instruction
     *Now checking the type of the instruction from the MOT table
     *If the instruction is of type 1, 3, 4, 5 ie. only Mnemonic..... No further search required just return the index*/

    if( MOTInst[i].Type == 1 || MOTInst[i].Type == 3 || MOTInst[i].Type == 4 || MOTInst[i].Type == 5 )
    {
        //printf("TYPE:%d", MOTInst[i].Type);

        return( i );
    }

    // If the Instruction if of type 2, 6, 8, 9 ie. Mnemonic with operand1.
    if( MOTInst[i].Type == 2 || MOTInst[i].Type == 6 || MOTInst[i].Type == 8 || MOTInst[i].Type == 9 )
    {
        // Making SourceStr by appending Operand1 to Mnemonic
        strcpy( SourceStr, SourceInst.Mnemonic );
        strcat( SourceStr, SourceInst.Operand1 );
        //printf("\n%s\n", SourceStr);

        //Searching MOT
        for( i=1; i<N; ++i )
        {
            // Making MOTStr by appending Operand1 to Mnemonic
            strcpy( MOTStr, MOTInst[i].Mnemonic );
            strcat( MOTStr, MOTInst[i].Operand1 );

            if( !strcmp( SourceStr, MOTStr ) )
            {
                //printf("\nMATCH FOUND....EXACT INDEX:%d\n", i);
                break;
            }
        }
        if( i == N )                                                // If syntax error in Operand 1
        {
            DisplayError( 1001, LN );                               // Set error code to 1001
        }

    }

    // If the Instruction is of type 7 ie.Mnemonic With Operand1 and Operand2
    if( MOTInst[i].Type == 7 )
    {
        //Making SourceStr by appending Operand1 and Operand2 to Mnemonic
        strcpy( SourceStr, SourceInst.Mnemonic );
        strcat( SourceStr, SourceInst.Operand1 );
        strcat( SourceStr, SourceInst.Operand2 );
        //printf("\n%s\n", SourceStr);

        //Searching MOT
        for( i=1; i<N; ++i )
        {
            //Making MOTStr by appending Operand1 and Operand2 to Mnemonic
            strcpy( MOTStr, MOTInst[i].Mnemonic );
            strcat( MOTStr, MOTInst[i].Operand1 );
            strcat( MOTStr, MOTInst[i].Operand2 );


            if( !strcmp( SourceStr, MOTStr ) )
            {
                //printf("MATCH FOUND 2.... EXACT INDEX:%d\n", i);
                break;
            }
        }
        if( i == N )                                                           // If syntax error in Operand 2
        {
            DisplayError( 1002, LN );                                          // Set error code to 1002
        }
    }
    return( i );
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



