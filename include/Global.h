
#pragma once
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <list>

using namespace std;


#define ARGB(a,r,g,b)	((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define GETA(col)		((col)>>24)
#define GETR(col)		(((col)>>16) & 0xFF)
#define GETG(col)		(((col)>>8) & 0xFF)
#define GETB(col)		((col) & 0xFF)
#define SETA(col,a)		(((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define SETR(col,r)		(((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define SETG(col,g)		(((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)		(((col) & 0xFFFFFF00) + DWORD(b))



#define RANIM_FWD			0
#define RANIM_REV			1
#define RANIM_PINGPONG	2
#define RANIM_NOPINGPONG	0
#define RANIM_LOOP		4
#define RANIM_NOLOOP		0



#define HGEPRIM_POINTS		1
#define HGEPRIM_LINES		2
#define HGEPRIM_TRIPLES		3
#define HGEPRIM_QUADS		4

#define	BLEND_COLORADD		1
#define	BLEND_COLORMUL		0

#define	BLEND_ALPHABLEND	2
#define	BLEND_ALPHAADD		0

#define	BLEND_ZWRITE		4
#define	BLEND_NOZWRITE		0

#define BLEND_DEFAULT		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE)
#define BLEND_DEFAULT_Z		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE)

#define HGEFPS_UNLIMITED	0
#define HGEFPS_VSYNC		-1

#define PIOVER180	0.017453292519943295769236907684886
#define M_PI 3.14159265358979323846
#define PI2   PI * 2
#define DEG  -180 / M_PI
#define RAD  M_PI / -180



#define RAND_MAX 0x7fff

#define RMAX(a, b) (a) < (b) ? (b) : (a)
#define RMIN(a, b) (a) > (b) ? (b) : (a)



typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;


struct rVertex
{
	float			x, y,z;		// screen position
	Color			col;		// color
	float			tx, ty;		// texture coordinates

};



struct rQuad
{
	rVertex		v[4];
	Texture2D		tex;
	int				blend;
};



template <typename T, typename Key>
bool key_exists(const T& container, const Key& key)
{
    return (container.find(key) != std::end(container));
}
template<typename T>
inline void remove(vector<T> & v, const T & item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}


