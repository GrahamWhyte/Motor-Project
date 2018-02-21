% This contains physical constants and non-changing system parameters


% Erase entire Matlab environment
%clear all;

%%%%%%%%%%%%%%%%%%%%%%
% Physical Constants %
%%%%%%%%%%%%%%%%%%%%%%

G = 9.81;               % m/s^2
FtPerM = 3.28;          % ft / m
RadPerDeg = pi/180;     % rad / deg
DegPerRad = 180/pi;     % deg / rad
RadPSecPerRPM = pi/30;  % (rad/s) / rpm

%SI Units
milli = 1e-3;
centi = 1e-2;
micro = 1e-6;
kilo = 1e3;
mega = 1e6;
milsTomm = 0.0254;

Big = 1e6;   % General purpose large number
Sml = 1e-6;  % General purpose small number


% Sensor

% Geometry

% Material
rho_FR4 = 1.850; % g/cm^3

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Indices for Motor Vectors %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Values at Nominal Voltage
NomV          = 1;
NoLoadSpd     = 2;
NoLoadCurr    = 3;
NomSpd        = 4;
NomTorque     = 5;
NomCurr       = 6;
StallTorque   = 7;
StallCurr     = 8;

% Characteristics
TermR         = 9;
TermL         = 10;
TorqueK       = 11;
SpdK          = 12;
RotJ          = 13;

% Physical Dimensions
Weight        = 14;
OuterDiam     = 15;
Length        = 16;




