#include "interpolate.h"
#include "definitions_cxx.hpp"
#include <argon.hpp>

namespace deluge::dsp {
void interpolate(int32_t* sampleRead, int32_t numChannelsNow, int32_t whichKernel, uint32_t oscPos,
                 std::array<std::array<int16x4_t, kInterpolationMaxNumSamples / 4>, 2>& interpolationBuffer) {
	constexpr size_t numBitsInTableSize = 8;

	// that's (numBitsInInput - 16 - numBitsInTableSize); = 4 for now
	constexpr size_t rshiftAmount = ((24 + kInterpolationMaxNumSamplesMagnitude) - 16 - numBitsInTableSize + 1);

	uint32_t rshifted;
	if constexpr (rshiftAmount >= 0) {
		rshifted = oscPos >> rshiftAmount;
	}
	else {
		rshifted = oscPos << (-rshiftAmount);
	}

	int16_t strength2 = rshifted & 32767;

	int32_t progressSmall = oscPos >> (24 + kInterpolationMaxNumSamplesMagnitude - numBitsInTableSize);

	int16x8_t kernelVector[kInterpolationMaxNumSamples >> 3];

	for (int32_t i = 0; i < (kInterpolationMaxNumSamples >> 3); i++) {
		int16x8_t value1 = vld1q_s16(&windowedSincKernel[whichKernel][progressSmall][i << 3]);
		int16x8_t value2 = vld1q_s16(&windowedSincKernel[whichKernel][progressSmall + 1][i << 3]);
		int16x8_t difference = vsubq_s16(value2, value1);
		int16x8_t multipliedDifference = vqdmulhq_n_s16(difference, strength2);
		kernelVector[i] = vaddq_s16(value1, multipliedDifference);
	}

	int32x4_t multiplied;

	for (int32_t i = 0; i < (kInterpolationMaxNumSamples >> 3); i++) {

		if (i == 0) {
			multiplied = vmull_s16(vget_low_s16(kernelVector[i]), interpolationBuffer[0][i << 1]);
		}
		else {
			multiplied = vmlal_s16(multiplied, vget_low_s16(kernelVector[i]), interpolationBuffer[0][i << 1]);
		}

		multiplied = vmlal_s16(multiplied, vget_high_s16(kernelVector[i]), interpolationBuffer[0][(i << 1) + 1]);
	}

	int32x2_t twosies = vadd_s32(vget_high_s32(multiplied), vget_low_s32(multiplied));

	sampleRead[0] = vget_lane_s32(twosies, 0) + vget_lane_s32(twosies, 1);

	if (numChannelsNow == 2) {

		int32x4_t multiplied;

		for (int32_t i = 0; i < (kInterpolationMaxNumSamples >> 3); i++) {

			if (i == 0) {
				multiplied = vmull_s16(vget_low_s16(kernelVector[i]), interpolationBuffer[1][i << 1]);
			}
			else {
				multiplied = vmlal_s16(multiplied, vget_low_s16(kernelVector[i]), interpolationBuffer[1][i << 1]);
			}

			multiplied = vmlal_s16(multiplied, vget_high_s16(kernelVector[i]), interpolationBuffer[1][(i << 1) + 1]);
		}

		int32x2_t twosies = vadd_s32(vget_high_s32(multiplied), vget_low_s32(multiplied));

		sampleRead[1] = vget_lane_s32(twosies, 0) + vget_lane_s32(twosies, 1);
	}
}

void interpolateLinear(int32_t* sampleRead, int32_t numChannelsNow, int32_t whichKernel, uint32_t oscPos,
                       std::array<std::array<int16x4_t, kInterpolationMaxNumSamples / 4>, 2>& interpolationBuffer) {
	int16_t strength2 = oscPos >> 9;
	int16_t strength1 = 32767 - strength2;

	sampleRead[0] = (interpolationBuffer[0][0][1] * strength1) + (interpolationBuffer[0][0][0] * strength2);
	if (numChannelsNow == 2) {
		sampleRead[1] = (interpolationBuffer[1][0][1] * strength1) + (interpolationBuffer[1][0][0] * strength2);
	}
}
} // namespace deluge::dsp

