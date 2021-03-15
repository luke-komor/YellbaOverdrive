clear; clc; close('all');
pkg load symbolic;

syms v1 v2 v3 gr1 gr2 gc1 gc2 ic1eq ic2eq Ia Ib Ic gd1 gd2 id2eq id1eq

## V2
##   gc1*v3 + gr1*v1 - ic1eq
##  -----------------------
##         gc1 + gr1

eq1 = gr1 * (v2 - v1) - (gc1 * (v3 - v2) - ic1eq) == 0;
eq2 = gc1 * (v3 - ((gc1*v3 + gr1*v1 - ic1eq)/(gc1 + gr1))) - ic1eq + gc2 * (v3 - 0) - ic2eq + gr2 * (((gc1*v3 + gr1*v1 - ic1eq)/(gc1 + gr1)) - 0) == 0;

solve([eq1, eq2], [v3, v2]);

# diode clipper all 
eq1 = gr1 * (v2 - v1) - (gc1 * (v3 - v2) - ic1eq) == 0;
eq2 = gc1 * (v3 - v2) - ic1eq + gc2*(v3 - 0) - ic2eq + gr2 * (v3 - 0) - gd1 * (0 - v3) - id1eq + gd2 * (v3 -0) + id2eq == 0;

eq21 = gc1 * (v3 - ((gc1*v3 + gr1*v1 - ic1eq)/( gc1 + gr1))) - ic1eq + gc2*(v3 - 0) - ic2eq + gr2 * (v3 - 0) - gd1 * (0 - v3) - id1eq + gd2 * (v3 -0) + id2eq == 0;


## V3
##  gc1*v2 + ic1eq + ic2eq + id1eq - id2eq
##  --------------------------------------
##       gc1 + gc2 + gd1 + gd2 + gr2