%% Initialization
Constants;
JamecoWHITE12V85W;
M0 = MotorParam;
JamecoRED12V85W;
M1 = MotorParam;
Trajectory;
Encoder;

%%  Unit Conversions (Kg,m,s,N,A,V,Rad/Sec,Heneries,Ohms)             
M0(NoLoadSpd) = M0(NoLoadSpd)*RadPSecPerRPM; %RPM to RAD/SEC
M0(NomTorque) = M0(NomTorque)*milli;
M0(StallTorque) = M0(StallTorque)*milli;
M0(TermL) = M0(TermL)*milli;
M0(RotJ) = M0(RotJ)*(milli)*(centi)^2;   %gram*cm^2 => kg*m^2
M0(Weight) = M0(Weight)*milli;
M0(OuterDiam) = M0(OuterDiam)*milli;
M0(Length) = M0(Length)*milli;
M0(TorqueK) = M0(TorqueK)*milli; %mNm => Nm
M0(SpdK) = M0(TorqueK);

M1(NoLoadSpd) = M1(NoLoadSpd)*RadPSecPerRPM; %RPM to RAD/SEC
M1(NomTorque) = M1(NomTorque)*milli;
M1(StallTorque) = M1(StallTorque)*milli;
M1(TermL) = M1(TermL)*milli;
M1(RotJ) = M1(RotJ)*(milli)*(centi)^2;   %Gram*cm^2 => kg*m^2
M1(Weight) = M1(Weight)*milli;
M1(OuterDiam) = M1(OuterDiam)*milli;
M1(Length) = M1(Length)*milli;
M1(TorqueK) = M1(TorqueK)*milli;
M1(SpdK) = M0(TorqueK);

EncoderRadius = EncoderRadius/10;%cm
EncoderWidth = EncoderWidth*milsTomm/10; %cm


%% Calculations          
        %% Dynamic Friction from No Load Test
            % Torque = B(Wm) => At constant speed.
            NomTorque0 = M0(NoLoadCurr)*M0(TorqueK);
            Dyan_Fric0 =  3.34e-6; %NomTorque0/M0(NoLoadSpd);
            NomTorque1 = M1(NoLoadCurr)*M1(TorqueK);
            Dyan_Fric1 = NomTorque1/M1(NoLoadSpd);
        
        %% Moment of Inertia for Mechanical System
        
           % Encoder Inertia Calculation
           EncoderVolume = pi*(EncoderRadius)^2*(EncoderWidth); %cm^3
           EncoderMass = EncoderVolume*rho_FR4; %g
           EncoderJ = (1/2)*EncoderMass*(EncoderRadius^2); %gcm^2
           EncoderJ = EncoderJ*milli*(centi^2); 
        
        SysJ0 = EncoderJ;
        SysJ1 = EncoderJ;
            
%%     Transfer Functions      
    %% Motor/Mechanical Dynamics Model
        % MechX:             1
        %                ---------
        %                  Js + B
        % Torque/Velocity Transfer (No spring in model)
        Mech0n = [1];
        Mech0d = [M0(RotJ) + SysJ0, Dyan_Fric0];

        Mech1n = [1];
        Mech1d = [M1(RotJ) + SysJ1, Dyan_Fric1];
        
        TCon0 = M0(TorqueK);
        TCon1 = M1(TorqueK);
        
        NomCurr0 = M0(NomCurr);
        NomCurr1 = M1(NomCurr);
        StallCurr0 = M0(StallCurr);
        StallCurr1 = M1(StallCurr);
     
        SpdCon0 = M0(SpdK);
        SpdCon1 = M1(SpdK);
       
        % ElecX:             1
        %                ---------
        %                 Ls + R
        % Voltage/Current Transfer Function

        Elec0n = [1];
        Elec0d = [M0(TermL),M0(TermR)];
        Elec1n = [1];
        Elec1d = [M1(TermL),M1(TermR)];

    %%  Electronics Model           
        Amp0n =1;
        Amp0d =1;

        Amp1n =1;
        Amp1d =1;

        Sens0 = 1;
        Sens1 = 1;






