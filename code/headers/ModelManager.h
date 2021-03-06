#pragma once
#include <string>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


typedef glm::vec3 Vector3f;
typedef glm::vec2 Vector2f;

class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);

    bool Load();

    void Bind(GLenum TextureUnit);

private:

	bool LoadCubeMap();

    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
};

struct Vertex
{
    Vector3f m_pos;
    Vector2f m_tex;
    Vector3f m_normal;
	Vector3f m_tangent;
	Vector3f m_bitangent;

    Vertex() {}

	Vertex(const Vector3f& pos, const Vector2f& tex, const Vector3f& normal)
	{
		m_pos = pos;
		m_tex = tex;
		m_normal = normal;
		m_tangent = Vector3f(0);
		m_bitangent = Vector3f(0);
	}

    Vertex(const Vector3f& pos, const Vector2f& tex, const Vector3f& normal, const Vector3f& tangent, const Vector3f& bitangent)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = normal;
		m_tangent = tangent;
		m_bitangent = bitangent;
    }
};

class Mesh
{
public:
    Mesh();
	Mesh(const std::string& Filename);

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void Render();

//private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
            const std::vector<unsigned int>& Indices);

        GLuint VB;
        GLuint IB;
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<Texture*> m_Textures;
    std::vector<Texture*> m_Textures_Specular;
	std::vector<Texture*> m_Textures_Normal;
};

