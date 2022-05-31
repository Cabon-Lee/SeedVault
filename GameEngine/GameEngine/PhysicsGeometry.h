#pragma once


// PhysX에 있는 타입들
enum GeometryType 
{
   Sphere,
   Box,
   Capsule,

   //Plane,
   //ConvexMesh,
   //TriangleMesh,
   //HeightField,

   GeometryCount,	//!< internal use only!
   Invalid = -1		//!< internal use only!
};

//////////////////////////////////////////////////////////////////////////

class GeometryBase
{
public:
   GeometryType GetType() const { return m_Type; }
   virtual ~GeometryBase() {}

protected:
   GeometryBase(GeometryType(type)) : m_Type(type) {}

   GeometryType m_Type;
};

//////////////////////////////////////////////////////////////////////////

class SphereGeometry : public GeometryBase
{
public:
   SphereGeometry() : GeometryBase(GeometryType::Sphere), m_Radius(1.0f) {}
   SphereGeometry(float radius) : GeometryBase(GeometryType::Sphere), m_Radius(radius) {}
   virtual ~SphereGeometry() {}

   float GetSphereRadius() { return m_Radius; }

private:
   float m_Radius;
};

//////////////////////////////////////////////////////////////////////////

class BoxGeometry : public GeometryBase
{
public:
   BoxGeometry() : GeometryBase(GeometryType::Box), m_X(1.0f), m_Y(1.0f), m_Z(1.0f) {}
   BoxGeometry(float x, float y, float z) : GeometryBase(GeometryType::Box), m_X(x), m_Y(y), m_Z(z) {}
   virtual ~BoxGeometry() {}

   DirectX::SimpleMath::Vector3 GetBoxSize() { return DirectX::SimpleMath::Vector3(m_X, m_Y, m_Z); }
   void SetBoxSize(const DirectX::SimpleMath::Vector3 scale);

private:
   float m_X;
   float m_Y;
   float m_Z;
};


//////////////////////////////////////////////////////////////////////////

class CapsuleGeometry : public GeometryBase
{
public:
   CapsuleGeometry() : GeometryBase(GeometryType::Capsule), m_Radius(1.0f), m_Height(1.0f) {}
   CapsuleGeometry(float radius, float height) : GeometryBase(GeometryType::Capsule), m_Radius(radius), m_Height(height) {}
   virtual ~CapsuleGeometry() {}

   float GetCapsuleHeight() { return m_Height; }
   float GetCapsuleRadius() { return m_Radius; }

private:
   float m_Radius;
   float m_Height;
};

//////////////////////////////////////////////////////////////////////////

/*
class PlaneGeometry : public GeometryBase
{
public:
   PlaneGeometry() : GeometryBase(GeometryType::Plane), m_Radius(1.0f) {}
   PlaneGeometry(float radius) : GeometryBase(GeometryType::Plane), m_Radius(radius) {}

   float m_Radius;
};

//////////////////////////////////////////////////////////////////////////

class ConvexMeshGeometry : public GeometryBase
{
public:
   ConvexMeshGeometry() : GeometryBase(GeometryType::eCONVEXMESH), m_Radius(1.0f) {}
   ConvexMeshGeometry(float radius) : GeometryBase(GeometryType::eCONVEXMESH), m_Radius(radius) {}

   float m_Radius;
};

//////////////////////////////////////////////////////////////////////////

class TriangleGeometry : public GeometryBase
{
public:
   TriangleGeometry() : GeometryBase(GeometryType::eTRIANGLEMESH), m_Radius(1.0f) {}
   TriangleGeometry(float radius) : GeometryBase(GeometryType::eTRIANGLEMESH), m_Radius(radius) {}

   float m_Radius;
};

//////////////////////////////////////////////////////////////////////////

class HeightFieldGeometry : public GeometryBase
{
public:
   HeightFieldGeometry() : GeometryBase(GeometryType::eHEIGHTFIELD), m_Radius(1.0f) {}
   HeightFieldGeometry(float radius) : GeometryBase(GeometryType::eHEIGHTFIELD), m_Radius(radius) {}

   float m_Radius;
};
*/
