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
	/*  模型数据 */
	//vector表示动态数组 可以添加和删除  用于不知道规模大小时候使用
	vector<Texture> textures_loaded;	// 储存所有的纹理加载到目前为止，优化以确保纹理不加载超过一次。
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/* 函数   */
	// 构造函数, 通过路径加载3D模型.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// 遍历所有网格 调用各自的draw
	void Draw(Shader shader)
	{
		for(unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	/*  函数   */
	// 从文件中加载一个支持assimp扩展的模型，并将结果网格存储在网格向量中。
	void loadModel(string const &path)
	{
		// 导入  加载模型到scene数据结构中
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);//许多后期处理命令
		// 检查导入数据是否完整 如果不完整报告错误
		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 检查是否完整
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// 通过最后的‘/’字符获取文件路径
		directory = path.substr(0, path.find_last_of('/'));

		// 递归根节点处理场景中的所有节点
		processNode(scene->mRootNode, scene);
	}

	// 以递归方式处理节点。处理位于节点上的每个单独网格，并在其子节点（如果有的话）上重复此过程.
	void processNode(aiNode *node, const aiScene *scene)
	{
		// 处理位于当前节点的每个网格
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// 结点只包含索引数据  
			//场景包含所有数据
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// 递归访问
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	//将数据解析到mesh类
	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// 填写顶点数据
		vector<Vertex> vertices;
		//索引
		vector<unsigned int> indices;
		//纹理
		vector<Texture> textures;

		// 走过每一个网格的顶点
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//处理顶点数据
			glm::vec3 vector; 
			// 顶点位置
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// 法线向量
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// 纹理坐标
			if(mesh->mTextureCoords[0]) // 网络是否有纹理坐标?
			{
				glm::vec2 vec;
				//使用顶点可以有多个纹理坐标的模型，所以我们总是取第一个集合（0）。
				vec.x = mesh->mTextureCoords[0][i].x; 
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// 切线数据
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// 双切线数据
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		//走过每一个网格的表面获得索引数据 
		for(unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// 获取每一个索引并储存数据
			for(unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// 处理材料
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
		//名称约定

		// 1. 漫反射映射
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. 镜面反射映射
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. 法线映射
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. 高度映射
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// 返回一个从网格数据形成的对象
		return Mesh(vertices, indices, textures);
	}

	// 检查给定类型的所有材料纹理，如果尚未加载，则加载纹理。
	// 要求返回一个纹理的结构体.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			//循环检查各个纹理是否被加载
			bool skip = false;
			for(unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // 已经加载 下一个 
					break;
				}
			}
			if(!skip)
			{   // 如果纹理没有被加载  那就加载他
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // 存储它作为整个模型的纹理加载，以确保我们不会不需要加载重复的纹理。
			}
		}
		return textures;
	}
};

   //文件方式加载纹理  
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
		//生成纹理绑定
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//为当前绑定的纹理自动生成所有需要的多级渐远纹理
		glGenerateMipmap(GL_TEXTURE_2D);
		//设置环绕过滤方式
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