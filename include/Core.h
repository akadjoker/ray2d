#ifndef CORE_H
#define CORE_H
#include "Global.h"





class Matrix2D
{
  public:

    Matrix2D()
  {
		this->a = 1;
		this->b = 0;
		this->c = 0;
		this->d = 1;
		this->tx = 0;
		this->ty = 0;
  }

  Matrix2D(float a,float b, float c,float d,float tx,float ty)
  {
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
		this->tx = tx;
		this->ty = ty;
  }


  virtual ~Matrix2D()
  {
  }

   Matrix2D* clone()
   {
		return new Matrix2D (a, b, c, d, tx, ty);

   }


	void concat (Matrix2D& m)
	{

		float a1 = a * m.a + b * m.c;
		b = a * m.b + b * m.d;
		a = a1;

		float c1 = c * m.a + d * m.c;
		d = c * m.b + d * m.d;

		c = c1;

		float tx1 = tx * m.a + ty * m.c + m.tx;
		ty = tx * m.b + ty * m.d + m.ty;
		tx = tx1;

	}


	Vector2 deltaTransformPoint (Vector2 point)
	{
		Vector2 v;
		v.x=point.x * a + point.y * c;
		v.y=point.x * b + point.y * d;
		return v;
	}

    Vector2 TransformPoint (Vector2 point)
	{
		Vector2 v;
		v.x=point.x * a + point.y * c+ tx;
		v.y=point.x * b + point.y * d+ ty;
		return v;
	}

    Vector2 transformCoords (Vector2 point)
	{
		Vector2 v;

            v.x = a * point.x + c * point.y + tx;
            v.y = d * point.y + b * point.x + ty;

		return v;
	}



	void identity ()
	{

		a = 1;
		b = 0;
		c = 0;
		d = 1;
		tx = 0;
		ty = 0;

	}
	 Matrix2D* mult(const Matrix2D* m)
	 {

		Matrix2D* result = new Matrix2D ();

		result->a = a * m->a + b * m->c;
		result->b = a * m->b + b * m->d;
		result->c = c * m->a + d * m->c;
		result->d = c * m->b + d * m->d;

		result->tx = tx * m->a + ty * m->c + m->tx;
		result->ty = tx * m->b + ty * m->d + m->ty;

		return result;

	}

	void rotate (float angle)
	 {

		float  acos = cos (angle);
		float  asin = sin (angle);

		float a1 = a * acos - b * asin;
		b = a * asin + b * acos;
		a = a1;

		float c1 = c * acos - d * asin;
		d = c * asin + d * acos;
		c = c1;

		float tx1 = tx * acos - ty * asin;
		ty = tx * asin + ty * acos;
		tx = tx1;

	}

	void scale (float x, float y)
	{
		a *= x;
		b *= y;

		c *= x;
		d *= y;

		tx *= x;
		ty *= y;

	}

	void translate (float x, float y)
	{
		tx += x;
		ty += y;

	}

	void  setTo (float a,float b, float c,float d,float tx,float ty)
  {
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
		this->tx = tx;
		this->ty = ty;
  }

 void  skew(float  skewX,float  skewY)
        {
            float sinX =sin(skewX);
            float cosX =cos(skewX);
            float sinY =sin(skewY);
            float cosY =cos(skewY);

            setTo(       a  * cosY - b  * sinX,
                         a  * sinY + b  * cosX,
                         c  * cosY - d  * sinX,
                         c  * sinY + d  * cosX,
                         tx * cosY - ty * sinX,
                         tx * sinY + ty * cosX);
        }



  public:
  float a;
  float b;
  float c;
  float d;
  float tx;
  float ty;


};

