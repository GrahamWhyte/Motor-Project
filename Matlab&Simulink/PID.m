function [] = PID( OLTF )
%PID Summary of this function goes here
%   Detailed explanation goes here

pzplot(OLTF*tf(1,[1,0]))
run = 1;

while(run == 1)
prompt = 'Complex or Real Zero? Enter 1 for Real, 0 for Complex ';
imag = input(prompt);

if(imag == 1)
    prompt = 'First Zero ';
    s1 = input(prompt);
    prompt = 'Second Zero ';
    s2 = input(prompt);
    PID = OLTF*tf([1,s1+s2,s1*s2],[1,0]);
else
    prompt = 'Real Part ';
    s1 = input(prompt);
    prompt = 'Imaginary ';
    s2 = input(prompt);
    PID = OLTF*tf([1,-2*s1,s2^2 + s1^2],[1,0]);
end
rlocus(PID);

prompt = 'New Zeros? Enter 1 ';
run = input(prompt);
end
run1 = 1;
while(run1 == 1)
    prompt = 'Gain';
    kd = input(prompt);
    prompt = 'New Gain ?';
    run1 = input(prompt);
end
step(feedback(kd*PID,1));
pzplot(feedback(kd*PID,1));

if(imag == 1)
    P = (s1+s2)*kd
    I = s1*s2*kd
else
    P = -2*s1*kd
    I = (s2^2 + s1^2)*kd
end
    D = kd
end