// clang-format off
const int16_t windowedSincKernel[][17][16] __attribute__ ((aligned (CACHE_LINE_SIZE))) = {
	{
		{0, 0, 0, 0, 0, 0, 0, -1, 32767, -1, 0, 0, 0, 0, 0, 0, },
		{-2, 7, -28, 80, -188, 400, -831, 2056, 32548, -1784, 754, -363, 168, -70, 24, -6, },
		{-4, 15, -58, 165, -387, 820, -1708, 4351, 31900, -3278, 1413, -682, 314, -129, 43, -10, },
		{-6, 25, -91, 252, -588, 1245, -2602, 6847, 30837, -4470, 1959, -947, 435, -176, 58, -13, },
		{-7, 34, -124, 339, -787, 1658, -3480, 9498, 29388, -5352, 2381, -1150, 525, -210, 68, -16, },
		{-9, 44, -156, 422, -971, 2039, -4300, 12249, 27586, -5935, 2676, -1291, 586, -232, 74, -17, },
		{-11, 54, -186, 496, -1132, 2369, -5026, 15048, 25476, -6230, 2842, -1369, 617, -242, 76, -17, },
		{-13, 62, -211, 557, -1261, 2632, -5621, 17835, 23107, -6255, 2882, -1387, 621, -241, 74, -16, },
		{-15, 69, -230, 600, -1348, 2808, -6042, 20540, 20541, -6042, 2808, -1348, 600, -230, 69, -15, },
		{-16, 74, -241, 621, -1387, 2882, -6255, 23107, 17835, -5621, 2632, -1261, 557, -211, 62, -13, },
		{-17, 76, -242, 617, -1369, 2842, -6230, 25476, 15048, -5026, 2369, -1132, 496, -186, 54, -11, },
		{-17, 74, -232, 586, -1291, 2676, -5935, 27586, 12249, -4300, 2039, -971, 422, -156, 44, -9, },
		{-16, 68, -210, 525, -1150, 2381, -5352, 29388, 9498, -3480, 1658, -787, 339, -124, 34, -7, },
		{-13, 58, -176, 435, -947, 1959, -4470, 30837, 6847, -2602, 1245, -588, 252, -91, 25, -6, },
		{-10, 43, -129, 314, -682, 1413, -3278, 31900, 4351, -1708, 820, -387, 165, -58, 15, -4, },
		{-6, 24, -70, 168, -363, 754, -1784, 32548, 2056, -831, 400, -188, 80, -28, 7, -2, },
		{0, 0, 0, 0, 0, 0, -1, 32767, -1, 0, 0, 0, 0, 0, 0, 0, },
	},
	{
		{-4, 5, 91, -378, 469, 726, -3908, 7797, 23170, 7797, -3908, 726, 468, -378, 91, 1, },
		{-5, 11, 82, -403, 601, 491, -3903, 9183, 23090, 6444, -3837, 928, 338, -347, 95, -3, },
		{-6, 17, 71, -422, 735, 221, -3811, 10585, 22849, 5135, -3693, 1092, 213, -312, 97, -6, },
		{-8, 24, 55, -432, 866, -80, -3623, 11984, 22458, 3881, -3488, 1218, 96, -274, 96, -8, },
		{-9, 32, 36, -433, 988, -406, -3339, 13369, 21911, 2701, -3232, 1310, -12, -234, 93, -10, },
		{-10, 40, 13, -424, 1102, -755, -2951, 14719, 21223, 1600, -2932, 1366, -109, -194, 88, -11, },
		{-11, 49, -14, -403, 1200, -1119, -2457, 16019, 20402, 588, -2600, 1389, -194, -154, 82, -12, },
		{-12, 57, -45, -369, 1282, -1494, -1854, 17253, 19458, -328, -2244, 1381, -267, -115, 74, -12, },
		{-12, 66, -79, -324, 1344, -1872, -1144, 18403, 18404, -1144, -1872, 1344, -324, -79, 66, -12, },
		{-12, 74, -115, -267, 1381, -2244, -328, 19458, 17253, -1854, -1494, 1282, -369, -45, 57, -12, },
		{-12, 82, -154, -194, 1389, -2600, 588, 20402, 16019, -2457, -1119, 1200, -403, -14, 49, -11, },
		{-11, 88, -194, -109, 1366, -2932, 1600, 21223, 14719, -2951, -755, 1102, -424, 13, 40, -10, },
		{-10, 93, -234, -12, 1310, -3232, 2701, 21911, 13369, -3339, -406, 988, -433, 36, 32, -9, },
		{-8, 96, -274, 96, 1218, -3488, 3881, 22458, 11984, -3623, -80, 866, -432, 55, 24, -8, },
		{-6, 97, -312, 213, 1092, -3693, 5135, 22849, 10585, -3811, 221, 735, -422, 71, 17, -6, },
		{-3, 95, -347, 338, 928, -3837, 6444, 23090, 9183, -3903, 491, 601, -403, 82, 11, -5, },
		{5, 91, -378, 469, 726, -3908, 7797, 23170, 7797, -3908, 726, 468, -378, 91, 1, -4, },
	},
	{
		{0, -33, 0, 380, 0, -1955, 0, 9800, 16381, 9800, 0, -1955, 0, 380, 0, -33, },
		{-1, -36, 14, 401, -94, -2036, 417, 10481, 16351, 9106, -379, -1858, 85, 356, -12, -30, },
		{-2, -39, 30, 419, -197, -2100, 869, 11148, 16261, 8404, -721, -1749, 161, 330, -22, -27, },
		{-3, -42, 47, 434, -307, -2143, 1358, 11790, 16111, 7698, -1025, -1630, 227, 303, -30, -23, },
		{-4, -45, 67, 443, -425, -2166, 1883, 12406, 15904, 6992, -1288, -1504, 284, 275, -37, -20, },
		{-6, -47, 89, 448, -550, -2162, 2438, 12992, 15636, 6294, -1517, -1369, 332, 246, -42, -17, },
		{-7, -48, 112, 447, -681, -2133, 3023, 13541, 15317, 5605, -1708, -1232, 371, 218, -45, -15, },
		{-9, -49, 137, 439, -815, -2075, 3636, 14052, 14946, 4928, -1863, -1094, 402, 190, -48, -12, },
		{-10, -49, 163, 424, -953, -1985, 4272, 14519, 14523, 4271, -1985, -953, 424, 163, -49, -10, },
		{-12, -48, 190, 402, -1094, -1863, 4928, 14946, 14052, 3636, -2075, -815, 439, 137, -49, -9, },
		{-15, -45, 218, 371, -1232, -1708, 5605, 15317, 13541, 3023, -2133, -681, 447, 112, -48, -7, },
		{-17, -42, 246, 332, -1369, -1517, 6294, 15636, 12992, 2438, -2162, -550, 448, 89, -47, -6, },
		{-20, -37, 275, 284, -1504, -1288, 6992, 15904, 12406, 1883, -2166, -425, 443, 67, -45, -4, },
		{-23, -30, 303, 227, -1630, -1025, 7698, 16111, 11790, 1358, -2143, -307, 434, 47, -42, -3, },
		{-27, -22, 330, 161, -1749, -721, 8404, 16261, 11148, 869, -2100, -197, 419, 30, -39, -2, },
		{-30, -12, 356, 85, -1858, -379, 9106, 16351, 10481, 417, -2036, -94, 401, 14, -36, -1, },
		{-33, 0, 380, 0, -1955, 0, 9800, 16381, 9800, 0, -1955, 0, 380, 0, -33, 0, },
	},
	{
		{2, 33, 49, -252, -880, -369, 3226, 8782, 11583, 8782, 3226, -369, -880, -253, 49, 36, },
		{3, 36, 43, -289, -904, -247, 3553, 9092, 11572, 8462, 2909, -480, -850, -220, 53, 32, },
		{4, 39, 36, -326, -925, -110, 3889, 9388, 11536, 8131, 2600, -577, -817, -188, 56, 29, },
		{5, 42, 28, -365, -940, 40, 4232, 9670, 11476, 7790, 2304, -662, -781, -158, 58, 26, },
		{7, 45, 19, -407, -949, 203, 4582, 9936, 11392, 7443, 2019, -735, -741, -131, 59, 23, },
		{9, 48, 5, -446, -954, 383, 4934, 10188, 11284, 7092, 1744, -794, -703, -103, 59, 19, },
		{9, 52, -8, -488, -949, 576, 5291, 10421, 11152, 6736, 1483, -844, -662, -80, 58, 18, },
		{11, 54, -23, -532, -936, 781, 5653, 10634, 11002, 6375, 1236, -885, -618, -59, 57, 15, },
		{13, 56, -40, -575, -915, 1002, 6014, 10826, 10830, 6013, 1002, -915, -575, -40, 56, 13, },
		{15, 57, -59, -618, -885, 1236, 6375, 11002, 10634, 5653, 781, -936, -532, -23, 54, 11, },
		{18, 58, -80, -662, -844, 1483, 6736, 11152, 10421, 5291, 576, -949, -488, -8, 52, 9, },
		{19, 59, -103, -703, -794, 1744, 7092, 11284, 10188, 4934, 383, -954, -446, 5, 48, 9, },
		{23, 59, -131, -741, -735, 2019, 7443, 11392, 9936, 4582, 203, -949, -407, 19, 45, 7, },
		{26, 58, -158, -781, -662, 2304, 7790, 11476, 9670, 4232, 40, -940, -365, 28, 42, 5, },
		{29, 56, -188, -817, -577, 2600, 8131, 11536, 9388, 3889, -110, -925, -326, 36, 39, 4, },
		{32, 53, -220, -850, -480, 2909, 8462, 11572, 9092, 3553, -247, -904, -289, 43, 36, 3, },
		{33, 49, -252, -880, -369, 3226, 8782, 11583, 8782, 3226, -369, -880, -253, 49, 36, 2, },
	},
	{
		{0, -24, -134, -275, 0, 1412, 4142, 7078, 8367, 7078, 4142, 1412, 0, -275, -134, -24, },
		{0, -28, -144, -277, 48, 1548, 4338, 7225, 8361, 6925, 3947, 1281, -43, -271, -124, -21, },
		{-1, -32, -154, -277, 100, 1691, 4534, 7365, 8344, 6766, 3753, 1157, -82, -266, -115, -18, },
		{-1, -36, -165, -277, 160, 1837, 4732, 7495, 8319, 6598, 3564, 1035, -116, -260, -105, -15, },
		{-3, -40, -176, -271, 221, 1991, 4927, 7621, 8279, 6429, 3373, 924, -149, -252, -98, -11, },
		{-3, -47, -185, -268, 291, 2147, 5125, 7734, 8235, 6251, 3189, 814, -174, -246, -88, -10, },
		{-4, -52, -197, -259, 363, 2311, 5319, 7842, 8176, 6071, 3005, 713, -198, -236, -80, -9, },
		{-4, -59, -207, -249, 443, 2477, 5511, 7939, 8108, 5888, 2826, 616, -217, -227, -72, -8, },
		{-6, -65, -217, -234, 527, 2650, 5701, 8025, 8029, 5700, 2650, 527, -234, -217, -65, -6, },
		{-8, -72, -227, -217, 616, 2826, 5888, 8108, 7939, 5511, 2477, 443, -249, -207, -59, -4, },
		{-9, -80, -236, -198, 713, 3005, 6071, 8176, 7842, 5319, 2311, 363, -259, -197, -52, -4, },
		{-10, -88, -246, -174, 814, 3189, 6251, 8235, 7734, 5125, 2147, 291, -268, -185, -47, -3, },
		{-11, -98, -252, -149, 924, 3373, 6429, 8279, 7621, 4927, 1991, 221, -271, -176, -40, -3, },
		{-15, -105, -260, -116, 1035, 3564, 6598, 8319, 7495, 4732, 1837, 160, -277, -165, -36, -1, },
		{-18, -115, -266, -82, 1157, 3753, 6766, 8344, 7365, 4534, 1691, 100, -277, -154, -32, -1, },
		{-21, -124, -271, -43, 1281, 3947, 6925, 8361, 7225, 4338, 1548, 48, -277, -144, -28, 0, },
		{-24, -134, -275, 0, 1412, 4142, 7078, 8367, 7078, 4142, 1412, 0, -275, -134, -24, 0, },
	},
	{
		{-5, -25, -28, 155, 821, 2207, 4118, 5858, 6564, 5858, 4118, 2207, 821, 154, -28, -30, },
		{-5, -27, -26, 179, 886, 2316, 4241, 5940, 6562, 5772, 3994, 2100, 760, 133, -31, -29, },
		{-6, -29, -20, 203, 956, 2425, 4366, 6015, 6554, 5680, 3872, 1994, 703, 110, -32, -26, },
		{-7, -30, -16, 232, 1026, 2539, 4486, 6089, 6538, 5587, 3747, 1893, 646, 93, -34, -24, },
		{-9, -30, -11, 264, 1098, 2655, 4605, 6158, 6517, 5490, 3622, 1794, 592, 77, -36, -21, },
		{-10, -33, -4, 295, 1176, 2771, 4723, 6224, 6493, 5389, 3498, 1697, 542, 60, -36, -20, },
		{-11, -34, 4, 330, 1255, 2889, 4841, 6281, 6461, 5284, 3375, 1603, 495, 46, -36, -18, },
		{-12, -35, 13, 367, 1338, 3009, 4955, 6333, 6424, 5177, 3252, 1512, 450, 34, -36, -16, },
		{-14, -35, 23, 407, 1423, 3130, 5067, 6382, 6381, 5067, 3130, 1423, 407, 23, -35, -14, },
		{-16, -36, 34, 450, 1512, 3252, 5177, 6424, 6333, 4955, 3009, 1338, 367, 13, -35, -12, },
		{-18, -36, 46, 495, 1603, 3375, 5284, 6461, 6281, 4841, 2889, 1255, 330, 4, -34, -11, },
		{-20, -36, 60, 542, 1697, 3498, 5389, 6493, 6224, 4723, 2771, 1176, 295, -4, -33, -10, },
		{-21, -36, 77, 592, 1794, 3622, 5490, 6517, 6158, 4605, 2655, 1098, 264, -11, -30, -9, },
		{-24, -34, 93, 646, 1893, 3747, 5587, 6538, 6089, 4486, 2539, 1026, 232, -16, -30, -7, },
		{-26, -32, 110, 703, 1994, 3872, 5680, 6554, 6015, 4366, 2425, 956, 203, -20, -29, -6, },
		{-29, -31, 133, 760, 2100, 3994, 5772, 6562, 5940, 4241, 2316, 886, 179, -26, -27, -5, },
		{-25, -28, 155, 821, 2207, 4118, 5858, 6564, 5858, 4118, 2207, 821, 154, -28, -30, -5, },
	},
	{
		{0, 17, 128, 486, 1262, 2499, 3968, 5189, 5667, 5189, 3968, 2499, 1262, 486, 128, 17, },
		{0, 20, 142, 521, 1326, 2589, 4056, 5246, 5664, 5131, 3876, 2411, 1199, 453, 116, 15, },
		{1, 24, 156, 557, 1393, 2677, 4146, 5296, 5659, 5068, 3787, 2324, 1138, 421, 105, 13, },
		{1, 27, 171, 596, 1460, 2769, 4231, 5349, 5649, 5005, 3694, 2239, 1078, 391, 94, 11, },
		{1, 32, 187, 636, 1530, 2861, 4317, 5394, 5636, 4938, 3601, 2154, 1021, 363, 85, 9, },
		{2, 36, 205, 676, 1604, 2951, 4403, 5436, 5620, 4866, 3510, 2069, 968, 336, 76, 7, },
		{2, 41, 224, 721, 1677, 3044, 4484, 5476, 5598, 4795, 3416, 1989, 914, 311, 67, 6, },
		{3, 47, 243, 766, 1753, 3137, 4565, 5513, 5572, 4720, 3323, 1908, 863, 287, 60, 5, },
		{4, 53, 265, 814, 1830, 3230, 4644, 5541, 5545, 4643, 3230, 1830, 814, 265, 53, 4, },
		{5, 60, 287, 863, 1908, 3323, 4720, 5572, 5513, 4565, 3137, 1753, 766, 243, 47, 3, },
		{6, 67, 311, 914, 1989, 3416, 4795, 5598, 5476, 4484, 3044, 1677, 721, 224, 41, 2, },
		{7, 76, 336, 968, 2069, 3510, 4866, 5620, 5436, 4403, 2951, 1604, 676, 205, 36, 2, },
		{9, 85, 363, 1021, 2154, 3601, 4938, 5636, 5394, 4317, 2861, 1530, 636, 187, 32, 1, },
		{11, 94, 391, 1078, 2239, 3694, 5005, 5649, 5349, 4231, 2769, 1460, 596, 171, 27, 1, },
		{13, 105, 421, 1138, 2324, 3787, 5068, 5659, 5296, 4146, 2677, 1393, 557, 156, 24, 1, },
		{15, 116, 453, 1199, 2411, 3876, 5131, 5664, 5246, 4056, 2589, 1326, 521, 142, 20, 0, },
		{17, 128, 486, 1262, 2499, 3968, 5189, 5667, 5189, 3968, 2499, 1262, 486, 128, 17, 0, },
	},
};
