\ This is the file kernel.4, included by the cross compiler.
\ Copyright 1994 L.C. Benschop Vught, The Netherlands.
\ The program is released under the GNU General Public License version 2.
\ There is NO WARRANTY.

\ It is excessively commented as it must serve as an introduction to the
\ construction of Forth compilers. 

\ Lines starting with \G are comments that are included in the glossary.

\ All of the file after the next line is cross compiled.

\ Change 2025-01-11: FIxed DEPTH.
CROSS-COMPILE

\ PART 0: BOOT VECTORS. 
\ The computer starts execution at address 0. 

COLD \ call to cold start location.
WARM \ call to warm start location.
DIV-EX \ address of division exception routine.
BREAK-EX \ address of break handling routine.
TIMER-EX \ address of timer interrupt routine.  
\ PART 1: SINGLE OPCODE WORDS

\ Their execution parts contain the single opcode plus the return bit. The
\ compiler compiles the single opcode in-line.

0 
OPCODE NOOP  ( --- )      
\G Do nothing

1 
OPCODE SWAP  ( x1 x2 --- x2 x1 )   
\G Swap the two top items on the stack.

2 
OPCODE ROT   ( x1 x2 x3 --- x2 x3 x1 )
\G Rotate the three top items on the stack.  

3 
OPCODE 0=    ( x --- f)
\G f is true if and only if x is 0.

4 
OPCODE NEGATE  ( n1 --- -n1)
\G Negate top number on the stack.    

5 
OPCODE UM*     ( u1 u2 --- ud )
\G Multiply two unsigned numbers, giving double result. 

6 
OPCODE C@      ( c-addr --- c)
\G Fetch character c at c-addr.

7 
OPCODE @       ( a-addr --- x)
\G Fetch cell x at a-addr.
 
8 
OPCODE +       ( w1 w2 --- w3)    
\G Add the top two numbers on the stack.

9 
OPCODE AND     ( x1 x2 --- x3)
\G Bitwise and of the top two cells on the stack. 

10 
OPCODE OR      ( x1 x2 --- x3)
\G Bitwise or of the top two cells on the stack.

11 
OPCODE XOR     ( x1 x2 --- x3)
\G Bitwise exclusive or of the top two cells on the stack.

12 
OPCODE U<      ( u1 u2 ---- f)
\G f is true if and only if unsigned number u1 is less than u2.   

13 
OPCODE <      ( n1 n2 --- f)
\G f is true if and only if signed number n1 is less than n2. 

14 
OPCODE LSHIFT ( x1 u --- x2)
\G Shift x1 left by u bits, zeros are added to the right.

15 
OPCODE RSHIFT ( x1 u --- x2)
\G Shift x1 right by u bits, zeros are added to the left.

16 
OPCODE UM/MOD ( ud u1 --- urem uquot)
\G Divide the unsigned double number ud by u1, giving unsigned quotient
\G and remainder.    

17 
OPCODE +CY    ( n1 n2 cy1 --- n3 cy2)
\G Add n1 and n2 and the carry flag cy1 (must be 0 or 1) giving sum n3
\G and carry flag cy2. (n3 cy2 can be seen as a double number) 

18 
OPCODE SCAN1  ( x d --- u)
\G Scan x for the first 1 bit. u is the position of that bit (counted 
\G from the scan direction) and 32 if x=0. d is the scan direction,
\G 0 is left-to-right, 1 is right-to-left. 

19 
OPCODE SPECIAL ( x ---)
\G Any of a large number of special instructions, indicated by x.

20 
OPCODE DROP   ( x ---)
\G Discard the top item on the stack.    

21 
OPCODE >R     ( x ---)
\G Push x on the return stack. 

22 
OPCODE C!A    ( c c-addr --- c-addr)
\G Store character c at c-addr, keep address.

23 
OPCODE !A     ( x a-addr --- a-addr)
\G Store cell x at a-addr, keep address.

24 
OPCODE DUP    ( x --- x x )
\G Duplicate the top cell on the stack.    

25 
OPCODE OVER   ( x1 x2 --- x1 x2 x1)
\G Copy the second cell of the stack. 

26 
OPCODE R@     ( --- x)
\G x is a copy of the top of the return stack.

27 
OPCODE R>     ( --- x)
\G Pop the top of the return stack and place it on the stack.

28 
OPCODE 0      ( --- 0)
\G The constant number 0.    

29 
OPCODE 1      ( --- 1)
\G The constant number 1. 

30 
OPCODE 4      ( --- 4)
\G The constant number 4.

31 
OPCODE LIT    ( --- lit)
\G Push literal on the stack (literal number is in-line).

\ PART 2: RUNTIME PARTS THE VARIOUS DEFINITION CLASSES.

\ Only VARIABLES (or CREATE) need a runtime part in this system.
\ As this is a native code compiler, colon definitions have no runtime
\ part and for CONSTANT it is compiled inline. For variables, a call
\ to DOVAR is compiled. DOVAR pushes the return address (the address
\ where the data of the variable is stored) on the stack.

: DOVAR  ( --- a-addr)
\G Runtime part of variables.
  R> ; 

\ PART 3: SIMPLE DEFINITIONS

\ This is a large class of words, which would be written in machine code
\ on most non-native code systems. Many contain just a few words, so they
\ are implemented as macros. 

\ This category contains simple arithmetic and compare words, the runtime
\ parts of DO LOOP  and string related words etc, many on which are
\ dependent on each other, so they are in a less than logical order to
\ avoid large numbers of forward references.

M: -    ( w1 w2 --- w3 )
\G Subtract the top two numbers on the stack (w2 from w1).
  NEGATE + ;

M: =    ( x1 x2 --- f)
\G f is true if and only if x1 is equal to x2.
  - 0= ;

M: <>   ( x1 x2 --- f)
\G f is true if and only if x1 is not equal to x2.
  = 0= ;

M: 0<   ( n --- f)
\G f is true if and only if n is less than 0.
  0 < ;

M: >    ( n1 n2 --- f)
\G f is true if and only if the signed number n1 is less than n2. 
  SWAP < ;

M: 0>  ( n --- f)
\G f is true if and only if n is greater than 0.
  0 > ; 

M: U>  ( u1 u2 --- f)
\G f is true if and only if the unsigned number u1 is greater than u2.
  SWAP U< ;

M: EMIT ( c ---)
\G Output the character c to the terminal.
  1 1 + 32 SPECIAL ;

M: KEY ( --- c) 
\G Input the character c from the terminal.
  1 32 SPECIAL ;

M: BYE  ( ---)  
\G Terminate the execution of SOD-32 Forth, return to OS.
  0 32 SPECIAL ;

: CR  ( --- )
\G Output a newline to the terminal.
   13 EMIT 10 EMIT ;



VARIABLE S0 ( --- a-addr)
\G Variable that holds the bottom address of the stack.

VARIABLE R0 ( --- a-addr)
\G Variable that holds the bottom address of the return stack.

M: SP@      ( --- a-addr)
\G Return the address of the stack pointer (before SP@ was executed).
   0 SPECIAL ;

M: SP!      ( a-addr ---)
\G Set the stack pointer to a-addr.
   1 SPECIAL ;

M: RP@      ( --- a-addr)
\G Return the address of the return stack pointer.
  02 SPECIAL ;

M: RP!      ( a-addr ---)
\G Set the return stack pointer to a-addr.
  03 SPECIAL ;

M: 2* ( w1 --- w2) 
\G Multiply w1 by 2.
  1 LSHIFT ;

: 2/  ( n1 --- n2)
\G Divide signed number n1 by 2.
  DUP $80000000 AND SWAP 1 RSHIFT OR ;

: DEPTH ( --- n )
\G n is the number of cells on the stack (before DEPTH was executed). 
  SP@ S0 @ SWAP - 2 RSHIFT ; 


\ The DO LOOP related words use the return stack. The top of the
\ return stack is the loop counter (I) and the next cell is the limit.

\ (LOOP) and (+LOOP) are followed by an inline loop start address.
\ (?DO) and (LEAVE) are followed by an inline leave address.
\ The inline parameters are accessed through the return stack.
\ They can 'jump' by returning to a different address.
\ These words are called 'subroutines', not macros.

\ To access the loop parameters on the return stack, the DO LOOP words must 
\ first pop their OWN return address!

: (DO)   ( n1 n2 ---)
\G Runtime part of DO.
  R> ROT ROT SWAP >R >R >R ;
: (?DO)  ( n1 n2 ---)
\G Runtime part of ?DO
   OVER OVER - IF R> ROT ROT SWAP >R >R 4 + >R 
                  ELSE DROP DROP R> @ >R \ Jump to leave address if equal
                  THEN ;
M: I ( --- n )
\G Return the counter (index) of the innermost DO LOOP 
     R@ ;

: J  ( --- n)
\G Return the counter (index) of the next loop outer to the innermost DO LOOP
  RP@ 12 + @ ;

: (LEAVE)   ( --- )
\G Runtime part of LEAVE
  R> @ R> DROP R> DROP >R ; \ Remove loop parameters and replace top of ret
                            \ stack by leave address.

M: UNLOOP ( --- )
\G Remove one set of loop parameters from the return stack. 
   R> DROP R> DROP ;


: (LOOP) ( ---)
\G Runtime part of LOOP
  R> R> 1 + DUP R@ =   \ Add 1 to count and compare to limit.
  IF 
   R> DROP DROP 4 + >R \ Discard parameters and skip leave address.  
  ELSE 
   >R @ >R  \ Repush counter and jump to loop start address.  
  THEN ;

: (+LOOP) ( n ---)
\G Runtime part of +LOOP 
\ Very similar to (LOOP), but the compare condition is different.
\  exit if ( oldcount - lim < 0) xor ( newcount - lim < 0).
     R> SWAP R> DUP R@ - ROT ROT + DUP R@ - ROT XOR 0 < 
     IF R> DROP DROP 4 + >R 
     ELSE >R @ >R THEN ;

M: -1 ( --- -1)
\G The constant number -1.
  1 NEGATE ;

M: COUNT  ( c-addr1 --- c-addr2 c)
\G c-addr2 is the next address after c-addr1 and c is the character
\G stored at c-addr1.
\G This word is intended to be used with 'counted strings' where the
\G first character indicates the length of the string.
   DUP 1 + SWAP C@ ;

: TYPE ( c-addr1 u --- )
\G Output the string starting at c-addr and length u to the terminal. 
  DUP IF 0 DO DUP I + C@ EMIT LOOP DROP ELSE DROP DROP THEN ; 

M: ALIGNED ( c-addr --- a-addr )
\G a-addr is the first aligned address after c-addr.
  4 1 NEGATE + + 4 NEGATE AND ;

