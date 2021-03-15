clear; clc; close('all');
pkg load signal
pkg load ltfat

graphics_toolkit('gnuplot')

# Parameters
D = 1;
V0 = 0;
fs = 44.1e3;
f = 1e2;
Ain = 2;

Is = 1e-12;
is1 = Is;
is2 = Is;

Vt = 26e-3;
vt1 = Vt;
vt2 = Vt;

gr1 = 1/2.2e3;

# Euler
t = 0:(1 / fs):(2 / f);
N = length(t);

Vin = Ain * sawtooth (2 * pi * f * t);
Vout = zeros(1, N);
max_iter = 200
cov = 0.001;

for i = 1:N
  
  v1 = Vin(i);
  v2 = V0;
  vlast = 0;
  
  
  # diodes
  for j = 0:max_iter - 1
    
    vd1 = 0 - v2;
    ed1 = exp(vd1 / vt1);
    id1 = is1 * ed1 - is1;
    gd1 = is1 * ed1 / vt1;
    id1eq = id1 - gd1 * vd1;
    
    vd2 = v2 - 0;
    ed2 = exp(vd2 / vt2);
    id2 = is2 * ed2 - is2;
    gd2 = is2 * ed2 / vt2;
    id2eq = id2 - gd2 * vd2;
    
    v2 = (id1eq - id2eq + gr1 * v1) / (gd1 + gd2 + gr1);
    
    if(abs(vlast - v2) <= cov)
      printf("Number of iteration for %d sample is %d\n",i, j);
      break;
    endif
    
    vlast = v2;
  endfor
    
    Vout(i) = v2;
    
endfor

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



