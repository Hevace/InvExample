# -*- coding: utf-8 -*-
""" Simple numeric model """

import numpy as np
import matplotlib.pyplot as plt

A = np.matrix([[1.0000, 0.0100,  0.0001, 0.0],
               [0.0,    0.9982,  0.0267, 0.0001],
               [0.0,    0.0,     1.0016, 0.0100],
               [0.0,    -0.0045, 0.3119, 1.0016]])

B = np.matrix([[0.0001],
               [0.0182],
               [0.0002],
               [0.0454]])
               
C = np.matrix([[1, 0, 0, 0],
               [0, 0, 1, 0]])
               
D = np.matrix([[0],
               [0]])
               
K = np.matrix([-61.9933, -33.5040, 95.0597, 18.8300])

Nbar = -61.5500

Ts = 1.0/100
t = np.arange(0.0, 5.0, Ts)

r = 0.2         # input desired cart position, m

Nstates = A.shape[1]        # number of states
Noutputs = C.shape[0]       # number of outputs
x0 = np.asmatrix(np.zeros((Nstates, 1)))    # initial state
x = np.asmatrix(np.zeros((Nstates, t.size)))
x_1 = x
u = np.asmatrix(np.zeros((t.size,1)))
y = np.asmatrix(np.zeros((Noutputs, t.size)))

for k in np.arange(t.size):
    if k == 1:
        x[:,k] = x0
    else:
        x[:,k] = x_1[:,k-1]
    u[k] = Nbar * r - K * x[:,k]
    y[:,k] = C * x[:,k]
    x_1[:,k] = B*u[k] + A*x[:,k]

plt.figure(1)
plt.clf()
plt.plot(t,x.T)
plt.grid(True)
plt.xlabel('Time (sec)')
plt.ylabel('State Vector')
plt.title('Inverted Pendulum Step Response')
plt.legend(('Cart Pos (m)','Cart Vel (m/s)','Pend Pos (rad)','Pend Vel (rad/s)'))
plt.show()
