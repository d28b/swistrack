function [blobsCoordinate] = FindBlobs(image, numberBlobs, level)

%[blobsCoordinate] = FindBlobs(image, numberBlobs, level)
%
% Args:
%        image : name of the image where are the blobs
%        numberBlobs : number of Blobs on the image
%        level : threshold of the binarization
%
% Resultat:
%        blobsCoordinate: Vector Table, the first line must be the line of image 
%        coordinate u and the second line must be the line of image 
%        coordinate v.

% François Rey, 05-01-2007 francois.rey@epfl.ch

%Post the image in black and white
I=imread(image);
BW = im2bw(I, level);
BW = -(BW-ones(size(BW)));
imshow(BW);
hold on;

%Find label on images
L=bwlabel(BW);

%Calcul the center of label that the user click on
for i=1:numberBlobs
    blobs=ginput(1);
    nBlobs = L(cast(blobs(1,2),'int16'), cast(blobs(1,1),'int16'));
    while(nBlobs==0)
        warning('You must click on a blob')
        blobs=ginput(1);
        nBlobs = L(cast(blobs(1,2),'int16'), cast(blobs(1,1),'int16'));
    end
    centreX=0;
    centreY=0;
    taille=0;
    for abscisse=1:size(L,2)
        for ordonnee=1:size(L,1)
            if(L(ordonnee,abscisse)==nBlobs)
                centreX=centreX+abscisse;
                centreY=centreY+ordonnee;
                taille=taille+1;
            end
        end
    end
    blobsCoordinate(:,i)=[centreX/taille; centreY/taille]; 
    plot(blobsCoordinate(1,i),blobsCoordinate(2,i),'LineStyle', 'none', 'LineWidth', 2,'Marker', '+', 'MarkerFaceColor','g', 'MarkerSize', 6);
    hold on;
end