float  normalizeAngle(float angle);
void  RenderQuad(const rQuad *quad);
void  RenderTransform(Texture2D texture,Matrix2D* matrix,int blend=BLEND_DEFAULT);
void  RenderTransform(Texture2D texture,Rectangle clip,bool flipX,bool flipY,Color color, Matrix2D* matrix,int blend=BLEND_DEFAULT);
void  RenderTransform(Texture2D texture,float width,float height,Rectangle clip,bool flipX,bool flipY,Color color, Matrix2D* matrix,int blend=BLEND_DEFAULT);
void  RenderNormal(Texture2D texture, float x, float y,int blend=BLEND_DEFAULT);
void  RenderTile(Texture2D texture, float x, float y, float width, float height, Rectangle clip,bool flipx, bool flipy,int blend=BLEND_DEFAULT);
Vector2 vec2(float x,float y);
Vector2 vec2Zero();
Rectangle getMatrixBounds(Matrix2D* matrix,Rectangle rectangle);
int  sign(float value);
Vector2 normalize(Vector2 v , float thickness);
float distance(float x1,float  y1,float  x2 = 0,float y2 = 0);
float  distanceRects(float x1, float y1,float w1,float h1,float x2,float y2,float w2,float h2);
float  distanceRectPoint(float px, float py, float rx, float ry, float rw,float rh);
float  getAngle(float x1 , float y1, float x2, float y2);
float lerpAngleDegrees(float a, float b, float lerpFactor) ;
float lerpAngleRadians(float a, float b, float lerpFactor) ;

class Transform2D
{
public:
 Transform2D(Transform2D* Parent=NULL)
 {
   mTransformationMatrix=new Matrix2D();
   mOrientationChanged=true;
   parent=Parent;
   position=vec2(0,0);
   pivot=vec2(0,0);
   scale=vec2(1,1);
   skew=vec2(0,0);
   rotation=0;
 }
 virtual ~Transform2D()
 {
   delete mTransformationMatrix;
 }


 	Matrix2D* getTransformationMatrix()
	{



	    if (mOrientationChanged)
		{
			mOrientationChanged = false;

			if (skew.x == 0.0f && skew.y == 0.0f)
			{

				if (rotation == 0.0)
				{
					mTransformationMatrix->setTo(scale.x, 0.0, 0.0, scale.y, position.x - pivot.x * scale.x, position.y - pivot.y * scale.y);
				}
				else
				{
					float acos = cos(rotation*RAD);
					float asin = sin(rotation*RAD);
					float a   = scale.x *  acos;
					float b   = scale.x *  asin;
					float c   = scale.y * -asin;
					float d   = scale.y *  acos;
					float tx  = position.x - pivot.x * a - pivot.y * c;
					float ty  = position.y - pivot.x * b - pivot.y * d;

					mTransformationMatrix->setTo(a, b, c, d, tx, ty);
				}
			}
			else
			{
				mTransformationMatrix->identity();
				mTransformationMatrix->scale(scale.x,scale.y);
				mTransformationMatrix->skew(skew.x,skew.y);
				mTransformationMatrix->rotate(rotation);
				mTransformationMatrix->translate(position.x,position.y);

				if (pivot.x != 0.0f || pivot.y != 0.0f)
				{

					mTransformationMatrix->tx = position.x - mTransformationMatrix->a * pivot.x - mTransformationMatrix->c * pivot.y;
					mTransformationMatrix->ty = position.y - mTransformationMatrix->b * pivot.x - mTransformationMatrix->d * pivot.y;
				}
			}
		}

		mOrientationChanged=true;


		return mTransformationMatrix;

	}
	Matrix2D* getLocalToWorldMatrix()
    {
            if (parent == NULL)
            {
            return  getTransformationMatrix();
            }
           	else
            {
			return  getTransformationMatrix()->mult(parent->getTransformationMatrix());
            }
    }

public:
Vector2 position;
Vector2 pivot;
Vector2 scale;
Vector2 skew;
float rotation;
Transform2D* parent;


protected:

private:
Matrix2D* mTransformationMatrix;
bool mOrientationChanged;


};

class Component;

class GameObject
{
    private:

