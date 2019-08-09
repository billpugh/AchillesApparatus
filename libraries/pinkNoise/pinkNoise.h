
#ifndef PinkNoise_h
#define PinkNoise_h
#define PINK_MAX_RANDOM_ROWS   (30)

class PinkNoise
{
    long      pink_Rows[PINK_MAX_RANDOM_ROWS];
    long      pink_RunningSum;   /* Used to optimize summing of generators. */
    int       pink_Index;        /* Incremented each sample. */
    int       pink_IndexMask;    /* Index wrapped by ANDing with this mask. */
    float     pink_Scalar;       /* Used to scale within range of -1.0 to +1.0 */

  public:  PinkNoise(int numRows );

    float generate();
} ;

#endif
