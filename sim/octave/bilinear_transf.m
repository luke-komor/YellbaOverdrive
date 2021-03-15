clc
pkg load signal

fs = 44100;


# input buffer
# 1 * s^3 + 10 * s^2
# -------------------------------------------------------------------------------
# 1.00196 * s^3 + 112.176 * s^2 + 1217.65 * s + 1960.78


Sb = [1 10 0 0];
Sa = [1.00196 112.176 1217.65 1960.78];

[Zb, Za] = bilinear(Sb, Sa, 1/fs);

Za
Zb

[h, w] = freqz(Zb, Za, fs);

semilogx (w * (fs/2), abs(h))
grid

