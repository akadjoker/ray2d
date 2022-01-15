#include "Core.h"
bool FIX_ARTIFACTS_BY_STRECHING_TEXEL;
int quadsDraw=0;
int CurBlendMode=0;

#define COLLIDE_MAX(a,b)	((a > b) ? a : b)
#define COLLIDE_MIN(a,b)	((a < b) ? a : b)

float  normalizeAngle(float angle)
	{
		// move into range [-180 deg, +180 deg]
		while (angle < -PI)
			angle += PI * 2.0f;
		while (angle > PI)
			angle -= PI * 2.0f;
		return angle;
	}

Vector2 vec2(float x,float y)
{
  Vector2 v;
  v.x=x;
  v.y=y;
  return v;
}

Vector2 vec2Zero()
{
  Vector2 v;
  v.x=0;
  v.y=0;
  return v;
}


int  sign(float value)
{
    return value < 0 ? -1 : (value > 0 ? 1 : 0);
}

 float distance(float x1,float  y1,float  x2 ,float y2 )
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

 float  distanceRects(float x1, float y1,float w1,float h1,float x2,float y2,float w2,float h2)
{
    if (x1 < x2 + w2 && x2 < x1 + w1)
    {
        if (y1 < y2 + h2 && y2 < y1 + h1) return 0;
        if (y1 > y2) return y1 - (y2 + h2);
        return y2 - (y1 + h1);
    }
    if (y1 < y2 + h2 && y2 < y1 + h1)
    {
        if (x1 > x2) return x1 - (x2 + w2);
        return x2 - (x1 + w1);
    }
    if (x1 > x2)
    {
        if (y1 > y2) return distance(x1, y1, (x2 + w2), (y2 + h2));
        return distance(x1, y1 + h1, x2 + w2, y2);
    }
    if (y1 > y2) return distance(x1 + w1, y1, x2, y2 + h2);
    return distance(x1 + w1, y1 + h1, x2, y2);
}

 float  distanceRectPoint(float px, float py, float rx, float ry, float rw,float rh)
{
    if (px >= rx && px <= rx + rw)
    {
        if (py >= ry && py <= ry + rh) return 0;
        if (py > ry) return py - (ry + rh);
        return ry - py;
    }
    if (py >= ry && py <= ry + rh)
    {
        if (px > rx) return px - (rx + rw);
        return rx - px;
    }
    if (px > rx)
    {
        if (py > ry) return distance(px, py, rx + rw, ry + rh);
        return distance(px, py, rx + rw, ry);
    }
    if (py > ry) return distance(px, py, rx, ry + rh);
    return distance(px, py, rx, ry);
}

float  getAngle(float x1 , float y1, float x2, float y2)
{
    float a = atan2(y2 - y1, x2 - x1) * DEG;
    return a < 0 ? a + 360 : a;
}

float  floatLerp(float  value1, float value2,float  amount)
    {
        return value1 + (value2 - value1) * amount;
    }
float Hermite(float value1,float  tangent1,float  value2, float tangent2, float amount)
    {
        float v1 = value1;
        float v2 = value2;
        float t1 = tangent1;
        float t2 = tangent2;
        float s = amount;
        float result;
        float sCubed = s * s * s;
        float sSquared = s * s;

        if (amount == 0)
            result = value1;
        else if (amount == 1)
            result = value2;
        else
            result = (2 * v1 - 2 * v2 + t2 + t1) * sCubed +
                (3 * v2 - 3 * v1 - 2 * t1 - t2) * sSquared +
                t1 * s +
                v1;
        return result;
    }
   float clamp(float value,float min,float  max)
	{
		return value < min ? min : (value > max ? max : value);
	}
 float  SmoothStep(float value1,float  value2,float  amount)
    {

        float result= clamp(amount, 0, 1);
        result =Hermite(value1, 0, value2, 0, result);

        return result;
    }

float lerpAngleDegrees(float a, float b, float lerpFactor) // Lerps from angle a to b (both between 0.f and 360.f), taking the shortest path
{
    float result;
    float diff = b - a;
    if (diff < -180.f)
    {
        // lerp upwards past 360
        b += 360.f;
        result = Lerp(a, b, lerpFactor);
        if (result >= 360.f)
        {
            result -= 360.f;
        }
    }
    else if (diff > 180.f)
    {
        // lerp downwards past 0
        b -= 360.f;
        result = Lerp(a, b, lerpFactor);
        if (result < 0.f)
        {
            result += 360.f;
        }
    }
    else
    {
        // straight lerp
        result = Lerp(a, b, lerpFactor);
    }

    return result;
}

