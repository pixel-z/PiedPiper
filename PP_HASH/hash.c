#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LONG ((((1UL << 63) - 1) << 1) + 1)
typedef unsigned long ul;
#define RULE 30

ul W[80];
ul h[8] = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
           0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};

ul K[80] = {0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
            0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
            0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
            0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
            0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
            0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
            0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
            0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
            0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
            0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
            0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
            0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
            0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
            0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
            0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
            0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

ul majority(ul a, ul b, ul c) // O(1)
{
    return ((a & b) ^ (b & c) ^ (c & a));
}

ul rotate(ul n, int bits) // O(1)
{
    ul tmp = (1L << bits) - 1;
    // printf("%lx ", tmp);
    tmp = (tmp & n);
    // printf("%lx ", tmp);
    n >>= bits;
    // printf("%lx ", n);
    tmp <<= (64 - bits);
    // printf("%lx ", tmp);
    n |= tmp;
    // printf("%lx ", n);
    return n;
}

ul conditional(ul a, ul b, ul c) // O(1)
{
    return ((a & b) ^ ((~a) & c));
}

ul rot_function1(ul a)
{
    return (rotate(a, 28) ^ rotate(a, 34) ^ rotate(a, 39));
}
ul rot_function2(ul a)
{
    return (rotate(a, 14) ^ rotate(a, 18) ^ rotate(a, 41));
}

ul overflow_addition(ul a, ul b)
{
    return ((a + b) % MAX_LONG);
}

ul cell_automaton(ul a, ul b, ul c)
{
    ul rv = 0;
    int x = 0;
    while (x < 64)
    {
        int num = ((a & 1) << 2) | ((b & 1) << 1) | (c & 1);
        rv |= (((RULE >> num) & 1) << x);
        a >>= 1;
        b >>= 1;
        c >>= 1;
        x++;
    }
    return rv;
}

ul *round(ul *ar, int x) // return x and y as 2-D array
{

    ul mixer_2 = cell_automaton(conditional(ar[4], ar[5], ar[6]), rot_function2(ar[4]), cell_automaton(ar[7], W[x], K[x]));
    ul *ar2 = (ul *)malloc(2 * sizeof(ul));
    ar2[0] = cell_automaton(majority(ar[0], ar[1], ar[2]), rot_function1(ar[0]), mixer_2);
    ar2[1] = overflow_addition(mixer_2, ar[3]);
    // printf(" Round %d   X : %lx  Y : %lx\n", x, ar2[0], ar2[1]);
    ar[7] = ar[6];
    ar[6] = ar[5];
    ar[5] = ar[4];
    ar[4] = ar2[1];
    ar[2] = ar[3];
    ar[1] = ar[2];
    ar[0] = ar[1];
    ar[0] = ar2[0];
}

void hash(ul *hashArr)
{
    for (int x = 0; x < 80; x++)
    {
        round(hashArr, x);
    }
    for (int x = 0; x < 8; x++)
    {
        hashArr[x] = overflow_addition(hashArr[x], h[x]);
    }
}

void duplicate_W()
{
    for (int i = 16; i < 79; i++)
    {
        ul s0 = (rotate(W[i - 15], 1) ^ rotate(W[i - 15], 8) ^ rotate(W[i - 15], 7));
        ul s1 = (rotate(W[i - 2], 19) ^ rotate(W[i - 2], 61) ^ rotate(W[i - 2], 6));
        W[i] = overflow_addition(overflow_addition(W[i - 16], s0), overflow_addition(W[i - 7], s1));
    }
}

void displayHash(ul *ar, int l)
{
    for (int x = 0; x < l; x++)
    {
        printf("%lx", ar[x]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) // for now assume size of string is less than 1024-64 bytes
{
    // ul a = atol(argv[1]);
    // printf("\n[D]%lx Rotate right op\n", rotate(a, 3));
    // exit(0);
    ul *ar = (ul *)malloc(sizeof(ul) * 8);
    int app_l = 1024;
    char argument[1024];
    for (int x = 0; x < strlen(argv[1]); x++)
    {
        argument[x] = argv[1][x];
    }
    for (int x = strlen(argv[1]); x < app_l; x++)
    {
        argument[x] = 0x00;
    }

    int max_x = 0;
    for (int x = 0; x < app_l; x += 8)
    {
        W[x / 8] = 0x00;
        for (int z = x; z < x + 8; z++)
        {
            W[x / 8] <<= 8;
            W[x / 8] |= argument[z];
        }
        max_x = x / 8;
    }
    // for (int x = max_x + 1; x <= 15; x++)
    // {
    //     W[x] = (ul)0x0;
    // }
    for (int x = 0; x < 8; x++)
    {
        ar[x] = h[x];
    }
    W[15] = (ul)strlen(argv[1]);
    duplicate_W();
    hash(ar);
    displayHash(ar, 8);
}