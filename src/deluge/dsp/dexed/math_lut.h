
#pragma once

#include "Dexed.h"

// Use twice as much RAM for the LUT but avoid a little computation
#define SIN_DELTA

struct DexedLutData {
#define EXP2_LG_N_SAMPLES 10
#define EXP2_N_SAMPLES (1 << EXP2_LG_N_SAMPLES)
	int32_t exp2tab[EXP2_N_SAMPLES << 1];

#define TANH_LG_N_SAMPLES 10
#define TANH_N_SAMPLES (1 << TANH_LG_N_SAMPLES)
	int32_t tanhtab[TANH_N_SAMPLES << 1];

// Use twice as much RAM for the LUT but avoid a little computation
#define SIN_DELTA

#define SIN_LG_N_SAMPLES 10
#define SIN_N_SAMPLES (1 << SIN_LG_N_SAMPLES)
#ifdef SIN_DELTA
	int32_t sintab[SIN_N_SAMPLES << 1];
#else
	int32_t sintab[SIN_N_SAMPLES + 1];
#endif

#define FREQ_LG_N_SAMPLES 10
#define FREQ_N_SAMPLES (1 << FREQ_LG_N_SAMPLES)
	int32_t freq_lut[FREQ_N_SAMPLES + 1];
};

extern DexedLutData* lut_data;

void dexed_init_lut_data();

class Exp2 {
public:
	// Q24 in, Q24 out
	static int32_t lookup(int32_t x);
};

inline int32_t Exp2::lookup(int32_t x) {
	const int SHIFT = 24 - EXP2_LG_N_SAMPLES;
	int lowbits = x & ((1 << SHIFT) - 1);
	int x_int = (x >> (SHIFT - 1)) & ((EXP2_N_SAMPLES - 1) << 1);
	int dy = lut_data->exp2tab[x_int];
	int y0 = lut_data->exp2tab[x_int + 1];

	int y = y0 + (((int64_t)dy * (int64_t)lowbits) >> SHIFT);
	return y >> (6 - (x >> 24));
}

class Tanh {
public:
	// Q24 in, Q24 out
	static int32_t lookup(int32_t x);
};

inline int32_t Tanh::lookup(int32_t x) {
	int32_t signum = x >> 31;
	x ^= signum;
	if (x >= (4 << 24)) {
		if (x >= (17 << 23)) {
			return signum ^ (1 << 24);
		}
		int32_t sx = ((int64_t)-48408812 * (int64_t)x) >> 24;
		return signum ^ ((1 << 24) - 2 * Exp2::lookup(sx));
	}
	else {
		const int SHIFT = 26 - TANH_LG_N_SAMPLES;
		int lowbits = x & ((1 << SHIFT) - 1);
		int x_int = (x >> (SHIFT - 1)) & ((TANH_N_SAMPLES - 1) << 1);
		int dy = lut_data->tanhtab[x_int];
		int y0 = lut_data->tanhtab[x_int + 1];
		int y = y0 + (((int64_t)dy * (int64_t)lowbits) >> SHIFT);
		return y ^ signum;
	}
}

class Sin {
public:
	static int32_t lookup(int32_t phase);
};

inline int32_t Sin::lookup(int32_t phase) {
	const int SHIFT = 24 - SIN_LG_N_SAMPLES;
	int lowbits = phase & ((1 << SHIFT) - 1);
#ifdef SIN_DELTA
	int phase_int = (phase >> (SHIFT - 1)) & ((SIN_N_SAMPLES - 1) << 1);
	int dy = lut_data->sintab[phase_int];
	int y0 = lut_data->sintab[phase_int + 1];

	return y0 + (((int64_t)dy * (int64_t)lowbits) >> SHIFT);
#else
	int phase_int = (phase >> SHIFT) & (SIN_N_SAMPLES - 1);
	int y0 = lut_data->sintab[phase_int];
	int y1 = lut_data->sintab[phase_int + 1];

	return y0 + (((int64_t)(y1 - y0) * (int64_t)lowbits) >> SHIFT);
#endif
}

class Freqlut {
public:
	static int32_t lookup(int32_t logfreq);
};
