
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
