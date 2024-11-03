#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/rand.h>
#include <stdbool.h>

#define PRIME_ATTEMPTS 50000000  // Max attempts to find a prime

bool miller_rabin_test(unsigned long long n, int iterations);
unsigned long long generate_random_odd_number(unsigned long long min, unsigned long long max);

// Function to generate a single prime number within specified attempts
unsigned long long generate_prime_number(unsigned long long min, unsigned long long max) {
    unsigned long long candidate;

    for (int attempts = 0; attempts < PRIME_ATTEMPTS; attempts++) {
        candidate = generate_random_odd_number(min, max);
        
        if (candidate != (unsigned long long)-1 && miller_rabin_test(candidate, 5)) {  // 5 iterations for high confidence
            return candidate;  // Found a prime
        }
    }
    
    printf("Prime not found within %d attempts.\n", PRIME_ATTEMPTS);
    return -1;  // Indicate failure
}

// Miller-Rabin Test for Primality (probabilistic)
bool miller_rabin_test(unsigned long long n, int iterations) {
    if (n < 2) return false;                // 0 and 1 are not prime
    if (n == 2 || n == 3) return true;      // 2 and 3 are prime
    if (n % 2 == 0) return false;           // Exclude even numbers

    unsigned long long s = 0;
    unsigned long long d = n - 1;
    
    // Express n - 1 as 2^s * d
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }

    for (int i = 0; i < iterations; i++) {
        unsigned long long a = 2 + rand() % (n - 3);  // Random base in [2, n-2]
        unsigned long long x = (unsigned long long)pow(a, d) % n;
        
        if (x == 1 || x == n - 1) continue;

        bool continue_outer = false;
        for (unsigned long long r = 0; r < s - 1; r++) {
            x = (x * x) % n;
            if (x == n - 1) {
                continue_outer = true;
                break;
            }
        }
        
        if (continue_outer) continue;
        
        return false;  // Composite number
    }

    return true;  // Probably prime
}

// Generate a random odd number between min and max
unsigned long long generate_random_odd_number(unsigned long long min, unsigned long long max) {
    unsigned char buffer[8]; // Use 8 bytes for 64-bit random numbers
    if (RAND_bytes(buffer, sizeof(buffer)) != 1) {
        fprintf(stderr, "Error generating random bytes.\n");
        return -1;
    }

    unsigned long long random_value = *(unsigned long long *)buffer; // Convert to unsigned long long
    random_value = (random_value % (max - min + 1)) + min; // Scale to desired range

    // Ensure the number is odd
    if (random_value % 2 == 0) {
        random_value += 1;
    }

    // Ensure the number is within the bounds
    if (random_value > max) {
        return max;  // Cap the maximum
    }
    return random_value;
}

int main() {
    unsigned long long min = 168; // Minimum value for prime generation
    unsigned long long max = 5000000000; // Maximum value for prime generation (up to 10 digits)
    unsigned long long prime = generate_prime_number(min, max);
    
    if (prime != (unsigned long long)-1) {
        printf("Generated Prime: %llu\n", prime);
    } else {
        printf("Failed to generate a prime number.\n");
    }
    
    return 0;
}
