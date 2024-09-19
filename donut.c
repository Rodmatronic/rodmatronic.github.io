#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define RESET "\x1b[0m"

// Define color backgrounds
const char* colors[] = {
    "\x1b[48;5;234m", // Darkest grey
    "\x1b[48;5;235m",
    "\x1b[48;5;236m",
    "\x1b[48;5;238m",
    "\x1b[48;5;239m",
    "\x1b[48;5;240m", // Near white
    "\x1b[48;5;238m",
    "\x1b[48;5;249m", // Near white
    "\x1b[48;5;240m",
    "\x1b[48;5;241m",
    "\x1b[48;5;242m",
    "\x1b[48;5;243m",
    "\x1b[48;5;244m",
    "\x1b[48;5;30m", // Near white
    "\x1b[48;5;0m",
    "\x1b[48;5;0m",
    "\x1b[48;5;0m",
    "\x1b[48;5;30m", // Near white
};

int main() {
    float A = 0, B = 0, i, j, z[1760];
    char b[1760];

    printf("\x1b[?25l"); // Hide cursor

    while (1) {
        clock_t start = clock(); // Start the timer

        memset(b, 32, 1760); // Fill b with spaces
        memset(z, 0, 7040);  // Fill z with zeros

        for (j = 0; j < 6.28; j += 0.07) {
            for (i = 0; i < 6.28; i += 0.02) {
                float c = sin(i), d = cos(j), e = sin(A), f = sin(j),
                      g = cos(A), h = d + 2, D = 1 / (c * h * e + f * g + 5),
                      l = cos(i), m = cos(B), n = sin(B), t = c * h * g - f * e;

                int x = 40 + 30 * D * (l * h * m - t * n),
                    y = 12 + 15 * D * (l * h * n + t * m),
                    o = x + 80 * y,
                    N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);

                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    // Use color instead of characters based on depth
                    b[o] = N > 0 ? N : 0;
                }
            }
        }

        printf("\x1b[H"); // Move cursor to top left

        // Print each "pixel" with color instead of symbols
        for (int k = 0; k < 1760; k++) {
            if (k % 80 == 0) {
                printf("\n");
            } else {
                int colorIndex = b[k] / 2; // Scale to fit color range
                if (colorIndex < 0) colorIndex = 0;
                if (colorIndex > 17) colorIndex = 17; // Cap at max color
                printf("%s ", colors[colorIndex]); // Print colored background
            }
        }

        A += 0.04;
        B += 0.02;

        clock_t end = clock(); // End the timer
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        double fps = 1.0 / time_spent;

        printf("\nFrame time: %.5f seconds\n", time_spent); // Print time taken per frame    }
    }
    return 0;
}