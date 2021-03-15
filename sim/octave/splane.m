clear; clc; close('all');
pkg load signal

# comparision zplane and s plane

# simple low pass RC filter

s = tf('s');
Hs = 1 / (s + 1);

Sb = [0 1];
Sa = [1 1];

[sz, sp] = tf2zp(Sb, Sa)

[Zb, Za] = bilinear(Sb, Sa, 2);




subplot(2, 1, 1)
title ("S - plane")
pzmap(Hs)

subplot(2, 1, 2)
title("Z- plane")
zplane(Zb, Za)