        std::vector<Component*> components;
        std::map<const std::type_info*, Component*> componentTypeMap;
    protected:
        bool isActive;
        int layer;
        Matrix2D* matrix;
        Rectangle bound;
        Rectangle trasbound;
    public:
        std::string name;
        GameObject* parent;
        int width;
        int height;
        Transform2D* transform;


        GameObject();
        GameObject(std::string name,GameObject* parent=NULL,int Layer=0);

        int getLayer();
        void setLayer(int value);

        Rectangle getBounds();
        Rectangle getTransformBounds();

        void setBound(float x, float y, float w, float h) {width=(int)w;height=(int)h;bound.x=x; bound.y=y;bound.width=w;bound.height=h;};
        void setBound(float w, float h) {width=(int)w;height=(int)h;bound.x=0; bound.y=0;bound.width=w;bound.height=h;};

        void setPosition(float x,float y);

        void setCenterPivot();

        void moveByMouse();
        void MoveTo(float x, float y,float speed=1.0f,float distance=1.0f,float angleDiff=0);
        void TurnTo(float x, float y,float speed=1.0f,float angleDiff=0);
        void rotateAround(float anchor_x,float anchor_y,float angle = 0, bool relative = true);
        void angleXY(float  angle,float length,float  x=1,float  y=1);
        void anchorTo(float anchor_x,float anchor_y,float distance=0);
        void stepTowards(float anchor_x,float anchor_y,float distance=0);
        void rotateTowards(float anchor_x,float anchor_y,float distance=0);

        void advance ( float speed) ;
        void advance ( float speed,float angle) ;


        void Update(float deltaTime);
        void Render();
        void Destroy();
        bool IsActive() const;
        void ListAllComponents() const;

        Matrix2D* getLocalToWorldMatrix();
        Matrix2D* getTransformationMatrix();
        Matrix2D* getLocalMatrix() {return matrix;};


        template <typename T, typename... TArgs>
        T& AddComponent(TArgs&&... args)
        {
            T* newComponent(new T(std::forward<TArgs>(args)...));
            newComponent->owner = this;
            components.emplace_back(newComponent);
            componentTypeMap[&typeid(*newComponent)] = newComponent;
            newComponent->Initialize();
            return *newComponent;
        }

        template <typename T>
        bool HasComponent() const
        {
            return componentTypeMap.count(&typeid(T));
        }

        template <typename T>
        T* GetComponent() {
            return static_cast<T*>(componentTypeMap[&typeid(T)]);
        }
public:
 virtual void OnRender()
 { }
 virtual void OnCreate()
 {TraceLog(LOG_WARNING, "***********on create GameObject."); }
 virtual void OnClose()
 { }
 virtual void OnUpdate(float dt)
 { }
 virtual void OnHit(GameObject* git)
 { }




};


class Component
 {
    public:

        GameObject* owner;
        virtual ~Component() {}
        virtual void Initialize() {priority=0;}
        virtual void Update(float deltaTime) {}
        virtual void Render() {}
    protected:
    int priority;
};


class SpriteComponent: public Component
{
public:
   SpriteComponent(Texture2D tex):texture(tex)
   {

    width =tex.width;
    heigth=tex.height;
    clip.x=0;
    clip.y=0;
    clip.width =width;
    clip.height=heigth;
    FlipX=false;
    FlipY=false;
    color=WHITE;


   }
    SpriteComponent(Texture2D tex,float x, float y, float w,float h):texture(tex)
   {
    width =tex.width;
    heigth=tex.height;
    clip.x=x;
    clip.y=y;
    clip.width =w;
    clip.height=h;
    FlipX=false;
    FlipY=false;
    color=WHITE;

   }
    void Initialize() override
    {
     owner->setBound(width,heigth);



    }

   void Render() override
   {

            //TraceLog(LOG_INFO," %s ",owner->name.c_str());
           Matrix2D * mat=owner->getLocalMatrix();

           RenderTransform(texture,width,heigth,clip,FlipX,FlipY,color,mat);
          // DrawTexture(texture,owner->transform->position.x,owner->transform->position.y,color);


   }
public:
int width;
int heigth;
bool FlipX;
bool FlipY;
Rectangle clip;
Color color;

private:
  Texture2D texture;
};