#define PI 3.14159265359f
#define PI_TIMES_TWO 6.28318530718f
float lerpAngleRadians(float a, float b, float lerpFactor)
// Lerps from angle a to b (both between 0.f and PI_TIMES_TWO), taking the shortest path
{
    float result;
    float diff = b - a;
    if (diff < -PI)
    {
        // lerp upwards past PI_TIMES_TWO
        b += PI_TIMES_TWO;
        result = Lerp(a, b, lerpFactor);
        if (result >= PI_TIMES_TWO)
        {
            result -= PI_TIMES_TWO;
        }
    }
    else if (diff > PI)
    {
        // lerp downwards past 0
        b -= PI_TIMES_TWO;
        result = Lerp(a, b, lerpFactor);
        if (result < 0.f)
        {
            result += PI_TIMES_TWO;
        }
    }
    else
    {
        // straight lerp
        result = Lerp(a, b, lerpFactor);
    }

    return result;
}
/*
float  lerpAngle(float fromRadians, float toRadians, float progress)
	{
		float  delta = ((toRadians - fromRadians + PI2 + PI) % PI2) - PI;
		return (fromRadians + delta * progress + PI2) % PI2;
	}

float lerpAngleDeg(float fromDegrees,float  toDegrees,float progress)
{
    float delta = ((toDegrees - fromDegrees + 360 + 180) % 360) - 180;
    return (fromDegrees + delta * progress + 360) % 360;
}

float getAngle(float x, float y,float x2,float y2)
{
    float dx  = x - x2;
    float  dy = y - y2;
    return (atan2(dy, dx) * 180 / PI) + 90;
}
*/

 Vector2 normalize(Vector2 v , float thickness)
   {
      if (v.x == 0 && v.y == 0)
      {
         return v;

      } else
      {
         float norm = thickness / sqrt(v.x * v.x + v.y * v.y);
         v.x *= norm;
         v.y *= norm;
         return v;
      }
   }


 std::vector<Vector2> getPositions(Rectangle rectangle)
{

   std::vector<Vector2> points;
   points.push_back(vec2(rectangle.x,rectangle.y));
   points.push_back(vec2(rectangle.width,rectangle.y));
   points.push_back(vec2(rectangle.x,rectangle.height));
   points.push_back(vec2(rectangle.width,rectangle.height));
    return points;
}

Rectangle getMatrixBounds(Matrix2D* matrix,Rectangle rectangle)
{
float  minX=9999;
float  maxX=-9999;
float minY  =9999;
float maxY=-9999;

Rectangle out;


    std::vector<Vector2> positions= getPositions(rectangle);

    for (int i=0;i<4;i++)
    {
        Vector2 sPoint = matrix->TransformPoint(positions[i]);

        if (minX > sPoint.x) minX = sPoint.x;
        if (maxX < sPoint.x) maxX = sPoint.x;
        if (minY > sPoint.y) minY = sPoint.y;
        if (maxY < sPoint.y) maxY = sPoint.y;
    }

    out.x=minX;
    out.y=minY;
    out.width=maxX - minX;
    out.height=maxY - minY;
    return out;
}

bool getTransparentPixel(Color *pixels,int width,int height, int x , int y)
{
    if (x < 0  ) return false;
    if (y < 0 ) return false;
    if (x > width ) return false;
    if (y > height ) return false;


    Color r = pixels[y*width + x];

    return r.a!=0;

}

