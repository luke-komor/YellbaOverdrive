clc
pkg load signal

fs = 44100;

Sb = [1 10 0];
Sa = [1 110 1000];

[Zb, Za] = bilinear(Sb, Sa, 1/fs);

[h, w] = freqz(Zb, Za, fs);

semilogx(w * (fs/2), abs(h))
grid


