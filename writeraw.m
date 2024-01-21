function count = writeraw(G, filename, gray)
%writeraw - write RAW format image file 
% Usage :	writeraw(G, filename) or writeraw(G, filename, true)
% G:		input image matrix
% filename: file name of the file to write to disk
% gray:     true for gray scale img, false for RGB img. (default true)
% count:	return value, the elements written to file

disp([' Write image data to '  filename ' ...']);

% Get file ID
fid = fopen(filename,'wb');

% Check if file exists
if (fid == -1)
    error('Can not open output image file. Press CTRL-C to exit \n');
    pause
end

if(nargin<3)
    gray=true;
end

if(gray)
    % Reshape matrix to col vector
    temp = reshape(G',[],1);
else
    [m,n,~]=size(G);
    temp=zeros(m*n*3,1);
    temp(1:3:m*n*3)=reshape(G(:,:,1)',[],1);
    temp(2:3:m*n*3)=reshape(G(:,:,2)',[],1);
    temp(3:3:m*n*3)=reshape(G(:,:,3)',[],1);
end

% Write and close file
count = fwrite(fid,temp, 'uchar');
fclose(fid);

end %function
                            