int CollidePixel(Color* as , Image a, int ax , int ay ,    Color* bs ,Image b, int bx , int by, int skip)
{
	/*a - bottom right co-ordinates in world space*/
	int ax1 = ax + a.width - 1;
	int ay1 = ay + a.height - 1;

	/*b - bottom right co-ordinates in world space*/
	int bx1 = bx + b.width - 1;
	int by1 = by + b.height- 1;





	/*check if bounding boxes intersect*/
	if((bx1 < ax) || (ax1 < bx))
		return 0;
	if((by1 < ay) || (ay1 < by))
		return 0;


	int xstart = COLLIDE_MAX(ax,bx);
	int xend = COLLIDE_MIN(ax1,bx1);

	int ystart = COLLIDE_MAX(ay,by);
	int yend = COLLIDE_MIN(ay1,by1);





	for(int y = ystart ; y <= yend ; y += skip)
	{
		for(int x = xstart ; x <= xend ; x += skip)
		{
			bool ca = getTransparentPixel(as, a.width,a.height,x-ax , y-ay);
			bool cb = getTransparentPixel(bs, b.width,b.height,x-bx , y-by);
			if (ca && cb) return 1;
		}
	}
	return 0;
}

void SetBlendMode(int blend)
{

    if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
	{
		if(blend & BLEND_ALPHABLEND)
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//Alpha blending
			BeginBlendMode(BLEND_ALPHA);
		else
            BeginBlendMode(BLEND_ADDITIVE);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE); //Addictive
	}

    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
	{
		if(blend & BLEND_ZWRITE) rlEnableDepthTest();
		else rlDisableDepthTest();
	}

	if((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
	{
		//if(blend & BLEND_COLORADD) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	//	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	CurBlendMode = blend;

}





void RenderQuad(const rQuad *quad)
{


        rlCheckRenderBatchLimit(4);     // Make sure there is enough free space on the batch buffer
        rlSetTexture(quad->tex.id);



        if(CurBlendMode != quad->blend) SetBlendMode(quad->blend);




            rlBegin(RL_QUADS);


                Color a = quad->v[1].col;
                Color b = quad->v[0].col;
                Color c = quad->v[3].col;
                Color d = quad->v[2].col;

                rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer




                rlColor4ub(a.r,a.g,a.b,a.a);
                rlTexCoord2f(quad->v[1].tx,quad->v[1].ty);
                rlVertex3f(quad->v[1].x,quad->v[1].y,quad->v[1].z);

                rlColor4ub(b.r,b.g,b.b,b.a);
                rlTexCoord2f(quad->v[0].tx,quad->v[0].ty);
                rlVertex3f(quad->v[0].x,quad->v[0].y,quad->v[0].z);

                 rlColor4ub(c.r,c.g,c.b,c.a);
                rlTexCoord2f(quad->v[3].tx,quad->v[3].ty);
                rlVertex3f(quad->v[3].x,quad->v[3].y,quad->v[3].z);

                rlColor4ub(d.r,d.g,d.b,d.a);
                rlTexCoord2f(quad->v[2].tx,quad->v[2].ty);
                rlVertex3f(quad->v[2].x,quad->v[2].y,quad->v[2].z);

            rlEnd();

        EndBlendMode();
        rlSetTexture(0);

}


void  RenderTransform(Texture2D texture,Matrix2D* matrix,int blend)
{

rQuad quad;
quad.tex=texture;
quad.blend=blend;


float u=0.0f;
float v=0.0f;
float u2=1.0f;
float v2=1.0f;


float TempX1 = 0;
float TempY1 = 0;
float TempX2 = texture.width;
float TempY2 = texture.height;


    quad.v[1].x = TempX1;    quad.v[1].y = TempY1;
    quad.v[1].tx = u;    quad.v[1].ty = v;

    quad.v[0].x = TempX1;    quad.v[0].y = TempY2;
    quad.v[0].tx = u;    quad.v[0].ty = v2;

    quad.v[3].x = TempX2;    quad.v[3].y = TempY2;
    quad.v[3].tx = u2;    quad.v[3].ty = v2;

    quad.v[2].x = TempX2;    quad.v[2].y = TempY1;
    quad.v[2].tx = u2;    quad.v[2].ty = v;


    for (int i=0;i<4;i++)
    {
       float x=quad.v[i].x;
       float y=quad.v[i].y;
       quad.v[i].x=matrix->a* x + matrix->c * y+ matrix->tx;
       quad.v[i].y=matrix->d* y + matrix->b * x+ matrix->ty;

    }




    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = WHITE;


   RenderQuad(&quad);





}


void  RenderTransform(Texture2D texture,Rectangle clip,bool flipX,bool flipY,Color color, Matrix2D* matrix,int blend)
{

rQuad quad;
quad.tex=texture;
quad.blend=blend;


int widthTex  = texture.width;
int heightTex = texture.height;



float u=0.0f;
float v=0.0f;
float u2=1.0f;
float v2=1.0f;

float left;
float right;
float top;
float bottom;



if (FIX_ARTIFACTS_BY_STRECHING_TEXEL)
{
   left = (2*clip.x+1) / (2*widthTex);
   right =  left +(clip.width*2-2) / (2*widthTex);
   top = (2*clip.y+1) / (2*heightTex);
   bottom = top +(clip.height * 2 - 2) / (2 * heightTex);
}else
  {
   left = clip.x / widthTex;
   right =  (clip.x + clip.width) / widthTex;
   top = clip.y / heightTex;
   bottom = (clip.y + clip.height) / heightTex;
  }

if (flipX)
{
    float tmp = left;
    left = right;
    right = tmp;
}

if (flipY)
{
    float tmp = top;
    top = bottom;
    bottom = tmp;
}



float TempX1 = 0;
float TempY1 = 0;
float TempX2 = texture.width;
float TempY2 = texture.height;


    quad.v[1].x = TempX1;    quad.v[1].y = TempY1;
    quad.v[1].tx = left;    quad.v[1].ty = top;

    quad.v[0].x = TempX1;    quad.v[0].y = TempY2;
    quad.v[0].tx = left;    quad.v[0].ty = bottom;

    quad.v[3].x = TempX2;    quad.v[3].y = TempY2;
    quad.v[3].tx = right;    quad.v[3].ty = bottom;

    quad.v[2].x = TempX2;    quad.v[2].y = TempY1;
    quad.v[2].tx = right;    quad.v[2].ty = top;


    for (int i=0;i<4;i++)
    {
       float x=quad.v[i].x;
       float y=quad.v[i].y;
       quad.v[i].x=matrix->a* x + matrix->c * y+ matrix->tx;
       quad.v[i].y=matrix->d* y + matrix->b * x+ matrix->ty;

    }




    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = color;


   RenderQuad(&quad);





}


void  RenderTransform(Texture2D texture,float width,float height,Rectangle clip,bool flipX,bool flipY,Color color, Matrix2D* matrix,int blend)
{

rQuad quad;
quad.tex=texture;
quad.blend=blend;


int widthTex  = texture.width;
int heightTex = texture.height;



float u=0.0f;
float v=0.0f;
float u2=1.0f;
float v2=1.0f;

float left;
float right;
float top;
float bottom;



if (FIX_ARTIFACTS_BY_STRECHING_TEXEL)
{
   left = (2*clip.x+1) / (2*widthTex);
   right =  left +(clip.width*2-2) / (2*widthTex);
   top = (2*clip.y+1) / (2*heightTex);
   bottom = top +(clip.height * 2 - 2) / (2 * heightTex);
}else
  {
   left = clip.x / widthTex;
   right =  (clip.x + clip.width) / widthTex;
   top = clip.y / heightTex;
   bottom = (clip.y + clip.height) / heightTex;
  }

if (flipX)
{
    float tmp = left;
    left = right;
    right = tmp;
}

if (flipY)
{
    float tmp = top;
    top = bottom;
    bottom = tmp;
}



float TempX1 = 0;
float TempY1 = 0;
float TempX2 = width;
float TempY2 = height;


    quad.v[1].x = TempX1;    quad.v[1].y = TempY1;
    quad.v[1].tx = left;    quad.v[1].ty = top;

    quad.v[0].x = TempX1;    quad.v[0].y = TempY2;
    quad.v[0].tx = left;    quad.v[0].ty = bottom;

    quad.v[3].x = TempX2;    quad.v[3].y = TempY2;
    quad.v[3].tx = right;    quad.v[3].ty = bottom;

    quad.v[2].x = TempX2;    quad.v[2].y = TempY1;
    quad.v[2].tx = right;    quad.v[2].ty = top;


    for (int i=0;i<4;i++)
    {
       float x=quad.v[i].x;
       float y=quad.v[i].y;
       quad.v[i].x=matrix->a* x + matrix->c * y+ matrix->tx;
       quad.v[i].y=matrix->d* y + matrix->b * x+ matrix->ty;

    }




    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = color;


   RenderQuad(&quad);





}
void  RenderNormal(Texture2D texture, float x, float y,int blend)
{

rQuad quad;
quad.tex=texture;
quad.blend=blend;


float u=0.0f;
float v=0.0f;
float u2=1.0f;
float v2=1.0f;

float fx2=x+ texture.width;
float fy2=y+texture.height;


    quad.v[1].x = x;    quad.v[1].y = y;
    quad.v[1].tx = u;    quad.v[1].ty = v;

    quad.v[0].x = x;    quad.v[0].y = fy2;
    quad.v[0].tx = u;    quad.v[0].ty = v2;

    quad.v[3].x = fx2;    quad.v[3].y = fy2;
    quad.v[3].tx = u2;    quad.v[3].ty = v2;

    quad.v[2].x = fx2;    quad.v[2].y = y;
    quad.v[2].tx = u2;    quad.v[2].ty = v;




    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = WHITE;


   RenderQuad(&quad);





}
void  RenderTile(Texture2D texture, float x, float y, float width, float height, Rectangle clip,bool flipx, bool flipy,int blend)
{

float fx2 = x+width;
float fy2 = y+height;
rQuad quad;
quad.tex=texture;
quad.blend=blend;


int widthTex  = texture.width;
int heightTex = texture.height;

float left,right,top,bottom;


if (FIX_ARTIFACTS_BY_STRECHING_TEXEL)
{

   left = (2*clip.x+1) / (2*widthTex);
   right =  left +(clip.width*2-2) / (2*widthTex);
   top = (2*clip.y+1) / (2*heightTex);
   bottom = top +(clip.height * 2 - 2) / (2 * heightTex);



}else
  {
   left = clip.x / widthTex;
   right =  (clip.x + clip.width) / widthTex;
   top = clip.y / heightTex;
   bottom = (clip.y + clip.height) / heightTex;
  }


if (flipx)
{
    float tmp = left;
    left = right;
    right = tmp;
}

if (flipy)
{
    float tmp = top;
    top = bottom;
    bottom = tmp;
}


    quad.v[1].tx = left;    quad.v[1].ty = top;
    quad.v[1].x = x;        quad.v[1].y = y;

    quad.v[0].x = x;    quad.v[0].y = fy2;
    quad.v[0].tx = left;    quad.v[0].ty = bottom;

    quad.v[3].x = fx2;    quad.v[3].y = fy2;
    quad.v[3].tx = right;    quad.v[3].ty = bottom;


    quad.v[2].x = fx2;    quad.v[2].y = y;
    quad.v[2].tx = right;    quad.v[2].ty = top;



    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = WHITE;


   RenderQuad(&quad);




}


Animation::Animation(Texture2D texture,int frameWidth, int frameHeight,float fps,bool useCount)
{



   tex=texture;

   Reset(fps);

   if (useCount)
   {

        float  fWidth   = tex.width  / frameWidth;
        float  fHeight  = tex.height / frameHeight;
        int columns = (int) tex.width  / fWidth ;
        int rows     =(int) tex.height / fHeight ;

        for (int y=0;y<rows;y++)
        {
            for (int x=0;x<columns;x++)
            {
            Rectangle r;
            r.x=(float)x * frameWidth;
            r.y=(float)y * frameHeight;
            r.width =fWidth;
            r.height=fHeight;
            keyFrames.push_back(r);
            }
        }

   } else
   {



   int columns  = (int) tex.width  / frameWidth ;
   int rows     =(int) tex.height / frameHeight ;

        for (int y=0;y<rows;y++)
        {
            for (int x=0;x<columns;x++)
            {

            Rectangle r;

            r.x=(float)x * frameWidth;
            r.y=(float)y * frameHeight;
            r.width =(float)frameWidth;
            r.height=(float)frameHeight;


            keyFrames.push_back(r);

            }
        }

   }

   frames_=keyFrames.size();

   TraceLog(LOG_INFO, "create %d frames .",frames_);


}



Animation::Animation( float fps)
{
  Reset(fps);
}
void Animation::Reset(float fps)
{
    since_last_frame_ = -1.0f;
    speed_ = 1.0f / fps;
    is_playing_ = false;
    mode_ = RANIM_FWD | RANIM_LOOP;
    delta_ = 1;
    SetFrame(0);

}
Animation::~Animation()
{

}

void Animation::Play()
{
    is_playing_ = true;
    since_last_frame_ = -1.0f;
    if (mode_ & RANIM_REV)
        {
        delta_ = -1;
        SetFrame(frames_ - 1);
    }
    else {
        delta_ = 1;
        SetFrame(0);
    }


}

void Animation::Update(float delta_time)
{
      if (!is_playing_)
    {
        return;
    }

    if (since_last_frame_ == -1.0f)
    {
        since_last_frame_ = 0.0f;
    }
    else {
        since_last_frame_ += delta_time;
    }

        while (since_last_frame_ >= speed_)
        {
        since_last_frame_ -= speed_;

        if (cur_frame_ + delta_ == frames_)
        {
            switch (mode_)
            {
            case RANIM_FWD:
            case RANIM_REV | RANIM_PINGPONG:
                is_playing_ = false;
                break;

            case RANIM_FWD | RANIM_PINGPONG:
            case RANIM_FWD | RANIM_PINGPONG | RANIM_LOOP:
            case RANIM_REV | RANIM_PINGPONG | RANIM_LOOP:
                delta_ = -delta_;
                break;
            }
        }
        else if (cur_frame_ + delta_ < 0)
        {
            switch (mode_) {
            case RANIM_REV:
            case RANIM_FWD | RANIM_PINGPONG:
                is_playing_ = false;
                break;

            case RANIM_REV | RANIM_PINGPONG:
            case RANIM_REV | RANIM_PINGPONG | RANIM_LOOP:
            case RANIM_FWD | RANIM_PINGPONG | RANIM_LOOP:
                delta_ = -delta_;
                break;
            }
        }

        if (is_playing_)
        {
            SetFrame(cur_frame_ + delta_);
        }
    }




}

void Animation::SetMode(int mode)
{
  mode_ = mode;

    if (mode & RANIM_REV)
    {
        delta_ = -1;
        SetFrame(frames_ - 1);
    }
    else {
        delta_ = 1;
        SetFrame(0);
    }
}

void Animation::SetFrame(int n)
{

try {

 //   n = n % frames_;

 n= fmod(n,frames_);

 if (n>=frames_)
 {
   //  n=0;
 }

} catch( std::exception& e ) {

}




    cur_frame_ = n;

  //  TraceLog(LOG_INFO, "create %d frames %d.",frames_,n);
}


GameObject::GameObject(): parent(nullptr), name("GameObject"),layer(0)
{
    this->isActive = true;

  //  TraceLog(LOG_DEBUG, "***********create GameObject.");


    transform = new Transform2D();

    if (parent)
    {
    transform->parent=parent->transform;
    }

    matrix = transform->getLocalToWorldMatrix();
    bound.x=0;
    bound.y=0;
    bound.width=1;
    bound.height=1;
    width=1;
    height=1;

}
GameObject::GameObject(std::string name,GameObject* Parent,int Layer): parent(Parent), name(name),layer(Layer)
{
    this->isActive = true;

  //  TraceLog(LOG_DEBUG, "***********create GameObject.");


    transform = new Transform2D();

    if (Parent)
    {
    transform->parent=Parent->transform;
    }

    matrix = transform->getLocalToWorldMatrix();
    bound.x=0;
    bound.y=0;
    bound.width=1;
    bound.height=1;
    width=1;
    height=1;


}

void GameObject::Update(float deltaTime)
{

    matrix = transform->getLocalToWorldMatrix();
    trasbound=getMatrixBounds(matrix,bound);


    OnUpdate(deltaTime);
    for (auto& component: components) {
        component->Update(deltaTime);
    }
}

void GameObject::Render()
{
    OnRender();

 //   DrawCircle(transform->position.x,transform->position.y,5,LIME);

 //   DrawRectangleLines(trasbound.x,trasbound.y,trasbound.width,trasbound.height,RED);

    for (auto& component: components)
    {
        component->Render();
    }
}
void GameObject::setPosition(float x,float y)
{
  transform->position.x=x;
  transform->position.y=y;
}
int GameObject::getLayer()
{
return layer;
}
void GameObject::setLayer(int value)
{
layer=value;
}
void GameObject::moveByMouse()
{
  transform->position=GetMousePosition();
}

void GameObject::setCenterPivot()
{
transform->pivot.x=bound.width/2;
transform->pivot.y=bound.height/2;
}

void GameObject::MoveTo(float x, float y,float speed,float distance,float angleDiff)
{
 stepTowards(x,y,2);
 transform->rotation = lerpAngleDegrees(transform->rotation,
getAngle(transform->position.x,transform->position.y,x,y)+angleDiff,
speed);
}

void GameObject::TurnTo(float x, float y,float speed,float angleDiff)
{
  transform->rotation = lerpAngleDegrees(transform->rotation,getAngle(transform->position.x,transform->position.y,x,y)+angleDiff,speed);
}


void GameObject::rotateAround(float anchor_x, float anchor_y, float to_angle, bool relative)
{
   if (relative) to_angle += getAngle(anchor_x, anchor_y, transform->position.x, transform->position.y);
    angleXY( to_angle, distance(anchor_x, anchor_y,transform->position.x, transform->position.y), anchor_x, anchor_y);
}

void GameObject::angleXY(float to_angle, float length, float vx, float vy)
{
    to_angle *= RAD;
    transform->position.x = cos(to_angle) * length + vx;
    transform->position.y = sin(to_angle) * length + vy;
}

void GameObject::anchorTo(float anchor_x, float anchor_y, float distance)
{
    Vector2 point;
    point.x = transform->position.x - anchor_x;
    point.y = transform->position.y - anchor_y;
    if (Vector2Length(point) > distance) point=normalize(point,distance);
    transform->position.x = anchor_x + point.x;
    transform->position.y = anchor_y + point.y;
}
void  GameObject::rotateTowards(float anchor_x,float anchor_y,float distance)
{
Vector2 point;
    point.x = anchor_x - transform->position.x;
    point.y = anchor_y - transform->position.y;
    if (Vector2Length(point) <= distance)
    {
        return;
    }

}

void GameObject::stepTowards(float anchor_x, float anchor_y, float distance)
{
    Vector2 point;
    point.x = anchor_x - transform->position.x;
    point.y = anchor_y - transform->position.y;
    if (Vector2Length(point) <= distance)
    {
        transform->position.x = anchor_x;
        transform->position.y = anchor_y;
        return;
    }
    point=normalize(point,distance);
    transform->position.x += point.x;
    transform->position.y += point.y;
}
void GameObject::advance ( float speed,float angle)
{
    transform->position.x += cos(angle *  -180 / M_PI ) * speed;
    transform->position.y += sin(angle *  -180 / M_PI ) * speed;
}
void GameObject::advance ( float speed)
{
    transform->position.x += cos(transform->rotation*  -180 / M_PI ) * speed;
    transform->position.y += sin(transform->rotation*  -180 / M_PI ) * speed;
}

void GameObject::Destroy()
{
    OnClose();
    this->isActive = false;
    delete transform;
}
Rectangle GameObject::getBounds()
{
return bound;
}
Rectangle GameObject::getTransformBounds()
{
return trasbound;
}

bool GameObject::IsActive() const
{
    return this->isActive;
}

Matrix2D* GameObject::getLocalToWorldMatrix(){ return transform->getLocalToWorldMatrix();}
Matrix2D* GameObject::getTransformationMatrix(){ return transform->getTransformationMatrix();}

void GameObject::ListAllComponents() const
{
    for (auto mapElement: componentTypeMap)
    {
       TraceLog(LOG_INFO, " Component  %s ", mapElement.first->name());
    }
}

void Group::ClearData()
{

  for (auto& obj: gameobjects)
  {
        obj->Destroy();
    }

}


Group::Group()
{
setSort=false;
}
Group::~Group()
{
}


void Group::Update(float deltaTime)
{




for (auto& obj: gameobjects)
{
obj->Update(deltaTime);
}

DestroyInactiveGameObjects();


if (setSort)
{
    sort( gameobjects.begin(), gameobjects.end(), howToSortDepth );
    setSort=false;
}

}

void Group::Render()
{
   for (auto& obj: gameobjects)
   {
        obj->Render();
   }
}

bool Group::HasNoGameObject() const
{
 return gameobjects.size() == 0;
}

unsigned int Group::GetGameObjectCount() const
{
    return gameobjects.size();
}

void Group::ListAllObjects() const
{

 unsigned int i = 0;
    for (auto& obj: gameobjects)
    {
        TraceLog(LOG_INFO, "Entity[%d]: %s",i,obj->name.c_str());
        obj->ListAllComponents();
        i++;
    }

}

void Group::remove(GameObject* t)
{
 if (!t) return;

gameobjects.erase(std::remove(gameobjects.begin(), gameobjects.end(), t), gameobjects.end());


delete t;
}

bool Group::exists(GameObject* obj)
{
    if (!obj) return false;
    if (std::find(gameobjects.begin(), gameobjects.end(), obj) != gameobjects.end())
       {
          return true;
       }
    else
    {
       return false;
    }
}

std::vector<GameObject*> Group::GetGameObjects() const
{
    return gameobjects;
}

std::vector<GameObject*> Group::GetGameObjectByName(std::string name) const
{

 std::vector<GameObject*> selectedObjects;
    for (auto& obj: gameobjects)
    {
        if (obj->name == name)
        {
            selectedObjects.emplace_back(obj);
        }
    }
    return selectedObjects;

}
std::vector<GameObject*> Group::GetGameObjectByLayer(int layer) const
{

 std::vector<GameObject*> selectedObjects;
    for (auto& obj: gameobjects)
    {
        if (obj->getLayer() == layer)
        {
            selectedObjects.emplace_back(obj);
        }
    }
    return selectedObjects;

}
GameObject* Group::AddGameObject(GameObject* obj)
{
    gameobjects.emplace_back(obj);
    obj->OnCreate();
    setSort=true;
    return obj;
}
GameObject* Group::AddGameObject(std::string Name,GameObject* Parent,int Layer)
{
    GameObject *obj = new GameObject(Name,Parent, Layer);
    gameobjects.emplace_back(obj);
    obj->OnCreate();
    setSort=true;
    return obj;
}

void Group::CheckCollisions() const
{

for (int i = 0; i < gameobjects.size() - 1; i++)
{
        auto& thisEntity = gameobjects[i];
        if (thisEntity->HasComponent<ColliderComponent>() || thisEntity->HasComponent<BoxColliderComponent>() )
        {


            ColliderComponent* thisCollider = thisEntity->GetComponent<ColliderComponent>();
            if (thisEntity->HasComponent<BoxColliderComponent>())
            {
             thisCollider = thisEntity->GetComponent<BoxColliderComponent>();

            } else
            if (thisEntity->HasComponent<SphereColliderComponent>())
            {
             thisCollider = thisEntity->GetComponent<SphereColliderComponent>();

            }
            if (!thisCollider) continue;

          //  TraceLog(LOG_INFO," %d %s",i,thisEntity->name.c_str());

            for (int j = i + 1; j < gameobjects.size(); j++)
            {
                auto& thatEntity = gameobjects[j];



                if (thisEntity->name.compare(thatEntity->name) != 0 && (thatEntity->HasComponent<ColliderComponent>() || thatEntity->HasComponent<BoxColliderComponent>()))
                {
                    ColliderComponent* thatCollider = thatEntity->GetComponent<BoxColliderComponent>();

                    if (thatEntity->HasComponent<BoxColliderComponent>())
                    {
                    thatCollider = thatEntity->GetComponent<BoxColliderComponent>();

                    } else
                    if (thatEntity->HasComponent<SphereColliderComponent>())
                    {
                    thatCollider = thatEntity->GetComponent<SphereColliderComponent>();

                    }
                    if (!thatCollider) continue;

                     if (CheckCollisionRecs(thisCollider->collider,thatCollider->collider))
                     {
                          thisEntity->OnHit(thatEntity);
                          thatEntity->OnHit(thisEntity);
                          DrawText("Colide",20,20,22,RED);
                          break;
                     }
                }
            }
        }

    }

}

void Group::DestroyInactiveGameObjects()
{
 for (int i = 0; i < gameobjects.size(); i++)
 {
        if (!gameobjects[i]->IsActive())
        {
            gameobjects.erase(gameobjects.begin() + i);
        }
  }
}




