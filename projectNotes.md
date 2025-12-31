**"42 3.14 hello"**

**scanf("%d %f %s", \&num, \&flt, str);**



walks through the string one character at a time and looks for a '%' (use getchar() or getc())

following a '%' should be a format specifier

skip leading whitespace

separates on white space

stops when gets an unexpected type

getc() can read for stdin or file while getchar() is only stdin





**format specifiers**

%d - ints

 	+/- optional signs allowed

%s - string

 	ends on white space so does not allow for something like "two word" string

%f - float 6 decimal digits

 	rounds 6th decimal

 	%.xf x number of decimals (only up to 16 then you get garbage)

 

 



**Varied arguments**

takes variable num arguments

... int the function definition allows varied number of arguments

va\_list stores them 'list' of the arguments 

va\_start() gets the first thing in the 'list'

va\_arg() gets the next one in the 'list' of the specified type





**Testing**

Have tests written in files and redirect input from file to stdin



**Process**

Loop through the format string one char at a time 

First check if it is a literal or whitespace

&nbsp;	if whitespace consume any amount of whitespace from stream and keep going until you hit a non whitespace character

&nbsp;		then put back the non whitespace character you consumed 

&nbsp;	if it is a literal then look for an exact match and if not found return immediately 

If not it must be a %

&nbsp;	Check for modifiers ie how many characters to read or store

&nbsp;	Determine which format specifier is appropriate

Call format specifier

Increment successful conversions if applicable





Format specifier helper function

&nbsp;	Reads on character at a time from the input stream

&nbsp;	Checks if it is the type it is supposed to be

&nbsp;	If there is a width modifier only read a certain number of digits

&nbsp;	Upon hitting whitespace or mismatch type store the value in the appropriate data type



