clear; clc; close('all');
pkg load signal
pkg load ltfat

graphics_toolkit('gnuplot')

# Parameters
D = 1;
V0 = 0;
fs = 44.1e3;
f = 1e3;
Ain = 1;

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


##>> solve(eq2, v3)
##ans = (sym)
##
##  gc1*v2 - gr2*v2 + 2*ic1eq + ic2eq
##  ---------------------------------
##              gc1 - gc2
##
##>> solve(eq1, v2)
##ans = (sym)
##
##  gc1*v3 + gr1*v1 - ic1eq
##  -----------------------
##         gc1 + gr1
         
for i = 1:N
  
  v1 = Vin(i);
  
  #v3 = (gc1 * ic2eq + gr1 * (ic1eq + ic2eq + gc1 * v1)) / (gr1 * (gc2 + gr2) + gc1 *(gc2 + gr1 + gr2));

 
  v3 = (gc1*gr1*v1 + gc1*ic2eq - gr1*gr2*v1 + gr1*ic1eq + gr1*ic2eq + gr2*ic1eq) / (gc1*gc2 + gc1*gr1 + gc1*gr2 + gc2*gr1);
  #v3 = (-gc1*gr1*v1 - gc1*ic1eq - gc1*ic2eq + gr1*gr2*v1 - 2*gr1*ic1eq - gr1*ic2eq - gr2*ic1eq)/(gc1*gc2 - gc1*gr1 - gc1*gr2 + gc2*gr1);

  v2 = (gr1 * v1 + gc1 * v3 - ic1eq) / (gc1 + gr1);
  ic1eq += minv * (gc1 * (v3 - v2) - ic1eq);  
  ic2eq += minv * (gc2 * (v3 - 0) - ic2eq); 
    
  Vout(i) = v3;
  
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



