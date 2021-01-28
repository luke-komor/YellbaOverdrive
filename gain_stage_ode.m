clear; clc; close('all');
pkg load signal
pkg load ltfat

graphics_toolkit('gnuplot')

# ODE
function [dV] = dVdt (V, In, D)
  Cc = 51e-12;
  R2 = 51e3 + D * 500e3;
  
  Is = 1e-12;
  Vt = 26e-3;
  
  dV = In/Cc - V/(R2*Cc) - 2 * Is * sinh(V/Vt); 
  
endfunction

function [dI] = dIdt(I, Vi, Vim1, h)
  R = 4.7e3;
  C = 0.047e-6;
  
  dI = -I/(R * C) + (1/R) * ((Vi - Vim1)/h);
  
endfunction

# Parameters
D = 1;      # distortion
In = 1e-6;  # static for now
V0 = 0;
fs = 192e3;
f = 1e2;

# Euler
t = 0:1/fs:2/f;
N = length(t);
A = 0.01;
Vin = A*sin(2*pi*f*t);
V = zeros(1, N);
I = zeros(1, N);

h = 1/fs;

for i = 1:N - 1
  
  #I(i+1) = I(i) + h * dIdt(I(i), Vin(i+1), Vin(i), h);
  In = Vin(i)/4.7e3;
  
  V(i+1) = V(i) + h * dVdt(V(i), In, D);
  
endfor

Vout = V + Vin;

subplot(2, 1, 1)
plot(t, Vout)

fa = fs*(0:(N/2))/N;
Y = fft(Vout);
P2 = abs(Y/N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2*P1(2:end-1);

subplot(2, 1, 2)
plot(fa, mag2db(P1))
xlim([0 10e3])



