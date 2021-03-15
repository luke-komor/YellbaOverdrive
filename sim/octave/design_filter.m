clear; clc; close('all');

pkg load signal 

fs = 44100;


function [b, a] = makeHighPass(frequency, sampleRate) 

    Q = 0.70710678118654752440;
    n = tan (pi * frequency / sampleRate);

    nSquared = n * n;
    invQ = 1 / Q;
    c1 = 1 / (1 + invQ * n + nSquared);

    b0 = c1;
    b1 = c1 * -2;
    b2 =  c1;

    a0 = 1;
    a1 = c1 * 2 * (nSquared - 1);
    a2 =  c1 * (1 - invQ * n + nSquared);

    a = [a2 a1 a0];
    b = [b2 b1 b0];

end


#[b,a] = butter(order,1000/(sr/2),'low');

# 1 * s^3 + 10 * s^2
# -------------------------------------------------------------------------------
# 1.00196 * s^3 + 112.176 * s^2 + 1217.65 * s + 1960.78

Zb = [1 10 0 0 ]
Za = [1.00196 112.176 1217.65 1960.78]

[b, a] = bilinear(Zb, Za, 1/fs);

[h, f] = freqz(b, a, 10000);

fv = fs / 2 * f / pi;
hlog = 20 * log10(abs(h));

plot(fv, hlog)


#figure 
#[b, a] = makeHighPass(fs, 50);
#[h, f] = freqz(b, a);

% [sos] = tf2sos(b, a);

% x = [zeros(10, 1) ; 1; zeros(200, 1)]; %impulse


% % all in one
% Y = filter(b, a, x);

% % cascaded biquads
% Z = x;

% for nn = 1:size(sos, 1);
%     Z = filter(sos(nn, 1:3),sos(nn, 4:6), Z );
% end


% cla; 
% plot(Y, 'k'); 
% hold on; 
% plot(Z,':r'); 
% hold off