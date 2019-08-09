#include <Arduino.h>

#include "pinkNoise.h"

#define PINK_RANDOM_BITS       (24)
#define PINK_RANDOM_SHIFT      ((sizeof(long)*8)-PINK_RANDOM_BITS)


  PinkNoise::  PinkNoise(int numRows ) {
      int i;
      long pmax;
      pink_Index = 0;
      pink_IndexMask = (1 << numRows) - 1;
      /* Calculate maximum possible signed random value. Extra 1 for white noise always added. */
      pmax = (numRows + 1) * (1 << (PINK_RANDOM_BITS - 1));
      pink_Scalar = 1.0f / pmax;
      /* Initialize rows. */
      for ( i = 0; i < numRows; i++ )
        pink_Rows[i] = 0;
      pink_RunningSum = 0;
    }

    float PinkNoise:: generate() {
      unsigned long newRandom;
      long sum;
      float output;

      /* Increment and mask index. */
      pink_Index = (pink_Index + 1) &pink_IndexMask;

      /* If index is zero, don't update any random values. */
      if ( pink_Index != 0 )
      {
        /* Determine how many trailing zeros in PinkIndex. */
        /* This algorithm will hang if n==0 so test first. */
        int numZeros = 0;
        int n = pink_Index;
        while ( (n & 1) == 0 )
        {
          n = n >> 1;
          numZeros++;
        }

        /* Replace the indexed ROWS random value.
           Subtract and add back to RunningSum instead of adding all the random
           values together. Only one changes each time.
        */
        pink_RunningSum -= pink_Rows[numZeros];
        newRandom = ((unsigned long)rand()) >> PINK_RANDOM_SHIFT;
        pink_RunningSum += newRandom;
        pink_Rows[numZeros] = newRandom;
      }

      /* Add extra white noise value. */
      newRandom = ((long)rand()) >> PINK_RANDOM_SHIFT;
      sum = pink_RunningSum + newRandom;

      /* Scale to range of -1.0 to 0.9999. */
      output = pink_Scalar * sum;

      return output;
    }

