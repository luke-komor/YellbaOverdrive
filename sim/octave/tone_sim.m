
clear; clc; close('all');
pkg load signal
pkg load ltfat

% function [x1, x2] = solveQuadratic(a, b, c)

%     x1 = (-b - sqrt(b^2 - 4 * a *c)) / (2 * a);
%     x2 = (-b + sqrt(b^2 - 4 * a *c)) / (2 * a);
% end

% function [ans] = tG(x, T)
%     ans = (2-x*T)/T;
% end

% function [ans] = tZP(x, T)
%     ans = (2 + x * T) / (2 - x * T);
% end

% function [bz, az] = bt1(b, a, T)

%     % z²(a0 + a1*k + a2*k²) + z(2*a2*k² - 2*a0) + (a0 - a1*k + a2*k²)
%     % ---------------------------------------------------------------
%     % z²(b0 + b1*k + b2*k²) + z(2*b2*k² - 2*b0) + (b0 - b1*k + b2*k²)

%     b0 = b(3);
%     b1 = b(2);

%     a0 = a(3);
%     a1 = a(2);
%     a2 = a(1);

%     sz =  -b0 / b1;
%     [sp1, sp2] = solveQuadratic(a2, a1, a0);
%     sg = 1;

%     zg = real(sg * tG(sz, T) / (tG(sp1, T) * tG(sp2, T)));

%     zz1 = tZP(sz, T);
%     zz2 = -1;
%     zp1 = tZP(sp1, T);
%     zp2 = tZP(sp2, T);

%     bz2 = 1*zg;
%     bz1 = (- 1 * (zz1+zz2)) * zg;
%     bz0 = (zz1 * zz2) * zg;

%     az2 = 1;
%     az1 = -1 * (zp1 + zp2);
%     az0 = zp1 * zp2;

%     bz = [bz2 bz1 bz0];
%     az = [az2 az1 az0];

% end

function [bz, az] = bt(b, a, T)

    b0 = b(3);
    b1 = b(2);

    a0 = a(3);
    a1 = a(2);
    a2 = a(1);

    c = 2 / T;

    az0 = a0 + a1*c + a2*c^2;
    
    az2 = a0 - a1 * c + a2 * c^2;
    az1 = 2*a0 - 2 * a2 * c^2;

    bz2 = b0 - b1 * c;
    bz1 = 2*b0;
    bz0 = b0 + b1*c;

    az2 /= az0;
    az1 /= az0;

    bz2 /= az0;
    bz1 /= az0;
    bz0 /= az0;

    az0 = 1;

    bz = [bz2 bz1 bz0];
    az = [az2 az1 az0];

end

function [Ta, Tb] = calcTone(T)

    base = 20e3;
    Ta = base * T;
    Tb = base - Ta;

end

function [h, w] = calcFilter(fs, T)

   # tone stage tf
    % ( C2 * Ta * Tb * V1 + C2 * R4 * Ta * V1 + C2 * R4 * Tb * V1 + C2 * R1 * Ta * V1 ) * s + ( Ta * V1 + Tb * V1 )
    % ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    % ( C1 * C2 * R2 * Ta * Tb + C1 * C2 * R2 * R4 * Ta + C1 * C2 * R2 * R4 * Tb ) * s^2 + ( C2 * Ta * Tb + C2 * R4 * Ta + C2 * R4 * Tb + C1 * R2 * Ta + C1 * R2 * Tb + C2 * R2 * Tb ) * s + ( Ta + Tb )

    R1 = 1e3;
    R2 = 1e3;
    R3 = 10e3;
    R4 = 220;

    C1 = 0.22e-6;
    C2 = 0.22e-6;

    [Ta, Tb] = calcTone(T);

    Sb1 = C2 * Ta * Tb + C2 * R4 * Ta + C2 * R4 * Tb + C2 * R1 * Ta;
    Sb0 = Ta + Tb;

    Sb = [0 Sb1 Sb0];

    Sa2 = C1 * C2 * R2 * Ta * Tb + C1 * C2 * R2 * R4 * Ta + C1 * C2 * R2 * R4 * Tb; 
    Sa1 = C2 * Ta * Tb + C2 * R4 * Ta + C2 * R4 * Tb + C1 * R2 * Ta + C1 * R2 * Tb + C2 * R2 * Tb; 
    Sa0 = Ta + Tb;

    Sa = [Sa2 Sa1 Sa0];

   # [b, a] = bilinear(Sb, Sa, 1/fs)
    [b, a] = bt(Sb, Sa, 1/fs);

    for i = 1 : length(a)
        printf("a%d = %.30f;\n", i - 1, a(i));
    endfor

    printf("\n");

    for i = 1 : length(b)

        printf("b%d = %.30f;\n", i - 1, b(i));
    endfor

    h = b;
    w = a; 
  
    #[h, w] = freqz(b, a);

end

function y = filt(b, a, x)

    b0 = single(b(3));
    b1 = single(b(2));
    b2 = single(b(1));

    a0 = single(a(3));
    a1 = single(a(2));
    a2 =single(a(1));

    y = zeros(1, length(x));
    lv1 = single(0);
    lv2 = single(0);

    for i=1:length(x)

        input = single(x(i));
        output = single((input * b0) + lv1);
        y(i) = output;

        lv1 = (input * b1) - (output* a1) + lv2;
        lv2 = (input * b2) - (output* a2);
    end

end


#steps = [0.0, 0.25, 0.5, 0.75, 1.0];
steps = [0.0, 0.5, 0.98,  1.0]
fs = 44.1e3
N = fs;

#sig = [1, zeros(1, N-1)];

sig = noise(N);
f = (-N/2:1:N/2-1)*fs/N;
hold on;

for i = 1:length(steps)

    [b, a] = calcFilter(fs, steps(i));
    #y = filter(b, a, sig);
    y = filt(b, a, sig);
    #audiowrite(sprintf("processed_noise_%d.wav", steps(i)), y, fs);

    semilogx(f, abs(fftshift(fft(y))))
    grid on;
end



% b = [0 2 1];
% a = [2 1 3];

%     [b, a] = bilinear(b, a, 1/fs)
%     [b, a] = bt(b, a, 1/fs)