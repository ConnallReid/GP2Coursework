#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "Common.h"
#include "Vertices.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void update();

	void addChild(shared_ptr<GameObject> child);

	void createBuffers(Vertex * pVerts, int numVerts, int *pIndices, int numIndices);
	void loadShader(const string& vsFilename, const string& fsFilename);

	void setPosition(const vec3& position)
	{
		m_Position = position;
	};

	void setRotation(const vec3& rotation)
	{
		m_Rotation = rotation;
	};

	void setScale(const vec3& scale)
	{
		m_Scale = scale;
	};

	int getNumberOfChildren()
	{
		return m_ChildGameObjects.size();
	};

	GameObject * getParent()
	{
		return m_ParentGameObject;
	};

	shared_ptr<GameObject> getChild(int i)
	{
		return m_ChildGameObjects.at(i);
	};

	GLuint getVertexArrayObject()
	{
		return m_VAO;
	};

	GLuint getShaderProgram()
	{
		return m_ShaderProgram;
	};

	mat4& getModelMatrix()
	{
		return m_ModelMatrix;
	};

	vec4& getAmbientMaterial()
	{
		return m_AmbientMaterial;
	};

	vec4& getDiffuseMaterial()
	{
		return m_DiffuseMaterial;
	};

	vec4& getSpecularMaterial()
	{
		return m_SpecularMaterial;
	};

	float getSpecularPower()
	{
		return m_SpecularPower;
	};

	int getNumberOfIndices()
	{
		return m_NoOfIndices;
	};

	int getNumberOfVetices()
	{
		return m_NoOfVertices;
	};
private:
	GLuint m_VBO;
	GLuint m_EBO;
	GLuint m_VAO;
	GLuint m_ShaderProgram;
	int m_NoOfIndices;
	int m_NoOfVertices;

	mat4 m_ModelMatrix;
	vec3 m_Position;
	vec3 m_Rotation;
	vec3 m_Scale;

	vec4 m_AmbientMaterial;
	vec4 m_DiffuseMaterial;
	vec4 m_SpecularMaterial;
	float m_SpecularPower;

	vector<shared_ptr<GameObject> > m_ChildGameObjects;
	GameObject * m_ParentGameObject;
};

#endif