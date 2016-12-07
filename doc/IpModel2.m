% Simple numeric version

A = [
    1.0000    0.0100    0.0001    0.0000;
         0    0.9982    0.0267    0.0001;
         0   -0.0000    1.0016    0.0100;
         0   -0.0045    0.3119    1.0016;
         ];
     
B = [
    0.0001;
    0.0182;
    0.0002;
    0.0454;
    ];

C = [
     1     0     0     0;
     0     0     1     0;
     ];

D = [
     0;
     0;
     ];

K =[
  -61.9933  -33.5040   95.0597   18.8300;
  ];

Nbar = -61.5500;

Ts = 1/100;
t = 0:Ts:5;

r = 0.2;        % input desired cart position, m
x0 = zeros(size(A,2),1);    % initial state

x = zeros(size(A,2), length(t));
x_1 = x;
u = zeros(length(t),1);
y = zeros(size(C,1), length(t));

for k=1:length(t)
    if k == 1
        x(:,k) = x0;
    else
        x(:,k) = x_1(:,k-1);
    end
    u(k) = Nbar*r - K*x(:,k);
    y(:,k) = C*x(:,k);
    x_1(:,k) = B*u(k) + A*x(:,k);
end

plot(t,x);grid;
xlabel('Time (sec)');ylabel('State Vector');
legend('Cart Pos (m)','Cart Vel (m/s)','Pend Pos (rad)','Pend Vel (rad/s)');
