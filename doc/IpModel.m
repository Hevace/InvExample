% State space model and controller for inverted pendulum
% http://ctms.engin.umich.edu/CTMS/index.php?example=InvertedPendulum&section=ControlDigital

M = 0.5;    % mass of the cart, kg
m = 0.2;    % mass of the pendulum, kg
b = 0.1;    % coefficient of friction for cart, N/m/sec
I = 0.006;  % mass moment of inertia of the pendulum, kg.m^2
g = 9.8;    % gravity, m/s^2
l = 0.3;    % length to pendulum center of mass, m
% u = force applied to the cart, N
% x = cart position coordinate, m
% phi = pendulum deviation from vertical (up), rad


p = I*(M+m)+M*m*l^2; %denominator for the A and B matrices

A = [0      1              0           0;
     0 -(I+m*l^2)*b/p  (m^2*g*l^2)/p   0;
     0      0              0           1;
     0 -(m*l*b)/p       m*g*l*(M+m)/p  0];
 
B = [     0;
     (I+m*l^2)/p;
          0;
        m*l/p];
    
C = [1 0 0 0;
     0 0 1 0];
 
D = [0;
     0];

states = {'x' 'x_dot' 'phi' 'phi_dot'};
inputs = {'u'};
outputs = {'x'; 'phi'};

sys_ss = ss(A,B,C,D,'statename',states,'inputname',inputs,'outputname',outputs);

Ts = 1/100;

sys_d = c2d(sys_ss,Ts,'zoh');


%%% LQR Design
A = sys_d.a;
B = sys_d.b;
C = sys_d.c;
D = sys_d.d;
Q = C'*C;
Q(1,1) = 5000;
Q(3,3) = 100;
R = 1;
[K] = dlqr(A,B,Q,R);
Nbar = -61.55;

Ac = [(A-B*K)];
Bc = [B];
Cc = [C];
Dc = [D];

states = {'x' 'x_dot' 'phi' 'phi_dot'};
inputs = {'r'};
outputs = {'x'; 'phi'};

sys_cl = ss(Ac,Bc*Nbar,Cc,Dc,Ts,'statename',states,'inputname',inputs,'outputname',outputs);

t = 0:0.01:5;
r =0.2*ones(size(t));
[y,t,x]=lsim(sys_cl,r,t);
[AX,H1,H2] = plotyy(t,y(:,1),t,y(:,2),'plot');
set(get(AX(1),'Ylabel'),'String','cart position (m)')
set(get(AX(2),'Ylabel'),'String','pendulum angle (radians)')
title('Step Response with Digital LQR Control')
