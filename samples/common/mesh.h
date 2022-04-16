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
	// λ��
	glm::vec3 Position;
	// ����
	glm::vec3 Normal;
	// λ��
	glm::vec2 TexCoords;
	// ����
	glm::vec3 Tangent;
	// ˫����
	glm::vec3 Bitangent;
};

//����id  �������� ·��
struct Texture {
	unsigned int id;  
	string type;
	string path;
};

class Mesh {
public:
	/*  ��������  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	/*  ���캯��  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// ������������������Ҫ�����ݣ����ö��㻺������������ָ��
		setupMesh();
	}

	// ��Ⱦ����
	void Draw(Shader shader) 
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for(unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // �ڰ�֮ǰ������Ӧ������Ԫ
			// ��ȡ������ţ�diffuse_textureN �е� N��
			string number;
			string name = textures[i].type;
			if(name == "texture_diffuse")
				number = char(diffuseNr++);
			else if(name == "texture_specular")
				number = char(specularNr++);

			shader.setFloat(("material." + name + number).c_str(), i);
			//�����������ĸ�����Ԫ
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		// ��ʼ��������
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	/*  ��Ⱦ����  */
	unsigned int VBO, EBO;

	/*  ����   */
	// ��ʼ�����л������/����
	void setupMesh()
	{
		// ��������/����
		//��������
		glGenVertexArrays(1, &VAO);
		//���㻺��
		glGenBuffers(1, &VBO);
		//��������
		glGenBuffers(1, &EBO);
		//�󶨶����������
		glBindVertexArray(VAO);
		// �������ݵ����㻺����
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// ����ǰ��ͬ�˴�ʹ�ýṹ���еĶ�������
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// ����λ��
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// 
		glEnableVertexAttribArray(1);	
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// ��������λ��
		glEnableVertexAttribArray(2);	
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// ����˫����
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif