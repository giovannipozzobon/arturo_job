\ Extensions to sod Forth kernel to make a complete Forth system.
\ Copyright 1994 L.C. Benschop Vught, The Netherlands.
\ The program is released under the GNU General Public License version 2.
\ There is NO WARRANTY.

\ Changes: 2025-01-11: Added ERASE/BLANK, make sure to erase new wordlist.
\                      Added comments to some words.
\          2028-01-28: split off generic extension words from system dependent

: \G POSTPONE \ ; IMMEDIATE
\G comment till end of line for inclusion in glossary.

\ PART 1: MISCELLANEOUS WORDS.

: ERASE ( c-addr u )
\G Fill memory region of u bytes starting at c-addr with zero.    
    0 FILL ;

: COMPARE ( addr1 u1 addr2 u2 --- diff ) 
\G Compare two strings. diff is negative if addr1 u1 is smaller, 0 if it 
\G is equal and positive if it is greater than addr2 u2.
  ROT 2DUP - >R
  MIN DUP IF
   >R 
   BEGIN   
    OVER C@ OVER C@ - IF
     SWAP C@ SWAP C@ - R> DROP R> DROP EXIT
    THEN     
    1+ SWAP 1+ SWAP
    R> 1- DUP >R 0=
   UNTIL R>
  THEN DROP  
  DROP DROP R> NEGATE
;

: M: ( --- )
\G Make a definition similar to : that will be expanded as a macro.
  : LAST @ DUP C@ 32 OR SWAP C! ;

M: <= ( n1 n2 --- f)
\G f is true if and only if n1 is less than or equal to n2.
  > 0= ;

M: >= ( n1 n2 --- f)
\G f is true if and only if n1 is greater than or equal to n2.
  < 0= ;


M: 0<= ( n1 --- f)
\G f is true if and only if n1 is less than zero.
  0 <= ;

M: 0<> ( n1 n2 ---f)
\G f is true of and only of n1 and n2 are not equal.   
  0= 0= ;

M: BOUNDS ( addr1 n --- addr2 addr1)
\G Convert address and length to two bounds addresses for DO LOOP 
  OVER + SWAP ;

: WITHIN ( u1 u2  u3 --- f)
\G f is true if u1 is greater or equal to u2 and less than u3
  2 PICK U> ROT ROT U< 0= AND ;

: -TRAILING ( c-addr1 u1 --- c-addr2 u2)
\G Adjust the length of the string such that trailing spaces are excluded.
  BEGIN
   2DUP + 1- C@ BL = 
  WHILE
   1-
  REPEAT
;

M: NIP ( x1 x2 --- x2)
\G Discard the second item on the stack.
  SWAP DROP ;

M: TUCK ( x1 x2 --- x2 x1 x2 )
\G Copy the top of stack to a position under the second item.  
    SWAP OVER ;

