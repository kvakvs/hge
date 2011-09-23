
#ifndef GFX_OBJECT_H
#define GFX_OBJECT_H

#include "texasm.h"
#include <stdio.h>
#include <list>

using namespace std;

class CGfxObject
{
public:
	CGfxObject(char *_name, int _resgroup) { name=_name; placed = false; resgroup = _resgroup; }

	char	*GetName()						{ return name; }
	void	Place(int _x, int _y)			{ x = _x; y = _y; placed = true; }
	bool	IsPlaced()						{ return placed; }
	int		GetResGroup()					{ return resgroup; }
	void	GetTargetPos(int *_x, int *_y)	{ *_x = x; *_y = y; }

	virtual int			GetWidth() const = 0;
	virtual int			GetHeight() const = 0;
	virtual HTEXTURE	GetTexture() = 0;
	virtual void		GetSourcePos(int *_x, int *_y) = 0;
	virtual bool		SaveDescription(FILE *fp, char *texname) = 0;

protected:
	char		*name;
	int			resgroup;
	bool		placed;
	int			x;
	int			y;
};


// sorting predicate

// WORKAROUND for VC6 and earlier - http://support.microsoft.com/kb/265109

#ifdef _MSC_VER
#if _MSC_VER < 1300
template<>
struct std::greater<CGfxObject*>  : public binary_function<CGfxObject*, CGfxObject*, bool> 
{
	bool operator()(const CGfxObject* &a, const CGfxObject* &b) const
	{ 
		return (a->GetWidth()  < b->GetWidth() && a->GetWidth()  > b->GetHeight()) ||
               (a->GetHeight() > b->GetWidth() && a->GetHeight() > b->GetHeight());
	}
};
#endif
#endif

// END WORKAROUND

struct ByLargestDimensionDescending : public greater<CGfxObject *>
{
	bool operator()(CGfxObject* &a, CGfxObject* &b) const
	{ 
		return (a->GetWidth()  < b->GetWidth() && a->GetWidth()  > b->GetHeight()) ||
               (a->GetHeight() > b->GetWidth() && a->GetHeight() > b->GetHeight());
	}
};


// list and iterator

typedef list<CGfxObject *> GfxObjList;
typedef list<CGfxObject *>::iterator GfxObjIterator;


#endif
