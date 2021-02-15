// ----------------------------------------------------------------------------------------
// Name        : RectPlacement.cpp
// Description : A class that fits subrectangles into a power-of-2 rectangle
//               (C) Copyright 2000-2002 by Javier Arevalo
//               This code is free to use and modify for all purposes
// ----------------------------------------------------------------------------------------
// modified to support sprite margins and VC8
// (C) Copyright 2008 by Haaf
// --------------------------------------------------------------------------------

/*
  You have a bunch of rectangular pieces. You need to arrange them in a
  rectangular surface so that they don't overlap, keeping the total area of the
  rectangle as small as possible. This is fairly common when arranging characters
  in a bitmapped font, lightmaps for a 3D engine, and I guess other situations as
  well.

  The idea of this algorithm is that, as we add rectangles, we can pre-select
  "interesting" places where we can try to add the next rectangles. For optimal
  results, the rectangles should be added in order. I initially tried using area
  as a sorting criteria, but it didn't work well with very tall or very flat
  rectangles. I then tried using the longest dimension as a selector, and it
  worked much better. So much for intuition...

  These "interesting" places are just to the right and just below the currently
  added rectangle. The first rectangle, obviously, goes at the top left, the next
  one would go either to the right or below this one, and so on. It is a weird way
  to do it, but it seems to work very nicely.

  The way we search here is fairly brute-force, the fact being that for most off-
  line purposes the performance seems more than adequate. I have generated a
  japanese font with around 8500 characters and all the time was spent generating
  the bitmaps.

  Also, for all we care, we could grow the parent rectangle in a different way
  than power of two. It just happens that power of 2 is very convenient for
  graphics hardware textures.

  I'd be interested in hearing of other approaches to this problem. Make sure
  to post them on http://www.flipcode.com
*/

#include "RectPlacement.h"


int CRectPlacement::m_margin = 1;

void CRectPlacement::Init(const int w, const int h) {
  End();

  m_size = TRect(0, 0, w, h);
  m_vPositions.push_back(TPos(m_margin, m_margin));
  m_area = 0;
}


void CRectPlacement::End() {
  m_vPositions.clear();
  m_vRects.clear();
  m_size.w = 0;
}

// Description : Check if the given rectangle is partially or totally used
bool CRectPlacement::IsFree(const TRect &r) const {
  if (!m_size.Contains(r)) {
    return false;
  }
  for (auto it = m_vRects.begin();
       it != m_vRects.end();
       ++it)
    if (it->Intersects(r)) {
      return false;
    }
  return true;
}

// Description : Add new anchor point
void CRectPlacement::AddPosition(const TPos &p) {
  // Try to insert anchor as close as possible to the top left corner
  // So it will be tried first
  auto found = false;
  CPosArray::iterator it;
  for (it = m_vPositions.begin();
       !found && it != m_vPositions.end();
       ++it) {
    if (p.x + p.y < it->x + it->y) {
      found = true;
    }
  }
  if (found) {
    m_vPositions.insert(it, p);
  } else {
    m_vPositions.push_back(p);
  }
}

// Description : Add the given rect and updates anchor points
void CRectPlacement::AddRect(const TRect &r) {
  m_vRects.push_back(r);
  m_area += r.w * r.h;

  // Add two new anchor points
  AddPosition(TPos(r.x, r.y + r.h + m_margin));
  AddPosition(TPos(r.x + r.w + m_margin, r.y));
}

// Description : Add the given rectangle
bool CRectPlacement::AddAtEmptySpot(TRect &r) {
  // Find a valid spot among available anchors.

  auto b_found = false;
  CPosArray::iterator it;
  for (it = m_vPositions.begin();
       !b_found && it != m_vPositions.end();
       ++it) {
    const TRect Rect(it->x, it->y, r.w, r.h);

    if (IsFree(Rect)) {
      r = Rect;
      b_found = true;
      break; // Don't let the loop increase the iterator.
    }
  }
  if (b_found) {
    // Remove the used anchor point
    m_vPositions.erase(it);

    // Sometimes, anchors end up displaced from the optimal position
    // due to irregular sizes of the subrects.
    // So, try to adjut it up & left as much as possible.

    int x, y;

    for (x = 1; x <= r.x; x++)
      if (!IsFree(TRect(r.x - x, r.y, r.w, r.h))) {
        break;
      }
    for (y = 1; y <= r.y; y++)
      if (!IsFree(TRect(r.x, r.y - y, r.w, r.h))) {
        break;
      }
    if (y > x) {
      r.y -= y - 1;
    } else {
      r.x -= x - 1;
    }
    AddRect(r);
  }
  return b_found;
}


// Description : Add a rectangle of the given size, growing our area if needed
//               Area grows only until the max given.
//               Returns the placement of the rect in the rect's x,y coords
bool CRectPlacement::AddAtEmptySpotAutoGrow(TRect *p_rect, const int max_w,
                                            const int max_h) {
  if (p_rect->w <= 0) {
    return true;
  }

  const auto org_w = m_size.w;
  const auto org_h = m_size.h;

  // Try to add it in the existing space
  while (!AddAtEmptySpot(*p_rect)) {
    const int pw = m_size.w;
    const int ph = m_size.h;

    // Sanity check - if area is complete.
    if (pw >= max_w && ph >= max_h) {
      m_size.w = org_w;
      m_size.h = org_h;
      return false;
    }

    // Try growing the smallest dim
    if (pw < max_w && (pw < ph || ((pw == ph) && (p_rect->w >= p_rect->h)))) {
      m_size.w = pw * 2;
    } else {
      m_size.h = ph * 2;
    }
    if (AddAtEmptySpot(*p_rect)) {
      break;
    }

    // Try growing the other dim instead
    if (pw != m_size.w) {
      m_size.w = pw;
      if (ph < max_w) {
        m_size.h = ph * 2;
      }
    } else {
      m_size.h = ph;
      if (pw < max_w) {
        m_size.w = pw * 2;
      }
    }

    if (pw != m_size.w || ph != m_size.h)
      if (AddAtEmptySpot(*p_rect)) {
        break;
      }

    // Grow both if possible, and reloop.
    m_size.w = pw;
    m_size.h = ph;
    if (pw < max_w) {
      m_size.w = pw * 2;
    }
    if (ph < max_h) {
      m_size.h = ph * 2;
    }
  }
  return true;
}
