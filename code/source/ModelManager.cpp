#include "code/headers/ModelManager.h"
#include <FreeImage.h>

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName = FileName;
}

bool Texture::Load()
{
    int width, height;

    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(m_fileName.c_str(), 0);
    assert(formato != FIF_UNKNOWN);
    FIBITMAP* imagen = FreeImage_Load(formato, m_fileName.c_str());
	
    assert(imagen != NULL);
    FIBITMAP* temp = FreeImage_ConvertTo32Bits(imagen);
    assert(imagen != NULL);
    
    width = FreeImage_GetWidth(imagen);
    height = FreeImage_GetHeight(imagen);

    

    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
    glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(temp));
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(m_textureTarget, 0);

    FreeImage_Unload(imagen);
    FreeImage_Unload(temp);

    return true;
}

void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}


Mesh::MeshEntry::MeshEntry()
{
    VB = NULL;
    IB = NULL;
    NumIndices = 0;
    MaterialIndex = INVALID_MATERIAL;
};

Mesh::MeshEntry::~MeshEntry()
{
    if (VB != NULL)
    {
        glDeleteBuffers(1, &VB);
    }

    if (IB != NULL)
    {
        glDeleteBuffers(1, &IB);
    }
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
    const std::vector<unsigned int>& Indices)
{
    NumIndices = (unsigned int)Indices.size();

    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

Mesh::Mesh()
{
    m_Entries.resize(1);
    m_Entries[0] = Mesh::MeshEntry();
    m_Entries[0].MaterialIndex = 0;
    m_Textures.resize(1);
    m_Textures[0] = new Texture(GL_TEXTURE_2D, "Content/Images/white.png");
    m_Textures_Specular.resize(1);
    m_Textures_Specular[0] = new Texture(GL_TEXTURE_2D, "Content/Images/white.png");
	bool success = m_Textures[0]->Load();
	assert(success);
	success = m_Textures_Specular[0]->Load();
    assert(success);
}


Mesh::~Mesh()
{
    Clear();
}

void Mesh::Clear()
{
    for (unsigned int i = 0; i < m_Textures.size(); i++) {
        delete(m_Textures[i]);
    }
    for (unsigned int i = 0; i < m_Textures_Specular.size(); i++) {
        delete(m_Textures_Specular[i]);
    }
}

bool Mesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    bool Ret = false;
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(Filename.c_str(), 
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), importer.GetErrorString());
    }

    return Ret;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
    m_Textures_Specular.resize(pScene->mNumMaterials);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename);
}
void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
       
        Vertex v(Vector3f(pPos->x, pPos->y, pPos->z),
            Vector2f(pTexCoord->x, pTexCoord->y),
            Vector3f(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }
    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}
bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;
        m_Textures_Specular[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
        {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }
        if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures_Specular[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures_Specular[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures_Specular[i];
                    m_Textures_Specular[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }

        // Load a white texture in case the model does not include its own texture
        if (!m_Textures[i]) {
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "Content/Images/white.png");

            Ret = m_Textures[i]->Load();
        }
        if (!m_Textures_Specular[i]) {
            m_Textures_Specular[i] = new Texture(GL_TEXTURE_2D, "Content/Images/white.png");

            Ret = m_Textures_Specular[i]->Load();
        }
    }

    return Ret;
}

void Mesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);
        GLuint err = glGetError();
        if (GLEW_OK != err)
        {
            printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
        }
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }
        if (MaterialIndex < m_Textures_Specular.size() && m_Textures_Specular[MaterialIndex]) {
            m_Textures_Specular[MaterialIndex]->Bind(GL_TEXTURE1);
        }
        err = glGetError();
        if (GLEW_OK != err)
        {
            printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
        }



        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0); 
        err = glGetError();
        if (GLEW_OK != err)
        {
            printf("||%s||", gluErrorString(err));// glewGetErrorString(err));
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}