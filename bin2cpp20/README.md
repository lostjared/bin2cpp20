# bin2cpp20

Unix Tool to Convert a stream to a C++20 array/vector/string

% cat *cpp | bin2cpp20 -i stdin -v variablename 

% bin2cpp20 -i filename -o outputheader -v variable 

% bin2cpp20 -i filename -o -v variable -s --sort

or arguments:

jared@iLostSideDead bin2cpp % bin2cpp20 -h

-h              help      

-i              input file/stdin

-o              output    

-s              string output

-v              variable name

-z              sort      

--help          help message

--input         input file/stdin

--output        output file

--string        string output

--variable      variable name

--sort          sort string

