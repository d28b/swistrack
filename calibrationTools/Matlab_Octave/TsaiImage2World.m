function world= TsaiImage2World(image, R, T, f, k1, C, dx, dy, sx)
%function world= TsaiImage2World(image, R, T, f, k1, C, dx, dy, sx)
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
% sx        Uncertainty factor on dx/dy, close to 1
%
% Output:
% world     Computed world coordinates of the image points
%
% Pierre Roduit, 2009-05-29, pierre.roduit@epfl.ch 


%Check inputs
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

%Invert v coordinates
image(:,2)= -image(:,2);
C(2)=-C(2);

for i=1:size(image,1)
    Xd(i)=dx/sx*(image(i,1)-C(1));
    Yd(i)=dy*(image(i,2)-C(2));
end
r2 = (Xd.^2+Yd.^2);

%Calcul of the real coordinate
for i=1:size(image,1)
    world(i,1) = (f*(R(2,2)*(T(1))-R(1,2)*T(2))+(1+k1*r2(i))*((R(1,2)*Yd(i)-R(2,2)*Xd(i))*T(3)+R(3,2)*(Xd(i)*T(2)-Yd(i)*T(1))))/(f*(R(1,2)*R(2,1)-R(1,1)*R(2,2))+(1+k1*r2(i))*(Xd(i)*(R(2,2)*R(3,1)-R(2,1)*R(3,2))+Yd(i)*(R(1,1)*R(3,2)-R(1,2)*R(3,1))));
    world(i,2) =-(f*(R(2,1)*(T(1))-R(1,1)*T(2))+(1+k1*r2(i))*((R(1,1)*Yd(i)-R(2,1)*Xd(i))*T(3)+R(3,1)*(Xd(i)*T(2)-Yd(i)*T(1))))/(f*(R(1,2)*R(2,1)-R(1,1)*R(2,2))+(1+k1*r2(i))*(Xd(i)*(R(2,2)*R(3,1)-R(2,1)*R(3,2))+Yd(i)*(R(1,1)*R(3,2)-R(1,2)*R(3,1))));
end