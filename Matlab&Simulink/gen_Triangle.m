

function [ ThetaL, ThetaM ] = gen_Triangle(base,height,points,x_distance)
%CIRCLE_GEN AngleValues for drawing a triangle. Number of points you feed it
%needs to be divisible by 4; 



%Generate Y Coordiantes

    slope = (height)/(base/2);
    
    L1 = @(x) slope*x + height;
    L2 = @(x) -slope*x + height;
    
    half_arc = (base/2)/(points/4);

    Yd(1:(points/4+1)) = [-base/2:half_arc:0]';
    
    Yd((points/4+2):points/2+1) = [half_arc:half_arc:base/2]';
    
    Yd((points/2+2):points+1) = base/2-2*base/points:-2*base/points:-base/2;
    
%Generate Z Coordinates

    Zd(1:points/4+1) = L1(Yd(1:(points/4+1)));
    Zd(points/4+2:points/2+1) = L2(Yd((points/4+2):points/2+1));
    Zd(points/2+2:points+1) = 0;
    
%Inverse Kinematics    

    ThetaL = round((180*atan(Yd./x_distance)/pi)',0);
    ThetaM = round((180*(atan(Zd./x_distance)./2 + pi/4)/pi)',0);    
    
end


