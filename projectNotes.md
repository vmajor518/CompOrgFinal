**"42 3.14 hello"**

**scanf("%d %f %s", \&num, \&flt, str);**



walks through the string one character at a time and looks for a '%' (use getchar() or getc())

following a '%' should be a format specifier

skip leading whitespace

separates on white space

stops when gets an unexpected type

getc() can read for stdin or file while getchar() is only stdin





**format specifiers**

%s - string

 	ends on white space so does not allow for something like "two word" string

%f - float 6 decimal digits

 	rounds 6th decimal

 	%.xf x number of decimals (only up to 16 then you get garbage)

 

 



**Varied arguments**

takes variable num arguments





**Testing**

Have tests written in files and redirect input from file to stdin