: (.") ( --- )
\G Runtime part of ."
\ This expects an in-line counted string. 
  R> COUNT OVER OVER TYPE + ALIGNED >R ;
: (S")  ( --- c-addr u )
\G Runtime part of S"
\ It returns address and length of an in-line counted string.
  R> COUNT OVER OVER + ALIGNED >R ;


00 
CONSTANT FALSE ( --- 0)
\G Constant 0, indicates FALSE

-01 
CONSTANT TRUE ( --- -1)
\G Constant -1, indicates TRUE

32 
CONSTANT BL ( --- 32 )
\G Constant 32, the blank character

M: PICK  ( u --- x)
\G place a copy of stack cell number u on the stack. 0 PICK is DUP, 1 PICK
\G is OVER etc.
  1 + 1 1 + LSHIFT SP@ + @ ;

M: C! ( c c-addr --- )  
\G Store character c at c-addr
   C!A DROP ; 

M: ! ( x a-addr --- )
\G Store cell x at a-addr
   !A DROP ;

M: OFF ( a-addr ---)
\G Store FALSE at a-addr.
   0 SWAP ! ;

M: ON ( a-addr ---)
\G Store TRUE at a-addr.
   -1 SWAP ! ;

M: 1+ ( w1 --- w2 )
\G Add 1 to the top of the stack.
   1 + ;

M: 1- ( w1 --- w2)
\G Subtract 1 from the top of the stack.
   1 - ;

M: INVERT ( x1 --- x2)
\G Invert all the bits of x1 (one's complement)
   -1 XOR ;


\ The next few words manipulate addresses in a system-independent way.
\ Use CHAR+ instead of 1+ and it will be portable to systems where you
\ have to add something different from 1.
 
M: CHAR+ ( c-addr1 --- c-addr2)
\G c-addr2 is the next character address after c-addr1.
   1 + ;

M: CHARS ( n1 --- n2) 
\G n2 is the number of address units occupied by n1 characters.
; \ A no-op.

M: CHAR-  ( c-addr1 --- c-addr2)
\G c-addr2 is the previous character address before c-addr1.
   1 - ;

M: CELL+ ( a-addr1 --- a-addr2)
\G a-addr2 is the address of the next cell after a-addr2.
    4 + ;

M: CELLS ( n2 --- n1)
\G n2 is the number of address units occupied by n1 cells.
   1 1 + LSHIFT ;

M: CELL- ( a-addr1 --- a-addr2)
\G a-addr2 is the address of the previous cell before a-addr1.
   4 - ;

M: +! ( w a-addr ---)
\G Add w to the contents of the cell at a-addr.
   DUP @ ROT + SWAP !A DROP ;

\ Double numbers occupy two cells in memory and on the stack.
\ The most significant half on the number is in the first memory
\ cell or in the top cell on the stack (which is also the first address).

M: 2@ ( a-addr --- d )
\G Fetch double number d at a-addr. 
   DUP 4 + @ SWAP @ ; 

M: D+ ( d1 d2 --- d3)
\G Add the double numbers d1 and d2.
   >R ROT 0 +CY ROT + R> + ;

M: 2! ( d a-addr --- )
\G Store the double number d at a-addr.
   !A 4 + !A DROP ;

M: 2DUP  ( d --- d d)
\G Duplicate the top double number on the stack.
   OVER OVER ;

M: 2DROP ( d --- )
\G Discard the top double number on the stack.
   DROP DROP ;

: ?DUP ( n --- 0 | n n)
\G Duplicate the top cell on the stack, but only if it is nonzero.
   DUP IF DUP THEN ;

: MIN ( n1 n2 --- n3)
\G n3 is the minimum of n1 and n2. 
   OVER OVER > IF SWAP THEN DROP ;

: MAX ( n1 n2 --- n3)
\G n3 is the maximum of n1 and n2.
   OVER OVER < IF SWAP THEN DROP ;

M: DNEGATE ( d1 --- d2)
\G Negate the top double number on the stack.
   >R NEGATE R> NEGATE OVER 0= 0=  + ;  

: ABS ( n --- u)
\G u is the absolute value of n.
   DUP 0< IF NEGATE THEN ;

: DABS ( d --- ud)
\G ud is the absolute value of d.
   DUP 0< IF DNEGATE THEN ;

: SM/REM ( d n1 --- nrem nquot )
\G Divide signed double number d by single number n1, giving quotient and
\G remainder. Round towards zero, remainder has same sign as dividend.
  2DUP XOR >R OVER >R \ Push signs of quot and rem.
  ABS >R DABS R>
  UM/MOD
  SWAP R> 0< IF NEGATE THEN SWAP
  R> 0< IF NEGATE THEN ;

: FM/MOD ( d n1 --- nrem nquot )
\G Divide signed double number d by single number n1, giving quotient and
\G remainder. Round always down (floored division), 
\G remainder has same sign as divisor.
  DUP >R OVER OVER XOR >R
  SM/REM
  OVER R> 0< AND IF SWAP R@ + SWAP 1 - THEN R> DROP ;

: M* ( n1 n2 --- d ) 
\G Multiply the signed numbers n1 and n2, giving the signed double number d.
  2DUP XOR >R ABS SWAP ABS UM* R> 0< IF DNEGATE THEN ; 

M: * ( w1 w2 --- w3)
\G Multiply single numbers, signed or unsigned give the same result.
  UM* DROP ;

: */MOD ( n1 n2 n3 --- nrem nquot)
\G Multiply signed numbers n1 by n2 and divide by n3, giving quotient and
\G remainder. Intermediate result is double.
  >R M* R> FM/MOD ;

: */    ( n1 n2 n3 --- n4 )
\G Multiply signed numbers n1 by n2 and divide by n3, giving quotient n4.
\G Intermediate result is double.
  */MOD SWAP DROP ;

M: S>D  ( n --- d)
\G Convert single number to double number. 
   DUP 0< ;

: /MOD  ( n1 n2 --- nrem nquot)
\G Divide signed number n1 by n2, giving quotient and remainder.
   SWAP S>D ROT FM/MOD ;

: / ( n1 n2 --- n3) 
\G n3 is n1 divided by n2.
 /MOD SWAP DROP ;

: MOD ( n1 n2 --- n3)
\G n3 is the remainder of n1 and n2.
  /MOD DROP ;

: EXIT ( ---)
\G Exit the definition that calls EXIT.
   R> DROP ;

: EXECUTE ( xt ---) 
\G Execute the word with execution token xt. 
\ Return from EXECUTE goes to xt pushed on the ret stack by >R, return from
\ the word x1 returns to definition that calls EXECUTE
  >R ;

: ?THROW ( f n --- )
\G Perform n THROW if f is nonzero.
  SWAP IF THROW ELSE DROP THEN ;  

\ PART 4: NUMERIC OUTPUT WORDS.

VARIABLE BASE ( --- a-addr)
\G Variable that contains the numerical conversion base.

VARIABLE DP   ( --- a-addr)
\G Variable that contains the dictionary pointer. New space is allocated
\G from the address in DP

VARIABLE HLD ( --- a-addr)
\G Variable that holds the address of the numerical output conversion
\G character.

VARIABLE DPL ( --- a-addr)
\G Variable that holds the decimal point location for numerical conversion.

: DECIMAL ( --- )
\G Set numerical conversion to decimal.
  10 BASE ! ;

: HEX     ( --- )
\G Set numerical conversion to hexadecimal.
  16 BASE ! ;

: SPACE  ( ---)
\G Output a space to the terminal.
  32 EMIT ;

: SPACES ( u --- )
\G Output u spaces to the terminal.
  ?DUP IF 0 DO SPACE LOOP THEN ;

: HERE ( --- c-addr )
\G The address of the dictionary pointer. New space is allocated here.
  DP @ ;

: PAD ( --- c-addr )
\G The address of a scratch pad area. Right below this address there is
\G the numerical conversion buffer. 
  DP @ 84 + ;

: MU/MOD ( ud u --- urem udquot )
\G Divide unsigned double number ud by u and return a double quotient and
\G a single remainder.
  >R 0 R@ UM/MOD R> SWAP >R UM/MOD R> ;

\ The numerical conversion buffer starts right below PAD and grows down.
\ Characters are added to it from right to left, as as the div/mod algorithm
\ to convert numbers to an arbitrary base produces the digits from right to
\ left.

: HOLD ( c ---)
\G Insert character c into the numerical conversion buffer.
  1 NEGATE HLD +! HLD @ C! ;

: # ( ud1 --- ud2)
\G Extract the rightmost digit of ud1 and put it into the numerical
\G conversion buffer.
  BASE @ MU/MOD ROT DUP 9 > IF 7 + THEN 48 + HOLD ; 

: #S ( ud --- 0 0 )
\G Convert ud by repeated use of # until ud is zero.
  BEGIN # OVER OVER OR 0= UNTIL ;

: SIGN ( n ---)
\G Insert a - sign in the numerical conversion buffer if n is negative.
  0< IF 45 HOLD THEN ;

: <# ( --- )
\G Reset the numerical conversion buffer.
  PAD HLD ! ;

: #> ( ud --- addr u )
\G Discard ud and give the address and length of the numerical conversion 
\G buffer. 
  DROP DROP HLD @ PAD OVER - ;

: D. ( d --- )
\G Type the double number d to the terminal.
  SWAP OVER DABS <# #S ROT SIGN #> TYPE SPACE ;

: U. ( u ---)
\G Type the unsigned number u to the terminal.
  0 D. ;

: . ( n ---)
\G Type the signed number n to the terminal.
  S>D D. ;

\ PART 5: MEMORY BLOCK MOVE AND RELATED WORDS. 

: CMOVE ( c-addr1 c-addr2 u --- )
\G Copy u bytes starting at c-addr1 to c-addr2, proceeding in ascending
\G order.
   DUP IF >R 
   BEGIN
    OVER C@ SWAP C!A 1 + SWAP 1 + SWAP 
    R> 1 - DUP >R 0=
   UNTIL
   R>
   THEN
   DROP DROP DROP
;

: CMOVE> ( c-addr1 c-addr2 u --- )
\G Copy a block of u bytes starting at c-addr1 to c-addr2, proceeding in
\G descending order.
   DUP IF >R R@ + 1 - SWAP R@ + 1 - SWAP 
   BEGIN
    OVER C@ SWAP C!A 1 - SWAP 1 - SWAP
    R> 1 - DUP >R 0=     
   UNTIL
   R>
   THEN
   DROP DROP DROP 
;

\ It's here because it needs CMOVE>
: ROLL ( u ---)
\G  Move stack cell number u to the top. 1 ROLL is SWAP, 2 ROLL is ROT etc.
  1 + 1 1 + LSHIFT DUP SP@ + 4 + @ SWAP 
  SP@ 4 + DUP 4 + ROT CMOVE> DROP ;

: MOVE ( c-addr1 c-addr2 u --- )
\G Copy a block of u bytes starting at c-addr1 to c-addr2. Order is such
\G that partially overlapping blocks are copied intact.
  >R OVER OVER U< IF R> CMOVE> ELSE R> CMOVE THEN ; 

: FILL ( c-addr u c ---)
\G Fill a block of u bytes starting at c-addr with character c.
  OVER IF >R 
  BEGIN
   R@ ROT C!A 1 + SWAP   
   1 - DUP 0= 
  UNTIL
  R>
  THEN
  DROP DROP DROP
;     

: 2OVER ( d1 d2 --- d1 d2 d1)
\G Take a copy of the second double number of the stack and push it on the 
\G stack.
  03 PICK 03 PICK ;

: 2SWAP ( d1 d2 --- d2 d1)
\G Swap the top two double numbers on the stack.
  03 ROLL 03 ROLL ;

\ PART 6: FILE ACCESS WORDS.

00 
CONSTANT R/O ( --- mode)
\G Read only file access mode.

02 
CONSTANT W/O ( --- mode)
\G Write only file access mode.

04 
CONSTANT R/W ( --- mode)
\G Read write file access mode.

: BIN ( mode1 --- mode2)
\G Modify the R/O W/O or R/W mode so that it applies to binary files. 
  1 + ;

M: OSCALL ( n ---)
\G Call the operating system service number n.
  32 SPECIAL ;

\ All open files are known by a file-id, which is a number between 1 and 20.
\ All file operations return an io result (ior), which is zero if the
\ operation was successful and nonzero in the case of an error.

\ File ID's must be nonzero, therefore 1 higher than those used
\ by the OS.

: OPEN-FILE ( c-addr u mode --- fid ior)
\G Open the file with the name starting at c-addr and with length u.
\G File must already exist unless open mode is write only.
\G Return the file-ID and the IO result. (ior=0 if success)
  7 OSCALL SWAP 1+ SWAP ;

: CREATE-FILE ( c-addr u mode --- fid ior)
\G Create a new file with the name starting at c-addr with length u. 
\G Return the file-ID and the IO result. (ior=0 if success)
  1 AND 02 + OPEN-FILE ;

: CLOSE-FILE ( fid --- ior)
\G Close the open file described by fid.
  1- 8 OSCALL ;

: WRITE-LINE ( c-addr u fid --- ior)
\G Write the string at addr c-addr with length u to the file described by
\G fid. Append the end of line character to it.
  1- 9 OSCALL ;

: READ-LINE ( c-addr u1 fid --- u2 flag ior) 
\G Read a line from the file described by fid to a buffer at c-addr that
\G is u1+2 characters long. The line is at most u1 characters long.
\G flag is 0 at the end of file (no line could be read) TRUE otherwise.
\G (ior is 0 in this case.)
\G n2 is the length of the line read, 
  1- 10 OSCALL ;

: WRITE-FILE  ( c-addr u fid --- ior)
\G Write a block of u bytes starting at c-addr to the file described by
\G fid. (file must be opened in BIN mode).
  1- 11 OSCALL ;

: READ-FILE   ( c-addr u1 fid --- u2 ior)
\G Read a block of u1 bytes starting at c-addr from the file described by
\G fid. (file must be opened in BIN mode). u2 is the number of bytes 
\G actually read. This is less than u1 at the end of the file. 
  1- 12 OSCALL ;

: DELETE-FILE ( c-addr u --- ior)
\G Delete the file with a name starting at c-addr with length u.
  13 OSCALL ;

: REPOSITION-FILE ( ud fid --- ior)
\G Set the file position of the open file described by fid to ud.
  1- 14 OSCALL ;

: FILE-POSITION  ( fid --- ud ior)
\G ud is the file position of the open file described by fid.
  1- 15 OSCALL ;

: SYSTEM ( c-addr u --- ior)
\G Execute a the string at c-addr with length u as a system command.
  16 OSCALL ;


: FILE-SIZE  ( fid --- ud ior)
\G ud is the file size of the file described by fid. 
  1- 17 OSCALL ;

\ PART 7: SOURCE INPUT WORDS.

VARIABLE TERMMODE

: SETRAW ( --- )
\G Make the input raw mode.
   1 4 32 SPECIAL 1 TERMMODE ! ;

: NONRAW ( --- )
\G Make the input nonraw.
   0 4 32 SPECIAL 0 TERMMODE ! ;



: ACCEPT ( c-addr n1 --- n2 )
\G Read a line from the terminal to a buffer starting at c-addr with 
\G length n1. n2 is the number of characters read,
\ key is not echoed because it is 'cooked' mode. Backspace processing is
\ already in place for easy adaptation to 'raw' input.
  >R 0
  BEGIN
   KEY DUP 8 = OVER 127 = OR 
   IF \ Backspace/delete
     DROP DUP IF 1-  TERMMODE @ IF 8 EMIT THEN THEN 
   ELSE
     DUP 10 = OVER 13 = OR 
     IF \ CR/LF
      DROP SWAP DROP R> DROP SPACE EXIT      
     ELSE
      TERMMODE @ IF DUP EMIT THEN 
      OVER R@ - IF   
       >R OVER OVER + R> SWAP C! 1+
      ELSE
       DROP
      THEN
     THEN 
   THEN 
  0 UNTIL     
;

VARIABLE TIB ( --- addr) 
\G is the standard terminal input buffer.
80 CHARS-T ALLOT-T

VARIABLE SPAN ( --- addr)
\G This variable holds the number of characters read by EXPECT.

VARIABLE #TIB ( --- addr)
\G This variable holds the number of characters in the terminal input buffer.

VARIABLE >IN  ( --- addr)
\G This variable holds an index in the current input source where the next word 
\G will be parsed.

VARIABLE SID  ( --- addr)
\G This variable holds the source i.d. returned by SOURCE-ID.

VARIABLE SRC  ( --- addr)
\G This variable holds the address of the current input source.

VARIABLE #SRC ( --- addr)
\G This variable holds the length of the current input source.

VARIABLE LOADLINE ( --- addr)
\G This variable holds the line number in the file being included.


: EXPECT ( c-addr u --- )
\G Read a line from the terminal to a buffer at c-addr with length u.
\G Store the length of the line in SPAN. 
  ACCEPT SPAN ! ;

: QUERY ( --- )
\G Read a line from the terminal into the terminal input buffer.
  TIB 80 ACCEPT #TIB ! ;

: SOURCE ( --- addr len)
\G Return the address and length of the current input source.
   SRC @ #SRC @ ;

: SOURCE-ID ( --- sid)
\G Return the i.d. of the current source i.d., 0 for terminal, -1 
\G for EVALUATE and positive number for INCLUDE file.
   SID @ ;

: REFILL ( --- f)
\G Refill the current input source when it is exhausted. f is
\G true if it was successfully refilled.
  SOURCE-ID -1 = IF
   0 \ Not refillable for EVALUATE
  ELSE
   SOURCE-ID IF
    SRC @ 256 SOURCE-ID READ-LINE -37 ?THROW
    SWAP #SRC ! 0 >IN !
    #SRC @ IF SOURCE OVER + SWAP DO I C@ 9 = IF 32 I C! THEN LOOP THEN
    1 LOADLINE +!
    \ Change tabs to space. 
    \ flag from READ-LINE is returned (no success at EOF)
   ELSE
    QUERY #TIB @ #SRC ! 0 >IN ! -1 \ Always successful from terminal. 
   THEN 
  THEN
; 

: SCAN ( c-addr1 u1 c --- c-addr2 u2 )
\G Find the first occurrence of character c in the string c-addr1 u1
\G c-addr2 u2 is the remaining part of the string starting with that char.
\G It is a zero-length string if c was not found.
  BEGIN
   OVER
  WHILE
   ROT DUP C@ >R OVER R> = 
   IF
    ROT ROT DROP EXIT
   THEN
   1+ ROT ROT SWAP 1- SWAP 
  REPEAT DROP    
;  
 
: SKIP ( c-addr1 u1 c --- c-addr2 u2 )
\G Find the first character not equal to c in the string c-addr1 u1
\G c-addr2 u2 is the remaining part of the string starting with the
\G nonmatching char. It is a zero-length string if no other chars found.
  BEGIN
   OVER
  WHILE
   ROT DUP C@ >R OVER R> - 
   IF
    ROT ROT DROP EXIT
   THEN
   1+ ROT ROT SWAP 1- SWAP 
  REPEAT DROP    
;  

: PARSE ( c --- addr len )
\G Find a character sequence in the current source that is delimited by
\G character c. Adjust >IN to 1 past the end delimiter character.
  >R SOURCE >IN @ - SWAP >IN @ + R> OVER >R >R SWAP 
  R@ SKIP OVER R> SWAP >R SCAN IF 1 >IN +! THEN 
  DUP R@ - R> SWAP 
  ROT R> - >IN +! ;

: PLACE ( addr len c-addr --- )
\G Place the string starting at addr with length len at c-addr as
\G a counted string.
  OVER OVER C! 
  1+ SWAP CMOVE ;

: WORD ( c --- addr )
\G Parse a character sequence delimited by character c and return the
\G address of a counted string that is a copy of it. The counted
\G string is actually placed at HERE. The character after the counted
\G string is set to a space.
  PARSE HERE PLACE HERE BL HERE COUNT + C! ;

VARIABLE CAPS ( --- a-addr)
\G This variable contains a nonzero number if input is case insensitive.

: UPPERCASE? ( --- )
\G Convert the parsed word to uppercase is CAPS is true.
   CAPS @ HERE C@ AND IF
   HERE COUNT 0 DO 
    DUP I + C@ DUP 96 > SWAP 123 < AND IF DUP I + DUP C@ 32 - SWAP C! THEN
   LOOP DROP
  THEN
;


\ PART 8: INTERPRETER HELPER WORDS

\ First we need FIND and related words.

\ Each word list consists of a number of linked list of definitions (number
\ is a power of 2). Hashing
\ is used to speed up dictionary search. All names in the dictionary
\ are at aligned addresses and FIND is optimized to compare one 4-byte
\ cell at a time.  

\ Dictionary definitions are built as follows:
\ 
\ LINK field: 1 cell, aligned, contains name field of previous word in thread.
\ NAME field: counted string of at most 31 characters.
\             bits 5-7 of length byte have special meaning.
\                   7 is always set to mark start of name ( for >NAME)
\                   6 is set if the word is immediate.
\                   5 is set if the word is a macro.
\ CODE field: first aligned address after name, is execution token for word.
\             here the executable code for the word starts. (is 1 cell for
\             variables etc.)
\ PARAMETER field: (body) Contains the data of constants and variables etc.

VARIABLE NAMEBUF ( --- a-addr)
\G An aligned buffer that holds a copy of the name that is searched.
28 ALLOT-T

VARIABLE FORTH-WORDLIST ( --- addr)
32 CELLS-T ALLOT-T
\G This array holds pointers to the last definition of each thread in the Forth
\G word list.

VARIABLE LAST ( --- addr)
\G This variable holds a pointer to the last definition created.

VARIABLE CONTEXT 28 ALLOT-T ( --- a-addr)
\G This variable holds the addresses of up to 8 word lists that are
\G in the search order.

VARIABLE #ORDER ( --- addr)
\G This variable holds the number of word list that are in the search order.

VARIABLE CURRENT ( --- addr)
\G This variable holds the address of the word list to which new definitions
\G are added.

: HASH ( c-addr u #threads --- n)
\G Compute the hash function for the name c-addr u with the indicated number
\G of threads.
  >R OVER C@ 1 LSHIFT OVER 1 > IF ROT CHAR+ C@ 2 LSHIFT XOR ELSE ROT DROP 
   THEN XOR 
  R> 1- AND 
;  
                        
: NAME>BUF ( c-addr u ---)  
\G Move the name c-addr u to the aligned buffer NAMEBUF.
  NAMEBUF 32 0 FILL 32 MIN NAMEBUF PLACE ; 


: SEARCH-WORDLIST ( c-addr u wid --- 0 | xt 1 xt -1)
\G Search the wordlist with address wid for the name c-addr u.
\G Return 0 if not found, the execution token xt and -1 for non-immediate
\G words and xt and 1 for immediate words.
  ROT ROT 
  NAME>BUF
  NAMEBUF COUNT 2 PICK @ HASH 1+ CELLS SWAP + @ \ Get the right thread.  
  DUP IF
  BEGIN 
   NAMEBUF @ OVER @ $1FFFFFFF AND = \ Compare first cells of names.
           \ Mask off bits 5-7 of name length, these have special purposes.
   IF 
    NAMEBUF 4 + 
    OVER 4 +
    4 >R
    BEGIN
     OVER @ OVER @ - >R
     CELL+ SWAP CELL+ SWAP R> R> CELL+ >R \ Compare all of name until different
    UNTIL 
    R> NAMEBUF C@ 1 + ALIGNED 4 + = IF \ match to the right length then found
     SWAP DROP SWAP C@  
     64 AND IF \ determine immediateness of a word from bit 6 of name length.
      1
     ELSE
      -1
     THEN
     SWAP 4 - SWAP EXIT \ Exit with execution token and flag.  
    THEN
    DROP DROP 
   THEN
   4 - @ DUP 0=  \ Take address of next word from link field until 0
  UNTIL
  THEN
  DROP 0 \ Not found.
;

: FIND ( c-addr --- c-addr 0| xt 1|xt -1 )
\G Search all word lists in the search order for the name in the
\G counted string at c-addr. If not found return the name address and 0.
\G If found return the execution token xt and -1 if the word is non-immediate
\G and 1 if the word is immediate.
  #ORDER @ DUP 1 > IF
   CONTEXT #ORDER @ 1- CELLS + DUP @ SWAP 4 - @ = 
  ELSE 0 THEN
  IF 1- THEN \ If last wordlist is double, don't search it twice.
  BEGIN
   DUP
  WHILE
   1- >R
   DUP COUNT 
   R@ CELLS CONTEXT + @ SEARCH-WORDLIST
   DUP
   IF
    R> DROP ROT DROP EXIT \ Exit if found.     
   THEN 
   DROP R>
  REPEAT
;

\ The following words are related to numeric input.

: DIGIT? ( c -- 0| c--- n -1)
\G Convert character c to its digit value n and return true if c is a
\G digit in the current base. Otherwise return false.
  48 - DUP 0< IF DROP 0 EXIT THEN
  DUP 9 > OVER 17 < AND IF DROP 0 EXIT THEN
  DUP 9 > IF 7 - THEN
  DUP BASE @ < 0= IF DROP 0 EXIT THEN
  -1  
;

: >NUMBER ( ud1 c-addr1 u1 --- ud2 c-addr2 u2 )
\G Convert the string at c-addr with length u1 to binary, multiplying ud1
\G by the number in BASE and adding the digit value to it for each digit.
\G c-addr2 u2 is the remainder of the string starting at the first character
\G that is no digit.
  BEGIN
   DUP
  WHILE
   1 - >R
   COUNT DIGIT? 0= 
   IF
    R> 1+ SWAP 1 - SWAP  EXIT
   THEN  
   SWAP >R 
   >R 
   SWAP BASE @ UM* ROT BASE @ * 0 SWAP D+ \ Multiply ud by base.
   R> 0 D+                                \ Add new digit.
   R> R> 
  REPEAT
;  
  
: CONVERT ( ud1 c-addr1 --- ud2 c-addr2)
\G Convert the string starting at c-addr1 + 1 to binary. c-addr2 is the
\G address of the first non-digit. Digits are added into ud1 as in >NUMBER
  1 - -1 >NUMBER DROP ;

: NUMBER? ( c-addr ---- d f)
\G Convert the counted string at c-addr to a double binary number.
\G f is true if and only if the conversion was successful. DPL contains
\G -1 if there was no point in the number, else the position of the point 
\G from the right. Special prefixes: # means decimal, $ means hex.
  -1 DPL !
  BASE @ >R
  COUNT   
  OVER C@ 45 = DUP >R IF 1 - SWAP 1 + SWAP THEN \ Get any - sign 
  OVER C@ 36 = IF 16 BASE ! 1 - SWAP 1 + SWAP THEN   \ $ sign for hex.
  OVER C@ 35 = IF 10 BASE ! 1 - SWAP 1 + SWAP THEN   \ # sign for decimal
  DUP  0 > 0= IF  R> DROP R> BASE ! 0 EXIT THEN   \ Length 0 or less?
  >R >R 0 0 R> R>
  BEGIN  
   >NUMBER  
   DUP IF OVER C@ 46 = IF 1 - DUP DPL ! SWAP 1 + SWAP ELSE \ handle point. 
         R> DROP R> BASE ! 0 EXIT THEN   \ Error if anything but point  
       THEN    
  DUP 0= UNTIL DROP DROP R> IF DNEGATE THEN    
  R> BASE ! -1  
;

\ PART 9: THE COMPILER

VARIABLE ERROR$ ( --- a-addr )
\G Variable containing string address of ABORT" message.

VARIABLE HANDLER ( --- a-addr )
\G Variable containing return stack address where THROW should return.

: (ABORT") ( f -- - )
\G Runtime part of ABORT"
           IF R>  ERROR$ ! -2 THROW  
           ELSE R> COUNT + ALIGNED >R THEN ;

: THROW ( n --- )
\G If n is nonzero, cause the corresponding CATCH to return with n.
DUP IF
 HANDLER @ IF
  HANDLER @ RP!
  RP@ 4 + @ HANDLER ! \ point to previous exception frame.
  R>                  \ get old stack pointer. 
  SWAP >R SP! DROP R> \ save throw code temp. on ret. stack set old sp.
  R> DROP             \ remove address of handler.
                      \ return stack points to return address of CATCH.
 ELSE
  WARM \ Warm start if no exception frame on stack.
 THEN
ELSE
 DROP \ continue if zero.
THEN  
;  

: DIV-EX ( --- )
\G Divide excpetion handler
  -10 THROW ;

: TIMER-EX ( --- )
\G Timer interrupt handler
  49 SPECIAL ;

: BREAK-EX ( --- )
\G Break key handler
  -28 THROW ;

: CATCH ( xt --- n )
\G Execute the word with execution token xt. If it returns normally, return
\G 0. If it executes a THROW, return the throw parameter. 
 HANDLER @ >R  \ push handler on ret stack.
 SP@ >R        \ push stack pointer on ret stack,
 RP@ HANDLER ! 
 EXECUTE 
 RP@ 4 + @ HANDLER ! \ set handler to previous exception frame.
 R> DROP R> DROP \ remove exception frame.
 0 \ return 0
;

: ALLOT ( n --- )
\G Allot n extra bytes of memory, starting at HERE to the dictionary.
  DP +! ;

: , ( x --- )
\G Append cell x to the dictionary at HERE.
  HERE ! 1 CELLS ALLOT ;

: C, ( n --- )
\G Append character c to the dictionary at HERE.
  HERE C! 1 ALLOT ;

: ALIGN ( --- )
\G Add as many bytes to the dictionary as needed to align dictionary pointer.
  BEGIN HERE 03 AND WHILE 0 C, REPEAT ;

: >NAME ( addr1 --- addr2 )
\G Convert execution token addr1 (address of code) to address of name.
  BEGIN 1- DUP C@ 128 AND UNTIL ;

: NAME> ( addr1 --- addr2 )
\G Convert address of name to address of code.
  COUNT 31 AND + ALIGNED ;

: HEADER ( --- )
\G Create a header for a new definition without a code field. 
  ALIGN 0 , \ Create link field.
  HERE LAST !         \ Set LAST so definition can be linked by REVEAL
  32 WORD UPPERCASE?
           DUP FIND IF ." Redefining: " HERE COUNT TYPE CR THEN DROP
                       \ Give warning if existing word redefined.
  DUP COUNT CURRENT @ @ HASH 1+ CELLS CURRENT @ + @ HERE CELL- !
                       \ Set link field to point to the right thread
  C@ 1+ HERE C@ 128 + HERE C! ALLOT ALIGN 
                       \ Allot the name and set bit 7 in length byte.
; 

: REVEAL ( --- )
\G Add the last created definition to the CURRENT wordlist.
  LAST @ DUP COUNT 31 AND \ Get address and length of name 
  CURRENT @ @ HASH        \ compute hash code.
  1+ CELLS CURRENT @ + ! ;

: CREATE ( "ccc" --- )
\G Create a definition that returns its parameter field address when 
\G executed. Storage can be added to it with ALLOT.
  HEADER REVEAL POSTPONE DOVAR ;

: VARIABLE ( "ccc" --- )
\G Create a variable where 1 cell can be stored. When executed it
\G returns the address.
  CREATE 0 , ;

: CONSTANT ( x "ccc" ---)
\G Create a definition that returns x when executed.
\ Definition contains lit & return in its code field.
  HEADER REVEAL $8000003F , , ;


VARIABLE STATE ( --- a-addr)
\G Variable that holds the compiler state, 0 is interpreting 1 is compiling.

: ]  ( --- )
\G Start compilation mode.
  1 STATE ! ;

: [  ( --- )
\G Leave compilation mode. 
  0 STATE ! ; IMMEDIATE

VARIABLE OPLOC ( --- a-addr)
\G This variable holds the location of the cell where new opcodes are added.
  
VARIABLE OPSHIFT ( --- a-addr)
\G This variable holds the number of bits by which new opcode is shifted.

: CODEFLUSH  ( --- ) 
\G Indicate that no more opcodes 
\G may be filled in at current location.
\G The unused opcodes are left 0 (noop).
 32 OPSHIFT ! ;

: INSERT-OPCODE ( i --- ) 
\G Insert opcode i (0..31) into definition.
   OPSHIFT @ 30 > IF                                            
      HERE OPLOC ! 1 ,  \ Create a new 'sixpack' instruction
      1 OPSHIFT !          \ where opcodes may be inserted.
   THEN   
   OPSHIFT @ LSHIFT OPLOC @ +! \ Add the shifted opcode.
   5 OPSHIFT +!            \ Next opcode must be shifted 5 more.
;

: LITERAL ( n --- ) 
\G Add a literal to the current definition.
  31 INSERT-OPCODE , ; IMMEDIATE

: EXPAND-MACRO ( xt --- )
\G Copy the code contained in the definition xt into the current definition. 
          BEGIN
            DUP @         \ Get next instruction.
            DUP 1 AND 0= -100 ?THROW 
            1 RSHIFT
            SWAP 4 + SWAP   \ Increment source address.
            6 0 DO \ Pick apart the six opcodes.
              DUP 31 AND ?DUP IF \ Skip opcode 0 (noop)
               DUP INSERT-OPCODE
               31 = IF \ Copy the literal for opcode 31 (lit) 
                      OVER @ , SWAP 4 + SWAP 
                    THEN
              THEN
              5 RSHIFT    
            LOOP
            1 AND \ Repeat until return bit encountered.  
           UNTIL
           DROP
;

: COMPILE, ( xt --- )
\G Add the execution semantics of the definition xt to the current definition.
  DUP >NAME C@ 32 AND 
  IF 
   EXPAND-MACRO
  ELSE
   DUP @ LIT DOVAR = IF
    4 + LITERAL \ Convert variable to literal.
   ELSE
    DUP @ $8000003F = 
    IF
     4 + @ LITERAL  \ Convert constant to literal.
    ELSE
     CODEFLUSH ,    \ Lay down subroutine address. 
    THEN
   THEN
  THEN
;

VARIABLE CSP ( --- a-addr )
\G This variable is used for stack checking between : and ; 

VARIABLE 'LEAVE ( --- a-addr) 
\ This variable is used for LEAVE address resolution.

: !CSP ( --- )
\G Store current stack pointer in CSP.
   SP@ CSP ! ;

: ?CSP ( --- )
\G Check that stack pointer is equal to value contained in CSP. 
   SP@ CSP @ - -22 ?THROW ;

: ; ( --- )
\G Finish the current definition by adding a return to it, make it
\G visible and leave compilation mode.
    $80000000 OPSHIFT @ 32 = IF \ If no current opcode location then 
        1 + ,              \ Make new instruction with six noops and return.
    ELSE 
        OPLOC @ +! \ else set return bit of current instruction.
    THEN CODEFLUSH [ \ Quit compilation state.
    ?CSP REVEAL
; IMMEDIATE

: (POSTPONE) ( --- )
\G Runtime for POSTPONE.
\ has inline argument.
  R> DUP @ SWAP CELL+ >R 
  DUP >NAME C@ 64 AND IF EXECUTE ELSE COMPILE, THEN 
;  

: : ( "ccc" --- )
\G Start a new definition, enter compilation mode.
  !CSP HEADER ] ;

\ The following words are for control structures. They use the conditional
\ branch instruction (bit 1 is set). Unconditional branch is made by
\ opcode 28 (constant 0) and conditional branch.

: ?PAIRS ( n1 n2 ---)
\G Check that n1 matches n2, throw an error if not, used to check
\G correct pairing of control structures.
    - -22 ?THROW ;

: BEGIN ( --- x n)
\G Start a BEGIN UNTIL or BEGIN WHILE REPEAT loop.
  CODEFLUSH HERE 01 ; IMMEDIATE

: UNTIL ( x n --- )
\G Form a loop with matching BEGIN. 
\G Runtime: A flag is take from the stack
\G each time UNTIL is encountered and the loop iterates until it is nonzero. 
  01 ?PAIRS CODEFLUSH 02 + , ; IMMEDIATE 

: IF    ( --- x n)
\G Start an IF THEN or IF ELSE THEN construction. 
\G Runtime: At IF a flag is taken from
\G the stack and if it is true the part between IF and ELSE is executed,
\G otherwise the part between ELSE and THEN. If there is no ELSE, the part
\G between IF and THEN is executed only if flag is true.
  CODEFLUSH HERE 1 CELLS ALLOT 02 ; IMMEDIATE

: THEN ( x n ---)
\G End an IF THEN or IF ELSE THEN construction.
  02 ?PAIRS CODEFLUSH HERE 02 + SWAP ! ; IMMEDIATE

: ELSE ( x1 n2 --- x2 n2)
\G part of IF ELSE THEN construction.
  28 INSERT-OPCODE POSTPONE IF 2SWAP POSTPONE THEN ; IMMEDIATE 

: WHILE  ( x1 --- x2 x1 )
\G part of BEGIN WHILE REPEAT construction.
\G Runtime: At WHILE a flag is taken from the stack. If it is false,
\G  the program jumps out of the loop, otherwise the part between WHILE
\G  and REPEAT is executed and the loop iterates to BEGIN.
  POSTPONE IF 2SWAP ; IMMEDIATE

: REPEAT ( x1 n1 x2 n2 --- )
\G part of BEGIN WHILE REPEAT construction.
  28 INSERT-OPCODE POSTPONE UNTIL POSTPONE THEN ; IMMEDIATE

VARIABLE POCKET ( --- a-addr )
\G Buffer for S" strings that are interpreted.
  252 ALLOT-T

: '  ( "ccc" --- xt)
\G Find the word with name ccc and return its execution token.
  32 WORD UPPERCASE? FIND 0= -13 ?THROW ;

: ['] ( "ccc" ---)
\G Compile the execution token of the word with name ccc as a literal.
  ' LITERAL ; IMMEDIATE

: CHAR ( "ccc" --- c)
\G Return the first character of "ccc".
  BL WORD 1 + C@ ;

: [CHAR] ( "ccc" --- )
\G Compile the first character of "ccc" as a literal.
  CHAR LITERAL ; IMMEDIATE

: DO ( --- x n)
\G Start a DO LOOP.
\G Runtime: ( n1 n2 --- ) start a loop with initial count n2 and 
\G limit n1.
  POSTPONE (DO) 'LEAVE @ HERE 0 'LEAVE ! 03 ; IMMEDIATE

: ?DO ( --- x n)
\G Start a ?DO LOOP.
\G Runtime: ( n1 n2 --- ) start a loop with initial count n2 and
\G limit n1. Exit immediately if n1 = n2.  
  POSTPONE (?DO) 'LEAVE @ HERE 'LEAVE ! 0 , HERE 03 ; IMMEDIATE

: LEAVE ( --- )
\G Runtime: leave the matching DO LOOP immediately.
\ All places where a leave address for the loop is needed are in a linked
\ list, starting with 'LEAVE variable, the other links in the cells where
\ the leave addresses will come.
  POSTPONE (LEAVE) HERE 'LEAVE @ , 'LEAVE ! ; IMMEDIATE

: RESOLVE-LEAVE 
\G Resolve the references to the leave addresses of the loop.
          'LEAVE @ 
          BEGIN DUP WHILE DUP @ HERE ROT ! REPEAT DROP ; 

: LOOP  ( x n--- )
\G End a DO LOOP.
\G Runtime: Add 1 to the count and if it is equal to the limit leave the loop.
  3 ?PAIRS POSTPONE (LOOP) , RESOLVE-LEAVE 'LEAVE ! ; IMMEDIATE

: +LOOP ( x n--- )
\G End a DO +LOOP 
\G Runtime: ( n ---) Add n to the count and exit if this crosses the 
\G boundary between limit-1 and limit. 
  3 ?PAIRS POSTPONE (+LOOP) , RESOLVE-LEAVE 'LEAVE ! ; IMMEDIATE

: RECURSE ( --- )
\G Compile a call to the current (not yet finished) definition.
  LAST @ NAME> COMPILE, ; IMMEDIATE

: ."  ( "ccc<quote>" --- )
\G Parse a string delimited by " and compile the following runtime semantics.
\G Runtime: type that string.
   POSTPONE (.") 34 WORD C@ 1+ ALLOT ALIGN ; IMMEDIATE 


: S"  ( "ccc<quote>" --- )
\G Parse a string delimited by " and compile the following runtime semantics.
\G Runtime: ( --- c-addr u) Return start address and length of that string. 
  STATE @ IF POSTPONE (S") 34 WORD C@ 1+ ALLOT ALIGN 
             ELSE 34 WORD COUNT POCKET PLACE POCKET COUNT THEN ; IMMEDIATE 

: ABORT"  ( "ccc<quote>" --- )
\G Parse a string delimited by " and compile the following runtime semantics.
\G Runtime: ( f --- ) if f is nonzero, print the string and abort program.
  POSTPONE (ABORT") 34 WORD C@ 1+ ALLOT ALIGN ; IMMEDIATE

: ABORT ( --- )
\G Abort unconditionally without a message.
 -1 THROW ;

: POSTPONE ( "ccc" --- )
\G Parse the next word delimited by spaces and compile the following runtime.
\G Runtime: depending on immediateness EXECUTE or compile the execution 
\G semantics of the parsed word.
  POSTPONE (POSTPONE) ' , ; IMMEDIATE

: IMMEDIATE ( --- )
\G Make last definition immediate, so that it will be executed even in
\G compilation mode.
  LAST @ DUP C@ 64 OR SWAP C! ;

: ( ( "ccc<rparen>" --- )
\G Comment till next ).
  41 PARSE DROP DROP ; IMMEDIATE  

: \
\G Comment till end of line. 
  SOURCE >IN ! DROP ; IMMEDIATE

M: >BODY ( xt --- a-addr)
\G Convert execution token to parameter field address.
  4 + ;

: (;CODE) ( --- )
\G Runtime for DOES>, exit calling definition and make last defined word
\G execute the calling definition after (;CODE) 
  R> LAST @ NAME> ! ;

: DOES>  ( --- )
\G Word that contains DOES> will change the behavior of the last created
\G word such that it pushes its parameter field address onto the stack
\G and then executes whatever comes after DOES> 
  POSTPONE (;CODE)  
  27 INSERT-OPCODE \ Compile the R> primitive, which is the first
                   \ instruction that the defined word performs.                   
; IMMEDIATE

\ PART 10: TOP LEVEL OF INTERPRETER 

: ?STACK ( ---)
\G Check for stack over/underflow and abort with an error if needed.
  DEPTH DUP 0< -4 ?THROW 10000 > -3 ?THROW ;

: INTERPRET ( ---)
\G Interpret words from the current source until the input source is exhausted.
  BEGIN
   32 WORD UPPERCASE?  DUP C@ 
  WHILE
   FIND DUP 
   IF 
    -1 = STATE @ AND 
    IF
     COMPILE,
    ELSE 
     EXECUTE 
    THEN
   ELSE DROP  
    NUMBER? 0= -13 ?THROW  
    DPL @ 1+ IF
     STATE @ IF SWAP LITERAL LITERAL THEN  
    ELSE  
     DROP STATE @ IF LITERAL THEN 
    THEN
   THEN  ?STACK  
  REPEAT   DROP
;

: EVALUATE ( c-addr u --- )
\G Evaluate the string c-addr u as if it were typed on the terminal.
  SID @ >R SRC @ >R #SRC @ >R  >IN @ >R
  #SRC ! SRC ! 0 >IN ! -1 SID ! INTERPRET
  R> >IN ! R> #SRC ! R> SRC ! R> SID ! ;  

VARIABLE INCLUDE-BUFFER ( --- a-addr)
\G This is the buffer where the lines of included files are stored.
508 ALLOT-T

VARIABLE INCLUDE-POINTER ( --- a-addr)
\G This variable holds the address where the included line is stored.


: INCLUDE-FILE ( fid --- ) 
\G Read lines from the file identified by fid and interpret them.
\G INCLUDE and EVALUATE nest in arbitrary order.
  INCLUDE-POINTER @ >R SID @ >R SRC @ >R #SRC @ >R >IN @ >R
  LOADLINE >R
  #SRC @ INCLUDE-POINTER +! INCLUDE-POINTER @ SRC !
  SID ! 0 LOADLINE !
  BEGIN
   REFILL
  WHILE
   INTERPRET
  REPEAT
  R> LOADLINE !
  R> >IN ! R> #SRC ! R> SRC ! R> SID ! R> INCLUDE-POINTER ! 
;

: INCLUDED  ( c-addr u ---- )
\G Open the file with name c-addr u and interpret all lines contained in it.
  R/O OPEN-FILE -38 ?THROW 
  DUP >R INCLUDE-FILE
  R> CLOSE-FILE DROP      
; 

: INCLUDE ( "ccc" ----)
\G Open the file with name "ccc" and interpret all lines contained in it.
    BL WORD COUNT INCLUDED ;

VARIABLE ERRORS ( --- a-addr)
\G This variable contains the head of a linked list of error messages.

: ERROR-SOURCE ( --- )
\G Print location of error source.
     SID @ 0 > IF
      ." in line " LOADLINE @ . 
     THEN
     HERE COUNT TYPE CR WARM
;  

VARIABLE NESTING
\G Variable to hold nesting for conditional compilation.

VARIABLE COLD-STARTUP
: QUIT ( --- )
\G This word resets the return stack, resets the compiler state, the include
\G buffer and then it reads and interprets terminal input.
  NESTING OFF    
  R0 @ RP! [
  TIB SRC ! 0 SID !
  INCLUDE-BUFFER INCLUDE-POINTER !  
  32 OPSHIFT ! 
  BEGIN
   COLD-STARTUP @ IF
       POCKET COLD-STARTUP @ COLD-STARTUP OFF ['] INCLUDED
   ELSE
       REFILL DROP ['] INTERPRET
   THEN
   CATCH DUP 0= IF
	DROP STATE @ 0= IF ." OK" THEN CR
   ELSE \ throw occured.
     DUP -2 = IF
      ERROR$ @ COUNT TYPE SPACE
     ELSE
      ERRORS @
      BEGIN DUP WHILE
       OVER OVER @ = IF 8 + COUNT TYPE SPACE ERROR-SOURCE THEN 4 + @   
      REPEAT DROP       
      ." Error " . 
     THEN ERROR-SOURCE
   THEN      
  0 UNTIL
;

: WARM ( ---) 
\G This word is called when an error occurs. Clears the stacks, sets
\G BASE to decimal, closes the files and resets the search order.
  R0 @ RP! S0 @ SP! DECIMAL 
  21 1 DO I CLOSE-FILE DROP LOOP  
  2 #ORDER !
  FORTH-WORDLIST CONTEXT ! 
  FORTH-WORDLIST CONTEXT CELL+ !
  FORTH-WORDLIST CURRENT ! 
  SETRAW
  0 HANDLER !
  QUIT ;

: COLD ( --- )
\G The first word that is called at the start of Forth.
  SP@ S0 !
  RP@ 4 + R0 ! \ Initialize variables SO and RO
  POCKET 256 18 32 SPECIAL COLD-STARTUP !
  COLD-STARTUP @ 0= IF  
      ." Welcome to SOD32 Forth version 0.6" CR
      ." Copyright 2025 L.C. Benschop GPLv2" CR
  THEN
  WARM ;

END-CROSS
