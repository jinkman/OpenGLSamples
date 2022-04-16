#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader_s.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model 
{
public:
	/*  ģ������ */
	//vector��ʾ��̬���� ������Ӻ�ɾ��  ���ڲ�֪����ģ��Сʱ��ʹ��
	vector<Texture> textures_loaded;	// �������е�������ص�ĿǰΪֹ���Ż���ȷ���������س���һ�Ρ�
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/* ����   */
	// ���캯��, ͨ��·������3Dģ��.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// ������������ ���ø��Ե�draw
	void Draw(Shader shader)
	{
		for(unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	/*  ����   */
	// ���ļ��м���һ��֧��assimp��չ��ģ�ͣ������������洢�����������С�
	void loadModel(string const &path)
	{
		// ����  ����ģ�͵�scene���ݽṹ��
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);//�����ڴ�������
		// ��鵼�������Ƿ����� ����������������
		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // ����Ƿ�����
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// ͨ�����ġ�/���ַ���ȡ�ļ�·��
		directory = path.substr(0, path.find_last_of('/'));

		// �ݹ���ڵ㴦�����е����нڵ�
		processNode(scene->mRootNode, scene);
	}

	// �Եݹ鷽ʽ����ڵ㡣����λ�ڽڵ��ϵ�ÿ���������񣬲������ӽڵ㣨����еĻ������ظ��˹���.
	void processNode(aiNode *node, const aiScene *scene)
	{
		// ����λ�ڵ�ǰ�ڵ��ÿ������
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// ���ֻ������������  
			//����������������
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// �ݹ����
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	//�����ݽ�����mesh��
	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// ��д��������
		vector<Vertex> vertices;
		//����
		vector<unsigned int> indices;
		//����
		vector<Texture> textures;

		// �߹�ÿһ������Ķ���
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//����������
			glm::vec3 vector; 
			// ����λ��
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// ��������
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// ��������
			if(mesh->mTextureCoords[0]) // �����Ƿ�����������?
			{
				glm::vec2 vec;
				//ʹ�ö�������ж�����������ģ�ͣ�������������ȡ��һ�����ϣ�0����
				vec.x = mesh->mTextureCoords[0][i].x; 
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// ��������
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// ˫��������
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		//�߹�ÿһ������ı������������� 
		for(unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// ��ȡÿһ����������������
			for(unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// �������
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
		//����Լ��

		// 1. ������ӳ��
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. ���淴��ӳ��
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. ����ӳ��
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. �߶�ӳ��
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// ����һ�������������γɵĶ���
		return Mesh(vertices, indices, textures);
	}

	// ���������͵����в������������δ���أ����������
	// Ҫ�󷵻�һ������Ľṹ��.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			//ѭ�������������Ƿ񱻼���
			bool skip = false;
			for(unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // �Ѿ����� ��һ�� 
					break;
				}
			}
			if(!skip)
			{   // �������û�б�����  �Ǿͼ�����
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // �洢����Ϊ����ģ�͵�������أ���ȷ�����ǲ��᲻��Ҫ�����ظ�������
			}
		}
		return textures;
	}
};

   //�ļ���ʽ��������  
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		//���������
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
		//���û��ƹ��˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
#endif