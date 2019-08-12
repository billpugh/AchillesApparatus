

/** Call initially to initialize tubes */
void initializeTubes();

/** Set the number of ms for a tube period */
void setTubePeriod(unsigned int tubePeriodMS);

/** Set the amplitude of the tube; 0 is off, 1 is maximum */
void setTubeAmplitude(float amplitude);
 
/** Call at least once every 100ms, no more than once every 20ms. */
void updateTubes();
