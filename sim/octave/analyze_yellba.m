clear; clc; close('all');

pkg load signal
pkg load ltfat

[dist1, fs] = audioread("od_bare_tmin_impulse.wav");
#d1 = normalize(dist1(:, 1), 'peak');

[dist2, fs] = audioread("od_bare_thalf_impulse.wav");
#d2 = normalize(dist2(:, 1), 'peak');

[dist3, fs] = audioread("od_bare_tmax_impulse.wav");
#d3 = normalize(dist3(:, 1), 'peak');

N = length(dist1);



f = (-N/2:1:N/2-1)*fs/N;

hold on;
semilogx(f, abs(fftshift(fft(dist1))))
#semilogx(f, abs(fftshift(fft(dist2))))
semilogx(f, abs(fftshift(fft(dist3))))

#legend('min', 'half', 'max')

xlim([0 fs/2]);
grid on;

