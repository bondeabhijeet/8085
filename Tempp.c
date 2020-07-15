int ProcessMachineOpPass2( struct DEFP1OUTPUT P1OutputInst, struct MOT MOTInst[], FILE *P2OutputFP, unsigned int LN )
{
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

    if( P1OutputInst.IType == 1)
    {

        ProcessMOType1( P1OutputInst, MOTInst[], P2OutputFP, LN );
    }
    else if( P1OutputInst.IType == 2)
    {

    }
    else if( P1OutputInst.IType == 3)
    {

    }
    else if( P1OutputInst.IType == 4)
    {

    }
    else if( P1OutputInst.IType == 5)
    {

    }
    else if( P1OutputInst.IType == 6)
    {

    }
    else if( P1OutputInst.IType == 7)
    {

    }
    else if( P1OutputInst.IType == 8)
    {

    }
    else if( P1OutputInst.IType == 9)
    {

    }
}
