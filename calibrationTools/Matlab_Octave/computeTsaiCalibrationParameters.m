function [R,T,f,k1]= computeTsaiCalibrationParameters(image, world, C, dx, dy, sx, dispBool)
%function [R,T,f,k1]= computeTsaiCalibrationParameters(image, world, C, dx, dy, sx, dispBool)
%
% Compute the coplanar calibration using Tsai model developed in his paper:
% "An Efficient and Accurate Camera Calibration Technique for 3D Machine
% Vision", Roger Y. Tsai, Proceedings of IEEE Conference on Computer Vision
% and Pattern Recognition, Miami Beach, FL, 1986, pages 364-374."
% As Ncx and Nfx are not really used nowadays, without framegrabbers, we
% simplified the model this way: In the original model dx'=dx*Ncx/(Nfx*sx),
% thus we removed Ncx and Nfx. If there are differences between Ncx and
% Nfx, modify sx accordingly and dx' will remains correct.
%
% Inputs:
% image     N-by-2 matrix with the image coordinates of the calibration
%           points. The first column contains the X coordinates and second
%           column the Y coordinates. The unit is pixels.
% world     N-by-2 matrix with the world coordinates of the calibration
%           points. The first column contains the X coordinates and second
%           column the Y coordinates.
% C         Lens optical center, in the camera array referential (in pixels)
%           Most of the time, it corresponds to the image center (2D)
% dx        Size, along x, of a camera pixel (look at the camera datasheet)
% dy        Size, along y, of a camera pixel (look at the camera datasheet)
% sx        Uncertainty on dx/dy (In fact the real dx used is dx/sx)
%           If you don't know, set this value to 1 (or close to one)
% dispBool  If set to true, display the errors (default: true)
%
% Outputs:
% R         Rotation Matrix of the pinhole camera model (3D)
% T         Translation Matrix of the pinhole camera model (3D)
% f         Lens focal
% k1        1st order factor of the radial distorsion
%
% Pierre Roduit, 2009-05-29, pierre.roduit@epfl.ch 

%Inputs check

EPSILON = 1.0e-8;
%Test arguments
if ((size(image,2)~= 2))
    error('Image points matrix must be N by 2.')
end
if((size(world,2)~=2))
    error('World points matrix must be N by 2.')
end
if (size(image,1)~= size(world,1))
    error('image and world matrices must have the same number of points.')
end
if (size(image,1)<6)
    error('Matrices must have more than 6 points.')
end
if(size(C,1)*size(C,2)~=2)
    error('C must be a 2D vector')
end
if(not(exist('dispBool','var')))
    dispBool=true;
end

%Inversion of coordinate v
image(:,2)=-image(:,2);
C(2)=-C(2);

%Invert matrices for computing
image=image';
world=world';


Xd=dx/sx*(image(1,:)-C(1));
Yd=dy*(image(2,:)-C(2));

r2 = Xd.^2+Yd.^2;

%Compute the Rotation Matrix R and vector T
%Calcul of the Rotation Matrix R and of tx and ty 
M=[Yd.*world(1,:);Yd.*world(2,:);Yd;-Xd.*world(1,:);-Xd.*world(2,:)]';
a = inv(M'*M)*M'*Xd';

if(abs(a(1))< EPSILON && abs(a(2))<EPSILON)
    ty = sqrt(1/(a(4)^2+ a(5)^2));
elseif (abs(a(4))< EPSILON && abs(a(5))<EPSILON)
    ty = sqrt(1/(a(1)^2+ a(2)^2));
elseif (abs(a(1))<EPSILON && abs(a(4))<EPSILON)
    ty = sqrt(1/(a(2)^2+ a(5)^2));
elseif(abs(a(2))<EPSILON && abs(a(5))<EPSILON)
    ty = sqrt(1/(a(1)^2+ a(4)^2));
else
    sr = a(1)^2+a(2)^2+a(4)^2+a(5)^2;
    ty = sqrt((sr-sqrt(sr^2-4*(a(1)*a(5)-a(4)*a(2))^2))/(2*(a(1)*a(5)-a(4)*a(2))^2));
end

%Find a point far from image center
far_distance=0;
far_point=0;
for i=1:size(world,2)
    if(r2(i)>far_distance)
        far_point=i;
        far_distance = r2(i);
    end
end
    
R(1,1)=a(1)*ty;
R(1,2)=a(2)*ty;
R(2,1)=a(4)*ty;
R(2,2)=a(5)*ty;
tx = a(3)*ty;

x = R(1,1)*world(1,far_point)+R(1,2)*world(2,far_point)+tx;
y = R(2,1)*world(1,far_point)+R(2,2)*world(2,far_point)+ty;

if((sign(x)~=sign(Xd(far_point))) || (sign(y)~=sign(Yd(far_point))))
    ty = -ty;
    R = -R;
    tx = -tx;
end

R(1,3)= sqrt(1-R(1,1)^2-R(1,2)^2);
s = -sign(R(1,1)*R(2,1)+R(1,2)*R(2,2));
R(2,3)= s*sqrt(1-R(2,1)^2-R(2,2)^2);
R(3,:)=(cross(R(1,:)',R(2,:)'))';

%Compute an approximation of f and tz
yc =R(2,1)*world(1,:)+R(2,2)*world(2,:)+ty;
w =R(3,1)*world(1,:)+R(3,2)*world(2,:);
B = [yc; -Yd]';
D = (w.*Yd)';
ftz = inv(B'*B)*B'*D;

%Control the sign of the focal length
if(ftz(1)>0)
    R(1,3)=-R(1,3);
    R(2,3)=-R(2,3);
    R(3,1)=-R(3,1);
    R(3,2)=-R(3,2);
    
    yc =R(2,1)*world(1,:)+R(2,2)*world(2,:)+ty;
    w =R(3,1)*world(1,:)+R(3,2)*world(2,:);
    B = [yc; -Yd]';
    D = (w.*Yd)';
    ftz = inv(B'*B)*B'*D;
    if(ftz(1)>0)
        error('The focal is always negative, there must be an error with the data')
    end
end

%Compute f, tz and k1
fun = @(x) norm(Yd'+x(3)* Yd'.*r2' - x(1)* (R(2,1)*world(1,:)'+ R(2,2)*world(2,:)'+ ty)./(R(3,1)*world(1,:)'+ R(3,2)*world(2,:)'+ x(2)))+norm(Xd'+x(3)*Xd'.*r2'-x(1)*(R(1,1)*world(1,:)'+ R(1,2)*world(2,:)'+ tx)./(R(3,1)*world(1,:)'+ R(3,2)*world(2,:)'+ x(2)));
x0=[ftz(1);ftz(2);0];
%x=fminsearch(fun,x0);
x=fminsearch(fun,x0, optimset('MaxFunEvals', 1000000, 'MaxIter',  1000000));
T=[tx;ty;x(2)];
f = x(1);
k1 = x(3);

%Calcul the orientation of the camera and the errors
image(2,:)=-image(2,:);
C(2)=-C(2);

%Invert matrices back
image=image';
world=world';

%Compute and display errors if desired
if (dispBool)
    % Compute Error
    newReal=TsaiImage2World(image, R, T, f, k1, C, dx, dy, sx);
    [averageError, maxError, stdError]=computeError(world, newReal)
    % Display Error
    figure;
    hold on;
    plot(world(:,1),world(:,2),'+b');
    plot(newReal(:,1),newReal(:,2),'+r');
    legend('Orig. World Points','Computed World Points','Location','SouthOutside');
end
