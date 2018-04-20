# MWCA 
MWCA is a multi-stream word-based compression algorithm based on spaceless word model. MWCA is semi-static, which means, it creates two seperate dictionary files for each input. Dictionaries are created using most frequent 255+65536 words. Implementation details and performance results could be obtained from article:  
"E. ÖZTÜRK, A. MESUT, B. DİRİ, "Çoklu Akış Destekli Kelime Tabanlı Sıkıştırma Algoritması", Uluslararası Bilgisayar Bilimleri ve Mühendisliği Konferansı, UBMK 2017."  
  
MWCA generates 6 seperate streams from one input file.  
s1-> Dictionary for most frequent 255 keywords.  
s2-> Dictionary for most frequent 65536 keywords.   
f-> Codewords of words from s1.  
s-> Codewords of words from s2.  
z-> Bit vector containing word sequence.  
o-> Uncompressed words which is excluded from 255+65536 most frequent words.  
  
MWCA could be used as:  
  
./MWCA inputfile  
  
It wil generate 6 different files. Generated files will add their own extensions to the file name. For example:  
./MWCA loremipsum.txt will generate  
loremipsum.txt.s1  
loremipsum.txt.s2  
loremipsum.txt.f  
loremipsum.txt.s  
loremipsum.txt.z  
loremipsum.txt.o  
  
To decompress, MWDA could be used as:  
./MWDA loremipsum.txt  
  
MWDA needs the input file name without additional 6 stream extensions but needs the 6 files and not the original one.  
The implementation doesn't handle exceptions so if it couldn't find one or more of the 6 streams the application will crush.  
MWDA generates original file from six streams with another extension ".wba" which will create loremipsum.txt.wba for our example. And MWDA will not remove the six streams. The aim of this implementation choice is to get experimental results about compression ratio and to compare input / output files.
