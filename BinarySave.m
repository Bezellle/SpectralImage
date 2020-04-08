load 2.mat

[d1,d2,d3]=size(imnData);       %wymiary macierzy
D=size(imnData);
A=reshape(imnData,d1,d2*d3);

fileID=fopen('short.bin','w');
fwrite(fileID,D,'uint16');
fwrite(fileID,A,'uint16');
fclose(fileID);

fileID2=fopen('short.bin','r');
ReaD=fread(fileID2,'uint16');