: .(  ( "ccc<rparen>" ---)
\G Print the string up to the next right parenthesis.
   41 PARSE TYPE ;

\ PART 2: SEARCH ORDER WORDLIST

: GET-ORDER ( --- w1 w2 ... wn n )
\G Return all wordlists in the search order, followed by the count.
  #ORDER @ 0 ?DO CONTEXT I CELLS + @ LOOP #ORDER @ ;

: SET-ORDER ( w1 w2 ... wn n --- )
\G Set the search order to the n wordlists given on the stack.
  #ORDER ! 0 #ORDER @ 1- DO CONTEXT I CELLS + ! -1 +LOOP ;

: ALSO ( --- )
\G Duplicate the last wordlist in the search order.
  CONTEXT #ORDER @ CELLS + DUP CELL- @ SWAP ! 1 #ORDER +! ;

: PREVIOUS ( --- )
\G Remove the last wordlist from search order.
   -1 #ORDER +! ;
 
VARIABLE #THREADS ( --- a-addr)
\G This variable holds the number of threads a word list will have.

: WORDLIST ( --- wid)
\G Make a new wordlist and give its address.
  HERE #THREADS @ , HERE #THREADS @ CELLS DUP ALLOT ERASE ;

: DEFINITIONS  ( --- )
\G Set the definitions wordlist to the last wordlist in the search order.
CONTEXT #ORDER @ 1- CELLS + @ CURRENT ! ;

: FORTH ( --- )
\G REplace the last wordlist in the search order with FORTH-WORDLIST
  FORTH-WORDLIST CONTEXT #ORDER @ 1- CELLS + ! ;

1 #THREADS !
WORDLIST 
CONSTANT ROOT-WORDLIST ( --- wid )
\G Minimal wordlist for ONLY

32 #THREADS ! 

: ONLY ( --- )
\G Set the search order to the minimal wordlist.
  1 #ORDER ! ROOT-WORDLIST CONTEXT ! ;

: VOCABULARY ( --- )
\G Make a definition that will replace the last word in the search order
\G by its wordlist.
  WORDLIST CREATE  ,            \ Make a new wordlist and store it in def.
  DOES> >R                      \ Replace last item in the search order.
  GET-ORDER SWAP DROP R> @ SWAP SET-ORDER ;

: ENVIRONMENT? ( c-addr u --- false | val true)
\G Return an environmental query of the string c-addr u    
    2DROP 0 ;

\ Part 2A: Conditional compilation

: [IF] ( f ---)
\G If the flag is false, conditionally skip till the next [ELSE] or [ENDIF]
    0= IF
	BEGIN 
	    BEGIN
		BL WORD UPPERCASE? COUNT
		DUP WHILE
		    2DUP S" [ELSE]" COMPARE 0= IF
			2DROP NESTING @ 0= IF EXIT THEN
		    ELSE
			2DUP S" [THEN]" COMPARE 0= IF
			    2DROP NESTING @ 0= IF EXIT ELSE -1 NESTING +! THEN
			ELSE
			    S" [IF]" COMPARE 0= IF
				1 NESTING +!
			    THEN
			THEN
		    THEN	    
	    REPEAT
	    2DROP REFILL 0=
	UNTIL
	NESTING OFF
    THEN	
; IMMEDIATE

: [ELSE] ( --- )
    0 POSTPONE [IF] ; IMMEDIATE
\G Used in [IF] [ELSE] [THEN] for conditional compilation.    

: [THEN] ( --- )
\G Terminate [IF] [THEN] does nothing.
    ; IMMEDIATE

: [DEFINED] ( "ccc" --- f)
\G Produce a flag indicating whether the next word is defined.	
    BL WORD UPPERCASE? FIND SWAP DROP 0<> ; IMMEDIATE

\ PART 3: SOME UTILITIES, DUMP .S WORDS
 
: DL ( addr1 --- addr2 )
\G hex/ascii dump in one line of 16 bytes at addr1 addr2 is addr1+16
  BASE @ >R 16 BASE ! CR
  DUP 0 <# # # # # # # # # #> TYPE ." : "
  16 0 DO
   DUP I + C@ 0 <# # # #> TYPE SPACE 
  LOOP 
  16 0 DO
   DUP I + C@ DUP 127 AND 31 < IF DROP ." ." ELSE EMIT THEN
  LOOP 
  16 + R> BASE ! ;


: DUMP ( addr len --- )
\G Show a hex/ascii dump of the memory block of len bytes at addr  
  15 + 4 RSHIFT 0 DO
   DL
  LOOP DROP ; 

: .S ( --- )
\G Show the contents of the stack.
     DEPTH IF
      0 DEPTH 2 - DO I PICK . -1 +LOOP 
     ELSE ." Empty " THEN ;


: ID. ( nfa --- )
\G Show the name of the word with name field address nfa.
  COUNT 31 AND TYPE SPACE ;

: WORDS ( --- )
\G Show all words in the last wordlist of the search order.
  CONTEXT #ORDER @ 1- CELLS + @ 
  DUP @ >R \ number of threads to return stack.
  CELL+ R@ 0 DO DUP I CELLS + @ SWAP LOOP DROP \ All thread pointers to stack.
  BEGIN
   0 0  
   R@ 0 DO 
    I 2 + PICK OVER U> IF  
     DROP DROP I I 1 + PICK
    THEN 
   LOOP \ Find the thread pointer with the highest address. 
  WHILE
   DUP 1+ PICK DUP ID. \ Print the name.
   CELL- @             \ Link to previous.
   SWAP 2 + CELLS SP@ + ! \ Update the right thread pointer.
  REPEAT
  DROP R> 0 DO DROP LOOP  \ Drop the thread pointers.  
;


ROOT-WORDLIST CURRENT !
: FORTH FORTH ;
: ALSO ALSO ;
: ONLY ONLY ;
: PREVIOUS PREVIOUS ;
: DEFINITIONS DEFINITIONS ;
: WORDS WORDS ;
DEFINITIONS
\ Fill the ROOT wordlist.

\ PART 4: ERROR MESSAGES

: MESS" ( n "cccq" --- )
\G Create an error message for throw code n.
  ALIGN , ERRORS @ , HERE 8 - ERRORS ! 34 WORD C@ 1+ ALLOT ;

-3 MESS" Stack overflow"
-4 MESS" Stack underflow"
-10 MESS" Divide overflow"
-13 MESS" Undefined word"
-22 MESS" Incomplete control structure"
-28 MESS" BREAK key pressed"
-37 MESS" File I/O error"
-38 MESS" File does not exist"

: 2CONSTANT  ( d --- )
\G Create a new definition that has the following runtime behavior.
\G Runtime: ( --- d) push the constant double number on the stack. 
  CREATE HERE 2! 8 ALLOT DOES> 2@ ;

: D.R ( d n --- )
\G Print double number d right-justified in a field of width n. 
  >R SWAP OVER DABS <# #S ROT SIGN #> R> OVER - 0 MAX SPACES TYPE ;

: U.R ( u n --- )
\G Print unsigned number u right-justified in a field of width n. 
  >R 0 R> D.R ;

: .R ( n1 n2 --- )
\G Print number n1 right-justified in a field of width n2. 
 >R S>D R> D.R ;

: KEY? ( --- f)
\G f is true if and only if a key is pressed and KEY will return immediately
  3 OSCALL ;

: VALUE ( n --- ) 
\G Create a variable that returns its value when executed, prefix it with TO
\G to change its value.    
  CREATE , DOES> @ ;

: TO 
\G Change the value of the following VALUE type word.    
  ' >BODY STATE @ IF
      POSTPONE LITERAL POSTPONE ! 
  ELSE
   !
  THEN
; IMMEDIATE 

: D- ( d1 d2 --- d3)
\G subtract double numbers d2 from d1.    
  DNEGATE D+ ;

: D0= ( d ---f)
\G f is true if and only if d is equal to zero.    
  OR 0= ;

: D= ( d1 d1  --- f)
\G f is true if and only if d1 and d2 are equal.
  D- D0= ;

: BLANK ( c-addr u ----)
\G Fill the memory region of u bytes starting at c-addr with spaces.
  32 FILL ;


: AGAIN ( x ---)
\G Terminate a loop forever BEGIN..AGAIN loop.
  POSTPONE 0 POSTPONE UNTIL ; IMMEDIATE

: CASE ( --- )
\G Start a CASE..ENDCASE construct. Inside are one or more OF..ENDOF blocks.
\G runtime the CASE blocks takes one value from the stack and uses it to
\G select one OF..ENDOF block.
  CSP @ SP@ CSP ! ; IMMEDIATE
: OF ( --- x)
\G Start an OF..ENDOF block. At runtime it pops a value from the stack and
\G executes the block if this value is equal to the CASE value.
  POSTPONE OVER POSTPONE = POSTPONE IF POSTPONE DROP ; IMMEDIATE
: ENDOF ( x1 --- x2)
\G Terminate an OF..ENDOF block.
  POSTPONE ELSE ; IMMEDIATE
: ENDCASE ( variable# ---)
\G Terminate a CASE..ENDCASE construct.
  POSTPONE DROP BEGIN SP@ CSP @ - WHILE POSTPONE THEN REPEAT
  CSP ! ; IMMEDIATE

: SAVE-SYSTEM ( "ccc" --- )
\G Save the Forth system to a file.
  BL WORD COUNT W/O BIN CREATE-FILE ABORT" Can't open"
  0 HERE 2 PICK WRITE-FILE DROP
  CLOSE-FILE DROP ;

CAPS ON

