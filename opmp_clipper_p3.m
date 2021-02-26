clear; clc; close('all');
pkg load signal
pkg load ltfat

graphics_toolkit('gnuplot')

# Parameters
D = 1;
V0 = 0;
fs = 16 * 44.1e3;
f = 4e2;
Ain = 1e-2;

Is = 1e-12;
is1 = Is * 2;
is2 = Is;

Vt = 26e-3;
vt1 = Vt * 0.9;
vt2 = Vt;

gr1 = 1/4.7e3;
gr2 = 1/500e3;

m = 0.5;
minv = 1/m;

gc1 = 60e-9 * fs * minv;
gc2 = 50e-12 * fs * minv;

ic1eq = 0;
ic2eq = 0;

t = 0:(1 / fs):(8 / f);
N = length(t);

Vin = Ain * sin(2 * pi * f * t); # + Ain * sin(2 * pi * 3 * f * t);
Vout = zeros(1, N);

max_iter = 400;
cov = 0.001;

iter = 0;

for i = 1:N
  
  v1 = Vin(i);
  v2 = V0;
  v3 = V0;
  
  vlast2 = 0;
  vlast3 = 0;
  
  # diodes
  for j = 0:max_iter - 1
    
    vd1 = v3 - v1;
    ed1 = exp(vd1 / vt1);
    id1 = is1 * ed1 - is1;
    gd1 = is1 * ed1 / vt1;
    id1eq = id1 - gd1 * vd1;
    
    vd2 = v1 - v3;
    ed2 = exp(vd2 / vt2);
    id2 = is2 * ed2 - is2;
    gd2 = is2 * ed2 / vt2;
    id2eq = id2 - gd2 * vd2;
 
    v2 = (gc1 * v1 + ic1eq) / (gr1 + gc1);
    v3 = (v1 * (gr2 + gc2 + gd1 + gd2) + gr1 * v2 - ic2eq -id1eq + id2eq) / (gr2 + gc2 + gd1 + gd2);

    if(abs(vlast2 - v2) <= cov && abs(vlast3 - v3) <= cov)
        if (iter < j)
          iter = j;
        endif
      break;
    endif
    
    vlast2 = v2;
    vlast3 = v3;
    
  endfor
    
    ic1eq += minv * (gc1 * (v2 - v1) - ic1eq);  
    ic2eq += minv * (gc2 * (v1 - v3) - ic2eq); 
        
    Vout(i) = v3;
  
endfor
figure(1, 'position',[50,200,1500,1000]);
disp(iter);

subplot(2, 1, 1)
plot(t, Vout)

grid on;

fa = fs*(0:(N/2))/N;
Y = fft(Vout);
P2 = abs(Y/N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2*P1(2:end-1);

subplot(2, 1, 2)
plot(fa, mag2db(P1))
xlim([0 10e3])
grid on;



