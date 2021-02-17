/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
//
// hgeVector helper class implementation
//


#include <hgevector.h>

float hgeVector::InvSqrt(const float x) {
  union {
      int int_part;
      float float_part;
  } convertor{};

  convertor.float_part = x;
  convertor.int_part = 0x5f3759df - (convertor.int_part >> 1);
  return convertor.float_part *
         (1.5f - 0.4999f * x * convertor.float_part * convertor.float_part);
}

/*
hgeVector *hgeVector::Normalize()
{
	float lenRcp;

	lenRcp=sqrtf(Dot(this));

	if(lenRcp)
	{
		lenRcp=1.0f/lenRcp;

		x*=lenRcp;
		y*=lenRcp;
	}

	return this;
}
*/

float hgeVector::Angle(const hgeVector *v) const {
  if (v) {
    hgeVector s = *this, t = *v;

    s.Normalize();
    t.Normalize();
    return acosf(s.Dot(&t));
  }
  return atan2f(y, x);
}

hgeVector *hgeVector::Rotate(float a) {
  hgeVector v;

  v.x = x * cosf(a) - y * sinf(a);
  v.y = x * sinf(a) + y * cosf(a);

  x = v.x;
  y = v.y;

  return this;
}
