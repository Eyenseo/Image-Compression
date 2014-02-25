Image Compression
=================

This is a project from my third semester at the [Hochschule Bremen](http://www.hs-bremen.de/).<br \>
The project started at the 17.10.2013 and ended at the 19.11.2013

The makefile was only tested under Ubuntu 13.10 x64.

###Program functions:
```
The program can encode any file via the Huffman entropy encoding.
Additional it can transform a ppm file with the discrete cosiuns transformation.
It is able to Work with P1, P2, P3, P4, P5 and P6. 
Also it can display the difference between two pictures that went through the DCT  

Usage:
  main -h <input> <output> (--en|--de) [--detail]
    Example:
      main -h ./myFile.whatEver ./compressed.humdidum --en
  main -d <input> <output> (--en|--de) [--detail] [--quality] [--quanti]
    Example:
      main -d ./myPicture.ppm ./compressed.humdi --en --quality 50
  main -p <first> <second> <output> [--rmse]
    Example:
      main -p ./myFirstPicture.ppm ./mySecondPicture.ppm ./difference.ppm 
--rmse

Arguments:
  -h:    Selects the Huffman encoding / decoding
    <input>             Specifies the input file
    <output>            Specifies the output file
    --en                Sets the mode to encoding
    --de                Sets the mode to decoding
    --detail            Will show details of the compression
  -d:    Selects the Discrete Cosinus Transformation for ppm files
    <input>             Specifies the input file
    <output>            Specifies the output file
    --en                Sets the mode to encoding
    --de                Sets the mode to decoding
    --detail            Will show details of the compression
    --quality <number>  Sets the quality of the compression in range of 0
                        and 100. (Default 100)
    --quanti <file>     Loads a quantisation matrix from a file.
                        The format of the Quantisation Matrix has to be
                        the following:
                        It has to be an 8x8 matrix. There are only values
                        allowed between 0 and 255. Each value has to be
                        seperated by a space or a tab. The value may only 
                        be Integers.
                        Example:
                          16 11 10 16 24 40 51 61
                          12 12 14 19 26 58 60 55
                          14 13 16 24 40 57 69 56
                          14 17 22 29 51 87 80 62
                          18 22 37 56 68 109 103 77
                          24 35 55 64 81 104 113 92
                          49 64 78 87 103 121 120 101
                          72 92 95 98 112 100 103 99
  -p:    Selects the difference picture
    <first>             Specifies the first Image (original)
    <second>            Specifies the second Image (transformed)
    <output>            Specifies the output file
    --rmse:             Will print the root mean square error of the two 
                        pictures
  --help:             Show this help
```
