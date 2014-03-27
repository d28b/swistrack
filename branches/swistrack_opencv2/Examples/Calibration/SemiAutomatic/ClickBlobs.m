function [blobs] = ClickBlobs(image, blobs_count, level)
% Lets the user click on the blobs of an image and reports the centers of the blobs that have been clicked.
%
% Arguments:
%        image: image with the blobs (e.g. as returned by imread(file))
%        blobs_count: number of blobs on the image
%        level: threshold level when converting the image to black and white
%
% Result:
%        blobs: Vector Table, the first line must be the line of image 
%        coordinate u and the second line must be the line of image 
%        coordinate v.

% François Rey, 2007-01-05, francois.rey@epfl.ch

% Convert the image to black and white
BW = im2bw(image, level);
BW = -(BW-ones(size(BW)));

% Show the image on the screen
imagesc(BW);
axis equal;
hold on;

% Find label on images
L=bwlabel(BW);
%imagesc(L);

% Let the user click on the blobs and calculate their centers
disp('Click on the blobs in the image!');
for i=1:blobs_count
	% Retrieve the blob on which the user clicked
	while 1
	    user_click=ginput(1);
    	nBlobs = L(cast(user_click(1, 2),'int16'), cast(user_click(1, 1),'int16'));
    	if nBlobs>0, break; end
        warning('You must click on a blob!')
    end

	% Get the center of the blob
    centerX=0;
    centerY=0;
    count=0;
    for x=1:size(L, 2)
        for y=1:size(L, 1)
            if L(y, x)==nBlobs
                centerX=centerX+x;
                centerY=centerY+y;
                count=count+1;
            end
        end
    end
    centerX=centerX/count;
    centerY=centerY/count;
    disp(['Blob at ' num2str(centerX) ' ' num2str(centerY)])
    blobs(i, :)=[centerX, centerY];

	% Draw the point
    plot(blobs(i, 1), blobs(i, 2), 'LineStyle', 'none', 'LineWidth', 2, 'Marker', '+', 'MarkerFaceColor', 'g', 'MarkerSize', 6);
end
