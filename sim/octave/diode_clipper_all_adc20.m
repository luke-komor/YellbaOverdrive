clear; clc; close('all');
pkg load signal
pkg load ltfat

graphics_toolkit('gnuplot')

# Parameters
D = 1;
V0 = 0;
fs = 8 * 44.1e3;
f = 1e3;
Ain = 10;

Is = 1e-12;
is1 = Is;
is2 = Is;

Vt = 26e-3;
vt1 = Vt;
vt2 = Vt;

gr1 = 1/2.2e3;
gr2 = 1/6.8e3;

m = 0.5;
minv = 1/m;

gc1 = 0.47e-6 * fs * minv;
gc2 = 0.01e-6 * fs * minv;

ic1eq = 0;
ic2eq = 0;


t = 0:(1 / fs):(2 / f);
N = length(t);

Vin = Ain * sawtooth (2 * pi * f * t);
Vout = zeros(1, N);

max_iter = 400;
cov = 0.0001;

iter = 0;

for i = 1:N
  
  v1 = Vin(i);
  v2 = V0;
  v3 = V0;
  
  vlast2 = 0;
  vlast3 = 0;
  
  # diodes
  for j = 0:max_iter - 1
    
    vd1 = 0 - v3;
    ed1 = exp(vd1 / vt1);
    id1 = is1 * ed1 - is1;
    gd1 = is1 * ed1 / vt1;
    id1eq = id1 - gd1 * vd1;
    
    vd2 = v3 - 0;
    ed2 = exp(vd2 / vt2);
    id2 = is2 * ed2 - is2;
    gd2 = is2 * ed2 / vt2;
    id2eq = id2 - gd2 * vd2;
    
    v2 = (gr1 * v1 + gc1 * v3 - ic1eq) / (gc1 + gr1);
    v3 = (gc1*v2 + ic1eq + ic2eq + id1eq - id2eq) / (gc1 + gc2 + gd1 + gd2 + gr2);
    
    if(abs(vlast2 - v2) <= cov && abs(vlast3 - v3) <= cov)
        if (iter < j)
          iter = j;
        endif
      break;
    endif
    
    vlast2 = v2;
    vlast3 = v3;
    
  endfor
    
    Vout(i) = v3;
  
  ic1eq += minv * (gc1 * (v3 - v2) - ic1eq);  
  ic2eq += minv * (gc2 * (v3 - 0) - ic2eq); 
    
  Vout(i) = v3;
  
endfor

disp(iter);
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



