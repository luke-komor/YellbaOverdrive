clc
pkg load signal

fs = 44100;
fc = 500;
order = 3;

[b, a] = butter(order, fc / fs, "high");


# generate cpp 

printf("// Generated for fs = %d, fc = %d, order = %d, highpass \n", fs, fc, order);

for i = 1 : length(a)
    printf("const float A%d = %.30f;\n", i - 1, a(i));
endfor

printf("\n");

for i = 1 : length(b)
    printf("const float B%d = %.30f;\n", i - 1, b(i));
endfor