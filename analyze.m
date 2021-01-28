pkg load signal
pkg load ltfat

[dist, fs] = audioread("one_tone.wav");
d1 = normalize(dist(:, 1), 'peak');
d2 = normalize(dist(:, 2), 'peak');
N = length(d1);

step=ceil(20*fs/1000);    # one spectral slice every 20 ms
window=ceil(100*fs/1000); # 100 ms data window

f = (0:1:N-1)*fs/N;

figure(1, 'position',[1000, 100, 1500, 1800]);
subplot(2, 1, 1);
plot(f, abs(fft(d1)))
xlim([0 2000]);
title('GR');

subplot(2, 1, 2);
plot(f, abs(fft(d2)))
title('BIAS');
xlim([0 2000]);