%% Constant Set 
    TotalTime = 10;
    SampleTime = 0.1;
    Time = 0:SampleTime:TotalTime;
    Samples = length(Time);

%% Initialization
    Zd = 10*ones(Samples,1);
    Xd = zeros(Samples,1);
    Yd = zeros(Samples,1);
    
    Zd1 = 10*ones(Samples,1);
    Xd1 = zeros(Samples,1);
    Yd1 = zeros(Samples,1);

%% Draw 10X10 Square
    Xd1(1:25) = -10;
    Xd1(26:50) = [-10:20/24:10]';
    Xd1(51:75) = 10;
    Xd1(76:101) = [10:(-20/25):-10]';
    
    Yd1(1:25) = [-10:20/24:10]';
    Yd1(26:50) = 10;
    Yd1(51:75) = [10:(-20/24):-10]';
    Yd1(76:101) = -10;

%% Draw Radius 10 Circle 
    Xd(1:51) = [-10:(20/50):10]';
    Xd(52:101) = [10:(-20/49):-10]';
    for c = 1:50
        Yd(c) = sqrt(100-Xd(c)^2);
    end
    for c = 51:101
        Yd(c) = -sqrt(100-Xd(c)^2);
    end

