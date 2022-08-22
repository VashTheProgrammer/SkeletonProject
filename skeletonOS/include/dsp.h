#ifndef DSP_H_
#define DSP_H_

#define ARRAYSIZE(x)  (sizeof(x) / sizeof((x)[0]))
#define FLOAT_TO_INT(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// y = A * sin (2 * pi * f * x) + C
#define SIN_WAVE(AMPLITUDE, FREQUENCY, OFFSET, TIME_VALUE) (AMPLITUDE * sin(2 * PI * FREQUENCY * TIME_VALUE) + OFFSET)
 
#define PI 3.14159265358979323846	/* pi */

typedef struct {
   double real;
	double imag;
} COMPLEX;

extern int fft_realDSP(COMPLEX *x, uint32_t N);

#define DSP_FFT_REAL(ARRAY, SIZE) \
	fft_realDSP(ARRAY, SIZE)

extern int ifft_realDSP(COMPLEX *x, uint32_t N);

#define DSP_iFFT_REAL(ARRAY, SIZE) \
	ifft_realDSP(ARRAY, SIZE)

/**
 * ds_fft: input - the double sided fft
 * N: input - the size of the double sided fft
 * ss_fft: output - the single sided, amplitude corrected fft
 * size: output - the size of the ss_fft
 */
extern int fftGetSingleSidedDSP(COMPLEX *ds_fft, uint32_t N, double *ss_fft, uint32_t *size);

#define DSP_FFT_SINGLE_SIDED(INPUT, INPUT_SIZE, OUTPUT, OUTPUT_SIZE) \
	fftGetSingleSidedDSP(INPUT, INPUT_SIZE, OUTPUT, OUTPUT_SIZE)

// Statistic Functions

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the standard deviation value
 */
extern double standardDeviationDSP(double *input, uint64_t size);

#define DSP_STD(INPUT, SIZE) \
	standardDeviationDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the variance value
 */
extern double varianceDSP(double *input, uint64_t size);

#define DSP_VARIANCE(INPUT, SIZE) \
	varianceDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the skewness value
 */
extern double skewnessDSP(double *input, uint64_t size);

#define DSP_SKEWNESS(INPUT, SIZE) \
	skewnessDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the kurtosis value
 */
extern double kurtosisDSP(double *input, uint64_t size);

#define DSP_KURTOSIS(INPUT, SIZE) \
	kurtosisDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the maximum value
 */
extern double maxValueDSP(double *input, uint64_t size);

#define DSP_MAX(INPUT, SIZE) \
	maxValueDSP(INPUT, SIZE)

/**
 * input: input - an array of double on which found the min value
 * size: input - the size of the array
 * return: the minimum value
 */
double minValueDSP(double *input, uint64_t size);

#define DSP_MIN(INPUT, SIZE) \
	minValueDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the rms calculated
 */
extern double rmsDSP(double *input, uint64_t size);

#define DSP_RMS(INPUT, SIZE) \
	rmsDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the crest factor value
 */
extern double crestFactorDSP(double *input, uint64_t size);

#define DSP_CRESTFACTOR(INPUT, SIZE) \
	crestFactorDSP(INPUT, SIZE)

/**
 * input: input - an array of double
 * size: input - the size of the array
 * return: the mean calculated
 */
extern double meanDSP(double *input, uint64_t size);

#define DSP_MEAN(INPUT, SIZE) \
	mean(INPUT, SIZE)



#endif /* MATHAPI_H_ */
