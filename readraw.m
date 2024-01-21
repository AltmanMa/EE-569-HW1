function [ img ] = readraw( filename, height, width, gray )
%READRAW Write RAW format image file.
%   filename: file name of the file to read
disp(['Read image '  filename ' ...']);
% Get file ID
fr = fopen(filename,'rb');
% Check if file exists
if (fr == -1)
    error('Can not open output image file. Press CTRL-C to exit \n');
    pause
end
temp=fread(fr, 'uchar');

if(nargin<4)
    gray=true;
end

if(gray)
    img=reshape(temp,[width height]);
    img=img';
else
    img=zeros(height,width,3);
    for i=1:height
        for j=1:width
            img(i,j,1)=temp(((i-1)*width+j-1)*3+1);
            img(i,j,2)=temp(((i-1)*width+j-1)*3+2);
            img(i,j,3)=temp(((i-1)*width+j-1)*3+3);
        end
    end
end
    
fclose(fr);

end