enum SHAPETYPE {SHAPEBOX,SHAPESPHERE};

class SphereColliderComponent;
class BoxColliderComponent;

class ColliderComponent: public Component
{
public :
SHAPETYPE ColideType;
Rectangle collider;

ColliderComponent()
{
  collider.x=0;
  collider.y=0;
  collider.width=1;
  collider.height=1;
}




};
class SphereColliderComponent: public ColliderComponent
{
public:
   SphereColliderComponent(float Radius):radius(Radius)
   {
     ColideType=SHAPESPHERE;
   }



   void Render() override
   {

   }
private:
float radius;

};
class BoxColliderComponent: public ColliderComponent
{
public:

   BoxColliderComponent()
   {
   ColideType=SHAPEBOX;
   }

   void Render() override
   {
     // DrawRectangleLines(collider.x,collider.y,collider.width,collider.height,MAGENTA);

   }
   void Update(float deltaTime) override
   {

            Rectangle bound =owner->getTransformBounds();
            collider.x = bound.x;
            collider.y = bound.y;
            collider.width = bound.width;
            collider.height = bound.height;

    }
private:

};

class Group
{
    private:
        std::vector<GameObject*> gameobjects;
        bool setSort;

    public:
        Group();
        virtual ~Group();


        void ClearData();
        void Update(float deltaTime);
        void Render();


        bool HasNoGameObject() const;
        unsigned int GetGameObjectCount() const;
        void ListAllObjects() const;

        bool exists(GameObject* obj);
        void remove(GameObject* obj);

        std::vector<GameObject*> GetGameObjects() const;
        std::vector<GameObject*> GetGameObjectByName(std::string  name) const;
        std::vector<GameObject*> GetGameObjectByLayer(int layer) const;


        GameObject* AddGameObject(std::string Name,GameObject* Parent=NULL, int Layer=0);
        GameObject* AddGameObject(GameObject* go);

        void CheckCollisions() const;
        void DestroyInactiveGameObjects();

protected:
static bool howToSort(int i, int j)
{ return i < j; }

 static bool howToSortDepth(GameObject* i, GameObject* j)
 {
     return i->getLayer() < j->getLayer();
 }
};

class  Animation
{
public:

    Animation( float FPS);
    Animation(Texture2D texture,int frameWidth, int frameHeight,float FPS,bool useCount);


    ~Animation();

    void Play();

    void Reset(float fps);

    void Stop()
    {
        is_playing_ = false;
    }

    void Resume()
    {
        is_playing_ = true;
    }

    void Update(float fDeltaTime);

    bool IsPlaying() const
    {
        return is_playing_;
    }


    void SetMode(int mode);

    void SetSpeed(const float fps)
    {
        speed_ = 1.0f / fps;
    }

    void SetFrame(int n);

    void SetFrames(const int n)
   {
        frames_ = n;
    }

    int GetMode() const
    {
        return mode_;
    }

    float GetSpeed() const
    {
        return 1.0f / speed_;
    }

    int GetFrame() const
    {
        return cur_frame_;
    }

    Rectangle getClip()
    {
        if (keyFrames.empty())
        {
            return {0,0,1,1};
        }
        if (cur_frame_<=keyFrames.size())
        {
          return  keyFrames[cur_frame_] ;
        }


    }
     Rectangle getClip(int index)
    {
        if (keyFrames.empty())
        {
            return {0,0,1,1};
        }
        if (index<=keyFrames.size())
        {
          return  keyFrames[index] ;
        }


    }

    int GetFrames() const
    {
        return frames_;
    }
private:
    std::vector<Rectangle> keyFrames;
    bool is_playing_;
    float speed_;
    float since_last_frame_;
    int mode_;
    int delta_;
    int frames_;
    int cur_frame_;
    Texture2D tex;
};





#endif // CORE_H
