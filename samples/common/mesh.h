#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader_s.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
	// 位置
	glm::vec3 Position;
	// 法线
	glm::vec3 Normal;
	// 位置
	glm::vec2 TexCoords;
	// 切线
	glm::vec3 Tangent;
	// 双切线
	glm::vec3 Bitangent;
};

//纹理id  反射类型 路径
struct Texture {
	unsigned int id;  
	string type;
	string path;
};

class Mesh {
public:
	/*  网格数据  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	/*  构造函数  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// 现在我们有了所有需要的数据，设置顶点缓冲区及其属性指针
		setupMesh();
	}

	// 渲染网格
	void Draw(Shader shader) 
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for(unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
			// 获取纹理序号（diffuse_textureN 中的 N）
			string number;
			string name = textures[i].type;
			if(name == "texture_diffuse")
				number = char(diffuseNr++);
			else if(name == "texture_specular")
				number = char(specularNr++);

			shader.setFloat(("material." + name + number).c_str(), i);
			//绑定纹理属于哪个纹理单元
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		// 开始绘制网格
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	/*  渲染数据  */
	unsigned int VBO, EBO;

	/*  函数   */
	// 初始化所有缓冲对象/数组
	void setupMesh()
	{
		// 创建缓冲/数组
		//顶点数组
		glGenVertexArrays(1, &VAO);
		//顶点缓冲
		glGenBuffers(1, &VBO);
		//顶点索引
		glGenBuffers(1, &EBO);
		//绑定顶点对象数组
		glBindVertexArray(VAO);
		// 加载数据到顶点缓冲区
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 与以前不同此处使用结构体中的顶点数据
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// 顶点位置
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// 
		glEnableVertexAttribArray(1);	
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// 顶点纹理位置
		glEnableVertexAttribArray(2);	
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// 顶点双法线
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif