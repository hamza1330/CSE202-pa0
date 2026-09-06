#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
// Union to store 4 bytes as an array of bytes, an unsigned, signed, or float number
union value{
    unsigned uval;
    int sval;
    float fval;
    unsigned char bytes[4];
};
// reads 8 hex characters from string input and stores it in the union v
// returns -1 if the hexadecimal number is invalid, 0 otherwise
int read_hex(union value *v, char *input);

// converts the ASCII hex character c to binary
// returns the hex value of c if c is a valid hex digit, -1 otherwise
char hexDigit(char c);

// returns true if x has any even bit equal to 1, 0 otherwise
int any_even_one(unsigned x);
// returns a mask indicating the position of the left most one in x
int leftmost_one(unsigned x);
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n);
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n);
// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y);
// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f);
// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f);

int main(int argc, char** argv)
{
    if(argc != 3 && argc != 4)
    {
        printf("Invalid number of arguments\n");
        exit(0);
    }
    union value v;
    if(read_hex(&v, argv[2]) == -1)
    {
        printf("Invalid hex value\n");
        exit(0);
    }
    // Check which operation to perform based on the first argument
    if (strcmp(argv[1], "even") == 0) // any_even_one
    {
        printf("%s\n", any_even_one(v.uval)? "True" : "False");
    }
    else if (strcmp(argv[1], "left") == 0) // leftmost_one
    {
        printf("%08x\n", leftmost_one(v.uval));
    }
    else if (strcmp(argv[1], "lrotate") == 0) // rotate_left
    {
        if(argc != 4)
        {
            printf("Invalid number of arguments");
            exit(0);
        }
        int n = atoi(argv[3]);
        if(n < 0 || n > 31)
        {
            printf("Invalid number of shift positions");
            exit(0);
        }
        printf("%08x\n", rotate_left(v.uval, n));
    }
    else if (strcmp(argv[1], "rrotate") == 0) // rotate_right
    {
        if(argc != 4)
        {
            printf("Invalid number of arguments");
            exit(0);
        }
        int n = atoi(argv[3]);
        if(n < 0 || n > 31)
        {
            printf("Invalid number of shift positions");
            exit(0);
        }
        printf("%08x\n", rotate_right(v.uval, n));
    }
    else if (strcmp(argv[1], "saturate") == 0) // saturating_add
    {
        if(argc != 4)
        {
            printf("Invalid number of arguments");
            exit(0);
        }
        union value y;
        if(read_hex(&y, argv[3]) == -1)
        {
            printf("Invalid hex value");
            exit(0);
        }
        int out = saturating_add(v.sval, y.sval);
        printf("%08x %d\n", out, out);
    }
    else if (strcmp(argv[1], "twice") == 0) // float_twice
    {
        unsigned out = float_twice(v.uval);
        printf("%08x %e\n", out, *(float*)&out);
    }
    else if (strcmp(argv[1], "half") == 0) // float_half
    {
        unsigned out = float_half(v.uval);
        printf("%08x %e\n", out, *(float*)&out);
    }
    else 
    {
        printf("Invalid operation\n");
    }
    return 0;
}

int read_hex(union value *v, char *input)
{
    if(strlen(input) != 8)
    {
        return -1;
    }
    for (int i = 0; i < 4; i++)
    {
        char high = hexDigit(input[2 * i]);
        char low  = hexDigit(input[2 * i + 1]);

        if (high == -1 || low == -1)
            return -1;

        v->bytes[3 - i] = (high << 4) | low;
    }
    return 0;
}

char hexDigit(char c)
{
    if(c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if(c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if(c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
}

int any_even_one(unsigned x)
{
    // 0x55555555 is a mask with all even bits set to 1
    unsigned output = x & 0x55555555;
    output = output && 1;
    return output;
}

int leftmost_one(unsigned x)
{
    if (x == 0) 
    {
        return 0;
    }
    // Start with the leftmost bit set
    unsigned leftmost = 0x80000000;
    while ((x & leftmost) == 0) 
    {
        // Shift right until we find the leftmost one
        leftmost = leftmost >> 1; 
    }
    // Return the position of the leftmost one
    return leftmost; 
}

unsigned rotate_left(unsigned x, int n)
{
    // Shift x to the left by n positions
    unsigned left_shifted = x << n;
    // Get the bits that were shifted out of the left side
    unsigned wrapped_bits = x >> (32 - n);
    // Combine the left shifted value with the wrapped bits
    return left_shifted | wrapped_bits;
}

unsigned rotate_right(unsigned x, int n)
{
    // Shift x to the right by n positions
    unsigned right_shifted = x >> n;
    // Get the bits that were shifted out of the right side
    unsigned wrapped_bits = x << (32 - n);
    // Combine the right shifted value with the wrapped bits
    return right_shifted | wrapped_bits;
}

int saturating_add(int x, int y)
{
    int sum = x + y;
    // Get the sign bits of x, y, and sum
    unsigned x_sign_check = ((unsigned)x) >> 31;
    unsigned y_sign_check = ((unsigned)y) >> 31;
    unsigned sum_sign_check = ((unsigned)sum) >> 31;

    // Check for overflow: if x and y have the same sign but sum has a different sign, overflow occurred
    if (x_sign_check == y_sign_check && x_sign_check != sum_sign_check) 
    {
        if (x_sign_check == 0) 
        {
            return INT_MAX;
        } 
        else 
        {
            return INT_MIN;
        }
    }
    return sum;
}

unsigned float_twice(unsigned f)
{
    unsigned sign_mask = 0x80000000;
    unsigned exponent_mask = 0x7F800000;
    unsigned fraction_mask = 0x007FFFFF;

    unsigned sign = f & sign_mask;
    unsigned exponent = (f & exponent_mask) >> 23;
    unsigned fraction = f & fraction_mask;

    if (exponent == 255) 
    {
        // f is NaN or infinity, return f
        return f;
    } 
    else if (exponent == 0) 
    {
        // f is denormalized, multiply fraction by 2
        fraction = fraction << 1;
        // If the fraction overflows, we need to normalize it
        if (fraction & 0x00800000) 
        {
            exponent = 1;
            fraction = fraction & fraction_mask; // Clear the overflow bit
        }
    } 
    else 
    {
        // f is normalized, increment exponent by 1
        exponent += 1;
        // If exponent overflows, return infinity
        if (exponent == 255) 
        {
            return sign | exponent_mask; // Return infinity with the same sign
        }
    }
    return sign | (exponent << 23) | fraction;
}

unsigned float_half(unsigned f)
{
    unsigned sign_mask = 0x80000000;
    unsigned exponent_mask = 0x7F800000;
    unsigned fraction_mask = 0x007FFFFF;

    unsigned sign = f & sign_mask;
    unsigned exponent = (f & exponent_mask) >> 23;
    unsigned fraction = f & fraction_mask;

    if (exponent == 255) 
    {
        // f is NaN or infinity, return f
        return f;
    } 
    else if (exponent == 0) 
    {
        // f is denormalized, divide fraction by 2
        fraction = fraction >> 1;
        // If the least significant bit of the original fraction was 1, we need to round up
        if (f & 1) 
        {
            fraction += 1; // Round up
        }
    } 
    else if (exponent == 1) 
    {
        // f is normalized but will become denormalized after halving
        // We need to shift the fraction and set the exponent to 0
        fraction = (fraction | 0x00800000) >> 1; // Add the implicit leading 1 and then shift
        exponent = 0; // Set exponent to denormalized
    } 
    else 
    {
        // f is normalized, decrement exponent by 1
        exponent -= 1;
    }
    return sign | (exponent << 23) | fraction;
}
