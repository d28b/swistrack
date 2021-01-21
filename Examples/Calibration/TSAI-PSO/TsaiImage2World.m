function world= TsaiImage2World(image, R, T, f, k1, C, dx, dy,sx)
%function world= TsaiImage2World(image, R, T, f, k1, C, dx, dy)
%
% This function compute the transformation from the image coordinates to
% the world coordinates, using the Tsai calibration parameters
%
% Input:
% image     The image points coordinates. The first column corresponds to
%           the X coordinates, the second to the Y coordinates. The matrix
%           must be a N by 2 matrix
% R         Rotation matrix
% T         Translation vector
% f         Focal length of the camera lens
% k1        First order parameter of the optical distorsion
% C         The position of the optical center on the CCD (in pixels)
% dx        Distance between two pixels centers of the CCD array, along x
% dy        Distance between two pixels centers of the CCD array, along y
%
% Output:
% world     Computed world coordinates of the image points
%
% Pierre Roduit, 2009-05-29, pierre.roduit@epfl.ch 


% Check inputs
if ((size(image,2)~= 2))
    error('Image points matrix must be Nx2.')
end
if(size(R,1)~=3 | size(R,2)~=3)
    error('R matrix must be 3x3.')
end
if(size(T,1)*size(T,2)~=3)
    error('T must be a 3x1 vector.')
end
if(size(C,1)*size(C,2)~=2)
    error('C must be a 3x1 vector.')
end

% Invert v coordinates
image(:,2)= -image(:,2);
C(2)=-C(2);

Xd=dx.*(image(:,1)-C(1));
Yd=dy.*(image(:,2)-C(2));
r2 = (Xd.^2+Yd.^2);

% Calculate the real coordinate
nominator=f.*(R(2,2).*(T(1))-R(1,2).*T(2))+(1+k1.*r2).*((R(1,2).*Yd-R(2,2).*Xd).*T(3)+R(3,2).*(Xd.*T(2)-Yd.*T(1)));
denominator=f.*(R(1,2).*R(2,1)-R(1,1).*R(2,2))+(1+k1.*r2).*(Xd.*(R(2,2).*R(3,1)-R(2,1).*R(3,2))+Yd.*(R(1,1).*R(3,2)-R(1,2).*R(3,1)));
world(:,1)=nominator./denominator;

nominator=-(f.*(R(2,1).*(T(1))-R(1,1).*T(2))+(1+k1.*r2).*((R(1,1).*Yd-R(2,1).*Xd).*T(3)+R(3,1).*(Xd.*T(2)-Yd.*T(1))));
denominator=(f.*(R(1,2).*R(2,1)-R(1,1).*R(2,2))+(1+k1.*r2).*(Xd.*(R(2,2).*R(3,1)-R(2,1)*R(3,2))+Yd.*(R(1,1).*R(3,2)-R(1,2).*R(3,1))));
world(:,2)=nominator./denominator;
