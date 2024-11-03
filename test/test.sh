#!/bin/bash

# Define the number of tests
iterations=500
failureCount=0

# Path to your compiled C executable (update this path if necessary)
exePath="./a.exe"  # Assuming you named your compiled C executable as prime_generator

# Compile the C program if needed (uncomment and modify as necessary)
# gcc -o prime_generator prime_generator.c -lcrypto 

for ((i=1; i<=iterations; i++))
do
    # Run the executable and capture the output
    output=$($exePath)

    # Check for failure message
    if [[ $output == *"Prime not found within 100 attempts"* ]]; then
        ((failureCount++))
    fi

    # Print progress every 1000 iterations
    if (( $i % 1000 == 0 )); then
        echo "$i tests completed..."
    fi
done

# Print the final result
echo "Total Failures: $failureCount out of $iterations attempts"
