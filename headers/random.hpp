#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

class Random {
    public:
        Random() {
            srand(time(0));
        }

        // generates a random int in [0, maxRange)
        int random(int maxRange) {
            return rand() % maxRange;
        }

        // generates a random int in [min, max)
        int between(int min, int max) {
            // validate parameters
            if (min > max)
            {
                int temp = min;
                min = max;
                max = temp;
            }
            return (rand() % (max - min)) + min;
        }

        // generates a set of random ints in [0, maxRange).
        std::vector<int> randomSet(int maxRange, int setSize) {
            std::vector<int> set;
            for (int i = 0; i < setSize; i++) {
                set.push_back(rand() % maxRange);
            }
            return set;
        }

    private:
};