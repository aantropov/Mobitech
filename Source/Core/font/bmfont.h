#ifndef __BMFONT__
#define __BMFONT__

#include "../math/math3d.h"
#include <vector>
#include <map>
#include "../Resources.h"

#ifndef MAKE_RGBA
#define MAKE_RGBA(r,g,b,a)  (r | (g << 8) | (b << 16) | (a << 24))
#endif
 
#ifndef GET_BLUE
#define GET_BLUE(rgba) (( (rgba)>>16 ) & 0xff )
#endif
 
#ifndef GET_GREEN
#define GET_GREEN(rgba) (( (rgba)>>8 ) & 0xff )
#endif
 
#ifndef GET_RED
#define GET_RED(rgba) ( rgba & 0xff )
#endif

#ifndef GET_ALPHA
#define GET_ALPHA(rgba) (( (rgba)>>24 ) & 0xff)
#endif

using namespace std;

class KearningInfo
{
public:
	short First;
	short Second;
	short Amount;

	KearningInfo() :  First( 0 ), Second( 0 ), Amount( 0 )	{ }
};


class CharDescriptor
{
public:
	short x, y;
	short width;
	short height;
	short XOffset;
	short YOffset;
	short XAdvance;
	short Page;

	CharDescriptor() : x( 0 ), y( 0 ), width( 0 ), height( 0 ), XOffset( 0 ), YOffset( 0 ),
		XAdvance( 0 ), Page( 0 )
	{ }
};

class Texture;
class ShaderProgram;
class ResourceFactory;

class BMFont :  public Resource
{
 public:

    Texture *atlas;
    ShaderProgram *shader;

	virtual bool Load(const string font_file);    

	void SetColor(int r, int g, int b, int a) { fcolor = MAKE_RGBA(r,g,b,a); }
    void SetColor(vec4 color) { SetColor((int)(color.x * 255.0f), (int)(color.y * 255.0f), (int)(color.z * 255.0f), (int)(color.w * 255.0f)); }
	void SetBlend(int b) { fblend = b; }
	void SetScale(float scale) { fscale = scale; }
	float GetHeight() const { return line_height * fscale; }
	void Print(float, float, const char *,...);
    void Print(mat4 model, const char *fmt, ...);
    void Print(float x, float y, mat4 model, const char *text);
	void PrintCenter(float, const char *);

	BMFont() 
	{
		  SetColor(255,255,255,255);
   	      kern_count = 0;
		  fblend = 0;
          fscale = 1.0;
	};

	virtual ~BMFont ();
    virtual void Free() {}

private:
    //ResourceFactory *rf;
    short line_height;
	short base;
	short width;
	short height;
	short pages;
	short outline;
	short kern_count;
	std::map<int,CharDescriptor> chars;
	std::vector<KearningInfo> kearn;
	int fcolor;
	float fscale;
	int fblend;

	bool ParseFont(const char *);
	int GetKerningPair(int, int)  const;
	float GetStringWidth(const char *);
    void RenderString(ShaderProgram *shader, int len) const;
};

#endif