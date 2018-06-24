#include "convert.hpp"
#include <arm_neon.h>

void convert_reference(const uint8_t * src, uint8_t * dst, const int n)
{
	for(int i = 0; i < n; i++)
	{
		int r = *src++; // load red
		int g = *src++; // load green
		int b = *src++; // load blue

		// build weighted average:
		int y = (r*77)+(g*151)+(b*28);

		// undo the scale by 256 and write to memory:
		*dst++ = (y>>8);
	}
}

void convert_neon(const uint8_t * src, uint8_t * dst, const int n)
{
	const uint8x8_t rfac = vdup_n_u8(77);
	const uint8x8_t gfac = vdup_n_u8(151);
	const uint8x8_t bfac = vdup_n_u8(28);
	const int n_ = n >> 3;

	for(int i = 0; i < n_; i++)
	{
		uint16x8_t temp;
		uint8x8x3_t rgb = vld3_u8(src);
		uint8x8_t result;

		temp = vmull_u8(rgb.val[0], rfac);
		temp = vmlal_u8(temp,rgb.val[1], gfac);
		temp = vmlal_u8(temp,rgb.val[2], bfac);
		result = vshrn_n_u16(temp, 8);
		vst1_u8(dst, result);

		src  += 8*3;
		dst += 8;
	}
}
