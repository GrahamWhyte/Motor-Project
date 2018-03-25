function [ ThetaL, ThetaM ] = circle_Gen(radius,points, x_distance)
%CIRCLE_GEN Will Output the angle values for adrawing a circle
  %X_distance is distance from laser to projecting surface
  %Number of points must be even

%Generate Y Coordiantes

    half_arc = (2*radius)/(points/2);
    Yd(1:(points/2+1)) = [-radius:half_arc:radius]';
    Yd((points/2+2):points) = [radius-half_arc:-half_arc:-radius+half_arc]';

%Generate Z Coordinates

    for c = 1:points/2
        Zd(c) = sqrt(radius^2-(Yd(c))^2);
    end
    for c = (points/2 + 1): points
        Zd(c) = -sqrt(radius^2-(Yd(c))^2);
    end
    
%Inverse Kinematics    

    ThetaL = round((180*atan(Yd./x_distance)/pi)',0);
    ThetaM = round((180*(atan(Zd./x_distance)./2 + pi/4)/pi)',0);    
    
end
