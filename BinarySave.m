<<<<<<< HEAD
load 2.mat

[d1,d2,d3]=size(imnData);       %wymiary macierzy
D=size(imnData);
A=reshape(imnData,d1,d2*d3);

fileID=fopen('short.bin','w');
fwrite(fileID,D,'uint16');
fwrite(fileID,A,'uint16');
fclose(fileID);

fileID2=fopen('short.bin','r');
=======
load 2.mat

[d1,d2,d3]=size(imnData);       %wymiary macierzy
D=size(imnData);
A=reshape(imnData,d1,d2*d3);

fileID=fopen('short.bin','w');
fwrite(fileID,D,'uint16');
fwrite(fileID,A,'uint16');
fclose(fileID);

fileID2=fopen('short.bin','r');
>>>>>>> 50c6a52e2f207c731bdfd61272f45b1b08bac98e
ReaD=fread(fileID2,'uint16');