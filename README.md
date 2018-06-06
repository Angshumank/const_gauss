This is a implementation of the constant-time discrete Gaussian sampler as appeared in  IEEE Transactions on Computers (https://ieeexplore.ieee.org/document/8314133/).

Three files are included in this folder sampler-128.c, sampler-96.c and sampler-64.c with 128, 64 and 96 bits of precision cut.

The standard deviation of the paper is fixed at apprpximately 6.15543.

SHAKE-128 hase been used for generating pseudorandom numbers.

To run open a terminal and enter the following :

> gcc -Wall -Wextra -O3 -fomit-frame-pointer -march=native -o sample sampler-XX.c (replace XX with 64, 96 or 128).
> ./sample
